//------------------------------------------------------------------------------
//                              NLPFuncUtil_MultiPoint
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
// Created: 2016.07.13
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef NLPFuncUtil_MultiPoint_hpp
#define NLPFuncUtil_MultiPoint_hpp

#include "csaltdefs.hpp"
#include "NLPFuncUtil.hpp"
#include "Phase.hpp"
#include "NLPFunctionData.hpp"
#include "JacobianData.hpp"
#include "Rvector.hpp"
#include "SparseMatrixUtil.hpp"

class NLPFuncUtil_MultiPoint : public NLPFuncUtil
{
public:
   
   NLPFuncUtil_MultiPoint();
   NLPFuncUtil_MultiPoint(const NLPFuncUtil_MultiPoint &copy);
   NLPFuncUtil_MultiPoint& operator=(const NLPFuncUtil_MultiPoint &copy);
   virtual ~NLPFuncUtil_MultiPoint();
   
   // NOTE: jData changes every time we compute function and jacobian?
   // otherwise, we can make it as a private property and save it during
   // initialization, and use it when we compute functions and jacobians all
   // the time. - YK
   virtual void     Initialize(const std::vector<Phase*> &pList,
                               const FunctionOutputData  &funcData,
                               JacobianData              *jData,
                               Integer                   numVarsNLP);
   // changed function name from UpdateUserFunctionArrays for the naming
   // consistency of NLPFuncUtil classes - YK
   // NOTE: jData changes every time we compute function and jacobian?
   // otherwise, we can make it as a private property and save it during
   // initialization, and use it when we compute functions and jacobians all
   // the time.
   virtual void     FillUserNLPMatrices(const FunctionOutputData &funcData,
                                        JacobianData             *jData,
                                        Rvector                  &funcValues,
                                        RSMatrix                 &jacArray);
   
   // muted by YKK merged to ComputeFuncAndJac to avoid multiple computation of
   // user NLP Matrices (i.e., q and dq/dz)
   // virtual Rvector  ComputeFunctions();
   // virtual RSMatrix ComputeJacobian();
   // muted by YKK; we need a pointer instead of new RSMatrix
   // virtual RSMatrix ComputeSparsity();
   virtual void  ComputeFuncAndJac(const FunctionOutputData &funcData,
                                    JacobianData            *jData,
                                    Rvector                 &funcValues,
                                    RSMatrix                &jacArray);

   // newly added by YK; NOTE that I do not think we need new sparse matrix here
   // because SparseMatrixUtil::SetSparseBLockMatrix does not require the
   // sub-block sparse matrix but requires a pointer to the sub-block matrix.
   RSMatrix*     ComputeSparsityPattern();
   
protected:
   
   /// Data
   NLPFunctionData     *functionData;
   /// Number of functions
   Integer             numFunctions;
   /// Function offset in the total NLP problem
   Integer             functionOffset;
   /// Number of phases
   Integer             numPhases;
   /// List of phases
   std::vector<Phase*> phaseList;

   // the number of variables in NLP utility, added by YK
   Integer             numVars; 
   
};

#endif // NLPFuncUtil_MultiPoint_hpp
