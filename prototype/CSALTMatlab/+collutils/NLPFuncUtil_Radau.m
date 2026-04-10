classdef NLPFuncUtil_Radau < collutils.NLPFuncUtil_Coll
    %RADAUPHASE Radau transcription class. Evaluates defects and
    %quadratures.
    %   This class computes the defect constraints, constraint, and cost
    %   function quadratures using Radau Orthogonal Collocation.
    %
    %
    % This class has a multi-step initialization.  Need to
    % disallow execution if both steps Initialize() and PrepareToOptimize()
    % have not been executed.
    %  TODO: Validate mesh interval num poitns falls between
    %  minPolynomialDegree  and maxPolynomialDegree
    
    properties (SetAccess = 'protected')
        
        % Integer.  Defines which points are used in simple quadrature.  in
        % this case, Radua, use all but last point in the mesh.
        quadratureType = 2;
        %  Real Array. Radau collocation points for the mesh interval
        radauPoints
        %  Real Array. Radau weights for the mesh interval
        quadratureWeights
        %  Sparse matrix.  Radau differentiation matrix
        radauDiffSMatrix
        %  Bool. Indicates if constraint matrices have been initialized
        isConMatInitialized = false;
        %  Bool. Indicates if cost matrices have been initialized
        isCostMatInitialized = false;
        % Real.  The minimum allowed degree of a mesh polynomial.
        minPolynomialDegree = 3;
        % Real.  The maximum allowed degree of a mesh polynomial.
        maxPolynomialDegree = 14;
        % Real. Relative error tolerance for mesh refinement
        relErrorTol = 1E-5;
        % Real array. State array for new mesh guess
        newStateArray
        % Real array. State array for new control guess
        newControlArray
        % Array of pointers to Barycentric Lagrange Interpolators
        ptrStateInterpolatorArray
        % Array of pointers to Barycentric Lagrange Interpolators
        ptrControlInterpolatorArray
        % Relative error tolerance used for "pre" mesh refinement
        initRelErrorTol = 1e-2;
        % Flag indicating if still on "pre" mesh refinement
        isInitialMesh = false;
    end
    
    methods (Access = 'public')
        
        function InitializeTranscription(obj)
            %  Computes properties of the discretization
            
            %  Compute Radau points, weights, and differentiation matrix
            clear obj.FillDynamicDefectConMatrices
            [obj.radauPoints,obj.quadratureWeights,obj.radauDiffSMatrix]...
                = lgrPS(obj.ptrConfig.GetMeshIntervalFractions(),...
                obj.ptrConfig.GetMeshIntervalNumPoints());
            
            %  Compute the number of mesh points
            obj.numMeshPoints = length(obj.quadratureWeights);
            obj.numStatePoints = obj.numMeshPoints + 1;
            obj.numControlPoints = obj.numMeshPoints;
            obj.numPathConstraintPoints = obj.numMeshPoints;
            
            MeshIntervalPoints = obj.ptrConfig.GetMeshIntervalNumPoints();
            for i = 1:length(MeshIntervalPoints)
                if MeshIntervalPoints(i)-1 > obj.maxPolynomialDegree
                    error(['Error initializing Radau transcription. Number of mesh points (',num2str(MeshIntervalPoints(i)),') - 1 is larger than allowed polynomial degree (',num2str(obj.maxPolynomialDegree),')'])
                end
            end
            
            obj.discretizationPoints = obj.radauPoints;
            obj.ptrConfig.SetNumDefectConNLP(obj.ptrConfig.GetNumStateVars()*obj.numMeshPoints);
            obj.SetStageProperties();
            
            %  Compute bookkeeping properties of the discretization
            obj.ptrConfig.SetNumStateVarsNLP(obj.ptrConfig.GetNumStateVars()*obj.numStatePoints);
            obj.ptrConfig.SetNumControlVarsNLP(...
                obj.ptrConfig.GetNumControlVars()*obj.numControlPoints);
            obj.ptrConfig.SetNumDecisionVarsNLP(...
                obj.ptrConfig.GetNumStateVarsNLP() + ...
                obj.ptrConfig.GetNumControlVarsNLP() + ...
                obj.ptrConfig.GetNumTimeVarsNLP());
            obj.timeVectorType = ones(obj.numStatePoints,1);
            obj.timeVectorType(obj.numStatePoints,1) = 2;
            
            % Initialize interpolators used for mesh refinement
            obj.InitStateAndControlInterpolator()
        end
        
        function InitNLPHelpers(obj)
            %  Initialize NLP helper utilities for cost and defect calculations
            
            % Configure the defect NLP data utility
            import collutils.NLPFunctionData
            obj.DefectNLPData = NLPFunctionData();
            numDependencies = 1;
            obj.DefectNLPData.Initialize(obj.ptrConfig.GetNumDefectConNLP(),...
                obj.ptrConfig.GetNumDecisionVarsNLP(),numDependencies)
            
            % Configure the cost NLP data utility
            obj.CostNLPData = NLPFunctionData();
            numCostFunctions = 1;
            obj.CostNLPData.Initialize(numCostFunctions,...
                obj.ptrConfig.GetNumDecisionVarsNLP(),numDependencies)
        end
        
        function SetTimeVector(obj,initialTime,finalTime)
            %  Computes the vector of discretization times
            obj.deltaTime = finalTime - initialTime;
            obj.timeVector = (finalTime - initialTime)*...
                (obj.radauPoints + 1)/2 + initialTime;
            obj.numTimePoints = obj.numMeshPoints + 1;
        end
        
        function SetStageProperties(obj)
            %  Sets stage properties for the phase type
            obj.numStateStagePointsPerMesh = 0;
            obj.numControlStagePointsPerMesh = 0;
            obj.numStagePoints = 0;
        end
        
        function meshIdx = GetMeshIndex(~,pointIdx)
            %  Get the mesh index given the point index
            meshIdx = pointIdx;
        end
        
        function stageIdx = GetStageIndex(~,~)
            %  Get the stage index given the point index
            stageIdx = 0;
        end
        
        function dtkdTI = GetdCurrentTimedTI(obj,pointIdx,~)
            % Returns partial of time at point k with respect to initial time
            dtkdTI = (1 - obj.discretizationPoints(pointIdx))/2;
        end
        
        function dtkdTF = GetdCurrentTimedTF(obj,pointIdx,~)
            % Returns partial of time at point k with respect to final time
            dtkdTF = (1 + obj.discretizationPoints(pointIdx))/2;
        end
        
        function [isMeshRefined , newMeshIntervalNumPoints, newMeshIntervalFractions, ...
                newStateArray,newControlArray, maxRelErrorInMesh] = RefineMesh(obj,DecVector,...
                PathFuncInputData,PathFunctionManager)
            
            %  Set pointers to utilities required to evaluation user
            %  functions in between mesh points
            obj.PathFuncInputData = PathFuncInputData;
            obj.PathFunctionManager = PathFunctionManager;
            
            % assume mesh is refined
            isMeshRefined = 1;
            
            newMeshIntervalNumPoints = [];
            newMeshIntervalFractions = -1;
            newStateArray = [];
            newControlArray = [];
            for meshIdx=1:obj.ptrConfig.GetNumMeshIntervals()
                                
                %  Compute error in proposed new mesh and state and control
                [maxRelErrorInMesh,newStatesInMesh,newControlsInMesh,whichError] ...
                    =GetMaxRelErrorInMesh(obj,meshIdx,DecVector);
                
                newStateArray = [newStateArray;newStatesInMesh(1:end-1,:)]; %#ok<AGROW>
                newControlArray = [newControlArray;newControlsInMesh(1:end-1,:)]; %#ok<AGROW>
                
                %  If relative error is small enough given previous mesh,
                %  dont refine this mesh interval.
                if obj.isInitialMesh
                    activeMeshTol = obj.initRelErrorTol;
                else
                    activeMeshTol = obj.relErrorTol;
                end
                if maxRelErrorInMesh <= activeMeshTol
                    newMeshIntervalNumPoints(end+1)= ...
                        obj.ptrConfig.GetNumPointsInMeshInterval(meshIdx); %#ok<AGROW>
                    newMeshIntervalFractions(end+1)=...
                        obj.ptrConfig.GetMeshIntervalFraction(meshIdx+1); %#ok<AGROW>
                    continue
                end
                
                %  Copute the new polynomial degree for the mesh interval
                polynomialDegree = obj.ptrConfig.GetNumPointsInMeshInterval(meshIdx);
                logNq = log(polynomialDegree);
                logErrorRatio = log(maxRelErrorInMesh/obj.relErrorTol);
                polynomialDegreeDiff = ceil(logErrorRatio/logNq);
                newPolynomialDegree = polynomialDegree + polynomialDegreeDiff;
                
                %  If polynomial degree is ok, accept it, otherwise divide
                %  the interval
                if newPolynomialDegree <= obj.maxPolynomialDegree
                    newMeshIntervalNumPoints(end+1) = newPolynomialDegree; %#ok<AGROW>
                    newMeshIntervalFractions(end+1) = obj.ptrConfig.GetMeshIntervalFraction(meshIdx+1); %#ok<AGROW>
                    isMeshRefined = 0;
                else
                    newPolynomialDegreeRatio = ceil(newPolynomialDegree/obj.minPolynomialDegree);
                    
                    numSubIntervals = max(newPolynomialDegreeRatio,2);
                    newMeshPoints = ...
                        linspace(obj.ptrConfig.GetMeshIntervalFraction(meshIdx), ...
                        obj.ptrConfig.GetMeshIntervalFraction(meshIdx+1),numSubIntervals+1);
                    
                    for subIntervalIdx= 1:numSubIntervals
                        newMeshIntervalNumPoints(end+1) = obj.minPolynomialDegree; %#ok<AGROW>
                        newMeshIntervalFractions(end+1) = newMeshPoints(subIntervalIdx+1); %#ok<AGROW>
                    end
                    isMeshRefined = 0;
                end
            end
        end
        
        function SetRelErrorTol(obj,tol)
            % Sets the relative error tolerance. Used during standard
            % mesh refinement
           obj.relErrorTol = tol; 
        end
        
        function SetInitRelErrorTol(obj,tol)
            % Sets the initial relative error tolerance.   Used
            % during "pre" mesh refinement.
            obj.initRelErrorTol = tol;
        end
        
        function SetIsInitialMesh(obj,flag)
            % Sets flag to indicate if in "pre" mesh refinement (true)
            % or standard mesh refinement (false)
           obj.isInitialMesh = flag; 
        end
    end
    
    methods (Access = 'protected')
       
        
        function InitializeConstantDefectMatrices(obj,dynFunProps,dynFunVector)
            %  Fills in the A, B, and D matrices for defect constraints
            
            %  If nothing to do return
            if ~obj.ptrConfig.GetHasDefectCons && ~obj.isConMatInitialized
                return
            end
            
            %  Fill the A and B matrices
            conIdx = 0;
            for intervalIdx = 1:obj.ptrConfig.GetNumMeshIntervals()
                
                %  Extract Radau differentiation matrix for current mesh
                if intervalIdx == 1
                    lowIdx = 1;
                else
                    %  Count the number of mesh points up to and including
                    %  this interval
                    lowIdx = 1;
                    for i = 1:intervalIdx-1
                        lowIdx = lowIdx + obj.ptrConfig.GetNumPointsInMeshInterval(i);
                    end
                end
                highIdx = lowIdx + obj.ptrConfig.GetNumPointsInMeshInterval(intervalIdx) - 1;
                radauDiffBlockSMatrix = obj.radauDiffSMatrix(lowIdx:highIdx,lowIdx:highIdx+1);
                
                %  Loop over number of mesh points in the interval
                funcData = dynFunVector(lowIdx:highIdx+1);
                numMeshPoints = obj.ptrConfig.GetNumPointsInMeshInterval(intervalIdx);
                for rowIdx = 1:numMeshPoints
                    %  Loop over the number of states in the problem
                    for stateIdx = 1:obj.ptrConfig.GetNumStateVars()
                        %  Row * Col multiplication and A matrix config
                        conIdx = conIdx + 1;
                        for pointIdx = 1:numMeshPoints + 1
                            states = funcData{pointIdx}.GetStateIdxs();
                            states = states(stateIdx);
                            obj.DefectNLPData.InsertAMatPartition(...
                                radauDiffBlockSMatrix(rowIdx,pointIdx),conIdx,...
                                states);
                        end
                        %  Insert the B matrix element
                        obj.DefectNLPData.InsertBMatPartition(1,conIdx,conIdx);
                    end
                end
            end
            
            %  Fill the D matrices
            conDMatRD = sparse(obj.ptrConfig.GetNumDefectConNLP(),...
                obj.ptrConfig.GetNumDecisionVarsNLP());
            funcData = dynFunVector(1:end-1);
            for funcIdx = 1:length(funcData)
                
                %  Increment counters
                conStartIdx = (funcData{funcIdx}.GetMeshIdx()-1)*...
                    obj.ptrConfig.GetNumStateVars() + 1;
                conEndIdx = conStartIdx + obj.ptrConfig.GetNumStateVars() - 1;
                defConIdxs = conStartIdx:conEndIdx;
                %  Fill in state Jacobian elements
                if obj.ptrConfig.GetHasStateVars()
                    conDMatRD(defConIdxs,funcData{funcIdx}.GetStateIdxs()) = ...
                        dynFunProps.GetStateJacPattern();
                end
                %  Fill in control Jacobian terms
                if obj.ptrConfig.GetHasControlVars()
                    conDMatRD(defConIdxs,funcData{funcIdx}.GetControlIdxs()) = ...
                        dynFunProps.GetControlJacPattern();
                end
                %  Initial time terms
                conDMatRD(defConIdxs,1) = ones(obj.ptrConfig.GetNumStateVars(),1);
                %  Final time terms
                conDMatRD(defConIdxs,2) = ones(obj.ptrConfig.GetNumStateVars(),1);
                
            end
            obj.DefectNLPData.SetDMatrix(conDMatRD)
            
            %  Set flag indiciating the matrices are initialized
            obj.isConMatInitialized = true;
        end
        
        function [stateIdxs,controlIdxs] = GetStateAndControlIdxsInMesh(obj,meshIdx)
            %  Get state/control idxs for all points in given mesh interval
            %  Used in mesh refinement
            
            %  Validate input
            if meshIdx > obj.ptrConfig.GetNumMeshIntervals()
                error('invalid input')
            elseif meshIdx <=0
                error('invalid input')
            end
            %  Count points in mesh intervals previous to meshIdx
            pointSum = 0;
            for idx = 1:meshIdx - 1
                pointSum = pointSum + obj.ptrConfig.GetNumPointsInMeshInterval(idx);
            end
            %  Create the vector for the requested mesh interval
            stateIdxs = pointSum+1:1:pointSum+obj.ptrConfig.GetNumPointsInMeshInterval(meshIdx)+1;
            controlIdxs = pointSum+1:1:pointSum+obj.ptrConfig.GetNumPointsInMeshInterval(meshIdx);
        end
        
        function [stateMat,controlMat] = GetStateAndControlInMesh(obj,meshIdx,DecVector)
            %  Gets state and control at all points in given mesh interval
            %  Used in mesh refinement
            
            %  Initialize matrices
            [stateIdxs,controlIdxs] = obj.GetStateAndControlIdxsInMesh(meshIdx);
            stateMat = zeros(obj.ptrConfig.GetNumStateVars(),length(stateIdxs));
            controlMat = zeros(obj.ptrConfig.GetNumControlVars(),length(controlIdxs));
            for pointIdx = 1:length(stateIdxs)
                stateMat(:,pointIdx) = DecVector.GetStateVector(...
                    stateIdxs(pointIdx),0);
            end
            for pointIdx = 1:length(controlIdxs)
                controlMat(:,pointIdx) = DecVector.GetControlVector(...
                    controlIdxs(pointIdx),0);
            end
        end
        
        function InitStateAndControlInterpolator(obj)
            % in the C++ implementation, stateInterpolatorArray will be
            % a pointer array of BarycentricLagrangeInterpolator class
            %  Used in mesh refinement
            obj.ptrStateInterpolatorArray=[];
            obj.ptrControlInterpolatorArray=[];
            
            for numNodes = obj.minPolynomialDegree:obj.maxPolynomialDegree + 1
                
                normalLGRControlPoints = lgrnodes(numNodes - 1);
                normalLGRStatePoints = [normalLGRControlPoints;
                    1]; % LGR points on [-1,1]
                
                ptr1 = BarycentricLagrangeInterpolator(normalLGRStatePoints);
                obj.ptrStateInterpolatorArray=[obj.ptrStateInterpolatorArray ptr1];
                
                ptr2 = BarycentricLagrangeInterpolator(normalLGRControlPoints);
                obj.ptrControlInterpolatorArray=[obj.ptrControlInterpolatorArray ptr2];
                
            end
        end
        
        function [newNormalRadauPoints,interpolatedStateInNewMesh,interpolatedControlInNewMesh]=...
                GetNewCollocationInMesh(obj,meshIntervalIdx,DecVector)
            % increase polynomial degree by one in the given mesh interval
            %  Used in mesh refinement
            %
            [stateInMesh, controlInMesh] = ...
                obj.GetStateAndControlInMesh(meshIntervalIdx,DecVector);
            %% maybe temporary.
            stateInMesh=stateInMesh.';
            controlInMesh=controlInMesh.';
            %%
            numPointsInMesh = obj.ptrConfig.GetNumPointsInMeshInterval(meshIntervalIdx);
            % numLGRControlPoints == meshIntervalNumPoints
            
            polynomialDegree = numPointsInMesh;
            interpolatorIdx = polynomialDegree - obj.minPolynomialDegree + 1;
            newNormalRadauPoints = lgrnodes(numPointsInMesh);
            % increase polynomial order by one
            
            interpolatedStateInNewMesh = ...
                [stateInMesh(1,:);
                obj.ptrStateInterpolatorArray(interpolatorIdx). ...
                Interpolate(newNormalRadauPoints(2:end),stateInMesh);
                stateInMesh(end,:);];
            % at tau= -1 & 1, interpolation is not applied
            
            interpolatedControlInNewMesh= ...
                [controlInMesh(1,:);
                obj.ptrControlInterpolatorArray(interpolatorIdx). ...
                Interpolate(newNormalRadauPoints(2:end),controlInMesh)];
            % at tau = -1, interpolation is not applied
        end
        
        function [maxRelErrorInMesh,interpolatedStateInNewMesh,interpolatedControlInNewMesh,whichError] ...
                =GetMaxRelErrorInMesh(obj,meshIntervalIdx,DecVector)
            %  Used in mesh refinement
            
            [newNormalRadauPoints, interpolatedStateInNewMesh, interpolatedControlInNewMesh]=...
                GetNewCollocationInMesh(obj,meshIntervalIdx,DecVector);
            numNewRadauPoints = numel(newNormalRadauPoints);
            
            meshIntervalLength = (obj.ptrConfig.GetMeshIntervalFraction(meshIntervalIdx+1) - ...
                obj.ptrConfig.GetMeshIntervalFraction(meshIntervalIdx));
            meshIntervalCenter = (obj.ptrConfig.GetMeshIntervalFraction(meshIntervalIdx+1) + ...
                obj.ptrConfig.GetMeshIntervalFraction(meshIntervalIdx))/2;
            
            newRadauPoints = meshIntervalLength/2*newNormalRadauPoints + ...
                meshIntervalCenter;
            newTimeVector = (obj.timeVector(end) - obj.timeVector(1))*...
                    (newRadauPoints + 1)/2 + obj.timeVector(1);
            newDifferentiationSMatrix = ...
                collocD([newRadauPoints;obj.ptrConfig.GetMeshIntervalFraction(meshIntervalIdx+1)]);
            % now the output of collocD is not sparse matrix but it needs to be in future.
            
            newIntegrationSMatrix = newDifferentiationSMatrix(1:end-1,2:end)^-1;
            % now newIntegrationSMatrix is not sparse matrix but it needs to be in future.
            newDynFuncArray=zeros(numNewRadauPoints, obj.ptrConfig.GetNumStateVars());
            for i=1:numNewRadauPoints
                dynFuncData=obj.GetUserDynFuncData(newTimeVector(i),...
                    interpolatedStateInNewMesh(i,:).', interpolatedControlInNewMesh(i,:).');
                newDynFuncArray(i,:)=dynFuncData.GetFunctionValues();
            end
            phaseLength=(obj.deltaTime);
            
            newStateInNewMesh = [interpolatedStateInNewMesh(1,:);
                ones(numNewRadauPoints,1)*interpolatedStateInNewMesh(1,:) ...
                + phaseLength/2*newIntegrationSMatrix*newDynFuncArray];
            % need to track sparse matrices hereafter.
            
            errorMatrix = abs(newStateInNewMesh - interpolatedStateInNewMesh);
            relErrorMatrix=zeros(numNewRadauPoints+1,obj.ptrConfig.GetNumStateVars());
            
            for i=1:obj.ptrConfig.GetNumStateVars()
                maxAbsState=max(abs(interpolatedStateInNewMesh(:,i)));
                relErrorMatrix(:,i)=errorMatrix(:,i)./(1+maxAbsState);
            end
            [maxRelErrorInMesh,whichError] = max(max(relErrorMatrix));
            
            disp(['Interval ',num2str(meshIntervalIdx),': MaxError/ErrorTol = ',num2str(maxRelErrorInMesh/obj.relErrorTol),' in State ',num2str(whichError)])
        end
        
        function FillDynamicCostFuncMatrices(obj,funcData)
            
            %  Loop initializations
            dtBy2 = 0.5*obj.deltaTime;
            conqMatRD = sparse(obj.numMeshPoints,1);
            conparQMatRD = sparse(obj.numMeshPoints,obj.ptrConfig.GetNumDecisionVarsNLP());
            
            %  Loop over the function data and concatenate Q and parQ
            %  matrix terms
            for funcIdx = 1:length(funcData)-1
                
                %  Increment counters
                meshIdx = funcData{funcIdx}.GetMeshIdx();
                
                %  Fill in the function data
                conqMatRD(funcIdx,1) = ...
                    -dtBy2*funcData{funcIdx}.GetFunctionValues(); %#ok<*SPRIX>
                
                %  Fill in state Jacobian elements
                if obj.ptrConfig.GetHasStateVars()
                    conparQMatRD(funcIdx,funcData{funcIdx}.GetStateIdxs()) = ...
                        -dtBy2*funcData{funcIdx}.GetStateJac();
                end
                
                %  Fill in control Jacobian terms
                if obj.ptrConfig.GetHasControlVars()
                    conparQMatRD(funcIdx,funcData{funcIdx}.GetControlIdxs()) = ...
                        -dtBy2*funcData{funcIdx}.GetControlJac();
                end
                
                %  Initial time terms
                conparQMatRD(funcIdx,1) = ...
                    0.5*funcData{funcIdx}.GetFunctionValues() ...
                    -dtBy2*obj.GetdCurrentTimedTI(meshIdx)*...
                    funcData{funcIdx}.GetTimeJac();
                
                %  Final time terms
                conparQMatRD(funcIdx,2) = ...
                    -0.5*funcData{funcIdx}.GetFunctionValues()...
                    -dtBy2*obj.GetdCurrentTimedTF(meshIdx)*...
                    funcData{funcIdx}.GetTimeJac();
                
            end
            %  If we do what is suggested in comments above in GMAT, then
            %  this call is not needed.
            obj.CostNLPData.SetQVector(conqMatRD)
            obj.CostNLPData.SetparQMatrix(conparQMatRD)
        end
        
        %  Fill in the Q and parQ matrices for provided function data
        function FillDynamicDefectConMatrices(obj,funcData)
            
            persistent parAccumulator funAccumulator
            
