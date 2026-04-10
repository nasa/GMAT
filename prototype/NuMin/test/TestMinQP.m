%
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.
Options.Display = 1;
d = [ -2 -5 ]';
G = 2*eye(2,2);
A = [1 -2; -1 -2; -1 2; 1 0; 0 1];
B = [-2 -6 -2 0 0]';


x0 = [1 0]';
W0 = []';
eqInd = [];
ineqInd = [1:5]';

[xstar, q, lambda, Converged] = MinQP(x0, G, d, A, B, eqInd, ineqInd,...
                                                           W0, Options, 2);
                                                       
                                                       
G*xstar + d - A'*lambda                                                       
[X,FVAL,EXITFLAG,OUTPUT,LAMBDA] = quadprog(G,d,-A,-B,[],[],[],[],x0,[]);

%                                                                                         
% % % % % %==========================================================================
d = [2 3]';
G = [ 8 2 ; 2 2 ];
A = [1 -1; -1 -1;-1 0];
B = [0;-4;-3];

x0 = [0 0]';
W0 = [1:3]';
eqInd = [];
ineqInd = [1:3]';

[x, q, lambda, Converged] = MinQP(x0, G, d, A, B,eqInd,ineqInd, W0, Options, 2);

%G*x + d - A'*lambda 
Aeq   = A(eqInd,:); Beq = B(eqInd,:);
Aineq = A(ineqInd,:); Bineq = B(ineqInd,:);
[X,FVAL,EXITFLAG,OUTPUT,LAMBDA] = quadprog(G,d,-Aineq,-Bineq,-Aeq,-Beq,[],[],x0,[]);
% % % % %==========================================================================
d = [-8 -3 -3]';
G = [6 2 1; 2 5 2;1 2 4];
A = [1 0 1;0 1 1];
B = [3 0]';

x0 = [1 1 1]';
W0 = []';
eqInd   = [1:2]';
ineqInd = [];

[x, q, lambda, Converged] = MinQP(x0, G, d, A, B,eqInd,ineqInd, W0, Options, 2);


% % %==========================================================================
d = [3.9094
    8.3138
    8.0336
    0.6047
    3.9926
    5.2688
    4.1680
    6.5686
    6.2797
    2.9198];

G = [   54.8720    9.2338    2.6221    2.6248    5.4681    8.8517    6.5376    7.4407    2.3993    6.8197
    4.7092  101.5995    6.0284    8.0101    5.2114    9.1329    4.9417    5.0002    8.8651    0.4243
    2.3049    1.8482   72.0113    0.2922    2.3159    7.9618    7.7905    4.7992    0.2867    0.7145
    8.4431    9.0488    2.2175   89.3216    4.8890    0.9871    7.1504    9.0472    4.8990    5.2165
    1.9476    9.7975    1.1742    7.3033   51.6204    2.6187    9.0372    6.0987    1.6793    0.9673
    2.2592    4.3887    2.9668    4.8861    6.7914   46.5927    8.9092    6.1767    9.7868    8.1815
    1.7071    1.1112    3.1878    5.7853    3.9552    6.7973   85.8730    8.5944    7.1269    8.1755
    2.2766    2.5806    4.2417    2.3728    3.6744    1.3655    6.9875   16.4019    5.0047    7.2244
    4.3570    4.0872    5.0786    4.5885    9.8798    7.2123    1.9781    5.7672   18.0280    1.4987
    3.1110    5.9490    0.8552    9.6309    0.3774    1.0676    0.3054    1.8292    0.5962   23.9349];

A = [    0.3724    0.9203    0.5479    0.7011    0.1781    0.5612    0.4607    0.3763    0.5895    0.2904
        0.1981    0.0527    0.9427    0.6663    0.1280    0.8819    0.9816    0.1909    0.2262    0.6171
        0.4897    0.7379    0.4177    0.5391    0.9991    0.6692    0.1564    0.4283    0.3846    0.2653
        0.3395    0.2691    0.9831    0.6981    0.1711    0.1904    0.8555    0.4820    0.5830    0.8244
        0.9516    0.4228    0.3015    0.6665    0.0326    0.3689    0.6448    0.1206    0.2518    0.9827
        0.0596   0.68197    0.0424    0.0714    0.5216    0.0967    0.8181    0.8175    0.7224    0.1498];

B = [0.7302
    0.3439
    0.5841
    0.1078
    0.9063
    .8];

x0 = 4*[ rand(10,1)];
x0 = rand(10,1);
eqInd   = [1:3]';
ineqInd = [4:6]';
W0 = [];
[x, q, lambda, Converged,W] = MinQP(x0, G, d, A, B,eqInd, ineqInd,W0, Options, 2);

Options.Display = 1;
load Prob1.mat G d A B eqInd ineqInd  

x0 = rand(100,1);
eqInd = [1:30]'; ineqInd = [31:75]';
Aeq   = A(eqInd,:); Beq = B(eqInd,:);
Aineq = A(ineqInd,:); Bineq = B(ineqInd,:);
W     =  [];
A     = A([eqInd;ineqInd],:);
B     = B([eqInd;ineqInd],:);
[x, q, lambda, Converged]       = MinQP(x0, G, d, A, B, eqInd,ineqInd, W, Options, 2);
[X,FVAL,EXITFLAG,OUTPUT,LAMBDA] = quadprog(G,d,-Aineq,-Bineq,-Aeq,-Beq,[],[],x0,[]);
disp([num2str(min(A*X - B),4) '  ' num2str(min(A*x - B),4) '  ' ...
      num2str(norm(x - X),4) '  ' num2str(q - FVAL,4) ]);

%==========================================================================
%==========================================================================
%==========================================================================

clear G d A B x0
load Prob2.mat G d A B x0 
%load c:\temp.mat x0

eqInd = [1:150]'; ineqInd = [151:300]';
Aeq   = A(eqInd,:); Beq = B(eqInd,:);
Aineq = A(ineqInd,:); Bineq = B(ineqInd,:);
W     =  [];
A     = A([eqInd;ineqInd],:);
B     = B([eqInd;ineqInd],:);
[x, q, lambda, Converged]       = MinQP(x0, G, d, A, B, eqInd,ineqInd, W, Options, 2);
[X,FVAL,EXITFLAG,OUTPUT,LAMBDA] = quadprog(G,d,-Aineq,-Bineq,-Aeq,-Beq,[],[],x0,[]);
disp([num2str(min(A*X - B),4) '  ' num2str(min(A*x - B),4) '  ' ...
      num2str(norm(x - X),4) '  ' num2str(q - FVAL,4) ]);

