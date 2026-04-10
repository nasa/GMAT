
import collutils.DecVecTypeBetts

%% Configure the test vector 
clc
%  Properties of the state/controls for continuous optimal control
%  problem
numStates     = 3;
numControls   = 2;
numIntegrals  = 10;
numStaticsNLP = 12;
time0         = -12345;
timef         = 12345;

%  Properties of the discretization of the state and controls
numStateMeshPoints    = 4;
numControlMeshPoints  = 3;
numStateStagePoints   = 2;
numControlStagePoints = 2;
numStatePoints   = (numStateMeshPoints-1)*(1+numStateStagePoints)+1; 
if numStateMeshPoints  == numControlMeshPoints
numControlPoints = (numControlMeshPoints-1)*(1+numControlStagePoints)+1;
else
    numControlPoints = (numControlMeshPoints)*(1+numControlStagePoints);
end

%  Build a dummy V matrix in Eq. 40.
%  V = [X1 Y1 Z1 VX1 VY1 VZ1 M1]
%      [X2 Y2 Z2 VX2 VY2 VZ2 M2]
%      [XN YN ZN VXN VYN VZN M1]
value = 0;
V = zeros(numStatePoints,numStates);
for colIdx = 1:numStates 
    for rowIdx = 1:numStatePoints
        V(rowIdx,colIdx) = rowIdx + 0.1*colIdx ;
    end
end
zState = reshape(V,numStates*numStatePoints,1);

%  Build a dummy W matrix in Eq. 41.
%  W = [Ux1 Uy1 Uz1]
%      [Ux2 Uy2 Uz2]
%      [UxN UyN UzN]
value = 200;
W = zeros(numControlPoints,numControls);
for colIdx = 1:numControls
    for rowIdx = 1:numControlPoints
        value = value + 1;
        W(rowIdx,colIdx) = rowIdx + 0.1*colIdx + 1000;
    end
end
zControl = reshape(W,numControls*numControlPoints,1);

%  Build a dummy static Vector
value = 101;
zStatic = zeros(numStaticsNLP,1);
for rowIdx = 1:numStaticsNLP
    zStatic(rowIdx) = value;
    value = value + 1;
end

%  Build a dummy integral Vector
value = 501;
zIntegral = zeros(numIntegrals,1);
for rowIdx = 1:numIntegrals
    zIntegral(rowIdx) = value;
    value = value + 1;
end

zTime = [time0; timef;];
stateVector = [ zTime; zState; zControl; zStatic; zIntegral;  ];


myVector = DecVecTypeBetts();
myVector.Initialize(numStates,numControls,...
                 numIntegrals,numStaticsNLP,numStateMeshPoints,...
                 numControlMeshPoints,numStateStagePoints,...
                 numControlStagePoints);
             
%% Test setting decision parameters by type
disp(' The following tests failed ')
%  Define interpolation during initialization
%myVector.SetInterplotionType('Lagrange')
%  Send in the time vector for the nominal points, add new data member
%  called nominalTimeVector and set it during initialization.
%myVector.SetTimeVector(nominalTimeVector)
%  Interplate state to new times
%stateOut = myVector.InterpolateState(newTimeVector);
%  Interplate control to new times
%controlOut = myVector.InterpolateControl(newTimeVector);


myVector.SetTimeVector(zTime);
myVector.SetStaticVector(zStatic);
myVector.SetIntegralVector(zIntegral);

%  Time parameters
test = myVector.GetTimeVector();
diff = test - zTime;
if max(abs(diff)) > 1e-15
    disp('time set/get failed')
end

%  Static parameters
test = myVector.GetStaticVector();
diff = test - zStatic;
if max(abs(diff)) > 1e-15
    disp('static set/get failed')
end

%  integral parameters
test = myVector.GetIntegralVector();
diff = test - zIntegral;
if max(abs(diff)) > 1e-15
    disp('integral set/get failed')
end

%  state vector
myVector.SetStateVector(1,2,[10;11;12]);
diff =  myVector.decisionVector(13:15) - [10;11;12];
if max(abs(diff)) > 1e-15
    disp('state set failed')
