%clear all
%%  Define options for miNLP optimizer and fmincon
Options.DescentMethod     = 'SelfScaledBFGS';
Options.StepSearchMethod  = 'NocWright';
Options.FiniteDiffVector  = ones(5,1)*1e-9;
Options.DerivativeMethod  = 'Analytic';
Options.MaxIter           = 1350;
Options.MaxFunEvals       = 450;
Options.TolCon            = 1e-8;
Options.TolX              = 1e-8;
Options.TolF              = 1e-8;
Options.TolGrad           = 1e-3;
Options.TolStep           = 1e-8;
Options.MaxStepLength     = 1000;
Options.QPMethod          = 'minQP';
Options.ConstraintMode    = 'miNLP';
runMiNLP                  = true()
runFmincon                = true();
runSNOPT                  = false();

if runFmincon
    opt                       = optimset('Display','iter','GradObj','On','GradCon','On', 'MaxIter',Options.MaxIter ,...
        'MaxFunEvals',Options.MaxFunEvals,'DerivativeCheck','Off','TolFun',Options.TolF,'TolCon',Options.TolF,'Algorithm','active-set');
end

if runSNOPT
    
end

%%  Define the test problem names.  These are passing
ProblemSet = {'hs077';'hs065'; 'hs066';'TP328'; 'TP238'; 'TP369'; 'TP239';...
    'TP395' ;'TP394' ; 'PLR_T1_4';'QLR_T1_1'; 'LQR_T1_4'; ...
    'TP6';'TP1';  'TP218'; 'TP242'; 'TP225';'TP254';...
    'PQR_T1_6'; 'LLR_T1_1'; 'LPR_T1_1'; 'LPR_T1_5'; 'SGR_P1_2'; ...
    'PLR_T1_2';'SLR_T1_1'; 'QLR_T1_2' ; 'PQR_P1_4'; 'TP220'; 'LPR_T1_2'; };

%%  Define the test problem names.  These are failing do to constraint
%  degeneracy problems
%  TP319 is a problem when starting from [0 0 ] due to invalid constraint
%  linearization.
%ProblemSet = {'TP220'}%;'TP319','CEC_NandE_01'};%,},

%ProblemSet = {'TP319'};

%% Loop over the test problems, running each one in both optimizers
for i = 1:size(ProblemSet,1);
    
    %----- Set up problem data
    name    = ProblemSet{i};
    objname = ['OBJ_' name];
    conname = ['CON_' name];
    d       = feval(ProblemSet{i});
    RunData{i}.func =  name;
    disp(name)
    
    %----- Call miNLP and save data
    if runMiNLP
        [x,f,exitFlag,OutPut]       = miNLP(objname,d.x0,d.A,d.b,d.Aeq,d.beq,d.lb,d.ub,conname,Options);
        RunData{i}.miNLP.x         = x;
        RunData{i}.miNLP.f         = f;
        RunData{i}.miNLP.exitFlag  = exitFlag;
        RunData{i}.miNLP.iter      = OutPut.iter;
        RunData{i}.miNLP.fevals    = OutPut.fevals;
        RunData{i}.xstar           = d.xstar;
        RunData{i}.fstar           = d.fstar;
    end
    
    %----- Call fmincon and save data
    if runFmincon
        [x,f,exitFlag,OutPut]        = fmincon(objname,d.x0,-d.A,-d.b,d.Aeq,d.beq,d.lb,d.ub,'Fminconstraint',opt,conname);
        RunData{i}.fmincon.x         = x;
        RunData{i}.fmincon.f         = f;
        RunData{i}.fmincon.exitFlag  = exitFlag;
        RunData{i}.fmincon.iter      = OutPut.iterations;
        RunData{i}.fmincon.fevals    = OutPut.funcCount;
    end
    
    if runSNOPT
        
        global igrid iGfun jGvar objname conname
        
        snseti('Derivative Option',0);
        % Set the derivative verification level
        snseti('Verify Level',-1);
        % Set name of SNOPT print file
        snprint('snoptmain.out');
        % Print CPU times at bottom of SNOPT print file
        snseti('Timing level',3);
        % Echo SNOPT Output to MATLAB Command Window
        snscreen on;
        %  Set the SNOPT internal scaling option
        %snseti('Scale Option', 1)
        %  Set tolerance on optimality
        snset('Major optimality tolerance 1e-3')
        %  Set tolerance on feasibility
        snset('Major feasibility tolerance 1e-6')
                        
        % Set initial guess on basis and Lagrange multipliers to zero
        [Flow,Fupp,iGfun,jGvar] = prepSNOPT(name,d.x0,d);
        zmul = zeros(size(d.x0));
        zstate = zmul;
        Fmul = zeros(size(Flow));
        Fstate = Fmul;
        ObjAdd = 0;
        ObjRow = 1;

        % Assume for simplicity that all constraints are nonlinear
        AA      = [];
        iAfun   = [];
        jAvar   = [];
        userfun = 'SNOPTFunctionWrapper';
        
        %  Compute sparsity in form SNOPT needs
        %[iGfun,jGvar] = find(sparsityPattern);
        
        
        [x,F,xmul,Fmul,info,xstate,Fstate,ns,...
            ninf,sinf,mincw,miniw,minrw]...
            = snsolve(d.x0,d.lb,d.ub,zmul,zstate,...
            Flow,Fupp,Fmul,Fstate,...
            ObjAdd,ObjRow,AA,iAfun,jAvar,iGfun,jGvar,'SNOPTObjCon');
        
        
        
    end
    
