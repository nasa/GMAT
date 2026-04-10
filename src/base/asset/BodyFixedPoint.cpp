//$Id$
//------------------------------------------------------------------------------
//                            BodyFixedPoint
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
// number NNG06CA54C
//
// Author: Wendy C. Shoan, NASA/GSFC (moved from GroundStation code,
//         original author: Darrel J. Conway, Thinking Systems, Inc.)
// Created: 2008.08.22

// Modified:
//    2010.03.25 Thomas Grubb
//      - Fixed check for latitude to be between -90 and 90
//    2010.03.23 Thomas Grubb/Steve Hughes
//      - Fixed SetStringParameter to correctly check for statetype values
//    2010.03.19 Thomas Grubb
//      - Overrode Copy method
//      - Changed StateType values from (Cartesian, Geographical) to
//        (Cartesian, Spherical (Geographical deprecates to Spherical))
//      - Added Location Units labels code
//      - Added checks that Latitude, Longitude between 0 and 360 and altitude
//        greater than or equal to 0
//
/**
 * Implements the Groundstation class used to model ground based tracking stations.
 */
//------------------------------------------------------------------------------

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "BodyFixedPoint.hpp"
#include "AssetException.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include "StringUtil.hpp"
#include "TimeTypes.hpp"
#include "GmatDefaults.hpp"
#include "CoordinateSystem.hpp"
#include "FileManager.hpp"
#include "GmatConstants.hpp"
#include "FileUtil.hpp"


//#define DEBUG_OBJECT_MAPPING
//#define DEBUG_INIT
//#define DEBUG_BF_REF
//#define TEST_BODYFIXED_POINT
//#define DEBUG_BODYFIXED_STATE
//#define DEBUG_BODYFIXED_SET_REAL
//#define DEBUG_BFP_SPICE
//#define DEBUG_FK_SPICE


//---------------------------------
// static data
//---------------------------------

/// Labels used for the ground station parameters.
const std::string
BodyFixedPoint::PARAMETER_TEXT[BodyFixedPointParamCount - SpacePointParamCount] =
   {
         "CentralBody",
         "StateType",         // Cartesian or Spherical
         "HorizonReference",  // Sphere or Ellipsoid
         "Location1",         // X or Latitude value
         "Location2",         // Y or Longitude value
         "Location3",         // Z or Altitude value
         "LOCATION_LABEL_1",  // "X" or "Latitude"
         "LOCATION_LABEL_2",  // "Y" or "Longitude"
         "LOCATION_LABEL_3"   // "Z" or "Altitude"
         "LOCATION_UNITS_1",  // "km" or "deg"
         "LOCATION_UNITS_2",  // "km" or "deg"
         "LOCATION_UNITS_3"   // "km" or "km"
   };

const Gmat::ParameterType
BodyFixedPoint::PARAMETER_TYPE[BodyFixedPointParamCount - SpacePointParamCount] =
   {
         Gmat::OBJECT_TYPE,
         Gmat::ENUMERATION_TYPE,
         Gmat::ENUMERATION_TYPE,
         Gmat::REAL_TYPE,
         Gmat::REAL_TYPE,
         Gmat::REAL_TYPE,
         Gmat::STRING_TYPE,
         Gmat::STRING_TYPE,
         Gmat::STRING_TYPE,
         Gmat::STRING_TYPE,
         Gmat::STRING_TYPE,
         Gmat::STRING_TYPE,
   };


Integer BodyFixedPoint::gsNaifId = 999;

//---------------------------------
// public methods
//---------------------------------


//---------------------------------------------------------------------------
//  BodyFixedPoint(const std::string &itsName)
//---------------------------------------------------------------------------
/**
 * Constructs a BodyFixedPoint object (default constructor).
 *
 * @param <itsName> Optional name for the object.  Defaults to "".
 */
//---------------------------------------------------------------------------
BodyFixedPoint::BodyFixedPoint(const std::string &itsType, const std::string &itsName,
      const UnsignedInt objType) :
   SpacePoint           (objType, itsType, itsName),
   cBodyName            ("Earth"),
   theBody              (NULL),
   meanEquatorialRadius (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]),
   flattening           (GmatSolarSystemDefaults::PLANET_FLATTENING[GmatSolarSystemDefaults::EARTH]),
   stateType            ("Cartesian"),
   horizon              ("Sphere"),
//   solarSystem          (NULL),
   bfcsName             (""),
   bfcs                 (NULL),
   mj2kcsName           (""),
   mj2kcs               (NULL),
   lastStateTime        (GmatTimeConstants::MJD_OF_J2000),
   lastStateTimeGT      (GmatTimeConstants::MJD_OF_J2000),
   runEpoch             (0.0),
   kernelBaseName       (""),
   spkName              (""),
   fkName               (""),
   deleteSPK            (true),
   deleteFK             (true),
   naifIDDetermined     (false),
   kernelNamesDetermined (false),
   kernelsWritten       (false)
{
   objectTypes.push_back(Gmat::BODY_FIXED_POINT);
   objectTypeNames.push_back("BodyFixedPoint");
   parameterCount = BodyFixedPointParamCount;

   // assumes StateType = Cartesian
   locationLabels.push_back("X");
   locationLabels.push_back("Y");
   locationLabels.push_back("Z");

   // assumes StateType = Cartesian
   locationUnits.push_back("km");
   locationUnits.push_back("km");
   locationUnits.push_back("km");

   location[0] = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH];
   location[1] = 0.0;
   location[2] = 0.0;

   bfLocation[0] = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH];
   bfLocation[1] = 0.0;
   bfLocation[2] = 0.0;

   #ifdef __USE_SPICE__
      spice = NULL;
   #endif

}

//---------------------------------------------------------------------------
// ~BodyFixedPoint()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
BodyFixedPoint::~BodyFixedPoint()
{
   #ifdef __USE_SPICE__
      if (kernelsWritten)
      {
         if (deleteSPK && (spkName != ""))
         {
            #ifdef DEBUG_BFP_SPICE
               MessageInterface::ShowMessage("**** In BFP, About to unload %s\n",
                     spkName.c_str());
            #endif
            spice->UnloadKernel(spkName);
            remove(spkName.c_str());
         }
         if (deleteFK && (fkName != ""))
         {
            #ifdef DEBUG_BFP_SPICE
               MessageInterface::ShowMessage("**** In BFP, About to unload %s\n",
                     fkName.c_str());
            #endif
            spice->UnloadKernel(fkName);
            remove(fkName.c_str());
         }
      }

      if (spice) delete spice;
   #endif

      if (mj2kcs != nullptr)
      {
         delete mj2kcs;
         mj2kcs = nullptr;
      }
      if (bfcs != nullptr)
      {
         delete bfcs;
         bfcs = nullptr;
      }
}

