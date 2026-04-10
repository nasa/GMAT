//$Id$
//------------------------------------------------------------------------------
//                            BrentDekkerZero
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/04/10
//
/**
 * Implements interpolation class using the cubic spline algorithm described in
 * Numerical Recipes in C, 2nd Ed., pp 113 ff
 */
//------------------------------------------------------------------------------


#include "BrentDekkerZero.hpp"
#include <cmath>
#include "MessageInterface.hpp"


#define DEBUG_ZERO_FINDER


BrentDekkerZero::BrentDekkerZero() :
   macheps        (1.0e-15)
{
}

BrentDekkerZero::~BrentDekkerZero()
{
}


BrentDekkerZero::BrentDekkerZero(const BrentDekkerZero &bdz)
{
}


BrentDekkerZero&  BrentDekkerZero::operator=(const BrentDekkerZero &bdz)
{
   if (&bdz != this)
   {
      
   }
   
   return *this;
}


void BrentDekkerZero::SetInterval(Real a0, Real b0, Real fa0, Real fb0, 
      Real tolerance)
{
   a = a0;
   b = b0;
   fa = fa0;
   fb = fb0;
   t = tolerance;
   
   // Load up the c, fc, d, e variables
   SwapAC();
}


Real BrentDekkerZero::FindStep(Real lastStep, Real lastEval)
{
   b = lastStep;
   fb = lastEval;
   
   if (((fb > 0.0) && (fc > 0.0)) || ((fb <= 0.0) && (fc <= 0.0)))
      SwapAC();
   
   FindStepParameters();
   
   #ifdef DEBUG_ZERO_FINDER
      MessageInterface::ShowMessage("      a = %.12lf, f(a) = %.12lf"
            "\n      b = %.12lf, f(b) = %.12lf\n", a, fa, b, fb);
   #endif

   if ((fabs(e) < tol) || (fabs(fa) <= fabs(fb)))
   {
      #ifdef DEBUG_ZERO_FINDER
         MessageInterface::ShowMessage("   |e|(%.12lf) < tol(%.12lf)? %s\n",
               e, tol, ((fabs(e) < tol) ? "true" : "false"));
         MessageInterface::ShowMessage("   |fa|(%.12lf) < |fb|(%.12lf)? %s\n",
               fa, fb, ((fabs(fa) <= fabs(fb)) ? "true" : "false"));
      #endif
      d = e = m;
   }
   else
   {
      s = fb / fa;
      if (a == c)
      {
         // Linear interpolation
         #ifdef DEBUG_ZERO_FINDER
            MessageInterface::ShowMessage("   Using Linear Interp\n");
         #endif
         p = 2 * m * s;
         q = 1 - s;
      }
      else
      {
         // Inverse quadratic interpolation
         #ifdef DEBUG_ZERO_FINDER
            MessageInterface::ShowMessage("   Using Inverse Q Interp\n");
         #endif
         q = fa / fc;
         r = fb / fc;
         p = s * (2.0 * m * q * (q - r) - (b - a) * (r - 1));
         q = (q - 1) * (r - 1) * (s - 1);
      }
      
      if (p > 0.0)
         q = -q;
      else
         p = -p;
      s = e;
      e = d;
      
      if ((2.0 * p < (3.0 * m * q - fabs(tol * q))) && 
          (p < fabs(0.5 * s * q)))
         d = p / q;
      else
         d = e = m;
   }
   
   a = b;
   fa = fb;
   
   if (fabs(d) > tol)
      b += d;
   else
   {
      if (m > 0.0)
         b += tol;
      else
         b -= tol;
   }
   
   return b;
}


bool BrentDekkerZero::CheckConvergence()
{
#ifdef DEBUG_ZERO_FINDER
   MessageInterface::ShowMessage("Convergence Check\n");
   MessageInterface::ShowMessage("      m = %.12le >? tol = %.12le\n      "
         "      fb = %.12le\n", m, tol, fb);
#endif
   
   bool retval = (fabs(m) > tol) && (fabs(fb) != 0.0);
   return retval;
}


Real BrentDekkerZero::TestDriver(Real aVal, Real bVal, Real tVal)
{
   #ifdef DEBUG_ZERO_FINDER
      MessageInterface::ShowMessage("Testing Brent-Dekker Zero\n   Inputs:\n");
      MessageInterface::ShowMessage("      a = %.12lf\n      b = %.12lf\n      "
            "tol = %le\n", aVal, bVal, tVal);
   #endif
   
   Real fa0, fb0;
   fa0 = TestFunction(aVal);
   fb0 = TestFunction(bVal);

   #ifdef DEBUG_ZERO_FINDER
      MessageInterface::ShowMessage("      fa = %.12lf\n      fb = %.12lf\n", 
            fa0, fb0);
   #endif
   
   // 1.  Setup the internal data
   SetInterval(aVal, bVal, fa0, fb0, tVal);

   Real newVal = bVal, nextVal = bVal, fNext;
   Integer count = 0;
   while (CheckConvergence())
   {
      #ifdef DEBUG_ZERO_FINDER
         MessageInterface::ShowMessage("   Iteration %d:\n", ++count);
      #endif
      nextVal = newVal;
      fNext = TestFunction(nextVal);
      newVal = FindStep(nextVal, fNext);
   }
   
   #ifdef DEBUG_ZERO_FINDER
      MessageInterface::ShowMessage("   Converged to %.12lf\n", nextVal);
   #endif

   return b;
}

void BrentDekkerZero::SwapAC()
{
   c = a;
   fc = fa;
   d = e = b - a;
   #ifdef DEBUG_ZERO_FINDER
      MessageInterface::ShowMessage("SwapAC at end: a  = %.12lf b  = %.12lf  "
                                    "c = %.12lf\n            fa = %.12lf "
                                    "fb = %.12lf fc = %.12lf\n"
                                    "            d = %.12lf e = %.12lf\n",
                                    a, b, c, fa, fb, fc, d, e);
   #endif
}


void BrentDekkerZero::FindStepParameters()
{
   #ifdef DEBUG_ZERO_FINDER
      MessageInterface::ShowMessage("FSP: a  = %.12lf b  = %.12lf  c = %.12lf\n"
                                    "     fa = %.12lf fb = %.12lf "
                                    "fc = %.12lf\n", a, b, c, fa, fb, fc);
   #endif
   
   if (fabs(fc) < fabs(fb))
   {
      a = b;
      b = c;
      c = a;
      fa = fb;
      fb = fc;
      fc = fa;
   }
   tol = 2.0 * macheps * fabs(b) + t;
   m = 0.5 * (c - b);
   
   #ifdef DEBUG_ZERO_FINDER
      MessageInterface::ShowMessage("FSP at end: a  = %.12lf b  = %.12lf  "
                                    "c = %.12lf\n            fa = %.12lf "
                                    "fb = %.12lf fc = %.12lf\n", a, b, c, fa, 
                                    fb, fc);
      MessageInterface::ShowMessage("            tol = %.12lf  m = %.12lf\n"
                                    "=================\n", tol, m);
   #endif
}


#ifdef DEBUG_ZERO_FINDER
   Real BrentDekkerZero::TestFunction(Real x)
   {
      // Zero at about 0.7544
      return 3.0 * x * x * x - x * x + 7.0 * x - 6.0;  
   }
#endif
