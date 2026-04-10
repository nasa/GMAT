//$Id$
//------------------------------------------------------------------------------
//                                  ObjectReferencedAxes
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
// Created: 2005/03/02
//
/**
 * Implementation of the ObjectReferencedAxes class.
 *
 */
//------------------------------------------------------------------------------
#include <sstream>
#include <iomanip>
#include <algorithm>                    // Required by GCC 4.3
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ObjectReferencedAxes.hpp"
#include "DynamicAxes.hpp"
#include "CoordinateSystemException.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_OR_AXES
//#define DEBUG_ROT_MATRIX
//#define DEBUG_REFERENCE_SETTING
//#define DEBUG_RENAME

#ifdef DEBUG_OR_AXES
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
ObjectReferencedAxes::PARAMETER_TEXT[ObjectReferencedAxesParamCount - DynamicAxesParamCount] =
{
   "XAxis",
   "YAxis",
   "ZAxis",
   "Primary",
   "Secondary",
};

const Gmat::ParameterType
ObjectReferencedAxes::PARAMETER_TYPE[ObjectReferencedAxesParamCount - DynamicAxesParamCount] =
{
   Gmat::ENUMERATION_TYPE,
   Gmat::ENUMERATION_TYPE,
   Gmat::ENUMERATION_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::OBJECT_TYPE,
};


const Real ObjectReferencedAxes::MAGNITUDE_TOL   = 1.0e-16;
const Real ObjectReferencedAxes::ORTHONORMAL_TOL = 1.0e-14;


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  ObjectReferencedAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base ObjectReferencedAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 *
 */
//------------------------------------------------------------------------------
ObjectReferencedAxes::ObjectReferencedAxes(const std::string &itsName) :
   DynamicAxes("ObjectReferenced",itsName),
   primaryName   ("Earth"),
   secondaryName ("Luna"),
   primary       (NULL),
   secondary     (NULL),
   xAxis         (""),
   yAxis         (""),
   zAxis         ("")
{
   objectTypeNames.push_back("ObjectReferencedAxes");
   parameterCount = ObjectReferencedAxesParamCount;

   usesPrimary   = GmatCoordinate::REQUIRED;
   usesSecondary = GmatCoordinate::OPTIONAL_USE;
}

//---------------------------------------------------------------------------
//  ObjectReferencedAxes(const std::string &itsType,
//                       const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base ObjectReferencedAxes structures
 * (constructor).
 *
 * @param itsType Type for the object.
 * @param itsName Name for the object.
 */
//---------------------------------------------------------------------------
ObjectReferencedAxes::ObjectReferencedAxes(const std::string &itsType,
                                           const std::string &itsName) :
   DynamicAxes(itsType,itsName),
   primaryName   (""),
   secondaryName (""),
   primary       (NULL),
   secondary     (NULL),
   xAxis         (""),
   yAxis         (""),
   zAxis         ("")
{
   objectTypeNames.push_back("ObjectReferencedAxes");
   parameterCount = ObjectReferencedAxesParamCount;
}

