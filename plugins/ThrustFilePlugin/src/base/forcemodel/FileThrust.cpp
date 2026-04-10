//------------------------------------------------------------------------------
//                           FileThrust
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
// Created: Jan 13, 2016
/**
 * Model for thrusts applied by a thrust history file
 */
//------------------------------------------------------------------------------

#include "FileThrust.hpp"

#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "ODEModelException.hpp"
#include "PropagationStateManager.hpp"
#include "Rmatrix33.hpp"
#include <sstream>

#ifdef __unix
// Needed to use this friend function on Linux
Rmatrix33  SkewSymmetric(const Rvector3& v);
#endif

#ifdef __APPLE__
// Needed to use this friend function on MacOS
Rmatrix33  SkewSymmetric(const Rvector3& v);
#endif

//#define DEBUG_INITIALIZATION
//#define DEBUG_FILETHRUST_EXE
//#define DEBUG_INTERPOLATION
//#define DEBUG_MASS_FLOW
//#define DEBUG_SEGMENTS
//#define DEBUG_REF_OBJECTS
//#define DEBUG_TSF_SOLVEFOR
//#define DEBUG_ANGLE_SOLVEFOR

//------------------------------------------------------------------------------
// FileThrust(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor for forces from thrust history files.
 *
 * @param name Name of the constructed instance.
 */
//------------------------------------------------------------------------------
FileThrust::FileThrust(const std::string &name) :
   PhysicalModel           (Gmat::PHYSICAL_MODEL, "FileThrust", name),
   satCount                (0),
   cartIndex               (-1),
   fillCartesian           (true),
   dataIsThrust            (true),
   massFlowWarningNeeded   (true),
   segments                (NULL),
   scriptSegments          (NULL),
   mDotIndex               (-1),
   depleteMass             (false),
   coordSystem             (NULL),
   liner                   (NULL),
   spliner                 (NULL),
   warnTooFewPoints        (true),
   estimatingTSF           (false),
   tsfEpsilonID            (-1),
   tsfEpsilonRow           (-1),
   thrustSF                (1.0),
   thrustSFinitial         (0.0)
{
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   objectTypeNames.push_back("FileThrust");

   estimatingAngles[0] = estimatingAngles[1] = false;
   thrustAngleRows[0] = thrustAngleRows[1] = -1;
}

//------------------------------------------------------------------------------
// ~FileThrust()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
FileThrust::~FileThrust()
{
   if (liner != NULL)
      delete liner;
   if (spliner != NULL)
      delete spliner;
}

//------------------------------------------------------------------------------
// FileThrust(const FileThrust& ft)
//------------------------------------------------------------------------------
/**
 * Copy constructor for forces from thrust history files.
 *
 * @param ft The original that gets copied.
 */
//------------------------------------------------------------------------------
FileThrust::FileThrust(const FileThrust& ft) :
   PhysicalModel           (ft),
   spacecraftNames         (ft.spacecraftNames),
   satCount                (ft.satCount),
   cartIndex               (ft.cartIndex),
   fillCartesian           (ft.fillCartesian),
   dataIsThrust            (ft.dataIsThrust),
   massFlowWarningNeeded   (true),
   segmentNames            (ft.segmentNames),
   segments                (ft.segments),
   scriptSegments          (ft.scriptSegments),
   mDotIndex               (ft.mDotIndex),
   depleteMass             (ft.depleteMass),
   activeTankName          (ft.activeTankName),
   csNames                 (ft.csNames),
   coordSystem             (NULL),
   liner                   (NULL),
   spliner                 (NULL),
   warnTooFewPoints        (true),
   estimatingTSF           (ft.estimatingTSF),
   tsfEpsilonID            (ft.tsfEpsilonID),
   tsfEpsilonRow           (ft.tsfEpsilonRow),
   tsfInitial              (ft.tsfInitial),
   thrustSF                (ft.thrustSF),
   thrustSFinitial         (ft.thrustSFinitial)
   {
   estimatingAngles[0] = ft.estimatingAngles[0];
   estimatingAngles[1] = ft.estimatingAngles[1];
   thrustAngleRows[0] = ft.thrustAngleRows[0];
   thrustAngleRows[1] = ft.thrustAngleRows[1];
}

//------------------------------------------------------------------------------
// FileThrust& operator=(const FileThrust& ft)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param ft Instance that provides the parameter values.
 *
 * @return This instance, set to match the input instance.
 */
