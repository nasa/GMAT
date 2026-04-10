//------------------------------------------------------------------------------
//                           GNRangeAdapter
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
// Author: Tuan D. Nguyen, GSFC/NASA.
// Created: Jan 12, 2017
/**
 * A measurement adapter for GN range measurement
 */
//------------------------------------------------------------------------------

#include "GNRangeAdapter.hpp"
#include "RandomNumber.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "SignalBase.hpp"
#include "ErrorModel.hpp"
#include "BodyFixedPoint.hpp"
#include "SpaceObject.hpp"
#include "PropSetup.hpp"
#include "ODEModel.hpp"
#include "Propagator.hpp"
#include <sstream>


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_RANGE_CALCULATION


//------------------------------------------------------------------------------
// GNRangeAdapter(const std::string& name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
GNRangeAdapter::GNRangeAdapter(const std::string& name) :
   RangeAdapterKm(name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNRangeAdapter default constructor <%p>\n", this);
#endif

   typeName = "Range";           // change measurement type from "RangeKm" to "Range" for GN Range 
}


//------------------------------------------------------------------------------
// ~GNRangeAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GNRangeAdapter::~GNRangeAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNRangeAdapter default destructor  <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// GNRangeAdapter(const GNRangeAdapter& rak)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rak The adapter copied to make this one
 */
//------------------------------------------------------------------------------
GNRangeAdapter::GNRangeAdapter(const GNRangeAdapter& rak) :
   RangeAdapterKm      (rak)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNRangeAdapter copy constructor   from <%p> to <%p>\n", &rak, this);
#endif

}


//------------------------------------------------------------------------------
// GNRangeAdapter& operator=(const GNRangeAdapter& rak)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rak The adapter copied to make this one match it
 *
 * @return This adapter made to look like rak
 */
//------------------------------------------------------------------------------
GNRangeAdapter& GNRangeAdapter::operator=(const GNRangeAdapter& rak)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNRangeAdapter operator =   set <%p> = <%p>\n", this, &rak);
#endif

   if (this != &rak)
   {
      TrackingDataAdapter::operator=(rak);
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
GmatBase* GNRangeAdapter::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GNRangeAdapter::Clone() clone this <%p>\n", this);
#endif

   return new GNRangeAdapter(*this);
}


const MeasurementData& GNRangeAdapter::CalculateMeasurement(bool withEvents,
   ObservationData* forObservation, std::vector<RampTableData>* rampTB,
   bool forSimulation)
{
#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("GNRangeAdapter::CalculateMeasurement(%s, "
      "<%p>, <%p>) called\n", (withEvents ? "true" : "false"), forObservation,
      rampTB);
