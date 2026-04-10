//$Id$
//------------------------------------------------------------------------------
//                                  PlanetData
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
// Created: 2004/12/13
// Modified:
//   2005/6/10 Linda Jun - Moved BetaAngle to AngularParamters.cpp
//
/**
 * Implements class which provides planet related data, such as HourAngle.
 *   MHA, Longitude, Altitude(Geodetic), Latitude(Geodetic), LST
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "PlanetData.hpp"
#include "ParameterException.hpp"
#include "GmatConstants.hpp"
#include "CelestialBody.hpp"
#include "AngleUtil.hpp"          // for PutAngleInDegRange()
#include "Linear.hpp"             // for GmatRealUtil::ToString()
#include "CalculationUtilities.hpp"
#include "MessageInterface.hpp"

//#define __COMPUTE_LONGITUDE_OLDWAY__

//#define DEBUG_PLANETDATA_INIT
//#define DEBUG_PLANETDATA_RUN
//#define DEBUG_ALTITUDE

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const Real PlanetData::PLANET_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;

const std::string
PlanetData::VALID_OBJECT_TYPE_LIST[PlanetDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem",
   "CoordinateSystem",
   "SpacePoint"
}; 

const std::string PlanetData::VALID_PLANET_DATA_NAMES[LST_ID - LATITUDE + 1] =
{
   "Latitude",
   "Longitude",
   "Altitude",
   "MHA",
   "LST"
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// PlanetData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
PlanetData::PlanetData()
   : RefData()
{
   mCentralBodyName = "";
   
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mCentralBody = NULL;
   mOrigin = NULL;
   mInternalCoordSystem = NULL;
   mOutCoordSystem = NULL;
}


//------------------------------------------------------------------------------
// PlanetData(const PlanetData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the PlanetData object being copied.
 */
//------------------------------------------------------------------------------
PlanetData::PlanetData(const PlanetData &copy)
   : RefData(copy)
{
   mCentralBodyName = copy.mCentralBodyName;
   mSpacecraft = copy.mSpacecraft;
   mSolarSystem = copy.mSolarSystem;
   mCentralBody = copy.mCentralBody;
   mOrigin = copy.mOrigin;
   mInternalCoordSystem = copy.mInternalCoordSystem;
   mOutCoordSystem = copy.mOutCoordSystem;
}


