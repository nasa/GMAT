classdef Spacecraft < handle

    %----------------------------------------------------------------------
    %  Define the object properties
    %----------------------------------------------------------------------

    %-----  Set the public data
    properties  (SetAccess = 'public')
        Epoch    = 21545;
        Id       = 21639;
        X  = 7100;
        Y  = 0.0;
        Z  = 1300;
        VX = 0.1;
        VY = 7.35;
        VZ = 1.0;
        Cd = 2.0;
        Cr = 1.7;
        DragArea = 15;
        SRPArea  = 1.0;
        DryMass  = 850;
        OrbitCovariance = diag([100000*ones(3,1);1000*ones(3,1)]);
    end

    %-----  Set the public data
    properties  (SetAccess = 'protected')
        STM = eye(6,6);
    end

    %----------------------------------------------------------------------
    %  Define the object's methods
    %----------------------------------------------------------------------

    methods

        %-----  The constructor
        function Sat = Spacecraft(obj)

            if nargin == 0
                STM = eye(6,6);
                %  This is the copy constructor
            else
                fns = fieldnames(obj);
                for i = 1:length(fns)
                    Sat.(fns{i}) = obj.(fns{i});
                end
            end

        end % Spacecraft

        %----- Initialize
        function obj = Initialize(obj,Sandbox)
            obj.STM = eye(6);
        end % Initialize

        %-----  GetState
        function x = GetState(Sat,Id)

            if Id == 201;
                x = [Sat.X Sat.Y Sat.Z Sat.VX Sat.VY Sat.VZ]';
            elseif Id == 202;
                x = reshape(Sat.STM,36,1);
            elseif Id == 203
                x = Sat.Cd;
            elseif Id == 204
                x = Sat.Cr;
            elseif Id == 205
                x = Sat.Epoch;
            else
                disp(['State Id ' num2str(x) ' is not a supported set state in Spacecraft::GetState']);
            end

        end % GetState

        %----- SetState
        function Sat = SetState(Sat,Id,x)

            if Id == 201;
                Sat.X  = x(1,1);
                Sat.Y  = x(2,1);
                Sat.Z  = x(3,1);
                Sat.VX = x(4,1);
                Sat.VY = x(5,1);
                Sat.VZ = x(6,1);
            elseif Id == 202;
                Sat.STM = reshape(x,6,6)';
            elseif Id == 203
                Sat.Cd = x(1,1);
            elseif Id == 204
                Sat.Cr = x(1,1);
            elseif Id == 205
                Sat.Epoch = x(1,1);
            else
                disp(['State Id ' num2str(x) ' is not a supported set state in Spacecraft::SetState']);
            end

        end % SetState

        function Id = GetParamId(Sat,name);

            switch name
                case 'CartesianState'
                    Id = 201;
                case 'STM'
                    Id = 202;
                case 'Cr'
                    Id = 203;
                case 'Cd'
                    Id = 204;
                case 'Epoch'
                    Id = 205;
                otherwise
                    Id = '';
            end

        end % GetParamId
        
        %----- GetSTM
        function STM = GetSTM(Sat,paramId);

            switch paramId
                case 201
                    STM = Sat.STM;
                otherwise
                    STM = [];
            end

        end % GetSTM
        
        %----- Get id for dynamics model
        function Id = GetDynamicsId(Sat,paramId)
            
            switch paramId
                case 201
                    Id = 101;
                case 202
                    Id = 102;
                otherwise
                    Id = [];
            end
            
        end
        
        %----- Resest STM
        function Sat = ResetSTM(Sat)
            Sat.STM = eye(6);
        end % Resest STM

    end % method

end % classdef
