//$Id$
//------------------------------------------------------------------------------
//                                  AxisSystem
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
// Created: 2004/12/20
//
/**
 * Implementation of the AxisSystem class.  This is the base class for the
 * InertialAxes and DynamicAxes classes.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "CoordinateBase.hpp"
#include "Rmatrix33.hpp"
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"
#include "Linear.hpp"
#include "GmatConstants.hpp"
#include "Rvector3.hpp"
#include "MessageInterface.hpp"
#include "CoordinateSystemException.hpp"
#include "SolarSystem.hpp"

#include <iostream>


using namespace GmatMathUtil;           // for trig functions, etc.
using namespace GmatMathConstants;      // for RAD_PER_ARCSEC, etc.
using namespace GmatTimeConstants;      // for SECS_PER_DAY

//static Integer visitCount = 0;


//#define DEBUG_ROT_MATRIX 1
//#define DEBUG_UPDATE
//#define DEBUG_FIRST_CALL
//#define DEBUG_a_MATRIX
//#define DEBUG_ITRF_UPDATES
//#define DEBUG_CALCS
//#define DEBUG_DESTRUCTION
//#define DEBUG_AXIS_SYSTEM_INIT
//#define DEBUG_AXIS_SYSTEM_EOP
//#define DEBUG_ALLOW_NO_RATES
//#define DEBUG_ICRF_TOFK5

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif
#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif

//---------------------------------
// static data
//---------------------------------

const Real AxisSystem::DETERMINANT_TOLERANCE = 1.0e-14;
const Real AxisSystem::JD_OF_JANUARY_1_1997  = 2450449.5;  // correct????


const std::string
AxisSystem::PARAMETER_TEXT[AxisSystemParamCount - CoordinateBaseParamCount] =
{
   "Epoch",
   "UpdateInterval",
   "OverrideOriginInterval",
};

const Gmat::ParameterType
AxisSystem::PARAMETER_TYPE[AxisSystemParamCount - CoordinateBaseParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::BOOLEAN_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  AxisSystem(const std::string &itsType,
//             const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base AxisSystem structures used in derived classes
 * (default constructor).
 *
 * @param itsType GMAT script string associated with this type of object.
 * @param itsName Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for AxisSystem.  Derived 
 *       classes must pass in the itsType and itsName parameters.
 */
//---------------------------------------------------------------------------
AxisSystem::AxisSystem(const std::string &itsType,
                       const std::string &itsName) :
CoordinateBase(Gmat::AXIS_SYSTEM,itsType,itsName),
coordName        (""),
usesPrimary      (GmatCoordinate::NOT_USED),
usesSecondary    (GmatCoordinate::NOT_USED),
baseSystem       ("FK5"),
eop              (NULL),
itrf             (NULL),
epochFormat      ("A1ModJulian"),
needsCBOrigin    (false),
allowNoRates     (false),
updateInterval   (60.0), 
updateIntervalToUse    (60.0), 
overrideOriginInterval (false),
lastDPsi         (0.0),
nutationSrc      (GmatItrf::NUTATION_1980),
planetarySrc     (GmatItrf::PLANETARY_1980),
aVals            (NULL), 
apVals           (NULL),
precData         (NULL),
nutData          (NULL),
stData           (NULL),
stDerivData      (NULL),
pmData           (NULL),
AVals            (NULL),
BVals            (NULL),
CVals            (NULL),
DVals            (NULL),
EVals            (NULL),
FVals            (NULL),
ApVals           (NULL),
BpVals           (NULL),
CpVals           (NULL),
DpVals           (NULL),
isInertial       (false)
{
   objectTypes.push_back(Gmat::AXIS_SYSTEM);
   objectTypeNames.push_back("AxisSystem");
   parameterCount = AxisSystemParamCount;
   rotMatrix    = Rmatrix33(false); // want zero matrix, not identity matrix
   rotDotMatrix = Rmatrix33(false); 
   epoch        = GmatTimeConstants::A1MJD_OF_J2000;
   lastPRECEpoch       = A1Mjd(0.0);
   lastNUTEpoch        = A1Mjd(0.0);
   lastSTDerivEpoch    = GmatTime(0.0);
   lastPMEpoch         = A1Mjd(0.0);
}

//---------------------------------------------------------------------------
//  AxisSystem(const AxisSystem &axisSys);
//---------------------------------------------------------------------------
/**
 * Constructs base AxisSystem structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param axisSys  AxisSystem instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
AxisSystem::AxisSystem(const AxisSystem &axisSys) :
CoordinateBase(axisSys),
rotMatrix         (axisSys.rotMatrix),
rotDotMatrix      (axisSys.rotDotMatrix),
epoch             (axisSys.epoch),
coordName         (axisSys.coordName),
usesPrimary       (axisSys.usesPrimary),
usesSecondary     (axisSys.usesSecondary),
baseSystem        (axisSys.baseSystem),
eop               (axisSys.eop),
itrf              (axisSys.itrf),
epochFormat       (axisSys.epochFormat),
needsCBOrigin     (axisSys.needsCBOrigin),
allowNoRates      (axisSys.allowNoRates),
updateInterval    (axisSys.updateInterval),
updateIntervalToUse    (axisSys.updateIntervalToUse),
overrideOriginInterval (axisSys.overrideOriginInterval),
lastDPsi          (0.0),
nutationSrc       (GmatItrf::NUTATION_1980),
planetarySrc      (GmatItrf::PLANETARY_1980),
aVals            (NULL), 
apVals           (NULL),
precData         (NULL),
nutData          (NULL),
stData           (NULL),
stDerivData      (NULL),
pmData           (NULL),
AVals            (NULL),
BVals            (NULL),
CVals            (NULL),
DVals            (NULL),
EVals            (NULL),
FVals            (NULL),
ApVals           (NULL),
BpVals           (NULL),
CpVals           (NULL),
DpVals           (NULL),
isInertial       (axisSys.isInertial)
{
   lastPRECEpoch    = A1Mjd(0.0);
   lastNUTEpoch     = A1Mjd(0.0);
   lastSTDerivEpoch = GmatTime(0.0);
   lastPMEpoch      = A1Mjd(0.0);
}

//---------------------------------------------------------------------------
//  AxisSystem& operator=(const AxisSystem &axisSys)
//---------------------------------------------------------------------------
/**
 * Assignment operator for AxisSystem structures.
 *
 * @param axisSys The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const AxisSystem& AxisSystem::operator=(const AxisSystem &axisSys)
{
   if (&axisSys == this)
      return *this;
   CoordinateBase::operator=(axisSys);
   rotMatrix         = axisSys.rotMatrix;
   rotDotMatrix      = axisSys.rotDotMatrix;
   epoch             = axisSys.epoch;
   coordName         = axisSys.coordName;
   usesPrimary       = axisSys.usesPrimary;
   usesSecondary     = axisSys.usesSecondary;
   baseSystem        = axisSys.baseSystem;
   eop               = axisSys.eop;
   itrf              = axisSys.itrf;
   epochFormat       = axisSys.epochFormat;
   needsCBOrigin     = axisSys.needsCBOrigin;
   allowNoRates      = axisSys.allowNoRates;
   updateInterval    = axisSys.updateInterval;
   updateIntervalToUse    = axisSys.updateIntervalToUse;
   overrideOriginInterval = axisSys.overrideOriginInterval;
   lastPRECEpoch     = axisSys.lastPRECEpoch;
   lastNUTEpoch      = axisSys.lastNUTEpoch;   
   lastSTDerivEpoch  = axisSys.lastSTDerivEpoch;
   lastPMEpoch       = axisSys.lastPMEpoch;
   lastPREC          = axisSys.lastPREC;
   lastNUT           = axisSys.lastNUT;
   lastSTDeriv       = axisSys.lastSTDeriv;
   lastPM            = axisSys.lastPM;
   lastDPsi          = axisSys.lastDPsi;
   nutationSrc       = axisSys.nutationSrc;
   planetarySrc      = axisSys.planetarySrc;
   
   aVals             = NULL; 
   apVals            = NULL;
   precData          = NULL;
   nutData           = NULL;
   stData            = NULL;
   stDerivData       = NULL;
   pmData            = NULL;
   AVals             = NULL;
   BVals             = NULL;
   CVals             = NULL;
   DVals             = NULL;
   EVals             = NULL;
   FVals             = NULL;
   ApVals            = NULL;
   BpVals            = NULL;
   CpVals            = NULL;
   DpVals            = NULL;
   
   isInertial        = axisSys.isInertial;

   Initialize();
   
   return *this;
}
//---------------------------------------------------------------------------
//  ~AxisSystem()
//---------------------------------------------------------------------------
/**
 * Destructor.  Destroys this AxisSystem object.
 */
//---------------------------------------------------------------------------
AxisSystem::~AxisSystem()
{
   #ifdef DEBUG_DESTRUCTION
   MessageInterface::ShowMessage("---> Entering AxisSystem destructor for %s\n",
   coordName.c_str());
   #endif
   
   if (aVals != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (aVals, "aVals", "AxisSystem::~AxisSystem()", "deleting aVals");
      #endif
      delete [] aVals;
   }
   if (apVals != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (apVals, "apVals", "AxisSystem::~AxisSystem()", "deleting apVals");
      #endif
      delete [] apVals;
   }
   
   #ifdef DEBUG_DESTRUCTION
   MessageInterface::ShowMessage("---> LEAVING AxisSystem destructor for %s\n",
   coordName.c_str());
   #endif
}

//---------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem) const
//---------------------------------------------------------------------------
/**
 * Returns enum value indicating whether or not this axis system uses the EOP file,
 * for the input base system.
 *
 * @param <forBaseSystem> input base system   <future>
 *
 * @return enum value indicating whether or not this axis system uses the EOP file
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage AxisSystem::UsesEopFile(const std::string &forBaseSystem) const
{
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesItrfFile() const
//---------------------------------------------------------------------------
/**
 * Returns enum value indicating whether or not this axis system uses the ITRF
 * coefficients file.
 *
 * @return enum value indicating whether or not this axis system uses the ITRF
 *        coefficients file
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage AxisSystem::UsesItrfFile() const
{
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesEpoch() const
//---------------------------------------------------------------------------
/**
 * Returns enum value indicating whether or not this axis system uses an epoch.
 *
 * @return enum value indicating whether or not this axis system uses an epoch
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage AxisSystem::UsesEpoch() const
{
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesPrimary() const
//---------------------------------------------------------------------------
/**
 * Returns enum value indicating whether or not this axis system uses a
 * primary object.
 *
 * @return enum value indicating whether or not this axis system uses a
 *         primary object
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage AxisSystem::UsesPrimary() const
{
//   return GmatCoordinate::NOT_USED;
   return usesPrimary;
}

//---------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesSecondary() const
//---------------------------------------------------------------------------
/**
 * Returns enum value indicating whether or not this axis system uses a
 * secondary object.
 *
 * @return enum value indicating whether or not this axis system uses a
 *         secondary object
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage AxisSystem::UsesSecondary() const
{
//   return GmatCoordinate::NOT_USED;
   return usesSecondary;
}

//---------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesReferenceObject() const
//---------------------------------------------------------------------------
/**
 * Returns enum value indicating whether or not this axis system uses a
 * reference object.
 *
 * @return enum value indicating whether or not this axis system uses a
 *         reference object
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage AxisSystem::UsesReferenceObject() const
{
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesXAxis() const
//---------------------------------------------------------------------------
/**
 * Returns enum value indicating whether or not this axis system uses an
 * X axis.
 *
 * @return enum value indicating whether or not this axis system uses an
 *         X axis
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage AxisSystem::UsesXAxis() const
{
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesYAxis() const
//---------------------------------------------------------------------------
/**
 * Returns enum value indicating whether or not this axis system uses a
 * Y axis.
 *
 * @return enum value indicating whether or not this axis system uses a
 *         Y axis
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage AxisSystem::UsesYAxis() const
{
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesZAxis() const
//---------------------------------------------------------------------------
/**
 * Returns enum value indicating whether or not this axis system uses a
 * Z axis.
 *
 * @return enum value indicating whether or not this axis system uses a
 *         Z axis
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage AxisSystem::UsesZAxis() const
{
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const
//---------------------------------------------------------------------------
/**
 * Returns enum value indicating whether or not this axis system uses a
 * nutation update interval.
 *
 * @return enum value indicating whether or not this axis system uses a
 *         nutation update interval
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage AxisSystem::UsesNutationUpdateInterval() const
{
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
//  bool UsesSpacecraft(const std::string &withName) const
//---------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not this axis system uses any
 * spacecraft as origin, primary, or secondary if withName == "", or
 * if it uses the specific spacecraft if the name is specified
 *
 * @return flag indicating whether or not this axis system uses the
 *         spacecraft as origin, primary, or secondary
 *
 * @note Child classes using a Reference Object will check in their
 * implementation of this method.
 */