end

%%  Process the results and create report with comparison
header = sprintf('\n                                exit ');
disp(header);
header = sprintf(' Solver           Func          Flag    abs(f(x)-f(x*))   norm(x* - x)      Iter    Feval');
formatstr = '%s %16s      %5.0f    %15.6g   %15.6g   %6.0d  %6.0d';
formatstr2 = '%s %16s      %5.0f    %15.6g  %s   %6.0d  %6.0d';
disp(header)
disp('=====================================================================================================')
minNLPcount = 0;
fminconFail = 0;
fminconCount = 0;
minNLPfail = 0;
for i = 1:size(RunData,2)
    
    if runFmincon
        numSolvers = 2;
    else
        numSolvers = 1;
    end
    for j = 1:numSolvers
        
        if j == 1;
            if runMiNLP
            Solver = 'miNLP  ';
            Ferror = abs(RunData{i}.miNLP.f  - RunData{i}.fstar);
            if isnan(RunData{i}.xstar(1))
                type = 2;
            else
                Xerror = norm(RunData{i}.miNLP.x  - RunData{i}.xstar);
                type = 1;
            end
            if type == 1
                iterdata = sprintf(formatstr, Solver, RunData{i}.func, RunData{i}.miNLP.exitFlag,Ferror,Xerror,...
                    RunData{i}.miNLP.iter, RunData{i}.miNLP.fevals );
            else
                
                iterdata = sprintf(formatstr2, Solver, RunData{i}.func, RunData{i}.miNLP.exitFlag,Ferror,'         N/A    ',...
                    RunData{i}.miNLP.iter, RunData{i}.miNLP.fevals );
            end
            end
        else
            Solver = 'fmincon';
            Ferror = abs(RunData{i}.fmincon.f  - RunData{i}.fstar);
            if isnan(RunData{i}.xstar(1));
                type = 2;
            else
                Xerror = norm(RunData{i}.fmincon.x  - RunData{i}.xstar);
                type = 1;
            end
            if type == 1
                iterdata = sprintf(formatstr, Solver, RunData{i}.func, RunData{i}.fmincon.exitFlag,Ferror,Xerror,...
                    RunData{i}.fmincon.iter, RunData{i}.fmincon.fevals );
            else
                iterdata = sprintf(formatstr2, Solver, RunData{i}.func, RunData{i}.fmincon.exitFlag,Ferror,'         N/A    ',...
                    RunData{i}.fmincon.iter, RunData{i}.fmincon.fevals );
            end
        end
        disp(iterdata);
    end
    ;
    
    
    %  Check to see if the optimizers found a solution
    if  RunData{i}.miNLP.exitFlag > 0;
        minNLPStatus = 1;
    else
        minNLPStatus = 0;
        minNLPfail = minNLPfail + 1;
    end
    if runFmincon
        if  RunData{i}.fmincon.exitFlag > 0;
            fminconStatus = 1;
        else
            fminconStatus = 0 ;
            fminconFail = fminconFail + 1;
        end
        
        
        %  Check performance of fastest optimizer.
        if fminconStatus && minNLPStatus   % Check that both converged
            
            %  Both converged so see who was faster
            if RunData{i}.miNLP.fevals < RunData{i}.fmincon.fevals
                minNLPcount = minNLPcount + 1;
            else
                fminconCount = fminconCount + 1;
            end
            
        else
            
            % miNLP converged and fmincon failed
            if minNLPStatus && ~fminconStatus
                minNLPcount = minNLPcount + 1;
            end
            % fmincon converged and miNLP failed
            if fminconStatus && ~minNLPStatus
                fminconCount = fminconCount + 1;
            end
            
        end
    end
    
    
    
    disp('=====================================================================================================')
end

disp(['There were ' num2str( size(ProblemSet,1) ) ' test cases run '])
disp(['fmincon was fastest for  ' , num2str(fminconCount), ' cases']);
disp(['minNLP was fastest for  ' , num2str(minNLPcount), ' cases']);
disp(['fmincon failed for  ' , num2str(fminconFail), ' cases']);
disp(['minNLP failed for  ' , num2str(minNLPfail), ' cases']);






