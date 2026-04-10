classdef Optimizer < handle
    %OPTIMIZER The base class for numerical optimizers
    
    properties
        %  Use Old function call interface or new
        %     Options 'Old' and 'New'
        interfaceVersion = 'Old';
    end
    
    methods
        
        function Initialize(obj)
            %  Initialize the optimizer configuration
            
            global igrid iGfun jGvar 
            
            %%  TODO: Don't hard code these
            % Set the derivative option
            snseti('Derivative Option',0);
            % Set the derivative verification level
            snseti('Verify Level',-1);
            % Set name of SNOPT print file
            snprint('snoptmain.out');
            % Print CPU times at bottom of SNOPT print file
            snseti('Timing level',3);
            % Echo SNOPT Output to MATLAB Command Window
            snscreen on;
            %  Set the SNOPT internal scaling option
            %snseti('Scale Option', 1)
            %  Set tolerance on optimality
            snset('Major optimality tolerance 1e-3')
            %  Set tolerance on feasibility
            snset('Major feasibility tolerance 1e-6')
            
        end
        
        function [z,F,xmul,Fmul] = Optimize(obj,decVec,decVecLB, ...
                decVecUB,funLB,funUB,sparsityPattern)
                 %  Run the optimizer
            
            global igrid iGfun jGvar 
            
            % Set initial guess on basis and Lagrange multipliers to zero
            zmul = zeros(size(decVec));
            zstate = zmul;
            Fmul = zeros(size(funLB));
            Fstate = Fmul;
            ObjAdd = 0;
            ObjRow = 1;
           
            % Assume for simplicity that all constraints are nonlinear
            AA      = [];
            iAfun   = [];
            jAvar   = [];
            userfun = 'SNOPTFunctionWrapper';
            
            %  Compute sparsity in form SNOPT needs
            [iGfun,jGvar] = find(sparsityPattern);
            
            if strcmp(obj.interfaceVersion,'Old')
                [z,F,xmul,Fmul,info,xstate,Fstate,ns,...
                    ninf,sinf,mincw,miniw,minrw]...
                    = snsolve(decVec,decVecLB,decVecUB,zmul,zstate,...
                    funLB,funUB,Fmul,Fstate,...
                    ObjAdd,ObjRow,AA,iAfun,jAvar,iGfun,jGvar,userfun);
                
            elseif strcmp(obj.interfaceVersion,'New')
                [z,F,info,xmul,Fmul]...
                    = snsolve(decVec,decVecLB,decVecUB,zmul,zstate,...
                    Fmin,Fmax,Fmul,Fstate,userfun, ...
                    ObjAdd,ObjRow,AA,iAfun,jAvar,iGfun,jGvar);
            else
                error('Unsupported SNOPT Interface')
            end
            
        end
    end
    
end

