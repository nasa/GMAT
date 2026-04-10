classdef Biegler_Cuthrell_85 < MiNLPUserProblem
    
    properties
        numVars
        numCons
    end
    
    methods
        
        function obj = Biegler_Cuthrell_85()
            % Constructor for the test problem
            obj.numVars = 3;
            obj.numCons = 2;
        end
        
        function [numVars,numCons]= GetNLPInfo(obj)
            % Returns basic info about the problem
            numVars = obj.numVars;
            numCons = obj.numCons;
        end
        
        function [startingPoint] = GetStartingPoint(obj)
            % Returns the starting point
            startingPoint = [0 0 0]';
        end
        
        function [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = ...
                GetBoundsInfo(obj,numVars,numCons);
            % Returns bounds on decision vars and constraints
            varLowerBounds =  [-Inf 0.5 -Inf]';
            varUpperBounds =  [Inf Inf Inf]';
            conLowerBounds =  [-Inf -Inf]';
            conUpperBounds =  [0 0]';

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
            conFunctions = [1 + decVector(1) - decVector(2)^2;...
                            1 - decVector(1) - decVector(2)^2];
                            
        end
        
        function [conJacobian] = EvaluateConJac(obj,numVars,decVector,isNewX)
            % Returns the constraint Jacobian
            x1 = decVector(1);
            x2 = decVector(2);
            conJacobian = [1, -2*x2, 0;...
                          -1, -2*x2, 0];
        end
        
    end
    
end

