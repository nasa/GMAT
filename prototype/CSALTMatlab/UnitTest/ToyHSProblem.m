function [obj] = ToyHSProblem(obj);

global useAnalyticJac

y = obj.stateVec;
u = obj.controlVec;
obj.costFunction = 0.5*(y^2 + u^2);
obj.dynFunctions  = -y^3 + u;

if useAnalyticJac
       
    obj.costFuncStateJac = y;
    obj.costFuncControlJac = u;
    
    obj.dynFuncStateJac = -3*y^2;
    obj.dynFuncControlJac = 1;
end
