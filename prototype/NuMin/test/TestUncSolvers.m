%
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

Options.DescentMethod     = 'BFGS';
Options.StepSearchMethod  = 'Wolfe';
Options.FiniteDiffVector  = ones(5,1)*1e-8;
Options.DerivativeMethod  = 'Analytic';
Options.MaxIter           = 100;
Options.MaxFunEvals       = 200;
Options.TolCon            = 1e-12;
Options.TolX              = 1e-12;
Options.TolF              = 1e-12;
Options.TolGrad           = 1e-8;
Options.TolStep           = 1e-12;
Options.MaxStepLength     = 1000;
Options.QPMethod          = 'minQP';
opt                       = optimset('Display','iter','GradObj','On','GradCon','On',...
                                     'DerivativeCheck','On','TolFun',Options.TolF,'TolCon',Options.TolF);

ProblemSet = {'TP240';'TP201';'TP210'};

for i = 1:size(ProblemSet,1);
    
    %----- Set up problem data
    name    = ProblemSet{i};
    objname = ['OBJ_' name];
    conname = ['CON_' name];
    d       = feval(ProblemSet{i});
    RunData{i}.func  = objname;
    RunData{i}.xstar = d.xstar;
    RunData{i}.fstar = d.fstar;
    
    %----- Call miNLP and save data
    [x, f, g, exitFlag, OutPut] = uncTrust(objname,d.x0,Options);
    RunData{i}.uncTrust.x         = x;
    RunData{i}.uncTrust.f         = f;
    RunData{i}.uncTrust.exitFlag  = exitFlag;
    RunData{i}.uncTrust.iter      = OutPut.iter;
    RunData{i}.uncTrust.fevals    = OutPut.numfEval;

    %----- Call fmincon and save data
    [x, f, g, exitFlag, OutPut] = uncMin(objname,d.x0,Options);
    RunData{i}.uncMin.x         = x;
    RunData{i}.uncMin.f         = f;
    RunData{i}.uncMin.exitFlag  = exitFlag;
    RunData{i}.uncMin.iter      = OutPut.iter;
    RunData{i}.uncMin.fevals    = OutPut.numfEval;
    
        %----- Call fmincon and save data
    [x, f, exitFlag, OutPut, g] = fminunc(objname,d.x0,opt);
    RunData{i}.fminunc.x         = x;
    RunData{i}.fminunc.f         = f;
    RunData{i}.fminunc.exitFlag  = exitFlag;
    RunData{i}.fminunc.iter      = OutPut.iterations;
    RunData{i}.fminunc.fevals    = OutPut.funcCount;
    
end

% =========================================================================
% =======================  Output Summary Data  ===========================
% =========================================================================
header = sprintf('\n                                exit ');
disp(header);
header = sprintf(' Solver           Func          Flag    abs(f(x)-f(x*))   norm(x* - x)      Iter    Feval');
formatstr = '%s %16s      %5.0f    %15.6g  %15.6g   %6.0d  %6.0d';
disp(header)
disp('=====================================================================================================')

for i = 1:size(RunData,2)

    for j = 1:3

        if j == 1;
            Solver = 'uncTrust';
            Ferror = abs(RunData{i}.uncTrust.f  - RunData{i}.fstar);
            Xerror = norm(RunData{i}.uncTrust.x  - RunData{i}.xstar);
            iterdata = sprintf(formatstr, Solver, RunData{i}.func, RunData{i}.uncTrust.exitFlag,Ferror,Xerror,...
                                RunData{i}.uncTrust.iter, RunData{i}.uncTrust.fevals );
        elseif j ==2
            Solver = 'uncMin  ';
            Ferror = abs(RunData{i}.uncMin.f  - RunData{i}.fstar);
            Xerror = norm(RunData{i}.uncMin.x  - RunData{i}.xstar);
            iterdata = sprintf(formatstr, Solver, RunData{i}.func, RunData{i}.uncMin.exitFlag,Ferror,Xerror,...
                RunData{i}.uncMin.iter, RunData{i}.uncMin.fevals );
            
        else 
            Solver = 'fminunc ';
            Ferror = abs(RunData{i}.fminunc.f  - RunData{i}.fstar);
            Xerror = norm(RunData{i}.fminunc.x  - RunData{i}.xstar);
            iterdata = sprintf(formatstr, Solver, RunData{i}.func, RunData{i}.fminunc.exitFlag,Ferror,Xerror,...
                RunData{i}.fminunc.iter, RunData{i}.fminunc.fevals );
        end

        disp(iterdata);
    end
    disp('=====================================================================================================')
end

