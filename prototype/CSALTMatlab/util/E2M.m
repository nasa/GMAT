function M = E2M(E,e)

%  function M = E2M(E,e)
%
%  Solves for M given E and e using Kepler's Equation
%
%  Variable Name    I/0    Units      Dimens.    Description
%---------------------------------------------------------------------------------------
%  M                 )      rad        1x1       Mean anomaly at epoch        
%  e                 I      dimless.   1x1       eccentricity
%  E                 I      rad        1x1       Eccentric Anomaly at epoch
%  Modification History
%  02/16/01 - Created, S. Hughes

M = E - e* sin(E);