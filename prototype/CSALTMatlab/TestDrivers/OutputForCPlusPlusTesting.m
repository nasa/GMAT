function OutputForCPlusPlusTesting(funcData)

for dataIdx = 1:length(funcData)-1
    %  object creation code snippets
    className = ['thePointData' num2str(dataIdx)];
    disp(['UserPathFunctionData ' className ';']);
    
    %  Number of functions
    disp([className '.SetIsInitializing(true);']);
    data = funcData{dataIdx};
    numFunctions = data.GetNumFunctions();
    disp([className '.SetNumFunctions('  num2str(numFunctions) ');'])
    
    %  Function Values
    disp('//Set Function Values')
    funString = ['funValues' num2str(dataIdx)];
    WriteVectorCode(funString,data.GetFunctionValues());
    disp([className '.SetFunctions(' funString ');'])
    
    %  State Jacobian
    disp('//Set State Jacobian')
    jacStringState = ['jacValuesState' num2str(dataIdx)];
    WriteMatrixCode(jacStringState,data.GetStateJac());
    disp([className '.SetStateJacobian(' jacStringState ');'])
    
    %  Control Jacobian
    disp('//Set Control Jacobian')
    jacStringControl = ['jacValuesControl' num2str(dataIdx)];
    WriteMatrixCode(jacStringControl,data.GetControlJac());
    disp([className '.SetControlJacobian(' jacStringControl ');'])
    
    %  Time Jacobian
    disp('//Set Time Values')
    jacStringTime = ['jacValuesTime' num2str(dataIdx)];
    WriteMatrixCode(jacStringTime,data.GetTimeJac());
    disp([className '.SetControlJacobian(' jacStringTime ');'])
    
    %  State NLP Data
    meshIdx = data.GetMeshIdx();
    stageIdx = data.GetStageIdx();
    stateIdxs = data.GetStateIdxs();
    controlIdxs = data.GetControlIdxs();
    
    disp('//Set State Indeces')
    stateIdxString = ['stateIdxValues' num2str(dataIdx)];
    WriteIntegerArrayCode(stateIdxString,stateIdxs);
    
    disp('//Set Control Indeces')
    controlIdxString = ['controlIdxValues' num2str(dataIdx)];
    WriteIntegerArrayCode(controlIdxString,controlIdxs);
    
    disp([className '.SetNLPData(' num2str(meshIdx) ',' num2str(stageIdx) ',' stateIdxString ',' controlIdxString ');'])
    %currentData.GetLowerBounds()
    %currentData.GetUpperBounds()
    %currentData.GetControlJac()
    %currentData.GetStateJac()
    %currentData.GetTimeJac()
    %currentData.GetStateIdxs()
    %currentData.GetControlIdxs()
    disp([className  '.SetIsInitializing(false);']);
    disp(' ')
end
disp(['std::vector<UserPathFunctionData> funcData(' num2str(length(funcData)-1) ');'])
for dataIdx = 1:length(funcData)-1
    %  object creation code snippets
    className = ['thePointData' num2str(dataIdx)];
    disp(['funcData.at( ' num2str(dataIdx-1) ' )=' className ';']);
end

end

function WriteMatrixCode(arrayName,arrayData)
[numRows,numCols] = size(arrayData);
disp(['Rmatrix ' arrayName '(' num2str(numRows) ',' num2str(numCols)  ');'])
for rowIdx = 1:numRows
    for colIdx = 1:numCols
        indexString = ['(' num2str(rowIdx) ',' num2str(colIdx) ')'];
        dataString = [num2str(arrayData(rowIdx,colIdx),16)];
        totalStr = [arrayName indexString ' = ' dataString ';'];
        disp(totalStr);
    end
end
end

function WriteIntegerArrayCode(arrayName,arrayData)
[numRows] = length(arrayData);
disp(['IntegerArray ' arrayName '(' num2str(numRows) ',0);'])
intString = ['arr' arrayName];
arrString = ['int ' intString ' [] = { '];
symbol = '';
for rowIdx = 1:numRows
    arrString = [arrString symbol num2str(arrayData(rowIdx))];
    symbol = ' , ';
end
arrString = [arrString ' };'];
funcString = [arrayName '.insert(' arrayName '.begin()'];
funcString = [funcString ',' intString ',' intString '+' num2str(numRows) ');'];
disp(arrString)
disp(funcString)
end

function WriteVectorCode(vectorName,arrayData)
[numRows] = length(arrayData);
disp(['Rvector ' vectorName '(' num2str(numRows) ',1);'])
for rowIdx = 1:numRows
    indexString = ['(' num2str(rowIdx)  ')'];
    dataString = [num2str(arrayData(rowIdx),16)];
    totalStr = [vectorName indexString ' = ' dataString ';'];
    disp(totalStr);
end
end