classdef OrbitRaisingPathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            
            mu = 1;
            mdot = 0.0749;
            T = 0.1405;
            
            %%  Extract state and control components
            r         = stateVec(1,1);
            theta     = stateVec(2,1);
            dr_dt     = stateVec(3,1);
            dtheta_dt = stateVec(4,1);
            m         = stateVec(5,1);
            u_r       = controlVec(1,1);
            u_theta   = controlVec(2,1);
            
            %%  Compute the dynamics
            dynFunctions(1,1) = dr_dt ;
            dynFunctions(2,1) = dtheta_dt/r;
            dynFunctions(3,1) = dtheta_dt*dtheta_dt/r - mu/r^2 + T/m*u_r;
            dynFunctions(4,1) = -dr_dt*dtheta_dt/r +T/m*u_theta;
            dynFunctions(5,1) = -mdot;
            obj.SetDynFunctions(dynFunctions);
            
            obj.SetAlgFunctions(u_r^2 + u_theta^2);
            obj.SetAlgFuncUpperBounds(1);
            obj.SetAlgFuncLowerBounds(1);
            
        end
        
        function EvaluateJacobians(obj)
            
%             stateVec = obj.GetStateVec();
%             controlVec = obj.GetControlVec();
%             
%             %%  Extract state and control components
%             r         = stateVec(1,1);
%             theta     = stateVec(2,1);
%             dr_dt     = stateVec(3,1);
%             dtheta_dt = stateVec(4,1);
%             m         = stateVec(5,1);
%             u_r       = controlVec(1,1);
%             u_theta   = controlVec(2,1);
%             
%             mu = 1;
%             mdot = 0.0749;
%             T = 0.1405;
%             
%             obj.SetDynStateJac(...
%                 [0               0          1            0            0;
%                 -dtheta_dt/r^2       0          0           1/r           0;
%                 (-dtheta_dt*dtheta_dt/r^2 + 2*mu/r^3) 0 0 2*dtheta_dt/r -T/m^2*u_r;
%                 dr_dt*dtheta_dt/r^2 0 -dtheta_dt/r -dr_dt/r -T/m^2*u_theta
%                 0 0 0 0 0]);
%             
%             obj.SetDynTimeJac(zeros(5,1));
%             
%             obj.SetDynControlJac(...
%                 [0 0;
%                 0 0;
%                 T/m 0;
%                 0 T/m;
%                 0 0]);
%             
%             obj.SetAlgStateJac([0 0 0 0 0]);
%             obj.SetAlgTimeJac(0);
%             obj.SetAlgControlJac([2*u_r 2* u_theta]);
            
        end
    end
    
end



