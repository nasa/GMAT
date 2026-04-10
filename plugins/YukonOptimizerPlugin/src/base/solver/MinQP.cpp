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
// contract, Task 08
//
// Author.S.Hughes, steven.p.hughes at nasa.gov
//
// Code Conversion: Joshua Raymond, Thinking Systems, Inc.
//
// Modification History
// ----------------------------------------------------------------------------
// Sept - 08     S.Hughes:  Created the initial version.
// Mar - 04 - 17   S.Hughes : Converted to OO MATLAB
// Jun - 20 - 17   S.Hughes : Modified to handle upper and lower bounds
//                             on constraints.  Renamed variables for
//                             readablity.
/**
 * Defines MinQP methods
 */
//-----------------------------------------------------------------------------

#include "MinQP.hpp"
#include <iostream>
#include <iomanip>

//#define WRITE_DEBUG

//------------------------------------------------------------------------------
// MinQP(Rvector initGuess, Rmatrix G, Rvector d, Rmatrix AMatrix, 
//       Rvector conLB, Rvector conUB, Rvector W, Integer PhaseNum)
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param initGuess Initial guess vector
* @param G Hessian matrix
* @param d Gradient vector
* @param AMatrix Constraint Jacobian
* @param conLB Vector of constraint lower bound
* @param conUB Vector of constraint upper bound
* @param W Active inequality set vector
* @param PhaseNum The current phase number
*/
//------------------------------------------------------------------------------
MinQP::MinQP(Rvector initGuess, Rmatrix G, Rvector d, Rmatrix AMatrix,
             Rvector conLB, Rvector conUB, Rvector W, Integer PhaseNum,
             bool checkForDuplicateCons)
{
   conTolerance = 1e-12;

   // Set member data
   A = AMatrix;
   numCons = A.GetNumRows();
   initialGuess = initGuess;
   SetDecisionVector(initialGuess);
   hessianMat = G;
   gradVec = d;
   conLowerBounds = conLB;
   conUpperBounds = conUB;
   SetConstraintTypes();
   activeIneqSet = W;
   Phase = PhaseNum;
   TestForLinearlyDependentCons(checkForDuplicateCons);

   // Calculate dimensions of various quantities
   numDecisionVars = decVec.GetSize();
   numEqCons = eqIdxs.GetSize();
   numIneqCons = ineqIdxs.GetSize();
   numCons = A.GetNumRows();
   numWorkingIneqCons = activeIneqSet.GetSize();
   numActiveCons = numWorkingIneqCons + numEqCons;
   TestForNonZeroHessian();
   ValidateProblemConfig();
   calculatedQR = false;
}

MinQP::~MinQP()
{}

//------------------------------------------------------------------------------
// void SetWriteOutput(bool flag)
//------------------------------------------------------------------------------
/**
* Sets whether to write output lines
*
* @param flag Boolian flag to set WriteOuput with, true writes output while
*        false does not
*/
//------------------------------------------------------------------------------
void MinQP::SetWriteOutput(bool flag)
{
   WriteOutput = flag;
}

//------------------------------------------------------------------------------
// Rvector GetActiveSet()
//------------------------------------------------------------------------------
/**
* Returns the current active inequality set
*
* @return activeIneqSet Active inequality set as an Rvector
*/
//------------------------------------------------------------------------------
Rvector MinQP::GetActiveSet()
{
   return activeIneqSet;
}

