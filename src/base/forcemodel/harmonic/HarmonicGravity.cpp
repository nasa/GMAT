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
#include "HarmonicGravity.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "StringUtil.hpp"
#include "FileUtil.hpp"
#include "RealUtilities.hpp"
#include "TimeSystemConverter.hpp"
#include "SolarSystem.hpp"
//------------------------------------------------------------------------------
using namespace GmatMathUtil;
//------------------------------------------------------------------------------
HarmonicValue::HarmonicValue ()
   : N(0),
     M(0),
     C(0),
     S(0)
   {
   }
//------------------------------------------------------------------------------
HarmonicValue::HarmonicValue (const Integer& n, const Integer& m, 
      const Real& c, const Real& s)
   : N(n),
     M(m),
     C(c),
     S(s)
   {
   }
//------------------------------------------------------------------------------
HarmonicGravity::HarmonicGravity 
  (const std::string& filename, const std::string& tideFilename, const Real& radius,
   const Real& mukm, const std::string& bodyname, const bool& loadCoefficients)
   : Harmonic (),
     Filename (filename),
     TideFilename (tideFilename),
     BodyName (bodyname),
     ModelName (""),
     Normalized (true),
     HaveTideFree (true),
     HaveZeroTide (false),
     HaveLoveNumbers (false),
     TideLevel (0),
     ZeroTideMax (0),
     ZeroTideValues (0)
   {
   for (int i=0;  i<=LoveMax;  ++i)
      for (int j=0;  j<=LoveMax;  ++j)
         K[i][j] = 0;
   for (int i=0;  i<=LoveMax;  ++i)
      KPlus[i] = 0;
   FieldRadius = radius;
   Factor = -mukm;
   LM_Load (loadCoefficients);
   }
//------------------------------------------------------------------------------
HarmonicGravity::~HarmonicGravity()
   {
   }
//------------------------------------------------------------------------------
std::string HarmonicGravity::GetFilename()
   {
   return Filename;
   }
//------------------------------------------------------------------------------
std::string HarmonicGravity::GetTideFilename()
   {
   return TideFilename;
   }
//------------------------------------------------------------------------------
std::string HarmonicGravity::GetBodyName()
   {
   return BodyName;
   }
//------------------------------------------------------------------------------
bool HarmonicGravity::HaveTideModel (const int& etide)
   {
   switch (etide) {
      case NoTide:           return true;
      case Solid:          return HaveLoveNumbers;
      case SolidAndPole:   return (HaveLoveNumbers && BodyName == GmatSolarSystemDefaults::EARTH_NAME);
      default:             return false;
      }
   }
//------------------------------------------------------------------------------
bool HarmonicGravity::IsTideFree()
   {
   return HaveTideFree;
   }
//------------------------------------------------------------------------------
bool HarmonicGravity::IsZeroTide()
   {
   return HaveZeroTide;
   }
//------------------------------------------------------------------------------
std::string HarmonicGravity::TideString()
   {
   std::string out = "";
   if (HaveZeroTide && HaveTideFree)
      out = "Any Tide";
   else if (HaveZeroTide)
      out = "Zero Tide";
   else if (HaveTideFree)
      out = "Tide Free";

   if (K[2][0] != 0 || K[2][1] != 0 || K[2][2] != 0)
      out += ", K2";
   if (K[3][0] != 0 || K[3][1] != 0 || K[3][2] != 0 || K[3][3] != 0)
      out += " K3";
   if (KPlus[0] != 0 || KPlus[1] != 0 || KPlus[2] != 0)
      out += " K+";
   return out;
   }
//------------------------------------------------------------------------------
Real HarmonicGravity::Cnm (const Real& jday, const Integer& n, const Integer& m) const
   {
   if ((n <= LoveMax) && (m <= LoveMax) && (TideLevel > 0))
      {
      return C[n][m] + DeltaC[n][m];
      }
   else
      {
      return C[n][m];
      }
   }
//------------------------------------------------------------------------------
Real HarmonicGravity::Snm (const Real& jday, const Integer& n, const Integer& m) const
   {
   if ((n <= LoveMax) && (m <= LoveMax) && (TideLevel > 0))
      {
      return S[n][m] + DeltaS[n][m];
      }
   else
      {
      return S[n][m];
      }
   }
//------------------------------------------------------------------------------
void HarmonicGravity::CalculatePointField (const Real& jday, const Real pos[3],
   const Integer& nn, const Integer& mm,
   const bool& fillgradient, const Integer& gradientlimit,
   Real  acc[3], Rmatrix33& gradient)
   {
   Real r = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);
   if (r == 0)
      r = 0.01;   // was0,01 - is this correct?
   Real mu_r_3 = (-Factor) / (r * r * r);   // Factor = -mu
   // Calculate acceleration
   for (Integer i=0;  i<=2;  ++i)
      acc[i] = -mu_r_3 * pos[i];
   // Calculate gradient
   if (fillgradient)
      {
      for (Integer i=0;  i<=2;  ++i)
         for (Integer j=0;  j<=2;  ++j)
            {
            gradient(i,j) = 3*mu_r_3 * pos[i]/r * pos[j]/r; 
            if (i==j)
               gradient(i,j) += -mu_r_3;
            }
      }
   #ifdef DEBUG_GRADIENT
      MessageInterface::ShowMessage("In CalPtF, fillgradient = %s\n", (fillgradient? "true" : "false"));
      MessageInterface::ShowMessage("gradientPoint = %s\n", gradient.ToString().c_str());
   #endif
   }
