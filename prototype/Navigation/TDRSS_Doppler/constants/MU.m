function MU = MU(varargin)
%
% Earths Gravitational Parameter
%
% Optional input 'm' to get output in m^3/s^2, otherwise the output is in
% km^3/s^2
%
% Resource: Fundamentals of Astrodynamics and Applications, Vallado Fourth
% Edition

if nargin>1
    error('To many inputs defined!')
elseif nargin
    if strcmp(varargin{1},'m')
        MU = 398600.4118 *(1000^3);
    else
        error('Invalid input!')
    end
else
    MU = 398600.4118;
end




