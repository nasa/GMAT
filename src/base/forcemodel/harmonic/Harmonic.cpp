//$Id$
//------------------------------------------------------------------------------
//                           Harmonic
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
// Author: John P. Downing/GSFC/595
// Created: 2010.10.28
// (modified for style, etc. by Wendy Shoan/GSFC/583 2011.05.31)
// Tide fix, Cleaned up, April 2016 - John Downing
//
/**
 * This is the base class for the Harmonic set of classes (HarmonicGravity,
 * etc.).
 */
//------------------------------------------------------------------------------
#include <math.h>
#include "Harmonic.hpp"
#include "ODEModelException.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include "StringUtil.hpp"
//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
bool Harmonic::matrixTruncationWasPosted = false;
//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
Harmonic::Harmonic ()
   : NN         (0),
     MM         (0),
     FieldRadius(0.0),
     Factor     (0.0),
     C          (NULL),
     S          (NULL),
     A          (NULL),
     V          (NULL),
     Re         (NULL),
     Im         (NULL),
     N1         (NULL),
     N2         (NULL),
     VR01       (NULL),
     VR11       (NULL),
     VR02       (NULL),
     VR12       (NULL),
     VR22       (NULL)
   {
      theTimeConverter = TimeSystemConverter::Instance();
   }
//------------------------------------------------------------------------------
Harmonic::~Harmonic()
   {
   Deallocate();
   }
//------------------------------------------------------------------------------
Integer Harmonic::GetNN() const
   {
   return NN;
   }
//------------------------------------------------------------------------------
Integer Harmonic::GetMM() const
   {
   return MM;
   }
//------------------------------------------------------------------------------
Real Harmonic::GetFieldRadius() const
   {
   return FieldRadius;
   }
//------------------------------------------------------------------------------
Real Harmonic::GetFactor() const
   {
   return Factor;
   }
