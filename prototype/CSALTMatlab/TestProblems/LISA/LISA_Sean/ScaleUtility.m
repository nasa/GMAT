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
            error(['central body with id ' num2str(cbIndex) ' not found ']);
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
            error(['central body with id ' num2str(cbIndex) ' not found ']);
        end
        
        function time = ScaleTime(obj,time,cbIndex,refEpoch)
            if cbIndex == 1
                TU = obj.sunScaling.TU;
            elseif cbIndex == 3
                TU = obj.earthScaling.TU;
            else
                error(['central body with id ' num2str(cbIndex) ' not found ']);
            end
            time = (time - refEpoch)*86400/TU;
        end
        
        function time = UnScaleTime(obj,time,cbIndex,refEpoch)
            if cbIndex == 1
                TU = obj.sunScaling.TU;
            elseif cbIndex == 3
                TU = obj.earthScaling.TU;
            else
                error(['central body with id ' num2str(cbIndex) ' not found ']);
            end
            time = time*TU/86400 + + refEpoch;
        end
        
        function MJD = ScaledTimedToMJD(obj, scaledTime, cbIndex, refEpoch)
            % convert from scaled time to GMAT MJD, given a reference epoch
            % in GMAT MJD. The reference epoch is zero in scaled time
            if cbIndex == 1
                TU = obj.sunScaling.TU;
            elseif cbIndex == 3
                TU = obj.earthScaling.TU;
            else
                error(['central body with id ' num2str(cbIndex) ' not found ']);
            end
            MJD = scaledTime * TU / 86400 + refEpoch; % TU converts to seconds; divide by seconds per day to get to days, then add refEpoch to get to MJD
        end
        
        function scaledTime = MJDToScaledTimed(obj, MJD, cbIndex, refEpoch)
            % convert from GMAT MJD to scaled time, given a reference epoch
            % in GMAT MJD. The reference epoch is zero in scaled time
            if cbIndex == 1
                TU = obj.sunScaling.TU;
            elseif cbIndex == 3
                TU = obj.earthScaling.TU;
            else
                error(['central body with id ' num2str(cbIndex) ' not found ']);
            end
            scaledTime = (MJD - refEpoch) * 86400 / TU; % 86400 converts from days to seconds; TU converts to nondim time; subtracting refEpoch gets to fully scaled time
        end
        
    end
    
end