end
test = myVector.GetStateVector(1,2);
diff = test - [10;11;12];
if max(abs(diff)) > 1e-15
    disp('state get failed')
end

%  state Array
myVector.SetStateArray(V);
test = myVector.GetStateArray();
diff = test - V;
if max(abs(diff)) > 1e-15
    disp('state array set/get failed')
end

%  control vector
myVector.SetControlVector(1,2,[555.1;555.2]);
diff =  myVector.decisionVector(16:17) - [555.1;555.2];
if max(abs(diff)) > 1e-15
    disp('control set failed')
end
test = myVector.GetControlVector(1,2);
diff = test - [555.1;555.2];
if max(abs(diff)) > 1e-15
    disp('control get failed')
end

%  state Array
myVector.SetControlArray(W);
test = myVector.GetControlArray();
diff = test - W;
if max(abs(diff)) > 1e-15
    disp('control array set/get failed')
end



% 
% myVector.GetStateIdxsAtMeshPoint(1,0)
% myVector.GetControlIdxsAtMeshPoint(1,0)
% myVector.GetStateIdxsAtMeshPoint(1,1)
% myVector.GetControlIdxsAtMeshPoint(1,1)
% myVector.GetStateIdxsAtMeshPoint(1,2)
% myVector.GetControlIdxsAtMeshPoint(1,2)
% myVector.GetStateIdxsAtMeshPoint(2,0)
% myVector.GetControlIdxsAtMeshPoint(2,0)
% myVector.GetStateIdxsAtMeshPoint(2,1)
% myVector.GetControlIdxsAtMeshPoint(2,1)


%  Check getting state at mesh points and stage points
meshIdx  = 4;
stageIdx = 0;
stateAtMeshPoint = myVector.GetStateAtMeshPoint(meshIdx,stageIdx);
stateArray = myVector.GetStateArray();
% result = stateAtMeshPoint - stateArray(10,:)';
% if max(max(result) ~= 0
    


meshIdx = 3;
stageIdx = 2;
stateAtMeshPoint = myVector.GetStateAtMeshPoint(meshIdx,stageIdx);
stateArray = myVector.GetStateArray();
stateAtMeshPoint - stateArray(9,:)'

%  Check getting control at mesh points and stage points
meshIdx  = 3;
stageIdx = 0;
controlAtMeshPoint = myVector.GetControlAtMeshPoint(meshIdx,stageIdx);
controlArray = myVector.GetControlArray();
controlAtMeshPoint - controlArray(7,:)'

meshIdx  = 2;
stageIdx = 1;
controlAtMeshPoint = myVector.GetControlAtMeshPoint(meshIdx,stageIdx);
controlArray = myVector.GetControlArray();
controlAtMeshPoint - controlArray(5,:)'

return
%%  Test getting components of decision vector
%stateSubVector    = myVector.GetStateSubVector() - zState
%controlSubVector  = myVector.GetControlSubVector() - zControl
%integralSubVector = myVector.GetIntegralSubVector() - zIntegral
%staticSubVector   = myVector.GetStaticSubVector() - zStatic
%timeSubVector     = myVector.GetTimeSubVector() - [time0;timef]
stateArray        = myVector.GetStateArray() - V
controlArray      = myVector.GetControlArray() - W

%%  Test setting components of decision vector
myVector.SetStateArray(V)
stateVector - myVector.decisionVector
myVector.SetStateVector(zState)
stateVector - myVector.decisionVector

myVector.SetControlArray(W)
stateVector - myVector.decisionVector
myVector.SetControlVector(zControl)
stateVector - myVector.decisionVector

myVector.SetStaticVector(zStatic)
stateVector - myVector.decisionVector

myVector.SetIntegralVector(zIntegral)
stateVector - myVector.decisionVector

stateVec = myVector.GetFirstStateVector();
stateAtMeshPoint = myVector.GetStateAtMeshPoint(1);
stateVec - stateAtMeshPoint

stateVec = myVector.GetLastStateVector();
stateAtMeshPoint = myVector.GetStateAtMeshPoint(numStatePoints);
stateVec - stateAtMeshPoint