//------------------------------------------------------------------------------
void HarmonicGravity::CalculateFullField (const Real& jday, const Real pos[3],
   const Integer& nn, const Integer& mm, const Integer& tidelevel, 
   const Real sunpos[3], const Real& sunmukm, 
   const Real otherpos[3], const Real& othermukm,
   const Real &xp, const Real &yp, 
   const bool& fillgradient, const Integer& gradientlimit, 
   Real acc[3], Rmatrix33& gradient)
   {
   TideLevel = tidelevel;
   ClearDeltaCS ();
   if (tidelevel >= 2 && BodyName == GmatSolarSystemDefaults::EARTH_NAME)
      IncrementEarthTide(jday,sunpos,sunmukm,otherpos,othermukm,xp,yp);
   else if (tidelevel >= 1)
      {
      IncrementSolidTide (sunpos,sunmukm);
      if (othermukm > 0)
         IncrementSolidTide (otherpos,othermukm);
      }
   Real      accpoint[3];
   Real      accharmonic[3];
   Rmatrix33 gradientpoint;
   Rmatrix33 gradientharmonic;
   CalculatePointField(jday,pos,nn,mm,fillgradient,gradientlimit,accpoint,gradientpoint);
   CalculateField(jday,pos,nn,mm,fillgradient,gradientlimit,accharmonic,gradientharmonic);
   for (Integer i=0;  i<=2;  ++i)
      acc[i] = accpoint[i] + accharmonic[i];
   if (fillgradient)
      gradient = gradientpoint + gradientharmonic;
   #ifdef DEBUG_GRADIENT
      MessageInterface::ShowMessage("In CalFullField, fillgradient = %s\n", (fillgradient? "true" : "false"));
      MessageInterface::ShowMessage("gradient = %s\n", gradient.ToString().c_str());
   #endif
   }
//------------------------------------------------------------------------------
void HarmonicGravity::AddZeroTide (const Integer& n, const Integer& m, 
   const Real& c, const Real& s)
   {
   ZeroTideMax = GmatMathUtil::Max (ZeroTideMax,n);
   ZeroTideValues.push_back(HarmonicValue(n,m,c,s));
   }
//------------------------------------------------------------------------------
void HarmonicGravity::WriteCofFile (const std::string& filename)
   {
   std::ofstream f;
   f.open(filename.c_str());  // = std::ofstream(filename);
   f << "COMMENT   5" << std::endl;
   f << std::string(80,'C') << std::endl;
   f << "CCCCC  " << std::string(66,'-') << "  CCCCC" << std::endl;
   f << "CCCCC  " << ModelName << std::string(66-ModelName.size(),' ') << "  CCCCC" << std::endl;
   f << "CCCCC  " << std::string(66,'-') << "  CCCCC" << std::endl;
   f << std::string(80,'C') << std::endl;
   
   f << "POTFIELD" << GmatStringUtil::ToString (NN,3) << GmatStringUtil::ToString (MM,3);
   f << GmatStringUtil::ToString (BodyName == GmatSolarSystemDefaults::EARTH_NAME,3);
   f << GmatStringUtil::ToString (-Factor*1e9,false,true,true,14,21);
   f << GmatStringUtil::ToString (FieldRadius*1e3,false,true,true,14,21);
   f << GmatStringUtil::ToString (1,false,true,true,14,21);
   f << std::endl;

   for (int n=2;  n<=NN;  ++n)
      for (int m=0;  m<=MM&&m<=n;  ++m)
         {
         f << "RECOEF  " << GmatStringUtil::ToString (n,3) << GmatStringUtil::ToString (m,3);
         f << "   ";
         f << GmatStringUtil::ToString (C[n][m],false,true,true,14,21);
         if (m != 0)
            f << GmatStringUtil::ToString (S[n][m],false,true,true,14,21);
         f << std::endl;
         }
   f.close();
   }
//------------------------------------------------------------------------------
const std::string HarmonicGravity::ETideString[3] = { "None", "Solid", "SolidAndPole" };
const Integer HarmonicGravity::ETideCount = 3;
//==============================================================================
// Tide Module
//==============================================================================
// static data for earth tide model
//------------------------------------------------------------------------------
// Love numbers, k(n,m), IERS p.71, Table 6.3
// K         = elastic Love numbers
static const Real KEarth[LoveMax+1][LoveMax+1] =
{ { 0.0,     0.0,     0.0,     0.0   },   
  { 0.0,     0.0,     0.0,     0.0   },
//  { 0.29525, 0.29470, 0.29801, 0.0   },
  { 0.30190, 0.29830, 0.30102, 0.0   },
  { 0.093,   0.093,   0.093,   0.094 } };
// KPlus    = elastic Love numbers
static const Real KPlusEarth[LoveMax+1] =
  { -0.00087, -0.00079, -0.00057, 0 };  
