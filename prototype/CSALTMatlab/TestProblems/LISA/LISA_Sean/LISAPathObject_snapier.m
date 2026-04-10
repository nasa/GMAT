classdef LISAPathObject_snapier < userfunutils.UserPathFunction    

    %% Properties
    properties
        mu_earth = 3.986004415000000e+05;
        mu_sun   = 1.327124400179900e+11;
        scaleUtil
        
        %         % Parameters using GMAT API
                 ForceModel
        %         refFrameConverter
        %         earthFrame
        %         sunFrame
        %         Rvector_in
        %         Rvector_out
                 refEpoch
    end
    
    %% Methods
    methods
        
        %% Evaluate Path Constraints & Dynamics
        function EvaluateFunctions(obj)
            % Pass in Scaled State
            x           = obj.GetStateVec();
            u           = obj.GetControlVec();
            t           = obj.GetTime();             
            
            % Re-dimensionalize time
            tDim = obj.scaleUtil.UnScaleTime(t, 3, obj.refEpoch);
            
            % Re-dimensionalize state
            xDim_LISA1  = obj.scaleUtil.UnScaleState(x(1:6),    3);                
            xDim_LISA2  = obj.scaleUtil.UnScaleState(x(7:12),   3);
            xDim_LISA3  = obj.scaleUtil.UnScaleState(x(13:end), 3);
            
            % Get derivatives
            dx_2B_LISA1_old = obj.d_2body_central(t, xDim_LISA1, obj.mu_earth); % Control off
            dx_2B_LISA2_old = obj.d_2body_central(t, xDim_LISA2, obj.mu_earth); % Control off
            dx_2B_LISA3_old = obj.d_2body_central(t, xDim_LISA3, obj.mu_earth); % Control off
            %             dx_2B_LISA1 = dx_2B_LISA1_old;
