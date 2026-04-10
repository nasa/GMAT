function [State] = BlowDownTank_GetState(Tank)

switch Tank.ThermoModel

    case 'Isothermal'

        %  Calculate dependent tank parameters
        State(1,1) = Tank.ml
        
    case {'Adiabatic'}

        %  Set state parameters from input
        State(1,1) =  Tank.ml;
        State(2,1) =  Tank.Tl;
        State(3,1) =  Tank.Tg;

    case {'HeatTransfer'}

        %  Set state parameters from input
        State(1,1) =  Tank.ml;
        State(2,1) =  Tank.Tl;
        State(3,1) =  Tank.Tg;
        State(4,1) =  Tank.Tg;

end
