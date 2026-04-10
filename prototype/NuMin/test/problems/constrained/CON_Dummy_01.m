function [ci,ce,Ji,Je,iGfun,jGvar] = CON_Dummy01(x)     
 
%  For unit testing OO User function interface.  This is NOT  a real
%  optimization problem
%   Copyright 2002-2005, United States Government as represented by the 
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

Ae =    [ 0.964888535199277         0.485375648722841         0.915735525189067        0.0357116785741896
         0.157613081677548           0.8002804688888         0.792207329559554         0.849129305868777
         0.970592781760616         0.141886338627215         0.959492426392903         0.933993247757551
         0.957166948242946         0.421761282626275         0.655740699156587         0.678735154857773];
     
Aineq =    2*[ 0.964888535199277         0.485375648722841         0.915735525189067        0.0357116785741896
         0.157613081677548           0.8002804688888         0.792207329559554         0.849129305868777
         0.970592781760616         0.141886338627215         0.959492426392903         0.933993247757551
         0.957166948242946         0.421761282626275         0.655740699156587         0.678735154857773];
ce =Ae*x;
Je =Ae;

iGfun  = ones(20,1);
jGvar  = (1:20)';

ci = Aineq*x;
Ji = Aineq;
