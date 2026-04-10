%%  This scripte is a basic test interface for the CCSDS OEM ephemeris 
% reader and interpolator.  The script reads in a GMAT ephemeris file and
% then interpolates the ephemeris to a requested time step.

% NOTE ON PATH:  This test drivers assumes you are running from the 
% folder named ccsdsOEM\test 

%% Create an ephemeris object and parse the file
ephFile = Ephemeris();
ephFile.EphemerisFile = ['..\ephemfiles\GMATTwoBlocks.eph'];
ephFile.ParseEphemeris();

%%  Interpolate the file at a new time step as a test.  
%  NOTE:  the interplotor currently works in MATLAB datenum time.  We
%  should change this.
timeStep = 345;
timeVec = ephFile.firstEpochOnFile:timeStep/86400:ephFile.lastEpochOnFile;
interpData = zeros(length(timeVec),6);
for timeIdx = 1:length(timeVec)      
   interpData(timeIdx,:) = ephFile.GetState(timeVec(timeIdx))';
end