//------------------------------------------------------------------------------
// void Optimize(Rvector &dV, Real &costVal, Rvector &lagMult,
//               Integer &exitFlag, Integer &numIter, Rvector &activeCI)
//------------------------------------------------------------------------------
/**
* Solves the QP Optimization problem
*
* @param dV The resulting decision vector
* @param costVal The cost value at the current decision vector
* @param lagMult The Lagrange multipliers
* @param exitFlag Flag indicating whether a solution successfully converged or
*        if a solution could not be achieved
* @param numIter The current number of iterations used in the calculation
* @param activeCI The active constraint indices
*/
//------------------------------------------------------------------------------
void MinQP::Optimize(Rvector &dV, Real &costVal, Rvector &lagMult,
                     Integer &exitFlag, Integer &numIter, Rvector &activeCI)
{
   Integer flag;

   // ----- Check that the user inputs are consistent
   if (Phase == 2)
   {
      flag = ValidateProblemConfig();
      if (flag == 0)
      {
         exitFlag = 0;
         PrepareOutput(dV, costVal, numIter, activeCI, exitFlag);
         MessageInterface::ShowMessage("Optimization did not succeed.  There "
            "are errors in the problem statement.");
         return;
      }
   }

   // ----- Scale the constraints and cost
   Rvector normA;
   Real fac;
   Rvector currentRow(A.GetNumColumns());

   if (Phase == 2)
   {
      normA.SetSize(numCons);
      for (Integer i = 0; i < numCons; ++i)
      {
         currentRow = A.GetRow(i);
         fac = currentRow.Norm();
         if (fac > eps)
         {
            for (Integer j = 0; j < A.GetNumColumns(); ++j)
               A(i, j) = A(i, j) / fac;
            conLowerBounds[i] = conLowerBounds[i] / fac;
            conUpperBounds[i] = conUpperBounds[i] / fac;
            normA[i] = fac;
         }
         else
            normA[i] = 1;
      }
      fac = ComputeHessianScaleFactor();
      if (fac > eps)
      {
         hessianMat = hessianMat / fac;
         gradVec = gradVec / fac;
      }
   }
   else
   {
      fac = 1;
      normA.SetSize(numCons);
      for (Integer i = 0; i < normA.GetSize(); ++i)
         normA[i] = 1;
   }
   // Remove inactive inequality constraints from the working set guess
   Rvector conViolation;
   Rvector removeIds(0);
   Integer numToRemove;
   Integer conIdx;
   bool hasConIdx = false;
   if (Phase == 2 && numWorkingIneqCons > 0)
   {
      conViolation = ComputeAllConViolations();
      numToRemove = 0;
      for (Integer loopIdx = 0; loopIdx < activeIneqSet.GetSize(); ++loopIdx)
      {
         conIdx = (Integer)activeIneqSet[loopIdx];
         for (Integer i = 0; i < ineqIdxs.GetSize(); ++i)
         {
            if (ineqIdxs[i] == conIdx)
               hasConIdx = true;
         }
         if (hasConIdx && std::abs(conViolation[conIdx]) > 10 * eps)
         {
            numToRemove++;
            removeIds.Resize(numToRemove);
            removeIds[numToRemove - 1] = loopIdx;
            hasConIdx = false;
         }
      }
      Rvector updateActiveIneqSet = activeIneqSet;
      activeIneqSet.SetSize(updateActiveIneqSet.GetSize() - numToRemove);
      bool removeElement;
      Integer iterator = 0;
      for (Integer i = 0; i < updateActiveIneqSet.GetSize(); ++i)
      {
         removeElement = false;
         for (Integer removeIdPos = 0; removeIdPos < removeIds.GetSize();
              ++removeIdPos)
         {
            if (i == removeIds[removeIdPos])
               removeElement = true;
         }
         if (!removeElement)
         {
            activeIneqSet[iterator] = updateActiveIneqSet[i];
            iterator++;
         }
      }
      numWorkingIneqCons = activeIneqSet.GetSize();
      numActiveCons = numWorkingIneqCons + numEqCons;
   }

   // ----- If we're in Phase 2 check to see if the initial guess is feasible.
   //       If the initial guess is not feasible, modify the intial guess to
   //       find a feasible point, using the standard Phase I method.
   Integer feasible;
   Real maxConViolation;
   Real minConViolation;
   Integer index;

   if (numEqCons + numIneqCons > 0 && Phase == 2)
   {
      // Determine if any of the constraints are violated for initial guess.
      conViolation = ComputeAllConViolations();

      feasible = 1;
      Rvector absConViolation = conViolation;
      for (Integer i = 0; i < absConViolation.GetSize(); ++i)
         absConViolation[i] = std::abs(absConViolation[i]);
      if (numEqCons > 0)
      {
         maxConViolation = GetMax(absConViolation);
         if (maxConViolation > conTolerance)
            feasible = 0;
      }
      if (numIneqCons > 0)
      {
         minConViolation = GetMin(conViolation);
         if (minConViolation < -conTolerance)
            feasible = 0;
      }

      // If the intial guess is infeasible, perform the Phase I optimization.
      if (feasible == 0)
      {
         //  Create initial guess for active set and solve the Phase I
         //  problem.  We reformulate the problem by adding another variable
         //  that is the a measure of the constraint violation.  The problem
         //  sets the Hessian equal to zero, and the gradient is defined
         //  so that the only contribution to the cost function, is the
         //  constraint violations.  When minimized, the cost function should
         //  be zero, or there is not a feasible solution.
         Rvector initialGuess_I;
         Rmatrix A_I;
         Rmatrix G_I;
         Rvector d_I;
         Rvector b_L;
         Rvector b_U;
         SetUpPhaseI(decVec, initialGuess_I, A_I, G_I, d_I, b_L, b_U);

         MinQP phase1QP(initialGuess_I, G_I, d_I, A_I, b_L, b_U,
                        activeIneqSet, 1, false);

         //phase1QP.WriteOutput = false;
         #ifdef WRITE_DEBUG
            phase1QP.SetWriteOutput(true);
            MessageInterface::ShowMessage("\nEntering QP Phase 1 Feasibilization Attempt\n");
         #endif
         //TODO: Clean up set get for W
         Rvector guessDecVec;
         Rvector lagMult;

         //Create unused values to fill in required Optimize fields
         Real emptyq;
         Integer emptyIter;
         Rvector emptyActiveIS;

         phase1QP.Optimize(guessDecVec, emptyq, lagMult, flag, emptyIter,
                           emptyActiveIS);

         #ifdef WRITE_DEBUG
            if (flag != 1)
               MessageInterface::ShowMessage("QP Phase 1 Feasibilization FAILED\n");
            else
               MessageInterface::ShowMessage("QP Phase 1 Feasibilization SUCCEDED\n");
         #endif

         activeIneqSet.SetSize(phase1QP.activeIneqSet.GetSize());
         activeIneqSet = phase1QP.activeIneqSet;

         // Extract data from the Phase I solution.  First check to see
         // if a feasible solution was found.

         Rvector decisionVector(numDecisionVars);
         for (Integer i = 0; i < decisionVector.GetSize(); ++i)
            decisionVector[i] = guessDecVec[i];
         SetDecisionVector(decisionVector);
         Real error = guessDecVec[numDecisionVars];
         Rvector errorLowerBound = multiMatrixToColumn(A, decisionVector) -
            conLowerBounds;

         Rvector errorUpperBound = multiMatrixToColumn(A, decisionVector) -
            conUpperBounds;
         Rvector errorLowerBoundIneq(ineqIdxs.GetSize());
         Rvector errorUpperBoundIneq(ineqIdxs.GetSize());
         index = 0;
         for (Integer i = 0; i < errorLowerBound.GetSize(); ++i)
         {
            for (Integer j = 0; j < ineqIdxs.GetSize(); ++j)
            {
               if (i == ineqIdxs[j])
               {
                  errorLowerBoundIneq[index] = errorLowerBound[i];
                  errorUpperBoundIneq[index] = errorUpperBound[i];
                  index++;
               }
            }
         }
         activeIneqSet.SetSize(0);
         inactiveIneqSet.SetSize(0);
         Rvector activeIneqSetHolder(errorLowerBoundIneq.GetSize());
         Rvector inactiveIneqSetHolder(errorLowerBoundIneq.GetSize());
         Integer activeSize = 0;
         Integer inactiveSize = 0;
         index = 0;
         for (Integer i = 0; i < errorLowerBoundIneq.GetSize(); ++i)
         {
            activeIneqSetHolder[index] = -1;
            inactiveIneqSetHolder[index] = -1;
            if (std::abs(errorLowerBoundIneq[i]) < 10 * eps ||
                std::abs(errorUpperBoundIneq[i]) < 10 * eps)
            {
               activeIneqSetHolder[index] = i;
               activeSize++;
            }
            else
            {
               inactiveIneqSetHolder[index] = i;
               inactiveSize++;
            }
            index++;
         }
         activeIneqSet.SetSize(activeSize);
         inactiveIneqSet.SetSize(inactiveSize);
         Integer activeIndex = 0;
         Integer inactiveIndex = 0;
         for (Integer i = 0; i < errorLowerBoundIneq.GetSize(); ++i)
         {
            if (activeIneqSetHolder[i] != -1)
            {
               activeIneqSet[activeIndex] = activeIneqSetHolder[i];
               activeIndex++;
            }
            if (inactiveIneqSetHolder[i] != -1)
            {
               inactiveIneqSet[inactiveIndex] = inactiveIneqSetHolder[i];
               inactiveIndex++;
            }
         }
         for (Integer i = 0; i < activeIneqSet.GetSize(); ++i)
            activeIneqSet[i] = ineqIdxs[(Integer)activeIneqSet[i]];
         if (error >= 1e-4)
         {
            if (WriteOutput == 1)
            {
               MessageInterface::ShowMessage("Error in minQP:  Problem "
                  "appears infeasible\n");
            }
            Rvector lagMultCopy = lagMult;
            lagMult.SetSize(numCons);
            for (Integer i = 0; i < numCons; ++i)
               lagMult[i] = lagMultCopy[i];
            exitFlag = -1;
            PrepareOutput(dV, costVal, numIter, activeCI, exitFlag);
            return;
         }
         Rvector activeIneqSetCopy = activeIneqSet;
         activeSize = activeIneqSet.GetSize();
         for (Integer i = 0; i < activeIneqSet.GetSize(); ++i)
         {
            if (activeIneqSet[i] == numIneqCons)
            {
               //activeIneqSetCopy[i] = 0.1;
               activeSize--;
               break;
            }
         }
         activeIneqSet.SetSize(activeSize);
         index = 0;
         if (activeSize != activeIneqSetCopy.GetSize())
         {
            for (Integer i = 0; i < activeSize; ++i)
            {
               activeIneqSet[i] = activeIneqSetCopy[i + 1];
               index++;
            }
         }
         else
            activeIneqSet = activeIneqSetCopy;
         numWorkingIneqCons = activeIneqSet.GetSize();
         numActiveCons = numEqCons + numWorkingIneqCons;
      }
   }

   // ----- Preparations to begin iteration.  Initialize counters and arrays
   //       determine the non-active set of inequality constraints.
   costVal = (0.5*multiRowToMatrix(hessianMat, decVec)*decVec + decVec*gradVec)*fac;
   numIterations = 0;
   Integer isConverged = 0;
   Real alpha = 0;
   lagMult.SetSize(0);
   inactiveIneqSet.SetSize(ineqIdxs.GetSize());
   inactiveIneqSet = ineqIdxs;
   Rvector inactiveIneqSetCopy = inactiveIneqSet;
   Integer inactiveSize = inactiveIneqSet.GetSize();
   for (Integer i = 0; i < activeIneqSet.GetSize(); ++i)
   {
      for (Integer j = 0; j < inactiveIneqSet.GetSize(); ++j)
      {
         if (inactiveIneqSet[j] == activeIneqSet[i])
         {
            inactiveIneqSetCopy[j] = 0.1;
            inactiveSize--;
         }
      }
   }

   index = 0;
   inactiveIneqSet.SetSize(inactiveSize);
   for (Integer i = 0; i < inactiveIneqSetCopy.GetSize(); ++i)
   {
      if (inactiveIneqSetCopy[i] != 0.1)
      {
         inactiveIneqSet[index] = inactiveIneqSetCopy[i];
         index++;
      }
   }
   Rmatrix partA(eqIdxs.GetSize() + activeIneqSet.GetSize(), A.GetNumColumns());
   for (Integer i = 0; i < partA.GetNumRows(); ++i)
   {
      for (Integer j = 0; j < partA.GetNumColumns(); ++j)
      {
         if (i < eqIdxs.GetSize())
            partA(i, j) = A((Integer)eqIdxs[i], j);
         else
            partA(i, j) = A((Integer)activeIneqSet[i - eqIdxs.GetSize()], j);
      }
   }
   Rmatrix Q(partA.GetNumColumns(), partA.GetNumColumns());
   Rmatrix R(partA.GetNumColumns(), partA.GetNumRows());
   qr.Factor(partA.Transpose(), R, Q);

   #ifdef WRITE_DEBUG
      MessageInterface::ShowMessage("==== Working Sets After Call to Phase 1 ====\n");
      MessageInterface::ShowMessage("Working Set\n");
      for (Integer i = 0; i < activeIneqSet.GetSize(); ++i)
         MessageInterface::ShowMessage("%.7f   ", activeIneqSet[i]);
      MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("Inactive Set\n");
      for (Integer i = 0; i < inactiveIneqSet.GetSize(); ++i)
         MessageInterface::ShowMessage("%.7f   ", inactiveIneqSet[i]);
      MessageInterface::ShowMessage("\n");

      if (Phase == 2)
      {
         MessageInterface::ShowMessage("\n");
         MessageInterface::ShowMessage("Entering QP Phase 2 Optimization Attempt\n");
      }
   #endif

   // ----- Write output headers for data, and write data for initial guess
   
   if (WriteOutput == 1)
   {
      MessageInterface::ShowMessage("\n                                       "
         "           Max        Min\n Iteration        f(x)         Step-size "
         "        Lambda     Violation       Action \n      %i              "
         "%.6f                 0\n", numIterations, costValue);
   }

   Integer MaxIter;
   if (numDecisionVars > numCons - numEqCons)
      MaxIter = 10 * numDecisionVars;
   else
      MaxIter = 10 * (numCons - numEqCons);

   while (isConverged == 0)
   {
      // Loop updates
      numIterations++;
      // The quadratic cost function
      costValue = (0.5*multiRowToMatrix(hessianMat, decVec)*decVec + decVec*gradVec)*
         fac;
      // First derivative of q w/r/t x
      Rvector g = multiMatrixToColumn(hessianMat, decVec) + gradVec;
      // Initialize lagMult to empty
      lagMult.SetSize(0);

      // ---- - Calculate the step size
      //       If the matrix of active constraints is not empty then use the
      //       null space method to calculate the step size.Otherwise, use
      //       the a Newton step.
      // Solve the following system :
      //        [G - A^T][p] = [g]
      //        [A   0][lagMult] = [h]
      // where
      //        h = A*x - b(for active set)
      //        g = G*x + d

      //LUFactorization *lu = new LUFactorization();
      Rvector p;
      std::string stepType;

      if (numActiveCons < numDecisionVars && numActiveCons > 0)
      {
         // -----  Calculate the step using the null space method
         Rmatrix Z(Q.GetNumRows(), numDecisionVars - (numActiveCons + 1) + 1);
         Integer colZ = 0;
         for (Integer i = 0; i < Z.GetNumRows(); ++i)
         {
            for (Integer j = numActiveCons; j < numDecisionVars; ++j)
            {
               Z(i, colZ) = Q(i, j);
               colZ++;
            }
            colZ = 0;
         }
         //Rvector h = ComputeActiveConViolations();
         p.SetSize(Z.GetNumRows());
         
         Rmatrix ZTrans = Z.Transpose();

         if (Phase == 2)
         {
            stepType = "Null Space Phase 2";
            try
            {
               Rmatrix Y(Q.GetNumRows(), numActiveCons);
               for (Integer i = 0; i < Y.GetNumRows(); ++i)
               {
                  for (Integer j = 0; j < Y.GetNumColumns(); ++j)
                     Y(i, j) = Q(i, j);
               }
               Rvector h = ComputeActiveConViolations();

               Rmatrix partAY = multiMatrix(partA, Y);
               Rmatrix L(partAY.GetNumRows(), partAY.GetNumColumns());
               Rmatrix U(partAY.GetNumRows(), partAY.GetNumColumns());
               lu.Factor(partAY, L, U);
               Rvector x(h.GetSize());  // Intermediate column vector for -U\(L\h)
               lu.SolveSystem(L, h, x);
               Rvector py(h.GetSize());
               lu.SolveSystem(-U, x, py);
               Rmatrix L2(Z.GetNumColumns(), Z.GetNumColumns());
               Rmatrix U2(Z.GetNumColumns(), Z.GetNumColumns());
               Rmatrix ZTransTimesHessian = multiMatrix(ZTrans, hessianMat);
               lu.Factor(multiMatrix(ZTransTimesHessian, Z), L2, U2);
               x.SetSize(Z.GetNumColumns());
               lu.SolveSystem(L2, multiMatrixToColumn(ZTransTimesHessian,
                  multiMatrixToColumn(Y, py)) +
                  multiMatrixToColumn(ZTrans, g), x);
               Rvector pz(x.GetSize());
               lu.SolveSystem(-U2, x, pz);
               p = multiMatrixToColumn(Y, py) + multiMatrixToColumn(Z, pz);

               for (Integer i = 0; i < pz.GetSize(); ++i)
               {
                  if (GmatMathUtil::IsNaN(pz[i]))
                  {
                     exitFlag = -4;
                     PrepareOutput(dV, costVal, numIter, activeCI, exitFlag);
                     return;
                  }
               }
            }
            catch (const std::exception &e)
            {
               exitFlag = -4;
               PrepareOutput(dV, costVal, numIter, activeCI, exitFlag);
               return;
            }
         }
         else
         {
            p = multiMatrixToColumn(-Z, multiMatrixToColumn(ZTrans, g));
            stepType = "Null Space Phase 1";
         }
      }

      else
      {
         if (numActiveCons == numDecisionVars)
         {
            // -----  Null space is empty because there are no free variables.
            Rmatrix Anear(numEqCons + numWorkingIneqCons, numDecisionVars);
            Rvector bnear(decVec.GetSize());
            stepType = "No Free Variables";
            ComputeNearbyFeasibleLinearSystem(decVec, Anear, bnear);
            Rvector xnear(bnear.GetSize());
            try
            {
               bool usePivot = false;
               for (Integer i = 0; i < Anear.GetNumRows(); ++i)
               {
                  if (Anear(i, i) == 0)
                     usePivot = true;
               }

               if (usePivot)
                  luPivot.SolveSystem(Anear, bnear, xnear);
               else
                  lu.SolveSystem(Anear, bnear, xnear);
               p.SetSize(decVec.GetSize());
               p = xnear - decVec;
            }
            catch (const std::exception &e)
            {
               exitFlag = -3;
               PrepareOutput(dV, costVal, numIter, activeCI, exitFlag);
               return;
            }
         }

         else
         {
            // -----  No constraints, Hessian is NOT zero, so use the Newton 
            //        step
            if (!isHessianZero)
            {
               Rmatrix Rchol(hessianMat.GetNumRows(),
                             hessianMat.GetNumColumns());
               Rmatrix blankMatrix; // Unused matrix
               CholeskyFactorization *chol = new CholeskyFactorization();
               chol->Factor(hessianMat, Rchol, blankMatrix);
               Rvector xchol(g.GetSize());
               lu.SolveSystem(Rchol.Transpose(), g, xchol);
               p.SetSize(g.GetSize());
               lu.SolveSystem(Rchol, xchol, p);
               p = -p;
               stepType = "Newton";
            }
            else
            {
               // -----  No constraints,  Hessian is zero, so use steepest
               //        descent
               p.SetSize(g.GetSize());
               p = -g;
               stepType = "Steepest Descent";
            }
         }
      }
      #ifdef WRITE_DEBUG
         MessageInterface::ShowMessage("\n==== Computed Step Size and Method ====\n");
         MessageInterface::ShowMessage("Step Type : " + stepType + "\n");
         for (Integer i = 0; i < p.GetSize(); ++i)
            MessageInterface::ShowMessage("%.7f   ", p[i]);
         MessageInterface::ShowMessage("\n");
      #endif

      // ----- Check to see if we found a solution
      Integer j = 0;
      std::string action;
      Real minLambda;

      if (p.GetMagnitude() <= 1e-7 || numIterations >= MaxIter)
      {
         // Calculate the Lagrange multipliers of the active inequality
         // constraints.  If the matrix is empty then set lagMult to emtpy.
         
         if (!calculatedQR)
         {
            partA.SetSize(eqIdxs.GetSize() + activeIneqSet.GetSize(),
               A.GetNumColumns());
            for (Integer i = 0; i < partA.GetNumRows(); ++i)
            {
               for (Integer j = 0; j < partA.GetNumColumns(); ++j)
               {
                  if (i < eqIdxs.GetSize())
                     partA(i, j) = A((Integer)eqIdxs[i], j);
                  else
                  {
                     partA(i, j) =
                        A((Integer)activeIneqSet[i - eqIdxs.GetSize()], j);
                  }
               }
            }

            Q.SetSize(partA.GetNumColumns(), partA.GetNumColumns());
            R.SetSize(partA.GetNumColumns(), partA.GetNumRows());
            qr.Factor(partA.Transpose(), R, Q);
         }

         calculatedQR = false;
         lagMult.SetSize(numCons);
         for (Integer lagMultIdx = 0; lagMultIdx < numCons; ++lagMultIdx)
            lagMult[lagMultIdx] = 0;
         Rvector lagMultW(0);
         if (numActiveCons > 0)
         {
            // Calculate the Lagrange mulipliers and undo the scaling.
            Rvector lagMultAct(R.GetNumColumns());
            try
            {
               lu.SolveSystem(R, multiMatrixToColumn(Q.Transpose(), g)*fac,
                  lagMultAct);
            }
            catch (const std::exception &e)
            {
               exitFlag = -5;
               PrepareOutput(dV, costVal, numIter, activeCI, exitFlag);
               return;
            }

            for (Integer i = 0; i < lagMultAct.GetSize(); ++i)
            {
               if (i < eqIdxs.GetSize())
               {
                  lagMultAct[i] = lagMultAct[i] / normA[(Integer)eqIdxs[i]];
                  lagMult[(Integer)eqIdxs[i]] = lagMultAct[i];
               }
               else
               {
                  lagMultAct[i] = lagMultAct[i] /
                     normA[(Integer)activeIneqSet[i - eqIdxs.GetSize()]];
                  lagMult[(Integer)activeIneqSet[i - eqIdxs.GetSize()]] = 
                     lagMultAct[i];
               }
            }

            lagMultW.SetSize(numWorkingIneqCons);
            for (Integer i = 0; i < lagMultW.GetSize(); ++i)
               lagMultW[i] = lagMultAct(i + numEqCons);
            Rvector error;
            Rmatrix activeIneqSetFromA(activeIneqSet.GetSize(),
                                       A.GetNumColumns());
            for (Integer i = 0; i < activeIneqSet.GetSize(); ++i)
            {
               for (Integer j = 0; j < A.GetNumColumns(); ++j)
               {
                  activeIneqSetFromA(i, j) = A((Integer)activeIneqSet[i], j);
               }
            }
            error = multiMatrixToColumn(activeIneqSetFromA, decVec);
            for (Integer i = 0; i < error.GetSize(); ++i)
               error[i] -= conUpperBounds[(Integer)activeIneqSet[i]];
            // if active at upper bound, reverse the sign of
            // lagrange multipler
            for (Integer i = 0; i < error.GetSize(); ++i)
            {
               if (conType[i] == 1 && std::abs(error[i]) <= 10 * eps)
                  lagMultW[i] = -lagMultW[i];
            }
         }

         #ifdef WRITE_DEBUG
            MessageInterface::ShowMessage("\n==== Variables After Updating "
               "Lagrange Multipliers ====\n");
            MessageInterface::ShowMessage("Working Set\n");
            for (Integer i = 0; i < activeIneqSet.GetSize(); ++i)
               MessageInterface::ShowMessage("%.7f   ", activeIneqSet[i]);
            MessageInterface::ShowMessage("\nInactive Set\n");
            for (Integer i = 0; i < inactiveIneqSet.GetSize(); ++i)
               MessageInterface::ShowMessage("%.7f   ", inactiveIneqSet[i]);
            MessageInterface::ShowMessage("\nWorking Set Lagrange Multipliers\n");
            for (Integer i = 0; i < lagMultW.GetSize(); ++i)
               MessageInterface::ShowMessage("%.7f   ", lagMultW[i]);
            MessageInterface::ShowMessage("\n");
         #endif

         // Check to see if there are any negative multipliers
         if (numWorkingIneqCons > 0)
         {
            minLambda = GetMin(lagMultW);
            for (Integer i = 0; i < lagMultW.GetSize(); ++i)
            {
               if (lagMultW[i] == minLambda)
               {
                  j = i;
                  break;
               }
            }
         }
         else
            minLambda = 0;
         if (lagMult.GetSize() == 0 || minLambda >= -1e-10 || numIterations >= MaxIter)
         {
            // We found the solution
            if (lagMult.GetSize() == 0 || minLambda >= -1e-10)
            {
               exitFlag = 1;
               action = "      Stop";
            }
            else
            {
               exitFlag = -2;
               action = "  Max. Iterations";
            }
            PrepareOutput(dV, costVal, numIter, activeCI, exitFlag);

            if (WriteOutput == 1)
            {
               MessageInterface::ShowMessage("      %i               %.6f"
                  "                 %.6f               %.6f             %s\n",
                  numIterations, costVal, alpha, minLambda, action.c_str());
            }
            return;
         }
         else
         {
            // Remove the constraint with the most negative multiplier
            // [Q,R]  = qrdelete(Q,R,mE + j);
            Rvector inactiveIneqSetCopy = inactiveIneqSet;
            inactiveIneqSet.SetSize(inactiveIneqSetCopy.GetSize() +
               activeIneqSet.GetSize());
            for (Integer i = 0; i < inactiveIneqSet.GetSize(); ++i)
            {
               if (i < inactiveIneqSetCopy.GetSize())
                  inactiveIneqSet[i] = inactiveIneqSetCopy[i];
               else
               {
                  inactiveIneqSet[i] = 
                     activeIneqSet[i - inactiveIneqSetCopy.GetSize()];
               }
            }
            action = "      Remove Constraint " +
               std::to_string(Integer(activeIneqSet[j]) + 1);
            Rvector activeIneqSetCopy = activeIneqSet;
            activeIneqSet.SetSize(activeIneqSet.GetSize() - 1);
            for (Integer i = 0; i < activeIneqSet.GetSize(); ++i)
            {
               if (i < j)
                  activeIneqSet[i] = activeIneqSetCopy[i];
               else
               {
                  if (i >= j)
                     activeIneqSet[i] = activeIneqSetCopy[i + 1];
               }
            }
            numWorkingIneqCons--;
            numActiveCons--;

            
            if (activeIneqSetCopy.GetSize() - activeIneqSet.GetSize() == 1 && partA.GetNumRows() - 1 != 0)
            {
               partA.SetSize(eqIdxs.GetSize() + activeIneqSet.GetSize(),
                  A.GetNumColumns());
               for (Integer i = 0; i < partA.GetNumRows(); ++i)
               {
                  for (Integer j = 0; j < partA.GetNumColumns(); ++j)
                  {
                     if (i < eqIdxs.GetSize())
                        partA(i, j) = A((Integer)eqIdxs[i], j);
                     else
                     {
                        partA(i, j) =
                           A((Integer)activeIneqSet[i - eqIdxs.GetSize()], j);
                     }
                  }
               }


               Rmatrix QCopy = Q;
               Rmatrix RCopy = R;
               qr.RemoveFromQR(RCopy, QCopy, "col", eqIdxs.GetSize() + j, R, Q);
            }
            

            else
            {
               partA.SetSize(eqIdxs.GetSize() + activeIneqSet.GetSize(),
                  A.GetNumColumns());
               for (Integer i = 0; i < partA.GetNumRows(); ++i)
               {
                  for (Integer j = 0; j < partA.GetNumColumns(); ++j)
                  {
                     if (i < eqIdxs.GetSize())
                        partA(i, j) = A((Integer)eqIdxs[i], j);
                     else
                     {
                        partA(i, j) =
                           A((Integer)activeIneqSet[i - eqIdxs.GetSize()], j);
                     }
                  }
               }
               Q.SetSize(partA.GetNumColumns(), partA.GetNumColumns());
               R.SetSize(partA.GetNumColumns(), partA.GetNumRows());
               qr.Factor(partA.Transpose(), R, Q);
            }

            calculatedQR = true;

            #ifdef WRITE_DEBUG
               MessageInterface::ShowMessage("\n==== Variables After Removing constraint with"
                  " most negative multiplier ====\n");
               MessageInterface::ShowMessage("Working Set\n");
               for (Integer i = 0; i < activeIneqSet.GetSize(); ++i)
                  MessageInterface::ShowMessage("%.7f   ", activeIneqSet[i]);
               MessageInterface::ShowMessage("\nInactive Set\n");
               for (Integer i = 0; i < inactiveIneqSet.GetSize(); ++i)
                  MessageInterface::ShowMessage("%.7f   ", inactiveIneqSet[i]);
               MessageInterface::ShowMessage("\nWorking Set Lagrange Multipliers\n");
               for (Integer i = 0; i < lagMultW.GetSize(); ++i)
                  MessageInterface::ShowMessage("%.7f   ", lagMultW[i]);
               MessageInterface::ShowMessage("\n");
            #endif
         }
      }

      // ----- Calculate the step length
      else
      {
         //  Find distance to inactive constraints in the search direction.
         //  Begin by finding the change in the inactive inequality constraints.
         //  If the change is zero(to some tolerance) or positive, then
         //  there is no way the constraint can become active with the proposed
         //  step so we don't need to consider it further.
         Rvector distanceToInactiveCons(0);
         Rvector inactiveConIdxs(0);
         if (inactiveIneqSet.GetSize() != 0)
         {
            ComputeDistanceToInactiveCons(p, distanceToInactiveCons,
               inactiveConIdxs);
         }

         // Calculate step and if a new constraint is active, add it to
         // the working set, W
         Real mindist = 100;
         if (distanceToInactiveCons.GetSize() != 0)
         {
            mindist = GetMin(distanceToInactiveCons);
            for (Integer i = 0; i < distanceToInactiveCons.GetSize(); ++i)
            {
               if (distanceToInactiveCons[i] == mindist)
               {
                  j = i;
                  break;
               }
            }
         }

         // Pick the step size
         Integer hitCon;
         if (Phase == 2)
         {
            if (mindist <= 1 + eps)
            {
               alpha = mindist;
               hitCon = 1;
            }
            else
            {
               alpha = 1;
               hitCon = 0;
            }
         }
         else
         {
            alpha = mindist;
            hitCon = 1;
         }

         #ifdef WRITE_DEBUG
            MessageInterface::ShowMessage("Distance To Constraints\n");
            for (Integer i = 0; i < distanceToInactiveCons.GetSize(); ++i)
               MessageInterface::ShowMessage("%.7f   ", distanceToInactiveCons[i]);
            MessageInterface::ShowMessage("\n\n==== Computed step size an "
               "minimum distance ====\n");
            MessageInterface::ShowMessage("Minimum distance to constraint\n");
            MessageInterface::ShowMessage("%.7f\n", mindist);
            MessageInterface::ShowMessage("Selected Step Size\n");
            MessageInterface::ShowMessage("%.7f\n", alpha);
         #endif

         SetDecisionVector(decVec + alpha*p);

         // If we hit a contraint, add it to the active set vector
         if (hitCon)
         {
            inactiveIneqSetCopy.SetSize(inactiveIneqSet.GetSize());
            inactiveIneqSetCopy = inactiveIneqSet;
            Integer ind = 0;
            for (Integer i = 0; i < inactiveIneqSet.GetSize(); ++i)
            {
               if (inactiveIneqSet[i] == inactiveConIdxs[j])
                  ind++;
            }

            inactiveIneqSet.SetSize(inactiveIneqSetCopy.GetSize() - ind);
            Integer numOfDeletions = 0;
            for (Integer i = 0; i < inactiveIneqSetCopy.GetSize(); ++i)
            {
               if (inactiveIneqSetCopy[i] != inactiveConIdxs[j])
                  inactiveIneqSet[i - numOfDeletions] = inactiveIneqSetCopy[i];
               else
                  numOfDeletions++;
            }

            Rvector activeIneqSetCopy = activeIneqSet;
            activeIneqSet.SetSize(activeIneqSetCopy.GetSize() + 1);

            Rvector newColumn(A.GetNumColumns());
            Integer newColIndex = 0;

            if (inactiveConIdxs.GetSize() == 0)
            {
               exitFlag = -6;
               PrepareOutput(dV, costVal, numIter, activeCI, exitFlag);
               return;
            }

            for (Integer i = 0; i < activeIneqSet.GetSize(); ++i)
            {
               if (i < activeIneqSetCopy.GetSize())
                  activeIneqSet[i] = activeIneqSetCopy[i];
               else
                  activeIneqSet[i] = inactiveConIdxs[j];
            }
            numActiveCons++;
            numWorkingIneqCons++;
            partA.SetSize(eqIdxs.GetSize() + activeIneqSet.GetSize(),
               A.GetNumColumns());
            for (Integer i = 0; i < partA.GetNumRows(); ++i)
            {
               for (Integer j = 0; j < partA.GetNumColumns(); ++j)
               {
                  if (i < eqIdxs.GetSize())
                     partA(i, j) = A((Integer)eqIdxs[i], j);
                  else
                  {
                     if (i == partA.GetNumRows() - 1)
                     {
                        newColumn[newColIndex] = A((Integer)activeIneqSet[i - eqIdxs.GetSize()], j);
                        newColIndex++;
                     }
                     partA(i, j) =
                        A((Integer)activeIneqSet[i - eqIdxs.GetSize()], j);
                  }
               }
            }

            if (R.GetNumColumns() != 0 && R.GetNumRows() != 0)
            {
               Rmatrix QCopy = Q;
               Rmatrix RCopy = R;
               qr.AddToQR(RCopy, QCopy, "col", partA.GetNumRows() - 1, newColumn, R, Q);
            }

            else
            {
               Q.SetSize(partA.GetNumColumns(), partA.GetNumColumns());
               R.SetSize(partA.GetNumColumns(), partA.GetNumRows());
               qr.Factor(partA.Transpose(), R, Q);
            }

            calculatedQR = true;

            action = "      Step " + std::to_string(alpha);
            action += " and Add Constraint " +
               std::to_string(Integer(inactiveConIdxs[j] + 1));
         }
         else
            action = "      Full Step";
      }

      // Output iterate data
      costVal = (0.5*multiRowToMatrix(hessianMat.Transpose(), decVec)*decVec +
         decVec*gradVec)*fac;
      if (lagMult.GetSize() != 0)
         minLambda = GetMin(lagMult);
      else
         minLambda = 0;

      bool skipViolationPrint = false;
      Real maxActiveViolation;
      if (A.GetNumColumns() != 0 && (eqIdxs.GetSize() != 0 || activeIneqSet.GetSize() != 0))
      {
         Rvector absComputeActiveConViolations = ComputeActiveConViolations();
         for (Integer i = 0; i < absComputeActiveConViolations.GetSize(); ++i)
         {
            absComputeActiveConViolations[i] =
               std::abs(absComputeActiveConViolations[i]);
         }
         maxActiveViolation = GetMax(absComputeActiveConViolations);
      }
      else
      {
         skipViolationPrint = true;
      }

      if (WriteOutput == 1)
      {
         if (!skipViolationPrint)
         {
            MessageInterface::ShowMessage("      %i               %.6f"
               "                 %.6f               %.6f               %.6f"
               "             %s\n", numIterations, costValue, alpha, minLambda,
               maxActiveViolation, stepType.c_str());
         }
         else
         {
            MessageInterface::ShowMessage("      %i               %.6f"
               "                 %.6f               %.6f"
               "             %s\n", numIterations, costValue, alpha, minLambda,
               maxActiveViolation, stepType.c_str());
         }
      }
   }
}

