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
Options.FiniteDiffVector  = ones(5,1)*1e-9;
Options.DerivativeMethod  = 'Analytic';
Options.MaxIter           = 150;
Options.MaxFunEvals       = 550;
Options.TolX              = 1e-10;
Options.TolF              = 1e-10;
Options.TolGrad           = 1e-10;
Options.TolStep           = 1e-10;
Options.MaxStepLength     = 1000;
opt                       = optimset('Display','iter','TolFun',Options.TolF,'TolX',Options.TolX,'GradObj','on'...
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

%====== New Problem
i = i + 1;
uncminTestEx{i}.func  = 'Schaum11p3_n2';
uncminTestEx{i}.X0    = sqrt(3*pi/2)*[1 1]'*1.1;
uncminTestEx{i}.Sol   = -2;
uncminTestEx{i}.Xstar = sqrt(3*pi/2)*[1 1]';

%====== New Problem
i = i + 1;
uncminTestEx{i}.func  = 'Powell';
uncminTestEx{i}.X0    = [3 -1 0 1]';
uncminTestEx{i}.Sol   = 0;
uncminTestEx{i}.Xstar = [ 0 0 0 0 ]';

%====== New Problem
i = i + 1;
uncminTestEx{i}.func  = 'HelicalValley';
uncminTestEx{i}.X0    = [-1 0 0]';
uncminTestEx{i}.Sol   = 0;
uncminTestEx{i}.Xstar = [ 1 0 0 ]';

%====== New Problem
i = i + 1;
uncminTestEx{i}.func  = 'NandW4p3';
uncminTestEx{i}.X0    = 2*ones(50,1);
uncminTestEx{i}.Sol   = 0 ;
uncminTestEx{i}.Xstar = ones(50,1);
% 
%====== New Problem
i = i + 1;
uncminTestEx{i}.func  = 'quartic';
uncminTestEx{i}.X0    = 3;
uncminTestEx{i}.Sol   = NaN;
uncminTestEx{i}.Xstar = NaN;


RunData{size(uncminTestEx,2)} = {};

for i = 1:size(uncminTestEx,2)

    %  Run uncmin on this problem
    [x, f, g, exitFlag, OutPut] = uncTrust(uncminTestEx{i}.func,uncminTestEx{i}.X0, Options);
    
    RunData{i}.uncmin.x = x;
    RunData{i}.uncmin.f = f;
    RunData{i}.uncmin.exitFlag = exitFlag;
    RunData{i}.uncmin.OutPut = OutPut;
    
    %  Run fminunc
    [x, f, exitFlag, OutPut, g] = fminunc(uncminTestEx{i}.func,uncminTestEx{i}.X0, opt);
    
    RunData{i}.fminunc.x = x;
    RunData{i}.fminunc.f = f;
    RunData{i}.fminunc.exitFlag = exitFlag;
    RunData{i}.fminunc.OutPut = OutPut;

    
end
    header = sprintf('\n                                  exit ');
    disp(header);
    header = sprintf(' Solver           Func            Flag        f(x*)        norm(x* - x)     Iter    Feval');
    formatstr = '%s %16s      %5.0f    %15.6g  %15.6g   %6.0d  %6.0d';
    disp(header)
    disp('------------------------------------------------------------------------------------------------------')
for i = 1:size(RunData,2)

   for j = 1:2

    if j == 1;
        Solver = 'uncmin ';
        Ferror = abs(RunData{i}.uncmin.f  - uncminTestEx{i}.Sol);
        Xerror = norm(RunData{i}.uncmin.x  - uncminTestEx{i}.Xstar);
        iterdata = sprintf(formatstr, Solver, uncminTestEx{i}.func, RunData{i}.uncmin.exitFlag,Ferror,Xerror,...
                                RunData{i}.uncmin.OutPut.iter, RunData{i}.uncmin.OutPut.numfEval );
    else
        Solver = 'fminunc';
        Ferror = abs(RunData{i}.fminunc.f  - uncminTestEx{i}.Sol);
        Xerror = norm(RunData{i}.fminunc.x  - uncminTestEx{i}.Xstar);
        iterdata = sprintf(formatstr, Solver, uncminTestEx{i}.func, RunData{i}.fminunc.exitFlag,Ferror,Xerror,...
                                RunData{i}.fminunc.OutPut.iterations, RunData{i}.fminunc.OutPut.funcCount );
    end

    disp(iterdata);
   end
    disp('=====================================================================================================')
end








