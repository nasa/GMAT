function [tau,w,D] = lgrPS(s,N);

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
rowshift = 0;
colshift = 0;
for i=1:length(N)
  [xcurr,wcurr,Pcurr] = lgrnodes(N(i)-1);
  rows = rowshift+1:rowshift+N(i);
  cols = colshift+1:colshift+N(i)+1;
  scurr = (s(i+1)-s(i))*xcurr/2+(s(i+1)+s(i))/2;
  tau(rows) = scurr;
  w(rows) = (s(i+1)-s(i))*wcurr/2;
  scurr = [scurr; s(i+1)];
  Dcurr = collocD(scurr);
  Dcurr = Dcurr(1:end-1,:);
  D(rows,cols) = Dcurr;
  rowshift = rowshift+N(i);
  colshift = colshift+N(i);
end;
tau(end)=+1;
