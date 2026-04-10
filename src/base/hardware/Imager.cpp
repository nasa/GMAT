//$Id$
//------------------------------------------------------------------------------
//                           Imager
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 17, 2021
/**
 * 
 */
//------------------------------------------------------------------------------

#include "Imager.hpp"
#include "HardwareException.hpp"
#include "CustomFOV.hpp"

/// Labels used for the hardware element parameters.
const std::string
Imager::PARAMETER_TEXT[ImagerParamCount - HardwareParamCount] =
{
   "FieldOfView",
   "MaskConeAngles",
   "MaskClockAngles",
   "NAIFId",
};


/// Types of the parameters used by all hardware elements.
const Gmat::ParameterType
Imager::PARAMETER_TYPE[ImagerParamCount - HardwareParamCount] =
{
   Gmat::OBJECT_TYPE,    // FieldOfView
   Gmat::RVECTOR_TYPE,   // MaskConeAngles
   Gmat::RVECTOR_TYPE,   // MaskClockAngles
   Gmat::INTEGER_TYPE,      // NAIFId
};

//------------------------------------------------------------------------------
//  Imager(UnsignedInt typeId, const std::string &typeStr,
//           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param typeId Core object type for the component.
 * @param typeStr String label for the actual object type for the component.
 * @param withName Name of the component.
 */
//------------------------------------------------------------------------------
Imager::Imager(UnsignedInt typeId, const std::string &ofType, const std::string &withName) :
   Hardware          (typeId, ofType, withName),
   fovIsModeled      (false),
   fov               (nullptr),
   fovName           ("")
{
   objectTypes.push_back(Gmat::IMAGER);
   objectTypeNames.push_back("Imager");
   parameterCount = ImagerParamCount;

   for (Integer i = HardwareParamCount; i < ImagerParamCount; ++i)
         parameterWriteOrder.push_back(i);
}

//------------------------------------------------------------------------------
//  Imager(std::string FOVFileName, std::string &withName)
//------------------------------------------------------------------------------
/**
 * Alternative Constructor, used for default AZ EL mask construction
 *
 * @param typeId Core object type for the component.
 * @param typeStr String label for the actual object type for the component.
 * @param withName Name of the component.
 */
 //------------------------------------------------------------------------------
Imager::Imager(std::string FOVFileName, std::string &withName) :
   Hardware(Gmat::IMAGER, "Imager", withName),
   fovIsModeled(false),
   fov(nullptr),
   fovName("")
{
   objectTypes.push_back(Gmat::IMAGER);
   objectTypeNames.push_back("Imager");
   direction[0] = 0.0;
   direction[2] = 1.0;
   secondDirection[0] = -1.0;
   secondDirection[1] = 0.0;
   fovIsModeled = true;
   fovName = withName + "Mask";
   CustomFOV maskFOV(fovName);
   maskFOV.SetStringParameter("FOVFileName",FOVFileName);
   maskFOV.Initialize();
   fov = (FieldOfView*)maskFOV.Clone();
   for (Integer i = HardwareParamCount; i < ImagerParamCount; ++i)
      parameterWriteOrder.push_back(i);
}

//------------------------------------------------------------------------------
//  ~Imager()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
Imager::~Imager()
{
}

//------------------------------------------------------------------------------
// Imager(const Imager &other)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param other The original being copied
*/
//------------------------------------------------------------------------------
Imager::Imager(const Imager &other) :
   Hardware          (other),
   fovIsModeled      (other.fovIsModeled),
   fov               (other.fov),
   fovName           (other.fovName)
{
   for (Integer i = HardwareParamCount; i < ImagerParamCount; ++i)
      parameterWriteOrder.push_back(i);
}

