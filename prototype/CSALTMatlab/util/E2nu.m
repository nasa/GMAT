function nu = E2nu(E,e)

%  function nu = E2nu(E,e)
%
%  calculates the true anomaly given eccentric anomaly and eccentricity
%  -uses atan2 to insure the proper quadrant
%
%  Variable Name    I/0    Units      Dimens.    Description
%---------------------------------------------------------------------------------------
%  E                 I      rad        1x1       Eccentric Anomaly
%  ecc               I      dimless.   1x1       eccentricity
%  nu                O      rad        1x1       true anomaly      
%  Modification History
%  06/26/00 - Created, S. Hughes
%  07/17/00 - Modified so nu is always > 0

cos_E = cos(E);
sin_nu = sqrt(1 - e*e)*sin(E)/(1-e*cos_E);       %Vallado pg. 214,  Eq. 4-10
cos_nu = (cos_E - e)/(1 - e*cos_E);              %Vallado pg. 214,  Eq. 4-12
nu = atan2(sin_nu,cos_nu);

if (nu < 0)
   nu = nu + 2*pi;
end

