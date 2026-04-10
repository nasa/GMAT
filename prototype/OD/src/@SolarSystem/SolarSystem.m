
classdef SolarSystem < handle

    %----------------------------------------------------------------------
    %  Define the object properties
    %----------------------------------------------------------------------
    
    %  Set the public data
    properties  (SetAccess = 'public')

        SpeedofLight = 299792.458;
        Mu
        Mercury
        Venus
        Earth
        Mars
        Jupiter
        Saturn
        Uranus
        Neptune
        Pluto
        Luna
        Sol
        
    end

    %----------------------------------------------------------------------
    %  Define the object's methods
    %----------------------------------------------------------------------
    
    methods

        %----- Intialize
        function SolSys = SolarSystem(SolSys)


             SolSys.Earth = Earth;
             SolSys.Mu = [22032.080486418
             324858.59882646
             398600.4415
             42828.314258067
             126712767.85780
            37940626.061137
            5794549.0070719
            6836534.0638793
            981.60088770700
            4902.8005821478
            132712440017.99];
            Mu(3) = SolSys.Earth.Mu;


        end % Initialize
        
        function SolSys = Initialize(SolSys)
            
        end

    end % methods

end % classdef

