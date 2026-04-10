//------------------------------------------------------------------------------
//                           RangeRateAdapterKps
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
// Author: Michael Barrucco, Thinking Systems, Inc.
// Created: Oct 1st, 2014
/**
 * A measurement adapter for rangerates in Km/s
 */
//------------------------------------------------------------------------------

#include "RangeRateAdapterKps.hpp"
#include "RandomNumber.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include <iostream>
#include <sstream>

//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_RANGE_CALCULATION
//#define DEBUG_DERIV


//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
RangeRateAdapterKps::
PARAMETER_TEXT[RangeRateAdapterParamCount - RangeAdapterKmParamCount] =
{
   "DopplerInterval",                   
};

const Gmat::ParameterType
RangeRateAdapterKps::
PARAMETER_TYPE[RangeRateAdapterParamCount - RangeAdapterKmParamCount] =
{
   Gmat::REAL_TYPE,   
};



//------------------------------------------------------------------------------
// RangeRateAdapterKps(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
RangeRateAdapterKps::RangeRateAdapterKps(const std::string& name) :
   RangeAdapterKm       (name),
   dopplerInterval      (1.0)
{
   typeName="RangeRateKps";             //typeName="RangeRate";
   parameterCount = RangeRateAdapterParamCount;
}


//------------------------------------------------------------------------------
// ~RangeRateAdapterKps()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RangeRateAdapterKps::~RangeRateAdapterKps()
{
}


//------------------------------------------------------------------------------
// RangeRateAdapterKps(const RangeRateAdapterKps& rr)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rr The adapter copied to make this one
 */
//------------------------------------------------------------------------------
RangeRateAdapterKps::RangeRateAdapterKps(const RangeRateAdapterKps& rr) :
   RangeAdapterKm       (rr),
   dopplerInterval      (rr.dopplerInterval)
{
}


//------------------------------------------------------------------------------
// RangeRateAdapterKps& operator=(const RangeRateAdapterKps& rr)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rr The adapter copied to make this one match it
 *
 * @return This adapter made to look like rr
 */
