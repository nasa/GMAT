//$Id$
//------------------------------------------------------------------------------
//                             NumericJacobian
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2018.07.10
//
/**
* Defines the NumericJacobian class.  This class is a conversion of the numjac
* function from MATLAB.
*/
//------------------------------------------------------------------------------
#include "NumericJacobian.hpp"
#include "StringUtil.hpp"
#include "UtilityException.hpp"

//#define DEBUG_JACOBIAN_EVAL

//------------------------------------------------------------------------------
// NumericJacobian()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
NumericJacobian::NumericJacobian() :
   currentState(INITIALIZING),
   ny(0),
   nF(0),
   perturbIdx(0),
   nfevals(0),
   nfcalls(0),
   stepSizeCalculated(false),
   br(pow(GmatRealConstants::REAL_EPSILON, 0.875)),
   bl(pow(GmatRealConstants::REAL_EPSILON, 0.75)),
   bu(pow(GmatRealConstants::REAL_EPSILON, 0.25)),
   facmin(pow(GmatRealConstants::REAL_EPSILON, 0.78)),
   facmax(0.1),
   refineCols(false),
   refineCurrCol(false),
   refineColIdx(0),
   tmpfac(0),
   delVal(0)
{

}

//------------------------------------------------------------------------------
// NumericJacobian(const NumericJacobian &copy)
//------------------------------------------------------------------------------
/**
* Copy constructor
*/
//------------------------------------------------------------------------------
NumericJacobian::NumericJacobian(const NumericJacobian &copy) : 
   currentState(copy.currentState),
   ny(copy.ny),
   nF(copy.nF),
   perturbIdx(copy.perturbIdx),
   nfevals(copy.nfevals),
   nfcalls(copy.nfevals),
   stepSizeCalculated(copy.stepSizeCalculated),
   br(copy.br),
   bl(copy.bl),
   bu(copy.bu),
   facmin(copy.facmin),
   facmax(copy.facmax),
   refineCols(copy.refineCols),
   refineCurrCol(copy.refineCurrCol),
   refineColIdx(copy.refineColIdx),
   tmpfac(copy.tmpfac),
   delVal(copy.delVal)
{

}

//------------------------------------------------------------------------------
// NumericJacobian& operator=(const NumericJacobian &copy)
//------------------------------------------------------------------------------
/**
* Assignment operator
*/
//------------------------------------------------------------------------------
NumericJacobian& NumericJacobian::operator=(const NumericJacobian &copy)
{
   currentState = copy.currentState;
   ny = copy.ny;
   nF = copy.nF;
   perturbIdx = copy.perturbIdx;
   nfevals = copy.nfevals;
   nfcalls = copy.nfcalls;
   stepSizeCalculated = copy.stepSizeCalculated;
   br = copy.br;
   bl = copy.bl;
   bu = copy.bu;
   facmin = copy.facmin;
   facmax = copy.facmax;
   refineCols = copy.refineCols;
   refineCurrCol = copy.refineCurrCol;
   refineColIdx = copy.refineColIdx;
   tmpfac = copy.tmpfac;
   delVal = copy.delVal;

   return *this;
}

//------------------------------------------------------------------------------
// ~NumericJacobian()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
NumericJacobian::~NumericJacobian()
{

}

