
function a=n_to_sma(n)
% Compute semimajor axis from mean motion
%
% Inputs: n = mean motion (rads/s)
%
% Output: a = semimajor axis (km)

a = (MU/n^2)^(1/3);

end