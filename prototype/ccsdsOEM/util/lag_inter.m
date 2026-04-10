function yi=Lag_inter(x,y,xi)

%  function yi=Lag_inter(x,y,xi)
%
%  This function uses Lagrange's interpolating polynomials to calulate 
%  value of the dependant variable,yi,  for a given value of the 
%  independant variable, xi, and some surrounding data points.  The order
%  of the interpolation is (n-1) where n is the number of data points
%  contained in x and y.
%
%  Variable I/O
%    Input
%      x = n-dimensional row vector of independant variable values
%      y = n-dimensional row vector of dependant variable values
%      xi = independant variable value to be used in interpolation
%    Ouput
%      yi = dependant variable value at xi
%
%   External References:  None
%   Modification History
%             6/7/98      S. Hughes       Created              
%           11/20/00      S. Hughes       Modified so that the code only accepts row vectors


[n]=size(x,1);

yi=0;
for i=1:n
   product=y(i);
   for j=1:n
      if(i~=j)
         product=product*(xi-x(j))/(x(i)-x(j));
      end
   end
   yi=yi+product;
end

   