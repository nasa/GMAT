% ------------------------------------------------------------------------------
%
%                           function convtime
%
%  this function finds the time parameters and julian century values for inputs
%    of utc or ut1. numerous outputs are found as shown in the local variables.
%    because calucations are in utc, you must include timezone if ( you enter a
%    local time, otherwise it should be zero.
%
%  author        : david vallado                  719-573-2600    4 jun 2002
%
%  revisions
%    vallado     - add tcg, tcb, etc                              6 oct 2005
%    vallado     - fix documentation for dut1                     8 oct 2002
%
%  inputs          description                    range / units
%    year        - year                           1900 .. 2100
%    mon         - month                          1 .. 12
%    day         - day                            1 .. 28,29,30,31
%    hr          - universal time hour            0 .. 23
%    min         - universal time min             0 .. 59
%    sec         - universal time sec (utc)            0.0  .. 59.999
%    timezone    - offset to utc from local site  0 .. 23 hr
%    dut1        - delta of ut1 - utc             sec
%    dat         - delta of tai - utc             sec
%
%  outputs       :
%    ut1         - universal time                 sec
%    tut1        - julian centuries of ut1
%    jdut1       - julian date of ut1             days from 4713 bc
%    utc         - coordinated universal time     sec
%    tai         - atomic time                    sec
%    tdt         - terrestrial dynamical time     sec
%    ttdt        - julian centuries of tdt
%    jdtdt       - julian date of tdt             days from 4713 bc
%    tdb         - terrestrial barycentric time   sec
%    ttdb        - julian centuries of tdb
%    jdtdb       - julian date of tdb             days from 4713 bc
%    tcb         - celestial barycentric time     sec
%    tcg         - celestial geocentric time      sec
%    jdtdb       - julian date of tcg             days from 4713 bc
%
%  locals        :
%    hrtemp      - temporary hours                hr
%    mintemp     - temporary minutes              min
%    sectemp     - temporary seconds              sec
%    localhr     - difference to local time       hr
%    jd          - julian date of request         days from 4713 bc
%    me          - mean anomaly of the earth      rad
%
%  coupling      :
%    hms_2_sec   - conversion between hr-min-sec .and. seconds
%    jday        - find the julian date
%
%  references    :
%    vallado       2007, 201, alg 16, ex 3-7
%
% [ut1, tut1, jdut1, utc, tai, tt, ttt, jdtt, tdb, ttdb, jdtdb, tcg, jdtcg, tcb, jdtcb ] ...
% = convtime ( year, mon, day, hr, min, sec, timezone, dut1, dat );
% ------------------------------------------------------------------------------

function [ut1, tut1, jdut1, utc, tai, tt, ttt, jdtt, tdb, ttdb, jdtdb, tcg, jdtcg, tcb, jdtcb ] ...
         = convtime ( year, mon, day, hr, min, sec, timezone, dut1, dat );

        deg2rad = pi/180.0;

        % ------------------------  implementation   ------------------
        jd =  jday( year,mon,day,0,0,0.0 );
        mjd  = jd - 2400000.5;
        mfme = hr*60.0 + min + sec/60.0;

        % ------------------ start if ( ut1 is known ------------------
        localhr= timezone + hr;

        utc = hms2sec( localhr,min,sec );

        ut1= utc + dut1;
        [hrtemp,mintemp,sectemp] = sec2hms(  ut1 );
        jdut1 = jday( year,mon,day, hrtemp, mintemp, sectemp );
        tut1= (jdut1 - 2451545.0  )/ 36525.0;

        tai= utc + dat;
        [hrtemp,mintemp,sectemp] = sec2hms(  tai );
        jdtai = jday( year,mon,day, hrtemp, mintemp, sectemp );

        tt= tai + 32.184;   % sec
        [hrtemp,mintemp,sectemp] = sec2hms( tt );
        jdtt = jday( year,mon,day, hrtemp, mintemp, sectemp);
        ttt= (jdtt - 2451545.0  )/ 36525.0;

%%%%%%%%%%%%%%%%%%%%%% tdb
% vallado approach (extra digits)
%        me= 357.5277233  + 35999.05034 *ttt;
%        me= modulo( me,360.0  );
%        me= me * deg2rad;
%        tdb= tt + 0.001658  * sin(me) + 0.00001385 *sin(2.0 *me);
%        [hrtemp,mintemp,sectemp] = sec2hms( tdb );
%        jdtdb = jday( year,mon,day, hrtemp, mintemp, sectemp );
%        ttdb= (jdtdb - 2451545.0  )/ 36525.0;
% std approach (digits)
%        me= 357.53  + 0.9856003 * (jdtt - 2451545.0);   
%        me= modulo( me,360.0  );
%        me= me * deg2rad;
%        tdb1= tt + 0.001658  * sin(me) + 0.000014 *sin(2.0 *me);
%        [hrtemp,mintemp,sectemp] = sec2hms( tdb1 );
%        jdtdb1 = jday( year,mon,day, hrtemp, mintemp, sectemp );
%        ttdb1= (jdtdb1 - 2451545.0  )/ 36525.0;
% ast alm approach (2006)
%        me= 357.53  + 0.98560028 * (jdtt - 2451545.0);   
%        me= modulo( me,360.0  );
%        me= me * deg2rad;
%        dlje = 246.11 + 0.90255617*(jdtt - 2451545.0);
%        tdb2= tt + 0.001658  * sin(me) + 0.000021 *sin(dlje);
%        [hrtemp,mintemp,sectemp] = sec2hms( tdb2 );
%        jdtdb2 = jday( year,mon,day, hrtemp, mintemp, sectemp );
%        ttdb2 = (jdtdb2 - 2451545.0  )/ 36525.0;
% usno circular approach 
        tdb = tt + 0.001657*sin(628.3076*ttt+6.2401) ...
               + 0.000022*sin(575.3385*ttt+4.2970) ...
               + 0.000014*sin(1256.6152*ttt+6.1969) ...
               + 0.000005*sin(606.9777*ttt+4.0212) ...
               + 0.000005*sin(52.9691*ttt+0.4444) ...
               + 0.000002*sin(21.3299*ttt+5.5431) ...
               + 0.000010*ttt*sin(628.3076*ttt+4.2490);  % USNO circ (14)
        [hrtemp,mintemp,sectemp] = sec2hms( tdb );
        jdtdb = jday( year,mon,day, hrtemp, mintemp, sectemp );
        ttdb = (jdtdb - 2451545.0  )/ 36525.0;

%        fprintf(1,'tdb %8.6f ttdb  %16.12f jdtdb  %18.11f ',tdb,ttdb,jdtdb );
        [h,m,s] = sec2hms( tdb );
%        fprintf(1,'hms %3i %3i %8.6f \n',h,m,s);
        
        % 
%%%%%%%%%%%%%%%%%%%%%% tcg
% approx with tai
        tcg = tt + 6.969290134e-10*(jdtai - 2443144.5)*86400.0;  % AAS 05-352 (10) and IERS TN (104)
        [hrtemp,mintemp,sectemp] = sec2hms( tcg );
        jdtcg = jday( year,mon,day, hrtemp, mintemp, sectemp );
        tt2 = tcg-6.969290134e-10*(jdtcg-2443144.5003725)*86400.0;

%        fprintf(1,'tcg %8.6f jdtcg  %18.11f ',tcg,jdtcg );
        [h,m,s] = sec2hms( tcg );
%        fprintf(1,'hms %3i %3i %8.6f \n',h,m,s);        
        
% binomial approach with days
%        lg=6.969290134e-10*86400.0;
%        tcg1 = tt + (jdtt - 2443144.5003725)*(lg + lg*lg + lg*lg*lg);
% days from 77
%        jdttx = jday( year,mon,day, 0, 0, 0.0); 
%        ttx = tt/86400.0 + jdttx-2443144.5003725  % days from the 1977 epoch
%        tcg2 = (jdttx - 6.969290134e-10*2443144.5003725) / (1.0 - 6.969290134e-10) % days
%        tcg2 = (tcg2 - jdttx)*86400*86400;
% sec from 77
%        ttx = tt + (jdttx-2443144.5003725)*86400.0;  % s from the 1977 epoch
%        tcg3 = ttx / (1.0 - 6.969290134e-10); % s
%        tcg3 = tcg3 -(jdttx-2443144.5003725)*86400.0;
% check with tcg
%        tcg4 = tt + 6.969290134e-10*(jdtcg - 2443144.5003725)*86400.0;  % AAS 05-352 (10) and IERS TN (104)
%        [hrtemp,mintemp,sectemp] = sec2hms( tcg4 );
%        jdtcg4 = jday( year,mon,day, hrtemp, mintemp, sectemp );
%        tt2 = tcg4-6.969290134e-10*(jdtcg4-2443144.5003725)*86400.0;
%        difchk = tt2-tt
        
        
        tcbmtdb = 1.55051976772e-8*(jdtai - 2443144.5)*86400.0;  % sec, value for de405 AAS 05-352 (10) and IERS TN (104)?
        tcb = tdb + tcbmtdb;
        [hrtemp,mintemp,sectemp] = sec2hms( tcb );
        jdtcb = jday( year,mon,day, hrtemp, mintemp, sectemp );
%        fprintf(1,'tcb %8.6f jdtcb  %18.11f ',tcb,jdtcb );
        [h,m,s] = sec2hms( tcb );
%        fprintf(1,'hms %3i %3i %8.6f \n',h,m,s);
        
%        fprintf(1,'ut1 %13.10f tut1 %16.12f jdut1 %18.11f ',ut1,tut1,jdut1 );
        [h,m,s] = sec2hms( ut1 );
%        fprintf(1,'hms %3i %3i %9.6f \n',h,m,s);
%        fprintf(1,'utc %13.10f ',utc );
        [h,m,s] = sec2hms( utc );
%        fprintf(1,'%47s hms %3i %3i %9.6f \n',' ',h,m,s);
%        fprintf(1,'tai %13.10f ',tai );
        [h,m,s] = sec2hms( tai );
%        fprintf(1,'%47s hms %3i %3i %9.6f \n',' ',h,m,s);
%        fprintf(1,'tt  %13.10f ttt  %16.12f jdtt  %18.11f ',tt,ttt,jdtt );
        [h,m,s] = sec2hms( tt );
%        fprintf(1,'hms %3i %3i %9.6f \n\n',h,m,s);
%        fprintf(1,'tdb %13.10f ttdb %16.12f jdtdb %18.11f',tdb,ttdb,jdtdb );
        [h,m,s] = sec2hms( tdb );
%        fprintf(1,' hms %3i %3i %9.6f vallado ext digits\n',h,m,s);
%        fprintf(1,'tdb %13.10f ttdb %16.12f jdtdb %18.11f',tdb1,ttdb1,jdtdb1 );
%        [h,m,s] = sec2hms( tdb1 );
%        fprintf(1,' hms %3i %3i %9.6f std digits \n',h,m,s);
%        fprintf(1,'tdb %13.10f ttdb %16.12f jdtdb %18.11f',tdb2,ttdb2,jdtdb2 );
%        [h,m,s] = sec2hms( tdb2 );
%        fprintf(1,' hms %3i %3i %9.6f ast alm \n',h,m,s);
%        fprintf(1,'tdb %13.10f ttdb %16.12f jdtdb %18.11f',tdb3,ttdb3,jdtdb3 );
%        [h,m,s] = sec2hms( tdb3 );
%        fprintf(1,' hms %3i %3i %9.6f series \n\n',h,m,s);

%        fprintf(1,'tcg %13.10f',tcg );
        [h,m,s] = sec2hms( tcg );
%        fprintf(1,'%48s hms %3i %3i %9.6f tai method\n',' ',h,m,s);
%        fprintf(1,'tcg %13.10f',tcg1 );
%        [h,m,s] = sec2hms( tcg1 );
%        fprintf(1,'%48s hms %3i %3i %9.6f binomial\n',' ',h,m,s);
%        fprintf(1,'tcg %13.10f',tcg2 );
%        [h,m,s] = sec2hms( tcg2 );
%        fprintf(1,'%48s hms %3i %3i %9.6f days from 77\n',' ',h,m,s);
%        fprintf(1,'tcg %13.10f',tcg3 );
%        [h,m,s] = sec2hms( tcg3 );
%        fprintf(1,'%48s hms %3i %3i %9.6f secs from 77\n\n',' ',h,m,s);

        
%        fprintf(1,'tcb %13.10f',tcb );
        [h,m,s] = sec2hms( tcb );
%        fprintf(1,'%48s hms %3i %3i %9.6f \n',' ',h,m,s);

%[ut1, tut1, jdut1, utc, tai, tt, ttt, jdtt, tdb, ttdb, jdtdb, tcg, jdtcg, tcb, jdtcb ] = convtime(2000, 1, 1, 12, 0, 0.0, 0, 0, 32 );
%[ut1, tut1, jdut1, utc, tai, tt, ttt, jdtt, tdb, ttdb, jdtdb, tcg, jdtcg, tcb, jdtcb ] = convtime(2004, 4, 6, 7, 51, 28.386009, 0, -0.4399620, 32 );
