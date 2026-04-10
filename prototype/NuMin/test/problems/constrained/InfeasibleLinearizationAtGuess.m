classdef InfeasibleLinearizationAtGuess < MiNLPUserProblem
    
    properties
        numVars
        numCons
    end
    
    methods
        
        function obj = InfeasibleLinearizationAtGuess()
            % Constructor for the test problem
            obj.numVars = 2;
            obj.numCons = 1;
        end
        
        function [numVars,numCons]= GetNLPInfo(obj)
            % Returns basic info about the problem
            numVars = obj.numVars;
            numCons = obj.numCons;
        end
        
        function [startingPoint] = GetStartingPoint(obj)
            % Returns the starting point
            startingPoint = [0 0]';
            %startingPoint = -[sin(pi/4) sin(pi/4)]'; % The solution
            %startingPoint = [ -1 -1]'; % The solution
        end
        
        function [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = ...
                GetBoundsInfo(obj,numVars,numCons);
            % Returns bounds on decision vars and constraints
            varLowerBounds =  [-Inf -Inf]';
            varUpperBounds =  [Inf Inf]';
            conLowerBounds = 1;
            conUpperBounds = 1;

        end
        
        function [costFunc] = EvaluateCostFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            costFunc = (2*decVector(1) + 2)^2 + (decVector(2) + 2)^2;
        end
        
        function [costJacobian] = EvaluateCostJac(obj,numVars,decVector,isNewX)
            % Returns the cost Jacobian
            costJacobian = [8*decVector(1) + 8,2*decVector(2) + 4]';
        end
        
        function [conFunctions] = EvaluateConFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            conFunctions = decVector(1)^2 + decVector(2)^2 ;
        end
        
        function [conJacobian] = EvaluateConJac(obj,numVars,decVector,isNewX)
            % Returns the constraint Jacobian
            conJacobian = 2*[decVector(1) decVector(2)];
        end
        
    end
    
end

