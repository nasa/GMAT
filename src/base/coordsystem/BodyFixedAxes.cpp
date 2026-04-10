//$Id$
//------------------------------------------------------------------------------
//                                  BodyFixedAxes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/01/25
//
/**
 * Implementation of the BodyFixedAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "BodyFixedAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "DeFile.hpp"
#include "Planet.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "GmatConstants.hpp"
#include "Rvector3.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"
#include "MessageInterface.hpp"
#include "Attitude.hpp"
#include "Spacecraft.hpp"
#include "AttitudeConversionUtility.hpp"

using namespace GmatMathUtil;        // for trig functions, etc.
using namespace GmatTimeConstants;   // for JD offsets, etc.

//#define DEBUG_FIRST_CALL
//#define DEBUG_TIME_CALC
//#define DEBUG_MOON_MATRIX
//#define DEBUG_BF_MATRICES
//#define DEBUG_BF_CHECK_DETERMINANT
//#define DEBUG_BF_ROT_MATRIX
//#define DEBUG_BF_RECOMPUTE
//#define DEBUG_BF_EPOCHS
//#define DEBUG_BF_SC
//#define DEBUG_BFA_INIT
//#define DEBUG_SET_REF
//#define DEBUG_DE_FILE_USED

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  BodyFixedAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base BodyFixedAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 */
//------------------------------------------------------------------------------
BodyFixedAxes::BodyFixedAxes(const std::string &itsName) :
   DynamicAxes("BodyFixed",itsName),
   de                       (NULL),
   prevEpoch                (0.0),
   prevUpdateInterval       (-99.9),
   prevOriginUpdateInterval (-99.9),
   spiceFrameId             (""),
   spiceIdSet               (false),
   originSpiceIdPId         (0),
   forGravityUpdate         (false),
   prevLunaSrc              (Gmat::RotationDataSrcCount)
{
   objectTypeNames.push_back("BodyFixedAxes");
   parameterCount = BodyFixedAxesParamCount;

   needsCBOrigin = false; // true;
}


//------------------------------------------------------------------------------
//  BodyFixedAxes(const BodyFixedAxes &bfAxes);
//------------------------------------------------------------------------------
/**
 * Constructs base BodyFixedAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param bfAxes  BodyFixedAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
BodyFixedAxes::BodyFixedAxes(const BodyFixedAxes &bfAxes) :
   DynamicAxes(bfAxes),
   de                       (NULL),
   prevEpoch                (bfAxes.prevEpoch),
   prevUpdateInterval       (bfAxes.prevUpdateInterval),
   prevOriginUpdateInterval (bfAxes.prevOriginUpdateInterval),
   spiceFrameId             (bfAxes.spiceFrameId),
   spiceIdSet               (bfAxes.spiceIdSet),
   originSpiceIdPId         (bfAxes.originSpiceIdPId),
   forGravityUpdate         (false),
   prevLunaSrc              (bfAxes.prevLunaSrc)
{
   #ifdef DEBUG_BF_RECOMPUTE
      MessageInterface::ShowMessage("Constructing a new BFA (%p) from the old one (%p)\n",
            this, &bfAxes);
      MessageInterface::ShowMessage("   and prevEpoch(old) %12.10f copied to prevEpoch(new) %12.10f\n",
            bfAxes.prevEpoch.GetMjd(), prevEpoch.GetMjd());
   #endif
}

//------------------------------------------------------------------------------
//  BodyFixedAxes& operator=(const BodyFixedAxes &bfAxes)
//------------------------------------------------------------------------------
/**
 * Assignment operator for BodyFixedAxes structures.
 *
 * @param bfAxes The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const BodyFixedAxes& BodyFixedAxes::operator=(const BodyFixedAxes &bfAxes)
{
   if (&bfAxes == this)
      return *this;
   DynamicAxes::operator=(bfAxes); 
   de                       = bfAxes.de;
   prevEpoch                = bfAxes.prevEpoch;
   prevUpdateInterval       = bfAxes.prevUpdateInterval;
   prevOriginUpdateInterval = bfAxes.prevOriginUpdateInterval;
   spiceFrameId             = bfAxes.spiceFrameId;
   spiceIdSet               = bfAxes.spiceIdSet;
   originSpiceIdPId         = bfAxes.originSpiceIdPId;
   prevLunaSrc              = bfAxes.prevLunaSrc;
   forGravityUpdate         = false;

   return *this;
}


//------------------------------------------------------------------------------
//  ~BodyFixedAxes()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BodyFixedAxes::~BodyFixedAxes()
{
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesEopFile(
//                                 const std::string &forBaseSystem) const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//------------------------------------------------------------------------------
GmatCoordinate::ParameterUsage BodyFixedAxes::UsesEopFile(
                               const std::string &forBaseSystem) const
{
   if (forBaseSystem == baseSystem)
      return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesItrfFile() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//------------------------------------------------------------------------------
GmatCoordinate::ParameterUsage BodyFixedAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage BodyFixedAxes::UsesNutationUpdateInterval() const
{
   if (originName == GmatSolarSystemDefaults::EARTH_NAME)
      return GmatCoordinate::REQUIRED;
   return DynamicAxes::UsesNutationUpdateInterval();
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this BodyFixedAxes.
 *
 * @return success flag
 */