%             if isempty(parAccumulator) || ~obj.isFinalized
%                 parAccumulator = collutils.SparseMatAccumulator;
%             end
            
            %  Loop initializations
            dtBy2 = 0.5*(obj.deltaTime);
            conqMatRD = sparse(obj.ptrConfig.GetNumDefectConNLP(),1);
            parAccumulator = collutils.SparseMatAccumulator;
            funAccumulator = collutils.SparseMatAccumulator;
            parAccumulator.BeginAccumulation();
            funAccumulator.BeginAccumulation();
            %  Loop over the function data and concatenate Q and parQ matrix terms
            for funcIdx = 1:length(funcData)
                
                % Increment counters
                conStartIdx = (funcData{funcIdx}.GetMeshIdx()-1)*obj.ptrConfig.GetNumStateVars()+1;
                conEndIdx = conStartIdx + obj.ptrConfig.GetNumStateVars() - 1;
                defConIdxs = conStartIdx:conEndIdx;
                meshIdx = funcData{funcIdx}.GetMeshIdx();
                rowStart = defConIdxs(1);
                
                % Fill in the function data
                conqMatRD(defConIdxs,1) = -dtBy2*funcData{funcIdx}.GetFunctionValues();
                defectCon = -dtBy2*funcData{funcIdx}.GetFunctionValues();
                funAccumulator.Accumulate(rowStart,1,defectCon,1)
                
                % Fill in state Jacobian elements
                if obj.ptrConfig.GetHasStateVars()
                    denseMat = -dtBy2*funcData{funcIdx}.GetStateJac();
                    colIdxs = funcData{funcIdx}.GetStateIdxs();
                    parAccumulator.Accumulate(...
                        rowStart,colIdxs(1,1),denseMat,obj.dynFunProps.GetStateJacPattern());
                end
                
                % Fill in control Jacobian terms
                if obj.ptrConfig.GetHasControlVars()
                    denseMat = -dtBy2*funcData{funcIdx}.GetControlJac();
                    colIdxs = funcData{funcIdx}.GetControlIdxs();
                    parAccumulator.Accumulate(...
                        rowStart,colIdxs(1,1),denseMat,obj.dynFunProps.GetControlJacPattern());
                end
                
                % Initial time terms
                timeSparsity = ones(obj.ptrConfig.GetNumStateVars,1);
                denseMat = 0.5*funcData{funcIdx}.GetFunctionValues() ...
                    -dtBy2*obj.GetdCurrentTimedTI(meshIdx)*funcData{funcIdx}.GetTimeJac();
                parAccumulator.Accumulate(rowStart,1,denseMat,timeSparsity);
                
                % Final time terms
                denseMat = -0.5*funcData{funcIdx}.GetFunctionValues()...
                    -dtBy2*obj.GetdCurrentTimedTF(meshIdx)*funcData{funcIdx}.GetTimeJac();
                parAccumulator.Accumulate(rowStart,2,denseMat,timeSparsity);

            end
            parAccumulator.EndAccumulation();
            funAccumulator.EndAccumulation();
            
            % Set the vectors on the helper util
            [rowVector,colVector,valVector] = parAccumulator.Get3Vectors();
            tMat = sparse(rowVector,colVector,valVector,obj.ptrConfig.GetNumDefectConNLP(),...
                obj.ptrConfig.GetNumDecisionVarsNLP());
            obj.DefectNLPData.SetparQMatrix(tMat)
            obj.DefectNLPData.SetQVector(conqMatRD)
            
        end
        
    end
    
end
