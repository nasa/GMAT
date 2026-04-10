classdef ODEmodel < handle

    %----------------------------------------------------------------------
    %  Define the object properties
    %----------------------------------------------------------------------

    %-----  Set the public data
    properties  (SetAccess = 'protected')
        SolarSystem
    end

    %----------------------------------------------------------------------
    %  Define the object's methods
    %----------------------------------------------------------------------
    methods

        %----- Initialize
        function obj = Initialize(obj,SandBox)

            %  Give ODEmodel handle for SolarSystem
            obj.SolarSystem = SandBox.SolarSystem;

        end % Initialize

        %----- GetDerivative
        function [xdot] = GetDerivative(ODEmodel, t,x,propagator)


            %  Loop over all elements in the state vector and call the appropriate ODE
            %  model for each elements.  The ODEmap constains the data that describes
            %  what is contained in the state vector.
            %
            %  201 :  Spacecraft cartesian state
            %  202 :  Spacecraft STM
            %  203 :  Spacecraft Cr
            %  204 :  Spacecraft Cd
            %
            ODEMap          = propagator.PSM.ODEMap;
            NumPropElements = size(ODEMap.Size,2);
            NumStates       = size(x,1);

            i = 1;
            xCounter = 1;
            while i < NumPropElements

                % If element is the cartesian state with or w/o STM

                if ODEMap.Type(i) == 201;

                    %  Determine if STM is being propagated
                    PropSTM = 0;
                    sizeCurrElement = 6;
                    if i+1 <= NumPropElements
                        if ODEMap.Type(i+1) == 202
                            PropSTM = 1;
                            sizeCurrElement = 42;
                        end
                    end

                    %  Call the ODEmodel for orbit cartesian state
                    xCurrElement = x(xCounter:xCounter+sizeCurrElement-1);
                    xdot = ODEmodel.CartOrbit(t,xCurrElement,PropSTM,propagator);

                    i = i + 2;
                    xCounter = xCounter + sizeCurrElement;

                end


            end

        end % GetDerivative

        function [xdot] = CartOrbit(ODEmodel,t,x,PropSTM,propagator)

            %  Determine if the STM is being propagated
            xdot = zeros(6,1);
            if PropSTM == 1;
                A    = zeros(6,6);
                phi  = reshape(x(7:42),6,6)';
            end

            %  Call point mass model
            [xdot_plus,A_plus] = ODEmodel.PointMasses(t,x,propagator,PropSTM);
            xdot = xdot + xdot_plus;
            A    = A + A_plus;

            switch propagator.FM.SRP

                case 'On'

                    [xdot_plus,A_plus] = deriv_SRP(t,x,propagator,PropSTM);
                    xdot = xdot + xdot_plus;
                    A    = A + A_plus;

            end

            if PropSTM == 1
                phidot   = A*phi;
                xdot  = [xdot;reshape(phidot',36,1)];
            end

        end % CartOrbit

        function [xdot,Amat] = PointMasses(ODEModel, t,x,propagator,PropSTM);

            %  Determine if the STM is being propagated
            if size(x,1) == 6
                PropSTM = 0;
            elseif size(x,1) == 42
                PropSTM = 1;
            end

            %  Calculate useful terms for later
            rv     = x(1:3,1);
            vv     = x(4:6,1);
            r      = sqrt(rv'*rv);
            r3     = r^3;
            r5     = r^5;
            eye3   = eye(3);
            zero3  = zeros(3);
            muCB   = propagator.ODEmodel.SolarSystem.Mu(propagator.FM.CentralBodyIndex);
            mubyr3 = muCB/r3;

            %  Calulate terms associated with central body
            xdot(1:3,1) = vv;
            xdot(4:6,1) = -mubyr3*rv;

            if PropSTM
                A  = zero3;
                B  = eye3;
                C  = -mubyr3*eye3 + 3*muCB*rv*rv'/r5;
                D  = zero3;
            else
                A  = zero3;
                B  = zero3;
                C  = zero3;
                D  = zero3;
            end

            %  Calulate terms associated with point mass perturbations
            if size(propagator.FM.PointMassIndeces,2) > 1
                xdot_PM = zeros(3,1);
                not working yet!
                C_PM    = zeros(3,3);
                [DeltaAT] = MJD2TimeCoeff(jd_ref);
                DeltaTT   = DeltaAT + 32.184;
                jd_tt     = jd_ref + (DeltaTT + t)/86400;
                for i = 1:size(propagator.FM.PointMassIndeces,2)

                    %  If the point mass is not the central body!!
                    if propagator.FM.PointMassIndeces{i} ~= propagator.FM.CentralBodyIndex

                        muk     = SolarSystem.Mu(propagator.FM.PointMassIndeces{i});
                        rvk     = pleph(jd_tt,propagator.FM.PointMassIndeces{i},propagator.FM.CentralBodyIndex,1);
                        rvkmr   = rvk - rv;
                        rvk3    = norm(rvk)^3;
                        rvkmr3  = norm(rvkmr)^3;
                        rvkmr5  = norm(rvkmr)^5;
                        xdot    = xdot  + muk*(rvkmr/rvkmr3 - rvk/rvk3);

                        if PropSTM
                            C    = C + muk*( - 1/rvkmr3*eye3 + 3*rvkmr*rvkmr'/rvkmr5);
                        end

                    end

                end
            end

            %  Now construct Amat
            Amat = [A B; C D];

        end % PointMasses

    end % methods

end % classdef