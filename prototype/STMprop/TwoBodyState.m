function [rv,vv] = TwoBodyState(epoch,bodyId,centerId)


%% Get data for body and center 
[bodyMu,bodyRefEpoch,bodyOrbEl,bodycbId]         = GetBodyData(bodyId);
[centerMu,centerRefEpoch,centerOrbEl,centercbId] = GetBodyData(centerId);
muSum = bodyMu + centerMu;
dT    = (epoch - bodyRefEpoch)*86400;

%%  Perform the propagation.
%  The central body is the natural central body of the pair
if centerId == bodycbId 
    orbEl     = KeplerProp(bodyOrbEl,dT,muSum);
    cartState = stateconv([orbEl(1:3) orbEl(5) orbEl(4) orbEl(6)],2,1);
%  The body requiring propagation is the natural central body of the pair 
elseif bodyId == centercbId;
    orbEl     = KeplerProp(centerOrbEl,dT,muSum);
    cartState = -stateconv([orbEl(1:3) orbEl(5) orbEl(4) orbEl(6)],2,1);
%   The pair share the same central body
elseif bodycbId == centercbId
    [muCb] = GetBodyData(bodycbId);
    elBody = KeplerProp(bodyOrbEl,dT,bodyMu + muCb);
    elCb = KeplerProp(centerOrbEl,dT,centerMu + muCb);
    cartBody = stateconv([elBody(1:3) elBody(5) elBody(4) elBody(6)],2,1);
    cartCb   = stateconv([elCb(1:3) elCb(5) elCb(4) elCb(6)],2,1);
    cartState = cartBody - cartCb;
else
    disp('Error in TwoBodyStatem.m:  This function currently only handles co-orbiting bodies')
    stop
end
rv = cartState(1:3,1);
vv = cartState(4:6,1);

%%  Solve Keplers Equation
function newOrbEl = KeplerProp(propOrbEl,dT,mu)

meanMotion = sqrt(mu/propOrbEl(1,1)^3);
orbitPeriod = 2*pi/meanMotion;
meanAnomaly = nu2M(propOrbEl(1,6),propOrbEl(1,2));

%  Mod dT to avoid multiplication by large number 
if dT > 0
    dT = mod(dT,orbitPeriod);
elseif  dT< 0
    dT = mod(dT,-orbitPeriod);
end

%  Solve Kepler's equation
newMeanAnomaly = mod( meanAnomaly + meanMotion*dT,2*pi);
newTrueAnomaly = M2nu(newMeanAnomaly,propOrbEl(1,2));
newOrbEl       = [propOrbEl(1,1:5) newTrueAnomaly];
