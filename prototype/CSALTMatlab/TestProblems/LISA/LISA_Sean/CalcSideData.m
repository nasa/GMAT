
%% Calculate Geometry of LISA Constellation
function [ sv1,  sv2,  sv3, dsv1, dsv2, dsv3, s1,   s2,   s3,...
           ds1,  ds2,  ds3, re1,  re2,  re3, ang1, ang2, ang3] = ...
           CalcSideData(X1,X2,X3)

d2r  = pi/180;
re1  = norm(X1(1:3));
re2  = norm(X2(1:3));
re3  = norm(X3(1:3));
sv1  = X1(1:3) - X2(1:3);
sv2  = X2(1:3) - X3(1:3);
sv3  = X1(1:3) - X3(1:3);
dsv1 = X1(4:6) - X2(4:6);
dsv2 = X2(4:6) - X3(4:6);
dsv3 = X1(4:6) - X3(4:6);
s1   = norm(sv1(1:3));
s2   = norm(sv2(1:3));
s3   = norm(sv3(1:3));
ds1  = dot(dsv1(1:3),sv1(1:3))/s1;
ds2  = dot(dsv2(1:3),sv2(1:3))/s2;
ds3  = dot(dsv3(1:3),sv3(1:3))/s3;
ang1 = vecangle(sv2(1:3),-sv1(1:3))/d2r;
ang2 = vecangle(sv2(1:3), sv3(1:3))/d2r;
ang3 = vecangle(sv1(1:3), sv3(1:3))/d2r;
end