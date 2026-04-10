//$Id$
//------------------------------------------------------------------------------
//                              ProcessNoiseModel
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
 * The resource for process noise models
 */
//------------------------------------------------------------------------------

#include "ProcessNoiseModel.hpp"
#include "NoiseException.hpp"
#include "SNCProcessNoise.hpp"
#include "StringUtil.hpp"

//#define DEBUG_SET

#ifdef DEBUG_SET
   #include "MessageInterface.hpp"
#endif


//---------------------------------
// static data
//---------------------------------

const std::string
ProcessNoiseModel::PARAMETER_TEXT[ProcessNoiseModelParamCount - GmatBaseParamCount] =
{
   "Type",
   "UpdateTimeStep",
   "CoordinateSystem",
   "RateVector",
   "AccelNoiseSigma",
};

const Gmat::ParameterType
ProcessNoiseModel::PARAMETER_TYPE[ProcessNoiseModelParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECT_TYPE,      // "Type",
   Gmat::REAL_TYPE,     // "UPDATE_TIME_STEP",
   Gmat::ENUMERATION_TYPE, // "CoordinateSystem",
   Gmat::RVECTOR_TYPE,     // "RateVector",
   Gmat::RVECTOR_TYPE,     // "AccelNoiseSigma",
};

//------------------------------------------------------------------------------
// ProcessNoiseModel(const std::string &itsTypeName,
//       const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the new object
 */
//------------------------------------------------------------------------------
ProcessNoiseModel::ProcessNoiseModel(const std::string &name) :
   GmatBase    (GmatType::GetTypeId("ProcessNoiseModel"),"ProcessNoiseModel",name),
   noiseModel  (NULL),
   solarSystem (NULL)
{
   objectTypes.push_back(GmatType::GetTypeId("ProcessNoiseModel"));
   objectTypeNames.push_back("ProcessNoiseModel");

   parameterCount = ProcessNoiseModelParamCount;

   ownedObjectCount += 1;

   noiseModelName = "InternalNoiseModel";
   noiseModel = new SNCProcessNoise("");
}


//------------------------------------------------------------------------------
// ~ProcessNoiseModel()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ProcessNoiseModel::~ProcessNoiseModel()
{
   if (noiseModel)
      delete noiseModel;
}


//------------------------------------------------------------------------------
// ProcessNoiseModel(const ProcessNoiseModel& pnm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
ProcessNoiseModel::ProcessNoiseModel(
      const ProcessNoiseModel& pnm) :
   GmatBase (pnm),
   noiseModelName(pnm.noiseModelName),
   solarSystem(pnm.solarSystem),
   updateTimeStep(pnm.updateTimeStep)
{
   if (pnm.noiseModel)
   {
      noiseModelName = pnm.noiseModel->GetTypeName();
      noiseModel = (ProcessNoiseBase*) pnm.noiseModel->Clone();
   }
}



//------------------------------------------------------------------------------
// ProcessNoiseModel& operator=(const ProcessNoiseModel& pnm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
ProcessNoiseModel& ProcessNoiseModel::operator=(
      const ProcessNoiseModel& pnm)
{
   if (this != &pnm)
   {
      GmatBase::operator=(pnm);

      noiseModelName = pnm.noiseModelName;
      updateTimeStep = pnm.updateTimeStep;

      if (noiseModel)
         delete noiseModel;
   
      if (pnm.noiseModel)
      {
         noiseModelName = pnm.noiseModel->GetTypeName();
         noiseModel = (ProcessNoiseBase*) pnm.noiseModel->Clone();
      }
      else
         noiseModel = NULL;

      solarSystem = pnm.solarSystem;
   }
   return *this;
}


//---------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//---------------------------------------------------------------------------
/**
 * Set the solar system for this object
 */
//------------------------------------------------------------------------------
void ProcessNoiseModel::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
   if (noiseModel) noiseModel->SetSolarSystem(ss);
}


//------------------------------------------------------------------------------
// Rmatrix66 GetProcessNoise(const GmatEpoch elapsedTime, const GmatTime &epoch)
//------------------------------------------------------------------------------
/**
 * Get the process noise for a specified elaped time
 *
 * @param elapsedTime The elapsed time to evaluate the process noise over.
 * @param epoch The epoch to evaluate the process noise coordinate conversion at.
 *
 * @return The process noise matrix.
 */
//------------------------------------------------------------------------------
Rmatrix66 ProcessNoiseModel::GetProcessNoise(const GmatEpoch elapsedTime, const GmatTime &epoch)
{
   Rmatrix66 result(false);

   if (noiseModel)
      result = noiseModel->GetProcessNoise(elapsedTime, epoch);

   return result;
}


//------------------------------------------------------------------------------
// void SetNoiseModel(ProcessNoiseBase *noise)
//------------------------------------------------------------------------------
/**
 * Sets internal noise model pointer to given noise model.
 *
 *@param noise noise model pointer to set internal noise model to
 */
