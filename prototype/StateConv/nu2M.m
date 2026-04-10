function M = nu2M(nu,e)

%  function M = nu2M(nu,e)
%
%  Solves for M given nu and e using Kepler's Equation
%
%  Variable Name    I/0    Units      Dimens.    Description
%---------------------------------------------------------------------------------------
%  M                 O      rad        1x1       Mean anomaly at epoch        
%  e                 I      dimless.   1x1       eccentricity
%  nu                I      rad        1x1      True Anomaly at epoch
%  Modification History
%  04/9/01 - Created, S. Hughes


E = nu2E(nu,e);
M = E2M(E,e);