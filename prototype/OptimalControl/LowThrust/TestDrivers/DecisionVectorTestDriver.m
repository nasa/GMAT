%% Configure the test vector 
clc
%  Properties of the state/controls for continuous optimal control
%  problem
numStates       = 7;
numControls     = 3;
numIntegrals    = 10;
numStaticParams = 12;
time0           = -12345;
timef           = 12345;

%  Properties of the discretization of the state and controls
numStatePoints   = 8;
numControlPoints = 7;
numStateStages   = 0;
numControlStages = 0;

%  Build a dummy V matrix in Eq. 40.
%  V = [X1 Y1 Z1 VX1 VY1 VZ1 M1]
%      [X2 Y2 Z2 VX2 VY2 VZ2 M2]
%      [XN YN ZN VXN VYN VZN M1]
value = 0;
V = zeros(numStatePoints,numStates);
for colIdx = 1:numStates 
    for rowIdx = 1:numStatePoints
        value = value + 1;
        V(rowIdx,colIdx) = value
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
        W(rowIdx,colIdx) = -value
    end
end
zControl = reshape(W,numControls*numControlPoints,1);

%  Build a dummy static Vector
value = 101;
zStatic = zeros(numStaticParams,1);
for rowIdx = 1:numStaticParams
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

zTime = [time0; timef;]
stateVector = [zState; zControl; zIntegral; zTime;  zStatic];
            
%%  Set the test vector and the test exctracting parts
myVector = DecisionVector;
myVector.ConfigureDecisionVector(numStates,numControls,...
                 numIntegrals,numStaticParams,numStatePoints,...
                 numControlPoints,numStateStages, numControlStages);
myVector.SetDecisionVector(stateVector)           

%%  Test getting components of decision vector
stateSubVector    = myVector.GetStateSubVector() - zState
controlSubVector  = myVector.GetControlSubVector() - zControl
integralSubVector = myVector.GetIntegralSubVector() - zIntegral
staticSubVector   = myVector.GetStaticSubVector() - zStatic
timeSubVector     = myVector.GetTimeSubVector() - [time0;timef]
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

myVector.SetTimeVector(zTime)
stateVector - myVector.decisionVector

myVector.SetStaticVector(zStatic)
stateVector - myVector.decisionVector

myVector.SetIntegralVector(zIntegral)
stateVector - myVector.decisionVector

meshIdx = 4;
stateAtMeshPoint = myVector.GetStateAtMeshPoint(meshIdx);
stateArray = myVector.GetStateArray();
stateAtMeshPoint - stateArray(meshIdx,:)'

meshIdx = 3;
controlAtMeshPoint = myVector.GetControlAtMeshPoint(meshIdx);
controlArray = myVector.GetControlArray();
controlAtMeshPoint - controlArray(meshIdx,:)'

stateVec = myVector.GetFirstStateVector();
stateAtMeshPoint = myVector.GetStateAtMeshPoint(1);
stateVec - stateAtMeshPoint

stateVec = myVector.GetLastStateVector();
stateAtMeshPoint = myVector.GetStateAtMeshPoint(numStatePoints);
stateVec - stateAtMeshPoint

controlVec = myVector.GetFirstControlVector();
controlAtMeshPoint = myVector.GetControlAtMeshPoint(1);
controlVec - controlAtMeshPoint

controlVec = myVector.GetLastControlVector();
controlAtMeshPoint = myVector.GetControlAtMeshPoint(numControlPoints);
controlVec - controlAtMeshPoint
