function BarycentricLagrangeTestDriver()

%%  Define test case. Independent variables and dependent variables.
indVar = [.1 .12 .13 .14 .15 .16 .17 .18 .23 .33 .43 .53]';
funcName = 'exp';  %warning: derivative test hard-coded for sin.
depVar = TheFunction(funcName,indVar,1);
depVar2 = TheFunction('sin',indVar,1);
testVar = .125455 ;

%% Create the interpolator, set independent variables, and interpolate
theInterp = BarycentricLagrangeInterpolator;
theInterp.SetIndependentVariables(indVar);
interpValue = theInterp.Interpolate(testVar,depVar);
exactValue = feval(funcName,testVar);
percentDiff = (interpValue - exactValue)/exactValue;
disp(['Interpolated Value : ' num2str(interpValue)]);
disp(['Exact Value        : ' num2str(exactValue)]);
disp(['PercentDiff        : ' num2str(percentDiff)]);

%%  Test the derivative
diffApprox = theInterp.Differentiate(depVar);
diffExact  = TheFunction(funcName,indVar,2);
disp('Derivative Approximation Error At Provided Points')
disp(diffApprox - diffExact)

%% Test multi-dimensional interpolation

multiDepVar=[depVar depVar depVar];
multiTestVar=[testVar;2*testVar;3*testVar]
exactValue=feval(funcName,multiTestVar);
multiExactValue=[exactValue exactValue exactValue];
multiInterpValue=theInterp.Interpolate(multiTestVar,multiDepVar);
multiPercentDiff = (multiInterpValue - multiExactValue)./multiExactValue;
for i=1:3
    disp(['Multiple-Interpolated Value : ' num2str(multiInterpValue(i,:))]);
    disp(['Multiple-Exact Value        : ' num2str(multiExactValue(i,:))]);
    disp(['Multiple-PercentDiff        : ' num2str(multiPercentDiff(i,:))]);
end


function [outVar] = TheFunction(funcName,indVar,funcFlag)

%  Sin function
if strcmp(funcName,'sin')
    if funcFlag == 1
    outVar = feval(funcName,indVar);
    else
        outVar = feval('cos',indVar);
    end 
end
    
%  Sin function
if strcmp(funcName,'exp')
    if funcFlag == 1
    outVar = feval(funcName,indVar);
    else
        outVar = feval('exp',indVar);
    end 
end

%  Fourth order polynomial
for i = 1:length(indVar)
    
end


