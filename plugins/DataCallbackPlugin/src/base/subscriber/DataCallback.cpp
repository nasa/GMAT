//$Id: MatlabWorkspace.cpp 9460 2011-04-21 22:03:28Z rmathur $
//------------------------------------------------------------------------------
//                                  MatlabWorkspace
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// number S-67573-G
//
// Author: Ravi Mathur, Emergent Space Technologies, Inc.
// Created: December 17, 2014
//
/**
 * Implements DataCallback class which sends data to a callback function.
 */
//------------------------------------------------------------------------------

#include "DataCallback.hpp"

#include "MessageInterface.hpp"
#include "Parameter.hpp"
#include "SubscriberException.hpp"
#include "StringUtil.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
DataCallback::PARAMETER_TEXT[DataCallbackParamCount - SubscriberParamCount] =
{
  "DataElements"
};

const Gmat::ParameterType
DataCallback::PARAMETER_TYPE[DataCallbackParamCount - SubscriberParamCount] =
{
  Gmat::OBJECTARRAY_TYPE // "DataElements"
};

//------------------------------------------------------------------------------
// DataCallback(const std::string &type, const std::string &name,
//            const std::string &fileName)
//------------------------------------------------------------------------------
/**
 *  Constructor
 */
//------------------------------------------------------------------------------
DataCallback::DataCallback(const std::string &type, const std::string &name,
    Parameter *firstParam)
: Subscriber(type, name),
  mCallbackFcn(NULL),
  mUserData(NULL)
{
  objectTypes.push_back(Gmat::SUBSCRIBER);
  objectTypeNames.push_back("DataCallback");
  blockCommandModeAssignment = false;

  mNumParams = 0;
  if(firstParam != NULL)
    AddParameter(firstParam->GetName(), mNumParams);

  parameterCount = DataCallbackParamCount;
}

//------------------------------------------------------------------------------
// ~DataCallback(void)
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
DataCallback::~DataCallback(void)
{
}

//------------------------------------------------------------------------------
// DataCallback(const DataCallback &rf)
//------------------------------------------------------------------------------
/**
 * Copy Constructor
 */
//------------------------------------------------------------------------------
  DataCallback::DataCallback(const DataCallback &dc)
: Subscriber(dc),
  mCallbackFcn(dc.mCallbackFcn),
  mUserData(dc.mUserData)
{
  mParams = dc.mParams;
  mNumParams = dc.mNumParams;
  mParamNames = dc.mParamNames;
  mAllRefObjectNames = dc.mAllRefObjectNames;

  parameterCount = DataCallbackParamCount;
}

//------------------------------------------------------------------------------
// DataCallback& DataCallback::operator=(const DataCallback& dc)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
DataCallback& DataCallback::operator=(const DataCallback& dc)
{
  if(this == &dc) return *this;

  Subscriber::operator=(dc);

  mParams = dc.mParams;
  mNumParams = dc.mNumParams;
  mParamNames = dc.mParamNames;
  mAllRefObjectNames = dc.mAllRefObjectNames;
  mCallbackFcn = dc.mCallbackFcn;
  mUserData = dc.mUserData;

  return *this;
}

//------------------------------------------------------------------------------
// bool AddParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
/**
 * Adds Parameter name to array if name doesn't already exist.
 */