//---------------------------------------------------------------------------
//  ObjectReferencedAxes(const ObjectReferencedAxes &orAxes);
//---------------------------------------------------------------------------
/**
 * Constructs base ObjectReferencedAxes structures used in derived classes,
 * by copying the input instance (copy constructor).
 *
 * @param orAxes  ObjectReferencedAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
ObjectReferencedAxes::ObjectReferencedAxes(const ObjectReferencedAxes &orAxes) :
   DynamicAxes(orAxes),
   primaryName   (orAxes.primaryName),
   secondaryName (orAxes.secondaryName),
   primary       (orAxes.primary),
   secondary     (orAxes.secondary),
   xAxis         (orAxes.xAxis),
   yAxis         (orAxes.yAxis),
   zAxis         (orAxes.zAxis)
{
   parameterCount = ObjectReferencedAxesParamCount;   
}

//---------------------------------------------------------------------------
//  ObjectReferencedAxes& operator=(const ObjectReferencedAxes &orAxes)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ObjectReferencedAxes structures.
 *
 * @param orAxes The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ObjectReferencedAxes& ObjectReferencedAxes::operator=(
                            const ObjectReferencedAxes &orAxes)
{
   if (&orAxes == this)
      return *this;
   DynamicAxes::operator=(orAxes);
   primaryName   = orAxes.primaryName;
   secondaryName = orAxes.secondaryName;
   primary       = orAxes.primary;
   secondary     = orAxes.secondary;
   xAxis         = orAxes.xAxis;
   yAxis         = orAxes.yAxis;
   zAxis         = orAxes.zAxis;
   return *this;
}

//---------------------------------------------------------------------------
//  ~ObjectReferencedAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ObjectReferencedAxes::~ObjectReferencedAxes()
{
   primary = NULL;
   secondary = NULL;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesXAxis() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//------------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ObjectReferencedAxes::UsesXAxis() const
{
   return GmatCoordinate::OPTIONAL_USE;  // or REQUIRED?????  (2 of 3 are required)
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesYAxis() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//------------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ObjectReferencedAxes::UsesYAxis() const
{
   return GmatCoordinate::OPTIONAL_USE;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesZAxis() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//------------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ObjectReferencedAxes::UsesZAxis() const
{
   return GmatCoordinate::OPTIONAL_USE;
}


//------------------------------------------------------------------------------
//  void SetPrimaryObject(SpacePoint *prim)
//------------------------------------------------------------------------------
/**
 * Method to set the primary body for this system.
 *
 * @param prim  the object to use as the primary body.
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::SetPrimaryObject(SpacePoint *prim)
{
   primary     = prim;
   primaryName = primary->GetName();
}

//------------------------------------------------------------------------------
//  void SetSecondaryObject(SpacePoint *second)
//------------------------------------------------------------------------------
/**
 * Optional method to set the secondary body for this system.
 *
 * @param second  the object to use as the secondary body.
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::SetSecondaryObject(SpacePoint *second)
{
   secondary     = second;
   secondaryName = secondary->GetName();
}

//------------------------------------------------------------------------------
//  void SetXAxis(const std::string &toValue)
//------------------------------------------------------------------------------
/**
 * Method to set the X-Axis for this system.
 *
 * @param toValue  string value representing the X-axis; allowed values are:
 *                 "R", "V", "N", "-R", "-V", "-N" (see GMAT math specs).
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::SetXAxis(const std::string &toValue)
{
   if ((toValue !=  "R") && (toValue !=  "V") && (toValue !=  "N") &&
       (toValue != "-R") && (toValue != "-V") && (toValue != "-N") &&
       (toValue !=  "r") && (toValue !=  "v") && (toValue !=  "n") &&
       (toValue != "-r") && (toValue != "-v") && (toValue != "-n") && (toValue !=  "") )
   {
      CoordinateSystemException cse;
      cse.SetDetails(errorMessageFormatUnnamed.c_str(),
                     toValue.c_str(), "XAxis", "R, -R, V, -V, N, -N");
      throw cse;
//      throw CoordinateSystemException(
//            "ObjectReferencedAxes - Invalid value for X-Axis");
   }
   xAxis = toValue;
}

//------------------------------------------------------------------------------
//  void SetYAxis(const std::string &toValue)
//------------------------------------------------------------------------------
/**
 * Method to set the Y-Axis for this system.
 *
 * @param toValue  string value representing the Y-axis; allowed values are:
 *                 "R", "V", "N", "-R", "-V", "-N" (see GMAT math specs).
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::SetYAxis(const std::string &toValue)
{
   if ((toValue !=  "R") && (toValue !=  "V") && (toValue !=  "N") &&
       (toValue != "-R") && (toValue != "-V") && (toValue != "-N") &&
       (toValue !=  "r") && (toValue !=  "v") && (toValue !=  "n") &&
       (toValue != "-r") && (toValue != "-v") && (toValue != "-n") && (toValue !=  "") )
   {
      CoordinateSystemException cse;
      cse.SetDetails(errorMessageFormatUnnamed.c_str(),
                     toValue.c_str(), "YAxis", "R, -R, V, -V, N, -N");
      throw cse;
//      throw CoordinateSystemException(
//            "ObjectReferencedAxes - Invalid value for Y-Axis");
   }
   yAxis = toValue;
}

//------------------------------------------------------------------------------
//  void SetZAxis(const std::string &toValue)
//------------------------------------------------------------------------------
/**
 * Method to set the Z-Axis for this system.
 *
 * @param toValue  string value representing the Z-axis; allowed values are:
 *                 "R", "V", "N", "-R", "-V", "-N" (see GMAT math specs).
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::SetZAxis(const std::string &toValue)
{
   if ((toValue !=  "R") && (toValue !=  "V") && (toValue !=  "N") &&
       (toValue != "-R") && (toValue != "-V") && (toValue != "-N") &&
       (toValue !=  "r") && (toValue !=  "v") && (toValue !=  "n") &&
       (toValue != "-r") && (toValue != "-v") && (toValue != "-n") && (toValue !=  "") )
   {
      CoordinateSystemException cse;
      cse.SetDetails(errorMessageFormatUnnamed.c_str(),
                     toValue.c_str(), "ZAxis", "R, -R, V, -V, N, -N");
      throw cse;
//      throw CoordinateSystemException(
//            "ObjectReferencedAxes - Invalid value for Z-Axis");
   }
   zAxis = toValue;
}

//------------------------------------------------------------------------------
//  SpacePoint* GetPrimaryObject()
//------------------------------------------------------------------------------
/**
 * Returns pointer to the primary object.
 *
 * @return pointer to primary object
 *
 */
