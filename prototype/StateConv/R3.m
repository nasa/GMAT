function Rotmat = R3(angle)
   c = cos(angle); s = sin(angle);
   Rotmat = [ c s 0; -s c 0; 0 0 1];
