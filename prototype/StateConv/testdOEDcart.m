oe = [80000 .9 28.5*pi/180 2 3 0];

cart = stateconv(oe,2,1);
rv = cart(1:3,1);
vv = cart(4:6,1);

for i = 1:6
    
     %  Grad of OE w/r/t Cart
     dcart = zeros(6,1);
     if i <= 3
         dx = .00001;
     else
         dx = .000000001;
     end
     dcart(i) = dx;
     
     fplusdx = stateconv(cart + dcart,1,2);
     Joe(1:6,i) = (fplusdx - oe)'/dx;
     
     %  Grad of Cart w/r/t oe
     doe = zeros(1,6);
     doe(i) = .00000000001;
     fplusdx = stateconv(oe + doe,2,1);     
     Jcart(1:6,i) = (fplusdx - cart)/doe(i);
     
end

Joe;
doe = dOEdcart(cart,1);

for i = 1:6
    for j = 1:6
        perDiff(i,j) = abs(doe(i,j) - Joe(i,j))/Joe(i,j);
    end
end
perDiff


% Jcart
% dcart = dCartdOE(cart,1)
max(Joe - doe);
% max(Jcart - dcart)