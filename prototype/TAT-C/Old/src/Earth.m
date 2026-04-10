classdef Earth
    %EARTH A simple model of the earth physlical properties and rotation
    
    properties
        % Double.  The J2 term for Earth
        gravParamJ2 = 1.0826269e-003;
        % Double. The gravitational parameter of Earth
        gravParamMu = 3.986004415e+5;
        % Double. The equatorial radius of Earth
        bodyRadius = 6.3781363e+003;
        % Double.  The flattening of Earth
        bodyFlattening = 0.0033527;
    end
    
    methods
        
        function R = GetInertialToFixedRotation(obj,jDate)
            % Returns matrix from Intertial to Fixed given Julian date
            gmt = obj.ComputeGMT(jDate);
            cG = cos(gmt);
            sG = sin(gmt);
            R = [cG sG 0; -sG cG 0; 0 0 1];
        end
        
        function GMT = ComputeGMT(~,jDate)
            % Returns GMT given Julian Data
            timeUT1 = (jDate - 2451545)/36525;
            GMT  = 67310.54841 +(876600*3600+8640184.812866)*timeUT1 ...
                + 0.093104*timeUT1^2 - 6.2e-6*timeUT1^3;
            d2r = pi/180;
            GMT = mod(GMT,86400)/240*d2r;
        end
        
        function posVecFixed = GetBodyFixedState(obj,posVecInertial,jDate)
            % Rotate position vector from inertial to body fixed
            posVecFixed = obj.GetInertialToFixedRotation(jDate)*posVecInertial;
        end
        
        function R = FixedToTopoRMat(~,gdLat,gdLon)
            % Computes rotation matrix from inertial to topocentric axes
            % Inputs: int gdLat, int gdLon (geodedic coordinates of topo
            % system).
            zHat = [cos(gdLat)*cos(gdLon);cos(gdLat)*sin(gdLon);sin(gdLat)];
            kHat = [0 0 1]';
            yHat = cross(kHat,zHat);
            xHat = cross(yHat,zHat);
            R = [xHat yHat zHat];
        end
        
        function [gdLat] = GeocentricToGeodeticLat(obj,gcLat)
           % Converts from geocentric latitude to geodetic latitude
           eSquared = 2*obj.bodyFlattening - obj.bodyFlattening^2;
           gdLat = gcLat;
            xyPos = obj.bodyRadius*cos(gcLat);
            zPos = obj.bodyRadius*sin(gcLat);
            diff = 1;
            while diff > 1e-10;
                phi = gdLat;
                sinPhi = sin(gdLat);
                C = obj.bodyRadius/sqrt(1 - eSquared*sinPhi*sinPhi);
                gdLat = atan2(zPos + C*eSquared*sinPhi,xyPos);
                diff = abs(gdLat - phi);
            end
        end
        
        function [rsun,rtasc,decl] = GetEarthSunDistRaDec(~,jd)
            
            %  author        : david vallado                  719-573-2600   27 may 2002
            %
            %  revisions
            %    vallado     - fix mean lon of sun                            7 mat 2004
            
            % -------------------  initialize values   --------------------
            twopi      =     2.0*pi;
            deg2rad    =     pi/180.0;
            tut1= ( jd - 2451545.0  )/ 36525.0;
            
            meanlong= 280.460  + 36000.77*tut1;
            meanlong= rem( meanlong,360.0  );  %deg
            
            ttdb= tut1;
            meananomaly= 357.5277233  + 35999.05034 *ttdb;
            meananomaly= rem( meananomaly*deg2rad,twopi );  %rad
            if ( meananomaly < 0.0  )
                meananomaly= twopi + meananomaly;
            end
            eclplong= meanlong + 1.914666471 *sin(meananomaly) ...
                + 0.019994643 *sin(2.0 *meananomaly); %deg
            eclplong= rem( eclplong,360.0  );  %deg
            obliquity= 23.439291  - 0.0130042 *ttdb;  %deg
            eclplong = eclplong *deg2rad;
            obliquity= obliquity *deg2rad;
            
            % Find magnitude of sun vector components
            magr= 1.000140612  - 0.016708617 *cos( meananomaly ) ...
                - 0.000139589 *cos( 2.0 *meananomaly );    % in au's
            rsun(1)= magr*cos( eclplong );
            rsun(2)= magr*cos(obliquity)*sin(eclplong);
            rsun(3)= magr*sin(obliquity)*sin(eclplong);
            rtasc= atan( cos(obliquity)*tan(eclplong) );
            
            % Check that rtasc is in the same quadrant as eclplong ----
            if ( eclplong < 0.0  )
                eclplong= eclplong + twopi;    % make sure it's in 0 to 2pi range
            end
            if ( abs( eclplong-rtasc ) > pi*0.5  )
                rtasc= rtasc + 0.5 *pi*round( (eclplong-rtasc)/(0.5 *pi));
            end
            decl = asin( sin(obliquity)*sin(eclplong) );

        end
        
    end
    
end