//------------------------------------------------------------------------------
// AdvanceState()
//------------------------------------------------------------------------------
/**
* Method to advance the state of the Jacobian calculation
*
* @return The current state of the Jacobian calculation
*/
//------------------------------------------------------------------------------
NumericJacobian::JacState NumericJacobian::AdvanceState()
{
   switch (currentState)
   {
   case INITIALIZING:
      stepSizeCalculated = false;
      perturbIdx = 0;
      if (CheckInitialParams())
      {
         currentState = PERTURBING;
         CalculatePerturbations();
         PerturbVars();
      }
      break;
   case PERTURBING:
      if (perturbIdx >= ny)
      {
         currentState = CALCULATING;
         break;
      }
      PerturbVars();
      break;
   case CALCULATING:
      CalculateJacobian();
      if (refineCols)
      {
         PrepareForRefinement();
         currentState = REFINING;
         while (refineColIdx < ny)
         {
            if (!colsToRefine.at(refineColIdx) || !k1.at(refineColIdx))
               ++refineColIdx;
            else
               break;
         }
         if (refineColIdx >= ny)
         {
            UpdateWorkingStorage();
            currentState = FINISHED;
            break;
         }
         CalcRefinement();
         PerturbVars();
      }
      else
         currentState = FINISHED;
      break;
   case REFINING:
      if (refineCurrCol)
         RefineJacColumn();
      while (refineColIdx < ny)
      {
         if (!colsToRefine.at(refineColIdx) || !k1.at(refineColIdx))
            ++refineColIdx;
         else
            break;
      }
      if (refineColIdx >= ny)
      {
         UpdateWorkingStorage();
         currentState = FINISHED;
         break;
      }
      CalcRefinement();
      PerturbVars();
      break;
   default:
      throw UtilityException("Unknown state reached in numeric Jacobian "
         "computation.\n");
      break;
   }

   return currentState;
}

//------------------------------------------------------------------------------
// JacState GetState()
//------------------------------------------------------------------------------
/**
* Returns the current state of the state machine from the JacState enumeration
* 
* @return currentState The current state
*/
//------------------------------------------------------------------------------
NumericJacobian::JacState NumericJacobian::GetState()
{
   return currentState;
}

//------------------------------------------------------------------------------
// void SetInitialValues(Rvector initialState, Rvector initialDerivs,
//                       Rmatrix threshold, Rvector inputfac)
//------------------------------------------------------------------------------
/**
* Method to set initial values neccessary for computations, this method should
* be called during the INITIALIZING state
*
* @param initialState The initial dependent variable values
* @param initialDerivs The function values at the initial dependent variables
* @param threshold If a dependent variable is below the threshold,
*                  the threshold value is used instead for perturbation
*                  calculations to avoid the perturbation being too small
* @param inputfac The current working storage values, the size of the vector
*                 should be zero if this is the first Jacobian calculation
*/
//------------------------------------------------------------------------------
void NumericJacobian::SetInitialValues(Rvector initialState,
                                       Rvector initialDerivs,
                                       Rmatrix threshold,
                                       Rvector inputfac)
{
   y.SetSize(initialState.GetSize());
   y = initialState;
   ny = y.GetSize();

   Fty.SetSize(initialDerivs.GetSize());
   Fty = initialDerivs;
   nF = Fty.GetSize();

   threshScal.SetSize(threshold.GetNumRows(), threshold.GetNumColumns());
   threshScal = threshold;

   fac.SetSize(inputfac.GetSize());
   fac = inputfac;

   currentVars.SetSize(ny);

   Fdel.SetSize(nF, ny);
}

//------------------------------------------------------------------------------
// Rvector GetCurrentVars()
//------------------------------------------------------------------------------
/**
* Method to return the current dependent variable values, this is normally
* called once a perturbation has been applied
*
* @return currentVars Vector of dependent variables
*/
//------------------------------------------------------------------------------
Rvector NumericJacobian::GetCurrentVars()
{
   return currentVars;
}

//------------------------------------------------------------------------------
// void SetDerivs(Rvector inputDerivs)
//------------------------------------------------------------------------------
/**
* Method to set current function values, this is normally called from an
* outside class after a perturbation was used to calculate new derivatives
*
* @param inputDerivs The new function values
*/
//------------------------------------------------------------------------------
void NumericJacobian::SetDerivs(Rvector inputDerivs)
{
   if (inputDerivs.GetSize() != nF)
   {
      throw UtilityException("The number of derivatives provided to the "
         "numeric Jacobian does not match the number of initial "
         "derivatives.\n");
   }

   if (currentState == PERTURBING)
   {
      for (Integer i = 0; i < nF; ++i)
         Fdel(i, perturbIdx) = inputDerivs(i);
      ++perturbIdx;
   }
   else if (currentState == REFINING)
   {
      fdel = inputDerivs;
   }
}

