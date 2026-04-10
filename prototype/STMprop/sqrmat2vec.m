function vec = sqrmat2vec(mat)

n = size(mat,1);
vec = [];
for i = 1:n
   vec = [vec; mat(i,:)']; 
end

