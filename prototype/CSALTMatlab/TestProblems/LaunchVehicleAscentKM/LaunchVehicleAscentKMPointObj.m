classdef LaunchVehicleAscentKMPointObj < userfunutils.UserPointFunction
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            
            %%  Extract state and control components
            initialTime = obj.GetInitialTime(1);
            finalTime = obj.GetFinalTime(1);
            initialStateVec = obj.GetInitialStateVec(1);
            finalStateVec = obj.GetFinalStateVec(1);
                                  
            obj.SetAlgFunctions([initialTime ;initialStateVec; 
                finalStateVec(2);
                finalStateVec(3);
                finalStateVec(4);
                ]);
            obj.SetAlgFuncLowerBounds([0 0 0 0 0 1.8e5  7726.3 0]'/1000);
            obj.SetAlgFuncUpperBounds([0 0 0 0 0 1.8e5  7726.3 0]'/1000);
            obj.SetCostFunction(finalTime)
%             
%                         
%             obj.SetAlgFunctions([initialTime ;initialStateVec;     ]);
%             obj.SetAlgFuncLowerBounds([0 0 0 0 0 ]');
%             obj.SetAlgFuncUpperBounds([0 0 0 0 0 ]');
%             obj.SetCostFunction(finalTime)
            
        end
        
        function EvaluateJacobians(~)
            
        end
        
    end
    
end

