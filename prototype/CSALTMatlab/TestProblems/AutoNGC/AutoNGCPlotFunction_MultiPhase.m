function [] = AutoNGCPlotFunction_MultiPhase(times,controlVec,stateVec,traj)

%-------------------------------------------------------------------------%
% Setup %
%-------------------------------------------------------------------------%

% Define constants used in plotting
RE = 6378.1363;

% Call global variables
global traj config

%-------------------------------------------------------------------------%
% Figure 2: Control Profile %
%-------------------------------------------------------------------------%

h = figure(2);
set(h,'color','w');

for phaseIdx = 1:length(traj.phaseList)
    
    times = traj.GetTimeArray(phaseIdx);
    stateVec = traj.GetStateArray(phaseIdx);
    controlVec = traj.GetControlArray(phaseIdx);
    
    % Plot control magnitude
    subplot(2,1,1); hold on;
    plot( times(1:end-1) , controlVec(:,1) );
    ylabel('\bf Control Magnitude', 'interpreter', 'latex');
    %axis([min(times) max(times) 0 1]);
    
    % Plot components of control vector
    subplot(2,1,2); hold on;
    plot(times(1:end-1),controlVec(:,2),times(1:end-1),controlVec(:,3),times(1:end-1),controlVec(:,4))
    ylabel('\bf Control Unit Direction', 'interpreter', 'latex');
    l = legend('Radial', 'In-Track', 'Cross-Track', 'location', 'southeast');
    set(l, 'interpreter', 'latex');
    axis tight;
    xlabel('\bf Time [days]', 'interpreter', 'latex');
    
end

% Display selected parameters of final transfer trajectory
disp('Last Time In Days')
disp(times(end)*config.TU/86400)
disp('Final MLONG, Rad')
disp(stateVec(end,6))

for phaseIdx = 1:length(traj.phaseList)
    
    times = traj.GetTimeArray(phaseIdx);
    stateVec = traj.GetStateArray(phaseIdx);
    controlVec = traj.GetControlArray(phaseIdx);
    
    % Preallocate arrays for storing results
    cartState = zeros(size(stateVec,1),6);
    kepState = zeros(size(stateVec,1),6);
    % Loop through results and calculate Cartesian states and Keplerian orbital
    % elements for each point
    for iterIdx = 1:size(stateVec,1)
        dimMEE = config.StateUnScaleMat*stateVec(iterIdx,:)';
        initState = Mee2Cart(dimMEE(1:6),398600.4415);
        cartState(iterIdx,:) = Mee2Cart(dimMEE(1:6),398600.4415);
        %     kepState(iterIdx,:) = cart2oe(initState,398600.4415);
        kepState(iterIdx,:) = Cart2Kep(initState,398600.4415);
    end
    
    %-------------------------------------------------------------------------%
    % Figure 3: Keplerian Orbital Element Time History %
    %-------------------------------------------------------------------------%
    
    h = figure(3);
    set(h,'color','w');
    
    % Plot SMA
    subplot(3,2,1); hold on;
    plot(times,kepState(:,1)/RE);
    grid on
    ylabel('\bf SMA [RE]', 'interpreter', 'latex')
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
            timeThrust_days = timeThrust_days + diff(times(ii:ii+1))*config.TU/86400;
        else % if not thrusting
            timeCoast_days = timeCoast_days + diff(times(ii:ii+1))*config.TU/86400;
        end
    end
    
    % Define initial and final dimensional mass
    initM_dim = stateVec(1,7)*config.MU;
    finM_dim = stateVec(end,7)*config.MU;
    
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
    fprintf('Dimensional Total Transfer Time = %f days \n',times(end)*config.TU/86400)
    fprintf('Dimensional Total Thrust Time = %f days \n',timeThrust_days)
    fprintf('Dimensional Total Coast Time = %f days \n',timeCoast_days)
    fprintf('Maximum Available Acceleration = %e m/s^2 \n',3/initM_dim)
    fprintf('==============================================================\n')
    
end
