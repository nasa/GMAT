classdef MoonLanderPointObject < userfunutils.UserPointFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)

            %  Boundary Conditions
            stateVec1i = obj.GetInitialStateVec(1);
            stateVec1f = obj.GetFinalStateVec(1);
            hi = stateVec1i(1);
            vi = stateVec1i(2);
            mi = stateVec1i(3);
            hf = stateVec1f(1);
            vf = stateVec1f(2);
            boundCon = [obj.GetInitialTime(1) hi vi mi hf vf]'; 
            
            boundConUpper = [0 1 -0.783 1 0 0]';
            boundConLower = [0 1 -0.783 1 0 0]';
                        
            %  Set the constraints
            obj.SetAlgFunctions(boundCon);
            obj.SetAlgFuncLowerBounds(boundConLower);
            obj.SetAlgFuncUpperBounds(boundConUpper);
                     
        end
        
        function EvaluateJacobians(obj)
            
        end
    end
    
end



