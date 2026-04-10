function TestFiniteDiff

global control
control = [ 1.98470886329107
         -2.48142286289214];


stateVec = [1.00638653852165
         0.454397245332161
        0.0505566006422867
          1.02233811839586
                  0.964476];
fTY = FunInterface(0,stateVec);
thresh = ones(length(stateVec),1)*1e-10;

[Jac] = numjac(@FunInterface,0,stateVec,fTY,thresh,[],0)

function f = FunInterface(t,stateVec)

global control

obj.controlVec = control;
obj.stateVec = stateVec;
userFunction = 'OrbitRaisingPathFunction';

[obj] = feval(userFunction,obj)
f = obj.dynFunctions;