//------------------------------------------------------------------------------
// Table 6.3a, IERS p.64 (for C21,S21 coefficients)
//  l   l'  F   D   O     ip    op
static const Integer Table63aDim1 = 48;
static const Real Table63a[Table63aDim1][7] =
{ { 2,  0,  2,  0,  2,   -0.1,  0   },
  { 0,  0,  2,  2,  2,   -0.1,  0   },
  { 1,  0,  2,  0,  1,   -0.1,  0   },
  { 1,  0,  2,  0,  2,   -0.7,  0.1 },
  {-1,  0,  2,  2,  2,   -0.1,  0   },
  { 0,  0,  2,  0,  1,   -1.3,  0.1 },
  { 0,  0,  2,  0,  2,   -6.8,  0.6 },
  { 0,  0,  0,  2,  0,    0.1,  0   },
  { 1,  0,  2, -2,  2,    0.1,  0   },
  {-1,  0,  2,  0,  1,    0.1,  0   },
  {-1,  0,  2,  0,  2,    0.4,  0   },
  { 1,  0,  0,  0,  0,    1.3, -0.1 },
  { 1,  0,  0,  0,  1,    0.3,  0   },
  {-1,  0,  0,  2,  0,    0.3,  0   },
  {-1,  0,  0,  2,  1,    0.1,  0   },
  { 0,  1,  2, -2,  2,   -1.9,  0.1 },
  { 0,  0,  2, -2,  1,    0.5,  0   },
  { 0,  0,  2, -2,  2,  -43.4,  2.9 },
  { 0, -1,  2, -2,  2,    0.6,  0   },
  { 0,  1,  0,  0,  0,    1.6, -0.1 },
  {-2,  0,  2,  0,  1,    0.1,  0   },
  { 0,  0,  0,  0, -2,    0.1,  0   },
  { 0,  0,  0,  0, -1,   -8.8,  0.5 },
  { 0,  0,  0,  0,  0,  470.9,-30.2 },
  { 0,  0,  0,  0,  1,   68.1, -4.6 },
  { 0,  0,  0,  0,  2,   -1.6,  0.1 },
  {-1,  0,  0,  1,  0,    0.1,  0   },
  { 0, -1,  0,  0, -1,   -0.1,  0   },
  { 0, -1,  0,  0,  0,  -20.6, -0.3 },
  { 0,  1, -2,  2, -2,    0.3,  0   },
  { 0, -1,  0,  0,  1,   -0.3,  0   },
  {-2,  0,  0,  2,  0,   -0.2,  0   },
  {-2,  0,  0,  2,  1,   -0.1,  0   },
  { 0,  0, -2,  2, -2,   -5.0,  0.3 },
  { 0,  0, -2,  2, -1,    0.2,  0   },
  { 0, -1, -2,  2, -2,   -0.2,  0   },
  { 1,  0,  0, -2,  0,   -0.5,  0   },
  { 1,  0,  0, -2,  1,   -0.1,  0   },
  {-1,  0,  0,  0, -1,    0.1,  0   },
  {-1,  0,  0,  0,  0,   -2.1,  0.1 },
  {-1,  0,  0,  0,  1,   -0.4,  0   },
  { 0,  0,  0, -2,  0,   -0.2,  0   },
  {-2,  0,  0,  0,  0,   -0.1,  0   },
  { 0,  0, -2,  0, -2,   -0.6,  0   },
  { 0,  0, -2,  0, -1,   -0.4,  0   },
  { 0,  0, -2,  0,  0,   -0.1,  0   },
  {-1,  0, -2,  0, -2,   -0.1,  0   },
  {-1,  0, -2,  0, -1,   -0.1,  0   } };
//------------------------------------------------------------------------------
// Table 6.3b, IERS p.66 (for C20 coefficient)
//     l   l'  F   D   O     ip    op
static const Integer Table63bDim1 = 21;
static const Real Table63b[Table63bDim1][7] =
{ { 0,  0,  0,  0,  1,   16.6, -6.7 },
  { 0,  0,  0,  0,  2,   -0.1,  0.1 },
  { 0, -1,  0,  0,  0,   -1.2,  0.8 },
  { 0,  0, -2,  2, -2,   -5.5,  4.3 },
  { 0,  0, -2,  2, -1,    0.1, -0.1 },
  { 0, -1, -2,  2, -2,   -0.3,  0.2 },
  { 1,  0,  0, -2,  0,   -0.3,  0.7 },
  {-1,  0,  0,  0, -1,    0.1, -0.2 },
  {-1,  0,  0,  0,  0,   -1.2,  3.7 },
  {-1,  0,  0,  0,  1,    0.1, -0.2 },
  { 1,  0, -2,  0, -2,    0.1, -0.2 },
  { 0,  0,  0, -2,  0,    0.0,  0.6 },
  {-2,  0,  0,  0,  0,    0.0,  0.3 },
  { 0,  0, -2,  0, -2,    0.6,  6.3 },
  { 0,  0, -2,  0, -1,    0.2,  2.6 },
  { 0,  0, -2,  0,  0,    0.0,  0.2 },
  { 1,  0, -2, -2, -2,    0.1,  0.2 },
  {-1,  0, -2,  0, -2,    0.4,  1.1 },
  {-1,  0, -2,  0, -1,    0.2,  0.5 },
  { 0,  0, -2, -2, -2,    0.1,  0.2 },
  {-2,  0, -2,  0, -2,    0.1,  0.1 } };
//------------------------------------------------------------------------------
// Table 6.3c, IERS p.66 (for C22 coefficient)
//  l   l'  F   D   O     amp
static const Integer Table63cDim1 = 2;  
static const Real Table63c[Table63cDim1][6] =
{ { 1,  0,  2,  0,  2,  -0.3 },
  { 0,  0,  2,  0,  2,  -1.2 } };
//------------------------------------------------------------------------------
// Static Functions
//------------------------------------------------------------------------------
static void CartesianToPolar(const Real pos[3], Real out[3])
   {
   // R,Theta,Phi == R,Latitude,Longitude
   Real xy = Sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
   out[0]  = Sqrt(xy*xy + pos[2]*pos[2]);
   out[1]  = ATan2(pos[2],xy);
   out[2]  = ATan2(pos[1],pos[0]);
   }