//---------------------------------------------------------------------------
bool AxisSystem::UsesSpacecraft(const std::string &withName) const
{
   if (origin && origin->IsOfType("Spacecraft"))
   {
      if ((withName == "") || (origin->GetName() == withName))   return true;
   }
   if (usesPrimary)
   {
      SpacePoint *p = GetPrimaryObject();
      if (p && p->IsOfType("Spacecraft"))
      {
         if ((withName == "") || (p->GetName() == withName))   return true;
      }
   }
   if (usesSecondary)
   {
      SpacePoint *s = GetSecondaryObject();
      if (s && s->IsOfType("Spacecraft"))
      {
         if ((withName == "") || (s->GetName() == withName))   return true;
      }
   }
   // Child classes using a Reference Object will check in their
   // implementation of this method.
   return false;
}

//---------------------------------------------------------------------------
//  bool RequiresCelestialBodyOrigin() const
//---------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not this axis system requires a
 * celestial body origin
 *
 * @return flag indicating whether or not this axis system requires a
 *         celestial body as origin
 *
 * @note   derived classes must set needsCBOrigin to true if they do
 *         require an origin of type celestial body
 */
//---------------------------------------------------------------------------
bool AxisSystem::RequiresCelestialBodyOrigin() const
{
   return needsCBOrigin;
}


//---------------------------------------------------------------------------
//  bool HasCelestialBodyOrigin() const
//---------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not this axis system has a
 * celestial body origin
 *
 * @return flag indicating whether or not this axis system has a
 *         celestial body as origin
 */
//---------------------------------------------------------------------------
bool AxisSystem::HasCelestialBodyOrigin() const
{
   if (origin && origin->IsOfType("CelestialBody")) return true;
   return false;
}

void AxisSystem::SetAllowWithoutRates(bool allow)
{
   #ifdef DEBUG_ALLOW_NO_RATES
      MessageInterface::ShowMessage(
            "Setting allowNoRates for %s to %s\n",
            coordName.c_str(), (allow? "true" : "false"));
   #endif
   allowNoRates = allow;
}


bool AxisSystem::AllowWithoutRates() const
{
   return allowNoRates;
}


bool AxisSystem::IsInertial()
{
   return isInertial;
}

// methods to set parameters for the AxisSystems - AxisSystems that need these
// will need to override these implementations
//---------------------------------------------------------------------------
//  void SetPrimaryObject(SpacePoint *prim)
//---------------------------------------------------------------------------
/**
 * Sets the primary object to the input SpacePoint
 *
 * @param <prim> primary SpacePoint object
 *
 */
//---------------------------------------------------------------------------
void AxisSystem::SetPrimaryObject(SpacePoint *prim)
{
   // default behavior is to ignore this - should I throw an exception here??
}

//---------------------------------------------------------------------------
//  void SetSecondaryObject(SpacePoint *second)
//---------------------------------------------------------------------------
/**
 * Sets the secondary object to the input SpacePoint
 *
 * @param <second> secondary SpacePoint object
 *
 */
//---------------------------------------------------------------------------
void AxisSystem::SetSecondaryObject(SpacePoint *second)
{
   // default behavior is to ignore this
}

//---------------------------------------------------------------------------
//  void SetReferenceObject(SpacePoint *refObj)
//---------------------------------------------------------------------------
/**
 * Sets the reference object to the input SpacePoint
 *
 * @param <second> secondary SpacePoint object
 *
 */
//---------------------------------------------------------------------------
void AxisSystem::SetReferenceObject(SpacePoint *refObj)
{
   // default behavior is to ignore this
}



//---------------------------------------------------------------------------
//  void SetEpoch(const A1Mjd &toEpoch)
//---------------------------------------------------------------------------
/**
 * Sets the epoch for the AxisSystem class.
 *
 * @param <toEpoch> epoch value to use.
 */
//---------------------------------------------------------------------------
void AxisSystem::SetEpoch(const A1Mjd &toEpoch)
{
   epoch = toEpoch;
}

//---------------------------------------------------------------------------
//  void SetXAxis(const std::string &toValue)
//---------------------------------------------------------------------------
/**
 * Sets the X axis for the AxisSystem object.
 *
 * @param <toValue> X axis value to use.
 */
//---------------------------------------------------------------------------
void AxisSystem::SetXAxis(const std::string &toValue)
{
   // default behavior is to ignore this
}

//---------------------------------------------------------------------------
//  void SetYAxis(const std::string &toValue)
//---------------------------------------------------------------------------
/**
 * Sets the Y axis for the AxisSystem object.
 *
 * @param <toValue> Y axis value to use.
 */
//---------------------------------------------------------------------------
void AxisSystem::SetYAxis(const std::string &toValue)
{
   // default behavior is to ignore this
}

//---------------------------------------------------------------------------
//  void SetZAxis(const std::string &toValue)
//---------------------------------------------------------------------------
/**
 * Sets the Z axis for the AxisSystem object.
 *
 * @param <toValue> Z axis value to use.
 */
//---------------------------------------------------------------------------
void AxisSystem::SetZAxis(const std::string &toValue)
{
   // default behavior is to ignore this
}


//------------------------------------------------------------------------------
//  void SetEopFile(EopFile *eopF)
//------------------------------------------------------------------------------
/**
 * Method to set EopFile for this system.
 *
 * @param eopF  the eopFile.
 *
 */
//------------------------------------------------------------------------------
void AxisSystem::SetEopFile(EopFile *eopF)
{
   eop = eopF;
}

//------------------------------------------------------------------------------
//  void SetCoefficientsFile(ItrfCoefficientsFile *itrfF)
//------------------------------------------------------------------------------
/**
 * Method to set ITRF coefficients for this system.
 *
 * @param itrfF  the ITRF coefficients file.
 *
 */
//------------------------------------------------------------------------------
void AxisSystem::SetCoefficientsFile(ItrfCoefficientsFile *itrfF)
{
   itrf = itrfF;
}

//------------------------------------------------------------------------------
//  void SetEpochFormat(const std::string &fmt)
//------------------------------------------------------------------------------
/**
 * Method to set the epoch format for this system.
 *
 * @param <fmt>  the epoch format
 *
 */
//------------------------------------------------------------------------------
void AxisSystem::SetEpochFormat(const std::string &fmt)
{
   epochFormat = fmt;
}

//------------------------------------------------------------------------------
//  SpacePoint* GetPrimaryObject() const
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the primary SpacePoint object.
 *
 * @return  a pointer to the primary SpacePoint object
 *
 */
//------------------------------------------------------------------------------
SpacePoint* AxisSystem::GetPrimaryObject() const
{
   return NULL;
}

//------------------------------------------------------------------------------
//  SpacePoint* GetSecondaryObject() const
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the secondary SpacePoint object.
 *
 * @return  a pointer to the secondary SpacePoint object
 *
 */
//------------------------------------------------------------------------------
SpacePoint* AxisSystem::GetSecondaryObject() const
{
   return NULL;
}

//------------------------------------------------------------------------------
//  SpacePoint* GetReferenceObject() const
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the reference SpacePoint object.
 *
 * @return  a pointer to the reference SpacePoint object
 *
 */
//------------------------------------------------------------------------------
SpacePoint* AxisSystem::GetReferenceObject() const
{
   return NULL;
}

//------------------------------------------------------------------------------
// virtual std::string GetPrimaryObjectName() const
//------------------------------------------------------------------------------
std::string AxisSystem::GetPrimaryObjectName() const
{
   return "";
}

//------------------------------------------------------------------------------
// virtual std::string GetSecondaryObjectName() const
//------------------------------------------------------------------------------
std::string AxisSystem::GetSecondaryObjectName() const
{
   return "";
}

//------------------------------------------------------------------------------
// virtual std::string GetReferenceObjectName() const
//------------------------------------------------------------------------------
std::string AxisSystem::GetReferenceObjectName() const
{
   return "";
}

//---------------------------------------------------------------------------
//  A1Mjd GetEpoch() const
//---------------------------------------------------------------------------
/**
 * Returns the epoch of the AxisSystem class.
 *
 * @return epoch.
 *
 */
//---------------------------------------------------------------------------
A1Mjd AxisSystem::GetEpoch() const
{
   return epoch;
}


//---------------------------------------------------------------------------
//  std::string GetXAxis() const
//---------------------------------------------------------------------------
/**
 * Returns the X axis value of the AxisSystem class.
 *
 * @return X axis value.
 *
 */
//---------------------------------------------------------------------------
std::string AxisSystem::GetXAxis() const
{
   return "";
}

//---------------------------------------------------------------------------
//  std::string GetYAxis() const
//---------------------------------------------------------------------------
/**
 * Returns the Y axis value of the AxisSystem class.
 *
 * @return Y axis value.
 *
 */
//---------------------------------------------------------------------------
std::string AxisSystem::GetYAxis() const
{
   return "";
}

//---------------------------------------------------------------------------
//  std::string GetZAxis() const
//---------------------------------------------------------------------------
/**
 * Returns the Z axis value of the AxisSystem class.
 *
 * @return Z axis value.
 *
 */
//---------------------------------------------------------------------------
std::string AxisSystem::GetZAxis() const
{
   return "";
}

//---------------------------------------------------------------------------
//  EopFile* GetEopFile() const
//---------------------------------------------------------------------------
/**
 * Returns the pointer to the EOP file used by the AxisSystem class.
 *
 * @return pointer to EOP file
 *
 */
//---------------------------------------------------------------------------
EopFile* AxisSystem::GetEopFile() const
{
   return eop;
}

//---------------------------------------------------------------------------
//  std::string GetBaseSystem() const
//---------------------------------------------------------------------------
/**
 * Returns the base system used by the AxisSystem class.
 *
 * @return base system
 *
 */
//---------------------------------------------------------------------------
std::string AxisSystem::GetBaseSystem() const
{
   return baseSystem;
}

//---------------------------------------------------------------------------
//  ItrfCoefficientsFile* GetItrfCoefficientsFile() const
//---------------------------------------------------------------------------
/**
 * Returns the pointer to the ITRF coefficients file used by the AxisSystem class.
 *
 * @return pointer to ITRF coefficients file
 *
 */
//---------------------------------------------------------------------------
ItrfCoefficientsFile* AxisSystem::GetItrfCoefficientsFile()
{
   return itrf;
}

//---------------------------------------------------------------------------
//  std::string GetEpochFormat() const
//---------------------------------------------------------------------------
/**
 * Returns the epoch format used by the AxisSystem class.
 *
 * @return epoch format
 *
 */
//---------------------------------------------------------------------------
std::string AxisSystem::GetEpochFormat() const
{
   return epochFormat;
}

//---------------------------------------------------------------------------
//  Rmatrix33 GetLastRotationMatrix() const
//---------------------------------------------------------------------------
/**
 * Returns the last-computed rotation matrix.
 *
 * @return last rotation matrix
 *
 */
//---------------------------------------------------------------------------
Rmatrix33 AxisSystem::GetLastRotationMatrix() const
{
   return rotMatrix;
}

//---------------------------------------------------------------------------
//  Rmatrix33 GetLastRotationMatrix() const
//---------------------------------------------------------------------------
/**
 * Returns the last-computed rotation matrix.
 *
 * @param <mat> output last rotation matrix
 *
 * @return last rotation matrix
 *
 */
//---------------------------------------------------------------------------
void AxisSystem::GetLastRotationMatrix(Real *mat) const
{
   for (Integer i=0; i<9; i++)
      mat[i] = rotData[i];
   
}

//---------------------------------------------------------------------------
//  Rmatrix33 GetLastRotationDotMatrix() const
//---------------------------------------------------------------------------
/**
 * Returns the last-computed rotation dot matrix.
 *
 * @return last rotation dot matrix
 *
 */
//---------------------------------------------------------------------------
Rmatrix33 AxisSystem::GetLastRotationDotMatrix() const
{
   return rotDotMatrix;
}

