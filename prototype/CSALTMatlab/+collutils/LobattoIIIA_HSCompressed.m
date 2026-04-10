classdef LobattoIIIA_HSCompressed < collutils.LobattoIIIACompressed
    %UNTITLED4 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function obj = LobattoIIIA_HSCompressed()
            obj = InitializeData(obj);
        end
        
        function obj = InitializeData(obj)
            obj.numDefectCons = 1;
            obj.numPointsPerStep = 3;
            % Set up non-dimensional stage times
            obj.stageTimes = zeros(3,1);
            obj.stageTimes(2,1) = 0.5;
            obj.stageTimes(3,1) = 1.0;
            % Set up the optimization parameter dependency array
            obj.paramDepArray = zeros(1,3);
            obj.paramDepArray(1,:) = [-1    0   1 ];
            % Set up the function dependency array
            obj.funcConstArray = zeros(2,3);
            obj.funcConstArray(1,:) = -[1 1 1]/6;
            obj.funcDepPattern = [1 4 1];
        end
        
        function funcVec = GetFuncVecFromArray(obj,funcArray,numStates,deltaT) 
            funcVec = zeros(numStates*obj.numPointsPerStep,1);
            for funcIdx = 1:obj.numPointsPerStep
                endIdx = numStates*funcIdx;
                startIdx = endIdx - (numStates-1);
                funcVec(startIdx:endIdx,1) = deltaT*funcArray(:,funcIdx);
            end
        end
        
    end
    
end


