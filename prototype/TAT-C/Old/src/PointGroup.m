classdef PointGroup < handle
    %POINTGROUP. Computes and stores grid points on a sphere.
    
    properties (Access = private)
        % String. The name of the model used to create points
        modelName
        % Real array length num points. Latitude coordinates of grid points
        latVec
        % Real array length num points. Longitude coordinates of grid points
        lonVec
        % std:vec of 3x1 arrays. The cartesian coordinates of grid points
        testPointsArray
        % Integer.  The number of points in the point group
        numPoints
    end
    
    methods (Access = public)
        
        function obj = PointGroup(modelName,numPoints)
            % Class constructor
            % Inputs:  string modelName, int numPoints
            obj.numPoints = numPoints;
            obj.modelName = modelName;
            obj.DimensionArrays();
            obj.ComputeTestPoints('Helical')
        end
        
        function posVec = GetPointPositionVector(obj,poiIndex)
            % Returns body fixed location of point given point index
            % Inputs. int poiIndex
            % Outputs. Rvector 3x1 containing the position.
            posVec = obj.testPointsArray{poiIndex};
        end
        
        function [lat,lon] = GetLatAndLon(obj,poiIndex)
            % Returns body fixed location of point given point index
            % Inputs. int poiIndex
            % Outputs. double lat, double lon
            lat = obj.latVec(poiIndex);
            lon = obj.lonVec(poiIndex);
        end
        
        function numPoints = GetNumPoints(obj)
            % Returns number of points
            % Ouput: int numPoints
            numPoints = obj.numPoints;
        end
        
    end
    
    methods (Access = private)
        
        function DimensionArrays(obj)
            % Pre-dimensions arrays based on user's number of points
            obj.testPointsArray = cell(1,obj.numPoints);
            obj.latVec = zeros(1,obj.numPoints);
            obj.lonVec = zeros(1,obj.numPoints);
        end
        
        function ComputeTestPoints(obj,modelName)
            %  Computes surface grid points
            %  Inputs: string modelName
            
            % Place first point at north pole
            if (obj.numPoints >= 1)
                % One Point at North Pole
                pointIdx    = 1;
                obj.testPointsArray{pointIdx} = [0 0 1]';
                obj.latVec(1,1) = pi/2;
                obj.lonVec(1,1) = 0;
            end
            % Place second point at south pole
            if (obj.numPoints >= 2)
                % One Point at South Pole
                pointIdx    = 2;
                obj.testPointsArray{pointIdx} = [0 0 -1]';
                obj.latVec(1,1) = -pi/2;
                obj.lonVec(1,1) = 0;
            end
            % Place remaining points according to requested algorithm
            if (obj.numPoints >= 3)
                if strcmp(modelName,'Helical')
                    ComputeHelicalPoints(obj,obj.numPoints-2);
                end
            end
        end
        
        function ComputeHelicalPoints(obj,numPoints)
            % Build a set of evenly spaced points using Helical algorithm
            % Inputs: int, numPoints
            
            % Determine how many longitude "bands" and fill them in
            numDiscreteLatitudes = floor(sqrt((numPoints+1)*pi/4));
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
            numRemainingPoints(1) = numPoints;
            for latIdx=1:1:numDiscreteLatitudes
                numPointsByLatBand(latIdx) = round( numRemainingPoints(latIdx)*...
                    cos(discreteLatitudes(latIdx))/alpha(latIdx));
                numRemainingPoints(latIdx+1) = numRemainingPoints(latIdx) - ...
                    numPointsByLatBand(latIdx);
                alpha(latIdx+1) = alpha(latIdx) - cos(discreteLatitudes(latIdx));
                for j = 1:1:numPointsByLatBand(latIdx)
                    % Compute the latitude for the next point
                    currentLongitude = 2*pi*(j-1)/numPointsByLatBand(latIdx);
                    % Insert the point into the cartesian and spherical
                    % arrays.
                    pointIdx = pointIdx + 1;
                    %  TODO:  Use geodetic to Cartesian conversion.
                    obj.testPointsArray{pointIdx} = [ cos(currentLongitude) *...
                        cos(discreteLatitudes(latIdx));
                        sin(currentLongitude) * cos(discreteLatitudes(latIdx));
                        sin(discreteLatitudes(latIdx))];
                    obj.latVec(1,pointIdx) = discreteLatitudes(latIdx);
                    obj.lonVec(1,pointIdx) = currentLongitude;
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





