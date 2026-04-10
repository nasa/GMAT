function [Grad, NumFuncEvals] = numdiff(func,x,fx,Options,Type,varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

lengthx = length(x);
lengthfx = length(fx);
NumFuncEvals = 0;
Grad = zeros(lengthx,lengthfx);

switch Type

    case 'Jacobian'

        switch Options.DerivativeMethod

            case 'ForwardDiff'

                %  Evaluate function at nominal x
                for i = 1:lengthfx

                    dx = zeros(lengthx,1);  dx(i,1) = Options.FiniteDiffVector(i);
                    [ci,ce] =  feval(func,x + dx,varargin{:});
                    NumFuncEvals = NumFuncEvals + 1;
                    Grad(:,i) = (fxplusdx - fx)/Options.FiniteDiffVector(i);

                end

            case 'CentralDiff'

                %  Evaluate function at nominal x
                for i = 1:lengthfx

                    dx = zeros(lengthx,1);  dx(i,1) = .5*Options.FiniteDiffVector(i);
                    fxplusdx =  feval(func,x + dx,varargin{:});
                    NumFuncEvals = NumFuncEvals + 1;
                    fxminusdx =  feval(func,x - dx,varargin{:});
                    NumFuncEvals = NumFuncEvals + 1;
                    Grad(:,i) = (fxplusdx - fxminusdx)/Options.FiniteDiffVector(i);

                end

            otherwise

                disp(['Warning: Unsupported Derivative Method ' Options.DerivativeMethod ' in numdiff.m']);
                Grad = [];
                NumFuncEvals = 0;
                return

        end

    case 'Gradient'


        switch Options.DerivativeMethod

            case 'ForwardDiff'

                %  Evaluate function at nominal x
                for i = 1:lengthx

                    dx = zeros(lengthx,1);  dx(i,1) = Options.FiniteDiffVector(i);
                    fxplusdx =  feval(func,x + dx,varargin{:});
                    NumFuncEvals = NumFuncEvals + 1;
                    Grad(i,1) = (fxplusdx - fx)/Options.FiniteDiffVector(i);

                end

            case 'CentralDiff'

                %  Evaluate function at nominal x
                for i = 1:lengthx

                    dx = zeros(lengthx,1);  dx(i,1) = .5*Options.FiniteDiffVector(i);
                    fxplusdx =  feval(func,x + dx,varargin{:});
                    NumFuncEvals = NumFuncEvals + 1;
                    fxminusdx =  feval(func,x - dx,varargin{:});
                    NumFuncEvals = NumFuncEvals + 1;
                    Grad(i,1) = (fxplusdx - fxminusdx)/Options.FiniteDiffVector(i);

                end

            otherwise

                disp(['Warning: Unsupported Derivative Method ' Options.DerivativeMethod ' in numdiff.m']);
                Grad = [];
                NumFuncEvals = 0;
                return

        end



end