classdef UserPathFunction < UserFunction
    
    %  USERPATHFUNCTION is class to handle I/O for optimal control user
    %  path functions.  
    
    properties %(SetAccess = 'private')
        
        %%  State Data
        stateVec
        controlVec
        time
        %  1 if state and control.  2 if state only. 3 if control only.
        pointType 
        
        %%  Arrays of function values
        numAlgFunctions = 0;
        numDynFunctions = 0;
        numIntFunctions = 0;
        
        %%  Flags defining function types
        hasCostFunction = false();
        hasAlgFunctions = false();
        hasDynFunctions = false();
        hasIntFunctions = false();
        
        %%  Number of each type of function
        costFunction = [];
        algFunctions = [];
        dynFunctions = [];
        intFunctions = [];
                
    end
    
    methods
        
        %  Call the user function and determine problem characteristics
        function Initialize(obj)
            
            %  If there is no function set flag and return
            if isempty(obj.functionName)
                obj.hasFunction = false();
                return
            else
                obj.hasFunction = true();
            end
            
            %  Evaluate the function and determine output properties
            obj.Evaluate();
            if ~isempty(obj.costFunction)
                obj.hasCostFunction = true();
            end
            if ~isempty(obj.algFunctions)
                obj.hasAlgFunctions = true();
                obj.numAlgFunctions = length(obj.algFunctions);
            end
            if ~isempty(obj.intFunctions)
                obj.hasIntFunctions = true();
                obj.numAlgFunctions = length(obj.intFunctions);
            end
            if ~isempty(obj.dynFunctions)
                obj.hasDynFunctions = true();
                obj.numDynFunctions = length(obj.dynFunctions);
            end
        end
        
        function SetStateControlTime(obj,stateVec,controlVec,time)
            obj.SetState(stateVec)
            obj.SetControl(controlVec)
            obj.SetTime(time)
        end
        
        function SetState(obj,stateVec)
            obj.stateVec = stateVec;
        end
        
        function SetControl(obj,controlVec)
            obj.controlVec = controlVec;
        end
        
        function SetTime(obj,time)
            obj.time = time;
        end
        
    end
    
end

