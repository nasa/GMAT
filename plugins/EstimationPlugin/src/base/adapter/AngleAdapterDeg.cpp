//------------------------------------------------------------------------------
//                           AngleAdapterDeg
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
// Author: 
// Created: Aug 31, 2018
/**
* A measurement adapter for angles in degrees
*/
//------------------------------------------------------------------------------

#include "AngleAdapterDeg.hpp"
#include "RandomNumber.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "SignalBase.hpp"
#include "ErrorModel.hpp"
#include "BodyFixedPoint.hpp"
#include "Spacecraft.hpp"
#include "PropSetup.hpp"
#include "ODEModel.hpp"
#include "Propagator.hpp"
#include "GroundstationInterface.hpp"
#include "StringUtil.hpp"
#include <sstream>


//#define DEBUG_ADAPTER_EXECUTION
//#define DEBUG_ADAPTER_DERIVATIVES
//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_ANGLE_CALCULATION
//#define DEBUG_ANGLE_BIAS_DERIVATIVE
//#define DEBUG_ANGLE_IONOSPHERE_CORRECTION
//------------------------------------------------------------------------------
// AngleAdapterDeg(const std::string& name)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param name The name of the adapter
*/
//------------------------------------------------------------------------------
AngleAdapterDeg::AngleAdapterDeg(const std::string& name) :
   TrackingDataAdapter("Angle", name),                        //TrackingDataAdapter("AngleDeg", name)
   ocs(NULL),
   j2k(NULL),
   useAnnual(false),
   useDiurnal(false)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("AngleAdapterDeg constructor <%p>\n", this);
#endif
   parameterCount = AngleAdapterDegParamCount;
}


//------------------------------------------------------------------------------
// ~AngleAdapterDeg()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
AngleAdapterDeg::~AngleAdapterDeg()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("AngleAdapterDeg destructor  <%p>\n", this);
#endif
   if (ocs)
      delete ocs;

   if (j2k)
      delete j2k;
}


//------------------------------------------------------------------------------
// AngleAdapterDeg(const AngleAdapterDeg& aad)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param aad The adapter copied to make this one
*/
//------------------------------------------------------------------------------
AngleAdapterDeg::AngleAdapterDeg(const AngleAdapterDeg& aad) :
   TrackingDataAdapter   (aad),
   topoRange             (aad.topoRange),
   bfRange               (aad.bfRange),
   mj2000Range           (aad.mj2000Range),
   useAnnual             (aad.useAnnual),
   useDiurnal            (aad.useDiurnal)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("AngleAdapterDeg copy constructor   from <%p> to <%p>\n", &aad, this);
#endif
   if (ocs)
   {
      delete ocs;
      ocs = NULL;
   }
   if (j2k)
   {
      delete j2k;
      j2k = NULL;
   }
}


//------------------------------------------------------------------------------
// AngleAdapterDeg& operator=(const AngleAdapterDeg& aad)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param aad The adapter copied to make this one match it
*
* @return This adapter made to look like aad
*/
//------------------------------------------------------------------------------
AngleAdapterDeg& AngleAdapterDeg::operator=(const AngleAdapterDeg& aad)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("AngleAdapterDeg operator =   set <%p> = <%p>\n", this, &aad);
#endif

   if (this != &aad)
   {
      TrackingDataAdapter::operator=(aad);

      if (ocs)
      {
         delete ocs;
         ocs = NULL;
      }

      if (j2k)
      {
         delete j2k;
         j2k = NULL;
      }

      topoRange   = aad.topoRange;
      bfRange     = aad.bfRange;
      mj2000Range = aad.mj2000Range;
      useAnnual   = aad.useAnnual;
      useDiurnal  = aad.useDiurnal;
   }

   return *this;
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
std::string AngleAdapterDeg::GetParameterText(const Integer id) const
{
   if (id >= AdapterParamCount && id < AngleAdapterDegParamCount)
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
Integer AngleAdapterDeg::GetParameterID(const std::string& str) const
{
   for (Integer i = AdapterParamCount; i < AngleAdapterDegParamCount; i++)
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
Gmat::ParameterType AngleAdapterDeg::GetParameterType(const Integer id) const
{
   if (id >= AdapterParamCount && id < AngleAdapterDegParamCount)
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
std::string AngleAdapterDeg::GetParameterTypeString(const Integer id) const
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
bool AngleAdapterDeg::RenameRefObject(const UnsignedInt type,
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
bool AngleAdapterDeg::SetMeasurement(MeasureModel* meas)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("AngleAdapterDeg<%p>::SetMeasurement(meas = <%p,%s>)\n",
      this, meas, meas->GetName().c_str());
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
bool AngleAdapterDeg::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start Initializing an AngleAdapterDeg <%p>\n", this);
#endif

   bool retval = false;

   if (TrackingDataAdapter::Initialize())
   {
      retval = true;

      if (participantLists.size() > 1)
         MessageInterface::ShowMessage("Warning: .gmd files do not support "
         "multiple strands\n");

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
            cMeasurement.participantIDs.push_back(theId);
         }
      }
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("End Initializing an AngleAdapterDeg <%p>\n", this);
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
* @param forSimulation       Flag indicating function is being called from the
*                            simulator
*
* @return The computed measurement data
*/
//------------------------------------------------------------------------------
const MeasurementData& AngleAdapterDeg::CalculateMeasurement(bool withEvents,
   ObservationData* forObservation, std::vector<RampTableData>* rampTB,
   bool forSimulation)
{
#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("AngleAdapterDeg::CalculateMeasurement(%s, "
      "<%p>, <%p>, %s) called\n", (withEvents ? "true" : "false"), forObservation,
      rampTB, (forSimulation ? "true" : "false"));
#endif

   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
         instanceName + " before the measurement was set");

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
         Real correction = GetIonoCorrection();                        // unit: degree
         // Set a warning to measurement data when ionosphere correction is outside of range []  ???
         cMeasurement.isIonoCorrectWarning = false;
         cMeasurement.ionoCorrectRawValue = correction;                // unit: degree

         correction = GetTropoCorrection();                            // unit: degree
         // Set a warning to measurement data when troposphere correction is outside of range [0 deg, 0.5 deg]
         //cMeasurement.isTropoCorrectWarning = (correction < 0.0) || (correction > 0.5 * GmatMathConstants::RAD_PER_DEG);   cMeasurement.isTropoCorrectWarning = (correction < 0.0) || (correction > 0.5 * GmatMathConstants::RAD_PER_DEG);
         cMeasurement.isTropoCorrectWarning = (correction < 0.0) || (correction > 0.5);
         cMeasurement.tropoCorrectRawValue = correction;               // unit: degree
      }
#ifdef DEBUG_ADAPTER_EXECUTION
      MessageInterface::ShowMessage("isIonoCorrectWarning = %c, ionoCorrectRawValue = %.14lg degree\n",
         cMeasurement.isIonoCorrectWarning ? 't' : 'f', cMeasurement.ionoCorrectRawValue);
      MessageInterface::ShowMessage("isTropoCorrectWarning = %c, tropoCorrectRawValue = %.14lg degree\n",
         cMeasurement.isTropoCorrectWarning ? 't' : 'f', cMeasurement.tropoCorrectRawValue);
#endif

      std::vector<SignalBase*> paths = calcData->GetSignalPaths();

      // This signal leg  object is the first one in signal path. 
      SignalBase* firstleg = paths[0];
      // Get last signal leg
      SignalBase* lastleg = firstleg;
      while (lastleg->GetNext() != NULL)
         lastleg = lastleg->GetNext();
      if(firstleg->GetSignalDataObject()->tNode->IsOfType(Gmat::SPACECRAFT) && lastleg->GetSignalDataObject()->rNode->IsOfType(Gmat::SPACECRAFT))
         throw MeasurementException("The measurement type " + this->cMeasurement.typeName + " cannot be used for inter-spacecraft measurements.");

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
            current = currentleg->GetSignalDataObject();

            // Set feasibility value
            if (current->feasibilityReason[0] == 'N')
            {
               if ((current->stationParticipant) && (cMeasurement.unfeasibleReason == ""))
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
            cMeasurement.rCSOrigins.push_back((CelestialBody*)csOrigin);

            cMeasurement.tPrecTimes.push_back(current->tPrecTime);
            cMeasurement.rPrecTimes.push_back(current->rPrecTime);
            cMeasurement.tLocs.push_back(new Rvector3(current->tLoc));
            cMeasurement.rLocs.push_back(new Rvector3(current->rLoc));

            // accumulate all light time range for signal path ith 
            Rvector3 signalVec = current->rangeVecInertial;
            cMeasurement.rangeVecs.push_back(new Rvector3(signalVec));

            currentleg = currentleg->GetNext();

#ifdef DEBUG_ADAPTER_EXECUTION
            MessageInterface::ShowMessage("******* SIGNAL LEG *******\n");
            MessageInterface::ShowMessage("   isFeasible = %c, unfeasibleReason = %s, feasibilityValue = %.14lg\n",
               cMeasurement.isFeasible ? 't' : 'f', cMeasurement.unfeasibleReason.c_str(), cMeasurement.feasibilityValue);
            int ixArrs = cMeasurement.tBodies.size() - 1;
            MessageInterface::ShowMessage("   tBodies = %s, rBodies = %s\n",
               cMeasurement.tBodies.at(ixArrs)->GetName().c_str(), cMeasurement.rBodies.at(ixArrs)->GetName().c_str());
            MessageInterface::ShowMessage("   tPrecTimes = %s, rPrecTimes = %s\n",
               cMeasurement.tPrecTimes.at(ixArrs).ToString().c_str(), cMeasurement.rPrecTimes.at(ixArrs).ToString().c_str());
            MessageInterface::ShowMessage("   tLocs = %.14lg  %.14lg  %.14lg\n",
               (*cMeasurement.tLocs.at(ixArrs))[0], (*cMeasurement.tLocs.at(ixArrs))[1], (*cMeasurement.tLocs.at(ixArrs))[2]);
            MessageInterface::ShowMessage("   rLocs = %.14lg  %.14lg  %.14lg\n",
               (*cMeasurement.rLocs.at(ixArrs))[0], (*cMeasurement.rLocs.at(ixArrs))[1], (*cMeasurement.rLocs.at(ixArrs))[2]);
            MessageInterface::ShowMessage("   rangeVecs = %.14lg  %.14lg  %.14lg\n",
               (*cMeasurement.rangeVecs.at(ixArrs))[0], (*cMeasurement.rangeVecs.at(ixArrs))[1], (*cMeasurement.rangeVecs.at(ixArrs))[2]);
#endif
         } // while currentleg != NULL loop

         // here, current is SignalData for last leg (or NULL if no legs)
         if (current != NULL)
         {
            // get ionosphere and troposphere corrections, and aberration setting
            Real ionoCorr = 0.0;
            Real tropoCorr = 0.0;

            for (int j = 0; j < current->correctionIDs.size(); j++) {
               if (current->correctionIDs[j] == "Troposphere-Elev") {
                  if (current->useCorrection[j])
                     tropoCorr = current->corrections[j];                        // unit:rad
               }
               else if (current->correctionIDs[j] == "Ionosphere-Elev") {
                  if (current->useCorrection[j])
                     ionoCorr = current->corrections[j];                         // unit: rad
               }
            }
#ifdef DEBUG_ADAPTER_EXECUTION
            MessageInterface::ShowMessage("tropoCorr = %.14le rad, ionoCorr = %.14le rad\n",
               tropoCorr, ionoCorr);
            MessageInterface::ShowMessage("useAnnual = %c, useDiurnal = %c\n",
               useAnnual ? 't' : 'f', useDiurnal ? 't' : 'f');
#endif

            // get MJ2000 range vector
            Rvector3 lssb = - current->rangeVecInertial;  // gs to s/c, MJ2000 frame
#ifdef DEBUG_ADAPTER_EXECUTION
            MessageInterface::ShowMessage("lssb (MJ2000) = %.14lg  %.14lg  %.14lg\n",
               lssb[0], lssb[1], lssb[2]);
#endif

            // TBD - what to do with aberration corrections?  Here or in PhysicalSignal/SignalBase?
            // apply velocity aberration correction
            if (useAnnual || useDiurnal) {
               Rvector3 aberVel, correctedLssb;
               GetAberrationVel(useAnnual, useDiurnal, *current, aberVel);
               ApplyAberrationCorrection(lssb, aberVel, correctedLssb);
               lssb = correctedLssb;
            }
#ifdef DEBUG_ADAPTER_EXECUTION
            MessageInterface::ShowMessage("lssb (aberr corrected) = %.14lg  %.14lg  %.14lg\n",
               lssb[0], lssb[1], lssb[2]);
#endif

            // setup coordinate systems if not created yet
            if (ocs == NULL || j2k == NULL) {
               CelestialBody *earth = solarsys->GetBody("Earth");
               BodyFixedPoint *bf = (BodyFixedPoint*)current->rNode;
               ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS", "Topocentric",
                  bf, NULL, NULL, earth, solarsys);
               j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
                  bf->GetBodyFixedCoordinateSystem()->GetOrigin(), NULL, NULL, earth, solarsys);
            }

            // get range vector in body-fixed coordinates
            bfRange = current->rJ2kRotation * lssb;      // rJ2kRotation at receive time t3

