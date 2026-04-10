classdef UserPointFunction < UserFunction
    
    %  USERPOINTFUNCTION is class to handle I/O for optimal control user
    %  path functions.  
    
    properties
        
        %  State Data
        initialStateVec
        finalStateVec
        initialControlVec
        finalControlVec
        initialTime
        finalTime
        
        %  Arrays of function values
        numEventFunctions = 0;
        numIntFunctions   = 0;
        
        %  Flags defining function types
        hasCostFunction      = false();
        hasEventFunctions    = false();
        hasIntegralFunctions = false();
        
        %  Number of each type of function
        costFunction   = [];
        eventFunctions = [];
        intFunctions   = [];
        
        %  This is computed by integrating path functions then passed in to
        %  the point function
        intConstraintVec;

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
            
            %  Call the function, then determine what was populated and set
            %  properties accordingly.
            obj.Evaluate();
            if ~isempty(obj.costFunction)
                obj.hasCostFunction = true();
            end
            if ~isempty(obj.eventFunctions)
                obj.hasEventFunctions = true();
                obj.numEventFunctions = length(obj.eventFunctions);
            end
            if ~isempty(obj.intFunctions)
                obj.hasIntegralFunctions = true();
                obj.numIntFunctions = length(obj.intFunctions);
            end
            
        end
        
        %  Set initial state
         function SetInitialState(obj,stateVec)
            obj.initialStateVec = stateVec;
         end
        
         %  Set final state
         function SetFinalState(obj,stateVec)
            obj.finalStateVec = stateVec;
         end
        
         %  Set initial control
        function SetInitialControl(obj,controlVec)
            obj.initialControlVec = controlVec;
        end
        
        % Set State and Control
        function SetStateControlTime(obj,initialStateVec,finalStateVec,...
                initialControlVec)
            obj.SetInitialState(initialStateVec)
            obj.SetFinalState(finalStateVec)
            obj.SetInitialControl(initialControlVec)
        end
       
    end
    
end

