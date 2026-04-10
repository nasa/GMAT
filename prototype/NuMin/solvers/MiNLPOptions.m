classdef MiNLPOptions < handle
    %UNTITLED5 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        % BFGS Algorith
        hessUpdateMethod  = 'SelfScaledBFGS';  % 'DampedBFGS', 'SelfScaledBFGS'
        meritFunction     = 'NocWright';       % 'NocWright','Powell'
        FiniteDiffVector  = ones(5,1)*1e-9;
        DerivativeMethod  = 'Analytic';
        MaxIter           = 1350;
        MaxFunEvals       = 1750;
        TolCon            = 1e-8;
        TolX              = 1e-8;
        TolF              = 1e-8;
        TolGrad           = 1e-8;
        TolStep           = 1e-8;
        MaxStepLength     = 1000;
        QPMethod          = 'minQP';
        Display           = 'iter';
        MaxElasticWeight  = 10000;
    end
    
    methods
        
        function obj = MiNLPOptions()
            
        end
    end
    
end

