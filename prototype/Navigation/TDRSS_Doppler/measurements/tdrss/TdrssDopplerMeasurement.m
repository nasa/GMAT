
function [range,Fd,r_ephem,v_ephem]=TdrssDopplerMeasurement(epoch, d_count,user_service, use_tdrss_id,...
                                 r_user_j2k, v_user_j2k,source,user,mass,area,cd,fref,...
                                 total_time,varargin)
 % 5 way doppler measurement model (computed). Change in two way range for
 % the pilot tone is used to account for frequency translation one onboard
 % tdrss
 %
 % Inputs: epoch, start epoch UTC string
 %         d_count, doppler count(s)
 %         user_service: ksa-n(1 or 2) (ku single access), sma (sband multiple access)
 %                       ssa-(n) (1 or 2) (sband single access)
 %         r_user_j2k(m), initial j2k position vector associated with epoch
 %         v_user_j2k(m/s), initial j2k position vector associated with epoch
 %         Tdrss Ephemeris source: Currently tle's are only supported
 %         user, string name of the user satellite
 %         mass(kg), user satellite mass
 %         area(m2), user satellite area 
 %         cd, user satellite coefficient of drag
 %         fref(hz), user satellite carrier frequncy 
 %         total time(s), total time in which to take measurements
 % Outputs: range, range measurement (m)
 %          Fd (hz) doppler measurement
 %          r_ephem (m)   truth position vector ephem of user sat in j2k 
 %          v_ephem (m/s) truth position vector ephem of user sat in j2k 
 %

    if strcmp(varargin{1},'use_config_tle')
        use_config_tle=varargin{2};
    end        
    if ~exist('use_config_tle','var')
        use_config_tle=0;
    end
    
    
    % pull polar motion data, make available globally.
    global observed predicted taiminusutc ;

    fprintf('Reading earth orientation values lookup table\n')
    [observed,predicted] = earth_orientation;
    fprintf('Getting TAI-UTC\n')
    taiminusutc = get_tai_minus_utc;

    % handle user sat initial state vectors
    r_user_init = r_user_j2k;
    v_user_init = v_user_j2k;

    % handle different tdrss services
    service_types = {'ksa-1',...
                     'ksa-2',...
                     'sma',...
                     'ssa-1',...
                     'ssa-2'};
                 
    % initialize th while loop
    time=0 ;
    utc_time=epoch;
    i=1;
    fprintf('Running TDRSS 2-way, 5 leg doppler model\n')
    
    output_file= 'Whitesands_Doppler.txt';
    
    % open data file, write header
    fid = fopen(output_file,'w');
    fprintf(fid,'%% GMAT TDRSS Doppler Tracking File\n\n'); 
    % begin doppler calcs.
    while time < total_time
        % compute range to tdrss satellite in view and store
        measurement(i) = TdrssRange(utc_time,use_tdrss_id,...
                                    r_user_init,v_user_init,source,...
                                    mass, area,...
                                    cd,'use_config_tle',use_config_tle);
        % handle service type                       
        idx_service = find(  strcmp(user_service, service_types ) ==1,1,'first');
        if isempty(idx_service)
            error('No service found!')
        elseif strcmp(user_service,'ksa-1')
            frft = -1475.0E6;
        elseif strcmp(user_service,'ksa-2')
            frft = -1075.0E6;
        elseif strcmp(user_service,'sma')
            frft = -2279E6;
        % provision for tdrs-k (11)
        elseif strcmp(user_service,'sma') && measurement(i).tdid==11
            frft = -2287.5E6;
        elseif strcmp(user_service,'ssa-1')
            frft = 0.5*round(2*(13677.5-fref/1E6 ))*1E6;
        elseif strcmp(user_service,'ssa-2')
            frft = 0.5*round(2*(13697.5-fref/1E6 ))*1E6;
        end
        
        % increment time
        utc_time = datestr(datenum(utc_time)+ d_count/86400);
        
        [y,mm,d,h,m,s] = datevec(utc_time);

        mjd(i) = mjuliandate(y,mm,d,h,m,s);

        % save user ephemeris.
        r_ephem(:,i) = r_user_init;
        v_ephem(:,i) = v_user_init;

        % propagate the user satellite
        [r_user,v_user,dt] = j2_aero_prop(r_user_init,v_user_init,d_count,'j2_on',1,'drag_on',1,'mass',mass,'area',area,'cd',cd);
        r_user_init = r_user;
        v_user_init = v_user;

        % write to file
        if i>1
            delta_t = (mjd(i)-mjd(i-1))*86400;
            % compute two way range for pilot
            delta_range_s = (measurement(i).r4 + measurement(i).r5) - (measurement(i-1).r4 + measurement(i-1).r5);
            % compute doppler
            Fd(i) = (-1/speed_of_light('m'))*( fref*((measurement(i).range-measurement(i-1).range)/delta_t) + frft*(delta_range_s/delta_t));
            fprintf(fid,'%f %s %s %s %s %f %f\n',mjd(i),'doppler', strcat('TDRS-',num2str(measurement(i).tdid)),user,'STGT',measurement(i).one_way_range/1000,Fd(i) );
        else
            fprintf(fid,'%f %s %s %s %s %f %f\n',mjd(i),'doppler', strcat('TDRS-',num2str(measurement(i).tdid)),user,'STGT',measurement(i).one_way_range/1000,0 );
        end

        % if more then 1 measurement has been taken, compute doppler
        i=i+1;
        time = time + d_count;
    end
    % assign range structure
    range = measurement;
    fprintf('\n')
    fprintf('Doppler output file: %s\n\n',fullfile(pwd, output_file))
    fclose(fid);

end