//---------------------------------------------------------------------------
//  Rmatrix33 GetLastRotationDotMatrix() const
//---------------------------------------------------------------------------
/**
 * Returns the last-computed rotation dot matrix.
 *
 * @param <mat> last-computed rotation dot matrix
 *
 * @return last rotation dot matrix
 *
 */
//---------------------------------------------------------------------------
void AxisSystem::GetLastRotationDotMatrix(Real *mat) const
{
   for (Integer i=0; i<9; i++)
      mat[i] = rotDotData[i];
   
}


//---------------------------------------------------------------------------
//  std::vector<Rmatrix33> GetDerivativeOfLastRotationDotMatrix()
//---------------------------------------------------------------------------
/**
* Returns the last-computed derivetive of rotation matrix w.r.t. spacecraft state.
*
* @return derivative of last rotation matrix w.r.t. spacecraft state.
*
*/
//---------------------------------------------------------------------------
std::vector<Rmatrix33> AxisSystem::GetDerivativeOfLastRotationMatrix()
{
	rotMatrixDeriv.clear();
	Rmatrix33 Zero(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	for (Integer i = 0; i<6; i++)
		rotMatrixDeriv.push_back(Zero);

	return rotMatrixDeriv;
}



//---------------------------------------------------------------------------
//  void SetCoordinateSystemName(const std::string &csName)
//---------------------------------------------------------------------------
/**
 * Sets the coordinate system name.
 *
 * @param <csName> coordinate system name
 *
 */
//---------------------------------------------------------------------------
void AxisSystem::SetCoordinateSystemName(const std::string &csName)
{
   coordName    = csName;
}



//---------------------------------------------------------------------------
//  void AxisSystem::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this AxisSystem.
 *
 */
//---------------------------------------------------------------------------
bool AxisSystem::Initialize()
{
   CoordinateBase::Initialize();
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif
   
   rotData     = rotMatrix.GetDataVector();
   rotDotData  = rotDotMatrix.GetDataVector();
   precData    = PREC.GetDataVector();
   nutData     = NUT.GetDataVector();
   stData      = ST.GetDataVector();
   stDerivData = STderiv.GetDataVector();
   pmData      = PM.GetDataVector();
   
   // Make sure to initialize the origin, if necessary
   InitializeReference(origin);
   return true;
}

//------------------------------------------------------------------------------
//  bool RotateToBaseSystem(const A1Mjd &epoch, const Rvector &inState,
//                        Rvector &outState, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will rotate the input inState into the BaseSystem frame.
 *
 * @param epoch            the epoch at which to perform the rotation.
 * @param inState          the input state (in this AxisSystem) to be rotated.
 * @param outState         the output state, in the BaseSystem AxisSystem, the result
 *                         of rotating the input inState.
 * @param forceComputation flag to force the computation
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::RotateToBaseSystem(const A1Mjd &epoch, const Rvector &inState,
                                  Rvector &outState, 
                                  bool forceComputation)
{
   CalculateRotationMatrix(epoch, forceComputation);
   bool retval = CompleteRotateToBase(inState, outState);

   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == GmatTimeConstants::MJD_OF_J2000))
      {
         MessageInterface::ShowMessage(
            "AxisSystem::RotateToBaseSystem check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "   AxisSystem::input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         MessageInterface::ShowMessage(
            "   AxisSystem::output State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            outState[0], outState[1], outState[2], outState[4], outState[4], 
            outState[5]);
      }
   #endif

   return retval;
}

//------------------------------------------------------------------------------
//  bool RotateToBaseSystem(const GmatTime &epoch, const Rvector &inState,
//                        Rvector &outState, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will rotate the input inState into the BaseSystem frame.
 *
 * @param epoch            the epoch at which to perform the rotation.
 * @param inState          the input state (in this AxisSystem) to be rotated.
 * @param outState         the output state, in the BaseSystem AxisSystem, the result
 *                         of rotating the input inState.
 * @param forceComputation flag to force the computation
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::RotateToBaseSystem(const GmatTime &epoch, const Rvector &inState,
                                  Rvector &outState, 
                                  bool forceComputation)
{
   CalculateRotationMatrix(epoch, forceComputation);
   bool retval = CompleteRotateToBase(inState, outState);

   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch == GmatTimeConstants::MJD_OF_J2000))
      {
         MessageInterface::ShowMessage(
            "AxisSystem::RotateToBaseSystem check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.GetMjd());
         MessageInterface::ShowMessage(
            "   AxisSystem::input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         MessageInterface::ShowMessage(
            "   AxisSystem::output State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            outState[0], outState[1], outState[2], outState[4], outState[4], 
            outState[5]);
      }
   #endif

   return retval;
}

//------------------------------------------------------------------------------
//  bool RotateToBaseSystem(const A1Mjd &epoch, const Rvector &inState,
//                        Rvector &outState, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will rotate the input inState into the BaseSystem frame.
 *
 * @param epoch            the epoch at which to perform the rotation.
 * @param inState          the input state (in this AxisSystem) to be rotated.
 * @param outState         the output state, in the BaseSystem AxisSystem, the result
 *                         of rotating the input inState.
 * @param forceComputation flag to force the computation
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::RotateToBaseSystem(const A1Mjd &epoch, const Real *inState,
                                  Real *outState,
                                  bool forceComputation)
{
   CalculateRotationMatrix(epoch, forceComputation);
   bool retval = CompleteRotateToBase(inState, outState);

   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == GmatTimeConstants::MJD_OF_J2000))
      {
         MessageInterface::ShowMessage(
            "RotateToBaseSystem check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "   input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
      }
   #endif

   return retval;
}

//------------------------------------------------------------------------------
//  bool RotateToBaseSystem(const GmatTime &epoch, const Rvector &inState,
//                        Rvector &outState, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will rotate the input inState into the BaseSystem frame.
 *
 * @param epoch            the epoch at which to perform the rotation.
 * @param inState          the input state (in this AxisSystem) to be rotated.
 * @param outState         the output state, in the BaseSystem AxisSystem, the result
 *                         of rotating the input inState.
 * @param forceComputation flag to force the computation
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::RotateToBaseSystem(const GmatTime &epoch, const Real *inState,
                                  Real *outState,
                                  bool forceComputation)
{
   CalculateRotationMatrix(epoch, forceComputation);
   bool retval = CompleteRotateToBase(inState, outState);

   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch == GmatTimeConstants::MJD_OF_J2000))
      {
         MessageInterface::ShowMessage(
            "RotateToBaseSystem check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.GetMjd());
         MessageInterface::ShowMessage(
            "   input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
      }
   #endif

   return retval;
}

//------------------------------------------------------------------------------
//  bool RotateFromBaseSystem(const A1Mjd &epoch, const Rvector &inState,
//                          Rvector &outState, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will rotate the input inState from the BaseSystem frame into
 * this AxisSystem.
 *
 * @param epoch            the epoch at which to perform the rotation.
 * @param inState          the input state (in BaseSystem AxisSystem) to be rotated.
 * @param outState         the output state, in this AxisSystem, the result
 *                         of rotating the input inState.
 * @param forceComputation flag to force the computation
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::RotateFromBaseSystem(const A1Mjd &epoch,
                                    const Rvector &inState,
                                    Rvector &outState,
                                    bool forceComputation)
{
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("Entering AxisSystem::RotateFromBaseSystem on object of type %s\n",
            (GetTypeName()).c_str());
   #endif
   CalculateRotationMatrix(epoch, forceComputation);
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("In AxisSystem::rotateFromBaseSystem, DONE computing rotation matrix\n");
   #endif
   
   bool retval = CompleteRotateFromBase(inState, outState);

   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == GmatTimeConstants::MJD_OF_J2000))
      {
         MessageInterface::ShowMessage(
            "AxisSystem::RotateFromBaseSystem check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "   AxisSystem::input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         MessageInterface::ShowMessage(
            "   AxisSystem::output State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            outState[0], outState[1], outState[2], outState[3], outState[4], 
            outState[5]);
         firstCallFired = true;
      }
   #endif

   return retval;
}

//------------------------------------------------------------------------------
//  bool RotateFromBaseSystem(const GmatTime &epoch, const Rvector &inState,
//                          Rvector &outState, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will rotate the input inState from the BaseSystem frame into
 * this AxisSystem.
 *
 * @param epoch            the epoch at which to perform the rotation.
 * @param inState          the input state (in BaseSystem AxisSystem) to be rotated.
 * @param outState         the output state, in this AxisSystem, the result
 *                         of rotating the input inState.
 * @param forceComputation flag to force the computation
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::RotateFromBaseSystem(const GmatTime &epoch,
                                    const Rvector &inState,
                                    Rvector &outState,
                                    bool forceComputation)
{
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("Entering AxisSystem::RotateFromBaseSystem on object of type %s\n",
            (GetTypeName()).c_str());
   #endif

   CalculateRotationMatrix(epoch, forceComputation);
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("In AxisSystem::rotateFromBaseSystem, DONE computing rotation matrix\n");
   #endif
   
   bool retval = CompleteRotateFromBase(inState, outState);

   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch == GmatTimeConstants::MJD_OF_J2000))
      {
         MessageInterface::ShowMessage(
            "AxisSystem::RotateFromBaseSystem check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.GetMjd());
         MessageInterface::ShowMessage(
            "   AxisSystem::input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         MessageInterface::ShowMessage(
            "   AxisSystem::output State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            outState[0], outState[1], outState[2], outState[3], outState[4], 
            outState[5]);
         firstCallFired = true;
      }
   #endif

   return retval;
}

//------------------------------------------------------------------------------
//  bool RotateFromBaseSystem(const A1Mjd &epoch, const Rvector &inState,
//                          Rvector &outState, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will rotate the input inState from the BaseSystem frame into
 * this AxisSystem.
 *
 * @param epoch            the epoch at which to perform the rotation.
 * @param inState          the input state (in BaseSystem AxisSystem) to be rotated.
 * @param outState         the output state, in this AxisSystem, the result
 *                         of rotating the input inState.
 * @param forceComputation flag to force the computation
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::RotateFromBaseSystem(const A1Mjd &epoch,
                                      const Real *inState,
                                      Real *outState,
                                      bool forceComputation)
{
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("Entering AxisSystem::RotateFromBaseSystem (*) on object of type %s\n",
            (GetTypeName()).c_str());
   #endif

   CalculateRotationMatrix(epoch, forceComputation);
   bool retval = CompleteRotateFromBase(inState, outState);

   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == GmatTimeConstants::MJD_OF_J2000))
      {
         MessageInterface::ShowMessage(
            "AxisSystem::RotateFromBaseSystem check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "   input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         firstCallFired = true;
      }
   #endif

   return retval;
}

//------------------------------------------------------------------------------
//  bool RotateFromBaseSystem(const A1Mjd &GmatTime, const Rvector &inState,
//                          Rvector &outState, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will rotate the input inState from the BaseSystem frame into
 * this AxisSystem.
 *
 * @param epoch            the epoch at which to perform the rotation.
 * @param inState          the input state (in BaseSystem AxisSystem) to be rotated.
 * @param outState         the output state, in this AxisSystem, the result
 *                         of rotating the input inState.
 * @param forceComputation flag to force the computation
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::RotateFromBaseSystem(const GmatTime &epoch,
                                      const Real *inState,
                                      Real *outState,
                                      bool forceComputation)
{
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("Entering AxisSystem::RotateFromBaseSystem (*) on object of type %s\n",
            (GetTypeName()).c_str());
   #endif

   CalculateRotationMatrix(epoch, forceComputation);
   bool retval = CompleteRotateFromBase(inState, outState);

   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch == GmatTimeConstants::MJD_OF_J2000))
      {
         MessageInterface::ShowMessage(
            "AxisSystem::RotateFromBaseSystem check for %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   Rotation matrix = |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n"
            "                     |%20.10lf %20.10lf %20.10lf|\n",
            rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2),
            rotMatrix(1,0), rotMatrix(1,1), rotMatrix(1,2),
            rotMatrix(2,0), rotMatrix(2,1), rotMatrix(2,2));
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.GetMjd());
         MessageInterface::ShowMessage(
            "   input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         firstCallFired = true;
      }
   #endif

   return retval;
}

