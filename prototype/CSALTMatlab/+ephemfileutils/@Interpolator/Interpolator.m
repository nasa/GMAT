classdef Interpolator < handle
    %INTERPOLATOR An interpolator.
    
    %   Author: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    %   Date: March 2010
    
    properties
    end
    
    methods (Abstract)
        fi = Interpolate(x, f, xi, n)
    end
    
end