#ifdef DEBUG_ADAPTER_EXECUTION
            MessageInterface::ShowMessage("rJ2kRotation = [ %.14lg  %.14lg  %.14lg\n",
               current->rJ2kRotation(0, 0), current->rJ2kRotation(0, 1), current->rJ2kRotation(0, 2));
            MessageInterface::ShowMessage("                 %.14lg  %.14lg  %.14lg\n",
               current->rJ2kRotation(1, 0), current->rJ2kRotation(1, 1), current->rJ2kRotation(1, 2));
            MessageInterface::ShowMessage("                 %.14lg  %.14lg  %.14lg ]\n",
               current->rJ2kRotation(2, 0), current->rJ2kRotation(2, 1), current->rJ2kRotation(2, 2));
            MessageInterface::ShowMessage("bfRange = %.14lg  %.14lg  %.14lg\n",
               bfRange[0], bfRange[1], bfRange[2]);
#endif

            // apply tropospheric and/or ionospheric correction, if any
            A1Mjd      itsEpoch(current->rPrecTime.GetMjd());
            Rvector6   dummyIn(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
            Rvector6   dummyOut;

            if (tropoCorr + ionoCorr != 0.0) {
               // convert body-fixed vector to topocentric SEZ
               converter.Convert(itsEpoch, dummyIn, j2k, dummyOut, ocs);
               Rmatrix33 R_Obs_j2k = converter.GetLastRotationMatrix();

               Rvector3 topovec = R_Obs_j2k * lssb;

               Real azimuth, elevation;   // get unrefracted az, el in radians
               TopocentricSEZToAzEl(topovec, azimuth, elevation);

               Real sinAzim = sin(azimuth);
               Real cosAzim = cos(azimuth);
               Real sinElev = sin(elevation);
               Real cosElev = cos(elevation);

               Rvector3 N_hat, E_hat, Z_hat;
               GetENZUnitVectors(E_hat, N_hat, Z_hat);

               Real totalCorrRad = tropoCorr + ionoCorr;                 // unit: rad
               Rvector3 D_tilde = -sinElev * cosAzim * N_hat - sinElev * sinAzim * E_hat + cosElev * Z_hat;

               Rvector3 tropoIonoCorrVec = bfRange.GetMagnitude() * tan(totalCorrRad) * D_tilde;

               bfRange += tropoIonoCorrVec;
#ifdef DEBUG_ADAPTER_EXECUTION
               MessageInterface::ShowMessage("topovec for atmos corr = %.14lg  %.14lg  %.14lg\n",
                  topovec[0], topovec[1], topovec[2]);
               MessageInterface::ShowMessage("azim (rad) = %.14lg, elev (rad) = %.14lg\n",
                  azimuth, elevation);
               MessageInterface::ShowMessage("totalCorrRad = %.14lg\n", totalCorrRad);
               MessageInterface::ShowMessage("D_tilde = %.14lg  %.14lg  %.14lg\n",
                  D_tilde[0], D_tilde[1], D_tilde[2]);
               MessageInterface::ShowMessage("tropoIonoCorrVec = %.14lg  %.14lg  %.14lg\n",
                  tropoIonoCorrVec[0], tropoIonoCorrVec[1], tropoIonoCorrVec[2]);
#endif
            }

            // convert range vector from body-fixed to topocentric
            BodyFixedPoint *bf = (BodyFixedPoint*)current->rNode;
            CoordinateSystem *rcs = bf->GetBodyFixedCoordinateSystem();
            converter.Convert(itsEpoch, dummyIn, rcs, dummyOut, ocs);
            Rmatrix33 R_Obs_bf = converter.GetLastRotationMatrix();

            // TBD topoRange = R_Obs_bf * bfVector;
            Rvector3 gsLocBf = bf->GetBodyFixedLocation(0.0);

            //topoRange = R_Obs_bf * (bfRange - gsLocBf);
            topoRange = R_Obs_bf * bfRange;

            mj2000Range = current->rJ2kRotation.Inverse() * bfRange;

#ifdef DEBUG_ADAPTER_EXECUTION
            MessageInterface::ShowMessage("topoRange = %.14lg  %.14lg  %.14lg\n",
               topoRange[0], topoRange[1], topoRange[2]);
            MessageInterface::ShowMessage("mj2000Range = %.14lg  %.14lg  %.14lg\n",
               mj2000Range[0], mj2000Range[1], mj2000Range[2]);
#endif
         }
         else
         {
            throw MeasurementException("No signal path!");
         }

         // calculate the angle measurement value from the groundstation to spacecraft vector
         // in the South-East-Z topocentric frame
         values[i] = CalcMeasValue();               // unit: deg



         // TBD TBD TBD
         //// accumulate all range corrections for signal path ith
         //for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
         //{
         //   if (current->useCorrection[j])
         //   {
         //      values[i] += current->corrections[j];
         //      corrections[i] += current->corrections[j];        // TBD - 0 for azim, elev-corr for elev, what to do for XSouth?
         //   }
         //}// for j loop



         // Get measurement epoch in the first signal path. It will apply for all other paths
         receiveEpoch = current->rPrecTime + current->rDelay / GmatTimeConstants::SECS_PER_DAY;   // TBD
         cMeasurement.epochGT = receiveEpoch;
         cMeasurement.epoch = receiveEpoch.GetMjd();

#ifdef DEBUG_ADAPTER_EXECUTION
         MessageInterface::ShowMessage("epochGT = %s, epoch = %.14lg\n",
            cMeasurement.epochGT.ToString().c_str(), cMeasurement.epoch);
#endif
      }// for i loop (for each signalPath)

      // TBD - should these 3 be computed for angles?
      // Calculate uplink frequency at received time and transmit time
      cMeasurement.uplinkFreq = calcData->GetUplinkFrequency(0, rampTB) * 1.0e6;                         // unit: Hz
      cMeasurement.uplinkFreqAtRecei = calcData->GetUplinkFrequencyAtReceivedEpoch(0, rampTB) * 1.0e6;   // unit: Hz
      cMeasurement.uplinkBand = calcData->GetUplinkFrequencyBand(0, rampTB);
#ifdef DEBUG_ADAPTER_EXECUTION
      MessageInterface::ShowMessage("uplinkFreq = %.14lg, uplinkFreqAtRecei = %.14lg, uplinkBand = %d\n",
         cMeasurement.uplinkFreq, cMeasurement.uplinkFreqAtRecei, cMeasurement.uplinkBand);
