classdef LobattoIIIA_6Order < collutils.LobattoIIIASeparated
    %UNTITLED4 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties

    end
    
    methods
        
        function obj = LobattoIIIA_6Order()
            obj = LoadButcherTable(obj);
            obj = InitializeData(obj);
            obj.ComputeDependencies();
        end
        
        function obj = InitializeData(obj)
            obj.numDefectCons = 3;
            obj.numPointsPerStep = 4;
            % Set up non-dimensional stage times
            obj.stageTimes = zeros(4,1);
            obj.stageTimes(2,1) = obj.rhoVec(2,1);
            obj.stageTimes(3,1) = obj.rhoVec(3,1);
            obj.stageTimes(4,1) = obj.rhoVec(4,1);
            % Set up the optimization parameter dependency array
            obj.paramDepArray = zeros(3,4);
            obj.paramDepArray(1,:) = [-1  1  0  0];  % Eq. 3.25b
            obj.paramDepArray(2,:) = [-1  0  1  0]; % Eq. 3.25c
            obj.paramDepArray(3,:) = [-1  0  0  1];  % Eq. 3.25c

            % Set up the function dependency array
            obj.funcConstArray = zeros(3,4);
            obj.funcConstArray(1,:) = -obj.sigmaMatrix(2,:);  % Eq. 3.25b
            obj.funcConstArray(2,:) = -obj.sigmaMatrix(3,:); % Eq. 3.25c
            obj.funcConstArray(3,:) = -obj.betaVec';         % Eq. 3.25a
            %
            obj.numStagePointsPerMesh = 2;
            obj.numStateStagePointsPerMesh = 2;
            obj.numControlStagePointsPerMesh = 2;
        end
        
        function obj = LoadButcherTable(obj)
            % Populates the butcher array member data.  Taken from table
            % labelled with (S = 4) in Betts.
            
            sqrt5 = sqrt(5);
            % The non-dimensionalized stage times
            obj.rhoVec = zeros(4,1);
            obj.rhoVec(2,1) = 0.5 - sqrt5/10;
            obj.rhoVec(3,1) = 0.5 + sqrt5/10;
            obj.rhoVec(4,1) = 1;
            
            % The alpha vector
            obj.betaVec = zeros(4,1);
            obj.betaVec(1,1) = 1/12;
            obj.betaVec(2,1) = 5/12;
            obj.betaVec(3,1) = 5/12;
            obj.betaVec(4,1) = 1/12;
            
            % The beta matrix
            obj.sigmaMatrix = zeros(4,4);
            % Row 2
            obj.sigmaMatrix(2,1) = (11 + sqrt5)/120;
            obj.sigmaMatrix(2,2) = (25 - sqrt5)/120;
            obj.sigmaMatrix(2,3) = (25 - 13*sqrt5)/120;
            obj.sigmaMatrix(2,4) = (-1 + sqrt5)/120;
            % Row 3
            obj.sigmaMatrix(3,1) = (11 - sqrt5)/120;
            obj.sigmaMatrix(3,2) = (25 + 13*sqrt5)/120;
            obj.sigmaMatrix(3,3) = (25 + sqrt5)/120;
            obj.sigmaMatrix(3,4) = (-1 - sqrt5)/120;
            % Row 4
            obj.sigmaMatrix(4,1) = 1/12;
            obj.sigmaMatrix(4,2) = 5/12;
            obj.sigmaMatrix(4,3) = 5/12;
            obj.sigmaMatrix(4,4) = 1/12;
            
        end
        
    end
    
end