//------------------------------------------------------------------------------
// Rmatrix GetJacobian()
//------------------------------------------------------------------------------
/**
* Returns the Jacobian, this method should be called only in the FINISHED state
*
* @return dFdy The Jacobian matrix
*/
//------------------------------------------------------------------------------
Rmatrix NumericJacobian::GetJacobian()
{
   return dFdy;
}

//------------------------------------------------------------------------------
// Rvector GetWorkingStorage()
//------------------------------------------------------------------------------
/**
* Returns the current working storage, used to determine perturbation values.
* This method should be called only in the FINISHED state
*
* @return fac The vector of working storage values
*/
//------------------------------------------------------------------------------
Rvector NumericJacobian::GetWorkingStorage()
{
   return fac;
}

//------------------------------------------------------------------------------
// bool CheckInitialParams()
//------------------------------------------------------------------------------
/**
* Method to check that initial values recieved are set up correctly, this
* method is called during initialization
*
* @return True if parameters are set up correctly
*/
//------------------------------------------------------------------------------
bool NumericJacobian::CheckInitialParams()
{
   if (y.GetSize() == 0 || Fty.GetSize() == 0 || threshScal.GetNumRows() == 0 ||
      threshScal.GetNumColumns() == 0)
   {
      throw UtilityException("Not all required initial values have been "
         "provided to calculate a Jacobian.  An initial state, set of "
         "derivatives, and threshold values must be provided.\n");
   }
   
   ny = y.GetSize();
   nF = Fty.GetSize();

   if (threshScal.GetNumColumns() != ny)
   {
      throw UtilityException("The number of columns in the threshold matrix "
         "must match the number input variables for the Jacobian calculation.");
   }

   return true;
}

//------------------------------------------------------------------------------
// void CalculatePerturbations()
//------------------------------------------------------------------------------
/**
* Method to calculate perturbed values of the dependent variables.  Perturbed
* values are stored in a matrix where each column represents the index of the
* variable that was modified.
*/
//------------------------------------------------------------------------------
void NumericJacobian::CalculatePerturbations()
{
   // Temporary code to set S and g to empty while sparse matrices cannot be
   // used
   S.SetSize(0, 0);

   if (fac.GetSize() == 0)
   {
      fac.SetSize(ny);
      for (Integer i = 0; i < ny; ++i)
         fac(i) = sqrt(GmatRealConstants::REAL_EPSILON);
   }
   nfevals = 0;
   nfcalls = 0;

   // Select an increment del for a difference approximation to
   // column j of dFdy.The vector fac accounts for experience
   // gained in previous calls to numjac.
   Rvector thresh;
   Rvector typicalY;
   if (threshScal.GetNumRows() == 2)
   {
      thresh = threshScal.GetRow(0);
      for (Integer i = 0; i < threshScal.GetNumColumns(); ++i)
         typicalY(i) = GmatMathUtil::Abs(threshScal(1, i));
   }
   else if (threshScal.GetNumRows() == 1)
   {
      thresh = threshScal.GetRow(0);
      typicalY.SetSize(threshScal.GetNumColumns());
      for (Integer i = 0; i < threshScal.GetNumColumns(); ++i)
         typicalY(i) = 0.0;
   }

   yscale.SetSize(y.GetSize());
   for (Integer i = 0; i < y.GetSize(); ++i)
   {
      if (GmatMathUtil::Abs(y(i)) > thresh(i))
         yscale(i) = GmatMathUtil::Abs(y(i));
      else
         yscale(i) = thresh(i);
   }
   for (Integer i = 0; i < y.GetSize(); ++i)
   {
      if (typicalY(i) > yscale(i))
         yscale(i) = typicalY(i);
   }

   del.SetSize(y.GetSize());
   IntegerArray delZeroIdxs;
   for (Integer i = 0; i < del.GetSize(); ++i)
   {
      del(i) = fac(i) * yscale(i);
      if (del(i) == 0)
         delZeroIdxs.push_back(i);
   }
   for (Integer i = 0; i < delZeroIdxs.size(); ++i)
   {
      while (true)
      {
         if (fac(delZeroIdxs.at(i)) < facmax)
         {
            if (100 * fac(delZeroIdxs.at(i)) < facmax)
               fac(delZeroIdxs.at(i)) *= 100;
            else
               fac(delZeroIdxs.at(i)) = facmax;
            del(delZeroIdxs.at(i)) = fac(delZeroIdxs.at(i)) *
               yscale(delZeroIdxs.at(i));
            if (del(delZeroIdxs.at(i)) != 0)
               break;
         }
         else
         {
            del(delZeroIdxs.at(i)) = thresh(delZeroIdxs.at(i));
            break;
         }
      }
   }

   if (nF == ny)
   {
      BooleanArray s;
      for (Integer i = 0; i < Fty.GetSize(); ++i)
      {
         if (Fty(i) >= 0)
            s.push_back(1);
         else
            s.push_back(0);
      }

      // Keep del pointing into region
      for (Integer i = 0; i < del.GetSize(); ++i)
         del(i) = (s.at(i) - (!s.at(i))) * del(i);
   }

   // Form a difference approximation to all columns of dFdy
   if (S.GetNumRows() == 0 || S.GetNumColumns() == 0)
   {
      // Generate full matrix dFdy
      g.resize(0);
      for (Integer i = 0; i < ny; ++i)
         g.push_back(i);
      ydel.SetSize(ny, ny);
      for (Integer i = 0; i < y.GetSize(); ++i)
      {
         for (Integer j = 0; j < ny; ++j)
         {
            if (i == j)
               ydel(i, j) = y(i) + del(i);
            else
               ydel(i, j) = y(i);
         }
      }
   }

   stepSizeCalculated = true;
}

