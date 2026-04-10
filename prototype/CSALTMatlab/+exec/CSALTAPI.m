classdef CSALTAPI < handle
    %CSALTInterface Defines the API for executing CSALT problems

    properties (Access = protected)
        % A pointer to the trajectory object
        trajPointer
    end
    
    methods
                
        function Initialize(obj)
            % Calls Initialize() on the trajectory
           obj.trajPointer.Initialize(); 
        end
        
        function [z,F,xmul,Fmul,info] = Optimize(obj)
            % Initializes and executes the problem
            [z,F,xmul,Fmul,info] = obj.trajPointer.Optimize();
        end
        
        function numPhases = GetNumPhases(obj)
            % Returns the number of phases in the problem
            numPhases = obj.trajPointer.GetNumPhases();
        end
        
        function traj = GetTrajectory(obj)
            % Returns a pointer to the trajectory
            traj = obj.trajPointer;
        end
        
        function [timeArray,stateArray,controlArray] = GetTrajectoryData(obj,phaseNum)
            % Returns the time, state, and control arrays
            
            % Returns the time state and control arrays for a phase.
           if phaseNum > obj.trajPointer.GetNumPhases()
              error('Invalid Phase Number')
           else
               timeArray = obj.trajPointer.GetTimeArray(phaseNum);
               stateArray = obj.trajPointer.GetStateArray(phaseNum);
               controlArray = obj.trajPointer.GetControlArray(phaseNum);
           end
        end
        
    end
    
end

