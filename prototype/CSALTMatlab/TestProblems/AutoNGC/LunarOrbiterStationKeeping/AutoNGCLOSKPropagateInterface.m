classdef AutoNGCLOSKPropagateInterface < exec.CSALTAPI
    %AutoNGCLOSKInterface Interface class for AutoNGC lunar orbit station keeping Use Case
    %
    % Author, S. Hughes.
    
    properties
    end
    
    methods
        
        function obj = AutoNGCLOSKPropagateInterface(cartState,initMassKg,sc,fm,fmMoon2body)
            % fm: gmat force model object for full fidelity
            % fmMoon2body: gmat force model object for just lunar 2 body
            % force
            % sc: gmat spacecraft object

            global traj % Required for SNOPT function wrapper. Can't pass a Trajectory to SNOPT.
            % Globalize variables used in the initial guess function
            % i don't think this actually needs to be global, does it?
            global initKepState
            % Import CSALT classes
            import exec.Trajectory
            import exec.RadauPhase
            
            %%
            
            %==========================================================================
            %% =====  Define Non-Dimensionalization Properties
            %==========================================================================

            % Define quantities used in non-dimensionalization and re-dimensionalization
            thrustNewtons = 40e-3; % thruster does 40 mN
            Isp = 40; % seconds
%             thrustNewtons = 40; % thruster does 40 mN
%             Isp = 200; % seconds
            gravParamMoon = 4902.8005821478; % km3/s2
            REMoon = 1738.2; % km
            massUnit = 150; % kg
            scaleUtil = OrbitScaleUtility(REMoon,massUnit,gravParamMoon);

            %==========================================================================
            %% ===== USER INPUTS: Define Desired Initial and Final State Values
            %==========================================================================
                
            % Initial state in classical Keplerian orbital elements
            % this also is used to set the initial guess for the time of
            % flight
            initKepState = Cart2Kep(cartState,gravParamMoon);
            meanMotion = sqrt(gravParamMoon/initKepState(1)^3);
            initPeriod = 2*pi/meanMotion;
            periodMultiple = 2; % guess 
            timeOfFlightInSeconds = periodMultiple*initPeriod; % [seconds]
            propDuration = timeOfFlightInSeconds/scaleUtil.TU; % scaled time of flight

            % Final state in classical Keplerian orbital elements
            finalKepState = [6500
                0.01
                deg2rad(90)
                deg2rad(45)
                deg2rad(0)
                deg2rad(180)]; % final true anomaly is not fixed, but i'm just giving it something to start with; hopefully CSALT can fix it up on its own
            
            % Convert the initial state from Keplerian OE to lunar equatorial Cartesian states
            % in Km and Km/s
            finalCartState = Kep2Cart(finalKepState,gravParamMoon);
            
            %==========================================================================
            %% =====  Define Properties for the Trajectory
            %==========================================================================
            
            %  Create trajectory and configure user function names
            traj = Trajectory();
            
            % Add the path, boundary function, and phases to the trajectory
            import AutoNGCLOSKPathObject
            import AutoNGCLOSKPointObject
            traj.pathFunctionObject = AutoNGCLOSKPathObject();
            traj.pointFunctionObject = AutoNGCLOSKPropagatePointObject();
            
            % Set plot configurations and other settings
            traj.plotFunctionName  = 'AutoNGCLOSKPlotFunction';
            traj.showPlot          = false();
            traj.plotUpdateRate    = 75;
            traj.costLowerBound    = -Inf;
            traj.costUpperBound    = Inf;
            traj.maxMeshRefinementCount = 5;
            traj.maxMajorIterations = 100;
            traj.refineInitialMesh = false;
            traj.optimalityTol = 1e-3;
            traj.feasibilityTol = 1e-6;
            
            %==========================================================================
            %% =====  Non-Dimensionalize and Set User Supplied Initial Guess
            %==========================================================================

            % Nondimensionalize and set the initial and final states of the transfer
            initModEqElems = Cart2Mee(cartState,gravParamMoon);
            finalModEqElems = Cart2Mee(finalCartState,gravParamMoon);
            initModEqElemsNonDim = scaleUtil.ScaleModEqElements(initModEqElems);
            finalModEqElemsNonDim = scaleUtil.ScaleModEqElements(finalModEqElems);
            traj.pointFunctionObject.SetInitialConditions([initModEqElemsNonDim' initMassKg/scaleUtil.MU])
            
            %==========================================================================
            %% =====  Define Properties for the Phase
            %==========================================================================
            
            % This is the guess for the phase start times which determines
            % the guess for the duration of the burn arcs.  Seems to work
            % well but could be better if needed. 
            phase1StartTime = 0.0; % scaled
            phase1StopTime = propDuration; % scaled
            
            %  Phase 1:
            %phase1 = exec.OrbitPhase(scaleUtil);
            phase1 = orbitPhaseAutoNGCLOSKGmatApi(scaleUtil); % derives from exec.OrbitPhase and adds stuff for Gmat Api usage
            phase1.SetRelErrorTol(1e-6);
            phase1.SetStateRep('ModEquinoctial');
            phase1.SetControlRep('CartesianFourVec');
            phase1.SetThrustMode('Coast');
            phase1.SetAvailableThrust(thrustNewtons);
            phase1.SetIsp(Isp);
            phase1.initialGuessMode = 'UserGuessObject';
            meshIntervalFractions = linspace(-1,1,4); %[-1 0 1];
            meshIntervalNumPoints   = 4*(ones(length(meshIntervalFractions)-1,1));
            
            % new stuff for gmat api:
            phase1.setFullForceModel(fm);
            phase1.setTwoBodyForceModel(fmMoon2body);
            phase1.setSpacecraft(sc);
            
            %  Set time bounds and gueses
            initialGuessTime  = phase1StartTime;
            finalGuessTime    = phase1StopTime;
            timeLowerBound    = 0;
            timeUpperBound    = 10000; % probably can make this smaller, but starting with it large for now
            
            %  Set state bounds and guesses
            numStateVars      = 7;
            stateLowerBound   = [0 -.99 -.99 -100 -100 -100 1e-3]';
            %  Warning: the values for state guesses assume you are NOT
            % using phase1.initialGuessMode = 'UserGuessObject';
            % Changing that mode will result failure to convert
            initialGuessState = [0 -.99 -.99 -100 -100 -100 1]';
            finalGuessState   = [ 100 0.99 0.99 100 100 100 1]';
            stateUpperBound   = [ 100 0.99 0.99 100 100 100 100]';
            
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
            
            
            
            % Add phases to the trajectory
            traj.phaseList = {phase1};
            % TODO: Should this be managed by the Trajectory?
            traj.pathFunctionObject.phaseList = {phase1};
            traj.pointFunctionObject.phaseList = {phase1};
            theGuess = AutoNGCLOSKGuess(initKepState,initMassKg);
            traj.guessObject = theGuess;
            
            obj.trajPointer = traj;
            
        end
        
        function PlotSolution(obj)

            % Extract collocation results from trajectory object
            times = obj.trajPointer.GetTimeArray(1);
            stateVec = obj.trajPointer.GetStateArray(1);
            controlVec = obj.trajPointer.GetControlArray(1);
            
            % Run script to plot results and print problem summary
            AutoNGCLOSKPlotFunction_MultiPhase(times,controlVec,stateVec, obj.trajPointer); 
            
        end
        
    end
    
end

