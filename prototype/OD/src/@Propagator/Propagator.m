classdef Propagator < handle

    % Set the public properties
    properties  (SetAccess = 'public')
        CentralBody        = 'Earth';
        PointMasses        = {'Earth'};
        SRP                = 'Off';
        Type               = 'RungeKutta89';
        InitialStepSize    = 60;
        Accuracy           = 1.0e-12;
        MinStep            = 0.001;
        MaxStep            = 2700;
        MaxStepAttempts    = 50;
        FM
    end

    % Set the public properties
    properties  (SetAccess = 'public')
        ODEMap    = [];
        PSV       = [];
        PSM
        NumStates = 0;
        Epoch
        ODEmodel
    end

    % Set the methods
    methods

        %----- Constructor
        function Prop = Propagator(Prop)
            Prop.PSM = PropStateManager();
        end

        %-----  Initialize
        function Prop = Initialize(Prop,Sandbox)

            odeModel = ODEmodel;
            Prop.ODEmodel = odeModel.Initialize(Sandbox);

            if strcmp(Prop.FM.CentralBody,'Mercury')
                Prop.FM.CentralBodyIndex = 1;
            end

            if strcmp(Prop.FM.CentralBody,'Venus')
                Prop.FM.CentralBodyIndex = 2;
            end

            if strcmp(Prop.FM.CentralBody,'Earth')
                Prop.FM.CentralBodyIndex = 3;
            end

            if strcmp(Prop.FM.CentralBody,'Mars')
                Prop.FM.CentralBodyIndex = 4;
            end

            if strcmp(Prop.FM.CentralBody,'Jupiter')
                Prop.FM.CentralBodyIndex = 5;
            end

            if strcmp(Prop.FM.CentralBody,'Saturn')
                Prop.FM.CentralBodyIndex = 6;
            end

            if strcmp(Prop.FM.CentralBody,'Uranus')
                Prop.FM.CentralBodyIndex = 7;
            end

            if strcmp(Prop.FM.CentralBody,'Neptune')
                Prop.FM.CentralBodyIndex = 8;
            end

            if strcmp(Prop.FM.CentralBody,'Pluto')
                Prop.FM.CentralBodyIndex = 9;
            end

            if strcmp(Prop.FM.CentralBody,'Luna')
                Prop.FM.CentralBodyIndex = 10;
            end

            if strcmp(Prop.FM.CentralBody,'Sun')
                Prop.FM.CentralBodyIndex = 11;
            end

            %--------------------------------------------------------------------------
            %  Set DE file indeces for point mass bodies
            %--------------------------------------------------------------------------
            j = 0;
            for i = 1:size(Prop.FM.PointMasses,2);

                if strcmp(Prop.FM.PointMasses{i},'Mercury')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 1;
                end

                if strcmp(Prop.FM.PointMasses{i},'Venus')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 2;
                end

                if strcmp(Prop.FM.PointMasses{i},'Earth')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 3;
                end

                if strcmp(Prop.FM.PointMasses{i},'Mars')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 4;
                end

                if strcmp(Prop.FM.PointMasses{i},'Jupiter')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 5;
                end

                if strcmp(Prop.FM.PointMasses{i},'Saturn')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 6;
                end

                if strcmp(Prop.FM.PointMasses{i},'Uranus')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 7;
                end

                if strcmp(Prop.FM.PointMasses{i},'Neptune')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 8;
                end

                if strcmp(Prop.FM.PointMasses{i},'Pluto')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 9;
                end

                if strcmp(Prop.FM.PointMasses{i},'Luna')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 10;
                end

                if strcmp(Prop.FM.PointMasses{i},'Sun')
                    j = j + 1;
                    Prop.FM.PointMassIndeces{j} = 11;
                end

            end

        end

        function  Prop = SteptoEpoch(Prop,PropEpoch)

            %==============================================================
            %==============================================================
            %---- Step to the requested epoch
            %==============================================================
            %==============================================================

            %  Perform the Propagation
            tof = (PropEpoch - Prop.Epoch)*86400;
            if tof > 0

                %  Call the integrator
                [t,X] = ODE78('Propagator_ODEmodel', 0,  ...
                    (PropEpoch - Prop.Epoch)*86400, Prop.PSV, Prop.Accuracy , [], [], Prop);

                %  Extract state from ephemeris output and update the propagator
                numSteps   = size(t,1);
                X          = X(numSteps,:)';
                Prop.Epoch = Prop.Epoch + t(numSteps)/86400;
                Prop.PSV   = X;

                %---- Populate the propagated objects with the new state data
                Prop.PSM.SetStates(X,Prop.Epoch);

            end

        end % SteptoEpoch

        function  Prop = Step(Prop)

            %==============================================================
            %==============================================================
            %---- Take one step
            %==============================================================
            %==============================================================

            %  Call the integrator
            [t,X] = ODE78('Propagator_ODEmodel', 0,  ...
                    10000, Prop.PSV, Prop.Accuracy , [], [], Prop,1);
            X(1,1:6)'

            %  Extract state from ephemeris output and update the propagator
            numSteps   = size(t,1);
            X          = X(numSteps,:)';
            Prop.Epoch = Prop.Epoch + t(numSteps)/86400;
            Prop.PSV   = X;

            %---- Populate the propagated objects with the new state data
            Prop.PSM.SetStates(X,Prop.Epoch);

        end % SteptoEpoch

    end  % methods
end % classdef