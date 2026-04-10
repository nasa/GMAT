classdef Optimizer < handle
    %OPTIMIZER The base class for numerical optimizers
    
    properties
        %  Use Old function call interface or new
        %     Options 'Old' and 'New'
        interfaceVersion = 'New'; % 'New';
        MRCount = 0;
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
            snseti('Summary file', 1)
            snprint('SNOPTLogFile.txt');
            % Print CPU times at bottom of SNOPT print fileR
            snseti('Timing level',3);
            % Echo SNOPT Output to MATLAB Command Window
            snscreen on;
            %  Set the SNOPT internal scaling option
            snseti('Scale Option', 1)
            %  Set tolerance on optimality
            snset('Major optimality tolerance 1e-3')
            %  Set tolerance on feasibility
            snset('Major feasibility tolerance 5e-7')
            %  Set number of iterations
            snseti('Major iterations limit',75);
            %  Set number of iterations
            snseti('Iterations limit',300000);
            %snseti('Superbasics limit',300000)
            snseti('Major print level',1);
            snseti('Minor print level',0);
            
        end
        
		function SetScaling(obj,ifScaling)
			% Set whether to use SNOPT's internal scaling or 
			% whether the problem was scaled on the GMAT side
			if ifScaling
				snseti('Scale Option', 0);
			else
				snseti('Scale Option', 1);
            end
		end
		
        function [z,F,xmul,Fmul,info] = Optimize(obj,decVec,decVecLB, ...
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
            
            if strcmp(obj.interfaceVersion,'New')
				
                [z,F,xmul,Fmul,info]... %,xstate,Fstate,ns,...
                    ... %ninf,sinf,mincw,miniw,minrw]...
                    = snsolve(decVec,decVecLB,decVecUB,zmul,zstate,...
                    funLB,funUB,Fmul,Fstate,...
                    ObjAdd,ObjRow,AA,iAfun,jAvar,iGfun,jGvar,userfun);            
               
			elseif strcmp(obj.interfaceVersion,'old')
%                 specfile= which('Optimizer.spec');
%                 snspec(specfile)
% %                 Set the derivative option
%                 snseti('Derivative Option',0);
%                 % Set the derivative verification level
%                 snseti('Verify Level',-1);
%                 % Set name of SNOPT print file
%                 snprint('snoptmain.out');
%                 % Print CPU times at bottom of SNOPT print file
%                 snseti('Timing level',3);
%                 % Echo SNOPT Output to MATLAB Command Window
%                 snscreen on;
%                 %  Set the SNOPT internal scaling option
%                 snseti('Scale Option', 1)
%                 %  Set tolerance on optimality
%                 snset('Major optimality tolerance 1e-8')
%                 %  Set tolerance on feasibility
%                 snset('Major feasibility tolerance 1e-8')
%                 %  Set number of iterations
%                 snseti('Major iterations limit',3000);
%                 %  Set number of iterations
%                 snseti('Iterations limit',300000);           

                [z,F,info,xmul,Fmul]...
                    = snopt(decVec,decVecLB,decVecUB,zmul,zstate,...
                    funLB,funUB,Fmul,Fstate,userfun, ...
                    ObjAdd,ObjRow,AA,iAfun,jAvar,iGfun,jGvar);
                snend
                obj.MRCount = obj.MRCount + 1;
                
            else
                error('Unsupported SNOPT Interface')
            end
            
        end
        
        function SetOptimalityTol(~,tol)
			% Set the optimality tolerance
            stringTol = num2str(tol);
            snset(['Major optimality tolerance ' stringTol])
        end
        
        function SetFeasibilityTol(~,tol)
			% Set the feasibility tolerance
            stringTol = num2str(tol);
            snset(['Major feasibility tolerance ' stringTol])
        end
        
        function SetMaxTotalIterations(~,value)
			% Set the max total iterations
            stringVal = num2str(value);
            snset(['Iterations limit ' stringVal]);
        end
        
        function SetMaxMajorIterations(~,value)
			% Set max major iterations
            stringVal = num2str(value);
            snset(['Major iterations limit ' stringVal]);
        end
        
    end
    
end

