classdef ModEqDynamics < handle
    %UNTITLED2 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % Double. Central body gravitational parameter (i.e Mu)
        gravParam
        % Real. Absolute integration tolerance
        absTol = 1e-8
        % Real. Relative integration tolerance
        relTol = 1e-8
        % CostateControl object.  Utility for computing control using
        % costate parameterization
        controlUtil
    end
    
    methods
        
        function obj = ModEqDynamics(gravParam)
            % Constructor
            obj.gravParam = gravParam;
            %obj.controlUtil = CostateControl(gravParam);
        end
        
        function [odeRate] = ComputeOrbitDerivatives(obj,orbitElements,pertForces)
            %  Computes derivatives of the modified equinoctial elements
            %  with perturbing forces
            
            % Extract elements
            p = orbitElements(1);
            f = orbitElements(2);
            g = orbitElements(3);
            h = orbitElements(4);
            k = orbitElements(5);
            L = orbitElements(6);
            
            %  Compute repeated terms.   See Betts near Eq. 6.37
            sqrtPbyMu = sqrt(p/obj.gravParam);
            sinL = sin(L);
            cosL = cos(L);
            hsinL = h*sinL;
            kcosL = k*cosL;
            q = 1 + f*cosL +g*sinL;
            chi = sqrt(h*h + k*k);
            sSquared = 1 + chi*chi;
            fac1 = sqrtPbyMu*sSquared/2/q;
            
            %  Compute the A matrix.  Eq 6.35 of Betts
            A = zeros(6,3);
            A(1,2) = 2*p/q*sqrtPbyMu;
            A(2,1) = sqrtPbyMu*sinL;
            A(2,2) = sqrtPbyMu/q*((q+1)*cosL +f);
            A(3,2) = sqrtPbyMu/q*((q+1)*sinL +g);
            A(3,1) = -sqrtPbyMu*cosL;
            A(2,3) = -sqrtPbyMu*g/q*(hsinL - kcosL);
            A(3,3) = sqrtPbyMu*f/q*(hsinL - kcosL);
            A(4,3) = fac1*cosL;
            A(5,3) = fac1*sinL;
            A(6,3) = sqrtPbyMu/q*(hsinL - kcosL);
            
            b = zeros(6,1);
            b(6,1) = sqrt(obj.gravParam*p)*(q*q)/(p*p);
            
            %  Compute the derivatives.  Eq. 6.31 of Betts
            odeRate = A*pertForces + b;
            
        end
        
        function stateJac = GetOrbitStateJac(obj,orbitElements,pertForces)
            
            % Extract elements
            p = orbitElements(1);
            f = orbitElements(2);
            g = orbitElements(3);
            h = orbitElements(4);
            k = orbitElements(5);
            L = orbitElements(6);
            u1 = pertForces(1);
            u2 = pertForces(2);
            u3 = pertForces(3);
            mu = obj.gravParam;
            
            % Define terms that get re-used and compute upfront.
            % Note, further simplication is possible.
            sinL = sin(L);
            cosL = cos(L);
            fac4 = (p/mu)^(1/2);
            fac1 = (f*cosL + g*sinL + 1);
            fac2 = 4*mu*fac4;
            fac3 = (mu*p)^(1/2);
            fac5 = (h^2 + k^2 + 1);
            fac6 = k*cos(L) - h*sin(L);
            u3sinL = u3*sinL;
            u3cosL = u3*cosL;
            u1cosL = u1*cosL;
            u1sinL = u1*sinL;
            u2cosL = u2*cosL;
            u2sinL = u2*sinL;
            u1sinLSquared = u1sinL*sinL;
            u1cosLSquared = u1cosL*cosL;
            u2sinLSquared = u2sinL*sinL;
            u2cosLSquared = u2cosL*cosL;
            u3cosLSquared = u3cosL*cosL;
            u3sinLSquared = u3sinL*sinL;
            fac1Squared = fac1*fac1;
            gcosLMinusfsinL = g*cosL - f*sinL;
            
            J_p = [
                (3*p*u2)/(mu*fac4*fac1)
                (2*f*u2 + 4*u2cosL + 2*u1sinL + 2*f*u2cosLSquared + f*u1*sin(2*L) + g*u2*sin(2*L) + 2*g*u1sinLSquared + 2*g*k*u3cosL - 2*g*h*u3sinL)/(fac2*fac1)
                (2*g*u2 - 2*u1cosL + 4*u2sinL - 2*f*u1cosLSquared + f*u2*sin(2*L) - g*u1*sin(2*L) + 2*g*u2sinLSquared - 2*f*k*u3cosL + 2*f*h*u3sinL)/(fac2*fac1)
                (u3cosL*fac5)/(fac2*fac1)
                (u3sinL*fac5)/(fac2*fac1)
                (mu*fac1Squared)/(2*p^2*fac3) - (2*fac3*fac1Squared)/p^3 - (u3*(fac6))/(2*mu*fac4*fac1)
                ];
            
            J_f = [
                -(2*p*u2*cosL*fac4)/fac1Squared
                (fac4*(u2 - u2cosLSquared + g*u2*sinL - g*k*u3cosLSquared + g*h*u3cosL*sinL))/fac1Squared
                -(fac4*((u2*sin(2*L))/2 + g*u2*cosL + k*u3cosL - h*u3sinL - g*h*u3sinLSquared + (g*k*u3*sin(2*L))/2))/fac1Squared
                -(u3cosLSquared*fac4*fac5)/(2*fac1Squared)
                -(u3cosL*sinL*fac4*fac5)/(2*fac1Squared)
                (2*cosL*fac3*fac1)/p^2 + (u3cosL*(fac6)*fac4)/fac1Squared
                ];
            
            J_g = [
                -(2*p*u2*sinL*fac4)/fac1Squared
                -(fac4*((u2*sin(2*L))/2 - k*u3cosL + f*u2*sinL + h*u3sinL - f*k*u3cosLSquared + (f*h*u3*sin(2*L))/2))/fac1Squared
                (fac4*(u2 - u2sinLSquared + f*u2*cosL - f*h*u3sinLSquared + f*k*u3cosL*sinL))/fac1Squared
                -(u3cosL*sinL*fac4*fac5)/(2*fac1Squared)
                -(u3sinLSquared*fac4*fac5)/(2*fac1Squared)
                (2*sinL*fac3*fac1)/p^2 + (u3sinL*(fac6)*fac4)/fac1Squared
                ];
            
            J_h = [  0
                -(g*u3sinL*fac4)/fac1
                (f*u3sinL*fac4)/fac1
                (h*u3cosL*fac4)/fac1
                (h*u3sinL*fac4)/fac1
                (u3sinL*fac4)/fac1];
            
            J_k = [
                0
                (g*u3cosL*fac4)/fac1
                -(f*u3cosL*fac4)/fac1
                (k*u3cosL*fac4)/fac1
                (k*u3sinL*fac4)/fac1
                -(u3cosL*fac4)/fac1];
            
            J_L = [
                -(2*p*u2*(gcosLMinusfsinL)*fac4)/fac1Squared
                -(fac4*(3*g*u2 - u1cosL + 2*u2sinL + g^2*k*u3 - 2*f*u1cosLSquared - g^2*u1cosL - 2*g*u2cosLSquared + f*u2*sin(2*L) - g*u1*sin(2*L) - f^2*u1*cosL^3 + g^2*u1*cosL^3 - f^2*u2*sinL^3 + g^2*u2*sinL^3 + f*g*h*u3 + 3*f*g*u2*cosL + g*h*u3cosL - 2*f*g*u1*sinL + g*k*u3sinL - 2*f*g*u2*cosL^3 + 2*f*g*u1*sinL^3))/fac1Squared
                (fac4*(f*u2 + 2*g*u1 + 2*u2cosL + u1sinL + f^2*h*u3 + 2*f*u2cosLSquared - 2*g*u1cosLSquared + f*u1*sin(2*L) + f^2*u1*sinL + g*u2*sin(2*L) + f^2*u2*cosL^3 - g^2*u2*cosL^3 - f^2*u1*sinL^3 + g^2*u1*sinL^3 + f*g*k*u3 + 2*f*g*u1*cosL + f*h*u3cosL + 3*f*g*u2*sinL + f*k*u3sinL - 2*f*g*u1*cosL^3 - 2*f*g*u2*sinL^3))/fac1Squared
                -(u3*(g + sinL)*fac4*fac5)/(2*fac1Squared)
                (u3*(f + cosL)*fac4*fac5)/(2*fac1Squared)
                (2*fac3*(gcosLMinusfsinL)*fac1)/p^2 + (u3*(h*cosL + k*sinL)*fac4)/fac1 + (u3*(gcosLMinusfsinL)*(fac6)*fac4)/fac1Squared
                ];
            
            stateJac = [J_p J_f J_g J_h J_k J_L];
        end
        
        function meeFinal = PropagateNoControl(obj,meeInitial,tof)
            % Propagates given MEE set for given time of fligh
            odeOpts = odeset('AbsTol',obj.absTol,'RelTol',obj.relTol);
            [~,Xmee] = ode113(@obj.NoControlDynWrapper,[0 tof],meeInitial,odeOpts);
            meeFinal = Xmee(end,:);
        end
        
        function meeFinal = PropagateNoControlPeriod(obj,meeInitial)
            % Propagates given MEE set for given time of fligh
            odeOpts = odeset('AbsTol',obj.absTol,'RelTol',obj.relTol);
            tof = obj.MEEToPeriod(meeInitial);
            [~,Xmee] = ode113(@obj.NoControlDynWrapper,[0 tof],meeInitial,odeOpts);
            meeFinal = Xmee(end,:);
        end
        
        function [odeRHS] = NoControlDynWrapper(obj,t,X)
            % Wrapper for derivatives that assumes there is noperturbations
            pertForces = zeros(3,1);%R*pertForces;
            odeRHS = obj.ComputeOrbitDerivatives(X,pertForces);
        end
        
        function [t,stateHist] = PropagateWithControlPeriod(obj,initState,coStates)
            % Propagates given MEE set for given time of fligh
            odeOpts = odeset('AbsTol',obj.absTol,'RelTol',obj.relTol);
            tof = obj.MEEToPeriod(initState(1:6,1));
            [t,stateHist] = ode113(@obj.WithControlDynWrapper,[0 tof],initState,odeOpts,coStates);
        end
        
        function [t,stateHist] = PropagateWithControl(obj,initState,tof,coStates)
            % Propagates given MEE set for given time of fligh
            odeOpts = odeset('AbsTol',obj.absTol,'RelTol',obj.relTol);
            [t,stateHist] = ode113(@obj.WithControlDynWrapper,[0 tof],initState,odeOpts,coStates);
        end
        
        function [meeAverageRates] = ComputeAveragedMEERates(obj,initState,coStates)
            [t,stateHist] = obj.PropagateWithControlPeriod(initState,coStates);
            meeAverageRates = (stateHist(end,1:6) - stateHist(1,1:6))/(t(end) - t(1));
        end
        
        function [odeRHS] = WithControlDynWrapper(obj,t,X,coStates)
            % Wrapper for derivatives that assumes there is noperturbations
            cartState = Mee2Cart(X(1:6,1),obj.gravParam);
            kepElem = Cart2Kep(cartState,obj.gravParam);
            pertForces = obj.controlUtil.GetKepCostateControlInRSW(kepElem,coStates)/X(7);
            orbitDerivs = obj.ComputeOrbitDerivatives(X,pertForces);
            massFlowRate = obj.controlUtil.ComputeMassFlowRate();
            revDot = 1/(2*pi)/sqrt(kepElem(1)^3/obj.gravParam);
            odeRHS = [orbitDerivs;massFlowRate;revDot];
        end
        
        function [period] = MEEToPeriod(obj,elems)
            % Computes period given equinoctial elements
            cart = Mee2Cart(elems,obj.gravParam);
            oe = Cart2Kep(cart,obj.gravParam);
            period = 2*pi/sqrt(obj.gravParam/oe(1)^3);
        end
        
    end
    
end

