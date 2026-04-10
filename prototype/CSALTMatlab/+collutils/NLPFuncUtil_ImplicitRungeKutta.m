classdef NLPFuncUtil_ImplicitRungeKutta < collutils.NLPFuncUtil_Coll
    % ImplicitRungeKuttaPhase transcription class.
    %
    % This class computes the defect constraints, integralconstraint, and cost
    % function quadratures using implicite Runge Kutta methods of the "Separated" form.
    %
    % Ref [1].  Betts, John, T.  "Practical Methods for Optimal Control and
    % Estimation Using Nonlinear Programming", 2nd Edition.
    %
    % Ref [2].  Betts, John, T.  "Using Direct Transcription to Compute Optimal Low Thrust
    % Transfers Between Libration Point Orbits", Posted on his website.
    %
    % The comments below define mesh/step points, stage points, and
    % how bookeeping is performed for IRK methods.
    % An example with a 6 order IRK with 3 steps looks like this:
    % | = mesh/step of the integrator
    % x = internal state of integrator
    % Point Type  |  x  x  |  x  x  |  x  x  |
    % MeshIdx     1  1  1  2  2  2  3  3  3  4
    % StageIdx    0  1  2  0  1  1  0  1  2  0
    % PointIdx    1  2  3  4  5  6  7  8  9  10
    % Note:  3 steps means 4 mesh points: numSteps = numMeshPoints - 1
    % Total num points = numSteps*(numStages + 1) + 1;  There are two
    % stages in this example.
    
    properties (SetAccess = 'protected')
        % Integer.  Defines which points are used in simple quadrature.  in
        % this case, Lobatto, use all points in the mesh.
        quadratureType = 1;
        %  Real Array. LIIA weights for the mesh interval
        quadratureWeights
        %  Real Array: number of mesh steps for each mesh interval
        numMeshSteps % Not needed?
        % Int. Number of points per mesh
        numPointsPerMesh
        %  IRK data utility.
        ButcherTableData
        % Int.  Number of stages in the IRK method
        numStages
        % Int. Number of steps in the phase
        numStepsInPhase
        % Bool. Indicates constraint matrices A and B are initialized
        isConMatInitialized
        % The number of ode RHS terms.  numStates*numPoints
        numODERHS
        % Real array.  Vector of non-dimensional step sizes
        stepSizeVec
    end
    
    methods
        
        %  Compute Radau points, weights, and differentiation matrix
        function obj = InitializeTranscription(obj,collocationMethod)
            
            % Intialize the Butcher table helper class used throughout
            obj.SetButcherTable(collocationMethod);
            
            %  Set stage properties, step size vector and bookeeping stuff
            obj.SetStageProperties();
            obj.ComputeStepSizeVector();
            obj.discretizationPoints(obj.numMeshPoints) = 1;
            obj.numStatePoints = obj.numStepsInPhase + 1;
            obj.numControlPoints = obj.numStepsInPhase + 1;
            obj.numPathConstraintPoints = obj.numMeshPoints;
            obj.numODERHS = length(obj.discretizationPoints)*obj.ptrConfig.GetNumStateVars();
            obj.timeVectorType = ones(obj.numMeshPoints,1);
            obj.SetQuadratureWeights();
            
            % Compute number of various types of parameters
            numTimesNLP = 2;
            numStatesNLP = obj.ptrConfig.GetNumStateVars()*(obj.numMeshPoints);
            numControlsNLP = obj.ptrConfig.GetNumControlVars*(obj.numMeshPoints);
            numDecisionVarsNLP = numStatesNLP + numControlsNLP + numTimesNLP;
            numDefectConNLP = obj.ptrConfig.GetNumStateVars()*(obj.numMeshPoints - 1);
            
            % Set data on the configuration object
            obj.ptrConfig.SetNumStateVarsNLP(numStatesNLP);
            obj.ptrConfig.SetNumControlVarsNLP(numControlsNLP);
            obj.ptrConfig.SetNumDecisionVarsNLP(numDecisionVarsNLP);
            obj.ptrConfig.SetNumDefectConNLP(numDefectConNLP);
            
        end
        
        function SetQuadratureWeights(obj)
            obj.ptrConfig.GetMeshIntervalNumPoints()
            numPoints = obj.ptrConfig.GetMeshIntervalNumPoints();
            if length(numPoints) > 1
                error('Must use fixed step for IRK methods, MeshIntervalNumPoints must be length 1');
            end
            stepWeights = obj.ButcherTableData.GetQuadratureWeights();
            numStepWeights = length(stepWeights);
            numMeshWeights = numPoints*(numStepWeights-1)+1;
            obj.quadratureWeights = zeros(numMeshWeights,1);
            count = 1;
            for pointIdx = 1:numMeshWeights-1
                obj.quadratureWeights(pointIdx) = stepWeights(count)
                count = count + 1;
                if count == numStepWeights
                    count = 1;
                end
            end
            obj.quadratureWeights(numMeshWeights) = ...
                stepWeights(numStepWeights);
        end
        
        function InitNLPHelpers(obj)
            %  Initialize NLP helper utilities for cost and defect calculations
            
            % Configure the defect NLP data utility
            import collutils.NLPFunctionData
            obj.DefectNLPData = NLPFunctionData();
            obj.DefectNLPData.Initialize(obj.ptrConfig.GetNumDefectConNLP(),...
                obj.ptrConfig.GetNumDecisionVarsNLP(),obj.numODERHS)
            
            % Configure the cost NLP data utility
            obj.CostNLPData = NLPFunctionData();
            numCostFunctions = 1;
            obj.CostNLPData.Initialize(numCostFunctions,...
                obj.ptrConfig.GetNumDecisionVarsNLP(),obj.numODERHS)
        end
        
        function SetTimeVector(obj,initialTime,finalTime)
            %  Computes the vector of discretization times
            obj.deltaTime = finalTime - initialTime;
            obj.timeVector = obj.discretizationPoints*obj.deltaTime + initialTime;
            obj.numTimePoints = obj.numMeshPoints;
        end
        
        function meshIdx = GetMeshIndex(obj,pointIdx)
            %  Computes mesh index given a time point index
            idxByNumPointsPlusOne = pointIdx/(obj.numPointsPerMesh+1);
            if idxByNumPointsPlusOne < 1
                meshIdx = 1;
                return
            end
            quotient = pointIdx/(obj.numPointsPerMesh);
            if rem(pointIdx,(obj.numPointsPerMesh)) == 0 % At end point in the mesh
                meshIdx = quotient;
            else
                meshIdx = 1 + floor(quotient); % An internal (stage point)
            end
        end
        
        function stageIdx = GetStageIndex(obj,pointIdx)
            %  Compute the stage point given a time point index
            if pointIdx <= obj.numPointsPerMesh
                stageIdx = pointIdx - 1;
                return
            end
            quotient = floor(pointIdx/(obj.numPointsPerMesh));
            if rem(pointIdx,(obj.numPointsPerMesh)) == 0
                stageIdx = obj.numStagePoints;
            else
                stageIdx = pointIdx - quotient*obj.numPointsPerMesh - 1;
            end
        end
        
        function dtkdTI = GetdCurrentTimedTI(obj,meshIdx,stageIdx)
            %  Returns partial of time at point k with respect to initial time
            nonDimTime = obj.MeshPointIdxToNonDimTime(meshIdx,stageIdx);
            dtkdTI = 1 - nonDimTime;
        end
        
        function dtkdTF = GetdCurrentTimedTF(obj,meshIdx,stageIdx)
            %  Returns partial of time at point k with respect to initial time
            nonDimTime = obj.MeshPointIdxToNonDimTime(meshIdx,stageIdx);
            dtkdTF = nonDimTime;
        end
        
    end
    
    methods (Access = protected)
        
        %  Compute the scaled time value at a given mesh point index
        function nonDimTime = MeshPointIdxToNonDimTime(obj,meshIdx,stageIdx)
            pointIdx = GetPointIdxGivenMeshAndStageIdx(obj,meshIdx,stageIdx);
            nonDimTime =  obj.discretizationPoints(pointIdx);
        end
        
        function pointIdx = GetPointIdxGivenMeshAndStageIdx(obj,meshIdx,stageIdx)
            pointIdx = (obj.numStages)*(meshIdx-1) + stageIdx + 1;
        end
        
        function SetStageProperties(obj)
            %  Sets data on state and control at stage points
            obj.numStateStagePointsPerMesh   = ...
                obj.ButcherTableData.GetNumStateStagePointsPerMesh();
            obj.numControlStagePointsPerMesh = ...
                obj.ButcherTableData.GetNumControlStagePointsPerMesh();
            obj.numStagePoints = obj.ButcherTableData.GetNumStagePointsPerMesh();
            obj.numPointsPerMesh = 1 + obj.numStagePoints;
        end
        
        function ComputeStepSizeVector(obj)
            % Computes the step size vector.
            
            % Note this is generalized for later support of non-constant
            % step sizes.
            meshFractions = obj.ptrConfig.GetMeshIntervalFractions();
            meshFractions = obj.NormalizeMeshFractions(meshFractions);
            obj.ptrConfig.SetMeshIntervalFractions(meshFractions);
            meshNumPoints = obj.ptrConfig.GetMeshIntervalNumPoints();
            numMeshIntervals = length(meshNumPoints);
            stageTimes = obj.ButcherTableData.GetStageTimes();
            obj.numStages = length(stageTimes) - 1;
            obj.numStepsInPhase = sum(meshNumPoints) - length(meshNumPoints);
            obj.numMeshPoints = obj.numStepsInPhase*obj.numPointsPerMesh + 1;
            obj.discretizationPoints = zeros(1,obj.numMeshPoints);
            pointIdx = 1;
            count = 0;
            obj.stepSizeVec = zeros(obj.numStepsInPhase,1);
            for intervalIdx = 1:numMeshIntervals
                meshIntervalDuration = meshFractions(intervalIdx+1) - ...
                    meshFractions(intervalIdx);
                stepSize = meshIntervalDuration/(meshNumPoints(intervalIdx) - 1);
                for stepIdx = 1:meshNumPoints(intervalIdx) - 1
                    if stepIdx < meshNumPoints(intervalIdx)
                        count = count + 1;
                        obj.stepSizeVec(count) = stepSize;
                    end
                    for stageIdx = 1:obj.numStages;
                        pointIdx = pointIdx + 1;
                        obj.discretizationPoints(1,pointIdx) = ...
                            obj.discretizationPoints(1,pointIdx-1) + ...
                            stepSize*(stageTimes(stageIdx+1) - stageTimes(stageIdx));
                    end
                end
            end
            obj.CheckForConstantStepSize();
        end
        
        function newNeshIntervalFractions = NormalizeMeshFractions(~,meshIntervalFractions)
            % Normalizes mesh interval fractions to fall in [0 1]
            numPoints = length(meshIntervalFractions);
            meshSpan = meshIntervalFractions(numPoints) - meshIntervalFractions(1);
            newNeshIntervalFractions = zeros(1,numPoints);
            for meshIdx = 2:length(meshIntervalFractions)
                newNeshIntervalFractions(meshIdx) = (meshIntervalFractions(meshIdx) - ...
                    meshIntervalFractions(meshIdx-1))/meshSpan;
            end
            newNeshIntervalFractions(numPoints) = 1;
        end
        
        function CheckForConstantStepSize(obj)
            % Check that user configured constant step size
            for stepIdx = 1:obj.numStepsInPhase -1
                if (obj.stepSizeVec(stepIdx) - obj.stepSizeVec(stepIdx)) > 10*eps
                    error('Implicit Runge Kutta currently requires a constant step size')
                end
            end
        end
        
        function SetButcherTable(obj,collocationMethod)
            % Initializes Butcher table helper class for chosen method
            if strcmp(collocationMethod,'RungeKutta8')
                obj.ButcherTableData = collutils.LobattoIIIA_8Order;
            end
            if strcmp(collocationMethod,'RungeKutta6')
                obj.ButcherTableData = collutils.LobattoIIIA_6Order;
            end
            if strcmp(collocationMethod,'RungeKutta4')
                obj.ButcherTableData = collutils.LobattoIIIA_4Order;
            end
            if strcmp(collocationMethod,'HermiteSimpson')
                obj.ButcherTableData = collutils.LobattoIIIA_4HSOrder;
            end
            if strcmp(collocationMethod,'Trapezoid')
                obj.ButcherTableData = collutils.LobattoIIIA_2Order;
            end
        end
        
        function  InitStateAndControlInterpolator(~)
            
        end
        
        function InitializeConstantDefectMatrices(obj,dynFunProps,dynFunVector)
            
            %  If nothing to do return
            if ~obj.ptrConfig.GetHasDefectCons && ~obj.isConMatInitialized
                return
            end
            
            %% Populate the A and B matrices
            numStateVars = obj.ptrConfig.GetNumStateVars();
            defectStartIdx = 1;
            defectEndIdx = numStateVars;
            % Loop over the steps in the phase
            for stepIdx = 1:obj.numStepsInPhase
                %  Loop over defect constraints for current step
                for defectIdx = 1:obj.numStages
                    conIdxs = defectStartIdx:defectEndIdx;
                    % Loop over points used in current step
                    for subStepIdx = 1:obj.numStages + 1
                        if subStepIdx <= obj.numStages
                            pointIdx = obj.GetPointIdxGivenMeshAndStageIdx(stepIdx,subStepIdx-1);
                        else
                            pointIdx = obj.GetPointIdxGivenMeshAndStageIdx(stepIdx+1,0);
                        end
                        [aChunk,bChunk] = obj.ButcherTableData.GetDependencyChunk(...
                            defectIdx,subStepIdx,numStateVars);
                        stateIdxs = dynFunVector{pointIdx}.GetStateIdxs();
                        obj.DefectNLPData.InsertAMatPartition(aChunk,conIdxs,stateIdxs);
                        odeStartIdx = (pointIdx-1)*numStateVars + 1;
                        odeEndIdx = odeStartIdx + numStateVars - 1;
                        odeIdxs = odeStartIdx:odeEndIdx;
                        obj.DefectNLPData.InsertBMatPartition(-bChunk,conIdxs,odeIdxs);
                    end
                    defectStartIdx = defectStartIdx + numStateVars;
                    defectEndIdx = defectEndIdx + numStateVars;
                end
            end
            
            %  Fill the D matrices
            conDMatRD = sparse( obj.numODERHS,obj.ptrConfig.GetNumDecisionVarsNLP());
            funcData = dynFunVector;
            for funcIdx = 1:length(funcData)
                
                %  Increment counters
                funIdxStart = (funcIdx - 1)*obj.ptrConfig.GetNumStateVars() +  1;
                funIdxEnd = funIdxStart + obj.ptrConfig.GetNumStateVars() - 1;
                funIdxs = funIdxStart:funIdxEnd;
                %  Fill in state Jacobian elements
                if obj.ptrConfig.GetHasStateVars()
                    conDMatRD(funIdxs,funcData{funcIdx}.GetStateIdxs()) = ...
                        dynFunProps.GetStateJacPattern(); %#ok<SPRIX>
                end
                %  Fill in control Jacobian terms
                if obj.ptrConfig.GetHasControlVars()
                    conDMatRD(funIdxs,funcData{funcIdx}.GetControlIdxs()) = ...
                        dynFunProps.GetControlJacPattern(); %#ok<SPRIX>
                end
                %  Initial time terms
                conDMatRD(funIdxs,1) = ones(obj.ptrConfig.GetNumStateVars(),1); %#ok<SPRIX>
                %  Final time terms
                conDMatRD(funIdxs,2) = ones(obj.ptrConfig.GetNumStateVars(),1); %#ok<SPRIX>
                
            end
            obj.DefectNLPData.SetDMatrix(conDMatRD)
            
            %  Set flag indiciating the matrices are initialized
            obj.isConMatInitialized = true;
        end
        
        function FillDynamicDefectConMatrices(obj,funcData)
            
            %  Loop initializations
            parAccumulator = collutils.SparseMatAccumulator;
            funAccumulator = collutils.SparseMatAccumulator;
            parAccumulator.BeginAccumulation();
            funAccumulator.BeginAccumulation();
            
            %  Loop over the function data and concatenate Q and parQ matrix terms
            for funcIdx = 1:length(funcData)
                
                % Compute dimensionalized time step multiplier for this
                % function evaluation
                nonDimeTimeStep = obj.stepSizeVec(1);
                timeStep = obj.deltaTime*nonDimeTimeStep;%obj.stepSizeVec(meshIdx);
                % TODO:  Only works for fixed time step problems
                
                % Increment counters
                conStartIdx = (funcIdx-1)*obj.ptrConfig.GetNumStateVars() + 1;
                conEndIdx = conStartIdx + obj.ptrConfig.GetNumStateVars() - 1;
                defConIdxs = conStartIdx:conEndIdx;
                meshIdx = funcData{funcIdx}.GetMeshIdx();
                stageIdx = funcData{funcIdx}.GetStageIdx();
                rowStart = defConIdxs(1);
                
                % Fill in the function data
                conqMatRD(defConIdxs,1) = -timeStep*funcData{funcIdx}.GetFunctionValues(); %#ok<AGROW>
                
                % Fill in state Jacobian elements
                if obj.ptrConfig.GetHasStateVars()
                    denseMat = -timeStep*funcData{funcIdx}.GetStateJac();
                    colIdxs = funcData{funcIdx}.GetStateIdxs();
                    parAccumulator.Accumulate(...
                        rowStart,colIdxs(1,1),denseMat,obj.dynFunProps.GetStateJacPattern());
                end
                
                % Fill in control Jacobian terms
                if obj.ptrConfig.GetHasControlVars()
                    denseMat = -timeStep*funcData{funcIdx}.GetControlJac();
                    colIdxs = funcData{funcIdx}.GetControlIdxs();
                    parAccumulator.Accumulate(...
                        rowStart,colIdxs(1,1),denseMat,obj.dynFunProps.GetControlJacPattern());
                end
                
                % Initial time terms
                timeSparsity = ones(obj.ptrConfig.GetNumStateVars,1);
                denseMat = nonDimeTimeStep*funcData{funcIdx}.GetFunctionValues() ...
                    -timeStep*obj.GetdCurrentTimedTI(meshIdx,stageIdx)*funcData{funcIdx}.GetTimeJac();
                parAccumulator.Accumulate(rowStart,1,denseMat,timeSparsity);
                
                % Final time terms
                denseMat = -nonDimeTimeStep*funcData{funcIdx}.GetFunctionValues()...
                    -timeStep*obj.GetdCurrentTimedTF(meshIdx,stageIdx)*funcData{funcIdx}.GetTimeJac();
                parAccumulator.Accumulate(rowStart,2,denseMat,timeSparsity);
                
            end
            parAccumulator.EndAccumulation();
            
            % Set the vectors on the helper util
            [rowVector,colVector,valVector] = parAccumulator.Get3Vectors();
            tMat = sparse(rowVector,colVector,valVector,obj.numODERHS,...
                obj.ptrConfig.GetNumDecisionVarsNLP());
            obj.DefectNLPData.SetparQMatrix(tMat)
            obj.DefectNLPData.SetQVector(conqMatRD)
        end
        
        function FillDynamicCostFuncMatrices(obj,funcData)
            %  Loop initializations
            dtBy2 = 0.5*obj.deltaTime;
            conqMatRD = sparse(obj.numMeshPoints,1);
            conparQMatRD = sparse(obj.numMeshPoints,obj.ptrConfig.GetNumDecisionVarsNLP());
            
            %  Loop over the function data and concatenate Q and parQ
            %  matrix terms
            for funcIdx = 1:length(funcData)
                
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
                %  TODO: Fix the time partials
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
        
    end
