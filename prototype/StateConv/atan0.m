function y=atan0(y,x)

y=atan2(y,x);

if y < 0
    y=y+2*pi;
end