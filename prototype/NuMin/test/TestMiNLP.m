%
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

global objname conname

Options.DescentMethod     = 'DampedBFGS';
Options.StepSearchMethod  = 'NocWright';
Options.FiniteDiffVector  = ones(5,1)*1e-9;
Options.DerivativeMethod  = 'Analytic';
Options.MaxIter           = 5550;
Options.MaxFunEvals       = 5500;
Options.TolCon            = 1e-8;
Options.TolX              = 1e-8;
Options.TolF              = 1e-8;
Options.TolGrad           = 1e-8;
Options.TolStep           = 1e-8;
Options.MaxStepLength     = 1000;
Options.QPMethod          = 'minQP';
Options.ConstraintMode    = 'MiNLP';
opt                       = optimoptions('fmincon','Display','iter','GradObj','On','GradCon','On', 'MaxIter',Options.MaxIter ,...
                                     'MaxFunEvals',Options.MaxFunEvals,'DerivativeCheck','Off','TolFun',Options.TolF,'TolCon',Options.TolF,'Algorithm','active-set');
                                 


ProblemSet = {'TP_320';'Sample1' ; 'TP6'; 'PQR_P1_4'; 'PLR_T1_4'; 'TP220';'QLR_T1_1'; ...
              'SGR_P1_2';'LQR_T1_4'; 'TP1';  'TP218';'TP369';  ...
              'TP242'; 'TP225';'TP254'; 'PQR_T1_6' ; 'LQR_T1_4';...
              'LPR_T1_1'; 'LPR_T1_5';  'SGR_P1_2'; 'PLR_T1_2';...
              'SLR_T1_1'; 'QLR_T1_2';'LPR_T1_2'};

% snset('Defaults');
% snprintfile('snoptmain2.out');
% snsummary  ('snoptmain2.sum');
% snspec     ('snoptmain2.spc');
% snseti     ('Major Iteration limit', Options.MaxIter );
% snset('Minimize');

for i = 1:size(ProblemSet,1);
    
    %----- Set up problem data
    name    = ProblemSet{i};
    objname = ['OBJ_' name];
    conname = ['CON_' name];
    d       = feval(ProblemSet{i});
    RunData{i}.func = objname;
    
    %----- Call miNLP and save data
    [x,f,exitFlag,OutPut]      = miNLP(objname,d.x0,d.A,d.b,d.Aeq,d.beq,d.lb,d.ub,conname,Options);
    RunData{i}.miNLP.x         = x;
    RunData{i}.miNLP.f         = f;
    RunData{i}.miNLP.exitFlag  = exitFlag;
    RunData{i}.miNLP.iter      = OutPut.iter;
    RunData{i}.miNLP.fevals    = OutPut.fevals;
    RunData{i}.xstar           = d.xstar;
    RunData{i}.fstar           = d.fstar;
    
    %----- Call fmincon and save data
    [x,f,exitFlag,OutPut]        = fmincon(objname,d.x0,-d.A,-d.b,d.Aeq,d.beq,d.lb,d.ub,'Fminconstraint',opt,conname);
    RunData{i}.fmincon.x         = x;
    RunData{i}.fmincon.f         = f;
    RunData{i}.fmincon.exitFlag  = exitFlag;
    RunData{i}.fmincon.iter      = OutPut.iterations;
    RunData{i}.fmincon.fevals    = OutPut.funcCount;
    
%     [Flow,Fupp,iGfun,jGvar] = prepSNOPT(name,x,d);
%     [x,F,inform] = snopt(d.x0,d.lb,d.ub,Flow,Fupp,'SNOPTdummy',[],[], [],iGfun,jGvar);

    
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

    for j = 1:2

        if j == 1;
            Solver = 'miNLP  ';
            Ferror = abs(RunData{i}.miNLP.f  - RunData{i}.fstar);
            Xerror = norm(RunData{i}.miNLP.x  - RunData{i}.xstar);
            iterdata = sprintf(formatstr, Solver, RunData{i}.func, RunData{i}.miNLP.exitFlag,Ferror,Xerror,...
                                RunData{i}.miNLP.iter, RunData{i}.miNLP.fevals );
            funcEvals(i) = RunData{i}.miNLP.fevals;
        else
            Solver = 'fmincon';
            Ferror = abs(RunData{i}.fmincon.f  - RunData{i}.fstar);
            Xerror = norm(RunData{i}.fmincon.x  - RunData{i}.xstar);
            iterdata = sprintf(formatstr, Solver, RunData{i}.func, RunData{i}.fmincon.exitFlag,Ferror,Xerror,...
                RunData{i}.fmincon.iter, RunData{i}.fmincon.fevals );
        end

        disp(iterdata);
    end
    disp('=====================================================================================================')
end

return

%----- LPR_T1_2
x0 = [.1 .1]';
lb = [1 0 ]'; ub = [];
[x,f,Converged,OutPut]   = miNLP('OBJ_LPR_T1_2',x0,[],[],[],[],lb,ub,'CON_LPR_T1_2',Options);
[x,f,Converged,OutPut]   = fmincon('OBJ_LPR_T1_2',x0,[],[],[],[],lb,ub,'FMINCON_LPR_T1_2',opt);





