classdef ProblemCharacteristics < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    %  TODO: Validate mesh interval fractions and mesh interval points
    
    properties (Access = private)
        
        % ===== Optimal control parameter dimensions
        
        % Integer. The number of state variables in opt. control prob.
        numStateVars = 0;
        % Integer. The number of control variables in opt. control prob.
        numControlVars = 0;
        % Integer.  The number of integral variables in opt. control prob.
        numIntegralVars = 0
        % Ingeter.  The number of static variables in opt. control prob.
        numStaticVars = 0
        % Ingeter.  The number of time variables in opt. control prob.
        numTimeVars = 2;
        
        % ===== NLP parameter dimensions
        
        %  Integer. The number of state variables in the NLP problem
        numStateVarsNLP
        %  Integer. The number of time variables in the NLP problem
        numTimeVarsNLP = 2;
        %  Integer. The number of control variables in the NLP problem
        numControlVarsNLP
        %  Integer. The total number of decision vars in the NLP problem
        numDecisionVarsNLP
        % Integer. Number of defect constraints in the NLP problem
        numDefectConNLP
        % Integer. Number of total contraints in NLP problem
        numTotalConNLP

        % ===== Number of variables by type in optimal control problem
        
        %  Bool.  Indicates whether phase has state variables
        hasStateVars = false()
        %  Bool.  Indicates whether phase has control variables
        hasControlVars = false()
        %  Bool.  Indicates whether phase has integral variables
        hasIntegralVars = false()
        %  Bool.  Indicates whether phase has static variables
        hasStaticVars = false()
        %  Bool.  Indicates whether phase has time variables
        hasTimeVars = false()
        
        % ===== Flags describing function types in optimal control problem
        
        %  Bool.  Indicates whether phase has defect constraints
        hasDefectCons = false()
        %  Bool.  Indicates whether a phase has integral constraints
        hasIntegralCons = false()
        %  Bool.  Indicates whether phase has algebraic path constraints
        hasAlgPathCons = false()
        %  Bool. Flag to indicate if phase has a cost function
        hasIntegralCost = false();
        %  Bool.  Flag indicating there is an algebraic term to cost
        hasAlgebraicCost = false();
        
        % ==== Discretization properties
        %  Real array.  Vector defining the number of mesh intervalas and
        %  how the phase is partioned among them
        % For example, [-1 -.5 0 0.5 1] (for Radau) means there 4 equally
        % sized segements in the phase.
        % For HS, [0 0.5 1] means there are two equally sized segments in
        % the phase.
        meshIntervalFractions
        % Integer Array. Row number of control points per mesh interval
        meshIntervalNumPoints
        % Integer.  Number of mesh intervals
        numMeshIntervals
        
        % ==== State related quantities
        
        % Real array. Lower bound on state vector
        stateLowerBound
        % Real array.  Upper bound on state vector
        stateUpperBound
        % Real array. State guess at initial time
        stateInitialGuess
        % Real array. State guess at final time
        stateFinalGuess
        % Real. Time guess at beginning of phase
        timeInitialGuess
        % Real. Time guess at end of phase
        timeFinalGuess
        % Real. Lower bound on time
        timeLowerBound
        % Real. Upper bound on time
        timeUpperBound
        
        % ==== Control related quantities
        % Real array.  ower bound on control vector
        controlUpperBound
        % Real array. Upper bound on control vector
        controlLowerBound

    
    end
    
    methods (Access = public)
        
        % ===== Optimal control parameter dimensions
        
        function SetNumStateVars(obj,numStateVars)
            % Sets number of state vars and hasStateVars flag
            if numStateVars <= 0
                error('numStateVars cannot be negative')
            end
            obj.numStateVars = numStateVars;
            if obj.numStateVars > 0
                obj.hasStateVars = true;
            else
                obj.hasStateVars = false;
            end
        end
        
        function numStateVars = GetNumStateVars(obj)
            % Returns number of state variables in optimal control problem
            numStateVars = obj.numStateVars;
        end
        
        function hasStateVars = GetHasStateVars(obj)
            % Returns bool indicating if problem has state vars
            hasStateVars = obj.hasStateVars;
        end
        
        function SetNumControlVars(obj,numControlVars)
            % Sets number of control vars and hasControlVars flag
            if numControlVars < 0
                error('numControlVars cannot be negative')
            end
            obj.numControlVars = numControlVars;
            if obj.numControlVars > 0
                obj.hasControlVars = true;
            else
                obj.hasControlVars = false;
            end
        end
        
        function numControlVars = GetNumControlVars(obj)
            % Returns number of control variables in optimal control problem
            numControlVars = obj.numControlVars;
        end
        
        function hasControlVars = GetHasControlVars(obj)
            % Returns bool indicating if problem has control vars
            hasControlVars = obj.hasControlVars;
        end
        
        function SetNumIntegralVars(obj,numIntegralVars)
            % Sets number of integral vars and hasIntegralVars flag
            if numIntegralVars <= 0
                error('numIntegralVars cannot be negative')
            end
            obj.numIntegralVars = numIntegralVars;
            if obj.numIntegralVars > 0
                obj.hasIntegralVars = true;
            else
                obj.hasIntegralVars = false;
            end
        end
        
        function numIntegralVars = GetNumIntegralVars(obj)
            % Returns number of integral variables in optimal control problem
            numIntegralVars = obj.numIntegralVars;
        end
        
        function hasIntegralVars = GetHasIntegralVars(obj)
            % Returns bool indicating if problem has integral vars
            hasIntegralVars = obj.hasIntegralVars;
        end
        
        function SetNumStaticVars(obj,numStaticVars)
            % Sets number of static vars and hasStaticVars flag
            if numStaticVars <= 0
                error('numStaticVars cannot be negative')
            end
            obj.numStaticVars = numStaticVars;
            if obj.numStaticVars > 0
                obj.hasStaticVars = true;
            else
                obj.hasStaticVars = false;
            end
        end
        
        function numStaticVars = GetNumStaticVars(obj)
            % Returns number of static vars in optimal control problem
            numStaticVars = obj.numStaticVars;
        end
        
        function hasStaticVars = GetHasStaticVars(obj)
            % Returns bool indicating if problem has static vars
            hasStaticVars = obj.hasStaticVars;
        end
        
        %  numTimeVars is always two, so no set routine
        function numTimeVars = GetNumTimeVars(obj)
            % Returns number of time variables in optimal control problem
            numTimeVars = obj.numTimeVars;
        end
        
        % ===== Optimal control parameter dimensions
        
        function SetNumStateVarsNLP(obj,numStateVarsNLP)
            % Sets number of state decision vars in NLP
            if numStateVarsNLP <= 0
                error('numStateVarsNLP cannot be negative')
            end
            obj.numStateVarsNLP = numStateVarsNLP;
        end
        
        function numStateVarsNLP = GetNumStateVarsNLP(obj)
            % Returns number of state decision vars in NLP
            numStateVarsNLP = obj.numStateVarsNLP;
        end
        
        function SetNumControlVarsNLP(obj,numControlVarsNLP)
            % Sets number of control decision vars in NLP
            if numControlVarsNLP < 0
                error('numControlVarsNLP cannot be negative')
            end
            obj.numControlVarsNLP = numControlVarsNLP;
        end
        
        function numControlVarsNLP = GetNumControlVarsNLP(obj)
            % Returns number of control decision vars in NLP
            numControlVarsNLP = obj.numControlVarsNLP;
        end
        
        %  numTimeVars is always two, so no set routine
        function numTimeVarsNLP = GetNumTimeVarsNLP(obj)
            % Returns number of time decision vars in NLP
            numTimeVarsNLP = obj.numTimeVarsNLP;
        end
        
        function SetNumDecisionVarsNLP(obj,numDecisionVarsNLP)
            % Sets total number of decision vars in NLP
            if numDecisionVarsNLP <= 0
                error('numDecisionVarsNLP cannot be negative')
            end
            obj.numDecisionVarsNLP = numDecisionVarsNLP;
        end
        
        function numDecisionVarsNLP = GetNumDecisionVarsNLP(obj)
            % Returns total number of decision vars in NLP
            numDecisionVarsNLP = obj.numDecisionVarsNLP;
        end
        
        function SetNumDefectConNLP(obj,numDefectConNLP)
            obj.numDefectConNLP = numDefectConNLP;
        end
        
        function numDefectConNLP = GetNumDefectConNLP(obj)
            numDefectConNLP = obj.numDefectConNLP;
        end
        
        function SetNumTotalConNLP(obj,numTotalConNLP)
           obj.numTotalConNLP = numTotalConNLP; 
        end
        
        function numTotalConNLP = GetNumTotalConNLP(obj)
            numTotalConNLP = obj.numTotalConNLP;
        end
        
        % ===== Get\Set for optimal control function types
        
        function SetHasDefectCons(obj,hasDefectCons)
            % Sets flag to indicate if problem has defect constraints
            obj.hasDefectCons = hasDefectCons;
        end
        
        function hasDefectCons = GetHasDefectCons(obj)
            % Returns flag indicating if problem has defect constraints
            hasDefectCons = obj.hasDefectCons;
        end
        
        function SetHasAlgPathCons(obj,hasAlgPathCons)
            % Sets flag to indicate if problem has algebraic path constraints
            obj.hasAlgPathCons = hasAlgPathCons;
        end
        
        function hasAlgPathCons = GetHasAlgPathCons(obj)
            % Returns flag indicating if problem has algebraic path constraints
            hasAlgPathCons = obj.hasAlgPathCons;
        end
        
        function SetHasIntegralCons(obj,hasIntegralCons)
            % Sets flag to indicate if problem has integral constraints
            obj.hasIntegralCons = hasIntegralCons;
        end
        
        function hasIntegralCons = GetHasIntegralCons(obj)
            % Returns flag indicating if problem has integral constraints
            hasIntegralCons = obj.hasIntegralCons;
        end
        
        function SetHasIntegralCost(obj,hasIntegralCost)
            % Sets flag to indicate if problem has integral cost component
            obj.hasIntegralCost = hasIntegralCost;
        end
        
        function hasIntegralCost = GetHasIntegralCost(obj)
            % Returns flag indicating if problem has integral cost component
            hasIntegralCost = obj.hasIntegralCost;
        end
        
        function SetHasAlgebraicCost(obj,hasAlgebraicCost)
            % Sets flag to indicate if problem has algebraic cost component
            obj.hasAlgebraicCost = hasAlgebraicCost;
        end
        
        function hasAlgebraicCost = GetHasAlgebraicCost(obj)
            % Returns flag indicating if problem has algebraic cost component
            hasAlgebraicCost = obj.hasAlgebraicCost;
        end
        
        % ===== Discretization properties
        
        function SetMeshIntervalFractions(obj,meshIntervalFractions)
            if length(meshIntervalFractions) <= 1
                error('meshIntervalFractions contain at least two elements');
            end
            obj.meshIntervalFractions = meshIntervalFractions;
        end
        
        function meshIntervalFractions = GetMeshIntervalFractions(obj)
            meshIntervalFractions = obj.meshIntervalFractions;
        end
        
        function SetMeshIntervalNumPoints(obj,meshIntervalNumPoints)
            if length(meshIntervalNumPoints) < 1
                error('meshIntervalNumPoints must contain at least one element');
            end
            obj.meshIntervalNumPoints = meshIntervalNumPoints;
            obj.numMeshIntervals = length(obj.meshIntervalNumPoints);
        end
        
        function meshIntervalNumPoints = GetMeshIntervalNumPoints(obj)
            meshIntervalNumPoints = obj.meshIntervalNumPoints;
        end
        
        function numMeshIntervals = GetNumMeshIntervals(obj)
            numMeshIntervals = obj.numMeshIntervals;
        end
        
        function numPoints = GetNumPointsInMeshInterval(obj,meshId)
           if meshId > obj.numMeshIntervals
               error('meshId must be <= to number of mesh intervals');
           end
           numPoints = obj.meshIntervalNumPoints(meshId);
        end
        
        function meshFraction = GetMeshIntervalFraction(obj,meshId)
           if meshId > obj.numMeshIntervals + 1
               error('meshId must be <= to number of mesh intervals + 1');
           end
            meshFraction = obj.meshIntervalFractions(meshId);
        end
        
        function obj = ValidateMeshConfig(obj)
            %  Validates user configuration of the phase
            if length(obj.meshIntervalFractions) ~= ...
                    length(obj.meshIntervalNumPoints) + 1
                error(['Length of meshIntervalNumPoints' ...
                    ' must be the one greater than length ' ....
                    ' of meshIntervalFractions' ]);
            end
        end
        
        % ===== State vector related quantities
        
        function SetStateLowerBound(obj,stateLowerBound)
            obj.stateLowerBound = stateLowerBound;
        end
        
        function stateLowerBound = GetStateLowerBound(obj)
            stateLowerBound = obj.stateLowerBound;
        end
        
        function SetStateUpperBound(obj,stateUpperBound)
            obj.stateUpperBound = stateUpperBound;
        end
        
        function stateUpperBound = GetStateUpperBound(obj)
            stateUpperBound = obj.stateUpperBound;
        end
        
        function SetStateInitialGuess(obj,stateInitialGuess)
            obj.stateInitialGuess = stateInitialGuess;
        end
        
        function stateInitialGuess = GetStateInitialGuess(obj)
            stateInitialGuess = obj.stateInitialGuess;
        end
        
        function SetStateFinalGuess(obj,stateFinalGuess)
            obj.stateFinalGuess = stateFinalGuess;
        end
        
        function stateFinalGuess = GetStateFinalGuess(obj)
            stateFinalGuess = obj.stateFinalGuess;
        end
        
        function ValidateStateProperties(obj)
            % Validate that state related properties are valid
            
            % Check dimensions of lower bounds
            if length(obj.stateLowerBound) ~= obj.numStateVars
                error('length of stateLowerBounds must be numStateVars')
            end
            
            % Check dimensions of upper bounds
            if length(obj.stateUpperBound) ~= obj.numStateVars
                error('length of stateUpperBounds must be numStateVars')
            end
            
            % Check dimensions of initial guess
            if length(obj.stateInitialGuess) ~= obj.numStateVars
                error('length of stateInitialGuess must be numStateVars')
            end
            
            % Check dimensions of final guess
            if length(obj.stateFinalGuess) ~= obj.numStateVars
                error('length of stateFinalGuess must be numStateVars')
            end
            
            % Check that initial guess falls between upper and lower bounds
            for stateIdx = 1:obj.numStateVars
                if (obj.stateInitialGuess(stateIdx) > ...
                        (obj.stateUpperBound(stateIdx))) || ...
                        (obj.stateInitialGuess(stateIdx) < ...
                        (obj.stateLowerBound(stateIdx)))
                    error('stateInitialGuess elements must fall between upper and lower bounds')
                end
            end
            
            % Check that final guess falls between upper and lower bounds
            for stateIdx = 1:obj.numStateVars
                if (obj.stateFinalGuess(stateIdx) > ...
                        (obj.stateUpperBound(stateIdx))) || ...
                        (obj.stateFinalGuess(stateIdx) < ...
                        (obj.stateLowerBound(stateIdx)))
                    error('stateFinalGuess elements must fall between upper and lower bounds')
                end
            end
            
            % Lower bound must be less than upper bound
            boundDiff = 1e-5;
            for stateIdx = 1:obj.numStateVars
                if obj.stateLowerBound(stateIdx) > ...
                        (obj.stateUpperBound(stateIdx) - boundDiff)
                    error(['State lower bound for state id ' num2str(stateIdx) ...
                        ' must be 1e5*eps less state upper bound'])
                end
            end
            
        end
        
        % ===== Time vector related quantities
        
        function SetTimeLowerBound(obj,timeLowerBound)
            obj.timeLowerBound = timeLowerBound;
        end
        
        function timeLowerBound = GetTimeLowerBound(obj)
            timeLowerBound = obj.timeLowerBound;
        end
        
        function SetTimeUpperBound(obj,timeUpperBound)
            obj.timeUpperBound = timeUpperBound;
        end
        
        function timeUpperBound = GetTimeUpperBound(obj)
            timeUpperBound = obj.timeUpperBound;
        end
        
        function SetTimeInitialGuess(obj,timeInitialGuess)
            obj.timeInitialGuess = timeInitialGuess;
        end
        
        function timeInitialGuess = GetTimeInitialGuess(obj)
            timeInitialGuess = obj.timeInitialGuess;
        end
        
        function SetTimeFinalGuess(obj,timeFinalGuess)
            obj.timeFinalGuess = timeFinalGuess;
        end
        
        function timeFinalGuess = GetTimeFinalGuess(obj)
            timeFinalGuess = obj.timeFinalGuess;
        end
        
        function ValidateTimeProperties(obj)
            % Validate that time related properties are valid
            
            % Check dimensions of lower bounds
            if length(obj.timeLowerBound) ~= 1
                error('length of timeLowerBounds must be numTimeVars')
            end
            
            % Check dimensions of upper bounds
            if length(obj.timeUpperBound) ~= 1
                error('length of timeUpperBounds must be numTimeVars')
            end
            
            % Check dimensions of initial guess
            if length(obj.timeInitialGuess) ~= 1
                error('length of timeInitialGuess must be numTimeVars')
            end
            
            % Check dimensions of final guess
            if length(obj.timeFinalGuess) ~= 1
                error('length of timeFinalGuess must be numTimeVars')
            end
            
            % Check that initial guess falls between upper and lower bounds
            if (obj.timeInitialGuess > obj.timeUpperBound) || ...
                    (obj.timeInitialGuess < obj.timeLowerBound)
                error('timeInitialGuess  must fall between upper and lower bounds')
            end
            % Check that final guess falls between upper and lower bounds
            if (obj.timeFinalGuess >  obj.timeUpperBound) || ...
                    (obj.timeFinalGuess  < obj.timeLowerBound)
                error('timeFinalGuess must fall between upper and lower bounds')
            end
            
            % Lower bound must be less than upper bound
            boundDiff = 1e-5;
            if obj.timeLowerBound > (obj.timeUpperBound - boundDiff)
                error('Time lower bound must be 1e-5 less time upper bound')
            end
            
        end
        
        % ===== Control related quantities
        function SetControlLowerBound(obj,controlLowerBound)
            obj.controlLowerBound = controlLowerBound;
        end
        
        function controlLowerBound = GetControlLowerBound(obj)
            controlLowerBound = obj.controlLowerBound;
        end
        
        function SetControlUpperBound(obj,controlUpperBound)
            obj.controlUpperBound = controlUpperBound;
        end
        
        function controlUpperBound = GetControlUpperBound(obj)
            controlUpperBound = obj.controlUpperBound;
        end
        
        function ValidateControlProperties(obj)
            % Validate that control related properties are valid
            
            % Check dimensions of lower bounds
            if length(obj.controlLowerBound) ~= obj.numControlVars
                error('length of controlLowerBounds must be numControlVars')
            end
            
            % Check dimensions of upper bounds
            if length(obj.controlUpperBound) ~= obj.numControlVars
                error('length of controlUpperBounds must be numControlVars')
            end
            
            
            % Lower bound must be less than upper bound
            boundDiff = 1e-5;
            for controlIdx = 1:obj.numControlVars
                if obj.controlLowerBound(controlIdx) > ...
                        (obj.controlUpperBound(controlIdx) - boundDiff)
                    error(['Control lower bound for control id ' num2str(controlIdx) ...
                        ' must be 1e5*eps less control upper bound'])
                end
            end
            
        end
        
    end
    
end


