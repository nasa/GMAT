//$Id$
//------------------------------------------------------------------------------
//                                  BurnData
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
// Created: 2005/05/27
//
/**
 * Implements Burn related data class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "BurnData.hpp"
#include "ParameterException.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"           // for GmatRealUtil::ToString()
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BURNDATA_INIT 1
//#define DEBUG_BURNDATA_CONVERT 1
//#define DEBUG_BURNDATA_SET 1
//#define DEBUG_BURNDATA_GET
//#define DEBUG_BURNDATA_COORD_CONVERT

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const std::string
BurnData::VALID_OBJECT_TYPE_LIST[BurnDataObjectCount] =
{
   "ImpulsiveBurn",
   "FiniteBurn",
   "SolarSystem",
   "CoordinateSystem",
   "Spacecraft"
}; 

const Real BurnData::BURN_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;
const Real BurnData::BURN_TOL            = 1.0e-10;


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// BurnData(const std::string &name = "",
//          const UnsignedInt paramOwnerType = Gmat::IMPULSIVE_BURN)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
BurnData::BurnData(const std::string &name, const std::string &typeName,
                   const UnsignedInt paramOwnerType)
   : RefData(name, typeName, paramOwnerType)
{
   mBurn                      = NULL;
   mSpacecraft                = NULL;
   mSolarSystem               = NULL;
   mOrigin                    = NULL;
   mInternalCoordSystem       = NULL;
   mOutCoordSystem            = NULL;

   firstTimeEpochWarning      = false;
   firstTimeHasntFiredWarning = false;

   if (mParamDepName == "")
      mIsParamCSDep = false;
   else
      mIsParamCSDep = true;
}


//------------------------------------------------------------------------------
// BurnData(const BurnData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the BurnData object being copied.
 */
//------------------------------------------------------------------------------
BurnData::BurnData(const BurnData &copy)
   : RefData(copy)
{
   mBurn                      = copy.mBurn;
   mSpacecraft                = copy.mSpacecraft;
   mSolarSystem               = copy.mSolarSystem;
   mOrigin                    = copy.mOrigin;
   mInternalCoordSystem       = copy.mInternalCoordSystem;
   mOutCoordSystem            = copy.mOutCoordSystem;
   mIsParamCSDep              = copy.mIsParamCSDep;
   firstTimeEpochWarning      = copy.firstTimeEpochWarning;        // or false?
   firstTimeHasntFiredWarning = copy.firstTimeHasntFiredWarning;   // or false?
}


