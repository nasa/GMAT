function percentSun = GetPercentSun(rv,rv_sun)

%  Author: S. Hughes
%
%  Inputs
%  rv:  the spacecraft position with respect to the shadow body
%  rv_sun: The sun position with respect to the shadow body
%
%  Outputs
%  percentSun:  1 if full shadow, zero if umbra.  
%               0 < percentSun < 1 for annular or penumbral eclipse
%

%  Get body radii and location of sun and sat.
bodyRadius = 6378.1363;
sunRadius  = 695990.0000;
%  Calculate the apparent radius of Sun and Earth
appSunRad  = asin(sunRadius/norm(rv_sun-rv));
appBodyRad = asin(bodyRadius/norm(rv));
%  Calculate the apparent distance between Sun and Earth
appDistSunBody = acos(dot(-rv/norm(rv),(rv_sun-rv)/...
    norm(rv_sun-rv)));
%  Compute percent shadow based on the conditions
if (appSunRad + appBodyRad <= appDistSunBody )
    percentSun = 1;  % In full Sun
elseif (appDistSunBody<=appBodyRad-appSunRad)
    percentSun = 0;  %  In Umbra
elseif abs(appSunRad-appBodyRad)<appDistSunBody && ...
        appDistSunBody < appSunRad + appBodyRad
    % Penumbra, not an annular eclipse
    c1 = (appDistSunBody^2 + appSunRad^2 - appBodyRad^2 )...
        / 2/ appDistSunBody;
    c2 = sqrt( appSunRad^2 - c1^2 );
    A = appSunRad^2*acos(c1/appSunRad)+appBodyRad^2*...
        acos( (appDistSunBody - c1)/appBodyRad ) - ...
        appDistSunBody*c2;
    percentSun =  1 - A/pi/appSunRad^2;
else
    percentSun = 1 - appBodyRad^2/appSunRad^2; %  Annular
end