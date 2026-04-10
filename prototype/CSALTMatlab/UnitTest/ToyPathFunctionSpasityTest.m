function [obj] = ToyPathFunctionSpasityTest(obj)

global useAnalyticJac

y1 = obj.stateVec(1);
y2 = obj.stateVec(2);
y3 = obj.stateVec(3);
y4 = obj.stateVec(4);
y5 = obj.stateVec(5);
y6 = obj.stateVec(6);
u1 = obj.controlVec(1);
u2 = obj.controlVec(2);
t  = obj.time;

%%  Dynamics functions and partials
obj.dynFunctions  = [y1*y2*u1;
    y2*y3;
    y3*y4*t;
    y4*y5*u2;
    y5*y6
    y1*y6+t^2];

%%  Algegraic functions and partials
obj.algFunctions = [y1*y2*u1*u2*t;
    y1^2*y2^2*u1^2*u2^2*t^2;
    y2*y4];

%%  Algegraic functions and partials
obj.costFunction = sin(y1)*sin(y2)*sin(u1)*sin(y6)*sin(t);

