function [obj] = BrachistichronePathFunction(obj)

gravity = -10;
x = obj.stateVec(1);
y = obj.stateVec(2);
v = obj.stateVec(3);
u = obj.controlVec(1);

obj.dynFunctions  = [v*sin(u); v*cos(u); gravity*cos(u)];