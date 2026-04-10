function [rawData, m_AzimuthStepSize, m_ElevationStepSize] = GetRawData()

persistent rawData 

%% the raw data for firefly
forceScaleFac = 1;  % Used for unit conversion only.  If m, set to 1.0;


JWSTSpadFileData;
%SphereSpadFileData;


rawData(:,3:5) = rawData(:,3:5)*forceScaleFac;