//------------------------------------------------------------------------------
void ProcessNoiseModel::SetNoiseModel(ProcessNoiseBase *noise)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("ProcessNoiseModel::SetNoiseModel() this=<%p> '%s' entered, noiseModel=<%p>, "
       "noise=<%p>\n", this, GetName().c_str(), noiseModel, noise);
   #endif

   if (noise == NULL)
      throw NoiseException("SetNoiseModel() failed: noise is NULL");
   
   delete noiseModel;

   noiseModel = (ProcessNoiseBase *)noise->Clone();

   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("ProcessNoiseModel::SetNoiseModel() this=<%p> '%s' leaving, noiseModel=<%p>, "
       "noise=<%p>\n", this, GetName().c_str(), noiseModel, noise);
   #endif
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * This method returns the name of the referenced objects.
 *
 * @return name of the reference object of the requested type.
 */
//------------------------------------------------------------------------------
std::string ProcessNoiseModel::GetRefObjectName(const UnsignedInt type) const
{
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (noiseModel)
         return noiseModel->GetRefObjectName(type);
      else
         throw NoiseException("Unable to get Reference Object for " + typeName +
                              " named \"" + instanceName + "\", as the noise model is NULL\n");
   }

   return GmatBase::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ProcessNoiseModel::HasRefObjectTypeArray()
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
const ObjectTypeArray& ProcessNoiseModel::GetRefObjectTypeArray()
{
   refObjectTypes.clear();

   refObjectTypes.push_back(GmatType::GetTypeId("ProcessNoise"));

   if (noiseModel)
   {
      ObjectTypeArray noiseModelRefTypes;
      noiseModelRefTypes = noiseModel->GetRefObjectTypeArray();

      for (ObjectTypeArray::iterator i = noiseModelRefTypes.begin(); i != noiseModelRefTypes.end(); ++i)
         if (find(refObjectTypes.begin(), refObjectTypes.end(), *i) == refObjectTypes.end())
            refObjectTypes.push_back(*i);
   }

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
const StringArray& ProcessNoiseModel::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();

   if (noiseModelName != "" && noiseModelName != "InternalNoiseModel")
      if (type == GmatType::GetTypeId("ProcessNoise") || type == Gmat::UNKNOWN_OBJECT)
         refObjectNames.push_back(noiseModelName);

   if (noiseModel)
   {
      StringArray noiseModelRefNames;
      noiseModelRefNames = noiseModel->GetRefObjectNameArray(type);

      for (StringArray::iterator i = noiseModelRefNames.begin(); i != noiseModelRefNames.end(); ++i)
         if (find(refObjectNames.begin(), refObjectNames.end(), *i) == refObjectNames.end())
            refObjectNames.push_back(*i);
   }

   return refObjectNames;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames reference objects used in this class.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 * 
 * @return true if object name changed, false if not.
 */
//---------------------------------------------------------------------------
bool ProcessNoiseModel::RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("CoordinateBase::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif

   return true;
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
GmatBase* ProcessNoiseModel::GetRefObject(const UnsignedInt type,
                                   const std::string &name)
{
   switch (type)
   {
      case Gmat::COORDINATE_SYSTEM:
         if (noiseModel)
            return noiseModel->GetRefObject(type, name);
         break;

      default:
         break;
   }

   if (type == GmatType::GetTypeId("ProcessNoiseModel"))
      return noiseModel;

   return GmatBase::GetRefObject(type, name);
}


//---------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "");
//---------------------------------------------------------------------------
/*
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool ProcessNoiseModel::SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("ProcessNoiseModel::SetRefObject() entered, obj=<%p><%s> '%s', type=%d, name='%s'\n",
       obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
   if (obj == NULL)
      return false;

   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (noiseModel)
      {
         noiseModel->SetRefObject(obj, type, name);
         return true;
      }
   }

   if (type == GmatType::GetTypeId("ProcessNoise"))
   {
      SetNoiseModel((ProcessNoiseBase*)obj);
      return true;
   }

   return GmatBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  GmatBase* GetOwnedObject(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This method returns the unnamed objects owned by the ProcessNoiseModel.
 *
 * The current implementation only contains one ProcessNoiseModel owned object: the 
 * Noise model.
 * 
 * @return Pointer to the owned object.
 */
//------------------------------------------------------------------------------
GmatBase* ProcessNoiseModel::GetOwnedObject(Integer whichOne)
{
   if (whichOne == ownedObjectCount - 1)
      return noiseModel;
   return GmatBase::GetOwnedObject(whichOne);
}


//---------------------------------------------------------------------------
// bool IsOwnedObject(Integer id) const
//---------------------------------------------------------------------------
bool ProcessNoiseModel::IsOwnedObject(Integer id) const
{
   if (id == NOISE_TYPE)
      return true;

   return GmatBase::IsOwnedObject(id);
}


//------------------------------------------------------------------------------
//  ProcessNoiseModel* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ProcessNoiseModel.
 *
 * @return clone of the ProcessNoiseModel.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ProcessNoiseModel::Clone(void) const
{
   return (new ProcessNoiseModel(*this));
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ProcessNoiseModel::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ProcessNoiseModelParamCount)
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
std::string ProcessNoiseModel::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ProcessNoiseModelParamCount)
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
std::string ProcessNoiseModel::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ProcessNoiseModelParamCount)
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
Integer ProcessNoiseModel::GetParameterID(const std::string &str) const
{
   for (int i=GmatBaseParamCount; i<ProcessNoiseModelParamCount; i++)
   {
      if (str == ProcessNoiseModel::PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool ProcessNoiseModel::IsParameterReadOnly(const Integer id) const
{
   // All these are owned parameters
   if (id >= COORD_SYS && id < ProcessNoiseModelParamCount)
      return true;

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
bool ProcessNoiseModel::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
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
bool ProcessNoiseModel::IsParameterCommandModeSettable(const Integer id) const
{
   if (id >= COORD_SYS && id < ProcessNoiseModelParamCount)
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("ProcessNoise"));
      return noiseModel->IsParameterCommandModeSettable(actualId);
   }

   return GmatBase::IsParameterCommandModeSettable(id);
}


//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
Real ProcessNoiseModel::GetRealParameter(const Integer id) const
{
   if (id == UPDATE_TIME_STEP)
   {
      return updateTimeStep;
   }

   return GmatBase::GetRealParameter(id);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
Real ProcessNoiseModel::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //---------------------------------------------------------------------------
Real ProcessNoiseModel::SetRealParameter(const Integer id,
   const Real value)
{
   if (id == UPDATE_TIME_STEP)
   {
      if (value < 0)
      {
         throw NoiseException("UpdateTimeStep cannot be negative.");
      }
      updateTimeStep = value;
      return updateTimeStep;
   }

   return GmatBase::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
Real ProcessNoiseModel::SetRealParameter(const std::string &label,
   const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}



//---------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id,
//                        const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real ProcessNoiseModel::GetRealParameter(const Integer id,
                                         const Integer index) const
{
   if (id == RATE_VECTOR || id == ACCEL_SIGMA_VECTOR)
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("ProcessNoise"));
      return noiseModel->GetRealParameter(actualId, index);
   }

   return GmatBase::GetRealParameter(id, index);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label,
//                       const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real ProcessNoiseModel::GetRealParameter(const std::string &label,
                                         const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value,
//                        const Integer index)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real ProcessNoiseModel::SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index)
{
   if (id == RATE_VECTOR || id == ACCEL_SIGMA_VECTOR)
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("ProcessNoise"));
      return noiseModel->SetRealParameter(actualId, value, index);
   }

   return GmatBase::SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Real ProcessNoiseModel::SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ProcessNoiseModel::GetStringParameter(const Integer id) const
{
   std::string name;

   switch (id)
   {
   case NOISE_TYPE:
      if (noiseModel)
         name = noiseModel->GetShortName();
      else
         name = "UndefinedProcessNoise";
      break;
   case COORD_SYS:
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("ProcessNoise"));
      return noiseModel->GetStringParameter(actualId);
   }
   default:
      return GmatBase::GetStringParameter(id);
   }

   return name;
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string ProcessNoiseModel::GetStringParameter(const std::string &label) const
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
bool ProcessNoiseModel::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case NOISE_TYPE:
   {
      noiseModelName = value;
      return true;
   }

   case COORD_SYS:
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("ProcessNoise"));
      return noiseModel->SetStringParameter(actualId, value);
   }

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
bool ProcessNoiseModel::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& ProcessNoiseModel::GetRvectorParameter(const Integer id) const
{
   std::string name;

   switch (id)
   {
   case RATE_VECTOR:
   case ACCEL_SIGMA_VECTOR:
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("ProcessNoise"));
      return noiseModel->GetRvectorParameter(actualId);
   }
   default:
      return GmatBase::GetRvectorParameter(id);
   }
}