//------------------------------------------------------------------------------
// BurnData& operator= (const BurnData& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
BurnData& BurnData::operator= (const BurnData& right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
      
   mBurn                      = right.mBurn;
   mSpacecraft                = right.mSpacecraft;
   mSolarSystem               = right.mSolarSystem;
   mOrigin                    = right.mOrigin;
   mInternalCoordSystem       = right.mInternalCoordSystem;
   mOutCoordSystem            = right.mOutCoordSystem;
   mIsParamCSDep              = right.mIsParamCSDep;
   firstTimeEpochWarning      = right.firstTimeEpochWarning;
   firstTimeHasntFiredWarning = right.firstTimeHasntFiredWarning;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~BurnData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
BurnData::~BurnData()
{
}


//------------------------------------------------------------------------------
// Real GetReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves Burn element.
 */
//------------------------------------------------------------------------------
Real BurnData::GetReal(Integer item)
{
   #ifdef DEBUG_BURNDATA_GET
   MessageInterface::ShowMessage
      ("BurnData::GetReal() <%p> entered, mActualParamName='%s', mParamOwnerName='%s', "
       "mParamOwnerType='%s'\n   mParamDepName='%s', item=%d, mBurn=<%p>, mIsParamCSDep=%d\n",
       this, mActualParamName.c_str(), mParamOwnerName.c_str(),
       GmatBase::GetObjectTypeString(mParamOwnerType).c_str(), mParamDepName.c_str(),
       item, mBurn, mIsParamCSDep);
   #endif
   
   ///@todo convert internal DeltaV to Parameter CoordinateSystem.
   if (mBurn == NULL)
      InitializeRefObjects();
   
   if (mBurn == NULL)
   {
      MessageInterface::ShowMessage
         ("Cannot find Burn object so returning %f\n", BURN_REAL_UNDEFINED);
      return BURN_REAL_UNDEFINED;
   }

   
   Real lastFireEpoch = mBurn->GetEpochAtLastFire();
   #ifdef DEBUG_BURNDATA_GET
   MessageInterface::ShowMessage
      ("   lastFireEpoch=%.12f, fired=%d\n", lastFireEpoch, mBurn->HasFired());
   #endif
   
   Real result;
   if (mIsParamCSDep)
   {
      #ifdef DEBUG_BURNDATA_GET
      MessageInterface::ShowMessage("It is a CS dependent Parameter\n");
      #endif
      if ((mInternalCoordSystem == NULL) ||
          (mOutCoordSystem == NULL))
      {
         std::string errmsg = "**** ERROR **** Missing, invalid, or ";
         errmsg            += "nonexistent internal or output ";
         errmsg            += "CoordinateSystem object, for parameter \"";
         errmsg            += mActualParamName + "\"\n";
         throw ParameterException(errmsg);
      }
      
      if (!mBurn->HasFired())
      {
         if (!firstTimeHasntFiredWarning)
         {
            Real burnEpoch = mBurn->GetEpochAtLastFire();
            // Now Burn object can be ImpulsiveBurn or FiniteBurn so format accordingly
            std::string burnTypeName = GmatBase::GetObjectTypeString(mParamOwnerType);
            std::string errmsg = "*** WARNING *** " + burnTypeName + " \"";
            errmsg += mBurn->GetName() + "\" has not fired";
            std::string errmsg2 = ", so returning \"0\" for " + mActualParamName + " Parameter.\n";
            MessageInterface::ShowMessage("%s at epoch: %f%s", errmsg.c_str(), burnEpoch, errmsg2.c_str());
            firstTimeHasntFiredWarning = true;
         }
         return 0.0;
      }
      
      // we need to do conversions and check for epochs here
      // write a warning of the epoch of the maneuver does not match the
      // epoch of the participants in the coordinate system
      // this part has been removed/postponed  WCS 2013.07.01
//      Real burnEpoch = mBurn->GetEpochAtLastFire();
//      ObjectArray objs =  mOurCoordSystem->GetSpacePointParticipants();

      Real burnEpoch = mBurn->GetEpochAtLastFire();
      Real *burnDeltaV = mBurn->GetDeltaVInertial();
      Rvector6 burnState(0.0,0.0,0.0, burnDeltaV[0], burnDeltaV[1], burnDeltaV[2]);
      #ifdef DEBUG_BURNDATA_GET
      MessageInterface::ShowMessage(
            "In BurnData::GetReal, burnEpoch = %le, burnDeltaVInertial = %le %le %le\n",
            burnEpoch, burnDeltaV[0], burnDeltaV[1], burnDeltaV[2]);
      MessageInterface::ShowMessage("      and burnStateInertial = %s\n",
            burnState.ToString().c_str());
      MessageInterface::ShowMessage(
            "      mInternalCoordSystem = %s, mOutCoordSystem = %s\n",
            mInternalCoordSystem->GetName().c_str(),
            mOutCoordSystem->GetName().c_str());
      #endif
      Rvector6 burnOut(0.0,0.0,0.0,0.0,0.0,0.0);
      if (mInternalCoordSystem->GetName() != mOutCoordSystem->GetName())
      {
         mCoordConverter.Convert(A1Mjd(burnEpoch), burnState, mInternalCoordSystem,
                                 burnOut, mOutCoordSystem, false, true);   // do I want true here?
         #ifdef DEBUG_BURNDATA_COORD_CONVERT
            MessageInterface::ShowMessage("before conversion, burnState = %s\n",
                  burnState.ToString().c_str());
            Rmatrix33 lastR = mCoordConverter.GetLastRotationMatrix();
               MessageInterface::ShowMessage("Last R is: %s\n", lastR.ToString().c_str());
            Rmatrix33 lastRDot = mCoordConverter.GetLastRotationDotMatrix();
               MessageInterface::ShowMessage("Last RDot is: %s\n", lastRDot.ToString().c_str());
            MessageInterface::ShowMessage("after conversion, burnOut = %s\n",
                  burnOut.ToString().c_str());
         #endif
      }
      switch (item)
      {
      case ELEMENT1:
         result = burnOut[3];
         break;
      case ELEMENT2:
         result = burnOut[4];
         break;
      case ELEMENT3:
         result = burnOut[5];
         break;
      default:
         throw ParameterException("BurnData::GetReal() Unknown ELEMENT id: " +
                                  GmatRealUtil::ToString(item));
      }
   }
   else
   {
      #ifdef DEBUG_BURNDATA_GET
      MessageInterface::ShowMessage("   It is not a CS dependent Parameter\n");
      #endif
      
      // if there is no dependency object, then return the values from the
      // burn in the burn coordinate system
      switch (item)
      {
      case ELEMENT1:
         result = mBurn->GetRealParameter(mBurn->GetParameterID("Element1"));
         break;
      case ELEMENT2:
         result = mBurn->GetRealParameter(mBurn->GetParameterID("Element2"));
         break;
      case ELEMENT3:
         result = mBurn->GetRealParameter(mBurn->GetParameterID("Element3"));
         break;
      case TOTAL_MASS_FLOW_RATE:
         result = mBurn->GetTotalMassFlowRate();
         break;
      case TOTAL_ACCEL1:
      case TOTAL_ACCEL2:
      case TOTAL_ACCEL3:
      {
         Real *accel = mBurn->GetTotalAcceleration();
         #ifdef DEBUG_BURNDATA_GET
         MessageInterface::ShowMessage
            ("   totalAccel = [%.15e %.15e %.15e]\n", accel[0], accel[1], accel[2]);
         #endif
         switch (item)
         {
         case TOTAL_ACCEL1:
            result = accel[0];
            break;
         case TOTAL_ACCEL2:
            result = accel[1];
            break;
         case TOTAL_ACCEL3:
            result = accel[2];
            break;
         }
         break;
      }
      case TOTAL_THRUST1:
      case TOTAL_THRUST2:
      case TOTAL_THRUST3:
      {
         Real *thrust = mBurn->GetTotalThrust();
         #ifdef DEBUG_BURNDATA_GET
         MessageInterface::ShowMessage
            ("   totalThrust = [%.15e %.15e %.15e]\n", thrust[0], thrust[1], thrust[2]);
         #endif
         switch (item)
         {
         case TOTAL_THRUST1:
            result = thrust[0];
            break;
         case TOTAL_THRUST2:
            result = thrust[1];
            break;
         case TOTAL_THRUST3:
            result = thrust[2];
            break;
         }
         break;
      }
      default:
         throw ParameterException("BurnData::GetReal() Unknown ELEMENT id: " +
                                  GmatRealUtil::ToString(item));
      }
   }
   
   #ifdef DEBUG_BURNDATA_GET
   MessageInterface::ShowMessage
      ("BurnData::GetReal() <%p> returning %.12f for Parameter '%s'\n", this,
       result, mActualParamName.c_str());
   #endif
   return result;
}


//------------------------------------------------------------------------------
// void SetReal(Integer item, Real rval)
//------------------------------------------------------------------------------
void BurnData::SetReal(Integer item, Real rval)
{
   #if DEBUG_BURNDATA_SET
   MessageInterface::ShowMessage
      ("BurnData::SetReal() item=%d, rval=%f\n", item, rval);
   #endif
   
   ///@todo convert internal DeltaV to Parameter CoordinateSystem.
   if (mBurn == NULL)
      InitializeRefObjects();
   
   if (mBurn == NULL)
   {
      MessageInterface::ShowMessage
         ("Cannot find Burn object so returning %f\n", BURN_REAL_UNDEFINED);
   }
   
   std::string csName = mBurn->GetStringParameter(mBurn->GetParameterID("CoordinateSystem"));
//   if (mIsParamCSDep)
   if (mIsParamCSDep && (mOutCoordSystem->GetName() != csName))
   {
      ParameterException pe;
      pe.SetDetails("Currently GMAT cannot set %s; the impulsive burn '%s' "
                    "requires values to be in its own coordinate system (setting "
                    "values in different coordinate systems will be implemented in "
                    "future builds)",  mActualParamName.c_str(),
                    mBurn->GetName().c_str());
      throw pe;
   }


   switch (item)
   {
   case ELEMENT1:
      mBurn->SetRealParameter(mBurn->GetParameterID("Element1"), rval);
      break;
   case ELEMENT2:
      mBurn->SetRealParameter(mBurn->GetParameterID("Element2"), rval);
      break;
   case ELEMENT3:
      mBurn->SetRealParameter(mBurn->GetParameterID("Element3"), rval);
      break;
   default:
      throw ParameterException("BurnData::SetReal() Unknown ELEMENT id: " +
                               GmatRealUtil::ToString(item));
   }
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* BurnData::GetValidObjectList() const
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
bool BurnData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<BurnDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == BurnDataObjectCount)
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
void BurnData::InitializeRefObjects()
{
   #if DEBUG_BURNDATA_INIT
   MessageInterface::ShowMessage
      ("BurnData::InitializeRefObjects() entered.\n");
   #endif
   
   mBurn = (Burn*)FindFirstObject(VALID_OBJECT_TYPE_LIST[IMPULSIVE_BURN]);
   // If ImpulsiveBurn object not found, try FiniteBurn
   if (mBurn == NULL)
      mBurn = (Burn*)FindFirstObject(VALID_OBJECT_TYPE_LIST[FINITE_BURN]);
   
   if (mBurn == NULL)
   {
      // just write a message
      #if DEBUG_BURNDATA_INIT
      MessageInterface::ShowMessage
         ("BurnData::InitializeRefObjects() Cannot find Burn object.\n");
      #endif
      
      //throw ParameterException
      //   ("BurnData::InitializeRefObjects() Cannot find Burn object.\n");
   }
   
   //-------------------------------------------------------
   // Need this for future?
   //-------------------------------------------------------
//    mSolarSystem =
//       (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   
//    if (mSolarSystem == NULL)
//       throw ParameterException
//          ("BurnData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   if (mInternalCoordSystem == NULL)
   {
      std::string errmsg = "**** ERROR **** Missing, invalid, or ";
      errmsg            += "nonexistent internal CoordinateSystem object, ";
      errmsg            += "for parameter \"" + mActualParamName + "\"\n";
      throw ParameterException(errmsg);
   }
   
   #if DEBUG_BURNDATA_INIT
   MessageInterface::ShowMessage
      ("BurnData::InitializeRefObjects() Found internal coordinate system\n");
   #endif
   
   std::string outCoordSysName = FindFirstObjectName(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   #if DEBUG_BURNDATA_INIT
   MessageInterface::ShowMessage
      ("BurnData::InitializeRefObjects() looking for output coordinate system: '%s'\n",
       outCoordSysName.c_str());
   #endif
   
   // Check for blank coordinate system first (LOJ: 2015.02.11)
   if (outCoordSysName == "")
      mOutCoordSystem = NULL;
   else
   {
      mOutCoordSystem =
         (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
      
      if (mOutCoordSystem == NULL)
      {
         std::string errmsg = "**** ERROR **** Missing, invalid, or ";
         errmsg            += "nonexistent output CoordinateSystem object, ";
         errmsg            += "for parameter \"" + mActualParamName + "\"\n";
         throw ParameterException(errmsg);
      }
      #if DEBUG_BURNDATA_INIT
      MessageInterface::ShowMessage
         ("BurnData::InitializeRefObjects() Found OUT coordinate system\n");
      #endif
   }
   
//    // get Burn CoordinateSystem
//    std::string csName = mBurn->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
//    CoordinateSystem *cs = (CoordinateSystem*)mBurn->
//       GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
//
//    if (!cs)
//       throw ParameterException
//          ("BurnData::InitializeRefObjects() spacecraft CoordinateSystem not "
//           "found: " + csName + "\n");
//   #if DEBUG_BURNDATA_INIT
//   MessageInterface::ShowMessage
//      ("BurnData::InitializeRefObjects() Found spacecraft coordinate system\n");
//   #endif

   
   #if DEBUG_BURNDATA_INIT
   MessageInterface::ShowMessage
      ("BurnData::InitializeRefObjects() mBurn=%s(%p)\n",
       mBurn->GetName().c_str(), mBurn);
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
bool BurnData::IsValidObjectType(UnsignedInt type)
{
   for (int i=0; i<BurnDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystem()
//------------------------------------------------------------------------------
SolarSystem* BurnData::GetSolarSystem()
{
   return mSolarSystem;
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordSys()
//------------------------------------------------------------------------------
CoordinateSystem* BurnData::GetInternalCoordSys()
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
void BurnData::SetInternalCoordSys(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}