//------------------------------------------------------------------------------
// PlanetData& operator= (const PlanetData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
PlanetData& PlanetData::operator= (const PlanetData &right)
{
   if (this == &right)
      return *this;

   RefData::operator=(right);
      
   mCentralBodyName = right.mCentralBodyName;
   mSpacecraft = right.mSpacecraft;
   mSolarSystem = right.mSolarSystem;
   mCentralBody = right.mCentralBody;
   mOrigin = right.mOrigin;
   mInternalCoordSystem = right.mInternalCoordSystem;
   mOutCoordSystem = right.mOutCoordSystem;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~PlanetData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PlanetData::~PlanetData()
{
}


//------------------------------------------------------------------------------
// Real GetPlanetReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves planet related parameters.
 */
//------------------------------------------------------------------------------
Real PlanetData::GetPlanetReal(Integer item)
{
   #ifdef DEBUG_PLANETDATA_RUN
   MessageInterface::ShowMessage
      ("PlanetData::GetPlanetReal() item=%d, mSpacecraft=<%p>, mSolarSystem=<%p>\n",
       item, mSpacecraft, mSolarSystem);
   #endif
   
   if (item < LATITUDE || item > LST_ID)
      throw ParameterException
         ("PlanetData::GetPlanetReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item));
   
   if (mSpacecraft == NULL || mSolarSystem == NULL)
   {
      InitializeRefObjects();
      #ifdef DEBUG_PLANETDATA_RUN
      MessageInterface::ShowMessage
         ("   After InitializeRefObjects(), mSpacecraft=<%p>, mSolarSystem=<%p>\n",
          mSpacecraft, mSolarSystem);
      #endif
   }
   
   // Get current time
   Real a1mjd = mSpacecraft->GetEpoch();
   
   // Call GetHourAngle() on origin
   Real mha = 0.0;
   if(item == LST_ID || item == MHA_ID)
      mha = mOrigin->GetHourAngle(a1mjd);
   
   #ifdef DEBUG_PLANETDATA_RUN
   MessageInterface::ShowMessage("   mha = %f\n", mha);
   #endif
   
   // Why not use a1mjd obtained previously?
   Real epoch = mSpacecraft->GetRealParameter("A1Epoch");
   Rvector6 instate = mSpacecraft->GetState().GetState();
   Rvector6 state;
   mCoordConverter.Convert(A1Mjd(epoch), instate, mInternalCoordSystem,
                           state, mOutCoordSystem);
   // get flattening for the body
   Real flatteningFactor =
      mOrigin->GetRealParameter(mOrigin->GetParameterID("Flattening"));

   Real equatorialRadius =
      mOrigin->GetRealParameter(mOrigin->GetParameterID("EquatorialRadius"));

   return GmatCalcUtil::CalculatePlanetData(VALID_PLANET_DATA_NAMES[item - LATITUDE],
                                            state, equatorialRadius, flatteningFactor, mha);
}


//------------------------------------------------------------------------------
// Real GetPlanetReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrieves planet related parameters.
 */
//------------------------------------------------------------------------------
Real PlanetData::GetPlanetReal(const std::string &str)
{
   if (str == "Latitude")
      return GetPlanetReal(LATITUDE);
   else if (str == "Longitude")
      return GetPlanetReal(LONGITUDE);
   else if (str == "Altitude")
      return GetPlanetReal(ALTITUDE);
   else if (str == "MHA")
      return GetPlanetReal(MHA_ID);
   else if (str == "LST")
      return GetPlanetReal(LST_ID);
   else
   {
      throw ParameterException
         ("PlanetData::GetPlanetReal Unknown parameter name: " + str);
   }
}


//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * @param <cs> internal coordinate system what parameter data is representing.
 */
//------------------------------------------------------------------------------ 
void PlanetData::SetInternalCoordSystem(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* PlanetData::GetValidObjectList() const
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
bool PlanetData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<PlanetDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == PlanetDataObjectCount)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void PlanetData::InitializeRefObjects()
{
   #ifdef DEBUG_PLANETDATA_INIT
   MessageInterface::ShowMessage("PlanetData::InitializeRefObjects() entered\n");
   #endif
   
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);

   if (mSpacecraft == NULL)
      throw ParameterException
         ("PlanetData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set.\n");
   
   mSolarSystem = (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   if (mSolarSystem == NULL)
      throw ParameterException
         ("PlanetData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   mCentralBody = mSolarSystem->GetBody(mCentralBodyName);
   
   if (!mCentralBody)
      throw ParameterException("PlanetData::InitializeRefObjects() Body not found in the "
                               "SolarSystem: " + mCentralBodyName + "\n");
   
   if (mInternalCoordSystem == NULL)
      throw ParameterException
         ("PlanetData::InitializeRefObjects() Cannot find internal "
          "CoordinateSystem object\n");
   #ifdef DEBUG_PLANETDATA_INIT
      MessageInterface::ShowMessage
         ("PlanetData::InitializeRefObjects() getting output CS pointer.\n");
   #endif
   
   mOutCoordSystem =
      (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   
   #ifdef DEBUG_PLANETDATA_INIT
   MessageInterface::ShowMessage
      ("   mOutCoordSystem=<%p>'%s'\n", mOutCoordSystem,
       mOutCoordSystem ? mOutCoordSystem->GetName().c_str() : "NULL");
   #endif
   
   if (mOutCoordSystem == NULL)
      throw ParameterException
         ("PlanetData::InitializeRefObjects() Cannot find output "
          "CoordinateSystem object\n");
   
   // if dependent body name exist and it is a CelestialBody, set gravity constant
   
   std::string originName =
      FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[SPACE_POINT]));
   
   if (originName != "")
   {
      #ifdef DEBUG_PLANETDATA_INIT
         MessageInterface::ShowMessage
            ("PlanetData::InitializeRefObjects() getting originName:%s pointer.\n",
             originName.c_str());
      #endif
         
      mOrigin =
         (CelestialBody*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACE_POINT]);
      
      #ifdef DEBUG_PLANETDATA_INIT
      MessageInterface::ShowMessage
         ("   mOrigin=<%p>'%s'\n", mOrigin, mOrigin ? mOrigin->GetName().c_str() : "NULL");
      #endif
      
      if (!mOrigin)
      {
         throw ParameterException
            ("PlanetData::InitializeRefObjects() Cannot find Origin object: " +
             originName + "\n");
      }
   }
   #ifdef DEBUG_PLANETDATA_INIT
   MessageInterface::ShowMessage("PlanetData::InitializeRefObjects() leaving\n");
   #endif
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
bool PlanetData::IsValidObjectType(UnsignedInt type)
{
#ifdef DEBUG_PLANETDATA_INIT
   MessageInterface::ShowMessage
      ("PlanetData::IsValidObjectType() CHECKING type %s.\n",(GmatBase::GetObjectTypeString(type).c_str()));
#endif
   for (int i=0; i<PlanetDataObjectCount; i++)
   {
#ifdef DEBUG_PLANETDATA_INIT
   MessageInterface::ShowMessage
      ("PlanetData::IsValidObjectType() has type %s.\n",(VALID_OBJECT_TYPE_LIST[i]).c_str());
#endif
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
      // Special case for CelestialBody origin
      if ((VALID_OBJECT_TYPE_LIST[i] == "SpacePoint") && (GmatBase::GetObjectTypeString(type) == "CelestialBody"))
         return true;
   }
   
   return false;
}

