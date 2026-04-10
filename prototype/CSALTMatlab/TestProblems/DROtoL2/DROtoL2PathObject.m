classdef DROtoL2PathObject < userfunutils.UserPathFunction
    %DROtoL2PathObject Computes dynamics and path functions
    %  July 2016, N. Parrish.  Original Version
    %  July 2019, S. Hughes, Modified for tutorial.  Cleaned up code
    
    properties
        % Small number used for numerical purposes
        sqrtEps = sqrt(eps); 
    end
    
    methods
        
        function EvaluateFunctions(obj)
            % Computes path functions for DRO to L2 problem
            
            % Extract scale factors and constants
            global constants phase_type
            MU = constants.MU;
            TU = constants.TU;
            DU = constants.DU;
            Isp = constants.Isp;
            g0 = constants.g0;
            mscale = constants.mscale;
            thrust_limit = constants.thrust_limit;
            min_dist_moon = constants.min_dist_moon;
            min_dist_earth = constants.min_dist_earth;
            
            % Extract the orbit state and mass
            stateVec = obj.GetStateVec();
            x = stateVec( 1 );
            y = stateVec( 2 );
            z = stateVec( 3 );
            vx = stateVec( 4 );
            vy = stateVec( 5 );
            vz = stateVec( 6 );
            m = stateVec( 7 ) * mscale;
                
            % If thrusting is allowed for the phase, then 
            % compute thrust and mass flow rate
            if (phase_type( obj.GetPhaseNumber() ) == 2) || ...
                    (phase_type( obj.GetPhaseNumber() ) == 3)
                % Controls
                controlVec = obj.GetControlVec();
                ux = controlVec( 1 );
                uy = controlVec( 2 );
                uz = controlVec( 3 );
                uMag = sqrt(ux.*ux + uy.*uy + uz.*uz + obj.sqrtEps);
                
                % Thrust acceleration
                % (ux, uy, uz) are in Newtons
                % (m) is in kg
                % (kg*m/s^2) -> (km/s^2) -> (DU/TU^2)
                Tx = ux / m / 1000.0 * TU * TU / DU; % acceleration due to thrust (DU/TU^2)
                Ty = uy / m / 1000.0 * TU * TU / DU;
                Tz = uz / m / 1000.0 * TU * TU / DU;
            else
                % No thrust case, set thrust and mass flow to zero
                Tx = 0; Ty = 0; Tz = 0;
                uMag = 0;
            end
            
            % Compute r1 = Earth-spacecraft
            r1 = sqrt((x+MU)*(x+MU) + y*y + z*z);
            r13 = r1*r1*r1;
            
            % Compute r2 = Moon-spacecraft
            r2 = sqrt((x-(1-MU))*(x-(1-MU)) + y*y + z*z);
            r23 = r2*r2*r2;
            
            % Compute the dynamics functions (equations of motion)
            derivatives = NaN * ones(7,1);
            derivatives( 1 ) = vx;
            derivatives( 2 ) = vy;
            derivatives( 3 ) = vz;
            derivatives( 4 ) = -( (1-MU)/r13 * (x+MU) + MU/r23 * (x-(1-MU))) + 2*vy + x + Tx;
            derivatives( 5 ) = -( (1-MU)/r13 * y + MU/r23 * y) - 2*vx + y + Ty;
            derivatives( 6 ) = -( (1-MU)/r13 * z + MU/r23 * z) + Tz;
            derivatives( 7 ) = -uMag/(Isp*g0) * TU / mscale; % mass flow rate (kg/time-unit)
            
            % Set the dynamics functions
            obj.SetDynFunctions(derivatives);
            
            % Path constraints
            if phase_type( obj.GetPhaseNumber() ) == 2 %thrust optional
                path = NaN * ones(3,1);
                path( 1 ) = uMag; % thrust magnitude
                path( 2 ) = r2; % distance to Moon
                path( 3 ) = r1; % distance to Earth
                
                obj.SetAlgFunctions(path);
                obj.SetAlgFuncLowerBounds([0; min_dist_moon; min_dist_earth]);
                obj.SetAlgFuncUpperBounds([thrust_limit; 100; 100 ]);
            elseif phase_type( obj.GetPhaseNumber() ) == 3 %thrust required
                path = NaN * ones(3,1);
                path( 1 ) = uMag; % thrust magnitude
                path( 2 ) = r2; % distance to Moon
                path( 3 ) = r1; % distance to Earth
                
                obj.SetAlgFunctions(path);
                obj.SetAlgFuncLowerBounds([thrust_limit; min_dist_moon; min_dist_earth]);
                obj.SetAlgFuncUpperBounds([thrust_limit; 100; 100 ]);
            else %thrust not allowed (coast only)
                path = NaN*ones(2,1);
                path( 1 ) = r2; % distance to Moon
                path( 2 ) = r1; % distance to Earth
                
                obj.SetAlgFunctions(path);
                obj.SetAlgFuncLowerBounds([min_dist_moon; min_dist_earth]);
                obj.SetAlgFuncUpperBounds([ 100; 100 ]);
            end
            
        end
        %% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        function EvaluateJacobians(obj)
            % Computes Jacobians of the DRO to L2 Transfer problem
            
            % Extract scale factors and constants
            global constants phase_type
            MU = constants.MU;
            TU = constants.TU;
            DU = constants.DU;
            Isp = constants.Isp;
            g0 = constants.g0;
            mscale = constants.mscale;
            thrust_limit = constants.thrust_limit;
            min_dist_moon = constants.min_dist_moon;
            min_dist_earth = constants.min_dist_earth;
            
            % Extract states
            stateVec = obj.GetStateVec();
            x = stateVec( 1 );
            y = stateVec( 2 );
            z = stateVec( 3 );
            m = stateVec( 7 ) * mscale;
            
            % If thrusting is allowed for the phase, then 
            % compute thrust and mass flow rate
            if (phase_type( obj.GetPhaseNumber() ) == 2) || ...
                    (phase_type( obj.GetPhaseNumber() ) == 3) %thrust allowed
                controlVec = obj.GetControlVec();
                ux = controlVec( 1 );
                uy = controlVec( 2 );
                uz = controlVec( 3 );
                uMag = sqrt(ux.*ux + uy.*uy + uz.*uz + obj.sqrtEps);
            elseif (phase_type( obj.GetPhaseNumber() ) == 1) %thrust not allowed
                ux = 0; uy = 0; uz = 0;
                uMag = 0;
            end
            
            % Jacobian of dynamics w/r/t state
            d2 = (MU + x - 1)^2 + y^2 + z^2 ;
            d1 = (MU + x)^2 + y^2 + z^2;
            J_d = [ 0, 0, 0, 1, 0, 0, 0;...
                0, 0, 0, 0, 1, 0, 0;...
                0, 0, 0, 0, 0, 1, 0;...
                (MU - 1)/(d1)^(3/2) - MU/(d2)^(3/2) - (3*(MU + x)*(MU - 1)*(2*MU + 2*x))/(2*(d1)^(5/2)) + (3*MU*(MU + x - 1)*(2*MU + 2*x - 2))/(2*(d2)^(5/2)) + 1, (3*MU*y*(MU + x - 1))/(d2)^(5/2) - (3*y*(MU + x)*(MU - 1))/(d1)^(5/2), (3*MU*z*(MU + x - 1))/(d2)^(5/2) - (3*z*(MU + x)*(MU - 1))/(d1)^(5/2), 0, 2, 0, -(TU^2*ux)/(1000*DU*m^2);...
                (3*MU*y*(2*MU + 2*x - 2))/(2*(d2)^(5/2)) - (3*y*(MU - 1)*(2*MU + 2*x))/(2*(d1)^(5/2)), (MU - 1)/(d1)^(3/2) - MU/(d2)^(3/2) + (3*MU*y^2)/(d2)^(5/2) - (3*y^2*(MU - 1))/(d1)^(5/2) + 1, (3*MU*y*z)/(d2)^(5/2) - (3*y*z*(MU - 1))/(d1)^(5/2), -2, 0, 0, -(TU^2*uy)/(1000*DU*m^2);...
                (3*MU*z*(2*MU + 2*x - 2))/(2*(d2)^(5/2)) - (3*z*(MU - 1)*(2*MU + 2*x))/(2*(d1)^(5/2)), (3*MU*y*z)/(d2)^(5/2) - (3*y*z*(MU - 1))/(d1)^(5/2), (MU - 1)/(d1)^(3/2) - MU/(d2)^(3/2) + (3*MU*z^2)/(d2)^(5/2) - (3*z^2*(MU - 1))/(d1)^(5/2), 0, 0, 0, -(TU^2*uz)/(1000*DU*m^2);...
                0, 0, 0, 0, 0, 0, 0];
            obj.SetDynStateJac(J_d);
            
            % Jacobian of dynamics w/r/t control
            J_u = [ 0, 0, 0;...
                0, 0, 0;...
                0, 0, 0;...
                TU^2/(1000*DU*m), 0, 0;...
                0, TU^2/(1000*DU*m), 0;...
                0, 0, TU^2/(1000*DU*m);...
                -(TU*ux)/(Isp*g0*mscale*uMag), -(TU*uy)/(Isp*g0*mscale*uMag), -(TU*uz)/(Isp*g0*mscale*uMag)];
            obj.SetDynControlJac(J_u);
            
           % Jacobian of dynamics w/r/t time
            obj.SetDynTimeJac(zeros(7,1));
            
            % Jacobian for algebraic functions w/r/t state
            J_alg_state = [ 0, 0, 0, 0, 0, 0, 0;...
                (MU + x - 1)/(d2)^(1/2), y/(d2)^(1/2), z/(d2)^(1/2), 0, 0, 0, 0;...
                (2*MU + 2*x)/(2*(d1)^(1/2)), y/(d1)^(1/2), z/(d1)^(1/2), 0, 0, 0, 0];
            obj.SetAlgStateJac( J_alg_state );
            
            % Jacobian for algebraic functions w/r/t control
            J_alg_ctrl =[ ux/uMag, uy/uMag, uz/uMag;...
                0, 0, 0;...
                0, 0, 0];
            obj.SetAlgControlJac( J_alg_ctrl );
                
            % Jacobian for algebraic functions w/r/t time          
            obj.SetAlgTimeJac( zeros(3,1) );
        end
    end
    
end



