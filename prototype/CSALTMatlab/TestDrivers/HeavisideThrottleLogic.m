clear all


% Thruster1 = ElectricThruster();
% Thruster1.ThrustModel        = 'ThrustMassPolynomial';
% Thruster1.debugMath          = false();
% Thruster1.MaximumUsablePower = 2;
% Thruster1.MinimumUsablePower = 0.64;
% Thruster1.ThrustCoeff1       = 1.19388817E-02*1000;
% Thruster1.ThrustCoeff2       = 1.60989424E-02*1000;
% Thruster1.ThrustCoeff3       = 1.14181412E-02*1000;
% Thruster1.ThrustCoeff4       = -2.04053417E-03*1000;
% Thruster1.ThrustCoeff5       = 1.01855017E-04*1000; %  milli - Newton
% Thruster1.MassFlowCoeff1     = 2.75956482E-06*1e6;
% Thruster1.MassFlowCoeff2     = -1.71102132E-06*1e6;
% Thruster1.MassFlowCoeff3     = 1.21670237E-06*1e6;
% Thruster1.MassFlowCoeff4     = -2.07253445E-07*1e6;
% Thruster1.MassFlowCoeff5     = 1.10213671E-08*1e6; %  milligrams/sec
% 
% Thruster2 = ElectricThruster();
% Thruster2.ThrustModel        = 'ThrustMassPolynomial';
% Thruster2.debugMath          = false();
% Thruster2.MaximumUsablePower = 7.36;
% Thruster2.MinimumUsablePower = .7;
% Thruster2.ThrustCoeff1       = 1.19388817E-02*1000;
% Thruster2.ThrustCoeff2       = 1.60989424E-02*1000;
% Thruster2.ThrustCoeff3       = 1.14181412E-02*1000;
% Thruster2.ThrustCoeff4       = -2.04053417E-03*1000;
% Thruster2.ThrustCoeff5       = 1.01855017E-04*1000; %  milli - Newton
% Thruster2.MassFlowCoeff1     = 2.75956482E-06*1e6;
% Thruster2.MassFlowCoeff2     = -1.71102132E-06*1e6;
% Thruster2.MassFlowCoeff3     = 1.21670237E-06*1e6;
% Thruster2.MassFlowCoeff4     = -2.07253445E-07*1e6;
% Thruster2.MassFlowCoeff5     = 1.10213671E-08*1e6; %  milligrams/sec

% Thruster3 = ElectricThruster();
% Thruster3.ThrustModel        = 'ThrustMassPolynomial';
% Thruster3.debugMath          = false();
% Thruster3.MaximumUsablePower = 3;
% Thruster3.MinimumUsablePower = 0.9;
% Thruster3.ThrustCoeff1       = 1.19388817E-02*1000;
% Thruster3.ThrustCoeff2       = 1.60989424E-02*1000;
% Thruster3.ThrustCoeff3       = 1.14181412E-02*1000;
% Thruster3.ThrustCoeff4       = -2.04053417E-03*1000;
% Thruster3.ThrustCoeff5       = 1.01855017E-04*1000; %  milli - Newton
% Thruster3.MassFlowCoeff1     = 2.75956482E-06*1e6;
% Thruster3.MassFlowCoeff2     = -1.71102132E-06*1e6;
% Thruster3.MassFlowCoeff3     = 1.21670237E-06*1e6;
% Thruster3.MassFlowCoeff4     = -2.07253445E-07*1e6;
% Thruster3.MassFlowCoeff5     = 1.10213671E-08*1e6; %  milligrams/sec
% 
% Thruster4 = ElectricThruster();
% Thruster4.ThrustModel        = 'ThrustMassPolynomial';
% Thruster4.debugMath          = false();
% Thruster4.MaximumUsablePower = 7.36;
% Thruster4.MinimumUsablePower = .8;
% Thruster4.ThrustCoeff1       = 1.19388817E-02*1000;
% Thruster4.ThrustCoeff2       = 1.60989424E-02*1000;
% Thruster4.ThrustCoeff3       = 1.14181412E-02*1000;
% Thruster4.ThrustCoeff4       = -2.04053417E-03*1000;
% Thruster4.ThrustCoeff5       = 1.01855017E-04*1000; %  milli - Newton
% Thruster4.MassFlowCoeff1     = 2.75956482E-06*1e6;
% Thruster4.MassFlowCoeff2     = -1.71102132E-06*1e6;
% Thruster4.MassFlowCoeff3     = 1.21670237E-06*1e6;
% Thruster4.MassFlowCoeff4     = -2.07253445E-07*1e6;
% Thruster4.MassFlowCoeff5     = 1.10213671E-08*1e6; %  milligrams/sec


