//$Id$
//------------------------------------------------------------------------------
//                                  BplaneData
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
// Created: 2005/06/16
//
/**
 * Implements B-Plane related data class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "BplaneData.hpp"
#include "ParameterException.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"
#include "CalculationUtilities.hpp"
#include "GmatConstants.hpp"
#include "UtilityException.hpp"
#include "CelestialBody.hpp"
#include "Linear.hpp"           // for GmatRealUtil::ToString()
#include "MessageInterface.hpp"

//#define DBGLVL_BPLANEDATA_INIT 1
//#define DBGLVL_BPLANEDATA_CONVERT 1
//#define DBGLVL_BPLANEDATA_RUN 2

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const std::string BplaneData::VALID_OBJECT_TYPE_LIST[BplaneDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem",
   "CoordinateSystem",
   "SpacePoint"
}; 

const std::string BplaneData::VALID_PARAM_NAMES[BplaneParamEnd - BplaneParamBegin - 1] =
{
   "BDotT",
   "BDotR",
   "BVectorMag",
   "BVectorAngle"
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BplaneData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
BplaneData::BplaneData()
   : RefData()
{
   mCartState = Rvector6::RVECTOR6_UNDEFINED;
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mOrigin = NULL;
   mInternalCoordSystem = NULL;
   mOutCoordSystem = NULL;
   mCartEpoch = 0.0;
   mGravConst = 0.0;
}


//------------------------------------------------------------------------------
// BplaneData(const BplaneData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the BplaneData object being copied.
 */
//------------------------------------------------------------------------------
BplaneData::BplaneData(const BplaneData &copy)
   : RefData(copy)
{
   mCartState = copy.mCartState;
   mSpacecraft = copy.mSpacecraft;
   mSolarSystem = copy.mSolarSystem;
   mOrigin = copy.mOrigin;
   mInternalCoordSystem = copy.mInternalCoordSystem;
   mOutCoordSystem = copy.mOutCoordSystem;   
   mCartEpoch = copy.mCartEpoch;
   mGravConst = copy.mGravConst;
}


