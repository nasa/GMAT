classdef MinNLPOOTestProb < MiNLPUserProblem
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    %------ linear equality constraints, calculate values
    %------ nonlinear equality constraints
    %------ linear inequality constraints, calculate values
    %------ bound constraints
    %------ add in the nonlinear inequality constraints
    
    properties
        AVarBound
        bVarBound
        numBoundCon
        conType
        conMode
        bVarBoundUpper
        bVarBoundLower
        problemName
        costFuncName
        conFuncName
        problemData
        
        varLowerBounds
        varUpperBounds
        conLowerBounds
        conUpperBounds
        numNonLinIneqCon
        nonLinIneqConStartIdx
        % Integer. Number of non-linear equality constraints
        numNonLinEqCon
        nonLinEqConStartIdx
        % Integer. Number of linear inequality constraints
        numLinIneqCon
        linIneqConStartIdx
        % Integer. Number of linear equality constraints
        numLinEqCon
        linEqConStartIdx
        boundConStartIdx
        conJacobian
        costJacobian
        numNonLinCon
        numLinCon
        numVars
        numCons
        conFunctions
        
    end
    
    methods
        
        function obj = MinNLPOOTestProb(problemName,conMode)
            % Constructor for the test problem
            
            % Load the data from the old procedural inteface
            obj.problemName = problemName;
            obj.costFuncName = strcat('OBJ_',obj.problemName);
            obj.conFuncName  = strcat('CON_',obj.problemName);
            obj.problemData = feval(obj.problemName);
            obj.conMode = conMode;
            
            % Set/determine number of vars and constraints by type
            obj.numVars = length(obj.problemData.x0);
            obj.varLowerBounds = obj.problemData.lb;
            obj.varUpperBounds = obj.problemData.ub;
            
            obj.HandleOneSidedVarBounds(obj.problemData.lb,obj.problemData.ub);
            [ci,ce] = feval(obj.conFuncName,obj.problemData.x0);
            obj.numNonLinIneqCon = size(ci,1);
            obj.numNonLinEqCon = size(ce,1);
            obj.numLinIneqCon = size(obj.problemData.A,1);
            obj.numLinEqCon = size(obj.problemData.Aeq,1);
            obj.numNonLinCon = obj.numNonLinIneqCon + obj.numNonLinEqCon;
            obj.numLinCon = obj.numLinEqCon + obj.numLinIneqCon + obj.numBoundCon;
            obj.numCons = obj.numNonLinCon + obj.numLinCon;
            
            obj.linEqConStartIdx = 1;
            obj.nonLinEqConStartIdx = obj.linEqConStartIdx + obj.numLinEqCon;
            obj.linIneqConStartIdx = obj.nonLinEqConStartIdx + obj.numNonLinEqCon;
            obj.boundConStartIdx = obj.linIneqConStartIdx + obj.numLinIneqCon;
            obj.nonLinIneqConStartIdx = obj.linIneqConStartIdx + obj.numLinIneqCon + obj.numBoundCon;
            
            obj.conType = zeros(obj.numCons,1);
            for conIdx = 1:obj.numCons
                if conIdx >= obj.linEqConStartIdx && conIdx < obj.nonLinEqConStartIdx
                    currConType = 1;
                elseif conIdx >= obj.nonLinEqConStartIdx && conIdx < obj.linIneqConStartIdx
                    currConType = 2;
                elseif conIdx >= obj.linIneqConStartIdx && conIdx < obj.nonLinIneqConStartIdx
                    currConType = 3;
                else
                    currConType = 4;
                end
                obj.conType(conIdx,1) = currConType;
            end
            
            % Dimension constraint Jacobian and fill in linear terms now
            obj.conFunctions = zeros(obj.numCons,1);
            obj.conLowerBounds = zeros(obj.numCons,1);
            obj.conUpperBounds = zeros(obj.numCons,1);
            obj.conJacobian = zeros(obj.numCons,obj.numVars);
            % fill in Jacobian for linear equality constraints
            if obj.numLinEqCon >= 1
                obj.conJacobian(1:obj.numLinEqCon,1:obj.numVars) = obj.problemData.Aeq;
            end
            % fill in Jacobian for linear inequality constraints
            if obj.numLinIneqCon >= 1
                startIdx = obj.linIneqConStartIdx;
                stopIdx = startIdx + obj.numLinIneqCon - 1;
                obj.conJacobian(startIdx:stopIdx,1:obj.numVars) = obj.problemData.A;
            end
            % fill in Jacobian for bound constraints on decision vars
            if obj.numBoundCon > 0
                startIdx = obj.boundConStartIdx;
                stopIdx = startIdx + obj.numBoundCon -1;
                obj.conJacobian(startIdx:stopIdx,1:obj.numVars) = obj.AVarBound;
            end
            
            
            if isempty(obj.problemData.ub)
                obj.problemData.ub = inf*ones(obj.numVars,1);
            end
            if isempty(obj.problemData.lb)
                obj.problemData.lb = -inf*ones(obj.numVars,1);
            end
            
        end
        
        function [numVars,numCons]= GetNLPInfo(obj)
            % Returns basic info about the problem
            numVars = obj.numVars;
            numCons =  obj.numNonLinCon + obj.numLinCon;
        end
        
        function [startingPoint] = GetStartingPoint(obj)
            startingPoint = obj.problemData.x0;
        end
        
        function [varLowerBounds,varUpperBounds,conLowerBounds,conUpperBounds] = ...
                GetBoundsInfo(obj,numVars,numCons);
            % Returns bounds on decision vars and constraints
            varLowerBounds = obj.problemData.lb;
            varUpperBounds = obj.problemData.ub;
            
            % Nothing to do for linear constraints
            % Fill in linear inequality constraints and Jacobian
            if obj.numLinIneqCon >= 1
                startIdx = obj.linIneqConStartIdx;
                stopIdx = startIdx + obj.numLinIneqCon -1;
                obj.conLowerBounds(startIdx:stopIdx,1) = zeros(obj.numLinIneqCon,1);
                obj.conUpperBounds(startIdx:stopIdx,1) = 1e12*ones(obj.numLinIneqCon,1);
            end
            % Fill in non-linear inequality
            if obj.numNonLinIneqCon >= 1
                startIdx = obj.nonLinIneqConStartIdx;
                stopIdx = startIdx + obj.numNonLinIneqCon - 1;
                obj.conLowerBounds(startIdx:stopIdx,1) = zeros(obj.numNonLinIneqCon,1);
                obj.conUpperBounds(startIdx:stopIdx,1) = 1e12*ones(obj.numNonLinIneqCon,1);
            end
            % Fill in the bound constraints expressed as linear inequality
            if obj.conMode == 1
                if obj.numBoundCon > 0
                    startIdx = obj.boundConStartIdx;
                    stopIdx = startIdx + obj.numBoundCon -1;
                    obj.conLowerBounds(startIdx:stopIdx,1) = obj.bVarBound;
                end
            else
                if obj.numBoundCon > 0
                    startIdx = obj.boundConStartIdx;
                    stopIdx = startIdx + obj.numBoundCon -1;
                    obj.conLowerBounds(startIdx:stopIdx,1) = obj.bVarBoundLower;
                    obj.conUpperBounds(startIdx:stopIdx,1) = obj.bVarBoundUpper;
                end
            end
            conLowerBounds = obj.conLowerBounds;
            conUpperBounds = obj.conUpperBounds;
        end
        
        function [costFunc] = EvaluateCostFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            
            % The original inteface for MiNLP computed cost and cost
            % Jacobian in a single function call.  So here get both and
            % store cost Jac
            [costFunc,costJac] = feval(obj.costFuncName,decVector);
            obj.costJacobian = costJac;
        end
        
        function [costJacobian] = EvaluateCostJac(obj,numVars,decVector,isNewX)
            % Returns the cost function Jacobian
            
            % The original inteface for MiNLP computed cost and cost
            % Jacobian in a single function call.  So return value computed
            % on call to EvaluateCostFunc
            costJacobian = obj.costJacobian;
        end
        
        function [conFunctions] = EvaluateConFunc(obj,numVars,decVector,isNewX)
            % Returns the cost function value
            
            %----- If there are linear equality constraints, calculate values
            %----- Add in the nonlinear equality constraints
            %----- If there are linear inequality constraints, calculate values
            %----- Add in the nonlinear inequality constraints
            
            % The original inteface for MiNLP computed cost and cost
            % Jacobian in a single function call.  So here get both and
            % store cost Jac
            [ineqConFunc,eqConFunc,ineqConJac,eqConJac] = feval(obj.conFuncName,decVector);
            
            % Fill in linear equality constraints
            if obj.numLinEqCon >= 1
                obj.conFunctions(1:obj.numLinEqCon,1) = obj.problemData.Aeq*decVector;
            end
            % Fill in nonlinear equality constraints and Jacobian
            if obj.numNonLinEqCon >= 1
                startIdx = obj.nonLinEqConStartIdx;
                stopIdx = startIdx + obj.numNonLinEqCon - 1;
                obj.conFunctions(startIdx:stopIdx,1) = eqConFunc;
                obj.conJacobian(startIdx:stopIdx,:) = eqConJac';
            end
            % Fill in linear inequality constraints
            if obj.numLinIneqCon >= 1
                startIdx = obj.linIneqConStartIdx;
                stopIdx = startIdx + obj.numLinIneqCon - 1;
                obj.conFunctions(startIdx:stopIdx,1) = obj.problemData.Aeq*decVector;
            end
            % Fill in bound constraints
            if obj.numBoundCon > 0
                startIdx = obj.boundConStartIdx;
                stopIdx = startIdx + obj.numBoundCon - 1;
                obj.conFunctions(startIdx:stopIdx,1) = obj.AVarBound*decVector;
            end
            % Fill in non-linear inequality and Jacobian
            if obj.numNonLinIneqCon >= 1
                startIdx = obj.nonLinIneqConStartIdx;
                stopIdx = startIdx + obj.numNonLinIneqCon - 1;
                obj.conFunctions(startIdx:stopIdx,1) = ineqConFunc;
                obj.conJacobian(startIdx:stopIdx,:) = ineqConJac';
            end
            conFunctions = obj.conFunctions;
        end
        
        function [conJacobian] = EvaluateConJac(obj,numVars,decVector,isNewX)
            conJacobian = obj.conJacobian;
        end
        
    end
    
end

