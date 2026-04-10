classdef Trajectory < handle
    %TRAJECTORY The trajectory class is the executive for the optimal
    %control subsystem
    %
    %   Regarding Scaling:  All data internally is stored in dimensional
    %   units.  Scaling and unscaling is performed using the PUBLIC
    %   interface to the class. All protected and private data interfaces
    %   use dimensional data.  All public interfaces assume dimensional
    %   data if flag is not passed in.
    
    
    properties (SetAccess = 'public')
        
        % Real.  The lower bound on the cost function
        costLowerBound
        % Real.  The upper bound on the cost function
        costUpperBound
        % String.  The name of the user guess function
        guessFunctionName = '';
        % Pointer to a user Guess Class
        guessObject
        % String.  The name of the user plot function
        plotFunctionName = '';
        % UserPathFunction object.  User path function class
        pathFunctionObject = '';
        % UserPointFunction object.  User point function class
        pointFunctionObject = '';
        % Bool. Flag indicating whether or not to show user plot
        showPlot                = false();
        % Int. Number of func evals between plot updates
        plotUpdateRate          = 15;
        % Array of Phase objects. These are the phases in the problem.
        phaseList
        % Integer.  Maximum allowed number of mesh iterations
        maxMeshRefinementCount = 0;
		% Boolean: Flag to indicate if the problem will be solved in auto-scaled units or not
		ifScaling = false()
		% Real. The scaling of the cost function (If the auto-scaling utility is used)
		costScaling = 1;
        refineInitialMesh = false
        
        % Helper classes
        % Optimizer object. Optimizer helper class
        TrajOptimizer = exec.Optimizer();
        
                % ====  Optimizer Tolerances
        % Tolerance on optimality
        optimalityTol = 5e-7;
        % Tolerance on feasiblity
        feasibilityTol = 1e-3;
        % Maximum allowed total iterations (minor + major)
        maxTotalIterations = 300000;
        % Maximum allowed major iterations
        maxMajorIterations = 1000;
        
    end
    
    properties (Access = 'private')
        
        % ====  Decision vector related data members
        
        %  Real array. Total desicion vector for optimization
        %  problem containing chunks for all phases.
        decisionVector
        % Real array.  The lower bound vector for the decision vector
        decisionVecLowerBound
        % Real array.  The upper bound vector for the decision vector
        decisionVecUpperBound
        % Integer array.  The ith entry is the start of the decision vector
        % chunk for the ith phase
        decVecStartIdx
        % Integer array.  The ith entry is the index for the start of
        % the decision vector chunk for the ith phase
        decVecEndIdx
        %  Integer.  Number of elements in the trajectory decision
        %  vector
        totalnumDecisionParams
        %  Integer Array.  The ith entry is the number of decision
        %  parameters for the ith phase
        numPhaseDecisionParams
        % Integer.  The number of phases in the problem
        numPhases
        % Integer array.  The ith element is the number of constraints
        % in the ith phase.
        numPhaseConstraints
        % Integer.  The total number of constraints including all phases
        % and linkages
        totalnumConstraints
        % Integer.  Total number or boundary functions
        numBoundFunctions = 0;
        % Real. The cost function value
        costFunction
        %  Real Array.  The lower bound vector for all constraints in the
        %  problem (i.e. for all phases).
        allConLowerBound
        %  Real Array.  The upper bound vector for all constraints in the
        %  problem (i.e. for all phases).
        allConUpperBound
        %  Integer Array.  The ith element is the index indicates where
        %  the constraints for the ith phase start in the concatenated
        %  constraint vector
        conPhaseStartIdx
        %  The vector of constraint values
        constraintVec
		        
        % ==== Sparsity pattern data members
        
        %  Sparse matrix.  sparsity pattern for the cost function
        sparsityCost
        %  Sparse matrix.  Sparsity pattern for the constraints
        sparsityConstraints
        %  Sparse matrix.  The sparsity pattern for the complete problem
        %  including cost and constraints.
        sparsityPattern
        
        % ==== Helper classes
        

        % PointFunctionManager object. Manager for point function evaluation and partials
        PointFuncManager
		
        %  Non-dimensionalizer helper class
        ScaleHelper   = ScaleUtility();
		%  Integer array: which state variable corresponds to each defect constraint
		whichStateVar
		%  Bool array: vector to track whether constraints are defect or not
		ifDefect	
        
        % ==== Housekeeping parameters
        
        %  Bool.  Flag indicating whether or not optimization is under way
        isOptimizing       = false();
        %  Bool.  Flag indicating that optimization is complete
        isFinished         = false();
        %  Int.  Number of func evals since the last plot update
        plotUpdateCounter  = 1;
        %  Bool.  Flag to write out debug data
        displayDebugStatus = 0;
        %  Bool.  Flag for when perturbing for finite differencing
        isPerturbing = 0;
        %  Number of function evals during optimization
        numFunEvals = 0;
        %  Bool.  Indicates if mode is mesh refinement
        isMeshRefining
        % Integer.  Number of mesh refinements performed
        meshRefinementCount
        
    end
    
    methods (Access = public)
        
        function obj = Trajectory()
            import userfunutils.UserPointFunctionManager
            obj.PointFuncManager = userfunutils.UserPointFunctionManager();
        end
        
        function obj = Initialize(obj)
            % Initializes the trajectory, all phases, and helper classes.
            
            %  Assign phase numbers as they are required early in the
            %  intialization process.
            for phaseIdx = 1:length(obj.phaseList)
                obj.phaseList{phaseIdx}.SetPhaseNum(phaseIdx);
            end
            
            % Add the phase list to the path and point functions
            obj.pointFunctionObject.phaseList = obj.phaseList;
            obj.pathFunctionObject.phaseList = obj.phaseList;
  
            % Continue with initializations
            obj.InitializePhases();
            obj.InitializePointFunctions();
            obj.numBoundFunctions = obj.PointFuncManager.numBoundaryFunctions;
            obj.SetBounds();
            obj.SetInitialGuess();
            obj.TrajOptimizer.Initialize();
            %obj.WriteSetupReport();
            obj.PrepareToOptimize();
            obj.InitializeJacHelper();
			if obj.ifScaling
				obj.InitializeScaleUtility();
			end
        end
        
        function numPhases = GetNumPhases(obj)
            % Returns the number of phases.
           numPhases = obj.numPhases;    
        end
        
		function InitializeScaleUtility(obj)
			% Initilizes the scale utility if the problem is being auto-scaled.
			
			% Get the sparsity pattern
			[jacRowIdx,jacColIdx,s] = find(obj.sparsityPattern);
			% Initialize the utility's arrays
			obj.ScaleHelper.Initialize(obj.totalnumDecisionParams,obj.totalnumConstraints,jacRowIdx,jacColIdx);
			% Set the decision vector scaling based on its bounds
			obj.ScaleHelper.SetDecVecScaling_Bounds(obj.decisionVecUpperBound,obj.decisionVecLowerBound);
			% Scale the cost function
			obj.ScaleHelper.SetCostScaling_UserDefined(obj.costScaling);
			
			% Set the scale utility's helper arrays from the traj class
			obj.ScaleHelper.SetWhichStateVar(obj.whichStateVar);
			obj.ScaleHelper.SetIfDefect(obj.ifDefect);
			
			% Set the decision vector to get an initial Jacobian
			obj.SetDecisionVector(obj.decisionVector,true());
			J = obj.GetJacobian(true());
			
			% Set the constraint scaling
			obj.ScaleHelper.SetConstraintScaling_DefectAndUser(J);
			% obj.ScaleHelper.SetConstraintScaling_Jacobian(J);
		end
		
        function InitializeJacHelper(obj)
            
            % Loop over phases and determine numbers of functions and
            % non-zeros A*Z + B*q and dq/dZ
