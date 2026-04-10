//$Id$
//------------------------------------------------------------------------------
//                                  FunctionManager
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
// number NNG06CCA54C
//
// Author: Wendy C. Shoan
// Created: 2008.03.19
//
/**
 * Definition for the FunctionManager class
 */
//------------------------------------------------------------------------------


#ifndef FunctionManager_hpp
#define FunctionManager_hpp


#include <map>              // for mapping between object names and types
#include <stack>            // for objectMap and caller stacks
#include <algorithm>        // for find()
#include "gmatdefs.hpp"

//#include "Function.hpp"
#include "ObjectManagedFunction.hpp"
//#include "UserDefinedFunction.hpp"
#include "StringUtil.hpp"
#include "TextParser.hpp"
#include "SolarSystem.hpp"
#include "PhysicalModel.hpp"
#include "Validator.hpp"
#include "ObjectInitializer.hpp"


/**
 * GMAT FunctionManager Class, used for managing Function calls from a
 * CallFunction command or from a FunctionRunner Math Node.
 *
 */
class GMAT_API FunctionManager
{
public:
   // class constructor
   FunctionManager();
   // class destructor
   virtual ~FunctionManager();
   // Copy constructor
   FunctionManager(const FunctionManager &fm);
   // Assignment operator
   FunctionManager&     operator=(const FunctionManager &fm);
      
   virtual void         SetPublisher(Publisher *pub);
   virtual Publisher*   GetPublisher();
   
   virtual void         SetObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetGlobalObjectMap(std::map<std::string, GmatBase *> *map);
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual void         SetFunctionName(const std::string &itsName);
   virtual std::string  GetFunctionName() const;
   virtual void         SetFunction(Function *theFunction);
   virtual Function*    GetFunction() const;
   virtual void         AddInput(const std::string &withName, Integer atIndex = -999);
   virtual void         AddOutput(const std::string &withName, Integer atIndex = -999);
   virtual void         SetInputs(const StringArray &inputs);
   virtual void         SetOutputs(const StringArray &outputs);
   virtual StringArray  GetOutputs();
   virtual void         SetInternalCoordinateSystem(CoordinateSystem *intCS);
   
   virtual bool         SetInputWrapper(Integer index, ElementWrapper *ew);
   virtual ElementWrapper* GetInputWrapper(Integer index);
   virtual bool         SetPassedInput(Integer index, GmatBase *obj, bool &inputAdded);
   virtual WrapperArray& GetWrappersToDelete();
   
   // Sequence methods
   bool                 PrepareObjectMap();
   virtual bool         Initialize();
   virtual bool         Execute(FunctionManager *callingFM = NULL);
   virtual Real         Evaluate(FunctionManager *callingFM = NULL);
   virtual Rmatrix      MatrixEvaluate(FunctionManager *callingFM = NULL);
   virtual GmatBase*    EvaluateObject(FunctionManager *callingFM = NULL);
   virtual void         Finalize();
   bool                 IsFinalized();
   
   //void                 SetCallingFunction(FunctionManager *fm);
   void                 PushToStack();
   bool                 PopFromStack(ObjectMap* cloned, const StringArray &outNames, 
                                     const StringArray &callingNames);

protected:
   
