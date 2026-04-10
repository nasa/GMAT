//$Id: LocalAlignedConstrainedAxes.cpp 11232 2013-01-16 20:46:29Z wendys-dev $
//------------------------------------------------------------------------------
//                                  LocalAlignedConstrainedAxes
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
// Created: 2013.04.29
//
/**
 * Implementation of the LocalAlignedConstrainedAxes class.
 *
 */
//------------------------------------------------------------------------------
#include <sstream>
#include <iomanip>
#include <algorithm>                    // Required by GCC 4.3
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "LocalAlignedConstrainedAxes.hpp"
#include "DynamicAxes.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateSystemException.hpp"
#include "Rvector3.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_LAC_AXES
//#define DEBUG_ROT_MATRIX
//#define DEBUG_REFERENCE_SETTING
//#define DEBUG_RENAME
//#define DEBUG_LAC_SET

#ifdef DEBUG_LAC_AXES
#include <iostream>
#define DEBUG_ROT_MATRIX 1
#define DEBUG_REFERENCE_SETTING
#define DEBUG_CS_INIT 1
static Integer visitCount = 0;
#endif

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
LocalAlignedConstrainedAxes::PARAMETER_TEXT[
                             LocalAlignedConstrainedAxesParamCount -
                             DynamicAxesParamCount] =
{
   "ReferenceObject",
   "AlignmentVectorX",
   "AlignmentVectorY",
   "AlignmentVectorZ",
   "ConstraintVectorX",
   "ConstraintVectorY",
   "ConstraintVectorZ",
   "ConstraintCoordinateSystem",
   "ConstraintReferenceVectorX",
   "ConstraintReferenceVectorY",
   "ConstraintReferenceVectorZ",
};

const Gmat::ParameterType
LocalAlignedConstrainedAxes::PARAMETER_TYPE[
                             LocalAlignedConstrainedAxesParamCount -
                             DynamicAxesParamCount] =
{
   Gmat::OBJECT_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
};


const Real LocalAlignedConstrainedAxes::MAGNITUDE_TOL   = 1.0e-9;
const Real LocalAlignedConstrainedAxes::ORTHONORMAL_TOL = 1.0e-9;


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  LocalAlignedConstrainedAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base LocalAlignedConstrainedAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 *
 */
//------------------------------------------------------------------------------
LocalAlignedConstrainedAxes::LocalAlignedConstrainedAxes(const std::string &itsName) :
   DynamicAxes("LocalAlignedConstrained",itsName),
   referenceObjName    ("Luna"),
   referenceObject     (NULL),
   constraintCSName    ("EarthMJ2000Eq"),
   constraintCS        (NULL)
{
   objectTypeNames.push_back("LocalAlignedConstrainedAxes");
   parameterCount = LocalAlignedConstrainedAxesParamCount;

   // default values for the vectors
   alignmentVector.Set(1.0, 0.0, 0.0);
   constraintVector.Set(0.0, 0.0, 1.0);
   constraintRefVector.Set(0.0, 0.0, 1.0);
}

//---------------------------------------------------------------------------
//  LocalAlignedConstrainedAxes(const std::string &itsType,
//                              const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base LocalAlignedConstrainedAxes structures
 * (constructor).
 *
 * @param itsType Type for the object.
 * @param itsName Name for the object.
 */
//---------------------------------------------------------------------------
LocalAlignedConstrainedAxes::LocalAlignedConstrainedAxes(
                             const std::string &itsType,
                             const std::string &itsName) :
   DynamicAxes(itsType,itsName),
   referenceObjName    (""),
   referenceObject     (NULL),
   constraintCSName    (""),    // should I have a default here?
   constraintCS        (NULL)
{
   objectTypeNames.push_back("LocalAlignedConstrainedAxes");
   parameterCount = LocalAlignedConstrainedAxesParamCount;

   // default values for the vectors
   alignmentVector.Set(1.0, 0.0, 0.0);
   constraintVector.Set(0.0, 0.0, 1.0);
   constraintRefVector.Set(0.0, 0.0, 1.0);
}

