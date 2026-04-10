function oscele =BROLYD(dpele)
%        FOR dpele(I) and oscele(I),                                 *
%        I=1, SEMIMAJOR AXIS                    -- SMA               *
%        I=2, ECCENTRI%TY                      -- ECE               *
%        I=3, INCLINATION                       -- ORBIN             *
%        I=4, LONGITUDE OF THE ASCENDING NODE   -- ANL               *
%        I=5, argUMENT OF PERIGEE               -- AP                *
%        I=6, MEAN ANOMALY                      -- DM                *
%--------------------------------------------------------------------*

if dpele(3) > 180     | dpele(3) < 0 %  | dpele(2) > 0.99999 | dpele(1)*(1-dpele(2)) < 5800
    disp('BROLYD cannot calculate osculating elements because of incorrect input values')
    disp(['SMADP',' = ', num2str(dpele(1)), '   ECCDP',' = ', num2str(dpele(2)), '   INCDP',' = ', num2str(dpele(3))])
    oscele=[];
    return
end
pseudostate=0;

if dpele(3) > 177
    dpele(3)=180-dpele(3); % INC = 180 - INC
    dpele(4)=-dpele(4); % RAAN = -RAAN
    pseudostate=1;
end
dpele(3:6)=deg2rad(dpele(3:6));
ae      = 6378.1363E0;

j2     = 0.1082626925638815E-02;
j3     = -0.2532307818191774E-5;
j4     = -0.1620429990000000E-5;
j5     = -0.2270711043920343E-6;

re      =1.0E0;
%------------------------------I
% EPOCH ELEMENTS AT EPOCH TIME I
%------------------------------I
      ADP = dpele(1)/ae(1);
      EDP = dpele(2);
      BIDP = dpele(3);
      HDP = dpele(4);
      GDP = dpele(5);
      BLDP = dpele(6);

if EDP < 0
    EDP=EDP*-1;
    BLDP=BLDP+pi;
    GDP=GDP-pi;
end

if EDP > 0.999999
    disp('BROLYD cannot calculate osculating elements for near parabolic')
    disp('or hyperbolic orbits.')
    
    disp(['SMADP',' = ', num2str(dpele(1)), '   ECCDP',' = ', num2str(EDP), '   INCDP',' = ', num2str(dpele(3))])
    oscele=[];
    return
end
%-----------------------I
% ''MEAN'' MEAN ANOMALY I
%-----------------------I
      BLDP = mod(BLDP,(2*pi));
      if BLDP < 0
          BLDP = BLDP + (2*pi);
      end
%---------------------------I
%  MEAN argUMENT OF PERIGEE I
%---------------------------I
      GDP = mod(GDP,(2*pi));
      if GDP < 0
          GDP = GDP + (2*pi);
      end
%  MEAN LONGITUDE OF ASCENDING NODE
      HDP = mod(HDP,(2*pi));     
      if HDP < 0
          HDP = HDP + (2*pi);
      end
      BK2=(1/2)*(j2*re*re);
      BK3=-j3*re^3;
      BK4=-(3/8)*(j4*re^4);
      BK5=-j5*re^5;
      EDP2=EDP*EDP;
      CN2=1.0-EDP2;
      CN=sqrt(CN2);
      GM2=BK2/ADP^2;
      GMP2=GM2/(CN2*CN2);
      GM4=BK4/ADP^4;
      GMP4=GM4/CN^8;
      THETA=cos(BIDP);
      THETA2=THETA*THETA;
      THETA4=THETA2*THETA2;

      
      GM3=BK3/ADP^3;
      GMP3=GM3/(CN2*CN2*CN2);
      GM5=BK5/ADP^5;
      GMP5=GM5/CN^10;
      G3DG2=GMP3/GMP2;
      G4DG2=GMP4/GMP2;
      G5DG2=GMP5/GMP2;

	  SINLDP=sin(BLDP);
      COSLDP=cos(BLDP);
      SINHDP=sin(HDP);
      COSHDP=cos(HDP);

