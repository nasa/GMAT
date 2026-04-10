function Solver = RunEstimator_PreparetoSolve(Solver);

global ObjectStore

%==========================================================================
%==========================================================================
%---- Create the ESV
%==========================================================================
%==========================================================================

ESV = [];
%----- Loop over the objects involved in the estimation
for i = 1:size(Solver.ESM.EstimObjects,2)
   
    this = Solver.ESM.EstimObjects(i);
    Obj  = ObjectStore.Objects{this};
    %----- Loop over the parameters estimated on the ith object 
    for j = 1:size(Solver.ESM.EstimParamId{i},2)
        
        %----- Add spacecraft related state components
        if Solver.ESM.EstimObjectsId(i) == 1
           x   = Spacecraft_GetState(Obj, Solver.ESM.EstimParamId{i}(j));
        end
        
        %----- Add other related state components
        %  NEEDS TO BE FILLED IN!!
        
        %----- Add new component to ESV
        ESV = [ESV;x];    
        
    end

end

Solver.ESV = ESV;





