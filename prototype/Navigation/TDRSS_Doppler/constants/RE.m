function RE = RE(varargin)
%
% Earths Mean Equatorial Radius
%
% Optional input 'm' to get output in m, otherwise the output is in
% km
%
% Resource: Fundamentals of Astrodynamics and Applications, Vallado Fourth
% Edition

if nargin>1
    error('To many inputs defined!')
elseif nargin
    if strcmp(varargin{1},'m')
        RE = 6378.137 *(1000);
    else
        error('Invalid input!')
    end
else
    RE = 6378.137;
end