#endif

      ComputeMeasurementBias("Bias", measurementType, 2);
      ComputeMeasurementNoiseSigma("NoiseSigma", measurementType, 2);
      ComputeMeasurementErrorCovarianceMatrix();

      cMeasurement.covariance = &measErrorCovariance;

      // Set measurement values
      cMeasurement.value.clear();
      cMeasurement.correction.clear();
      for (UnsignedInt i = 0; i < values.size(); ++i)
      {
         cMeasurement.value.push_back(0.0);
         cMeasurement.correction.push_back(0.0);
      }

      for (UnsignedInt i = 0; i < values.size(); ++i)
      {
         Real measVal = values[i];                    // unit: deg
         Real corrVal = corrections[i];               // unit: deg
#ifdef DEBUG_ANGLE_CALCULATION
         MessageInterface::ShowMessage("===================================================================\n");
         MessageInterface::ShowMessage("====  AngleAdapterDeg (%s): Angle Calculation for Measurement Data %dth  \n", GetName().c_str(), i);
         MessageInterface::ShowMessage("===================================================================\n");

         MessageInterface::ShowMessage("      . Path : ");
         for (UnsignedInt k = 0; k < participantLists[i]->size(); ++k)
            MessageInterface::ShowMessage("%s,  ", participantLists[i]->at(k).c_str());
         MessageInterface::ShowMessage("\n");

         MessageInterface::ShowMessage("      . Measurement type           : <%s>\n", measurementType.c_str());
         MessageInterface::ShowMessage("      . C-value w/o noise and bias : %.14lg deg \n", values[i]);
         MessageInterface::ShowMessage("      . Noise adding option        : %s\n", (addNoise ? "true" : "false"));
         MessageInterface::ShowMessage("      . Bias adding option         : %s\n", (addBias ? "true" : "false"));
         MessageInterface::ShowMessage("      . Angle noise sigma          : %.14lg deg \n", noiseSigma[i]);
         MessageInterface::ShowMessage("      . Angle bias                 : %.14lg deg \n", measurementBias[i]);
         MessageInterface::ShowMessage("      . Multiplier                 : %.14lg \n", multiplier);
#endif

         // Apply multiplier for measurement model. This step has to
         // be done before adding bias and noise
         // (for angles multiplier is 1)
         //measVal = measVal*multiplier;
         //corrVal = corrVal*multiplier;

         // Add noise to measurement value
         if (addNoise)
         {
            // Add noise here
            RandomNumber* rn = RandomNumber::Instance();
            Real val = rn->Gaussian(0.0, noiseSigma[i]);                  // noise sigma unit: deg
            //val = rn->Gaussian(measVal, noiseSigma[i]);
            measVal += val;
            corrVal += val;
         }

         // Add bias to measurement value only after noise had been added in order to avoid adding bias' noise 
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
               measVal = measVal + passBias;                  // pass bias unit: deg
               corrVal = corrVal + passBias;                  // pass bias unit: deg
            }
            else
            {
               // add bias
               measVal = measVal + measurementBias[i];        // bias unit: deg
               corrVal = corrVal + measurementBias[i];        // bias unit: deg
            }
         }

         // adjust to 0 <= measVal < 360, or -180 <= measVal < 180
         if (cMeasurement.isPeriodic)   
         {
            measVal = GmatMathUtil::Mod(measVal, cMeasurement.period);

            if (measVal < cMeasurement.minValue)
               measVal += cMeasurement.period;

            if (measVal >= cMeasurement.minValue + cMeasurement.period)
               measVal -= cMeasurement.period;
         }
         
         cMeasurement.value[i] = measVal;                                          // unit: deg
         cMeasurement.correction[i] = corrVal;                                     // unit: deg

#ifdef DEBUG_ANGLE_CALCULATION
         MessageInterface::ShowMessage("      . C-value with noise and bias : %.14lg deg\n", cMeasurement.value[i]);
         MessageInterface::ShowMessage("      . Measurement epoch A1Mjd     : %.14lg\n", cMeasurement.epoch);
         MessageInterface::ShowMessage("      . Transmit frequency at receive epoch  : %.14lg Hz\n", cMeasurement.uplinkFreqAtRecei);
         MessageInterface::ShowMessage("      . Transmit frequency at transmit epoch : %.14lg Hz\n", cMeasurement.uplinkFreq);
         MessageInterface::ShowMessage("      . Measurement is %s\n", (cMeasurement.isFeasible ? "feasible" : "unfeasible"));
         MessageInterface::ShowMessage("      . Feasibility reason          : %s\n", cMeasurement.unfeasibleReason.c_str());
         MessageInterface::ShowMessage("      . Elevation angle             : %.14lg degree\n", cMeasurement.feasibilityValue);
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
      MessageInterface::ShowMessage("Computed measurement\n   Type:  %d\n   "
         "Type:  %s\n   UID:   %d\n   Epoch:%.14lg\n   Participants:\n",
         cMeasurement.type, cMeasurement.typeName.c_str(),
         cMeasurement.uniqueID, cMeasurement.epoch);
      for (UnsignedInt k = 0; k < cMeasurement.participantIDs.size(); ++k)
         MessageInterface::ShowMessage("      %s\n",
         cMeasurement.participantIDs[k].c_str());
      MessageInterface::ShowMessage("   Values:\n");
      for (UnsignedInt k = 0; k < cMeasurement.value.size(); ++k)
         MessageInterface::ShowMessage("      %.14lg\n",
         cMeasurement.value[k]);

      MessageInterface::ShowMessage("   Valid: %s\n",
         (cMeasurement.isFeasible ? "true" : "false"));
#endif
   }

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("AngleAdapterDeg::CalculateMeasurement(%s, "
      "<%p>, <%p>, %s) exit\n", (withEvents ? "true" : "false"), forObservation,
      rampTB, (forSimulation ? "true" : "false"));
#endif

   return cMeasurement;
}


//------------------------------------------------------------------------------
// void TopocentricSEZToAzEl(const Rvector3& topovec, Real &azimuth,
//       Real &elevation)
//------------------------------------------------------------------------------
/**
* Computes the azimuth and elevation from a topocentric South-East-Z vector
*
* @param topovec             A groundstation to S/C vector in topocentric
*                            South-East-Z (SEZ) coordinates
* @param azimuth             The output computed azimuth in radians
* @param elevation           The output computed elevation in radians
*/
//------------------------------------------------------------------------------
void AngleAdapterDeg::TopocentricSEZToAzEl(const Rvector3& topovec, Real &azimuth, Real &elevation)
{
   elevation = asin(topovec[2] / topovec.GetMagnitude());

   azimuth = atan2(topovec[1], -topovec[0]);

   if (azimuth < 0.0)
      azimuth += GmatMathConstants::TWO_PI;

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("TopocentricSEZToAzEl:  topovec = %.14lg  %.14lg  %.14lg\n",
      topovec[0], topovec[1], topovec[2]);
   MessageInterface::ShowMessage("TopocentricSEZToAzEl:  azim (rad) = %.14lg, elev (rad) = %.14lg\n",
      azimuth, elevation);
#endif
}


//------------------------------------------------------------------------------
// void TopocentricSEZToXEYN(const Rvector3& topovec, Real &xEast, Real &yNorth)
//------------------------------------------------------------------------------
/**
* Computes the X-East and Y-North angles from a topocentric South-East-Z vector
*
* @param topovec             A groundstation to S/C vector in topocentric
*                            South-East-Z (SEZ) coordinates
* @param xEast               The output computed X-East angle in radians
* @param yNorth              The output computed Y-North angle in radians
*/
//------------------------------------------------------------------------------
void AngleAdapterDeg::TopocentricSEZToXEYN(const Rvector3& topovec, Real &xEast, Real &yNorth)
{
   yNorth = asin(-topovec[0] / topovec.GetMagnitude());

   xEast = atan2(topovec[1], topovec[2]);

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("TopocentricSEZToXEYN:  topovec = %.14lg  %.14lg  %.14lg\n",
      topovec[0], topovec[1], topovec[2]);
   MessageInterface::ShowMessage("TopocentricSEZToXEYN:  xEast (rad) = %.14lg, yNorth (rad) = %.14lg\n",
      xEast, yNorth);
#endif
}


//------------------------------------------------------------------------------
// void TopocentricSEZToXSYE(const Rvector3& topovec, Real &xSouth, Real &yEast)
//------------------------------------------------------------------------------
/**
* Computes the X-East and Y-North angles from a topocentric South-East-Z vector
*
* @param topovec             A groundstation to S/C vector in topocentric
*                            South-East-Z (SEZ) coordinates
* @param xSouth              The output computed X-South angle in radians
* @param yEast               The output computed Y-East angle in radians
*/
//------------------------------------------------------------------------------
void AngleAdapterDeg::TopocentricSEZToXSYE(const Rvector3& topovec, Real &xSouth, Real &yEast)
{
   yEast = asin(topovec[1] / topovec.GetMagnitude());

   xSouth = atan2(topovec[0], topovec[2]);

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("TopocentricSEZToXSYE:  topovec = %.14lg  %.14lg  %.14lg\n",
      topovec[0], topovec[1], topovec[2]);
   MessageInterface::ShowMessage("TopocentricSEZToXSYE:  xSouth (rad) = %.14lg, yEast (rad) = %.14lg\n",
      xSouth, yEast);
#endif
}


