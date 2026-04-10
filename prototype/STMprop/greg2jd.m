function JD=UT2JD(date)

%  JD=UT2JD(date) Calculates the Julian Date (days) given the time
%  Variable I/O
%     date(1)=4 digit year
%     date(2)=month
%     date(3)=day
%     date(4)=hour
%     date(5)=minute
%     date(6)=seconds

if ischar(date)
    date = datevec(date);
end
    
yr = date(1);mo=date(2);d=date(3);h=date(4);min=date(5);s=date(6);

JD = 367*yr-fix( 7*(yr+fix((mo+9)/12))/4 ) + fix(275*mo/9)+d+1721013.5...
     +((s/60+min)/60+h)/24;
   