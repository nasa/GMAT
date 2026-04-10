function [obj ] = ToyPointFunction(obj )

global useAnalyticJac

%  Extract variables from input
ti  = obj.initialTime;
tf  = obj.finalTime;
y1i = obj.initialStateVec(1);
y2i = obj.initialStateVec(2);
y3i = obj.initialStateVec(3);
y1f = obj.finalStateVec(1);
y2f = obj.finalStateVec(2);
y3f = obj.finalStateVec(3);

obj.boundaryFunctions = [y1i^2 + y2i^3 + y3i^4 + ti*tf + cos(y1f*y2f*y3f);
    y1i*y2i*y3i*ti + ti*tf + cos(y1f*y2f*y3f);
    y1f^2 + y2f^3 + y3f^4 + ti*tf + cos(y1i*y2i*y3i) ;
    y1f*y2f*y3f*tf + ti*tf + cos(y1i*y2i*y3i)];

obj.costFunction = (tf - ti)*(y1i + y2i + y3i) + (tf - ti)*(y1f + y2f + y3f);

if useAnalyticJac
    obj.boundFuncInitStateJac = [2*y1i 3*y2i^2  4*y3i^3;
        y2i*y3i*ti y1i*y3i*ti y1i*y2i*ti
        -sin(y1i*y2i*y3i)*y2i*y3i -sin(y1i*y2i*y3i)*y1i*y3i  -sin(y1i*y2i*y3i)*y2i*y1i;
        -sin(y1i*y2i*y3i)*y2i*y3i -sin(y1i*y2i*y3i)*y1i*y3i  -sin(y1i*y2i*y3i)*y2i*y1i];
    obj.boundFuncFinalStateJac = [
        -sin(y1f*y2f*y3f)*y2f*y3f -sin(y1f*y2f*y3f)*y1f*y3f  -sin(y1f*y2f*y3f)*y2f*y1f;
        -sin(y1f*y2f*y3f)*y2f*y3f -sin(y1f*y2f*y3f)*y1f*y3f  -sin(y1f*y2f*y3f)*y2f*y1f;
        2*y1f 3*y2f^2 4*y3f^3;
        y2f*y3f*tf y1f*y3f*tf y1f*y2f*tf];
    obj.boundFuncInitTimeJac = [tf;
        y1i*y2i*y3i + tf;
        tf;
        tf];
    obj.boundFuncFinalTimeJac = [
        ti;
        ti;
        ti;
        y1f*y2f*y3f + ti];
end


end