//------------------------------------------------------------------------------
// Rmatrix GetModifiedCons()
//------------------------------------------------------------------------------
/**
* Returns a matrix containing which inequality constraints were combined into
* one constraint
*
* @return The combined constraints' indices
*/
//------------------------------------------------------------------------------
Rmatrix MinQP::GetModifiedCons()
{
   return combinedCons;
}

//------------------------------------------------------------------------------
// void PrepareOutput(Rvector &dV, Real &costVal, Integer &numIter,
//                    Rvector &activeEqs, Integer exitFlag);
//------------------------------------------------------------------------------
/**
* Method to prepare data to be output
*
* @param dV The resulting decision vector
* @param costVal The cost value at the current decision vector
* @param numIter The current number of iterations used in the calculation
* @param activeEqs The active constraint indices
* @param exitFlag Flag indicating whether a solution successfully converged or
*        if a solution could not be achieved
*/
//------------------------------------------------------------------------------
void MinQP::PrepareOutput(Rvector &dV, Real &costVal, Integer &numIter,
                          Rvector &activeEqs, Integer exitFlag)
{
   numIter = numIterations;
   if (exitFlag == 1)
   {
      costVal = costValue;
      activeEqs.SetSize(eqIdxs.GetSize() + activeIneqSet.GetSize());
      for (Integer i = 0; i < activeEqs.GetSize(); ++i)
      {
         if (i < eqIdxs.GetSize())
            activeEqs[i] = eqIdxs[i];
         else
            activeEqs[i] = activeIneqSet[i - eqIdxs.GetSize()];
      }
      dV.SetSize(decVec.GetSize());
      dV = decVec;
   }
   else if (exitFlag == 0)
   {
      dV.SetSize(0);
      costVal = GmatMathConstants::QUIET_NAN;
      activeEqs.SetSize(0);
   }
   else if (exitFlag == -1 || exitFlag == -2 || exitFlag == -3 ||
      exitFlag == -4 || exitFlag == -5 || exitFlag == -6)
   {
      costVal = costValue;
      activeEqs.SetSize(0);
      dV.SetSize(decVec.GetSize());
      dV = decVec;
   }
}


