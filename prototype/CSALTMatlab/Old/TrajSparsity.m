function [sparsityPattern] = TrajSparsity(obj)

%  Compute the sparsity pattern for constraints of this phase
%  with respect to variables in this phase.  These are the
%  diagonal blocks of the Trajectory sparsity pattern
iGfun = [];
jGvar = [];
initialGuess = obj.decisionVector();
sparseCnt = 0;
for decIdx = 1:obj.totalnumDecisionParams
    tempVec = initialGuess;
    tempVec(decIdx) = NaN;
    obj.SetDecisionVector(tempVec);
    conVec = obj.GetContraintVector();
    costVal = obj.GetCostFunction();
    totalVec = [costVal;conVec];
    for funIdx = 1:obj.totalnumConstraints + 1
        if isnan(totalVec(funIdx))
            sparseCnt = sparseCnt + 1;
            iGfun(sparseCnt,1) = funIdx;
            jGvar(sparseCnt,1) = decIdx;
        end
    end
    %  Set the initial guess back % mod by YK
    obj.SetDecisionVector(initialGuess);
end
%  Set the initial guess back
obj.SetDecisionVector(initialGuess);
sparsityPattern = sparse(iGfun,jGvar,1);