//------------------------------------------------------------------------------
void Harmonic::CalculateField (const Real& jday, const Real pos[3], 
   const Integer& nn, const Integer& mm, const bool& fillgradient,
   const Integer& gradientlimit, Real acc[3], Rmatrix33& gradient) const
   {
   Integer XS = fillgradient ? 2 : 1;
   // calculate vector components ----------------------------------
   Real r = sqrt (pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);    // Naming scheme from ref [3]
   Real s = pos[0]/r;
   Real t = pos[1]/r;
   Real u = pos[2]/r; // sin(phi), phi = geocentric latitude

   // Calculate values for A -----------------------------------------
   // generate the off-diagonal elements
   A[1][0] = u*sqrt(Real(3.0));
   for (Integer n=1;  n<=NN+XS && n<=nn+XS;  ++n)
      A[n+1][n] = u*sqrt(Real(2*n+3))*A[n][n];

   // apply column-fill recursion formula (Table 2, Row I, Ref.[1])
   for (Integer m=0;  m<=MM+XS && m<=mm+XS;  ++m)
      {
      for (Integer n=m+2;  n<=NN+XS && n<=nn+XS;  ++n)
         A[n][m] = u * N1[n][m] * A[n-1][m] - N2[n][m] * A[n-2][m];
      // Ref.[3], Eq.(24)
      Re[m] = m==0 ? 1 : s*Re[m-1] - t*Im[m-1]; // real part of (s + i*t)^m
      Im[m] = m==0 ? 0 : s*Im[m-1] + t*Re[m-1]; // imaginary part of (s + i*t)^m
      }

   // Now do summation ------------------------------------------------
   // initialize recursion
   Real rho = FieldRadius/r;
   Real rho_np1 = -Factor/r * rho;   // rho(0) ,Ref[3], Eq 26 , factor = mu for gravity
   Real rho_np2 = rho_np1 * rho;
   Real a1 = 0;
   Real a2 = 0;
   Real a3 = 0;
   Real a4 = 0;
   Real a11 = 0;
   Real a12 = 0;
   Real a13 = 0;
   Real a14 = 0;
   Real a23 = 0;
   Real a24 = 0;
   Real a33 = 0;
   Real a34 = 0;
   Real a44 = 0;
   Real sqrt2 = sqrt (Real(2)); 
   for (Integer n=1;  n<=NN && n<=nn;  ++n)
      {
      rho_np1 *= rho;
      rho_np2 *= rho;
      Real sum1 = 0;
      Real sum2 = 0;
      Real sum3 = 0;
      Real sum4 = 0;
      Real sum11 = 0;
      Real sum12 = 0;
      Real sum13 = 0;
      Real sum14 = 0;
      Real sum23 = 0;
      Real sum24 = 0;
      Real sum33 = 0;
      Real sum34 = 0;
      Real sum44 = 0;

      for (Integer m=0;  m <= n && m<=MM && m<=mm;  ++m) // wcs - removed "m<=n"
         {
         Real Cval = Cnm (jday,n,m);
         Real Sval = Snm (jday,n,m);
         // Pines Equation 27 (Part of)
         Real D =            (Cval*Re[m]   + Sval*Im[m]) * sqrt2;
         Real E = m==0 ? 0 : (Cval*Re[m-1] + Sval*Im[m-1]) * sqrt2;
         Real F = m==0 ? 0 : (Sval*Re[m-1] - Cval*Im[m-1]) * sqrt2;
         // Correct for normalization
         Real Avv00 = A[n][m];
         Real Avv01 = VR01[n][m] * A[n][m+1];
         Real Avv11 = VR11[n][m] * A[n+1][m+1];
         // Pines Equation 30 and 30b (Part of)
         sum1 += m * Avv00 * E;
         sum2 += m * Avv00 * F;
         sum3 +=     Avv01 * D;
         sum4 +=     Avv11 * D;

         // Truncate the gradient at GRADIENT_MAX x GRADIENT_MAX
         if (fillgradient)
            {
            if ((m <= gradientlimit) && (n <= gradientlimit))
               {
               // Pines Equation 27 (Part of)
               // 2015.09.18 GMT-5295 m<=2  -> m<=1
               Real G = m<=1 ? 0 : (Cval*Re[m-2] + Sval*Im[m-2]) * sqrt2;
               Real H = m<=1 ? 0 : (Sval*Re[m-2] - Cval*Im[m-2]) * sqrt2;
               // Correct for normalization
               Real Avv02 = VR02[n][m] * A[n][m+2];
               Real Avv12 = VR12[n][m] * A[n+1][m+2];
               Real Avv22 = VR22[n][m] * A[n+2][m+2];
               if (GmatMathUtil::IsNaN(Avv02) || GmatMathUtil::IsInf(Avv02))
                  Avv02 = 0.0;  // ************** wcs added ****

               // Pines Equation 36 (Part of)
               sum11 += m*(m-1) * Avv00 * G;
               sum12 += m*(m-1) * Avv00 * H;
               sum13 += m       * Avv01 * E;
               sum14 += m       * Avv11 * E;
               sum23 += m       * Avv01 * F;
               sum24 += m       * Avv11 * F;
               sum33 +=           Avv02 * D;
               sum34 +=           Avv12 * D;
               sum44 +=           Avv22 * D;
               }
            else
               {
               if (matrixTruncationWasPosted == false)
                  {
                  MessageInterface::ShowMessage("*** WARNING *** Gradient data "
                        "for the state transition matrix and A-matrix "
                        "computations are truncated at degree and order "
                        "<= %d.\n", gradientlimit);
                  matrixTruncationWasPosted = true;
                  }
               }
            } 
         }
      // Pines Equation 30 and 30b (Part of)
      Real rr = rho_np1/FieldRadius;
      a1 += rr*sum1;
      a2 += rr*sum2;
      a3 += rr*sum3;
      a4 -= rr*sum4;
      if (fillgradient)
         {
         // Pines Equation 36 (Part of)
         a11 += rho_np2/FieldRadius/FieldRadius*sum11;
         a12 += rho_np2/FieldRadius/FieldRadius*sum12;
         a13 += rho_np2/FieldRadius/FieldRadius*sum13;
         a14 -= rho_np2/FieldRadius/FieldRadius*sum14;
         a23 += rho_np2/FieldRadius/FieldRadius*sum23;
         a24 -= rho_np2/FieldRadius/FieldRadius*sum24;
         a33 += rho_np2/FieldRadius/FieldRadius*sum33;
         a34 -= rho_np2/FieldRadius/FieldRadius*sum34;
         a44 += rho_np2/FieldRadius/FieldRadius*sum44;
         #ifdef DEBUG_GRADIENT
         //   MessageInterface::ShowMessage("In Harmonic::CalField, fillgradient = %s\n", (fillgradient? "true" : "false"));
         //   MessageInterface::ShowMessage("a33   = %12.10f\n", a33);
         //   MessageInterface::ShowMessage("sum33 = %12.10f\n", sum33);
         //   MessageInterface::ShowMessage("u     = %12.10f\n", u);
         //   MessageInterface::ShowMessage("a44   = %12.10f\n", a44);
         //   MessageInterface::ShowMessage("a34   = %12.10f\n", a34);
         #endif
         }
      }

   // Pines Equation 31 
   acc[0] = a1+a4*s;
   acc[1] = a2+a4*t;
   acc[2] = a3+a4*u;
   if (fillgradient)
      {
      // Pines Equation 37
      gradient(0,0) =  a11 + s*s*a44 + a4/r + 2*s*a14;
      gradient(1,1) = -a11 + t*t*a44 + a4/r + 2*t*a24;
      gradient(2,2) =  a33 + u*u*a44 + a4/r + 2*u*a34;
      gradient(0,1) =
      gradient(1,0) =  a12 + s*t*a44 + s*a24 + t*a14;
      gradient(0,2) =
      gradient(2,0) =  a13 + s*u*a44 + s*a34 + u*a14;
      gradient(1,2) =
      gradient(2,1) =  a23 + t*u*a44 + u*a24 + t*a34;
      }
   }