//------------------------------------------------------------------------------
// Imager& operator=(const Imager &other)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param other The Imager providing parameters for this one
*
* @return This Imager
*/
//------------------------------------------------------------------------------
Imager& Imager::operator=(const Imager &other)
{
   if (this != &other)
   {
      fovIsModeled = other.fovIsModeled;

      if (fov)  // Check to see if a clone
      {
         fov = nullptr;
      }
      fovName      = other.fovName;
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase *Clone() const
//------------------------------------------------------------------------------
/**
* Creates a replica of this instance
*
* @return A pointer to the new instance
*/
//------------------------------------------------------------------------------
GmatBase* Imager::Clone() const
{
   return new Imager(*this);
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
* Prepares the imager for use
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool Imager::Initialize()
{
   if (fov)
      fov->Initialize();

   // need to add a "needs initialization flag" set false at construction
   // set true if parameter changes for direction or secondDirection, and tested
   // when matrix is requested or used internally to Hardware class.
   return Hardware::Initialize();
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves the scripted name for a parameter
*
* @param id The parameter's id
*
* @return The script string
*/
//------------------------------------------------------------------------------
std::string Imager::GetParameterText(const Integer id) const
{
   if (id >= HardwareParamCount && id < ImagerParamCount)
      return PARAMETER_TEXT[id - HardwareParamCount];
   return Hardware::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//------------------------------------------------------------------------------
/**
* Retrieves the ID for a parameter
*
* @param str The script string for the parameter
*
* @return The parameter's id
*/
//------------------------------------------------------------------------------
Integer Imager::GetParameterID(const std::string &str) const
{
   for (Integer i = HardwareParamCount; i < ImagerParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - HardwareParamCount])
         return i;
   }

   return Hardware::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves the type for a parameter
*
* @param id The parameter's id
*
* @return The parameter type
*/
//------------------------------------------------------------------------------
Gmat::ParameterType Imager::GetParameterType(const Integer id) const
{
   if (id >= HardwareParamCount && id < ImagerParamCount)
      return PARAMETER_TYPE[id - HardwareParamCount];

   return Hardware::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves a string describing a parameter's type
*
* @param id The parameter's id
*
* @return The type description
*/
//------------------------------------------------------------------------------
std::string Imager::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

bool Imager::IsParameterReadOnly(const Integer id) const
{
   if ((id == MASK_CONE_ANGLES) || (id == MASK_CLOCK_ANGLES) || (id == NAIF_ID))
      return true;

   if ((id == FOV_MODEL) || (id == DIRECTION_X) ||
      (id == DIRECTION_Y) || (id == DIRECTION_Z) ||
      (id == SECOND_DIRECTION_X) || (id == SECOND_DIRECTION_Y) ||
      (id == SECOND_DIRECTION_Z) || (id == HW_ORIGIN_BCS_X) ||
      (id == HW_ORIGIN_BCS_Y) || (id == HW_ORIGIN_BCS_Z))
      return false;

   return Hardware::IsParameterReadOnly(id);
}

bool Imager::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

bool Imager::IsParameterVisible(const Integer id) const
{
   if (id == FOV_MODEL)
      return false;

   return Hardware::IsParameterVisible(id);
}

std::string Imager::GetStringParameter (const Integer id) const
{
   std::string name;

   switch (id)
   {
      case FOV_MODEL:
         if (fov)
            name = fov->GetName();
         else
            name = fovName;
            if (name == "")
               name = "UndefinedFieldOfView";
         break;

      default:
         return Hardware::GetStringParameter(id);
   }

   return name;
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
* Retrieves a string parameter
*
* @param label The parameter's scripted identifier
*
* @return The parameter data, a string
*/
//------------------------------------------------------------------------------
std::string Imager::GetStringParameter (const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value)
//------------------------------------------------------------------------------
/**
* Sets the contents of a string parameter
*
* @param id The parameter's id
* @param value The new value
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool Imager::SetStringParameter (const Integer id, const std::string &value)
{
   switch (id)
   {
      case FOV_MODEL:
         fovIsModeled = true;
         fovName = value;
         break;

      default:
         return Hardware::SetStringParameter(id, value);
   }

   return true;
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label,
//       const std::string & value)
//------------------------------------------------------------------------------
/**
* Sets the contents of a string parameter
*
* @param label The parameter's scripted identifier
* @param value The new value
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool Imager::SetStringParameter (const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real Imager::GetRealParameter(const Integer id) const
{
   return Hardware::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real Imager::SetRealParameter(const Integer id, const Real value)
{
   return Hardware::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
//  Real GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Integer parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Integer Imager::GetIntegerParameter(const Integer id) const
{
   if (id == NAIF_ID)
   {
      return naifID;
   }
   return Hardware::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
//  Real SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Integer parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
 //------------------------------------------------------------------------------
Integer Imager::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == NAIF_ID)
   {
      naifID = value;
      return naifID;
   }
   return Hardware::SetIntegerParameter(id, value);
}

//------------------------------------------------------------------------------
//  Real GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Integer parameter.
 *
 * @param label The script label for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Integer Imager::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Integer parameter.
 *
 * @param label The script label for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
 //------------------------------------------------------------------------------
Integer Imager::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real Imager::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real Imager::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value from a Real parameter array.
 *
 * @param <id> The integer ID for the parameter.
 * @param <index> The index of the desired parameter in the array.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real Imager::GetRealParameter(const Integer id, const Integer index) const
{
   if (id == MASK_CONE_ANGLES)
   {
      Rvector result = GetMaskConeAngles();
      if ((index < 0) || (index >= result.GetSize()))
         throw HardwareException("Index out-of-range for MaskConeAngles\n");
      return result[index];
   }

   if (id == MASK_CLOCK_ANGLES)
   {
      Rvector result = GetMaskClockAngles();
      if ((index < 0) || (index >= result.GetSize()))
         throw HardwareException("Index out-of-range for MaskClockAngles\n");
      return result[index];
   }

   return GmatBase::GetRealParameter(id, index);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value,
//        const Integer index)
//------------------------------------------------------------------------------
/**
 * Set the value in a Real parameter array.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The value to assign the parameter.
 * @param <index> The index of the desired parameter in the array.
 *
 * @return The parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
 //------------------------------------------------------------------------------
Real Imager::SetRealParameter(const Integer id, const Real value,
      const Integer index)
{
   return Hardware::SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Get the value for an element in a Real parameter array.
 *
 * @param label The script label for the parameter.
 * @param index The index of the parameter in the array.
 *
 * @return the parameter value
 */
//------------------------------------------------------------------------------
Real Imager::GetRealParameter(const std::string &label,
      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Get the value for an element in a Real parameter array.
 *
 * @param label The script label for the parameter.
 * @param value The value to assign to the parameter.
 * @param index The index of the parameter in the array.
 *
 * @return The parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
 //------------------------------------------------------------------------------
Real Imager::SetRealParameter(const std::string &label, const Real value,
      const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
* Retrieves a string parameter from a string array
*
* @param id The parameter's id
* @param index The index into the array
*
* @return The parameter string
*/
//------------------------------------------------------------------------------
std::string Imager::GetStringParameter(const Integer id,
      const Integer index) const
{
   return Hardware::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
* Sets a parameter value in a string array
*
* @param id The parameter's id
* @param value The new parameter value
* @param index The index of the parameter in the array
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool Imager::SetStringParameter(const Integer id, const std::string &value,
      const Integer index)
{
   return Hardware::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  const Rvector& GetRvectorParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieve the value for an Rvector parameter.
*
* @param <id> The integer ID for the parameter.
*
* @return The parameter's value.
*/
//------------------------------------------------------------------------------
const Rvector& Imager::GetRvectorParameter(const Integer id) const
{
   static Rvector result;

   if (id == MASK_CONE_ANGLES)  // retrieved from fov; cannot be set
   {
      Rvector angles = GetMaskConeAngles();
      result.SetSize(angles.GetSize());
      result = angles;
      return result;
   }

   if (id == MASK_CLOCK_ANGLES) // retrieved from fov; cannot be set
   {
      Rvector angles = GetMaskClockAngles();
      result.SetSize(angles.GetSize());
      result = angles;
      return result;
   }

   return Hardware::GetRvectorParameter(id);
}

//------------------------------------------------------------------------------
//  const Rvector& SetRvectorParameter(const Integer id,
//       const Rvector& value)
//------------------------------------------------------------------------------
/**
* Set the value for an Rvector parameter.
*
* @param <id> The integer ID for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or
*         RVECTOR_PARAMETER_UNDEFINED if the parameter id is invalid or the
*         parameter type is not Rvector.
*/
//------------------------------------------------------------------------------
const Rvector& Imager::SetRvectorParameter(const Integer id,
      const Rvector &value)
{
   return Hardware::SetRvectorParameter(id, value);
}

//------------------------------------------------------------------------------
// const  Rvector& GetRvectorParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Retrieve the value for an Rvector parameter.
*
* @param <label> The (string) label for the parameter.
*
* @return The parameter's value.
*/
//------------------------------------------------------------------------------
const Rvector& Imager::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  const Rvector& SetRvectorParameter(const std::string &label,
//       const Rvector& value)
//------------------------------------------------------------------------------
/**
* Set the value for an Rvector parameter.
*
* @param <label> The (string) label for the parameter.
* @param <value> The new parameter value.
*
* @return the parameter value at the end of this call, or
*         RVECTOR_PARAMETER_UNDEFINED if the parameter id is invalid or the
*         parameter type is not Rvector.
*/
//------------------------------------------------------------------------------
const Rvector& Imager::SetRvectorParameter(const std::string &label,
      const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}


// Reference Objects
//------------------------------------------------------------------------------
//  std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Returns the name of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.
 *
 * @return The name of the reference object.
 */
//------------------------------------------------------------------------------
std::string Imager::GetRefObjectName(const UnsignedInt type) const
{
   if (type == Gmat::FIELD_OF_VIEW)
      return fovName;

   // kick it up the chain for all other types
   return Hardware::GetRefObjectName(type);
}

//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Returns flag indicating whether GetRefObjectTypeArray() is implemented or not.
 * Currently this method is used in the Interpreter::FinalPass() for validation.
 */
//------------------------------------------------------------------------------
bool Imager::HasRefObjectTypeArray()
{
   return true;
}

//------------------------------------------------------------------------------
//  const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Returns the types of the reference object.
 *
 * @return The types of the reference object.
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Imager::GetRefObjectTypeArray()
{
   refObjectTypes.clear();

   // Current assumption: Hardware and GmatBase have no ref objects

   if (fovIsModeled)
      refObjectTypes.push_back(Gmat::FIELD_OF_VIEW);

   return refObjectTypes;
}

//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object.
 *
 * @param <type> reference object type.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& Imager::GetRefObjectNameArray(const UnsignedInt type)
{
   static StringArray fullList;  // Maintain scope if the full list is requested
   fullList.clear();

   // If type is UNKNOWN_OBJECT, add fovName anyway.
   if ( (type == Gmat::UNKNOWN_OBJECT) || (type ==Gmat::FIELD_OF_VIEW) )
   {
      // Put in the FieldOfView object name
      if (fovIsModeled)
      {
         fullList.push_back(fovName);
         #ifdef DEBUG_HW_SET
           MessageInterface::ShowMessage("pushed name '%s' onto list\n",
                                       fovName.c_str());
         #endif
      }
      #ifdef DEBUG_HW_SET
         MessageInterface::ShowMessage(
            "Exiting Hardware::GetRefObjectNameArray()\n\n");
      #endif
      return fullList;
   }
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage(
         "Returning GmatBase::GetRefObjectNameArray()\n\n");
   #endif

   return Hardware::GetRefObjectNameArray(type);
}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Returns the reference object pointer.
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return reference object pointer.
 */
//------------------------------------------------------------------------------
GmatBase* Imager::GetRefObject(const UnsignedInt type, const std::string &name)
{
   if ((type == Gmat::FIELD_OF_VIEW) && (name == fovName))
      return (GmatBase*) fov;

   return Hardware::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
//  bool SetRefObjectName(const UnsignedInt type, const char *name)
//------------------------------------------------------------------------------
/**
 * Sets the name of the reference object.
 *
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool Imager::SetRefObjectName(const UnsignedInt type, const std::string &name)
{
   if (type == Gmat::FIELD_OF_VIEW)
   {
      fovName = name;
      #ifdef DEBUG_HW_SET
         MessageInterface::ShowMessage("Exiting Hardware::SetRefObjectName(),");
         MessageInterface::ShowMessage(" fovname = '%s'\n\n");
      #endif
      return true;
   }

   // kick it up the chain for all other types
   return Hardware::SetRefObjectName(type,name);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the reference object.
 *
 * @param <obj> reference object pointer.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool Imager::SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string &name)
{
   if (obj == NULL)
      return false;

   if ( (obj->IsOfType(Gmat::FIELD_OF_VIEW)) && (fovName == name)  )
   {
      fov = (FieldOfView*) obj;
      return true;
   }

   // kick it up the chain for all other types
   return Hardware::SetRefObject(obj,type,name);
}

//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 *
 * @return true if object name changed, false if not.
 */
//------------------------------------------------------------------------------
bool Imager::RenameRefObject(const UnsignedInt type, const std::string &oldName,
      const std::string &newName)
{
   if ((type == Gmat::FIELD_OF_VIEW) && (fovName == oldName))
   {
      fovName = newName;
      return true;
   }

   return Hardware::RenameRefObject(type, oldName, newName);
}

//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name,
//       const Integer index)
//---------------------------------------------------------------------------
/**
 * Returns the reference object pointer.
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 * @param index Index into the object array.
 *
 * @return reference object pointer.
 */
//------------------------------------------------------------------------------
GmatBase* Imager::GetRefObject(const UnsignedInt type, const std::string &name,
      const Integer index)
{
   return Hardware::GetRefObject(type,name,index);
}

//---------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name, const Integer index)
//---------------------------------------------------------------------------
/**
 * Sets the reference object.
 *
 * @param obj reference object pointer.
 * @param type type of the reference object.
 * @param name name of the reference object.
 * @param index Index into the object array.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool Imager::SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string &name, const Integer index)
{
   return Hardware::SetRefObject(obj,type,name,index);
}

//---------------------------------------------------------------------------
// FieldOfView* GetFieldOfView()
//---------------------------------------------------------------------------
/**
 * Returns the FieldOfView object being used by the imager.
 *
 * @return The FieldOfView object.
 */
//------------------------------------------------------------------------------
FieldOfView* Imager::GetFieldOfView()
{
   return fov;
}


//------------------------------------------------------------------------------
//  bool CheckTargetVisibility(const Rvector3 &target)
//------------------------------------------------------------------------------
/**
 * This method determines if FOV is being modeled
 *
 * @param <target> is a unit vector in the spacecraft body frame
 *
 * @return boolean is true if the target vector points inside the FOV.
 *
 */
//------------------------------------------------------------------------------
bool Imager::CheckTargetVisibility(const Rvector3 &target)
{
   // may add use of location offset later; for now just rotation
   return fov->CheckTargetVisibility(R_SB*target);

}

//------------------------------------------------------------------------------
//  Rvector GetMaskConeAngles() const
//------------------------------------------------------------------------------
/**
 * This method returns cone angle(s)  for field of view
 * Note: see specifications for how each FOV class uses the vectors
 *
 * @return Rvector -- returns cone angle(s) for field of view
 *
 */
//------------------------------------------------------------------------------

Rvector Imager::GetMaskConeAngles() const
{
   static Rvector result;
   if (fov)
      return fov->GetMaskConeAngles();
   else
   {
      // @todo write message
      result.SetSize(0);
      return result;
   }
}

//------------------------------------------------------------------------------
//  Rvector GetMaskClockAngles() const
//------------------------------------------------------------------------------
/**
 * This method returns clock angle(s)  for field of view. This vector is empty
 * if the FOV is object in class ConicalFOV.
 * Note: see specifications for how each FOV class uses the vectors
 *
 * @return Rvector -- returns clock angle(s for field of view
 *
 */
//------------------------------------------------------------------------------
Rvector Imager::GetMaskClockAngles() const
{
   static Rvector result;
   if (fov)
      return fov->GetMaskClockAngles();
   else
   {
      // @todo write message
      result.SetSize(0);
      return result;
   }
}


//------------------------------------------------------------------------------
//  bool HasFOV()
//------------------------------------------------------------------------------
/**
 * This method determines if FOV is being modeled
 *
 * @return boolean is true if this hardware object includes a field of view
 *
 */
 //------------------------------------------------------------------------------
bool Imager::HasFOV()
{
   return fovIsModeled; //(!(fov == NULL));
}