function [obj] = OrbitRaisingPathFunction(obj)

obj.costFunction = 0.5*(obj.stateVec*obj.stateVec+ ...
    obj.controlVec*obj.controlVec);
obj.dynFunctions  = -obj.stateVec^3 + obj.controlVec; 