//------------------------------------------------------------------------------
bool BodyFixedAxes::Initialize()
{
#ifdef DEBUG_BFA_INIT
   MessageInterface::ShowMessage("Entering BFA::Init ...\n");
   if (origin)
      MessageInterface::ShowMessage("origin is %s<%p>\n",
            origin->GetName().c_str(), origin);
#endif
   DynamicAxes::Initialize();
   if (originName == GmatSolarSystemDefaults::EARTH_NAME) InitializeFK5();
   
   // Check for spacecraft origin whose attitude does not compute rates
   if (origin && origin->IsOfType("Spacecraft"))
   {
      Attitude *att = (Attitude*) origin->GetRefObject(Gmat::ATTITUDE, "");
      if (!att)
      {
         std::string errmsg = "The value of \"";
         errmsg += origin->GetName() + "\" for field \"Origin\"";
         errmsg += " on BodyFixed coordinate system \"" + coordName;
         errmsg += "\" is not an allowed value.\n";
         errmsg += "The allowed values are: ";
         errmsg += "[ Spacecraft with an attitude model ].\n";
         throw CoordinateSystemException(errmsg);
      }
      if ((!allowNoRates) && !(att->ModelComputesRates()))
      {
         std::string errmsg = "The value of \"";
         errmsg += origin->GetName() + "\" for field \"Origin\"";
         errmsg += " on BodyFixed coordinate system \"" + coordName;
         errmsg += "\" is not an allowed value.\n";
         errmsg += "The allowed values are: ";
         errmsg += "[ Spacecraft whose attitude model computes rates ].\n";
         throw CoordinateSystemException(errmsg);
      }
   }

   originSpiceIdPId = origin->GetParameterID("SpiceFrameId");
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BodyFixedAxes.
 *
 * @return clone of the BodyFixedAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* BodyFixedAxes::Clone() const
{
   return (new BodyFixedAxes(*this));
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the BodyFixedAxes class.  This is
 * overridden from the CoordinateBase version, in order to make sure the origin
 * is a CelestialBody.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool BodyFixedAxes::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                 const std::string &name)
{
   #ifdef DEBUG_SET_REF
   MessageInterface::ShowMessage
      ("BodyFixedAxes::SetRefObject() <%s>, obj=<%p>'%s', name=%s\n", GetName().c_str(),
       obj, obj ? obj->GetName().c_str() : "NULL",  name.c_str());
   #endif
   
   if (obj == NULL)
      return false;

   if (name == originName)
   {
      if ((!obj->IsOfType("CelestialBody")) && (!obj->IsOfType("Spacecraft")))
      {
         CoordinateSystemException cse("");
         cse.SetDetails(errorMessageFormatUnnamed.c_str(),
                        (obj->GetName()).c_str(),
                       "Origin", "Celestial Body or Spacecraft");
         throw cse;
      }
   }
   #ifdef DEBUG_SET_REF
   MessageInterface::ShowMessage
      ("BodyFixedAxes::SetRefObject() returning GmatBase::SetRefObject()\n");
   #endif
   return DynamicAxes::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  void BodyFixedAxes::SetSpiceFrameId(std::string spiceId)
//------------------------------------------------------------------------------
/**
 * Sets the spice frame id which turns on using Spice for rotations. 
 * To turn off: set to ""
 *
 * @param spiceId   name of the spice frame to use
 *
 */
 //------------------------------------------------------------------------------
void BodyFixedAxes::SetSpiceFrameId(std::string spiceId)
{
   spiceFrameId = spiceId;
   
   if(spiceFrameId == "")
      spiceIdSet = false; 
   else
      spiceIdSet = true;
}

//------------------------------------------------------------------------------
//  void BodyFixedAxes::SetForGravityUpdate(bool turnOn)
//------------------------------------------------------------------------------
/**
 * Tells the frame whether the next rotation matrix should be for a gravity update
 * This call is used because gravity systems for some bodies are fixed to a certain 
 * frame. This is only Earth and Moon right now as they gravity fixed to specific frames. 
 *
 * @param turnOn   turns on gravity update
 *
 */
 //------------------------------------------------------------------------------
void BodyFixedAxes::SetForGravityUpdate(bool turnOn)
{
   if(originName == GmatSolarSystemDefaults::EARTH_NAME || originName == GmatSolarSystemDefaults::MOON_NAME)
      forGravityUpdate = turnOn;
   else
      forGravityUpdate = false;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch, 
//                               bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000Eq system
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 */
//------------------------------------------------------------------------------
void BodyFixedAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                            bool forceComputation) 
{
   CalculateRotationMatrix(GmatTime(atEpoch.Get()), forceComputation);
}
//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const GmatTime &atEpoch, 
//                               bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000Eq system
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 */
//------------------------------------------------------------------------------
void BodyFixedAxes::CalculateRotationMatrix(const GmatTime &atEpoch,
                                            bool forceComputation) 
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "Calling BFA::CalculateRotationMatrix at epoch %.15lf; \n", atEpoch.GetMjd());
   #endif
   Real theEpoch = atEpoch.GetMjd();
   #ifdef DEBUG_BF_EPOCHS
      MessageInterface::ShowMessage("BFA::CalculateRotationMatrix(%s)   epoch = %12.10f, prevEpoch = %12.10f ...... ",
            (coordName.c_str()), theEpoch, prevEpoch.GetMjd());
   #endif
   #ifdef DEBUG_BF_RECOMPUTE
      MessageInterface::ShowMessage("Entering BFA::CalculateRotationMatrix on object %s (%p) of type %s, origin = %s\n",
            (coordName.c_str()), this, (GetTypeName()).c_str(), originName.c_str());
      MessageInterface::ShowMessage("     epoch = %12.10f, prevEpoch = %12.10f\n", theEpoch, prevEpoch.GetMjd());
   #endif

   // if the origin is a Spacecraft, we need to get attitude data from the
   // spacecraft in order to compute the R and Rdot matrices
   if (origin->IsOfType("Spacecraft"))
   {
      if ((!forceComputation) && (IsEqual(atEpoch, prevEpoch)))
      {
         #ifdef DEBUG_BF_RECOMPUTE
            MessageInterface::ShowMessage(
            "Don't need to recompute R and Rdot BFA for Spacecraft "
            "%s at this time!!\n",
            origin->GetName().c_str());
         #endif
         return;
      }
      #ifdef DEBUG_BF_SC
       if (!firstCallFired)
       {
          MessageInterface::ShowMessage(
                "ABOUT to get dcm (and possibly av) from spacecraft %s\n",
                origin->GetName().c_str());
       }
       #endif
       // First compute the R matrix
      Spacecraft *sc = (Spacecraft*) origin;
      // DCM from spacecraft is inertial-to-body
      Rmatrix33  dcm = sc->GetAttitude(theEpoch);

      // we want the rotation matrix to be body-to-inertial
      rotMatrix      = dcm.Transpose();

      Attitude *att = (Attitude*) sc->GetRefObject(Gmat::ATTITUDE, "");
      if (!att->ModelComputesRates())
      {
         if (allowNoRates)
         {
            rotDotMatrix = Rmatrix33();  // zero matrix
         }
         else
         {
            std::string errmsg = "The value of \"";
            errmsg += origin->GetName() + "\" for field \"Origin\"";
            errmsg += " on BodyFixed coordinate system \"" + coordName;
            errmsg += "\" is not an allowed value.\n";
            errmsg += "The allowed values are: ";
            errmsg += "[ Spacecraft whose attitude model computes rates ].\n";
            throw CoordinateSystemException(errmsg);
         }
      }
      else
      {
         // av from spacecraft is wrt inertial, expressed in inertial
         Rvector3   av  = sc->GetAngularVelocity(theEpoch);
         #ifdef DEBUG_BF_SC
          if (!firstCallFired)
          {
             MessageInterface::ShowMessage("Computing R and Rdot for BFA with spacecraft origin\n");
             MessageInterface::ShowMessage("   Spacecraft = %s\n", sc->GetName().c_str());
             MessageInterface::ShowMessage("   dcm    = %s\n", dcm.ToString().c_str());
             MessageInterface::ShowMessage("   av     = %s\n", av.ToString().c_str());
          }
          #endif

         // rotate angular velocity from inertial to body
         Rvector3  avB = dcm * av;
         // compute Rdot
         Rmatrix33 skew( 0.0,   -avB[2], avB[1],
                         avB[2], 0.0,   -avB[0],
                        -avB[1], avB[0], 0.0);
         Rmatrix33 RdotBI = -skew * dcm;
         // we want Rdot to be body-to-inertial
         rotDotMatrix = RdotBI.Transpose();
         #ifdef DEBUG_BF_SC
         if (!firstCallFired)
         {
            MessageInterface::ShowMessage("   avB    = %s\n", avB.ToString().c_str());
            MessageInterface::ShowMessage("   skew   = %s\n", skew.ToString().c_str());
            MessageInterface::ShowMessage("   RdotBI = %s\n", RdotBI.ToString().c_str());
         }
         #endif
      }
      #ifdef DEBUG_BF_SC
      if (!firstCallFired)
      {
         MessageInterface::ShowMessage("   rotMatrix    = %s\n", RdotBI.ToString().c_str());
         MessageInterface::ShowMessage("   rotDotMatrix = %s\n", RdotBI.ToString().c_str());
      }
      #endif
   }
   else
   {
      // Code must check to see if we need to recompute.  Recomputation is only necessary
      // if one or more of the following conditions are true:
      // 1. the epoch is different (within machine precision) from the epoch at the last computation
      // 2. if the origin is the Earth, the nutation update interval has changed
      // 3. if the origin is Luna, the rotation data source has changed

      if ((!forceComputation)                    &&
          (originName == GmatSolarSystemDefaults::MOON_NAME) &&
          (IsEqual(atEpoch,       prevEpoch))    &&
          (prevLunaSrc == ((CelestialBody*)origin)->GetRotationDataSource()))
         {
            #ifdef DEBUG_BF_RECOMPUTE
               MessageInterface::ShowMessage("Don't need to recompute for Luna at this time!!\n");
            #endif
            return;
         }
      
      // compute rotMatrix and rotDotMatrix
      if (!forGravityUpdate && (spiceIdSet || ((CelestialBody*)origin)->GetRotationDataSource() == Gmat::SPICE_KERNEL))
      {
         std::string spiceToFrame;

         if (spiceIdSet)
            spiceToFrame = spiceFrameId;
         else //((CelestialBody*)origin)->GetRotationDataSource() == Gmat::SPICE_KERNEL)
            spiceToFrame = origin->GetStringParameter(originSpiceIdPId);
         CalculateSpiceFrameRotationMatrix(spiceToFrame, atEpoch);
      }
      else if (originName == GmatSolarSystemDefaults::EARTH_NAME)
      {
         Real intervalFromOrigin = ((Planet*) origin)->GetNutationUpdateInterval();
         if ((!forceComputation)                                     &&
             (IsEqual(atEpoch,            prevEpoch)                 &&
             (IsEqual(intervalFromOrigin, prevOriginUpdateInterval)) &&
             (IsEqual(updateInterval,     prevUpdateInterval))))
         {
            #ifdef DEBUG_BF_EPOCHS
               MessageInterface::ShowMessage("NOT recomputing\n");
            #endif
            #ifdef DEBUG_BF_RECOMPUTE
               MessageInterface::ShowMessage("Don't need to recompute for Earth at this time!!\n");
            #endif
            return;
         }
   
         #ifdef DEBUG_BF_EPOCHS
            MessageInterface::ShowMessage("RECOMPUTING!!!\n");
         #endif
         #ifdef DEBUG_BF_RECOMPUTE
            MessageInterface::ShowMessage("   RECOMPUTING!!! - body name is EARTH\n");
         #endif
         #ifdef DEBUG_FIRST_CALL
            if (!firstCallFired)
               MessageInterface::ShowMessage("In BFA, Body is the Earth\n");
         #endif
         Real dPsi             = 0.0;
         Real longAscNodeLunar = 0.0;
         Real cosEpsbar        = 0.0;
         Real cosAst           = 0.0;
         Real sinAst           = 0.0;

         // Convert to MJD UTC to use for polar motion  and LOD
         // interpolations
         GmatTime mjdUTC = theTimeConverter->Convert(atEpoch,
                       TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
                       JD_JAN_5_1941);
         Real offset = JD_JAN_5_1941 - JD_NOV_17_1858;
         // convert to MJD referenced from time used in EOP file
         mjdUTC = mjdUTC + offset;


         // convert input time to UT1 for later use (for AST calculation)
         GmatTime mjdUT1 = theTimeConverter->Convert(atEpoch,
                       TimeSystemConverter::A1MJD, TimeSystemConverter::UT1,
                       JD_JAN_5_1941);

         // convert input A1 MJD to TT MJD (for most calculations)
         GmatTime mjdTT = theTimeConverter->Convert(atEpoch,
                      TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
                      JD_JAN_5_1941);
         GmatTime jdTT    = mjdTT + JD_JAN_5_1941; // right?
         // Compute Julian centuries of TDB from the base epoch (J2000)
         // NOTE - this is really TT, an approximation of TDB *********
         Real tDiff = JD_JAN_5_1941 - JD_OF_J2000;
         Real tTDB    = (mjdTT + tDiff).GetMjd() / DAYS_PER_JULIAN_CENTURY;

         #ifdef DEBUG_FIRST_CALL
            if (!firstCallFired)
            {
               Real jdUT1    = mjdUT1.GetMjd() + JD_JAN_5_1941; // right?
               MessageInterface::ShowMessage(
                  "   Epoch data[mjdUTC, mjdUT1, jdUT1, tUT1, mjdTT1, jdTT, tTDB] "
                  "=\n        [%.15lf %.15lf %.15lf %.15lf %.15lf %.15lf ]\n",
                  mjdUTC, mjdUT1, jdUT1, mjdTT, jdTT, tTDB);
            }
         #endif

         #ifdef DEBUG_BF_ROT_MATRIX
            MessageInterface::ShowMessage("   about to figure out update interval ...\n");
         #endif
         if (overrideOriginInterval)
         {
            updateIntervalToUse = updateInterval;
            //MessageInterface::ShowMessage("Overriding origin interval .....\n");
            #ifdef DEBUG_FIRST_CALL
               if (!firstCallFired)
                  MessageInterface::ShowMessage(
                     "   Overrode origin interval; set to %.12lf\n",
                     updateIntervalToUse);
            #endif
         }
         else
         {
            updateIntervalToUse = intervalFromOrigin;
            #ifdef DEBUG_FIRST_CALL
               if (!firstCallFired)
                  MessageInterface::ShowMessage(
                     "   Using body's origin interval, %.12lf\n",
                     updateIntervalToUse);
            #endif
         }

         #ifdef DEBUG_BF_ROT_MATRIX
            MessageInterface::ShowMessage("About to call ComputePrecessionMatrix\n");
         #endif
         ComputePrecessionMatrix(tTDB, atEpoch.GetMjd());
         #ifdef DEBUG_BF_ROT_MATRIX
            MessageInterface::ShowMessage("About to call ComputeNutationMatrix\n");
         #endif
         ComputeNutationMatrix(tTDB, atEpoch.GetMjd(), dPsi, longAscNodeLunar, cosEpsbar,
                               forceComputation);
         #ifdef DEBUG_BF_ROT_MATRIX
            MessageInterface::ShowMessage("About to call ComputeSiderealTimeRotation\n");
         #endif
         ComputeSiderealTimeRotation(jdTT.GetMjd(), mjdUT1, dPsi, longAscNodeLunar, cosEpsbar,
                                cosAst, sinAst);
         #ifdef DEBUG_BF_ROT_MATRIX
            MessageInterface::ShowMessage("About to call ComputeSiderealTimeDotRotation\n");
         #endif
         ComputeSiderealTimeDotRotation(mjdUTC, atEpoch, cosAst, sinAst,
                                        forceComputation);
         #ifdef DEBUG_BF_ROT_MATRIX
            MessageInterface::ShowMessage("About to call ComputePolarMotionRotation\n");
         #endif
         ComputePolarMotionRotation(mjdUTC.GetMjd(), atEpoch.GetMjd(), forceComputation);
         #ifdef DEBUG_BF_ROT_MATRIX
            MessageInterface::ShowMessage("DONE calling all computation submethods\n");
         #endif

      #ifdef DEBUG_BF_MATRICES
         MessageInterface::ShowMessage("atEpoch = %.15f\n", atEpoch.GetMjd());
        MessageInterface::ShowMessage("PREC = %s\n", PREC.ToString().c_str());
         MessageInterface::ShowMessage("NUT = %s\n", NUT.ToString().c_str());
         MessageInterface::ShowMessage("ST = %s\n", ST.ToString().c_str());
         MessageInterface::ShowMessage("PM = %s\n", PM.ToString().c_str());
        MessageInterface::ShowMessage("STderiv = %s\n", STderiv.ToString().c_str());
      #endif

         Real np[3][3], rot[3][3], tmp[3][3];
         Integer p3;

         // NUT * PREC
         for (Integer p = 0; p < 3; ++p)
         {
            p3 = 3*p;
            for (Integer q = 0; q < 3; ++q)
            {
               np[p][q] = nutData[p3]   * precData[q]   +
                          nutData[p3+1] * precData[q+3] +
                          nutData[p3+2] * precData[q+6];
            }
         }

         // ST * (NUT * PREC)
         for (Integer p = 0; p < 3; ++p)
         {
            p3 = 3*p;
            for (Integer q = 0; q < 3; ++q)
            {
               tmp[p][q] = stData[p3]   * np[0][q] +
                           stData[p3+1] * np[1][q] +
                           stData[p3+2] * np[2][q];
            }
         }

         // PM * (ST * (NUT * PREC))
         for (Integer p = 0; p < 3; ++p)
         {
            p3 = 3*p;
            for (Integer q = 0; q < 3; ++q)
            {
               rot[p][q] = pmData[p3]   * tmp[0][q] +
                           pmData[p3+1] * tmp[1][q] +
                           pmData[p3+2] * tmp[2][q];
            }
         }

         rotMatrix.Set(rot[0][0], rot[1][0], rot[2][0],
                       rot[0][1], rot[1][1], rot[2][1],
                       rot[0][2], rot[1][2], rot[2][2]);

         #ifdef DEBUG_BF_CHECK_DETERMINANT
            Real determinant =
                    rot[0][0] * (rot[1][1] * rot[2][2] - rot[1][2]*rot[2][1]) +
                    rot[0][1] * (rot[1][2] * rot[2][0] - rot[1][0]*rot[2][2]) +
                    rot[0][2] * (rot[1][0] * rot[2][1] - rot[1][1]*rot[2][0]);
            if (Abs(determinant - 1.0) > DETERMINANT_TOLERANCE)
               throw CoordinateSystemException(
                     "Computed rotation matrix has a determinant not equal to 1.0");
         #endif

         // NUT * PREC calculated above
         // STderiv * (NUT * PREC)
         for (Integer p = 0; p < 3; ++p)
         {
            p3 = 3*p;
            for (Integer q = 0; q < 3; ++q)
            {
               tmp[p][q] = stDerivData[p3]   * np[0][q] +
                           stDerivData[p3+1] * np[1][q] +
                           stDerivData[p3+2] * np[2][q];
            }
         }

         // PM * (STderiv * (NUT * PREC))
         for (Integer p = 0; p < 3; ++p)
         {
            p3 = 3*p;
            for (Integer q = 0; q < 3; ++q)
            {
               rot[p][q] = pmData[p3]   * tmp[0][q] +
                           pmData[p3+1] * tmp[1][q] +
                           pmData[p3+2] * tmp[2][q];
            }
         }
         rotDotMatrix.Set(rot[0][0], rot[1][0], rot[2][0],
                          rot[0][1], rot[1][1], rot[2][1],
                          rot[0][2], rot[1][2], rot[2][2]);

         // save the data to compare against next time
         prevUpdateInterval       = updateInterval;
         prevOriginUpdateInterval = intervalFromOrigin;

         #ifdef DEBUG_FIRST_CALL
            if (!firstCallFired)
            {
               MessageInterface::ShowMessage("FK5 Components\n   PREC = \n");
               for (Integer m = 0; m < 3; ++m)
                  MessageInterface::ShowMessage(
                     "          %20.12le %20.12le %20.12le \n", PREC(m,0),
                     PREC(m,1), PREC(m,2));
               MessageInterface::ShowMessage("\n   NUT = \n");
               for (Integer m = 0; m < 3; ++m)
                  MessageInterface::ShowMessage(
                     "          %20.12le %20.12le %20.12le \n", NUT(m,0),
                     NUT(m,1), NUT(m,2));
               MessageInterface::ShowMessage("\n   ST = \n");
               for (Integer m = 0; m < 3; ++m)
                  MessageInterface::ShowMessage(
                     "          %20.12le %20.12le %20.12le \n", ST(m,0),
                     ST(m,1), ST(m,2));
               MessageInterface::ShowMessage("\n   STderiv = \n");
               for (Integer m = 0; m < 3; ++m)
                  MessageInterface::ShowMessage(
                     "          %20.12le %20.12le %20.12le \n", STderiv(m,0),
                     STderiv(m,1), STderiv(m,2));
               MessageInterface::ShowMessage("\n   PM = \n");
               for (Integer m = 0; m < 3; ++m)
                  MessageInterface::ShowMessage(
                     "          %20.12le %20.12le %20.12le \n", PM(m,0),
                     PM(m,1), PM(m,2));
               MessageInterface::ShowMessage("\n   rotMatrix = \n");
               for (Integer m = 0; m < 3; ++m)
                  MessageInterface::ShowMessage(
                     "          %20.12le %20.12le %20.12le \n", rotMatrix(m,0),
                     rotMatrix(m,1), rotMatrix(m,2));
            }
         #endif

      }
      //Checks for SpiceKernel && forGravityUpdate as under a forGravityUpdate we should use this if Spice_Kernel is set
      else if ((originName == GmatSolarSystemDefaults::MOON_NAME) &&
              (((CelestialBody*)origin)->GetRotationDataSource() == Gmat::DE_FILE || (((CelestialBody*)origin)->GetRotationDataSource() == Gmat::SPICE_KERNEL && forGravityUpdate)))
      {
         #ifdef DEBUG_BF_RECOMPUTE
            MessageInterface::ShowMessage("   RECOMPUTING!!! - body name is LUNA\n");
         #endif
         if (!de)
         {
            de = (DeFile*) ((CelestialBody*)origin)->GetSourceFile();
            if (!de)
            throw CoordinateSystemException(
                  "No DE file specified - cannot get Moon data");
            // De file is initialized in its constructor
            #ifdef DEBUG_DE_FILE_USED
               MessageInterface::ShowMessage(
                                    "In BFA, de file used is <%p> of type %d\n",
                                    de, de->GetDeFileType());
            #endif
         }
         bool override = ((CelestialBody*)origin)->GetOverrideTimeSystem();
         Real librationAngles[3], andRates[3];
         de->GetAnglesAndRates(atEpoch, librationAngles, andRates, override);

         rotMatrix    = (AttitudeConversionUtility::ToCosineMatrix(librationAngles, 3, 1, 3)).Transpose();
         Real ca1    = cos(librationAngles[0]);
         Real ca2    = cos(librationAngles[1]);
         Real ca3    = cos(librationAngles[2]);
         Real sa1    = sin(librationAngles[0]);
         Real sa2    = sin(librationAngles[1]);
         Real sa3    = sin(librationAngles[2]);
         Real s1c2   = sa1*ca2;
         Real s1c3   = sa1*ca3;
         Real s2c3   = sa2*ca3;
         Real s3c1   = sa3*ca1;
         Real s3c2   = sa3*ca2;
         Real s1s2   = sa1*sa2;
         Real s1s3   = sa1*sa3;
         Real c1c2   = ca1*ca2;
         Real c1c3   = ca1*ca3;
         Real c2c3   = ca2*ca3;

         Real s3c2s1 = s3c2*sa1;
         Real c1c2c3 = ca1*c2c3;
         Real s3c1c2 = s3c1*ca2;
         Real s1c2c3 = c2c3*sa1;

         rotDotMatrix.Set(
            -andRates[2]*(s3c1+s1c2c3) + andRates[1]*sa3*s1s2  - andRates[0]*(s1c3+s3c1c2),
            -andRates[2]*(c1c3-s3c2s1) + andRates[1]*ca3*s1s2  + andRates[0]*(s1s3-c1c2c3),
                                         andRates[1]*s1c2      + andRates[0]*sa2*ca1,
            -andRates[2]*(s1s3-c1c2c3) - andRates[1]*s3c1*sa2  + andRates[0]*(c1c3-s3c2s1),
            -andRates[2]*(s1c3+s3c1c2) - andRates[1]*s2c3*ca1  - andRates[0]*(s3c1+s1c2c3),
                                       - andRates[1]*c1c2      + andRates[0]*s1s2,
              andRates[2]*s2c3          + andRates[1]*s3c2,
             -andRates[2]*sa2*sa3       + andRates[1]*c2c3,
                                        - andRates[1]*sa2
            );

          #ifdef DEBUG_MOON_MATRIX
             MessageInterface::ShowMessage("angles are: %.17f  %.17f  %.17f\n",
              librationAngles[0], librationAngles[1], librationAngles[2]);
             MessageInterface::ShowMessage("rates are: %.17f  %.17f  %.17f\n",
              andRates[0], andRates[1], andRates[2]);
             MessageInterface::ShowMessage(
             "rotMatrix = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f %.17f  %.17  %.17ff\n",
              rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
              rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
              rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
             MessageInterface::ShowMessage(
             "rotDotMatrix = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f %.17f  %.17f %.17f\n",
              rotDotMatrix(0,0), rotDotMatrix(0,1), rotDotMatrix(0,2),
              rotDotMatrix(1,0), rotDotMatrix(1,1), rotDotMatrix(1,2),
              rotDotMatrix(2,0), rotDotMatrix(2,1), rotDotMatrix(2,2));
         #endif
      }
      else // compute for other bodies, using IAU data
      {
         #ifdef DEBUG_BF_RECOMPUTE
            MessageInterface::ShowMessage("   RECOMPUTING!!! - and the body name is %s\n", originName.c_str());
          MessageInterface::ShowMessage("   IAUSimplified method\n");
         #endif
         Real Wderiv[9];
         Real R13[3][3];
         Real rotResult[3][3];
         Real rotDotResult[3][3];
         static Rvector cartCoord(4);
         const Real *cartC = cartCoord.GetDataVector();
         #ifdef DEBUG_FIRST_CALL
            if (!firstCallFired)
               MessageInterface::ShowMessage("In BFA, Body is %s\n", originName.c_str());
         #endif
         // this method will return alpha (deg), delta (deg),
         // W (deg), and Wdot (deg/day)
         cartCoord           = ((CelestialBody*)origin)->
                               GetBodyCartographicCoordinates(atEpoch.GetMjd());
         Real rot1           = GmatMathConstants::PI_OVER_TWO + Rad(cartC[0]);
         Real rot2           = GmatMathConstants::PI_OVER_TWO - Rad(cartC[1]);
         Real W              = Rad(cartC[2]);
         Real Wdot           = Rad(cartC[3]) / SECS_PER_DAY;
         // Convert Wdot from deg/day to rad/sec
         Real R3leftT[9]     =  {Cos(rot1),-Sin(rot1),0.0,
                                 Sin(rot1), Cos(rot1),0.0,
                                       0.0,       0.0,1.0};
         Real R1middleT[9]   =  {1.0,      0.0,       0.0,
                                 0.0,Cos(rot2),-Sin(rot2),
                                 0.0,Sin(rot2), Cos(rot2)};

         Real R3rightT[9]    =  {Cos(W),-Sin(W),0.0,
                                 Sin(W), Cos(W),0.0,
                                    0.0,    0.0,1.0};
         
        #ifdef DEBUG_BF_RECOMPUTE
           MessageInterface::ShowMessage("At time: %f\n",atEpoch.GetMjd());
           MessageInterface::ShowMessage(" RA = %f rad\n", rot1 - GmatMathConstants::PI_OVER_TWO);
          MessageInterface::ShowMessage(" DEC = %f rad\n", -rot2 + GmatMathConstants::PI_OVER_TWO);
          MessageInterface::ShowMessage(" W = %f rad\n", W);
          MessageInterface::ShowMessage("R3T(Pi/2+RA) = [\n");
          for (int i=0; i < 3; ++i)
          {
             MessageInterface::ShowMessage("  %e,   %e,   %e\n", R3leftT[i*3], R3leftT[i*3+1], R3leftT[i*3+2]);
          }
          MessageInterface::ShowMessage("]\n");
          MessageInterface::ShowMessage("R1T(Pi/2-DEC) = [\n");
          for (int i=0; i < 3; ++i)
          {
             MessageInterface::ShowMessage("  %e,   %e,   %e\n", R1middleT[i*3], R1middleT[i*3+1], R1middleT[i*3+2]);
          }
          MessageInterface::ShowMessage("]\n");
          MessageInterface::ShowMessage("R3T(W) = [\n");
          for (int i=0; i < 3; ++i)
          {
             MessageInterface::ShowMessage("  %e,   %e,   %e\n", R3rightT[i*3], R3rightT[i*3+1], R3rightT[i*3+2]);
          }
          MessageInterface::ShowMessage("]\n");
         #endif

         Integer p3 = 0;
         for (Integer p = 0; p < 3; ++p)
         {
            p3 = 3*p;
            for (Integer q = 0; q < 3; ++q)
            {
               R13[p][q] = R1middleT[p3]   * R3rightT[q]   +
                           R1middleT[p3+1] * R3rightT[q+3] +
                           R1middleT[p3+2] * R3rightT[q+6];
            }
         }
         for (Integer p = 0; p < 3; ++p)
         {
            p3 = 3*p;
            for (Integer q = 0; q < 3; ++q)
            {
               rotResult[p][q] = R3leftT[p3]   * R13[0][q] +
                                 R3leftT[p3+1] * R13[1][q] + 
                                 R3leftT[p3+2] * R13[2][q];
            }
         }
         rotMatrix.Set(rotResult[0][0],rotResult[0][1],rotResult[0][2],
                       rotResult[1][0],rotResult[1][1],rotResult[1][2],
                       rotResult[2][0],rotResult[2][1],rotResult[2][2]);

         Wderiv[0] = -Wdot*Sin(W);
         Wderiv[1] = -Wdot*Cos(W);
         Wderiv[2] =  0.0;
         Wderiv[3] =  Wdot*Cos(W);
         Wderiv[4] = -Wdot*Sin(W);
         Wderiv[5] =  0.0;
         Wderiv[6] =  0.0;
         Wderiv[7] =  0.0;
         Wderiv[8] =  0.0;

         for (Integer p = 0; p < 3; ++p)
         {
            p3 = 3*p;
            for (Integer q = 0; q < 3; ++q)
            {
               R13[p][q] = R1middleT[p3]   * Wderiv[q]   +
                           R1middleT[p3+1] * Wderiv[q+3] +
                           R1middleT[p3+2] * Wderiv[q+6];
            }
         }
         for (Integer p = 0; p < 3; ++p)
         {
            p3 = 3*p;
            for (Integer q = 0; q < 3; ++q)
            {
               rotDotResult[p][q] = R3leftT[p3]   * R13[0][q] +
                                    R3leftT[p3+1] * R13[1][q] +
                                    R3leftT[p3+2] * R13[2][q];
            }
         }
         rotDotMatrix.Set(rotDotResult[0][0],rotDotResult[0][1],rotDotResult[0][2],
                          rotDotResult[1][0],rotDotResult[1][1],rotDotResult[1][2],
                          rotDotResult[2][0],rotDotResult[2][1],rotDotResult[2][2]);
      }
   }   // end else
      
   // Save the epoch for comparison the next time through
   prevEpoch = atEpoch;
   #ifdef DEBUG_BF_RECOMPUTE
      MessageInterface::ShowMessage("at the end, just set prevEpoch to %12.10f\n", prevEpoch.GetMjd());
   #endif
   if (originName == GmatSolarSystemDefaults::MOON_NAME)
      prevLunaSrc = ((CelestialBody*)origin)->GetRotationDataSource();

   #ifdef DEBUG_FIRST_CALL
      firstCallFired = true;
      MessageInterface::ShowMessage("NOW exiting BFA::CalculateRotationMatrix ...\n");
   #endif

}
