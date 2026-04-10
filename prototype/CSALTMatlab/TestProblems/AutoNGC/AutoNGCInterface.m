classdef AutoNGCInterface < exec.CSALTAPI
    %AutoNGCInterface Interface class for AutoNGC Use Case
    %
    % Author, S. Hughes.
    
    properties
    end
    
    methods
        
        function obj = AutoNGCInterface(cartState,initMassKg)

            global traj % Required for SNOPT function wrapper.. Can't pass a Trajectory to SNOPT.
            % Globalize variables used in the initial guess function
            global initKepState
            % Import CSALT classes
            import exec.Trajectory
            import exec.RadauPhase
            
            %==========================================================================
            %% =====  Define Non-Dimensionalization Properties
            %==========================================================================

            % Define quanitities used in non-dimensionalization and re-dimensionalization
            thrustNewtons = 100;
            Isp = 220;
            gravParam = 398600.4415;
            RE = 6378.1363;
            massUnit = 1000;
            scaleUtil = OrbitScaleUtility(RE,massUnit,gravParam);

            %==========================================================================
            %% ===== USER INPUTS: Define Desired Initial and Final State Values
            %==========================================================================
                
            % Initial state in classical Keplerian orbital elements
            initKepState = Cart2Kep(cartState,gravParam);
            meanMotion = sqrt(gravParam/initKepState(1)^3);
            initPeriod = 2*pi/meanMotion;
            periodMultiple = 0.5;
            timeOfFlightInSeconds = periodMultiple*initPeriod; % [seconds]
            propDuration = timeOfFlightInSeconds/scaleUtil.TU;

            % Final state in classical Keplerian orbital elements
            delt_SMA = 10; % [km], define desired change in SMA
            delt_ECC = 0.00; % define desired change in ECC
            delt_INC = deg2rad(0); % define desired change in inclination
            delt_RAAN = deg2rad(0); % define desired change in RAAN
            delt_AOP = deg2rad(0); % define desired change in AOP
            finalKepState = [initKepState(1) + delt_SMA
                initKepState(2) + delt_ECC
                initKepState(3) + delt_INC
                initKepState(4) + delt_RAAN
                initKepState(5) + delt_AOP
                initKepState(6) + pi];
            
            % Convert the initial state from Keplerian OE to EMEJ2000 Cartesian states
            % in Km and Km/s
            finalState = Kep2Cart(finalKepState,gravParam);
            
            %==========================================================================
            %% =====  Define Properties for the Trajectory
            %==========================================================================
            
            %  Create trajectory and configure user function names
            traj = Trajectory();
            
            % Add the path, boundary function, and phases to the trajectory
            import AutoNGCPathObject
            import AutoNGCPointObject
            traj.pathFunctionObject = AutoNGCPathObject();
            traj.pointFunctionObject = AutoNGCPointObject();
            
            % Set plot configurations and other settings
            traj.plotFunctionName  = 'AutoNGCPlotFunction';
            traj.showPlot          = false();
            traj.plotUpdateRate    = 75;
            traj.costLowerBound    = -Inf;
            traj.costUpperBound    = Inf;
            traj.maxMeshRefinementCount = 0;
            traj.maxMajorIterations = 300;
            traj.refineInitialMesh = false;
            traj.optimalityTol = 1e-3;
            traj.feasibilityTol = 1e-6;
            
            %==========================================================================
            %% =====  Non-Dimensionalize and Set User Supplied Initial Guess
            %==========================================================================

            % Nondimensionalize and set the initial and final states of the transfer
            initModEqElems = Cart2Mee(cartState,gravParam);
            initModEqElemsNonDim = scaleUtil.ScaleModEqElements(initModEqElems);
            traj.pointFunctionObject.SetInitialConditions([initModEqElemsNonDim' initMassKg/scaleUtil.MU])
            traj.pointFunctionObject.SetFinalCartState(finalState)
            
            %==========================================================================
            %% =====  Define Properties for the Phase
            %==========================================================================
            
            % This is the guess for the phase start times which determines
            % the guess for the duration of the burn arcs.  Seems to work
            % well but could be better if needed. 
            phase1StartTime = 0.0;
            phase1StopTime = 0.01*propDuration;
            phase2StopTime = 0.99*propDuration;
            phase3StopTime = propDuration;
            
            %  Phase 1:
            phase1 = exec.OrbitPhase(scaleUtil);
            phase1.SetStateRep('ModEquinoctial');
            phase1.SetControlRep('CartesianFourVec');
            phase1.SetThrustMode('Thrust')
            phase1.SetAvailableThrust(thrustNewtons);
            phase1.SetIsp(Isp);
            phase1.initialGuessMode = 'UserGuessObject';
            meshIntervalFractions = [-1 0 1];
            meshIntervalNumPoints   = 3*(ones(length(meshIntervalFractions)-1,1));
            
            %  Set time bounds and gueses
            initialGuessTime  = phase1StartTime;
            finalGuessTime    = phase1StopTime;
            timeLowerBound    = 0;
            timeUpperBound    = 100;
            
            %  Set state bounds and guesses
            numStateVars      = 7;
            stateLowerBound   = [-100 -100 -100 -100 -100 -100 -100]';
            %  Warning: the values for state guesses assume you are NOT
            % using phase1.initialGuessMode = 'UserGuessObject';
            % Changing that mode will result failure to convert
            initialGuessState = [-100 -100 -100 -100 -100 -100 -100]';
            finalGuessState   = [ 100 100 100 100 100 100 100]';
            stateUpperBound   = [ 100 100 100 100 100 100 100]';
            
            %  Set control bounds (guess is set to 1s)
            numControlVars    = 4;
            controlUpperBound = [1  2  2  2]';
            controlLowerBound = [1 -2 -2 -2]';
            
            % Call set methods to configure the data
            phase1.SetNumStateVars(numStateVars);
            phase1.SetNumControlVars(numControlVars);
            phase1.SetMeshIntervalFractions(meshIntervalFractions);
            phase1.SetMeshIntervalNumPoints(meshIntervalNumPoints);
            phase1.SetStateLowerBound(stateLowerBound)
            phase1.SetStateUpperBound(stateUpperBound)
            phase1.SetStateInitialGuess(initialGuessState)
            phase1.SetStateFinalGuess(finalGuessState)
            phase1.SetTimeLowerBound(timeLowerBound)
            phase1.SetTimeUpperBound(timeUpperBound)
            phase1.SetTimeInitialGuess(initialGuessTime)
            phase1.SetTimeFinalGuess(finalGuessTime)
            phase1.SetControlLowerBound(controlLowerBound)
            phase1.SetControlUpperBound(controlUpperBound)
            phase1.SetRefineInitialMesh(true); % this option allows mesh refinement to be performed before the first collocation iteration; it can improve a very poor initial guess
            
            %  Phase 1:
            phase2 = exec.OrbitPhase(scaleUtil);
            phase2.SetStateRep('ModEquinoctial');
            phase2.SetControlRep('CartesianFourVec');
            phase2.SetThrustMode('Coast')
            phase2.SetAvailableThrust(thrustNewtons);
            phase2.SetIsp(Isp);
            phase2.initialGuessMode = 'UserGuessObject';
            meshIntervalFractions = [-1  0  1];
            meshIntervalNumPoints   = 5*(ones(length(meshIntervalFractions)-1,1));
            
            %  Set time bounds and gueses
            initialGuessTime  = phase1StopTime ;
            finalGuessTime    = phase2StopTime ;
            timeLowerBound    = 0;
            timeUpperBound    = 100;
            
            %  Set state bounds and guesses
            numStateVars      = 7;
            stateLowerBound   = [-100 -100 -100 -100 -100 -100 -100]';
            %  Warning: the values for state guesses assume you are NOT
            % using phase2.initialGuessMode = 'UserGuessObject';
            % Changing that mode will result failure to convert
            initialGuessState = [-100 -100 -100 -100 -100 -100 -100]';
            finalGuessState   = [ 100 100 100 100 100 100 100]';
            stateUpperBound   = [ 100 100 100 100 100 100 100]';
            
            %  Set control bounds (guess is set to 1s)
            numControlVars    = 4;
            controlUpperBound = [0  2  2  2]';
            controlLowerBound = [0 -2 -2 -2]';
            
            % Call set methods to configure the data
            phase2.SetNumStateVars(numStateVars);
            phase2.SetNumControlVars(numControlVars);
            phase2.SetMeshIntervalFractions(meshIntervalFractions);
            phase2.SetMeshIntervalNumPoints(meshIntervalNumPoints);
            phase2.SetStateLowerBound(stateLowerBound)
            phase2.SetStateUpperBound(stateUpperBound)
            phase2.SetStateInitialGuess(initialGuessState)
            phase2.SetStateFinalGuess(finalGuessState)
            phase2.SetTimeLowerBound(timeLowerBound)
            phase2.SetTimeUpperBound(timeUpperBound)
            phase2.SetTimeInitialGuess(initialGuessTime)
            phase2.SetTimeFinalGuess(finalGuessTime)
            phase2.SetControlLowerBound(controlLowerBound)
            phase2.SetControlUpperBound(controlUpperBound)
            phase2.SetRefineInitialMesh(true); % this option allows mesh refinement to be performed before the first collocation iteration; it can improve a very poor initial guess
            
            %  Phase 1:
            phase3 = exec.OrbitPhase(scaleUtil);
            phase3.SetStateRep('ModEquinoctial');
            phase3.SetControlRep('CartesianFourVec');
            phase3.SetThrustMode('Thrust')
            phase3.SetAvailableThrust(thrustNewtons);
            phase3.SetIsp(Isp);
            phase3.initialGuessMode = 'UserGuessObject';
            meshIntervalFractions = [-1 0 1];
            meshIntervalNumPoints   = 3*(ones(length(meshIntervalFractions)-1,1));
            
            %  Set time bounds and gueses
            initialGuessTime  = phase2StopTime ;
            finalGuessTime    = phase3StopTime;
            timeLowerBound    = 0;
            timeUpperBound    = 100;
            
            %  Set state bounds and guesses
            numStateVars      = 7;
            stateLowerBound   = [-100 -100 -100 -100 -100 -100 -100]';
            %  Warning: the values for state guesses assume you are NOT
            % using phase3.initialGuessMode = 'UserGuessObject';
            % Changing that mode will result failure to convert
            initialGuessState = [-100 -100 -100 -100 -100 -100 -100]';
            finalGuessState   = [ 100 100 100 100 100 100 100]';
            stateUpperBound   = [ 100 100 100 100 100 100 100]';
            
            %  Set control bounds (guess is set to 1s)
            numControlVars    = 4;
            controlUpperBound = [1  2  2  2]';
            controlLowerBound = [1 -2 -2 -2]';
            
            % Call set methods to configure the data
            phase3.SetNumStateVars(numStateVars);
            phase3.SetNumControlVars(numControlVars);
            phase3.SetMeshIntervalFractions(meshIntervalFractions);
            phase3.SetMeshIntervalNumPoints(meshIntervalNumPoints);
            phase3.SetStateLowerBound(stateLowerBound)
            phase3.SetStateUpperBound(stateUpperBound)
            phase3.SetStateInitialGuess(initialGuessState)
            phase3.SetStateFinalGuess(finalGuessState)
            phase3.SetTimeLowerBound(timeLowerBound)
            phase3.SetTimeUpperBound(timeUpperBound)
            phase3.SetTimeInitialGuess(initialGuessTime)
            phase3.SetTimeFinalGuess(finalGuessTime)
            phase3.SetControlLowerBound(controlLowerBound)
            phase3.SetControlUpperBound(controlUpperBound)
            phase3.SetRefineInitialMesh(true); % this option allows mesh refinement to be performed before the first collocation iteration; it can improve a very poor initial guess
            
            % Add phase 1 to the trajectory
            traj.phaseList = {phase1,phase2,phase3};
            % TODO: Should this be managed by the Trajectory?
            traj.pathFunctionObject.phaseList = {phase1,phase2,phase3};
            traj.pointFunctionObject.phaseList = {phase1,phase2,phase3};
            theGuess = AutoNGCGuess(initKepState,initMassKg);
            traj.guessObject = theGuess;
            
            obj.trajPointer = traj;
            
        end
        
        function PlotSolution(obj)

            % Extract collocation results from trajectory object
            times = obj.trajPointer.GetTimeArray(1);
            stateVec = obj.trajPointer.GetStateArray(1);
            controlVec = obj.trajPointer.GetControlArray(1);
            
            % Run script to plot results and print problem summary
            AutoNGCPlotFunction_MultiPhase(times,controlVec,stateVec); 
            
        end
        
    end
    
end

