//------------------------------------------------------------------------------
//                           GPSPointMeasureModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Tuan Dang Nguyen, GSFC/NASA.
// Created: Oct 3, 2016
/**
 * The GPS Point Measurement Model class for estimation
 */
//------------------------------------------------------------------------------

#include "GPSPointMeasureModel.hpp"
#include "SignalBase.hpp"
#include "ProgressReporter.hpp"
#include "MeasurementException.hpp"

#include "PhysicalSignal.hpp"
//#include "SinglePointSignal.hpp"
#include "ObservationData.hpp"

#include "PropSetup.hpp"
#include "ODEModel.hpp"
#include "Propagator.hpp"
#include "MessageInterface.hpp"
#include "TextParser.hpp"
#include "GroundstationInterface.hpp"
#include "Transmitter.hpp"
#include "StringUtil.hpp"

#include <sstream>

//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_LIGHTTIME
//#define DEBUG_TIMING
//#define DEBUG_FEASIBILITY
//#define DEBUG_EXECUTION
//#define DEBUG_DERIVATIVE
//#define DEBUG_CALCULATE_MEASUREMENT
//#define DEBUG_OFFSET

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

//const std::string
//GPSPointMeasureModel::PARAMETER_TEXT[GPSPointMeasurementParamCount - MeasurementParamCount] =
//{
//};
//
//
//const Gmat::ParameterType
//GPSPointMeasureModel::PARAMETER_TYPE[GPSPointMeasurementParamCount - MeasurementParamCount] =
//{
//};



//------------------------------------------------------------------------------
// GPSPointMeasureModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The scripted name (when needed) for the measurement model
 */
//------------------------------------------------------------------------------
GPSPointMeasureModel::GPSPointMeasureModel(const std::string &name) :
   MeasureModel (name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GPSPointMeasureModel default constructor  <%p>\n", this);
#endif

}


//------------------------------------------------------------------------------
// ~GPSPointMeasureModel()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GPSPointMeasureModel::~GPSPointMeasureModel()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GPSPointMeasureModel default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// GPSPointMeasureModel(const GPSPointMeasureModel& mm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param mm The model used as a template for this one
 */
//------------------------------------------------------------------------------
GPSPointMeasureModel::GPSPointMeasureModel(const GPSPointMeasureModel& mm) :
   MeasureModel          (mm)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GPSPointMeasureModel copy constructor  from <%p> to <%p>\n", &mm, this);
#endif
}


//------------------------------------------------------------------------------
// GPSPointMeasureModel& operator=(const GPSPointMeasureModel& mm)
//------------------------------------------------------------------------------
/**
 * Assignmant operator
 *
 * @param mm The measurement model used to configure this one
 *
 * @return This model, set to match mm
 */
//------------------------------------------------------------------------------
GPSPointMeasureModel& GPSPointMeasureModel::operator=(const GPSPointMeasureModel& mm)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GPSPointMeasureModel operator =   set <%p> = <%p>\n", this, &mm);
#endif

   if (this != &mm)
   {
      MeasureModel::operator=(mm);
   }
   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this model
 *
 * @return A clone of this model
 */
//------------------------------------------------------------------------------
GmatBase* GPSPointMeasureModel::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GPSPointMeasureModel::Clone()   clone this <%p>\n", this);
#endif

   return new GPSPointMeasureModel(*this);
}

////------------------------------------------------------------------------------
//// std::string GetParameterText(const Integer id) const
////------------------------------------------------------------------------------
///**
// * Returns the script name for the parameter
// *
// * @param id The id of the parameter
// *
// * @return The script name
// */
////------------------------------------------------------------------------------
//std::string GPSPointMeasureModel::GetParameterText(const Integer id) const
//{
//   if (id >= MeasurementParamCount && id < GPSPointMeasurementParamCount)
//      return PARAMETER_TEXT[id - MeasurementParamCount];
//   return MeasureModel::GetParameterText(id);
//}
//
//
////------------------------------------------------------------------------------
//// Integer GetParameterID(const std::string& str) const
////------------------------------------------------------------------------------
///**
// * Retrieves the ID for a scriptable parameter
// *
// * @param str The string used for the parameter
// *
// * @return The parameter ID
// */
////------------------------------------------------------------------------------
//Integer GPSPointMeasureModel::GetParameterID(const std::string& str) const
//{
//   for (Integer i = MeasurementParamCount; i < GPSPointMeasurementParamCount; i++)
//   {
//      if (str == PARAMETER_TEXT[i - MeasurementParamCount])
//         return i;
//   }
//   return MeasureModel::GetParameterID(str);
//}
//
//
////------------------------------------------------------------------------------
//// Gmat::ParameterType GetParameterType(const Integer id) const
////------------------------------------------------------------------------------
///**
// * Retrieves the type for a specified parameter
// *
// * @param id The ID for the parameter
// *
// * @return The parameter's type
// */
////------------------------------------------------------------------------------
//Gmat::ParameterType GPSPointMeasureModel::GetParameterType(const Integer id) const
//{
//   if (id >= MeasurementParamCount && id < GPSPointMeasurementParamCount)
//      return PARAMETER_TYPE[id - MeasurementParamCount];
//
//   return MeasureModel::GetParameterType(id);
//}
//
//
////------------------------------------------------------------------------------
//// std::string GetParameterTypeString(const Integer id) const
////------------------------------------------------------------------------------
///**
// * Retrieves a text description for a parameter
// *
// * @param id The ID for the parameter
// *
// * @return The description string
// */
////------------------------------------------------------------------------------
//std::string GPSPointMeasureModel::GetParameterTypeString(const Integer id) const
//{
//   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
//}



