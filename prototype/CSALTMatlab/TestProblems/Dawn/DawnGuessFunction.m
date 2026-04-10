function [stateGuess,controlGuess] = DawnGuessFunction(timeVector,timeTypeVec,numControls)

global config
	
maxFlightTime = 2650*86400/config.TU;
InitialTime = 45*86400/config.TU;

InitialState = config.StateScaleMat*[ 1.497041343587040E+08;  8.183852954373193E+06;  3.547375086237265E+06; -2.256114916548608E+00;  2.716808234523704E+01;  1.177708598889056E+01; 2000];

FinalState   = config.StateScaleMat*[-6.250395787524774E+07; -3.821450537476053E+08; -1.674063563340448E+08;  1.684252437284989E+01; -2.217791783978667E+00; -4.476657880003078E+00; 1000];
% FinalState   = config.StateScaleMat*[ 3.360857875235767E+08; -2.822963445710570E+08; -7.082550987772526E+07;  1.068366733882287E+01;  1.264566082911572E+01; -1.571979708066422E+00; 1000];

initialH = cross(InitialState(1:3),InitialState(4:6));
finalH = cross(FinalState(1:3),FinalState(4:6));
initialHmag = norm(initialH);
finalHmag = norm(finalH);

dH = finalHmag - initialHmag;
dH_dTime = dH / maxFlightTime;

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

nRevs = 3;

if finalTheta < 0
	finalTheta = finalTheta + 2*pi;
end

finalRevStart = initialTheta + 2*pi*nRevs;
finalTheta    = finalTheta + 2*pi*nRevs;

if finalTheta < finalRevStart
	finalTheta = finalTheta + 2*pi;
end

deltaTheta = finalTheta - initialTheta;

dTheta_dTime = deltaTheta/maxFlightTime;

nPlot = 100;

dTime = maxFlightTime/nPlot;

dR = endR-startR;

dR_dTime = dR / maxFlightTime;

initialPhi = acos(InitialState(3)/startR);
finalPhi = acos(FinalState(3)/endR);

dPhi = finalPhi - initialPhi;

dPhi_dTime = dPhi / maxFlightTime;

initialV = norm(InitialState(4:6));
finalV = norm(FinalState(4:6));
dV = finalV - initialV;
dV_dTime = dV / maxFlightTime;

initialMass = InitialState(7);
finalMass = FinalState(7);

dMass = finalMass - initialMass;
dMass_dTime = dMass / maxFlightTime;

fac = 1;

% [m,n] = size(timeVector);

% if n>m
% 	timeVector = timeVector';
% end

Rguess = startR + dR_dTime * (timeVector-InitialTime);
theta_guess = initialTheta + dTheta_dTime * (timeVector-InitialTime);
phi_guess = initialPhi + dPhi_dTime * (timeVector-InitialTime);
vTheta_guess = theta_guess + pi/2;
vMag_guess = initialV + dV_dTime * (timeVector-InitialTime);
mass_guess = initialMass + dMass_dTime * (timeVector-InitialTime);

if config.method == 1
    stateGuess = [Rguess.*sin(phi_guess).*cos(theta_guess),Rguess.*sin(phi_guess).*sin(theta_guess),Rguess.*cos(phi_guess),vMag_guess.*sin(phi_guess).*cos(vTheta_guess),vMag_guess.*sin(phi_guess).*sin(vTheta_guess),vMag_guess.*cos(phi_guess),mass_guess];
else
    stateGuess = [Rguess.*sin(phi_guess).*cos(theta_guess);Rguess.*sin(phi_guess).*sin(theta_guess);Rguess.*cos(phi_guess);vMag_guess.*sin(phi_guess).*cos(vTheta_guess);vMag_guess.*sin(phi_guess).*sin(vTheta_guess);vMag_guess.*cos(phi_guess);mass_guess];
end

last_phi = -1;
last_theta = -1;

if config.method == 1
    end_val = length(timeVector)-1;
else
    end_val = length(timeVector);
end

for i = 1:length(timeVector)
    
    if config.method == 1
        stateGuess(i,1:3) = (RotZ(-xy_angle)*RotX(angle2up)*RotZ(xy_angle)*stateGuess(i,1:3)');
        stateGuess(i,4:6) = (RotZ(-xy_angle)*RotX(angle2up)*RotZ(xy_angle)*stateGuess(i,4:6)');
        unitVel = stateGuess(i,4:6)/norm(stateGuess(i,4:6));
    else
        stateGuess(1:3,i) = (RotZ(-xy_angle)*RotX(angle2up)*RotZ(xy_angle)*stateGuess(1:3,i));
        stateGuess(4:6,i) = (RotZ(-xy_angle)*RotX(angle2up)*RotZ(xy_angle)*stateGuess(4:6,i));
        unitVel = stateGuess(4:6,i)/norm(stateGuess(4:6,i));
    end

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

    if config.method == 1
        if i <= end_val
            if strcmp(config.Control,'Spherical')
                controlGuess(i,1:3) = [theta;phi;norm(unitVel)];
            elseif strcmp(config.Control,'Cartesian')
                controlGuess(i,1:3) = unitVel;
            end
        end
    else
        if strcmp(config.Control,'Spherical')
            controlGuess(1:3,i) = [theta;phi;norm(unitVel)];
        elseif strcmp(config.Control,'Cartesian')
            controlGuess(1:3,i) = unitVel;
        end

    end
	
end

figure(1); hold on;



if config.method == 1
    plot3(stateGuess(:,1),stateGuess(:,2),stateGuess(:,3),'b')
%     stateGuess = stateGuess';
%     controlGuess = controlGuess';
else
    plot3(stateGuess(1,:),stateGuess(2,:),stateGuess(3,:),'b')
    stateGuess = stateGuess';
    controlGuess = controlGuess';
end
% for i = 1:length(timeVector)-1
% 	% ux = controlGuess(i,3) * sin(controlGuess(i,1)) * cos(controlGuess(i,2));
% 	% uy = controlGuess(i,3) * sin(controlGuess(i,1)) * sin(controlGuess(i,2));
% 	% uz = controlGuess(i,3) * cos(controlGuess(i,1));
% 	ux = controlGuess(i,1);
% 	uy = controlGuess(i,2);
% 	uz = controlGuess(i,3);
%     % plot3([stateGuess(i,1) stateGuess(i,1)+ux*fac],[stateGuess(i,2) stateGuess(i,2)+uy*fac],[stateGuess(i,3) stateGuess(i,3)+uz*fac],'r')
%     % plot3([stateGuess(i,1) stateGuess(i,1)+stateGuess(i,4)*fac],[stateGuess(i,2) stateGuess(i,2)+stateGuess(i,5)*fac],[stateGuess(i,3) stateGuess(i,3)+stateGuess(i,6)*fac],'r')
% end
% disp('guessed')
% disp('new phase')
% sum(sum(isnan(controlGuess)))
% stop
% keyboard