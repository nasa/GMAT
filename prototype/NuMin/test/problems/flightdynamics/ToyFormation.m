function x = ToyFormation

x1 = [7000 0 3000 1 7 1]';
x2 = [7005 5 3005 1.0001 7.0001 1.0001]';
x0 = [  6999.99929651745
       0.00580107657072211
          3000.00022085242
          1.00084054426947
          6.99941336710433
          1.00029115393441
          7004.93703871983
          4.99416141673004
          3004.97246942915
         0.998249150799259
           6.9936182727495
         0.998798623077224];

global ScaleVector mov c

c = 0;

fig=figure(1);
%set(fig,'DoubleBuffer','on');
%set(gca,'NextPlot','replace','Visible','on')
%mov = avifile('example.avi')
close all
%  Define the vector of scale factors and nondimensionalize x0
PosScale    = .1*ones(3,1); VelScale    = 1e-5*ones(3,1);
ScaleVector = [PosScale ; VelScale; PosScale; VelScale];
x0 = inv(diag(ScaleVector))*x0;

%  Define optimzer settings and run the optimizer
% options = optimset('TolCon',1e-5,'TolFun',1e-5,'Display','iter', ...
%            'DerivativeCheck','Off','GradObj','On', 'GradConstr','On','Algorithm','sqp');
% [x,fval,exitflag] = fmincon(@CostFun,x0,[],[],[],[],[],[],@ConstraintFun,options);

Options.DescentMethod     = 'SelfScaledBFGS';
Options.StepSearchMethod  = 'NocWright';
Options.FiniteDiffVector  = ones(5,1)*1e-9;
Options.DerivativeMethod  = 'Analytic';
Options.MaxIter           = 550;
Options.MaxFunEvals       = 500;
Options.TolCon            = 1e-5;
Options.TolX              = 1e-5;
Options.TolF              = 1e-5;
Options.TolGrad           = 1e-5;
Options.TolStep           = 1e-5;
Options.MaxStepLength     = 1000;
Options.QPMethod          = 'minQP';
Options.ConstraintMode    = 'miNLP';

sqpOpt = MiNLPClass(@CostFun,x0,[],[],[],[],...
    [],[],@ConstraintFun,Options);

[x,f,exitFlag,OutPut] = sqpOpt.Optimize()

% [x,fval,exitflag] = MiNLP(@CostFun,x0,[],[],[],[],[],[],@ConstraintFun,Options);

%  Unscale the solution
x = diag(ScaleVector)*x;
 mov = close(mov);

%==========================================================================
%----- Define the cost function
%==========================================================================
function [J,dJdx] = CostFun(x)

global ScaleVector mov c

%  Unscale the input
c = c + 1;
x = diag(ScaleVector)*x;

%  Create the vector of times
timeVec  = 60:100:7200;
numTimes = size(timeVec,2);
L        = 10;

% Propagate the orbits and generate ephem
[rv1mat,vv1mat,Phi1mat] = PropOrbitandSTM(x(1:6,1),timeVec);
[rv2mat,vv2mat,Phi2mat] = PropOrbitandSTM(x(7:12,1),timeVec);

