function [obj] = HohmannTransferPointFunction(obj)


%%  Define constants
mu   = 1;

%%  Extract state and control components
rvf = obj.finalStateVec(1:3,1);
vvf = obj.finalStateVec(4:6,1);
mf  = obj.finalStateVec(7,1);               
vf  = sqrt(vvf'*vvf);
rf  = sqrt(rvf'*rvf);

% SMA constraint
orbitEnergy = vf^2/2 - mu/rf;
orbitSMA    = -mu/2/orbitEnergy;
orbitECCv   = ((vf^2-mu/rf)*rvf-(rvf'*vvf)*vvf)/mu;
orbitECC    = sqrt(orbitECCv'*orbitECCv);
orbitAngMom = cross(rvf,vvf);
orbitAngMomMag = sqrt(orbitAngMom'*orbitAngMom);
orbitINC    = atan(orbitAngMom(3,1)/orbitAngMomMag);
eventFunctions = [orbitSMA;orbitECC];%;orbitINC ];
obj.eventFunctions = eventFunctions;
obj.costFunction          = -mf*1000 - 1000; 



