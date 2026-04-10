classdef UserGuessInterface < handle
    %UserGuess Interface for providing a guess via an object
    
    properties
    end
    
    methods (Abstract)
        GetInitialGuess(obj)
    end
    
end

