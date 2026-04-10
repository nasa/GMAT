function [obj ] = ToyPointFunctionSparsityTest(obj )

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

                       % ti   tf   y1i   y2i   y3i   y1f   y2f   y3f 
obj.boundaryFunctions = [ti - tf       + y2i       *       y2f       ;
                            - tf + y1i + y2i + y3i * y1f + y2f - y3f;
                         ti   *    y1i + y2i   +           y2f       ];
                     
                  % t1   tf   y1i   y2i   y3i   y1f   y2f   y3f 
obj.costFunction = [ti + tf + y1i +                       + y3f];
