//------------------------------------------------------------------------------
//                              UserPathFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Wendy Shoan / NASA
// Created: 2015.07.16
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef UserPathFunction_hpp
#define UserPathFunction_hpp

#include "csaltdefs.hpp"
#include "UserFunction.hpp"
#include "FunctionInputData.hpp"
#include "PathFunctionContainer.hpp"
#include "BoundData.hpp"

class CSALT_API UserPathFunction : public UserFunction
{
public:
  
   UserPathFunction();
   UserPathFunction(const UserPathFunction &copy);
   UserPathFunction& operator=(const UserPathFunction &copy);
   virtual ~UserPathFunction();
   
   virtual void           Initialize(FunctionInputData     *pd,
                                     PathFunctionContainer *pfc);
   virtual PathFunctionContainer*
                          EvaluateUserFunction(FunctionInputData *pd,
                                               PathFunctionContainer *pfc);
   virtual PathFunctionContainer*
                          EvaluateUserJacobian(FunctionInputData *pd,
                                               PathFunctionContainer *pfc);
   virtual void           SetParamData(FunctionInputData *pd);
   virtual void           SetFunctionData(PathFunctionContainer *pfc);
   virtual const Rvector& GetStateVector();
   virtual const Rvector& GetControlVector();
   virtual const Rvector& GetStaticVector(); // YK mod static params
   virtual Real           GetTime();
   virtual Integer        GetPhaseNumber();

   virtual std::vector<std::vector<Rmatrix>>
                          EvaluateJacobianPattern();
   
   DEPRECATED(virtual void SetAlgFunctions(const Rvector& toValues));
   DEPRECATED(virtual void           SetAlgFunctionsUpperBounds(const Rvector& toValues));
   DEPRECATED(virtual void           SetAlgFunctionsLowerBounds(const Rvector& toValues));
   DEPRECATED(virtual void           SetAlgFunctionNames(const StringArray& toNames));
   DEPRECATED(virtual void SetAlgStateJacobian(const Rmatrix& toJacobian));
   DEPRECATED(virtual void SetAlgControlJacobian(const Rmatrix& toJacobian));
   DEPRECATED(virtual void SetAlgTimeJacobian(const Rmatrix& toJacobian));
   
   DEPRECATED(virtual void SetDynFunctions(const Rvector& toValues));
   DEPRECATED(virtual void SetDynStateJacobian(const Rmatrix& toJacobian));
   DEPRECATED(virtual void SetDynControlJacobian(const Rmatrix& toJacobian));
   DEPRECATED(virtual void SetDynTimeJacobian(const Rmatrix& toJacobian));
   
   DEPRECATED(virtual void SetCostFunction(const Rvector& toValues));
   DEPRECATED(virtual void SetCostStateJacobian(const Rmatrix& toJacobian));
   DEPRECATED(virtual void SetCostControlJacobian(const Rmatrix& toJacobian));
   DEPRECATED(virtual void SetCostTimeJacobian(const Rmatrix& toJacobian));

   virtual void           SetFunctions(FunctionType type, const Rvector& toValues);
   virtual void           SetJacobian(FunctionType funcType, JacobianType jacType,
                                      const Rmatrix& toJacobian);
   virtual void           SetFunctionNames(FunctionType type, const StringArray& toNames);
   virtual void           SetFunctionBounds(FunctionType type, FunctionBound bound, const Rvector& toValues);
   //virtual void           SetJacobian(FunctionType funcType, JacobianType jacType, const Rmatrix& toJacobian);
   
   DEPRECATED(virtual Rvector        GetAlgFunctions());
   DEPRECATED(virtual Rvector        GetAlgFunctionsUpperBounds());
   DEPRECATED(virtual Rvector        GetAlgFunctionsLowerBounds());
   DEPRECATED(virtual const StringArray&
                          GetAlgFunctionNames());

   virtual Rvector        GetFunction(FunctionType type);
   virtual Rvector        GetFunctionBounds(FunctionType type, FunctionBound bound);
   virtual const StringArray&
                          GetFunctionNames(FunctionType type);
   
   virtual Integer        GetNumControlVars();
   
   virtual bool           IsPerturbing();
   virtual bool           IsSparsity();
   
protected:

   /// number of dyn functions
   Integer               numDynFunctions;
   /// indicates if user function has dynamics functions
   bool                  hasDynFunctions;

   /// pointer to data
   FunctionInputData     *paramData;
   /// pointer to path function container
   PathFunctionContainer *pfContainer;

   
};
#endif // UserPathFunction_hpp
