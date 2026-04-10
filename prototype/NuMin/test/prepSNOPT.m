function [Flow,Fupp,iGfun,jGvar] = prepSNOPT(objname,x,d)

%
%   Copyright 2002-2005, United States Government as represented by the
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

%-----  Call the constraint function to determine constraint dimensions
conname                     = ['CON_' objname];
try
    [ci,ce,Ji,Je,ciGfun,cjGvar] = feval(conname,x);
catch
    [ci,ce,Ji,Je] = feval(conname,x);
    ciGfun = [];
    cjGvar = [];
end

%-----  Call the objective function
objname       = ['OBJ_' objname];
[f,g, fiGfun, fjGvar] = feval(objname,x);
fiGfun = [];
fjGvar = [];

%-----  Detemine constraint dimensions
mNLI = size(ci,1);              %  Number of nonlin. ineq. constraints
mNLE = size(ce,1);              %  Number of nonlin. eq. constraints
mNL  = mNLI + mNLE;             %

%-----  Set upper and lower bounds on functions
Flow  = [-inf;zeros(mNLE+mNLI,1)];
Fupp  = [ inf;zeros(mNLE,1);inf*ones(mNLI,1)];

%-----  Seup up vector of nonzero elements of gradient and Jacobian
iGfun = [fiGfun;ciGfun+1];
jGvar = [fjGvar;cjGvar];


snseti('Derivative Option',0);
% Set the derivative verification level
snseti('Verify Level',-1);
% Set name of SNOPT print file
snprint('snoptmain.out');
% Print CPU times at bottom of SNOPT print fileR
snseti('Timing level',3);
% Echo SNOPT Output to MATLAB Command Window
snscreen on;
%  Set the SNOPT internal scaling option
snseti('Scale Option', 1)
%  Set tolerance on optimality
snset('Major optimality tolerance 1e-10')
%  Set tolerance on feasibility
snset('Major feasibility tolerance 1e-10')
%  Set number of iterations
snseti('Major iterations limit',3000);
%  Set number of iterations
snseti('Iterations limit',300000);




