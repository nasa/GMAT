//------------------------------------------------------------------------------
//                              FunctionOutputData
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
// Created: 2015.07.13
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 *
 *  This class stores user function data for the optimal control
 *  problem.  This data includes the function values and Jacobians.
 *  The data on this class is used to form NLP function values.
 *
 */
//------------------------------------------------------------------------------

#ifndef FunctionOutputData_hpp
#define FunctionOutputData_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "UserFunction.hpp"

class FunctionOutputData
{
public:

   // class methods
   FunctionOutputData();
   FunctionOutputData(const FunctionOutputData &copy);
   FunctionOutputData& operator=(const FunctionOutputData &copy);
   
   virtual ~FunctionOutputData();
   
   /// Sets the function descriptions
   virtual void    SetFunctionNames(StringArray fNames);

   /// Sets number of functions and function values
   virtual void    SetFunctionValues(Integer numFuncs,
                                     const Rvector &funcValues);
   /// Sets function values
   virtual void    SetFunctions(const Rvector &funcValues);
   /// Sets the number of functions
   virtual void    SetNumFunctions(Integer numFuncs);
   /// Sets initialization flag
   virtual void    SetIsInitializing(bool isInit);
   /// Sets the upper bounds
   virtual bool    SetUpperBounds(const Rvector &toUpper);
   /// Sets the lower bounds
   virtual bool    SetLowerBounds(const Rvector &toLower);
   
   bool                SetNLPData(Integer meshIdx, Integer stageIdx,
                                  const IntegerArray &stateIdxs,
                                  const IntegerArray &controlIdxs, 
                                  const IntegerArray &staticIdxs);
   
   DEPRECATED(bool                SetStateJacobian(const Rmatrix& sj));
   DEPRECATED(bool                SetControlJacobian(const Rmatrix& cj));
   DEPRECATED(bool                SetTimeJacobian(const Rmatrix& tj));
   
   DEPRECATED(const Rmatrix&      GetStateJacobian());
   DEPRECATED(const Rmatrix&      GetControlJacobian());
   DEPRECATED(const Rmatrix&      GetTimeJacobian());
   
   DEPRECATED(bool                HasUserStateJacobian());
   DEPRECATED(bool                HasUserControlJacobian());
   DEPRECATED(bool                HasUserTimeJacobian());

   bool                SetJacobian(UserFunction::JacobianType jacType, const Rmatrix& j);
   const Rmatrix&      GetJacobian(UserFunction::JacobianType jacType);
   bool                HasUserJacobian(UserFunction::JacobianType jacType);
   std::vector<bool>   HasUserJacobian();

   const IntegerArray& GetStateIdxs();
   const IntegerArray& GetControlIdxs();
   // YK mod static vars
   const IntegerArray& GetStaticIdxs();

   Integer             GetMeshIdx();
   Integer             GetStageIdx();
   /// Does it have a user function?
   virtual bool        HasUserFunction() const;
   /// Returns the number of functions
   virtual Integer     GetNumFunctions() const;
   /// returns the function descriptions
   virtual const StringArray&
                       GetFunctionNames() const;
   /// returns array of function values
   virtual const Rvector&
                       GetFunctionValues() const;
   /// Returns the array of upper bounds
   virtual const Rvector&
                       GetUpperBounds();
   /// Returns the array of lower bounds
   virtual const Rvector&
                       GetLowerBounds();
   /// Have the bounds been set?
   virtual bool        BoundsSet();
   /// Is it initializing?
   virtual bool        IsInitializing() const;

   
protected:
   /// indicates if the user has defined data of this type
   bool         hasUserFunction;
   /// indicates of the user has provided state Jacobian
   //bool         hasUserStateJacobian;
   /// indicates of the user has provided control Jacobian
   //bool         hasUserControlJacobian;
   /// indicates of the user has provided time Jacobian
   //bool         hasUserTimeJacobian;

   std::vector<bool> hasJacobian;

   /// mesh index
   Integer      meshIndex;
   /// stage index
   Integer      stageIndex;
   /// state indeces in phase decision vector
   IntegerArray stateIndexes;
   /// control indeces in phase decision vector
   IntegerArray controlIndexes;
   /// static var indices in phase decision vector; YK mod static vars
   IntegerArray staticIndexes;
   /// Jacobian of the function wrt state
   //Rmatrix      stateJacobian;
   /// Jacobian of the function wrt control
   //Rmatrix      controlJacobian;
   /// Jacobian of the function wrt time
   //Rmatrix      timeJacobian;

   std::map<UserFunction::JacobianType, Rmatrix> jacobian;

   /// indicates if it is initializing or not.  Note,
   /// some data can optionally be set by the user, and if not provided by user
   /// the system must fill it in.  What is/is not provided by user is
   /// determined during intialization and flags are set accordingly at
   /// that time.
   bool         isInitializing;
   /// the function values
   Rvector      functionValues;
   /// number of functions
   Integer      numFunctions;
   /// upper bounds on function values
   Rvector      upperBounds;
   /// lower bounds on function values
   Rvector      lowerBounds;
   /// String description of function values
   StringArray  functionNames;
   /// Have the bounds on the function values been set?
   bool         boundsSet;
   
};

#endif  // FunctionOutputData_hpp
