//------------------------------------------------------------------------------
//                           GPSAdapter
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
// Author: Tuan Dang Nguyen, GSFC/NASA
// Created: Aug 24, 2016
/**
 * A measurement adapter for GPS position vector in Km
 */
//------------------------------------------------------------------------------

#include "GPSAdapter.hpp"
#include "GPSPointMeasureModel.hpp"
#include "RandomNumber.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "Spacecraft.hpp"
#include "ErrorModel.hpp"
#include "Receiver.hpp"


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_RANGE_CALCULATION


//------------------------------------------------------------------------------
// GPSAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
GPSAdapter::GPSAdapter(const std::string& name) :
   TrackingDataAdapter("GPS_PosVec", name),
   ecf  (NULL),
   ej2k (NULL),
   cv   (NULL)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GPSAdapter default constructor <%p>\n", this);
#endif
   parameterCount = GPSAdapterParamCount;

   dimNames.push_back("X");
   dimNames.push_back("Y");
   dimNames.push_back("Z");
}


//------------------------------------------------------------------------------
// ~GPSAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GPSAdapter::~GPSAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GPSAdapter default destructor  <%p>\n", this);
#endif

   if (ecf)
      delete ecf;
   if (ej2k)
      delete ej2k;
   if (cv)
      delete cv;
}


//------------------------------------------------------------------------------
// GPSAdapter(const GPSAdapter& gps)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param gps The adapter copied to make this one
 */
//------------------------------------------------------------------------------
GPSAdapter::GPSAdapter(const GPSAdapter& gps) :
   TrackingDataAdapter      (gps),
   ecf                      (NULL),
   ej2k                     (NULL),
   cv                       (NULL)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GPSAdapter copy constructor   from <%p> to <%p>\n", &gps, this);
#endif

}


//------------------------------------------------------------------------------
// GPSAdapter& operator=(const GPSAdapter& gps)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param gps The adapter copied to make this one match it
 *
 * @return This adapter made to look like gps
 */
