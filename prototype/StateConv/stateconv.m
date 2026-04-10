function stateOut = stateconv(stateIn,inputType,outPutType,mu)

% Defintion of Types
% Type = 1;  Cartesian             [X Y Z VX VY BZ]';
% Type = 2;  Classical Keplerian   [SMA ECC INC AOP RAAN TA]';
% Type = 3;  Modified keplerian    [RadPer RadApo INC AOP RAAN TA]';
% Type = 4;  SphericalAzFPA        [RMAG RA DEC VMAG AZI FPA]';
% Type = 5;  SphericalRaDec        [RMAG RA DEC VMAG RAV DECV]';
% Type = 6;  Equinoctial           [SMA H K P Q MLONG]'
% Type = 7;  Modified Equinoctial  [P F G H K L]'
% Type = 8;  Nonsingular Keplerian [SMA E1 E2 E3 E4 E5]'
% Type = 9;  Delaunay              [L G H l g h ]';

% Validate the input
if max(size(stateIn)) ~=6 || min(size(stateIn)) ~=1
   disp('Error in StateConv:  Input state vector has incorrect dimensions');
   stateOut = [];
   return
end
   
%  Use earth's mu if not provided by user
if nargin < 4
    mu = 398600.4415;
end

%  If input state type and output type are the same, return the input.
if  (inputType == outPutType )
    stateOut = stateIn;
    return
end

%   Convert to cartesian state.
if inputType ~= 1
    cart = ToCartesian(stateIn,inputType,mu);
else
    cart = stateIn;
end

%  Convert to state-type requested.
switch outPutType
   case {1}
       stateOut = cart;
   case {2,3,4,5,6,7,8,9}
       stateOut = FromCartesian(cart,outPutType,mu);
end


