//------------------------------------------------------------------------------
//                           Sensor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2017.03.21
//
/**
 * Definition of the base Sensor class.  This class models a sensor.
 */
//------------------------------------------------------------------------------
#ifndef Sensor_hpp
#define Sensor_hpp

#include "gmatdefs.hpp"
#include "Rmatrix33.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"

class Sensor
{
public:
   
   /// class construction/destruction
   Sensor(Real angle1 = 0.0, Real angle2 = 0.0, Real angle3 = 0.0,
          Integer seq1 = 1, Integer seq2 = 2, Integer seq3 = 3);
   Sensor( const Sensor &copy);
   Sensor& operator=(const Sensor &copy);
   
   virtual ~Sensor();
   
   /// Set the sensor-to-body offset angles
   virtual void  SetSensorBodyOffsetAngles(
                        Real angle1 = 0.0, Real angle2 = 0.0, Real angle3 = 0.0,
                        Integer seq1 = 1, Integer seq2 = 2,   Integer seq3 = 3);
   /// Get the body-to-sensor matrix
   virtual Rmatrix33 GetBodyToSensorMatrix(Real forTime);
   
   //------------------------------------------------------------------------------
   // bool CheckTargetVisibility(Real viewConeAngle, Real viewClockAngle = 0.0)
   //------------------------------------------------------------------------------
   /**
    * Check the target visibility given the input cone and clock angles:
    * determines whether or not the point is in the sensor FOV.
    *
    * @param viewConeAngle  cone angle
    * @param viewClockAngle clock angle
    *
    * @return true if point is in the sensor FOV; false otherwise
    *
    * @note This method is pure virtual and MUST be implemented in child
    *       classes
    */
   //---------------------------------------------------------------------------
   virtual bool  CheckTargetVisibility(Real viewConeAngle,
                                       Real viewClockAngle = 0.0) = 0;
   
protected:
   
   /// The maximum excursion angle
   Real          maxExcursionAngle;
   
   /// Offset angles
   Real          offsetAngle1;
   Real          offsetAngle2;
   Real          offsetAngle3;
   
   /// Euler sequence
   Integer       eulerSeq1;
   Integer       eulerSeq2;
   Integer       eulerSeq3;
   
   /// The rotation matrix from the body frame to the sensor frame
   Rmatrix33     R_SB;
   
   /// Check the target maximum excursion angle
   virtual bool  CheckTargetMaxExcursionAngle(Real viewConeAngle);
   /// Compute the body-to-sensor matrix
   virtual void  ComputeBodyToSensorMatrix();
   
   /// Coordinate conversion utilities
   void ConeClocktoRADEC(Real coneAngle, Real clockAngle,
                         Real &RA, Real &dec);
   Rvector3 RADECtoUnitVec(Real RA, Real dec);
   void UnitVecToStereographic(const Rvector3 &u,
                               Real &xCoord, Real &yCoord);
   void ConeClockToStereographic(Real coneAngle, Real clockAngle,
                                 Real &xCoord, Real &yCoord);
   void ConeClockArraysToStereographic(const Rvector &coneAngleVec,
                                       const Rvector &clockAngleVec,
                                       Rvector &xArray, Rvector &yArray);

};
#endif // Sensor_hpp
