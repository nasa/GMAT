function [FuncValues] = GetUserFunctionValues(obj,PathFunc,PointFunc)
%  Function to hide the gradient type issue from the other areas of the
%  code.  This will not be an issue in GMAT as we will not use IntLab
%  there.

%  Check that the functions are up to date
obj.CheckFunctions()
        
if obj.hasDefectCons
    if isa(PathFunc.dynFunctions,'gradient');
        userDynFuncArray = obj.userDynFuncArray.x;
    else
        userDynFuncArray = obj.userDynFuncArray;
    end
else
    userDynFuncArray = [];
end

if obj.hasAlgPathCons
    if isa(PathFunc.algFunctions,'gradient');
        userAlgPathFuncArray = obj.userAlgPathFuncArray.x;
    else
        userAlgPathFuncArray = obj.userAlgPathFuncArray;
    end
else
    userAlgPathFuncArray = [];
end

if obj.hasAlgEventCons
    if isa(PointFunc.boundaryFunctions,'gradient');
        userEventFuncArray = obj.userEventFuncArray.x;
    else
        userEventFuncArray = obj.userEventFuncArray;
    end
else
    userEventFuncArray = [];
end

if obj.hasIntegralCons
    if isa(PathFunc.intFunctions,'gradient');
        userIntegralFuncArray = obj.userIntegralFuncArray.x;
    else
        userIntegralFuncArray = obj.userIntegralFuncArray;
    end
else
    userIntegralFuncArray = [];
end

if obj.hasLinkageCons
    if isa(PathFunc.intFunctions,'gradient');
        linkageConVec = obj.linkageConVec.x;
    else
        linkageConVec = obj.linkageConVec;
    end
else
    linkageConVec = [];
end

if PathFunc.hasCostFunction
    if isa(obj.costFuncIntegral,'gradient');
        userCostFuncArray = obj.userCostFuncArray.x;
    else
        userCostFuncArray = obj.userCostFuncArray;
    end
else
    userCostFuncArray = [];
end

if PointFunc.hasCostFunction
    if isa(obj.costFuncIntegral,'gradient');
        costFuncAlgebraic = obj.costFuncAlgebraic.x;
    else
        costFuncAlgebraic = obj.costFuncAlgebraic;
    end
else
    costFuncAlgebraic = [];
end

FuncValues.userDynFuncArray = userDynFuncArray;
FuncValues.userAlgPathFuncArray = userAlgPathFuncArray;
FuncValues.userEventFuncArray = userEventFuncArray;
FuncValues.userIntegralFuncArray = userIntegralFuncArray;
FuncValues.linkageConVec = linkageConVec;
FuncValues.userCostFuncArray = userCostFuncArray;
FuncValues.costFuncAlgebraic = costFuncAlgebraic;

