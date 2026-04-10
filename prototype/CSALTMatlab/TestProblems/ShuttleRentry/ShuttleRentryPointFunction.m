function [obj] = ShuttleRentryPointFunction(obj)

 
h0 = 260000.0 ;
v0 = 25600.0 ;
phi0 = pi/180*(-0.5*75.3153) ;
gamma0 = pi/180*(-1.0);
theta0 = 0.0 ;
psi0 = pi/180*(90.0) ;
hf = 80000.0 ;
vf = 2500.0 ;
gammaf = pi/180*(-5.0);

finalState = obj.finalStateVec;
hFinal = finalState(1);
vFinal = finalState(4);
gammaFinal = finalState(5);

 obj.boundaryFunctions = [obj.initialStateVec;hFinal;vFinal;gammaFinal];

 obj.conLowerBound = [h0 phi0 theta0 v0 gamma0 psi0 hf vf gammaf ]';
 obj.conUpperBound = [h0 phi0 theta0 v0 gamma0 psi0 hf vf gammaf]';
 
 obj.SetAlgFunctions(
