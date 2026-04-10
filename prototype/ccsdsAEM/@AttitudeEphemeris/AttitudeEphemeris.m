classdef AttitudeEphemeris < handle
    
    %AttitudeEphemeris A CCSDS AEM file.
  
    properties
        
        %  Required AEM header contents
        CCSDS_AEM_VERS = ''
        CREATION_DATE  = ''
        ORIGINATOR     = ''
                
        %  string containing file name and path
        EphemerisFile             
        %  the file handle returned from fopen(obj.EphemerisFile)
        fileHandle
        
    end
    
    properties (Access = private)
        %  Number of segments on the file
        numSegments = 0; 
        %  List of segment objects.
        Segments = {};
    end
    
    methods
         
        %  Initialize the file.  Read it, set up all data.
        Initialize(obj)
        %  Open the AEM text file
        OpenFile(obj) 
        %  Close the AEM text file
        CloseFile(obj)  
        %  Parse the file and set up data.
        ParseFile(obj)
        %  Add a new segment to the ephemeris object
        AddSegment(obj,segment)
        %  Get an structure of metadata contents
        [struct] = GetEmptyMetaDataStruct(obj)
        %  Gets the segment number given epoch
        segmentNumber = GetSegmentNumberByUseableStartTime(obj, epoch);
        %  Returns requested segment.  Mainly for debug!!
        [segment] = GetSegment(obj,segIdx)
        %  Write ephemeris
        %  TODO: WriteEphemeris(obj)
        
    end
    
end