end

%         function [timeStep] = MeshPointIdxToTimeStep(obj,pointIdx)
%             %  Compute the time step for a given mesh index.
%
%             %  May not be needed but in case the user forgot to call first
%             obj.ComputeTimeParameters();
%
%             %Note the time step is different in different mesh intervals,
%             % but constant withing a given interval.
%             for intervalIdx = 1:obj.numMeshIntervals
%                 if pointIdx <= sum(obj.numMeshSteps(1:intervalIdx))
%                     break
%                 end
%             end
%             timeStep = (obj.finalTime - obj.initialTime)*...
%                 obj.meshStepSizes(intervalIdx);
%         end
%
%         %  Compute the indexes into the function array given mesh idx
%         function funIdxs = MeshPointIdxToFunIdx(obj,meshIdx)
%             lowIdx = (1 + obj.numStateStagePointsPerMesh)*(meshIdx - 1) + 1;
%             highIdx = lowIdx + obj.numStateStagePointsPerMesh + 1;
%             funIdxs = lowIdx:highIdx;
%         end
%         function [quadIdxs,interpIdxs] = MeshPointIdxToDefectIdxs(obj,pointIdx)
%             %  Compute the defect constraint indeces for given mesh index
%             lowerIdx = 2*(pointIdx -1)*obj.numStateVars + 1;
%             upperIdx = lowerIdx + obj.numStateVars - 1;
%             interpIdxs  = lowerIdx:upperIdx;
%             quadIdxs = interpIdxs + obj.numStateVars;
%         end
%
%
% %% This block of methods managed computation of functions and partials
% %  depending upon the user configuration.  Only the functions and
% %  partials used in the problem are computed.
%
% function QuadSum(obj,funData,funIdxs,dt,meshPointIdx)
% % Sum the current quadrature point
%
% if obj.ptrPhase.hasCostFunction
%     obj.QuadFunction(funData,funIdxs,dt)
% else
%     return
% end
%
% % Compute state partials and insert them in Jacobian
% funType = 1;  %  Hard coded for cost, ->generalize for integral functions
% if obj.ptrPhase.hasStateVars
%     obj.ComputeQuadFunStateJac(funData,funIdxs,dt,funType)
% end
%
% %  Compute control partials and insert them in Jacobian
% if obj.ptrPhase.hasControlVars
%     obj.ComputeQuadFunControlJac(funData,funIdxs,dt,funType)
% end
%
% obj.ComputeQuadFunFinalTimeJac(funData,funIdxs,dt,meshPointIdx,funType)
% obj.ComputeQuadFunInitTimeJac(funData,funIdxs,dt,meshPointIdx,funType)
%
% end
%
% function InterpDefectCon(obj,funData,funIdxs,dt,meshPointIdx)
% % Compute the Hermite Interpolant defect con/partials
%
% % Compute the constraint values
% if obj.ptrPhase.hasDefectCons
%     q = obj.ComputeInterpConstraint(funData,funIdxs,dt);
% else
%     return
% end
%
% % Compute state partials and insert them in Jacobian
% if obj.ptrPhase.hasStateVars
%     obj.ComputeInterpStateJac(funData,funIdxs,dt,q);
% end
%
% %  Compute control partials and insert them in Jacobian
% if obj.ptrPhase.hasControlVars
%     obj.ComputeInterpControlJac(funData,funIdxs,dt,q);
% end
%
% %  Compute time partials
% obj.ComputeInterpInitTimeJac(funData,funIdxs,dt,meshPointIdx,q)
% obj.ComputeInterpFinalTimeJac(funData,funIdxs,dt,meshPointIdx,q)
%
% end
%
% function QuadDefectCon(obj,funData,funIdxs,dt,meshPointIdx)
% % Compute the Simpson quadrature defect constraint/partials
%
% if obj.ptrPhase.hasDefectCons
%     q = obj.ComputeQuadConstraint(funData,funIdxs,dt);
% else
%     return
% end
%
% % Compute state partials and insert them in Jacobian
% if obj.ptrPhase.hasStateVars
%     obj.ComputeQuadStateJac(funData,funIdxs,dt,q);
% end
%
% %  Compute control partials and insert them in Jacobian
% if obj.ptrPhase.hasControlVars
%     obj.ComputeQuadControlJac(funData,funIdxs,dt,q);
% end
%
% %  Compute time partials and insert them in Jacobian
% obj.ComputeQuadInitTimeJac(funData,funIdxs,dt,meshPointIdx,q);
% obj.ComputeQuadFinalTimeJac(funData,funIdxs,dt,meshPointIdx,q);
% end
%
% %% This block of methods computes constraint values.
%
% function q = ComputeQuadConstraint(obj,funData,funIdxs,dt,q)
% %  Computes Jacobian of simpson integrand constraint
% %  Extract data.  This is mainly for code readability  later
% yk = funData{1}.stateVec;
% ykplus1 = funData{3}.stateVec;
% fk = funData{1}.funcValues;
% fkbarplus1 = funData{2}.funcValues;
% fkplus1 = funData{3}.funcValues;
% %  Compute the constraint and insert in defect vector
% %  This is equation 4.104 of Ref [1]
% qVec = [fkplus1;4*fkbarplus1;fk];
% q = (fkplus1+4*fkbarplus1+fk);
% B = -dt/6*eye(obj.numStateVars);
% conValues = ykplus1 - yk + B*q; %dt/6*q;
% InsertDefectCon(obj,funIdxs,conValues);
% end
%
% function q = ComputeInterpConstraint(obj,funData,funIdxs,dt,q)
% %  Compute the Hermite Interpolant defect constraint
% % Compute the constraint and insert in defect vector
% %  This is Eq.  4.103 of Ref[1]
% yk = funData{1}.stateVec;
% yb = funData{2}.stateVec;
% ykplus1 = funData{3}.stateVec;
% fk = funData{1}.funcValues;
% fkplus1 = funData{3}.funcValues;
% q = (fk - fkplus1);
% conValues = yb - 0.5*(ykplus1 +  yk)- dt*0.125*q;
% InsertDefectCon(obj,funIdxs,conValues);
% end
%
% function QuadFunction(obj,funData,funIdxs,dt)
%
% %  Extract data.  This is mainly for code readability  later in
% %  the function.
% jk = funData{1}.funcValues;
% jbarkplus1 = funData{2}.funcValues;
% jkplus1 = funData{3}.funcValues;
%
% %  Compute the quadrature sume
% %  This is equation 4.104 of Ref [1]
% obj.ptrPhase.costFuncIntegral = obj.ptrPhase.costFuncIntegral + ...
%     dt/6*(jkplus1 + 4*jbarkplus1 + jk);
% end
%
% %% This block of methods computes partial derivatives.  These functions should
% %  NOT be called directly.  They are called via the manager methods
% %  above.
%
% function ComputeQuadFunStateJac(obj,funData,funIdxs,dt,funType)
% %  Computes Jacobian of simpson integrand w/r/t state
% %ykplus1 - yk - dt/6*(fkplus1 + 4*fkbarplus1 + fk );
% stateJac_yk =  dt/6*funData{1}.stateJac;
% stateJac_yb =  4*dt/6*funData{2}.stateJac;
% stateJac_ykplus1 = dt/6*funData{3}.stateJac;
% %  Insert them in the right place in the Jacobian
% funIdx = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% if funType == 1
%     obj.SumCostJacTerms(stateJac_yk,funIdx,funData{1}.stateIdxs);
%     obj.SumCostJacTerms(stateJac_yb,funIdx,funData{2}.stateIdxs);
%     obj.SumCostJacTerms(stateJac_ykplus1,funIdx,funData{3}.stateIdxs);
% end
% end
%
% function ComputeQuadFunInitTimeJac(obj,funData,funIdxs,dt,meshPointIdx,funType)
% nlpIdxs = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% %  Compute Jacobian of Simpson integrand w/r/t final time
% fk = funData{1}.funcValues;
% fkplus1 = funData{3}.funcValues;
% fkbarplus1 = funData{2}.funcValues;
% dDeltaTdTI = -(obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0) - ...
%     obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkdTI = (1 - obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkbarplus1dTI = (1 - obj.MeshPointIdxToNonDimTime(meshPointIdx,1));
% dtkplus1dTI = (1 - obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0));
%
% initTimeJac = dDeltaTdTI/6*(fkplus1 + 4*fkbarplus1 + fk) + dt/6*(...
%     funData{3}.timeJac*dtkplus1dTI + 4*funData{2}.timeJac*dtkbarplus1dTI +...
%     funData{1}.timeJac*dtkdTI);
% %  TODO: Don't hard code time partials
% %  TODO: Get second term correct test case is exacly zero
% %A = dDeltaTdTI/6*(fkplus1 + 4*fkbarplus1 + fk);
% %B =  dt/6*eye(obj.numStateVars,obj.numStateVars);
% %Q = (...
% %     funData{3}.timeJac*dtkplus1dTI + 4*funData{2}.timeJac*dtkbarplus1dTI +...
% %     funData{1}.timeJac*dtkdTI)
%
% %  TODO: Don't hard code time partials index
%
% if funType == 1
%     obj.SumCostJacTerms(initTimeJac,nlpIdxs,1);
%     %D = obj.costFuncProps.timeJacPattern;
%     %obj.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,1,funData{1}.stateIdxs);
% end
%
% end
%
% function ComputeQuadFunFinalTimeJac(obj,funData,funIdxs,dt,meshPointIdx,funType)
% %  Compute Jacobian of Simpson integrand w/r/t initial time
% funIdx = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% fk = funData{1}.funcValues;
% fkplus1 = funData{3}.funcValues;
% fkbarplus1 = funData{2}.funcValues;
% dDeltaTdTF = (obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0) - ...
%     obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkdTF = (obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkbarplus1dTF = (obj.MeshPointIdxToNonDimTime(meshPointIdx,1));
% dtkplus1dTF = (obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0));
% finalTimeJac = dDeltaTdTF/6*(fkplus1 + 4*fkbarplus1 + fk) + dt/6*(...
%     funData{3}.timeJac*dtkplus1dTF + 4*funData{2}.timeJac*dtkbarplus1dTF +...
%     funData{1}.timeJac*dtkdTF);
% %  TODO: Don't hard code time partials
% %  TODO: Get second term correct test case is exacly zero
%
% if funType == 1
%     obj.SumCostJacTerms(finalTimeJac,funIdx,2);
% end
% end
%
% function ComputeQuadFunControlJac(obj,funData,funIdxs,dt,funType)
% %  Computes Jacobian of simpson integrand w/r/t control
% controlJac_uk = dt/6*funData{1}.controlJac;
% controlJac_ub = 4*dt/6*funData{2}.controlJac;
% controlJac_ukplus1 = dt/6*funData{3}.controlJac;
% funIdx = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% %  Insert them into the Jacobian
% if funType == 1
%     obj.SumCostJacTerms(controlJac_uk,funIdx,funData{1}.controlIdxs);
%     obj.SumCostJacTerms(controlJac_ub,funIdx,funData{2}.controlIdxs);
%     obj.SumCostJacTerms(controlJac_ukplus1,funIdx,funData{3}.controlIdxs);
% end
% end
%
% function ComputeQuadStateJac(obj,funData,funIdxs,dt,q)
% %  Computes Jacobian of simpson integrand w/r/t state
% %  conValue = ykplus1 - yk - dt/6*(fkplus1 + 4*fkbarplus1 + fk );
%
% %  Compute commmon arrays
% D = obj.ptrPhase.dynFuncProps.stateJacPattern;
%
% %  Partials w/r/t state at yk
% nlpIdxs = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% A = -eye(obj.numStateVars);
% B = -dt/6*eye(obj.numStateVars);
% Q = funData{1}.stateJac;
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{1}.stateIdxs);
% %  Partials w/r/t state at ykbarplus1
% A = zeros(obj.numStateVars);
% B = -4*dt/6*eye(obj.numStateVars);
% Q = funData{2}.stateJac;
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{2}.stateIdxs);
% %  Formulate the partials
% A = eye(obj.numStateVars);
% B = -dt/6*eye(obj.numStateVars);
% Q = funData{3}.stateJac;
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{3}.stateIdxs);
% end
%
% function ComputeQuadControlJac(obj,funData,funIdxs,dt,q)
% %  Computes Jacobian of simpson integrand w/r/t control
% %  conValue = ykplus1 - yk - dt/6*(fkplus1 + 4*fkbarplus1 + fk );
%
% %  Compute commmon arrays
% D = obj.ptrPhase.dynFuncProps.controlJacPattern;
%
% nlpIdxs = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% %  Partials w/r/t control at point k
% A = zeros(obj.numStateVars,obj.numControlVars);
% B = -dt/6*eye(obj.numStateVars,obj.numStateVars);
% Q = funData{1}.controlJac;
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{1}.controlIdxs);
% %  Partials w/r/t control at point kbarplus1
% B = -4*dt/6*eye(obj.numStateVars,obj.numStateVars);
% Q = funData{2}.controlJac;
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{2}.controlIdxs);
% %  Partials w/r/t control at point k plus 1
% B = -dt/6*eye(obj.numStateVars,obj.numStateVars);
% Q = funData{3}.controlJac;
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{3}.controlIdxs);
% end
%
% function ComputeQuadInitTimeJac(obj,funData,funIdxs,dt,meshPointIdx,q)
%
% %  Compute commmon quantities
% nlpIdxs = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
%
% %  Compute Jacobian of Simpson integrand w/r/t final time
% fk = funData{1}.funcValues;
% fkplus1 = funData{3}.funcValues;
% fkbarplus1 = funData{2}.funcValues;
% dDeltaTdTI = -(obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0) - ...
%     obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkdTI = (1 - obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkbarplus1dTI = (1 - obj.MeshPointIdxToNonDimTime(meshPointIdx,1));
% dtkplus1dTI = (1 - obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0));
% A = -dDeltaTdTI/6*(fkplus1 + 4*fkbarplus1 + fk);
% B = -dt/6*eye(obj.numStateVars,obj.numStateVars);
% Q = (funData{3}.timeJac*dtkplus1dTI + ...
%     4*funData{2}.timeJac*dtkbarplus1dTI +...
%     funData{1}.timeJac*dtkdTI);
% D = obj.ptrPhase.dynFuncProps.timeJacPattern;
% %  TODO: Don't hard code time partials index
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,1);
%
% end
%
% function ComputeQuadFinalTimeJac(obj,funData,funIdxs,dt,meshPointIdx,q)
% %  Compute Jacobian of Simpson integrand w/r/t initial time
% nlpIdxs = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% fk = funData{1}.funcValues;
% fkplus1 = funData{3}.funcValues;
% fkbarplus1 = funData{2}.funcValues;
% dDeltaTdTF = (obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0) - ...
%     obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkdTF = (obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkbarplus1dTF = (obj.MeshPointIdxToNonDimTime(meshPointIdx,1));
% dtkplus1dTF = (obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0));
% A = -dDeltaTdTF/6*(fkplus1 + 4*fkbarplus1 + fk);
% B = -dt/6*eye(obj.numStateVars,obj.numStateVars);
% Q = (funData{3}.timeJac*dtkplus1dTF + ...
%     4*funData{2}.timeJac*dtkbarplus1dTF +...
%     funData{1}.timeJac*dtkdTF);
% D = obj.ptrPhase.dynFuncProps.timeJacPattern;
% %  TODO: Don't hard code time partials index
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,2);
% end
%
% function ComputeInterpFinalTimeJac(obj,funData,funIdxs,dt,meshPointIdx,q)
% %  Compute Jacobian of Interpolant w/r/t initial time
% nlpIdxs = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% fk = funData{1}.funcValues;
% fkplus1 = funData{3}.funcValues;
% dDeltaTdTF = (obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0) - ...
%     obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkdtf = (obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkplus1dtf = (obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0));
% A = -dDeltaTdTF/8*(fk - fkplus1);
% B = -dt/8*eye(obj.numStateVars,obj.numStateVars);
% Q = funData{1}.timeJac*dtkdtf  - funData{3}.timeJac*dtkplus1dtf;
% D = obj.ptrPhase.dynFuncProps.timeJacPattern;
% %  TODO: Don't hard code time partials index
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,2);
% end
%
% function ComputeInterpInitTimeJac(obj,funData,funIdxs,dt,meshPointIdx,q)
% %  Compute Jacobian of Interpolant w/r/t initial time
%
% %  Compute initial time partials and insert them in Jacobian
% %  Equation: - dt*0.125*(fk - fkplus1);
% nlpIdxs = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% fk = funData{1}.funcValues;
% fkplus1 = funData{3}.funcValues;
% dDeltaTdTI = -(obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0) - ...
%     obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkdTI = (1 - obj.MeshPointIdxToNonDimTime(meshPointIdx,0));
% dtkplus1dTI = (1 - obj.MeshPointIdxToNonDimTime(meshPointIdx+1,0));
% A = -dDeltaTdTI/8*(fk - fkplus1);
% B = -dt/8*eye(obj.numStateVars,obj.numStateVars);
% Q = funData{1}.timeJac*dtkdTI  - funData{3}.timeJac*dtkplus1dTI;
% D = obj.ptrPhase.dynFuncProps.timeJacPattern;
% %  TODO: Don't hard code time partials index
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,1);
% end
%
% function ComputeInterpControlJac(obj,funData,funIdxs,dt,q)
% %  Computes Jacobian of Hermite Interpolant w/r/t control
% nlpIdxs = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% A = zeros(obj.numStateVars,obj.numControlVars);
% B = -dt*0.125*eye(obj.numStateVars,obj.numStateVars);
% Q = funData{1}.controlJac;
% D = obj.ptrPhase.dynFuncProps.controlJacPattern;
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{1}.controlIdxs);
% B = dt*0.125*eye(obj.numStateVars,obj.numStateVars);
% Q = funData{3}.controlJac;
% D = obj.ptrPhase.dynFuncProps.controlJacPattern;
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{3}.controlIdxs);
% end
%
% function ComputeInterpStateJac(obj,funData,funIdxs,dt,q)
% %  Compute Jacobian of Interpolant w/r/t State vars
% nlpIdxs = obj.ptrPhase.defectConStartIdx + funIdxs - 1;
% % Partials w/r/t state yk
% A = -0.5*eye(obj.numStateVars);
% B = -dt*0.125*eye(obj.numStateVars,obj.numStateVars);
% Q = funData{1}.stateJac;
% D = obj.ptrPhase.dynFuncProps.stateJacPattern;
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{1}.stateIdxs);
% % Partials w/r/t state yk barplus1
% A = eye(obj.numStateVars);
% B = zeros(obj.numStateVars,obj.numStateVars);
% Q = zeros(obj.numStateVars,obj.numStateVars);
% D = zeros(obj.numStateVars,obj.numStateVars);
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{2}.stateIdxs);
% % Partials w/r/t state ykplus1
% A = -0.5*eye(obj.numStateVars);
% B = dt*0.125*eye(obj.numStateVars,obj.numStateVars);
% Q = funData{3}.stateJac;
% D = obj.ptrPhase.dynFuncProps.stateJacPattern;
% obj.ptrPhase.UpdateConJacMatrices(A,B,D,Q,nlpIdxs,...
%     funData{3}.stateIdxs);
% end
%
% %%  This block of code contains bookkeeping and utility functions
% function InsertDefectCon(obj,conIdxs,conValues)
% %  If statement is a workaround for intlab.
% if isempty(obj.ptrPhase.defectConVec)
%     obj.ptrPhase.defectConVec = conValues;
% else
%     % only need this in GMAT.
%     obj.ptrPhase.defectConVec(conIdxs,:) = conValues;
% end
% end
%  Compute the defect constraints
% function ComputeQuadratures(obj)
% %  Compute defect constraints and quadratures
%
% %  If there are no path functions, nothing to do so return.
% if ~obj.ptrPhase.PathFunction.hasDynFunctions && ...
%         ~obj.ptrPhase.PathFunction.hasCostFunction
%     obj.recomputeQuadratures = false();
%     return
% end
%
% %  Loop over meshes and compute defect constraints and
% %  quadratures
% obj.ptrPhase.costFuncIntegral = 0;
% obj.ptrPhase.jacArray = obj.ptrPhase.jacArray*0;  % Major Hack!!
% %  Workaround for IntLab, not necessary in GMAT
% obj.ptrPhase.defectConVec = [];
%
% for meshPointIdx = 1:obj.numMeshPoints - 1;
%
%     %  Determine relevant indeces for current mesh
%     [quadConIdxs,interpConIdxs] = obj.MeshPointIdxToDefectIdxs(meshPointIdx);
%     timeStep = obj.MeshPointIdxToTimeStep(meshPointIdx);
%     funIdxs = obj.MeshPointIdxToFunIdx(meshPointIdx);
%
%     if obj.ptrPhase.PathFunction.hasCostFunction
%         %  Perform quadrature for cost function
%         funData = obj.ptrPhase.costIntFuncData(funIdxs);
%         obj.QuadSum(funData,funIdxs,timeStep,meshPointIdx)
%     end
%
%     if obj.ptrPhase.PathFunction.hasDynFunctions
%         %  Extract function data for this mesh
%         funData = obj.ptrPhase.userDynFuncData(funIdxs);
%         %  Compute the defect constraints
%         obj.InterpDefectCon(funData,interpConIdxs,timeStep,meshPointIdx);
%         obj.QuadDefectCon(funData,quadConIdxs,timeStep,meshPointIdx);
%     end
%
% end
% %  Now that quadratures are updated set flag to recompute to
% %  false
% obj.ptrPhase.recomputeQuadratures = false();
%
% end

