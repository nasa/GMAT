

%% ========================================================================
%% =====  Initializations
%==========================================================================
ClearAll
clc
global traj configEarth configSun initialState propDuration phaseList
global scaleUtill
import exec.Trajectory
import exec.RadauPhase
thePathObject  = LISAPathObject_snapier();
thePointObject = LISAPointObject_snapier();

format longG
%% ========================================================================



%% ========================================================================
%% =====  Define Non-Dimensionalization Properties
%==========================================================================

%=== Quanitities used in non-dimensionalization and re-dimensionalization

%----  Earth Scaling
configEarth.gravParam          = 398600.4415; % km^3/s^2
configEarth.RE                 = 6378.1363; % km earth radius
configEarth.DU                 = 5.0e7; %km ~ LISA earth distance       %42000;
configEarth.TU                 = 86400*365.25;% ~1 year in seconds     %sqrt(configEarth.DU^3/configEarth.gravParam);
configEarth.VU                 = configEarth.DU/configEarth.TU;
%configEarth.ACCU               = configEarth.TU^2/configEarth.DU;
configEarth.ACCU               = configEarth.DU/configEarth.TU^2;
configEarth.StateScaleMat      = diag([ 1/configEarth.DU; 1/configEarth.DU;...
                                        1/configEarth.DU; 1/configEarth.VU;...
                                        1/configEarth.VU; 1/configEarth.VU]);
configEarth.StateAccScaleMat   = configEarth.TU*configEarth.StateScaleMat;                                                
configEarth.StateUnScaleMat    = inv(configEarth.StateScaleMat);
configEarth.StateAccUnScaleMat = inv(configEarth.StateAccScaleMat);

%----  Sun Scaling
configSun.gravParam          = 132712439935.4841;
configSun.DU                 = 149597870.7;
configSun.TU                 = sqrt(configSun.DU^3/configSun.gravParam);
configSun.ACCU               = configSun.DU/configSun.TU^2;
configSun.MU                 = 1000;
configSun.VU                 = configSun.DU/configSun.TU;
configSun.StateScaleMat      = diag([1/configSun.DU; ones(5,1); 1/configSun.MU]);
configSun.StateAccScaleMat   = configSun.TU*configSun.StateScaleMat;
configSun.StateUnScaleMat    = inv(configSun.StateScaleMat);
configSun.StateAccUnScaleMat = inv(configSun.StateAccScaleMat);

%  Configure the scale utility
scaleUtill              = ScaleUtility();
scaleUtill.earthScaling = configEarth;
scaleUtill.sunScaling   = configSun;
%% ========================================================================



%% ========================================================================
%% ===== USER INPUTS: Define Initial and Final State Values
%==========================================================================

% Create timeVector input for guess function
for ii=1:183
    epochMat(ii) = (1728000/86400)*(ii-1)+34114.99962963;
    epochMat(ii) = scaleUtill.ScaleTime(epochMat(ii), 3, 34114.99962963);
end

% Phase object input for guess function
phase1                  = RadauPhase;
phase1.initialGuessMode = 'UserGuessFunction';
phase1.centralBody      = 3; % Earth
phase1.refEpoch         = 34114.99962963; % LISA reference epoch;
phase1.phaseName        = 'LISAPhase1';

% Set Mesh Refinement Properties
meshIntervalFractions = [-1 -0.75 -0.5 -0.25 0 0.25 0.5 0.75 1];
meshIntervalNumPoints = 4*(ones(length(meshIntervalFractions)-1,1));

% Set Time Properties
initialA1MJDEpoch = 34114.99962963; %34050.99962787928;
finalA1MJDEpoch   = 1*365.25+34114.99962963; %thePointObject.GetFinalEpoch();
initialGuessTime  = scaleUtill.ScaleTime(   initialA1MJDEpoch,...
                                           phase1.centralBody,...
                                              phase1.refEpoch);
finalGuessTime    = scaleUtill.ScaleTime(     finalA1MJDEpoch,...
                                           phase1.centralBody,...
                                              phase1.refEpoch);
timeLowerBound    = scaleUtill.ScaleTime(   initialA1MJDEpoch,...
                                           phase1.centralBody,...
                                              phase1.refEpoch);
