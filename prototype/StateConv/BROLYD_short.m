function oscele=BROLYD_short(elep)

pseudostate=0;
if elep(3) > 180    | elep(3) < 0 % | elep(2) > 0.99999 | elep(1)*(1-elep(2)) < 5800
    disp('BROLYD cannot calculate osculating elements because of incorrect input values')
    
    disp(['SMADP',' = ', num2str(elep(1)), '   ECCDP',' = ', num2str(elep(2)), '   INCDP',' = ', num2str(elep(3))])
    oscele=[];
    return
end

if elep(3) > 177
    elep(3)=180-elep(3); % INC = 180 - INC
    elep(4)=-elep(4); % RAAN = -RAAN
    pseudostate=1;
end
elep(3:6)=deg2rad(elep(3:6));
j2     = 0.1082626925638815E-02;
%j2=0.10826158E-02;

re      = 6378.1363;
ae=1;

smap=elep(1)/re;
eccp=elep(2);
incp=elep(3);
raanp=elep(4);
aopp=elep(5);
map=elep(6);

if eccp < 0
    eccp=eccp*-1;
    map=map+pi;
    aopp=aopp-pi;
end

if eccp > 0.999999
    disp('BROLYD cannot calculate osculating elements for near parabolic')
    disp('or hyperbolic orbits.')
    
    disp(['SMADP',' = ', num2str(elep(1)), '   ECCDP',' = ', num2str(eccp), '   INCDP',' = ', num2str(elep(3))])
    
    oscele=[];
    return
end
raanp = mod(raanp,2*pi);
if raanp < 0
   raanp = raanp + 2*pi;
end

aopp = mod(aopp,2*pi);
if aopp < 0
   aopp = aopp + 2*pi;
end

map = mod(map,2*pi);
if map < 0
   map = map + 2*pi;
end

eta=sqrt(1-eccp^2);
theta=cos(incp);
p=smap*eta^2;
k2=1/2*j2*ae*ae;
gm2=k2/smap^2;
gm2p=gm2/eta^4;

eap=DKEPLR(map,eccp);
tap=2*atan(sqrt(1+eccp)/sqrt(1-eccp)*tan(eap/2));
if tap < 0
	tap=tap+2*pi;
end


rp=p/(1+eccp*cos(tap));
adr=smap/rp;


sma1=smap+smap*gm2*((adr^3-1/eta^3)*(-1+3*theta^2)+3*(1-theta^2)*adr^3*cos(2*aopp+2*tap));

decc=eta^2/2*((3*(1/eta^6)*gm2*(1-theta^2)*cos(2*aopp+2*tap)*(3*eccp*cos(tap)^2+3*cos(tap)+eccp^2*cos(tap)^3+eccp)) ...
    -gm2p*(1-theta^2)*(3*cos(2*aopp+tap)+cos(3*tap+2*aopp))+(3*theta^2-1)*gm2/eta^6*(eccp*eta+eccp/(1+eta)+3*eccp*cos(tap)^2+3*cos(tap)+eccp^2*cos(tap)^3));
      
dinc=gm2p/2*theta*sin(incp)*(3*cos(2*aopp+2*tap)+3*eccp*cos(2*aopp+tap)+eccp*cos(2*aopp+3*tap));

%eta=sqrt(1-eccp^2);

draan= -gm2p/2*theta*(6*(tap-map+eccp*sin(tap)) ...
    -3*sin(2*aopp+2*tap)- 3*eccp*sin(2*aopp+tap) - eccp*sin(2*aopp+3*tap));

