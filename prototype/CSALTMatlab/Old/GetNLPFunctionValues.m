function [FuncValues] = GetNLPFunctionValues(obj,PathFunc,PointFunc)
%  Function to hide the gradient type issue from the other areas of the
%  code.  This will not be an issue in GMAT as we will not use IntLab
%  there.

%  Check that the functions are up to date
obj.CheckFunctions()

if obj.hasDefectCons
    if isa(obj.defectConVec,'gradient');
        defectConVec = obj.defectConVec.x;
    else
        defectConVec = obj.defectConVec;
    end
else
    defectConVec = [];
end

if obj.hasAlgPathCons
    if isa(obj.algPathConVec,'gradient');
        algPathConVec = obj.algPathConVec.x;
    else
        algPathConVec = obj.algPathConVec;
    end
else
    algPathConVec = [];
end

if obj.hasAlgEventCons
    if isa(obj.algEventConVec,'gradient');
        algEventConVec = obj.algEventConVec.x;
    else
        algEventConVec = obj.algEventConVec;
    end
else
    algEventConVec = [];
end

if obj.hasIntegralCons
    if isa(obj.intConVec,'gradient');
        intConVec = obj.intConVec.x;
    else
        intConVec = obj.intConVec;
    end
else
    intConVec = [];
end

if obj.hasLinkageCons
    if isa(obj.linkageConVec,'gradient');
        linkageConVec = obj.linkageConVec.x;
    else
        linkageConVec = obj.linkageConVec;
    end
else
    linkageConVec = [];
end

if PathFunc.hasCostFunction
    if isa(obj.costFuncIntegral,'gradient');
        costFuncIntegral = obj.costFuncIntegral.x;
    else
        costFuncIntegral = obj.costFuncIntegral;
    end
else
    costFuncIntegral = [];
end

if PointFunc.hasCostFunction
    if isa(obj.costFuncAlgebraic,'gradient');
        costFuncAlgebraic = obj.costFuncAlgebraic.x;
    else
        costFuncAlgebraic = obj.costFuncAlgebraic;
    end
else
    costFuncAlgebraic = [];
end

FuncValues.defectConVec = defectConVec;
FuncValues.algPathConVec = algPathConVec;
FuncValues.algEventConVec = algEventConVec;
FuncValues.intConVec = intConVec;
FuncValues.linkageConVec = linkageConVec;
FuncValues.costFuncAlgebraic = costFuncAlgebraic;
FuncValues.costFuncIntegral  = costFuncIntegral;

