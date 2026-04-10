//$Id$
//------------------------------------------------------------------------------
//                            BeginFunction
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// order 124.
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/08/30
//
/**
 * Definition code for the BeginFunction command, a wrapper that manages the
 * commands in a GMAT function.
 */
//------------------------------------------------------------------------------

#ifndef BeginFunction_hpp
#define BeginFunction_hpp



#include "GmatCommand.hpp"
#include "Function.hpp"



class GMAT_API BeginFunction : public GmatCommand
{
public:
   BeginFunction();
   virtual ~BeginFunction();
   BeginFunction(const BeginFunction& bf);
   BeginFunction&       operator=(const BeginFunction& bf);


   virtual GmatBase*    Clone() const;
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                    const std::string &name = "");
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);


   // Access methods inherited from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual bool        IsParameterReadOnly(const Integer id) const;
   virtual bool        IsParameterReadOnly(const std::string &label) const;

   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const Integer id,
                                          const Integer index) const;
   virtual const StringArray&
                       GetStringArrayParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value,
                                          const Integer index);
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual std::string GetStringParameter(const std::string &label,
                                          const Integer index) const;
   virtual const StringArray&
                       GetStringArrayParameter(const std::string &label) const;
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value);
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value,
                                          const Integer index);

   virtual bool        TakeAction(const std::string &action,
                                  const std::string &actionData = "");


   virtual bool         Initialize();
   virtual bool         Execute();
   void                 ClearReturnObjects();
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   void                 InitializeInternalObjects();
   void                 BuildReferences(GmatBase *obj);
   void                 SetRefFromName(GmatBase *obj, const std::string &oName);
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   void                 ClearInputMap();

   DEFAULT_TO_NO_CLONES

protected:
   /// Name of the function
   std::string          functionName;
   /// The actual GMAT function
   Function             *gfun;
   /// List of function parameters defined in the function file
   StringArray          inputs;
   /// List of output elements defined in the function file
   StringArray          outputs;
   /// Names of the function parameters used in the CallFunction
   StringArray          inputObjects;
   /// Names of output elements expected by the CallFunction
   StringArray          outputObjects;
   /// Mapping of parameter names to local vars and clones of the input objects
   std::map <std::string, GmatBase *>
                        localMap;
   /// Vector of the return objects
   ObjectArray          returnObjects;
   /// Transient force container, in case finite burns are active
   std::vector<PhysicalModel*>
                        *transientForces;
   /// CoordinateSystem used internally
   CoordinateSystem     *internalCoordSys;


   SpacePoint*          FindSpacePoint(const std::string &spName);
   std::string          trimIt( std::string s );


   /// Published parameters for functions
   enum
   {
      FUNCTION_NAME = GmatCommandParamCount,
      INPUTS,
      OUTPUTS,
      INPUT_OBJECT_NAMES,
      OUTPUT_OBJECT_NAMES,
      BeginFunctionParamCount
   };

   /// burn parameter labels
   static const std::string
      PARAMETER_TEXT[BeginFunctionParamCount - GmatCommandParamCount];
   /// burn parameter types
   static const Gmat::ParameterType
      PARAMETER_TYPE[BeginFunctionParamCount - GmatCommandParamCount];

};



#endif /* BeginFunction_hpp */
