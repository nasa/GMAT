//------------------------------------------------------------------------------
//                           EquationWrapper
//------------------------------------------------------------------------------
// GMAT:  General Mission Analysis Tool
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jun 14, 2018
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef EquationWrapper_hpp
#define EquationWrapper_hpp

#include "gmatdefs.hpp"
#include "ElementWrapper.hpp"
#include "RHSEquation.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"

class EquationWrapper: public ElementWrapper
{
public:
   EquationWrapper(const std::string &eq = "");
   virtual ~EquationWrapper();
   EquationWrapper(const EquationWrapper& ew);
   EquationWrapper& operator=(const EquationWrapper& ew);

   virtual ElementWrapper* Clone() const;

   void SetEquation(const std::string &eq);
   void SetConfiguredObjectMap(ObjectMap *map);

   virtual void            ClearRefObjectNames();
   virtual const StringArray& GetRefObjectNames();
   virtual bool            SetRefObjectName(const std::string &name,
                                            Integer index = 0);
   virtual GmatBase*       GetRefObject(const std::string &name = "");
   virtual bool            SetRefObject(GmatBase *obj);
   virtual bool            RenameObject(const std::string &oldName,
                                           const std::string &newName);

   bool                    ConstructTree();
   RHSEquation             *GetTree();
   bool                    Initialize(ObjectMap *objectMap,
                                      ObjectMap *globalObjectMap);
   bool                    Initialize();

   virtual Gmat::ParameterType GetDataType() const;
   bool                    EvaluateEquation();

   // Evaluation methods.  Equations can return Reals, Rvectors, Arrays, or strings
   virtual Real            EvaluateReal() const;
   virtual bool            SetReal(const Real toValue);
   virtual const Rmatrix&  EvaluateArray() const;
   virtual bool            SetArray(const Rmatrix &toValue);
   virtual const Rvector&  EvaluateRvector() const;
   virtual bool            SetRvector(const Rvector &toValue);

   // Disable string returns for now
//   virtual bool            SetString(const std::string &toValue);
//   virtual std::string     EvaluateString() const;

protected:
   /// String expression for the equation
   std::string             theEquation;
   /// Return type for the equation
   Gmat::ParameterType     dataType;
   /// Equation manager
   RHSEquation             theTree;
   /// Return container for an Rvector
   Rvector                 retVec;
   /// Return container for an Rmatrix
   Rmatrix                 retMat;
   /// Output from running the math tree
   ElementWrapper          *resultant;

   /// Objects referenced in the equation
   StringArray             refObjects;
   /// The mapping for objects
   ObjectMap               *configObjectMap;
   /// Mapping of objects set on the wrapper
   ObjectMap               equationObjectMap;

   /// Flag used to warn if the equation is not yet evaluated
   bool                    hasEvaluated;

   // Disable string returns for now
//   /// String return container
//   std::string             retStr;

   virtual void            SetupWrapper();
};

#endif /* EquationWrapper_hpp */
