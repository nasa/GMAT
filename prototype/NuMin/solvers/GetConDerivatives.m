function [ci,ce,Ji,Je,numfEval] = GetConDerivatives(func,x,Options,varargin)

%   Copyright 2002-2005, United States Government as represented by the
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

numfEval = 0;
lengthx = length(x);

switch Options.DerivativeMethod
    
    case {'ForwardDiff','BackwardDiff','CentralDiff'}
        [ci,ce]            = feval(func,x,varargin{:});
        numfEval = numfEval +1;
        numEqCons = length(ce);
        numIneqCons = length(ci);
        for i = 1:lengthx
            dx = zeros(lengthx,1);
            dx(i,1) = Options.FiniteDiffVector(i);
            [cixplusdx,cexplusdx] = feval(func,x+dx,varargin{:});
            numfEval = numfEval +1;
            %            NumFuncEvals = NumFuncEvals + 1;
            if numIneqCons > 0
                Ji(:,i) = (cixplusdx - ci)/Options.FiniteDiffVector(i);
            else
                Ji = [];
            end
            if numEqCons > 0
                Je(:,i) = (cexplusdx - ce)/Options.FiniteDiffVector(i);
            else
                Je = [];
            end
        end
        Je = Je';
        Ji = Ji';
    case 'Analytic'
        
        if ischar(func) || isa(func,'function_handle')
            [ci,ce,Ji,Je] = feval(func,x,varargin{:});
            numfEval = numfEval + 1;
        else
            [ce,ci] = func.EvaluateConstraints(x);
            [Je,Ji] = func.EvaluateConstraintJacobian(x);
            numfEval = numfEval + 1;
        end
        
    otherwise
        
        disp('DerivativeMethod supplied in Options.DerivativeMethod is not supported')
        
end