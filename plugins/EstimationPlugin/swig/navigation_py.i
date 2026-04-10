%module navigation_py
%include "PythonSwigUtil.swg"

%import "gmat_py.i"

// Ignore nested struct (Warning 325) even though they are protected?
%ignore TrackingFileSet::MeasurementDefinition;

%include "navigation.swg"
