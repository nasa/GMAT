classdef PointFunctionContainer < userfunutils.FunctionContainer
    
    %PATHFUNCTIONCONTAINER is a container class for data structures passed
    %  to UserPathFunctions and returned containing user data.  This class
    %  basically helps keep the user interfaces clean as the amount of I/O
    %  data increases by bundling all data into a single class.
    
    properties

    end
    
    methods
        
        %  Intialize the object
        function Initialize(obj)
            costData = userfunutils.FunctionOutputData();
            algData  = userfunutils.FunctionOutputData();
            UpdateFunctionPointers(obj,costData,algData)
        end
        
        % Updates pointers to desired data classes
        function UpdateFunctionPointers(obj,costData,algData)
            obj.costData = costData;
            obj.algData = algData;
        end
        
    end
    
end