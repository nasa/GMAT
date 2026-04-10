classdef ImplicitRungeKuttaPhase < exec.Phase
    
    properties (SetAccess = 'protected')
        % string.  The collocation transcription type
        collocationMethod
    end
    
    methods
        function InitializeTranscription(obj)
            %  Initialize the collocation helper class
            import collutils.NLPFuncUtil_ImplicitRungeKutta
            obj.TransUtil = NLPFuncUtil_ImplicitRungeKutta();
            obj.TransUtil.Initialize(obj.Config,obj.collocationMethod);
        end
        function isMeshRefined= RefineMesh(obj,ifUpdateMesh)
            isMeshRefined = true;
            disp('Warning:  Mesh refinement is not implemented for IRK type methods')
        end
        function SetTranscription(obj,type)
            % NOTE: in C++ this should probably be handled via an
            % overloaded constructor, but MATLAB does not let you do that
               obj.collocationMethod = type;
        end
    end
end

