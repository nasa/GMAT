classdef BatchEstimator < handle

    % Set the public properties
    properties  (SetAccess = 'public')
        MaxIterations = 4;
        RelTolerance  = 1e-5;
        AbsTolerance  = 1e-5;
        Measurements  = {};
        SolveFor      = {};
        Propagator;
        ESV;
        SolutionEpoch;
    end

    % Set the private properties
    properties  (SetAccess = 'private')
        ESM;
        MeasManager;
        numStates;
    end

    % Set the methods
    methods

        %-----  Constructor
        function Estimator = BatchEstimator(Estimator)
            
            %  Create the estimator's state manager
            Estimator.ESM = EstimatorStateManager;
            Estimator.MeasManager = MeasurementManager;
            
        end
        
        %----- Initialize the state manager
        function Estimator = Initialize(Estimator,Sandbox)
            
            %--------------------------------------------------------------
            % - Get handle to propagator
            % - Initialize the estimator
            % - Initialize the estimator state manager
            % - Initialize the propagator state manager
            % - Intialize the measurement manager
            %--------------------------------------------------------------

            %  Get handle to propagator:  Replace string with handle!
            Estimator.Propagator = Sandbox.GetHandle(Estimator.Propagator);

            %  Initialize the ESM for the estimator
            Estimator.ESM = Estimator.ESM.Initialize(Sandbox,Estimator);
            
            %  Intialize the PSM for the propagator
            Estimator.Propagator.PSM.Initialize(Estimator.ESM);
            
            %  Initialize the measurement manager and measurements.
            %Estimator.Measurements{1} = Sandbox.GetHandle(Estimator.Measurements);
            %Estimator.Measurements{1} = Estimator.Measurements{1}.Initialize(Sandbox);
            %  Get handle to measurements:  Replace string with handle!
            Estimator.MeasManager.Initialize(Sandbox,Estimator);
           % Estimator.Measurements{1} = Sandbox.GetHandle(Estimator.Measurements);
            
            
        end % Initialize
        
    end % methods

end % classdef