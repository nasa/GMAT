


%%  Define vectors and compute rotation matrix

% Frame 1 is inertial
% Frame 2 is spacecraft body

%  Define Alignment vectors to align body y-axis with s/c nadir direction
aVec1 = -[7100 100 4444]';
aVec2 = [1 1 .1]';

%  Define Constraint vectors to constrain body z-axis to velocity direction
cVec1 = [0 7.35 1]';
cVec2 = [1 1 1]';

%  Compute the rotation matrix
[R] = ComputeAlignConstrainedDCM(aVec1, aVec2, cVec1, cVec2);

%% Test the results

%  Initializations
aVec1Hat = aVec1/norm(aVec1);
aVec2Hat = aVec2/norm(aVec2);
cVec1Hat = cVec1/norm(cVec1);
cVec2Hat = cVec2/norm(cVec2);
nVec1    = cross(aVec1Hat,cVec1Hat);
nVec1Hat = nVec1/norm(nVec1);
nVec2    = cross(aVec2Hat,cVec2Hat);
nVec2Hat = nVec2/norm(nVec2);

%  If computation worked the following matrices test1 and
%  test2  must be zero
test1    = aVec1Hat - R*aVec2Hat
test2    = nVec1Hat - R*nVec2Hat