//------------------------------------------------------------------------------
// void TestForNonZeroHessian()
//------------------------------------------------------------------------------
/**
* Tests for nonzero hessian and sets isHessianZero accordingly
*
*/
//------------------------------------------------------------------------------
void MinQP::TestForNonZeroHessian()
{
   isHessianZero = true;
   for (Integer rowIdx = 0; rowIdx < numDecisionVars; ++rowIdx)
   {
      for (Integer colIdx = 0; colIdx < numDecisionVars; ++colIdx)
      {
         if (hessianMat(rowIdx, colIdx) != 0)
            isHessianZero = false;
      }
   }
}

//------------------------------------------------------------------------------
// Real ComputeHessianScaleFactor()
//------------------------------------------------------------------------------
/**
* Computes scale factor fro QP based on Inf norm of the Hessian.  Returns 1 if
* Hessian contains all zeros.  Must be called after TestForNonZeroHessian().
*
* @return scaleFac The scale factor to use on the Hessian matrix
*/
//------------------------------------------------------------------------------
Real MinQP::ComputeHessianScaleFactor()
{
   Real scaleFac;
   Real rowMag = 0;
   if (isHessianZero)
   {
      scaleFac = 1;
      return scaleFac;
   }
   scaleFac = 0;
   for (Integer rowIdx = 0; rowIdx < numDecisionVars; ++rowIdx)
   {
      for (Integer colIdx = 0; colIdx < numDecisionVars; ++colIdx)
      {
         rowMag += std::abs(hessianMat(rowIdx, colIdx));
      }
      if (rowMag > scaleFac)
         scaleFac = rowMag;
      rowMag = 0;
   }

   return scaleFac;
}