//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the measurement model for use
 *
 * @return true if the initialization succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool GPSPointMeasureModel::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start MeasurementModel<%s,%p>::Initialize()\n", GetName().c_str(), this);
#endif
   bool retval = false;

   if (GmatBase::Initialize() == false)
      return retval;

   if (navLog)
   {
      logLevel = navLog->GetLogLevel("Measurement");
      #ifdef DEBUG_LOGGING
         MessageInterface::ShowMessage("Current log level for measurements "
                                       "is %d\n", logLevel);
      #endif
   }
   else
      logLevel = 32767;

   if (participantLists.size() == 0)
      throw MeasurementException("Error: Measurement has no participants.\n");

   // Clear stale pointers
   for (UnsignedInt i = 0; i < participants.size(); ++i)
   {
      delete participants[i];
   }
   participants.clear();
   for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
      delete signalPaths[i];
   signalPaths.clear();
   theData.clear();

   for (UnsignedInt i = 0; i < participantLists.size(); ++i)
   {
      participants.push_back(new ObjectArray);
      if (participantLists[i]->size() == 0)
         throw MeasurementException("Error: Measurement has no participants.\n");
   }

   // Put all participants in place for the model
   std::string theMissing = "";
   for (UnsignedInt i = 0; i < participantLists.size(); ++i)
   {
      for (UnsignedInt j = 0; j < participantLists[i]->size(); ++j)
      {
         std::string pname = participantLists[i]->at(j);
         GmatBase *obj = NULL;
         for (UnsignedInt k = 0; k < candidates.size(); ++k)
            if (candidates[k]->GetName() == pname)
               obj = candidates[k];
         if (obj != NULL)
            participants[i]->push_back(obj);
         else
         {
            if (theMissing != "")
               theMissing += ", ";
            theMissing += pname;
         }
      }
   }

   if (theMissing != "")
      throw MeasurementException("Cannot configure the measurement "
            "model " + instanceName + "; the following participants are "
            "not in the object: " + theMissing);

   retval = InitializePointModel();

   if (navLog)
   {
      std::stringstream data;
      data.precision(16);

      if (logLevel <= 1)
      {
         data << "   " << instanceName
            << (retval ? " initialized\n" : " failed to initialize\n");
      }
      if (logLevel == 0)
      {
         data << "   For measurement model " << instanceName
            << ", constructed the signal path(s):" << "\n";
         for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
            data << "      "
            << (signalPaths[i]->GetPathDescription(true))
            << "\n";
      }
      navLog->WriteData(data.str());
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("End MeasurementModel<%s,%p>::Initialize()\n", GetName().c_str(), this);
#endif

   return retval;
}