//------------------------------------------------------------------------------
// void GetENZUnitVectors(Rvector& E, Rvector& N, Rvector& Z)
//------------------------------------------------------------------------------
/**
* Computes the groundstation East, North, and Z vectors
*
* @param E                   The output groundstation East vector
* @param N                   The output groundstation North vector
* @param Z                   The output groundstation Z vector
*/
//------------------------------------------------------------------------------
void AngleAdapterDeg::GetENZUnitVectors(Rvector& E, Rvector& N, Rvector& Z)
{
   SignalData *current = GetLastSignalData();
   if (current == NULL)
      throw MeasurementException("Path for " + measurementType + " data has no signal data!");

   if (!current->rNode->IsOfType(Gmat::GROUND_STATION))
      throw MeasurementException("Final node in signal path for " + measurementType + " data is not a GroundStation!");

   GroundstationInterface *gs = (GroundstationInterface*)current->rNode;
   Rvector3 latLongHeight = gs->GetSphericalLocation(0.0);

   Real cosLat = cos(latLongHeight[0]);
   Real sinLat = sin(latLongHeight[0]);
   Real cosLong = cos(latLongHeight[1]);
   Real sinLong = sin(latLongHeight[1]);

   E[0] = -sinLong;
   E[1] = cosLong;
   E[2] = 0.0;

   N[0] = -sinLat * cosLong;
   N[1] = -sinLat * sinLong;
   N[2] = cosLat;

   Z[0] = cosLat * cosLong;
   Z[1] = cosLat * sinLong;
   Z[2] = sinLat;

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("GetENZUnitVectors:  bfLocation = %.14lg  %.14lg  %.14lg\n",
      bfLocation[0], bfLocation[1], bfLocation[2]);
   MessageInterface::ShowMessage("GetENZUnitVectors:  latLongHeight = %.14lg  %.14lg  %.14lg\n",
      latLongHeight[0], latLongHeight[1], latLongHeight[2]);
   MessageInterface::ShowMessage("GetENZUnitVectors:  E = %.14lg  %.14lg  %.14lg\n",
      E[0], E[1], E[2]);
   MessageInterface::ShowMessage("GetENZUnitVectors:  N = %.14lg  %.14lg  %.14lg\n",
      N[0], N[1], N[2]);
   MessageInterface::ShowMessage("GetENZUnitVectors:  Z = %.14lg  %.14lg  %.14lg\n",
      Z[0], Z[1], Z[2]);
#endif
}


void AngleAdapterDeg::MJ2000ToRaDec(const Rvector3& mj2000Vector, Real &rightAscension, Real &declination)
{
   //Real xyMag = sqrt(mj2000Vector[0] * mj2000Vector[0] + mj2000Vector[1] * mj2000Vector[1]);

   Real sinDec = mj2000Vector[2] / mj2000Vector.GetMagnitude();

   //Real sinLongSc = mj2000Vector[1] / xyMag;
   //Real cosLongSc = mj2000Vector[0] / xyMag;

   //rightAscension = atan2(sinLongSc, cosLongSc);   // -pi <= rightAscension <= +pi radians
   rightAscension = atan2(mj2000Vector[1], mj2000Vector[0]);   // -pi <= rightAscension <= +pi radians

   if (rightAscension < 0.0)
      rightAscension += GmatMathConstants::TWO_PI;

   declination = asin(sinDec);    // -pi/2 <= declination <= +pi/2 radians

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("MJ2000ToRaDec:  mj2000Vector = %.14lg  %.14lg  %.14lg\n",
      mj2000Vector[0], mj2000Vector[1], mj2000Vector[2]);
   MessageInterface::ShowMessage("MJ2000ToRaDec:  rightAscension (rad) = %.14lg, declination (rad) = %.14lg\n",
      rightAscension, declination);
#endif
}


void AngleAdapterDeg::BodyFixedToLongDec(const Rvector3& bfVector, Real &longitude, Real &declination)
{
   //Real xyMag = sqrt(bfVector[0] * bfVector[0] + bfVector[1] * bfVector[1]);

   Real sinDec = bfVector[2] / bfVector.GetMagnitude();

   //Real sinLongSc = bfVector[1] / xyMag;
   //Real cosLongSc = bfVector[0] / xyMag;

   //longitude = atan2(sinLongSc, cosLongSc);   // -pi <= longSc <= +pi radians
   longitude = atan2(bfVector[1], bfVector[0]);   // -pi <= longSc <= +pi radians
   if (longitude < 0)
      longitude += GmatMathConstants::TWO_PI;

   declination = asin(sinDec);    // -pi/2 <= declination <= +pi/2 radians

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("BodyFixedToLongDec:  bfVector = %.14lg  %.14lg  %.14lg\n",
      bfVector[0], bfVector[1], bfVector[2]);
   MessageInterface::ShowMessage("BodyFixedToLongDec:  longitude (rad) = %.14lg, declination (rad) = %.14lg\n",
      longitude, declination);
#endif
}


// Rvector3 gsBfLoc = rNode->GetBodyFixedLocation(theData.rPrecTime);   // time is not used, gs assumed not to move
// Real longStn = atan2(gsBfLoc[1], gsBfLoc[0]);
// -pi <= longStn <= +pi radians
//void AngleAdapterDeg::BodyFixedToRaDec(const Rvector3& bfVector, Real longStn, Real &rightAscension, Real &declination)
//{
//   Real xyMag = sqrt(bfVector[0] * bfVector[0] + bfVector[1] * bfVector[1]);
//
//   Real sinDec = bfVector[2] / bfVector.GetMagnitude();
//
//   Real sinLongSc = bfVector[1] / xyMag;
//   Real cosLongSc = bfVector[0] / xyMag;
//
//   Real longSc = atan2(sinLongSc, cosLongSc);   // -pi <= longSc <= +pi radians
//
//   // TBD - how to get local sidereal time at station (rad)?
//   Real localSiderealTimeStn = 0.0;
//   
//   Real hourAngle = longStn - longSc;
//
//   rightAscension = localSiderealTimeStn - hourAngle;
//
//   if (rightAscension < 0.0)   // TBD - RA < -2pi?
//      rightAscension += GmatMathConstants::TWO_PI;
//
//   declination = asin(sinDec);    // -pi/2 <= declination <= +pi/2 radians
//}


