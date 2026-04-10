% R = aphi2R(a,phi)
%     a is the Euler axis (a 3 x 1 matrix)
%     phi is the Euler principal angle, a scalar
%     returns the rotation matrix
function R = aphi2R(a,phi)
[m,n]=size(a);
if (m==1 & n==3)
    a=a';
end
if ((m~=1 & n~=1) | (m~=3 & n~=3))
   disp('a is not a 3 x 1 matrix');
   R=NaN*ones(3,3);
   return;
end
c = cos(phi); s = sin(phi);
R = c*eye(3) + (1-c)*a*a' -s*skew(a);
