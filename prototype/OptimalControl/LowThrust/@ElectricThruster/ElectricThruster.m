classdef ElectricThruster < handle
    
    % ElectricThruster.  This is a prototype spacecraft class for GMAT design and
    %testing.  Written by S. Hughes.  steven.p.hughes@nasa.gov
    
    properties  (SetAccess = public)
        %  Options: ThrustMassPolynomial, ContantThrustIsp, FixedEfficiency
        ThrustModel        = 'ThrustMassPolynomial';
        MaximumUsablePower = 7.266;
        MinimumUsablePower = 0.638;
        powerAllocated     = 5;
        
        %  Thrust coefficients
        ThrustCoeff1 = -1000.92E-06 ;
        ThrustCoeff2 = 54.05382 ;
        ThrustCoeff3 = -14.41789;
        ThrustCoeff4 =  2.96519 ;
        ThrustCoeff5 = -5.190820; %  milli - Newton
        
        %  Mass Flow rate coefficients
        MassFlowCoeff1  = 2.13781  ;
        MassFlowCoeff2  = 0.03211  ;
        MassFlowCoeff3  = -0.09956;
        MassFlowCoeff4  = 0.05717 ;
        MassFlowCoeff5  = -0.004776 ; %  milligrams/sec
        
        %  Efficiency coefficient
        FixedEfficiency     = 0.6;
        %  Const. Thrust Isp settings
        ConstantThrust      = 0.01
        ThrustConst         = 0.02;
        GravitationalAccel  = 9.80665;
        Isp                 = 3000;
        
        %  Thrust Direction and general settings
        ThrustCoordSys    = 1;  % 1 = J2000Eq, 2 = VNB
        Spacecraft        = {}; %  Spacecraft parent object
        ThrustDirection1  = 1;
        ThrustDirection2  = 0;
        ThrustDirection3  = 0;
        DutyCycle         = 1;
        ThrustScaleFactor = 1;
        
        debugMath = false();
    end
    
    methods
        
        %%  Get nominal thrust magnitude (before duty cycle and scale fac)
        % return thrust in kNewtons and mass flow in kg/s
        function [thrustMag,mDot,powerUsed] = ...
                GetNomThrustandMassFlow(obj)
            
            %  Compute amount of power to use
            powerUsed = obj.GetUsablePower();
            
            %  If no power is used then no thrust and mass flow so return a
            %  value of zero
            if powerUsed <= 0
                thrustMag = 0;
                mDot      = 0;
                return
            end
            
            %%  Compute nominal thrust (kN) and mass flow rate (kg/s)
            if strcmp(obj.ThrustModel,'ThrustMassPolynomial')
                % Englander: Eq 12
                thrustMag = (obj.ThrustCoeff5*powerUsed^4 + ...
                    obj.ThrustCoeff4*powerUsed^3+...
                    obj.ThrustCoeff3*powerUsed^2+ ...
                    obj.ThrustCoeff2*powerUsed + ...
                    obj.ThrustCoeff1)/1e6;
                mDot = (obj.MassFlowCoeff5*powerUsed^4 + ...
                    obj.MassFlowCoeff4*powerUsed^3 + ...
                    obj.MassFlowCoeff3*powerUsed^2+...
                    obj.MassFlowCoeff2*powerUsed^1 +...
                    obj.MassFlowCoeff1)/1e6;
                obj.Isp = thrustMag/mDot/obj.GravitationalAccel*1000;
            elseif strcmp(obj.ThrustModel,'ConstantThrustAndIsp')
                %  ConstantThrust is in Newtons, must return K Newton.
                fac = 1000;
                thrustMag =(obj.ConstantThrust/fac);
                mDot = (obj.ConstantThrust)/obj.Isp/...
                    (obj.GravitationalAccel);
            elseif strcmp(obj.ThrustModel,'FixedEfficiency')
                % Englander: Eq. 15
                fac = 1000;
                thrustMag = 2*obj.FixedEfficiency*powerUsed/fac/...
                    obj.Isp/(obj.GravitationalAccel/fac);
                mDot = 2*obj.FixedEfficiency*powerUsed/fac/...
                    (obj.Isp*(obj.GravitationalAccel/fac))^2;
            end
            
            if obj.debugMath
                disp(['Thrust Mag      : ' num2str(thrustMag,16)]);
                disp(['Mass Flow       : ' num2str(mDot,16)]);
                disp(['Isp             : ' num2str(obj.Isp,16)]);
            end
            
        end
        
        %%  Compute Usable Power (kW)
        function powerUsed = GetUsablePower(obj)
            %  Compute amount of power to use
            if obj.powerAllocated < obj.MinimumUsablePower
                powerUsed = 0;
                return
            elseif obj.powerAllocated >= obj.MinimumUsablePower &&...
                    obj.powerAllocated <= obj.MaximumUsablePower
                powerUsed = obj.powerAllocated;
            else
                powerUsed = obj.MaximumUsablePower;
            end
            
            if obj.debugMath
                disp(['Power Used      : ' num2str(powerUsed,16)]);
            end
        end
        
        %% Set power allocated
        function SetPowerAllocated(obj,power)
            obj.powerAllocated = power;
            if obj.debugMath
                disp('====== Electric Thruster Debug Data =====');
                disp(['Power Allocated : ' num2str(power,16)]);
            end
        end
        
        %%  Compute rotation matrix from thrust system to inertial
        function DCM = GetThrustDCM(obj)
            
            if obj.ThrustCoordSys == 1
                %  J2000Eq
                DCM = eye(3);
            elseif obj.ThrustCoordSys ==2
                % VNB
                rv   = obj.Spacecraft.OrbitState(1:3,1);
                vv   = obj.Spacecraft.OrbitState(4:6,1);
                xhat = vv/norm(vv);
                yhat = cross(rv,vv);
                yhat = yhat/norm(yhat);
                zhat = cross(xhat,yhat);
                DCM  = [xhat yhat zhat];
            elseif obj.ThrustCoordSys == 3;
                % J2000Ec
                DCM(1,1) = 1.0;
                DCM(1,2) = 0.0;
                DCM(1,3) = 0.0;
                DCM(2,1) = 0.0;
                DCM(2,2) = 0.917482062076895741;
                DCM(2,3) = -0.397777155914121383;
                DCM(3,1) = 0.0;
                DCM(3,2) = 0.397777155914121383;
                DCM(3,3) = 0.917482062076895741;
                DCM      = DCM;
            end
            
        end
        
        %%  Compute thrust unit vector in thrust system
        function thrustUnitVec = GetThrustUnitVec(obj)
            thrustDir = [obj.ThrustDirection1 obj.ThrustDirection2...
                obj.ThrustDirection3]';
            thrustUnitVec = thrustDir/norm(thrustDir);
        end
        
        %%  Get thrust vector (kN) and mass flow rate (kg/s)
        function [thrustVec,mDot,powerUsed] = ...
                GetThrustVectorMassFlowRate(obj)
            
            %  Compute thrust unit vector in thrust coord sys
            thrustUnitVec  = obj.GetThrustUnitVec();
            
            %  Compute matrix from thrust system to inertial
            Rmat = GetThrustDCM(obj);
            %  Compute the thrust vector
            thrustDir = Rmat*thrustUnitVec;
            [thrustMag,mDot,powerUsed] = ...
                obj.GetNomThrustandMassFlow();
            thrustVec = obj.DutyCycle*obj.ThrustScaleFactor*...
                thrustMag*thrustDir;
            mDot = -obj.DutyCycle*mDot;
            if obj.debugMath
                disp(['Thrust Unit X   : ' num2str(thrustDir(1),16)]);
                disp(['Thrust Unit Y   : ' num2str(thrustDir(2),16)]);
                disp(['Thrust Unit Z   : ' num2str(thrustDir(3),16)]);
                disp(['Thrust Vec  X   : ' num2str(thrustVec(1),16)]);
                disp(['Thrust Vec  Y   : ' num2str(thrustVec(2),16)]);
                disp(['Thrust Vec  Z   : ' num2str(thrustVec(3),16)]);
                disp(['Scaled MassFlow : ' num2str(mDot,16)]);
            end
        end
    end
    
end