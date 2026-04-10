# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Estimation Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/EstimationPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT Estimation Plugin setup " ${VERSION}) 

# ====================================================================
# Major error checks
if(NOT EXISTS ${F2C_DIR})
  MESSAGE(WARNING "Skipping due to missing F2C")
  RETURN()
endif()

if(NOT XercesC_FOUND)
  MESSAGE(WARNING "Skipping Estimation: Xerces missing")
  RETURN()
endif()

SET(TargetName GmatEstimation)

# Add Preproccessor Definitions
# ADD_DEFINITIONS("-DENABLE_UNFINISHED_FEATURES")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS
  adapter
  command
  datafilter
  errormodel
  estimator
  event
  factory
  hardware
  include
  measurement
  measurement/Ionosphere
  measurement/Troposphere
  measurementfile
  measurementmodel
  plugin
  propagator
  reporter
  signal
  tdmReader
  trackingfile
  )

# ====================================================================
# source files
SET(PLUGIN_SRCS
    adapter/AngleAdapterDeg.cpp
	adapter/AzimuthAdapter.cpp
    adapter/BRTSDopplerAdapter.cpp
    adapter/BRTSRangeAdapter.cpp
	adapter/DeclinationAdapter.cpp
    adapter/DopplerAdapter.cpp
    adapter/DSNPNRangeAdapter.cpp
    adapter/DSNRangeAdapter.cpp
	adapter/ElevationAdapter.cpp
    adapter/GNDopplerAdapter.cpp
    adapter/GNRangeAdapter.cpp
    adapter/GPSAdapter.cpp
    adapter/PointRangeRateAdapterKps.cpp
    adapter/RangeAdapterKm.cpp
    adapter/RangeRateAdapterKps.cpp
	adapter/RightAscAdapter.cpp
    adapter/TDRSDopplerAdapter.cpp
	adapter/TDRSDOWDAdapter.cpp
    adapter/TDRSRangeAdapter.cpp
	adapter/TDRS3LReturnDopplerAdapter.cpp
    adapter/TrackingDataAdapter.cpp
	adapter/XEastAdapter.cpp
	adapter/XSouthAdapter.cpp
	adapter/YEastAdapter.cpp
	adapter/YNorthAdapter.cpp
    adapter/RangeSkinAdapter.cpp
    command/RunEstimator.cpp
    command/RunSimulator.cpp
    datafilter/DataFilter.cpp
    datafilter/AcceptFilter.cpp
    datafilter/RejectFilter.cpp
    errormodel/ErrorModel.cpp
    estimator/BatchEstimatorBase.cpp
    estimator/BatchEstimator.cpp
    estimator/EstimationStateManager.cpp
    estimator/Estimator.cpp
    estimator/EstimatorException.cpp
    estimator/Simulator.cpp
    event/EstimationRootFinder.cpp
    event/Event.cpp
    event/EventData.cpp
    event/EventException.cpp
    event/EventManager.cpp
    factory/DataFileFactory.cpp
    factory/ErrorModelFactory.cpp
    factory/EstimationCommandFactory.cpp
    factory/EstimationDataFilterFactory.cpp
    factory/EstimatorFactory.cpp
    factory/EstimatorHardwareFactory.cpp
    factory/MeasurementModelFactory.cpp
    factory/NavPropagatorFactory.cpp
    factory/ObTypeFactory.cpp
    factory/SignalModelFactory.cpp
    hardware/Antenna.cpp
	hardware/RFHardware.cpp
    hardware/Receiver.cpp
    hardware/Sensor.cpp
    hardware/Signal.cpp
    hardware/Transmitter.cpp
    hardware/Transponder.cpp
	measurement/Ionosphere/Ionosphere.cpp
    measurement/Ionosphere/APData.cpp
    measurement/Ionosphere/MagneticField.cpp
    measurement/Ionosphere/MagneticHistory.cpp
    measurement/Ionosphere/IonosphereCoefficients.cpp
    measurement/Ionosphere/IonosphereCoefficientsFull.cpp
    measurement/MeasurementData.cpp
    measurement/MeasurementManager.cpp
    measurement/MeasurementModelBase.cpp
    measurement/MediaCorrection.cpp
    measurement/Troposphere/Troposphere.cpp
    measurementfile/B3_obtype.cpp
    measurementfile/DataFile.cpp
    measurementfile/DataFileAdapter.cpp
    measurementfile/GmatObType.cpp
    measurementfile/GmatData.cpp
    measurementfile/GmatObType.cpp
    measurementfile/ObservationData.cpp
    measurementfile/ObType.cpp
    measurementfile/RampTableData.cpp
    measurementfile/RampTableType.cpp
    measurementmodel/MeasureModel.cpp
    measurementmodel/GPSPointMeasureModel.cpp
    plugin/GmatPluginFunctions.cpp
    propagator/RungeKutta4.cpp
    reporter/ProgressReporter.cpp
    signal/PhysicalSignal.cpp
    signal/SignalBase.cpp
    signal/SignalData.cpp
    signal/SinglePointSignal.cpp
    signal/PassivePhysicalSignal.cpp
    signal/SignalDataCache.cpp
    tdmReader/TdmErrorHandler.cpp
    tdmReader/TdmObType.cpp
    tdmReader/TdmReadWriter.cpp
    trackingfile/TFSMagicNumbers.cpp
    trackingfile/TrackingFileSet.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "ESTIMATION_EXPORTS")


# ====================================================================
# Additional includes
TARGET_INCLUDE_DIRECTORIES(${TargetName} PRIVATE ${F2C_DIR})

# ====================================================================
# Additional link libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE XercesC::XercesC)
if(WIN32)
  TARGET_COMPILE_DEFINITIONS(${TargetName} PUBLIC -DXERCES_STATIC_LIBRARY)
  TARGET_LINK_LIBRARIES(${TargetName} PUBLIC Ws2_32)
endif()

# Set RPATH to find shared libraries in default locations on Mac/Linux
# Disabled since static Xerces is being used. Enable in future if GMAT
# switches to dynamic Xerces
if(UNIX AND false)
  if(APPLE)
    SET(MAC_BASEPATH "../Frameworks/") # GmatBase and Xerces in Frameworks
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/${MAC_BASEPATH}"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/;\$ORIGIN/libxerces/"
      )
  endif()
endif()