%  Loop over the times to calcuate the cost
J = 0; 
dJdr10 = zeros(1,3); dJdv10 = zeros(1,3);
dJdr20 = zeros(1,3); dJdv20 = zeros(1,3);
for k = 1:numTimes
    
   % Calculate the side vector and side length at current time
   svk   = (rv2mat(k,:) - rv1mat(k,:))';
   sk    = sqrt(svk'*svk);
   svkhat = svk/sk;
   s(k) = sk;                    % For plotting purposes only
   
   %  Calculate the cost function contribution
   J   = J + (sk - 10)^2;
   
   %  Calculate the constraint function contributions
   Phi1 = reshape(Phi1mat(k,:),6,6); A1 = Phi1(1:3,1:3); B1 = Phi1(1:3,4:6);
   Phi2 = reshape(Phi2mat(k,:),6,6); A2 = Phi2(1:3,1:3); B2 = Phi2(1:3,4:6);
   dJdr10 = dJdr10 - 2*(sk - L)*svkhat'*A1;
   dJdv10 = dJdv10 - 2*(sk - L)*svkhat'*B1;
   dJdr20 = dJdr20 + 2*(sk - L)*svkhat'*A2;
   dJdv20 = dJdv20 + 2*(sk - L)*svkhat'*B2;
   
end

%  Assemble the deriviative and nondimensionalize
dJdx = [dJdr10';dJdv10';dJdr20';dJdv20'];
for i = 1:size(x,1)
    dJdx(i,:) = dJdx(i,:)*ScaleVector(i,1);
end
J    = J/numTimes;  dJdx = dJdx/numTimes;  

%  Draw the current iteration




      
       
figure(1);
plot(timeVec,s)
axis([0 7500 0 15])
text(1500,2.5,['Func. Call = ' num2str(c)]);
xlabel('Time, seconds'); ylabel('Side Length, km')
title('Side Length vs. Time for One Orbit Period')
grid on; drawnow;
%       	set(h,'EraseMode','xor');
%F = getframe(gcf);
%mov = addframe(mov,F);

%==========================================================================
%----- Define the constraint function
%==========================================================================
function [gi,ge,dgidx,dgedx] = ConstraintFun(x)

global ScaleVector
mu = 398600.4415;

%  Unscale the input
x = diag(ScaleVector)*x;

%  Extract useful info from the input vector
rv1 = x(1:3,1);  vv1 = x(4:6,1);
r1  = norm(rv1); v1  = norm(vv1);
a1  = -mu/(vv1'*vv1-2*mu/sqrt(rv1'*rv1));
rv2 = x(7:9,1);  vv2 = x(10:12,1);
r2  = norm(rv2); v2  = norm(vv2);
a2  = -mu/(vv2'*vv2-2*mu/sqrt(rv2'*rv2));

%  Calculate the constraint values
ge(1,1) = a1 - 7425;
ge(2,1) = a2 - 7425;

%  Calculate the Jacobian
dgedx(1:6,1) =  [(2 * ( a1 / r1 )^2*rv1'/r1)';...
                (2 * ( v1*a1^2 / mu )*vv1'/v1)'];
dgedx(7:12,2) =  [(2 * ( a2 / r2 )^2*rv2'/r2)';...
                (2 * ( v2*a2^2 / mu )*vv2'/v2)'];            
dgedx(7:12,1) = zeros(6,1);
dgedx(1:6,2) = zeros(6,1);
 
%  Nondimensionalize the Jacobian
for i = 1:size(x,1)
    dgedx(i,:) = dgedx(i,:)*ScaleVector(i,1);
end
fac   = 1000;
ge    = ge*fac;
dgedx = dgedx*fac;

% Tell matlab there are no inequality constraints
gi    = [];
dgidx = [];

%==========================================================================
%========== The two body orbit propagator
%==========================================================================
function [rvmat,vvmat,Phimat] = PropOrbitandSTM(xv0,timeVec)

X0 = [xv0;reshape(eye(6),36,1)];
options = odeset('AbsTol',1e-9,'RelTol',1e-9);
[thist, Xhist] = ode45(@OrbitDot, timeVec, X0, options);

rvmat = Xhist(:,1:3);
vvmat = Xhist(:,4:6);
Phimat = Xhist(:,7:42);

%==========================================================================
%========== The orbital and variational equations
%==========================================================================
function [xdot] = OrbitDot(t,X)

%  constants and definitions
mu          = 398600.4415;
I3          = eye(3);
rv          = X(1:3,1);
vv          = X(4:6,1);
r           = norm(rv);
Phi         = reshape(X(7:42,1),6,6);

% orbit equations of motion
xdot(1:3,1) = vv;
xdot(4:6,1) = -mu/r^3*rv;

% variational equations 
A           = zeros(6,6);
A(1:3,4:6)  = I3;
A(4:6,1:3)  = -mu/r^3*I3 + 3*mu*rv*rv'/r^5;
Phidot      = A*Phi;
xdot(7:42)  = reshape(Phidot,36,1);
