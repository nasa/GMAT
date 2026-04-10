//$Id: DataCallback.hpp 9460 2011-04-21 22:03:28Z rmathur $
//------------------------------------------------------------------------------
//                                  DataCallback
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
// Created: December 15, 2014
//
/**
 * Declares DataCallback class which sends data to a custom callback function.
 */
//------------------------------------------------------------------------------

#ifndef DataCallback_hpp
#define DataCallback_hpp

#include "datacallback_defs.hpp"
#include "Subscriber.hpp"

class Parameter;

class DATACALLBACK_API DataCallback : public Subscriber
{
  public:
    DataCallback(const std::string &typeName, const std::string &name,
	Parameter *firstParam = NULL);

    virtual ~DataCallback(void);

    DataCallback(const DataCallback &);
    DataCallback& operator=(const DataCallback&);

    // methods for this class
    bool AddParameter(const std::string &paramName, Integer index);
    void SetCallback(void (*CBFcn)(const double*, int, void*), void *userdata);

    // methods inhereted from GmatBase
    virtual bool Initialize();
    virtual GmatBase* Clone(void) const;
    virtual void Copy(const GmatBase* orig);

    virtual std::string  GetParameterText(const Integer id) const;
    virtual Integer      GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string  GetParameterTypeString(const Integer id) const;

    virtual bool         IsParameterCommandModeSettable(const Integer id) const;

    virtual bool         SetStringParameter(const Integer id,
	const std::string &value);
    virtual bool         SetStringParameter(const std::string &label,
	const std::string &value);
    virtual bool         SetStringParameter(const Integer id,
	const std::string &value,
	const Integer index);
    virtual bool         SetStringParameter(const std::string &label,
	const std::string &value,
	const Integer index);
    virtual const StringArray&
      GetStringArrayParameter(const Integer id) const;
    virtual const StringArray&
      GetStringArrayParameter(const std::string &label) const;

    virtual GmatBase*    GetRefObject(const UnsignedInt type,
	const std::string &name);
    virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
	const std::string &name = "");

    virtual bool         HasRefObjectTypeArray();
    virtual const ObjectTypeArray&
      GetRefObjectTypeArray();
    virtual const StringArray&
      GetRefObjectNameArray(const UnsignedInt type);

    // methods for setting up the items to subscribe
    virtual const StringArray&
      GetWrapperObjectNameArray(bool completeSet = false);

  protected:
    std::vector<Parameter*> mParams;
    Integer mNumParams;
    StringArray mParamNames;
    StringArray mAllRefObjectNames;

    // methods inhereted from Subscriber
    virtual bool Distribute(const Real *dat, Integer len);

    void (*mCallbackFcn)(const double *dat, int len, void *userdata);
    void *mUserData; // User data sent to callback function

    enum
    {
      DATA_ELEMENTS = SubscriberParamCount,
      DataCallbackParamCount /// Count of the parameters for this class
    };

  private:

    static const std::string
      PARAMETER_TEXT[DataCallbackParamCount - SubscriberParamCount];
    static const Gmat::ParameterType
      PARAMETER_TYPE[DataCallbackParamCount - SubscriberParamCount];
};

#endif