#endif
   
   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
      instanceName + " before the measurement was set");
   
   std::vector<SignalBase*> paths = calcData->GetSignalPaths();

   // This signal leg  object is the first one in signal path. 
   SignalBase* firstleg = paths[0];
   // Get last signal leg
   SignalBase* lastleg = firstleg;
   while (lastleg->GetNext() != NULL)
      lastleg = lastleg->GetNext();
   if (firstleg->GetSignalDataObject()->tNode->IsOfType(Gmat::SPACECRAFT) && lastleg->GetSignalDataObject()->rNode->IsOfType(Gmat::SPACECRAFT) && this->cMeasurement.typeName == "Range_Skin")
      throw MeasurementException("The measurement type " + this->cMeasurement.typeName + " cannot be used for inter-spacecraft measurements.");


   bool epochValid = true;

   GmatTime computeEpoch;
   if (forObservation)
	   computeEpoch = forObservation->epochGT;
   else
   {
	   for (UnsignedInt i = 0; i < refObjects.size(); ++i)
	   {
		   if (refObjects[i]->IsOfType(Gmat::SPACEOBJECT))
		   {
			   computeEpoch = ((SpaceObject*)refObjects[i])->GetEpochGT();
			   break;
		   }
	   }
   }


   if (calcData->CheckEpochValidity(computeEpoch))
   {
	   // Fire the measurement model to build the collection of signal data
	   if (calcData->CalculateMeasurement(withLighttime, withMediaCorrection, forObservation, rampTB, forSimulation))
	   {
		   // QA Media correction:
		   cMeasurement.isIonoCorrectWarning = false;
		   cMeasurement.ionoCorrectRawValue = 0.0;
		   cMeasurement.ionoCorrectValue = 0.0;
		   cMeasurement.isTropoCorrectWarning = false;
		   cMeasurement.tropoCorrectRawValue = 0.0;
		   cMeasurement.tropoCorrectValue = 0.0;

		   if (withMediaCorrection)
		   {
			   Real correction = GetIonoCorrection();                                  // unit: km

			   // Set a warning to measurement data when ionosphere correction is outside of range [0 km , 0.04 km]
			   cMeasurement.isIonoCorrectWarning = (correction < 0.0) || (correction > 0.04);
			   cMeasurement.ionoCorrectRawValue = correction;                   // unit: km

			   correction = GetTropoCorrection();                                      // unit: km

			   // Set a warning to measurement data when troposphere correction is outside of range [0 km , 0.12 km]
			   cMeasurement.isTropoCorrectWarning = (correction < 0.0) || (correction > 0.12);
			   cMeasurement.tropoCorrectRawValue = correction;                  // unit: km
		   }

		   std::vector<SignalBase*> paths = calcData->GetSignalPaths();
		   std::string unfeasibilityReason;
		   Real        unfeasibilityValue;

		   // set to default
		   cMeasurement.isFeasible = false;
		   cMeasurement.unfeasibleReason = "";
		   cMeasurement.feasibilityValue = 90.0;

		   GmatTime transmitEpoch, receiveEpoch;
		   RealArray values, corrections;
		   cMeasurement.rangeVecs.clear();

		   cMeasurement.tBodies.clear();
		   cMeasurement.rBodies.clear();
		   cMeasurement.tCSOrigins.clear();
		   cMeasurement.rCSOrigins.clear();

		   cMeasurement.tPrecTimes.clear();
		   cMeasurement.rPrecTimes.clear();
		   cMeasurement.tLocs.clear();
		   cMeasurement.rLocs.clear();
		   for (UnsignedInt i = 0; i < paths.size(); ++i)           // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths 
		   {
			   // Calculate C-value for signal path ith:
			   values.push_back(0.0);
			   corrections.push_back(0.0);
			   SignalBase *currentleg = paths[i];
			   SignalData *current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));
			   SignalData *first = current;
			   UnsignedInt legIndex = 0;

			   while (currentleg != NULL)
			   {
				   ++legIndex;
				   // Set feasibility value
				   if (current->feasibilityReason[0] == 'N')
				   {
					   if ((cMeasurement.unfeasibleReason == ""))
					   {
						   cMeasurement.isFeasible = true;
						   cMeasurement.unfeasibleReason = "N";
						   cMeasurement.feasibilityValue = current->feasibilityValue;
					   }
				   }
				   else if (current->feasibilityReason[0] == 'B')
				   {
					   std::stringstream ss;
					   ss << "B" << legIndex << current->feasibilityReason.substr(1);
					   current->feasibilityReason = ss.str();
					   if ((cMeasurement.unfeasibleReason == "") || (cMeasurement.unfeasibleReason == "N"))
					   {
						   cMeasurement.unfeasibleReason = current->feasibilityReason;
						   cMeasurement.isFeasible = false;
						   cMeasurement.feasibilityValue = current->feasibilityValue;
					   }
				   }
               else if (current->feasibilityReason[0] == 'H')
               {
                  std::stringstream ss;
                  ss << "HORP";
                  current->feasibilityReason = ss.str();
                  if ((cMeasurement.unfeasibleReason == "") || (cMeasurement.unfeasibleReason == "N"))
                  {
                     cMeasurement.unfeasibleReason = current->feasibilityReason;
                     cMeasurement.isFeasible = false;
                     cMeasurement.feasibilityValue = current->feasibilityValue;
                  }
               }

				   // Get leg participants

				   SpacePoint* body;
				   SpacePoint* csOrigin;
				   BodyFixedPoint *bf;
				   CoordinateSystem *cs;
				   SpaceObject* spObj;

				   if (current->tNode->IsOfType(Gmat::GROUND_STATION))
				   {
					   bf = (BodyFixedPoint*)current->tNode;
					   cs = bf->GetBodyFixedCoordinateSystem();
					   body = cs->GetOrigin();
					   csOrigin = body;
				   }
				   else
				   {
					   if (current->tPropagator->GetPropagator()->UsesODEModel())
						   body = current->tPropagator->GetODEModel()->GetForceOrigin();
					   else
						   body = current->tPropagator->GetPropagator()->GetPropOrigin();
					   csOrigin = ((SpaceObject*)current->tNode)->GetOrigin();
				   }
				   cMeasurement.tBodies.push_back((CelestialBody*)body);
				   cMeasurement.tCSOrigins.push_back((CelestialBody*)csOrigin);

				   if (current->rNode->IsOfType(Gmat::GROUND_STATION))
				   {
					   bf = (BodyFixedPoint*)current->rNode;
					   cs = bf->GetBodyFixedCoordinateSystem();
					   body = cs->GetOrigin();
					   csOrigin = body;
				   }
				   else
				   {
					   if (current->rPropagator->GetPropagator()->UsesODEModel())
						   body = current->rPropagator->GetODEModel()->GetForceOrigin();
					   else
						   body = current->rPropagator->GetPropagator()->GetPropOrigin();
					   csOrigin = ((SpaceObject*)current->rNode)->GetOrigin();
				   }
				   cMeasurement.rBodies.push_back(body);
				   cMeasurement.rCSOrigins.push_back(csOrigin);

				   cMeasurement.tPrecTimes.push_back(current->tPrecTime);
				   cMeasurement.rPrecTimes.push_back(current->rPrecTime);
				   cMeasurement.tLocs.push_back(new Rvector3(current->tLoc));
				   cMeasurement.rLocs.push_back(new Rvector3(current->rLoc));

				   // accumulate all light time range for signal path ith 
				   Rvector3 signalVec = current->rangeVecInertial;
				   cMeasurement.rangeVecs.push_back(new Rvector3(signalVec));
				   values[i] += signalVec.GetMagnitude();

				   // accumulate all range corrections for signal path ith
				   for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
				   {
					   if (current->useCorrection[j] && current->correctionTypes[j] == "Range")
					   {
						   values[i] += current->corrections[j];
						   corrections[i] += current->corrections[j];
					   }
				   }// for j loop

				   // accumulate all hardware delays for signal path ith
				   values[i] += ((current->tDelay + current->rDelay)*
					   GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);
				   corrections[i] += ((current->tDelay + current->rDelay)*
					   GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);

				   // Get measurement epoch in the first signal path. It will apply for all other paths
				   if (i == 0)
				   {
					   transmitEpoch = first->tPrecTime - first->tDelay / GmatTimeConstants::SECS_PER_DAY;
					   receiveEpoch = current->rPrecTime + current->rDelay / GmatTimeConstants::SECS_PER_DAY;
					   if (calcData->GetTimeTagFlag())
					   {
						   // Measurement epoch will be at the end of signal path when time tag is at the receiver
						   if (current->next == NULL)
						   {
							   cMeasurement.epochGT = receiveEpoch;
							   cMeasurement.epoch = receiveEpoch.GetMjd();
						   }
					   }
					   else
					   {
						   // Measurement epoch will be at the begin of signal path when time tag is at the transmiter
						   cMeasurement.epochGT = transmitEpoch;
						   cMeasurement.epoch = transmitEpoch.GetMjd();
					   }
				   }

               cMeasurement.horpHeight = GmatMathUtil::Min(cMeasurement.horpHeight, current->horpHeight);
               cMeasurement.horpAngle = GmatMathUtil::Max(cMeasurement.horpAngle, current->horpAngle);

				   currentleg = currentleg->GetNext();
				   current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));

			   }// while loop

		   }// for i loop

		   // Caluclate uplink frequency at received time and transmit time
		   cMeasurement.uplinkFreq = calcData->GetUplinkFrequency(0, rampTB) * 1.0e6;                        // unit: Hz
		   cMeasurement.uplinkFreqAtRecei = calcData->GetUplinkFrequencyAtReceivedEpoch(0, rampTB) * 1.0e6;   // unit: Hz
		   cMeasurement.uplinkBand = calcData->GetUplinkFrequencyBand(0, rampTB);

		   // Compute bias and noise for measurements
		   ComputeBiasAndNoise(measurementType, 2);

		   // Set measurement values
		   cMeasurement.value.clear();
		   cMeasurement.correction.clear();
		   for (UnsignedInt i = 0; i < values.size(); ++i)
		   {
			   cMeasurement.value.push_back(0.0);
			   cMeasurement.correction.push_back(0.0);
		   }

		   // Apply bias and noise to computed measurements
		   ApplyBiasAndNoise(measurementType, corrections, values);

		   // Calculate measurement covariance
		   cMeasurement.covariance = &measErrorCovariance;