//------------------------------------------------------------------------------
// void SetConstraintTypes()
//------------------------------------------------------------------------------
/**
* Sets arrays eqIdxs and ineqIdxs and conType and numbers of members of the sets
*
*/
//------------------------------------------------------------------------------
void MinQP::SetConstraintTypes()
{
   eqIdxs.SetSize(0);
   ineqIdxs.SetSize(0);
   conType.SetSize(0);
   Integer numEqCon = -1;
   Integer numIneqCon = -1;

   for (Integer conIdx = 0; conIdx < numCons; ++conIdx)
   {
      if (conLowerBounds[conIdx] > conUpperBounds[conIdx])
      {
         std::string errMessage = "A lower constraint bound is larger than "
            "an upper constraint bound";
         throw UtilityException(errMessage);
      }
      else
      {
         if (std::abs(conLowerBounds[conIdx] - conUpperBounds[conIdx]) > eps)
         {
            numIneqCon++;
            ineqIdxs.Resize(numIneqCon + 1);
            ineqIdxs[numIneqCon] = conIdx;
            conType.Resize(conIdx + 1);
            conType[conIdx] = 1;
         }
         else
         {
            numEqCon++;
            eqIdxs.Resize(numEqCon + 1);
            eqIdxs(numEqCon) = conIdx;
            conType.Resize(conIdx + 1);
            conType[conIdx] = 2;
         }
      }
   }
}

//------------------------------------------------------------------------------
// void SetDecisionVector(Rvector dV)
//------------------------------------------------------------------------------
/**
* Sets decision vector and updates constraint values
*
* @param dV The decision vector decVec will be set to
*/
//------------------------------------------------------------------------------
void MinQP::SetDecisionVector(Rvector dV)
{
   decVec = dV;
   if (A.GetNumColumns() != 0 || A.GetNumRows() != 0)
      ComputeConstraints();
}

//------------------------------------------------------------------------------
// void ComputeConstraints()
//------------------------------------------------------------------------------
/**
* Computes the constraint values A*x
*
*/
//------------------------------------------------------------------------------
void MinQP::ComputeConstraints()
{
   conValues.SetSize(A.GetNumRows());
   conValues = multiMatrixToColumn(A, decVec);
}

//------------------------------------------------------------------------------
// Rvector ComputeAllConViolations()
//------------------------------------------------------------------------------
/**
* Computes constraint violations at the current value of x.
* Returns zero for element if conLowerBounds(i) <= A*x(i) <= conUpperBounds(i)
* Returns A*x(i) - conLowerBounds(i) for element  if A*x(i) < conLowerBounds(i)
* Returns -A*x(i) + conUpperBounds(i) for element  if A*x(i) > conUpperBounds(i)
*
* @return conViolations Vector of constraint violations corresponding to 
*         relationship between A*x(i) and the upper/lower bounds
*/
//------------------------------------------------------------------------------
Rvector MinQP::ComputeAllConViolations()
{
   Rvector conViolations(numCons);
   for (Integer conIdx = 0; conIdx < numCons; ++conIdx)
   {
      if (conType[conIdx] == 2)
         conViolations[conIdx] = conValues[conIdx] - conLowerBounds[conIdx];
      else
      {
         if (conType[conIdx] == 1)
         {
            if (conLowerBounds[conIdx] <= conValues[conIdx] &&
                conValues[conIdx] <= conUpperBounds[conIdx])
               conViolations[conIdx] = 0;
            else
            {
               if (conLowerBounds[conIdx] > conValues[conIdx])
               {
                  conViolations[conIdx] =
                     conValues[conIdx] - conLowerBounds[conIdx];
               }
               else
               {
                  conViolations[conIdx] =
                     -conValues[conIdx] + conUpperBounds[conIdx];
               }
            }
         }
      }
   }
   return conViolations;
}

//------------------------------------------------------------------------------
// Rvector ComputeActiveConViolations()
//------------------------------------------------------------------------------
/**
* Returns array of active constraint violations defined as violations
* for equality and working set
*
* @return conViolations Vector of active constriant violations
*/
//------------------------------------------------------------------------------
Rvector MinQP::ComputeActiveConViolations()
{
   Rvector allConViolations = ComputeAllConViolations();
   Rvector conViolations(eqIdxs.GetSize() + activeIneqSet.GetSize());
   for (Integer i = 0; i < conViolations.GetSize(); ++i)
   {
      if (i < eqIdxs.GetSize())
         conViolations[i] = allConViolations[(Integer)eqIdxs[i]];
      else
      {
         conViolations[i] =
            allConViolations[(Integer)activeIneqSet[i - eqIdxs.GetSize()]];
      }
   }
   return conViolations;
}

