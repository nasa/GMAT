classdef JacobianData < handle
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
        % Real Array. The Jacobian of boundary functions w/r/t final time
        finalTimeJac
        % Real Array. The Jacobian of boundary functions w/r/t final state
        finalStateJac
        % Real Array. The Jacobian of boundary functions w/r/t initial time
        initTimeJac
        % Real Array. The Jacobian of boundary functions w/r/t initial state
        initStateJac
        
        % Vector of Integer Matrices. Sparsity of Jac. of boundary functions w/r/t time
        initTimeJacPattern
        % Vector of Integer Matrices. Sparsity of Jac. of boundary functions w/r/t state
        initStateJacPattern
        % Vector of Integer Matrices. Sparsity of Jac. of boundary functions w/r/t time
        finalTimeJacPattern
        % Vector of Integer Matrices. Sparsity of Jac. of boundary functions w/r/t state
        finalStateJacPattern
        
        % Vector of bools.  Indicates if phase bound funcs have init time state dependency
        hasInitTimeDepend
        % Vector of bools.  Indicates if phase bound funcs have init state dependency
        hasInitStateDepend
        % Vector of bools.  Indicates if phase bound funcs have final time dependency
        hasFinalTimeDepend
        % Vector of bools.  Indicates if phase bound funcs have final state dependency
        hasFinalStateDepend
        
        %  Vector of integer arrays.  Probably should be c-style in GMAT
        initialStateIdxs
        %  Vector of integer arrays.  Probably should be c-style in GMAT
        finalStateIdxs
        %  Vector of integer arrays.  Probably should be c-style in GMAT
        initialTimeIdxs
        %  Vector of integer arrays.  Probably should be c-style in GMAT
        finalTimeIdxs
        
        %  Integer.  Number of phases
        numPhases
        %  Bool.  Flag indicating
        hasFunctions
        %  Integer. Number of functions
        numFunctions
        %  Vector of pointers to all phases
        phaseList
        %  Vector of integers.  Contains start index of each phase dec.
        %  vec. into total dec. vec.
        decVecStartIdxs
    end
    
    methods
        
        function Initialize(obj,numFunctions,hasFunction,phaseList,decVecStartIdxs)
            %  Intialized the class
            obj.hasFunctions = hasFunction;
            obj.numFunctions = numFunctions;
            obj.numPhases = length(phaseList);
            obj.phaseList = phaseList;
            obj.decVecStartIdxs = decVecStartIdxs;
            obj.InitJacobianArrays(phaseList);
            obj.InitDataIndeces()
        end
        
        function InitDataIndeces(obj)
            %  Set indeces of different variable types
            for phaseIdx = 1:obj.numPhases
                obj.initialStateIdxs{phaseIdx} = ...
                    obj.phaseList{phaseIdx}.DecVector.GetInitialStateIdxs() + ...
                    obj.decVecStartIdxs(phaseIdx) - 1;
                obj.finalStateIdxs{phaseIdx} = ...
                    obj.phaseList{phaseIdx}.DecVector.GetFinalStateIdxs() + ...
                    obj.decVecStartIdxs(phaseIdx) - 1;
                obj.initialTimeIdxs{phaseIdx} = ...
                    obj.phaseList{phaseIdx}.DecVector.GetInitialTimeIdx() + ...
                    obj.decVecStartIdxs(phaseIdx) - 1;
                obj.finalTimeIdxs{phaseIdx} = ...
                    obj.phaseList{phaseIdx}.DecVector. GetFinalTimeIdx() + ...
                    obj.decVecStartIdxs(phaseIdx) - 1;
            end
        end
        
        function InitJacobianArrays(obj,phaseList)
            %  Initializes/dimensions boundary function data members
            for phaseIdx = 1:obj.numPhases
                obj.initTimeJacPattern{phaseIdx} = ...
                    zeros(obj.numFunctions,1);
                obj.initStateJacPattern{phaseIdx}  = ...
                    zeros(obj.numFunctions,phaseList{phaseIdx}.Config.GetNumStateVars());
                obj.finalTimeJacPattern{phaseIdx} = ...
                    zeros(obj.numFunctions,1);
                obj.finalStateJacPattern{phaseIdx}  = ...
                    zeros(obj.numFunctions,phaseList{phaseIdx}.Config.GetNumStateVars());
                obj.initTimeJac{phaseIdx} = ...
                    zeros(obj.numFunctions,1);
                obj.initStateJac{phaseIdx}  = ...
                    zeros(obj.numFunctions,phaseList{phaseIdx}.Config.GetNumStateVars());
                obj.finalTimeJac{phaseIdx} = ...
                    zeros(obj.numFunctions,1);
                obj.finalStateJac{phaseIdx}  = ...
                    zeros(obj.numFunctions,phaseList{phaseIdx}.Config.GetNumStateVars());
            end
        end
        
        
    end
    
end

