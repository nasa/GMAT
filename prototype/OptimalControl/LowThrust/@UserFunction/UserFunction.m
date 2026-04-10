classdef UserFunction < handle
    
    %  FunctionData class to handle I/O for optimal control user functions
    %
    %  Author: S. Hughes, steven.p.hughes@nasa.gov
    
    properties (SetAccess = 'public')
        %  Name of the user function
        functionName
    end
    
    properties %(SetAccess = 'private')
        
        %  Miscellaneous parameters
        phaseNum
        
        %  Flag indicating whether user provided a function
        hasFunction     = false();
        
    end
    
    methods
        
        function Evaluate(obj)
            if obj.hasFunction
                feval(obj.functionName,obj);
            end
        end
        
    end
    
end

