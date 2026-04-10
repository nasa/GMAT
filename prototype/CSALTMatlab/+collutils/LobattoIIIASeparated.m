classdef LobattoIIIASeparated < collutils.ImplicitRungeKutta
    %UNTITLED9 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = protected)
        isSeparated = true;
    end
    
    methods
        function isSeparated = GetIsSeparated(obj)
            isSeparated = obj.isSeparated;
        end
        function funcVec = GetFuncVecFromArray(obj,funcArray,numStates,deltaT) 
            funcVec = zeros(numStates*obj.numPointsPerStep,1);
            for funcIdx = 1:obj.numPointsPerStep
                endIdx = numStates*funcIdx;
                startIdx = endIdx - (numStates-1);
                funcVec(startIdx:endIdx,1) = deltaT*funcArray(:,funcIdx);
            end
        end
        function weights = GetQuadratureWeights(obj)
           weights = obj.betaVec; 
        end
    end
    
end