//---------------------------------------------------------------------------
//  BodyFixedPoint(const BodyFixedPoint& bfp)
//---------------------------------------------------------------------------
/**
 * Constructs a new BodyFixedPoint by copying the input instance (copy
 * constructor).
 *
 * @param bfp  BodyFixedPoint instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
BodyFixedPoint::BodyFixedPoint(const BodyFixedPoint& bfp) :
   SpacePoint           (bfp),
   cBodyName            (bfp.cBodyName),
   theBody              (NULL),
   meanEquatorialRadius (bfp.meanEquatorialRadius),
   flattening           (bfp.flattening),
   locationLabels       (bfp.locationLabels),
   locationUnits        (bfp.locationUnits),
   stateType            (bfp.stateType),
   horizon              (bfp.horizon),
//   solarSystem          (NULL),
   bfcsName             (bfp.bfcsName),
   bfcs                 (NULL),
   mj2kcsName           (bfp.mj2kcsName),
   mj2kcs               (NULL),
   lastStateTime        (bfp.lastStateTime),
   lastStateTimeGT      (bfp.lastStateTimeGT),
   lastState            (bfp.lastState),
   runEpoch             (bfp.runEpoch),
   kernelBaseName       (bfp.kernelBaseName),
   spkName              (bfp.spkName),
   fkName               (bfp.fkName),
   deleteSPK            (bfp.deleteSPK),
   deleteFK             (bfp.deleteFK),
   naifIDDetermined     (bfp.naifIDDetermined), // ??
   kernelNamesDetermined (false),
   kernelsWritten       (false)
{
   location[0]   = bfp.location[0];
   location[1]   = bfp.location[1];
   location[2]   = bfp.location[2];;

   bfLocation[0] = bfp.bfLocation[0];
   bfLocation[1] = bfp.bfLocation[1];
   bfLocation[2] = bfp.bfLocation[2];

   #ifdef __USE_SPICE__
      spice = NULL;
   #endif

}


//---------------------------------------------------------------------------
//  BodyFixedPoint& operator=(const BodyFixedPoint& bfp)
//---------------------------------------------------------------------------
/**
 * Assignment operator for BodyFixedPoints.
 *
 * @param bfp The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
BodyFixedPoint& BodyFixedPoint::operator=(const BodyFixedPoint& bfp)
{
   if (&bfp != this)
   {
      SpacePoint::operator=(bfp);

      cBodyName            = bfp.cBodyName;
      theBody              = bfp.theBody;
      meanEquatorialRadius = bfp.meanEquatorialRadius;
      flattening           = bfp.flattening;
      locationLabels       = bfp.locationLabels;
      locationUnits        = bfp.locationUnits;
      stateType            = bfp.stateType;
      horizon              = bfp.horizon;
//      solarSystem          = bfp.solarSystem;
      bfcsName             = bfp.bfcsName;
      //bfcs                 = bfp.bfcs;       // yes or no?
      bfcs                 = NULL;
      mj2kcsName           = bfp.mj2kcsName;
      //mj2kcs               = bfp.mj2kcs;     // yes or no?
      mj2kcs               = NULL;
      lastStateTime        = bfp.lastStateTime;
      lastStateTimeGT      = bfp.lastStateTimeGT;
      lastState            = bfp.lastState;
      runEpoch             = bfp.runEpoch;
      kernelBaseName       = bfp.kernelBaseName;
      spkName              = bfp.spkName;
      fkName               = bfp.fkName;
      deleteSPK            = bfp.deleteSPK;
      deleteFK             = bfp.deleteFK;
      naifIDDetermined     = bfp.naifIDDetermined;
      kernelNamesDetermined = bfp.kernelNamesDetermined;
      kernelsWritten       = bfp.kernelsWritten;

      location[0]          = bfp.location[0];
      location[1]          = bfp.location[1];
      location[2]          = bfp.location[2];;

      bfLocation[0]        = bfp.bfLocation[0];
      bfLocation[1]        = bfp.bfLocation[1];
      bfLocation[2]        = bfp.bfLocation[2];

      #ifdef __USE_SPICE__
         spice = NULL;
      #endif

   }

   return *this;
}

//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Initializes this object.
 *
 */
