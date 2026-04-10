//$Id$
//------------------------------------------------------------------------------
//                                ObjectManagedFunction
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.10.20
//
/**
 * Defines the ObjectManagedFunction base class for BuiltinGmatFunction and
 * UserDefinedFunction
 */
//------------------------------------------------------------------------------
#ifndef ObjectManagedFunction_hpp
#define ObjectManagedFunction_hpp

#include "Function.hpp"
#include "ElementWrapper.hpp"
#include "ObjectInitializer.hpp"
#include "Validator.hpp"
#include <map>

/**
 * All function classes using function object store are derived from this base class.
 */
class GMAT_API ObjectManagedFunction : public Function
{
public:
   ObjectManagedFunction(const std::string &typeStr, const std::string &nomme);
   virtual ~ObjectManagedFunction();
   ObjectManagedFunction(const ObjectManagedFunction &f);
   ObjectManagedFunction& operator=(const ObjectManagedFunction &f);
   
   virtual bool         Initialize(ObjectInitializer *objInit,
                                   bool reinitialize = false,
                                   bool resetLocalObjects = true);
   virtual bool         Execute(ObjectInitializer *objInit, bool reinitialize = false);
   virtual void         Finalize(bool cleanUp = false);
   
   virtual void         SetFunctionObjectMap(ObjectMap *objMap);
   virtual ObjectMap*   GetFunctionObjectStore();
   virtual void         SetGlobalObjectMap(ObjectMap *objMap);
   
   virtual bool         SetInputElementWrapper(const std::string &forName,
                                               ElementWrapper *wrapper);
   virtual ElementWrapper* GetOutputArgument(Integer argNumber);
   virtual ElementWrapper* GetOutputArgument(const std::string &byName);
   virtual WrapperArray&   GetWrappersToDelete();
   virtual void            ClearInOutArgMaps(bool deleteInputs, bool deleteOutputs);

   virtual void            SetChangingInputType(bool hasChangingInput);
   virtual bool            HasChangingInputType();
   
   // Inherited methods from GmatBase
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   
   virtual bool         SetStringParameter(const Integer id, 
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const char *value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   
   /// Function input name and element wrapper map
   WrapperMap           inputArgMap;
   /// Function output name element wrapper map
   WrapperMap           outputArgMap;
   /// Object store for the Function 
   ObjectMap            *objectStore;
   /// Object store obtained from the Sandbox
   ObjectMap            *globalObjectStore;
   /// Old wrappers to delete
   WrapperArray         wrappersToDelete;
   /// Whether object store has changing input types
   bool                 changingInputType;
  
   // for debug
   void ShowObjectMap(ObjectMap *objMap, const std::string &title = "",
                      const std::string &mapName = "");
};


#endif // ObjectManagedFunction_hpp
