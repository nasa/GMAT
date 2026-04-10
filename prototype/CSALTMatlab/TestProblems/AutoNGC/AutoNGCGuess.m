classdef  AutoNGCGuess < userfunutils.UserGuessInterface
    %AutoNGCGuessObject Computes guess for auto NGC use case
    
    properties
        % The initial mass of the spacecraft
        initMassKg = 1000;
        % The initial keplerian elements
        initKepState
        % Mu
        gravParam = 398600.4415;
    end
    
    methods
        
        function obj = AutoNGCGuess(state,mass)
            % Constructor
            
            % Initial mass in kg
            obj.initMassKg = mass;
            % Initial keplerian state in Km and Rad.
            obj.initKepState = state;
            
        end
        
        function [stateGuess,controlGuess] = GetInitialGuess(obj,timeVector,~,phasePointer)
            % Computes initial guess for states and controls
            
            persistent revCounter lastAnomaly firstCall
            if isempty(revCounter)
                % Initialize revolution counter
                revCounter = 0;
                lastAnomaly = inf;
                firstCall = true;
            end
            
            scaleUtil = phasePointer.orbitScaleUtil;
            
            % Compute number of times and pre-allocate the guess arrays
            numTimes = size(timeVector,1);
            stateGuess = zeros(numTimes,6);
            controlGuess = zeros(numTimes-1,4);
            
            %% Compute Initial Guess for States
            
            % Loop through each time in input time vector
            kepState = obj.initKepState;
            for ii = 1:numTimes
                
                % Compute true anomaly after propagation duration
                finalTrueAnomaly = obj.KeplerProp(timeVector(ii)*scaleUtil.TU);

                % Convert Keplerian orbital elements at current time step to Cartesian
                % coordinates in the inertial frame
                kepState(1:5) = obj.initKepState(1:5);
                kepState(6) = finalTrueAnomaly;
                cartState = Kep2Cart(kepState,scaleUtil.gravParam);
                
                % Convert to Mod. Eq, using Cartesian intermediate state
                [modEqState] = Cart2Mee(cartState,scaleUtil.gravParam);
                stateGuess(ii,:) = modEqState';
                stateGuess(ii,1) = stateGuess(ii,1)/scaleUtil.DU; % nondimensionalize length
                
                % Ensure true anomaly/true longitude increases monotonically
                L_ii = 2*pi*revCounter + stateGuess(ii,6); % update true longitude value at current time according to the number of completed revs
                if ~firstCall
                    if lastAnomaly > L_ii % if true longitude value of previous iteration is greater than the true longitude at the current iteration
                        revCounter = revCounter + 1; % iterate rev counter
                        L_ii = 2*pi + stateGuess(ii,6); % update true longitude value at current time according to the number of completed revs
                    end
                    lastAnomaly = L_ii;
                    stateGuess(ii,6) = L_ii; % update true longitude value at current time in stateGuess matrix
                else
                    firstCall = false;
                    lastAnomaly = L_ii;
                end
                
            end % end loop through times in input time vector
            
            % Add mass state to initial guess
            stateGuess = [stateGuess, (obj.initMassKg/scaleUtil.MU).*ones(numTimes,1)];
            
            %% Compute Initial Guess for Controls
            for ii = 1:numTimes-1
                controlGuess(ii,:) = [1 0 1 0]; % indicates that
            end
            
        end
        
        function finalTrueAnomaly = KeplerProp(obj,deltaT)
            % Propagates the true anomaly of the initial state through 
            % a delta time
            meanMotion = sqrt(obj.gravParam/obj.initKepState(1)^3);
            initMeanAnomaly = nu2M(obj.initKepState(6),obj.initKepState(2));
            finalMeanAnomaly = initMeanAnomaly + meanMotion*deltaT;
            finalEccAnomaly = Kepler(finalMeanAnomaly,obj.initKepState(2));
            finalTrueAnomaly = E2nu(finalEccAnomaly,obj.initKepState(2));
        end
       
    end
end