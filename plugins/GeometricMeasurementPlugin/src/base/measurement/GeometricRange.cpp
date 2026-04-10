//$Id: GeometricRange.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         GeometricRange
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
// Created: 2009/06/29
//
/**
 * Implementation of the geometric range measurement.
 */
//------------------------------------------------------------------------------


#include "GeometricRange.hpp"
#include "GroundstationInterface.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"

#include "Rvector3.hpp"


//#define DEBUG_RANGE_CALC
//#define SHOW_RANGE_CALC
//#define DEBUG_DERIVATIVES


//------------------------------------------------------------------------------
// GeometricRange(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name (if any) for the new measurement
 */
//------------------------------------------------------------------------------
GeometricRange::GeometricRange(const std::string &name) :
   CoreMeasurement          ("GeometricRange", name)
{
   objectTypeNames.push_back("GeometricRange");

   // Prep value array in measurement
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "GeometricRange";
   currentMeasurement.type = Gmat::GEOMETRIC_RANGE;
//   parameterCount = GeometricGeometricRangeParamCount;

   covariance.SetDimension(1);
   covariance(0,0) = 1.0;
}


//------------------------------------------------------------------------------
// ~GeometricRange()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GeometricRange::~GeometricRange()
{
}


//------------------------------------------------------------------------------
// GeometricRange(const GeometricRange &rm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rm The measurement copied into the new one
 */
//------------------------------------------------------------------------------
GeometricRange::GeometricRange(const GeometricRange &rm) :
   CoreMeasurement          (rm)
{
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "GeometricRange";
   currentMeasurement.type = Gmat::GEOMETRIC_RANGE;
   currentMeasurement.uniqueID = rm.currentMeasurement.uniqueID;
   currentMeasurement.participantIDs.push_back("NotSet");
   currentMeasurement.participantIDs.push_back("NotSet");

   covariance = rm.covariance;
}


//------------------------------------------------------------------------------
// GeometricRange& operator=(const GeometricRange &rm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rm The measurement copied into this one
 *
 * @return this measurement set to look like rm.
 */
