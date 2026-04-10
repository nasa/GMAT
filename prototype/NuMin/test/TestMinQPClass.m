%
%   Copyright 2002-2005, United States Government as represented by the
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.
% Options.Display = 1;
% 
% d = [ -2 -5 ]';
% G = 2*eye(2,2);
% A = [-1 -2; -1 2; 1 0; 0 1];
% B = [-6 -2 0 0]';
% 
% 
% x0 = [1 0]';
% W0 = []';
% eqInd = [];
% ineqInd = [1:5]';
% import MinQPClass
% %qpOpt = MinQPClass(x0, G, d, A, B, eqInd, ineqInd, W0, Options, 2);
% 
% Blower = B;
% Bupper = 1e10*ones(size(B,1),1);
% Bupper(eqInd) = B(eqInd);
% Bupper(2) = 2;
% qpOpt = MinQPClass(x0, G, d, A, Blower,Bupper, W0, 2);
% qpOpt.WriteOutput = true;
% [x2, q2, lambda2, Converged2] = qpOpt.Optimize();
% 
% % ===============================================
% 
% d = [ -2 -5 ]';
% G = 2*eye(2,2);
% A = [1 -2; -1 -2; -1 2; 1 0; 0 1];
% B = [-2 -6 -2 0 0]';
% 
% 
% x0 = [1 0]';
% W0 = []';
% eqInd = [];
% ineqInd = [1:5]';
% import MinQPClass
% %qpOpt = MinQPClass(x0, G, d, A, B, eqInd, ineqInd, W0, Options, 2);
% 
% [x1, q1, lambda1, Converged1] = MinQP(x0, G, d, A, B,eqInd,ineqInd, W0, Options, 2);
% Blower = B;
% Bupper = 1e10*ones(size(B,1),1);
% Bupper(eqInd) = B(eqInd);
% qpOpt = MinQPClass(x0, G, d, A, Blower,Bupper, W0, 2);
% qpOpt.WriteOutput = true;
% [x2, q2, lambda2, Converged2] = qpOpt.Optimize();
% 
% if max(abs(x1 - x2)) > 1e-14
%     error('solution is different')
% end
% 
% if max(abs(q1 - q2)) > 1e-14
%     error('solution is different')
% end
% 
% if max(abs(lambda1 - lambda2)) > 1e-15
%     error('solution is different')
% end
% 
% if max(abs(Converged1 - Converged1)) > 1e-15
%     error('solution is different')
% end
% 
% %[x, q, lambda, Converged] = qpOpt.Optimize();
% 
% %G*xstar + d - A'*lambda;
% %[X,FVAL,EXITFLAG,OUTPUT,LAMBDA] = quadprog(G,d,-A,-B,[],[],[],[],x0,[]);
% 
% %%
% % % % % % %==========================================================================
% %
% 
% d = [2 3]';
% G = [ 8 2 ; 2 2 ];
% A = [1 -1; -1 -1;-1 0];
% B = [0;-4;-3];
% 
% x0 = [0 0]';
% W0 = [1:3]';
% eqInd = [];
% ineqInd = [1:3]';
% 
% [x1, q1, lambda1, Converged1] = MinQP(x0, G, d, A, B,eqInd,ineqInd, W0, Options, 2);
% Blower = B;
% Bupper = 1e10*ones(size(B,1),1);
% Bupper(eqInd) = B(eqInd);
% qpOpt = MinQPClass(x0, G, d, A, Blower,Bupper, W0, 2);
% qpOpt.WriteOutput = true;
% [x2, q2, lambda2, Converged2] = qpOpt.Optimize();
% 
% if max(abs(x1 - x2)) > 1e-15
%     error('solution is different')
% end
% 
% if max(abs(q1 - q2)) > 1e-14
%     error('solution is different')
% end
% 
% if max(abs(lambda1 - lambda2)) > 1e-15
%     error('solution is different')
% end
% 
% if max(abs(Converged1 - Converged1)) > 1e-15
%     error('solution is different')
% end
% 
% % %G*x + d - A'*lambda
% % Aeq   = A(eqInd,:); Beq = B(eqInd,:);
% % Aineq = A(ineqInd,:); Bineq = B(ineqInd,:);
% % %[X,FVAL,EXITFLAG,OUTPUT,LAMBDA] = quadprog(G,d,-Aineq,-Bineq,-Aeq,-Beq,[],[],x0,[]);
% % 
% % %%
% % % % % % %==========================================================================
% % d = [-8 -3 -3]';
% % G = [6 2 1; 2 5 2;1 2 4];
% % A = [1 0 1;0 1 1];
% % B = [3 0]';
% % 
% % x0 = [1 1 1]';
% % W0 = []';
% % eqInd   = [1:2]';
% % ineqInd = [];
% % 
% % %[x, q, lambda, Converged] = MinQP(x0, G, d, A, B,eqInd,ineqInd, W0, Options, 2);
% % 
% % [x1, q1, lambda1, Converged1] = MinQP(x0, G, d, A, B,eqInd,ineqInd, W0, Options, 2);
% % Blower = B;
% % Bupper = 1e10*ones(size(B,1),1);
% % Bupper(eqInd) = B(eqInd);
% % qpOpt = MinQPClass(x0, G, d, A, Blower,Bupper, W0, 2);
% % qpOpt.WriteOutput = true;
% % [x2, q2, lambda2, Converged2] = qpOpt.Optimize();
% % 
% % if max(abs(x1 - x2)) > 1e-15
% %     error('solution is different')
% % end
% % 
% % if max(abs(q1 - q2)) > 1e-15
% %     error('solution is different')
% % end
% % 
% % if max(abs(lambda1 - lambda2)) > 1e-15
% %     error('solution is different')
% % end
% % 
% % if max(abs(Converged1 - Converged1)) > 1e-15
% %     error('solution is different')
% % end
% % 
% % % % %==========================================================================
% d = [3.9094
%     8.3138
%     8.0336
%     0.6047
%     3.9926
%     5.2688
%     4.1680
%     6.5686
%     6.2797
%     2.9198];
% 
% G = [   54.8720    9.2338    2.6221    2.6248    5.4681    8.8517    6.5376    7.4407    2.3993    6.8197
%     4.7092  101.5995    6.0284    8.0101    5.2114    9.1329    4.9417    5.0002    8.8651    0.4243
%     2.3049    1.8482   72.0113    0.2922    2.3159    7.9618    7.7905    4.7992    0.2867    0.7145
%     8.4431    9.0488    2.2175   89.3216    4.8890    0.9871    7.1504    9.0472    4.8990    5.2165
%     1.9476    9.7975    1.1742    7.3033   51.6204    2.6187    9.0372    6.0987    1.6793    0.9673
%     2.2592    4.3887    2.9668    4.8861    6.7914   46.5927    8.9092    6.1767    9.7868    8.1815
%     1.7071    1.1112    3.1878    5.7853    3.9552    6.7973   85.8730    8.5944    7.1269    8.1755
%     2.2766    2.5806    4.2417    2.3728    3.6744    1.3655    6.9875   16.4019    5.0047    7.2244
%     4.3570    4.0872    5.0786    4.5885    9.8798    7.2123    1.9781    5.7672   18.0280    1.4987
%     3.1110    5.9490    0.8552    9.6309    0.3774    1.0676    0.3054    1.8292    0.5962   23.9349];
% 
% A = [    0.3724    0.9203    0.5479    0.7011    0.1781    0.5612    0.4607    0.3763    0.5895    0.2904
%     0.1981    0.0527    0.9427    0.6663    0.1280    0.8819    0.9816    0.1909    0.2262    0.6171
%     0.4897    0.7379    0.4177    0.5391    0.9991    0.6692    0.1564    0.4283    0.3846    0.2653
%     0.3395    0.2691    0.9831    0.6981    0.1711    0.1904    0.8555    0.4820    0.5830    0.8244
%     0.9516    0.4228    0.3015    0.6665    0.0326    0.3689    0.6448    0.1206    0.2518    0.9827
%     0.0596   0.68197    0.0424    0.0714    0.5216    0.0967    0.8181    0.8175    0.7224    0.1498];
% 
% B = [0.7302
%     0.3439
%     0.5841
%     0.1078
%     0.9063
%     .8];
% 
% x0 = 4*[ rand(10,1)];
% x0 = rand(10,1);
% eqInd   = [1:3]';
% ineqInd = [4:6]';
% W0 = [];
% %[x, q, lambda, Converged,W] = MinQP(x0, G, d, A, B,eqInd, ineqInd,W0, Options, 2);
% clear x1 q1 lambda1 Converged1 x2 q2 lambda2 Converged2 
% [x1, q1, lambda1, Converged1] = MinQP(x0, G, d, A, B,eqInd,ineqInd, W0, Options, 2);
% %return
% 
% Blower = B;
% Bupper = 1e3*ones(size(B,1),1);
% Bupper(eqInd) = B(eqInd);
% qpOpt = MinQPClass(x0, G, d, A, Blower,Bupper, W0, 2);
% qpOpt.WriteOutput = true;
% [x2, q2, lambda2, Converged2] = qpOpt.Optimize();
% 
% if max(abs(x1 - x2)) > 1e-15
%     error('solution is different')
% end
% 
% if max(abs(q1 - q2)) > 1e-12
%     error('solution is different')
% end
% 
% if max(abs(lambda1 - lambda2)) > 1e-13
%     error('solution is different')
% end
% 
% if max(abs(Converged1 - Converged1)) > 1e-15
%     error('solution is different')
% end


