function J = dCartdOE(state,type)

%  Define constants
mu = 398600.4415;

%  Convert to elements if necessary
if type == 1;
    oe = stateconv(state,1,2);
else 
    oe = state;
end

%  Break up the elements
a = oe(1); e = oe(2); i = oe(3); om = oe(4); Om = oe(5); nu = oe(6);

%----------------------------------------
%  Calculate useful quantities for later
%----------------------------------------
cosnu = cos(nu);
sinnu = sin(nu);
p = a*(1 - e*e);
eta = sqrt( 1 - e*e );
r = p/(1 + e*cosnu);
rp_hat = [cosnu sinnu 0]';
rp = r*rp_hat;
vp = [    -(mu/a/(1-e^2))^(1/2)*sin(nu)
 (mu/a/(1-e^2))^(1/2)*(e+cos(nu))
                                0];
Rt = R3(-Om)*R1(-i)*R3(-om);

%----------------------------------------
%  Calculate derivatives of position
%----------------------------------------
drvda  = Rt*rp_hat*r/a;
drvde  = (-2*r*e/eta^2-a*(1-e^2)/(1+e*cos(nu))^2*cos(nu))*Rt*rp_hat;
drvdi  = -R3(-Om)*dR1(-i)*R3(-om)*rp;
drvdom = -R3(-Om)*R1(-i)*dR3(-om)*rp;
drvdOm = -dR3(-Om)*R1(-i)*R3(-om)*rp;
drvdnu = Rt*[  a*(-1+e^2)*sin(nu)/(1+e*cos(nu))^2
 (1-e^2)*(sin(nu)^2*e+cos(nu)+e*cos(nu)^2)*a/(1+e*cos(nu))^2
                          0];

%----------------------------------------
%  Calculate derivatives of position
%----------------------------------------
dvvda  = Rt*[      1/2/(mu/a/(1-e^2))^(1/2)*sin(nu)*mu/a^2/(1-e^2)
 -1/2/(mu/a/(1-e^2))^(1/2)*(e+cos(nu))*mu/a^2/(1-e^2)
                                                    0];
                                                                             
                                                
dvvde  = Rt*[    -1/(mu/a/(1-e^2))^(1/2)*sin(nu)*mu/a/(1-e^2)^2*e
             1/(mu/a/(1-e^2))^(1/2)*(e+cos(nu))*mu/a/(1-e^2)^2*e+(mu/a/(1-e^2))^(1/2)
                                 0];
                             
dvvdi  = -R3(-Om)*dR1(-i)*R3(-om)*vp;
dvvdom = -R3(-Om)*R1(-i)*dR3(-om)*vp;
dvvdOm = -dR3(-Om)*R1(-i)*R3(-om)*vp;

dvvdnu = Rt* [ -(mu/a/(1-e^2))^(1/2)*cos(nu)
 -(mu/a/(1-e^2))^(1/2)*sin(nu)
                             0];
                      
dnudE = a*sqrt(1 - e^2)/r;
E = nu2E(nu,e);
dEdM  = 1/(1 - e*cos(E));
fac = 1;%dnudE*dEdM;
J(1:3,:) = [drvda drvde drvdi drvdom drvdOm drvdnu*fac ];
J(4:6,:) = [dvvda dvvde dvvdi dvvdom dvvdOm dvvdnu*fac ];




