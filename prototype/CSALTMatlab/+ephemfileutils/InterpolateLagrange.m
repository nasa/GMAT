function state = InterpolateLagrange(data, t, order)
    %INTERPOLATELAGRANGE Perform a Lagrange interpolation of state data.
    %   STATE = INTERPOLATELAGRANGE(DATA, T, ORDER) performs a Lagrange
    %   interpolation returning the interpolated state STATE in the
    %   ephemeris data DATA at the requested epoch T. The interpolation is
    %   performed at the order given by the ORDER argument.
    %
    %   DATA must be an n-by-7 matrix, where n is the number of ephemeris
    %   states. The columns must be:
    %       epoch (numeric format)
    %       x position
    %       y position
    %       z position
    %       x velocity
    %       y velocity
    %       z velocity

    %   Author: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    %   Adapted from original code by Steven Hughes
    %       <steven.p.hughes@nasa.gov>
    %   See SVN for revision history.
    
    %% Input parsing
    n = order;
    
    %% Sanity Checks
    tMin = min(data(:,1));
    tMax = max(data(:,1));
    if (t < tMin) || ...
            (t > tMax)
        throw(MException(['InterpolateLangrange:' ...
            'independentVariableOutOfBounds'], ...
            ['Independent variable value %f must be within the range ' ...
            '[%f %f]'], t, tMin, tMax));
    end
    
    numStates = size(data, 1);
    if n >= numStates
        throw(MException(['InterpolateLagrange:' ...
            'insufficientDataPoints'], ...
            ['Interpolation order %i must be less than the number of ' ...
            'independent variables (%i)'], n, numStates));
    end
    
    %% Initialization    
    state = zeros(1, 6);

    % find intended position of epoch in ephemeris data
    iEpoch = find(data(:,1) > t, 1);
    
    % pick starting point for interpolation data
    % (region ending just before epoch's position in the ephemeris)
    if (n+1 >= iEpoch)
        initIndex = 1;
    else
        initIndex = iEpoch - (n+1);
    end
    
    % slide interpolation data region forward until epoch is nearest
    % numerical center
    pDiff = intmax();
    for i = initIndex:(numStates-n)
        diff = abs((data(i,1) + data(i+n,1)) / 2 - t);
        if diff > pDiff
            break;
        else
            q = i;
        end
        pDiff = diff;
    end
    
    %% Interpolation Algorithm
    for i=q:q+n
        prod = data(i,2:7);
        for j=q:q+n
            if i ~= j
                prod = prod * ((t - data(j,1)) / ...
                    (data(i,1) - data(j,1)));
            end
        end
        state = state + prod;
    end
end
