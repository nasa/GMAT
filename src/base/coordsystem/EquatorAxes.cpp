//$Id$
//------------------------------------------------------------------------------
//                                  EquatorAxes
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/02/24
//
/**
 * Implementation of the EquatorAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "EquatorAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Planet.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "GmatConstants.hpp"
#include "TimeSystemConverter.hpp"
#include "Attitude.hpp"
#include "Rvector3.hpp"
#include "CoordinateSystemException.hpp"

//#define DEBUG_EQUATOR_AXES
//#define DEBUG_EQ_LUNA

#ifdef DEBUG_EQUATOR_AXES
   #include "MessageInterface.hpp"
#endif


using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeConstants;      // for JD offsets, etc.

//---------------------------------
// static data
//---------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  EquatorAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base EquatorAxes structures
 * (default constructor).
 *
 * @param itdName name of the EquatorAxes object.
 */
//------------------------------------------------------------------------------
EquatorAxes::EquatorAxes(const std::string &itsName) :
DynamicAxes("Equator",itsName)
{
   objectTypeNames.push_back("EquatorAxes");
   parameterCount = EquatorAxesParamCount;
   theDeFile = NULL;

   needsCBOrigin = true;
}

//------------------------------------------------------------------------------
//  EquatorAxes(const EquatorAxes &eqAxes);
//------------------------------------------------------------------------------
/**
 * Constructs base EquatorAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param eqAxes  EquatorAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
EquatorAxes::EquatorAxes(const EquatorAxes &eqAxes) :
DynamicAxes(eqAxes)
{
   theDeFile = NULL;
}

//------------------------------------------------------------------------------
//  EquatorAxes& operator=(const EquatorAxes &eqAxes)
//------------------------------------------------------------------------------
/**
 * Assignment operator for EquatorAxes structures.
 *
 * @param eqAxes The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const EquatorAxes& EquatorAxes::operator=(const EquatorAxes &eqAxes)
{
   if (&eqAxes == this)
      return *this;
   DynamicAxes::operator=(eqAxes);  
   theDeFile = NULL;
   return *this;
}

//------------------------------------------------------------------------------
//  ~EquatorAxes()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
EquatorAxes::~EquatorAxes()
{
}

//------------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "FK5") const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage EquatorAxes::UsesEopFile(const std::string &forBaseSystem) const
{
   if ((originName == GmatSolarSystemDefaults::EARTH_NAME) && (forBaseSystem == baseSystem)) return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}

//------------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesItrfFile() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage EquatorAxes::UsesItrfFile() const
{
   if (originName == GmatSolarSystemDefaults::EARTH_NAME) return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}

//------------------------------------------------------------------------------
// GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage EquatorAxes::UsesNutationUpdateInterval() const
{
   if (originName == GmatSolarSystemDefaults::EARTH_NAME) 
      return GmatCoordinate::REQUIRED;
   return DynamicAxes::UsesNutationUpdateInterval();
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this EquatorAxes.
 *
 */
