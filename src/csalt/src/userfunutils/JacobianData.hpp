//------------------------------------------------------------------------------
//                              JacobianData
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
// Created: 2015.08.14
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef JacobianData_hpp
#define JacobianData_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Phase.hpp"

class JacobianData
{
public:
   
   JacobianData();
   JacobianData(const JacobianData &copy);
   JacobianData& operator=(const JacobianData &copy);
   virtual ~JacobianData();

   void      Initialize(Integer              numF,
                        bool                 hasF,
                        std::vector<Phase*>  pList,
                        const IntegerArray   &dvStartIdxs);
   bool      InitDataIndexes();
   bool      InitJacobianArrays();
   bool      InitDependencies();
   
   Rmatrix   GetInitTimeJacobianPattern(Integer phaseIdx);
   Rmatrix   GetFinalTimeJacobianPattern(Integer phaseIdx);
   Rmatrix   GetInitStateJacobianPattern(Integer phaseIdx);
   Rmatrix   GetFinalStateJacobianPattern(Integer phaseIdx);
   
   Rmatrix   GetInitTimeJacobian(Integer phaseIdx);
   Rmatrix   GetFinalTimeJacobian(Integer phaseIdx);
   Rmatrix   GetInitStateJacobian(Integer phaseIdx);
   Rmatrix   GetFinalStateJacobian(Integer phaseIdx);
   
   IntegerArray GetInitStateIdxs(Integer phaseIdx);
   IntegerArray GetFinalStateIdxs(Integer phaseIdx);
   IntegerArray GetInitTimeIdxs(Integer phaseIdx);
   IntegerArray GetFinalTimeIdxs(Integer phaseIdx);
   
   void      SetInitTimeJacobian(Integer phaseIdx,
                                 Integer row, Integer col, Real val);
   void      SetFinalTimeJacobian(Integer phaseIdx,
                                  Integer row, Integer col, Real val);
   void      SetInitStateJacobian(Integer phaseIdx,
                                 Integer row, Integer col, Real val);
   void      SetFinalStateJacobian(Integer phaseIdx,
                                  Integer row, Integer col, Real val);
   
   void      SetInitTimeJacobianPattern(Integer phaseIdx,
                                        const Rmatrix &toPattern);
   void      SetFinalTimeJacobianPattern(Integer phaseIdx,
                                         const Rmatrix &toPattern);
   void      SetInitStateJacobianPattern(Integer phaseIdx,
                                         const Rmatrix &toPattern);
   void      SetFinalStateJacobianPattern(Integer phaseIdx,
                                          const Rmatrix &toPattern);
   
   void      SetInitTimeJacobianPattern(Integer phaseIdx,
                                        Integer row, Integer col, 
                                        Real val);
   void      SetFinalTimeJacobianPattern(Integer phaseIdx,
                                         Integer row, Integer col, 
                                         Real val);
   void      SetInitStateJacobianPattern(Integer phaseIdx,
                                         Integer row, Integer col, 
                                         Real val);
   void      SetFinalStateJacobianPattern(Integer phaseIdx,
                                          Integer row, Integer col, 
                                          Real val);
   
   void      SetInitialTimeDependency(Integer phaseIdx, bool hasDepend);
   void      SetFinalTimeDependency(Integer phaseIdx, bool hasDepend);
   void      SetInitialStateDependency(Integer phaseIdx, bool hasDepend);
   void      SetFinalStateDependency(Integer phaseIdx, bool hasDepend);

   // YK mod static params
   Rmatrix   GetStaticJacobianPattern(Integer phaseIdx);
   IntegerArray GetStaticIdxs(Integer phaseIdx);
   Rmatrix   GetStaticJacobian(Integer phaseIdx);
   void      SetStaticJacobian(Integer phaseIdx,
                               Integer row, Integer col, Real val);
   void      SetStaticJacobianPattern(Integer phaseIdx,
                                      const Rmatrix &toPattern);
   void      SetStaticJacobianPattern(Integer phaseIdx,
                                      Integer row, Integer col, 
                                      Real val);
   void      SetStaticDependency(Integer phaseIdx, bool hasDepend);
   
protected:
   
   /// The Jacobian of boundary functions wrt initial time
   std::vector<Rmatrix> initTimeJacobian;
   /// The Jacobian of boundary functions wrt initial state
   std::vector<Rmatrix> initStateJacobian;
   /// The Jacobian of boundary functions wrt final time
   std::vector<Rmatrix> finalTimeJacobian;
   /// The Jacobian of boundary functions wrt final state
   std::vector<Rmatrix> finalStateJacobian;
   /// The Jacobian of boundary functions wrt static params; YK mod
   std::vector<Rmatrix> staticJacobian;

   
   // same size as number of phases <<<< should be integers
   
   /// Sparsity of Jacobian of boundary functions wrt time
   std::vector<Rmatrix> initTimeJacobianPattern;
   /// Sparsity of Jacobian of boundary functions wrt state
   std::vector<Rmatrix> initStateJacobianPattern;
   /// Sparsity of Jacobian of boundary functions wrt time
   std::vector<Rmatrix> finalTimeJacobianPattern;
   /// Sparsity of Jacobian of boundary functions wrt state
   std::vector<Rmatrix> finalStateJacobianPattern;
   /// Sparsity of Jacobian of boundary functions wrt static
   std::vector<Rmatrix> staticJacobianPattern;
   
   /// Indicates if phase bound functions have initial time dependency
   std::vector<bool>    hasInitTimeDependency;
   /// Indicates if phase bound functions have initial state dependency
   std::vector<bool>    hasInitStateDependency;
   /// Indicates if phase bound functions have final time dependency
   std::vector<bool>    hasFinalTimeDependency;
   /// Indicates if phase bound functions have final state dependency
   std::vector<bool>    hasFinalStateDependency;
   /// Indicates if phase bound functions have static dependency
   std::vector<bool>    hasStaticDependency;

   /// Initial state indexes
   std::vector<IntegerArray>    initialStateIdxs;
   /// Final state indexes
   std::vector<IntegerArray>    finalStateIdxs;
   /// Initial time indexes
   std::vector<IntegerArray>    initialTimeIdxs;
   /// Final time indexes
   std::vector<IntegerArray>    finalTimeIdxs;
   /// static indexes
   std::vector<IntegerArray>    staticIdxs;

   /// Number of phases
   Integer                   numPhases;
   /// Indicates whether or not there are functions
   bool                      hasFunctions;
   /// Number of functions
   Integer                   numFunctions;
   /// Pointers to all phases    *** Phase TBD
   std::vector<Phase*>       phaseList;
   /// Start index of each phase decision vector into total decision vector
   IntegerArray              decVecStartIdxs;
   
   void  CopyArrays(const JacobianData &jd);
  
};

#endif // JacobianData_hpp
