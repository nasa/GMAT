//------------------------------------------------------------------------------
//                           PointRangeRateAdapterKps
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

#include "PointRangeRateAdapterKps.hpp"
#include "RandomNumber.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include <iostream>

//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_RANGE_CALCULATION
//#define DEBUG_DERIV


//------------------------------------------------------------------------------
// PointRangeRateAdapterKps(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
PointRangeRateAdapterKps::PointRangeRateAdapterKps(const std::string& name) :
   RangeAdapterKm       (name),
//   dopplerInterval      (1.0),
   targetSat            (NULL)
{
   typeName="RangeRate";
}


//------------------------------------------------------------------------------
// ~PointRangeRateAdapterKps()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PointRangeRateAdapterKps::~PointRangeRateAdapterKps()
{
}


//------------------------------------------------------------------------------
// PointRangeRateAdapterKps(const PointRangeRateAdapterKps& rr)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rr The adapter copied to make this one
 */
//------------------------------------------------------------------------------
PointRangeRateAdapterKps::PointRangeRateAdapterKps(const PointRangeRateAdapterKps& rr) :
   RangeAdapterKm       (rr),
//   dopplerInterval      (rr.dopplerInterval),
   targetSat            (NULL)
{
}


//------------------------------------------------------------------------------
// PointRangeRateAdapterKps& operator=(const PointRangeRateAdapterKps& rr)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rr The adapter copied to make this one match it
 *
 * @return This adapter made to look like rr
 */
