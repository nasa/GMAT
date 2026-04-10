//------------------------------------------------------------------------------
//                           LinearAlgebra
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2017.03.23
//
/**
 * Implementation of the LinearAlgebra class
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "LinearAlgebra.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "TATCException.hpp" // need new specific exception class?
#include "MessageInterface.hpp"

//#define DEBUG_LINE_SEGMENT

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public static methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void LineSegmentIntersect(const Rmatrix &XY1,
//                            const Rmatrix &XY2,
//                            IntegerArray  adjacency,
//                            Rmatrix       matrixX,
//                            Rmatrix       matrixY,
//                            Rmatrix       distance1To2,
//                            Rmatrix       distance2To1,
//                            IntegerArray  parallelAdjacency,
//                            IntegerArray  coincidentAdjacency)
//------------------------------------------------------------------------------
/**
 * This method finds the 2D Cartesian Coordinates of
 * intersection points between the set of line segments given in XY1 and XY2
 *
 * @param XY1            array of line segments - each line is (x1, y1, x2, y2)
 *                       where (x1,y1) is the start point and (x2,y2) is the end
 * @param XY2            array of line segments - each line is (x1, y1, x2, y2)
 *                       where (x1,y1) is the start point and (x2,y2) is the end
 * @param adjacency      [output] entry (i,j) is 1 if line segments XY1(i,*) 
 *                       and XY2(j,*)intersect; 0 otherwise
 * @param matrixX        [output] entry (i,j) is the X coordinate of the 
 *                       intersection point between line segments XY1(i,*) 
 *                       and XY2(j,*)
 * @param matrixY        [output] entry (i,j) is the Y coordinate of the 
 *                       intersection point between line segments XY1(i,*) 
 *                       and XY2(j,*)
 * @param distance1To2   [output] entry (i,j) is the normalized distance from
 *                       the start point of the line segment XY1(i,*) to the
 *                       intersection point with XY2(j,*)
 * @param distance2To1   [output] entry (i,j) is the normalized distance from
 *                       the start point of the line segment XY1(j,*) to the
 *                       intersection point with XY2(i,*)
 * @param parallelAdjacency [output] entry (i,j) is 1 if line segments
 *                        XY1(i,*) and XY2(j,*) are parallel; 0 otherwise
 * @param coincidentAdjacency [output] entry (i,j) is 1 if line segments
 *                        XY1(i,*) and XY2(j,*) are coincident; 0 otherwise
 *
 * @note Notes from original MATLAB:
 * function out = lineSegmentIntersect(XY1,XY2)
 * LINESEGMENTINTERSECT Intersections of line segments.
 *   OUT = LINESEGMENTINTERSECT(XY1,XY2) finds the 2D Cartesian Coordinates of
 *   intersection points between the set of line segments given in XY1 and XY2.
 *
 *   XY1 and XY2 are N1x4 and N2x4 matrices. Rows correspond to line segments.
 *   Each row is of the form [x1 y1 x2 y2] where (x1,y1) is the start point and
 *   (x2,y2) is the end point of a line segment:
 *
 *                  Line Segment
 *       o--------------------------------o
 *       ^                                ^
 *    (x1,y1)                          (x2,y2)
 *
 *   OUT is a structure with fields:
 *
 *   'intAdjacencyMatrix' : N1xN2 indicator matrix where the entry (i,j) is 1 if
 *       line segments XY1(i,:) and XY2(j,:) intersect.
 *
 *   'intMatrixX' : N1xN2 matrix where the entry (i,j) is the X coordinate of the
 *       intersection point between line segments XY1(i,:) and XY2(j,:).
 *
 *   'intMatrixY' : N1xN2 matrix where the entry (i,j) is the Y coordinate of the
 *       intersection point between line segments XY1(i,:) and XY2(j,:).
 *
 *   'intNormalizedDistance1To2' : N1xN2 matrix where the (i,j) entry is the
 *       normalized distance from the start point of line segment XY1(i,:) to the
 *       intersection point with XY2(j,:).
 *
 *   'intNormalizedDistance2To1' : N1xN2 matrix where the (i,j) entry is the
 *       normalized distance from the start point of line segment XY1(j,:) to the
 *       intersection point with XY2(i,:).
 *
 *   'parAdjacencyMatrix' : N1xN2 indicator matrix where the (i,j) entry is 1 if
 *       line segments XY1(i,:) and XY2(j,:) are parallel.
 *
 *   'coincAdjacencyMatrix' : N1xN2 indicator matrix where the (i,j) entry is 1
 *       if line segments XY1(i,:) and XY2(j,:) are coincident.
 *
 * Version: 1.00, April 03, 2010
 * Version: 1.10, April 10, 2010
 * Author:  U. Murat Erdem
 *
 * CHANGELOG:
 *
 * Ver. 1.00:
 *   -Initial release.
 *
 * Ver. 1.10:
 *   - Changed the input parameters. Now the function accepts two sets of line
 *   segments. The intersection analysis is done between these sets and not in
 *   the same set.
 *   - Changed and added fields of the output. Now the analysis provides more
 *   information about the intersections and line segments.
 *   - Performance tweaks.
 *
 * I opted not to call this 'curve intersect' because it would be misleading
 * unless you accept that curves are pairwise linear constructs.
 * I tried to put emphasis on speed by vectorizing the code as much as possible.
 * There should still be enough room to optimize the code but I left those out
 * for the sake of clarity.
 * The math behind is given in:
 *   http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
 * If you really are interested in squeezing as much horse power as possible out
 * of this code I would advise to remove the argument checks and tweak the
 * creation of the OUT a little bit.
 */
