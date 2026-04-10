//$Id$
//------------------------------------------------------------------------------
//                                   MathTree
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
// Created: 2006/04/17
//
/**
 * Defines the MathTree base class used for Math in Scripts.
 */
//------------------------------------------------------------------------------
#ifndef MATHTREE_HPP_
#define MATHTREE_HPP_

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include <map>

// Forward references for GMAT core objects
class MathNode;
class ElementWrapper;
class Function;
class FunctionManager;
class SolarSystem;
class PhysicalModel;
class CoordinateSystem;
class Publisher;

class GMAT_API MathTree : public GmatBase
{
public:
   
   MathTree(const std::string &typeStr, const std::string &nomme);
   virtual ~MathTree();
   MathTree(const MathTree& mt);
   MathTree& operator=(const MathTree& mt);
   
   const StringArray&   GetGmatFunctionNames();
   void                 SetGmatFunctionNames(StringArray funcList);
   
   std::vector<Function*> GetFunctions() const;
   void                 SetFunction(Function *function);
   
   virtual void         SetCallingFunction(FunctionManager *fm);
   
   MathNode*            GetTopNode();
   void                 SetTopNode(MathNode *node);
   void                 SetMathWrappers(WrapperMap *wrapperMap);
   
   bool                 Validate(std::string &msg);
   Real                 Evaluate();
   Rmatrix              MatrixEvaluate();
   bool                 Initialize(ObjectMap *objectMap,
                                   ObjectMap *globalObjectMap);
   void                 Finalize();
   void                 GetOutputInfo(Integer &type, Integer &rowCount,
                                      Integer &colCount);
   
   // for setting objects to FunctionRunner
   void                 SetObjectMap(ObjectMap *map);
   void                 SetGlobalObjectMap(ObjectMap *map);
   void                 SetSolarSystem(SolarSystem *ss);
   void                 SetInternalCoordSystem(CoordinateSystem *cs);
   void                 SetTransientForces(std::vector<PhysicalModel*> *tf);
   void                 SetPublisher(Publisher *pub);
   
   // Inherited (GmatBase) methods
   virtual GmatBase*    Clone() const;
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual const StringArray& 
                        GetWrapperObjectNameArray(bool completeSet = false);
   
   DEFAULT_TO_NO_CLONES

protected:
   
   /// Top node of the math tree
   MathNode   *theTopNode;
   
   /// Object store obtained from the Sandbox
   ObjectMap  *theObjectMap;
   /// Global object store obtained from the Sandbox
   ObjectMap  *theGlobalObjectMap;
   /// Wrapper name and ElementWrapper pointer Map for RHS math element
   WrapperMap *theWrapperMap;
   
   /// All parameter name array
   StringArray theParamArray;
   StringArray theAllParamArray;
   StringArray theGmatFunctionNames;
   StringArray theWrapperNames;
   
   std::vector<Function*> theFunctions;
   std::vector<MathNode*> nodesToDelete;
   
   bool InitializeParameter(MathNode *node);
   void FinalizeFunctionRunner(MathNode *node);
   void SetMathElementWrappers(MathNode *node);
   void SetFunctionToRunner(MathNode *node, Function *function);
   void SetCallingFunctionToRunner(MathNode *node, FunctionManager *fm);
   void SetObjectMapToRunner(MathNode *node, ObjectMap *map);
   void SetGlobalObjectMapToRunner(MathNode *node, ObjectMap *map);
   void SetSolarSystemToRunner(MathNode *node, SolarSystem *ss);
   void SetInternalCoordSystemToRunner(MathNode *node, CoordinateSystem *cs);
   void SetTransientForcesToRunner(MathNode *node, std::vector<PhysicalModel*> *tf);
   void SetPublisherToRunner(MathNode *node, Publisher *pub);
   bool RenameParameter(MathNode *node, const UnsignedInt type,
                        const std::string &oldName, const std::string &newName);
   void CreateParameterNameArray(MathNode *node);
   void DeleteNode(MathNode *node);
   
};

#endif /*MATHTREE_HPP_*/
