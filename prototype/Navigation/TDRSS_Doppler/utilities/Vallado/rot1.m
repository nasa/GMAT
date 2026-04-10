% ------------------------------------------------------------------------------
%
%                                  rot1
%
%  this function performs a rotation about the 1st axis.
%
%  author        : david vallado                  719-573-2600   27 may 2002
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    vec         - input vector
%    xval        - angle of rotation              rad
%
%  outputs       :
%    outvec      - vector result
%
%  locals        :
%    c           - cosine of the angle xval
%    s           - sine of the angle xval
%    temp        - temporary extended value
%
%  coupling      :
%    none.
%
% [outvec] = rot1 ( vec, xval );
% ----------------------------------------------------------------------------- }

function [outvec] = rot1 ( vec, xval );

        temp= vec(3);
        c= cos( xval );
        s= sin( xval );

        outvec(3)= c*vec(3) - s*vec(2);
        outvec(2)= c*vec(2) + s*temp;
        outvec(1)= vec(1);


