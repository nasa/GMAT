classdef Spacecraft < handle
    % SPACECRAFT: A spacecraft model.

    properties (Access = private)
        % OrbitState oject.  The orbital state.
        orbitState
        % OrbitEpoch object.  The orbit epoch.
        orbitEpoch
    end
    
    methods
        function obj = Spacecraft(orbitEpoch,orbitState)
           % Class constructor
           obj.orbitState = orbitState;
           obj.orbitEpoch = orbitEpoch;
        end
        
        function orbitState = GetOrbitState(obj)
            % Returns the orbit state object
            orbitState = obj.orbitState;
        end
        
        function orbitEpoch = GetOrbitEpoch(obj)
            % Returns the orbit epoch object
            orbitEpoch = obj.orbitEpoch;
        end
        
        function jDate = GetJulianDate(obj)
            % Returns Julian date of spacecraft
           jDate = obj.orbitEpoch.GetJulianDate(); 
        end
        
        function cartState = GetCartesianState(obj)
            % Returns Cartesian state of spacecraft
           cartState = obj.orbitState.GetCartesianState(); 
        end
        
    end
    
end

