//$Id$
//------------------------------------------------------------------------------
//                                  CoordinateSystem
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
// Created: 2004/12/22
//
/**
 * Implementation of the CoordinateSystem class.
 *
 * @note Code currently assumes that state vector passed in is an Rvector6.
 *       This will need updating soon.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateSystemException.hpp"
#include "CoordinateBase.hpp"
#include "ObjectReferencedAxes.hpp"
#include "MJ2000EqAxes.hpp"
#include "MJ2000EcAxes.hpp"
#include "TopocentricAxes.hpp"
#include "TODEqAxes.hpp"
#include "TEMEAxes.hpp"
#include "ICRFAxes.hpp"
#include "SpiceAxes.hpp"
#include "BodyFixedAxes.hpp"
#include "Rmatrix33.hpp"
#include "GmatGlobal.hpp"               // for GetEopFile(), GetItrfCoefficientsFile()
#include "CoordinateConverter.hpp"
#include "MessageInterface.hpp"

#include <algorithm>                    // Required by GCC 4.3

//#define DEBUG_RENAME 1
//#define DEBUG_INPUTS_OUTPUTS
//#define DEBUG_DESTRUCTION
//#define DEBUG_CONSTRUCTION
//#define DEBUG_CS_INIT 1
//#define DEBUG_CS_SET_REF
//#define DEBUG_TRANSLATION
//#define DEBUG_CS_CREATE
//#define DEBUG_CS_SET_AXIS
//#define DEBUG_CS_SET
//#define DEBUG_REFERENCE_SETTING
//#define DEBUG_CS_REF_OBJECT 3

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
CoordinateSystem::PARAMETER_TEXT[CoordinateSystemParamCount - CoordinateBaseParamCount] =
{
   "Axes",
   "UpdateInterval",
   "OverrideOriginInterval",
   "SpiceFrameId",
   "Epoch",
};

const Gmat::ParameterType
CoordinateSystem::PARAMETER_TYPE[CoordinateSystemParamCount - CoordinateBaseParamCount] =
{
   Gmat::OBJECT_TYPE,     // "Axes",
   Gmat::REAL_TYPE,       // "UpdateInterval",
   Gmat::BOOLEAN_TYPE,    // "OverrideOriginInterval",
   Gmat::STRING_TYPE,     // "SpiceFrameId",
   Gmat::REAL_TYPE,       // "Epoch",
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  CoordinateSystem(const std::string &itsType,
//                   const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs CoordinateSystem structures
 * (default constructor).
 *
 * @param itsType  GMAT script string associated with this type of object.
 * @param itsName  Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for CoordinateSystem.  Derived 
 *       classes must pass in the ofType and itsType parameters.
 */
//---------------------------------------------------------------------------
CoordinateSystem::CoordinateSystem(const std::string &itsType,
                                   const std::string &itsName) :
CoordinateBase(Gmat::COORDINATE_SYSTEM,itsType,itsName),
spiceFrameId(""),
axes               (NULL)
{
   objectTypes.push_back(Gmat::COORDINATE_SYSTEM);
   objectTypeNames.push_back("CoordinateSystem");
   parameterCount = CoordinateSystemParamCount;
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("Now constructing CS %s of type %s\n",
         itsName.c_str(), itsType.c_str());
   #endif
}

//---------------------------------------------------------------------------
//  CoordinateSystem(const CoordinateSystem &coordSys);
//---------------------------------------------------------------------------
/**
 * Constructs base CoordinateSystem structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param coordSys  CoordinateSystem instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
CoordinateSystem::CoordinateSystem(const CoordinateSystem &coordSys) :
CoordinateBase (coordSys),
spiceFrameId(coordSys.spiceFrameId)
{
   if (coordSys.axes)
   {
      axes = (AxisSystem*)coordSys.axes->Clone();
      axes->SetCoordinateSystemName(instanceName);
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (axes, "clonedAxes", "CoordinateSystem Copy Constructor",
          "(AxisSystem*) obj->Clone()");
      #endif
   }
   else
      axes = NULL;
}

//---------------------------------------------------------------------------
//  CoordinateSystem& operator=(const CoordinateSystem &coordSys)
//---------------------------------------------------------------------------
/**
* Assignment operator for CoordinateSystem structures.
 *
 * @param coordSys The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const CoordinateSystem& CoordinateSystem::operator=(
                        const CoordinateSystem &coordSys)
{
   if (&coordSys == this)
      return *this;
   CoordinateBase::operator=(coordSys);
   spiceFrameId = coordSys.spiceFrameId;
   if (axes)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (axes, axes->GetTypeName(), "CoordinateSystem::operator=",
          "deleting axes");
      #endif
      delete axes;
   }
   
   if (coordSys.axes)
   {
      axes = (AxisSystem*)coordSys.axes->Clone();
      axes->SetCoordinateSystemName(instanceName);
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (axes, "clonedAxes", "CoordinateSystem Assignment operator",
          "(AxisSystem*) obj->Clone()");
      #endif
   }
   else
      axes = NULL;

   return *this;
}

//---------------------------------------------------------------------------
//  const bool operator==(const CoordinateSystem &coordSys)
//---------------------------------------------------------------------------
/**
 * Equality operator for CoordinateSystem structures.
 *
 * @param coordSys  The original that is being checked for equality with
 *                   "this".
 *
 * @return true if "this" is the same as the input coordSys
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::operator==(const CoordinateSystem &coordSys)
{
   if (&coordSys == this)
      return true;

   if (axes == NULL)
   {
      if (coordSys.axes == NULL)
         return true;
      else                       
         return false;
   }
   
   if (spiceFrameId != coordSys.spiceFrameId)
      return false; 

   if (axes->GetType() == (coordSys.axes)->GetType())
   {
      if (origin == coordSys.origin)
         return true;
      // need to check j2000Body too?
   }
   
   
   return false;
}

//---------------------------------------------------------------------------
//  ~CoordinateSystem()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
CoordinateSystem::~CoordinateSystem()
{
   #ifdef DEBUG_DESTRUCTION
   MessageInterface::ShowMessage("---> Entering CoordinateSystem destructor for %s\n",
   instanceName.c_str());
   #endif
   
   if (axes)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (axes, axes->GetTypeName(), "CoordinateSystem::~CoordinateSystem()",
          "deleting axes");
      #endif
      delete axes;
   }
   axes = NULL;
   #ifdef DEBUG_DESTRUCTION
   MessageInterface::ShowMessage("---> LEAVING CoordinateSystem destructor for %s\n",
   instanceName.c_str());
   #endif
}


//---------------------------------------------------------------------------
// AxisSystem* GetAxisSystem()
//---------------------------------------------------------------------------
AxisSystem* CoordinateSystem::GetAxisSystem()
{
   return axes;
}


//---------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem) const
//---------------------------------------------------------------------------
/**
 * Returns a value indicating whether the use of an Eop File is required, optional,
 * or not used for this coordinate system.
 *
 * @param <forBaseSystem> system for which an EOP file may be needed <future>
 *
 * @return required, not used, or optional use of an EOP File
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage CoordinateSystem::UsesEopFile(const std::string &forBaseSystem) const
{
   if (axes) return axes->UsesEopFile(forBaseSystem);
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesItrfFile() const
//---------------------------------------------------------------------------
/**
 * Returns a value indicating whether the use of an ITRF File is required, optional,
 * or not used for this coordinate system.
 *
 * @return required, not used, or optional use of an ITRF File
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage CoordinateSystem::UsesItrfFile() const
{
   if (axes) return axes->UsesItrfFile();
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesEpoch() const
//---------------------------------------------------------------------------
/**
 * Returns a value indicating whether the use of an epoch is required, optional,
 * or not used for this coordinate system.
 *
 * @return required, not used, or optional use of an epoch
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage CoordinateSystem::UsesEpoch() const
{
   if (axes) return axes->UsesEpoch();
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesPrimary() const
//---------------------------------------------------------------------------
/**
 * Returns a value indicating whether the use of a primary body is required, optional,
 * or not used for this coordinate system.
 *
 * @return required, not used, or optional use of a primary body
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage CoordinateSystem::UsesPrimary() const
{
   if (axes) return axes->UsesPrimary();
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesSecondary() const
//---------------------------------------------------------------------------
/**
 * Returns a value indicating whether the use of a secondary body is required, optional,
 * or not used for this coordinate system.
 *
 * @return required, not used, or optional use of a secondary body
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage CoordinateSystem::UsesSecondary() const
{
   if (axes) return axes->UsesSecondary();
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesReferenceObject() const
//---------------------------------------------------------------------------
/**
 * Returns a value indicating whether the use of a reference object is required,
 * optional, or not used for this coordinate system.
 *
 * @return required, not used, or optional use of a secondary body
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage CoordinateSystem::UsesReferenceObject() const
{
   if (axes) return axes->UsesReferenceObject();
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesXAxis() const
//---------------------------------------------------------------------------
/**
 * Returns a value indicating whether the use of an X Axis is required, optional,
 * or not used for this coordinate system.
 *
 * @return required, not used, or optional use of an X axis
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage CoordinateSystem::UsesXAxis() const
{
   if (axes) return axes->UsesXAxis();
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesYAxis() const
//---------------------------------------------------------------------------
/**
 * Returns a value indicating whether the use of a Y Axis is required, optional,
 * or not used for this coordinate system.
 *
 * @return required, not used, or optional use of a Y axis
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage CoordinateSystem::UsesYAxis() const
{
   if (axes) return axes->UsesYAxis();
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesZAxis() const
//---------------------------------------------------------------------------
/**
 * Returns a value indicating whether the use of a Z Axis is required, optional,
 * or not used for this coordinate system.
 *
 * @return required, not used, or optional use of a Z axis
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage CoordinateSystem::UsesZAxis() const
{
   if (axes) return axes->UsesZAxis();
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const
//---------------------------------------------------------------------------
/**
 * Returns a value indicating whether the use of an nutation update interval
 * is required, optional, or not used for this coordinate system.
 *
 * @return required, not used, or optional use of a nutation update interval
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage CoordinateSystem::UsesNutationUpdateInterval() const
{
   if (axes) return axes->UsesNutationUpdateInterval();
   return GmatCoordinate::NOT_USED;
}

//---------------------------------------------------------------------------
// bool UsesSpacecraft(const std::string &withName) const
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not this coordinate system has a
 * spacecraft as the origin, secondary, or primary.
 *
 * @return flag indicating whether or not this coordinate system has
 *         a spacecraft as the origin, primary, or secondary
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::UsesSpacecraft(const std::string &withName) const
{
   if (axes) return axes->UsesSpacecraft(withName);
   return false;
}

//---------------------------------------------------------------------------
// bool RequiresCelestialBodyOrigin() const
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not this coordinate system requires
 * its origin to be a celestial body.
 *
 * @return flag indicating whether or not this coordinate system requires
 *         a celestial body as the origin
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::RequiresCelestialBodyOrigin() const
{
   if (axes) return axes->RequiresCelestialBodyOrigin();
   return false;
}


//---------------------------------------------------------------------------
// bool HasCelestialBodyOrigin() const
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not this coordinate system has a
 * celestial body as the origin.
 *
 * @return flag indicating whether or not this coordinate system has
 *         a celestial body as the origin
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::HasCelestialBodyOrigin() const
{
   if (axes) return axes->HasCelestialBodyOrigin();
   return false;
}

void CoordinateSystem::SetAllowWithoutRates(bool allow)
{
   if (axes) axes->SetAllowWithoutRates(allow);
}

bool CoordinateSystem::AllowWithoutRates() const
{
   if (axes) return axes->AllowWithoutRates();
   return false;
}


//------------------------------------------------------------------------------
// bool IsInertial()
//------------------------------------------------------------------------------
/**
 * Checks the axis system to see if it is inertially pointing
 *
 * @return True for inertial systems, false if not inertial
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::IsInertial()
{
   return (axes ? axes->IsInertial() : false);
}

// methods to set parameters for the AxisSystems
//---------------------------------------------------------------------------
// void SetPrimaryObject(SpacePoint *prim)
//---------------------------------------------------------------------------
/**
 * Sets the primary object for a coordinate system that needs it.
 *
 * @param <prim> primary object
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::SetPrimaryObject(SpacePoint *prim)
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("Entering SetPrimaryObject for %s\n", instanceName.c_str());
   #endif
   if (allowModify)
   {
      if (axes) axes->SetPrimaryObject(prim);
   }
   else
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
}

//---------------------------------------------------------------------------
// void SetSecondaryObject(SpacePoint *second)
//---------------------------------------------------------------------------
/**
 * Sets the secondary object for a coordinate system that needs it.
 *
 * @param <second> secondary object
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::SetSecondaryObject(SpacePoint *second)
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("Entering SetSecondaryObject for %s\n", instanceName.c_str());
   #endif
   if (allowModify)
   {
      if (axes) axes->SetSecondaryObject(second);
   }
   else
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
}

//---------------------------------------------------------------------------
// void SetReferenceObject(SpacePoint *refObj)
//---------------------------------------------------------------------------
/**
 * Sets the reference object for a coordinate system that needs it.
 *
 * @param <refObj> reference object
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::SetReferenceObject(SpacePoint *refObj)
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("Entering SetReferenceObject for %s\n", instanceName.c_str());
   #endif
   if (allowModify)
   {
      if (axes) axes->SetReferenceObject(refObj);
   }
   else
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
}

//---------------------------------------------------------------------------
// void SetEpoch(const A1Mjd &toEpoch)
//---------------------------------------------------------------------------
/**
 * Sets the epoch for a coordinate system that needs it.
 *
 * @param <toEpoch> epoch to set
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::SetEpoch(const A1Mjd &toEpoch)
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("Entering SetEpoch for %s\n", instanceName.c_str());
   #endif
   if (allowModify)
   {
      if (axes) axes->SetEpoch(toEpoch);
   }
   else
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
}

//---------------------------------------------------------------------------
// void SetXAxis(const std::string &toValue)
//---------------------------------------------------------------------------
/**
 * Sets the X axis value for a coordinate system that needs it.
 *
 * @param <toValue> X axis value
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::SetXAxis(const std::string &toValue)
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("Entering SetXAxis for %s\n", instanceName.c_str());
   #endif
   if (allowModify)
   {
      if (axes) axes->SetXAxis(toValue);
   }
   else
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
}

//---------------------------------------------------------------------------
// void SetYAxis(const std::string &toValue)
//---------------------------------------------------------------------------
/**
 * Sets the Y axis value for a coordinate system that needs it.
 *
 * @param <toValue> Y axis value
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::SetYAxis(const std::string &toValue)
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("Entering SetYAxis for %s\n", instanceName.c_str());
   #endif
   if (allowModify)
   {
      if (axes) axes->SetYAxis(toValue);
   }
   else
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
}

//---------------------------------------------------------------------------
// void SetZAxis(const std::string &toValue)
//---------------------------------------------------------------------------
/**
 * Sets the Z axis value for a coordinate system that needs it.
 *
 * @param <toValue> Z axis value
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::SetZAxis(const std::string &toValue)
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("Entering SetZAxis for %s\n", instanceName.c_str());
   #endif
   if (allowModify)
   {
      if (axes) axes->SetZAxis(toValue);
   }
   else
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
}

//---------------------------------------------------------------------------
// void SetEopFile(EopFile *eopF)
//---------------------------------------------------------------------------
/**
 * Sets the EOP file for a coordinate system that needs it.
 *
 * @param <eopF> EOP file pointer
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::SetEopFile(EopFile *eopF)
{
   if (axes) axes->SetEopFile(eopF);
}

//---------------------------------------------------------------------------
// void SetCoefficientsFile(ItrfCoefficientsFile *itrfF)
//---------------------------------------------------------------------------
/**
 * Sets the ITRF coefficients file for a coordinate system that needs it.
 *
 * @param <itrfF> ITRF coefficients file pointer
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::SetCoefficientsFile(ItrfCoefficientsFile *itrfF)
{
   if (axes) axes->SetCoefficientsFile(itrfF);
}

//---------------------------------------------------------------------------
// SpacePoint* GetPrimaryObject() const
//---------------------------------------------------------------------------
/**
 * Returns a pointer to the primary object.
 *
 * @return pointer to the primary object
 *
 */
