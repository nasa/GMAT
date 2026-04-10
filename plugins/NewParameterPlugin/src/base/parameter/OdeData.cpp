//$Id: OdeData.cpp 9811 2011-08-29 21:15:18Z wendys-dev $
//------------------------------------------------------------------------------
//                                  OdeData
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28.
//
// Author: Darrel Conway, Thinking Systems, Inc
// Created: 2013/05/24  (Based on EnvData)
//
/**
 * Implements class which provides data from a force model, such as
 * acceleration, atmospheric density, current or average F10.7 value,
 * relativistic correction, etc.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "OdeData.hpp"
#include "ParameterException.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

#include "DragForce.hpp"

//#define DEBUG_ODEDATA_INIT 1
//#define DEBUG_ODEDATA_RUN  1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const Real OdeData::ODE_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;

const std::string
OdeData::VALID_OBJECT_TYPE_LIST[OdeDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem",
   "ODEModel"
}; 

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OdeData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
OdeData::OdeData() :
   RefData        (),
   mSpacecraft    (NULL),
   mSolarSystem   (NULL),
   mModel         (NULL),
   transients     (NULL)
{
}


//------------------------------------------------------------------------------
// OdeData(const OdeData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param copy The OdeData object being copied.
 */
//------------------------------------------------------------------------------
OdeData::OdeData(const OdeData &copy) :
   RefData        (copy),
   mSpacecraft    (copy.mSpacecraft),
   mSolarSystem   (copy.mSolarSystem),
   mModel         (copy.mModel),
   transients     (copy.transients)
{
}