Rvector3 AngleAdapterDeg::TopocentricToMJ2000T3(const Rvector3 &topoVector)
{
   SignalData *current = GetLastSignalData();
   if (current == NULL)
      throw MeasurementException("Path has no signal data!");

   //std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   //SignalBase *currentleg = paths[0]; // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths 
   //SignalData *current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));
   // TBD if current is null, throw ex
   // get SignalData of last leg
   //while (currentleg != NULL)
   //{
   //   current = currentleg->GetSignalDataObject();
   //   currentleg = currentleg->GetNext();
   //}

   // transform topoVector from topocentric to body fixed at receive time t3
   A1Mjd      itsEpoch(current->rPrecTime.GetMjd());
   Rvector6   dummyIn(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
   Rvector6   dummyOut;

   BodyFixedPoint *bf = (BodyFixedPoint*)current->rNode;
   CoordinateSystem *rcs = bf->GetBodyFixedCoordinateSystem();
   converter.Convert(itsEpoch, dummyIn, rcs, dummyOut, ocs);
   Rmatrix33 R_Obs_bf = converter.GetLastRotationMatrix();

   Rvector3 bfVector = R_Obs_bf.Inverse() * topoVector;

   // transform bodyfixed vector to inertial at receive time t3
   Rvector3 mj2000Vector = current->rJ2kRotation.Inverse() * bfVector;

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("TopocentricToMJ2000T3:  topoVector = %.14lg  %.14lg  %.14lg\n",
      topoVector[0], topoVector[1], topoVector[2]);
   MessageInterface::ShowMessage("TopocentricToMJ2000T3:  bfVector = %.14lg  %.14lg  %.14lg\n",
      bfVector[0], bfVector[1], bfVector[2]);
   MessageInterface::ShowMessage("TopocentricToMJ2000T3:  mj2000Vector = %.14lg  %.14lg  %.14lg\n",
      mj2000Vector[0], mj2000Vector[1], mj2000Vector[2]);
#endif

   return mj2000Vector;
}


Rvector3 AngleAdapterDeg::BodyFixedToMJ2000T3(const Rvector3 &bfVector)
{
   SignalData *current = GetLastSignalData();
   if (current == NULL)
      throw MeasurementException("Path has no signal data!");

   //std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   //SignalBase *currentleg = paths[0]; // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths 
   //SignalData *current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));
   // TBD if current is null, throw ex
   // get SignalData of last leg
   //while (currentleg != NULL)
   //{
   //   current = currentleg->GetSignalDataObject();
   //   currentleg = currentleg->GetNext();
   //}

   // transform bodyfixed vector to inertial at receive time t3
   Rvector3 mj2000Vector = current->rJ2kRotation.Inverse() * bfVector;

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("BodyFixedToMJ2000T3:  bfVector = %.14lg  %.14lg  %.14lg\n",
      bfVector[0], bfVector[1], bfVector[2]);
   MessageInterface::ShowMessage("BodyFixedToMJ2000T3:  mj2000Vector = %.14lg  %.14lg  %.14lg\n",
      mj2000Vector[0], mj2000Vector[1], mj2000Vector[2]);
#endif

   return mj2000Vector;
}


//------------------------------------------------------------------------------
// void PartialXEPartialEl(const Rvector3& topovec, Real &partial)
//------------------------------------------------------------------------------
/**
* Computes the partial derivatives of XEast w.r.t elevation from a topocentric
* South-East-Z vector
*
* @param topovec             A groundstation to S/C vector in topocentric
*                            South-East-Z (SEZ) coordinates
* @param partial             The output partial derivative
*/
//------------------------------------------------------------------------------
void AngleAdapterDeg::PartialXEPartialEl(const Rvector3& topovec, Real &partial)
{
   Real topomag = topovec.GetMagnitude();

   partial = -(topovec[1] * topomag*topomag) /
      ((topomag*topomag - topovec[0]*topovec[0]) *
      GmatMathUtil::Sqrt(topomag*topomag - topovec[2]*topovec[2]));

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("PartialXEPartialEl:  topovec = %.14lg  %.14lg  %.14lg\n",
      topovec[0], topovec[1], topovec[2]);
   MessageInterface::ShowMessage("PartialXEPartialEl:  partial = %.14lg\n",
      partial);
#endif
}


//------------------------------------------------------------------------------
// void PartialYNPartialEl(const Rvector3& topovec, Real &partial)
//------------------------------------------------------------------------------
/**
* Computes the partial derivatives of YNorth w.r.t elevation from a topocentric
* South-East-Z vector
*
* @param topovec             A groundstation to S/C vector in topocentric
*                            South-East-Z (SEZ) coordinates
* @param partial             The output partial derivative
*/
//------------------------------------------------------------------------------
void AngleAdapterDeg::PartialYNPartialEl(const Rvector3& topovec, Real &partial)
{
   Real topomag = topovec.GetMagnitude();

   partial = (topovec[0] * topovec[2]) /
      (GmatMathUtil::Sqrt(topomag*topomag - topovec[0]*topovec[0]) *
      GmatMathUtil::Sqrt(topomag*topomag - topovec[2]*topovec[2]));

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("PartialYNPartialEl:  topovec = %.14lg  %.14lg  %.14lg\n",
      topovec[0], topovec[1], topovec[2]);
   MessageInterface::ShowMessage("PartialYNPartialEl:  partial = %.14lg\n",
      partial);
#endif
}


//------------------------------------------------------------------------------
// void PartialXSPartialEl(const Rvector3& topovec, Real &partial)
//------------------------------------------------------------------------------
/**
* Computes the partial derivatives of XSouth w.r.t elevation from a topocentric
* South-East-Z vector
*
* @param topovec             A groundstation to S/C vector in topocentric
*                            South-East-Z (SEZ) coordinates
* @param partial             The output partial derivative
*/
//------------------------------------------------------------------------------
void AngleAdapterDeg::PartialXSPartialEl(const Rvector3& topovec, Real &partial)
{
   Real topomag = topovec.GetMagnitude();

   partial = (-topovec[0] * topomag*topomag) /
      ((topomag*topomag - topovec[1]*topovec[1]) *
      GmatMathUtil::Sqrt(topomag*topomag - topovec[2]*topovec[2]));

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("PartialXSPartialEl:  topovec = %.14lg  %.14lg  %.14lg\n",
      topovec[0], topovec[1], topovec[2]);
   MessageInterface::ShowMessage("PartialXSPartialEl:  partial = %.14lg\n",
      partial);
#endif
}


//------------------------------------------------------------------------------
// void PartialYEPartialEl(const Rvector3& topovec, Real &partial)
//------------------------------------------------------------------------------
/**
* Computes the partial derivatives of YEast w.r.t elevation from a topocentric
* South-East-Z vector
*
* @param topovec             A groundstation to S/C vector in topocentric
*                            South-East-Z (SEZ) coordinates
* @param partial             The output partial derivative
*/
//------------------------------------------------------------------------------
void AngleAdapterDeg::PartialYEPartialEl(const Rvector3& topovec, Real &partial)
{
   Real topomag = topovec.GetMagnitude();

   partial = -(topovec[1] * topovec[2]) /
      (GmatMathUtil::Sqrt(topomag*topomag - topovec[1]*topovec[1]) *
      GmatMathUtil::Sqrt(topomag*topomag - topovec[2]*topovec[2]));

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("PartialYEPartialEl:  topovec = %.14lg  %.14lg  %.14lg\n",
      topovec[0], topovec[1], topovec[2]);
   MessageInterface::ShowMessage("PartialYEPartialEl:  partial = %.14lg\n",
      partial);
#endif
}


//------------------------------------------------------------------------------
// void GetAberrationVel(bool useAnnual, bool useDiurnal,
//    const SignalData& theData, Rvector& aberrationVel)
//------------------------------------------------------------------------------
/**
* Computes the groundstation velocity to use for aberration correction
*
* @param useAnnual           Annual aberration correction is requested
* @param useDiurnal          Diurnal aberration correction is requested
* @param theData             The SignalData object for the downlink path
* @param aberrationVel       The output SSB velocity of the groundstation's
*                            central body at reception time
*/
//------------------------------------------------------------------------------
void AngleAdapterDeg::GetAberrationVel(bool useAnnual, bool useDiurnal,
   const SignalData& theData, Rvector& aberrationVel)
{
   aberrationVel.MakeZeroVector();

   if (useAnnual)
   {
      // rNode's central body's SSB velocity at receive time t3
      if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
      {
         aberrationVel += theData.rOStateSSB.GetV();
#ifdef DEBUG_ADAPTER_EXECUTION
         MessageInterface::ShowMessage("GetAberrationVel:  annual adding aberrVel %lf %lf %lf\n",
            theData.rOStateSSB.GetV()[0], theData.rOStateSSB.GetV()[1], theData.rOStateSSB.GetV()[2]);
#endif
      }
   }

   if (useDiurnal)
   {
      // rNode's SSB velocity due to central body rotation only, at receive time t3
      if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
      {
         SpecialCelestialPoint* ssb = solarsys->GetSpecialPoint("SolarSystemBarycenter");
         Rvector6 gsMJ2000SSB = theData.rNode->GetMJ2000PrecState(theData.rPrecTime) -
            ssb->GetMJ2000PrecState(theData.rPrecTime);
         Rvector3 originVelMJ2000 = theData.rOStateSSB.GetV();
         Rvector3 correctionVec = gsMJ2000SSB.GetV() - originVelMJ2000;
         aberrationVel += correctionVec;
#ifdef DEBUG_ADAPTER_EXECUTION
         MessageInterface::ShowMessage("GetAberrationVel:  diurnal adding aberrVel %.14lg  %.14lg  %.14lg\n",
            correctionVec[0], correctionVec[1], correctionVec[2]);
#endif
      }
   }

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("GetAberrationVel:  aberrationVel = %.14lg  %.14lg  %.14lg\n",
      aberrationVel[0], aberrationVel[1], aberrationVel[2]);
#endif
}


//------------------------------------------------------------------------------
// void ApplyAberrationCorrection(const Rvector3& lssb, const Rvector3& gsvel,
//    Rvector3& corrected_lssb)
//------------------------------------------------------------------------------
/**
* Apply aberration correction to a groundstation to S/C vector in the SSB 
* (inertial) frame, to compute an aberration corrected vector in the same frame
*
* @param lssb                The groundstation to S/C vector in the SSB frame
* @param gsvel               The SSB velocity of the groundstation's central
*                            body at reception time
* @param corrected_lssb      The output aberration corrected groundstation to
*                            S/C vector in the SSB frame
*/
//------------------------------------------------------------------------------
void AngleAdapterDeg::ApplyAberrationCorrection(const Rvector3& lssb, const Rvector3& gsvel,
   Rvector3& corrected_lssb)
{
   Real c = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M;      // unit: km/s

   Rvector3 lssb_unit = lssb.GetUnitVector();

   Rvector3 gsvel_over_c = gsvel / c;

   Real beta_inv = sqrt(1.0 - pow(gsvel_over_c.GetMagnitude(), 2));

   Real f1 = lssb_unit * gsvel_over_c;

   Real f2 = 1.0 + f1 / (1.0 + beta_inv);

   corrected_lssb = (lssb * beta_inv + gsvel_over_c * f2 * lssb.GetMagnitude()) / (1.0 + f1);

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("ApplyAberrationCorrection:  lssb_unit = %.14lg  %.14lg  %.14lg\n",
      lssb_unit[0], lssb_unit[1], lssb_unit[2]);
   MessageInterface::ShowMessage("ApplyAberrationCorrection:  gsvel_over_c = %.14lg  %.14lg  %.14lg\n",
      gsvel_over_c[0], gsvel_over_c[1], gsvel_over_c[2]);
   MessageInterface::ShowMessage("ApplyAberrationCorrection:  beta_inv = %.14lg, f1 = %.14lg, f2 = %.14lg\n",
      beta_inv, f1, f2);
   MessageInterface::ShowMessage("ApplyAberrationCorrection:  corrected_lssb = %.14lg  %.14lg  %.14lg\n",
      corrected_lssb[0], corrected_lssb[1], corrected_lssb[2]);
#endif
}


//-------------------------------------------------------------------------------------
// Real GetIonoCorrection()
//-------------------------------------------------------------------------------------
/**
* This function is used to get ionosphere correction (unit: deg)
*
* @return     ionosphere correction of a measurement in deg
*/
//-------------------------------------------------------------------------------------
Real AngleAdapterDeg::GetIonoCorrection()
{
   Real correction = 0.0;

   SignalData *current = GetLastSignalData();
   if (current == NULL)
      throw MeasurementException("Path has no signal data!");

   // get ionosphere elevation correction
   if (current != NULL)
   {
      for (UnsignedInt i = 0; i < current->correctionIDs.size(); ++i)
      {
         if ((current->useCorrection[i]) && (current->correctionIDs[i] == "Ionosphere-Elev"))
         {
            correction = current->corrections[i] * GmatMathConstants::DEG_PER_RAD;           // angle ionosphere correction in degree
            break;
        }
      }
   }

#ifdef DEBUG_ANGLE_IONOSPHERE_CORRECTION
   MessageInterface::ShowMessage("GetIonoCorrection:  correction = %.14le degree\n", correction);
#endif
   return correction;
}


//-------------------------------------------------------------------------------------
// Real GetTropoCorrection()
//-------------------------------------------------------------------------------------
/**
* This function is used to get troposphere correction (unit: deg)
*
* @return     troposphere correction of a measurement in deg
*/
//-------------------------------------------------------------------------------------
Real AngleAdapterDeg::GetTropoCorrection()
{
   Real correction = 0.0;

   SignalData *current = GetLastSignalData();
   if (current == NULL)
      throw MeasurementException("Path has no signal data!");

   //std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   //SignalBase *currentleg = paths[0]; // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths 
   //SignalData *current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));
   // find SignalData of last leg
   //while (currentleg != NULL)
   //{
   //   current = currentleg->GetSignalDataObject();
   //   currentleg = currentleg->GetNext();
   //}

   // get troposphere elevation correction
   if (current != NULL)
   {
      for (UnsignedInt i = 0; i < current->correctionIDs.size(); ++i)
      {
         if ((current->useCorrection[i]) && (current->correctionIDs[i] == "Troposphere-Elev"))
         {
            correction = current->corrections[i] * GmatMathConstants::DEG_PER_RAD;
            break;
         }
      }
   }

#ifdef DEBUG_ADAPTER_EXECUTION
   MessageInterface::ShowMessage("GetTropoCorrection:  correction = %.14lg\n", correction);
#endif
   return correction;
}




//const MeasurementData& AngleAdapterDeg::CalculateMeasurementAtOffset(
//   bool withEvents, Real dt, ObservationData* forObservation,
//   std::vector<RampTableData>* rampTB, bool forSimulation)
//{
//   static MeasurementData offsetMeas;
//
//   if (!calcData)
//      throw MeasurementException("Measurement data was requested for " +
//      instanceName + " before the measurement was set");
//
//   // Fire the measurement model to build the collection of signal data
//   if (calcData->CalculateMeasurement(withLighttime, withMediaCorrection, forObservation, rampTB, forSimulation, dt))
//   {
//      std::vector<SignalData*> data = calcData->GetSignalData();
//      std::string unfeasibilityReason;
//      Real        unfeasibilityValue;
//
//      // set to default
//      offsetMeas.isFeasible = true;
//      offsetMeas.unfeasibleReason = "N";
//      offsetMeas.feasibilityValue = 90.0;
//
//      RealArray values;
//      for (UnsignedInt i = 0; i < data.size(); ++i)
//      {
//         // Calculate C-value for signal path ith:
//         values.push_back(0.0);
//         SignalData *current = data[i];
//         SignalData *first = current;
//         UnsignedInt legIndex = 0;
//
//         while (current != NULL)
//         {
//            ++legIndex;
//            // Set feasibility value
//            if (current->feasibilityReason[0] == 'N')
//            {
//               if (current->stationParticipant)
//                  offsetMeas.feasibilityValue = current->feasibilityValue;
//            }
//            else if (current->feasibilityReason[0] == 'B')
//            {
//               std::stringstream ss;
//               ss << "B" << legIndex << current->feasibilityReason.substr(1);
//               current->feasibilityReason = ss.str();
//               if (offsetMeas.unfeasibleReason[0] == 'N')
//               {
//                  offsetMeas.unfeasibleReason = current->feasibilityReason;
//                  offsetMeas.isFeasible = false;
//                  offsetMeas.feasibilityValue = current->feasibilityValue;
//               }
//            }
//
//            // accumulate all light time range for signal path ith
//            Rvector3 signalVec = current->rangeVecInertial;
//            values[i] += signalVec.GetMagnitude();
//
//            // accumulate all range corrections for signal path ith
//            for (UnsignedInt j = 0; j < current->correctionIDs.size(); ++j)
//            {
//               if (current->useCorrection[j])
//                  values[i] += current->corrections[j];
//            }// for j loop
//
//            // accumulate all hardware delays for signal path ith
//            values[i] += ((current->tDelay + current->rDelay)*
//               GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM);
//
//            // Get measurement epoch in the first signal path. It will apply for all other paths
//            if (i == 0)
//            {
//               if (calcData->GetTimeTagFlag())
//               {
//                  // Measurement epoch will be at the end of signal path when time tag is at the receiver
//                  if (current->next == NULL)
//                  {
//                     offsetMeas.epochGT = current->rPrecTime + current->rDelay / GmatTimeConstants::SECS_PER_DAY;
//                     offsetMeas.epoch = current->rPrecTime.GetMjd() + current->rDelay / GmatTimeConstants::SECS_PER_DAY;
//                  }
//               }
//               else
//               {
//                  // Measurement epoch will be at the begin of signal path when time tag is at the transmiter
//                  offsetMeas.epochGT = first->tPrecTime - first->tDelay / GmatTimeConstants::SECS_PER_DAY;
//                  offsetMeas.epoch = first->tPrecTime.GetMjd() - first->tDelay / GmatTimeConstants::SECS_PER_DAY;
//               }
//            }
//
//            current = current->next;
//         }// while loop
//
//      }// for i loop
//
//
//      // Set measurement values
//      offsetMeas.value.clear();
//      for (UnsignedInt i = 0; i < values.size(); ++i)
//         offsetMeas.value.push_back(0.0);
//
//      Real nsigma;
//      for (UnsignedInt i = 0; i < values.size(); ++i)
//      {
//         Real measVal = values[i];
//
//         //@Todo: write code to add bias to measuement value here
//#ifdef DEBUG_RANGE_CALCULATION
//         MessageInterface::ShowMessage("      . No bias was implemented in this GMAT version.\n");
//#endif
//
//         // Add noise to measurement value
//         nsigma = noiseSigma[i];                  // noiseSigma[i] is noise sigma associated with measurement values[i]
//         if (addNoise)
//         {
//            // Add noise here
//            RandomNumber* rn = RandomNumber::Instance();
//            Real val = rn->Gaussian(measVal, nsigma);
//            while (val <= 0.0)
//               val = rn->Gaussian(measVal, nsigma);
//            measVal = val;
//         }
//         offsetMeas.value[i] = measVal;
//
//      }
//   }
//
//   return offsetMeas;
//}


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
const std::vector<RealArray>& AngleAdapterDeg::CalculateMeasurementDerivatives(
   GmatBase* obj, Integer id)
{
#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("AngleAdapterDeg::CalculateMeasurementDerivatives(%s, %d) called\n",
      obj->GetName().c_str(), id);
#endif

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

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("Solve-for parameter: %s\n", paramName.c_str());
#endif

   // Perform the calculations
#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("parameterID = %d; Epoch %.14lg\n",
      parameterID, cMeasurement.epoch);
