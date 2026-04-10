//$Id$
//------------------------------------------------------------------------------
//                             EstimatedParameter
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
// Created: 2020/02/04
/**
 * The resource for estimated parameters
 */
//------------------------------------------------------------------------------

#include "EstimatedParameter.hpp"
#include "EstimatedParameterException.hpp"
#include "FirstOrderGaussMarkov.hpp"
#include "StringUtil.hpp"

//---------------------------------
// static data
//---------------------------------

const std::string
EstimatedParameter::PARAMETER_TEXT[EstimatedParameterParamCount - GmatBaseParamCount] =
{
   "Model",
   "SolveFor",
   "SteadyStateValue",
   "SteadyStateSigma",
   "HalfLife",
};

const Gmat::ParameterType
EstimatedParameter::PARAMETER_TYPE[EstimatedParameterParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECT_TYPE,      // "Model",
   Gmat::STRING_TYPE,      // "SolveFor",
   Gmat::REAL_TYPE,        // "SteadyStateValue",
   Gmat::REAL_TYPE,        // "SteadyStateSigma",
   Gmat::REAL_TYPE,        // "HalfLife",
};

//------------------------------------------------------------------------------
// EstimatedParameter(const std::string &itsTypeName,
//       const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the new object
 */
//------------------------------------------------------------------------------
EstimatedParameter::EstimatedParameter(const std::string &name) :
   GmatBase    (GmatType::GetTypeId("EstimatedParameter"),"EstimatedParameter",name),
   estimatedParameterModel(NULL)
{
   objectTypes.push_back(GmatType::GetTypeId("EstimatedParameter"));
   objectTypeNames.push_back("EstimatedParameter");

   parameterCount = EstimatedParameterParamCount;

   ownedObjectCount += 1;

   estimatedParameterModelName = "InternalEstimatedParameter";
   estimatedParameterModel = new FirstOrderGaussMarkov("");
}


//------------------------------------------------------------------------------
// ~EstimatedParameter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EstimatedParameter::~EstimatedParameter()
{
   if (estimatedParameterModel)
      delete estimatedParameterModel;
}


//------------------------------------------------------------------------------
// EstimatedParameter(const EstimatedParameter& ep)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EstimatedParameter::EstimatedParameter(
      const EstimatedParameter& ep) :
   GmatBase (ep),
   estimatedParameterModelName(ep.estimatedParameterModelName)
{
   if (ep.estimatedParameterModel)
   {
      estimatedParameterModelName = ep.estimatedParameterModel->GetTypeName();
      estimatedParameterModel = (EstimatedParameterModel*) ep.estimatedParameterModel->Clone();
   }
}



//------------------------------------------------------------------------------
// EstimatedParameter& operator=(const EstimatedParameter& ep)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
EstimatedParameter& EstimatedParameter::operator=(
      const EstimatedParameter& ep)
{
   if (this != &ep)
   {
      GmatBase::operator=(ep);

      estimatedParameterModelName = ep.estimatedParameterModelName;

      if (estimatedParameterModel)
         delete estimatedParameterModel;
   
      if (ep.estimatedParameterModel)
      {
         estimatedParameterModelName = ep.estimatedParameterModel->GetTypeName();
         estimatedParameterModel = (EstimatedParameterModel*) ep.estimatedParameterModel->Clone();
      }
      else
         estimatedParameterModel = NULL;
   }
   return *this;
}


//------------------------------------------------------------------------------
// Rmatrix GetProcessNoise(GmatEpoch elapsedTime, const Real forceVector[3])
//------------------------------------------------------------------------------
/**
 * Get the process noise for a specified elaped time
 *
 * @param elapsedTime The elapsed time to evaluate the process noise over.
 * @param accelVector The epoch to evaluate the process noise coordinate conversion at.
 *
 * @return The process noise matrix.
 */
 //------------------------------------------------------------------------------
Rmatrix EstimatedParameter::GetProcessNoise(GmatEpoch elapsedTime, const Real accelVector[3])
{
   if (estimatedParameterModel)
      return estimatedParameterModel->GetProcessNoise(elapsedTime, accelVector);

   Rmatrix result;
   return result;
}


//------------------------------------------------------------------------------
// void SetConversionFactor(Real factor)
//------------------------------------------------------------------------------
/**
 * Set the value to convert from the epsilon value to the nominal value
 *
 * @param factor The conversion factor for the EstimatedParameter
 */
 //------------------------------------------------------------------------------
