classdef SparseMatAccumulator < handle
    
    %SparseMatAccumulator Manages accumulation of sparse matrix data
    %   This class handle setting sparse matrices in small sparse chunks
    %   The class uses std vector to grow arrays during the first
    %   accumulation.  After the initial accumulation, pre-dimensioned
    %   arrays are used.  
    %
    %   It is essential that the number of non-zeros during the first
    %   accumulation, and later accumulations are identical and that the
    %   accumulation is performed in the same order for initialization and
    %   later accumulations.  This is enforced for performance reasons so
    %   that this class operates efficiently.
   
    
    properties (SetAccess = 'private')
        
        % Integer.  The number of non-zeros
        numNonZeros;
        % Integer. The index of current accumlated three vector rep.
        currentIdx;
        % std vec of ints.  Contains row idxs of non-zero elements
        vecMatRowIdxs
        % std vec of ints.  Contains col idxs of non-zero elements
        vecMatColIdxs
        % std vec of ints.  Contains values non-zero elements
        vecMatValues
        
        %  Bool.  Flag indicating initial accumulation is complete.
        isInitialized
    end
    
    methods (Access = public)
        
        function obj = SparseMatAccumulator()
            % Constructor
            obj.isInitialized = false;
            obj.numNonZeros = 0;
            obj.BeginAccumulation();
        end
        
        function BeginAccumulation(obj)
            % Prepares class to accumulate sparse matrix chunks
            obj.currentIdx = 0;
        end
        
        function EndAccumulation(obj)
            % Prepares class to accumulate sparse matrix chunks
            obj.isInitialized = true;
        end
        
        function Accumulate(obj,rowStart,colStart,matChunk,spPattern)
            % Accumulates row-col idxs and matrix values
            
            % Get the row, column idxs for the non-zeros in small chunk
            [localRowIdxs,localColIdxs] = find(spPattern);
            
            % Determine non-zeros in the chunk
            numChunkNonZeros= length(localRowIdxs);
            
            % Loop over the non-zero elements and accumulate them
            for currIdx = 1:numChunkNonZeros
                
                obj.currentIdx = obj.currentIdx + 1;

                if ~obj.isInitialized
                    obj.numNonZeros = obj.numNonZeros + 1;
                    % Accumlate row-col and value data
                    obj.vecMatRowIdxs(obj.currentIdx) = rowStart + localRowIdxs(currIdx) - 1;
                    obj.vecMatColIdxs(obj.currentIdx) = colStart + localColIdxs(currIdx) - 1;
                end
                
                obj.vecMatValues(obj.currentIdx) = ...
                    matChunk(localRowIdxs(currIdx),localColIdxs(currIdx));
                
            end
            
        end
        
        function [rowVector,colVector,valVector] = Get3Vectors(obj)
            % Accumulates row-col idxs and matrix values
            rowVector = obj.vecMatRowIdxs;
            colVector = obj.vecMatColIdxs;
            valVector = obj.vecMatValues;
        end
        
        function numNonZeros = GetNumNonZeros(obj)
            % Returns number of nonzeros
           numNonZeros = obj.numNonZeros; 
        end
        
        function valVector = GetValueVector(obj)
           % Returns the vector of non-zero values
           valVector = obj.vecMatValues;
        end
        
    end
    
end