//---------------------------------------------------------------------------
bool BodyFixedPoint::Initialize()
{
   // Initialize the body data
   if (!theBody)
   {
      if (!theSolarSystem)
         throw AssetException("Unable to initialize ground station " +
               instanceName + "; its solar system is not set\n");
      theBody = theSolarSystem->GetBody(cBodyName);
      if (!theBody)
      {
         std::string errmsg = "Unable to initialize ground station ";
         errmsg += instanceName + "; its central body \"";
         errmsg += cBodyName + "\" has not been set or is invalid.  ";
         errmsg += "The allowed values are: [valid SpacePoint] \n";
         throw AssetException(errmsg);
      }
   }
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("...BodyFixedPoint now has body %s\n",
                                    cBodyName.c_str());
   #endif

   // Get required data from the body
   flattening            = theBody->GetRealParameter("Flattening");
   meanEquatorialRadius  = theBody->GetRealParameter("EquatorialRadius");

   // @todo: This should work, but doesn't.  It needs to be figured out
   //        eventually, I think.
   //
   //        One clue: When walking through this code in nemiver (Linux
   //        debugger), I saw a covariance matrix destructor being called,
   //        and did not expect that.  Maybe there is something around that
   //        piece...
   
   // set up local coordinate systems - delete the old ones if they have already been created
   //if (mj2kcs)   delete mj2kcs;
   //if (bfcs)     delete bfcs;
   if (!mj2kcs)
   {
      mj2kcsName = "mj2kcs";
      // This needs to be relative to the J2000Body in order for this to work
      // for non-Earth bodies
      mj2kcs     = CoordinateSystem::CreateLocalCoordinateSystem(mj2kcsName,
                   "MJ2000Eq", theBody->GetJ2000Body(), //theBody,
                   NULL, NULL, theBody->GetJ2000Body(), theSolarSystem);
   }
   if (!bfcs)
   {
      bfcsName = "bfcs";
      bfcs     = CoordinateSystem::CreateLocalCoordinateSystem(bfcsName,
                 "BodyFixed", theBody,
                 NULL, NULL, theBody->GetJ2000Body(), theSolarSystem);
   }

   if (!spiceCS)
   {
      if (GmatGlobal::Instance()->InTestingMode())
      {
         if (theBody->IsOfType("CelestialBody"))
         {
            if (((CelestialBody*)theBody)->GetRotationDataSource() == Gmat::SPICE_KERNEL || spiceFrameID != "")
            {
               std::string spiceToFrame;
               if (spiceFrameID != "")
                  spiceToFrame = spiceFrameID;
               else //((CelestialBody*)origin)->GetRotationDataSource() == Gmat::SPICE_KERNEL)
                  spiceToFrame = theBody->GetStringParameter("SpiceFrameId");

               spiceCS = CoordinateSystem::CreateLocalCoordinateSystem("spiceCS",
                  "SPICE", theBody,
                  NULL, NULL, theBody->GetJ2000Body(), theSolarSystem);
               spiceCS->SetStringParameter("SpiceFrameId", spiceToFrame);
            }
         }
      }
   }

   // Calculate the body-fixed Cartesian position
   // If it was input in Cartesian, we're done
   UpdateBodyFixedLocation();

   if (!naifIDDetermined)
   {
      // Initialize/set the Naif IDs
      Integer bodyNaif = theBody->GetIntegerParameter("NAIFId");
      naifId           = bodyNaif * 1000 + gsNaifId--;
      naifIdRefFrame   = naifId + 1000000;

      naifIDDetermined = true;
   }

   // Set the initial epoch so that the state is computed at the start
   SetEpoch(GetEpoch());

   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("...BodyFixedPoint %s Initialized!\n", instanceName.c_str());
   #endif

   #ifdef TEST_BODYFIXED_POINT
      MessageInterface::ShowMessage("For %s, %s %s location [%lf "
            "%lf %lf] --> XYZ [%lf %lf %lf]\n", instanceName.c_str(),
            stateType.c_str(), horizon.c_str(), location[0], location[1],
            location[2], bfLocation[0], bfLocation[1], bfLocation[2]);
      // Check the MJ2000 methods
      if (theBody == NULL)
      {
         MessageInterface::ShowMessage(
               "Error initializing ground station %s: theBody is not set\n",
               instanceName.c_str());
         return false;
      }
      if (bfcs == NULL)
      {
         MessageInterface::ShowMessage(
               "Error initializing ground station %s: bfcs is not set\n",
               instanceName.c_str());
         return false;
      }
      if (mj2kcs == NULL)
      {
         MessageInterface::ShowMessage(
               "Error initializing ground station %s: mj2kcs is not set\n",
               instanceName.c_str());
         return false;
      }

      Rvector6 j2kState = GetMJ2000State(GmatTimeConstants::MJD_OF_J2000);
      MessageInterface::ShowMessage("The resulting MJ2000 Cartesian state is "
            "\n   [%s]\n", j2kState.ToString(16).c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// Real GetEpoch()
//------------------------------------------------------------------------------
/**
 * Accessor for the current epoch of the object, in A.1 Modified Julian format.
 *
 * @return The A.1 epoch.
 *
 * @todo The epoch probably should be TAI throughout GMAT.
 */
//------------------------------------------------------------------------------
Real BodyFixedPoint::GetEpoch()
{
   Real theEpoch = (runEpoch == GmatGlobal::Instance()->GetCurrentEpoch() ?
      lastStateTime.Get() : GmatGlobal::Instance()->GetCurrentEpoch());


   return theEpoch; // lastEphemTime.Get();
}


//------------------------------------------------------------------------------
// Real SetEpoch(const Real ep)
//------------------------------------------------------------------------------
/**
 * Accessor used to set epoch (in A.1 Modified Julian format) of the object.
 *
 * @param <ep> The new A.1 epoch.
 *
 * @return The updated A.1 epoch.
 *
 * @todo The epoch probably should be TAI throughout GMAT.
 */
//------------------------------------------------------------------------------
Real BodyFixedPoint::SetEpoch(const Real ep)
{
   A1Mjd a1(ep);
   GetMJ2000State(a1);
   runEpoch = GmatGlobal::Instance()->GetCurrentEpoch();
   return lastStateTime.Get();
}

Rvector6 BodyFixedPoint::GetLastState()
{
   return lastState;
}



// Parameter access methods - overridden from GmatBase

//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void BodyFixedPoint::Copy(const GmatBase* orig)
{
   operator=(*((BodyFixedPoint *)(orig)));
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string BodyFixedPoint::GetParameterText(const Integer id) const
{
   if (id >= SpacePointParamCount && id < BodyFixedPointParamCount)
      return PARAMETER_TEXT[id - SpacePointParamCount];
   return SpacePoint::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer BodyFixedPoint::GetParameterID(const std::string &str) const
{
   // Handle 3 special cases
   if (str == locationLabels[0])
      return LOCATION_1;

   if (str == locationLabels[1])
      return LOCATION_2;

   if (str == locationLabels[2])
      return LOCATION_3;

   for (Integer i = SpacePointParamCount; i < BodyFixedPointParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SpacePointParamCount])
         return i;
   }

   return SpacePoint::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType BodyFixedPoint::GetParameterType(const Integer id) const
{
   if (id >= SpacePointParamCount && id < BodyFixedPointParamCount)
      return PARAMETER_TYPE[id - SpacePointParamCount];

   return SpacePoint::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string BodyFixedPoint::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool BodyFixedPoint::IsParameterReadOnly(const Integer id) const
{
   if ((id >= SpacePointParamCount) && (id < BodyFixedPointParamCount))
      return ((id >= LOCATION_LABEL_1) && (id <= LOCATION_UNITS_3));

   return SpacePoint::IsParameterReadOnly(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool BodyFixedPoint::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//---------------------------------------------------------------------------
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
UnsignedInt BodyFixedPoint::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case CENTRAL_BODY:
      return Gmat::CELESTIAL_BODY;
   default:
      return SpacePoint::GetPropertyObjectType(id);
   }
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves eumeration symbols of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return list of enumeration symbols
 */
//---------------------------------------------------------------------------
const StringArray& BodyFixedPoint::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   switch (id)
   {
   case STATE_TYPE:
      enumStrings.clear();
      enumStrings.push_back("Cartesian");
      enumStrings.push_back("Spherical");
      return enumStrings;
   case HORIZON_REFERENCE:
      enumStrings.clear();
      enumStrings.push_back("Sphere");
      enumStrings.push_back("Ellipsoid");
      return enumStrings;
   default:
      return SpacePoint::GetPropertyEnumStrings(id);
   }
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string BodyFixedPoint::GetStringParameter(const Integer id) const
{
   if (id == CENTRAL_BODY)
   {
      if (theBody)
         return theBody->GetName();
      else
         return cBodyName;
   }

   if (id == STATE_TYPE)
      return stateType;

   if (id == HORIZON_REFERENCE)
      return horizon;

   if ((id >= LOCATION_LABEL_1) && (id <= LOCATION_LABEL_3))
      return locationLabels[id-LOCATION_LABEL_1];

   if ((id >= LOCATION_UNITS_1) && (id <= LOCATION_UNITS_3))
      return locationUnits[id-LOCATION_UNITS_1];

   return SpacePoint::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
bool BodyFixedPoint::SetStringParameter(const Integer id,
                                        const std::string &value)
{
   if (IsParameterReadOnly(id))
       return false;

   static bool firstTimeWarning = true;
   bool        retval           = false;
   std::string stateTypeList    = "Cartesian, Spherical";
   std::string horizonList      = "Sphere, Ellipsoid";
   std::string currentStateType = stateType;
   std::string currentHorizon   = horizon;

   if (id == CENTRAL_BODY)
   {
//      if (value != SolarSystem::EARTH_NAME)
//      {
//         std::string errmsg =
//            "The value of \"" + value + "\" for field \"CentralBody\""
//            " on object \"" + instanceName + "\" is not an allowed value.\n"
//            "The allowed values are: [ " + SolarSystem::EARTH_NAME + " ]. ";
//         throw AssetException(errmsg);
//      }
      if (theBody)
         theBody = NULL;
      cBodyName = value;
      retval = true;
   }
   else if (id == STATE_TYPE)
   {
      std::string v = value;
      if (v == "Geographical") // deprecated value
      {
        v = "Spherical";
        // write one warning per GMAT session
        if (firstTimeWarning)
        {
           std::string msg =
              "The value of \"" + value + "\" for field \"StateType\""
              " on object \"" + instanceName + "\" is not an allowed value.\n"
              "The allowed values are: [ " + stateTypeList + " ]. ";
           MessageInterface::ShowMessage("*** WARNING *** " + msg + "\n");
           firstTimeWarning = false;
        }
      }

      if ((v == "Cartesian") || (v == "Spherical"))
      {
         stateType = v;
         if (v == "Cartesian")
         {
            locationLabels[0] = "X";
            locationLabels[1] = "Y";
            locationLabels[2] = "Z";
            locationUnits[0] = "km";
            locationUnits[1] = "km";
            locationUnits[2] = "km";
         }
         else
         {
            locationLabels[0] = "Latitude";
            locationLabels[1] = "Longitude";
            locationLabels[2] = "Altitude";
            locationUnits[0] = "deg";
            locationUnits[1] = "deg";
            locationUnits[2] = "km";
         }
         if (currentStateType != stateType)
         {
            Rvector3 locIn(location[0], location[1], location[2]);
            Rvector3 locOut = BodyFixedStateConverterUtil::Convert(locIn,
                              currentStateType, horizon, stateType, horizon,
                              flattening, meanEquatorialRadius);
            location[0] = locOut[0];
            location[1] = locOut[1];
            location[2] = locOut[2];
         }
         retval = true;
      }
      else
      {
         std::string errmsg =
            "The value of \"" + value + "\" for field \"StateType\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [ " + stateTypeList + " ]. ";
         throw AssetException(errmsg);
      }
   }
   else if (id == HORIZON_REFERENCE)
   {
      if ((value == "Sphere") || (value == "Ellipsoid"))
      {
         horizon = value;
         if (currentHorizon != horizon)
         {
            Rvector3 locIn(location[0], location[1], location[2]);
            Rvector3 locOut = BodyFixedStateConverterUtil::Convert(locIn,
                              stateType, currentHorizon, stateType, horizon,
                              flattening, meanEquatorialRadius);
            location[0] = locOut[0];
            location[1] = locOut[1];
            location[2] = locOut[2];
         }
         retval = true;
      }
      else
      {
         std::string errmsg =
            "The value of \"" + value + "\" for field \"HorizonReference\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [ " + horizonList + " ]. ";
         throw AssetException(errmsg);
      }
   }
   else
      retval = SpacePoint::SetStringParameter(id, value);

   return retval;
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param   label  label ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string BodyFixedPoint::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    label Integer ID for the parameter
 * @param    value The new value for the parameter
 */
//------------------------------------------------------------------------------
bool BodyFixedPoint::SetStringParameter(const std::string &label,
                                           const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the BodyFixedPoint class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 */
//------------------------------------------------------------------------------
GmatBase* BodyFixedPoint::GetRefObject(const UnsignedInt type,
                                       const std::string &name)
{
   #ifdef TEST_BODYFIXED_POINT
      MessageInterface::ShowMessage("Entering BodyFixedPoint::GetRefObject()");
      MessageInterface::ShowMessage("name = %s, cBodyName = %s\n",
            name.c_str(), cBodyName.c_str());
      if (!theBody)
         MessageInterface::ShowMessage("The Body is NULL, though!!!\n");
   #endif
   if ((type == Gmat::SPACE_POINT) || (type == Gmat::CELESTIAL_BODY))
      if (name == cBodyName)
         return theBody;

   // Not handled here -- invoke the next higher GetRefObject call
   return SpacePoint::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the SpacePoint class.
 *
 * @param <obj>   pointer to the reference object
 * @param <type>  type of the reference object
 * @param <name>  name of the reference object
 *
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool BodyFixedPoint::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                 const std::string &name)
{
   if (obj == NULL)
      return false;

   #ifdef DEBUG_OBJECT_MAPPING
      MessageInterface::ShowMessage
         ("BodyFixedPoint::SetRefObject() this=%s, obj=<%p><%s>, type=<%d><%s> entered\n",
          GetName().c_str(), obj, obj->GetName().c_str(), (Integer) type,
          GetObjectTypeString(type).c_str());
   #endif

   switch (type)
   {
      case Gmat::SPACE_POINT:
      case Gmat::CELESTIAL_BODY:
         if (obj->GetName() == cBodyName)
         {
            theBody = (SpacePoint*)obj;
            // Let ancestors process this object as well

            #ifdef DEBUG_OBJECT_MAPPING
               MessageInterface::ShowMessage
                  ("BodyFixedPoint::Setting theBody to %s\n",
                   theBody->GetName().c_str());
            #endif

//            SpacePoint::SetRefObject(obj, type, name);
            return true;
         }
         break;

      default:
         break;
   }

   // Not handled here -- invoke the next higher SetRefObject call
   return SpacePoint::SetRefObject(obj, type, name);
}


Real BodyFixedPoint::GetRealParameter(const Integer id) const
{
   if (id == EPOCH_PARAM)  // from SpacePoint
   {
      return lastStateTime.Get();
   }
   if ((id >= LOCATION_1) && (id <= LOCATION_3))
   {
      if ((stateType == "Cartesian") || (id == LOCATION_3))  // all units are km
         return location[id - LOCATION_1];
      // need to return units of degrees for Spherical state latitude and longitude
      else
         return location[id - LOCATION_1] * GmatMathConstants::DEG_PER_RAD;
   }

   return SpacePoint::GetRealParameter(id);
}


Real BodyFixedPoint::SetRealParameter(const Integer id,
                                      const Real value)
{
   if (id == EPOCH_PARAM)  // from SpacePoint
   {
      A1Mjd a1(value);
      GetMJ2000State(a1);
      runEpoch = GmatGlobal::Instance()->GetCurrentEpoch();
      return lastStateTime.Get();
   }
   #ifdef DEBUG_BODYFIXED_SET_REAL
      MessageInterface::ShowMessage("Entering BFP::SetRealParameter with id = %d (%s) and value = %12.10f\n",
            id, (GetParameterText(id)).c_str(), value);
//      MessageInterface::ShowMessage("stateType = %s and horizon = %s\n",
//            stateType.c_str(), horizon.c_str());
   #endif
   if (((id == LOCATION_1) || (id == LOCATION_2)) && stateType == "Spherical")
   {
      // if Spherical statetype, then check if Latitude/Longitude are in the correct range
      if (id == LOCATION_1) // latitude
      {
         if ((value >= -90.0) && (value <= 90))
            location[id-LOCATION_1] = value * GmatMathConstants::RAD_PER_DEG;
         else
         {
            AssetException aException("");
            aException.SetDetails(errorMessageFormat.c_str(),
                        GmatStringUtil::ToString(value, 16).c_str(),
                        GetStringParameter(id-LOCATION_1+LOCATION_LABEL_1).c_str(), "Real Number >= -90.0 and <= 90.0");
            throw aException;
         }
      }
      else // longitude (0-360)
         location[id-LOCATION_1] = (GmatMathUtil::Mod(value,360)) * GmatMathConstants::RAD_PER_DEG;

      // BodyFixedPoint objects can have different internal data for the
      // lat/long setting (radians rather than degrees), so return the input
      // setting for this case.
      return value;
   }
   else if ((id >= LOCATION_1) && (id <= LOCATION_3)) // not Spherical
   {
      location[id-LOCATION_1] = value;
      return location[id-LOCATION_1];
   }

   return SpacePoint::SetRealParameter(id, value);
}


Real BodyFixedPoint::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

Real BodyFixedPoint::SetRealParameter(const std::string &label,
                                      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

// These indexed methods seem like they should NOT be needed, but GCC gets
// confused about the overloaded versions of the following six methods:

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value from a vector of strings,
 * given the input parameter ID and the index into the vector.
 *
 * @param id ID for the requested parameter.
 * @param index index for the particular string requested.
 *
 * @return The requested string.
 */
//------------------------------------------------------------------------------
std::string BodyFixedPoint::GetStringParameter(const Integer id,
                                              const Integer index) const
{
   return SpacePoint::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a value on a string parameter value in a vector of strings,
 * given the input parameter ID, the value, and the index into the vector.
 *
 * @param id ID for the requested parameter.
 * @param value The new string.
 * @param index index for the particular string requested.
 *
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool BodyFixedPoint::SetStringParameter(const Integer id,
                                       const std::string &value,
                                       const Integer index)
{
   return SpacePoint::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//                                const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value from a vector of strings,
 * given the label associated with the input parameter and the index into the
 * vector.
 *
 * @param label String identifier for the requested parameter.
 * @param index index for the particular string requested.
 *
 * @return The requested string.
 */
//------------------------------------------------------------------------------
std::string BodyFixedPoint::GetStringParameter(const std::string &label,
                                           const Integer index) const
{
   return SpacePoint::GetStringParameter(label,  index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a value on a string parameter value in a vector of strings,
 * given the label associated with the input parameter and the index into the
 * vector.
 *
 * @param label String identifier for the requested parameter.
 * @param value The new string.
 * @param index index for the particular string requested.
 *
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool BodyFixedPoint::SetStringParameter(const std::string &label,
                                       const std::string &value,
                                       const Integer index)
{
   return SpacePoint::SetStringParameter(label, value, index);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to a reference object contained in a vector of
 * objects in the BodyFixedPoint class.
 *
 * @param type type of the reference object requested
 * @param name name of the reference object requested
 * @param index index for the particular object requested.
 *
 * @return pointer to the reference object requested.
 */
//------------------------------------------------------------------------------
GmatBase* BodyFixedPoint::GetRefObject(const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index)
{
   return SpacePoint::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a pointer to a reference object in a vector of objects in
 * the BodyFixedPoint class.
 *
 * @param obj The reference object.
 * @param type type of the reference object requested
 * @param name name of the reference object requested
 * @param index index for the particular object requested.
 *
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool BodyFixedPoint::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                  const std::string &name,
                                  const Integer index)
{
   // Call parent class to add objects to bodyList
   return SpacePoint::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool BodyFixedPoint::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
std::string BodyFixedPoint::GetRefObjectName(const UnsignedInt type) const
{
   return cBodyName;
}

//------------------------------------------------------------------------------
// const StringArray&  GetRefObjectNameArray(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
const StringArray& BodyFixedPoint::GetRefObjectNameArray(const UnsignedInt type)
{
   #ifdef DEBUG_BF_REF
      MessageInterface::ShowMessage("In BFP::GetRefObjectNameArray, requesting type %d (%s)\n",
            (Integer) type, (GmatBase::OBJECT_TYPE_STRING[type]).c_str());
   #endif

   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT ||
       type == Gmat::CELESTIAL_BODY)
   {
      static StringArray refs;
      refs.clear();
      refs.push_back(cBodyName);
      return refs;
   }
   
   // Not handled here -- invoke the next higher GetRefObjectNameArray call
   return SpacePoint::GetRefObjectNameArray(type);
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 *
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& BodyFixedPoint::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
//   refObjectTypes.push_back(Gmat::CELESTIAL_BODY);
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   return refObjectTypes;
}

// Handle the J2000Body methods
//------------------------------------------------------------------------------
//  const Rvector6 GetMJ2000State(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Method returning the MJ2000 state of the SpacePoint at the time atTime.
 *
 * @param <atTime> Time for which the state is requested.
 *
 * @return state of the SpacePoint at time atTime.
 *
 * @note This method is pure virtual and must be implemented by the
 *       'leaf' (non-abstract derived) classes.
 */
//------------------------------------------------------------------------------
const Rvector6 BodyFixedPoint::GetMJ2000State(const A1Mjd &atTime)
{
   #ifdef DEBUG_BODYFIXED_STATE
      MessageInterface::ShowMessage("In GetMJ2000State for BodyFixedPoint %s\n",
            instanceName.c_str());
   #endif

   UpdateBodyFixedLocation();
   Real     epoch = atTime.Get();
   Rvector6 bfState;

   // For now I'm ignoring velocity; this assumes bfLocation is kept up-to-date
   bfState.Set(bfLocation[0], bfLocation[1], bfLocation[2], 0.0, 0.0, 0.0);

   // Convert from the body-fixed location to a J2000 location,
   // assuming you have pointer to coordinate systems mj2k and bfcs,
   // where mj2k is a J2000 system and bfcs is BodyFixed
   #ifdef DEBUG_BODYFIXED_STATE
      MessageInterface::ShowMessage("... before call to Convert, epoch = %12.10f\n",
            epoch);
      MessageInterface::ShowMessage(" ... bfcs (%s) = %s  and mj2kcs (%s) = %s\n",
            bfcsName.c_str(), (bfcs? "NOT NULL" : "NULL"),
            mj2kcsName.c_str(), (mj2kcs? "NOT NULL" : "NULL"));
      MessageInterface::ShowMessage("bf state (in bfcs, cartesian) = %s\n",
            (bfState.ToString()).c_str());
      MessageInterface::ShowMessage("SolarSystem is%s NULL\n", (solarSystem? " NOT " : ""));
      MessageInterface::ShowMessage("before, J2000PosVel = %s\n",
            (j2000PosVel.ToString()).c_str());
   #endif
   ccvtr.Convert(epoch, bfState, bfcs, j2000PosVel, mj2kcs);
   #ifdef DEBUG_BODYFIXED_STATE
      MessageInterface::ShowMessage("bf state (in mj2kcs, cartesian) = %s\n",
            (j2000PosVel.ToString()).c_str());
   #endif

   lastStateTime = atTime;
   lastStateTimeGT = atTime.GetReal();
   lastState     = j2000PosVel;
   return j2000PosVel;
}


const Rvector6 BodyFixedPoint::GetMJ2000State(const GmatTime &atTime)
{
#ifdef DEBUG_BODYFIXED_STATE
   MessageInterface::ShowMessage("In GetMJ2000State for BodyFixedPoint %s\n",
      instanceName.c_str());
#endif

   UpdateBodyFixedLocation();
   GmatTime epoch = atTime;
   Rvector6 bfState;

   // For now I'm ignoring velocity; this assumes bfLocation is kept up-to-date
   bfState.Set(bfLocation[0], bfLocation[1], bfLocation[2], 0.0, 0.0, 0.0);

   // Convert from the body-fixed location to a J2000 location,
   // assuming you have pointer to coordinate systems mj2k and bfcs,
   // where mj2k is a J2000 system and bfcs is BodyFixed
#ifdef DEBUG_BODYFIXED_STATE
   MessageInterface::ShowMessage("... before call to Convert, epoch = %12.10f\n",
      epoch.GetMjd());
   MessageInterface::ShowMessage(" ... bfcs (%s) = %s  and mj2kcs (%s) = %s\n",
      bfcsName.c_str(), (bfcs ? "NOT NULL" : "NULL"),
      mj2kcsName.c_str(), (mj2kcs ? "NOT NULL" : "NULL"));
   MessageInterface::ShowMessage("bf state (in bfcs, cartesian) = %s\n",
      (bfState.ToString()).c_str());
   MessageInterface::ShowMessage("SolarSystem is%s NULL\n", (solarSystem ? " NOT " : ""));
   MessageInterface::ShowMessage("before, J2000PosVel = %s\n",
      (j2000PosVel.ToString()).c_str());
#endif
   ccvtr.Convert(epoch, bfState, bfcs, j2000PosVel, mj2kcs);
#ifdef DEBUG_BODYFIXED_STATE
   MessageInterface::ShowMessage("bf state (in mj2kcs, cartesian) = %s\n",
      (j2000PosVel.ToString()).c_str());
#endif

   lastStateTimeGT = atTime;
   lastStateTime   = GmatTime(atTime).GetMjd();
   lastState = j2000PosVel;
   return j2000PosVel;
}


//------------------------------------------------------------------------------
//  const Rvector3 GetMJ2000Position(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Method returning the MJ2000 position of the SpacePoint at the time atTime.
 *
 * @param <atTime> Time for which the position is requested.
 *
 * @return position of the SpacePoint at time atTime.
 *
 * @note This method is pure virtual and must be implemented by the
 *       'leaf' (non-abstract derived) classes.
 */
//------------------------------------------------------------------------------
const Rvector3 BodyFixedPoint::GetMJ2000Position(const A1Mjd &atTime)
{
   Rvector6 rv = GetMJ2000State(atTime);
   j2000Pos = rv.GetR();
   return j2000Pos;
}


const Rvector3 BodyFixedPoint::GetMJ2000Position(const GmatTime &atTime)
{
   Rvector6 rv = GetMJ2000State(atTime);
   j2000Pos = rv.GetR();
   return j2000Pos;
}


//------------------------------------------------------------------------------
//  const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Method returning the MJ2000 velocity of the SpacePoint at the time atTime.
 *
 * @param <atTime> Time for which the velocity is requested.
 *
 * @return velocity of the SpacePoint at time atTime.
 *
 * @note This method is pure virtual and must be implemented by the
 *       'leaf' (non-abstract derived) classes.
 */
//------------------------------------------------------------------------------
const Rvector3 BodyFixedPoint::GetMJ2000Velocity(const A1Mjd &atTime)
{
   Rvector6 rv = GetMJ2000State(atTime);
   j2000Vel = rv.GetV();
   return j2000Vel;
}


const Rvector3 BodyFixedPoint::GetMJ2000Velocity(const GmatTime &atTime)
{
   Rvector6 rv = GetMJ2000State(atTime);
   j2000Vel = rv.GetV();
   return j2000Vel;
}


//------------------------------------------------------------------------------
//  bool GetBodyFixedLocation(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Method returning the BodyFixed location of the BodyFixedPoint
 * at the time atTime.
 *
 * @param <atTime> Time for which the location is requested.
 *
 * @return location of the BodyFixedPoint at time atTime.
 *
 * @note This method may be moved to an intermediate BodyFixedPoint
 * class, if/when appropriate.
 * @note time is ignored as the body-fixed-point is assumed not to move
 */
//------------------------------------------------------------------------------
const Rvector3 BodyFixedPoint::GetBodyFixedLocation(const A1Mjd &atTime)
{
   UpdateBodyFixedLocation();

   Rvector3 locBodyFixed;
   locBodyFixed[0] = bfLocation[0];
   locBodyFixed[1] = bfLocation[1];
   locBodyFixed[2] = bfLocation[2];

   return locBodyFixed;
}


//------------------------------------------------------------------------------
//  bool GetSphericalLocation(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
* Method returning the latitude, longitude, and height of the BodyFixedPoint
* at the time atTime.
*
* @param <atTime> Time for which the location is requested.
*
* @return location of the BodyFixedPoint at time atTime.
*
* @note This method may be moved to an intermediate BodyFixedPoint
* class, if/when appropriate.
* @note time is ignored as the body-fixed-point is assumed not to move
*/
//------------------------------------------------------------------------------
const Rvector3 BodyFixedPoint::GetSphericalLocation(const A1Mjd &atTime)
{
   Rvector3 latLongHeight;

   if (stateType == "Cartesian")
   {
      Rvector3 bfLocation(location[0], location[1], location[2]);

      latLongHeight = BodyFixedStateConverterUtil::CartesianToSphericalEllipsoid(
         bfLocation, flattening, meanEquatorialRadius);
   }
   else if (stateType == "Spherical")
   {
      latLongHeight[0] = location[0];
      latLongHeight[1] = location[1];
      latLongHeight[2] = location[2];
   }
   else
   {
      throw AssetException("Unable to compute spherical location for BodyFixedPoint \"" +
         instanceName + "\"; state type is not a recognized type (known "
         "types are either \"Cartesian\" or \"Spherical\")");
   }

   return latLongHeight;
}


//------------------------------------------------------------------------------
//  CoordinateSystem* GetBodyFixedCoordinateSystem() const
//------------------------------------------------------------------------------
/**
 * Method returning the BodyFixed coordinate system used by this BodyFixedPoint.
 *
  * @return the BodyFixed coordinate system.
 *
 * @note This method may be moved to an intermediate BodyFixedPoint
 * class, if/when appropriate.
 */
//------------------------------------------------------------------------------
CoordinateSystem* BodyFixedPoint::GetBodyFixedCoordinateSystem() const
{
   return bfcs;
}


//------------------------------------------------------------------------------
//  void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
void BodyFixedPoint::SetSolarSystem(SolarSystem *ss)
{
   SpacePoint::SetSolarSystem(ss);
}

//------------------------------------------------------------------------------
//  bool IsValidId(const std::string & id)
//------------------------------------------------------------------------------
/**
 */
 //------------------------------------------------------------------------------
bool BodyFixedPoint::IsValidID(const std::string &id)
{
   return true;
}

//------------------------------------------------------------------------------
//  bool InitializeForContactLocation(bool deleteFiles = true)
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
bool BodyFixedPoint::InitializeForContactLocation(bool deleteFiles)
{
   #ifdef DEBUG_BFP_SPICE
      MessageInterface::ShowMessage("Entering InitializeForC for %s\n",
            instanceName.c_str());
   #endif

   if (kernelsWritten) // if we've already written them, don't do it again
      return true;

   if (!kernelNamesDetermined)
   {
      // Use upper case name to figure out spice frame name
      std::string thisName = GmatStringUtil::ToUpper(instanceName);
      if (thisName.size() > 20)
      {
         Integer hash = 0;
         for (Integer i = 0; i < thisName.length(); i++)
         {
            hash += thisName[i];
         }

         thisName = GmatStringUtil::ToString(hash) + GmatStringUtil::ToString(naifId).substr(GmatStringUtil::ToString(naifId).length() - 3, 3) + thisName.substr(0, 15 -GmatStringUtil::ToString(hash).length());
      }
      spiceFrameID         = thisName + "_TOPO";

      // Set up the file names for the SPK and FK kernels
      std::stringstream ss("");
      ss << "tmp_" << instanceName;
      std::string tmpPath = GmatFileUtil::GetTemporaryDirectory();
      kernelBaseName = tmpPath + ss.str();
      #ifdef DEBUG_BFP_SPICE
         MessageInterface::ShowMessage(
               "In InitializeForC,  fileName (full path) = %s\n",
               kernelBaseName.c_str());
      #endif

      kernelNamesDetermined = true;
   }

   #ifdef __USE_SPICE__
      if (!spice) spice = new SpiceInterface();
   #endif

   #ifdef DEBUG_BFP_SPICE
      MessageInterface::ShowMessage("   About to write the SPK and FK kernels\n");
   #endif
   if (!WriteSPK(deleteFiles) || (!WriteFK(deleteFiles)))
      return false;
   #ifdef DEBUG_BFP_SPICE
      MessageInterface::ShowMessage("   Kernels written successfully for %s\n",
            instanceName.c_str());
   #endif
   kernelsWritten = true;

   return true;
}

//------------------------------------------------------------------------------
//  GmatBase * Clone()
//------------------------------------------------------------------------------
/**
 *
 */
 //------------------------------------------------------------------------------
GmatBase * BodyFixedPoint::Clone() const
{
   return (new BodyFixedPoint(*this));
}

 SpacePoint * BodyFixedPoint::GetCentralBody()
 {
    return theBody;
 }

//------------------------------------------------------------------------------
//  void UpdateBodyFixedLocation()
//------------------------------------------------------------------------------
/**
 * This method makes sure that the bfLocation is up-to-date (as new location
 * data may have been input)
 *
 */
//------------------------------------------------------------------------------
void BodyFixedPoint::UpdateBodyFixedLocation()
{
   if (stateType == "Cartesian")
   {
      bfLocation[0] = location[0];
      bfLocation[1] = location[1];
      bfLocation[2] = location[2];
   }
   // Otherwise, convert from input type to Cartesian
   else if (stateType == "Spherical")
   {
      Rvector3 spherical(location[0], location[1], location[2]);
      Rvector3 cart;
      if (horizon == "Sphere")
      {
         cart = BodyFixedStateConverterUtil::SphericalToCartesian(spherical,
                flattening, meanEquatorialRadius);
         bfLocation[0] = cart[0];
         bfLocation[1] = cart[1];
         bfLocation[2] = cart[2];
      }
      else if (horizon == "Ellipsoid")
      {
         cart = BodyFixedStateConverterUtil::SphericalEllipsoidToCartesian(spherical,
                flattening, meanEquatorialRadius);
         bfLocation[0] = cart[0];
         bfLocation[1] = cart[1];
         bfLocation[2] = cart[2];
      }
      else
         throw AssetException("Unable to set body fixed location for BodyFixedPoint \"" +
               instanceName + "\"; horizon reference is not a recognized type (known "
                     "types are either \"Sphere\" or \"Ellipsoid\")");
   }
   else
   {
      throw AssetException("Unable to set body fixed location for BodyFixedPoint \"" +
            instanceName + "\"; state type is not a recognized type (known "
                  "types are either \"Cartesian\" or \"Spherical\")");
   }

}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool WriteSPK(bool deleteFile)
//------------------------------------------------------------------------------

bool BodyFixedPoint::WriteSPK(bool deleteFile)
{
   deleteSPK = deleteFile; // false is temporary - for testing only!!

   // Get the time (seconds since January 1, 1970), to make the temporary file name unique
   std::string now = GmatTimeUtil::FormatCurrentTime(4);
   spkName         = kernelBaseName + "_" + now + ".bsp";

   #ifdef DEBUG_BFP_SPICE
      MessageInterface::ShowMessage("In WriteSPK, spkName = \"%s\"\n", spkName.c_str());
   #endif

    // We are not renaming here - just remove the existing file
   if (GmatFileUtil::DoesFileExist(spkName))
      remove(spkName.c_str());

   #ifdef __USE_SPICE__
   if (!spice)
      spice = new SpiceInterface();

      SpiceInt        maxChar = 4000; // need static const for this?
      std::string thisName = GmatStringUtil::ToUpper(instanceName);
      if (thisName.size() > 20)
      {
         Integer hash = 0;
         for (Integer i = 0; i < thisName.length(); i++)
         {
            hash += thisName[i];
         }

         thisName = GmatStringUtil::ToString(hash) + GmatStringUtil::ToString(naifId).substr(GmatStringUtil::ToString(naifId).length()-3,3) + thisName.substr(0, 15 - GmatStringUtil::ToString(hash).length());
      }
      std::string     internalFileName  = "GMAT-generated SPK file for " + thisName;
      ConstSpiceChar  *internalSPKName  = internalFileName.c_str();
      ConstSpiceChar  *spkNameSPICE     = spkName.c_str();
      SpiceInt        handle;

      #ifdef DEBUG_BFP_SPICE
         MessageInterface::ShowMessage("In WriteSPK, about to open SPK ...\n");
      #endif
      spkopn_c(spkNameSPICE, internalSPKName, maxChar, &handle); // CSPICE method to create and open an SPK kernel
      if (failed_c()) // CSPICE method to detect failure of previous call to CSPICE
      {
         ConstSpiceChar option[]   = "LONG"; // retrieve long error message
         SpiceInt       numErrChar = MAX_LONG_MESSAGE_VALUE;
         //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
         SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
         getmsg_c(option, numErrChar, err);
         std::string errStr(err);
         std::string errmsg = "Error getting file handle for GroundStation SPK file \"";
         errmsg += spkName + "\".  Message received from CSPICE is: ";
         errmsg += errStr + "\n";
         reset_c();
         delete [] err;
         throw AssetException(errmsg);
      }
      // Write the data to the GroundStation SPK
      SpiceInt       spiceId      = naifId;
      Integer        bodyNaif     = theBody->GetIntegerParameter("NAIFId");
      SpiceInt       spiceCentral = bodyNaif;
      std::string    bodyFrame    = theBody->GetStringParameter("SpiceFrameId");
      ConstSpiceChar *bFrame      = bodyFrame.c_str();

      SpiceDouble    theMax       = GmatRealConstants::REAL_MAX - 10.0;
      SpiceDouble    first        = -theMax/2.0;
      SpiceDouble    last         =  theMax/2.0;
      SpiceDouble    epoch1       = first;
      SpiceDouble    step         = last - first;
      std::string    segmentId    = "Segment 1 for Asset " + thisName;
      ConstSpiceChar *segId       = segmentId.c_str();
      // put state into a SpiceDouble array
      UpdateBodyFixedLocation();
      SpiceDouble  *stateArray;
      stateArray = new SpiceDouble[6*2];
      stateArray[0]  = bfLocation[0];
      stateArray[1]  = bfLocation[1];
      stateArray[2]  = bfLocation[2];
      stateArray[3]  = 0.0;
      stateArray[4]  = 0.0;
      stateArray[5]  = 0.0;
      stateArray[6]  = bfLocation[0];
      stateArray[7]  = bfLocation[1];
      stateArray[8]  = bfLocation[2];
      stateArray[9]  = 0.0;
      stateArray[10] = 0.0;
      stateArray[11] = 0.0;

      #ifdef DEBUG_BFP_SPICE
         MessageInterface::ShowMessage("In WriteSPK, about to write SPK ...\n");
         MessageInterface::ShowMessage("spiceId = %d, spiceCentral = %d\n",
                                       naifId, bodyNaif);
      #endif
         if (bodyFrame == "")
         {
            throw AssetException("The parameter " + theBody->GetName() + ".SpiceFrameId must be defined in order to create a SPICE FrameKernel for Contact Location.");
         }
      spkw08_c(handle, spiceId, spiceCentral, bFrame, first, last, segId, 1, 2,
            stateArray, epoch1, step);

      spkcls_c(handle);

      spice->LoadKernel(spkName);

      delete[] stateArray;
   #endif  // USE_SPICE

   return true;
}

//------------------------------------------------------------------------------
// bool WriteFK(bool deleteFile)
//------------------------------------------------------------------------------
bool BodyFixedPoint::WriteFK(bool deleteFile)
{
   deleteFK = deleteFile; // false is temporary - for testing only!!

   // Get the time (seconds since January 1, 1970), to make the temporary file name unique
   std::string now = GmatTimeUtil::FormatCurrentTime(4);
   fkName          = kernelBaseName + "_" + now + ".tf";

   // We are not renaming here - just remove the existing file
   if (GmatFileUtil::DoesFileExist(fkName))
      remove(fkName.c_str());

   #ifdef __USE_SPICE__

      if (!spice)
         spice = new SpiceInterface();

      // Upper case name
      std::string thisName = GmatStringUtil::ToUpper(instanceName);
      // Get the topocentric conversion
      Integer        bodyNaif         = theBody->GetIntegerParameter("NAIFId");
      std::string    centralNaifStr   = GmatStringUtil::Trim(GmatStringUtil::ToString(bodyNaif));
      std::string    centralBodyFrame = theBody->GetStringParameter("SpiceFrameId");

      #ifdef DEBUG_FK_SPICE
         MessageInterface::ShowMessage("In WriteFK, about to write FK ...\n");
         MessageInterface::ShowMessage("bodyNaif = %d, centralNaifStr = %s, centralBodyFrame = %s\n",
                                       bodyNaif, centralNaifStr.c_str(), centralBodyFrame.c_str());
      #endif
      Rvector topo = GetTopocentricConversion(centralNaifStr);
      /// Write the text FK kernel
      std::ofstream fkStream(fkName.c_str(), std::ios::out);
      fkStream.precision(16);
      fkStream << "KPL/FK\n";
      fkStream << "\\begindata\n";
      fkStream << "NAIF_BODY_NAME += '" << thisName << "'\n";
      fkStream << "NAIF_BODY_CODE += " << naifId       << "\n\n";
      fkStream << "FRAME_" << spiceFrameID   << " = " << naifIdRefFrame << "\n";
      fkStream << "FRAME_" << naifIdRefFrame << "_NAME = '" << spiceFrameID << "'\n";
      fkStream << "FRAME_" << naifIdRefFrame << "_CLASS = 4\n";
      fkStream << "FRAME_" << naifIdRefFrame << "_CLASS_ID = " << naifIdRefFrame << "\n";
      fkStream << "FRAME_" << naifIdRefFrame << "_CENTER = " << naifId << "\n\n";
      fkStream << "OBJECT_" << naifId << "_FRAME = '" << spiceFrameID << "'\n\n";
//      fkStream << "TKFRAME_" << spiceFrameID << "_RELATIVE = '" << centralBodyFrame << "'\n";
//      fkStream << "TKFRAME_" << spiceFrameID << "_SPEC = 'ANGLES'\n";
//      fkStream << "TKFRAME_" << spiceFrameID << "_UNITS = 'DEGREES'\n";
//      fkStream << "TKFRAME_" << spiceFrameID << "_AXES = " << "( 3, 2, 3 )\n";
//      fkStream << "TKFRAME_" << spiceFrameID << "_ANGLES = ( " << topo[0]
//               << ", " << topo[1] << ", " << topo[2] << " )\n";
      fkStream << "TKFRAME_" << naifIdRefFrame << "_RELATIVE = '" << centralBodyFrame << "'\n";
      fkStream << "TKFRAME_" << naifIdRefFrame << "_SPEC = 'ANGLES'\n";
      fkStream << "TKFRAME_" << naifIdRefFrame << "_UNITS = 'DEGREES'\n";
      fkStream << "TKFRAME_" << naifIdRefFrame << "_AXES = " << "( 3, 2, 3 )\n";
      fkStream << "TKFRAME_" << naifIdRefFrame << "_ANGLES = ( " << topo[0]
               << ", " << topo[1] << ", " << topo[2] << " )\n";

      fkStream.close();

      spice->LoadKernel(fkName);
   #endif  // USE_SPICE

   return true;
}

//------------------------------------------------------------------------------
// Rvector3 GetTopocentricConversion(const std::string &centralNaifId)
//------------------------------------------------------------------------------
Rvector3 BodyFixedPoint::GetTopocentricConversion(
                         const std::string &centralNaifId)
{
   // x_F, y_F, z_F : position w.r.t. the body-fixed frame (e.g. ECEF)
   // R : equatorial radius of the referenced body
   // f : flattening of the referenced body
   // angles: (3,2,3) Euler angles from the body-fixed frame to
   //          topocentric (NWU) frame

   Rvector3 result;

    // body-fixed coordinates
   Real x_F = bfLocation[0];
   Real y_F = bfLocation[1];
   Real z_F = bfLocation[2];

   #ifdef __USE_SPICE__

      // Get body properties
      std::string bodyID = "BODY" + centralNaifId + "_RADII";
      ConstSpiceChar *bID = bodyID.c_str();
      SpiceInt     n;
      SpiceDouble  radii[3];
      SpiceBoolean found;
      // Ask SPICE for the body Radii
      gdpool_c(bID, 0, 3, &n, radii, &found);
      #ifdef DEBUG_BFP_SPICE
         MessageInterface::ShowMessage(
               "In GetTopocentricConversion, xyz (%s) = %12.10f   %12.10f   %12.10f\n",
               instanceName.c_str(), x_F, y_F, z_F);
         MessageInterface::ShowMessage(
               "In GetTopocentricConversion, radii (%s) = %12.10f   %12.10f   %12.10f\n",
               bodyID.c_str(), radii[0], radii[1], radii[2]);
      #endif

      Real R = radii[0];
      Real f = (radii[0] - radii[2]) / radii[0];

      // conversion
      Real Lambda = GmatMathUtil::ATan(y_F, x_F);
      Real r_xy   = GmatMathUtil::Sqrt((x_F*x_F) + (y_F*y_F));
      Real phi_gd = GmatMathUtil::ATan(z_F, r_xy); // initial guess
      Real e      = GmatMathUtil::Sqrt(2*f-(f*f));

      Real delta = 1.0;
      Real phi, C, sinPhi;
      while (delta > 1.0e-11)
      {
          phi    = phi_gd;
          sinPhi = GmatMathUtil::Sin(phi);
          C      = R/GmatMathUtil::Sqrt(1-(e*e)*(sinPhi * sinPhi));
          phi_gd = GmatMathUtil::ATan(z_F+C*(e*e)*sinPhi, r_xy);
          delta  = GmatMathUtil::Abs(phi_gd-phi);
      }

//      angles = [-Lambda, -(pi()/2 - phi_gd), pi()]*180/pi();
      result[0] = -Lambda * GmatMathConstants::DEG_PER_RAD;
      result[1] = -(GmatMathConstants::PI_OVER_TWO - phi_gd) * GmatMathConstants::DEG_PER_RAD;
      result[2] = 180.0;  // pi * deg-per-rad

   #endif // __USE_SPICE__
   return result;
}

