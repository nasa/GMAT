function [rho] = ExponentialAtmosphere(height,refHeight,scaleHeight,refDensity)

rho  = refDensity*exp(-(height-refHeight)/scaleHeight);