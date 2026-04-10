classdef RadauPhase < exec.Phase
        
    properties (SetAccess = 'protected')
        
    end
    
    methods
        
        function InitializeTranscription(obj)
            %  Initialize the collocation helper class
            import collutils.NLPFuncUtil_Radau
            obj.TransUtil = NLPFuncUtil_Radau();
            %  Note in GMAT Config should be a pointer to the config objects
            obj.TransUtil.Initialize(obj.Config);
            obj.SetRelErrorTol(obj.relErrorTol);
            obj.SetInitRelErrorTol(obj.initRelErrorTol);
        end
        
        function [isMeshRefined , newMeshIntervalNumPoints, newMeshIntervalFractions, ...
                newStateArray,newControlArray,maxRelErrorInMesh] = RefineMesh(obj,ifUpdateMesh)
            %  Computes new mesh intervals and orders, state and control guesses
            obj.isRefining = true;
            [isMeshRefined , newMeshIntervalNumPoints, newMeshIntervalFractions, ...
                newStateArray,newControlArray,maxRelErrorInMesh] = ...
                obj.TransUtil.RefineMesh(obj.DecVector,obj.pathFuncInputData,obj.PathFuncManager);
            if ~isMeshRefined && ifUpdateMesh
                obj.SetMeshIntervalFractions(newMeshIntervalFractions);
                obj.SetMeshIntervalNumPoints(newMeshIntervalNumPoints);
            end
        end
    end
end