//------------------------------------------------------------------------------
// const Rvector& GetRvectorParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& ProcessNoiseModel::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const Rvector& SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& ProcessNoiseModel::SetRvectorParameter(const Integer id, const Rvector &value)
{
   switch (id)
   {
   case RATE_VECTOR:
   case ACCEL_SIGMA_VECTOR:
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("ProcessNoise"));
      return noiseModel->SetRvectorParameter(actualId, value);
   }
   default:
      return GmatBase::SetRvectorParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// const Rvector& SetStringParameter(const std::string &label, const Rvector &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const Rvector& ProcessNoiseModel::SetRvectorParameter(const std::string &label,
                                   const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Integer GetOwnedObjectId(Integer id, UnsignedInt objType) const
//------------------------------------------------------------------------------
/**
 * Returns property id of owned object.
 */
//------------------------------------------------------------------------------
Integer ProcessNoiseModel::GetOwnedObjectId(Integer id, UnsignedInt objType) const
{
   Integer actualId = -1;
   
   try
   {
      if (objType == GmatType::GetTypeId("ProcessNoise"))
      {
         if (noiseModel == NULL)
            throw NoiseException
               ("ProcessNoiseModel::GetOwnedObjectId() failed: Process Noise is NULL");
         
         actualId = noiseModel->GetParameterID(GetParameterText(id));
      }
   }
   catch (BaseException &)
   {
      throw;
   }
   
   return actualId;
}