//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
void Harmonic::Allocate()
   {
   AllocateArray(C,NN,0);
   AllocateArray(S,NN,0);
   AllocateArray(A,NN,3);
   AllocateArray(V,NN,3);
   AllocateArray(Re,NN,3);
   AllocateArray(Im,NN,3);
   AllocateArray(N1,NN,3);
   AllocateArray(N2,NN,3);
   AllocateArray(VR01,NN,0);
   AllocateArray(VR11,NN,0);
   AllocateArray(VR02,NN,0);
   AllocateArray(VR12,NN,0);
   AllocateArray(VR22,NN,0);

   // initialize the diagonal elements (not a function of the input)
   A[0][0] = 1.0;
   for (Integer n=1;  n<=NN+2;  ++n)
      A[n][n] = sqrt (Real(2*n+1)/Real(2*n)) * A[n-1][n-1];

   // Compute normalization coefficients V(n,m)     V(0..degree,0..order)
   //   V(n,0) = sqrt (2n+1)
   //   V(n,m) = sqrt (2(2n+1) * (n-m)! / (n+m)! )
   //   note that
   //       V(n,m) = V(n,m-1) / sqrt( (n+m) * (n-m+1) )
   for (Integer n=0;  n<=NN+2;  ++n)
      {
      V[n][0] = sqrt(Real(2*(2*n+1)));   // Temporary, to make following loop work
       for (Integer m=1;  m<=n+2 && m<=MM+2;  ++m)
         {
         V[n][m] = V[n][m-1] / sqrt(Real((n+m)*(n-m+1)));
         }
      V[n][0] = sqrt(Real(2*n+1));       // Now set true value
      }
   for (Integer n=0;  n<=NN;  ++n)
      for (Integer m=0;  m<=n && m<=MM;  ++m)
         {
         Real nn = n;
         VR01[n][m] = sqrt(Real((nn-m)*(nn+m+1)));
         VR11[n][m] = sqrt(Real((2*nn+1)*(nn+m+2)*(nn+m+1))/Real((2*nn+3)));
         VR02[n][m] = sqrt(Real((nn-m)*(nn-m-1)*(nn+m+1)*(nn+m+2))) ;
         VR12[n][m] = sqrt(Real(2*nn+1)/Real(2*nn+3)*Real((nn-m)*(nn+m+1)*(nn+m+2)*(nn+m+3)));
         VR22[n][m] = sqrt(Real(2*nn+1)/Real(2*nn+5)*Real((nn+m+1)*(nn+m+2)*(nn+m+3)*(nn+m+4)));
         if (m==0) 
            {
            VR01[n][m] /= sqrt(Real(2));
            VR11[n][m] /= sqrt(Real(2));
            VR02[n][m] /= sqrt(Real(2));
            VR12[n][m] /= sqrt(Real(2));
            VR22[n][m] /= sqrt(Real(2));
            }
         }

   for (Integer m=0;  m<=MM+2;  ++m)
      {
      for (Integer n=m+2;  n<=NN+2;  ++n)
         {
         N1[n][m] = sqrt (Real((2*n+1)*(2*n-1)) / Real((n-m)*(n+m)));
         N2[n][m] = sqrt (Real((2*n+1)*(n-m-1)*(n+m-1)) / 
                          Real((2*n-3)*(n+m)*(n-m)));
         }
      }
   }
