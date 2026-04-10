classdef UserFunctionManager < handle
    
    %  FunctionData class to handle I/O for optimal control user functions
    %
    %  Author: S. Hughes, steven.p.hughes@nasa.gov
    
    properties (Access = 'protected')
        % Pointer to UserFunctData_Path.
        userClass
        % Integer. The phase id number
        phaseNum
        % Flag indicating whether user provided a function
        hasFunction     = false();
        % Integer. Number of state parametesr
        numStateVars
        % Integer. Number of controls
        numControlVars
        % Integer. Number of time variables
        numTimeVars
        % Bool. Flag to indicate testing of user provided derivatives.
        forceFiniteDiff = false()
        
    end
    
    methods
                        
        function vec = HandleGradient(~,vec)
            %  Removes gradient type info if a gradient.  Not necessary in
            %  GMAT! This works around a MATLAB issue and doesn't need to
            %  be migrated to GMAT.
            if isa(vec,'gradient')
                vec = vec.x;
            end
        end
        
        function randVec = GetRandomVector(~,upperBound,lowerBound)
            % Computes random vector given upper and lower bounds
            numVars = size(upperBound,1);
            randVec = zeros(numVars,1);
            for varIdx = 1:numVars
                randVec(varIdx,1) = lowerBound(varIdx) + ...
                    rand*(upperBound(varIdx) - lowerBound(varIdx));
            end
        end
 
    end
    
end

