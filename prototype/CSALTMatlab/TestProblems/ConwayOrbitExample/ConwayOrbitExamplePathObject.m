classdef ConwayOrbitExamplePathObject < userfunutils.UserPathFunction
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        
    end
    
    methods
        
        function EvaluateFunctions(obj)
            
            %  Extract state and control components
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            r = stateVec(1);
            vr = stateVec(3);
            vt = stateVec(4);
            A = 0.01;
            B = controlVec(1);
            
            %  Set the dynamics function values
            obj.SetDynFunctions([vr;
                vt/r;
                vt^2/r - 1/r^2 + A*sin(B);
                -vr*vt/r + A*cos(B)]);
            
        end
        
        function EvaluateJacobians(obj)
            
            %  Extract state and control components
            stateVec = obj.GetStateVec();
            controlVec = obj.GetControlVec();
            r = stateVec(1);
            vr = stateVec(3);
            vt = stateVec(4);
            A = 0.01;
            B = controlVec(1);
            
            %  Set dynamics function Jacobians
            obj.SetDynStateJac([0 0 1 0;
                -vt/r^2 0 0 1/r;
                (2/r^3 - vt^2/r^2) 0 0 2*vt/r;
                vr*vt/r^2 0 -vt/r -vr/r ]);
            %
            obj.SetDynControlJac( [0;
                0;
                A*cos(B);
                -A*sin(B)]);
            
            obj.SetDynTimeJac(zeros(4,1));
        end
    end
    
end



