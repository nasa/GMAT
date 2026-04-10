function WriteDebug()
%%  Write out debug data
if Debug
    disp(['Year: '   num2str(year)])
    disp(['Day of Year: '   num2str(doy)])
    disp(['Second of Day: ' num2str(sec,16)]);
    disp(['Longitude, (deg.): ' num2str(lon*180/pi,16)])
    disp(['Latitude, (deg.): '  num2str(geolat*180/pi,16)])
    disp(['Height, (km): '  num2str(geolat*180/pi,16)])
    disp(['Density, (kg/km^3): '   num2str(rho,16)])
    disp(['Spin Axis X (rad/s); '  num2str(omega(1),16)]);
    disp(['Spin Axis Y (rad/s); '  num2str(omega(2),16)]);
    disp(['Spin Axis Z (rad/s); '  num2str(omega(3),16)]);
    disp(['Total accel X (km/s^2): ' num2str(Xdot(4,1),16)]);
    disp(['Total accel X (km/s^2): ' num2str(Xdot(5,1),16)]);
    disp(['Total accel X (km/s^2): ' num2str(Xdot(6,1),16)]);
    disp(['Drag accel X (km/s^2): '  num2str(accDrag(1,1),16)]);
    disp(['Drag accel Y (km/s^2): '  num2str(accDrag(2,1),16)]);
    disp(['Drag accel Z (km/s^2): '  num2str(accDrag(3,1),16)]);
    disp(['Drag Relativity X (km/s^2): '  num2str(accRelativity(1,1),16)]);
    disp(['Drag Relativity Y (km/s^2): '  num2str(accRelativity(2,1),16)]);
    disp(['Drag Relativity Z (km/s^2): '  num2str(accRelativity(3,1),16)]);
    disp(['F107A : ' num2str(Force.MSISE.F107A,16)]);
    disp(['F107 : ' num2str(Force.MSISE.F107,16)]);
    disp(['Ap : ' num2str(Force.MSISE.Ap,16)]);
end
