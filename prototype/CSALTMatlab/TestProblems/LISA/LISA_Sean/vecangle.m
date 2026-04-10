%% Compute angle (in radians) between two vectors
function theta = vecangle(vec1,vec2)
%  function theta = vecangle(vec1,vec2)
%
%  This function calculates the angle between two vectors using the
%  dot product.  The solution is always in quadrant one or two.
%
%  Variable I/O
%
%   Variable Name    I/0    Units     Dim.  Description
%   vec1              I      UOI      3x1    Vector one
%   vec2              I      UOI      3x1    Vector two
%   theta             O      rad      1x1    angel between vector in radians
%
%  External References:  None
%
%  Modification History
%  S. Hughes - 05/25/01 -  Created File
arg = dot(vec1,vec2) / norm(vec1) / norm(vec2);
theta = acos( arg );
end