function mat = RandomArrayOfOnes(numRows,numCols,numNonZeros)

%  Construct the sparse A matrix
for idx = 1:numNonZeros
    rowIdx = randi([1 numRows]);
    colIdx = randi([1 numCols]);
    mat(rowIdx,colIdx) = 1;
end
