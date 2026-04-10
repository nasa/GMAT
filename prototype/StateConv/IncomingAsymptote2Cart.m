function cartState = IncomingAsymptote2Cart(HypAsymptote,mu)

% cartState = [rx ry rz vx vy vz];
% HypAsymptote = [radPer C3 RLA DLA AMI TA];
%  Set mu.  If not provided assume Earth.

if nargin < 2
    mu = 398600.4418;
end

%%  Compute variables needed elsewhere

HypAsymptote(3:6)=deg2rad(HypAsymptote(3:6));

radPer = HypAsymptote(1);
C3     = HypAsymptote(2);
RLA    = HypAsymptote(3);
DLA    = HypAsymptote(4);
BvA    = HypAsymptote(5);
TA     = HypAsymptote(6);


%%  Compute SMA and ECC

SMA    = -mu/C3;
ECC    = 1 - radPer/SMA;

if ECC < 1e-7
        cartState = [];
        disp('Error in HypAsymptote2Cart: Cannot convert from Asymptote to Cartestion because orbit is circular.');   
    return
end

%%  Compute angular momentum vector

svhat = [cos(DLA)*cos(RLA) cos(DLA)*sin(RLA) sin(DLA)].';
uz=[ 0 0 1].';

if acos(abs(dot(svhat,uz))) < 1e-7
    HypAsymptote = [];
    disp('Error in HypAsymptote2Cart: Cannot convert from Asymptote state to Cartestion because Asymptote vector is aligned with z-direction.')
end
AMI=pi/2-BvA; % AMI is angular momentum azimuth at infinity
Ehat=cross(uz,svhat)/norm(cross(svhat,uz));
Nhat=cross(svhat,Ehat);
hvhat=sin(AMI)*Ehat+cos(AMI)*Nhat;


if C3 <= - 1e-7
        disp('Warning in HypAsymptote2Cart: Orbit is elliptic so using Apsides vector for asymptote.')
        
        
        %  If orbit is circular, throw an exception
    elseif abs(C3) < 1e-7
        cartState = [];
        disp('Error in HypAsymptote2Cart: Cannot convert from hyperbolic Asymptote state to Cartestion because orbit is parabolic.');   
        return
end



%%  Compute INC 

INC=real(acos(dot(uz,hvhat)));
nv=cross(uz,hvhat);
nmag=norm(nv);
ux=[ 1 0 0].';
%% Compute Deflection Angle

TAMAX=pi - real(acos(1/ECC));
%% Compute RAAN and AOP



Ovhat  = cross(hvhat,svhat);
rpvhat = sin(TAMAX)*Ovhat + cos(TAMAX)*svhat; 

    
    
if INC > 1e-7 && INC < pi-1e-7 
    
    RAAN = real(acos(dot(ux,nv)/nmag));
    if nv(2) < 0
        RAAN=2*pi-RAAN;
    end
    
    AOP=real(acos(dot(nv/nmag,rpvhat)));
    if rpvhat(3) < 0
        AOP=2*pi-AOP;
    end
elseif INC <= 1e-7 % equatorial prograde 
    RAAN = 0; 
    AOP    = real(acos(rpvhat(1)));
    if ( rpvhat(2) < 0 )
        AOP = 2*pi - AOP;
    end
elseif INC >= pi-1e-7  % equatorial retrograde
    AOP = 0; 
    RAAN    = real(acos(rpvhat(1)));
    if ( rpvhat(2) < 0 )
        RAAN = 2*pi - RAAN;
    end
end


%%  Assemble classical orbital elements
COEs=real([SMA ECC INC AOP RAAN TA]);
%% Kep2Cart

cartState = Kep2Cart(COEs,mu); % cart(1:3)= position vector, cart(4:6) = velocity vector