   /// Publisher for data generation
   Publisher            *publisher;
   /// Object store for the Function 
   ObjectMap            *functionObjectStore;
   /// Object store obtained from the caller
   ObjectMap            *localObjectStore;
   /// Object store obtained from the Sandbox
   ObjectMap            *globalObjectStore;
   /// Combined object store, used by Validator
   ObjectMap            combinedObjectStore;
   /// Object stores cloned when push to stack
   std::vector<ObjectMap *> clonedObjectStores;
   /// Solar System, set by the local Sandbox, to pass to the function
   SolarSystem          *solarSys;
   /// transient forces to pass to the function
   std::vector<PhysicalModel *> 
                        *forces;
   /// Name of the function this FunctionManager manages
   std::string          functionName;
   /// the function that this FunctionManager manages
   ////old Function             *currentFunction;
   ObjectManagedFunction    *currentFunction;
   /// the list of passing input strings for this call of the function
   StringArray          passedIns;
   /// the list of passing output strings for this call of the function
   StringArray          passedOuts;
   /// wrappers for the outut objects
   WrapperArray         outputWrappers;
   // wrapper map for the input objects
   WrapperMap           inputWrapperMap;
   /// flag indicating whether or not it's the first execution
   bool                 firstExecution;
   /// flag indicating whether or not FunctionManager is finalized
   bool                 isFinalized;
   /// flag indicating whether to reinitialize the current function
   bool                 reinitializeFunc;
   /// flag indicating whether this FM is the original owner of a functions
   bool                 isObjStoreOwner;
   // number of Variables created for the FOS
   Integer              numVarsCreated;
   /// Output Objects
   //ObjectArray          outObjects; 
   // Validator used to create the ElementWrappers
   Validator            *validator;
   // Created objects for string or numeric literal inputs
   ObjectMap            createdLiterals;
   // Created objects that need to be reevaluated each time (i.e. they are based on
   // current values of some object (e.g. an array element))
   ObjectMap            createdOthers;
   /// Real value output
   Real                 realResult;
   /// Rmatrix value output
   Rmatrix              matResult;
   /// Object output
   GmatBase             *objectResult;
   /// Flag indicating whether or not there is one nameless result (i.e. called from FunctionRunner)
   bool                 blankResult;
   /// Which type of output was saved last - real or rmatrix?
   std::string          outputType;
   /// Object needed to initialize the FOS objects
   ObjectInitializer    *objInit;
   /// the internal coordinate system
   CoordinateSystem     *internalCS;
   /// pointer to the function's function control sequence
   GmatCommand          *fcs;
   /// Call stack of FOSs for nested/recursive function calls
   ObjectMapStack       callStack;
   /// Call stack of LOSs for nested/recursive function calls
   ObjectMapStack       losStack;
   /// pointers to the calling function of this function (needs to be a stack 
   // because we need to be able to handle nested and recursive functions)
   std::stack<FunctionManager*> callers;
   // pointer to the current calling function
   FunctionManager      *callingFunction;
   void                 PrepareExecution(FunctionManager *callingFM = NULL);
   bool                 ValidateFunctionArguments();
   bool                 CreatePassingArgWrappers();
   void                 RefreshFOS();
   void                 RefreshFormalInputObjects();
   GmatBase*            FindObject(const std::string &name, bool arrayElementsAllowed = false);
   GmatBase*            CreateObject(const std::string &fromString);
   GmatBase*            CreateObjectForBuiltinGmatFunction(
                           const std::string &outName,
                           Gmat::WrapperDataType wType, 
                           Integer numRows, Integer numCols);
   void                 AssignResult();
   bool                 HandleCallStack();
   void                 SaveLastResult();
   
   void                 Cleanup();
   void                 DeleteObject(GmatBase *obj);
   void                 UnsubscribeSubscribers(ObjectMap *om);
   bool                 EmptyObjectMap(ObjectMap *om, const std::string &mapID = "");  
   bool                 DeleteObjectMap(ObjectMap *om, const std::string &mapID = "");
   bool                 ClearInOutWrappers();
   bool                 CloneObjectMap(ObjectMap *orig, ObjectMap *cloned);
   bool                 CopyObjectMap(ObjectMap *from, ObjectMap *to);
   bool                 IsOnStack(ObjectMap *om);
   
   // for debug
   void                 ShowObjectMap(ObjectMap *om, const std::string &mapID = "");
   void                 ShowStackContents(ObjectMapStack omStack, const std::string &stackID = "");
   void                 ShowCallers(const std::string &label = "");
   void                 ShowTrace(Integer count, Integer t1, const std::string &label = "",
                                  bool showMemoryTracks = false, bool addEol = false);
};

#endif // FunctionManager_hpp
