% ------------------------------------------------------------------------------
%
%                           function unit
%
%  this function calculates a unit vector given the original vector.  if a
%    zero vector is input, the vector is set to zero.
%
%  author        : david vallado                  719-573-2600    4 jun 2002
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    vec         - vector
%
%  outputs       :
%    outvec      - unit vector
%
%  locals        :
%    i           - index
%
%  coupling      :
%    mag           magnitude of a vector
%
% [outvec] = norm ( vec );
% ------------------------------------------------------------------------------

function [outvec] = unit ( vec );

        % -------------------------  implementation   -----------------
        small = 0.000001;
        magv = mag(vec);
        if ( magv > small )
            for i= 1: 3
                outvec(i)= vec(i)/magv;
              end
          else
            for i= 1: 3
                outvec(i)= 0.0;
              end
          end