bool GPSPointMeasureModel::InitializePointModel()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Start MeasurementModel<%s,%p>::InitializePointModel()\n", GetName().c_str(), this);
   #endif

   bool retval = false;

   // Build the signals
   for (UnsignedInt i = 0; i < participantLists.size(); ++i)
   {
      SignalBase *head = NULL;
      if (participantLists[i]->size() > 1)
         throw MeasurementException("Error: GPS measurement has more than one participants\n");
      
      // 1. Create a signal leg
      SignalBase *sb = NULL;
      if (isPhysical)
         sb = new PhysicalSignal("");
      else
      {
         // The signals are single point
         //sb = new SinglePointSignal("");
         throw MeasurementException("Single point signals are not "
               "yet implemented");
      }
      
      // 2. Set solar system, lightime correction, names of transmit and receive participants,
      //    participant objects, and their propagators 
      if (sb)
      {
         if (navLog)
            sb->SetProgressReporter(navLog);
         sb->SetSolarSystem(solarsys);
         sb->UsesLighttime(withLighttime);
         
         // Set name for transmit participant and receive participant 
         sb->SetTransmitParticipantName("");
         if (sb->SetReceiveParticipantName(
            participantLists[i]->at(0)) == false)
         {
            throw MeasurementException("Failed to set the name of "
               "the receive participant to " +
               participantLists[i]->at(0) +
               " on measurement model " + instanceName);
         }
         
         // Set receive participant and its propagator in SignalBase object
         GmatBase* obj = participants[i]->at(0);
         if (sb->SetRefObject(obj, obj->GetType(), obj->GetName()) == false)
            throw MeasurementException("Failed to set the receive "
                  "participant\n");
         else if (obj->IsOfType(Gmat::SPACEOBJECT) && propMap[(SpacePoint*)obj])
            sb->SetPropagator(propMap[(SpacePoint*)obj], obj);
         
         if (!sb->Initialize())
         {
            throw MeasurementException("Signal initialization "
                  "failed in measurement model " + instanceName);
         }
         
         signalPaths.push_back(sb);
         head = sb;
         //theData.push_back(&(sb->GetSignalData()));
         theData.push_back(sb->GetSignalDataObject());
      }
   }
   
   // Add new types of measurement corrections to signal path
   for(UnsignedInt i = 0; i < correctionTypeList.size(); ++i)
      AddCorrection(correctionModelList[i], correctionTypeList[i]);
   
   // For each ground station, clone all ErrorModel objects for each signal path
   for (UnsignedInt i = 0; i < participants.size(); ++i)
   {
      if (participants[i]->size() > 1)
      {
         GmatBase* firstPart = participants[i]->at(0);
         GmatBase* lastPart = participants[i]->at(participants[i]->size() - 1);
         
         if ((firstPart->IsOfType(Gmat::GROUND_STATION)) &&
            (lastPart->IsOfType(Gmat::GROUND_STATION) == false))
         {
            // clone all ErrorModel objects belonging to groundstation firstPart
            std::string spacecraftName = "";
            std::string spacecraftId = "";
            GmatBase* obj = participants[i]->at(1);
            if (obj->IsOfType(Gmat::SPACECRAFT))
            {
               spacecraftName = obj->GetName();
               spacecraftId = obj->GetStringParameter("Id");
            }
            else
               throw MeasurementException("Error: It has 2 ground stations (" +
               firstPart->GetName() + ", " + obj->GetName() +
               ") next to each other in signal path.\n");
            
            ((GroundstationInterface*)firstPart)->CreateErrorModelForSignalPath(spacecraftName, spacecraftId);
         }
         else
         {
            // clone all ErrorModel objects belonging to groundstation firstPart
            std::string spacecraftName = "";
            std::string spacecraftId = "";
            GmatBase* obj = participants[i]->at(participants[i]->size() - 2);
            if (obj->IsOfType(Gmat::SPACECRAFT))
            {
               spacecraftName = obj->GetName();
               spacecraftId = obj->GetStringParameter("Id");
            }
            else
               throw MeasurementException("Error: It has 2 ground stations (" +
               obj->GetName() + ", " +
               lastPart->GetName() + ") next to each other in signal path.\n");
            
            ((GroundstationInterface*)lastPart)->CreateErrorModelForSignalPath(spacecraftName, spacecraftId);
         }
      }
   }
   
   retval = true;

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("End MeasurementModel<%s,%p>::InitializePointModel()\n", GetName().c_str(), this);
   #endif

   return retval;
}






//@todo: Move the calling parameters here into the adapters
//------------------------------------------------------------------------------
// bool CalculateMeasurement(bool withEvents, ObservationData* forObservation,
//       std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------
/**
 * Fires all of the Signal objects to generate the raw measurement data
 *
 * @param withEvents            Flag used to indicate if a light time solution should be
 *                              computed (not used)
 * @param forObservation        An observation supplying data needed for the
 *                              calculation (not used)
 * @param rampTB                A ramp table for the data (not used)
 * @param atTimeOffset          Time offset, in seconds, from the base epoch (used for
 *                              differenced measurements)
 * @param withMediaCorrection   true for adding media correction to measurement,
 *                              false otherwise
 *
 * @return true if the the calculation succeeded, false if not
 */
//------------------------------------------------------------------------------
bool GPSPointMeasureModel::CalculateMeasurement(bool withEvents, bool withMediaCorrection,
   ObservationData* forObservation, std::vector<RampTableData>* rampTB, bool forSImulation, 
   Real atTimeOffset, Integer forStrand)
{
#ifdef DEBUG_EXECUTION
   MessageInterface::ShowMessage(" Enter GPSPointMeasureModel::CalculateMeasurement(%s, <%p>, <%p>)\n", (withEvents?"true":"false"), forObservation, rampTB); 
#endif

#ifdef DEBUG_TIMING
   MessageInterface::ShowMessage("Calculating Signal Data in GPSPointMeasureModel\n");
#endif

   bool retval = false;
   feasible = true;

   // 1. Prepare the propagators
#ifdef DEBUG_TIMING
   MessageInterface::ShowMessage("1. Prepare the propagators\n");
#endif
   //if (propsNeedInit)
   //   PrepareToPropagate();
   PrepareToPropagate();

   // 2. Find the measurement epoch needed for the computation
#ifdef DEBUG_TIMING
   MessageInterface::ShowMessage("2. Find the measurement epoch needed for the computation\n");
#endif

   GmatTime forEpoch;
   if (forObservation)
   {
      forEpoch = forObservation->epochGT;
   }
   else // Grab epoch from the first SpaceObject in the participant data
   {
      for (UnsignedInt i = 0; i < candidates.size(); ++i)
      {
         if (candidates[i]->IsOfType(Gmat::SPACEOBJECT))
         {
            forEpoch = ((SpaceObject*)candidates[i])->GetEpochGT();
            break;
         }
      }
   }

   // 3. Synchronize the propagators to the measurement epoch by propagating each
   // spacecraft that is off epoch to that epoch
#ifdef DEBUG_TIMING
   MessageInterface::ShowMessage("3. Synchronizing in GPSPointMeasureModel at time = %.12lf\n", forEpoch.GetMjd());
#endif

   for (std::map<SpacePoint*, PropSetup*>::iterator i = propMap.begin();
      i != propMap.end(); ++i)
   {
      if (i->first->IsOfType(Gmat::SPACEOBJECT) && (i->second != NULL))
      {
         GmatTime satTime = ((SpaceObject*)i->first)->GetEpochGT();
         Real dt = (forEpoch - satTime).GetTimeInSec();

#ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("forEpoch: %.12lf, satTime = %.12lf, "
            "dt = %le\n", forEpoch.GetMjd(), satTime, dt);
#endif

         // Make sure the propagators are set to the spacecraft data
         Propagator *prop = i->second->GetPropagator();
         prop->UpdateFromSpaceObject();

         if (dt != 0.0)
         {
            retval = prop->Step(dt);
         }
      }
   }

   // 4.Calculate the measurement data ("C" value data) for all signal paths
