function CloseFile(obj)

% open the ephemeris file
flag = fclose(obj.fileHandle);
if flag ~= 0
    disp(['Attitude ephemeris file named "'...
        obj.EphemerisFile '" failed to close.']);
    return
else
    obj.fileHandle = [];
end

