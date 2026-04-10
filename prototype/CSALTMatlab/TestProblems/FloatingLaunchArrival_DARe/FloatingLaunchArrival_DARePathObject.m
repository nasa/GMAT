classdef FloatingLaunchArrival_DARePathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        %  Pointer to the dynamics model class
        SEPForces
        %  Pointer to non-dimensionalization data
        config
    end
    
    methods
        
        function EvaluateFunctions(obj)

            useHiFiDynamics = false(); % 793 secs with HiFi
            
            %  Re-Dimensionalize States/Time etc.
            time = obj.GetTime()*obj.config.TU;
            stateVec = obj.config.StateUnScaleMat*obj.GetStateVec();
            controlVec = obj.GetControlVec();
            mu = 132712440017.99;
            controlMag_Newtons = .236/1000*.8;
            g = 9.81;
            Isp = 4190;
            Xdot(1:3,1) = stateVec(4:6,1);
            
            if obj.GetPhaseNumber() == 1 || obj.GetPhaseNumber() == 3;
                controlVec = zeros(3,1);
            end
            
            if useHiFiDynamics
                [Xdot,A]= obj.SEPForces.ComputeForcesGivenControl(time,stateVec,controlVec);
            else
                Xdot(4:6,1) = -mu*stateVec(1:3,1)/sqrt(stateVec(1:3,1)'*stateVec(1:3,1))^3 + ...
                    (controlMag_Newtons)*controlVec/stateVec(7,1);
                Xdot(7,1) = -controlMag_Newtons*sqrt(controlVec'*controlVec)/(Isp*g/1000);
            end
            
            if obj.GetPhaseNumber() == 2 || obj.GetPhaseNumber() == 4
                obj.SetAlgFunctions(controlVec'*controlVec);
                obj.SetAlgFuncUpperBounds(1);
                obj.SetAlgFuncLowerBounds(0);
            end
            
            obj.SetDynFunctions(obj.config.StateAccScaleMat*Xdot);
            
            
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end



