classdef MinNLPOOTestProb < MiNLPUserProblem
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    %------ linear equality constraints, calculate values
    %------ nonlinear equality constraints
    %------ linear inequality constraints, calculate values
    %------ bound constraints
    %------ add in the nonlinear inequality constraints
    
    properties

    end
    
    methods
        
        function obj = MinNLPOOTestProb(problemName)
            % Constructor for the test problem
            
        end
        
        function [numVars,numCons]= GetNLPInfo(obj)
            % Returns basic info about the problem
            numVars = obj.numVars;
            numCons =  obj.numNonLinCon + obj.numLinCon;
        end
        
        function [startingPoint] = GetStartingPoint(obj)
            % Returns the starting point
        end
        
        function [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = ...
                GetBoundsInfo(obj,numVars,numCons);
            % Returns bounds on decision vars and constraints

        end
        
        function [costFunc] = EvaluateCostFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            
        end
        
        function [costJacobian] = EvaluateCostJac(obj,numVars,decVector,isNewX)
            % Returns the cost Jacobian
        end
        
        function [conFunctions] = EvaluateConFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value

        end
        
        function [conJacobian] = EvaluateConJac(obj,numVars,decVector,isNewX)
            % Returns the constraint Jacobian
        end
        
    end
    
end

