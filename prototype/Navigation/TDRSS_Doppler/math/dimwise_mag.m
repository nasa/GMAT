function mag = dimwise_mag(A,dim)
% Compute the norm of an array of vectors along the specified direction
% Input: array A of any size, dim, 'row' or 'col'
%
%
% Output: Return an array of vector magnitudes along the dimension specified. 

[ m n ] = size(A);

if strcmp(dim,'col')
    dimension = 2;
end


if strcmp(dim,'row')
    dimension = 1;
end

    if dimension==1
        mag = NaN(1,m);
        for i = 1:m
            mag(i) = norm(A(i,1:n));
        end
    end

    if dimension==2
        mag = NaN(1,n);
        for i=1:n
            mag(i) = norm(A(1:m,i));
         end
    end
       
end
