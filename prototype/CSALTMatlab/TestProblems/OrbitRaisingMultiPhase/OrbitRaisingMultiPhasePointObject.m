classdef OrbitRaisingMultiPhasePointObject < userfunutils.UserPointFunction
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            mu = 1;
            %  Algegraic constraints at beginning and end of phase 1
            phase1Functions = [obj.GetInitialTime(1); obj.GetInitialStateVec(1)];
            phase1ConLowerBound = [ 0  1 0 0 1 1 ]';
            phase1ConUpperBound = [ 0  1 0 0 1 1 ]';
            
            %  Algegraic constraints at beginning and end of phase 2
            finalStateVec = obj.GetFinalStateVec(2);
            r     = finalStateVec(1,1);
            theta = finalStateVec(2,1);
            dr_dt = finalStateVec(3,1);
            dtheta_dt = finalStateVec(4,1);
            orbitRateError = sqrt(mu/r) - dtheta_dt;
            obj.SetCostFunction(-r);
            phase2Functions=[orbitRateError;
                obj.GetFinalTime(2);
                finalStateVec];
            phase2ConLowerBound = [0 3.32  -1 -pi 0 -10 0]';
            phase2ConUpperBound = [0 3.32  10  pi 0  10 1]';
            
            %  Linkage constraints
            link1 = [obj.GetInitialTime(2) - obj.GetFinalTime(1); ...
                     obj.GetInitialStateVec(2) - obj.GetFinalStateVec(1)];
            link1LowerBound = zeros(6,1);
            link1UpperBound = zeros(6,1);
            
            %  Set the values
            obj.SetAlgFunctions([phase1Functions;phase2Functions;link1])
            obj.SetAlgFuncLowerBounds([phase1ConLowerBound;phase2ConLowerBound;link1LowerBound]);
            obj.SetAlgFuncUpperBounds([phase1ConUpperBound;phase2ConUpperBound;link1UpperBound]);
            
        end
        
        function EvaluateJacobians(~)
            
        end
        
    end
    
end

