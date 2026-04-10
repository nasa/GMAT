// Common SWIG imports for Navigation plugin

%apply double[] {double *};

ARRAYRETURN(double, Double, ErrorModel, GetEstimationParameterValue(const Integer), GetEstimationParameterSize(arg2))

%include "std_string.i"
%{
#include "NavigationAPI.hpp"
%}

VECTORCONVERT(RampTableData, RampTableData, RampTableDataVector)
VECTORCONVERT(TrackingDataAdapter*, TrackingDataAdapter, TrackingDataAdapterVector)
VECTORCONVERT(SignalData*, SignalData, SignalDataVector)
VECTORCONVERT(SignalBase*, SignalBase, SignalBaseVector)

%include "estimation_defs.hpp"
%include "MeasurementData.hpp"
DOWNCAST(MeasurementModelBase,GmatBase)
%include "MeasurementModelBase.hpp"
DOWNCAST(MeasureModel,GmatBase)
%include "MeasureModel.hpp"
%include "GmatData.hpp"
DOWNCAST(RampTableData,GmatData)
%include "RampTableData.hpp"
DOWNCAST(TrackingDataAdapter,GmatBase)
%include "TrackingDataAdapter.hpp"
DOWNCAST(RangeAdapterKm,GmatBase)
%include "RangeAdapterKm.hpp"
DOWNCAST(TrackingFileSet,GmatBase)
%include "TrackingFileSet.hpp"
DOWNCAST(Sensor,GmatBase)
%include "Sensor.hpp"
DOWNCAST(RFHardware,GmatBase)
%include "RFHardware.hpp"
DOWNCAST(Antenna,GmatBase)
%include "Antenna.hpp"
DOWNCAST(Receiver,GmatBase)
%include "Receiver.hpp"
DOWNCAST(Transmitter,GmatBase)
%include "Transmitter.hpp"
DOWNCAST(Transponder,GmatBase)
%include "Transponder.hpp"
DOWNCAST(ErrorModel,GmatBase)
%include "ErrorModel.hpp"
%include "MeasurementManager.hpp"
%include "SignalData.hpp"
DOWNCAST(SignalBase,GmatBase)
%include "SignalBase.hpp"
DOWNCAST(PhysicalSignal,GmatBase)
%include "PhysicalSignal.hpp"
