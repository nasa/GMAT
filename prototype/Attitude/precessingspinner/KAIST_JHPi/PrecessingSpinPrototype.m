function [ R, Rdot, Omega_IB_B] = PrecessingSpinPrototype(time, BodySpinAxis, ...
    NutationReferenceVector, PrecessionRate, InitialPrecessionAngle, SpinRate, InitialSpinAngle, NutationAngle)
flag = 1 ;
% Default return values
R = eye(3);                 % Rotation matrix from Body frame to the Inertial frame
Rdot = zeros(3);            % Derivative of R
Omega_IB_B = zeros(3,1);    % Angluar veclocity of Body frame w.r.t. the Inertial frame expressed in Body frame
% set small value to check singularities
SmallValue = 1e-6;
%% Check Singularities
if (norm(BodySpinAxis) < SmallValue)
    disp('Error: The norm of BodySpinAxis is too small.')
    flag = 0 ;
elseif (norm(NutationReferenceVector) < SmallValue)
    disp('Error: The norm of NutationReferenceVector is too small.')
    flag = 0 ;
end
%% Unit conversion
D2R = pi/180;
InitialPrecessionAngle = InitialPrecessionAngle * D2R;
InitialSpinAngle = InitialSpinAngle * D2R;
NutationAngle = NutationAngle * D2R;
PrecessionRate = PrecessionRate * D2R;
SpinRate = SpinRate * D2R;
%% Normalize vector
BodySpinAxis = BodySpinAxis/norm(BodySpinAxis);
NutationReferenceVector = NutationReferenceVector/norm(NutationReferenceVector);
%% Calculate current PrecessionAngle and SpinAngle
PrecessionAngle = InitialPrecessionAngle + time*PrecessionRate;
SpinAngle = InitialSpinAngle + time*SpinRate;
%% main routine
while (flag)
    % Rotate to align BodySpinAxis to NutationReferenceVector
    if(norm(cross(BodySpinAxis, NutationReferenceVector)) < SmallValue)
        R_init = eye(3,3);
    else
        aVec = cross(BodySpinAxis, NutationReferenceVector);        
        aVec = aVec/norm(aVec);
        alpha = acos(dot(BodySpinAxis, NutationReferenceVector));
        R_init = ComputeEulerAxisRotationMatrix(aVec, alpha);
    end
    
    % Set Body_123 axis to perform 3-1-3 rotation
    BodyAxis1 = cross(BodySpinAxis,[1 0 0]');
    if(norm(BodyAxis1) < SmallValue)
        BodyAxis1 = cross(BodySpinAxis,[0 1 0]');
    end
    BodyAxis3 = BodySpinAxis;
    BodyAxis2 = cross(BodyAxis3, BodyAxis1);
    
    BodyAxis1 = BodyAxis1/norm(BodyAxis1);
    BodyAxis2 = BodyAxis2/norm(BodyAxis2);
    BodyAxis3 = BodyAxis3/norm(BodyAxis3);
    
    % Rotate with PrecessionAngle
    RotationVector_Precession = BodyAxis3;
    R_Precession = ComputeEulerAxisRotationMatrix(RotationVector_Precession, PrecessionAngle);
    
    % Rotate with NutationAngle
    RotationVector_Nutation = BodyAxis1;          
    R_Nutation = ComputeEulerAxisRotationMatrix(RotationVector_Nutation, NutationAngle);
   
    % Rotate with SpinAngle
    RotationVector_Spin = BodyAxis3;
    R_Spin = ComputeEulerAxisRotationMatrix(RotationVector_Spin, SpinAngle);
  
    R_BI = R_Spin*R_Nutation*R_Precession*R_init;      % Inertial to Body
    R = R_BI';                                         % Body to Inertial
   
    % Compute angular velocity in Body_123 frame
    omega_1 = PrecessionRate*sin(NutationAngle)*sin(SpinAngle);                                             
    omega_2 = PrecessionRate*sin(NutationAngle)*cos(SpinAngle);             
    omega_3 = PrecessionRate*cos(NutationAngle) + SpinRate;
    Omega_IB_B123 = [omega_1; omega_2; omega_3];
    
    % Rotation matrix from Body_123 frame to Body_xyz frame
    R_xyz_123 = [BodyAxis1 BodyAxis2 BodyAxis3];

    % Compute angular velocity of Body w.r.t. Inertial frame
    Omega_IB_B = R_xyz_123*Omega_IB_B123;           % expressed in Body
    Omega_IB_I = R*Omega_IB_B;                      % expressed in Inertial
    Omega_skew = [       0        -Omega_IB_I(3)   Omega_IB_I(2)
                   Omega_IB_I(3)       0          -Omega_IB_I(1)
                  -Omega_IB_I(2)   Omega_IB_I(1)         0      ];
              
    Rdot = Omega_skew*R';
    flag = 0 ;    
end