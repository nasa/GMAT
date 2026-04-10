//$Id$
//------------------------------------------------------------------------------
//                                 CallFunction
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the CallFunction command class
 */
//------------------------------------------------------------------------------
#ifndef CallFunction_hpp
#define CallFunction_hpp

#include "GmatCommand.hpp"
#include "Function.hpp"
#include "FunctionManager.hpp"
#include "Parameter.hpp"
#include "Array.hpp"
#include "StringVar.hpp"

// Forward references for GMAT core objects
class Publisher;

class GMAT_API CallFunction : public GmatCommand
{
public:
   CallFunction(const std::string &type);
   virtual ~CallFunction();
   
   CallFunction(const CallFunction& cf);
   CallFunction&        operator=(const CallFunction& cf);
   
   std::string          FormEvalString();
   bool                 AddInputParameter(const std::string &paramName, Integer index);
   bool                 AddOutputParameter(const std::string &paramName, Integer index);

   /// Override this method so that references can be checked
   /// for Remove/Delete of objects
   virtual bool         HasOtherReferenceToObject(const std::string &withName);

   // override GmatCommand methods
   virtual bool         Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   
   // override these to set on FunctionManager (and find function object in GOS)
   virtual void         SetPublisher(Publisher *pub);
   virtual void         SetObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetGlobalObjectMap(std::map<std::string, GmatBase *> *map);
   virtual bool         HasAFunction();
   virtual bool         IsMatlabFunctionCall();
   
   // override GmatBase methods
   virtual GmatBase*    Clone() const;
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id, const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const char *value);
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

   // Current assumption: Owned clones in functions are handled separately
   DEFAULT_TO_NO_CLONES

protected:

   ObjectArray objectArray;
   std::vector<Parameter*> mInputList;
   std::vector<Parameter*> mOutputList;
   GmatCommand *callcmds;
   
   StringArray mInputNames;
   StringArray mOutputNames;
   
   Integer mNumInputParams;
   Integer mNumOutputParams;
   
   Function *mFunction;
   std::string mFunctionName;
   std::string mFunctionPathAndName;
   
   /// the manager for the Function
   FunctionManager fm;
   
   bool isGmatFunction;
   bool isMatlabFunction;
   bool isBuiltinGmatFunction;
   
   void ClearInputParameters();
   void ClearOutputParameters();
   
   enum
   {
      FUNCTION_NAME = GmatCommandParamCount,
      ADD_INPUT,
      ADD_OUTPUT,
      COMMAND_STREAM,
      CallFunctionParamCount  ///< Count of the parameters for this class
   };
   
   
   static const std::string
      PARAMETER_TEXT[CallFunctionParamCount - GmatCommandParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[CallFunctionParamCount - GmatCommandParamCount];
};
#endif // CallFunction_hpp