//------------------------------------------------------------------------------
void Harmonic::Deallocate()
   {
   DeallocateArray(C,NN,0);
   DeallocateArray(S,NN,0);
   DeallocateArray(A,NN,3);
   DeallocateArray(V,NN,3);
   DeallocateArray(Re,NN,3);
   DeallocateArray(Im,NN,3);
   DeallocateArray(N1,NN,3);
   DeallocateArray(N2,NN,3);
   DeallocateArray(VR01,NN,0);
   DeallocateArray(VR11,NN,0);
   DeallocateArray(VR02,NN,0);
   DeallocateArray(VR12,NN,0);
   DeallocateArray(VR22,NN,0);
   }
//------------------------------------------------------------------------------
void Harmonic::AllocateArray(Real**& a, const Integer& nn, const Integer& excess)
   {
   // Allocate out to full m, regardless of M_FileOrder
   a = new Real*[nn+1+excess];
   if (!a)
      throw ODEModelException ("Harmonic::AllocateArray failed");
   for (Integer n=0;  n<=nn+1+excess-1;  ++n)
      {
      a[n] = new Real[nn+1+excess];   // wcs 2011.06.02 n -> nn
      if (!a[n])
         throw ODEModelException ("Harmonic::AllocateArray failed");
      for (Integer m=0;  m<=nn+1+excess-1;  ++m)   // wcs 2011.06.02  n -> nn
         a[n][m] = 0.0;
      }
   }
//------------------------------------------------------------------------------
void Harmonic::AllocateArray(Real*& a, const Integer& nn, const Integer& excess)
   {
   a = new Real[nn+1+excess];
   if (!a)
      throw ODEModelException ("Harmonic::AllocateArray failed");
   for (Integer n=0;  n<=nn+1+excess-1;  ++n)
      a[n] = 0.0;
   }
//------------------------------------------------------------------------------
void Harmonic::DeallocateArray(Real**& a, const Integer& nn, const Integer& excess)
   {
   if (a != NULL)
      {
      for (Integer n=0;  n<=nn+1+excess-1;  ++n)
         {
         if (a[n] != NULL)
            {
            delete[] a[n];
            a[n] = NULL;
            }
         }
      delete[] a;
      a = NULL;
      }
   }
//------------------------------------------------------------------------------
void Harmonic::DeallocateArray(Real*& a, const Integer& nn, const Integer& excess)
   {
   if (a != NULL)
      {
      delete[] a;
      a = NULL;
      }
   }
//------------------------------------------------------------------------------
