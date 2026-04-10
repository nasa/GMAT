



ProblemSet = {
    %     %'TP239'; %Fail with upper/lower bound implementation
    %     'TP238';
    %     'TP395';
    %     'TP394';
    %     'TP246';
    %     'TP201';
    %     'TP328'; % SNOPT issue
    %     'TP6';
    %     'PLR_T1_4';
    %     'QLR_T1_1';
    %     'SGR_P1_2';
    %     'TP6';
    %     'TP1';
    %     'TP218';
    %     'TP369';
    %     'TP242';% SNOPT issue
    %     'TP225'; % SNOPT issue
    %     'TP254'; % SNOPT issue
    %     'PQR_T1_6'; % SNOPT issue
    %     'LQR_T1_4';
    %     'LLR_T1_1' % SNOPT issue
    %     'LPR_T1_1'; % SNOPT issue
    'LPR_T1_2' %MiNLP fails
    'LPR_T1_5'; % SNOPT issue
    'SGR_P1_2';
    'PLR_T1_2'; % SNOPT issue
    'SLR_T1_1';
    'QLR_T1_2';
    'BoundsTest';
    };

conMode = 3;
testMode = 2;

for probIdx = 1:length(ProblemSet)
    
    if testMode == 1
        %% Test Non-Elastic Mode
        problemName = ProblemSet{probIdx};
        
        testProb = MinNLPOOTestProb(problemName,conMode);
        
        numUserVars = testProb.GetNLPInfo();
        isNewX = true;
        
        funManager = UserFunctionManager(testProb)
        decVector = funManager.GetNLPStartingPoint();
        
        % Test call to get problem dimensions
        [numVars,numCons] = funManager.GetNLPInfo()
        
        % Test call to get the upper and lower bounds
        [conLowerBounds,conUpperBounds] = funManager.GetNLPBoundsInfo()
        
        % Test call to compute all function values
        [fun,numFunEvals,conFunc,numGEvals] = ...
            funManager.EvaluteNLPFunctionsOnly(numUserVars,decVector,isNewX)
        
        % Test call to compute all derivatives
        [nlpCostJac,nlpConJac] = funManager.EvaluateNLPDerivsOnly(numUserVars,decVector,isNewX)
        
        % Test call to compute all functions and derivatives
        [nlpCost,nlpCostJac,numCostEvals,nlpConFunc,nlpConJac,numGEvals] = ...
            funManager.EvaluateAllNLPFuncJac(numVars,decVector,isNewX)
    end
    %% Test Non-Elastic Mode
    %% Test Non-Elastic Mode
    problemName = ProblemSet{probIdx};
    
    testProb = MinNLPOOTestProb(problemName,conMode);
    
    numUserVars = testProb.GetNLPInfo();
    isNewX = true;
    isModeElastic = true;
    funManager = UserFunctionManager(testProb,isModeElastic,1.0);
    decVector = funManager.GetNLPStartingPoint();
    % Test call to get problem dimensions
    [numVars,numCons] = funManager.GetNLPInfo()
    
    %     % Test call to get the upper and lower bounds
    [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = funManager.GetNLPBoundsInfo()
    
    %     % Test call to compute all function values
    [fun,numFunEvals,conFunc,numGEvals] = ...
        funManager.EvaluteNLPFunctionsOnly(numUserVars,decVector,isNewX)
    %
    %     % Test call to compute all derivatives
    [nlpCostJac,nlpConJac] = funManager.EvaluateNLPDerivsOnly(numUserVars,decVector,isNewX)
    %
    %     % Test call to compute all functions and derivatives
    [nlpCost,nlpCostJac,numCostEvals,nlpConFunc,nlpConJac,numGEvals] = ...
        funManager.EvaluateAllNLPFuncJac(numVars,decVector,isNewX)
    
end