#ifdef DEBUG_CALCULATE_MEASUREMENT
   MessageInterface::ShowMessage("*************************************************************\n");

   MessageInterface::ShowMessage("*          Calculate Measurement Data %s at Epoch (%.12lf) \n", (withEvents ? "with Event" : "w/o Event"), forEpoch.GetMjd());

   MessageInterface::ShowMessage("*************************************************************\n");
#endif

   SignalBase *leg;
   for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
   {
      // For a measurement that need to control strand by strand with time
      // offsets (e.g. for differencing
      if (forStrand != -1)
         i = forStrand;

#ifdef DEBUG_CALCULATE_MEASUREMENT
      MessageInterface::ShowMessage("*** Calculate Measurement Data for Path %d of %d:  ", i, signalPaths.size());
      SignalBase* s = signalPaths[i];
      SignalData* sdata = &(s->GetSignalData());
      MessageInterface::ShowMessage("<NULL -> %s>\n\n", sdata->rNode->GetName().c_str());
#endif

      // 4.1. Initialize all signal legs in this path:
#ifdef DEBUG_TIMING
      MessageInterface::ShowMessage("4.1. Initialize all signal legs in signal path %d:\n", i);
#endif
      leg = signalPaths[i];
      leg->InitializeSignal(epochIsAtEnd);

      // 4.2. Sync receiver epoch to forEpoch, and Spacecraft state
      // data to the state known in the PropSetup for the starting Signal
#ifdef DEBUG_TIMING
      MessageInterface::ShowMessage("4.2. Propagate spacecrafts to forEpoch.\n");
#endif
      SignalData *sdObj = leg->GetSignalDataObject();
      sdObj->tPrecTime = sdObj->rPrecTime = forEpoch;
      sdObj->tLoc = Rvector3(0.0,0.0,0.0);
      sdObj->tVel = Rvector3(0.0, 0.0, 0.0);

      if (sdObj->rNode->IsOfType(Gmat::SPACECRAFT))
      {
         // this spacecraft's state presents in MJ2000Eq with origin at ForceModel.CentralBody

         if (propMap[sdObj->rNode] == NULL)
            throw MeasurementException("GPSPointMeasureModel::CalculateMeasurement(): "
               "The propagator for " + sdObj->rNode->GetName() + " is not defined");

         const Real* propState =
            propMap[sdObj->rNode]->GetPropagator()->AccessOutState();
         Rvector6 state(propState);

         // This step is used to convert spacecraft's state to Spacecraft.CoordinateSystem                                                                          // fix bug GMT-5364
         SpacePoint* spacecraftOrigin = ((Spacecraft*)(sdObj->rNode))->GetOrigin();                 // the origin of the receive spacecraft's cooridinate system    // fix bug GMT-5364

         SpacePoint* forcemodelOrigin;
         if (propMap[sdObj->rNode]->GetPropagator()->UsesODEModel())
            forcemodelOrigin = propMap[sdObj->rNode]->GetODEModel()->GetForceOrigin();
         else
            forcemodelOrigin = propMap[sdObj->rNode]->GetPropagator()->GetPropOrigin();    
         state = state + (forcemodelOrigin->GetMJ2000PrecState(sdObj->rPrecTime) - spacecraftOrigin->GetMJ2000PrecState(sdObj->rPrecTime));                         // fix bug GMT-5364
         sdObj->rLoc = state.GetR();
         sdObj->rVel = state.GetV();

         // receive participant STM at measurement type tm
         Integer stmRowCount = sdObj->rNode->GetIntegerParameter("FullSTMRowCount");
         // Set size for rSTMtm
         if ((sdObj->rSTMtm.GetNumRows() != stmRowCount) || (sdObj->rSTMtm.GetNumColumns() != stmRowCount))
            sdObj->rSTMtm.ChangeSize(stmRowCount, stmRowCount, true);

         // Get start index of STM
         const std::vector<ListItem*>* stateMap = propMap[sdObj->rNode]->GetPropStateManager()->GetStateMap();
         UnsignedInt stmStartIndex = -1;
         for (UnsignedInt index = 0; index < stateMap->size(); ++index)
         {
            if (((*stateMap)[index]->object == sdObj->rNode) && ((*stateMap)[index]->elementName == "STM"))
            {
               stmStartIndex = index;
               break;
            }
         }

         // Set value for eSTMtm
         for (UnsignedInt ii = 0; ii < stmRowCount; ++ii)
            for (UnsignedInt jj = 0; jj < stmRowCount; ++jj)
               sdObj->rSTMtm(ii, jj) = propState[stmStartIndex + ii*stmRowCount + jj];

         // receive participant STM at receive time t2
         if ((sdObj->rSTM.GetNumRows() != sdObj->rSTMtm.GetNumRows()) || (sdObj->rSTM.GetNumColumns() != sdObj->rSTMtm.GetNumColumns()))
            sdObj->rSTM.ChangeSize(sdObj->rSTMtm.GetNumRows(), sdObj->rSTMtm.GetNumColumns(), true);
         sdObj->rSTM = sdObj->rSTMtm;
      }

      // 4.3. Compute C-value:
      sdObj->feasibility = true;
      sdObj->feasibilityReason = "N";
      sdObj->feasibilityValue = 0.0;
      
      #ifdef DEBUG_TIMING
            MessageInterface::ShowMessage("4.3. Compute C-value:\n");
            MessageInterface::ShowMessage("4.3.1 Calculate spacecraft's position and velocity signal path %d:\n", i);
            MessageInterface::ShowMessage("   spacecraft position : [%.12lf,  %.12lf,  %.12lf] km\n", sdObj->rLoc(0), sdObj->rLoc(1), sdObj->rLoc(2));
            MessageInterface::ShowMessage("   spacecraft velocity : [%.12lf,  %.12lf,  %.12lf] km/s\n", sdObj->rVel(0), sdObj->rVel(1), sdObj->rVel(2));
      #endif

      // 4.4. Specify feasibility:
#ifdef DEBUG_TIMING
      MessageInterface::ShowMessage("4.3. Specify feasibility for signal path %d:\n", i);
#endif
      feasible = true;

      if (forStrand != -1)
         break;
   }

   retval = true;

