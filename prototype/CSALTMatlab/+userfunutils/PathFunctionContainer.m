classdef PathFunctionContainer < userfunutils.FunctionContainer
    
    %PATHFUNCTIONCONTAINER is a container class for data structures passed
    %  to UserPathFunctions and returned containing user data.  This class
    %  basically helps keep the user interfaces clean as the amount of I/O
    %  data increases by bundling all data into a single class.
    
    properties
        % Pointer  to UserFunctionData_Path.
        dynData
    end
    
    methods
        
        %  Intialize the object
        function Initialize(obj)
            costDataObj = userfunutils.FunctionOutputData();
            algDataObj  = userfunutils.FunctionOutputData();
            dynDataObj  = userfunutils.FunctionOutputData();
            UpdateFunctionPointers(obj,dynDataObj,costDataObj,algDataObj)
        end
        
        % Updates pointers to desired data classes
        function UpdateFunctionPointers(obj,dynData,costData,algData)
            obj.dynData = dynData;
            obj.costData = costData;
            obj.algData = algData;
        end
        
    end
    
end