% aThruster                    = ElectricThruster();
% aThruster.debugMath          = false();
% %  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
% aThruster.ThrustModel        = 'ThrustMassPolynomial';
% aThruster.MaximumUsablePower = 7.36;
% aThruster.MinimumUsablePower = 0.64;
% % aThruster.MinimumUsablePower = 0.0;
% aThruster.ThrustCoeff1       = 3.68945763000000015452428669959772378206253051757812;
% aThruster.ThrustCoeff2       = 40.54325099999999792999005876481533050537109375;
% aThruster.ThrustCoeff3       = -7.9162181399999997921668182243593037128448486328125;
% aThruster.ThrustCoeff4       = 1.72548415999999993353242189186858013272285461425781;
% aThruster.ThrustCoeff5       =  -0.11156312599999999857214305620800587348639965057373; %  milli - Newton
% aThruster.MassFlowCoeff1     = 2.22052154999999995510506778373382985591888427734375;
% aThruster.MassFlowCoeff2     = -0.18091926199999999735901212716271402314305305480957;
% aThruster.MassFlowCoeff3     = 0.0277715759999999992069241727676853770390152931213379;
% aThruster.MassFlowCoeff4     = 0.0298873979999999990486703183023564633913338184356689;
% aThruster.MassFlowCoeff5     = -0.00291399099999999989032972713687286159256473183631897; %  milligrams/sec
% % aThruster.ThrustCoeff1       = 1.19388817E-02*1000;
% % aThruster.ThrustCoeff2       = 1.60989424E-02*1000;
% % aThruster.ThrustCoeff3       = 1.14181412E-02*1000;
% % aThruster.ThrustCoeff4       = -2.04053417E-03*1000;
% % aThruster.ThrustCoeff5       = 1.01855017E-04*1000; %  milli - Newton
% % aThruster.MassFlowCoeff1     = 2.75956482E-06*1e6;
% % aThruster.MassFlowCoeff2     = -1.71102132E-06*1e6;
% % aThruster.MassFlowCoeff3     = 1.21670237E-06*1e6;
% % aThruster.MassFlowCoeff4     = -2.07253445E-07*1e6;
% % aThruster.MassFlowCoeff5     = 1.10213671E-08*1e6; %  milligrams/sec
% aThruster.FixedEfficiency    = 0.654321;
% aThruster.GravitationalAccel = 9.805;
% %aThruster.Isp                = 3219.12314;
% %aThruster.ConstantThrust     = 0.01243;
% %aThruster.ThrustCoordSys     = 2; 
% %aThruster.ThrustDirection1   = 1;
% %aThruster.ThrustDirection2   = 0;
% %aThruster.ThrustDirection3   = 0;
% aThruster.DutyCycle          = .90;
% aThruster.ThrustScaleFactor  = 1.0;
% 
% bThruster                    = ElectricThruster();
% bThruster.debugMath          = false();
% %  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
% bThruster.ThrustModel        = 'ThrustMassPolynomial';
% bThruster.MaximumUsablePower = 7.36;
% bThruster.MinimumUsablePower = 0.64;
% % bThruster.MinimumUsablePower = 0.0;
% bThruster.ThrustCoeff1       = 3.68945763000000015452428669959772378206253051757812;
% bThruster.ThrustCoeff2       = 40.54325099999999792999005876481533050537109375;
% bThruster.ThrustCoeff3       = -7.9162181399999997921668182243593037128448486328125;
% bThruster.ThrustCoeff4       = 1.72548415999999993353242189186858013272285461425781;
% bThruster.ThrustCoeff5       =  -0.11156312599999999857214305620800587348639965057373; %  milli - Newton
% bThruster.MassFlowCoeff1     = 2.22052154999999995510506778373382985591888427734375;
% bThruster.MassFlowCoeff2     = -0.18091926199999999735901212716271402314305305480957;
% bThruster.MassFlowCoeff3     = 0.0277715759999999992069241727676853770390152931213379;
% bThruster.MassFlowCoeff4     = 0.0298873979999999990486703183023564633913338184356689;
% bThruster.MassFlowCoeff5     = -0.00291399099999999989032972713687286159256473183631897; %  milligrams/sec
% % bThruster.ThrustCoeff1       = 1.19388817E-02*1000;
% % bThruster.ThrustCoeff2       = 1.60989424E-02*1000;
% % bThruster.ThrustCoeff3       = 1.14181412E-02*1000;
% % bThruster.ThrustCoeff4       = -2.04053417E-03*1000;
% % bThruster.ThrustCoeff5       = 1.01855017E-04*1000; %  milli - Newton
% % bThruster.MassFlowCoeff1     = 2.75956482E-06*1e6;
% % bThruster.MassFlowCoeff2     = -1.71102132E-06*1e6;
% % bThruster.MassFlowCoeff3     = 1.21670237E-06*1e6;
% % bThruster.MassFlowCoeff4     = -2.07253445E-07*1e6;
% % bThruster.MassFlowCoeff5     = 1.10213671E-08*1e6; %  milligrams/sec
% bThruster.FixedEfficiency    = 0.654321;
% bThruster.GravitationalAccel = 9.805;
% %bThruster.Isp                = 3219.12314;
% %bThruster.ConstantThrust     = 0.01243;
% %bThruster.ThrustCoordSys     = 2; 
% %bThruster.ThrustDirection1   = 1;
% %bThruster.ThrustDirection2   = 0;
% %bThruster.ThrustDirection3   = 0;
% bThruster.DutyCycle          = .90;
% bThruster.ThrustScaleFactor  = 1.0;


