function [R] = ComputeAlignConstrainedDCM(aVec1, aVec2,...
                                          cVec1, cVec2)
%  This function computes a rotation matrix based on an aligned-constrained 
%  coordinate system definition.  The function returns the matrix from
%  frame 2 to frame 1.  The approach uses the Euler axis/angle method to
%  transform first from frame 1 to an intemediate frame such that the align
%  ment vector conditions are satisfied.  Then, the Euler axis/angle method
%  is used to rotate from the intermediate frame to frame 2 about the
%  alignment vector through an angle to satisfy the constraint vector
%  conditions.
%
%  Inputs:
%  aVec1, aVec1: Alignment vectors expressed in frame 1 and 2 respectively
%  cVec1, cVec1: Constraint vectors expressed in frame 1 and 2 respectively
%       
%  Author: S. Hughes.  steven.p.hughes@nasa.gov

%% Initializations                                      
aVec1Hat = aVec1/norm(aVec1);
aVec2Hat = aVec2/norm(aVec2);
cVec1Hat = cVec1/norm(cVec1);
cVec2Hat = cVec2/norm(cVec2);

%% Compute first rotation which aligns the desired coordinate axis
phi    = acos(aVec2Hat'*aVec1Hat);
rotVec = cross(aVec2Hat,aVec1Hat);
rotVec = rotVec/norm(rotVec);
R1     = aphi2R(rotVec,phi);

%% Compute Second rotation which constrains the coordinates
rotVec     = aVec2Hat;
nVec1      = R1*cross(cVec1Hat, aVec1Hat);
nVec1Hat   = nVec1/norm(nVec1);
nVec2      = cross(cVec2Hat, aVec2Hat);
nVec2Hat   = nVec2/norm(nVec2);
phi        = acos(nVec1Hat'*nVec2Hat);
R2         = aphi2R(rotVec,phi);

%% Assemble the rotation matrix
R = (R2*R1)';