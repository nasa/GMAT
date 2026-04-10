classdef EOPFileWriter < handle
    %EOPFileWriter Utility for creating EOP files
    %   
    %  This class writes EOP files by combining historical and predicted
    %  data.  The historical data must be in the format descibed here:
    %         https://datacenter.iers.org/eop/-/somos/5Rgv/
    %         getMeta/223/eopc04_14.62-now
    %  Predicted data must be in the format described here
    %         https://celestrak.com/SpaceData/EOP-format.asp
    %
    %  Usage Example:
    %
    %      % Write an EOP file to the current MATLAB directory
    %      filePath = pwd;
    %      fileName = 'HistoricalAndPredictedEOP.txt';
    %      fReader = EOPFileWriter(filePath,fileName);
    %      fReader.WriteFile();
    %
    %  NOTE: This class downloads large files (~10MB). The files are
    %  cleaned up after execution, but expect slow run times if using slow
    %  network connection. 
    %
    %  NOTE: The class defaults point to files updated daily that that meet  
    %  formating requirements as of the time of writing.  The Public Set 
    %  methods allow users to  change locations for future applications 
    %  if needed:
    %  SetHistoricalDataLocation(obj,nameAndLocation)
    %  SetPredictedDataLocation(obj,nameAndLocation)
    %
    %  Author: S. Hughes (NASA/GSFC)
    %  Created: 2017/02/21
    
    properties (Access = 'private')
        
        % String. The location of the historical data EOP file
        historicalDataLocation = ['https://datacenter.iers.org/eop'...
            '/-/somos/5Rgv/latest/223'];
        % String. The location of the historical data EOP file
        predictedDataLocation = ['http://celestrak.com/SpaceData/' ....
            'eop19620101.txt'];
        % String. Path to the new EOP file
        filePath
        % String. Name of the new EOP file
        eopFileName
        % String. Name for temporary historical data file
        tempHistoricalFile
        % String. Name for temporary predicted data file
        tempPredictedFile
        % Cell array containing the raw historical data
        rawHistoricalData
        % Cell array containing the raw predicted data
        rawPredictedData
        % String. Concatenated eopFileName and filePath
        fileNameAndPath
        % Cell array of header text read from historical file
        headerText
    end
    
    methods (Access = 'public')
        
        function obj = EOPFileWriter(filePath,eopFileName)
            % Writes EOP file to location in filePath to eopFileName
            
            % Replace "\" with "/" for cross platform functionality
            for locIdx = 1:length(filePath)
                if strcmp(filePath(locIdx),'\')
                    filePath(locIdx) = '/';
                end
            end
            % Make sure there is a slash at the end of the path
            if ~strcmp(filePath(end),'/')
                filePath(end+1) = '/';
            end
            obj.filePath = filePath;
            obj.eopFileName = eopFileName;
            obj.tempHistoricalFile = [filePath 'histData.txt'];
            obj.tempPredictedFile = [filePath 'predData.txt'];
            obj.fileNameAndPath = [filePath eopFileName];
        end
        
        function SetHistoricalDataLocation(obj,nameAndLocation)
            % Sets location of historical data using http protocol
            % Data must be in IERS format described here:
            % https://datacenter.iers.org/eop/-/somos/5Rgv/
            % getMeta/223/eopc04_14.62-now
            obj.historicalDataLocation = nameAndLocation;
        end
        
        function SetPredictedDataLocation(obj,nameAndLocation)
            % Sets location of predicted data using http protocol
            % Data must be in format described here
            % https://celestrak.com/SpaceData/EOP-format.asp
            obj.predictedDataLocation = nameAndLocation;
        end
        
        function WriteFile(obj)
            % Retrieves data, combines, and writes to specified file
            obj.SaveHistoricalDataFile();
            obj.SavePredictedDataFile();
            obj.ParseHistoricalFile()
            obj.ParsePredictedFile()
            obj.WriteData()
            obj.DeleteTempFiles()
        end
        
    end
    
    methods (Access = 'private')
        
        function SaveHistoricalDataFile(obj)
            % Uses websave() to download data from historical date loc.
            options = weboptions('Timeout',Inf);
            websave(obj.tempHistoricalFile,...
                obj.historicalDataLocation,options);
        end
        
        function SavePredictedDataFile(obj)
            % Uses websave() to download data from predicted date loc.
            options = weboptions('Timeout',Inf);
            websave(obj.tempPredictedFile,...
                obj.predictedDataLocation,options);
        end
        
        function DeleteTempFiles(obj)
            % Deletes temporary predicted and historical files
            delete(obj.tempHistoricalFile);
            delete(obj.tempPredictedFile);
        end
        
        function ParseHistoricalFile(obj)
            % Parse the historical data file and populates data arrays
            linesOfData = 0;
            fileHandle = fopen(obj.tempHistoricalFile);
            dataParsing = false;
            headerLineCount = 0;
            while ~feof(fileHandle)
                line = fgetl(fileHandle);
                % Save the header lines for writing out later
                if ~dataParsing
                    headerLineCount = headerLineCount + 1;
                    obj.headerText{headerLineCount} = line;
                end
                if isempty(line)
                    continue
                end
                if length(line) < 4
                    continue
                end
                if strcmp(line(1:4),'1962')
                    dataParsing = true;
                end
                if dataParsing
                    linesOfData = linesOfData + 1;
                    formatSpec = ['%4d %4d %4d %7d %11.6f %11.6f' ...
                        ' %12.7f %12.7f %11.6f %11.6f %11.6f %11.6f' ...
                        ' %11.7f %11.7f %12.6f %12.6f'];
                    data = textscan(line,formatSpec);
                    obj.AccumulateHistoricalData(data,linesOfData)
                    
                end
            end
            fclose(fileHandle);
        end
        
        function AccumulateHistoricalData(obj,dataStruct,lineNumber)
            % Adds new record into the historical data arrays
            obj.rawHistoricalData.Year(lineNumber) = dataStruct{1};
            obj.rawHistoricalData.Month(lineNumber) = dataStruct{2};
            obj.rawHistoricalData.Day(lineNumber) = dataStruct{3};
            obj.rawHistoricalData.MJD(lineNumber) = dataStruct{4};
            obj.rawHistoricalData.xPM(lineNumber) = dataStruct{5};
            obj.rawHistoricalData.yPM(lineNumber) = dataStruct{6};
            obj.rawHistoricalData.dUT1(lineNumber) = dataStruct{7};
            obj.rawHistoricalData.LOD(lineNumber) = dataStruct{8};
            obj.rawHistoricalData.dPsi(lineNumber) = dataStruct{9};
            obj.rawHistoricalData.dEps(lineNumber) = dataStruct{10};
            obj.rawHistoricalData.xErr(lineNumber) = dataStruct{11};
            obj.rawHistoricalData.yErr(lineNumber) = dataStruct{12};
            obj.rawHistoricalData.dUT1Error(lineNumber) = dataStruct{13};
            obj.rawHistoricalData.LODError(lineNumber) = dataStruct{14};
            obj.rawHistoricalData.dPsiError(lineNumber) = dataStruct{15};
            obj.rawHistoricalData.dEpsError(lineNumber) = dataStruct{16};
        end
        
        function ParsePredictedFile(obj)
            % Parse the predicted data file and populates data arrays
            linesOfData = 0;
            fileHandle = fopen(obj.tempPredictedFile);
            dataParsing = false;
            while ~feof(fileHandle)
                line = fgetl(fileHandle);
                if ~dataParsing
                    if strfind(line,'BEGIN OBSERVED')
                        dataParsing = true;
                        continue
                    end
                end
                if dataParsing
                    if ~isempty(strfind(line,'END OBSERVED'))|| ...
                            ~isempty(strfind(line, 'NUM_PREDICTED_POINTS')) || ...
                            isempty(line) || ...
                            ~isempty(strfind(line,'BEGIN PREDICTED'))
                        continue
                    end
                    if strfind(line,'END PREDICTED')
                        fclose(fileHandle);
                        return
                    end
                    linesOfData = linesOfData + 1;
                    % I4,I3,I3,I6,2F10.6,2F11.7,4F10.6,I4
                    formatSpec = ['%4d %3d %3d %6d %10.6f %10.6f' ...
                        ' %11.7f %11.7f %10.6f %10.6f %10.6f' ...
                        ' %10.6f %4d'];
                    data = textscan(line,formatSpec);
                    obj.AccumulatePredictedData(data,linesOfData)
                    
                end
            end
        end
        
        function AccumulatePredictedData(obj,dataStruct,lineNumber)
            % Addes new record into predicted data arrays
            obj.rawPredictedData.Year(lineNumber) = dataStruct{1};
            obj.rawPredictedData.Month(lineNumber) = dataStruct{2};
            obj.rawPredictedData.Day(lineNumber) = dataStruct{3};
            obj.rawPredictedData.MJD(lineNumber) = dataStruct{4};
            obj.rawPredictedData.xPM(lineNumber) = dataStruct{5};
            obj.rawPredictedData.yPM(lineNumber) = dataStruct{6};
            obj.rawPredictedData.dUT1(lineNumber) = dataStruct{7};
            obj.rawPredictedData.LOD(lineNumber) = dataStruct{8};
            obj.rawPredictedData.dPsi(lineNumber) = dataStruct{9};
            obj.rawPredictedData.dEps(lineNumber) = dataStruct{10};
            obj.rawPredictedData.dxPM(lineNumber) = dataStruct{11};
            obj.rawPredictedData.dyPM(lineNumber) = dataStruct{12};
            obj.rawPredictedData.DAT(lineNumber) = dataStruct{13};
        end
        
        function WriteData(obj)
            % Writes EOP data to the file
            fileHandle = fopen(obj.fileNameAndPath,'w+');
            obj.WriteFileHeader(fileHandle);
            obj.WriteHistoricalData(fileHandle);
            obj.WritePredictedData(fileHandle);
        end
        
        function WriteHistoricalData(obj,fileHandle)
            % Writes historical data to the EOP file
            
            %  Find the last MJD on the historical data
            formatSpec = ['%4d%4d%4d%7d%11.6f%11.6f' ...
                '%12.7f%12.7f%11.6f%11.6f%11.6f%11.6f' ...
                '%11.7f%11.7f%12.6f%12.6f\n'];
            for lineNumber = 1:length(obj.rawHistoricalData.Year)
                fprintf(fileHandle,formatSpec,...
                    obj.rawHistoricalData.Year(lineNumber),...
                    obj.rawHistoricalData.Month(lineNumber),...
                    obj.rawHistoricalData.Day(lineNumber),...
                    obj.rawHistoricalData.MJD(lineNumber),...
                    obj.rawHistoricalData.xPM(lineNumber),...
                    obj.rawHistoricalData.yPM(lineNumber),...
                    obj.rawHistoricalData.dUT1(lineNumber),...
                    obj.rawHistoricalData.LOD(lineNumber),...
                    obj.rawHistoricalData.dPsi(lineNumber),...
                    obj.rawHistoricalData.dEps(lineNumber),...
                    obj.rawHistoricalData.xErr(lineNumber),...
                    obj.rawHistoricalData.yErr(lineNumber),...
                    obj.rawHistoricalData.dUT1Error(lineNumber),...
                    obj.rawHistoricalData.LODError(lineNumber),...
                    obj.rawHistoricalData.dPsiError(lineNumber),...
                    obj.rawHistoricalData.dEpsError(lineNumber));
            end
        end
        
        function WritePredictedData(obj,fileHandle)
            % Writes predicted EOP data to the file
            lastEpoch = obj.rawHistoricalData.MJD(end);
            count = 1;
            while obj.rawPredictedData.MJD(count) < lastEpoch
                count = count + 1;
            end
            rowPrintStart = count + 1;
            formatSpec = ['%4d%4d%4d%7d%11.6f%11.6f' ...
                '%12.7f%12.7f%11.6f%11.6f%11.6f%11.6f\n'];
            for lineNumber = rowPrintStart:length(obj.rawPredictedData.MJD)
                fprintf(fileHandle,formatSpec,...
                    obj.rawPredictedData.Year(lineNumber),...
                    obj.rawPredictedData.Month(lineNumber),...
                    obj.rawPredictedData.Day(lineNumber),...
                    obj.rawPredictedData.MJD(lineNumber),...
                    obj.rawPredictedData.xPM(lineNumber),...
                    obj.rawPredictedData.yPM(lineNumber),...
                    obj.rawPredictedData.dUT1(lineNumber),...
                    obj.rawPredictedData.LOD(lineNumber),...
                    obj.rawPredictedData.dPsi(lineNumber),...
                    obj.rawPredictedData.dEps(lineNumber),...
                    obj.rawPredictedData.dxPM(lineNumber),...
                    obj.rawPredictedData.dyPM(lineNumber));
            end
        end
        
        function WriteFileHeader(obj,fileHandle)
            % Writes EOP header to the file
            % TODO: fix this kludge to removed first lind of data from
            % the header array.
            for rowIdx = 1:length(obj.headerText)-1
                fprintf(fileHandle,'%s\n',obj.headerText{rowIdx});
            end
        end
        
    end
    
end

