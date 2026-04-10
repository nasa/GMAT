function [StateRate] = TankSystemRHS(t,X,What,Tank)

%  Evaluate the mass flow rate.  This is a kludge until the thruster models
%  are coded!!
Tank = BlowDownTank_SetState(Tank,X);
Thrust = polyval(Tank.TPoly,Tank.Pt);
Isp    = polyval(Tank.IspPoly,Tank.Pt);
mdote = Thrust/Isp;

%  Get the state derivative for the tank.
[StateRate,Tank] = BlowDownTank_GetDerivative(Tank,X,mdote);


