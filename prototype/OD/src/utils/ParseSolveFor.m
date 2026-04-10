function [objName,propName] = ParseSolveFor(solveFor)

length = size(solveFor,2);
index = strfind(solveFor,'.');

%  Assume there is only one level of nesting at this point!!

objName = solveFor(1:index-1);
propName = solveFor(index+1:length);