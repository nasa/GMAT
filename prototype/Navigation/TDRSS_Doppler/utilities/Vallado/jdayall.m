% ------------------------------------------------------------------------------
%
%                           function jdayall
%
%  this function finds the julian date given the year, month, day, and time.
%    the julian date is defined by each elapsed day since noon, jan 1, 4713 bc.
%
%  author        : david vallado                  719-573-2600   27 may 2002
%
%  revisions
%                -
%
%  inputs          description                    range / units
%    year        - year                           1900 .. 2100
%    mon         - month                          1 .. 12
%    day         - day                            1 .. 28,29,30,31
%    hr          - universal time hour            0 .. 23
%    min         - universal time min             0 .. 59
%    sec         - universal time sec             0.0 .. 59.999
%    whichtype   - julian or gregorian calender   'j' or 'g'
%
%  outputs       :
%    jd          - julian date                    days from 4713 bc
%
%  locals        :
%    b           - var to aid gregorian dates
%
%  coupling      :
%    none.
%
%  references    :
%    vallado       2001, 187
%
% jd = jdayall(year, mon, day, hr, min, sec, whichtype);
% -----------------------------------------------------------------------------

function jd = jdayall(year, mon, day, hr, min, sec, whichtype);

        if mon <= 2
            year= year - 1;
            mon = mon + 12;
          end;

        if whichtype == 'j'
            % --------- use for julian calender, every 4 years --------
            b= 0.0;
          else
            % ---------------------- use for gregorian ----------------
            b= 2 - floor(year*0.01) + floor(floor(year*0.01)*0.25);
          end

        jd= floor( 365.25*(year + 4716) ) ...
             + floor( 30.6001*(mon+1) ) ...
             + day + b - 1524.5 ...
             + ( (sec/60.0 + min ) / 60.0 + hr ) / 24.0;  % ut in days

