
function TestStateConv

clc
mu = 398600.1145;
numReps = 6;
numOrbitReps = 5;
maxError = -1e300;


for orbitIdx = 1:numOrbitReps
    cnt = 0;
    for inputIdx = 1:numReps
        for outputIdx = 1:numReps
            cnt = cnt + 1;
            %  Get state for this test
            stateIn      = GetInputState(orbitIdx,inputIdx,mu);
            %  Convert state from input to ouput rep.
            stateOut     = stateconv(stateIn,inputIdx,outputIdx,mu);
            %  Convert state from output back to input rep.
            stateCompare = stateconv(stateOut,outputIdx,inputIdx,mu);
            %  Compare results of the two conversions
            if IsStateReal(stateOut) && IsStateReal(stateCompare)
                c1 = stateconv(stateIn,inputIdx,1,mu);
                c2 = stateconv(stateCompare,inputIdx,1,mu);
                error(cnt) = norm(c1 - c2);
                disp(['Error   : ' num2str(orbitIdx) '  '  num2str(inputIdx)  '  ' num2str(outputIdx) ' Diff = ' num2str(error(cnt))])
                if error(cnt) > maxError
                   maxError = error(cnt);
                end
            else
                  disp(['Error: ' num2str(orbitIdx) '  '  num2str(inputIdx)  '  ' num2str(outputIdx) ' Not a real number'])
            end
        end
    end
end
disp(['MaxError = ' num2str(maxError,16)]);


function state = GetInputState(orbitIdx,inputIdx,mu)

d2r = pi/180;

if orbitIdx == 1
   oe = [42095 0 0 0 0 pi/2]; 
elseif orbitIdx == 2;
   oe = [82095 .8181 0 2 3 5];
elseif orbitIdx == 3;
    oe = [7500 0 pi/2 0 0 3*pi/2];
elseif orbitIdx == 4
    oe = [6578 .02 28.5*d2r 3 6 2];
elseif orbitIdx == 5;
    oe = [6578 .02 98*d2r 3 6 2];
elseif orbitIdx == 6;
    oe = [6578 0 pi 3 6 2];
end

state = stateconv(oe,2,inputIdx,mu);

function flag = IsStateReal(state)

flag = 1;
if isempty(state)
    flag = 0;
    return
end

for idx = 1:6
    if isnan(state(idx)) || isinf(state(idx))
        flag = 0;
    end
end
