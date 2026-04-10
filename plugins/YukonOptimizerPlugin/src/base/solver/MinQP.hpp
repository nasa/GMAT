//$Id$
//------------------------------------------------------------------------------
//                                MinQP
//------------------------------------------------------------------------------  
// GMAT: General Mission Analysis Tool
//
// MinQP finds a solution to the quadratic programming problem
// defined by :
//               min 0.5*x'*G*x + x'*d
//        subject to  b_lower <= A*x < = b_upper
//
// MinQP uses an active set method similar to algorithm 16.1 of Nocedal
// and Wright.  A feasible initial guess is calculated using a "Phase I"
// stage that accepts a user provided estimate for a feasible point.The
// Phase I stage solves a simplified optimization problem, that minimizes
// the infinity norm of the constraint violations using a method based on
// section 7.9.2 of Ref.[2].  If the infinity norm of the constraint violation
// is zero, then a feasible solution has been found.  If the variance is
// non - zero, then there is not a feasible solution.  To solve the
// simplified Phase I problem, MinQP is called recursively with
// modifications made to G, f, A, and b.
//
// At each iteration, the KKT conditions are solved using the null
// space method. MinQP supports a "Hot Start" strategy, which means
// the user can provide a guess for the active constraint set. This is
// important when using QPSOLVE inside of an SQP optimizer.
//
// exitFlag:  1 Converged
//            0 Invalid QP problem.Mistake in the problem definition
//            -1 The QP problem is not feasible
//            -2 Max iterations reached before convergence
//            -3 Singular QP, no free variables
//            -4 Failed null space solution
//            -5 Failed to solve for Lagrange multipliers
//
//  References:
//  1) Nocedal, J., and Wright, S., "Numerical Optimization", 2nd Edition,
//  Springer, Ithica, NY., 2006.
//
//  2) Gill, P.E., Murray, W., and Wright, M.H., " Numerical Linear Algebra
//  and Optimization, "  Addison Wesley Publishing Company, Redwood City,
//  CA, 1991.
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
// express or implied.  See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Author: S.Hughes, steven.p.hughes at nasa.gov
//
// Code Conversion: Joshua Raymond, Thinking Systems, Inc.
//
// Modification History
// ---------------------------------------------------------------------- -
// Sept - 08     S.Hughes:  Created the initial version.
// Mar - 04 - 17   S.Hughes : Converted to OO MATLAB
// Jun - 20 - 17   S.Hughes : Modified to handle upper and lower bounds
//                            on constraints.  Renamed variables for
//                            readablity.
/**
 * Declares MinQP methods
 */
//------------------------------------------------------------------------------

#ifndef MinQP_hpp
#define MinQP_hpp

#include "gmatdefs.hpp"
#include "yukon_defs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "QRFactorization.hpp"
#include "LUFactorization.hpp"
#include "CholeskyFactorization.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"


class YUKON_API MinQP
{
public:
   MinQP(Rvector initGuess, Rmatrix G, Rvector d, Rmatrix AMatrix,
      Rvector conLB, Rvector conUB, Rvector W, Integer PhaseNum,
      bool checkForDuplicateCons);
   ~MinQP();
   void SetWriteOutput(bool flag);
   Rvector GetActiveSet();
   void Optimize(Rvector &sol, Real &q, Rvector &lagMult, Integer &Converged, 
      Integer &iter, Rvector &activeIS);
   Rmatrix GetModifiedCons();

