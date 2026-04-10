classdef HypSenPathFunction < userfunutils.UserPathFunction
    %UNTITLED3 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function [funcData] = ...
                Evaluate(~,paramData,funcData,isPerturbing)
            
            % Extract paramater data
            y = paramData.stateVec;
            u = paramData.controlVec;
            t = paramData.time;
            timeFac = 0.0;
            
            % Evaluate cost function and compute Jacobians
            funcData.costData.SetFunctions(0.5*(y^2 + u^2) + timeFac*t/100);
            if ~isPerturbing
                %funcData.costData.SetStateJac(y);
                %funcData.costData.SetControlJac(u);
                %funcData.costData.SetTimeJac(timeFac/100);
            end
            
            % Evaluate dynamics and compute Jacobians
            funcData.dynData.SetFunctions(-y^3 + u + timeFac*t^3/100);
            if ~isPerturbing
                %funcData.dynData.SetStateJac(-3*y^2);
                %funcData.dynData.SetControlJac(1);
                %funcData.dynData.SetTimeJac(timeFac*3*t^2/100);
            end
            
        end
    end
    
end