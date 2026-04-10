function [yValuePrimeArray, yRatePrimeArray] = CubicHermiteSpline ...
    (xVec,yValueArray,yRateArray,xPrimeVec)
%   A cubic spline algorithm for y' & (dydx)' of x' based on x, y, dydx.
%   To get y' & (dydx)', it utilizes the piecewise Hermite Interpolation.
%   It is recommendable that for performance, deriver small-sized x & x'
%   as possible as you can. As numel(x) & numel(x') becomes lager, the
%   speed of the algorithm gets slower, rapidly. 
% 
%   Author: Youngkwang Kim, Dep. of Astronomy, Yonsei University, Korea
%   E-mail: youngkwangkim88@gmail.com
%   Revision Date: 2015.03.04
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   Input: 
%           xVec = x; a strictly ascending real vector defined in the  
%                   domain of a differentiable (vector) function f:x->y;                    
%           yValueArray = f(x)
%           yRateArray = df/dx(x)
%           xPrimeVec = x'; a strictly ascending real vector defined in the  
%                   domain of a differentiable (vector) function f:x->y;  
%   Output:
%           yValuePrimeArray = f(x')
%           yRatePrimeArray  = df/dx(x')
%   (IMPORTANT!) Necessary conditions for inputs:
%           1. The vectors x & x' must be strictly ascending.
%           2. It must hold that x(1) <= x'(i) <= x(end) for any index i.
%           3. For y and dydx of m by n or n by m matrices, numel(x)=m.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
numXVec=numel(xVec);
numXPrimeVec=numel(xPrimeVec);

[numRowYValueArray,numColYValueArray]=size(yValueArray);
[numRowYRateArray,numColYRateArray]=size(yRateArray);
if xVec(1) > xPrimeVec(1) || xVec(end) < xPrimeVec(end)
    error('It must hold that x(1) <= x''(i) <=x(end) for any index i.')
end
if (numRowYValueArray-numRowYRateArray~=0) ...
        || (numColYValueArray-numColYRateArray~=0) 
    error('The size of yVecArray and yRateArray must be same.')
end
% pre-allocation
if numRowYValueArray == numXVec
    DataType='Row-Sequence';
    numVar=numColYValueArray;
    yValuePrimeArray=zeros(numXPrimeVec,numColYValueArray);
    yRatePrimeArray=zeros(numXPrimeVec,numColYValueArray);
elseif numColYValueArray == numXVec
    DataType='Column-Sequence';
    numVar=numRowYValueArray;    
    yValuePrimeArray=zeros(numRowYValueArray,numXPrimeVec);
    yRatePrimeArray=zeros(numRowYValueArray,numXPrimeVec);
else
    error(['It must hold that either ' ...
    '''numel(xVec) ==numel(yValueArray(:,1))''' ...
    ', or ''numel(xVec) ==numel(yValueArray(1,:))''.'])
end 
intvXPIdxBegin=1; 
intvXPIdxEnd=intvXPIdxBegin;  
% x(intvXIdx) <= x'(intvXPIdxBeing:intvXPIdxEnd) < x(intvXIdx+1)
ExitFlag=0;

while 1
    % iterative until all the y' & (dydx)' are found
    if ~isempty(find(xVec>xPrimeVec(intvXPIdxBegin),1))
        intvXIdx=find(xVec>xPrimeVec(intvXPIdxBegin),1)-1;
    else
        % no more intervals
        % check the point
        if xVec(end) == xPrimeVec(end)
            if strcmpi(DataType,'Column-Sequence')
                yValuePrimeArray(:,end)=yValueArray(:,end);
                yRatePrimeArray(:,end)=yRateArray(:,end);
            elseif strcmpi(DataType,'Row-Sequence')
                yValuePrimeArray(end,:)=yValueArray(end,:);
                yRatePrimeArray(end,:)=yRateArray(end,:);                
            end
        end
        break
    end
    intvLB=xVec(intvXIdx);
    intvUB=xVec(intvXIdx+1);

    while xPrimeVec(intvXPIdxEnd+1) < intvUB 
        % find out what is the intervalIdxEnd
        intvXPIdxEnd=intvXPIdxEnd+1;
        if (intvXPIdxEnd) == numXPrimeVec
            ExitFlag=1;
            break
        end
    end

    
    
    h=intvUB-intvLB;
    intvVec=xPrimeVec(intvXPIdxBegin:intvXPIdxEnd)-intvLB;
    % interval vector on [0,h]
    
    if strcmpi(DataType,'Row-Sequence')
        for varIdx=1:numVar
            [yValuePrimeData, ...
                yRatePrimeData]= ...
                CubicHermiteInterpolation( ...
                intvVec,h,yValueArray(intvXIdx,varIdx),...
                yRateArray(intvXIdx,varIdx), ...
                yValueArray(intvXIdx+1,varIdx),...
                yRateArray(intvXIdx+1,varIdx));            
            try
                yValuePrimeArray(intvXPIdxBegin:intvXPIdxEnd,varIdx)=...
                    yValuePrimeData;
                yRatePrimeArray(intvXPIdxBegin:intvXPIdxEnd,varIdx)=...
                    yRatePrimeData;
            catch
                yValuePrimeArray(intvXPIdxBegin:intvXPIdxEnd,varIdx)=...
                    yValuePrimeData.';
                yRatePrimeArray(intvXPIdxBegin:intvXPIdxEnd,varIdx)=...
                    yRatePrimeData.';
            end
        end
    elseif strcmpi(DataType,'Column-Sequence')
        for varIdx=1:numVar
            [yValuePrimeData, ...
                yRatePrimeData]= ...
                CubicHermiteInterpolation( ...
                intvVec,h,yValueArray(varIdx,intvXIdx),...
                yRateArray(varIdx,intvXIdx), ...
                yValueArray(varIdx,intvXIdx+1),...
                yRateArray(varIdx,intvXIdx+1));            
            try
                yValuePrimeArray(varIdx,intvXPIdxBegin:intvXPIdxEnd)=...
                    yValuePrimeData;
                yRatePrimeArray(varIdx,intvXPIdxBegin:intvXPIdxEnd)=...
                    yRatePrimeData;
            catch
                yValuePrimeArray(varIdx,intvXPIdxBegin:intvXPIdxEnd)=...
                    yValuePrimeData.';
                yRatePrimeArray(varIdx,intvXPIdxBegin:intvXPIdxEnd)=...
                    yRatePrimeData.';
            end
        end
    end
    if ~ExitFlag
        intvXPIdxBegin=intvXPIdxEnd+1; 
        intvXPIdxEnd=intvXPIdxBegin;   
    else
        break
    end    
end
end
