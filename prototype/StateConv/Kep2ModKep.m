function [modkepel] = Kep2ModKep(kepel)

if isempty(kepel)
    modkepel = [];
    return
end
%  This function converts from m keplerian elements to  modified keplerian
%  elements.

SMA  = kepel(1);
ECC  = kepel(2);
INC  = kepel(3);
RAAN = kepel(4);
AOP  = kepel(5);
TA   = kepel(6);



%  Sanity check input
if ( ECC < 0 )
    modkepel = [];
    disp('Warning: ECC must be greater than 0');
    return
end
%  Signs on SMA and ECC are not consistent
if ( ECC > 1 & SMA > 0)
    modkepel = [];
    disp('Warning: If ECC > 1, SMA must be negative');
    return
end
%  Exaclty Parabolic orbit
if ( ECC == 1 )
    modkepel = [];
    disp('Warning: Parabolic orbits cannot be entered in Keplerian or Modified Keplerian format');
    return
end
%  Parabolic orbit to machine precision
if (abs(ECC - 1) < 2*eps)
    disp('Warning:  Orbit is nearly parabolic and state conversion routine is near numerical singularity');
end
%  SMA is infinite
if ( SMA == inf)
    modkepel = [];
    disp('Warning: Parabolic orbits cannot be entered in Keplerian or Modified Keplerian format');
    return
end

RadPer = SMA*( 1 - ECC );
RadApo = SMA*( 1 + ECC );

modkepel = [RadPer RadApo INC RAAN AOP TA];