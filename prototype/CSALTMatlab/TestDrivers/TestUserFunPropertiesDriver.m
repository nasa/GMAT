import userfunutils.UserFunProperties
tc = UserFunProperties()
stateJacPattern = [1 0 1; 1 1 1;0 0 1];
timeJacPattern = [1 0;0 1;1 0];
controlJacPattern = [1;0;0];
numFunctions = 3;
hasStateVars = 1;
hasControlVars = 1;

tc.SetStateJacPattern(stateJacPattern)
tc.SetControlJacPattern(controlJacPattern)
tc.SetTimeJacPattern(timeJacPattern)
tc.SetNumFunctions(numFunctions)
tc.SetHasStateVars(hasStateVars)
tc.SetHasControlVars(hasControlVars)


tc.GetStateJacPattern() - stateJacPattern
tc.GetControlJacPattern() -controlJacPattern
tc.GetTimeJacPattern() - timeJacPattern
tc.GetNumFunctions() - numFunctions
tc.GetHasStateVars() - hasStateVars
tc.GetHasControlVars() - hasControlVars
