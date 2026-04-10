classdef PropStateManager < StateManager

    % Set the public properties
    properties  (SetAccess = 'private')
        ODEMap
        PSV
        estObjectIndeces
        estParamIndeces
    end

    % Set the methods
    methods

        % Initialize
        function PSM = Initialize(PSM,ESM)

            %==============================================================
            %==============================================================
            %  - Set up the handles to the objects that require propagation
            %  - Add ParamIds for properties being propagated
            %  - Set the subStatesSizes array
            %  - Set up the ODEmap
            %  - Determine number of states
            %  - Save indeces that relate prop objects and paramIds to
            %    estimator objects and paramIds.
            %==============================================================
            %==============================================================

            %----- Determine size of state vector and dimension it accordingly.
            StateCount = 1;
            numObjects = 0;
            
            %----- Loop over the objects involved in estimation
            for i = 1:size(ESM.Objects,2)

                currObj = ESM.Objects{i};
                %----- Loop over the parameters estimated on the ith object
                for j = 1:size(ESM.ParamIds{i},2)

                    %  Determine if the current parameter requires analytic
                    %  or numeric propagation
                    paramId = ESM.ParamIds{i}(j);
                    dynamicsId = currObj.GetDynamicsId(paramId);
                    if dynamicsId <= 200

                        %  Set OD map
                        ODEMap.Size(StateCount) = ESM.subStateSizes{i}(j);
                        ODEMap.Type(StateCount) = ESM.ParamIds{i}(j);
                        PSM.numStates = PSM.numStates + ESM.subStateSizes{i}(j);
                        StateCount = StateCount + 1;

                        %  Check to see if object is already in Object list
                        %  and add it to the list if not there already
                        isDuplicate = 0;
                        objIndex    = 1;
                        while  ~isDuplicate & objIndex <= PSM.numObjects
                            if isequal(PSM.Objects{objIndex},currObj)
                                isDuplicate = 1;
                            end
                            objIndex = objIndex + 1;
                        end
                        if ~isDuplicate
                            %  Add objects
                            PSM.numObjects              = PSM.numObjects + 1;
                            PSM.Objects{PSM.numObjects} = currObj;
                            PSM.estObjectIndeces{objIndex} = i;
                            %  Add another column to ParamIds and subStateSizes
                            PSM.ParamIds{PSM.numObjects} = [];
                            PSM.subStateSizes{PSM.numObjects} = [];
                        end

                        % Part II: Add parameter to list if not already there.
                        isDuplicate = 0;
                        paramIndex  = 1;
                        while  ~isDuplicate  & paramIndex <= size(PSM.ParamIds{objIndex},2)
                            if isequal(PSM.ParamIds{objIndex}(paramIndex),paramId)
                                isDuplicate = 1;
                            end
                            paramIndex = paramIndex + 1;
                        end
                        
                        %  KLUDGE TO HANDLE STM PROPAGATION
                        if ~isDuplicate & paramId == currObj.GetParamId('CartesianState')
                            PSM.ParamIds{objIndex}(paramIndex:paramIndex+1) = [paramId currObj.GetParamId('STM') ];
                            PSM.estParamIndeces{objIndex}(paramIndex)  = j;
                            PSM.subStateSizes{objIndex}(paramIndex:paramIndex+1) = [size(currObj.GetState(paramId),1) 36];
                            ODEMap.Size(StateCount) = 36;
                            ODEMap.Type(StateCount) = 202;
                            PSM.numStates = PSM.numStates + ESM.subStateSizes{i}(j);
                            StateCount = StateCount + 1;
                        end

                    end

                end

            end

            PSM.ODEMap = ODEMap;
            PSM.PSV    = zeros(PSM.numStates);
            PSM.numObjects = 1;   % KLUDGE

        end % Initialize
        
        %  Update objects based on estimator iteration.
        function PSM = UpdateObjects(PSM,ESM)
            
            for i = 1:PSM.numObjects
               PSM.Objects{i} = ESM.Objects{PSM.estObjectIndeces{i}};
            end
            
        end

    end % methods

end % classdef