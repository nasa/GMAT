classdef ObstacleAvoidancePointObject < userfunutils.UserPointFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
                     
            %  Boundary conditions at beginning of phase 1
            boundCon = [obj.GetInitialTime(1) obj.GetFinalTime(1)...
                obj.GetInitialStateVec(1)' obj.GetFinalStateVec(1)']';
            boundConUpper = [0 1 0 0 1.2 1.6]';
            boundConLower = [0 1 0 0 1.2 1.6]';
                        
            %  Set the constraints
            obj.SetAlgFunctions(boundCon);
            obj.SetAlgFuncLowerBounds(boundConLower);
            obj.SetAlgFuncUpperBounds(boundConUpper);
                     
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end



