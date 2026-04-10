classdef SchwartzPointObject < userfunutils.UserPointFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
                     
            %  Set the cost function
            finalStatePhase2 = obj.GetFinalStateVec(2);
            obj.SetCostFunction(5*(finalStatePhase2(1)^2 + finalStatePhase2(2)^2));
            
            %  Boundary conditions at beginning of phase 1
            boundCon = [obj.GetInitialTime(1);obj.GetFinalTime(1);obj.GetInitialStateVec(1);
                obj.GetInitialTime(2);obj.GetFinalTime(2)];
            boundConUpper = [0 1 1 1 1 2.9]';
            boundConLower = [0 1 1 1 1 2.9]';
            
            %  Link the two phases
            link = [obj.GetFinalTime(1) - obj.GetInitialTime(2);
                obj.GetFinalStateVec(1) - obj.GetInitialStateVec(2)];
            linkBound = [0 0 0]';
            
            %  Set the constraints
            obj.SetAlgFunctions([boundCon;link]);
            obj.SetAlgFuncLowerBounds([boundConLower;linkBound]);
            obj.SetAlgFuncUpperBounds([boundConUpper;linkBound]);
                     
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end



