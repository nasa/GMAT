classdef OrbitScaleUtility < handle
    %OrbitScaleUtility  Utility for performing scaling of orbit data
    % using canonical units
    %
    % Author.  S. Hughes.  steven.p.hughes@nasa.gov
    % 8/12/19  First Version
    properties
        % The distance unit
        DU
        % The mass unit
        MU
        % The gravitational paramater
        gravParam
        % The time unit
        TU
        % The acceleration unit
        ACCU
        % The velocity unit
        VU
        % Mass flow unit
        MFU
    end
    
    methods
        
        function obj = OrbitScaleUtility(distUnit,massUnit,gravParam)
            % Class constructor
            
            % Set input values
            obj.DU  = distUnit;
            obj.MU  = massUnit;
            obj.gravParam = gravParam;
            
            % Compute quantities based on input values
            obj.TU  = sqrt(obj.DU^3/obj.gravParam);
            obj.ACCU = obj.DU/obj.TU^2;
            obj.VU = obj.DU/obj.TU;
            obj.MFU = obj.MU/obj.TU;

        end
                
        function timeUnit = GetTU(obj)
           % Returns the time unit
           timeUnit = obj.TU; 
        end
               
        function state = UnScaleModEqFullState(obj,state)
            % Unscales Mod. Equinoctials with Mass
            state(1) = state(1)*obj.DU;
            state(7) = state(7)*obj.MU;
        end
        
        function state = ScaleModEqElements(obj,state)
            % Scales Mod Equinoctial elements (no mass)
            state(1) = state(1)/obj.DU;
        end
        
        function state = UnScaleModEqElements(obj,state)
            % Scales Mod Equinoctial elements (no mass)
            state(1) = state(1)*obj.DU;
        end
        
        function scaledThrust = ScaleThrustMag(obj,availableThrustNewtons)
            % Scales thrust in Newtons using canonical scaling
            scaledThrust =  availableThrustNewtons/obj.ACCU/obj.MU;
        end
        
        function scaledMassFlow = ScaleMassFlowRate(obj,flowRate)
            % Scales mass flow rate
            scaledMassFlow = flowRate*obj.TU/obj.MU;
        end
        
        function t = ScaleTime(obj, t)
            % scale time
            t = t / obj.TU;
        end
        
        function t = UnscaleTime(obj, t)
            % unscale time
            t = t * obj.TU;
        end
        
        function a = ScaleAcc(obj, a)
            % scale acceleration
            a = a / obj.ACCU;
        end
        
        function a = UnscaleAcc(obj, a)
            % unscale acceleration
            a = a * obj.ACCU;
        end
        
    end
    
end