Options.Display = 1;
load Prob1.mat G d A B eqInd ineqInd

% x0 = rand(100,1);
x0 = [0.8147
0.9058
0.127
0.9134
0.6324
0.0975
0.2785
0.5469
0.9575
0.9649
0.1576
0.9706
0.9572
0.4854 
0.8003
0.1419
0.4218
0.9157
0.7922
0.9595
0.6557
0.0357
0.8491
0.934
0.6787
0.7577
0.7431
0.3922
0.6555
0.1712
0.706
0.0318
0.2769
0.0462
0.0971
0.8235
0.6948
0.3171
0.9502
0.0344
0.4387
0.3816
0.7655
0.7952
0.1869
0.4898
0.4456
0.6463
0.7094
0.7547
0.276
0.6797
0.6551
0.1626
0.119
0.4984
0.9597
0.3404
0.5853
0.2238
0.7513
0.2551
0.506
0.6991
0.8909
0.9593
0.5472
0.1386
0.1493
0.2575
0.8407
0.2543
0.8143
0.2435
0.9293
0.35
0.1966
0.2511
0.616
0.4733
0.3517
0.8308
0.5853
0.5497
0.9172
0.2858
0.7572
0.7537
0.3804
0.5678
0.0759
0.054
0.5308
0.7792
0.934
0.1299
0.5688
0.4694
0.0119
0.3371];

