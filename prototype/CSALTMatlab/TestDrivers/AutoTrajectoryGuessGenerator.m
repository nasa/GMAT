% Define the dynamics

% Define thruster config per phase
numThrustersPerPhase = [0 1 0 1 0 0 1 0];
    
% Define how phases are connected together 
phaseConnectors = {{0,1,'e2b',}; 
    {0,1,'e2b',};

% Define contstraints on start and end time of each phase.
phaseTimeGuessesStart = [];
phaseTimeGuessesEnd = [];
phaseTimeBoundUpper = [];
phaseTimeBoundsLower = [];

% Simple bounds on the controls and states
stateLowerBound = 
stateUpperBound =
controlLowerBound =
controlUpperBound =

% Configure Mesh intervals

