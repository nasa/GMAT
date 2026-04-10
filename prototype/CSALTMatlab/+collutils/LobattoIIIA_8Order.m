classdef LobattoIIIA_8Order < collutils.LobattoIIIASeparated
    %UNTITLED4 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties

    end
    
    methods
        
        function obj = LobattoIIIA_8Order()
            obj = LoadButcherTable(obj);
            obj = InitializeData(obj);
            obj.ComputeDependencies();
        end
        
        function obj = InitializeData(obj)
            obj.numDefectCons = 4;
            obj.numPointsPerStep = 5;
            % Set up non-dimensional stage times
            obj.stageTimes = zeros(5,1);
            obj.stageTimes(2,1) = obj.rhoVec(2,1);
            obj.stageTimes(3,1) = obj.rhoVec(3,1);
            obj.stageTimes(4,1) = obj.rhoVec(4,1);
            obj.stageTimes(5,1) = obj.rhoVec(5,1);
            % Set up the optimization parameter dependency array
            obj.paramDepArray = zeros(4,5);
            obj.paramDepArray(1,:) = [-1  1  0  0  0];  % Eq. 3.25b
            obj.paramDepArray(2,:) = [-1  0  1  0  0]; % Eq. 3.25c
            obj.paramDepArray(3,:) = [-1  0  0  1  0];  % Eq. 3.25c
            obj.paramDepArray(4,:) = [-1  0  0  0  1];  % Eq. 3.25a
            % Set up the function dependency array
            obj.funcConstArray = zeros(4,5);
            obj.funcConstArray(1,:) = -obj.sigmaMatrix(2,:);  % Eq. 3.25b
            obj.funcConstArray(2,:) = -obj.sigmaMatrix(3,:); % Eq. 3.25c
            obj.funcConstArray(3,:) = -obj.sigmaMatrix(4,:);  % Eq. 3.25c
            obj.funcConstArray(4,:) = -obj.betaVec';         % Eq. 3.25a
            %
            obj.numStagePointsPerMesh = 3;
            obj.numStateStagePointsPerMesh = 3;
            obj.numControlStagePointsPerMesh = 3;
        end
        
        function obj = LoadButcherTable(obj)
            % Populates the butcher array member data.  Taken from table
            % labelled with (S = 5) in Betts.
            
            sqrt21 = sqrt(21);
            % The non-dimensionalized stage times
            obj.rhoVec = zeros(5,1);
            obj.rhoVec(2,1) = 0.5 - sqrt21/14;
            obj.rhoVec(3,1) = 0.5;
            obj.rhoVec(4,1) = 0.5 + sqrt21/14;
            obj.rhoVec(5,1) = 1;
            
            % The alpha vector
            obj.betaVec = zeros(5,1);
            obj.betaVec(1,1) = 1/20;
            obj.betaVec(2,1) = 49/180;
            obj.betaVec(3,1) = 16/45;
            obj.betaVec(4,1) = 49/180;
            obj.betaVec(5,1) = 1/20;
            
            % The beta matrix
            obj.sigmaMatrix = zeros(5,5);
            % Row 2
            obj.sigmaMatrix(2,1) = (119 + 3*sqrt21)/1960;
            obj.sigmaMatrix(2,2) = (343 - 9*sqrt21)/2520;
            obj.sigmaMatrix(2,3) = (392 - 96*sqrt21)/2205;
            obj.sigmaMatrix(2,4) = (343 - 69*sqrt21)/2520;
            obj.sigmaMatrix(2,5) = (-21 + 3*sqrt21)/1960;
            % Row 3
            obj.sigmaMatrix(3,1) = 13/320;
            obj.sigmaMatrix(3,2) = (392 + 105*sqrt21)/2880;
            obj.sigmaMatrix(3,3) = 8/45;
            obj.sigmaMatrix(3,4) = (392 - 105*sqrt21)/2880;
            obj.sigmaMatrix(3,5) = 3/320;
            % Row 4
            obj.sigmaMatrix(4,1) = (119 - 3*sqrt21)/1960;
            obj.sigmaMatrix(4,2) = (343 + 69*sqrt21)/2520;
            obj.sigmaMatrix(4,3) = (392 + 96*sqrt21)/2205;
            obj.sigmaMatrix(4,4) = (343 + 9*sqrt21)/2520;
            obj.sigmaMatrix(4,5) = (-21 - 3*sqrt21)/1960;
            % Row 5
            obj.sigmaMatrix(5,1) = 1/20;
            obj.sigmaMatrix(5,2) = 49/180;
            obj.sigmaMatrix(5,3) = 16/45;
            obj.sigmaMatrix(5,4) = 49/180;
            obj.sigmaMatrix(5,5) = 1/20;
            
        end
        
    end
    
end