//------------------------------------------------------------------------------
RangeRateAdapterKps& RangeRateAdapterKps::operator=(
      const RangeRateAdapterKps& rr)
{
   if (this != &rr)
   {
      RangeAdapterKm::operator=(rr);
      dopplerInterval = rr.dopplerInterval;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new adapter that matches this one
 *
 * @return A new adapter set to match this one
 */
//------------------------------------------------------------------------------
GmatBase* RangeRateAdapterKps::Clone() const
{
   return new RangeRateAdapterKps(*this);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the script name for the parameter
 *
 * @param id The id of the parameter
 *
 * @return The script name
 */
//------------------------------------------------------------------------------
std::string RangeRateAdapterKps::GetParameterText(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < RangeRateAdapterParamCount)
      return PARAMETER_TEXT[id - RangeAdapterKmParamCount];
   return RangeAdapterKm::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a scriptable parameter
 *
 * @param str The string used for the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer RangeRateAdapterKps::GetParameterID(const std::string& str) const
{
   for (Integer i = RangeAdapterKmParamCount; i < RangeRateAdapterParamCount;
         ++i)
   {
      if (str == PARAMETER_TEXT[i - RangeAdapterKmParamCount])
         return i;
   }
   return RangeAdapterKm::GetParameterID(str);
}



//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a specified parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType RangeRateAdapterKps::GetParameterType(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < RangeRateAdapterParamCount)
      return PARAMETER_TYPE[id - RangeAdapterKmParamCount];

   return RangeAdapterKm::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description for a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The description string
 */
//------------------------------------------------------------------------------
std::string RangeRateAdapterKps::GetParameterTypeString(const Integer id) const
{
   return MeasurementModelBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// std::string GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real RangeRateAdapterKps::GetRealParameter(const Integer id) const
{
   if (id == DOPPLER_INTERVAL)
      return dopplerInterval;

   return RangeAdapterKm::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real RangeRateAdapterKps::SetRealParameter(const Integer id, const Real value)
{
   if (id == DOPPLER_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: Doppler interval must be a "
               "positive value\n");

      dopplerInterval = value;
      return dopplerInterval;
   }

   return RangeAdapterKm::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param label The name for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real RangeRateAdapterKps::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real RangeRateAdapterKps::SetRealParameter(const std::string &label,
      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string& oldName,
//       const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Method used to rename reference objects
 *
 * @param type The type of the object that is renamed
 * @param oldName The old object name
 * @param newName The new name
 *
 * @return true if a rename happened, false if not
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = RangeAdapterKm::RenameRefObject(type, oldName, newName);

   // Handle additional renames specific to this adapter (currently none)

   return retval;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the adapter for use
 *
 * @return true if the initialization succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::Initialize()
{
   bool retval = false;

   // Set up a second signal path identical to the first
   if (participantLists.size() == 1)
   {
      StringArray *secondStrand = new StringArray(*(participantLists[0]));
      participantLists.push_back(secondStrand);

      for (UnsignedInt i = 0; i < secondStrand->size(); ++i)
         calcData->SetStringParameter("SignalPath", secondStrand->at(i), 1);
   }

   // Epoch is at start!
   calcData->SetTimeTagFlag(false);

   // Validate identical strands
   if (participantLists.size() != 2)
      throw MeasurementException("Range rate adapter strand data is not "
            "initializing correctly");
   if (participantLists[0]->size() != participantLists[1]->size())
      throw MeasurementException("Range rate adapter strand data is not "
            "initializing correctly");
   for (UnsignedInt i = 0; i < participantLists[0]->size(); ++i)
      if (participantLists[0]->at(i) != participantLists[1]->at(i))
         throw MeasurementException("Range rate adapter strand data is not "
                     "initializing correctly");

   if (TrackingDataAdapter::Initialize())
   {
      retval = true;

      for (UnsignedInt i = 0; i < participantLists.size(); ++i)
      {
         for (UnsignedInt j = 0; j < participantLists[i]->size(); ++j)
         {
            std::string theId;

            for (UnsignedInt k = 0; k < refObjects.size(); ++k)
            {
               if (refObjects[k]->GetName() == participantLists[i]->at(j))
               {
                  theId = refObjects[k]->GetStringParameter("Id");
                  break;
               }
            }
            // Hide the duplicate strand from the data file
            if (i == 0)
               cMeasurement.participantIDs.push_back(theId);
         }
      }
   }

   // Calculate measurement covariance
   // We use 2 strands to build a single measuremetn value for this model
   // std::vector<SignalData*> data = calcData->GetSignalData();
   Integer measurementSize = 1; //data.size();
   measErrorCovariance.SetDimension(measurementSize);
   for (Integer i = 0; i < measurementSize; ++i)
   {
      for (Integer j = 0; j < measurementSize; ++j)
      {
         measErrorCovariance(i,j) = (i == j ?
                        (noiseSigma[0] != 0.0 ? (noiseSigma[0] * noiseSigma[0]) : 1.0) :            // noiseSigma[0] is used for Range in Km. Its unit is Km
                        0.0);
      }
   }

   #ifdef DEBUG_INITIALIZATON
      MessageInterface::ShowMessage("%d strands:\n", participantLists.size());
      for (UnsignedInt i = 0; i < participantLists.size(); ++i)
      {
         for (UnsignedInt j = 0; j < participantLists[i]->size(); ++j)
         {
            if (j == 0)
               MessageInterface::ShowMessage("   ");
            else
               MessageInterface::ShowMessage(" --> ");
            MessageInterface::ShowMessage("%s", participantLists[i]->at(j).c_str());
         }
         MessageInterface::ShowMessage("\n");
      }
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// const MeasurementData& CalculateMeasurement(bool withEvents,
//       ObservationData* forObservation, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------
/**
 * Computes the measurement associated with this adapter
 *
 * @note: The parameters associated with this call will probably be removed;
 * they are here to support compatibility with the old measurement models
 *
 * @param withEvents Flag indicating is the light time solution should be
 *                   included
 * @param forObservation The observation data associated with this measurement
 * @param rampTB Ramp table for a ramped measurement
 *
 * @return The computed measurement data
 */
//------------------------------------------------------------------------------
const MeasurementData& RangeRateAdapterKps::CalculateMeasurement(
                              bool withEvents, ObservationData* forObservation,
                              std::vector<RampTableData>* rampTB, 
                              bool forSimulation)
{
   // Compute range in km, at epoch and at epoch plus offset
   cMeasurement1 = CalculateMeasurementAtOffset(false, 0.0, NULL, NULL, 0, forSimulation);
   cMeasurement2 = CalculateMeasurementAtOffset(false, dopplerInterval,
                      NULL, NULL, 1, forSimulation);

   if ((cMeasurement1.isFeasible) && (cMeasurement2.isFeasible))
   {
      // two way range
      Real two_way_range  = 0.0;
      Real two_way_range2 = 0.0;

      // set range rate
      Real range_rate = 0;

      // sum up the ranges
      for (UnsignedInt i = 0; i < cMeasurement1.value.size(); i++)
      {
        two_way_range  = two_way_range  + cMeasurement1.value[i];
        two_way_range2 = two_way_range2 + cMeasurement2.value[i];
      }

      // one way range
      Real one_way_range  = two_way_range  / 2.0;
      Real one_way_range2 = two_way_range2 / 2.0;

      #ifdef DEBUG_MEASUREMENT
         MessageInterface::ShowMessage("R1: %lf, R2: %lf\n", one_way_range,
               one_way_range2);
      #endif

      // Compute range-rate
      range_rate = (one_way_range2-one_way_range)/(dopplerInterval);

      // set the measurement value
      cMeasurement.value.clear();
      cMeasurement.value.push_back(range_rate);
      cMeasurement.isFeasible = true;

      // Get measurement epoch in the first signal path. It will apply for all other paths
      cMeasurement.epochGT = cMeasurement1.epochGT;
      cMeasurement.epoch   = cMeasurement1.epoch;

      #ifdef DEBUG_RANGE_CALCULATION
        MessageInterface::ShowMessage("epoch %f, range %f, range rate %f, "
              "isFeasible %s, range2 %f, dRange %le\n", cMeasurement.epoch,
              one_way_range, range_rate,
              (cMeasurement.isFeasible ? "true" : "false"), one_way_range2,
              one_way_range - one_way_range2);
      #endif
   }
   else
      cMeasurement.isFeasible = false;

   #ifdef DEBUG_MEASUREMENT
      if (cMeasurement.isFeasible)
         MessageInterface::ShowMessage("%.12lf %.12lf ", cMeasurement.epoch,
               cMeasurement.value[0]);
   #endif

   return cMeasurement;
}


//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(GmatBase* obj,
//                                     Integer id)
//------------------------------------------------------------------------------
/**
 * Computes measurement derivatives for a given parameter on a given object
 *
 * @param obj The object that has the w.r.t. parameter
 * @param id  The ID of the w.r.t. parameter
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& RangeRateAdapterKps::
            CalculateMeasurementDerivatives(GmatBase* obj, Integer id)
{
      if (!calcData)
         throw MeasurementException("Measurement derivative data was requested "
               "for " + instanceName + " before the measurement was set");

      // Perform the calculations
      #ifdef DEBUG_ADAPTER_DERIVATIVES
         Integer parmId = GetParmIdFromEstID(id, obj);
         MessageInterface::ShowMessage("RangeAdapterKm::CalculateMeasurement"
               "Derivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n",
               obj->GetName().c_str(), id, parmId, cMeasurement.epoch);
      #endif

      Integer parameterID = -1;

      if (obj == NULL)
         throw MeasurementException("With respect to object for derivatives is "
               "not set correctly in a RangeRate measurement");

      if (id > 250)
         parameterID = GetParmIdFromEstID(id, obj);
      else
         parameterID = id;

      if (obj->GetParameterText(parameterID) == "Position")
      {
         std::vector<RealArray> dv =
               calcData->CalculateMeasurementDerivatives(obj, id);
         if (dv.size() != 2)
            throw MeasurementException("The 2-way range rate measurement is "
                  "improperly sized");

         RealArray oneRow;
         oneRow.assign(3, 0.0);
         theDataDerivatives.clear();
         theDataDerivatives.push_back(oneRow);

         // Now put the two strands together
         for (UnsignedInt i = 0; i < 6; ++i)
            theDataDerivatives[0][i] = (dv[1][i] - dv[0][i]) / dopplerInterval;
      }
      else if (obj->GetParameterText(parameterID) == "Velocity")
      {
         std::vector<RealArray> dv =
               calcData->CalculateMeasurementDerivatives(obj, id);
         if (dv.size() != 2)
            throw MeasurementException("The 2-way range rate measurement is "
                  "improperly sized");

         RealArray oneRow;
         oneRow.assign(3, 0.0);
         theDataDerivatives.clear();
         theDataDerivatives.push_back(oneRow);

         // Now put the two strands together
         for (UnsignedInt i = 0; i < 6; ++i)
            theDataDerivatives[0][i] = (dv[1][i] - dv[0][i]) / dopplerInterval;
      }
      else if (obj->GetParameterText(parameterID) == "CartesianX")
      {
         std::vector<RealArray> dv =
               calcData->CalculateMeasurementDerivatives(obj, id);
         if (dv.size() != 2)
            throw MeasurementException("The 2-way range rate measurement is "
                  "improperly sized");

         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("Derivative data:\n");
            for (UnsignedInt i = 0; i < dv.size(); ++i)
            {
               MessageInterface::ShowMessage("   %d:  ", i);
               for (UnsignedInt j = 0; j < dv[i].size(); ++j)
               {
                  if (j > 0)
                     MessageInterface::ShowMessage(", ");
                  MessageInterface::ShowMessage("%.12le", dv[i][j]);
               }
               MessageInterface::ShowMessage("\n");
            }
         #endif

         RealArray oneRow;
         oneRow.assign(6, 0.0);
         theDataDerivatives.clear();
         theDataDerivatives.push_back(oneRow);

         // Now put the two strands together
         for (UnsignedInt i = 0; i < 6; ++i)
            theDataDerivatives[0][i] = (dv[1][i] - dv[0][i]) / dopplerInterval;
      }
      else if (obj->GetParameterText(parameterID) == "Bias")
      {
         RealArray oneRow;
         oneRow.assign(1, 0.0);
         theDataDerivatives.clear();
         theDataDerivatives.push_back(oneRow);
         theDataDerivatives[0][0] = 1.0;
      }
      else
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. something "
                     "independent, so zero\n");
         #endif
         for (UnsignedInt i = 0; i < 3; ++i)
            theDataDerivatives[0][i] += 0.0;
      }

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("Deriv:\n");
      for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      {
         for (UnsignedInt j = 0; j < theDataDerivatives[i].size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(" ");
            else
               MessageInterface::ShowMessage("   [");
            MessageInterface::ShowMessage("%.12le", theDataDerivatives[i][j]);
         }
         MessageInterface::ShowMessage("]\n");
      }
   #endif

   return theDataDerivatives;
}

//------------------------------------------------------------------------------
// bool WriteMeasurements()
//------------------------------------------------------------------------------
/**
 * Method to write measurements
 *
 * @todo

 Implement this method
 *
 * @return true if written, false if not
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::WriteMeasurements()
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// bool WriteMeasurement(Integer id)
//------------------------------------------------------------------------------
/**
 * Method to write a specific measurement
 *
 * @todo Implement this method
 *
 * @param id ID of the parameter to write
 *
 * @return true if written, false if not
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::WriteMeasurement(Integer id)
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// Integer HasParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
 * Checks to see if adapter has covariance data for a specified parameter ID
 *
 * @param paramID The parameter ID
 *
 * @return Size of the covariance data that is available
 */
//------------------------------------------------------------------------------
Integer RangeRateAdapterKps::HasParameterCovariances(Integer parameterId)
{
   Integer retval = 0;
   return retval;
}


//------------------------------------------------------------------------------
// Integer GetEventCount()
//------------------------------------------------------------------------------
/**
 * Returns the number of light time solution events in the measurement
 *
 * @return The event count
 */
//------------------------------------------------------------------------------
Integer RangeRateAdapterKps::GetEventCount()
{
   Integer retval = 0;
   return retval;
}


//------------------------------------------------------------------------------
// void SetCorrection(const std::string& correctionName,
//       const std::string& correctionType)
//------------------------------------------------------------------------------
/**
 * Passes a correction name into the owned CoreMeasurement
 *
 * @param correctionName The name of the correction
 * @param correctionType The type of correction
 *
 * @note This information originally was not passed via SetStringParameter
 *       because it wasn't managed by scripting on MeasurementModels.  It was
 *       managed in the TrackingSystem code.  Since it is now scripted on the
 *       measurement -- meaning on the adapter -- this code should move into the
 *       Get/SetStringParameter methods.  It is included here !!TEMPORARILY!!
 *       to get a scripted adapter functioning in GMAT Nav.
 */
//------------------------------------------------------------------------------
void RangeRateAdapterKps::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   TrackingDataAdapter::SetCorrection(correctionName, correctionType);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets reference objects: overridden from base class to facilitate epoch
 * generation
 *
 * @param obj The reference object
 * @param type The object type
 * @param name The name of the object
 *
 * @return true is the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name)
{
   return RangeAdapterKm::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets reference objects: overridden from base class to facilitate epoch
 * generation
 *
 * @param obj The reference object
 * @param type The object type
 * @param name The name of the object
 * @param index Index into the array of objects that receive this one
 *
 * @return true is the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool RangeRateAdapterKps::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name, const Integer index)
{
   return RangeAdapterKm::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// const MeasurementData& CalculateMeasurementAtOffset(
//       bool withEvents, Real dt, ObservationData* forObservation,
//       std::vector<RampTableData>* rampTB, Integer forStrand, bool forSimulation)
//------------------------------------------------------------------------------
/**
 * Calculate the measurement at a time offset from the base epoch
 *
 * @note This code is adapterd from the RangeAdapterKm::CalculateMeasurement
 * method.  The single strand and time offsets were added, but the bulk of the
 * code is a direct copy of the code found there.
 *
 * Computes the signal data an individual strand of the measurement, with a
 * time offset.  The interface here preserves the interfaces used in the Range
 * Adapter so that if it proves more globally useful, it can be moved into that
 * code.
 *
 * @param withEvents Flag usied in the legacy code to toggle light time.  Unused
 *                   here.
 * @param dt The time offset, in seconds, applied to teh start epoch of the
 *           strand.
 * @param forObservation Observation supplying extra data.  Unused in this code.
 * @param rampTB Ramp table data.  Unused in this code.
 * @param forStrand Strand index for the computations.
 * @param forSimulation     true, if it runs for sumulation, false, it runs for estimation 
 *
 * @return The measurement data
 */
//------------------------------------------------------------------------------
const MeasurementData& RangeRateAdapterKps::CalculateMeasurementAtOffset(
      bool withEvents, Real dt, ObservationData* forObservation,
      std::vector<RampTableData>* rampTB, Integer forStrand, bool forSimulation)
{
   static MeasurementData offsetMeas;

   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
            instanceName + " before the measurement was set");

   if ((forStrand == -1) || (forStrand >= (Integer)participantLists.size()))
      throw MeasurementException("Strand index is out of bounds");

   // Fire the measurement model to build the collection of signal data
   if (calcData->CalculateMeasurement(withLighttime, withMediaCorrection, forObservation, rampTB, 
         forSimulation, dt, forStrand))
   {
      std::vector<SignalData*> data = calcData->GetSignalData();
      std::string unfeasibilityReason;

      // set to default
      offsetMeas.isFeasible = true;
      offsetMeas.unfeasibleReason = "N";
      offsetMeas.feasibilityValue = 90.0;

      RealArray values;

      // Calculate C-value for signal path ith:
      values.push_back(0.0);
      SignalData *current = data[forStrand];
      SignalData *first = current;
      UnsignedInt legIndex = 0;

      while (current != NULL)
      {
         ++legIndex;

         // Set feasibility value
         if (current->feasibilityReason[0] == 'N')
         {
            if (current->stationParticipant)
               offsetMeas.feasibilityValue = current->feasibilityValue;
         }
         else if (current->feasibilityReason[0] == 'B')
         {
            std::stringstream ss;
            ss << "B" << legIndex << current->feasibilityReason.substr(1);
            current->feasibilityReason = ss.str();
            if (offsetMeas.unfeasibleReason[0] == 'N')
            {
               offsetMeas.unfeasibleReason = current->feasibilityReason;
               offsetMeas.isFeasible = false;
               offsetMeas.feasibilityValue = current->feasibilityValue;
            }
         }
         else if (current->feasibilityReason[0] == 'H')
         {
            std::stringstream ss;
            ss << "HORP";
            current->feasibilityReason = ss.str();
            if (offsetMeas.unfeasibleReason[0] == 'N')
            {
               offsetMeas.unfeasibleReason = current->feasibilityReason;
               offsetMeas.isFeasible = false;
               offsetMeas.feasibilityValue = current->feasibilityValue;
            }
         }

         // accumulate all light time range for signal path ith
         Rvector3 signalVec = current->rangeVecInertial;
         values[0] += signalVec.GetMagnitude();

         // accumulate all range corrections for signal path ith
         for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
         {
            if (current->useCorrection[j] && current->correctionTypes[j] == "Range")
               values[0] += current->corrections[j];
         }// for j loop

         // Get measurement epoch from the first member
         offsetMeas.epochGT = first->tPrecTime - 
                  first->tDelay / GmatTimeConstants::SECS_PER_DAY;
         offsetMeas.epoch = first->tPrecTime.GetMjd() -
                  first->tDelay/GmatTimeConstants::SECS_PER_DAY;

         current = current->next;
      }

      // Set measurement values
      offsetMeas.value.clear();
      for (UnsignedInt i = 0; i < values.size(); ++i)
         offsetMeas.value.push_back(0.0);

      for (UnsignedInt i = 0; i < values.size(); ++i)
      {
         Real measVal = values[i];

//         // Add noise to measurement value
//         if (addNoise)
//         {
//            // Add noise here
//            RandomNumber* rn = RandomNumber::Instance();
//            Real val = rn->Gaussian(measVal, nsigma);
//            while (val <= 0.0)
//               val = rn->Gaussian(measVal, nsigma);
//            measVal = val;
//         }
         offsetMeas.value[i] = measVal;

      }
   }

   return offsetMeas;
}
