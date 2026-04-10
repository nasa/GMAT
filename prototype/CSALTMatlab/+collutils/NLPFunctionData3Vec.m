classdef NLPFunctionData3Vec < handle
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (SetAccess = 'private')
        
        % Sparse Matrix. The sparse matrix representation
        spMatrix
        % Integer array. Vector of row indeces of non-zero elements
        matRowIdxs
        % Integer array. Vector of column indeces of non-zero elements
        matColIdxs
        % Real array. The vector of values of non-zero elements
        matValues
        % Integer. The number of nonzero elements
        numNonZeros 
        % Integer. The current Idx for accumulation
        currentIdx
        % Integer. Number of rows in the sparse matrix
        numRows
        % Integer. Number of columns in the sparse matrix
        numCols
        % Integer. Maximum number of allowed non-zeros
        maxNumNonZeros
        % Bool. Flag to indicate accumlation/initialization is not complete
        isAccumulating
        
    end
    
    methods
        
        %  Class constructor
        function obj = NLPFunctionData3Vec(numRows,numCols,maxNumNonZeros)
            obj.numRows = numRows;
            obj.numCols = numCols;
            obj.maxNumNonZeros = maxNumNonZeros;
            obj.spMatrix = spalloc(numRows,numCols,maxNumNonZeros);
        end
        
        % Set the entire vector at once
        function SetVectorRepresentation(obj,matRowIdxs,matColIdxs,matValues)
            obj.matRowIdxs = matRowIdxs;
            obj.matColIdxs = matColIdxs;
            obj.matValues = matValues;
            obj.spMatrix = sparse(matRowIdxs,matColIdxs,matValues,obj.numRows,obj.numCols);
        end
            
%         % Return Vector partitions
%         function [rowIdxs,colIdxs,valVec] = GetVectorRep(obj)
%             %spMatrix = sparse(
%             %[rowIdxs,colIdxs,valVec] = find(
%             keyboard
%         end%  Row Idx

        

    end
    
end

