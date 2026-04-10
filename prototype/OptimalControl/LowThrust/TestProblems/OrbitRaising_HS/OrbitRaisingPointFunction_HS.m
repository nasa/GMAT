function [obj] = OrbitRaisingPointFunction(obj)


%%  Define constants
mu   = 1;

%%  Extract state and control components
r                  = obj.finalStateVec(1,1);
theta              = obj.finalStateVec(2,1);
dtheta_dt          = obj.finalStateVec(4,1);
obj.eventFunctions = [sqrt(mu/r) - dtheta_dt];
obj.costFunction   = -r; 

%% 
obj.intFunctions = obj.intConstraintVec;