//------------------------------------------------------------------------------
static void PolarToLegendre(const Real polar[3], Real P[5][5])
   {
   // First initialize
   for (Integer i = 0; i < 5; i++)
      for (Integer j = 0; j < 5; j++)
         P[i][j] = 0.0;
   Real s           = Sin(polar[1]);
   Real c           = Cos(polar[1]);
   try
      {
      Real sqrt5       = sqrt(5.0);
      Real sqrt5over3  = sqrt(5.0/3.0);
      Real sqrt7       = sqrt(7.0);
      Real sqrt7over6  = sqrt(7.0/6.0);
      Real sqrt7over15 = sqrt(7.0/15.0);
      Real sqrtPoint7  = sqrt(0.7);
      // P(2,0)
      P[2][0] = sqrt5*(1.5*s*s-0.5);
      P[2][1] = 3.0*sqrt5over3*c*s;
      P[2][2] = 1.5*sqrt5over3*c*c;
      P[3][0] = sqrt7*(2.5*s*s*s-1.5*s);
      P[3][1] = sqrt7over6*c*(7.5*s*s-1.5);
      P[3][2] = 7.5*sqrt7over15*c*c*s;
      P[3][3] = 2.5*sqrtPoint7*c*c*c;
      }
   catch (BaseException &be)
      {
      MessageInterface::ShowMessage("error computing sqrt.  Message is:\n");
      MessageInterface::ShowMessage("%s\n", (be.GetFullMessage()).c_str());
      }
   }
//------------------------------------------------------------------------------
void HarmonicGravity::ClearDeltaCS ()
   {
   for (int n=0;  n<=LoveMax;  ++n)
      for (int m=0;  m<=LoveMax;  ++m)
         {
         DeltaC[n][m] = 0;
         DeltaS[n][m] = 0;
         }
   }
//------------------------------------------------------------------------------
void HarmonicGravity::IncrementSolidTide (const Real pos[3], const Real& mukm)
   {
   Real massratio    = -mukm/Factor;      // factor is minus body mukm
   Real polar[3];    // R,Latitude,Longitude (Radians)
   CartesianToPolar (pos,polar);
   Real poly[5][5];
   PolarToLegendre (polar,poly);
   //------------------------------------------------------------------------------
   // eqn 1, p.59
   // eqn 4, p.60 (4th degree coefficients)
   for (Integer n=2;  n<=3;  ++n)
      for (Integer m=0;  m<=n;  ++m)  // should this be m <= 2????
         {
         Real f  = massratio*Pow(FieldRadius/polar[0],n+1)*poly[n][m];
         DeltaC[n][m] += K[n][m]/(2*n+1) * (f*Cos(m*polar[2]));
         DeltaS[n][m] += K[n][m]/(2*n+1) * (f*Sin(m*polar[2]));
         if (n==2)
            {
            DeltaC[4][m] += KPlus[m]/(2*n+1) * (f*Cos(m*polar[2]));
            DeltaS[4][m] += KPlus[m]/(2*n+1) * (f*Sin(m*polar[2]));
            }
         }
   }