aop1=aopp+ 3*j2*ae^2/2/p^2*((2-5/2*sin(incp)^2)*(tap-map+eccp*sin(tap)) ...
    + (1-3/2*sin(incp)^2)*(1/eccp*(1-1/4*eccp^2)*sin(tap)+1/2*sin(2*tap)+eccp/12*sin(3*tap)) ...
    - 1/eccp*(1/4*sin(incp)^2 + (1/2-15/16*sin(incp)^2)*eccp^2)*sin(tap+2*aopp) ...
    + eccp/16*sin(incp)^2*sin(tap-2*aopp) -1/2*(1-5/2*sin(incp)^2)*sin(2*tap+2*aopp) ...
    + 1/eccp*(7/12*sin(incp)^2-1/6*(1-19/8*sin(incp)^2)*eccp^2)*sin(3*tap+2*aopp) ...
    + 3/8*sin(incp)^2*sin(4*tap+2*aopp) + eccp/16*sin(incp)^2*sin(5*tap+2*aopp));

ma1=map+ 3*j2*ae^2*eta/2/eccp/p^2*(-(1-3/2*sin(incp)^2)*((1-eccp^2/4)*sin(tap)+eccp/2*sin(2*tap)+eccp^2/12*sin(3*tap)) ...
+ sin(incp)^2*(1/4*(1+5/4*eccp^2)*sin(tap+2*aopp)-eccp^2/16*sin(tap-2*aopp) -7/12*(1-eccp^2/28)*sin(3*tap+2*aopp) ...
    -3*eccp/8*sin(4*tap+2*aopp)-eccp^2/16*sin(5*tap+2*aopp)));

lgh=raanp+aopp+map+gm2p/4*(6*(-1-2*theta+5*theta^2)*(tap-map+eccp*sin(tap)) ...
    + (3 + 2*theta- 5*theta^2)*(3*sin(2*aopp+2*tap)+3*eccp*sin(2*aopp+tap)+eccp*sin(2*aopp+3*tap))) ...
    + gm2p/4*eta^2/(eta+1)*eccp*(3*(1-theta^2)*(sin(3*tap+2*aopp)*(1/3+adr^2*eta^2+adr)+sin(2*aopp+tap)*(1-adr^2*eta^2-adr))+2*sin(tap)*(3*theta^2-1)*(1+adr^2*eta^2+adr));


eccpdl=-eta^3/4*gm2p*(2*(-1+3*theta^2)*(adr^2*eta^2+adr+1)*sin(tap) ...
    + 3*(1-theta^2)*((-adr^2*eta^2-adr+1)*sin(2*aopp+tap) + (adr^2*eta^2+adr+1/3)*sin(2*aopp+3*tap)));
ecosl=(eccp+decc)*cos(map)-eccpdl*sin(map);
esinl=(eccp+decc)*sin(map)+eccpdl*cos(map);
ecc1=sqrt(ecosl^2+esinl^2);
if ecc1 == 0
    ma1=0;
else
    ma1=atan0(esinl,ecosl);
end

sinhalfisinh=(sin(0.5*incp)+cos(0.5*incp)*0.5*dinc)*sin(raanp)+sin(incp/2)*draan*cos(raanp);
sinhalficosh=(sin(0.5*incp)+cos(0.5*incp)*0.5*dinc)*cos(raanp)-sin(incp/2)*draan*sin(raanp);
sqrt(sinhalfisinh^2+sinhalficosh^2)
inc1=2*asin(sqrt(sinhalfisinh^2+sinhalficosh^2));
    
if inc1 ==0 & pseudostate == 0
    raan1=0;
    aop1=lgh-ma1-raan1;
else
    raan1=atan0(sinhalfisinh,sinhalficosh);
    aop1=lgh-ma1-raan1;
end

if inc1 ==0 & pseudostate ~=0
    aop1 = 0;
    raan1=lgh-ma1-aop1;
end

aop1=mod(aop1,2*pi);
if aop1 < 0
    aop1=aop1+2*pi;
end


raan1=mod(raan1,2*pi);
if raan1 < 0
    raan1=raan1+2*pi;
end
oscele=[sma1*re ecc1 inc1 raan1 aop1 ma1];

oscele(3:6)=rad2deg(oscele(3:6));

if pseudostate ~= 0
    oscele(3)=180-oscele(3); % INC = 180 - INC
    oscele(4)=-oscele(4); % RAAN = -RAAN
end


