//$Id$
//------------------------------------------------------------------------------
//                              ProcessNoiseBase
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. under
// contract number NNG15CR67C
//
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2018/09/17
/**
 * The base class for process noise models
 */
//------------------------------------------------------------------------------

#include "ProcessNoiseBase.hpp"
#include "NoiseException.hpp"
#include "CoordinateConverter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_CONVERSION

//---------------------------------
// static data
//---------------------------------

const std::string
ProcessNoiseBase::PARAMETER_TEXT[ProcessNoiseBaseParamCount - GmatBaseParamCount] =
{
   "CoordinateSystem",
};

const Gmat::ParameterType
ProcessNoiseBase::PARAMETER_TYPE[ProcessNoiseBaseParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECT_TYPE,      // CoordinateSystem
};


//------------------------------------------------------------------------------
// ProcessNoiseBase(const std::string &itsTypeName,
//       const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param itsTypeName The type name of the new object
 * @param itsName The name of the new object
 * @param itsShortName The short name of the new object
 */
//------------------------------------------------------------------------------
ProcessNoiseBase::ProcessNoiseBase(const std::string &itsTypeName,
       const std::string &itsName, const std::string &itsShortName) :
   GmatBase    (GmatType::GetTypeId("ProcessNoise"),itsTypeName,itsName),
   shortName        (itsShortName),
   solarSystem      (NULL),
   needsReinit      (false),
   coordSysName     ("EarthMJ2000Eq"),
   coordinateSystem (NULL),
   j2k              (NULL)
{
   objectTypes.push_back(GmatType::GetTypeId("ProcessNoise"));
   objectTypeNames.push_back("ProcessNoise");

   parameterCount = ProcessNoiseBaseParamCount;

   isInitialized = false;
}


//------------------------------------------------------------------------------
// ~ProcessNoiseBase()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ProcessNoiseBase::~ProcessNoiseBase()
{
   if (j2k)
      delete j2k;
}


//------------------------------------------------------------------------------
// ProcessNoiseBase(const ProcessNoiseBase& pnm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
ProcessNoiseBase::ProcessNoiseBase(
      const ProcessNoiseBase& pnm) :
   GmatBase (pnm),
   shortName        (pnm.shortName),
   solarSystem      (pnm.solarSystem),
   needsReinit      (false),
   coordSysName     (pnm.coordSysName),
   coordinateSystem (pnm.coordinateSystem),
   j2k              (NULL)
{
   isInitialized = false;
}



//------------------------------------------------------------------------------
// ProcessNoiseBase& operator=(const ProcessNoiseBase& pnm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
ProcessNoiseBase& ProcessNoiseBase::operator=(
      const ProcessNoiseBase& pnm)
{
   if (this != &pnm)
   {
      GmatBase::operator=(pnm);

      shortName        = pnm.shortName;
      solarSystem      = pnm.solarSystem;
      isInitialized    = false;
      needsReinit      = false;
      coordSysName     = pnm.coordSysName;
      coordinateSystem = pnm.coordinateSystem;
      j2k              = NULL;
   }
   return *this;
}


//------------------------------------------------------------------------------
// std::string GetShortName()
//------------------------------------------------------------------------------
/**
 * Get the short name for the script field
 */
//------------------------------------------------------------------------------
std::string ProcessNoiseBase::GetShortName()
{
   return shortName;
}


//---------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//---------------------------------------------------------------------------
/**
 * Set the solar system for this object
 */
//------------------------------------------------------------------------------
void ProcessNoiseBase::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}


//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Initialize the process noise object, including the coordinate systems
 */
//------------------------------------------------------------------------------
bool ProcessNoiseBase::Initialize()
{
   if (isInitialized && !needsReinit) return true;
   GmatBase::Initialize();

   // Remove any old coordinate systems
   if (j2k)
      delete j2k;

   if (!coordinateSystem)
      throw NoiseException("CoordinateSystem not defined for process noise.");

   SpacePoint *j2kBody = coordinateSystem->GetJ2000Body();

   j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq", j2kBody, NULL, NULL, j2kBody, solarSystem);

   isInitialized = true;
   needsReinit = false;

   return true;
}


