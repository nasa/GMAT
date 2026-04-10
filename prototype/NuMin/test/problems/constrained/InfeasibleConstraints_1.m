classdef InfeasibleConstraints_1 < MiNLPUserProblem
    
    properties
        numVars
        numCons
    end
    
    methods
        
        function obj = InfeasibleConstraints_1()
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
            startingPoint = [1 1]';
        end
        
        function [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = ...
                GetBoundsInfo(obj,numVars,numCons)
            % Returns bounds on decision vars and constraints
            varLowerBounds = [-Inf -Inf]';
            varUpperBounds = [0    Inf]';
            conLowerBounds = [4]';
            conUpperBounds = [Inf]';
        end
        
        function [costFunc] = EvaluateCostFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            costFunc = decVector(1)^2 + decVector(2)^2;
        end
        
        function [costJacobian] = EvaluateCostJac(obj,numVars,decVector,isNewX)
            % Returns the cost Jacobian
            costJacobian = [2*decVector(1),2*decVector(2)]';
        end
        
        function [conFunctions] = EvaluateConFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            conFunctions = [decVector(1)^2]';
        end
        
        function [conJacobian] = EvaluateConJac(obj,numVars,decVector,isNewX)
            % Returns the constraint Jacobian
            conJacobian = [2*decVector(1), 0];
        end
        
    end
    
end

