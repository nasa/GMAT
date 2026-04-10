classdef Ephemeris < handle
    %EPHEMERIS A CCSDS OEM file.
    
    %   Author: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    
    properties
        
        % TODO not all of this data should be public (SPH)
        Segments = EphemerisSegment.empty()
        StateTolerance = 1e-3 / 86400                       % 1 ms
        
         % SPH: Added these data
        numSegments   %  Number of segments on the file
        EphemerisFile %  The file name and path
        firstEpochOnFile =  inf;  % initialize to end of time
        lastEpochOnFile  = -inf;  % initialize to beginning of time
        
    end
    
    methods
        segmentNumber = GetSegmentNumberByUseableStartTime(obj, epoch);
        [state centralBody] = GetState(obj, epoch);
        
        ParseEphemeris(obj)
    end
    
end
