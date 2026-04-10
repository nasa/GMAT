function speed_of_light = speed_of_light(varargin)
%
% Speed of light
%
% Optional input 'm' to get output in m/s, otherwise the output is in
% km/s
%
% Resource: Fundamentals of Astrodynamics and Applications, Vallado Fourth
% Edition

if nargin>1
    error('To many inputs defined!')
elseif nargin
    if strcmp(varargin{1},'m')
        speed_of_light = 2.99792458E5 *(1000);
    else
        error('Invalid input!')
    end
else
    speed_of_light = 2.99792458E5;
end