//---------------------------------------------------------------------------
SpacePoint* CoordinateSystem::GetPrimaryObject() const
{
   if (axes) return axes->GetPrimaryObject();
   return NULL;
}

//---------------------------------------------------------------------------
// SpacePoint* GetSecondaryObject() const
//---------------------------------------------------------------------------
/**
 * Returns a pointer to the secondary object.
 *
 * @return pointer to the secondary object
 *
 */
//---------------------------------------------------------------------------
SpacePoint* CoordinateSystem::GetSecondaryObject() const
{
   if (axes) return axes->GetSecondaryObject();
   return NULL;
}

//---------------------------------------------------------------------------
// SpacePoint* GetReferenceObject() const
//---------------------------------------------------------------------------
/**
 * Returns a pointer to the reference object.
 *
 * @return pointer to the reference object
 *
 */
//---------------------------------------------------------------------------
SpacePoint* CoordinateSystem::GetReferenceObject() const
{
   if (axes) return axes->GetReferenceObject();
   return NULL;
}

//---------------------------------------------------------------------------
// virtual std::string GetPrimaryObjectName() const
//---------------------------------------------------------------------------
std::string CoordinateSystem::GetPrimaryObjectName() const
{
   if (axes) return axes->GetPrimaryObjectName();
   return "";
}

//---------------------------------------------------------------------------
// virtual std::string GetSecondaryObjectName() const
//---------------------------------------------------------------------------
std::string CoordinateSystem::GetSecondaryObjectName() const
{
   if (axes) return axes->GetSecondaryObjectName();
   return "";
}

//---------------------------------------------------------------------------
// virtual std::string GetReferenceObjectName() const
//---------------------------------------------------------------------------
std::string CoordinateSystem::GetReferenceObjectName() const
{
   if (axes) return axes->GetReferenceObjectName();
   return "";
}

//---------------------------------------------------------------------------
// A1Mjd GetEpoch() const
//---------------------------------------------------------------------------
/**
 * Returns the epoch value
 *
 * @return A1Mjd epoch
 *
 */
//---------------------------------------------------------------------------
A1Mjd CoordinateSystem::GetEpoch() const
{
   if (axes) return axes->GetEpoch();
   return A1Mjd();  // does this make sense?
}

//---------------------------------------------------------------------------
// std::string GetXAxis() const
//---------------------------------------------------------------------------
/**
 * Returns the X Axis value
 *
 * @return X axis value
 *
 */
//---------------------------------------------------------------------------
std::string CoordinateSystem::GetXAxis() const
{
   if (axes) return axes->GetXAxis();
   return "";
}

//---------------------------------------------------------------------------
// std::string GetYAxis() const
//---------------------------------------------------------------------------
/**
 * Returns the Y Axis value
 *
 * @return Y axis value
 *
 */
//---------------------------------------------------------------------------
std::string CoordinateSystem::GetYAxis() const
{
   if (axes) return axes->GetYAxis();
   return "";
}

//---------------------------------------------------------------------------
// std::string GetZAxis() const
//---------------------------------------------------------------------------
/**
 * Returns the Z Axis value
 *
 * @return Z axis value
 *
 */
//---------------------------------------------------------------------------
std::string CoordinateSystem::GetZAxis() const
{
   if (axes) return axes->GetZAxis();
   return "";
}

//---------------------------------------------------------------------------
// EopFile* GetEopFile() const
//---------------------------------------------------------------------------
/**
 * Returns a pointer to the EOP file
 *
 * @return pointer to the EOP file
 *
 */
//---------------------------------------------------------------------------
EopFile* CoordinateSystem::GetEopFile() const
{
   if (axes) return axes->GetEopFile();
   return NULL;
}

//---------------------------------------------------------------------------
// ItrfCoefficientsFile* GetItrfCoefficientsFile() const
//---------------------------------------------------------------------------
/**
 * Returns a pointer to the ITRF coefficients file
 *
 * @return pointer to the ITRF coefficients file
 *
 */
//---------------------------------------------------------------------------
ItrfCoefficientsFile* CoordinateSystem::GetItrfCoefficientsFile()
{
   if (axes) return axes->GetItrfCoefficientsFile();
   return NULL;
}

//---------------------------------------------------------------------------
// Rmatrix33 GetLastRotationMatrix() const
//---------------------------------------------------------------------------
/**
 * Returns the rotation matrix last computed by this coordinate system
 *
 * @return last-computed rotation matrix (matrix from this coordinate system
 *         to its base system)
 *
 */
//---------------------------------------------------------------------------
Rmatrix33 CoordinateSystem::GetLastRotationMatrix() const
{
   if (!axes) 
   {
      std::string errmsg = "No AxisSystem defined for coordinate system \"";
      errmsg += instanceName + "\".\n";
      throw CoordinateSystemException(errmsg);
   }
   return axes->GetLastRotationMatrix();
}

//---------------------------------------------------------------------------
// void GetLastRotationMatrix(Real *mat) const
//---------------------------------------------------------------------------
/**
 * Returns the rotation matrix last computed by this coordinate system
 *
 * @param <mat> output last-computed rotation matrix (matrix from this
 *              coordinate system to its base system)
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::GetLastRotationMatrix(Real *mat) const
{
   if (!axes) 
   {
      std::string errmsg = "No AxisSystem defined for coordinate system \"";
      errmsg += instanceName + "\".\n";
      throw CoordinateSystemException(errmsg);
   }
   axes->GetLastRotationMatrix(mat);
}

//---------------------------------------------------------------------------
// Rmatrix33 GetLastRotationDotMatrix() const
//---------------------------------------------------------------------------
/**
 * Returns the rotation dot matrix last computed by this coordinate system
 *
 * @return last-computed rotation dot matrix (matrix from this coordinate system
 *         to its base system)
 *
 */
