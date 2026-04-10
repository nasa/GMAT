gsloc =   [  -593.9438378691
    5159.44681506209
    -3691.17290768164];

satloc =  [-33676.9539725404
    24914.768850706
    5824.40009033107];


satloc = satloc ;

% Compute the range vector from station to spacecraft (inertial)
rangeApparent = satloc - gsloc;
range_mag     = norm(rangeApparent);

% compute apparent Declination
decTop = asin(rangeApparent(3,1)/range_mag);
foo    = sqrt(rangeApparent(1,1)^2+rangeApparent(2,1)^2);
raTop  = atan2((rangeApparent(2,1)/foo),(rangeApparent(1,1)/foo));
raTop  = mod(raTop,2*pi); % RA is between 0 and 360 degrees
yref   = [raTop;decTop];

%=========================
%--- The partials
%===========================
xhat = [1 0 0]';
yhat = [0 1 0]';
zhat = [0 0 1]';

ddecdsatpos = 1/sqrt(1 - (rangeApparent(3,1)/range_mag)^2)*...
              (zhat'/range_mag - rangeApparent(3,1)/range_mag^3*rangeApparent');
dradsatpos  = 1/ (1 + (rangeApparent(2,1)/rangeApparent(1,1))^2)*...
    (yhat'/rangeApparent(1,1) - xhat'*rangeApparent(2,1)/rangeApparent(1,1)^2);

dydxref = [dradsatpos;ddecdsatpos];

%==============================================================
%==============================================================

gsloc =   [  -593.9438378691
             5159.44681506209
            -3691.17290768164];

satloc =  [-33676.9539725404
            24914.768850706
             5824.40009033107];

h = 1e-6;
satloc = satloc + transpose([ h 0 0 ]);

rangeApparent = satloc - gsloc;
range_mag     = sqrt(transpose(rangeApparent)*rangeApparent);

% compute apparent Declination
decTop = asin(rangeApparent(3,1)/range_mag);
foo = sqrt(rangeApparent(1,1)^2+rangeApparent(2,1)^2);
raTop = atan2((rangeApparent(2,1)/foo),(rangeApparent(1,1)/foo));
raTop = mod(raTop,2*pi); % RA is between 0 and 360 degrees

y = [raTop;decTop];

dydx_x = (y - yref)/h;

%==============================================================
%==============================================================

gsloc =   [  -593.9438378691
             5159.44681506209
            -3691.17290768164];

satloc =  [-33676.9539725404
            24914.768850706
             5824.40009033107];

satloc = satloc + transpose([0 h 0 ]);

rangeApparent = satloc - gsloc;
range_mag     = sqrt(transpose(rangeApparent)*rangeApparent);

% compute apparent Declination
decTop = asin(rangeApparent(3,1)/range_mag);
foo = sqrt(rangeApparent(1,1)^2+rangeApparent(2,1)^2);
raTop = atan2((rangeApparent(2,1)/foo),(rangeApparent(1,1)/foo));
raTop = mod(raTop,2*pi); % RA is between 0 and 360 degrees

y = [raTop;decTop];

dydx_y = (y - yref)/h;

%==============================================================
%==============================================================

gsloc =   [  -593.9438378691
             5159.44681506209
            -3691.17290768164];

satloc =  [-33676.9539725404
            24914.768850706
             5824.40009033107];

satloc = satloc + transpose([0 0 h]);

rangeApparent = satloc - gsloc;
range_mag     = sqrt(transpose(rangeApparent)*rangeApparent);

% compute apparent Declination
decTop = asin(rangeApparent(3,1)/range_mag);
foo    = sqrt(rangeApparent(1,1)^2+rangeApparent(2,1)^2);
raTop  = atan2((rangeApparent(2,1)/foo),(rangeApparent(1,1)/foo));
raTop  = mod(raTop,2*pi); % RA is between 0 and 360 degrees

y = [raTop;decTop];

dydx_z = (y - yref)/h;

dydxref
dydx_i = [dydx_x dydx_y dydx_z]