//------------------------------------------------------------------------------
//  bool CompleteRotateToBase(const Rvector &inState, Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method applies the rotation for RotateToBaseSystem.
 *
 * @param inState          the input state (in this AxisSystem) to be rotated.
 * @param outState         the output state, in the BaseSystem AxisSystem, the result
 *                         of rotating the input inState.
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::CompleteRotateToBase(const Rvector &inState, Rvector &outState)
{
   static Rvector3 tmpPosVecTo;
   static Rvector3 tmpVelVecTo;
   static const Real  *tmpPosTo = tmpPosVecTo.GetDataVector();
   static const Real  *tmpVelTo = tmpVelVecTo.GetDataVector();
   
   // *********** assuming only one 6-vector for now - UPDATE LATER!!!!!!
   tmpPosVecTo.Set(inState[0],inState[1], inState[2]);
   tmpVelVecTo.Set(inState[3],inState[4], inState[5]);
   
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Input vector to RotateToBaseSystem = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
      MessageInterface::ShowMessage(
      "the rotation matrix is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2),
      rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2),
      rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));
      MessageInterface::ShowMessage(
      "the rotation dot matrix is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotDotMatrix(0,0),rotDotMatrix(0,1),rotDotMatrix(0,2),
      rotDotMatrix(1,0),rotDotMatrix(1,1),rotDotMatrix(1,2),
      rotDotMatrix(2,0),rotDotMatrix(2,1),rotDotMatrix(2,2));
   #endif
   Real  outPos[3];
   Real  outVel[3];
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      outPos[p] = rotData[p3]   * tmpPosTo[0]   + 
                  rotData[p3+1] * tmpPosTo[1] + 
                  rotData[p3+2] * tmpPosTo[2];
      outVel[p] = (rotDotData[p3]    * tmpPosTo[0]   + 
                   rotDotData[p3+1]  * tmpPosTo[1] + 
                   rotDotData[p3+2]  * tmpPosTo[2])
                  +
                  (rotData[p3]    * tmpVelTo[0]   + 
                   rotData[p3+1]  * tmpVelTo[1] + 
                   rotData[p3+2]  * tmpVelTo[2]);
   }     
 
   outState.Set(6,outPos[0], outPos[1], outPos[2], 
                  outVel[0], outVel[1], outVel[2]);
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Computed Output vector in ToBaseSystem = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outPos[0], outPos[1], outPos[2], outVel[0], outVel[1], outVel[2]);
      MessageInterface::ShowMessage(
         "Output vector from ToBaseSystem = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outState[0], outState[1], outState[2], outState[3], outState[4], outState[5]);
   #endif

   return true;
}
//------------------------------------------------------------------------------
//  bool CompleteRotateToBase(const Rvector &inState, Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method applies the rotation for RotateToBaseSystem.
 *
 * @param inState          the input state (in this AxisSystem) to be rotated.
 * @param outState         the output state, in the BaseSystem AxisSystem, the result
 *                         of rotating the input inState.
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::CompleteRotateToBase(const Real *inState, Real *outState)
{
   Real pos[3] = {inState[0], inState[1], inState[2]};   
   Real vel[3] = {inState[3], inState[4], inState[5]};   
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Input vector to RotateToBaseSystem = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
      MessageInterface::ShowMessage(
      "the rotation matrix is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2),
      rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2),
      rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));
      MessageInterface::ShowMessage(
      "the rotation dot matrix is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotDotMatrix(0,0),rotDotMatrix(0,1),rotDotMatrix(0,2),
      rotDotMatrix(1,0),rotDotMatrix(1,1),rotDotMatrix(1,2),
      rotDotMatrix(2,0),rotDotMatrix(2,1),rotDotMatrix(2,2));
   #endif
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      outState[p] = rotData[p3]   * pos[0]   + 
                    rotData[p3+1] * pos[1] + 
                    rotData[p3+2] * pos[2];
      outState[p+3] = (rotDotData[p3]    * pos[0]   + 
                       rotDotData[p3+1]  * pos[1] + 
                       rotDotData[p3+2]  * pos[2])
                      +
                      (rotData[p3]    * vel[0]   + 
                       rotData[p3+1]  * vel[1] + 
                       rotData[p3+2]  * vel[2]);
   }    
   
   
 
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Output vector from ToBaseSystem = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outState[0], outState[1], outState[2], outState[3], outState[4], outState[5]);
   #endif

   return true;
}

//------------------------------------------------------------------------------
//  bool CompleteRotateFromBase(const Rvector &inState, Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method applies the rotation for RotateFromBaseSystem.
 *
 * @param inState          the input state (in BaseSystem AxisSystem) to be rotated.
 * @param outState         the output state, in this AxisSystem, the result
 *                         of rotating the input inState.
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::CompleteRotateFromBase(const Rvector &inState, Rvector &outState)
{
   static Rvector3 tmpPosVec;
   static Rvector3 tmpVelVec;
   static const Real  *tmpPos = tmpPosVec.GetDataVector();
   static const Real  *tmpVel = tmpVelVec.GetDataVector();
   
   // *********** assuming only one 6-vector for now - UPDATE LATER!!!!!!
   tmpPosVec.Set(inState[0],inState[1], inState[2]);
   tmpVelVec.Set(inState[3],inState[4], inState[5]);

   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Input vector to AxisSystem = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
      MessageInterface::ShowMessage(
      "the rotation matrix is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2),
      rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2),
      rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));
   #endif
   Real  outPos[3];
   Real  outVel[3];
   const Real  rotDataT[9] = {rotData[0], rotData[3], rotData[6],
                              rotData[1], rotData[4], rotData[7],
                              rotData[2], rotData[5], rotData[8]};
   const Real  rotDotDataT[9] = {rotDotData[0], rotDotData[3], rotDotData[6],
                                 rotDotData[1], rotDotData[4], rotDotData[7],
                                 rotDotData[2], rotDotData[5], rotDotData[8]};
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      outPos[p] = rotDataT[p3]   * tmpPos[0]   + 
                  rotDataT[p3+1] * tmpPos[1] + 
                  rotDataT[p3+2] * tmpPos[2];
      outVel[p] = (rotDotDataT[p3]    * tmpPos[0]   + 
                   rotDotDataT[p3+1]  * tmpPos[1] + 
                   rotDotDataT[p3+2]  * tmpPos[2])
                  +
                  (rotDataT[p3]    * tmpVel[0]   + 
                   rotDataT[p3+1]  * tmpVel[1] + 
                   rotDataT[p3+2]  * tmpVel[2]);
   }     
   outState.Set(6,outPos[0], outPos[1], outPos[2], 
                  outVel[0], outVel[1], outVel[2]);

   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Computed Output vector in FromBaseSystem = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outPos[0], outPos[1], outPos[2], outVel[0], outVel[1], outVel[2]);
      MessageInterface::ShowMessage(
         "Output vector from FromBaseSystem = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outState[0], outState[1], outState[2], outState[3], outState[4], outState[5]);
   #endif

   return true;
}

//------------------------------------------------------------------------------
//  bool CompleteRotateFromBase(const Rvector &inState, Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method applies the rotation for RotateFromBaseSystem.
 *
 * @param inState          the input state (in BaseSystem AxisSystem) to be rotated.
 * @param outState         the output state, in this AxisSystem, the result
 *                         of rotating the input inState.
 *
 * @return success or failure of the operation.
 */
//------------------------------------------------------------------------------
bool AxisSystem::CompleteRotateFromBase(const Real *inState, Real *outState)
{
   Real pos[3] = {inState[0], inState[1], inState[2]};   
   Real vel[3] = {inState[3], inState[4], inState[5]};   
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Input vector to AxisSystem::FromBaseSystem = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
      MessageInterface::ShowMessage(
      "the rotation matrix is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2),
      rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2),
      rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));
      MessageInterface::ShowMessage(
      "the rotation matrix (as array) is : %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f %.17f\n",
      rotData[0],rotData[1],rotData[2],rotData[3],rotData[4],
      rotData[5],rotData[6],rotData[7],rotData[8]);
   #endif
   const Real  rotDataT[9] = {rotData[0], rotData[3], rotData[6],
                              rotData[1], rotData[4], rotData[7],
                              rotData[2], rotData[5], rotData[8]};
   const Real  rotDotDataT[9] = {rotDotData[0], rotDotData[3], rotDotData[6],
                                 rotDotData[1], rotDotData[4], rotDotData[7],
                                 rotDotData[2], rotDotData[5], rotDotData[8]};
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      outState[p]   = rotDataT[p3]   * pos[0]   + 
                      rotDataT[p3+1] * pos[1] + 
                      rotDataT[p3+2] * pos[2];
      outState[p+3] = (rotDotDataT[p3]    * pos[0]   + 
                       rotDotDataT[p3+1]  * pos[1] + 
                       rotDotDataT[p3+2]  * pos[2])
                      +
                      (rotDataT[p3]    * vel[0]   + 
                       rotDataT[p3+1]  * vel[1] + 
                       rotDataT[p3+2]  * vel[2]);
   }     
   #ifdef DEBUG_CALCS
      MessageInterface::ShowMessage(
         "Output vector from AxisSystem::FromBaseSystem = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
         outState[0], outState[1], outState[2], outState[3], outState[4], outState[5]);
   #endif

   return true;   
}

