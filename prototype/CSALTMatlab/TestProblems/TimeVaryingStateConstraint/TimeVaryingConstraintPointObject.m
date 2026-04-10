classdef TimeVaryingConstraintPointObject < userfunutils.UserPointFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)

            %  Boundary Conditions
            stateVec1 = obj.GetInitialStateVec(1);
            x1 = stateVec1(1);
            x2 = stateVec1(2);

            boundCon = [obj.GetInitialTime(1) obj.GetFinalTime(1) x1 x2]'; 
            
            boundConUpper = [0 1 0 -1]';
            boundConLower = [0 1 0 -1]';
                        
            %  Set the constraints
            obj.SetAlgFunctions(boundCon);
            obj.SetAlgFuncLowerBounds(boundConLower);
            obj.SetAlgFuncUpperBounds(boundConUpper);
                     
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end



