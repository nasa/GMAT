//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravity
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
 * This is the base class for the HarmonicGravity set of classes
 * (HarmonicGravityCof, etc.).
 */
//------------------------------------------------------------------------------
#ifndef HarmonicGravity_hpp
#define HarmonicGravity_hpp
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Harmonic.hpp"
#include "Rmatrix33.hpp"
//------------------------------------------------------------------------------
const Integer LoveMax = 4;
//------------------------------------------------------------------------------
class GMAT_API HarmonicValue {
public:
   HarmonicValue ();
   HarmonicValue (const Integer& n, const Integer& m, 
      const Real& c, const Real& s);
public:
   Integer N;
   Integer M; 
   Real    C;
   Real    S;
};
//------------------------------------------------------------------------------
class GMAT_API HarmonicGravity : public Harmonic {
public:
   HarmonicGravity 
     (const std::string& filename, const std::string& tideFilename, const Real& radius,
      const Real& mukm, const std::string& bodyname, const bool& loadCoefficients);
private: // Copy protected
   HarmonicGravity (const HarmonicGravity& hg);
   HarmonicGravity& operator=(const HarmonicGravity& hg);
public:
   virtual ~HarmonicGravity();

   std::string GetFilename();
   std::string GetTideFilename();
   std::string GetBodyName();
   bool HaveTideModel (const int& etide);
   bool IsTideFree();
   bool IsZeroTide();
   std::string TideString();
   virtual Real Cnm(const Real& jday, const Integer& n, const Integer& m) const;
   virtual Real Snm(const Real& jday, const Integer& n, const Integer& m) const;
   void CalculatePointField(const Real& jday, const Real pos[3],
      const Integer& nn, const Integer& mm,
      const bool& fillgradient,  const Integer& gradientlimit,
      Real  acc[3], Rmatrix33& gradient);
   void CalculateFullField(const Real& jday, const Real pos[3],
      const Integer& nn, const Integer& mm, const Integer& tidelevel, 
      const Real sunpos[3], const Real& sunmukm, 
      const Real otherpos[3], const Real& othermukm,
      const Real &xp, const Real &yp,
      const bool& fillgradient,  const Integer& gradientlimit,
      Real acc[3], Rmatrix33& gradient);

   void AddZeroTide (const Integer& n, const Integer& m, 
      const Real& c, const Real& s);
   void WriteCofFile (const std::string& filename);

   enum ETide { NoTide,Solid,SolidAndPole };
   static const std::string ETideString[3];
   static const Integer ETideCount;

private: 
   std::string Filename;
   std::string TideFilename;
   std::string BodyName;
   std::string ModelName;
   bool Normalized;
   bool HaveZeroTide;         // In C,s
   bool HaveTideFree;         // In CTideFree,STideFree
   bool HaveLoveNumbers;      // In K,KPlus
   Integer TideLevel;   // Temporary during full field call

   // Tide Free coefficients
   Integer  ZeroTideMax;
   std::vector<HarmonicValue> ZeroTideValues;
   // Love Numbers
   Real   K[LoveMax+1][LoveMax+1];   
   Real   KPlus[LoveMax+1];
   // Variable Coefficients (Temporary)
   Real   DeltaC[LoveMax+1][LoveMax+1];  // Temporary during full field call
   Real   DeltaS[LoveMax+1][LoveMax+1];  // Temporary during full field call

   // Methods useful in Tide computations
   void ClearDeltaCS ();
   void IncrementSolidTide (const Real pos[3], const Real& mukm);
   void IncrementEarthTide (const Real &jday, 
      const Real sunpos[3], const Real& sunmukm, 
      const Real otherpos[3], const Real& othermukm,
      const Real &xp, const Real &yp);
   // Load Module
   void LM_Error (const std::string& error);
   void LM_TideError (const std::string& error);
   void LM_SetNN (const std::string& st);
   void LM_SetMM (const std::string& st);
   void LM_SetMu (const std::string& st);
   void LM_SetNormalized (const std::string& st);
   void LM_SetFieldRadius (const std::string& st);
   void LM_SetCoefficients (const StringArray& sa, 
      const bool& zerotidevalue); 
   void LM_SetCoefficients (const std::string& line, 
      const bool& zerotidevalue); 
   void LM_SetDefaultEarthTide ();
   void LM_Load (const bool& loadcoefficients);
   void LM_Load (std::string& filename, const bool& loadcoefficients);
   void LM_LoadCof (std::ifstream& instream, const bool& loadcoefficients);
   void LM_LoadGfc (std::ifstream& instream, const bool& loadcoefficients);
   void LM_LoadGrv (std::ifstream& instream, const bool& loadcoefficients);
   void LM_LoadTab (std::ifstream& instream, const bool& loadcoefficients);
   void LM_LoadTide (std::string& filename, const bool& loadcoefficients);
   void CheckEarthCoefficient ();
};
//------------------------------------------------------------------------------
#endif // HarmonicGravity_hpp