void AxisSystem::CalculateRotationMatrix(const GmatTime &atEpoch,
                                         bool forceComputation)
{
   CalculateRotationMatrix(A1Mjd(atEpoch.GetMjd()), forceComputation);
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
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string AxisSystem::GetParameterText(const Integer id) const
{
   if (id >= CoordinateBaseParamCount && id < AxisSystemParamCount)
      return PARAMETER_TEXT[id - CoordinateBaseParamCount];
   return CoordinateBase::GetParameterText(id);
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
 *
 */
//------------------------------------------------------------------------------
Integer AxisSystem::GetParameterID(const std::string &str) const
{
   for (Integer i = CoordinateBaseParamCount; i < AxisSystemParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - CoordinateBaseParamCount])
         return i;
   }
   
   return CoordinateBase::GetParameterID(str);
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
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType AxisSystem::GetParameterType(const Integer id) const
{
   if (id >= CoordinateBaseParamCount && id < AxisSystemParamCount)
      return PARAMETER_TYPE[id - CoordinateBaseParamCount];
   
   return CoordinateBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string AxisSystem::GetParameterTypeString(const Integer id) const
{
   return CoordinateBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool AxisSystem::IsParameterReadOnly(const Integer id) const
{
   if ((id == ORIGIN_NAME) || (id == J2000_BODY_NAME) ||
       // The next 2 repeat settings passed in from the CS
       (id == UPDATE_INTERVAL) || (id == OVERRIDE_ORIGIN_INTERVAL))
      return true;
   // Only access epoch if it is used
   if ((id == EPOCH) && (UsesEpoch() == GmatCoordinate::NOT_USED))
      return true;
   return CoordinateBase::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the real value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real AxisSystem::GetRealParameter(const Integer id) const
{
   if (id == EPOCH)           return epoch.Get(); 
   if (id == UPDATE_INTERVAL) return updateInterval;
   return CoordinateBase::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the real value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 * @param value to use to set the parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real AxisSystem::SetRealParameter(const Integer id, const Real value)
{
   if (id == EPOCH && (UsesEpoch() != GmatCoordinate::NOT_USED))
   {
      epoch.Set(value);
      return true;
   }
   if (id == UPDATE_INTERVAL)
   {
      /// @todo - when this is removed from the system, remember to remove the
      /// UsesNutationUpdateInterval method everywhere in base/coordsys and
      /// gui/cordSys as well
      static bool writeIgnoredMessage = true;
      if (writeIgnoredMessage)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"UpdateInterval\" on AxisSystems is ignored and will be "
             "removed from a future build\n");
         writeIgnoredMessage = false;
      }
//      updateInterval = value;
      return true;
   }
   return CoordinateBase::SetRealParameter(id,value);
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the real value, given the input parameter label.
 *
 * @param label label for the requested parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real AxisSystem::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const std::string &label, const Real value) 
//------------------------------------------------------------------------------
/**
 * This method sets the real value, given the input parameter label.
 *
 * @param label label for the requested parameter.
 * @param value to use to set the parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real AxisSystem::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool  GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the boolean value, given the input parameter id.
 *
 * @param <id> id for the requested parameter.
 *
 * @return bool value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
bool AxisSystem::GetBooleanParameter(const Integer id) const
{
   if (id == OVERRIDE_ORIGIN_INTERVAL) return overrideOriginInterval;
   return CoordinateBase::GetBooleanParameter(id); 
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the boolean value, given the input parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return bool value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
bool AxisSystem::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  bool  SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> bool value for the requested parameter.
 *
 * @return  the boolean set.
 *
 */
 //------------------------------------------------------------------------------
bool AxisSystem::SetBooleanParameter(const Integer id,
                                     const bool value)
{
   if (id == OVERRIDE_ORIGIN_INTERVAL)
   {
      static bool writeIgnoredMessage = true;
      if (writeIgnoredMessage)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"OverrideOriginInterval\" on AxisSystems is ignored and will be "
             "removed from a future build\n");
         writeIgnoredMessage = false;
      }
//      overrideOriginInterval = value;
      return overrideOriginInterval;
   }
   return CoordinateBase::SetBooleanParameter(id, value);
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets the boolean value for the input parameter label.
 *
 * @param <label> id for the requested parameter.
 * @param <value> boolean value to use for the specified parameter
 *
 * @return true if set successfully; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool AxisSystem::SetBooleanParameter(const std::string &label,
                                     const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
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
const ObjectTypeArray& AxisSystem::GetRefObjectTypeArray()
{
   // Default is none
   refObjectTypes.clear();
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// void InitializeFK5()
//------------------------------------------------------------------------------
/**
 * This method initializes the FK5 computations.
 *
 */
//------------------------------------------------------------------------------
void AxisSystem::InitializeFK5()
{
   #ifdef DEBUG_AXIS_SYSTEM_INIT
      MessageInterface::ShowMessage("Entering InitializeFK5 for coordinate system %s of type %s\n",
            (GetName()).c_str(), (GetTypeName()).c_str());
   #endif
   if (eop == NULL)
      throw CoordinateSystemException(
            "EOP file has not been set for " + coordName);
   if (itrf == NULL)
      throw CoordinateSystemException(
            "Coefficient file has not been set for " + coordName);

   nutationSrc    = itrf->GetNutationTermsSource();
   planetarySrc   = itrf->GetPlanetaryTermsSource();
   Integer numNut = itrf->GetNumberOfNutationTerms();
   A.SetSize(numNut);   A.MakeZeroVector();
   #ifdef DEBUG_ITRF_UPDATES
   MessageInterface::ShowMessage("In Axis System, nutation source is %d\n",
                     (Integer) nutationSrc);
   #endif
   B.SetSize(numNut);   B.MakeZeroVector();
   C.SetSize(numNut);   C.MakeZeroVector();
   D.SetSize(numNut);   D.MakeZeroVector();
   E.SetSize(numNut);   E.MakeZeroVector();
   F.SetSize(numNut);   F.MakeZeroVector();

   Integer numPlan = itrf->GetNumberOfPlanetaryTerms();
   Ap.SetSize(numPlan);   Ap.MakeZeroVector();
   Bp.SetSize(numPlan);   Bp.MakeZeroVector();
   Cp.SetSize(numPlan);   Cp.MakeZeroVector();
   Dp.SetSize(numPlan);   Dp.MakeZeroVector();

   bool OK = itrf->GetNutationTerms(a, A, B, C, D, E, F);
   if (!OK) throw CoordinateSystemException("Error getting nutation data.");
   #ifdef DEBUG_ITRF_UPDATES
   for (Integer ii = 0; ii < A.GetSize(); ii++)
      MessageInterface::ShowMessage("In AxisSystem, after getting nutation data, A(%d) = %f\n", ii, A(ii));
   #endif

   if (aVals)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (aVals, "aVals", "AxisSystem::InitializeFK5()", "deleting aVals");
      #endif
      delete [] aVals;
   }

   aVals = new Integer[numNut * 5];
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (aVals, "aVals", "AxisSystem::InitializeFK5()", "aVals = new Integer[numNut * 5]");
   #endif

   for (Integer i = 0; i < 5; i++)
   {
      for (Integer j=0; j< numNut; j++)
      {
         aVals[i*numNut+j] = (a.at(i)).at(j);
      }
   }

   #ifdef DEBUG_a_MATRIX
      MessageInterface::ShowMessage("aVals = \n");
      for (Integer q = 0; q < numNut; ++q)
         MessageInterface::ShowMessage("         %3d: %8d %8d %8d %8d %8d\n",
            q+1, aVals[q], aVals[q+numNut], aVals[q+2*numNut],
            aVals[q+3*numNut], aVals[q+4*numNut]);
   #endif

   if (nutationSrc == GmatItrf::NUTATION_1996)
   {
      OK      = itrf->GetPlanetaryTerms(ap, Ap, Bp, Cp, Dp);
      if (!OK) throw CoordinateSystemException("Error getting planetary data.");

      if (apVals)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (apVals, "apVals", "AxisSystem::InitializeFK5()", "deleting apVals");
         #endif
         delete [] apVals;
      }
      
      apVals = new Integer[numPlan*10];
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (apVals, "apVals", "AxisSystem::InitializeFK5()", "apVals = new Integer[numPlan*10]");
      #endif
      
      for (Integer i = 0; i < 10; i++)
      {
         for (Integer j=0; j< numPlan; j++)
         {
            apVals[i*numPlan+j] = (ap.at(i)).at(j);
         }
      }

      #ifdef DEBUG_Ap_MATRIX
          MessageInterface::ShowMessage("apVals = \n");
         for (Integer q = 0; q < numPlan; ++q)
             MessageInterface::ShowMessage(
               "         %3d: %8d %8d %8d %8d %8d   %8d %8d %8d %8d %8d\n",
               q+1, apVals[q], apVals[q+numPlan], apVals[q+2*numPlan],
               apVals[q+3*numPlan], apVals[q+4*numPlan], apVals[q+5*numPlan],
               apVals[q+6*numPlan], apVals[q+7*numPlan], apVals[q+8*numPlan],
               apVals[q+9*numPlan]);
      #endif
    }

   AVals = A.GetDataVector();
   #ifdef DEBUG_ITRF_UPDATES
   for (Integer ii = 0; ii < A.GetSize(); ii++)
      MessageInterface::ShowMessage("AVals(%d) = %f\n", ii, AVals[ii]);
   #endif
   BVals = B.GetDataVector();
   CVals = C.GetDataVector();
   DVals = D.GetDataVector();
   EVals = E.GetDataVector();
   FVals = F.GetDataVector();
   ApVals = Ap.GetDataVector();
   BpVals = Bp.GetDataVector();
   CpVals = Cp.GetDataVector();
         DpVals = Dp.GetDataVector();
}   

//------------------------------------------------------------------------------
//  void ComputePrecessionMatrix(const Real tTDB, A1Mjd atEpoch)
//------------------------------------------------------------------------------
/**
 * This method will compute the Precession rotation matrix.
 *
 * @param tTDB     TDB time
 * @param atEpoch  epoch at which to compute the rotation matrix
 *
 * @note Code (for Earth) adapted from C code written by Steve Queen/ GSFC, 
 *       based on Vallado, pgs. 211- 227.  Equation references in parentheses
 *       will refer to those of the Vallado book.
 */
//------------------------------------------------------------------------------
void AxisSystem::ComputePrecessionMatrix(const Real tTDB, A1Mjd atEpoch)
{

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   AxisSystem::ComputePrecessionMatrix(%.15lf, %.15lf)\n", tTDB, 
            atEpoch.Get());
   #endif
   
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("**** tTDB = %.15lf\n",  tTDB);
   #endif
   Real tTDB2   = tTDB  * tTDB;
   Real tTDB3   = tTDB2 * tTDB;

   // Compute precession (Vallado, Eq. 3-56)
   Real zeta  = ( 2306.2181*tTDB + 0.30188*tTDB2 + 0.017998*tTDB3 )
      *RAD_PER_ARCSEC;
   Real Theta = ( 2004.3109*tTDB - 0.42665*tTDB2 - 0.041833*tTDB3 )
      *RAD_PER_ARCSEC;
   Real     z = ( 2306.2181*tTDB + 1.09468*tTDB2 + 0.018203*tTDB3 )
      *RAD_PER_ARCSEC;
   
   // Compute trigonometric quantities
   Real cosTheta = cos(Theta);
   Real cosz     = cos(z);
   Real coszeta  = cos(zeta);
   Real sinTheta = sin(Theta);
   Real sinz     = sin(z);
   Real sinzeta  = sin(zeta);
   
   // Compute Rotation matrix for transformations from FK5 to MOD
   // (Vallado Eq. 3-57)
   PREC.Set( cosTheta*cosz*coszeta - sinz*sinzeta,
            -sinzeta*cosTheta*cosz - sinz*coszeta,
            -sinTheta*cosz,
             sinz*cosTheta*coszeta + sinzeta*cosz,
            -sinz*sinzeta*cosTheta + cosz*coszeta,
            -sinTheta*sinz,
             sinTheta*coszeta,
            -sinTheta*sinzeta,
             cosTheta);
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("PREC = %s\n", (PREC.ToString()).c_str());
   #endif
   lastPREC = PREC;
   lastPRECEpoch = atEpoch;
}

//------------------------------------------------------------------------------
//  void ComputeNutationMatrix(const Real tTDB, A1Mjd atEpoch,
//                             Real &dPsi,
//                             Real &longAscNodeLunar,
//                             Real &cosEpsbar,
//                             bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will compute the Nutation rotation matrix.
 *
 * @param tTDB              TDB time
 * @param forEpoch          epoch at which to compute the rotation matrix
 * @param longAscNodeLunar  lunar longitude of the ascending node
 * @param cosEpsbar         computed quantity
 * @param forceComputation  force matrix computation?
 *
 */
//------------------------------------------------------------------------------
void AxisSystem::ComputeNutationMatrix(const Real tTDB, A1Mjd atEpoch,
                                            Real &dPsi,
                                            Real &longAscNodeLunar,
                                            Real &cosEpsbar,
                                            bool forceComputation)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
      {
         MessageInterface::ShowMessage("firstCallFired set to TRUE!!!!\n");
         MessageInterface::ShowMessage(
            "   AxisSystem::ComputeNutationMatrix(%.12lf, %.12lf, %.12lf, "
            "%.12lf, %.12lf)\n", tTDB, atEpoch.Get(), dPsi, longAscNodeLunar, 
            cosEpsbar);
      }
      else
         MessageInterface::ShowMessage("firstCallFired set to TRUE!!!!\n");
   #endif
   
   // Updated coefficients for GMT-4295.  Vallado's text is incorrect 
   // and updated based on Supplement to the Astronomical Almanac. - TDN 
   Real const125, const134, const357, const93, const297;
   if (nutationSrc == GmatItrf::NUTATION_1980)
   {
      const125 = 125.04452222*RAD_PER_DEG;
      const134 = 134.96298139*RAD_PER_DEG;
      const357 = 357.52772333*RAD_PER_DEG;
      const93  =  93.27191028*RAD_PER_DEG;
      const297 = 297.85036306*RAD_PER_DEG;
   }
   else if (nutationSrc == GmatItrf::NUTATION_1996)
   {
      const125 = 125.04455501*RAD_PER_DEG;
      const134 = 134.96340251*RAD_PER_DEG;
      const357 = 357.52910918*RAD_PER_DEG;
      const93  =  93.27209062*RAD_PER_DEG;
      const297 = 297.85019547*RAD_PER_DEG;
   }
   else
   {
      throw CoordinateSystemException("Error: code to calculate nutation matrix for the current nutation source is not implemented\n"); 
   }


   #ifdef DEBUG_UPDATE
      MessageInterface::ShowMessage("consts computed ... \n");
      MessageInterface::ShowMessage("  const125 = %12.10f\n", const125);
      MessageInterface::ShowMessage("  const134 = %12.10f\n", const134);
      MessageInterface::ShowMessage("  const357 = %12.10f\n", const357);
      MessageInterface::ShowMessage("  const93  = %12.10f\n", const93);
      MessageInterface::ShowMessage("  const297 = %12.10f\n", const297);
   #endif

   Real tTDB2   = tTDB  * tTDB;
   Real tTDB3   = tTDB2 * tTDB;
   Real tTDB4   = tTDB3 * tTDB;
   // Compute nutation - NOTE: this algorithm is based on the IERS 1996
   // Theory of Precession and Nutation. This can also be used with the 
   // 1980 Theory - the E and F terms are zero, and will fall out on the 
   // floor, roll around and under the door, down the stairs and into
   // the neighbor's yard.  It can also be used, but is not tested, 
   // with the 2000 Theory.
   #ifdef DEBUG_UPDATE
      if (nutationSrc == GmatItrf::NUTATION_1980)
         MessageInterface::ShowMessage("NUTATION_1980\n");
      else if (nutationSrc == GmatItrf::NUTATION_1996)
         MessageInterface::ShowMessage("NUTATION_1996\n");
      else
         MessageInterface::ShowMessage("NUTAION other\n");

      MessageInterface::ShowMessage("registers set up\n");
      MessageInterface::ShowMessage("  tTDB  = %.15lf\n", tTDB);
      MessageInterface::ShowMessage("  tTDB2 = %.15lf\n", tTDB2);
      MessageInterface::ShowMessage("  tTDB3 = %.15lf\n", tTDB3);
      MessageInterface::ShowMessage("  tTDB4 = %.15lf\n", tTDB4);
   #endif

   // Compute values to be passed out first ... 
   // Updated coefficients for GMT-4295.  Vallado's text is incorrect 
   // and updated based on Supplement to the Astronomical Almanac. - TDN 
   if (nutationSrc == GmatItrf::NUTATION_1980)
      longAscNodeLunar  = const125 + (  -6962890.5390*tTDB
                       + 7.455*tTDB2 + 0.008*tTDB3)
                       * RAD_PER_ARCSEC;
   else if (nutationSrc == GmatItrf::NUTATION_1996)
      longAscNodeLunar  = const125 + (  -6962890.2665*tTDB
                       + 7.4722*tTDB2 + 0.007702*tTDB3 - 0.00005939*tTDB4)
                       * RAD_PER_ARCSEC;

   longAscNodeLunar = longAscNodeLunar - ((int)(longAscNodeLunar/(2*GmatMathConstants::PI)))*2*GmatMathConstants::PI;	
   
   Real Epsbar       = (84381.448 - 46.8150*tTDB - 0.00059*tTDB2 
                        + 0.001813*tTDB3) * RAD_PER_ARCSEC;
   cosEpsbar         = cos(Epsbar);
   
   // if not enough time has passed, just return the last value
   Real dt = fabs(atEpoch.Subtract(lastNUTEpoch)) * SECS_PER_DAY;
   #ifdef DEBUG_UPDATE
      MessageInterface::ShowMessage("ENTERED ComputeNutation .....\n");
      MessageInterface::ShowMessage("  longAscNodeLunar = %.15lf\n", longAscNodeLunar);
      MessageInterface::ShowMessage("  Epsbar = %.15lf\n", Epsbar);
      MessageInterface::ShowMessage("  cosEpsbar = %.15lf\n", cosEpsbar);
   #endif
   if (( dt < updateIntervalToUse) && (!forceComputation))
   {
      #ifdef DEBUG_UPDATE
         MessageInterface::ShowMessage(">>> In ComputeNutationMatrix, using previously saved values ......\n");
      #endif
      dPsi = lastDPsi;

      #ifdef DEBUG_FIRST_CALL
         if (!firstCallFired)
            MessageInterface::ShowMessage(
               "   Using buffered nutation data: %.13lf = %.12f*(%.12lf - %.12lf)\n",
               dt, SECS_PER_DAY, atEpoch.Get(), lastNUTEpoch.Get());
      #endif
      
      return;
   }

   #ifdef DEBUG_UPDATE
      MessageInterface::ShowMessage("----> Computing NEW NUT matrix at time %12.10f\n",
         atEpoch.Get());
   #endif
   // otherwise, need to recompute all the nutation data
   dPsi      = 0.0;
   Real dEps = 0.0;
   // First, compute useful angles (Vallado Eq. 3-54)
   // NOTE - taken from Steve Queen's code - he has apparently converted
   // the values in degrees (from Vallado Eq. 3-54) to arcsec before
   // performing these computations

   Real meanAnomalyMoon, meanAnomalySun, argLatitudeMoon, meanElongationSun;
   if (nutationSrc == GmatItrf::NUTATION_1980)
   {
      meanAnomalyMoon   = const134 + (1717915922.6330*tTDB 
        + 31.310*tTDB2 + 0.064*tTDB3)*RAD_PER_ARCSEC;
      meanAnomalySun    = const357 + ( 129596581.2240*tTDB 
        -  0.577*tTDB2 - 0.012*tTDB3)*RAD_PER_ARCSEC;
      argLatitudeMoon   =  const93 + (1739527263.1370*tTDB 
        - 13.257*tTDB2 + 0.011*tTDB3)*RAD_PER_ARCSEC;
      meanElongationSun = const297 + (1602961601.3280*tTDB 
        -  6.891*tTDB2 + 0.019*tTDB3)*RAD_PER_ARCSEC;
   }
   else if (nutationSrc == GmatItrf::NUTATION_1996)
   {
      meanAnomalyMoon   = const134 + (1717915923.2178*tTDB 
        + 31.8792*tTDB2 + 0.051635*tTDB3 - 0.00024470*tTDB4)*RAD_PER_ARCSEC;
      meanAnomalySun    = const357 + ( 129596581.0481*tTDB 
        -  0.5532*tTDB2 - 0.000136*tTDB3 - 0.00001149*tTDB4)*RAD_PER_ARCSEC;
      argLatitudeMoon   =  const93 + (1739527262.8478*tTDB 
        - 12.7512*tTDB2 + 0.001037*tTDB3 + 0.00000417*tTDB4)*RAD_PER_ARCSEC;
      meanElongationSun = const297 + (1602961601.2090*tTDB 
        -  6.3706*tTDB2 + 0.006593*tTDB3 - 0.00003169*tTDB4)*RAD_PER_ARCSEC;
   }
   // Apply equations used in Matlab prototype - TDN
   meanAnomalyMoon = meanAnomalyMoon - ((int)(meanAnomalyMoon/(2*GmatMathConstants::PI)))*2*GmatMathConstants::PI; 
   meanAnomalySun = meanAnomalySun - ((int)(meanAnomalySun/(2*GmatMathConstants::PI)))*2*GmatMathConstants::PI; 
   argLatitudeMoon = argLatitudeMoon - ((int)(argLatitudeMoon/(2*GmatMathConstants::PI)))*2*GmatMathConstants::PI; 
   meanElongationSun = meanElongationSun - ((int)(meanElongationSun/(2*GmatMathConstants::PI)))*2*GmatMathConstants::PI; 

   // Now, sum using nutation coefficients  (Vallado Eq. 3-60)
   Integer i  = 0;
   Real apNut = 0.0;
   Real cosAp = 0.0;
   Real sinAp = 0.0;
   Integer nut = itrf->GetNumberOfNutationTerms();
   #ifdef DEBUG_UPDATE
      MessageInterface::ShowMessage(">>> After call to ITRF object ......\n");
      MessageInterface::ShowMessage("   and nut = %d\n", nut);
   #endif

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   Nutation data:\n"
            "      nut               = %d\n"
            "      meanAnomalyMoon   = %.13lf\n"
            "      meanAnomalySun    = %.13lf\n"
            "      argLatitudeMoon   = %.13lf\n"
            "      meanElongationSun = %.13lf\n"
            "      longAscNodeLunar  = %.13lf\n"
            "      cosEpsbar         = %.13lf\n"
            "      tTDB              = %.13le\n"
            "      tTDB2             = %.13le\n"
            "      tTDB3             = %.13le\n"
            "      tTDB4             = %.13le\n",
            nut, meanAnomalyMoon, meanAnomalySun, argLatitudeMoon,
            meanElongationSun, longAscNodeLunar, cosEpsbar, tTDB, tTDB2, tTDB3,
            tTDB4);

      if (!firstCallFired) 
      {
         MessageInterface::ShowMessage(
            "      a[0](1st 12 els)   = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[0], aVals[1], aVals[2], aVals[3], aVals[4], aVals[5], 
            aVals[6], aVals[7], aVals[8], aVals[9], aVals[10], aVals[11]);
         MessageInterface::ShowMessage(
            "      a[1](1st 12 els)   = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*1+0], aVals[nut*1+1], aVals[nut*1+2], aVals[nut*1+3], 
            aVals[nut*1+4], aVals[nut*1+5], aVals[nut*1+6], aVals[nut*1+7], 
            aVals[nut*1+8], aVals[nut*1+9], aVals[nut*1+10], aVals[nut*1+11]);
         MessageInterface::ShowMessage(
            "      a[2](1st 12 els)   = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*2+0], aVals[nut*2+1], aVals[nut*2+2], aVals[nut*2+3], 
            aVals[nut*2+4], aVals[nut*2+5], aVals[nut*2+6], aVals[nut*2+7], 
            aVals[nut*2+8], aVals[nut*2+9], aVals[nut*2+10], aVals[nut*2+11]);
         MessageInterface::ShowMessage(
            "      a[3](1st 12 els)   = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*3+0], aVals[nut*3+1], aVals[nut*3+2], aVals[nut*3+3], 
            aVals[nut*3+4], aVals[nut*3+5], aVals[nut*3+6], aVals[nut*3+7], 
            aVals[nut*3+8], aVals[nut*3+9], aVals[nut*3+10], aVals[nut*3+11]);
         MessageInterface::ShowMessage(
            "      a[4](1st 12 els)   = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*4+0], aVals[nut*4+1], aVals[nut*4+2], aVals[nut*4+3], 
            aVals[nut*4+4], aVals[nut*4+5], aVals[nut*4+6], aVals[nut*4+7], 
            aVals[nut*4+8], aVals[nut*4+9], aVals[nut*4+10], aVals[nut*4+11]);
         MessageInterface::ShowMessage(
            "      a[*](Last els)     = [%d %d %d %d %d]\n",
            aVals[nut-1], aVals[nut+nut-1], aVals[nut*2+nut-1], 
            aVals[nut*3+nut-1], aVals[nut*4+nut-1]);
            
         MessageInterface::ShowMessage(
            "      a[0](els 51-62)    = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[51], aVals[52], aVals[53], aVals[54], aVals[55], aVals[56], 
            aVals[57], aVals[58], aVals[59], aVals[60], aVals[61], aVals[62]);
         MessageInterface::ShowMessage(
            "      a[1](els 51-62)    = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*1+51], aVals[nut*1+52], aVals[nut*1+53], aVals[nut*1+54], 
            aVals[nut*1+55], aVals[nut*1+56], aVals[nut*1+57], aVals[nut*1+58], 
            aVals[nut*1+59], aVals[nut*1+60], aVals[nut*1+61], aVals[nut*1+62]);
         MessageInterface::ShowMessage(
            "      a[2](els 51-62)    = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*2+51], aVals[nut*2+52], aVals[nut*2+53], aVals[nut*2+54], 
            aVals[nut*2+55], aVals[nut*2+56], aVals[nut*2+57], aVals[nut*2+58], 
            aVals[nut*2+59], aVals[nut*2+60], aVals[nut*2+61], aVals[nut*2+62]);
         MessageInterface::ShowMessage(
            "      a[3](els 51-62)    = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*3+51], aVals[nut*3+52], aVals[nut*3+53], aVals[nut*3+54], 
            aVals[nut*3+55], aVals[nut*3+56], aVals[nut*3+57], aVals[nut*3+58], 
            aVals[nut*3+59], aVals[nut*3+60], aVals[nut*3+61], aVals[nut*3+62]);
         MessageInterface::ShowMessage(
            "      a[4](els 51-62)    = [%d %d %d %d %d %d %d %d %d %d %d %d]\n",
            aVals[nut*4+51], aVals[nut*4+52], aVals[nut*4+53], aVals[nut*4+54], 
            aVals[nut*4+55], aVals[nut*4+56], aVals[nut*4+57], aVals[nut*4+58], 
            aVals[nut*4+59], aVals[nut*4+60], aVals[nut*4+61], aVals[nut*4+62]);
         MessageInterface::ShowMessage(
            "      a[*](Last els)    = [%d %d %d %d %d]\n",
            aVals[nut-1], aVals[nut+nut-1], aVals[nut*2+nut-1], 
            aVals[nut*3+nut-1], aVals[nut*4+nut-1]);
            
         MessageInterface::ShowMessage(
            "      A(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            AVals[0], AVals[1], AVals[2], AVals[3], 
            AVals[4], AVals[5], AVals[6], AVals[7], 
            AVals[8], AVals[9], AVals[10], AVals[11]);
         MessageInterface::ShowMessage(
            "      B(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            BVals[0], BVals[1], BVals[2], BVals[3], BVals[4], BVals[5], 
            BVals[6], BVals[7], BVals[8], BVals[9], BVals[10], BVals[11]);
         MessageInterface::ShowMessage(
            "      C(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            CVals[0], CVals[1], CVals[2], CVals[3], CVals[4], CVals[5], 
            CVals[6], CVals[7], CVals[8], CVals[9], CVals[10], CVals[11]);
         MessageInterface::ShowMessage(
            "      D(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            DVals[0], DVals[1], DVals[2], DVals[3], DVals[4], DVals[5], 
            DVals[6], DVals[7], DVals[8], DVals[9], DVals[10], DVals[11]);
         MessageInterface::ShowMessage(
            "      E(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            EVals[0], EVals[1], EVals[2], EVals[3], EVals[4], EVals[5], 
            EVals[6], EVals[7], EVals[8], EVals[9], EVals[10], EVals[11]);
         MessageInterface::ShowMessage(
            "      F(1st 12 els)     = [%.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf\n"
            "                           %.13lf %.13lf %.13lf %.13lf]\n",
            FVals[0], FVals[1], FVals[2], FVals[3], FVals[4], FVals[5], 
            FVals[6], FVals[7], FVals[8], FVals[9], FVals[10], FVals[11]);
      }
   #endif

   for (i = nut-1; i >= 0; i--)
   {
      #ifdef DEBUG_a_MATRIX
         MessageInterface::ShowMessage("   a[%d}:  %8d %8d %8d %8d %8d\n", i+1,
            aVals[i], aVals[nut*1+i], aVals[nut*2+i], aVals[nut*3+i], 
            aVals[nut*4+i]);
      #endif

      apNut = aVals[i]*meanAnomalyMoon + aVals[nut*1+i]*meanAnomalySun 
         + aVals[nut*2+i]*argLatitudeMoon + aVals[nut*3+i]*meanElongationSun 
         + aVals[nut*4+i]*longAscNodeLunar;
      
      cosAp = cos(apNut);
      sinAp = sin(apNut);
      if (nutationSrc == GmatItrf::NUTATION_1980)
      {
         dPsi += (AVals[i] + BVals[i]*tTDB )*sinAp;
         dEps += (CVals[i] + DVals[i]*tTDB )*cosAp;
      }
      else // 1996 amd 2000 have E and F terms
      {
         dPsi += (AVals[i] + BVals[i]*tTDB )*sinAp + EVals[i]*cosAp;
         dEps += (CVals[i] + DVals[i]*tTDB )*cosAp + FVals[i]*sinAp;
      }

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      apNut(%3d) dPsi(%3d) dEps(%3d) = %.13le %.13le %.13le\n", i, 
            i, i, apNut, dPsi, dEps);
   #endif
   }
   
   dPsi *= RAD_PER_ARCSEC;
   dEps *= RAD_PER_ARCSEC;
   
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      dPsi              = %.13lf\n"
            "      dEps              = %.13lf\n", dPsi, dEps);
   #endif

   // Compute the corrections for planetary effects on the nutation and
   // the obliquity of the ecliptic 
   // NOTE - this part is commented out for now, per Steve Hughes
   // First, compute the mean Heliocentric longitudes of the planets, and the
   // general precession in longitude
   Real dPsiAddend = 0.0, dEpsAddend = 0.0;
   if (nutationSrc == GmatItrf::NUTATION_1996)
   {   
   
      Real longVenus   = (181.979800853  + 58517.8156748  * tTDB)* RAD_PER_DEG;
      Real longEarth   = (100.466448494  + 35999.3728521  * tTDB)* RAD_PER_DEG;
      Real longMars    = (355.433274605  + 19140.299314   * tTDB)* RAD_PER_DEG;
      Real longJupiter = ( 34.351483900  +  3034.90567464 * tTDB)* RAD_PER_DEG;
      Real longSaturn  = ( 50.0774713998 +  1222.11379404 * tTDB)* RAD_PER_DEG;
      Real genPrec     = (1.39697137214 * tTDB + 0.0003086 * tTDB2)
                                 * RAD_PER_DEG;
      Real apPlan = 0.0;
      Real cosApP = 0.0;
      Real sinApP = 0.0;
      Integer nutpl = itrf->GetNumberOfPlanetaryTerms();
    
      #ifdef DEBUG_FIRST_CALL
         if (!firstCallFired)
            MessageInterface::ShowMessage(
               "      longVenus         = %.13lf\n"
               "      longEarth         = %.13lf\n"
               "      longMars          = %.13lf\n"
               "      longJupiter       = %.13lf\n"
               "      longSaturn        = %.13lf\n"
               "      genPrec           = %.13lf\n"
               "      nutpl             = %d\n",
               longVenus, longEarth, longMars, longJupiter, longSaturn, genPrec, 
               nutpl);
      #endif
     
      for (i = nutpl-1; i >= 0; i--)
      {
             apPlan = apVals[0+i]*longVenus + apVals[nutpl*1+i]*longEarth 
             + apVals[nutpl*2+i]*longMars   + apVals[nutpl*3+i]*longJupiter 
             + apVals[nutpl*4+i]*longSaturn + apVals[nutpl*5+i]*genPrec
             + apVals[nutpl*6+i]*meanElongationSun
             + apVals[nutpl*7+i]*argLatitudeMoon
             + apVals[nutpl*8+i]*meanAnomalyMoon
             + apVals[nutpl*9+i]*longAscNodeLunar;
             cosApP = cos(apPlan);
             sinApP = sin(apPlan);
             dPsiAddend += (( ApVals[i] + BpVals[i]*tTDB )*sinApP);
             dEpsAddend += (( CpVals[i] + DpVals[i]*tTDB )*cosApP);
      }
   } // end if nutationSrc == NUTATION_1996
   
    dPsi += dPsiAddend * RAD_PER_ARCSEC;
    dEps += dEpsAddend * RAD_PER_ARCSEC;
    
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      dPsi(0)           = %.13lf\n"
            "      dEps(0)           = %.13lf\n",
            dPsi, dEps);
   #endif
    
   // FOR NOW, SQ's code to approximate GSRF frame
   // NOTE - do we delete this when we put in the planetary stuff above?
   // offset and rate correction to approximate GCRF, Ref.[1], Eq (3-63)  - SQ
   // This is Vallado Eq. 3-62 - WCS
   
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      dPsi(1)           = %.13lf\n"
            "      dEps(1)           = %.13lf\n",
            dPsi, dEps);
   #endif
    
   // Compute obliquity of the ecliptic (Vallado Eq. 3-52 & Eq. 3-63)
   Real TrueOoE = Epsbar + dEps;
   
   // Compute useful trigonometric quantities
   Real cosdPsi   = cos(dPsi);
   Real cosTEoE   = cos(TrueOoE);
   Real sindPsi   = sin(dPsi);
   Real sinEpsbar = sin(Epsbar);
   Real sinTEoE   = sin(TrueOoE);   
   
   // Compute Rotation matrix for transformations from MOD to TOD
   // (Vallado Eq. 3-64)
   NUT.Set( cosdPsi,
           -sindPsi*cosEpsbar,
           -sindPsi*sinEpsbar,
            sindPsi*cosTEoE, 
            cosTEoE*cosdPsi*cosEpsbar + sinTEoE*sinEpsbar,
            sinEpsbar*cosTEoE*cosdPsi - sinTEoE*cosEpsbar,
            sinTEoE*sindPsi,
            sinTEoE*cosdPsi*cosEpsbar - sinEpsbar*cosTEoE,
            sinTEoE*sinEpsbar*cosdPsi + cosTEoE*cosEpsbar);
   
   lastNUTEpoch = atEpoch;
   lastNUT      = NUT;
   lastDPsi     = dPsi; 
   
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("At end of ComputeNutationmatrix ...\n");
      MessageInterface::ShowMessage("   atEpoch   = %.15lf\n", atEpoch.Get());
      MessageInterface::ShowMessage("   longAscNodeLunar   = %.15lf\n", longAscNodeLunar);
      MessageInterface::ShowMessage("   cosEpsbar   = %.15lf\n", cosEpsbar);
      MessageInterface::ShowMessage("   dPsi   = %.15lf\n", dPsi);
	  MessageInterface::ShowMessage("   dEps   = %.15lf\n", dEps);
	  MessageInterface::ShowMessage("   TrueOoE   = %.15lf\n", TrueOoE);
   #endif
