function [ Kep ] = Dela2Kep(Dela,mu)

% % If mu is not provided, use the mu for Earth
if nargin < 2
    mu = 398600.4415;
end

if ( length(Dela) ~= 6 )
    Kep = [];
    disp('Error: Delaunay Orbital Elements have six elements')
    return
end

% % Initializations
L = Dela(1);
G = Dela(2);
H = Dela(3);
ll = Dela(4);
gg = Dela(5);
hh = Dela(6);

SMA = L^2 / mu;
ECC = sqrt( 1-(G/L)^2 );
if ( imag(ECC) ~= 0 )
    ECC = abs(ECC);
end
INC = acos(H/G); %
MA = ll;
AOP = gg; 
RAAN = hh;

% % if ( ((-pi < MA) && (MA < 0)) || (MA > pi) )
% %     EA(1) = MA - ECC;
% % else
% %     EA(1) = MA + ECC;
% % end
% % 
% % tol = 1e-7; i = 1; del = 1; iterMax = 1000;
% % while ( del > tol )
% %     EA(i+1) = EA(i) + ( MA-EA(i)+ECC*sin(EA(i)) ) / ( 1-ECC*cos(EA(i)) );
% %     del = abs(EA(i+1)-EA(i));
% %     i = i+1;    
% %     if ( i > iterMax )
% %         disp('too many iteration for calculating Kepler equation')
% %         break;
% %     end
% % end
% % 
% % tEA = EA(end);
% % sinTA = (sin(tEA)*sqrt(1-ECC^2)) / (1-ECC*cos(tEA));
% % cosTA = (cos(tEA)-ECC) / (1-ECC*cos(tEA));
% % % TA = atan3(sinTA, cosTA);
% % TA = mod(atan2(sinTA,cosTA),2*pi);
TA = M2nu(MA,ECC);
Kep = [ SMA ECC INC AOP RAAN TA]';

end