//------------------------------------------------------------------------------
bool DataCallback::AddParameter(const std::string &paramName, Integer index)
{
  if (paramName != "" && index == mNumParams)
  {
    // if paramName not found, add it
    if (find(mParamNames.begin(), mParamNames.end(), paramName) ==
	mParamNames.end())
    {
      mParamNames.push_back(paramName);
      mNumParams = mParamNames.size();
      mParams.push_back(NULL);
      yParamWrappers.push_back(NULL);

      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
// bool SetCallback(void (*)(const double*, int))
//------------------------------------------------------------------------------
/**
 * Sets the callback function to which data is sent
 */
//------------------------------------------------------------------------------
void DataCallback::SetCallback(void (*CBFcn)(const double*, int, void*), void *userdata)
{
  mCallbackFcn = CBFcn;
  mUserData = userdata;
}

//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
bool DataCallback::Initialize()
{
  // Check if there are parameters selected for report
  if (active)
  {
    if (mNumParams == 0)
    {
      MessageInterface::ShowMessage
	("*** WARNING *** The DataCallback named \"%s\" will not be created.\n"
	 "No parameters were added to DataCallback.\n", GetName().c_str());

      active = false;
      return false;
    }

    if ((mNumParams > 0))
      if (mParams[0] == NULL)
      {
	MessageInterface::ShowMessage
	  ("*** WARNING *** The DataCallback named \"%s\" will not be created.\n"
	   "The first parameter:%s added for the report file is NULL\n",
	   GetName().c_str(), mParamNames[0].c_str());

	active = false;
	return false;
      }
  }

  Subscriber::Initialize();

  // if active and not initialized already, then initialize
  if (active && !isInitialized)
  {
    isInitialized = true;
  }

  return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the DataCallback.
 *
 * @return clone of the DataCallback.
 *
 */
//------------------------------------------------------------------------------
GmatBase* DataCallback::Clone() const
{
  return (new DataCallback(*this));
}

//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void DataCallback::Copy(const GmatBase* orig)
{
  operator=(*((DataCallback *)(orig)));
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string DataCallback::GetParameterText(const Integer id) const
{
  if (id >= SubscriberParamCount && id < DataCallbackParamCount)
    return PARAMETER_TEXT[id - SubscriberParamCount];
  else
    return Subscriber::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer DataCallback::GetParameterID(const std::string &str) const
{
  for (Integer i = SubscriberParamCount; i < DataCallbackParamCount; i++)
  {
    if (str == PARAMETER_TEXT[i - SubscriberParamCount])
      return i;
  }

  return Subscriber::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType DataCallback::GetParameterType(const Integer id) const
{
  if (id >= SubscriberParamCount && id < DataCallbackParamCount)
    return PARAMETER_TYPE[id - SubscriberParamCount];
  else
    return Subscriber::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string DataCallback::GetParameterTypeString(const Integer id) const
{
  if (id >= SubscriberParamCount && id < DataCallbackParamCount)
    return Subscriber::PARAM_TYPE_STRING[GetParameterType(id)];
  else
    return Subscriber::GetParameterTypeString(id);

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
bool DataCallback::IsParameterCommandModeSettable(const Integer id) const
{
  // Override this one from the base class
  if (id == SOLVER_ITERATIONS)
    return true;

  // Turn these off
  if (id == DATA_ELEMENTS)
    return false;

  // Turn on the rest that are DataCallback specific (FILENAME, PRECISION, ADD,
  // WRITE_HEADERS, LEFT_JUSTIFY, COL_WIDTH, and WRITE_REPORT)
  if (id >= SubscriberParamCount)
    return true;

  // And let the base class handle its own
  return Subscriber::IsParameterCommandModeSettable(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool DataCallback::SetStringParameter(const Integer id, const std::string &value)
{
  if (id == DATA_ELEMENTS)
  {
    return AddParameter(value, mNumParams);
  }

  return Subscriber::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool DataCallback::SetStringParameter(const std::string &label, const std::string &value)
{
  return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool DataCallback::SetStringParameter(const Integer id, const std::string &value, const Integer index)
{
  switch (id)
  {
    case DATA_ELEMENTS:
      return AddParameter(value, index);
    default:
      return Subscriber::SetStringParameter(id, value, index);
  }
}

//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool DataCallback::SetStringParameter(const std::string &label,
    const std::string &value,
    const Integer index)
{
  return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& DataCallback::GetStringArrayParameter(const Integer id) const
{
  switch (id)
  {
    case DATA_ELEMENTS:
      return mParamNames;
    default:
      return Subscriber::GetStringArrayParameter(id);
  }
}

//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& DataCallback::GetStringArrayParameter(const std::string &label) const
{
  return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
GmatBase* DataCallback::GetRefObject(const UnsignedInt type,
    const std::string &name)
{
  for (int i=0; i<mNumParams; i++)
  {
    if (mParamNames[i] == name)
      return mParams[i];
  }

  throw SubscriberException("DataCallback::GetRefObject() the object name: " + name + "not found\n");
}

//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
bool DataCallback::SetRefObject(GmatBase *obj, const UnsignedInt type,
    const std::string &name)
{
  if(obj == NULL)
  {
    return false;
  }

  if (type == Gmat::PARAMETER)
  {
    SetWrapperReference(obj, name);

    for (int i=0; i<mNumParams; i++)
    {
      // Handle array elements
      std::string realName = GmatStringUtil::GetArrayName(mParamNames[i]);

      if (realName == name)
	mParams[i] = (Parameter*)obj;
    }

    return true;
  }

  return Subscriber::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool DataCallback::HasRefObjectTypeArray()
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
const ObjectTypeArray& DataCallback::GetRefObjectTypeArray()
{
  refObjectTypes.clear();
  refObjectTypes.push_back(Gmat::PARAMETER);
  return refObjectTypes;
}

//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
const StringArray& DataCallback::GetRefObjectNameArray(const UnsignedInt type)
{
  mAllRefObjectNames.clear();

  switch (type)
  {
    case Gmat::UNKNOWN_OBJECT:
    case Gmat::PARAMETER:
      {
	// Handle array index
	for (int i=0; i<mNumParams; i++)
	{
	  std::string realName = GmatStringUtil::GetArrayName(mParamNames[i]);
	  mAllRefObjectNames.push_back(realName);
	}
      }
      break;            // Clears an Eclipse warning
    default:
      break;
  }

  return mAllRefObjectNames;
}

//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& DataCallback::GetWrapperObjectNameArray(bool completeSet)
{
  yWrapperObjectNames.clear();
  yWrapperObjectNames.insert(yWrapperObjectNames.begin(), mParamNames.begin(),
      mParamNames.end());

  return yWrapperObjectNames;
}

//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
/**
 * Distributes the data with size of len through the Publisher.
 */
//------------------------------------------------------------------------------
bool DataCallback::Distribute(const Real *dat, Integer len)
{
  if((mCallbackFcn != NULL) && (len > 0))
  {
    double *convertedData = new double[len];
    for(int i = 0; i < len; ++i)
    {
      // Need to convert input data to the right reference frame
      convertedData[i] = yParamWrappers[i]->EvaluateReal();
    }
    mCallbackFcn(convertedData, len, mUserData);
	delete convertedData;
  }

  return true;
}