timeUpperBound    = scaleUtill.ScaleTime(  finalA1MJDEpoch+10,...
                                           phase1.centralBody,...
                                              phase1.refEpoch);
%===  Set state properties

% Get Initial Guess
[stateGuess,controlGuess] = LISAGuessFunction_snapier(epochMat,1,phase1);
initialGuessState         = stateGuess(1,:)';
finalGuessState           = stateGuess(100,:)';
numStateVars              = 18;
stateLowerBound           = stateGuess(1,:)'-3*abs(stateGuess(1,:))';  
stateUpperBound           = stateGuess(1,:)'+3*abs(stateGuess(1,:))';

%===  Set control properties
numControlVars    = 0;%3;
controlLowerBound = [0 0 0]; %[0 -2 -2 -2]';
controlUpperBound = [0 0 0]; %[1  2  2  2]';

% Set Phase Properties
phase1.SetRefineInitialMesh    (true);
phase1.SetNumStateVars         (numStateVars);
phase1.SetNumControlVars       (numControlVars);
phase1.SetMeshIntervalFractions(meshIntervalFractions);
phase1.SetMeshIntervalNumPoints(meshIntervalNumPoints);
phase1.SetStateLowerBound      (stateLowerBound)
phase1.SetStateUpperBound      (stateUpperBound)
phase1.SetStateInitialGuess    (initialGuessState)
phase1.SetStateFinalGuess      (finalGuessState)
phase1.SetTimeLowerBound       (timeLowerBound)
phase1.SetTimeUpperBound       (timeUpperBound)
phase1.SetTimeInitialGuess     (initialGuessTime)
phase1.SetTimeFinalGuess       (finalGuessTime)
phase1.SetControlLowerBound    (controlLowerBound)
phase1.SetControlUpperBound    (controlUpperBound)

%% Trajectory Object

%LISAPointObject_snapier.desiredInitState = initialGuessState(1,1:end);
%LISAPointObject_snapier.desiredInitTime  = initialGuessState(1,1);

traj                    = Trajectory();
traj.pathFunctionObject = LISAPathObject_snapier(); %thePathObject;
traj.pathFunctionObject.SetScaleUtil(scaleUtill);
traj.pathFunctionObject.refEpoch = phase1.refEpoch;

%% GMAT API: Create force model for path object & pass it into the path function object

%{
So, I...
*use the initial guess tool to create an 18x1 3-s/c state for LISA constellation
*load gmat
*create a force model w/ earth central body only
*create 3 separate spacecraft & set their states inside the path function
*link force model & spacecraft state
 from obj.getState (unscale that state before setting)
*state vector: pstate = earthorb.GetState().GetState()
*fm.GetDerivatives(pstate)
*dv = fm.GetDerivativeArray()
%}

[fm_earth, e2s_converter, earthSystem, sunSystem, instate, outstate] = ...
    GMAT_API_ForceModel_LISA(stateGuess, configEarth);

% Set properties on path objectfor gmat API material
traj.pathFunctionObject.ForceModel        = fm_earth;
% traj.pathFunctionObject.refFrameConverter = e2s_converter;
% traj.pathFunctionObject.earthFrame        = earthSystem;
% traj.pathFunctionObject.sunFrame          = sunSystem;
% traj.pathFunctionObject.Rvector_in        = instate;
% traj.pathFunctionObject.Rvector_out       = outstate;
% traj.pathFunctionObject.ref_epoch         = phase1.refEpoch;



%% Continue trajectory...
traj.pointFunctionObject                  = LISAPointObject_snapier();
traj.pointFunctionObject.desiredInitState = initialGuessState;
traj.pointFunctionObject.desiredInitTime  = initialGuessTime;%phase1.refEpoch;
%traj.pointFunctionObject.desiredFinalState = finalGuessState;
traj.pointFunctionObject.desiredFinalTime  = finalGuessTime;%finalA1MJDEpoch;

traj.pointFunctionObject.desiredInitEarthDist = 5.6e7/scaleUtill.earthScaling.DU;
traj.pointFunctionObject.desiredFinEarthDist  = 5.6e7/scaleUtill.earthScaling.DU;

