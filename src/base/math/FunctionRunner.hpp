//$Id$
//------------------------------------------------------------------------------
//                              FunctionRunner
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
// number NNG06CCA54C
//
// Author: Wendy Shoan
// Created: 2008.04.21
//
/**
 * Declares FunctionRunner class.
 */
//------------------------------------------------------------------------------
#ifndef FunctionRunner_hpp
#define FunctionRunner_hpp

#include "MathFunction.hpp"
#include "FunctionManager.hpp"
#include "SolarSystem.hpp"
#include "PhysicalModel.hpp"
#include "RealUtilities.hpp"

typedef std::vector<MathNode*> MathNodeArray;

class GMAT_API FunctionRunner : public MathFunction
{
public:
   FunctionRunner(const std::string &nomme);
   virtual ~FunctionRunner();
   FunctionRunner(const FunctionRunner &copy);

   // for Function
   void                 SetFunctionName(const std::string &fname);
   void                 SetFunction(Function *function);
   
   // for Function input
   void                 AddFunctionInput(const std::string &name);
   void                 SetFunctionInputs();
   const StringArray&   GetInputs();
   void                 AddInputNode(MathNode *node);
   
   // for Function output
   void                 AddFunctionOutput(const std::string &name);
   void                 SetFunctionOutputs();
   
   // for calling function
   virtual void         SetCallingFunction(FunctionManager *fm);
   
   // for setting objects to FunctionManager
   virtual void         SetObjectMap(ObjectMap *map);
   virtual void         SetGlobalObjectMap(ObjectMap *map);
   void                 SetSolarSystem(SolarSystem *ss);
   void                 SetInternalCoordSystem(CoordinateSystem *cs);
   void                 SetTransientForces(std::vector<PhysicalModel*> *tf);
   void                 SetPublisher(Publisher *pub);
   
   // inherited from MathFunction
   virtual void         GetOutputInfo(Integer &type, Integer &rowCount, 
                                      Integer &colCount);
   virtual bool         ValidateInputs(); 
   virtual Real         Evaluate();
   virtual Rmatrix      MatrixEvaluate();
   virtual GmatBase*    EvaluateObject();
   virtual void         Finalize();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   
protected:
   
   FunctionManager theFunctionManager;
   ObjectMap       *theObjectMap;
   ObjectMap       *theGlobalObjectMap;
   std::string     theFunctionName;
   Function        *theFunction;
   StringArray     theInputNames;
   StringArray     theOutputNames;
   MathNodeArray   theInputNodes;
   FunctionManager *callingFunction;
   
   CoordinateSystem *internalCS;
   
   GmatBase* FindObject(const std::string &name);
   void      HandlePassingMathExp(Function *function);
};

#endif // FunctionRunner_hpp
