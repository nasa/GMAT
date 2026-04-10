%% This is a the basic, minimalistic TAT-C API example. It computes requested 
%  quantities but performs no analysis using the computed data

clear all; close all;

% Create the epoch object and set the initial epoch
date = AbsoluteDate();
date.SetGregorianDate(2017,1,15,22,30,20.111);

% Create the spacecraft state object and set Keplerian elements
state = OrbitState();
state.SetKeplerianState(6500,0.002,45*pi/180,pi/4 + pi/6,.2345,pi/6);

% Create a spacecraft giving it a state and epoch
sat1 = Spacecraft(date,state);

% Create the propagator
prop = Propagator(sat1);

% Create the point group and initialize the coverage checker
pGroup = PointGroup('Helical',300);
covChecker = CoverageChecker(pGroup,sat1);

% Propagate for a duration and collect data
startDate = date.GetJulianDate();
count = 0;
while date.GetJulianDate() < startDate + 0.25
    
    % Propagate
    date.Advance(60);
    prop.Propagate(date);
    % Compute coverage data for current point
    covChecker.AccumulateCoverageData();
   
end

% Compute coverage data for the propagtion span
coverageEvents = covChecker.ProcessCoverageDate();