//------------------------------------------------------------------------------
// void PerturbVars()
//------------------------------------------------------------------------------
/**
* Method to update the current variables with perturbed values taken from a
* column from the perturbation matrix
*/
//------------------------------------------------------------------------------
void NumericJacobian::PerturbVars()
{
   if (currentState == PERTURBING)
      currentVars = ydel.GetColumn(perturbIdx);
   else if (currentState == REFINING)
      currentVars = ydel.GetColumn(0);
}

//------------------------------------------------------------------------------
// void CalculateJacobian()
//------------------------------------------------------------------------------
/**
* Calculates the Jacobian after all derivative data was received from the
* outside class that needs a Jacobian using finite differencing.  It is also
* determined whether refinement is required for any column of the Jacobian.
*/
//------------------------------------------------------------------------------
void NumericJacobian::CalculateJacobian()
{
   // Get the difference between the derivatives after and before the step
   // was applied to y
   Rmatrix Fdiff(nF, ny);
   for (Integer i = 0; i < nF; ++i)
   {
      for (Integer j = 0; j < ny; ++j)
         Fdiff(i, j) = Fdel(i, j) - Fty(i);
   }

   // Divide step sizes taken in y into corresponding derivative values
   Rmatrix diagdel(ny, ny);
   for (Integer i = 0; i < ny; ++i)
      diagdel(i, i) = 1.0 / del(i);
   dFdy.SetSize(nF, ny);
   dFdy = Fdiff * diagdel;

   diffMax.SetSize(ny);
   diffMax.MakeZeroVector();
   rowMax.resize(ny);
   for (Integer i = 0; i < nF; ++i)
   {
      for (Integer j = 0; j < ny; ++j)
      {
         if (GmatMathUtil::Abs(Fdiff(i, j)) > diffMax(j))
         {
            diffMax(j) = GmatMathUtil::Abs(Fdiff(i, j));
            rowMax.at(j) = i;
         }
      }
   }

   // If Fdel is a vector, then index is a scalar, so indexing is okay.
   Rvector FdelVec(Fdel.GetNumRows() * Fdel.GetNumColumns());
   Integer vecIdx = 0;
   for (Integer j = 0; j < Fdel.GetNumColumns(); ++j)
   {
      for (Integer i = 0; i < Fdel.GetNumRows(); ++i)
      {
         FdelVec(vecIdx) = Fdel(i, j);
         ++vecIdx;
      }
   }
   absFdelRm.SetSize(ny);
   for (Integer i = 0; i < ny; ++i)
      absFdelRm(i) = GmatMathUtil::Abs(FdelVec(i * nF + rowMax.at(i)));

   // Adjust fac for next call to CalculateJacobian
   absFty.SetSize(nF);
   for (Integer i = 0; i < nF; ++i)
      absFty(i) = GmatMathUtil::Abs(Fty(i));
   absFtyRm.SetSize(rowMax.size());
   for (Integer i = 0; i < rowMax.size(); ++i)
      absFtyRm(i) = absFty(rowMax.at(i));
   refineCols = false;
   for (Integer i = 0; i < nF; ++i)
   {
      for (Integer j = 0; j < ny; ++j)
      {
         if (GmatMathUtil::IsNaN(dFdy(i, j)))
            return;
      }
   }

   for (Integer i = 0; i < diffMax.GetSize(); ++i)
   {
      // Refine only requested columns (g) that satisfy conditions
      if ((((absFdelRm(i) != 0) && (absFtyRm(i) != 0)) || (diffMax(i) == 0)) &&
         (g.at(i) > -1))
      {
         colsToRefine.push_back(true);
         refineCols = true;
      }
      else
         colsToRefine.push_back(false);
   }
}

