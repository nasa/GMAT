
classdef LISAPointObject_snapier < userfunutils.UserPointFunction
   % Boundary functions for the LISA mission orbit design
   properties
       desiredInitState % Initial state of all 3 orbits in non-dimensional units.
       desiredInitTime  % Initial time in non-dimensional units
       desiredInitEarthDist
       
       %desiredFinalState
       desiredFinalTime
       desiredFinEarthDist
       
       scaleUtil
       
       % Parameters using GMAT API
        ForceModel
        refFrameConverter
        earthFrame
        sunFrame
        Rvector_in
        Rvector_out
        ref_epoch
   end
   
   methods
       function EvaluateFunctions(obj)
           %% Set Constraints
           
           phaseNum           = 1;                                % Evaluate boundary functions
           initState          = obj.GetInitialStateVec(phaseNum); % Extract the initial and final time and state for
           initTime           = obj.GetInitialTime(phaseNum);     % Phase 1, (the only phase in the problem)
           
           % Match Initial State Constraint
           initTimeError      = initTime  - obj.desiredInitTime;
           
           % Be within initial Earth Distance Bounds
           initEarthDistErr  = norm(initState(1:3),2);% - obj.desiredInitEarthDist
           
           % Be within final desired Earth Distance Bounds
           finState          = obj.GetFinalStateVec(phaseNum); % Extract the initial and final time and state for
           finEarthDistErr   = norm(finState(1:3),2);% - obj.desiredFinEarthDist %desiredFinalState; % Compute the boundary constraints
           finTime           = obj.GetFinalTime(phaseNum);    % Phase 1, (the only phase in the problem)
           finTimeError      = finTime - obj.desiredFinalTime;
           
           %----------------- Initial time & state, final state constraints
           %%{
           initConditionError = initState - obj.desiredInitState; % Compute the boundary constraints
           %obj.SetAlgFunctions([initTimeError; initConditionError]);%; finTimeError]);
           obj.SetAlgFunctions([initTimeError; initConditionError; finTimeError])
           
           %}
           
           %----------------- Initial & final time
           %{
           obj.SetAlgFunctions([ initTimeError; finTimeError]); % USE IF YOU WANT THE INITIAL STATE TO VARY BUT TIME TO BE TIGHTLY CONSTRAINED
           %}
           
           %----------------- Ecliptic Inclination
           %{
           % Re-dimensionalize state
           xDim_LISA1  = obj.scaleUtil.UnScaleState(initState(1:6),    3);
           xDim_LISA2  = obj.scaleUtil.UnScaleState(initState(7:12),   3);
           xDim_LISA3  = obj.scaleUtil.UnScaleState(initState(13:end), 3);
           [eclipinc,eclipinc_lb, eclipinc_ub] = obj.constraint_eclipticInclination([xDim_LISA1; xDim_LISA2; xDim_LISA3], 0);
           %}
           
           %----------------- Set point constraints
           %{
           obj.SetAlgFunctions([  initTimeError; initEarthDistErr;...
                                    finEarthDistErr; finTimeError]);%;...
                                %    eclipinc]); % USE IF YOU WANT THE INITIAL STATE TO VARY BUT TIME TO BE TIGHTLY CONSTRAINED
           %}
           
           %% Set Cost Function
           %            for ss=1:
           %            obj.SetCostFunction(finalTime);
           %{
           xx = obj.phaseList{1}.DecVector.GetStateArray;       
           [cost] = obj.costfun_sidelen_integral(xx);
           obj.SetCostFunction(cost);
           %}
           
           %% If initializing, Set Constraint Bounds
           if obj.IsInitializing              
               %----------------- Initial & final time & initial state
               %%{
               obj.SetAlgFuncLowerBounds(zeros(20,1))
               obj.SetAlgFuncUpperBounds(zeros(20,1))
               %}
               
               %----------------- Initial & Final Time constraints only
               %{
               obj.SetAlgFuncLowerBounds(zeros(2,1))
               obj.SetAlgFuncUpperBounds(zeros(2,1))
               %}
               
               %----------------- Initil & Final Time constraints & initial & final Earth distance constraints
               %{
               obj.SetAlgFuncLowerBounds([0;... % Initial time error
                                          0.9;... % Initial Earth distance
                                          0.9;... % Final Earth distance
                                          0]);... % Final time error
                                          %eclipinc_lb]); % Ecliptic inclination
               obj.SetAlgFuncUpperBounds([0;...
                                          1.33;...
                                          1.33;...
                                          0]);...
                                          %eclipinc_ub]);           
               %}
               
               

           end
       end
       
       %% Ecliptic Inclination Constraint
       function [constraint, constraint_lowerbounds, constraint_upperbounds]...
               = constraint_eclipticInclination(obj, X, t) % Pass in the DIMENSTIONAL state
           %% LISA 1
             for i=1:6
                 obj.Rvector_in.SetElement(i-1, X(i)); % SetElement indexes from zero
             end
             
             % Convert coordinates
             time = obj.scaleUtil.UnScaleTime(t, 3, obj.ref_epoch);
             obj.refFrameConverter.Convert(time,   obj.Rvector_in,...
                                                   obj.earthFrame,...
                                                  obj.Rvector_out,...
                                                     obj.sunFrame);
             for i=1:6
                 LISA1_sun(i) = obj.Rvector_out.GetElement(i-1);
             end
             
             %% LISA 2
             for i=1:6
                 obj.Rvector_in.SetElement(i-1, X(i+6)); % SetElement indexes from zero
             end
             
             % Convert coordinates
             time = obj.scaleUtil.UnScaleTime(t, 3, obj.ref_epoch);
             obj.refFrameConverter.Convert(time,   obj.Rvector_in,...
                                                   obj.earthFrame,...
                                                  obj.Rvector_out,...
                                                     obj.sunFrame);
             for i=1:6
                 LISA2_sun(i) = obj.Rvector_out.GetElement(i-1);
             end
             
             %% LISA 3
             for i=1:6
                 obj.Rvector_in.SetElement(i-1, X(i+12)); % SetElement indexes from zero
             end
             
             % Convert coordinates
             time = obj.scaleUtil.UnScaleTime(t, 3, obj.ref_epoch);
             obj.refFrameConverter.Convert(time,   obj.Rvector_in,...
                                                   obj.earthFrame,...
                                                  obj.Rvector_out,...
                                                     obj.sunFrame);
             for i=1:6
                 LISA3_sun(i) = obj.Rvector_out.GetElement(i-1);
             end
             
            %% Transform Earth-centered ecliptic state into sun-centered ecliptic
            h1     = cross(LISA1_sun(1:3),LISA1_sun(4:6));
            h1mag  = norm(h1);
            h1uv   = h1/h1mag;
            
            h2     = cross(LISA2_sun(1:3),LISA2_sun(4:6));
            h2mag  = norm(h2);
            h2uv   = h2/h2mag;
            
            h3     = cross(LISA3_sun(1:3),LISA3_sun(4:6));
            h3mag  = norm(h3);
            h3uv   = h3/h3mag;
            
            z      = [0 0 1];
            
            %% Constraint
            r2d = (180/pi);
            
            constraint             = [z*h1uv'; z*h2uv'; z*h3uv'];
            constraint_upperbounds = [cos(0.0/r2d); cos(0.0/r2d); cos(0.0/r2d)]; % bounds reverse b/c cosine
            constraint_lowerbounds = [cos(0.5/r2d); cos(0.5/r2d); cos(0.5/r2d)];
       end
       
       %% Calculate Geometry of LISA Constellation
       function [ sv1,  sv2,  sv3, dsv1, dsv2, dsv3,s1,   s2,   s3,...
                  ds1,  ds2,  ds3, re1,  re2,  re3, ang1, ang2, ang3]...
                  = CalcSideData(obj, X1,X2,X3)     
              
              d2r  = pi/180;
              re1  = norm(X1(1:3));
              re2  = norm(X2(1:3));
              re3  = norm(X3(1:3));
              sv1  = X1(1:3) - X2(1:3);
              sv2  = X2(1:3) - X3(1:3);
              sv3  = X1(1:3) - X3(1:3);
              dsv1 = X1(4:6) - X2(4:6);
              dsv2 = X2(4:6) - X3(4:6);
              dsv3 = X1(4:6) - X3(4:6);
              s1   = norm(sv1(1:3));
              s2   = norm(sv2(1:3));
              s3   = norm(sv3(1:3));
              ds1  = dot(dsv1(1:3),sv1(1:3))/s1;
              ds2  = dot(dsv2(1:3),sv2(1:3))/s2;
              ds3  = dot(dsv3(1:3),sv3(1:3))/s3;
              ang1 = obj.vecangle(sv2(1:3),-sv1(1:3))/d2r;
              ang2 = obj.vecangle(sv2(1:3), sv3(1:3))/d2r;
              ang3 = obj.vecangle(sv1(1:3), sv3(1:3))/d2r;
       end
       
       %% Cost function (pass in scaled state)
       function [cost] = costfun_sidelen_integral(obj, X)
           [rr cc] = size(X);
           X1 = X(:,  1:6);
           X2 = X(:, 7:12);
           X3 = X(:,13:18);
           cost = 0;
           
           for ii=1:rr
               [~, ~, ~, ~, ~, ~, s1(ii), s2(ii), s3(ii),...
                   ~, ~, ~, ~, ~, ~, ~, ~, ~] = obj.CalcSideData(X1,X2,X3);
               
               cost = cost + (s1(ii)-0.05)^2 + (s2(ii)-0.05)^2 + (s3(ii)-0.05)^2;
           end
       end
       
       %% Compute angle (in radians) between two vectors
        function theta = vecangle(obj, vec1,vec2)            
            %  function theta = vecangle(vec1,vec2)
            %
            %  This function calculates the angle between two vectors using the
            %  dot product.  The solution is always in quadrant one or two.
            %
            %  Variable I/O
            %
            %   Variable Name    I/0    Units     Dim.  Description
            %   vec1              I      UOI      3x1    Vector one
            %   vec2              I      UOI      3x1    Vector two
            %   theta             O      rad      1x1    angel between vector in radians
            %
            %  External References:  None
            %
            %  Modification History
            %  S. Hughes - 05/25/01 -  Created File
            arg = dot(vec1,vec2) / norm(vec1) / norm(vec2);
            theta = acos( arg );
        end
       
       %% Set Scale Utility Object
       function SetScaleUtil(obj,scaleUtill)
           obj.scaleUtil = scaleUtill;
       end
       
       %% Evaluate Cosntraint Jacobians
       function EvaluateJacobians(~)
           % The Jacobian is evaluated via numerical differencing for this
           % problem, not a user supplied function.
       end
   end
end