//---------------------------------------------------------------------------
//  LocalAlignedConstrainedAxes(const LocalAlignedConstrainedAxes &copy);
//---------------------------------------------------------------------------
/**
 * Constructs base LocalAlignedConstrainedAxes structures used in derived classes,
 * by copying the input instance (copy constructor).
 *
 * @param copy  LocalAlignedConstrainedAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
LocalAlignedConstrainedAxes::LocalAlignedConstrainedAxes(
                             const LocalAlignedConstrainedAxes &copy) :
   DynamicAxes(copy),
   referenceObjName    (copy.referenceObjName),
   referenceObject     (copy.referenceObject),
   constraintCSName    (copy.constraintCSName),
   constraintCS        (copy.constraintCS),
   alignmentVector     (copy.alignmentVector),
   constraintVector    (copy.constraintVector),
   constraintRefVector (copy.constraintRefVector)
{
   parameterCount = LocalAlignedConstrainedAxesParamCount;
}

//---------------------------------------------------------------------------
//  LocalAlignedConstrainedAxes& operator=(
//                               const LocalAlignedConstrainedAxes &copy)
//---------------------------------------------------------------------------
/**
 * Assignment operator for LocalAlignedConstrainedAxes structures.
 *
 * @param copy The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const LocalAlignedConstrainedAxes& LocalAlignedConstrainedAxes::operator=(
                                   const LocalAlignedConstrainedAxes &copy)
{
   if (&copy == this)
      return *this;
   DynamicAxes::operator=(copy);
   referenceObjName    = copy.referenceObjName;
   referenceObject     = copy.referenceObject;
   constraintCSName    = copy.constraintCSName;
   constraintCS        = copy.constraintCS;
   alignmentVector     = copy.alignmentVector;
   constraintVector    = copy.constraintVector;
   constraintRefVector = copy.constraintRefVector;
   return *this;
}

//---------------------------------------------------------------------------
//  ~LocalAlignedConstrainedAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
LocalAlignedConstrainedAxes::~LocalAlignedConstrainedAxes()
{
}


//---------------------------------------------------------------------------
//  bool LocalAlignedConstrainedAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this LocalAlignedConstrainedAxes.
 *
 * @return success flag
 */