%             Loop through mesh intervals
%                For
%             for i = 1:numMeshIntervals
%                 %  Compute the number of points in each mesh interval
%                 %  and the step size in the mesh interval
%                 obj.numMeshSteps(i) = obj.ptrConfig.GetNumPointsInMeshInterval(i)-1;
%                 obj.numMeshPoints   = obj.numMeshPoints + obj.numMeshSteps(i);
%                 obj.meshStepSizes(i) =(obj.ptrConfig.GetMeshIntervalFraction(i+1)-...
%                     obj.ptrConfig.GetMeshIntervalFraction(i))/obj.numMeshSteps(i);
%
%                 %  Compute state step sizes and number of state steps
%                 obj.stageStepSizes(i) = obj.meshStepSizes(i)/2;
%                 obj.numStagePointsPerMesh(i) = obj.numMeshSteps(i)*2;
%
%                 %  Compute the total number of points in the phase
%                 obj.totalNumPoints = obj.totalNumPoints + ...
%                     obj.numMeshSteps(i)*(1 + obj.numStateStagePointsPerMesh);
%             end
%             obj.numMeshPoints = obj.numMeshPoints + 1;
%
%             %  Compute the time vector including all mesh intervals/stages
%             %  Loop over mesh intervals
%             obj.discretizationPoints = zeros(obj.totalNumPoints,1);
%             cnt     = 0;
%             for meshIdx = 1:numMeshIntervals
%                 %  Loop through the step sizes
%                 cnt = cnt + 1;
%                 obj.discretizationPoints(cnt) = ...
%                     obj.ptrConfig.GetMeshIntervalFraction(meshIdx);
%                 for pointIdx = 1:obj.numStagePointsPerMesh(meshIdx) - 1
%                     cnt = cnt + 1;
%                     obj.discretizationPoints(cnt) =...
%                         obj.discretizationPoints(cnt-1) + ...
%                         obj.stageStepSizes(meshIdx);
%                 end
%             end
