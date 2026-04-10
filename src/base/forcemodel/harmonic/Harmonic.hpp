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
//====================================================================
// Normalized Derived Associated Lengendre Polynomials (of the 1st kind)
// per the method of Ref.[1]. "Fully" normalized for compatibility with
// the EGM96 coefficients per Ref[2]
//
// REFERENCES:
// [1] Lundberg, J.B., and Schutz, B.E., "Recursion Formulas of Legendre
//     Functions for Use with Nonsingular Geopotential Models", Journal
//     of Guidance, Dynamics, and Control, Vol. 11, No.1, Jan.-Feb. 1988.
//
// [2] Heiskanen, W.A., and Moritz, H., "Physical Geodesy", W.H. Freeman
//     and Company, San Francisco, 1967.
//
// [3] Pines, S., "Uniform Representation of the Gravitational Potential
//     and its Derivatives", AIAA Journal, Vol. 11, No. 11, 1973.
//
// Steven Queen
// Goddard Space Flight Center
// Flight Dynamics Analysis Branch
// Steven.Z.Queen@nasa.gov
// February 28, 2003
//
// Modification History : 3/17/2003 - D. Conway, Thinking Systems, Inc.
// Under Contract:  P.O.  GSFC S-67521-G
//
// Modification History : May 2010 - John Downing, GSFC.
//====================================================================
#ifndef Harmonic_hpp
#define Harmonic_hpp
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Rmatrix33.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton

//------------------------------------------------------------------------------
class GMAT_API Harmonic
{
public:
   Harmonic();
private: // Copy protected
   Harmonic(const Harmonic& x);
   Harmonic& operator=(const Harmonic& x);
public:
   virtual ~Harmonic();

   virtual Real Cnm (const Real& jday, 
      const Integer& n, const Integer& m) const = 0;
   virtual Real Snm (const Real& jday, 
      const Integer& n, const Integer& m) const = 0;
   Integer GetNN() const;
   Integer GetMM() const;
   Real GetFieldRadius() const;
   Real GetFactor() const;
   void CalculateField(const Real& jday, const Real pos[3], 
       const Integer& nn, const Integer& mm, const bool& fillgradient, 
       const Integer& gradientlimit, Real acc[3], Rmatrix33& gradient) const;
//--------------------------------------------------------------------
protected:
   Integer     NN;      // Maximum value of n (Jn=J2,J3...)
   Integer     MM;      // Maximum value of m (Jnm=Jn2,Jn3...
   Real        FieldRadius;  // Radius for harmonic coefficients
   Real        Factor;  // Factor = 1 (magnetic) or -mu (gravity)
   Real**      C;       // Normalized harmonic coefficients
   Real**      S;       // Normalized harmonic coefficients
   Real**      A;       // Normalized 'derived' Assoc. Legendre Poly
   Real**      V;       // Normalization factor
   Real*       Re;      // powers of projection of pos onto x_ecf (re)
   Real*       Im;      // powers of projection of pos onto y_ecf (im)
   Real**      N1;      // Temporary
   Real**      N2;      // Temporary
   Real**      VR01;    // Temporary
   Real**      VR11;    // Temporary
   Real**      VR02;    // Temporary
   Real**      VR12;    // Temporary
   Real**      VR22;    // Temporary
   /// Flag used to warn about truncating matrix calculations to 20x20 only once
   static bool matrixTruncationWasPosted;

   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;
protected:
   void Allocate();
   void Deallocate();
protected:
   static void AllocateArray (Real**& a,   
      const Integer& nn, const Integer& excess);
   static void AllocateArray (Real*& a,    
      const Integer& nn, const Integer& excess);
   static void DeallocateArray (Real**& a, 
      const Integer& nn, const Integer& excess);
   static void DeallocateArray (Real*& a,  
      const Integer& nn, const Integer& excess);
//--------------------------------------------------------------------
};
//====================================================================
#endif // Harmonic_hpp
