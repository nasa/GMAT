//------------------------------------------------------------------------------
//                           DSNRangeAdapter
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
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: Aug 13, 2014
/**
 * A measurement adapter for DSN ranges in Range Unit (RU)
 */
//------------------------------------------------------------------------------

#include "DSNRangeAdapter.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "RandomNumber.hpp"
#include "ErrorModel.hpp"
#include <sstream>

//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_DERIVATIVE_CALCULATION
//#define DEBUG_RANGE_CALCULATION

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
DSNRangeAdapter::PARAMETER_TEXT[DSNRangeAdapterParamCount - RangeAdapterKmParamCount] =
{
   "RangeModuloConstant",
};


const Gmat::ParameterType
DSNRangeAdapter::PARAMETER_TYPE[DSNRangeAdapterParamCount - RangeAdapterKmParamCount] =
{
   Gmat::REAL_TYPE,
};




//------------------------------------------------------------------------------
// DSNRangeAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
DSNRangeAdapter::DSNRangeAdapter(const std::string& name) :
   RangeAdapterKm      (name),
   rangeModulo         (1.0e18)
{
   typeName = "DSN_SeqRange";              // change type name from "RangeKm" to "DSN_SeqRange"
   parameterCount = DSNRangeAdapterParamCount;

   rampTB = NULL; 
}


//------------------------------------------------------------------------------
// ~DSNRangeAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DSNRangeAdapter::~DSNRangeAdapter()
{
}


//------------------------------------------------------------------------------
// DSNRangeAdapter(const DSNRangeAdapter& dsnr)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param dsnr The adapter copied to make this one
 */
//------------------------------------------------------------------------------
DSNRangeAdapter::DSNRangeAdapter(const DSNRangeAdapter& dsnr) :
   RangeAdapterKm      (dsnr),
   rangeModulo         (dsnr.rangeModulo)
{
   rampTB = NULL;
}


//------------------------------------------------------------------------------
// DSNRangeAdapter& operator=(const DSNRangeAdapter& dsnr)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param dsnr The adapter copied to make this one match it
 *
 * @return This adapter made to look like rak
 */
