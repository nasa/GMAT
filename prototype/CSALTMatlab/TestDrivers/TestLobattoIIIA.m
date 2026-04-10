function TestLobattoIIIA

Y0 = [6000 0 1000 0 7.8 1.1]';
% Y0 = [13726.2251459063
%           16692.3353084274
%           9134.45913282995
%          0.813034167013597
%           5.01473066709156
%          0.871359033094766
% ]
odeOpt = odeset('AbsTol',1e-12,'RelTol',1e-12);
% tspan = [0 6000];
% [t,yHist] = ode113(@OrbitGravity,tspan,Y0,odeOpt);
% plot3(yHist(:,1),yHist(:,2),yHist(:,3))
% axis equal

stepSize = 20;
%lobUtil = collutils.LobattoIIIA_HSCompressed();
lobUtil = collutils.LobattoIIIA_8Order();
tspan = stepSize*lobUtil.GetStageTimes();
[t,yHist] = ode113(@OrbitGravity,tspan,Y0,odeOpt);

if length(tspan) == 2
    t = [t(1);t(end)]; %#ok<NASGU>
    yHist = [yHist(1,:);yHist(end,:)];
end

numFuncs = lobUtil.GetNumDefectCons;
numPointsPerStep = lobUtil.GetNumPointsPerStep();
numStates = length(Y0);
% Vectorize the states and the dynamics functions

decVec = reshape(yHist',numPointsPerStep*numStates,1);
funcArray = zeros(numStates,numPointsPerStep);
for i = 1:1:(numFuncs+1)
    funcArray(:,i) = OrbitGravity(0,yHist(i,:)');
end

if lobUtil.GetIsSeparated()
    %  Compute defect constraints for separated methods
   qVec = lobUtil.GetFuncVecFromArray(funcArray,numStates,stepSize);
   [aMat,bMat] = lobUtil.ComputeAandB(numStates)
   defectCons = aMat*decVec + bMat*qVec;
   disp(defectCons)
else
    %  Compute defect constraints for compressed methods
    % TODO: Expand to handle more than one defect contraint per step
    qVec = lobUtil.GetFuncVecFromArray(funcArray,numStates,stepSize);
    funcPattern = lobUtil.GetFuncDepPattern();
    qVec(1:numStates,1) = zeros(numStates,1);
    nvM1 = numStates - 1;
    for funIdx = 1:size(funcPattern,2)
        rowStartIdx = numStates*funIdx - nvM1;
        qVec(1:numStates,1) = qVec(1:numStates,1) + funcVec(rowStartIdx:rowStartIdx+nvM1);
    end
end

end

function [Ydot] = OrbitGravity(~,Y)
mu = 398600.4415;
Ydot(1:3,1) = Y(4:6);
Ydot(4:6,1) = -mu*Y(1:3,1)/norm(Y(1:3,1))^3;
end

