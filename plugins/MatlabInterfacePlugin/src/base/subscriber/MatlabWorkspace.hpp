//$Id: MatlabWorkspace.hpp 9460 2011-04-21 22:03:28Z lindajun $
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
// Author: Linda Jun
// Created: 2004/09/08
//
/**
 * Declares MatlabWorkspace class which evaluates parameters to be sent to MATLAB
 * workspace.
 */
//------------------------------------------------------------------------------
#ifndef MatlabWorkspace_hpp
#define MatlabWorkspace_hpp

#include "matlabinterface_defs.hpp"
#include "Subscriber.hpp"

class Parameter;
class MatlabInterface;

class MATLAB_API MatlabWorkspace : public Subscriber
{
public:
   MatlabWorkspace(const std::string &name, Parameter *firstParam = NULL);
   virtual ~MatlabWorkspace(void);
   MatlabWorkspace(const MatlabWorkspace &copy);
   MatlabWorkspace& operator=(const MatlabWorkspace &right);
   
   Integer              GetNumParameters();
   bool                 AddParameter(const std::string &paramName, Integer index);
   
   // methods inherited from Subscriber
   virtual bool         Initialize();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone(void) const;
   
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id, const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual bool         SetStringParameter(const Integer id, const std::string &value,
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
   
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   
protected:
   Integer mUpdateFrequency;
   Integer mNumParams;
   Integer mDataCount;
   Integer mSendCount;
   
   std::vector<Parameter*> mParams;
   StringArray mParamNames;
   StringArray mAllRefObjectNames;
   
   MatlabInterface *matlabIf;
   
   virtual bool         Distribute(Integer len);
   virtual bool         Distribute(const Real * dat, Integer len);
   
private:
   void ClearParameters();
   
   enum
   {
      ADD = SubscriberParamCount,
      UPDATE_FREQUENCY,
      MatlabWorkspaceParamCount  /// Count of the parameters for this class
   };
   
   static const std::string
      PARAMETER_TEXT[MatlabWorkspaceParamCount - SubscriberParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[MatlabWorkspaceParamCount - SubscriberParamCount];
   
};
#endif