//------------------------------------------------------------------------------
// void ConvertMatrix(Rmatrix &mat, const GmatTime &epoch)
//------------------------------------------------------------------------------
/**
 * Convert the covariance matrix from the input frame to the inertial frame

 * @param mat The covariance matrix to convert
 * @param epoch The epoch to evaluate the process noise coordinate conversion at.
 */
//------------------------------------------------------------------------------
void ProcessNoiseBase::ConvertMatrix(Rmatrix &mat, const GmatTime &epoch)
{
   #ifdef DEBUG_CONVERSION
      MessageInterface::ShowMessage("Entering ProcessNoiseBase::ConvertMatrix()\n");
      MessageInterface::ShowMessage("   Input matrix:\n");
      for (UnsignedInt ii = 0U; ii < mat.GetNumRows(); ii++)
         MessageInterface::ShowMessage("   [ %s ]\n", mat.ToRowString(ii, 6).c_str());
      MessageInterface::ShowMessage("\n");
   #endif

   if (!isInitialized || needsReinit)
      Initialize();

   if (coordinateSystem == j2k)
   {
      #ifdef DEBUG_CONVERSION
         MessageInterface::ShowMessage("   Input and output coordinate systems match, no transformation required.\n");
         MessageInterface::ShowMessage("Exiting ProcessNoiseBase::ConvertMatrix()\n\n");
      #endif

      return; // No conversion needed
   }

   Integer stateSize = mat.GetNumRows();

   CoordinateConverter cc;
   Rvector inState(stateSize); // At origin of local coordinate system
   Rvector outState(stateSize); // Placeholder, only the rotation matrix is needed
   cc.Convert(epoch, inState, coordinateSystem, outState, j2k, true, false);
   Rmatrix33 vnbRot = cc.GetLastRotationMatrix();

   Rmatrix transform = Rmatrix::Identity(stateSize);

   for (UnsignedInt ii = 0; ii < 3U; ii++)
   {
      for (UnsignedInt jj = 0; jj < 3U; jj++)
      {
         transform(ii, jj) = vnbRot(ii, jj);
         transform(ii+3, jj+3) = vnbRot(ii, jj);
      }
   }

   #ifdef DEBUG_CONVERSION
      MessageInterface::ShowMessage("   Transformation matrix:\n");
      for (UnsignedInt ii = 0U; ii < transform.GetNumRows(); ii++)
         MessageInterface::ShowMessage("   [ %s ]\n", transform.ToRowString(ii, 6).c_str());
      MessageInterface::ShowMessage("\n");
   #endif

   mat = transform*mat*transform.Transpose();

   #ifdef DEBUG_CONVERSION
      MessageInterface::ShowMessage("   Output matrix:\n");
      for (UnsignedInt ii = 0U; ii < mat.GetNumRows(); ii++)
         MessageInterface::ShowMessage("   [ %s ]\n", mat.ToRowString(ii, 6).c_str());
      MessageInterface::ShowMessage("Exiting ProcessNoiseBase::ConvertMatrix()\n\n");
   #endif
}



//------------------------------------------------------------------------------
// void ConvertMatrix(Rmatrix &mat, const Real &epoch)
//------------------------------------------------------------------------------
/**
 * Convert the covariance matrix from the input frame to the inertial frame
 * Overload for non-GmatTime epochs

 * @param mat The covariance matrix to convert
 * @param epoch The epoch to evaluate the process noise coordinate conversion at.
 */
