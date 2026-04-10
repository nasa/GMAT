%% Unit test script for AbsoluteDate class

% Create the epoch object and set the initial epoch
date = AbsoluteDate();

%  Test Gregorian date and test converstion to Julian Date
date.SetGregorianDate(2017,1,15,22,30,20.111);
jDate = date.GetJulianDate();
truthDate = 27769.4377327662 + 2430000;
if norm(jDate - truthDate) > 1e-9;
    error('error converting Gregorian to Julian Date')
end

%  Set Julian date and test conversion to Gregorian date
date.SetJulianDate(2457269.123456789);
gregDate = date.GetGregorianDate();
truthDate = [2015   9    3   14  57   46.6665852069856];
if norm(gregDate - truthDate) > 1e-13;
    error('error converting Gregorian to Julian Date')
end