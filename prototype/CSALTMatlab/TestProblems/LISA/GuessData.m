%===================================================================================================
%=== User inputs
%===================================================================================================
clear all
numPoints = 50;
initEpoch = 34050.99962787928 ;
finalEpoch = 34513.39999998195 ;
GM = 132712439935.4841;
initialMass = 1900;
finalMass = 1800; 

lisaInitKep = [158797392.9412244         0.06209002601054015       0.6605491672023329          176.9700773888353            358.9160899822751           12.85647633256843];        
lisaFinalKep = [149436673.8920391         0.004873989367471467      0.4762197574981971        8.814958376535856         89.77322309553581         161.7792714662273 ];
%lisaFinalKep = [149433983.7949498  0.004838228485697236  0.4817938398475211        128.9348351417585         88.41841224691763         43.20982521128722]
%lisaFinalKep = [149434202.1334874         0.004756232072251762      0.4785040526395089        248.2430507570471         90.00028536520144         281.4045863579698         ];

%===================================================================================================
%=== Convert inputs and set constants
%===================================================================================================
d2r = pi/180;
lisaFinalKep(3) = lisaFinalKep(3)*d2r;
lisaFinalKep(4) = lisaFinalKep(4)*d2r;
lisaFinalKep(5) = lisaFinalKep(5)*d2r;
lisaFinalKep(6) = lisaFinalKep(6)*d2r;

lisaInitKep(3) = lisaInitKep(3)*d2r;
lisaInitKep(4) = lisaInitKep(4)*d2r;
lisaInitKep(5) = lisaInitKep(5)*d2r;
lisaInitKep(6) = lisaInitKep(6)*d2r;

iDx =1;
times = linspace(initEpoch,finalEpoch,numPoints);
masses = linspace(initialMass,finalMass,numPoints);

cartStateInit = Kep2Cart(lisaInitKep,GM);
modEqElemsInit = Cart2Mee(cartStateInit,GM);

cartStateFinal = Kep2Cart(lisaFinalKep,GM);
modEqElemsFinal = Cart2Mee(cartStateFinal,GM);
modEqElemsFinal(6) = modEqElemsFinal(6) + 2*pi; 

for iDx = 1:6
   kepElems(:,iDx) = linspace(lisaInitKep(iDx) ,lisaFinalKep(iDx),numPoints)';
   modEqElems(:,iDx) = linspace(modEqElemsInit(iDx) ,modEqElemsFinal(iDx),numPoints)';
end

numRevs = 0;
lastTLONG = 0;
for rowIdx = 1:size(kepElems,1)
%     cartState = Kep2Cart(kepElems(rowIdx,:),GM);
%     modEqElems = Cart2Mee(cartState,GM);
%    % modEqElems(6)*180/pi
%     if modEqElems(6) < pi && lastTLONG > pi
%         numRevs = numRevs + 1;
%     end
%     lastTLONG = modEqElems(6);
%     modEqElems(6) = modEqElems(6) + numRevs*2*pi;
    guessData(rowIdx,1) = times(rowIdx);
    guessData(rowIdx,2:7) = modEqElems(rowIdx,:);
    guessData(rowIdx,8) = masses(rowIdx);
    guessData(rowIdx,9:12) = [.1 1 1 1];
    data = sprintf('%16.12g ', guessData(rowIdx,:));
    disp(data)
end
