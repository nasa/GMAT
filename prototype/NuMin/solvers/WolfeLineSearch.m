function [alpha, phi_alpha, G_alpha, NumFuncEvals] = WolfeLineSearch(func, x, phi_0, phiprime_0, p, alphaguess, Options, varargin)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

%  Define constants

%  Note:  0 < c1 < c2 < 1
%  Define c1, which is used in the sufficient decrease test
c1  = .0001;                        % the value is suggested by N&W pg. 200

%  Define c2, which is used in the curvature condition test
c2  = 0.9;                          % the value is suggested by N&W pg. 200

%  Define the factor to increase the step size if the initial guess does
%  satisfy either the sufficient decrease or curvature condition.
fac = 9.0;

%  Define the maximum iterations for the line search algorithm.
MaxIter = 25;

%  Initialize variables
alpha = alphaguess;
NumFuncEvals = 0;

%  Loop to try and find a bracket for alpha
%  This loop is Algorithm 3.2 from Nocedal and Wright.  The algorithm has
%  been slightly modified, however the modification will not effect the
%  resulting solution.  The change is simply that the gradient is evaluated
%  along with the function, at the beginning of each pass through the loop.
%  This simplifies bookeeping a little, as the gradient value will be
%  needed in the Zoom algorithm if
i = 0;
phi_prev      = phi_0;
phiprime_prev = phiprime_0;
alpha_prev    = 0;
while i <= MaxIter

    %  Increment the loop counter
    i = i + 1;


    %  Evaluate the function, gradient, and directional derivative
    %  at the current step size
    switch Options.DerivativeMethod

        case {'ForwardDiff','BackwardDiff','CentralDiff'}

            phi_alpha           = feval(func,x + alpha*p,varargin{:});
            [G_alpha, NumGFuncEvals] = numdiff(func,x + alpha*p,phi_alpha,Options,'Gradient',varargin{:});
            phiprime_alpha     = G_alpha'*p;
            NumFuncEvals       = NumFuncEvals + NumGFuncEvals + 1;

        case 'Analytic'

            [phi_alpha,G_alpha]  = feval(func,x + alpha*p,varargin{:});
            phiprime_alpha       = G_alpha'*p;
            NumFuncEvals         = NumFuncEvals +  1;

        otherwise

            disp('DerivativeMethod supplied in Options.DerivativeMethod is not supported')

    end

    %  If we violate the significant decrease condition, or if the function value
    %  starts to increase, stop and perform the sectioning algorithm
    if ( phi_alpha > phi_0 + c1*alpha*phiprime_0 ) || ( phi_alpha >= phi_prev && i > 1 )
        [alpha,phi_alpha, G_alpha, NumFuncEvals] = Zoom(func, x, p, phi_0, phiprime_0, alpha_prev, alpha,...
            phi_prev, phiprime_prev, phi_alpha, phiprime_alpha,Options);

        return
    end

    %  Evaluate the curvature condition.  If this condition is satisfied,
    %  then the step is acceptable and there is no need to perform the zoom
    %  algorithm!!
    if abs( phiprime_alpha ) <= -c2*phiprime_0
        return
    end

    %  Check to make sure that point at alpha still provides decrease
    if phiprime_alpha >= 0

        [alpha,phi_alpha, G_alpha, NumFuncEvals] = Zoom(func, x, p, phi_0, phiprime_0, alpha, alpha_prev,...
            phi_alpha, phiprime_alpha, phi_prev, phiprime_prev, Options);

        return
    end

    %  If we get to this point in the loop, the current value of alpha is
    %  not accpetable.  So, scale alpha and try a larger value.
    phi_prev      = phi_alpha;
    phiprime_prev = phiprime_alpha;
    alpha_prev    = alpha;
    alpha         = min(fac*alpha,Options.MaxStepLength);

    %  Check to see if we've reached upper bounds on alpha and return if we
    %  have.
    if alpha == Options.MaxStepLength
        return
    end

end


%==========================================================================
function [alpha, phi_alpha, G_alpha, NumFuncEvals] = Zoom(func, x, p, phi_0, phiprime_0, alphaL, alphaH, ...
    phi_alphaL, phiprime_alphaL, phi_alphaH, phiprime_alphaH, Options, varargin)

	%  Algorithm 3.3 of N&W 1st Edition.
	
if phi_alphaH < phi_alphaL
    stop
end

c1 = .0001;
c2 = .9;
NumFuncEvals = 0;
j = 0;
MaxIter = 40;
alpha = zeros(MaxIter,1);

while j <= MaxIter + 1;

    %  Loop updates
    j = j + 1;

    % Step 2:  Choose Alpha(j) from the set below.  For now use the mean.
    alpha =  SafeCubicInterp(alphaL,alphaH,phi_0,phiprime_0, phi_alphaL, phi_alphaH, phiprime_alphaL, phiprime_alphaH );
    tau2 = min(0.1,c2); tau3 = 0.5;
    brcktEndpntA = alphaL + tau2*(alphaH - alphaL);
    brcktEndpntB = alphaH - tau3*(alphaH - alphaL);
    if alpha < min(brcktEndpntA,brcktEndpntB)
        alpha = min(brcktEndpntA,brcktEndpntB);
    elseif alpha > max(brcktEndpntA,brcktEndpntB);
        alpha = max(brcktEndpntA,brcktEndpntB);
    end


    % Step 3:  Evaluate the line search function at alpha(j)
    %phi_alpha = feval(func,x + alpha*p);
    %[G_alpha,NumGFuncEvals] = numdiff(func,x + alpha*p,phi_alpha,Options,'Gradient', varargin{:});
    switch Options.DerivativeMethod

        case {'ForwardDiff','BackwardDiff','CentralDiff'}

            phi_alpha                  = feval(func,x + alpha*p,varargin{:});
            [G_alpha, NumGFuncEvals] = numdiff(func,x + alpha*p,phi_alpha,Options,'Gradient',varargin{:});
            NumFuncEvals = NumFuncEvals + NumGFuncEvals + 1;

        case 'Analytic'

            [phi_alpha,G_alpha]              = feval(func,x + alpha*p,varargin{:});
            NumFuncEvals = NumFuncEvals +  1;

        otherwise

            disp('DerivativeMethod supplied in Options.DerivativeMethod is not supported')

    end
    

    % Step 4:  Check if the selection of alpha is a good one
    if ( phi_alpha > phi_0 + c1*alpha*phiprime_0 ) ||  ( phi_alpha > phi_alphaL   )

        alphaH = alpha;

    else

        phiprime_alpha = G_alpha'*p;
        if abs(phiprime_alpha) <= -c2*phiprime_0;
            return
        end

        if ( phiprime_alpha*( alphaH - alphaL ) >= 0 )
            alphaH = alphaL;
        end

        alphaL = alpha;

    end

end

%==========================================================================
function [alpha] = SafeCubicInterp(alphaL, alphaH, phi_0, phiprime_0, phi_alphaL, phi_alphaH, phiprime_alphaL, phiprime_alphaH)

d1 = phiprime_alphaL + phiprime_alphaH - 3*(phi_alphaL - alphaH)/(alphaL - alphaH);
d2 = sqrt(d1^2 - phiprime_alphaL*phiprime_alphaH);
alpha = alphaH - (alphaH - alphaL)*((phiprime_alphaH + d2 - d1)/...
    (phiprime_alphaH - phiprime_alphaL + 2*d2));