#ifdef DEBUG_CALCULATE_MEASUREMENT
   MessageInterface::ShowMessage("**** All paths in this measurement are feasible\n");
   MessageInterface::ShowMessage("**** Calculation for this measurement is completed ****\n\n");
#endif

#ifdef DEBUG_EXECUTION
   MessageInterface::ShowMessage(" Exit GPSPointMeasureModel::CalculateMeasurement(%s, <%p>, <%p>)\n", (withEvents ? "true" : "false"), forObservation, rampTB);
#endif

   return retval;
}


//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(GmatBase* obj,
//       Integer id, Integer forStrand)
//------------------------------------------------------------------------------
/**
 * Computes the measurement derivative
 *
 * @param obj The "with respect to" object owning the "with respect to"
 *            parameter.
 * @param id The ID of the "with respect to" field
 * @param atTimeOffset Time offset, in seconds, from the base epoch (used for
 *                     differenced measurements)

 *
 * @return The vector of derivative data
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& GPSPointMeasureModel::CalculateMeasurementDerivatives(
   GmatBase* obj, Integer id, Integer forStrand)
{
#ifdef DEBUG_DERIVATIVE
   MessageInterface::ShowMessage("GPSPointMeasureModel::CalculateMeasurementDerivatives(%s, %d) called\n", obj->GetName().c_str(), id);
#endif

   //PointMeasurementDerivatives(theDataDerivatives, obj, id, forStrand);
   
   // Collect the data from the signals
   theDataDerivatives.clear();
   if (forStrand == -1)
   {
      for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
      {
         std::vector < RealArray > derivative;
         ModelPointSignalDerivative(obj, id, signalPaths[i], derivative);
         for (UnsignedInt row = 0; row < derivative.size(); ++row)
            theDataDerivatives.push_back(derivative[row]);
      }
   }
   else
   {
      std::vector < RealArray > derivative;
      ModelPointSignalDerivative(obj, id, signalPaths[forStrand], theDataDerivatives);
      for (UnsignedInt row = 0; row < derivative.size(); ++row)
         theDataDerivatives.push_back(derivative[row]);
   }

#ifdef DEBUG_DERIVATIVE
   MessageInterface::ShowMessage("Exit GPSPointMeasureModel::CalculateMeasurementDerivatives()\n");
#endif

   return theDataDerivatives;
}


//void GPSPointMeasureModel::PointMeasurementDerivatives(std::vector<RealArray>& derivative, GmatBase *obj, Integer id, Integer forStrand)
//{
//#ifdef DEBUG_DERIVATIVE
//   MessageInterface::ShowMessage("GPSPointMeasureModel::PointMeasurementDerivatives(%s, %d) called\n", obj->GetName().c_str(), id);
//#endif
//
//   // Collect the data from the signals
//   if (forStrand == -1)
//   {
//      for (UnsignedInt i = 0; i < signalPaths.size(); ++i)
//         ModelPointSignalDerivative(obj, id, signalPaths[i], derivative);
//   }
//   else
//   {
//      ModelPointSignalDerivative(obj, id, signalPaths[forStrand], derivative);
//   }
//
//#ifdef DEBUG_DERIVATIVE
//   MessageInterface::ShowMessage("Exit GPSPointMeasureModel::PointMeasurementDerivatives()\n");
//#endif
//}



//-----------------------------------------------------------------------
// void GetDerivativeWRTState(GmatBase *forObj, bool wrtR, bool wrtV, 
//         GmatTime measTime, std::vector<RealArray>& derivative)
//-----------------------------------------------------------------------
/**
* This function is used to calculate partial derivative of Position vector
* w.r.t. spacecraft state in ForceModel origin MJ2000Eq coordinate system 
* at measurement time tm. 
* 
*/
//-----------------------------------------------------------------------
void GPSPointMeasureModel::GetDerivativeWRTState(GmatBase *forObj, bool wrtR, bool wrtV, GmatTime measTime, std::vector<RealArray>& derivative)
{
   // 1. Create ForceModel coordinate system
   CelestialBody* earthBody = solarsys->GetBody("Earth");
   CoordinateSystem* forceModelCs = NULL;
   
   if (forObj->IsOfType(Gmat::SPACECRAFT))
   {
      SpacePoint* forcemodelOrigin;
      if (propMap[(SpacePoint*)forObj]->GetPropagator()->UsesODEModel())
         forcemodelOrigin = propMap[(SpacePoint*)forObj]->GetODEModel()->GetForceOrigin();
      else
         forcemodelOrigin = propMap[(SpacePoint*)forObj]->GetPropagator()->GetPropOrigin();
      forceModelCs = CoordinateSystem::CreateLocalCoordinateSystem("forceModelCs",
         "MJ2000Eq", forcemodelOrigin, NULL, NULL, earthBody, solarsys);
   }
   else
      throw MeasurementException("Error: Receive participant is not a spacecraft in point measurement.\n");
   
   // 2. Create ECF coordiante system
   CoordinateSystem* ecf = CoordinateSystem::CreateLocalCoordinateSystem("ecf", 
      "BodyFixed", earthBody, NULL, NULL, earthBody, solarsys);
   
   // 3. Get matrix transformation matrix from ECF to ForceModel coordinate system
   CoordinateConverter* cv = new CoordinateConverter();
   Rvector inState(6, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
   Rvector outState(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   cv->Convert(A1Mjd(measTime.GetMjd()), inState, forceModelCs, outState, ecf);
   Rmatrix33 R = cv->GetLastRotationMatrix();

   #ifdef DEBUG_DERIVATIVE
      Rvector3 x1(1.0, 2.0, 3.0);
      Rvector3 x2 = R*x1;
      Rvector3 x3(outState(0), outState(1), outState(2));
      MessageInterface::ShowMessage("R*xin = [%lf  %lf  %lf]   xout = [%lf  %lf  %lf]\n", x2[0], x2[1], x2[2], x3[0], x3[1], x3[2]);
   #endif


   // 4. Calculate derivative
   derivative.clear();

   if (wrtR)
   {
      if (wrtV)
      {
         for (Integer row = 0; row < 3; ++row)         // GPS X, Y, and Z
         {
            RealArray ar;
            for (Integer col = 0; col < 6; ++col)      // derivative w.r.t. x, y, z, vx, vy, and vz
            {
               if (col < 3)
                  ar.push_back(R(row, col));
               else
                  ar.push_back(0.0);
            }
            derivative.push_back(ar);
         }
      }
      else
      {
         Integer arSize = 3;
         for (Integer row = 0; row < 3; ++row)             // GPS X, Y, and Z
         {
            RealArray ar;
            for (Integer col = 0; col < arSize; ++col)     // derivative w.r.t. x, y, and z
               ar.push_back(R(row, col));
            derivative.push_back(ar);
         }
      }
   }
   else
   {
      for (Integer row = 0; row < 3; ++row)         // GPS X, Y, and Z
      {
         RealArray ar;
         for (Integer col = 0; col < 3; ++col)      // derivative w.r.t. vx, vy, and vz
            ar.push_back(0.0);
         derivative.push_back(ar);
      }
   }
   
   if (ecf)
   {
      delete ecf;
      ecf = NULL;
   }
   if (forceModelCs)
   {
      delete forceModelCs;
      forceModelCs = NULL;
   }
   if (cv)
   {
      delete cv;
      cv = NULL;
   }
   #ifdef DEBUG_DERIVATIVE
      MessageInterface::ShowMessage("derivative = [\n");
      for (UnsignedInt row = 0; row < derivative.size(); ++row)
      {
         for (UnsignedInt col = 0; col < derivative[row].size(); ++col)
            MessageInterface::ShowMessage("%lf,  ", derivative[row][col]);
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("]\n");
   #endif
}

void GPSPointMeasureModel::GetDerivativeWRTC(GmatBase *forObj, GmatTime measTime, std::vector<RealArray>& derivative)
{
}

void GPSPointMeasureModel::GetDerivativeWRTCr(GmatBase *forObj, GmatTime measTime, std::vector<RealArray>& derivative)
{
   for (UnsignedInt row = 0; row < 3; ++row)       // GPS X, Y, and Z
   {
      RealArray ar;
      ar.push_back(0.0);                           // derivative w.r.t. Cr
      derivative.push_back(ar);
   }
}

void GPSPointMeasureModel::GetDerivativeWRTCd(GmatBase *forObj, GmatTime measTime, std::vector<RealArray>& derivative)
{
   for (UnsignedInt row = 0; row < 3; ++row)       // GPS X, Y, and Z
   {
      RealArray ar;
      ar.push_back(0.0);                           // derivative w.r.t. Cr
      derivative.push_back(ar);
   }
}


void GPSPointMeasureModel::ModelPointSignalDerivative(GmatBase* obj, Integer forId, SignalBase* sb, std::vector<RealArray>& derivative)
{
#ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("GPSPointMeasureModel::ModelPointSignalDerivative(object name = %s, forId = %d)\n", obj->GetName().c_str(), forId);
#endif

   // Verify valid input
   if (obj == NULL)
      throw MeasurementException("Error: a NULL object inputs to GPSPointMeasureModel::ModelPointSignalDerivative() function\n");

   // Get parameter ID
   Integer parameterID = -1;
   if (forId > 250)
      parameterID = forId - obj->GetType() * 250;
   else
      parameterID = forId;
   std::string paramName = obj->GetParameterText(parameterID);
#ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("Solve-for parameter: %s.%s\n", obj->GetName().c_str(), obj->GetParameterText(parameterID).c_str());
#endif

   // Clear and reset derivative data
   derivative.clear();
   Integer size = obj->GetEstimationParameterSize(forId);
   RealArray oneRow;
   oneRow.assign(size, 0.0);
   for (Integer i = 0; i < 3; ++i)                // for X, Y, and Z
      derivative.push_back(oneRow);

   // Check to see if obj is a participant
   SignalData* data = sb->GetSignalDataObject();
   GmatBase *objPtr = NULL;
   if (data->tNode == obj)
      objPtr = data->tNode;
   else if (data->rNode == obj)
      objPtr = data->rNode;
   
   if (objPtr != NULL)                    // Derivative object is a participant (either GroundStation or Spacecraft):
   {
      if (paramName == "Position")
      {
         std::vector<RealArray> result;
         GetDerivativeWRTState(objPtr, true, false, sb->GetSignalDataObject()->rPrecTime, result);
         for (UnsignedInt row = 0; row < 3; ++row)             // for GPS X, Y, and Z
         {
            for (UnsignedInt col = 0; col < 3; ++col)          // derivative w.r.t. dx, dy, and dz
            {
               // It accumulates derivatives of all signal legs in its path
               derivative[row][col] += result[row][col];
            }
         }
      }
      else if (paramName == "Velocity")
      {
         std::vector<RealArray> result;
         GetDerivativeWRTState(objPtr, false, true, sb->GetSignalDataObject()->rPrecTime, result);
         for (UnsignedInt row = 0; row < 3; ++row)             // for GPS X, Y, and Z
         {
            for (UnsignedInt col = 0; col < 3; ++col)          // derivative w.r.t. dvx, dvy, and dvz
            {
               // It accumulates derivatives of all signal legs in its path
               derivative[row][col] += result[row][col];
            }
         }
      }
      else if (paramName == "CartesianX")
      {
         std::vector<RealArray> result;
         GetDerivativeWRTState(objPtr, true, true, sb->GetSignalDataObject()->rPrecTime, result);
         for (UnsignedInt row = 0; row < 3; ++row)             // for GPS X, Y, and Z
         {
            for (UnsignedInt col = 0; col < 6; ++col)          // derivative w.r.t. dx, dy, dz, dvx, dvy, and dvz
            {
               // It accumulates derivatives of all signal legs in its path
               derivative[row][col] += result[row][col];
            }
         }

         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("Derivatives GPS (X,Y,Z) w.r.t. state: [");
            for (UnsignedInt row = 0; row < 3; ++row)
            {
               for (UnsignedInt col = 0; col < 6; ++col)
                  MessageInterface::ShowMessage("%.12lf   ", result[row][col]);
               MessageInterface::ShowMessage("\n");
            }
            MessageInterface::ShowMessage("]\n");
         #endif
      }
      else if (paramName == "Cr_Epsilon")
      {
         std::vector<RealArray> result;
         GetDerivativeWRTCr(objPtr, sb->GetSignalDataObject()->rPrecTime, result);
         for (UnsignedInt row = 0; row < 3; ++row)            // for GPS X, Y, and Z
         {
            for (UnsignedInt col = 0; col < 1; ++col)         // derivative w.r.t. Cr
            {
               derivative[row][col] += result[row][col];
            }
         }
      }
      else if (paramName == "Cd_Epsilon")
      {
         std::vector<RealArray> result;
         GetDerivativeWRTCd(objPtr, sb->GetSignalDataObject()->rPrecTime, result);
         for (UnsignedInt row = 0; row < 3; ++row)            // for GPS X, Y, and Z
         {
            for (UnsignedInt col = 0; col < 1; ++col)         // derivative w.r.t. Cd
            {
               derivative[row][col] += result[row][col];
            }
         }
      }
   }
   else    // Derivative object is not a participant (neither a GroundStation nor a Spacecraft): such as ErrorModel object or GPSReceiver object
   {
      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   Deriv is w.r.t. something "
            "independent, so zero\n");
      #endif
      // Set 0 to all elements (number of elements is specified by size)
      for (Integer row = 0; row < 3; ++row)                   // for GPS X, Y, and Z
         for (UnsignedInt col = 0; col < size; ++col)         // derivative w.r.t. a given parameter 
            derivative[row][col] = 0.0;
   }

#ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("Exit GPSPointMeasureModel::ModelPointSignalDerivative()\n");
#endif
}


Real GPSPointMeasureModel::GetParamDerivative(GmatBase *forObj, std::string paramName, GmatBase *associateObj, SignalData *theData)
{
   //MessageInterface::ShowMessage("SignalBase::GetParamDerivative(forObj = <%s>, paramName = <%s>, associateObj = <%s>) start\n", forObj->GetName().c_str(), paramName.c_str(), (associateObj == NULL?"":associateObj->GetName().c_str()));
   // Get index for paramName
   StringArray stmElemNames;
   std::string paramFullName;
   if (associateObj)
   {
      // associateObj is a Spacecraft object (example: estSat.Plate1.DiffuseFraction, associateObj is estSat, forObj is Plate1, and paramName is DiffuseFraction)
      stmElemNames = associateObj->GetStringArrayParameter("StmElementNames");
      paramFullName = forObj->GetName() + "." + paramName;
   }
   else
   {
      // forObj is a Spacecraft object (example: estSat.Cr, forObj is estSat, paramName is Cr)
      stmElemNames = forObj->GetStringArrayParameter("StmElementNames");
      paramFullName = paramName;
   }

   UnsignedInt paramIndex = 0;
   bool found = false;
   //for (int i = 0; i < stmElemNames.size(); i++)
   //   MessageInterface::ShowMessage("for i = %d: stmElemNames[%d] = <%s>\n", i, i, stmElemNames[i].c_str());

   for (int i = 0; i < stmElemNames.size(); i++)
   {
      if (stmElemNames.at(i) == "CartesianState" || stmElemNames.at(i) == "KeplerianState")
         paramIndex += 0;  // vector that GetCDerivativeVector() returns does not include cartesian state
      else if (stmElemNames.at(i) == paramFullName)
      {
         found = true;
         break;
      }
      else
      {
         std::string name = stmElemNames.at(i);
         size_t pos = name.find_last_of('.');
         if (pos != name.npos)
            name = name.substr(pos + 1);
         paramIndex += forObj->GetEstimationParameterSize(forObj->GetParameterID(name));
      }
   }

   if (!found)
   {
      //MessageInterface::ShowMessage("Not found paramFullName <%s> in StmElementNames: deriv = 0.0\n", paramFullName.c_str());
      //MessageInterface::ShowMessage("SignalBase::GetParamDerivative(forObj = <%s>) end\n", forObj->GetName().c_str());
      return 0.0;
   }

   // Get C derivative vector
   Rvector dVector;
   if (associateObj)
      GetCDerivativeVector(associateObj, dVector, paramFullName, theData);
   else
      GetCDerivativeVector(forObj, dVector, paramFullName, theData);

   // Get paramName partial derivative
   Real deriv = dVector[paramIndex];

   //MessageInterface::ShowMessage("Found parameter with name <%s> in StmElementNames: deriv = %.12le    Index = %d\n", paramFullName.c_str(), deriv, paramIndex);
   //MessageInterface::ShowMessage("SignalBase::GetParamDerivative(forObj = <%s>) end\n", forObj->GetName().c_str());
   return deriv;
}


// Note that: forObj is always a Spacecraft object (it is never a Groundstation object due to ground station does not has its own STM)
void GPSPointMeasureModel::GetCDerivativeVector(GmatBase *forObj, Rvector &deriv, const std::string &solveForType, SignalData *theData)
{
   // 1. Calculate phi matrix
   //MessageInterface::ShowMessage("forObj = <%p,%s>      theData.tNode = <%p,%s>    theData.rNode = <%p,%s>\n",
   //   forObj, forObj->GetName().c_str(),
   //   theData.tNode, theData.tNode->GetName().c_str(),
   //   theData.rNode, theData.rNode->GetName().c_str());


   bool forTransmitter = true;
   if (theData->rNode == forObj)
      forTransmitter = false;
   else
   {
      if (theData->tNode != forObj)
         throw MeasurementException(solveForType + " derivative requested, but "
            "neither participant is the \"for\" object");
   }
   Rmatrix phi = (forTransmitter ? (theData->tSTM*theData->tSTMtm.Inverse()) : (theData->rSTM*theData->rSTMtm.Inverse()));

   // 2. Calculate E matrix
   Integer m = phi.GetNumColumns() - 6;
   Rmatrix E(3, m);
   for (Integer i = 0; i < 3; ++i)
   {
      for (Integer j = 0; j < m; ++j)
      {
         E(i, j) = phi(i, j + 6);
      }
   }

   // 3. Calculate: sign * R * phi
   Real sign = (forTransmitter ? -1.0 : 1.0);

   Rmatrix33 body2FK5_matrix = (forTransmitter ? theData->tJ2kRotation :
      theData->rJ2kRotation);
   Rmatrix tempMatrix(3, m);
   // tempMatrix = body2FK5_matrix * E;
   for (Integer i = 0; i < 3; ++i)
   {
      for (Integer j = 0; j < m; ++j)
      {
         tempMatrix(i, j) = 0.0;
         for (Integer k = 0; k < 3; ++k)
            tempMatrix(i, j) += sign * body2FK5_matrix(i, k) * E(k, j);
      }
   }


   // 4. Calculate range unit vector
   Rvector3 rangeVec = theData->rangeVecInertial;
   Rvector3 unitRange = rangeVec / rangeVec.GetMagnitude();


   // 5. Calculate C vector derivative
   deriv.SetSize(m);
   for (Integer j = 0; j < m; ++j)
   {
      deriv[j] = 0.0;
      for (Integer i = 0; i < 3; ++i)
         deriv[j] += unitRange[i] * tempMatrix(i, j);
   }
}



