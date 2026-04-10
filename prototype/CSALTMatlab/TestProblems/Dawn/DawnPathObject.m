classdef DawnPathObject < userfunutils.OrbitPathFunction
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
            
                if strcmp(config.Control,'Spherical')
                    obj.ComputeSphericalControlOrbitalDynamics(obj.SEPForces);
                elseif strcmp(config.Control,'Cartesian')
                    obj.ComputeCartesianControlOrbitalDynamics(obj.SEPForces);
                end

            else
				
	            %  Re-Dimensionalize States/Time etc.
	            time = obj.GetTime()*obj.config.TU;
	            stateVec = obj.config.StateUnScaleMat*obj.GetStateVec();
	            controlVec = obj.GetControlVec();
	            Xdot(1:3,1) = stateVec(4:6,1);
				
                mu = 132712440017.99;
                controlMag_Newtons = .241/1000*.9;
                g = 9.81;
                Isp = 3666;
				
				% ux = sin(controlVec(1)) * cos(controlVec(2));
				% uy = sin(controlVec(1)) * sin(controlVec(2));
				% uz = cos(controlVec(1));
				
				if sum(isnan(controlVec))
					controlVec = zeros(3,1);
				elseif obj.GetNumControlVars() == 0
					controlVec = zeros(3,1);
				end
				
				if obj.GetNumControlVars()
		            obj.SetAlgFunctions([norm(controlVec)]);
		            obj.SetAlgFuncUpperBounds([1]);
		            obj.SetAlgFuncLowerBounds([0]);
					obj.SetAlgFuncNames({'Control Magnitude'});
					% controlVec = ocontrolVec;
				end
				
				ux = controlVec(1);
				uy = controlVec(2);
				uz = controlVec(3);
				
                Xdot(4:6,1) = -mu*stateVec(1:3,1)/sqrt(stateVec(1:3,1)'*stateVec(1:3,1))^3 + ...
                    (controlMag_Newtons)*[ux;uy;uz]/stateVec(7,1);
                Xdot(7,1) = -controlMag_Newtons*norm([ux;uy;uz])/(Isp*g/1000);
				   
	            obj.SetDynFunctions(obj.config.StateAccScaleMat*Xdot);
            end
                        
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end



