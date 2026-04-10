function [Xdot] = dynfunc(t,x,ForceModel)


global SolarSystem
%ForceModel = dummy2;

% State Initializations
xdot = zeros(6,1);
xdot_PM   = zeros(6,1);
xdot_SRP  = zeros(6,1);
xdot_Drag = zeros(6,1);
xdot_NSG  = zeros(6,1);

% STM initializations
if ForceModel.PropSTM
    A    = zeros(6,6);
    phi      = vec2sqrmat(x(7:42));
    A_PM   = zeros(6,6);
    A_SRP  = zeros(6,6);
    A_Drag = zeros(6,6);
    A_NSG  = zeros(6,6);
end

%  Call point mass model
[xdot_PM,A_PM] = deriv_pointmass(t,x,ForceModel);
switch ForceModel.SRP

    case 'On'
        [xdot_SRP,A_SRP] = deriv_SRP(t,x,ForceModel);
        
end

xdot = xdot_PM + xdot_SRP;
if ForceModel.PropSTM
    A    = A_PM + A_SRP;
    phidot   = A*phi;
    Xdot  = [xdot;sqrmat2vec(phidot)];
else
    Xdot = xdot;
end

