function [R] = ComputeEulerAxisRotationMatrix(RotationVector, RotationAngle)
%  Compute rotation matrix for Euler axis rotation
ux = RotationVector(1);
uy = RotationVector(2);
uz = RotationVector(3);

theta = RotationAngle;

R11 = cos(theta) + ux^2*(1-cos(theta));
R12 = ux*uy*(1-cos(theta)) - uz*sin(theta);
R13 = ux*uz*(1-cos(theta)) + uy*sin(theta);
R21 = uy*ux*(1-cos(theta)) + uz*sin(theta);
R22 = cos(theta) + uy^2*(1-cos(theta));
R23 = uy*uz*(1-cos(theta)) - ux*sin(theta);
R31 = uz*ux*(1-cos(theta)) - uy*sin(theta);
R32 = uz*uy*(1-cos(theta)) + ux*sin(theta);
R33 = cos(theta) + uz^2*(1-cos(theta));

R = [R11 R12 R13; R21 R22 R23; R31 R32 R33]';