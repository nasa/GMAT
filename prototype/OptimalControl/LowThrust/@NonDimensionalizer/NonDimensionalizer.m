classdef NonDimensionalizer < handle
    %   NONDIMENSIONLIZER: Base class for non-dimensionalization of decision
    %   vector and sparse Jacobian.
    %
    %   This class performs scaling of decision vector, cost, constraints,
    %   and Jacobian. 
    %
    %   Determine and store the weights and shifts properties for
    %   the different quantities to be scaled such as decision vector,
    %   constraint vector, cost function, and Jacobian.
    %   Function: Initialize(), SetDecVecScaling_Bounds,
    %   SetConstraintScaling_UserDefined(), SetCostScaling_UserDefined().
    %   SetConstraintScaling_Jacobian()
    %
    %   Scale and unscale the decision vector.
    %   Functions: ScaleDecisionVector(), UnScaleDecisionVector()
    %
    %   Scale and unscale the constraint vector.
    %   Functions: ScaleConstraintVector(), UnScaleConstraintVector(),
    %   ScaleCostConstraintVector()
    %
    %   Scale and unscale the cost function.
    %   Functions: ScaleCostFunction(), UnScaleCostFunction(),
    %   ScaleCostConstraintVector()
    %
    %   Scale and unscale the Jacobian.
    %   Functions: ScaleJacobian(), UnScaleJacobian()
    
    properties (SetAccess = 'private')
        
        %%  The quantities to be scaled
        
        %  Integer: The number of decision variables
        numVars
        %  Integer: The number of constraints
        numCons
        
        %% Quantities used in scaling
        
        %  Real Array: Quantities that multiply the decision variables
        decVecWeight
        %  Real Array: Quantities added to decision variables
        decVecShift
        %  Real:  Quantity that multiplies the cost Function
        costWeight
        %  Real Array: Quantities that multiply the constraints
        conVecWeight
        %  Integer array:  Array of indeces for non-zero rows in Jacobian
        jacRowIdxVec
        %  Integer array:  Array of indeces for non-zero cols in Jacobian
        jacColIdxVec
        %  Integer:  number of entries in jacRowIdxVec and jacColIdxVec
        numRowsinSparsity
        
    end
    
    methods
        
        function Initialize(obj,numVars,numCons,...
                jacRowIdxVec,jacColIdxVec)
            %  Configure the non-dimensionalizer and prepare for use
            %  Initializes weights to ones and shifts to zeros
            %  @param numVars  The number of variables in the decision
            %  vector
            %  @param numCons The number of constraints in the constraint
            %  vector
            %  @param jacRowIdxVec Column vector containing row indeces of
            %  non zero Jacobian entries.  Must be same length as
            %  jacColIdxVec
            %  @param jacColIdxVec Column vector containing column indeces
            %  of non-zero Jacobian entries.  Must be same length as
            %  jacColIdxVec jacRowIdxVec
            
            %  Set nubmer of variables, constraints and sparsity info.
            obj.numVars = numVars;
            obj.numCons = numCons;
            obj.jacRowIdxVec = jacRowIdxVec;
            obj.jacColIdxVec = jacColIdxVec;
            obj.numRowsinSparsity = length(jacColIdxVec);
            %  TODO:  Validate that jacRowIdxVec and jacColIdxVec are the
            %  same length
            %  TODO:  Call the class specific methods to intialize
            %  data if necessary.
            
            % Initialize scaling parameters to ones and zeros.
            obj.decVecWeight = ones(obj.numVars)*2;
            obj.decVecShift  = zeros(obj.numVars);
            obj.conVecWeight = ones(obj.numCons)*.5;
            obj.costWeight   = 1;
        end
        
        function [decVec] =  ScaleDecisionVector(obj,decVec)
            %  Scale the decision vector
            %
            %  @param decVec The decision vector in unscaled units
            %  @return decVec.  The decision vector in scaled units
            for varIdx = 1:obj.numVars
                decVec(varIdx) = decVec(varIdx)*obj.decVecWeight(varIdx)+...
                    obj.decVecShift(varIdx);
            end
        end
        
        function [decVec] =  UnScaleDecisionVector(obj,decVec)
            %  Unscale the decision vector
            %
            %  @param decVec The decision vector in scaled units
            %  @return decVec.  The decision vector in unscaled units
            for varIdx = 1:obj.numVars
                decVec(varIdx) = (decVec(varIdx)-...
                    obj.decVecShift(varIdx))/obj.decVecWeight(varIdx);
            end
        end
        
        function [conVec] =  ScaleConstraintVector(obj,conVec)
            %  Scale the constraint vector
            %
            %  @param conVec The constraint vector in unscaled units
            %  @return conVec The constraint vector in scaled units
            for conIdx = 1:obj.numCons
                conVec(conIdx) = conVec(conIdx)*obj.conVecWeight(conIdx);
            end
        end
        
        function [conVec] =  UnScaleConstraintVector(obj,conVec)
            %  Unscale the constraint vector
            %
            %  @param conVec The constraint vector in scaled units
            %  @return conVec The constraint vector in unscaled units
            for conIdx = 1:obj.numCons
                conVec(conIdx) = conVec(conIdx)/obj.conVecWeight(conIdx);
            end
        end
        
        function [costFunc] =  ScaleCostFunction(obj,costFunc)
            %  Scale the cost function
            %
            %  @param costFunc The cost function in unscaled units
            %  @return costFunc The cost function in scaled units
            costFunc= costFunc*obj.costWeight;
        end
        
        function [costFunc] =  UnScaleCostFunction(obj,costFunc)
            %  Unscale the cost function
            %
            %  @param costFunc The cost function in scaled units
            %  @return costFunc The cost function in unscaled units
            costFunc= costFunc/obj.costWeight;
        end
        
        function [costConVec] = ScaleCostConstraintVector(obj,costConVec)
            %  Unscale cost and constraints when in a single vector
            %
            %  @param costConVec  Vector with cost and constraints in
            %  single vector in dimensional form
            %  @return costConVec Vector with cost and constraints in
            %  single vector in non-dimensional form
            costConVec(1) = obj.ScaleCostFunction(costConVec(1));
            costConVec(2:end) = ...
                obj.ScaleConstraintVector(costConVec(2:end));
        end
        
        function [jacArray] = ScaleJacobian(obj,jacArray)
            %  Scale sparse Jacobian
            %
            %  @param jacArray The sparse jacobian matrix in dimensional
            %  form
            %  @return jacArray The sparse jacobian matrix in
            %  non-dimensional form
            
            %  If Jacobian is undefined (i.e. optimizer is doing FD), then
            %  there is nothing to do.
            if obj.numRowsinSparsity == 0
                return
            end
            
            %  Scale the Jacobian
            for arrIdx = 1:obj.numRowsinSparsity
                funIdx = obj.jacRowIdxVec(arrIdx);
                varIdx = obj.jacColIdxVec(arrIdx);
                jacArray(funIdx,varIdx) = ...
                    jacArray(funIdx,varIdx)*obj.conVecWeight(funIdx)...
                    *obj.decVecWeight(varIdx);
            end
        end
        
        function [jacArray] = UnScaleJacobian(obj,jacArray)
            %  Unscale sparse Jacobian
            %
            %  @param jacArray The sparse jacobian matrix in
            %  non-dimensional form
            %  @return jacArray The sparse jacobian matrix in dimensional
            %  form
            
            %  If Jacobian is undefined (i.e. optimizer is doing FD), then
            %  there is nothing to do.
            if obj.numRowsinSparsity == 0
                return
            end
            
            %  Unscale the Jacobian
            for arrIdx = 1:obj.numRowsinSparsity
                funIdx = obj.jacRowIdxVec(arrIdx);
                varIdx = obj.jacColIdxVec(arrIdx);
                jacArray(funIdx,varIdx) = ...
                    jacArray(funIdx,varIdx)/obj.conVecWeight(funIdx)/...
                    obj.decVecWeight(varIdx);
            end
        end
        
        function SetDecVecScaling_Bounds(obj,decVecUpper,decVecLower)
            %  Set decision vector weights and shifts based on bounds
            %
            %  @param decVecUpper.  Array of upper bound values for
            %  decision vector
            %  @param decVecLower.  Array of lower bound values for
            %  decision vector

            for varIdx = 1:obj.numVars
                obj.decVecWeight(varIdx) = 1/(decVecUpper(varIdx) ...
                    -decVecLower(varIdx));
                obj.decVecShift(varIdx)  = 0.5 - decVecUpper(varIdx)*...
                    obj.decVecWeight(varIdx);
            end
        end
        
        function SetConstraintScaling_UserDefined(obj,conVecWeight)
            %  Set constraint weights based on user defined values
            %
            %  @param conVecWeight.  Array of constraint weights of
            %  length numCon x 1
            
            %  Validate the input dimensions
            if length(conVecWeight) ~= obj.numCons
                errorMsg = ['Length of constraint weight vector'  ...
                    ' must be equal to numCons'];
                errorLoc= ['NonDimensionalizer:' ...
                    'SetConstraintScaling_UserDefined'];
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            % Set the weight array
            obj.conVecWeight = conVecWeight;
        end
        
        function SetCostScaling_UserDefined(obj,costWeight)
            %  Set cost function weight based on user defined value
            %
            %  @param costWeight.  Scalar cost function weight
            
            %  Validate the input dimensions
            if size(costWeight,1) ~= 1 && size(costWeight,2) ~= 1
                errorMsg = 'Cost weight must be a 1x1 array';
                errorLoc= ['NonDimensionalizer:' ...
                    'SetCostScaling_UserDefined'];
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            % Set the weight array
            obj.costWeight = costWeight;
        end
        
    end
    
end

