//------------------------------------------------------------------------------
//                      RadauMathUtilLib
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
//
// Author: Youngkwang Kim
// Created: 2015/06/23
//
/**
 */
//------------------------------------------------------------------------------
#include <algorithm>         // for plus
#include <functional>

#include "GmatConstants.hpp"
#include "RadauMathUtil.hpp"
#include "LowThrustException.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RADAU_MATH_UTIL

using GmatMathConstants::PI;


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public static methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// RSMatrix GetLagrangeDiffMatrix(const Rvector *lgrNodes)
//------------------------------------------------------------------------------
/**
 * Compute the differentiation matrix from single segment lgrNodes.
 *
 * @param <lgrNodes>        the lgr nodes.
 *
 * return <lagDiffMatrix> the differentiation matrix from single segment 
 *                        lgrNodes
 */
//------------------------------------------------------------------------------
RSMatrix RadauMathUtil::GetLagrangeDiffMatrix(const Rvector *lgrNodes)
{
   //sort((*lgrNodes).begin(), (*lgrNodes).end()); 
   UnsignedInt N = (*lgrNodes).GetSize();
   UnsignedInt N1 = N + 1;
   UnsignedInt N2 = N*N;
         
   IntegerArray rowIdxVec, colIdxVec, zeroVec;
   Real tempValue;
   Rvector weigthVec;
   rowIdxVec.resize(N);
   colIdxVec.resize(N);
   weigthVec.SetSize(N);      
   zeroVec.resize(N);      
            
   for (UnsignedInt idx = 0; idx < N; ++idx)
   {
      // this is for the identity matrix
      rowIdxVec[idx] = idx;
      colIdxVec[idx] = idx;
      // those are vectors for various uses
      zeroVec[idx] = 0;
      weigthVec(idx) = 1;
   }

   RSMatrix spBlockMat, spMat, spDiffMat;
   SparseMatrixUtil::SetSize(spDiffMat, N, N);

   // setting a column vector in matrix form         
   SparseMatrixUtil::SetSparseMatrix(spBlockMat, N, 1, 
                                    &rowIdxVec, &zeroVec,
  	                                 lgrNodes);

   // Compute the barycentric Weights
   SparseMatrixUtil::ReplicateSparseMatrix(&spBlockMat, 1, N, spMat);
         
   // the following is for diffMat = mat + mat.' + eye(N);
   for (UnsignedInt idx1 = 0; idx1 < N; ++idx1)
   {
      for (UnsignedInt idx2 = 0; idx2 < N; ++idx2)
      {
         tempValue = SparseMatrixUtil::GetElement(&spMat,idx1, idx2) 
                     - SparseMatrixUtil::GetElement(&spMat, idx2, idx1);
         SparseMatrixUtil::SetElement(spDiffMat,idx1,idx2, tempValue);
         if (idx1 == idx2) 
            SparseMatrixUtil::SetElement(spDiffMat, idx1, idx2, ++tempValue);
      }
   }

   // the following is for weigthVec = 1/prod(spDiffMat,2);
   for (UnsignedInt idx = 0; idx < N; ++idx)
   {
      for (UnsignedInt idx2 = 0; idx2 < N; ++idx2)
      {  
         weigthVec(idx) /= SparseMatrixUtil::GetElement(&spDiffMat, idx, idx2);
      }
   } 

   // create a sparse matrix equivalent to weigthVec         
   SparseMatrixUtil::SetSparseMatrix(spBlockMat, N, 1, &rowIdxVec, &zeroVec, 
  	                  &weigthVec); 

   // the following is for W = repmat(weigthVec,1,N);         
   SparseMatrixUtil::ReplicateSparseMatrix(&spBlockMat, 1, N, spMat);
         
   RSMatrix lagDiffMat;
   SparseMatrixUtil::SetSize(lagDiffMat, N, N);

   UnsignedInt rowIdx, colIdx;
   IntegerArray colIdxs, rowIdxs;
   Rvector valueVec;

   // the following loop is for D = W./(W.'*DiffMat);
   SparseMatrixUtil::GetThreeVectorForm(&spDiffMat, rowIdxs, colIdxs, valueVec);
   for (UnsignedInt idx = 0; idx < rowIdxs.size(); ++idx)
   {
	   colIdx = colIdxs[idx];
	   rowIdx = rowIdxs[idx];

      tempValue = SparseMatrixUtil::GetElement(&spMat,rowIdx, colIdx) 
                  / SparseMatrixUtil::GetElement(&spMat,colIdx, rowIdx) 
                  / SparseMatrixUtil::GetElement(&spDiffMat,rowIdx, colIdx);

	   SparseMatrixUtil::SetElement(lagDiffMat,rowIdx, colIdx, tempValue);
   }

   std::vector<double> sumVec;
   sumVec.resize(N);
   for (UnsignedInt idx2 = 0; idx2 < N; ++idx2)
   {
      for (UnsignedInt idx1 = 0; idx1 < N; ++idx1)
      {
         sumVec[idx2] += SparseMatrixUtil::GetElement(&lagDiffMat,idx1, idx2);
      }
   }
   UnsignedInt idx = 0;
   for (UnsignedInt idx1 = 0; idx1 < N2; idx1 += N1)
   {
      colIdx = idx1 % N;
      rowIdx = (idx1 - colIdx)/N;

      tempValue = 1 - sumVec[idx];
      SparseMatrixUtil::SetElement(lagDiffMat,rowIdx, colIdx, tempValue);
      ++idx;                        
   }

   RSMatrix lagDiffMatrix2;
   SparseMatrixUtil::SetSize(lagDiffMatrix2, N, N);

   for (UnsignedInt idx1 = 0; idx1 < N; ++idx1)
   {
      for (UnsignedInt idx2 = 0; idx2 < N; ++idx2)
      {
         tempValue = -SparseMatrixUtil::GetElement(&lagDiffMat, idx2, idx1);
         SparseMatrixUtil::SetElement(lagDiffMatrix2, idx1, idx2, tempValue);
      }
   }

   #ifdef DEBUG_RADAU_MATH_UTIL
      MessageInterface::ShowMessage("test 1 for Radau Math Refactoring.\n");
   #endif

   return lagDiffMatrix2;
}

