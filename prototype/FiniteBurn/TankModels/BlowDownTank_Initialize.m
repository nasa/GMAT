function [Tank] = BlowDownTank_Initialize(Tank)

%-------------------------------------
%  Set properties of gas
%-------------------------------------
switch Tank.GasType

    case 'Helium'
        
        Tank.Rg = 2075;
        Tank.cg = 3138;

    case 'Nitrogen'

        Tank.Rg = 593;
        Tank.cg = 1485;
end

%-------------------------------------
%  Set properties of liquid and vapor
%-------------------------------------
switch Tank.LiquidType

    case 'N2O4'

        Tank.cv = 753;
        Tank.Rv = 90.4;
        Tank.Pv = 100013;
        Tank.vl = 7.09e-4;
        Tank.cl = 1582;

    case 'N2H4'
        
        %Tank.cv = 753;
        %Tank.Rv = 90.4;
        Stop
        Tank.Pv = 1917.175;
        Tank.vl = 1/1008.3;
        Tank.cl = 3086.5;
        
    case 'MMH'

end

%-------------------------------------
%  Set tank properties
%-------------------------------------
Tank.hf = 14.1;
Tank.hg = 4.72;
Tank.ht = 84.7;
Tank.hw = 0;
Tank.cw = 460;
Tank.D  = (6*Tank.Vt/pi)^(1/3);

%-------------------------------------
%  Set dependent tank parameters
%-------------------------------------
%  Detemine properties of tank that are dependent upon user parameteres
%   -  Here we convert the user provide data in terms of pressure, to
%   internal data, mass.
Tank.Pg = Tank.Pt - Tank.Pv;
Tank.Vl = Tank.vl*Tank.ml;
Tank.Vg = Tank.Vt - Tank.Vl;
Tank.mg = Tank.Pg*Tank.Vg/Tank.Rg/Tank.Tg;
Tank.mv = Tank.Pv*Tank.Vg/Tank.Rv/Tank.Tg;