//------------------------------------------------------------------------------
// void PrepareForRefinement()
//------------------------------------------------------------------------------
/**
* Prepare required vectors for refining and add another check whether a change
* in a set of function values was too small and requires refinement
*/
//------------------------------------------------------------------------------
void NumericJacobian::PrepareForRefinement()
{
   ydel.SetSize(ny, 1);
   for (Integer i = 0; i < ny; ++i)
      ydel(i, 0) = y(i);
   Fscale.SetSize(absFdelRm.GetSize());
   for (Integer i = 0; i < Fscale.GetSize(); ++i)
   {
      if (absFdelRm(i) > absFtyRm(i))
         Fscale(i) = absFdelRm(i);
      else
         Fscale(i) = absFtyRm(i);
   }

   // If the difference in f values is so small that the column might be just
   // roundoff error, try a bigger increment
   for (Integer i = 0; i < diffMax.GetSize(); ++i)
   {
      // Difmax and Fscale might be zero
      if (diffMax(i) <= br*Fscale(i))
         k1.push_back(true);
      else
         k1.push_back(false);
   }
}

//------------------------------------------------------------------------------
// void CalcRefinement()
//------------------------------------------------------------------------------
/**
* Calculate the perturbation to be used in the refinement, store the
* perturbation to possibly be using in the working storage for future calls to
* this class
*/
//------------------------------------------------------------------------------
void NumericJacobian::CalcRefinement()
{
   if (sqrt(fac(refineColIdx)) < facmax)
      tmpfac = sqrt(fac(refineColIdx));
   else
      tmpfac = facmax;
   delVal = tmpfac*yscale(refineColIdx);
   if ((tmpfac != fac(refineColIdx)) && (delVal != 0))
   {
      if (nF == ny)
      {
         if (Fty(refineColIdx) >= 0) // keep del pointing into the region
            delVal = GmatMathUtil::Abs(delVal);
         else
            delVal = -GmatMathUtil::Abs(delVal);
      }

      ydel(refineColIdx, 0) = y(refineColIdx) + delVal;
      refineCurrCol = true;
   }
   else
   {
      refineCurrCol = false;
      ++refineColIdx;
   }
}

