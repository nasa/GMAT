classdef StateManager < handle


    % Set the public properties
    properties  (SetAccess = 'public')
        Objects    = {};  %  This should be moved to private and scoping issue fixed
    end

    % Set the public properties
    properties  (SetAccess = 'protected')
        ParamIds      = {};
        subStateSizes = {};
        numObjects    =  0;
        numStates     =  0;
        ObjectsVector  = {};
        paramIdsVector = [];
    end

    % Set the methods
    methods
        
        %----- GetStates
        function X = GetStates(StateManager)
            
            xIndex = 1;
            
            %----- Loop over the objects 
            for i = 1:size(StateManager.Objects,2)

                currObj = StateManager.Objects{i};

                %----- Loop over the parameters 
                for j = 1:size(StateManager.ParamIds{i},2)
                       X(xIndex:xIndex+StateManager.subStateSizes{i}(j)-1,1) = ...
                          currObj.GetState(StateManager.ParamIds{i}(j));
                      xIndex = xIndex + StateManager.subStateSizes{i}(j);
                end % j = 1:size(StateManager.ParamIds{i},2)

            end % i = 1:size(StateManager.Objects,2)
 
        end % GetStates
        

        %----- SetStates
        function StateManager = SetStates(StateManager, X, epoch)

            xIndex = 1;
            
            %----- Loop over the objects 
            for i = 1:size(StateManager.Objects,2)

                currObj = StateManager.Objects{i};
                if nargin == 3
                   StateManager.Objects{i}.Epoch = epoch;
                end
                
                %----- Loop over the parameters 
                for j = 1:size(StateManager.ParamIds{i},2)
                      chunkSize = StateManager.subStateSizes{i}(j);
                      x         = X(xIndex:xIndex+chunkSize-1);
                      currObj   = currObj.SetState(StateManager.ParamIds{i}(j),x);
                      xIndex    = xIndex + chunkSize;
                end % j = 1:size(StateManager.ParamIds{i},2)
                
                StateManager.Objects{i} = currObj;

            end % i = 1:size(StateManager.Objects,2)

        end % SetStates
        
        %----- CloneObjects
        function estObjClone = CloneObjects(Manager)
            
            %  Step through the objects and call their copy constructors
            for i = 1:Manager.numObjects;
                
                %  Extract the current object
                currObj  = Manager.Objects{i};
                
                %  Determine the class type of the object
                objClass = class(currObj);
                
                %  Call the copy constructor
                estObjClone{i} = feval(objClass,currObj);
                
            end
            
        end
        
        %----- Copy Objects
        function estObjClone = CopyObjects(ESM,Objects)
            
            %  Step through the objects and call their copy constructors
            numObjects = size(Objects,2);
            for i = 1:numObjects;
                
                %  Extract the current object
                currObj  = Objects{i};
                
                %  Determine the class type of the object
                objClass = class(currObj);
                
                %  Call the copy constructor
                estObjClone{i} = feval(objClass,currObj);
                
            end
            
        end

    end % methods

end % classdef