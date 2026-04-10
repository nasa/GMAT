classdef FunctionInputData < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = 'protected')
        % Real. Number of state variables
        numStateVars
        %  Real array.  State vector
        stateVec
        % Real.  Time
        time
        % Integer.  Phase number Id
        phaseNum
        %  Bool.  Flag indicating if perturbation call
        isPerturbing = false();
        % Bool. Flag indicating if sparsity call
        isSparsity = false();
    end
    
    properties (Access = 'private')
        % Real. Number of control variables
        numControlVars
        % Real array.  The control vector
        controlVec
    end
    
    methods (Access = 'public')
                
        function SetStateVec(obj,stateVec)
            %  Sets the state vector
            obj.stateVec = stateVec;
        end
        
        function stateVec = GetStateVec(obj)
            %  Returns the state vector
            stateVec = obj.stateVec;
        end
        
        function SetPhaseNum(obj,phaseNum)
            % Sets the phase number
            obj.phaseNum = phaseNum;
        end
        
        function phaseNum = GetPhaseNum(obj)
            % Returns the phase number
            phaseNum = obj.phaseNum;
        end
        
        function SetIsPerturbing(obj,isPerturbing)
            %  Sets flag for perturbing or not
            obj.isPerturbing = isPerturbing;
        end
        
        function isPerturbing = GetIsPerturbing(obj)
            %  Returns isPertubing flag
            isPerturbing = obj.isPerturbing;
        end
        
        function SetIsSparsity(obj,isSparsity)
            %  Sets flag for perturbing for sparsity or not
            obj.isSparsity = isSparsity;
        end
        
        function isSparsity = GetIsSparsity(obj)
            %  Returns isSparsity flag
            isSparsity = obj.isSparsity;
        end
        
        function SetTime(obj,time)
            %  Sets the time
            obj.time = time;
        end
        
        function time = GetTime(obj)
            %  Returns the time
            time = obj.time;
        end
        
        function numStateVars = GetNumStateVars(obj)
            %  Gets the number of state vars
            numStateVars = obj.numStateVars;
        end
       
        function Initialize(obj,numStateVars,numControlVars)
            %  Initializes the class
            obj.numStateVars = numStateVars;
            obj.numControlVars = numControlVars;
        end

        function SetControlVec(obj,controlVec)
            %  Sets the control vector
            obj.controlVec = controlVec;
        end
        
        function controlVec = GetControlVec(obj)
            % Returns the control vector
            controlVec = obj.controlVec;
        end
                
        function numControlVars = GetNumControlVars(obj)
            %  Gets the number of control vars
            numControlVars = obj.numControlVars;
        end
        
    end
    
end

