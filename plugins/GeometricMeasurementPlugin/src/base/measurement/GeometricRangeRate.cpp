//$Id: GeometricRangeRate.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         GeometricRangeRate
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
// Author: Darrel J. Conway, Thinking Systems, Inc./Wendy Shoan, GSFC/GSSB
// Created: 2009.08.11
//
/**
 * Implementation of the geometric range rate measurement.
 */
//------------------------------------------------------------------------------


#include "GeometricRangeRate.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"

#include "Rvector3.hpp"


//#define DEBUG_RANGE_RATE_CALC
//#define SHOW_RANGE_RATE_CALC
//#define DEBUG_DERIVATIVES


//------------------------------------------------------------------------------
// GeometricRangeRate(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name Name of the measurement (if any)
 */
//------------------------------------------------------------------------------
GeometricRangeRate::GeometricRangeRate(const std::string &name) :
   CoreMeasurement          ("GeometricRangeRate", name)
{
   objectTypeNames.push_back("GeometricRangeRate");

   // Prep value array in measurement
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "RangeRate";
   currentMeasurement.type = Gmat::GEOMETRIC_RANGE_RATE;
//   parameterCount = GeometricGeometricRangeRateParamCount;
}


GeometricRangeRate::~GeometricRangeRate()
{
}


GeometricRangeRate::GeometricRangeRate(const GeometricRangeRate &rrm) :
   CoreMeasurement          (rrm)
{
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "RangeRate";
   currentMeasurement.type = Gmat::GEOMETRIC_RANGE_RATE;
   currentMeasurement.uniqueID = rrm.currentMeasurement.uniqueID;
   // assuming two participants
   currentMeasurement.participantIDs.push_back("NotSet");
   currentMeasurement.participantIDs.push_back("NotSet");
}


GeometricRangeRate& GeometricRangeRate::operator=(const GeometricRangeRate &rrm)
{
   if (&rrm != this)
   {
      CoreMeasurement::operator=(rrm);

      // Allocate exactly one value in current measurement for range
      currentMeasurement.value.clear();
      currentMeasurement.value.push_back(0.0);
      currentMeasurement.typeName = "RangeRate";
      currentMeasurement.type = Gmat::GEOMETRIC_RANGE_RATE;
      currentMeasurement.uniqueID = rrm.currentMeasurement.uniqueID;
   }

   return *this;
}


GmatBase* GeometricRangeRate::Clone() const
{
   #ifdef DEBUG_RANGE_RATE_CALC
      MessageInterface::ShowMessage("Entered GeometricRangeRate::Clone() "
            "with %d participants; this = %p\n", participants.size(), this);
   #endif
   GmatBase *retval =  new GeometricRangeRate(*this);
   #ifdef DEBUG_RANGE_RATE_CALC
      MessageInterface::ShowMessage("   clone address is %p\n", retval);
   #endif
   return retval;
}


bool GeometricRangeRate::Initialize()
{
#ifdef DEBUG_RANGE_RATE_CALC
   MessageInterface::ShowMessage("Entered GeometricRangeRate::Initialize()"
         "; this = %p\n", this);
#endif

bool retval = false;

if (CoreMeasurement::Initialize())
   retval = true;

#ifdef DEBUG_RANGE_RATE_CALC
   MessageInterface::ShowMessage("   Initialization %s with %d "
         "participants\n", (retval ? "succeeded" : "failed"),
         participants.size());
#endif

return retval;
}


