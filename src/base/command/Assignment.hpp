//$Id$
//------------------------------------------------------------------------------
//                                Assignment
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
// Author: Darrel J. Conway
// Created: 2004/02/03
//
/**
 * Definition of the Assignment command class.
 * 
 * The assignment class is the Command class that handles commands of the form
 * 
 *     GMAT object.parameter = value;
 *     GMAT variable = parameter;
 *     GMAT variable = equation;
 */
//------------------------------------------------------------------------------

#ifndef Assignment_hpp
#define Assignment_hpp

#include "gmatdefs.hpp"
#include "GmatCommand.hpp"
#include "Function.hpp"
#include "RHSEquation.hpp"


/**
 * Sets one property or object equal to a computed quantity or other object
 */
class GMAT_API Assignment : public GmatCommand
{
public:
   Assignment();
   virtual ~Assignment();
   Assignment(const Assignment& a);
   Assignment&          operator=(const Assignment& a);
   
   MathTree*            GetMathTree();
   virtual bool         HasAFunction();
   const StringArray&   GetGmatFunctionNames();
   void                 SetMathWrappers();

   void                 SetLHS(const std::string &left) { lhs = left; }
   void                 SetRHS(const std::string &right) {rhs = right; }
   std::string          GetLHS() { return lhs; }
   std::string          GetRHS() { return rhs; }
   
   virtual void         SetFunction(Function *function);
   virtual std::vector<Function*> GetFunctions() const;
   
   // inherited from GmatCommand
   virtual void         SetPublisher(Publisher *pub);
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual void         SetObjectMap(ObjectMap *map);
   virtual void         SetGlobalObjectMap(ObjectMap *map);
   
   virtual bool         InterpretAction();
   virtual const StringArray& GetObjectList();
   virtual bool         Validate();
   virtual bool         Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();
   virtual bool         SkipInterrupt();
   virtual void         SetCallingFunction(FunctionManager *fm);
   
   virtual const StringArray& 
                        GetWrapperObjectNameArray(bool completeSet = false);
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                                          const std::string &withName);
   virtual void         ClearWrappers();
   
   // inherited from GmatBase
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual GmatBase*    Clone() const;
   
   virtual const std::string&
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");

   virtual bool         AffectsClones();
   virtual GmatBase*    GetUpdatedObject();
   virtual Integer      GetUpdatedObjectParameterIndex();

   DEFAULT_TO_NO_CLONES

protected:
   
   /// string on the left side of the equals sign
   std::string          lhs;
   /// Error message(s) generated if the lhs object is not command mode settable
   std::string          settabilityError;
   /// Flag used to decide if lhs goes in the object list - omit if not settable
   bool                 omitLHSBecauseOfSettability;
   /// string on the right side of the equals sign
   std::string          rhs;
   /// ElementWrapper pointer for the lhs of the equals sign
   ElementWrapper*      lhsWrapper;
   /// ElementWrapper pointer for the rhs of the equals sign
   ElementWrapper*      rhsWrapper;
   /// Container for RHS equation
   RHSEquation          *rhsEquation;
   /// Wrapper name and ElementWrapper pointer Map for RHS math element
   WrapperMap           mathWrapperMap;

   /// Object pointer for clone management
   GmatBase             *lhsOwner;
   /// Object parameter ID if lhs is an attribute
   Integer              lhsOwnerID;
   
   // methods
   bool ValidateArrayElement(ElementWrapper *lhsWrapper, ElementWrapper *rhsWrapper);
   bool ValidateRmatrix(ElementWrapper *lhsWrapper, ElementWrapper *rhsWrapper);
   bool ValidateRvector(ElementWrapper *lhsWrapper, ElementWrapper *rhsWrapper);
   
   void ClearMathTree();
   // ElementWrapper* RunMathTree();
   void HandleObjectPropertyChange(ElementWrapper *lhsWrapper);

   void PassToClones();
   void MatchAttribute(Integer id, GmatBase *owner, GmatBase *receiver);

   bool ParseRHS(std::string &rhs);
};

#endif // Assignment_hpp
