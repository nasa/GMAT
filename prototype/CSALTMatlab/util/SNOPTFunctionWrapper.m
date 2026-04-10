function [C,G] = SNOPTFunctionWrapper(z);
%  Compute the cost and constraints
% Construct the vector of constraints plus the objective function

global iGfun jGvar traj callCount
reDim = true();
nonDim = true();

if traj.ifScaling
	z = traj.ScaleHelper.UnScaleDecisionVector(z);
end

traj.SetDecisionVector(z,reDim);
C = traj.GetCostConstraintFunctions(nonDim);
J = traj.GetJacobian(nonDim);

if traj.ifScaling
	% CA = C;

	C(1) = traj.ScaleHelper.ScaleCostFunction(C(1));
	C(2:end) = traj.ScaleHelper.ScaleConstraintVector(C(2:end));
	J = traj.ScaleHelper.ScaleJacobian(J);
	
	% CB = C;
	% CB(1) = traj.ScaleHelper.UnScaleCostFunction(C(1));
	% CB(2:end) = traj.ScaleHelper.UnScaleConstraintVector(C(2:end));
	% disp(sum(CA-CB))
	% stop
end

if 0
	callCount = callCount + 1;
	% save(['Iteration',num2str(callCount),'_data'])
	[constraintFunctions,ifEquality] = traj.GetScaledConstraintFunctions();
	constraintNames = traj.GetConstraintVectorNames();

	worstConstraint = 0;
	worst_idx = 0;

	for i = 1:length(ifEquality)
		if constraintFunctions(i) < 0 && abs(constraintFunctions(i)) > worstConstraint
			worstConstraint = abs(constraintFunctions(i));
			worst_idx = i;
		elseif ifEquality(i) && worstConstraint < abs(constraintFunctions(i))
			worstConstraint = abs(constraintFunctions(i));
			worst_idx = i;
		elseif constraintFunctions(i) > 1 && constraintFunctions(i)-1 > worstConstraint
			worstConstraint = constraintFunctions(i) - 1;
			worst_idx = i;
		end
	end
	disp([constraintNames{worst_idx},': ',num2str(C(worst_idx+1)),' (',sprintf('%.3e',worstConstraint),')'])
end

if ~isempty(J)
    G = snfindG(iGfun,jGvar,J);
else
    G = [];
end