traj.pointFunctionObject.SetScaleUtil(scaleUtill); %MUST NOT set properties on the object before it's passed in; set the path object for traj, then set properties on traj's object


%% GMAT API Parameters for point object
traj.pointFunctionObject.ForceModel        = fm_earth;
traj.pointFunctionObject.refFrameConverter = e2s_converter;
traj.pointFunctionObject.earthFrame        = earthSystem;
traj.pointFunctionObject.sunFrame          = sunSystem;
traj.pointFunctionObject.Rvector_in        = instate;
traj.pointFunctionObject.Rvector_out       = outstate;
traj.pointFunctionObject.ref_epoch         = phase1.refEpoch;

%% Continue trajectory...
traj.plotFunctionName       = 'LISAPlotFunction_snapier';
traj.showPlot               = true(); %false(); 
traj.plotUpdateRate         = 5;
traj.costLowerBound         = -Inf;
traj.costUpperBound         = Inf;
traj.maxMeshRefinementCount = 5;
traj.refineInitialMesh      = true;
traj.optimalityTol          = 1e-8;
traj.feasibilityTol         = 1e-4; % Reduce to 1e-4 b/c 1e-8 is too strict         
                                    %(probably) especially for 1st mesh 
                                    % refinement. Also: can MATLAB CSALT
                                    % use vector of tolerances for mesh
                                    % refinement iterations?
    
% Configure the list of phases.
phaseList = {phase1};

% Add the scale utility to all phases
for phaseIdx = 1:length(phaseList)
    phaseList{phaseIdx}.scaleUtil = scaleUtill;
end

thePathObject.SetScaleUtil(scaleUtill);
thePathObject.phaseList  = phaseList;

%thePathObject.EvaluateFunctions()

thePointObject.SetScaleUtil(scaleUtill);
thePointObject.phaseList = phaseList;

traj.phaseList         = {phase1};
traj.guessFunctionName = 'LISAGuessFunction_snapier';

%% Optimize Trajectory OR load previously optimized one
OPTIMIZZE    = 1;
traj_desired = 'Traj_feasible_10year_initialEclpinc_andcostfun.mat';%Traj_feasible_10year_tooGood2beTrue_noEclipticInc.mat';

if OPTIMIZZE
    [z,F,xmul,Fmul] = traj.Optimize();
    InitialTimeError = z(1)-initialGuessTime
    InitialStateError = max(abs(z(3:20)-initialGuessState))
    FinalTimeError  = z(2)-finalGuessTime    
    
    % Extract data from trajectory object
    times     = traj.phaseList{1}.TransUtil.GetTimeVector();
    stateVec  = traj.phaseList{1}.DecVector.GetStateArray();
    init_dist = norm(stateVec(1,1:3));
    fin_dist  = norm(stateVec(end,1:3));
     
    % Plot
    LISAPlotFunction_snapier(traj)
    
    xx0 = stateVec(1,1:6)';
    xx0(1:3) = xx0(1:3)*configEarth.DU;
    xx0(4:6) = xx0(4:6)*configEarth.VU;
    
    xxf = stateVec(end,1:6)';
    xxf(1:3) = xxf(1:3)*configEarth.DU;
    xxf(4:6) = xxf(4:6)*configEarth.VU;
    %{   
    xx_matlab =
    34480.24962963
         -45960034.2460826
           27071084.397309
          13079549.4156999
         -4.79265399790082
         -8.66516535947231
         -3.75985702870614
    
    xx_GMAT
    34480.05634898365
         -46792326.78764857
          27903161.27981699       
          13440352.13490317         
         -4.975690717383076        
         -8.820749838899587       
         -3.824384601461049     
   
    34481.21375639106         
         -47279778.12167754     
          27017115.54572335        
          13055920.20409038         
         -4.77310484720731         
         -8.899485929863499        
         -3.863959474395423        

    
    %}
    

