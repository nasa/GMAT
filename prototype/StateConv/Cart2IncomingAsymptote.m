function [HypAsymptote] = Cart2IncomingAsymptote(cartState,mu)

% HypAsymptote = [radPer C3 RLA DLA BPA TA];

%  Set mu.  If not provided assume Earth.
if nargin < 2
    mu = 398600.4418;
end

%%  Compute variables needed elsewhere
rv   = cartState(1:3,1);
rmag = norm(rv);
vv   = cartState(4:6,1);
vmag = norm(vv);
hv   = cross(rv,vv);
hmag = norm(hv);
ev   = (vmag*vmag - mu/rmag)*rv/mu - dot(rv,vv)*vv/mu;
ECC  = norm(ev);
%%  Compute C3 and SMA
C3     = vmag * vmag - 2 * mu / rmag;

if abs(C3) < 1e-7
        cartState = [];
        disp('Error in Cart \2HypAsymptote: Cannot convert from  Cartestion to Asymptote because orbit is parabolic.');   
    return
end

SMA    = -mu/C3;
%% Compute radPer

radPer = SMA*(1 - ECC);
%%  Compute Asymptote vector
%  If orbit is hyperbolic, use asymptote

if C3 >= 1e-7
    fac1  = 1/(1 + C3*(hmag*hmag)/(mu*mu));
    svhat = fac1*(-sqrt(C3)/mu*cross(hv,ev) - ev); % The original expression written by SPH has an error. It is fixed. svhat indicates position direction at infinity. 
        
else
    %
    %  The two options below are really hacks, the asymptote doesn't exist.  However,
    %  having a definition that is unique and has an inverse computation is
    %  useful, especially when numeric solvers change regimes during
    %  iterative processes. - SPH
    %
    %  If orbit is elliptic, use apsides vector
    if ECC > 1e-7
        svhat = -ev/ECC;
        disp('Warning in Cart2HypAsymptote: Orbit is elliptic so using Apsides vector for asymptote.')
        %  If orbit is circular, throw an exception
    else
        HypAsymptote = [];
        disp('Error in Cart2HypAsymptote: Cannot convert from Cartestion to Asymptote because orbit is circular.');   
    return
    end
end
%%  Compute B vector azimuth at infinity (BvA)
uz=[0 0 1].';
if real(acos(abs(dot(svhat,uz)))) < 1e-11
    HypAsymptote = [];
    disp('Error in Cart2HypAsymptote: Cannot convert from  Cartestion to Asymptote because Asymptote vector is aligned with z-direction.')
end

Ehat=cross(uz,svhat)/norm(cross(svhat,uz)); % singular when svhat is parallel to uz
Nhat=cross(svhat,Ehat);
Bvector=cross(hv,svhat);

cosBvA=dot(Bvector,-Nhat)./hmag;
sinBvA=dot(Bvector,Ehat)./hmag;
BvA=atan2(sinBvA,cosBvA);

if BvA < 0 
    BvA = BvA + 2*pi;
end

%%  Compute other elements
DLA    = real(asin(svhat(3))); % range is -pi/2, to pi/2
RLA    = atan2(svhat(2),svhat(1));
if RLA < 0
    RLA = RLA + 2*pi;
end

TA=acos(dot(ev,rv)/rmag/ECC);

if dot(vv,rv) < 0
    TA = 2*pi - TA;
end

%%  Assemble the output
HypAsymptote = [radPer C3 rad2deg([RLA DLA BvA TA])];