//------------------------------------------------------------------------------
void ProcessNoiseBase::ConvertMatrix(Rmatrix &mat, const Real &epoch)
{
   #ifdef DEBUG_CONVERSION
      MessageInterface::ShowMessage("Entering ProcessNoiseBase::ConvertMatrix()\n");
      MessageInterface::ShowMessage("   Input matrix:\n");
      for (UnsignedInt ii = 0U; ii < mat.GetNumRows(); ii++)
         MessageInterface::ShowMessage("   [ %s ]\n", mat.ToRowString(ii, 6).c_str());
      MessageInterface::ShowMessage("\n");
   #endif

   if (!isInitialized || needsReinit)
      Initialize();

   if (coordinateSystem == j2k)
   {
      #ifdef DEBUG_CONVERSION
         MessageInterface::ShowMessage("   Input and output coordinate systems match, no transformation required.\n");
         MessageInterface::ShowMessage("Exiting ProcessNoiseBase::ConvertMatrix()\n\n");
      #endif

      return; // No conversion needed
   }

   Integer stateSize = mat.GetNumRows();

   CoordinateConverter cc;
   Rvector inState(stateSize); // At origin of local coordinate system
   Rvector outState(stateSize); // Placeholder, only the rotation matrix is needed
   cc.Convert(epoch, inState, coordinateSystem, outState, j2k, true, false);
   Rmatrix33 vnbRot = cc.GetLastRotationMatrix();

   Rmatrix transform = Rmatrix::Identity(stateSize);

   for (UnsignedInt ii = 0; ii < 3U; ii++)
   {
      for (UnsignedInt jj = 0; jj < 3U; jj++)
      {
         transform(ii, jj) = vnbRot(ii, jj);
         transform(ii+3, jj+3) = vnbRot(ii, jj);
      }
   }

   #ifdef DEBUG_CONVERSION
      MessageInterface::ShowMessage("   Transformation matrix:\n");
      for (UnsignedInt ii = 0U; ii < transform.GetNumRows(); ii++)
         MessageInterface::ShowMessage("   [ %s ]\n", transform.ToRowString(ii, 6).c_str());
      MessageInterface::ShowMessage("\n");
   #endif

   mat = transform*mat*transform.Transpose();

   #ifdef DEBUG_CONVERSION
      MessageInterface::ShowMessage("   Output matrix:\n");
      for (UnsignedInt ii = 0U; ii < mat.GetNumRows(); ii++)
         MessageInterface::ShowMessage("   [ %s ]\n", mat.ToRowString(ii, 6).c_str());
      MessageInterface::ShowMessage("Exiting ProcessNoiseBase::ConvertMatrix()\n\n");
   #endif
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ProcessNoiseBase::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ProcessNoiseBaseParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ProcessNoiseBase::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ProcessNoiseBaseParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ProcessNoiseBase::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ProcessNoiseBaseParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer ProcessNoiseBase::GetParameterID(const std::string &str) const
{
   for (int i=GmatBaseParamCount; i<ProcessNoiseBaseParamCount; i++)
   {
      if (str == ProcessNoiseBase::PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ProcessNoiseBase::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case COORD_SYS:
      return coordSysName;
   default:
      return GmatBase::GetStringParameter(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ProcessNoiseBase::GetStringParameter(const std::string &label) const
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
bool ProcessNoiseBase::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case COORD_SYS:
      coordSysName = value;
      return true;

   default:
      return GmatBase::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ProcessNoiseBase::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * This method returns the name of the referenced objects.
 *
 * @param type The type of the reference object to return
 *
 * @return name of the reference object of the requested type.
 */
//------------------------------------------------------------------------------
std::string ProcessNoiseBase::GetRefObjectName(const UnsignedInt type) const
{
   if (type == Gmat::COORDINATE_SYSTEM)
      return coordSysName;

   return GmatBase::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ProcessNoiseBase::HasRefObjectTypeArray()
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
const ObjectTypeArray& ProcessNoiseBase::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::COORDINATE_SYSTEM);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//---------------------------------------------------------------------------
/**
 * Returns the names of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& ProcessNoiseBase::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();

   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::COORDINATE_SYSTEM)
      refObjectNames.push_back(coordSysName);

   return refObjectNames;
}


//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Returns the reference object pointer.
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return reference object pointer.
 */
//---------------------------------------------------------------------------
GmatBase* ProcessNoiseBase::GetRefObject(const UnsignedInt type,
                                   const std::string &name)
{
   if (type == Gmat::COORDINATE_SYSTEM)
      return coordinateSystem;

   return GmatBase::GetRefObject(type, name);
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
bool ProcessNoiseBase::SetRefObject(GmatBase *obj, const UnsignedInt type,
                        const std::string &name)
{
   switch (type)
   {
   case Gmat::COORDINATE_SYSTEM:
      {
         if (coordSysName == name)
            coordinateSystem = (CoordinateSystem*)obj;

         return true;
      }
   default:
      return GmatBase::SetRefObject(obj, type, name);
   }
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames reference object name used in this class.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 *
 * @return true if object name changed, false if not.
 */
//---------------------------------------------------------------------------
bool ProcessNoiseBase::RenameRefObject(const UnsignedInt type,
                           const std::string &oldName,
                           const std::string &newName)
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (coordSysName == oldName)
         coordSysName = newName;
   }

   return true;
}
