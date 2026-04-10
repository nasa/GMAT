%
%   Copyright 2002-2005, United States Government as represented by the
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.
clear all
clc
global objname conname

%-----  Set Options Structure for miNLP
Options = MiNLPOptions();

%-----  Define names of test problems
ProblemSet = {
    %    'Psaiki_Park_95b'
    'TP239'; %Fail with upper/lower bound implementation
         'TP238';
        'TP395';
        'TP394';
        'TP246';
        'TP201';
        'TP328'; % SNOPT issue
        'TP6';
        'PLR_T1_4';
        'QLR_T1_1';
        'SGR_P1_2';
        'TP6';
        'TP1';
        'TP218';
        'TP369';
        'TP242';% SNOPT issue
        'TP225'; % SNOPT issue
        'TP254'; % SNOPT issue
        'PQR_T1_6'; % SNOPT issue
        'LQR_T1_4';
        'LLR_T1_1' % SNOPT issue
        'LPR_T1_1'; % SNOPT issue
        'LPR_T1_2' %MiNLP fails
        'LPR_T1_5'; % SNOPT issue
        'SGR_P1_2';
        'PLR_T1_2'; % SNOPT issue
        'SLR_T1_1';
        'QLR_T1_2';
        'BoundsTest';
    };

%-----  Define names of test problems
% Starting this problem at x0 = [0 0] results in NaNs.
%ProblemSet = {'TP319';  };
optimizer = 'miNLP';
optimizer = 'MiNLPClass';
optimizer = 'MiNLPClass2';
useReverseCommunication = 0;
%optimizer = 'SNOPT';
%optimizer = 'fmincon';
%ProblemSet = {'BoundsTest'}
Options.MaxIter = 10000;
Options.MaxFunEvals = 10000;
%ProblemSet = {ProblemSet{12}}
%-----  Loop over problem set and solve each one
for testIdx = 1:size(ProblemSet,1);
    
    %----- Set up problem data
    
    name    = ProblemSet{testIdx};
    objname = ['OBJ_' name];
    conname = ['CON_' name];
    d       = feval(ProblemSet{testIdx});
    RunData{testIdx}.func = name;
    
    %----- Call miNLP and save data
    import MiNLPClass
    disp([ ' Problem : ' name ])
    disp(  '------------------');
    disp( ' ');
    switch optimizer
        case 'miNLP'
            [x,f,exitFlag,OutPut]      = miNLP(objname,d.x0,d.A,d.b,d.Aeq,d.beq,d.lb,d.ub,conname,Options);
            RunData{testIdx}.miNLP.iter      = OutPut.iter;
            RunData{testIdx}.miNLP.fevals    = OutPut.fevals;
        case 'MiNLPClass'
            conMode = 2;
            testProb = MinNLPOOTestProb(name,conMode);
            sqpOpt = MiNLPClass(testProb,Options);
            [x,f,exitFlag,OutPut] = sqpOpt.Optimize();
            RunData{testIdx}.miNLP.iter      = OutPut.iter;
            RunData{testIdx}.miNLP.fevals    = OutPut.fevals;
            RunData{testIdx}.miNLP.f         = f;
        case 'MiNLPClass2'
            conMode = 3;
            testProb = MinNLPOOTestProb(name,conMode);
            sqpOpt = MiNLPClass2(testProb,Options);
            
            if useReverseCommunication
                sqpOpt.PrepareToOptimize();
                status = 1;
                while status~= 0
                    [status,funTypes,decVec,isNewX,userFunPointer] = sqpOpt.CheckStatus();
                    
                    sqpOpt.RespondToData()
                end
                [x,f,exitFlag,OutPut] = sqpOpt.PrepareOutput();
            else
                [x,f,exitFlag,OutPut] = sqpOpt.Optimize();
                
            end
            RunData{testIdx}.miNLP.iter      = OutPut.iter;
            RunData{testIdx}.miNLP.fevals    = OutPut.fevals;
            RunData{testIdx}.miNLP.f         = f;
        case 'fmincon'
            fminConOptions = optimset('TolCon',Options.TolCon,'TolFun',...
                Options.TolF,'Display','iter', ...
                'DerivativeCheck','Off','GradObj','On', 'GradConstr','On',...
                'MaxIter',Options.MaxIter,'MaxFunEvals',Options.MaxFunEvals, ...
                'Algorithm','active-set','ScaleProblem','none');
            [x,f,exitFlag,OutPut]      = fmincon(objname,d.x0,d.A,d.b,d.Aeq,d.beq,d.lb,d.ub,conname,fminConOptions);
            RunData{testIdx}.miNLP.iter      = OutPut.iterations;
            RunData{testIdx}.miNLP.fevals    = OutPut.funcCount;
        case 'SNOPT'
            clear mex
            [Flow,Fupp,iGfun,jGvar] = prepSNOPT(name,d.x0,d);
            [x,f,exitFlag,iter,o1] = snopt(d.x0,d.lb,d.ub,Flow,Fupp,'SNOPTObjCon',[],[], [],iGfun,jGvar);
            RunData{testIdx}.miNLP.iter      = 1000;
            RunData{testIdx}.miNLP.fevals    = 1000;
    end
    RunData{testIdx}.miNLP.x         = x;
    RunData{testIdx}.miNLP.f         = f(1);
    RunData{testIdx}.miNLP.exitFlag  = exitFlag;
    RunData{testIdx}.xstar           = d.xstar;
    RunData{testIdx}.fstar           = d.fstar;
    
end

% =========================================================================
% =======================  Output Summary Data  ===========================
% =========================================================================
header = sprintf('\n                  Test         exit ');
disp(header);
header = sprintf(' Solver          Problem        Flag    abs(f(x)-f(x*))     norm(x* - x)    Iter    Feval');
formatstr = '%s %16s      %5.0f    %15.6g  %15s   %6.0d  %6.0d';
disp(header)
disp('=====================================================================================================')

for testIdx = 1:size(RunData,2)
    
    Solver = 'miNLP  ';
    Ferror = abs(RunData{testIdx}.miNLP.f  - RunData{testIdx}.fstar);
    if ~strcmp(RunData{testIdx}.xstar,'N/A')
        Xerror = sprintf('%15.6g', norm(RunData{testIdx}.miNLP.x  - RunData{testIdx}.xstar));
    else
        Xerror = 'N/A';
    end
    iterdata = sprintf(formatstr, Solver, RunData{testIdx}.func, RunData{testIdx}.miNLP.exitFlag,Ferror,Xerror,...
        RunData{testIdx}.miNLP.iter, RunData{testIdx}.miNLP.fevals );
    disp(iterdata)
    
end




