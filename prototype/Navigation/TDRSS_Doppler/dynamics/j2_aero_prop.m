
function [r,v,dt] = j2_aero_prop(r,v,dt,varargin)
% [r,v,dt] = j2_aero_prop(r,v,dt,'j2_on',1,'drag_on',1,'mass',50,'area',3,'cd',2.0);
% Propagator with optional j2/aero effects
% r: position vector in inertial coordinates (m)
% v: velocity vector in inertial coordinates (m/s)
% max time:integration time step (secs)
%
% To do: Dont use ODE45, code my own rk4
%        Verify Drag model 
%   CAUTION: This has been tested for gross errors but has not been fully
%   unit tested. Also, you should not back propagate with drag. The
%   solution seems unstable.
%   

% test r, v for size  
    [m1, n1] = size(r);
    [m2, n2] = size(v);

    if m1==3 && n1==1
        r = r';
        trans_r=1;
    end

    if m2==3 && n2==1
        v = v';
        trans_v=1;
    end

    if m1>3 || n1>3 || m1==2 || n1==2
        error('Malsized state vector!')
    end

    if m2>3 || n2>3 || m1==2 || n1==2
       error('Malsized state vector!');
    end
    

    opt = odeset('RelTol',1e-10,'AbsTol',1e-12);
    U = [r v ]; 

    if dt>=0
       [t,x] = ode45(@ode_j2_aero,[0 dt],U,varargin{:},opt);
    else
       [t,x] = ode45(@ode_j2_aero,[-dt 0],U,varargin{:},opt); %ode45 has trouble with spans 0 to negative. bit of a hack
    end
     
    r = x(end,1:3);
    v = x(end,4:6);

    % transpose back if needed
    if exist('trans_r','var')
        r = r';
    end
    
    if exist('trans_v','var')
        v=v';
    end
end