//------------------------------------------------------------------------------
// Rvector ComputeIneqConViolations()
//------------------------------------------------------------------------------
/**
* Returns array of equality constriant violations
*
* @return conViolations Vector of equality constriant violations
*/
//------------------------------------------------------------------------------
Rvector MinQP::ComputeIneqConViolations()
{
   Rvector allConViolations = ComputeAllConViolations();
   Rvector conViolations(ineqIdxs.GetSize());
   for (Integer i = 0; i < conViolations.GetSize(); ++i)
      conViolations[i] = allConViolations[(Integer)ineqIdxs[i]];
   return conViolations;
}

//------------------------------------------------------------------------------
// void ComputeDistanceToInactiveCons(Rvector stepVector,
//                                    Rvector &distanceToInactiveCons,
//                                    Rvector &inactiveConIdxs)
//------------------------------------------------------------------------------
/**
* Computes the distance from the current decVec to the inactive
* constraint bounds, scaled by the step size.The function
* checks the direction of the step, and computes the direction
* to the bound in the direction of the step.Note the distance
* is scaled by the step size to help in comparing the step size
* to each constraint boundary with the step size to thd cost
* function minimum in the search direction in the line search.
*
* @param stepVector The current calculated step size
* @param &distanceToInactiveCons The computed distance to the inactive 
*        constraints
* @param &inactiveConIdxs The indices of the inactive constraints
*/
//------------------------------------------------------------------------------
void MinQP::ComputeDistanceToInactiveCons(Rvector stepVector,
   Rvector &distanceToInactiveCons,
   Rvector &inactiveConIdxs)
{
   Rmatrix partA(inactiveIneqSet.GetSize(), A.GetNumColumns());
   for (Integer i = 0; i < partA.GetNumRows(); ++i)
   {
      for (Integer j = 0; j < partA.GetNumColumns(); ++j)
         partA(i, j) = A((Integer)inactiveIneqSet[i], j);
   }
   Rvector AtimesStep = multiMatrixToColumn(partA, stepVector);

   Integer index = 0;
   Integer conIdx = 0;
   Integer distanceIndex = 0;
   Integer inactiveConIdxsIndex = 0;
   for (Integer conCount = 0; conCount < inactiveIneqSet.GetSize(); ++conCount)
   {
      if (std::abs(AtimesStep[conCount]) > conTolerance)
      {
         conIdx = (Integer)inactiveIneqSet[conCount];
         // Change in constraint is in direction of lower bound
         Real inf = std::numeric_limits<double>::infinity();
         Real ninf = -inf;
         if (AtimesStep[conCount] < 0 && conLowerBounds[conIdx] > ninf)
         {
            distanceToInactiveCons.Resize(distanceIndex + 1);
            distanceToInactiveCons[distanceIndex] =
               (conLowerBounds[conIdx] - A.GetRow(conIdx)*decVec) /
               (A.GetRow(conIdx)*stepVector);
            distanceIndex++;
            inactiveConIdxs.Resize(inactiveConIdxsIndex + 1);
            inactiveConIdxs[inactiveConIdxsIndex] = conIdx;
            inactiveConIdxsIndex++;
         }
         else
         {
            if (AtimesStep[conCount] > 0 && conUpperBounds[conIdx] < inf)
            {
               // Change in constraint is in direction of upper bound
               distanceToInactiveCons.Resize(distanceIndex + 1);
               distanceToInactiveCons[distanceIndex] =
                  (conUpperBounds[conIdx] - A.GetRow(conIdx)*decVec) /
                  (A.GetRow(conIdx)*stepVector);
               distanceIndex++;
               inactiveConIdxs.Resize(inactiveConIdxsIndex + 1);
               inactiveConIdxs[inactiveConIdxsIndex] = conIdx;
               inactiveConIdxsIndex++;
            }
         }
      }
   }

   for (Integer i = 0; i < distanceToInactiveCons.GetSize(); ++i)
      distanceToInactiveCons[i] = std::abs(distanceToInactiveCons[i]);
}

//------------------------------------------------------------------------------
// void ComputeNearbyFeasibleLinearSystem(Rvector dV, Rmatrix &Anear, 
//                                        Rvector &bnear)
//------------------------------------------------------------------------------
/**
* Given a decision vector, computes a "nearby" system of
* equations for solution to create a feasible guess for phase
* I.  This involves choosing b in Ax = b.  The full
* problem is an inequality, conLowerBounds <= Ax < conUpperBounds.  If
* Ax satisfies the inequaly, then bnear(i) = A(i,:)*x.
* If upper bound is violated,  A(i,:)*x > conUpperBounds, 
*                             then bnear(i) = conUpperBounds(i).
* If lower bound is violated, A(i,:)*x < conLowerBounds, 
*                             then bnear(i) = conLowerBounds(i);
*
* @param dV The decision vector
* @param &Anear Matrix containing coefficients of linear equations
* @param &bnear Chosen vector for Ax = b
*/
//------------------------------------------------------------------------------
void MinQP::ComputeNearbyFeasibleLinearSystem(Rvector dV, Rmatrix &Anear,
                                              Rvector &bnear)
{
   // Dimension arrays
   Integer numGuessCons = numEqCons + numWorkingIneqCons;
   Anear.SetSize(numGuessCons, numDecisionVars);
   bnear.SetSize(numGuessCons);
   // Add in the equality constraints
   for (Integer i = 0; i < numEqCons; ++i)
   {
      for (Integer j = 0; j < Anear.GetNumColumns(); ++j)
         Anear(i, j) = A((Integer)eqIdxs[i], j);
      bnear[i] = conLowerBounds[(Integer)eqIdxs[i]];
   }
   // Now handle inequalities.  If current x satisfies constraint
   // use the resulting b value, otherwise choose the violated
   // boundary (upper or lower).
   Integer linSysRowCount = numEqCons;
   Integer conIdx;
   Real conValue;
   for (Integer loopIdx = 0; loopIdx < numWorkingIneqCons; ++loopIdx)
   {
      linSysRowCount++;
      conIdx = (Integer)activeIneqSet(loopIdx);
      for (Integer j = 0; j < Anear.GetNumColumns(); ++j)
         Anear(linSysRowCount - 1, j) = A(conIdx, j);
      conValue = Anear.GetRow(linSysRowCount - 1)*decVec;
      if (conValue < conLowerBounds[conIdx])
         bnear[linSysRowCount - 1] = conLowerBounds[conIdx];
      else
      {
         if (conValue > conUpperBounds[conIdx])
            bnear[linSysRowCount - 1] = conUpperBounds[conIdx];
         else
            bnear[linSysRowCount - 1] = conValue;
      }
   }
}

//------------------------------------------------------------------------------
// void SetUpPhaseI(Rvector initialGuess, Rvector &initialGuess_I,
//                         Rmatrix &A_I, Rmatrix &G, Rvector &d, Rvector &b_L,
//                         Rvector &b_U)
//------------------------------------------------------------------------------
/**
*  This helper function sets up the Phase I problem which attempts to find
*  a feasible guess to the QP problem by formulating an alternative QP
*  problem that is equivalent to minimizing the infinity norm of the
*  contraint violations.  This function sets up the cost function, constraints,
*  and initial guess for the phase I feasibility issue based on the write up on
*  pg. 473 of Ref. [1].

*----- Calculate a step that satisfies the initial working set, but is as
*      close to the user's guess as possible.
*            solve        Min dx
*            subject to   A(initialGuess + dx) = b
*
*       The solution is dx = A^-1*( b - A*initialGuess);
*
* @param dV The decision vector
* @param &Anear Matrix containing coefficients of linear equations
* @param &bnear Chosen vector for Ax = b
*/
//------------------------------------------------------------------------------
void MinQP::SetUpPhaseI(Rvector initialGuess, Rvector &initialGuess_I,
                        Rmatrix &A_I, Rmatrix &G, Rvector &d, Rvector &b_L,
                        Rvector &b_U)
{
   Real inf = std::numeric_limits<double>::infinity();
   Real ninf = -inf;
   Rmatrix Anear;
   Rvector bnear;
   Rvector dx;
   Real singularityTestValue = 1e-15;
   ComputeNearbyFeasibleLinearSystem(initialGuess, Anear, bnear);
   if (numDecisionVars >= numActiveCons && numActiveCons > 0)
   {
      dx = multiMatrixToColumn(Anear.Pseudoinverse(singularityTestValue),
         (bnear - multiMatrixToColumn(Anear, initialGuess)));
      initialGuess += dx;
   }

   // Calculate the maximum constraint violation
   SetDecisionVector(initialGuess);
   Rvector Viol = ComputeAllConViolations();
   Rvector copyViol(numCons - numEqCons + 1);
   for (Integer violIdx = 0; violIdx < copyViol.GetSize(); ++violIdx)
   {
      if (violIdx != copyViol.GetSize() - 1)
         copyViol[violIdx] = Viol[violIdx + numEqCons];
      else
         copyViol[violIdx] = 0;
   }
   Real maxViol;
   if (numCons > numEqCons)
      maxViol = InfNorm(Viol);
   else
      maxViol = 0;

   // Set up the cost function quantities
   G.SetSize(numDecisionVars + 1, numDecisionVars + 1); // Linear so G is zero
   d.SetSize(numDecisionVars + 1);
   for (Integer i = 0; i < d.GetSize(); ++i)
      d[i] = 0;
   d[d.GetSize() - 1] = 1; // Ref. [2]. Eqs. 7.9.5

   // Set up constraint functions
   initialGuess_I.SetSize(initialGuess.GetSize() + 1);
   for (Integer i = 0; i < initialGuess_I.GetSize(); ++i) // Ref. [2], Eqs. 7.9.5
   {
      if (i != initialGuess_I.GetSize() - 1)
         initialGuess_I[i] = initialGuess[i];
      else
         initialGuess_I[i] = maxViol;
   }

   b_L.SetSize(conLowerBounds.GetSize() + 1);
   for (Integer i = 0; i < b_L.GetSize(); ++i) // Ref. [2], Eqs. 7.9.6
   {
      if (i != b_L.GetSize() - 1)
         b_L[i] = conLowerBounds[i];
      else
         b_L[i] = 0;
   }
   
   b_U.SetSize(conUpperBounds.GetSize() + 1);
   for (Integer i = 0; i < b_U.GetSize(); ++i)
   {
      if (i != b_U.GetSize() - 1)
         b_U[i] = conUpperBounds[i];
      else
         b_U[i] = inf;
   }

   if (numEqCons > 0)
   {
      // Ref. [2], Eq. 7.9.6 modified to handle equality constraints
      A_I.SetSize(A.GetNumRows() + 1, numDecisionVars + 1);
      for (Integer i = 0; i < A_I.GetNumRows(); ++i)
      {
         for (Integer j = 0; j < A_I.GetNumColumns(); ++j)
         {
            if (i < A.GetNumRows() && j < A.GetNumColumns())
               A_I(i, j) = A(i, j);
            if (i >= numEqCons && j >= A.GetNumColumns())
               A_I(i, j) = 1;
         }
      }
   }
   else
   {
      // Ref. [2], Eq. 7.9.6
      A_I.SetSize(A.GetNumRows() + 1, A.GetNumColumns() + 1);
      for (Integer i = 0; i < A_I.GetNumRows(); ++i)
      {
         for (Integer j = 0; j < A_I.GetNumColumns(); ++j)
         {
            if (i < A.GetNumRows() && j < A.GetNumColumns())
               A_I(i, j) = A(i, j);
            if (j >= A.GetNumColumns())
               A_I(i, j) = 1;
         }
      }
   }
}

