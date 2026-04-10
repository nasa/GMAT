function [obj] = ToyPathFunction(obj)

global useAnalyticJac

y1 = obj.stateVec(1);
y2 = obj.stateVec(2);
u1 = obj.controlVec(1);
u2 = obj.controlVec(2);
t  = obj.time;

%%  Dynamics functions and partials
obj.dynFunctions  = [y1*y2^2 + u1*u2*t;
    y1^2*y2 + t^2*u1^4*u2^3];

if useAnalyticJac
    obj.dynFuncStateJac = [y2^2 2*y1*y2;
        2*y1*y2 y1^2];
    
    obj.dynFuncControlJac = [u2*t u1*t;
        4*t^2*u1^3*u2^3 3*t^2*u1^4*u2^2];
    
    obj.dynFuncTimeJac = [u1*u2;
        2*t*u1^4*u2^3];
end

%%  Algegraic functions and partials
obj.algFunctions = [y1*y2*u1*u2*t;
    y1^2*y2^2*u1^2*u2^2*t^2];

if useAnalyticJac
    obj.algFuncStateJac = [y2*u1*u2*t y1*u1*u2*t;
        2*y1*y2^2*u1^2*u2^2*t^2 2*y1^2*y2*u1^2*u2^2*t^2];
    
    obj.algFuncControlJac = [y1*y2*u2*t y1*y2*u1*t;
        2*y1^2*y2^2*u1*u2^2*t^2 2*y1^2*y2^2*u1^2*u2*t^2];
    
    obj.algFuncTimeJac = [y1*y2*u1*u2;
        2*y1^2*y2^2*u1^2*u2^2*t];
end
%%  Algegraic functions and partials
obj.costFunction = sin(y1)*sin(y2)*sin(u1)*sin(u2)*sin(t);

if useAnalyticJac
    obj.costFuncStateJac = [cos(y1)*sin(y2)*sin(u1)*sin(u2)*sin(t), ...
        sin(y1)*cos(y2)*sin(u1)*sin(u2)*sin(t)];
    
    obj.costFuncControlJac = [sin(y1)*sin(y2)*cos(u1)*sin(u2)*sin(t),...
        sin(y1)*sin(y2)*sin(u1)*cos(u2)*sin(t)];
    
    obj.costFuncTimeJac = sin(y1)*sin(y2)*sin(u1)*sin(u2)*cos(t);
end



