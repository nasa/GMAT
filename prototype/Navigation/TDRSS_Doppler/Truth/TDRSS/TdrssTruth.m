

function truth = TdrssTruth(varargin)
% truth = TdrssTruth('epoch','01/23/2014 4:55:00','to_plot',1,'use_tdrss_id',[5,6,7],'maxtime',0,'step',1,'use_config_tle',1)
% 
% Tdrss Truth Model
%
% Creates an ephemeris for TDRSS based on NORAD TLE's
%
% Varargin
%   epoch: UTC time epoch from which to start the ephemeris (default is
%   latest epoch on celestrak)
%   maxtime: Length of ephemeris in minutes  (default is 1440) 
%   step: Time step of ephemeris in minutes (default is 1)
%   plot: Plot the ground traces for the ephemeris (deault is 0)
%   use_sats: Array of tdrss sats to use (default to all available on
%   celestrak)
%   use_config_tle = 'use_config_tle' uses a configured tle file

% attempt at decreasing run time. the observed and predicted variables
% contains polar motion data that need only be read and processed once

% define global an persistent variables to optimize calls
global observed predicted 
persistent tdrss taiminusutc

% process inputs
for i=1:length(varargin)
    if strcmp(varargin{i},'epoch')
        epoch=datenum(varargin{i+1});
    elseif strcmp(varargin{i},'maxtime')
        maxtime=varargin{i+1};
    elseif strcmp(varargin{i},'step')
        step=varargin{i+1};
    elseif strcmp(varargin{i},'to_plot')
        to_plot=varargin{i+1};
    elseif strcmp(varargin{i},'use_tdrss_id')
        use_tdrss_id=varargin{i+1};
    elseif strcmp(varargin{i},'use_config_tle')
        use_config_tle=varargin{i+1};
    end        
end

% set defaults if variables not set
if ~exist('maxtime','var')
    maxtime=1440;
end

if ~exist('step','var')
    step=1;
end

if ~exist('to_plot','var')
    to_plot=0;
end

if ~exist('use_config_tle','var')
    use_config_tle=0;
end


% call the reader if tdrss isnt already in the name space
if isempty(tdrss)
    tdrss = tdrss_tle_reader('use_config_tle',use_config_tle);
    fprintf( 'Reading TDRSS tle\n')
end

% get each satellites True Equator Mean Equinox ECI state vectors
for i=1:length(tdrss.id)
    tdrss.a(i) = n_to_sma(tdrss.n(i)*2*pi/86400);
    tdrss.E(i) = KepEqtnE(tdrss.M(i),tdrss.ecc(i));
    tdrss.nu(i)= E2nu(tdrss.E(i),tdrss.ecc(i));
    [tdrss.r_teme(:,i),tdrss.v_teme(:,i)] = orbital2eci(tdrss.a(i),...
        tdrss.ecc(i),tdrss.inc(i),tdrss.raan(i),tdrss.argp(i),tdrss.nu(i)); 
end

% Get earth orientation parameters. 
% this is needed to convert the TLEs to mean equator mean equinox (j2000).
if isempty(observed) || isempty(predicted) 
    [observed,predicted] = earth_orientation;
end

% get sats to process
if ~exist('use_tdrss_id','var')
    num_sats = length(tdrss.id);
    sat_id = str2double(tdrss.id);
    for i=1:num_sats
    % array of sats to use
        idx(i) = i;
    end
else
    sat_id_init = use_tdrss_id;
    j=1;
    for i=1:length(sat_id_init)
        if isempty(find(str2double(tdrss.id)==sat_id_init(i),1))
            error('TDRS Sat not found!')
        else
            idx(j) = find(str2double(tdrss.id)==sat_id_init(i));
            sat_id(j)= sat_id_init(i);
            j=j+1;
        end
    end
    % initialize length of valid sats
    num_sats = length(idx);
end