%-------------------------------------I
% COMPUTE TRUE ANOMALY(DOUBLE PRIMED) I
%-------------------------------------I
      EADP = DKEPLR(BLDP,EDP);
      SINDE= sin(EADP);
      COSDE= cos(EADP);
      SINFD= CN*SINDE;
      COSFD= COSDE - EDP;
      FDP=atan0(SINFD,COSFD);
      DADR=(1.0-EDP*COSDE)^(-1);
      SINFD=SINFD*DADR;
      COSFD=COSFD*DADR;
      CS2GFD=cos(2.0*GDP+2.0*FDP);
      DADR2=DADR*DADR;
      DADR3=DADR2*DADR;
      COSFD2=COSFD*COSFD;
%---------------I
% COMPUTE A1-A8 I
%---------------I
      A1=((1/8)*GMP2*CN2)*(1.0-11.0*THETA2-((40.0*THETA4)/(1.0-5.0*THETA2)));
      A2=((5/12)*G4DG2*CN2)*(1.0-((8.0*THETA4)/(1.0-5.0*THETA2))-3.0*THETA2);
      A3=G5DG2*((3.0*EDP2)+4.0);
      A4=G5DG2*(1.0-(24.0*THETA4)/(1.0-5.0*THETA2)-9.0*THETA2);
      A5=(G5DG2*(3.0*EDP2+4.0))*(1.0-(24.0*THETA4)/(1.0-5.0*THETA2)-9.0*THETA2);
      A6=G3DG2*(1/4);
      SINI=sin(BIDP);
      A10=CN2*SINI;
      A7=A6*A10;
      A8P=G5DG2*EDP*(1.0-(16.0*THETA4)/(1.0-5.0*THETA2)-5.0*THETA2);
      A8=A8P*EDP;
%   COMPUTE B13-B15
      B13=EDP*(A1-A2);
      B14=A7+(5/64)*A5*A10;
      B15=A8*A10*(35/384);
%  COMPUTE A11-A27
      A11=2.0+EDP2;
      A12=3.0*EDP2+2.0;
      A13=THETA2*A12;
      A14=(5.0*EDP2+2.0)*(THETA4/(1.0-5.0*THETA2));
      A17=THETA4/((1.0-5.0*THETA2)*(1.0-5.0*THETA2));
      A15=(EDP2*THETA4*THETA2)/((1.0-5.0*THETA2)*(1.0-5.0*THETA2));
      A16=THETA2/(1.0-5.0*THETA2);
      A18=EDP*SINI;
      A19=A18/(1.0+CN);
      A21=EDP*THETA;
      A22=EDP2*THETA;
      SINI2=sin(BIDP/2.0);
      COSI2=cos(BIDP/2.0);
      TANI2=tan(BIDP/2.0);
      A26=16.0*A16+40.0*A17+3.0;
      A27=A22*(1/8)*(11.0+200.0*A17+80.0*A16);
%----------------I
% COMPUTE B1-B12 I
%----------------I
      B1=CN*(A1-A2)-((A11-400.0*A15-40.0*A14-11.0*A13)*(1/16)+(11.0+200.0*A17+80.0*A16)*A22*(1/8))*GMP2+((-80.0*A15-8.0*A14-3.0*A13+A11)*(5/24)+(5/12)*A26*A22)*G4DG2;
      B2=A6*A19*(2.0+CN-EDP2)+(5/64)*A5*A19*CN2-(15/32)*A4*A18*CN*CN2+((5/64)*A5+A6)*A21*TANI2+(9.0*EDP2+26.0)*(5/64)*A4*A18+(15/32)*A3*A21*A26*SINI*(1.0-THETA);
      B3=((80.0*A17+5.0+32.0*A16)*A22*SINI*(THETA-1.0)*(35/576)*G5DG2*EDP)-((A22*TANI2+(2.0*EDP2+3.0*(1.0-CN2*CN))*SINI)*(35/1152)*A8P);
      B4=CN*EDP*(A1-A2);
      B5=((9.0*EDP2+4.0)*A10*A4*(5/64)+A7)*CN;
      B6=(35/384)*A8*CN2*CN*SINI;
      B7=((CN2*A18)/(1.0-5.0*THETA2))*((1/8)*GMP2*(1.0-15.0*THETA2)+(1.0-7.0*THETA2)*G4DG2*(-(5/12)));
      B8=(5/64)*(A3*CN2*(1.0-9.0*THETA2-(24.0*THETA4/(1.0-5.0*THETA2))))+A6*CN2;
      B9=A8*(35/384)*CN2;
      B10=SINI*(A22*A26*G4DG2*(5/12)-A27*GMP2);
      B11=A21*(A5*(5/64)+A6+A3*A26*(15/32)*SINI*SINI);
      B12=-((80.0*A17+32.0*A16+5.0)*(A22*EDP*SINI*SINI*(35/576)*G5DG2)+(A8*A21*(35/1152)));
