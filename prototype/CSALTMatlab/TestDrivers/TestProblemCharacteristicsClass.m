config = collutils.ProblemCharacteristics;



%% Test get/set for optimal control variable data

numStateVars = 6;
numControlVars = 7;
numStaticVars = 8;
numIntegralVars = 9;

config.SetNumStateVars(numStateVars)
error = config.GetNumStateVars() - numStateVars
config.GetHasStateVars()

config.SetNumControlVars(numControlVars)
error = config.GetNumControlVars() - numControlVars
config.GetHasControlVars()

config.SetNumStaticVars(numStaticVars)
error = config.GetNumStaticVars() - numStaticVars
config.GetHasStaticVars()

config.SetNumIntegralVars(numIntegralVars)
error = config.GetNumIntegralVars() - numIntegralVars
config.GetHasIntegralVars()

%% Test get set for NLP variable data
numStateVarsNLP = 68;
numControlVarsNLP = 76;
numDecisionVarsNLP = 785;

config.SetNumStateVarsNLP(numStateVarsNLP)
error = config.GetNumStateVarsNLP() - numStateVarsNLP

config.SetNumControlVarsNLP(numControlVarsNLP)
error = config.GetNumControlVarsNLP() - numControlVarsNLP

config.SetNumDecisionVarsNLP(numDecisionVarsNLP)
error = config.GetNumDecisionVarsNLP() - numDecisionVarsNLP

%% Test get set for types of optimal control functions
config.SetHasDefectCons(true)
config.GetHasDefectCons()
config.SetHasIntegralCons(true)
config.GetHasIntegralCons()
config.SetHasIntegralCost(true)
config.GetHasIntegralCost()
config.SetHasAlgebraicCost(true)
config.GetHasAlgebraicCost()

