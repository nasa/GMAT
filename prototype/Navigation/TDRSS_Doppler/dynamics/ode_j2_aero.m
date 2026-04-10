function X = ode_j2_aero(t,U,varargin)
%
% Acceleration Terms with central body, J2, and Drag.
%
% Inputs: U: Initial condition vector [R V ]
%            R: position vector,m
%            V: velocity vector, m/s
%         Varargin:
%         j2_on Boolean, defaults to on
%         drag_on Boolean, defaults to off 
%         mass, kg
%         cross-sectional area, m
%         cd
%
%         if using drag, note this is a simple drag model.
%         you need to input 'cd', and 'area' in meters^2
%
% example call:

X= zeros(1,6);
r = U(1:3);
v = U(4:6);
% process inputs
if nargin>12
    error('To many inputs!')
end

if isempty(varargin)
     j2_on=1;
     drag_on=0;
else
    for i =1:length(varargin)
        if strcmp(varargin{i},'j2_on')
            if isnumeric(varargin{i+1})
                j2_on = varargin{i+1};
            else
                error('Improper input spec')
            end
        end
        
        if strcmp(varargin{i},'drag_on')
            if isnumeric(varargin{i+1})
                drag_on = varargin{i+1};
            else
                error('Improper input spec')
            end
        end
        if strcmp(varargin{i},'cd')
            if isnumeric(varargin{i+1})
                cd = varargin{i+1};
            else
                error('Improper input spec')
            end
        end
        if strcmp(varargin{i},'area')
            if isnumeric(varargin{i+1})
                area = varargin{i+1};
            else
                error('Improper input spec')
            end
        end
        if strcmp(varargin{i},'mass')
            if isnumeric(varargin{i+1})
                mass = varargin{i+1};
            else
                error('Improper input spec')
            end
        end
           
        
    end

end

if ~exist('j2_on','var')
    j2_on=1;
end

if ~exist('drag_on','var')
    drag_on=0;
end

if drag_on
    % check for all variables
    if ~exist('mass','var') || ~exist('area','var') || ~exist('cd','var')
        error('not enough input parameters to compute drag')
    end

     SH = 88667.0; %m scale height
     ro = (700000.0+RE('m')); % reference altitude
     rhoa = 3.614e-13; %kg/m^3
end

% compute accelerations

%needed terms for drag
Rinitial= norm(r);

% acceleration terms
if drag_on && j2_on
    % Calculate the s/c velocity wrt the atmosphere
    VectorVa= [ v(1) + r(2)*omega_earth; v(2)-r(1)*omega_earth; v(3) ];
    Va = norm(VectorVa);
    rhoo= rhoa*exp((-Rinitial+ro)/SH);
    accel_x =(-MU('m')*((1/(Rinitial^3))+j2_on*((((3/2)*J2*(RE('m')^2))/(Rinitial^5))*(1-(5*(r(3)^2)/(Rinitial^2)))))*r(1))-drag_on*(((1/2)*rhoo*cd*area*Va*VectorVa(1)/mass)*exp(-(Rinitial-ro)/SH));
    accel_y =(-MU('m')*((1/(Rinitial^3))+j2_on*((((3/2)*J2*(RE('m')^2))/(Rinitial^5))*(1-(5*(r(3)^2)/(Rinitial^2)))))*r(2))-drag_on*(((1/2)*rhoo*cd*area*Va*VectorVa(2)/mass)*exp(-(Rinitial-ro)/SH));
    accel_z =(-MU('m')*((1/(Rinitial^3))+j2_on*((((3/2)*J2*(RE('m')^2))/(Rinitial^5))*(3-(5*(r(3)^2)/(Rinitial^2)))))*r(3)) -drag_on*(((1/2)*rhoo*cd*area*Va*VectorVa(3)/mass)*exp(-(Rinitial-ro)/SH));
elseif ~drag_on && j2_on
    accel_x =(-MU('m')*((1/(Rinitial^3))+j2_on*((((3/2)*J2*(RE('m')^2))/(Rinitial^5))*(1-(5*(r(3)^2)/(Rinitial^2)))))*r(1));
    accel_y =(-MU('m')*((1/(Rinitial^3))+j2_on*((((3/2)*J2*(RE('m')^2))/(Rinitial^5))*(1-(5*(r(3)^2)/(Rinitial^2)))))*r(2));
    accel_z =(-MU('m')*((1/(Rinitial^3))+j2_on*((((3/2)*J2*(RE('m')^2))/(Rinitial^5))*(3-(5*(r(3)^2)/(Rinitial^2)))))*r(3));
elseif ~drag_on && ~j2_on
     accel_x =-MU('m')*(1/(Rinitial^3))*r(1);
    accel_y =-MU('m')*(1/(Rinitial^3))*r(2);
    accel_z =-MU('m')*(1/(Rinitial^3))*r(3);
    
else
    error('Something bad went wrong, shouldnever get here!')
end
    

accel = [accel_x, accel_y, accel_z ];

X(1) = v(1);
X(2) = v(2);
X(3) = v(3);
X(4) = accel(1);
X(5) = accel(2);
X(6) = accel(3);

X = X';

end