//------------------------------------------------------------------------------
DSNRangeAdapter& DSNRangeAdapter::operator=(const DSNRangeAdapter& dsnr)
{
   if (this != &dsnr)
   {
      RangeAdapterKm::operator=(dsnr);

      rangeModulo = dsnr.rangeModulo;
      rampTB = NULL;
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
GmatBase* DSNRangeAdapter::Clone() const
{
   return new DSNRangeAdapter(*this);
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
std::string DSNRangeAdapter::GetParameterText(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < DSNRangeAdapterParamCount)
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
Integer DSNRangeAdapter::GetParameterID(const std::string& str) const
{
   for (Integer i = RangeAdapterKmParamCount; i < DSNRangeAdapterParamCount; i++)
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
Gmat::ParameterType DSNRangeAdapter::GetParameterType(const Integer id) const
{
   if (id >= RangeAdapterKmParamCount && id < DSNRangeAdapterParamCount)
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
std::string DSNRangeAdapter::GetParameterTypeString(const Integer id) const
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
Real DSNRangeAdapter::GetRealParameter(const Integer id) const
{
   if (id == RANGE_MODULO_CONSTANT)
      return rangeModulo;

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
Real DSNRangeAdapter::SetRealParameter(const Integer id, const Real value)
{
   if (id == RANGE_MODULO_CONSTANT)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: range modulo constant has a nonpositive value\n");

      rangeModulo = value;
      return rangeModulo;
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
Real DSNRangeAdapter::GetRealParameter(const std::string &label) const
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
Real DSNRangeAdapter::SetRealParameter(const std::string &label, const Real value)
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
bool DSNRangeAdapter::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = RangeAdapterKm::RenameRefObject(type, oldName, newName);

   // Handle additional renames specific to this adapter

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
bool DSNRangeAdapter::Initialize()
{
   bool retval = false;

   if (RangeAdapterKm::Initialize())
   {
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
const MeasurementData& DSNRangeAdapter::CalculateMeasurement(bool withEvents,
      ObservationData* forObservation, std::vector<RampTableData>* rampTable, 
      bool forSimulation)
{
   #ifdef DEBUG_RANGE_CALCULATION
      MessageInterface::ShowMessage("DSNRangeAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
            rampTable);
   #endif

   // 1. Set value for local variables
   rampTB = rampTable;
   Integer err = 0;
   if (rampTable != NULL)
   {
       try
       {
           BeginEndIndexesOfRampTable(err);
       }
       catch (MeasurementException exp)
       {
           cMeasurement.isFeasible = false;
           cMeasurement.unfeasibleReason = "R";

           //Do not throw an error if the Ramp table exists but 
           //no ramp record is found for a given time or gs
           if ((err == 1) || (err == 4) || (err == 5))
               return cMeasurement;
               
           throw exp;

           
       }
   }

   obsData = forObservation;
   // 1.2. Reset value for range modulo constant
   if (obsData)
      rangeModulo = obsData->rangeModulo;                                                                         // unit: RU
   
   // 2. Compute range in km
   RangeAdapterKm::CalculateMeasurement(withEvents, forObservation, rampTB, forSimulation);

   // 3. Convert range from km to RU and store in cMeasurement:
   for (UnsignedInt i = 0; i < cMeasurement.value.size(); ++i)
   {
      // 3.1. Specify uplink frequency
      // Note that: In the current version, only one signal path is used in AdapterConfiguration. Therefore, path index is 0 
      uplinkFreq        = calcData->GetUplinkFrequency(0, rampTB, obsData);                                                // unit: MHz
      uplinkFreqAtRecei = calcData->GetUplinkFrequencyAtReceivedEpoch(0, rampTB, obsData);                                 // unit: MHz
      freqBand          = calcData->GetUplinkFrequencyBand(0, rampTB, obsData);
      
      // 3.2. Specify multiplier (at time t1)
      // multiplier only changes its value only after range in km is computed
      multiplier = GetFrequencyFactor(uplinkFreq*1.0e6);

      // 3.2.1. Convert correction from km to RU. 
      // In RangeAdapterKm, cMeasurement.correction[i] already has tropo, iono, 
      // ET-TAI, hardware delay, and relativity correction in Km. At this point,
      // the corrects are converted from km to RU.
      Real lightspeed = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM;                  // unit: km/s
      cMeasurement.correction[i] = multiplier*(cMeasurement.correction[i]/lightspeed);

      // 3.3. Convert range from km to RU in cMeasurement.value[0]
      Integer errnum = 0;
      Real realTravelTime = cMeasurement.value[i]/lightspeed;                                                     // unit: seconds
      
      if (rampTB != NULL)
      {
         // ramped frequency
         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("Calculate DSN_SeqRange based on ramp table\n");
         #endif

         try
         {
            cMeasurement.value[i] = IntegralRampedFrequency(cMeasurement.epochGT, realTravelTime, errnum);                  // unit: RU
         } catch (MeasurementException exp)
         {
            cMeasurement.value[i] = 0.0;               // It has no C-value due to the failure of calculation of IntegralRampedFrequency()
            cMeasurement.isFeasible = false;
            cMeasurement.unfeasibleReason = "R";
            if ((errnum == 2)||(errnum == 3))
               throw exp;
         }
      }
      else
      {
         // constant frequency
         #ifdef DEBUG_RANGE_CALCULATION
            MessageInterface::ShowMessage("Calculate DSN_SeqRange based on constant frequency\n");
         #endif

         cMeasurement.value[i] = multiplier*realTravelTime;
      }
      Real C_idealVal = cMeasurement.value[i];
      
      if (measurementType == "DSN_SeqRange")
      {
         // Compute bias
         ComputeMeasurementBias("Bias", "DSN_SeqRange", 2);
         // Compute noise sigma
         ComputeMeasurementNoiseSigma("NoiseSigma", "DSN_SeqRange", 2);
         // Compute measurement error covariance matrix
         ComputeMeasurementErrorCovarianceMatrix();

         // Calculate measurement covariance
         cMeasurement.covariance = &measErrorCovariance;

         // if need range value only, skip this section, otherwise add noise and bias as possible
         if (!rangeOnly)
         {
            // Add noise to measurement value
            if (addNoise)
            {
               // Add noise here
               if (cMeasurement.unfeasibleReason != "R")
               {
                  RandomNumber* rn = RandomNumber::Instance();
                  Real val = rn->Gaussian(cMeasurement.value[i], noiseSigma[i]);
                  Real noiseVal = val - cMeasurement.value[i];
                  cMeasurement.value[i] = val;
                  cMeasurement.correction[i] = cMeasurement.correction[i] + noiseVal;
               }
            }

            //Add bias to measurement value only after noise had been added in order to avoid adding bias' noise 
            if (addBias)
            {
               #ifdef DEBUG_RANGE_CALCULATION
                  MessageInterface::ShowMessage("      . Add bias...\n");
               #endif
               if (biasType == BIASTYPE_IS_PASSBIAS) //(timeGapForPassBreak > 0.0)
               {
                  // add pass bias
                  // specify pass number associate to the current observation
                  Real passBias = measErrorModel->GetPassBias(cMeasurement.epochGT);
                  cMeasurement.value[i] = cMeasurement.value[i] + passBias;                            // pass bias unit: RU
                  cMeasurement.correction[i] = cMeasurement.correction[i] + passBias;                  // pass bias unit: RU
               }
               else
               {
                  // add bias
                  cMeasurement.value[i] = cMeasurement.value[i] + measurementBias[i];                  // bias unit: RU
                  cMeasurement.correction[i] = cMeasurement.correction[i] + measurementBias[i];        // bias unit: RU
               }
            }
         }
      }

      cMeasurement.uplinkFreq = uplinkFreq*1.0e6;                       // convert Mhz to Hz due cMeasurement.uplinkFreq's unit is Hz
      cMeasurement.uplinkFreqAtRecei = uplinkFreqAtRecei*1.0e6;         // convert Mhz to Hz due cMeasurement.uplinkFreqAtRecei's unit is Hz
      cMeasurement.uplinkBand = freqBand;
      cMeasurement.rangeModulo = rangeModulo;

      // Update media corrections
      cMeasurement.ionoCorrectValue = multiplier*GetIonoCorrection()/lightspeed;                        // convert Km to RU
      cMeasurement.tropoCorrectValue = multiplier*GetTropoCorrection()/lightspeed;                      // convert Km to RU

      #ifdef DEBUG_RANGE_CALCULATION
         MessageInterface::ShowMessage("===================================================================\n");
         MessageInterface::ShowMessage("====  DSNRangeAdapter: Range Calculation for Measurement Data %dth  \n", i);
         MessageInterface::ShowMessage("===================================================================\n");
         
         MessageInterface::ShowMessage("      . Path : ");
         for (UnsignedInt k=0; k < participantLists[i]->size(); ++k)
            MessageInterface::ShowMessage("%s,  ", participantLists[i]->at(k).c_str());
         MessageInterface::ShowMessage("\n");

         MessageInterface::ShowMessage("      . Measurement epoch          : %.12lf\n", cMeasurement.epochGT.GetMjd());
         MessageInterface::ShowMessage("      . Measurement type           : <%s>\n", measurementType.c_str());
         MessageInterface::ShowMessage("      . Noise adding option        : %s\n", (addNoise?"true":"false"));
         MessageInterface::ShowMessage("      . Range modulo constant      : %.12lf RU\n", rangeModulo);
         MessageInterface::ShowMessage("      . Real travel time           : %.12lf seconds\n", realTravelTime);
         MessageInterface::ShowMessage("      . Uplink frequency at transmit time: %.12lf Hz\n", uplinkFreq);
         MessageInterface::ShowMessage("      . Uplink frequency at receive time : %.12lf Hz\n", uplinkFreqAtRecei);
         MessageInterface::ShowMessage("      . Multiplier factor          : %.12lf\n", GetMultiplierFactor());
         MessageInterface::ShowMessage("      . C-value w/o noise and bias : %.12lf RU\n", C_idealVal);
         MessageInterface::ShowMessage("      . DSN Noise sigma            : %.12lf RU\n", noiseSigma[i]);
         MessageInterface::ShowMessage("      . DSN Bias                   : %.12lf RU\n", measurementBias[i]);
         MessageInterface::ShowMessage("      . C-value with noise and bias: %.12lf RU\n", cMeasurement.value[i]);
         MessageInterface::ShowMessage("      . Measurement epoch A1Mjd    : %.12lf\n", cMeasurement.epoch); 
         MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible?"feasible":"unfeasible"));
         MessageInterface::ShowMessage("      . Feasibility reason         : %s\n", cMeasurement.unfeasibleReason.c_str());
         MessageInterface::ShowMessage("      . Elevation angle            : %.12lf degree\n", cMeasurement.feasibilityValue);
         MessageInterface::ShowMessage("      . Covariance matrix          : <%p>\n", cMeasurement.covariance);
         if (cMeasurement.covariance)
         {
            MessageInterface::ShowMessage("      . Covariance matrix size = %d\n", cMeasurement.covariance->GetDimension());
            MessageInterface::ShowMessage("     [ ");
            for (UnsignedInt i = 0; i < cMeasurement.covariance->GetDimension(); ++i)
            {
               if ( i > 0)
                  MessageInterface::ShowMessage("\n");
               for (UnsignedInt j = 0; j < cMeasurement.covariance->GetDimension(); ++j)
                  MessageInterface::ShowMessage("%le   ", cMeasurement.covariance->GetCovariance()->GetElement(i,j));
            }
            MessageInterface::ShowMessage("]\n");
         }

         MessageInterface::ShowMessage("===================================================================\n");
      #endif

   }

   #ifdef DEBUG_RANGE_CALCULATION
      MessageInterface::ShowMessage("DSNRangeAdapter::CalculateMeasurement(%s, "
            "<%p>, <%p>) exit\n", (withEvents ? "true" : "false"), forObservation,
            rampTable);
   #endif

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
const std::vector<RealArray>& DSNRangeAdapter::CalculateMeasurementDerivatives(
      GmatBase* obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVE_CALCULATION
      Integer parmId = GetParmIdFromEstID(id, obj);
      MessageInterface::ShowMessage("Enter DSNRangeAdapter::CalculateMeasurementDerivatives(%s, %d) called; parm ID is %d; Epoch %.12lf\n", obj->GetName().c_str(), id, parmId, cMeasurement.epoch);
   #endif

   // Get parameter name specified by id
   Integer parameterID;
   if (id > 250)
      parameterID = id - obj->GetType() * 250; //GetParmIdFromEstID(id, obj);
   else
      parameterID = id;
   std::string paramName = obj->GetParameterText(parameterID);

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Solve-for parameter: %s\n", paramName.c_str());
   #endif

   // Clear derivative variable
   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      theDataDerivatives[i].clear();
   theDataDerivatives.clear();

   if (paramName == "Bias")
   {
      // Check added for multi-SC bias estimation. Checker sees if participants in the measurement are in the bias being solved for, and returns the value if so.
      Integer checker = 0;
      std::string name = obj->GetFullName();

      // For example, if solve-for object's name is 'P0.P1_ErrorModelName'.                                         // made changes by TUAN NGUYEN
      // That means: solver-for variable Bias' is related to the signal path ('P0', 'P1', ... 'Pn').                // made changes by TUAN NGUYEN
      // Ideally, the name should be 'P0.P1_P2_...._Pn_ErrorModelName'. It should be contain the name               // made changes by TUAN NGUYEN
      // of all participants in the signal path. It is a long name. Therefore, we need to make it                   // made changes by TUAN NGUYEN
      // shorter by take the names of first 2 participants. As a result, the name is 'P0.P1_ErrorModelName'.        // made changes by TUAN NGUYEN

      // We need to take derivative the measurement value of that path w.r.t. 'Bias' parameter in                   // made changes by TUAN NGUYEN
      // 'ErrorModelName' object. Before taking derivative, it needs to check participant[0] to be 'P0' and         // made changes by TUAN NGUYEN
      // participant[1] = 'P1' and so on, then calculate derivative. If its is not matched set 0 to derivative      // made changes by TUAN NGUYEN
      // because the measurement value of that signal path is independent of the solver-for bias.                   // made changes by TUAN NGUYEN

      StringArray *nameList = participantLists[0];
      for (Integer indx = 0; indx < nameList->size(); indx++)
      {
         size_t pos = name.find(nameList->at(indx));                                                                // made changes by TUAN NGUYEN
         //if (name.rfind(nameList->at(indx)) != std::string::npos)                                                 // made changes by TUAN NGUYEN
         if (pos != std::string::npos)
         {
            //std::string fullCheck = name.substr(name.rfind(nameList->at(indx)) + nameList->at(indx).length(),1);  // made changes by TUAN NGUYEN
            std::string fullCheck = name.substr(pos + nameList->at(indx).length(), 1);                              // made changes by TUAN NGUYEN
            if (fullCheck == "_" || fullCheck == ".")
            {
               // name.erase(name.rfind(nameList->at(indx)), nameList->at(indx).length());                          // made changes by TUAN NGUYEN
               name.erase(pos, nameList->at(indx).length() + 1);     // It needs to remove "_" or "." as well       // made changes by TUAN NGUYEN
               checker++;
            }
         }
      }

      if (((ErrorModel*)obj)->GetStringParameter("Type") == "DSN_SeqRange" && checker > 1)
         theDataDerivatives = calcData->CalculateMeasurementDerivatives(obj, id);
      else
      {
         Integer size = obj->GetEstimationParameterSize(id);
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);
      }

   }
   else if (paramName == "PassBiases")
   {
      if (((ErrorModel*)obj)->GetStringParameter("Type") == "DSN_SeqRange")
      {
         theDataDerivatives = calcData->CalculateMeasurementDerivatives(obj, id, &cMeasurement.epochGT);
      }
      else
      {
         Integer size = obj->GetEstimationParameterSize(id);
         RealArray oneRow;
         oneRow.assign(size, 0.0);
         theDataDerivatives.push_back(oneRow);
      }
   }
   else
   {
      // Compute measurement derivatives w.r.t position and velocity in km:
      RangeAdapterKm::CalculateMeasurementDerivatives(obj, id);

      // Convert measurement derivatives from km to RU
      Real freqFactor = multiplier/(GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM * GmatMathConstants::M_TO_KM);
      for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      {
         for (UnsignedInt j = 0; j < theDataDerivatives[i].size(); ++j)
         {
            if ((paramName == "Position")||(paramName == "Velocity")||(paramName == "CartesianX"))
               theDataDerivatives[i][j] = theDataDerivatives[i][j] * freqFactor;
         }
      }
   }
   #ifdef DEBUG_DERIVATIVE_CALCULATION
      for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      {
         MessageInterface::ShowMessage("Derivative for path %dth:\n", i);
         MessageInterface::ShowMessage("[");
         for (UnsignedInt j = 0; j < theDataDerivatives[i].size(); ++j)
         {
            MessageInterface::ShowMessage("    %.12lf", theDataDerivatives[i][j]);
            MessageInterface::ShowMessage("%s", ((j == theDataDerivatives[i].size()-1)?"":","));
         }
         MessageInterface::ShowMessage("]\n");
      }
      
   #endif

   #ifdef DEBUG_DERIVATIVE_CALCULATION
      MessageInterface::ShowMessage("Exit DSNRangeAdapter::CalculateMeasurementDerivatives():\n");
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
bool DSNRangeAdapter::WriteMeasurements()
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
bool DSNRangeAdapter::WriteMeasurement(Integer id)
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
Integer DSNRangeAdapter::HasParameterCovariances(Integer parameterId)
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
Integer DSNRangeAdapter::GetEventCount()
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
void DSNRangeAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   RangeAdapterKm::SetCorrection(correctionName, correctionType);
}





//------------------------------------------------------------------------------
// Real GetFrequencyFactor(Real frequency)
//------------------------------------------------------------------------------
/**
 * Constructs the multiplier used to convert into range units
 *
 * @param frequency F_T used in the computation.  The default (0.0) generates
 *                  the factor for DSN14.
 *
 * @return The factor used in the conversion.
 */
//------------------------------------------------------------------------------
Real DSNRangeAdapter::GetFrequencyFactor(Real frequency)
{
   Real factor;

   if ((obsData == NULL)&&(rampTB == NULL))
   {
      // Map the frequency to the corresponding factor here
      if ((frequency >= 2000000000.0) && (frequency <= 4000000000.0))
      {
         // S-band
         factor = frequency / 2.0;
         if (freqBand == 0)
            freqBand = 1;               // 1 for S-band
      } //These values roughly match the JPL DSN specifications for bands. These may need to be updated slightly depending on use
      else if ((frequency >= 7000000000.0) && (frequency <= 8400000000.0))
      {
         // X-band 
         // factor = frequency * 11.0 / 75.0;            // for X-band with BVE and HEF attenna mounted before BVE:    Moyer's eq 13-109
         factor = frequency * 221.0 / 1498.0;            // for X-band with BVE:   Moyer's eq 13-110
         if (freqBand == 0)
            freqBand = 2;               // 2 for X-band
      }
      else
      {
         std::stringstream ss;
         ss << "Error: No frequency band was specified for frequency = " << frequency << "Hz. Only S-Band and X-Band are supported\n";
         throw MeasurementException(ss.str());
      }
      // Todo: Figure out how to detect HEV and BVE
   }
   else
   {
      switch (freqBand)
      {
      case 1:
         factor = frequency/ 2.0;
         break;
      case 2:
         factor = frequency *221.0/1498.0;
         break;
      }
   }

   return factor;
}


//------------------------------------------------------------------------------
// Real RampedFrequencyIntergration(Real t0, Real delta_t)
//------------------------------------------------------------------------------
/**
 * Calculate the integration of frequency factor in range from time t0 to time t1
 *
 * @param t1         The end time for integration (unit: A1Mjd)
 * @param delta_t    Elapse time (unit: second)
 * @param err        Error number
 *
 * @return The integration of frequency factor.
 * Assumptions: ramp table had been sorted by epoch 
 */
//------------------------------------------------------------------------------
Real DSNRangeAdapter::IntegralRampedFrequency(GmatTime t1, Real delta_t, Integer& err)
{
   // Verify ramp table and elpase time
   err = 0;
   if (delta_t < 0)
   {
      err = 1;
      errMsg = "Error: Elapse time has to be a non negative number\n";
      throw MeasurementException("Error: Elapse time has to be a non negative number\n");
   }

   if (rampTB == NULL)
   {
      err = 2;
      errMsg = "Error: No ramp table available for measurement calculation\n";
      throw MeasurementException("Error: No ramp table available for measurement calculation\n");
   }

   if ((*rampTB).size() == 0)
   {
      err = 3;
      std::stringstream ss;
      ss << "Error: Ramp table has no data records. It needs at least 1 record.\n";
      errMsg = ss.str();
      throw MeasurementException(ss.str());
   }

   // Get the beginning index and the ending index for frequency data records for this measurement model 
   BeginEndIndexesOfRampTable(err);

   //Real t0 = t1 - delta_t / GmatTimeConstants::SECS_PER_DAY;
   GmatTime t0 = t1;
   t0.SubtractSeconds(delta_t);

   GmatTime time_min = (*rampTB)[beginIndex].epochGT;

   if (t1 < time_min)
   {
      // Convert t1 and time_min from A1Mjd to TAIMjd
      GmatTime t1TAI, tminTAI;
      std::string tais;
      GmatTime a1Time = t1;
      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", t1TAI, tais);
      a1Time = time_min;
      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", tminTAI, tais);

      // Generate error message
      char s[200];
      sprintf(&s[0], "Error: End epoch t3R = %s is out of range [%s, +Inf) of ramp table\n", t1TAI.ToString().c_str(), tminTAI.ToString().c_str());
      std::string st(&s[0]);
      err = 4;
      errMsg = st;
      throw MeasurementException(st);
   }

   if (t0 < time_min)
   {
      // Convert t0 and time_min from A1Mjd to TAIMjd
      GmatTime t0TAI, tminTAI;
      std::string tais;
      GmatTime a1Time = t0;
      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", t0TAI, tais);
      a1Time = time_min;
      theTimeConverter->Convert("A1ModJulian", a1Time, "", "TAIModJulian", tminTAI, tais);

      // Generate error message
      char s[200];
      sprintf(&s[0], "Error: Start epoch t1T = %s is out of range [%s, +Inf) of ramp table\n", t0TAI.ToString().c_str(), tminTAI.ToString().c_str());
      std::string st(&s[0]);
      err = 5;
      errMsg = st;
      throw MeasurementException(st);
   }


   // Search for end interval:
   UnsignedInt end_interval = beginIndex;
   for (UnsignedInt i = beginIndex; i < endIndex; ++i)
   {
      if (t1 >= (*rampTB)[i].epochGT)
         end_interval = i;
      else
         break;
   }

   // Integration of the frequency from t0 to t1:
   Real f0, f1, f_dot;
   Real value1;
   Real interval_len;

   Real basedFreq = (*rampTB)[end_interval].rampFrequency;
   Real basedFreqFactor = GetFrequencyFactor(basedFreq);
   Real value = 0.0;
   Real dt = delta_t;
   for (Integer i = end_interval; dt > 0; --i)
   {
      // Specify frequency at the begining and lenght of the current interval   
      if (i == end_interval)
         interval_len = (t1 - (*rampTB)[i].epochGT).GetTimeInSec();                         // unit:sec
      else
         interval_len = ((*rampTB)[i + 1].epochGT - (*rampTB)[i].epochGT).GetTimeInSec();   // unit: sec

      f0 = (*rampTB)[i].rampFrequency;                              // unit: Hz
      f_dot = (*rampTB)[i].rampRate;                                // unit: Hz/second
      if (dt < interval_len)
      {
         f0 = (*rampTB)[i].rampFrequency + f_dot*(interval_len - dt);
         interval_len = dt;
      }

      // Specify frequency at the end of the current interval
      f1 = f0 + f_dot*interval_len;

      // Take integral for the current interval
      value1 = ((GetFrequencyFactor(f0) + GetFrequencyFactor(f1)) / 2 - basedFreqFactor) * interval_len;
      value = value + value1;

      // Specify dt 
      dt = dt - interval_len;
   }
   value = value + basedFreqFactor*delta_t;

   return value;
}

