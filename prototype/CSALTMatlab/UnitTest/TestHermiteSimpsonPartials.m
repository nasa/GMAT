

%  Brachistichrone tests
ClearAll
BrachistichroneHS_Main
J = traj.GetJacobian();
traj.phaseList{1}.costJac - J(1,:)
traj.phaseList{1}.jacArray - J(2:end,:)

return

int1S = traj.phaseList{1}.jacArray(61:63,1:2)
int1A  = J(62:64,1:2)
int1S - int1A

return

%  Hypersensitive tests
ClearAll
HyperSensitiveHS_Main
J = traj.GetJacobian();
traj.phaseList{1}.costJac - J(1,:)
traj.phaseList{1}.jacArray - J(2:end,:)

%  Orbit Raising Tests
ClearAll
OrbitRaisingHS_Main
J = traj.GetJacobian();
traj.phaseList{1}.costJac - J(1,:)
traj.phaseList{1}.jacArray - J(2:end,:)

return
%  Cost partials
Jan1 = traj.phaseList{1}.jacArray.x;
%  Constraint partials
conBetts = traj.phaseList{1}.ComputeConstraintsBetts().x;

conTruth = traj.phaseList{1}.GetContraintVector.x;

conTruth(1:3)
conBetts(1:3)



return

startIdx = 11;
endIdx = 20;
traj.phaseList{1}.costJac(1,startIdx:endIdx)
J(1,startIdx:endIdx)

startIdx = 1;
endIdx = 10;
traj.phaseList{1}.costJac(1,startIdx:endIdx)
J(1,startIdx:endIdx)
traj.phaseList{1}.costJac(1,startIdx:endIdx) - J(1,startIdx:endIdx)

pointIdx = 0;
for meshIdxIdx =1:traj.phaseList{1}.DecVector.numStateMeshPoints -2;
    for stageIdx = 0:1
    pointIdx = pointIdx + 1;
    currIdx = traj.phaseList{1}.DecVector.GetControlIdxsAtMeshPoint(meshIdxIdx,stageIdx);
    stateIdx(pointIdx,1) = currIdx;
    end
end   

traj.phaseList{1}.costJac(1,stateIdx)
J(1,stateIdx)
traj.phaseList{1}.costJac(1,stateIdx) - J(1,stateIdx)

%traj.phaseList{1}.jacArray - J(2:end,:)