classdef OrbitPathFunc < userfunutils.UserPathFunction
    %OrbitPathFunc Intermediat class contianing common data
    % and interfaces for orbit dynamics path functions
    
    properties (Access = protected)
        
    end
    
    methods (Access = public)
        
        function SetDynamics(obj)
            % Sets the orbit dynamics in non-dimensional units 
            
            % Set constants
            phaseIdx = obj.GetPhaseNumber();
            gravParam = 1.0;
            
            %  Get the state and control
            [thrustVec,mdot] = ComputeThrust(obj);
            stateVec = obj.GetStateVec();
            if strcmp(obj.phaseList{phaseIdx}.GetStateRep(),'ModEquinoctial')
                dynFunc = ModEqDynamics(gravParam);
                odeRHS = dynFunc.ComputeOrbitDerivatives(stateVec,thrustVec);
                odeRHS(7,1) = -mdot;
                obj.SetDynFunctions(odeRHS);
            else
                error(['Unsupported state rep : ' obj.phaseList{phaseIdx}])
            end
            
        end
        
        function SetControlPathConstraint(obj)
            % Set constraint on the control unit vector magnitude
            phaseIdx = obj.GetPhaseNumber();
            if strcmp(obj.phaseList{phaseIdx}.GetControlRep(),'CartesianFourVec')
                controlVec = obj.GetControlVec();
                controlMagCon = norm(controlVec(2:4));
                obj.SetAlgFunctions(controlMagCon);
                obj.SetAlgFuncUpperBounds(1);
                obj.SetAlgFuncLowerBounds(1);
            else
                error(['Unsupported control rep : ' obj.phaseList{phaseIdx}])
            end
        end
        
        function [thrustVec,mdot] = ComputeThrust(obj)
            % Computes the thrust vector in non-dimensional units
            
            phaseIdx = obj.GetPhaseNumber();
            scaleUtil = obj.phaseList{phaseIdx}.orbitScaleUtil;
            
            % If phase is a coast, rates are zero.
            if strcmp(obj.phaseList{phaseIdx}.GetThrustMode(),'Coast')
                thrustVec = zeros(3,1);
                mdot = 0.0;
                return
            end
            
            % Define constants.
            scaleFac = 1e3;
            Isp = obj.phaseList{phaseIdx}.GetIsp(); 
            availableThrustNewtons = obj.phaseList{phaseIdx}.GetAvailableThrust();             
            availableThrustNonDim = scaleUtil.ScaleThrustMag(availableThrustNewtons)/scaleFac;

            % Compute the thrust based on the control representation
            controlVec = obj.GetControlVec();
            stateVec = obj.GetStateVec();
            if strcmp(obj.phaseList{phaseIdx}.GetControlRep(),'CartesianFourVec')
                thrustVec = controlVec(1)*availableThrustNonDim*controlVec(2:4)/stateVec(7);
                mDotDim = controlVec(1)*(availableThrustNewtons/Isp/9.80665);
                mdot = scaleUtil.ScaleMassFlowRate(mDotDim);
            else
                error(['Unsupported control rep : ' obj.phaseList{phaseIdx}]);
            end
            
        end
        
    end
    
end

