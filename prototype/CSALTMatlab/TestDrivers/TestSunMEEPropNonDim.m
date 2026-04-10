clear all
global config pertForceDim gravParam

gravParam = 132712440017.99

config.gravParam = gravParam;
config.DU  = 149597871/1000;
config.AU = 149597871;%2*2*pi;
config.TU  = sqrt(config.DU^3/config.gravParam);
config.ACCU = config.TU^2/config.DU;
config.MU  = 100;%100000;
config.VU = config.DU/config.TU;
config.TimeOffSet = 29366.400011573;
config.StateScaleMat = diag([1/config.DU; ones(5,1); 1/config.MU]);
config.StateAccScaleMat = config.TU*config.StateScaleMat
config.StateUnScaleMat = inv(config.StateScaleMat);
config.StateAccUnScaleMat = inv(config.StateAccScaleMat);

%%  This is a simple test driver to compare MOE propagation with 
%   propagation using Cartesian elements.
cart = [ -53148434.8103882
         -142051331.860741
          7265.76148179626
          30.8670612779478
         -12.3094757771286
          2.19483473365826];

pertForceDim = [1 1 1]'/1e3;
pertForceDim = [-8.59349583375601e-05
      0.000598890610195828
      0.000167605187100244];

meeElems = Cart2Mee(cart,config.gravParam);

meeElemsNonDim = config.StateScaleMat(1:6,1:6)*meeElems;

cart = Mee2Cart(config.StateUnScaleMat(1:6,1:6)*meeElemsNonDim,config.gravParam);

MOEDynWrapper(0,meeElemsNonDim)


odeOpts = odeset('AbsTol',1e-12,'RelTol',1e-12);
propDuration = 10*86400;
[t2,Xcart] = ode113('TwoBodyForce',[0 propDuration],cart,odeOpts);
[t,Xmee] = ode113('MOEDynWrapper',[0 propDuration/config.TU],meeElemsNonDim,odeOpts);
size(Xcart)
size(Xmee)
lastMee = config.StateUnScaleMat(1:6,1:6)*Xmee(end,:)';
lastCart = Mee2Cart(lastMee,config.gravParam);
   
posError = Xcart(end,1:3)' - lastCart(1:3,1)
velError = Xcart(end,4:6)' - lastCart(4:6,1)

return
if max(abs(posError) > 1e-3) || max(abs(velError)) > 1e-9
    error('error in MOE propagation')
end




