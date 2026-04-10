function [StateRate,Tank] = BlowDownTank_GetDerivative(Tank,X,mdote)


%-----------------------------------------------
%   Set tank state using inputs contained in X
%-----------------------------------------------

Tank = BlowDownTank_SetState(Tank,X);

%-----------------------------------------------
%              The Isothermal model
%-----------------------------------------------
switch Tank.ThermoModel

    case 'Isothermal'

        %  Set state parameter from input
        StateRate(1) = -mdote/(1 - 0*Tank.Pv*Tank.vl/(Tank.Rv*Tank.Tg));
        return

end

%-----------------------------------------------
%     The Adiabatic and Heat Transfer Models
%-----------------------------------------------

%  Calculate terms common to both Adiabatic and Heat Transfer models
hv  = Tank.Tg*(Tank.cv + Tank.Rv);
A11 = Tank.Tg*Tank.Rv - Tank.Pt*Tank.vl;
A13 = Tank.mv*Tank.cv + Tank.mg*Tank.cg;
A21 = Tank.cl*Tank.Tl + Tank.Pt*Tank.vl - hv;
A22 = Tank.ml*Tank.cl;
A34 = Tank.mw*Tank.cw;
A41 = 1 - Tank.vl*Tank.Pv/Tank.Rv/Tank.Tg;
A43 = -Tank.mv/Tank.Tg;
denom = A11*A43 - A41*A13;

%  Calculate terms specific to each model
switch Tank.ThermoModel

    case {'Adiabatic'}

        %  Calculate bi with Qdot set to zero
        b1 = -mdote*Tank.Tg*Tank.Rv;
        b2 = mdote*(hv - Tank.cl*Tank.Tl);
        b4 = -mdote;

        %  Calculate derivatives
        StateRate(1,1) = (A43*b1 -A13*b4)/denom;
        StateRate(2,1) = (b2 - A21*(A43*b1 - A13*b4)/denom)/A22;
        StateRate(3,1) = (A11*b4 - A41*b1)/denom;

    case {'HeatTransfer'}

        %  Caclulate surface areas of control volumes
        V23 = Tank.Vt^(2/3);
        Aw = pi*Tank.D^2;
        Ag = 4.0675*V23*(Tank.Vg/Tank.Vt)^(0.62376);
        Ab = Ag - 3.4211*V23*(Tank.Vg/Tank.Vt)^(1.24752);
        Al = Aw - Ag;

        %  Calculate the heat transfer rates
        Qdotg = Tank.hg*Ag*(Tank.Tw - Tank.Tg);
        Qdotv = Tank.hf*Ab*(Tank.Tl - Tank.Tg);
        Qdotl = Tank.ht*Al*(Tank.Tw - Tank.Tl);
        Qdotw = Tank.hw*Aw*(Tank.Ta - Tank.Tw);

        %  Calculate bi
        b1 = Qdotv + Qdotg -mdote*Tank.Tg*Tank.Rv;
        b2 = Qdotl - Qdotv + mdote*(hv - Tank.cl*Tank.Tl);
        b3 = Qdotw - Qdotl - Qdotg;
        b4 = -mdote;

        %  Calculate derivatives
        StateRate(1,1) = (A43*b1 -A13*b4)/denom;
        StateRate(2,1) = (b2 - A21*(A43*b1 - A13*b4)/denom)/A22;
        StateRate(3,1) = (A11*b4 - A41*b1)/denom;
        StateRate(4,1) = b3/A34;

end

