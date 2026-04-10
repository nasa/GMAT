//------------------------------------------------------------------------------
//                           Spacecraft
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
// Created: 2016.05.02
//
/**
 * Definition of the Spacecraft class.  This class contains data and
 * methods for a simple Spacecraft.
 */
//------------------------------------------------------------------------------
#ifndef Spacecraft_hpp
#define Spacecraft_hpp

#include "gmatdefs.hpp"
#include "OrbitState.hpp"
#include "AbsoluteDate.hpp"
#include "Sensor.hpp"
#include "Attitude.hpp"
#include "LagrangeInterpolator.hpp"
#include "Rvector6.hpp"

class Spacecraft
{
public:
   
   /// class construction/destruction
   // class methods
   Spacecraft(AbsoluteDate *epoch, OrbitState *state, Attitude *att,
              LagrangeInterpolator *interp,
              Real angle1 = 0.0, Real angle2 = 0.0, Real angle3 = 0.0,
              Integer seq1 = 1, Integer seq2 = 2, Integer seq3 = 3);
   Spacecraft( const Spacecraft &copy);
   Spacecraft& operator=(const Spacecraft &copy);
   
   virtual ~Spacecraft();
   
   /// Get the orbit state
   virtual OrbitState*    GetOrbitState();
   /// Get the orbit epoch
   virtual AbsoluteDate*  GetOrbitEpoch();
   /// Get the Julian date
   virtual Real           GetJulianDate();
   /// Get the current cartesian state
   virtual Rvector6       GetCartesianState();
   /// Add a sensor to the spacecraft
   virtual void           AddSensor(Sensor* sensor);
   /// Does this spacecraft have sensors?
   virtual bool           HasSensors();
   /// Set the drag area
   virtual void           SetDragArea(Real area);
   /// Set the drag coefficient
   virtual void           SetDragCoefficient(Real Cd);
   /// Set the total mass
   virtual void           SetTotalMass(Real mass);
   /// Set the attitude for the spacecraft
   virtual void           SetAttitude(Attitude *att);
   /// Get the drag area
   virtual Real           GetDragArea();
   // Get the drag coefficient
   virtual Real           GetDragCoefficient();
   /// Get the toal mass
   virtual Real           GetTotalMass();
   
   /// This method returns the interpolated MJ2000 Cartesian state
   virtual Rvector6       GetCartesianStateAtEpoch(const AbsoluteDate &atDate);
   
   /// Check the target visibility given the input cone and clock angles for
   /// the input sensor number
   virtual bool           CheckTargetVisibility(Real    targetConeAngle,
                                                Real    targetClockAngle,
                                                Integer sensorNumber);

   /// Check the target visibility given the input body fixed state and
   /// spacecraft-to-target vector, at the input time, for the input
   /// sensor number
   virtual bool           CheckTargetVisibility(const Rvector6 &bodyFixedState,
                                                const Rvector3 &satToTargetVec,
                                                Real            atTime,
                                                Integer         sensorNumber);

   /// Get the body-fixed-to-inertial rotation matrix
   virtual Rmatrix33      GetBodyFixedToInertial(const Rvector6 &bfState);
   
   /// Add an orbit state (Keplerian elements) for the spacecraft at the input
   /// time t
   virtual bool           SetOrbitState(const AbsoluteDate &t,
                                        const Rvector6 &kepl);
   
   /// Set the body nadir offset angles for the spacecraft
   virtual void           SetBodyNadirOffsetAngles(
                              Real angle1 = 0.0, Real angle2 = 0.0,
                              Real angle3 = 0.0,
                              Integer seq1 = 1, Integer seq2 = 2,
                              Integer seq3 = 3);
   
   /// Can the orbit be interpolated - i.e. are there enough points, etc.?
   virtual bool           CanInterpolate(Real atTime);
   
   /// Is it time to interpolate?  i.e. are there enough points? if so, what is
   /// the midpoint of the independent variable lower/upper range
   virtual bool           TimeToInterpolate(Real atTime, Real &midRange);
   
   /// Interpolate the data to the input toTime
   virtual Rvector6       Interpolate(Real toTime);
   
protected:
   
   /// Drag coefficient
	Real                 dragCoefficient;
   /// Drag area in m^2
	Real                 dragArea;
    /// Total Mass in kg
	Real                 totalMass;
	/// Orbit State
	OrbitState           *orbitState;
	/// Orbit Epoch
	AbsoluteDate         *orbitEpoch;
	/// Number of attached sensors
	Integer              numSensors;
	/// Vector of attached sensor objects
	std::vector<Sensor*> sensorList;
   /// Pointer to the Attitude object
   Attitude             *attitude;
   /// The interpolator to use (for Hermite only, currently)
   LagrangeInterpolator *interpolator;
   /// Offset angles
   Real                 offsetAngle1;
   Real                 offsetAngle2;
   Real                 offsetAngle3;
   
   /// Euler sequence
   Integer              eulerSeq1;
   Integer              eulerSeq2;
   Integer              eulerSeq3;
   
   /// The rotation matrix from the nadir frame to the body frame
   Rmatrix33            R_BN;
   
   
   /// @todo - do we need to buffer states here as well??
   
   /// Convert inertial view vector to cone and clock angles
   virtual void  InertialToConeClock(const Rvector3 &viewVec,
                                     Real &cone,
                                     Real &clock);
   /// Compute the nadir-to-body-matrix
   virtual void  ComputeNadirToBodyMatrix();

};
#endif // Spacecraft_hpp
