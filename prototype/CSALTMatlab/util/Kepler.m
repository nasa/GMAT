function E = Kepler(M,e)

%  function E = Kepler(M,e)
%
%  Solves Keplers Problem for the eccentric anomaly using Newton Iteration
%
%  Variable Name    I/0    Units      Dimens.    Description
%---------------------------------------------------------------------------------------
%  M                 I      rad        1x1       Mean anomaly at epoch        
%  e                 I      dimless.   1x1       eccentricity
%  E                 O      rad        1x1       Eccentric Anomaly at epoch
%  Modification History
%  06/26/00 - Created, S. Hughes

%  pick initial guess for E
if ( -pi < M < 0 )
   E = M - e;
else
   E = M + e;
end

%  Iterate until tolerance is met
tol = 1e-12;
diff = 1;

while ( diff > tol )
   f = M - E + e*sin(E);
   fprime = 1 - e*cos(E);
   E_new = E + f/fprime;
   diff = abs(E_new - E);
   E = E_new;
end

% %  Check quadrant
% if (M > pi & E < pi)
%   E = 2*pi - E;
% elseif (M < pi & E > pi)
%   E = 2*pi - E;
% end



