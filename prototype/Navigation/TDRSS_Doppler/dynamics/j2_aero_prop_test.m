clc; clear

% 
% Test the propagator 

% iss obit
inc=51.6;
mass =450000;
area=3800;
cd=2.0;
[r_user_init,v_user_init] = orbital2eci((RE+410),0.0001,inc,0,0,0);
r_user_j2k=r_user_init*1000;
v_user_j2k=v_user_init*1000;

% test the user propagation
i=1;
r_test(i,:)=r_user_init*1000;
v_test(i,:)=v_user_init*1000;
t=0;
while t <5400
    [ h_mag a(i) inc(i) OMEGA e_mag(i) omega ta ] = eci2orbital(r_test(i,:), v_test(i,:));
    i=i+1;
    [r_test(i,:),v_test(i,:),dt] = j2_aero_prop(r_test(i-1,:),v_test(i-1,:),1,'j2_on',1,'drag_on',1,'mass',mass,'area',area,'cd',cd);
    t=t+1;

end


  % plot sma,inc, e does the traceloo right?
  figure(1)
  plot((a-RE('m'))/1000)
  xlabel('Time(s)')
  ylabel('Semjax (km)')  

  figure(2)
  plot(inc)
  xlabel('Time(s)')
  ylabel('inc (deg)')  


  figure(3)
  plot(e_mag)
  xlabel('Time(s)')
  ylabel('eccentricity')  