classdef QuaternionSegment < AttitudeEphemerisSegment
    
    %QUATERNIONSEGMENT A segment of type quaternion of a CCSDS AEM file.
        
    properties (Access = public)
        
        %  Required Metadata contents
        QUATERNION_TYPE
        % Array containing time and quaternion data
        Data
        % length of the data array
        lengthData = 0;
        
    end
    
    properties (Access = private)
                
    end
    
    methods
        
        %  Sets up fields unique to QUATERNION type
        InitializeUniqueFields(obj,struct)

        %  Adds a new row of data to the segment give line from parser
        function  AddData(obj,line)
            
            %  Parse data row and validate
            dataLine = sscanf(line, '%s %f %f %f %f');
            epochStr = char(dataLine(1:end-4)');
            epoch = ParseEphemEpoch(obj,epochStr);
            state = dataLine(end-3:end)';
            
            %  Append the data
            obj.lengthData = obj.lengthData + 1;
            obj.Data(obj.lengthData,1)   = epoch;
            obj.Data(obj.lengthData,2:5) = state;
            
        end
        
    end
    
end
