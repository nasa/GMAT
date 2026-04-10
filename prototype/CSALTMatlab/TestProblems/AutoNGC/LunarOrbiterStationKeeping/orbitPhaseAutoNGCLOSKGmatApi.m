classdef orbitPhaseAutoNGCLOSKGmatApi < exec.OrbitPhase
    %orbitPhaseGmatApi Adds stuff for calling gmat api
    
    properties
        fm % full-fidelity force model gmat object
        fm2body % 2-body gravity force model gmat object
        sc % spacecraft gmat object
    end
    
    methods
%         function obj = orbitPhaseAutoNGCLOSKGmatApi(theUtil)
%             % Set the orbit scale utility pointer
%             obj.orbitScaleUtil  = theUtil;
%         end

        
        function setFullForceModel(obj, fm)
            obj.fm = fm;
        end
        
        function setTwoBodyForceModel(obj, fm2body)
            obj.fm2body = fm2body;
        end
        function setSpacecraft(obj, sc)
            obj.sc = sc;
        end
    end
end

