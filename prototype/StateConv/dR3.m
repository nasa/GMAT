function dRotmat = R3(angle)
   c = cos(angle); s = sin(angle);
    % Rotmat =   [ c s 0; -s c 0; 0 0 1];
      dRotmat = [ -s c 0; -c -s 0; 0 0 0];
