classdef Phase < handle
    %PHASE Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (SetAccess = 'public')
        
        
        %% State bounds phase
        controlUpperBound
        controlLowerBound
        stateUpperBound
        stateLowerBound
        initialTimeUpperBound
        initialTimeLowerBound
        finalTimeUpperBound
        finalTimeLowerBound
        initialStateLowerBound
        initialStateUppperBound
        finalStateLowerBound
        finalStateUpperBound
        finalGuessState
        finalGuessEpoch
        
        %%  Initial Guess
        initialEpoch
        finalEpoch
        initialGuessState
        initialGuessEpoch
        initialGuessMode
        initialGuessControl
        
        %%  Phase properties
        numStates
        numControls
        numIntegrals = 0
        decisionVector
        timeVector
        timeVectorType
        
        %% User functions
        pathFunctionName
        pointFunctionName
        
        %%  Constraint bounds vectors for different constraint types.
        %  These are assembled from user inputs.
        algConstraintLowerBound
        algConstraintUpperBound
        eventConstraintLowerBound
        eventConstraintUpperBound
        intConstraintUpperBound
        intConstraintLowerBound

        phaseNum
        
    end
    
    properties (SetAccess = 'protected')
        
        %  These do not go here
        meshPoints
        numMeshPoints
        numCollocationPoints
        numPathConstraintPoints
        
        %  Cost and constraint related parameters
        numMeshIntervals
        numStagePoints
        numStatePoints
        numControlPoints
        numStateParams
        numTimeParams
        numControlParams
        numStaticParams = 0;
        numDecisionParams
        
        %  Constraint vectors
        defectConstraintVec
        algConstraintVec
        eventConstraintVec
        intConstraintVec
        costFuncIntegral
        costFuncPoint
        
        %  Bound vectors
        defectConLowerBound
        defectConUpperBound
        pathConLowerBound
        pathConUpperBound
        eventConLowerBound
        eventConUpperBound
        allConLowerBound
        allConUpperBound
        
        %  Number of constraints
        numConstraints
        numDefectConstraints
        numPathConstraints
        numEventConstraints
        
        %%  Bookkeeping parameters
        
        %  Constraint bookkeeping
        defectStartIdx
        pathStartIdx
        eventStartIdx
        defectEndIdx
        pathEndIdx
        eventEndIdx
        
        % timeVector
        finalTime
        initialTime
        
        %%  Helper classes
        % For evaluating user functions
        PathFunction;% = UserPathFunction();
        % For evaluating user functions
        PointFunction;% = UserPointFunction();
        %  For managinging decision vector
        DecVector;% = DecisionVector();
        %probTranscription();  %  For transcription details
        %  For Computing the intitial guess
        GuessGen = GuessGenerator
        
        decisionVecLowerBound
        decisionVecUpperBound
        
        
    end
    
    properties (SetAccess = 'private')
        
    end
    
    methods
        
        %  The constructor
        function obj = Phase()
            obj.DecVector     = DecisionVector();
            obj.PathFunction  = UserPathFunction();
            obj.PointFunction = UserPointFunction();
        end
        
        %  Initialize the phase
        function obj = Initialize(obj)
            
            %  Compute the properties for defined mesh properties
            ValidatePhaseConfig(obj);
            
            %  Get properties that are dependent upon the transcription
            %  being used.  These come from the leaf classes
            obj = InitTranscription(obj);
            
            %  Initialize the decision vector helper class
            obj.DecVector.Initialize(obj.numStates, ...
                obj.numControls,obj.numIntegrals,obj.numStaticParams,...
                obj.numStatePoints,obj.numControlPoints,...
                obj.numStateStagePoints,obj.numControlStagePoints);
            
            %  Compute and set the initial guess and configure time.
            obj.InitializeTimeVector();
            obj.IntializeUserFunctions();
            obj.SetInitialGuess();
            
            %  Set constraint size properties and bounds
            obj.SetConstraintProperties();
            obj.SetConstraintBounds();
            obj.SetDecisionVectorBounds();
            
        end
        
        function IntializeUserFunctions(obj)
            
            %  Set the phase number and function names
            obj.PathFunction.phaseNum = obj.phaseNum;
            obj.PointFunction.phaseNum = obj.phaseNum;
            obj.PathFunction.functionName  = obj.pathFunctionName;
            obj.PointFunction.functionName = obj.pointFunctionName;
            %  Prepare the function for evaluation and intialize
            obj.PreparePathFunction(1,0,1);  % Evaluate at first point
            obj.PathFunction.Initialize();
            obj.ComputePathFunctions();
            obj.PreparePointFunction();
            obj.PointFunction.Initialize();
            
        end
        
        %  Set indeces for chunks of the constraint vector
        function obj = SetConstraintProperties(obj)
            
            %  Set chunk values and bounds.
            obj.numIntegrals = 0;  % TODO:  should not be hard coded
            obj.numPathConstraints = obj.numPathConstraintPoints*...
                obj.PathFunction.numAlgFunctions;
            obj.numEventConstraints = obj.PointFunction.numEventFunctions;
            obj.numConstraints = obj.numDefectConstraints + ...
                obj.numPathConstraints + obj.numEventConstraints;
            
            %  Set start and end indeces of different constraint
            %  parameter types.  These describe where different chunks
            %  of the constraint vector begin and end.
            obj.defectStartIdx = 1;
            obj.defectEndIdx  = obj.defectStartIdx + ...
                obj.numDefectConstraints -1;
            obj.pathStartIdx = obj.defectEndIdx + 1;
            obj.pathEndIdx = obj.pathStartIdx + ...
                obj.numPathConstraints -1;
            obj.eventStartIdx = obj.pathEndIdx + 1;
            obj.eventEndIdx = obj.eventStartIdx + ...
                obj.numEventConstraints -1;
            
        end
        
        %  Set the upper and lower bounds on the decision vector
        function obj = SetDecisionVectorBounds(obj)
            
            % numPoints = obj.numStatePoints;
            %  Dimension the vectors
            obj.decisionVecLowerBound = zeros(obj.numDecisionParams,1);
            obj.decisionVecUpperBound = zeros(obj.numDecisionParams,1);
            
            %  Use a temporary decision vector object for bookkeeping
            boundVector = DecisionVector();
            boundVector.Initialize(obj.numStates, ...
                obj.numControls,obj.numIntegrals,obj.numStaticParams,...
                obj.numStatePoints,obj.numControlPoints,...
                obj.numStateStagePoints,obj.numControlStagePoints);
            
            %  Assemble the state bound array
            upperBoundStateArray = zeros(...
                obj.DecVector.numStatePoints,obj.numStates);
            lowerBoundStateArray = zeros(...
                obj.DecVector.numStatePoints,obj.numStates);
            for rowIdx = 1:obj.DecVector.numStatePoints
                if rowIdx == 1;
                    lowerBoundVec = obj.initialStateLowerBound;
                    upperBoundVec = obj.initialStateUppperBound;
                elseif rowIdx > 1 && rowIdx < obj.DecVector.numStatePoints
                    lowerBoundVec = obj.stateLowerBound;
                    upperBoundVec = obj.stateUpperBound;
                elseif rowIdx == obj.DecVector.numStatePoints
                    lowerBoundVec = obj.finalStateLowerBound;
                    upperBoundVec = obj.finalStateUpperBound;
                end
                lowerBoundStateArray(rowIdx,:) = lowerBoundVec;
                upperBoundStateArray(rowIdx,:) = upperBoundVec;
            end
            
            %  Assemble the control bound array
            upperBoundControlArray = zeros(...
                obj.DecVector.numControlPoints,obj.numControls);
            lowerBoundControlArray = zeros(...
                obj.DecVector.numControlPoints,obj.numControls);
            for rowIdx = 1:obj.DecVector.numControlPoints
                lowerBoundControlArray(rowIdx,:) = obj.controlLowerBound;
                upperBoundControlArray(rowIdx,:) = obj.controlUpperBound;
            end
            
            %  Set time vector bounds
            upperBoundTimeArray = [...
                obj.initialTimeUpperBound obj.finalTimeUpperBound]';
            lowerBoundTimeArray = [...
                obj.initialTimeLowerBound obj.finalTimeLowerBound]';
            
            %  Set lower bound vector
            boundVector.SetStateArray(lowerBoundStateArray);
            boundVector.SetControlArray(lowerBoundControlArray);
            boundVector.SetTimeVector(lowerBoundTimeArray);
            obj.decisionVecLowerBound = boundVector.decisionVector;
            
            %  Set upper bound vector
            boundVector.SetStateArray(upperBoundStateArray);
            boundVector.SetControlArray(upperBoundControlArray);
            boundVector.SetTimeVector(upperBoundTimeArray);
            obj.decisionVecUpperBound = boundVector.decisionVector;
            
        end
        
        %
        function InitializeTimeVector(obj)
            obj.DecVector.SetTimeVector(...
                [obj.initialEpoch; obj.finalEpoch]);
            obj.ComputeTimeVector();
        end
        
        %  Set bounds on constraints
        function obj = SetConstraintBounds(obj)
            %  Set bounds for each chunk
            obj = SetDefectConstraintBounds(obj);
            obj = SetPathConstraintBounds(obj);
            obj = SetEventConstraintBounds(obj);
            
            %  Now insert chunks into the complete bounds vector
            dStartIdx = obj.defectStartIdx;
            dEndIdx = obj.defectEndIdx;
            pStartIdx = obj.pathStartIdx;
            pEndIdx = obj.pathEndIdx;
            eStartIdx = obj.eventStartIdx;
            eEndIdx = obj.eventEndIdx;
            obj.allConLowerBound(dStartIdx:dEndIdx,1) = ...
                obj.defectConLowerBound;
            obj.allConUpperBound(dStartIdx:dEndIdx,1) = ...
                obj.defectConUpperBound;
            obj.allConLowerBound(pStartIdx:pEndIdx,1) = ...
                obj.pathConLowerBound;
            obj.allConUpperBound(pStartIdx:pEndIdx,1) = ...
                obj.pathConUpperBound;
            obj.allConLowerBound(eStartIdx:eEndIdx,1) = ...
                obj.eventConLowerBound;
            obj.allConUpperBound(eStartIdx:eEndIdx,1) = ...
                obj.eventConUpperBound;
        end
        
        %  Set bounds on the defect constraints
        function obj = SetDefectConstraintBounds(obj)
            obj.defectConLowerBound = ...
                zeros(obj.numStates*obj.numCollocationPoints,1);
            obj.defectConUpperBound = ...
                zeros(obj.numStates*obj.numCollocationPoints,1);
        end
        
        %  Set bounds on the path constraints
        function obj = SetPathConstraintBounds(obj)
            if obj.PathFunction.hasAlgFunctions
                obj.pathConLowerBound = zeros(obj.numPathConstraints,1);
                obj.pathConUpperBound = zeros(obj.numPathConstraints,1);
                lowIdx = 1;
                for conIdx = 1:obj.numCollocationPoints
                    highIdx = lowIdx + obj.PathFunction.numAlgFunctions-1;
                    obj.pathConLowerBound(lowIdx:highIdx,1) = ...
                        obj.algConstraintLowerBound;
                    obj.pathConUpperBound(lowIdx:highIdx,1) = ...
                        obj.algConstraintUpperBound;
                    lowIdx = lowIdx + obj.PathFunction.numAlgFunctions;
                end
            else
                obj.pathConLowerBound = [];
                obj.pathConUpperBound = [];
            end
        end
        
        %  Set bounds on event constraints
        function obj = SetEventConstraintBounds(obj)
            obj.eventConLowerBound = obj.eventConstraintLowerBound;
            obj.eventConUpperBound = obj.eventConstraintUpperBound;
        end
        
        %  Compute the initial guess.  Needs to be a new class!
        function obj = SetInitialGuess(obj)
            
            %  Intialize the guess generator class
            obj.GuessGen.Initialize(...
                obj.timeVector,obj.DecVector.numStates,...
                obj.DecVector.numStatePoints,obj.DecVector.numControls,...
                obj.DecVector.numControlPoints)
            
            switch obj.initialGuessMode
                
                case {'LinearNoControl','LinearUnityControl','LinearCoast'}
                    
                    xGuessMat = obj.GuessGen.ComputeLinearStateGuess(...
                        obj.initialGuessState,obj.finalGuessState);
                    
                    %  Fill in the control array
                    if strcmp(obj.initialGuessMode,'LinearUnityControl')
                        controlMag = 1;
                    else
                        controlMag = 0;
                    end
                    if strcmp(obj.initialGuessMode,'LinearUnityControl') ...
                            || strcmp(obj.initialGuessMode,'LinearNoControl')
                        uGuessMat = obj.GuessGen.ComputeConstControlGuess(...
                            controlMag);
                    else
                        uGuessMat = [];
                    end
                    
            end
                        
            %  Call the decision vector and populate with the guess
            obj.DecVector.SetStateArray(xGuessMat);
            obj.DecVector.SetControlArray(uGuessMat);
            
        end
        
        %  Sets decision vector provided by optimizer
        function obj = SetDecisionVector(obj,decVector)
            
            %  Set the decision vector on the trajectory.
            %  Maybe this is not needed, maybe we should just pass straight
            %  through to phases.  Not Sure.
            if length(decVector) == obj.numDecisionParams
                obj.DecVector.SetDecisionVector(decVector);
            else
                errorMsg = ['Length of decisionVector must be ' ...
                    ' equal to totalnumDecisionParams'];
                errorLoc  = 'Phase:SetDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Update the time vector accordingly.
            %  TODO: This is a kludge.  Should not need to be set here.
            obj.ComputeTimeVector()
            
        end
        
        %  Get all constraints for the phase
        function [contraintVec] = GetContraintVector(obj)
            % Call path and point functions and then assemble the
            % constraint vector
            obj.ComputePathFunctions();
            obj.ComputePointFunctions();
            if obj.PathFunction.hasDynFunctions
                contraintVec = ...
                    [obj.defectConstraintVec];
            end
            if obj.PathFunction.hasAlgFunctions
                contraintVec = [contraintVec; ...
                    obj.algConstraintVec];
            end
            if obj.PointFunction.hasEventFunctions
                contraintVec = [contraintVec; ...
                    obj.eventConstraintVec];
            end
        end
        
        %  Compute the path functions
        function ComputePathFunctions(obj)
            
            %  Loop over mesh and stage points compute
            %  functions at each point
            algIdx               = 1;
            obj.costFuncIntegral = 0;
            quadCount            = 0;
            for pointCnt = 1:length(obj.timeVector)
                
                %  Prepare then evaluate the path function
                pointType = obj.timeVectorType(pointCnt);
                meshIdx  =  obj.GetMeshIndex(pointCnt);
                stageIdx = GetStageIndex(obj,pointCnt);
                obj.PreparePathFunction(meshIdx,stageIdx,pointType);
                obj.PathFunction.Evaluate();
                
                %  Only evaluate when quadrature point,(i.e.pointType=1)
                if pointType == 1
                    
                    quadCount = quadCount + 1;
                    %  Handle defect constraints
                    if obj.PathFunction.hasDynFunctions
                        odeMatrix(quadCount,1:obj.numStates) = ...
                            obj.PathFunction.dynFunctions';
                    end
                    
                    %  Handle cost function
                    if obj.PathFunction.hasCostFunction
                        costVec(quadCount,1) = ...
                            obj.PathFunction.costFunction;
                    end
                    
                    %  Handle algebraic constraints
                    if obj.PathFunction.hasAlgFunctions
                        algArray(algIdx:algIdx+...
                            obj.PathFunction.numAlgFunctions-1,1) = ...
                            obj.PathFunction.algFunctions;
                        algIdx = algIdx + ...
                            obj.PathFunction.numAlgFunctions;
                    end
                end
                
            end
            
            % Compute defect constraints and integrals
            if ~obj.PathFunction.hasDynFunctions
                odeMatrix = [];
            end
            if ~obj.PathFunction.hasCostFunction
                costVec = [];
            end
            obj.ComputeQuadratures(odeMatrix,costVec);
            
            %  Set algebraic constraints
            if obj.PathFunction.hasAlgFunctions
                obj.algConstraintVec = algArray;
            end
            
        end
        
        %  Evaluate the user point functions
        function ComputePointFunctions(obj)
            %  Prepare and call the user function
            obj.PreparePointFunction();
            obj.PointFunction.Evaluate();
            %  Set event constraints if there are any
            if obj.PointFunction.hasEventFunctions
                obj.eventConstraintVec = obj.PointFunction.eventFunctions;
            end
            if obj.PointFunction.hasCostFunction
                obj.costFuncPoint =  obj.PointFunction.costFunction;
            end
            if obj.PointFunction.hasIntegralFunctions
                obj.intConstraintVec = obj.PointFunction.intFunctions;
            end
        end
        
        %  Compute defect constraints
        function costFunction = GetCostFunction(obj)
            %  Warning: Call to GetCostFunction must ALWAYS be preceded by
            %  call to GetContraintVector()
            costFunction = 0;
            if obj.PathFunction.hasCostFunction
                costFunction = costFunction + obj.costFuncIntegral;
            end
            if obj.PointFunction.hasCostFunction
                costFunction = costFunction + obj.costFuncPoint;
            end
        end
        
        %  Configure input structure for given mesh point
        function inputData = UpdateInputData(obj,pointIdx)
            
            %  This function extracts the state, control, time for state
            %  data from the decision vector at the mesh poin "pointIdx"
            inputData.stateVec =...
                obj.DecVector.GetStateAtMeshPoint(pointIdx);
            inputData.controlVec = ...
                obj.DecVector.GetControlAtMeshPoint(pointIdx);
            
        end
        
        %  Configure input structure for given mesh point
        function PreparePathFunction(obj,meshIdx,stageIdx,pointType)
            
            %  This function extracts the state, control, time for state
            %  data from the decision vector
            obj.PathFunction.pointType = pointType;
            if pointType == 1 || pointType == 2
                obj.PathFunction.stateVec =...
                    obj.DecVector.GetStateAtMeshPoint(meshIdx,stageIdx);
            else
                obj.PathFunction.stateVec = ...
                    obj.PathFunction.stateVec*NaN;
            end
            if pointType == 1 || pointType == 3
                obj.PathFunction.controlVec = ...
                    obj.DecVector.GetControlAtMeshPoint(meshIdx,stageIdx);
            else
                obj.PathFunction.controlVec = ...
                    obj.PathFunction.controlVec*NaN;
            end
            
        end
        
        %  Configure input structure for given mesh point
        function PreparePointFunction(obj)
            
            %  This function extracts the state, control, time for state
            %  data from the decision vector at the mesh point "pointIdx"
            obj.PointFunction.initialStateVec =...
                obj.DecVector.GetFirstStateVector();
            obj.PointFunction.finalStateVec = ...
                obj.DecVector.GetLastStateVector();
            obj.PointFunction.intConstraintVec = obj.intConstraintVec;
            obj.PointFunction.initialTime = ...
                obj.DecVector.GetFirstTime();
            obj.PointFunction.finalTime = ...
                obj.DecVector.GetLastTime();
            
        end
        
        %  Evaluate constraint feasiblity
        function Status = CheckConstraintVariances(obj)
            
            %  Evaluate constraints initialize items
            constraintVec   = GetContraintVector(obj);
            if isa(constraintVec,'gradient')
                constraintVec = constraintVec.x;
            end
            numEventViolations  = 0;
            numdefectViolations = 0;
            numPathViolations   = 0;
            eventViolation  = false();
            defectViolation = false();
            pathViolation   = false();
            tol = 1e-5;  %  TODO.  Don't hard code this
            
            %  Check point (event) constraints for violation
            if obj.PointFunction.hasEventFunctions;
                for conIdx = obj.eventStartIdx:obj.eventEndIdx
                    if constraintVec(conIdx) <= ...
                            obj.allConLowerBound(conIdx) - tol|| ...
                            constraintVec(conIdx) >= ...
                            obj.allConUpperBound(conIdx) + tol;
                        eventViolation = true();
                        numEventViolations = numEventViolations + 1;
                    end
                end
            end
            
            %  Check point (event) constraints for violation
            if obj.PathFunction.hasDynFunctions;
                for conIdx = obj.defectStartIdx:obj.defectEndIdx
                    if constraintVec(conIdx) <= ...
                            obj.allConLowerBound(conIdx) - tol || ...
                            constraintVec(conIdx) >= ...
                            obj.allConUpperBound(conIdx) + tol;
                        defectViolation = true();
                        numPathViolations = numPathViolations + 1;
                    end
                end
            end
            
            %  Check path constraints for violation
            if obj.PathFunction.hasAlgFunctions;
                for conIdx = obj.pathStartIdx:obj.pathEndIdx
                    if constraintVec(conIdx) <= ...
                            obj.allConLowerBound(conIdx) - tol|| ...
                            constraintVec(conIdx) >= ...
                            obj.allConUpperBound(conIdx) + tol;
                        pathViolation = true();
                        numdefectViolations = numdefectViolations + 1;
                    end
                end
            end
            Status.numEventViolations  = numEventViolations;
            Status.numdefectViolations = numdefectViolations;
            Status.numPathViolations   = numPathViolations;
            Status.eventViolation      = eventViolation;
            Status.defectViolation     = defectViolation;
            Status.pathViolation       = pathViolation;
        end
        
        %  Returns the time decision variables in [2x1] matrix form
        function  timeMat = GetInitialFinalTime(obj)
            timeMat = [obj.DecVector.GetFirstTime();...
                obj.DecVector.GetLastTime()];
        end
        
        %  Configure initial time parameters
        function ComputeInitialTimeParameters(obj)
            obj.finalTime   = obj.finalEpoch;
            obj.initialTime = obj.initialEpoch;
        end
        
        %  Validate that the mesh properties are set acceptably
        function ValidateMeshProperties(obj)
            
            %  Check that sizes are consistent
            if size(obj.meshIntervalPoints,2) ~=  ...
                    size(obj.meshIntervalFractions,2)
                errorMsg = ['meshIntervalPoints must have the same' ...
                    'number of columns as meshFraction'];
                errorLoc  = 'Phase:SetMeshProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Check that each are row vectors
            if size(obj.meshIntervalPoints,1) ~= 1
                errorMsg = 'meshIntervalPoints must be a row vector';
                errorLoc  = 'Phase:SetMeshProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            if size(obj.meshIntervalFractions,1) ~= 1
                errorMsg = 'meshFraction must be a row vector';
                errorLoc  = 'Phase:SetMeshProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        % Validate that the bounds have been set appropriately
        function ValidateBoundProperties(obj)
            
            if  obj.positionUpperBound <= obj.positionLowerBound
                errorMsg = ['positionUpperBound must be greater than' ...
                    'positionLowerBound'];
                errorLoc  = 'Phase:ValidateBoundProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            if  obj.velocityUpperBound <= obj.velocityLowerBound
                errorMsg = ['velocityUpperBound must be greater than' ...
                    'velocityLowerBound'];
                errorLoc  = 'Phase:ValidateBoundProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            if  obj.massUpperBound <= obj.massLowerBound
                errorMsg = ['massUpperBound must be greater than' ...
                    'massLowerBound'];
                errorLoc  = 'Phase:ValidateBoundProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        % Validate that the bounds have been set appropriately
        function ValidateStateProperties(obj)
            
            [rows,cols] = size(obj.initialEpoch);
            if rows ~= 1 && cols ~= 1
                errorMsg = 'initialEpoch must scalar real';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            [rows,cols] = size(obj.finalEpoch);
            if rows ~= 1 && cols ~= 1
                errorMsg = 'finalEpoch must scalar real';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            [rows,cols] = size(obj.initialGuessEpoch);
            if rows ~= 1 && cols ~= 1
                errorMsg = 'intialGuessEpoch must scalar real';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            [rows,cols] = size(obj.initialGuessState);
            if rows ~= 7 && cols ~= 1
                errorMsg = 'intialGuessState must be 7x1 vector';
                errorLoc  = 'Phase:ValidateStateProperties';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        %  Validates user configuration of the phase
        function obj = ValidatePhaseConfig(obj)
            if length(obj.meshIntervalFractions) ~= ...
                    length(obj.meshIntervalNumPoints) + 1
                errorMsg = ['Length of meshIntervalNumPoints' ...
                    ' must be the one greater than length ' ....
                    ' of meshIntervalFractions' ];
                errorLoc  = 'Phase:ValidatePhaseConfig';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
        end
        
    end
    
    %  Abstract methods that must be implemented on the derived classes as
    %  they are specific to the type of phase.
    methods (Abstract)
        SetStageProperties(obj)
        InitTranscription(obj)
        % InitializeTimeVector(obj)
        ComputeTimeVector(obj)
        ComputeQuadratures(obj)
        ComputeTimeParameters(obj)
        GetMeshIndex(obj)
        GetStageIndex(obj)
    end
    
end

%         %  Compute sparsity pattern of defects contraints
%         function [sparsityPattern] = GetSparsityPattern_Defect_State(obj)
%             sparsityPattern = abs(sign(obj.radauDiffMatrix));
%         end
%
%         %  Compute sparsity pattern of defects contraints
%         function [sparsityPattern] = GetSparsityPattern_Defect_Control(obj)
%             sparsityPattern = eye(obj.numRadauPoints);
%         end
%
%         %  Compute sparsity pattern of defects contraints
%         function [sparsityPattern] = GetSparsityPattern_Defect_Time(obj)
%             sparsityPattern = ones(obj.numRadauPoints,2);
%         end