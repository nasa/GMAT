function [xdot,Amat] = deriv_NSG(t,x,ForceModel)

global SolarSystem jd_ref
persistent term1 term2

%  Determine if the STM is being propagated
if size(x,1) == 6
    PropSTM = 0;
elseif size(x,1) == 42
    PropSTM = 1;
end

%  Useful terms for later
rv = x(1:3,1);
vv = x(4:6,1);
rmag = norm(rv);
vmag = norm(vv);
x  = rv(1,1);
y  = rv(2,1);
z  = rv(3,1);
s  = x/rmag;
t  = y/rmag;
u  = z/rmag;
eye3   = eye(3);
zero3  = zeros(3);

%  Loop over degree and order
mu = 398600.4415;
ForceModel.Order  = 4;
ForceModel.Degree = 4;
Energy = vmag^2/2 - mu/rmag;
a      = -mu/2/Energy;
rhop   = a/rmag;
rho(1) = mu/rmag;

%  Cacluate matrix terms that you can calculate once and use over again
if size(term1,1) < ForceModel.Degree | size(term1,2) < ForceModel.Order
    for n = 0:ForceModel.Degree
        N = n + 1;
        for m = 0:n
            M = m + 1;
            if m < n
                AOffDiagTerm1(N,M) = sqrt( (2*n + 1)*(2*n-1)/(n-m)/(n+m) );
                AOffDiagTerm2(M,M) = sqrt( (2*n + 1)*(n - m -1)*(n + m -1)/(2*n-3)/(n+m)/(n-m) );
            else
                if n > 0
                    ADiagTerm(N,N) = sqrt((2*n+1)/(2*n));
                end
            end
            c(N,M+1)   = sqrt((n-m)*(n+m+1));
            c(N+1,M+1) = sqrt((n+m+2)*(n+m+1)/(2*n+3)/(2*n+2));
        end %  for m = 1:ForceModel.Order
    end % for n = 2:ForceModel.Degree
end


%  Loop over degree and order
cosphi = sqrt(x^2 + y^2)/rmag;
Abar(1,1) = sqrt(3)*cosphi;
rho(1) = rhop*mu/rmag;
%  The following matrices must be indexed with N and M (instead of n and m)
%      AOffDiagTerm1, AOffDiagTerm2, c
for n = 2:ForceModel.Degree
    
    N = n + 1;
    rho(n) = rhop*rho(n-1);
    
    for m = 0:ForceModel.Order
        
        M = m + 1;
        if M <= 2
            re(1) = 1;
            re(2) = s;
            im(1) = 0;
            im(2) = t;
        else
            re(M) = s*re(m-1) - t*im(m-1);
            im(M) = s*im(m-1) + t*re(m-1);
        end
       

        NN(N,M) = sqrt( factorial(n-m)*factorial(2*n + 1)/factorial(n+m)  );
        C(N,M) = rand;
        S(N,M) = rand;
        Cbar(N,M) = C(N,M)/NN(N,M);
        Sbar(N,M) = S(N,M)/NN(N,M);
        
        if M == N
        end

        A(N,N) = cosphi*sqrt((2*n+1)/2*n)*Abar(n-1,n-1);
        Dbar(N,M) = Cbar(N,M)*re(M) + Sbar(N,M)*im(M);
        Abar(N,M) = u*term1*Abar(N-1,M) - term2*Abar(N-2,M);
        
    end %  for m = 1:ForceModel.Order
    
end % for n = 2:ForceModel.Degree

%  Calculate the acceleration
P  = ForceModel.SolarRadiationPressure.Flux/3e8*...
    (norm(sv)/ForceModel.SolarRadiationPressure.Nominal_Sun)^2;
Cr = 1.7;
As = 1*1e-3;
m = 1000;
xdot(1:3,1) = [0 0 0]';
xdot(4:6,1) = -P*Cr*As/m*svhat;

%  Calculate the STM Terms
Amat = zeros(6,6);
if PropSTM
    Amat(4:6,1:3)  = P*Cr*As/m*(1/s3*eye3 - 3*sv*sv'/s5);
end


