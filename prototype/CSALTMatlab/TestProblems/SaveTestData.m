
fileName = 'C:\Users\sphughe1\Documents\Repos\BitBucket\TestData\OldVersion\ConwayOrbitExample_TestData.mat';

solDecVec = [];
for phaseIdx = 1:traj.numPhases
   solDecVec = [solDecVec ; traj.phaseList{phaseIdx}.DecVector.decisionVector];
end

[solFuncVec,SolJacVec] = SNOPTFunctionWrapper(solDecVec)

solSparsJac = traj.GetJacobian(1)
solSparsJacPattern = traj.sparsityPattern;

save(fileName,'solDecVec','solFuncVec','SolJacVec','solSparsJac','solSparsJacPattern','traj')