//------------------------------------------------------------------------------
// bool ComputeMultiSegmentLGRNodes(const Rvector      *segmentPoints,
//                                  const IntegerArray *numNodesVec,
//                                  Rvector            &lgrNodes,
//                                  Rvector            &lgrWeightVec,
//                                  RSMatrix           &lagDiffMat)
//------------------------------------------------------------------------------
/**
 * Compute multi-segment Lagrange-Gauss-Radau nodes along with 
 * its differentiation matrix and weight vector for integration.
 *
 * @param <segmentPoints>   the points that define where each segments are 
 * @param <numNodesVec>     the array define the number of nodes of each segment
 * @param <lgrNodes>        the resultant lgr nodes.
 * @param <lgrWeightVec>    the resultant weight vector.
 * @param <lagDiffMat>      the resultant differentiation matrix.
 *
 * return <true>  when successful, 
 * return <false> otherwise.
 */
//------------------------------------------------------------------------------
bool RadauMathUtil::ComputeMultiSegmentLGRNodes(
                                       const Rvector      *segmentPoints,
                                       const IntegerArray *numNodesVec,
                                       Rvector            &lgrNodes,
                                       Rvector            &lgrWeightVec,
                                       RSMatrix           &lagDiffMat)
{
   UnsignedInt numMeshIntervals = (*numNodesVec).size();
   if ((*segmentPoints).GetSize() != (Integer)(numMeshIntervals + 1))
   {
      
//      MessageInterface::ShowMessage("the length of segmentPoints must be "
//      "the size of numNodesVec plus one\n");
      return false;
   }
   if ((*segmentPoints)(0) != -1)
   {
//      MessageInterface::ShowMessage("the first segment point must be -1\n");
      return false;
   }
   if ((*segmentPoints)(numMeshIntervals) != 1)
   {
      MessageInterface::ShowMessage("the last segment point must be 1\n");
      return false;
   }
   for (UnsignedInt idx = 0; idx < numMeshIntervals; ++idx)
   {
      if (((*segmentPoints)(idx + 1) - (*segmentPoints)(idx)) <= 0)
      {
//          MessageInterface::ShowMessage(
//          "the segment points must be strictly increasing\n");
         return false;
      } 
   }
   UnsignedInt Ntotal = 0;
   
   IntegerArray::const_iterator idxIt;

   for (idxIt = (*numNodesVec).begin(); 
         idxIt != (*numNodesVec).end();  ++idxIt)
   {
      Ntotal += *idxIt;
   }
   
   lgrNodes.SetSize(Ntotal + 1);
   lgrWeightVec.SetSize(Ntotal); 
   SparseMatrixUtil::SetSize(lagDiffMat, Ntotal,Ntotal + 1);
//   MessageInterface::ShowMessage(" initialization is successful\n");

   UnsignedInt rowshift = 0;
   UnsignedInt colshift = 0;
   Rvector lgrNodesPerSeg, lgrWeightPerSeg, lgrNodesPerSeg2, valueVec;
   RSMatrix diffBlockMatrix;
   IntegerArray rowIdxVec, colIdxVec;

   for (UnsignedInt idx = 0; idx < ((*numNodesVec).size()); ++idx)
   {
      ComputeSingleSegLGRNodes((*numNodesVec)[idx] -1, 
                                    lgrNodesPerSeg,
                                    lgrWeightPerSeg);

      lgrNodesPerSeg2.SetSize(lgrNodesPerSeg.GetSize()+1);
      for (int idx2 =0; idx2 < (lgrNodesPerSeg.GetSize()); ++idx2)
      {
               
         lgrNodesPerSeg(idx2) *= ((*segmentPoints)(idx+1)-
                                  (*segmentPoints)(idx))/2;
         lgrNodesPerSeg(idx2) += ((*segmentPoints)(idx+1)+
                                  (*segmentPoints)(idx))/2;
         lgrNodesPerSeg2(idx2) = lgrNodesPerSeg(idx2);
         lgrNodes(rowshift + idx2) = lgrNodesPerSeg(idx2);

         lgrWeightPerSeg(idx2) *= ((*segmentPoints)(idx+1)-
                                   (*segmentPoints)(idx))/2;
         lgrWeightVec(rowshift + idx2) = lgrWeightPerSeg(idx2);
      }
      lgrNodesPerSeg2(lgrNodesPerSeg.GetSize()) = (*segmentPoints)(idx+1);
      diffBlockMatrix = GetLagrangeDiffMatrix(&lgrNodesPerSeg2);

      SparseMatrixUtil::GetThreeVectorForm(&diffBlockMatrix, 
                           0, ((*numNodesVec)[idx]) - 1,
                           0, ((*numNodesVec)[idx]),
                           rowIdxVec, 
	                        colIdxVec, 
                           valueVec); 
      SparseMatrixUtil::SetSparseBLockMatrix(lagDiffMat, 
                           rowshift, colshift, 
	                        &rowIdxVec, 
                           &colIdxVec, 
                           &valueVec); 
      //// update the row and column shitfs
      rowshift = rowshift+(*numNodesVec)[idx];
      colshift = colshift+(*numNodesVec)[idx];
   }
         
   lgrNodes[Ntotal] = 1;
   
   #ifdef DEBUG_RADAU_MATH_UTIL
      MessageInterface::ShowMessage("test 3 for Radau Math Refactoring.\n");
   #endif
   return true;
}

