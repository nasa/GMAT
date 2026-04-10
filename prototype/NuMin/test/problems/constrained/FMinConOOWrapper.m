classdef FMinConOOWrapper < MiNLPUserProblem
    %UNTITLED3 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        problemData
        lastX
        conFunctionName
        costFunctionName
       
        costFunction
        costGradient
        eqConFunctions
        ineqConFunctions
        eqConJacobian
        ineqConJacobian
    end
    
    methods
        
        function obj = FMinConOOWrapper(testName)
            obj.problemData = eval(testName);
            obj.conFunctionName = ['CON_' testName];
            obj.costFunctionName = ['OBJ_' testName];
        end
        
        function x0 = GetInitialGuess(obj)
            x0 = obj.problemData.x0;
        end
        
        function [xL,xU] = GetBounds(obj)
            xL = obj.problemData.lb;
            xU = obj.problemData.ub;
        end
        
        function [A,b,Aeq,beq] = GetLinearConstraintArrays(obj)
            
           A = obj.problemData.A;
           b = obj.problemData.b;
           Aeq = obj.problemData.Aeq;
           beq = obj.problemData.beq;
           
        end

        function cost = EvaluateCostFunction(obj,decisionVector)
            [obj.costFunction,obj.costGradient] = feval(obj.costFunctionName,decisionVector);
            cost = obj.costFunction;
        end
        
        function costGrad = EvaluateCostGradient(obj,~)
            costGrad = obj.costGradient;
        end
        
        function [eqCon,ineqCon] = EvaluateConstraints(obj,decisionVector)
            [obj.eqConFunctions,obj.ineqConFunctions,...
                obj.eqConJacobian,obj.ineqConJacobian] = ...
                feval(obj.conFunctionName,decisionVector);
            eqCon = obj.eqConFunctions;
            ineqCon = obj.ineqConFunctions;
        end
        
        function [eqConJacobian,ineqConJacobian] = EvaluateConstraintJacobian(obj,~)
            eqConJacobian = obj.eqConJacobian;
            ineqConJacobian = obj.ineqConJacobian;
        end
        
    end
    
end

