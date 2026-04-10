%
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

clc

%==========================================================================
%==================  Set up the Options for uncmin ========================
%==========================================================================
Options.DescentMethod     = 'BFGS';
Options.StepSearchMethod  = 'Wolfe';
Options.FiniteDiffVector  = ones(50,1)*1e-9;
Options.DerivativeMethod  = 'Analytic';
Options.MaxIter           = 550;
Options.MaxFunEvals       = 550;
Options.TolX              = 1e-215;
Options.TolF              = 1e-215;
Options.TolGrad           = 1e-7;
Options.TolStep           = 1e-215;
Options.MaxStepLength     = 1000;

opt                       = optimset('Display','iter','TolFun',Options.TolF,'TolX',Options.TolX,'GradObj','off'...
                             ,'DiffMinChange',1e-9,'DiffMaxChange',1e-9,'HessUpdate','bfgs');%,'TolFun',Options.TolF,'TolX',Options.TolX);

i = 0;
%====== New Problem
i = i + 1;
uncminTestEx{i}.func  = 'rosenbrock';
uncminTestEx{i}.X0    = [ -1.2 1  ]';
uncminTestEx{i}.Sol   = 0;
uncminTestEx{i}.Xstar = [1 1]';

%====== New Problem
i = i + 1;
uncminTestEx{i}.func  = 'paraboloid';
uncminTestEx{i}.X0    = [ -2 -2  ]';
uncminTestEx{i}.Sol   = 0;
uncminTestEx{i}.Xstar = [2 2]';

x0 = ones(50,1)*3;
[x,f,G,Converged,OutPut] = uncTrust('NandW4p3',x0,Options);





opt                       = optimset('Display','iter','TolFun',Options.TolF,'TolX',Options.TolX,'GradObj','on'...
                             ,'DiffMinChange',1e-9,'DiffMaxChange',1e-9,'HessUpdate','bfgs');%,'TolFun',Options.TolF,'TolX',Options.TolX);
fminunc('NandW4p3',x0,opt);