#ifdef DEBUG_ADAPTER_EXECUTION
		   MessageInterface::ShowMessage("Computed measurement\n   Type:  %d\n   "
			   "Type:  %s\n   UID:   %d\n   Epoch:%.12lf\n   Participants:\n",
			   cMeasurement.type, cMeasurement.typeName.c_str(),
			   cMeasurement.uniqueID, cMeasurement.epoch);
		   for (UnsignedInt k = 0; k < cMeasurement.participantIDs.size(); ++k)
			   MessageInterface::ShowMessage("      %s\n",
				   cMeasurement.participantIDs[k].c_str());
		   MessageInterface::ShowMessage("   Values:\n");
		   for (UnsignedInt k = 0; k < cMeasurement.value.size(); ++k)
			   MessageInterface::ShowMessage("      %.12lf\n",
				   cMeasurement.value[k]);

		   MessageInterface::ShowMessage("   Valid: %s\n",
			   (cMeasurement.isFeasible ? "true" : "false"));
#endif
	   }
	   if (!calcData->CheckEpochValidity(computeEpoch))
	   {
		   epochValid = false;
	   }
   }
   else // The requested epoch is not supported - likely not on a TDRS ephem
   {
   epochValid = false;
   }

   if (!epochValid)
   {
	   cMeasurement.isFeasible = false;
	   cMeasurement.unfeasibleReason = "EGAP";
   }


