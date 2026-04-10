%  Report data about the problem
function WriteSetupReport(obj,includeSolution)

% If this is a solution write out information on the solution. 
if nargin < 2
    includeSolution = false();
end

% number of phases
disp(' -----------------------------------------------------------');
disp(' ----------- GMAT Optimal Control SetUp Summary ------------');
disp(' -----------------------------------------------------------');

%  Write out function data
for phaseIdx = 1:obj.numPhases;
    str = sprintf('%s %3i',' PHASE NUMBER        : ',...
        phaseIdx);
    disp(str);
    %  Dynamics function information
    str = sprintf('%s %3i %s  %3i',' Dynamics Functions  : ',...
        obj.phaseList{phaseIdx}.PathFunction.numDynFunctions,...
        '    Dynamics Constraints  :',...
        obj.phaseList{phaseIdx}.numDefectConstraints);
    disp(str)
    %  Algebraic path function information
    str = sprintf('%s %3i %s  %3i',' Algebraic Functions : ',...
        obj.phaseList{phaseIdx}.PathFunction.numAlgFunctions,...
        '    Algebraic Constraints :',...
        obj.phaseList{phaseIdx}.numPathConstraints);
    disp(str)
    %  Integral Function information
    str = sprintf('%s %3i %s  %3i',' Integral Functions  : ',...
        obj.phaseList{phaseIdx}.PathFunction.numIntFunctions,...
        '    Integral Constraints  :',...
        obj.phaseList{phaseIdx}.numEventConstraints);
    disp(str)
    %  Point function information
    str = sprintf('%s %3i %s  %3i',' Point Functions     : ',...
        obj.phaseList{phaseIdx}.PointFunction.numEventFunctions,...
        '    Total Num Constraints :',...
        obj.phaseList{phaseIdx}.numConstraints);
    disp(str)
    
    %% Cost Function information (integral)
    if obj.phaseList{phaseIdx}.PathFunction.hasCostFunction
        flagStr = 'true';
    else
        flagStr = 'false';
    end
    str = sprintf('%s %s',' Has Integral Cost   : ',flagStr);
    disp(str)
    %  Cost Function information (point)
    if obj.phaseList{phaseIdx}.PointFunction.hasCostFunction
        flagStr = 'true';
    else
        flagStr = 'false';
    end
    str = sprintf('%s %s',' Has Point Cost      : ',flagStr);
    disp(str)
    disp('  ')
    
    %%  Write out parameterization data
    str = sprintf('%s %3i %s  %3i',' Number of States    : ',...
        obj.phaseList{phaseIdx}.numStates,...
        '    State Parameters      :',...
        obj.phaseList{phaseIdx}.numStateParams);
    disp(str)
    str = sprintf('%s %3i %s  %3i',' Number of Controls  : ',...
        obj.phaseList{phaseIdx}.numControls,...
        '    Control Parameters    :',...
        obj.phaseList{phaseIdx}.numControlParams);
    disp(str)
    str = sprintf('%s %4i',' Decision Parameters : ',...
        obj.phaseList{phaseIdx}.numDecisionParams);
    disp(str)
    
    %%  Write out sparsity information
    
    %%  Write out constraint data
    disp('  ')
    if includeSolution
        conStatus = obj.phaseList{phaseIdx}.CheckConstraintVariances();
        str = sprintf('%s %3i %s  %3i',' Number of Defect Violations  : ',...
            conStatus.numdefectViolations);
        disp(str)
        str = sprintf('%s %3i %s  %3i',' Number of Path Violations    : ',...
            conStatus.numEventViolations);
        disp(str)
        str = sprintf('%s %3i',       ' Number of Event Violations   : ',...
            conStatus.numPathViolations);
        disp(str)
        
    end
    
    %  Write space between sections if not the last section
    if phaseIdx ~= obj.numPhases
        disp(' -----------------------------------------------------------')
    end
    
end

%%  Write out sparsity information
disp(' -----------------------------------------------------------');
disp(' ');
headerText  = ['                  Dec. Vec.     Defect    Algebraic' ...
    '  Integral   Point      Linkage'];
dividerText = ['                 ===================================' ...
    '==============================='];
for phaseIdx = 1:obj.numPhases
    
    %%  Decision Variable Chunk Indeces
    phaseStartIdx = obj.decVecStartIdx(phaseIdx);
    stateStart    = phaseStartIdx;
    stateStop     = phaseStartIdx + ...
        obj.phaseList{phaseIdx}.DecVector.stateStopIdx;
    controlStart = phaseStartIdx + ...
        obj.phaseList{phaseIdx}.DecVector.controlStartIdx;
    controlStop = phaseStartIdx + ...
        obj.phaseList{phaseIdx}.DecVector.controlStopIdx;
    timeStart = phaseStartIdx + ...
        obj.phaseList{phaseIdx}.DecVector.timeStartIdx;
    timeStop = phaseStartIdx + ...
        obj.phaseList{phaseIdx}.DecVector.timeStopIdx;
    
    %%  Constraint Chunk Indeces
    conStartIdx = obj.conPhaseStartIdx(phaseIdx);
    defectStart = conStartIdx;
    defectStop  = conStartIdx + ...
        obj.phaseList{phaseIdx}.defectEndIdx;
    defectStart = conStartIdx;
    defectStop  = conStartIdx + ...
        obj.phaseList{phaseIdx}.defectEndIdx;
    algStart = conStartIdx + ...
        obj.phaseList{phaseIdx}.pathStartIdx;
    algStop = conStartIdx + ...
        obj.phaseList{phaseIdx}.pathEndIdx;
    pointStart = conStartIdx + ...
        obj.phaseList{phaseIdx}.eventStartIdx;
    pointStop = conStartIdx + ...
        obj.phaseList{phaseIdx}.eventEndIdx;
    
    %% Decision Parameter Chunk Strings
    stateString =  [ sprintf('%04d',stateStart) ':' ...
        sprintf('%04d',stateStop)];
    controlString = [ sprintf('%04d',controlStart) ':' ...
        sprintf('%04d',controlStop)];
    timeString = [ sprintf('%04d',timeStart) ':' ...
        sprintf('%04d',timeStop)];
    
    defectString =  [ sprintf('%04d',defectStart) ':' ...
        sprintf('%04d',defectStop)];
    algString = [ sprintf('%04d',algStart) ':' ...
        sprintf('%04d',algStop)];
    pointString = [ sprintf('%04d',pointStart) ':' ...
        sprintf('%04d',pointStop)];
    
    %%  Decision Variable Chunk Indeces
    disp(['          Defect  Constraints     Path Constaints' ...
        '     Event Constraints  ']);
    phaseNum = num2str(phaseIdx);
    disp(['Phase ' phaseNum '  =====================================' ...
        '==========================='])
    disp(['State   | ' stateString ',' defectString ' | ' stateString ',' ...
        algString ' | ' stateString ',' pointString]);
    disp(['Control | ' controlString ',' defectString ' | ' controlString ',' ...
        algString ' | ' controlString ',' pointString]);
    disp(['Time    | ' timeString ',' defectString ' | ' timeString ',' ...
        algString ' | ' timeString ',' pointString]);
    
end

% disp(' -----------------------------------------------------------');
% disp(' -----------------------------------------------------------');
% disp(' -----------------------------------------------------------');
end