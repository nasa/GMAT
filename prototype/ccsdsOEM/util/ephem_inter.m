function ephem2 = ephem_inter(ephem,T_V,order)

%  function ephem2 = ephem_inter(ephem,T_V,order)
%
%  This function interpolates an ephemris file and outputs
%  the new ephemeris at the time given in the column vector T_V
%
%  Variable I/O
%  UOI -> units of input
%  Variable Name    I/0    Units       Dim.       Description
%  ephem             I      UOI        mx7        [t | x | y | z | vx| vy | vz]
%  T_V               I      UOI        nx1        cartesian state of companion
%  order             I      dimless    1x1        order of Lagrange Interpolating Polynomial
%  ephem2            O      UOI        nx7        new ephemeris file
%
%  Modification History
%  11/20/00 - Created, S. Hughes

% set order if not provided
if nargin < 3
   order = 7;
end

%  Ensure time vector is in column format
n = size(T_V,1);
if (n == 1)
    T_V = T_V';
    n = size(T_V,1);
end

%  Initializations
count = 1;
i = 1;
m = size(ephem,1);

if last(T_V) > last(ephem(:,1));
    disp('Warning:  Requested  final time outside range  of ephemeris file')
    ephem2 = [];
    return
end

if first(T_V) < last(ephem(1,1));
    disp('Warning:  Requested  initial time outside range  of ephemeris file')
    ephem2 = [];
    return
end


while ( i <= n )
    T = T_V(i);
    
    %  Determine which rows in the ephemeris file to use in the interpolation
    %check = 1;
    
    if T == ephem(count,1) | T == ephem(count+1,1)
        count = count;
    else
        
        if T > ephem(count,1)
            while T > ephem(count,1)
                count = count + 1;
            end
        elseif T < ephem(count,1)
            while T < ephem(count,1) & count > 0
                count = count - 1;
            end
            
        end
    end
    
    
    %  Center the time, for interpolation
    count = fix(count - order/2);
    
    % don't go past beginning of file !!!!
    if (count <= 0)
        count = 1;
    end
    
    % don't go past end of file !!!!
    if (count > (m-order))
        count = m-order;
    end
    
    %  Interpolate the positions and velocities
    T_v = ephem(count:count+order,1);
    x_i = lag_inter( T_v, ephem(count:count+order,2),T );
    y_i = lag_inter( T_v, ephem(count:count+order,3),T );
    z_i = lag_inter( T_v, ephem(count:count+order,4),T );
    vx_i = lag_inter( T_v, ephem(count:count+order,5),T );
    vy_i = lag_inter( T_v, ephem(count:count+order,6),T );
    vz_i = lag_inter( T_v, ephem(count:count+order,7),T );
    
    %  Construct the ephemeris and increment counter
    ephem2(i,1) = T;
    ephem2(i,2:7) = [x_i y_i z_i vx_i vy_i vz_i];
    i = i+1;
    
end

function firstData = first(data)
   firstData = data(1,1);

function lastData = last(data)
   lastData = data(size(data,1),1);     