%             numPhaseCons = 0;
%             numAMatNonZeros = 0;
%             numBMatNonZeros = 0;
%             numQMatNonZeros = 0;
%             for phaseIdx = 1:obj.numPhases
%                 numPhaseCons = numPhaseCons + obj.phaseList{phaseIdx}.GetNumTotalConNLP();
%                 [numAZeros,numBZeros,numQZeros] = obj.phaseList{phaseIdx}.GetNumNLPNonZeros();
%                 numAMatNonZeros = numAMatNonZeros + numAZeros;
%                 numBMatNonZeros = numBMatNonZeros + numBZeros;
%                 numQMatNonZeros = numQMatNonZeros + numQZeros;
%             end
            % Now loop over phases and insert portions
            %            keyboard
        end
        
        function InitializePointFunctions(obj)
            % Intializes point function manager and user point function
            
            %  Create vector of pointers to all phasa
            for phaseIdx = 1:obj.numPhases
                phasePointerVec{phaseIdx} = obj.phaseList{phaseIdx}; %#ok<AGROW>
            end
            if obj.isMeshRefining
                obj.PointFuncManager.isInitializing = true();
            end
            obj.PointFuncManager.Initialize(obj.pointFunctionObject,phasePointerVec,...
                obj.totalnumDecisionParams,obj.decVecStartIdx)
            obj.numBoundFunctions = obj.PointFuncManager.numBoundaryFunctions;
            
            obj.totalnumConstraints = obj.totalnumConstraints + obj.numBoundFunctions;
			
			obj.ifDefect = [obj.ifDefect,zeros(1,obj.numBoundFunctions)];
			obj.whichStateVar = [obj.whichStateVar,zeros(1,obj.numBoundFunctions)];
        end
        
        function obj = PrepareToOptimize(obj)
            %  Things that must be done after Initialize() but before Optimize()
            for phaseIdx = 1:obj.numPhases
                obj.phaseList{phaseIdx}.PrepareToOptimize();
            end
            obj.SetSparsityPattern();
        end
        
        function obj = SetDecisionVector(obj,decVector,~)
            %  Sets decision vector on the phases.
            % This forces MATLAB to refresh command window
