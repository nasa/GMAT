function EA=DKEPLR(M,E)
%	  SUBROUTINE TO SOLVE KEPLER'S EQ.
% KEPLER'S EQ.,RELATES GEOMETRY OR POSITION IN ORBIT PLANE TO TIME.\
%M - MEAN ANOMALY (0<M<2PI)
%E - ECCENTRICITY
%EA- ECCENTRIC ANOMALY
PI2=2*pi;
TOL=0.5E-15;
EA=0;
maxiter=500;
if M~=0 
    EA=M + E*sin(M);
    DELEA=1;
    i=0;
	while DELEA >=TOL %TEST FOR CONVERGENCE
        OLDEA=EA;
        FE=EA-E*sin(EA)-M;
        EA=EA-FE/(1-E*cos(EA-0.5*FE));
        DELEA=abs(EA-OLDEA);
        i=i+2;
        if i > maxiter
            break
        end
	end
end

EA=mod(EA,PI2);
