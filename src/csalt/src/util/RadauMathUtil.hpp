//------------------------------------------------------------------------------
//                         RadauMathUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Youngkwang Kim
// Created: 2015/07/08
//
// NOTE: This is a static utility class.  The constructors, destructor, and
// operator= are not implemented.  No instances of this class can be
// instantiated.
//
/**
 */
//------------------------------------------------------------------------------
#ifndef RadauMathUtil_hpp
#define RadauMathUtil_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "SparseMatrixUtil.hpp" // for sparse matrix

/*
 * RadauMathUtil class
*/

class RadauMathUtil
{
public:
   //  the methods using Rvector
   /// 'collocD.m'; this algorithm does not assume LGR nodes.
   static RSMatrix GetLagrangeDiffMatrix(const Rvector *lgrNodes);     

   /// 'lgrPS.m'; this algorithm assumes multiple sets of LGR nodes on [-1,1] 
   static bool     ComputeMultiSegmentLGRNodes(const Rvector *segmentPoints,
                                               const IntegerArray *numNodesVec,
                                               Rvector &lgrNodes,
                                               Rvector &lgrWeightVec,
                                               RSMatrix &lagDiffMat);

   /// 'lgrnodes.m'; this algorithm assumes single set of LGR nodes on [-1,1] 
   static void     ComputeSingleSegLGRNodes(Integer numNodes,
                                            Rvector &lgrNodes,
                                            Rvector &lgrWeightVec);
   
private:
   /// private constructors, destructor, operator=   *** UNIMPLEMENTED ***
   RadauMathUtil();
   RadauMathUtil(const RadauMathUtil &copy);
   RadauMathUtil& operator=(const RadauMathUtil &copy);
   virtual ~RadauMathUtil();
   
};

#endif // RadauMathUtil_hpp

