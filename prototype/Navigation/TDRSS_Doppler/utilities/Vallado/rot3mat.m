% ------------------------------------------------------------------------------
%
%                                  rot3mat
%
%  this function sets up a rotation matrix for an input angle about the third
%    axis.
%
%  author        : david vallado                  719-573-2600   10 jan 2003
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    xval        - angle of rotation              rad
%
%  outputs       :
%    outmat      - matrix result
%
%  locals        :
%    c           - cosine of the angle xval
%    s           - sine of the angle xval
%
%  coupling      :
%    none.
%
% [outmat] = rot3mat ( xval );
% ----------------------------------------------------------------------------- }

function [outmat] = rot3mat ( xval );

        c= cos( xval );
        s= sin( xval );

        outmat(1,1)= c;
        outmat(1,2)= s;
        outmat(1,3)= 0.0;

        outmat(2,1)= -s;
        outmat(2,2)= c;
        outmat(2,3)= 0.0;

        outmat(3,1)= 0.0;
        outmat(3,2)= 0.0;
        outmat(3,3)= 1.0;


