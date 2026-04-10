//------------------------------------------------------------------------------
//                           CustomSensor
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
// Author: Mike Stark, NASA/GSFC
// Created: 2016.04.03
//
/**
 * Implementation of the CustomSensor class
 */
//------------------------------------------------------------------------------
#ifndef CustomSensor_hpp
#define CustomSensor_hpp

#include "Sensor.hpp"
#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"

class CustomSensor: public Sensor
{
public:
 
   /// class construction/destruction
   CustomSensor (const Rvector &coneAngleVecIn, const Rvector &clockAngleVecIn);
   CustomSensor (const CustomSensor &copy);
   CustomSensor& operator= (const CustomSensor & copy);
   virtual ~CustomSensor();
   
   /// visibility methods
   /// Check the target visibility given the input cone and clock angles:
   /// determines whether or not the point is in the sensor FOV.
   bool CheckTargetVisibility(Real viewConeAngle, Real viewClockAngle);
   bool CheckRegionVisibility(const Rvector &coneAngleVec,
                              const Rvector &clockAngleVec);
   
 protected:
   
   // data
   /// data computed from constructor inputs
   Integer numFOVPoints;   // number of points representing FOV boundaries
   Rvector coneAngleVec;   // numFOVPoints cone angles, measured from +Z (rad)
   Rvector clockAngleVec;  // numFOVPoints clock angles (right ascensions) (rad)
   
   // string projectionAlgorithm; -- seems to be unused so far
   
   /// stereographic projection of the numFOVpoints cone & clock angles
   Rvector xProjectionCoordArray;   // numFOVpoints x values
   Rvector yProjectionCoordArray;   // numFOVpoints y values
   Rmatrix segmentArray;   // numFOVpoints x 4 representing line segments
                           // connecting points in stereographic projection
   /// test points computed in ComputeExternalPoints()
   Integer numTestPoints;
   Rmatrix externalPointArray; // n x 2 array where n <= numtestpoints
                               // and each row is an (x,y) stereographic pair
   /// maximum and minimum values for x and y values in stereographic projection
   Real maxXExcursion;
   Real minXExcursion;
   Real maxYExcursion;
   Real minYExcursion;
      
   // protected methods
   
   /// class hidden methods used by constructor
   bool     CheckTargetMaxExcursionCoordinates(Real xCoord, Real yCoord);
   Rmatrix  PointsToSegments(const Rvector &xCoords, const Rvector &yCoords);
   void     ComputeExternalPoints();
   
   /// helper methods for checkRegionVisibility()
   bool RegionIsFullyContained(std::vector<IntegerArray> &adjacency);
   
   ///  rVector utilities
   void Sort(Rvector &v, bool ascending = true);
   void Sort(Rvector &v, IntegerArray &indices, bool ascending=true);
   Real Max(const Rvector &v);
   Real Min(const Rvector &v);
   
};

/// data type and comparison method used in Sort(Rvector,IntegerArray,bool)
/// to mimic Matlab sort of both values and indices of original array
struct SensorElement
{
   Real value;
   Integer index;
};
bool CompareSensorElements(const SensorElement &e1, const SensorElement &e2);

#endif /* CustomSensor_hpp */
