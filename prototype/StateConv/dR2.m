function dRotmat = dR2(angle)
   c = cos(angle); s = sin(angle);
  %    Rotmat = [ c 0 -s; 0 1 0; s 0 c];
       dRotmat = [ -s 0 -c; 0 0 0; c 0 -s];