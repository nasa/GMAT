function [J,oe] = dOEdCart(state,type);

%  Define constants
mu = 398600.4415;

%  Extract position and velocity vector from input
if type == 1;
    rv = state(1:3,1);
    vv = state(4:6,1);
elseif type == 2
    cart = stateconv(state,2,1)
    rv = cart(1:3,1);
    vv = cart(4:6,1);
end

%  Basic orbit properties
r  = norm(rv);
v  = norm(vv);
hv = cross(rv,vv);
h  = norm(hv);
hvz = hv(3,1);

%  Useful unit vectors in calculating RAAN and AOP
zhat = [0 0 1]';
xhat = [1 0 0]';

%  The RAAN vector (nv)
nv   = cross(zhat,hv);
n    = norm(nv);
nvx  = xhat'*nv;

%  The eccentricity vector, energy, and SMA
ev = ( (v^2 - mu/r)*rv - dot(rv,vv)*vv )/mu;
e  = norm(ev);
E  = v^2/2 - mu/r;
a  = -mu/2/E;

%--------------------------------------------------------------------------
%------------------ Derivative of Intermediate Quantities -----------------
%--------------------------------------------------------------------------

%  Derivatives of eccentricity vector and eccentricity unit vector
devdrv   = ( (v^2 - mu/r)*eye(3) + mu*rv*rv'/r^3 - vv*vv' )/mu;
devdvv   = 1/mu*( 2*rv*vv' - rv'*vv*eye(3) - vv*rv');
dehatdrv = devdrv/e - ev*ev'*devdrv/e^3; 
dehatdvv = devdvv/e - ev*ev'*devdvv/e^3; 

%  Derivatives of the node vector and node unit vector
dndrv    = -nv'/n*skew(zhat)*skew(vv);
dnvdrv   = -skew(zhat)*skew(vv);
dndvv    = nv'/n*skew(zhat)*skew(rv);
dnvdvv   = skew(zhat)*skew(rv);
dnhatdrv = dnvdrv/n - nv*dndrv/n^2; 
dnhatdvv = dnvdvv/n - nv*dndvv/n^2; 

%--------------------------------------------------------------------------
%------------------ Derivative of Orbital Elements ------------------------
%--------------------------------------------------------------------------

%----- SMA Derivatives
dadrv  = 2*(a/r)^2*rv'/r;
dadvv  = 2*a^2/mu*vv';

%----- ECC Derivatives
dedrv  = ev'/e/mu*((v^2 - mu/r)*eye(3) +mu*rv*rv'/r^3 - vv*vv');
dedvv  = ev'/e/mu*( 2*rv*vv' - rv'*vv*eye(3) - vv*rv');

%----- INC Derivatives
didrv  =  zhat'/sqrt(1 - (hvz/h)^2 )*( 1/h*eye(3) - hv*hv'/h^3 )*skew(vv);
didvv  = -zhat'/sqrt(1 - (hvz/h)^2 )*( 1/h*eye(3) - hv*hv'/h^3 )*skew(rv);

%----- RAAN Derivatives
dOmdrv = -1/sqrt(1 - (nvx/n)^2)*(nvx/n^3*nv' - 1/n*xhat')*skew(zhat)*skew(vv);
dOmdvv = 1/sqrt(1 - (nvx/n)^2)*(nvx/n^3*nv' - 1/n*xhat')*skew(zhat)*skew(rv);

%----- AOP Derivatives
domdrv   = -1/sqrt(1 - (dot(ev,nv)/e/n)^2)* (  nv'/n*dehatdrv  + ev'/e*dnhatdrv  ) ;
domdvv   = -1/sqrt(1 - (dot(ev,nv)/e/n)^2)* (  nv'/n*dehatdvv  + ev'/e*dnhatdvv  ) ;

%----- TA Derivatives
dnudrv   = -1/sqrt(1 - (dot(ev,rv)/e/r)^2)* (  rv'/r*dehatdrv  + ev'/e*(eye(3)/r - rv*rv'/r^3) ) ;
dnudvv   = -1/sqrt(1 - (dot(ev,rv)/e/r)^2)* (  rv'/r*dehatdvv  ) ;

%--------------------------------------------------------------------------
%-------------------------- Assemble the Jacobian -------------------------
%--------------------------------------------------------------------------
J(:,1) = [ dadrv dadvv ];
J(:,2) = [ dedrv dedvv ];
J(:,3) = [ didrv didvv ];
J(:,4) = [ domdrv domdvv ];
J(:,5) = [ dOmdrv dOmdvv ];
J(:,6) = [ dnudrv dnudvv ];
J = J';


