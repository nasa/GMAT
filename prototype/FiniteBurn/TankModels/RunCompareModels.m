%-----------------------------------------------
%     Set Up Variables for this Run
%-----------------------------------------------
Pt         = 400*6894.8;
DutyCycle  = 60/360;
Tank.TPoly =   [-3.7221e-011  3.2147e-004 66.0750 ];
Tank.IspPoly = [-5.6046e-018 1.6877e-011 -5.2779e-006  303.5800  ];
Tank.TPoly =   [-3.3221e-011  4.2147e-004 66.0750 ];
Tank.IspPoly = [2.6877e-011 -5.8779e-006  220.5800  ];
BurnTime   = 300;


%-----------------------------------------------
%     Run Mission using All 3 Tank Models
%-----------------------------------------------
for i = 1:3

    if i == 1
        Tank.ThermoModel = 'Isothermal';
    elseif i == 2;
        Tank.ThermoModel = 'HeatTransfer';
    else
        Tank.ThermoModel = 'Adiabatic';
    end

    % Define the tank properties and initialize the tank
    Tank.GasType     = 'Helium';
    Tank.LiquidType  = 'N2O4';
    Tank.Vt = 0.713;
    Tank.Pt = Pt;
    Tank.Ta = 0;
    Tank.Tl = 294;
    Tank.Tg = 294;
    Tank.Tw = 294;
    Tank.ml = 807;
    Tank.mw = 13.6;
    Tank = BlowDownTank_Initialize(Tank);

    % Set initial state
    X0 = BlowDownTank_GetState(Tank);

    %     [TOUT,YOUT] = ODE45(ODEFUN,TSPAN,Y0,OPTIONS)
    ODEopt = odeset('RelTol', 1e-10, 'AbsTol', 1e-10);
    [t,X] = ode45('TankSystemRHS',[0:1:BurnTime],X0,ODEopt,Tank);

    for i = 1:size(X,1);
        Tank = BlowDownTank_SetState(Tank,X(i,:)');
        P(i,1)  = Tank.Pt;
        Tg(i,1) = Tank.Tg;
        ml(i,1) = Tank.ml;
    end;

    switch Tank.ThermoModel

        case  'Isothermal'

            Tgiso = Tg;
            Piso  = P;
            mliso = ml;

        case 'HeatTransfer'

            Pht   = P;
            Tght  = Tg;
            mlht  = ml;

        case 'Adiabatic'

            Pad   = P;
            mlad  = ml;

    end

end

IsoStyle = 'b-.';
HTStyle  = 'k-';
AdStyle  = 'r--';

%-----------------------------------------------
%     Plot Pressure History Data
%-----------------------------------------------
figure(1)
subplot(2,1,1)
hold on
plot(t,Piso/Piso(1,1),IsoStyle,t,Pht/Pht(1,1),HTStyle,t,Pad/Pad(1,1),AdStyle )
title('Pressure vs Time')
ylabel('Pressure, P/P_o');
xlabel('Time, s')
grid on
legend('Isothermal','Heat Transfer', 'Adiabatic', 'Location', 'Best')

figure(1)
subplot(2,1,2)
for i = 1:size(t,1)
    IsoError(i) = (Pht(i) - Piso(i))/Pht(i)*100;
    AdError(i)  = (Pht(i) - Pad(i))/Pht(i)*100;
    mIsoError(i)   = ( mlht(i) - mliso(i) );
    mAdError(i)    = ( mlht(i) - mlad(i) );
end
plot(t, IsoError, t, AdError)
grid on
title('Percent Difference in Model Pressure Predictions vs. Time')
xlabel('Time, s')
ylabel('Percent Different')
legend('H.T. - Isothermal', 'H.T. - Adiabatic', 'Location', 'NorthWest')

%-----------------------------------------------
%     Plot Mass History Data
%-----------------------------------------------

figure(2);
subplot(2,1,1)
plot(t,mliso/mliso(1,1),IsoStyle,t,mlht/mlht(1,1),HTStyle,t,mlad/mlad(1,1),AdStyle )
hold on 
grid on
title('Mass vs Time')
ylabel('Mass, m/m_o');
xlabel('Time, s')
grid on
legend('Isothermal','Heat Transfer', 'Adiabatic', 'Location', 'SouthWest')

subplot(2,1,2)
plot(t,mIsoError,t,mAdError);
grid on;
grid on
title('Absolute Difference in Mass Predictions vs. Time')
xlabel('Time, s')
ylabel('Mass Difference')
legend('H.T. - Isothermal', 'H.T. - Adiabatic', 'Location', 'NorthWest')

%-----------------------------------------------
%     Plot Temperature History Data
%-----------------------------------------------

figure(3);
subplot(2,1,1)
plot(t,(Tght -  Tgiso)/Tgiso(1,1)*100)
title('Percent Temperature Difference Between Iso. and HT Models vs Time')
ylabel('Temperature, 100*(T_{ht} - T_{iso})/T_o');
xlabel('Time, s')
grid on
grid on

figure(3);
subplot(2,1,2)
plot(t,(Tght -  Tgiso))
grid on
title('Absolute Temperature Difference Between Iso. and HT Models vs Time')
ylabel('Temperature, T_{ht} - T_{iso}');
xlabel('Time, s')
grid on
grid on
%figure(2)
%legend('Isothermal','Heat Transfer', 'Adiabatic')