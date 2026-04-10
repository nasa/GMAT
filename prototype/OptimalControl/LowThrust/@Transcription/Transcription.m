classdef Transcription < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
        %  These do not go here
        meshPoints
        numMeshPoints
        numCollocationPoints
        
        %  Cost and constraint related parameters
        numMeshIntervals
        numStagePoints
        numStatePoints
        numControlPoints
        numStateParams
        numTimeParams
        numControlParams
        numStaticParams = 0;
        numDecisionParams
        
    end
    
    methods
    end
    
end