//------------------------------------------------------------------------------
PointRangeRateAdapterKps& PointRangeRateAdapterKps::operator=(const PointRangeRateAdapterKps& rr)
{
   if (this != &rr)
   {
      RangeAdapterKm::operator=(rr);
//      dopplerInterval = rr.dopplerInterval;
      targetSat = NULL;
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
GmatBase* PointRangeRateAdapterKps::Clone() const
{
   return new PointRangeRateAdapterKps(*this);
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
std::string PointRangeRateAdapterKps::GetParameterText(const Integer id) const
{
//   if (id >= RangeAdapterKmParamCount && id < RangeRateAdapterParamCount)
//      return PARAMETER_TEXT[id - RangeAdapterKmParamCount];
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
Integer PointRangeRateAdapterKps::GetParameterID(const std::string& str) const
{
//   for (Integer i = RangeAdapterKmParamCount; i < RangeRateAdapterParamCount; i++)
//   {
//      if (str == PARAMETER_TEXT[i - RangeAdapterKmParamCount])
//         return i;
//   }
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
Gmat::ParameterType PointRangeRateAdapterKps::GetParameterType(const Integer id) const
{
//   if (id >= RangeAdapterKmParamCount && id < RangeRateAdapterParamCount)
//      return PARAMETER_TYPE[id - RangeAdapterKmParamCount];

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
std::string PointRangeRateAdapterKps::GetParameterTypeString(const Integer id) const
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
Real PointRangeRateAdapterKps::GetRealParameter(const Integer id) const
{
//   if (id == DOPPLER_INTERVAL)
//      return dopplerInterval;

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
Real PointRangeRateAdapterKps::SetRealParameter(const Integer id, const Real value)
{
//   if (id == DOPPLER_INTERVAL)
//   {
//      if (value <= 0.0)
//         throw MeasurementException("Error: Doppler interval must be a "
//               "positive value\n");
//
//      dopplerInterval = value;
//      return dopplerInterval;
//   }

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
Real PointRangeRateAdapterKps::GetRealParameter(const std::string &label) const
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
Real PointRangeRateAdapterKps::SetRealParameter(const std::string &label,
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
bool PointRangeRateAdapterKps::RenameRefObject(const UnsignedInt type,
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
bool PointRangeRateAdapterKps::Initialize()
{
   bool retval = false;

   if (RangeAdapterKm::Initialize())
   {
      // Handle additional initialization specific to this adapter (currently none)
      retval = true;
   }

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
const MeasurementData& PointRangeRateAdapterKps::CalculateMeasurement(
                              bool withEvents, ObservationData* forObservation,
                              std::vector<RampTableData>* rampTB,
                              bool forSimulation)
{
   // Compute range in km, at epoch and at epoch plus offset
   cMeasurement = RangeAdapterKm::CalculateMeasurement
         (false, NULL, NULL, forSimulation);

   if (cMeasurement.isFeasible)
   {
      std::vector<SignalData*> data = calcData->GetSignalData();

      // set range rate
      Real range_rate = 0;

      // some up the ranges
      if (data.size() != 1)
         throw MeasurementException("Signal data is poorly sized for point "
               "range rate adapter data");

      SignalData* strand = data[0];
      Real rdotv = 0.0;
      Integer count = 0;
      while (strand)
      {
         Real range = strand->rangeVecInertial.GetMagnitude();
         rdotv += strand->rangeRateVecInertial *
               strand->rangeVecInertial / range;
         strand = strand->next;
         ++count;
      }
      range_rate = rdotv / count;

      // set the measurement value
      cMeasurement.value.clear();
      cMeasurement.value.push_back(range_rate);
   }

   return cMeasurement;
}


//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(GmatBase* obj,
//       Integer id)
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
const std::vector<RealArray>& PointRangeRateAdapterKps::
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
   UnsignedInt size = 6;


   if (obj != NULL)
   {
      if (id > 250)
         parameterID = GetParmIdFromEstID(id, obj);
      else
         parameterID = id;

      // Point range rate is computed off of the measurement data
      std::vector<SignalData*> data = calcData->GetSignalData();

      if (obj->GetParameterText(parameterID) == "Position")
      {
         RealArray oneRow;
         oneRow.assign(3, 0.0);
         theDataDerivatives.clear();
         theDataDerivatives.push_back(oneRow);

         SignalData *upStrand = data[0];
         SignalData *downStrand = data[0]->next;

         Rmatrix33 I33(true);

         if ((downStrand == NULL) || (downStrand->next != NULL))
            throw MeasurementException("Range rate computations require signal "
                  "path of the form T1 -> S1 -> T1");

         if (obj->IsOfType(Gmat::GROUND_STATION))
            throw MeasurementException("Derivatives w.r.t. Station location "
                  "parameters are not yet supported");
         else
         {
            Real upRange = upStrand->rangeVecInertial.GetMagnitude();
            Real upRangeCubed = upRange * upRange * upRange;
            Real upRDotV =
                  upStrand->rangeVecInertial(0) * upStrand->rangeRateVecInertial(0) +
                  upStrand->rangeVecInertial(1) * upStrand->rangeRateVecInertial(1) +
                  upStrand->rangeVecInertial(2) * upStrand->rangeRateVecInertial(2);

            Real downRange = downStrand->rangeVecInertial.GetMagnitude();
            Real downRangeCubed = downRange * downRange * downRange;
            Real downRDotV =
                  downStrand->rangeVecInertial(0) * downStrand->rangeRateVecInertial(0) +
                  downStrand->rangeVecInertial(1) * downStrand->rangeRateVecInertial(1) +
                  downStrand->rangeVecInertial(2) * downStrand->rangeRateVecInertial(2);

            for (UnsignedInt ii = 0; ii < 3; ++ii)
            {
               theDataDerivatives[0][ii]   = 0.5 * (
                     (upStrand->rangeRateVecInertial(ii)   / upRange   -
                      upStrand->rangeVecInertial(ii)   * upRDotV   / upRangeCubed) -
                     (downStrand->rangeRateVecInertial(ii) / downRange -
                      downStrand->rangeVecInertial(ii) * downRDotV / downRangeCubed));
            }
         }
      }
      else if (obj->GetParameterText(parameterID) == "Velocity")
      {
         RealArray oneRow;
         oneRow.assign(3, 0.0);
         theDataDerivatives.clear();
         theDataDerivatives.push_back(oneRow);

         SignalData *upStrand = data[0];
         SignalData *downStrand = data[0]->next;

         if ((downStrand == NULL) || (downStrand->next != NULL))
            throw MeasurementException("Range rate computations require signal "
                  "path of the form T1 -> S1 -> T1");

         if (obj->IsOfType(Gmat::GROUND_STATION))
            throw MeasurementException("Derivatives w.r.t. Station location "
                  "parameters are not yet supported");
         else
         {
            Real upRange = upStrand->rangeVecInertial.GetMagnitude();
            Real downRange = downStrand->rangeVecInertial.GetMagnitude();
            for (UnsignedInt ii = 0; ii < 3; ++ii)
            {
               theDataDerivatives[0][ii]   = 0.5 *
                     (upStrand->rangeVecInertial(ii)   / upRange -
                      downStrand->rangeVecInertial(ii) / downRange);
            }
         }
      }
      else if (obj->GetParameterText(parameterID) == "CartesianX")
      {
         RealArray oneRow;
         oneRow.assign(6, 0.0);
         theDataDerivatives.clear();
         theDataDerivatives.push_back(oneRow);

         SignalData *upStrand = data[0];
         SignalData *downStrand = data[0]->next;

         Rmatrix33 I33(true);

         if ((downStrand == NULL) || (downStrand->next != NULL))
            throw MeasurementException("Range rate computations require signal "
                  "path of the form T1 -> S1 -> T1");

         if (obj->IsOfType(Gmat::GROUND_STATION))
            throw MeasurementException("Derivatives w.r.t. Station location "
                  "parameters are not yet supported");
         else
         {
            Real upRange = upStrand->rangeVecInertial.GetMagnitude();
            Real upRangeCubed = upRange * upRange * upRange;
            Real upRDotV =
                  upStrand->rangeVecInertial(0) * upStrand->rangeRateVecInertial(0) +
                  upStrand->rangeVecInertial(1) * upStrand->rangeRateVecInertial(1) +
                  upStrand->rangeVecInertial(2) * upStrand->rangeRateVecInertial(2);

            Real downRange = downStrand->rangeVecInertial.GetMagnitude();
            Real downRangeCubed = downRange * downRange * downRange;
            Real downRDotV =
                  downStrand->rangeVecInertial(0) * downStrand->rangeRateVecInertial(0) +
                  downStrand->rangeVecInertial(1) * downStrand->rangeRateVecInertial(1) +
                  downStrand->rangeVecInertial(2) * downStrand->rangeRateVecInertial(2);

            for (UnsignedInt ii = 0; ii < 3; ++ii)
            {
               theDataDerivatives[0][ii]   = 0.5 * (
                     (upStrand->rangeRateVecInertial(ii)   / upRange   -
                      upStrand->rangeVecInertial(ii)   * upRDotV   / upRangeCubed) -
                     (downStrand->rangeRateVecInertial(ii) / downRange -
                      downStrand->rangeVecInertial(ii) * downRDotV / downRangeCubed));
               theDataDerivatives[0][ii+3] = 0.5 *
                     (upStrand->rangeVecInertial(ii)   / upRange -
                      downStrand->rangeVecInertial(ii) / downRange);
            }
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("   Range [%.12lf  %.12f] Range Vec "
                     "[%s] [%s] Range rate [%s] [%s] Range Vec: [%s] [%s] "
                     "Range rate [%s] [%s]\n", upRange, downRange,
                     upStrand->rangeVecInertial.ToString(15).c_str(),
                     downStrand->rangeVecInertial.ToString(15).c_str(),
                     upStrand->rangeRateVecInertial.ToString(15).c_str(),
                     downStrand->rangeRateVecInertial.ToString(15).c_str(),
                     upStrand->rangeVecInertial.ToString(15).c_str(),
                     downStrand->rangeVecInertial.ToString(15).c_str(),
                     upStrand->rangeRateVecInertial.ToString(15).c_str(),
                     downStrand->rangeRateVecInertial.ToString(15).c_str());
            #endif
         }
      }
      else if (obj->GetParameterText(parameterID) == "Bias")
      {
         size = 1;
         for (UnsignedInt i = 0; i < size; ++i)
            theDataDerivatives[0][i] += 1.0;
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
   }

   #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("   Derivatives: [");
      for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      {
         if (i > 0)
            MessageInterface::ShowMessage("]\n                [");
         for (UnsignedInt j = 0; j < theDataDerivatives.at(i).size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("%.12le",
                  (theDataDerivatives.at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
   #endif

   return theDataDerivatives;
}


//------------------------------------------------------------------------------
// bool WriteMeasurements()
//------------------------------------------------------------------------------
/**
 * Method to write measurements
 *
 * @todo Implement this method
 *
 * @return true if written, false if not
 */
//------------------------------------------------------------------------------
bool PointRangeRateAdapterKps::WriteMeasurements()
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
bool PointRangeRateAdapterKps::WriteMeasurement(Integer id)
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
Integer PointRangeRateAdapterKps::HasParameterCovariances(Integer parameterId)
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
Integer PointRangeRateAdapterKps::GetEventCount()
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
void PointRangeRateAdapterKps::SetCorrection(const std::string& correctionName,
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
bool PointRangeRateAdapterKps::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name)
{
   if (obj->IsOfType(Gmat::SPACECRAFT))
      targetSat = (SpaceObject*) obj;
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
bool PointRangeRateAdapterKps::SetRefObject(GmatBase* obj,
      const UnsignedInt type, const std::string& name, const Integer index)
{
   if (obj->IsOfType(Gmat::SPACECRAFT))
      targetSat = (SpaceObject*) obj;
   return RangeAdapterKm::SetRefObject(obj, type, name, index);
}
