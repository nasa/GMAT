function [BLmean emag j]=Cart2BLlong(Cart)

tol=1e-6;
maxiter=200;
Kep=Cart2Kep2(Cart);
if Kep(2) >= 0.999999
	
		disp('Warning: Orbit is nearly parabolic or hyperbolic. So, GMAT cannot calculate mean elements.\n')
		BLmean=[0.0, 0.0, 0.0, 0.0, 0.0, 0.0];	 
        emag=[];
        j=[];
		return;
end
pseudostate=0;
if Kep(3) > 177
    Kep(3)=180-Kep(3); % INC = 180 - INC
    Kep(4)=-Kep(4); % RAAN = -RAAN
    Cart=Kep2Cart2(Kep);
    pseudostate=1;
end

BLmean=Kep

Kep2=BROLYD(BLmean)
BLmean2=BLmean+(Kep-Kep2)
if BLmean2(3) <0
    BLmean2(3)=0;
end

emag_old=1;
emag=0.9;
j=0;
while emag > tol
    BLmean=BLmean2
    

    Kep2=BROLYD(BLmean)
    cart=Kep2Cart2(Kep2) 
    if ~isempty(Kep2)
        emag=norm(Cart-Kep2Cart2(Kep2))./norm(Cart)
        
    else
        disp('GMAT could not find Brouwer Mean elements, please try another input values')
        BLmean=[];
        return 
    end
    if emag_old > emag
        emag_old=emag;
    else
        disp('the solution is not converging')
        break
    end
    
    BLmean2=BLmean+(Kep-Kep2)
    if BLmean2(3) <0
        BLmean2(3)=0;
    end
    if j >= maxiter
        disp('warning : maximum iteration number has been reached');
        break
    end
    j=j+1;
end
BLmean=BLmean2;

if pseudostate ~= 0
    BLmean(3)=180-BLmean(3); % INC = 180 - INC
    BLmean(4)=-BLmean(4); % RAAN = -RAAN
end
if BLmean(2) < 0
    BLmean(2)=BLmean(2)*-1;
    BLmean(6)=BLmean(6)+180;
    BLmean(5)=BLmean(5)-180;
end
BLmean(4)=mod(BLmean(4),2*180);
BLmean(5)=mod(BLmean(5),2*180);
BLmean(6)=mod(BLmean(6),2*180);
if BLmean(6) <0
    BLmean(6)=BLmean(6)+2*180;
end

if BLmean(4) <0
    BLmean(4)=BLmean(4)+2*180;
end

if BLmean(5) <0
    BLmean(5)=BLmean(5)+2*180;
end

