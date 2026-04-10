function [] = AutoNGCLOSKPlotFunction_MultiPhase(times,controlVec,stateVec,traj)

%-------------------------------------------------------------------------%
% Setup %
%-------------------------------------------------------------------------%

% Define constants used in plotting
RE = 1738.2;
gravParam = 4902.8005821478;

% assume all phases have the same scaling for now
TU = traj.phaseList{1}.orbitScaleUtil.TU;
MU = traj.phaseList{1}.orbitScaleUtil.MU;


% Call global variables
global config

%-------------------------------------------------------------------------%
% Figure 2: Control Profile %
%-------------------------------------------------------------------------%

h = figure(2);
set(h,'color','w');

for phaseIdx = 1:length(traj.phaseList)
    
    times = traj.GetTimeArray(phaseIdx);
    times = times * TU / 86400; % unscale then convert to days
    stateVec = traj.GetStateArray(phaseIdx);
    controlVec = traj.GetControlArray(phaseIdx);
    
    % Plot control magnitude
    subplot(3,1,1); hold on;
    plot( times(1:end-1) , controlVec(:,1) );
    ylabel('\bf Control Magnitude', 'interpreter', 'latex');
    %axis([min(times) max(times) 0 1]);
    
    % Plot components of control vector
    subplot(3,1,2); hold on;
    plot(times(1:end-1),controlVec(:,2),times(1:end-1),controlVec(:,3),times(1:end-1),controlVec(:,4))
    ylabel('\bf Control Unit Direction', 'interpreter', 'latex');
    l = legend('Radial', 'In-Track', 'Cross-Track', 'location', 'southeast');
    set(l, 'interpreter', 'latex');
    axis tight;

    % plot mass
    subplot(3,1,3);
    hold on
    plot( times(1:end) , stateVec(:,7)*MU );
    ylabel('\bf Mass [kg]', 'interpreter', 'latex');
    xlabel('\bf Time [days]', 'interpreter', 'latex');
    
end

% Display selected parameters of final transfer trajectory
disp('Last Time In Days')
disp(times(end)*TU/86400)
disp('Final MLONG, Rad')
disp(stateVec(end,6))

