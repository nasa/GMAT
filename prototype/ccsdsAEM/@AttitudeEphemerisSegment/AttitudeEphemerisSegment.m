classdef AttitudeEphemerisSegment < handle
    
    %EPHEMERISSEGMENT A segment of a CCSDS OEM file.

    properties (Access = public)
        
        %  Required Metadata contents
        OBJECT_NAME
        OBJECT_ID
        CENTER_NAME
        REF_FRAME_A
        REF_FRAME_B
        ATTITUDE_DIR
        TIME_SYSTEM
        START_TIME
        USEABLE_START_TIME
        USEABLE_STOP_TIME
        STOP_TIME
        ATTITUDE_TYPE
        INTERPOLATION_METHOD
        INTERPOLATION_DEGREE
        
    end

    methods
        
        %  Sets up metadata passed from parser
        Initialize(obj,struct)
        %  Parses an epoch
        [epoch] = ParseEphemEpoch(obj,dateStr)
        %  Returns data array.  Primarly for debugging
        [data] = GetData(obj);
        
    end
    
end
