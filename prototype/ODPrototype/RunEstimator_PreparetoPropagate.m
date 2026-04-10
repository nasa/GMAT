function Prop = RunEstimator_PreparetoPropagate(Prop,Solver);

global ObjectStore

%==========================================================================
%==========================================================================
%---- Populate the ESV
%==========================================================================
%==========================================================================

PSV        = [];
for i = 1:size(Prop.PropObjects,2)

    this = Prop.PropObjects(i);
    Obj  = ObjectStore.Objects{this};
    %----- Loop over the parameters estimated on the ith object
    for j = 1:size(Prop.PropParamId{i},2)

        %----- Add spacecraft related state components
        if Prop.PropObjects(i) == 1
            x   = Spacecraft_GetState(Obj, Solver.ESM.PropParamId{i}(j));
        end

        %----- Add other related state components
        %  NEEDS TO BE FILLED IN!!

        %----- Add new component to ESV
        PSV = [PSV;x];

    end

end

Prop.PSV = PSV;


