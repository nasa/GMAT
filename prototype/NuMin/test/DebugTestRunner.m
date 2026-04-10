testProb = Psaiki_Park_95()
%testProb = InfeasibleLinearizationAtGuess();
%testProb = Biegler_Cuthrell_85();
%testProb = GPR_T1_1();
%testProb = InfeasibleConstraints_1();
%testProb = PGR_P1_3();
%testProb = GPR_P1_2();
Options = MiNLPOptions();
Options.TolF = 1e-23;
Options.TolX = 1e-23;
Options.MaxIter = 1000;
Options.MaxFunEvals = 5000;
sqpOpt = MiNLPClass2(testProb,Options)
[x,f,exitFlag,OutPut] = sqpOpt.Optimize();

