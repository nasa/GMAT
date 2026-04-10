
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   conversion Equinoctial State to Alternate Equinoctial State    
%   by HYKim, CBNU       
%   modified on May,22,2013     
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



function AltEquinoctial = Eq2AltEq(Equinoctial)

% If mu in not provided, use the mu for Earth
if nargin < 2
   mu = 398600.4415;
end

% Initializations
sma = Equinoctial(1,1);
h   = Equinoctial(2,1);
k   = Equinoctial(3,1);
p   = Equinoctial(4,1);
q   = Equinoctial(5,1);
meanLon = Equinoctial(6,1);

% Sanity check input
if max(size(Equinoctial))~= 6
    AltEquinoctial = [];
    disp('Error: Equinoctial elements have six elements')
    return
end

% Using the relationship between classical and equnoctial elements
incl = 2*atan(sqrt(p^2+q^2));

    if(incl == pi)
    disp('Warning: Conversion results is near singularity that occurs when inclination = 180(deg).');
     return
    end
    
    c = cos(incl/2);  

% Find the alternate equinoctial elements.
altp = p*c;
altq = q*c;

% Output
AltEquinoctial = real([sma h k altp altq meanLon]');     



