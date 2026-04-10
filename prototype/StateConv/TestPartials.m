
%  Define the state vector
oe   = [42095 .8181818 28.5*pi/180 .5 1.5 2.5];
cart = stateconv(oe,2,1);

%  Calculate the partials
doe   = doedcart(cart,1)
dcart = dcartdoe(cart,1)

%  Should get identity matrix if partials are correct
doe*dcart


%---- The rest uses finite differencing to compare to analytic.
da = .01;
de = .00001;
dang = .0001;
dpos = .01;
dvel = .0001;


for i = 1:6;
    
    oepert   = zeros(1,6);
    cartpert = zeros(6,1);
    if i == 1;
        del = da;
        dx  = dpos;
    elseif i == 2;
        del = de;
        dx  = dpos;
    elseif i == 3;
        del = dang;
        dx  = dpos;
    elseif i == 4;
        del = dang;
        dx  = dvel;
    elseif i == 5;
        del = dang;
        dx  = dvel;
    else
        del = dang;
        dx  = dvel;
    end
    
    oepert(1,i)   = del;
    cartpert(i,1) = dx;
    cartnew = cart + cartpert;
    oenew   = oe + oepert;
    oecalc   = stateconv(cartnew,1,2);
    cartcalc = stateconv(oenew,2,1);
    doepert(:,i)   = ( oecalc - oe)/dx;
    dcartpert(:,i) = ( cartcalc - cart)/del;
    
end
        


for i = 1:6
    for j =  1:6;
        oe_pd(i,j) = (doe(i,j) - doepert(i,j))/doe(i,j);
        cart_pd(i,j) = (dcart(i,j) - dcartpert(i,j))/dcart(i,j);
    end
end
oe_pd
cart_pd
