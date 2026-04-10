testProb = 'TP239';
isNewX = false;

% ProblemSet = {
%     'TP239';
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
%       'LPR_T1_2' %MiNLP fails
%     'LPR_T1_5'; % SNOPT issue
%     'SGR_P1_2';
%     'PLR_T1_2'; % SNOPT issue
%     'SLR_T1_1'; % SNOPT issue
%     'QLR_T1_2' % SNOPT issue
%     };

ProblemSet = {'Dummy_01'}
clc
for probIdx = 1:length(ProblemSet)
    
    % Create the user problem object
    testProb = ProblemSet{probIdx};
    userProblem = MinNLPOOTestProb(testProb);
    
    % Call basic interfaces for problem characteristics
    [numVars,numNonLinCon,numLinCon] = userProblem.GetNLPInfo();
    [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = ...
        userProblem.GetBoundsInfo(numVars,numNonLinCon,numLinCon);
    startingPoint = userProblem.GetStartingPoint();
    
    % Evaluate cost function and Jacobian
    [costFunc] = userProblem.EvaluateCostFunc(numVars,startingPoint,isNewX)
    [costJacobian] = userProblem.EvaluateCostJac(numVars,startingPoint,isNewX)
    
    % Evaluate constraint functions and Jacobian
    [conFunc] = userProblem.EvaluateConFunc(numVars,startingPoint,isNewX)
    [conJacobian] = userProblem.EvaluateConJac(numVars,startingPoint,isNewX)
    
end