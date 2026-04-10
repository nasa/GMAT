function jdate = julian_mod(year,month,day,hour,minute,second)

% Purpose:  Comput the Julian date

% Input:

%  month = calendar month [1 - 12]
%  day   = calendar day [1 - 31]
%  year  = calendar year [yyyy]
%  hour  = hour in 24 hour format
%  minute & second (self explanatory)

% Output:

%  jdate = Julian date

% special notes

%  (1) calendar year must the 4 digit format (more digits if the world
%      survives that long)

%  (2) Algorithm is valid between March, 1 1900 to February, 28 2100
%      

% From Vallado: Fundamentals of Atrodynamics and Applications (pp. 67)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
format long g

jdate = (367*year) - floor((7*(year+floor((month+9)/12)))/4) +...
         floor((275*month)/9) + day + 1721013.5 +...
         (((((second/60) + minute)/60)+hour)/24);

%...end of function jdate = julian_mod(year,month,day,hour,minute,second)