//------------------------------------------------------------------------------
// Integer ValidateProblemConfig()
//------------------------------------------------------------------------------
/**
* This method determines whether the inputs using in this class are valid
* sizes/values to be able to begin optimization
*
* @return flag Returns 1 on success and 0 when an issue is encountered
*/
//------------------------------------------------------------------------------
Integer MinQP::ValidateProblemConfig()
{
   //---------------------------------
   //  Check constraint data
   //---------------------------------

   Integer flag = 0;
   // Are A matrix and x vector consistent?
   if (A.GetNumRows() != 0 && A.GetNumColumns() != 0)
   {
      if (A.GetNumColumns() != numDecisionVars)
      {
         MessageInterface::ShowMessage("The number of columns in A is not the same as the ");
         MessageInterface::ShowMessage("number of rows in X\n");
         return flag;
      }
   }

   // Are sets of inequality and equality constraint sets consistent with the
   // A matrix?
   if (numEqCons + numIneqCons > numCons)
   {
      MessageInterface::ShowMessage("The number constraints in the equality "
         "and inequality sets exceeds the number of constraints in the A "
         "matrix.\n");
      return flag;
   }

   // Are A matrix and b vector consistent?
   if (conLowerBounds.GetSize() != 0)
   {
      if (numCons != conLowerBounds.GetSize())
      {
         MessageInterface::ShowMessage("The number of rows in A is not the "
            "same as the number of rows in b.\n");
         return flag;
      }
   }

   // Check that constraint upper bounds are >= to Lower bounds
   if (conLowerBounds.GetSize() != 0)
   {
      for (Integer conIdx = 0; conIdx < numCons; ++conIdx)
      {
         if (conUpperBounds[conIdx] < conLowerBounds[conIdx])
         {
            MessageInterface::ShowMessage("Upper Bound on constriant %i is "
               "less than lower bound\n", conIdx);
            return flag;
         }
      }
   }

   // Are the elements of W0 contained only in the inequality set?
   for (Integer i = 0; i < numWorkingIneqCons; ++i)
   {
      for (Integer eqIndex = 0; eqIndex < eqIdxs.GetSize(); ++eqIndex)
      {
         if (activeIneqSet[i] == eqIdxs[eqIndex])
         {
            MessageInterface::ShowMessage("An index in the working set is "
               "also in the equality set.\n");
            return flag;
         }
      }
   }

   // Are the elements of W0 all contained in the inequality set?
   bool contained = false;
   if (activeIneqSet.GetSize() != 0)
   {
      for (Integer i = 0; i < numWorkingIneqCons; ++i)
      {
         for (Integer ineqIndex = 0; ineqIndex < ineqIdxs.GetSize(); 
            ++ineqIndex)
         {
            if (activeIneqSet[i] == ineqIdxs[ineqIndex])
               contained = true;
         }
      }

      if (!contained)
      {
         MessageInterface::ShowMessage("An index in the working set is also "
            "in the inequality set.\n");
         return flag;
      }
   }

   // Are the elements of ineqIdxs and eqIdxs unique?
   for (Integer e = 0; e < numEqCons; ++e)
   {
      for (Integer i = 0; i < ineqIdxs.GetSize(); ++i)
      {
         if (eqIdxs[e] == ineqIdxs[i])
         {
            MessageInterface::ShowMessage("A constraint index occurs in both "
               "the equality and inequality sets.\n");
            return flag;
         }
      }
   }

   //---------------------------------
   //  Check function data
   //---------------------------------

   // Are the dimensions of G consistent with X?
   if (hessianMat.GetNumRows() != numDecisionVars ||
      hessianMat.GetNumColumns() != numDecisionVars)
   {
         MessageInterface::ShowMessage("The dimensions of G are not "
            "consistent with the dimension of X\n");
         return flag;
   }

   // Are the dimensions of d ok?
   if (gradVec.GetSize() != numDecisionVars)
   {
         MessageInterface::ShowMessage("The dimensions of d are not correct\n");
         return flag;
   }

   flag = 1;
   return flag;
}

//------------------------------------------------------------------------------
// void TestForLinearlyDependentCons()
//------------------------------------------------------------------------------
/**
* This method tests the input constraint data for linearly dependent
* constraints which will cause issues in the MinQP computations.  These
* constraints are removed.  If a constraint is found to be the second bound in
* a fully bounded inequality constraint, the bound is moved into the linearly
* independent version of the constraint before being removed.
*/
//------------------------------------------------------------------------------
void MinQP::TestForLinearlyDependentCons(bool checkForDuplicateCons)
{
   // First check to see if there are any repeat constraints in the constraint
   // Jacobian if requested
   combinedCons.SetSize(0, 0);

   if (checkForDuplicateCons)
   {
      Rvector removeConIdxs(0);
      Rvector similarityTest(A.GetNumColumns());
      Integer numRemovedCons = 0;
      bool similarCon = true;
      bool skipCurrentRow = false;
      for (Integer rowIdx1 = 0; rowIdx1 < A.GetNumRows(); ++rowIdx1)
      {
         skipCurrentRow = false;
         for (Integer checkRemoveConIdx = 0; checkRemoveConIdx < numRemovedCons;
            ++checkRemoveConIdx)
         {
            if (rowIdx1 == removeConIdxs[checkRemoveConIdx])
            {
               skipCurrentRow = true;
               break;
            }
         }
         if (skipCurrentRow)
            continue;
         for (Integer rowIdx2 = rowIdx1 + 1; rowIdx2 < A.GetNumRows();
            ++rowIdx2)
         {
            similarCon = true;
            for (Integer colIdx = 0; colIdx < A.GetNumColumns(); ++colIdx)
            {
               if (std::abs(A(rowIdx1, colIdx) -
                  A(rowIdx2, colIdx)) > 1e-12)
               {
                  similarCon = false;
                  break;
               }
            }
            if (similarCon)
            {
               ++numRemovedCons;
               combinedCons.ChangeSize(numRemovedCons, 2, false);
               removeConIdxs.Resize(numRemovedCons);
               removeConIdxs[numRemovedCons - 1] = rowIdx2;
               if (conType[rowIdx1] == 1)
               {
                  if (conLowerBounds[rowIdx2] > conLowerBounds[rowIdx1])
                     conLowerBounds[rowIdx1] = conLowerBounds[rowIdx2];
                  if (conUpperBounds[rowIdx2] < conUpperBounds[rowIdx1])
                     conUpperBounds[rowIdx1] = conUpperBounds[rowIdx2];
                  combinedCons(numRemovedCons - 1, 0) = rowIdx1;
                  combinedCons(numRemovedCons - 1, 1) = rowIdx2;
               }
               else
               {
                  combinedCons(numRemovedCons - 1, 0) = -1;
                  combinedCons(numRemovedCons - 1, 1) = rowIdx2;
               }
            }
         }
      }
      numCons -= numRemovedCons;

      // Update constraint parameters
      Rmatrix ACopy = A;
      Rvector conLBCopy = conLowerBounds;
      Rvector conUBCopy = conUpperBounds;
      Integer rowsSkipped = 0;
      bool skipRow;
      A.SetSize(ACopy.GetNumRows() - numRemovedCons, A.GetNumColumns());
      conLowerBounds.SetSize(ACopy.GetNumRows() - numRemovedCons);
      conUpperBounds.SetSize(ACopy.GetNumRows() - numRemovedCons);
      for (Integer i = 0; i < ACopy.GetNumRows(); ++i)
      {
         skipRow = false;
         for (Integer removalCheckIdx = 0; removalCheckIdx < numRemovedCons;
            ++removalCheckIdx)
         {
            if (i == removeConIdxs[removalCheckIdx])
            {
               skipRow = true;
               ++rowsSkipped;
               break;
            }
         }
         if (!skipRow)
         {
            for (Integer j = 0; j < A.GetNumColumns(); ++j)
               A(i - rowsSkipped, j) = ACopy(i, j);
            conLowerBounds[i - rowsSkipped] = conLBCopy[i];
            conUpperBounds[i - rowsSkipped] = conUBCopy[i];
         }
      }

      SetConstraintTypes();
   }

   // Next check that inequality constraints are linearly independent through
   // QR factorization
   QRFactorization qrtest = QRFactorization(false);
   Rmatrix QTest(A.GetNumColumns(), A.GetNumColumns());
   Rmatrix RTest(A.GetNumColumns(), A.GetNumRows());
   qrtest.Factor(A.Transpose(), RTest, QTest);
   Rvector depInd(0);
   Integer depCount = 0;
   Real tolDep = 100 * A.GetNumColumns()*eps;
   Rmatrix permuMat = qrtest.GetParameterMatrix();
   Integer maxDiag = 0;
   if (RTest.GetNumColumns() < RTest.GetNumRows())
      maxDiag = RTest.GetNumColumns();
   else
      maxDiag = RTest.GetNumRows();
   for (Integer i = 0; i < maxDiag; ++i)
   {
      if (std::abs(RTest(i, i)) < tolDep)
      {
         depInd.Resize(depCount + 1);
         depInd[depCount] = i;
         ++depCount;
      }
   }

   // If dependent constraints are found, begin the process to delete them
   if (depCount > 0)
   {
      Rvector switchedRowIdxs(permuMat.GetNumRows());
      for (Integer i = 0; i < permuMat.GetNumRows(); ++i)
      {
         for (Integer j = 0; j < permuMat.GetNumColumns(); ++j)
         {
            if (permuMat(i, j) == 1)
            {
               switchedRowIdxs[i] = j;
               break;
            }
         }
      }

      combinedCons.SetSize(depCount, 2);
      Rvector removeConIdxs(depCount);
      bool changeBounds = true;
      Integer numChangedCons = 0;
      for (Integer i = 0; i < depCount; ++i)
         removeConIdxs[i] = switchedRowIdxs[(Integer)depInd[i]];

      if (numChangedCons < depCount)
      {
         for (Integer i = numChangedCons; i < depCount; ++i)
         {
            combinedCons(i, 0) = -1;
            combinedCons(i, 1) = removeConIdxs[i];
         }
      }

      numCons -= depCount;

      // Update constraint parameters
      Rmatrix ACopy = A;
      Rvector conLBCopy = conLowerBounds;
      Rvector conUBCopy = conUpperBounds;
      Integer rowsSkipped = 0;
      bool skipRow;
      A.SetSize(ACopy.GetNumRows() - depCount, A.GetNumColumns());
      conLowerBounds.SetSize(ACopy.GetNumRows() - depCount);
      conUpperBounds.SetSize(ACopy.GetNumRows() - depCount);
      for (Integer i = 0; i < ACopy.GetNumRows(); ++i)
      {
         skipRow = false;
         for (Integer removalCheckIdx = 0; removalCheckIdx < depCount;
            ++removalCheckIdx)
         {
            if (i == removeConIdxs[removalCheckIdx])
            {
               skipRow = true;
               ++rowsSkipped;
               break;
            }
         }
         if (!skipRow)
         {
            for (Integer j = 0; j < A.GetNumColumns(); ++j)
               A(i - rowsSkipped, j) = ACopy(i, j);
            conLowerBounds[i - rowsSkipped] = conLBCopy[i];
            conUpperBounds[i - rowsSkipped] = conUBCopy[i];
         }
      }

      SetConstraintTypes();
   }
}