void EstimatedParameter::SetConversionFactor(Real factor)
{
   if (estimatedParameterModel)
      return estimatedParameterModel->SetConversionFactor(factor);
}


//------------------------------------------------------------------------------
// void SetEstimatedParameterModel(EstimatedParameterModel *param)
//------------------------------------------------------------------------------
/**
 * Sets internal estimated parameter model pointer to given estimated parameter model.
 *
 *@param param estimated parameter model pointer to set internal estimated parameter model to
 */
//------------------------------------------------------------------------------
void EstimatedParameter::SetEstimatedParameterModel(EstimatedParameterModel *param)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("EstimatedParameter::SetEstimatedParameterModel() this=<%p> '%s' entered, estimatedParameterModel=<%p>, "
       "param=<%p>\n", this, GetName().c_str(), estimatedParameterModel, param);
   #endif

   if (param == NULL)
      throw EstimatedParameterException("SetEstimatedParameterModel() failed: param is NULL");
   
   delete estimatedParameterModel;

   estimatedParameterModel = (EstimatedParameterModel *)param->Clone();

   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("EstimatedParameter::SetEstimatedParameterModel() this=<%p> '%s' leaving, estimatedParameterModel=<%p>, "
       "param=<%p>\n", this, GetName().c_str(), estimatedParameterModel, param);
   #endif
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
bool EstimatedParameter::HasRefObjectTypeArray()
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
const ObjectTypeArray& EstimatedParameter::GetRefObjectTypeArray()
{
   refObjectTypes.clear();

   refObjectTypes.push_back(GmatType::GetTypeId("EstimatedParameterModel"));

   if (estimatedParameterModel)
   {
      ObjectTypeArray paramModelRefTypes;
      paramModelRefTypes = estimatedParameterModel->GetRefObjectTypeArray();

      for (ObjectTypeArray::iterator i = paramModelRefTypes.begin(); i != paramModelRefTypes.end(); ++i)
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
const StringArray& EstimatedParameter::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();

   if (estimatedParameterModelName != "" && estimatedParameterModelName != "InternalEstimatedParameter")
      if (type == GmatType::GetTypeId("EstimatedParameterModel") || type == Gmat::UNKNOWN_OBJECT)
         refObjectNames.push_back(estimatedParameterModelName);

   if (estimatedParameterModel)
   {
      StringArray paramModelRefNames;
      paramModelRefNames = estimatedParameterModel->GetRefObjectNameArray(type);

      for (StringArray::iterator i = paramModelRefNames.begin(); i != paramModelRefNames.end(); ++i)
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
bool EstimatedParameter::RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("EstimatedParameter::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
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
GmatBase* EstimatedParameter::GetRefObject(const UnsignedInt type,
                                   const std::string &name)
{
   if (type == GmatType::GetTypeId("EstimatedParameterModel"))
      return estimatedParameterModel;

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
bool EstimatedParameter::SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("EstimatedParameter::SetRefObject() entered, obj=<%p><%s> '%s', type=%d, name='%s'\n",
       obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
   if (obj == NULL)
      return false;

   if (type == GmatType::GetTypeId("EstimatedParameterModel"))
   {
      SetEstimatedParameterModel((EstimatedParameterModel*)obj);
      return true;
   }

   return GmatBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  GmatBase* GetOwnedObject(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This method returns the unnamed objects owned by the EstimatedParameter.
 *
 * The current implementation only contains one EstimatedParameter owned object: the 
 * EstimatedParameter model.
 * 
 * @return Pointer to the owned object.
 */
//------------------------------------------------------------------------------
GmatBase* EstimatedParameter::GetOwnedObject(Integer whichOne)
{
   if (whichOne == ownedObjectCount - 1)
      return estimatedParameterModel;
   return GmatBase::GetOwnedObject(whichOne);
}


//---------------------------------------------------------------------------
// bool IsOwnedObject(Integer id) const
//---------------------------------------------------------------------------
bool EstimatedParameter::IsOwnedObject(Integer id) const
{
   if (id == EST_PARAM_TYPE)
      return true;

   return GmatBase::IsOwnedObject(id);
}


//------------------------------------------------------------------------------
//  EstimatedParameter* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EstimatedParameter.
 *
 * @return clone of the EstimatedParameter.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EstimatedParameter::Clone(void) const
{
   return (new EstimatedParameter(*this));
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType EstimatedParameter::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EstimatedParameterParamCount)
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
std::string EstimatedParameter::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EstimatedParameterParamCount)
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
std::string EstimatedParameter::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EstimatedParameterParamCount)
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
Integer EstimatedParameter::GetParameterID(const std::string &str) const
{
   for (int i=GmatBaseParamCount; i<EstimatedParameterParamCount; i++)
   {
      if (str == EstimatedParameter::PARAMETER_TEXT[i - GmatBaseParamCount])
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
bool EstimatedParameter::IsParameterReadOnly(const Integer id) const
{
   // All these are owned parameters
   if (id >= SOLVE_FOR && id < EstimatedParameterParamCount)
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
bool EstimatedParameter::IsParameterReadOnly(const std::string &label) const
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
bool EstimatedParameter::IsParameterCommandModeSettable(const Integer id) const
{
   if (id >= SOLVE_FOR && id < EstimatedParameterParamCount)
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("EstimatedParameterModel"));
      return estimatedParameterModel->IsParameterCommandModeSettable(actualId);
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
Real EstimatedParameter::GetRealParameter(const Integer id) const
{
   if (id == VALUE || id == SIGMA || id == HALF_LIFE)
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("EstimatedParameterModel"));
      return estimatedParameterModel->GetRealParameter(actualId);
   }

   return GmatBase::GetRealParameter(id);
}

//---------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
Real EstimatedParameter::GetRealParameter(const std::string &label) const
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
Real EstimatedParameter::SetRealParameter(const Integer id,
                                         const Real value)
{
   if (id == VALUE || id == SIGMA || id == HALF_LIFE)
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("EstimatedParameterModel"));
      return estimatedParameterModel->SetRealParameter(actualId, value);
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
Real EstimatedParameter::SetRealParameter(const std::string &label,
                                         const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string EstimatedParameter::GetStringParameter(const Integer id) const
{
   std::string name;

   switch (id)
   {
   case EST_PARAM_TYPE:
      if (estimatedParameterModel)
         name = estimatedParameterModel->GetShortName();
      else
         name = "UndefinedEstimatedParameter";
      break;
   case SOLVE_FOR:
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("EstimatedParameterModel"));
      return estimatedParameterModel->GetStringParameter(actualId);
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
std::string EstimatedParameter::GetStringParameter(const std::string &label) const
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
bool EstimatedParameter::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case EST_PARAM_TYPE:
   {
      estimatedParameterModelName = value;
      return true;
   }

   case SOLVE_FOR:
   {
      if (value == instanceName)
      {
         // Want to throw allowed value exception instead of name exception first
         StringArray allowedSolveFors = estimatedParameterModel->GetAllowedSolveFors();
         if (find(allowedSolveFors.begin(), allowedSolveFors.end(), value) != allowedSolveFors.end())
         {
            throw EstimatedParameterException("Error: The value set to SolveFors in "
               "EstimatedParameter \"" + instanceName + "\" is not allowed to match its name");
            return false;
         }
         else
         {
            std::string allowedList;

            if (allowedSolveFors.size() > 0)
            {
               allowedList = allowedSolveFors[0];
               for (UnsignedInt n = 1; n < allowedSolveFors.size(); n++)
                  allowedList += ", " + allowedSolveFors[n];
            }

            throw EstimatedParameterException("Error: The value \"" + value + "\" is not allowed "
               "for field \"SolveFors\". Allowed values are {" + allowedList + "}");
            return false;
         }
      }

      // Get actual id
      Integer actualId = GetOwnedObjectId(id, GmatType::GetTypeId("EstimatedParameterModel"));
      return estimatedParameterModel->SetStringParameter(actualId, value);
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
bool EstimatedParameter::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Integer GetOwnedObjectId(Integer id, UnsignedInt objType) const
//------------------------------------------------------------------------------
/**
 * Returns property id of owned object.
 */
//------------------------------------------------------------------------------
Integer EstimatedParameter::GetOwnedObjectId(Integer id, UnsignedInt objType) const
{
   Integer actualId = -1;
   
   try
   {
      if (objType == GmatType::GetTypeId("EstimatedParameterModel"))
      {
         if (estimatedParameterModel == NULL)
            throw EstimatedParameterException
               ("EstimatedParameter::GetOwnedObjectId() failed: Estimated Parameter is NULL");
         
         actualId = estimatedParameterModel->GetParameterID(GetParameterText(id));
      }
   }
   catch (BaseException &)
   {
      throw;
   }
   
   return actualId;
}
