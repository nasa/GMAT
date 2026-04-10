function omega_earth = omega_earth(varargin)
%
% Earths Rotation Rate
%
% Optional input 'deg' to get output in deg/s, otherwise the output is in
% rad/s
%
% Resource: Fundamentals of Astrodynamics and Applications, Vallado Fourth
% Edition

if nargin>1
    error('To many inputs defined!')
elseif nargin
    if strcmp(varargin{1},'deg')
        omega_earth = r2d(0.000072921158553);
    else
        error('Invalid input!')
    end
else
    omega_earth = 0.000072921158553;
end
