%...this script tests the implementation and algorithms designed to perform
%   weighted batch least-squares on a set of optical angles (ra/dec) data.
%
%   t. kelecy, boeing-lts, 12/17/2008
%

format long g

global statesize eciPos eciVel ecefPos
global xp yp dEpsilon dPsi UT1minusUTC dAT
global omegaearth Rearth muearth fearth J2 speedoflight
global rad2deg deg2rad arcsec2rad rad2arcsec
global OWLT

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% define constants, conversion factors, and the like %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% radians to degrees
rad2deg = 180/pi;
% degrees to radians
deg2rad = pi/180;
% arcseconds to radians
arcsec2rad = (1/3600)*deg2rad;
% radians to arcseconds
rad2arcsec = 1/arcsec2rad;
% Speed of Light in m/s
speedoflight = 299792.458*1000;
% Earth's angular velocity in rad/sec
omegaearth = 7.2921158553*(10^(-5));
% dominant zonal harmonic coefficient
%J2=1.082626925638815*(10^(-3));	% J2 coefficient
J2=0.001082626683553;
% Earth Flatness Factor
fearth = 3.353642e-3;
% Equatorial Radius of the Earth (m)
%Rearth = 6378.137*1000;
Rearth = 6378136.3;
% gravitational parameter of Earth (m^3/s^2)
%muearth = 398600.4328969392*(1000^3);
muearth = 3.986004415e+14;
% spacecraft mass (kg)
%scmass = 2200; %TDRSS5
scmass = 1703.6700;
% spacecraft area (m^2) all in sc-X direction
%scarea = [29 + pi*1^2 + pi*(4.9*0.5)^2 + 3^2;0;0]; % TDRSS5
scarea = 40;

OWLT = 1;  %  this is now a flag: 0 = no light time correction; 1 = apply light time correction

MaxIter = 6;  %  maximum number of iterations

%# ----------------------------------------------------------------------------------------------------
%#   Date    MJD      x         y       UT1-UTC      LOD       dPsi    dEpsilon     dX        dY    DAT
%# (0h UTC)           "         "          s          s          "        "          "         "     s 
%# ----------------------------------------------------------------------------------------------------
%2007 05 08 54228  0.093609  0.486619 -0.1142208  0.0007897 -0.054137 -0.007758  0.000288 -0.000379  33


%xp = 0.092003*arcsec2rad; % radians
%yp = 0.487744*arcsec2rad; % radians
%dPsi = -0.054082/3600; % degrees
%dEpsilon = -0.007554/3600; % degrees
%UT1minusUTC = -0.1141492; % seconds
%dAT = 33; %seconds

%...new values added for odtk comparison
xp_arcs     = 0.093609; 
yp_arcs     = 0.486619;
xp          = xp_arcs*arcsec2rad; % radians
yp          = yp_arcs*arcsec2rad; % radians
dPsi        = -0.054137/3600; % degrees
dEpsilon    = -0.007758/3600; % degrees
UT1minusUTC = -0.1142208; % seconds 
dAT         =  33; %seconds

% estimation epoch (UTC)
year   = 2007;
month  = 5;
day    = 8;
hour   = 5;
minute = 27;
second = 55.000;

%...read b3 angle observations
readB3;
Nobs = length(obs(:,1));  %  number of observation pairs

step = 10;  %  ode45 integration step parameter (seconds?)

% set the integration options
options = odeset('RelTol',1E-12,'AbsTol',1E-12,'InitialStep',1,'MaxStep',step);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% define telescope site (rme on maui)%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Lat = 20.746500;  %  latitude (deg)
Lon = -156.432050;  %  longitude (deg)
Hgt = 100.78;  %  geodetic height (m)
[ ecefPos(1,1), ecefPos(2,1), ecefPos(3,1) ] = LLA_to_XYZ( Lat, Lon, Hgt );  %  ecef fixed site vector
ecefVel = zeros(3,1);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%...convert ecef site coordinates to eci %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[eciPos,eciVel,secondsPastJ2K] = ecef2eci_convert(year,month,day,...
                              hour,minute,second,ecefPos,ecefVel);
% define initial time and current time
timeInitial = secondsPastJ2K;
timeNow = timeInitial;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% define initial state vector %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
r = [ -33676553.9725404; 24914728.850706; 5824400.09033107];
v = [ -1813.37607817738; -2471.54335695136; 79.7839760900568];
%r = [ -33676953.97254; 24914768.85071; 5824400.09033];
%v = [ -1813.37608; -2471.54336; 79.78398];

%...store state in working arrays
Xo = [r;v];
Xest = Xo;

% define number of state variables
statesize = length(Xo);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% define measurement covariance %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
rasigma_arcsec = 1;
decsigma_arcsec = 1;
rasigma = rasigma_arcsec*arcsec2rad; % arcseconds to radians
decsigma = decsigma_arcsec*arcsec2rad; % arcseconds to radians
rms = [rasigma;decsigma];
Wra = 1/(rasigma^2);
Wdec = 1/(decsigma^2);

