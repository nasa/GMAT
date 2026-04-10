classdef LISAPointObject < userfunutils.UserPointFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % Initial state in non-dimensional modified equinoctial elements,
        % with mass at the end
        initModEqElemNonDim
        % Structure containing scale values
        scaleUtil
        % The Propagagation duration in seconds
        propDuration
        % Final Cartesian state in EME2000 KM, KM/S
        finalCartState
        % Initial Cartesian state in EME2000 KM, KM/S
        initialCartState
        % Initial mass guess, Kg
        initialMass
        % Init total state MEE + Mass non-dimensionalized
        fullInitStateNonDim
        % Initial Epoch
        initA1ModJulianEpoch
        % 
        finalModEqElemNonDim
        finalA1ModJulianEpoch
        % NumCalls
        numCalls = 0;
        conValues
        conLowerBounds
        conUpperBounds
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            global propDuration phaseList
            obj.ResetConstraints();
            
            % Convert final achieved state to Cartesian and compute error
            obj.AppendConstraint('LaunchInjectionState')
            obj.AppendConstraint('ArrivalState')
            %obj.AppendConstraint('ArrivalEpoch')
            % Set the boundary constraints
            obj.SetAlgFunctions(obj.conValues);
            obj.SetAlgFuncLowerBounds(obj.conLowerBounds);
            obj.SetAlgFuncUpperBounds(obj.conUpperBounds);
            
            % Set the cost function
            finalState = obj.GetFinalStateVec(1);
            obj.SetCostFunction(-finalState(7));
            
        end
        
        function AppendConstraint(obj,conName)
            conName = ['obj.' conName];
            [f,l,u] = eval(conName);
            obj.conValues = [obj.conValues;f];
            obj.conLowerBounds = [obj.conLowerBounds;l];
            obj.conUpperBounds = [obj.conUpperBounds;u];
        end
        
        function ResetConstraints(obj)
            obj.conLowerBounds = [];
            obj.conUpperBounds = [];
            obj.conValues = [];
        end
        
        function EvaluateJacobians(obj)
            
        end
        
        function epoch = GetInitialEpoch(obj)
            epoch = obj.initA1ModJulianEpoch;
        end
        
        function epoch = GetFinalEpoch(obj)
            epoch = obj.finalA1ModJulianEpoch;
        end
        
        function SetInitialMass(obj,mass)
           obj.initialMass = mass; 
        end
        
        function SetInitialConditionsByName(obj,stateName)
            global phaseList
            % Sets the initial conditions
            [stateVec,a1ModJulianEpoch] = LISAStates(stateName,'ModifiedEquinoctial');
            obj.initModEqElemNonDim = stateVec;
            obj.initA1ModJulianEpoch = a1ModJulianEpoch;
        end
        
         function SetFinalConditionsByName(obj,stateName)
            global phaseList
            % Sets the initial conditions
            [stateVec,a1ModJulianEpoch] = LISAStates(stateName,'ModifiedEquinoctial');
            obj.finalModEqElemNonDim = stateVec;
            obj.finalA1ModJulianEpoch = a1ModJulianEpoch;
        end
        
        function SetScaleUtil(obj,scaleUtil)
            % Sets the structure containing scale data
            obj.scaleUtil = scaleUtil;
        end
        
        function [conValue,lowerBound,upperBound] = LaunchInjectionState(obj);
            global phaseList
            
            phaseIdx = GetPhaseIdxFromName(obj,'LISAPhase1');
            
            % Get the states and times at the boundaries
            initState = obj.GetInitialStateVec( phaseIdx);
            initState(6) = mod(initState(6),2*pi);
            initTime = obj.GetInitialTime( phaseIdx);
            
            % Compute non-dimensional boundary condition
            meeInitDim = [obj.initModEqElemNonDim ; obj.initialMass];
            meeInitNonDim= obj.scaleUtil.ScaleState(meeInitDim,phaseList{phaseIdx}.centralBody);
            timeNonDim = obj.scaleUtil.ScaleTime(obj.initA1ModJulianEpoch,phaseList{phaseIdx}.centralBody,phaseList{phaseIdx}.refEpoch);
            conValue = [(timeNonDim - initTime);(meeInitNonDim - initState)];
            lowerBound = zeros(8,1);
            upperBound = lowerBound;
        end
        
        function [conValue,lowerBound,upperBound] = ArrivalState(obj);
            global phaseList
            phaseIdx = GetPhaseIdxFromName(obj,'LISAPhase1');

            % Get the states and times at the boundaries
            meeFinalDim = [obj.finalModEqElemNonDim ; 1800]; % mass is dummy variable not used in constraint
            finalState = obj.GetFinalStateVec( phaseIdx);
            finalState(6) = mod(finalState(6),2*pi);
            meeFinalDim(6) = mod(meeFinalDim(6),2*pi);
            finalTime = obj.GetFinalTime(phaseIdx);
            
            % Compute non-dimensional boundary condition
            meeFinalNonDim= obj.scaleUtil.ScaleState(meeFinalDim,phaseList{phaseIdx}.centralBody);
            timeNonDim = obj.scaleUtil.ScaleTime(obj.finalA1ModJulianEpoch,phaseList{phaseIdx}.centralBody,phaseList{phaseIdx}.refEpoch);
            conValue = [(timeNonDim - finalTime);(meeFinalNonDim(1:6) - finalState(1:6))];
            lowerBound = zeros(7,1);
            upperBound = lowerBound;
            
        end
        
        function [conValue,lowerBound,upperBound] = ArrivalEpoch(obj);
            global phaseList
            phaseIdx = GetPhaseIdxFromName(obj,'LISAPhase1');
            a1Epoch =  34513.39999998195;
            finalTime = obj.GetFinalTime(phaseIdx);
            timeNonDim = obj.scaleUtil.ScaleTime(a1Epoch,phaseList{phaseIdx}.centralBody,phaseList{phaseIdx}.refEpoch);
            conValue = [(timeNonDim - finalTime)];
            lowerBound = zeros(1,1);
            upperBound = lowerBound;
            
        end
        
        function phaseIdx = GetPhaseIdxFromName(obj,phaseName)
            global phaseList
            for iDx = 1:length(phaseList)
                if strcmp(phaseName,phaseList{iDx}.phaseName)
                    phaseIdx = iDx;
                    return
                end
            end
            error(['phase named ' phaseName ' not found in phase list'])
        end
    end
    
end



