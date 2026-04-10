function accelSRP = ComputeSPADAccel(t,X,Force,Sat)

mode = 2;  % mode 1 means use sun to sat in file interploation
           % mode 2 means use sat 2 sun 
showDebug = 0;

%%  Set constants
d2r            = pi/180;

%  Compute vector from sun to spacecraft
a1Epoch             = Force.RefEpoch + t/86400 + 2430000;
utcEpoch            = a1Epoch - 0.0343817/86400 - Force.deltaAT/86400;
[T_TT,JD_UT1,JD_TT] = ConvertJD_UTC(utcEpoch,Force.deltaAT,Force.deltaUT1);
ttt                 = (JD_TT - 2451545) / 36525;
mE                  = mod(357.5277233 + 35999.05034 * ttt, 360);
JD_TDB              = JD_TT + (0.001658 * sind(mE) + ...
                      0.00001385 * sind(2*mE)) / (86400);
[cbSunVector]       = pleph(JD_TDB,11,3,1);
sunToSpacecraft_I   = X(1:3,1) - cbSunVector;

%  Rotation matrix from inertial to body 
if Sat.Attitude == 1
    R_BI = [1 0 0;0 1 0; 0 0 1];
elseif Sat.Attitude == 2;
    rv = X(1:3);
    vv = X(4:6);
    xhat = vv/norm(vv);
    yhat = cross(rv,vv); 
    yhat = yhat/norm(yhat);
    zhat = cross(xhat,yhat);
    R_BI = [xhat yhat zhat]';
elseif Sat.Attitude == 3
    R_BI = (R1(20.984145832262929*d2r)*R2(7.6282996845926885*d2r)*R3(8.1823681007212254*d2r));
elseif Sat.Attitude == 4
    a1ModJul = Force.RefEpoch + t/86400;
    if a1ModJul > 28553.00003512016 && a1ModJul < 28665.00003512016 
        R_BI = (R1(230*d2r)*R2(120*d2r)*R3(60*d2r));
    elseif a1ModJul > 28665.00003512016
        R_BI = (R1(30*d2r)*R2(20*d2r)*R3(10*d2r));
    else
        keyboard
    end
end

percentSun = GetPercentSun(X(1:3),cbSunVector);

%  Sun unit vector data in body coordinates
if mode ==2
    signDir = -1;
end
spacecraftToSun_B = R_BI*(-sunToSpacecraft_I/norm(sunToSpacecraft_I));
xSun     = spacecraftToSun_B(1);
ySun     = spacecraftToSun_B(2);
zSun     = spacecraftToSun_B(3);
signDir = 1;
aziSun   = atan2(ySun,xSun)/d2r;
elevSun  = atan2(zSun,sqrt(xSun^2 + ySun^2))/d2r;
if (aziSun > 180)
    aziSun = aziSun - 360
end
if (elevSun > 90) 
    elevSun = elevSun - 360;
end
if (elevSun < -90)
    elevSun = elevSun + 180;
end

%  unit conversion hard coded for mm^2 to m^2
areaVec_B      = (GetAreaVector(aziSun,elevSun))';
vecTest        = acos(dot(areaVec_B, -spacecraftToSun_B)\norm(areaVec_B)...
                  \norm(spacecraftToSun_B))*180/pi;
areaVec_I      = R_BI'*areaVec_B;
sunDistance    = norm(sunToSpacecraft_I);
fluxPressure   = Force.flux/Force.speedOfLight;
distanceFactor = (Force.AU / sunDistance);
distanceFactor = distanceFactor*distanceFactor/1000;
mag            = Force.srpScaleFactor/Sat.DryMass*...
                 fluxPressure*distanceFactor;
accelSRP       = percentSun*mag*areaVec_I;

