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
#include "NumericJacobianOriginal.hpp"
#include "StringUtil.hpp"

//#define DEBUG_JACOBIAN_EVAL

//------------------------------------------------------------------------------
// NumericJacobian()
//------------------------------------------------------------------------------
/**
* Constructor
*/
//------------------------------------------------------------------------------
NumericJacobianOriginal::NumericJacobianOriginal()
{

}

//------------------------------------------------------------------------------
// NumericJacobian(const NumericJacobian &copy)
//------------------------------------------------------------------------------
/**
* Copy constructor
*/
//------------------------------------------------------------------------------
NumericJacobianOriginal::NumericJacobianOriginal(const NumericJacobianOriginal &copy)
{

}

//------------------------------------------------------------------------------
// NumericJacobian& operator=(const NumericJacobian &copy)
//------------------------------------------------------------------------------
/**
* Assignment operator
*/
//------------------------------------------------------------------------------
NumericJacobianOriginal& NumericJacobianOriginal::operator=(const NumericJacobianOriginal &copy)
{
   return *this;
}

//------------------------------------------------------------------------------
// ~NumericJacobian()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
NumericJacobianOriginal::~NumericJacobianOriginal()
{

}

//------------------------------------------------------------------------------
//void CalculateJacobian(PhysicalModel *F, Rvector y, Rvector Fty,
//                       Rmatrix threshScal, Rvector fac,
//                       Integer vectorized, Rmatrix S,
//                       IntegerArray g, Rmatrix &dFdy,
//                       Integer &nfevals, Integer &nfcalls)
//------------------------------------------------------------------------------
/**
* Numerically compute the Jacobian dF/dY of function F(T,Y)
*
* @param F Pointer to the physcial model being used to get derivative data from
* @param y A vector of dependent variables
* @param Fty A vector of derivatives evaluated at the input state and time
* @param threshScal Matrix containing the threshold of significance of Y
*        (first row) and typical value of Y (second row).  The threshold
*        determines that a component Y(i) with abs(Y(i)) < threshScal(i) is not
*        important.  If only one row is entered for the threshold, typical Y
*        values are all initialized as zeros.
* @param fac Working storage used on repeated calls to this method.  On the
*        first call, set fac to an empty vector.
* @param vectorized Tells method whether multiple values of F can be obtained
*        with a single function evaluation.  Currently values 1 and 2 are not
*        supported.
* @param S Sparsity matrix, currently only empty matrices can be used
* @param g Working storage, enter as empty on first call
* @param dFdy Matrix containing the desired jacobian
* @param nfevals Number of derivative evaluations computed
* @param nfcalls Number of calls to derivative method
*/
//------------------------------------------------------------------------------
void NumericJacobianOriginal::CalculateJacobian(PhysicalModel *F, Rvector y, Rvector Fty,
                                        Rmatrix threshScal, Rvector &fac,
                                        Integer vectorized, Rmatrix S,
                                        IntegerArray g, Rmatrix &dFdy,
                                        Integer &nfevals, Integer &nfcalls)
{
   #ifdef DEBUG_JACOBIAN_EVAL
      MessageInterface::ShowMessage(
         "\n========= Jacobian Eval Inputs =========\n\n");

      MessageInterface::ShowMessage("y:\n");
      MessageInterface::ShowMessage(y.ToString() + "\n\n");

      MessageInterface::ShowMessage("Fty:\n");
      MessageInterface::ShowMessage(Fty.ToString() + "\n\n");
   #endif

   // Temporary code to set S and g to empty while sparse matrices cannot be
   // used
   S.SetSize(0, 0);

   // Set bounds for working storage values
   Real br = pow(GmatRealConstants::REAL_EPSILON, 0.875);
   Real bl = pow(GmatRealConstants::REAL_EPSILON, 0.75);
   Real bu = pow(GmatRealConstants::REAL_EPSILON, 0.25);
   Real facmin = pow(GmatRealConstants::REAL_EPSILON, 0.78);
   Real facmax = 0.1;
   Integer ny = y.GetSize();
   Integer nF = Fty.GetSize();
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

   Rvector yscale(y.GetSize());
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

   Rvector del(y.GetSize());
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
            del(delZeroIdxs.at(i)) = fac(delZeroIdxs.at(i)) * yscale(delZeroIdxs.at(i));
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
   Rmatrix Fdel, Fdiff, ydel;
   const Real *derivs = F->GetDerivativeArray();
   Real *depVars;
   Rvector diffMax, absFdelRm;
   IntegerArray rowMax;
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

      switch (vectorized)
      {
      case 1:
         // @todo Add case of multiple dependent variable inputs if needed
      case 2:
         // @todo Add case of multiple independent variable inputs if needed
      default: // not vectorized
         Fdel.SetSize(nF, ny);
         for (Integer i = 0; i < ny; ++i)
         {
            depVars = new Real[ydel.GetNumRows()];
            for (Integer j = 0; j < ydel.GetNumRows(); ++j)
               depVars[j] = ydel(j, i);
            F->GetDerivatives(depVars, 0.0);
            for (Integer j = 0; j < nF; ++j)
               Fdel(j, i) = derivs[j];
            delete[] depVars;
         }
         nfcalls = ny;
      }
      nfevals = ny;

      // Get the difference between the derivatives after and before the step
      // was applied to y
      Fdiff.SetSize(nF, ny);
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

      #ifdef DEBUG_JACOBIAN_EVAL
         MessageInterface::ShowMessage(
            "========= Jacobian Eval Outputs =========\n\n");

         MessageInterface::ShowMessage("Number of function evaluations: " 
            + GmatStringUtil::ToString(nfevals) + "\n\n");

         MessageInterface::ShowMessage("del:\n");
         MessageInterface::ShowMessage(del.ToString() + "\n\n");

         MessageInterface::ShowMessage("ydel:\n");
         for (Integer i = 0; i < ny; ++i)
            MessageInterface::ShowMessage(ydel.GetRow(i).ToString() + "\n");
         MessageInterface::ShowMessage("\n");

         MessageInterface::ShowMessage("Fdel:\n");
         for (Integer i = 0; i < nF; ++i)
            MessageInterface::ShowMessage(Fdel.GetRow(i).ToString() + "\n");
         MessageInterface::ShowMessage("\n");

         MessageInterface::ShowMessage("Fty:\n");
         MessageInterface::ShowMessage(Fty.ToString() + "\n\n");

         MessageInterface::ShowMessage("Fdiff:\n");
         for (Integer i = 0; i < nF; ++i)
            MessageInterface::ShowMessage(Fdiff.GetRow(i).ToString() + "\n");
         MessageInterface::ShowMessage("\n");

         MessageInterface::ShowMessage("dFdy:\n");
         for (Integer i = 0; i < nF; ++i)
            MessageInterface::ShowMessage(dFdy.GetRow(i).ToString() + "\n");
         MessageInterface::ShowMessage("\n");
      #endif

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
      IntegerArray absFdelRmIdxs;
      absFdelRm.SetSize(ny);
      for (Integer i = 0; i < ny; ++i)
         absFdelRm(i) = GmatMathUtil::Abs(FdelVec(i * nF + rowMax.at(i)));
   }
   else // sparse dFdy with structure S
   {
      // @todo Code to handle sparse matrix structures would go here
   }

   // Adjust fac for next call to CalculateJacobian
   Rvector absFty(nF);
   for (Integer i = 0; i < nF; ++i)
      absFty(i) = GmatMathUtil::Abs(Fty(i));
   Rvector absFtyRm(rowMax.size());
   for (Integer i = 0; i < rowMax.size(); ++i)
      absFtyRm(i) = absFty(rowMax.at(i));
   BooleanArray refineColIdx;
   bool refineCols = false;
   for (Integer i = 0; i < diffMax.GetSize(); ++i)
   {
      // Refine only requested columns (g) that satisfy conditions
      if ((((absFdelRm(i) != 0) && (absFtyRm(i) != 0)) || diffMax(i) == 0) &&
         (g.at(i) > -1))
      {
         refineColIdx.push_back(true);
         refineCols = true;
      }
      else
         refineColIdx.push_back(false);
   }

   if (refineCols)
   {
      ydel.SetSize(ny, 1);
      for (Integer i = 0; i < ny; ++i)
         ydel(i, 0) = y(i);
      Rvector Fscale(absFdelRm.GetSize());
      for (Integer i = 0; i < Fscale.GetSize(); ++i)
      {
         if (absFdelRm(i) > absFtyRm(i))
            Fscale(i) = absFdelRm(i);
         else
            Fscale(i) = absFtyRm(i);
      }

      // If the difference in f values is so small that the column might be just
      // roundoff error, try a bigger increment
      BooleanArray k1;
      for (Integer i = 0; i < diffMax.GetSize(); ++i)
      {
         // Difmax and Fscale might be zero
         if (diffMax(i) <= br*Fscale(i))
            k1.push_back(true);
         else
            k1.push_back(false);
      }

      for (Integer i = 0; i < refineColIdx.size(); ++i)
      {
         if (refineColIdx.at(i) && k1.at(i))
         {
            Real tmpfac;
            if (sqrt(fac(i)) < facmax)
               tmpfac = sqrt(fac(i));
            else
               tmpfac = facmax;
            Real delVal = (y(i) + tmpfac*yscale(i)) - y(i);
            if ((tmpfac != fac(i)) && (del != 0))
            {
               if (nF == ny)
               {
                  if (Fty(i) >= 0) // keep del pointing into the region
                     delVal = GmatMathUtil::Abs(delVal);
                  else
                     delVal = -GmatMathUtil::Abs(delVal);
               }
               
               ydel(i, 0) = y(i) + delVal;
               Rvector fdel(nF);
               depVars = new Real[ydel.GetNumRows()];
               for (Integer j = 0; j < ydel.GetNumRows(); ++j)
                  depVars[j] = ydel(j, 0);
               F->GetDerivatives(depVars, 0.0);
               for (Integer j = 0; j < ny; ++j)
                  fdel(j) = derivs[j];
               delete[] depVars;
               ++nfevals;  // stats
               ++nfcalls;  // stats
               ydel(i, 0) = y(i);
               Rvector fdiff = fdel - Fty;
               Rvector tmp(nF);
               for (Integer j = 0; j < nF; ++j)
                  tmp(j) = fdiff(j) / del(j);

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
               for (Integer j = 0; j < tmp.GetSize(); ++j)
               {
                  if (GmatMathUtil::Abs(tmp(0)) > tmpNorm)
                     tmpNorm = GmatMathUtil::Abs(tmp(j));
               }
               Real dFdyNorm = GmatMathUtil::Abs(dFdy(0, i));
               for (Integer j = 0; j < dFdy.GetNumRows(); ++j)
               {
                  if (GmatMathUtil::Abs(dFdy(j, i)) > dFdyNorm)
                     dFdyNorm = GmatMathUtil::Abs(dFdy(j, i));
               }


               if (tmpfac * tmpNorm >= dFdyNorm)
               {
                  // The new difference is more signification, so
                  // use the column computed with this increment
                  if (S.GetNumRows() == 0 || S.GetNumColumns() == 0)
                  {
                     for (Integer j = 0; j < nF; ++j)
                        dFdy(j, i) = tmp(j);

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
                     fac(i) = 10 * tmpfac;
                  else
                     fac(i) = facmax;
               }

               else if (diffmax > bu*fscale)
               {
                  // The difference is large, so reduce the increment
                  if (0.1 * tmpfac > facmin)
                     fac(i) = 0.1 * tmpfac;
                  else
                     fac(i) = facmin;
               }

               else
                  fac(i) = tmpfac;
            }
         }
      }

      // If the difference is small, increase the increment
      for (Integer k = 0; k < refineColIdx.size(); ++k)
      {
         if (refineColIdx.at(k) && !k1.at(k) && (diffMax(k) <= bl*Fscale(k)))
         {
            if (10*fac(k) < facmax)
               fac(k) = 10*fac(k);
            else
               fac(k) = facmax;
         }

         if (refineColIdx.at(k) && (diffMax(k) > bu*Fscale(k)))
         {
            if (0.1*fac(k) > facmin)
               fac(k) = 0.1*fac(k);
            else
               fac(k) = facmin;
         }
      }
   }

   #ifdef DEBUG_JACOBIAN_EVAL
      MessageInterface::ShowMessage("fac:\n");
      MessageInterface::ShowMessage(fac.ToString() + "\n\n");
   #endif
}
