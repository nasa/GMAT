classdef Propagator
    %J2Propagator. Propagates orbital elements using J2 only
    
    properties (Access = private)
        
        % *Spacecraft. The spacecraft to be propagated.
        Spacecraft
        
        % Double. The J2 term for Earth
        gravParamJ2 = 1.0826269e-003;
        % Double. The gravitational parameter of Earth
        gravParamMu = 3.986004415e+5;
        % Double. The equatorial radius of Earth
        bodyRadius = 6.3781363e+003;
        
        % Double. Julian date of the reference orbital elements
        refEpoch
        % AbsoluteDate. The epoch at which new state is requested.
        propEpoch
        
        % Double. The orbital semi-major axis
        orbitSMA
        % Double. The orbital eccentricity
        orbitECC
        % Double. The orbital inclination
        orbitINC
        % Double. The orbital right ascension of ascending node
        orbitRAAN
        % Double. The orbital argument of periapsis
        orbitAOP
        % Double. The orbital true anomaly
        orbitTA
        % Double. THe orbital mean anomaly
        orbitMA
        
        % Double. The drift in mean motion caused by J2.
        meanMotionRate
        % Double. The drift in argument of periapsis caused by J2.
        argPeriapsisRate
        % Double. The drift in right ascension of ascending node from J2
        rightAscensionNodeRate
        % Double. The orbital semilatus rectum.
        semiLatusRectum
        % Double. The orbital mean motion.
        meanMotion
        
    end
    
    methods (Access = public)
        
        function obj = Propagator(Sat)
            % Class constructor.  Requires a spacecraft.
            obj.Spacecraft = Sat;
            obj = obj.SetOrbitState(Sat.GetOrbitState());
            obj = obj.SetReferenceEpoch(Sat.GetOrbitEpoch);
            obj = ComputeOrbitRates(obj);
        end
        
        function obj = SetPhysicalConstants(obj,gravParamMu,gravParamJ2,bodyRadius)
            %  Set physical constants need to model orbits with J2
            obj.gravParamMu = gravParamMu;
            obj.gravParamJ2 = gravParamJ2;
            obj.bodyRadius = bodyRadius;
        end

        function cartState = Propagate(obj,date)
            % Propgate and return cartesian state given AbsoluteDate
            propDuration = (date.GetJulianDate - obj.refEpoch)*86400;
            orbElem = PropOrbElements(obj,propDuration);
            % Get the pointer to the orbit state
            ptrOrbitState = obj.Spacecraft.GetOrbitState();
            ptrOrbitState.SetKeplerianVectorState(orbElem);
            cartState = ptrOrbitState.GetCartesianState();
        end
        
    end
    
    methods (Access = private)
        
        function obj = SetReferenceEpoch(obj,orbitEpoch)
            % Sets the reference epoch for the initial orbital elements
            obj.refEpoch = orbitEpoch.GetJulianDate();
        end
        
        function obj = SetOrbitEpoch(obj,orbitEpoch)
            %  Set orbit epoch given AbsoluteDate object.
            obj.propEpoch = orbitEpoch.GetJulianDate();
        end
        
        function obj = SetOrbitState(obj,orbitState)
            %  Set orbit state given OrbitState object.
            
            %Done at intialization for performance reasons.
            kepElements = orbitState.GetKeplerianState();
            obj.orbitSMA = kepElements(1);
            obj.orbitECC = kepElements(2);
            obj.orbitINC = kepElements(3);
            obj.orbitRAAN = kepElements(4);
            obj.orbitAOP = kepElements(5);
            obj.orbitTA = kepElements(6);
            obj.orbitMA = obj.TrueToMeanAnomaly(obj.orbitTA,obj.orbitECC);
        end
        
        %  Propagate and return cartesian state
        function cartState = PropCartState(obj,propDuration)
            % Propgate and return cartesian state
            orbElem = PropOrbElements(obj,propDuration);
            cartState = obj.OrbElemToCartesian(orbElem);
        end
        
        function orbElements = PropOrbElements(obj,propDuration)
            %  Propagate and return orbital elements
            twopi = 2*pi;
            orbElements = zeros(6,1);
            orbElements(1) = obj.orbitSMA;
            orbElements(2) = obj.orbitECC;
            orbElements(3) = obj.orbitINC;
            orbElements(4) = mod(obj.orbitRAAN + obj.rightAscensionNodeRate*propDuration,twopi);
            orbElements(5) = mod(obj.orbitAOP + obj.argPeriapsisRate*propDuration,twopi);
            newMA = mod(obj.orbitMA + obj.meanMotionRate*propDuration ,twopi);
            orbElements(6) = obj.MeanToTrueAnomaly(newMA,obj.orbitECC);
        end
        
        function meanMotion = MeanMotion(obj)
            %  Computes the orbital mean motion
            meanMotion = sqrt(obj.gravParamMu/obj.orbitSMA^3);
        end
        
        function semiParameter = SemiParameter(obj)
            %  Computes the orbital semi parameter
            semiParameter = obj.orbitSMA*(1-obj.orbitECC*obj.orbitECC);
        end
        
        function nu = MeanToTrueAnomaly(~,M,e)
            % Computes true anomlaly given mean anomaly and eccentricity
            
            %  pick initial guess for E
            if ( -pi < M < 0 )
                E = M - e;
            else
                E = M + e;
            end
            
            %  Iterate until tolerance is met
            tol = 1e-8;
            diff = 1;
            
            while ( diff > tol )
                f = M - E + e*sin(E);
                fprime = 1 - e*cos(E);
                E_new = E + f/fprime;
                diff = abs(E_new - E);
                E = E_new;
            end
            
            cos_E = cos(E);
            sin_nu = sqrt(1 - e*e)*sin(E)/(1-e*cos_E);       %Vallado pg. 214,  Eq. 4-10
            cos_nu = (cos_E - e)/(1 - e*cos_E);              %Vallado pg. 214,  Eq. 4-12
            nu = atan2(sin_nu,cos_nu);
            
            if (nu < 0)
                nu = nu + 2*pi;
            end
            
        end
        
        function eccentricAnomaly = TrueToEccentricAnomaly(~,trueAnomaly,orbitEcc)
            % Computes ecentric anomaly given true anomaly and eccentricty
            
            cosnu = cos(trueAnomaly);
            %Vallado pg. 213,  Eq. 4-9
            sin_E = sqrt(1 - orbitEcc*orbitEcc)*sin(trueAnomaly)/...
                (1+orbitEcc*cosnu);
            cos_E = (orbitEcc+cosnu)/(1+orbitEcc*cosnu);
            eccentricAnomaly = atan2(sin_E,cos_E);
            
            %  make sure E > 0;
            while (eccentricAnomaly < 0)
                eccentricAnomaly = eccentricAnomaly + 2*pi;
            end
            
        end
        
        function meanAnomaly = E2M(~,eccentricAnomaly,orbitEcc)
            % Computes the mean anomaly given eccentric anomaly and eccentricity
            meanAnomaly = eccentricAnomaly - orbitEcc* sin(eccentricAnomaly);
        end
        
        function meanAnomaly = TrueToMeanAnomaly(obj,trueAnomaly,orbitECC)
            % Computes the true anomaly given mean anomaly and eccentricity
            eccentricAnomaly = obj.TrueToEccentricAnomaly(trueAnomaly,orbitECC);
            meanAnomaly = obj.E2M(eccentricAnomaly,orbitECC);
        end
                
        function obj = ComputeOrbitRates(obj)
            %  Compute orbit element rates
            obj = ComputeMeanMotionRate(obj);
            obj = ComputeArgPerRate(obj);
            obj = ComputeRAANRate(obj);
        end
        
        function obj = ComputeMeanMotionRate(obj)
            %  Computes the orbital mean motion Rate
            %  Vallado, 3rd. Ed.  Eq9-41
            n = obj.MeanMotion();
            J2 = obj.gravParamJ2;
            e = obj.orbitECC;
            p = obj.SemiParameter();
            obj.meanMotionRate = n - 0.75*n*J2*(obj.bodyRadius/p)^2*sqrt(1-e^2)*(3*sin(obj.orbitINC)^2 - 2);
        end
        
        function obj = ComputeArgPerRate(obj)
            %  Computes the argument of periapsis rate
            %  Vallado, 3rd. Ed.  Eq9-39
            n = obj.MeanMotion();
            obj.argPeriapsisRate = 3*n*obj.bodyRadius^2*obj.gravParamJ2/...
                4/obj.SemiParameter()^2*(4 - 5*sin(obj.orbitINC)^2);
        end
        
        function obj = ComputeRAANRate(obj)
            %  Computes rate right ascension of node rate
            %  Vallado, 3rd. Ed.  Eq9-37
            n = obj.MeanMotion();
            obj.rightAscensionNodeRate = -3*n*obj.bodyRadius^2*obj.gravParamJ2/...
                2/obj.SemiParameter()^2*cos(obj.orbitINC);
        end
        
    end
    
end