#endif

   // Clear derivative variable
   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
      theDataDerivatives[i].clear();
   theDataDerivatives.clear();

   Integer size = obj->GetEstimationParameterSize(id);
   if (size <= 0)
      throw MeasurementException("The derivative parameter on derivative "
         "object " + obj->GetName() + "is not recognized");
   RealArray oneRow;
   oneRow.assign(size, 0.0);
   theDataDerivatives.push_back(oneRow);

   std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   if (paths.size() > 1)
      throw MeasurementException("Current version of GmatEstimation plugin only "
         "supports 1 signal path.");

   SignalData *current = GetLastSignalData();
   if (current == NULL)
      throw MeasurementException("Path has no signal data!");

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
      
      //CalculateBiasDerivatives(obj, id, current, size);
      if (((ErrorModel*)obj)->GetStringParameter("Type") == typeName && checker > 1)
      {
         theDataDerivatives = calcData->CalculateMeasurementDerivatives(obj, id);
      }
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
      if (((ErrorModel*)obj)->GetStringParameter("Type") == typeName)
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
      // Check to see if obj is a participant
      GmatBase *objPtr = NULL;
      if (current->tNode == obj)
         objPtr = current->tNode;
      if (current->rNode == obj)
         objPtr = current->rNode;

      if (objPtr != NULL)           // Derivative object is a participant (either GroundStation or Spacecraft):
      {
         if (paramName == "Position")
         {
            Rvector6 dv = CalcDerivValue();

            Rmatrix M_dR_t2_dR_t3 = Calc_dR_t2_dR_t3(obj, paramName, current);

            Rvector r_vector = dv.GetR();
            Rvector temp = r_vector * M_dR_t2_dR_t3;

            for (UnsignedInt jj = 0; jj < 3; ++jj)
            {
               theDataDerivatives[0][jj] = temp[jj];
            }
         }
         else if (paramName == "Velocity")
         {
            Rvector6 dv = CalcDerivValue();

            Rmatrix M_dR_t2_dR_t3 = Calc_dR_t2_dR_t3(obj, paramName, current);

            Rvector r_vector = dv.GetR();
            Rvector temp = r_vector * M_dR_t2_dR_t3;

            for (UnsignedInt jj = 0; jj < 3; ++jj)
            {
               theDataDerivatives[0][jj] = temp[jj];
            }
         }
         else if (paramName == "CartesianX")
         {
            Rvector6 dv = CalcDerivValue();

            Rmatrix M_dR_t2_dR_t3 = Calc_dR_t2_dR_t3(obj, paramName, current);

            Rvector r_vector = dv.GetR();
            Rvector temp = r_vector * M_dR_t2_dR_t3;

            for (UnsignedInt jj = 0; jj < 6; ++jj)
            {
               theDataDerivatives[0][jj] = temp[jj];
            }
         }
         else if (paramName == "Cr_Epsilon")
         {
            Real result = GetCrDerivative(objPtr, current);
            theDataDerivatives[0][0] = result;
         }
         else if (paramName == "Cd_Epsilon")
         {
            Real result = GetCdDerivative(objPtr, current);
            theDataDerivatives[0][0] = result;
         }
         else if (paramName == "AtmosDensity_Epsilon")
         {
            Real result = GetADSFDerivative(objPtr, current);
            theDataDerivatives[0][0] = result;
         }
      }
      else if (GmatStringUtil::EndsWith(paramName, ".TSF_Epsilon"))
      {
         // find S/C
         GmatBase *forObj = NULL;
         ODEModel *odeModel = (ODEModel *)obj;
         int numForces = odeModel->GetNumForces();
         for (int forceIndex = 0; forceIndex < numForces; forceIndex++) {   // for each force
            PhysicalModel *physMod = odeModel->GetForce(forceIndex);
            if (physMod->GetTypeName() == "FileThrust") {   // is is a FileThrust force?
               // does this FileThrust contain our parameter?
               Integer id = physMod->GetParameterID(paramName);
               if (id > -1) {
                  StringArray scNames = physMod->GetRefObjectNameArray(Gmat::SPACECRAFT);

                  if (current->tNode != NULL && current->tNode->GetType() == Gmat::SPACECRAFT) {
                     std::string name = current->tNode->GetName();
                     if (find(scNames.begin(), scNames.end(), name) != scNames.end()) {
                        forObj = current->tNode;
                        break;
                     }
                  }
                  if (current->rNode != NULL && current->rNode->GetType() == Gmat::SPACECRAFT) {
                     std::string name = current->rNode->GetName();
                     if (find(scNames.begin(), scNames.end(), name) != scNames.end()) {
                        forObj = current->rNode;
                        break;
                     }
                  }
               }
            }
         }
         if (forObj != NULL) {
            Real result = GetTSFDerivative(forObj, paramName, current);
            theDataDerivatives[0][0] = result;
         }
      }
      else
      {
#ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   Deriv is w.r.t. something "
            "independent, so zero\n");
