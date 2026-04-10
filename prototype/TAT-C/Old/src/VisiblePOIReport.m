classdef VisiblePOIReport < VisiblityReport
    %VISIBLEPOIREPORT. Container for data on POI interval event
    
    properties
        % Integer. Index of point of interest
        poiIndex
        % KeyValueStatistics Object. Data on Observatory zenith
%         ObsZenith
%         % KeyValueStatistics Object. Data on Observatory azimuth
%         ObsAzimuth
%         % KeyValueStatistics Object. Data on Observatory range
%         ObsRange
%         % KeyValueStatistics Object. Data on sun zenith
%         SunZenith
%         % KeyValueStatistics Object. Data on sun azimuth
%         SunAzimugh
%         % KeyValueStatistics Object. Lunar phase
    end
    
    methods
        function SetPOIIndex(obj,index)
           % Sets index of point of interest
           obj.poiIndex = index; 
        end
%         function SetObsZenithData(obj,minValue,maxValue,avgValue)
%             % Sets observatory zenith statistics
%             obj.ObsZenith = KeyValueStatistics(minValue,maxValue,avgValue);
%         end
%         function SetObsAzimuthData(obj,minValue,maxValue,avgValue)
%             % Sets observatory azimuth data
%             obj.ObsAzimuth = KeyValueStatistics(minValue,maxValue,avgValue);
%         end
%         function ObsZenith = GetObsZenithData(obj)
%             % Sets observatory zenith statistics
%             ObsZenith = obj.ObsZenith;
%         end
%         function ObsAzimuth = GetObsAzimuthData(obj)
%             % Sets observatory azimuth data
%             ObsAzimuth = obj.ObsAzimuth;
%         end
    end
    
end

