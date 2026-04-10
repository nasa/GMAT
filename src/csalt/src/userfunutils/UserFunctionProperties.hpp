//------------------------------------------------------------------------------
//                           UserFunctionProperties
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
// Created: 2015.12.04
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef UserFunctionProperties_hpp
#define UserFunctionProperties_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "UserFunction.hpp"

class UserFunctionProperties
{
public:
   
   UserFunctionProperties();
   UserFunctionProperties(const UserFunctionProperties &copy);
   UserFunctionProperties& operator=(const UserFunctionProperties &copy);
   virtual ~UserFunctionProperties();
   
   // YK mod static vars: vectorization of methods
   virtual void SetJacobianPattern(UserFunction::JacobianType idx, const Rmatrix &pattern);
   DEPRECATED(virtual void      SetStateJacobianPattern(const Rmatrix &pattern));
   DEPRECATED(virtual void      SetTimeJacobianPattern(const Rmatrix &pattern));
   DEPRECATED(virtual void      SetControlJacobianPattern(const Rmatrix &pattern));
   DEPRECATED(virtual void      SetStaticJacobianPattern(const Rmatrix &pattern)); // static variables
   
   virtual void      SetNumberOfFunctions(Integer numFun);

   virtual void      SetHasVars(UserFunction::JacobianType idx, bool hasVars);
   DEPRECATED(virtual void      SetHasStateVars(bool hasVars));
   DEPRECATED(virtual void      SetHasControlVars(bool hasVars));
   DEPRECATED(virtual void      SetHasStaticVars(bool hasVars)); // static variables
   
   DEPRECATED(virtual const Rmatrix&    GetStateJacobianPattern());
   DEPRECATED(virtual const Rmatrix&    GetTimeJacobianPattern());
   DEPRECATED(virtual const Rmatrix&    GetControlJacobianPattern());
   DEPRECATED(virtual const Rmatrix&    GetStaticJacobianPattern()); // static variables
   virtual const Rmatrix&    GetJacobianPattern(UserFunction::JacobianType idx);
   
   virtual Integer           GetNumberOfFunctions();
   virtual bool              HasVars(UserFunction::JacobianType idx);
   DEPRECATED(virtual bool              HasStateVars());
   DEPRECATED(virtual bool              HasControlVars());
   DEPRECATED(virtual bool              HasStaticVars()); // static variables
   
   
protected:
   
   /*
   /// Jacobian pattern for state derivatives
   Rmatrix stateJacobianPattern;
   /// Jacobian pattern for time derivatives
   Rmatrix timeJacobianPattern;
   /// Jacobian pattern for control derivatives
   Rmatrix controlJacobianPattern;
   /// Jacobian pattern for static derivatives; YK mod static params
   Rmatrix staticJacobianPattern;
   */
   std::vector<Rmatrix> jacobianPattern;

   /// the number of functions
   
   Integer numFunctions;
   std::vector<bool> hasVars;
   /*
   /// true if user problem has state variables
   bool    hasStateVars;
   /// true if user problem has control variables
   bool    hasControlVars;
   /// true if user problem has static variables; YK mod static params
   bool    hasStaticVars;
   */
   
};
#endif // UserFunctionProperties_hpp

