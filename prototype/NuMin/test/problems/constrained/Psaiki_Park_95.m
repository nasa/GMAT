classdef Psaiki_Park_95 < MiNLPUserProblem
    
    properties
        numVars
        numCons
    end
    
    methods
        
        function obj = Psaiki_Park_95()
            % Constructor for the test problem
            obj.numVars = 3;
            obj.numCons = 1;
        end
        
        function [numVars,numCons]= GetNLPInfo(obj)
            % Returns basic info about the problem
            numVars = obj.numVars;
            numCons = obj.numCons;
        end
        
        function [startingPoint] = GetStartingPoint(obj)
            % Returns the starting point
            startingPoint = [0 0.999 0]';
            %startingPoint = [0.9687 -0.2481 0]';
        end
        
        function [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = ...
                GetBoundsInfo(obj,numVars,numCons);
            % Returns bounds on decision vars and constraints
            varLowerBounds =  [-Inf -Inf -Inf]';
            varUpperBounds =  [Inf Inf Inf]';
            conLowerBounds = 0;
            conUpperBounds = 0;

        end
        
        function [costFunc] = EvaluateCostFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            costFunc = decVector(2) + 0.5*decVector(3)^2;
        end
        
        function [costJacobian] = EvaluateCostJac(obj,numVars,decVector,isNewX)
            % Returns the cost Jacobian
            costJacobian = [0 1 decVector(3)]';
        end
        
        function [conFunctions] = EvaluateConFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            x1 = decVector(1);
            x2 = decVector(2);
            conFunctions = (x1 - 1)^2 + x2^2 + 10000*(x1^2 + x2^2 - 1)^2 - 0.0625;
        end
        
        function [conJacobian] = EvaluateConJac(obj,numVars,decVector,isNewX)
            % Returns the constraint Jacobian
            x1 = decVector(1);
            x2 = decVector(2);
            conJacobian = [2*x1 + 40000*x1*(x1^2 + x2^2 - 1) - 2,...
                2*x2 + 40000*x2*(x1^2 + x2^2 - 1),...
                0];
        end
        
    end
    
end