eqInd = [1:30]'; ineqInd = [31:75]';
Aeq   = A(eqInd,:); Beq = B(eqInd,:);
Aineq = A(ineqInd,:); Bineq = B(ineqInd,:);
W     =  [];
A     = A([eqInd;ineqInd],:);
B     = B([eqInd;ineqInd],:);
[x1, q1, lambda1, Converged1]       = MinQP(x0, G, d, A, B, eqInd,ineqInd, W, Options, 2);
%[x1, q1, lambda1, Converged1] = MinQP(x0, G, d, A, B,eqInd,ineqInd, W0, Options, 2);
Blower = B;
Bupper = 1e3*ones(size(B,1),1);
Bupper(eqInd) = B(eqInd);
qpOpt = MinQPClass(x0, G, d, A, Blower,Bupper, W0, 2);
qpOpt.WriteOutput = true;

tic
[x2, q2, lambda2, Converged2] = qpOpt.Optimize();
toc

if max(abs(x1 - x2)) > 1e-15
    error('solution is different')
end

if max(abs(q1 - q2)) > 1e-12
    error('solution is different')
end

if max(abs(lambda1 - lambda2)) > 1e-12
    error('solution is different')
end

if max(abs(Converged1 - Converged2)) > 1e-15
    error('solution is different')
end