%             pause(0.00001)
            %  if isScaled = 1, incoming vector is non-dimensional.
            %  if isScaled ~= 1, incoming vector is non-dimensional.
            if ~length(decVector) == obj.totalnumDecisionParams
                errorMsg = ['Length of decisionVector must be ' ...
                    ' equal to totalnumDecisionParams'];
                errorLoc  = 'Trajectory:SetDecisionVector';
                ME = MException(errorLoc,errorMsg);
                throw(ME);
            end
            
            %  Now loop over phases and set on each phase
            for phaseIdx = 1:obj.numPhases
                obj.phaseList{phaseIdx}.SetDecisionVector(...
                    decVector(obj.decVecStartIdx(phaseIdx):...
                    obj.decVecEndIdx(phaseIdx)));
            end
            
            %  Handle plotting
            if obj.showPlot
                obj.PlotUserFunction();
            end
            
        end
        
        function Jacobian=GetJacobian(obj,~)
            %  Returns the sparse Jacobian
            
            %  Plus one is for cost function offset.
            Jacobian = sparse(1 + obj.totalnumConstraints,obj.totalnumDecisionParams);
            for phaseIdx = 1:obj.numPhases
                %  Plus one is for cost function offset.
                rowLow = obj.conPhaseStartIdx(phaseIdx) + 1;
                rowHigh = rowLow + obj.numPhaseConstraints(phaseIdx) -1;
                colLow = obj.decVecStartIdx(phaseIdx);
                colHigh = colLow + obj.numPhaseDecisionParams(phaseIdx) - 1;
                Jacobian(1,colLow:colHigh) = obj.phaseList{phaseIdx}.GetCostJacobian();  %#ok<SPRIX>
                Jacobian(rowLow:rowHigh,colLow:colHigh) =...
                    obj.phaseList{phaseIdx}.GetConJacobian(); %#ok<SPRIX>
                
                funcIdxLow = obj.totalnumConstraints- obj.numBoundFunctions + 1 + 1;
                funcIdxHigh = obj.totalnumConstraints + 1;
                funcIdxs = funcIdxLow:funcIdxHigh;
                obj.PointFuncManager.EvaluateUserJacobian()
                if obj.PointFuncManager.hasBoundaryFunctions;
                    Jacobian(funcIdxs,:) = ...
                        obj.PointFuncManager.ComputeBoundNLPJacobian(); %#ok<SPRIX>
                end
                if obj.PointFuncManager.hasCostFunction;
                    Jacobian(1,:) = Jacobian(1,:) + ...
                        obj.PointFuncManager.ComputeCostNLPJacobian(); %#ok<SPRIX>
                end
            end
        end
        
        function [AllFunctions] = GetCostConstraintFunctions(obj,~)
            %  Computes cost and constraint functions
            obj.numFunEvals = obj.numFunEvals + 1;
            conVec  = GetConstraintVector(obj);
            costFun = GetCostFunction(obj);
            if obj.PointFuncManager.hasBoundaryFunctions;
                AllFunctions = [costFun;conVec;obj.PointFuncManager.ComputeBoundNLPFunctions()];
            else
                AllFunctions = [costFun;conVec];
            end
            pause(0.0001)
        end
        
		function [ConstraintFunctions,ifEquality] = GetScaledConstraintFunctions(obj) 
			AllFunctions = GetCostConstraintFunctions(obj);
			ConstraintFunctions = zeros(length(AllFunctions)-1,1);
			ifEquality = zeros(length(AllFunctions)-1,1);
			for i = 1:length(AllFunctions)-1
				if obj.allConLowerBound(i) == obj.allConUpperBound(i)
					ConstraintFunctions(i) = AllFunctions(i+1)-obj.allConLowerBound(i);
					ifEquality(i) = 1;
					%if abs(ConstraintFunctions(i)) > 1e-6
					%	disp('Nonfeasible equality')
					%end
				else
					ConstraintFunctions(i) = (AllFunctions(i+1)-obj.allConLowerBound(i))/(obj.allConUpperBound(i)-obj.allConLowerBound(i));
					%if ConstraintFunctions(i) < -1e-6 || ConstraintFunctions(i) > 1+1e-6
					%	disp('Nonfeasible bounded')
					%end						
				end
			end
		end
		
        function [constraintVecNames] = GetConstraintVectorNames(obj)
            %  Loop over the phases and concatenate the constraint vectors
            constraintVecNames = {};
            for phaseIdx = 1:obj.numPhases
				newNames = obj.phaseList{phaseIdx}.GetConstraintVectorNames();
                constraintVecNames = {constraintVecNames{1:end},newNames{1:end}}; %#ok<AGROW>
            end
            if obj.PointFuncManager.hasBoundaryFunctions
				newNames = obj.PointFuncManager.funcData.algData.GetFunctionNames();
				if isempty(newNames)
					numConstraints = length(obj.PointFuncManager.ComputeBoundNLPFunctions());
					for i = 1:numConstraints
                		constraintVecNames = {constraintVecNames{1:end},['User Point Function Constraint ',num2str(i)]};
					end
				else
                	constraintVecNames = {constraintVecNames{1:end},newNames{1:end}};
				end
            end
        end
		
        function [z,F,xmul,Fmul,info] = Optimize(obj)
            %  Executes the optimization problem
            
            %%  If the user has requested to refine the initial mesh, then run the mesh refiner
            if obj.refineInitialMesh
                obj.Initialize();
                isMeshRefined = false;
                meshIterCount = 0;
                if ~isempty(obj.plotFunctionName)
                   feval(obj.plotFunctionName,obj);
                end
                while ~isMeshRefined && meshIterCount < 2 
                    isMeshRefined = true;
                    for phaseIdx = 1:obj.numPhases
                        obj.phaseList{phaseIdx}.TransUtil.SetIsInitialMesh(true);
                        [isRefined] =  obj.phaseList{phaseIdx}.RefineMesh(true);
                        if ~isRefined
                            isMeshRefined = false;
                        end
                        obj.phaseList{phaseIdx}.SetIsRefining(false);
                        obj.phaseList{phaseIdx}.Initialize();
                        if ~isempty(obj.plotFunctionName)
                           clf;
                           feval(obj.plotFunctionName,obj)
                        end
                    end
                    meshIterCount = meshIterCount + 1;
                end
            end
            for phaseIdx = 1:obj.numPhases
               obj.phaseList{phaseIdx}.TransUtil.SetIsInitialMesh(false);
            end
            
            %%  Initialize the trajectory class and prepare for optimization loop.
            obj.Initialize();
            obj.meshRefinementCount = 0;
            IsMeshRefinementFinished = 0;
            while obj.meshRefinementCount <= obj.maxMeshRefinementCount
                
                %  Optimize on the current mesh (first loop is on user's mesh
                obj.isOptimizing      = true();
                obj.plotUpdateCounter = 1;
                obj.numFunEvals       = 0;
                obj.isMeshRefining = false;
				
                clear mex
                obj.TrajOptimizer.Initialize();
                obj.SetOptimizerParams();
				if obj.ifScaling
					obj.TrajOptimizer.SetScaling(true());
					% Call SNOPT with the inputs scaled
					[z,F,xmul,Fmul,info] = obj.TrajOptimizer.Optimize(...
								obj.ScaleHelper.ScaleDecisionVector(obj.decisionVector),...
								-.5*(ones(size(obj.decisionVector))),...
								 .5*(ones(size(obj.decisionVector))), ...
								[obj.costLowerBound*obj.ScaleHelper.GetCostWeight();obj.allConLowerBound.*obj.ScaleHelper.GetConVecWeights()],...
								[obj.costUpperBound*obj.ScaleHelper.GetCostWeight();obj.allConUpperBound.*obj.ScaleHelper.GetConVecWeights()],...
								obj.sparsityPattern);

					% Unscale the problem and answer!
					z = obj.ScaleHelper.UnScaleDecisionVector(z);
					F(1) = obj.ScaleHelper.UnScaleCostFunction(F(1));
					F(2:end) = obj.ScaleHelper.UnScaleConstraintVector(F(2:end));
				else
					obj.TrajOptimizer.SetScaling(false());
	                [z,F,xmul,Fmul,info] = obj.TrajOptimizer.Optimize(...
	                    obj.decisionVector,obj.decisionVecLowerBound,...
	                    obj.decisionVecUpperBound, ...
	                    [obj.costLowerBound;obj.allConLowerBound],...
	                    [obj.costUpperBound;obj.allConUpperBound],...
	                    obj.sparsityPattern);
				end
                
                % Check to see if mesh refinement is required
                if obj.meshRefinementCount == obj.maxMeshRefinementCount
                    ifUpdateMeshInterval = 0;
                else
                    ifUpdateMeshInterval = 1;
                end
                IsMeshRefinementFinished = 1;
                obj.isMeshRefining = true;
                for phaseIdx = 1:obj.numPhases
                    [isMeshRefined] =  obj.phaseList{phaseIdx}.RefineMesh(ifUpdateMeshInterval);
                    if ~isMeshRefined
                        IsMeshRefinementFinished = false;
                    end
                end
                
                %  Get ready to optimize again, or break out if done.
                if IsMeshRefinementFinished
                    % Done, so break out of the loop and finalize
                    disp(' ')
                    disp(['Success: Mesh Refinement Criteria Were Satisfied in ' ...
                        num2str(obj.meshRefinementCount) ' Iterations'])
                    break
				elseif ifUpdateMeshInterval
                    % Not done, so prepare for next optimization
                    obj.Initialize();
                    obj.isOptimizing      = true();
                    obj.numFunEvals       = 0;
                    obj.meshRefinementCount = obj.meshRefinementCount + 1;
                    disp(' ')
                    disp('===========================================')
                    disp(['====  Mesh Refinement Status:  Iteration ' ...
                        num2str(obj.meshRefinementCount)]);
                    disp('===========================================')
				else
                    obj.meshRefinementCount = obj.meshRefinementCount + 1;
                end
            end
            
            % Write out messages for mesh refinement convergence criter
            if ~IsMeshRefinementFinished && obj.maxMeshRefinementCount >= 1
                disp(' ')
                disp(['Warning: Mesh Refinement Criteria Was Not ' ...
                    'Satisfied Before Maximum Iterations Were Reached'])
            elseif ~IsMeshRefinementFinished && obj.maxMeshRefinementCount == 0
                disp(' ')
                disp(['Warning: Mesh Refinement Was Not Performed ' ...
                    'Because Max Iterations Are Set To Zero'])
            end
            
            %   Configure house-keeping data
            obj.isMeshRefining = false;
            obj.isOptimizing = false();
            obj.isFinished   = true();
            obj.PlotUserFunction();
            %obj.WriteSetupReport(1);
            
        end

        function grad = GetGrad(obj, decisionVec)
            obj.SetDecisionVector(decisionVec);
            allFuncs = obj.GetCostConstraintFunctions();
            numConstr = numel(allFuncs) - 1;            
            grad = [1 zeros(1, numConstr)]*obj.GetJacobian(decisionVec);
        end
        
        function jac = GetConstrJac(obj, decisionVec)
            obj.SetDecisionVector(decisionVec);
            allFuncs = obj.GetCostConstraintFunctions();
            numConstr = numel(allFuncs) - 1;            
            jac = sparse([zeros(numConstr,1) eye(numConstr)]*obj.GetJacobian(decisionVec));
        end
        
        
        function constr= GetConstrVec(obj, decisionVec)
            obj.SetDecisionVector(decisionVec);
            allFuncs = obj.GetCostConstraintFunctions();
            numConstr = numel(allFuncs) - 1;
            constr = [zeros(numConstr,1) eye(numConstr)]*allFuncs;            
            obj.GetJacobian(decisionVec);
        end
        
        function constr= GetCost(obj, decisionVec)
            obj.SetDecisionVector(decisionVec);
            allFuncs = obj.GetCostConstraintFunctions();
            numConstr = numel(allFuncs) - 1;
            constr = [1 zeros(1, numConstr)]*allFuncs;            
            obj.GetJacobian(decisionVec);
        end
        
        function hess= GetHess(obj, decisionVec,  sigma,lambda)
            decVecNow = decisionVec;
            obj.SetDecisionVector(decVecNow);
            % element-wise hessian computation
            pert = 3e-5;
            numVars = numel(decVecNow);
            normFunc = obj.GetCostConstraintFunctions();
            numAllFunc = numel(normFunc);
            deltaFunc = zeros(numAllFunc, numVars);
            deltaFunc2 = {};
            obj.GetJacobian(); % redandunt but necessary for now
            for idx = 1:numVars
                decVec = decVecNow;
                decVec(idx) = decVec(idx) + pert;
                obj.SetDecisionVector(decVec);
                currFunc = obj.GetCostConstraintFunctions();
                obj.GetJacobian();
                deltaFunc(:,idx) = currFunc;
                for idx2 = 1:idx
                    decVec2 = decVec;
                    decVec2(idx2) = decVec2(idx2) + pert;
                    obj.SetDecisionVector(decVec2);
                    currFunc2 = obj.GetCostConstraintFunctions();
                    obj.GetJacobian();
                    deltaFunc2{idx,idx2} = currFunc2;
                end
            end
            hessTensor = zeros(numVars, numVars, numAllFunc);
            for idx = 1:numAllFunc
                for rowIdx = 1:numVars
                    for colIdx = 1:rowIdx
                        value = 1/pert^2*(deltaFunc2{rowIdx,colIdx}(idx)-deltaFunc(idx,rowIdx)-deltaFunc(idx,colIdx)+normFunc(idx));
                        if value ~= 0.0
                            hessTensor(rowIdx, colIdx,idx) = value;
                        end
                    end
                end
            end
            hess = hessTensor(:,:,1)*sigma;
            for idx = 1:numAllFunc - 1
                hess = hess + hessTensor(:,:,idx+1)*lambda(idx);
            end
            hess = sparse(hess);
        end
        
        function [z,F] = OptimizeIPOPT(obj)
            % added by YK            
            %  Initialize the trajectory class and prepare for optimization loop.
            obj.Initialize();            
            obj.meshRefinementCount = 0;
            IsMeshRefinementFinished = 0;
            constrJacPattern = obj.sparsityPattern(2:end,:);
            decVec = obj.decisionVector;

            hessSparsityPattern = sparse(tril(ones(numel(decVec),numel(decVec))));
            
            funcs.objective         = @(decisionVec) obj.GetCost(decisionVec);
            funcs.constraints       = @(decisionVec) obj.GetConstrVec(decisionVec);
            funcs.gradient          = @(decisionVec) obj.GetGrad(decisionVec);
            funcs.jacobian          = @(decisionVec) obj.GetConstrJac(decisionVec);
            funcs.jacobianstructure = @() constrJacPattern;   
            funcs.hessian           = @(decisionVec, sigma,lambda) obj.GetHess(decisionVec, sigma,lambda);
            funcs.hessianstructure  = @() hessSparsityPattern;    % now hessianSparsityPattern is upper triangular but ipopt wants lower one.
%             
            %             
            while obj.meshRefinementCount <= obj.maxMeshRefinementCount
                
                %  Optimize on the current mesh (first loop is on user's mesh
                obj.isOptimizing      = true();
                obj.plotUpdateCounter = 100;
                obj.numFunEvals       = 0;
                obj.isMeshRefining = false;
				
                [z,info] = obj.TrajOptimizer.Optimize(funcs,...
                    obj.decisionVector,obj.decisionVecLowerBound,...
                    obj.decisionVecUpperBound, ...
                    obj.allConLowerBound,...
                    obj.allConUpperBound);
                
                % Check to see if mesh refinement is required
                if obj.meshRefinementCount == obj.maxMeshRefinementCount
                    ifUpdateMeshInterval = 0;
                else
                    ifUpdateMeshInterval = 1;
                end
                IsMeshRefinementFinished = 1;
                obj.isMeshRefining = true;
                for phaseIdx = 1:obj.numPhases
                    [isMeshRefined] =  obj.phaseList{phaseIdx}.RefineMesh(ifUpdateMeshInterval);
                    if ~isMeshRefined
                        IsMeshRefinementFinished = false;
                    end
                end
                
                %  Get ready to optimize again, or break out if done.
                if IsMeshRefinementFinished
                    % Done, so break out of the loop and finalize
                    disp(' ')
                    disp(['Success: Mesh Refinement Criteria Were Satisfied in ' ...
                        num2str(obj.meshRefinementCount) ' Iterations'])
                    break
				elseif ifUpdateMeshInterval
                    % Not done, so prepare for next optimization
                    obj.Initialize();
                    obj.isOptimizing      = true();
                    obj.numFunEvals       = 0;
                    obj.meshRefinementCount = obj.meshRefinementCount + 1;
                    disp(' ')
                    disp('===========================================')
                    disp(['====  Mesh Refinement Status:  Iteration ' ...
                        num2str(obj.meshRefinementCount)]);
                    disp('===========================================')
				else
                    obj.meshRefinementCount = obj.meshRefinementCount + 1;
                end
            end
            
            % Write out messages for mesh refinement convergence criter
            if ~IsMeshRefinementFinished && obj.maxMeshRefinementCount >= 1
                disp(' ')
                disp(['Warning: Mesh Refinement Criteria Was Not ' ...
                    'Satisfied Before Maximum Iterations Were Reached'])
            elseif ~IsMeshRefinementFinished && obj.maxMeshRefinementCount == 0
                disp(' ')
                disp(['Warning: Mesh Refinement Was Not Performed ' ...
                    'Because Max Iterations Are Set To Zero'])
            end
            
            %   Configure house-keeping data
            obj.isMeshRefining = false;
            obj.isOptimizing = false();
            obj.isFinished   = true();
            obj.PlotUserFunction();
            %obj.WriteSetupReport(1);
            
            % set decision vector data
            obj.decisionVector = z; %why do we need this?
            obj.SetDecisionVector(z,true()); % added by YK            
            F = obj.GetCostConstraintFunctions();
        end        
        
        function SetOptimalityTol(obj,value)
            % Set the tolerance on optimality
            obj.optimalityTol = value;
        end
        
        function SetFeasibilityTol(obj,value)
            % Set the tolerance on feasbility
            obj.feasibilityTol = value;
        end
        
        function SetMaxTotalIterations(obj,value)
            % Set maximum total allowed iterations
            obj.maxTotalIterations = value;
        end
        
        function SetMaxMajorIterations(obj,value)
            % Set maximum number of major iterations
            obj.maxMajorIterations = value;
        end
		        
        function SetOptimizerParams(obj);
            % Calls optimizer helper class and sets values
            obj.TrajOptimizer.SetOptimalityTol(obj.optimalityTol);
            obj.TrajOptimizer.SetFeasibilityTol(obj.feasibilityTol);
            obj.TrajOptimizer.SetMaxTotalIterations(obj.maxTotalIterations);
            obj.TrajOptimizer.SetMaxMajorIterations(obj.maxMajorIterations);
        end
        
        %  Handles plotting of user function
        function PlotUserFunction(obj)
            % Manages plotting of user plot function
            
            %  Plot the function if at solution or at rate
            if ~isempty(obj.plotFunctionName)
                % Show the plot if criteria pass
                if (obj.showPlot && obj.isOptimizing && ...
                        obj.plotUpdateCounter == 1 && ~obj.isPerturbing)...
                        || (obj.showPlot && obj.isFinished);
                    feval(obj.plotFunctionName,obj);
                end
                
                %  Update rate counter, and reset if needed
                obj.plotUpdateCounter = obj.plotUpdateCounter + 1;
                if obj.plotUpdateCounter == obj.plotUpdateRate
                    obj.plotUpdateCounter = 1;
                end
            end
        end
        
        function stateArray = GetStateArray(obj,phaseIdx)
            % Returns state decision vector data given phase Idx
            if phaseIdx <= obj.numPhases
                stateArray = obj.phaseList{phaseIdx}.DecVector.GetStateArray();
            else
                error('phaseIdx must be <= numPhases')
            end
        end
        
        function controlArray = GetControlArray(obj,phaseIdx)
            % Returns control decision vector data given phase Idx
            if phaseIdx <= obj.numPhases
                controlArray = obj.phaseList{phaseIdx}.DecVector.GetControlArray();
            else
                error('phaseIdx must be <= numPhases')
            end
        end
        
        function timeArray = GetTimeArray(obj,phaseIdx)
            % Returns time decision vector data given phase Idx
            if phaseIdx <= obj.numPhases
                timeArray = obj.phaseList{phaseIdx}.TransUtil.GetTimeVector();
            else
                error('phaseIdx must be <= numPhases')
            end
        end
        
        function meshCount = GetMeshRefinementCount(obj)
            % Returns meshRefinementCount
            meshCount = obj.meshRefinementCount;
        end
        
        function sparsityPattern = GetSparsityPattern(obj)
            sparsityPattern = obj.sparsityPattern;
        end
      
    end
    
    %% Private methods
    methods (Access = private)
        
        function SetCostLowerBound(obj,costLowerBound)
            %  Sets lower bound on cost
            obj.costLowerBound = costLowerBound;
        end
        
        function costLowerBound = GetCostLowerBound(obj)
            %  Sets lower bound on cost
            costLowerBound = obj.costLowerBound;
        end
        
        %% Intialize the phase helper classes and phase related data
        function InitializePhases(obj)
            
            %  Get properties of each phase for later book-keeping
            obj.numPhases = length(obj.phaseList);
            if obj.numPhases <= 0
                error('Trajectory requires at least one phase.');
            end
            
            %  Initialize phases and a few trajectory data members
            obj.numPhaseConstraints = 1;
            obj.totalnumDecisionParams = 0;
            obj.totalnumConstraints = 0;
			
			% Define an index to track location in the constraint vector
			if obj.ifScaling
				constraintStartIdx = 1;
			end
            
			for phaseIdx = 1:obj.numPhases
                
                %  Configure user functions
                obj.phaseList{phaseIdx}.guessFunctionName = ...
                    obj.guessFunctionName;
                obj.phaseList{phaseIdx}.guessObject = obj.guessObject;
                obj.phaseList{phaseIdx}.pathFunctionObj = obj.pathFunctionObject;
                
                %  Intialize the current phase
                obj.phaseList{phaseIdx}.Initialize();
                
                %  Extract decision vector and constraint properties
                obj.numPhaseConstraints(phaseIdx) = ...
                    obj.phaseList{phaseIdx}.Config.GetNumTotalConNLP();
                obj.numPhaseDecisionParams(phaseIdx) = ...
                    obj.phaseList{phaseIdx}.Config.GetNumDecisionVarsNLP();
                obj.totalnumDecisionParams = ...
                    obj.totalnumDecisionParams + ...
                    obj.numPhaseDecisionParams(phaseIdx);
                obj.totalnumConstraints = obj.totalnumConstraints + ...
                    obj.phaseList{phaseIdx}.Config.GetNumTotalConNLP();
                
				if obj.ifScaling
					% Initialize the scale utility helper arrays
					obj.ifDefect(constraintStartIdx:obj.totalnumConstraints) = zeros(obj.totalnumConstraints-constraintStartIdx+1,1);
					obj.whichStateVar(constraintStartIdx:obj.totalnumConstraints) = zeros(obj.totalnumConstraints-constraintStartIdx+1,1);		
					
					% Use the phase accessor functions to determine which constraints are defects		
					obj.ifDefect([obj.phaseList{phaseIdx}.GetDefectConStartIdx():obj.phaseList{phaseIdx}.GetDefectConEndIdx()] + constraintStartIdx-1) = 1;
				
					% Loop through each mesh point in order to figure out the relevant state variable for each defect constraint
					% nMeshPts = sum(obj.phaseList{phaseIdx}.GetMeshIntervalNumPoints());
					nMeshPts = obj.phaseList{phaseIdx}.Config.GetNumTotalConNLP() / obj.phaseList{phaseIdx}.GetNumStateVars();
					defectConIdx = constraintStartIdx;
					for meshIdx = 1:nMeshPts
						obj.whichStateVar(defectConIdx:defectConIdx+obj.phaseList{phaseIdx}.GetNumStateVars()-1) = 1:obj.phaseList{phaseIdx}.GetNumStateVars();
						defectConIdx = defectConIdx + obj.phaseList{phaseIdx}.GetNumStateVars();
					end
				
					% Update the constraint vector index 
					constraintStartIdx = obj.totalnumConstraints + 1;
					
				end
				
            end
            obj.SetChunkIndeces();
        end
        
        %  Compute all constraints
        function [constraintVec] = GetConstraintVector(obj)
            %  Loop over the phases and concatenate the constraint vectors
            constraintVec = [];
            for phaseIdx = 1:obj.numPhases
                constraintVec = [constraintVec ; ...
                    obj.phaseList{phaseIdx}.GetConstraintVector()]; %#ok<AGROW>
            end
            
            %  Set the constraint vector on the class for use in partial
            %  derivatives later
            obj.constraintVec = constraintVec;
        end
	        
        %  Compute the cost function.
        function [costFunction] = GetCostFunction(obj)
            costFunction = 0;
            if obj.PointFuncManager.hasCostFunction
                costFunction = obj.PointFuncManager.ComputeCostNLPFunctions();
            end
            for phaseIdx = 1:obj.numPhases
                costFunction = costFunction + ...
                    obj.phaseList{phaseIdx}.GetCostFunction();
            end
            
            %  Set the cost function for use in partials
            obj.costFunction = costFunction;
        end
        
        %  Configure start and stop indeces for different chunks of the
        %  decision vector and constraint vector
        function obj = SetChunkIndeces(obj)
            
            %  Loop over phases and set start and end indeces for different
            %  chunks of the decision vector
            for phaseIdx = 1:obj.numPhases
                
                %  Indeces for start and end of decision vector chunks
                obj.decVecStartIdx(phaseIdx) = ...
                    obj.totalnumDecisionParams - ...
                    sum(obj.numPhaseDecisionParams(phaseIdx:end)) + 1;
                obj.decVecEndIdx(phaseIdx) = ...
                    obj.decVecStartIdx(phaseIdx) ...
                    + obj.numPhaseDecisionParams(phaseIdx) - 1;
                
                %  Function indeces
                obj.conPhaseStartIdx(phaseIdx) = ...
                    sum(obj.numPhaseConstraints(1:phaseIdx-1)) + 1;
                
            end
        end
        
        %  Set bounds for decision vector and constraints
        function obj = SetBounds(obj)
            obj.SetConstraintBounds();
            obj.SetDecisionVectorBounds();
        end
        
        %  Sets upper and lower bounds on the complete constraint vector
        %  concatenating all phases.
        function obj = SetConstraintBounds(obj)
            
            %  Loop over all phases concatenating constraint vectors
            obj.allConLowerBound = [];
            obj.allConUpperBound = [];
            for phaseIdx = 1:obj.numPhases
                obj.allConLowerBound = [obj.allConLowerBound; ...
                    obj.phaseList{phaseIdx}.allConLowerBound];
                obj.allConUpperBound = [obj.allConUpperBound;...
                    obj.phaseList{phaseIdx}.allConUpperBound];
            end
            
            if obj.PointFuncManager.hasBoundaryFunctions
                obj.allConLowerBound = [obj.allConLowerBound;...
                    obj.PointFuncManager.GetConLowerBound()];
                obj.allConUpperBound = [obj.allConUpperBound;...
                    obj.PointFuncManager.GetConUpperBound()];
            end
            
        end
        
        function obj = SetDecisionVectorBounds(obj)
            %  Sets upper and lower bounds on the complete decision
            obj.decisionVecLowerBound = zeros(obj.totalnumDecisionParams,1);
            obj.decisionVecUpperBound = zeros(obj.totalnumDecisionParams,1);
            lowIdx = 1;
            for phaseIdx = 1:obj.numPhases
                highIdx = lowIdx + ...
                    obj.numPhaseDecisionParams(phaseIdx)-1;
                obj.decisionVecLowerBound(lowIdx:highIdx) = ...
                    obj.phaseList{phaseIdx}.decisionVecLowerBound;
                obj.decisionVecUpperBound(lowIdx:highIdx) = ...
                    obj.phaseList{phaseIdx}.decisionVecUpperBound;
                lowIdx = highIdx + 1;
            end
        end
        
        function obj = SetInitialGuess(obj)
            %  Computes initial guess for complete decision vector
            obj.decisionVector = zeros(obj.totalnumDecisionParams,1);
            lowIdx = 1;
            for phaseIdx = 1:obj.numPhases
                highIdx=lowIdx+obj.numPhaseDecisionParams(phaseIdx)-1;
                obj.decisionVector(lowIdx:highIdx) = ...
                    obj.phaseList{phaseIdx}.DecVector.decisionVector;
                lowIdx = highIdx + 1;
            end
        end
        
        function obj = SetSparsityPattern(obj)
            %  Set the sparsity pattern for the  NLP problem
            
            %  Initialize the sparse matrix
            obj.sparsityPattern = sparse(1+obj.totalnumConstraints,obj.totalnumDecisionParams);
            
            %  Handle the path constraints
            for phaseIdx = 1:obj.numPhases
                %  Determine the indeces for the diagonal block
                currentPhase = obj.phaseList{phaseIdx};
                %  Add one because cost function is first
                lowConIdx  = obj.conPhaseStartIdx(phaseIdx) + 1;
                highConIdx = lowConIdx + currentPhase.Config.GetNumTotalConNLP() - 1;
                lowVarIdx  = obj.decVecStartIdx(currentPhase.phaseNum);
                highVarIdx = obj.decVecEndIdx(currentPhase.phaseNum);
                %  Insert the current block in the right location
                obj.sparsityPattern(1,lowVarIdx:highVarIdx) = ...
                    obj.phaseList{phaseIdx}.GetCostSparsityPattern();
                obj.sparsityPattern(lowConIdx:highConIdx,...
                    lowVarIdx:highVarIdx) = ...
                    obj.phaseList{phaseIdx}.GetConSparsityPattern();
            end
            
            %  Handle boundary constraints/linkage constraints
            funcIdxLow = obj.totalnumConstraints - obj.numBoundFunctions + 1 + 1;
            funcIdxHigh = obj.totalnumConstraints + 1;
            funcIdxs = funcIdxLow:funcIdxHigh;
            if obj.PointFuncManager.hasBoundaryFunctions;
                obj.sparsityPattern(funcIdxs,:) = ...
                    obj.PointFuncManager.ComputeBoundNLPSparsityPattern();
            end;
            
            %  Handle cost function
            if obj.PointFuncManager.hasCostFunction;
                obj.sparsityPattern(1,:) = obj.sparsityPattern(1,:) + ...
                    obj.PointFuncManager.ComputeCostNLPSparsityPattern();
            end
            
        end
    end
end