//------------------------------------------------------------------------------
void HarmonicGravity::IncrementEarthTide (const Real& jday, 
   const Real sunpos[3], const Real& sunmukm, 
   const Real otherpos[3], const Real& othermukm,
   const Real &xp, const Real &yp)

   {
   // Solid Earth Tide Model
   // Ref: IERS Technical Note 32, Chapter 6 (updated version, 22 March 2006)
   // Blair F. Thompson
   // last modified: 16 FEB 2009
   // function [dC,dS] = solid_tides(JD,r_moon,r_sun,xp,yp,perm_tide_flag,norm_coeff_flag,k,k_plus,table_63a,table_63b,table_63c,Re)
   // OUTPUT:
   //   dC = 5x5 matrix of the C gravity coefficient deltas
   //   dS = 5x5 matrix of the S gravity coefficient deltas
   //
   // INPUT:
   //   jday = (days) julian date
   //   moonpos = (meters) in earth centered fixed (ECF) coordinates (NOT ECI!)
   //   sunpos  = (meters) in earth centered fixed (ECF) coordinates (NOT ECI!)
   //   xp,yp  = (arcsec) polar motion parameters
   //   perm_tide_flag  = 1 if the gravity coefficient C20 includes the permanent tide
   //                  0 if it does not
   //   norm_coeff_flag = 1 if the gravity coefficients are normalized
   //                  0 if not

   // table_63a = data from TechNote 32, p.64
   // table_63b = data from TechNote 32, p.66
   // table_63c = data from TechNote 32, p.66

   IncrementSolidTide (sunpos,sunmukm);
   IncrementSolidTide (otherpos,othermukm);
   // IERS Step 3 (correct for permanent tide if needed, IERS p.66)
   // This has been moved to model setup, and correction to TideFree coefficients
   

   //------------------------------------------------------------------------------
   // TechNote32 Step 2, p.60
   // compute GMST (or add it as a function input)
   //   Real JD = jday + 2300000.0;  // what is this number and why is it added on???  wcs
   // jday is A1 JD; we want UT1 (approximated by UTC) JD
   Real a1mjd  = jday - GmatTimeConstants::JD_JAN_5_1941;
   Real JD     = theTimeConverter->Convert(a1mjd, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
                 GmatTimeConstants::JD_JAN_5_1941) + GmatTimeConstants::JD_JAN_5_1941;
   Real t  = (JD-GmatTimeConstants::JD_OF_J2000)/GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;  // (ignore difference between TDB and TDT)
   Real t2 = t*t;
   Real t3 = t2*t;
   Real t4 = t3*t;
   Real GMST = 67310.54841 + 3164400184.812866*t + 0.093104*t2 - 6.2E-06*t3; // seconds of time
   GMST /= 240.0; // degrees

   // Fundamental arguments of nutation theory (Delaunay variables)
   // Tech Note 32 p.48
   Real F[5];
   F[0] = 134.96340251*3600 + 1717915923.2178*t + 31.8792*t2 + 0.051635*t3 - 0.00024470*t4;
   F[0] /= 3600;  // deg

   F[1] = 357.52910918*3600 + 129596581.0481*t - 0.5532*t2 + 0.000136*t3 - 0.00001149*t4;
   F[1] /= 3600;  // deg

   F[2] = 93.27209062*3600 + 1739527262.8478*t - 12.7512*t2 - 0.001037*t3 + 0.00000417*t4;
   F[2] /= 3600;  // deg

   F[3] = 297.85019547*3600 + 1602961601.2090*t - 6.3706*t2 + 0.006593*t3 - 0.00003169*t4;
   F[3] /= 3600;  // deg

   F[4] = 125.04455501*3600 - 6962890.5431*t + 7.4722*t2 + 0.007702*t3 - 0.00005939*t4;
   F[4] /= 3600;  // deg

   // compute (2,0) freq dependent terms, IERS eqn 5a, p.60, (n=2,m=0)
   Real freq_dep_C20 = 0;
   
   for (Integer f=0;  f<Table63bDim1;  ++f)
      {
      Real theta_f = 0;
      for (Integer j=0;  j<=4;  ++j)
         theta_f += Table63b[f][j]*F[j];  // p.61
      theta_f = -theta_f * GmatMathConstants::RAD_PER_DEG; // radians
      freq_dep_C20 += (Table63b[f][5]*Cos(theta_f)-Table63b[f][6]*Sin(theta_f)); // eqn 5a
      }
   DeltaC[2][0] += freq_dep_C20 * 1e-12;

   // compute (2,1) freq dependent terms, IERS eqn 5b, p.60, (n=2,m=1)
   Real freq_dep_C21 = 0;
   Real freq_dep_S21 = 0;
   Integer m = 1;

   for (Integer f=0;  f<Table63aDim1;  ++f)
      {
      Real theta_f = 0;
      for (Integer j=0;  j<=4;  ++j)
         theta_f += Table63a[f][j]*F[j];
      theta_f = (m*(GMST+GmatMathConstants::PI_DEG)-theta_f) * GmatMathConstants::RAD_PER_DEG; // radians
      freq_dep_C21 += Table63a[f][5]*Sin(theta_f)+Table63a[f][6]*Cos(theta_f); // eqn 5b
      freq_dep_S21 += Table63a[f][5]*Cos(theta_f)-Table63a[f][6]*Sin(theta_f); // eqn 5b
      }
   DeltaC[2][1] += freq_dep_C21 * 1e-12;
   DeltaS[2][1] += freq_dep_S21 * 1e-12;

   // compute (2,2) freq dependent terms, IERS eqn 5b, p.60, (n=2,m=2)
   Real freq_dep_C22 = 0;
   Real freq_dep_S22 = 0;
   m = 2;

   for (Integer f=0;  f<Table63cDim1;  ++f)
      {
      Real theta_f = 0;
      for (Integer j=0;  j<=4;  ++j)
         theta_f += Table63c[f][j]*F[j];
      theta_f = (m*(GMST+GmatMathConstants::PI_DEG)-theta_f) * GmatMathConstants::RAD_PER_DEG; // radians
      freq_dep_C22 += ( Table63c[f][5]*Cos(theta_f));
      freq_dep_S22 += (-Table63c[f][5]*Sin(theta_f));
      }

   DeltaC[2][2] += freq_dep_C22 * 1e-12;
   DeltaS[2][2] += freq_dep_S22 * 1e-12;

   // solid earth pole tide, IERS p.65
   // Commented out unless we have xp and yp data

   Real ym2000 = (JD-GmatTimeConstants::JD_OF_J2000)/GmatTimeConstants::DAYS_PER_YEAR;  // years past 2000
   Real xp_bar = 0.054 + ym2000*0.00083;  // IERS p. 84
   Real yp_bar = 0.357 + ym2000*0.00395;  // IERS p. 84

   Real m1 =   xp-xp_bar;
   Real m2 = -(yp-yp_bar);

   DeltaC[2][1] -= 1.333E-09*(m1+0.0115*m2);
   DeltaS[2][1] -= 1.333E-09*(m2-0.0115*m1);

   // ocean pole tide (TechNote 32 working version, section 6.3, p.10)
   DeltaC[2][1] -= 2.2344E-10*(m1-0.01737*m2);
   DeltaS[2][1] -= 1.7680E-10*(m2-0.03351*m1);

   #ifdef DEBUG_TIDE
      MessageInterface::ShowMessage("Tide coefficients:\n");
      for (Integer n=0;  n<5;  ++n)   // wcs < instead of <=
         for (Integer m=0;  m<=n;  ++m)
            {
            MessageInterface::ShowMessage("   C[%d][%d] = %le, S[%d][%d] = %le\n", n, m,
               CTide[n][m], n, m, STide[n][m]);
            }
   #endif      
   }
