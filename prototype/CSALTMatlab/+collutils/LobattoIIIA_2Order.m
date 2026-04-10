classdef LobattoIIIA_2Order < collutils.LobattoIIIASeparated
    %UNTITLED4 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties

    end
    
    methods
        
        function obj = LobattoIIIA_2Order()
            obj = LoadButcherTable(obj);
            obj = InitializeData(obj);
            obj.ComputeDependencies();
        end
                
        function obj = InitializeData(obj)
            obj.numDefectCons = 1;
            obj.numPointsPerStep = 2;
            % Set up non-dimensional stage times
            obj.stageTimes = zeros(1,1);
            obj.stageTimes(2,1) = obj.rhoVec(2,1);
            % Set up the optimization parameter dependency array
            obj.paramDepArray = zeros(1,2);
            obj.paramDepArray(1,:) = [-1  1]; 
            % Set up the function dependency array
            obj.funcConstArray = zeros(1,2);
            obj.funcConstArray(1,:) = -obj.sigmaMatrix(2,:);        
            %
            obj.numStagePointsPerMesh = 0;
            obj.numStateStagePointsPerMesh = 0;
            obj.numControlStagePointsPerMesh = 0;
        end
        
        function obj = LoadButcherTable(obj)
            % Populates the butcher array member data.  Taken from table
            % labelled with (S = 3) in Betts.
            
            % The non-dimensionalized stage times
            obj.rhoVec = zeros(2,1);
            obj.rhoVec(2,1) = 1.0;
            % The alpha vector
            obj.betaVec = zeros(2,1);
            obj.betaVec(1,1) = 1/2;
            obj.betaVec(2,1) = 1/2;
            % The beta matrix
            obj.sigmaMatrix = zeros(2,2);
            obj.sigmaMatrix(2,1) = 1/2;
            obj.sigmaMatrix(2,2) = 1/2;
            
        end
        
    end
    
end