#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("GNRangeAdapter::CalculateMeasurement(%s, "
      "<%p>, <%p>) exit\n", (withEvents ? "true" : "false"), forObservation,
      rampTB);
#endif

   return cMeasurement;
}

//------------------------------------------------------------------------------
// void ApplyBiasAndNoise(const std::string measType, 
//                        const RealArray corrections, const RealArray values)
//------------------------------------------------------------------------------
/**
 * Applies computed bias and noise to measurements and handles range based multiplier
 *
 * @param useMeasType the measurement type to apply the corrections
 * @param corrections the corrections
 * @param values the measurment values 
 */
 //------------------------------------------------------------------------------
void GNRangeAdapter::ApplyBiasAndNoise(const std::string& useMeasType, 
   const RealArray& corrections, const RealArray& values)
{
  for (UnsignedInt i = 0; i < values.size(); ++i)
  {
     Real measVal = values[i];
     Real corrVal = corrections[i];
#ifdef DEBUG_RANGE_CALCULATION
     MessageInterface::ShowMessage("===================================================================\n");
     MessageInterface::ShowMessage("====  GNRangeAdapter (%s): Range Calculation for Measurement Data %dth  \n", GetName().c_str(), i);
     MessageInterface::ShowMessage("===================================================================\n");
  
     MessageInterface::ShowMessage("      . Path : ");
     for (UnsignedInt k = 0; k < participantLists[i]->size(); ++k)
        MessageInterface::ShowMessage("%s,  ", participantLists[i]->at(k).c_str());
     MessageInterface::ShowMessage("\n");
  
     MessageInterface::ShowMessage("      . Measurement epoch          : %.12lf\n", cMeasurement.epochGT.GetMjd());
     MessageInterface::ShowMessage("      . Measurement type           : <%s>\n", measurementType.c_str());
     MessageInterface::ShowMessage("      . C-value w/o noise and bias : %.12lf km \n", values[i]);
     MessageInterface::ShowMessage("      . Range only option          : %s\n", (rangeOnly ? "true" : "false"));
     MessageInterface::ShowMessage("      . Noise adding option        : %s\n", (addNoise ? "true" : "false"));
     MessageInterface::ShowMessage("      . Bias adding option        : %s\n", (addBias ? "true" : "false"));
     if (useMeasType == "Range")
     {
        MessageInterface::ShowMessage("      . Range noise sigma          : %.12lf km \n", noiseSigma[i]);
        MessageInterface::ShowMessage("      . Range bias                 : %.12lf km \n", measurementBias[i]);
        MessageInterface::ShowMessage("      . Multiplier                 : %.12lf \n", multiplier);
     }
#endif
  
     // This section is only done when measurement type is "Range". For other types such as DSN_SeqRange or DSN_TCP, it will be done in their adapters  
     if (useMeasType == "Range")
     {
        // Apply multiplier for ("Range_KM") "Range" measurement model. This step has to
        // be done before adding bias and noise
        measVal = measVal * multiplier;
        corrVal = corrVal * multiplier;
  
        // if need range value only, skip this section, otherwise add noise and bias as possible
        // Note: for Doppler measurement for E and S paths, we only need range value only and no noise and bias are added to measurement value. 
        if (!rangeOnly)
        {
           // Add noise to measurement value
           if (addNoise)
           {
              // Add noise here
              RandomNumber* rn = RandomNumber::Instance();
              Real val = rn->Gaussian(0.0, noiseSigma[i]);                  // noise sigma unit: Km
              measVal += val;
              corrVal += val;
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
                 corrVal = corrVal + passBias;                                    // pass bias unit: Km

              }
              else
              {
                 // add bias
                 measVal = measVal + measurementBias[i];                          // bias unit: Km
                 corrVal = corrVal + measurementBias[i];                          // bias unit: Km
              }
           }
        }
     }
     cMeasurement.value[i] = measVal;
     cMeasurement.correction[i] = corrVal;
  
     // Update media corrections
     cMeasurement.ionoCorrectValue = cMeasurement.ionoCorrectRawValue;
     cMeasurement.tropoCorrectValue = cMeasurement.tropoCorrectRawValue;
  
#ifdef DEBUG_RANGE_CALCULATION
     MessageInterface::ShowMessage("      . C-value with noise and bias : %.12lf km\n", cMeasurement.value[i]);
     MessageInterface::ShowMessage("      . Measurement epoch A1Mjd     : %.12lf\n", cMeasurement.epoch);
     MessageInterface::ShowMessage("      . Transmit frequency at receive epoch  : %.12le Hz\n", cMeasurement.uplinkFreqAtRecei);
     MessageInterface::ShowMessage("      . Transmit frequency at transmit epoch : %.12le Hz\n", cMeasurement.uplinkFreq);
     MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible ? "feasible" : "unfeasible"));
     MessageInterface::ShowMessage("      . Feasibility reason          : %s\n", cMeasurement.unfeasibleReason.c_str());
     MessageInterface::ShowMessage("      . Elevation angle             : %.12lf degree\n", cMeasurement.feasibilityValue);
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

}

//------------------------------------------------------------------------------
// void ComputeBiasAndNoise(std::string measType, Integer numTrip)
//------------------------------------------------------------------------------
/**
 * Computes bias and noise for applied measurements
 *
 * @param measType the measurement type to apply the corrections
 * @param numTrip number of signal trips 
 */
 //------------------------------------------------------------------------------
void GNRangeAdapter::ComputeBiasAndNoise(const std::string& useMeasType, const Integer numTrip)
{
  if (useMeasType == "Range")
  {
     // @todo: it needs to specify number of trips instead of using 2
     ComputeMeasurementBias("Bias", useMeasType, numTrip);
     ComputeMeasurementNoiseSigma("NoiseSigma", useMeasType, numTrip);
     ComputeMeasurementErrorCovarianceMatrix();
  }
}