//------------------------------------------------------------------------------
SpacePoint* ObjectReferencedAxes::GetPrimaryObject() const
{
   return primary;
}

//------------------------------------------------------------------------------
//  SpacePoint* GetSecondaryObject()
//------------------------------------------------------------------------------
/**
 * Returns pointer to the secondary object.
 *
 * @return pointer to secondary object
 *
 */
//------------------------------------------------------------------------------
SpacePoint* ObjectReferencedAxes::GetSecondaryObject() const
{
   return secondary;
}

//------------------------------------------------------------------------------
// virtual std::string GetPrimaryObjectName() const
//------------------------------------------------------------------------------
std::string ObjectReferencedAxes::GetPrimaryObjectName() const
{
   return primaryName;
}

//------------------------------------------------------------------------------
// virtual std::string GetSecondaryObjectName() const
//------------------------------------------------------------------------------
std::string ObjectReferencedAxes::GetSecondaryObjectName() const
{
   return secondaryName;
}

//------------------------------------------------------------------------------
//  std::string GetXAxis() const
//------------------------------------------------------------------------------
/**
 * Returns X axis value
 *
 * @return X axis value
 *
 */
//------------------------------------------------------------------------------
std::string ObjectReferencedAxes::GetXAxis() const
{
   return xAxis;
}

//------------------------------------------------------------------------------
//  std::string GetYAxis() const
//------------------------------------------------------------------------------
/**
 * Returns Y axis value
 *
 * @return Y axis value
 *
 */
//------------------------------------------------------------------------------
std::string ObjectReferencedAxes::GetYAxis() const
{
   return yAxis;
}

//------------------------------------------------------------------------------
//  std::string GetZAxis() const
//------------------------------------------------------------------------------
/**
 * Returns Z axis value
 *
 * @return Z axis value
 *
 */
//------------------------------------------------------------------------------
std::string ObjectReferencedAxes::GetZAxis() const
{
   return zAxis;
}

//------------------------------------------------------------------------------
//  void ResetAxes()
//------------------------------------------------------------------------------
/**
 * Resets the X, Y, and Z axes to blank
 *
 */
//------------------------------------------------------------------------------
void ObjectReferencedAxes::ResetAxes()
{
   xAxis = "";
   yAxis = "";
   zAxis = "";
}


//---------------------------------------------------------------------------
//  bool ObjectReferencedAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this ObjectReferencedAxes.
 *
 * @return success flag
 */
