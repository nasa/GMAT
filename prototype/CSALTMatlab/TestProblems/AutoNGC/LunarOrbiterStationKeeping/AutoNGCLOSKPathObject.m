classdef AutoNGCLOSKPathObject < userfunutils.OrbitPathFunc
    % Path functions for the AutoNGC lunar orbit station keeping problem
    
    properties
        
    end
    
    methods (Access = public)
        
        function EvaluateFunctions(obj)
            
            % Compute and set the dynamics
            obj.SetDynamicsGmatApi();
            
            % Compute and set the control path constraint
            % says that magnitude of control elements (2:4) must be 1,
            % which is still true; don't need to change
            % i THINK the current setup allows for throttling, too, but i'm
            % not 100% sure
            obj.SetControlPathConstraint();
            
        end
        
        function EvaluateJacobians(~)
            
            % The Jacobian is evaluated via numerical differencing for this
            % problem, not a user supplied function.
            
        end
        
        function SetDynamicsGmatApi(obj)
            % evaluate dynamics using gmat api to get natural perturbations
            
            phaseIdx = obj.GetPhaseNumber();
            scaleUtil = obj.phaseList{phaseIdx}.orbitScaleUtil;
            gravParam = 1.0;
            gravParamUnscaled = scaleUtil.gravParam;
            
            % use existing utility to compute thrust
            [thrustVec, mdot] = ComputeThrust(obj); % outputs are scaled
            
            % need to get the non-2-body acceleration from gmat api and
            % convert it to the frame used by the modified equinoctial
            % elements
            
            % need unscaled cartesian state and time and mass to pass to gmat api
            stateVec = obj.GetStateVec(); % scaled 7 state in MEE       
            t = obj.GetTime(); % scaled time

            % unscale time and state
            tUnscaled = scaleUtil.UnscaleTime(t);
            stateVecUnscaled = scaleUtil.UnScaleModEqFullState(stateVec); % unscaled MEE state and mass
            stateCartUnscaled = Mee2Cart(stateVecUnscaled(1:6), gravParamUnscaled); % unscaled cartesian 6 state
            
            useGmatApi = 0;
            if useGmatApi == 1
                for i=1:6
                    if isnan(stateCartUnscaled(i)) || isinf(stateCartUnscaled(i))
                        useGmatApi = 0;
                        break
                    end
                end
            end
            if useGmatApi == 1
                sc = obj.phaseList{phaseIdx}.sc;
                fm = obj.phaseList{phaseIdx}.fm;
                fmMoon2body = obj.phaseList{phaseIdx}.fm2body;
                
                % set gmat api spacecraft dry mass and reinitialize
                sc.SetField("DryMass", stateVecUnscaled(7));
                gmat.gmat.Initialize();

                % full acceleration
                fm.GetDerivatives(stateCartUnscaled, tUnscaled);
                dxFullUnscaled = fm.GetDerivativeArray();

                % 2-body acceleration
                fmMoon2body.GetDerivatives(stateCartUnscaled, tUnscaled);
                dx2bodyUnscaled = fmMoon2body.GetDerivativeArray();

                % unscaled cartesian acceleration in lunar equatorial frame
                accPertUnscaled = dxFullUnscaled(4:6) - dx2bodyUnscaled(4:6);

                % scale the perturbing acceleration
                accPert = scaleUtil.ScaleAcc(accPertUnscaled);

                % convert to RTN frame
                xyz2rtnMat = XYZ2RTN(stateCartUnscaled(1:3), stateCartUnscaled(4:6)); % get transformation matrix
                accPertRTN = xyz2rtnMat * accPert;
            else
                accPertRTN = [0;0;0];
            end
            
            % combine natural perturbations and thrust
            totalPertRTN = accPertRTN + thrustVec; % total scaled non-2-body acceleration in RTN
            
            % get MEE derivatives
            odeRHS = zeros(7,1);
            dynFunc = ModEqDynamics(gravParam);
            odeRHS(1:6,1) = dynFunc.ComputeOrbitDerivatives(stateVec, totalPertRTN);
            odeRHS(7,1) = -mdot;
            
            % set dynamics
            obj.SetDynFunctions(odeRHS);
        end
        
    end
    
end