//------------------------------------------------------------------------------
// BplaneData& operator= (const BplaneData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
BplaneData& BplaneData::operator= (const BplaneData &right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
   
   mCartState = right.mCartState;
   mSpacecraft = right.mSpacecraft;
   mSolarSystem = right.mSolarSystem;
   mOrigin = right.mOrigin;
   mInternalCoordSystem = right.mInternalCoordSystem;
   mOutCoordSystem = right.mOutCoordSystem;
   mCartEpoch = right.mCartEpoch;
   mGravConst = right.mGravConst;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~BplaneData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
BplaneData::~BplaneData()
{
}


//------------------------------------------------------------------------------
// Real GetBplaneReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Computes B-Plane related paramters.
 *
 * @note - Implements GMAT math Spec 2.11 B-Plane Coordinates
 */
//------------------------------------------------------------------------------
Real BplaneData::GetBplaneReal(Integer item)
{
   // Check item range
   if (item <= BplaneParamBegin || item >= BplaneParamEnd)
      throw ParameterException
         ("BplaneData::GetBplaneReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item, false, 2));
   
   if (mSpacecraft == NULL || mSolarSystem == NULL || mOrigin == NULL ||
       mInternalCoordSystem == NULL || mOutCoordSystem == NULL)
      InitializeRefObjects();
   
   // If the selected coordinate system does not have a celestial body as
   // its origin, then throw an exception
   if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
   else
      throw ParameterException
         ("BplaneData::GetBplaneReal() Cannot compute B-Plane "
          "parameter for CoordinateSystem that has non-celestial body as origin.  "
          "CoordinateSystem: " + mOutCoordSystem->GetName() + "  Origin: " +
          mOrigin->GetName());
   
   Rvector6 state = GetCartState();
   
   #if DBGLVL_BPLANEDATA_RUN > 1
   MessageInterface::ShowMessage
      ("BplaneData::GetBplaneReal() item = %d, mGravConst = %f\n     state = %s\n",
       item, mGravConst, state.ToString().c_str());
   #endif

   return GmatCalcUtil::CalculateBPlaneData(VALID_PARAM_NAMES[item-1], state, mGravConst);

}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* BplaneData::GetValidObjectList() const
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
bool BplaneData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<BplaneDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == BplaneDataObjectCount)
      return true;
   else
      return false;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void BplaneData::InitializeRefObjects()
{
   #if DBGLVL_BPLANEDATA_INIT
   MessageInterface::ShowMessage
      ("BplaneData::InitializeRefObjects() entered.\n");
   #endif
   
   mSpacecraft =
      (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   
   if (mSpacecraft == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set to any unnamed parameters\n");
   
   mSolarSystem =
      (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   
   if (mSolarSystem == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   if (mInternalCoordSystem == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find internal "
          "CoordinateSystem object\n");
   
   mOutCoordSystem =
      (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   
   if (mOutCoordSystem == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find output "
          "CoordinateSystem object\n");
   
   // get spacecraft CoordinateSystem
   std::string csName = mSpacecraft->GetRefObjectName(Gmat::COORDINATE_SYSTEM);   
   CoordinateSystem *cs = (CoordinateSystem*)mSpacecraft->
      GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
   
   if (!cs)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() spacecraft CoordinateSystem not "
          "found: " + csName + "\n");
   
   mOrigin = mOutCoordSystem->GetOrigin();
   
   if (!mOrigin)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() origin not found: " +
          cs->GetOriginName() + "\n");
   
   // get gravity constant if out coord system origin is CelestialBody
   if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
   
   #if DBGLVL_BPLANEDATA_INIT
   MessageInterface::ShowMessage
      ("BplaneData::InitializeRefObjects() mOriginName=%s\n",
       mOrigin->GetName().c_str());
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
bool BplaneData::IsValidObjectType(UnsignedInt type)
{
   for (int i=0; i<BplaneDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   MessageInterface::ShowMessage
      ("==> BplaneData::IsValidObjectType() type=%d is not valid object type\n",
       type);
   
   return false;
   
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystem()
//------------------------------------------------------------------------------
SolarSystem* BplaneData::GetSolarSystem()
{
   return mSolarSystem;
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordSys()
//------------------------------------------------------------------------------
CoordinateSystem* BplaneData::GetInternalCoordSys()
{
   return mInternalCoordSystem;
}


//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * @param <cs> internal coordinate system what parameter data is representing.
 */
//------------------------------------------------------------------------------ 
void BplaneData::SetInternalCoordSys(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}


//------------------------------------------------------------------------------
// Rvector6 GetCartState()
//------------------------------------------------------------------------------
Rvector6 BplaneData::GetCartState()
{   
   mCartEpoch = mSpacecraft->GetEpoch();
   mCartState.Set(mSpacecraft->GetState().GetState());
   
   #if DBGLVL_BPLANEDATA_RUN
   MessageInterface::ShowMessage
      ("BplaneData::GetCartState() internalCoordName=%s\n"
       "     outCoordName=%s\n",
       mInternalCoordSystem->GetName().c_str(),
       mOutCoordSystem->GetName().c_str());
   #endif
   
   
   if (mInternalCoordSystem == NULL || mOutCoordSystem == NULL)
   {
      MessageInterface::ShowMessage
         ("BplaneData::GetCartState() Internal CoordSystem or Output "
          "CoordSystem is NULL.\n");
         
      throw ParameterException
         ("BplaneData::GetCartState() internal or output CoordinateSystem "
          "is NULL.\n");
   }
   
   // convert to output CoordinateSystem
   if (mInternalCoordSystem->GetName() != mOutCoordSystem->GetName())
   {
      #if DBGLVL_BPLANEDATA_CONVERT
      MessageInterface::ShowMessage
         ("BplaneData::GetCartState() ===> mOutCoordSystem:%s Axis=%s\n",
          mOutCoordSystem->GetName().c_str(),
          mOutCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, "")->GetName().c_str());
      MessageInterface::ShowMessage
         ("BplaneData::GetCartState() Before convert: mCartEpoch=%f\n"
          "state = %s\n", mCartEpoch, mCartState.ToString().c_str());
      #endif
      
      try
      {
         mCoordConverter.Convert(A1Mjd(mCartEpoch), mCartState,
                                 mInternalCoordSystem,
                                 mCartState, mOutCoordSystem);
         
         #if DBGLVL_BPLANEDATA_CONVERT
         MessageInterface::ShowMessage
            ("BplaneData::GetCartState() --After convert: mCartEpoch=%f\n"
             "     state = %s\n", mCartEpoch, mCartState.ToString().c_str());
         #endif
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage(e.GetFullMessage());
      }
   }
   
   return mCartState;
}