//------------------------------------------------------------------------------
GPSAdapter& GPSAdapter::operator=(const GPSAdapter& gps)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GPSAdapter operator =   set <%p> = <%p>\n", this, &gps);
#endif

   if (this != &gps)
   {
      TrackingDataAdapter::operator=(gps);
   }

   ecf  = NULL;
   ej2k = NULL;
   cv   = NULL;

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
GmatBase* GPSAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GPSAdapter::Clone() clone this <%p>\n", this);
#endif

   return new GPSAdapter(*this);
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
std::string GPSAdapter::GetParameterText(const Integer id) const
{
   if (id >= AdapterParamCount && id < GPSAdapterParamCount)
      return PARAMETER_TEXT[id - AdapterParamCount];
   return TrackingDataAdapter::GetParameterText(id);
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
Integer GPSAdapter::GetParameterID(const std::string& str) const
{
   for (Integer i = AdapterParamCount; i < GPSAdapterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - AdapterParamCount])
         return i;
   }
   return TrackingDataAdapter::GetParameterID(str);
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
Gmat::ParameterType GPSAdapter::GetParameterType(const Integer id) const
{
   if (id >= AdapterParamCount && id < GPSAdapterParamCount)
      return PARAMETER_TYPE[id - AdapterParamCount];

   return TrackingDataAdapter::GetParameterType(id);
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
std::string GPSAdapter::GetParameterTypeString(const Integer id) const
{
   return MeasurementModelBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
bool GPSAdapter::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = TrackingDataAdapter::RenameRefObject(type, oldName, newName);

   // Handle additional renames specific to this adapter

   return retval;
}


//------------------------------------------------------------------------------
// bool SetMeasurement(MeasureModel* meas)
//------------------------------------------------------------------------------
/**
 * Sets the measurement model pointer
 *
 * @param meas The pointer
 *
 * @return true if set, false if not
 */
//------------------------------------------------------------------------------
bool GPSAdapter::SetMeasurement(MeasureModel* meas)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("GPSAdapter<%p>::SetMeasurement(meas = <%p,%s>)\n", this, meas, meas->GetName().c_str()); 
#endif

   return TrackingDataAdapter::SetMeasurement(meas);
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
bool GPSAdapter::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Start Initializing a GPSAdapter <%p>\n", this);
   #endif

   bool retval = false;

   if (TrackingDataAdapter::Initialize())
   {
      retval = true;

      if (participantLists.empty())
         throw MeasurementException("Error: No participant is defined in GPS tracking configuration.\n");

      if (participantLists.size() > 1)
         throw MeasurementException("Error: .gmd files do not support "
               "multiple strands\n");

      for (UnsignedInt i = 0; i < participantLists.size(); ++i)
      {
         if (participantLists[i]->size() != 1)
         {
            std::stringstream ss;
            ss << "Error: Number of participant is " << participantLists[i]->size() << ". GPS measurement allows only one participant.\n";
            throw MeasurementException(ss.str());
         }

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
            cMeasurement.participantIDs.push_back(theId);
         }
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("End Initializing a GPSAdapter <%p>\n", this);
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
 * @param withEvents          Flag indicating is the light time solution should be
 *                            included
 * @param forObservation      The observation data associated with this measurement
 * @param rampTB              Ramp table for a ramped measurement
 *
 * @return The computed measurement data
 */
//------------------------------------------------------------------------------
const MeasurementData& GPSAdapter::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTB,
      bool forSimulation)
{
   #ifdef DEBUG_ADAPTER_EXECUTION
      MessageInterface::ShowMessage("GPSAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTB);
   #endif
   
   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
            instanceName + " before the measurement was set");
   
   // Fire the measurement model to build the collection of signal data
   if (((GPSPointMeasureModel*)calcData)->CalculateMeasurement(withLighttime, withMediaCorrection, forObservation, rampTB, forSimulation))
   {
      // 1. Get measurment data
      std::vector<SignalData*> data = calcData->GetSignalData();

      cMeasurement.epochGT = data[0]->rPrecTime;
      cMeasurement.epoch = data[0]->rPrecTime.GetMjd();
      cMeasurement.isFeasible = true;
      cMeasurement.feasibilityValue = 0.0;
      cMeasurement.unfeasibleReason = "N";            // Normal

      CelestialBody* earthBody = solarsys->GetBody("Earth");
      // 2. Specify GPS position vector:
      // 2.1. Get spacecraft's location w.r.t. the spacecraft's origin MJ2000Eq coordinate system
      Rvector3 pos = data[0]->rLoc;
      // 2.2. Convert spacecraft's location to EarthMJ2000Eq coordiante system
      Spacecraft* sc = (Spacecraft*)(data[0]->rNode);
      CoordinateSystem* cs = (CoordinateSystem*)(sc->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
      if (cs->GetOrigin()->GetName() != "Earth")
      {
         CelestialBody * origin = (CelestialBody*)(cs->GetOrigin());
         Rvector3 fromScOriginToEarth = earthBody->GetMJ2000Position(cMeasurement.epochGT) - origin->GetMJ2000Position(cMeasurement.epochGT);
         pos = pos - fromScOriginToEarth;
      }
      // 2.3. Convert EarthMJ200Eq to EarthFixed coordinate system
      Rvector instate(6, pos[0], pos[1], pos[2], 0.0, 0.0, 0.0);
      Rvector outstate(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      if (ecf == NULL)
         ecf = CoordinateSystem::CreateLocalCoordinateSystem("ecf", "BodyFixed",
            earthBody, NULL, NULL, earthBody, solarsys);
      if (ej2k == NULL)
         ej2k = CoordinateSystem::CreateLocalCoordinateSystem("emj2k", "MJ2000Eq",
            earthBody, NULL, NULL, earthBody, solarsys);
      if (cv == NULL)
         cv = new CoordinateConverter();
      cv->Convert(A1Mjd(cMeasurement.epoch), instate, ej2k, outstate, ecf, true, true);

      RealArray values;
      for (Integer i = 0; i < 3; ++i)
         values.push_back(outstate[i]);


      // Get GPS Receiver
      std::string receiverName = gpsReceiverName.substr(gpsReceiverName.find_last_of('.') + 1);
      std::string scName = gpsReceiverName.substr(0, gpsReceiverName.find_last_of('.'));
      ObjectArray hwList = data[0]->rNode->GetRefObjectArray(Gmat::HARDWARE);
      Receiver* gpsReceiver = NULL;
      for (UnsignedInt i = 0; i < hwList.size(); ++i)
      {
         //if (hwList[i]->IsOfType("GPSReceiver"))
         if (hwList[i]->IsOfType("Receiver"))
         {
            //if (hwList[i]->GetName() == gpsReceiverName)
            if (hwList[i]->GetName() == receiverName)
            {
               gpsReceiver = (Receiver*)hwList[i];
               break;
            }
         }
      }
      if (gpsReceiver == NULL)
         throw MeasurementException("Error: No Receiver with name '" + receiverName + "' was defined in script and/or added to spacecraft '" + scName + "' to perform GPS measurement.\n");

      // Store value of Receiver.ID to MeasurementData 
      std::string theReceiverID = gpsReceiver->GetStringParameter("Id");
      cMeasurement.sensorIDs.clear();
      cMeasurement.sensorIDs.push_back(theReceiverID);

      // Get measurement ErrorModel
      if (measErrorModel == NULL)
      {
         ObjectArray objList = gpsReceiver->GetRefObjectArray(Gmat::ERROR_MODEL);
         for (UnsignedInt i = 0; i < objList.size(); ++i)
         {
            if (((ErrorModel*)objList[i])->GetStringParameter("Type") == "GPS_PosVec")
            {
               measErrorModel = (ErrorModel*)objList[i];
               break;
            }
         }
         if (measErrorModel == NULL)
            throw MeasurementException("Error: No GPS_PosVec error model is added to receiver '" + gpsReceiver->GetName() + "' to make GPS measurement.\n");
      }

      // Specify noise sigma from receiver's ErrorModel
      Real noise = measErrorModel->GetRealParameter("NoiseSigma");
      noiseSigma.clear();
      for (Integer i = 0; i < 3; ++i)
         noiseSigma.push_back(noise);
      
      // @todo: in current GMAT version, one-dimension bias is defined 
      // in GPS measurement model. It needs to add code to specify multi-dimension bias
      Real bias = measErrorModel->GetRealParameter("Bias");
      measurementBias.clear();
      for (Integer i = 0; i < 3; ++i)
         measurementBias.push_back(bias);

      // Specify measurement error covarian matrix
      UnsignedInt measurementSize = 3;                   // GPS point measurement has 3 dimesions
      measErrorCovariance.SetDimension(measurementSize);
      for (UnsignedInt i = 0; i < measurementSize; ++i)
      {
         for (Integer j = 0; j < measurementSize; ++j)
         {
            if (i != j)
               measErrorCovariance(i, j) = 0.0;
            else
            {
               if (noiseSigma[i] != 0.0)
                  measErrorCovariance(i, j) = noiseSigma[i] * noiseSigma[i];
               else
                  measErrorCovariance(i, j) = 1.0;          // if no noise setting, set it to 1.0
            }
         }
      }
      cMeasurement.covariance = &measErrorCovariance;
      
#ifdef DEBUG_RANGE_CALCULATION
      MessageInterface::ShowMessage("==============================================================================\n");
      MessageInterface::ShowMessage("====  GPSAdapter (%s): Position Vector Calculation\n", GetName().c_str());
      MessageInterface::ShowMessage("==============================================================================\n");

      MessageInterface::ShowMessage("      . Path : {");
      for (UnsignedInt k = 0; k < participantLists[0]->size(); ++k)
      {
         if (k == participantLists[0]->size()-1)
            MessageInterface::ShowMessage("%s", participantLists[0]->at(k).c_str());
         else
            MessageInterface::ShowMessage("%s,  ", participantLists[0]->at(k).c_str());
      }
      MessageInterface::ShowMessage("}\n");

      MessageInterface::ShowMessage("      . Measurement type           : <%s>\n", measurementType.c_str());
      MessageInterface::ShowMessage("      . C-value w/o noise and bias : [%.12lf, %.12lf, %.12lf] km \n", values[0], values[1], values[2]);
      MessageInterface::ShowMessage("      . Noise adding option        : %s\n", (addNoise ? "true" : "false"));
      MessageInterface::ShowMessage("      . Bias adding option         : %s\n", (addBias ? "true" : "false"));
      MessageInterface::ShowMessage("      . Position vector noise sigma: [%.12lf, %.12lf, %.12lf] km \n", noiseSigma[0], noiseSigma[1], noiseSigma[2]);
      MessageInterface::ShowMessage("      . Position vector bias       : [%.12lf, %.12lf, %.12lf] km \n", measurementBias[0], measurementBias[1], measurementBias[2]);
#endif


      // Set measurement values
      cMeasurement.value.clear();
      for (UnsignedInt i = 0; i < values.size(); ++i)
         cMeasurement.value.push_back(0.0);
      
      for (UnsignedInt i = 0; i < values.size(); ++i)
      {
         Real measVal = values[i];
         
         // Add noise to measurement value
         if (addNoise)
         {
            // Add noise here
            RandomNumber* rn = RandomNumber::Instance();
            Real val = rn->Gaussian(measVal, noiseSigma[i]);                  // noise sigma unit: Km
            measVal = val;
         }
         
         // Add bias to measurement value only after noise had been added in order to avoid adding bias' noise 
         if (addBias)
         {
            if (biasType == BIASTYPE_IS_PASSBIAS) //(timeGapForPassBreak > 0.0)
            {
               // add pass bias
               // specify pass number associate to the current observation
               Real passBias = measErrorModel->GetPassBias(cMeasurement.epochGT);
               measVal = measVal + passBias;                                    // pass bias unit: Km
            }
            else
            {
               // add bias
               measVal = measVal + measurementBias[i];                          // bias unit: Km
            }
         }

         cMeasurement.value[i] = measVal;
      }
      
#ifdef DEBUG_RANGE_CALCULATION
      MessageInterface::ShowMessage("      . C-value with noise and bias : [%.12lf, %.12lf, %.12lf] km\n", cMeasurement.value[0], cMeasurement.value[1], cMeasurement.value[2]);
      MessageInterface::ShowMessage("      . Measurement epoch A1Mjd     : %.12lf\n", cMeasurement.epoch);
      MessageInterface::ShowMessage("      . Covariance matrix           : <%p>\n", cMeasurement.covariance);
      if (cMeasurement.covariance)
      {
         MessageInterface::ShowMessage("      . Covariance matrix size = %d\n", cMeasurement.covariance->GetDimension());
         MessageInterface::ShowMessage("     [ ");
         for (UnsignedInt i = 0; i < cMeasurement.covariance->GetDimension(); ++i)
         {
            if (i > 0)
               MessageInterface::ShowMessage("\n");
            for (UnsignedInt j = 0; j < cMeasurement.covariance->GetDimension(); ++j)
               MessageInterface::ShowMessage("%le   ", cMeasurement.covariance->GetCovariance()->GetElement(i, j));
         }
         MessageInterface::ShowMessage("]\n");
      }

      MessageInterface::ShowMessage("===================================================================\n");
#endif
   }
   
   #ifdef DEBUG_ADAPTER_EXECUTION
      MessageInterface::ShowMessage("GPSAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) exit\n", (withEvents ? "true" : "false"), forObservation,
            rampTB);
   #endif

   return cMeasurement;
}


//-------------------------------------------------------------------------------------
// Real GetIonoCorrection()
//-------------------------------------------------------------------------------------
/**
* This function is used to get ionosphere correction (unit: km)
*
* @return     ionosphere correction of a measurment in km
*/
//-------------------------------------------------------------------------------------
Real GPSAdapter::GetIonoCorrection()
{
   return 0.0;
}


//-------------------------------------------------------------------------------------
// Real GetTropoCorrection()
//-------------------------------------------------------------------------------------
/**
* This function is used to get troposphere correction (unit: km)
*
* @return     troposphere correction of a measurment in km
*/
//-------------------------------------------------------------------------------------
Real GPSAdapter::GetTropoCorrection()
{
   return 0.0;
}


//------------------------------------------------------------------------------
// bool ReCalculateFrequencyAndMediaCorrection(UnsignedInt pathIndex, 
//        Real uplinkFrequency, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------
/**
* This function is used to recalculate frequency and media correction for TDRS 
* Doppler measurement.
*
* @param pathIndex           Calculation for the given signal path specified by
*                            pathIndex
* @param uplinkFrequency     Transmit frequency
* @param rampTB              Ramp table for a ramped measurement
*
* @return                    true if no error ocurrs, false otherwise
*/
//------------------------------------------------------------------------------
bool GPSAdapter::ReCalculateFrequencyAndMediaCorrection(UnsignedInt pathIndex, 
                         Real uplinkFrequency, std::vector<RampTableData>* rampTB)
{
   // It does nothing
   return true;
}




const MeasurementData& GPSAdapter::CalculateMeasurementAtOffset(
      bool withEvents, Real dt, ObservationData* forObservation,
      std::vector<RampTableData>* rampTB, bool forSimulation)
{
   static MeasurementData offsetMeas;

   // It does nothing
   
   return offsetMeas;
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
const std::vector<RealArray>& GPSAdapter::CalculateMeasurementDerivatives(
      GmatBase* obj, Integer id)
{
   if (!calcData)
      throw MeasurementException("Measurement derivative data was requested "
            "for " + instanceName + " before the measurement was set");

   //Integer parmId = GetParmIdFromEstID(id, obj);
   Integer parameterID;
   if (id > 250)
      parameterID = id - obj->GetType() * 250; //GetParmIdFromEstID(id, obj);
   else
      parameterID = id;
   std::string paramName = obj->GetParameterText(parameterID);

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Solve-for parameter: %s\n", paramName.c_str());
   #endif

   // Perform the calculations
   #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("GPSAdapter::CalculateMeasurement"
            "Derivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n",
            obj->GetFullName().c_str(), id, parameterID, cMeasurement.epoch);
   #endif

   // Clear derivative variable
   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      theDataDerivatives[i].clear();
   theDataDerivatives.clear();

   if (paramName == "Bias")
   {
      //Integer size = obj->GetEstimationParameterSize(id);
      Integer size = 3;    // X_bias, Y_bias, and Z_bias
      RealArray oneRow;
      oneRow.assign(size, 0.0);
      theDataDerivatives.push_back(oneRow);
   }
   else
   {
      const std::vector<RealArray> *derivativeData =
         &(((GPSPointMeasureModel*)calcData)->CalculateMeasurementDerivatives(obj, id));

      #ifdef DEBUG_ADAPTER_DERIVATIVES
         MessageInterface::ShowMessage("   Derivatives: [");
         for (UnsignedInt i = 0; i < derivativeData->size(); ++i)
         {
            if (i > 0)
               MessageInterface::ShowMessage("]\n                [");
            for (UnsignedInt j = 0; j < derivativeData->at(i).size(); ++j)
            {
               if (j > 0)
                  MessageInterface::ShowMessage(", ");
               MessageInterface::ShowMessage("%.12le", (derivativeData->at(i))[j]);
            }
         }
         MessageInterface::ShowMessage("]\n");
      #endif

      // Now assemble the derivative data into the requested derivative
      // Note that: multiplier is only applied for elements of spacecraft's state, position, and velocity
      Real factor = 1.0;

      UnsignedInt size = derivativeData->at(0).size();
      for (UnsignedInt i = 0; i < derivativeData->size(); ++i)
      {
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);

         if (derivativeData->at(i).size() != size)
            throw MeasurementException("Derivative data size is a different size "
                  "than expected");

         for (UnsignedInt j = 0; j < size; ++j)
         {
            theDataDerivatives[i][j] = (derivativeData->at(i))[j] * factor;
         }
      }
   }

   #ifdef DEBUG_ADAPTER_DERIVATIVES
      MessageInterface::ShowMessage("Exit GPSAdapter::CalculateMeasurementDerivatives():\n");
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
bool GPSAdapter::WriteMeasurements()
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
bool GPSAdapter::WriteMeasurement(Integer id)
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
Integer GPSAdapter::HasParameterCovariances(Integer parameterId)
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
Integer GPSAdapter::GetEventCount()
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
void GPSAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   TrackingDataAdapter::SetCorrection(correctionName, correctionType);
}


Rvector3 GPSAdapter::ConvertToEFCVector(const Rvector3 pos, const GmatEpoch epoch) const
{
   A1Mjd measTime(epoch);
   Rvector3 result;

   CelestialBody* earthBody = solarsys->GetBody("Earth");
   SpecialCelestialPoint* ssb = solarsys->GetSpecialPoint("SolarSystemBarycenter");
   Rvector3 ssbEarth = earthBody->GetMJ2000Position(measTime) - ssb->GetMJ2000Position(measTime);

   return pos - ssbEarth;
}
