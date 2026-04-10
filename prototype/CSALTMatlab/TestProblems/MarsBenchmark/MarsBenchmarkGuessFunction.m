function [stateGuess,controlGuess] = MarsBenchmarkGuessFunction(timeVector,timeTypeVec,numControls)

global config

InitialState = config.StateScaleMat*[-2.612780114384178E+04;  1.328257092868841E+05;  5.757956041029914E+04; -2.981220584493626E-02; -4.955837697510423E-03; -2.146951323568623E-03; 2.1]*1000;

FinalState   = config.StateScaleMat*[-2.444427441837995E+05;  3.779124150799458E+04;  2.392934774236356E+04; -3.405553850853295E-03; -1.982966469562482E-02; -9.003561269272474E-03; 1.8]*1000;

initialH = cross(InitialState(1:3),InitialState(4:6));
finalH = cross(FinalState(1:3),FinalState(4:6));
initialHmag = norm(initialH);
finalHmag = norm(finalH);

dH = finalHmag - initialHmag;
dH_dTime = dH / config.maxFlightTime;


up = [0,0,1];

angle2up = acos(dot(up,initialH));

rotVec = cross(up,initialH);

xy_angle = atan2(rotVec(2),rotVec(1));

rotVec = rotVec * RotZ(xy_angle);

InitialState(1:3) = (RotZ(-xy_angle)*RotX(-angle2up)*RotZ(xy_angle)*InitialState(1:3));
InitialState(4:6) = (RotZ(-xy_angle)*RotX(-angle2up)*RotZ(xy_angle)*InitialState(4:6));
FinalState(1:3) = (RotZ(-xy_angle)*RotX(-angle2up)*RotZ(xy_angle)*FinalState(1:3));
FinalState(4:6) = (RotZ(-xy_angle)*RotX(-angle2up)*RotZ(xy_angle)*FinalState(4:6));

startR = norm(InitialState(1:3));
endR = norm(FinalState(1:3));

initialTheta = atan2(InitialState(2),InitialState(1));
finalTheta = atan2(FinalState(2),FinalState(1));

nRevs = 1;

if finalTheta < 0
	finalTheta = finalTheta + 2*pi;
end

finalRevStart = initialTheta + 2*pi*nRevs;
finalTheta    = finalTheta + 2*pi*nRevs;

if finalTheta < finalRevStart
	finalTheta = finalTheta + 2*pi;
end

deltaTheta = finalTheta - initialTheta;

dTheta_dTime = deltaTheta/config.maxFlightTime;

nPlot = 100;

dTime = config.maxFlightTime/nPlot;

dR = endR-startR;

dR_dTime = dR / config.maxFlightTime;

initialPhi = acos(InitialState(3)/startR);
finalPhi = acos(FinalState(3)/endR);

dPhi = finalPhi - initialPhi;

dPhi_dTime = dPhi / config.maxFlightTime;

initialV = norm(InitialState(4:6));
finalV = norm(FinalState(4:6));
dV = finalV - initialV;
dV_dTime = dV / config.maxFlightTime;

initialMass = InitialState(7);
finalMass = FinalState(7);

dMass = finalMass - initialMass;
dMass_dTime = dMass / config.maxFlightTime;

fac = 1;

Rguess = startR + dR_dTime * timeVector;
theta_guess = initialTheta + dTheta_dTime * timeVector;
phi_guess = initialPhi + dPhi_dTime * timeVector;
vTheta_guess = theta_guess + pi/2;
vMag_guess = initialV + dV_dTime * timeVector;
mass_guess = initialMass + dMass_dTime * timeVector;

stateGuess = [Rguess.*sin(phi_guess).*cos(theta_guess),Rguess.*sin(phi_guess).*sin(theta_guess),Rguess.*cos(phi_guess),vMag_guess.*sin(phi_guess).*cos(vTheta_guess),vMag_guess.*sin(phi_guess).*sin(vTheta_guess),vMag_guess.*cos(phi_guess),mass_guess];
% controlGuess = [phi_guess(1:end-1),vTheta_guess(1:end-1),ones(length(timeVector)-1,1)];

last_phi = -1;
last_theta = -1;
for i = 1:length(timeVector)
    stateGuess(i,1:3) = (RotZ(-xy_angle)*RotX(angle2up)*RotZ(xy_angle)*stateGuess(i,1:3)')';
    stateGuess(i,4:6) = (RotZ(-xy_angle)*RotX(angle2up)*RotZ(xy_angle)*stateGuess(i,4:6)')';
	
	if i < length(timeVector)
		unitVel = stateGuess(i,4:6)/norm(stateGuess(i,4:6));
		theta = acos(unitVel(3)/norm(unitVel));
		phi = atan2(unitVel(2),unitVel(1));
		if theta < 0
			theta = theta + 2*pi;
		end
		if phi < 0 
			phi = phi + 2*pi;
		end
		if phi < last_phi
			phi = phi + 2*pi;
		end
		last_phi = phi;
		% if theta < last_theta
		% 	theta = theta + 2*pi;
		% end
		% last_theta = theta;
		
		
		
		
		
		% controlGuess(i,1:3) = [theta;phi;norm(unitVel)];
		controlGuess(i,1:3) = unitVel;
	end
	
end


figure(1); hold on;
plot3(stateGuess(:,1),stateGuess(:,2),stateGuess(:,3),'b')

for i = 1:length(timeVector)-1
	% ux = controlGuess(i,3) * sin(controlGuess(i,1)) * cos(controlGuess(i,2));
	% uy = controlGuess(i,3) * sin(controlGuess(i,1)) * sin(controlGuess(i,2));
	% uz = controlGuess(i,3) * cos(controlGuess(i,1));
	ux = controlGuess(i,1);
	uy = controlGuess(i,2);
	uz = controlGuess(i,3);
    plot3([stateGuess(i,1) stateGuess(i,1)+ux*fac],[stateGuess(i,2) stateGuess(i,2)+uy*fac],[stateGuess(i,3) stateGuess(i,3)+uz*fac],'r')
    % plot3([stateGuess(i,1) stateGuess(i,1)+stateGuess(i,4)*fac],[stateGuess(i,2) stateGuess(i,2)+stateGuess(i,5)*fac],[stateGuess(i,3) stateGuess(i,3)+stateGuess(i,6)*fac],'r')
end

% stop