for phaseIdx = 1:length(traj.phaseList)
    
    times = traj.GetTimeArray(phaseIdx);
    times = times * TU / 86400; % unscale then convert to days
    stateVec = traj.GetStateArray(phaseIdx);
    controlVec = traj.GetControlArray(phaseIdx);
    
    % Preallocate arrays for storing results
    cartState = zeros(size(stateVec,1),6);
    kepState = zeros(size(stateVec,1),6);
    % Loop through results and calculate Cartesian states and Keplerian orbital
    % elements for each point
    for iterIdx = 1:size(stateVec,1)
        %dimMEE = config.StateUnScaleMat*stateVec(iterIdx,:)';
        dimMEE = traj.phaseList{phaseIdx}.orbitScaleUtil.UnScaleModEqFullState(stateVec(iterIdx,:));
        initState = Mee2Cart(dimMEE(1:6),gravParam);
        cartState(iterIdx,:) = Mee2Cart(dimMEE(1:6),gravParam);
        %     kepState(iterIdx,:) = cart2oe(initState,gravParam);
        kepState(iterIdx,:) = Cart2Kep(initState,gravParam);
    end
    
    %-------------------------------------------------------------------------%
    % Figure 3: Keplerian Orbital Element Time History %
    %-------------------------------------------------------------------------%
    
    h = figure(3);
    set(h,'color','w');
    
    % Plot SMA
    subplot(3,2,1); hold on;
    plot(times,kepState(:,1));
    grid on
    ylabel('\bf SMA [km]', 'interpreter', 'latex')
    xlabel('\bf Time [days]', 'interpreter', 'latex')
    
    % Plot ECC
    subplot(3,2,2)
    plot(times,kepState(:,2));hold on;
    grid on
    ylabel('\bf Ecc, dimless', 'interpreter', 'latex')
    xlabel('\bf Time [days]', 'interpreter', 'latex')
    
    % Plot INC
    subplot(3,2,3)
    plot(times,kepState(:,3)*180/pi);hold on;
    grid on
    ylabel('\bf Inclination [deg]', 'interpreter', 'latex')
    xlabel('\bf Time [days]', 'interpreter', 'latex')
    
    % Plot AOPhold on;
    subplot(3,2,4);hold on;
    plot(times,kepState(:,5)*180/pi);
    grid on
    ylabel('\bf AOP [deg]', 'interpreter', 'latex')
    xlabel('\bf Time [days]', 'interpreter', 'latex')
    
    % Plot RAAN
    subplot(3,2,5);hold on;
    meanRAAN = mean(kepState(:,4));hold on;
    raan = zeros(size(kepState(:,4)));
    for i = 1:size(kepState(:,4))
        raan(i,1) = kepState(i,4);
        raanDiff = raan(i,1) - meanRAAN;
        if raanDiff  > pi
            raan(i,1) = raan(i) - 2*pi;
        elseif raanDiff < -pi
            raan(i,1) = raan(i) + 2*pi;
        end
    end
    plot(times,raan*180/pi);
    grid on
    ylabel('\bf RAAN [deg]', 'interpreter', 'latex')
    xlabel('\bf Time [days]', 'interpreter', 'latex')
    
    % Plot TA
    subplot(3,2,6)
    plot(times,kepState(:,6)*180/pi);hold on;
    ylabel('\bf TA [deg]', 'interpreter', 'latex')
    xlabel('\bf Time [days]', 'interpreter', 'latex')
    grid on
    
    %-------------------------------------------------------------------------%
    % Figure 4: 3D Cartesian Plot %
    %-------------------------------------------------------------------------%
    
    h = figure(4);
    set(h,'color','w');
    plot3(cartState(:,1), cartState(:,2), cartState(:,3));
    xlabel('\bf X [km]', 'interpreter', 'latex')
    ylabel('\bf Y [km]', 'interpreter', 'latex')
    zlabel('\bf Z [km]', 'interpreter', 'latex')
    axis equal
    
    %-------------------------------------------------------------------------%
    % Compute and Print Low-Thrust Transfer Summary
    %-------------------------------------------------------------------------%
    
    % Calculate number of segments/intervals in the phase
    sz_controlVec = size(controlVec);
    n_segs = sz_controlVec(1);
    
    % Determine total time thrusting
    timeThrust_days = 0;
    timeCoast_days = 0;
    thrust_tol = 1e-6;
    for ii = 1:n_segs
        % Define current thrust magnitude
        Tmag = controlVec(ii,1);
        
        % Check if thrust magnitude is significantly greater than zero
        if Tmag > thrust_tol % if thrusting
            timeThrust_days = timeThrust_days + diff(times(ii:ii+1));
        else % if not thrusting
            timeCoast_days = timeCoast_days + diff(times(ii:ii+1));
        end
    end
    
    % Define initial and final dimensional mass
    initM_dim = stateVec(1,7)*MU;
    finM_dim = stateVec(end,7)*MU;
    
    % Compute Change in Mass
    deltM_dim = initM_dim - finM_dim;
    
    % Calculate acceleration due to gravity on the surface of the Earth
    % g0 = gravParam/RE^2; % [km/sec^2], calculate g from other simulation parameters
    g0 = 9.80665; % [m/sec^2], use standard value of g
    
    % Calculate the exhaust velocity of the low-thrust spacecraft
    vExhaust_dim = 2831*g0; % [m/s]
    
    % Compute the delta-V corrsponding to the change in mass based on the
    % defined spacecraft characteristics
    deltV_dim = vExhaust_dim*log(initM_dim/finM_dim);
    
    fprintf('\n')
    fprintf('==============================================================\n')
    fprintf('Low-Thrust Transfer Problem Summary\n')
    fprintf('==============================================================\n')
    fprintf('Number of Segments = %d \n',n_segs)
    fprintf('Number of Mesh Refinements = %d \n',traj.GetMeshRefinementCount())
    fprintf('Initial Dimensional Mass = %f kg \n',initM_dim)
    fprintf('Final Dimensional Mass = %f kg \n',finM_dim)
    fprintf('Dimensional Change in Mass = %f kg \n',deltM_dim)
    fprintf('Dimensional Change in Velocity = %f m/s \n',deltV_dim)
    fprintf('Dimensional Total Transfer Time = %.16f days \n',times(end))
    fprintf('Dimensional Total Thrust Time = %f days \n',timeThrust_days)
    fprintf('Dimensional Total Coast Time = %f days \n',timeCoast_days)
    fprintf('Maximum Available Acceleration = %e m/s^2 \n',3/initM_dim)
    fprintf('Initial Unscaled Cartesian state:\n')
    fprintf('%.8f, %.8f, %.8f, %.8f, %.8f, %.8f \n',cartState(1,1:6))
    fprintf('Final Unscaled Cartesian state:\n')
    fprintf('%.8f, %.8f, %.8f, %.8f, %.8f, %.8f \n',cartState(end,1:6))
    fprintf('==============================================================\n')
    
end
