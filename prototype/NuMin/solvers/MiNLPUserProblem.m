classdef MiNLPUserProblem < handle
    %MiNLPUserProblem Abstract base class for MiNLPUserProblems
    
    methods (Abstract)
        GetNLPInfo(obj)
        GetStartingPoint(obj)
        GetBoundsInfo(obj)
        EvaluateCostFunc(obj,decisionVector)
        EvaluateCostJac(obj,decisionVector)
        EvaluateConFunc(obj,decisionVector)
        EvaluateConJac(obj,decisionVector)
    end
    
    methods
        
        function HandleOneSidedVarBounds(obj,lowerBoundVector,upperBoundVector)
            % Temporary function that creates linear inequality constraints
            % from bounds on the decision varaibles.  Using one-sided
            % (lower bound)
            if obj.conMode == 1
                obj.numBoundCon = sum([(lowerBoundVector > -inf);(upperBoundVector < inf)]);
                obj.AVarBound = zeros(obj.numBoundCon,obj.numVars);
                obj.bVarBound = zeros(obj.numBoundCon,1);
                cb = 0;
                if ~isempty(lowerBoundVector)
                    for i = 1:obj.numVars
                        % Construct the lower bound constraint matrix
                        if lowerBoundVector(i) > -inf
                            cb = cb + 1;
                            obj.AVarBound(cb,i) = 1;
                            obj.bVarBound(cb,1) = lowerBoundVector(i);
                        end
                    end
                end
                if ~isempty(upperBoundVector)
                    for i = 1:obj.numVars
                        % Construct the upper bound constraint matrix
                        if upperBoundVector(i) < inf
                            cb = cb + 1;
                            obj.AVarBound(cb,i) = -1;
                            obj.bVarBound(cb,1) = -upperBoundVector(i);
                        end
                    end
                end
            elseif obj.conMode == 2;
                obj.numBoundCon = obj.numVars;
                obj.AVarBound = eye(obj.numVars);
                if isempty(lowerBoundVector)
                    lowerBoundVector = -Inf*ones(obj.numVars,1);
                end
                if isempty(upperBoundVector)
                    upperBoundVector = Inf*ones(obj.numVars,1);
                end
                
                obj.bVarBoundLower = lowerBoundVector;
                obj.bVarBoundUpper = upperBoundVector;
                
                % Remove spurions bounds (-inf to inf) to avoid numerical
                % issues
                deleteRows = [];
                for conIdx = 1:obj.numVars
                    if obj.bVarBoundLower(conIdx) == -Inf && obj.bVarBoundUpper(conIdx) == Inf
                        deleteRows = [deleteRows conIdx];
                    end
                end
                if ~isempty(deleteRows)
                    obj.AVarBound(deleteRows,:) = [];
                    obj.bVarBoundLower(deleteRows) = [];
                    obj.bVarBoundUpper(deleteRows) = [];
                    obj.numBoundCon = size(obj.AVarBound,1);
                end
            elseif obj.conMode == 3
                obj.AVarBound  = [];
                obj.bVarBoundLower  = [];
                obj.bVarBoundUpper  = [];
                obj.numBoundCon = 0;
            end
        end
        
    end
    
    
end

