//------------------------------------------------------------------------------
//                           ThfDataSegment
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 17, 2016
/**
 * Segment data read from a Thrust History File.
 * 
 * This class acts as s structure - member data is public - and is a helper
 * for the ThrustHistoryFile and ThrustSegment classes.
 */
//------------------------------------------------------------------------------

#ifndef ThfDataSegment_hpp
#define ThfDataSegment_hpp

#include "ThrustFileDefs.hpp"
#include "CoordinateSystem.hpp"

/**
 * Structure used to contain thrust history file data segments
 *
 * Implemented as a class to pick up assignment operator and copy constructor
 * methods.
 */
class ThfDataSegment
{
public:
   ThfDataSegment();
   virtual ~ThfDataSegment();
   ThfDataSegment(const ThfDataSegment &ds);
   ThfDataSegment& operator=(const ThfDataSegment &ds);

   bool SetPrecisionTimeFlag(bool onOff = true);
   bool HasPrecisionTime();

   /// Structure for the thrust profile data points
   struct ThrustPoint
   {
      /// Time from epoch, stored in days and fraction of day
      Real time;
      /// Magnitude of the thrust in the vector array (not currently used)
      Real magnitude;
      /// Cartesian components of the instantaneous thrust (or acceleration)
      Real vector[3];
      /// Mass flow for the point
      Real mdot;

      // constructor, destructor, copy constructor, assignment operator
      ThrustPoint();
      ~ThrustPoint();
      ThrustPoint(const ThrustPoint& tp);
      ThrustPoint& operator=(const ThrustPoint& tp);
   };

   /// Identifiers for the supported interpolators
   enum InterpolationType
   {
      NONE,
      LINEAR,
      SPLINE
   };

   //-------------------------------------------------
   // Data parsed from the file
   //-------------------------------------------------
   // Header fields
   /// Name of the segment
   std::string segmentName;
   /// Start epoch
   std::string startEpochString;
   /// a.1 start epoch
   GmatEpoch startEpoch;
   /// a.1 start epoch precision time
   GmatTime startEpochGT;
   /// a.1 epoch of the last node
   GmatEpoch endEpoch;
   /// a.1 epoch of the last node precision time
   GmatTime endEpochGT;
   /// Is precision time used?
   bool hasPrecisionTime;
   /// Name of the coordinate system for the thrust data
   std::string csName;
   /// The coordinate system
   CoordinateSystem *cs;
   /// Method of data interpolation
   std::string interpolationMethod;
   /// Interpolation method type for the force/accel
   InterpolationType accelIntType;
   /// Interpolation method for mass flow rate
   std::string massFlowInterpolationMethod;
   /// Interpolation method type for the mass flow
   InterpolationType massIntType;
   /// Segment modeling behavior
   std::string modelFlag;
   /// Thrust or acceleration?
   bool modelThrust;
   /// The thrust profile data, node by node
   std::vector<ThrustPoint> profile;
   /// has data been loaded
   bool isDataLoaded;
   /// Is segment active in Thrust History File
   bool isActive;

   //-------------------------------------------------
   // Data passed from the ThrustSegment Object
   //-------------------------------------------------
   /// Thrust scale factor
   Real thrustScaleFactor;
   /// Mass flow scale factor
   Real massFlowScaleFactor;
   /// Flag indicating is the mass flow includes the thrust scale factor
   bool includeThrustFactorInMassFlow;
   /// List of tanks that are used for mass flow
   StringArray tanks;
};

#endif /* ThfDataSegment_hpp */