%             dx_2B_LISA2 = dx_2B_LISA2_old;
%             dx_2B_LISA3 = dx_2B_LISA3_old;
            
            % Get derivatives using GMAT API
            %%{
            obj.ForceModel.GetDerivatives(xDim_LISA1, 86400*(tDim-obj.refEpoch));
            dx_2B_LISA1 = obj.ForceModel.GetDerivativeArray();
            
            obj.ForceModel.GetDerivatives(xDim_LISA2, 86400*(tDim-obj.refEpoch));
            dx_2B_LISA2 = obj.ForceModel.GetDerivativeArray();
            
            obj.ForceModel.GetDerivatives(xDim_LISA3, 86400*(tDim-obj.refEpoch));
            dx_2B_LISA3 = obj.ForceModel.GetDerivativeArray();
            %}
            
            %dx_LISA_2B  = [dx_2B_LISA1; dx_2B_LISA2; dx_2B_LISA3];

            % Scale Derivatives for output
            dx_2B_LISA1_nondim = [dx_2B_LISA1(1:3)...
                                  /obj.scaleUtil.earthScaling.VU;
                                  dx_2B_LISA1(4:6)...
                                  /obj.scaleUtil.earthScaling.ACCU];
            dx_2B_LISA2_nondim = [dx_2B_LISA2(1:3)...
                                  /obj.scaleUtil.earthScaling.VU;
                                  dx_2B_LISA2(4:6)...
                                  /obj.scaleUtil.earthScaling.ACCU];
            dx_2B_LISA3_nondim = [dx_2B_LISA3(1:3)...
                                  /obj.scaleUtil.earthScaling.VU;
                                  dx_2B_LISA3(4:6)...
                                  /obj.scaleUtil.earthScaling.ACCU];
       
            dx_LISA_2B_nondim = [dx_2B_LISA1_nondim;...
                                 dx_2B_LISA2_nondim;...
                                 dx_2B_LISA3_nondim];
            obj.SetDynFunctions(dx_LISA_2B_nondim);
            
            %% Path Constraints
            %[intang,   intang_lb,  intang_ub]   = obj.constraint_intang([xDim_LISA1; xDim_LISA2; xDim_LISA3]);
            %[sidelen, sidelen_lb, sidelen_ub]   = obj.constraint_sidelength(x);%[xDim_LISA1; xDim_LISA2; xDim_LISA3]); % Try nondimensionalized state to make scaling easier..
            %[eclipinc,eclipinc_lb, eclipinc_ub] = obj.constraint_eclipticInclination([xDim_LISA1; xDim_LISA2; xDim_LISA3], t);
            
            %obj.SetAlgFunctions([intang; sidelen]);% eclipinc]);
            %obj.SetAlgFuncLowerBounds([intang_lb; sidelen_lb]);% eclipinc_lb]);
            %obj.SetAlgFuncUpperBounds([intang_ub; sidelen_ub]); %eclipinc_ub]);
        end
        
        %% Two-body; assumes xx = 6-element posvel state
        function [dx] = d_2body_central(obj, tt, xx, mu)
            rvec  = xx(1:3);
            vvec  = xx(4:6); 
            rmag  = norm(xx(1:3));
            dx    = [vvec; -(mu/(rmag^3))*rvec];
        end
        
        %% Compute angle (in radians) between two vectors
        function theta = vecangle(obj, vec1,vec2)            
            %  function theta = vecangle(vec1,vec2)
            %
            %  This function calculates the angle between two vectors using the
            %  dot product.  The solution is always in quadrant one or two.
            %
            %  Variable I/O
            %
            %   Variable Name    I/0    Units     Dim.  Description
            %   vec1              I      UOI      3x1    Vector one
            %   vec2              I      UOI      3x1    Vector two
            %   theta             O      rad      1x1    angel between vector in radians
            %
            %  External References:  None
            %
            %  Modification History
            %  S. Hughes - 05/25/01 -  Created File
            arg = dot(vec1,vec2) / norm(vec1) / norm(vec2);
            theta = acos( arg );
        end
        
        %% Calculate Geometry of LISA Constellation
        function [ sv1,  sv2,  sv3,...
                  dsv1, dsv2, dsv3,...
                    s1,   s2,   s3,...
                   ds1,  ds2,  ds3,...
                   re1,  re2,  re3,...
                  ang1, ang2, ang3] = CalcSideData(obj, X1,X2,X3)     
              
              d2r  = pi/180;
              re1  = norm(X1(1:3));
              re2  = norm(X2(1:3));
              re3  = norm(X3(1:3));
              sv1  = X1(1:3) - X2(1:3);
              sv2  = X2(1:3) - X3(1:3);
              sv3  = X1(1:3) - X3(1:3);
              dsv1 = X1(4:6) - X2(4:6);
              dsv2 = X2(4:6) - X3(4:6);
              dsv3 = X1(4:6) - X3(4:6);
              s1   = norm(sv1(1:3));
              s2   = norm(sv2(1:3));
              s3   = norm(sv3(1:3));
              ds1  = dot(dsv1(1:3),sv1(1:3))/s1;
              ds2  = dot(dsv2(1:3),sv2(1:3))/s2;
              ds3  = dot(dsv3(1:3),sv3(1:3))/s3;
              ang1 = obj.vecangle(sv2(1:3),-sv1(1:3))/d2r;
              ang2 = obj.vecangle(sv2(1:3), sv3(1:3))/d2r;
              ang3 = obj.vecangle(sv1(1:3), sv3(1:3))/d2r;
            end
            
        %% Interior Angle Constraint
        function [            constraint,...
                  constraint_lowerbounds,...
                  constraint_upperbounds] = constraint_intang(obj, X)
            X1 = X(1:6);
            X2 = X(7:12);
            X3 = X(13:18);
            
            [sv1,  sv2,  sv3, dsv1, dsv2, dsv3,   s1,   s2,   s3,...
             ds1,  ds2,  ds3,  re1,  re2,  re3, ang1, ang2, ang3] = ...
                obj.CalcSideData(X1,X2,X3);

            constraint = [(ang1 - 60.0); (ang2 - 60.0); (ang3 - 60.0)];
            constraint;
            constraint_lowerbounds = [-0.99; -0.99; -0.99];%[-1.0; -1.0; -1.0];
            constraint_upperbounds = [ 0.99;  0.99;  0.99];%[ 1.0;  1.0;  1.0];
            constraint_jacobian    = [];
         
        end
        
        %% Side Length Constraint
        function [            constraint,...
                  constraint_lowerbounds,...
                  constraint_upperbounds] = constraint_sidelength(obj, X)
            X1 = X(1:6);
            X2 = X(7:12);
            X3 = X(13:18);
            
            [sv1,  sv2,  sv3, dsv1, dsv2, dsv3,   s1,   s2,   s3,...
             ds1,  ds2,  ds3,  re1,  re2,  re3, ang1, ang2, ang3] = ...
                obj.CalcSideData(X1,X2,X3);

            constraint = [s1-0.05; s2-0.05; s3-0.05];%[(s1 - 2.5e6); (s2 - 2.5e6); (s3 - 2.5e6)];
            constraint;
            constraint_lowerbounds = -[0.01; 0.01; 0.01];%[-5e5; -5e5; -5e5;];%[-1.0; -1.0; -1.0];
            constraint_upperbounds = [0.01; 0.01; 0.01];%[ 5e5;  5e5;  5e5;];%[ 1.0;  1.0;  1.0];
            constraint_jacobian    = [];
        end
        
        %% Set the State Scale Utility
        function SetScaleUtil(obj,scaleUtill)
            obj.scaleUtil = scaleUtill;
        end
        
        %% Evaluate Jacobians
        function EvaluateJacobians(~)
        end
    end
    
end


































