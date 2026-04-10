
function [view, nadir_angle] = view_between_sats(r_observer_eci, r_user_eci)
%
% Computes if a satellite is viewable with respect to another satellite
%
% State vectors should be in ECI with common units. 
%
% Output: View=1, satellite is viewable. View = 0, not in view
%          Nadir angle: This is a bit hacky. I should use the 
%                        elevation angle wrt the lvlh local horizon when
%                        more then one sat is in view( tdrss/user
%                        application)
%
% TODO: Unit test

% compute needed magnitudes and relative vector
r_sat_observer = norm(r_observer_eci);
r_vector_between =  r_observer_eci-r_user_eci;
Dsat = norm(r_vector_between);

% get nadir angle between 
eta = atan2(norm(cross(r_observer_eci,r_vector_between)),dot(r_vector_between,r_observer_eci));

% find the angular radius of the earth as seen from the observer
rho = asin(RE/r_sat_observer);

view = 0;

 if eta > rho
     view = 1;
 elseif eta < rho && Dsat < r_sat_observer
     view = 1;
 else
     view = 0;
 end
 
 % hacky. I should use the elevation angle wrt the local horizon to compute
 % which sat to use when more then one is in view, but for now ill use the
 % lowest nadir angle.
 
 nadir_angle = r2d(eta);


end