//------------------------------------------------------------------------------
//--- Load Module
//------------------------------------------------------------------------------
void HarmonicGravity::LM_Error (const std::string& error)
   {
   throw GravityFileException ("File \"" + Filename + "\" " + error);
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_TideError (const std::string& error)
   {
   throw GravityFileException ("Tide file \"" + TideFilename + "\" " + error);
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_SetNN (const std::string& st)
   {
   Integer ii = 0;
   if (GmatStringUtil::ToInteger(st,ii))
      NN = ii;
   else
      LM_Error ("Value for Degree (" + st + ") is not an integer");
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_SetMM (const std::string& st)
   {
   Integer ii = 0;
   if (GmatStringUtil::ToInteger(st,ii))
      MM = ii;
   else
      LM_Error ("Value for Order (" + st + ") is not an integer");
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_SetMu (const std::string& st)
   {
   Real rr = 0;
   if (GmatStringUtil::ToReal(st,rr))
      Factor = -rr/1e9;
   else
      LM_Error ("Value for Mu (" + st + ") is not an real number");
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_SetFieldRadius (const std::string& st)
   {
   Real rr = 0;
   if (GmatStringUtil::ToReal(st,rr))
      FieldRadius = rr/1e3;
   else
      LM_Error ("Value for Field Radius (" + st + ") is not an real number");
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_SetNormalized (const std::string& st)
   {
   Real rr = 0;
   if (GmatStringUtil::ToReal(st,rr))
      Normalized = rr > 0;
   else
      LM_Error ("Value for Normalized Flag (" + st + ") is not an real number");
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_SetCoefficients (const StringArray& sa,
   const bool& zerotidevalue)
   {
   if (sa.size() == 0) return; // Blank line
   if (sa[0] == "") return; // Blank line
   bool good = true;
   Integer m,n;
   Real c,s;
   if (sa.size() < 3) LM_Error ("Too few items (" + 
      GmatStringUtil::ToString((int)sa.size()) + ") in coefficient line");
   good = good && GmatStringUtil::ToInteger(sa[0], n);
   good = good && GmatStringUtil::ToInteger(sa[1], m);
   good = good && GmatStringUtil::ToReal(sa[2], c);
   if (!good) LM_Error ("Conversion Error");
   if (n > NN) LM_Error ("n is greater than NN");
   if (m > MM) LM_Error ("m is greater than MM");
   s = 0;
   if (m > 0)
      {
      if (sa.size() < 4) LM_Error ("Too few items");
      good = good && GmatStringUtil::ToReal(sa[3], s);
      if (!good) LM_Error("Conversion Error");
      }
   if (!Normalized)
      {
      if (V[n][m] != 0)
         {
         c /= V[n][m];
         s /= V[n][m];
         }
      else
         {
         c = 0;
         s = 0;
         }
      }
   if (zerotidevalue)
      AddZeroTide (n,m,c,s);
   else
      {
      C[n][m] = c;
      S[n][m] = s;
      }
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_SetCoefficients (const std::string& line,
   const bool& zerotidevalue)
   {
   StringArray sa = GmatStringUtil::SeparateBy (line," ");
   LM_SetCoefficients (sa,zerotidevalue);
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_SetDefaultEarthTide ()
   {
   if (!HaveLoveNumbers && BodyName == GmatSolarSystemDefaults::EARTH_NAME)
      {
      for (int n=0;  n<=LoveMax;  ++n)
         for (int m=0;  m<=LoveMax;  ++m)
            K[n][m] = KEarth[n][m];
      for (int m=0;  m<=LoveMax;  ++m)
         KPlus[m] = KPlusEarth[m];
      CheckEarthCoefficient();
      HaveLoveNumbers = true;  
      }
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_Load (const bool& loadcoefficients)
   {
   LM_Load (Filename,loadcoefficients);
   if (TideFilename.find(".tide") != std::string::npos)
   {
       LM_LoadTide(TideFilename, loadcoefficients);
   }
   LM_SetDefaultEarthTide ();
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_Load (std::string& filename, const bool& loadcoefficients)
   {
   std::ifstream instream (filename.c_str());
   if (!instream.good())
      {
      MessageInterface::ShowMessage("Cannot open gravity file \"" + filename + "\"");
      throw GravityFileException("Cannot open gravity file \"" + filename + "\"");
      }

   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
   bool testMode = runmode == GmatGlobal::TESTING || runmode == GmatGlobal::TESTING_NO_PLOTS;
   std::string filenameLower = GmatStringUtil::ToLower(filename);

   if      (filenameLower.find (".cof") != std::string::npos)
      LM_LoadCof (instream,loadcoefficients);
   else if (filenameLower.find (".grv") != std::string::npos)
      LM_LoadGrv (instream,loadcoefficients);
   else if (testMode)
      {
      if      (filenameLower.find (".gfc") != std::string::npos)
         LM_LoadGfc (instream,loadcoefficients);
      else if (filenameLower.find (".tab") != std::string::npos)
         LM_LoadTab (instream,loadcoefficients);
      }
   instream.close();
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_LoadCof (std::ifstream& instream, const bool& loadcoefficients)
   {
   int pos1 = Filename.rfind ("\\");
   int pos2 = GmatStringUtil::ToLower(Filename).find (".cof");
   ModelName = Filename.substr (pos1+1,pos2-pos1-1);

   std::string line;
   while (!instream.eof())
      {
      GmatFileUtil::GetLine(&instream, line);
      // ignore comment lines
      if (line[0] != 'C')
         {
         std::string firstStr = line.substr(0, 8);
         firstStr = GmatStringUtil::Trim(firstStr);

         if (firstStr == "END") break;
         if (firstStr == "99999") break;  // sometimes this is EOF marker
         if (firstStr == "POTFIELD")
            {
            LM_SetNN (line.substr (8,3));
            LM_SetMM (line.substr (11,3));
            StringArray sa = GmatStringUtil::SeparateBy (line.substr(14)," ");
            LM_SetMu (sa[1]);
            LM_SetFieldRadius (sa[2]);
            LM_SetNormalized (sa[3]);
            if (loadcoefficients) 
               Allocate ();
            }
         else if (firstStr == "RECOEF")
            {
            if (!loadcoefficients) 
               break;
            StringArray sa = StringArray (4,"");
            sa[0] = line.substr(8, 3);
            sa[1] = line.substr(11, 3);
            sa[2] = line.substr(17, 21);
            if (line.size() > 38)
               sa[3] = line.substr(38, 21);
            LM_SetCoefficients (sa,false);
            }
         }
      }
   CheckEarthCoefficient();
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_LoadGfc (std::ifstream& instream, const bool& loadcoefficients)
   {
   std::string line;
   bool tidefree = true;
   
   while (!instream.eof())
      {
      GmatFileUtil::GetLine(&instream, line);
      StringArray sa = GmatStringUtil::SeparateBy (line," ");
      if (sa.size() >=2)
         {
         if (sa[0] == "modelname")
            ModelName = sa[1];
         else if (sa[0] == "earth_gravity_constant")
            LM_SetMu (sa[1]);
         else if (sa[0] == "gravity_constant")
            LM_SetMu (sa[1]);
         else if (sa[0] == "radius")
            LM_SetFieldRadius (sa[1]);
         else if (sa[0] == "max_degree")
            {
            LM_SetNN (sa[1]);
            LM_SetMM (sa[1]);
            }
         else if (sa[0] == "norm")
            Normalized = sa[1] == "fully_normalized";
         else if (sa[0] == "tide_system")
            tidefree = (sa[1] == "tide_free");
         else if ((sa[0] == "gfc" || sa[0] == "gfct") && sa.size() >= 5)
            {
            if (loadcoefficients)
               {
               if (C == NULL)
                  Allocate();
               sa.erase(sa.begin());
               LM_SetCoefficients (sa,false);
               }
            else
               break;
            }
         }
      }
   if (tidefree)
      {
      HaveTideFree = true;
      HaveZeroTide = ZeroTideValues.size() > 0;
      }
   else
      {
      HaveTideFree = false;
      HaveZeroTide = true;
      }
   CheckEarthCoefficient();
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_LoadGrv(std::ifstream& instream, const bool& loadcoefficients)
   {
   std::string rawLine;
   GmatFileUtil::GetLine(&instream, rawLine);  // Read first line -- stk.v.n.n

   bool includespermtide = true;
   bool insimpletidemodel = false;
   bool intidefreevalues = false;
   bool inzerotidevalues = false;
   bool incoefficients = false;
   bool inlovenumbers = false;
   bool insecularvariations = false;
   bool inprocessnoise = false;
   bool isAllocated = false;
   bool isDegreeSet = false;
   bool isOrderSet = false;

   while (!instream.eof())
      {
      // If we have information required to allocate do so if not already done.            
      if (!isAllocated && isDegreeSet && isOrderSet)
      {
         Allocate();
         isAllocated = true;
      }

      // Read the next line, and respond accoring to line type
      GmatFileUtil::GetLine(&instream, rawLine);
      rawLine = GmatStringUtil::ToUpper(rawLine);
      std::string tabFreeline = GmatStringUtil::RemoveAll(rawLine, "\t");
      if (GmatStringUtil::IsBlank(tabFreeline, true))
         ;  // blank line
      else if (tabFreeline.find("#") == 0)
         ;  // Comment
      else if (tabFreeline.find("BEGIN SIMPLETIDEMODEL") == 0)
         insimpletidemodel = true;
      else if (tabFreeline.find("END SIMPLETIDEMODEL") == 0)
         insimpletidemodel = false;
      else if (tabFreeline.find("BEGIN TIDEFREEVALUES") == 0)
         intidefreevalues = true;
      else if (tabFreeline.find("END TIDEFREEVALUES") == 0)
         intidefreevalues = false;
      else if (tabFreeline.find("BEGIN ZEROTIDEVALUES") == 0)
         inzerotidevalues = true;
      else if (tabFreeline.find("END ZEROTIDEVALUES") == 0)
         inzerotidevalues = false;
      else if (tabFreeline.find("BEGIN LOVENUMBERS") == 0)
         inlovenumbers = true;
      else if (tabFreeline.find("END LOVENUMBERS") == 0)
         inlovenumbers = false;
      else if (tabFreeline.find("BEGIN SECULARVARIATIONS") == 0)
         insecularvariations = true;
      else if (tabFreeline.find("END SECULARVARIATIONS") == 0)
         insecularvariations = false;
      else if (tabFreeline.find("BEGIN PROCESSNOISE") == 0)
         inprocessnoise = true;
      else if (tabFreeline.find("END PROCESSNOISE") == 0)
         inprocessnoise = false;
      else if (tabFreeline.find("BEGIN COEFFICIENTS") == 0)
         incoefficients = true;
      else if (tabFreeline.find("END COEFFICIENTS") == 0)
         incoefficients = false;
      else
         {
         StringArray lineStringArray = GmatStringUtil::SeparateBy(rawLine, "\t ");
         if (incoefficients)
            {
            if (loadcoefficients && lineStringArray.size() >=3)
                {
                LM_SetCoefficients(lineStringArray, false);
                }
            }
         else if (intidefreevalues)
            ;
         else if (inzerotidevalues)
            {
            if (lineStringArray.size() >= 3)
               LM_SetCoefficients(lineStringArray, true);
            }
         else if (insecularvariations)
            ;
         else if (inprocessnoise)
            ;
         else if (lineStringArray.size() >= 2)
            {
            std::string keyword = GmatStringUtil::ToUpper(lineStringArray[0]);
            Real rr = 0;
            bool isreal = GmatStringUtil::ToReal(lineStringArray[1], rr);
            std::string yn = GmatStringUtil::ToUpper(lineStringArray[1]);
            bool bb = yn == "YES";
            bool isbool = yn == "YES" || yn == "NO";
            if (keyword == "MODEL")
                ModelName = lineStringArray[1];
            else if (keyword == "DEGREE"){
                LM_SetNN(lineStringArray[1]);
                isDegreeSet = true;
            }
            else if (keyword == "ORDER"){
                LM_SetMM(lineStringArray[1]);
                isOrderSet = true;
            }
            else if (keyword == "GM")
                LM_SetMu(lineStringArray[1]);
            else if (keyword == "REFDISTANCE")
                LM_SetFieldRadius(lineStringArray[1]);
            else if (keyword == "NORMALIZED" && isbool)
               Normalized = bb;
            else if (keyword == "INCLUDESPERMTIDE" && isbool)
               includespermtide = bb;
            else if (keyword == "K2" && isreal && insimpletidemodel)
               {
               HaveLoveNumbers = true;
               for (int i=0;  i<=2;  ++i)
                  K[2][i] = rr;
               }
            else if (keyword == "K3" && isreal && insimpletidemodel)
               {
               HaveLoveNumbers = true;
               for (int i=0;  i<=3;  ++i)
                  K[3][i] = rr;
               }
            }
         }
      }
   if (includespermtide)
      {
      HaveTideFree = false;
      HaveZeroTide = true;
      }
   else
      {
      HaveTideFree = true;
      HaveZeroTide = ZeroTideValues.size() > 0;
      }
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_LoadTab (std::ifstream& instream, const bool& loadcoefficients)
   {
   int pos1 = Filename.rfind ("\\");
   int pos2 = GmatStringUtil::ToLower(Filename).find (".tab");
   ModelName = Filename.substr (pos1+1,pos2-pos1-1);

   std::string line;
   GmatFileUtil::GetLine(&instream, line);
   StringArray sa = GmatStringUtil::SeparateBy (line,", ");
   if (sa.size() < 6)
      throw GravityFileException
         ("File \"" + Filename + "\" has to few numbers on first line");

   LM_SetFieldRadius (sa[0]);
   FieldRadius *= 1e3;  // Correct correction
   LM_SetMu (sa[1]);
   Factor *= 1e9;  // Correct correction
   LM_SetNN (sa[3]);
   LM_SetMM (sa[4]);
   LM_SetNormalized (sa[5]);

   if (loadcoefficients)
      {
      Allocate();
      while (!instream.eof())
         {
         GmatFileUtil::GetLine(&instream, line);
         sa = GmatStringUtil::SeparateBy (line,", ");
         LM_SetCoefficients (sa,false);
         }
      }
   CheckEarthCoefficient();
   }
//------------------------------------------------------------------------------
void HarmonicGravity::LM_LoadTide(std::string& filename, const bool& loadcoefficients)
   {
   std::ifstream instream (filename.c_str());
   if (!instream.good())
   {
      MessageInterface::ShowMessage("Cannot open tide file \"" + filename + "\"");
      throw GravityFileException("Cannot open tide file \"" + filename + "\"");
   }

   std::string line;

   while (!instream.eof())
      {
      GmatFileUtil::GetLine(&instream, line);
      line = GmatStringUtil::ToLower(line);
      if (GmatStringUtil::IsBlank(line, true))
         ;  // blank line
      else if (line.find("%") == 0)
         ;  // Comment
      else
         {
         StringArray sa = GmatStringUtil::SeparateBy (line,"\t ");
         if (sa.size() >= 2)
            {
            std::string keyword = sa[0];
            if (keyword == "k")
               {
               if (sa.size () >= 4)
                  {
                  bool good = true;
                  Integer n;
                  good = good && GmatStringUtil::ToInteger (sa[1],n);
                  Integer m;
                  good = good && GmatStringUtil::ToInteger (sa[2],m);
                  Real v;
                  good = good && GmatStringUtil::ToReal (sa[3],v);
                  if (!good)
                     LM_Error ("Bad number in " + line);
                  if (n > 3 || m > 3 || n < 0 || m < 0)
                     LM_Error ("n or m is out of range in " + line);
                  else 
                     {
                     HaveLoveNumbers = true;
                     K[n][m] = v;
                     }
                  }
               }
            else if (keyword == "kplus")
               {
               if (sa.size () >= 3)
                  {                  bool good = true;
                  Integer m;
                  good = good && GmatStringUtil::ToInteger (sa[1],m);
                  Real v;
                  good = good && GmatStringUtil::ToReal (sa[2],v);
                  if (!good)
                     LM_Error ("Bad number in " + line);
                  if (m > 2 || m < 0)
                     LM_Error ("m is out of range in " + line);
                  else 
                     {
                     HaveLoveNumbers = true;
                     KPlus[m] = v;
                     }
                  }
               }
            else
               LM_TideError ("Unknown keyword \"" + keyword + "\" in line \"" + line + "\".");
            }
         else
            LM_TideError ("Unknown line \"" + line + "\".");
         }
      }
   }
//------------------------------------------------------------------------------
// CheckEarthCoefficient()
//------------------------------------------------------------------------------
/**
 * Checks the Earth C[2][0] coefficient to see if it is for a tide-free or
 * zero-tide model. This does not need to be checked if the file format
 * of the gravitational potential identifies if it is tide-free or zero tide.
 *
 * This function is not to be called on file formats which provide both
 * tide-free and zero-tide coefficients in the same file, as this function
 * will mark the coefiicients as exclusively either tide-free or zero-tide.
 *
 * For more information, see the IERS Conventions (2010) Section 6.2.2
 */
//------------------------------------------------------------------------------
void HarmonicGravity::CheckEarthCoefficient()
{
   if (BodyName == GmatSolarSystemDefaults::EARTH_NAME && C != NULL && NN >= 2)
   {
      // Special case for Earth
      bool tidefreemodel = C[2][0] > -4.84167E-04;
      HaveTideFree = tidefreemodel;
      HaveZeroTide = !tidefreemodel;
   }
}
