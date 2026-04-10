classdef OrbitTutorialPointFunc < OrbitTestPointFunc
    %UNTITLED4 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        ScaleUtility
        conValues
        initEpoch
        finalEpoch
        initState
        isInit = false
    end
    
    methods
        function InitializeFuncData(obj)
            obj.initEpoch = 30542.0;
            obj.finalEpoch = 30542.0 + 250.00;
            obj.initState = [125291184.0 -75613036.0 -32788527.0 ...
                                 13.438 25.234 10.903 3000];
            obj.conValues(1,1) = obj.ScaleUtility.ScaleTime(obj.initEpoch);
            obj.conValues(9,1) = obj.ScaleUtility.ScaleTime(obj.finalEpoch);
            obj.conValues(2:8) = obj.ScaleUtility.ScaleState(obj.initState);
            obj.conValues = obj.conValues(1:9);
            obj.isInit = true;
        end
        function EvaluateFunctions(obj)
            % Hack currently doing everything in non-dimensional coords
            % just to get this runningg.
            if ~obj.isInit
                obj.InitializeFuncData()
            end
            state = obj.GetInitialStateVec(1);
            timeInit = obj.GetInitialTime(1);
            timeFinal = obj.GetFinalTime(1);
            funcs = [timeInit;state(1:7,1); timeFinal; ] ;
            obj.SetAlgFunctions(funcs);;
            obj.SetAlgFuncLowerBounds(obj.conValues);
            obj.SetAlgFuncUpperBounds(obj.conValues);
        end
        function EvaluateJacobians(obj)
            
        end
        function PrintFuncValues(funcValues)
            
        end
    end
    
end

