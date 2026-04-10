classdef PointGroup < handle
    %POINTGROUP. Computes and stores grid points on a sphere.
    
    properties (Access = private)
        % Real array length num points. Latitude coordinates of grid points
        latVec
        % Real array length num points. Longitude coordinates of grid points
        lonVec
        % std:vec of 3x1 arrays. The cartesian coordinates of grid points
        testPointsArray
        % Integer. The number of points in the point group (i.e. passed
        % constraint tests)
        numPoints = 0;
        % Integer. Number of points requested in point algorithm
        numRequestedPoints
        % Double. Upper bound on allowable latitude. -pi/2 <= latUpper <= pi/2
        latUpper = pi/2
        % Double. Lower bound on allowable latitude. -pi/2 <= latLower <= pi/2
        latLower = -pi/2
        % Double. Upper bound on allowable longitude
        lonUpper = 2*pi
        % Double. Lower bound on allowable longitude
        lonLower = 0
    end
    
    methods (Access = public)
        
        function obj = AddUserDefinedPoints(obj,latVec,lonVec)
            %  Add user defined latitude and longitude points
            %  Inputs are real arrays of longitude and latitude in radians
            if length(latVec) ~= length(lonVec)
                error('latVec and lonVec must have the same length)')
            end
            for pointIdx = 1:length(latVec)
                obj.AccumulatePoints(latVec(pointIdx),lonVec(pointIdx))
            end
        end
        
        function obj = AddHelicalPointsByNumPoints(obj,numGridPoints)
            obj.ComputeTestPoints('Helical',numGridPoints);
        end
        
        function obj = AddHelicalPointsByAngle(obj,angleBetweenPoints)
            numGridPoints = floor(4*pi/angleBetweenPoints^2);
            obj.ComputeTestPoints('Helical',numGridPoints);
        end
        
        function posVec = GetPointPositionVector(obj,poiIndex)
            % Returns body fixed location of point given point index
            % Inputs. int poiIndex
            % Outputs. Rvector 3x1 containing the position.
                 
            % Make sure there are points
            obj.CheckHasPoints()
            
            posVec = obj.testPointsArray{poiIndex};
        end
        
        function [lat,lon] = GetLatAndLon(obj,poiIndex)            
            % Returns body fixed location of point given point index
            % Inputs. int poiIndex
            % Outputs. double lat, double lon
            
            % Make sure there are points
            obj.CheckHasPoints()
            
            lat = obj.latVec(poiIndex);
            lon = obj.lonVec(poiIndex);
        end
        
        function numPoints = GetNumPoints(obj)
            % Returns number of points
            % Ouput: int numPoints
            numPoints = obj.numPoints;
        end
        
        function [latVec,lonVec] = GetLatLonVectors(obj)
            % Returns the latitude and longitude vectors
                        
            % Make sure there are points
            obj.CheckHasPoints()
            
            latVec = obj.latVec;
            lonVec = obj.lonVec;
        end
        
        function SetLatLonBounds(obj,latUpper,latLower,lonUpper,lonLower)
            
            if obj.numPoints >0
                error('You must set Lat/Lon Bounds Before adding points')
            end
            
            % Sets bounds on latitude and longitude for grid points
            % angle inputs are in radians
            if latLower > latUpper || latLower == latUpper
                error('latLower > latUpper or they are equal')
            end
            if lonLower > lonUpper || lonLower == lonUpper
                error('lonLower > lonUpper or they are equal')
            end
            if latUpper < -pi/2 || latUpper > pi/2
                error('latUpper value is invalid')
            else
                obj.latUpper = latUpper;
            end
            if latLower < -pi/2 || latLower > pi/2
                error('latUpper value is invalid')
            else
                obj.latLower = latLower;
            end
            obj.lonUpper = mod(lonUpper,2*pi);
            obj.lonLower = mod(lonLower,2*pi);
            
        end
        
        
        
    end
    
    methods (Access = private)
        
        function CheckHasPoints(obj)
            %  Checks to see if points have been added before performing
            %  computations that require points
            if obj.numPoints == 0
                error('The point group does not have any points')
            end
        end
        
        function AccumulatePoints(obj,lat,lon)
            %  Accumlates points, only adding them if they pass constraint
            %  checks
            if (lat >= obj.latLower && lat <= obj.latUpper) && ...
                    (lon >= obj.lonLower && lon <= obj.lonUpper)
                obj.numPoints = obj.numPoints + 1;
                obj.lonVec(obj.numPoints) = lon;
                obj.latVec(obj.numPoints) = lat;
                %  TODO:  Use geodetic to Cartesian conversion and don't
                %  hard code the Earth radius.
                obj.testPointsArray{obj.numPoints} = [ cos(lon) * cos(lat);
                    sin(lon) * cos(lat);
                    sin(lat)]*6378.1363;
            end
        end
        
        function ComputeTestPoints(obj,modelName,numGridPoints)
            %  Computes surface grid points
            %  Inputs: string modelName
            
            % Place first point at north pole
            if (numGridPoints >= 1)
                % One Point at North Pole
                AccumulatePoints(obj,pi/2,0)
            end
            % Place second point at south pole
            if (numGridPoints >= 2)
                % One Point at South Pole
                AccumulatePoints(obj,-pi/2,0)
            end
            % Place remaining points according to requested algorithm
            if (numGridPoints >= 3)
                if strcmp(modelName,'Helical')
                    ComputeHelicalPoints(obj,numGridPoints-2);
                end
            end
        end
        
        function ComputeHelicalPoints(obj,numRequestedPoints)
            % Build a set of evenly spaced points using Helical algorithm
            % Inputs: int, numPoints
            
            % Determine how many longitude "bands" and fill them in
            numDiscreteLatitudes = floor(sqrt((numRequestedPoints+1)*pi/4));
            discreteLatitudes = zeros(1,numDiscreteLatitudes); % array of ints  in C++
            for latIdx = 1:2:numDiscreteLatitudes
                % Odd Numbers
                discreteLatitudes(latIdx) = (pi/2)*(1-(latIdx+1)/(numDiscreteLatitudes+1));
                % Even Numbers
                discreteLatitudes(latIdx+1) = -discreteLatitudes(latIdx);
            end
            
            %  Now compute the longitude points for each latitude band
            alpha = zeros(1,numDiscreteLatitudes+1); % Array of doubles in C++
            alpha(1) = 0;
            for latIdx = 1:1:numDiscreteLatitudes
                alpha(1) = alpha(1) + cos(discreteLatitudes(latIdx));
            end
            numPointsByLatBand = zeros(1,numDiscreteLatitudes); % Array of ints  in C++
            pointIdx = 2; % this is two because we already added in points at the poles
            numRemainingPoints = zeros(1,numDiscreteLatitudes+1); % Array of ints in C++
            numRemainingPoints(1) = numRequestedPoints;
            for latIdx=1:1:numDiscreteLatitudes
                numPointsByLatBand(latIdx) = round( numRemainingPoints(latIdx)*...
                    cos(discreteLatitudes(latIdx))/alpha(latIdx));
                numRemainingPoints(latIdx+1) = numRemainingPoints(latIdx) - ...
                    numPointsByLatBand(latIdx);
                alpha(latIdx+1) = alpha(latIdx) - cos(discreteLatitudes(latIdx));
                for j = 1:1:numPointsByLatBand(latIdx)
                    % Compute the latitude for the next point
                    currentLongitude = 2*pi*(j-1)/numPointsByLatBand(latIdx);
                    % Insert the point into the cartesian and spherical arrays.
                    pointIdx = pointIdx + 1;
                    %  TODO:  Use geodetic to Cartesian conversion.
                    obj.AccumulatePoints(discreteLatitudes(latIdx),currentLongitude);
                end
            end
        end
        
    end
    
    methods (Access = public)
        %%  All methods here are specific to MATLAB and are placed here for that reason
        %  They are NOT needed in C++
        function figHandle = PlotAllTestPoints(obj)
            %  Used to visualize points. not needed in GMAT.
            %  DO NOT CONVERT TO C++
            [xSphere,ySphere,zSphere] = sphere(25);
            xSphere = xSphere*6378.1363;
            ySphere = ySphere*6378.1363;
            zSphere = zSphere*6378.1363;
            figHandle = figure(100);
            surf(xSphere,ySphere,zSphere,'EdgeColor',[0.784 0.816 0.831])
            colormap('white')
            hold on
            for pointIdx = 1:obj.numPoints
                plot3(obj.testPointsArray{pointIdx}(1),obj.testPointsArray{pointIdx}(2),...
                    obj.testPointsArray{pointIdx}(3),'k.','MarkerSize',2);
            end
            axis equal
        end
        
        function PlotSelectedTestPoints(obj,pointIds)
            %  Used to visualize points. not needed in GMAT.
            %  DO NOT CONVERT TO C++
            AddPlotSelectedTestPoints(obj,pointIds)
        end
        
        function AddPlotSelectedTestPoints(obj,pointIds)
            %  Used to visualize points. not needed in GMAT.
            %  DO NOT CONVERT TO C++
            for pointIdx = 1:length(pointIds)
                plotIdx = pointIds(pointIdx);
                plot3(obj.testPointsArray{plotIdx}(1),obj.testPointsArray{plotIdx}(2),...
                    obj.testPointsArray{plotIdx}(3),'Color',[0 .5 0],'Marker','*','MarkerSize',3);
            end
        end
    end
end





