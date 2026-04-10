%%  Load the configuration for the test case
%caseName = 'NuclearPower_ThrustMassPoly_Earth_NoShadowBody';
%caseName = 'SolarPower_ThrustMassPoly_Earth_EarthShadowBody';
%caseName = 'SolarPower_ThrustMassPoly_Earth_EarthShadowBodyButNoShadows';
%caseName = 'SolarPower_ThrustMassPoly_Earth_NoShadowBodies';
%caseName = 'NuclearPower_FixedEfficiency_Earth_NoShadowBody';
%caseName = 'SolarPower_FixedEfficiency_Earth_NoShadowBodies';
%caseName = 'SolarPower_FixedEfficiency_Earth_EarthShadowBody'
%caseName = 'SolarPower_ConstantThrustAndIsp_Earth_EarthShadowBody';
%caseName = 'SolarPower_ConstantThrustAndIsp_Earth_NoShadowBody';
%caseName = 'NuclearPower_ConstantThrustAndIsp_Earth_NoShadowBody';
%caseName = 'SolarPower_ConstantThrustAndIsp_Earth_NoShadowBody_J2000Eq';
caseName = 'SolarPower_ConstantThrustAndIsp_Earth_NoShadowBody_J2000Ec';
eval(caseName)

%%  Set up prop epoch
writeTruthFile = 1;
if writeTruthFile
    repoPath = 'C:\Users\sphughe1\Documents\Repos\Jazz\trunk\test\script\input\Resources\FRR-45_ElectricThruster\';
    truthFolder = 'truth\';
    truthPath = [repoPath truthFolder];
    truthFileName = [truthPath caseName '.truth'];
    fileId = fopen(truthFileName, 'w+');
end

%%  Configure integrator and propagate
if Force.STM  && ~Force.UseFiniteBurn
    initialState = [Sat.OrbitState; reshape(eye(6),[36,1])];
elseif Force.STM  && Force.UseFiniteBurn
    initialState = [Sat.OrbitState; Sat.GetTotalMass(); ...
        reshape(eye(6),[36,1])];
elseif ~Force.STM  && Force.UseFiniteBurn
    initialState = [Sat.OrbitState; Sat.GetTotalMass()];
else
    initialState = Sat.OrbitState;
end
odeOptions           = odeset('RelTol',1e-12,'AbsTol',1e-12);

%[timeHist, stateHist, data] = ode78_FM('OrbitForce', 0, propDuration, initialState, 1e-13, 0, Force, Sat);
propDuration    = 1*86400;
[timeHist, stateHist, data] = ode113('OrbitForce', [0:.1:1]*propDuration, initialState,odeOptions,Force,Sat);

%SEPPropagationPlots
truth = [timeHist stateHist];
if writeTruthFile
    formatStr = '%16.8f %16.8f  %16.8f  %16.8f  %16.8f  %16.8f %16.8f %16.8f\n';
    for rowIdx = 1:size(truth,1)
        data = sprintf(formatStr,truth(rowIdx,:));
        fprintf(fileId,'%s',data);
    end
    fclose(fileId);
end
