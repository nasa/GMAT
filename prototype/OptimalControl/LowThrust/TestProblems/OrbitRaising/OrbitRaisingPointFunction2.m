function [obj] = OrbitRaisingPointFunction2(obj)


%%  Define constants
mu   = 1;

%%  Extract state and control components
r                  = obj.finalStateVec(1,1);
theta              = obj.finalStateVec(2,1);
rdot               = obj.finalStateVec(3,1);
dtheta_dt          = obj.finalStateVec(4,1);
m                  = obj.finalStateVec(5,1);
obj.costFunction   = -m; 

%% 
rvf = [r*cos(theta);r*sin(theta)];
vvf = [rdot*cos(theta) - r*sin(theta)*dtheta_dt;...
       rdot*sin(theta) + r*cos(theta)*dtheta_dt];
rf = sqrt(rvf'*rvf);
vf = sqrt(vvf'*vvf);
vf = sqrt(rdot^2 + (rf*dtheta_dt)^2);
orbitEnergy = vf^2/2 - mu/r;
orbitSMA    = -mu/2/orbitEnergy;
orbitECCv   = ((vf^2-mu/rf)*rvf-(rvf'*vvf)*vvf)/mu;
orbitECC    = sqrt(orbitECCv'*orbitECCv);
eventConstraints = [r rdot ]';
obj.eventFunctions = eventConstraints;


