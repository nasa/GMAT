classdef LinkageConstraint < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
        %  "Pointer" to the phases
        initialPhase
        finalPhase
        
        %  Attribures of the linkage
        numLinkageConstraints
        linkageConstraints
        
        %  Bounds
        lowerBound
        upperBound
       
    end
    
    methods
        
        %  Initialize the object so that we have everything ready to
        %  compute the constraints.
        function Initialize(obj,phase1,phase2)
            
            %  Set "pointers" to the phases
            obj.initialPhase = phase1;
            obj.finalPhase   = phase2;
            
            %  For now, the only linkage is complete state continuity.
            %  TODO: This will change later
            obj.numLinkageConstraints = phase1.numStates + 1;
            obj.lowerBound = zeros(obj.numLinkageConstraints,1);
            obj.upperBound = zeros(obj.numLinkageConstraints,1);
            
        end
        
        %  Compute the linkage constraints
        function [linkConstraints] = GetLinkageConstraints(obj)
            stateLinkError = ...
                obj.finalPhase.DecVector.GetFirstStateVector() - ...
                obj.initialPhase.DecVector.GetLastStateVector();
               
            timeLinkError = ...
                obj.finalPhase.DecVector.GetFirstTime() - ...        
                obj.initialPhase.DecVector.GetLastTime();
            
            linkConstraints = [stateLinkError;timeLinkError];
        end
        
    end
    
end

