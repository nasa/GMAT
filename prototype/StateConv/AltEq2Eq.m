
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   conversion Alternate Equinoctial State to Equinoctial State        
%   by HYKim, CBNU                                             
%   modified on May,22,2013         
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



function Equinoctial = AltEq2Eq(AltEquinoctial)

% If mu in not provided, use the mu for Earth
if nargin < 2
   mu = 398600.4415;
end

% Initializations
sma     = AltEquinoctial(1,1);
h       = AltEquinoctial(2,1);
k       = AltEquinoctial(3,1);
altp    = AltEquinoctial(4,1);
altq    = AltEquinoctial(5,1);
meanLon = AltEquinoctial(6,1);

% Sanity check input
if max(size(AltEquinoctial))~= 6
    Equinoctial = [];
    disp('Error: Alternate equinoctial state have six elements')
    return
end

% Using the relationship between classical and equnoctial elements
incl = 2*asin(sqrt(altp^2+altq^2));

    if(incl == pi)
    disp('Warning: Conversion results is near singularity that occurs when inclination = 180(deg).');
     return
    end
    
    c = cos(incl/2);
        
% Find the equnoctial elements
p   = altp/c;
q   = altq/c;

% Output
Equinoctial = real([sma h k p q meanLon]'); 