%  simulated noise
%raN = rasigma * randn(1,Nobs);
%decN = rasigma * randn(1,Nobs);

W = [Wra,0;0,Wdec];

data_types = length(W);

% setup a priori covariance
Pobar      = zeros(statesize,statesize);
Pobar(1,1) = (1000000)^2;
Pobar(2,2) = (1000000)^2;
Pobar(3,3) = (1000000)^2;
Pobar(4,4) = (1000)^2;
Pobar(5,5) = (1000)^2;
Pobar(6,6) = (1000)^2;
invPobar   = inv(Pobar);  %  inverst a priori covariance
P          = Pobar;

myState = zeros(statesize,Nobs);
myHmatrix = zeros(data_types,statesize,Nobs);
myResids = zeros(2,Nobs);
meastime =  zeros(1,Nobs);
XHAT= zeros(statesize,MaxIter);
RMS = zeros(2,MaxIter);
icount = 0;

oldRMS = 0;
newRMS = 9999;
tolRMS = 1.0e-6;

while ( icount <= MaxIter && abs( newRMS - oldRMS ) > tolRMS ) %iteration loop
    icount = icount + 1;  %  increment the iteration counter
    fprintf('Iteration # %d\n',icount)
    %...set updated epoch state
    X = Xest;
    %...set epoch time (julian date)
    timeNow = timeInitial;
    
    % satellite position and velocity
    r = [X(1,1);X(2,1);X(3,1)];
    v = [X(4,1);X(5,1);X(6,1)];

    % h terms initialization
    htwh = zeros(statesize,statesize,Nobs);
    htwy = zeros(statesize,Nobs);
    % initialize Htilde matrix
    Htilde = zeros(data_types,statesize);
    % initial state transition matrix (identity)
    phio = eye(statesize);
    phi = phio;
    % initialize rms
    rms = zeros(data_types,1);
    % initialize obs deviation
    y = zeros(data_types,1);
    
    %...set a priorit state
    xobar = zeros(statesize,1);
    
    inc = 1;  %  initialize observation increment
    
    %while timeNow <= timeFinal
    while inc <= Nobs

        year = obs(inc,1);
        month = obs(inc,2);
        day = obs(inc,3);
        hour = obs(inc,4);
        minute = obs(inc,5);
        second = obs(inc,6);
        %YY = year - 100 * fix(year/100);
        %DDD = Cal_2_Doy( year, month, day );
        %DateTime = [year,month,day,hour,minute,second];       
                
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        %...convert ecef site coordinates to eci %
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        [eciPos,eciVel,timeObsJ2k] = ecef2eci_convert(year,month,day,...
                                      hour,minute,second,ecefPos,ecefVel);
        
        %%%%%%%%%%%%%%%%%%%%%%%%
        % Propagate Trajectory %
        %%%%%%%%%%%%%%%%%%%%%%%%

        %fprintf('timeObsJ2k-OWLT-timeNow = %f\n',timeObsJ2k-OWLT-timeNow)
        if ( timeNow ~= timeObsJ2k )
            phiV = reshape(phi,statesize*statesize,1);
            Xaug = [X;phiV];
            [tv,stateout] = ode45('grav_withPHI',[timeNow timeObsJ2k],Xaug,options);
                    
            [staterow,statecol] = size(stateout);
            %size(stateout)

            t = tv(staterow);
            r = stateout(staterow,1:3)';
            v = stateout(staterow,4:6)';
            phiV = stateout(staterow,7:end)';
            phi = reshape(phiV,statesize,statesize);
        
        end  %  if ( timeNow ~= timeObsJ2k )
        
        % update state variables
        myState(:,inc) = X;
        X(1,1)=r(1,1);
        X(2,1)=r(2,1);
        X(3,1)=r(3,1);
        X(4,1)=v(1,1);
        X(5,1)=v(2,1);
        X(6,1)=v(3,1);
        %[inc;X;radeccomp];           
         
        % compute predicted observations
        radeccomp = myOpticalObservations_batch(X);
        
        %...stellar aberration not used for now
        %raL = radeccomp(1,1) * rad2deg;
        %decL = radeccomp(2,1) * rad2deg;
        %[ saRA, saDEC, sunV  ] = stellar_aberration(YY,DDD,raL,decL);
        %sa = [saRA*arcsec2rad;saDEC*arcsec2rad];
        
        % compute observation partials
        Htilde = myOpticalHtildeMatrix(radeccomp,X);
        % compute observation partials mapped to epoch
        H = Htilde*phi;
        % compute observation residuals
        %  load observations for this time
        radecobs = obs(inc,7:8)';
        y = radecobs - radeccomp; 
        myResids(:,inc) = y;
        % compute observation residual RMS
        rms = sqrt((sum(myResids.^2,2))/inc);

        % compute innovations covariance
        [Sic,pc] = chol(Htilde*Htilde');
        innCov(:,:,inc) = Sic'*Sic*W*Sic'*Sic + inv(W);
        
        % accumulate matrices to be used in solving normal equations
        if inc == 1
            htwh(:,:,1) = H'*W*H;
            htwy(:,1) = H'*W*y;
        else
            htwh(:,:,inc) = H'*W*H + htwh(:,:,inc-1);
            htwy(:,inc) = H'*W*y + htwy(:,inc-1);
        end  %  if inc == 1
        
        %...save the H matrix
        myHmatrix(1:data_types,1:statesize,inc) = H;

        %meastime(inc) = timeInSecs;
        meastime(inc) = timeObsJ2k;
       
        inc = inc+1;  %  increment the observation counter

        timeNow = timeObsJ2k;  %  update current time with obs time
        
    end  %  while inc <= Nobs

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % Classic batch - Normal Equations %
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    gamma = invPobar + htwh(:,:,end);
    N = ( invPobar * xobar ) + htwy(:,end);
    %...comverged covariance
    estCov = inv( gamma );
    xhat = estCov * N;

    XHAT(:,icount) = xhat;
    RMS(:,icount) = rms;
    Xest = Xest + xhat;
    xobar = xobar - xhat;
    
    %...comput new rms
    sumRMS = 0;
    for ii=1:Nobs
        epsilon = myResids(:,ii) - myHmatrix(:,:,ii) * xhat;
        sumRMS = sumRMS + epsilon' * W * epsilon;
    end  %  for ii=1:Nobs
    oldRMS = newRMS;
    newRMS = sqrt( sumRMS / Nobs / data_types );
    
end  %  while ( icount <= MaxIter && abs( newRMS - oldRMS ) > tolRMS ) %iteration loop



fprintf('Final Numer of Interations = %d\n',icount)
fprintf('Final RMS = %e\n',newRMS)
fprintf('Initial State  = %+f m, %+f m, %+f m, %+f m/s, %+f m/s, %+f m/s\n',Xo')
fprintf('Estimated State  = %+f m, %+f m, %+f m, %+f m/s, %+f m/s, %+f m/s\n',Xest')
fprintf('State Adjustment  = %+f m, %+f m, %+f m, %+f m/s, %+f m/s, %+f m/s\n',Xest'-Xo')
%fprintf('Initial Error    = %+f km, %+f km, %+f km, %+f km/s, %+f km/s, %+f km/s\n',blsq_pv(1,3:8)-blsq_pv(2,3:8))
%fprintf('Estimation Error = %+f km, %+f km, %+f km, %+f km/s, %+f km/s, %+f km/s\n',Xest'-blsq_pv(2,3:8))
fprintf('Estimated Covariance (Diagaonals are Sigmas) = \n')
fprintf('%+8.4e\n',sqrt(estCov(1,1)))
fprintf('%+8.4e %+8.4e\n',estCov(2,1),sqrt(estCov(2,2)))
fprintf('%+8.4e %+8.4e %+8.4e\n',estCov(3,1:2),sqrt(estCov(3,3)))
fprintf('%+8.4e %+8.4e %+8.4e %+8.4e\n',estCov(4,1:3),sqrt(estCov(4,4)))
fprintf('%+8.4e %+8.4e %+8.4e %+8.4e %+8.4e\n',estCov(5,1:4),sqrt(estCov(5,5)))
fprintf('%+8.4e %+8.4e %+8.4e %+8.4e %+8.4e %+8.4e\n',estCov(6,1:5),sqrt(estCov(6,6)))


for i = 1:inc-1;
    innCovSigmas(:,i) = sqrt(diag(innCov(:,:,i)));
end
figure(1)
subplot(211)
plot((meastime(:)-meastime(1))/60,3600*180*myResids(1,:)/pi,'+')
hold on
plot((meastime(:)-meastime(1))/60,3*3600*180*innCovSigmas(1,:),'-+r')
plot((meastime(:)-meastime(1))/60,-3*3600*180*innCovSigmas(1,:),'-+r')
grid
title('Measurement Residuals')
xlabel('Minutes from Start')
ylabel('RA (arcsec)')
subplot(212)
plot((meastime(:)-meastime(1))/60,3600*180*myResids(2,:)/pi,'+')
hold on
plot((meastime(:)-meastime(1))/60,3*3600*180*innCovSigmas(2,:),'-+r')
plot((meastime(:)-meastime(1))/60,-3*3600*180*innCovSigmas(2,:),'-+r')
grid
xlabel('Minutes from Start')
ylabel('Dec (arcsec)')

%...end of script