function [t, PosVel, Phi] = Propagate(Spacecraft,ForceModel,TOF)

global SolarSystem jd_ref

%  Initialize the solar system
InitSolarSystem;

%  Initialize the force model
ForceModel = InitForceModel(ForceModel);

%  Define the Initial State
X0v  = [Spacecraft.X Spacecraft.Y Spacecraft.Z...
    Spacecraft.VX Spacecraft.VY Spacecraft.VZ ]';
if ForceModel.PropSTM
    X0   = [X0v; sqrmat2vec(eye(6))];
else
    X0 = X0v;
end

%  Calculate the reference epoch in UTC Julian Date
datevec = datestr2vec(Spacecraft.Epoch);
jd_ref  = greg2jd(datevec);


%  Perform the Propagation
[t,X] = ode78_FM('dynfunc', 0, TOF, X0, 1e-10 , [], [], ForceModel);
%opt = odeset('AbsTol',1e-13,'RelTol',1e-13);
%[t,X] = ode113('dynfunc',[0:864:TOF], X0, opt);
%  Break out portions of the solution
for i = 1:size(t,1)
    if ForceModel.PropSTM
        Phi(1:6,1:6,i) = vec2sqrmat(X(i,7:42)');
    else
        Phi = [];
    end
    PosVel(i,:)     = X(i,1:6);
end




