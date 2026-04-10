%% Step 1:  Load problem specific data and configure problem characteristics
BrysonMaxRangeData;

%%  Configure the problem characteristics for BrysonMax Range problem
probChar = collutils.ProblemCharacteristics();
probChar.SetNumStateVars(numStateVars);
probChar.SetNumControlVars(numControlVars);
probChar.SetMeshIntervalFractions(meshIntervalFractions);
probChar.SetMeshIntervalNumPoints(meshIntervalNumPoints);
probChar.SetStateLowerBound(stateLowerBound)
probChar.SetStateUpperBound(stateUpperBound)
probChar.SetStateInitialGuess(initialGuessState)
probChar.SetStateFinalGuess(finalGuessState)
probChar.SetTimeLowerBound(timeLowerBound)
probChar.SetTimeUpperBound(timeUpperBound)
probChar.SetTimeInitialGuess(initialGuessTime)
probChar.SetTimeFinalGuess(finalGuessTime)
probChar.SetControlLowerBound(controlLowerBound)
probChar.SetControlUpperBound(controlUpperBound)

%% Step 2: Create the nlpRadau and decision vector and perform first stage of intialization
nlpUtil = collutils.NLPFuncUtil_Radau();
nlpUtil.Initialize(probChar);
nlpUtil.InitializeTranscription();

decVector = collutils.DecVecTypeBetts();
decVector.Initialize(probChar.GetNumStateVars(), ...
    probChar.GetNumControlVars,probChar.GetNumIntegralVars,...
    probChar.GetNumStaticVars(),...
    nlpUtil.GetNumStatePoints(),...
    nlpUtil.GetNumControlPoints(),...
    nlpUtil.GetNumStateStagePointsPerMesh(),...
    nlpUtil.GetNumControlStagePointsPerMesh());
decVector.SetDecisionVector(decVectorAtSolution);

timeVec = decVector.GetTimeVector();
nlpUtil.SetTimeVector(timeVec(1),timeVec(2));

%% Step 3.  Configure the user path function, manager, and associated helper classes 
paramData = userfunutils.FunctionInputData();
paramData.Initialize(probChar.GetNumStateVars(),probChar.GetNumControlVars());
paramData.SetStateVec(probChar.GetStateUpperBound());
paramData.SetControlVec(probChar.GetControlUpperBound());
paramData.SetTime(probChar.GetTimeUpperBound());
paramData.SetPhaseNum(1);

%  Create bounds data needed for initialization
boundData.stateLowerBound = probChar.GetStateLowerBound();
boundData.stateUpperBound = probChar.GetStateUpperBound();
boundData.controlLowerBound = probChar.GetControlLowerBound();
boundData.controlUpperBound = probChar.GetControlUpperBound();
boundData.timeUpperBound = probChar.GetTimeUpperBound();
boundData.timeLowerBound = probChar.GetTimeLowerBound();

import userfunutils.PathFunctionContainer;
funcData = userfunutils.PathFunctionContainer();
funcData.Initialize();

pathFuncManager = userfunutils.UserPathFunctionManager();
pathFunctionObject = BrysonMaxRangePathObj();
pathFuncManager.Initialize(pathFunctionObject,paramData,funcData,boundData);
dynFunProps = pathFuncManager.GetDynFuncProperties();
costFunProps = pathFuncManager.GetCostFuncProperties();
if pathFuncManager.GetHasAlgFunctions()
    probChar.SetHasAlgPathCons(true);
end
if pathFuncManager.GetHasDynFunctions
    probChar.SetHasDefectCons(true);
end
if pathFuncManager.GetHasCostFunction()
    probChar.SetHasIntegralCost(true);
end

%% Step 4.  Loop over the discretization points and evaluate the functions at each point
timeVectorType = nlpUtil.GetTimeVectorType();
pathFuncInputData = userfunutils.FunctionInputData();
pathFuncInputData.Initialize(probChar.GetNumStateVars(),probChar.GetNumControlVars());
for pointCnt = 1:nlpUtil.GetNumTimePoints()
    
    %  Extract info on the current mesh/stage point
    pointType = timeVectorType(pointCnt);
    meshIdx = nlpUtil.GetMeshIndex(pointCnt);
    stageIdx = nlpUtil.GetStageIndex(pointCnt);
    timeIdxs = decVector.GetTimeIdxs();
    stateIdxs = decVector.GetStateIdxsAtMeshPoint(meshIdx,stageIdx);
    controlIdxs = decVector.GetControlIdxsAtMeshPoint(meshIdx,stageIdx);
    
    % Prepare the user function data structures
    pathFuncInputData.SetPhaseNum(1);
    if pointType == 1 || pointType == 2
        pathFuncInputData.SetStateVec(...
            decVector.GetStateAtMeshPoint(meshIdx,stageIdx));
    else
        % TODO: Fix NaN eval. Will not work in C++
        obj.pathFuncInputData.SetControlVec(obj.PathFuncManager.stateVec*NaN);
    end
    if pointType == 1 || pointType == 3
        pathFuncInputData.SetControlVec(...
            decVector.GetControlAtMeshPoint(meshIdx,stageIdx));
    else
        % TODO: Fix NaN eval. Will not work in C++
        pathFuncInputData.SetControlVec(ones(probChar.GetNumControlVars(),1)*NaN);
    end
    pathFuncInputData.SetTime(nlpUtil.GetTimeAtMeshPoint(pointCnt));
    import userfunutils.PathFunctionContainer;
    funcData = PathFunctionContainer();
    funcData.Initialize();
    
    % Evaluate user functions and Jacobians
    funcData = pathFuncManager.EvaluateUserFunction(pathFuncInputData,funcData);
    funcData = pathFuncManager.EvaluateUserJacobian(pathFuncInputData,funcData);
    
    %  Handle defect constraints
    if pathFuncManager.GetHasDynFunctions()
        dynFunValues{pointCnt} = funcData.dynData;
        dynFunValues{pointCnt}.SetNLPData(...
            pointType,meshIdx,stageIdx,timeIdxs,...
            stateIdxs,controlIdxs);
    end
    
end

%% Step 5. Compute the defect constraints and Jacobians
costFunValues = {};
nlpUtil.PrepareToOptimize(dynFunProps,dynFunValues,costFunProps,costFunValues,decVector)
decVec = decVector.GetDecisionVector();
[funcValues,jacArray] = nlpUtil.ComputeDefectFunAndJac(dynFunValues,decVec);

funcValues
jacArray




