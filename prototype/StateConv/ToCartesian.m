function [cart] = ToCartesian(stateIn,inputType,mu)

% Defintion of Types
% Type = 1;  Cartesian             [X Y Z VX VY BZ]';
% Type = 2;  Classical Keplerian   [SMA ECC INC RAAN AOP TA]';
% Type = 3;  Modified keplerian    [RadPer RadApo INC RAAN AOP TA]';
% Type = 4;  SphericalAzFPA        [RMAG RA DEC VMAG AZI FPA]';
% Type = 5;  SphericalRaDec        [RMAG RA DEC VMAG RAV DECV]';
% Type = 6;  Equinoctial           [SMA H K P Q MLONG]'
% Type = 7;  Modified Equinoctial  [P F G H K L]'
% Type = 8;  Nonsingular Keplerian [SMA E1 E2 E3 E4 E5]'
% Type = 9;  Delaunay              [L G H MA AOP RAAN]'

%  Use earth's mu if not provided 
if nargin < 3
    mu = 398600.4415;
end

%  Perform the conversion depending upon the file type
if inputType == 1
    cart = stateIn;                    %  The Cartesian state
elseif inputType == 2
    cart = Kep2Cart(stateIn,mu);       %  The Keplerian state
elseif inputType == 3
    kep  = ModKep2Kep(stateIn);
    cart = Kep2Cart(kep,mu);           %  The Modified Keplerian state
elseif inputType == 4
    cart = SphAzFPA2Cart(stateIn);     %  The SphericalAzFPA state
elseif inputType == 5
    cart = SphRADec2Cart(stateIn);     %  The SphericalRADec state
elseif inputType == 6
    cart = Equinoctial2Cart(stateIn,mu);  % The Equinoctial state
elseif inputType == 7
    cart = Mee2Cart(stateIn,mu);        % The Modified Equinoctial state
elseif inputType == 8
%     kep = NSK2KEP(stateIn);   % The Nonsingular Keplerian state
%     cart = Kep2Cart(kep,mu);
    eq = Nonsingular2Equinoctial(stateIn, mu);
    cart = Equinoctial2Cart(eq, mu);
elseif inputType == 9                 % Delaunay state
    cart = Dela2Cart(stateIn,mu);
else
	cart = [];
	disp('Error in ToCartesian:  input state type is not supported')
end

