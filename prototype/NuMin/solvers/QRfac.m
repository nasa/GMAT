function [Q,R] = qrfac(A)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

[m,n] = size(A);
Beta  = zeros(m);
if m < n
    disp('Error:  qrfac.m requires a m x n rectangular matrix with m >= n ') 
    Q = [];
    R = [];
    return
end

for j = 1:n
    [v,Beta(j)]   = householder(A(j:m,j));
    A(j:m,j:n) = ( eye(m-j+1) - Beta(j)*v*v')*A(j:m,j:n);
    if j < m
       A(j+1:m,j) = v(2:m-j+1); 
    end
end
R = triu(A);

eyem = eye(m);
Q = eyem;
for j = n:-1:1
   v          = [1; A(j+1:m,j) ]; 
   Q(j:m,j:m) = (eye(m - j + 1) - Beta(j)*v*v')*Q(j:m,j:m);
end

