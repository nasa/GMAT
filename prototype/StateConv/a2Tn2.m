function [T,n] = a2Tn(a,GM)


%  function [T,n] = a2Tn(a)
%
%  This function calculates the osculating period and mean motion give the semimajor axis
%
%  Variable I/O
%
%  Variable Name    I/0    Units     Description
%  a                 I      m        orbit semimajor axis
%  n                 O      rad/s    mean motion
%  T                 O      s        orbit period
%
%  Modification History
%  06/30/00  -  Created,  S. Hughes


n = sqrt(GM/a^3);
T = 2*pi/n;