%----------------------------I
% COMPUTE A(SEMI-MAJOR AXIS) I
%----------------------------I
      A=ADP*(1.0+GM2*((3.0*THETA2-1.0)*(EDP2/(CN2*CN2*CN2))*(CN+(1.0/(1.+CN)))+((3.0*THETA2-1.0)/(CN2*CN2*CN2))*(EDP*COSFD)*(3.0+3.0*EDP*COSFD+EDP2*COSFD2)+3.0*(1.0-THETA2)*DADR3*CS2GFD));
      SN2GFD=sin(2.0*GDP+2.0*FDP);
      SNF2GD=sin(2.0*GDP+FDP);
      CSF2GD=cos(2.0*GDP+FDP);
      SN2GD=sin(2.0*GDP);
      CS2GD=cos(2.0*GDP);
      SN3GD=sin(3.0*GDP);
      CS3GD=cos(3.0*GDP);
      SN3FGD=sin(3.0*FDP+2.0*GDP);
      CS3FGD=cos(3.0*FDP+2.0*GDP);
      SINGD=sin(GDP);
      COSGD=cos(GDP);
%------------------------I
% COMPUTE (L+G+H) PRIMED I
%------------------------I

% COMPUTE ISUBC TO TEST CRITICAL INCLINATION

BISUBC=((1.0-5.0*THETA2)^(-2))*((25.0*THETA4*THETA)*(GMP2*EDP2));

if BISUBC >= 0.1
% MODIFICATIONS FOR CRITICAL INCLINATION
    disp('Mean inclination is close to the critial inclination 63 deg. 26 min. Result can be inaccurate')
    DLT1E=0.0;
    BLGHP=0.0;
    EDPDL=0.0;
    DLTI=0.0;
    SINDH=0.0;
else
    
	DLT1E=B14*SINGD+B13*CS2GD-B15*SN3GD;
%
% COMPUTE (L+G+H) PRIMED
%
      BLGHP=HDP+GDP+BLDP+B3*CS3GD+B1*SN2GD+B2*COSGD;
      BLGHP=mod(BLGHP,(2*pi));
      if BLGHP < 0
          BLGHP=BLGHP+(2*pi);
      end
      EDPDL=B4*SN2GD-B5*COSGD+B6*CS3GD-(1/4)*CN2*CN*GMP2*(2.0*(3.0*THETA2-1.0)*(DADR2*CN2+DADR+1.0)*SINFD+3.0*(1.0-THETA2)*((-DADR2*CN2-DADR+1.0)*SNF2GD+(DADR2*CN2+DADR+(1/3))*SN3FGD));
      DLTI=(1/2)*THETA*GMP2*SINI*(EDP*CS3FGD+3.0*(EDP*CSF2GD+CS2GFD))-(A21/CN2)*(B8*SINGD+B7*CS2GD-B9*SN3GD);
      SINDH=(1.0/COSI2)*((1/2)*(B12*CS3GD+B11*COSGD+B10*SN2GD-((1/2)*GMP2*THETA*SINI*(6.0*(EDP*SINFD-BLDP+FDP)-(3.0*(SN2GFD+EDP*SNF2GD)+EDP*SN3FGD)))));
end

