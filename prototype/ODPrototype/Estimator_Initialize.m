function Solver = Estimator_Initialize(Solver)

global ObjectStore

%==========================================================================
%==========================================================================
%---- Determine the participants
%==========================================================================
%==========================================================================

%----- HARD CODE FOR NOW. BUT FIX LATER.
%----- These are pointers to objects in ObjectStore
ESM.Participants = [1 2];

%----- Loop over the number of measurement types
% for i = 1:size(Solver.Measurements,2);
% 
%     CurrentMeas = Solver.Measurements{i};
%     %----- Loop over the measurements for each type
%     for j = 1:size(CurrentMeas.Measurements,2)
%         
%         
% 
%     end
% 
% end


%==========================================================================
%==========================================================================
%---- Determine what objects and properties will need to be propagated and
%     set their pointers and object ids
%==========================================================================
%==========================================================================

%  PropObjects is a list of strings containing the objects that have some
%  property that must be numerically integrated.
%
%  PropParamId is a cell array of matrices that containt integer identifiers
%  for what parameters must be propagated for each PropObject.
%  For a spacecraft
%     1 - Orbit Cartesian state
%     2 - Cr
%     3 - Cd
%     4 - STM
%  For a GroundStation Measurement object
%     1 - Range bias
%     2 - Range rate bias

%----- HARD CODE FOR NOW. BUT FIX LATER.
ESM.PropObjects    = [1];      % pointers to objects in ObjectStore
ESM.PropParamId{1} = [1 4];

%==========================================================================
%==========================================================================
%---- Determine what objects and properties will need to be estimated and
%     set their pointers and object ids
%==========================================================================
%==========================================================================

%  Estimator Objects Ids
%     1 - Spacecraft
%     2 - Ground station
%     3 - GroundStationMeasurement

%----- HARD CODE FOR NOW. BUT FIX LATER.
ESM.EstimObjects    = [1];         % pointers to objects in ObjectStore
ESM.EstimObjectsId  = [ 1 ];
ESM.EstimParamId{1} = [1 2 3];

Solver.ESM = ESM;
 
%==========================================================================
%==========================================================================
%---- "clone" and attach propagator to estimator
%==========================================================================
%==========================================================================
Solver.Prop = ObjectStore.Objects{5};





