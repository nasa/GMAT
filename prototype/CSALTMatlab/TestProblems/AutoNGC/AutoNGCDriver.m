

for test = 1:300
    clear all 
    close all 
    gravParam = 398600.4415;
    global initMassKg
    % Clears persistent variable used in Guess function
    clear AutoNGCGuessFunction
    
    % Create random guess
    kepState = [7000 + 400*randn;
        rand/100;
        pi/2*rand;
        2*pi*rand;
        2*pi*rand;
        2*pi*rand]
    
%     kepState= [ 6600
%                  0
%                  0
%                  0
%                  0
%                  0
%                 ];

    cartState = Kep2Cart(kepState,gravParam);
    initMassKg = 1000; % [kg]   

    % Create  the interface and optimize 
    ngcRunner = AutoNGCInterface(cartState,initMassKg);
    [z,F,xmul,Fmul,info] = ngcRunner.Optimize()
    %ngcRunner.PlotSolution();
    % Stop if SNOPT failed to converge
    if info ~= 1
        keyboard
    end
    
end