classdef GPR_T1_1 < MiNLPUserProblem
    
    properties
        numVars
        numCons
    end
    
    methods
        
        function obj = GPR_T1_1()
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
            startingPoint = [2 2]';
        end
        
        function [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = ...
                GetBoundsInfo(obj,numVars,numCons);
            % Returns bounds on decision vars and constraints
            varLowerBounds =  [-Inf -Inf]';
            varUpperBounds =  [Inf Inf ]';
            conLowerBounds =  [0];
            conUpperBounds =  [0];

        end
        
        function [costFunc] = EvaluateCostFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            costFunc = log(1 + decVector(1)^2) - decVector(2);
        end
        
        function [costJacobian] = EvaluateCostJac(obj,numVars,decVector,isNewX)
            % Returns the cost Jacobian
            x1 = decVector(1);
            x2 = decVector(2);
            costJacobian = [(2*x1)/(x1^2 + 1),-1]';
        end
        
        function [conFunctions] = EvaluateConFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            x1 = decVector(1);
            x2 = decVector(2);
            conFunctions = (1+x1^2)^2 + x2^2 - 4;   
        end
        
        function [conJacobian] = EvaluateConJac(obj,numVars,decVector,isNewX)
            % Returns the constraint Jacobian
            x1 = decVector(1);
            x2 = decVector(2);
            conJacobian = [4*x1*(x1^2 + 1),2*x2];
        end
        
    end
    
end