%%  Configure the Thrusters
aThruster                    = ElectricThruster();
aThruster.debugMath          = false();
%  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
aThruster.ThrustModel        = 'ThrustMassPolynomial';
aThruster.MaximumUsablePower = 7.1;
aThruster.MinimumUsablePower = 0.6;
aThruster.ThrustCoeff1       = 7.282;
aThruster.ThrustCoeff2       = 32.901;
aThruster.ThrustCoeff3       = 0;
aThruster.ThrustCoeff4       = 0;
aThruster.ThrustCoeff5       =  0; %  milli - Newton
aThruster.MassFlowCoeff1     = 1.066;
aThruster.MassFlowCoeff2     = .699;
aThruster.MassFlowCoeff3     = 0;
aThruster.MassFlowCoeff4     = 0;
aThruster.MassFlowCoeff5     = 0; %  milligrams/sec
aThruster.DutyCycle          = 1.0;
aThruster.ThrustScaleFactor  = 1.0;

%%  Configure the Thrusters
bThruster                    = ElectricThruster();
bThruster.debugMath          = false();
%  ConstantThrustAndIsp, FixedEfficiency, ThrustMassPolynomial
bThruster.ThrustModel        = 'ThrustMassPolynomial';
bThruster.MaximumUsablePower = 7.1;
bThruster.MinimumUsablePower = 0.6;
bThruster.ThrustCoeff1       = 7.282;
bThruster.ThrustCoeff2       = 32.901;
bThruster.ThrustCoeff3       = 0;
bThruster.ThrustCoeff4       = 0;
bThruster.ThrustCoeff5       =  0; %  milli - Newton
bThruster.MassFlowCoeff1     = 1.066;
bThruster.MassFlowCoeff2     = .699;
bThruster.MassFlowCoeff3     = 0;
bThruster.MassFlowCoeff4     = 0;
bThruster.MassFlowCoeff5     = 0; %  milligrams/sec
bThruster.DutyCycle          = 1.0;
bThruster.ThrustScaleFactor  = 1.0;
FB = FiniteBurn();
FB.debugMath    = false();
FB.ThrottleLogic = 'HeavisideMaxNumberOfThrusters';

