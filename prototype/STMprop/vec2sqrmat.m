function mat = vec2sqrmat(vec);

n = sqrt(size(vec,1));
mat = [];
nm1 = n - 1;
for i = 1:n
   mat = [mat; vec(n*i - nm1:n*i,1)']; 
end;