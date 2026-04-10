function CPRUnitTestDriver%% Define the toy problem

numVar  = 8;
denseJac = zeros(numVar,numVar);
for funIdx = 1:numVar
    for varIdx = funIdx:1:funIdx+1
        denseJac(funIdx,varIdx) = 1;
    end
end
denseJac
[i,j,f] = find(denseJac);
sparsityPattern = sparse(i,j,f)
IndexSet = GetOptIndexSet(numVar,sparsityPattern)

    %% Use CPR algorithm to determine index sets
    function IndexSet = GetOptIndexSet(numVar,sparsityPattern)
        nonSet = 1:numVar;
        numNonSetMember = length(nonSet);
        currentSetIdx= 1;
        
        while numNonSetMember > 0
            currentSet=nonSet(1);   % initialize the current Set
            nonSet(1)=[];             % reflect the Set
            for i=nonSet
                orthogonality = 1;
                for j=currentSet
                    if ~IsStrOrthogonal(sparsityPattern(:,j),sparsityPattern(:,i))
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
            IndexSet{currentSetIdx} = currentSet;
            currentSetIdx = currentSetIdx + 1;
            
            numIndexSet = length(IndexSet);
            if numIndexSet >= numVar - 1
                warnMessage=char(['SparseFiniteDifference.GetOptIndexSet : ' ...
                    'The number of variable Sets is equal to'], ...
                    ['the number of variables. The sparse finite ' ...
                    'differencing is not effective']);
                disp(warnMessage)
            end
        end
    end

    %%  Determine orthogonality
    function answer=IsStrOrthogonal(colVec1,colVec2) 
        % only belong to the CPR based SFD algorithm
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
end