//---------------------------------------------------------------------------
Rmatrix33 CoordinateSystem::GetLastRotationDotMatrix() const
{
   if (!axes) 
   {
      std::string errmsg = "No AxisSystem defined for coordinate system \"";
      errmsg += instanceName + "\".\n";
      throw CoordinateSystemException(errmsg);
   }
   return axes->GetLastRotationDotMatrix();
}

//---------------------------------------------------------------------------
// void GetLastRotationDotMatrix(Real *mat) const
//---------------------------------------------------------------------------
/**
 * Returns the rotation dot matrix last computed by this coordinate system
 *
 * @param <mat> output last-computed dot rotation matrix (matrix from this
 *              coordinate system to its base system)
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::GetLastRotationDotMatrix(Real *mat) const
{
   if (!axes) 
   {
      std::string errmsg = "No AxisSystem defined for coordinate system \"";
      errmsg += instanceName + "\".\n";
      throw CoordinateSystemException(errmsg);
   }
   axes->GetLastRotationDotMatrix(mat);
}


//---------------------------------------------------------------------------
// std::vector<Rmatrix33> GetDerivativeOfLastRotationMatrix()
//---------------------------------------------------------------------------
/**
* Returns derivative of the rotation matrix last computed by this coordinate system
*
* @return derivative of the last-computed rotation matrix (matrix from this 
*            coordinate system to its base system)
*
*/
//---------------------------------------------------------------------------
std::vector<Rmatrix33> CoordinateSystem::GetDerivativeOfLastRotationMatrix()
{
	if (!axes)
	{
		std::string errmsg = "No AxisSystem defined for coordinate system \"";
		errmsg += instanceName + "\".\n";
		throw CoordinateSystemException(errmsg);
	}
	return axes->GetDerivativeOfLastRotationMatrix();
}




//---------------------------------------------------------------------------
// bool AreAxesOfType(const std::string &ofType) const
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the coordinate system has
 * an axis system of the specified type
 *
 * @param <ofType> axis type specified
 *
 * @return true if axes are of specified type; false otherwise
 *
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::AreAxesOfType(const std::string &ofType) const
{
   if (!axes) return false;
   return axes->IsOfType(ofType);
}

//---------------------------------------------------------------------------
// std::string GetBaseSystem() const
//---------------------------------------------------------------------------
/**
 * Returns the string value of the base system for this coordinate system/
 * axis system
 *
 * @return base system
 *
 */
//---------------------------------------------------------------------------
std::string CoordinateSystem::GetBaseSystem() const
{
   if (!axes)
   {
      std::string errmsg = "No AxisSystem defined for coordinate system \"";
      errmsg += instanceName + "\".\n";
      throw CoordinateSystemException(errmsg);
   }
   return axes->GetBaseSystem();
}

//---------------------------------------------------------------------------
//  bool CoordinateSystem::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this CoordinateSystem.
 *
 * @return true if successfully initialized; false otherwise
 *
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::Initialize()
{
   #if DEBUG_CS_INIT
   MessageInterface::ShowMessage
      ("CoordinateSystem::Initialize() csName=%s, axes=%p\n",
       GetName().c_str(), axes);
   #endif
   
   CoordinateBase::Initialize();
   
   if (axes)
   {
      #if DEBUG_CS_INIT
      MessageInterface::ShowMessage
         ("   solar=<%p>, originName='%s', j2000BodyName='%s', origin=<%p>'%s', "
          "j2000Body=<%p>'%s'\n", solar, originName.c_str(), j2000BodyName.c_str(),
          origin, origin->GetName().c_str(), j2000Body, j2000Body->GetName().c_str());
      #endif
      
      axes->SetSolarSystem(solar);
      axes->SetOriginName(originName);
      axes->SetJ2000BodyName(j2000BodyName);
      axes->SetRefObject(origin,Gmat::SPACE_POINT,originName);
      axes->SetRefObject(j2000Body,Gmat::SPACE_POINT,j2000BodyName);
      axes->SetCoordinateSystemName(instanceName);
      if (axes->IsOfType("SpiceAxes"))
         ((SpiceAxes*)axes)->SetSpiceFrameId(spiceFrameId);
      axes->Initialize();

      isInitialized = true;
   }
   
   return true;
}

//---------------------------------------------------------------------------
// void SetModifyFlag(bool modFlag)
//---------------------------------------------------------------------------
/**
 * Sets the allowModify flag for the coordinate system
 *
 * @param modFlag  flag indicating whether or not this coordinate
 *                 system is a built-in system and cannot be
 *                 modified by the user
 *
 */
//---------------------------------------------------------------------------
void CoordinateSystem::SetModifyFlag(bool modFlag)
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("CS::SetModifyFlag setting flag for %s to %s\n",
            instanceName.c_str(), (modFlag? "true" : "false"));
   #endif
   CoordinateBase::SetModifyFlag(modFlag);
   if (axes) axes->SetModifyFlag(modFlag);
}

//------------------------------------------------------------------------------
//  Rvector  ToBaseSystem(const A1Mjd &epoch, const Rvector &inState,
//                        bool coincident, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method converts the input state, in 'this' axisSystem, to the base system.
 *
 * @param epoch            epoch for which to do the conversion.
 * @param inState          input state to convert.
 * @param coincident       are the systems coincident?.
 * @param forceComputation should we force computation even if it is not time to
 *                         compute
 *
 * @return input state converted to the base system.
 *
 */
//------------------------------------------------------------------------------
Rvector CoordinateSystem::ToBaseSystem(const A1Mjd &epoch, const Rvector &inState,
                                       bool coincident,
                                       bool forceComputation)
{
   static Rvector internalState;
   static Rvector finalState;
   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::ToBaseSystem, inState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
   #endif
   internalState.SetSize(inState.GetSize());
   finalState.SetSize(inState.GetSize());
   
   if (axes)
   {
      if (!axes->RotateToBaseSystem(epoch,inState,internalState, forceComputation))
         throw CoordinateSystemException("Error rotating state to the base system for "
                                         + instanceName);
   }
   else // assume this is the base system, so no rotation is necessary
      internalState = inState;

   if (!coincident)
   {
      if (!TranslateToBaseSystem(epoch,internalState, finalState))
         throw CoordinateSystemException("Error translating state to the base system for "
                                         + instanceName);
      #ifdef DEBUG_INPUTS_OUTPUTS
         MessageInterface::ShowMessage(
         "In CS::ToBaseSystem, translation happening\n");
      #endif
   }
   else
      finalState = internalState;

   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::ToBaseSystem, internalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      internalState[0], internalState[1], internalState[2], 
      internalState[3],internalState[4], internalState[5]);
      MessageInterface::ShowMessage(
      "In CS::ToBaseSystem, finalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      finalState[0], finalState[1], finalState[2], 
      finalState[3],finalState[4], finalState[5]);
   #endif
   return finalState; // implicit copy constructor
}

//------------------------------------------------------------------------------
//  Rvector  ToBaseSystem(const GmatTime &epoch, const Rvector &inState,
//                        bool coincident, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method converts the input state, in 'this' axisSystem, to the base system.
 *
 * @param epoch            epoch for which to do the conversion.
 * @param inState          input state to convert.
 * @param coincident       are the systems coincident?.
 * @param forceComputation should we force computation even if it is not time to
 *                         compute
 *
 * @return input state converted to the base system.
 *
 */
//------------------------------------------------------------------------------
Rvector CoordinateSystem::ToBaseSystem(const GmatTime &epoch, const Rvector &inState,
                                       bool coincident,
                                       bool forceComputation)
{
   static Rvector internalState;
   static Rvector finalState;
   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::ToBaseSystem, inState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
   #endif
   internalState.SetSize(inState.GetSize());
   finalState.SetSize(inState.GetSize());
   
   if (axes)
   {
      if (!axes->RotateToBaseSystem(epoch,inState,internalState, forceComputation))
         throw CoordinateSystemException("Error rotating state to the base system for "
                                         + instanceName);
   }
   else // assume this is the base system, so no rotation is necessary
      internalState = inState;

   if (!coincident)
   {
      if (!TranslateToBaseSystem(epoch,internalState, finalState))
         throw CoordinateSystemException("Error translating state to the base system for "
                                         + instanceName);
      #ifdef DEBUG_INPUTS_OUTPUTS
         MessageInterface::ShowMessage(
         "In CS::ToBaseSystem, translation happening\n");
      #endif
   }
   else
      finalState = internalState;

   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::ToBaseSystem, internalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      internalState[0], internalState[1], internalState[2], 
      internalState[3],internalState[4], internalState[5]);
      MessageInterface::ShowMessage(
      "In CS::ToBaseSystem, finalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      finalState[0], finalState[1], finalState[2], 
      finalState[3],finalState[4], finalState[5]);
   #endif
   return finalState; // implicit copy constructor
}

//------------------------------------------------------------------------------
// void ToBaseSystem(const A1Mjd &epoch, const Real *inState, Real *outState,
//                   bool coincident, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method converts the input state, in 'this' axisSystem, to the base system.
 *
 * @param epoch            epoch for which to do the conversion.
 * @param inState          input state to convert.
 * @parm  outState         output state resulting from the conversion
 * @param coincident       are the systems coincident?.
 * @param forceComputation should we force computation even if it is not time to
 *                         compute
 */
