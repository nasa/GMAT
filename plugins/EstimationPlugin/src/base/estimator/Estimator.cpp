//$Id: Estimator.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         Estimator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/08/03
//
/**
 * Implementation of the Estimator base class
 */
//------------------------------------------------------------------------------


#include "Estimator.hpp"
//#include "GmatState.hpp"
//#include "GmatGlobal.hpp"
//#include "PropagationStateManager.hpp"
#include "SolverException.hpp"
//#include "SpaceObject.hpp"
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"
//#include "Spacecraft.hpp"
#include "StateConversionUtil.hpp"
#include "GroundstationInterface.hpp"
#include "CalculationUtilities.hpp"
#include "Transponder.hpp"
#include "Transmitter.hpp"
#include "Receiver.hpp"
//#include "Antenna.hpp"
#include "ErrorModel.hpp"
#include "AcceptFilter.hpp"
#include "RejectFilter.hpp"
#include "Planet.hpp"
//#include "StateConversionUtil.hpp"
#include "StringUtil.hpp"
#include "SolarRadiationPressure.hpp"
#include "ODEModel.hpp"
#include "Propagator.hpp"
#include "GravityField.hpp"
#include "DragForce.hpp"
#include "FileManager.hpp"
#include "DataWriterInterface.hpp"

//#include <sstream>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <map>


// This is used for getting computer operating system name and version
#ifdef __linux__
#include <sys/utsname.h>
#else
#ifdef __APPLE__
#include <sys/utsname.h>
#endif
#endif

// This is used for getting hostname and user ID
#ifdef __linux__
#include <unistd.h>
#endif
#ifdef __APPLE__
#include <unistd.h>
#endif
#ifdef _MSC_VER
#include <WinSock2.h>
#endif

#include <iostream>
#include <fstream>

//#define DEBUG_CONSTRUCTION
//#define DEBUG_STATE_MACHINE
//#define DEBUG_ESTIMATOR_WRITE
//#define DEBUG_ESTIMATOR_INITIALIZATION
//#define DEBUG_INITIALIZE
//#define DEBUG_CLONED_PARAMETER UPDATES
//#define DEBUG_FILTER
//#define DEBUG_CLONED_PARAMETER UPDATES
//#define DEBUG_RESIDUAL_PLOTS
//#define DEBUG_RESIDUALS
//#define DEBUG_REPORT
//#define DEBUG_WRITE_TO_TEXT_FILE

//#define SPACECRAFT_TABLE_COLUMN_BREAK_UP             1
#define CELESTIAL_BODIES_TABLE_COLUMN_BREAK_UP       5
#define MAX_COLUMNS                                  7                 // covariance matrix column

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
Estimator::PARAMETER_TEXT[] =
{
   "Measurements",
   "AddSolveFor",
   "Propagator",
   "EstimationEpochFormat",         // The epoch of the solution
   "EstimationEpoch",               // The epoch of the solution
   "PredictTimeSpan",
   "AddPredictToMatlabFile",
   "ShowAllResiduals",
   "AddResidualsPlot",
   "DataFilters",
   "MatlabFile",
   "DataFile",
   "DataFileStyle",
};

const Gmat::ParameterType
Estimator::PARAMETER_TYPE[] =
{
   Gmat::OBJECTARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
//   Gmat::OBJECT_TYPE,
   Gmat::OBJECTARRAY_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::ON_OFF_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::FILENAME_TYPE,        // MATLAB_OUTPUT_FILENAME
   Gmat::FILENAME_TYPE,        // JSON_OUTPUT_FILENAME
   Gmat::STRING_TYPE,          // DATA_FILE_STYLE
};

const Integer Estimator::NORMAL_FLAG  = 0;      // Normal
const Integer Estimator::BLOCKED_FLAG = 1;      // Signal blocked
const Integer Estimator::U_FLAG       = 2;      // Unmatched
const Integer Estimator::RAMP_FLAG    = 4;      // Out of ramped table
const Integer Estimator::IRMS_FLAG    = 8;      // Initial RMS Sigma Editing
const Integer Estimator::OLSE_FLAG    = 16;     // Outer-Loop Sigma Editing
const Integer Estimator::USER_FLAG    = 32;     // Second level data edit
const Integer Estimator::ILSE_FLAG    = 64;     // Inner-Loop Sigma Editing
const Integer Estimator::HORP_FLAG    = 128;       // Signal blocked by HORP
const Integer Estimator::EGAP_FLAG    = 256;       // Ephemeris gap

// 730486.5 is the MATLAB datenum for J2000
const Real Estimator::MATLAB_DATE_CONVERSION = 730486.5 - GmatTimeConstants::MJD_OF_J2000;

const Real Estimator::COV_INV_TOL = std::numeric_limits<Real>::epsilon();

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Estimator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The estimator type
 * @param name The name of the new Estimator
 */
//------------------------------------------------------------------------------
Estimator::Estimator(const std::string &type, const std::string &name) :
   Solver               (type, name),
   solarSystem          (NULL),
   resetState           (false),
   timeStep             (60.0),
   transientForces      (NULL),
   needsSatPropMap      (false),
   estEpochFormat       ("FromParticipants"),
   estEpoch             ("FromParticipants"),
   estimationEpochGT      (-1.0),
   currentEpochGT         (-1.0),
   nextMeasurementEpochGT (-1.0),
   predictTimeSpan        (0.0),
   finalEpochGT           (-1.0),
   isPredicting           (false),
   addPredictToMatlab     (false),
   stm                  (NULL),
   stateCovariance      (NULL),
   estimationState      (NULL),
   stateSize            (0),
   qr                   (false),
   writeMeasurmentsAtEnd(false),
   showAllResiduals     (true),
   showSpecificResiduals(false),
   showErrorBars        (false),
   locatingEvent        (false),
   matWriter            (NULL),
   writeMatFile         (false),
   dontWriteDataInUpdate(false),
   matFileName          (""),
   writeJsonFile        (false),
   jsonFileName         (""),
   dataFileStyle        (""),
   matDeprecatedFlag    (false)
{

   objectTypeNames.push_back("Estimator");
   parameterCount = EstimatorParamCount;

   // Default value for Estimation.MaximumIterations = 15
   maxIterations = 15;

   esm.SetMeasurementManager(&measManager);
   theTimeConverter = TimeSystemConverter::Instance();

   delayInitialization = true;
}

//------------------------------------------------------------------------------
// ~Estimator()
//------------------------------------------------------------------------------
/**
 * Class destructor
 */
//------------------------------------------------------------------------------
Estimator::~Estimator()
{
   measurementNames.clear();
   modelNames.clear();
   solveForStrings.clear();   
   considerStrings.clear();
   refObjectList.clear();

   measManager.CleanUp();
   esm.CleanUp();

   removedNormalMatrixIndexes.clear();

   for (std::map<std::string, std::vector<Real *>>::iterator i = forceDerivativeCache.begin(); i != forceDerivativeCache.end();)
      forceDerivativeCache.erase(i++);
   forceDerivativeCache.clear();
   
   // clean up PropSetups
   for (UnsignedInt i = 0; i < propagators.size(); ++i)
      if (propagators[i])
         delete propagators[i];
   propagators.clear();

   // clean up std::vector<RealArray>  hTilde;
   for (Integer i = 0; i < hTilde.size(); ++i)
      hTilde[i].clear();
   hTilde.clear();
   
   modelsToAccess.clear();
   
   //// clean up Rmatrix  *stm;
   //// clean up Covariance *stateCovariance;
   //// clean up GmatState *estimationState;


   for (std::map<GmatBase*, CoordinateSystem*>::iterator i = vnbFrames.begin(); i != vnbFrames.end(); ++i)
      if (i->second)
         delete i->second;
   vnbFrames.clear();

   addedPlots.clear();
   // clean up std::vector<OwnedPlot*> residualPlots;
   for (UnsignedInt i = 0; i < residualPlots.size(); ++i)
   {
      if (residualPlots[i])
         delete residualPlots[i];
   }
   residualPlots.clear();
   
   // clean up std::vector<RealArray*> hiLowData;
   hiLowData.clear();

   // Clean up ObjectArray activeEvents;
   activeEvents.clear();
   numRemovedRecords.clear();
   
   //clean up DataWriter *matWriter;
   if (matWriter != NULL)
   {
      matWriter->CloseFile();
      matWriter->ClearData();
      // It cannot delete matWriter due to it got from an Instance() function;
      matWriter = NULL;
   }

   /////////// Data container used during accumulation
   ////////DataBucket              matData;
   /////////// Data container used for observations
   ////////DataBucket              matObsData;
   /////////// Data container for estimation config data
   ////////DataBucket              matConfigData;

   
   matIndex.clear();
   matObsIndex.clear();
   matConfigIndex.clear();

   //// clean up SolarSystem *solarSystem;
   solarSystem = NULL;

   ionoWarningList.clear();
   tropoWarningList.clear();
   dataFilterStrings.clear();

   // Clean up ObjectArray dataFilterObjs;
   for (UnsignedInt i = 0; i < dataFilterObjs.size(); ++i)
   {
      if (dataFilterObjs[i])
         delete dataFilterObjs[i];
   }
   dataFilterObjs.clear();

   if (matWriter != NULL)
     delete matWriter;
   
   editedRecords.clear();

   ///// Time converter singleton
   //TimeSystemConverter *theTimeConverter;
   theTimeConverter = NULL;

   for (Integer i = 0; i < measStats.size(); ++i)
   {
      // Clean up memory holded by measStats[i]
      measStats[i].measValue.clear();
      measStats[i].residual.clear();
      measStats[i].weight.clear();

      for (Integer j = 0; j < measStats[i].hAccum.size(); ++j)
         measStats[i].hAccum[j].clear();
      measStats[i].hAccum.clear();
   }
   measStats.clear();
   

   stationsList.clear();
   measTypesList.clear();

   textFile0.clear();
   textFile1.clear();
   textFile1_1.clear();
   textFile2.clear();
   textFile3.clear();
   textFile4.clear();
}


//------------------------------------------------------------------------------
// Estimator(const Estimator& est)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param est The estimator that is being copied
 */
//------------------------------------------------------------------------------
Estimator::Estimator(const Estimator& est) :
   Solver               (est),
   solarSystem          (est.solarSystem),
   measurementNames     (est.measurementNames),
   modelNames           (est.modelNames),
   solveForStrings      (est.solveForStrings),
   propagatorNames      (est.propagatorNames),
   propagatorSatMap     (est.propagatorSatMap),
   resetState           (false),
   timeStep             (est.timeStep),
   transientForces      (NULL),
   refObjectList        (est.refObjectList),
   estEpochFormat       (est.estEpochFormat),
   estEpoch             (est.estEpoch),
   estimationEpochGT      (est.estimationEpochGT),
   currentEpochGT         (est.currentEpochGT),
   nextMeasurementEpochGT (est.nextMeasurementEpochGT),
   predictTimeSpan      (est.predictTimeSpan),
   finalEpochGT         (est.finalEpochGT),
   isPredicting         (est.isPredicting),
   addPredictToMatlab   (est.addPredictToMatlab),
   stm                  (NULL),
   stateCovariance      (NULL),
   estimationState      (NULL),
   stateSize            (0),
   cf                   (est.cf),
   qr                   (est.qr),
   writeMeasurmentsAtEnd(est.writeMeasurmentsAtEnd),
   showAllResiduals     (est.showAllResiduals),
   showSpecificResiduals(est.showSpecificResiduals),
   showErrorBars        (est.showErrorBars),
   locatingEvent        (false),
   dataFilterStrings    (est.dataFilterStrings),
   matWriter            (NULL),
   writeMatFile         (est.writeMatFile),
   dontWriteDataInUpdate(est.dontWriteDataInUpdate),
   matFileName          (est.matFileName),
   writeJsonFile        (est.writeJsonFile),
   jsonFileName         (est.jsonFileName),
   dataFileStyle        (est.dataFileStyle)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("Estimator::Estimator() enter: <%p,%s> copy constructor from <%p,%s>\n", this, GetName().c_str(), &est, est.GetName().c_str());  
#endif

   if (est.propagators.size() > 0)
      for (UnsignedInt i = 0; i < est.propagators.size(); ++i)
         propagators.push_back((PropSetup*)(est.propagators[i]->Clone()));

   measManager = est.measManager;
   esm         = est.esm;
   addedPlots  = est.addedPlots;
   esm.SetMeasurementManager(&measManager);
   theTimeConverter = TimeSystemConverter::Instance();

   needsSatPropMap = (propagatorSatMap.size() > 0);

   delayInitialization = true;
   matDeprecatedFlag = false;

#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("Estimator::Estimator() exit: <%p,%s> copy constructor from <%p,%s>\n", this, GetName().c_str(), &est, est.GetName().c_str());  
#endif
}


//------------------------------------------------------------------------------
// Estimator& operator=(const Estimator& est)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param est The Estimator supplying the configuration data for this instance
 *
 * @return This instance, configured to match est
 */
//------------------------------------------------------------------------------
Estimator& Estimator::operator=(const Estimator& est)
{
   if (this != &est)
   {
      Solver::operator=(est);

      solarSystem = est.solarSystem;

      measurementNames = est.measurementNames;
      modelNames       = est.modelNames;
      solveForStrings  = est.solveForStrings;

      propagatorNames   = est.propagatorNames;
      propagatorSatMap  = est.propagatorSatMap;
      needsSatPropMap   = (propagatorSatMap.size() > 0);

      transientForces   = NULL;

      // clear old PropSetups
      for (UnsignedInt i = 0; i < propagators.size(); ++i)
         if (propagators[i])
            delete propagators[i];
      propagators.clear();

      if (est.propagators.size() > 0)
         for (UnsignedInt i = 0; i < est.propagators.size(); ++i)
            propagators.push_back((PropSetup*)(est.propagators[i]->Clone()));

      measManager          = est.measManager;
      esm                  = est.esm;
      esm.SetMeasurementManager(&measManager);

      estEpochFormat       = est.estEpochFormat;
      estEpoch             = est.estEpoch;
      estimationEpochGT      = est.estimationEpochGT;
      currentEpochGT         = est.currentEpochGT;
      nextMeasurementEpochGT = est.nextMeasurementEpochGT;
      predictTimeSpan      = est.predictTimeSpan;
      finalEpochGT         = est.finalEpochGT;
      isPredicting         = est.isPredicting;
      addPredictToMatlab   = est.addPredictToMatlab;
      stm                  = NULL;
      covariance           = NULL;
      estimationState      = NULL;
      stateSize            = 0;
      cf                   = est.cf;
      qr                   = est.qr;
      writeMeasurmentsAtEnd= est.writeMeasurmentsAtEnd,
      showAllResiduals     = est.showAllResiduals;
      showSpecificResiduals= est.showSpecificResiduals;
      showErrorBars        = est.showErrorBars;
      addedPlots           = est.addedPlots;
      dontWriteDataInUpdate = est.dontWriteDataInUpdate;
      locatingEvent        = false;

      dataFilterStrings    = est.dataFilterStrings;

      if (matWriter != NULL)
      {
         matWriter->CloseFile();
         matWriter->ClearData();

         // It cannot delete matWriter due to it got from Instance() function
         matWriter = NULL;
      }
      writeMatFile = est.writeMatFile;
      matFileName  = est.matFileName;

      writeJsonFile = est.writeJsonFile;
      jsonFileName = est.jsonFileName;
      dataFileStyle = est.dataFileStyle;

      matDeprecatedFlag = false;
   }

   return *this;
}


void Estimator::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the estimator - checks for unset references and does some
 * validation checking.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Estimator::Initialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Enter Estimator::Initialize()\n");
#endif

   // it the delay flag is on, skip initialization to the next time
   if (delayInitialization)
   {
      #ifdef DEBUG_INITIALIZE
         MessageInterface::ShowMessage("Exit Estimator::Initialize(): delay initialization.\n");
      #endif
      return true;
   }

   // If initialized, do not do it again
   if (isInitialized)
      return true;

   bool retval = Solver::Initialize();

   if (retval)
   {
      // Check to make sure required objects have been set
      if (propagators.size() == 0)
         throw EstimatorException(
               "Estimator error - no propagators are set for estimation or "
               "propagators are not defined in your script.\n");
      if (propagators.size() > 1)
      {
         Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
         if (runmode != GmatGlobal::TESTING)
         {
            if (IsOfType("ExtendedKalmanFilter"))
               throw EstimatorException("Estimator error.  This GMAT version does not allow use of multiple propagators with the ExtendedKalmanFilter.\n");
            else if (IsOfType("Smoother"))
               throw EstimatorException("Estimator error.  This GMAT version does not allow use of multiple propagators with the Smoother.\n");
         }
      }
      
      if (measurementNames.empty())
         throw EstimatorException("Error: no measurements are set for estimation.\n");

      // Check to see if all non-groundstation objects in the trackingFileSets have been assigned to a propagator and that all propagator objects are in the trackingFileSets
      StringArray trcNamesList;
      for (Integer ind = 0; ind < measManager.GetAllTrackingFileSets().size(); ind++)
      {
         StringArray satPropMapNames;
         if (satPropMap.size() > 0)
         {
            for (auto const& element : satPropMap) {
               satPropMapNames.push_back(element.first);
            }
         }
         else
         {
            for (auto const& element : propagatorSatMap) {
               satPropMapNames.insert(satPropMapNames.end(), element.second.begin(), element.second.end());
            }
         }

         for (Integer ind2 = 0; ind2 < measManager.GetAllTrackingFileSets()[ind]->GetRefObjectArray(Gmat::SPACE_POINT).size(); ind2++)
         {
            if (Gmat::GROUND_STATION != measManager.GetAllTrackingFileSets()[ind]->GetRefObjectArray(Gmat::SPACE_POINT)[ind2]->GetType())
            {
               trcNamesList.push_back(measManager.GetAllTrackingFileSets()[ind]->GetRefObjectArray(Gmat::SPACE_POINT)[ind2]->GetName());
               for (Integer ind3 = 0; ind3 < satPropMapNames.size(); ind3++)
               {
                  if (measManager.GetAllTrackingFileSets()[ind]->GetRefObjectArray(Gmat::SPACE_POINT)[ind2]->GetName() == satPropMapNames[ind3])
                  {
                     break;
                  }
                  else if (ind3 == satPropMapNames.size() - 1)
                  {
                     throw SolverException("Estimator error - All tracking participants must be assigned to a propagator before running an estimation with multiple propagators. Assign " + measManager.GetAllTrackingFileSets()[ind]->GetRefObjectArray(Gmat::UNKNOWN_OBJECT)[ind2]->GetName() + " to a propagator. \n");
                  }
               }
            }
         }

         if (ind == measManager.GetAllTrackingFileSets().size() - 1)
         {
            for (Integer ind2 = 0; ind2 < satPropMapNames.size(); ind2++)
            {
               for (Integer ind3 = 0; ind3 < trcNamesList.size(); ind3++)
               {
                  if (trcNamesList[ind3] == satPropMapNames[ind2])
                  {
                     break;
                  }
                  else if (ind3 == trcNamesList.size() - 1)
                  {
                     MessageInterface::ShowMessage("Estimator Warning - " + satPropMapNames[ind2] + " is assigned to a propagator, but does not appear in any tracking file sets assigned to the estimator.\n");
                  }
               }
            }
         }
      }


      if (propagators.size() == 1)
      {
         if (propagatorSatMap.size() == 0)
         {
            propagatorSatMap[propagators[0]->GetName()] = {};
            for(int k = 0; k < trcNamesList.size(); k++)
               if(find(propagatorSatMap[propagators[0]->GetName()].begin(), propagatorSatMap[propagators[0]->GetName()].end(), trcNamesList[k]) == propagatorSatMap[propagators[0]->GetName()].end())
                  propagatorSatMap[propagators[0]->GetName()].push_back(trcNamesList[k]);
         }
      }

      // comment this out for now for testing with RSSStep
      std::string propSettingError;
      for (UnsignedInt i = 0; i < propagators.size(); ++i)
      {
         ODEModel *ode = propagators[i]->GetODEModel();
         if (ode)
         {
            if (ode->GetStringParameter("ErrorControl") != "None")
            {
               propSettingError += "GMAT navigation requires use of fixed "
                  "stepped propagation. The ErrorControl parameter specified for "
                  "the ForceModel resource associated with the propagator, ";
               propSettingError += propagatorNames[i];
               propSettingError += ", used  with the ";
               propSettingError += typeName;
               propSettingError += " named ";
               propSettingError += instanceName;
               propSettingError += " must be 'None.' Of course, when using fixed step "
                     "control, the user must choose a step size, as given by the "
                     "Propagator InitialStepSize field, for the chosen orbit regime "
                     "and force profile, that yields the desired accuracy.\n";
            }

            // Ensure only one harmonic gravity model except in testing mode
            if (!GmatGlobal::Instance()->InTestingMode())
            {
               if (ode->GetForce("GravityField", 1) != NULL)
               {
                  propSettingError += "GMAT navigation requires use of at most "
                     "one spherical harmonic gravity model in numerical "
                     "propagation. The \"PrimaryBodies\" parameter specified for "
                     "the ForceModel resource associated with the propagator, ";
                  propSettingError += propagatorNames[i];
                  propSettingError += ", used  with the ";
                  propSettingError += typeName;
                  propSettingError += " named ";
                  propSettingError += instanceName;
                  propSettingError += " contains too many entries.\n";
               }
            }
         }
         // If initializing an estimator, throw error if contains TLE propagator
         if (propagators[i]->GetPropagator()->GetTypeName()=="SPICESGP4")
         {
            propSettingError += "GMAT navigation does not currently support the use of TLE propagators.\n";
         }
      }
      if (propSettingError != "")
         throw EstimatorException(propSettingError);

      // Check the names of measurement models shown in est.AddData have to be the names of created objects
      std::vector<TrackingFileSet*> tfs = measManager.GetAllTrackingFileSets();

      std::vector<PhysicalModel*> pms = esm.GetAllPhysicalModels();              // Thrust Scale Factor Solve For

     

      StringArray measNames = measManager.GetMeasurementNames();

      for(UnsignedInt i = 0; i < measNames.size(); ++i)
      {
         std::string name = measNames[i];
         bool found = false;
         for(UnsignedInt j = 0; j < tfs.size(); ++j)
         {
            if (tfs[j]->GetName() == name)
            {
               found = true;
               break;
            }
         }
		 // Thrust Scale Factor Solve For
         if (!found)
		 {
			 for (UnsignedInt j = 0; j < pms.size(); ++j)
			 {
				 if (pms[j]->GetName() == name)
				 {
					 found = true;
					 break;
				 }
			 }
		 }

         if (!found)
            throw EstimatorException("Error: Cannot initialize estimator; '" + name +
                  "' object is not defined in script.\n");
      }

      tfs.clear();
      measNames.clear();
      pms.clear();                // Thrust Scale Factor Solve For

      // Set estimation data filter objects
      ObjectMap objMap = GetConfiguredObjectMap();
      for (UnsignedInt i = 0; i < dataFilterStrings.size(); ++i)
      {
         for (UnsignedInt j = 0; j < dataFilterObjs.size(); ++j)
         {
            if (dataFilterStrings[i] == dataFilterObjs[j]->GetName())
            {
               throw EstimatorException("Error: Cannot initialize estimator '"
                  + GetName() + "';  in the estimation data filter list, estimation data filter '" 
                  + dataFilterStrings[i] + "' object is duplicated.\n");
            }
         }

         GmatBase* obj = objMap[dataFilterStrings[i]];
         if (obj == NULL)
            throw EstimatorException("Error: Cannot initialize estimator '"
            + GetName() + "'; Estimation data filter '" + dataFilterStrings[i] +
            "' object is not defined in script.\n");
         else
         {
            GmatBase* obj1 = obj->Clone();
            obj1->Initialize();
            dataFilterObjs.push_back(obj1);
         }
      }

      if (matFileName != "")
      {
         // the mat writer
         MessageInterface::ShowMessage("\nInitializing new mat data writer\n");
         matWriter = DataWriterInterface::Instance()->GetDataWriter("MatWriter");
         if (matWriter != NULL)
         {
            writeMatFile = true;
            // Add default path is there is no path data in matFileName
            if ((matFileName.find("/") == std::string::npos) &&
               (matFileName.find("\\") == std::string::npos))
            {
               FileManager *fileman = FileManager::Instance();
               std::string path = fileman->GetPathname(FileManager::OUTPUT_PATH);
               matFileName = path + matFileName;
            }

            MessageInterface::ShowMessage("MATLAB file will be written to "
               "%s\n", matFileName.c_str());

            matWriter->Initialize(matFileName, "w5");

            // Move later in the process
            matData.SetInitialRealValue(NAN);
            matObsData.SetInitialRealValue(NAN);
            matConfigData.SetInitialRealValue(NAN);
            matData.Clear();
            matObsData.Clear();
            matConfigData.Clear();
         }
      }

      if (jsonFileName != "")
         writeJsonFile = true;

      // Get time range of EOP file
      EopFile* eop = GmatGlobal::Instance()->GetEopFile();

      // Initializes the EOP file in case it has not yet been by other objects
      eop->Initialize();

      // Get EOP time range
      eop->GetTimeRange(eopTimeMin, eopTimeMax);

      // Set scientific notation formatter
      formatter << std::scientific;
      formatter.precision(15);
   }

   matDeprecatedFlag = false;

#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Exit Estimator::Initialize()   retval = %s\n", (retval?"true":"false"));
#endif

   return retval;
}


bool Estimator::Reinitialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Enter Estimator::Reinitialize()\n");
#endif

   // Should match propagators with their respective names
   for (const std::string& pName : propagatorNames)
   {
      if (std::none_of(propagators.begin(), propagators.end(),
         [&pName](const PropSetup* pSetup) { return pName.compare(pSetup->GetName()) == 0; }))
      {
         throw EstimatorException(
            "Estimator::CompleteInitialization - Propagator with name '"+ pName +"' does not exist.\n");
      }
   
   }
   
   // Tell the measManager to complete its initialization
   bool measOK = measManager.SetPropagators(&propagators, &propagatorSatMap);
   measOK = measOK && measManager.Initialize();
   if (!measOK)
      // Note that this could be the Batch estimator or the EKF
      throw EstimatorException(
        "Estimator::CompleteInitialization - error initializing "
         "MeasurementManager.\n");

   //@todo: auto generate tracking data adapters from observation data
   //1. Read observation data from files and create a list of all tracking configs
   UnsignedInt numRec = measManager.LoadObservations();
   if (numRec == 0)
      throw EstimatorException("No observation data is used for estimation\n");

   //2. Generate tracking data adapters based on the list of tracking configs
   measManager.AutoGenerateTrackingDataAdapters();

   /* Code from GSFC
      // Tell the measManager to complete its initialization
      bool measOK = measManager.SetPropagator(propagator);
      measOK = measOK && measManager.Initialize();
      if (!measOK)
         // Note that this could be the Batch estimator or the EKF
         throw EstimatorException(
           "Estimator::CompleteInitialization - error initializing "
            "MeasurementManager.\n");
      
      //@todo: auto generate tracking data adapters from observation data
      //1. Read observation data from files and create a list of all tracking configs
      UnsignedInt numRec = measManager.LoadObservations();
      if (numRec == 0)
         throw EstimatorException("No observation data is used for estimation\n");

      //2. Generate tracking data adapters based on the list of tracking configs
      measManager.AutoGenerateTrackingDataAdapters();
   */ 

   editedRecords.resize(numRec, 0);
   
   measManager.SetPassBiasesToTrackingDataAdapters();

#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Exit Estimator::Reinitialize()\n");
#endif
   return true;
}


void Estimator::SetDelayInitialization(bool delay)
{
   delayInitialization = delay;
}


//------------------------------------------------------------------------------
// void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * Completes the initialization process.
 *
 * This method is called in the INITIALIZING state of the estimator's finite
 * state machine.
 */
//------------------------------------------------------------------------------
void Estimator::CompleteInitialization()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Enter Estimator::CompleteInitialization()\n");
#endif

   if (esm.IsPropertiesSetupCorrect())       // Verify solve-for parameters setting up correctly 
   {
      stm = esm.GetSTM();
      stateCovariance = esm.GetCovariance();
   }

   // Get list of signal paths and specify the length of participants' column
   pcolumnLen = 29; // Long enough so measurements fill entire file width
   if (IsIterative())
      pcolumnLen -= 5;

   for (int i = 0; i < propagators.size(); i++)
   {
      if (propagators[i]->GetPropagator()->IsOfType("Code500Propagator"))
         throw EstimatorException("Estimator error - Code500 propagators are not supported in the estimator.\n");

      //Do not allow spacecraft using ephempropagators to be estimated unless in RunMode = InitialGuess.
      if (currentMode != INITIAL_GUESS)
      {
         if (propagators[i]->GetPropagator()->IsOfType("EphemerisPropagator"))
         {
            if (propagatorSatMap.size() > 0)
            {
               StringArray scNames = propagatorSatMap[propagators[i]->GetName()];
               for (int j = 0; j < scNames.size(); j++)
               {
                  StringArray solveForObjectNames = esm.GetObjectList("SolveFor");
                  if (std::count(solveForObjectNames.begin(), solveForObjectNames.end(), scNames[j]))
                  {
                     StringArray slvFors = esm.GetObjectSolveFors(scNames[i]);
                     std::string slvForsStr = "{";
                     if (slvFors.size() > 0)
                     {
                        slvForsStr += slvFors[0];
                        for (int k = 1; k < slvFors.size(); k++)
                           slvForsStr += ", " + slvFors[k];
                     }
                     slvForsStr += "}";

                     throw EstimatorException("Estimator error - Spacecraft " + scNames[i] + " is using an ephemeris propagator, estimation of "
                        + slvForsStr +
                        " is not possible. If this is intended, please ensure SolveMode = RunInitialGuess is set for the estimator.");
                  }
               }
            }
         }
      }
   }
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Exit Estimator::CompleteInitialization()\n");
#endif
}


//------------------------------------------------------------------------------
//  bool Finalize()
//------------------------------------------------------------------------------
/**
 * Finalizes the estimator.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Estimator::Finalize()
{
   bool retval = Solver::Finalize();

   // Remove all estimation data filters in finalized stage
   for (UnsignedInt i = 0; i < dataFilterObjs.size(); ++i)
   {
      if (dataFilterObjs[i])
         delete dataFilterObjs[i];
   }
   dataFilterObjs.clear();

   // clear all estimation flags
   editedRecords.clear();

   return retval;
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Estimator::GetParameterText(const Integer id) const
{
   if (id >= SolverParamCount && id < EstimatorParamCount)
      return PARAMETER_TEXT[id - SolverParamCount];
   return Solver::GetParameterText(id);
}

//---------------------------------------------------------------------------
//  std::string GetParameterUnit(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the unit for the parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return unit for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Estimator::GetParameterUnit(const Integer id) const
{
   return Solver::GetParameterUnit(id); // TBD
}

//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer     Estimator::GetParameterID(const std::string &str) const
{
   if (str == "MatlabFile" && !matDeprecatedFlag)
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** \"MatlabFile\" field of Estimator "
          "is deprecated. Please use the \"DataFile\" option instead.\n");

      matDeprecatedFlag = true;
   }

   for (Integer i = SolverParamCount; i < EstimatorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SolverParamCount])
         return i;
   }

   return Solver::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Estimator::GetParameterType(const Integer id) const
{
   if (id >= SolverParamCount && id < EstimatorParamCount)
      return PARAMETER_TYPE[id - SolverParamCount];

   return Solver::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Estimator::GetParameterTypeString(const Integer id) const
{
   return Solver::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
 //---------------------------------------------------------------------------
bool Estimator::IsParameterReadOnly(const Integer id) const
{
   if (id == PREDICT_TIME_SPAN)
      return true;

   if (id == ADD_PREDICT_TO_MATLAB)
      return true;

   if (id == DATA_FILE_STYLE)
      return true;

   return Solver::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
 //---------------------------------------------------------------------------
bool Estimator::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param id ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Estimator::GetRealParameter(const Integer id) const
{
   if (id == PREDICT_TIME_SPAN)
      return predictTimeSpan;

   return Solver::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param id ID for the parameter whose value to change.
 * @param value Value for the parameter.
 *
 * @return  Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Estimator::SetRealParameter(const Integer id, const Real value)
{
   if (id == PREDICT_TIME_SPAN)
   {
      if (value >= 0.0)
         predictTimeSpan = value;
      else
         throw EstimatorException("Error: " + GetName() + "." + GetParameterText(id) + " cannot be negative\n");

      return predictTimeSpan;
   }

   return Solver::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
//  Integer GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* This method gets value of an real parameter specified by parameter name.
*
* @param label    name of parameter.
*
* @return         value of a real parameter.
*/
//------------------------------------------------------------------------------
Real Estimator::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Integer SetRealParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
* This method sets value to a real parameter specified by the input parameter name.
*
* @param label    name for the requested parameter.
* @param value    real value used to set to the request parameter.
*
* @return value set to the requested parameter.
*/
//------------------------------------------------------------------------------
Real Estimator::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Estimator::GetStringParameter(const Integer id) const
{
//   if (id == PROPAGATOR)
//      return propagatorName;

   if (id == ESTIMATION_EPOCH_FORMAT)
      return estEpochFormat;

   if (id == ESTIMATION_EPOCH)
      return estEpoch;

   if (id == MATLAB_OUTPUT_FILENAME)
      return matFileName;

   if (id == JSON_OUTPUT_FILENAME)
      return jsonFileName;

   if (id == DATA_FILE_STYLE)
      return dataFileStyle;

   return Solver::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value stored in a StringArray, given
 * the input parameter ID and location in the array.
 *
 * @param id ID for the requested parameter.
 * @param index Location of the requested parameter in the array.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Estimator::GetStringParameter(const Integer id,
                                          const Integer index) const
{
   if (id == MEASUREMENTS)
   {
      if (((Integer)measurementNames.size() > index) && (index >= 0))
         return measurementNames[index];
      else
         throw EstimatorException("Index out of bounds when trying to access "
               "a measurement");
      
      //return measurementNames.at(index);
   }

   if (id == SOLVEFORS)
   {
      if (((Integer)solveForStrings.size() > index) && (index >= 0))
         return solveForStrings[index];
      else
         throw EstimatorException("Index out of bounds when trying to access "
               "a solve-for");

      //return solveForStrings.at(index);
   }

   if (id == ADD_RESIDUAL_PLOT)
   {
      if (((Integer)addedPlots.size() > index) && (index >= 0))
         return addedPlots[index];
      else
         throw EstimatorException("Index out of bounds when trying to access "
               "a plot");

      //return addedPlots.at(index);
   }

   if (id == DATA_FILTERS)
   {
      if (((Integer)dataFilterStrings.size() > index) && (index >= 0))
         return dataFilterStrings[index];
      else
         throw EstimatorException("Index out of bounds when trying to access "
         "a estimation data filters");
   }

   return Solver::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool Estimator::SetStringParameter(const Integer id,
                                   const std::string &value) // const?
{
   if (id == PROPAGATOR)
   {
      if (!GmatStringUtil::IsValidIdentity(value))
         throw EstimatorException("Error: '" + value + "' set to " +
               instanceName + ".Propagator is an invalid GMAT object name.\n");

      if (propagatorNames.size() > 0)
      {
//         if (propagatorNames[0] != value)
//            // Warn if resetting default propagator
//            MessageInterface::ShowMessage("Resetting default propagator from "
//                  "%s to %s\n", propagatorNames[0].c_str(), value.c_str());
         propagatorNames[0] = value;
      }
      else
         propagatorNames.push_back(value);

      currentPropagator = value;
      return true;
   }

   else if (id == ESTIMATION_EPOCH_FORMAT)
   {
      if (value != "FromParticipants")
         throw EstimatorException("Error: An invalid value (" + value + ") was set to " + GetName() + ".EstimationEpochFormat parameter. In current GMAT version, only 'FromParticipants' is a valid value.\n");

      bool retVal = false;
      StringArray sa = GetPropertyEnumStrings(id);
      for (UnsignedInt i=0; i < sa.size(); ++i)
      {
         if (value == sa[i])
         {
            estEpochFormat = value;
            retVal = true;
            break;
         }
      }

      if (value == "FromParticipants")
      {
         estimationEpochGT = 0.0;
         estEpoch = "";
      }

      return retVal;
   }

   else if (id == ESTIMATION_EPOCH)
   {
      if (value != "FromParticipants")
         throw EstimatorException("Error: An invalid value (" + value + ") was set to " + GetName() + ".EstimationEpoch parameter. In current GMAT version, only 'FromParticipants' is a valid value.\n");

      if (value == "")
         throw EstimatorException("Error: No value was set to " + GetName() + ".EstimationEpoch parameter.\n");

      if (estEpochFormat == "FromParticipants")
      {
         MessageInterface::ShowMessage("Setting value for %s.EstimationEpoch has no "
               "effect due to %s.EstimationEpochFormat to be \"%s\"\n", 
               GetName().c_str(), GetName().c_str(), estEpochFormat.c_str());
      }
      else
      {
         estEpoch = value;
         if (theTimeConverter->IsValidTimeSystem(estEpochFormat))
         {
            // Convert to a.1 time for internal processing
            estimationEpochGT = ConvertToGmatTimeEpoch(estEpoch, estEpochFormat);
         }
         else
            throw EstimatorException("Error: Cannot set value '" + value + " to " + GetName() + ".EstimationEpoch parameter due to its invalid time format.\n");
      }

      return true;
   }

   else if (id == MEASUREMENTS)
   {
      std::string measName = GmatStringUtil::Trim(GmatStringUtil::RemoveOuterString(value, "{", "}"));
      if (measName == "")
         throw EstimatorException("Error: No measurement is set to " + GetName() + ".Measurements parameter.\n");

      if (!GmatStringUtil::IsValidIdentity(value))
         throw EstimatorException("Error: '" + value + "' set to " + GetName() + ".Measurements is an invalid GMAT object name.\n");

      return SetStringParameter(id, measName, measurementNames.size());
   }


   else if (id == ReportStyle)
   {
      Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
      if (runmode != GmatGlobal::TESTING)
      {
         Integer i = NORMAL_STYLE;
         if (value == STYLE_TEXT[i - NORMAL_STYLE])
         {
            textFileMode = value;
            progressStyle = i;
            return true;
         }
         throw SolverException(
            "The value of \"" + value + "\" for field \"Report Style\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Normal].\n");
      }
   }

   else if (id == DATA_FILTERS)
   {
      //MessageInterface::ShowMessage("%s.DataFilters = %s\n", GetName().c_str(), value.c_str());
      if (GmatStringUtil::Trim(GmatStringUtil::RemoveOuterString(value, "{", "}")) == "")
      {
         dataFilterStrings.clear();
         return true;
      }
   }

   else if (id == MATLAB_OUTPUT_FILENAME)
   {
      // verify a valid file name
      Integer error;
      if (!GmatStringUtil::IsValidFullFileName(value, error))
         throw EstimatorException("Error: '" + value + "' set to " + GetName() + ".MatlabFile is an invalid file name.\n");

      matFileName = value;
      if (matFileName.find(".mat") == std::string::npos)
         matFileName += ".mat";
      return true;
   }

   else if (id == JSON_OUTPUT_FILENAME)
   {
      //verify a valid file name
      Integer error;
      if (!GmatStringUtil::IsValidFullFileName(value, error))
         throw EstimatorException("Error: '" + value + "' set to " +
               GetName() + ".JsonFile is an invalid file name.\n");

      jsonFileName = value;
      //if (jsonFileName.find(".json") == std::string::npos)
      //   jsonFileName += ".json";

      return true;
   }

   else if (id == DATA_FILE_STYLE)
   {
      //verify a valid style
      if (GmatStringUtil::ToLower(value) == "normal")
      {
         dataFileStyle = "Normal";
         return true;
      }
      else if (GmatStringUtil::ToLower(value) == "verbose")
      {
         dataFileStyle = "Verbose";
         return true;
      }
      throw SolverException(
         "The value of \"" + value + "\" for field \"Data File Style\""
         " on object \"" + instanceName + "\" is not an allowed value.\n"
         "The allowed values are: [Normal, Verbose].\n");
   }

   return Solver::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value,
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID and the index.
 *
 * @param id ID for the requested parameter.
 * @param value string value for the requested parameter.
 * @param index index into the StringArray.
 *
 * @exception <EstimatorException> thrown if value is out of range
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool Estimator::SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index)
{
   if (id == PROPAGATOR)
   {
      bool retval = false;

      #ifdef DEBUG_PROPAGATORS
         MessageInterface::ShowMessage("Propagator setting on %s for index %d "
               "to %s\n", instanceName.c_str(), index, value.c_str());
      #endif

//      if ((index == 0) && (propagatorName == ""))
//         retval = SetStringParameter(id, value);

      if (!GmatStringUtil::IsValidIdentity(value))
         throw EstimatorException("Error: '" + value + "' set to " +
               instanceName + ".Propagator is an invalid GMAT object name.\n");

      // Start the name mapping
      if (index == 0)
      {
         currentPropagator = value;

         if (find(propagatorNames.begin(), propagatorNames.end(), value) == propagatorNames.end())
            propagatorNames.push_back(value);

         if (propagatorSatMap.find(value) == propagatorSatMap.end())
         {
            StringArray satList;
            propagatorSatMap[currentPropagator] = satList;
         }
         retval = true;
      }
      else
      {
         StringArray theSats = propagatorSatMap[currentPropagator];
         if (find(theSats.begin(), theSats.end(), value) == theSats.end())
         {
            if (find(propagatorSatMap[currentPropagator].begin(), propagatorSatMap[currentPropagator].end(), value) == propagatorSatMap[currentPropagator].end())
            {
               propagatorSatMap[currentPropagator].push_back(value);
               retval = true;
            }
         }
         else
         {
            MessageInterface::ShowMessage("%s is already in the sat list for "
                  "%s\n", value.c_str(), currentPropagator.c_str());
         }
      }

      if (retval)
         needsSatPropMap = true;

      return retval;
   }
   if (id == MEASUREMENTS)
   {
      // an empty list is set to Measurements parameter when index == -1
      if (index == -1)
      {
         measurementNames.clear();
         return true;
      }

      // Verify measurement name
      if (!GmatStringUtil::IsValidIdentity(value))
         throw EstimatorException("Error: '" + value + "' set to " + GetName() + ".Measurements is an invalid GMAT object name.\n");
      
      Integer sz = (Integer) measurementNames.size();
      if (index == sz) // needs to be added to the end of the list
      {
         measurementNames.push_back(value);
         measManager.AddMeasurementName(value);
      }
      else if ((index) < 0 || (index > sz)) // out of bounds
      {
         std::string errmsg = "Estimator::SetStringParameter error - index "
               "into measurement array is out of bounds.\n";
         throw EstimatorException(errmsg);
      }
      else // is in bounds
      {
         measurementNames.at(index) = value;
         measManager.AddMeasurementName(value);
      }
      return true;
   }

   if (id == SOLVEFORS)
   {
      // an empty list is set to SolveFors parameter when index == -1
      if (index == -1)
      {
         measurementNames.clear();
         return true;
      }

      Integer sz = (Integer) solveForStrings.size();
      if (index == sz) // needs to be added to the end of the list
         solveForStrings.push_back(value);
      else if ((index) < 0 || (index > sz)) // out of bounds
      {
         std::string errmsg = "Estimator::SetStringParameter error - index "
               "into measurement array is out of bounds.\n";
         throw EstimatorException(errmsg);
      }
      else // is in bounds
         solveForStrings.at(index) = value;

      // Load the string into the ESM so that the object list can be built
      esm.SetProperty(value, index);

      return true;
   }

   if (id == ADD_RESIDUAL_PLOT)
   {
      // Nothing to do when an empty list is added to AddResualPlot parameter
      if (index == -1)
         return true;

      Integer sz = (Integer)addedPlots.size();
      if (index == sz) // needs to be added to the end of the list
         addedPlots.push_back(value);
      else if ((index) < 0 || (index > sz)) // out of bounds
      {
         std::string errmsg = "Estimator::SetStringParameter error - index "
               "into residual plot array is out of bounds.\n";
         throw EstimatorException(errmsg);
      }
      else // is in bounds
         addedPlots.at(index) = value;

      return true;
   }

   if (id == DATA_FILTERS)
   {
      // Nothing to do when an empty list is added to DataFilters parameter
      if (index == -1)
         return true;

      Integer sz = (Integer)dataFilterStrings.size();
      if (index == sz) // needs to be added to the end of the list
         dataFilterStrings.push_back(value);
      else if ((index) < 0 || (index > sz)) // out of bounds
      {
         std::string errmsg = "Estimator::SetStringParameter error - index "
            "into data filter array is out of bounds.\n";
         throw EstimatorException(errmsg);
      }
      else // is in bounds
         dataFilterStrings.at(index) = value;

      return true;
   }

   return Solver::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param label The text label for the parameter
 *
 * @return The string assigned to the parameter
 */
 //------------------------------------------------------------------------------
std::string Estimator::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a parameter
 *
 * @param label The text label for the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
bool Estimator::SetStringParameter(const std::string &label,
   const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param label The text label for the parameter
 * @param index The index into the StringArray
 *
 * @return The value
 */
 //------------------------------------------------------------------------------
std::string Estimator::GetStringParameter(const std::string &label,
   const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter in a StringArray
 *
 * @param label The text label for the parameter
 * @param value The new value
 * @param index The index into the StringArray
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
bool Estimator::SetStringParameter(const std::string &label,
   const std::string &value,
   const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter((const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& Estimator::GetStringArrayParameter(const Integer id) const
{
   if (id == PROPAGATOR)
      return propagatorNames;

   if (id == MEASUREMENTS)
      return measurementNames; // temporary

   if (id == SOLVEFORS)
      return solveForStrings;

   if (id == ADD_RESIDUAL_PLOT)
      return addedPlots;

   if (id == DATA_FILTERS)
      return dataFilterStrings;

   return Solver::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter((const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter ID.
 *
 * @param label The string used to script the parameter
 *
 * @return  StringArray value of the requested parameter.
 *
 */
 //------------------------------------------------------------------------------
const StringArray& Estimator::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// std::string GetOnOffParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves boolean parameters that are scripted using the words On and Off
 *
 * @param id The ID of the parameter
 *
 * @return the word "On" or "Off"
 */
//------------------------------------------------------------------------------
std::string Estimator::GetOnOffParameter(const Integer id) const
{
   if (id == SHOW_RESIDUALS)
      return (showAllResiduals ? "On" : "Off");

   return Solver::GetOnOffParameter(id);
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets boolean parameters that are scripted using the words On and Off
 *
 * @param id The ID of the parameter
 * @param value The new setting, "On" or "Off"
 *
 * @return true on success, false if the parameter was not set
 */
//------------------------------------------------------------------------------
bool Estimator::SetOnOffParameter(const Integer id, const std::string &value)
{
   if (id == SHOW_RESIDUALS)
   {
      if (value == "On")
      {
         showAllResiduals = true;
         return true;
      }
      else if (value == "Off")
      {
         showAllResiduals = false;
         return true;
      }

      return false;
   }

   return Solver::SetOnOffParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetOnOffParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves boolean parameters that are scripted using the words On and Off
 *
 * @param label The string used to script the parameter
 *
 * @return The word "On" or "Off"
 */
//------------------------------------------------------------------------------
std::string Estimator::GetOnOffParameter(const std::string &label) const
{
   return GetOnOffParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetOnOffParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets boolean parameters that are scripted using the words On and Off
 *
 * @param label The string used to script the parameter
 * @param value The new setting, "On" or "Off"
 *
 * @return true on success, false if the parameter was not set
 */
//------------------------------------------------------------------------------
bool Estimator::SetOnOffParameter(const std::string &label,
      const std::string &value)
{
   return SetOnOffParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  bool  GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Boolean parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Boolean value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
bool Estimator::GetBooleanParameter(const Integer id) const
{
   if (id == ADD_PREDICT_TO_MATLAB)
       return addPredictToMatlab;

   return Solver::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
//  Integer SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets a Boolean parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> Boolean value for the parameter.
 *
 * @return  The value of the parameter at the completion of the call.
 */
//------------------------------------------------------------------------------
bool Estimator::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == ADD_PREDICT_TO_MATLAB)
   {
      addPredictToMatlab = value;
      return addPredictToMatlab;
   }

   return Solver::SetBooleanParameter(id, value);
}


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
 //---------------------------------------------------------------------------
bool Estimator::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const char *value)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 */
 //---------------------------------------------------------------------------
bool Estimator::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the list of allowable settings for the enumerated parameters
 *
 * @param id The ID of the parameter
 *
 * @return A const string array with the allowed settings.
 */
//------------------------------------------------------------------------------
const StringArray& Estimator::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   enumStrings.clear();

   if (id == ESTIMATION_EPOCH_FORMAT)
   {
      enumStrings.push_back("FromParticipants");

      StringArray nameList = theTimeConverter->GetValidTimeRepresentations();
      for (UnsignedInt i = 0; i < nameList.size(); ++i)
         enumStrings.push_back(nameList[i]);

      return enumStrings;
   }
   return Solver::GetPropertyEnumStrings(id);

}


//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param id ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
UnsignedInt Estimator::GetPropertyObjectType(const Integer id) const
{
   if (id == MEASUREMENTS)
      return Gmat::MEASUREMENT_MODEL;

   if (id == PROPAGATOR)
      return Gmat::PROP_SETUP;

   return Solver::GetPropertyObjectType(id);
}


//------------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id and index.
 *
 * @param id ID for the parameter.
 * @param id Array index for the parameter.
 *
 * @return parameter ObjectType
 */
//------------------------------------------------------------------------------
UnsignedInt Estimator::GetPropertyObjectType(const Integer id,
      const Integer index) const
{
   if (id == PROPAGATOR)
   {
      if (index == 0)
         return Gmat::PROP_SETUP;
      else
         return Gmat::SPACECRAFT;
   }

   return GetPropertyObjectType(id);
}

//------------------------------------------------------------------------------
// void UpdateCurrentEpoch(GmatEpoch newEpoch)
//------------------------------------------------------------------------------
/**
 * Sets the current epoch to a new value
 *
 * @param newEpoch The new epoch
 */
//------------------------------------------------------------------------------
void Estimator::UpdateCurrentEpoch(GmatTime newEpoch)
{
   currentEpochGT = newEpoch;
}


//------------------------------------------------------------------------------
// GmatEpoch GetCurrentEpoch()
//------------------------------------------------------------------------------
/**
 * Retrieves the current epoch
 *
 * @return the current a.1 modified Julian epoch
 */
//------------------------------------------------------------------------------
GmatTime Estimator::GetCurrentEpoch()
{
   return currentEpochGT;
}


//------------------------------------------------------------------------------
// void BeginPredicting(Real predictSpan)
//------------------------------------------------------------------------------
/**
 * Configure the estimator to begin predicting
 *
 * @param predictSpan How long to predict in seconds
 */
 //------------------------------------------------------------------------------
void Estimator::BeginPredicting(Real predictSpan)
{
   isPredicting = true;

   finalEpochGT = currentEpochGT;
   if (measManager.IsForward())
      finalEpochGT.AddSeconds(predictSpan);
   else
      finalEpochGT.SubtractSeconds(predictSpan);

   currentState = PROPAGATING;
}


//------------------------------------------------------------------------------
// UnsignedInt TrimObsByEpoch(const GmatTime& epoch, bool removeObsAtEpoch)
//------------------------------------------------------------------------------
/**
 * Removes observations from the measManager that are prior to the input epoch
 *
 * @param epoch The epoch to remove observations prior to
 * @param removeObsAtEpoch True to also remove observations that are at the input epoch,
 *                         False to keep observations that are at the input epoch
 *
 * @return The number of observations that were removed
 */
 //------------------------------------------------------------------------------
UnsignedInt Estimator::TrimObsByEpoch(const GmatTime& epoch, bool removeObsAtEpoch)
{
   // Check if first observation is before estimation epoch
   UnsignedInt obsIdx = 0U;
   UnsignedInt numObsRemoved = 0U;

   while (obsIdx < measManager.GetMeasurementSize())
   {
      bool obsIsBefore;
      GmatTime measEpochGT = measManager.GetObsData(obsIdx)->epochGT;

      Real epochDiff = (measEpochGT - epoch).GetMjd();
      obsIsBefore = epochDiff < -ESTTIME_ROUNDOFF;

      if (removeObsAtEpoch)
         obsIsBefore = obsIsBefore || GmatMathUtil::Abs(epochDiff) <= ESTTIME_ROUNDOFF;

      if (obsIsBefore)
      {
         measManager.RemoveObservation(obsIdx);
         numObsRemoved++;
      }
      else
      {
         obsIdx++;
      }
   }

   return numObsRemoved;
}


//------------------------------------------------------------------------------
// bool UpdateInitialConditions()
//------------------------------------------------------------------------------
/**
 * Check if the initial conditions of the estimator need to be updated,
 * and update them if needed. By default, this method does nothing.
 *
 * @return true if the initial conditions were changed, false otherwise
 */
 //------------------------------------------------------------------------------
bool Estimator::UpdateInitialConditions()
{
   return false;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//------------------------------------------------------------------------------
/**
 * Renames references objects
 *
 * @param type The type of object that is renamed
 * @param oldName The name of the object that is changing
 * @param newName the new object name
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Estimator::RenameRefObject(const UnsignedInt type,
      const std::string & oldName, const std::string & newName)
{
   /// @todo Estimator rename code needs to be implemented
   return Solver::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object's name
 *
 * @param type The type of the object
 * @param name The name of the object
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Estimator::SetRefObjectName(const UnsignedInt type,
      const std::string & name)
{
   return Solver::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * This method retrieves am array of reference object types
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Estimator::GetRefObjectTypeArray()
{
   return Solver::GetRefObjectTypeArray();
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Initialization method that identifies the reference objects needed
 *
 * @param type The ObjectType for the references; UNKNOWN_OBJECT retrieves all
 *
 * @return A StringArray with all of the object names.
 */
//------------------------------------------------------------------------------
const StringArray& Estimator::GetRefObjectNameArray(const UnsignedInt type)
{
   #ifdef DEBUG_ESTIMATOR_INITIALIZATION
      MessageInterface::ShowMessage(
            "Estimator::GetRefObjectNameArray(%d) entered\n", type);
   #endif

   refObjectList.clear();

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::PROP_SETUP) ||
       (type == Gmat::MEASUREMENT_MODEL))
   {
      if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::PROP_SETUP))
      {
         #ifdef DEBUG_ESTIMATOR_INITIALIZATION
            MessageInterface::ShowMessage("   Adding propagators:\n")
            for (UnsignedInt i = 0; i < propagatorNames.size(); ++i)
            {
               MessageInterface::ShowMessage("      %s\n",
                     propagatorNames[i].c_str());
            }
         #endif

         for (UnsignedInt i = 0; i < propagatorNames.size(); ++i)
         {
            if (find(refObjectList.begin(), refObjectList.end(),
                  propagatorNames[i]) == refObjectList.end())
               refObjectList.push_back(propagatorNames[i]);
         }
      }

      if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::MEASUREMENT_MODEL))
      {
         // Add the measurements this Estimator needs

         for (StringArray::iterator i = measurementNames.begin();
               i != measurementNames.end(); ++i)
         {
            #ifdef DEBUG_ESTIMATOR_INITIALIZATION
               MessageInterface::ShowMessage(
                     "   Adding measurement: %s\n", i->c_str());
            #endif
            if (find(refObjectList.begin(), refObjectList.end(), *i) ==
                  refObjectList.end())
               refObjectList.push_back(*i);
         }
      }
   }
   else
   {
      // Fill in any base class needs
      refObjectList = Solver::GetRefObjectNameArray(type);
   }

   return refObjectList;
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a referenced object of a given type
 *
 * @param type The object's type
 *
 * @return The name of the associated object
 */
//------------------------------------------------------------------------------
std::string Estimator::GetRefObjectName(const UnsignedInt type) const
{
   return Solver::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a referenced object of a given type and name
 *
 * @param type The object's type
 * @param name The object's name
 *
 * @return The pointer to the associated object
 */
//------------------------------------------------------------------------------
GmatBase* Estimator::GetRefObject(const UnsignedInt type,
      const std::string & name)
{
   if (type == Gmat::PROP_SETUP)
   {
      for (UnsignedInt i = 0; i < propagators.size(); ++i)
      {
         if (propagators[i]->GetName() == name)
            return propagators[i];
      }
   }

   return Solver::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string & name,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a referenced object of a given type and name from an
 * array of reference objects
 *
 * @param type The object's type
 * @param name The object's name
 * @param index The index to the object
 *
 * @return The pointer to the associated object
 */
//------------------------------------------------------------------------------
GmatBase* Estimator::GetRefObject(const UnsignedInt type,
      const std::string & name, const Integer index)
{
   return Solver::GetRefObject(type, name, index);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string & name)
//------------------------------------------------------------------------------
/**
 * Sets a pointer to a referenced object of a given type and name
 *
 * @param obj  The object
 * @param type The object's type
 * @param name The object's name
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Estimator::SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string & name)
{
   #ifdef DEBUG_ESTIMATOR_INITIALIZATION
      MessageInterface::ShowMessage("Setting ref object %s with type %s\n",
            name.c_str(), obj->GetTypeName().c_str());
   #endif

   for (UnsignedInt i = 0; i < propagatorNames.size(); ++i)
   {
      if (name == propagatorNames[i])
      {
         if (type == Gmat::PROP_SETUP)
         {
            PropSetup *propagator = (PropSetup*)obj->Clone();
            if (propagators.size() <= i)
               propagators.push_back(propagator);
            else
            {
               PropSetup *oldProp = propagators[i];
               propagators[i] = propagator;
               delete oldProp;
            }

            // Set the spacecraft - propagator name map
            if (propagatorSatMap.find(name) != propagatorSatMap.end())
            {
               StringArray propSats = propagatorSatMap[name];
               for (UnsignedInt i = 0; i < propSats.size(); ++i)
                  satPropMap[propSats[i]] = propagator;
            }

            measManager.SetPropagators(&propagators, &propagatorSatMap);
            return true;
         }
      }
   }

   StringArray measList = measManager.GetMeasurementNames();

   if (find(measList.begin(), measList.end(), name) != measList.end())
   {
      if (obj->IsOfType(Gmat::MEASUREMENT_MODEL))
      {
         // Handle MeasurmentModel and TrackingFileSet
         #ifdef DEBUG_ESTIMATOR_INITIALIZATION
            MessageInterface::ShowMessage("Estimator::SetRefObject Handle MeasurementModel and TrackingFileSet: <%s>\n", name.c_str());
         #endif

         modelNames.push_back(obj->GetName());
         measManager.AddMeasurement((TrackingFileSet *)obj);
         return true;
      }
   }

   return Solver::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const std::string & typeString)
//------------------------------------------------------------------------------
/**
 * This method retrieves an array of reference objects of a given type
 *
 * @param typeString The type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray& Estimator::GetRefObjectArray(const std::string & typeString)
{
   return GetRefObjectArray(GetObjectType(typeString));
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a pointer to a referenced object of a given type and name in an array of
 * objects of that type
 *
 * @param obj  The object
 * @param type The object's type
 * @param name The object's name
 * @param index The index into the object array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Estimator::SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string & name, const Integer index)
{
   #ifdef DEBUG_ESTIMATOR_INITIALIZATION
      MessageInterface::ShowMessage(""
            "Setting indexed ref object %s with type %s\n", name.c_str(),
            obj->GetTypeName().c_str());
   #endif

   return Solver::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// ObjectArray & GetRefObjectArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * This method retrieves an array of reference objects of a given type
 *
 * @param type The type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray & Estimator::GetRefObjectArray(const UnsignedInt type)
{
   if (type == Gmat::EVENT)
   {
      activeEvents.clear();
      // Get all active events from the measurements
      activeEvents = measManager.GetActiveEvents();
      return activeEvents;
   }

   return Solver::GetRefObjectArray(type);
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string & action, const std::string & actionData)
//------------------------------------------------------------------------------
/**
 * This method performs a custom action
 *
 * @param action The string defining the action
 * @param actionData Data associated with that action
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Estimator::TakeAction(const std::string & action,
      const std::string & actionData)
{
   bool retval = false;

   if (action == "ResetInstanceCount")
   {
      instanceNumber = 0;
      retval = true;
   }
   else if (action == "IncrementInstanceCount")
   {
      ++instanceNumber;
      retval = true;
   }
   else if (action == "NoOutput")
   {
      showProgress = false;
      solverTextFile = "";
      matFileName = "";
      jsonFileName = "";
   }
   else
      retval = Solver::TakeAction(action, actionData);

   return retval;
}


//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
* Passes the transient force vector into the Estimator
*
* The transient force vector is a set of models used in GMAT's ODEModel for
* effects that are turned on and off over the course of a mission.  An example
* of a transient force is a finite burn, which is toggled by the
* BeginFiniteBurn and EndFiniteBurn commands.  These components are only used
* by commands that need them.  Typical usage is found in the propagation
* enabled commands.
*
* @param tf The vector of transient forces
*/
//------------------------------------------------------------------------------
void Estimator::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   transientForces = tf;
}


//------------------------------------------------------------------------------
//  std::vector<Real *>* GetForceDerivativeCache()
//------------------------------------------------------------------------------
/**
* Get the vector containing the derivatives of each force in the ODEModel
*
* This vector is used by SeqEstimators for calculating the process noise for
* EstimatedParameter objects, which need to know the accelerations due to
* their related force models
*
* @return The vector that contains the force derivatives
*/
//------------------------------------------------------------------------------
std::map<std::string, std::vector<Real *>>* Estimator::GetForceDerivativeCache()
{
   return &forceDerivativeCache;
}


//------------------------------------------------------------------------------
// PropSetup* GetPropagator(const std::string &forSpacecraft)
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the PropSetup object.
 *
 * @param forSpacecraft Spacecraft associated with the propagator.  If empty,
 *                      the default PropSetup is returned.
 *
 * @return The PropSetup pointer
 */
//------------------------------------------------------------------------------
PropSetup* Estimator::GetPropagator(const std::string &forSpacecraft)
{
   PropSetup *retval = nullptr;


   if (forSpacecraft == "")
   {
      if (propagators.size() > 0)
         retval = propagators[0];
   }
   else
   {
      if (needsSatPropMap)
      {
         BuildSatPropMap();
      }

      if (satPropMap.find(forSpacecraft) != satPropMap.end())
      {
         retval = satPropMap[forSpacecraft];
      }
      else
      {
         if (propagators.size() > 0)
            retval = propagators[0];
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
//  MeasurementManager* GetMeasurementManager()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the MeasurmentManager object.
 *
 * @return The MeasurementManager pointer
 */
//------------------------------------------------------------------------------
MeasurementManager* Estimator::GetMeasurementManager()
{
   return &measManager;
}


//------------------------------------------------------------------------------
// EstimationStateManager* GetEstimationStateManager()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the EstimationStateManager object.
 *
 * @return The EstimationStateManager pointer
 */
//------------------------------------------------------------------------------
EstimationStateManager* Estimator::GetEstimationStateManager()
{
   return &esm;
}


//------------------------------------------------------------------------------
//  Real GetTimeStep()
//------------------------------------------------------------------------------
/**
 * Returns the time step of the estimator.
 *
 * @return The time step
 */
//------------------------------------------------------------------------------
Real Estimator::GetTimeStep()
{
   return timeStep;
}


//------------------------------------------------------------------------------
// bool ResetState()
//------------------------------------------------------------------------------
/**
 * This method returns the resetState flag, turning it off in the process
 *
 * @return The resetState flag
 */
//------------------------------------------------------------------------------
bool Estimator::ResetState()
{
   bool retval = resetState;

   if (resetState == true)
      resetState = false;

   return retval;
}


//------------------------------------------------------------------------------
// bool ResetSTM()
//------------------------------------------------------------------------------
/**
 * This method resets the stm to the identity matrix
 */
 //------------------------------------------------------------------------------
void Estimator::ResetSTM()
{
   for (UnsignedInt i = 0; i < stateSize; ++i)
      for (UnsignedInt j = 0; j < stateSize; ++j)
         if (i == j)
            (*stm)(i, j) = 1.0;
         else
            (*stm)(i, j) = 0.0;
}


//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
 * Method to check for the presence of local clones
 *
 * @return true if there are local clones, false if not
 */
//------------------------------------------------------------------------------
bool Estimator::HasLocalClones()
{
   return true;
}


//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Updates cloned objects that are copies of the object passed in
 *
 * @param obj The object with setting updates
 */
//------------------------------------------------------------------------------
void Estimator::UpdateClonedObject(GmatBase *obj)
{
   if (obj->IsOfType("Spacecraft"))
      return;
   if (obj->IsOfType("Parameter"))
      return;
   throw EstimatorException("To do: implement Estimator::UpdateClonedObject "
         "for " + obj->GetTypeName() + " objects");
}

//------------------------------------------------------------------------------
// void UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
//------------------------------------------------------------------------------
/**
 * Added method to remove message in the Message window.
 *
 * The current implementation needs to be updated to actually process parameters
 * when they are updated in the system.  For now, it is just overriding the base
 * class "do nothing" method so that the message traffic is not shown to the
 * user.
 *
 * Turn on the debug to figure out the updates being requested.
 *
 * @param obj The master object holding the new parameter value
 * @param updatedParameterId The ID of the updated parameter
 */
//------------------------------------------------------------------------------
void Estimator::UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
{
#ifdef DEBUG_CLONED_PARAMETER_UPDATES
   MessageInterface::ShowMessage("Estimator updating parameter %d (%s) using "
         "object %s\n", updatedParameterId, obj->GetParameterText(updatedParameterId).c_str(),
         obj->GetName().c_str());
#endif
}


//------------------------------------------------------------------------------
// bool TestForConvergence(std::string &reason)
//------------------------------------------------------------------------------
/**
 * Method that tests an estimation to see if the process has converged.  Derived
 * classes implement this method to provide convergence tests.  This default
 * version always returns false.
 *
 * @param reason String indicating the condition(s) that indicate convergence
 *
 * @return return an integer to tell status of estimation
 */
//------------------------------------------------------------------------------
Integer Estimator::TestForConvergence(std::string &reason)
{
    return UNKNOWN;
}

//------------------------------------------------------------------------------
// GmatTime ConvertToGmatTimeEpoch(const std::string &theEpoch,
//                                 const std::string &theFormat)
//------------------------------------------------------------------------------
/**
 * Converts an epoch string is a specified format into a GmatTime
 *
 * @param theEpoch The input epoch
 * @param theFormat The format of the input epoch
 *
 * @return The converted epoch
 */
//------------------------------------------------------------------------------
GmatTime Estimator::ConvertToGmatTimeEpoch(const std::string &theEpoch, 
                                           const std::string &theFormat)
{
   GmatTime fromMjd(-999.999);
   GmatTime retval(-999.999);
   std::string outStr;

   theTimeConverter->Convert(theFormat, fromMjd, theEpoch, "A1ModJulian",
         retval, outStr);

   if (retval == -999.999)
      throw EstimatorException("Error converting the time string \"" + theEpoch +
            "\"; please check the format for the input string.");
   return retval;
}


//------------------------------------------------------------------------------
// void BuildResidualPlot(const std::string &plotName,
//       const StringArray &measurementNames)
//------------------------------------------------------------------------------
/**
 * Creates an OwnedPlot instance that is used for plotting residuals
 *
 * @param plotName The name of the plot.  This name needs to be unique in the
 *                 Sandbox
 * @param measurementNames The names of the measurement models that are sources
 *                         for the residuals being plotted
 */
//------------------------------------------------------------------------------
void Estimator::BuildResidualPlot(const std::string &plotName,
      const StringArray &measurementNames)
{
   OwnedPlot *rPlot;
   for (UnsignedInt i = 0; i < measurementNames.size(); ++i)
   {
      // Register measurement ID for this curve
      IntegerArray id = measManager.GetMeasurementId(measurementNames[i]);
      //std::vector<TrackingFileSet*> tfs = measManager.GetTrackingSets();
      std::vector<TrackingFileSet*> tfs = measManager.GetAllTrackingFileSets();

      UnsignedInt k;
      for (k = 0; k < tfs.size(); ++k)
         if (tfs[k]->GetName() == measurementNames[i])
            break;

      if (k == tfs.size())
      {
         // processing for old measurement model
         IntegerArray id = measManager.GetMeasurementId(measurementNames[i]);

         rPlot = new OwnedPlot(plotName);
         rPlot->SetStringParameter("PlotTitle", plotName);
         rPlot->SetBooleanParameter("UseLines", false);
         rPlot->SetBooleanParameter("UseHiLow", showErrorBars);
         rPlot->SetStringParameter("Add", measurementNames[i] + " Residuals");
         rPlot->SetUsedDataID(id[0]);
         rPlot->Initialize();

         residualPlots.push_back(rPlot);
      }
      else
      {
         // processing for tracking data adapter 
         std::vector<TrackingDataAdapter*>* adapters = tfs[k]->GetAdapters(); 
         for (UnsignedInt j = 0; j < adapters->size(); ++j)
         {
            Integer id = adapters->at(j)->GetModelID();
            std::string pName = adapters->at(j)->GetName();

            rPlot = new OwnedPlot(pName);
            rPlot->SetStringParameter("PlotTitle", plotName);
            rPlot->SetBooleanParameter("UseLines", false);
            rPlot->SetBooleanParameter("UseHiLow", showErrorBars);
            StringArray dimNames = adapters->at(j)->GetMeasurementDimension();
            Integer dim = dimNames.size();
            if (dimNames.size() > 1)
            {
               for (Integer k = 0; k < dim; ++k)
               {
                  std::stringstream ss;
                  ss << pName << "." << dimNames[k] << " Residuals";
                  rPlot->SetStringParameter("Add", ss.str());
               }
            }
            else
            {
               std::stringstream ss;
               ss << pName << " Residuals";
               rPlot->SetStringParameter("Add", ss.str());
            }

            rPlot->SetUsedDataID(id);
            rPlot->Initialize();

            residualPlots.push_back(rPlot);
         }
      }
   }

}


//------------------------------------------------------------------------------
// void PlotResiduals()
//------------------------------------------------------------------------------
/**
 * Plots residuals by passing the residual data to the OwnedPlot objects that
 * display the data.
 */
//------------------------------------------------------------------------------
void Estimator::PlotResiduals()
{
   #ifdef DEBUG_RESIDUAL_PLOTS
      MessageInterface::ShowMessage("Entered PlotResiduals\n");
      MessageInterface::ShowMessage("Processing plot with %d Residuals\n",
            measurementResiduals.size());
   #endif
   
   std::vector<RealArray*> dataBlast;
   RealArray epochs;
   //RealArray values;
   std::vector<RealArray> values;
   RealArray hiErrors; 
   RealArray lowErrors;

   RealArray *hi = NULL, *low = NULL;

   for (UnsignedInt i = 0; i < residualPlots.size(); ++i)
   {
      dataBlast.clear();
      epochs.clear();
      for (UnsignedInt j = 0; j < values.size(); ++j)
         values[j].clear();
      values.clear();

      if (showErrorBars)
      {
         hiErrors.clear();
         lowErrors.clear();
         if (hiLowData.size() > 0)
         {
            hi = hiLowData[0];
            if (hiLowData.size() > 1)
            {
               low = hiLowData[1];
            }
         }
      }

      // Collect residuals by plot
      Integer dim = 0;
      for (UnsignedInt j = 0; j < measStats.size(); ++j)
      {
         // for each measurement
         if (measStats[j].editFlag == NORMAL_FLAG && residualPlots[i]->UsesData(measStats[j].uniqueID) >= 0 && measStats[j].residual.size() > 0)
         {
            // Specify measurement dimension
            if (values.size() == 0)
            {
               RealArray yval;
               dim = measStats[j].residual.size();
               for (Integer k = 0; k < dim; ++k)
                  values.push_back(yval);
            }

            epochs.push_back(measStats[j].epoch.GetMjd());

            for (Integer k = 0; k < measStats[j].residual.size(); ++k)
               values[k].push_back(measStats[j].residual[k]);
            
            if (hi && showErrorBars)
            {
               hiErrors.push_back((*hi)[j]);
            }
            if (low && showErrorBars)
               lowErrors.push_back((*low)[j]);
         }
      }

      if (epochs.size() > 0)
      {
         dataBlast.push_back(&epochs);
         RealArray yval;
         for (UnsignedInt j = 0; j < values.size(); ++j)   // for j = X, Y, or Z
            dataBlast.push_back(&values[j]);

         residualPlots[i]->TakeAction("ClearData");
         residualPlots[i]->Deactivate();
         residualPlots[i]->SetData(dataBlast, hiErrors, lowErrors);
         residualPlots[i]->TakeAction("Rescale");
         residualPlots[i]->Activate();
      }

      #ifdef DEBUG_RESIDUALS
         // Dump the data to screen
         MessageInterface::ShowMessage("DataDump for residuals plot %d:  number of measurement = %d\n", i, dataBlast[0]->size());
         for (UnsignedInt k = 0; k < dataBlast[0]->size(); ++k)
         {
            MessageInterface::ShowMessage("  dim = %d\n", dataBlast.size());
            for (UnsignedInt n = 0; n < dataBlast.size(); ++n)
            {
               MessageInterface::ShowMessage("%.12lf  ", dataBlast[n]->at(k));    // measurement kth's value[n] 

               //if (hi)
               //   if (hi->size() > k)
               //      if (low)
               //         MessageInterface::ShowMessage("   + %.12lf", (*hi)[k]);
               //      else
               //         MessageInterface::ShowMessage("   +/- %.12lf", (*hi)[k]);
               //if (low)
               //   if (low->size() > k)
               //      MessageInterface::ShowMessage(" - %.12lf", (*low)[k]);
            }
            MessageInterface::ShowMessage("\n");
         }
      #endif
   }

}

//------------------------------------------------------------------------------
// void EnhancePlot()
//------------------------------------------------------------------------------
/**
 * Adds decorations to residuals plots
 *
 * This method is used to add new information to the residuals plots, like
 * deviation curves, notations, and so on.
 *
 * This default method provides no enhancements
 */
//------------------------------------------------------------------------------
void Estimator::EnhancePlot()
{
}


//------------------------------------------------------------------------------
// unused methods
//------------------------------------------------------------------------------

// Methods required by base classes
// Not needed for simulation

//------------------------------------------------------------------------------
// Integer SetSolverResults(Real*, const std::string&, const std::string&)
//------------------------------------------------------------------------------
/**
 * Method used to report values generated in a SolverControlSequence.
 *
 * This method is not used in GMAT's current estimators; it is required to
 * satisfy Solver abstract method requirements.
 */
//------------------------------------------------------------------------------
Integer Estimator::SetSolverResults(Real*, const std::string&,
      const std::string&)
{
   return -1;
}


//------------------------------------------------------------------------------
// void SetResultValue(Integer, Real, const std::string&)
//------------------------------------------------------------------------------
/**
 * Method used to register contributors to that provide generated data.
 *
 * This method is not used in GMAT's current estimators; it is required to
 * satisfy Solver abstract method requirements.
 */
//------------------------------------------------------------------------------
void Estimator::SetResultValue(Integer, Real, const std::string&)
{
}


//--------------------------------------------------------------------------
// bool StateConversion(GmatTime forTime, CoordinateSystem* fromCS, 
//     std::string fromCSType, Rvector fromState, CoordinateSystem* toCS, 
//     std::string toCSType, Rvector& toState)
//--------------------------------------------------------------------------
/** This function is used to convert state in a given coordinate system with 
*     a give state type (Cartesian or Keplerian) to another coordinate system
*     with another state type at a given epoch.
*
*/
//--------------------------------------------------------------------------
//bool Estimator::StateConversion(GmatTime forTime, CoordinateSystem* fromCS, 
//                                std::string fromCSType, Rvector fromState,
//                                CoordinateSystem* toCS, std::string toCSType, 
//                                Rvector& toState)
//{
//   // 1. Convert from Keplerian to Cartesian if it needs
//   Rvector inState;
//   Rvector outState;
//   if (fromCSType != "KeplerianState")
//      inState = fromState;
//   else
//   {
//      // 1.1. Get mu value
//      Planet* pn = (Planet*)(fromCS->GetOrigin());
//      Real mu = pn->GetRealParameter("Mu");
//
//      // 1.2. Convert from Keplerian to Cartesian
//      Rvector6 inState6(fromState[0], fromState[1], fromState[2], fromState[3], fromState[4], fromState[5]);
//      Rvector outState = StateConversionUtil::KeplerianToCartesian(mu, inState6);
//   }
//
//   // 2. Convert from inState from coordinate system fromCS to coordiante system toCS at time fromTime
//   CoordinateConverter *cv = new CoordinateConverter();
//   cv->Convert(forTime, inState, fromCS, outState, toCS, false, false);
//   delete cv;
//
//   // 3. Convert from Cartesian to Keplerian if it needs
//   if (toCSType != "CartesianState")
//      toState = outState;
//   else
//   {
//      // 3.1. Get mu value
//      Planet* pn = (Planet*)(toCS->GetOrigin());
//      Real mu = pn->GetRealParameter("Mu");
//
//      // 3.2. Convert from Cartesian to Keplerian
//      Rvector6 outState6(outState[0], outState[1], outState[2], outState[3], outState[4], outState[5]);
//      toState = StateConversionUtil::CartesianToKeplerian(mu, outState6);
//   }
//
//   return true;
//}


//------------------------------------------------------------------------------
// bool Estimator::ConvertToParticipantCoordSystem(ListItem* infor, Real epoch, 
//                    Rvector6 &inState, Rvector6 &outState)
//------------------------------------------------------------------------------
/**
 * Method used to convert result of a spacecraft state from internal coordinate
 * system to the coordinate system specified in script
 *
 * @param infor       information about state's element
 * @param epoch       the epoch at which the state is converted it's 
 *                    coordinate system
 * @param inState     spacecraft's state in its internal coordinate system
 *                              (A1Mjd)
 * @param outState    spacecraft's state in its apparent coordinate system
 *
*/
//------------------------------------------------------------------------------
//bool Estimator::ConvertToParticipantCoordSystem(ListItem* infor, GmatTime epoch, Rvector &inState, Rvector &outState)
//{
//   outState = inState;
//
//   if (infor->object->IsOfType(Gmat::SPACEOBJECT))
//   {
//      if ((infor->elementName == "CartesianState") || (infor->elementName == "Position") || (infor->elementName == "Velocity"))
//      {
//         Spacecraft* obj = (Spacecraft*)(infor->object);
//         std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
//         CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
//         CoordinateSystem* internalcs = obj->GetInternalCoordSystem();
//
//         if (cs == NULL)
//            throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
//         if (internalcs == NULL)
//            throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");
//
//         CoordinateConverter* cv = new CoordinateConverter();
//         //cv->Convert(A1Mjd(epoch), inState, internalcs, outState, cs);
//         cv->Convert(epoch, inState, internalcs, outState, cs);
//
//         delete cv;
//      }
//      else if (infor->elementName == "KeplerianState")
//      {
//         Spacecraft* obj = (Spacecraft*)(infor->object);
//         std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
//         CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
//         CoordinateSystem* internalcs = obj->GetInternalCoordSystem();
//
//         if (cs == NULL)
//            throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
//         if (internalcs == NULL)
//            throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");
//
//         CoordinateConverter* cv = new CoordinateConverter();
//         Rvector6 outStateCart;
//         cv->Convert(epoch, inState, internalcs, outStateCart, cs);
//
//         // Convert Cartesian to Keplerian 
//         Planet* pn = (Planet*)(cs->GetOrigin());
//         Real mu = pn->GetRealParameter("Mu");
//         outState = StateConversionUtil::CartesianToKeplerian(mu, outStateCart);
//         
//         delete cv;
//      }
//   }
//
//   return true;
//}


//-------------------------------------------------------------------------
// GmatState& Estimator::GetEstimationStateForReport()
//-------------------------------------------------------------------------
/**
 * This Method used to convert result of estimation state to participants'
 * coordinate system. For report, it reports Cr and Cd instead of Cr_Epsilon 
 * and Cd_Epsilon.
 *
 * @return        estimation state in participants' coordinate systems
 *
*/
//-------------------------------------------------------------------------
//GmatState Estimator::GetEstimationStateForReport()
//{
//   GmatState outputState;
//   const std::vector<ListItem*> *map = esm.GetStateMap();
//
//   Real outputStateElement;
//   outputState.SetSize(map->size());
//
//   for (UnsignedInt i = 0; i < map->size(); ++i)
//   {
//      //ConvertToParticipantCoordSystem((*map)[i], estimationEpoch, (*estimationState)[i], &outputStateElement);
//      //outputState[i] = outputStateElement;
//
//      // get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
//      if ((*map)[i]->elementName == "Cr_Epsilon")
//         outputState[i] = ((Spacecraft*) (*map)[i]->object)->GetRealParameter("Cr");
//      else if ((*map)[i]->elementName == "Cd_Epsilon")
//         outputState[i] = ((Spacecraft*) (*map)[i]->object)->GetRealParameter("Cd");
//      // Thrust Scale Factor Solve For
//      else if ((*map)[i]->elementName == "TSF_Epsilon")
//         outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("ThrustScaleFactor");
//      else if (((*map)[i]->elementName == "CartesianState") || 
//               ((*map)[i]->elementName == "KeplerianState") || 
//               ((*map)[i]->elementName == "Position"))
//      {
//         if ((*map)[i]->subelement == 1)
//         {
//            Rvector inState(6,
//               (*estimationState)[i],
//               (*estimationState)[i + 1],
//               (*estimationState)[i + 2],
//               (*estimationState)[i + 3],
//               (*estimationState)[i + 4],
//               (*estimationState)[i + 5]);
//            Rvector outState(6);
//            
//            //ConvertToParticipantCoordSystem((*map)[i], estimationEpoch, inState, outState);
//            ConvertToParticipantCoordSystem((*map)[i], estimationEpochGT, inState, outState);
//            for (Integer j = 0; j < 6; ++j)
//               outputState[i + j] = outState[j];
//
//            i = i + 5;
//         }
//      }
//      //else if ((*map)[i]->elementName == "KeplerianState")
//      //{
//      //   if ((*map)[i]->subelement == 1)
//      //   {
//      //      Rvector inState(6,
//      //         (*estimationState)[i],
//      //         (*estimationState)[i + 1],
//      //         (*estimationState)[i + 2],
//      //         (*estimationState)[i + 3],
//      //         (*estimationState)[i + 4],
//      //         (*estimationState)[i + 5]);
//      //      Rvector outState(6);
//
//      //      ConvertToParticipantCoordSystem((*map)[i], estimationEpochGT, inState, outState);
//      //      for (Integer j = 0; j < 6; ++j)
//      //         outputState[i + j] = outState[j];
//
//      //      i = i + 5;
//      //   }
//      //}
//      else if ((*map)[i]->elementName == "Bias")
//      {
//         Rvector inState(1, (*estimationState)[i]);
//         Rvector outState(1);
//         ConvertToParticipantCoordSystem((*map)[i], estimationEpochGT, inState, outState);
//         outputState[i] = outState[0];
//      }
//   }
//
//   return outputState;
//}


//------------------------------------------------------------------------------
// ObservationData* FilteringData(ObservationData* obsData, Integer obDataId)
//------------------------------------------------------------------------------
/**
* This function performs second level data editing.
*
* @param obsData     Observation data object needed for filtering
* @param obDataId    record number of the observation data
*
* @return            a pointer to the observation data object if observation
*                    data meet criteria defined in data filter objects, NULL
*                    otherwise
*/
//------------------------------------------------------------------------------
ObservationData* Estimator::FilteringData(ObservationData* dataObject, Integer obDataId, Integer& filterIndex)
{
#ifdef DEBUG_FILTER
   MessageInterface::ShowMessage("Enter Estimator<%s,%p>::FilteringData(dataObject = <%p>, obDataId = %d).\n", GetName().c_str(), this, dataObject, obDataId);
#endif

   Integer rejReason;

   filterIndex = dataFilterObjs.size();
   ObservationData* obdata = dataObject;

   // Run estimation reject filters
   if (obdata)
   {
      for (UnsignedInt i = 0; i < dataFilterObjs.size(); ++i)
      {
         if (dataFilterObjs[i]->IsOfType("RejectFilter"))
         {
            rejReason = 0;
            obdata = ((RejectFilter*)dataFilterObjs[i])->FilteringData(dataObject, rejReason, obDataId);

            // it is rejected when it has been rejected by any reject filter
            if (obdata == NULL)
            {
               filterIndex = i;
               break;
            }
         }
      }
   }

   // Run statistic accept filters when it passes reject filters
   if (obdata)
   {
      ObservationData* obdata1 = NULL;
      ObservationData* od;
      bool hasAcceptFilter = false;
      for (UnsignedInt i = 0; i < dataFilterObjs.size(); ++i)
      {
         if (dataFilterObjs[i]->IsOfType("AcceptFilter"))
         {
            hasAcceptFilter = true;
            rejReason = 0;
            od = ((AcceptFilter*)dataFilterObjs[i])->FilteringData(dataObject, rejReason, obDataId);
            //MessageInterface::ShowMessage("   od = <%p>   rejReason = %d   \n", od, rejReason);
            // it is accepted when it has been accepted by any accept filter
            if (od)
            {
               obdata1 = od;
            }
            else
               filterIndex = i;
         }
      }

      if (hasAcceptFilter)
      {
         obdata = obdata1;
         if (obdata1)
            filterIndex = dataFilterObjs.size();
      }
   }

#ifdef DEBUG_FILTER
   MessageInterface::ShowMessage("Enter Estimator<%s,%p>::FilteringData(dataObject = <%p>, obDataId = %d).\n", GetName().c_str(), this, dataObject, obDataId);
#endif

   return obdata;
}

//------------------------------------------------------------------------------
// void Symmetrize(Rmatrix& mat)
//------------------------------------------------------------------------------
/**
 * Symmetrizes a covariance matrix
 *
 * @param mat The covariance matrix that is symmetrized
 */
//------------------------------------------------------------------------------
void Estimator::Symmetrize(Covariance& mat)
{
   Integer size = mat.GetDimension();

   for (Integer i = 0; i < size; ++i)
   {
      for (Integer j = i+1; j < size; ++j)
      {
         mat(i,j) = 0.5 * (mat(i,j) + mat(j,i));
         mat(j,i) = mat(i,j);
      }
   }
}


//------------------------------------------------------------------------------
// void Symmetrize(Rmatrix& mat)
//------------------------------------------------------------------------------
/**
 * Symmetrizes a square Rmatrix
 *
 * @param mat The matrix that is symmetrized
 */
//------------------------------------------------------------------------------
void Estimator::Symmetrize(Rmatrix& mat)
{
   Integer size = mat.GetNumRows();

   if (size != mat.GetNumColumns())
   {
      throw EstimatorException("Cannot symmetrize non-square matrices");
   }

   for (Integer i = 0; i < size; ++i)
   {
      for (Integer j = i+1; j < size; ++j)
      {
         mat(i,j) = 0.5 * (mat(i,j) + mat(j,i));
         mat(j,i) = mat(i,j);
      }
   }
}


//------------------------------------------------------------------------------
// Rmatrix thinQR(const Rmatrix &mat1, const Rmatrix &mat2)
//------------------------------------------------------------------------------
/**
 * Performs the "thin" QR decomposition of the compound matrix.
 *
 * @param mat1 The left side of the matrix to concatenate before perorming the "thin" QR decomposition.
 * @param mat2 The right side of the matrix to concatenate before perorming the "thin" QR decomposition.
 */
 //------------------------------------------------------------------------------
Rmatrix Estimator::thinQR(const Rmatrix &mat1, const Rmatrix &mat2)
{
   Rmatrix mat(mat1.GetNumRows(), mat1.GetNumColumns() + mat2.GetNumColumns());

   for (UnsignedInt ii = 0U; ii < mat.GetNumRows(); ii++)
   {
      for (UnsignedInt jj = 0U; jj < mat1.GetNumColumns(); jj++)
         mat(ii, jj) = mat1(ii, jj);

      for (UnsignedInt jj = 0U; jj < mat2.GetNumColumns(); jj++)
         mat(ii, jj + mat1.GetNumColumns()) = mat2(ii, jj);
   }

   Rmatrix Q(mat.GetNumColumns(), mat.GetNumRows());
   Rmatrix R(mat.GetNumColumns(), mat.GetNumRows());
   qr.Factor(mat.Transpose(), R, Q);

   Rmatrix N(mat.GetNumRows(), mat.GetNumRows());

   for (UnsignedInt ii = 0U; ii < mat.GetNumRows(); ii++)
      for (UnsignedInt jj = 0U; jj <= ii; jj++)
         N(ii, jj) = R(jj, ii);

   return N;
}


//------------------------------------------------------------------------------
// Covariance* GetMeasurementCovariance()
//------------------------------------------------------------------------------
/**
 * This method returns the covariance associated with the measurement.
 * If the observation has covariance, that value is returned, otherwise the
 * covariance of the calculated measurement is returned
 *
 * @return The covariance of the measurement
 */
//------------------------------------------------------------------------------
Covariance* Estimator::GetMeasurementCovariance()
{
   // Get current observation and measurement data
   const ObservationData *currentObs = measManager.GetObsData();
   const MeasurementData *calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);

   if (currentObs->noiseCovariance == NULL)
      return calculatedMeas->covariance;
   else
      return currentObs->noiseCovariance;
}


//------------------------------------------------------------------------------
// Real GetMeasurementCovariance(const UnsignedInt r, const UnsignedInt c)
//------------------------------------------------------------------------------
/**
 * This method returns the covariance associated with the measurement.
 * If the observation has covariance, that value is returned, otherwise the
 * covariance of the calculated measurement is returned
 *
 * @param r Row of covarience element
 * @param c Column of covarience element
 *
 * @return The covariance of the measurement
 */
//------------------------------------------------------------------------------
Real Estimator::GetMeasurementCovariance(const UnsignedInt r, const UnsignedInt c)
{
   return (*GetMeasurementCovariance())(r, c);
}


//------------------------------------------------------------------------------
// Real GetMeasurementWeight(const UnsignedInt index)
//------------------------------------------------------------------------------
/**
 * This method returns the weight of the measurement. If the covariance is zero,
 * The weight is set to 1.0
 *
 * Weight is diag(1 / sigma^2), per Montebruck & Gill, eq. 8.33
 * Covariance diagonal is ~diag(sigma^2)
 * If no off diagonal terms, inverse in 1/element along diagonal
 *
 * @param index Index of measurement to calculate weight
 *
 * @return The weight of the measurement
 */
//------------------------------------------------------------------------------
Real Estimator::GetMeasurementWeight(const UnsignedInt index)
{
   Real cov = GetMeasurementCovariance(index, index);

   // If no noise sigma is specified, the covariance is zero,
   // so the weight is set to 1.0 to evenly weight all measurements
   if (cov == 0.0)
      return 1.0;
   else
      return 1.0 / cov;
}


//------------------------------------------------------------------------------
// bool SetNominalValues()
//------------------------------------------------------------------------------
/**
 * Set the nominal values for solve fors if the EstimatedParameter requires it
 *
 * @return true if any values were updated, false otherwise
 */
 //------------------------------------------------------------------------------
bool Estimator::SetNominalValues()
{
   // flag to check if a nominal value was changed,
   // menaing the covariance needs an epsilon conversion update
   bool valueChanged = false;

   Covariance * cov = esm.GetCovariance();

   RealArray prevEpsilonConversions(esm.GetStateSize());
   for (UnsignedInt ii = 0; ii < esm.GetStateSize(); ii++)
      prevEpsilonConversions[ii] = GetEpsilonConversion(ii);

   if (GmatType::GetTypeId("EstimatedParameter") != Gmat::UNKNOWN_OBJECT)
   {
      const std::vector<ListItem*>* stateMap = esm.GetStateMap();

      for (UnsignedInt ii = 0U; ii < stateMap->size();)
      {
         GmatBase* obj = (*stateMap)[ii]->object;

         ObjectArray params = obj->GetRefObjectArray(GmatType::GetTypeId("EstimatedParameter"));
         GmatBase* estParam = NULL; // Placeholder for the EstimatedParameter for this state

         // Loop thorugh each EstimatedParameter on the object and see if there is a solve-for match
         for (UnsignedInt jj = 0U; jj < params.size(); jj++)
         {
            if (params[jj] && params[jj]->IsOfType("EstimatedParameter"))
            {
               std::string solveForName = params[jj]->GetStringParameter("SolveFor");
               Integer estParamID = obj->GetEstimationParameterID(solveForName);

               if (estParamID == (*stateMap)[ii]->elementID)
               {
                  estParam = params[jj];
                  break;
               }
            }
         }

         // If there is an EstimatedParameter for this solve for, use the SteadyStateValue
         // as the nominal value, and set the epsilon accordingly
         if (estParam)
         {
            for (UnsignedInt jj = 0U; jj < (*stateMap)[ii]->length; jj++)
            {
               Integer index = ii + jj;
               std::string solveForName = estParam->GetStringParameter("SolveFor");
               Integer solveForID = obj->GetParameterID(solveForName);

               Real currentValue = GetStateValue(solveForID, index);
               Real nominalValue = estParam->GetRealParameter("SteadyStateValue");
               Real currentEpsilon = currentValue / nominalValue - 1.0;

               // Set the nominal and new epsilon values
               SetStateValue(solveForID, index, nominalValue);
               SetStateValue((*stateMap)[ii]->parameterID, index, currentEpsilon);

               valueChanged = true;
            }
         }

         ii += (*stateMap)[ii]->length;
      }
   }

   // If a parameter was changed, the covariance needs to be updated for the new
   // nominal/epsilon value pair
   if (valueChanged)
   {
      esm.MapObjectsToVector();

      UpdateCovarianceNominalValues(prevEpsilonConversions);

      esm.MapCovariancesToObjects();
      PropagationStateManager *psm = propagators[0]->GetPropStateManager();
      psm->MapObjectsToVector();
   }

   return valueChanged;
}


//------------------------------------------------------------------------------
// void UpdateCovarianceNominalValues(RealArray prevEpsilonConversions)
//----------------------------------------------------------------------
/**
 * Updates the nominal values of solve-fors which use epsilon values
 *
 * @param prevEpsilonConversions The array the previous epsilon conversion values.
 */
 //------------------------------------------------------------------------------
void Estimator::UpdateCovarianceNominalValues(RealArray prevEpsilonConversions)
{
   Covariance * cov = esm.GetCovariance();

   for (UnsignedInt ii = 0; ii < esm.GetStateSize(); ii++)
   {
      Real iiConv = GetEpsilonConversion(ii);

      for (UnsignedInt jj = 0; jj <= ii; jj++)
      {
         Real jjConv = GetEpsilonConversion(jj);

         Real value = (*cov)(ii, jj);
         value *= prevEpsilonConversions[ii] * prevEpsilonConversions[jj];
         value /= iiConv * jjConv;
         (*cov)(ii, jj) = value;
         (*cov)(jj, ii) = value;
      }
   }
}


//------------------------------------------------------------------------------
// void GetStateValue(const Integer id, const Integer stateIndex)
//------------------------------------------------------------------------------
/**
 * Set the nominal values for solve fors if the EstimatedParameter requires it
 */
 //------------------------------------------------------------------------------
Real Estimator::GetStateValue(const Integer id, const Integer stateIndex)
{
   const std::vector<ListItem*>* stateMap = esm.GetStateMap();
   GmatBase *obj = (*stateMap)[stateIndex]->object;

   Real retValue;

   switch ((*stateMap)[stateIndex]->parameterType)
   {
   case Gmat::REAL_TYPE:
      retValue = obj->GetRealParameter(id);
      break;

   case Gmat::RVECTOR_TYPE:
      retValue = obj->GetRealParameter(id,
         (*stateMap)[stateIndex]->rowIndex);
      break;

   case Gmat::RMATRIX_TYPE:
      retValue = obj->GetRealParameter(id,
         (*stateMap)[stateIndex]->rowIndex, (*stateMap)[stateIndex]->colIndex);
      break;

   default:
      std::stringstream sel("");
      sel << (*stateMap)[stateIndex]->subelement;
      std::string label = (*stateMap)[stateIndex]->objectName + "." +
         (*stateMap)[stateIndex]->elementName + "." + sel.str();
      MessageInterface::ShowMessage(
         "%s not set; Element type not handled\n", label.c_str());
   }

   return retValue;
}


//------------------------------------------------------------------------------
// void SetStateValue(const Integer id, const Integer stateIndex, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the nominal values for solve fors if the EstimatedParameter requires it
 */
 //------------------------------------------------------------------------------
Real Estimator::SetStateValue(const Integer id, const Integer stateIndex, const Real value)
{
   const std::vector<ListItem*>* stateMap = esm.GetStateMap();
   GmatBase *obj = (*stateMap)[stateIndex]->object;

   Real retValue;

   switch ((*stateMap)[stateIndex]->parameterType)
   {
   case Gmat::REAL_TYPE:
      retValue = obj->SetRealParameter(id, value);
      break;

   case Gmat::RVECTOR_TYPE:
      retValue = obj->SetRealParameter(id, value,
         (*stateMap)[stateIndex]->rowIndex);
      break;

   case Gmat::RMATRIX_TYPE:
      retValue = obj->SetRealParameter(id, value,
         (*stateMap)[stateIndex]->rowIndex, (*stateMap)[stateIndex]->colIndex);
      break;

   default:
      std::stringstream sel("");
      sel << (*stateMap)[stateIndex]->subelement;
      std::string label = (*stateMap)[stateIndex]->objectName + "." +
         (*stateMap)[stateIndex]->elementName + "." + sel.str();
      MessageInterface::ShowMessage(
         "%s not set; Element type not handled\n", label.c_str());
   }

   return retValue;
}


//------------------------------------------------------------------------------
// Real GetEpsilonConversion(const UnsignedInt stateIndex)
//------------------------------------------------------------------------------
/**
 * This method will get the conversion value to convert partials from the
 * Epsilon value. If no conversion is required, this function will return 1.0
 * For Cr, return Cr0
 * For Cd, return Cd0
 *
 * @param stateIndex Index of the state element to get the conversion value for
 *
 * @return The conversion value
 */
//------------------------------------------------------------------------------
Real Estimator::GetEpsilonConversion(const UnsignedInt stateIndex)
{
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   std::string stateName = (*stateMap)[stateIndex]->elementName;
   GmatBase *stateObject = (*stateMap)[stateIndex]->object;

   Real conversionValue = 1.0;

   // Check if the state requires a conversion from Epsilon
   if (stateName == "Cr_Epsilon")
   {
      if (((Spacecraft*)(*stateMap)[stateIndex]->object)->GetSRPShapeModel() == "Spherical")
      {
         conversionValue = (*stateMap)[stateIndex]->object->GetRealParameter("Cr") / (1 + (*stateMap)[stateIndex]->object->GetRealParameter("Cr_Epsilon"));
      }
      else if (((Spacecraft*)(*stateMap)[stateIndex]->object)->GetSRPShapeModel() == "SPADFile")
      {
         conversionValue = (*stateMap)[stateIndex]->object->GetRealParameter("SPADSRPScaleFactor") / (1 + (*stateMap)[stateIndex]->object->GetRealParameter("Cr_Epsilon"));
      }
   }
   else if (stateName == "Cd_Epsilon")
   {
      if (((Spacecraft*)(*stateMap)[stateIndex]->object)->GetDragShapeModel() == "Spherical")
      {
         conversionValue = (*stateMap)[stateIndex]->object->GetRealParameter("Cd") / (1 + (*stateMap)[stateIndex]->object->GetRealParameter("Cd_Epsilon"));
      }
      else if (((Spacecraft*)(*stateMap)[stateIndex]->object)->GetDragShapeModel() == "SPADFile")
      {
         conversionValue = (*stateMap)[stateIndex]->object->GetRealParameter("SPADDragScaleFactor") / (1 + (*stateMap)[stateIndex]->object->GetRealParameter("Cd_Epsilon"));
      }
   }
   else if (stateName == "AtmosDensity_Epsilon")
   {
      conversionValue = (*stateMap)[stateIndex]->object->GetRealParameter("AtmosDensityScaleFactor")
         / (1 + (*stateMap)[stateIndex]->object->GetRealParameter("AtmosDensity_Epsilon"));
   }
   else if (GmatStringUtil::EndsWith(stateName, ".TSF_Epsilon"))     // <segmentName>.TSF_Epsilon
   {
      StringArray parts = GmatStringUtil::SeparateBy(stateName, ".");
      std::string tsfName = parts.at(0) + ".ThrustScaleFactor";

      conversionValue = stateObject->GetRealParameter(tsfName) / (1 + stateObject->GetRealParameter(stateName));
   }

   return conversionValue;
}


//------------------------------------------------------------------------------
// void CovarianceEpsilonConversion(Rmatrix& cov)
//------------------------------------------------------------------------------
/**
 * This method will convert the terms in the covariance that require a
 * conversion from their epsilon value for reporting.
 *
 * @param cov The covariance matrix to convert
 */
//------------------------------------------------------------------------------
void Estimator::CovarianceEpsilonConversion(Rmatrix& cov)
{
   for (UnsignedInt i = 0; i < esm.GetStateSize(); ++i)
   {
      Real conversionValue = GetEpsilonConversion(i);
      for (UnsignedInt j = 0; j < esm.GetStateSize(); ++j)
      {
         cov(i, j) *= conversionValue;
         cov(j, i) *= conversionValue;
      }
   }
}


//------------------------------------------------------------------------------
// void CovarianceEpsilonConversionReverse(Rmatrix& cov)
//------------------------------------------------------------------------------
/**
 * This method will convert the terms in the covariance that require a
 * conversion to their epsilon value for reporting.
 *
 * @param cov The covariance matrix to convert
 */
 //------------------------------------------------------------------------------
void Estimator::CovarianceEpsilonConversionReverse(Rmatrix& cov)
{
   for (UnsignedInt i = 0; i < esm.GetStateSize(); ++i)
   {
      Real conversionValue = GetEpsilonConversion(i);
      for (UnsignedInt j = 0; j < esm.GetStateSize(); ++j)
      {
         cov(i, j) /= conversionValue;
         cov(j, i) /= conversionValue;
      }
   }
}


//------------------------------------------------------------------------------
// void CartesianCovarianceRotation(Rmatrix& cov, bool allStatesCart)
//------------------------------------------------------------------------------
/**
 * This method will convert the terms in the covariance that require a
 * rotation from MJ2000Eq state to the coordinate system specified in script.
 *
 * @param cov The covariance matrix to convert
 * @param allStatesCart Flag if the position/velocity states have already been
 *                      converted to Cartesian
 */
 //------------------------------------------------------------------------------
void Estimator::CartesianCovarianceRotation(Rmatrix& cov, bool allStatesCart)
{
   const std::vector<ListItem*> *map = esm.GetStateMap();
   GmatState* state = esm.GetState();
   GmatTime epoch = state->GetEpochGT();

   Rmatrix transform = Rmatrix::Identity(esm.GetStateSize());

   UnsignedInt i = 0;
   while (i < esm.GetStateSize())
   {
      if (((*map)[i]->elementName == "CartesianState") ||
          (((*map)[i]->elementName == "KeplerianState") && allStatesCart))
      {
         if ((*map)[i]->object->IsOfType(Gmat::SPACECRAFT))
         {
            Rvector6 inState((*state)[i], (*state)[i + 1], (*state)[i + 2], (*state)[i + 3], (*state)[i + 4], (*state)[i + 5]);

            Spacecraft *obj = (Spacecraft*)(*map)[i]->object;
            std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
            CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
            CoordinateSystem* internalcs = obj->GetInternalCoordSystem();

            if (cs == NULL)
               throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
            if (internalcs == NULL)
               throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

            CoordinateConverter cv;
            Rvector6 outStateCart;
            cv.Convert(epoch, inState, internalcs, outStateCart, cs);

            Rmatrix33 rot = cv.GetLastRotationMatrix();

            for (UnsignedInt row = 0U; row < 3U; row++)
            {
               for (UnsignedInt col = 0U; col < 3U; col++)
               {
                  transform(i + row, i + col) = rot(row, col);
                  transform(i+3 + row, i+3 + col) = rot(row, col);
               }
            }
         }
      }

      i += (*map)[i]->length;
   }

   cov = transform * cov * transform.Transpose();
}

//------------------------------------------------------------------------------
// Rmatrix GetCovarianceVNB(const Rmatrix& inCov)
//------------------------------------------------------------------------------
/**
 * Converts the position covariance from MJ2000 to VNB
 */
 //------------------------------------------------------------------------------
Rmatrix Estimator::GetCovarianceVNB(const Rmatrix& inCov)
{
   CoordinateConverter cc;
   ObjectArray satArray;
   esm.GetStateObjects(satArray, Gmat::SPACECRAFT);
   UnsignedInt satArraySize = satArray.size();
   UnsignedInt matSize = satArraySize*6;
   Rvector outState(stateSize);
   Rvector inState(stateSize);
   inState.Set(estimationState->GetState(), stateSize);

   Rmatrix transform(matSize, matSize);
   UnsignedIntArray satStartIdxs; // Start index of the S/C Cartesian states in the esm
   const std::vector<ListItem*> *map = esm.GetStateMap();

   // Make sure the state offset is included when calculating VNB
   esm.MapFullVectorToObjects();
   for (UnsignedInt idx = 0; idx < satArraySize; idx++)
   {
      UnsignedInt satIdx;
      for (satIdx = 0U; satIdx < stateSize; satIdx++)
      {
         if ((*map)[satIdx]->objectName == satArray[idx]->GetName() &&
            (((*map)[satIdx]->elementName == "CartesianState") ||
            ((*map)[satIdx]->elementName == "KeplerianState")))
            break;
      }

      // Skip this if not found in esm state map
      if (satIdx == stateSize)
         continue;

      // Start index for block of data going into transform matrix
      UnsignedInt offsetIdx = satStartIdxs.size() * 6;
      satStartIdxs.push_back(satIdx);

      cc.Convert(currentEpochGT, inState, ((Spacecraft*)satArray[idx])->GetInternalCoordSystem(), outState, vnbFrames[satArray[idx]], true, false);

      Rmatrix33 vnbRot = cc.GetLastRotationMatrix();

      for (UnsignedInt ii = 0; ii < 3U; ii++)
      {
         for (UnsignedInt jj = 0; jj < 3U; jj++)
         {
            transform(offsetIdx + ii, offsetIdx + jj) = vnbRot(ii, jj);
            transform(offsetIdx + ii + 3, offsetIdx + jj + 3) = vnbRot(ii, jj);
         }
      }
   }
   esm.MapVectorToObjects();

   // Resize transform matrix if previous loop skipped a spacecraft
   UnsignedInt satSize = satStartIdxs.size();
   UnsignedInt vnbSize = 6 * satSize;
   if (vnbSize != matSize)
      transform.SetSize(vnbSize, vnbSize, false);

   Rmatrix dX_dS = esm.CartToSolveForStateConversionDerivativeMatrix();
   Rmatrix cartCov = dX_dS * inCov * dX_dS.Transpose();
   Rmatrix cov(vnbSize, vnbSize);

   for (UnsignedInt satIdx1 = 0; satIdx1 < satSize; satIdx1++)
   {
      UnsignedInt covIdx1 = satStartIdxs[satIdx1];
      for (UnsignedInt satIdx2 = 0; satIdx2 < satSize; satIdx2++)
      {
         UnsignedInt covIdx2 = satStartIdxs[satIdx2];

         for (UnsignedInt ii = 0; ii < 6U; ii++)
            for (UnsignedInt jj = 0; jj < 6U; jj++)
               cov(6 * satIdx1 + ii, 6 * satIdx2 + jj) = cartCov(covIdx1 + ii, covIdx2 + jj);
      }
   }

   Rmatrix covVNB = transform * cov * transform.Transpose();
   return covVNB;
}


Real Estimator::ObservationDataCorrection(Real cValue, Real oValue, Real moduloConstant)
{
   Real delta = cValue - oValue;
   int N = (int)(delta/moduloConstant + 0.5);

   return (oValue + N*moduloConstant);
}


void Estimator::ValidateMediaCorrection(const MeasurementData* measData)
{
   if (measData->isIonoCorrectWarning)
   {
      // Get measurement pass:
      std::stringstream ss1;
      ss1 << "{{";
      for (Integer i = 0; i < measData->participantIDs.size(); ++i)
      {
         ss1 << measData->participantIDs[i] << (((i + 1) < measData->participantIDs.size()) ? "," : "");
      }
      ss1 << "}," << measData->typeName << "}";

      // if the pass is not in warning list, then display warning message
      if (find(ionoWarningList.begin(), ionoWarningList.end(), ss1.str()) == ionoWarningList.end())
      {
         // specify unit of this measurement data
         std::string unit = GetUnit(measData->typeName);

         // generate warning message
         //MessageInterface::ShowMessage("Warning: When running estimator '%s', ionosphere correction is %lf m for measurement %s at measurement time tag %.12lf A1Mjd. Media corrections to the computed measurement may be inaccurate.\n", GetName().c_str(), measData->ionoCorrectValue * 1000.0, ss1.str().c_str(), measData->epoch);
         MessageInterface::ShowMessage("Warning: When running estimator '%s', "
            "ionosphere correction is %lf %s "
            "for measurement %s at measurement time tag %.12lf A1Mjd. "
            "Media corrections to the computed measurement may be inaccurate.\n", 
            GetName().c_str(), 
            measData->ionoCorrectValue, unit.c_str(),
            ss1.str().c_str(), measData->epoch);

         // add pass to the list
         ionoWarningList.push_back(ss1.str());
      }

   }

   if (measData->isTropoCorrectWarning)
   {
      // Get measurement path:
      std::stringstream ss1;
      ss1 << "{{";
      for (Integer i = 0; i < measData->participantIDs.size(); ++i)
      {
         ss1 << measData->participantIDs[i] << (((i + 1) < measData->participantIDs.size()) ? "," : "");
      }
      ss1 << "}," << measData->typeName << "}";

      // if the pass is not in warning list, then display warning message
      if (find(tropoWarningList.begin(), tropoWarningList.end(), ss1.str()) == tropoWarningList.end())
      {
         // specify unit of this measurement data
         std::string unit = GetUnit(measData->typeName);

         // generate warning message
         //MessageInterface::ShowMessage("Warning: When running estimator '%s', troposphere correction is %lf m for measurement %s at measurement time tag %.12lf A1Mjd. Media corrections to the computed measurement may be inaccurate.\n", GetName().c_str(), measData->tropoCorrectValue * 1000.0, ss1.str().c_str(), measData->epoch);
         MessageInterface::ShowMessage("Warning: When running estimator '%s', "
            "troposphere correction is %lf %s "
            "for measurement %s at measurement time tag %.12lf A1Mjd. "
            "Media corrections to the computed measurement may be inaccurate.\n", 
            GetName().c_str(), 
            measData->tropoCorrectValue, unit.c_str(),
            ss1.str().c_str(), measData->epoch);

         // add pass to the list
         tropoWarningList.push_back(ss1.str());
      }
   }
}


//------------------------------------------------------------------------------
// void CalculateResiduals(MeasurementInfoType &measStat)
//------------------------------------------------------------------------------
/**
 * This method calculates the residuals.
 *
 * @param measStat The calculated residuals
 */
//------------------------------------------------------------------------------
void Estimator::CalculateResiduals(MeasurementInfoType &measStat)
{
   // Measurements are possible!
   const MeasurementData *calculatedMeas = NULL;

   // Get state map, measurement models, and measurement data
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   modelsToAccess = measManager.GetValidMeasurementList();            // Get valid measurement models
   const ObservationData *currentObs = measManager.GetObsData();      // Get current observation data

   // Perform second level data editing on observation data
   Integer recNum = measManager.GetCurrentRecordNumber();

   Integer dataFilterIndex = -1;
   ObservationData* obData = FilteringData(measManager.GetObsDataObject(), recNum, dataFilterIndex);
   if (obData)
      editedRecords[recNum] = NORMAL_FLAG;
   else
   {
      editedRecords[recNum] = USER_FLAG;
      ++numRemovedRecords["USER"];
   }

   // Get ground station and measurement type from current observation data
   //Uses receive station for report file
   std::string gsName = currentObs->participantIDs[currentObs->participantIDs.size()-1];
   std::string measTypeName = currentObs->typeName;

   // Build vectors of each unique ground station and measurement type
   bool found = false;
   for (UnsignedInt indexKey = 0; indexKey < stationsList.size(); ++indexKey)
   {
      if (stationsList[indexKey] == gsName)
      {
         found = true;
         break;
      }
   }
   if (!found)
      stationsList.push_back(gsName);

   found = false;
   for (UnsignedInt indexKey = 0; indexKey < measTypesList.size(); ++indexKey)
   {
      if (measTypesList[indexKey] == measTypeName)
      {
         found = true;
         break;
      }
   }
   if (!found)
      measTypesList.push_back(measTypeName);

   // Prepare measurement statistics
   measStat.epoch = currentEpochGT;
   measStat.recNum = recNum;
   measStat.modelSize = modelsToAccess.size();
   measStat.station = gsName;
   measStat.type = measTypeName;

   // Add default values that aren't assigned on all logic branches
   measStat.isCalculated = false;
   measStat.uniqueID = 0;
   measStat.feasibilityValue = 0.0;

   //std::string times;
   //Real temp;

   // Verify measurement epoch is inside of EOP time range
   if ((currentObs->epoch < eopTimeMin) || (currentObs->epoch > eopTimeMax))
   {
      if (warningCount == 0)
         MessageInterface::ShowMessage("Warning: measurement epoch %.12lf A1Mjd is outside EOP time range [%.12lf A1Mjd, %.12lf A1Mjd]\n", currentObs->epoch, eopTimeMin, eopTimeMax);
      ++warningCount;
   }

   if (modelsToAccess.size() == 0)
   {
      // Count number of records removed by measurement model unmatched 
      numRemovedRecords["U"]++;
      measManager.GetObsDataObject()->inUsed = false;
      measManager.GetObsDataObject()->removedReason = "U";
      editedRecords[recNum] = editedRecords[recNum] | U_FLAG;          // adding Unmatched flag
   }
   else
   {
      int count = measManager.CountFeasibleMeasurements(modelsToAccess[0]);
      calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);

      // verify media correction to be in acceptable range. It is [0m, 60m] for troposphere correction and [0m, 20m] for ionosphere correction
      ValidateMediaCorrection(calculatedMeas);

      std::string ss = measManager.GetObsDataObject()->removedReason;
      if (OverwriteEditFlag(ss) || calculatedMeas->unfeasibleReason == "EGAP")
         ss = measManager.GetObsDataObject()->removedReason = calculatedMeas->unfeasibleReason;

      if (count == 0)
      {
         // count = 0 when calulation is out of ramp table
         if (ss == "R")
         {
            numRemovedRecords["R"]++;
            editedRecords[recNum] = editedRecords[recNum] | RAMP_FLAG;
         }
         else if (ss == "EGAP")
         {
            numRemovedRecords["EGAP"]++;
            editedRecords[recNum] = editedRecords[recNum] | EGAP_FLAG;           // Adding HORP flag
         }
      }
      else // (count >= 1)
      {
         // Currently assuming uniqueness; modify if more than 1 possible here
         // Case: ((modelsToAccess.size() > 0) && (measManager.Calculate(modelsToAccess[0], true) >= 1))

         // Make corrections to observation values before running data filter
         if ((iterationsTaken == 0) && (currentObs->typeName == "DSN_SeqRange" || currentObs->typeName == "DSN_PNRange"))
         {
            // value correction is only applied for DSN_SeqRange and it is only performed at the first time
            for (Integer index = 0; index < currentObs->value.size(); ++index)
               measManager.GetObsDataObject()->value[index] = ObservationDataCorrection(calculatedMeas->value[index], currentObs->value[index], currentObs->rangeModulo);
         }

         // Calculate the measurement partials
         std::vector<RealArray> hMeas;
         EstimationPartials(hMeas);
         measStat.hAccum = hMeas;

         // Peform Sigma Editing
         const char one = ss[0];
         if(one != 'B' && one != 'U' && one != 'H')
            if (WriteEditFlag())
                DataFilter();

         // Get the new edit flags
         ss = measManager.GetObsDataObject()->removedReason;

         if (ss[0] == 'B')
         {
            numRemovedRecords["B"]++;
            editedRecords[recNum] = editedRecords[recNum] | BLOCKED_FLAG;           // Adding blocked flag
         }
         else if (ss[0] == 'H')
         {
            numRemovedRecords["HORP"]++;
            editedRecords[recNum] = editedRecords[recNum] | HORP_FLAG;           // Adding HORP flag
         }
         else if (ss == "EGAP")
         {
            numRemovedRecords["EGAP"]++;
            editedRecords[recNum] = editedRecords[recNum] | EGAP_FLAG;           // Adding HORP flag
         }
         else if (ss == "IRMS")
         {
            numRemovedRecords["IRMS"]++;
            editedRecords[recNum] = editedRecords[recNum] | IRMS_FLAG;              // Adding IRMS flag
         }
         else if (ss == "OLSE")
         {
            numRemovedRecords["OLSE"]++;
            editedRecords[recNum] = editedRecords[recNum] | OLSE_FLAG;              // Adding OSLE flag
         }
         else if (ss == "SIG")
         {
            numRemovedRecords["SIG"]++;
            editedRecords[recNum] = editedRecords[recNum] | OLSE_FLAG;              // Adding SIG flag
         }
         else if (!(editedRecords[recNum] == USER_FLAG))
            numRemovedRecords[ss]++;                    // Count number of records with Normal flag

         // Get measurement statistics
         for (Integer k = 0; k < currentObs->value.size(); ++k)
         {
            Real ocDiff = currentObs->value[k] - calculatedMeas->value[k];
            if (calculatedMeas->isPeriodic)
               if (fabs(ocDiff) > calculatedMeas->period / 2.0)
                  ocDiff = (calculatedMeas->period - fabs(ocDiff)) * (ocDiff < 0 ? 1 : -1);

            measStat.measValue.push_back(calculatedMeas->value[k]);
            measStat.residual.push_back(ocDiff);
            measStat.weight.push_back(GetMeasurementWeight(k));

				#ifdef DEBUG_O_MINUS_C
				Real OD_Epoch = theTimeConverter->Convert(currentObs->epoch,
					TimeSystemConverter::A1MJD, TimeSystemConverter::TAIMJD,
					GmatTimeConstants::JD_JAN_5_1941);
				Real C_Epoch = theTimeConverter->Convert(calculatedMeas->epoch,
					TimeSystemConverter::A1MJD, TimeSystemConverter::TAIMJD,
					GmatTimeConstants::JD_JAN_5_1941);
				MessageInterface::ShowMessage("Observation data: %s  %s  %s  Epoch = %.12lf  O = %.12le;         Calculated measurement: %s  %s  Epoch = %.12lf  C = %.12le;       O-C = %.12le     frequency = %.12le\n",
					currentObs->typeName.c_str(), currentObs->participantIDs[0].c_str(), currentObs->participantIDs[1].c_str(), OD_Epoch, currentObs->value[k],
					calculatedMeas->participantIDs[0].c_str(), calculatedMeas->participantIDs[1].c_str(), C_Epoch, calculatedMeas->value[k],
					ocDiff, calculatedMeas->uplinkFreq);
				#endif

            #ifdef DEBUG_WEIGHTS
				MessageInterface::ShowMessage("Covariance(%d %d) "
					"is %le\n", k, k, GetMeasurementCovariance(k, k));
				MessageInterface::ShowMessage("editedRecords[recNum = %d] = %d   NORMAL_FLAG = %d\n", recNum, editedRecords[recNum], NORMAL_FLAG);
            #endif

         }

         measStat.tropoCorrectValue = calculatedMeas->tropoCorrectValue;
         measStat.ionoCorrectValue = calculatedMeas->ionoCorrectValue;

      } // end of if (count >= 1)

      measStat.isCalculated = (count >= 1);
      measStat.uniqueID = calculatedMeas->uniqueID;
      measStat.frequency = calculatedMeas->uplinkFreqAtRecei;
      measStat.feasibilityValue = calculatedMeas->feasibilityValue;
      measStat.horpHeight = calculatedMeas->horpHeight;
      measStat.horpAngle = calculatedMeas->horpAngle;


   }  // end of if (modelsToAccess.size() == 0)

   // Write user edited flag
   if (editedRecords[recNum] & USER_FLAG) {
       measManager.GetObsDataObject()->removedReason = "USER";

       // for reporting make sure to reduce counter by one in case of overrided edited flag
       if (editedRecords[recNum] & BLOCKED_FLAG)
          numRemovedRecords["B"]--;
       else if (editedRecords[recNum] & IRMS_FLAG)
          numRemovedRecords["IRMS"]--;
       else if (editedRecords[recNum] & OLSE_FLAG)
          numRemovedRecords["OLSE"]--;
       else if (editedRecords[recNum] & HORP_FLAG)
          numRemovedRecords["HORP"]--;
       else if (editedRecords[recNum] & EGAP_FLAG)
          numRemovedRecords["EGAP"]--;
   }

   measStat.editFlag      = editedRecords[recNum];
   measStat.removedReason = measManager.GetObsDataObject()->removedReason;

}


//------------------------------------------------------------------------------
// void OverwriteEditFlag(const std::string &editFlag)
//------------------------------------------------------------------------------
/**
 * This method indicates if the edit flag is to be overwritten
 *
 * @param editFlag The edit flag that might be overwitten
 */
//------------------------------------------------------------------------------
bool Estimator::OverwriteEditFlag(const std::string &editFlag)
{
   return true;
}


//------------------------------------------------------------------------------
// void WriteEditFlag()
//------------------------------------------------------------------------------
/**
 * This method indicates if a new edit flag is to be written
 */
//------------------------------------------------------------------------------
bool Estimator::WriteEditFlag()
{
   return true;
}


//-------------------------------------------------------------------------
// bool EstimationPartials(std::vector<RealArray> &hMeas)
//-------------------------------------------------------------------------
/**
* Calculates the estimation partials for the current measurement in the
* solve-for states. This does not do any time mapping via the STM,
* but this method can be overridden in a derived class.
*/
//-------------------------------------------------------------------------
void Estimator::EstimationPartials(std::vector<RealArray> &hMeas)
{
	////MessageInterface::ShowMessage("Estimator::EstimationPartials() start\n");
   // Get state map, measurement models, and measurement data
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   const MeasurementData *calculatedMeas = measManager.GetMeasurement(modelsToAccess[0]);
   std::vector<RealArray> stateDeriv;

   RealArray hRow;
   hRow.assign(stateSize, 0.0);
   UnsignedInt rowCount = calculatedMeas->value.size();

   hTilde.clear();
   for (UnsignedInt i = 0; i < rowCount; ++i)
      hTilde.push_back(hRow);
   hMeas.clear();

   // Now walk the state vector and get elements of H-tilde for each piece
   for (UnsignedInt i = 0; i < stateSize; ++i)
   {
      if ((*stateMap)[i]->subelement == 1)
      {
         // Partial derivatives at measurement time
         stateDeriv = measManager.CalculateDerivatives(
            (*stateMap)[i]->object, (*stateMap)[i]->elementID,
            modelsToAccess[0]);

         // Fill in the corresponding elements of hTilde
         // hTilde is partial derivates at measurement time tm (not at t0)
         for (UnsignedInt j = 0; j < rowCount; ++j)
            for (UnsignedInt k = 0; k < (*stateMap)[i]->length; ++k)
               hTilde[j][i+k] = stateDeriv[j][k];
      }
   }

   for (UnsignedInt i = 0; i < rowCount; ++i)
   {
      hRow = hTilde[i];

      // Convert hRow into solve-for coordinates
      RealArray hRowSolveFor(hRow.size(), 0.0);
      for (UnsignedInt ii = 0; ii < hRow.size(); ii++)
      {
         // Specify hAccum vector in Solve-for state
         for (UnsignedInt jj = 0; jj < cart2SolvMatrix.GetNumColumns(); ++jj)
            hRowSolveFor[ii] += hRow[jj] * cart2SolvMatrix(jj, ii);
      }

		////MessageInterface::ShowMessage("Estimator::EstimationPartials(): hRowSolveFor = [");
		////for (Integer i = 0; i < hRowSolveFor.size(); ++i)
		////	MessageInterface::ShowMessage("%.12le   ", hRowSolveFor[i]);
		////MessageInterface::ShowMessage("]\n\n");

      hMeas.push_back(hRowSolveFor);
   }
	////MessageInterface::ShowMessage("Estimator::EstimationPartials() end\n");
}


//------------------------------------------------------------------------------
// void BuildMeasurementLine(const MeasurementInfoType &measStat)
//------------------------------------------------------------------------------
/**
 * This method builds the line for each measurement to the text file.
 *
 * @note The contents of the text file are TBD
 */
//------------------------------------------------------------------------------
void Estimator::BuildMeasurementLine(const MeasurementInfoType &measStat)
{
   const ObservationData *currentObs = measManager.GetObsData(measStat.recNum); // Get current observation data
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();                  // Get state map

   std::stringstream sLine;
   char s[1000];
   std::string timeString;
   std::string ss;
   Real temp;
   std::string linePrefix;

   // Write to report file iteration number, record number, and time:
   bool handleLeapSecond;
   theTimeConverter->Convert("A1ModJulian", currentObs->epoch, "", "UTCGregorian", temp, timeString, 1, &handleLeapSecond);
   if (IsIterative())
      sprintf(&s[0], "%4d %6d  %s  ", iterationsTaken, measStat.recNum, timeString.c_str());
   else
      sprintf(&s[0], "%6d  %s  ", measStat.recNum, timeString.c_str());
   linePrefix = s;

   if (textFileMode != "Normal")
   {
      Real timeTAI = theTimeConverter->Convert(currentObs->epoch, currentObs->epochSystem, TimeSystemConverter::TAIMJD);
      sprintf(&s[0], "%.12lf ", timeTAI);
      linePrefix += s;
   }

   // Write to report file the measurement type

   // Write to report file measurement type name:
   linePrefix += GmatStringUtil::GetAlignmentString(currentObs->typeName, 19) + " ";

   // Write to report file measurement type unit:
   if (textFileMode != "Normal")
      linePrefix += GmatStringUtil::GetAlignmentString(currentObs->unit, 6) + " ";

   // Write to report file all participants in signal path:
   ss = "";
   for (UnsignedInt n = 0; n < currentObs->participantIDs.size(); ++n)
      ss = ss + currentObs->participantIDs[n] + (((n + 1) == currentObs->participantIDs.size()) ? "" : ",");
   linePrefix += GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(ss), pcolumnLen) + " ";


   std::string removedReason = measStat.removedReason;
   std::string elevationAngle = "N/A";

   if (measStat.modelSize > 0)
   {
      // Create elevation angle string
      if (measStat.type == "GPS_PosVec")
         elevationAngle = " ";
      else
      {
         if (measStat.feasibilityValue != -100.0)
         {
            if (textFileMode == "Normal")
               sprintf(&s[0], "%6.2lf", measStat.feasibilityValue);
            else
               sprintf(&s[0], "%18.12lf", measStat.feasibilityValue);

            elevationAngle = s;
         }
      }
   }

   // Other output formats have a wider column for elevation angle
   if (textFileMode == "Normal")
      elevationAngle = GmatStringUtil::GetAlignmentString(elevationAngle, 6, GmatStringUtil::RIGHT);
   else
      elevationAngle = GmatStringUtil::GetAlignmentString(elevationAngle, 18, GmatStringUtil::RIGHT);

   if (measStat.isCalculated && removedReason == "N")
   {
      if (textFileMode == "Normal")
         removedReason = "";
      else
         removedReason = "-";
   }


   for (Integer k = 0; k < currentObs->value.size(); ++k)
   {
      // write a line in report file information about currentObs->value[k]
      sLine << linePrefix;   // write line prefix for each element. The GPS Point Solution has three elements.

      sLine << GmatStringUtil::GetAlignmentString(removedReason, 4, GmatStringUtil::LEFT) + " ";

      if (textFileMode != "Normal")
      {
         sprintf(&s[0], "%21.6lf", currentObs->value_orig[k]);
         sLine << s << " ";
      }

      // Write O-value
      sprintf(&s[0], "%21.6lf", currentObs->value[k]);
      sLine << s << " ";

      // Write C-value, and O-C
      if (!measStat.isCalculated)
      {
         sLine << GmatStringUtil::GetAlignmentString("N/A", 21, GmatStringUtil::RIGHT) << " ";      // C-value
         sLine << GmatStringUtil::GetAlignmentString("N/A", 20, GmatStringUtil::RIGHT) << " ";      // O-C
      }
      else
      {
         sprintf(&s[0], "%21.6lf %20.6lf", measStat.measValue[k], measStat.residual[k]);
         sLine << s << " ";
      }

      if (textFileMode != "Normal")
      {
         // Write W, W*(O-C)^2, and sqrt(W)*|O-C|
         if (!measStat.isCalculated)
         {
            sLine << GmatStringUtil::GetAlignmentString("N/A", 21, GmatStringUtil::RIGHT) << " ";      // W
            sLine << GmatStringUtil::GetAlignmentString("N/A", 21, GmatStringUtil::RIGHT) << " ";      // W*(O-C)^2
            sLine << GmatStringUtil::GetAlignmentString("N/A", 21, GmatStringUtil::RIGHT) << " ";      // sqrt(W)*|O-C|
         }
         else
         {
            sprintf(&s[0], "%21.12le %21.12le %21.12le", measStat.weight[k], measStat.residual[k]*measStat.residual[k]*measStat.weight[k], sqrt(measStat.weight[k])*GmatMathUtil::Abs(measStat.residual[k]));
            sLine << s << " ";
         }
      }

      // Write elevation angle
      sLine << elevationAngle;

      if (textFileMode != "Normal")
      {
         sLine << "    ";

         // Partial derivatives
         if (!measStat.isCalculated)
         {
            // fill out N/A for partial derivative
            for (int i = 0; i < stateMap->size(); ++i)
               sLine << GmatStringUtil::GetAlignmentString("N/A", 19, GmatStringUtil::RIGHT) << " ";   // derivative
         }
         else
         {
            // fill out partial derivative
            for (UnsignedInt p = 0; p < measStat.hAccum[k].size(); ++p)
            {
               Real derivative = measStat.hAccum[k][p] / GetEpsilonConversion(p);
               sLine << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(derivative, false, true, true, 10, 19), 19, GmatStringUtil::RIGHT) << " ";
            }
         }

         if (currentObs->typeName == "DSN_SeqRange" || currentObs->typeName == "DSN_PNRange")
            sprintf(&s[0], "            %d   %.15le   %.15le                     N/A", currentObs->uplinkBand, measStat.frequency, currentObs->rangeModulo);
         else if ((currentObs->typeName == "DSN_TCP") || 
                  (currentObs->typeName == "RangeRate") || 
                  (currentObs->typeName == "SN_Doppler") || 
                  (currentObs->typeName == "SN_Doppler_Rtn") || 
                  (currentObs->typeName == "BRTS_Doppler") ||
                  (currentObs->typeName == "SN_DOWD"))
            sprintf(&s[0],"            %d                     N/A                     N/A                 %.4lf", currentObs->uplinkBand, currentObs->dopplerCountInterval);
         else
            sprintf(&s[0],"          N/A                     N/A                     N/A                     N/A");
         sLine << s;
      }
      

      //Write horp height and central angle
      if (textFileMode != "Normal")
      {
         Real h = 0.0;
         if (measStat.horpHeight != GmatRealConstants::REAL_MAX)
         {
            h = measStat.horpHeight;
            sLine << "    ";
            sprintf(&s[0], "%12.7lf", h);
            sLine << GmatStringUtil::GetAlignmentString(s, 15, GmatStringUtil::RIGHT) << "    ";
            
         }
         else
         {
            sprintf(&s[0], "N/A");
            sLine << GmatStringUtil::GetAlignmentString(s, 19, GmatStringUtil::RIGHT) << "    ";
         }
         if (measStat.horpAngle != GmatRealConstants::REAL_MIN)
         {
            sprintf(&s[0], "%12.9lf", measStat.horpAngle);
            sLine << GmatStringUtil::GetAlignmentString(s, 21, GmatStringUtil::RIGHT) << "  ";
         }
         else
         {
            sprintf(&s[0], "N/A");
            sLine << GmatStringUtil::GetAlignmentString(s, 21, GmatStringUtil::RIGHT) << "    ";
         }
      }

      sLine << "\n";
   }

   linesBuff = sLine.str();
}


//------------------------------------------------------------------------------
// void WriteMeasurementLine(Integer recNum)
//------------------------------------------------------------------------------
/**
 * This method writes the line for each measurement to the text file.
 * The measurement header is also written here, if needed.
 *
 * @note The contents of the text file are TBD
 */
//------------------------------------------------------------------------------
void Estimator::WriteMeasurementLine(Integer recNum)
{
   if ((recNum != 0) && (GmatMathUtil::Mod(recNum, 80) < 0.001))
      WritePageHeader();
   textFile << linesBuff;
   textFile.flush();
}


//------------------------------------------------------------------------------
// bool AddMatData(DataBucket &matData, Integer index)
//------------------------------------------------------------------------------
/**
 * Method used to add this iteration's data to the MATLAB .mat file
 *
 * @param matData    The DataBucket containing the data to add
 * @param index      Index of the structure array to add this data to
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool Estimator::AddMatData(DataBucket &data, Integer index)
{
   bool retval = true;

   // Package the data lists
   std::vector<WriterData*> containers;
   WriterData* writerData;

   // The Real data 2D arrays
   for (UnsignedInt i = 0; i < data.real2DArrayNames.size(); ++i)
   {
      writerData = matWriter->GetContainer(Gmat::REAL_TYPE, data.real2DArrayNames[i]);
      writerData->AddData(data.real2DArrayValues[i]);
      containers.push_back(writerData);
   }

   // The String data 2D arrays
   for (UnsignedInt i = 0; i < data.string2DArrayNames.size(); ++i)
   {
      writerData = matWriter->GetContainer(Gmat::STRING_TYPE, data.string2DArrayNames[i]);
      writerData->AddData(data.string2DArrayValues[i]);
      containers.push_back(writerData);
   }

   // The Real data containers
   for (UnsignedInt i = 0; i < data.realNames.size(); ++i)
   {
      writerData = matWriter->GetContainer(Gmat::REAL_TYPE, data.realNames[i]);
      bool isJagged = data.GetRealContainerSize(i) == 0;

      if (!isJagged)
      {
         // Transpose the array to make it row major
         RealArray temp(data.realValues[i].size(), 0.0);
         std::vector<RealArray> transpose;

         for (UnsignedInt j = 0; j < data.realValues[i][0].size(); ++j)
         {
            for (UnsignedInt k = 0; k < data.realValues[i].size(); ++k)
               temp[k] = data.realValues[i][k][j];

            transpose.push_back(temp);
         }
         data.realValues[i] = transpose;
      }

      writerData->AddData(data.realValues[i], isJagged);
      containers.push_back(writerData);
   }

   // The String data containers
   for (UnsignedInt i = 0; i < data.stringNames.size(); ++i)
   {
      writerData = matWriter->GetContainer(Gmat::STRING_TYPE, data.stringNames[i]);
      std::vector<StringArray> strData;

      #ifdef DEBUG_MAT_WRITER
         MessageInterface::ShowMessage("%s has %d strings\n",
               data.stringNames[i].c_str(), data.stringValues[i].size());
         MessageInterface::ShowMessage("   %d:  %s\n", 0,
               data.stringValues[i][0].c_str());
      #endif

      strData = data.stringValues[i];

      #ifdef DEBUG_MAT_WRITER
         MessageInterface::ShowMessage("   strData has %d entries\n",
               strData.size());
         MessageInterface::ShowMessage("   strData[0] has %d entries\n",
               strData[0].size());
         MessageInterface::ShowMessage("      %d:  %s\n", 0,
               strData[0][0].c_str());
      #endif

      bool isJagged = data.GetStringContainerSize(i) == 0;

      if (!isJagged)
      {
         // Transpose the array to make it row major
         StringArray temp(strData.size(), "");
         std::vector<StringArray> transpose;

         for (UnsignedInt j = 0; j < strData[0].size(); ++j)
         {
            for (UnsignedInt k = 0; k < strData.size(); ++k)
               temp[k] = data.stringValues[i][k][j];

            transpose.push_back(temp);
         }
         strData = transpose;
      }

      writerData->AddData(strData, isJagged);
      containers.push_back(writerData);
   }

   // The Real data 3D containers
   for (UnsignedInt i = 0; i < data.real3DNames.size(); ++i)
   {
      writerData = matWriter->GetContainer(Gmat::REAL_TYPE, data.real3DNames[i]);
      writerData->AddData(data.real3DValues[i]);
      containers.push_back(writerData);
   }

   // The String data 3D containers
   for (UnsignedInt i = 0; i < data.string3DNames.size(); ++i)
   {
      writerData = matWriter->GetContainer(Gmat::STRING_TYPE, data.string3DNames[i]);
      std::vector<std::vector<StringArray>> str3DData;

      str3DData = data.string3DValues[i];

      writerData->AddData(str3DData);
      containers.push_back(writerData);
   }

   // Add it
   matWriter->AddDataBlock();
   for (UnsignedInt i = 0; i < containers.size(); ++i)
      matWriter->AddData(containers[i], index);

   // Clean up for the next pass
   data.Clear();

   return retval;
}


//------------------------------------------------------------------------------
// bool WriteMatData()
//------------------------------------------------------------------------------
/**
 * Method used to write the MATLAB .mat file
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool Estimator::WriteMatData()
{
   bool retval = true;

   MessageInterface::ShowMessage("Writing Estimator MATLAB File...\n");

   StringArray dataDesc;

   std::string varName;

   if (IsIterative())
      varName = "Iteration";
   else
      varName = "Computed";

   dataDesc = GetMatDataDescription(matData);
   matWriter->DescribeData(dataDesc, iterationsTaken);
   matWriter->WriteData(varName);
   
   // Add observation data
   matWriter->ClearData();

   AddMatData(matObsData);

   dataDesc = GetMatDataDescription(matObsData);
   matWriter->DescribeData(dataDesc);
   matWriter->WriteData("Observed");

   // Add estimation config data
   matWriter->ClearData();

   AddMatData(matConfigData);

   dataDesc = GetMatDataDescription(matConfigData);
   matWriter->DescribeData(dataDesc);
   matWriter->WriteData("EstimationConfig");
   matWriter->ClearData();
   
   MessageInterface::ShowMessage("Finished Writing Estimator MATLAB File.\n\n");

   return retval;
}


//------------------------------------------------------------------------------
// StringArray GetMatDataDescription(DataBucket &data)
//------------------------------------------------------------------------------
/**
 * Populates the names of the fields to add to the MATLAB structure
 *
 * @return StringArray of the fields
 */
//------------------------------------------------------------------------------
StringArray Estimator::GetMatDataDescription(DataBucket &data)
{
   StringArray dataDesc;

   // The Real data 2D arrays
   for (UnsignedInt i = 0; i < data.real2DArrayNames.size(); ++i)
      dataDesc.push_back(data.real2DArrayNames[i]);

   // The String data 2D arrays
   for (UnsignedInt i = 0; i < data.string2DArrayNames.size(); ++i)
      dataDesc.push_back(data.string2DArrayNames[i]);

   // The Real data containers
   for (UnsignedInt i = 0; i < data.realNames.size(); ++i)
      dataDesc.push_back(data.realNames[i]);

   // The String data containers
   for (UnsignedInt i = 0; i < data.stringNames.size(); ++i)
      dataDesc.push_back(data.stringNames[i]);

   // The Real data 3D containers
   for (UnsignedInt i = 0; i < data.real3DNames.size(); ++i)
      dataDesc.push_back(data.real3DNames[i]);

   // The String data 3D containers
   for (UnsignedInt i = 0; i < data.string3DNames.size(); ++i)
      dataDesc.push_back(data.string3DNames[i]);

   return dataDesc;
}


//------------------------------------------------------------------------------
//  void AddMatlabData()
//------------------------------------------------------------------------------
/**
 * This method adds the measurement data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void Estimator::AddMatlabData(const MeasurementInfoType &measStat)
{
   if (writeMatFile && (matWriter != NULL))
      AddMatlabData(measStat, matData, matIndex);
}


//------------------------------------------------------------------------------
//  void AddMatlabData()
//------------------------------------------------------------------------------
/**
 * This method adds the measurement data to the MATLAB output file.
 */
 //------------------------------------------------------------------------------
void Estimator::AddMatlabData(const MeasurementInfoType &measStat,
                              DataBucket &matData, IntegerMap &matIndex)
{
   // .mat file index of this measurement
   Integer matMeasIndex;

   // Get state map, measurement models, and measurement data
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   const ObservationData *currentObs = measManager.GetObsData(measStat.recNum);      // Get current observation data

   std::string temp;

   Real utcEpoch;
   Real taiEpoch;
   theTimeConverter->Convert("A1ModJulian", currentObs->epoch, "", "TAIModJulian", taiEpoch, temp, 1);
   theTimeConverter->Convert("A1ModJulian", currentObs->epoch, "", "UTCModJulian", utcEpoch, temp, 1);

   if (matIndex.count("Meas") == 0)
   {
      matIndex["Meas"] = matData.AddRealContainer("Measurement", 0);
      matIndex["MeasNum"] = matData.AddRealContainer("MeasurementNumber", 1);
      if (IsOfType("SeqEstimator"))
         matIndex["Resid"] = matData.AddRealContainer("PreUpdateResidual", 0);
      else
         matIndex["Resid"] = matData.AddRealContainer("Residual", 0);
      matIndex["Elevation"] = matData.AddRealContainer("Elevation", 1);
      matIndex["EditFlag"] = matData.AddStringContainer("MeasurementEditFlag", 1);

      matIndex["Iono"] = matData.AddRealContainer("IonosphericCorrection", 1);
      matIndex["Tropo"] = matData.AddRealContainer("TroposphericCorrection", 1);

      matIndex["Partials"] = matData.AddReal3DContainer("MeasurementPartials");

      addTdrsStats = measStat.horpHeight != GmatRealConstants::REAL_MAX;

      if (addTdrsStats)
      {
         matIndex["Horp"] = matData.AddRealContainer("HeightOfRayPath", 1);
         matIndex["Caorp"] = matData.AddRealContainer("CentralAngleOfRayPath", 1);
      }
   }

   matMeasIndex = matData.AddPoint();

   matData.elementStatus[matMeasIndex] = 0.0;

   if (measStat.modelSize > 0)
   {
      std::string removedReason = measStat.removedReason;

      if (measStat.isCalculated)
      {
         matData.realValues[matIndex["Meas"]][matMeasIndex] = measStat.measValue;
         matData.realValues[matIndex["MeasNum"]][matMeasIndex][0] = measStat.recNum;
         matData.realValues[matIndex["Resid"]][matMeasIndex] = measStat.residual;

         for (UnsignedInt k = 0; k < measStat.hAccum.size(); k++)
         {
            RealArray derivatives = measStat.hAccum[k];

            for (UnsignedInt p = 0; p < derivatives.size(); p++)
               derivatives[p] /= GetEpsilonConversion(p);

            matData.real3DValues[matIndex["Partials"]][matMeasIndex].push_back(derivatives);
         }

         if (measStat.editFlag == NORMAL_FLAG)
            matData.elementStatus[matMeasIndex] = 1.0;

         matData.realValues[matIndex["Tropo"]][matMeasIndex][0] = measStat.tropoCorrectValue;
         matData.realValues[matIndex["Iono"]][matMeasIndex][0] = measStat.ionoCorrectValue;

         if (addTdrsStats)
         {
            if(measStat.horpHeight != GmatRealConstants::REAL_MAX)
               matData.realValues[matIndex["Horp"]][matMeasIndex][0] = measStat.horpHeight;
            else
               matData.realValues[matIndex["Horp"]][matMeasIndex][0] = NAN;

            if (measStat.horpHeight != GmatRealConstants::REAL_MIN)
               matData.realValues[matIndex["Caorp"]][matMeasIndex][0] = measStat.horpAngle;
            else
               matData.realValues[matIndex["Horp"]][matMeasIndex][0] = NAN;
         }
      } // end of if (count >= 1)

   }  // end of if (measStat.modelSize > 0)

   matData.stringValues[matIndex["EditFlag"]][matMeasIndex][0] = measStat.removedReason;

   if (measStat.modelSize > 0)
      matData.realValues[matIndex["Elevation"]][matMeasIndex][0] = measStat.feasibilityValue;


   // Write observation data only on initial iterations
   if (iterationsTaken == 0)
   {
      if (matObsIndex.count("EpochTAI") == 0)
      {
         matObsIndex["EpochTAI"] = matObsData.AddRealContainer("EpochTAI", 2);
         matObsIndex["EpochUTC"] = matObsData.AddRealContainer("EpochUTC", 2);
         matObsIndex["Meas"] = matObsData.AddRealContainer("Measurement", 0);
         matObsIndex["MeasNum"] = matObsData.AddRealContainer("MeasurementNumber", 1);
         matObsIndex["Weight"] = matObsData.AddRealContainer("MeasurementWeight", 0);
         matObsIndex["Part"] = matObsData.AddStringContainer("Participants", 0);
         matObsIndex["Type"] = matObsData.AddStringContainer("MeasurementType", 1);

         matObsIndex["Frequency"] = matObsData.AddRealContainer("Frequency", 1);
         matObsIndex["DoppCount"] = matObsData.AddRealContainer("DopplerCountInterval", 1);
         matObsIndex["RangeModulo"] = matObsData.AddRealContainer("RangeModulo", 1);

         addTdrsStats = measStat.horpHeight != GmatRealConstants::REAL_MAX;
         if (addTdrsStats)
         {
            matObsIndex["TDRSServ"] = matObsData.AddStringContainer("TdrsServiceId", 1);
            matObsIndex["TDRSSmar"] = matObsData.AddRealContainer("TdrsSmarId", 1);
         }
      }

      matMeasIndex = matObsData.AddPoint();

      matObsData.realValues[matObsIndex["EpochTAI"]][matMeasIndex][0] = taiEpoch + MATLAB_DATE_CONVERSION;
      matObsData.realValues[matObsIndex["EpochTAI"]][matMeasIndex][1] = taiEpoch;
      matObsData.realValues[matObsIndex["EpochUTC"]][matMeasIndex][0] = utcEpoch + MATLAB_DATE_CONVERSION;
      matObsData.realValues[matObsIndex["EpochUTC"]][matMeasIndex][1] = utcEpoch;
      matObsData.realValues[matObsIndex["Meas"]][matMeasIndex] = currentObs->value;
      matObsData.realValues[matObsIndex["MeasNum"]][matMeasIndex][0] = measStat.recNum;

      matObsData.stringValues[matObsIndex["Part"]][matMeasIndex] = currentObs->participantIDs;
      matObsData.stringValues[matObsIndex["Type"]][matMeasIndex][0] = currentObs->typeName;

      if (measStat.modelSize > 0)
      {
         if (measStat.isCalculated)
         {
            matObsData.realValues[matObsIndex["Weight"]][matMeasIndex] = measStat.weight;

         } // end of if (count >= 1)

      }  // end of if (measStat.modelSize > 0)

      if (currentObs->typeName == "DSN_SeqRange" || currentObs->typeName == "DSN_PNRange")
      {
         matObsData.realValues[matObsIndex["Frequency"]][matMeasIndex][0] = measStat.frequency;
         matObsData.realValues[matObsIndex["RangeModulo"]][matMeasIndex][0] = currentObs->rangeModulo;
      }
      else if ((currentObs->typeName == "DSN_TCP") ||
         (currentObs->typeName == "RangeRate") ||
         (currentObs->typeName == "SN_Doppler") ||
         (currentObs->typeName == "SN_Doppler_Rtn") ||
         (currentObs->typeName == "BRTS_Doppler") ||
         (currentObs->typeName == "SN_DOWD")
         )
      {
         matObsData.realValues[matObsIndex["DoppCount"]][matMeasIndex][0] = currentObs->dopplerCountInterval;
         matObsData.realValues[matObsIndex["Frequency"]][matMeasIndex][0] = measStat.frequency;
      }

      if (addTdrsStats)
      {
         matObsData.stringValues[matObsIndex["TDRSServ"]][matMeasIndex][0] = currentObs->tdrsServiceID;
         matObsData.realValues[matObsIndex["TDRSSmar"]][matMeasIndex][0] = currentObs->tdrsSMARID;
      }
   }
}


//------------------------------------------------------------------------------
//  void AddMatlabIterationData()
//------------------------------------------------------------------------------
/**
 * This method adds the iteration data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void Estimator::AddMatlabIterationData()
{
   if (writeMatFile && (matWriter != NULL))
   {
      // Get state map, measurement models, and measurement data
      const std::vector<ListItem*> *stateMap = esm.GetStateMap();

      if (matIndex.count("CartState") == 0)
      {
         if (IsIterative())
         {
            matIndex["Iteration"] = matData.AddReal2DArray("IterationNumber");
            matIndex["PreviousCartState"] = matData.AddReal2DArray("PreviousCartesianState");
            matIndex["PreviousKeplState"] = matData.AddReal2DArray("PreviousKeplerianState");
         }
         matIndex["CartState"] = matData.AddReal2DArray("CartesianState");
         matIndex["KeplState"] = matData.AddReal2DArray("KeplerianState");
         matIndex["CartCov"] = matData.AddReal2DArray("CartesianCovariance");
         matIndex["CartCor"] = matData.AddReal2DArray("CartesianCorrelation");
         matIndex["KeplCov"] = matData.AddReal2DArray("KeplerianCovariance");
         matIndex["KeplCor"] = matData.AddReal2DArray("KeplerianCorrelation");
      }

      matData.real2DArrayValues[matIndex["Iteration"]] = { { (Real) iterationsTaken } };

      for (Integer ii = 0; ii < stateMap->size(); ii++)
      {
         if (IsIterative())
         {
            matData.real2DArrayValues[matIndex["PreviousCartState"]].push_back({ previousSolveForStateC[ii] });
            matData.real2DArrayValues[matIndex["PreviousKeplState"]].push_back({ previousSolveForStateK[ii] });
         }
         matData.real2DArrayValues[matIndex["CartState"]].push_back({ currentSolveForStateC[ii] });
         matData.real2DArrayValues[matIndex["KeplState"]].push_back({ currentSolveForStateK[ii] });
      }

      // GTDS MathSpec Eq 8-45, 8-46a, and 8-46b
      Rmatrix dX_dS = cart2SolvMatrix;                               // [dX/dS] matrix, where S is solve-for state. It could Cartesian or Keplerian
      // GTDS MathSpec Eq 8-49
      Rmatrix finalCovariance = dX_dS * informationInverse * dX_dS.Transpose(); // finalCovariance is in Cartesian state

      // 2.3. Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
      CovarianceEpsilonConversion(finalCovariance);

      Rmatrix finalCorrelation(finalCovariance);
      for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
         for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
            finalCorrelation(i, j) /= sqrt(finalCovariance(i, i)*finalCovariance(j, j));

      Rmatrix convmatrix = solv2KeplMatrix.Inverse();                            // GTDS MathSpec Eq 8-45, 8-46a, and 8-46b

      // 4. Write final covariance and correlation matrix for Keplerian coordinate system:
      // 4.1. Calculate covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon
      Rmatrix finalKeplerCovariance = convmatrix * informationInverse * convmatrix.Transpose();          // Equation 8-49 GTDS MathSpec

      // 4.2. Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
      CovarianceEpsilonConversion(finalKeplerCovariance);

      Rmatrix finalKeplerCorrelation(finalKeplerCovariance);
      for (Integer i = 0; i < finalKeplerCovariance.GetNumRows(); ++i)
         for (Integer j = 0; j < finalKeplerCovariance.GetNumColumns(); ++j)
            finalKeplerCorrelation(i, j) /= sqrt(finalKeplerCovariance(i, i)*finalKeplerCovariance(j, j));

      for (Integer ii = 0; ii < stateMap->size(); ii++)
      {
         matData.real2DArrayValues[matIndex["CartCov"]].push_back(finalCovariance.GetRow(ii).GetRealArray());
         matData.real2DArrayValues[matIndex["CartCor"]].push_back(finalCorrelation.GetRow(ii).GetRealArray());
         matData.real2DArrayValues[matIndex["KeplCov"]].push_back(finalKeplerCovariance.GetRow(ii).GetRealArray());
         matData.real2DArrayValues[matIndex["KeplCor"]].push_back(finalKeplerCorrelation.GetRow(ii).GetRealArray());
      }

   }
}


//------------------------------------------------------------------------------
//  void AddMatlabConfigData()
//------------------------------------------------------------------------------
/**
 * This method adds the estimation configuration data to the MATLAB output file.
 */
//------------------------------------------------------------------------------
void Estimator::AddMatlabConfigData()
{
   if (writeMatFile && (matWriter != NULL))
   {
      matConfigIndex["InitialEpoch"] = matConfigData.AddReal2DArray("InitialEpochUTC");
      matConfigIndex["FinalEpoch"] = matConfigData.AddReal2DArray("FinalEpochUTC");

      Real utcEpoch;
      utcEpoch = theTimeConverter->Convert(estimationEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD).GetMjd();
      matConfigData.real2DArrayValues[matConfigIndex["InitialEpoch"]] = { { utcEpoch + MATLAB_DATE_CONVERSION }, { utcEpoch } };
      utcEpoch = theTimeConverter->Convert(currentEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD).GetMjd();
      matConfigData.real2DArrayValues[matConfigIndex["FinalEpoch"]] = { { utcEpoch + MATLAB_DATE_CONVERSION }, { utcEpoch } };

      PropSetup* propSetup = propagators[0];
      ODEModel* ode = propSetup->GetODEModel();
	  if (ode)
	  {
		  std::string cbName = ode->GetStringParameter("CentralBody");
		  CelestialBody* cb = solarSystem->GetBody(cbName);
		  if (cb)
		  {
			  matConfigIndex["Mu"] = matConfigData.AddReal2DArray("GravitationalParameter");
			  matConfigData.real2DArrayValues[matConfigIndex["Mu"]] = { { cb->GetRealParameter(cb->GetParameterID("Mu")) } };
		  }
	  }
   }
}


//--------------------------------------------------------------------------------------
// std::string GetElementName(ListItem* infor, bool isInternalCS) const
//--------------------------------------------------------------------------------------
/**
* This function is used to get a full name of an element as specified by ListItem* infor
*
* @param infor              The ListItem element containing information about the elment
* @param isInternalCS       flag to indicate whether the element is presented in an 
*                           internal coordinate system or not. 
* @param stateType          display state in this specify state type. Its value can be 
*                           "Cartesian", "Keplerian", or "". When stateType is "", state
*                           will be displayed in the state specified in spacecraft 
*                           DispalyStateType parameter.
* @param anomalyType        specify what form anomaly is used. Default value is "TA". 
*                           Valid values are "TA" and "MA".
*
* return                    a string containing element full name
*/
//--------------------------------------------------------------------------------------
std::string Estimator::GetElementName(ListItem* infor, bool isInternalCS,
                                     std::string stateType, std::string anomalyType) const
{
   std::stringstream ss;
   //std::string dispStateType = sinfor->object->GetStringParameter("DisplayStateType");   // This will get error message due to object is not a spacecraft

   //if (infor->elementName == "KeplerianState")
   if ((infor->elementName == "CartesianState") || (infor->elementName == "KeplerianState"))
   {
      std::string dispStateType = infor->object->GetStringParameter("DisplayStateType");
      
      // Case 1: solve-for is a spacecraft state
      if ((stateType == "Keplerian") || ((stateType == "") && (dispStateType == "Keplerian")))
      {
         ss << ((Spacecraft*)(infor->object))->GetRefObject(Gmat::COORDINATE_SYSTEM, "")->GetName() << ".";

         switch(infor->subelement)
         {
         case 1:
            ss << "SMA";
            break;
         case 2:
            ss << "ECC";
            break;
         case 3:
            ss << "INC";
            break;
         case 4:
            ss << "RAAN";
            break;
         case 5:
            ss << "AOP";
            break;
         case 6:
            //ss << "TA";
            ss << anomalyType;
            break;
         }
      }
      else if ((stateType == "Cartesian") || ((stateType == "") && (dispStateType == "Cartesian")))
      {
         ss << ((Spacecraft*)(infor->object))->GetRefObject(Gmat::COORDINATE_SYSTEM, "")->GetName() << ".";

         switch (infor->subelement)
         {
         case 1:
            ss << "X";
            break;
         case 2:
            ss << "Y";
            break;
         case 3:
            ss << "Z";
            break;
         case 4:
            ss << "VX";
            break;
         case 5:
            ss << "VY";
            break;
         case 6:
            ss << "VZ";
            break;
         }
      }
   }
   else if (infor->elementName == "Position")
   {
      switch(infor->subelement)
      {
      case 1:
         ss << "X";
         break;
      case 2:
         ss << "Y";
         break;
      case 3:
         ss << "Z";
         break;
      }
   }
   else if (infor->elementName == "Velocity")
   {
      switch(infor->subelement)
      {
      case 1:
         ss << "VX";
         break;
      case 2:
         ss << "VY";
         break;
      case 3:
         ss << "VZ";
         break;
      }
   }
   else if (infor->elementName == "Cr_Epsilon")
   {
      if (((Spacecraft*)(infor->object))->GetSRPShapeModel() == "Spherical")
         ss << "Cr";
      else if (((Spacecraft*)(infor->object))->GetSRPShapeModel() == "SPADFile")
         ss << "SPADSRPScaleFactor";
   }
   else if (infor->elementName == "Cd_Epsilon")
   {
      if (((Spacecraft*)(infor->object))->GetDragShapeModel() == "Spherical")
         ss << "Cd";
      else if (((Spacecraft*)(infor->object))->GetDragShapeModel() == "SPADFile")
         ss << "SPADDragScaleFactor";
   }
   else if (infor->elementName == "AtmosDensity_Epsilon")
   {
      ss << "AtmosDensityScaleFactor";
   }
   else if (infor->elementName == "Bias")
      ss << "Bias";
   else if (GmatStringUtil::EndsWith(infor->elementName, ".TSF_Epsilon"))          // <segmentName>.TSF_Epsilon
   {
       StringArray parts = GmatStringUtil::SeparateBy(infor->elementName, ".");
       ss << parts.at(0) << ".ThrustScaleFactor";
   }
   else if (GmatStringUtil::EndsWith(infor->elementName, ".ThrustAngle1"))         // <segmentName>.ThrustAngle1
   {
       StringArray parts = GmatStringUtil::SeparateBy(infor->elementName, ".");
       ss << parts.at(0) << ".ThrustAngle1.C" << infor->subelement - 1;
   }
   else if (GmatStringUtil::EndsWith(infor->elementName, ".ThrustAngle2"))         // <segmentName>.ThrustAngle2
   {
       StringArray parts = GmatStringUtil::SeparateBy(infor->elementName, ".");
       ss << parts.at(0) << ".ThrustAngle2.C" << infor->subelement - 1;
   }
   else if (infor->elementName == "FrequencyBias")
   {
      ss << "FrequencyBias.C" << infor->subelement-1;
   }
   else
      ss << infor->elementName << "." << infor->subelement;

   return ss.str();
}


//--------------------------------------------------------------------------------------
// std::string GetElementNameMJ2000Eq(ListItem* infor, bool isInternalCS) const
//--------------------------------------------------------------------------------------
/**
* This function is used to get a full name of an element as specified by ListItem* infor
*
* @param infor              The ListItem element containing information about the elment
* @param isInternalCS       flag to indicate whether the element is presented in an 
*                           internal coordinate system or not. 
* @param stateType          display state in this specify state type. Its value can be 
*                           "Cartesian", "Keplerian", or "". When stateType is "", state
*                           will be displayed in the state specified in spacecraft 
*                           DispalyStateType parameter.
* @param anomalyType        specify what form anomaly is used. Default value is "TA". 
*                           Valid values are "TA" and "MA".
*
* return                    a string containing element full name
*/
//--------------------------------------------------------------------------------------
std::string Estimator::GetElementNameMJ2000Eq(ListItem* infor, bool isInternalCS,
                                              std::string stateType, std::string anomalyType) const
{
   std::stringstream ss;
   //std::string dispStateType = sinfor->object->GetStringParameter("DisplayStateType");   // This will get error message due to object is not a spacecraft

   //if (infor->elementName == "KeplerianState")
   if ((infor->elementName == "CartesianState") || (infor->elementName == "KeplerianState"))
   {
      std::string dispStateType = infor->object->GetStringParameter("DisplayStateType");

      // Case 1: solve-for is a spacecraft state
      if ((stateType == "Keplerian") || ((stateType == "") && (dispStateType == "Keplerian")))
      {
         GmatBase* obj = infor->object;
         std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
         CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
         SpacePoint* origin = cs->GetOrigin();

         ss << origin->GetName() << "MJ2000Eq.";

         switch (infor->subelement)
         {
         case 1:
            ss << "SMA";
            break;
         case 2:
            ss << "ECC";
            break;
         case 3:
            ss << "INC";
            break;
         case 4:
            ss << "RAAN";
            break;
         case 5:
            ss << "AOP";
            break;
         case 6:
            //ss << "TA";
            ss << anomalyType;
            break;
         }
      }
      else if ((stateType == "Cartesian") || ((stateType == "") && (dispStateType == "Cartesian")))
      {
         GmatBase* obj = infor->object;
         std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
         CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
         SpacePoint* origin = cs->GetOrigin();

         ss << origin->GetName() << "MJ2000Eq.";

         switch (infor->subelement)
         {
         case 1:
            ss << "X";
            break;
         case 2:
            ss << "Y";
            break;
         case 3:
            ss << "Z";
            break;
         case 4:
            ss << "VX";
            break;
         case 5:
            ss << "VY";
            break;
         case 6:
            ss << "VZ";
            break;
         }
      }
   }
   else
      ss << GetElementName(infor, isInternalCS, stateType, anomalyType);

   return ss.str();
}


//--------------------------------------------------------------------------------------
// std::string GetElementFullName(ListItem* infor, bool isInternalCS) const
//--------------------------------------------------------------------------------------
/**
* This function is used to get a full name of an element as specified by ListItem* infor
*
* @param infor              The ListItem element containing information about the elment
* @param isInternalCS       flag to indicate whether the element is presented in an
*                           internal coordinate system or not.
* @param stateType          display state in this specify state type. Its value can be
*                           "Cartesian", "Keplerian", or "". When stateType is "", state
*                           will be displayed in the state specified in spacecraft
*                           DispalyStateType parameter.
* @param anomalyType        specify what form anomaly is used. Default value is "TA".
*                           Valid values are "TA" and "MA".
*
* return                    a string containing element full name
*/
//--------------------------------------------------------------------------------------
std::string Estimator::GetElementFullName(ListItem* infor, bool isInternalCS,
   std::string stateType, std::string anomalyType) const
{
   std::stringstream ss;
   ss << infor->objectFullName << ".";

   ss << GetElementName(infor, isInternalCS, stateType, anomalyType);

   return ss.str();
}


//--------------------------------------------------------------------------------------
// std::string GetElementFullNameID(ListItem* infor, bool isInternalCS) const
//--------------------------------------------------------------------------------------
/**
* This function is used to get a full id of an element as specified by ListItem* infor
*
* @param infor              The ListItem element containing information about the elment
* @param isInternalCS       flag to indicate whether the element is presented in an
*                           internal coordinate system or not.
* @param stateType          display state in this specify state type. Its value can be
*                           "Cartesian", "Keplerian", or "". When stateType is "", state
*                           will be displayed in the state specified in spacecraft
*                           DispalyStateType parameter.
* @param anomalyType        specify what form anomaly is used. Default value is "TA".
*                           Valid values are "TA" and "MA".
*
* return                    a string containing element full name
*/
//--------------------------------------------------------------------------------------
std::string Estimator::GetElementFullNameID(ListItem* infor, bool isInternalCS,
   std::string stateType, std::string anomalyType) const
{
   std::stringstream ss;

   try
   {
      ss << infor->object->GetStringParameter("Id") << ".";
   }
   catch (GmatBaseException &ex)
   {
      // If the object doesn't have an Id parameter, and isn't an ErrorModel, just use its name
      ss << infor->objectFullName << ".";
   }

   ss << GetElementName(infor, isInternalCS, stateType, anomalyType);

   return ss.str();
}


//--------------------------------------------------------------------------------------
// std::string GetElementFullNameMJ2000Eq(ListItem* infor, bool isInternalCS) const
//--------------------------------------------------------------------------------------
/**
* This function is used to get a full name of an element as specified by ListItem* infor
*
* @param infor              The ListItem element containing information about the elment
* @param isInternalCS       flag to indicate whether the element is presented in an
*                           internal coordinate system or not.
* @param stateType          display state in this specify state type. Its value can be
*                           "Cartesian", "Keplerian", or "". When stateType is "", state
*                           will be displayed in the state specified in spacecraft
*                           DispalyStateType parameter.
* @param anomalyType        specify what form anomaly is used. Default value is "TA".
*                           Valid values are "TA" and "MA".
*
* return                    a string containing element full name
*/
//--------------------------------------------------------------------------------------
std::string Estimator::GetElementFullNameMJ2000Eq(ListItem* infor, bool isInternalCS,
   std::string stateType, std::string anomalyType) const
{
   std::stringstream ss;
   ss << infor->objectFullName << ".";

   ss << GetElementNameMJ2000Eq(infor, isInternalCS, stateType, anomalyType);

   return ss.str();
}


//--------------------------------------------------------------------------------------
// std::string GetElementUnit(ListItem* infor, std::string stateType) const
//--------------------------------------------------------------------------------------
/**
* Get element unit
*
* @param infor         The ListItem element containing information about the elment
* @param stateType     Specified state type. Its value can be "Cartesian", "Keplerian", 
*                      or "". When stateType is "", state type is specified in 
*                      spacecraft's DispalyStateType parameter.
*
* @return              unit of the element
*/
//--------------------------------------------------------------------------------------
std::string Estimator::GetElementUnit(ListItem* infor, std::string stateType) const
{
   std::string unit = "";

   if ((infor->elementName == "CartesianState") || (infor->elementName == "KeplerianState"))
   {
      std::string dispStateType = infor->object->GetStringParameter("DisplayStateType");
      
      if ((stateType == "Keplerian") ||
         ((stateType == "") &&
         (dispStateType == "Keplerian")))
      {
         switch (infor->subelement)
         {
         case 1:
            unit = "km";
            break;
         case 2:
            break;
         case 3:
         case 4:
         case 5:
         case 6:
            unit = "deg";
            break;
         }
      }
      else if ((stateType == "Cartesian") || ((stateType == "") && (dispStateType == "Cartesian")))
      {
         switch (infor->subelement)
         {
         case 1:
         case 2:
         case 3:
            unit = "km";
            break;
         case 4:
         case 5:
         case 6:
            unit = "km/s";
            break;
         }
      }
   }
   else if (infor->elementName == "Position")
   {
      switch (infor->subelement)
      {
      case 1:
      case 2:
      case 3:
         unit = "km";
         break;
      case 4:
      case 5:
      case 6:
         unit = "km/s";
         break;
      }
   }
   else if (infor->elementName == "Velocity")
   {
      switch (infor->subelement)
      {
      case 1:
      case 2:
      case 3:
         unit = "km/s";
         break;
      }
   }
   else if ((infor->elementName == "Bias") || (infor->elementName == "PassBiases"))
   {
      if (infor->object->IsOfType(Gmat::MEASUREMENT_MODEL))
      {
         // Get full name for Bias
         //MeasurementModel* mm = (MeasurementModel*)(infor->object);
         TrackingDataAdapter* mm = (TrackingDataAdapter*)(infor->object);

         // Get Bias unit:
         if (mm->IsOfType("DSN_SeqRange"))
            unit = "RU";
         else if (mm->IsOfType("DSN_PNRange"))
             unit = "RU";
         else if (mm->IsOfType("DSN_TCP"))
            unit = "Hz";
         else if (mm->IsOfType("Range"))
            unit = "km";
         else if (mm->IsOfType("RangeRate"))
            unit = "km/s";
         else if (mm->IsOfType("SN_Range"))
            unit = "km";
         else if (mm->IsOfType("SN_Doppler"))
            unit = "Hz";
         else if (mm->IsOfType("SN_Doppler_Rtn"))
            unit = "Hz";
         else if (mm->IsOfType("SN_DOWD"))
            unit = "Hz";
         else if (mm->IsOfType("BRTS_Range"))
            unit = "km";
         else if (mm->IsOfType("BRTS_Doppler"))
            unit = "Hz";
         else
            unit = "****";
      }
      else
      {
         // Get Bias unit:
         unit = GetUnit(infor->object->GetStringParameter("Type"));
      }
   }
   else if ((infor->elementName == "Cr_Epsilon") || (infor->elementName == "Cd_Epsilon")
      || (infor->elementName == "AtmosDensity_Epsilon"))
      unit = "";
   else if (GmatStringUtil::EndsWith(infor->elementName, ".TSF_Epsilon"))         // <segmentName>.TSF_Epsilon
       unit = "";
   else if (GmatStringUtil::EndsWith(infor->elementName, ".ThrustAngle1") ||
            GmatStringUtil::EndsWith(infor->elementName, ".ThrustAngle2"))
   {
      unit = "deg";
      Integer pow = infor->subelement - 1;
      if (pow >= 1)
      {
         unit = unit + "/s";

         if (pow >= 2)
            unit = unit + GmatStringUtil::ToString(pow, 0);
      }
   }
   else if (infor->elementName == "FrequencyBias")
   {
      if (infor->subelement == 1)
      {
         unit = "Hz";
      }
      else if (infor->subelement == 2)
      {
         unit = "Hz/s";
      }
      else
      {
         unit = "Hz/s^" + std::to_string(infor->subelement-1);
      }
   }
   //@ todo: code to specify unit of other solve-for parameters is added here

   return unit;
}


//--------------------------------------------------------------------------------------
// Integer GetElementPrecision(std::string unit) const
//--------------------------------------------------------------------------------------
/**
* Get element precision associated with th given unit
*
* @param unit      a measurement unit
*
* @return          precision asscociated with the unit
*/
//--------------------------------------------------------------------------------------
Integer Estimator::GetElementPrecision(std::string unit) const
{
   // parameter's precision is specified based on its unit 
   int precision = 8;

   if ((unit == "km") || (unit == "RU") || (unit == "Hz"))
      precision = 8;                                                      // 6;
   else if ((unit == "km/s") || (unit == "deg") || (unit == "km2/s2"))
      precision = 8;
   else if (unit == "min")
      precision = 8;                                                      // 6;
   else if ((unit == "min/day") || (unit == "deg/day"))
      precision = 8;
   else if (GmatStringUtil::StartsWith(unit, "deg/s"))
      precision = 8;
   else if (unit == "")
      precision = 8;
   //@ todo: code to specify precision of other solve-for parameters is added here

   return precision;
}


//------------------------------------------------------------------------------
// void WriteToTextFile(Solver::SolverState)
//------------------------------------------------------------------------------
/**
 * This method adds text to the estimator text file.
 *
 * @note The contents of the text file are TBD
 *
 * @param solverState The current state from the finite state machine
 */
//------------------------------------------------------------------------------
void Estimator::WriteToTextFile(Solver::SolverState sState)
{
   // Only write to report file when ReportStyle is Normal or Verbose
//   if ((textFileMode != "Normal")&&(textFileMode != "Verbose"))
//      return;
#ifdef DEBUG_WRITE_TO_TEXT_FILE
   MessageInterface::ShowMessage("Estimator::WriteToTextFile() enter: sState = %d\n", sState);
#endif

   GmatState outputEstimationState;

   if (!showProgress)
      return;
   
   if (!textFile.is_open())
      OpenSolverTextFile();
   
   Solver::SolverState theState = sState;
   if (sState == Solver::UNDEFINED_STATE)
      theState = currentState;

   const std::vector<ListItem*> *map = esm.GetStateMap();

   textFile.setf(std::ios::fixed, std::ios::floatfield);
   
   switch (theState)
   {
      case INITIALIZING:
      {
         #ifdef DEBUG_WRITE_TO_TEXT_FILE
         MessageInterface::ShowMessage("Estimator::WriteToTextFile(): theState = INITIALIZING\n");
         #endif

         WriteReportFileHeader();
         #ifdef DEBUG_WRITE_TO_TEXT_FILE
         MessageInterface::ShowMessage("Estimator::WriteToTextFile(): theState = INITIALIZING completed\n");
         #endif
         break;
      }
      case ACCUMULATING:
      {
         #ifdef DEBUG_WRITE_TO_TEXT_FILE
         MessageInterface::ShowMessage("Estimator::WriteToTextFile(): theState = ACCUMULATING\n");
         #endif

         WriteMeasurementLine(measManager.GetCurrentRecordNumber());
         #ifdef DEBUG_WRITE_TO_TEXT_FILE
         MessageInterface::ShowMessage("Estimator::WriteToTextFile(): theState = ACCUMULATING completed\n");
         #endif
         break;
      }
      case ESTIMATING:
      {
         #ifdef DEBUG_WRITE_TO_TEXT_FILE
         MessageInterface::ShowMessage("Estimator::WriteToTextFile(): theState = ESTIMATING\n");
         #endif

         WriteReportFileSummary(theState);
         #ifdef DEBUG_WRITE_TO_TEXT_FILE
         MessageInterface::ShowMessage("Estimator::WriteToTextFile(): theState = ESTIMATING completed\n");
         #endif
         break;
      }
      case CHECKINGRUN:
      {
         #ifdef DEBUG_WRITE_TO_TEXT_FILE
         MessageInterface::ShowMessage("Estimator::WriteToTextFile(): theState = CHECKINGRUN\n");
         #endif

         WriteReportFileSummary(theState);
         textFile << textFile0.str() << textFile1.str() << textFile1_1.str() << textFile2.str() << textFile3.str() << textFile4.str() << textFile5.str();
         textFile0.str(""); textFile1.str(""); textFile1_1.str(""); textFile2.str(""); textFile3.str(""); textFile4.str(""); textFile5.str("");
         WriteIterationHeader();

         #ifdef DEBUG_WRITE_TO_TEXT_FILE
         MessageInterface::ShowMessage("Estimator::WriteToTextFile(): theState = CHECKINGRUN completed\n");
         #endif
         break;
      }
      case FINISHED:
      {
         #ifdef DEBUG_WRITE_TO_TEXT_FILE
         MessageInterface::ShowMessage("Estimator::WriteToTextFile(): theState = FINISHED\n");
         #endif

         WriteReportFileSummary(theState);
         textFile << textFile0.str() << textFile1.str() << textFile1_1.str() << textFile2.str() << textFile3.str() << textFile4.str() << textFile5.str();
         textFile0.str(""); textFile1.str(""); textFile1_1.str(""); textFile2.str(""); textFile3.str(""); textFile4.str(""); textFile5.str("");
         #ifdef DEBUG_WRITE_TO_TEXT_FILE
         MessageInterface::ShowMessage("Estimator::WriteToTextFile(): theState = FINISHED completed\n");
         #endif
         break;
      }
      default:
         break;
   }

#ifdef DEBUG_WRITE_TO_TEXT_FILE
   MessageInterface::ShowMessage("Estimator::WriteToTextFile() exit: sState = %d\n", sState);
#endif
}



//----------------------------------------------------------------------------
// std::string GetFileCreateTime(std::string fileName)
//----------------------------------------------------------------------------
/**
* This function is used to get build date and time for a given file.
*
* @param  fileName       name of a file
*
* @return                a string containning build date and time
*/
//----------------------------------------------------------------------------
std::string Estimator::GetFileCreateTime(std::string fileName)
{
   std::string time;
   struct stat sb;
   if (stat(fileName.c_str(), &sb) == -1)
   {
      MessageInterface::ShowMessage("Error:: Cannot get build date for file '%s'\n", fileName.c_str());
      return "";
   }

   return CTime(&sb.st_ctime);
}


//----------------------------------------------------------------------------
// std::string CTime(const time_t* time)
//----------------------------------------------------------------------------
/**
* This function is used to convert time from type time_t to string.
*
* @param time       time in time_t type
*
* @return           a string containning date and time
*/
//----------------------------------------------------------------------------
std::string Estimator::CTime(const time_t* time)
{
   char* dt = ctime(time);
   std::string s;
   s.assign(dt);
   size_t pos = s.find_last_of(' ');
   std::string syear = s.substr(pos + 1, 4);
   s = s.substr(0, pos);

   pos = s.find_last_of(' ');
   std::string stime = s.substr(pos + 1);
   s = s.substr(0, pos);

   s = s + ", " + syear + " " + stime;

   return s;
}


//------------------------------------------------------------------------------
// std::string GetGMATBuildDate()
//------------------------------------------------------------------------------
/**
* Get GMAT build date
*/
//------------------------------------------------------------------------------
std::string Estimator::GetGMATBuildDate()
{
   // std::istringstream s(__DATE__);
   std::istringstream s(GmatGlobal::Instance()->GetGMATBuildDate().c_str());

   std::string smonth;
   Integer day, month, year;
   s >> smonth >> day >> year;
   switch(smonth.at(0))
   {
   case 'J':
      if (smonth == "Jun")
         month = 6;
      else if (smonth == "Jul")
         month = 7;
      else
         month = 1;
      break;
   case 'F':
      month = 2;
      break;
   case 'M':
      if (smonth == "May")
         month = 5;
      else
         month = 3;
      break;
   case 'A':
      if (smonth == "Apr")
         month = 4;
      else
         month = 8;
      break;
   case 'S':
      month = 9;
      break;
   case 'O':
      month = 10;
      break;
   case 'N':
      month = 11;
      break;
   case 'D':
      month = 12;
      break;
   }

   std::string sday = GetDayOfWeek(day, month, year);

   std::stringstream ss;
   // ss << sday << " " << smonth << " " << day << ", " << year << " " << __TIME__;
   ss << sday << " " << smonth << " " << day << ", " << year << " " << GmatGlobal::Instance()->GetGMATBuildTime();
   return ss.str();
}


//------------------------------------------------------------------------------
// std::string GetDayOfWeek(Integer day, Integer month, Integer year)
//------------------------------------------------------------------------------
/**
* Specify day of week for a given date month/day/year
*/
//------------------------------------------------------------------------------
std::string Estimator::GetDayOfWeek(Integer day, Integer month, Integer year)
{

   Integer daysOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

   // Specify number of days of February
   if ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)))
      daysOfMonth[1] = 29;

   // Calculate number of days from day, month, year to 01/01/0001
   Integer y = year - 1;
   Integer m = month - 1;
   Integer d = day - 1;
   Integer days = 365*y + (int)(y / 4) - (int)(y / 100) + (int)(y / 400);   // total number of days from 01/01/y  to 01/01/0001
   for (Integer i = 0; i < m; ++i)
      days += daysOfMonth[i];                          // total number of days from m/01/y   to 01/01/0001
   days += d;                                          // total number of days from m/d/y    to 01/01/0001
   
   // Calculate weekday
   Integer weekdayOffset = 1;
   Integer weekday = days % 7 + weekdayOffset;

   if (weekday > 6)
      weekday -= 7;

   std::string sweekday;
   switch (weekday)
   {
   case 0:
      sweekday = "Sunday";
      break;
   case 1:
      sweekday = "Monday";
      break;
   case 2:
      sweekday = "Tuesday";
      break;
   case 3:
      sweekday = "Wednesday";
      break;
   case 4:
      sweekday = "Thursday";
      break;
   case 5:
      sweekday = "Friday";
      break;
   case 6:
      sweekday = "Saturday";
      break;
   }
   
   return sweekday;
}


//----------------------------------------------------------------------------
// std::string GetOperatingSystemName()
//----------------------------------------------------------------------------
/**
* This function is used to get computer operating system name.
*
* @return        name of OS
*/
//----------------------------------------------------------------------------
std::string Estimator::GetOperatingSystemName()
{
   std::string osName = "";
#ifdef __linux__
   struct utsname uts;
   if (uname(&uts) == -1)
      throw EstimatorException("Error: cannot get OS information\n");
   
   osName.assign(uts.sysname);
#else
   #ifdef __APPLE__
   struct utsname uts;
   if (uname(&uts) == -1)
      throw EstimatorException("Error: cannot get OS information\n");

   osName.assign(uts.sysname);

   #endif
#endif

#ifdef _MSC_VER
   osName = "Windows";
#endif

   return osName;
}


//----------------------------------------------------------------------------
// std::string GetOperatingSystemVersion()
//----------------------------------------------------------------------------
/**
* This function is used to get computer operating system version.
*
* @return        version of OS
*/
//----------------------------------------------------------------------------
std::string Estimator::GetOperatingSystemVersion()
{
   std::string osVersion = "";
#ifdef __linux__
   struct utsname uts;
   if (uname(&uts) == -1)
      throw EstimatorException("Error: cannot get OS information\n");

   osVersion.assign(uts.version);
#else
#ifdef __APPLE__
   struct utsname uts;
   if (uname(&uts) == -1)
      throw EstimatorException("Error: cannot get OS information\n");

   osVersion.assign(uts.version);

#endif
#endif

#ifdef _MSC_VER
   DWORD version = 0;
   DWORD maVersion = 0;
   DWORD miVersion = 0;

   version = GetVersion();
   maVersion = (DWORD)(LOBYTE(LOWORD(version)));
   miVersion = (DWORD)(HIBYTE(LOWORD(version)));

   char s[100];
   sprintf(&s[0], "%d.%d", maVersion, miVersion);
   osVersion.assign(s);
#endif

   return osVersion;
}


//----------------------------------------------------------------------------
// std::string GetHostName()
//----------------------------------------------------------------------------
/**
* This function is used to get computer name.
*
* @return   name of computer on which GMAT runs. 
*/
//----------------------------------------------------------------------------
std::string Estimator::GetHostName()
{
   std::string hostName = "";
   char s[256];
#ifdef __linux__
   gethostname(s, sizeof(s));
#else
#ifdef __APPLE__
   gethostname(s, sizeof(s));
#endif
#endif

#ifdef _MSC_VER
   gethostname(s, sizeof(s));
#endif

   hostName.assign(s);
   return hostName.c_str();
}


//----------------------------------------------------------------------------
// std::string GetUserID()
//----------------------------------------------------------------------------
/**
* This function is used to get computer user ID.
*
* @return   computer user ID 
*/
//----------------------------------------------------------------------------
std::string Estimator::GetUserID()
{
   std::string userName = "";
   
#ifdef __linux__
   char *name;
   name = getlogin();
   if (name != NULL)
      userName.assign(name);
#else
#ifdef __APPLE__
   char *name;
   name = getlogin();
   if (name != NULL)
      userName.assign(name);
#endif
#endif

#ifdef _MSC_VER
   char s[256];
   char name[128];
   DWORD size = sizeof(s);
   GetUserName((LPTSTR)s,&size);
   for (UnsignedInt i = 0; i < size; ++i)
      name[i] = s[2 * i];
   userName.assign(name);
#endif

   return userName;
}


//----------------------------------------------------------------------------
// void WriteReportFileHeader()
//----------------------------------------------------------------------------
/**
* This function writes estimation report header. It contains 6 parts:
*      . Part 1: contains information about GMAT build, OS, user infor
*      . Part 2: contains batch least squares initial conditions
*      . Part 2b: contains apriori covariance matrix
*      . Part 3: contains information about orbit generator
*      . Part 4: contains information about measurements
*      . Part 5: contains information about astrodynamic constants
*      . Part 6: contains information about estimation options
*/
//----------------------------------------------------------------------------
void Estimator::WriteReportFileHeader()
{
   
   WriteReportFileHeaderPart1();
   WriteReportFileHeaderPart2();
   WriteReportFileHeaderPart2b();
   WriteReportFileHeaderPart2c();
   WriteReportFileHeaderPart3();
   WriteReportFileHeaderPart4();
   WriteReportFileHeaderPart5();
   WriteReportFileHeaderPart6();
   WriteIterationHeader();
}


//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart1()
//------------------------------------------------------------------------------
/**
* This function is used to write GMAT release, build, OS information, and user
* information to report file.
*/
//------------------------------------------------------------------------------
void Estimator::WriteReportFileHeaderPart1()
{
   /// 1. Write header 1:
   time_t now = time(NULL);
   std::string runDate = CTime(&now);
   //std::string buildTime = GetFileCreateTime("GMAT.exe");
   std::string buildTime = GetGMATBuildDate();
   std::string version = GmatGlobal::Instance()->GetGmatVersion();
   
   textFile
      << "\n"
      << "                                                     G E N E R A L  M I S S I O N  A N A L Y S I S  T O O L       \n"
      << "\n"
      << "                                                                          Release " << version << "\n"
      << GmatStringUtil::GetAlignmentString("", 59) + "Build Date : " << buildTime << "\n"
      << "\n"
      << GmatStringUtil::GetAlignmentString("", 36) + "Hostname : " << GmatStringUtil::GetAlignmentString(GetHostName(), 36, GmatStringUtil::LEFT) << " OS / Arch : " << GetOperatingSystemName() << " " << GetOperatingSystemVersion() << "\n"
      << GmatStringUtil::GetAlignmentString("", 36) + "User ID  : "
      << GmatStringUtil::GetAlignmentString(GetUserID(), 36, GmatStringUtil::LEFT)
      << " Run Date  : " << runDate << "\n"
      << "\n"
      << "\n";

   textFile.flush();
}


//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart2()
//------------------------------------------------------------------------------
/**
* This function is used to write batch least square initial information to the 
* report file.
*/
//------------------------------------------------------------------------------
void Estimator::WriteReportFileHeaderPart2()
{
   // 1. Write state at beginning iteration:
   textFile
      << "***************************************************************  SPACECRAFT INITIAL CONDITIONS  ****************************************************************\n"
      << "\n"
      << " Spacecraft State at Beginning of Estimation :\n"
      << "\n";

   // 2. Fill in parameter name:
   StringArray paramNames;
   paramNames.push_back("Spacecraft Name");
   paramNames.push_back("ID");
   paramNames.push_back("");
   paramNames.push_back("Epoch (UTC)");
   paramNames.push_back("Coordinate System");
   paramNames.push_back("X  (km)");
   paramNames.push_back("Y  (km)");
   paramNames.push_back("Z  (km)");
   paramNames.push_back("VX (km/s)");
   paramNames.push_back("VY (km/s)");
   paramNames.push_back("VZ (km/s)");
   paramNames.push_back("Cr");
   paramNames.push_back("CrSigma");
   paramNames.push_back("Cd");
   paramNames.push_back("CdSigma");
   paramNames.push_back("DryMass  (kg)");
   paramNames.push_back("DragArea (m^2)");
   paramNames.push_back("SRPArea  (m^2)");

   paramNames.push_back("SPADSRPScaleFactor");
   paramNames.push_back("SPADSRPScaleFactorSigma");
   paramNames.push_back("SPADDragScaleFactor");
   paramNames.push_back("SPADDragScaleFactorSigma");
   paramNames.push_back("AtmosDensityScaleFactor");
   paramNames.push_back("AtmosDensityScaleFactorSigma");

   Integer dragAreaIndex = 16;
   Integer srpAreaIndex = 17;
   // 3. Write a table containing spacecraft initial condition:
   std::vector<StringArray> spacecraftValues;
   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   for (UnsignedInt i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);

      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         bool found;
         Real val;
         std::string s;
         StringArray paramValues;

         // 3.1. Get a spacecraft for processing:
         Spacecraft *sc = (Spacecraft *)obj;

         // 3.2. Fill in parameter's value and unit:
         paramValues.push_back(sc->GetName());
         paramValues.push_back(sc->GetStringParameter("Id"));
         paramValues.push_back("");
         paramValues.push_back(sc->GetUTCEpochString());
         paramValues.push_back(sc->GetStringParameter("CoordinateSystem"));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("X"), false, false, true, 8, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("Y"), false, false, true, 8, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("Z"), false, false, true, 8, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("VX"), false, false, true, 12, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("VY"), false, false, true, 12, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("VZ"), false, false, true, 12, 22));

         s = GmatStringUtil::RealToString(sc->GetRealParameter("Cr"), false, false, true, 6, 22);
         if (s.find('.') == s.npos)
            s = s + ".00";
         paramValues.push_back(s);

         StringArray solveforList = obj->GetStringArrayParameter("SolveFors");
         ObjectArray estimatedParameterList = obj->GetRefObjectArray(GmatType::GetTypeId("EstimatedParameter"));
         found = false;
         for (Integer j = 0; j < solveforList.size(); ++j)
         {
            if (solveforList[j] == "Cr")
            {
               found = true;
               break;
            }
         }
         for (Integer j = 0; j < estimatedParameterList.size(); ++j)
         {
            if (estimatedParameterList[j]->GetStringParameter("SolveFor") == "Cr")
            {
               found = true;
               break;
            }
         }
         if (found)
         {
            val = sc->GetRealParameter("CrSigma");
            if ((1.0e-5 < val) && (val < 1.0e10))
               s = GmatStringUtil::RealToString(val, false, false, true, 6, 22);
            else
               s = GmatStringUtil::RealToString(val, false, true, true, 6, 22);
         }
         else
            s = "Not Estimated";
         paramValues.push_back(s);


         s = GmatStringUtil::RealToString(sc->GetRealParameter("Cd"), false, false, true, 6, 22);
         if (s.find('.') == s.npos)
            s = s + ".00";
         paramValues.push_back(s);

         found = false;
         for (Integer j = 0; j < solveforList.size(); ++j)
         {
            if (solveforList[j] == "Cd")
            {
               found = true;
               break;
            }
         }
         for (Integer j = 0; j < estimatedParameterList.size(); ++j)
         {
            if (estimatedParameterList[j]->GetStringParameter("SolveFor") == "Cd")
            {
               found = true;
               break;
            }
         }
         if (found)
         {
            val = sc->GetRealParameter("CdSigma");
            if ((1.0e-5 < val) && (val < 1.0e10))
               s = GmatStringUtil::RealToString(val, false, false, true, 6, 22);
            else
               s = GmatStringUtil::RealToString(val, false, true, true, 6, 22);
         }
         else
            s = "Not Estimated";
         paramValues.push_back(s);

         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("DryMass"), false, false, true, 6, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("DragArea"), false, false, true, 6, 22));
         paramValues.push_back(GmatStringUtil::RealToString(sc->GetRealParameter("SRPArea"), false, false, true, 6, 22));


         s = GmatStringUtil::RealToString(sc->GetRealParameter("SPADSRPScaleFactor"), false, false, true, 6, 22);
         if (s.find('.') == s.npos)
            s = s + ".00";
         paramValues.push_back(s);

         found = false;
         for (Integer j = 0; j < solveforList.size(); ++j)
         {
            if (solveforList[j] == "SPADSRPScaleFactor")
            {
               found = true;
               break;
            }
         }
         if (found)
         {
            val = sc->GetRealParameter("SPADSRPScaleFactorSigma");
            if ((1.0e-5 < val) && (val < 1.0e10))
               s = GmatStringUtil::RealToString(val, false, false, true, 6, 22);
            else
               s = GmatStringUtil::RealToString(val, false, true, true, 6, 22);
         }
         else
            s = "Not Estimated";
         paramValues.push_back(s);


         s = GmatStringUtil::RealToString(sc->GetRealParameter("SPADDragScaleFactor"), false, false, true, 6, 22);
         if (s.find('.') == s.npos)
            s = s + ".00";
         paramValues.push_back(s);

         found = false;
         for (Integer j = 0; j < solveforList.size(); ++j)
         {
            if (solveforList[j] == "SPADDragScaleFactor")
            {
               found = true;
               break;
            }
         }
         if (found)
         {
            val = sc->GetRealParameter("SPADDragScaleFactorSigma");
            if ((1.0e-5 < val) && (val < 1.0e10))
               s = GmatStringUtil::RealToString(val, false, false, true, 6, 22);
            else
               s = GmatStringUtil::RealToString(val, false, true, true, 6, 22);
         }
         else
            s = "Not Estimated";
         paramValues.push_back(s);


         s = GmatStringUtil::RealToString(sc->GetRealParameter("AtmosDensityScaleFactor"), false, false, true, 6, 22);
         if (s.find('.') == s.npos)
            s = s + ".00";
         paramValues.push_back(s);

         found = false;
         for (Integer j = 0; j < solveforList.size(); ++j)
         {
            if (solveforList[j] == "AtmosDensityScaleFactor")
            {
               found = true;
               break;
            }
         }
         for (Integer j = 0; j < estimatedParameterList.size(); ++j)
         {
            if (estimatedParameterList[j]->GetStringParameter("SolveFor") == "AtmosDensityScaleFactor")
            {
               found = true;
               break;
            }
         }
         if (found)
         {
            val = sc->GetRealParameter("AtmosDensityScaleFactorSigma");
            if ((1.0e-5 < val) && (val < 1.0e10))
               s = GmatStringUtil::RealToString(val, false, false, true, 6, 22);
            else
               s = GmatStringUtil::RealToString(val, false, true, true, 6, 22);
         }
         else
            s = "Not Estimated";
         paramValues.push_back(s);

         // 3.3. Add information of the spacecraft to spacecarftValues vector
         spacecraftValues.push_back(paramValues);
         paramValues.clear();
      }
   }

   // 4. Specify name len
   Integer nameLen = 0;
   for (UnsignedInt i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());
   int maxCol = 5;// ((160 - 26) - (nameLen + 2)) / 26;

   // 5. Get row index for SPADSRPScaleFactorSigma and SPADDragScaleFactorSigma
   UnsignedInt srpFactorIndex = 0;
   while (paramNames[srpFactorIndex] != "SPADSRPScaleFactorSigma")
      ++srpFactorIndex;

   UnsignedInt dragFactorIndex = 0;
   while (paramNames[dragFactorIndex] != "SPADDragScaleFactorSigma")
      ++dragFactorIndex;

   UnsignedInt atmosDensityFactorIndex = 0;
   while (paramNames[atmosDensityFactorIndex] != "AtmosDensityScaleFactorSigma")
      ++atmosDensityFactorIndex;

   // 6. Write out content
   StringArray rowContent;

   bool disableDragContent = true;
   bool disableSRPContent = true;
   bool disableADSFContent = true;
   for (UnsignedInt scCount = 0, colCount = 0; scCount < spacecraftValues.size(); ++scCount)
   {
      // 6.1. for each spacecraft, write out all its contents on each column      
      colCount++;
      for (UnsignedInt j = 0; j < paramNames.size(); ++j)
      {
         if (j == rowContent.size())
            rowContent.push_back("");
         rowContent[j] += (GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(spacecraftValues[scCount][j]), 25, GmatStringUtil::RIGHT) + " ");

         // Set flags to write content of SPADSRPScaleFactor and SPADDragScaleFactor 
         if ((j == srpFactorIndex) && (spacecraftValues[scCount][j] != "Not Estimated"))
            disableSRPContent = false;
         if ((j == dragFactorIndex) && (spacecraftValues[scCount][j] != "Not Estimated"))
            disableDragContent = false;
         if ((j == atmosDensityFactorIndex) && (spacecraftValues[scCount][j] != "Not Estimated"))
            disableADSFContent = false;
      }

      // 6.2. Beak up columns in a table 
      //if ((nameLen + 2 + colCount * 26) > (160 - 26))
      if ((colCount == maxCol) || (scCount == spacecraftValues.size()-1))
      {
         // Recalculate name len
         Integer nLen = 0;
         for (UnsignedInt i = 0; i < paramNames.size(); ++i)
         {
            if ((!disableDragContent) && (i == dragAreaIndex))
               continue;

            if ((!disableSRPContent) && (i == srpAreaIndex))
               continue;

            if ((disableDragContent) && ((i == dragFactorIndex) || (i == dragFactorIndex - 1)))
               continue;

            if ((disableSRPContent) && ((i == srpFactorIndex) || (i == srpFactorIndex - 1)))
               continue;

            if ((disableADSFContent) && ((i == atmosDensityFactorIndex) || (i == atmosDensityFactorIndex - 1)))
               continue;

            nLen = (Integer)GmatMathUtil::Max(nLen, paramNames[i].size());
         }
         nLen = 28;
         for (UnsignedInt j = 0; j < rowContent.size(); ++j)
         {
            if ((!disableDragContent) && (j == dragAreaIndex))
               continue;

            if ((!disableSRPContent) && (j == srpAreaIndex))
               continue;

            if ((disableDragContent) && ((j == dragFactorIndex) || (j == dragFactorIndex - 1)))
                  continue;

            if ((disableSRPContent) && ((j == srpFactorIndex) || (j == srpFactorIndex - 1)))
               continue;

            if ((disableADSFContent) && ((j == atmosDensityFactorIndex) || (j == atmosDensityFactorIndex - 1)))
               continue;

            rowContent[j] = (" " + GmatStringUtil::GetAlignmentString(paramNames[j], nLen, GmatStringUtil::LEFT) + " ") + rowContent[j];
            textFile << rowContent[j] << "\n";
         }
         textFile << "\n";
         textFile << "\n";

         rowContent.clear();
         colCount = 0;
         disableDragContent = true;
         disableSRPContent = true;
      }
   }
   
   textFile.flush();

   // Clean up memory
   paramNames.clear();
   for (UnsignedInt i = 0; i < spacecraftValues.size(); ++i)
      spacecraftValues[i].clear();
   spacecraftValues.clear();
}


//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart2b()
//------------------------------------------------------------------------------
/**
* This function is used to write apriori covariance matrix to report file.
*/
//------------------------------------------------------------------------------
void Estimator::WriteReportFileHeaderPart2b()
{
   // 1.1 Write subheader
   textFile
      << "***************************************************************  ESTIMATION INITIAL CONDITIONS  ****************************************************************\n"
      << "\n";

   // 1.2. Prepare for writing
   const std::vector<ListItem*> *map = esm.GetStateMap();

   Rmatrix aprioriCov = *(stateCovariance->GetCovariance());

   // Convert covariance from CrEpsilon to Cr, from CdEpsilon to Cd
   CovarianceEpsilonConversion(aprioriCov);

   // covariance matrix from MJ2000Eq to Spacecraft CoordinateSystem
   Rmatrix covar = aprioriCov;
   CartesianCovarianceRotation(covar, false);

   // 2. Write standard deviation
   // 2.1. Specify maximum len of elements' names (Cartesian element names)
   Integer max_len = 27;         // 27 is the maximum lenght of ancillary element names
   for (int i = 0; i < map->size(); ++i)
   {
      std::stringstream ss;
      if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL)) &&
         ((*map)[i]->elementName == "Bias"))
      {
         TrackingDataAdapter* mm = (TrackingDataAdapter*)((*map)[i]->object);
         StringArray sa = mm->GetStringArrayParameter("Participants");
         ss << mm->GetStringParameter("Type") << " ";
         for (UnsignedInt j = 0; j < sa.size(); ++j)
            ss << sa[j] << (((j + 1) != sa.size()) ? "," : " Bias.");
         ss << (*map)[i]->subelement;
      }
      else
      {
         std::string solveforState = "Catersian";
         if ((*map)[i]->elementName == "KeplerianState")
         {
            solveforState = "Keplerian";
            ss << GetElementFullNameMJ2000Eq((*map)[i], false, solveforState, "MA");
         }
         else
            ss << GetElementFullName((*map)[i], false, solveforState, "MA");
      }
      max_len = (Integer)GmatMathUtil::Max(max_len, ss.str().size());
   }
   textFile << " " << GmatStringUtil::GetAlignmentString("State Component", max_len+3) 
            << " " << GmatStringUtil::GetAlignmentString("Units", 8) 
            << GmatStringUtil::GetAlignmentString("Apriori State", 19, GmatStringUtil::RIGHT)
            << GmatStringUtil::GetAlignmentString("Standard Dev.", 19, GmatStringUtil::RIGHT)
            << "\n";
   textFile << "\n";

   // 2.2. Write a table containing solve-for name, unit, apriori  state, and standard deviation
   for (Integer i = 0; i < map->size(); ++i)
   {
      std::string solveforState = "";

      std::stringstream ss;
      if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL)) &&
         ((*map)[i]->elementName == "Bias"))
      {
         // Get full name for Bias
         TrackingDataAdapter* mm = (TrackingDataAdapter*)((*map)[i]->object);
         StringArray sa = mm->GetStringArrayParameter("Participants");
         ss << mm->GetStringParameter("Type") << " ";
         for (UnsignedInt j = 0; j < sa.size(); ++j)
            ss << sa[j] << (((j + 1) != sa.size()) ? "," : " Bias.");
         ss << (*map)[i]->subelement;

         // Get Bias unit. It is km for (Range_KM) Range, RU for DSN_SeqRange, km/s for (Doppler_RangeRate) RangeRate, and Hz for DSN_TCP 
      }
      else
      {
         // Get full name for Bias
         solveforState = "Cartesian";
         if ((*map)[i]->elementName == "KeplerianState")
         {
            solveforState = "Keplerian";
            ss << GetElementFullNameMJ2000Eq((*map)[i], false, solveforState, "MA");
         }
         else
            ss << GetElementFullName((*map)[i], false, solveforState, "MA");
      }

      std::string unit = GetElementUnit((*map)[i], solveforState);
      int precision = GetElementPrecision(unit);

      textFile << GmatStringUtil::ToString(i + 1, 3);
      textFile << " ";
      textFile << GmatStringUtil::GetAlignmentString(ss.str(), max_len + 1, GmatStringUtil::LEFT);
      textFile << GmatStringUtil::GetAlignmentString(unit, 8, GmatStringUtil::LEFT);

      if ((*map)[i]->elementName == "KeplerianState")
         textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(aprioriSolveForStateK[i], false, false, true, precision, 18)), 19, GmatStringUtil::RIGHT);                         // apriori state in mean anomaly form
      else
         textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(aprioriSolveForStateC[i], false, false, true, precision, 18)), 19, GmatStringUtil::RIGHT);
      textFile << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(covar(i,i)), false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);     // standard deviation
      textFile << "\n";
   }
   textFile << "\n";


   // 3. write apriori covariance matrix
   textFile
      //      << "                                                    Apriori Covariance Matrix in Cartesian Coordinate System\n"
      << GmatStringUtil::GetAlignmentString("Apriori Covariance Matrix (MJ2000Eq Axes)", 160, GmatStringUtil::CENTER) << "\n"
      << "\n";
   textFile << WriteMatrixReport(aprioriCov, true);


   // Write Stochastic Parameters
   if (GmatType::GetTypeId("EstimatedParameter") != Gmat::UNKNOWN_OBJECT)
   {
      ObjectArray objs;
      esm.GetStateObjects(objs);

      std::vector<std::pair<GmatBase*, GmatBase*>> estParams;
      for (UnsignedInt ii = 0U; ii < objs.size(); ii++)
      {
         ObjectArray params = objs[ii]->GetRefObjectArray(GmatType::GetTypeId("EstimatedParameter"));

         for (UnsignedInt jj = 0U; jj < params.size(); jj++)
         {
            std::pair<GmatBase*, GmatBase*> pair(objs[ii], params[jj]);
            estParams.push_back(pair);
         }
      }

      // Write table containing EstimatedParameter's information
      if (estParams.size() > 0U)
      {
         textFile
            << GmatStringUtil::GetAlignmentString("Stochastic Parameters", 160, GmatStringUtil::CENTER) << "\n"
            << "\n";
      }

      StringArray paramNames, paramValues, rowContent;
      Integer colCount = 0;
      Integer colLimit = 3;
      Integer colWidth = 50;

      // Fill in parameter's name
      paramNames.clear();
      paramNames.push_back("Name");
      paramNames.push_back("Solve For");
      paramNames.push_back("Model");

      // Set value to paramValues
      for (Integer i = 0; i < estParams.size(); ++i)
      {
         // Get EstimatedParameter object
         GmatBase *estParam = estParams[i].second;
         std::string estParamModel = estParam->GetStringParameter("Model");

         // Fill in parameter's value
         paramValues.push_back(estParam->GetName());                                                            // Name
         paramValues.push_back(estParams[i].first->GetName() + "." + estParam->GetStringParameter("SolveFor")); // SolveFor
         paramValues.push_back(estParamModel);                                                                   // Model

         if (estParamModel == "FirstOrderGaussMarkov")
         {
            paramNames.push_back("Half Life (s)");
            paramNames.push_back("SteadyStateSigma");
            paramNames.push_back("SteadyStateValue");

            paramValues.push_back(GmatStringUtil::RealToString(estParam->GetRealParameter("HalfLife"), false, false, false, 8));
            paramValues.push_back(GmatStringUtil::RealToString(estParam->GetRealParameter("SteadyStateSigma"), false, false, false, 8));
            paramValues.push_back(GmatStringUtil::RealToString(estParam->GetRealParameter("SteadyStateValue"), false, false, false, 8));
         }

         // Write information of the process noise model to the column
         for (UnsignedInt j = 0; j < paramNames.size(); ++j)
         {
            if (j == rowContent.size())
               rowContent.push_back(GmatStringUtil::PadWithBlanks("", (colWidth + 3)*colCount));

            rowContent[j] += " " + paramNames[j] + "  ";
            Integer width = colWidth - (paramNames[j].size() + 2);
            rowContent[j] += GmatStringUtil::GetAlignmentString(paramValues[j], width, GmatStringUtil::RIGHT) + "  ";
         }

         colCount++;

         // Beak up columns in a table
         if (colCount == colLimit || i + 1 == estParams.size())
         {
            for (UnsignedInt j = 0; j < rowContent.size(); ++j)
               textFile << GmatStringUtil::Trim(rowContent[j], GmatStringUtil::TRAILING) << "\n";
            textFile << "\n";

            rowContent.clear();
            colCount = 0;
         }

         paramNames.clear();
         paramNames.push_back("Name");
         paramNames.push_back("Solve For");
         paramNames.push_back("Model");

         // Clear paramValues and paramUnits
         paramValues.clear();
      }

      // Clean up memory
      paramNames.clear();
      paramValues.clear();
      rowContent.clear();
   }

   textFile.flush();

}


//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart2c()
//------------------------------------------------------------------------------
/**
* This function is used to write information about spacecraft hardware to 
* estimation report file.
*/
//------------------------------------------------------------------------------
//********************************************************************  SPACECRAFT HARDWARE  *********************************************************************
//
//Spacecraft Name                      TDRS10
//
//Transponders
//Name                              XPNDR - A
//Primary Antenna                       HGA
//Hardware Delay(m)                123.456
//Turnaround Ratio                  221 / 240
//
//Antennas
//Name                                  HGA

void Estimator::WriteReportFileHeaderPart2c()
{
   // 1. Write header for this section:
   textFile
      << "********************************************************************  SPACECRAFT HARDWARE  *********************************************************************\n"
      << "\n";

   // 2. Write information spacecraft hardware a string array buffer
   StringArray participantNames = GetMeasurementManager()->GetParticipantList();

   StringArray buffer;
   Integer columncount = 0;
   Integer startrow = 0;
   Integer currentrow = 0;
   Integer columnwidth = 25;
   std::string pad = "";

   // Set Up Spacecraft Hardware categories
   StringArray spacecraftHardwarePrint;
   StringArray spacecraftNames;
   StringArray spacecraftAntennas;
   StringArray spacecraftTransmitters;
   StringArray spacecraftReceivers;
   StringArray errorModels;
   StringArray spacecraftTransponders;
   StringArray spacecraftMisc;

   spacecraftNames.push_back(" Spacecraft Name             ");
   spacecraftNames.push_back("");
   int scCount = 0;
   int freqSize = -1;
   //Figure out how many freqBias transmitter lines there needs to be
   for (Integer i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);

      //Figure out how many freqBias transmitter lines there needs to be
      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         Spacecraft* sc = (Spacecraft*)obj;
         ObjectArray hwList = sc->GetRefObjectArray("Hardware");
         for (Integer j = 0; j < hwList.size(); ++j)
         {
            if (hwList[j]->IsOfType("Transmitter"))
            {
               int fs = ((Transmitter*)hwList[j])->GetRvectorParameter("FrequencyBias").GetSize();
               if (fs > freqSize)
                  freqSize = fs;
            }
         }
      }
   }
   for (Integer i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);

      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         Spacecraft* sc = (Spacecraft*)obj;
         if (spacecraftNames.size() == 0)
         {
            spacecraftNames.push_back(" Spacecraft Name             ");
            spacecraftNames.push_back("");
         }
         scCount++;
         spacecraftNames[0] = spacecraftNames[0] + GmatStringUtil::GetAlignmentString(sc->GetName(), columnwidth, GmatStringUtil::LEFT);

         int antCount = 0;
         int transmCount = 0;
         int recCount = 0;
         int transpCount = 0;
         int miscCount = 0;


         // 2.2. Write properties of spacecraft hardware:
         ObjectArray hwList = sc->GetRefObjectArray("Hardware");
         for (Integer j = 0; j < hwList.size(); ++j)
         {
            
            if (hwList[j]->IsOfType("Antenna"))
            {
               // 2.2.1. Write properties for Antenna
               // Currently antenna object has no property

               if (antCount * 3 >= spacecraftAntennas.size())
               {
                  spacecraftAntennas.push_back(" Name                        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount-1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(hwList[j]->GetName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftAntennas.push_back(" Type                        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount-1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(hwList[j]->GetTypeName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftAntennas.push_back("");
               }
               else
               {
                  spacecraftAntennas[antCount*3] =      (spacecraftAntennas[antCount * 3] + GmatStringUtil::GetAlignmentString(hwList[j]->GetName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftAntennas[antCount * 3 +1] = (spacecraftAntennas[antCount * 3 +1] + GmatStringUtil::GetAlignmentString(hwList[j]->GetTypeName(), columnwidth, GmatStringUtil::LEFT));
               }
               antCount++;
            }
            else if (hwList[j]->IsOfType("Transmitter"))
            {
               // 2.2.2. Write properties for Transmiter
               Transmitter *tm = (Transmitter*)hwList[j];
               std::string pAnt = tm->GetStringParameter("PrimaryAntenna");
               std::string refEp = tm->GetStringParameter("ReferenceEpoch");
               Real freq = tm->GetRealParameter("Frequency");
               std::string freqB = tm->GetStringParameter("FrequencyBand");
               Rvector freqBias = tm->GetRvectorParameter("FrequencyBias");
               Rvector freqBiasSigma = tm->GetRvectorParameter("FrequencyBiasSigma");
               StringArray sf = tm->GetStringArrayParameter("SolveFors");
               int fs = freqBias.GetSize();

               if (transmCount * (8 + (freqSize*2)) >= spacecraftTransmitters.size())
               {
                  spacecraftTransmitters.push_back(" Name                        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(hwList[j]->GetName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransmitters.push_back(" Type                        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(hwList[j]->GetTypeName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransmitters.push_back(" Primary Antenna             " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(pAnt, columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransmitters.push_back(" ReferenceEpoch              " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(refEp, columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransmitters.push_back(" Frequency (MHz)             " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(freq, false, false, true, 8, 20), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransmitters.push_back(" Frequency Band              " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(freqB, columnwidth, GmatStringUtil::LEFT));
                  //Only one solvefor for now so sf[0] is fine
                  if (sf.size() == 0)
                     sf.push_back("None");
                  spacecraftTransmitters.push_back(" Solve Fors                  " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(sf[0], columnwidth, GmatStringUtil::LEFT));
                  for (int k = 0; k < freqSize; k++)
                  {
                     if (k == 0)
                        spacecraftTransmitters.push_back(" Bias C" + std::to_string(k) + " (Hz)                ");
                     else if (k == 1)
                        spacecraftTransmitters.push_back(" Bias C" + std::to_string(k) + " (Hz/sec)            ");
                     else
                        spacecraftTransmitters.push_back(" Bias C" + std::to_string(k) + " (Hz/sec^" + std::to_string(k) + ")          ");

                     if(k < fs)
                        spacecraftTransmitters[spacecraftTransmitters.size() - 1] += GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(freqBias[k], false, true, true, 8, 20), columnwidth, GmatStringUtil::LEFT);
                     else
                        spacecraftTransmitters[spacecraftTransmitters.size() - 1] += GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT);
                     
                     if (k == 0)
                        spacecraftTransmitters.push_back(" Bias Sigma C" + std::to_string(k) + " (Hz)          ");
                     else if (k == 1)
                        spacecraftTransmitters.push_back(" Bias Sigma C" + std::to_string(k) + " (Hz/sec)      ");
                     else
                        spacecraftTransmitters.push_back(" Bias Sigma C" + std::to_string(k) + " (Hz/sec^" + std::to_string(k) + ")    ");
                  
                     if (k < fs)
                        spacecraftTransmitters[spacecraftTransmitters.size() - 1] += GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(freqBiasSigma[k], false, true, true, 8, 20), columnwidth, GmatStringUtil::LEFT);
                     else
                        spacecraftTransmitters[spacecraftTransmitters.size() - 1] += GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT);

                     }
                  spacecraftTransmitters.push_back("");
               }
               else
               {
                  spacecraftTransmitters[transmCount * (8 + (freqSize * 2))] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2))] + GmatStringUtil::GetAlignmentString(hwList[j]->GetName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 1] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 1] + GmatStringUtil::GetAlignmentString(hwList[j]->GetTypeName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 2] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 2] + GmatStringUtil::GetAlignmentString(pAnt, columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 3] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 3] + GmatStringUtil::GetAlignmentString(refEp, columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 4] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 4] + GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(freq, false, false, true, 8, 20), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 5] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 5] + GmatStringUtil::GetAlignmentString(freqB, columnwidth, GmatStringUtil::LEFT));
                  //Only one solvefor for now so sf[0] is fine
                  if (sf.size() == 0)
                     sf.push_back("None");
                  spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 6] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 6] + GmatStringUtil::GetAlignmentString(sf[0], columnwidth, GmatStringUtil::LEFT));
                  for (int k = 0; k < freqSize; k++)
                  {
                     if (k < fs)
                     {
                        spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 7 + (k*2)] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 7 + (k * 2)] + GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(freqBias[k], false, true, true, 8, 20), columnwidth, GmatStringUtil::LEFT));
                        spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 8 + (k * 2)] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 8 + (k * 2)] + GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(freqBiasSigma[k], false, true, true, 8, 20), columnwidth, GmatStringUtil::LEFT));
                     }
                     else
                     {
                        spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 7 + (k * 2)] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 7 + (k * 2)] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
                        spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 8 + (k * 2)] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 8 + (k * 2)] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
                     }
                  }
               }
               transmCount++;
            }
            else if (hwList[j]->IsOfType("Receiver"))
            {
               // 2.2.3. Write properties for Receiver
               Receiver *rc = (Receiver*)hwList[j];
               std::string id = rc->GetStringParameter("Id");
               StringArray errorModelsList = rc->GetStringArrayParameter("ErrorModels");
               std::string vals = "{";
               for (Integer index = 0; index < errorModelsList.size(); ++index)
               {
                  vals = vals + errorModelsList[index];
                  if (index < errorModelsList.size() - 1)
                     vals = vals + ",";
               }
               vals = vals + "}";

               if (recCount * 4 >= spacecraftReceivers.size())
               {
                  spacecraftReceivers.push_back(" Name                        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(hwList[j]->GetName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftReceivers.push_back(" Type                        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(hwList[j]->GetTypeName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftReceivers.push_back(" Receiver Id                 " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(id, columnwidth, GmatStringUtil::LEFT));
                  spacecraftReceivers.push_back("");
               }
               else
               {
                  spacecraftReceivers[recCount * 4]     = (spacecraftReceivers[recCount * 4]     + GmatStringUtil::GetAlignmentString(hwList[j]->GetName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftReceivers[recCount * 4 + 1] = (spacecraftReceivers[recCount * 4 + 1] + GmatStringUtil::GetAlignmentString(hwList[j]->GetTypeName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftReceivers[recCount * 4 + 2] = (spacecraftReceivers[recCount * 4 + 2] + GmatStringUtil::GetAlignmentString(id, columnwidth, GmatStringUtil::LEFT));
               }
               recCount++;

               // Write Error Models out
               if (errorModels.size() == 0)
               {
                  for (Integer index = 0; index < errorModelsList.size(); ++index)
                  {
                     vals = errorModelsList[index];
                     if (index == 0)
                     {
                        errorModels.push_back(" Error Models                " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(vals, columnwidth, GmatStringUtil::LEFT));
                     }
                     else
                     {
                        errorModels.push_back("                             " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(vals, columnwidth, GmatStringUtil::LEFT));
                     }

                  }
                  errorModels.push_back("");
               }
               else
               {
                  int errModCount = 0;
                  for (Integer counter = 0; counter < errorModels.size(); counter++)
                  {

                     if (errorModels[counter].substr(0, 29) == " Error Models                ")
                     {
                        errModCount++;
                     }
                     if (errModCount == recCount)
                     {
                        for (Integer index = 0; index < errorModelsList.size(); ++index)
                        {
                           if (errorModels[counter] != "")
                           {
                              vals = errorModelsList[index];
                              errorModels[counter] = (errorModels[counter] + GmatStringUtil::GetAlignmentString(vals, columnwidth, GmatStringUtil::LEFT));
                              counter++;
                           }
                           else
                           {
                              vals = errorModelsList[index];
                              StringArray filler = { ("                             " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(vals, columnwidth, GmatStringUtil::LEFT)) };
                              errorModels.insert(errorModels.begin() + counter, filler.begin(), filler.end());
                              counter++;
                           }
                        }
                     }
                     else if (counter == errorModels.size() - 1 && errModCount < recCount)
                     {
                        for (Integer index = 0; index < errorModelsList.size(); ++index)
                        {
                           vals = errorModelsList[index];
                           if (index == 0)
                           {
                              errorModels.push_back(" Error Models                " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(vals, columnwidth, GmatStringUtil::LEFT));
                           }
                           else
                           {
                              errorModels.push_back("                             " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(vals, columnwidth, GmatStringUtil::LEFT));
                           }
                        }
                        errorModels.push_back("");
                     }
                  }
               }
            }
            else if (hwList[j]->IsOfType("Transponder"))
            {
               // 2.2.3. Write properties for Transponder
               Transponder*tp = (Transponder*)hwList[j];
               std::string pAnt = tp->GetStringParameter("PrimaryAntenna");
               Real tpDelay = tp->GetRealParameter("HardwareDelay");
               std::string ratio = tp->GetStringParameter("TurnAroundRatio");

               if (transpCount * 6 >= spacecraftTransponders.size())
               {
                  spacecraftTransponders.push_back(" Name                        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(hwList[j]->GetName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransponders.push_back(" Type                        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(hwList[j]->GetTypeName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransponders.push_back(" Primary Antenna             " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(pAnt, columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransponders.push_back(" Hardware Delay (sec)        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(tpDelay, false, true, true, 8,20), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransponders.push_back(" Turnaround Ratio            " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(ratio, columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransponders.push_back("");
               }
               else
               {
                  spacecraftTransponders[transpCount * 6]     = (spacecraftTransponders[transpCount * 6]     + GmatStringUtil::GetAlignmentString(hwList[j]->GetName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransponders[transpCount * 6 + 1] = (spacecraftTransponders[transpCount * 6 + 1] + GmatStringUtil::GetAlignmentString(hwList[j]->GetTypeName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransponders[transpCount * 6 + 2] = (spacecraftTransponders[transpCount * 6 + 2] + GmatStringUtil::GetAlignmentString(pAnt, columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransponders[transpCount * 6 + 3] = (spacecraftTransponders[transpCount * 6 + 3] + GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(tpDelay, false, true, true, 8, 20), columnwidth, GmatStringUtil::LEFT));
                  spacecraftTransponders[transpCount * 6 + 4] = (spacecraftTransponders[transpCount * 6 + 4] + GmatStringUtil::GetAlignmentString(ratio, columnwidth, GmatStringUtil::LEFT));
               }
               transpCount++;


            }
            else
            {
               // 2.2.5 Write Properties for any additional Misc. Hardware that may be present.
               if (miscCount * 3 >= spacecraftMisc.size())
               {
                  spacecraftMisc.push_back(" Name                        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(hwList[j]->GetName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftMisc.push_back(" Type                        " + GmatStringUtil::GetAlignmentString("", columnwidth*(scCount - 1) + scCount - 1, GmatStringUtil::LEFT) + GmatStringUtil::GetAlignmentString(hwList[j]->GetTypeName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftMisc.push_back("");
               }
               else
               {
                  spacecraftMisc[miscCount * 3]     = (spacecraftMisc[miscCount * 3] + GmatStringUtil::GetAlignmentString(hwList[j]->GetName(), columnwidth, GmatStringUtil::LEFT));
                  spacecraftMisc[miscCount * 3 + 1] = (spacecraftMisc[miscCount * 3 + 1] + GmatStringUtil::GetAlignmentString(hwList[j]->GetTypeName(), columnwidth, GmatStringUtil::LEFT));
               }
               miscCount++;
            }
         }
         

         // Fill in blank spaces if this hardware item is not on this SC
         while (antCount * 3 < spacecraftAntennas.size())
         {
            spacecraftAntennas[antCount * 3] = (spacecraftAntennas[antCount * 3] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftAntennas[antCount * 3 + 1] = (spacecraftAntennas[antCount * 3 + 1] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            antCount++;
         }
         while (transmCount * (8 +(freqSize*2)) < spacecraftTransmitters.size())
         {
            spacecraftTransmitters[transmCount * (8 + (freqSize * 2))] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2))] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 1] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 1] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 2] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 2] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 3] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 3] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 4] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 4] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 5] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 5] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 6] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 6] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            for (int k = 0; k < freqSize; k++)
            {
               spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 7] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 7] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
               spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 8] = (spacecraftTransmitters[transmCount * (8 + (freqSize * 2)) + 8] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            }
            transmCount++;
         }
         while (recCount * 4 < spacecraftReceivers.size())
         {
            spacecraftReceivers[recCount * 4] = (spacecraftReceivers[recCount * 4] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftReceivers[recCount * 4 + 1] = (spacecraftReceivers[recCount * 4 + 1] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftReceivers[recCount * 4 + 2] = (spacecraftReceivers[recCount * 4 + 2] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            recCount++;
         }
         while (transpCount * 6 < spacecraftTransponders.size())
         {
            spacecraftTransponders[transpCount * 6] = (spacecraftTransponders[transpCount * 6] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftTransponders[transpCount * 6 + 1] = (spacecraftTransponders[transpCount * 6 + 1] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftTransponders[transpCount * 6 + 2] = (spacecraftTransponders[transpCount * 6 + 2] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftTransponders[transpCount * 6 + 3] = (spacecraftTransponders[transpCount * 6 + 3] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftTransponders[transpCount * 6 + 4] = (spacecraftTransponders[transpCount * 6 + 4] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            transpCount++;
         }
         while (miscCount * 3 < spacecraftMisc.size())
         {
            spacecraftMisc[miscCount * 3] = (spacecraftMisc[miscCount * 3] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            spacecraftMisc[miscCount * 3 + 1] = (spacecraftMisc[miscCount * 3 + 1] + GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT));
            miscCount++;
         }
         for (Integer errModCount = 0; errModCount < errorModels.size(); errModCount++)
         {
            if (errorModels[errModCount] != "" && errorModels[errModCount].length() < scCount*columnwidth +29)
            {
               errorModels[errModCount] += GmatStringUtil::GetAlignmentString("", columnwidth, GmatStringUtil::LEFT);
            }
         }

         // Add buffer space to columns
         bool allDone = false;
         for (Integer blanker = 0; allDone == false; blanker++)
         {
            if (blanker < spacecraftNames.size() && spacecraftNames[blanker] != "")
            {
               spacecraftNames[blanker] = spacecraftNames[blanker] + " ";
            }
            if (blanker < spacecraftAntennas.size() && spacecraftAntennas[blanker] != "")
            {
               spacecraftAntennas[blanker] = spacecraftAntennas[blanker] + " ";
            }
            if (blanker < spacecraftTransmitters.size() && spacecraftTransmitters[blanker] != "")
            {
               spacecraftTransmitters[blanker] = spacecraftTransmitters[blanker] + " ";
            }
            if (blanker < spacecraftReceivers.size() && spacecraftReceivers[blanker] != "")
            {
               spacecraftReceivers[blanker] = spacecraftReceivers[blanker] + " ";
            }
            if (blanker < spacecraftTransponders.size() && spacecraftTransponders[blanker] != "")
            {
               spacecraftTransponders[blanker] = spacecraftTransponders[blanker] + " ";
            }
            if (blanker < errorModels.size() && errorModels[blanker] != "")
            {
               errorModels[blanker] = errorModels[blanker] + " ";
            }
            if (blanker < spacecraftMisc.size() && spacecraftMisc[blanker] != "")
            {
               spacecraftMisc[blanker] = spacecraftMisc[blanker] + " ";
            }
            allDone = (blanker >= spacecraftNames.size() && blanker >= spacecraftAntennas.size() && blanker >= spacecraftTransmitters.size() && blanker >= spacecraftReceivers.size() && blanker >= spacecraftTransponders.size() && blanker >= errorModels.size() && blanker >= spacecraftMisc.size());
         }
      }

      
      if (scCount >= 5)
      {
         for (Integer indexer = 0; indexer < spacecraftReceivers.size(); indexer++)
         {
            Integer lastFound = 0;
            if (spacecraftReceivers[indexer] == "")
            {
               for (Integer newFound = lastFound + 1; newFound != lastFound; newFound++)
               {
                  if (errorModels[newFound] == "")
                  {
                     spacecraftReceivers.insert(spacecraftReceivers.begin() + indexer, errorModels.begin() + lastFound, errorModels.begin() + newFound);
                     indexer = indexer + newFound - lastFound;
                     lastFound = newFound;
                     break;
                  }
               }
            }
         }
         // If list would become too wide, start a new list below
         spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftNames.begin(), spacecraftNames.end());
         spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftAntennas.begin(), spacecraftAntennas.end());
         spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftTransmitters.begin(), spacecraftTransmitters.end());
         spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftReceivers.begin(), spacecraftReceivers.end());
         spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftTransponders.begin(), spacecraftTransponders.end());
         spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftMisc.begin(), spacecraftMisc.end());
         spacecraftHardwarePrint.push_back("");

         spacecraftNames.clear();
         spacecraftAntennas.clear();
         spacecraftTransmitters.clear();
         spacecraftReceivers.clear();
         errorModels.clear();
         spacecraftTransponders.clear();
         spacecraftMisc.clear();
         scCount = 0;
      }

   }

   for (Integer indexer = 0; indexer < spacecraftReceivers.size(); indexer++)
   {
      Integer lastFound = 0;
      if (spacecraftReceivers[indexer] == "")
      {
         for (Integer newFound = lastFound + 1; newFound != lastFound; newFound++)
         {
            if (errorModels[newFound] == "")
            {
               spacecraftReceivers.insert(spacecraftReceivers.begin() + indexer, errorModels.begin() + lastFound, errorModels.begin() + newFound);
               indexer = indexer + newFound - lastFound;
               lastFound = newFound;
               break;
            }
         }
      }
   }

   spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftNames.begin(), spacecraftNames.end());
   spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftAntennas.begin(), spacecraftAntennas.end());
   spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftTransmitters.begin(), spacecraftTransmitters.end());
   spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftReceivers.begin(), spacecraftReceivers.end());
   spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftTransponders.begin(), spacecraftTransponders.end());
   spacecraftHardwarePrint.insert(spacecraftHardwarePrint.end(), spacecraftMisc.begin(), spacecraftMisc.end());
   for (int i = 0; i < spacecraftHardwarePrint.size() - 1; i++)
   {
      textFile << spacecraftHardwarePrint[i] << "\n";
   }

   // 3. Write out information in string array buffer to report file and clear the buffer
   for (Integer i = 0; i < buffer.size(); ++i)
      textFile << buffer[i] << "\n";
   textFile << "\n";
   textFile.flush();

   // Clean up memory
   buffer.clear();
   participantNames.clear();
}

//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart3()
//------------------------------------------------------------------------------
/**
* This function is used to write force modeling options to the report file.
*/
//------------------------------------------------------------------------------
void Estimator::WriteReportFileHeaderPart3()
{
   // 1. Write subheader
   textFile << "*******************************************************************  FORCE MODELING OPTIONS  *******************************************************************\n";
   textFile << "\n";
   
   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   PropSetup *prop = propagators[0];
   StringArray paramNames, paramValues, rowContent;
   
   // 2. Fill in parameter's name
   paramNames.push_back("Spacecraft Name");
   paramNames.push_back("");
   paramNames.push_back("Orbit Generator");
   paramNames.push_back("Central Body of Integration");
   paramNames.push_back("System of Integration");
   paramNames.push_back("Integrator");
   paramNames.push_back("  Error Control");
   paramNames.push_back("  Initial Step Size (sec)");
   paramNames.push_back("  Accuracy (km)");
   paramNames.push_back("  Minimum Step Size (sec)");
   paramNames.push_back("  Maximum Step Size (sec)");
   paramNames.push_back("  Maximum Attempts");
   paramNames.push_back("  Stop if Accuracy is Violated");
   paramNames.push_back("Central Body Gravity Model");
   paramNames.push_back("  Degree and Order");
   paramNames.push_back("Non - Central Bodies");
   paramNames.push_back("Solar Radiation Pressure");
   paramNames.push_back("  Spacecraft Area Model");
   paramNames.push_back("  Solar Irradiance (W/m^2)");
   paramNames.push_back("  Astronomical Unit (km)");
   paramNames.push_back("Drag");
   paramNames.push_back("  Spacecraft Area Model");
   paramNames.push_back("  Atmospheric Density Model");
   paramNames.push_back("Central Body Solid Tides");
   paramNames.push_back("Central Body Albedo");
   paramNames.push_back("Central Body Thermal Radiation");
   paramNames.push_back("Spacecraft Thermal Radiation Pressure");
   paramNames.push_back("Relativistic Accelerations");

   // Set flag to skip some section in force model table
   bool skipGravityModel = true;
   bool skipRadPressure = true;
   bool skipDrag = true;
   Integer gmIndex = 0;                // index such as paramNames[index] == "Central Body Gravity Model"
   Integer rpIndex = 0;                // index such as paramNames[index] == "Solar Radiation Pressure"
   Integer dragIndex = 0;              // index such as paramNames[index] == "Drag"

   Integer nameLen = 0;
   for (UnsignedInt i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());

   
   // 3. Write table containing force model information for each spacecraft
   Integer colCount = 0;
   std::string val;
   std::stringstream ss;
   Integer totalColumns = 0;
   Integer max_size = 1;
   Integer scCount = -1;
   for (UnsignedInt i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);
      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         scCount++;
         Spacecraft *sc = (Spacecraft *)obj;

         //3.0. Get PropSetup
         PropSetup  *ps = propagators[0];
         if (propagators.size() > 1)
         {
            for (Integer counter = 0; counter < propagators.size(); counter++)
            {
               std::string correctPropName = "";
               StringArray scPropNames = propagatorSatMap[propagatorNames[counter]];
               for (Integer counterInner = 0; counterInner < scPropNames.size(); counterInner++)
               {
                  if (sc->GetName() == scPropNames[counterInner])
                  {
                     correctPropName = propagatorNames[counter];
                     break;
                  }
               }
               if (correctPropName != "")
               {
                  for (Integer counterInner = 0; counterInner < propagators.size(); counterInner++)
                  {
                     if (propagators[counterInner]->GetName() == correctPropName)
                     {
                        ps = propagators[counterInner];
                     }
                  }
                  break;
               }
            }
         }
         else
         {
            ps = propagators[0];
         }
         ODEModel   *ode  = ps->GetODEModel();
         Propagator *prop = ps->GetPropagator();

         // 3.1. Skip writing information of spacecraft ODE model if no ODE model is used
         if (ode == NULL)
            continue;

         // 3.2. Fill in parameter's value and unit
         paramValues.push_back(sc->GetName());                                                            // Spacecraft Name
         paramValues.push_back("");

         // Get all needed info from force model

         paramValues.push_back("");                                                                       // Orbit Generator
         paramValues.push_back(ode->GetStringParameter("CentralBody"));                                   // Central Body of Integration
         paramValues.push_back("J2000Eq");                                                                // System of Integration    // for current GMAT version, only J2000Eq is used for force model
         paramValues.push_back(prop->GetTypeName());                                                      // Integrator
         paramValues.push_back(ode->GetStringParameter("ErrorControl"));                                  //   Error Control
         ss.str(""); ss << GmatStringUtil::RealToString(ps->GetRealParameter("InitialStepSize"),false, false, false, 8); paramValues.push_back(ss.str());      //   Initial Step Size
         ss.str(""); ss << GmatStringUtil::RealToString(ps->GetRealParameter("Accuracy"), false, true, false, 8); paramValues.push_back(ss.str());             //   Accuracy
         ss.str(""); ss << GmatStringUtil::RealToString(ps->GetRealParameter("MinStep"), false, false, false, 8); paramValues.push_back(ss.str());              //   Minimum Step Size
         ss.str(""); ss << GmatStringUtil::RealToString(ps->GetRealParameter("MaxStep"), false, false, false, 8); paramValues.push_back(ss.str());              //   Maximum Step Size
         ss.str(""); ss << GmatStringUtil::RealToString(ps->GetIntegerParameter("MaxStepAttempts"), false, false, false, 8); paramValues.push_back(ss.str());   //   Maximum Attempts
         
         val = (ps->GetBooleanParameter("StopIfAccuracyIsViolated") ? "True" : "False");
         paramValues.push_back(val);                                                                      //   Stop if Accuracy is Violated

         PhysicalModel *force   = NULL;
         GravityField  *gvForce = NULL;
         for (Integer j = 0; j < ode->GetNumForces(); ++j)
         {
            PhysicalModel *force = ode->GetForce(j);
            if (force->GetTypeName() == "GravityField")
            {
               gvForce = (GravityField*)force;
            }
         }

         
         if (gvForce != NULL)
         {
            Integer deg = gvForce->GetIntegerParameter("Degree");
            Integer ord = gvForce->GetIntegerParameter("Order");
            std::string potentialFile = gvForce->GetStringParameter("Model");
            
            std::string::size_type pos = potentialFile.find_last_of('\\');
            if (pos == std::string::npos)
               pos = potentialFile.find_last_of('/');
            if (pos != std::string::npos)
               potentialFile = potentialFile.substr(pos+1);

            pos = potentialFile.find_first_of('.');
            potentialFile = potentialFile.substr(0, pos);

            ss.str(""); ss << deg << "x" << ord;
            paramValues.push_back(potentialFile);                                                        // Central Body Gravity Model
            paramValues.push_back(ss.str());                                                             //   Degree and Order

            skipGravityModel = false;
         }
         else
         {
            paramValues.push_back("None");                                                               // Central Body Gravity Model
            paramValues.push_back("N/A");                                                                //   Degree and Order
         }
         gmIndex = paramValues.size() - 2;

         
         // fill blanks
         StringArray bodyNames = ode->GetStringArrayParameter("PointMasses");
         if (bodyNames.size() <= max_size)
         {
            // Fill in Values and Units
            for (UnsignedInt index = 0; index < bodyNames.size(); ++index)
            {
               paramValues.push_back(bodyNames[index]);
            }
            
            // Fill blanks for the remain
            for (UnsignedInt index = bodyNames.size(); index < max_size; ++index)
            {
               paramValues.push_back("");
            }
         }
         else
         {  // For max_size < bodyNames.size()
            // Specify the start index
            Integer k = 0;
            for (; k < paramNames.size(); ++k)
            {
               if (paramNames[k] == "Non - Central Bodies")
                  break;
            }
            // Specify the end index
            Integer k1 = k+1;
            for (; k1 < paramNames.size(); ++k1)
            {
               if (paramNames[k1] != "")
                  break;
            }
            
            // Insert blank lines to paramNames as needed
            StringArray::iterator pos = paramNames.begin() + k1;
            Integer mm = bodyNames.size() - (k1 - k);
            if (mm > 0)
            {
               paramNames.insert(pos, mm, "");

               // Insert blank line to rowContent as needed
               if (colCount != 0)
               {
                  // Add blanks lines to rowContent as needed
                  StringArray::iterator pos1 = rowContent.begin() + k1;
                  rowContent.insert(pos1, mm, GmatStringUtil::GetAlignmentString("", (pos1-1)->size()));
               }
            }


            // Set value to paramValues and paramUnits
            if (bodyNames.size() == 0)
            {
               paramValues.push_back("");
               for (Integer j = 1; j < max_size; ++j)
               {
                  paramValues.push_back("");
               }
            }
            else
            {
               for (Integer j = 0; j < bodyNames.size(); ++j)
               {
                  paramValues.push_back(bodyNames[j]);
               }
               for (Integer j = bodyNames.size(); j < max_size; ++j)
               {
                  paramValues.push_back("");
               }
            }

            // Reset max_size
            max_size = bodyNames.size();
         }
         
         
         std::string srp = ode->GetOnOffParameter("SRP");
         if (srp == "On")
         {
            paramValues.push_back("Yes");                                                                 // Solar Radiation Pressure

            PhysicalModel *force;
            UnsignedInt numForces = ode->GetNumForces();
            for (UnsignedInt index = 0; index < numForces; ++index)
            {
               force = ode->GetForce(index);
               if (force != NULL)
               {
                  if (force->IsOfType("SolarRadiationPressure"))
                     break;
               }
            }

            SolarRadiationPressure *srp = (SolarRadiationPressure*)force;
            paramValues.push_back(srp->GetStringParameter(srp->GetParameterID("SRPModel")));              // Solar Radiation Model
            ss.str(""); ss << GmatStringUtil::RealToString(srp->GetRealParameter(srp->GetParameterID("Flux")), false, false, true, 4);
            paramValues.push_back(ss.str());                                                              // Solar Irradiance
            ss.str(""); ss << GmatStringUtil::RealToString(srp->GetRealParameter(srp->GetParameterID("Nominal_Sun"))*GmatMathConstants::M_TO_KM, false, true, false, 8); 
            paramValues.push_back(ss.str());                                                              // Astronomical Unit

            skipRadPressure = false;
         }
         else
         {
            paramValues.push_back("No");                                                                   // Solar Radiation Pressure
            paramValues.push_back("");                                                                     // Solar Radiation Model
            paramValues.push_back("");                                                                     // Solar Irradiance
            paramValues.push_back("");                                                                     // Astronomical Unit
         }
         rpIndex = paramValues.size() - 4;


         std::string drag = ode->GetStringParameter("Drag");
         if (drag == "None")
         {
            paramValues.push_back("No");                                                                   // Drag
            paramValues.push_back("None");                                                                 // Spacecraft Area Model
            paramValues.push_back("None");                                                                 // Atmospheric Density Model
         }
         else
         {
            PhysicalModel *force = NULL;
            UnsignedInt numForces = ode->GetNumForces();
            for (UnsignedInt index = 0; index < numForces; ++index)
            {
               force = ode->GetForce(index);
               if (force != NULL)
               {
                  if (force->IsOfType("DragForce"))
                     break;
               }
            }
            DragForce *df = (DragForce*)force;
            
            paramValues.push_back("Yes");                                                                  // Drag
            paramValues.push_back(df->GetStringParameter(df->GetParameterID("DragModel")));     //GMT-6652 // Spacecraft Area Model
            paramValues.push_back(drag);                                                                   // Atmospheric Density Model

            skipDrag = false;
         }
         dragIndex = paramValues.size() - 3;


         if (gvForce != NULL)
            paramValues.push_back(gvForce->GetStringParameter("TideModel"));
         else
            paramValues.push_back("");                                                                     // Central Body Solid Tides

         //@todo: It needs to modify this code when Central Body Albedo, Central Body Thermal Radiation, 
         //       and Spacecraft Thermal Radiation Pressure options are added to GMAT
         paramValues.push_back("No");                                                                     // Central Body Albedo
         paramValues.push_back("No");                                                                     // Central Body Thermal Radiation
         paramValues.push_back("No");                                                                     // Spacecraft Thermal Radiation Pressure

         val = ((ode->GetOnOffParameter("RelativisticCorrection") == "On") ? "Yes" : "No");
         paramValues.push_back(val);                                                                       // Relativistic Accelerations

         // 3.3. Increasing column count by 1
         ++colCount;
         if (totalColumns != 0 && totalColumns % 5 == 0)
         {
            textFile << "\n";
            textFile << "\n";
         }
         totalColumns++;
         // 3.4. Write information of the spacecraft on the column
         for (UnsignedInt j = 0; j < paramNames.size(); ++j)
         {
            if (j == rowContent.size())
               rowContent.push_back("");

            if (colCount == 1)
               rowContent[j] += (" " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen = 38) + " ");

            rowContent[j] += (GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(paramValues[j]), 24) + " ");
         }
         
         // 3.5. Beak up columns in a table 
         if (colCount > 4)
         {
            for (UnsignedInt j = 0; j < rowContent.size(); ++j)
            {
               // skip 1 line showing the details of central body gravity model
               if (skipGravityModel && (j == (gmIndex + 1)))
                  continue;

               // skip 3 lines showing the details of solar radiation pressure 
               if (skipRadPressure && (rpIndex + 1 <= j) && (j <= rpIndex + 3))
                  continue;

               // skip 2 lines showing the details of drag model
               if (skipDrag && (dragIndex + 1 <= j) && (j <= dragIndex + 2))
                  continue;

               textFile << rowContent[j] << "\n";
            }

            rowContent.clear();
            paramNames.clear();
            colCount = 0;
            max_size = 1;

            // Fill in parameter's name
            paramNames.push_back("Spacecraft Name");
            paramNames.push_back("");
            paramNames.push_back("Orbit Generator");
            paramNames.push_back("Central Body of Integration");
            paramNames.push_back("System of Integration");
            paramNames.push_back("Integrator");
            paramNames.push_back("  Error Control");
            paramNames.push_back("  Initial Step Size (sec)");
            paramNames.push_back("  Accuracy (Km)");
            paramNames.push_back("  Minimum Step Size (sec)");
            paramNames.push_back("  Maximum Step Size (sec)");
            paramNames.push_back("  Maximum Attempts");
            paramNames.push_back("  Stop if Accuracy is Violated");
            paramNames.push_back("Central Body Gravity Model");
            paramNames.push_back("  Degree and Order");
            paramNames.push_back("Non - Central Bodies");
            paramNames.push_back("Solar Radiation Pressure");
            paramNames.push_back("  Spacecraft Area Model");
            paramNames.push_back("  Solar Irradiance (W/m^2)");
            paramNames.push_back("  Astronomical Unit (km)");
            paramNames.push_back("Drag");
            paramNames.push_back("  Spacecraft Area Model");
            paramNames.push_back("  Atmospheric Density Model");
            paramNames.push_back("Central Body Solid Tides");
            paramNames.push_back("Central Body Albedo");
            paramNames.push_back("Central Body Thermal Radiation");
            paramNames.push_back("Spacecraft Thermal Radiation Pressure");
            paramNames.push_back("Relativistic Accelerations");

            skipGravityModel = true;
            skipRadPressure = true;
            skipDrag = true;
            gmIndex = 0;
            rpIndex = 0;
            dragIndex = 0;
         }
         
         // 3.6. Clear paramValues and paramUnits
         paramValues.clear();
      }
   }
   
   for (UnsignedInt j = 0; j < rowContent.size(); ++j)
   {
      // skip 1 line showing the details of central body gravity model
      if (skipGravityModel && (j == (gmIndex + 1)))
         continue;

      // skip 3 lines showing the details of solar radiation pressure 
      if (skipRadPressure && (rpIndex + 1 <= j) &&(j <= rpIndex + 3))
         continue;

      // skip 2 lines showing the details of drag model
      if (skipDrag && (dragIndex + 1 <= j) && (j <= dragIndex + 2))
         continue;

      textFile << rowContent[j] << "\n";
   }
   textFile << "\n";
   textFile.flush();

   rowContent.clear();
   
   // Get a list of all thrust history files
   StringArray thfActiveSegs;
   StringArray thfList = GetListOfObjects("ThrustHistoryFile");

   for (UnsignedInt i = 0; i < thfList.size(); ++i)
   {
      GmatBase* thfObj = GetConfiguredObject(thfList[i]);
      StringArray segs = thfObj->GetRefObjectNameArray(Gmat::INTERFACE);

      for (UnsignedInt j = 0; j < segs.size(); ++j)
         thfActiveSegs.push_back(segs[j]);
   }

   // Get a list of all thrust history file segments
   std::vector<GmatBase*> thfsList;

   for (UnsignedInt i = 0; i < transientForces->size(); ++i)
   {
      PhysicalModel* force = transientForces->at(i);
      if (force->IsOfType("FileThrust"))
      {
         StringArray objs = force->GetRefObjectNameArray(Gmat::INTERFACE);

         for (UnsignedInt j = 0; j < objs.size(); ++j)
         {
            GmatBase* obj = force->GetRefObject(Gmat::INTERFACE, objs[j]);

            if (std::find(thfActiveSegs.begin(), thfActiveSegs.end(), obj->GetName()) !=
                  thfActiveSegs.end() && obj->IsOfType("ThrustSegment"))
               thfsList.push_back(obj);
         }
      }
   }

   // Write table containing thrust history file segments' information
   if (thfsList.size() > 0U)
   {
      textFile << GmatStringUtil::GetAlignmentString("", 73) + "Thrust Models\n";
      textFile << "\n";
   }
   
   // Fill in parameter's name
   paramNames.clear();
   paramNames.push_back("Thrust Segment");
   paramNames.push_back("");
   paramNames.push_back("Start Time");
   paramNames.push_back("End Time");
   paramNames.push_back("Duration (sec)");
   paramNames.push_back("Thrust Scale Factor");
   paramNames.push_back("Thrust Scale Factor Sigma");
   paramNames.push_back("Mass Flow Scale Factor");
   paramNames.push_back("Apply TSF to Mass Flow");
   paramNames.push_back("Thrust Angle Constraint Vector");
   paramNames.push_back("Solve Fors");
   Integer solveForSize = 1;

   StringArray angle1Names, angle2Names;
   std::vector<StringArray> angle1Values, angle2Values;
   Integer angle1Size = 0;
   Integer angle2Size = 0;

   nameLen = 37;
   colCount = 0;
   for (Integer i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());

   // Set value to paramValues
   for (Integer i = 0; i < thfsList.size(); ++i)
   {
      // Get Thrust History File object
      GmatBase *seg = thfsList[i];
      StringArray solveFors = (seg->GetStringArrayParameter("SolveFors"));

      // Fill in parameter's value
      paramValues.push_back(seg->GetName());                                                         // Thrust Segment
      paramValues.push_back("");                                                                     // Blank
      Real startEpoch = seg->GetRealParameter("StartEpoch");
      Real endEpoch = seg->GetRealParameter("EndEpoch");
      ss.str(""); ss << theTimeConverter->ConvertMjdToGregorian(
         theTimeConverter->Convert(startEpoch, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD));
      paramValues.push_back(ss.str());                                                               // Start Time
      ss.str(""); ss << theTimeConverter->ConvertMjdToGregorian(
         theTimeConverter->Convert(endEpoch, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD));
      paramValues.push_back(ss.str());                                                               // End Time
      ss.str(""); ss << GmatStringUtil::RealToString((endEpoch - startEpoch)*GmatTimeConstants::SECS_PER_DAY, false, false, true, 1);
      paramValues.push_back(ss.str());                                                               // Duration (sec)
      ss.str(""); ss << GmatStringUtil::RealToString(seg->GetRealParameter("ThrustScaleFactor"), false, false, true, 6);
      paramValues.push_back(ss.str());                                                               // Thrust Scale Factor
      ss.str("");
      if (std::find(solveFors.begin(), solveFors.end(), "ThrustScaleFactor") != solveFors.end())
         ss << GmatStringUtil::RealToString(seg->GetRealParameter("ThrustScaleFactorSigma"), false, true, true, 6);
      paramValues.push_back(ss.str());                                                               // Thrust Scale Factor Sigma
      ss.str(""); ss << GmatStringUtil::RealToString(seg->GetRealParameter("MassFlowScaleFactor"), false, false, true, 6);
      paramValues.push_back(ss.str());                                                               // Mass Flow Scale Factor
      ss.str(""); ss << (seg->GetBooleanParameter("ApplyThrustScaleToMassFlow") ? "True" : "False");
      paramValues.push_back(ss.str());                                                               // Apply TSF to Mass Flow
      ss.str(""); ss << "[ " << GmatStringUtil::Trim(seg->GetRvectorParameter("ThrustAngleConstraintVector").ToString(false, false, true, 7, 10)) << " ]";
      paramValues.push_back(ss.str());                                                               // Thrust Angle Constraint

      if (solveFors.size() == 0)
      {
         paramValues.push_back("None");                                                              // Solve Fors

         Integer sizeDiff = solveForSize - 1;
         // Fill blank rows
         if (sizeDiff > 0)
            for (Integer j = 0; j < sizeDiff; ++j)
               paramValues.push_back("");
      }
      else if (solveFors.size() == 1)
      {
         paramValues.push_back(solveFors[0]);                                                               // Solve Fors

         Integer sizeDiff = solveForSize - 1;
         // Fill blank rows
         if (sizeDiff > 0)
            for (Integer j = 0; j < sizeDiff; ++j)
               paramValues.push_back("");
      }
      else
      {
         // Each solve-for goes on its own row
         for (Integer j = 0; j < solveFors.size(); ++j)
         {
            paramValues.push_back(solveFors[j]);                                                            // Solve Fors
         }

         // Add new rows
         Integer sizeDiff = solveFors.size() - solveForSize;
         if (sizeDiff > 0)
         {
            // Get index of row after "Solve Fors"
            auto paramNamesIter = std::find(paramNames.begin(), paramNames.end(), "Solve Fors") + solveForSize;
            Integer index = std::distance(paramNames.begin(), paramNamesIter);

            paramNames.insert(paramNamesIter, sizeDiff, "");

            if (colCount > 0)
            {
               // Add new blank rows to rowContent
               auto rowContentIter = rowContent.begin() + index;
               rowContent.insert(rowContentIter, sizeDiff, GmatStringUtil::GetAlignmentString("", nameLen + 3 + 40 * colCount));
            }

            solveForSize = solveFors.size();
         }
         else if (sizeDiff < 0)
         {
            // Fill blank rows
            for (Integer j = 0; j > sizeDiff; --j)
            {
               paramValues.push_back("");
            }
         }
      }

      Rvector angle1Vec = seg->GetRvectorParameter("ThrustAngle1");
      Rvector angle1SigmaVec = seg->GetRvectorParameter("ThrustAngle1Sigma");
      StringArray angleStr;

      if (angle1Vec.GetSize() > angle1Size)
      {
         for (Integer j = angle1Size; j < angle1Vec.GetSize(); ++j)
         {
            std::string unit = "deg";
            if (j == 1)
               unit += "/sec";
            else if (j > 1)
               unit += "/sec^" + GmatStringUtil::ToString(j, false, 1);

            std::string name = "Thrust Angle 1 C" + GmatStringUtil::ToString(j, false, 1) + " (" + unit + ")";
            angle1Names.push_back(GmatStringUtil::GetAlignmentString(name, nameLen, GmatStringUtil::LEFT));
            name = "Thrust Angle 1 C" + GmatStringUtil::ToString(j, false, 1) + " Sigma (" + unit + ")";
            angle1Names.push_back(GmatStringUtil::GetAlignmentString(name, nameLen, GmatStringUtil::LEFT));
         }
         angle1Size = angle1Vec.GetSize();
      }

      for (UnsignedInt j = 0; j < angle1Vec.GetSize(); ++j)
      {
         ss.str("");
         if (angle1Vec[j] >= 0.0)
            ss << " ";
         ss << GmatStringUtil::RealToString(angle1Vec[j], false, true, true, 8);
         angleStr.push_back(ss.str());

         ss.str("");
         if (std::find(solveFors.begin(), solveFors.end(), "ThrustAngle1") != solveFors.end())
         {
            if (angle1SigmaVec.GetSize() > j)
               ss << " " << GmatStringUtil::RealToString(angle1SigmaVec[j], false, true, true, 8);
         }
         angleStr.push_back(ss.str());
      }
      angle1Values.push_back(angleStr);
      angleStr.clear();

      Rvector angle2Vec = seg->GetRvectorParameter("ThrustAngle2");
      Rvector angle2SigmaVec = seg->GetRvectorParameter("ThrustAngle2Sigma");

      if (angle2Vec.GetSize() > angle2Size)
      {
         for (Integer j = angle2Size; j < angle2Vec.GetSize(); ++j)
         {
            std::string unit = "deg";
            if (j == 1)
               unit += "/sec";
            else if (j > 1)
               unit += "/sec^" + GmatStringUtil::ToString(j, false, 1);

            std::string name = "Thrust Angle 2 C" + GmatStringUtil::ToString(j, false, 1) + " (" + unit + ")";
            angle2Names.push_back(GmatStringUtil::GetAlignmentString(name, nameLen, GmatStringUtil::LEFT));
            name = "Thrust Angle 2 C" + GmatStringUtil::ToString(j, false, 1) + " Sigma (" + unit + ")";
            angle2Names.push_back(GmatStringUtil::GetAlignmentString(name, nameLen, GmatStringUtil::LEFT));
         }
         angle2Size = angle2Vec.GetSize();
      }

      for (UnsignedInt j = 0; j < angle2Vec.GetSize(); ++j)
      {
         ss.str("");
         if (angle2Vec[j] >= 0.0)
            ss << " ";
         ss << GmatStringUtil::RealToString(angle2Vec[j], false, true, true, 8);
         angleStr.push_back(ss.str());

         ss.str("");
         if (std::find(solveFors.begin(), solveFors.end(), "ThrustAngle2") != solveFors.end())
         {
            if (angle2SigmaVec.GetSize() > j)
               ss << " " << GmatStringUtil::RealToString(angle2SigmaVec[j], false, true, true, 8);
         }
         angleStr.push_back(ss.str());
      }
      angle2Values.push_back(angleStr);
      angleStr.clear();

      ++colCount;

      // Write information of the thrust history file to the column
      for (UnsignedInt j = 0; j < paramNames.size(); ++j)
      {
         if (j == rowContent.size())
         {
            if (colCount == 1)
               rowContent.push_back("");
            else
               rowContent.push_back(GmatStringUtil::GetAlignmentString("", rowContent.at(rowContent.size() - 1).size()));
         }
      }
      for (UnsignedInt j = 0; j < paramNames.size(); ++j)
      {
         if (colCount == 1)
            rowContent[j] += (" " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen, GmatStringUtil::LEFT) + "  ");

         rowContent[j] += (GmatStringUtil::GetAlignmentString(paramValues[j], 39, GmatStringUtil::LEFT) + " ");      // each column has size of 40
      }

      // Beak up columns in a table
      if (colCount >= 3)         // break up column when 4 segments are displayed
      {

         // Wait to write angles until we know how many angles are in all 3 segments written in this row
         rowContent.push_back("");

         for (Integer j = 0; j < angle1Names.size(); ++j)
         {
            ss.str("");
            ss << " " << GmatStringUtil::GetAlignmentString(angle1Names[j], nameLen, GmatStringUtil::LEFT) << "  ";

            for (Integer k = 0; k < angle1Values.size(); ++k)
            {
               if (angle1Values[k].size() > j)
                  ss << (GmatStringUtil::GetAlignmentString(angle1Values[k][j], 39, GmatStringUtil::LEFT) + " "); // each column has size of 40
               else
                  ss << (GmatStringUtil::GetAlignmentString("", 39, GmatStringUtil::LEFT) + " ");                 // each column has size of 40
            }
            rowContent.push_back(ss.str());
         }

         rowContent.push_back("");

         for (Integer j = 0; j < angle2Names.size(); ++j)
         {
            ss.str("");
            ss << " " << GmatStringUtil::GetAlignmentString(angle2Names[j], nameLen, GmatStringUtil::LEFT) << "  ";

            for (Integer k = 0; k < angle2Values.size(); ++k)
            {
               if (angle2Values[k].size() > j)
                  ss << (GmatStringUtil::GetAlignmentString(angle2Values[k][j], 39, GmatStringUtil::LEFT) + " "); // each column has size of 40
               else
                  ss << (GmatStringUtil::GetAlignmentString("", 39, GmatStringUtil::LEFT) + " ");                 // each column has size of 40
            }
            rowContent.push_back(ss.str());
         }

         angle1Size = angle2Size = 0;
         angle1Names.clear();
         angle1Values.clear();
         angle2Names.clear();
         angle2Values.clear();

         for (UnsignedInt j = 0; j < rowContent.size(); ++j)
            textFile << rowContent[j] << "\n";
         textFile << "\n";

         rowContent.clear();
         colCount = 0;

         paramNames.clear();
         paramNames.push_back("Thrust Segment");
         paramNames.push_back("");
         paramNames.push_back("Start Time");
         paramNames.push_back("End Time");
         paramNames.push_back("Duration (sec)");
         paramNames.push_back("Thrust Scale Factor");
         paramNames.push_back("Thrust Scale Factor Sigma");
         paramNames.push_back("Mass Flow Scale Factor");
         paramNames.push_back("Apply TSF to Mass Flow");
         paramNames.push_back("Thrust Angle Constraint Vector");
         paramNames.push_back("Solve Fors");
         solveForSize = 1;
      }

      // Clear paramValues and paramUnits
      paramValues.clear();
   }
   
   // Wait to write angles until we know how many angles are in all 3 segments written in this row
   if (angle1Names.size() > 0)
      rowContent.push_back("");

   for (Integer j = 0; j < angle1Names.size(); ++j)
   {
      ss.str("");
      ss << " " << GmatStringUtil::GetAlignmentString(angle1Names[j], nameLen, GmatStringUtil::LEFT) << "  ";

      for (Integer k = 0; k < angle1Values.size(); ++k)
      {
         if (angle1Values[k].size() > j)
            ss << (GmatStringUtil::GetAlignmentString(angle1Values[k][j], 39, GmatStringUtil::LEFT) + " "); // each column has size of 40
         else
            ss << (GmatStringUtil::GetAlignmentString("", 39, GmatStringUtil::LEFT) + " ");                 // each column has size of 40
      }
      rowContent.push_back(ss.str());
   }

   if (angle2Names.size() > 0)
      rowContent.push_back("");

   for (Integer j = 0; j < angle2Names.size(); ++j)
   {
      ss.str("");
      ss << " " << GmatStringUtil::GetAlignmentString(angle2Names[j], nameLen, GmatStringUtil::LEFT) << "  ";

      for (Integer k = 0; k < angle2Values.size(); ++k)
      {
         if (angle2Values[k].size() > j)
            ss << (GmatStringUtil::GetAlignmentString(angle2Values[k][j], 39, GmatStringUtil::LEFT) + " "); // each column has size of 40
         else
            ss << (GmatStringUtil::GetAlignmentString("", 39, GmatStringUtil::LEFT) + " ");                 // each column has size of 40
      }
      rowContent.push_back(ss.str());
   }

   for (UnsignedInt j = 0; j < rowContent.size(); ++j)
      textFile << rowContent[j] << "\n";

   if (rowContent.size() > 0)
      textFile << "\n";
   textFile.flush();

   rowContent.clear();
   
   // Get a list of all process noise models
   StringArray pnmList;
   for (UnsignedInt i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);
      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         Spacecraft *sc = (Spacecraft *)obj;
         std::string noiseName = sc->GetStringParameter("ProcessNoiseModel");
         GmatBase* noiseModel = sc->GetRefObject(GmatType::GetTypeId("ProcessNoiseModel"), noiseName);
         if (noiseModel)
         {
            // Add the process noise model to list of all process noise models
            bool found = false;
            for (Integer k = 0; k < pnmList.size(); ++k)
            {
               if (pnmList[k] == noiseName)
               {
                  found = true;
                  break;
               }
            }

            if (!found)
               pnmList.push_back(noiseName);
         }

      }
   }
   
   bool hasEphemProp = false;
   for (Integer indxer = 0; indxer < propagators.size(); indxer++)
   {
      std::string propTypeName = propagators[indxer]->GetPropagator()->GetTypeName();
      if (propTypeName == "SPK" || 
		  propTypeName == "STK" ||
		  propTypeName == "Code500" ||
		  propTypeName == "CCSDS-OEM")
      {

         if (hasEphemProp == false)
         {
            // Write table containing process noise models' information

            textFile << GmatStringUtil::GetAlignmentString("", 70) + "Ephemeris Propagators\n";
            textFile << "\n";
            textFile << " Spacecraft          Type         Start Time                End Time                   Ephemeris File Name\n";
            textFile << "\n";

            hasEphemProp = true;
         }
         GmatBase *propo = propagators[indxer]->Clone();
         Propagator* propClone = ((PropSetup *)propo)->GetPropagator();
         propClone->SetName("Chareleir");
         std::string propagatorName = propagators[indxer]->GetName();
         for (Integer scindxer = 0; scindxer < propagatorSatMap[propagatorName].size(); scindxer++)
         {
            std::string scName = propagatorSatMap[propagatorName][scindxer];
            Spacecraft * sc = (Spacecraft *)GetConfiguredObject(scName);
            
            // Have to initialize the cloned propagator in order to access the start/end epochs
            PropagationStateManager *psm = ((PropSetup *)propo)->GetPropStateManager();
            psm->SetObject(sc);
            psm->SetProperty("CartesianState");
            psm->SetProperty("STM");
            psm->BuildState();
            psm->MapObjectsToVector();
            propClone->SetPropStateManager(psm);
            propClone->SetRefObject(sc, sc->GetType(), sc->GetName());
            propClone->SetSolarSystem(solarSystem);
            propClone->Initialize();
            GmatTime fromMjd(-999.999);
            GmatTime toGreg(-999.999);
            
            //for each propagator, check the sat name, the ephem type, start epoch, end epoch, and ephemeris path
            std::string newLine = " ";
            std::string buffer;
            newLine += (GmatStringUtil::GetAlignmentString(sc->GetName(), 19, GmatStringUtil::LEFT) + " ");
            newLine += (GmatStringUtil::GetAlignmentString(propagators[indxer]->GetPropagator()->GetTypeName(), 12, GmatStringUtil::LEFT) + " ");                       // SC name
            theTimeConverter->Convert("A1ModJulian", fromMjd, GmatStringUtil::RealToString(propClone->GetRealParameter("EphemStart"), false), "UTCGregorian", toGreg, buffer);   // Start of Ephem
            newLine += (GmatStringUtil::GetAlignmentString(buffer, 24, GmatStringUtil::LEFT) + "  ");
            theTimeConverter->Convert("A1ModJulian", fromMjd, GmatStringUtil::RealToString(propClone->GetRealParameter("EphemEnd"), false), "UTCGregorian", toGreg, buffer);     // End of Ephem
            newLine += (GmatStringUtil::GetAlignmentString(buffer, 24, GmatStringUtil::LEFT) + "   ");
            
            std::string ephemFileName;
            if (propagators[indxer]->GetPropagator()->GetTypeName() == "SPK")
            {
               ephemFileName = sc->GetStringArrayParameter("OrbitSpiceKernelName")[0];
            }
            else
            {
               ephemFileName = sc->GetStringParameter("EphemerisName");
            }
            newLine += (GmatStringUtil::GetAlignmentString(ephemFileName, 72, GmatStringUtil::LEFT) + " ");  // Path to the Ephem file
            newLine += "\n";
            textFile << newLine;

            // buffer = propagators[indxer]->GetName();
            if (scindxer + 1 < propagatorSatMap[propagatorName].size())
            {
               //if (propo)
               //{
               //   delete propo;
               //   propo = NULL;
               //}
               
               propo = propagators[indxer]->Clone();
               propClone = ((PropSetup *)propo)->GetPropagator();
               propClone->SetName("Chareleir");
            }
         }
         //if (propo)
         //   delete propo;
      }
   }
   
   if (hasEphemProp == true)
   {
      textFile << "\n";
   }
   textFile.flush();
   
   // Write table containing process noise models' information
   if (pnmList.size() > 0U)
   {
      textFile << GmatStringUtil::GetAlignmentString("", 70) + "Process Noise Models\n";
      textFile << "\n";
   }
   
   // Fill in parameter's name
   paramNames.clear();
   paramNames.push_back("Name");
   paramNames.push_back("Type");
   paramNames.push_back("CoordinateSystem");
   paramNames.push_back("UpdateTimeStep (sec)");

   nameLen = 30;
   colCount = 0;
   for (Integer i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());

   // Set value to paramValues
   for (Integer i = 0; i < pnmList.size(); ++i)
   {
      // Get Process Noise Model object
      GmatBase *pnm = GetConfiguredObject(pnmList[i]);
      std::string pnmType = pnm->GetStringParameter("Type");

      // Fill in parameter's value
      paramValues.push_back(pnm->GetName());                                                         // Name
      paramValues.push_back(pnmType);                                                                // Type
      paramValues.push_back(pnm->GetStringParameter("CoordinateSystem"));                            // CoordinateSystem
      paramValues.push_back(GmatStringUtil::RealToString(pnm->GetRealParameter("UpdateTimeStep")));                            // UpdateTimeStep

      if (pnmType == "LinearTime")
      {
         if (colCount == 0)
            paramNames.push_back("RateVector");

         Rvector vec = pnm->GetRvectorParameter("RateVector");
         std::string vecStr = "[ " + vec.ToString(16) + " ]";
         paramValues.push_back(vecStr);
      }
      else if (pnmType == "StateNoiseCompensation")
      {
         if (colCount == 0)
            paramNames.push_back("AccelNoiseSigma (km/sec^3/2)");

         Rvector vec = pnm->GetRvectorParameter("AccelNoiseSigma");
         std::string vecStr = "[ " + vec.ToString(false, true, true, 4, 10) + " ]";
         paramValues.push_back(vecStr);
      }

      ++colCount;

      // Write information of the process noise model to the column
      for (UnsignedInt j = 0; j < paramNames.size(); ++j)
      {
         if (j == rowContent.size())
            rowContent.push_back("");

         if (colCount == 1)
            rowContent[j] += (" " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen, GmatStringUtil::LEFT) + "  ");

         rowContent[j] += GmatStringUtil::GetAlignmentString(paramValues[j], 40, GmatStringUtil::LEFT) + "  ";
      }

      // Beak up columns in a table
      if (colCount >= 3)         // break up column when 4 segments are displayed
      {
         for (UnsignedInt j = 0; j < rowContent.size(); ++j)
            textFile << rowContent[j] << "\n";
         textFile << "\n";

         rowContent.clear();
         colCount = 0;

         paramNames.clear();
         paramNames.push_back("Name");
         paramNames.push_back("Type");
         paramNames.push_back("CoordinateSystem");
      }

      // Clear paramValues and paramUnits
      paramValues.clear();
   }

   if (colCount > 0)
   {
      for (UnsignedInt j = 0; j < rowContent.size(); ++j)
         textFile << rowContent[j] << "\n";
      textFile << "\n";
   }

   rowContent.clear();

   textFile << "\n";
   textFile.flush();
   
   // Clean up memory
   paramNames.clear(); 
   paramValues.clear(); 
   rowContent.clear();
   participantNames.clear();
}


void Estimator::WriteReportFileHeaderPart4_1()
{
   // 1. Write sub header
   textFile << GmatStringUtil::GetAlignmentString("", 66) << "Tracking Data Configuration\n";
   textFile << "\n";

   StringArray paramNames, paramValues, rowContent;

   // 2. Set values to paramNames
   paramNames.push_back("Tracking File Set");
   paramNames.push_back("");
   paramNames.push_back("Tracking Data");
   paramNames.push_back("Light Time");
   paramNames.push_back("Relativistic Corrections");
   paramNames.push_back("ET-TAI Corrections");
   paramNames.push_back("Aberration Correction");
   paramNames.push_back("Frequency Model");
   paramNames.push_back("  Ramp Table");
   
   Integer nameLen = 0;
   for (Integer i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());
   
   // 3. Set values to rowContent
   Integer colCount = 0;
   Integer maxNumConfig = 1;
   std::vector<TrackingFileSet*> tfsList = GetMeasurementManager()->GetAllTrackingFileSets();
   for (Integer i = 0; i < tfsList.size(); ++i)
   {
      // 3.1. Set values to paramValues
      // TrackingFileSet name
      paramValues.push_back(tfsList[i]->GetName());
      paramValues.push_back("");
      
      // Tracking configurations
      StringArray trackingConfigs = tfsList[i]->GetStringArrayParameter("AddTrackingConfig");
      if (trackingConfigs.size() == 0)
      {
         paramValues.push_back("All");
         for (Integer j = 1; j < maxNumConfig; ++j)
            paramValues.push_back("");
      }
      else
      {
         
         if (maxNumConfig >= trackingConfigs.size())
         {
            for (Integer j = 0; j < trackingConfigs.size(); ++j)
               paramValues.push_back(trackingConfigs[j]);
            for (Integer j = trackingConfigs.size(); j < maxNumConfig; ++j)
               paramValues.push_back("");
         }
         else
         {
            for (Integer j = 0; j < trackingConfigs.size(); ++j)
               paramValues.push_back(trackingConfigs[j]);

            // Insert blank lines to paramNames
            StringArray::iterator pos1 = paramNames.begin() + 2 + maxNumConfig;
            paramNames.insert(pos1, trackingConfigs.size() - maxNumConfig, "");

            if (colCount != 0)
            {
               StringArray::iterator pos2 = rowContent.begin() + 2 + maxNumConfig;
               rowContent.insert(pos2, trackingConfigs.size() - maxNumConfig, GmatStringUtil::GetAlignmentString("", (pos2-1)->size()));
            }

            maxNumConfig = trackingConfigs.size();
         }
      }
      
      paramValues.push_back(((tfsList[i]->GetBooleanParameter("UseLightTime")) ? "Yes" : "No"));               // Light Time
      paramValues.push_back(((tfsList[i]->GetBooleanParameter("UseRelativityCorrection")) ? "Yes" : "No"));    // Relativistic Corrections
      paramValues.push_back(((tfsList[i]->GetBooleanParameter("UseETminusTAI")) ? "Yes" : "No"));              // ET-TAI Corrections
      paramValues.push_back(tfsList[i]->GetStringParameter("AberrationCorrection"));                           // Aberration Correction

      // Ramp Table
      StringArray rtList = tfsList[i]->GetStringArrayParameter("RampTable");
      if (rtList.size() == 0)
      {
         paramValues.push_back("Constant Frequency");
         paramValues.push_back("N/A");
      }
      else
      {
         paramValues.push_back("Ramp Table");
         paramValues.push_back(rtList[0]);
      }
      
      // 3.2. Set values to rowContent
      Integer valueLen = 0;
      for (Integer j = 0; j < paramNames.size(); ++j)
         valueLen = (Integer)GmatMathUtil::Max(valueLen, paramValues[j].size());

      std::string s;
      for (Integer j = 0; j < paramNames.size(); ++j)
      {
         if (colCount == 0)
         {
            s = " " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen + 3) + GmatStringUtil::GetAlignmentString(paramValues[j], valueLen);
            rowContent.push_back(s);
         }
         else
            rowContent[j] += ("   " + GmatStringUtil::GetAlignmentString(paramValues[j], valueLen));
      }

      // 3.3. Increase column count by 1
      ++colCount;
      
      // 3.4. Break the column when the size is too big
      if (colCount == 2)
      {
         // Write rowContent to report file
         for (Integer j = 0; j < rowContent.size(); ++j)
            textFile << rowContent[j] << "\n";
         textFile << "\n";

         colCount = 0;
         rowContent.clear();

         // reset paramNames
         paramNames.clear();
         paramNames.push_back("Tracking File Set");
         paramNames.push_back("");
         paramNames.push_back("Tracking Data");
         paramNames.push_back("Light Time");
         paramNames.push_back("Relativistic Corrections");
         paramNames.push_back("ET-TAI Corrections");
         paramNames.push_back("Frequency Model");
         paramNames.push_back("  Ramp Table");
      }
      
      // 3.5. Clear paramValues
      paramValues.clear();
   }
   
   // 4. Write rowContent to report file
   for (Integer j = 0; j < rowContent.size(); ++j)
      textFile << rowContent[j] << "\n";
   textFile << "\n";

   textFile.flush();

   // Clean up memory
   paramNames.clear();
   paramValues.clear(); 
   rowContent.clear();
   tfsList.clear();
}


//------------------------------------------------------------------------------
// void WriteReportFileHeaderPart4_2()
//------------------------------------------------------------------------------
/**
* This function is used to write measurement modeling options to the report file.
*/
//------------------------------------------------------------------------------
void Estimator::WriteReportFileHeaderPart4_2()
{
   StringArray paramNames, paramValues, rowContent;
   
   // 2. Fill in parameter's name
   paramNames.push_back("Name");
   paramNames.push_back("Central Body");
   paramNames.push_back("State Type");
   paramNames.push_back("Horizon Reference");
   paramNames.push_back("Location1 (km)");
   paramNames.push_back("Location2 (km)");
   paramNames.push_back("Location3 (km)");
   paramNames.push_back("Pad ID");
   paramNames.push_back("Min.Elevation Angle (deg)");
   paramNames.push_back("Ionosphere Model");
   paramNames.push_back("Troposphere Model");
   paramNames.push_back("  Temperature (K)");
   paramNames.push_back("  Pressure    (hPa)");
   paramNames.push_back("  Humidity    (%)");
   paramNames.push_back("Measurement Error Models");

   Integer nameLen = 0;
   for (Integer i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());
   
   
   bool skipTropoDesc = true;
   Integer tropoIndex = 10;           // index such as paramNames[index] == "Troposphere Model"

   Integer gsCount = 0;
   Integer colCount = 0;
   Integer totalColumns = 0;
   std::stringstream ss;
   std::string gsName;
   
   Integer maxNumErrorModels = 1;
   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   for (UnsignedInt i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);
      if (obj->IsOfType(Gmat::GROUND_STATION))
      {
         if (gsCount == 0)
         {
            // 3. Write table containing ground stations' information
            textFile << GmatStringUtil::GetAlignmentString("", 66) + "Ground Station Configuration\n";
            textFile << "\n";
         }
         gsCount++;

         // 3.1. Get a ground station for processing:
         GroundstationInterface *gs = (GroundstationInterface *)obj;
         gsName = participantNames[i];

         // 3.2. Fill in parameter's value
         paramValues.push_back(gs->GetName());
         paramValues.push_back(gs->GetStringParameter("CentralBody"));
         paramValues.push_back(gs->GetStringParameter("StateType"));
         paramValues.push_back(gs->GetStringParameter("HorizonReference"));
         ss.str(""); ss << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(gs->GetRealParameter("Location1"), false, false, true, 6), 12, GmatStringUtil::RIGHT); paramValues.push_back(ss.str());
         ss.str(""); ss << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(gs->GetRealParameter("Location2"), false, false, true, 6), 12, GmatStringUtil::RIGHT); paramValues.push_back(ss.str());
         ss.str(""); ss << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(gs->GetRealParameter("Location3"), false, false, true, 6), 12, GmatStringUtil::RIGHT); paramValues.push_back(ss.str());
         paramValues.push_back(gs->GetStringParameter("Id"));
         ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("MinimumElevationAngle"), false, false, false, 8); paramValues.push_back(ss.str());
         paramValues.push_back(gs->GetStringParameter("IonosphereModel"));
         paramValues.push_back(gs->GetStringParameter("TroposphereModel"));

         if (gs->GetStringParameter("TroposphereModel") == "HopfieldSaastamoinen")
         {
            ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("Temperature"), false, false, false, 8); paramValues.push_back(ss.str());
            ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("Pressure"), false, false, false, 8); paramValues.push_back(ss.str());
            ss.str(""); ss << GmatStringUtil::RealToString(gs->GetRealParameter("Humidity"), false, false, false, 8); paramValues.push_back(ss.str());
            
            skipTropoDesc = false;
         }
         else
         {
            paramValues.push_back("");         // Temperature
            paramValues.push_back("");         // Pressure
            paramValues.push_back("");         // Humidity
         }

         StringArray emList = gs->GetStringArrayParameter("ErrorModels");
         if (emList.size() == 0)
         {
            paramValues.push_back("None");
            for (Integer j = 1; j < maxNumErrorModels; ++j)
               paramValues.push_back("");
         }
         else
         {
            if (maxNumErrorModels >= emList.size())
            {
               for (Integer j = 0; j < emList.size(); ++j)
                  paramValues.push_back(emList[j]);

               for (Integer j = emList.size(); j < maxNumErrorModels; ++j)
                  paramValues.push_back("");
            }
            else
            {
               for (Integer j = 0; j < emList.size(); ++j)
               {
                  paramValues.push_back(emList[j]);
               }
               
               // Insert blank lines to paramNames
               for (Integer j = 0; j < emList.size() - maxNumErrorModels; ++j)
               {
                  paramNames.push_back(GmatStringUtil::GetAlignmentString("", nameLen));
               }
               
               maxNumErrorModels = emList.size();
            }
         }
         
         // 3.3. Increasing column count by 1
         ++colCount;
         if (totalColumns != 0 && totalColumns % 5 == 0)
         {
            textFile << "\n";
            textFile << "\n";
         }
         totalColumns++;
         
         // 3.4. Write information of the ground station to the column
         for (UnsignedInt j = 0; j < paramNames.size(); ++j)
         {
            if (j == rowContent.size())
            {
               if (colCount == 1)
                  rowContent.push_back("");
               else
                  rowContent.push_back(GmatStringUtil::GetAlignmentString("", rowContent.at(rowContent.size() - 1).size()));
            }
         }
         for (UnsignedInt j = 0; j < paramNames.size(); ++j)
         {
            if (colCount == 1)
               rowContent[j] += (" " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen, GmatStringUtil::LEFT) + "  ");
            
            rowContent[j] += (GmatStringUtil::GetAlignmentString(paramValues[j], 27, GmatStringUtil::LEFT)+" ");      // each column has size of 28
         }
         

         // 3.5. Beak up columns in a table
         //if ((nameLen+3+ colCount*24) > (160-48))
         if (colCount >= 5 )         // break up column when 5 stations are displayed 
         {
            for (UnsignedInt j = 0; j < rowContent.size(); ++j)
            {
               // Remove 3 lines containing information about Temperature, Pressure, and Humidity when Troposphere model set to None for all stations in table
               if (skipTropoDesc && (tropoIndex < j) && (j <= tropoIndex + 3))
                  continue;

               textFile << rowContent[j] << "\n";
            }

            rowContent.clear();
            colCount = 0;

            // Reset paramNames
            paramNames.clear();
            paramNames.push_back("Name");
            paramNames.push_back("Central Body");
            paramNames.push_back("State Type");
            paramNames.push_back("Horizon Reference");
            paramNames.push_back("Location1 (km)");
            paramNames.push_back("Location2 (km)");
            paramNames.push_back("Location3 (km)");
            paramNames.push_back("Pad ID");
            paramNames.push_back("Min.Elevation Angle (deg)");
            paramNames.push_back("Ionosphere Model");
            paramNames.push_back("Troposphere Model");
            paramNames.push_back("  Temperature (K)");
            paramNames.push_back("  Pressure    (hPa)");
            paramNames.push_back("  Humidity    (%)");
            paramNames.push_back("Measurement Error Models");

            maxNumErrorModels = 1;
            skipTropoDesc = true;
         }
         
         // 3.6. Clear paramValues and paramUnits
         paramValues.clear();
      }
   }
   
   for (UnsignedInt j = 0; j < rowContent.size(); ++j)
   {
      // Remove 3 lines containing information about Temperature, Pressure, and Humidity when Troposphere model set to None for all stations in table
      if (skipTropoDesc && (tropoIndex < j) && (j <= tropoIndex + 3))
         continue;

      textFile << rowContent[j] << "\n";
   }
   if (gsCount > 0)
   {
      textFile << "\n";
      textFile << "\n";
   }

   textFile.flush();

   // Clean up memory
   paramNames.clear();
   paramValues.clear();
   rowContent.clear();
   participantNames.clear();
}


//-------------------------------------------------------------------------
// void Estimator::WriteReportFileHeaderPart4_3()
//-------------------------------------------------------------------------
/**
* Write information about measurement ErrorModel to estimation report file
*/
//-------------------------------------------------------------------------
void Estimator::WriteReportFileHeaderPart4_3()
{
   // 1. Get a list of all error models
   StringArray emList;
   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   for (UnsignedInt i = 0; i < participantNames.size(); ++i)
   {
      GmatBase* obj = GetConfiguredObject(participantNames[i]);
      if (obj->IsOfType(Gmat::GROUND_STATION))
      {
         // 1.1. Get a ground station
         GroundstationInterface *gs = (GroundstationInterface *)obj;

         // 1.2. Get error models used by that ground station
         StringArray errorModels = gs->GetStringArrayParameter("ErrorModels");

         // 1.3. Add those error models to list of all error models
         for (Integer j = 0; j < errorModels.size(); ++j)
         {
            bool found = false;
            for (Integer k = 0; k < emList.size(); ++k)
            {
               if (emList[k] == errorModels[j])
               {
                  found = true;
                  break;
               }
            }

            if (!found)
               emList.push_back(errorModels[j]);
         }
      }
      else if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         // 1.1. Get a spacecraft
         Spacecraft *sc = (Spacecraft *)obj;

         // 1.2. Get GPS receiver used by that spacecraft
         StringArray hwList = sc->GetStringArrayParameter("AddHardware");
         for (UnsignedInt j1 = 0; j1 < hwList.size(); ++j1)
         {
            GmatBase* hw = GetConfiguredObject(hwList[j1]);
            if (hw->IsOfType("Receiver"))
            {
               // 1.3. Get ErrnorModel from GPS receiver
               StringArray errorModels = ((Receiver*)hw)->GetStringArrayParameter("ErrorModels");
               // 1.4. Add those error models to list of all error models
               StringArray errorModelsNames = ((Receiver*)hw)->GetStringArrayParameter("ErrorModels");
               for (Integer j = 0; j < errorModelsNames.size(); ++j)
               {
                  bool found = false;
                  for (Integer k = 0; k < emList.size(); ++k)
                  {
                     if (emList[k] == errorModels[j])
                     {
                        found = true;
                        break;
                     }
                  }

                  if (!found)
                     emList.push_back(errorModels[j]);
               }
            }
         }

      }
   }


   // 2. Write table containing error models' information
   //textFile << GmatStringUtil::GetAlignmentString("", 66) + "Measurement Error Models\n";
   textFile << GmatStringUtil::GetAlignmentString("", 68) + "Measurement Error Models\n";
   textFile << "\n";

   StringArray paramNames, paramValues, rowContent;

   // 2.1. Fill in parameter's name
   paramNames.push_back("Name");
   paramNames.push_back("Measurement Type");
   paramNames.push_back("Noise Sigma");
   paramNames.push_back("Bias");
   paramNames.push_back("Bias Sigma");
   paramNames.push_back("Solve Fors");
   
   Integer nameLen = 0;
   for (Integer i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());

   // 2.2. Set value to paramValues
   Integer colCount = 0;
   Integer totalColumns = 0;
   std::stringstream ss;

   Integer maxNumSolveFors = 1;
   for (Integer i = 0; i < emList.size(); ++i)
   {
      // 2.2.1. Get ErrorModel object
      ErrorModel *em = (ErrorModel*)GetConfiguredObject(emList[i]);

      // 2.2.2. Fill in parameter's value
      paramValues.push_back(em->GetName());                                                         // Name
      paramValues.push_back(em->GetStringParameter("Type"));                                        // Measurement Type
      ss.str(""); ss << GmatStringUtil::RealToString(em->GetRealParameter("NoiseSigma"), false, true, false, 8); paramValues.push_back(ss.str());         // Noise Sigma
      ss.str(""); ss << GmatStringUtil::RealToString(em->GetRealParameter("Bias"), false, false, false, 8); paramValues.push_back(ss.str());              // Bias
      ss.str(""); ss << GmatStringUtil::RealToString(em->GetRealParameter("BiasSigma"), false, true, false, 8); paramValues.push_back(ss.str());          // Bias Sigma

      StringArray sfList = em->GetStringArrayParameter("SolveFors");
      if (sfList.size() == 0)
      {
         paramValues.push_back("None");
         for (Integer j = 1; j < maxNumSolveFors; ++j)
            paramValues.push_back("");
      }
      else
      {
         if (maxNumSolveFors >= sfList.size())
         {
            for (Integer j = 0; j < sfList.size(); ++j)
               paramValues.push_back(sfList[j]);                                                   // Solve Fors

            for (Integer j = sfList.size(); j < maxNumSolveFors; ++j)
               paramValues.push_back("");
         }
         else
         {
            for (Integer j = 0; j < sfList.size(); ++j)
                  paramValues.push_back(sfList[j]);                                               // Solve Fors

            // Insert blank lines to paramNames and rowContent
            StringArray::iterator pos1 = paramNames.begin() + (paramNames.size() - 1);
            paramNames.insert(pos1, sfList.size() - maxNumSolveFors, "");

            if (colCount != 0)
            {
               StringArray::iterator pos2 = rowContent.begin() + (paramNames.size() - 1);
               rowContent.insert(pos2, sfList.size()-maxNumSolveFors, GmatStringUtil::GetAlignmentString("", (pos2-1)->size()));
            }

            maxNumSolveFors = sfList.size();
         }
      }
      

      // 2.2.3. Increasing column count by 1
      ++colCount;
      if (totalColumns != 0 && totalColumns % 5 == 0)
      {
         textFile << "\n";
         textFile << "\n";
      }
      totalColumns++;

      // 2.2.4. Write information of the error model to the column
      for (UnsignedInt j = 0; j < paramNames.size(); ++j)
      {
         if (j == rowContent.size())
            rowContent.push_back("");

         if (colCount == 1)
            rowContent[j] += (" " + GmatStringUtil::GetAlignmentString(paramNames[j], nameLen, GmatStringUtil::LEFT) + "  ");

         rowContent[j] += (GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(paramValues[j]), 22, GmatStringUtil::LEFT) + "  ");
      }

      // 3.5. Beak up columns in a table
      if (colCount > 5)
      {
         for (UnsignedInt j = 0; j < rowContent.size(); ++j)
            textFile << rowContent[j] << "\n";

         rowContent.clear();
         colCount = 0;

         paramNames.clear();
         paramNames.push_back("Name");
         paramNames.push_back("Measurement Type");
         paramNames.push_back("Noise Sigma");
         paramNames.push_back("Bias");
         paramNames.push_back("Bias Sigma");
         paramNames.push_back("Solve Fors");
      }

      // 3.6. Clear paramValues and paramUnits
      paramValues.clear();
   }

   for (UnsignedInt j = 0; j < rowContent.size(); ++j)
      textFile << rowContent[j] << "\n";
   textFile << "\n";
   textFile << "\n";

   textFile.flush();

   // Clean up memory
   emList.clear();
   participantNames.clear();
   paramNames.clear();
   paramValues.clear();
   rowContent.clear();
}


//-------------------------------------------------------------------------
// void Estimator::WriteReportFileHeaderPart4_3b()
//-------------------------------------------------------------------------
/**
* Write information about pass bias initial information to estimation 
* report file
*/
//-------------------------------------------------------------------------
void Estimator::WriteReportFileHeaderPart4_3b()
{
   // 1. Write information of pass biases to buffer
   std::stringstream buffer;
   buffer << "";
   const std::vector<ListItem*>* map = esm.GetStateMap();
   for (Integer i = 0; i < (*map).size(); ++i)
   {
      if ((*map)[i]->elementName == "PassBiases")
      {
         std::stringstream ss;
         // 2.1. Write pass bias' full name
         ss << (*map)[i]->object->GetFullName() << "." << (*map)[i]->elementName << "." << (*map)[i]->subelement;
         buffer << " " << GmatStringUtil::GetAlignmentString(ss.str(), 57, GmatStringUtil::LEFT);

         // 2.2. Write pass bias' unit
         std::string unit = ((ErrorModel*)((*map)[i]->object))->GetParameterUnit(((ErrorModel*)((*map)[i]->object))->GetParameterID("PassBiases"));
         buffer << GmatStringUtil::GetAlignmentString(unit, 5, GmatStringUtil::LEFT);

         Integer passIndex = (*map)[i]->subelement - 1;
         // 2.3. Write pass bias' apriori value
         Real val = ((ErrorModel*)((*map)[i]->object))->GetPassBias(passIndex);
         buffer << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(val, false, false, true, 8), 22, GmatStringUtil::RIGHT);

         // 2.4. Write pass bias' standard deviation
         val = ((ErrorModel*)((*map)[i]->object))->GetRealParameter("BiasSigma");
         buffer << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(val, false, true, true, 8), 19, GmatStringUtil::RIGHT);

         // 2.5. Write bias span start and end
         buffer << "     " << ((ErrorModel*)((*map)[i]->object))->GetTimeRangeStartInUTCGregorian(passIndex) << "  " << ((ErrorModel*)((*map)[i]->object))->GetTimeRangeEndInUTCGregorian(passIndex);
         buffer << "\n";
      }
   }

   // 1. Write table containing pass bias' information
   if (buffer.str() != "")
   {
      textFile << GmatStringUtil::GetAlignmentString("", 70) + "Pass Bias Time Spans\n";
      textFile << "\n";
      textFile << " Bias State Name                                          Units         Apriori Value      Standard Dev.     Bias Span Start           Bias Span End" << "\n";
      textFile << "\n";
      textFile << buffer.str();
      textFile << "\n";
      textFile << "\n";

      textFile.flush();
   }
}


//-------------------------------------------------------------------------
// void Estimator::WriteReportFileHeaderPart4_4()
//-------------------------------------------------------------------------
/**
* Write information about Data Filter to estimation report file
*/
//-------------------------------------------------------------------------
void Estimator::WriteReportFileHeaderPart4_4()
{
   // 1. Get a list of all statistics data filters
   ObjectArray dataFilters = GetMeasurementManager()->GetStatisticsDataFilters();
   Integer estdfStart = dataFilters.size();
   StringArray recAcceptList;
   StringArray recRejectList;

   // 2. Append the list of all estimation data filters
   // Add them to objList
   for (Integer i = 0; i < dataFilterObjs.size(); ++i)
   {
      // Get name of dataFilterObjs[i]
      std::string name = dataFilterObjs[i]->GetName();

      // if it is not already in dataFilters list, then add it to the list
      bool found = false;
      for (Integer j = 0; j < dataFilters.size(); ++j)
      {
         if (name == dataFilters[j]->GetName())
         {
            found = true;
            break;
         }
      }

      if (!found)
         dataFilters.push_back(dataFilterObjs[i]);
   }

   if (dataFilters.empty())
      return;
   
   // 3. Write table containing data filters' information
   textFile << GmatStringUtil::GetAlignmentString("", 66) + "Tracking Data Editing Criteria\n";
   textFile << "\n";
   textFile << " Filter Name           Type    Initial Epoch             Final Epoch               Trackers                      Data Types                           Thin Rate  " << "\n";
   textFile << "\n";
   
   std::string accepts = "";
   std::string rejects = "";
   for (Integer i = 0; i < dataFilters.size(); ++i)
   {
      StringArray lines;
      lines.push_back("");
      Integer lineNo = lines.size() - 1;

      // 3.1. Write filter name
      lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString(dataFilters[i]->GetName(), 21);

      // 3.2. Write filter type
      std::string dftype = "";
      if (dataFilters[i]->IsOfType("AcceptFilter") || dataFilters[i]->IsOfType("StatisticsAcceptFilter"))
         dftype = "Accept";
      else if (dataFilters[i]->IsOfType("RejectFilter") || dataFilters[i]->IsOfType("StatisticsRejectFilter"))
         dftype = "Reject";
      lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString(dftype, 7);

      // 3.3. Initial Epoch
      Real epoch = dataFilters[i]->GetRealParameter("InitialEpoch");
      Real epochUTC;
      std::string epochUTCs;
      theTimeConverter->Convert("A1ModJulian", epoch, "", "UTCGregorian", epochUTC, epochUTCs);
      lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString(epochUTCs, 25);

      // 3.4. Final Epoch
      epoch = dataFilters[i]->GetRealParameter("FinalEpoch");
      // epochUTC; -- typo, unused line?
      //epochUTCs; -- typo, unused line?
      theTimeConverter->Convert("A1ModJulian", epoch, "", "UTCGregorian", epochUTC, epochUTCs);
      lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString(epochUTCs, 25);

      // 3.5. Trackers
      ObjectArray trackers = dataFilters[i]->GetRefObjectArray(Gmat::GROUND_STATION);
      std::string names;
      Integer j = 0;
      lineNo = 0;
      bool start = true;
      Integer columnSize = 29;             // size of a column. For Trackers column, its size is 29
      for (; j < trackers.size(); ++j)
      {
         if (start)
         {
            start = false;
            names = trackers[j]->GetName();
         }
         else
         {
            std::string s = names + "," + trackers[j]->GetName();
            if (s.size() > columnSize)
            {
               // write out to current line
               Integer len = lines[lineNo].size();
               lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString(names, columnSize);

               // Create a new line
               ++lineNo;
               if (lineNo == lines.size())
                  lines.push_back(GmatStringUtil::GetAlignmentString("", len));
               names = trackers[j]->GetName();
            }
            else
               names = s;
         }
      }
      lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString(names, columnSize);

      ++lineNo;
      for (; lineNo < lines.size(); ++lineNo)
         lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString("", columnSize);


      // 3.6. Data Types
      StringArray dataTypes = dataFilters[i]->GetStringArrayParameter("DataTypes");
      j = 0;
      lineNo = 0;
      start = true;
      columnSize = 36;             // size of a column. For DataTypes column, its size is 36
      for (; j < dataTypes.size(); ++j)
      {
         if (start)
         {
            start = false;
            names = dataTypes[j];
         }
         else
         {
            std::string s = names + "," + dataTypes[j];
            if (s.size() > columnSize)
            {
               // write out to current line
               Integer len = lines[lineNo].size();
               lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString(names, columnSize);

               // Create a new line
               ++lineNo;
               if (lineNo == lines.size())
                  lines.push_back(GmatStringUtil::GetAlignmentString("", len));
               names = dataTypes[j];
            }
            else
               names = s;
         }
      }
      lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString(names, columnSize);
      
      ++lineNo;
      for (; lineNo < lines.size(); ++lineNo)
         lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString("", columnSize);

      // 3.7. Thin Rate
      columnSize = 10;                             // size of Thin Rate column is set to 10
      std::string thinRate = "";
      if (dataFilters[i]->IsOfType("AcceptFilter") || dataFilters[i]->IsOfType("StatisticsAcceptFilter"))
      {
         AcceptFilter* af = (AcceptFilter*)dataFilters[i];
         std::string mode = af->GetStringParameter("ThinMode");
         Integer thinFreq = af->GetIntegerParameter("ThinningFrequency");

         std::stringstream ss;
         if (mode == "Frequency")
            ss << "1:" << thinFreq;
         else if (mode == "Time")
            ss << thinFreq << " second";

         thinRate = ss.str();
      }
      lineNo = 0;
      lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString(thinRate, columnSize);

      ++lineNo;
      for (; lineNo < lines.size(); ++lineNo)
         lines[lineNo] = lines[lineNo] + " " + GmatStringUtil::GetAlignmentString("", columnSize);


      // 3.8. Write lines to textFile
      for (Integer j = 0; j < lines.size(); ++j)
         textFile << lines[j] << "\n";

      if (i >= estdfStart)
      {
         // 3.9. Get list of accepted record number
         if (dataFilters[i]->IsOfType("AcceptFilter"))
         {
            AcceptFilter* af = (AcceptFilter*)dataFilters[i];
            StringArray recordNums = af->GetStringArrayParameter(af->GetParameterID("RecordNumbers"));
            for (Integer j = 0; j < recordNums.size(); ++j)
            {
               if (accepts == "")
                  accepts = recordNums[j];
               else
                  accepts = accepts + ", " + recordNums[j];
            }
         }
         else if (dataFilters[i]->IsOfType("RejectFilter"))
         {
            RejectFilter* rf = (RejectFilter*)dataFilters[i];
            StringArray recordNums = rf->GetStringArrayParameter(rf->GetParameterID("RecordNumbers"));
            for (Integer j = 0; j < recordNums.size(); ++j)
            {
               if (rejects == "")
                  rejects = recordNums[j];
               else
                  rejects = rejects + ", " + recordNums[j];
            }
         }
      }

   }
   textFile << "\n";
   textFile << "\n";
   
   
   textFile << " Observations Accepted by Record Number : " << accepts << "\n";
   textFile << " Observations Rejected by Record Number : " << rejects << "\n";
   textFile << "\n";

   textFile.flush();

   // Clean up memory
   dataFilters.clear();
   recAcceptList.clear();
   recRejectList.clear();
}


//----------------------------------------------------------------------
// void WriteReportFileHeaderPart4()
//----------------------------------------------------------------------
/**
* Write Measurement Modeling section to estimation report file 
*/
//----------------------------------------------------------------------
void Estimator::WriteReportFileHeaderPart4()
{
   // 1. Write subheader
   textFile << "********************************************************************  MEASUREMENT MODELING  ********************************************************************\n";
   textFile << "\n";

   // 2. Write information about tracking file sets
   WriteReportFileHeaderPart4_1();
   
   // 3. Write information about ground stations
   WriteReportFileHeaderPart4_2();
   
   // 4. Write information about error models
   WriteReportFileHeaderPart4_3();

   // 5. Write information about pass bias
   WriteReportFileHeaderPart4_3b();

   // 6. Write information about data filters
   WriteReportFileHeaderPart4_4();
}


StringArray Estimator::ColumnBreak(std::string content, Integer colLenMax, StringArray seperators)
{
   StringArray sa;
   if (content == "")
   {
      sa.push_back(content);
      return sa;
   }

   std::string s1 = content;
   while (s1 != "")
   {
      // 1. Specify lastIndex
      std::string s = s1;
      Integer lastIndex = 0;
      Integer i = 0;
      for (; i < s.size(); ++i)
      {
         // Is s[i] is a seperator?
         bool isSeperator = false;
         for (Integer j = 0; j < seperators.size(); ++j)
         {
            std::string sep = seperators[j];
            std::string s2 = s.substr(i, sep.size());
            if (s2 == sep)
            {
               isSeperator = true;
               break;
            }
         }

         if (isSeperator)
         {
            if (i <= colLenMax)
               lastIndex = i;
            else
               break;
         }
      }
      if (lastIndex == 0)
         lastIndex = s.size();

      // 2. Get left string and right string 
      std::string ls = s1.substr(0, lastIndex);
      std::string rs = s1.substr(lastIndex);

      // 3. Add left string to the list
      sa.push_back(ls);

      // 4. reset content
      s1 = rs;
   }

   return sa;
}


//*******************************************************************  ASTRODYNAMIC CONSTANTS  *******************************************************************
//
//Earth Orientation Parameters / common / eopc04_08.62 - now
//Atmospheric Model Data File                         Not used
//Central Body Gravity Model / home / sslojkow / gmat / grav / grgm900c.cof
//Planetary Ephemeris File / apps / gmat / data / planetary_ephem / spk / de421.bsp
//
//Planetary Ephemeris                                   DE421
//Solar Irradiance(W / m ^ 2 at 1 AU)                      1358.0
//Speed of Light(km / sec)                               1234567.123
//Universal Gravitational Constant(gm*km ^ 3 / sec ^ 2)      6.6730000e-22
//
//Central Body                              Earth               Luna                 Sun
//Gravitational Constant(km ^ 3 / sec ^ 2)       3.9860044000e+06    4.90279920000e+04    1.32712200000e+12
//Mean Equatorial Radius(km)               6.3781400000e+04    6.37814000000e+04    6.37814000000e+04
//Inverse Flattening Coefficient            2.9825700000e+03    1.00000000000e+00    1.00000000000e+00
//Rotation Rate(rad / sec)                   7.2921159000e-04    7.29211590000e-04    7.29211590000e-04

//----------------------------------------------------------------------
// void WriteReportFileHeaderPart5()
//----------------------------------------------------------------------
/**
* Write Astrodynamic Constants section to estimation report file
*/
//----------------------------------------------------------------------
void Estimator::WriteReportFileHeaderPart5()
{
   // 1. Write astrodynamic contants header
   textFile << "*******************************************************************  ASTRODYNAMIC CONSTANTS  *******************************************************************\n";
   textFile << "\n";

   //Planet* earth = (Planet*)(solarSystem->GetBody("Earth"));
   //Integer id = earth->GetParameterID("EopFileName");
   //std::string eopFileName = earth->GetStringParameter(id);
   std::string eopFileName = GmatGlobal::Instance()->GetEopFile()->GetFileName();
   std::string ephFileName = solarSystem->GetStringParameter("DEFilename");
   std::string ephSource = solarSystem->GetStringParameter("EphemerisSource");

   // Check if Ephemeris source is SPICE then get SPK filename 
   if (ephSource == "SPICE")
      ephFileName = solarSystem->GetStringParameter("SPKFilename");


   bool useWraping = false;             // it does not warp text when the line is too long

   StringArray seperators;
   seperators.push_back("\\");
   seperators.push_back("/");
   
   // 1.1. Earth orientation parameters
   StringArray contents;
   if (eopFileName == "")
      contents.push_back("Not used");
   else
   {
      if (useWraping)
         contents = ColumnBreak(eopFileName, 107, seperators);
      else
         contents.push_back(eopFileName.substr(0, 107));
   }

   textFile << " Earth Orientation Parameters                        " << contents[0] << "\n";
   for (Integer i = 1; i < contents.size(); ++i)
      textFile << "                                                     " << contents[i] << "\n";

   // 1.2. Atmosphere model data file:
   // For now, scan through the propagators and use the first ODEModel found
   ObjectArray forces;
   for (UnsignedInt i = 0; i < propagators.size(); ++i)
   {
      ODEModel *odeForce = propagators[i]->GetODEModel();
      if (odeForce)
      {
         forces = odeForce->GetRefObjectArray("PhysicalModel");
         break;
      }
   }

   DragForce* df = NULL;
   for (Integer i = 0; i < forces.size(); ++i)
   {
      if (forces[i]->IsOfType("DragForce"))
      {
         df = (DragForce*)forces[i];
         break;
      }
   }
   if (df == NULL)
      textFile << " Atmospheric Model Data File                         " << "Not used" << "\n";
   else
   {
      std::string amtFileName = df->GetStringParameter("CSSISpaceWeatherFile");
      contents.clear();
      if (amtFileName == "")
         contents.push_back("Not used");
      else
      {
         if (useWraping)
            contents = ColumnBreak(amtFileName, 107, seperators);
         else
            contents.push_back(amtFileName.substr(0, 107));
      }

      textFile << " Atmospheric Model Data File                         " << contents[0] << "\n";
      for (Integer i = 1; i < contents.size(); ++i)
         textFile << "                                                     " << contents[i] << "\n";
   }

   // 1.3. Central body gravity model:
   GravityField* gf = NULL;
   for (Integer i = 0; i < forces.size(); ++i)
   {
      if (forces[i]->IsOfType("GravityField"))
      {
         gf = (GravityField*)forces[i];
         break;
      }
   }
   if (gf == NULL)
      textFile << " Central Body Gravity Model                          " << "Not used" << "\n";
   else
   {
      std::string potFileName = gf->GetStringParameter("PotentialFileFullPath");
      contents.clear();
      if (potFileName == "")
         contents.push_back("Not used");
      else
      {
         if (useWraping)
            contents = ColumnBreak(potFileName, 107, seperators);
         else
            contents.push_back(potFileName.substr(0, 107));
      }

      textFile << " Central Body Gravity Model                          " << contents[0] << "\n";
      for (Integer i = 1; i < contents.size(); ++i)
         textFile << "                                                     " << contents[i] << "\n";
   }

   // 1.4. Planetary ephemeris file:
   contents.clear();
   if (ephFileName == "")
      contents.push_back("Not used");
   else
   {
      if (useWraping)
         contents = ColumnBreak(ephFileName, 107, seperators);
      else
         contents.push_back(ephFileName.substr(0, 107));
   }

   textFile << " Planetary Ephemeris File                            " << contents[0] << "\n";
   for (Integer i = 1; i < contents.size(); ++i)
      textFile << "                                                     " << contents[i] << "\n";


   // 1.5. Astrodynamic constants:
   textFile << "\n";
   textFile << " Planetary Ephemeris                                 " << ephSource << "\n";
   textFile << " Speed of Light (km/sec)                             " << GmatStringUtil::RealToString(GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / 1000.0, false, false, false, 6) << "\n";
   textFile << " Universal Gravitational Constant (km^3/kg*sec^2)    " << GmatStringUtil::RealToString(GmatPhysicalConstants::UNIVERSAL_GRAVITATIONAL_CONSTANT, false, true, true, 6) << "\n";
   textFile << "\n";

   // 2. Write information about central bodies to report file
   // 2.1. Get all central body objects
   StringArray nameList, cbNames;
   std::string name = "";
   // 2.1.1. Add central body's name from participants to cbNames
   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   for (Integer i = 0; i < participantNames.size(); ++i)
   {
      // Get name of central body from participants
      name = "";
      GmatBase* obj = GetConfiguredObject(participantNames[i]);
      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         // Get central body objects used in spacecrafts' coordinate system
         Spacecraft *sc = (Spacecraft*)obj;
         std::string csName = sc->GetStringParameter("CoordinateSystem");
         CoordinateSystem *cs = NULL;
         // undo code to handle bug GMT-5619 due to it was handle by Spacecraft's code
         //try
         //{
            cs = (CoordinateSystem *)GetConfiguredObject(csName);
         //}
         //catch (...)
         //{
         //   throw EstimatorException("Error: CoordinateSystem object with name '" + csName + "' set to " + sc->GetName() + ".CoordinateSystem was not defined in GMAT script.\n");
         //}
         // name = cs->GetStringParameter("Origin");

         SpacePoint* origin = cs->GetOrigin();
         if (origin->IsOfType(Gmat::GROUND_STATION))
         {
            GroundstationInterface *gs = (GroundstationInterface*)origin;
            name = gs->GetStringParameter("CentralBody");
         }
         else
            name = origin->GetName();
      }
      else if (obj->IsOfType(Gmat::GROUND_STATION))
      {
         // Get central body objects used in ground stations' central body
         GroundstationInterface *gs = (GroundstationInterface*)obj;
         name = gs->GetStringParameter("CentralBody");
      }
      
      bool found = false;
      for (Integer i = 0; i < nameList.size(); ++i)
      {
         if (nameList[i] == name)
         {
            found = true;
            break;
         }
      }

      if (!found)
         nameList.push_back(name);
   }
   
   // 2.1.2. Add central body's name used in force models to cbNames
   PropSetup* propSetup = propagators[0];
   ODEModel* ode = propSetup->GetODEModel();
   if (ode != NULL)
      name = ode->GetStringParameter("CentralBody");
   else
      name = propSetup->GetStringParameter("CentralBody");
   nameList.push_back(name);

   // 2.1.3. Add all point masses
   StringArray sa;
   if (ode != NULL)
      sa = ode->GetStringArrayParameter("PointMasses");

   for (UnsignedInt i = 0; i < sa.size(); ++i)
      nameList.push_back(sa[i]);

   // 2.1.4. Create a list of all celestial bodies
   bool found;
   for (UnsignedInt i = 0; i < nameList.size(); ++i)
   {
      if (nameList[i] == "")
         continue;

      found = false;
      for (Integer j = 0; j < cbNames.size(); ++j)
      {
         if (cbNames[j] == nameList[i])
         {
            found = true;
            break;
         }
      }
      if (!found)
         cbNames.push_back(nameList[i]);
   }


   // 2.2. Write information about central body
   StringArray paramNames, paramValues, rowContent;
   
   // 2.2.1. Set value to paramNames
   paramNames.push_back("Celestial Body");
   paramNames.push_back("Gravitational Constant (km^3/sec^2)");
   paramNames.push_back("Mean Equatorial Radius (km)");
   paramNames.push_back("Inverse Flattening Coefficient");
   paramNames.push_back("Rotation Rate (deg/day)");

   Integer nameLen = 0;
   for (Integer i = 0; i < paramNames.size(); ++i)
      nameLen = (Integer)GmatMathUtil::Max(nameLen, paramNames[i].size());

   std::stringstream ss;
   Integer colCount = 0;
   for (Integer i = 0; i < cbNames.size(); ++i)
   {
      // Get central body object
      CelestialBody* cb = solarSystem->GetBody(cbNames[i]);
      if (cb == NULL)
         continue;
      
      // Set value to paramValues
      paramValues.push_back(cb->GetName());                                                                                 // Central Body
      ss.str(""); ss << GmatStringUtil::RealToString(cb->GetRealParameter(cb->GetParameterID("Mu")), false, false, false, 8); paramValues.push_back(ss.str());                    // Gravitational Constant
      ss.str(""); ss << GmatStringUtil::RealToString(cb->GetRealParameter(cb->GetParameterID("EquatorialRadius")), false, false, false, 8); paramValues.push_back(ss.str());      // Mean Equatorial Radius
      ss.str(""); ss << GmatStringUtil::RealToString(1.0 / cb->GetRealParameter(cb->GetParameterID("Flattening")), false, false, false, 8); paramValues.push_back(ss.str());      // Inverse Flattening Cofficient
      ss.str(""); ss << GmatStringUtil::RealToString(cb->GetRealParameter(cb->GetParameterID("RotationRate")), false, false, false, 8); paramValues.push_back(ss.str());          // Rotation Rate

      Integer valueLen = 20;
      for (Integer j = 0; j < paramValues.size(); ++j)
         valueLen = (Integer)GmatMathUtil::Max(valueLen, paramValues[j].size());

      // Set value for rowContent
      if (colCount == 0)
      {
         for (Integer j = 0; j < paramNames.size(); ++j)
            rowContent.push_back(GmatStringUtil::GetAlignmentString(paramNames[j], nameLen+5));
      }

      for (Integer j = 0; j < paramNames.size(); ++j)
         rowContent[j] += GmatStringUtil::GetAlignmentString(paramValues[j], valueLen+1);
      
      // increase colCount by 1
      ++colCount;

      // break the table as needed
      if (colCount == CELESTIAL_BODIES_TABLE_COLUMN_BREAK_UP)
      {
         for (Integer j = 0; j < rowContent.size(); ++j)
            textFile << " " << rowContent[j] << "\n";
         textFile << "\n";

         rowContent.clear();
         colCount = 0;
      }
      
      // clear paramValues
      paramValues.clear();
   }
   
   for (Integer j = 0; j < rowContent.size(); ++j)
      textFile << " " << rowContent[j] << "\n";
   textFile << "\n";

   textFile.flush();

   // Clean up memory
   paramNames.clear();
   rowContent.clear();
   forces.clear();
   nameList.clear();
   cbNames.clear();
   participantNames.clear();
   seperators.clear();
   contents.clear();
}


//----------------------------------------------------------------------
// void WriteIterationHeader()
//----------------------------------------------------------------------
/**
* Write iteration header
*/
//----------------------------------------------------------------------
void Estimator::WriteIterationHeader()
{
   /// 1. Write iteration header
   textFile
      << CenterAndPadHeader(GetHeaderName() + " MEASUREMENT RESIDUALS") << "\n"
      << "\n";

   textFile
      << "                                                                  Notations Used In Report File\n"
      << "\n";
   WriteNotationHeader();
   textFile
      << "\n"
      << "                                                                  Measurement and Residual Units\n"
      << "\n";
   WriteMeasurementHeader();

   textFile.flush();

   WritePageHeader();
}


//----------------------------------------------------------------------
// void WriteNotationHeader()
//----------------------------------------------------------------------
/**
* Write notation header
*/
//----------------------------------------------------------------------
void Estimator::WriteNotationHeader()
{
   textFile
      << "                         -    : Not edited                                                    IRMS : Edited by initial RMS sigma filter\n"
      << "                         U    : Unused because no computed value configuration available      OLSE : Edited by outer-loop sigma editor\n"
      << "                         R    : Out of ramp table range                                       ILSE : Edited by inner-loop sigma editor\n"
      << "                         BXY  : Blocked, X = Path index, Y = Count index(Doppler)             USER : Edited by second-level data editor\n"
      << "                         HORP : Blocked by Height of Ray Path criteria                        EGAP : Edited due to ephemeris gap\n";
}


//----------------------------------------------------------------------
// void WriteMeasurementHeader()
//----------------------------------------------------------------------
/**
* Write measurement header
*/
//----------------------------------------------------------------------
void Estimator::WriteMeasurementHeader()
{
   textFile
      << "              Azimuth                   degrees               Range                 kilometers                RangeRate       kilometers/second\n"
      << "              Elevation                 degrees               Range_Skin            kilometers                DSN_TCP         Hertz\n"
      << "              XEast                     degrees               DSN_SeqRange          Range Units               SN_Doppler      Hertz\n"
      << "              XSouth                    degrees               DSN_PNRange           Range Units               SN_Doppler_Rtn  Hertz\n"
      << "              YNorth                    degrees               GPS_PosVec            kilometers                SN_DOWD         Hertz\n"
      << "              YEast                     degrees               SN_Range              kilometers                BRTS_Doppler    Hertz\n"
      << "                                                              BRTS_Range            kilometers                                     \n";
}


//----------------------------------------------------------------------
// void WritePageHeader()
//----------------------------------------------------------------------
/**
* Write page header
*/
//----------------------------------------------------------------------
void Estimator::WritePageHeader()
{
   /// 4.1. Write page header
   textFile << "\n";

   if (IsIterative())
      textFile << "Iter ";

   textFile << GetMeasurementResidualTitles();
   
   if (textFileMode != "Normal")
   { 
      // fill out N/A for partial derivative
      for (int i = 0; i < esm.GetStateMap()->size() - 1; ++i)
         textFile << GmatStringUtil::GetAlignmentString(" ", 20);
      textFile << "   Uplink-Band        Uplink-Frequency            Range-Modulo        Doppler-Interval    HeightOfRayPath    CentralAngleOfRayPath\n";
   }
   textFile << "\n";

   textFile.flush();
}


//----------------------------------------------------------------------
// std::string GetHeaderName()
//----------------------------------------------------------------------
/**
* Write the name of the estimator type in upper case for report headers
*/
//----------------------------------------------------------------------
std::string Estimator::GetHeaderName()
{
   return "ESTIMATOR";
}


//----------------------------------------------------------------------
// std::string GetHeaderName()
//----------------------------------------------------------------------
/**
* Write the name of the estimator type in upper case for report headers
*/
//----------------------------------------------------------------------
std::string Estimator::CenterAndPadHeader(std::string text)
{
   UnsignedInt colWidth = 160U;
   char padChar = '*';
   std::string retStr = "  " + text + "  ";
   UnsignedInt width = (colWidth - retStr.length())/2;
   std::string padChars(width, padChar);
   retStr = padChars + retStr + padChars;

   if (retStr.length() < colWidth)
      retStr += padChar;

   return retStr;
}


//----------------------------------------------------------------------
// std::string GetUnit(std::string type)
//----------------------------------------------------------------------
/**
* Get unit for a given observation data type
*/
//----------------------------------------------------------------------
std::string Estimator::GetUnit(const std::string type) const
{
   std::string unit = "";
   if (type == "GPS_PosVec")
      unit = "km";
   else if (type == "DSN_SeqRange")
      unit = "RU";
   else if (type == "DSN_TCP")
      unit = "Hz";
   else if (type == "DSN_PNRange")
      unit = "RU";
   else if (type == "Range")
      unit = "km";
   else if (type == "SN_Range")
      unit = "km";
   else if (type == "BRTS_Range")
      unit = "km";
   else if (type == "RangeRate")
      unit = "km/s";
   else if (type == "SN_Doppler")
      unit = "Hz";
   else if (type == "SN_Doppler_Rtn")
      unit = "Hz";
   else if (type == "SN_DOWD")
      unit = "Hz";
   else if (type == "BRTS_Doppler")
      unit = "Hz";
   else if (type == "Azimuth")
      unit = "deg";
   else if (type == "Elevation")
      unit = "deg";
   else if (type == "XEast")
      unit = "deg";
   else if (type == "YNorth")
      unit = "deg";
   else if (type == "XSouth")
      unit = "deg";
   else if (type == "YEast")
      unit = "deg";
   else if (type == "RightAscension")
      unit = "deg";
   else if (type == "Declination")
      unit = "deg";
   else if (type == "Range_Skin")
      unit = "km";

   return unit;
}


//----------------------------------------------------------------------
// std::string WriteObservationSummary(ObsFilterType filter1, ObsFilterType filter2)
//----------------------------------------------------------------------
/**
 * Generate an Observation Summary table for the report. The observations can be
 * grouped by selecting different filter criteria
 *
 * @param filter1 The first filter to group the measurements by for summarizing
 * @param filter2 The second filter to group the first filter's results by
 *
 * @return A string of the observation summary table for the report
 */
//----------------------------------------------------------------------
std::string Estimator::WriteObservationSummary(ObsFilterType filter1, ObsFilterType filter2)
{
   std::stringstream textStream;

   std::string title = "Observation Summary";
   std::string column1 = "", column2 = "";

   StringArray stationListSorted = stationsList;
   std::sort(stationListSorted.begin(), stationListSorted.end());
   StringArray measTypesListSorted = measTypesList;
   std::sort(measTypesListSorted.begin(), measTypesListSorted.end());

   StringArray filterList1, filterList2;
   StringArray filterNames1, filterNames2; // contains the names to put in the report file
   StringArray stationsPadList; // To contain station Pad # and station name in one string for output.

   for (UnsignedInt ii = 0; ii < stationListSorted.size(); ii++)
   {
      std::string padStation = stationListSorted[ii];
      StringArray participants = GetMeasurementManager()->GetParticipantList();

      for (Integer jj = participants.size()-1; jj >= 0; jj--)
      {
         GmatBase* obj = GetConfiguredObject(participants[jj]);
         if (obj->IsOfType(Gmat::GROUND_STATION) && obj->GetStringParameter("Id") == stationListSorted[ii])
         {
            padStation = GmatStringUtil::GetAlignmentString(stationListSorted[ii], 4) + " " + obj->GetName();
            break;
         }
      }

      stationsPadList.push_back(padStation);
   }

   switch (filter1)
   {
   case STATION:
      title += " by Station";
      column1 = "Pad# Station";
      filterList1 = stationListSorted;
      filterNames1 = stationsPadList;
      break;
   case DATATYPE:
      title += " by Data Type";
      column1 = "Data Type";
      filterList1 = measTypesListSorted;
      filterNames1 = measTypesListSorted;
      break;
   default:
      column1 = "";
   }

   switch (filter2)
   {
   case STATION:
      title += " and Station";
      column2 = "Pad# Station";
      filterList2 = stationListSorted;
      filterNames2 = stationsPadList;
      break;
   case DATATYPE:
      title += " and Data Type";
      column2 = "Data Type";
      filterList2 = measTypesListSorted;
      filterNames2 = measTypesListSorted;
      break;
   default:
      column2 = "";
   }

   column1 = GmatStringUtil::GetAlignmentString(column1, 20, GmatStringUtil::LEFT);
   column2 = GmatStringUtil::GetAlignmentString(column2, 20, GmatStringUtil::LEFT);

   // Write table header
   textStream << "\n";
   textStream << GmatStringUtil::Trim(GmatStringUtil::GetAlignmentString(title, 160, GmatStringUtil::CENTER), GmatStringUtil::TRAILING) << "\n";
   textStream << "\n";
   textStream << "                                                                                Mean      Standard      Weighted     User          Mean      Standard\n";
   textStream << " " << column1       << column2       << "  Total   Accepted    Percent      Residual     Deviation           RMS   Edited      Residual     Deviation  Units\n";
   textStream << " ---------------------------------------------------------------------  ----------------------------------------  -----------------------------------  --------\n";

   UnsignedIntArray indicies;

   for (UnsignedInt ii = 0; ii < filterList1.size(); ii++)
   {
      std::string col1Name, col2Name;
      std::string gsName = "";
      std::string typeName = "";

      switch (filter1)
      {
      case Estimator::STATION:
         gsName = filterList1[ii];
         break;
      case Estimator::DATATYPE:
         typeName = filterList1[ii];
         break;
      default:
         break;
      }

      indicies = FilterObservations(gsName, typeName);

      col1Name = filterNames1[ii];

      if (filterList2.size() > 0)
         col2Name = "All";
      else
         col2Name = "";

      textStream << " "
         << GmatStringUtil::GetAlignmentString(col1Name, 19) << " "
         << GmatStringUtil::GetAlignmentString(col2Name, 19, GmatStringUtil::LEFT) << "  "
         << SummarizeObservationValues(indicies)
         << "\n";

      for (UnsignedInt jj = 0; jj < filterList2.size(); jj++)
      {
         switch (filter2)
         {
         case Estimator::STATION:
            gsName = filterList2[jj];
            break;
         case Estimator::DATATYPE:
            typeName = filterList2[jj];
            break;
         default:
            break;
         }

         indicies = FilterObservations(gsName, typeName);

         col1Name = "";
         col2Name = filterNames2[jj];

         if (indicies.size() > 0)
         {
            textStream << " "
               << GmatStringUtil::GetAlignmentString(col1Name, 19) << " "
               << GmatStringUtil::GetAlignmentString(col2Name, 19, GmatStringUtil::LEFT) << "  "
               << SummarizeObservationValues(indicies)
               << "\n";
         }
      }

      // Add extra line break between groups of 2nd level filtering, but not after last block
      if (filterList2.size() > 0 && (ii+1) < filterList1.size())
         textStream << "\n";
   }

   return textStream.str();
}


//----------------------------------------------------------------------
// UnsignedIntArray FilterObservations(std::string groundStation, std::string measType)
//----------------------------------------------------------------------
/**
 * Filter the observations based on ground station, and measurement type.
 * Providing an empty string to any parameter will indicate to not filter
 * measurements based on that parameter.
 *
 * @param groundStation Ground station name to filter by
 * @param measType Measurement type name to filter by
 *
 * @return Indicies of measStats that match the provided ground station
 *         and measurement type
 */
//----------------------------------------------------------------------
UnsignedIntArray Estimator::FilterObservations(std::string groundStation, std::string measType) const
{
   #ifdef DEBUG_REPORT
      MessageInterface::LogMessage("Filtering measurements by ground station: %s and measurement type %s \n", groundStation.c_str(), measType.c_str());
   #endif

   UnsignedIntArray matchedIndicies;

   for (UnsignedInt ii = 0; ii < measStats.size(); ii++)
   {
      bool meetsReq = true;

      // Filter based on ground station name
      if (groundStation.size() > 0)
         meetsReq = meetsReq && measStats[ii].station == groundStation;

      // Filter based on measurement type
      if (measType.size() > 0)
         meetsReq = meetsReq && measStats[ii].type == measType;

      if (meetsReq)
         matchedIndicies.push_back(ii);
   }

   return matchedIndicies;
}


//----------------------------------------------------------------------
// std::string SummarizeObservations(UnsignedIntArray indicies)
//----------------------------------------------------------------------
/**
 * Generate a string containing the statistics for the Observation Summary
 * table.
 *
 * @param indicies Indicies of measStats to use in generating the statistics
 *
 * @return String of the statistics used in the table
 */
//----------------------------------------------------------------------
std::string Estimator::SummarizeObservationValues(UnsignedIntArray indicies) const
{
   std::stringstream stringOut;

   Integer totalCount = 0, acceptCount = 0, userCount = 0;
   Integer acceptValueCount = 0, userValueCount = 0;
   Real acceptResidSum = 0.0, acceptResidWeightSq = 0.0;
   Real userResidSum = 0.0;

   std::string units = "";

   std::string dataType = measStats[indicies[0]].type;
   bool sameDataType = true;

   #ifdef DEBUG_REPORT
      MessageInterface::LogMessage("Summarizing %d measurements \n", indicies.size());
   #endif

   for (UnsignedInt ii = 0; ii < indicies.size(); ii++)
   {
      UnsignedInt index = indicies[ii];

      // Check if each index has the same data type
      sameDataType = sameDataType && (dataType == measStats[index].type);

      // Combine statistics
      totalCount++;

      if (measStats[index].editFlag == NORMAL_FLAG)
      {
         acceptCount++;

         for (UnsignedInt jj = 0; jj < measStats[index].residual.size(); jj++)
         {
            acceptValueCount++;
            acceptResidSum      += measStats[index].residual[jj];
            acceptResidWeightSq += measStats[index].residual[jj] * measStats[index].residual[jj] * measStats[index].weight[jj];
         }
      }
      else if (measStats[index].editFlag & USER_FLAG)
      {
         userCount++;

         for (UnsignedInt jj = 0; jj < measStats[index].residual.size(); jj++)
         {
            userValueCount++;
            userResidSum      += measStats[index].residual[jj];
         }
      }
   }

   // Calculate the means and WRMS
   Real meanResid = 0.0, meanResidUser = 0.0, wRMS = 0.0;
   if (acceptValueCount > 0)
   {
      meanResid = acceptResidSum / acceptValueCount;
      wRMS = GmatMathUtil::Sqrt(acceptResidWeightSq / acceptValueCount);
   }
   if (userValueCount > 0)
      meanResidUser = userResidSum / userValueCount;

   Real stdDev = 0.0, stdDevUser = 0.0;

   for (UnsignedInt ii = 0; ii < indicies.size(); ii++)
   {
      UnsignedInt index = indicies[ii];

      if (measStats[index].editFlag == NORMAL_FLAG)
      {
         for (UnsignedInt jj = 0; jj < measStats[index].residual.size(); jj++)
         {
            Real deviation = measStats[index].residual[jj] - meanResid;
            stdDev += deviation * deviation;
         }
      }
      else if (measStats[index].editFlag & USER_FLAG)
      {
         for (UnsignedInt jj = 0; jj < measStats[index].residual.size(); jj++)
         {
            Real deviation = measStats[index].residual[jj] - meanResidUser;
            stdDevUser += deviation * deviation;
         }
      }
   }

   // Calculate standard deviation and WRMS
   if (acceptValueCount > 0)
      stdDev = GmatMathUtil::Sqrt(stdDev / acceptValueCount);
   if (userValueCount > 0)
      stdDevUser = GmatMathUtil::Sqrt(stdDevUser / userValueCount);

   if (sameDataType)
      units = GetUnit(dataType);

   // Write out summary values
   stringOut << GmatStringUtil::ToString(totalCount, 6) << "     "  // Total
             << GmatStringUtil::ToString(acceptCount, 6) << "    "  // Accepted
             << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(acceptCount * 100.0 / totalCount, false, false, true, 2, 6), 6, GmatStringUtil::RIGHT) << "% "; // Percent

   if (acceptCount > 0)
   {
      if (sameDataType)
      {
         stringOut << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(meanResid, 6, true, 13), 13, GmatStringUtil::RIGHT) << " ";  // Mean Residual
         if (acceptCount > 1) // Don't write standard deviation if only one measurement
            stringOut << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(stdDev   , 6, true, 13), 13, GmatStringUtil::RIGHT) << " "; // Standard Deviation
         else
            stringOut << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "; // Standard Deviation
      }
      else
      {
         stringOut << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "  // Mean Residual
                   << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "; // Standard Deviation
      }

      stringOut << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(wRMS, 6, true, 13), 13, GmatStringUtil::RIGHT) << "   "; // Weighted RMS
   }
   else
   {
      stringOut << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "  // Mean Residual
                << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "  // Standard Deviation
                << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "   "; // Weighted RMS
   }

   if (userCount > 0)
   {
      stringOut << GmatStringUtil::ToString(userCount, 6) << " "; // User Edited

      if (sameDataType)
      {
         stringOut << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(meanResidUser, 6, true, 13), 13, GmatStringUtil::RIGHT) << " ";  // Mean Residual
         if (userCount > 1) // Don't write standard deviation if only one measurement
            stringOut << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(stdDevUser   , 6, true, 13), 13, GmatStringUtil::RIGHT) << "  "; // Standard Deviation
         else
            stringOut << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "; // Standard Deviation
      }
      else
      {
         stringOut << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "   // Mean Residual
                   << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "; // Standard Deviation
      }
   }
   else
   {
      stringOut << GmatStringUtil::GetAlignmentString("",  6, GmatStringUtil::RIGHT) << " "   // User Edited
                << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << " "   // Mean Residual
                << GmatStringUtil::GetAlignmentString("", 13, GmatStringUtil::RIGHT) << "  "; // Standard Deviation
   }
   stringOut << GmatStringUtil::GetAlignmentString(units, 6, GmatStringUtil::LEFT);       // Units

   return stringOut.str();
}


//----------------------------------------------------------------------
// void WriteReportFileSummaryPart0(Solver::SolverState sState)
//----------------------------------------------------------------------
/**
* Write observation statistics summary
*/
//----------------------------------------------------------------------
void Estimator::WriteReportFileSummaryPart0(Solver::SolverState sState)
{
   if (writeMeasurmentsAtEnd)
   {
      for (UnsignedInt ii = 0; ii < measStats.size(); ii++)
      {
         BuildMeasurementLine(measStats[ii]);
         WriteMeasurementLine(measStats[ii].recNum);
         AddMatlabData(measStats[ii]);
      }
   }
}


//----------------------------------------------------------------------
// void WriteReportFileSummaryPart2(Solver::SolverState sState)
//----------------------------------------------------------------------
/**
* Write observation statistics summary
*/
//----------------------------------------------------------------------
void Estimator::WriteReportFileSummaryPart2(Solver::SolverState sState)
{
   /// 1. Write observation summary by station and data type
   textFile2 << WriteObservationSummary(STATION, DATATYPE);
   textFile2 << "\n";

   /// 2. Write observation summary by measurement type and station
   textFile2 << WriteObservationSummary(DATATYPE, STATION);
   textFile2 << "\n";

   /// 3. Write observation summary by station
   textFile2 << WriteObservationSummary(STATION);
   textFile2 << "\n";

   /// 4. Write observation summary by measurement type
   textFile2 << WriteObservationSummary(DATATYPE);
   textFile2 << "\n";

   textFile2.flush();
}


//------------------------------------------------------------------------------------------
// void WriteReportFileSummaryPart3(Solver::SolverState sState)
//------------------------------------------------------------------------------------------
/**
* Write State Information section
*/
//------------------------------------------------------------------------------------------
void Estimator::WriteReportFileSummaryPart3(Solver::SolverState sState)
{
   const std::vector<ListItem*> *map = esm.GetStateMap();
   GmatState outputEstimationState;
   
   // 1. Write state summary header
   textFile3 << "\n";
   textFile3 << CenterAndPadHeader(GetHeaderName() + " STATE INFORMATION") << "\n";
   textFile3 << "\n";

   // 1a. Report any reduction of the normal matrix
   if (removedNormalMatrixIndexes.size() > 0) 
   {
      for (int i = 0; i < removedNormalMatrixIndexes.size(); i++)
      {
         // *** Performed normal matrix reduction for EstSat.EarthMJ2000Eq.VZ ***
         int index = removedNormalMatrixIndexes.at(i);
         std::stringstream ss;
         ss << "*** Performed normal matrix reduction for ";
         if (((*map)[index]->object->IsOfType(Gmat::MEASUREMENT_MODEL)) &&
            ((*map)[index]->elementName == "Bias"))
         {
            //MeasurementModel* mm = (MeasurementModel*)((*map)[index]->object);
            TrackingDataAdapter* mm = (TrackingDataAdapter*)((*map)[index]->object);
            StringArray sa = mm->GetStringArrayParameter("Participants");
            ss << mm->GetStringParameter("Type") << " ";
            for (UnsignedInt j = 0; j < sa.size(); ++j)
               ss << sa[j] << (((j + 1) != sa.size()) ? "," : " Bias.");
            ss << (*map)[index]->subelement;
         }
         else
            ss << GetElementFullName((*map)[index], false);
         ss << " ***";
         Integer pad_size = GmatMathUtil::Max((160 - ss.str().size()) / 2, 0);
         if (pad_size > 0)
            textFile3 << std::string(pad_size, ' ');
         textFile3 << ss.str() << "\n";
      }
      textFile3 << "\n";
   }
   
   // 2. Write estimation time
   if (IsIterative())
   {
      bool handleLeapSecond;
      GmatTime utcMjdEpoch = theTimeConverter->Convert(estimationEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
            GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
      std::string utcEpoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);
      textFile3 << " Estimation Epoch : " << utcEpoch << " UTCG\n";
   }
   else
   {
      bool handleLeapSecond;
      GmatTime utcMjdEpoch = theTimeConverter->Convert(currentEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
            GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
      std::string utcEpoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);
      textFile3 << " Final Epoch : " << utcEpoch << " UTCG\n";
   }
   textFile3 << "\n";

   
   // 3. Convert state to participants' coordinate system
   outputEstimationState = esm.GetEstimationStateForReport();

   
   // 4. Specify maximum len of elements' names (Cartisian element names)
   Integer max_len = 27;         // 27 is the maximum lenght of ancillary element names
   for (int i = 0; i < map->size(); ++i)
   {
      std::stringstream ss;
      if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL)) &&
         ((*map)[i]->elementName == "Bias"))
      {
         //MeasurementModel* mm = (MeasurementModel*)((*map)[i]->object);
         TrackingDataAdapter* mm = (TrackingDataAdapter*)((*map)[i]->object);
         StringArray sa = mm->GetStringArrayParameter("Participants");
         ss << mm->GetStringParameter("Type") << " ";
         for (UnsignedInt j = 0; j < sa.size(); ++j)
            ss << sa[j] << (((j + 1) != sa.size()) ? "," : " Bias.");
         ss << (*map)[i]->subelement;
      }
      else
         ss << GetElementFullName((*map)[i], false);
      max_len = (Integer)GmatMathUtil::Max(max_len, ss.str().size());
   }

   std::map<GmatBase*, Rvector6> aprioriKeplerianStateMap, previousKeplerianStateMap, currentKeplerianStateMap;
   std::map<GmatBase*, RealArray> aprioriAEStateMap, previousAEStateMap, currentAEStateMap;
   
   // 5.1. Calculate Keplerian state for apriori, previous, current states:
   if (IsIterative())
   {
      aprioriKeplerianStateMap = CalculateKeplerianStateMap(map, aprioriSolveForState);
      previousKeplerianStateMap = CalculateKeplerianStateMap(map, previousSolveForState);
   }
   currentKeplerianStateMap = CalculateKeplerianStateMap(map, currentSolveForState);

   // 5.2. Calculate ancillary elements for apriori, previous, current states:
   if (IsIterative())
   {
      aprioriAEStateMap = CalculateAncillaryElements(map, aprioriSolveForState);
      previousAEStateMap = CalculateAncillaryElements(map, previousSolveForState);
   }
   currentAEStateMap = CalculateAncillaryElements(map, currentSolveForState);

   // 5.3. Get previous estimation Cartesian state for all spacecraft: 
   //      Note that: the previous estimation state is the initial estimation state for the current iteration. 
   //                 It is needed for calculate [dK/dX] matrix at initial state of the current iteration 
   //std::map<GmatBase*, Rvector6> currentCartesianStateMap = CalculateCartesianStateMap(map, currentSolveForState);
   //std::map<GmatBase*, Rvector6> previousCartesianStateMap; = CalculateCartesianStateMap(map, previousSolveForState);

   
   // 6. Specify maximum len of elements' names (Keplerian element names)
   Integer len = 0;
   for (std::map<GmatBase*, Rvector6>::iterator i = currentKeplerianStateMap.begin(); i != currentKeplerianStateMap.end(); ++i)
   {
      Integer csNameSize = ((Spacecraft*)(i->first))->GetRefObject(Gmat::COORDINATE_SYSTEM, "")->GetName().size();
      len = (Integer)GmatMathUtil::Max(len, i->first->GetName().size() + csNameSize + 6);
   }
   max_len = (Integer)GmatMathUtil::Max(max_len, len);

   
   // 7. Write state information
   if (IsIterative())
   {
      textFile3 << " " << GmatStringUtil::GetAlignmentString("State Component", max_len + 4, GmatStringUtil::LEFT)
         << "Units           Current State        Apriori State      Standard Dev.       Previous State    Current-Apriori   Current-Previous\n";
   }
   else
   {
      textFile3 << " " << GmatStringUtil::GetAlignmentString("State Component", max_len + 4, GmatStringUtil::LEFT)
         << "Units           Current State      Standard Dev.\n";
   }
   textFile3 << "\n";
   textFile3.precision(8);

   // covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon
   // GTDS MatSpec Eq 8-45
   Rmatrix dX_dS = cart2SolvMatrix;                                      // [dX/dS] matrix where S is solve-for state. X is Cartesian state
   Rmatrix covar = dX_dS * informationInverse * dX_dS.Transpose();       // GTDS MatSpec Eq 8-49

   // covariance matrix w.r.t. Cr and Cd
   CovarianceEpsilonConversion(covar);
   // covariance matrix from MJ2000Eq to Spacecraft CoordinateSystem
   CartesianCovarianceRotation(covar, true);
   std::vector<GmatBase*> objectOrder;

   for (Integer i = 0; i < map->size(); ++i)
   {
      std::stringstream ss;
      //std::stringstream ss1;

      if (((*map)[i]->object->IsOfType(Gmat::MEASUREMENT_MODEL)) &&
         ((*map)[i]->elementName == "Bias"))
      {
         // Get full name for Bias
         //MeasurementModel* mm = (MeasurementModel*)((*map)[i]->object);
         TrackingDataAdapter* mm = (TrackingDataAdapter*)((*map)[i]->object);
         StringArray sa = mm->GetStringArrayParameter("Participants");
         ss << mm->GetStringParameter("Type") << " ";
         for (UnsignedInt j = 0; j < sa.size(); ++j)
            ss << sa[j] << (((j + 1) != sa.size()) ? "," : " Bias.");
         ss << (*map)[i]->subelement;

         // Get Bias unit. It is km for Range, RU for DSN_SeqRange, km/s for RangeRate, and Hz for DSN_TCP 
      }
      else
      {
         // Get full name for Bias
         ss << GetElementFullName((*map)[i], false, "Cartesian");   // display in Cartesian state
         //if ((*map)[i]->elementName == "PassBiases")
         //{
         //   ErrorModel* errmodel = (ErrorModel*)((*map)[i]->object);
         //   Integer passNum = (*map)[i]->subelement - 1;
         //   ss1 << "  in time range of " << errmodel->GetTimeRangeInUTCGregorian(passNum);
         //}
         //else
         //   ss1 << "";

      }

      std::string unit = GetElementUnit((*map)[i], "Cartesian");   // display unit w.r.t Cartesian elements
      int precision = GetElementPrecision(unit);
         
      textFile3 << GmatStringUtil::ToString(i+1, 3);
      textFile3 << " ";
      bool useScientific = false;
      if (currentSolveForStateC[i] < 1e-5 && currentSolveForStateC[i] > -1e-5)
         useScientific = true;
      textFile3 << GmatStringUtil::GetAlignmentString(ss.str(), max_len + 1, GmatStringUtil::LEFT);
      textFile3 << GmatStringUtil::GetAlignmentString(unit, 8, GmatStringUtil::LEFT);
      textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentSolveForStateC[i], false, useScientific, true, precision, 20)), 21, GmatStringUtil::RIGHT);                             // current state in Cartesian coordinate system
      if (IsIterative())
      {
         useScientific = false;
         if (aprioriSolveForStateC[i] < 1e-5 && aprioriSolveForStateC[i] > -1e-5)
            useScientific = true;
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(aprioriSolveForStateC[i], false, useScientific, true, precision, 20)), 21, GmatStringUtil::RIGHT);                             // apriori state in Cartesian coordinate system
      }
      if (covar(i, i) >= 0.0)
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(GmatMathUtil::Sqrt(covar(i, i)), false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);                    // standard deviation in Cartesian coordinate system
      else
         textFile3 << GmatStringUtil::GetAlignmentString("N/A", 19, GmatStringUtil::RIGHT);
      if (IsIterative())
      {
         useScientific = false;
         if (previousSolveForStateC[i] < 1e-5 && previousSolveForStateC[i] > -1e-5)
            useScientific = true;
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(previousSolveForStateC[i], false, useScientific, true, precision, 20)), 21, GmatStringUtil::RIGHT);                            // previous state in Cartesian coordinate system
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentSolveForStateC[i] - aprioriSolveForStateC[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);   // current state - apriori
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentSolveForStateC[i] - previousSolveForStateC[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);  // current state - previous state
      }
      
         textFile3 << "\n";

         // The objectOrder matrix is used to ensure that the Keplarian elements and Ancillary elements order matches the cartesian order. 
         bool found = false;
         for (Integer indx = 0; indx < objectOrder.size(); indx++)
         {
            if ((*map)[i]->object == objectOrder[indx])
            {
               found = true;
            }
         }
         if (found == false && (*map)[i]->object->GetTypeName() == "Spacecraft")
         {
            objectOrder.push_back((*map)[i]->object);
         }
         
   }
   textFile3 << "\n";
   

   // 8. Calculate matrix [dK/dS]
   Rmatrix dK_dS = solv2KeplMatrix.Inverse();     // GTDS MathSpec 8-45, 8-46a, and 8-46b

   
   // 9. Write Keplerian state
   // 9.1. Calculate Keplerian covariance matrix
   //Rmatrix keplerianCovar = convmatrix * covar * convmatrix.Transpose();                 // Equation 8-49 GTDS MathSpec
   Rmatrix keplerianCovar = dK_dS * informationInverse * dK_dS.Transpose();                // Equation 8-49 GTDS MathSpec

   // 9.2. Write Keplerian apriori, previous, current states
   std::vector<std::string> nameList;
   std::vector<std::string> unitList;
   RealArray aprioriArr, previousArr, currentArr, stdArr;
   if (IsIterative())
   {
      for (Integer i = 0; i < objectOrder.size(); ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
            aprioriArr.push_back(aprioriKeplerianStateMap[objectOrder[i]][j]);

         for (UnsignedInt j = 0; j < 6; ++j)
            previousArr.push_back(previousKeplerianStateMap[objectOrder[i]][j]);
      }
   }

   for (Integer i = 0; i < objectOrder.size(); ++i)
   {
      std::string csName = objectOrder[i]->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)objectOrder[i]->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      SpacePoint* origin = cs->GetOrigin();
      csName = origin->GetName() + "MJ2000Eq";

      nameList.push_back(objectOrder[i]->GetName() + "." + csName + ".SMA"); unitList.push_back("km");
      nameList.push_back(objectOrder[i]->GetName() + "." + csName + ".ECC"); unitList.push_back("");
      nameList.push_back(objectOrder[i]->GetName() + "." + csName + ".INC"); unitList.push_back("deg");
      nameList.push_back(objectOrder[i]->GetName() + "." + csName + ".RAAN"); unitList.push_back("deg");
      nameList.push_back(objectOrder[i]->GetName() + "." + csName + ".AOP"); unitList.push_back("deg");
      nameList.push_back(objectOrder[i]->GetName() + "." + csName + ".MA"); unitList.push_back("deg");
      for (UnsignedInt j = 0; j < 6; ++j)
         currentArr.push_back(currentKeplerianStateMap[objectOrder[i]][j]);

      UnsignedInt k = 0;
      for (; k < map->size(); ++k)
      {
         if ((((*map)[k]->elementName == "CartesianState") || ((*map)[k]->elementName == "KeplerianState"))
            && ((*map)[k]->object == objectOrder[i]))
            break;
      }

      for (UnsignedInt j = 0; j < 6; ++j)
      {
         if (keplerianCovar(k, k) >= 0.0)
         {
            stdArr.push_back(GmatMathUtil::Sqrt(keplerianCovar(k, k)));
         }
         else
            stdArr.push_back(-1.0);
         ++k;
      }
   }

   for (Integer i = 0; i < nameList.size(); ++i)
   {
      int precision = GetElementPrecision(unitList[i]);
      textFile3 << GmatStringUtil::ToString(i+1, 3);
      textFile3 << " ";
      textFile3 << GmatStringUtil::GetAlignmentString(nameList[i], max_len + 1, GmatStringUtil::LEFT);
      textFile3 << GmatStringUtil::GetAlignmentString(unitList[i], 8, GmatStringUtil::LEFT);
      bool useScientific = false;
      if (currentArr[i] < 1e-5  && currentArr[i] > -1e-5)
         useScientific = true;
      textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentArr[i], false, useScientific, true, precision, 20)), 21, GmatStringUtil::RIGHT);             // current state
      if (IsIterative())
      {
         useScientific = false;
         if (aprioriArr[i] < 1e-5  && aprioriArr[i] > -1e-5)
            useScientific = true;
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(aprioriArr[i], false, useScientific, true, precision, 20)), 21, GmatStringUtil::RIGHT);             // apriori state
      }
      if (stdArr[i] >= 0.0)
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(stdArr[i], false, true, true, 8, 18)), 19, GmatStringUtil::RIGHT);                       // standard deviation
      else
         textFile3 << GmatStringUtil::GetAlignmentString("N/A", 19, GmatStringUtil::RIGHT);

      if (IsIterative())
      {
         useScientific = false;
         if (previousArr[i] < 1e-5  && previousArr[i] > -1e-5)
            useScientific = true;
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(previousArr[i], false, useScientific, true, precision, 20)), 21, GmatStringUtil::RIGHT);            // previous state
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentArr[i] - aprioriArr[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);   // current state - apriori 
         textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentArr[i] - previousArr[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);  // current state - previous state
      }
      textFile3 << "\n";
   }
   textFile3 << "\n";
   

   // 10. Write ancillary elements to the summary:
   StringArray nameList1, units;

   nameList1.push_back("Right Ascension"); units.push_back("deg");
   nameList1.push_back("Declination"); units.push_back("deg");
   nameList1.push_back("Vertical Flight Path Angle"); units.push_back("deg");
   nameList1.push_back("Azimuth Angle"); units.push_back("deg");
   nameList1.push_back("Magnitude of Radius Vector"); units.push_back("km");
   nameList1.push_back("Magnitude of Velocity"); units.push_back("km/s");

   nameList1.push_back("Eccentric Anomaly"); units.push_back("deg");
   nameList1.push_back("True Anomaly"); units.push_back("deg");
   nameList1.push_back("Period"); units.push_back("min");
   nameList1.push_back("Perifocal Oblate Height"); units.push_back("km");
   nameList1.push_back("Perifocal Radius"); units.push_back("km");
   nameList1.push_back("Apofocal Oblate Height"); units.push_back("km");
   nameList1.push_back("Apofocal Radius"); units.push_back("km");
   nameList1.push_back("Mean Motion"); units.push_back("deg/day");
   nameList1.push_back("Velocity at Apoapsis"); units.push_back("km/s");
   nameList1.push_back("Velocity at Periapsis"); units.push_back("km/s");
   nameList1.push_back("Planetocentric Latitude"); units.push_back("deg");
   nameList1.push_back("Planetodetic Latitude"); units.push_back("deg");
   nameList1.push_back("Longitude"); units.push_back("deg");
   nameList1.push_back("Height"); units.push_back("km");
   nameList1.push_back("C3 Energy"); units.push_back("km2/s2");

   Integer nameLen = 0;
   Integer unitLen = 0;
   for (Integer i = 0; i < nameList1.size(); ++i)
   {
      nameLen = (Integer)GmatMathUtil::Max(nameLen, nameList1[i].size());
      unitLen = (Integer)GmatMathUtil::Max(unitLen, units[i].size());
   }
   
   if (IsIterative())
   {
      textFile3 << " " << GmatStringUtil::GetAlignmentString("Ancillary Elements", max_len + 4, GmatStringUtil::LEFT)
         << "Units           Current State        Apriori State      Standard Dev.       Previous State    Current-Apriori   Current-Previous\n";
   }
   else
   {
      textFile3 << " " << GmatStringUtil::GetAlignmentString("Ancillary Elements", max_len + 4, GmatStringUtil::LEFT)
         << "Units           Current State      Standard Dev.\n";
   }
   textFile3 << "\n";

   // Specify value of all elements:
      
   // Write each element to report file
   for (Integer i = 0; i < objectOrder.size(); ++i)
   {
      // Get spacecraft and its ancillary elements
      GmatBase* sc = objectOrder[i];
      if (currentAEStateMap.size() > 1)
         textFile3 << "  " << sc->GetName() + "\n";

      RealArray currentAE, aprioriAE, previousAE;
      currentAE = currentAEStateMap[objectOrder[i]];
      if (IsIterative())
      {
         aprioriAE = aprioriAEStateMap[sc];
         previousAE = previousAEStateMap[sc];
      }

      // Write ancillary elements information for this spacecraft to report file 
      for (Integer i = 0; i < nameList1.size(); ++i)
      {
         int precision = GetElementPrecision(units[i]);

         textFile3 << GmatStringUtil::GetAlignmentString("", 4, GmatStringUtil::LEFT);
         textFile3 << GmatStringUtil::GetAlignmentString(nameList1[i], max_len + 1, GmatStringUtil::LEFT);
         textFile3 << GmatStringUtil::GetAlignmentString(units[i], 8, GmatStringUtil::LEFT);
         if (currentAE[i] == 0.0)
         {
            //textFile3 << "               ****               ****               ****               ****               ****               ****\n";
            textFile3 << "                                                                                                                  \n";
         }
         else
         {

            bool useScientific = false;
            if (currentAE[i] < 1e-5 && currentAE[i] > -1e-5)
               useScientific = true;
            textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentAE[i], false, useScientific, true, precision, 20)), 21, GmatStringUtil::RIGHT);             // current state
            if (IsIterative())
            {
               useScientific = false;
               if (aprioriAE[i] < 1e-5 && aprioriAE[i] > -1e-5)
                  useScientific = true;
               textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(aprioriAE[i], false, useScientific, true, precision, 20)), 21, GmatStringUtil::RIGHT);             // apriori state
            }
            textFile3 << GmatStringUtil::GetAlignmentString("", 19, GmatStringUtil::RIGHT);

            if (IsIterative())
            {
               useScientific = false;
               if (previousAE[i] < 1e-5 && previousAE[i] > -1e-5)
                  useScientific = true;
               textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(previousAE[i], false, useScientific, true, precision, 20)), 21, GmatStringUtil::RIGHT);            // previous state
               textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentAE[i] - aprioriAE[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);   // current state - apriori 
               textFile3 << GmatStringUtil::GetAlignmentString(GmatStringUtil::Trim(GmatStringUtil::RealToString(currentAE[i] - previousAE[i], false, true, true, precision, 18)), 19, GmatStringUtil::RIGHT);  // current state - previous state
            }
            textFile3 << "\n";
         }

      }
      textFile3 << "\n";
   }

   textFile3.flush();
}


//------------------------------------------------------------------------------------------
// void WriteReportFileSummaryPart4(Solver::SolverState sState)
//------------------------------------------------------------------------------------------
/**
* Write Covariance/Correlation Matrix section
*/
//------------------------------------------------------------------------------------------
void Estimator::WriteReportFileSummaryPart4(Solver::SolverState sState)
{
   const std::vector<ListItem*> *map = esm.GetStateMap();
   
   /// Write covariance matrix and correlation matrix
   // 1. Write header:
   textFile4 << "\n";
   textFile4 << CenterAndPadHeader(GetHeaderName() + " COVARIANCE/CORRELATION MATRIX") << "\n";
   textFile4 << "\n";

   // 2. Write covariance and correlation matrxies in Cartesian coordiante system:
   // 2.1. Calculate the previous Cartesian state for all spacecrafts:
   //      Note that: the previous estimation state is the initial estimation state for the current iteration. 
   //                 It is needed for calculate [dK/dX] matrix at initial state of the current iteration 
   //std::map<GmatBase*, Rvector6> currentCartesianStateMap = CalculateCartesianStateMap(map, currentSolveForState);
   //std::map<GmatBase*, Rvector6> previousCartesianStateMap = CalculateCartesianStateMap(map, previousSolveForState);


   // 2.2 Get covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon 
   Rmatrix finalCovariance = informationInverse;                  // Note that: information is in solve-for state
   // GTDS MathSpec Eq 8-45, 8-46a, and 8-46b
   Rmatrix dX_dS = cart2SolvMatrix;                               // [dX/dS] matrix, where S is solve-for state. It could Cartesian or Keplerian
   // GTDS MathSpec Eq 8-49
   finalCovariance = dX_dS * finalCovariance * dX_dS.Transpose(); // finalCovariance is in Cartesian state

   // 2.3. Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
   CovarianceEpsilonConversion(finalCovariance);

   // 2.4. Write covariance matrix:
   textFile4 << GmatStringUtil::GetAlignmentString("Covariance Matrix in Cartesian Coordinate System (MJ2000Eq Axes)", 160, GmatStringUtil::CENTER) << "\n";
   textFile4 << "\n";
   textFile4 << WriteMatrixReport(finalCovariance, true);

   Rmatrix finalCorrelation(finalCovariance);
   for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
      for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
         finalCorrelation(i, j) /= sqrt(finalCovariance(i, i)*finalCovariance(j, j));

   // 2.5. Write correlation matrix:
   textFile4 << GmatStringUtil::GetAlignmentString("Correlation Matrix in Cartesian Coordinate System (MJ2000Eq Axes)", 160, GmatStringUtil::CENTER) << "\n";
   textFile4 << "\n";
   textFile4 << WriteMatrixReport(finalCorrelation);


   // 3. Calculate converison derivative matrix [dK/dS] from solve-for state to Keplerian state
   Rmatrix convmatrix = solv2KeplMatrix.Inverse();                            // GTDS MathSpec Eq 8-45, 8-46a, and 8-46b

   // 4. Write final covariance and correlation matrix for Keplerian coordinate system:
   // 4.1. Calculate covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon
   Rmatrix finalKeplerCovariance = convmatrix * informationInverse * convmatrix.Transpose();          // Equation 8-49 GTDS MathSpec

   // 4.2. Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
   CovarianceEpsilonConversion(finalKeplerCovariance);


   // 4.3. Write to report file covariance matrix in Keplerian Coordinate System:
   textFile4 << GmatStringUtil::GetAlignmentString("Covariance Matrix in Keplerian Coordinate System (MJ2000Eq Axes)", 160, GmatStringUtil::CENTER) << "\n";
   textFile4 << "\n";
   textFile4 << WriteMatrixReport(finalKeplerCovariance, true);

   finalCorrelation = finalKeplerCovariance;
   for (Integer i = 0; i < finalKeplerCovariance.GetNumRows(); ++i)
      for (Integer j = 0; j < finalKeplerCovariance.GetNumColumns(); ++j)
         finalCorrelation(i, j) /= sqrt(finalKeplerCovariance(i, i)*finalKeplerCovariance(j, j));

   // 4.4. Write to report file correlation matrix in Keplerian Coordinate System:
   textFile4 << GmatStringUtil::GetAlignmentString("Correlation Matrix in Keplerian Coordinate System (MJ2000Eq Axes)", 160, GmatStringUtil::CENTER) << "\n";
   textFile4 << "\n";
   textFile4 << WriteMatrixReport(finalCorrelation);

   textFile4.flush();
}


//------------------------------------------------------------------------------------------
// void WriteReportFileSummaryPart5(Solver::SolverState sState)
//------------------------------------------------------------------------------------------
/**
* Write additional covariance information
*/
//------------------------------------------------------------------------------------------
void Estimator::WriteReportFileSummaryPart5(Solver::SolverState sState)
{
   // Write end of file line by default
   textFile5 << "***********************************************************************  END OF REPORT  ************************************************************************\n";
   textFile5.flush();
}


//------------------------------------------------------------------------------------------
// std::string WriteMatrixReport(const Rmatrix &matrix, bool scientific) const
//------------------------------------------------------------------------------------------
/**
* Write estimation summary
*/
//------------------------------------------------------------------------------------------
std::string Estimator::WriteMatrixReport(const Rmatrix &matrix, bool scientific) const
{
   std::stringstream stringOut;

   for (Integer startIndex = 0; startIndex < matrix.GetNumColumns(); startIndex += MAX_COLUMNS)
   {
      stringOut << "               ";
      for (Integer i = startIndex; i < GmatMathUtil::Min(startIndex + MAX_COLUMNS, matrix.GetNumColumns()); ++i)
      {
         stringOut << GmatStringUtil::ToString(i + 1, 3);
         if (i < matrix.GetNumColumns() - 1)
            stringOut << "                  ";
      }
      stringOut << "\n";

      for (Integer i = 0; i < matrix.GetNumRows(); ++i)
      {
         stringOut << " " << GmatStringUtil::ToString(i + 1, 2) << "  ";
         for (Integer j = startIndex; j < GmatMathUtil::Min(startIndex + MAX_COLUMNS, matrix.GetNumColumns()); ++j)
         {
            stringOut << GmatStringUtil::GetAlignmentString(GmatStringUtil::RealToString(matrix(i, j), false, scientific, true, 12, 20), 21, GmatStringUtil::RIGHT);
         }
         stringOut << "\n";
      }
      stringOut << "\n";
   }

   return stringOut.str();
}


//------------------------------------------------------------------------------------------
// void WriteReportFileSummary(Solver::SolverState sState)
//------------------------------------------------------------------------------------------
/**
* Write estimation summary
*/
//------------------------------------------------------------------------------------------
void Estimator::WriteReportFileSummary(Solver::SolverState sState)
{
   if (sState == ESTIMATING)
   {
      if (IsIterative())
      {
         WriteReportFileSummaryPart0(sState);
      }
      else
      {
         WriteMeasurementLine(measManager.GetCurrentRecordNumber());
      }
   }
   else if (sState == CHECKINGRUN || sState == FINISHED)
   {
      WriteReportFileSummaryPart1(sState);
      WriteReportFileSummaryPart2(sState);
	  if (currentMode != INITIAL_GUESS || !IsOfType("BatchEstimator"))
	  {
		  WriteReportFileSummaryPart3(sState);
		  WriteReportFileSummaryPart4(sState);
	  }
      if (sState == FINISHED)
         WriteReportFileSummaryPart5(sState);
   }
}

//------------------------------------------------------------------------------------------
// void Estimator::WriteJsonData()
//------------------------------------------------------------------------------------------
/**
* This function outputs run data to a JSON-formatted file.
*/
//------------------------------------------------------------------------------------------
bool Estimator::WriteJsonData()
{
   std::string estEpoch, initialState, cartState, kepState, coordSystem, scName, gravParameter,
      status, finalWRMS, observations;

   StringArray nameList, cbNames;
   std::string name = "";

   if (jsonFileName != "")
   {
      if ((jsonFileName.find("/") == std::string::npos) &&
         (jsonFileName.find("\\") == std::string::npos))
      {
         FileManager *fileman = FileManager::Instance();
         std::string path = fileman->GetPathname(FileManager::OUTPUT_PATH);
         jsonFileName = path + jsonFileName;
      }

      MessageInterface::ShowMessage("JSON file will be written to "
                     "%s\n", jsonFileName.c_str());
   }
   else
      return false;

   std::ofstream outFile;
   outFile.open(jsonFileName.c_str());

   // Add central body's name from participants to cbNames
   StringArray participantNames = GetMeasurementManager()->GetParticipantList();
   for (Integer i = 0; i < participantNames.size(); ++i)
   {
      // Get name of central body from participants
      name = "";
      GmatBase* obj = GetConfiguredObject(participantNames[i]);
      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         // Get central body objects used in spacecrafts' coordinate system
         Spacecraft *sc = (Spacecraft*)obj;
         std::string csName = sc->GetStringParameter("CoordinateSystem");
         CoordinateSystem *cs = NULL;

         cs = (CoordinateSystem *)GetConfiguredObject(csName);

         SpacePoint* origin = cs->GetOrigin();
         if (origin->IsOfType(Gmat::GROUND_STATION))
         {
            GroundstationInterface *gs = (GroundstationInterface*)origin;
            name = gs->GetStringParameter("CentralBody");
         }
         else
            name = origin->GetName();
      }
      else if (obj->IsOfType(Gmat::GROUND_STATION))
      {
         // Get central body objects used in ground stations' central body
         GroundstationInterface *gs = (GroundstationInterface*)obj;
         name = gs->GetStringParameter("CentralBody");
      }

      bool found = false;
      for (Integer i = 0; i < nameList.size(); ++i)
      {
         if (nameList[i] == name)
         {
            found = true;
            break;
         }
      }

      if (!found)
         nameList.push_back(name);
   }

   PropSetup* propSetup = propagators[0];
   ODEModel* ode = propSetup->GetODEModel();
   if (ode != NULL)
      name = ode->GetStringParameter("CentralBody");
   else
      name = propSetup->GetStringParameter("CentralBody");
   nameList.push_back(name);

   // Add all point masses
   StringArray sa;
   if (ode != NULL)
      sa = ode->GetStringArrayParameter("PointMasses");

   for (UnsignedInt i = 0; i < sa.size(); ++i)
      nameList.push_back(sa[i]);

   // Create a list of all celestial bodies
   bool found;
   for (UnsignedInt i = 0; i < nameList.size(); ++i)
   {
      if (nameList[i] == "")
         continue;

      found = false;
      for (Integer j = 0; j < cbNames.size(); ++j)
      {
         if (cbNames[j] == nameList[i])
         {
            found = true;
            break;
         }
      }
      if (!found)
         cbNames.push_back(nameList[i]);
   }

   outFile << "{\n";

   // EstimationEpoch
   bool handleLeapSecond;
   GmatTime utcMjdEpoch = theTimeConverter->Convert(estimationEpochGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
         GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
   estEpoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);

   outFile << "    \"EstimationEpoch\" : \"" + estEpoch + "\",\n";

   // Retrieve state names
   EstimationStateManager *esm = GetEstimationStateManager();
   const std::vector<ListItem*> *map = esm->GetStateMap();
   bool useCS = false;
   StringArray cartesianNames, keplerianNames;

   for (UnsignedInt i = 0; i < map->size(); i++)
   {
      cartesianNames.push_back(GetElementFullName((*map)[i], useCS, "Cartesian"));
      // Anomoly type hardcoded "MA" to match Matlab functionality (see BatchEstimatorBase,Line 2265).
      // Unsure why. Needs review. CRC 10/10/24
      keplerianNames.push_back(GetElementFullNameMJ2000Eq((*map)[i], useCS, "Keplerian", "MA"));
   }
   // InitialState
   Real* initState = aprioriSolveForState.GetState();

   formatter << "    \"InitialState\" : [";

   for (UnsignedInt j = 0; j < cartesianNames.size(); j++)
   {
      if (j != 0)
         formatter << ",";
      formatter << initState[j];
   }
   formatter << "],\n";
   outFile << formatter.str();
   formatter.str("");

   // CartesianStateNames
   cartState = "    \"CartesianStateNames\" : [\"";
   for (UnsignedInt j = 0; j < cartesianNames.size(); j++)
   {
      if (j != 0)
         cartState = cartState + "\", \"";
      cartState = cartState + cartesianNames[j];
   }
   cartState = cartState + "\"],\n";
   outFile << cartState;

   // KeplerianStateNames
   kepState = "    \"KeplerianStateNames\" : [\"";
   for (UnsignedInt j = 0; j < keplerianNames.size(); j++)
   {
      if (j != 0)
         kepState = kepState + "\", \"";
      kepState = kepState + keplerianNames[j];
   }
   kepState = kepState + "\"],\n";
   outFile << kepState;

   if (dataFileStyle == "Verbose")
   {
   //GravitationalParameter
      for (Integer i = 0; i < cbNames.size(); ++i)
      {
         CelestialBody* cb = solarSystem->GetBody(cbNames[i]);
         if (cb == NULL)
            continue;

         formatter << "    \"GravitationalParameter\" : {\""
                   << cb->GetName() << "\" : "
                   << cb->GetRealParameter(cb->GetParameterID("Mu"))
                   << "},\n";

         outFile << formatter.str();
         formatter.str("");
      }

      // Currently unimplemented fields:
      // Status
      // FinalWRMS
   }

   //Observations
   outFile << "\n    \"Observations\" : [\n";
   for (Integer i = 0; i < jsonObsData.size(); ++i)
   {
      if (i != 0)
         outFile << ",\n";
      outFile << jsonObsData[i];
   }
   outFile << "\n    ],\n\n";

   //Iterations
   outFile << "    \"Iterations\" : [";
   for (Integer i = 0; i < jsonIterData.size(); ++i)
   {
      if (i != 0)
         outFile << ",";
      outFile << jsonIterData[i];
   }
   outFile << "]\n\n";


   outFile << "}";
   outFile.close();

   MessageInterface::ShowMessage("Finished Writing Estimator JSON File.\n\n");

   return true;
}

//------------------------------------------------------------------------------------------
// void Estimator::AddJsonObservationData()
//------------------------------------------------------------------------------------------
/**
* This function generates a JSON-formatted output string for observation data, and adds it to
* jsonObsData.
*/
//------------------------------------------------------------------------------------------
void Estimator::AddJsonObservationData(const MeasurementInfoType &measStat)
{
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   const ObservationData *currentObs = measManager.GetObsData(measStat.recNum);

   std::string output, epoch, type, value, weight, dopplerCountInterval;
   StringArray participants;

   output = "";

   // Write observation data only on initial iterations
   if (iterationsTaken == 0)
   {
      output = output + "        { ";
      //Epoch
      bool handleLeapSecond;
      GmatTime utcMjdEpoch = theTimeConverter->Convert(currentObs->epoch, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
            GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
      epoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);
      output = output + "\"Epoch\" : \"" + epoch + "\"";

      //Participants
      participants = currentObs->participantIDs;
      output = output + ", \"Participants\" : \"";
      for (Integer i = 0; i < participants.size(); ++i)
      {
         if (i == 0)
            output = output + participants[i];
         else
            output = output + "," + participants[i];
      }
      output = output + "\"";

      //Type
      type = currentObs->typeName;
      output = output + ", \"Type\" : \"" + type + "\"";

      //Value
      formatter << ", \"Value\" : ";
      if ((currentObs->value).size() == 1)
         formatter << (currentObs->value)[0];
      else
      {
         formatter << "[";
         for (Integer i = 0; i < (currentObs->value).size(); i++)
         {
            if (i != 0)
               formatter << ", ";
            formatter << (currentObs->value)[i];
         }
         formatter << "]";
      }
      output = output + formatter.str();
      formatter.str("");

      // Skip the following fields in Normal Mode
      if (dataFileStyle == "Verbose")
      {
         //Weight
         if (measStat.modelSize > 0)
         {
            if (measStat.isCalculated)
            {
               formatter << ", \"Weight\" : "
                         << (measStat.weight)[0];
               output = output + formatter.str();
               formatter.str("");
            }
         }

         //Frequency
         formatter << ", \"Frequency\" : " << measStat.frequency;
         output = output + formatter.str();
         formatter.str("");

         //DopplerCountInterval
         if ((currentObs->typeName == "DSN_TCP") ||
            (currentObs->typeName == "RangeRate") ||
            (currentObs->typeName == "SN_Doppler") ||
            (currentObs->typeName == "SN_Doppler_Rtn") ||
            (currentObs->typeName == "BRTS_Doppler") ||
            (currentObs->typeName == "SN_DOWD")
            )
         {
            formatter << ", \"DopplerCountInterval\" : "
                      << currentObs->dopplerCountInterval;
            output = output + formatter.str();
            formatter.str("");
         }

         //RangeModulo
         if (currentObs->typeName == "DSN_SeqRange")
         {
            formatter << ", \"RangeModulo\" : " << currentObs->rangeModulo;
            output = output + formatter.str();
            formatter.str("");
         }
      }

      output = output + "}";
      jsonObsData.push_back(output);
   }
}

//------------------------------------------------------------------------------------------
// void Estimator::AddJsonIterationData()
//------------------------------------------------------------------------------------------
/**
* This function generates a JSON-formatted output string for an iteration, and adds it to
* jsonIterData.
*/
//------------------------------------------------------------------------------------------
void Estimator::AddJsonIterationData()
{
   const std::vector<ListItem*> *stateMap = esm.GetStateMap();
   std::string output, epoch, cartState, kepState, cartCov, cartCorr, kepCov, kepCorr, compMeas;

   output = "{\n";

   // CartesianState
   cartState = "        \"CartesianState\" : [";
   kepState = "        \"KeplerianState\" : [";
   for (Integer i = 0; i < stateMap->size(); i++)
   {
      if (i == 0)
      {
         formatter << currentSolveForStateC[i];
         cartState = cartState + formatter.str();
         formatter.str("");

         formatter << currentSolveForStateK[i];
         kepState = kepState + formatter.str();
         formatter.str("");
      }
      else
      {
         formatter << currentSolveForStateC[i];
         cartState = cartState + "," + formatter.str();
         formatter.str("");

         formatter << currentSolveForStateK[i];
         kepState = kepState +"," + formatter.str();
         formatter.str("");
      }
   }
   cartState = cartState + "],\n";
   kepState = kepState + "],\n";
   output = output + cartState + kepState;

   //Covariances & Correlations

   // GTDS MathSpec Eq 8-45, 8-46a, and 8-46b
   Rmatrix dX_dS = cart2SolvMatrix;                               // [dX/dS] matrix, where S is solve-for state. It could Cartesian or Keplerian
   // GTDS MathSpec Eq 8-49
   Rmatrix finalCovariance = dX_dS * informationInverse * dX_dS.Transpose(); // finalCovariance is in Cartesian state

   // 2.3. Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
   CovarianceEpsilonConversion(finalCovariance);

   Rmatrix finalCorrelation(finalCovariance);
   for (Integer i = 0; i < finalCovariance.GetNumRows(); ++i)
      for (Integer j = 0; j < finalCovariance.GetNumColumns(); ++j)
         finalCorrelation(i, j) /= sqrt(finalCovariance(i, i)*finalCovariance(j, j));

   Rmatrix convmatrix = solv2KeplMatrix.Inverse();                            // GTDS MathSpec Eq 8-45, 8-46a, and 8-46b

   // 4. Write final covariance and correlation matrix for Keplerian coordinate system:
   // 4.1. Calculate covariance matrix w.r.t. Cr_Epsilon and Cd_Epsilon
   Rmatrix finalKeplerCovariance = convmatrix * informationInverse * convmatrix.Transpose();          // Equation 8-49 GTDS MathSpec

   // 4.2. Convert covariance matrix for Cr_Epsilon and Cd_Epsilon to covariance matrix for Cr and Cd
   CovarianceEpsilonConversion(finalKeplerCovariance);

   Rmatrix finalKeplerCorrelation(finalKeplerCovariance);
   for (Integer i = 0; i < finalKeplerCovariance.GetNumRows(); ++i)
      for (Integer j = 0; j < finalKeplerCovariance.GetNumColumns(); ++j)
         finalKeplerCorrelation(i, j) /= sqrt(finalKeplerCovariance(i, i)*finalKeplerCovariance(j, j));

   cartCov = "        \"CartesianCovariance\" : [[";
   cartCorr = "        \"CartesianCorrelation\" : [[";
   kepCov = "        \"KeplerianCovariance\" : [[";
   kepCorr = "        \"KeplerianCorrelation\" : [[";


   RealArray tempArray;
   for (Integer i = 0; i < stateMap->size(); i++)
   {
      // CartesianCovariance
      tempArray = finalCovariance.GetRow(i).GetRealArray();
      for (Integer j = 0; j < tempArray.size(); j++)
      {
         formatter << tempArray[j];
         cartCov = cartCov + formatter.str();
         formatter.str("");
         if (j != tempArray.size() - 1)
            cartCov = cartCov + ",";
      }
      cartCov = cartCov + "]";
      if (i != stateMap->size() - 1)
         cartCov = cartCov + ",[";

      if (dataFileStyle == "Verbose")
      {
         // CartesianCorrelation
         tempArray = finalCorrelation.GetRow(i).GetRealArray();
         for (Integer j = 0; j < tempArray.size(); j++)
         {
            formatter << tempArray[j];
            cartCorr = cartCorr + formatter.str();
            formatter.str("");
            if (j != tempArray.size() - 1)
               cartCorr = cartCorr + ",";
         }
         cartCorr = cartCorr + "]";
         if (i != stateMap->size() - 1)
            cartCorr = cartCorr + ",[";
      }

      // KeplerianCovariance
      tempArray = finalKeplerCovariance.GetRow(i).GetRealArray();
      for (Integer j = 0; j < tempArray.size(); j++)
      {
         formatter << tempArray[j];
         kepCov = kepCov + formatter.str();
         formatter.str("");
         if (j != tempArray.size() - 1)
            kepCov = kepCov + ",";
      }
      kepCov = kepCov + "]";
      if (i != stateMap->size() - 1)
         kepCov = kepCov + ",[";

      if (dataFileStyle == "Verbose")
      {
         // KeplerianCorrelation
         tempArray = finalKeplerCorrelation.GetRow(i).GetRealArray();
         for (Integer j = 0; j < tempArray.size(); j++)
         {
            formatter << tempArray[j];
            kepCorr = kepCorr + formatter.str();
            formatter.str("");
            if (j != tempArray.size() - 1)
               kepCorr = kepCorr + ",";
         }
         kepCorr = kepCorr + "]";
         if (i != stateMap->size() - 1)
            kepCorr = kepCorr + ",[";
      }
   }
   cartCov = cartCov + "],\n";
   cartCorr = cartCorr + "],\n";
   kepCov = kepCov + "],\n";
   kepCorr = kepCorr + "],\n";

   if (dataFileStyle == "Verbose")
      output = output + cartCov + cartCorr + kepCov + kepCorr;
   else
      output = output + cartCov + kepCov;

   // ComputedMeasurements
   if(jsonMeasData.size() == 0)
   {
      throw EstimatorException("Error: Computed Measurements for JSON data not found.\n");
   }

   compMeas = "        \"ComputedMeasurements\" : [\n";

   for (Integer i = 0; i < jsonMeasData.size(); i++)
   {
      const ObservationData *currentObs = measManager.GetObsData(jsonMeasData[i].recNum);

      if (i != 0)
         compMeas = compMeas + ",\n            { ";
      else
         compMeas = compMeas + "            { ";

      //Epoch
      bool handleLeapSecond;
      GmatTime utcMjdEpoch = theTimeConverter->Convert(currentObs->epoch, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
            GmatTimeConstants::JD_JAN_5_1941, &handleLeapSecond);
      epoch = theTimeConverter->ConvertMjdToGregorian(utcMjdEpoch.GetMjd(), handleLeapSecond);

      compMeas = compMeas + "\"Epoch\" : \"" + epoch + "\"";

      //Participants
      compMeas = compMeas + ", \"Participants\" : \"";
      for (Integer j = 0; j < jsonMeasData[i].participants.size(); j++)
      {
         if (j == 0)
//            compMeas = compMeas + "\"" + jsonMeasData[i].participants[j] + "\"";
            compMeas = compMeas + jsonMeasData[i].participants[j];
         else
//            compMeas = compMeas + ", \"" + jsonMeasData[i].participants[j] + "\"";
            compMeas = compMeas + "," + jsonMeasData[i].participants[j];
      }
      compMeas = compMeas + "\"";

      //Type
      compMeas = compMeas + ", \"Type\" : \"" + jsonMeasData[i].type + "\"";

      //Residual
      formatter << ", \"Residual\" : ";

      if((jsonMeasData[i].residual).size() == 1)
         formatter << (jsonMeasData[i].residual)[0];
      else
      {
         formatter << "[";
         for (Integer j = 0; j < (jsonMeasData[i].residual).size(); j++)
         {
            if (j != 0)
               formatter << ", ";
            formatter << (jsonMeasData[i].residual)[j];
         }
         formatter << "]";
      }
      compMeas = compMeas + formatter.str();
      formatter.str("");

      //EditFlag
      compMeas = compMeas + ", \"EditFlag\" : ";
      if (jsonMeasData[i].editFlag.empty() || jsonMeasData[i].editFlag == "N")
         compMeas = compMeas + "null";
      else
         compMeas = compMeas + "\"" + jsonMeasData[i].editFlag + "\"";

      //Value
      formatter << ", \"Value\" : ";
      if ((jsonMeasData[i].measValue).size() == 1)
         formatter << (jsonMeasData[i].measValue)[0];
      else
      {
         formatter << "[";
         for (Integer j = 0; j < (jsonMeasData[i].measValue).size(); j++)
         {
            if (j != 0)
               formatter << ", ";
            formatter << (jsonMeasData[i].measValue)[j];
         }
         formatter << "]";
      }
      compMeas = compMeas + formatter.str();
      formatter.str("");

      //Elevation
      formatter << ", \"Elevation\" : " << jsonMeasData[i].elevation;
      compMeas = compMeas + formatter.str();
      formatter.str("");

      // Skip the following fields in Normal Mode
      if (dataFileStyle == "Verbose")
      {
         //IonosphericCorrection
         formatter << ", \"IonosphericCorrection\" : " << jsonMeasData[i].ionoCorrectValue;
         compMeas = compMeas + formatter.str();
         formatter.str("");

         //TroposphericCorrection
         formatter << ", \"TroposphericCorrection\" : " << jsonMeasData[i].tropoCorrectValue;
         compMeas = compMeas + formatter.str();
         formatter.str("");

         //Partials
         compMeas = compMeas + ", \"Partials\" : [";

         for (Integer j = 0; j < jsonMeasData[i].partials.size(); j++)
         {
            if (j != 0)
               compMeas = compMeas + ",";
            formatter << jsonMeasData[i].partials[j];
            compMeas = compMeas + formatter.str();
            formatter.str("");
         }
         compMeas = compMeas + "]";
      }

      compMeas = compMeas + " }";
   }
   compMeas = compMeas + "\n        ]\n";
   output = output + compMeas;
   jsonMeasData.clear();

   output = output + "    }";
   jsonIterData.push_back(output);
}

//------------------------------------------------------------------------------------------
// void Estimator::AddJsonIterationData(const MeasurementInfoType &measStat)
//------------------------------------------------------------------------------------------
/**
* This function is used to populate jsonMeasData with computed measurements for a
* given iteration, so that they can be output to a JSON file.
*
* @param measStat
*
*/
//------------------------------------------------------------------------------------------
void Estimator::AddJsonIterationData(const MeasurementInfoType &measStat)
{
   const ObservationData *currentObs = measManager.GetObsData(measStat.recNum);

   JsonComputedMeas tmp;

   tmp.recNum = measStat.recNum;
   tmp.epoch = currentObs->epoch;
   tmp.participants = currentObs->participantIDs;
   tmp.type = currentObs->typeName;
   tmp.residual = measStat.residual;
   tmp.editFlag = measStat.removedReason;
   tmp.measValue = measStat.measValue;
   tmp.elevation = measStat.feasibilityValue;
   tmp.ionoCorrectValue = measStat.ionoCorrectValue;
   tmp.tropoCorrectValue = measStat.tropoCorrectValue;


   RealArray partials;
   for (UnsignedInt i = 0; i < measStat.hAccum.size(); i++)
   {
      RealArray derivatives = measStat.hAccum[i];

      for (UnsignedInt j = 0; j < derivatives.size(); j++)
         derivatives[j] /= GetEpsilonConversion(j);

      tmp.partials = derivatives;
   }




   jsonMeasData.push_back(tmp);
}

//------------------------------------------------------------------------------------------
// std::map<GmatBase*, Rvector6> CalculateCartesianStateMap(
//                                  const std::vector<ListItem*> *map, GmatState state)
//------------------------------------------------------------------------------------------
/**
* This function is used to calculate state in Cartesian coordinate system
*
* @param map
* @param state    state of spacecraft in Cartesian or Keplerian (in form of TA)
*
*/
//------------------------------------------------------------------------------------------
std::map<GmatBase*, Rvector6> Estimator::CalculateCartesianStateMap(const std::vector<ListItem*> *map, GmatState state)
{
   static std::map<GmatBase*, Rvector6> stateMap;
   stateMap.clear();

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if (((*map)[i]->elementName == "CartesianState") || ((*map)[i]->elementName == "KeplerianState"))
      {
         // Solve-for is a spacraft state
         Spacecraft* sc = (Spacecraft*)(*map)[i]->object;
         std::string stateType = sc->GetStringParameter("DisplayStateType");
         if (stateType == "Cartesian")
         {
            Rvector6 cState;
            cState.Set(state[i], state[i+1], state[i+2], state[i+3], state[i+4], state[i+5]);
            stateMap[(*map)[i]->object] = cState;
            i = i + 5;
         }
         else if (stateType == "Keplerian")
         {
            // Get Keplerian state in TA form
            Rvector6 cState, kState;
            kState.Set(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);

            // Convert Keplerian state in TA form to Cartesian state
            CoordinateSystem* cs = (CoordinateSystem*)(((Spacecraft*)((*map)[i]->object))->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
            SolarSystem* ss = cs->GetSolarSystem();
            SpacePoint* origin = cs->GetOrigin();
            CelestialBody * body = (CelestialBody*)origin;
            if (origin->IsOfType(Gmat::GROUND_STATION))
            {
               // for case CS is GDSTropocentric, origin is ground station GDS
               std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
               body = ss->GetBody(cbName);
            }
            
            Real mu = body->GetRealParameter(body->GetParameterID("Mu"));
            cState = StateConversionUtil::KeplerianToCartesian(mu, kState, "TA");        // Keplerian anomaly element is in form of TA due to report files only display TA value (not MA)

            stateMap[(*map)[i]->object] = cState;
            i = i + 5;
         }
         else
            throw EstimatorException("Error: Value of DisplayStateType of the spacecraft '" + 
                                     (*map)[i]->object->GetName() + 
                                     "' is (" + stateType + 
                                     ") invalid. It has to be Cartesian or Keplerian.\n");
      }
   }
   return stateMap;
}


//------------------------------------------------------------------------------------------
// std::map<GmatBase*, Rvector6> CalculateKeplerianStateMap(
//                                  const std::vector<ListItem*> *map, GmatState state)
//------------------------------------------------------------------------------------------
/**
* This function is used to calculate state in Keplerian coordinate system
*
* @param map
* @param state    state of spacecraft in Cartesian or Keplerian (in form of MA) 
*/
//------------------------------------------------------------------------------------------
std::map<GmatBase*, Rvector6> Estimator::CalculateKeplerianStateMap(const std::vector<ListItem*> *map, GmatState state)
{
   static std::map<GmatBase*, Rvector6> stateMap;
   stateMap.clear();

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if (((*map)[i]->elementName == "CartesianState") || ((*map)[i]->elementName == "KeplerianState"))
      {
         // Case1 : spacecraft state
         Spacecraft* sc = (Spacecraft*)((*map)[i]->object);
         std::string stateType = sc->GetStringParameter("DisplayStateType");
         if (stateType == "Cartesian")
         {
            Rvector6 inState,cState,kState;
            inState.Set(state[i], state[i+1], state[i+2], state[i+3], state[i+4], state[i+5]);
            CoordinateSystem* cs = (CoordinateSystem*)(((Spacecraft*)((*map)[i]->object))->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
            SolarSystem* ss = cs->GetSolarSystem();
            SpacePoint* origin = cs->GetOrigin();
            CelestialBody* body = (CelestialBody*)origin;
            if (origin->IsOfType(Gmat::GROUND_STATION))
            {
               // for case CS is GDSTropocentric, origin is ground station GDS
               std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
               body = ss->GetBody(cbName);
            }
            CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem(origin->GetName() + "mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);

            GmatTime epoch = state.GetEpochGT();
            CoordinateConverter* cv = new CoordinateConverter();
            cv->Convert(epoch, inState, cs, cState, mj2kCS);
            delete cv;
            delete mj2kCS;

            Real mu = body->GetRealParameter(body->GetParameterID("Mu"));
            kState = StateConversionUtil::CartesianToKeplerian(mu, cState, "MA");      // It has to be MA due to the estimation report file show MA value (not TA)

            if ((kState[1] < 0)||(kState[1] >= 1.0))
               MessageInterface::ShowMessage("Warning: eccentricity (%lf) is out of range [0,1) when converting Cartesian state (%lf, %lf, %lf, %lf, %lf, %lf) to Keplerian state.\n", kState[1], state[i], state[i+1], state[i+2], state[i+3], state[i+4], state[i+5]);

            stateMap[(*map)[i]->object] = kState;
            i = i + 5;
         }
         else if (stateType == "Keplerian")
         {
            // Get Keplerian state in form of TA
            Rvector6 kState;
            kState.Set(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);   // element state[i+5] is in form of TA

            // convert TA to MA
            kState[5] = StateConversionUtil::TrueToMeanAnomaly(kState[5] * GmatMathConstants::RAD_PER_DEG, kState[1]) *GmatMathConstants::DEG_PER_RAD;
            stateMap[(*map)[i]->object] = kState;
            i = i + 5;
         }
         else
            throw EstimatorException("Error: Value of DisplayStateType of the spacecraft '" +
                                     (*map)[i]->object->GetName() +
                                     "' is (" + stateType +
                                     ") invalid. It has to be Cartesian or Keplerian.\n");
      }
   }
   return stateMap;
}


//------------------------------------------------------------------------------------------
// std::map<GmatBase*, Rvector6> CalculateAncillaryElements(
//                                  const std::vector<ListItem*> *map, GmatState state)
//------------------------------------------------------------------------------------------
/**
* This function is used to calculate ancillary elements
*/
//------------------------------------------------------------------------------------------
std::map<GmatBase*, RealArray> Estimator::CalculateAncillaryElements(const std::vector<ListItem*> *map, GmatState state)
{
   static std::map<GmatBase*, RealArray> stateMap;
   stateMap.clear();
   Real elementValue;

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if (((*map)[i]->elementName == "CartesianState") || ((*map)[i]->elementName == "KeplerianState"))
      {
         /// Case 1: solve-for is a spacecraft state
         Rvector6 cState;
         RealArray elements;

         CoordinateConverter* cv = new CoordinateConverter();
         CoordinateSystem * cs = (CoordinateSystem*)(((Spacecraft*)((*map)[i]->object))->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
         SolarSystem* ss = cs->GetSolarSystem();
         SpacePoint* body = cs->GetOrigin();
         if (body->IsOfType(Gmat::GROUND_STATION))
         {
            // for case CS is GDSTropocentric, the origin is ground station GDS
            std::string cbName = ((GroundstationInterface*)body)->GetStringParameter("CentralBody");
            body = ss->GetBody(cbName);
         }
         CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem(body->GetName() + "mj2kCS", "MJ2000Eq", body, NULL, NULL, cs->GetJ2000Body(), ss);

         // 1. Get spacecraft Cartesian state
         Spacecraft* sc = (Spacecraft*)((*map)[i]->object);
         std::string stateType = sc->GetStringParameter("DisplayStateType");
         if (stateType == "Cartesian")
         {

            // Convert state from internal coordinate system to MJ2000Eq Cartesian coordinate system
            Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);
            GmatTime epoch = state.GetEpochGT();
            Rvector6 outStateCart;
            cv->Convert(epoch, inState, cs, cState, mj2kCS);
         }
         else if (stateType == "Keplerian")
         {
            Rvector6 kState;
            kState.Set(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);

            Real mu = body->GetRealParameter("Mu");
            cState = StateConversionUtil::KeplerianToCartesian(mu, kState, "TA");
         }

         // 2. Calculation
         SpacePoint* J2000body = cs->GetJ2000Body();
         CoordinateSystem* bodyFixedCs = CoordinateSystem::CreateLocalCoordinateSystem("bodyfixedCS", "BodyFixed",
            body, NULL, NULL, J2000body, solarSystem);
         Real epoch  = ((Spacecraft*)((*map)[i]->object))->GetEpoch();

         Real originMu = body->GetRealParameter(body->GetParameterID("Mu"));
         Real originRadius = body->GetRealParameter(body->GetParameterID("EquatorialRadius"));
         Real originFlattening = body->GetRealParameter(body->GetParameterID("Flattening"));

         Rvector6 sphStateAZFPA = StateConversionUtil::Convert(cState, "Cartesian", "SphericalAZFPA",
            originMu, originFlattening, originRadius);
         Rvector6 sphStateRADEC = StateConversionUtil::Convert(cState, "Cartesian", "SphericalRADEC",
            originMu, originFlattening, originRadius);

         Rvector6 kepState = StateConversionUtil::CartesianToKeplerian(originMu, cState, "TA");  // Keplerian state with anomaly in form of TA
         
         Real ea = 0.0;
         Real ha = 0.0;
         Real ma = 0.0;
         bool isEccentric = false;
         bool isHyperbolic = false;
         if (kepState[1] < (1.0 - GmatOrbitConstants::KEP_ECC_TOL))
         {
            ea = StateConversionUtil::TrueToEccentricAnomaly(kepState[5] * GmatMathConstants::RAD_PER_DEG,
               kepState[1], true) * GmatMathConstants::DEG_PER_RAD;
            isEccentric = true;
         }
         else if (kepState[1] > (1.0 + GmatOrbitConstants::KEP_TOL)) // *** or KEP_ECC_TOL or need new tolerance for this?  1.0e-10
         {
            ha = StateConversionUtil::TrueToHyperbolicAnomaly(kepState[5] * GmatMathConstants::RAD_PER_DEG,
               kepState[1], true) * GmatMathConstants::DEG_PER_RAD;
            isHyperbolic = true;
         }
         ma = StateConversionUtil::TrueToMeanAnomaly(kepState[5] * GmatMathConstants::RAD_PER_DEG,
            kepState[1], !isHyperbolic) * GmatMathConstants::DEG_PER_RAD;
         
         
         Rvector6 perKepState = kepState;
         Rvector6 apoKepState = kepState;
         perKepState[5] = 0.0;
         if(isEccentric)
            apoKepState[5] = 180.0;
         Rvector6 perCartState = StateConversionUtil::KeplerianToCartesian(originMu, perKepState, "TA");
         Rvector6 apoCartState;
         if(isEccentric)
            apoCartState = StateConversionUtil::KeplerianToCartesian(originMu, apoKepState, "TA");

         Rvector6 perCartStateBodyFix, apoCartStateBodyFix;
         cv->Convert(A1Mjd(epoch), perCartState, mj2kCS, perCartStateBodyFix, bodyFixedCs);
         if (isEccentric)
            cv->Convert(A1Mjd(epoch), apoCartState, mj2kCS, apoCartStateBodyFix, bodyFixedCs);

         Rvector3 perLLA = BodyFixedStateConverterUtil::CartesianToSphericalEllipsoid(perCartState.GetR(), originFlattening, originRadius);
         Rvector3 apoLLA = BodyFixedStateConverterUtil::CartesianToSphericalEllipsoid(apoCartState.GetR(), originFlattening, originRadius);

         


         // 3. Calculate ancillary elements' vector
         elementValue = sphStateAZFPA[1];
         elements.push_back(elementValue);           // Right Ascension                unit: deg

         elementValue = sphStateAZFPA[2];
         elements.push_back(elementValue);           // Declination                    unit: deg

         elementValue = sphStateAZFPA[5];
         elements.push_back(elementValue);           // Vertical Flight Path Angle     unit: deg

         elementValue = sphStateAZFPA[4];
         elements.push_back(elementValue);           // Azimuth Angle                  unit: deg

         elementValue = sphStateAZFPA[0];
         elements.push_back(elementValue);           // Magnitude of Radius Vector     unit: km

         elementValue = sphStateAZFPA[3];
         elements.push_back(elementValue);           // Magnitude of Velocity          unit: km/s

         elementValue = ea;
         elements.push_back(elementValue);           // Eccentric Anomaly              unit: deg

         elementValue = kepState[5];
         elements.push_back(elementValue);           // True Anomaly                   unit: deg
         
         elementValue = GmatCalcUtil::CalculateKeplerianData("OrbitPeriod", cState, originMu)/GmatTimeConstants::SECS_PER_MINUTE;
         elements.push_back(elementValue);           // Period                         unit: min

         elementValue = GmatCalcUtil::CalculatePlanetData("Altitude", perCartStateBodyFix, originRadius, originFlattening, 0.0);
         elements.push_back(elementValue);           // Perifocal Oblate Height        unit: km

         elementValue = GmatCalcUtil::CalculateKeplerianData("RadPeriapsis", cState, originMu);
         elements.push_back(elementValue);           // Perifocal Radius               unit: km

         if (isEccentric)
            elementValue = GmatCalcUtil::CalculatePlanetData("Altitude", apoCartStateBodyFix, originRadius, originFlattening, 0.0);
         else
            elementValue = 0.0;
         elements.push_back(elementValue);           // Apofocal Oblate Height         unit: km

         if (isEccentric)
            elementValue = GmatCalcUtil::CalculateKeplerianData("RadApoapsis", cState, originMu);
         else
            elementValue = 0.0;
         elements.push_back(elementValue);           // Apofocal Radius                unit: km

         if (isEccentric)
         {
            elementValue = GmatCalcUtil::CalculateKeplerianData("MeanMotion", cState, originMu);   // unit: rad/s
            elementValue = elementValue * GmatMathConstants::DEG_PER_RAD*GmatTimeConstants::SECS_PER_DAY;  // unit: degree/day
         }
         else
            elementValue = 0.0;
         elements.push_back(elementValue);           // Mean Motion                    unit: deg/day

         if (isEccentric)
            elementValue = GmatCalcUtil::CalculateKeplerianData("VelApoapsis", cState, originMu);
         else
            elementValue = 0.0;
         elements.push_back(elementValue);           // Velocity at Apoapsis           unit: km/s

         elementValue = GmatCalcUtil::CalculateKeplerianData("VelPeriapsis", cState, originMu);
         elements.push_back(elementValue);           // Velocity at Periapsis          unit: km/s
         
         // To calculate planet data, It needs to have state in body fixed coordinate system
         // Convert cState to bodyFixed coordinate system
         Rvector6 cStateBodyFix;
         cv->Convert(A1Mjd(epoch), cState, mj2kCS, cStateBodyFix, bodyFixedCs);

         elementValue = GmatCalcUtil::CalculatePlanetData("Latitude", cStateBodyFix, originRadius, 0.0, 0.0);    // set flattenning = 0
         elements.push_back(elementValue);           // Planetocentric Latitude        unit: deg

         elementValue = GmatCalcUtil::CalculatePlanetData("Latitude", cStateBodyFix, originRadius, originFlattening, 0.0);
         elements.push_back(elementValue);           // Planetodetic Latitude          unit: deg

         elementValue = GmatCalcUtil::CalculatePlanetData("Longitude", cStateBodyFix, originRadius, originFlattening, 0.0);
         elements.push_back(elementValue);           // Longitude                      unit: deg
         
         elementValue = GmatCalcUtil::CalculatePlanetData("Altitude", cStateBodyFix, originRadius, originFlattening, 0.0);
         elements.push_back(elementValue);           // Height                         unit: km

         elementValue = GmatCalcUtil::CalculateKeplerianData("C3Energy", cState, originMu);
         elements.push_back(elementValue);           // C3 Energy                      unit: km2/s2

         // 4. Set value to state map
         stateMap[(*map)[i]->object] = elements;

         // Clean up memory
         delete mj2kCS;
         delete bodyFixedCs;
         delete cv;
         cv = NULL; bodyFixedCs = NULL;

         // 5. Skip to the next spacecraft
         i = i + 5;
      }
   }
   
   return stateMap;
}


//------------------------------------------------------------------------------
// void BuildSatPropMap()
//------------------------------------------------------------------------------
/**
 * Sets up the mapping between spacecraft and their propagators
 */
//------------------------------------------------------------------------------
void Estimator::BuildSatPropMap()
{
   satPropMap.clear();
   
   if (propagatorSatMap.size() == 1)
   {
      if (propagators.size() == 1)
      {
         if (propagatorSatMap[propagators[0]->GetName()].size() == 0)
         {

            ObjectArray satArray;
            esm.GetStateObjects(satArray, Gmat::SPACECRAFT);
            for (int i = 0; i < satArray.size(); i++)
               propagatorSatMap[propagators[0]->GetName()].push_back(satArray[i]->GetName());
         }
      }
   }

   for (std::map<std::string, StringArray>::iterator i = propagatorSatMap.begin();
         i != propagatorSatMap.end(); ++i)
   {
      // Find the propagator
      PropSetup *theProp = nullptr;
      std::string propName = i->first;
      for (UnsignedInt j = 0; j < propagators.size(); ++j)
      {
         if (propagators[j]->GetName() == propName)
         {
            theProp = propagators[j];
            break;
         }
      }
      if (!theProp)
         throw EstimatorException("The propagator " + propName
               + " set on the Estimator " + instanceName + " was not found.");

      StringArray scs = i->second;
      for (UnsignedInt j = 0; j < scs.size(); ++j)
      {
         if (satPropMap.find(scs[j]) != satPropMap.end())
            throw EstimatorException("The spacecraft " + scs[j]
                  + " is set to propagate with more than one propagator in "
                    "the Estimator " + instanceName);
         satPropMap[scs[j]] = theProp;
      }
   }
   needsSatPropMap = false;
}


//------------------------------------------------------------------------------
// bool Estimator::DoesSatUseEphemerisPropagator(std::string satName)
//------------------------------------------------------------------------------
/**
 * Returns true if a given sat is attached to an ephemeris propagator in this estimator
 * Returns false if not.
 *
 * @param satName: string containing the name of the sat to be checked
 *
 * @return true if the sat is attached to an ephem prop, false if not
 */
 //------------------------------------------------------------------------------
bool Estimator::DoesSatUseEphemerisPropagator(std::string satName)
{
   //if only one propagator, just check that
   if (propagators.size() == 1)
      if (propagators[0]->GetPropagator()->IsOfType("EphemerisPropagator"))
         return true;
      else
         return false;

   if (needsSatPropMap)
      BuildSatPropMap();

   //if key is not in map, return false
   if (satPropMap.find(satName) == satPropMap.end())
      return false;

   if (satPropMap.at(satName)->GetPropagator()->IsOfType("EphemerisPropagator"))
      return true;

   return false;
}


//------------------------------------------------------------------------------
// void WriteStringArrayValue(Gmat::WriteMode mode, std::string &prefix,
//       Integer id, bool writeQuotes, std::stringstream &stream)
//------------------------------------------------------------------------------
/**
 * Writes out parameters of StringArrayType or ObjectArrayType in the GMAT script syntax.
 *
 * Overridden here to manage multiple propagators on Estimators.
 *
 * @param mode Output mode for the parameter.
 * @param prefix Prefix for the parameter (e.g. "GMAT ")
 * @param id   ID for the parameter that gets written.
 * @param writeQuotes Toggle indicating if quote marks should be written.
 * @param stream Output stream for the data.
 */
//------------------------------------------------------------------------------
void Estimator::WriteStringArrayValue(Gmat::WriteMode mode, std::string &prefix,
      Integer id, bool writeQuotes, std::stringstream &stream)
{
   if (id == PROPAGATOR)
   {
      std::stringstream propstr;

      if (mode != Gmat::OBJECT_EXPORT)
         propstr << GetAttributeCommentLine(id);

      if (propagatorNames.size() > 0)
      {
         std::string thePrefix = prefix + GetParameterText(id);

         // Set the default propagator
         if (writeQuotes)
            propstr << thePrefix << " = '" << propagatorNames[0] << "'\n";
         else
            propstr << thePrefix << " = " << propagatorNames[0] << "\n";

         for (UnsignedInt i = 0; i < propagatorNames.size(); ++i)
         {
            if (propagatorSatMap.find(propagatorNames[i]) != propagatorSatMap.end())
            {
               std::string mapping;
               StringArray sats = propagatorSatMap[propagatorNames[i]];
               if (sats.size() > 0)
               {
                  mapping = thePrefix + " = {";

                  if (writeQuotes)
                     mapping += "'" + propagatorNames[i] + "', ";
                  else
                     mapping += propagatorNames[i] + ", ";

                  for (UnsignedInt j = 0; j < sats.size(); ++j)
                  {
                     if (j > 0)
                        mapping += ", ";

                     if (writeQuotes)
                        mapping += "'";
                     mapping += sats[j];
                     if (writeQuotes)
                        mapping += "'";
                  }
                  propstr << mapping << "};\n";
               }
            }
         }

         stream << propstr.str();

         if ((propstr.str() != "") && ((mode == Gmat::SCRIPTING) ||
                                 (mode == Gmat::OWNED_OBJECT) ||
                                 (mode == Gmat::SHOW_SCRIPT)))
         {
            stream << GetInlineAttributeComment(id);
         }

         #ifdef DEBUG_SCRIPTOUTPUT
            MessageInterface::ShowMessage("Overriding setting for ID %d (%s):\n",
                  id, GetParameterText(id).c_str());
            MessageInterface::ShowMessage("%s\n", propstr.str().c_str());
         #endif
      }
      return;
   }

   Solver::WriteStringArrayValue(mode, prefix, id, writeQuotes, stream);
}


//------------------------------------------------------------------------------
// std::string GetMeasurementResidualTitles()
//------------------------------------------------------------------------------
/**
 * Returns the Mesurement residual titles to be used in report files.
 */
 //------------------------------------------------------------------------------
std::string Estimator::GetMeasurementResidualTitles()
{
   if(textFileMode == "Normal")
      return "RecNum  UTCGregorian-Epoch        Obs-Type            " + GmatStringUtil::GetAlignmentString("Participants", pcolumnLen) + " Edit          Observed (O)          Computed (C)       Residual (O-C)  Elev.\n";
   else
      return "RecNum  UTCGregorian-Epoch        TAIModJulian-Epoch Obs Type            Units  " + GmatStringUtil::GetAlignmentString("Participants", pcolumnLen) + " Edit               Obs (O)    Obs-Correction (O)               Cal (C)       Residual (O-C)            Weight (W)             W*(O-C)^2         sqrt(W)*|O-C|    Elevation-Angle    Partial-Derivatives";

}


//------------------------------------------------------------------------------
// bool SetPropagatorSatMap(std::string propName, std::string satName)
//------------------------------------------------------------------------------
/**
 * Adds satName to propagatorSatMap[propName]
 *
 * Overridden here as the default set function is not set up for adding 
 * individual satNames to a given propagator.
 *
 */
 //------------------------------------------------------------------------------
bool Estimator::SetPropagatorSatMap(std::string propName, std::string satName)
{
	bool retval = false;

	if (propagatorSatMap.find(propName) != propagatorSatMap.end())
	{
		StringArray theSats = propagatorSatMap[propName];
		if (find(theSats.begin(), theSats.end(), satName) == theSats.end())
		{
         if (find(propagatorSatMap[propName].begin(), propagatorSatMap[propName].end(), satName) == propagatorSatMap[propName].end())
         {
            propagatorSatMap[propName].push_back(satName);
            retval = true;
         }
		}
	}

	return retval;
}


//------------------------------------------------------------------------------
// void Estimator::UpdateCov()
//------------------------------------------------------------------------------
/**
 * Updates covariance. Does nothing here and is overridden elsewhere
 *
 */
 //------------------------------------------------------------------------------
void Estimator::UpdateCov()
{
   //Do nothing
}


std::map<std::string, StringArray> Estimator::GetPropSatMap()
{
   return propagatorSatMap;
}

std::map<std::string, PropSetup*> Estimator::GetSatPropMap()
{
   return satPropMap;
}