//---------------------------------------------------------------------------
bool LocalAlignedConstrainedAxes::Initialize()
{
   DynamicAxes::Initialize();

   if (!referenceObject)
   {
      std::string errmsg = "Cannot initialize LocalAlignedConstrained object ";
      errmsg += + "- Reference Object \"";
      errmsg += referenceObjName + "\" is not yet set, or is of an invalid type!\n";
      throw CoordinateSystemException(errmsg);
   }

   if (!constraintCS)
   {
      std::string errmsg = "Cannot initialize LocalAlignedConstrained object ";
      errmsg += + "- Coordinate System \"";
      errmsg += constraintCSName + "\" is not yet set, or is of an invalid type!\n";
      throw CoordinateSystemException(errmsg);
   }

   #ifdef DEBUG_LAC_INIT
   MessageInterface::ShowMessage
      ("LocalAlignedConstrainedAxes::Initialize() this=<%p>, refObj = %s\n",
            this, referenceObject.GetName().c_str());
   #endif

   // Make sure to initialize the reference object, if necessary
   InitializeReference(referenceObject);

   return true;
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the LocalAlignedConstrainedAxes.
 *
 * @return clone of the LocalAlignedConstrainedAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* LocalAlignedConstrainedAxes::Clone() const
{
   return (new LocalAlignedConstrainedAxes(*this));
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
GmatCoordinate::ParameterUsage
                LocalAlignedConstrainedAxes::UsesReferenceObject() const
{
   return GmatCoordinate::REQUIRED;
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
SpacePoint* LocalAlignedConstrainedAxes::GetReferenceObject() const
{
   return referenceObject;
}

//------------------------------------------------------------------------------
// virtual std::string GetReferenceObjectName() const
//------------------------------------------------------------------------------
std::string LocalAlignedConstrainedAxes::GetReferenceObjectName() const
{
   return referenceObjName;
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
void LocalAlignedConstrainedAxes::SetReferenceObject(SpacePoint *refObj)
{
   referenceObject  = refObj;
   referenceObjName = refObj->GetName();
   #ifdef DEBUG_REFERENCE_SETTING
      MessageInterface::ShowMessage("Just set reference object for LAC <%p> to %s\n",
            this, referenceObjName.c_str());
   #endif
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
 */
//---------------------------------------------------------------------------
bool LocalAlignedConstrainedAxes::UsesSpacecraft(
                                  const std::string &withName) const
{
   if (DynamicAxes::UsesSpacecraft(withName)) return true;

   // Check reference object and constraint coordinate system as well
   if (referenceObject && referenceObject->IsOfType("Spacecraft"))
   {
      if ((withName == "") || (referenceObject->GetName() == withName))
         return true;
   }
   if (constraintCS && constraintCS->UsesSpacecraft(withName))
      return true;

   return false;
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
std::string LocalAlignedConstrainedAxes::GetParameterText(const Integer id) const
{
   if (id >= DynamicAxesParamCount && id < LocalAlignedConstrainedAxesParamCount)
      return PARAMETER_TEXT[id - DynamicAxesParamCount];
   return DynamicAxes::GetParameterText(id);
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
Integer LocalAlignedConstrainedAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = DynamicAxesParamCount;
        i < LocalAlignedConstrainedAxesParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - DynamicAxesParamCount])
         return i;
   }

   return DynamicAxes::GetParameterID(str);
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
Gmat::ParameterType LocalAlignedConstrainedAxes::GetParameterType(
                                                 const Integer id) const
{
   if (id >= DynamicAxesParamCount && id < LocalAlignedConstrainedAxesParamCount)
      return PARAMETER_TYPE[id - DynamicAxesParamCount];

   return DynamicAxes::GetParameterType(id);
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
std::string LocalAlignedConstrainedAxes::GetParameterTypeString(
                                         const Integer id) const
{
   return DynamicAxes::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real LocalAlignedConstrainedAxes::GetRealParameter(const Integer id) const
{
   if (id == ALIGNMENT_VECTOR_X)       return alignmentVector[0];
   if (id == ALIGNMENT_VECTOR_Y)       return alignmentVector[1];
   if (id == ALIGNMENT_VECTOR_Z)       return alignmentVector[2];

   if (id == CONSTRAINT_VECTOR_X)      return constraintVector[0];
   if (id == CONSTRAINT_VECTOR_Y)      return constraintVector[1];
   if (id == CONSTRAINT_VECTOR_Z)      return constraintVector[2];

   if (id == CONSTRAINT_REF_VECTOR_X)  return constraintRefVector[0];
   if (id == CONSTRAINT_REF_VECTOR_Y)  return constraintRefVector[1];
   if (id == CONSTRAINT_REF_VECTOR_Z)  return constraintRefVector[2];

   return DynamicAxes::GetRealParameter(id);

}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real LocalAlignedConstrainedAxes::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <id>    ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return  Real value of the requested parameter.
 *
 * @note Assumes that these are called before initialization and the execution
 *       of the mission sequence.
 *
 */
//------------------------------------------------------------------------------
Real LocalAlignedConstrainedAxes::SetRealParameter(const Integer id,
                                                   const Real value)
{
   if (id == ALIGNMENT_VECTOR_X)
   {
      alignmentVector[0] = value;
      return true;
   }
   if (id == ALIGNMENT_VECTOR_Y)
   {
      alignmentVector[1] = value;
      return true;
   }
   if (id == ALIGNMENT_VECTOR_Z)
   {
      alignmentVector[2] = value;
      return true;
   }

   if (id == CONSTRAINT_VECTOR_X)
   {
      constraintVector[0] = value;
      return true;
   }
   if (id == CONSTRAINT_VECTOR_Y)
   {
      constraintVector[1] = value;
      return true;
   }
   if (id == CONSTRAINT_VECTOR_Z)
   {
      constraintVector[2] = value;
      return true;
   }

   if (id == CONSTRAINT_REF_VECTOR_X)
   {
      constraintRefVector[0] = value;
      return true;
   }
   if (id == CONSTRAINT_REF_VECTOR_Y)
   {
      constraintRefVector[1] = value;
      return true;
   }
   if (id == CONSTRAINT_REF_VECTOR_Z)
   {
      constraintRefVector[2] = value;
      return true;
   }

   return DynamicAxes::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 */
//------------------------------------------------------------------------------
Real LocalAlignedConstrainedAxes::SetRealParameter(const std::string &label,
                                                   const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id  ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string LocalAlignedConstrainedAxes::GetStringParameter(
                                         const Integer id) const
{
   if (id == REFERENCE_OBJECT)      return referenceObjName;
   if (id == CONSTRAINT_COORDSYS)   return constraintCSName;

   return DynamicAxes::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id     ID for the requested parameter.
 * @param value  string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool LocalAlignedConstrainedAxes::SetStringParameter(const Integer id,
                                                     const std::string &value)
{
   #ifdef DEBUG_LAC_SET
   MessageInterface::ShowMessage
      ("LocalAlignedConstrainedAxes::SetStringParameter() entered, id=%d, value='%s'\n",
       id, value.c_str());
   #endif
   if (!allowModify)
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += coordName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
   if (id == REFERENCE_OBJECT)
   {
      referenceObjName = value;
      return true;
   }
   if (id == CONSTRAINT_COORDSYS)
   {
      if (constraintCSName == coordName)
      {
         std::string errmsg = "Cannot set coordinate system ";
         errmsg += constraintCSName + " as the constraint coordinate system ";
         errmsg += "on itself.\n";
         throw CoordinateSystemException(errmsg);
      }
      constraintCSName = value;
      return true;
   }

   return DynamicAxes::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    label  label ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string LocalAlignedConstrainedAxes::GetStringParameter(
                                         const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    label  Integer ID for the parameter
 * @param    value  The new value for the parameter
 */
//------------------------------------------------------------------------------

bool LocalAlignedConstrainedAxes::SetStringParameter(const std::string &label,
                                                     const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
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
const ObjectTypeArray& LocalAlignedConstrainedAxes::GetRefObjectTypeArray()
{
   // Default is none
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   refObjectTypes.push_back(Gmat::COORDINATE_SYSTEM);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the LocalAlignedConstrainedAxes class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* LocalAlignedConstrainedAxes::GetRefObject(const UnsignedInt type,
                                                    const std::string &name)
{
   switch (type)
   {
      case Gmat::SPACE_POINT:
         if ((referenceObject) && (name == referenceObjName))
            return referenceObject;
         break;
      case Gmat::COORDINATE_SYSTEM:
         if ((constraintCS) && (name == constraintCSName))
            return constraintCS;
      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return DynamicAxes::GetRefObject(type, name);
}

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
 * @return The name(s) of the reference object(s) of the specified type
 */
//------------------------------------------------------------------------------
const StringArray& LocalAlignedConstrainedAxes::GetRefObjectNameArray(
                                                const UnsignedInt type)
{
   static StringArray refs;
   refs.clear();
   refs = DynamicAxes::GetRefObjectNameArray(type);

   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
   {
      if (find(refs.begin(), refs.end(), referenceObjName) == refs.end())
         refs.push_back(referenceObjName);
   }
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::COORDINATE_SYSTEM)
   {
      if (find(refs.begin(), refs.end(), constraintCSName) == refs.end())
         refs.push_back(constraintCSName);
   }

   #ifdef DEBUG_REFERENCE_SETTING
      MessageInterface::ShowMessage("+++ LocalAlignedConstrained::ReferenceObjects:\n");
      for (StringArray::iterator i = refs.begin(); i != refs.end(); ++i)
         MessageInterface::ShowMessage("   %s\n", i->c_str());
   #endif

   return refs;

   // Not handled here -- invoke the next higher GetRefObject call
//   return DynamicAxes::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the LocalAlignedConstrainedAxes class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool LocalAlignedConstrainedAxes::SetRefObject(GmatBase *obj,
                                               const UnsignedInt type,
                                               const std::string &name)
{
   #ifdef DEBUG_REFERENCE_SETTING
      MessageInterface::ShowMessage(
         "Entering SetRefObject for LAC <%p> with type = %d and name = %s\n",
         this, (Integer) type, name.c_str());
      MessageInterface::ShowMessage("coordName for this LAC is \"%s\"", coordName.c_str());
   #endif
   if (obj->IsOfType("CoordinateSystem"))
   {
      if (name == constraintCSName)
      {
         if ((obj == this) || (name == coordName))
         {
            std::string errmsg = "Cannot set coordinate system ";
            errmsg += coordName + " as the constraint coordinate system ";
            errmsg += "on itself.\n";
            throw CoordinateSystemException(errmsg);
         }
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage(
               "Setting %s as constraint CS object for %s\n",
               name.c_str(), coordName.c_str());
         #endif
         constraintCS = (CoordinateSystem*) obj;
      }
      return true;
   }
   if (obj->IsOfType("SpacePoint"))
   {
      if (name == referenceObjName)
      {
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage(
               "Setting %s as reference object for %s\n",
               name.c_str(), coordName.c_str());
         #endif
         referenceObject = (SpacePoint*) obj;
      }
      // do not return here; instead, pass up to ancestor classes so that
      // origin and j2000Body can be set
   }

   // Not handled here -- invoke the next higher SetRefObject call
   return DynamicAxes::SetRefObject(obj, type, name);
}

//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName,
//                       const std::string &newName)
//---------------------------------------------------------------------------
bool LocalAlignedConstrainedAxes::RenameRefObject(const UnsignedInt type,
                                                  const std::string &oldName,
                                                  const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("LocalAlignedConstrainedAxes::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif

   if (referenceObjName == oldName)
      referenceObjName = newName;

   if (constraintCSName == oldName)
      constraintCSName = newName;

   return DynamicAxes::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//---------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the LocalAlignedConstrainedAxes system.
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 */
//---------------------------------------------------------------------------
void LocalAlignedConstrainedAxes::CalculateRotationMatrix(
                                  const A1Mjd &atEpoch,
                                  bool forceComputation)
{
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage(
            "Entering CalculateRotM for LAC for time %le and forceComp = %s\n",
            atEpoch.Get(), (forceComputation? "true" : "false"));
   #endif
   if (!referenceObject)
      throw CoordinateSystemException("Reference Object \"" +
         referenceObjName +
         "\" is not yet set, or is of an invalid type, in local aligned constrained coordinate system!");

   if (!constraintCS)
      throw CoordinateSystemException("Constraint Coordinate System \"" +
            constraintCSName +
            "\" is not yet set, or is of an invalid type, in local aligned constrained coordinate system!");

   // alignment and constraint vectors are user-input
   // alignmentVector                           // av_B
   // constraintVector                          // cv_B
   // The dot vectors are both zero vectors
   Rvector3 avdot_B(0.0,0.0,0.0);
   Rvector3 cvdot_B(0.0,0.0,0.0);

   // Compute the state of the reference body with respect to the origin
   Rvector6 originState = origin->GetMJ2000State(atEpoch);
   Rvector6 refObjState = referenceObject->GetMJ2000State(atEpoch);
   Rvector3 av_I        = refObjState.GetR() - originState.GetR();
   Rvector3 avdot_I     = refObjState.GetV() - originState.GetV();

   Rvector6 constraint_B(constraintRefVector[0], constraintRefVector[1],
                         constraintRefVector[2], 0.0, 0.0, 0.0);

   Rvector constraint_I(6);
   constraint_I  = constraintCS->ToBaseSystem(atEpoch, constraint_B);
   Rvector3 cv_I(constraint_I[0],constraint_I[1],constraint_I[2]);
   Rvector3 cvdot_I(constraint_I[3],constraint_I[4],constraint_I[5]);

   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage(
            "   av_B = %s\n"
            "   cv_B = %s\n",  alignmentVector.ToString().c_str(), constraintVector.ToString().c_str());
      MessageInterface::ShowMessage(
            "   avdot_B = %s\n"
            "   cvdot_B = %s\n",  avdot_B.ToString().c_str(), cvdot_B.ToString().c_str());
      MessageInterface::ShowMessage(
            "   av_I    = %s\n"
            "   avdot_I = %s\n",  av_I.ToString().c_str(), avdot_I.ToString().c_str());
      MessageInterface::ShowMessage(
            "   cv_I    = %s\n"
            "   cvdot_I = %s\n",  cv_I.ToString().c_str(), cvdot_I.ToString().c_str());
   #endif

   // Use triad algorithm to compute R and Rdot
   Real a_B = alignmentVector.GetMagnitude();
   Real a_I = av_I.GetMagnitude();
   Real c_B = constraintVector.GetMagnitude();
   Real c_I = cv_I.GetMagnitude();
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage(
            "   a_B = %le\n"
            "   a_I = %le\n",  a_B, a_I);
      MessageInterface::ShowMessage(
            "   c_B = %le\n"
            "   c_I = %le\n",  c_B, c_I);
   #endif

   // check for divide-by-zero
   if ((GmatMathUtil::IsZero(a_B, MAGNITUDE_TOL)) || (GmatMathUtil::IsZero(a_I, MAGNITUDE_TOL)) ||
       (GmatMathUtil::IsZero(c_B, MAGNITUDE_TOL)) || (GmatMathUtil::IsZero(c_I, MAGNITUDE_TOL)))
   {
      #ifdef DEBUG_ROT_MATRIX
         MessageInterface::ShowMessage("--- Something is close to zero!!!!!\n");
      #endif
      std::string errmsg = "Local Aligned Constrained axis system named \"";
      errmsg += coordName + "\" is undefined because at least one axis/vector is near zero in length.\n";
      throw CoordinateSystemException(errmsg);
   }

   // Compute R

   // Compute unit vectors and cross products
   Rvector3 avhat_I     = av_I/av_I.GetMagnitude();
   Rvector3 avhat_B     = alignmentVector/alignmentVector.GetMagnitude();
   Rvector3 nv_I        = Cross(av_I,cv_I);
   Real     n_I         = nv_I.GetMagnitude();
   Rvector3 nv_B        = Cross(alignmentVector,constraintVector);
   Real     n_B         = nv_B.GetMagnitude();
   // check for divide-by-zero
   if ((GmatMathUtil::IsZero(n_I, MAGNITUDE_TOL)) || (GmatMathUtil::IsZero(n_B, MAGNITUDE_TOL)))
   {
      #ifdef DEBUG_ROT_MATRIX
         MessageInterface::ShowMessage("--- Something else is close to zero!!!!!\n");
      #endif
      std::string errmsg = "Local Aligned Constrained axis system named \"";
      errmsg += coordName + "\" is undefined because at least one computed axis/vector is near zero in length.\n";
      throw CoordinateSystemException(errmsg);
   }
   Rvector3 nvhat_I     = nv_I/n_I;
   Rvector3 nvhat_B     = nv_B/n_B;
   Rvector3 avInvICross = Cross(avhat_I, nvhat_I);
   Rvector3 avBnvBCross = Cross(avhat_B, nvhat_B);
   #ifdef DEBUG_ROT_MATRIX
   MessageInterface::ShowMessage(
         "   avhat_I = %s\n"
         "   avhat_B = %s\n",  avhat_I.ToString().c_str(), avhat_B.ToString().c_str());
   MessageInterface::ShowMessage(
         "   nv_I    = %s\n"
         "   nvhat_I = %s\n",  nv_I.ToString().c_str(), nvhat_I.ToString().c_str());
   MessageInterface::ShowMessage(
         "   nv_B    = %s\n"
         "   nvhat_B = %s\n",  nv_B.ToString().c_str(), nvhat_B.ToString().c_str());
   MessageInterface::ShowMessage(
         "   avInvICross = %s\n"
         "   avBnvBCross = %s\n",
         avInvICross.ToString().c_str(), avBnvBCross.ToString().c_str());
   #endif
   Rmatrix33 R1(avhat_I[0],     nvhat_I[0],     avInvICross[0],
                avhat_I[1],     nvhat_I[1],     avInvICross[1],
                avhat_I[2],     nvhat_I[2],     avInvICross[2]);
   Rmatrix33 R2(avhat_B[0],     avhat_B[1],     avhat_B[2],
                nvhat_B[0],     nvhat_B[1],     nvhat_B[2],
                avBnvBCross[0], avBnvBCross[1], avBnvBCross[2]);
   rotMatrix   = R1 * R2;

   #ifdef DEBUG_ROT_MATRIX
   MessageInterface::ShowMessage(
         "   R1 = %s\n"
         "   R2 = %s\n",  R1.ToString().c_str(), R2.ToString().c_str());
   MessageInterface::ShowMessage(
         "   rotMatrix = %s\n", rotMatrix.ToString().c_str());
   #endif
   // Compute the time derivative of rotation matrix, Rdot
   Rvector3 avhatdot_I   = avdot_I/a_I - (avhat_I/a_I)*(avhat_I*avdot_I); // (avhat_I'*avdot_I);
   Rvector3 avhatdot_B   = avdot_B/a_B - (avhat_B/a_B)*(avhat_B*avdot_B); // (avhat_B'*avdot_B)
   Rvector3 mvdimdot_I   = Cross(avdot_I,cv_I) + Cross(av_I,cvdot_I);
   Rvector3 mvdimdot_B   = Cross(avdot_B,constraintVector) + Cross(alignmentVector,cvdot_B);
   Rvector3 mvdot_I      = mvdimdot_I/n_I - (nvhat_I/n_I)*(nvhat_I*mvdimdot_I); // (nvhat_I'*mvdimdot_I)
   Rvector3 mvdot_B      = mvdimdot_B/n_B - (nvhat_B/n_B)*(nvhat_B*mvdimdot_B); // (nvhat_B'*mvdimdot_B)
   Rvector3 ddtsvdotmv_I = Cross(avhatdot_I,nvhat_I) + Cross(avhat_I,mvdot_I);
   Rvector3 ddtsvdotmv_B = Cross(avhatdot_B,nvhat_B) + Cross(avhat_B,mvdot_B);
   #ifdef DEBUG_ROT_MATRIX
   MessageInterface::ShowMessage(
         "   avhatdot_I = %s\n"
         "   avhatdot_B = %s\n",  avhatdot_I.ToString().c_str(), avhatdot_B.ToString().c_str());
   MessageInterface::ShowMessage(
         "   mvdimdot_I = %s\n"
         "   mvdimdot_B = %s\n"
         "   mvdot_I    = %s\n"
         "   mvdot_B    = %s\n",
         mvdimdot_I.ToString().c_str(), mvdimdot_B.ToString().c_str(),
         mvdot_I.ToString().c_str(), mvdot_B.ToString().c_str());
   MessageInterface::ShowMessage(
         "   ddtsvdotmv_I = %s\n"
         "   ddtsvdotmv_B = %s\n",
         ddtsvdotmv_I.ToString().c_str(), ddtsvdotmv_B.ToString().c_str());
   #endif

   Rmatrix33 R1dot(avhatdot_I[0],     mvdot_I[0],     ddtsvdotmv_I[0],
                   avhatdot_I[1],     mvdot_I[1],     ddtsvdotmv_I[1],
                   avhatdot_I[2],     mvdot_I[2],     ddtsvdotmv_I[2]);
   Rmatrix33 R2dot(avhatdot_B[0],     avhatdot_B[1],   avhatdot_B[2],
                   mvdot_B[0],        mvdot_B[1],      mvdot_B[2],
                   ddtsvdotmv_B[0],   ddtsvdotmv_B[1], ddtsvdotmv_B[2]);

   rotDotMatrix   = (R1dot * R2) + (R1 * R2dot);
   #ifdef DEBUG_ROT_MATRIX
   MessageInterface::ShowMessage(
         "   R1dot = %s\n"
         "   R2dot = %s\n",  R1dot.ToString().c_str(), R2dot.ToString().c_str());
   MessageInterface::ShowMessage(
         "   rotDotMatrix = %s\n", rotDotMatrix.ToString().c_str());
   #endif

}