FB.SetThrusters({aThruster,bThruster});
% FB.SetThrusters({Thruster1,Thruster2,Thruster3,Thruster4});



% disp([num2str(FB.numToFire),' thrusters firing (',num2str(FB.smoothed_numToFire),')'])
% disp(['Thruster 1: ',num2str(Thruster1.powerAllocated)])
% disp(['Thruster 2: ',num2str(Thruster2.powerAllocated)])
% disp(['Thruster 3: ',num2str(Thruster3.powerAllocated)])
% disp(['Thruster 4: ',num2str(Thruster4.powerAllocated)])

% sharpness = [10 100 1000 10000];
sharpness = [10000];

figure(1)
% clf
subplot(4,1,1)
hold on
subplot(4,1,2)
hold on
subplot(4,1,3)
hold on
subplot(4,1,4)
hold on
% power_in = 7.32:.00001:7.42;
power_in = 0:.0001:22;
% power_in = 8;
thrust_out = zeros(length(sharpness),length(power_in));
mdot_out = zeros(length(sharpness),length(power_in));
n_out = zeros(length(sharpness),length(power_in));
PowerAllocated1 = zeros(length(sharpness),length(power_in));
PowerAllocated2 = zeros(length(sharpness),length(power_in));
PowerAllocated3 = zeros(length(sharpness),length(power_in));
PowerAllocated4 = zeros(length(sharpness),length(power_in));
Heavi1 = zeros(length(sharpness),length(power_in));
Heavi2 = zeros(length(sharpness),length(power_in));
Heavi3 = zeros(length(sharpness),length(power_in));
Heavi4 = zeros(length(sharpness),length(power_in));
for j = 1:length(sharpness)
	FB.throttleHeavisideSharpness = sharpness(j);
	for i = 1:length(power_in)
		[thrust,massFlowRate] = FB.GetThrustAndMassFlowRate(power_in(i));
		thrust_out(j,i) = thrust(1);
		mdot_out(j,i) = massFlowRate;
		PowerAllocated1(j,i) = FB.Thrusters{1}.GetPowerAllocated();
		PowerAllocated2(j,i) = FB.Thrusters{2}.GetPowerAllocated();
% 		PowerAllocated3(j,i) = FB.Thrusters{3}.GetPowerAllocated();
% 		PowerAllocated4(j,i) = FB.Thrusters{4}.GetPowerAllocated();
		Heavi1(j,i) = FB.Heaviside(1);
		Heavi2(j,i) = FB.Heaviside(2);
% 		Heavi3(j,i) = FB.Heaviside(3);
% 		Heavi4(j,i) = FB.Heaviside(4);
		
		n_out(j,i) = sum(FB.Heaviside);
	end
	subplot(4,1,1); hold on
	plot(power_in,thrust_out(j,:),'b','DisplayName',['k = ',num2str(sharpness(j))],'linewidth',3)
	subplot(4,1,2); hold on
	plot(power_in,mdot_out(j,:),'b','DisplayName',['k = ',num2str(sharpness(j))],'linewidth',3)
	subplot(4,1,3); hold on
	plot(power_in,n_out(j,:),'b','DisplayName',['k = ',num2str(sharpness(j))],'linewidth',3)
	subplot(4,1,4); hold on
	plot(power_in,PowerAllocated1(j,:))
	plot(power_in,PowerAllocated2(j,:))
% 	plot(power_in,PowerAllocated3(j,:))
% 	plot(power_in,PowerAllocated4(j,:))
	% plot(power_in,power_in)
	% plot(power_in,PowerAllocated1(j,:)+PowerAllocated2(j,:)+PowerAllocated3(j,:)+PowerAllocated4(j,:))
end
figure(1)
subplot(4,1,1)
ylabel('thrust')

subplot(4,1,2)
ylabel('mdot')
legend show

subplot(4,1,3)
ylabel('number of thrusters, smoothed')

subplot(4,1,4)
ylabel('Power Allocated')
xlabel('Power In, kW')
