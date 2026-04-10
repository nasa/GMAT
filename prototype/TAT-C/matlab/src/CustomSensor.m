classdef CustomSensor < Sensor
    %CUSTOMSENSOR A sensor model based on user-defined FOV points
    
    % TODO: Fix edge failures (tangent and vertex)
    
    properties (Access = 'private')
        
        % Vector of Doubles. Declination of sensor FOV points, radians
        coneAngleVec
        % Vector of Doubles. Declination of sensor FOV points, radians
        clockAngleVec
        % Integer.  Number of points in the users FOV arrays.
        numFOVPoints
        % String. The type of projection used internally
        projectionAlgorithm
        % Array of Reals. The x coordinate of sensor FOV in the chosen projection
        xProjectionCoordArray
        % Array of Reals. The y coordinate of sensor FOV in the chosen projection
        yProjectionCoordArray
        % Array of spherical coordinates of points outside of region
        externalPointArray
        % Matrix of Reals. Line segments used in Jordan Curve Thereom algorithm
        segmentArray
        % Real. Maximum excursion of FOV in X coord
        maxXExcursion
        % Real. Minimum excursion of FOV in X coord
        minXExcursion
        % Real. Maximum excursion of FOV in Y coord
        maxYExcursion
        % Real. Minimum excursion of FOV in Y coord
        minYExcursion
        % Integer.  Number of points outside of FOV for Jordan Curve Theroem test.
        % We Have more than one to address special cases/edge conditions
        numTestPoints
    end
    
    methods (Access = 'public')
        
        function obj = CustomSensor(coneAngleVec,clockAngleVec)
            % Status: Ready for conversion
            % Class constructor.
            
            % Set core member data items
            obj.coneAngleVec = coneAngleVec;
            obj.clockAngleVec = clockAngleVec;
            obj.ValidateConeAndClockInputs()
            obj.numFOVPoints = length(obj.coneAngleVec);
            if obj.numFOVPoints <= 2
                error('Must have at least 3 points')
            end
            
            % Initialize various required quantities
            obj.ComputeProjectionArrays();
            obj.ComputeLineSegments();
            obj.ComputeMaxExcursion();
            obj.ComputeExternalPoints();
            
        end
        
        function inView = CheckTargetVisibility(obj,viewConeAngle,viewClockAngle)
            % Status: Ready for conversion
            % Determines if a point is in sensor FOV
            
            %  Peform fast cone angle feasibilty check first
            possiblyInView = obj.CheckTargetMaxExcursionAngle(viewConeAngle);
            if ~possiblyInView
                inView = false;
                return
            end
            
            % Convert to stereographic and check if inside bound box of FOV
            [xCoord,yCoord] = obj.ClockConeToStereographic(viewConeAngle,viewClockAngle);
            possiblyInView = CheckTargetMaxExcursionCoordinates(obj,xCoord,yCoord);
            if ~possiblyInView
                inView = false;
                return
            end
            
            % Fast feasiblity checks passed, so peform rigorous test for visiblity
            % by performing line-polygon intersection test
            foundValidPoint = false();
            distTol = 1e-12;
            for pointIdx = 1:obj.numTestPoints
                lineSeg = [xCoord yCoord obj.externalPointArray(pointIdx,:)];
                out = lineSegmentIntersect(obj.segmentArray,lineSeg);
                % Check if test segment passes within tolerance distance of user
                % defined point.  If so, results are  not trustworthy and
                % need to try another point
                if ~(any(abs(out.intNormalizedDistance1To2) <= distTol) || ...
                        any(abs(out.intNormalizedDistance1To2 -1 ) <= distTol))
                    foundValidPoint = true;
                    break
                end
            end
            
            % Fundamental algorithm failure, this should never happen and should
            % be trapped with an internal error letting user know to report
            % issue.
            if foundValidPoint == false
                error('Internal Error: No valid external point was found during polygon intersection test')
            end
            
            % If even number of crossings, not in view. If odd, in view.
            numCrossings = length(find(out.intAdjacencyMatrix == 1));
            if mod(numCrossings,2) == 0
                inView = false;
            else
                inView = true;
            end
            
        end
        
        function [inView] = CheckRegionVisibility(obj,coneAngleVec,clockAngleVec)
            % Status: Ready for conversion
            % Check if entire region lies inside sensor FOV
            
            [xCoords,yCoords] = obj.ConeClockArraysToStereoGraphic(coneAngleVec,clockAngleVec);
            lineSegArray = obj.PointsToSegments(xCoords,yCoords);
            out = lineSegmentIntersect(obj.segmentArray,lineSegArray);
            if any(any(out.intAdjacencyMatrix))
                inView = false;
            else
                inView = true;
            end
        end
        
        function PlotTargetGeometry(obj,viewConeAngle,viewClockAngle)
            % MATLAB SPECIFIC
            obj.PlotSensorGeometry()
            obj.PlotTargetPoint(viewConeAngle,viewClockAngle)
