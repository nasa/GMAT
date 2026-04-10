classdef FunctionContainer < handle
    
    %PATHFUNCTIONCONTAINER is a container class for data structures passed
    %  to UserPathFunctions and returned containing user data.  This class
    %  basically helps keep the user interfaces clean as the amount of I/O
    %  data increases by bundling all data into a single class.
    
    properties
        % Pointer  to FunctionOutputData object.
        costData
        % Pointer  to FunctionOutputData object.
        algData
    end
    
    methods
        
    end
    
end

