function [tau,w,D] = lgrPS(s,N)

%--------------------------------------------------------%
% This function computes the points, weights, and        %
% differentiation matrix for use with a multiple-segment %
% Radau pseudospectral method.                           %
% INPUTS                                                 %
%  s:  a vector of length K+1 that contains mesh points  %
%      on the closed interval [-1,+1]. s must be         %
%      monotonically increasing on [-1,+1].              %
%  N:  a vector of length K that contains the degree of  %
%      the polynomial within each mesh interval          %
% OUTPUTS                                                %
%  tau: a vector of LGR points on [-1,+1]                %
%  w:   a vector of LGR weights                          %
%  D:   the LGR differentiation matrix                   %
%--------------------------------------------------------%

% s = [-1; s];
Ntotal = sum(N);
tau = zeros(Ntotal+1,1);
w   = zeros(Ntotal,1);
D   = sparse(Ntotal,Ntotal+1);
% it is just as large as 14 by 14 double matrix. 
% do we need to make it a sparse matrix?


rowshift = 0;
colshift = 0;
for i=1:length(N)
  %% compute the standard Legendre-Gauss-Radau nodes 
  [xcurr,wcurr,Pcurr] = lgrnodes(N(i)-1);
  % xcurr is the lgr points 
  % wcurr is the weight vector
  % Pcurr is the Legendre-Gauss-Radau Vandermonde matrix
  
  %% compute row and column index shifts 
  rows = rowshift+1:rowshift+N(i);
  cols = colshift+1:colshift+N(i)+1;
  %% obtain radau points for the single segment
  % scurr is the radau points for a mesh interval considering both 
  % the length factor '(s(i+1)-s(i))/2' and shift '(s(i+1)+s(i))/2'  
  scurr = (s(i+1)-s(i))*xcurr/2+(s(i+1)+s(i))/2;
  tau(rows) = scurr;
  %% obtain radau weights for the single segment
  w(rows) = (s(i+1)-s(i))*wcurr/2;
  % in the case of the weigth vector, only consider the length factor
  % and do not consider the shift.
  %% obtain radau differentiation matrix
  scurr = [scurr; s(i+1)];
  Dcurr = collocD(scurr);
  Dcurr = Dcurr(1:end-1,:);
  D(rows,cols) = Dcurr;
  %% update the row and column shitfs
  rowshift = rowshift+N(i);
  colshift = colshift+N(i);
end;
tau(end)=+1;