//------------------------------------------------------------------------------
// void RefineJacColumn()
//------------------------------------------------------------------------------
/**
* Apply the refinement after new derivatives were gathered fromt he
* perturbation
*/
//------------------------------------------------------------------------------
void NumericJacobian::RefineJacColumn()
{
   ydel(refineColIdx, 0) = y(refineColIdx);
   Rvector fdiff = fdel - Fty;
   Rvector tmp(nF);
   for (Integer j = 0; j < nF; ++j)
      tmp(j) = fdiff(j) / delVal;

   Real diffmax = GmatMathUtil::Abs(fdiff(0));
   Integer rowmax = 0;
   for (Integer j = 1; j < nF; ++j)
   {
      if (GmatMathUtil::Abs(fdiff(j)) > diffmax)
      {
         diffmax = GmatMathUtil::Abs(fdiff(j));
         rowmax = j;
      }
   }

   Real tmpNorm = GmatMathUtil::Abs(tmp(0));
   for (Integer j = 1; j < tmp.GetSize(); ++j)
   {
      if (GmatMathUtil::Abs(tmp(j)) > tmpNorm)
         tmpNorm = GmatMathUtil::Abs(tmp(j));
   }
   Real dFdyNorm = GmatMathUtil::Abs(dFdy(0, refineColIdx));
   for (Integer j = 1; j < dFdy.GetNumRows(); ++j)
   {
      if (GmatMathUtil::Abs(dFdy(j, refineColIdx)) > dFdyNorm)
         dFdyNorm = GmatMathUtil::Abs(dFdy(j, refineColIdx));
   }


   if (tmpfac * tmpNorm >= dFdyNorm)
   {
      // The new difference is more signification, so
      // use the column computed with this increment
      if (S.GetNumRows() == 0 || S.GetNumColumns() == 0)
      {
         for (Integer j = 0; j < nF; ++j)
            dFdy(j, refineColIdx) = tmp(j);

#ifdef DEBUG_JACOBIAN_EVAL
         MessageInterface::ShowMessage("Refined dFdy:\n");
         for (Integer i = 0; i < nF; ++i)
         {
            MessageInterface::ShowMessage(
               dFdy.GetRow(i).ToString() + "\n");
         }
         MessageInterface::ShowMessage("\n");
#endif
      }
      else
      {
         // @todo Code to handle sparse matrix structures would go
         // here
      }
   }

   // Adjust fac for the next call to ComputeJacobian
   Real fscale;
   if (GmatMathUtil::Abs(fdel(rowmax)) > absFty(rowmax))
      fscale = GmatMathUtil::Abs(fdel(rowmax));
   else
      fscale = absFty(rowmax);

   if (diffmax <= bl*fscale)
   {
      // The difference is small, so increase the increment
      if (10 * tmpfac < facmax)
         fac(refineColIdx) = 10 * tmpfac;
      else
         fac(refineColIdx) = facmax;
   }

   else if (diffmax > bu*fscale)
   {
      // The difference is large, so reduce the increment
      if (0.1 * tmpfac > facmin)
         fac(refineColIdx) = 0.1 * tmpfac;
      else
         fac(refineColIdx) = facmin;
   }

   else
      fac(refineColIdx) = tmpfac;

   ++refineColIdx;
}

//------------------------------------------------------------------------------
// void UpdateWorkingStorage()
//------------------------------------------------------------------------------
/**
* Change the working storage if current perturbations caused very large or very
* small changes to function values
*/
//------------------------------------------------------------------------------
void NumericJacobian::UpdateWorkingStorage()
{
   for (Integer k = 0; k < colsToRefine.size(); ++k)
   {
      // If the difference is small, increase the increment
      if (colsToRefine.at(k) && !k1.at(k) && (diffMax(k) <= bl*Fscale(k)))
      {
         if (10 * fac(k) < facmax)
            fac(k) = 10 * fac(k);
         else
            fac(k) = facmax;
      }

      // If the difference is large, reduce the increment
      if (colsToRefine.at(k) && (diffMax(k) > bu*Fscale(k)))
      {
         if (0.1*fac(k) > facmin)
            fac(k) = 0.1*fac(k);
         else
            fac(k) = facmin;
      }
   }
}