//   return NUT;
}

//------------------------------------------------------------------------------
//  void ComputeSiderealTimeRotation(const Real jdTT,
//                                   const GmatTime &mjdUT1,
//                                   Real dPsi,
//                                   Real longAscNodeLunar,
//                                   Real cosEpsbar,
//                                   Real &cosAst,
//                                   Real &sinAst)
//------------------------------------------------------------------------------
/**
 * This method will compute the Sidereal time rotation matrix.
 *
 * @param jdTT              TT Julian Date
 * @param mjdUT1            UT1 time
 * @param dPsi              delta Psi
 * @param longAscNodeLunar  lunar longitude of the ascending node
 * @param cosEpsbar         computed quantity
 * @param cosAst            cos quantity
 * @param sinAst            sin quantity
 *
 */
//------------------------------------------------------------------------------
void AxisSystem::ComputeSiderealTimeRotation(const Real jdTT,
                                             const GmatTime &mjdUT1,
                                             Real dPsi,
                                             Real longAscNodeLunar,
                                             Real cosEpsbar,
                                             Real &cosAst,
                                             Real &sinAst)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
      {
         MessageInterface::ShowMessage("AxisSystem::ComputeSiderealTimeRotation, "
         "for object of type %s\n", typeName.c_str());
         MessageInterface::ShowMessage(
            "   AxisSystem::ComputeSiderealTimeRotation(%.12lf, %.12lf, %.12lf,"
            " %.12lf, %.12lf, %.12lf, %.12lf)\n", jdTT, mjdUT1.GetMjd(), dPsi, 
            longAscNodeLunar, cosEpsbar, cosAst, sinAst);
      }
   #endif
   
   // Compute elapsed Julian centuries (UT1)
   Real tDiff = JD_JAN_5_1941 - JD_OF_J2000;
   Real tUT1 = (mjdUT1 + tDiff).GetMjd() / DAYS_PER_JULIAN_CENTURY;

   Real tUT12    = tUT1  * tUT1;
   Real tUT13    = tUT12 * tUT1;
   
   // First, compute the equation of the equinoxes
   // If the time is before January 1, 1997, don't use the last two terms
   Real term2 = 0.0;
   Real term3 = 0.0;
   if (jdTT > JD_OF_JANUARY_1_1997)
   {
      term2 = (0.00264 * sin(longAscNodeLunar))        * RAD_PER_ARCSEC;
      term3 = (0.000063 * sin(2.0 * longAscNodeLunar)) * RAD_PER_ARCSEC;
   }
   Real EQequinox = (dPsi * cosEpsbar) + term2 + term3;
   
   // Compute Greenwich Apparent Sidereal Time from the Greenwich Mean 
   // Sidereal Time and the Equation of the equinoxes
   // (Vallado Eq. 3-45)
   // NOTE: 1 sec = 15"; 1 hour (= 15 deg) = 54000"
   Real hour2deg = 15.0;
   Real sec2deg = hour2deg / GmatTimeConstants::SECS_PER_HOUR;
   Real ThetaGmst = ((67310.54841 * sec2deg) + 
                     (mjdUT1.GetSec() + mjdUT1.GetFracSec())/SECS_PER_DAY*TWO_PI_DEG + 
                     (8640184.812866 * sec2deg)*tUT1 +
                     (0.093104 * sec2deg)*tUT12 - (6.2e-06 * sec2deg)*tUT13 )
                     * RAD_PER_DEG;
   ThetaGmst = AngleUtil::PutAngleInRadRange(ThetaGmst,0.0,GmatMathConstants::TWO_PI);
   
   Real ThetaAst = ThetaGmst + EQequinox;
   
   //Compute useful trignonometric quantities
   cosAst = cos(ThetaAst);
   sinAst = sin(ThetaAst);
   
   // Compute Rotation matrix for Sidereal Time
   // (Vallado Eq. 3-64)
   ST.Set( cosAst, sinAst, 0.0,
          -sinAst, cosAst, 0.0,
              0.0,    0.0, 1.0);
   
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      EQequinox           = %.15lf\n"
            "      ThetaGmst           = %.15lf\n"
            "      ThetaAst            = %.15lf\n",
            EQequinox, ThetaGmst, ThetaAst);
   #endif
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("ST = %s\n", (ST.ToString()).c_str());
   #endif

