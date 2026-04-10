%  Report data about the problem
function WriteSolutionReport(obj)

phaseIdx = 1;

%  Write out states that have active bound constraints
decVector = obj.phaseList{phaseIdx}.DecVector.decisionVector;
if isa(decVector,'gradient')
    decVector = decVector.x;
end
upperBounds = obj.phaseList{phaseIdx}.decisionVecUpperBound;
upperBounds = obj.phaseList{phaseIdx}.decisionVecUpperBound;

%  Write out info on functions that have active constraints
for phaseIdx = 1:obj.numPhases
    conStatus = traj.phaseList{1}.CheckConstraintVariances();
    
end

end