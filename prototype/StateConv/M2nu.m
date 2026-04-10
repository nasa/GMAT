function nu = M2nu(M,e)

%  function nu = M2nu(M,e)
%
%  calculates the true anomaly given the mean anomaly and eccentricity
%  by first solving Kepler's equation for E and then converting E to M
%  -uses atan2 to insure the proper quadrant
%  
%  External Refs:  Kepler, E2nu
%
%  Variable Name    I/0    Units      Dimens.    Description
%---------------------------------------------------------------------------------------
%  M                 I      rad        1x1       mean anomaly
%  e                 I      dimless.   1x1       eccentricity
%  nu                O      rad        1x1       true anomaly      
%
%  Modification History
%  11/17/00 - Created, S. Hughes

E = Kepler(M,e);
nu = E2nu(E,e);