//   return ST;
}

//------------------------------------------------------------------------------
//  void ComputeSiderealTimeDotRotation(const GmatTime &mjdUTC,
//                                      const GmatTime &atEpoch,
//                                      Real &cosAst,
//                                      Real &sinAst,
//                                      bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will compute the Sidereal time dot rotation matrix.
 *
 * @param mjdUTC            UTC mean julian day
 * @param atEpoch           A1Mjd epoch
 * @param cosAst            cos quantity
 * @param sinAst            sin quantity
 * @param forceComputation  force matrix computation?
 */
//------------------------------------------------------------------------------
void AxisSystem::ComputeSiderealTimeDotRotation(const GmatTime &mjdUTC, 
                                                const GmatTime &atEpoch,
                                                     Real cosAst, Real sinAst,
                                                     bool forceComputation)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   AxisSystem::ComputeSiderealTimeDotRotation(%.12lf, %.12lf,"
            " %.12lf, %.12lf)\n", mjdUTC.GetMjd(), atEpoch.GetMjd(),
            cosAst, sinAst);
   #endif

   #ifdef DEBUG_UPDATE
      MessageInterface::ShowMessage("----> Computing NEW STderiv matrix at time %12.10f\n",
         atEpoch.GetMjd());
   #endif
   // Convert to MJD UTC to use for polar motion  and LOD 
   // interpolations
   // Get the polar motion and lod data
   Real lod = 0.0;
   Real x, y;
   eop->GetPolarMotionAndLod(mjdUTC,x,y,lod);
   #ifdef DEBUG_AXIS_SYSTEM_EOP
      MessageInterface::ShowMessage("in STderiv calc, mjdUtc     = %12.10f\n", mjdUTC.GetMjd());
      MessageInterface::ShowMessage("                 atEpoch    = %12.10f\n", atEpoch.GetMjd());
      MessageInterface::ShowMessage("                 lod        = %12.10f\n", lod);
      MessageInterface::ShowMessage("                 x          = %12.10f\n", x);
      MessageInterface::ShowMessage("                 y          = %12.10f\n", y);
   #endif
   
   // Compute the portion that has a significant time derivative
   Real omegaE = 7.29211514670698e-05 * (1.0 - (lod / SECS_PER_DAY));
   STderiv.Set(-omegaE * sinAst,  omegaE * cosAst, 0.0,
               -omegaE * cosAst, -omegaE * sinAst, 0.0,
                            0.0,              0.0, 0.0);
   
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("lod = %12,10f\n", lod);
      MessageInterface::ShowMessage("STderiv = %s\n", (STderiv.ToString()).c_str());
   #endif
      
   lastSTDeriv      = STderiv;
   lastSTDerivEpoch = atEpoch;
   
}