//------------------------------------------------------------------------------
FileThrust& FileThrust::operator=(const FileThrust& ft)
{
   if (this != &ft)
   {
      PhysicalModel::operator=(ft);

      spacecraftNames = ft.spacecraftNames;
      satCount      = ft.satCount;
      cartIndex     = ft.cartIndex;
      fillCartesian = ft.fillCartesian;
      dataIsThrust  = ft.dataIsThrust;
      segmentNames  = ft.segmentNames;
      segments      = ft.segments;
      scriptSegments= ft.scriptSegments;
      mDotIndex     = ft.mDotIndex;
      depleteMass   = ft.depleteMass;
      activeTankName = ft.activeTankName;
      csNames       = ft.csNames;
      estimatingTSF = ft.estimatingTSF;
      tsfEpsilonID  = ft.tsfEpsilonID;
      tsfEpsilonRow = ft.tsfEpsilonRow;
      tsfInitial    = ft.tsfInitial;
      thrustSF      = ft.thrustSF;
      thrustSFinitial = ft.thrustSFinitial;
      coordSystem   = NULL;

      // Interpolator resets
      if (liner != NULL)
      {
         delete liner;
         liner = NULL;
      }
      if (spliner != NULL)
      {
         delete spliner;
         spliner = NULL;
      }

      massFlowWarningNeeded = true;
      warnTooFewPoints      = true;

      estimatingAngles[0] = ft.estimatingAngles[0];
      estimatingAngles[1] = ft.estimatingAngles[1];
      thrustAngleRows[0] = ft.thrustAngleRows[0];
      thrustAngleRows[1] = ft.thrustAngleRows[1];
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool operator==(const FileThrust& ft) const
//------------------------------------------------------------------------------
/**
 * Checks to see if two FileThrusts apply the same force.
 *
 * @param ft The second FileThrust
 *
 * @return true if the segment data are the same.
 */
//------------------------------------------------------------------------------
bool FileThrust::operator==(const FileThrust& ft) const
{
   return (segments == ft.segments);
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Overridden Clone method used to make copies from a GmatBase pointer.
 *
 * @return New instance, set to look like this one.
 */
//------------------------------------------------------------------------------
GmatBase* FileThrust::Clone() const
{
   return new FileThrust(*this);
}

//------------------------------------------------------------------------------
// void Clear(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Clears the arrays of elements that get set by the Propagate commands
 *
 * @param type The type of element that gets cleared.  Set to
 *             Gmat::UNKNOWN_OBJECT to clear all of the configrable arrays.
 */
//------------------------------------------------------------------------------
void FileThrust::Clear(const UnsignedInt type)
{
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACECRAFT))
   {
      spacecraftNames.clear();
      spacecraft.clear();
   }
}


//------------------------------------------------------------------------------
// Integer         GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Returns look-up ID for thrust segment data.
 *
 * This method overrides the GmatBase method used to look up scriptable
 * parameters.  The use here is not standard, so should not be replicated
 * elsewhere.
 *
 * @param str The string identifying the look up parameter
 *
 * @return An overloaded ID for the parameter
 */
//------------------------------------------------------------------------------
Integer FileThrust::GetParameterID(const std::string &str) const
{
   Integer retval = -1, seg, id;

   std::string::size_type loc = str.find(".");
   if (loc != std::string::npos)
   {
      std::string segName, parmName;
      segName = str.substr(0, loc);
      parmName = str.substr(loc+1);

      for (UnsignedInt i = 0; i < segments->size(); ++i)
      {
         if (segments->at(i).GetName() == segName)
         {
            seg = i;
            id = segments->at(i).GetParameterID(parmName);
            retval = 100 * (i+1) + segments->at(i).GetParameterID(parmName);
            break;
         }
      }
   }

   #ifdef DEBUG_MAPPING
      MessageInterface::ShowMessage("In FileThrust: Loaded ID for %s is %d "
            "for segment %d, parameter %d\n", str.c_str(), retval, seg, id);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter, or
 *         UNKNOWN_PARAMETER_TYPE.
 */
 //------------------------------------------------------------------------------
Gmat::ParameterType FileThrust::GetParameterType(const Integer id) const
{
   if (id > 99)
   {
      // Break ID apart into segment number and parm id
      Integer segId, parmId;
      segId = (Integer)(id / 100) - 1;
      parmId = id - 100 * (segId + 1);

      #ifdef DEBUG_MAPPING
         MessageInterface::ShowMessage("Get %d: Segment %d, parm %d\n", id,
               segId, parmId);
      #endif

      return segments->at(segId).GetParameterType(parmId);
   }

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies parameters that are not written to script
 *
 * @param id The parameter checked here
 *
 * @return true for parameters that are hidden
 *
 * @note: Since this is an internal object (a force model element), this method
 * isn't really used, but included for completeness
 */
//------------------------------------------------------------------------------
bool FileThrust::IsParameterReadOnly(const Integer id) const
{
   return true;
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Identifies parameters that are not written to script
 *
 * @param label The parameter checked here
 *
 * @return true for parameters that are hidden
 *
 * @note: Since this is an internal object (a force model element), this method
 * isn't really used, but included for completeness
 */
//------------------------------------------------------------------------------
bool FileThrust::IsParameterReadOnly(const std::string &label) const
{
   return true;
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor for floating point parameter data
 *
 * Note that the usage here is non-standard, and used for the solve-for
 * implementation so that a common interface is used.
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real FileThrust::GetRealParameter(const Integer id) const
{
   if (id > 99)
   {
      // Break ID apart into segment number and parm id
      Integer segId, parmId;
      segId = (Integer)(id / 100) - 1;
      parmId = id - 100 * (segId + 1);

      #ifdef DEBUG_MAPPING
         MessageInterface::ShowMessage("Get %d: Segment %d, parm %d\n", id,
               segId, parmId);
      #endif

      return segments->at(segId).GetRealParameter(parmId); 
   }

   return PhysicalModel::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor for floating point parameter data
 *
 * Note that the usage here is non-standard, and used for the solve-for
 * implementation so that a common interface is used.
 *
 * @param id The ID of the parameter
 * @param value The new parameter value
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real FileThrust::SetRealParameter(const Integer id, const Real value)
{
   if (id > 99)
   {
      // Break ID apart into segment number and parm id
      Integer segId, parmId;
      segId = (Integer)(id / 100) - 1;
      parmId = id - 100 * (segId + 1);

      UpdateScriptSegments(segId, parmId, value);
      return segments->at(segId).SetRealParameter(parmId, value);
   }

   return PhysicalModel::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Real FileThrust::GetRealParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id);
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
 //------------------------------------------------------------------------------
Real FileThrust::SetRealParameter(const std::string &label, const Real value)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Accessor for floating point parameter data
 *
 * Note that the usage here is non-standard, and used for the solve-for
 * implementation so that a common interface is used.
 *
 * @param id The ID of the parameter
 * @param index Index for objects in arrays
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real FileThrust::GetRealParameter(const Integer id, const Integer index) const
{
   if (id > 99)
   {
      // Break ID apart into segment number and parm id
      Integer segId, parmId;
      segId = (Integer)(id / 100) - 1;
      parmId = id - 100 * (segId + 1);

      #ifdef DEBUG_MAPPING
         MessageInterface::ShowMessage("Get %d: Segment %d, parm %d\n", id,
               segId, parmId);
      #endif

      return segments->at(segId).GetRealParameter(parmId, index); 
   }

   return PhysicalModel::GetRealParameter(id, index);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Accessor for floating point parameter data
 *
 * Note that the usage here is non-standard, and used for the solve-for
 * implementation so that a common interface is used.
 *
 * @param id The ID of the parameter
 * @param value The new parameter value
 * @param index Index for objects in arrays
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real FileThrust::SetRealParameter(const Integer id, const Real value, const Integer index)
{
   if (id > 99)
   {
      // Break ID apart into segment number and parm id
      Integer segId, parmId;
      segId = (Integer)(id / 100) - 1;
      parmId = id - 100 * (segId + 1);

      UpdateScriptSegments(segId, parmId, value, index);
      return segments->at(segId).SetRealParameter(parmId, value, index);
   }

   return PhysicalModel::SetRealParameter(id, value, index);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The (string) label for the parameter.
 * @param index Index for objects in arrays.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
Real FileThrust::GetRealParameter(const std::string &label, const Integer index) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id, index);
}


//---------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value,
//                        const Integer index)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new parameter value.
 * @param index Index for objects in arrays.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
 //------------------------------------------------------------------------------
Real FileThrust::SetRealParameter(const std::string &label, const Real value,
                                     const Integer index)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor for vector parameter data
 *
 * Note that the usage here is non-standard, and used for the solve-for
 * implementation so that a common interface is used.
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
const Rvector& FileThrust::GetRvectorParameter(const Integer id) const
{
   if (id > 99)
   {
      // Break ID apart into segment number and parm id
      Integer segId, parmId;
      segId = (Integer)(id / 100) - 1;
      parmId = id - 100 * (segId + 1);

      #ifdef DEBUG_MAPPING
         MessageInterface::ShowMessage("Get %d: Segment %d, parm %d\n", id,
               segId, parmId);
      #endif

      return segments->at(segId).GetRvectorParameter(parmId);
   }

   return PhysicalModel::GetRvectorParameter(id);
}

//------------------------------------------------------------------------------
// Rvector& SetRvectorParameter(const Integer id, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * Accessor for vector parameter data
 *
 * Note that the usage here is non-standard, and used for the solve-for
 * implementation so that a common interface is used.
 *
 * @param id The ID of the parameter
 * @param value The new parameter value
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
const Rvector& FileThrust::SetRvectorParameter(const Integer id, const Rvector &value)
{
   if (id > 99)
   {
      // Break ID apart into segment number and parm id
      Integer segId, parmId;
      segId = (Integer)(id / 100) - 1;
      parmId = id - 100 * (segId + 1);

      UpdateScriptSegments(segId, parmId, value);
      return segments->at(segId).SetRvectorParameter(parmId, value);
   }

   return PhysicalModel::SetRvectorParameter(id, value);
}

//------------------------------------------------------------------------------
//  Rvector& GetRvectorParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for an Rvector parameter.
 *
 * @param label The (string) label for the parameter.
 *
 * @return The parameter's value.
 */
 //------------------------------------------------------------------------------
const Rvector& FileThrust::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Rvector& SetRvectorParameter(const std::string &label, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * Set the value for an Rvector parameter.
 *
 * @param label The (string) label for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
 //------------------------------------------------------------------------------
const Rvector& FileThrust::SetRvectorParameter(const std::string &label, const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
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
const ObjectTypeArray& FileThrust::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::COORDINATE_SYSTEM);
   refObjectTypes.push_back(Gmat::SPACECRAFT);
   refObjectTypes.push_back(Gmat::INTERFACE);
   return refObjectTypes;
}

//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets the names for referenced objects.
 *
 * FileThrust instances use Spacecraft and (set separately) thrust segment
 * objects.  This method sets the names for the Spacecraft objects.
 *
 * @param type The type of object that has the name.
 * @param name The object's name
 *
 * @return true on success, false otherwise.
 */
//------------------------------------------------------------------------------
bool FileThrust::SetRefObjectName(const UnsignedInt type,
      const std::string& name)
{
   if (type == Gmat::SPACECRAFT)
   {
      if (find(spacecraftNames.begin(), spacecraftNames.end(), name) ==
            spacecraftNames.end())
         spacecraftNames.push_back(name);
      return true;
   }

   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (find(csNames.begin(), csNames.end(), name) == csNames.end())
         csNames.push_back(name);
      return true;
   }

   return PhysicalModel::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Accesses the names for referenced objects.
 *
 * @param type The type of object requested.
 *
 * @return The vector of names.
 */
//------------------------------------------------------------------------------
const StringArray& FileThrust::GetRefObjectNameArray(
      const UnsignedInt type)
{
   #ifdef DEBUG_REF_OBJECTS
      MessageInterface::ShowMessage("FileThrust::GetRefObjectNameArray called "
            "for type %d\n", type);
   #endif
   PhysicalModel::GetRefObjectNameArray(type);

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::UNKNOWN_OBJECT))
      refObjectNames.insert(refObjectNames.begin(), spacecraftNames.begin(),
            spacecraftNames.end());

   if ((type == Gmat::INTERFACE) || (type == Gmat::UNKNOWN_OBJECT))
      refObjectNames.insert(refObjectNames.begin(), segmentNames.begin(),
         segmentNames.end());

   if ((type == Gmat::COORDINATE_SYSTEM) || (type == Gmat::UNKNOWN_OBJECT))
      refObjectNames.insert(refObjectNames.begin(), csNames.begin(),
            csNames.end());

   #ifdef DEBUG_REF_OBJECTS
      MessageInterface::ShowMessage("  Returning %d names\n",
            refObjectNames.size());
   #endif

   return refObjectNames;
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets referenced object pointers.
 *
 * @param obj The object.
 * @param type The type of the object.
 * @param name The object's name.
 *
 * @return true if the object is set, false if not.
 */
//------------------------------------------------------------------------------
bool FileThrust::SetRefObject(GmatBase* obj, const UnsignedInt type,
      const std::string& name)
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      #ifdef DEBUG_REF_OBJECTS
         MessageInterface::ShowMessage("Setting cs named %s\n", name.c_str());
      #endif

      bool retval = false;

      if (obj->IsOfType("CoordinateSystem") == false)
         throw ODEModelException(
            "FileThrust::SetRefObject cannot use objects of type " +
            obj->GetTypeName());
      if (find(csNames.begin(), csNames.end(), name) != csNames.end())
      {
         // Pass the CS to each segment that needs it
         for (UnsignedInt i = 0; i < segments->size(); ++i)
         {
            if (segments->at(i).segData.csName == name)
            {
               segments->at(i).segData.cs = (CoordinateSystem*)obj;
               retval = true;
            }
         }
      }
      return retval;
   }

   return PhysicalModel::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets referenced object pointers in an object array.
 *
 * @param obj The object.
 * @param type The type of the object.
 * @param name The object's name.
 * @param index The index of the object in the array
 *
 * @return true if the object is set, false if not.
 */
//------------------------------------------------------------------------------
bool FileThrust::SetRefObject(GmatBase* obj, const UnsignedInt type,
      const std::string& name, const Integer index)
{
   MessageInterface::ShowMessage("Setting %s\n", name.c_str());
   return PhysicalModel::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//       const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Changes the name for reference objects
 *
 * @param type The type of the object.
 * @param oldName The object's name before the change.
 * @param newName The object's proposed name after the change.
 *
 * @return true if a change was made, false if not
 */
//------------------------------------------------------------------------------
bool FileThrust::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   if (type == Gmat::SPACECRAFT)
   {
      for (UnsignedInt i = 0; i < spacecraftNames.size(); ++i)
      {
         if (spacecraftNames[i] == oldName)
            spacecraftNames[i] = newName;
      }
      return true;
   }

   if (type == Gmat::INTERFACE)
   {
      for (UnsignedInt i = 0; i < segmentNames.size(); ++i)
      {
         if (segmentNames[i] == oldName)
            segmentNames[i] = newName;
      }
      return true;
   }

   if (type == Gmat::COORDINATE_SYSTEM)
   {
      for (UnsignedInt i = 0; i < csNames.size(); ++i)
      {
         if (csNames[i] == oldName)
            csNames[i] = newName;
      }
      return true;
   }

   return PhysicalModel::RenameRefObject(type, oldName, newName);
}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a reference object
 *
 * @param type The type of the object
 * @param name The object's name
 *
 * @return The object pointer
 */
//------------------------------------------------------------------------------
GmatBase* FileThrust::GetRefObject(const UnsignedInt type,
      const std::string& name)
{
   if (type == Gmat::SPACECRAFT)
   {
      if (name == "")
         if (spacecraft.size() > 0)
            return spacecraft[0];
      for (UnsignedInt i = 0; i < spacecraft.size(); ++i)
         if (spacecraft[i]->GetName() == name)
            return spacecraft[i];
      return NULL;
   }
   if (type == Gmat::INTERFACE)
   {
      if (name == "")
         if (segments->size() > 0)
            return &(segments->at(0));
      for (UnsignedInt i = 0; i < segments->size(); ++i)
         if (segments->at(i).GetName() == name)
            return &(segments->at(i));
      return NULL;
   }

   return PhysicalModel::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a reference object from an array
 *
 * @param type The type of the object
 * @param name The object's name
 * @param index The index of the object in the array
 *
 * @return The object pointer
 */
//------------------------------------------------------------------------------
GmatBase* FileThrust::GetRefObject(const UnsignedInt type,
      const std::string& name, const Integer index)
{
   return PhysicalModel::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
// bool IsTransient()
//------------------------------------------------------------------------------
/**
 * Specifies whether the force is transient.
 *
 * @return true for all FiniteThrust forces.
 */
//------------------------------------------------------------------------------
bool FileThrust::IsTransient()
{
   return true;
}

//------------------------------------------------------------------------------
// bool DepletesMass()
//------------------------------------------------------------------------------
/**
 * Detects mass depletion from a PhysicalModel
 *
 * @return true if the model depletes mass, false if it does not
 */
//------------------------------------------------------------------------------
bool FileThrust::DepletesMass()
{
   return depleteMass;
}

//------------------------------------------------------------------------------
// void FileThrust::SetSegmentList(std::vector<ThrustSegment> *segs)
//------------------------------------------------------------------------------
/**
 * Sets the list of thrust file segments used by the force
 *
 * @param segs The segment list
 */
//------------------------------------------------------------------------------
void FileThrust::SetSegmentList(std::vector<ThrustSegment> *segs, std::vector<ThrustSegment*> *scriptSegs)
{
   segments = segs;
   scriptSegments = scriptSegs;

   depleteMass = false;

   // Activate mass flow if any segment needs it and collect ref objects
   segmentNames.clear();
   csNames.clear();
   for (UnsignedInt i = 0; i < segments->size(); ++i)
   {
      depleteMass |= segments->at(i).DepletesMass();

      // Collect the names of all of the coordinate systems needed
      std::string name = segments->at(i).GetName();
      if (find(segmentNames.begin(), segmentNames.end(), name) == segmentNames.end())
         segmentNames.push_back(name);

      // Collect the names of all of the coordinate systems needed
      std::string csname = segments->at(i).segData.csName;
      if (find(csNames.begin(), csNames.end(), csname) == csNames.end())
         csNames.push_back(csname);
 
      // Set initial TSF
      if (i < tsfInitial.size())
      {
          segments->at(i).SetRealParameter("TSF_Epsilon", 0.0);
          thrustSF = segments->at(i).GetRealParameter("ThrustScaleFactor");
          if (tsfInitial[i] == -99999999.9999)
              tsfInitial[i] = segments->at(i).GetRealParameter("ThrustScaleFactor");
      }
      else
      {
          segments->at(i).SetRealParameter("TSF_Epsilon", 0.0);
          thrustSF = segments->at(i).GetRealParameter("ThrustScaleFactor");
          tsfInitial.push_back(segments->at(i).GetRealParameter("ThrustScaleFactor"));
      }
   }

   #ifdef DEBUG_SEGMENTS
      // Dump seg list here
      MessageInterface::ShowMessage("%d segments in the list\n", segments->size());
      for (UnsignedInt i = 0; i < segments->size(); ++i)
      {
         MessageInterface::ShowMessage("%d:  %s\n", i,
               segments->at(i).GetName().c_str());
         MessageInterface::ShowMessage("     TSF:   %lf\n",
               segments->at(i).GetRealParameter("ThrustScaleFactor"));
         MessageInterface::ShowMessage("     MFSF:  %lf\n",
               segments->at(i).GetRealParameter("MassFlowScaleFactor"));
         MessageInterface::ShowMessage("     Tanks: %d\n",
               segments->at(i).GetStringArrayParameter("MassSource").size());
         MessageInterface::ShowMessage("     CS: %s\n",
               segments->at(i).segData.csName.c_str());
      }
   #endif
}

//------------------------------------------------------------------------------
// void SetPropList(ObjectArray *soList)
//------------------------------------------------------------------------------
/**
 * Sets the list of propagated space objects for transient forces.
 */
//------------------------------------------------------------------------------
void FileThrust::SetPropList(ObjectArray *soList)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("FileThrust::SetPropList called with %d "
            "objects\n", soList->size());
   #endif
   spacecraft.clear();
   for (ObjectArray::iterator i = soList->begin(); i != soList->end(); ++i)
      spacecraft.push_back(*i);
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Set up data structures to manage the file based burns
 *
 * @return true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool FileThrust::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Initializing FileThrust\n");
   #endif

   bool retval = false;

   isInitialized = PhysicalModel::Initialize();

   // Zero the data containers
   for (Integer i = 0; i < 5; ++i)
   {
      dataBlock[i] = 0.0;
      dataSet[i][0] = dataSet[i][1] = dataSet[i][2] =
      dataSet[i][3] = dataSet[i][4] = 0.0;
   }

   if (isInitialized)
   {
      if ((segments != NULL) && (spacecraftNames.size() > 0))
      {
         // set up the indices into the state vector that match spacecraft
         Integer satIndex, stateIndex;
         for (StringArray::iterator satName = spacecraftNames.begin();
              satName != spacecraftNames.end(); ++satName)
         {
            satIndex = 0;
            stateIndex = 0;

            for (ObjectArray::iterator propSat = spacecraft.begin();
                 propSat != spacecraft.end(); ++propSat)
            {
               if ((*propSat)->IsOfType(Gmat::SPACEOBJECT))
               {
                  stateIndex += ((SpaceObject*)(*propSat))->GetState().GetSize();
                  ++satIndex;
               }
            }

            // Initialize thrust segments
            for (std::vector<ThrustSegment>::iterator seg = segments->begin();
               seg != segments->end(); ++seg)
               seg->Initialize();
            if (scriptSegments != NULL)
            {
               for (int i = 0; i < scriptSegments->size(); i++)
                  scriptSegments->at(i)->Initialize();
            }
         }

         massFlowWarningNeeded = true;
         warnTooFewPoints      = true;
         indexPair[0]          = -1;
         retval                = true;
      }
      else
         isInitialized = false;
   }

   if (!isInitialized)
      throw ODEModelException("Unable to initialize FileThrust base");

   interpolatorData[0] = interpolatorData[1] = interpolatorData[2] =
   interpolatorData[3] = interpolatorData[4] = -1;

   return retval;
}

//------------------------------------------------------------------------------
// bool GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Method invoked to calculate derivatives
 *
 * This method is invoked to fill the deriv array with derivative information
 * for the system that is being integrated.  It uses the state and elapsedTime
 * parameters, along with the time interval dt passed in as a parameter, to
 * calculate the derivative information at time \f$t=t_0+t_{elapsed}+dt\f$.
 *
 * @param state         Pointer to the current state data.  This can differ
 *                      from the PhysicalModel state if the subscribing
 *                      integrator samples other state values during
 *                      propagation.  (For example, the Runge-Kutta integrators
 *                      do this during the stage calculations.)
 * @param dt            Additional time increment for the derivative
 *                      calculation; defaults to 0.
 * @param order         The order of the derivative to be taken (first
 *                      derivative, second derivative, etc)
 *
 * @return              true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool FileThrust::GetDerivatives(Real* state, Real dt, Integer order,
      const Integer id)
{
   bool retval = true;

   #ifdef DEBUG_FILETHRUST_EXE
      MessageInterface::ShowMessage("Supplying derivative data from a THF, %s\n",
            (depleteMass ? "including mass flow" : "without mass flow"));
   #endif

   #ifdef DEBUG_TSF_SOLVEFOR
      MessageInterface::ShowMessage("===> %sEstimating TSF, Entries go in row %d\n",
            (estimatingTSF ? "" : "NOT"), tsfEpsilonRow);
   #endif

   IntegerArray segIndicies(spacecraft.size(), -1);
   std::vector<RealArray> nominalBurns(spacecraft.size(), { 0.0, 0.0, 0.0 });

   if (fillCartesian)
   {
      Real accel[3], mDot, burnData[4];
      Integer i6, mloc = -1;
      Integer i = 0, j = 0;
      SpaceObject *sat;

      // Loop through the spacecraft list, building accels for active sats
      for (ObjectArray::iterator sc = spacecraft.begin();
           sc != spacecraft.end(); ++sc)
      {
         i6 = cartIndex + i * 6;

         if (depleteMass)
         {
            mloc = mDotIndex + j;
            ++j;
         }
         else
            mloc = -1;

         if ((*sc)->IsOfType(Gmat::SPACEOBJECT) == false)
            continue;

         // Just a convenience
         sat = (SpaceObject*)(*sc);

         #ifdef DEBUG_FILETHRUST_EXE
            MessageInterface::ShowMessage("   Checking spacecraft %s\n",
               sat->GetName().c_str());
         #endif

         if (find(spacecraftNames.begin(), spacecraftNames.end(), sat->GetName()) !=
             spacecraftNames.end())
         {

            #ifdef DEBUG_FILETHRUST_EXE
               MessageInterface::ShowMessage("   Maneuvering %s\n",
                     (*sc)->GetName().c_str());
            #endif

            if (sat->GetType() != Gmat::SPACECRAFT)
               throw ODEModelException("FileThrust::GetDerivatives Finite "
               "burns cannot maneuver " + sat->GetTypeName() + " objects");

            //Ensure that segments have any script updates
            for (int i = 0; i < scriptSegments->size(); i++)
            {
               if ((*scriptSegments)[i]->HadScriptUpdate())
               {
                  for (int j = 0; j < segments->size(); j++)
                  {
                     if ((*segments)[j].GetName() == (*scriptSegments)[i]->GetName())
                     {
                        if ((*segments)[j].GetRealParameter("ThrustScaleFactor") != (*scriptSegments)[i]->GetRealParameter("ThrustScaleFactor"))
                           (*segments)[j].SetRealParameter("ThrustScaleFactor", (*scriptSegments)[i]->GetRealParameter("ThrustScaleFactor"));

                        if ((*segments)[j].GetRvectorParameter("ThrustAngle1") != (*scriptSegments)[i]->GetRvectorParameter("ThrustAngle1"))
                           (*segments)[j].SetRvectorParameter("ThrustAngle1", (*scriptSegments)[i]->GetRvectorParameter("ThrustAngle1"));

                        if ((*segments)[j].GetRvectorParameter("ThrustAngle2") != (*scriptSegments)[i]->GetRvectorParameter("ThrustAngle2"))
                           (*segments)[j].SetRvectorParameter("ThrustAngle2", (*scriptSegments)[i]->GetRvectorParameter("ThrustAngle2"));

                        (*scriptSegments)[i]->ResetScriptUpdate();
                     }
                  }
               }
            }
            // Start with zero thrust
            mDot = accel[0] = accel[1] = accel[2] = 0.0;
            if (HasPrecisionTime()) {
               GmatTime segmentEpochGT = epochGT;
               segmentEpochGT.AddSeconds(elapsedTime);
               GmatTime nowGT = epochGT;
               nowGT.AddSeconds(elapsedTime);
               nowGT.AddSeconds(dt);
               segIndicies[i] = ComputeAccelerationMassFlow(segmentEpochGT, nowGT, burnData);
            }
            else {
               Real segmentEpoch = epoch + elapsedTime / GmatTimeConstants::SECS_PER_DAY;
               Real now = epoch + (elapsedTime + dt) / GmatTimeConstants::SECS_PER_DAY;
               segIndicies[i] = ComputeAccelerationMassFlow(segmentEpoch, now, burnData);
            }

            // Factor used to convert m/s^2 to km/s^2, and to divide out mass
            // if modeling thrust
            Real factor = 0.001;
            if (dataIsThrust)
            {
               // todo: replace string label with an int ID
               Real mass = sat->GetRealParameter("TotalMass");
               factor /= mass;
            }

            accel[0] += burnData[0] * factor;
            accel[1] += burnData[1] * factor;
            accel[2] += burnData[2] * factor;

            #ifdef DEBUG_MASS_FLOW
               MessageInterface::ShowMessage(
                     "   --> direction = [%.12le %.12le %.12le]",
                     accel[0], accel[1], accel[2]);
            #endif

            for (Integer jj = 0; jj < 3; jj++)
               nominalBurns[i][jj] = nominalBurnBody[jj] * factor;

            if (depleteMass)
            {
               // Set the mass flow flag on sat's tanks
               FuelTank *tank = (FuelTank*)(sat->GetRefObject(
                     Gmat::FUEL_TANK, activeTankName));
               if (tank != NULL)
                  tank->SetFlowWithoutThruster(true);

               if (order != 1)
                  throw ODEModelException("Mass depletion cannot be "
                        "performed with the selected propagator.");
               mDot -= burnData[3];
               #ifdef DEBUG_MASS_FLOW
                  MessageInterface::ShowMessage("  mDot =  %.12lf\n",
                        mDot);
               #endif
            }
            #ifdef DEBUG_MASS_FLOW
            else
               MessageInterface::ShowMessage("  mDot =  0.0\n");
            #endif

            // Apply the burns to the state vector
            if (order == 1)
            {
               // dr/dt = v
               deriv[i6]     =
               deriv[1 + i6] =
               deriv[2 + i6] = 0.0;
               deriv[3 + i6] = accel[0];
               deriv[4 + i6] = accel[1];
               deriv[5 + i6] = accel[2];

               if (mloc >= 0)
               {
                  #ifdef DEBUG_MASS_FLOW
                     MessageInterface::ShowMessage("Setting mdot in deriv[%d] "
                           "to %le\n", mloc+i, mDot);
                  #endif
                  deriv[mloc+i] = mDot;
               }
            }
            else
            {
               deriv[ i6 ] = accel[0];
               deriv[i6+1] = accel[1];
               deriv[i6+2] = accel[2];
               deriv[i6+3] = 0.0;
               deriv[i6+4] = 0.0;
               deriv[i6+5] = 0.0;
            }
         }
         else
         {
            deriv[ i6 ] =
            deriv[i6+1] =
            deriv[i6+2] =
            deriv[i6+3] =
            deriv[i6+4] =
            deriv[i6+5] = 0.0;
         }
         ++i;
      }
   }

   #ifdef DEBUG_FILETHRUST_EXE
      //ShowDerivative("FiniteThrust::GetDerivatives() AFTER compute", state,
      //               satCount);
      MessageInterface::ShowMessage
         ("     deriv[1:3] = [%18le %18le %18le]\n"
          "     deriv[4:6] = [%18le %18le %18le]\n"
          "     mdot       = %18le\n",
          deriv[0], deriv[1], deriv[2], deriv[3], deriv[4], deriv[5],
          (depleteMass ? deriv[mDotIndex] : 0.0));
      MessageInterface::ShowMessage("FiniteThrust::GetDerivatives finished\n");
   #endif

   Integer i, ix;
   if (fillSTM || fillAMatrix)
   {
      Real now;
      Real burnData[4];

      #ifdef DEBUG_TSF_SOLVEFOR
         MessageInterface::ShowMessage("FileThrust STM row count = %d\n",
               stmRowCount);
      #endif
      
      Integer iStart = stmStart;
      for (i = 0; i < satCount; ++i)
      {
         //Get Spacecraft object
         Spacecraft* sc = (Spacecraft*)scObjs[i];

         // Create aTilde matrix
         Integer stmRowCount = sc->GetIntegerParameter("FullSTMRowCount");
         Integer stmSize = stmRowCount * stmRowCount;
         Real *aTilde = new Real[stmSize];
         for (Integer j = 0; j < stmRowCount; ++j)
         {
            ix = j * stmRowCount;
            for (Integer k = 0; k < stmRowCount; ++k)
               aTilde[ix + k] = 0.0;
         }

         if (estimatingTSF)
         {
            // probably don't need this block
            if (!fillCartesian) {
                // in case ComputeAccelerationMassFlow() wasn't called
                Real scaleFactors[2];
                if (HasPrecisionTime())
                {
                   GmatTime segmentEpochGT = epochGT;
                   segmentEpochGT.AddSeconds(elapsedTime);

                   for (UnsignedInt i = 0; i < segments->size(); ++i)
                   {
                      if (InSegmentInterval((*segments)[i].segData.startEpochGT,
                         (*segments)[i].segData.endEpochGT, segmentEpochGT))
                      {
                         (*segments)[i].GetScaleFactors(scaleFactors);
                         scaleFactors[0] *= (1.0 + (*segments)[i].GetRealParameter("TSF_Epsilon"));
                         thrustSF = scaleFactors[0];
                         thrustSFinitial = tsfInitial[i];
                         break;
                      }
                   }
                }
                else
                {
                   Real tt = epoch + elapsedTime / GmatTimeConstants::SECS_PER_DAY;

                   for (UnsignedInt i = 0; i < segments->size(); ++i)
                   {
                      if (InSegmentInterval((*segments)[i].segData.startEpoch,
                         (*segments)[i].segData.endEpoch, tt))
                      {
                         (*segments)[i].GetScaleFactors(scaleFactors);
                         scaleFactors[0] *= (1.0 + (*segments)[i].GetRealParameter("TSF_Epsilon"));
                         thrustSF = scaleFactors[0];
                         thrustSFinitial = tsfInitial[i];
                         break;
                      }
                   }
                }
            }

            #ifdef DEBUG_TSF_SOLVEFOR
               MessageInterface::ShowMessage("In FileThrust: ix = %d tsfEpsilonRow = %d, i = %d\n", ix, tsfEpsilonRow, i);
            #endif
            for (UnsignedInt jj = 0; jj < 3; ++jj)
            {
               ix = stmRowCount * (3 + jj);
               aTilde[ix + tsfEpsilonRow] = deriv[i * 6 + 3 + jj] * thrustSFinitial / thrustSF;
            }
         }

         if ((estimatingAngles[0] || estimatingAngles[1]) && segIndicies[i] != -1)
         {
            #ifdef DEBUG_ANGLE_SOLVEFOR
               MessageInterface::ShowMessage("In FileThrust: ix = %d thrustAngleRow = %d, i = %d\n", ix, thrustAngleRow, i);
            #endif

            GmatTime nowGT = epochGT;
            Real now;
            Real offset;
            if (HasPrecisionTime())
            {
               nowGT.AddSeconds(elapsedTime);
               nowGT.AddSeconds(dt);

               GmatTime offsetGT = nowGT - (*segments)[segIndicies[i]].segData.startEpochGT;
               offset = offsetGT.GetTimeInSec() / GmatTimeConstants::SECS_PER_DAY;
            }
            else
            {
               now = epoch + (elapsedTime + dt) / GmatTimeConstants::SECS_PER_DAY;
               offset = now - (*segments)[segIndicies[i]].segData.startEpoch;
            }
            Real offsetSec = offset * GmatTimeConstants::SECS_PER_DAY;

            coordSystem = (*segments)[segIndicies[i]].segData.cs;
            for (Integer angleIndex = 0; angleIndex < 2; angleIndex++)
            {
               if (estimatingAngles[angleIndex])
               {
                  Rvector3 thrustVecOut = ApplyAngleDotRotations(segIndicies[i], nominalBurns[i], offset, angleIndex + 1);
                  Real thrustVec[3] = { thrustVecOut[0], thrustVecOut[1], thrustVecOut[2] };

                  if (HasPrecisionTime())
                  {
                     if (coordSystem != NULL)
                        ConvertDirectionToInertial(thrustVec, thrustVec, nowGT);
                  }
                  else
                  {
                     if (coordSystem != NULL)
                        ConvertDirectionToInertial(thrustVec, thrustVec, now);
                  }

                  Integer angleSize = 0;
                  switch (angleIndex)
                  {
                     case 0:
                        angleSize = (*segments)[segIndicies[i]].GetRvectorParameter("ThrustAngle1").GetSize();
                        break;
                     case 1:
                        angleSize = (*segments)[segIndicies[i]].GetRvectorParameter("ThrustAngle2").GetSize();
                        break;
                     default:
                        break;
                  }

                  for (Integer sz = 0; sz < angleSize; sz++)
                  {
                     Real factor = thrustSFinitial * GmatMathUtil::Pow(offsetSec, sz) * GmatMathConstants::RAD_PER_DEG;
                     for (UnsignedInt jj = 0; jj < 3; ++jj)
                     {
                        ix = stmRowCount * (3 + jj);
                        aTilde[ix + thrustAngleRows[angleIndex] + sz] = thrustVec[jj] * factor;
                     }
                  }
               }
            }
         }

         Integer element;
         for (Integer j = 0; j < stmRowCount; ++j)
         {
            for (Integer k = 0; k < stmRowCount; ++k)
            {
               element = j * stmRowCount + k;
               deriv[iStart + element] = aTilde[element];
            }
         }

         #ifdef DUMP_DERIVATIVE
            MessageInterface::ShowMessage("A-matrix:\n");
            for (Integer m = 0; m < stmRowCount; ++m)
            {
               MessageInterface::ShowMessage("      ");
               for (Integer n = 0; n < stmRowCount; ++n)
               {
                  if (n > 0)
                     MessageInterface::ShowMessage(", ");
                  MessageInterface::ShowMessage("%le", aTilde[m*stmRowCount + n]);
               }
               MessageInterface::ShowMessage("\n");
            }
            MessageInterface::ShowMessage("--------------------------------------"
               "-----------------------------------------------------------\n");
         #endif

         #ifdef DEBUG_FINITEDIFF
            MessageInterface::ShowMessage("Drag derivatives:\n   State: [");
            Integer p, q;
            for (p = 0; p < 6; ++p)
              MessageInterface::ShowMessage(" %le ", deriv[p]);
            MessageInterface::ShowMessage("]\n\n   A-Matrix: [");
            for (p = 0; p < stmRowCount; ++p)
            {
               if (p > 0)
                  MessageInterface::ShowMessage(";\n              ");
               for (q = 0; q < stmRowCount; ++q)
               {
                  MessageInterface::ShowMessage(" %le ", deriv[6+p*stmRowCount+q]);
               }
            }
            MessageInterface::ShowMessage("]\n\n");
         #endif
         
         delete[] aTilde;
         iStart = stmStart + stmSize;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft* sc)
//------------------------------------------------------------------------------
/**
 * Retrieves derivative data used in the force model Parameter code
 *
 * @param sc The spacecraft that supplies the needed state data
 *
 * @return The derivative data as a 6-vector
 *
 * @note: Not yet implemented for the Thrust History File reader code.
 */
//------------------------------------------------------------------------------
Rvector6 FileThrust::GetDerivativesForSpacecraft(Spacecraft* sc)
{
   Rvector6 dv;
   return dv;
}

//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative
 * information for a specified type.
 *
 * @param id State Element ID for the derivative type
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool FileThrust::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "FileThrust checking for support for id %d\n", id);
   #endif

   if (id == Gmat::CARTESIAN_STATE)
      return true;

   if (id == Gmat::MASS_FLOW)
   {
      return true;
   }

   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
      return true;

   if (id == Gmat::ORBIT_A_MATRIX)
      return true;

   return PhysicalModel::SupportsDerivative(id);
}

//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Function used to set the start point and size information for the state
 * vector, so that the derivative information can be placed in the correct place
 * in the derivative vector.
 *
 * @param id State Element ID for the derivative type
 * @param index Starting index in the state vector for this type of derivative
 * @param quantity Number of objects that supply this type of data
 * @param totalSize  Number of elements for a given state element ID. For case STM, 
                     it is the number of elements in all STMs
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool FileThrust::SetStart(Gmat::StateElementId id, Integer index,
      Integer quantity, Integer totalSize)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage("FileThrust setting start data for id = %d"
            " to index %d; %d objects identified, size is %d\n", id, index,
            quantity, sizeOfType);
   #endif

   bool retval = false;

   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         satCount = quantity;
         cartIndex = index;
         fillCartesian = true;
         retval = true;
         break;

      case Gmat::MASS_FLOW:
         // todo: add mass flow bits here
//         satThrustCount = quantity;
         mDotIndex = index;
         depleteMass = true;
         retval = true;
         break;

	  case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
		  stmCount = quantity;
		  stmStart = index;
		  fillSTM = true;
		  totalSTMSize = totalSize;
		  retval = true;
		  break;

	  case Gmat::ORBIT_A_MATRIX:
		  aMatrixCount = quantity;
		  aMatrixStart = index;
		  fillAMatrix = true;
		  totalSTMSize = totalSize;
		  retval = true;
		  break;

      default:
         break;
   }

   return retval;
}


//------------------------------------------------------------------------------
// StringArray GetSolveForList()
//------------------------------------------------------------------------------
/**
 * Retrieves solve-for parameters supported for the FileThrust force
 *
 * @return The list of object/solve-for names
 */
//------------------------------------------------------------------------------
StringArray FileThrust::GetSolveForList()
{
   solveForList.clear();

   StringArray sfors;
   std::string segName;

   for (UnsignedInt i = 0; i < segments->size(); ++i)
   {
      segName = segments->at(i).GetName();
      segName += ".";
      sfors = segments->at(i).GetStringArrayParameter("SolveFors");
      for (UnsignedInt j=0; j < sfors.size(); ++j)
         solveForList.push_back(segName + sfors[j]);
   }

   return solveForList;
}

//------------------------------------------------------------------------------
// void FileThrust::SetStmIndex(Integer id)
//------------------------------------------------------------------------------
/**
 * Method invoked to set the STM index for this physical model
 *
 * @param id The index used to find the parameter in the STM
 */
 //------------------------------------------------------------------------------
void FileThrust::SetStmIndex(Integer id, Integer paramID)
{
   if (paramID > 99)
   {
      // Get segment number from paramID
      Integer segId, pId;
      segId = (Integer)(paramID / 100) - 1;
      pId = paramID - 100 * (segId + 1);

      segments->at(segId).SetStmIndex(id, pId);
   }
}


//------------------------------------------------------------------------------
// void ComputeAccelerationMassFlow(const GmatEpoch segEpoch, const GmatEpoch atEpoch, Real burnData[4])
//------------------------------------------------------------------------------
/**
 * Retrieves dv data at a specified epoch from data in the thrust history file
 *
 * @param segEpoch The epoch corresponding to dt=0 that is used to identify the segment
 * @param atEpoch The epoch of the data request
 * @param burnData The container receiving the acceleration and mass flow
 */
//------------------------------------------------------------------------------
Integer FileThrust::ComputeAccelerationMassFlow(const GmatEpoch segEpoch, const GmatEpoch atEpoch,
      Real burnData[4])
{
   // Start from nothin'
   burnData[0] = burnData[1] = burnData[2] = burnData[3] = 0.0;
   dataBlock[0] = dataBlock[1] = dataBlock[2] = dataBlock[3] = dataBlock[4] = 0.0;
   nominalBurnBody[0] = nominalBurnBody[1] = nominalBurnBody[2] = 0.0;

   Real scaleFactors[2];

   // Find the segment with data covering the input epoch.  Note that if
   // segments overlap, we use the data in the first segment covering the epoch
   Integer index = -1;
   for (UnsignedInt i = 0; i < segments->size(); ++i)
   {
      if ((*segments)[i].segData.isActive &&
            InSegmentInterval((*segments)[i].segData.startEpoch,
            (*segments)[i].segData.endEpoch, segEpoch))
      {
         index = i;
         // Factor used to convert m/s^2 to km/s^2, and to divide out mass if
         // modeling thrust
         dataIsThrust = (*segments)[i].segData.modelThrust;
         (*segments)[i].GetScaleFactors(scaleFactors);
         coordSystem = (*segments)[i].segData.cs;

         // Thrust Scale Factor Solve For
         scaleFactors[0] *= (1.0 + (*segments)[i].GetRealParameter("TSF_Epsilon"));

         thrustSF = scaleFactors[0];
         thrustSFinitial = tsfInitial[i];

         Integer tsfID = (*segments)[i].GetStmIndex((*segments)[i].GetParameterID("ThrustScaleFactor"));
         Integer ta1ID = (*segments)[i].GetStmIndex((*segments)[i].GetParameterID("ThrustAngle1"));
         Integer ta2ID = (*segments)[i].GetStmIndex((*segments)[i].GetParameterID("ThrustAngle2"));
         Integer tsfIndexSTM = psm->GetSTMIndex(tsfID, NULL);
         Integer ta1IndexSTM = psm->GetSTMIndex(ta1ID, NULL);
         Integer ta2IndexSTM = psm->GetSTMIndex(ta2ID, NULL);

#ifdef DEBUG_TSF_SOLVEFOR
         MessageInterface::ShowMessage("File TSF from psm %p (id %d) index in the STM "
            "is %d, dim %d\n", psm, tsfID, index, dimension);
#endif

         // Thrust Scale Factor Solve For settings
         if (tsfIndexSTM >= 0)
         {
            estimatingTSF = true;
            tsfEpsilonRow = tsfIndexSTM;
            fillSTM = true;
         }

         // Thrust Angle Solve For settings
         if (ta1IndexSTM >= 0)
         {
            estimatingAngles[0] = true;
            thrustAngleRows[0] = ta1IndexSTM;
            fillSTM = true;
         }
         if (ta2IndexSTM >= 0)
         {
            estimatingAngles[1] = true;
            thrustAngleRows[1] = ta2IndexSTM;
            fillSTM = true;
         }

         break;
      }
   }

   if (index != -1)
   {
      dataBlock[5] = (*segments)[index].segData.accelIntType;
      dataBlock[6] = (*segments)[index].segData.massIntType;

      // Interpolate the thrust/acceleration

      Integer profileIndex;
      // First use segment epoch
      Real offset = segEpoch - (*segments)[index].segData.startEpoch;
      if ((*segments)[index].segData.endEpoch == segEpoch)
         profileIndex = (*segments)[index].segData.profile.size() - 2;
      else
         profileIndex = GetSegmentData(index, offset);

      if (profileIndex >= 0)
      {
         // Now use actual epoch
         offset = atEpoch - (*segments)[index].segData.startEpoch;
         Interpolate(index, profileIndex, offset);

         nominalBurnBody[0] = dataBlock[0];
         nominalBurnBody[1] = dataBlock[1];
         nominalBurnBody[2] = dataBlock[2];

         ApplyAngleRotations(index, offset);
      }

      burnData[0] = dataBlock[0] * scaleFactors[0];
      burnData[1] = dataBlock[1] * scaleFactors[0];
      burnData[2] = dataBlock[2] * scaleFactors[0];
      burnData[3] = dataBlock[3] * scaleFactors[1];

      if (burnData[3] != 0.0)
      {
         #ifdef DEBUG_MASS_FLOW
            MessageInterface::ShowMessage("Accessing segment %d at epoch "
                  "%.12lf; [3] = %le\n", index, atEpoch, burnData[3]);
            MessageInterface::ShowMessage("   %d mass sources\n",
                  (*segments)[index].massSource.size());
         #endif
         if (segments->size() > index)
         {
            if ((*segments)[index].massSource.size() > 0)
               activeTankName = (*segments)[index].massSource[0]->GetName();
            else
            {
               if (massFlowWarningNeeded)
               {
                  MessageInterface::ShowMessage("Warning: The Thrust History "
                        "File force %s cannot deplete mass: no mass source "
                        "is identified\n", instanceName.c_str());
                  massFlowWarningNeeded = false;
               }
               burnData[3] = 0.0;
               depleteMass = false;
               massFlowWarningNeeded = false;
            }
         }
      }

      if (coordSystem != NULL)
         ConvertDirectionToInertial(burnData, burnData, atEpoch);
   }

   return index;
}


//------------------------------------------------------------------------------
// void ComputeAccelerationMassFlow(const GmatTime &segEpoch, const GmatTime &atEpoch, Real burnData[4])
//------------------------------------------------------------------------------
/**
* Retrieves dv data at a specified epoch from data in the thrust history file
*
* @param segEpoch The epoch corresponding to dt=0 that is used to identify the segment
* @param atEpoch The epoch of the data request
* @param burnData The container receiving the acceleration and mass flow
*/
//------------------------------------------------------------------------------
Integer FileThrust::ComputeAccelerationMassFlow(const GmatTime &segEpoch, const GmatTime &atEpoch,
   Real burnData[4])
{
   // Start from nothin'
   burnData[0] = burnData[1] = burnData[2] = burnData[3] = 0.0;
   dataBlock[0] = dataBlock[1] = dataBlock[2] = dataBlock[3] = dataBlock[4] = 0.0;
   nominalBurnBody[0] = nominalBurnBody[1] = nominalBurnBody[2] = 0.0;

   Real scaleFactors[2];

   // Find the segment with data covering the input epoch.  Note that if
   // segments overlap, we use the data in the first segment covering the epoch
   Integer index = -1;
   for (UnsignedInt i = 0; i < segments->size(); ++i)
   {
      if ((*segments)[i].segData.isActive &&
            InSegmentInterval((*segments)[i].segData.startEpochGT,
            (*segments)[i].segData.endEpochGT, segEpoch))
      {
         index = i;
         // Factor used to convert m/s^2 to km/s^2, and to divide out mass if
         // modeling thrust
         dataIsThrust = (*segments)[i].segData.modelThrust;
         (*segments)[i].GetScaleFactors(scaleFactors);
         coordSystem = (*segments)[i].segData.cs;

         // Thrust Scale Factor Solve For
         scaleFactors[0] *= (1.0 + (*segments)[i].GetRealParameter("TSF_Epsilon"));

         thrustSF = scaleFactors[0];
         thrustSFinitial = tsfInitial[i];

         Integer tsfID = (*segments)[i].GetStmIndex((*segments)[i].GetParameterID("ThrustScaleFactor"));
         Integer ta1ID = (*segments)[i].GetStmIndex((*segments)[i].GetParameterID("ThrustAngle1"));
         Integer ta2ID = (*segments)[i].GetStmIndex((*segments)[i].GetParameterID("ThrustAngle2"));
         Integer tsfIndexSTM = psm->GetSTMIndex(tsfID, NULL);
         Integer ta1IndexSTM = psm->GetSTMIndex(ta1ID, NULL);
         Integer ta2IndexSTM = psm->GetSTMIndex(ta2ID, NULL);

#ifdef DEBUG_TSF_SOLVEFOR
         MessageInterface::ShowMessage("File TSF from psm %p (id %d) index in the STM "
            "is %d, dim %d\n", psm, tsfID, index, dimension);
#endif

         // Thrust Scale Factor Solve For settings
         if (tsfIndexSTM >= 0)
         {
            estimatingTSF = true;
            tsfEpsilonRow = tsfIndexSTM;
            fillSTM = true;
         }

         // Thrust Angle Solve For settings
         if (ta1IndexSTM >= 0)
         {
            estimatingAngles[0] = true;
            thrustAngleRows[0] = ta1IndexSTM;
            fillSTM = true;
         }
         if (ta2IndexSTM >= 0)
         {
            estimatingAngles[1] = true;
            thrustAngleRows[1] = ta2IndexSTM;
            fillSTM = true;
         }

         break;
      }
   }

   if (index != -1)
   {
      dataBlock[5] = (*segments)[index].segData.accelIntType;
      dataBlock[6] = (*segments)[index].segData.massIntType;

      // Interpolate the thrust/acceleration

      Integer profileIndex;
      // First use segment epoch
      GmatTime offsetGT = segEpoch - (*segments)[index].segData.startEpochGT;
      Real offset = offsetGT.GetTimeInSec() / GmatTimeConstants::SECS_PER_DAY;
      if ((*segments)[index].segData.endEpochGT == segEpoch)
         profileIndex = (*segments)[index].segData.profile.size() - 2;
      else
         profileIndex = GetSegmentData(index, offset);


      if (profileIndex >= 0)
      {
         // Now use actual epoch
         offsetGT = atEpoch - (*segments)[index].segData.startEpochGT;
         offset = offsetGT.GetTimeInSec() / GmatTimeConstants::SECS_PER_DAY;
         Interpolate(index, profileIndex, offset);

         nominalBurnBody[0] = dataBlock[0];
         nominalBurnBody[1] = dataBlock[1];
         nominalBurnBody[2] = dataBlock[2];

         ApplyAngleRotations(index, offset);
      }

      burnData[0] = dataBlock[0] * scaleFactors[0];
      burnData[1] = dataBlock[1] * scaleFactors[0];
      burnData[2] = dataBlock[2] * scaleFactors[0];
      burnData[3] = dataBlock[3] * scaleFactors[1];

      if (burnData[3] != 0.0)
      {
#ifdef DEBUG_MASS_FLOW
         MessageInterface::ShowMessage("Accessing segment %d at epoch "
            "%s; [3] = %le\n", index, atEpoch.ToString(), burnData[3]);
         MessageInterface::ShowMessage("   %d mass sources\n",
            (*segments)[index].massSource.size());
#endif
         if (segments->size() > index)
         {
            if ((*segments)[index].massSource.size() > 0)
               activeTankName = (*segments)[index].massSource[0]->GetName();
            else
            {
               if (massFlowWarningNeeded)
               {
                  MessageInterface::ShowMessage("Warning: The Thrust History "
                     "File force %s cannot deplete mass: no mass source "
                     "is identified\n", instanceName.c_str());
                  massFlowWarningNeeded = false;
               }
               burnData[3] = 0.0;
               depleteMass = false;
               massFlowWarningNeeded = false;
            }
         }
      }

      if (coordSystem != NULL)
         ConvertDirectionToInertial(burnData, burnData, atEpoch);
   }

   return index;
}


//------------------------------------------------------------------------------
// Integer GetSegmentData(Integer atIndex, Real offset)
//------------------------------------------------------------------------------
/**
 * Retrieves the index of the ThrustPoint vector element for the segment
 * containing the input epoch
 *
 * @param atIndex Index of the segment containing the data
 * @param offset The offset from the start epoch of the requested segment
 *
 * @return Index of the ThrustPoint vector element containing the data
 */
//------------------------------------------------------------------------------
Integer FileThrust::GetSegmentData(Integer atIndex, Real offset)
{
   #ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage("Entered GetSegmentData(%d, %.12lf)\n",
            atIndex, offset);
   #endif

   Integer profileIndex = -1;
   for (UnsignedInt i = 0; i < (*segments)[atIndex].segData.profile.size()-1; ++i)
   {
      if (InSegmentInterval((*segments)[atIndex].segData.profile[i].time,
            (*segments)[atIndex].segData.profile[i + 1].time, offset))
      {
         profileIndex = i;
         break;
      }
   }

   return profileIndex;
}


//------------------------------------------------------------------------------
// void Interpolate(Integer atIndex, Integer profileIndex, Real offset)
//------------------------------------------------------------------------------
/**
 * Performs desired interpolation and populates dataBlock array
 *
 * @param atIndex Index of the segment containing the data
 * @param profileIndex Index of the ThrustPoint vector element containing the data
 * @param offset The offset from the start epoch of the requested data
 */
 //------------------------------------------------------------------------------
void FileThrust::Interpolate(Integer atIndex, Integer profileIndex, Real offset)
{
   switch ((Integer)dataBlock[5])
   {
   case ThfDataSegment::LINEAR:
      LinearInterpolate(atIndex, profileIndex, offset);
      #ifdef DEBUG_INTERPOLATION
         MessageInterface::ShowMessage("Thrust/Acceleration: Linear Interpolation "
                  "-> VXdot = %.12le\n", dataBlock[0] * scaleFactors[0]);
      #endif
      break;

   case ThfDataSegment::SPLINE:
      SplineInterpolate(atIndex, profileIndex, offset);
      #ifdef DEBUG_INTERPOLATION
         MessageInterface::ShowMessage("Thrust/Acceleration: Spline Interpolation "
                  "-> VXdot = %.12le\n", dataBlock[0] * scaleFactors[0]);
      #endif
      break;

   case ThfDataSegment::NONE:
   default:
      #ifdef DEBUG_INTERPOLATION
         MessageInterface::ShowMessage("Thrust/Acceleration: No Interpolation "
                  "-> VXdot = %.12le\n", dataBlock[0] * scaleFactors[0]);
      #endif
      dataBlock[0] = (*segments)[atIndex].segData.profile[profileIndex].vector[0];
      dataBlock[1] = (*segments)[atIndex].segData.profile[profileIndex].vector[1];
      dataBlock[2] = (*segments)[atIndex].segData.profile[profileIndex].vector[2];
      dataBlock[3] = (*segments)[atIndex].segData.profile[profileIndex].mdot;
      dataBlock[4] = (*segments)[atIndex].segData.profile[profileIndex].time;
      break;
   }

   if (dataBlock[5] != dataBlock[6])
   {
      // Interpolate the mass flow, using data already collected
      switch ((Integer)dataBlock[6])
      {
      case ThfDataSegment::LINEAR:
         LinearInterpolate(atIndex, profileIndex, offset);
         #ifdef DEBUG_INTERPOLATION
            MessageInterface::ShowMessage("Mass Flow: Linear Interpolation "
                  "-> Mdot = %.12le\n", dataBlock[3] * scaleFactors[1]);
         #endif
         break;

      case ThfDataSegment::SPLINE:
         SplineInterpolate(atIndex, profileIndex, offset);
         #ifdef DEBUG_INTERPOLATION
            MessageInterface::ShowMessage("Mass Flow: Spline Interpolation "
                  "-> Mdot = %.12le\n", dataBlock[3] * scaleFactors[1]);
         #endif
         break;

      case ThfDataSegment::NONE:
      default:
         #ifdef DEBUG_INTERPOLATION
            MessageInterface::ShowMessage("Mass Flow: No Interpolation "
                  "-> Mdot = %.12le\n", dataBlock[3] * scaleFactors[1]);
         #endif
         dataBlock[3] = (*segments)[atIndex].segData.profile[profileIndex].mdot;
         break;
      }
   }
}


//------------------------------------------------------------------------------
// void LinearInterpolate(Integer atIndex, Integer profileIndex, Real offset)
//------------------------------------------------------------------------------
/**
 * Retrieves linearly interpolated segment data for the input epoch offset
 * and populates the dataBlock array
 *
 * @param atIndex Index of the segment containing the data
 * @param profileIndex Index of the ThrustPoint vector element containing the data
 * @param offset The offset from the start epoch of the requested data
 */
//------------------------------------------------------------------------------
void FileThrust::LinearInterpolate(Integer atIndex, Integer profileIndex, Real offset)
{
   bool includeMass = false;

   dataSet[0][0] = (*segments)[atIndex].segData.profile[profileIndex].vector[0];
   dataSet[0][1] = (*segments)[atIndex].segData.profile[profileIndex].vector[1];
   dataSet[0][2] = (*segments)[atIndex].segData.profile[profileIndex].vector[2];
   dataSet[0][3] = (*segments)[atIndex].segData.profile[profileIndex].mdot;
   dataSet[0][4] = (*segments)[atIndex].segData.profile[profileIndex].time;

   dataSet[1][0] = (*segments)[atIndex].segData.profile[profileIndex+1].vector[0];
   dataSet[1][1] = (*segments)[atIndex].segData.profile[profileIndex+1].vector[1];
   dataSet[1][2] = (*segments)[atIndex].segData.profile[profileIndex+1].vector[2];
   dataSet[1][3] = (*segments)[atIndex].segData.profile[profileIndex+1].mdot;
   dataSet[1][4] = (*segments)[atIndex].segData.profile[profileIndex+1].time;

   Real pct = 0.0;
   if ((dataSet[1][4] != dataSet[0][4]))
      pct = (offset - dataSet[0][4]) / (dataSet[1][4] - dataSet[0][4]);

   if (dataBlock[5] == ThfDataSegment::LINEAR)
   {
      dataBlock[0] = dataSet[0][0] + pct * (dataSet[1][0] - dataSet[0][0]);
      dataBlock[1] = dataSet[0][1] + pct * (dataSet[1][1] - dataSet[0][1]);
      dataBlock[2] = dataSet[0][2] + pct * (dataSet[1][2] - dataSet[0][2]);
   }
   if (dataBlock[6] == ThfDataSegment::LINEAR)
      dataBlock[3] = dataSet[0][3] + pct * (dataSet[1][3] - dataSet[0][3]);

   #ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage("Linear Interpolating to offset %.12lf "
            "using t, vector, mdot:\n", offset);
      MessageInterface::ShowMessage("   %.12lf [%.12le  %.12le  %.12le] "
            "%.12le\n   %.12lf [%.12le  %.12le  %.12le] %.12le\n",
            dataSet[0][4], dataSet[0][0], dataSet[0][1], dataSet[0][2],
            dataSet[0][3], dataSet[1][4], dataSet[1][0], dataSet[1][1],
            dataSet[1][2], dataSet[1][3]);

      MessageInterface::ShowMessage("-> %.12lf [%.12le  %.12le  %.12le] "
            "%.12le\n", offset, dataBlock[0], dataBlock[1], dataBlock[2], dataBlock[3]);
   #endif
}


//------------------------------------------------------------------------------
// void SplineInterpolate(Integer atIndex, Integer profileIndex, Real offset)
//------------------------------------------------------------------------------
/**
 * Retrieves spline interpolated segment data for the input epoch offset
 * and populates the dataBlock array
 *
 * @param atIndex Index of the segment containing the data
 * @param profileIndex Index of the ThrustPoint vector element containing the data
 * @param offset The offset from the start epoch of the requested data
 */
//------------------------------------------------------------------------------
void FileThrust::SplineInterpolate(Integer atIndex, Integer profileIndex, Real offset)
{
   // Handle case of too few points by falling back to linear interpolation
   if ((*segments)[atIndex].segData.profile.size() < 5)
   {
      if (warnTooFewPoints)
      {
         MessageInterface::ShowMessage("Cannot perform spline interpolation: "
               "the thrust history data segment contains %d points, but spline "
               "interpolation requires at least 5.  Linear interpolation will "
               "be applied instead.\n",
               (*segments)[atIndex].segData.profile.size());
         warnTooFewPoints = false;
      }
      LinearInterpolate(atIndex, profileIndex, offset);
   }

   if (spliner == NULL)
   {
      spliner = new NotAKnotInterpolator("SplineInterpolator", 4);
      spliner->SetExtrapolation(true); // Allow extrapolation for RK89
   }

   if (spliner == NULL)
      throw ODEModelException("The cubic spline interpolator failed to build");

   Real data[4];
   Integer profileSize = (*segments)[atIndex].segData.profile.size();
   Integer interpIndex = profileIndex;

   // Want to make sure interpolatorData has valid indicies
   if (interpIndex < 1)
      interpIndex = 1;
   else if (interpIndex > (profileSize - 4))
      interpIndex = profileSize - 4;

   interpolatorData[0] = interpIndex - 1;
   interpolatorData[1] = interpIndex;
   interpolatorData[2] = interpIndex + 1;
   interpolatorData[3] = interpIndex + 2;
   interpolatorData[4] = interpIndex + 3;

   // Reload the interpolator.  For now, this is done at each call.
   spliner->Clear();
   for (UnsignedInt i = 0; i < 5; ++i)
   {
      data[0] = (*segments)[atIndex].segData.profile[interpolatorData[i]].vector[0];
      data[1] = (*segments)[atIndex].segData.profile[interpolatorData[i]].vector[1];
      data[2] = (*segments)[atIndex].segData.profile[interpolatorData[i]].vector[2];
      data[3] = (*segments)[atIndex].segData.profile[interpolatorData[i]].mdot;

      spliner->AddPoint((*segments)[atIndex].segData.profile[interpolatorData[i]].time, data);
   }

   spliner->Interpolate(offset, data);

   if (dataBlock[5] == ThfDataSegment::SPLINE)
   {
      dataBlock[0] = data[0];
      dataBlock[1] = data[1];
      dataBlock[2] = data[2];
   }
   if (dataBlock[6] == ThfDataSegment::SPLINE)
      dataBlock[3] = data[3];
}


//------------------------------------------------------------------------------
// void ApplyAngleRotations(Integer atIndex, Real offset)
//------------------------------------------------------------------------------
/**
 * Applies the ThrustAngle rotations to the dataBlock array
 *
 * @param atIndex Index of the segment containing the data
 * @param offset The offset from the start epoch of the requested data in days
 */
 //------------------------------------------------------------------------------
void FileThrust::ApplyAngleRotations(Integer atIndex, Real offset)
{
   // Get the rotation angles
   Rvector angleVec1 = (*segments)[atIndex].GetRvectorParameter("ThrustAngle1");
   Rvector angleVec2 = (*segments)[atIndex].GetRvectorParameter("ThrustAngle2");

   if (!angleVec1.IsZeroVector() || !angleVec2.IsZeroVector())
   {
      Real angle1 = 0;
      Real angle2 = 0;
      Real offsetSec = offset * GmatTimeConstants::SECS_PER_DAY;

      for (Integer ii = 0; ii < angleVec1.GetSize(); ii++)
         angle1 += angleVec1[ii] * GmatMathUtil::Pow(offsetSec, ii);
      for (Integer ii = 0; ii < angleVec2.GetSize(); ii++)
         angle2 += angleVec2[ii] * GmatMathUtil::Pow(offsetSec, ii);

      angle1 *= GmatMathConstants::RAD_PER_DEG;
      angle2 *= GmatMathConstants::RAD_PER_DEG;

      // Create the rotation matricies based on the Constraint Vector
      Rvector constraintVector = (*segments)[atIndex].GetRvectorParameter("ThrustAngleConstraintVector");
      Rvector3 constraintVec(constraintVector[0], constraintVector[1], constraintVector[2]);
      Rvector3 thrustVec(dataBlock[0], dataBlock[1], dataBlock[2]);
      Rvector3 thrustUnit;

      Real thrustMag = thrustVec.GetMagnitude();
      if (thrustMag == 0)
      {
         // Return no change to thrust vector if thrust magnitude is zero
         return;
      }
      else
      {
         // Manually normalize in case thrust magnitude is smaller than tolerance for Normalize()
         for (Integer ii = 0; ii < 3; ii++)
            thrustUnit[ii] = thrustVec[ii] / thrustMag;
      }

      Rvector3 rotVec1 = Cross(thrustUnit, constraintVec);
      if (rotVec1.GetMagnitude() < 1e-14)
         throw ODEModelException("The cross product of the thrust vector and the constraint vector created a zero vector. "
            "The ThrustAngleConstraintVector cannot be specified parallel to the thrust vector.");
      rotVec1.Normalize();

      Rvector3 rotVec2 = Cross(rotVec1, thrustUnit);
      if (rotVec2.GetMagnitude() < 1e-14)
         throw ODEModelException("The cross product of the first thrust angle rotation axis and the thrust vector created a zero vector. "
         "These vectors cannot be parallel.");
      rotVec2.Normalize();

      Rmatrix33 rotMat1;
      Rmatrix33 rotMat2;
      CalcAngleRotationMatrix(angle1, rotVec1, rotMat1);
      CalcAngleRotationMatrix(angle2, rotVec2, rotMat2);

      Rmatrix33 rot = rotMat1 * rotMat2;
      Rvector3 thrustVecOut = rot * thrustVec;

      dataBlock[0] = thrustVecOut[0];
      dataBlock[1] = thrustVecOut[1];
      dataBlock[2] = thrustVecOut[2];
   }
}


//------------------------------------------------------------------------------
// Rvector3 ApplyAngleDotRotations(Integer atIndex, RealArray nominalBurn, Real offset, Integer derivAngle)
//------------------------------------------------------------------------------
/**
 * Calculates the partial derivatives for the ThrustAngle rotations
 *
 * @param atIndex Index of the segment containing the data
 * @param derivAngle Angle which the derivative is taken with respect to
 */
 //------------------------------------------------------------------------------
Rvector3 FileThrust::ApplyAngleDotRotations(Integer atIndex, RealArray nominalBurn, Real offset, Integer derivAngle)
{
   // Get the rotation angles
   Rvector angleVec1 = (*segments)[atIndex].GetRvectorParameter("ThrustAngle1");
   Rvector angleVec2 = (*segments)[atIndex].GetRvectorParameter("ThrustAngle2");

   Real angle1 = 0;
   Real angle2 = 0;
   Real offsetSec = offset * GmatTimeConstants::SECS_PER_DAY;

   for (Integer ii = 0; ii < angleVec1.GetSize(); ii++)
      angle1 += angleVec1[ii] * GmatMathUtil::Pow(offsetSec, ii);
   for (Integer ii = 0; ii < angleVec2.GetSize(); ii++)
      angle2 += angleVec2[ii] * GmatMathUtil::Pow(offsetSec, ii);

   angle1 *= GmatMathConstants::RAD_PER_DEG;
   angle2 *= GmatMathConstants::RAD_PER_DEG;

   // Create the rotation matricies based on the Constraint Vector
   Rvector constraintVector = (*segments)[atIndex].GetRvectorParameter("ThrustAngleConstraintVector");
   Rvector3 constraintVec(constraintVector[0], constraintVector[1], constraintVector[2]);
   Rvector3 thrustVec(nominalBurn[0], nominalBurn[1], nominalBurn[2]);
   Rvector3 thrustUnit;

   Real thrustMag = thrustVec.GetMagnitude();
   if (thrustMag == 0)
   {
      // Return zero if thrust magnitude is zero
      Rvector3 thrustVecOut;
      return thrustVecOut;
   }
   else
   {
      // Manually normalize in case thrust magnitude is smaller than tolerance for Normalize()
      for (Integer ii = 0; ii < 3; ii++)
         thrustUnit[ii] = thrustVec[ii] / thrustMag;
   }

   Rvector3 rotVec1 = Cross(thrustUnit, constraintVec);
   if (rotVec1.GetMagnitude() < 1e-14)
      throw ODEModelException("The cross product of the thrust vector and the constraint vector created a zero vector. "
         "The ThrustAngleConstraintVector cannot be specified parallel to the thrust vector.");
   rotVec1.Normalize();

   Rvector3 rotVec2 = Cross(rotVec1, thrustUnit);
   if (rotVec2.GetMagnitude() < 1e-14)
      throw ODEModelException("The cross product of the first thrust angle rotation axis and the thrust vector created a zero vector. "
         "These vectors cannot be parallel.");
   rotVec2.Normalize();

   Rmatrix33 rotMat1;
   Rmatrix33 rotMat2;

   switch (derivAngle)
   {
      case 1:
         CalcAngleDotRotationMatrix(angle1, rotVec1, rotMat1);
         CalcAngleRotationMatrix(angle2, rotVec2, rotMat2);
         break;
      case 2:
         CalcAngleRotationMatrix(angle1, rotVec1, rotMat1);
         CalcAngleDotRotationMatrix(angle2, rotVec2, rotMat2);
         break;
      default:
         throw ODEModelException("Invalid angle index selected for calculating ThrustAngle rotation matrix partials");
         break;
   }

   Rmatrix33 rot = rotMat1 * rotMat2;
   Rvector3 thrustVecOut = rot * thrustVec;
   return thrustVecOut;
}


//------------------------------------------------------------------------------
// Rmatrix33 CalcAngleRotationMatrix(Real angle, const Rvector3 &rotVec, Rmatrix33 &rotMat)
//------------------------------------------------------------------------------
/**
 * Calculates the ThrustAngle rotation matrix
 *
 * @param angle Rotation angle in radians
 * @param rotVec Unit vector the rotation is about
 * @param rotMat Output 3x3 rotation matrix
 */
 //------------------------------------------------------------------------------
void FileThrust::CalcAngleRotationMatrix(Real angle, const Rvector3 &rotVec, Rmatrix33 &rotMat) const
{
   // [C] = cos(theta)*I + (1 - cos(theta)) * vec * vec^T + sin(theta)*SkewSymmetric(vec)
   Rmatrix33 eye(true);
   rotMat = GmatMathUtil::Cos(angle) * eye
      + (1 - GmatMathUtil::Cos(angle)) * Outerproduct(rotVec, rotVec)
      + GmatMathUtil::Sin(angle) * SkewSymmetric(rotVec);
}


//------------------------------------------------------------------------------
// Rmatrix33 CalcAngleDotRotationMatrix(Real angle, const Rvector3 &rotVec, Rmatrix33 &rotDotMat)
//------------------------------------------------------------------------------
/**
 * Calculates the ThrustAngle dot rotation matrix
 *
 * @param angle Rotation angle in radians
 * @param rotVec Unit vector the rotation is about
 * @param rotDotMat Output 3x3 rotation matrix
 */
 //------------------------------------------------------------------------------
void FileThrust::CalcAngleDotRotationMatrix(Real angle, const Rvector3 &rotVec, Rmatrix33 &rotDotMat) const
{
   // [C] = sin(theta)*I +sin(theta) * vec * vec^T + cos(theta)*SkewSymmetric(vec)
   Rmatrix33 eye(true);
   rotDotMat = -GmatMathUtil::Sin(angle) * eye
      + GmatMathUtil::Sin(angle) * Outerproduct(rotVec, rotVec)
      + GmatMathUtil::Cos(angle) * SkewSymmetric(rotVec);
}


//------------------------------------------------------------------------------
// void ConvertDirectionToInertial(Real *dir, Real *dirInertial, Real epoch)
//------------------------------------------------------------------------------
/*
 * Converts thrust direction to inertial frame
 *
 * @param dir  Thrust/accel direction in internal frame
 * @param dirInertial  Thrust direction in inertial frame
 * @param epoch  Epoch to be used for conversion
 */
//------------------------------------------------------------------------------
void FileThrust::ConvertDirectionToInertial(Real *dir, Real *dirInertial, Real epoch)
{
   #ifdef DEBUG_FILETHRUST_EXE
      MessageInterface::ShowMessage("Rotating into a new frame\n");
   #endif

   Real inDir[6], outDir[6];
   for (Integer i=0; i<3; i++)
      inDir[i] = dir[i];
   for (Integer i=3; i<6; i++)
      inDir[i] = 0.0;

   // Now rotate to base system axes, we don't want to translate so
   // set coincident to true
   coordSystem->ToBaseSystem(A1Mjd(epoch), inDir, outDir, true);

   dirInertial[0] = outDir[0];
   dirInertial[1] = outDir[1];
   dirInertial[2] = outDir[2];
}


//------------------------------------------------------------------------------
// void ConvertDirectionToInertial(Real *dir, Real *dirInertial, GmatTime epoch)
//------------------------------------------------------------------------------
/*
* Converts thrust direction to inertial frame
*
* @param dir  Thrust/accel direction in internal frame
* @param dirInertial  Thrust direction in inertial frame
* @param epoch  Epoch to be used for conversion
*/
//------------------------------------------------------------------------------
void FileThrust::ConvertDirectionToInertial(Real *dir, Real *dirInertial,
   const GmatTime &epoch)
{
#ifdef DEBUG_FILETHRUST_EXE
   MessageInterface::ShowMessage("Rotating into a new frame\n");
#endif

   Real inDir[6], outDir[6];
   for (Integer i = 0; i<3; i++)
      inDir[i] = dir[i];
   for (Integer i = 3; i<6; i++)
      inDir[i] = 0.0;

   // Now rotate to base system axes, we don't want to translate so
   // set coincident to true
   coordSystem->ToBaseSystem(epoch, inDir, outDir, true);

   dirInertial[0] = outDir[0];
   dirInertial[1] = outDir[1];
   dirInertial[2] = outDir[2];
}


//------------------------------------------------------------------------------
// const std::vector<ThrustSegment> GetAllThrustSegments()
//------------------------------------------------------------------------------
/**
 * Accessor for the ThrustSegment array
 *
 * @return The ThrustSegment array
 */
//------------------------------------------------------------------------------
const std::vector<ThrustSegment> FileThrust::GetAllThrustSegments()
{
	return *segments;
}


//------------------------------------------------------------------------------
// bool IsEstimationParameterValid(const Integer item)
//------------------------------------------------------------------------------
/**
 * Checks to see if a parameter can be estimated
 *
 * @param item The ID for the parameter, as known in the estimation system
 *
 * @return true if estimation is available, false if not
 */
//------------------------------------------------------------------------------
bool FileThrust::IsEstimationParameterValid(const Integer item)
{
   // convert Estimation ID to object parameter ID
   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   #ifdef DEBUG_TSF_SOLVEFOR
      MessageInterface::ShowMessage("Testing estimation capability for "
            "ID %d (%s)\n", id, GetParameterText(id).c_str());
   #endif

   if (id > 99)
   {
      // Break ID apart into segment number and parm id
      Integer segId, parmId;
      segId = (Integer)(id / 100) - 1;
      parmId = id - 100 * (segId + 1);
      std::string parmName = segments->at(segId).GetParameterText(parmId);
      Integer estID = segments->at(segId).GetEstimationParameterID(parmName);

      return segments->at(segId).IsEstimationParameterValid(estID);
   }

   return PhysicalModel::IsEstimationParameterValid(id);
}


//------------------------------------------------------------------------------
// Integer GetEstimationParameterSize(const Integer item)
//------------------------------------------------------------------------------
/**
 * Checks the size of an estimated parameter
 *
 * @param item The ID for the parameter, as known in the estimation system
 *
 * @return the size of the estimated parameter
 */
//------------------------------------------------------------------------------
Integer FileThrust::GetEstimationParameterSize(const Integer item)
{
   // convert Estimation ID to object parameter ID
   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   #ifdef DEBUG_TSF_SOLVEFOR
      MessageInterface::ShowMessage("Testing estimation capability for "
            "ID %d (%s)\n", id, GetParameterText(id).c_str());
   #endif

   if (id > 99)
   {
      // Break ID apart into segment number and parm id
      Integer segId, parmId;
      segId = (Integer)(id / 100) - 1;
      parmId = id - 100 * (segId + 1);
      std::string parmName = segments->at(segId).GetParameterText(parmId);
      Integer estID = segments->at(segId).GetEstimationParameterID(parmName);

      return segments->at(segId).GetEstimationParameterSize(estID);
   }

   return PhysicalModel::GetEstimationParameterSize(id);
}

//-------------------------------------------------------------------------
// Integer HasParameterCovariances(Integer parameterId)
//-------------------------------------------------------------------------
/**
* This function is used to verify whether a parameter (with ID specified by
* parameterId) having a covariance or not.
*
* @param parameterId      ID of a parameter
* @return                 size of covariance matrix associated with the parameter
*                         return -1 when the parameter has no covariance
*/
//-------------------------------------------------------------------------
Integer FileThrust::HasParameterCovariances(Integer parameterId)
{
   if (parameterId > 99)
   {
      // Break ID apart into segment number and parm id
      Integer segId, parmId;
      segId = (Integer)(parameterId / 100) - 1;
      parmId = parameterId - 100 * (segId + 1);

#ifdef DEBUG_MAPPING
      MessageInterface::ShowMessage("Get %d: Segment %d, parm %d\n", id,
         segId, parmId);
#endif

      return segments->at(segId).HasParameterCovariances(parmId);
   }

   return GmatBase::HasParameterCovariances(parameterId);
}


//------------------------------------------------------------------------------
// Rmatrix* GetParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
* Get covariance of a given FileThrust's parameter
*
* @param paramId     The Id of a FileThrust's parameter
*
* @return            Covariance matrix of the parameter specified by the
*                    parameter Id
*/
//------------------------------------------------------------------------------
Rmatrix* FileThrust::GetParameterCovariances(Integer parameterId)
{
   if (isInitialized)
      return covariance.GetCovariance(parameterId);
   else
      throw GmatBaseException("Error: cannot get " + GetName() + " FileThrust's covariance when it is not initialized.\n");
   return NULL;
}

//------------------------------------------------------------------------------
// Covariance* GetCovariance()
//------------------------------------------------------------------------------
Covariance* FileThrust::GetCovariance()
{
   std::vector<Rmatrix> covVector;
   covarianceSizes.clear();

   for (UnsignedInt ii = 0; ii < segments->size(); ii++)
   {
      for (UnsignedInt jj = 0; jj < (*segments)[ii].GetParameterCount(); jj++)
      {
         Integer covSize = (*segments)[ii].HasParameterCovariances(jj);

         if (covSize > 0)
         {
            std::string paramName = (*segments)[ii].GetParameterText(jj);
            std::string elementName = (*segments)[ii].GetName() + "." + paramName;
            covariance.AddCovarianceElement(elementName, this);

            covVector.push_back(*(*segments)[ii].GetCovariance()->GetCovariance(jj));
            covarianceSizes.push_back(covSize);
         }
      }
   }

   UnsignedInt covIndex = 0;

   for (UnsignedInt ii = 0; ii < covarianceSizes.size(); ii++)
   {
      covariance.ConstructRHS(covVector[ii], covIndex);
      covIndex += covarianceSizes[ii];
   }

   return &covariance;
}


//------------------------------------------------------------------------------
// bool SetPrecisionTimeFlag(bool onOff)
//------------------------------------------------------------------------------
/**
* Set whether the thrust segment is using precision time or not
*
* @param onOff Flag indicating whether the thrust segment uses precision time
*
* @return Returns the value of the onOff flag passed in
*/
//------------------------------------------------------------------------------
bool FileThrust::SetPrecisionTimeFlag(bool onOff)
{
   hasPrecisionTime = onOff;

   if (segments != NULL)
      for (ThrustSegment seg : *segments)
         seg.SetPrecisionTimeFlag(onOff);

   return hasPrecisionTime;
}


//------------------------------------------------------------------------------
// Real GetForceMaxStep(bool forward = true)
//------------------------------------------------------------------------------
/**
 * Gets the max step size allowed by this PhysicalModel
 *
 * @param forward Flag to indicate which direction in time to travel
 *
 * @return The max step size in seconds, returns +/- REAL_MAX if the step size
 *         is not limited
 */
//------------------------------------------------------------------------------
Real FileThrust::GetForceMaxStep(bool forward)
{
   if (hasPrecisionTime)
   {
      //GmatTime theEpoch = theState->GetEpochGT();
      GmatTime theEpoch = epochGT;
      theEpoch.AddSeconds(elapsedTime);
      return GetForceMaxStep(theEpoch, forward);
   }
   else
   {
      //Real theEpoch = theState->GetEpoch();
      Real theEpoch = epoch;
      theEpoch += elapsedTime / GmatTimeConstants::SECS_PER_DAY;
      return GetForceMaxStep(theEpoch, forward);
   }
}


//------------------------------------------------------------------------------
// Real GetForceMaxStep(Real theEpoch, bool forward = true)
//------------------------------------------------------------------------------
/**
 * Gets the max step size allowed by this ODEModel
 *
 * @param theEpoch The reference epoch to use for calculating the max step size
 * @param forward Flag to indicate which direction in time to travel
 *
 * @return The max step size in seconds, returns +/- REAL_MAX if the step size
 *         is not limited
 */
//------------------------------------------------------------------------------
Real FileThrust::GetForceMaxStep(Real theEpoch, bool forward)
{
   // Get default values
   Real dt = PhysicalModel::GetForceMaxStep(theEpoch, forward);

   for (UnsignedInt ii = 0U; ii < segments->size(); ii++)
   {
      Real dtStart, dtEnd;

      dtStart = ((*segments)[ii].segData.startEpoch - theEpoch) * GmatTimeConstants::SECS_PER_DAY;
      dtEnd = ((*segments)[ii].segData.endEpoch - theEpoch) * GmatTimeConstants::SECS_PER_DAY;

      if (forward)
      {
         if (dtStart > 0)
            dt = GmatMathUtil::Min(dt, dtStart);
         if (dtEnd > 0)
            dt = GmatMathUtil::Min(dt, dtEnd);
      }
      else
      {
         if (dtStart < 0)
            dt = GmatMathUtil::Max(dt, dtStart);
         if (dtEnd < 0)
            dt = GmatMathUtil::Max(dt, dtEnd);
      }

      // If inside the segment, check the profile epochs
      if (dtStart <= 0.0 && dtEnd >= 0.0)
      {
         // Don't need to check last epoch as it always is aligned with endEpoch
         for (UnsignedInt jj = 0U; jj < (*segments)[ii].segData.profile.size()-1; jj++)
         {
            Real offset = theEpoch - (*segments)[ii].segData.startEpoch;
            Real dtProfile = ((*segments)[ii].segData.profile[jj].time - offset) * GmatTimeConstants::SECS_PER_DAY;

            if (forward)
            {
               if (dtProfile > 0)
                  dt = GmatMathUtil::Min(dt, dtProfile);
            }
            else
            {
               if (dtProfile < 0)
                  dt = GmatMathUtil::Max(dt, dtProfile);
            }
         }
      }
   }

   return dt;
}


//------------------------------------------------------------------------------
// Real GetForceMaxStep(const GmatTime& theEpochGT, bool forward = true)
//------------------------------------------------------------------------------
/**
 * Gets the max step size allowed by this ODEModel
 *
 * @param theEpochGT The reference epoch to use for calculating the max step size
 * @param forward Flag to indicate which direction in time to travel
 *
 * @return The max step size in seconds, returns +/- REAL_MAX if the step size
 *         is not limited
 */
//------------------------------------------------------------------------------
Real FileThrust::GetForceMaxStep(const GmatTime& theEpochGT, bool forward)
{
   // Get default values
   Real dt = PhysicalModel::GetForceMaxStep(theEpochGT, forward);

   for (UnsignedInt ii = 0U; ii < segments->size(); ii++)
   {
      Real dtStart, dtEnd;

      dtStart = ((*segments)[ii].segData.startEpochGT - theEpochGT).GetTimeInSec();
      dtEnd = ((*segments)[ii].segData.endEpochGT - theEpochGT).GetTimeInSec();

      if (forward)
      {
         if (dtStart > 0)
            dt = GmatMathUtil::Min(dt, dtStart);
         if (dtEnd > 0)
            dt = GmatMathUtil::Min(dt, dtEnd);
      }
      else
      {
         if (dtStart < 0)
            dt = GmatMathUtil::Max(dt, dtStart);
         if (dtEnd < 0)
            dt = GmatMathUtil::Max(dt, dtEnd);
      }

      // If inside the segment, check the profile epochs
      if (dtStart <= 0.0 && dtEnd >= 0.0)
      {
         // Don't need to check last epoch as it always is aligned with endEpoch
         for (UnsignedInt jj = 0U; jj < (*segments)[ii].segData.profile.size()-1; jj++)
         {
            GmatTime offsetGT = theEpochGT - (*segments)[ii].segData.startEpochGT;
            Real offset = offsetGT.GetTimeInSec() / GmatTimeConstants::SECS_PER_DAY;
            Real dtProfile = ((*segments)[ii].segData.profile[jj].time - offset) * GmatTimeConstants::SECS_PER_DAY;


            if (forward)
            {
               if (dtProfile > 0)
                  dt = GmatMathUtil::Min(dt, dtProfile);
            }
            else
            {
               if (dtProfile < 0)
                  dt = GmatMathUtil::Max(dt, dtProfile);
            }
         }
      }
   }

   return dt;
}

bool FileThrust::IsEndOfThrust(bool forward)
{
   for (UnsignedInt i = 0U; i < segments->size(); i++)
   {
      if (forward)
      {
         if (hasPrecisionTime)
         {
            GmatTime e = epochGT;
            e.AddSeconds(elapsedTime);
            if (e.IsNearlyEqual((*segments)[i].segData.endEpochGT, 1e-6))
               return true;
         }
         else
         {
            Real e = epoch + elapsedTime;
            if (abs(abs(e) - abs((*segments)[i].segData.endEpoch)) < 1e-6)
               return true;
         }
      } 
      else
      {
         if (hasPrecisionTime)
         {
            GmatTime e = epochGT;
            e.AddSeconds(elapsedTime);
            if (e.IsNearlyEqual((*segments)[i].segData.startEpochGT, 1e-6))
               return true;
         }
         else
         {
            Real e = epoch + elapsedTime;
            if (abs(abs(e) - abs((*segments)[i].segData.startEpoch)) < 1e-6)
               return true;
         }
      }
   }
   return false;
}

//------------------------------------------------------------------------------
// bool FileThrust::IsSolvingForThrustEventsWithSpinningAttitude()
//------------------------------------------------------------------------------
/**
 * Checks to see if we are solving for a thrust segment using an spacecraft with 
 * a spinning attitude. This is disallowed for now
 *
 * return true if solving solving for a thrust segment using an spacecraft with 
 * a spinning attitude. false if not.
 */
 //------------------------------------------------------------------------------
bool FileThrust::IsSolvingForThrustEventsWithSpinningAttitude()
{
   bool solvingForThrustEvent = false;
   StringArray sfors;
   for (UnsignedInt i = 0; i < segments->size(); ++i)
   {
      sfors = segments->at(i).GetStringArrayParameter("SolveFors");
      for (UnsignedInt j = 0; j < sfors.size(); ++j)
      {
         if (sfors[j] == "ThrustAngle1" || sfors[j] == "ThrustAngle2" || sfors[j] == "ThrustScaleFactor")
         {
            solvingForThrustEvent = true;
         }
      }
   }

   if (solvingForThrustEvent) 
   {
      for (UnsignedInt i = 0; i < spacecraft.size(); ++i)
      {
         Attitude * att = (Attitude *)((Spacecraft*)spacecraft[i])->GetRefObject(Gmat::ATTITUDE, "Name");
         if (att)
         {
            std::string attType = att->GetAttitudeModelName();
            if (attType == "Spinner" || attType == "PrecessingSpinner" || attType == "ThreeAxisKinematic")
            {
               return true;
            }
         }
      }
   }
   return false;
}

//------------------------------------------------------------------------------
// void FileThrust::UpdateScriptSegments(const Integer segId, const Integer parmId, const Real value)
//------------------------------------------------------------------------------
/**
 * Updates the script created thrust segment objects to ensure any updates are script accessible.
 *
 * @param segId The index of the thrust segment to update in segments.
 * @param parmId The parameter ID to update in the given thrust segment
 * @param  value Real value to update to
 *
 */
 //------------------------------------------------------------------------------
void FileThrust::UpdateScriptSegments(const Integer segId, const Integer parmId, const Real value)
{
   if (scriptSegments != NULL)
   {
      for (UnsignedInt i = 0; i < scriptSegments->size(); ++i)
      {
         if ((*scriptSegments)[i]->GetName() == segments->at(segId).GetName())
         {
            (*scriptSegments)[i]->SetRealParameter(parmId, value);
         }
      }
   }
}

//------------------------------------------------------------------------------
// void FileThrust::UpdateScriptSegments(const Integer segId, const Integer parmId, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Updates the script created thrust segment objects to ensure any updates are script accessible.
 *
 * @param segId The index of the thrust segment to update in segments.
 * @param parmId The parameter ID to update in the given thrust segment
 * @param  value Real value to update to
 *
 */
 //------------------------------------------------------------------------------
void FileThrust::UpdateScriptSegments(const Integer segId, const Integer parmId, const Real value, const Integer index)
{
   if (scriptSegments != NULL)
   {
      for (UnsignedInt i = 0; i < scriptSegments->size(); ++i)
      {
         if ((*scriptSegments)[i]->GetName() == segments->at(segId).GetName())
         {
            (*scriptSegments)[i]->SetRealParameter(parmId, value, index);
         }
      }
   }
}

//------------------------------------------------------------------------------
// void FileThrust::UpdateScriptSegments(const Integer segId, const Integer parmId, const Rvector value)
//------------------------------------------------------------------------------
/**
 * Updates the script created thrust segment objects to ensure any updates are script accessible.
 *
 * @param segId The index of the thrust segment to update in segments.
 * @param parmId The parameter ID to update in the given thrust segment
 * @param  value Rvector value to update to
 *
 */
 //------------------------------------------------------------------------------
void FileThrust::UpdateScriptSegments(const Integer segId, const Integer parmId, const Rvector value)
{
   if (scriptSegments != NULL)
   {
      for (UnsignedInt i = 0; i < scriptSegments->size(); ++i)
      {
         if ((*scriptSegments)[i]->GetName() == segments->at(segId).GetName())
         {
            (*scriptSegments)[i]->SetRvectorParameter(parmId, value);
         }
      }
   }
}


//------------------------------------------------------------------------------
// bool InSegmentInterval(GmatEpoch begin, GmatEpoch end, GmatEpoch epoch)
//------------------------------------------------------------------------------
/**
 * Checks if the epoch is in the interval between the provided beginning and
 * ending epoch, and is direction dependent on the boundaries. It is true at
 * the beginning epoch only if the direction is forward, and it is true at
 * the ending epoch only if the direction is backward.
 *
 * @param begin The beginning epoch of the interval
 * @param end The ending epoch of the interval
 * @param epoch The epoch to check if it is in the interval
 *
 * @return If the epoch is inside the interval
 */
 //------------------------------------------------------------------------------
bool FileThrust::InSegmentInterval(GmatEpoch begin, GmatEpoch end, GmatEpoch epoch)
{
   if (begin < epoch && epoch < end)
      return true;

   if (direction == 1.0 && begin == epoch)
      return true;

   if (direction == -1.0 && end == epoch)
      return true;

   return false;
}


//------------------------------------------------------------------------------
// bool InSegmentInterval(const GmatTime& begin, const GmatTime& end, const GmatTime& epoch)
//------------------------------------------------------------------------------
/**
 * Checks if the epoch is in the interval between the provided beginning and
 * ending epoch, and is direction dependent on the boundaries. It is true at
 * the beginning epoch only if the direction is forward, and it is true at
 * the ending epoch only if the direction is backward.
 *
 * @param begin The beginning epoch of the interval
 * @param end The ending epoch of the interval
 * @param epoch The epoch to check if it is in the interval
 *
 * @return If the epoch is inside the interval
 */
 //------------------------------------------------------------------------------
bool FileThrust::InSegmentInterval(const GmatTime& begin, const GmatTime& end, const GmatTime& epoch)
{
   if (begin < epoch && epoch < end)
      return true;

   if (direction == 1.0 && begin == epoch)
      return true;

   if (direction == -1.0 && end == epoch)
      return true;

   return false;
}
