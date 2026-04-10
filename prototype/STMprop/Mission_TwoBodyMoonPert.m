

%% Load the propagator and spacecraft, set the initial epoch and TOF
Sat.Epoch = '27 Jul 2000 20:48:50.014';
State_Earth_LunarFlyby
FM_Earth_AllPlanets_0_0_TwoBody
FM_Earth_EarthSunMoon_0_0_TwoBody
%FM_Earth_Earth_0_0
TOF  = 86400*4;


%%  Propagate the Spacecraft and create output
global jd_ref
[t, X, Phi] = Propagate(Sat,ForceModel,TOF);
n =  size(X,1);
disp(['               X-Comp.            Y-Comp.           Z- Comp.    ']);
disp(['Position   ' num2str( X(n,1:3),12) ]);
disp(['Velocity   ' num2str( X(n,4:6),12) ]);
disp('  ')


return
path = 'C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\test\script\input\InProgress\InProgress\truth\'
name = 'xxxGMAT_Earth_MoonTwoBody_0_0b.truth'
fname = [path name];
fId = fopen(fname,'w+');
format = ['%16.16f %16.16f %16.16f %16.16f %16.16f %16.16f %16.16f \n'];
for i = 1:n
    mjd_ref = (jd_ref -2430000) + (t(i) + 32 + 0.0343817)/86400 
    time = (t(i)/86400 + 21753.36798840278);
    [rv,vv] = TwoBodyState(mjd_ref,10,3)
   
   vec = [time (rv' - X(i,1:3)) (vv' - X(i,4:6)) ];
   
   fprintf(fId,format,vec); 
end

