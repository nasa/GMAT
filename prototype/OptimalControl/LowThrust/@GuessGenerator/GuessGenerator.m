classdef GuessGenerator < handle
    %   GuessGenerator Class to generate optimal control initial guess
    %   state and control.
    %
    %   This class computes the initial guess for optimal control problems
    %   Inputs are the dimensions of the problem, discretization points,
    %   and details required to compute the initial guess (method, 
    %   and possibly pointers to user dynamics functions.  
    %   The returned values are the guess for the states and controls at
    %   the discretizetion points
    
    properties
        % Real array. The vector of times in the phase
        timeVector
        % Integer. The number of states in the optimal control problem
        numStates
        % Integer. The number of state mesh points
        numStatePoints
        % Integer. The number of controls in the optimal control problem
        numControls
        % Integer. The number of control mesh points
        numControlPoints
    end
    
    methods
        
        function Initialize(obj,timeVector,numStates,numStatePoints,...
                numControls,numControlPoints)
            %  Intialize the guess generator based on problem dimensions
            %  
            %  @param timeVector.  The vector of times for the phase
            %  discretization.
            %  @param  numStates.  The number of states in the optimal
            %  control problem.  For orbit problem with mass flow,
            %  numStates is 7 (x,y,z,vx,vy,vz,m)
            %  @param numStatePoints.  The number of state points in the
            %  optimal control discretization.  
            %  @param numControlPoints.  The number of control points in 
            %  the optimal control discretization.  
            %  @param numControls.  The number of controls in the optimal
            %  control problem.  for orbit problem with [ux, uy, and uz],
            %  the number of controls equals three.
            %  @return.  In GMAT, return true if successful, false if not
            %  successful.
            
            %  Initialize the state and control array
            obj.timeVector   = timeVector;
            obj.numStates = numStates;
            obj.numStatePoints = numStatePoints;
            obj.numControls = numControls;
            obj.numControlPoints = numControlPoints;
            
        end
        
        function stateGuessMat = ComputeLinearStateGuess(obj,...
                initialGuessState,finalGuessState)
            %  Compute guess for linear state guess method.  
            %
            %  @param initialGuessState.  Real array of numStates x 1. 
            %  This is the guess for the state at the end of the phase
            %  (i.e. at the beginning of the phase). 
            %  @param  finalGuessState.  Real array of numStates x 1. 
            %  This is the guess for the state at the end of the phase.
            %  @return stateGuessMat, an array numStatePoints x numStates
            %  containing the initial guess for the state
            
            %  Fill in the state array
            stateGuessMat = obj.InitStateGuessMat();
            for i = 1:obj.numStates
                stateGuessMat(:,i) = linspace(...
                    initialGuessState(i),...
                    finalGuessState(i),...
                    obj.numStatePoints).';
            end
        end
        
        function controlGuessMat = ComputeConstControlGuess(obj,...
                controlMag)
            %  Compute guess for costant control
            %  @param controlMag.  The magnitude of each control vector 
            %  component.  Each element of returned guess will be
            %  controlMag
            %  @return controlGuessMat, an array numControlPoints x
            %  numControls containing the initial guess for the control
            
            %  Fill in the control array
            controlGuessMat = obj.InitControlGuessMat();
            for i = 1:obj.numControls
                controlGuessMat(:,i) = linspace(...
                    controlMag,controlMag,...
                    obj.numControlPoints).';
            end
        end
        
        function [stateGuessMat] = InitStateGuessMat(obj)
            % Intialize the dimensions for the state guess array
            %  @return stateGuessMat, an array numStatePoints x numStates
            %  filled with zeros.
            
            %  containing the initial guess for the state
            stateGuessMat = zeros(obj.numStatePoints,obj.numStates);
        end
        
        function [controlGuessMat] = InitControlGuessMat(obj)
            % Intialize the dimensions for the control guess array
            %  @return controlGuessMat, an array numControlPoints x
            %  numControls filled with zeros
            controlGuessMat = zeros(obj.numControlPoints,obj.numControls);
        end
        
    end
    
end



