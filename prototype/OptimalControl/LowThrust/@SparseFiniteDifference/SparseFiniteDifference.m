classdef SparseFiniteDifference < handle
    
    %  Class to conduct Sparse Finite Differencing (based on CPR now)
    %  Last update : 2014-10-13
    %  Author: Youngkwang Kim, youngkwangkim88@gmail.com
        
    properties (SetAccess = 'private')
        numObjFun   % number of object functions, equal to the number of rows of the Jacobian
        numVar      % number of variables, equal to the number of columns of the Jacobian
        SparsityPattern
        IndexSet    % variable Sets for sparse finite differencing. 
                    % Currently,a structurally orthogonal Set (CPR) is
                    % adopted.
        numIndexSet

        EPS = 2.2201e-16; % the small number related to the accuracy of the numerics        
        EPSR = sqrt(2.2201e-16); % the small number related to the size of the perturbation for finite differencing 
%         it must be satisfied that EPSR >= sqrt(EPS) = 1.490e-8.

        ObjClass = [];          % The name (character) of the instance of the objective class 
        ObjGetVarMethod = [];   % they are for the class variable & functions setting & getting.
        ObjSetVarMethod = [];   % Getting Methods require only one input "ObjGetMethod"
        ObjGetFunMethod = [];   % whereas Setting Methods require two input
                                % "ObjClass" and "variables".                            
        
    end
    
    methods        
        function SetSparseFiniteDifference(this,ObjClass,SparsityPattern,ObjSetVarMethod,ObjGetVarMethod,ObjGetFunMethod)
            SetObjClass(this,ObjClass);
            SetSparsityPattern(this,SparsityPattern);
            SetObjSetVarMethod(this,ObjSetVarMethod);
            SetObjGetVarMethod(this,ObjGetVarMethod);
            SetObjGetFunMethod(this,ObjGetFunMethod);
        end
        function SetObjClass(this,ObjClass)
            if isa(ObjClass,'numeric')
                error('SparseFiniteDifference.SetObjClass : the ojective class must not be a numeric, and it must be an instance of the class.')
            else
                this.ObjClass = ObjClass;
            end            
        end        
        function SetSparsityPattern(this,SparsityPattern)
            if ~isnumeric(SparsityPattern)
                error('SparseFiniteDifference : SparsityPattern must be numeric');
            else
                [this.numObjFun, this.numVar]=size(SparsityPattern);                
            end
            if this.numObjFun == 1
                warnMessage=char('SparseFiniteDifference.SetSparsityPattern : there is only one objective function.', ...
                    'Sparse Finite Differencing is not effective.');
                disp(warnMessage)
            end
            if this.numVar == 1
                warnMessage=char('SparseFiniteDifference.SetSparsityPattern : there is only one variable.', ...
                    'Sparse Finite Differencing is not effective.');
                disp(warnMessage)                
            end
            if issparse(SparsityPattern)
                this.SparsityPattern=SparsityPattern;
            else
                this.SparsityPattern=spones(sparse(SparsityPattern));
            end
            this.IndexSet={}; % as sparsity pattern is updated, initialize relatied data
            this.numIndexSet=[];
            this.GetOptIndexSet();
        end
        function SetObjGetVarMethod(this,ObjGetVarMethod)
            if ~isa(ObjGetVarMethod, 'char')
                error('SparseFiniteDifference.SetObjGetVarMethod : ObjGetVarMethod must be a character.')
            end
            this.ObjGetVarMethod = ObjGetVarMethod;
                % ex. will be used as eval(['GetCostConstraintFunctions' '(' 'traj' ')'])                        
        end        
        function SetObjSetVarMethod(this,ObjSetVarMethod)
            if ~isa(ObjSetVarMethod, 'char')
                error('SparseFiniteDifference.SetObjSetMethod : ObjSetVarMethod must be a character.')
            end
            this.ObjSetVarMethod = ObjSetVarMethod;
                % ex. will be used as eval(['SetDecisionVector' '(' 'traj' ',' 'decisionVector' ')'])                        
        end         
        function SetObjGetFunMethod(this,ObjGetFunMethod)
            if ~isa(ObjGetFunMethod, 'char')
                error('SparseFiniteDifference.SetObjGetFunMethod : ObjGetFunMethod must be a character.')
            end
            this.ObjGetFunMethod = ObjGetFunMethod;
                % ex. will be used as eval(['SetDecisionVector' '(' 'traj' ',' 'decisionVector' ')'])                        
        end               
        function SetPerturbationParameter(this,pert)
            if ~isreal(pert) || pert > 1 
                error('SparseFiniteDifference.SetPerturbationParameter : The perturbation parameter must be a real constant (much) smaller than one.')
            elseif pert < sqrt(this.EPS)
                error('SparseFiniteDifference.SetPerturbationParameter : The perturbation parameter EPSR must satisfy that EPSR >= 1.490e-8.')                
            else
                this.EPSR=pert;
            end            
        end
        
        function GetOptIndexSet(this)
            % this method for calculating variable Sets based on CPR
            % algorithm.
            if isempty(this.SparsityPattern)
                error('SparseFiniteDifference.GetOptIndexSet : the sparsity pattern must not be empty matrix.')
            end
            % find the initial non-zero column as the first Set member
      
            nonSet = 1:this.numVar;
            numNonSetMember = length(nonSet);
            
            currentSetIdx= 1;
            
            while numNonSetMember > 0
                currentSet=nonSet(1);   % initialize the current Set
                nonSet(1)=[];             % reflect the Set
                for i=nonSet
                    orthogonality = 1;
                    for j=currentSet
                        if ~this.IsStrOrthogonal(this.SparsityPattern(:,j),this.SparsityPattern(:,i))
                            orthogonality = 0;
                        end
                        if orthogonality == 0
                            break
                        end
                    end
                    if orthogonality == 1
                        currentSet(end+1) = i;% add the column to the current Set
                        location = find(nonSet == i);
                        nonSet(location)=[];  % remove the vector from the non-Set vectors
                    end
                end                
                numNonSetMember = length(nonSet);
                this.IndexSet{currentSetIdx} = currentSet;
                currentSetIdx = currentSetIdx + 1;                
            end
            
            this.numIndexSet = length(this.IndexSet);
            if this.numIndexSet >= this.numVar - 1
                 warnMessage=char('SparseFiniteDifference.GetOptIndexSet : The number of variable Sets is equal to', ...
                     'the number of variables. The sparse finite differencing is not effective');
                 disp(warnMessage)        
            end
        end                
        function answer=IsStrOrthogonal(this,colVec1,colVec2) % only belong to the CPR based SFD algorithm
            % this method for checking whether two columns are structurally 
            % orthogonal. inputs are either sparse or full column vectors
            
            % assume that the answer is no
            answer = 0;
            tmpVec1=colVec1./colVec1; % if the matrix element of ColVec1 is 0 then, it becomes NaN. Otherwise, it becomes 1.            
            tmpVec2=colVec2./colVec2;
            sumResult=tmpVec1+tmpVec2;
            if isempty(find(sumResult == 2,1))
                answer = 1;
            end            
        end
  
        function Jacobian=GetCentralDiff(this)
            if isempty(this.IndexSet)
                this.GetOptIndexSet();
            end            
            if isempty(this.ObjClass) || isempty(this.ObjGetFunMethod) || isempty(this.ObjSetVarMethod) || isempty(this.ObjGetVarMethod) || isempty(this.SparsityPattern)
                error('SparseFiniteDifference.GetCentralDiff : at least one of the object class, setting and getting methods of the class, and the sparsity pattern of the Jacobian are not set, yet.')
            end
            try           
                eval([this.ObjGetFunMethod '( this.ObjClass );']);
            catch exception
                error('Currently, it seems that the ObjGetFunMethod of the objective class is not properly assigned. ')
            end
            
            try           
                currentVar=eval([this.ObjGetVarMethod '( this.ObjClass );']); 
            catch exception
                error('Currently, it seems that the ObjGetVarMethod of the objective class is not properly assigned. ')
            end     
            
            try           
                eval([this.ObjSetVarMethod '( this.ObjClass ,currentVar );']);
            catch exception
                error('Currently, it seems that the ObjSetVarMethod of the objective class is not properly assigned. ')
            end                
            [tmp1, tmp2]=size(currentVar);
            if tmp1*tmp2 ~= length(currentVar)
                error('SparseFiniteDifference.GetCentralDiff : the variable vector of the object class must be given as a row or column vector.')                
            end
            if tmp2 ~= 1
                currentVar = currentVar.';
            end                
            if length(currentVar) < this.numVar                
                error('SparseFiniteDifference.GetCentralDiff : the current Varialbe vector does not match with the dimension of the provided sparisity pattern.')                
            end
            
            Jacobian=spones(this.SparsityPattern);
            for i=1:this.numIndexSet

                IndexSetLength = length(this.IndexSet{i});
                denPertVec=zeros(IndexSetLength,1);

                for j= 1:IndexSetLength 
                    if abs(currentVar(this.IndexSet{i}(j))*this.EPSR^0.6) > this.EPSR
                        denPertVec(j) = currentVar(this.IndexSet{i}(j)).*this.EPSR^0.6; % refered from SOCS user guide p. 199                        
                    elseif currentVar(this.IndexSet{i}(j)) ~= 0
                        denPertVec(j)=sign(currentVar(this.IndexSet{i}(j)))*this.EPSR; % refered from SOCS user guide p. 199
                    elseif currentVar(this.IndexSet{i}(j)) == 0
                        denPertVec(j)=this.EPSR;
                    end
                end
                pertVec = sparse(this.IndexSet{i},1,denPertVec,this.numVar,1);
                forwardPoint = currentVar + pertVec;
                backwardPoint = currentVar - pertVec;
                eval([this.ObjSetVarMethod '(this.ObjClass,forwardPoint );']);
                forwardObjFuns = eval([this.ObjGetFunMethod '(this.ObjClass);']);
                
                eval([this.ObjSetVarMethod '(this.ObjClass,backwardPoint );']);
                backwardObjFuns = eval([this.ObjGetFunMethod '(this.ObjClass);']);
                
                deltaObjFuns=forwardObjFuns-backwardObjFuns;
                eval([this.ObjSetVarMethod '(this.ObjClass ,currentVar );']); % restore the original values
                
                for j=1:IndexSetLength
                    Jacobian(:,this.IndexSet{i}(j))=this.SparsityPattern(:,this.IndexSet{i}(j))/(2*pertVec(this.IndexSet{i}(j))).*deltaObjFuns; % element-wise multiplication
                end
            end                        
        end             
    end
    
end

