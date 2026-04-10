clear all
config.gravParam = 132712440017.99;
config.DU  = 149597871;
config.AU = 2*pi;
config.TU  = sqrt(config.DU^3/config.gravParam);
config.MU  = 100000;
config.VU = config.DU/config.TU;
config.TimeOffSet = 29366.400011573;
config.StateScaleMat = diag([1/config.DU; ones(4,1); 1/config.AU; 1/config.MU]);
config.StateAccScaleMat = diag([
    ones(1,1)*(config.TU/config.DU);
    ones(4,1);
    config.TU/config.AU;
    (config.TU/config.MU)]);
config.StateUnScaleMat = inv(config.StateScaleMat);
config.StateAccUnScaleMat = inv(config.StateAccScaleMat);

%%  This is a simple test driver to compare MOE propagation with 
%   propagation using Cartesian elements.
cart = [-5.31484348103882E+10 -1.30332439062922E+11 -5.64981086004718E+10 ...
         3.08670612779478E+04 -1.21667783491300E+04 -2.88270677432904E+03]/1000;

dynFunc = ModEqDynamics(config.gravParam);

meeElems = Cart2Mee(cart,config.gravParam);

pertForces = [0 0 0]';

derivs = dynFunc.ComputeOrbitDerivatives(meeElems,pertForces);

[odeRHS] = MOEDynWrapper(0,meeElems);

odeOpts = odeset('AbsTol',1e-11,'RelTol',1e-11);
[t2,Xcart] = ode113('TwoBodyForce',[0 12000],cart,odeOpts);
[t,Xmee] = ode113('MOEDynWrapper',[0 12000],meeElems,odeOpts);

lastMee = Xmee(end,:)';
lastCart = Mee2Cart(lastMee,config.gravParam);
   
posError = Xcart(end,1:3)' - lastCart(1:3,1);
velError = Xcart(end,4:6)' - lastCart(4:6,1);

if max(posError > 1e-6) || max(velError) > 1e-9
    error('error in MOE propagation')
end