%[X,FVAL,EXITFLAG,OUTPUT,LAMBDA] = quadprog(G,d,-Aineq,-Bineq,-Aeq,-Beq,[],[],x0,[]);
% disp([num2str(min(A*X - B),4) '  ' num2str(min(A*x2 - B),4) '  ' ...
%     num2str(norm(x2 - X),4) '  ' num2str(q2 - FVAL,4) ]);

% %==========================================================================
% %==========================================================================
% %==========================================================================
% clear x1 q1 lambda1 Converged1 x2 q2 lambda2 Converged2 
% clear G d A B x0
% load Prob2.mat G d A B x0
% %load c:\temp.mat x0
% 
% eqInd = [1:150]'; ineqInd = [151:300]';
% Aeq   = A(eqInd,:); Beq = B(eqInd,:);
% Aineq = A(ineqInd,:); Bineq = B(ineqInd,:);
% W     =  [];
% A     = A([eqInd;ineqInd],:);
% B     = B([eqInd;ineqInd],:);
% [x1, q1, lambda1, Converged1]       = MinQP(x0, G, d, A, B, eqInd,ineqInd, W, Options, 2);
% Blower = B;
% Bupper = 1e3*ones(size(B,1),1);
% Bupper(eqInd) = B(eqInd);
% qpOpt = MinQPClass(x0, G, d, A, Blower,Bupper, W0, 2);
% qpOpt.WriteOutput = true;
% [x2, q2, lambda2, Converged2] = qpOpt.Optimize();
% % [X,FVAL,EXITFLAG,OUTPUT,LAMBDA] = quadprog(G,d,-Aineq,-Bineq,-Aeq,-Beq,[],[],x0,[]);
% % disp([num2str(min(A*X - B),4) '  ' num2str(min(A*x - B),4) '  ' ...
% %     num2str(norm(x - X),4) '  ' num2str(q - FVAL,4) ]);
% 
% if max(abs(x1 - x2)) > 1e-15
%     error('solution is different')
% end
% 
% if max(abs(q1 - q2)) > 1e-12
%     error('solution is different')
% end
% 
% if max(abs(lambda1 - lambda2)) > 1e-12
%     error('solution is different')
% end
% 
% if max(abs(Converged1 - Converged2)) > 1e-15
%     error('solution is different')
% end

%% ================================================================================================
%  Test problem from here: https://www.me.utexas.edu/~jensen/ORMM/supplements/methods/nlpmethod/S2_quadratic.pdf
clear x1 q1 lambda1 Converged1 x2 q2 lambda2 Converged2 
Options.Display = 1;
load Prob1.mat G d A B eqInd ineqInd

x0 = [0 0]';
G = [2 0 ; 0 8];
d = [-18 -16]';
A =[-1 -1;
    1 0;
    0 1;
    -1 0];
bl = [-5 0 0 -3]';
W0 = [];
eqInd = [];
ineqInd = [1 2 3 4]';
[x1, q1, lambda1, Converged1] = MinQP(x0, G, d, A, bl, eqInd,ineqInd, W0, Options, 2);


x0 = [0 0]';
G = [2 0 ; 0 8];
d = [-18 -16]';
A =[1 1;
    1 0;
    0 1];
bl = [-100 0 0 ]';
bu = [5 3 100]';
W0 = [];
qpOpt = MinQPClass(x0, G, d, A, bl, bu, W0, 2);
qpOpt.WriteOutput = true;
[x2, q2, lambda2, Converged2] = qpOpt.Optimize();


if max(abs(x1 - x2)) > 1e-15
    error('solution is different')
end

if max(abs(q1 - q2)) > 1e-12
    error('solution is different')
end

% if max(abs(lambda1 - lambda2)) > 1e-13
%     error('solution is different')
% end

if max(abs(Converged1 - Converged1)) > 1e-15
    error('solution is different')
end

%%==================================================================================================
%  Test providing active set guess.  Should add constraints 63 and 64 then
%  converge
clear x1 q1 lambda1 Converged1 x2 q2 lambda2 Converged2 
Options.Display = 1;
load Prob1.mat G d A B eqInd ineqInd

