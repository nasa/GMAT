classdef LaunchVehicle < handle

	% LaunchVehicle. This is a prototype launch vehicle class for GMAT design and testing
	% Written by Jeremy Knittel. jeremy.m.knittel@nasa.gov
	
	properties (SetAccess = private)
	
		launchVehicleType = 1; 
	    % -2: custom launch vehicle
	    % -1: burn with departure stage engine
	    % 0: fixed initial mass
	    % 1: Atlas V (401)	 NLSII
	    % 2: Atlas V (411)	NLSII
	    % 3: Atlas V (421)	NLSII
	    % 4: Atlas V (431)	NLSII
	    % 5: Atlas V (501)	NLSII
	    % 6: Atlas V (511)	NLSII
	    % 7: Atlas V (521)	NLSII
	    % 8: Atlas V (531)	NLSII
	    % 9: Atlas V (541)	NLSII
	    % 10: Atlas V (551)	NLSII
	    % 11: Falcon 9 (v1.0)	NLSII
	    % 12: Falcon 9 (v1.1)	NLSII
	    % 13: Atlas V (551) w/Star 48	NLSI
	    % 14: Falcon 9 Heavy - Not currently allowed due to proprietary nature of model
	    % 15: Delta IV Heavy	NLSI
	    % 16: SLS Block 1 - Not currently allowed due to proprietary nature of model
	
		a1 = [-1.80995E-07,   0.00E+00,    2.70E-07,   9.00E-08,  0.00000077, -0.00000044, -0.00000075,  0.00000069, -0.00000048,  -0.0000005,  0.00001949,  0.00000051, -3.5649E-08, 0, 8.0e-7, 0];
		a2 = [    2.81E-05,   5.62E-06,   -3.88E-05,  -2.48E-05, -0.00009382,  0.00006198,  0.00012334, -0.00010901,  0.00006465,  0.00007576, -0.00138695, -0.00004196, 2.31305E-05, 0,-0.0002, 0];
		a3 = [-0.001517208,-0.00067462,  0.00190765, 0.00211196,  0.00403555, -0.00295026, -0.00712978,  0.00596291, -0.00276635, -0.00405132,  0.03050117,  -0.0035373,-0.006458248, 0, 0.0145, 0];
		a4 = [ 0.357956243, 0.44268134,  0.43698409, 0.47075449,  0.26854604,  0.41398944,  0.61102598,  0.37650144,  0.60963424,  0.70733066,  0.30110723,  0.91375291, 1.065903806, 0, 0.5825, 0];
		a5 = [-64.48375826,-78.8465652,-88.38856438,-98.4962944,-55.39915501,-69.35547443,-83.52984026,-91.90752777,-102.9890546,-111.2601399,-69.96585082,-110.0132867,-114.5157292, 0,-183.69, 0];
		a6 = [ 3034.683258,3930.871041, 4655.158371,5235.260181,     2094.89,     3265.42,     4195.86,     4939.98,     5595.09,     6106.14,     1974.88,     3634.59, 6595.738063, 0,12170.0, 0];

		c3max = [60,60,60,60,50,60,60,60,60,60,35,40,200,0,100,0];
		
		customCoefficients = [0,0,0,0,0,0];
	
	end
	
	methods 
	
		function SetLaunchVehicleType(obj,inputValue)
	
			if inputValue > 16 || inputValue ~= round(inputValue) || inputValue == 14 || inputValue == 16 || inputValue == -1 || inputValue == 0
				disp('Invalid launch vehicle type.')
				stop
			end
	
			obj.launchVehicleType = inputValue;
	
		end
	
		function SetCustomCoefficients(obj,coefficients)
		
			if length(coefficients) ~= 6
				disp('Wrong length of custom launch vehicle coefficient vector')
				stop
			end
			
			obj.customCoefficients = coefficients;
			
		end
		
		function mass = GetMassToEscapeC3(obj,C3)
		

			if C3 < 0
				disp('Non escape trajectories not currently modelled.')
				stop
			end
			
			if obj.launchVehicleType == -2
				C32 = C3*C3;
				C33 = C3*C32;
				C34 = C3*C33;
				C35 = C3*C34;

				mass = obj.customCoefficients(1)*C35 + obj.customCoefficients(2)*C34 + obj.customCoefficients(3)*C33 + obj.customCoefficients(4)*C32 + obj.customCoefficients(5)*C3 + obj.customCoefficients(6); % - options->LV_adapter_mass;
			
			else 
			
				if C3 > obj.c3max(obj.launchVehicleType)
                    C3max = obj.c3max(obj.launchVehicleType); 
					expfun = exp(-(C3-C3max)/C3max);
					C32 = C3max*C3max;
					C33 = C3max*C32;
					C34 = C3max*C33;
					C35 = C3max*C34;
					mass = obj.a1(obj.launchVehicleType)*C35 + obj.a2(obj.launchVehicleType)*C34 + obj.a3(obj.launchVehicleType)*C33 + obj.a4(obj.launchVehicleType)*C32 + obj.a5(obj.launchVehicleType)*C3max + obj.a6(obj.launchVehicleType); % - options->LV_adapter_mass;
                    mass = mass * expfun;
				else
					C32 = C3*C3;
					C33 = C3*C32;
					C34 = C3*C33;
					C35 = C3*C34;
					mass = obj.a1(obj.launchVehicleType)*C35 + obj.a2(obj.launchVehicleType)*C34 + obj.a3(obj.launchVehicleType)*C33 + obj.a4(obj.launchVehicleType)*C32 + obj.a5(obj.launchVehicleType)*C3 + obj.a6(obj.launchVehicleType); % - options->LV_adapter_mass;
				end
		

			end
			
		end
	
	end
	
end