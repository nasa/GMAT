
%%  This is a simple test driver to compare MOE propagation with 
%   propagation using Cartesian elements.
gravParam = 398600.4415;
cart = [7100 0.0 1300 0.0 7.35 1.0];

dynFunc = ModEqDynamics(gravParam);

meeElems = Cart2Mee(cart,gravParam);

pertForces = [0 0 0]';

derivs = dynFunc.ComputeOrbitDerivatives(meeElems,pertForces);

[odeRHS] = MOEDynWrapper(0,meeElems);

odeOpts = odeset('AbsTol',1e-11,'RelTol',1e-11);
[t2,Xcart] = ode113('TwoBodyForce',[0 12000],cart,odeOpts);
[t,Xmee] = ode113('MOEDynWrapper',[0 12000],meeElems,odeOpts);


lastMee = Xmee(end,:)';

lastCart = Mee2Cart(lastMee,gravParam);
   
posError = Xcart(end,1:3)' - lastCart(1:3,1);
velError = Xcart(end,4:6)' - lastCart(4:6,1);

if max(posError > 1e-6) || max(velError) > 1e-9
    error('error in MOE propagation')
end