//------------------------------------------------------------------------------
// OdeData& operator= (const OdeData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param right the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
OdeData& OdeData::operator= (const OdeData &right)
{
   if (this != &right)
   {
      RefData::operator=(right);

      mSpacecraft = right.mSpacecraft;
      mSolarSystem = right.mSolarSystem;
      mModel = right.mModel;
      transients = right.transients;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~OdeData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OdeData::~OdeData()
{
   //MessageInterface::ShowMessage("==> OdeData::~OdeData()\n");
}


//------------------------------------------------------------------------------
// Real GetEnvReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrieves requested data for the current epoch and location of the Spacecraft
 *
 * @param str The string identifying the desired data
 *
 * @return The value for the requested data
 */
//------------------------------------------------------------------------------
Real OdeData::GetOdeReal(const std::string &str)
{
   //-------------------------------------------------------
   // 1. Get Spacecraft's central body (It's done in InitializeRefObjects())
   // 2. Get current time (use parameter or sc pointer?)
   // 3. Get spacecraft's position (use parameter?)
   // 4. Call GetDensity() on central body
   //-------------------------------------------------------
   
   if (mSpacecraft == NULL || mSolarSystem == NULL || mModel == NULL)
      InitializeRefObjects();

   if (str == "FMDensity")
   {
      Real density = 0.0;
      
      // Call GetDensity() on if origin is CelestialBody
      if ((mModel != NULL) && (mModel->IsOfType(Gmat::ODE_MODEL)))
      {
         DragForce *df = (DragForce*)((ODEModel*)mModel)->GetForceOfType("DragForce", "");
         if (df != NULL)
         {
            Real epoch  = mSpacecraft->GetEpoch();
            GmatTime epochgt = mSpacecraft->GetEpochGT();

            // It converts spacecraft j2kbodyMJ2000Eq to bodyMJ2000Eq where body is the planet containing the amtmosphere
            // (for example: body of MarsGRAM atmosphere model is Mars)   
            //Real *state = mSpacecraft->GetState().GetState();
            Real *j2kState = mSpacecraft->GetState().GetState();
            Real state[6];

            if (mSpacecraft->HasPrecisionTime())
               df->BuildModelStateGT(epochgt, state, j2kState);
            else
               df->BuildModelState(epoch, state, j2kState);

            #ifdef DEBUG_ODEDATA_RUN
               MessageInterface::ShowMessage("Getting density for epoch %.12lf and state [%.8lf %.8lf %.8lf %.12lf %.12lf %.12lf]; ",
                  epoch, state[0], state[1], state[2], state[3], state[4], state[5]);
            #endif

            // Return density in kg/km^3; the force uses kg/m^3, so scale by 1e9
            if (df->HasPrecisionTime())
               density = df->GetDensity(state, epochgt.GetMjd(), 1) * 1.0e9;
            else
               density = df->GetDensity(state, epoch, 1) * 1.0e9;

            #ifdef DEBUG_ODEDATA_RUN
               MessageInterface::ShowMessage("Density = %.12le\n", density);
            #endif
         }
      }
      
      return density;
   }

   if ((str == "FMAcceleration")  || (str == "FMAccelerationX") ||
       (str == "FMAccelerationY") || (str == "FMAccelerationZ"))
   {
      Real acceleration = 0.0;

      if ((mModel != NULL) && (mModel->IsOfType(Gmat::ODE_MODEL)))
      {
         #ifdef DEBUG_ODEDATA_RUN
            MessageInterface::ShowMessage("ODEModel %s Found\n",
                  mModel->GetName().c_str());
         #endif

         #ifdef DEBUG_TRANSIENT_FORCES
            if (transients == NULL)
               MessageInterface::ShowMessage("   transients pointer: NULL\n");
            else
               MessageInterface::ShowMessage("   transients pointer: %p\n",
                     transients);
         #endif

         // If a transient is active for the spacecraft, add to force model
         bool hasTransients = false;
         std::vector<PhysicalModel*> forcesAdded;
         if (transients != NULL)
         {
            for (std::vector<PhysicalModel*>::iterator fpp =
                  transients->begin(); fpp != transients->end(); ++fpp)
            {
               PhysicalModel *candidate = *fpp;

               StringArray sats =
                     candidate->GetRefObjectNameArray(Gmat::SPACECRAFT);
               if (find(sats.begin(), sats.end(), mSpacecraft->GetName()) !=
                     sats.end())
               {
                  hasTransients = true;
                  ((ODEModel*)mModel)->AddForce(candidate);
                  forcesAdded.push_back(candidate);
               }
            }
         }

         // Now actually build the parameter data
         Rvector6 deriv = mModel->GetDerivativesForSpacecraft(mSpacecraft);

         if (str == "FMAcceleration")
            // Return the magnitude of the acceleration
            acceleration = sqrt(deriv[3]*deriv[3] + deriv[4]*deriv[4] +
                  deriv[5]*deriv[5]);
         if (str == "FMAccelerationX")
            acceleration = deriv[3];
         if (str == "FMAccelerationY")
            acceleration = deriv[4];
         if (str == "FMAccelerationZ")
            acceleration = deriv[5];

         // Remove any transient forces that were added
         if (hasTransients)
         {
            for (UnsignedInt l = 0; l < forcesAdded.size(); ++l)
            {
               ((ODEModel*)mModel)->DeleteForce(forcesAdded[l]);
            }
         }
      }

      return acceleration;
   }
   else
   {
      throw ParameterException("OdeData::GetEnvReal() Unknown "
            "parameter name: " + str);
   }
}

//------------------------------------------------------------------------------
// Real GetOdeRvec3(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrieves requested Rvector3 data for the current epoch and location of the
 * Spacecraft
 *
 * @param str The string identifying the desired data
 *
 * @return The Rvector3 for the requested data
 */
//------------------------------------------------------------------------------
Rvector3 OdeData::GetOdeRvec3(const std::string &str)
{
   if (str == "BurnTorque" || str == "GravityTorque" || str == "SRPTorque" ||
      str == "TotalTorque")
   {
      Rvector3 torque = Rvector3::RVECTOR3_UNDEFINED;
      if ((mModel != NULL) && (mModel->IsOfType(Gmat::ODE_MODEL)))
      {
         // If a transient is active for the spacecraft, add to force model
         bool hasTransients = false;
         std::vector<PhysicalModel*> forcesAdded;
         if (transients != NULL)
         {
            for (std::vector<PhysicalModel*>::iterator fpp =
               transients->begin(); fpp != transients->end(); ++fpp)
            {
               PhysicalModel *candidate = *fpp;

               StringArray sats =
                  candidate->GetRefObjectNameArray(Gmat::SPACECRAFT);
               if (find(sats.begin(), sats.end(), mSpacecraft->GetName()) !=
                  sats.end())
               {
                  hasTransients = true;
                  ((ODEModel*)mModel)->AddForce(candidate);
                  forcesAdded.push_back(candidate);
               }
            }
         }

         // Get the torques
         torque = ((ODEModel*)mModel)->GetTorquesForSpacecraft(mSpacecraft, str);

         // Remove any transient forces that were added
         if (hasTransients)
         {
            for (UnsignedInt l = 0; l < forcesAdded.size(); ++l)
            {
               ((ODEModel*)mModel)->DeleteForce(forcesAdded[l]);
            }
         }
      }
      return torque;
   }
   else
   {
      throw ParameterException("OdeData::GetOdeRvec3() Unknown "
         "parameter name: " + str);
   }
}

//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* OdeData::GetValidObjectList() const
{
   return VALID_OBJECT_TYPE_LIST;
}


//------------------------------------------------------------------------------
// bool ValidateRefObjects(GmatBase *param)
//------------------------------------------------------------------------------
/**
 * Validates reference objects for given parameter
 */
//------------------------------------------------------------------------------
bool OdeData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<OdeDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == OdeDataObjectCount)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void OdeData::InitializeRefObjects()
{
   #if DEBUG_ODEDATA_INIT
   MessageInterface::ShowMessage("OdeData::InitializeRefObjects() entered.\n");
   #endif
   
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   if (mSpacecraft == NULL)
      throw ParameterException
         ("OdeData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set to any unnamed parameters\n");
   
   mSolarSystem = (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   if (mSolarSystem == NULL)
      throw ParameterException
         ("OdeData::InitializeRefObjects() Cannot find SolarSystem object\n");

   // set force model
   std::string modelName =
      FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[PHYSICAL_MODEL]));
   if (modelName != "")
   {
      #if DEBUG_ODEDATA_INIT
         MessageInterface::ShowMessage
            ("OdeData::InitializeRefObjects() getting modelName:%s pointer.\n",
             modelName.c_str());
      #endif
         
      mModel =
         (ODEModel*)FindFirstObject(VALID_OBJECT_TYPE_LIST[PHYSICAL_MODEL]);
      
      if (!mModel)
         throw ParameterException
            ("OdeData::InitializeRefObjects() parameter dependent force model "
             "not found in the Configuration: " + modelName + "\n");

   }
   if (mModel == NULL)
      throw ParameterException
         ("OdeData::InitializeRefObjects() Cannot find ODE Model object\n");

}


//------------------------------------------------------------------------------
// virtual bool IsValidObjectType(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 *
 * @return return true if object is valid object, false otherwise
 */
//------------------------------------------------------------------------------
bool OdeData::IsValidObjectType(UnsignedInt type)
{
   for (int i=0; i<OdeDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;
}

