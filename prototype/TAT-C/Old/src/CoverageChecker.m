classdef CoverageChecker < handle
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % PointGroup object. The points to use for coverage
        pointGroup
        % Pointer to Spacecraft object. The spacecraft for coverage computations
        spaceCraft
        % Earth object. The model of earth's properties and rotation
        centralBody
        % Integer. The number of accumulated propagation data points.
        timeIdx
        % std:vec of std:vec of integers storing times when points are visible
        timeSeriesData
        % std:vec of reals. Contains the date of each propagation point
        dateData
        % Array of ints. Contains the number of prop. times when each
        % point was visible
        numEventsPerPoint
    end
    
    methods
        
        function obj = CoverageChecker(pointGroup,spaceCraft)
            % Constructor.
            obj.pointGroup = pointGroup;
            obj.spaceCraft = spaceCraft;
            obj.centralBody = Earth();
            obj.timeSeriesData = cell(1,obj.pointGroup.GetNumPoints());
            obj.dateData = cell(1,obj.pointGroup.GetNumPoints());
            obj.timeIdx = 0;
            obj.numEventsPerPoint = zeros(1,obj.pointGroup.GetNumPoints());
        end
        
        function vec = CheckPointCoverage(obj)
            % Check coverage given a spacecraft location in body fixed coordinates
            currentDate = obj.spaceCraft.GetJulianDate();
            bodyFixedState = obj.GetEarthFixedSatState(currentDate);
            covCount = 0;
            vec = [];
            for pointIdx = 1:obj.pointGroup.GetNumPoints()
                % Compute range vector
                pointVec = obj.pointGroup.GetPointPositionVector(pointIdx);
                rangeVec = bodyFixedState - pointVec;
                % Check in view
                dotProd = rangeVec'*pointVec;
                % Simple line of site test
                if dotProd > 0
                    covCount = covCount + 1;
                    vec(covCount) = pointIdx; %#ok<AGROW>
                    obj.numEventsPerPoint(pointIdx) = obj.numEventsPerPoint(pointIdx) + 1;
                    obj.timeSeriesData{pointIdx,obj.numEventsPerPoint(pointIdx)} = obj.timeIdx;
                end
            end
            
        end
        
        function vec = AccumulateCoverageData(obj)
            %  Accumulates coverage data after propagation update
            obj.timeIdx = obj.timeIdx + 1;
            obj.dateData{obj.timeIdx} = obj.spaceCraft.GetJulianDate();
            vec = CheckPointCoverage(obj);
        end
        
        function bodyFixedState = GetEarthFixedSatState(obj,jDate)
            % Converts state from Earth interial to Earth fixed
            inertialState = obj.spaceCraft.GetCartesianState();
            %  TODO.  Handle differences in units of points and states.
            bodyFixedState = obj.centralBody.GetBodyFixedState(...
                inertialState(1:3,1),jDate)/6378.1373;
        end
        
        function coverageEvents = ProcessCoverageDate(obj)
            coverageEvents = {};
            numCoverageEvents = 0;
            
            for pointIdx = 1:obj.pointGroup.GetNumPoints()
                %  Only perform if there are interval events (2 or more events)
                isEnd = false;
                if obj.numEventsPerPoint(pointIdx) >= 2
                    numEvents = obj.numEventsPerPoint(pointIdx);
                    startTime = obj.dateData{obj.timeSeriesData{pointIdx,1}};
                    for dateIdx = 2:numEvents
                        %  Test for end of an interval
                        if (obj.timeSeriesData{pointIdx,dateIdx} - ...
                                obj.timeSeriesData{pointIdx,dateIdx - 1} ~= 1)
                            endTime = obj.dateData{obj.timeSeriesData{pointIdx,dateIdx-1}};
                            isEnd = true;
                        % Test for the last event for this point
                        elseif dateIdx == numEvents
                            endTime = obj.dateData{obj.timeSeriesData{pointIdx,dateIdx}};
                            isEnd = true;
                        end
                        if isEnd
                            poiReport = obj.CreateNewPOIReport(startTime,endTime,pointIdx);
                            numCoverageEvents = numCoverageEvents + 1;
                            coverageEvents{numCoverageEvents} = poiReport; %#ok<AGROW>
                            startTime = obj.dateData{obj.timeSeriesData{pointIdx,dateIdx}};
                            isEnd = false;
                        end
                    end
                end
            end
        end
        
        function poiReport = CreateNewPOIReport(~,startJulianDate,endJulianDate,poiIndex)
            % Creates VisiblePOIReport given poind indeces and start/end dates
            poiReport = VisiblePOIReport();
            poiReport.SetPOIIndex(poiIndex);
            startEpoch = AbsoluteDate();
            startEpoch.SetJulianDate(startJulianDate);
            endEpoch = AbsoluteDate();
            endEpoch.SetJulianDate(endJulianDate);
            poiReport.SetStartDate(startEpoch);
            poiReport.SetEndDate(endEpoch);
        end
    end
end
