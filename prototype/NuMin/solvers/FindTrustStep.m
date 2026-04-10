function diff = FindTrustStep(tau,pU,pB,Delta2,Type)

%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

if Type == 1
    v = tau*pU;
else
    v = pU + (tau - 1)*(pB - pU);
end

diff = 1000*( v'*v - Delta2 );