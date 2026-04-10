function [y,Htilde] = GroundStationRange(Sat,GroundStation)

GSposvec = [GroundStation.X GroundStation.Y GroundStation.Z]';

R_IF     = eye(3);

rv       = [Sat.X Sat.Y Sat.Z]' - R_IF*GSposvec;

y        = sqrt(rv'*rv);
Htilde   = rv'/y*eye(3); 