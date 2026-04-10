classdef BrysonMaxRangePathObj < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            
            x = stateVec(1);
            y = stateVec(2);
            v = stateVec(3);
            u1 = controlVec(1);
            u2 = controlVec(2);
           
            g = 1;
            a = 0.5*g;
            obj.SetDynFunctions([v*u1;
                v*u2;
                a*g-u2]);
            
            obj.SetAlgFunctions(u1^2 + u2^2);
            obj.SetAlgFuncUpperBounds(1);
            obj.SetAlgFuncLowerBounds(1);
            
        end
        
        function EvaluateJacobians(obj)
            
        end
        
    end
end


% obj.dynFuncStateJac = [
%     0 1 0 0;
%     0 0 0 0;
%     0 0 0 1;
%     0 0 0 0];
%
% obj.dynFuncControlJac = [0;
%     -a*sin(u);
%     0;
%     a*cos(u)];
%
% obj.dynFuncTimeJac = [0 0 0 0]';
%