//------------------------------------------------------------------------------
// void ComputeSingleSegLGRNodes(Integer N, Rvector &x, Rvector &lgrWeightVec)
//------------------------------------------------------------------------------
/**
 * Compute single-segment Lagrange-Gauss-Radau nodes along with
 * its weight vector for integration.
 *
 * @param <N>               the number of nodes 
 * @param <x>               the resultant lgr nodes.
 * @param <lgrWeightVec>    the resultant weight vector.
 *
 * return nothing
 */
//------------------------------------------------------------------------------
void RadauMathUtil::ComputeSingleSegLGRNodes(Integer N,
                                             Rvector &x,
                                             Rvector &lgrWeightVec)
{
   Real tempValue;
   UnsignedInt N1 = N+1;
   x.SetSize(N1);
   lgrWeightVec.SetSize(N1);

   // Use Chebyshev-Gauss-Radau nodes as initial guess for LGR nodes
         
   for (UnsignedInt idx = 0; idx < N1; ++idx)
   {
      x(idx) = -cos(2*PI*idx/(2*N+1));
            
   }                 
         
   std::vector<double> emptyValueVec;
   IntegerArray emptyIdxVec;
   //The Legendre Vandermonde Matrix
   RSMatrix P;
   SparseMatrixUtil::SetSparseMatrix(P, N1, N1+1, 
            &emptyIdxVec, &emptyIdxVec, &emptyValueVec);

   // Compute P_(N) using the recursion relation
   // Compute its first and second derivatives and 
   // update x using the Newton-Raphson method.

   // Free abscissae
   IntegerArray free;         
   Rvector xold;
   std::vector<double> xAbsDiff;
   free.resize(N);
   xold.SetSize(N1);
   xAbsDiff.resize(N1);
         
   for (Integer idx = 0; idx < N; ++idx)
   {
      free[idx] = idx + 1;
      xold(idx) = 2;
      xAbsDiff[idx] = GmatMathUtil::Abs(x(idx) - xold[idx]);
   }
   xold(N) = 2;         
   xAbsDiff[N] = GmatMathUtil::Abs(x(N) - xold(N));

   std::vector<double>::iterator maxIt;
   maxIt = std::max_element(xAbsDiff.begin(), xAbsDiff.end());
   double maxError = *maxIt;
   while (maxError > 2.220446049250313e-16)
   {
      xold = x;
      for (int idx = 0; idx < N+2; ++idx)
      {
         if (idx % 2 == 0)
            SparseMatrixUtil::SetElement(P,0,idx, 1.0);
         else
            SparseMatrixUtil::SetElement(P,0, idx, -1.0);
      }            
      for (UnsignedInt idx = 1; idx < N1; ++idx)
      {
         SparseMatrixUtil::SetElement(P,idx, 0, 1);
         SparseMatrixUtil::SetElement(P,idx, 1, x(idx));
      }
      for (UnsignedInt k = 2; k < N1+1; ++k)
      {
         for (UnsignedInt idx = 1; idx <N1; ++idx)
         {
            tempValue = ((2 * k - 1)*x(idx)*
                         SparseMatrixUtil::GetElement(&P,idx, k - 1)
                        - (k - 1)*SparseMatrixUtil::GetElement(&P,idx, k - 2))
                        / k;
            SparseMatrixUtil::SetElement(P,idx, k, tempValue);
         }
      }
      for (UnsignedInt idx = 1; idx <N1; ++idx)
      {               
         x(idx) = xold(idx) - ((1 - xold(idx)) / N1)
                  * (SparseMatrixUtil::GetElement(&P, idx, N) 
                  + SparseMatrixUtil::GetElement(&P, idx, N1))
                  / (SparseMatrixUtil::GetElement(&P, idx, N)
                  - SparseMatrixUtil::GetElement(&P,idx, N1));
         xAbsDiff[idx] = GmatMathUtil::Abs(x(idx) - xold(idx));
      }                
      xAbsDiff[0] = GmatMathUtil::Abs(x(0) - xold(0));
      maxIt = std::max_element(xAbsDiff.begin(), xAbsDiff.end());
      maxError = *maxIt;
   }


   lgrWeightVec(0) = 2.0;
   lgrWeightVec(0) /= N1*N1;
         
   for (UnsignedInt idx = 1; idx <N1; ++idx)
   {
      lgrWeightVec(idx) = (1 - x(idx));
      lgrWeightVec(idx) /= (N1*SparseMatrixUtil::GetElement(&P, idx, N))
                           * (N1*SparseMatrixUtil::GetElement(&P,idx, N));
           
   }
   #ifdef DEBUG_RADAU_MATH_UTIL
      MessageInterface::ShowMessage("test 2 for Radau Math Refactoring.\n");
   #endif
   return;
} 
