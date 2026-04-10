% GMAT Application Programmer's Interface Example
%
% Coded by D. Conway. Thinking Systems, Inc.
% Ported to MATLAB by J. McGreevy, Emergent Space Technologies Inc.
%
% This file drives a parameter scan for a GMAT script file.  The scenario driven
% here is a launch-coast-burn configuration, seeking a solution that performs a 
% lunar flyby.  

% The basic mission is scripted in the GMAT script named ToLuna.script.  Here, 
% we step through launch epochs, coast times, and delta-V values for a 
% translunar injection burn, and then evaluate how close the resulting 
% trajectory comes at closest approach to the moon using each set of parameters.

% Use a core script file to drive the analysis
theScript = "Ex_R2020a_ToLuna";
load_gmat(theScript + ".script");

% Set up the objects used to scan for a solution
burn = GMATAPI.GetObject("TOI");
Time = GMATAPI.GetObject("LeoTime");
start = GMATAPI.GetObject("StartEpoch");

disp('')

closest = 1000000000.0;
dt = 4000;
dv = 2.9;
startTime = 0.0;

% Scan through the delta-V using the burn object 
for i=0:9
   for j=0:19
      for k=0:24
         StartEpoch = k / 2.0;
         start.SetField("Value", StartEpoch / 24);
         Time.SetField("Value", 1500.0 + i * 100);
         burn.SetField("Element1", 3.0 + j * 0.01);
         gmat.gmat.RunScript();
         MoonDistance = GMATAPI.GetRuntimeObject("MoonDistance");
         theRange = MoonDistance.GetNumber("Value");
         if (closest > theRange)
            closest = theRange;
            dt = Time.GetNumber("Value");
            dv = burn.GetNumber("Element1");
            startTime = start.GetNumber("Value");

            % Report intermediate results if they are better than previous values
            fprintf('%f Launch %f Coast time: %f   Delta-V: %f   Moon Distance: %f\n', closest, startTime, dt, dv, theRange);
         end
      end
   end
end

% Build the name for the solution script
theSolution = theScript + "_solution.script";
fprintf("\nSaving solution to %s\n", theSolution);

% Set the values for the best solution found
start.SetField("Value", startTime);
Time.SetField("Value", dt);
burn.SetField("Element1", dv);

% Write out a script with the best values found
gmat.gmat.SaveScript(theSolution);
