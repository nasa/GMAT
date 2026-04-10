function [accRelativity] = GetRelativityAccel(t,X,Force,Sat)

persistent initFlag
rvSat = X(1:3,1);
vvSat = X(4:6,1);
rSat  = norm(rvSat);
vSat  = norm(vvSat);


if isempty(initFlag)
     initFlag = init_eph('de405.dat');
end

Beta  = 1;
Gamma = 1;
L     = 1;


%  Compute Earth Angular Momentum
a1Epoch = Sat.initialEpoch + t/86400 + 2430000;
utcEpoch = a1Epoch - 0.0343817/86400 - Force.deltaAT/86400;
[T_TT,JD_UT1,JD_TT] = ConvertJD_UTC(utcEpoch,Force.deltaAT,Force.deltaUT1);


%  Hard coded stuff for Earth
magJ  = 11.8e2; % km^2/s;
[R,Rdot,omega] = FK5( utcEpoch, Force.deltaAT, Force.deltaUT1,Force.lod,Force.xp,Force.yp,Force.dPsi,Force.dEps,2);
bodyAngMom     = omega/norm(omega)*magJ;
%bodyAngMom     = [0 0 1]'*magJ;
[rvSunToCb,vvSunToCb] = pleph(JD_TT,11,3,1);

%  Hard coded stuff for jupiter
% magJ  = 11.8e2; % km^2/s;
% [R,Rdot,omega] = FK5( utcEpoch, Force.deltaAT, Force.deltaUT1,Force.lod,Force.xp,Force.yp,2);
% bodyAngMom     = omega/norm(omega)*magJ;
% bodyAngMom     = [0 0 1]'*magJ;
% [rvSunToCb,vvSunToCb] = pleph(JD_TT,11,3,1);
% R_IF = [ -0.163994656845715        -0.986352326688516       -0.0146574268617593;
%          0.891420074377636        -0.141814764578153        -0.430417034449396;
%          0.422464203835811       -0.0836520184083732         0.902511128071881];
% omega      = [0 0 -0.000175853364145314];
% omegaMag   = norm(omega);
% Rb = 71492;
% bodyAngMom = R_IF*[0 0 2/5*Rb^2*omegaMag]';

% Old way that works!
% magJ = 359518.908757473;
% bodyAngMom = [-0.01465737819671631 -0.4304170310497284 0.9025111347436905]'*magJ;

[rvSunToCb,vvSunToCb] = pleph(JD_TT,11,5,1);

%  Hard coded stuff for sun
% magJ = 503238.761979423;
% bodyAngMom = [0.1223534941673279 -0.4230720847845078 0.8977971002459526]'*magJ;
% [rvSunToCb,vvSunToCb] = pleph(JD_TT,11,11,1);

%%  Huang model
schwarzTerm = Force.bodyMu/Force.speedOfLight^2/rSat^3*((2*(Beta+Gamma) ...
         * Force.bodyMu/rSat - Gamma*vSat*vSat)*rvSat + (2 + 2*Gamma)*(rvSat'*vvSat)*vvSat) ;

     if norm(rvSunToCb) < 1
         precessTerm = zeros(3,1);
     else
         rhsTerm = -Force.muSun*rvSunToCb/Force.speedOfLight^2/norm(rvSunToCb)^3;
         omegaCB = 1.5*cross(vvSunToCb,rhsTerm);
         precessTerm = 2*cross(omegaCB,vvSat);
     end

lenseThirringTerm = L*(1+Gamma)*Force.bodyMu/Force.speedOfLight^2/rSat^3*...
    (3/rSat^2*(cross(rvSat,vvSat)*dot(rvSat,bodyAngMom)) + cross(vvSat,bodyAngMom));

fac1  =  Force.muSun/Force.speedOfLight^2/norm(rvSunToCb)^3;
term1 = fac1*(   (4*Force.muSun/norm(rvSunToCb) - norm(vvSunToCb)^2)*(-rvSunToCb) +...
         4*((-rvSunToCb')*(-vvSunToCb))*(vvSunToCb));
     
accRelativity = schwarzTerm  + precessTerm + lenseThirringTerm;

return

%% This is the GEODYNE model from 1976 math spec.
fac1  =  Force.muSun/Force.speedOfLight^2/norm(rvSunToCb)^3;
term1 = fac1*(   (4*Force.muSun/norm(rvSunToCb) - norm(vvSunToCb)^2)*(-rvSunToCb) +...
         4*((-rvSunToCb')*(-vvSunToCb))*(vvSunToCb));
     
     
fac2  = Force.bodyMu/Force.speedOfLight^2/norm(rvSat)^3;
term2 = fac2*(   (4*Force.bodyMu/norm(rvSat) - norm(vvSat)^2)*rvSat +...
         4*(rvSat'*vvSat)*vvSat);

accRelativity =  -(term1 - term2);