#endif
         // Set 0 to all elements (number of elements is specified by size)
         for (UnsignedInt i = 0; i < size; ++i)
            theDataDerivatives[0][i] += 0.0;
      }
   }

#ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("   Deriv is w.r.t. %s  it value %lf\n", paramName.c_str(), theDataDerivatives[0][0]);
#endif

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
            MessageInterface::ShowMessage("%.14lg", (theDataDerivatives.at(i))[j]);
         }
      }
      MessageInterface::ShowMessage("]\n");
#endif

   //   UnsignedInt size = derivativeData->at(0).size();
   //   for (UnsignedInt i = 0; i < derivativeData->size(); ++i)
   //   {
   //      RealArray oneRow;
   //      oneRow.assign(size, 0.0);
   //      theDataDerivatives.push_back(oneRow);

   //      if (derivativeData->at(i).size() != size)
   //         throw MeasurementException("Derivative data size is a different size "
   //         "than expected");

   //      for (UnsignedInt j = 0; j < size; ++j)
   //      {
   //         theDataDerivatives[i][j] = (derivativeData->at(i))[j] * factor;
   //      }
   //   }
   //}

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("Exit AngleAdapterDeg::CalculateMeasurementDerivatives(%s, %d)\n",
      obj->GetName().c_str(), id);
#endif

   return theDataDerivatives;
}


Rmatrix AngleAdapterDeg::Calc_dR_t2_dR_t3(GmatBase *forObj, const std::string &paramName,
   SignalData *current)
{
   // TBD - I think this is overkill, I think forObj is always rNode
   // 1. Calculate phi matrix
   bool forTransmitter = true;
   if (current->rNode == forObj)
      forTransmitter = false;
   else
   {
      if (current->tNode != forObj)
         throw MeasurementException(paramName + " derivative requested, but "
            "neither participant is the \"for\" object");
   }
   Rmatrix phi = (forTransmitter ? (current->tSTM * current->tSTMtm.Inverse()) :
      (current->rSTM * current->rSTMtm.Inverse()));

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("forObj = %s, paramName = %s\n", forObj->GetName().c_str(),
      paramName);
   MessageInterface::ShowMessage("forTransmitter = %s\n", forTransmitter ? "true" : "false");
   Rmatrix STM1 = (forTransmitter ? current->tSTM : current->rSTM);
   Rmatrix STMtm = (forTransmitter ? current->tSTMtm : current->rSTMtm);
   MessageInterface::ShowMessage("STM = [ %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg\n",
      STM1(0, 0), STM1(0, 1), STM1(0, 2), STM1(0, 3), STM1(0, 4), STM1(0, 5));
   MessageInterface::ShowMessage("        %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg\n",
      STM1(1, 0), STM1(1, 1), STM1(1, 2), STM1(1, 3), STM1(1, 4), STM1(1, 5));
   MessageInterface::ShowMessage("        %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg\n",
      STM1(2, 0), STM1(2, 1), STM1(2, 2), STM1(2, 3), STM1(2, 4), STM1(2, 5));
   MessageInterface::ShowMessage("        %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg\n",
      STM1(3, 0), STM1(3, 1), STM1(3, 2), STM1(3, 3), STM1(3, 4), STM1(3, 5));
   MessageInterface::ShowMessage("        %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg\n",
      STM1(4, 0), STM1(4, 1), STM1(4, 2), STM1(4, 3), STM1(4, 4), STM1(4, 5));
   MessageInterface::ShowMessage("        %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg ]\n",
      STM1(5, 0), STM1(5, 1), STM1(5, 2), STM1(5, 3), STM1(5, 4), STM1(5, 5));

   MessageInterface::ShowMessage("STMtm = [ %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg\n",
      STMtm(0, 0), STMtm(0, 1), STMtm(0, 2), STMtm(0, 3), STMtm(0, 4), STMtm(0, 5));
   MessageInterface::ShowMessage("          %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg\n",
      STMtm(1, 0), STMtm(1, 1), STMtm(1, 2), STMtm(1, 3), STMtm(1, 4), STMtm(1, 5));
   MessageInterface::ShowMessage("          %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg\n",
      STMtm(2, 0), STMtm(2, 1), STMtm(2, 2), STMtm(2, 3), STMtm(2, 4), STMtm(2, 5));
   MessageInterface::ShowMessage("          %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg\n",
      STMtm(3, 0), STMtm(3, 1), STMtm(3, 2), STMtm(3, 3), STMtm(3, 4), STMtm(3, 5));
   MessageInterface::ShowMessage("          %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg\n",
      STMtm(4, 0), STMtm(4, 1), STMtm(4, 2), STMtm(4, 3), STMtm(4, 4), STMtm(4, 5));
   MessageInterface::ShowMessage("          %.14lg  %.14lg  %.14lg %.14lg  %.14lg  %.14lg ]\n",
      STMtm(5, 0), STMtm(5, 1), STMtm(5, 2), STMtm(5, 3), STMtm(5, 4), STMtm(5, 5));
#endif

   // 2. Calculate matrix
   if (paramName == "Position") {
      Rmatrix A(3, 3);

      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            A(i, j) = phi(i, j);        // sub-matrix A of state transition matrix Phi in Equation 6.31 in GMAT MathSpec

#ifdef DEBUG_ADAPTER_DERIVATIVES
         MessageInterface::ShowMessage("A = [ %.14lg  %.14lg  %.14lg\n",
            A(0, 0), A(0, 1), A(0, 2));
         MessageInterface::ShowMessage("      %.14lg  %.14lg  %.14lg\n",
            A(1, 0), A(1, 1), A(1, 2));
         MessageInterface::ShowMessage("      %.14lg  %.14lg  %.14lg ]\n",
            A(2, 0), A(2, 1), A(2, 2));
#endif
         return A;
   }
   else if (paramName == "Velocity") {
      Rmatrix B(3, 3);

      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            B(i, j) = phi(i, j + 3);     // sub-matrix B of state transition matrix Phi in Equation 6.31 in GMAT MathSpec

#ifdef DEBUG_ADAPTER_DERIVATIVES
         MessageInterface::ShowMessage("B = [ %.14lg  %.14lg  %.14lg\n",
            B(0, 0), B(0, 1), B(0, 2));
         MessageInterface::ShowMessage("      %.14lg  %.14lg  %.14lg\n",
            B(1, 0), B(1, 1), B(1, 2));
         MessageInterface::ShowMessage("      %.14lg  %.14lg  %.14lg ]\n",
            B(2, 0), B(2, 1), B(2, 2));
#endif
         return B;
   }
   else if (paramName == "CartesianX") {
      Rmatrix AB(3, 6);

      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 6; ++j)
            AB(i, j) = phi(i, j);

#ifdef DEBUG_ADAPTER_DERIVATIVES
         MessageInterface::ShowMessage("AB = [ %.14lg  %.14lg  %.14lg  %.14lg  %.14lg  %.14lg\n",
            AB(0, 0), AB(0, 1), AB(0, 2), AB(0, 3), AB(0, 4), AB(0, 5));
         MessageInterface::ShowMessage("       %.14lg  %.14lg  %.14lg  %.14lg  %.14lg  %.14lg\n",
            AB(1, 0), AB(1, 1), AB(1, 2), AB(1, 3), AB(1, 4), AB(1, 5));
         MessageInterface::ShowMessage("       %.14lg  %.14lg  %.14lg  %.14lg  %.14lg  %.14lg ]\n",
            AB(2, 0), AB(2, 1), AB(2, 2), AB(2, 3), AB(2, 4), AB(2, 5));
#endif
         return AB;
   }
   else {
      // this should not happen, as this function should only be called when paramName is one of the 3 values above
      throw MeasurementException(paramName + " derivative requested, but "
         "expecting only \"Position\", \"Velocity\", or \"CartesianX\".");
   }
}


void AngleAdapterDeg::CalculateBiasDerivatives(GmatBase* obj, Integer id,
   SignalData *current, Integer derivSize)
{
#ifdef DEBUG_ANGLE_BIAS_DERIVATIVE
   MessageInterface::ShowMessage("CalculateBiasDerivatives(%s, %d, %p, %d) start\n",
      obj->GetName().c_str(), id, current, derivSize);
#endif
   
   // Get ErrorModel.type parameter
   std::string meastype = ((ErrorModel*)obj)->GetStringParameter("Type");
   if (measurementType == meastype)
   {
      if (meastype == "Azimuth" || meastype == "Elevation" ||
         meastype == "XEast" || meastype == "YNorth" ||
         meastype == "XSouth" || meastype == "YEast" ||
         meastype == "RightAscension" || meastype == "Declination")
      {
         // This signal leg is the last one in signal path
         if (current->rNode->IsOfType(Gmat::GROUND_STATION))
         {
            // if ground station is at the end of signal path, take derivative w.r.t
            // the bias associate to ground station's error mode, otherwise keep default value 0
            // Get full name of the derivitive object
            std::string derivObjName = obj->GetFullName();

            // Get names of all error models defined in the ground station
            GroundstationInterface* gs = (GroundstationInterface*)current->rNode;
            std::map<std::string, ObjectArray> errmodelMap = gs->GetErrorModelMap();

            // Search for error model
            bool found = false;
            for (std::map<std::string, ObjectArray>::iterator mapIndex = errmodelMap.begin();
               mapIndex != errmodelMap.end(); ++mapIndex)
            {
               for (UnsignedInt j = 0; j < (mapIndex)->second.size(); ++j)
               {
                  // TBD what is this?  debug this!
                  // does meas type name need to match?
                  // modelType(Azimuth, ...) or could use
                  // cMeasurement.typeName  (Azimuth, ...)

               ////MessageInterface::ShowMessage("derivObjNAme = <%s>    ErrorModel obj = <%s,%p>",
               ////   derivObjName.c_str(),
               ////   (mapIndex)->second.at(j)->GetFullName().c_str(), (mapIndex)->second.at(j));

                  if ((mapIndex)->second.at(j)->GetFullName() == derivObjName)
                  {
                     found = true;
                     break;
                  }
                  if (found)
                     break;
               }
            }

            if (found)
            {
               for (Integer i = 0; i < derivSize; ++i)
                  theDataDerivatives[0][i] = 1.0;
            }
         }
      }
   }

#ifdef DEBUG_ANGLE_BIAS_DERIVATIVE
   for (Integer i = 0; i < derivSize; ++i)
   {
      if (i > 0)
         MessageInterface::ShowMessage(", ");
      MessageInterface::ShowMessage("%.14lg", (theDataDerivatives.at(0))[i]);
   }
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("CalculateBiasDerivatives(%s, %d, %p, %d) end.\n    ",
      obj->GetName().c_str(), id, current, derivSize);
#endif
}


