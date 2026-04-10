//$Id$
//------------------------------------------------------------------------------
//                                  SpacePoint
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy C. Shoan
// Created: 2004/12/07
//
/**
 * Implementation of the SpacePoint class.  This is the base class for SpaceObject
 * (base class for Spacecraft and Formation), CelestialBody (base class for Star,
 * Planet, and Moon), and CalculatedPoint (base class for LibrationPoint and
 * Barycenter)  - any object that can be used as an origin, primary, or
 * secondary when defining a CoordinateSystem.
 *
 */
//------------------------------------------------------------------------------

#include <iomanip>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"
#include "StringUtil.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"
#include "CoordinateConverter.hpp"      // for Convert()
#include "GmatDefaults.hpp"             // for GmatSolarSystemDefaults::
#include "RgbColor.hpp"
#include "ColorTypes.hpp"               // for namespace GmatColor::
#include "ColorDatabase.hpp"
#include "MessageInterface.hpp"
#include "GmatBaseException.hpp"


//#define DEBUG_J2000_STATE
//#define DEBUG_SP_CLOAKING
//#define DEBUG_SPICE_KERNEL
//#define DEBUG_SPACE_POINT_ORBIT_KERNELS
//#define DEBUG_ATTITUDE
//#define DEBUG_KERNEL_VALIDATE
//#define DEBUG_SET_STRING
//#define DEBUG_COLOR

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

const std::string
SpacePoint::PARAMETER_TEXT[SpacePointParamCount - GmatBaseParamCount] =
{
   "J2000BodyName",
   "NAIFId",
   "NAIFIdReferenceFrame",
   "SpiceFrameId",
   "OrbitSpiceKernelName",
   "AttitudeSpiceKernelName",
   "SCClockSpiceKernelName",
   "FrameSpiceKernelName",
   "A1Epoch",
   "OrbitColor",
   "TargetColor",
};

const Gmat::ParameterType
SpacePoint::PARAMETER_TYPE[SpacePointParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,       // "J2000BodyName"
   Gmat::INTEGER_TYPE,      // "NAIFId"
   Gmat::INTEGER_TYPE,      // "NAIFIdReferenceFrame"
   Gmat::STRING_TYPE,       // "SpiceFrameId"
   Gmat::STRINGARRAY_TYPE,  // "OrbitSpiceKernelName"
   Gmat::STRINGARRAY_TYPE,  // "AttitudeSpiceKernelName"
   Gmat::STRINGARRAY_TYPE,  // "SCClockSpiceKernelName"
   Gmat::STRINGARRAY_TYPE,  // "FrameSpiceKernelName"
   Gmat::REAL_TYPE,         // "A1Epoch"
   Gmat::COLOR_TYPE,        // "OrbitColor"
   Gmat::COLOR_TYPE,        // "TargetColor"
};

#ifdef __USE_SPICE__
   const Integer SpacePoint::UNDEFINED_NAIF_ID           = SpiceInterface::DEFAULT_NAIF_ID;
   const Integer SpacePoint::UNDEFINED_NAIF_ID_REF_FRAME = SpiceInterface::DEFAULT_NAIF_ID_REF_FRAME;
#else
   const Integer SpacePoint::UNDEFINED_NAIF_ID           = -123456789;
   const Integer SpacePoint::UNDEFINED_NAIF_ID_REF_FRAME = -123456789;
#endif

/// initialize the Spacecraft instances counter
Integer SpacePoint::spacecraftInstanceCount = 0;

