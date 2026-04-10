classdef LobattoIIIA_4HSOrder < collutils.LobattoIIIASeparated
    %UNTITLED4 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function obj = LobattoIIIA_4HSOrder()
            obj = LoadButcherTable(obj);
            obj = InitializeData(obj);
            obj.ComputeDependencies();
        end
        
        function obj = InitializeData(obj)
            obj.numDefectCons = 2;
            obj.numPointsPerStep = 3;
            % Set up non-dimensional stage times
            obj.stageTimes = zeros(3,1);
            obj.stageTimes(2,1) = obj.rhoVec(2,1);
            obj.stageTimes(3,1) = obj.rhoVec(3,1);
            % Set up the optimization parameter dependency array
            obj.paramDepArray = zeros(2,3);
            obj.paramDepArray(1,:) = [-1/2  1 -1/2 ];
            obj.paramDepArray(2,:) = [-1    0   1 ];
            % Set up the function dependency array
            obj.funcConstArray = zeros(2,3);
            obj.funcConstArray(1,:) = [-1/8 0 1/8];
            obj.funcConstArray(2,:) = -obj.betaVec';
            obj.numStagePointsPerMesh = 1;
            obj.numStateStagePointsPerMesh = 1;
            obj.numControlStagePointsPerMesh = 1;
            
        end
        
        function obj = LoadButcherTable(obj)
            % Populates the butcher array member data.  Taken from table
            % labelled with (S = 3) in Betts.
            
            % The non-dimensionalized stage times
            obj.rhoVec = zeros(3,1);
            obj.rhoVec(2,1) = 0.5;
            obj.rhoVec(3,1) = 1.0;
            
            % The alpha vector
            obj.betaVec = zeros(3,1);
            obj.betaVec(1,1) = 1/6;
            obj.betaVec(2,1) = 2/3;
            obj.betaVec(3,1) = 1/6;
            
            % The beta matrix
            obj.sigmaMatrix = zeros(3,3);
            % Row 2
            obj.sigmaMatrix(2,1) = 5/24;
            obj.sigmaMatrix(2,2) = 1/3;
            obj.sigmaMatrix(2,3) = -1/24;
            % Row 3
            obj.sigmaMatrix(3,1) = 1/6;
            obj.sigmaMatrix(3,2) = 2/3;
            obj.sigmaMatrix(3,3) = 1/6;
            
        end
        
    end
    
end

