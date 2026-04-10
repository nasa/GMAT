function [stateGuess,controlGuess] = LISAGuessFunction_snapier(timeVector,timeVectorType,phaseObj)

global scaleUtill

guessData  = LISAGuessData_snapier(scaleUtill);
guessTimes = guessData(:,1);
numTimes   = length(timeVector);
stateGuess = zeros(numTimes,18);%zeros(numTimes,7);

%breakpt = 1

%% Build Nondimensional Guess
for i = 1:numTimes
    %     DimTime(i)            = scaleUtill.UnScaleTime(          timeVector(i),...
    %                                                       phaseObj.centralBody,...
    %                                                          phaseObj.refEpoch); % REDIMENSIONALIZED TIME
    
    % LISA 1
%     stateDim_LISA1(i,:)   = interp1(                            guessTimes,...
%                                                           guessData(:,2:7),...
%                                                              timeVector(i),...
%                                                                  'spline'); % [T X] DIMensional interpolated state
%     stateGuess_LISA1(i,:) = transpose(scaleUtill.ScaleState(...
%                                                         stateDim_LISA1(i,:)',...
%                                                      phaseObj.centralBody)); % [T X] NONdimensional interpolated state
%     interp1(                            guessTimes,...
%                                                           guessData(:,2:7),...
%                                                              timeVector(i),...
%                                                                  'spline');                                                                                        
    %x_test = stateDim_LISA1(1,:)';
                                                 %x_test = scaleUtill.UnScaleState(stateGuess_LISA1(i,:)',phaseObj.centralBody);
   stateGuess_LISA1(i,:) = interp1(                            guessTimes,...
                                                          guessData(:,2:7),...
                                                             timeVector(i),...
                                                                 'spline'); 
                                                             
                                                             
                                                             
    % LISA 2
%     stateDim_LISA2(i,:)   = interp1(                            guessTimes,...
%                                                          guessData(:,8:13),...
%                                                              timeVector(i),...
%                                                                  'spline'); % [T X] dimensional interpolated state
%     stateGuess_LISA2(i,:) = transpose(scaleUtill.ScaleState(...
%                                                         stateDim_LISA2(i,:)',...
%                                                      phaseObj.centralBody)); % [T X] nondimensional interpolated state    
%                                                  
    stateGuess_LISA2(i,:) = interp1(                            guessTimes,...
                                                          guessData(:,8:13),...
                                                             timeVector(i),...
                                                                 'spline');                                                  
                                                 
    % LISA 3
%     stateDim_LISA3(i,:)   = interp1(                            guessTimes,...
%                                                         guessData(:,14:19),...
%                                                              timeVector(i),...
%                                                                  'spline'); % [T X] dimensional interpolated state
%     stateGuess_LISA3(i,:) = transpose(scaleUtill.ScaleState(...
%                                                         stateDim_LISA3(i,:)',...
%                                                      phaseObj.centralBody)); % [T X] nondimensional interpolated state    
%                                                  
     stateGuess_LISA3(i,:) = interp1(                           guessTimes,...
                                                          guessData(:,14:19),...
                                                             timeVector(i),...
                                                                 'spline');                                               
   % Coupled interpolated nondimensional LISA constellation state [T X1 X2 X3]
%    stateGuess(i,:) = [stateGuess_LISA1(i,:),...
%                       stateGuess_LISA2(i,:),...
%                       stateGuess_LISA3(i,:)];
%   
stateGuess(i,:) = [stateGuess_LISA1(i,:),...
                   stateGuess_LISA2(i,:),...
                   stateGuess_LISA3(i,:)];
end

%% Guess Control Vector
% for i = 1:numTimes
%    % if timeVectorType(i) == 1
%        controlGuess(i,:) = ~; %[0 0 0];%  [0.0 1 1 1];
%    % end
% end

controlGuess = [];
end

%% Load nominal guess
function guessData = LISAGuessData_snapier(scaleUtill)
t0     = 2464114.99962963-2430000.0; % Make times into A1MJD
times  = load('t_csalt.mat');
states = load('x_csalt.mat');
tt     = times.t_csalt/86400+t0; % Convert seconds to days then shift by the t0 A1MJD epoch
xx     = states.x_csalt;

for ii=1:length(xx)
    % Unscaled
    %guessData(ii,:) = [tt(ii), xx(ii, 1:18)];
    
    % Scale the guess data
    time_scaled(ii)   = scaleUtill.ScaleTime(tt(ii),     3, t0);
    x_scaled_L1(ii,:) = scaleUtill.ScaleState(xx(ii,  1:6)',  3);
    x_scaled_L2(ii,:) = scaleUtill.ScaleState(xx(ii, 7:12)',  3);
    x_scaled_L3(ii,:) = scaleUtill.ScaleState(xx(ii,13:18)',  3);

    guessData(ii,:)   = [   time_scaled(ii), x_scaled_L1(ii,:),...
                          x_scaled_L2(ii,:), x_scaled_L3(ii,:)];
    
end



end







