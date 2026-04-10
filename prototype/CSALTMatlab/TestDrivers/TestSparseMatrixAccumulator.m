
%%  Create a random sparse matrix and random rhs vector for testing A*x

clear all
numRows = 100;
numCols = 110;
numNonZeros = 45;
%  Construct the sparse A matrix
for idx = 1:numNonZeros
    matRowIdxs(idx) = randi([1 numRows]);
    matColIdxs(idx) = randi([1 numCols]);
    matValues(idx) = randn();
end
spAmatrix = sparse(matRowIdxs,matColIdxs,matValues,numRows,numCols);

% Construct random x vector
for idx = 1:numCols
    xVector(idx,1) =  randn();
end


%%  Test setting the three vector representation
truthspMat = spAmatrix*sparse(xVector);

sMatrix = collutils.NLPFunctionData3Vec(numRows,numCols,numRows*numCols)
import collutils.SparseMatAccumulator
matAccum = collutils.SparseMatAccumulator();
sMatrix.SetVectorRepresentation(matRowIdxs,matColIdxs,matValues)

%% Now loop over the matrix setting it in chunks
done = false;
currRowIdx = 1;
currColIdx = 1;
numRows = 100;
numCols = 110;
numNonZeros = 45;
while ~done
   %  Create small random subchunks
    chunkRowIdxs(idx) = randi([1 numRows]);
    chunkColIdxs(idx) = randi([1 numCols]);
    chunkValues(idx) = randn();
   
end





