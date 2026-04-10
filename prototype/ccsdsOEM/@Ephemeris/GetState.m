function [state centralBody] = GetState(obj, epoch)
    %GETSTATE Get an individual state from an ephemeris.
    %   STATE = GETSTATE(OBJ, EPOCH) returns an individual state at epoch
    %   EPOCH from an active Ephemeris object OBJ. If the state is not
    %   present in the ephemeris, it is interpolated. EPOCH must be in
    %   serial date format (such as returned by datenum()).
    %
    %   [STATE CENTRALBODY] = GETSTATE(OBJ, EPOCH) also returns a handle to
    %   the CelestialBody object that is the central body for the state.
    
    %   Author: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    
    flaggedSegment = [];
    for i=1:length(obj.Segments)
        if ~isempty(obj.Segments(i).UseableStartTime)
            if epoch >= obj.Segments(i).UseableStartTime && ...
                    epoch <= obj.Segments(i).UseableStopTime
                flaggedSegment = i;
                break;
            end
        else
            if epoch >= obj.Segments(i).StartTime && ...
                    epoch <= obj.Segments(i).StopTime
                flaggedSegment = i;
                break;
            end
        end
    end
    
    if isempty(flaggedSegment)
        throw(MException('Events:Ephemeris:GetState:noMatchingSegment', ...
            'There is no matching segment that contains the epoch %s', ...
            datestr(epoch, 'yyyy-mm-ddTHH:MM:SS.FFF')));
    end
    
    exactMatch = abs(obj.Segments(flaggedSegment).Data(:,1) - epoch) <= ...
        obj.StateTolerance;
    if any(exactMatch)
        state = obj.Segments(flaggedSegment).Data(exactMatch, 2:end);
    else
        state = obj.Segments(flaggedSegment).Interpolator.Interpolate( ...
            obj.Segments(flaggedSegment), epoch);
    end
    
    centralBody = obj.Segments(flaggedSegment).CentralBody;
    
end
