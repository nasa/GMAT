% ------------------------------------------------------------------------------
%
%                            function rot3
%
%  this function performs a rotation about the 3rd axis.
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
% [outvec] = rot3 ( vec, xval );
% ----------------------------------------------------------------------------- }

function [outvec] = rot3 ( vec, xval );

        temp= vec(2);
        c= cos( xval );
        s= sin( xval );

        outvec(2)= c*vec(2) - s*vec(1);
        outvec(1)= c*vec(1) + s*temp;
        outvec(3)= vec(3);