Real AngleAdapterDeg::GetCrDerivative(GmatBase *forObj, SignalData *current)
{
   // Get index for Cr
   StringArray stmElemNames = forObj->GetStringArrayParameter("StmElementNames");
   UnsignedInt CrIndex = 0;
   bool found = false;
   for (int i = 0; i < stmElemNames.size(); i++)
   {
      if (stmElemNames.at(i) == "CartesianState" || stmElemNames.at(i) == "KeplerianState")
         CrIndex += 6;
      else if ((stmElemNames.at(i) == "Cr") || (stmElemNames.at(i) == "SPADSRPScaleFactor"))
      {
         found = true;
         break;
      }
      else
      {
         CrIndex += forObj->GetEstimationParameterSize(forObj->GetParameterID(stmElemNames.at(i)));
      }
   }

   if (!found)
      return 0.0;

   CrIndex -= 6;  // vector that GetCDerivativeVector() returns does not include cartesian state

   // Get C derivative vector
   Rvector dVector;
   GetCDerivativeVector(forObj, dVector, "Cr", current);

   // Get Cr partial derivative
   Real deriv = dVector[CrIndex];

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("AngleAdapterDeg::GetCrDerivative(%s, %p):  deriv = %.14lg\n",
      forObj->GetName().c_str(), current, deriv);
#endif
   return deriv;
}


Real AngleAdapterDeg::GetCdDerivative(GmatBase *forObj, SignalData *current)
{
   // Get index for Cd
   StringArray stmElemNames = forObj->GetStringArrayParameter("StmElementNames");
   UnsignedInt CdIndex = 0;
   bool found = false;
   for (int i = 0; i < stmElemNames.size(); i++)
   {
      if (stmElemNames.at(i) == "CartesianState" || stmElemNames.at(i) == "KeplerianState")
         CdIndex += 6;
      else if ((stmElemNames.at(i) == "Cd") || (stmElemNames.at(i) == "SPADDragScaleFactor"))
      {
         found = true;
         break;
      }
      else
      {
         CdIndex += forObj->GetEstimationParameterSize(forObj->GetParameterID(stmElemNames.at(i)));
      }
   }

   if (!found)
      return 0.0;

   CdIndex -= 6;  // vector that GetCDerivativeVector() returns does not include cartesian state

   // Get C derivative vector
   Rvector dVector;
   GetCDerivativeVector(forObj, dVector, "Cd", current);

   // Get Cr partial derivative
   Real deriv = dVector[CdIndex];

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("AngleAdapterDeg::GetCdDerivative(%s, %p):  deriv = %.14lg\n",
      forObj->GetName().c_str(), current, deriv);
#endif
   return deriv;
}


Real AngleAdapterDeg::GetADSFDerivative(GmatBase *forObj, SignalData *current)
{
   // Get index for Cd
   StringArray stmElemNames = forObj->GetStringArrayParameter("StmElementNames");
   UnsignedInt AdsfIndex = 0;
   bool found = false;
   for (int i = 0; i < stmElemNames.size(); i++)
   {
      if (stmElemNames.at(i) == "CartesianState" || stmElemNames.at(i) == "KeplerianState")
         AdsfIndex += 6;
      else if (stmElemNames.at(i) == "AtmosDensityScaleFactor")
      {
         found = true;
         break;
      }
      else
      {
         AdsfIndex += forObj->GetEstimationParameterSize(forObj->GetParameterID(stmElemNames.at(i)));
      }
   }

   if (!found)
      return 0.0;

   AdsfIndex -= 6;  // vector that GetCDerivativeVector() returns does not include cartesian state

   // Get C derivative vector
   Rvector dVector;
   GetCDerivativeVector(forObj, dVector, "AtmosDensityScaleFactor", current);

   // Get ADSF partial derivative
   Real deriv = dVector[AdsfIndex];

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("AngleAdapterDeg::GetCdDerivative(%s, %p):  deriv = %.14lg\n",
      forObj->GetName().c_str(), current, deriv);
#endif
   return deriv;
}


// Added for Thrust Scale Factor Solve For
Real AngleAdapterDeg::GetTSFDerivative(GmatBase *forObj, const std::string &paramName,
   SignalData *current)
{
   // paramName looks like <segmentName>.TSF_Epsilon
   StringArray parts = GmatStringUtil::SeparateBy(paramName, ".");
   std::string tsfName = parts.at(0) + ".ThrustScaleFactor";

   // get index for TSF
   StringArray stmElemNames = forObj->GetStringArrayParameter("StmElementNames");
   UnsignedInt TSFIndex = 0;
   bool found = false;
   for (int i = 0; i < stmElemNames.size(); ++i)
   {
      if (stmElemNames.at(i) == "CartesianState" || stmElemNames.at(i) == "KeplerianState")
         TSFIndex += 6;
      else if (stmElemNames.at(i) == tsfName)
      {
         found = true;
         break;
      }
      else
      {
         TSFIndex += forObj->GetEstimationParameterSize(forObj->GetParameterID(stmElemNames.at(i)));
      }
   }

   if (!found)
      return 0.0;

   TSFIndex -= 6;  // vector that GetCDerivativeVector() returns does not include cartesian state

   // Get C derivative vector
   Rvector dVector;
   GetCDerivativeVector(forObj, dVector, "TSF", current);

   // Get TSF partial derivative
   Real deriv = dVector[TSFIndex];

#ifdef DEBUG_ADAPTER_DERIVATIVES
   MessageInterface::ShowMessage("AngleAdapterDeg::GetTSFDerivative(%s, %p):  deriv = %.14lg\n",
      forObj->GetName().c_str(), current, deriv);
#endif
   return deriv;
}


void AngleAdapterDeg::GetCDerivativeVector(GmatBase *forObj, Rvector &deriv,
   const std::string &solveForType, SignalData *current)
{
   // TBD - I think this is overkill, I think forObj is always rNode
   // 1. Calculate phi matrix
   bool forTransmitter = true;
   if (current->rNode == forObj)
      forTransmitter = false;
   else
   {
      if (current->tNode != forObj)
         throw MeasurementException(solveForType + " derivative requested, but "
            "neither participant is the \"for\" object");
   }
   Rmatrix phi = (forTransmitter ? (current->tSTM * current->tSTMtm.Inverse()) :
      (current->rSTM * current->rSTMtm.Inverse()));


   // 2. Calculate E matrix
   Integer m = phi.GetNumColumns() - 6;
   Rmatrix E(3, m);
   for (Integer i = 0; i < 3; ++i)
      for (Integer j = 0; j < m; ++j)
         E(i, j) = phi(i, j + 6);


   //// 3. Calculate: sign * R * phi
   //Real sign = (forTransmitter ? -1.0 : 1.0);

   //Rmatrix33 body2FK5_matrix = (forTransmitter ? current->tJ2kRotation :
   //   current->rJ2kRotation);
   //Rmatrix tempMatrix(3, m);
   //// tempMatrix = body2FK5_matrix * E;
   //for (Integer i = 0; i < 3; ++i)
   //{
   //   for (Integer j = 0; j < m; ++j)
   //   {
   //      tempMatrix(i, j) = 0.0;
   //      for (Integer k = 0; k < 3; ++k)
   //         tempMatrix(i, j) += sign*body2FK5_matrix(i, k) * E(k, j);
   //   }
   //}


   // 4. Calculate derivative of measurement w.r.t. position (inertial frame at transmit time t2)
   Rvector6 dMeasDPosvel = CalcDerivValue();
   Rvector3 dMeasDPos = dMeasDPosvel.GetR();


   // 5. Calculate C vector derivative
   deriv.SetSize(m);
   for (Integer j = 0; j < m; ++j)
   {
      deriv[j] = 0.0;
      for (Integer i = 0; i < 3; ++i)
         //deriv[j] += dMeasDPos[i] * tempMatrix(i, j);
         deriv[j] += dMeasDPos[i] * E(i, j);
   }
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
bool AngleAdapterDeg::WriteMeasurements()
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
bool AngleAdapterDeg::WriteMeasurement(Integer id)
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
Integer AngleAdapterDeg::HasParameterCovariances(Integer parameterId)
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
Integer AngleAdapterDeg::GetEventCount()
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
void AngleAdapterDeg::SetCorrection(const std::string& correctionName,
   const std::string& correctionType)
{
   if (correctionName == "Aberration")
   {
      if (correctionType == "Aberration-None") {
         useAnnual = useDiurnal = false;
      }
      else if (correctionType == "Aberration-Annual") {
         useAnnual = true;
         useDiurnal = false;
      }
      else if (correctionType == "Aberration-Diurnal") {
         useAnnual = false;
         useDiurnal = true;
      }
      else if (correctionType == "Aberration-AnnualAndDiurnal") {
         useAnnual = useDiurnal = true;
      }
      return;
   }

   TrackingDataAdapter::SetCorrection(correctionName, correctionType);
}


SignalData *AngleAdapterDeg::GetLastSignalData()
{
   std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   // In the current version of GmatEstimation plugin, it has only 1 signal path. The code has to be modified for multiple signal paths 
   SignalBase *currentleg = paths[0]; 
   SignalData *current = ((currentleg == NULL) ? NULL : (currentleg->GetSignalDataObject()));

   // get SignalData of last leg
   while (currentleg != NULL)
   {
      current = currentleg->GetSignalDataObject();
      currentleg = currentleg->GetNext();
   }

   return current;
}