   // Options below this line
   bool WriteOutput = false;

protected:
   /// Pointer to use LU factorization
   LUFactorization lu = LUFactorization(false);
   /// Pointer to use LU factorization with pivoting
   LUFactorization luPivot = LUFactorization(true);
   /// Pointer to use QR factorization
   QRFactorization qr = QRFactorization(false);

private:
   void PrepareOutput(Rvector &dV, Real &cV, Integer &numIter,
      Rvector &activeEqs, Integer exitFlag);
   void TestForNonZeroHessian();
   Real ComputeHessianScaleFactor();
   void SetConstraintTypes();
   void SetDecisionVector(Rvector dV);
   void ComputeConstraints();
   Rvector ComputeAllConViolations();
   Rvector ComputeActiveConViolations();
   Rvector ComputeIneqConViolations();
   void ComputeDistanceToInactiveCons(Rvector stepVector,
      Rvector &distanceToInactiveCons, Rvector &inactiveConIdxs);
   void ComputeNearbyFeasibleLinearSystem(Rvector decVec, Rmatrix &Anear,
      Rvector &bnear);
   void SetUpPhaseI(Rvector initialGuess, Rvector &initialGuess_I,
      Rmatrix &A_I, Rmatrix &G, Rvector &d, Rvector &b_L, Rvector &b_U);
   Integer ValidateProblemConfig();
   void TestForLinearlyDependentCons(bool checkForDuplicateCons);
   Real GetMax(Rvector inputVector);
   Real GetMin(Rvector inputVector);
   Real InfNorm(Rvector inputVector);
   Rvector multiMatrixToColumn(Rmatrix inputMatrix, Rvector inputVector);
   Rvector multiRowToMatrix(Rmatrix inputMatrix, Rvector inputVector);
   Rmatrix multiMatrix(Rmatrix matrix1, Rmatrix matrix2);
   Rmatrix TransposeMatrix(Rmatrix inputMatrix);

   /// Real Vector. Guess for optimization variables. numDecisionVars x 1
   Rvector initialGuess;
   /// Real Vector. The decision vector (vector of optization variables)
   /// numDecisionVars x 1
   Rvector decVec;
   /// Matrix. Hessian matrix (numDecisionVars x numDecisionVars)
   Rmatrix hessianMat;
   /// Real Vector. Gradient vector. numDecisionVars x 1
   Rvector gradVec;
   /// Matrix. Linear constraints A matrix A*x = b. numCons x numDecisionVars 
   Rmatrix A;
   /// Real Vector.Lower bound on linear constraints A*x.numCons x 1
   Rvector conLowerBounds;
   /// Real Vector.Upper bound on linear constraints A*x.numCons x 1
   Rvector conUpperBounds;
   /// Integer vector.Indeces of the equality constraints.numEqCons x 1
   Rvector eqIdxs;
   /// Integer vector.Indeces of the inequality constraints.Dimension varies.
   Rvector ineqIdxs;
   /// Integer.The problem phase(1 for infeasbible startup, 2 for feasible run)
   Integer Phase;
   /// Integer.Number of decision variables
   Integer numDecisionVars;
   /// Integer.Number of equality constraints
   Integer numEqCons;
   /// Integer.Number of inequality constraints
   Integer numIneqCons;
   /// Integer.The total number of constraints(equality + inequality)
   Integer numCons;
   /// Integer.Number of constraints in the working set
   Integer numWorkingIneqCons;
   /// Integer.Number of active constraints(num equality + num working)
   Integer numActiveCons;
   /// Integer vector.Indeces of working set.stdvec: Dimension varies.
   Rvector activeIneqSet;
   /// Inactive inequality constraint indeces.stdvec : Dimension varies.
   Rvector inactiveIneqSet;
   /// Real Vector.Product of A and decVec.numCons x1
   Rvector conValues;
   /// Integer array. Vector of 1s and 2s where 1 means inequality con and
   /// 2 means equality
   Rvector conType;
   /// Floating point relative accuracy
   Real eps = std::numeric_limits<double>::epsilon();
   /// Real. Tolerance on constraint satisfaction
   Real conTolerance;
   /// Bool. True if Hessian has all zero entries, false otherwise.
   bool isHessianZero;
   /// Bool. True if QR was found for current matrix in convergence step,
   /// used to avoid redundant caculations.
   bool calculatedQR;
   /// Integer. Number of QP iterations
   Integer numIterations;
   /// Real. The value of the cost function
   Real costValue;
   /// Integer Matrix. Contains indices of which constraints were combined
   /// of similar constraints were found
   Rmatrix combinedCons;
};

#endif
