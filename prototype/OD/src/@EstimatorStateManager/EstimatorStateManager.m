classdef EstimatorStateManager < StateManager

    %----------------------------------------------------------------------
    %  Define the object properties
    %----------------------------------------------------------------------

    %----- Set the public properties
    properties  (SetAccess = 'public')

    end

    %----- Set the public properties
    properties  (SetAccess = 'private')
        Measurements = {};
        ObjectMap    = Map;
    end

    %----------------------------------------------------------------------
    %  Define the object's methods
    %----------------------------------------------------------------------

    methods

        %----- The constructor
        function ESM = EstimatorStateManager(ESM)
            ParamIds{1}  = [];
            ESM.ParamIds = ParamIds;
        end % EstimatorStateManager


        %----- Initialize the state manager
        function ESM = Initialize(ESM,Sandbox,Estimator)

            %=============================================================
            %=============================================================
            %  - Set handles for objects that are being estimated
            %  - Set paramId's for all properties being estimated
            %  - Set chunkSizes for propagator state vector
            %  - Set chunkSizes for estimator state vector
            %=============================================================
            %=============================================================

            %----- Loop over the solve-for parameters and add required
            %      items to Objects, ParamIds, PropObjects, and propParamIds.
            %      Also set up stateChunks arrays.
            ESM.numStates = 0;
            c = 0;
            for i = 1:size(Estimator.SolveFor,2)

                %-----  Extract names of object and property
                solveFor           = Estimator.SolveFor{i};
                [objName,propName] = ParseSolveFor(solveFor);

                %-----  Get handle from SandBox for the current object
                solveObj     = Sandbox.GetHandle(objName);
                objType      = class(solveObj);
                paramId      = solveObj.GetParamId(propName);
                stateChunk   = solveObj.GetState(paramId);
                ESM.numStates    = ESM.numStates + size(stateChunk,1);
                if isempty(paramId)
                    display(['The solve-for parameter "' solveFor '" is not supported'])
                    stop
                end
                         
                %------------------------------------------------------
                %----- Estimator related items.
                %------------------------------------------------------

                % Part I:  Add object to list if not already there.
                isDuplicate = 0;
                objIndex    = 1;
                while  ~isDuplicate & objIndex <= ESM.numObjects
                    if isequal(ESM.Objects{objIndex},solveObj)
                        isDuplicate = 1;
                    end
                    objIndex = objIndex + 1;
                end
                if ~isDuplicate
                    %  Add objects
                    ESM.numObjects              = ESM.numObjects + 1;
                    ESM.Objects{ESM.numObjects} = solveObj;
                    
                    %  This is a work-around for the prototype to handle
                    %  the fact that Matlab doesn't support pointers.  The
                    %  ObjectMap should not be needed in GMAT I suspect.
                    %  ... SPH.
                    ESM.ObjectMap.Add(solveObj,objName);   
                    
                    %  Add another column to ParamIds and subStateSizes
                    ESM.ParamIds{ESM.numObjects} = [];
                    ESM.subStateSizes{ESM.numObjects} = [];
                end

                % Part II: Add parameter to list if not already there.
                isDuplicate = 0;
                paramIndex  = 1;
                while  ~isDuplicate  & paramIndex <= size(ESM.ParamIds{objIndex},2)
                    if isequal(ESM.ParamIds{objIndex}(paramIndex),paramId)
                        isDuplicate = 1;
                    end
                    paramIndex = paramIndex + 1;
                end
                if ~isDuplicate
                    ESM.ParamIds{objIndex}(paramIndex) = paramId ;
                    ESM.subStateSizes{objIndex}(paramIndex) = size(stateChunk,1);
                end

            end % i = 1:size(obj.SolveFor,2)
            
            %  Assemble the ObjectVector and paramIdVector for later use
            c = 0;
            for i = 1:ESM.numObjects
                for j = 1:size(ESM.ParamIds{i},2)
                    c = c + 1;
                    ESM.ObjectsVector{c}  = ESM.Objects{i};
                    ESM.paramIdsVector{c} = ESM.ParamIds{i}(j);
                end
            end

        end % obj = Initialize(obj,Sandbox)
        
        %----- GetSTM
        function STM = GetSTM(ESM)
            
            STM   = zeros(ESM.numStates);
            count = 1;
            for i = 1:ESM.numObjects
                for j = 1:size(ESM.ParamIds{i})
                   chunkSize = ESM.subStateSizes{i}(j);
                   STMchunk = ESM.Objects{i}.GetSTM(ESM.ParamIds{i}(j));
                   STM(count:count+chunkSize-1,count:count+chunkSize-1) = STMchunk;
                   count = count + chunkSize;
                end
            end
            
        end % GetSTM
     
    end % methods

end % classdef