//------------------------------------------------------------------------------
bool EquatorAxes::Initialize()
{
   DynamicAxes::Initialize();
   if (originName == GmatSolarSystemDefaults::EARTH_NAME) InitializeFK5();
   return true;
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the EquatorAxes.
 *
 * @return clone of the EquatorAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* EquatorAxes::Clone() const
{
   return (new EquatorAxes(*this));
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000Eq system
 *
 * @param atEpoch  epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it's not time to do it
 *
 */
//------------------------------------------------------------------------------
void EquatorAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                          bool forceComputation) 
{
   #ifdef DEBUG_EQUATOR_AXES
      MessageInterface::ShowMessage("Entering Equator::Calculate with epoch = %.12f\n",
         (Real) atEpoch.Get());
   #endif
   if (originName == GmatSolarSystemDefaults::EARTH_NAME)
   {
      Real dPsi             = 0.0;
      Real longAscNodeLunar = 0.0;
      Real cosEpsbar        = 0.0;
      
      // convert epoch (A1 MJD) to TT MJD (for calculations)
      Real mjdTT = theTimeConverter->Convert(atEpoch.Get(),
                   TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
                   GmatTimeConstants::JD_JAN_5_1941);      
      Real offset = GmatTimeConstants::JD_JAN_5_1941 - GmatTimeConstants::JD_OF_J2000;
      Real tTDB  = (mjdTT + offset) / GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;
      
      if (overrideOriginInterval) updateIntervalToUse = 
                                  ((Planet*) origin)->GetNutationUpdateInterval();
      else                        updateIntervalToUse = updateInterval;
      #ifdef DEBUG_EQUATOR_AXES
         MessageInterface::ShowMessage(
            "In Equator::Calculate tTDB = %.12f and updateIntervalToUse = %.12f\n",
            tTDB, updateIntervalToUse);
      #endif
      
      ComputePrecessionMatrix(tTDB, atEpoch);
      ComputeNutationMatrix(tTDB, atEpoch, dPsi, longAscNodeLunar, cosEpsbar,
                            forceComputation);
      
      Real PrecT[9] = {precData[0], precData[3], precData[6],
                       precData[1], precData[4], precData[7],
                       precData[2], precData[5], precData[8]};
      Real NutT[9] =  {nutData[0], nutData[3], nutData[6],
                       nutData[1], nutData[4], nutData[7],
                       nutData[2], nutData[5], nutData[8]};

      #ifdef DEBUG_EQUATOR_AXES
         MessageInterface::ShowMessage(
            "In Equator::Calculate precData = \n%.12f %.12f %.12f\n%.12f %.12f %.12f\n%.12f %.12f %.12f\n",
            precData[0],precData[1],precData[2],precData[3],precData[4],
            precData[5],precData[6],precData[7],precData[8]);
         MessageInterface::ShowMessage(
            "In Equator::Calculate nutData = \n%.12f %.12f %.12f\n%.12f %.12f %.12f\n%.12f %.12f %.12f\n",
            nutData[0],nutData[1],nutData[2],nutData[3],nutData[4],
            nutData[5],nutData[6],nutData[7],nutData[8]);
      #endif
   
      Integer p3 = 0;
      Real res[3][3];
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            res[p][q] = PrecT[p3]   * NutT[q]   + 
                        PrecT[p3+1] * NutT[q+3] + 
                        PrecT[p3+2] * NutT[q+6];
         }
      }     
      rotMatrix.Set(res[0][0],res[0][1],res[0][2],
                    res[1][0],res[1][1],res[1][2],
                    res[2][0],res[2][1],res[2][2]); 
      
      #ifdef DEBUG_EQUATOR_AXES
         MessageInterface::ShowMessage(
            "In Equator::rotMatrix is set to = \n%.12f %.12f %.12f\n%.12f %.12f %.12f\n%.12f %.12f %.12f\n",
            rotMatrix(0,0),rotMatrix(0,1),rotMatrix(0,2),
            rotMatrix(1,0),rotMatrix(1,1),rotMatrix(1,2),
            rotMatrix(2,0),rotMatrix(2,1),rotMatrix(2,2));
      #endif
      // rotDotMatrix is still the default zero matrix 
      // (assume it is negligibly small)
      
   }
   else if ((originName == GmatSolarSystemDefaults::MOON_NAME) && 
           (((CelestialBody*)origin)->GetRotationDataSource() == Gmat::DE_FILE))
   {
      #ifdef DEBUG_EQ_LUNA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "Entering Luna Equator calculations (DE file is source)\n");
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      if (!theDeFile)
      {
         theDeFile = (DeFile*) ((CelestialBody*)origin)->GetSourceFile();         
         if (!theDeFile)
         {
            throw CoordinateSystemException(
               "No DE file specified - cannot get Moon data");
            // should we automatically switch to IAU data here?
            // De file is initialized in its constructor
         }
      }
      bool override = ((CelestialBody*)origin)->GetOverrideTimeSystem();
      Real librationAngles[3], andRates[3];
      theDeFile->GetAnglesAndRates(atEpoch, librationAngles, andRates, override);
      
      #ifdef DEBUG_EQ_LUNA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage("Luna Equator: override flag = %s\n",
         (override? "true" : "false"));
         MessageInterface::ShowMessage(
         "Luna Equator: libration angles from DE are: %.16f  %.16f  %.16f\n",
         librationAngles[0], librationAngles[1], librationAngles[2]);
         MessageInterface::ShowMessage(
         "Luna Equator: rates from DE are: %.16f  %.16f  %.16f\n",
         andRates[0], andRates[1], andRates[2]);
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      Real ca1    = GmatMathUtil::Cos(librationAngles[0]);
      Real ca2    = GmatMathUtil::Cos(librationAngles[1]);
      Real sa1    = GmatMathUtil::Sin(librationAngles[0]);
      Real sa2    = GmatMathUtil::Sin(librationAngles[1]);
      // transposed R3(theta1)
      Real R3_ang1_T[9] = {ca1, -sa1, 0.0, sa1, ca1, 0.0, 0.0, 0.0, 1.0};
      // transposed R1(theta2)
      Real R1_ang2_T[9] = {1.0, 0.0, 0.0, 0.0, ca2, -sa2, 0.0, sa2, ca2};
      // transposed R1Dot(theta2)
      Real R1Dot_ang2_T[9] = {0.0,                0.0,                0.0, 
                              0.0, -andRates[1] * sa2, -andRates[1] * ca2,
                              0.0,  andRates[1] * ca2, -andRates[1] * sa2};
      // transposed R3Dot(theta1)
      Real R3Dot_ang1_T[9] = {-andRates[0] * sa1, -andRates[0] * ca1, 0.0,
                               andRates[0] * ca1, -andRates[0] * sa1, 0.0,
                                             0.0,                0.0, 0.0};
      // rotMatrix = R3_ang1_T * R1_ang2_T
      Real R3R1[3][3];
      Integer p3 = 0;
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            R3R1[p][q] = R3_ang1_T[p3]   * R1_ang2_T[q]   + 
                         R3_ang1_T[p3+1] * R1_ang2_T[q+3] + 
                         R3_ang1_T[p3+2] * R1_ang2_T[q+6];
         }
      }     
      rotMatrix.Set(R3R1[0][0],R3R1[0][1],R3R1[0][2],  
                    R3R1[1][0],R3R1[1][1],R3R1[1][2],
                    R3R1[2][0],R3R1[2][1],R3R1[2][2]); 
      
      //rotDotmatrix = R3_ang1_T * R1Dot_ang2_T + R3Dot_ang1_T * R1_ang2_T
      Real R3R1Dot[3][3];
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            R3R1Dot[p][q] = R3_ang1_T[p3]   * R1Dot_ang2_T[q]   + 
                            R3_ang1_T[p3+1] * R1Dot_ang2_T[q+3] + 
                            R3_ang1_T[p3+2] * R1Dot_ang2_T[q+6];
         }
      }   
      Real R3DotR1[3][3];
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            R3DotR1[p][q] = R3Dot_ang1_T[p3]   * R1_ang2_T[q]   + 
                            R3Dot_ang1_T[p3+1] * R1_ang2_T[q+3] + 
                            R3Dot_ang1_T[p3+2] * R1_ang2_T[q+6];
         }
      }   
      
      rotDotMatrix.Set(R3R1Dot[0][0] +  R3DotR1[0][0],
                       R3R1Dot[0][1] +  R3DotR1[0][1],
                       R3R1Dot[0][2] +  R3DotR1[0][2],
                       R3R1Dot[1][0] +  R3DotR1[1][0],
                       R3R1Dot[1][1] +  R3DotR1[1][1],
                       R3R1Dot[1][2] +  R3DotR1[1][2],
                       R3R1Dot[2][0] +  R3DotR1[2][0],
                       R3R1Dot[2][1] +  R3DotR1[2][1],
                       R3R1Dot[2][2] +  R3DotR1[2][2]
         );
    }
   else  // use IAU data for all other bodies, and Luna (if DE file not selected)
   {
      #ifdef DEBUG_EQ_LUNA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage("Equator for body %s with NO DE_FILE source ...\n",
         originName.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      // this method will return alpha (deg), delta (deg), 
      // W (deg), and Wdot (deg/day)
      static Rvector cartCoord(4);  
      const Real *cartC = cartCoord.GetDataVector();
      // this method will return alpha (deg), delta (deg), 
      // W (deg), and Wdot (deg/day)
      cartCoord   = ((CelestialBody*)origin)->
                      GetBodyCartographicCoordinates(atEpoch);
      
      Real rot1           = GmatMathConstants::PI_OVER_TWO + Rad(cartC[0]);
      Real rot2           = GmatMathConstants::PI_OVER_TWO - Rad(cartC[1]);
      // Don't care about W and Wdot here
      Real R3leftT[9] =  {Cos(rot1),-Sin(rot1),0.0,
                          Sin(rot1), Cos(rot1),0.0,
                                0.0,       0.0,1.0};
      Real R1middleT[9] =  {1.0,      0.0,       0.0,
                            0.0,Cos(rot2),-Sin(rot2),
                            0.0,Sin(rot2), Cos(rot2)};
      Integer p3 = 0;
      Real    rot[3][3];
      for (Integer p = 0; p < 3; ++p)
      {
         p3 = 3*p;
         for (Integer q = 0; q < 3; ++q)
         {
            rot[p][q] = R3leftT[p3]   * R1middleT[q]   + 
                        R3leftT[p3+1] * R1middleT[q+3] + 
                        R3leftT[p3+2] * R1middleT[q+6];
         }
      }   
      rotMatrix.Set(rot[0][0], rot[0][1], rot[0][2],
                    rot[1][0], rot[1][1], rot[1][2],
                    rot[2][0], rot[2][1], rot[2][2]);  
   }
}
