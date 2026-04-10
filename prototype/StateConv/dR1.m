% this function returns a rotation matrix corresponding to
% a "1" rotation through angle
function dRotmat = dR1(angle)
   c = cos(angle); s = sin(angle);
   %Rotmat = [ 1 0 0; 0 c s; 0 -s c ];
  dRotmat = [ 0 0 0; 0 -s c; 0 -c -s ];