else
    
    load(traj_desired);
    %cspice_furnsh('de430.bsp')
    cspice_furnsh('C:\GMAT\gmat\GMAT-R2018a-Windows-x64\data\planetary_ephem\spk\DE405AllPlanets.bsp');%.bsp');
    % Extract data from trajectory object
    times     = traj.phaseList{1}.TransUtil.GetTimeVector();
    stateVec  = traj.phaseList{1}.DecVector.GetStateArray();
    init_dist = norm(stateVec(1,1:3));
    fin_dist  = norm(stateVec(end,1:3));
    
    %% Plot the trajectory
    %%{
    % Re-dimensionalize state
    %% GMAT API Parameters for point object
    pointobj = LISAPointObject_snapier();
    pointobj.ForceModel        = fm_earth;
    pointobj.refFrameConverter = e2s_converter;
    pointobj.earthFrame        = earthSystem;
    pointobj.sunFrame          = sunSystem;
    pointobj.Rvector_in        = instate;
    pointobj.Rvector_out       = outstate;
    pointobj.ref_epoch         = phase1.refEpoch;
    pointobj.scaleUtil         = scaleUtill;
    
    %     for s=1:length(times)
    %         xDim_LISA1  = pointobj.scaleUtil.UnScaleState(stateVec(s,1:6)',   3);
    %         xDim_LISA2  = pointobj.scaleUtil.UnScaleState(stateVec(s,7:12)',  3);
    %         xDim_LISA3  = pointobj.scaleUtil.UnScaleState(stateVec(s,13:end)',3);
    %         [cos_rad_inc,lbb, ubb] = pointobj.constraint_eclipticInclination(...
    %             [xDim_LISA1; xDim_LISA2; xDim_LISA3], times(s));
    %         inc1(s) = acos(cos_rad_inc(1))*(180/pi);
    %         inc2(s) = acos(cos_rad_inc(2))*(180/pi);
    %         inc3(s) = acos(cos_rad_inc(3))*(180/pi);
    %     end
    % force model on the traj can't be saved on the output traj object
    % being loaded in here, so make a new one.
    % NLPfuncRadau class for setting vector of mesh refinement tolerances
    % ('rel error tol' in the header)
    traj.pointFunctionObject = pointobj;
    %LISAPlotFunction_snapier(traj)
    %}
    
    %% Test out n-body spline ephem interface
    
    % Re-dimensionalize nominal (incorrect) trajectory's states
    for ii=1:length(times)               
        xDim_LISA1(ii,:) = scaleUtill.UnScaleState(stateVec(ii,1:6)',    3);
        xDim_LISA2(ii,:) = scaleUtill.UnScaleState(stateVec(ii,7:12)',   3);
        xDim_LISA3(ii,:) = scaleUtill.UnScaleState(stateVec(ii,13:end)', 3);        
        tDim(ii)         = scaleUtill.UnScaleTime(          times(ii),...
                                                   phase1.centralBody,...
                                                      phase1.refEpoch);
    end
    
    %% Derivative function for spice n-body propagation
    u_s             = [3.986004415000000e+05, 1.327124400179900e+11];
    refEpoch        = phase1.refEpoch+2430000;
    
    % LISA 1
    dt              = (tDim(2)-tDim(1))*86400;
    ops             = odeset('RelTol',1e-13, 'AbsTol', 1e-13);
    X0              = xDim_LISA1(1,:)';
    
    % Compare derivatives at states along (wrong) LISA trajectory
    for ii=1:length(times)
        traj.pointFunctionObject.ForceModel.GetDerivatives(xDim_LISA1(ii,:),86400*(tDim(ii)-tDim(1)));
        dt(ii)       = (tDim(ii)-tDim(1))*86400;
        dx_spice{ii} = EOM_Nbody_splinephem(dt(ii), xDim_LISA1(ii,:)', u_s, refEpoch);     
        %traj.pointFunctionObject.ForceModel.SetTime((tDim(ii)-tDim(1))*86400);%SetEpoch(tDim(ii));%%SetTime(tDim(ii)-tDim(1));
        dx_API{ii}   = traj.pointFunctionObject.ForceModel.GetDerivativeArray();
        dx_diff(ii)  = max(abs((dx_API{ii}-dx_spice{ii})./dx_spice{ii}));
        
        vx_diff(ii)  = max(abs((dx_API{ii}(1)-dx_spice{ii}(1))/dx_spice{ii}(1)));
        vy_diff(ii)  = max(abs((dx_API{ii}(2)-dx_spice{ii}(2))/dx_spice{ii}(2)));
        vz_diff(ii)  = max(abs((dx_API{ii}(3)-dx_spice{ii}(3))/dx_spice{ii}(3)));
        ax_diff(ii)  = max(abs((dx_API{ii}(4)-dx_spice{ii}(4))/dx_spice{ii}(4)));
        ay_diff(ii)  = max(abs((dx_API{ii}(5)-dx_spice{ii}(5))/dx_spice{ii}(5)));
        az_diff(ii)  = max(abs((dx_API{ii}(6)-dx_spice{ii}(6))/dx_spice{ii}(6)));
    end
    
    figure;
    hold on
    subplot(2,3,1)
    semilogy(times,vx_diff)
    title('vx error')
    
    subplot(2,3,2)
    semilogy(times,vy_diff)
    title('vy error')
    
    subplot(2,3,3)
    semilogy(times,vz_diff)
    title('vz error')
    
    subplot(2,3,4)
    semilogy(times,ax_diff)
    title('ax error')
    
    subplot(2,3,5)
    semilogy(times,ay_diff)
    title('ay error')
    
    subplot(2,3,6)
    semilogy(times,az_diff)
    title('az error')

    hold off
    
    
    %{
    idx = 100;
    ttt = 27*86400;
    [dx_spiceNbody]  = EOM_Nbody_splinephem(0, xDim_LISA1(idx,:)', u_s, tDim(idx)+2430000); % dt = dt in seconds w/r.t. refEpoch (MJD)
        [tt, xx]        = ode113(@(t,X) ...
                                    EOM_Nbody_splinephem(t,X, u_s, tDim(idx)+2430000),...
                                                                   [0 ttt],...
                                                            xDim_LISA1(idx,:)',...
                                                                       ops);
    
    %%% GMAT API Force model derivatives
    %traj.pointFunctionObject.ForceModel.GetDerivatives(xDim_LISA1(1,:));
    %dx_2B_LISA1 = traj.pointFunctionObject.ForceModel.GetDerivativeArray();
    %}
    
    %% NOTES:
    % SetEpoch() and SetTime() functions on the force model - do we need to
    % update these every time we call the force model at a new time?
end



%% GMAT Propagation Test
%{
% state_out_LISA1 = [ -1.09782636336071 0.278658414400632 0.147649421905549,...
%                     -3.13635351853637 -5.59620975377563 -2.42659292564024]';
% state_LISA1_dim = [state_out_LISA1(1:3)*configEarth.DU;                
%                    state_out_LISA1(4:6)*configEarth.VU];
% desired_epoch   = 34133.26462963;
% LISA1_gmat_out  = [ 34133.21385249509,...
%                    -54870438.61160102 13969866.63105381,  7398487.004575604,...
%                    -4.972476912581967 -8.87280590764979  -3.847374657560314];        
%                
% 
%               
%}

%% NOTES:
%{
-10 year "goodish" trajectory I obtained appears not to agree with GMAT's
propagation using equivalent Sun, Earth, Moon point mass dynamics

-Tried to ascertain cause. Stumbled on ACCU = TU^2/DU instead of DU/TU^2. 
 This did not fix things. Why the disagreement?

-Began stripping off force model perturbations, path constraints & point
constraints. Did not seem to speed up optimization.

-Tried a simpler problem: find a feasible solution from the initial guess I
have. That guess has forces: Earth, Sun, Venus & self-grav. 

-Constraints to "propagate" state: initial time error, final time error,
initial state error.

-Feasible solution found for a 0.2 year propagation; infeasible beyond
this... why can't it propagate? turned off API completely & resorted to Earth
2-body only. Still "the problem appears to be infeasible" why?? Too
hyperbolic?

-API definitely has sun on when I tell it to. But perhaps there's more to
adding the solar point mass than just adding a point mass command...

TRY spline ephem
spline ephem:
-enclosing timespan: a timespan longer than the timespan expceted to need during mission (2030-2045, say)
-choose a lot of intermediate steps on a matlab linspace (1000 pts/year for example)
-use spice to get positions at all those times
-use matlab's interp1 function to create a pseudo-ephemeris for those bodies
-have to interpolate each position coordinate for each body individually
-just get mice on my windows

%}
               
               


               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
               