%-----------------I
% COMPUTE (L+G+H) I
%-----------------I
      BLGH=BLGHP+((1.0/(CN+1.0))*(1/4)*EDP*GMP2*CN2*(3.0*(1.0-THETA2)*(SN3FGD*((1/3)+DADR2*CN2+DADR)+SNF2GD*(1.0-(DADR2*CN2+DADR)))+2.0*SINFD*(3.0*THETA2-1.0)*(DADR2*CN2+DADR+1.0))) ...
          +GMP2*(3/2)*((-2.0*THETA-1.0+5.0*THETA2)*(EDP*SINFD+FDP-BLDP))+(3.0+2.0*THETA-5.0*THETA2)*(GMP2*(1/4)*(EDP*SN3FGD+3.0*(SN2GFD+EDP*SNF2GD)));
      BLGH=mod(BLGH,(2*pi));
      if BLGH < 0
          BLGH=BLGH+(2*pi);
      end
      DLTE=DLT1E+((1/2)*CN2*((3.0*(1.0/(CN2*CN2*CN2))*GM2*(1.0-THETA2)*CS2GFD*(3.0*EDP*COSFD2+3.0*COSFD+EDP2*COSFD*COSFD2+EDP)) ...
          -(GMP2*(1.0-THETA2)*(3.0*CSF2GD+CS3FGD))+(3.0*THETA2-1.0)*GM2*(1.0/(CN2*CN2*CN2))*(EDP*CN+(EDP/(1.0+CN))+3.0*EDP*COSFD2+3.0*COSFD+EDP2*COSFD*COSFD2)));
      EDPDL2=EDPDL*EDPDL;
      EDPDE2=(EDP+DLTE)*(EDP+DLTE);
%-------------------------I
% COMPUTE E(ECCENTRI%TY) I
%-------------------------I
      E=sqrt(EDPDL2+EDPDE2);
      SINDH2=SINDH*SINDH;
      SQUAR=(DLTI*COSI2*(1/2)+SINI2)*(DLTI*COSI2*(1/2)+SINI2);
      SQRI=sqrt(SINDH2+SQUAR);
%--------------------------I
% COMPUTE BI (INCLINATION) I
%--------------------------I
      BI=2*asin(SQRI);
      BI=mod(BI,(2*pi));
      

%-------------------------I
% COMPUTE L(MEAN ANOMALY) I
%-------------------------I
 if E==0
     BL=0;
 else
      arg1=EDPDL*COSLDP+(EDP+DLTE)*SINLDP;
      arg2=(EDP+DLTE)*COSLDP-(EDPDL*SINLDP);
      BL=atan2(arg1,arg2);
      BL=mod(BL,(2*pi));
      if BL < 0
          BL=BL+(2*pi);
      end
 end
%----------------------------------------I
% COMPUTE H(LONGITUDE OF ASCENDING NODE) I
%----------------------------------------I
 if BI==0 & pseudostate == 0
     H=0;
     G=BLGH-BL-H;
 else
	  arg1=SINDH*COSHDP+SINHDP*((1/2)*DLTI*COSI2+SINI2);
      arg2=COSHDP*((1/2)*DLTI*COSI2+SINI2)-(SINDH*SINHDP);
      H=atan2(arg1,arg2);
      G=BLGH-BL-H;
 end
 
 if BI == 0 & pseudostate ~= 0
    G = 0;
    H=BLGH-BL-G;
 end
 
%---------------------------------I
% COMPUTE G(argUMENT  OF PERIGEE) I
%---------------------------------I
	  H=mod(H,(2*pi));
      G=mod(G,(2*pi));

      if H < 0
          H=H+(2*pi);
      end
      
      if G < 0
          G=G+(2*pi);
      end
%----------------------------------------I
% COMPUTE oscele                         I
%----------------------------------------I
      oscele(1) = A * ae(1);
      oscele(2) = E;
      oscele(3) = BI;
      oscele(4) = H;
      oscele(5) = G;
      oscele(6) = BL;
      oscele(3:6)=rad2deg(oscele(3:6));
      
if pseudostate ~= 0
    oscele(3)=180-oscele(3); % INC = 180 - INC
    oscele(4)=-oscele(4); % RAAN = -RAAN
end