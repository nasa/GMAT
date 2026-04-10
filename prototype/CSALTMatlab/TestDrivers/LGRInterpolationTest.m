function void()
    % initialize NLPFuncUtil_Radau < NLPFuncUtil_Coll

    [obj.radauPoints,obj.radauWeights,obj.radauDiffMatrix]...
                    = lgrPS([-1 -0.5 0 0.5 1],[7 7 7 7]);
    obj.numMeshPoints = length(obj.radauWeights);
    obj.numStatePoints = obj.numMeshPoints + 1;
    obj.numControlPoints = obj.numMeshPoints;
    obj.radauPointsType = ones(obj.numStatePoints,1);
    obj.radauPointsType(obj.numStatePoints,1) = 2;
    obj.stateInterpolator = BarycentricLagrangeInterpolator;    
    obj.stateInterpolator.SetIndependentVariables(obj.radauPoints);
    stateMat=[sin(obj.radauPoints) cos(obj.radauPoints) tan(obj.radauPoints)];
    controlMat=[sin(obj.radauPoints(1:end-1)) cos(obj.radauPoints(1:end-1)) tan(obj.radauPoints(1:end-1))];
    
    newNormTimeVec=(obj.radauPoints(1:end-1)+obj.radauPoints(2:end))/2;
    trueNewStateMat=[sin(newNormTimeVec) cos(newNormTimeVec) tan(newNormTimeVec)];
    trueNewControlMat=trueNewStateMat;
    
    newStateMat=InterpolateState(obj,newNormTimeVec);    
    newControlMat=InterpolateControl(obj,newNormTimeVec);
    figure; subplot(1,2,1); plot(obj.radauPoints,stateMat);
    hold on
    plot(newNormTimeVec,newStateMat,'*','markersize',15);
    hold off
    subplot(1,2,2); plot(newNormTimeVec,newStateMat-trueNewStateMat,'*','markersize',15);
    
    figure; subplot(1,2,1); plot(obj.radauPoints(1:end-1),controlMat);
    hold on
    plot(newNormTimeVec,newControlMat,'*','markersize',15);
    hold off
    subplot(1,2,2); plot(newNormTimeVec,newControlMat-trueNewControlMat,'*','markersize',15);
    
    
end
function newStateMat=InterpolateState(obj,newNormTimeVec)
    % input must be a vector s.t. max(newNormTimeVec) < 1 
    % & min(newNormTimeVec) > -1;

    [numRows, numCols]=size(newNormTimeVec);
    if numRows ~= 1 && numCols ~= 1
        error('input must be a column or row vector');
    elseif numCols ~=1
            newNormTimeVec=newNormTimeVec.';                
    end
    if min(newNormTimeVec) < -1 || max(newNormTimeVec) > 1
        error('the range of the normalized time sequence must be [-1,1].')
    end
%     stateMat = obj.ptrPhase.DecVector.GetStateArray();
    stateMat=[sin(obj.radauPoints) cos(obj.radauPoints) tan(obj.radauPoints)];
    newStateMat = obj.stateInterpolator.Interpolate(newNormTimeVec,stateMat);        
end
function newControlMat=InterpolateControl(obj,newNormTimeVec)            
    [numRows, numCols]=size(newNormTimeVec);
    if numRows ~= 1 && numCols ~= 1
        error('input must be a column or row vector');
    elseif numCols ~=1
            newNormTimeVec=newNormTimeVec.';                
    end
    if min(newNormTimeVec) < -1 || max(newNormTimeVec) > 1
        error('the range of the normalized time sequence must be [-1,1].')
    end
%     controlMat = obj.ptrPhase.DecVector.GetControlArray();
    controlMat=[sin(obj.radauPoints(1:end-1)) cos(obj.radauPoints(1:end-1)) tan(obj.radauPoints(1:end-1))];
    newControlMat = (spline(obj.radauPoints(1:end-1),controlMat.',newNormTimeVec)).';
    % matlab cubic spline works for row vector but not for column
    % vectors!

end            