//------------------------------------------------------------------------------
void CoordinateSystem::ToBaseSystem(const A1Mjd &epoch, const Real *inState,
                                    Real *outState,     bool coincident,
                                    bool forceComputation)
{
   #ifdef DEBUG_INPUTS_OUTPUTS
   MessageInterface::ShowMessage
      ("In CS::ToBaseSystem, inState = \n   %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
       inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
   MessageInterface::ShowMessage
      ("   axes=<%p> '%s'\n", axes, axes ? (axes->GetName().c_str() == "" ?
       axes->GetTypeName().c_str() : axes->GetName().c_str()) : "NULL");
   #endif
   Real internalState[6];
   if (axes)
   {
      if (!axes->RotateToBaseSystem(epoch, inState, internalState, forceComputation))
         throw CoordinateSystemException("Error rotating state to the base system for "
                                         + instanceName);
      #ifdef DEBUG_INPUTS_OUTPUTS
      Rvector6 rv(internalState);
      MessageInterface::ShowMessage
         ("In CS::ToBaseSystem, Rotated to the base system, internalState=\n   %s\n",
          rv.ToString().c_str());
      #endif
   }
   else // assume this is the base system, so no rotation is necessary
      for (Integer i=0; i<6;i++) internalState[i] = inState[i];
   
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage("In ToBaseSystem for %s, coincident = %s\n",
            instanceName.c_str(), (coincident? "TRUE" : "FALSE"));
   #endif
   if (!coincident)
   {
      if (!TranslateToBaseSystem(epoch, internalState, outState))
         throw CoordinateSystemException("Error translating state to the base system for "
                                         + instanceName);
      #ifdef DEBUG_INPUTS_OUTPUTS
      Rvector6 rv(outState);
      MessageInterface::ShowMessage
         ("In CS::ToBaseSystem, translated to the base system, outState=\n   %s\n",
          rv.ToString().c_str());
      #endif
   }
   else
      for (Integer i=0; i<6; i++)  outState[i] = internalState[i];
}

//------------------------------------------------------------------------------
// void ToBaseSystem(const A1Mjd &epoch, const Real *inState, Real *outState,
//                   bool coincident, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method converts the input state, in 'this' axisSystem, to the base system.
 *
 * @param epoch            epoch for which to do the conversion.
 * @param inState          input state to convert.
 * @parm  outState         output state resulting from the conversion
 * @param coincident       are the systems coincident?.
 * @param forceComputation should we force computation even if it is not time to
 *                         compute
 */
//------------------------------------------------------------------------------
void CoordinateSystem::ToBaseSystem(const GmatTime &epoch, const Real *inState,
                                    Real *outState,     bool coincident,
                                    bool forceComputation)
{
   #ifdef DEBUG_INPUTS_OUTPUTS
   MessageInterface::ShowMessage
      ("In CS::ToBaseSystem, inState = \n   %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
       inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
   MessageInterface::ShowMessage
      ("   axes=<%p> '%s'\n", axes, axes ? (axes->GetName().c_str() == "" ?
       axes->GetTypeName().c_str() : axes->GetName().c_str()) : "NULL");
   #endif
   Real internalState[6];
   if (axes)
   {
      if (!axes->RotateToBaseSystem(epoch, inState, internalState, forceComputation))
         throw CoordinateSystemException("Error rotating state to the base system for "
                                         + instanceName);
      #ifdef DEBUG_INPUTS_OUTPUTS
      Rvector6 rv(internalState);
      MessageInterface::ShowMessage
         ("In CS::ToBaseSystem, Rotated to the base system, internalState=\n   %s\n",
          rv.ToString().c_str());
      #endif
   }
   else // assume this is the base system, so no rotation is necessary
      for (Integer i=0; i<6;i++) internalState[i] = inState[i];
   
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage("In ToBaseSystem for %s, coincident = %s\n",
            instanceName.c_str(), (coincident? "TRUE" : "FALSE"));
   #endif
   if (!coincident)
   {
      if (!TranslateToBaseSystem(epoch, internalState, outState))
         throw CoordinateSystemException("Error translating state to the base system for "
                                         + instanceName);
      #ifdef DEBUG_INPUTS_OUTPUTS
      Rvector6 rv(outState);
      MessageInterface::ShowMessage
         ("In CS::ToBaseSystem, translated to the base system, outState=\n   %s\n",
          rv.ToString().c_str());
      #endif
   }
   else
      for (Integer i=0; i<6; i++)  outState[i] = internalState[i];
}

//------------------------------------------------------------------------------
//  Rvector  FromBaseSystem(const A1Mjd &epoch, const Rvector &inState,
//                          bool coincident, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method converts the input state, in the base axisSystem, to "this"
 * axisSystem.
 *
 * @param epoch            epoch for which to do the conversion.
 * @param inState          input state to convert.
 * @param coincident       are the systems coincident?.
 * @param forceComputation should we force computation even if it is not time to
 *                         compute
 *
 * @return input state converted from the base system to 'this' axisSystem.
 *
 */
//------------------------------------------------------------------------------
Rvector CoordinateSystem::FromBaseSystem(const A1Mjd &epoch, const Rvector &inState,
                                         bool coincident,
                                         bool forceComputation)
{
   static Rvector internalState;
   static Rvector finalState;
   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromBaseSystem, inState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
   #endif
   internalState.SetSize(inState.GetSize());
   finalState.SetSize(inState.GetSize());
   if (!coincident)
   {
      if (!TranslateFromBaseSystem(epoch,inState, internalState))//,j2000Body))
         throw CoordinateSystemException("Error translating from the base system for "
                                         + instanceName);
    #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromBaseSystem (2), translation happening ...\n");
    #endif
   }
   else
      internalState = inState;
      
   if (axes)
   {
      if (!axes->RotateFromBaseSystem(epoch,internalState,finalState,
          forceComputation))//,j2000Body))
         throw CoordinateSystemException("Error rotating state from the base system for "
                                         + instanceName);
   }
   else
      finalState    = internalState;
                                         

   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromBaseSystem, internalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      internalState[0], internalState[1], internalState[2], 
      internalState[3],internalState[4], internalState[5]);
      MessageInterface::ShowMessage(
      "In CS::FromBaseSystem, finalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      finalState[0], finalState[1], finalState[2], 
      finalState[3], finalState[4], finalState[5]);
   #endif
   return finalState; // implicit copy constructor
}

//------------------------------------------------------------------------------
//  Rvector  FromBaseSystem(const GmatTime &epoch, const Rvector &inState,
//                          bool coincident, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method converts the input state, in the base axisSystem, to "this"
 * axisSystem.
 *
 * @param epoch            epoch for which to do the conversion.
 * @param inState          input state to convert.
 * @param coincident       are the systems coincident?.
 * @param forceComputation should we force computation even if it is not time to
 *                         compute
 *
 * @return input state converted from the base system to 'this' axisSystem.
 *
 */
//------------------------------------------------------------------------------
Rvector CoordinateSystem::FromBaseSystem(const GmatTime &epoch, const Rvector &inState,
                                         bool coincident,
                                         bool forceComputation)
{
   static Rvector internalState;
   static Rvector finalState;
   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromBaseSystem, inState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
   #endif
   internalState.SetSize(inState.GetSize());
   finalState.SetSize(inState.GetSize());
   if (!coincident)
   {
      if (!TranslateFromBaseSystem(epoch,inState, internalState))//,j2000Body))
         throw CoordinateSystemException("Error translating from the base system for "
                                         + instanceName);
    #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromBaseSystem (2), translation happening ...\n");
    #endif
   }
   else
      internalState = inState;
      
   if (axes)
   {
      if (!axes->RotateFromBaseSystem(epoch,internalState,finalState,
          forceComputation))//,j2000Body))
         throw CoordinateSystemException("Error rotating state from the base system for "
                                         + instanceName);
   }
   else
      finalState    = internalState;
                                         

   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromBaseSystem, internalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      internalState[0], internalState[1], internalState[2], 
      internalState[3],internalState[4], internalState[5]);
      MessageInterface::ShowMessage(
      "In CS::FromBaseSystem, finalState = %.17f  %.17f  %.17f  %.17f  %.17f  %.17f\n",
      finalState[0], finalState[1], finalState[2], 
      finalState[3], finalState[4], finalState[5]);
   #endif
   return finalState; // implicit copy constructor
}

//------------------------------------------------------------------------------
//  void  FromBaseSystem(const A1Mjd &epoch, const Rvector &inState,
//                       bool coincident, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method converts the input state, in the base axisSystem, to "this"
 * axisSystem.
 *
 * @param epoch            epoch for which to do the conversion.
 * @param inState          input state to convert.
 * @param outState         output state resulting from the conversion
 * @param coincident       are the systems coincident?.
 * @param forceComputation should we force computation even if it is not time to
 *                         compute
 *
 */
//------------------------------------------------------------------------------
void CoordinateSystem::FromBaseSystem(const A1Mjd &epoch, const Real *inState,
                                      Real *outState,  bool coincident,
                                      bool forceComputation)
{
   #ifdef DEBUG_INPUTS_OUTPUTS
     MessageInterface::ShowMessage(
     "Entering CS::FromBaseSystem (1) ..........\n");
   #endif
   Real internalState[6];
   if (!coincident)
   {
      if (!TranslateFromBaseSystem(epoch,inState, internalState))//,j2000Body))
         throw CoordinateSystemException("Error translating from the base system for "
                                         + instanceName);
    #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromBaseSystem (1), translation happening\n");
    #endif
   }
   else
      for (Integer i=0; i<6; i++) internalState[i] = inState[i];

   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
     "In CS::FromBaseSystem set internalState ... %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  \n",
     internalState[0],internalState[1],internalState[2],internalState[3],internalState[4],internalState[5]);
   #endif
      

   if (axes)
   {
      #ifdef DEBUG_INPUTS_OUTPUTS
         MessageInterface::ShowMessage("About to call RotateFromBaseSystem for object %s\n",
               (GetName()).c_str());
      #endif
      if (!axes->RotateFromBaseSystem(epoch,internalState,outState,
          forceComputation))//,j2000Body))
         throw CoordinateSystemException("Error rotating state from the base system for "
                                         + instanceName);
   }
   else
   {
      #ifdef DEBUG_INPUTS_OUTPUTS
         MessageInterface::ShowMessage("AXES are NULL in CS::FromBaseSystem (1) for c.s.of type %s\n",
               (GetTypeName()).c_str());
      #endif
      for (Integer i=0; i<6; i++) outState[i]    = internalState[i];
   }
   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
     "LEAVING CS::FromBaseSystem outSTate =  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  \n",
     outState[0],outState[1],outState[2],outState[3],outState[4],outState[5]);
   #endif
                                         
}

//------------------------------------------------------------------------------
//  void  FromBaseSystem(const GmatTime &epoch, const Rvector &inState,
//                       bool coincident, bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method converts the input state, in the base axisSystem, to "this"
 * axisSystem.
 *
 * @param epoch            epoch for which to do the conversion.
 * @param inState          input state to convert.
 * @param outState         output state resulting from the conversion
 * @param coincident       are the systems coincident?.
 * @param forceComputation should we force computation even if it is not time to
 *                         compute
 *
 */
