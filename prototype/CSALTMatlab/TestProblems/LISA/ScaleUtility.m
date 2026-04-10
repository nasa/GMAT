classdef ScaleUtility < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        phaseList
        earthScaling
        sunScaling
    end
    
    methods
        
        function state = ScaleState(obj,state,cbIndex)
            if cbIndex == 1
                state = obj.sunScaling.StateScaleMat*state;
                return
            end
            if cbIndex == 3
                state = obj.earthScaling.StateScaleMat*state;
                return
            end
            error(['central body with id ' num2str(dbIdx) ' not found ']);
        end
        
        function state = UnScaleState(obj,state,cbIndex)
            if cbIndex == 1
                state = inv(obj.sunScaling.StateScaleMat)*state;
                return
            end
            if cbIndex == 3
                state = inv(obj.earthScaling.StateScaleMat)*state;
                return
            end
            error(['central body with id ' num2str(dbIdx) ' not found ']);
        end
        
        function time = ScaleTime(obj,time,cbIndex,refEpoch)
            if cbIndex == 1
                TU = obj.sunScaling.TU;
            elseif cbIndex == 3
                TU = obj.earthScaling.TU;
            else
                error(['central body with id ' num2str(dbIdx) ' not found ']);
            end
            time = (time - refEpoch)*86400/TU;
        end
        
        function time = UnScaleTime(obj,time,cbIndex,refEpoch)
            if cbIndex == 1
                TU = obj.sunScaling.TU;
            elseif cbIndex == 3
                TU = obj.earthScaling.TU;
            else
                error(['central body with id ' num2str(dbIdx) ' not found ']);
            end
            time = time*TU/86400 + + refEpoch;
        end
        
    end
    
end

