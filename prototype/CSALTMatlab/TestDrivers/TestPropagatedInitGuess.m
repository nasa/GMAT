function TestPropagatedInitGuess()

%%  State Guess
initialGuessState = [-0.117491025467658
       -0.0314059518134201
          1.01630018575157
         -3.40881243826767];
initialTime = 0;
finalTime   = 1.38169857080711;

%% Create a UserPathFunction and initialize it
userPathFunc = UserPathFunction();
userPathFunc.functionName = 'EarthMoonTrajectoryPathFunction';
numStates   = 4;
numControls = 2
userPathFunc.stateVec = initialGuessState;
userPathFunc.controlVec = [1 1]';
userPathFunc.time = 0;
userPathFunc.Initialize();

  
%% Use ODE45, send in dummyfile interface and propagate
[t,Y] = ode45(@DummmyPathFun,...
    [initialTime:0.05:finalTime],initialGuessState,[],userPathFunc,1)

plot(Y(:,1),Y(:,2))

function Xdot =  DummmyPathFun(t,X,funcObj,phaseIdx)

%% Prepare the input structure required by user path function
%  Prepare state for input
funcObj.stateVec = X;
%  Prepare control for input
funcObj.controlVec = [0 0]';
%  Prepare the time
funcObj.time = t; 
%  Prepare the phase number
funcObj.phaseNum = phaseIdx

%% Evaluate the functions and return the derivative
funcObj.Evaluate()
Xdot = funcObj.dynFunctions