bool GeometricRangeRate::Evaluate(bool withEvents)
{
   #ifdef DEBUG_RANGE_RATE_CALC
      MessageInterface::ShowMessage("Entered GeometricRangeRate::Evaluate()\n");
      MessageInterface::ShowMessage("  ParticipantCount: %d\n",
            participants.size());
   #endif
        
   if (!initialized) InitializeMeasurement();
   
   CalculateRangeRateVectorObs();
   
   // @todo waiting for updated specs from Steve for this part *******************
   currentMeasurement.feasibilityValue = rangeVecInertial * p1Loc; 
   if (currentMeasurement.feasibilityValue > 0.0)
   {
      currentMeasurement.isFeasible = true;
      Rvector3 rangeUnit = rangeVecObs.GetUnitVector();
      currentMeasurement.value[0] = rangeRateVecObs * rangeUnit;
   }
   else
   {
      currentMeasurement.isFeasible = false;
      currentMeasurement.value[0] = 0.0; // ******************************
   }
   
   #ifdef DEBUG_RANGE_RATE_CALC
      MessageInterface::ShowMessage("Calculating Geometric Range Rate at epoch "
            "%.12lf\n", currentMeasurement.epoch);
      MessageInterface::ShowMessage("   Location of %s, id = '%s':  %s",
            participants[0]->GetName().c_str(),
            currentMeasurement.participantIDs[0].c_str(),
            p1Loc.ToString().c_str());
      MessageInterface::ShowMessage("   Location of %s, id = '%s':  %s",
            participants[1]->GetName().c_str(),
            currentMeasurement.participantIDs[1].c_str(),
            p2Loc.ToString().c_str());
      MessageInterface::ShowMessage("   Range Vector (Inertial):  %s\n",
            rangeVecInertial.ToString().c_str());
      MessageInterface::ShowMessage("   Range Vector (Observation):  %s\n",
            rangeVecObs.ToString().c_str());
      MessageInterface::ShowMessage("   Range Rate vector (Observation):  %s\n",
            rangeRateVecObs.ToString().c_str());
      MessageInterface::ShowMessage("   R(Groundstation) dot RangeVec =  %lf\n", // ******** ??
            currentMeasurement.feasibilityValue);
      MessageInterface::ShowMessage("   Feasibility:  %s\n",
            (currentMeasurement.isFeasible ? "true" : "false"));
      MessageInterface::ShowMessage("   Range Rate is %.12lf\n",
            currentMeasurement.value[0]);
   #endif
   
   #ifdef SHOW_RANGE_RATE_CALC
      MessageInterface::ShowMessage("Range Rate at epoch %.12lf is ",
            currentMeasurement.epoch);
      if (currentMeasurement.isFeasible)
         MessageInterface::ShowMessage("feasible, value = %.12lf\n",
            currentMeasurement.value[0]);
      else
         MessageInterface::ShowMessage("not feasible\n");
   #endif
   
   return true;
}