//------------------------------------------------------------------------------
void LinearAlgebra::LineSegmentIntersect(const Rmatrix    &XY1,
                               const Rmatrix              &XY2,
                               std::vector<IntegerArray>  &adjacency,
                               Rmatrix                    &matrixX,
                               Rmatrix                    &matrixY,
                               Rmatrix                    &distance1To2,
                               Rmatrix                    &distance2To1,
                               std::vector<IntegerArray>  &parallelAdjacency,
                               std::vector<IntegerArray>  &coincidentAdjacency)
{
   if ((!XY1.IsSized()) || (!XY2.IsSized()))
      throw TATCException(
            "ERROR: arguments to LineSegmentIntersect must be sized\n");

   Integer sz1row, sz1col, sz2row, sz2col;
   XY1.GetSize(sz1row, sz1col);
   XY2.GetSize(sz2row, sz2col);
   
   if ((sz1col != 4) || (sz2col !=4))
      throw TATCException(
            "ERROR: arguments to LineSegmentIntersect must be nx4 matrices\n");
   
   #ifdef DEBUG_LINE_SEGMENT
      MessageInterface::ShowMessage("Entering LineSegmentIntersect with:\n");
      MessageInterface::ShowMessage("  XY1(%d, %d)\n", sz1row, sz1col);
      MessageInterface::ShowMessage("  XY2(%d, %d)\n", sz2row, sz2col);
   #endif
   
   // Prepare matrices for vectorized computation of line intersection points.
   Rmatrix X1(sz1row, sz2row); // sz2row copies of XY1's column 0
   Rmatrix X2(sz1row, sz2row); // sz2row copies of XY1's column 2
   Rmatrix Y1(sz1row, sz2row); // sz2row copies of XY1's column 1
   Rmatrix Y2(sz1row, sz2row); // sz2row copies of XY1's column 3
   for (unsigned int jj = 0; jj < sz2row; jj++)
   {
      for (unsigned int ii = 0; ii < sz1row; ii++)
      {
         X1(ii,jj) = XY1(ii,0);
         X2(ii,jj) = XY1(ii,2);
         Y1(ii,jj) = XY1(ii,1);
         Y2(ii,jj) = XY1(ii,3);
      }
   }
   
   Rmatrix XY2T(sz2col, sz2row);
   XY2T = XY2.Transpose();
   #ifdef DEBUG_LINE_SEGMENT
      MessageInterface::ShowMessage("  XY2 transposed to XY2T(%d, %d): \n%s\n",
                                    sz2col, sz2row, XY2T.ToString(6).c_str());
   #endif
   
   Rmatrix X3(sz1row, sz2row); // sz1row copies of XY2T's row 0
   Rmatrix X4(sz1row, sz2row); // sz1row copies of XY2T's row 2
   Rmatrix Y3(sz1row, sz2row); // sz1row copies of XY2T's row 1
   Rmatrix Y4(sz1row, sz2row); // sz1row copies of XY2T's row 3
   for (unsigned int ii = 0; ii < sz1row; ii++)
   {
      for (unsigned int jj = 0; jj < sz2row; jj++)
      {
         X3(ii,jj) = XY2T(0,jj);
         X4(ii,jj) = XY2T(2,jj);
         Y3(ii,jj) = XY2T(1,jj);
         Y4(ii,jj) = XY2T(3,jj);
      }
   }

   #ifdef DEBUG_LINE_SEGMENT
      MessageInterface::ShowMessage("X3 = %s\n", X3.ToString(6).c_str());
      MessageInterface::ShowMessage("X4 = %s\n", X4.ToString(6).c_str());
      MessageInterface::ShowMessage("Y3 = %s\n", Y3.ToString(6).c_str());
      MessageInterface::ShowMessage("Y4 = %s\n", Y4.ToString(6).c_str());
   #endif
   
   
   Rmatrix X4_X3(sz1row, sz2row);
   Rmatrix Y1_Y3(sz1row, sz2row);
   Rmatrix Y4_Y3(sz1row, sz2row);
   Rmatrix X1_X3(sz1row, sz2row);
   Rmatrix X2_X1(sz1row, sz2row);
   Rmatrix Y2_Y1(sz1row, sz2row);
   
   X4_X3 = (X4 - X3);
   Y1_Y3 = (Y1 - Y3);
   Y4_Y3 = (Y4 - Y3);
   X1_X3 = (X1 - X3);
   X2_X1 = (X2 - X1);
   Y2_Y1 = (Y2 - Y1);
   
   
   Rmatrix numA(sz1row, sz2row);
   Rmatrix numB(sz1row, sz2row);
   Rmatrix denom(sz1row, sz2row);
   Rmatrix u_a(sz1row, sz2row);
   Rmatrix u_b(sz1row, sz2row);
   
   numA  = X4_X3.ElementWiseMultiply(Y1_Y3) - Y4_Y3.ElementWiseMultiply(X1_X3);
   numB  = X2_X1.ElementWiseMultiply(Y1_Y3) - Y2_Y1.ElementWiseMultiply(X1_X3);
   denom = Y4_Y3.ElementWiseMultiply(X2_X1) - X4_X3.ElementWiseMultiply(Y2_Y1);
   
   u_a   = numA.ElementWiseDivide(denom);
   u_b   = numB.ElementWiseDivide(denom);
   
   // Find the adjacency matrix A of intersecting lines.
   
   Rmatrix INT_X(sz1row, sz2row);
   Rmatrix INT_Y(sz1row, sz2row);
   Rmatrix INT_B(sz1row, sz2row); // need Real version first, to multiply

   INT_X   = X1 + X2_X1.ElementWiseMultiply(u_a);
   INT_Y   = Y1 + Y2_Y1.ElementWiseMultiply(u_a);
   
   adjacency.clear();
   IntegerArray adjRow;
   IntegerArray parallelRow;
   IntegerArray coincidentRow;
   for (Integer ii = 0; ii < sz1row; ii++)
   {
      adjRow.clear();
      parallelRow.clear();
      coincidentRow.clear();
      // First check for adjacency
      for (Integer jj = 0; jj < sz2row; jj++)
      {
         if ((u_a(ii,jj) >= -GmatRealConstants::REAL_EPSILON)      &&
             (u_a(ii,jj) <= 1.0 + GmatRealConstants::REAL_EPSILON) &&
             (u_b(ii,jj) >= -GmatRealConstants::REAL_EPSILON)      &&
             (u_b(ii,jj) <= 1.0 + GmatRealConstants::REAL_EPSILON))
         {
            INT_B(ii,jj) = 1.0;
            adjRow.push_back(1);
         }
         else
         {
            INT_B(ii,jj) = 0.0;
            adjRow.push_back(0);
         }
         // Now check for parallel or coincident adjacency
         if (denom(ii,jj) == 0)
         {
            parallelRow.push_back(1);
            if (numA(ii,jj) == 0 && (numB(ii,jj) == 0))
               coincidentRow.push_back(1);
            else
               coincidentRow.push_back(0);
         }
         else
         {
            parallelRow.push_back(0);
            coincidentRow.push_back(0);
         }
      }
      adjacency.push_back(adjRow);
      parallelAdjacency.push_back(parallelRow);
      coincidentAdjacency.push_back(coincidentRow);
   }
   
//   INT_B   = (u_a >= 0) & (u_a <= 1) & (u_b >= 0) & (u_b <= 1);
//   PAR_B   = denominator == 0;
//   COINC_B = (numerator_a == 0 & numerator_b == 0 & PAR_B);
   
   // Size Rmatrix outputs
   matrixX.SetSize(sz1row, sz2row);
   matrixY.SetSize(sz1row, sz2row);
   distance1To2.SetSize(sz1row, sz2row);
   distance2To1.SetSize(sz1row, sz2row);
   
   // Arrange output.
   matrixX      = INT_X.ElementWiseMultiply(INT_B);
   matrixY      = INT_Y.ElementWiseMultiply(INT_B);
   distance1To2 = u_a;
   distance2To1 = u_b;
//   out.parAdjacencyMatrix = PAR_B;
//   out.coincAdjacencyMatrix= COINC_B;

}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// class construction/destruction and operator= methods unimplemented