%             axis([-.7 .7 -.7 .7]);
%             drawnow
        end
        
        function PlotTargetPoint(obj,viewConeAngle,viewClockAngle)
            % MATLAB SPECIFIC
            inView = obj.CheckTargetVisibility(viewConeAngle,viewClockAngle);
            [xCoord,yCoord] = obj.ClockConeToStereographic(viewConeAngle,viewClockAngle);
            
            if ~inView
                marker1 = 'k*';
                marker2 = 'ro';
            else
                marker1 = 'k*';
                marker2 = 'go';
            end
            plot(xCoord,yCoord,marker1)
            plot(xCoord,yCoord,marker2,'MarkerSize',10,'LineWidth',2)
            
            axis equal;
            grid on;
            drawnow
            axisFac = 1.1;
            axis(axisFac*[min([obj.minXExcursion xCoord])...
                max([obj.maxXExcursion xCoord])...
                min([obj.minYExcursion yCoord])...
                max([obj.maxYExcursion yCoord])])
            drawnow
        end
        
        function PlotRegionGeometry(obj,coneAngleVec,clockAngleVec)
            %% MATLAB SPECIFIC
            obj.PlotSensorGeometry()
            numPoints = length(coneAngleVec);
            % Plot the points and their view status
            for pointIdx = 1:numPoints
                obj.PlotTargetPoint(coneAngleVec(pointIdx),clockAngleVec(pointIdx))
            end
            inView = obj.CheckRegionVisibility(coneAngleVec,clockAngleVec);
            if inView
                style = 'g-';
            else
                style = 'r-';
            end
            
            % Plot the segments
            for pointIdx = 1:numPoints-1
                [xCoord1,yCoord1] = obj.ClockConeToStereographic(...
                    coneAngleVec(pointIdx),clockAngleVec(pointIdx));
                [xCoord2,yCoord2] = obj.ClockConeToStereographic(...
                    coneAngleVec(pointIdx+1),clockAngleVec(pointIdx+1));
                plot([xCoord1 xCoord2],[yCoord1 yCoord2],style);
            end
            [xCoord1,yCoord1] = obj.ClockConeToStereographic(...
                coneAngleVec(numPoints),clockAngleVec(numPoints));
            [xCoord2,yCoord2] = obj.ClockConeToStereographic(...
                coneAngleVec(1),clockAngleVec(1));
            plot([xCoord1 xCoord2],[yCoord1 yCoord2],style);
        end
        
        function PlotSensorGeometry(obj)
            %% MATLAB SPECIFIC
            gca(obj.figHandle); clf
            hold on
            plot([obj.xProjectionCoordArray;obj.xProjectionCoordArray(1,1)],...
                [obj.yProjectionCoordArray;obj.yProjectionCoordArray(1,1)])
            obj.PlotMaxExcursionCone()
            axisFac = 1.1;
            axis(axisFac*[obj.minXExcursion obj.maxXExcursion obj.minYExcursion obj.maxYExcursion])
        end
        
        function PlotMaxExcursionCone(obj)
            % MATLAB ONLY
            numPoints = 30;
            dAngle = 2*pi/numPoints;
            for angleIdx = 1:numPoints
                coneAngle1 = (angleIdx - 1)*dAngle;
                coneAngle2 = (angleIdx)*dAngle;
                [xCoord1,yCoord1] = obj.ClockConeToStereographic(obj.maxExcursionAngle,coneAngle1);
                [xCoord2,yCoord2] = obj.ClockConeToStereographic(obj.maxExcursionAngle,coneAngle2);
                gca(obj.figHandle);
                plot([xCoord1;xCoord2],[yCoord1;yCoord2],'b-.')
            end
        end
        
        function PlotCandidatePoints(obj)
            % MATLAB ONLY
            gca(obj.figHandle);
            hold on;
            for pointIdx = 1:obj.numTestPoints
                plot(obj.externalPointArray(pointIdx,1) ,obj.externalPointArray(pointIdx,2),'b*')
            end
        end
        
    end
    
    methods (Access = 'private')
        
        function ComputeExternalPoints(obj)
            % Status: Ready for conversion
            % Computes points external to FOV to use in visibility tests
            pointDist = zeros(obj.numFOVPoints,1);
            for pointIdx = 1:obj.numFOVPoints
                pointDist(pointIdx,1) = sqrt(obj.xProjectionCoordArray(pointIdx)^2 + ...
                    obj.yProjectionCoordArray(pointIdx)^2);
            end
            
            %  If there are no points outside unit sphere, then easy
            obj.numTestPoints = 3;
            obj.externalPointArray = zeros(obj.numTestPoints,2);
            [sortedPointsVals,~] = sort(pointDist);
            if max(sortedPointsVals) <= 1
                % Pick random points just outside of unit circle
                clockAngles = ones(obj.numTestPoints,1)*pi/2 + 0.001;
                coneAngles = 2*pi*rand(obj.numTestPoints,1);
                for pointIdx = 1:obj.numTestPoints
                    [x,y] = obj.ClockConeToStereographic(clockAngles(pointIdx),coneAngles(pointIdx));
                    obj.externalPointArray(pointIdx,:) = [x,y];
                end
                return
            end
            
            %  Handle trickier case when points are outside unit sphere and
            %  you can't tell if sensor wraps back on itself "s-shaped sensor" for example
            numCandidatePoints = 0;
            coneAngles = [];
            %  Loope over points and compute the internal angle at each vertex.
            for pointIdx1 = 1:obj.numFOVPoints
                if pointIdx1 <= obj.numFOVPoints - 2
                    pointIdx2 = pointIdx1 + 1;
                    pointIdx3 = pointIdx2 + 1;
                elseif pointIdx1 <= obj.numFOVPoints - 1
                    pointIdx2 = pointIdx1 + 1;
                    pointIdx3 = 1;
                else
                    pointIdx2 = 1;
                    pointIdx3 = 2;
                end
                vec1 = [
                    obj.xProjectionCoordArray(pointIdx2) - obj.xProjectionCoordArray(pointIdx1);
                    obj.yProjectionCoordArray(pointIdx2) - obj.yProjectionCoordArray(pointIdx1)];
                vec2 = [
                    obj.xProjectionCoordArray(pointIdx3) - obj.xProjectionCoordArray(pointIdx2);
                    obj.yProjectionCoordArray(pointIdx3) - obj.yProjectionCoordArray(pointIdx2)];
                interiorAngle = mod(mod(atan2(vec2(2),vec2(1)),2*pi)- ...
                                mod(atan2(vec1(2),vec1(1)),2*pi),2*pi);
                            
                % If the internal angle is less than pi, keep the
                % point as a candidate external test point.
                if interiorAngle <= pi
                    numCandidatePoints = numCandidatePoints + 1;
                    candidatePoints(numCandidatePoints,:) = ...
                        [obj.xProjectionCoordArray(pointIdx2) ...
                        obj.yProjectionCoordArray(pointIdx2)]; %#ok<AGROW>
                    coneAngles(numCandidatePoints) = obj.coneAngleVec(pointIdx2); %#ok<AGROW>
                end
            end
            % Sort the candidate points in order of decreasing cone angle
            % and use the first three as the test points
            safetyFactor = 1.1;  % Make sure we're outside the selected vertex
            [~,sortedIdxs] = sort(coneAngles,'descend');
            obj.externalPointArray = safetyFactor*[...
                candidatePoints(sortedIdxs(1:obj.numTestPoints),1),...
                candidatePoints(sortedIdxs(1:obj.numTestPoints),2)];
            %obj.PlotCandidatePoints()
        end
        
        function lineSegArray = PointsToSegments(~,xCoords,yCoords)
            % Status: Ready for conversion
            % Convert array of xy coords in stereographic to line segment array
            numRows = size(xCoords,1);
            lineSegArray = zeros(numRows,4);
            for rowIdx = 1:numRows-1
                lineSegArray(rowIdx,:) = ...
                    [xCoords(rowIdx) yCoords(rowIdx) xCoords(rowIdx+1) yCoords(rowIdx+1)];
            end
            lineSegArray(numRows,:) = [xCoords(numRows) yCoords(numRows) xCoords(1) yCoords(1)];
        end
        
        function ComputeProjectionArrays(obj)
            % Status: Ready for conversion
            % Initializes the projection of the FOV arrays
            [xArray,yArray] = obj.ConeClockArraysToStereoGraphic(obj.coneAngleVec,obj.clockAngleVec);
            obj.xProjectionCoordArray = xArray;
            obj.yProjectionCoordArray = yArray;
            
            %  Compute the maximum excursion. i.e the bounding box.
            obj.maxXExcursion = max(obj.xProjectionCoordArray);
            obj.minXExcursion = min(obj.xProjectionCoordArray);
            obj.maxYExcursion = max(obj.yProjectionCoordArray);
            obj.minYExcursion = min(obj.yProjectionCoordArray);
        end
        
        function [xArray,yArray] = ConeClockArraysToStereoGraphic(obj,coneAngleVec,clockAngleVec)
            % Status: Ready for conversion
            % Converts points to line segements required by curve thereom code
            numPoints = length(coneAngleVec);
            xArray = zeros(numPoints,1);
            yArray = xArray;
            for pointIdx = 1:numPoints
                [RA,Dec] = obj.ConeClockToRADEC(coneAngleVec(pointIdx),clockAngleVec(pointIdx));
                unitVec = obj.RADECToUnitVec(RA,Dec);
                [xCoord,yCoord] = obj.UnitVecToStereoGraphic(unitVec);
                xArray(pointIdx) = xCoord;
                yArray(pointIdx) = yCoord;
            end
        end
        
        function obj = ComputeLineSegments(obj)
            % Status: Ready for conversion
            % Set up arrays of points required by the
            obj.segmentArray = ...
                PointsToSegments(obj,obj.xProjectionCoordArray,obj.yProjectionCoordArray);
        end
        
        function ComputeMaxExcursion(obj)
            % Status: Ready for conversion
            % Computes maximum cone angle excursion
            obj.maxExcursionAngle = max(obj.coneAngleVec);
        end
        
        function possiblyInView = CheckTargetMaxExcursionCoordinates(obj,xCoord,yCoord)
            % Status: Ready for conversion
            % Check if target falls within bound box
            if xCoord > obj.maxXExcursion
                possiblyInView = false;
                return
            elseif xCoord < obj.minXExcursion
                possiblyInView = false;
                return
            elseif yCoord > obj.maxYExcursion
                possiblyInView = false;
                return
            elseif yCoord < obj.minYExcursion
                possiblyInView = false;
                return
            end
            possiblyInView = true;
        end
        
        function [unitVec] = RADECToUnitVec(~,RA,Dec)
            % Status: Ready for conversion
            % Convert RA/Dec to unitVec
            cosDec = cos(Dec);
            unitVec(1,1) = cosDec*cos(RA);
            unitVec(2,1) = cosDec*sin(RA);
            unitVec(3,1) = sin(Dec);
        end
        
        function [xCoord,yCoord] = UnitVecToStereoGraphic(~,cartVec)
            % Status: Ready for conversion
            % Convert unit vector to sterographic coords.
            % TODO, assumes a boresite of +Z axis.  Should change this.
            xCoord = cartVec(1,1)/(1+cartVec(3,1));
            yCoord = cartVec(2,1)/(1+cartVec(3,1));
        end
        
        function [RA,Dec] = ConeClockToRADEC(~,coneAngle,clockAngle)
            % Status: Ready for conversion
            % Convert clock and cone angle to RA/Dec
            RA = clockAngle;
            Dec = pi/2 - coneAngle;
        end
        
        function [xCoord,yCoord] = ClockConeToStereographic(obj,clockAngle,coneAngle)
            % Status: Ready for conversion
            % Converts clock and cone angle to sterographic X and Y
            [RA,Dec] = obj.ConeClockToRADEC(clockAngle,coneAngle);
            unitVec = obj.RADECToUnitVec(RA,Dec);
            [xCoord,yCoord] = obj.UnitVecToStereoGraphic(unitVec);
        end
        
        function ValidateConeAndClockInputs(obj)
            % Status: Ready for conversion
            % Validate that the Cone and Clock angle inputs are valid
            if length(obj.coneAngleVec) ~= length(obj.clockAngleVec)
                error('cone angle vectors and clock angle vectors must be the same length');
            end
            for vecIdx = 1:length(obj.coneAngleVec)
                if obj.coneAngleVec(vecIdx) > pi - 100*eps
                    error('cone angle vector components must be less than pi/2 - 100*eps');
                end
            end
        end
        
    end
    
end