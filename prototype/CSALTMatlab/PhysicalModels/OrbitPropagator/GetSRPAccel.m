function [accSRP,  dadr, dadv] = GetSRPAccel(t,X,Force,Sat)

persistent initFlag

showDebug = 0;

%%  Open the DE file if it is not already open
% if isempty(initFlag)
%      initFlag = init_eph('de405.dat');
% end

%% Extract data from state
rvSat = X(1:3,1);
vvSat = X(4:6,1);
rSat  = norm(rvSat);
vSat  = norm(vvSat);

%%  Get position of sun w/r/t earth.
a1Epoch             = Force.RefEpoch + t/86400 + 2430000;
utcEpoch            = a1Epoch - 0.0343817/86400 - Force.deltaAT/86400;
[T_TT,JD_UT1,JD_TT] = ConvertJD_UTC(utcEpoch,Force.deltaAT,Force.deltaUT1);
ttt = (JD_TT - 2451545) / 36525;
mE = mod(357.5277233 + 35999.05034 * ttt, 360);
jdTDB = JD_TT + (0.001658 * sind(mE) + 0.00001385 * sind(2*mE)) / (86400);
[rvSun]             = pleph(jdTDB,11,3,1);
rSun                = norm(rvSun);

%%  Calculate the shadow function
rvSatToSun = rvSun - rvSat;
percentSun = GetPercentSun(rvSat,rvSun);

%%  Calculate the acceleration
if Force.SRPModel == 1
    fluxPressure = Force.flux/Force.speedOfLight;
    accSRP       = -percentSun*fluxPressure*Sat.Cr*Sat.SRPArea/1e3/Sat.DryMass...
                    *rvSatToSun/norm(rvSatToSun)^3*Force.AU^2;
elseif Force.SRPModel == 2
    accSRP = ComputeSPADAccel(t,X,Force,Sat);
end

dadv = zeros(3,3);
dadr = zeros(3,3);
if Force.STM
    if Force.SRPModel == 1
        dadr = -percentSun*fluxPressure*Sat.Cr*Sat.SRPArea/1e6/Sat.DryMass*...
            (1/norm(rvSatToSun)^3*eye(3) - 3*rvSatToSun*rvSatToSun'/norm(rvSatToSun)^5);
        dVarEq = zeros(3,3);
    elseif Force.SRPModel == 2
       %dadr = numjac('ComputeSPADAccel',t,X(1:3,1),accSRP,abs(accSRP) *1e-8,[],0,[],[],Force,Sat);
        %                             (F,T,Y,FTY,THRESH,FAC,VECTORIZED) 
        dx        = rSat*1e-4;
        dy        = rSat*1e-4;
        dz        = rSat*1e-4;
       % disp('New Call===============================')
        accSRPdx  = ComputeSPADAccel(t,X(1:6)  + [dx 0 0 0 0 0]',Force,Sat);
       %  disp('New Call===============================')
        accSRPdy  = ComputeSPADAccel(t,X(1:6)  + [0 dy 0 0 0 0]',Force,Sat);
       %  disp('New Call===============================')
        accSRPdz  = ComputeSPADAccel(t,X(1:6)  + [0 0 dz 0 0 0]',Force,Sat);
        % disp('New Call===============================')
        dadr(:,1) = (accSRPdx - accSRP)/dx;
        dadr(:,2) = (accSRPdy - accSRP)/dy;
        dadr(:,3) = (accSRPdz - accSRP)/dz;
        dadr = dadr';
        if showDebug
            disp(accSRP)
            disp(accSRPdx)
            disp(accSRPdy)
            disp(accSRPdz)
            disp(dadr)
        end
    end
else
    cVarEq = [];
    dVarEq = [];
end






