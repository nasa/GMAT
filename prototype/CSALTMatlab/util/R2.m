function Rotmat = R2(angle)
   c = cos(angle); s = sin(angle);
   Rotmat = [ c 0 -s; 0 1 0; s 0 c];
