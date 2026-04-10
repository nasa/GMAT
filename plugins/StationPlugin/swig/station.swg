// Common SWIG imports for Station plugin

%apply double[] {double *};

ARRAYRETURN(double, Double, GroundStation, GetEstimationParameterValue(const Integer), GetEstimationParameterSize(arg2))
ARRAYRETURNSIZE(double, Double, GroundStation, IsValidElevationAngle(const Rvector6 &), 3)

%include "std_string.i"
%{
#include "StationAPI.hpp"
%}

// Plugin pieces to expose
%include "StationDefs.hpp"

//Does not link on Windows
%ignore GroundStation::PARAMETER_TEXT;
%ignore GroundStation::PARAMETER_TYPE;
DOWNCAST(GroundStation,GmatBase)
%include "GroundStation.hpp"

