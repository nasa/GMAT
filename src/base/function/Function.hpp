//$Id$
//------------------------------------------------------------------------------
//                                   Function
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
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Defines the Function base class used for Matlab and Gmat functions.
 */
//------------------------------------------------------------------------------
#ifndef Function_hpp
#define Function_hpp

#include "GmatBase.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "PhysicalModel.hpp"
#include "ObjectInitializer.hpp"

/**
 * All function classes are derived from this base class.
 */
class GMAT_API Function : public GmatBase
{
public:
   Function(const std::string &typeStr, const std::string &nomme);
   virtual ~Function();
   Function(const Function &f);
   Function&            operator=(const Function &f);
   
   virtual void         SetCallDescription(const std::string &desc);
   
   virtual WrapperTypeArray
                        GetOutputTypes(IntegerArray &rowCounts,
                                       IntegerArray &colCounts) const;
   virtual void         SetOutputTypes(WrapperTypeArray &outputTypes,
                                       IntegerArray &rowCounts,
                                       IntegerArray &colCounts);
   
   virtual void         SetErrorFound(bool errFlag);
   virtual bool         ErrorFound();
   
   virtual std::string  GetFunctionPathAndName();
   
   virtual bool         IsNewFunction();
   virtual void         SetNewFunction(bool flag);
   
   virtual bool         Initialize(ObjectInitializer *objInit, bool reinitialize = false);
   virtual bool         Execute(ObjectInitializer *objInit, bool reinitialize = false);
   virtual void         Finalize(bool cleanUp = false);
   
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const char *value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   
   virtual void         SetOutputName(const std::string &outputName);

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Call description to be written out in error message
   std::string          callDescription;
   /// Fully-qualified path for function script
   std::string          functionPath;
   /// Function name
   std::string          functionName;
   /// Function input names
   StringArray          inputNames;
   /// Function output names
   StringArray          outputNames;
   /// Output wrapper type array
   WrapperTypeArray     outputWrapperTypes;
   
   /// Output row count used for returning one Array type
   IntegerArray         outputRowCounts;
   /// Output column count used for returning one Array type
   IntegerArray         outputColCounts;
   
   /// Solar System, set by the local Sandbox, to pass to the function
   SolarSystem          *solarSys;
   /// Internal CS, set by the local Sandbox, to pass to the function
   CoordinateSystem     *internalCoordSys;
   /// transient forces to pass to the function
   std::vector<PhysicalModel *> *forces;
   
   /// the flag indicating error found in function, this flag is set by Interpreter
   bool                 errorFound;
   
   enum
   {
      FUNCTION_PATH = GmatBaseParamCount,
      FUNCTION_NAME,
      FUNCTION_INPUT,
      FUNCTION_OUTPUT,
      FunctionParamCount  ///< Count of the parameters for this class
   };
   
   static const std::string
      PARAMETER_TEXT[FunctionParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[FunctionParamCount - GmatBaseParamCount];
   
};


#endif // Function_hpp