// For automatic default colors
const UnsignedInt
SpacePoint::DEFAULT_ORBIT_COLOR[MAX_SP_COLOR] =
{
   GmatColor::RED,       GmatColor::GREEN,        GmatColor::YELLOW,     GmatColor::BLUE,
   GmatColor::PINK,      GmatColor::LIME,         GmatColor::GOLD,       GmatColor::NAVY,
   GmatColor::PURPLE,    GmatColor::OLIVE,        GmatColor::BEIGE,      GmatColor::CYAN,
   GmatColor::FUCHSIA,   GmatColor::SEA_GREEN,    GmatColor::PERU,       GmatColor::SLATE_BLUE,  
   GmatColor::ORCHID,    GmatColor::SPRING_GREEN, GmatColor::TAN,        GmatColor::TURQUOISE,
};
const UnsignedInt
SpacePoint::DEFAULT_TARGET_COLOR[MAX_SP_COLOR] =
{
   GmatColor::TEAL,             GmatColor::LIGHT_GRAY,   GmatColor::DARK_GRAY,  GmatColor::DIM_GRAY,
   GmatColor::DARK_SLATE_GRAY,  GmatColor::LIGHT_GRAY,   GmatColor::DARK_GRAY,  GmatColor::DIM_GRAY,
   GmatColor::DARK_SLATE_GRAY,  GmatColor::LIGHT_GRAY,   GmatColor::DARK_GRAY,  GmatColor::DIM_GRAY,
   GmatColor::DARK_SLATE_GRAY,  GmatColor::LIGHT_GRAY,   GmatColor::DARK_GRAY,  GmatColor::DIM_GRAY,
   GmatColor::DARK_SLATE_GRAY,  GmatColor::LIGHT_GRAY,   GmatColor::DARK_GRAY,  GmatColor::DIM_GRAY,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  SpacePoint(UnsignedInt ofType, const std::string &itsType,
//             const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base SpacePoint structures used in derived classes
 * (default constructor).
 *
 * @param <ofType>  UnsignedInts enumeration for the object.
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for SpacePoint.  Derived 
 *       classes must pass in the typeId and typeStr parameters.
 */
//---------------------------------------------------------------------------
SpacePoint::SpacePoint(UnsignedInt ofType, const std::string &itsType,
                       const std::string &itsName) :
GmatBase(ofType,itsType,itsName),
theSolarSystem     (NULL),
inertialCS         (NULL),
bodyFixedCS        (NULL),
spiceCS            (NULL),
j2000Body          (NULL),
j2000BodyName      ("Earth"),
naifId             (UNDEFINED_NAIF_ID),
naifIdRefFrame     (UNDEFINED_NAIF_ID),
spiceFrameID       (""),
naifIdObserver     (UNDEFINED_NAIF_ID),
spiceSetupDone     (false),
hasAttitude        (false),
useOrbitColorName  (true),
useTargetColorName (true),
defaultOrbitColor  (GmatColor::WHITE),
defaultTargetColor (GmatColor::WHITE),
orbitColor         (0),
targetColor        (0),
orbitColorStr      (""),
targetColorStr     ("")
{
   objectTypes.push_back(Gmat::SPACE_POINT);
   objectTypeNames.push_back("SpacePoint");
   
   // Derived classes must override these colors as necessary
   SetDefaultColors(GmatColor::WHITE, GmatColor::WHITE);
   
   // Increment the spacecraft instance count and set default colors
   if (ofType == Gmat::SPACECRAFT)
   {
      ++spacecraftInstanceCount;
      SetSpacecraftDefaultColors();
   }
   
   /// derived classes must override this, as necessary, to point to
   /// the correct path
   //theSpkPath = FileManager::Instance()->GetFullPathname(FileManager::SPK_PATH);
   // Use VEHICLE_EPHEM_SPK_PATH. SPK_PATH was renamed to PLANETARY_EPHEM_SPK_PATH (LOJ: 2014.06.18)
   theSpkPath = FileManager::Instance()->GetFullPathname(FileManager::VEHICLE_EPHEM_SPK_PATH);

   theTimeConverter = TimeSystemConverter::Instance();

   #ifdef DEBUG_SPK_PATH
   MessageInterface::ShowMessage
      ("In SpacePoint() constructor, theSpkPath = '%s'\n", theSpkPath.c_str());
   #endif
   
   SaveAllAsDefault();
}

//---------------------------------------------------------------------------
//  SpacePoint(const SpacePoint &sp);
//---------------------------------------------------------------------------
/**
 * Constructs base SpacePoint structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param <sp>  SpacePoint instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
SpacePoint::SpacePoint(const SpacePoint &sp) :
GmatBase(sp),
theSolarSystem           (sp.theSolarSystem),
inertialCS               (NULL),
bodyFixedCS              (NULL),
spiceCS                  (NULL),
j2000Body                (sp.j2000Body), //(NULL),
j2000BodyName            (sp.j2000BodyName),
naifId                   (sp.naifId),
naifIdRefFrame           (sp.naifIdRefFrame),
spiceFrameID             (sp.spiceFrameID),
naifIdObserver           (sp.naifIdObserver),
default_j2000BodyName    (sp.default_j2000BodyName),
default_naifId           (sp.default_naifId),
default_naifIdRefFrame   (sp.default_naifIdRefFrame),
default_spiceFrameID     (sp.default_spiceFrameID),
spiceSetupDone           (false),
orbitSpiceKernelNames    (sp.orbitSpiceKernelNames),
attitudeSpiceKernelNames (sp.attitudeSpiceKernelNames),
scClockSpiceKernelNames  (sp.scClockSpiceKernelNames),
frameSpiceKernelNames    (sp.frameSpiceKernelNames),
theSpkPath               (sp.theSpkPath),
hasAttitude              (false),
useOrbitColorName        (sp.useOrbitColorName),
useTargetColorName       (sp.useTargetColorName),
defaultOrbitColor        (sp.defaultOrbitColor),
defaultTargetColor       (sp.defaultTargetColor),
orbitColor               (sp.orbitColor),
targetColor              (sp.targetColor),
orbitColorStr            (sp.orbitColorStr),
targetColorStr           (sp.targetColorStr)
{
   theTimeConverter = TimeSystemConverter::Instance();

   default_orbitSpiceKernelNames.clear();
   default_attitudeSpiceKernelNames.clear();
   default_frameSpiceKernelNames.clear();
   default_scClockSpiceKernelNames.clear();
   default_orbitSpiceKernelNames    = sp.default_orbitSpiceKernelNames;
   default_attitudeSpiceKernelNames = sp.default_attitudeSpiceKernelNames;
   default_frameSpiceKernelNames    = sp.default_frameSpiceKernelNames;
   default_scClockSpiceKernelNames  = sp.default_scClockSpiceKernelNames;
}

//---------------------------------------------------------------------------
//  SpacePoint& operator=(const SpacePoint &sp)
//---------------------------------------------------------------------------
/**
 * Assignment operator for SpacePoint structures.
 *
 * @param <sp> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const SpacePoint& SpacePoint::operator=(const SpacePoint &sp)
{
   if (&sp == this)
      return *this;
   theSolarSystem           = sp.theSolarSystem;
   inertialCS               = NULL;
   bodyFixedCS              = NULL;
   spiceCS                  = NULL;
   j2000Body                = sp.j2000Body;
   j2000BodyName            = sp.j2000BodyName;
   naifId                   = sp.naifId;
   naifIdRefFrame           = sp.naifIdRefFrame;
   spiceFrameID             = sp.spiceFrameID;
   spiceSetupDone           = sp.spiceSetupDone;

   orbitSpiceKernelNames.clear();
   attitudeSpiceKernelNames.clear();
   scClockSpiceKernelNames.clear();
   frameSpiceKernelNames.clear();
   orbitSpiceKernelNames    = sp.orbitSpiceKernelNames;
   attitudeSpiceKernelNames = sp.attitudeSpiceKernelNames;
   scClockSpiceKernelNames  = sp.scClockSpiceKernelNames;
   frameSpiceKernelNames    = sp.frameSpiceKernelNames;
   default_j2000BodyName    = sp.default_j2000BodyName;
   default_naifId           = sp.default_naifId;
   default_naifIdRefFrame   = sp.default_naifIdRefFrame;
   default_spiceFrameID     = sp.default_spiceFrameID;
   theSpkPath               = sp.theSpkPath;
   hasAttitude              = false;
   useOrbitColorName        = sp.useOrbitColorName;
   useTargetColorName       = sp.useTargetColorName;
   defaultOrbitColor        = sp.defaultOrbitColor;
   defaultTargetColor       = sp.defaultTargetColor;
   orbitColor               = sp.orbitColor;
   targetColor              = sp.targetColor;
   orbitColorStr            = sp.orbitColorStr;
   targetColorStr           = sp.targetColorStr;
   
   default_orbitSpiceKernelNames.clear();
   default_attitudeSpiceKernelNames.clear();
   default_frameSpiceKernelNames.clear();
   default_scClockSpiceKernelNames.clear();
   default_orbitSpiceKernelNames    = sp.default_orbitSpiceKernelNames;
   default_attitudeSpiceKernelNames = sp.default_attitudeSpiceKernelNames;
   default_frameSpiceKernelNames    = sp.default_frameSpiceKernelNames;
   default_scClockSpiceKernelNames  = sp.default_scClockSpiceKernelNames;

   return *this;
}

//---------------------------------------------------------------------------
//  ~SpacePoint(void)
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
SpacePoint::~SpacePoint()
{
   if (inertialCS)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (inertialCS, inertialCS->GetTypeName(), "SpacePoint::~SpacePoint()",
          "deleting inertialCS");
      #endif
      delete inertialCS;
   }
   
   if (bodyFixedCS)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (bodyFixedCS, bodyFixedCS->GetTypeName(), "SpacePoint::~SpacePoint()",
          "deleting bodyFixedCS");
      #endif
      delete bodyFixedCS;
   }

   if (spiceCS)
   {
#ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
      (bodyFixedCS, bodyFixedCS->GetTypeName(), "SpacePoint::~SpacePoint()",
         "deleting spiceCS");
#endif
      delete spiceCS;
   }
}

//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void SpacePoint::SetSolarSystem(SolarSystem *ss)
{
   theSolarSystem = ss;
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
Real SpacePoint::GetEpoch()
{
   std::string errmsg = "GetEpoch not implemented for SpacePoint ";
   errmsg += instanceName + "\n";
   throw GmatBaseException(errmsg);
}

GmatTime SpacePoint::GetEpochGT()
{
   std::string errmsg = "GetEpochGT not implemented for SpacePoint ";
   errmsg += instanceName + "\n";
   throw GmatBaseException(errmsg);
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
Real SpacePoint::SetEpoch(const Real ep)
{
   std::string errmsg = "SetEpoch not implemented for SpacePoint ";
   errmsg += instanceName + "\n";
   throw GmatBaseException(errmsg);
}


GmatTime SpacePoint::SetEpochGT(const GmatTime ep)
{
   std::string errmsg = "GmatTime SpacePoint::SetEpoch(const GmatTime rp) function was not implemented for SpacePoint ";
   errmsg += instanceName + "\n";
   throw GmatBaseException(errmsg);
}

Rvector6 SpacePoint::GetLastState()
{
   std::string errmsg = "GetLastState not implemented for SpacePoint ";
   errmsg += instanceName + "\n";
   throw GmatBaseException(errmsg);
}

//------------------------------------------------------------------------------
//  bool RequiresJ2000Body()
//------------------------------------------------------------------------------
/**
 * Identifies objects that need to have the J2000 body set in the Sandbox.
 *
 * @return true for all SpacePoint objects.
 */
//------------------------------------------------------------------------------
bool SpacePoint::RequiresJ2000Body()
{
   return true;
}


// methods for accessing the bodyName or body pointer
//------------------------------------------------------------------------------
//  const std::string  GetJ2000BodyName() const
//------------------------------------------------------------------------------
/**
 * This method returns the j2000 Body name.
 *
 * @return J2000 Body name.
 *
 */
//------------------------------------------------------------------------------
const std::string SpacePoint::GetJ2000BodyName() const
{
   if (j2000Body) return j2000Body->GetName();
   else           return j2000BodyName;
}

//------------------------------------------------------------------------------
//  SpacePoint* GetJ2000Body() const
//------------------------------------------------------------------------------
/**
 * This method returns the j2000 Body pointer.
 *
 * @return J2000 Body pointer.
 *
 */
//------------------------------------------------------------------------------
SpacePoint* SpacePoint::GetJ2000Body() const
{
   return j2000Body;
}

//------------------------------------------------------------------------------
//  bool  SetJ2000BodyName(const std::string &toName)
//------------------------------------------------------------------------------
/**
 * This method sets the j2000 Body name.
 *
 * @return true if successful; false, otherwise
 *
 * WARNING: The J200Body must be set identically for all objects in a GMAT run;
 * not doing so will give incorrect results.
 * In addition, the setting of a body other than Earth as the J2000Body has
 * not been tested.
 */
//------------------------------------------------------------------------------
bool SpacePoint::SetJ2000BodyName(const std::string &toName)
{
   j2000BodyName = toName;
   return true;
}

//------------------------------------------------------------------------------
//  void  SetJ2000Body(SpacePoint* toBody)
//------------------------------------------------------------------------------
/**
 * This method sets the j2000 Body.
 *
 * WARNING: The J200Body must be set identically for all objects in a GMAT run;
 * not doing so will give incorrect results.
 * In addition, the setting of a body other than Earth as the J2000Body has
 * not been tested.
 */
//------------------------------------------------------------------------------
void SpacePoint::SetJ2000Body(SpacePoint* toBody)
{
   #ifdef DEBUG_J2000_STATE
      if (toBody)
         MessageInterface::ShowMessage("Setting J2000 body to %s for %s\n",
            toBody->GetName().c_str(), instanceName.c_str());
      else
         MessageInterface::ShowMessage("Setting J2000 body to NULL for %s\n",
            instanceName.c_str());
   #endif
   
   j2000Body        = toBody;
   if (j2000Body)
      j2000BodyName = j2000Body->GetName();

   #ifdef DEBUG_J2000_STATE
      if (j2000Body)
         MessageInterface::ShowMessage("J2000 body is now set\n");
   #endif
}

//------------------------------------------------------------------------------
// bool IsParameterCloaked(const Integer id) const
//------------------------------------------------------------------------------
bool SpacePoint::IsParameterCloaked(const Integer id) const
{
   #ifdef DEBUG_SP_CLOAKING
      MessageInterface::ShowMessage
         ("SpacePoint::IsParameterCloaked() <%p>'%s' entered, id = %2d %s, cloaking = %s\n",
          this, GetName().c_str(), id, (GetParameterText(id)).c_str(), cloaking ? "true" : "false");
   #endif
   if (!cloaking)
   {
      #ifdef DEBUG_SP_CLOAKING
      MessageInterface::ShowMessage
         ("SpacePoint::IsParameterCloaked() <%p>'%s' returning false\n", this, GetName().c_str());
      #endif
      return false;
   }
   
   // if it's read-only, we'll cloak it
   if (IsParameterReadOnly(id))
   {
      #ifdef DEBUG_SP_CLOAKING
      MessageInterface::ShowMessage
         ("SpacePoint::IsParameterCloaked() <%p>'%s' returning true - parameter is read-only\n",
          this, GetName().c_str());
      #endif
      return true;
   }
   
   if (id >= GmatBaseParamCount && id < SpacePointParamCount)
   {
      bool equalToDefault = IsParameterEqualToDefault(id);
      #ifdef DEBUG_SP_CLOAKING
      MessageInterface::ShowMessage
         ("SpacePoint::IsParameterCloaked() <%p>'%s' returning %s\n", this, GetName().c_str(),
          equalToDefault ? "true" : "false");
      #endif
      return equalToDefault;
   }
   
   bool isCloaked = GmatBase::IsParameterCloaked(id);
   #ifdef DEBUG_SP_CLOAKING
   MessageInterface::ShowMessage
      ("SpacePoint::IsParameterCloaked() <%p>'%s' returning %s\n", this, GetName().c_str(),
       isCloaked ? "true" : "false");
   #endif
   return isCloaked;
}

//------------------------------------------------------------------------------
// bool IsParameterEqualToDefault(const Integer id) const
//------------------------------------------------------------------------------
bool SpacePoint::IsParameterEqualToDefault(const Integer id) const
{
   if (id == J2000_BODY_NAME)
   {
      return (j2000BodyName == default_j2000BodyName);
   }
   if (id == NAIF_ID)
   {
      return (default_naifId == naifId);
   }
   if (id == NAIF_ID_REFERENCE_FRAME)
   {
      return (default_naifIdRefFrame == naifIdRefFrame);
   }
   if (id == ORBIT_COLOR)
   {
      #ifdef DEBUG_SP_CLOAKING
      MessageInterface::ShowMessage
         ("SpacePoint::IsParameterEqualToDefault() '%s', defaultOrbitColor=%06X, orbitColor=%06X\n",
          GetName().c_str(), defaultOrbitColor, orbitColor);
      #endif
      return (defaultOrbitColor == orbitColor);
   }
   if (id == TARGET_COLOR)
   {
      return (defaultTargetColor == targetColor);
   }
   if (id == SPICE_FRAME_ID)
   {
#ifdef DEBUG_SP_CLOAKING
MessageInterface::ShowMessage
   ("SpacePoint::IsParameterEqualToDefault() '%s'\n", GetName().c_str());
MessageInterface::ShowMessage("--- spiceFrameID = %s, default = %s\n",
      spiceFrameID.c_str(), default_spiceFrameID.c_str());
#endif
      return (default_spiceFrameID == spiceFrameID);
   }

   if (id == ORBIT_SPICE_KERNEL_NAME)
   {
      if (default_orbitSpiceKernelNames.size() != orbitSpiceKernelNames.size())
         return false;
      for (Integer ii = 0; ii < orbitSpiceKernelNames.size(); ii++)
         if (default_orbitSpiceKernelNames.at(ii) != orbitSpiceKernelNames.at(ii))
            return false;
      return true;
   }

   if (id == ATTITUDE_SPICE_KERNEL_NAME)
   {
      #ifdef DEBUG_SP_CLOAKING
      MessageInterface::ShowMessage
         ("SpacePoint::IsParameterEqualToDefault() '%s'\n", GetName().c_str());
      MessageInterface::ShowMessage("--- attitude spice kernel names ...\n");
      MessageInterface::ShowMessage("--- defaults are:\n");
      for (Integer ii = 0; ii < default_attitudeSpiceKernelNames.size(); ii++)
         MessageInterface::ShowMessage("   \"%s\"\n", default_attitudeSpiceKernelNames.at(ii).c_str());
      MessageInterface::ShowMessage("--- current values are:\n");
      for (Integer ii = 0; ii < attitudeSpiceKernelNames.size(); ii++)
         MessageInterface::ShowMessage("   \"%s\"\n", attitudeSpiceKernelNames.at(ii).c_str());
      #endif
      if (default_attitudeSpiceKernelNames.size() != attitudeSpiceKernelNames.size())
         return false;
      for (Integer ii = 0; ii < attitudeSpiceKernelNames.size(); ii++)
         if (default_attitudeSpiceKernelNames.at(ii) != attitudeSpiceKernelNames.at(ii))
            return false;
      return true;
   }

   if (id == FRAME_SPICE_KERNEL_NAME)
   {
      if (default_frameSpiceKernelNames.size() != frameSpiceKernelNames.size())
         return false;
      for (Integer ii = 0; ii < frameSpiceKernelNames.size(); ii++)
         if (default_frameSpiceKernelNames.at(ii) != frameSpiceKernelNames.at(ii))
            return false;
      return true;
   }

   if (id == SC_CLOCK_SPICE_KERNEL_NAME)
   {
      if (default_scClockSpiceKernelNames.size() != scClockSpiceKernelNames.size())
         return false;
      for (Integer ii = 0; ii < scClockSpiceKernelNames.size(); ii++)
         if (default_scClockSpiceKernelNames.at(ii) != scClockSpiceKernelNames.at(ii))
            return false;
      return true;
   }

   return GmatBase::IsParameterEqualToDefault(id);    
}

bool SpacePoint::SaveAllAsDefault()
{
#ifdef DEBUG_SP_CLOAKING
   MessageInterface::ShowMessage("In SaveAllAsDefault (for %s)...\n", instanceName.c_str());
   MessageInterface::ShowMessage("--- spiceFrameID = %s, default_spiceFrameID = %s\n",
         spiceFrameID.c_str(), default_spiceFrameID.c_str());
//   MessageInterface::ShowMessage("--- defaults (attitude spice kernels) are:\n");
//   for (Integer ii = 0; ii < default_attitudeSpiceKernelNames.size(); ii++)
//      MessageInterface::ShowMessage("   \"%s\"\n", default_attitudeSpiceKernelNames.at(ii).c_str());
//   MessageInterface::ShowMessage("--- current values (attitude spice kernels) are:\n");
//   for (Integer ii = 0; ii < attitudeSpiceKernelNames.size(); ii++)
//      MessageInterface::ShowMessage("   \"%s\"\n", attitudeSpiceKernelNames.at(ii).c_str());
#endif
   GmatBase::SaveAllAsDefault();
   default_j2000BodyName  = j2000BodyName;
   default_naifId         = naifId;
   default_naifIdRefFrame = naifIdRefFrame;
   default_spiceFrameID   = spiceFrameID;

   default_orbitSpiceKernelNames.clear();
   default_attitudeSpiceKernelNames.clear();
   default_frameSpiceKernelNames.clear();
   default_scClockSpiceKernelNames.clear();
   default_orbitSpiceKernelNames    = orbitSpiceKernelNames;
   default_attitudeSpiceKernelNames = attitudeSpiceKernelNames;
   default_frameSpiceKernelNames    = frameSpiceKernelNames;
   default_scClockSpiceKernelNames  = scClockSpiceKernelNames;
#ifdef DEBUG_SP_CLOAKING
   MessageInterface::ShowMessage("At end of SaveAllAsDefault (for %s)...\n", instanceName.c_str());
   MessageInterface::ShowMessage("--- spiceFrameID = %s, default_spiceFrameID = %s\n",
         spiceFrameID.c_str(), default_spiceFrameID.c_str());
//   MessageInterface::ShowMessage("--- defaults (attitude spice kernels) are:\n");
//   for (Integer ii = 0; ii < default_attitudeSpiceKernelNames.size(); ii++)
//      MessageInterface::ShowMessage("   \"%s\"\n", default_attitudeSpiceKernelNames.at(ii).c_str());
//   MessageInterface::ShowMessage("--- current values (attitude spice kernels) are:\n");
//   for (Integer ii = 0; ii < attitudeSpiceKernelNames.size(); ii++)
//      MessageInterface::ShowMessage("   \"%s\"\n", attitudeSpiceKernelNames.at(ii).c_str());
#endif
   return true;
}

bool SpacePoint::SaveParameterAsDefault(const Integer id)
{
   if (id == J2000_BODY_NAME)  
   {
      default_j2000BodyName = j2000BodyName;
      return true;
   }
   if (id == NAIF_ID)
   {
      default_naifId = naifId;
      return true;
   }
   if (id == NAIF_ID_REFERENCE_FRAME)
   {
      default_naifIdRefFrame = naifIdRefFrame;
      return true;
   }
   if (id == SPICE_FRAME_ID)
   {
      default_spiceFrameID = spiceFrameID;
      return true;
   }

   if (id == ORBIT_SPICE_KERNEL_NAME)
   {
      default_orbitSpiceKernelNames.clear();
      default_orbitSpiceKernelNames = orbitSpiceKernelNames;
      return true;
   }
   if (id == ATTITUDE_SPICE_KERNEL_NAME)
   {
      default_attitudeSpiceKernelNames.clear();
      default_attitudeSpiceKernelNames = attitudeSpiceKernelNames;
      return true;
   }
   if (id == FRAME_SPICE_KERNEL_NAME)
   {
      default_frameSpiceKernelNames.clear();
      default_frameSpiceKernelNames = frameSpiceKernelNames;
      return true;
   }
   if (id == SC_CLOCK_SPICE_KERNEL_NAME)
   {
      default_scClockSpiceKernelNames.clear();
      default_scClockSpiceKernelNames = scClockSpiceKernelNames;
      return true;
   }

   return GmatBase::SaveParameterAsDefault(id);
}


//------------------------------------------------------------------------------
//  virtual bool HasAttitude() const
//------------------------------------------------------------------------------
/**
 * @return true if attitude was computed or can be computed, false otherwise
 */
//------------------------------------------------------------------------------
bool SpacePoint::HasAttitude() const
{
   if (theSolarSystem != NULL && j2000Body != NULL)
      if (inertialCS == NULL && bodyFixedCS == NULL)
         return hasAttitude;
      else
         return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// virtual const Rmatrix33& GetAttitude(Real a1mjdTime)
//------------------------------------------------------------------------------
/**
 * @return Computed cosine matrix if attitude can be computed for celestial body
 */
//------------------------------------------------------------------------------
const Rmatrix33& SpacePoint::GetAttitude(Real a1mjdTime)
{
   #ifdef DEBUG_ATTITUDE
   MessageInterface::ShowMessage
      ("SpacePoint::GetAttitude() '%s' entered, epoch=%f, theSolarSystem=<%p>, "
       "j2000Body=<%p>\n   inertialCS=<%p>, bodyFixedCS=<%p>\n", GetName().c_str(),
       a1mjdTime, theSolarSystem, j2000Body, inertialCS, bodyFixedCS);
   #endif
   
   // If not a celestial body, just return identity matrix
   if (!IsOfType(Gmat::CELESTIAL_BODY))
      return cosineMat;
   
   if (theSolarSystem != NULL && j2000Body != NULL)
   {
      Rvector6 currState = GetMJ2000State(a1mjdTime);
      Rvector6 outState;
      
      #ifdef DEBUG_ATTITUDE
      MessageInterface::ShowMessage
         ("   currState=%s\n", currState.ToString(16).c_str());
      #endif
      
      if (inertialCS == NULL && bodyFixedCS == NULL)
      {
         inertialCS =
            CoordinateSystem::
            CreateLocalCoordinateSystem("Sp_Inertial", "MJ2000Eq", this,
                                        NULL, NULL, j2000Body, theSolarSystem);
         bodyFixedCS =
            CoordinateSystem::
            CreateLocalCoordinateSystem("Sp_BodyFixed", "BodyFixed", this,
                                        NULL, NULL, j2000Body, theSolarSystem);
         #ifdef DEBUG_ATTITUDE
         MessageInterface::ShowMessage
            ("   inertialCS=<%p>, bodyFixedCS=<%p>\n", inertialCS, bodyFixedCS);
         #endif
         
         // if coordinate systems are still NULL, just return identity matrix
         if (inertialCS == NULL || bodyFixedCS == NULL)
            return cosineMat;
      }
      
      CoordinateConverter coordConverter;
      // The Attitude matrix is rotation matrix from inertial to body fixed,
      // but coordConverter.GetLastRotationMatrix() returns rotation matrix to inertial
      // so convert body fixed to inertial
      coordConverter.Convert(a1mjdTime, currState, bodyFixedCS, outState,
                             inertialCS, false, false);
      Rmatrix33 rotMat = coordConverter.GetLastRotationMatrix();
      
      #ifdef DEBUG_ATTITUDE
      MessageInterface::ShowMessage
         ("   outState =%s\n", outState.ToString(16).c_str());
      MessageInterface::ShowMessage
         ("   rotMat   =\n%s", rotMat.ToString(16, 25, false, "      ").c_str());
      #endif
      
      cosineMat = rotMat;
      hasAttitude = true;
   }
   else
   {
      hasAttitude = false;
      MessageInterface::ShowMessage
         ("*** WARNING *** SpacePoint::GetAttitude() Cannot compute attitude at epoch %f, "
          "SolarSystem or J2000Body is NULL\n", a1mjdTime);
   }
   
   return cosineMat;
}

//------------------------------------------------------------------------------
// UnsignedInt GetDefaultOrbitColor()
//------------------------------------------------------------------------------
UnsignedInt SpacePoint::GetDefaultOrbitColor()
{
   return defaultOrbitColor;
}

//------------------------------------------------------------------------------
// UnsignedInt GetDefaultTargetColor()
//------------------------------------------------------------------------------
UnsignedInt SpacePoint::GetDefaultTargetColor()
{
   return defaultTargetColor;
}

//------------------------------------------------------------------------------
// UnsignedInt GetCurrentOrbitColor()
//------------------------------------------------------------------------------
UnsignedInt SpacePoint::GetCurrentOrbitColor()
{
   return orbitColor;
}

//------------------------------------------------------------------------------
// UnsignedInt GetCurrentTargetColor()
//------------------------------------------------------------------------------
UnsignedInt SpacePoint::GetCurrentTargetColor()
{
   return targetColor;
}

//------------------------------------------------------------------------------
// std::string GetOrbitColorString()
//------------------------------------------------------------------------------
std::string SpacePoint::GetOrbitColorString()
{
   return orbitColorStr;
}

//------------------------------------------------------------------------------
// std::string GetTargetColorString()
//------------------------------------------------------------------------------
std::string SpacePoint::GetTargetColorString()
{
   return targetColorStr;
}

//------------------------------------------------------------------------------
// void SetCurrentOrbitColor(UnsignedInt color)
//------------------------------------------------------------------------------
void SpacePoint::SetCurrentOrbitColor(UnsignedInt color)
{
   #ifdef DEBUG_COLOR
   MessageInterface::ShowMessage
      ("SpacePoint::SetCurrentOrbitColor() setting color = %06X\n", color);
   #endif
   
   orbitColor = color;
}

//------------------------------------------------------------------------------
// void SetCurrentTargetColor(UnsignedInt color)
//------------------------------------------------------------------------------
void SpacePoint::SetCurrentTargetColor(UnsignedInt color)
{
   targetColor = color;
}

//------------------------------------------------------------------------------
// void SetDefaultColors(UnsignedInt orbColor, UnsignedInt targColor)
//------------------------------------------------------------------------------
void SpacePoint::SetDefaultColors(UnsignedInt orbColor, UnsignedInt targColor)
{
   #ifdef DEBUG_COLOR
   MessageInterface::ShowMessage
      ("SpacePoint::SetDefaultColors() <%p>'%s' entered, orbColor=%06X, targColor=%06X, "
       "useOrbitColorName=%d, useTargetColorName=%d\n", this, GetName().c_str(), orbColor,
       targColor, useOrbitColorName, useTargetColorName);
   #endif
   
   defaultOrbitColor = orbColor;
   defaultTargetColor = targColor;
   orbitColor = orbColor;
   targetColor = targColor;
   orbitColorStr = RgbColor::ToRgbString(orbColor);
   targetColorStr = RgbColor::ToRgbString(targColor);
   
   #ifdef DEBUG_COLOR
   MessageInterface::ShowMessage
      ("   orbitColorStr=%s, targetColorStr=%s\n", orbitColorStr.c_str(), targetColorStr.c_str());
   #endif
   
   // Get predefined color name
   try
   {
      ColorDatabase *colordb = ColorDatabase::Instance();
      if (useOrbitColorName)
         orbitColorStr = colordb->GetColorName(orbitColor);
      if (useTargetColorName)
         targetColorStr = colordb->GetColorName(targetColor);
   }
   catch (BaseException &be)
   {
      throw;
   }

   #ifdef DEBUG_COLOR
   MessageInterface::ShowMessage
      ("SpacePoint::SetDefaultColors() '%s' leaving, orbitColorStr=%s, targetColorStr=%s\n",
       GetName().c_str(), orbitColorStr.c_str(), targetColorStr.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void SetSpacecraftDefaultColors()
//------------------------------------------------------------------------------
/**
 * Sets spacecraft default colors from the predefined color list
 */
//------------------------------------------------------------------------------
void SpacePoint::SetSpacecraftDefaultColors()
{
   #ifdef DEBUG_COLOR
   MessageInterface::ShowMessage
      ("SpacePoint::SetSpacecraftDefaultColors() <%p>'%s' entered, "
       "spacecraftInstanceCount = %d, MAX_SP_COLOR = %d\n", this, GetName().c_str(),
       spacecraftInstanceCount, MAX_SP_COLOR);
   #endif
   
   // MAX_SP_COLOR doesn't work with % modulo (LOJ: 2014.03.13)
   //Integer index = (spacecraftInstanceCount-1) % MAX_SP_COLOR;
   Integer maxColor = (Integer)MAX_SP_COLOR;
   Integer index = (spacecraftInstanceCount-1) % maxColor;
   
   #ifdef DEBUG_COLOR
   MessageInterface::ShowMessage("   ==> index = %d\n", index);
   #endif
   UnsignedInt orbColor = DEFAULT_ORBIT_COLOR[index];
   UnsignedInt targColor = DEFAULT_TARGET_COLOR[index];
   SetDefaultColors(orbColor, targColor);
   
   #ifdef DEBUG_COLOR
   MessageInterface::ShowMessage
      ("SpacePoint::SetSpacecraftDefaultColors() <%p>'%s' leaving\n",
       this, GetName().c_str());
   #endif
}

//------------------------------------------------------------------------------
// static void SpacePoint::ClearInstanceCount()
//------------------------------------------------------------------------------
void SpacePoint::ClearInstanceCount()
{
   spacecraftInstanceCount = 0;
}

//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000Acceleration(const A1Mjd &atTime)
//------------------------------------------------------------------------------
const Rvector3 SpacePoint::GetMJ2000Acceleration(const A1Mjd &atTime)
{
   return Rvector3(0.0,0.0,0.0);
}

//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000Acceleration(const GmatTime &atTime)
//------------------------------------------------------------------------------
const Rvector3 SpacePoint::GetMJ2000Acceleration(const GmatTime &atTime)
{
   return Rvector3(0.0, 0.0, 0.0);
}


//------------------------------------------------------------------------------
// const Rvector6 GetMJ2000PrecState(const GmatTime &atTime)
//------------------------------------------------------------------------------
const Rvector6 SpacePoint::GetMJ2000PrecState(const GmatTime &atTime)
{
   Rvector6 s;
   if (hasPrecisionTime)
      s = GetMJ2000State(atTime);
   else
   {
      if (IsOfType(Gmat::SPACECRAFT))
      {
         GmatTime t = atTime;						      // GmatTime value of atTime
         GmatEpoch te = t.GetMjd();				      // GmatEpoch value of atTime		Note that: value of t and value of te are different due to their type's precision
         Real dt = (t - te).GetTimeInSec();			// difference in time (second) between GmatTime value and Gmatepoch value of atTime)  

         s = GetMJ2000State(A1Mjd(te));				// state at time te
         Rvector3 a = GetMJ2000Acceleration(A1Mjd(te));		      // acceleration at time te
         Rvector6 sdot(s[3], s[4], s[5], a[0], a[1], a[2]); 		// velocity at time te
         s = s + sdot*dt;							      // state at time atTime 
      }
      else
      {
         s = GetMJ2000State(atTime);
      }
   }

   return s;
}


//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000PrecPosition(const GmatTime &atTime)
//------------------------------------------------------------------------------
const Rvector3 SpacePoint::GetMJ2000PrecPosition(const GmatTime &atTime)
{
   Rvector3 r;
   if (hasPrecisionTime)
      r = GetMJ2000Position(atTime);
   else
   {
      if (IsOfType(Gmat::SPACECRAFT))
      {
         GmatTime t = atTime;						// GmatTime value of atTime
         GmatEpoch te = t.GetMjd();				// GmatEpoch value of atTime		Note that: value of t and value of te are different due to their type's precision
         Real dt = (t - te).GetTimeInSec();  // difference in time (second) between GmatTime value and Gmatepoch value of atTime)  

         r = GetMJ2000Position(A1Mjd(te));		         // position at time te
         Rvector3 v = GetMJ2000Velocity(A1Mjd(te));		// velocity at time te
         r = r + v*dt;							               // pocition at time atTime 
      }
      else
      {
         r = GetMJ2000Position(atTime);
      }
   }

   return r;
}



//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000PrecVelocity(const GmatTime &atTime)
//------------------------------------------------------------------------------
const Rvector3 SpacePoint::GetMJ2000PrecVelocity(const GmatTime &atTime)
{
   Rvector3 v;
   if (hasPrecisionTime)
      v = GetMJ2000Velocity(atTime);
   else
   {
      if (IsOfType(Gmat::SPACECRAFT))
      {
         GmatTime t = atTime;						      // GmatTime value of atTime
         GmatEpoch te = t.GetMjd();				      // GmatEpoch value of atTime		Note that: value of t and value of te are different due to their type's precision
         Real dt = (t - te).GetTimeInSec();			// difference in time (second) between GmatTime value and Gmatepoch value of atTime)  

         v = GetMJ2000Velocity(A1Mjd(te));		         // velocity at time te
         Rvector6 s = GetMJ2000State(A1Mjd(te));
         Rvector3 a = GetMJ2000Acceleration(A1Mjd(te));	// acceleration at time te
         v = v + a*dt;			               				// velocity at time atTime 
      }
      else
      {
         v = GetMJ2000Velocity(atTime);
      }
   }

   return v;
}


//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000PrecAcceleration(const GmatTime &atTime)
//------------------------------------------------------------------------------
const Rvector3 SpacePoint::GetMJ2000PrecAcceleration(const GmatTime &atTime)
{
   if (hasPrecisionTime)
      return GetMJ2000Acceleration(atTime);
   else
   {
      if (IsOfType(Gmat::SPACECRAFT))
      {
         GmatTime t = atTime;
         // Assume that acceleration is the same with a very tiny change of time
         return GetMJ2000Acceleration(A1Mjd(t.GetMjd()));
      }
      else
         return GetMJ2000Acceleration(atTime);
   }
}


void SpacePoint::RemoveSpiceKernelName(const std::string &kernelType,
                                       const std::string &fileName)
{
   if (kernelType == "Orbit")
   {
      StringArray::iterator i;
      i = find(orbitSpiceKernelNames.begin(), orbitSpiceKernelNames.end(), fileName);
      if (i != orbitSpiceKernelNames.end())  orbitSpiceKernelNames.erase(i);
   }
   else if ((kernelType == "Attitude") || (kernelType == "Planetary"))
   {
      StringArray::iterator i;
      i = find(attitudeSpiceKernelNames.begin(), attitudeSpiceKernelNames.end(), fileName);
      if (i != attitudeSpiceKernelNames.end())  attitudeSpiceKernelNames.erase(i);
   }
   else if (kernelType == "SCClock")
   {
      StringArray::iterator i;
      i = find(scClockSpiceKernelNames.begin(), scClockSpiceKernelNames.end(), fileName);
      if (i != scClockSpiceKernelNames.end())  scClockSpiceKernelNames.erase(i);
   }
   else if (kernelType == "Frame")
   {
      StringArray::iterator i;
      i = find(frameSpiceKernelNames.begin(), frameSpiceKernelNames.end(), fileName);
      if (i != frameSpiceKernelNames.end())  frameSpiceKernelNames.erase(i);
   }
}



//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string SpacePoint::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SpacePointParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer SpacePoint::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < SpacePointParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType SpacePoint::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < SpacePointParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   
   return GmatBase::GetParameterType(id);
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
 *
 */
//------------------------------------------------------------------------------
std::string SpacePoint::GetParameterTypeString(const Integer id) const
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
bool SpacePoint::IsParameterReadOnly(const Integer id) const
{
   if (id == J2000_BODY_NAME)
      return true;
   if (id == NAIF_ID) // set to false in appropriate derived classes
      return true;
   if (id == NAIF_ID_REFERENCE_FRAME) // set to false in appropriate derived classes
      return true;
//   if (id == SPICE_FRAME_ID) // set to false in appropriate derived classes
//      return true;

//   // Turn off parameters for string arrays if they are empty
//   if (id == ORBIT_SPICE_KERNEL_NAME)
//      if (orbitSpiceKernelNames.size() == 0)
//         return true;
//   if (id == ATTITUDE_SPICE_KERNEL_NAME)
//      if (attitudeSpiceKernelNames.size() == 0)
//         return true;
//   if (id == SC_CLOCK_SPICE_KERNEL_NAME)
//      if (scClockSpiceKernelNames.size() == 0)
//         return true;
//   if (id == FRAME_SPICE_KERNEL_NAME)
//      if (frameSpiceKernelNames.size() == 0)
//         return true;

   // Hide A1Epoch
   if (id == EPOCH_PARAM)
   {
      return true;
   }

   return GmatBase::IsParameterReadOnly(id);
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
bool SpacePoint::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//---------------------------------------------------------------------------
// bool IsSquareBracketAllowedInSetting(const Integer id) const
//---------------------------------------------------------------------------
bool SpacePoint::IsSquareBracketAllowedInSetting(const Integer id) const
{
   if (id == ORBIT_COLOR || id == TARGET_COLOR)
      return true;
   else
      return GmatBase::IsSquareBracketAllowedInSetting(id);
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool SpacePoint::IsParameterCommandModeSettable(const Integer id) const
{
   if ((id == ORBIT_COLOR) || (id == TARGET_COLOR))
      return true;

   return GmatBase::IsParameterCommandModeSettable(id);
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer SpacePoint::GetIntegerParameter(const Integer id) const
{
   if (id == NAIF_ID)                 return naifId;
   if (id == NAIF_ID_REFERENCE_FRAME) return naifIdRefFrame;

   return GmatBase::GetIntegerParameter(id);
}


//---------------------------------------------------------------------------
//  Integer GetIntegerParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Integer SpacePoint::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Integer  SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * This method sets the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Integer value for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer SpacePoint::SetIntegerParameter(const Integer id,
                                        const Integer value)
{
   #ifdef DEBUG_SPICE_KERNEL
      MessageInterface::ShowMessage("SpacePoint: setting %s to %d\n",
            GetParameterText(id).c_str(), value);
   #endif
   if (id == NAIF_ID)
   {
      naifId              = value;
      return naifId;
   }
   if (id == NAIF_ID_REFERENCE_FRAME)
   {
      naifIdRefFrame      = value;
      return naifIdRefFrame;
   }
   return GmatBase::SetIntegerParameter(id,value);

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
 *
 */
//------------------------------------------------------------------------------
std::string SpacePoint::GetStringParameter(const Integer id) const
{
   if (id == J2000_BODY_NAME)   
   {
      if (j2000Body) return j2000Body->GetName();
      else           return j2000BodyName;
   }
   if (id == SPICE_FRAME_ID)
   {
      return spiceFrameID;
   }

   // return entire brace-enclosed array for the kernel name arrays
   // (needed so that assignments will work inside of GmatFunctions) wcs 2010.05.19
   std::string kernelArrayString = "{";
   if (id == ORBIT_SPICE_KERNEL_NAME)
   {
      for (unsigned int ii = 0; ii < orbitSpiceKernelNames.size(); ii++)
      {
         if (ii != 0) kernelArrayString += ",";
         kernelArrayString              += orbitSpiceKernelNames.at(ii);
      }
      kernelArrayString += "}";
      return kernelArrayString;
   }
   if (id == ATTITUDE_SPICE_KERNEL_NAME)
   {
      for (unsigned int ii = 0; ii < attitudeSpiceKernelNames.size(); ii++)
      {
         if (ii != 0) kernelArrayString += ",";
         kernelArrayString              += attitudeSpiceKernelNames.at(ii);
      }
      kernelArrayString += "}";
      return kernelArrayString;
   }
   if (id == SC_CLOCK_SPICE_KERNEL_NAME)
   {
      for (unsigned int ii = 0; ii < scClockSpiceKernelNames.size(); ii++)
      {
         if (ii != 0) kernelArrayString += ",";
         kernelArrayString              += scClockSpiceKernelNames.at(ii);
      }
      kernelArrayString += "}";
      return kernelArrayString;
   }
   if (id == FRAME_SPICE_KERNEL_NAME)
   {
      for (unsigned int ii = 0; ii < frameSpiceKernelNames.size(); ii++)
      {
         if (ii != 0) kernelArrayString += ",";
         kernelArrayString              += frameSpiceKernelNames.at(ii);
      }
      kernelArrayString += "}";
      return kernelArrayString;
   }

   if (id == ORBIT_COLOR || id == TARGET_COLOR)
   {
      if (id == ORBIT_COLOR)
         return orbitColorStr;
      
      if (id == TARGET_COLOR)
         return targetColorStr;
   }
   
   return GmatBase::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool SpacePoint::SetStringParameter(const Integer id, 
                                    const std::string &value)
{
   #ifdef DEBUG_SET_STRING
   MessageInterface::ShowMessage
      ("SpacePoint::SetStringParameter() <%p>'%s' entered, id=%d, value='%s'\n",
       this, GetName().c_str(), id, value.c_str());
   #endif
   
   // this is also handled in CelestialBody and Spacecraft
   // it is caught here if not handled there for some reason
   if (id == J2000_BODY_NAME) 
   {
      static bool writeIgnoredMessage = true;
      if (writeIgnoredMessage)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"J2000BodyName\" on SpacePoint objects is ignored and will be "
             "removed from a future build\n");
         writeIgnoredMessage = false;
      }

      return true;
   }
   
   if (id == SPICE_FRAME_ID)
   {
      // @todo - add validation here <<<<<<<<
      spiceFrameID = value;
      if (spiceFrameID != "")
      {
         spiceCS =
            CoordinateSystem::
            CreateLocalCoordinateSystem("Sp_Spice", "SPICE", this,
               NULL, NULL, j2000Body, theSolarSystem);
      }
      return true;
   }

//   bool alreadyInList = false;
   if (id == ORBIT_SPICE_KERNEL_NAME)
   {
      #ifdef DEBUG_SPACE_POINT_ORBIT_KERNELS
          MessageInterface::ShowMessage("Entering SP::SetString with value = \"%s\"\n",
             value.c_str());
      #endif
      // if it is the whole StringArray of kernel names, handle that here
      // (needed so that assignments will work inside of GmatFunctions) wcs 2010.05.18
      std::string value1 = GmatStringUtil::Trim(value);
      std::string parsed = "";
      if (GmatStringUtil::IsEnclosedWithBraces(value1))
      {
         orbitSpiceKernelNames.clear();
         StringArray kernels = GmatStringUtil::ToStringArray(value1);
         #ifdef DEBUG_SPACE_POINT_ORBIT_KERNELS
             MessageInterface::ShowMessage("In SP::SetString value IS enclosed with braces and values are:\n");
             for (unsigned int ii = 0; ii < kernels.size(); ii++)
                MessageInterface::ShowMessage("   (%d)   %s\n", (Integer) ii, (kernels.at(ii)).c_str());
         #endif
         for (unsigned int ii = 0; ii < kernels.size(); ii++)
         {
            parsed = ParseKernelName(kernels.at(ii));
            ValidateKernel(parsed, "OrbitSpiceKernelName", "spk");
            orbitSpiceKernelNames.push_back(parsed);
         }
      }
      else
      {
         // trim leading and trailing blanks and change back slashes to forward slashes
         std::string trimmed = ParseKernelName(value);
         #ifdef DEBUG_SPACE_POINT_ORBIT_KERNELS
             MessageInterface::ShowMessage(
                   "In SP::SetString value IS NOT enclosed with braces and trimmed value is %s\n", trimmed.c_str());
         #endif
         ValidateKernel(trimmed, "OrbitSpiceKernelName", "spk");
         #ifdef DEBUG_SPACE_POINT_ORBIT_KERNELS
             MessageInterface::ShowMessage("In SP::SetString, value = %s, trimmed = %s\n",
                   value.c_str(), trimmed.c_str());
         #endif
         if (find(orbitSpiceKernelNames.begin(), orbitSpiceKernelNames.end(), trimmed) == orbitSpiceKernelNames.end())
            orbitSpiceKernelNames.push_back(trimmed);
      }
      spiceSetupDone = false;
      return true;
   }
   if (id == ATTITUDE_SPICE_KERNEL_NAME)
   {
      #ifdef DEBUG_SET_STRING
      MessageInterface::ShowMessage
         ("SpacePoint::SetStringParameter() Setting attitude/planetary file(s)\n");
      #endif
      // if it is the whole StringArray of kernel names, handle that here
      // (needed so that assignments will work inside of GmatFunctions) wcs 2010.05.18
      std::string value1 = GmatStringUtil::Trim(value);
      std::string parsed = "";
      if (GmatStringUtil::IsEnclosedWithBraces(value1))
      {
         attitudeSpiceKernelNames.clear();
         StringArray kernels = GmatStringUtil::ToStringArray(value1);
         for (unsigned int ii = 0; ii < kernels.size(); ii++)
         {
            parsed = ParseKernelName(kernels.at(ii));
            if (IsOfType("CelestialBody"))
               ValidateKernel(parsed, "PlanetarySpiceKernelName", "pck");
            else
               ValidateKernel(parsed, "AttitudeSpiceKernelName", "ck");

            attitudeSpiceKernelNames.push_back(parsed);
         }
      }
      else
      {
         std::string trimmed = ParseKernelName(value);
         #ifdef DEBUG_SET_STRING
         MessageInterface::ShowMessage
            ("SpacePoint::SetStringParameter() trimmed = %s, about to validate\n",
                  trimmed.c_str());
         #endif
         if (IsOfType("CelestialBody"))
            ValidateKernel(trimmed, "PlanetarySpiceKernelName", "pck");
         else
            ValidateKernel(trimmed, "AttitudeSpiceKernelName", "ck");
         if (find(attitudeSpiceKernelNames.begin(), attitudeSpiceKernelNames.end(),trimmed) == attitudeSpiceKernelNames.end())
            attitudeSpiceKernelNames.push_back(trimmed);
     }
      spiceSetupDone = false;
      return true;
   }
   if (id == SC_CLOCK_SPICE_KERNEL_NAME)
   {
      // if it is the whole StringArray of kernel names, handle that here
      // (needed so that assignments will work inside of GmatFunctions) wcs 2010.05.18
      std::string value1 = GmatStringUtil::Trim(value);
      std::string parsed = "";
      if (GmatStringUtil::IsEnclosedWithBraces(value1))
      {
         scClockSpiceKernelNames.clear();
         StringArray kernels = GmatStringUtil::ToStringArray(value1);
         for (unsigned int ii = 0; ii < kernels.size(); ii++)
         {
            parsed = ParseKernelName(kernels.at(ii));
            ValidateKernel(parsed, "SCClockSpiceKernelName", "sclk");
            scClockSpiceKernelNames.push_back(parsed);
         }
      }
      else
      {
         std::string trimmed = ParseKernelName(value);
         ValidateKernel(trimmed, "SCClockSpiceKernelName", "sclk");
         if (find(scClockSpiceKernelNames.begin(), scClockSpiceKernelNames.end(),trimmed) == scClockSpiceKernelNames.end())
            scClockSpiceKernelNames.push_back(trimmed);
      }
      spiceSetupDone = false;
      return true;
   }
   if (id == FRAME_SPICE_KERNEL_NAME)
   {
      // if it is the whole StringArray of kernel names, handle that here
      // (needed so that assignments will work inside of GmatFunctions) wcs 2010.05.18
      std::string value1 = GmatStringUtil::Trim(value);
      std::string parsed = "";
      if (GmatStringUtil::IsEnclosedWithBraces(value1))
      {
         frameSpiceKernelNames.clear();
         StringArray kernels = GmatStringUtil::ToStringArray(value1);
         for (unsigned int ii = 0; ii < kernels.size(); ii++)
         {
            parsed = ParseKernelName(kernels.at(ii));
            ValidateKernel(parsed, "FrameSpiceKernelName", "fk");
            frameSpiceKernelNames.push_back(parsed);
         }
      }
      else
      {
         std::string trimmed = ParseKernelName(value);
         ValidateKernel(trimmed, "FrameSpiceKernelName", "fk");
         if (find(frameSpiceKernelNames.begin(), frameSpiceKernelNames.end(),trimmed) == frameSpiceKernelNames.end())
            frameSpiceKernelNames.push_back(trimmed);
      }
      spiceSetupDone = false;
      return true;
   }
   
   // Set colors (LOJ: 2013.11.05)
   if (id == ORBIT_COLOR || id == TARGET_COLOR)
   {
      #ifdef DEBUG_SET_STRING
      MessageInterface::ShowMessage("   orbitColor  = %06X\n", orbitColor);
      MessageInterface::ShowMessage("   targetColor = %06X\n", targetColor);
      #endif
      bool useColorName = true;
      UnsignedInt intColor;
      std::string colorStr = value;
      // Check if RGB value is used
      if (GmatStringUtil::IsEnclosedWithBrackets(value))
      {
         useColorName = false;
         try
         {
            // Convert RGB value to unsigned integer
            intColor = RgbColor::ToIntColor(value);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }
      
      // Check if color is in the database
      if (useColorName)
      {
         // Get corresponding UnsignedInt color
         try
         {
            intColor = ColorDatabase::Instance()->GetIntColor(value);
         }
         catch (BaseException &be)
         {
            throw;
         }
      }
      
      // Save as unsigned integer
      if (id == ORBIT_COLOR)
      {
         useOrbitColorName = useColorName;
         orbitColor = intColor;
         if (useColorName)
            orbitColorStr = colorStr;
         else
            orbitColorStr = RgbColor::ToRgbString(intColor);
      }
      else if (id == TARGET_COLOR)
      {
         useTargetColorName = useColorName;
         targetColor = intColor;
         if (useColorName)
            targetColorStr = colorStr;
         else
            targetColorStr = RgbColor::ToRgbString(intColor);
      }
      
      #ifdef DEBUG_SET_STRING
      MessageInterface::ShowMessage
         ("   useColorName = %d, cloaking = %d\n", useColorName, cloaking);
      MessageInterface::ShowMessage
         ("   orbitColor  = %06X, orbitColorStr='%s'\n", orbitColor, orbitColorStr.c_str());
      MessageInterface::ShowMessage
         ("   targetColor = %06X, targetColorStr='%s'\n", targetColor, targetColorStr.c_str());
      MessageInterface::ShowMessage
         ("SpacePoint::SetStringParameter() '%s' returning true\n", GetName().c_str());
      #endif
      return true;
   }
   
   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label>  label ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string SpacePoint::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    <label> Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------

bool SpacePoint::SetStringParameter(const std::string &label,
                                    const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Accesses lists of names and other StringArray parameters.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& SpacePoint::GetStringArrayParameter(const Integer id) const
{
   if (id == ORBIT_SPICE_KERNEL_NAME)
      return orbitSpiceKernelNames;
   if (id == ATTITUDE_SPICE_KERNEL_NAME)
      return attitudeSpiceKernelNames;
   if (id == SC_CLOCK_SPICE_KERNEL_NAME)
      return scClockSpiceKernelNames;
   if (id == FRAME_SPICE_KERNEL_NAME)
      return frameSpiceKernelNames;
   return GmatBase::GetStringArrayParameter(id);
}

const StringArray& SpacePoint::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the SpacePoint class.
 *
 * @param <type>  type of the reference object requested
 * @param <name>  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SpacePoint::GetRefObject(const UnsignedInt type,
                                   const std::string &name)
{
   switch (type)
   {
      case Gmat::SPACE_POINT:
         return j2000Body;
      default:
         break;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObject(type, name);
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
 *
 * WARNING: The J200Body must be set identically for all objects in a GMAT run;
 * not doing so will give incorrect results.
 * In addition, the setting of a body other than Earth as the J2000Body has
 * not been tested.
 */
//------------------------------------------------------------------------------
bool SpacePoint::SetRefObject(GmatBase *obj, const UnsignedInt type,
                              const std::string &name)
{
   
   switch (type)
   {
      case Gmat::SPACE_POINT:
      {
         j2000Body = (SpacePoint*) obj;
         return true;
      }
      default:
         break;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return GmatBase::SetRefObject(obj, type, name);
}


// DJC Added, 12/16/04
// This seems like it should NOT be needed, but GCC seems to be confused about 
// the overloaded versions of the following six methods:
// WCS modified 2010.05.04 - added string arrays for kernel names, so some of these
// need to be implemented

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
std::string SpacePoint::GetStringParameter(const Integer id,
                                           const Integer index) const
{
   switch (id)
   {
   case ORBIT_SPICE_KERNEL_NAME:
      {
         if ((index >= 0) && (index < (Integer) orbitSpiceKernelNames.size()))
            return orbitSpiceKernelNames[index];
         else
            throw GmatBaseException("Index into array of SPK kernels is out-of-bounds.\n");
      }
   case ATTITUDE_SPICE_KERNEL_NAME:
      {
         if ((index >= 0) && (index < (Integer) attitudeSpiceKernelNames.size()))
            return attitudeSpiceKernelNames[index];
         else
            throw GmatBaseException("Index into array of CK kernels is out-of-bounds.\n");
     }
   case SC_CLOCK_SPICE_KERNEL_NAME:
      {
         if ((index >= 0) && (index < (Integer) scClockSpiceKernelNames.size()))
            return scClockSpiceKernelNames[index];
         else
            throw GmatBaseException("Index into array of SCLK kernels is out-of-bounds.\n");
      }
   case FRAME_SPICE_KERNEL_NAME:
      {
         if ((index >= 0) && (index < (Integer) frameSpiceKernelNames.size()))
            return frameSpiceKernelNames[index];
         else
            throw GmatBaseException("Index into array of FK kernels is out-of-bounds.\n");
     }

      default:
         break;
   }
   return GmatBase::GetStringParameter(id, index);
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
bool SpacePoint::SetStringParameter(const Integer id, 
                                    const std::string &value, 
                                    const Integer index)
{
#ifdef DEBUG_SPICE_KERNEL
   MessageInterface::ShowMessage(
         "Entering SpacePoint::SetStringParameter with id = %d, value = %s, and index = %d\n",
         id, value.c_str(), index);
#endif
//   if (index < 0)
//   {
//      GmatBaseException ex;
//      ex.SetDetails("The index %d is out-of-range for field \"%s\"", index,
//                    GetParameterText(id).c_str());
//      throw ex;
//   }
    switch (id)
   {
      case ORBIT_SPICE_KERNEL_NAME:
      {
         ValidateKernel(value, "OrbitSpiceKernelName", "spk");
         if (index < (Integer)orbitSpiceKernelNames.size())
            orbitSpiceKernelNames[index] = value;
         // Only add the orbit spice kernel name if it is not in the list already
         else if (find(orbitSpiceKernelNames.begin(), orbitSpiceKernelNames.end(),
               value) == orbitSpiceKernelNames.end())
            orbitSpiceKernelNames.push_back(value);
         spiceSetupDone = false;
         return true;
      }
   case ATTITUDE_SPICE_KERNEL_NAME:
      {
         if (IsOfType("CelestialBody"))
            ValidateKernel(value, "PlanetarySpiceKernelName", "pck");
         else
            ValidateKernel(value, "AttitudeSpiceKernelName", "ck");
         if (index < (Integer)attitudeSpiceKernelNames.size())
            attitudeSpiceKernelNames[index] = value;
         // Only add the orbit spice kernel name if it is not in the list already
         else if (find(attitudeSpiceKernelNames.begin(), attitudeSpiceKernelNames.end(),
               value) == attitudeSpiceKernelNames.end())
            attitudeSpiceKernelNames.push_back(value);
         spiceSetupDone = false;
         return true;
     }
   case SC_CLOCK_SPICE_KERNEL_NAME:
      {
         ValidateKernel(value, "SCClockSpiceKernelName", "sclk");
         if (index < (Integer)scClockSpiceKernelNames.size())
            scClockSpiceKernelNames[index] = value;
         // Only add the orbit spice kernel name if it is not in the list already
         else if (find(scClockSpiceKernelNames.begin(), scClockSpiceKernelNames.end(),
               value) == scClockSpiceKernelNames.end())
            scClockSpiceKernelNames.push_back(value);
         spiceSetupDone = false;
         return true;
      }
   case FRAME_SPICE_KERNEL_NAME:
      {
         ValidateKernel(value, "FrameSpiceKernelName", "fk");
         if (index < (Integer)frameSpiceKernelNames.size())
            frameSpiceKernelNames[index] = value;
         // Only add the orbit spice kernel name if it is not in the list already
         else if (find(frameSpiceKernelNames.begin(), frameSpiceKernelNames.end(),
               value) == frameSpiceKernelNames.end())
            frameSpiceKernelNames.push_back(value);
         spiceSetupDone = false;
         return true;
     }

      default:
         break;
   }

   return GmatBase::SetStringParameter(id, value, index);
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
std::string SpacePoint::GetStringParameter(const std::string &label, 
                                           const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetStringParameter(id, index);
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
bool SpacePoint::SetStringParameter(const std::string &label, 
                                    const std::string &value, 
                                    const Integer index)
{
   Integer id = GetParameterID(label);
   return SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to a reference object contained in a vector of
 * objects in the SpacePoint class.
 *
 * @param type type of the reference object requested
 * @param name name of the reference object requested
 * @param index index for the particular object requested.
 *
 * @return pointer to the reference object requested.
 */
//------------------------------------------------------------------------------
GmatBase* SpacePoint::GetRefObject(const UnsignedInt type,
                                   const std::string &name, 
                                   const Integer index)
{
   return GmatBase::GetRefObject(type, name, index);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a pointer to a reference object in a vector of objects in 
 * the SpacePoint class.
 *
 * @param obj The reference object.
 * @param type type of the reference object requested
 * @param name name of the reference object requested
 * @param index index for the particular object requested.
 *
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool SpacePoint::SetRefObject(GmatBase *obj, const UnsignedInt type,
                              const std::string &name, const Integer index)
{
   return GmatBase::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// std::string ParseKernelName(const std::string &kernel)
//------------------------------------------------------------------------------
/**
 * This method parses the input kernel name to trim blanks and enclosing
 * single quotes.
 *
 * @param kernel The input kernel name
 *
 * @return the kernel name without leading or trailing blanks and without
 *         enclosing single quotes
 */
//------------------------------------------------------------------------------
std::string SpacePoint::ParseKernelName(const std::string &kernel)
{
   return GmatStringUtil::Trim(GmatStringUtil::RemoveEnclosingString(GmatStringUtil::Trim(kernel), "\'"));
}

//------------------------------------------------------------------------------
// void ValidateKernel(const std::string &kName,
//                     const std::string label = "OrbitSpiceKernelName",
//                     const std::string ofType)
//------------------------------------------------------------------------------
/**
 * This method checks the input kernel for existence and proper type.
 *
 * @param kName  input kernel name
 * @param label  label for the field
 * @param ofType kernel type
 *
 */
//------------------------------------------------------------------------------
void SpacePoint::ValidateKernel(const std::string &kName, const std::string label,
                                const std::string ofType)
{
   #ifdef DEBUG_KERNEL_VALIDATE
   MessageInterface::ShowMessage("ValidateKernel with kName = \"%s\", and ofType = \"%s\"\n",
         kName.c_str(), ofType.c_str());
   MessageInterface::ShowMessage("   and label = %s\n", label.c_str());
   #endif
   
   std::string fullSpkName = kName;
   
   // User FileManager::FindPath() to use search order (LOJ: 2014.06.17)
   static bool writeWarning = true;
   std::string tempSpkName =
      FileManager::Instance()->FindPath(kName, FileManager::PLANETARY_EPHEM_SPK_PATH, true, writeWarning);
   
   // If file not found, try with VEHICLE_EPHEM_SPK_PATH (LOJ: 2016.10.04)
   if (tempSpkName == "")
   {
      tempSpkName =
      FileManager::Instance()->FindPath(kName, FileManager::VEHICLE_EPHEM_SPK_PATH, true, writeWarning);
   }
   writeWarning = false;
   
   #ifdef DEBUG_KERNEL_VALIDATE
   MessageInterface::ShowMessage("   tempSpkName = '%s'\n", tempSpkName.c_str());
   #endif
   
   if (tempSpkName == "")
   {
      GmatBaseException gbe;
      gbe.SetDetails(errorMessageFormat.c_str(),
                     fullSpkName.c_str(), label.c_str(), "File must exist");
      throw gbe;
   }
   else
      fullSpkName = tempSpkName;
   
   
   #ifdef DEBUG_KERNEL_VALIDATE
      MessageInterface::ShowMessage(" ................. kName = \"%s\"\n",
            fullSpkName.c_str());
   #endif

   //======================================================================
   // This section will be removed when file path testing completes
   //======================================================================
   #if 0
   //======================================================================
   if (!(GmatFileUtil::DoesFileExist(kName)))
   {
      // try again with path name from startup file
      if (GmatFileUtil::ParsePathName(kName) == "")
         fullSpkName = theSpkPath + fullSpkName;
      #ifdef DEBUG_KERNEL_VALIDATE
         MessageInterface::ShowMessage(" ................. AND NOW fullSpkName = \"%s\"\n",
               fullSpkName.c_str());
      #endif

      if (!(GmatFileUtil::DoesFileExist(fullSpkName)))
      {
         // Need to check with standard (slash) path separator here, as DoesFileExist
         // does not currently check, and back-slashes do not work on Mac
         std::string fName = GmatStringUtil::Replace(fullSpkName, "\\", "/");
         fullSpkName = fName;
         if (!(GmatFileUtil::DoesFileExist(fullSpkName)))
         {
            GmatBaseException gbe;
            gbe.SetDetails(errorMessageFormat.c_str(),
                  fullSpkName.c_str(), label.c_str(), "File must exist");
            throw gbe;
         }
      }
   }
   //======================================================================
   #endif
   //======================================================================
   
   #ifdef __USE_SPICE__
      if (!SpiceInterface::IsValidKernel(fullSpkName, ofType))
      {
         MessageInterface::ShowMessage("File is of wrong type!!!!\n"); // *****************
         GmatBaseException gbe;
         std::string valid = "Valid ";
         valid += GmatStringUtil::ToUpper(ofType) + " kernel";
         gbe.SetDetails(errorMessageFormat.c_str(),
               fullSpkName.c_str(), label.c_str(), valid.c_str());
         throw gbe;
      }
   #endif
}


//------------------------------------------------------------------------------
// bool isFromEphemeris()
//------------------------------------------------------------------------------
/**
 * This method checks to see if the spacecraft has an attached ephemeris file.
 */
 //------------------------------------------------------------------------------
bool SpacePoint::HasEphemerisFile()
{
	if (orbitSpiceKernelNames.size() > 0)
		return true;
	return false;
}
