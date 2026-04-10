classdef EphemerisSegment < handle
    %EPHEMERISSEGMENT A segment of a CCSDS OEM file.
    
    %   Author: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    
    properties
        CentralBody
        Data
        Interpolation
        InterpolationDegree
        Interpolator
        StartTime
        StopTime
        UseableStartTime
        UseableStopTime
    end
    
    methods
    end
    
end
