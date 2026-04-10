classdef NLPFunctionData < handle
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    %  The nlp functions are computed using
    %       nlpFuncs = conAMat*DecVector + conBMat*userFuns
    %  The sparsity pattern is determined using
    %       sparsePattern = conAMat + conBMat*conDMat
    %  The Jacobian is calculated using
    %       nlpJac = conAMat + conBMat*conQMat
    
    properties (Access = 'private')
        
        % Sparse matrix. Decision vector matrix multiplier.
        AMatrix
        % Sparse matrix. Function value multiplier.
        BMatrix
        % Sparse matrix. Function value array.
        QVector
        % Sparse matrix. Function partials matrix
        parQMatrix
        % The sparsity pattern of parQMatrix
        DMatrix
        % Real Array.  The decision vector.
        DecVector
        
        % SparseMatAccumulator object.  Used to accumulate sparse matrices
        conQMatUtil
        
    end
    
    methods
        %% This group of methods initialize and perform housekeeping
        
        %  Initialize the array dimenstions
        function Initialize(obj,numFuncs,numVars,numFuncDependencies)
            
            % A matrix is numFuncs by numVars
            obj.AMatrix = sparse(numFuncs,numVars);
            % B is numFuncs x numFuncDependencies
            obj.BMatrix = sparse(numFuncs,numFuncDependencies);
            % Q numFuncDependencies x 1
            obj.QVector = sparse(numFuncDependencies,1);
            % parQ is numFuncDependencies x numVars
            obj.parQMatrix = sparse(numFuncDependencies,numVars);
            % D Matrix is the same size as parQ matrix
            obj.DMatrix = obj.parQMatrix ;
            % DecVector is numVarsx1
            obj.DecVector = zeros(numVars,1);
            
        end
        
        %%  This group of methods inserts (replaces) partitions into various matrices
        
        function InsertAMatPartition(obj,A,rowIdxs,colIdxs)
            % Inserts elements into A matrix
            obj.AMatrix(rowIdxs,colIdxs) = A;
        end
        
        function InsertBMatPartition(obj,B,rowIdxs,colIdxs)
            % Inserts elements into B matrix
            obj.BMatrix(rowIdxs,colIdxs) = B;
        end
        
        function InsertQVecPartition(obj,Qvec,rowIdxs)
            % Inserts elements into Q Vector
            obj.QVector(rowIdxs,1) = Qvec;
        end
        
        function InsertparQMatPartition(obj,parQ,rowIdxs,colIdxs)
            % Inserts elements into parQ matrix
            obj.parQMatrix(rowIdxs,colIdxs) = parQ;
        end
        
        function InsertDMatPartition(obj,D,rowIdxs,colIdxs)
            % Inserts elements into D matrix
            obj.DMatrix(rowIdxs,colIdxs) = D;
        end
        
        function SetQVector(obj,Qvec)
            % Set the entire Q Vector
            obj.QVector = Qvec;
        end
        
        function SetparQMatrix(obj,Q)
            % Set the entire par Q matrix
            obj.parQMatrix = Q;
        end
        
        function SetDMatrix(obj,D)
            % Set the entire D matrix
            obj.DMatrix = D;
        end
        
        function [numAZeros,numBZeros,numQZeros] = GetMatrixNumZeros(obj)
            % Returns number of non-zero elements in A, B and Q matrices
            [rowIdxVec] = GetAMatrixThreeVectors(obj);
            numAZeros = length(rowIdxVec);
            [rowIdxVec] = GetBMatrixThreeVectors(obj);
            numBZeros = length(rowIdxVec);
            [rowIdxVec] = GetQMatrixThreeVectors(obj);
            numQZeros = length(rowIdxVec);
        end
        
        function [rowIdxVec,colIdxVec,valVec] = GetAMatrixThreeVectors(obj)
            % Returns three vector representation of AMatrix
            [rowIdxVec,colIdxVec,valVec] = find(obj.AMatrix);
        end
        
        function [rowIdxVec,colIdxVec,valVec] = GetBMatrixThreeVectors(obj)
            % Returns three vector representation of BMatrix
            [rowIdxVec,colIdxVec,valVec] = find(obj.BMatrix);
        end
        
        function [rowIdxVec,colIdxVec,valVec] = GetQMatrixThreeVectors(obj)
            % Returns three vector representation of QMatrix
            [rowIdxVec,colIdxVec,valVec] = find(obj.parQMatrix);
        end
        
        function SetBMatrix(obj,B)
            % Set the entire B matrix
            obj.BMatrix = B;
        end
        
        function SetDecisionVector(obj,DecVector)
            % Set Decision Vector
            obj.DecVector = DecVector;
        end
        
        %%  This group of methods sums partitions into various matrices.
        
        function SumBMatPartition(obj,B,rowIdxs,colIdxs)
            % Sums elements in B matrix
            obj.BMatrix(rowIdxs,colIdxs) = obj.BMatrix(rowIdxs,colIdxs) + B;
        end
        
        %% This block of methods accumulates matrices
        function AccumParQMatrix(obj,rowStart,colStart,matChunk,spPattern)
            
        end
        
        %%  This block of methods computes NLP functions once matrices are populated
        
        function funcValues = ComputeFunctions(obj)
            % Computes NLP functions
            funcValues = obj.AMatrix*obj.DecVector + obj.BMatrix*obj.QVector;
        end
        
        function funcJacobian = ComputeJacobian(obj)
            % Computes the NLP function Jacobian
            funcJacobian = obj.AMatrix + obj.BMatrix*obj.parQMatrix;
        end
        
        function funcSparsity = ComputeSparsityPattern(obj)
            %  Compute sparsity pattern
			funcSparsity = obj.ToOnes(obj.ToOnes(obj.AMatrix) + ...
                obj.ToOnes(obj.BMatrix)*obj.ToOnes(obj.DMatrix));
        end
        
        %  Sets sparse matrix elements that are non-zero to one
        function Mat = ToOnes(~,Mat)
            Mat = spones(Mat);
        end
        
    end
    
end

% %  NEW SPARSE HANDLING
% function jacStruct = ComputeJacobianVectors(obj)
% funcJacobian = ComputeJacobian(obj);
% [jacStruct.rowIdxVec,jacStruct.colIdxVec,jacStruct.valVec] = find(funcJacobian ~= 0);
% end
% 
% %  NEW SPARSE HANDLING
% function numNonZeros = GetNumNonZeros(obj)
% % not sure if this is what to do in C++!
% funcJacobian = ComputeJacobian(obj);
% numNonZeros = length(funcJacobian);
% end
% 
% %  NEW SPARSE HANDLING
% function jacStruct = ComputeSparsityPatternVectors(obj)
% %  Compute sparsity pattern
% funcSparsity = obj.ToOnes(obj.ToOnes(obj.AMatrix) + ...
%     obj.ToOnes(obj.BMatrix)*obj.ToOnes(obj.DMatrix));
% [jacStruct.rowIdxVec,jacStruct.colIdxVec,jacStruct.valVec] = find(funcSparsity ~= 0);
% end