//---------------------------------------------------------------------------
bool ObjectReferencedAxes::Initialize()
{
   DynamicAxes::Initialize();
   
   // Setting ObjectReferencedAxes properties inside a GmatFunction does not
   // work properly if not all axes are set. All setting inside a function
   // is done by Assignment commands and this object can be initialized more
   // than one time during function execution.
   
   #ifdef DEBUG_ORA_INIT
   MessageInterface::ShowMessage
      ("ObjectReferencedAxes::Initialize() this=<%p>, xAxis='%s', yAxis='%s', "
       "zAxis='%s'\n", this, xAxis.c_str(), yAxis.c_str(), zAxis.c_str());
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
 * This method returns a clone of the ObjectReferencedAxes.
 *
 * @return clone of the ObjectReferencedAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ObjectReferencedAxes::Clone() const
{
   return (new ObjectReferencedAxes(*this));
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool ObjectReferencedAxes::RenameRefObject(const UnsignedInt type,
                                           const std::string &oldName,
                                           const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ObjectReferencedAxes::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif

   if (primaryName == oldName)
      primaryName = newName;

   if (secondaryName == oldName)
      secondaryName = newName;

   return AxisSystem::RenameRefObject(type, oldName, newName);
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
std::string ObjectReferencedAxes::GetParameterText(const Integer id) const
{
   if (id >= DynamicAxesParamCount && id < ObjectReferencedAxesParamCount)
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
Integer ObjectReferencedAxes::GetParameterID(const std::string &str) const
{
   for (Integer i = DynamicAxesParamCount; i < ObjectReferencedAxesParamCount; i++)
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
Gmat::ParameterType ObjectReferencedAxes::GetParameterType(const Integer id) const
{
   if (id >= DynamicAxesParamCount && id < ObjectReferencedAxesParamCount)
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
std::string ObjectReferencedAxes::GetParameterTypeString(const Integer id) const
{
   return DynamicAxes::PARAM_TYPE_STRING[GetParameterType(id)];
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
std::string ObjectReferencedAxes::GetStringParameter(const Integer id) const
{
   if (id == X_AXIS)                return xAxis;
   if (id == Y_AXIS)                return yAxis;
   if (id == Z_AXIS)                return zAxis;
   if (id == PRIMARY_OBJECT_NAME)   return primaryName;
   if (id == SECONDARY_OBJECT_NAME) return secondaryName;

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
bool ObjectReferencedAxes::SetStringParameter(const Integer id,
                                              const std::string &value)
{
   #ifdef DEBUG_ORA_SET
   MessageInterface::ShowMessage
      ("ObjectReferencedAxes::SetStringParameter() entered, id=%d, value='%s'\n",
       id, value.c_str());
   #endif
   bool OK = false;
   if (!allowModify)
   {
      std::string errmsg = "Modifications to built-in coordinate system ";
      errmsg += instanceName + " are not allowed.\n";
      throw CoordinateSystemException(errmsg);
   }
   if ((UsesXAxis() != GmatCoordinate::NOT_USED) && (id == X_AXIS))
   {
      SetXAxis(value);
      OK = true;
   }
   if ((UsesYAxis() != GmatCoordinate::NOT_USED) && (id == Y_AXIS))
   {
      SetYAxis(value);
      OK = true;
   }
   if ((UsesZAxis() != GmatCoordinate::NOT_USED) && (id == Z_AXIS))
   {
      SetZAxis(value);
      OK = true;
   }
   if ((usesPrimary != GmatCoordinate::NOT_USED) &&
       (usesPrimary != GmatCoordinate::REQUIRED_UNMODIFIABLE) &&
       (id == PRIMARY_OBJECT_NAME))
   {
      primaryName = value;
      OK = true;
   }
   if ((usesSecondary != GmatCoordinate::NOT_USED) &&
       (usesSecondary != GmatCoordinate::REQUIRED_UNMODIFIABLE) &&
       (id == SECONDARY_OBJECT_NAME))
   {
      secondaryName = value;
      OK = true;
   }
   if (OK) return true;

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
std::string ObjectReferencedAxes::GetStringParameter(
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

bool ObjectReferencedAxes::SetStringParameter(const std::string &label,
                                              const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the ObjectReferencedAxes class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ObjectReferencedAxes::GetRefObject(const UnsignedInt type,
                                             const std::string &name)
{
   switch (type)
   {
      case Gmat::SPACE_POINT:
         if ((primary) && (name == primaryName))     return primary;
         if ((secondary) && (name == secondaryName)) return secondary;
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
const StringArray& ObjectReferencedAxes::GetRefObjectNameArray(const UnsignedInt type)
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
   {
      static StringArray refs;
      refs.clear();
      refs = DynamicAxes::GetRefObjectNameArray(type);

      if (find(refs.begin(), refs.end(), primaryName) == refs.end())
         refs.push_back(primaryName);
      if (find(refs.begin(), refs.end(), secondaryName) == refs.end())
         refs.push_back(secondaryName);
      if (find(refs.begin(), refs.end(), originName) == refs.end())
         refs.push_back(originName);
      if (find(refs.begin(), refs.end(), j2000BodyName) == refs.end())
         refs.push_back(j2000BodyName);

      #ifdef DEBUG_REFERENCE_SETTING
         MessageInterface::ShowMessage("+++ReferenceObjects:\n");
         for (StringArray::iterator i = refs.begin(); i != refs.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif

      return refs;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return DynamicAxes::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the ObjectReferencedAxes class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool ObjectReferencedAxes::SetRefObject(GmatBase *obj,
                                        const UnsignedInt type,
                                        const std::string &name)
{
   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      if (name == primaryName)
      {
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage("Setting %s as primary for %s\n",
               name.c_str(), instanceName.c_str());
         #endif
         primary = (SpacePoint*) obj;
      }
      if (name == secondaryName)
      {
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage("Setting %s as secondary for %s\n",
               name.c_str(), instanceName.c_str());
         #endif
         secondary = (SpacePoint*) obj;
      }
      // pass up to ancestor classes so that origin and j2000Body can be set
   }

   // Not handled here -- invoke the next higher SetRefObject call
   return DynamicAxes::SetRefObject(obj, type, name);
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
 * from/to this AxisSystem to/from the ObjectReferencedAxes system.
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 */
//---------------------------------------------------------------------------
void ObjectReferencedAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                                   bool forceComputation)
{
#ifdef DEBUG_ROT_MATRIX
   if (visitCount == 0)
   {
      std::stringstream ss;
      ss.precision(30);
      ss << "xAxis = \"" << xAxis << "\" and yAxis = \"" << yAxis;
      ss << "\" and zAxis = \"" << zAxis << "\"" << std::endl;

      MessageInterface::ShowMessage("%s\n", ss.str().c_str());
//      visitCount++;
   }
#endif
   if (!primary)
      throw CoordinateSystemException("Primary \"" + primaryName +
         "\" is not yet set in object referenced coordinate system!");

   if (!secondary)
      throw CoordinateSystemException("Secondary \"" + secondaryName +
         "\" is not yet set in object referenced coordinate system!");

   
   if ((xAxis == yAxis) || (xAxis == zAxis) || (yAxis == zAxis))
   {
      CoordinateSystemException cse;
      cse.SetDetails("For object referenced axes, axes are improperly "
                     "defined.\nXAxis = '%s', YAxis = '%s', ZAxis = '%s'",
                     xAxis.c_str(), yAxis.c_str(), zAxis.c_str());
      throw cse;
   }
   
   if ((xAxis != "") && (yAxis != "") && (zAxis != ""))
   {
      CoordinateSystemException cse;
      cse.SetDetails("For object referenced axes, too many axes are defined.\n"
                     "XAxis = '%s', YAxis = '%s', ZAxis = '%s'",
                     xAxis.c_str(), yAxis.c_str(), zAxis.c_str());
      throw cse;
   }
   
   SpacePoint *useAsSecondary = secondary;
//   if (!useAsSecondary)  useAsSecondary = origin;
   Rvector6 rv     = useAsSecondary->GetMJ2000State(atEpoch) -
                     primary->GetMJ2000State(atEpoch);
   #ifdef DEBUG_ROT_MATRIX
      if (visitCount == 0)
      {
         MessageInterface::ShowMessage(" ------------ rv Primary (%s) to Secondary (%s) = %s\n",
               primary->GetName().c_str(), secondary->GetName().c_str(), rv.ToString().c_str());
//         visitCount++;
      }
   #endif

   #ifdef DEBUG_ROT_MATRIX
      if (visitCount == 0)
      {
         std::stringstream ss;
         ss.precision(30);
         ss << " ----------------- rv Earth to Moon (truncated)    = "
              << rv << std::endl;

         MessageInterface::ShowMessage("%s\n", ss.str().c_str());
//         visitCount++;
      }
   #endif

   Rvector3 a     =  useAsSecondary->GetMJ2000Acceleration(atEpoch) -
                     primary->GetMJ2000Acceleration(atEpoch);
   
   Rvector3 r      = rv.GetR();
   Rvector3 v      = rv.GetV();
   Rvector3 n     =  Cross(r,v);

   // This set up of the spacecraft' acceleration to the same value in
   // the GMAT version without CCSDS_OEM_v2_EphemerisPropagator branch
   a[0] = a[1] = a[2] = 0.0;

#ifdef DEBUG_ROT_MATRIX
   MessageInterface::ShowMessage(" r = [%.15lf   %.15lf   %.15lf]\n", r[0], r[1], r[2]);
   MessageInterface::ShowMessage(" v = [%.15lf   %.15lf   %.15lf]\n", v[0], v[1], v[2]);
   MessageInterface::ShowMessage(" n = [%.15lf   %.15lf   %.15lf]\n", n[0], n[1], n[2]);
   MessageInterface::ShowMessage(" a = [%.15lf   %.15lf   %.15lf]\n", a[0], a[1], a[2]);
#endif

   Rvector3 rUnit = r.GetUnitVector();
   Rvector3 vUnit = v.GetUnitVector();
   Rvector3 nUnit = n.GetUnitVector();
   Real     rMag  = r.GetMagnitude();
   Real     vMag  = v.GetMagnitude();
   Real     nMag  = n.GetMagnitude();
   // check for divide-by-zero
   if ((GmatMathUtil::IsEqual(rMag, MAGNITUDE_TOL)) || (GmatMathUtil::IsEqual(vMag, MAGNITUDE_TOL)) || (GmatMathUtil::IsEqual(nMag, MAGNITUDE_TOL)))
   {
      std::string errmsg = "Object referenced axis system named \"";
      errmsg += coordName + "\" is undefined because at least one axis is near zero in length.\n";
      throw CoordinateSystemException(errmsg);
   }

   Rvector3 rDot  = (v / rMag) - (rUnit / rMag) * (rUnit * v);
   Rvector3 vDot  = (a / vMag) - (vUnit / vMag) * (vUnit * a);
   Rvector3 nDot = (Cross(r,a) / nMag) - (nUnit / nMag) * (Cross(r,a) * nUnit);
   Rvector3 xUnit, yUnit, zUnit, xDot, yDot, zDot;
   bool     xUsed = true, yUsed = true, zUsed = true;


   // determine the x-axis
   if ((xAxis == "R") || (xAxis == "r"))
   {
      xUnit = rUnit;
      xDot  = rDot;
   }
   else if ((xAxis == "-R") || (xAxis == "-r"))
   {
      xUnit = -rUnit;
      xDot  = -rDot;
   }
   else if ((xAxis == "V") || (xAxis == "v"))
   {
      xUnit = vUnit;
      xDot  = vDot;
   }
   else if ((xAxis == "-V") || (xAxis == "-v"))
   {
      xUnit = -vUnit;
      xDot  = -vDot;
   }
   else if ((xAxis == "N") || (xAxis == "n"))
   {
      xUnit = nUnit;
      xDot  = nDot;
   }
   else if ((xAxis == "-N") || (xAxis == "-n"))
   {
      xUnit = -nUnit;
      xDot  = -nDot;
   }
   else
   {
      xUsed = false;
   }
   // determine the y-axis
   if ((yAxis == "R") || (yAxis == "r"))
   {
      yUnit = rUnit;
      yDot  = rDot;
   }
   else if ((yAxis == "-R") || (yAxis == "-r"))
   {
      yUnit = -rUnit;
      yDot  = -rDot;
   }
   else if ((yAxis == "V") || (yAxis == "v"))
   {
      yUnit = vUnit;
      yDot  = vDot;
   }
   else if ((yAxis == "-V") || (yAxis == "-v"))
   {
      yUnit = -vUnit;
      yDot  = -vDot;
   }
   else if ((yAxis == "N") || (yAxis == "n"))
   {
      yUnit = nUnit;
      yDot  = nDot;
   }
   else if ((yAxis == "-N") || (yAxis == "-n"))
   {
      yUnit = -nUnit;
      yDot  = -nDot;
   }
   else
   {
      yUsed = false;
   }
   // determine the z-axis
   if ((zAxis == "R") || (zAxis == "r"))
   {
      zUnit = rUnit;
      zDot  = rDot;
   }
   else if ((zAxis == "-R") || (zAxis == "-r"))
   {
      zUnit = -rUnit;
      zDot  = -rDot;
   }
   else if ((zAxis == "V") || (zAxis == "v"))
   {
      zUnit = vUnit;
      zDot  = vDot;
   }
   else if ((zAxis == "-V") || (zAxis == "-v"))
   {
      zUnit = -vUnit;
      zDot  = -vDot;
   }
   else if ((zAxis == "N") || (zAxis == "n"))
   {
      zUnit = nUnit;
      zDot  = nDot;
   }
   else if ((zAxis == "-N") || (zAxis == "-n"))
   {
      zUnit = -nUnit;
      zDot  = -nDot;
   }
   else
   {
      zUsed = false;
   }

   #ifdef DEBUG_ROT_MATRIX
      if (visitCount == 0)
      {
         std::stringstream ss;
         ss.precision(30);
         ss << "xUsed = \"" << (xUsed? "true" : "false") << "\" and yUsed = \"" << (yUsed? "true" : "false");
         ss << "\" and zUsed = \"" << (zUsed? "true" : "false") << "\"" << std::endl;

         MessageInterface::ShowMessage("%s\n", ss.str().c_str());
         visitCount++;
      }
   #endif

   // determine the third axis
   if (xUsed && yUsed && !zUsed)
   {
      zUnit = Cross(xUnit, yUnit);
      zDot  = Cross(xDot, yUnit) + Cross(xUnit, yDot);
   }
   else if (xUsed && zUsed && !yUsed)
   {
      yUnit = Cross(zUnit,xUnit);
      yDot  = Cross(zDot, xUnit) + Cross(zUnit, xDot);
   }
   else if (yUsed && zUsed && !xUsed)
   {
      xUnit = Cross(yUnit,zUnit);
      xDot  = Cross(yDot, zUnit) + Cross(yUnit, zDot);
   }
   else
   {
      throw CoordinateSystemException(
            "Object referenced axes are improperly defined.\n");
   }
   // Compute the rotation matrix
   rotMatrix(0,0) = xUnit(0);
   rotMatrix(0,1) = yUnit(0);
   rotMatrix(0,2) = zUnit(0);
   rotMatrix(1,0) = xUnit(1);
   rotMatrix(1,1) = yUnit(1);
   rotMatrix(1,2) = zUnit(1);
   rotMatrix(2,0) = xUnit(2);
   rotMatrix(2,1) = yUnit(2);
   rotMatrix(2,2) = zUnit(2);

   // Compute the rotation derivative matrix
   rotDotMatrix(0,0) = xDot(0);
   rotDotMatrix(0,1) = yDot(0);
   rotDotMatrix(0,2) = zDot(0);
   rotDotMatrix(1,0) = xDot(1);
   rotDotMatrix(1,1) = yDot(1);
   rotDotMatrix(1,2) = zDot(1);
   rotDotMatrix(2,0) = xDot(2);
   rotDotMatrix(2,1) = yDot(2);
   rotDotMatrix(2,2) = zDot(2);

   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage
         ("rotMatrix=%s\n", rotMatrix.ToString().c_str());

      std::stringstream ss;

      ss.setf(std::ios::fixed);
      ss.precision(30);
      ss << " ----------------- rotMatrix    = " << rotMatrix << std::endl;
      ss.setf(std::ios::scientific);
      ss << " ----------------- rotDotMatrix = " << rotDotMatrix << std::endl;

      MessageInterface::ShowMessage("%s\n", ss.str().c_str());
   #endif

   if (!rotMatrix.IsOrthonormal(ORTHONORMAL_TOL))
   {
      std::stringstream errmsg("");
      errmsg << "*** WARNING*** Object referenced axis system \"" << coordName;
      errmsg << "\" has a non-orthogonal rotation matrix. " << std::endl;
   }
}
