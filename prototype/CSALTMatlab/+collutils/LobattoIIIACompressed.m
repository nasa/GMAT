classdef LobattoIIIACompressed < collutils.ImplicitRungeKutta
    %UNTITLED9 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Access = protected)
        isSeparated = false;
        funcDepPattern
    end
    
    methods
        function isSeparated = GetIsSeparated(obj)
            isSeparated = obj.isSeparated;
        end
        function funcDepPattern = GetFuncDepPattern(obj)
            funcDepPattern = obj.funcDepPattern;
        end
    end
    
end



