function x = Spacecraft_GetState(Sat,Id)

global ObjectStore

%==========================================================================
%==========================================================================
%------  Get the requested stat value defined by Id
%==========================================================================
%==========================================================================

if Id == 1;
    x = [Sat.X Sat.Y Sat.Z Sat.VX Sat.VY Sat.VZ]';
elseif Id == 2;
    x = Sat.Cd;
elseif Id == 3
    x = Sat.Cr;
elseif Id == 4
    x = reshape(Sat.STM,36,1);
end