x0 = rand(100,1);
eqInd = [1:30]'; ineqInd = [31:75]';
Aeq   = A(eqInd,:); Beq = B(eqInd,:);
Aineq = A(ineqInd,:); Bineq = B(ineqInd,:);
W     =  [];
A     = A([eqInd;ineqInd],:);
B     = B([eqInd;ineqInd],:);
[x1, q1, lambda1, Converged1]       = MinQP(x0, G, d, A, B, eqInd,ineqInd, W, Options, 2);
%[x1, q1, lambda1, Converged1] = MinQP(x0, G, d, A, B,eqInd,ineqInd, W0, Options, 2);
Blower = B;
Bupper = 1e3*ones(size(B,1),1);
Bupper(eqInd) = B(eqInd);
W0 = [     31
    32
    33
    34
    35
    36
    37
    38
    39
    40
    41
    42
    43
    44
    45
    55
    56
    57
    60
    61
    69
    71
    73
    %63
    %64
    ];
x0 = [
        0.0500207658905975
        -0.148352286085037
        0.0634536942414626
       -0.0602904530936101
        -0.162786778728289
        0.0865337127018454
        0.0364530926548848
         0.134229479227696
        0.0371819451066342
        0.0945406344124422
         0.108119064179903
       -0.0532084256534078
        -0.124077384155365
      -0.00816341381119916
       -0.0541666102304128
       -0.0628117727647909
        0.0562545527439893
        -0.225135039497738
       0.00423068297895064
      -0.00840553466980912
       -0.0259024409768565
       -0.0105008859396222
         0.177700521540385
         0.128013749309783
       -0.0756283817031003
        0.0749603030842033
        0.0431317974763777
       -0.0420611849514343
       -0.0673360340522439
        0.0912136363657021
       -0.0663493312544001
        -0.031794650748165
        0.0241765953652507
         0.208989326877027
        0.0724489142420715
        0.0962338792209153
       -0.0299291216827375
       -0.0350375219565036
        -0.189461766450043
        0.0418289014393859
         0.239366575172792
         0.109028236060498
         0.275889924647269
        0.0303740341626008
         0.231809905019142
        0.0176702655779077
      -0.00559186892239208
         0.242788298693257
      -0.00405545582441662
       -0.0652164377114922
        -0.266629032618605
      -0.00881241065644572
        -0.156694783307559
        0.0266033423509099
      -0.00973618355564283
        0.0189964262349872
       0.00401809437097511
         0.083211598767005
         0.146173437790251
       -0.0821087371616795
        0.0920368682002114
        -0.126970564120088
      -0.00696109169742398
       -0.0433008579146023
       -0.0234589074903595
     -0.000943365798691845
        -0.185938863946065
        -0.163629375203335
        0.0715388051018542
       -0.0381030721760643
        0.0749499994050926
       -0.0104451090142377
        0.0898287673782725
       -0.0578325654403558
        0.0920973601712847
       -0.0663706219271733
       -0.0217176789263125
         0.108130747405927
        0.0501808361415668
         0.117974260230451
         0.232978539066402
        0.0279064425410996
         0.104798254036747
       -0.0109371316245832
        0.0468200966832241
        0.0288173120564186
        0.0191527487255264
        0.0128482363985886
       -0.0244140051931736
        0.0613222216116667
        0.0140629294847729
         0.154344596754621
       -0.0943187850831211
       -0.0178441915322221
       -0.0209104907798076
        0.0227160441226689
      -0.00253078482257254
       -0.0957242098237662
        0.0285962591813645
      -0.00195915232754417];
qpOpt = MinQPClass(x0, G, d, A, Blower,Bupper, W0, 2);
qpOpt.WriteOutput = true;
[x2, q2, lambda2, Converged2] = qpOpt.Optimize();

if max(abs(x1 - x2)) > 1e-15
    error('solution is different')
end

if max(abs(q1 - q2)) > 1e-12
    error('solution is different')
end

if max(abs(lambda1 - lambda2)) > 1e-12
    error('solution is different')
end

if max(abs(Converged1 - Converged1)) > 1e-15
   error('solution is different')
end