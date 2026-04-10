function D=collocD(x);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% collocD.m
%
% Computes the pseudospectral/collocation differentiation matrix for the 
% arbitrary nodes stored in the vector x. Uses the lagrange polynomial 
% formulation.
%
% Reference:
%
% Jean-Paul Berrut & Lloyd N. Trefethen, "Barycentric Lagrange Interpolation" 
  % http://web.comlab.ox.ac.uk/oucl/work/nick.trefethen/berrut.ps.gz
%
  % Written by: Greg von Winckel       07/18/04
  % Contact:    gregvw@chtm.unm.edu 
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Make x a column vector if it isn't already and order it
% and get the number of nodes
x=sort(x(:));                       N=length(x); N1=N+1; N2=N*N;

% Compute the barycentric weights
X=repmat(x,1,N);                    Xdiff=X-X'+eye(N);
W=repmat(1./prod(Xdiff,2),1,N);     D=W./(W'.*Xdiff); 
D(1:N1:N2)=1-sum(D);                D=-D';
