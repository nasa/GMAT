function state = Interpolate(ephemSegment, epoch)
    %INTERPOLATE Perform a Lagrange interpolation.
    %   FI = INTERPOLATE(OBJ, EPHEMSEGMENT, EPOCH) performs a Lagrange
    %   interpolation returning the state STATE in the ephemeris segment
    %   EPHEMSEGMENT at epoch EPOCH.

    %   Author: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    
    %% Input parsing
    n = ephemSegment.InterpolationDegree;
    
    %% Sanity Checks
    eMin = min(ephemSegment.Data(:,1));
    eMax = max(ephemSegment.Data(:,1));
    if (epoch < eMin) || ...
            (epoch > eMax)
        throw(MException(['events:LagrangeInterpolator:Interpolate:' ...
            'independentVariableOutOfBounds'], ...
            ['Independent variable value %f must be within the range ' ...
            '[%f %f]'], epoch, eMin, eMax));
    end
    
    numStates = size(ephemSegment.Data, 1);
    if n >= numStates
        throw(MException(['events:LagrangeInterpolator:Interpolate:' ...
            'insufficientDataPoints'], ...
            ['Interpolation order %i must be less than the number of ' ...
            'independent variables (%i)'], n, numStates));
    end
    
    %% Initialization    
    state = zeros(1, 6);

    % find intended position of epoch in ephemeris data
    iEpoch = find(ephemSegment.Data(:,1) > epoch, 1);
    
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
        diff = abs((ephemSegment.Data(i,1) + ephemSegment.Data(i+n,1)) ...
            / 2 - epoch);
        if diff > pDiff
            break;
        else
            q = i;
        end
        pDiff = diff;
    end
    
    %% Interpolation Algorithm
%     for i=q:q+n
%         prod = ephemSegment.Data(i,2:7);
%         for j=q:q+n
%             if i ~= j
%                 num = (epoch - ephemSegment.Data(j,1));
%                 denom = (ephemSegment.Data(i,1) - ephemSegment.Data(j,1));
%                 prod = prod * ( num / denom );
%             end
%         end
%         state = state + prod;
%     end
    tempData = ephemSegment.Data;
    tempData(:,1) = (tempData(:,1) - epoch)*86400;
    epoch = 0;
    for i=q:q+n
        prod = tempData(i,2:7);
        for j=q:q+n
            if i ~= j
                num = (epoch - tempData(j,1));
                denom = (tempData(i,1) - tempData(j,1));
                prod = prod * ( num / denom );
            end
        end
        state = state + prod;
    end
end
