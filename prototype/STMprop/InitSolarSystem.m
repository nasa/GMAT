function SolarSystem = InitSolarSystem();

global SolarSystem

%     1 -> Mercury          8 -> Neptune
%     2 -> Venus            9 -> Pluto
%     3 -> Earth           10 -> Moon
%     4 -> Mars            11 -> Sun
%     5 -> Jupiter         12 -> Solar system barycenter
%     6 -> Saturn          13 -> Earth-Moon barycenter
%     7 -> Uranus          14 -> Nutations (longitude and obliquity)
%               15 -> Librations, if on file


%  Set the mu values
SolarSystem.Mu = [22032.080486418
                  324858.59882646 
                  398600.4415  
                  42828.314258067 
                  126712767.85780 
                  37940626.061137
                  5794549.0070719 
                  6836534.0638793 
                  981.60088770700
                  4902.8005821478
                  132712440017.99];
   
   
%  Initialize the DE file 
addpath(genpath('C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\prototype\events\bin\contrib\mice'))
addpath('C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\test\script\extern\utils\KSC_DEReader\ephm')
%init_eph('de421.dat');
% if stcmp(ForceModel.EphemerisSource,'SPICE')
cspice_furnsh( 'C:\Users\sphughe1\Documents\GMATRepos\ScriptTest\gmatdata\planetary_ephem\spk\de421.bsp' )
% cspice_furnsh( 'C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\test\files\planetary_ephem\spk\mar080.bsp')
% cspice_furnsh( 'C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\test\files\planetary_ephem\spk\jup230-long.bsp')
% cspice_furnsh( 'C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\test\files\planetary_ephem\spk\sat288.bsp')
% cspice_furnsh( 'C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\test\files\planetary_ephem\spk\ura083.bsp')
% cspice_furnsh( 'C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\test\files\planetary_ephem\spk\nep078.bsp')
% cspice_furnsh( 'C:\Users\sphughe1\Documents\GMAT Files\Jazz\trunk\test\files\planetary_ephem\spk\plu017.bsp')
% cspice_furnsh( 'C:\Users\sphughe1\Documents\GMAT\data\time\naif0009.tls')
% end
