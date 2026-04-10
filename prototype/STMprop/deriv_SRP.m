function [xdot,Amat] = deriv_SRP(t,x,ForceModel)

global SolarSystem jd_ref

%  Determine if the STM is being propagated
if size(x,1) == 6
    PropSTM = 0;
elseif size(x,1) == 42
    PropSTM = 1;
end

%  Useful terms for later
rv     = x(1:3,1);
eye3   = eye(3);
zero3  = zeros(3);
[DeltaAT] = MJD2TimeCoeff(jd_ref);
DeltaTT   = DeltaAT + 32.184;
jd_tt     = jd_ref + (DeltaTT + t)/86400;;
rvsun     = pleph(jd_tt,11,ForceModel.CentralBodyIndex,1);
sv        = rvsun - rv;
s         = norm(sv);
svhat     = sv/s;
s3        = s^3;
s5        = s^5;

%  Calculate the acceleration
P  = ForceModel.SolarRadiationPressure.Flux/3e8*...
    (norm(sv)/ForceModel.SolarRadiationPressure.Nominal_Sun)^2;
Cr = 1.7;
As = 1*1e-3;
m = 1000;
xdot(1:3,1) = [0 0 0]';
xdot(4:6,1) = -P*Cr*As/m*svhat;

%  Calculate the STM Terms
Amat = zeros(6,6);
if PropSTM
    Amat(4:6,1:3)  = P*Cr*As/m*(1/s3*eye3 - 3*sv*sv'/s5);
end


