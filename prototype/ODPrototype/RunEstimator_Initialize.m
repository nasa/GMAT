function Prop = RunEstimator_Initialize(Prop,Solver)

global ObjectStore

%----- Call ESM on the estimator to get get prop objects and parameter Ids
Prop.PropObjects = Solver.ESM.PropObjects;
Prop.PropParamId = Solver.ESM.PropParamId;

%----- Determine size of state vector and dimension it accordingly.
NumStates  = 0;
ODEmap     = [];
StateCount = 0;
PSV        = [];
for i = 1:size(Prop.PropObjects,2)

    this = Prop.PropObjects(i);
    Obj  = ObjectStore.Objects{this};
    %----- Loop over the parameters estimated on the ith object
    for j = 1:size(Prop.PropParamId{i},2)

        %----- Add spacecraft related state components
        if Prop.PropObjects(i) == 1
            x   = Spacecraft_GetState(Obj, Solver.ESM.PropParamId{i}(j));
            StateCount = StateCount + 1;
            ODEmap.Size(StateCount) = size(x,1);
            ODEmap.Type(StateCount) = Prop.PropParamId{i}(j);
        end

        %----- Add other related state components
        %  NEEDS TO BE FILLED IN!!

        %----- Add new component to ESV
        PSV = [PSV;x*0];

    end

end

Prop.ODEmap = ODEmap;
Prop.PSV = PSV;
Prop.NumStates = size(PSV,1);
Prop.STM = zeros(Prop.NumStates,Prop.NumStates);