% condition initial states
for i=1:num_sats
    % compute acceleration vector in TEME coordinates
    init.r_teme(:,i) =tdrss.r_teme(:,idx(i));
    init.v_teme(:,i) =tdrss.v_teme(:,idx(i));
    init.a_teme(:,i) = -MU*init.r_teme(:,i)/norm(init.r_teme(:,i))^3;
    init.epoch(i) = tdrss.epoch(idx(i));
    init.epoch_str(i) = tdrss.epoch_str(idx(i));
    % condition spg4 inputs.
    init.n(i) = tdrss.n(idx(i))*(2.0*pi)/1440.0;
    init.inc(i)= d2r(tdrss.inc(idx(i)));
    init.raan(i)= d2r(tdrss.raan(idx(i)));
    init.argp(i)=d2r(tdrss.argp(idx(i)));
    init.M(i)=d2r(tdrss.M(idx(i)));
    init.bstar(i)= tdrss.bstar(idx(i)); 
    init.ecc(i)=tdrss.ecc(idx(i));
    init.juliandate(i) = tdrss.juliandate(idx(i));
    init.sat_num1{i} = tdrss.sat_num1{idx(i)};
    % compute time and polar motion values to convert to ECEF
    if isempty(taiminusutc)
        taiminusutc = get_tai_minus_utc;
    end
    % find tai
    tai = tdrss.epoch(idx(i)) +taiminusutc/86400;
    % find dynamic time tt
    tt = tai+32.184/86400;
    [Y,MO,D,H,MI,S] = datevec(tt);
    JDtt = jdayall(Y,MO,D,H,MI,S,'g');
    % find julian centeries
    Ttt = (JDtt -2451545.0)/36525;
    % find jdut1
    [year,month,day, hour, minute, sec]=datevec(init.epoch(i));
    % date of tdrss satellite TLE
    date_num  = datenum([year,month,day]);
    % find earth orientation data at the epoch. Use observed if available, if not, use
    % predicted
    idx_epoch = find(observed.utc_num==date_num);
    if isempty(idx_epoch)
        idx_epoch = find(predicted.utc_num==date_num);
        data=predicted;
    else
        data=observed;
    end
    
    % if empty still, throw error
    if isempty(idx_epoch)
        error('Astronomical data not available for this epoch!')
    end
    
    % pull out needed data for conversion
    % polar motion in radians
    x=d2r(data.x(idx_epoch)/60);
    y=d2r(data.y(idx_epoch)/60);
    
    UT1minusUTC=data.UT1minusUTC(idx_epoch);
    % observed length of day
    lod=data.lod(idx_epoch);
    
    % go teme to ecef
    ut1 = init.epoch(i)+UT1minusUTC/86400;
    [Y,MO,D,H,MI,S] = datevec(ut1);
    JDut1 = jdayall(Y,MO,D,H,MI,S,'g');
    
    [init.r_ecef(:,i),init.v_ecef(:,i),init.a_ecef(:,i)] = teme2ecef(init.r_teme(:,i),...
        init.v_teme(:,i),init.a_teme(:,i),Ttt,JDut1,lod,x,y );
    
end
    % convert to ecef
    [init.lat, init.long, init.alt] = ecef2lla(init.r_ecef );

    % Define SPG4 epoch.
    [spg4_year,spg4_month,spg4_day,spg4_hr,spg4_min,spg4_sec] = datevec('01/00/1950 00:00');
    spg4_jdepoch = jdayall(spg4_year,spg4_month,spg4_day,spg4_hr,spg4_min,spg4_sec,'g');

    % set up data array of structs
     orb=struct('julian_date',[],'r_teme',[],'v_teme',[],'a_teme',[],'r_ecef',[],'v_ecef',[],'lat',[],'long',[],'alt',[]);
     truth= repmat(orb, 1, num_sats);
     
    % set up anchor epoch if none was specified. This uses the latest epoch
    % by convention
    if ~exist('epoch','var')
        [ val idx_latest] = max(init.epoch);
        epoch = val;
    end

