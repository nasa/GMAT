
%  --------- WHAT THIS PROTOTYPE DOES

This prototype propagates an orbit and state transition matrix (STM).
As of this writing, you must use the GMAT script syntax to run the
software.  You must provide the spacecraft state in cartesian form,
in MJ2000Eq axes, and w/r/t the central body of the force model.  
You must provide the epoch in UTCGregorian format.  

The force model currently supports any planet (or Earth's moon) as the
central body of propagation, supports point mass gravity from any gouping
of planets (and Earth's moon), and supports SRP (without shadow model yet).

This is the first checkin so I can begin to CM this prototype.  There
are a lot of rough edges, but the numerics agree very well
with GMAT, and the STM propagation appears to be working correctly for
point mass and SRP perturbations.  

%  --------- WHAT YOU NEED TO GET FOR THIS PROTOTYPE TO RUN

This prototype requires a free DE405 file reader not contained in
this distibution due to licensing issues.  To obtain the reader
go here:
http://openchannelsoftware.com/orders/index.php?group_id=333

%  --------- WHAT FURTHER DEVELOPMENT IS REQUIRED
-   Remove hard coded spacecraft properties from deriv_SRP.m
-   Clean up dynfunc.m
-   Add nonspherical gravity to forcemodel
-   Add exponential atmosphere to force model

%  --------- CREDITS

%  This prototype uses a modified version of an ode78 solver
written by Daljeet Singh available here in original form:
http://www.ccr.jussieu.fr/ccr/Documentation/Calcul/matlab5v11/docs/ftp.mathworks.com/pub/contrib/v4/diffeq/ode78.m



