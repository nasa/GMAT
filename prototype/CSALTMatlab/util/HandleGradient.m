function vec = HandleGradient(vec)
%  Removes gradient type info if a gradient
%  Not necessary in GMAT!
if isa(vec,'gradient')
    vec = vec.x;
end