//------------------------------------------------------------------------------
void CoordinateSystem::FromBaseSystem(const GmatTime &epoch, const Real *inState,
                                      Real *outState,  bool coincident,
                                      bool forceComputation)
{
   #ifdef DEBUG_INPUTS_OUTPUTS
     MessageInterface::ShowMessage(
     "Entering CS::FromBaseSystem (1) ..........\n");
   #endif
   Real internalState[6];
   if (!coincident)
   {
      if (!TranslateFromBaseSystem(epoch,inState, internalState))//,j2000Body))
         throw CoordinateSystemException("Error translating from the base system for "
                                         + instanceName);
    #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
      "In CS::FromBaseSystem (1), translation happening\n");
    #endif
   }
   else
      for (Integer i=0; i<6; i++) internalState[i] = inState[i];

   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
     "In CS::FromBaseSystem set internalState ... %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  \n",
     internalState[0],internalState[1],internalState[2],internalState[3],internalState[4],internalState[5]);
   #endif
      

   if (axes)
   {
      #ifdef DEBUG_INPUTS_OUTPUTS
         MessageInterface::ShowMessage("About to call RotateFromBaseSystem for object %s\n",
               (GetName()).c_str());
      #endif
      if (!axes->RotateFromBaseSystem(epoch,internalState,outState,
          forceComputation))//,j2000Body))
         throw CoordinateSystemException("Error rotating state from the base system for "
                                         + instanceName);
   }
   else
   {
      #ifdef DEBUG_INPUTS_OUTPUTS
         MessageInterface::ShowMessage("AXES are NULL in CS::FromBaseSystem (1) for c.s.of type %s\n",
               (GetTypeName()).c_str());
      #endif
      for (Integer i=0; i<6; i++) outState[i]    = internalState[i];
   }
   #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage(
     "LEAVING CS::FromBaseSystem outSTate =  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  \n",
     outState[0],outState[1],outState[2],outState[3],outState[4],outState[5]);
   #endif
                                         
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
* This method returns a clone of the CoordinateSystem.
 *
 * @return clone of the CoordinateSystem.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CoordinateSystem::Clone() const
{
   return (new CoordinateSystem(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void CoordinateSystem::Copy(const GmatBase* orig)
{
   operator=(*((CoordinateSystem *)(orig)));
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames the specified reference object to the specified new name.
 *
 * @param type type of the reference object to rename
 * @param oldName old name of the object
 * @param newName new name of the object
 *
 * @return true if rename is successful or not necessary because the
 *         axis system does not contain a reference to the object;
 *         false otherwise
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::RenameRefObject(const UnsignedInt type,
                                       const std::string &oldName,
                                       const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("CoordinateSystem::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (!CoordinateBase::RenameRefObject(type, oldName, newName))
      return false;
   if (axes)
      return axes->RenameRefObject(type, oldName, newName);
   else
      return true;
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::IsParameterReadOnly(const Integer id) const
{
   if ((id == EPOCH) || (id == UPDATE_INTERVAL) ||
       (id == OVERRIDE_ORIGIN_INTERVAL))
      return true;
   
   return CoordinateBase::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool CoordinateSystem::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
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
 *
 */
//------------------------------------------------------------------------------
std::string CoordinateSystem::GetParameterText(const Integer id) const
{
   if (id >= CoordinateBaseParamCount && id < CoordinateSystemParamCount)
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
Integer CoordinateSystem::GetParameterID(const std::string &str) const
{
   for (Integer i = CoordinateBaseParamCount; i < CoordinateSystemParamCount; i++)
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
Gmat::ParameterType CoordinateSystem::GetParameterType(const Integer id) const
{
   if (id >= CoordinateBaseParamCount && id < CoordinateSystemParamCount)
      return PARAMETER_TYPE[id - CoordinateBaseParamCount];
   
   return CoordinateBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id  ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string CoordinateSystem::GetParameterTypeString(const Integer id) const
{
   return CoordinateBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
Real CoordinateSystem::GetRealParameter(const Integer id) const
{
   if (id == UPDATE_INTERVAL) 
   {
      if (axes) return axes->GetRealParameter("UpdateInterval");
   }
   else if (id == EPOCH) 
   {
      if (axes) return axes->GetRealParameter("Epoch");
   }
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
Real CoordinateSystem::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_CS_SET
   MessageInterface::ShowMessage
      ("CoordinateSystem::SetRealParameter()entered, axes=<%p>, id=%d, "
       "value=%f\n", axes, id, value);
   #endif
   
   if (!allowModify)
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
//   if (id == UPDATE_INTERVAL)  // removed
//   }
   if (id == EPOCH)
   {
      if (axes)
         return axes->SetRealParameter("Epoch", value);
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
Real CoordinateSystem::GetRealParameter(const std::string &label) const
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
Real CoordinateSystem::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or throw an exception if
 *         there is no axis system set.
 */
//------------------------------------------------------------------------------
std::string CoordinateSystem::GetStringParameter(const Integer id) const
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("Entering GetStringParameter for %s\n", instanceName.c_str());
   #endif
   if (id == AXES)
   {
      if (axes)
         return axes->GetTypeName();
      else
         throw CoordinateSystemException("Axis system not set for "
                                         + instanceName);
   }
   if (id == SPICE_FRAME_ID)
      return spiceFrameId;
      
   return CoordinateBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 *
 * @return The string stored for this parameter, or throw an exception if
 *         there is no axis system set.
 */
//------------------------------------------------------------------------------
std::string CoordinateSystem::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::SetStringParameter(const Integer id,
                                          const std::string &value)
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("Entering CS::SetStringParameter for %s with id = %d, values = %s, allowModify = %s\n",
            instanceName.c_str(), id, value.c_str(), (allowModify? "true" : "false"));
   #endif
   if (!allowModify)
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
   switch (id)
   {
      case AXES:
         return true;
      case SPICE_FRAME_ID:
         spiceFrameId = value;
         return true;
      default:
         return CoordinateBase::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
/**
 * @see SetStringParameter(const std::string &label, const std::string &value)
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::SetStringParameter(const std::string &label,
                                          const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::SetStringParameter(const std::string &label,
                                          const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::GetBooleanParameter(const Integer id) const
{
   if (id == OVERRIDE_ORIGIN_INTERVAL) 
   {
      if (axes) return axes->GetBooleanParameter("OverrideOriginInterval");
      else      return false;  // or throw an exception here?
   }
   return CoordinateBase::GetBooleanParameter(id); 
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::SetBooleanParameter(const Integer id,
                                           const bool value)
{
   #ifdef DEBUG_CS_SET
      MessageInterface::ShowMessage("Entering GetBooleanParameter for %s\n", instanceName.c_str());
   #endif
   if (!allowModify)
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
//   if (id == OVERRIDE_ORIGIN_INTERVAL) // removed
//   {
//   }
   return CoordinateBase::SetBooleanParameter(id, value);
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::SetBooleanParameter(const std::string &label,
                                           const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the CoordinateSystem class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CoordinateSystem::GetRefObject(const UnsignedInt type,
                                         const std::string &name)
{
   switch (type)
   {
      case Gmat::AXIS_SYSTEM:
         return axes;
      // This is a way to get origin, primary and secondary from GmatFunction
      // without casting CoordinateSystem * (LOJ: 2009.12.18)
      // @see GmatFunction::BuildUnusedGlobalObjectList()
      case Gmat::SPACE_POINT:
         if (name == "_GFOrigin_")
            return origin;
         if (name == "_GFPrimary_")
            return GetPrimaryObject();
         if (name == "_GFSecondary_")
            return GetSecondaryObject();
      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return CoordinateBase::GetRefObject(type, name);
}


//---------------------------------------------------------------------------
// bool IsOwnedObject(Integer id) const;
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::IsOwnedObject(Integer id) const
{
   if (id == AXES)
      return true;
   else
      return false;
}


//---------------------------------------------------------------------------
//  GmatBase* GetOwnedObject(Integer whichOne)
//---------------------------------------------------------------------------
/**
 * Access GmatBase objects belonging to this instance.
 *
 * This method is used to access the axis system when a script is written to a
 * file.
 *
 * @param <whichOne> The index for this owned object.
 *
 * @return Pointer to the owned object.
 */
//---------------------------------------------------------------------------
GmatBase* CoordinateSystem::GetOwnedObject(Integer whichOne)
{
   if (whichOne == 0)
      return axes;

   return CoordinateBase::GetOwnedObject(whichOne);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::HasRefObjectTypeArray()
{
   return true;
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
const ObjectTypeArray& CoordinateSystem::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes = CoordinateBase::GetRefObjectTypeArray();
   refObjectTypes.push_back(Gmat::SPACE_POINT);

   if (axes)
   {
      static ObjectTypeArray axisRefTypes;
      axisRefTypes.clear();

      axisRefTypes = axes->GetRefObjectTypeArray();
      #ifdef DEBUG_REFERENCE_SETTING
         MessageInterface::ShowMessage("In CS, axes = %s\n", axes->GetTypeName().c_str());
         for (unsigned int jj = 0; jj < axisRefTypes.size(); jj++)
            MessageInterface::ShowMessage("   ref type %d = %d\n", (Integer) jj,
                                         (Integer) axisRefTypes[jj]);
      #endif

      for (ObjectTypeArray::iterator i = axisRefTypes.begin(); i != axisRefTypes.end(); ++i)
         if (find(refObjectTypes.begin(), refObjectTypes.end(), *i) == refObjectTypes.end())
            refObjectTypes.push_back(*i);
   }
   return refObjectTypes;
}


// DJC added 5/9/05 to facilitate Sandbox initialization
//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns the name(s) of the reference object(s). (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The name(s) of the reference object(s).
 */
//------------------------------------------------------------------------------
const StringArray& CoordinateSystem::GetRefObjectNameArray(const UnsignedInt type)
{
   #if DEBUG_CS_REF_OBJECT
   MessageInterface::ShowMessage
      ("CoordinateSystem::GetRefObjectNameArray() type=%d(%s)\n", type,
       GetObjectTypeString(type).c_str());
   #endif
   
//   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
//   {
      // Here we want the names of all named reference objects used in this
      // coordinate system.  (The axis system is not named, so we do not return
      // anything for it.)
      static StringArray refs;
      refs.clear();
      
      refs = CoordinateBase::GetRefObjectNameArray(type);
      
      #if DEBUG_CS_REF_OBJECT
      for (unsigned int i=0; i<refs.size(); i++)
         MessageInterface::ShowMessage("   cs obj=%s\n", refs[i].c_str());
      #endif
      
      if (axes)
      {
         static StringArray axisRefs;
         axisRefs.clear();
         
         axisRefs = axes->GetRefObjectNameArray(type);
         
         #if DEBUG_CS_REF_OBJECT
         for (unsigned int i=0; i<axisRefs.size(); i++)
            MessageInterface::ShowMessage("   axisRef=%s\n", axisRefs[i].c_str());
         #endif
         
         for (StringArray::iterator i = axisRefs.begin(); i != axisRefs.end(); ++i)
            if (find(refs.begin(), refs.end(), *i) == refs.end())
               if( (*i) != "")
               {
                  #if DEBUG_CS_REF_OBJECT
                  MessageInterface::ShowMessage("      axis obj=%s\n", (*i).c_str());
                  #endif
                  refs.push_back(*i);
               }
      }
      return refs;
//   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return CoordinateBase::GetRefObjectNameArray(type);
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const char *name)
//------------------------------------------------------------------------------
/**
 * @see SetRefObject(GmatBase *obj, const UnsignedInt type, const std::string &name)
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                    const char *name)
{
   return SetRefObject(obj, type, std::string(name));
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
* This method sets a reference object for the CoordinateSystem class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object 
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                    const std::string &name)
{
   if (obj == NULL)
      return false;

   #ifdef DEBUG_CS_SET_REF
      MessageInterface::ShowMessage
         ("Entering CS::SetRefObject with obj of type %s and name '%s'\n",
          (obj->GetTypeName()).c_str(), name.c_str());
   #endif
   
   bool retval = false;
   
   switch (type)
   {
      case Gmat::AXIS_SYSTEM:
      {
         #ifdef DEBUG_CS_SET_AXIS
         MessageInterface::ShowMessage
            ("CoordinateSystem::SetRefObject() %s, before axes=%p, obj=%p\n",
             GetName().c_str(), axes, obj);
         MessageInterface::ShowMessage("CS::SetRefObject - object is of type AXIS_SYSTEM\n");
         #endif
         
         AxisSystem *oldAxis = axes;
         
         axes = (AxisSystem*) obj->Clone();
         #ifdef DEBUG_CS_SET_AXIS
            MessageInterface::ShowMessage("CS::SetRefObject - axis cloned!!!\n");
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (axes, "clonedAxes", "CoordinateSystem::SetRefObject()",
             "(AxisSystem*) obj->Clone()");
         #endif
         axes->SetName("");
         axes->SetCoordinateSystemName(instanceName);
         ownedObjectCount = 1;
         
         #ifdef DEBUG_CS_SET_AXIS
         MessageInterface::ShowMessage
            ("CoordinateSystem::SetRefObject() %s, after axes=%p\n",
             GetName().c_str(), axes);
         #endif
         if (oldAxis)
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (oldAxis, oldAxis->GetTypeName(), "CoordinateSystem::SetRefObject()",
                "deleting oldAxis");
            #endif
            delete oldAxis;
            oldAxis = NULL;
         }
         return true;
      }
      default:
         break;
   }
   
   // Set ref object on owned axis system
   if (obj->IsOfType(Gmat::SPACE_POINT))
      retval = CoordinateBase::SetRefObject(obj, type, name);
   
   if (axes)
      retval |= axes->SetRefObject(obj, type, name);
   
   if (retval)
      return true;
   #ifdef DEBUG_CS_SET_REF
      MessageInterface::ShowMessage("CS::SetRefObject - object is NOT of type AXIS_SYSTEM\n");
   #endif
   
   // Not handled here -- invoke the next higher SetRefObject call
   return CoordinateBase::SetRefObject(obj, type, name);
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
UnsignedInt CoordinateSystem::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case AXES:
      return Gmat::AXIS_SYSTEM;
   default:
      return CoordinateBase::GetPropertyObjectType(id);
   }
}


//----------------------------
// public static methods
//----------------------------
//------------------------------------------------------------------------------
// static CoordinateSystem* CreateLocalCoordinateSystem(
//                     const std::string &csName, const std::string &axesType,
//                     SpacePoint *origin, SpacePoint *primary,
//                     SpacePoint *secondary, SpacePoint *j2000Body,
//                     SolarSystem *solarSystem, bool initializeIt = true)
//------------------------------------------------------------------------------
/**
 * Creates CoordinateSystem with VNB, LVLH, MJ2000Eq, and SpacecraftBody axis.
 *
 * @param  csName      Name of the cooridnate system
 * @param  axesType    Axes system type name
 * @param  origin      Axes system origin body pointer
 * @param  primary     Axes system primary body pointer
 * @param  secondary   Axes system secondary body pointer
 * @param  j2000Body   Axes system J2000 body pointer
 * @param  solarSystem Solar system pointer
 * @param  initializeIt Initialize the Coordinate System before returning it?
 *
 * @return new CoordinateSystem pointer if successful, NULL otherwise
 *
 * WARNING: The J200Body must be set identically for all objects in a GMAT run;
 * not doing so will give incorrect results.
 * In addition, the setting of a body other than Earth as the J2000Body has
 * not been tested.
 */
//------------------------------------------------------------------------------
CoordinateSystem* CoordinateSystem::CreateLocalCoordinateSystem(
                     const std::string &csName, const std::string &axesType,
                     SpacePoint *origin, SpacePoint *primary,
                     SpacePoint *secondary, SpacePoint *j2000Body,
                     SolarSystem *solarSystem, bool initializeIt)
{
   #ifdef DEBUG_CS_CREATE
   MessageInterface::ShowMessage
      ("CoordinateSystem::CreateLocalCoordinateSystem() entered\n   csName='%s', "
       "axesType='%s', origin=<%p>, primary=<%p>, secondary=<%p>\n   j2000Body=<%p>, "
       "solarSystem=<%p>\n", csName.c_str(), axesType.c_str(), origin, primary,
       secondary, j2000Body, solarSystem);
   #endif
   // check for NULL pointers
   if (origin == NULL || j2000Body == NULL || solarSystem == NULL)
      return NULL;
   
   CoordinateSystem *localCS = NULL;
   AxisSystem       *theAxes = NULL;
   
   // check for supported axes name - these are used at least in the Burn code
   if (axesType == "VNB" || axesType == "LVLH" || axesType == "SpacecraftBody")
   {
      if (primary == NULL || secondary == NULL)
         return NULL;
      
      localCS  = new CoordinateSystem("CoordinateSystem",csName);
      theAxes  = new ObjectReferencedAxes(csName);
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (localCS, "localCS", "CoordinateSystem::CreateLocalCoordinateSystem()",
          "new CoordinateSystem()");
      MemoryTracker::Instance()->Add
         (theAxes, "theAxes", "CoordinateSystem::CreateLocalCoordinateSystem()",
          "new ObjectReferencedAxes()");
      #endif
      
      theAxes->SetStringParameter("Primary", primary->GetName());
      theAxes->SetStringParameter("Secondary", secondary->GetName());
      theAxes->SetRefObject(origin, Gmat::SPACE_POINT, origin->GetName());
      theAxes->SetRefObject(primary, Gmat::SPACE_POINT, primary->GetName());
      theAxes->SetRefObject(secondary, Gmat::SPACE_POINT, secondary->GetName());
      
      if (axesType == "VNB")
      {
         theAxes->SetStringParameter("XAxis", "V");
         theAxes->SetStringParameter("YAxis", "N");
         localCS->SetStringParameter("Origin", secondary->GetName());
         localCS->SetRefObject(secondary, Gmat::SPACE_POINT, secondary->GetName());
      }
      else if (axesType == "LVLH")
      {
         // theAxes->SetStringParameter("XAxis", "-R");
         // theAxes->SetStringParameter("YAxis", "-N");
         theAxes->SetStringParameter("XAxis", "R");
         theAxes->SetStringParameter("ZAxis", "N");
         localCS->SetStringParameter("Origin", secondary->GetName());
         localCS->SetRefObject(secondary, Gmat::SPACE_POINT, secondary->GetName());
      }
      else if (axesType == "SpacecraftBody")
      {
         localCS->SetStringParameter("Origin", j2000Body->GetName());
      }
      
      localCS->SetRefObject(theAxes, Gmat::AXIS_SYSTEM, theAxes->GetName());
      localCS->SetRefObject(j2000Body, Gmat::SPACE_POINT, j2000Body->GetName());
      localCS->SetSolarSystem(solarSystem);
      if (initializeIt) localCS->Initialize();
      
      #ifdef DEBUG_CS_CREATE
      MessageInterface::ShowMessage
         ("   Local CS %s<%p> created with AxisSystem <%p>:\n"
          "      axesType    = '%s'\n      Origin      = <%p>'%s'\n"
          "      Primary     = <%p>'%s'\n      Secondary   = <%p>'%s'\n"
          "      j2000body   = <%p>'%s'\n", csName.c_str(), localCS, theAxes, axesType.c_str(),
          localCS->GetOrigin(), localCS->GetOriginName().c_str(), localCS->GetPrimaryObject(),
          localCS->GetPrimaryObject() ? localCS->GetPrimaryObject()->GetName().c_str() : "NULL",
          localCS->GetSecondaryObject(),
          localCS->GetSecondaryObject() ? localCS->GetSecondaryObject()->GetName().c_str() : "NULL",
          localCS->GetJ2000Body(),
          localCS->GetJ2000Body() ? localCS->GetJ2000Body()->GetName().c_str() : "NULL");
      #endif
      
      // Since CoordinateSystem clones AxisSystem, delete it from here
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (theAxes, "theAxes", "CoordinateSystem::CreateLocalCoordinateSystem()",
          "deleting theAxes");
      #endif
      delete theAxes;
      return localCS;
   }
   // these are needed at least by the Measurement code
   else if ((axesType == "MJ2000Eq") || (axesType == "MJ2000Ec") ||
            (axesType == "Topocentric") || (axesType == "TODEq" ) ||
            (axesType == "BodyFixed" ) || (axesType == "TEME") || (axesType == "ICRF") || axesType == "SPICE")
   {
      localCS = new CoordinateSystem("CoordinateSystem",csName);
      if (axesType == "MJ2000Eq")
         theAxes = new MJ2000EqAxes(csName);
      else if (axesType == "MJ2000Ec")
         theAxes = new MJ2000EcAxes(csName);
      else if (axesType == "Topocentric")
         theAxes = new TopocentricAxes(csName);
      else if (axesType == "TODEq")
         theAxes = new TODEqAxes(csName);
      else if (axesType == "TEME")
         theAxes = new TEMEAxes(csName);
      else if (axesType == "ICRF")
         theAxes = new ICRFAxes(csName);
      else if (axesType == "SPICE")
         theAxes = new SpiceAxes(csName);
      else
         theAxes = new BodyFixedAxes(csName);
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (localCS, "localCS", "CreateLocalCoordinateSystem()",
          "new CoordinateSystem()");
      MemoryTracker::Instance()->Add
         (theAxes, "theAxes", "CreateLocalCoordinateSystem()",
          "new ObjectReferencedAxes()");
      #endif
      
      GmatGlobal *gmatGlobal = GmatGlobal::Instance();
      
      #ifdef DEBUG_CS_CREATE
      MessageInterface::ShowMessage
         ("   eop=<%p>, itrf=<%p>\n", gmatGlobal->GetEopFile(),
          gmatGlobal->GetItrfCoefficientsFile());
      #endif
      
      if (theAxes->UsesEopFile() == GmatCoordinate::REQUIRED)
         theAxes->SetEopFile(gmatGlobal->GetEopFile());
      
      if (theAxes->UsesItrfFile() == GmatCoordinate::REQUIRED)
         theAxes->SetCoefficientsFile(gmatGlobal->GetItrfCoefficientsFile());
      
      #ifdef DEBUG_CS_CREATE
        MessageInterface::ShowMessage("   --> origin is %s <%p>\n",
              origin->GetName().c_str(), origin);
        MessageInterface::ShowMessage("   --> J2000Body is %s <%p>\n",
              j2000Body->GetName().c_str(), j2000Body);
      #endif
      localCS->SetRefObject(theAxes, Gmat::AXIS_SYSTEM, theAxes->GetName());
      localCS->SetStringParameter("Origin", origin->GetName());
      localCS->SetRefObject(origin, Gmat::SPACE_POINT, origin->GetName());
      localCS->SetJ2000Body(j2000Body);
//      localCS->SetRefObject(j2000Body, Gmat::SPACE_POINT, j2000Body->GetName());
      localCS->SetSolarSystem(solarSystem);
      if (initializeIt) localCS->Initialize();
      
      #ifdef DEBUG_CS_CREATE
      MessageInterface::ShowMessage
         ("   Local CS %s<%p> created with AxisSystem <%p>:\n"
          "      axesType    = '%s'\n      Origin      = <%p>'%s'\n"
          "      Primary     = <%p>'%s'\n      Secondary   = <%p>'%s'\n"
          "      j2000body   = <%p>'%s'\n", csName.c_str(), localCS, theAxes, axesType.c_str(),
          localCS->GetOrigin(), localCS->GetOriginName().c_str(), localCS->GetPrimaryObject(),
          localCS->GetPrimaryObject() ? localCS->GetPrimaryObject()->GetName().c_str() : "NULL",
          localCS->GetSecondaryObject(),
          localCS->GetSecondaryObject() ? localCS->GetSecondaryObject()->GetName().c_str() : "NULL",
          localCS->GetJ2000Body(),
          localCS->GetJ2000Body() ? localCS->GetJ2000Body()->GetName().c_str() : "NULL");
      #endif
      
      // Since CoordinateSystem clones AxisSystem, delete it from here
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (theAxes, "theAxes", "CoordinateSystem::CreateLocalCoordinateSystem()",
          "deleting theAxes");
      #endif
      delete theAxes;
      return localCS;
   }
   else
   {
      //MessageInterface::ShowMessage
      //   ("**** ERROR **** CoordinateSystem::CreateLocalCoordinateSystem() cannot " 
      //    "create CoordinateSystem, axes name \"%s\" is not supported\n", axesType.c_str());
      throw GmatBaseException("**** ERROR **** CoordinateSystem::CreateLocalCoordinateSystem() cannot "
          "create CoordinateSystem, axes name \"" + axesType + "\" is not supported\n");

      return NULL;
   }
   
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  bool  TranslateToBaseSystem(const A1Mjd &epoch, const Rvector &inState,
//                              Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in "this" axisSystem to the base system
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateToBaseSystem(const A1Mjd &epoch,
                                             const Rvector &inState,
                                             Rvector &outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateToBaseSystem, coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
   #endif
   if (origin == j2000Body)  
      outState = inState;
   else
   {
      // currently assuming an Rvector6 - update needed later?!?!   - awaiting Darrel on this
      // compute vector from origin of the base system to origin of this system
      Rvector6 rif =  origin->GetMJ2000State(epoch) - 
                      (j2000Body->GetMJ2000State(epoch));
	  if (this->GetBaseSystem() == "ICRF")
	  {
		  // get rotation matrix R from ICRF to FK5:
		  CoordinateConverter cc;
		  Rmatrix33 R = cc.GetRotationMatrixFromICRFToFK5(epoch);
		  // convert rif coordinate from FK5 to ICRF:
		  Rvector3 r(rif(0), rif(1), rif(2));
		  Rvector3 v(rif(3), rif(4), rif(5));
		  r = R.Transpose()*r;
		  v = R.Transpose()*v;
		  rif.Set(r(0), r(1), r(2), v(0), v(1), v(2));
	  }

      outState = inState + rif;
      #ifdef DEBUG_TRANSLATION
         Rvector6 tmpOrigin = origin->GetMJ2000State(epoch);
         MessageInterface::ShowMessage(
            "In translation, origin state is %12.17f  %12.17f  %12.17f \n",
            tmpOrigin[0], tmpOrigin[1], tmpOrigin[2]);
         MessageInterface::ShowMessage(
            "                                %12.17f  %12.17f  %12.17f \n",
            tmpOrigin[3], tmpOrigin[4], tmpOrigin[5]);
         Rvector6 tmpJ2000 = j2000Body->GetMJ2000State(epoch);
         MessageInterface::ShowMessage(
            "In translation, J2000Body state is %12.17f  %12.17f  %12.17f \n",
            tmpJ2000[0], tmpJ2000[1], tmpJ2000[2]);
         MessageInterface::ShowMessage(
            "                                %12.17f  %12.17f  %12.17f \n",
            tmpJ2000[3], tmpJ2000[4], tmpJ2000[5]);
         MessageInterface::ShowMessage(
            "In translation, outState is %12.17f  %12.17f  %12.17f \n",
            outState[0], outState[1], outState[2]);
         MessageInterface::ShowMessage(
            "                            %12.17f  %12.17f  %12.17f \n",
            outState[3], outState[4], outState[5]);
      #endif
   }
   return true;
}

//------------------------------------------------------------------------------
//  bool  TranslateToBaseSystem(const GmatTime &epoch, const Rvector &inState,
//                              Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in "this" axisSystem to the base system
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateToBaseSystem(const GmatTime &epoch,
                                             const Rvector &inState,
                                             Rvector &outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateToBaseSystem, coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
   #endif
   if (origin == j2000Body)  
      outState = inState;
   else
   {
      // currently assuming an Rvector6 - update needed later?!?!   - awaiting Darrel on this
      // compute vector from origin of the base system to origin of this system
      Rvector6 rif =  origin->GetMJ2000State(epoch) - 
                      (j2000Body->GetMJ2000State(epoch));
	  if (this->GetBaseSystem() == "ICRF")
	  {
		  // get rotation matrix R from ICRF to FK5:
		  CoordinateConverter cc;
		  Rmatrix33 R = cc.GetRotationMatrixFromICRFToFK5(epoch.GetMjd());
		  // convert rif coordinate from FK5 to ICRF:
		  Rvector3 r(rif(0), rif(1), rif(2));
		  Rvector3 v(rif(3), rif(4), rif(5));
		  r = R.Transpose()*r;
		  v = R.Transpose()*v;
		  rif.Set(r(0), r(1), r(2), v(0), v(1), v(2));
	  }

      outState = inState + rif;
      #ifdef DEBUG_TRANSLATION
         Rvector6 tmpOrigin = origin->GetMJ2000State(epoch);
         MessageInterface::ShowMessage(
            "In translation, origin state is %12.17f  %12.17f  %12.17f \n",
            tmpOrigin[0], tmpOrigin[1], tmpOrigin[2]);
         MessageInterface::ShowMessage(
            "                                %12.17f  %12.17f  %12.17f \n",
            tmpOrigin[3], tmpOrigin[4], tmpOrigin[5]);
         Rvector6 tmpJ2000 = j2000Body->GetMJ2000State(epoch);
         MessageInterface::ShowMessage(
            "In translation, J2000Body state is %12.17f  %12.17f  %12.17f \n",
            tmpJ2000[0], tmpJ2000[1], tmpJ2000[2]);
         MessageInterface::ShowMessage(
            "                                %12.17f  %12.17f  %12.17f \n",
            tmpJ2000[3], tmpJ2000[4], tmpJ2000[5]);
         MessageInterface::ShowMessage(
            "In translation, outState is %12.17f  %12.17f  %12.17f \n",
            outState[0], outState[1], outState[2]);
         MessageInterface::ShowMessage(
            "                            %12.17f  %12.17f  %12.17f \n",
            outState[3], outState[4], outState[5]);
      #endif
   }
   return true;
}

//------------------------------------------------------------------------------
// bool TranslateToBaseSystem(const A1Mjd &epoch, const Real *inState,
//                            Real *outState)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in "this" axisSystem to the base system
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateToBaseSystem(const A1Mjd &epoch,
                                             const Real *inState,
                                             Real *outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateToBaseSystem, coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
   #endif
   #ifdef DEBUG_INPUTS_OUTPUTS
   MessageInterface::ShowMessage
      ("In CS::TranslateToBaseSystem, inState = %.17f  %.17f  %.17f  %.17f  %.17f  "
       "%.17f\n", inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
   #endif
   if (origin == j2000Body)  
      for (Integer i=0; i<6; i++) outState[i] = inState[i];
   else
   {
      Rvector6 originState    = origin->GetMJ2000State(epoch);
      Rvector6 j2000BodyState = j2000Body->GetMJ2000State(epoch);
      #ifdef DEBUG_INPUTS_OUTPUTS
         MessageInterface::ShowMessage
            ("   originState =\n   %s\n", originState.ToString().c_str());
         MessageInterface::ShowMessage
            ("   j2000BodyState =\n   %s\n", j2000BodyState.ToString().c_str());
      #endif
      
      Rvector6 rif =  originState - j2000BodyState;
      
	  if (this->GetBaseSystem() == "ICRF")
	  {
		  // get rotation matrix R from ICRF to FK5:
		  CoordinateConverter cc;
		  Rmatrix33 R = cc.GetRotationMatrixFromICRFToFK5(epoch);
		  // convert rif coordinate from FK5 to ICRF:
		  Rvector3 r(rif(0), rif(1), rif(2));
		  Rvector3 v(rif(3), rif(4), rif(5));
		  r = R.Transpose()*r;
		  v = R.Transpose()*v;
		  rif.Set(r(0), r(1), r(2), v(0), v(1), v(2));
	  }

      #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage("   rif = %s\n", rif.ToString().c_str());
      #endif
      const Real *toRif = rif.GetDataVector();
      for (Integer i=0; i<6; i++)  outState[i] = inState[i] + toRif[i];
   }
   return true;
}

//------------------------------------------------------------------------------
// bool TranslateToBaseSystem(const GmatTime &epoch, const Real *inState,
//                            Real *outState)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in "this" axisSystem to the base system
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateToBaseSystem(const GmatTime &epoch,
                                             const Real *inState,
                                             Real *outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateToBaseSystem, coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
   #endif
   #ifdef DEBUG_INPUTS_OUTPUTS
   MessageInterface::ShowMessage
      ("In CS::TranslateToBaseSystem, inState = %.17f  %.17f  %.17f  %.17f  %.17f  "
       "%.17f\n", inState[0], inState[1], inState[2], inState[3],inState[4], inState[5]);
   #endif
   if (origin == j2000Body)  
      for (Integer i=0; i<6; i++) outState[i] = inState[i];
   else
   {
      Rvector6 originState    = origin->GetMJ2000State(epoch);
      Rvector6 j2000BodyState = j2000Body->GetMJ2000State(epoch);
      #ifdef DEBUG_INPUTS_OUTPUTS
         MessageInterface::ShowMessage
            ("   originState =\n   %s\n", originState.ToString().c_str());
         MessageInterface::ShowMessage
            ("   j2000BodyState =\n   %s\n", j2000BodyState.ToString().c_str());
      #endif
      
      Rvector6 rif =  originState - j2000BodyState;
      
	  if (this->GetBaseSystem() == "ICRF")
	  {
		  // get rotation matrix R from ICRF to FK5:
		  CoordinateConverter cc;
		  Rmatrix33 R = cc.GetRotationMatrixFromICRFToFK5(epoch.GetMjd());
		  // convert rif coordinate from FK5 to ICRF:
		  Rvector3 r(rif(0), rif(1), rif(2));
		  Rvector3 v(rif(3), rif(4), rif(5));
		  r = R.Transpose()*r;
		  v = R.Transpose()*v;
		  rif.Set(r(0), r(1), r(2), v(0), v(1), v(2));
	  }

      #ifdef DEBUG_INPUTS_OUTPUTS
      MessageInterface::ShowMessage("   rif = %s\n", rif.ToString().c_str());
      #endif
      const Real *toRif = rif.GetDataVector();
      for (Integer i=0; i<6; i++)  outState[i] = inState[i] + toRif[i];
   }
   return true;
}

//------------------------------------------------------------------------------
//  bool  TranslateFromBaseSystem(const A1Mjd &epoch, const Rvector &inState,
//                                Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in the base axisSystem to 'this'
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateFromBaseSystem(const A1Mjd &epoch,
                                               const Rvector &inState,
                                               Rvector &outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateFromBaseSystem, coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
      MessageInterface::ShowMessage("   inState  = %le  %le  %le  %le  %le  %le\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
      MessageInterface::ShowMessage("   epoch = %le\n", epoch.Get());
   #endif
   if (origin == j2000Body)  
   {
      #ifdef DEBUG_TRANSLATION
         MessageInterface::ShowMessage(
               "In TranslateFromBaseSystem, origin IS the same as j2000Body\n");
      #endif
      outState = inState;
   }
   else
   {
      #ifdef DEBUG_TRANSLATION
         MessageInterface::ShowMessage(
               "In TranslateFromBaseSystem, origin IS NOT the same as j2000Body\n");
      #endif
      // currently assuming an Rvector6 - update needed later?!?!    - awaiting Darrel on this
      // compute vector from origin of of this system to origin of the base system
      Rvector6 rif =  j2000Body->GetMJ2000State(epoch) - 
                      (origin->GetMJ2000State(epoch));

	  if (this->GetBaseSystem() == "ICRF")
	  {
		  // get rotation matrix R from ICRF to FK5:
		  CoordinateConverter cc;
		  Rmatrix33 R = cc.GetRotationMatrixFromICRFToFK5(epoch);
		  // convert rif coordinate from FK5 to ICRF:
		  Rvector3 r(rif(0), rif(1), rif(2));
		  Rvector3 v(rif(3), rif(4), rif(5));
		  r = R.Transpose()*r;
		  v = R.Transpose()*v;
		  rif.Set(r(0), r(1), r(2), v(0), v(1), v(2));
	  }

      outState = inState + rif;
   }
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage("Leaving TranslateFromBaseSystem\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
//  bool  TranslateFromBaseSystem(const GmatTime &epoch, const Rvector &inState,
//                                Rvector &outState)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in the base axisSystem to 'this'
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateFromBaseSystem(const GmatTime &epoch,
                                               const Rvector &inState,
                                               Rvector &outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateFromBaseSystem, coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
      MessageInterface::ShowMessage("   inState  = %le  %le  %le  %le  %le  %le\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);
//      MessageInterface::ShowMessage("   epoch = %le\n", epoch.Get());
   #endif
   if (origin == j2000Body)  
   {
      #ifdef DEBUG_TRANSLATION
         MessageInterface::ShowMessage(
               "In TranslateFromBaseSystem, origin IS the same as j2000Body\n");
      #endif
      outState = inState;
   }
   else
   {
      #ifdef DEBUG_TRANSLATION
         MessageInterface::ShowMessage(
               "In TranslateFromBaseSystem, origin IS NOT the same as j2000Body\n");
      #endif
      // currently assuming an Rvector6 - update needed later?!?!    - awaiting Darrel on this
      // compute vector from origin of of this system to origin of the base system
      Rvector6 rif =  j2000Body->GetMJ2000State(epoch) - 
                      (origin->GetMJ2000State(epoch));

	  if (this->GetBaseSystem() == "ICRF")
	  {
		  // get rotation matrix R from ICRF to FK5:
		  CoordinateConverter cc;
		  Rmatrix33 R = cc.GetRotationMatrixFromICRFToFK5(epoch.GetMjd());
		  // convert rif coordinate from FK5 to ICRF:
		  Rvector3 r(rif(0), rif(1), rif(2));
		  Rvector3 v(rif(3), rif(4), rif(5));
		  r = R.Transpose()*r;
		  v = R.Transpose()*v;
		  rif.Set(r(0), r(1), r(2), v(0), v(1), v(2));
	  }

      outState = inState + rif;
   }
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage("Leaving TranslateFromBaseSystem\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool TranslateFromBaseSystem(const A1Mjd &epoch, const Real *inState,
//                              Real *outState)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in the base axisSystem to 'this'
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateFromBaseSystem(const A1Mjd &epoch,
                                               const Real *inState,
                                               Real *outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateFromBaseSystem (2), coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
   #endif
   if (origin == j2000Body)
   {
      #ifdef DEBUG_TRANSLATION
         MessageInterface::ShowMessage(
               "In TranslateFromBaseSystem (2), origin IS the same as j2000Body\n");
      #endif
      for (Integer i=0; i<6; i++) outState[i] = inState[i];
   }
   else
   {
      #ifdef DEBUG_TRANSLATION
         MessageInterface::ShowMessage(
               "In TranslateFromBaseSystem (2), origin IS NOT the same as j2000Body\n");
      #endif
      Rvector6 rif =  j2000Body->GetMJ2000State(epoch) - 
                      (origin->GetMJ2000State(epoch));

	  if (this->GetBaseSystem() == "ICRF")
	  {
		  // get rotation matrix R from ICRF to FK5:
		  CoordinateConverter cc;
		  Rmatrix33 R = cc.GetRotationMatrixFromICRFToFK5(epoch);
		  // convert rif coordinate from FK5 to ICRF:
		  Rvector3 r(rif(0), rif(1), rif(2));
		  Rvector3 v(rif(3), rif(4), rif(5));
		  r = R.Transpose()*r;
		  v = R.Transpose()*v;
		  rif.Set(r(0), r(1), r(2), v(0), v(1), v(2));
	  }

      #ifdef DEBUG_TRANSLATION
         MessageInterface::ShowMessage(
               "In TranslateFromBaseSystem (2), rif = %s\n",rif.ToString().c_str());
      #endif
      const Real *toRif = rif.GetDataVector();
      for (Integer i=0; i<6; i++) outState[i] = inState[i] + toRif[i];
   }
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage("Leaving TranslateFromBaseSystem (2)\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool TranslateFromBaseSystem(const GmatTime &epoch, const Real *inState,
//                              Real *outState)
//------------------------------------------------------------------------------
/**
 * This method translates the input state, in the base axisSystem to 'this'
 * axisSystem.
 *
 * @param epoch      epoch for which to do the conversion.
 * @param inState    input state to convert.
 * @param outState   output (converted) state.
 *
 * @return true if successful; false if not.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateSystem::TranslateFromBaseSystem(const GmatTime &epoch,
                                               const Real *inState,
                                               Real *outState)
{
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage(
            "In TranslateFromBaseSystem (2), coord. system is %s, origin is %s and j2000Body is %s\n",
            (axes->GetTypeName()).c_str(), (origin->GetName()).c_str(), (j2000Body->GetName()).c_str());
   #endif
   if (origin == j2000Body)
   {
      #ifdef DEBUG_TRANSLATION
         MessageInterface::ShowMessage(
               "In TranslateFromBaseSystem (2), origin IS the same as j2000Body\n");
      #endif
      for (Integer i=0; i<6; i++) outState[i] = inState[i];
   }
   else
   {
      #ifdef DEBUG_TRANSLATION
         MessageInterface::ShowMessage(
               "In TranslateFromBaseSystem (2), origin IS NOT the same as j2000Body\n");
      #endif
      Rvector6 rif =  j2000Body->GetMJ2000State(epoch) - 
                      (origin->GetMJ2000State(epoch));

	  if (this->GetBaseSystem() == "ICRF")
	  {
		  // get rotation matrix R from ICRF to FK5:
		  CoordinateConverter cc;
		  Rmatrix33 R = cc.GetRotationMatrixFromICRFToFK5(epoch.GetMjd());
		  // convert rif coordinate from FK5 to ICRF:
		  Rvector3 r(rif(0), rif(1), rif(2));
		  Rvector3 v(rif(3), rif(4), rif(5));
		  r = R.Transpose()*r;
		  v = R.Transpose()*v;
		  rif.Set(r(0), r(1), r(2), v(0), v(1), v(2));
	  }

      #ifdef DEBUG_TRANSLATION
         MessageInterface::ShowMessage(
               "In TranslateFromBaseSystem (2), rif = %s\n",rif.ToString().c_str());
      #endif
      const Real *toRif = rif.GetDataVector();
      for (Integer i=0; i<6; i++) outState[i] = inState[i] + toRif[i];
   }
   #ifdef DEBUG_TRANSLATION
      MessageInterface::ShowMessage("Leaving TranslateFromBaseSystem (2)\n");
   #endif
   return true;
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
bool  CoordinateSystem::SetCalculateRotMatrixDeriv(bool turnOn)
{
	if (axes)
		return axes->SetCalculateRotMatrixDeriv(turnOn);
	else
		throw GmatBaseException("Error: cannot turn on calculation rotation matrix derivative in Coordinate system '" + GetName() + "' due to no axes was set to this coordinate system\n");

	return false;    // for turn off
}


//---------------------------------------------------------------------------
//  void CoordinateSystem::SetForGravityUpdate(bool turnOn)
//---------------------------------------------------------------------------
/**
* Turn on/off flag to tell BodyFixedAxes to use the default body fixed frame 
* instead of a user defined one. This is important as gravity updates are
* are tied to a certain body fixed frame and if a different one is used it leads
* to incorrect results. Only works if axes is of type BodyFixedAxes. 
* 
*
* @param turnOn  flag indicates to use default body fixed frame or not
*
*/
//---------------------------------------------------------------------------
void CoordinateSystem::SetForGravityUpdate(bool turnOn)
{
   if (axes->IsOfType("BodyFixedAxes"))
      ((BodyFixedAxes*)axes)->SetForGravityUpdate(turnOn);
}
