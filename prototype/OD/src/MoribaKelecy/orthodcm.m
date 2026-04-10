% This script will orthogonalize a direction cosine matrix
function [orthoDCM] = orthodcm(DCM)

%...purpose:  algorithm to orthogonalize transformation matrices
%
%...input:   DCM = 3 x 3 direction cosine (transformation) matrix
%
%...output:  orthoDCM = 3 x 3 orthogonalized DCM
%
%   m. jah, afrl

format long g

%...define constants
c12 = 1/2;
c34 = 3/4;
c56 = 5/6;
c78 = 7/8;
c910 = 9/10;
c1112 = 11/12;
c1314 = 13/14;
c1516 = 15/16;
c1718 = 17/18;
c1920 = 19/20;

%...do the orthogonalization
delT = DCM*DCM'-eye(3);
orthoDCM = DCM*(eye(3)-c12*delT+c12*c34*delT.^2-c12*c34*c56*delT.^3+c12*c34*c56*c78*delT.^4-c12*c34*c56*c78*c910*delT.^5+...
    c12*c34*c56*c78*c910*c1112*delT.^6-c12*c34*c56*c78*c910*c1112*c1314*delT.^7+c12*c34*c56*c78*c910*c1112*c1314*c1516*delT.^8-...
c12*c34*c56*c78*c910*c1112*c1314*c1516*c1718*delT.^9+c12*c34*c56*c78*c910*c1112*c1314*c1516*c1718*c1920*delT^10);

%...end of function [orthoDCM] = orthodcm(DCM)
