
function measurement = TdrssRange(epoch_str,use_tdrss_id,...
                                             r_user_m_j2k,v_user_mps_j2k,source,...
                                             user_mass_kg, user_area_m,...
                                             user_cd,varargin)
%        measurement = TdrssRange('2014-01-25 05:30',[5,6,7],...
%                                              r_user,v_user,...
%                                              mass, area,...
%                                              cd,'use_config_tle',1);   
%        5 way range tdrss model (ground-tdrs-user-ground-tdrs)
%        leg 5 is the range/de;au for the pilot tone
%
%        Input:      r_user_m_j2k: User position vector in j2k, m
%                    v_user_mps_j2k: User velocity vector in j2k, m/s
%                    Source: tle or j2k (currently only tle's are
%                    supported)
%                    user_mass_kg: User satellite mass in kg
%                    user_area_m: User satellite area in m
%                    user_cd: User satellite coefficient of drag
%                    varargin: use_config_tle = 1 uses a configured tle file
%
%        Output:     r<n> range of leg (m)
%                    t<n> leg light time delay (s)
%                    x<n> Vector of common participant to subsequent leg.
%                    range: two way range (m)
%                    one_way_range: one way range (m) (r1+r2+r3+r4)/2

    if strcmp(varargin{1},'use_config_tle')
        use_config_tle=varargin{2};
    end        
    if ~exist('use_config_tle','var')
        use_config_tle=0;
    end
    % convert epoch to serial date
    epoch=datenum(epoch_str);
    measurement.utc_date_num=epoch;
    
    %==================== ground station configs==========================%
    % sources: 
    % http://en.wikipedia.org/wiki/Tracking_and_Data_Relay_Satellite_System#Ground_segment
    % http://gu.geoview.info/usaf_gts_guam_satellite_tracking_station,7871230
    % white sands (White Sands Ground Terminal(WSGT) )
    % TODO: Get actual geodetics. This should be ok for now 
    % m, taking las cruces altitude for now
    whitesands.wsgt.lat  = 32.5007;
    whitesands.wsgt.long = -106.6086;
    whitesands.wsgt.alt  = 1191; 
    % white sands (Second TDRS Ground Terminal (STGT) )
    whitesands.stgt.lat  =  32.5230;
    whitesands.stgt.long = -106.6120;
    % m, taking las cruces altitude for now
    whitesands.stgt.alt  =  1191; 
    % guam (Guam Remote Ground Terminal (GRGT)
    guam.lat  = 13.6148;
    guam.long = 144.8565;
    guam.alt  = 150; % m
    whitesands.wsgt.x_gt_ecef = lla2ecef(whitesands.wsgt.lat,whitesands.wsgt.long,0);
    % convert from earth fixed(itrf) to the eci mean equator mean equinox (j2000).
    [whitesands.wsgt.r_j2000,whitesands.wsgt.v_j000, whitesands.wsgt.a_j2000] = ECEF_to_J2000(whitesands.wsgt.x_gt_ecef,...
                                                               zeros(3,1),zeros(3,1),2,epoch);
    whitesands.wsgt.r_j2000=whitesands.wsgt.r_j2000*1000;

    % figure out which sat is in view. To do that, call the Tdrss Truth
    % model to figure out what sat to use
    truth = TdrssTruth('epoch',epoch_str,'step',1,'to_plot',0,'maxtime',0,'use_tdrss_id',use_tdrss_id,'use_config_tle',use_config_tle);
    % algorithm to pick what tdrss participant i should use
    for i=1:length(use_tdrss_id)
        % compute if the satellite is in view
        [view(i), eta(i)] = view_between_sats(truth(i).r_j2000, r_user_m_j2k);     
    end
  
    % find smallest nadir angle of sat in view. This is a hack for now until
    % i compute LVLH elevation angle
    in_view = find(view==1 );
    [val,i]=min(eta(in_view));

    % initialize the measurement structure
    mstruct=struct('r4',[],'t4',[],'x4k',[],...
                   'r3',[],'t3',[],'x3k',[],...
                   'r2',[],'t2',[],'x2k',[],...
                   'r1',[],'t1',[],'x1k',[],...
                   'r5',[],'t5',[],'x5k',[] );

    fprintf('----------------------------------------\n')
    fprintf('Solving for Sat %i\n',use_tdrss_id(i))
    fprintf('----------------------------------------\n')
    fprintf('Computing  the range of leg 4\n')
    % Solve the 5 leg doppler model, Phung Henry Special algorithm
    % TDRSS to Groundstation leg
    r_init= .133*speed_of_light('m');
    r4_k = r_init;
    r4_k_1=0;
    r4_diff = abs(r4_k-r4_k_1);
    t_4_k=0;
    truth=0;
    r4_diff_prev=10000;
    while r4_diff >= 0.1
        t_4_k = epoch - (r4_k/speed_of_light('m'))/86400;
        truth = TdrssTruth('epoch',datestr(t_4_k,'yyyy-mm-dd HH:MM:SS.FFF'),'step',1,'to_plot',0,'maxtime',0,'use_tdrss_id',use_tdrss_id(i),'use_config_tle',use_config_tle);
        truth.r_j2000=truth.r_j2000*1000;
        r4_k_1 = norm(whitesands.wsgt.r_j2000 - truth.r_j2000);
        r4_diff = abs(r4_k_1 - r4_k);
        r4_k=r4_k_1;
        x_4k=truth.r_j2000;
        % check for unchanging differences
        if ~abs(r4_diff-r4_diff_prev)>0
            fprintf('WARNING: Leg not converging below threshold\n')
            break
        end
        r4_diff_prev = r4_diff;
    end
    % save off results
    measurement.r4   = r4_k;
    measurement.t4   = (epoch-t_4_k)*86400;
    measurement.tdid = use_tdrss_id(i);
    measurement.x4_k = truth.r_j2000;

    % User to TDRSS Leg
    % Uses j2 plug aero propagation
    fprintf('----------------------------------------\n')
    fprintf('Computing  the range of leg 3\n')
    r3_k = r_init;
    r3_k_1=0;
    r3_diff = abs(r3_k-r3_k_1);
    t_3_k=0;
    r_user_init = r_user_m_j2k;
    v_user_init = v_user_mps_j2k;
    r3_diff_prev=10000;    
    while r3_diff >= 0.1
        % for now, assume state vector of satellite is known at time TL?
        % propagate user sat.
        t_3_k = measurement.t4 + r3_k / speed_of_light('m');
        [r_user_3k,v_user_3k,dt] = j2_aero_prop(r_user_init,v_user_init,-t_3_k,'j2_on',1,'drag_on',1,'mass',user_mass_kg,'area',user_area_m,'cd',user_cd);
        r3_k_1 = norm(measurement.x4_k -r_user_3k );
        r3_diff = abs(r3_k_1 - r3_k);
        r3_k=r3_k_1;  
        % check for unchanging differences
        if ~abs(r3_diff-r3_diff_prev)>0
            fprintf('WARNING: Leg not converging below threshold\n')
            break
        end
        r3_diff_prev = r3_diff;
    end
    % save off results
    measurement.r3   = r3_k;
    measurement.t3   = t_3_k;
    measurement.tdid = use_tdrss_id(i);
    measurement.x3_k = r_user_3k;

    % TDRSS to User Leg
    fprintf('----------------------------------------\n')
    fprintf('Computing  the range of leg 2\n')  
    r2_k = r_init;
    r2_k_1=0;
    r2_diff = abs(r2_k-r2_k_1);
    t_2_k=0;
    truth=[];
    r2_diff_prev=10000; 
    while r2_diff >= 0.1
        % for now, assume state vector of satellite is known at time TL?
        % propagate user sat.
        t_2_k = epoch - (measurement.t4 + measurement.t3 + r2_k / speed_of_light('m'))/86400;
        truth = TdrssTruth('epoch',datestr(t_2_k,'yyyy-mm-dd HH:MM:SS.FFF'),'step',1,'to_plot',0,'maxtime',0,'use_tdrss_id',use_tdrss_id(i),'use_config_tle',use_config_tle);
        truth.r_j2000=truth.r_j2000*1000;
        r2_k_1 = norm(measurement.x3_k -truth.r_j2000 );
        r2_diff = abs(r2_k_1 - r2_k);
        r2_k=r2_k_1;  
        % check for unchanging differences
        if ~abs(r2_diff-r2_diff_prev)>0
            fprintf('WARNING: Leg not converging below threshold\n')
            break
        end
        r2_diff_prev = r2_diff;
    end
    measurement.r2   = r2_k;
    measurement.t2   = (epoch-t_2_k)*86400;
    measurement.tdid = use_tdrss_id(i);
    measurement.x2_k = truth.r_j2000;
    
    % Ground station to TDRSS Leg
    fprintf('----------------------------------------\n')
    fprintf('Computing  the range of leg 1\n')
    r1_k = r_init;
    r1_k_1=0;
    r1_diff = abs(r1_k-r1_k_1);
    t_1_k=0;
    r1_diff_prev=10000;
    while r1_diff >= 0.1
        % propagate user sat.
        t_1_k = epoch-(measurement.t4 + measurement.t3 +measurement.t2 + r1_k / speed_of_light('m'))/86400;
        [whitesands.wsgt.r_j2000,whitesands.wsgt.v_j000, whitesands.wsgt.a_j2000] = ECEF_to_J2000(whitesands.wsgt.x_gt_ecef,...
                                                           zeros(3,1),zeros(3,1),2,t_1_k);
        whitesands.wsgt.r_j2000 = whitesands.wsgt.r_j2000*1000;
        r1_k_1 = norm(measurement.x2_k - whitesands.wsgt.r_j2000 );
        r1_diff = abs(r1_k_1 - r1_k);
        r1_k=r1_k_1;  
        %check for unchanging differencesuse_tdrss_id
        if ~abs(r1_diff-r1_diff_prev)>0
            fprintf('WARNING: Leg not converging below threshold\n')
            break
        end
        r1_diff_prev = r1_diff;

    end
    % save off results
    measurement.r1   = r1_k;
    measurement.t1   = (epoch-t_1_k)*86400;
    measurement.tdid = use_tdrss_id(i);
    measurement.x1_k = whitesands.wsgt.r_j2000;
    
    % solve for the pilot leg to account for frequency translation done
    % aboard TDRSS for return t downlink transition
    fprintf('----------------------------------------\n')
    fprintf('Computing  the range of leg 5\n')
    r5_k = r_init;
    r5_k_1=0;
    r5_diff = abs(r5_k-r5_k_1);
    t_5_k=0;
    truth=[];
    r5_diff_prev=10000;
    while r5_diff >= 0.1
        % for now, assume state vector of satellite is known at time TL?
        % propagate user sat.
        t_5_k = epoch-(measurement.t4  + r5_k / speed_of_light('m'))/86400;
        [whitesands.wsgt.r_j2000,whitesands.wsgt.v_j000, whitesands.wsgt.a_j2000] = ECEF_to_J2000(whitesands.wsgt.x_gt_ecef,...
                                                           zeros(3,1),zeros(3,1),2,t_5_k);
        whitesands.wsgt.r_j2000 = whitesands.wsgt.r_j2000*1000;
        r5_k_1 = norm(measurement.x4_k - whitesands.wsgt.r_j2000 );
        r5_diff = abs(r5_k_1 - r5_k);
        r5_k=r5_k_1;  
        %check for unchanging differences
        if ~abs(r5_diff-r5_diff_prev)>0
            fprintf('WARNING: Leg not converging below threshold\n')
            break
        end
        r5_diff_prev = r5_diff;

    end
    measurement.r5   = r5_k;
    measurement.t5   = (epoch-t_5_k)*86400;
    measurement.tdid = use_tdrss_id(i);
    measurement.x5_k = whitesands.wsgt.r_j2000;
    measurement.range = (measurement.r1 + measurement.r2 + measurement.r3 + measurement.r4);
    measurement.one_way_range = (measurement.r1 + measurement.r2 + measurement.r3 + measurement.r4)/2;
 
end
