classdef ShuttleReEntryPointObject < userfunutils.UserPointFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            global bounds
            
            %  Get the state and time 
            initState = obj.GetInitialStateVec(1);
            finalState = obj.GetFinalStateVec(1);
            initTime = obj.GetInitialTime(1);
            finalTime = obj.GetFinalTime(1);
            
            %  Set bounds on initial and final time
            bounds.phase.initialtime.lower;
            bounds.phase.initialtime.upper;
            bounds.phase.finaltime.lower; % Put in
            bounds.phase.finaltime.upper;
            bounds.phase.initialstate.lower;
            bounds.phase.initialstate.upper;
            bounds.phase.finalstate.lower;
            bounds.phase.finalstate.upper;
            
            obj.SetAlgFunctions([initTime;...
                finalTime;...
                initState;]);
            obj.SetAlgFuncLowerBounds([bounds.phase.initialtime.lower;...
                bounds.phase.finaltime.lower;...
                bounds.phase.initialstate.lower';]);
            obj.SetAlgFuncUpperBounds([bounds.phase.initialtime.upper;...
                bounds.phase.finaltime.upper;...
                bounds.phase.initialstate.upper';]);;

            %  Set the objective function
            latf = finalState(3);
            obj.SetCostFunction(latf);
            
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end
