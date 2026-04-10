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

Options.DescentMethod     = 'SelfScaledBFGS';
Options.StepSearchMethod  = 'NocWright';
Options.FiniteDiffVector  = ones(5,1)*1e-9;
Options.DerivativeMethod  = 'Analytic';
Options.MaxIter           = 350;
Options.MaxFunEvals       = 750;
Options.TolCon            = 1e-8;
Options.TolX              = 1e-8;
Options.TolF              = 1e-8;
Options.TolGrad           = 1e-8;
Options.TolStep           = 1e-8;
Options.MaxStepLength     = 1000;
Options.QPMethod          = 'minQP';

opt                       = optimset('Display','iter','GradObj','On','GradCon','On', 'MaxIter',Options.MaxIter ,...
                                     'MaxFunEvals',Options.MaxFunEvals,'DerivativeCheck','Off','TolFun',Options.TolF,'TolCon',Options.TolF);

ProblemSet = {'TP239'; 'TP239Test'}

for i = 1:size(ProblemSet,1);
    
    %----- Set up problem data
    name    = ProblemSet{i};
    objname = ['OBJ_' name];
    conname = ['CON_' name];
    d       = feval(ProblemSet{i});
    RunData{i}.func =  name;
    
     %----- Call miNLP and save data
     if i == 2
        Options.ConstraintMode    = 'MATLAB';   
     else
        Options.ConstraintMode    = 'miNLP';
     end
     [x,f,exitFlag,OutPut]       = miNLP(objname,d.x0,d.A,d.b,d.Aeq,d.beq,d.lb,d.ub,conname,Options);
end