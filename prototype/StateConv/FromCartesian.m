function [state] = FromCartesian(cart,outPutType,mu)

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

%  Validate the input
if size(cart,1) ~=6 || size(cart,2) ~=1
   disp('Error in ToCartesian:  Input state vector must have dimensions of 6 x 1');
   return
end
if nargin < 3
    mu = 398600.4415;
end

if outPutType == 1
    state = cart;                    %  The Cartesian state
elseif outPutType == 2
    state = Cart2Kep(cart,mu);       %  The Keplerian state
elseif outPutType == 3
    state = Cart2Kep(cart,mu);
    state = Kep2ModKep(state);    %  The Modified Keplerian state
elseif outPutType == 4
    state = Cart2SphAzFPA(cart);  %  The SphericalAzFPA state
elseif outPutType == 5
    state = Cart2SphRADec(cart);  %  The SphericalRADec state
elseif outPutType == 6
    state = Cart2Equinoctial(cart,mu);  % The Equinoctial state
elseif outPutType == 7
    state = Cart2Mee(cart,mu);      % The Modified Equinoctial state
elseif outPutType == 8
%     kep = Cart2Kep(cart,mu); % The Nonsingular Keplerian state
%     state = KEP2NSK(kep);
    eq = Cart2Equinoctial(cart, mu);
    state = Equinoctial2Nonsingular(eq, mu);
%     state = Cart2Nonsingular(cart, mu);
elseif outPutType == 9
    state = Cart2Dela(cart,mu);
else
	state = [];
	disp('Error in ToCartesian:  input state type is not supported')
end