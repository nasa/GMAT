classdef RunEstimator < handle

    % Set the public properties
    properties  (SetAccess = 'public')

        Estimator

    end

    % Set the methods
    methods

        %------------------------------------------------------------------
        %-----  Initialize
        %------------------------------------------------------------------

        function obj = Initialize(obj,Estimator)
            
            %==============================================================
            %==============================================================
            % - Add estimator to RunEstimator Object
            %==============================================================
            %==============================================================
        
            %----- Add the estimator
            obj.Estimator = Estimator;
                        
        end
        
        %------------------------------------------------------------------
        %----- Prepare to Solve
        %------------------------------------------------------------------
        
        % - Set up the estimation state vector

        function RunEst = PreparetoSolve(RunEst)
            
            RunEst.Estimator.ESV = RunEst.Estimator.ESM.GetStates();
            
        end %  Prepare to Solve
        
        %------------------------------------------------------------------
        %----- Prepare to Propagate
        %------------------------------------------------------------------

        function RunEst = PreparetoPropagate(RunEst)
           
            %---- KLUDGE TO GET EPOCH FOR PROPAGATOR
            RunEst.Estimator.Propagator.Epoch =RunEst.Estimator.Propagator.PSM.Objects{1}.Epoch; 
            
            %Prop.PSV = PSV;
            RunEst.Estimator.Propagator.PSV   = RunEst.Estimator.Propagator.PSM.GetStates();

        end % Prepare to Propagate
        
        %------------------------------------------------------------------
        %----- Execute:  Solve the problem.
        %------------------------------------------------------------------

        function Execute(RunEst)
            
            global TestCase
            
            %----- Call prepare to solve to populate ESV with values.
            RunEst.PreparetoSolve();
            
            %----- Extract attached objects and variables to shorten later code
            Prop        = RunEst.Estimator.Propagator;
            Estimator   = RunEst.Estimator;
            measManager = Estimator.MeasManager;
            numObs      = measManager.numObs;
            Epochs      = measManager.Epochs;
            Obs         = measManager.Obs;
            
            %----- Clone the objects
            estObjectClone  = Estimator.ESM.CloneObjects();
            propObjectClone = Prop.PSM.CloneObjects();
            
            %----- Display related stuff
            disp(' Iter    norm(Yo - Yc)     norm(dx)');
            disp('------------------------------------')
            formatstr   = '%5.0f  %14.6g %14.6g %12.4g %12.3g %12.3g %12.3g %s  %s';
            
            %----- The Estimator Loop
            oldRMS = 0;  newRMS = 1e12; normdx = 1e12;
            iter = 1;
            numStates = Estimator.ESM.numStates;          
            while abs( newRMS - oldRMS ) > Estimator.RelTolerance ...
                              & iter <= Estimator.MaxIterations ...
                              & normdx > Estimator.AbsTolerance;
                          
                infoMat   = zeros(numStates,numStates);
                residMat  = zeros(numStates,1); 
                                
                %  Copy object clones in ESM.Objects
                Estimator.ESM.Objects = Estimator.ESM.CopyObjects(estObjectClone);
                
                %----------------------------------------------------------
                %  Update the states of objects based on current iteration
                %  !!! This part of the code is probably more complex than
                %  it would be in C, C++, based on the fact that if I
                %  change an object on the ESM, it does not update that
                %  same object on the PSM or MM. ??? Not sure why, matlab
                %  handle classes are supposed be similar to pointers.
                %----------------------------------------------------------
                
                %  update objects on ESM
                Estimator.ESM.SetStates(Estimator.ESV);
                
                %  update objects on PSM
                Prop.PSM  = Prop.PSM.UpdateObjects(Estimator.ESM);
                
                % update objects on MM
                
              
                %----------------------------------------------------------
                %  Prepare to Propagate
                %  update the values in the state vector, based on updates
                %  to the objects being propagated.
                %----------------------------------------------------------
                RunEst = RunEst.PreparetoPropagate();
                
                %----- Perform the accumulation
                for i = 1:numObs

                    %  Step to next measurement epoch
                    Prop = Prop.SteptoEpoch(Epochs(i));
                    
                    %  KLUDGE - THIS WILL BE AVOIDED BY MODS TO MEASUREMENT
                    %  MANAGER AND SOME CODE CHANGES TO ADDRESS HANDLE
                    %  CLASS SHORTCOMINGS IN MATLAB.
                    if TestCase == 1
                        Estimator.ESM.Objects{1} = Prop.PSM.Objects{1};
                        measManager.MeasurementHandles{i}.Participants{1}{1} = Prop.PSM.Objects{1};
                        dgdvv        = zeros(1,3);
                        [y,htilde] = measManager.GetMeasurement(i);
                        Htilde       = [htilde dgdvv];
                    elseif TestCase == 2
                        Estimator.ESM.Objects{1} = Prop.PSM.Objects{1};
                        measManager.MeasurementHandles{i}.Participants{1}{1} = Prop.PSM.Objects{1};
                        measManager.MeasurementHandles{i}.Participants{2}{1} = Prop.PSM.Objects{1};
                        measManager.MeasurementHandles{i}.RangeMeas.Bias     = Estimator.ESV(7,1);
                        dgdvv        = zeros(1,3);
                        [y,htilde] = measManager.GetMeasurement(i);
                        Htilde       = [htilde dgdvv 1];
                    elseif TestCase == 3
                    
                        Estimator.ESM.Objects{1} = Prop.PSM.Objects{1};
                        measManager.MeasurementHandles{i}.Participants{1}{1} = Prop.PSM.Objects{1};
                        measManager.MeasurementHandles{i}.Participants{2}{1} = Prop.PSM.Objects{1};
                        measManager.MeasurementHandles{i}.RangeMeas.Bias     = Estimator.ESV(7,1);
                        dgdvv        = zeros(1,3);
                        [y,htilde] = measManager.GetMeasurement(i);
                        Htilde       = [htilde dgdvv 1 -htilde];
                    
                    end
                    observations(i,1) = y;
                    
                    %  Calculate the H matrix and accumulate         
                    STM          = Estimator.ESM.GetSTM;


                    Hi           = Htilde*STM;
                    resid(i,1)   = Obs(i,1) - y;
                    infoMat      = infoMat + Hi'*Hi;
                    residMat     = residMat + Hi'*resid(i,1);
                    
                end
            
                %  Solve the normal equations and update the ESV
                oldRMS = newRMS;
                newRMS = sqrt( sum(resid) / numObs );
                dx     = inv(infoMat)*residMat;
                R      = chol(infoMat);
                z      = inv(R')*residMat;
                x      = inv(R)*z;
                normdx = norm(dx);
                Estimator.ESV = Estimator.ESV + dx; 
                
                %  Output iteration data
                iterdata = sprintf(formatstr,iter,norm(resid)/numObs,norm(dx));
                disp(iterdata);
                iter = iter + 1;
                
            end
            
            %  Display convergence method
            if iter > Estimator.MaxIterations
                disp(' ')
                disp('Batch Estimator Did Not Converge Before Reaching Max Iterations!!')
            else      
                disp(' ')
                disp('Batch Estimator Has Converged!!')
            end

            %  Plot the residuals
            figure(1);
            plot(1:1:numObs,resid,'*')
                      
            %  Copy object clones in ESM.Objects
            Estimator.ESM.Objects = Estimator.ESM.CopyObjects(estObjectClone);
            Estimator.ESM.SetStates(Estimator.ESV);
            
            % -- This is another KLUDGE!!
            for i = 1:Estimator.ESM.numObjects
                
                for j = 1:size(Estimator.ESM.ParamIds{i},2)
                
                    
                    Id = Estimator.ESM.ParamIds{i}(j);
                    State = Estimator.ESM.Objects{i}.GetState(Id);
                    disp(' ');
                    if Id == 201
                        disp('Spacecraft State Estimate is:')
                    elseif Id ==401
                        disp('Measurement Bias Estimate is:')
                    elseif Id ==301
                        disp('Ground Station Location Estimate is:')
                    end
                    disp(State)
                    
                end
            end
               
        end % Execute
   end
end