% this processes all satellites, snaps all the satellites to the anchor
% epoch and propagates forward. 
for i=1:num_sats
    truth(i).tdrs_id=sat_id(i);
    clear satrec;
    %Propagate all satellites to the epoch
    satrec=struct();
    % assign id of tdrs
    satrec.satnum = init.sat_num1{i};
    % initialize structure
    [satrec] = sgp4init(72, satrec,init.bstar(i), init.ecc(i),...
             init.juliandate(i)-spg4_jdepoch,init.argp(i), init.inc(i),init.M(i) ,  init.n(i), init.raan(i));
    % find time between TLE epochand current epoch
    delta_time = (epoch-init.epoch(i))*24*60;
     j=1;
     tsince=0;
    while abs(tsince)<=abs(maxtime)  && satrec.error == 0 
        if j==1
            truth(i).julian_date(j) = init.juliandate(i)+delta_time/1440;
            [satrec, truth(i).r_teme(:,j), truth(i).v_teme(:,j)] = sgp4(satrec,delta_time);
        else
             
             [satrec, truth(i).r_teme(:,j), truth(i).v_teme(:,j)] = sgp4(satrec,tsince+delta_time);
             truth(i).julian_date(j) = truth(i).julian_date(j-1) +step/1440;
        end
        
        if satrec.error~=0
            error('There was a problem in the propagation')
        end
        tsince = tsince + step;
        j=j+1;
    end
           
end
% convert to ecef, J2000, and LLA
for j=1:num_sats;
    for i=1:length(truth(j).julian_date)
             [year,mon,day,hr,min,sec] = invjday ( truth(j).julian_date(i) );
             truth(j).date_num(i) = datenum([year,mon,day,hr,min,sec]);
             % find tai
             tai = truth(j).date_num(i) +taiminusutc/86400;
             % find dynamic time tt
             tt = tai+32.184/86400;
             [Y,MO,D,H,MI,S] = datevec(tt);
             JDtt = jdayall(Y,MO,D,H,MI,S,'g');
             % find julian centeries
             Ttt = (JDtt -2451545.0)/36525;
             % find jdut1
             % date of tdrss satellite TLE
             day_num  = datenum([year,mon,day]);
             % find space data at the epoch. Use observed if available, if not, use
             % predicted
             idx_epoch = find(observed.utc_num==day_num);
             if isempty(idx_epoch)
                idx_epoch = find(predicted.utc_num==day_num);
                data=predicted;
             else
                data=observed;
             end

             % if empty still, throw error
             if isempty(idx_epoch)
                 error('Astronomical data not available for this epoch!')
             end
             % pull out needed data for conversion
             x=d2r(data.x(idx_epoch)/60);
             y=d2r(data.y(idx_epoch)/60);
             dpsi= d2r(data.dpsi(idx_epoch)/60);
             deps= d2r(data.deps(idx_epoch)/60);
             UT1minusUTC=data.UT1minusUTC(idx_epoch);
             lod=data.lod(idx_epoch);
             % go teme to ecef
             ut1 = truth(j).date_num(i)+UT1minusUTC/86400;
             [Y,MO,D,H,MI,S] = datevec(ut1);
             JDut1 = jdayall(Y,MO,D,H,MI,S,'g');
             truth(j).a_teme(:,i) = -MU*truth(j).r_teme(:,i)/norm(truth(j).r_teme(:,i))^3;
             % compute the earth fixed vectors
             [truth(j).r_ecef(:,i),truth(j).v_ecef(:,i),truth(j).a_ecef(:,i)] = teme2ecef(truth(j).r_teme(:,i),...
                        truth(j).v_teme(:,i),truth(j).a_teme(:,i),Ttt,JDut1,lod,x,y );
             [truth(j).latgc(i),truth(j).lat(i),truth(j).long(i),truth(j).alt(i)] = ijk2llb ( truth(j).r_ecef(:,i) );
             % lets also provide J2000 inertial r,v
             [truth(j).r_j2000(:,i),truth(j).v_j2000(:,i),truth(j).a_j2000(:,i)] = ecef2eci  ( truth(j).r_ecef(:,i),...
                 truth(j).v_ecef(:,i),truth(j).a_ecef(:,i), Ttt,JDut1,lod,x,y,2,dpsi,deps );

    end
end

% plot the ephemerides
    if to_plot
        % make different colors
        cmap = hsv(num_sats); 
        mercator;
        legend_str={};
        j=1;
        for i=1:num_sats 
            legend_str = [legend_str, strcat('TDRS-',num2str(sat_id(i)))];
            plot( r2d(truth(i).long), r2d(truth(i).lat),'*','Color',cmap(j,:)); hold on;
            j=j+1;
            legend(legend_str)
            title(strcat('Epoch:', datestr(epoch)))
        end
        
    end

end