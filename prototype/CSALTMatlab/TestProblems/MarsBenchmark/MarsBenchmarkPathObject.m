classdef MarsBenchmarkPathObject < userfunutils.OrbitPathFunction
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

            global config
            
            useHiFiDynamics = true();
            
            if useHiFiDynamics
				
				% if obj.GetPhaseNumber() == 1
				%     obj.UpdateAlgFuncUpperBounds([100]);
				%     obj.UpdateAlgFuncLowerBounds([7.1]);
				% 	obj.UpdateAlgFuncNames({'Usable Power'});
				% elseif obj.GetPhaseNumber() == 2
				%     obj.UpdateAlgFuncUpperBounds([7.1]);
				%     obj.UpdateAlgFuncLowerBounds([.6]);
				% 	obj.UpdateAlgFuncNames({'Usable Power'});
				% end

				obj.ComputeCartesianControlOrbitalDynamics(obj.SEPForces);

				% usablePower = obj.SEPForces.Spacecraft.GetThrustPower();
				% obj.UpdateUserAlgFunctions([usablePower]);
				
				
	            %  Re-Dimensionalize States/Time etc.
				% 	            time = obj.GetTime()*obj.config.TU;
				% 	            stateVec = obj.config.StateUnScaleMat*obj.GetStateVec();
				% 	            controlVec = obj.GetControlVec();
				% 	            Xdot(1:3,1) = stateVec(4:6,1);
				%
				%                 ifSparsity = obj.GetIsSparsity();
				%                 if ifSparsity
				%                     sharp0 = obj.SEPForces.Spacecraft.ThrusterSet.throttleHeavisideSharpness;
				%                     obj.SEPForces.Spacecraft.ThrusterSet.throttleHeavisideSharpness = 10;
				%                 end
				%                 % [Xdot,A]= obj.SEPForces.ComputeForcesGivenSphericalControl(time,stateVec,controlVec);
				%                 [Xdot,A]= obj.SEPForces.ComputeForcesGivenControl(time,stateVec,controlVec);
				% usablePower = obj.SEPForces.Spacecraft.GetThrustPower();
				%                 if ifSparsity
				%                     obj.SEPForces.Spacecraft.ThrusterSet.throttleHeavisideSharpness = sharp0;
				%                 end
				%
				% ux = controlVec(1);
				% uy = controlVec(2);
				% uz = controlVec(3);
				%
				% % 	            obj.SetAlgFunctions([norm([ux;uy;uz])]);
				% % 	            obj.SetAlgFuncUpperBounds([1]);
				% % 	            obj.SetAlgFuncLowerBounds([0]);
				% % obj.SetAlgFuncNames({'Control Magnitude'});
				%
				% % if obj.GetPhaseNumber() == 1
				% %     obj.SetAlgFunctions([usablePower]);
				% %     obj.SetAlgFuncUpperBounds([100]);
				% %     obj.SetAlgFuncLowerBounds([7.1]);
				% % 	obj.SetAlgFuncNames({'Usable Power'});
				% % elseif obj.GetPhaseNumber() == 2
				% %     obj.SetAlgFunctions([usablePower]);
				% %     obj.SetAlgFuncUpperBounds([7.1]);
				% %     obj.SetAlgFuncLowerBounds([.6]);
				% % 	obj.SetAlgFuncNames({'Usable Power'});
				% % end
				% if obj.GetPhaseNumber() == 1
				%     obj.SetAlgFunctions([usablePower;norm([ux;uy;uz])]);
				%     obj.SetAlgFuncUpperBounds([100;1]);
				%     obj.SetAlgFuncLowerBounds([7.1;0]);
				% 	obj.SetAlgFuncNames({'Usable Power','Control Magnitude'});
				% elseif obj.GetPhaseNumber() == 2
				%     obj.SetAlgFunctions([usablePower;norm([ux;uy;uz])]);
				%     obj.SetAlgFuncUpperBounds([7.1;1]);
				%     obj.SetAlgFuncLowerBounds([.6;0]);
				% 	obj.SetAlgFuncNames({'Usable Power','Control Magnitude'});
				% end
				%
				%
				% obj.SetDynFunctions(obj.config.StateAccScaleMat*Xdot);
            else
				
	            %  Re-Dimensionalize States/Time etc.
	            time = obj.GetTime()*obj.config.TU;
	            stateVec = obj.config.StateUnScaleMat*obj.GetStateVec();
	            controlVec = obj.GetControlVec();
	            Xdot(1:3,1) = stateVec(4:6,1);
				
                mu = 132712440017.99;
                controlMag_Newtons = .241/1000*.9*2;
                g = 9.81;
                Isp = 3666;
				
				% ux = sin(controlVec(1)) * cos(controlVec(2));
				% uy = sin(controlVec(1)) * sin(controlVec(2));
				% uz = cos(controlVec(1));
				
				ux = controlVec(1);
				uy = controlVec(2);
				uz = controlVec(3);
				
                Xdot(4:6,1) = -mu*stateVec(1:3,1)/sqrt(stateVec(1:3,1)'*stateVec(1:3,1))^3 + ...
                    (controlMag_Newtons)*[ux;uy;uz]/stateVec(7,1);
                Xdot(7,1) = -controlMag_Newtons*norm([ux;uy;uz])/(Isp*g/1000);
				
	            obj.SetAlgFunctions([norm([ux;uy;uz])]);
	            obj.SetAlgFuncUpperBounds([1]);
	            obj.SetAlgFuncLowerBounds([0]);
				obj.SetAlgFuncNames({'Control Magnitude'});
                        
	            obj.SetDynFunctions(obj.config.StateAccScaleMat*Xdot);
            end
                        
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end



