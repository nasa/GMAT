classdef LISAPathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        controlMag = 0.01/10;
        scaleUtil
        ForceConfig
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            global phaseList configSun
            
            %=== Set constants
            coast = false;%false;   % For testing propagation
            includePerts = true;
            scaleFac = 1e3;
            gravParam = 1.0;
            dutyCycle = 0.9;
            Isp = 1662;
            availableThrustNewtons = 0.0841;
            availableThrustNonDim = availableThrustNewtons/scaleFac*...
                obj.scaleUtil.sunScaling.TU^2/obj.scaleUtil.sunScaling.MU...
                /obj.scaleUtil.sunScaling.DU;
            
            %===  Extract state and control components
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            time = obj.GetTime();
            
            %=== Compute external forces
            if includePerts
                appliedControlMag = norm(controlVec(2:4));
                saaAngle = asin(controlVec(2)/(appliedControlMag + sqrt(eps)));
                modEqDim = obj.scaleUtil.UnScaleState(stateVec,1);
                cartStateDim = Mee2Cart(modEqDim(1:6,1),configSun.gravParam);
                dimTime = obj.scaleUtil.UnScaleTime(time,1,phaseList{1}.refEpoch);
                elapsedTimeSeconds = (dimTime - phaseList{1}.refEpoch)*86400;
                elapsedTimeDays = elapsedTimeSeconds/86400;
                [Xdot] = dynfunc(elapsedTimeSeconds,cartStateDim,obj.ForceConfig);
                orbAccNonDim = Xdot(4:6,1)*configSun.TU^2/configSun.DU;
                R_RTN_Inert = obj.ComputeRTNRotationMatrix(cartStateDim);
                accExtern = R_RTN_Inert*orbAccNonDim;
            else
                accExtern = [0 0 0]';
            end
            
            %===  Compute the dynamics
            lossFactor = cos(saaAngle);  % loss due to attitude slew
            accThrust = controlVec(1)*availableThrustNonDim*controlVec(2:4)/stateVec(7)*lossFactor*dutyCycle;
            mdot = controlVec(1)*(availableThrustNewtons/Isp/9.81)*...
                obj.scaleUtil.sunScaling.TU/obj.scaleUtil.sunScaling.MU*lossFactor*dutyCycle;
            dynFunc = ModEqDynamics(gravParam);
            if coast == true
                accThrust = accThrust*0;
                mdot = mdot*0;
            end
            pertForces = accThrust + accExtern;
            odeRHS = dynFunc.ComputeOrbitDerivatives(stateVec,pertForces);
            odeRHS(7,1) = -mdot;
            obj.SetDynFunctions(odeRHS);
            
            %=== Constraint on the control vector magnitude
            controlMagCon = norm(controlVec(2:4));
            
            obj.SetAlgFunctions([controlMagCon;saaAngle]);
            obj.SetAlgFuncUpperBounds([1;30*pi/180]);
            obj.SetAlgFuncLowerBounds([1;-30*pi/180]);
            
        end
        
        function EvaluateJacobians(obj)
            
        end
        
        function SetScaleUtil(obj,scaleUtil)
            obj.scaleUtil = scaleUtil;
        end
        
        function R = ComputeRTNRotationMatrix(~,cart)
            % Computes rotation matrix from intertial to RTN
            rvHat = cart(1:3)/norm(cart(1:3));
            hv = cross(cart(1:3,1),cart(4:6,1));
            nvHat = hv/norm(hv);
            tvHat = cross(nvHat,rvHat);
            R = [rvHat tvHat nvHat]';
        end
        
    end
    
end