if showDebug
    disp(['Sat to Sun Vector in Inertial = ' num2str(sunToSpacecraft_I',12)]);
    disp(['Sat to Sun Vector in Body = ' num2str(spacecraftToSun_B'*sunDistance,12)]);
    disp(['aziSun = ' num2str(aziSun,12)])
    disp(['elevSun = ' num2str(elevSun,12)])
    disp(['Sun To Spacecraft: ' num2str(sunToSpacecraft_I',12)]);
    disp(['Sun Distance: ' num2str(sunDistance,12)]);
    disp(['flux pressure = ' num2str(fluxPressure,12)]);
    disp(['distanceFactor = ' num2str(distanceFactor,12)]);
    disp(['massSpacecraft = ' num2str(Sat.DryMass,12)]);
    disp(['interpolated areaVec in Body = ' num2str(areaVec_B',12 )]);
    disp(['interpolated areaVec in Inertial = ' num2str(areaVec_I',12 )]);
    disp(['Sun/Area Vector Angle = ' num2str(vecTest,12)]);
    disp(['mag = ' num2str(mag,12 )]);
    disp(['accelSRP = ' num2str(accelSRP' ,12)]);
end
   if vecTest > 90
        disp(['Warning:  angle between sun light and force vector is ' ...
            num2str(vecTest,12)]);
    end
function areaVec = GetAreaVector(azideg,eledeg)

%  Hardcoded for firefly.spo
persistent spadData m_AzimuthStepSize m_ElevationStepSize

if isempty(spadData)
    [spadData, m_AzimuthStepSize, m_ElevationStepSize] = ...
        GetSPADData();
end

m_AzimuthCount      = 360 / m_AzimuthStepSize + 1;
m_ElevationCount    = 180 / m_ElevationStepSize + 1;

%  determine the interplation indeces
ilo = floor ((azideg+180)/m_AzimuthStepSize);
if (ilo > m_AzimuthCount-1)
    ilo = m_AzimuthCount-1;
end
if (ilo < 0)
    ilo = 0;
end
if (ilo == m_AzimuthCount-1) 
    ilo = ilo - 1;
end
ihi = ilo + 1;
    
jlo = floor((eledeg+90)/m_ElevationStepSize);
if (jlo > m_ElevationCount-1)
    jlo = m_ElevationCount-1;
end
if (jlo < 0)
    jlo = 0;
end
if (jlo == m_ElevationCount-1) 
    jlo = jlo - 1;
end
jhi = jlo + 1;

%  change to index from 1 instead of zero
ilo = ilo + 1;
ihi = ihi + 1;
jlo = jlo + 1;
jhi = jhi + 1;

% disp(['azi = ' num2str(spadData(ilo,jlo).azimuth) '   '...
%       'ele = ' num2str(spadData(ilo,jlo).elevation) '   '...
%       'area = ' num2str(spadData(ilo,jlo).vector)]);
% 
%   disp(['azi = ' num2str(spadData(ilo,jhi).azimuth) '   '...
%       'ele = ' num2str(spadData(ilo,jhi).elevation) '   '...
%       'area = ' num2str(spadData(ilo,jhi).vector)]);
%   
% disp(['azi = ' num2str(spadData(ihi,jlo).azimuth) '   '...
%       'ele = ' num2str(spadData(ihi,jlo).elevation) '   '...
%       'area = ' num2str(spadData(ihi,jlo).vector)]);
% 
%   disp(['azi = ' num2str(spadData(ihi,jhi).azimuth) '   '...
%       'ele = ' num2str(spadData(ihi,jhi).elevation) '   '...
%       'area = ' num2str(spadData(ihi,jhi).vector)]);

% Interpolated in two dimension.  This is a linear interpolation
for  k=1:3
    resultjlo = Interpolate(azideg,spadData(ilo,jlo).azimuth,...
        spadData(ihi,jlo).azimuth,spadData(ilo,jlo).vector(k),...
        spadData(ihi,jlo).vector(k));
    resultjhi = Interpolate(azideg,spadData(ilo,jhi).azimuth,...
        spadData(ihi,jhi).azimuth,spadData(ilo,jhi).vector(k),...
        spadData(ihi,jhi).vector(k));
     areaVec(k) = Interpolate (eledeg,spadData(ihi,jlo).elevation,...
         spadData(ihi,jhi).elevation, resultjlo, resultjhi);
end

function [spadData, m_AzimuthStepSize, m_ElevationStepSize] =  GetSPADData()

%% Load the spad data into a useful array
[rawData, m_AzimuthStepSize, m_ElevationStepSize] =  GetRawData();
m_AzimuthCount      = 360 / m_AzimuthStepSize + 1;
m_ElevationCount    = 180 / m_ElevationStepSize + 1;

rowCount = 1;
aziCount = 0;
colCount = 0;
for i = 1:size(rawData,1) 
    
    %  Row count
    if aziCount > m_ElevationCount - 1
        rowCount = rowCount + 1;
        aziCount = 1;
    else
        aziCount = aziCount + 1;
    end
    
    %  Column count
    if colCount > m_ElevationCount - 1
        colCount = 1;
    else
        colCount = colCount + 1;
    end
    spadData(rowCount,colCount).azimuth    = rawData(i,1);
    spadData(rowCount,colCount).elevation  = rawData(i,2);
    spadData(rowCount,colCount).vector     = rawData(i,3:5);
end

function value = Interpolate(x, x1, x2, y1, y2)
%% 2-D linear interpolation
if (x2-x1 == 0.0)
    value = y1;
else
    value = (x2-x)/(x2-x1)*y1 + (x-x1)/(x2-x1)*y2;
end

function [rawData, m_AzimuthStepSize, m_ElevationStepSize] = GetRawData()

%% the raw data for firefly
forceScaleFac = 1;  % Used for unit conversion only.  If m, set to 1.0;

JWSTSpadFileData;
%SphereSpadFileData;
%SphereSpadFileData2Degrees;

rawData(:,3:5) = rawData(:,3:5)*forceScaleFac;

   