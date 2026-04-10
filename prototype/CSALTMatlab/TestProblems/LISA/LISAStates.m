function [stateVec,a1ModJulianEpoch] = LISAStates(stateName,repType)

% States should be in sun-centered mean earth equator of J2000
muSun = 132712440017.99;

switch stateName
    case 'NominalLaunch-8DayCheckout'
        cartState = [-147763933.119309
            -18561787.84952292
            -7697831.981676191
            3.786798547985236
            -28.1513721981408
            -11.81021407232496];
        a1ModJulianEpoch = 34050.0208306428;
    case 'ESA-LISA1Nominal'
        a1ModJulianEpoch = 34513.3996278793;
        cartState = [-25121521.015837
            -135322619.12155
            -59959666.833997
            29.237272788490
            -4.5635311521085
            -2.0639556428154 ];
    case 'ESA-LISA2Nominal'
        a1ModJulianEpoch = 34513.3996278793;
        cartState = [-24411732.245574
            -135139151.85546
            -57569897.996757
            29.484994940395
            -4.5224358867576
            -2.1421193862704 ];
    case 'ESA-LISA3Nominal'
        a1ModJulianEpoch = 34513.3996278793;
        cartState = [-26827909.409107
            -134839394.65083
            -58191319.431271
            29.367825332076
            -4.8892098789031
            -1.8538217919739];
    case 'AlternateLaunch-8DayCheckout'
        cartState = [-147763281.4573298
            -20994566.2400329
            -8694510.655455915
            3.926476087321248
            -28.13266280595623
            -11.78122017122741     ];
        a1ModJulianEpoch = 34050.99962787928;
    case 'AlternateLaunch-30DayCheckout'
        cartState = [-130114305.8487434
            -71389186.34397227
            -29947958.94214279
            14.35805552875861
            -24.37621730475227
            -10.3293633768908          ];
        a1ModJulianEpoch = 34072.99962787928;
    case 'AlternateLaunch-60DayCheckout'
        cartState = [-78398469.04367553
            -123035978.3566301
            -51934147.96707047
            24.60303556233616
            -14.77098110558508
            -6.327514564086196           ];
        a1ModJulianEpoch = 34102.99962787928;
end

if strcmp(repType,'ModifiedEquinoctial')
    stateVec = Cart2Mee(cartState,muSun);
elseif strcmp(repType,'Cartesian')
    stateVec = cartState;
else
    error('Unimplemented state type conversion requested')
end