//------------------------------------------------------------------------------
GeometricRange& GeometricRange::operator=(const GeometricRange &rm)
{
   if (&rm != this)
   {
      CoreMeasurement::operator=(rm);

      // Allocate exactly one value in current measurement for range
      currentMeasurement.value.clear();
      currentMeasurement.value.push_back(0.0);
      currentMeasurement.typeName = "GeometricRange";
      currentMeasurement.type = Gmat::GEOMETRIC_RANGE;
      currentMeasurement.uniqueID = rm.currentMeasurement.uniqueID;

      covariance = rm.covariance;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Produces a clone of this measurement
 *
 * @return A clone created by the copy constructor
 */
//------------------------------------------------------------------------------
GmatBase* GeometricRange::Clone() const
{
   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Entered GeometricRange::Clone() "
            "with %d participants; this = %p\n", participants.size(), this);
   #endif
   GmatBase *retval =  new GeometricRange(*this);
   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("   clone address is %p\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the measurement for use in the mission control sequence.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool GeometricRange::Initialize()
{
   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Entered GeometricRange::Initialize()"
            "; this = %p\n", this);
   #endif

   bool retval = false;

   if (CoreMeasurement::Initialize())
      retval = true;

   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("   Initialization %s with %d "
            "participants\n", (retval ? "succeeded" : "failed"),
            participants.size());
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool Evaluate(bool withEvents)
//------------------------------------------------------------------------------
/**
 * Method used to evaluate the geometric range measurement
 *
 * @param withEvents Specifies if events should be included in the calculation.
 *                   This flag has no effect for geometric range.
 *
 * @return true if the measurement was calculated, false if not
 */
//------------------------------------------------------------------------------
bool GeometricRange::Evaluate(bool withEvents)
{
   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Entered GeometricRange::Evaluate()\n");
      MessageInterface::ShowMessage("  ParticipantCount: %d\n",
            participants.size());
   #endif
      
   if (!initialized) InitializeMeasurement();

   // Get minimum elevation angle for ground station
   Real minAngle;
   if (participants[0]->IsOfType(Gmat::SPACECRAFT) == false)
      minAngle = ((GroundstationInterface*)participants[0])->GetRealParameter("MinimumElevationAngle");
   else if (participants[1]->IsOfType(Gmat::SPACECRAFT) == false)
      minAngle = ((GroundstationInterface*)participants[1])->GetRealParameter("MinimumElevationAngle");

   CalculateRangeVectorInertial();
//   currentMeasurement.feasibilityValue = rangeVecInertial * p1Loc;
   UpdateRotationMatrix(currentMeasurement.epoch, "All");
   Rvector3 outState = (R_o_j2k * rangeVecInertial).GetUnitVector();
   currentMeasurement.feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;		// elevation angle in degree


   // @todo waiting for updated specs from Steve for this part
//   if (currentMeasurement.feasibilityValue > 0.0)
   if (withEvents == false)
   {
	   currentMeasurement.isFeasible = true;
	   currentMeasurement.value[0] = rangeVecInertial.GetMagnitude();
   }
   else
   {
	  if (currentMeasurement.feasibilityValue > minAngle)
	  {
         currentMeasurement.isFeasible = true;
		 currentMeasurement.value[0] = rangeVecInertial.GetMagnitude();
	  }
	  else
	  {
		 currentMeasurement.isFeasible = false;
		 currentMeasurement.value[0] = rangeVecInertial.GetMagnitude();
		 currentMeasurement.unfeasibleReason = "B1";
	  }
      
      
   }

   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Calculating Geometric Range at epoch "
            "%.12lf\n", currentMeasurement.epoch);
      MessageInterface::ShowMessage("   Location of %s, id = '%s':  %s",
            participants[0]->GetName().c_str(),
            currentMeasurement.participantIDs[0].c_str(),
            p1Loc.ToString().c_str());
      MessageInterface::ShowMessage("   Location of %s, id = '%s':  %s",
            participants[1]->GetName().c_str(),
            currentMeasurement.participantIDs[1].c_str(),
            p2Loc.ToString().c_str());
      MessageInterface::ShowMessage("   Range Vector:  %s\n",
            rangeVecInertial.ToString().c_str());
      MessageInterface::ShowMessage("   Elevation angle =  %lf degree\n",
            currentMeasurement.feasibilityValue);
      MessageInterface::ShowMessage("   Feasibility:  %s\n",
            (currentMeasurement.isFeasible ? "true" : "false"));
      MessageInterface::ShowMessage("   Geometric range is %.12lf\n",
            currentMeasurement.value[0]);
   #endif

   #ifdef SHOW_RANGE_CALC
      MessageInterface::ShowMessage("GeometricRange at epoch %.12lf is ",
            currentMeasurement.epoch);
      if (currentMeasurement.isFeasible)
         MessageInterface::ShowMessage("feasible, value = %.12lf\n",
            currentMeasurement.value[0]);
      else
         MessageInterface::ShowMessage("not feasible\n");
   #endif

   return true;
}


//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(
//                      GmatBase *obj, Integer id)
//------------------------------------------------------------------------------
/**
 * Calculates the derivatives with respect to the measurement state vector
 *
 * @param obj The object providing the state data
 * @param id The id of the vector component
 *
 * @return The resulting Jacobian matrix
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& GeometricRange::CalculateMeasurementDerivatives(
                     GmatBase *obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("GeometricRange::CalculateMeasurement"
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
            "GeometricRange error - object is neither participant nor "
            "measurement model.");

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
      if (objNumber == 1) // participant number 1, either a GroundStation or a Spacecraft
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of Participant"
                     " 1\n", objPtr->GetParameterText(parameterID).c_str());
         #endif
         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            CalculateRangeVectorInertial();
            Rvector3 tmp, result;   
            Rvector3 rangeUnit = rangeVecInertial.GetUnitVector();
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("   RVInertial      = %.12lf %.12lf %.12lf\n",
                        rangeVecInertial[0], rangeVecInertial[1], rangeVecInertial[2]);
               MessageInterface::ShowMessage("   Unit RVInertial = %.12lf %.12lf %.12lf ",
                        rangeUnit[0], rangeUnit[1], rangeUnit[2]);
            #endif
            if (stationParticipant)
            {
               for (UnsignedInt i = 0; i < 3; ++i)
                  tmp[i] = - rangeUnit[i];
               
               // for a Ground Station, need to rotate to the F1 frame
               result = tmp * R_j2k_1;
               for (UnsignedInt jj = 0; jj < 3; jj++)
                  currentDerivatives[0][jj] = result[jj];
            }
            else
            {
               // for a spacecraft participant 1, we don't need the rotation matrices (I33)
               for (UnsignedInt i = 0; i < 3; ++i)
                  currentDerivatives[0][i] = - rangeUnit[i];
            }
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            for (UnsignedInt i = 0; i < 3; ++i)
               currentDerivatives[0][i] = 0.0;
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            CalculateRangeVectorInertial();
            Rvector3 tmp, result;   
            Rvector3 rangeUnit = rangeVecInertial.GetUnitVector();
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("   RVInertial      = %.12lf %.12lf %.12lf\n",
                        rangeVecInertial[0], rangeVecInertial[1], rangeVecInertial[2]);
               MessageInterface::ShowMessage("   Unit RVInertial = %.12lf %.12lf %.12lf ",
                        rangeUnit[0], rangeUnit[1], rangeUnit[2]);
            #endif
            if (stationParticipant)
            {
               for (UnsignedInt i = 0; i < 3; ++i)
                  tmp[i] = - rangeUnit[i];
               
               // for a Ground Station, need to rotate to the F1 frame
               result = tmp * R_j2k_1;
               for (UnsignedInt jj = 0; jj < 3; jj++)
                  currentDerivatives[0][jj] = result[jj];
            }
            else
            {
               // for a spacecraft participant 1, we don't need the rotation matrices (I33)
               for (UnsignedInt i = 0; i < 3; ++i)
                  currentDerivatives[0][i] = - rangeUnit[i];
            }  
            // velocity all zeroes
            for (UnsignedInt ii = 3; ii < 6; ii++)
               currentDerivatives[0][ii] = 0.0;
         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
         else
         {
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("   Deriv is w.r.t. something "
                        "independent, so zero\n");
            #endif
            for (UnsignedInt i = 0; i < 3; ++i)
               currentDerivatives[0][i] = 0.0;
         }
   
      }
      else if (objNumber == 2) // participant 2, always a Spacecraft
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of Participant"
                     " 2\n", objPtr->GetParameterText(parameterID).c_str());
         #endif

         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            CalculateRangeVectorInertial();
            Rvector3 tmp, result;   
            Rvector3 rangeUnit = rangeVecInertial.GetUnitVector();
            if (stationParticipant)
            {
               for (UnsignedInt i = 0; i < 3; ++i)
                  tmp[i] = rangeUnit[i];
               
               // for a Ground Station, need to rotate to the F1 frame
               result = tmp * R_j2k_2;
               for (UnsignedInt jj = 0; jj < 3; jj++)
                  currentDerivatives[0][jj] = result[jj];
            }
            else
            {
               // for a spacecraft participant 1, we don't need the rotation matrices (I33)
               for (UnsignedInt i = 0; i < 3; ++i)
                  currentDerivatives[0][i] = rangeUnit[i];
            }
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
               for (UnsignedInt i = 0; i < 3; ++i)
                  currentDerivatives[0][i] = 0.0;
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            CalculateRangeVectorInertial();
            Rvector3 tmp, result;   
            Rvector3 rangeUnit = rangeVecInertial.GetUnitVector();
            if (stationParticipant)
            {
               for (UnsignedInt i = 0; i < 3; ++i)
                  tmp[i] = rangeUnit[i];
               
               // for a Ground Station, need to rotate to the F1 frame
               result = tmp * R_j2k_2;
               for (UnsignedInt jj = 0; jj < 3; jj++)
                  currentDerivatives[0][jj] = result[jj];
            }
            else
            {
               // for a spacecraft participant 1, we don't need the rotation matrices (I33)
               for (UnsignedInt i = 0; i < 3; ++i)
                  currentDerivatives[0][i] = rangeUnit[i];
            }  
            // velocity all zeroes
            for (UnsignedInt ii = 3; ii < 6; ii++)
               currentDerivatives[0][ii] = 0.0;
         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
         else
         {
            for (UnsignedInt i = 0; i < 3; ++i)
               currentDerivatives[0][i] = 0.0;
         }
   
      }
      else if (objNumber == 0) // measurement model
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of the "
                     "measurement model\n",
                     objPtr->GetParameterText(parameterID).c_str());
         #endif
         if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
      }
      else
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of a non-"
                     "Participant\n",
                     objPtr->GetParameterText(parameterID).c_str());
         #endif
         for (UnsignedInt i = 0; i < 3; ++i)
            currentDerivatives[0][i] = 0.0;
      }

      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   Deriv =\n   ");

         for (Integer i = 0; i < size; ++i)
            MessageInterface::ShowMessage("   %.12le",currentDerivatives[0][i]);
         MessageInterface::ShowMessage("\n");
      #endif
   }
   return currentDerivatives;
}
