classdef DROtoL2PointObject < userfunutils.UserPointFunction
    %DROtoL2PointObject Computes boundary functions for DRO to L2 problem
    %  July 2016, N. Parrish.  Original Version
    %  July 2019, S. Hughes, Modified for tutorial.  Cleaned up code
      
    properties
    end
    
    methods
        
        function EvaluateFunctions(obj)
            % Compute the boundary functions
            
            % Extract contants
            global constants Nphase
            MU = constants.MU;
            TU = constants.TU;
            DU = constants.DU;
            Isp = constants.Isp;
            g0 = constants.g0;
            mscale = constants.mscale;
            PERIOD_DRO = constants.PERIOD_DRO;
            PERIOD_L2Halo = constants.PERIOD_L2Halo;
            day = constants.day;
            m0 = constants.m0;
            thrust_limit = constants.thrust_limit;
            min_dist_moon = constants.min_dist_moon;
            min_dist_earth = constants.min_dist_earth;
            
            %% Define endpoints
            initialEpochCon = 0; %TU
            finalEpochCon = 4.50492518; %TU
            initialStateCon =[ 1.07730946
                0
                0
                0
                -0.46973763
                0
                1.5]; %DU, VU, MU
            finalStateCon = [ 1.11989122
                -0.11468544
                -0.08789384
                -0.09155242
                -0.10503982
                0.1516538
                1.5]; %DU, VU, MU
            
            % Extract initial and final states
            % Note, based on setup state is [X Y Z VX VY VZ M]
            % in DU, VU, MU
            initialState = obj.GetInitialStateVec(1);
            finalState = obj.GetFinalStateVec(Nphase);
            
            % Compute the cost function, the propellant mass used. 
            mass_propellant = (initialState(7) - finalState(7))*mscale;
            obj.SetCostFunction(mass_propellant); 
            
            %% Compute the boundary constraints
            
            % Initialize the boundary constraints
            % (pos, vel, mass) at Nphase match points. (pos, vel) at end.
            stateError = NaN * ones( 7*Nphase + 6, 1 );
            timeError = NaN * ones( Nphase + 1, 1);
          
            % Compute errors in the initial state and time
            stateError(1:7) = initialState - initialStateCon;
            timeError(1) = obj.GetInitialTime(1) - initialEpochCon;
            
            % Compute errors in the final state and time
            stateError(end-5:end) = finalStateCon(1:6) - finalState(1:6);
            timeError(end) = obj.GetFinalTime(Nphase) - finalEpochCon;
                       
            % Set the constraint values 
            obj.SetAlgFunctions([timeError; stateError]);
            
            % If initializing, set bounds.  
            if obj.IsInitializing()
                % Boundary errors should be zero, so set bounds arrays to 
                % all zeros.
                AlgFuncLB = zeros(length(timeError) + length(stateError), 1);
                AlgFuncUB = zeros(length(timeError) + length(stateError), 1);
                obj.SetAlgFuncLowerBounds(AlgFuncLB);
                obj.SetAlgFuncUpperBounds(AlgFuncUB);
            end
            
        end
        
        function EvaluateJacobians(~)
            
        end
    end
    
end

