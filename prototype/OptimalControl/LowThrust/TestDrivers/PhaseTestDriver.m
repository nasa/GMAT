phase1 = Phase();

%% Test setting bounds
phase1.positionUpperBound  = 30000;
phase1.positionLowerBound  = -30000;
phase1.velocityUpperBound  = 12;
phase1.velocityLowerBound  = -12;
phase1.massUpperBound      = 1000;  
phase1.massLowerBound      = 0;
phase1.ValidateBoundProperties();

%% Test mesh setting
phase1.meshIntervalPoints     = [45];
phase1.meshIntervalFractions  = [1];
phase1.ValidateMeshProperties();

%%  Test Setting intial epoch and state
phase1.initialEpoch      = 21545;
phase1.finalEpoch        = 21545.2;
phase1.initialGuessState = [7100 0 1300 0 7.35 1 500]'; 
phase1.initialGuessEpoch = 21545;
phase1.ValidateStateProperties();


        
        
        
        
