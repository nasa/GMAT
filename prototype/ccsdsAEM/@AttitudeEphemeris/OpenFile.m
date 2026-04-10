function OpenFile(obj)

% open the ephemeris file
obj.fileHandle = fopen(obj.EphemerisFile);
if obj.fileHandle == -1
    disp(['Attitude ephemeris file named "'...
        obj.EphemerisFile '" failed to load.']);
    return
end

