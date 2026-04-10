function state = Cart2SphRADec(cart)

rMag = norm(cart(1:3,1));
vMag = norm(cart(4:6,1));
rightAsc = atan2(cart(2,1),cart(1,1));
dec  = asin(cart(3,1)/rMag);

rightAscVel = atan2(cart(5,1),cart(4,1));
decVel     = asin(cart(6,1)/vMag);

state = [rMag rightAsc dec vMag rightAscVel decVel]';