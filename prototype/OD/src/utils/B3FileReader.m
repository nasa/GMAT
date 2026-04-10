function B3FileReader(Infile)

%...script to read the b3 format

%   t. kelecy, boeing-lts, maui, 5/8/2008


%...11 punch mapping for negative dec and elevation
ELP = [ 'J'; 'K'; 'L'; 'M'; 'N'; 'O'; 'P'; 'Q'; 'R' ];
rtd = 180 / pi;  %  radians to degrees
rth = rtd * 24 / 360;  %  radians to hours
htd = 360 / 24;  %  hours to degrees

%...assign input file id
fid_in = fopen(Infile);


Nobs = 0;
while( 1 )
    %...read in the record as string
    Nobs = Nobs + 1;
    Record = fgetl(fid_in);
    if ~ischar(Record), break, end
    %fprintf('%s\n',Record)
    %...parse the data
    Classification = Record(1);  %  classification i.d. (string)
    SatID = str2double(Record(2:6));  %  satellite i.d. (integer)
    SensorID = str2double(Record(7:9));  %  sensor i.d. (integer)
    %   year
    YY = str2double(Record(10:11));
    if ( YY < 50 )
        Year = YY + 2000;
    else
        Year = YY + 1900;
    end  %  if ( YY < 50 )
    Doy = str2double(Record(12:14));  %  day of year (int)
    %   hours, minutes, seconds
    [ Month, Day] = doy_2_mmdd( Year, Doy );
    Hour = str2double(Record(15:16));  %  hours (int)
    Minute = str2double(Record(17:18));  %  minutes (int)
    Second = str2double(Record(19:23))/1000;  %  seconds (float)
    %   observation type
    ObsType = str2double(Record(75));  %  observation type (int)
    %   elevation or declination
    EDs24 = Record(24);
    EDs = Record(25:29);
    switch EDs24
        case 'J'
            EleDec = str2double(['-1',EDs])/10000;
        case 'K'
            EleDec = str2double(['-2',EDs])/10000;
        case 'L'
            EleDec = str2double(['-3',EDs])/10000;
        case 'M'
            EleDec = str2double(['-4',EDs])/10000;
        case 'N'
            EleDec = str2double(['-5',EDs])/10000;
        case 'O'
            EleDec = str2double(['-6',EDs])/10000;
        case 'P'
            EleDec = str2double(['-7',EDs])/10000;
        case 'Q'
            EleDec = str2double(['-8',EDs])/10000;
        case 'R'
            EleDec = str2double(['-9',EDs])/10000;
        otherwise
            EleDec = str2double([EDs24,EDs])/10000;
    end
    %   azimuth or right ascension
    if ( ObsType == 1 || ObsType == 2 || ObsType == 3 || ObsType == 4 || ObsType == 8 )
        AziRta = str2double(Record(31:37))/10000;  %  decimal degrees
    elseif( ObsType == 5 || ObsType == 9 )
        AziRta = htd * ( str2double(Record(31:32)) + str2double(Record(33:34))/60 + str2double(Record(35:37))/36000 );  %  decimal degrees
    end
    %   range
    RngExponent = str2double(Record(46));
    Range = (str2double(Record(39:45))/10000)*10^RngExponent;
    %   other obs types
    if ( ObsType == 8 || ObsType == 9 )
        EcfX = str2double(Record(47:55));  %  ecf x position (meters)
        EcfY = str2double(Record(56:64));  %  ecf y position (meters)
        EcfZ = str2double(Record(65:73));  %  ecf z position (meters)
    else
        RngRate = str2double(Record(48:54))/100000;  %  range-rate (km/s)
    end  %  if ( ObsType == 8 || ObsType == 9 )
    
    obs(Nobs,1:8) = [ Year, Month, Day, Hour, Minute, Second, AziRta/rtd, EleDec/rtd ];
            
end  %  while( 1 )

%...close the input file i.d.
fclose(fid_in);
%...close the output file i.d.
%%fclose(fid_out);

%...end of script Obs2B3.m