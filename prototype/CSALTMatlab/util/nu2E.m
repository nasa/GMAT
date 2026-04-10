function E = nu2E(nu,e)

%  function E = nu2E(nu,e)
%
%  calculates the eccentric anomaly given true anomaly and eccentricity
%  -uses atan2 to insure the proper quadrant
%
%  Variable Name    I/0    Units      Dimens.    Description
%---------------------------------------------------------------------------------------
%  nu                I      rad        1x1       true anomaly         
%  ecc               I      dimless.   1x1       eccentricity
%  E                 O      rad        1x1       Eccentric Anomaly
%  Modification History
%  04/03/00 - Created, S. Hughes
%  05/08/00 - S.Hughes,  fixed so that the value for E is always > 0

cosnu = cos(nu);
sin_E = sqrt(1 - e*e)*sin(nu)/(1+e*cosnu);       %Vallado pg. 213,  Eq. 4-9
cos_E = (e+cosnu)/(1+e*cosnu);
E = atan2(sin_E,cos_E);

%  make sure E > 0;
while (E < 0)
   E = E + 2*pi;
end
