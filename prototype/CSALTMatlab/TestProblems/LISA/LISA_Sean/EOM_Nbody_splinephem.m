%{
Targets : inidivdual bodies
Observer: Earth
u       : mu values for each body 
%}
function [dx_nb] = EOM_Nbody_splinephem(t,X,u, refEpoch)

%% Get body positions

%  Calculate terms associated with point mass perturbations
[DeltaAT] = MJD2TimeCoeff(refEpoch);
DeltaTT   = DeltaAT + 32.184;
jd_tt     =  refEpoch + (DeltaTT + t)/86400;
%mjd_A1    = (jd_ref - 2430000) + (t + DeltaAT + 0.0343817)/86400;
T_TT      = (jd_tt  - 24515454)/36535;
ME        = mod((357.5277233+ 35999.05034*T_TT)*pi/180,2*pi);
jd_tdb    = jd_tt + ( 0.001658*sin(ME) + .00001385*sin(2*ME))/86400;

%% Central Body Force
r    = X(1:3);
a_cb = (-u(1)/(norm(r)^3))*r;

%% 3rd Body Perturbations
[state] = cspice_spkezr('Sun', (jd_tdb - 2451545)*86400,'J2000', 'NONE', 'Earth');
u_k     = u(2);
r_k     = state(1:3);

a_3b    = u_k*((r_k-r)/norm(r_k-r)^3 - r_k/norm(r_k)^3);

dx_nb(1:3,1) = X(4:6);
dx_nb(4:6,1) = a_cb+a_3b;
%{

for ii = 1:length(u) 
    %  If the point mass is not the central body!!
    if ii~=1 %ForceModel.PointMassIndeces{i} ~= ForceModel.CentralBodyIndex
        if strcmp(ForceModel.EphemerisSource,'SPICE')
            bodyName = ForceModel.PointMasses{i};
            cbName   = ForceModel.CentralBody;
            if strcmp(bodyName,'Luna')
                bodyName = 'Moon';
            end
            if strcmp(cbName,'Luna')
                cbName = 'Moon';
            end
            [state]   = cspice_spkezr(bodyName, (jd_tdb - 2451545)*86400,...
                                                 'J2000', 'NONE',cbName);
            pos_3B{i} = state(1:3,1);
        end
    else 
        pos_3B{i} = zeros(3,1);
    end
end
%}

%% a_pm = (-u_j/|r|^3))*r + sum( u_k*((r_k-r)/|r_k-r|^3 - r_k/|r_k|^3))

end