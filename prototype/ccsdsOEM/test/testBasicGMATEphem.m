%%  This scripte is a basic test interface for the CCSDS OEM ephemeris 
% reader and interpolator.  The script reads in a GMAT ephemeris file and
% then interpolates the ephemeris to a requested time step.

% NOTE ON PATH:  This test drivers assumes you are running from the 
% folder named ccsdsOEM\test 

%% Create an ephemeris object and parse the file
ephFile_Interp = Ephemeris();
ephFile_Interp.EphemerisFile = '..\ephemfiles\ephInterp.eph';
ephFile_Interp.ParseEphemeris();

ephFile_Raw = Ephemeris();
ephFile_Raw.EphemerisFile = '..\ephemfiles\ephRaw.eph';
ephFile_Raw.ParseEphemeris();

%%  Interpolate the raw file at a new time step 
timeStep = 345;
timeVec  = ephFile_Raw.firstEpochOnFile:timeStep/86400 ...
          :ephFile_Raw.lastEpochOnFile;
interpData = zeros(length(timeVec),6);
for timeIdx = 1:length(timeVec)      
   interpData(timeIdx,:) = ephFile_Raw.GetState(timeVec(timeIdx))';
end

%% Compare GMAT interpolated ephem with MATLAB interpolated ephem
rssPosDiff = zeros(size(interpData),1);
rssVelDiff = rssPosDiff;
for ephIdx = 1:length(interpData)
    rssPosDiff(ephIdx,1) = norm(interpData(ephIdx,1:3) - ...
        ephFile_Interp.Segments(1).Data(ephIdx,2:4));
    
    rssVelDiff(ephIdx,1) = norm(interpData(ephIdx,4:6) - ...
        ephFile_Interp.Segments(1).Data(ephIdx,5:7));
end

max(rssPosDiff)
max(rssVelDiff)

