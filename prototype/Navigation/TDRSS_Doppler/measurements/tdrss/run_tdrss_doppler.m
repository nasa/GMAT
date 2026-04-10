% wrapper script for TdrsssDopplerMeasurement.m
clc; clear all; close all;

%start timer
tic

% transmit frequency
fref=2255503200; %hz

% User service
user_service = 'ssa-1';

% user sat configuration
user='gmat_user';

% use/dont use config tles
use_config_tle=1;

% User satellite example (TRMM) a prior conditions
mass =2671;
area=16;
cd=2.2;
r_user_init = [3312.091811 ;   4740.266565 ; -3535.858658 ];  
v_user_init = [ -5.392818297 ;5.134414847 ; 1.833577623 ];
r_user_j2k_init=r_user_init*1000;
v_user_j2k_init=v_user_init*1000;

% utc epoch, and TDRSS participants
epoch = '2014-03-03 12:37:10';
epoch_sv = '2014-03-04 00:00:10';
delta = (datenum(epoch)-datenum(epoch_sv))*86400;

% snap state vector epoch to measurement epoch
[r_user_j2k,v_user_j2k,dt] = j2_aero_prop(r_user_j2k_init,v_user_j2k_init,delta);
use_tdrss_id = [9];

% duration corresponds to timespan in measurement file
 total_time = (60*7+40);

% doppler count
d_count = 10; 

% call the measurement model
[range,Fd,r_ephem,v_ephem]=TdrssDopplerMeasurement(epoch, d_count,user_service,use_tdrss_id,...
                                 r_user_j2k, v_user_j2k,'tle',user,mass,area,cd,fref,...
                                 total_time,'use_config_tle',1);
% convert to ecef
for i=1:length(r_ephem)
    a_eci(:,i) = -MU*r_ephem(:,i)/norm(r_ephem(:,i))^3;
    [r_ecef(:,i),v_ecef(:,i),a_ecef(:,i)] = J2000_to_ECEF(r_ephem(:,i),v_ephem(:,i),a_eci(:,i),2,range(i).utc_date_num);   
    [lat(i), long(i), alt(i)] = ecef2lla(r_ecef(:,i));
    
end


% visualization of the problem
whitesands.stgt.lat  =  32.5230;
whitesands.stgt.long = -106.6120;
truth = TdrssTruth('epoch',epoch,'to_plot',1,'maxtime',total_time,'step',1,'use_tdrss_id',[7,8,9,10,11],'use_config_tle',1);
plot( long, lat,'*','Color','yellow'); hold on;
plot(whitesands.stgt.long,whitesands.stgt.lat,'*','Color','cyan')
legend('TDRS-7','TDRS-8','TDRS-9','TDRS-10','TDRS-11','TRMM','Whitesands')

toc