//------------------------------------------------------------------------------
//  void ComputePolarMotionRotation(const Real mjdUTC,
//                                  A1Mjd atEpoch,
//                                  bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will compute the polar motion rotation matrix.
 *
 * @param mjdUTC            UTC mean julian day
 * @param atEpoch           A1Mjd epoch
 * @param forceComputation  force matrix computation?
 *
 */
//------------------------------------------------------------------------------
void AxisSystem::ComputePolarMotionRotation(const Real mjdUTC, A1Mjd atEpoch,
                                            bool forceComputation)
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "   AxisSystem::ComputePolarMotionRotation(%.12lf, %.12lf)\n", 
            mjdUTC, atEpoch.Get());
   #endif

   #ifdef DEBUG_UPDATE
      MessageInterface::ShowMessage("----> Computing NEW PM matrix at time %12.10f\n",
         atEpoch.Get());
   #endif
   // Get the polar motion and lod data
   Real lod = 0.0;
   Real x, y;
   eop->GetPolarMotionAndLod(mjdUTC,x,y,lod);
   #ifdef DEBUG_AXIS_SYSTEM_EOP
      MessageInterface::ShowMessage("in PM calc,      mjdUtc     = %12.10f\n", mjdUTC);
      MessageInterface::ShowMessage("                 atEpoch    = %12.10f\n", atEpoch.Get());
      MessageInterface::ShowMessage("                 lod        = %12.10f\n", lod);
      MessageInterface::ShowMessage("                 x          = %12.10f\n", x);
      MessageInterface::ShowMessage("                 y          = %12.10f\n", y);
   #endif
   
   // Compute useful trigonometric quantities
   Real cosX = cos(-x * RAD_PER_ARCSEC);
   Real sinX = sin(-x * RAD_PER_ARCSEC);
   Real cosY = cos(-y * RAD_PER_ARCSEC);
   Real sinY = sin(-y * RAD_PER_ARCSEC);
   
   // Compute the polar motion rotation matrix
   PM.Set( cosX,  sinX*sinY, -sinX*cosY,
            0.0,       cosY,       sinY,
           sinX, -cosX*sinY,  cosX*cosY); 

   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("x = %12.10f  and y = %12.10f\n", x, y);
      MessageInterface::ShowMessage("PM = %s\n", (PM.ToString()).c_str());
   #endif
      
   lastPM      = PM;
   lastPMEpoch = atEpoch;

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "      lod         = %.13lf\n"
            "      x           = %.13lf\n"
            "      y           = %.13lf\n",
            lod, x * RAD_PER_ARCSEC, y * RAD_PER_ARCSEC);
   #endif
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = true;
   #endif

}

//---------------------------------------------------------------------------
//  void InitializeReference(SpacePoint *refObj)
//---------------------------------------------------------------------------
/**
 * Initialization of the origin or other reference object for the AxisSystem.
 *
 * @note  This is necessary for the BodyFixedPoint origins/references because
 *        they require local coordinate systems that must be created/handled.
 *
 */
//---------------------------------------------------------------------------
void AxisSystem::InitializeReference(SpacePoint *refObj)
{
   // if the origin/reference is a BodyFixedPoint it needs to be initialized
   if (refObj->IsOfType("BodyFixedPoint"))
   {
      refObj->SetSolarSystem(solar);
      refObj->Initialize();
   }
   // otherwise do nothing (for now, anyway)
}


//---------------------------------------------------------------------------
//  void AxisSystem::RotationMatrixFromICRFToFK5(const A1Mjd & atEpoch)
//---------------------------------------------------------------------------
/**
 * Calculates the rotation matrix from ICRF to FK5 used for Spice rotations
 *
 * @param atEpoch  the epoch
 *
 */
 //---------------------------------------------------------------------------
void AxisSystem::RotationMatrixFromICRFToFK5(const A1Mjd & atEpoch)
{
   Real theEpoch = atEpoch.Get();


   if (theEpoch == lastIcrfToFk5Epoch.Get())
      return;
#ifdef DEBUG_ICRF_TOFK5
   MessageInterface::ShowMessage(
      "Enter AxisSystem::RotationMatrixFromICRFToFK5 at epoch %18.12lf; \n\n", theEpoch);
#endif

   // Specify Euler rotation vector for theEpoch:
   Real vec[3];
   ICRFFile* icrfFile = ICRFFile::Instance();
   icrfFile->Initialize();
   icrfFile->GetICRFRotationVector(theEpoch, vec, 3, 9);

   // Calculate rotation matrix based on Euler rotation vector:
   Real angle = GmatMathUtil::Sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
   Real a[3];
   a[0] = vec[0] / angle; a[1] = vec[1] / angle; a[2] = vec[2] / angle;
   //Need to check if this angle is within Cos range. This code was copied from a method by the same name in CoordinateConverter
   Real c = GmatMathUtil::Cos(angle);
   Real s = GmatMathUtil::Sin(angle);

   // rotation matrix from FK5 to ICRF:
   Rmatrix33 rotM;
   rotM.SetElement(0, 0, c + a[0] * a[0] * (1 - c));
   rotM.SetElement(0, 1, a[0] * a[1] * (1 - c) + a[2] * s);
   rotM.SetElement(0, 2, a[0] * a[2] * (1 - c) - a[1] * s);
   rotM.SetElement(1, 0, a[0] * a[1] * (1 - c) - a[2] * s);
   rotM.SetElement(1, 1, c + a[1] * a[1] * (1 - c));
   rotM.SetElement(1, 2, a[1] * a[2] * (1 - c) + a[0] * s);
   rotM.SetElement(2, 0, a[0] * a[2] * (1 - c) + a[1] * s);
   rotM.SetElement(2, 1, a[1] * a[2] * (1 - c) - a[0] * s);
   rotM.SetElement(2, 2, c + a[2] * a[2] * (1 - c));

   // rotation matrix from ICRF to FK5:
   icrfToFK5 = rotM.Transpose();

   lastIcrfToFk5Epoch = theEpoch;
#ifdef DEBUG_ICRF_TOFK5
   MessageInterface::ShowMessage("theEpoch  = %18.12lf\n", theEpoch);

   MessageInterface::ShowMessage("rotation vector = %18.12e %18.12e %18.12e\n", vec[0], vec[1], vec[2]);
   MessageInterface::ShowMessage("R(0,0)=%18.12e,  R(0,1)=%18.12e,  R(0,2)=%18.12e\n", icrfToFK5(0, 0), icrfToFK5(0, 1), icrfToFK5(0, 2));
   MessageInterface::ShowMessage("R(1,0)=%18.12e,  R(1,1)=%18.12e,  R(1,2)=%18.12e\n", icrfToFK5(1, 0), icrfToFK5(1, 1), icrfToFK5(1, 2));
   MessageInterface::ShowMessage("R(2,0)=%18.12e,  R(2,1)=%18.12e,  R(2,2)=%18.12e\n", icrfToFK5(2, 0), icrfToFK5(2, 1), icrfToFK5(2, 2));
#endif

#ifdef DEBUG_ICRF_TOFK5
   MessageInterface::ShowMessage("NOW exiting AxisSystem::RotationMatrixFromICRFToFK5 ...\n\n");
#endif
}

//---------------------------------------------------------------------------
//  CalculateSpiceFrameRotationMatrix(const std::string spiceToFrame, const GmatTime &atEpoch)
//---------------------------------------------------------------------------
/**
 * Calculates the rotation matrix from J2000 to spiceToFrame using Spice calls
 * Also calculates the rot dot matrix
 *
 * @param atEpoch, the epoch the rotation occurs on
 * @param spiceToFrame, The spice frame to rotate to
 *
 */
 //---------------------------------------------------------------------------
void AxisSystem::CalculateSpiceFrameRotationMatrix(const std::string spiceToFrame, const GmatTime &atEpoch)
{
   CelestialBody* origin;
   
   if (this->origin->IsOfType("CelestialBody"))
      origin = (CelestialBody*)(this->origin);
   else
      origin = (CelestialBody*) this->origin->GetJ2000Body();

   Rmatrix66 rotMatSpice;
   if (!origin->IsSpiceSetUp())
      origin->SetUpSPICE();
   rotMatSpice = origin->GetSpiceKernelReader()->GetRotationMatrix("J2000", spiceToFrame, atEpoch);
   rotMatrix = rotMatSpice.UpperLeft().Transpose();

   RotationMatrixFromICRFToFK5(atEpoch.GetMjd());
   rotMatrix = icrfToFK5 * rotMatrix;
   rotDotMatrix = icrfToFK5 * (rotMatSpice.LowerLeft().Transpose());
}


//---------------------------------------------------------------------------
//  bool SetCalculateRotMatrixDeriv(bool turnOn) 
//---------------------------------------------------------------------------
/**
* Turn on/off flag to indicate GMAT will calculate d[R]/dX derivative of 
* rotation matrix or not
*
* @param turnOn  flag indicates to calculate d[R]/dX 
*
* @return        true if calculation is successed, true otherwise
*/
//---------------------------------------------------------------------------
bool AxisSystem::SetCalculateRotMatrixDeriv(bool turnOn) 
{
	bool prevFlag = specifyRotMatrixDeriv;
	specifyRotMatrixDeriv = turnOn;

	return prevFlag;
}