//------------------------------------------------------------------------------
// Real GetMax(Rvector inputVector)
//------------------------------------------------------------------------------
/**
* Method to find maximum value in an Rvector
*
* @param inputVector The vector to find a maximum in
*
* @return currentMax The maximum value in the vector
*/
//------------------------------------------------------------------------------
Real MinQP::GetMax(Rvector inputVector)
{
   if (inputVector.GetSize() == 0)
      return 0.0;

   Real currentMax = inputVector[0];
   for (Integer i = 1; i < inputVector.GetSize(); ++i)
   {
      if (inputVector[i] > currentMax)
         currentMax = inputVector[i];
   }

   return currentMax;
}

//------------------------------------------------------------------------------
// Real GetMin(Rvector inputVector)
//------------------------------------------------------------------------------
/**
* Method to find minimum value in an Rvector
*
* @param inputVector The vector to find a minimum in
*
* @return currentMin The minimum value in the vector
*/
//------------------------------------------------------------------------------
Real MinQP::GetMin(Rvector inputVector)
{
   Real currentMin = inputVector[0];
   for (Integer i = 1; i < inputVector.GetSize(); ++i)
   {
      if (inputVector[i] < currentMin)
         currentMin = inputVector[i];
   }

   return currentMin;
}

//------------------------------------------------------------------------------
// Real InfNorm(Rvector inputVector)
//------------------------------------------------------------------------------
/**
* Method to calculate the infinite-norm of an Rvector
*
* @param inputVector The vector to calculate the infinite-norm of
*
* @return currentMax The infinite-norm
*/
//------------------------------------------------------------------------------
Real MinQP::InfNorm(Rvector inputVector)
{
   Real currentMax = std::abs(inputVector[0]);
   for (Integer i = 1; i < inputVector.GetSize(); ++i)
   {
      if (std::abs(inputVector[i]) > currentMax)
         currentMax = std::abs(inputVector[i]);
   }

   return currentMax;
}

//------------------------------------------------------------------------------
// Rvector multiMatrixToColumn(Rmatrix inputMatrix, Rvector inputVector)
//------------------------------------------------------------------------------
/**
* Method to multiply an Rmatrix to an Rvector when the Rvector is considered
* to be a column vector
*
* @param inputMatrix The matrix to be multiplied
* @param inputVector The column vector to be multiplied
*
* @return product The product of the matrix and column vector
*/
//------------------------------------------------------------------------------
Rvector MinQP::multiMatrixToColumn(Rmatrix inputMatrix, Rvector inputVector)
{
   Rvector product(inputMatrix.GetNumRows());
   Real* productData = (Real*)product.GetDataVector();
   Real* inputMatrixData = (Real*)inputMatrix.GetDataVector();
   Real* inputVectorData = (Real*)inputVector.GetDataVector();

   Integer rowCount = inputMatrix.GetNumRows();
   Integer colCount = inputMatrix.GetNumColumns();
   Integer vectorSize = inputVector.GetSize();

   for (Integer i = 0; i < rowCount; ++i)
   {
      for (Integer j = 0; j < vectorSize; ++j)
      {
         productData[i] +=
            inputMatrixData[i * colCount + j] * inputVectorData[j];
      }
   }

   return product;
}

//------------------------------------------------------------------------------
// Rvector multiRowToMatrix(Rmatrix inputMatrix, Rvector inputVector)
//------------------------------------------------------------------------------
/**
* Method to multiply an Rvector to an Rmatrix when the Rvector is considered
* a row vector
*
* @param inputMatrix The matrix to be multiplied
* @param inputVector The row vector to be multiplied
*
* @return product The product of the row vector and matrix
*/
//------------------------------------------------------------------------------
Rvector MinQP::multiRowToMatrix(Rmatrix inputMatrix, Rvector inputVector)
{
   Rvector product(inputVector.GetSize());
   Real* productData = (Real*)product.GetDataVector();
   Real* inputMatrixData = (Real*)inputMatrix.GetDataVector();
   Real* inputVectorData = (Real*)inputVector.GetDataVector();
   
   Integer rowCount = inputMatrix.GetNumRows();
   Integer colCount = inputMatrix.GetNumColumns();
   Integer vectorSize = inputVector.GetSize();

   for (Integer i = 0; i < rowCount; ++i)
   {
      for (Integer j = 0; j < vectorSize; ++j)
      {
         productData[i] +=
            inputMatrixData[j * colCount + i] * inputVectorData[j];
      }
   }

   return product;
}

//------------------------------------------------------------------------------
// Rmatrix multiMatrix(Rmatrix matrix1, Rmatrix matrix2)
//------------------------------------------------------------------------------
/**
* Method to multiply an Rmatrix to an Rmatrix using data vectors
*
* @param matrix1 The first matrix being multiplied
* @param matrix2 The second matrix being multiplied 
*
* @return product The product of the two matrices
*/
//------------------------------------------------------------------------------
Rmatrix MinQP::multiMatrix(Rmatrix matrix1, Rmatrix matrix2)
{
   Rmatrix product(matrix1.GetNumRows(), matrix2.GetNumColumns());
   Real* productData = (Real*)product.GetDataVector();
   Real* matrix1Data = (Real*)matrix1.GetDataVector(); 
   Real* matrix2Data = (Real*)matrix2.GetDataVector();

   Integer rowCount = product.GetNumRows();
   Integer colCount = product.GetNumColumns();
   Integer matrix1ColCount = matrix1.GetNumColumns();
   Integer matrix2ColCount = matrix2.GetNumColumns();
   Integer rowStartElem1;
   Integer rowStartElem2;
   for (Integer rowIdx = 0; rowIdx < rowCount; ++rowIdx)
   {
      rowStartElem1 = rowIdx * colCount;
      rowStartElem2 = rowIdx * matrix1ColCount;
      for (Integer colIdx = 0; colIdx < colCount; ++colIdx)
      {
         for (Integer multIdx = 0; multIdx < matrix1ColCount; ++multIdx)
         {
            productData[rowStartElem1 + colIdx] +=
               matrix1Data[rowStartElem2 + multIdx] *
               matrix2Data[multIdx * matrix2ColCount + colIdx];
         }
      }
   }

   return product;
}
