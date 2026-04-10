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
        %
        pointArray
        feasibilityTest
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
            obj.pointArray = zeros(obj.pointGroup.GetNumPoints(),3);
            for pointIdx = 1:obj.pointGroup.GetNumPoints()
                vec = obj.pointGroup.GetPointPositionVector(pointIdx);
                obj.pointArray(pointIdx,:) = vec/norm(vec);
            end
            obj.feasibilityTest = zeros(obj.pointGroup.GetNumPoints(),1);
            
        end
        
        function vec = CheckPointCoverage(obj)
            % Check coverage given a spacecraft location in body fixed coordinates
            currentDate = obj.spaceCraft.GetJulianDate();
            bodyFixedState = obj.GetEarthFixedSatState(currentDate);
            covCount = 0;
            vec = [];
            obj.CheckGridFeasibility(bodyFixedState(1:3,1));
            for pointIdx = 1:obj.pointGroup.GetNumPoints()
                
                % Simple line of site test
                if obj.feasibilityTest(pointIdx) > 0
                    sensorNum = 1;
                    % Compute the coordinates in the sensor frame
                    %  TODO: this assumes bore site of sensor is nadir pointing
                    satToTargetVec= obj.pointGroup.GetPointPositionVector(pointIdx) - bodyFixedState(1:3,1);
                    zHat = -bodyFixedState(1:3,1);
                    zHat = zHat/norm(zHat);
                    xHat = -cross(zHat,bodyFixedState(4:6,1));
                    xHat = xHat/norm(xHat);
                    yHat = cross(zHat,xHat);
                    R_fixed_to_nadir = [xHat yHat zHat]';
                    viewVector = R_fixed_to_nadir*satToTargetVec;
                    targetRA = atan2(viewVector(2),viewVector(1));
                    targetDEC = asin(viewVector(3)/norm(viewVector));
                    targetConeAngle =  pi/2 - targetDEC;
                    
                    if obj.spaceCraft.GetHasSensors()
                        %  Call sensor model
%                         if pointIdx == 1
%                             figure(102)
%                             obj.spaceCraft.sensorList{sensorNum}.PlotTargetGeometry(targetConeAngle,targetRA)
%                         end
                        inView = obj.spaceCraft.CheckTargetVisibility(targetConeAngle,...
                            targetRA,sensorNum);
                    else
                        % No sensor, just perform horizon test
                        rangeVec = -satToTargetVec;
                        cosineOffNadirAngle = (rangeVec')*bodyFixedState(1:3,1)/...
                            norm(rangeVec)/norm(bodyFixedState(1:3,1));
                        offNadirAngle = acos(cosineOffNadirAngle);
                        if offNadirAngle < pi/2 - acos(6378.1363/norm(bodyFixedState(1:3,1))) && rangeVec(3) > 0
                            inView = true();
                        else
                            inView = false();
                        end
                    end
%                     if pointIdx == 114  
%                         keyboard
%                     end
                    if inView
                        covCount = covCount + 1;
                        vec(covCount) = pointIdx; %#ok<AGROW>
                        obj.numEventsPerPoint(pointIdx) = obj.numEventsPerPoint(pointIdx) + 1;
                        obj.timeSeriesData{pointIdx,obj.numEventsPerPoint(pointIdx)} = obj.timeIdx;
                    end
                end
            end
        end
        
        function [rangeVec,isFeasible] = CheckGridFeasibility(obj,bodyFixedState)
            
            % TODO: Don't hard code body radii.
            bodyFixedState = bodyFixedState/6378.1363;
            obj.feasibilityTest = obj.pointArray*(bodyFixedState/norm(bodyFixedState));
            rangeArray = repmat(bodyFixedState',size(obj.pointArray,1),1) - obj.pointArray;
            testArray = rangeArray.*obj.pointArray;
            obj.feasibilityTest = sum(testArray,2);
            return
            %  The code above is very MATLAB specific.  Probably need to do
            %  this in C++.
            [maybe] = find(obj.feasibilityTest>0);
            for testIdx = 1:size(maybe,1)
                pointIdx = maybe(testIdx);
                if obj.feasibilityTest(pointIdx) > 0;
                    pointVec = obj.pointArray(pointIdx,:)';
                    rangeVec = bodyFixedState - pointVec;
                    dotProd = rangeVec(1)*pointVec(1)+ rangeVec(2)*pointVec(2)+ ...
                        rangeVec(3)*pointVec(3);
                    if dotProd > 0
                        obj.feasibilityTest(pointIdx) = true;
                    else
                        obj.feasibilityTest(pointIdx) = false;
                    end
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
            bodyFixedState(1:3,1) = obj.centralBody.GetBodyFixedState(...
                inertialState(1:3,1),jDate);
            bodyFixedState(4:6,1) = obj.centralBody.GetBodyFixedState(...
                inertialState(4:6,1),jDate);
        end
        
        function coverageEvents = ProcessCoverageEvents(obj)
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
