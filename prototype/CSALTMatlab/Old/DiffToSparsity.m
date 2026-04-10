function [sparsePattern] = DiffToSparsity(obj,diffVec)
%  Returns a sparsity pattern for difference array.  If diff value is >
%  1e-14, then sparsity is 1, otherwise zero.

diffTol = 1e-14;
numDiffs = length(diffVec);
sparsePattern = zeros(numDiffs,1);
for diffIdx = 1:numDiffs
   if abs(diffVec(diffIdx)) > diffTol
       sparsePattern(diffIdx) = 1;
   end
end