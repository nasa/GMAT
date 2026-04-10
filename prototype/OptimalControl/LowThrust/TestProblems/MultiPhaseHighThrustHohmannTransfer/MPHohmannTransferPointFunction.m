function [obj] = MPHohmannTransferPointFunction(obj)

%  Only constraints are in phase 3
if obj.phaseNum ~= 3
    return
end

%%  Define constants
mu   = 1;

%%  Extract state and control components
rvf = obj.finalStateVec(1:3,1);
vvf = obj.finalStateVec(4:6,1);
mf  = obj.finalStateVec(7,1);
vf  = sqrt(vvf'*vvf);
rf  = sqrt(rvf'*rvf);

% if isa(rf,'gradient')
%     rf.x
% else
%     rf
% end

% SMA constraint
orbitEnergy = vf^2/2 - mu/rf;
orbitSMA    = -mu/2/orbitEnergy;
orbitECCv   = ((vf^2-mu/rf)*rvf-(rvf'*vvf)*vvf)/mu;
orbitECC    = sqrt(orbitECCv'*orbitECCv);
%orbitAngMom = cross(rvf,vvf);
%orbitAngMomMag = sqrt(orbitAngMom'*orbitAngMom);
%orbitINC    = atan(orbitAngMom(3,1)/orbitAngMomMag);
timeError    = obj.finalTime - obj.initialTime;
eventFunctions = [orbitSMA;orbitECC;timeError];%;orbitINC ];
%eventFunctions = [rf;orbitECC;timeError];
%obj.eventFunctions = eventFunctions;
obj.costFunction          = -mf*0;%*1000 - 1000;
%orbitSMA
%orbitECC

% if isa(orbitSMA,'gradient')
%     orbitSMA = orbitSMA.x;
%     orbitECC = orbitECC.x;
%     timeError = timeError.x;
% end
% figHandle = figure(9876);
% set(figHandle,'Position',[464 294 300 100])
% data = [orbitSMA orbitECC timeError];
% colNames = {'SMA', 'ECC', 'Time Error'};
% % uitable(figHandle, 'Data', data, 'ColumnName', colNames,...
%     'Position', [20 20 300 100]);
