function [xdot,Amat] = deriv_pointmass(t,x,ForceModel);

global SolarSystem jd_ref jdUpdateEpoch
persistent updateIntervalET

if ~exist('updateIntervalET')
    updateIntervalET = t;
end

%  Determine if the STM is being propagated
if size(x,1) == 6
    PropSTM = 0;
elseif size(x,1) == 42
    PropSTM = 1;
end

%  Calculate useful terms for later
rv     = x(1:3,1);
vv     = x(4:6,1);
xdot = zeros(6,1);
r      = sqrt(rv'*rv);
r3     = r^3;
r5     = r^5;
eye3   = eye(3);
zero3  = zeros(3);
muCB   = SolarSystem.Mu(ForceModel.CentralBodyIndex);
mubyr3 = muCB/r3;

%  Calulate terms associated with central body
if ForceModel.IncludeCentralBody
    xdot(1:3,1) = vv;
    xdot(4:6,1) = -mubyr3*rv;
    if ForceModel.PropSTM
        A  = zero3;
        B  = eye3;
        C  = -mubyr3*eye3 + 3*muCB*rv*rv'/r5;
        D  = zero3;
    else
        A  = zero3;
        B  = zero3;
        C  = zero3;
        D  = zero3;
    end
end

%  Calulate terms associated with point mass perturbations
xdot_PM = zeros(3,1);
C_PM = zeros(3,3);
[DeltaAT] = MJD2TimeCoeff(jd_ref);
DeltaTT = DeltaAT + 32.184;
jd_tt = jd_ref + (DeltaTT + t)/86400;
mjd_A1 = (jd_ref -2430000) + (t + DeltaAT + 0.0343817)/86400;
T_TT = (jd_tt - 24515454)/36535;
ME = mod((357.5277233+ 35999.05034*T_TT)*pi/180,2*pi);
jd_tdb = jd_tt + ( 0.001658*sin(ME)  + .00001385*sin(2*ME))/86400;
jd_tdb = jd_ref + t/86400;
% dt = t - updateIntervalET;
% if dt == 0;
%     jdUpdateEpoch = jd_tdb;
% end
% if dt <86400
%     jd_tdb = jdUpdateEpoch;
% else
%     updateIntervalET = t;
%     jdUpdateEpoch       = jd_tdb;
% end


for i = 1:size(ForceModel.PointMassIndeces,2)
    
    %  If the point mass is not the central body!!
    if ForceModel.PointMassIndeces{i} ~= ForceModel.CentralBodyIndex
        
        muk     = SolarSystem.Mu(ForceModel.PointMassIndeces{i});
        if strcmp(ForceModel.EphemerisSource,'DE405')
            rvk     = pleph(jd_tdb,ForceModel.PointMassIndeces{i},ForceModel.CentralBodyIndex,1);
        elseif strcmp(ForceModel.EphemerisSource,'TwoBody')
            [rvk]   = TwoBodyState(mjd_A1, ForceModel.PointMassIndeces{i},ForceModel.CentralBodyIndex);
        elseif strcmp(ForceModel.EphemerisSource,'SPICE')
            bodyName = ForceModel.PointMasses{i};
            cbName   = ForceModel.CentralBody;
            if strcmp(bodyName,'Luna')
                bodyName = 'Moon';
            end
            if strcmp(cbName,'Luna')
                cbName = 'Moon';
            end
            try
                [state] = cspice_spkezr(bodyName  , (jd_tdb - 2451545)*86400, 'J2000', 'NONE',cbName);
            catch
                % TODO: this should not be necessary.
                addpath(genpath('C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\prototype\events\bin\contrib\mice'))
                addpath('C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\test\script\extern\utils\KSC_DEReader\ephm')
                init_eph('de421.dat');
                % if stcmp(ForceModel.EphemerisSource,'SPICE')
                cspice_furnsh( 'C:\Users\sphughe1\Documents\GMATRepos\ScriptTest\gmatdata\planetary_ephem\spk\de421.bsp' )
                [state] = cspice_spkezr(bodyName  , (jd_tdb - 2451545)*86400, 'J2000', 'NONE',cbName);
            end
            rvk = state(1:3,1);
        end
        
        rvkmr   = rvk - rv;
        rvk3    = norm(rvk)^3;
        rvkmr3  = norm(rvkmr)^3;
        rvkmr5  = norm(rvkmr)^5;
        xdot_PM = xdot_PM + muk*(rvkmr/rvkmr3 - rvk/rvk3);
        if ForceModel.PropSTM
            C_PM    = C_PM + muk*( - 1/rvkmr3*eye3 + 3*rvkmr*rvkmr'/rvkmr5);
        end
        
    end
    
end

%  Now add in point mass perturbations to acceleration and Amat
xdot(4:6,1) =  xdot(4:6,1) + xdot_PM;
if ForceModel.PropSTM
    Amat = [A B; (C + C_PM) D];
else
    Amat = zeros(6,6);
end