const std::vector<RealArray>& GeometricRangeRate::CalculateMeasurementDerivatives(GmatBase *obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("GeometricRangeRate::CalculateMeasurement"
            "Derivatives(%s, %d) called\n", obj->GetName().c_str(), id);
   #endif
      
   if (!initialized) InitializeMeasurement();
   
   GmatBase *objPtr = NULL;
   
   Integer size = obj->GetEstimationParameterSize(id);
   Integer objNumber = -1;
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   ParameterSize = %d\n", size);
   #endif
   
   if (size <= 0)
      throw MeasurementException("The derivative parameter on derivative "
            "object " + obj->GetName() + "is not recognized");
   
   // Check to see if obj is a participant
   for (UnsignedInt i = 0; i < this->participants.size(); ++i)
   {
      if (participants[i] == obj)
      {
         objPtr = participants[i];
         objNumber = i + 1;
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Participant %s found\n",
                  objPtr->GetName().c_str());
         #endif
         break;
      }
   }
   
   // Or if it is the measurement model for this object
   if (obj->IsOfType(Gmat::MEASUREMENT_MODEL))
      if (obj->GetRefObject(Gmat::CORE_MEASUREMENT, "") == this)
      {
         objPtr = obj;
         objNumber = 0;
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   The measurement is the object\n",
                  objPtr->GetName().c_str());
         #endif
      }
   if (objNumber == -1)
      throw MeasurementException(
            "GeometricRangeRate error - object is neither participant nor measurement model.");
   
   RealArray oneRow;
   oneRow.assign(size, 0.0);
   currentDerivatives.clear();
   currentDerivatives.push_back(oneRow);
   
   Integer parameterID = GetParmIdFromEstID(id, obj);
   
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   Looking up id %d\n", parameterID);
   #endif
   
   if (objPtr != NULL)
   {
      if (objNumber == 1)
      {
         Rvector3  tmp, tmpVel;
         Rvector6  result;  
         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            CalculateRangeRateVectorObs();
            Rvector3  rangeUnit     = rangeVecObs.GetUnitVector();
            Real      range         = rangeVecInertial.GetMagnitude();
            Rvector3  pd1 = rangeRateVecObs / range;
            Rmatrix   po( 1,3,rangeUnit[0], rangeUnit[1], rangeUnit[2]); 
            Rmatrix   poT(3,1,rangeUnit[0], rangeUnit[1], rangeUnit[2]);
            if (stationParticipant)
            {
               tmp    = - pd1 * ((I33 - po * poT) * R_o_1) - (rangeUnit * RDot_o_1);
            }
            else
            {
               tmp    = - pd1 * (I33 - po * poT);
            }
            for (UnsignedInt ii = 0; ii < 3; ii++)
               currentDerivatives[0][ii] = tmp[ii];
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            CalculateRangeRateVectorObs();
            Rvector3  rangeUnit     = rangeVecObs.GetUnitVector();
            if (stationParticipant)
            {
               tmpVel = - rangeUnit * R_o_1;
            }
            else
            {
               tmpVel = - rangeUnit;
            }
            for (UnsignedInt ii = 0; ii < 3; ii++)
               currentDerivatives[0][ii] = tmpVel[ii];
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            CalculateRangeRateVectorObs();
            Rvector3  rangeUnit     = rangeVecObs.GetUnitVector();
            Real      range         = rangeVecInertial.GetMagnitude();
            Rvector3  pd1 = rangeRateVecObs / range;
            Rmatrix   po( 1,3,rangeUnit[0], rangeUnit[1], rangeUnit[2]); 
            Rmatrix   poT(3,1,rangeUnit[0], rangeUnit[1], rangeUnit[2]);
            if (stationParticipant)
            {
               tmp    = - pd1 * ((I33 - po * poT) * R_o_1) - (rangeUnit * RDot_o_1);
               tmpVel = - rangeUnit * R_o_1;
            }
            else
            {
               tmp    = - pd1 * (I33 - po * poT);
               tmpVel = - rangeUnit;
            }
            for (UnsignedInt ii = 0; ii < 3; ii++)
            {
               currentDerivatives[0][ii]   = tmp[ii];
               currentDerivatives[0][ii+3] = tmpVel[ii];
            }
         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i) currentDerivatives[0][i] = 1.0;
         }
         else
         {
            for (UnsignedInt i = 0; i < 3; ++i)  currentDerivatives[0][i] = 0.0;
         }
      }
      else if (objNumber == 2)
      {
         Rvector3  tmp, tmpVel;
         Rvector6  result;  
         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            CalculateRangeRateVectorObs();
            Rvector3  rangeUnit     = rangeVecObs.GetUnitVector();
            Real      range         = rangeVecInertial.GetMagnitude();
            Rvector3  pd1 = rangeRateVecObs / range;
            Rmatrix   po( 1,3,rangeUnit[0], rangeUnit[1], rangeUnit[2]); 
            Rmatrix   poT(3,1,rangeUnit[0], rangeUnit[1], rangeUnit[2]);
            if (stationParticipant)
            {
               tmp    = pd1 * ((I33 - po * poT) * R_o_2) + (rangeUnit * RDot_o_2);
            }
            else
            {
               tmp    = pd1 * (I33 - po * poT);
            }
            for (UnsignedInt ii = 0; ii < 3; ii++)
               currentDerivatives[0][ii] = tmp[ii];
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            CalculateRangeRateVectorObs();
            Rvector3  rangeUnit     = rangeVecObs.GetUnitVector();
            if (stationParticipant)
            {
               tmpVel = rangeUnit * R_o_2;
            }
            else
            {
               tmpVel = rangeUnit;
            }
            for (UnsignedInt ii = 0; ii < 3; ii++)
               currentDerivatives[0][ii] = tmpVel[ii];
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            CalculateRangeRateVectorObs();
            Rvector3  rangeUnit     = rangeVecObs.GetUnitVector();
            Real      range         = rangeVecInertial.GetMagnitude();
            Rvector3  pd1 = rangeRateVecObs / range;
            Rmatrix   po( 1,3,rangeUnit[0], rangeUnit[1], rangeUnit[2]); 
            Rmatrix   poT(3,1,rangeUnit[0], rangeUnit[1], rangeUnit[2]);
            if (stationParticipant)
            {
               tmp    = pd1 * ((I33 - po * poT) * R_o_2) + (rangeUnit * RDot_o_2);
               tmpVel = rangeUnit * R_o_2;
            }
            else
            {
               tmp    = pd1 * (I33 - po * poT);
               tmpVel = rangeUnit;
            }
            for (UnsignedInt ii = 0; ii < 3; ii++)
            {
               currentDerivatives[0][ii]   = tmp[ii];
               currentDerivatives[0][ii+3] = tmpVel[ii];
            }
         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i) currentDerivatives[0][i] = 1.0;
         }
         else
         {
            for (UnsignedInt i = 0; i < 3; ++i)  currentDerivatives[0][i] = 0.0;
         }
      }
      else if (objNumber == 0) // measurement model
      {
         if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i) currentDerivatives[0][i] = 1.0;
         }
      }
      else
      {
         for (UnsignedInt i = 0; i < 3; ++i)  currentDerivatives[0][i] = 0.0;
      }

   }
   return currentDerivatives;
}
