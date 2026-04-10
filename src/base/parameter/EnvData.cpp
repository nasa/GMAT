//$Id$
//------------------------------------------------------------------------------
//                                  EnvData
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/12/07
//
/**
 * Implements class which provides environmental data, such as atmospheric density,
 * solar flux, etc.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "EnvData.hpp"
#include "ParameterException.hpp"
#include "CelestialBody.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ENVDATA_INIT 1
//#define DEBUG_ENVDATA_RUN 1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const Real EnvData::ENV_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;

const std::string
EnvData::VALID_OBJECT_TYPE_LIST[EnvDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem",
   "SpacePoint"
}; 

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// EnvData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
EnvData::EnvData()
   : RefData()
{
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mOrigin = NULL;
}


//------------------------------------------------------------------------------
// EnvData(const EnvData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the EnvData object being copied.
 */
//------------------------------------------------------------------------------
EnvData::EnvData(const EnvData &copy)
   : RefData(copy)
{
   mSpacecraft = copy.mSpacecraft;
   mSolarSystem = copy.mSolarSystem;
   mOrigin = copy.mOrigin;
}


//------------------------------------------------------------------------------
// EnvData& operator= (const EnvData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
EnvData& EnvData::operator= (const EnvData &right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
   
   mSpacecraft = right.mSpacecraft;
   mSolarSystem = right.mSolarSystem;
   mOrigin = right.mOrigin;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~EnvData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EnvData::~EnvData()
{
   //MessageInterface::ShowMessage("==> EnvData::~EnvData()\n");
}


//------------------------------------------------------------------------------
// Real GetEnvReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives atmospheric density where spacecraft is at.
 */
//------------------------------------------------------------------------------
Real EnvData::GetEnvReal(const std::string &str)
{
   //-------------------------------------------------------
   // 1. Get Spacecraft's central body (It's done in InitializeRefObjects())
   // 2. Get current time (use parameter or sc pointer?)
   // 3. Get spacecraft's position (use parameter?)
   // 4. Call GetDensity() on central body
   //-------------------------------------------------------
   
   if (mSpacecraft == NULL || mSolarSystem == NULL || mOrigin == NULL)
      InitializeRefObjects();

   if (str == "AtmosDensity")
   {
      Real a1mjd = mSpacecraft->GetRealParameter("A1Epoch");
      Rvector6 cartState = mSpacecraft->GetState().GetState();
      
//       // Rvector6 cartState = mSpacecraft->GetStateVector("Cartesian");
//       Rvector6 cartState = mSpacecraft->GetState(0);
//       Real state[6];
//       for (int i=0; i<6; i++)
//          state[i] = cartState[i];
      Real *state = (Real*)cartState.GetDataVector();
      Real density = 0.0;
      
      // Call GetDensity() on if origin is CelestialBody
      if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      {
         if (((CelestialBody*)mOrigin)->GetDensity(state, &density, a1mjd, 1))
         {
            #ifdef DEBUG_ENVDATA_RUN
            MessageInterface::ShowMessage
               ("EnvData::GetEnvReal() mOrigin=%s, a1mjd=%f, state=%s, density=%g\n",
                mOrigin->GetName().c_str(), a1mjd, cartState.ToString().c_str(), density);
            #endif
         }
         else
         {
            #ifdef DEBUG_ENVDATA_RUN
            MessageInterface::ShowMessage
               ("EnvData::GetEnvReal() AtmosphereModel used for %s is NULL\n",
                mOrigin->GetName().c_str());
            #endif
         }
      }
      
      return density;
   }
   else
   {
      throw ParameterException("EnvData::GetEnvReal() Unknown parameter name: " +
                               str);
   }
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* EnvData::GetValidObjectList() const
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
bool EnvData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<EnvDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == EnvDataObjectCount)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void EnvData::InitializeRefObjects()
{
   #if DEBUG_ENVDATA_INIT
   MessageInterface::ShowMessage("EnvData::InitializeRefObjects() entered.\n");
   #endif
   
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);

   if (mSpacecraft == NULL)
      throw ParameterException
         ("EnvData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set to any unnamed parameters\n");
   
   mSolarSystem = (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   if (mSolarSystem == NULL)
      throw ParameterException
         ("EnvData::InitializeRefObjects() Cannot find SolarSystem object\n");

   // set origin
   std::string originName =
      FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[SPACE_POINT]));

   if (originName != "")
   {
      #if DEBUG_ENVDATA_INIT
         MessageInterface::ShowMessage
            ("EnvData::InitializeRefObjects() getting originName:%s pointer.\n",
             originName.c_str());
      #endif
         
      mOrigin =
         (CelestialBody*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACE_POINT]);
      
      if (!mOrigin)
         throw ParameterException
            ("EnvData::InitializeRefObjects() parameter dependent body not "
             "found in the SolarSystem: " + originName + "\n");

   }
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
bool EnvData::IsValidObjectType(UnsignedInt type)
{
   for (int i=0; i<EnvDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}

