classdef Phase < handle
    %PHASE The phase base class for collocation type phases.
    
    %  Note:  All internal computations are performed in dimensional units.
    %    Non-dimensionalization occur at the class boundaries.
    %    Phase.SetDecisionVector() takes either a dimensional or
    %    non-dimensional vector and sets the internal vector to
    %    dimensional.  The following functions optionally return
    %    dimensional function values and all other functions return
    %    dimensional values:  GetContraintVector, GetCostFunction
    
    properties (SetAccess = 'public')
        
        % String.  The model for the initial guess
        %     Options:  LinearNoControl, LinearUnityControl, Propagated, File
        initialGuessMode
        %  Real Array.  Guess for control
        initialGuessControl
        % UserPathFunction.  The user's path function object.
        pathFunctionObj
        %  String.  The name of the user's guess function
        guessFunctionName
        %  Pointer to a GuessInterface
        guessObject
        centralBody
        refEpoch
        phaseIdx
        phaseName
        scaleUtil
        
    end
    
    properties (SetAccess = 'protected')
        
        %  Integer.  The assigned phase number from Trajectory
        phaseNum
        % Integer. Number of algebraic path constraints in the NLP
        numAlgPathConNLP
        %  Real Array. Vector of algebraic path constraint values
        algPathConVec
        % Real Array. Vector of defect constraint values.
        defectConVec
        %  Cell Array. Cell array of algebraic path constraint descriptions
        algPathConVecNames
        % Real. Integral contribution to cost function
        costFuncIntegral
        % Sparse matrix. The Jacobian of the NLP contraints
        nlpConstraintJac
        % Sparse matrix. The Jacbian of the NLP cost (algebraic + quadrature)
        nlpCostJac
        % sparse matrix. The sparsity pattern for the phase NLP contraints
        conSparsityPattern
        % sparse matrix. Sparsity pattern for the cost function
        costSparsityPattern
        %  Real array. Lower bound vector on defect constraints
        defectConLowerBound
        %  Real array. Upper bound vector on defect constraints
        defectConUpperBound
        %  Real array. Lower bound vector on algebraic path constraints
        algPathConLowerBound
        %  Real array. Upper bound vector on algebraic path constraints
        algPathConUpperBound
        %  Real array. Lower bound vector on algebraic event constraints
        algEventConLowerBound
        %  Real array. Upper bound vector on algebraic event constraints
        algEventConUpperBound
        %  Real array. Lower bound vector for all constraints concatenated
        allConLowerBound
        %  Real array. Uower bound vector for all constraints concatenated
        allConUpperBound
        %  Real array. Lower bound on decision vector
        decisionVecLowerBound
        %  Real array. Upper bound on decision vector
        decisionVecUpperBound
        % Integer. The index for the first defect constraint
        defectConStartIdx
        % Integer. The index for the first defect constraint
        algPathConStartIdx
        % Integer. The index for the last defect constraint
        defectConEndIdx
        % Integer. The index for the last defect constraint
        algPathConEndIdx
        
        % ==== Housekeeping
        
        %  Bool.  Flag to indicate if functions must be recomputed
        %  This is set to true whenever decision vector is reset.
        %  This is set to false after call to ComputeUserFunctions()
        recomputeUserFunctions = 1
        %  Bool. Flag to indicate alg. path contraints must be updated
        recomputeAlgPathConstraints = true;
        %  Bool. Flag to indicate alg. cost must be updated
        recomputeAlgCostFunction = true;
        %  Bool.  Indicates Initialization is complete.
        isInitialized = false;
        %  Bool.  Flag to indicate if quadratures must be recomputed.
        %  This is set to true whenever decision vector is set or
        %  ComputeUserFunctions() is called.  Set to false in
        %  ComputeQuadratures()
        recomputeQuadratures = 1
        %  Flag to indicate that the phase has been run once and is now in
        %  mesh refinement mode.
        isRefining = false;
        refineInitialMesh = false;
        relErrorTol = 1e-5
        initRelErrorTol = 1e-2;
        maxInitMeshIterations = 3;
        
        % ==== Helper classes
        
        %  Array of UserFunctionData pointers.  Contains user dynamics
        %  functions evaluated at path points.
        userDynFuncData
        %  Array of UserFunctionData pointers.  Contains albegraic path
        %  functions evaluated at path points.
        userAlgFuncData
        %  Array of UserFunctionData pointers.  Contains user cost
        %  integrand evaluated at path points.
        costIntFuncData
        % UserFunProperties object. Properties of dynamic functions
        dynFuncProps
        % UserFunProperties object. Properties of cost functions
        costFuncProps
        % UserFunProperties object. Properties of algebraic functions
        algFuncProps
        % UserPathFunction. Helper class for evaluating user point funcs
        % Set in constructor
        PathFuncManager;
        % DecisionVector.  Helper class for managing data in dec. vec.
        % Set in constructor
        DecVector;
        % GuessGenerator.  Helper class for computing the intitial guess
        GuessGen = exec.GuessGenerator();
        % NLPFuncUtil_AlgPath object.  Helper for NLP functions/Jacobian
        algPathNLPFuncUtil
        % FunctionInputData object.  Input data for user path functions
        pathFuncInputData
        % ProblemCharacterisics object.  Problem settings and shared info
        Config
        % Collocation utility
        TransUtil
        %  TEMPORARY
        oldTimeVector
        oldState
        oldControl
        
    end
    
    methods (Access = public)
        
        %  The constructor
        function obj = Phase()
            import collutils.DecVecTypeBetts
            obj.DecVector     = DecVecTypeBetts();
            obj.PathFuncManager  = userfunutils.UserPathFunctionManager();
            obj.Config = collutils.ProblemCharacteristics;
            import userfunutils.FunctionInputData;
            obj.pathFuncInputData    = userfunutils.FunctionInputData();
        end
        
        %  Initialize the phase
        function obj = Initialize(obj)
            
            %  This is required for re-intialization during mesh refinement
            obj.isInitialized = false();
            obj.recomputeUserFunctions = true();
            
            %  Check the user's configuration before proceeding
            obj.Config.ValidateMeshConfig();
            obj.Config.ValidateStateProperties();
            
            %  Initialize transcription and other helper classes
            if obj.isRefining
                obj.PrepareForMeshRefinement();
            end
            obj.InitializeTranscription();
            obj.InitializeDecisionVector();
            obj.InitializeTimeVector();
            if ~obj.isRefining
                obj.IntializeUserFunctions();
            end
            obj.SetProblemCharacteristics();
            obj.SetInitialGuess();
            
            %  Dimension and initialize bounds and other quantities
            obj.SetConstraintProperties();
            obj.SetConstraintBounds();
            obj.SetDecisionVectorBounds();
            obj.IntializeJacobians();
            obj.InitializepathFuncInputData();
            obj.InitializeNLPHelpers();
            
            %  We're done intializing!
            obj.isInitialized = true();
            
        end
        
        function SetRefineInitialMesh(obj,flag)
           obj.refineInitialMesh = flag; 
        end
        
        function SetRelErrorTol(obj,tol)
           obj.relErrorTol = tol; 
        end
        
        function SetInitRelErrorTol(obj,tol)
           obj.initRelErrorTol = tol; 
        end
        
        function SetMaxInitMeshIterations(obj,numIter)
            obj.maxInitMeshIterations = numIter;
        end
        
        function SetPhaseNum(obj,phaseNum)
            obj.phaseNum = phaseNum;
        end
        
        function phaseNum = GetPhaseNum(obj)
            phaseNum = obj.phaseNum;
        end
        
        function SetNumStateVars(obj,numStateVars)
            obj.Config.SetNumStateVars(numStateVars);
        end
        
        function numStateVars = GetNumStateVars(obj)
            numStateVars = obj.Config.GetNumStateVars();
        end
        
        function SetNumControlVars(obj,numControlVars)
            obj.Config.SetNumControlVars(numControlVars);
        end
        
        function numControlVars = GetNumControlVars(obj)
            numControlVars = obj.Config.GetNumControlVars();
        end
        
        function SetMeshIntervalFractions(obj,meshIntervalFractions)
            obj.Config.SetMeshIntervalFractions(meshIntervalFractions);
        end
        
        function meshIntervalFractions = GetMeshIntervalFractions(obj)
            meshIntervalFractions = obj.Config.GetMeshIntervalFractions();
        end
        
        function SetMeshIntervalNumPoints(obj,meshIntervalNumPoints)
            obj.Config.SetMeshIntervalNumPoints(meshIntervalNumPoints);
        end
        
        function meshIntervalNumPoints = GetMeshIntervalNumPoints(obj)
            meshIntervalNumPoints = obj.Config.GetMeshIntervalNumPoints();
        end
        
        function SetStateLowerBound(obj,stateLowerBound)
            obj.Config.SetStateLowerBound(stateLowerBound);
        end
        
        function stateLowerBound = GetStateLowerBound(obj)
            stateLowerBound = obj.Config.GetStateLowerBound();
        end
        
        function SetStateUpperBound(obj,stateUpperBound)
            obj.Config.SetStateUpperBound(stateUpperBound);
        end
        
        function stateUpperBound = GetStateUpperBound(obj)
            stateUpperBound = obj.Config.GetStateUpperBound();
        end
        
        function SetStateInitialGuess(obj,stateInitialGuess)
            obj.Config.SetStateInitialGuess(stateInitialGuess);
        end
        
        function stateInitialGuess = GetStateInitialGuess(obj)
            stateInitialGuess = obj.Config.GetStateInitialGuess();
        end
        
        function SetStateFinalGuess(obj,stateFinalGuess)
            obj.Config.SetStateFinalGuess(stateFinalGuess);
        end
        
        function stateFinalGuess = GetStateFinalGuess(obj)
            stateFinalGuess = obj.Config.GetStateFinalGuess();
        end
        
        function SetTimeLowerBound(obj,timeLowerBound)
            obj.Config.SetTimeLowerBound(timeLowerBound);
        end
        
        function timeLowerBound = GetTimeLowerBound(obj)
            timeLowerBound = obj.Config.GetTimeLowerBound();
        end
        
        function SetTimeUpperBound(obj,timeUpperBound)
            obj.Config.SetTimeUpperBound(timeUpperBound);
        end
        
        function timeUpperBound = GetTimeUpperBound(obj)
            timeUpperBound = obj.Config.GetTimeUpperBound();
        end
        
        function SetTimeInitialGuess(obj,timeInitialGuess)
            obj.Config.SetTimeInitialGuess(timeInitialGuess);
        end
        
        function timeInitialGuess = GetTimeInitialGuess(obj)
            timeInitialGuess = obj.Config.GetTimeInitialGuess();
        end
        
        function SetTimeFinalGuess(obj,timeFinalGuess)
            obj.Config.SetTimeFinalGuess(timeFinalGuess);
        end
        
        function timeFinalGuess = GetTimeFinalGuess(obj)
            timeFinalGuess = obj.Config.GetTimeFinalGuess();
        end
        
        function SetControlLowerBound(obj,controlLowerBound)
            obj.Config.SetControlLowerBound(controlLowerBound);
        end
        
        function controlLowerBound = GetControlLowerBound(obj)
            controlLowerBound = obj.Config.GetControlLowerBound();
        end
        
        function SetControlUpperBound(obj,controlUpperBound)
            obj.Config.SetControlUpperBound(controlUpperBound);
        end
        
        function controlUpperBound = GetControlUpperBound(obj)
            controlUpperBound = obj.Config.GetControlUpperBound();
        end
        
        function numTotalConNLP = GetNumTotalConNLP(obj)
            numTotalConNLP = obj.Config.GetNumTotalConNLP();
        end
        
        function startIdx = GetDefectConStartIdx(obj)
            startIdx = obj.defectConStartIdx;
        end
        
        function endIdx = GetDefectConEndIdx(obj)
            endIdx = obj.defectConEndIdx;
        end
        
        function obj = InitializeNLPHelpers(obj)
            
            % Initialization for quadrature type functions
            if obj.PathFuncManager.GetHasDynFunctions()
                obj.dynFuncProps = obj.PathFuncManager.GetDynFuncProperties();
            end
            if obj.PathFuncManager.GetHasCostFunction()
                obj.costFuncProps = obj.PathFuncManager.GetCostFuncProperties();
            end
            obj.ComputeUserFunctions();
            obj.TransUtil.PrepareToOptimize(obj.dynFuncProps,obj.userDynFuncData,...
                obj.costFuncProps,obj.costIntFuncData,obj.DecVector.decisionVector);
            
            % Initialization for algebraic path functions
            if obj.PathFuncManager.GetHasAlgFunctions()
                %  Set up the properties for the algebraic functions
                obj.algFuncProps = obj.PathFuncManager.GetAlgFuncProperties();
                obj.algFuncProps.SetHasStateVars(obj.Config.GetHasStateVars());
                obj.algFuncProps.SetHasControlVars(obj.Config.GetHasControlVars());
                % Build array needed by NLP util but computed by Coll util
                numPathConPoints = obj.TransUtil.GetNumPathConstraintPoints();
                dTimedTi = zeros(numPathConPoints);
                dTimedTf = zeros(numPathConPoints);
                for pointIdx = 1:obj.TransUtil.GetNumPathConstraintPoints()
                    meshIdx = obj.userAlgFuncData{pointIdx}.GetMeshIdx();
                    stageIdx = obj.userAlgFuncData{pointIdx}.GetStageIdx();
                    dTimedTi(pointIdx) = obj.TransUtil.GetdCurrentTimedTI(meshIdx,stageIdx);
                    dTimedTf(pointIdx) = obj.TransUtil.GetdCurrentTimedTF(meshIdx,stageIdx);
                end
                % Intialize the NLP util
                import collutils.NLPFuncUtil_AlgPath
                obj.algPathNLPFuncUtil = NLPFuncUtil_AlgPath();
                obj.algPathNLPFuncUtil.Initialize(...
                    obj.algFuncProps,obj.userAlgFuncData,obj.DecVector.numDecisionParams,...
                    obj.TransUtil.GetNumPathConstraintPoints(),dTimedTi,dTimedTf);
            end
        end
        
        function obj = PrepareToOptimize(obj)
            %  Init to be done after all phases are initialized but BEFORE optimization
            obj.ComputeSparsityPattern();
        end
        
        function [numAZerosTot,numBZerosTot,numQZerosTot] = GetNumNLPNonZeros(obj)
            %  Get number of defect contraints
            numAZerosTot = 0;
            numBZerosTot = 0;
            numQZerosTot = 0;
            if obj.Config.GetHasAlgPathCons()
                [numAZeros,numBZeros,numQZeros] = obj.algPathNLPFuncUtil.GetMatrixNumZeros();
                numAZerosTot = numAZerosTot + numAZeros;
                numBZerosTot = numBZerosTot + numBZeros;
                numQZerosTot = numQZerosTot + numQZeros;
            end
            if obj.Config.GetHasDefectCons()
                [numAZeros,numBZeros,numQZeros] = obj.TransUtil.GetDefectMatrixNumZeros();
                numAZerosTot = numAZerosTot + numAZeros;
                numBZerosTot = numBZerosTot + numBZeros;
                numQZerosTot = numQZerosTot + numQZeros;
            end
        end
        function SetIsRefining(obj,flag)
            obj.isRefining = flag;
        end
        
        function [constraintVec] = GetConstraintVector(obj)
            % Assemble the total constraint vector for the phase
            
            %  Update functions if they need to be recomputed
            obj.CheckFunctions()
            
            %  Assemble the total constraint vector
            if obj.PathFuncManager.GetHasDynFunctions()
                constraintVec(obj.defectConStartIdx:obj.defectConEndIdx,1) =  obj.defectConVec;
            end
            if obj.PathFuncManager.GetHasAlgFunctions()
                constraintVec(obj.algPathConStartIdx:obj.algPathConEndIdx,1) = obj.algPathConVec;
            end
        end
        
        function constraintNames = GetConstraintVectorNames(obj)
            % Assemble the total constraint vector descriptions cell array for the phase
            
            %  Assemble the total constraint vector
            if obj.PathFuncManager.GetHasDynFunctions()
                meshIdx = 1;
                nStateVars = 0;
                for i = obj.defectConStartIdx:obj.defectConEndIdx
                    nStateVars = nStateVars + 1;
                    constraintNames{i,1} = ['Phase ',num2str(obj.phaseNum),', Mesh Index ',num2str(meshIdx),': State Variable ',num2str(nStateVars)];
                    if obj.Config.GetNumStateVars() == nStateVars
                        meshIdx = meshIdx + 1;
                        nStateVars = 0;
                    end
                end
            end
            if obj.PathFuncManager.GetHasAlgFunctions()
                meshIdx = 1;
                nAlgFuns = 0;
                for i = obj.algPathConStartIdx:obj.algPathConEndIdx
                    nAlgFuns = nAlgFuns + 1;
                    if isempty(obj.algPathConVecNames)
                        constraintNames{i,1} = ['Phase ',num2str(obj.phaseNum),', Mesh Index ',num2str(meshIdx),': User Path Constraint ',num2str(nAlgFuns)];
                    else
                        constraintNames{i,1} = ['Phase ',num2str(obj.phaseNum),', Mesh Index ',num2str(meshIdx),': ',obj.algPathConVecNames{nAlgFuns}];
                    end
                    if obj.PathFuncManager.GetNumAlgFunctions() == nAlgFuns
                        meshIdx = meshIdx + 1;
                        nAlgFuns = 0;
                    end
                end
            end
        end
        
        %  Sets decision vector provided by optimizer
        function obj = SetDecisionVector(obj,decVector)
            %  Sets the decision vector given a vector
            
            %  Set the decision vector
            if length(decVector) == obj.Config.GetNumDecisionVarsNLP()
                obj.DecVector.SetDecisionVector(decVector);
                %  Update the time data on the Coll util helper
                timeVector = obj.DecVector.GetTimeVector();
                obj.TransUtil.SetTimeVector(timeVector(1),timeVector(2))
                obj.recomputeUserFunctions = true();
                obj.recomputeQuadratures = true();
                obj.recomputeAlgPathConstraints = true();
                obj.recomputeAlgCostFunction = true();
                %  Reset cost Jacobian to zeros so that summing elements
                %  does not sum across iterations.
                obj.nlpCostJac = sparse(1,obj.Config.GetNumDecisionVarsNLP());
            else
                errorMsg = ['Length of decisionVector must be ' ...
                    ' equal to totalnumDecisionVarsNLP'];
                errorLoc  = 'Phase:SetDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
        end
        
        function costFunction = GetCostFunction(obj)
            %  Computes and returns cost function
            obj.CheckFunctions()
            costFunction = 0;
            if obj.PathFuncManager.GetHasCostFunction()
                costFunction = costFunction + obj.costFuncIntegral;
            end
        end
        
        function ComputeDefectConstraints(obj)
            %  Insert the defect constraints  and Jacobian into the constraint vector
            
            %  If not intialized, cannot compute functions.
            if ~obj.isInitialized
                return
            end
            [funcValues,defJacArray] = obj.TransUtil.ComputeDefectFunAndJac(...
                obj.userDynFuncData,obj.DecVector.decisionVector);
            obj.defectConVec = funcValues;
            obj.InsertJacobianRowChunk(defJacArray,obj.defectConStartIdx:obj.defectConEndIdx);
            
        end
        
        function ComputeIntegralCost(obj)
            %  Insert the integral cost  and Jacobian into the appropriatelocations
            
            %  If not intialized, cannot compute functions.
            if ~obj.isInitialized
                return
            end
            [funcValue,costJacArray] = obj.TransUtil.ComputeCostFunAndJac(...
                obj.costIntFuncData,obj.DecVector.decisionVector);
            obj.costFuncIntegral = funcValue;
            obj.nlpCostJac = costJacArray;
        end
        
        function ComputeAlgPathConstraints(obj)
            %  Insert alg path constraints and Jacobian into the constraint vector
            
            %  If not intialized, cannot compute functions.
            if ~obj.isInitialized
                return
            end
            [funcValues,jacValues] = obj.algPathNLPFuncUtil.ComputeFuncAndJac(obj.userAlgFuncData);
            obj.algPathConVec = funcValues;
            obj.InsertJacobianRowChunk(jacValues,obj.algPathConStartIdx:obj.algPathConEndIdx);
        end
        
        %  For debugging, allows you to set user functions to recompute
        %  even when there is not change in decision vector.
        function SetRecomputeUserFunctions(obj,flag)
            obj.recomputeUserFunctions = flag;
        end
        
        %  Returns the total Jacobian of the cost function by summing
        %  Jacobian of algebraic and quadrature terms
        function jacArray = GetCostJacobian(obj)
            jacArray = obj.nlpCostJac;
        end
        
        function jacArray = GetConJacobian(obj)
            %  Returns the total Jacobian of the constraints
            jacArray = obj.nlpConstraintJac;
        end
        
        function sparsityPattern = GetCostSparsityPattern(obj)
            % Returns sparse array with sparsity pattern of cost
            sparsityPattern = obj.costSparsityPattern;
        end
        
        function sparsityPattern = GetConSparsityPattern(obj)
            % Returns sparse array with sparsity pattern of constraints
            sparsityPattern = obj.conSparsityPattern;
        end
        
        function  timeMat = GetInitialFinalTime(obj)
            %  Returns the time decision variables in [2x1] matrix form
            timeMat = [obj.DecVector.GetFirstTime();...
                obj.DecVector.GetLastTime()];
        end
        
        function DebugPathFunction(obj)
            obj.ComputePathFunctions()
        end
        
    end
    
    methods (Access = protected)
        
        function InitializepathFuncInputData(obj)
            obj.pathFuncInputData.Initialize(obj.Config.GetNumStateVars(),obj.GetNumControlVars());
        end
        
        function IntializeJacobians(obj)
            obj.nlpConstraintJac = sparse(obj.Config.GetNumTotalConNLP(),...
                obj.Config.GetNumDecisionVarsNLP());
            obj.nlpCostJac = sparse(1,obj.Config.GetNumDecisionVarsNLP());
        end
        
        function PrepareForMeshRefinement(obj)
            if obj.isRefining
                obj.PathFuncManager.SetIsInitializing(true);
                obj.oldTimeVector = obj.TransUtil.GetTimeVector();
                obj.oldState = obj.DecVector.GetStateArray();
                obj.oldControl = obj.DecVector.GetControlArray();
                obj.Config.SetTimeInitialGuess(obj.DecVector.GetFirstTime());
                obj.Config.SetTimeFinalGuess(obj.DecVector.GetLastTime());
            end
        end
        
        function SetInitialGuess(obj)
            
            if ~obj.isRefining
                obj.SetInitialGuessFromGuessGen();
            else
                
                % TODO: THIS IS RADAU SPECIFIC BY ASSUMING FINAL CONTROL NOT INCLUDED.
                newStateArray = zeros(obj.TransUtil.GetNumTimePoints(),...
                    obj.Config.GetNumStateVars());
                newControlArray = zeros(obj.TransUtil.GetNumTimePoints()-1,...
                    obj.GetNumControlVars());
                newTimeVector = obj.TransUtil.GetTimeVector();
                for stateIdx = 1:obj.Config.GetNumStateVars()
                    newStateArray(:,stateIdx) = interp1(obj.oldTimeVector,...
                        obj.oldState(:,stateIdx),newTimeVector,'linear');
                end
                for controlIdx = 1:obj.GetNumControlVars()
                    newControlArray(:,controlIdx) = interp1(obj.oldTimeVector(1:end-1),...
                        obj.oldControl(:,controlIdx),...
                        newTimeVector(1:end-1),'linear','extrap');
                end
                obj.DecVector.SetStateArray(newStateArray);
                obj.DecVector.SetControlArray(newControlArray);
            end
            
        end
        
        function InitializeDecisionVector(obj)
            %  Initialize the decision vector helper class
            
            obj.DecVector.Initialize(obj.Config.GetNumStateVars(), ...
                obj.GetNumControlVars,obj.Config.GetNumIntegralVars,...
                obj.Config.GetNumStaticVars(),...
                obj.TransUtil.GetNumStatePoints(),...
                obj.TransUtil.GetNumControlPoints(),...
                obj.TransUtil.GetNumStateStagePointsPerMesh(),...
                obj.TransUtil.GetNumControlStagePointsPerMesh());
        end
        
        function CheckFunctions(obj)
            %  Recomputes user functions and/or quadratures if stale
            
            %  Update functions if necessary.
            if obj.recomputeUserFunctions
                obj.ComputeUserFunctions();
            end
            %  Update quadratures if necessary
            if obj.recomputeQuadratures
                if obj.Config.GetHasDefectCons()
                    obj.ComputeDefectConstraints()
                end
                if obj.PathFuncManager.GetHasCostFunction()
                    obj.costFuncIntegral = 0;
                    obj.ComputeIntegralCost()
                end
                obj.recomputeQuadratures = false();
            end
            %  Update Algebraic path constraints
            if obj.recomputeAlgPathConstraints && obj.Config.GetHasAlgPathCons()
                obj.ComputeAlgPathConstraints()
                obj.recomputeAlgPathConstraints = false();
            end
            %  Update cost function
            if obj.recomputeAlgCostFunction && obj.Config.GetHasAlgebraicCost()
                obj.ComputeAlgCostFunction()
                obj.recomputeAlgCostFunction = false();
            end
        end
        
        function ComputeUserFunctions(obj)
            %  Computes user path and point functions
            obj.ComputePathFunctions();
            obj.recomputeUserFunctions = false();
            obj.recomputeQuadratures = true();
        end
        
        %  Compute the path functions
        function ComputePathFunctions(obj)
            %  Computes user path functions
            
            %  Compute user functions at each point
            timeVectorType = obj.TransUtil.GetTimeVectorType();
            for pointCnt = 1:obj.TransUtil.GetNumTimePoints()
                
                %  Extract info on the current mesh/stage point
                pointType = timeVectorType(pointCnt);
                meshIdx = obj.TransUtil.GetMeshIndex(pointCnt);
                stageIdx = obj.TransUtil.GetStageIndex(pointCnt);
                timeIdxs = obj.DecVector.GetTimeIdxs();
                stateIdxs = obj.DecVector.GetStateIdxsAtMeshPoint(meshIdx,stageIdx);
                controlIdxs = obj.DecVector.GetControlIdxsAtMeshPoint(meshIdx,stageIdx);
                
                %  Prepare the user function data structures then call the
                %  user function
                obj.PreparePathFunction(meshIdx,stageIdx,pointType,pointCnt);
                import userfunutils.PathFunctionContainer;
                funcData = PathFunctionContainer();
                funcData.Initialize();
                
                % Evaluate user functions and Jacobians
                funcData = obj.PathFuncManager.EvaluateUserFunction(obj.pathFuncInputData,funcData);
                funcData = obj.PathFuncManager.EvaluateUserJacobian(obj.pathFuncInputData,funcData);
                
                %  Handle defect constraints
                if obj.PathFuncManager.GetHasDynFunctions()
                    obj.userDynFuncData{pointCnt} = funcData.dynData;
                    obj.userDynFuncData{pointCnt}.SetNLPData(...
                        pointType,meshIdx,stageIdx,timeIdxs,...
                        stateIdxs,controlIdxs);
                end
                
                %  Handle cost function
                if obj.PathFuncManager.GetHasCostFunction()
                    obj.costIntFuncData{pointCnt} = funcData.costData;
                    obj.costIntFuncData{pointCnt}.SetNLPData(...
                        pointType,meshIdx,stageIdx,timeIdxs,...
                        stateIdxs,controlIdxs);
                end
                
                %  Handle algebraic constraints
                if obj.PathFuncManager.GetHasAlgFunctions()
                    obj.userAlgFuncData{pointCnt} = funcData.algData;
                    obj.userAlgFuncData{pointCnt}.SetNLPData(...
                        pointType,meshIdx,stageIdx,timeIdxs,...
                        stateIdxs,controlIdxs);
                end
                
            end
        end
        
        function ComputeSparsityPattern(obj)
            %  Computes the sparsity of the Phase NLP problem.
            
            obj.conSparsityPattern = sparse(obj.Config.GetNumTotalConNLP(),...
                obj.Config.GetNumDecisionVarsNLP());
            obj.costSparsityPattern = sparse(1,obj.Config.GetNumDecisionVarsNLP());
            if obj.Config.GetHasAlgPathCons()
                obj.conSparsityPattern(obj.algPathConStartIdx:obj.algPathConEndIdx,:) = ...
                    obj.algPathNLPFuncUtil.ComputeSparsity();
            end
            if obj.Config.GetHasDefectCons()
                obj.conSparsityPattern(obj.defectConStartIdx:obj.defectConEndIdx,:) = ...
                    obj.TransUtil.ComputeDefectSparsityPattern();
            end
            if obj.Config.GetHasIntegralCost()
                obj.costSparsityPattern = obj.costSparsityPattern + ...
                    obj.TransUtil.ComputeCostSparsityPattern();
            end
            
        end
        
        function SetProblemCharacteristics(obj)
            %  Set variable dependencies and function type flags
            
            if obj.PathFuncManager.GetHasAlgFunctions()
                obj.Config.SetHasAlgPathCons(true);
            end
            if obj.PathFuncManager.GetHasDynFunctions
                obj.Config.SetHasDefectCons(true);
            end
            if obj.PathFuncManager.GetHasCostFunction()
                obj.Config.SetHasIntegralCost(true);
            end
            
        end
        
        function IntializeUserFunctions(obj)
            
            %  Initialize the path function
            paramData = userfunutils.FunctionInputData();
            paramData.Initialize(obj.GetNumStateVars(),obj.GetNumControlVars());
            paramData.SetStateVec(obj.GetStateUpperBound());
            paramData.SetControlVec(obj.GetControlUpperBound());
            paramData.SetTime(obj.GetTimeUpperBound());
            paramData.SetPhaseNum (obj.phaseNum);
            
            %  Create bounds data needed for initialization
            boundData.stateLowerBound = obj.GetStateLowerBound();
            boundData.stateUpperBound = obj.GetStateUpperBound();
            boundData.controlLowerBound = obj.GetControlLowerBound();
            boundData.controlUpperBound = obj.GetControlUpperBound();
            boundData.timeUpperBound = obj.GetTimeUpperBound();
            boundData.timeLowerBound = obj.GetTimeLowerBound();
            
            import userfunutils.PathFunctionContainer;
            funcData= userfunutils.PathFunctionContainer();
            funcData.Initialize();
            
            obj.PathFuncManager.Initialize(obj.pathFunctionObj,paramData,funcData,boundData);
            
            obj.algPathConVecNames = funcData.algData.GetFunctionNames();
        end
        
        %  Set indeces for chunks of the constraint vector
        function obj = SetConstraintProperties(obj)
            %  Computes number of constraints and start/end indeces
            
            %  Set chunk values and bounds.
            obj.numAlgPathConNLP = obj.TransUtil.GetNumPathConstraintPoints()*...
                obj.PathFuncManager.GetNumAlgFunctions();
            obj.Config.SetNumTotalConNLP(obj.Config.GetNumDefectConNLP() + obj.numAlgPathConNLP);
            
            %  Set start and end indeces of different constraint
            %  parameter types.  These describe where different chunks
            %  of the constraint vector begin and end.
            obj.defectConStartIdx = 1;
            obj.defectConEndIdx  = obj.defectConStartIdx + obj.Config.GetNumDefectConNLP() - 1;
            % TODO.  If certain type of constraint does not exist, indeces
            % are populated with useless data that makes it look like they
            % exist.  May want to change that to be zeros or something.
            obj.algPathConStartIdx = obj.defectConEndIdx + 1;
            obj.algPathConEndIdx = obj.algPathConStartIdx + obj.numAlgPathConNLP -1;
        end
        
        function obj = SetDecisionVectorBounds(obj)
            %  Set the upper and lower bounds on the decision vector
            
            %  Dimension the vectors
            obj.decisionVecLowerBound = zeros(obj.Config.GetNumDecisionVarsNLP(),1);
            obj.decisionVecUpperBound = zeros(obj.Config.GetNumDecisionVarsNLP(),1);
            
            %  Use a temporary decision vector object for bookkeeping
            import collutils.DecVecTypeBetts
            boundVector = DecVecTypeBetts();
            
            boundVector.Initialize(obj.Config.GetNumStateVars(), ...
                obj.GetNumControlVars,obj.Config.GetNumIntegralVars(),...
                obj.Config.GetNumStaticVars(),...
                obj.TransUtil.GetNumStatePoints(),obj.TransUtil.GetNumControlPoints(),...
                obj.TransUtil.GetNumStateStagePointsPerMesh(),...
                obj.TransUtil.GetNumControlStagePointsPerMesh());
            
            %  Assemble the state bound array
            upperBoundStateArray = zeros(obj.DecVector.numStatePoints,...
                obj.Config.GetNumStateVars());
            lowerBoundStateArray = zeros(obj.DecVector.numStatePoints,...
                obj.Config.GetNumStateVars());
            for rowIdx = 1:obj.DecVector.numStatePoints
                lowerBoundStateArray(rowIdx,:) = obj.GetStateLowerBound();
                upperBoundStateArray(rowIdx,:) = obj.GetStateUpperBound();
            end
            
            %  Assemble the control bound array
            if obj.Config.GetHasControlVars()
                upperBoundControlArray = zeros(obj.DecVector.numControlPoints,...
                    obj.GetNumControlVars());
                lowerBoundControlArray = zeros(obj.DecVector.numControlPoints,...
                    obj.GetNumControlVars());
                for rowIdx = 1:obj.DecVector.numControlPoints
                    lowerBoundControlArray(rowIdx,:) = obj.GetControlLowerBound();
                    upperBoundControlArray(rowIdx,:) = obj.GetControlUpperBound();
                end
            end
            
            %  Set time vector bounds
            upperBoundTimeArray = [obj.GetTimeUpperBound() obj.GetTimeUpperBound() ]';
            lowerBoundTimeArray = [obj.GetTimeLowerBound() obj.GetTimeLowerBound() ]';
            
            %  Set lower bound vector
            boundVector.SetStateArray(lowerBoundStateArray);
            if obj.Config.GetHasControlVars()
                boundVector.SetControlArray(lowerBoundControlArray);
            end
            boundVector.SetTimeVector(lowerBoundTimeArray);
            obj.decisionVecLowerBound = boundVector.decisionVector;
            
            %  Set upper bound vector
            boundVector.SetStateArray(upperBoundStateArray);
            if obj.Config.GetHasControlVars()
                boundVector.SetControlArray(upperBoundControlArray);
            end
            boundVector.SetTimeVector(upperBoundTimeArray);
            obj.decisionVecUpperBound = boundVector.decisionVector;
            
        end
        
        %
        function InitializeTimeVector(obj)
            %  Initializes the dimensional time vector
            obj.DecVector.SetTimeVector([obj.GetTimeInitialGuess(); obj.GetTimeFinalGuess()]);
            obj.TransUtil.SetTimeVector(obj.GetTimeInitialGuess(), obj.GetTimeFinalGuess());
        end
        
        %  Set bounds on constraints
        function obj = SetConstraintBounds(obj)
            %  Set bounds for all constraint types
            
            %  Computes the bounds for each constraint type.
            obj = SetDefectConstraintBounds(obj);
            obj = SetPathConstraintBounds(obj);
            
            %  Now insert chunks into the complete bounds vector
            dStartIdx = obj.defectConStartIdx;
            dEndIdx = obj.defectConEndIdx;
            pStartIdx = obj.algPathConStartIdx;
            pEndIdx = obj.algPathConEndIdx;
            
            %  Concatenate defect constraints bounds if any
            if obj.PathFuncManager.GetHasDynFunctions()
                obj.allConLowerBound(dStartIdx:dEndIdx,1) = obj.defectConLowerBound;
                obj.allConUpperBound(dStartIdx:dEndIdx,1) = obj.defectConUpperBound;
            end
            
            %  Concatenate algebraic path constraint bounds if any
            if obj.PathFuncManager.GetHasAlgFunctions()
                obj.allConLowerBound(pStartIdx:pEndIdx,1) = obj.algPathConLowerBound;
                obj.allConUpperBound(pStartIdx:pEndIdx) = obj.algPathConUpperBound;
            end
            
        end
        
        function obj = SetDefectConstraintBounds(obj)
            % Sets bounds on defect constraints
            obj.defectConLowerBound = zeros(obj.Config.GetNumDefectConNLP(),1);
            obj.defectConUpperBound = zeros(obj.Config.GetNumDefectConNLP(),1);
        end
        
        function obj = SetPathConstraintBounds(obj)
            % Sets bounds on the path constraints
            if obj.PathFuncManager.GetHasAlgFunctions()
                obj.algPathConLowerBound = zeros(obj.numAlgPathConNLP,1);
                obj.algPathConUpperBound = zeros(obj.numAlgPathConNLP,1);
                lowIdx = 1;
                for conIdx = 1:obj.TransUtil.GetNumPathConstraintPoints()
                    highIdx = lowIdx + obj.PathFuncManager.GetNumAlgFunctions()-1;
                    obj.algPathConLowerBound(lowIdx:highIdx,1) = ...
                        obj.PathFuncManager.GetAlgFunctionsLowerBounds();
                    obj.algPathConUpperBound(lowIdx:highIdx,1) = ...
                        obj.PathFuncManager.GetAlgFunctionsUpperBounds();
                    lowIdx = lowIdx + obj.PathFuncManager.GetNumAlgFunctions();
                end
            else
                obj.algPathConLowerBound = [];
                obj.algPathConUpperBound = [];
            end
        end
        
        
        function obj = SetInitialGuessFromGuessGen(obj)
            %  Calls guess utility to compute guess for state and control
            
            %  Intialize the guess generator class
            obj.GuessGen.Initialize(...
                obj.TransUtil.GetTimeVector(),...
                obj.DecVector.numStateVars,...
                obj.DecVector.numStatePoints,...
                obj.DecVector.numControlVars,...
                obj.DecVector.numControlPoints,...
                obj.initialGuessMode);
            
            switch obj.initialGuessMode
                case {'LinearNoControl','LinearUnityControl','LinearCoast'}
                    [xGuessMat, uGuessMat] = obj.GuessGen.ComputeLinearGuess(...
                        obj.GetStateInitialGuess(),obj.GetStateFinalGuess());
                case 'UserGuessFunction'
                    [xGuessMat, uGuessMat] = obj.GuessGen.ComputeUserFunGuess(...
                        obj.guessFunctionName,obj.TransUtil.GetTimeVectorType,obj);
                case 'UserGuessObject'
                    [xGuessMat, uGuessMat] = obj.GuessGen.ComputeGuessFromObject(...
                        obj.guessObject,obj.TransUtil.GetTimeVectorType,obj);
            end
            
            %  Call the decision vector and populate with the guess
            obj.DecVector.SetStateArray(xGuessMat);
            obj.DecVector.SetControlArray(uGuessMat);
            
        end
        
        function PreparePathFunction(obj,meshIdx,stageIdx,pointType,pointIdx)
            %  Prepares user path function evaluation at a specific point
            
            %  This function extracts the state, control, and time from decision vector
            obj.pathFuncInputData.SetPhaseNum(obj.phaseNum);
            if pointType == 1 || pointType == 2
                obj.pathFuncInputData.SetStateVec(...
                    obj.DecVector.GetStateAtMeshPoint(meshIdx,stageIdx));
            else
                % TODO: Fix NaN eval. Will not work in C++
                obj.pathFuncInputData.SetControlVec(obj.PathFuncManager.stateVec*NaN);
            end
            if pointType == 1 || pointType == 3
                obj.pathFuncInputData.SetControlVec(...
                    obj.DecVector.GetControlAtMeshPoint(meshIdx,stageIdx));
            else
                % TODO: Fix NaN eval. Will not work in C++
                obj.pathFuncInputData.SetControlVec(ones(obj.GetNumControlVars(),1)*NaN);
            end
            obj.pathFuncInputData.SetTime(obj.TransUtil.GetTimeAtMeshPoint(pointIdx));
            
        end
        
        function InsertConJac(obj,jacArray,rowIdxVec,colIdxVec)
            %  Insert a subarray into the phase Jacobian
            obj.nlpConstraintJac(rowIdxVec,colIdxVec) = jacArray;
        end
        
        function InsertJacobianRowChunk(obj,jacChunk,funIdxs)
            % Insert jacobian row chunk into sparse jacobian
            obj.nlpConstraintJac(funIdxs,:) = jacChunk;
        end
        
    end
    
end

