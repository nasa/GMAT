//$Id$
//------------------------------------------------------------------------------
//                           RealRHS
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: May 31, 2018
/**
 * Class used to manage equations on the right side of expressions
 */
//------------------------------------------------------------------------------

#ifndef RealRHS_hpp
#define RealRHS_hpp

#include "gmatdefs.hpp"
#include "ElementWrapper.hpp"

class MathTree;
class FunctionManager;

/**
 * Equation manager for right hand side strings describing equations
 */
class GMAT_API RHSEquation
{
public:
   RHSEquation();
   virtual ~RHSEquation();
   RHSEquation(const RHSEquation &rhs);
   RHSEquation& operator=(const RHSEquation &rhs);

   std::string GetEquationString();

   bool Initialize(ObjectMap *objectMap, ObjectMap *globalObjectMap);

   virtual bool BuildExpression(const std::string &rhside, ObjectMap *theMap,
         bool allowSingleElement = false);
   MathTree* GetMathTree(bool releaseOwnership = true);
   ElementWrapper* RunMathTree(ElementWrapper *lhsWrapper);
   bool Validate(std::string &msg);
   bool ValidateStringExpression(std::string &msg);
   WrapperMap GetMathWrapperMap();
   void SetCallingFunction(FunctionManager *fm);
   void SetMathWrappers();
   bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName);
   void SetWrapperReferences(ElementWrapper &ew, ObjectMap *objectMap,
         ObjectMap *globalObjectMap);
   void ClearWrappers();
   void Finalize();

   bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
         const std::string &newName);
   StringArray GetWrapperObjectNames();
   StringArray GetFunctionNames();

protected:
   /// String that defines the equation
   std::string rhsString;
   /// Map of resources used by the RHS expression
   ObjectMap *resourceMap;
   /// ElementWrapper pointer for the rhs of the equals sign
   ElementWrapper* rhsWrapper;
   /// MathNode pointer for RHS equation
   MathTree *mathTree;
   /// Wrapper name and ElementWrapper pointer Map for RHS math element
   WrapperMap mathWrapperMap;

};

#endif /* RealRHS_hpp */
