function [Tank] = BlowDownTank_SetState(Tank,X)

%-----------------------------------------------
%  Set state parameters according to ThermoModel
%-----------------------------------------------
switch Tank.ThermoModel

    case 'Isothermal'

        %  Set state parameter from input
        Tank.ml = X(1);

    case {'Adiabatic'}

        %  Set state parameters from input
        Tank.ml = X(1);
        Tank.Tl = X(2);
        Tank.Tg = X(3);

    case {'HeatTransfer'}

        %  Set state parameters from input
        Tank.ml = X(1);
        Tank.Tl = X(2);
        Tank.Tg = X(3);
        Tank.Tw = X(4);

end

%-----------------------------------------------
%  Calculate dependent tank parameters
%-----------------------------------------------
Tank.Vl = Tank.vl*Tank.ml;
Tank.Vg = Tank.Vt - Tank.Vl;
Tank.Pg = Tank.mg*Tank.Rg*Tank.Tg/Tank.Vg;
Tank.Pv = Tank.Pv;
Tank.mv = Tank.Pv*Tank.Vg/Tank.Rv/Tank.Tg;
Tank.Pt = Tank.Pv + Tank.Pg;