classdef STKTrajectory < userfunutils.TrajectoryData
    %STKTrajectory Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        fileNameAndPath = '';
    end
    
    methods
        
        function obj = STKTrajectory(fileNameAndPath)
            obj.fileNameAndPath = fileNameAndPath;
            obj.numControlParams = 0;
            obj.numIntegralParams = 0;
            obj.numStateParams = 7;
            
            import ephemfileutils.STKEphemeris
            ephFile = STKEphemeris();
            ephFile.EphemerisFile = obj.fileNameAndPath;
            ephFile.ParseEphemeris() 
            
           % obj.SetStateGuessArrays()
            
        end
        
    end
    
end

