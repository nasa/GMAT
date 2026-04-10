classdef OrbitState < handle
    %ORBITSTATE: A utility for managing the orbit state
    %   Detailed explanation goes here
    
    properties (Access = private)
        % Array of  doubles. The current state in cartesian format
        currentState = [7100 0 2000 0 7.4 1]';
        % Double. The gravitational parameter for the central body
        gravParam = 398600.4415
    end
    
    methods
        
        function obj = SetKeplerianState(obj,SMA,ECC,INC,RAAN,AOP,TA)
            % Sets the Keplerian state
            % obj = SetKeplerianState(obj,SMA,ECC,INC,RAAN,AOP,TA)
            obj.currentState = obj.ConvertKepToCart(SMA,ECC,INC,RAAN,AOP,TA);
        end
        
        function obj = SetKeplerianVectorState(obj,kepVector)
            % Sets Keplerian state given states in an array
            obj.currentState = obj.ConvertKepToCart(...
                kepVector(1),kepVector(2),kepVector(3),...
                kepVector(4),kepVector(5),kepVector(6));
        end
        
        function obj = SetCartesianState(obj,cartVec)
            % Sets the cartesian state
            obj.currentState = cartVec;
        end
        
        function SetGravParam(obj,mu)
            % Sets the gravitational parameter
            obj.gravParam = mu;
        end
        
        function kepState = GetKeplerianState(obj)
            % Returns the Keplerian state
            kepState = obj.ConvertCartToKep(obj.currentState);
        end
        
        function cartState = GetCartesianState(obj)
            % Returns the Cartesian state
            cartState = obj.currentState;
        end
        
    end
    
    methods (Access = private)
        
        function cartVec = ConvertKepToCart(obj,a,e,i,Om,om,nu)
            % Converts from Cartesian to Keplerian
            
            % C++ code should be borrowed from GMAT state converter
            p = a*(1-e*e);
            r = p/(1+e*cos(nu));
            rv = [r*cos(nu); r*sin(nu); 0];			% in PQW frame
            vv = sqrt(obj.gravParam/p)*[-sin(nu); e+cos(nu); 0];
            %
            %	now rotate
            %
            cO = cos(Om);  sO = sin(Om);
            co = cos(om);  so = sin(om);
            ci = cos(i);   si = sin(i);
            R  = [cO*co-sO*so*ci  -cO*so-sO*co*ci  sO*si;
                sO*co+cO*so*ci  -sO*so+cO*co*ci -cO*si;
                so*si            co*si           ci];
            cartVec = [R*rv;R*vv];
        end
        
        function oe =  ConvertCartToKep(obj,cart)
            % Converts from Keplerian to Cartesian
            
            % C++ code should be borrowed from GMAT state converter
            twopi = 2*pi;
            
            rv = cart(1:3);  % position vector
            vv = cart(4:6);  % velocity vector
            r  = norm(rv);   % magnitude of position vector
            v  = norm(vv);   % magnitude of velocity vector
            
            hv = cross(rv,vv);  % orbit angular momentum vector
            h  = norm(hv);
            
            kv = [0 0 1]';      %  Inertial z axis
            nv = cross(kv,hv);  %  vector in direction of RAAN,  zero vector if equatorial orbit
            n = norm(nv);
            
            %  Caculate eccentricity vector and eccentricity
            ev = (v*v - obj.gravParam/r)*rv/obj.gravParam - dot(rv,vv)*vv/obj.gravParam;
            ECC  = norm(ev);
            E = v*v/2 - obj.gravParam/r;   %  Orbit Energy
            
            % Check eccentrity for parabolic orbit
            if ( abs(1 - ECC) < 2*eps )
                disp('Warning:  Orbit is nearly parabolic and state conversion routine is near numerical singularity')
            end
            % Check Energy for parabolic orbit
            if ( abs(E) < 2 * eps )
                disp('Warning:  Orbit is nearly parabolic and state conversion routine is near numerical singularity')
            end
            
            %  Determine SMA depending on orbit type
            if ( ECC ~= 1 )
                SMA = -obj.gravParam/2/E;
                %p   = SMA*(1-ECC*ECC);
            else
                SMA = inf;
                %p   = h*h/mu;
            end
            
            %  Determine Inclination
            INC     = acos(hv(3)/h);
            
            %=== Elliptic equatorial
            if ( INC <= 1e-11 && ECC > 1e-11 )
                
                %  Determine RAAN
                RAAN = 0;
                
                %  Determine AOP
                AOP    = acos(ev(1)/ECC);
                if ( ev(2) < 0 )
                    AOP = twopi - AOP;
                end
                
                %  Determine TA
                TA = acos(dot(ev,rv)/ECC/r);
                if ( dot(rv,vv) < 0 )
                    TA = twopi - TA;
                end
                
                %=== Circular Inclined
            elseif ( INC >= 1e-11 && ECC <= 1e-11 )
                
                %  Determine RAAN
                RAAN    = acos(nv(1)/n);
                if ( nv(2) < 0 )
                    RAAN = twopi - RAAN;
                end
                
                %  Determine AOP
                AOP  = 0;
                
                %  Determine TA
                TA = acos(dot(nv/n,rv/r));
                if ( rv(3) < 0 )
                    TA = twopi - TA;
                end
                
                %=== Circular equatorial
            elseif ( INC <= 1e-11 && ECC <= 1e-11  )
                
                RAAN = 0;
                AOP  = 0;
                TA   = acos(rv(1)/r);
                if ( rv(2) < 0 )
                    TA = twopi - TA;
                end
                
            else  %  Not a special case
                
                %  Determine RAAN
                RAAN    = acos(nv(1)/n);
                if ( nv(2) < 0 )
                    RAAN = twopi - RAAN;
                end
                
                %  Determine AOP
                AOP    = acos(dot(nv/n,ev/ECC));
                if ( ev(3) < 0 )
                    AOP = twopi - AOP;
                end
                
                %  Determine TA
                TA = acos(dot(ev/ECC,rv/r));
                if ( dot(rv,vv) < 0 )
                    TA = twopi - TA;
                end
                
            end
            
            oe = real([SMA ECC INC RAAN AOP TA]);
        end
        
    end
    
end