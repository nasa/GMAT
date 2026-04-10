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
 * 
 */
//------------------------------------------------------------------------------

#include "ThfDataSegment.hpp"



//------------------------------------------------------------------------------
// ThfDataSegment()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThfDataSegment() :
   segmentName                   (""),
   startEpochString              (""),
   startEpoch                    (0.0),      // 0.0 so relative timing is simple
   startEpochGT                  (0.0),
   endEpoch                      (0.0),
   endEpochGT                    (0.0),
   hasPrecisionTime              (false),
   csName                        ("EarthMJ2000Eq"),
   cs                            (NULL),
   interpolationMethod           ("None"),   // Default stairsteps the data
   accelIntType                  (NONE),
   massFlowInterpolationMethod   ("None"),
   massIntType                   (NONE),
   modelFlag                     ("ModelThrustOnly"),
   modelThrust                   (true),
   isDataLoaded                  (false),
   isActive                      (false),
   thrustScaleFactor             (1.0),
   massFlowScaleFactor           (1.0),
   includeThrustFactorInMassFlow (false)
{
}

//------------------------------------------------------------------------------
// ~ThfDataSegment()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThfDataSegment::~ThfDataSegment()
{
}

//------------------------------------------------------------------------------
// ThfDataSegment::ThfDataSegment(const ThfDataSegment& ds) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ds The thrust data segment providing data for this one
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThfDataSegment(const ThfDataSegment& ds) :
   segmentName                   (ds.segmentName),
   startEpochString              (ds.startEpochString),
   startEpoch                    (ds.startEpoch),
   startEpochGT                  (ds.startEpochGT),
   endEpoch                      (ds.endEpoch),
   endEpochGT                    (ds.endEpochGT),
   hasPrecisionTime              (ds.hasPrecisionTime),
   csName                        (ds.csName),
   cs                            (ds.cs),
   interpolationMethod           (ds.interpolationMethod),
   accelIntType                  (ds.accelIntType),
   massFlowInterpolationMethod   (ds.massFlowInterpolationMethod),
   massIntType                   (ds.massIntType),
   modelFlag                     (ds.modelFlag),
   modelThrust                   (ds.modelThrust),
   isDataLoaded                  (ds.isDataLoaded),
   isActive                      (ds.isActive),
   thrustScaleFactor             (ds.thrustScaleFactor),
   massFlowScaleFactor           (ds.massFlowScaleFactor),
   includeThrustFactorInMassFlow (ds.includeThrustFactorInMassFlow),
   tanks                         (ds.tanks)
{
   for (UnsignedInt i = 0; i < ds.profile.size(); ++i)
   {
      ThrustPoint tp = ds.profile[i];
      profile.push_back(tp);
   }
}

//------------------------------------------------------------------------------
// ThfDataSegment& operator =(const ThfDataSegment& ds)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ds The thrust data segment copied to this one
 *
 * @return This data segment, set to match ds
 */
//------------------------------------------------------------------------------
ThfDataSegment& ThfDataSegment::operator =(const ThfDataSegment& ds)
{
   if (this != &ds)
   {
      segmentName                 =  ds.segmentName;
      startEpochString            =  ds.startEpochString;
      startEpoch                  =  ds.startEpoch;
      startEpochGT                =  ds.startEpochGT;
      endEpoch                    =  ds.endEpoch;
      endEpochGT                  =  ds.endEpochGT;
      hasPrecisionTime            =  ds.hasPrecisionTime;
      csName                      =  ds.csName;
      cs                          =  ds.cs;
      interpolationMethod         =  ds.interpolationMethod;
      accelIntType                =  ds.accelIntType;
      massFlowInterpolationMethod =  ds.massFlowInterpolationMethod;
      massIntType                 =  ds.massIntType;
      modelFlag                   =  ds.modelFlag;
      modelThrust                 =  ds.modelThrust;
      isDataLoaded                =  ds.isDataLoaded;
      isActive                    =  ds.isActive;
      thrustScaleFactor           =  ds.thrustScaleFactor;
      massFlowScaleFactor         =  ds.massFlowScaleFactor;
      includeThrustFactorInMassFlow
                                  =  ds.includeThrustFactorInMassFlow;
      tanks                       =  ds.tanks;

      profile.clear();
      for (UnsignedInt i = 0; i < ds.profile.size(); ++i)
      {
         ThrustPoint tp = ds.profile[i];
         profile.push_back(tp);
      }
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool SetPrecisionTimeFlag(bool onOff)
//------------------------------------------------------------------------------
/**
* Set whether the thf data segment is using precision time or not
*
* @param onOff Flag indicating whether the thf data segment uses precision time
*
* @return Returns the value of the onOff flag passed in
*/
//------------------------------------------------------------------------------
bool ThfDataSegment::SetPrecisionTimeFlag(bool onOff)
{
   if ((hasPrecisionTime == false) && (onOff == true)) {
      startEpochGT = startEpoch;
      endEpochGT = endEpoch;
   }

   hasPrecisionTime = onOff;
   return hasPrecisionTime;
}

//------------------------------------------------------------------------------
// bool HasPrecisionTime()
//------------------------------------------------------------------------------
/**
* Return whether the thf data segment is using precision time or not
*
* @return Flag indicating whether the thf data segment uses precision time
*/
//------------------------------------------------------------------------------
bool ThfDataSegment::HasPrecisionTime()
{
   return hasPrecisionTime;
}

// Convenience methods for the thrust profile data structure

//------------------------------------------------------------------------------
// ThrustPoint()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThrustPoint::ThrustPoint() :
   time           (0.0),
   magnitude      (0.0),
   mdot           (0.0)

{
   vector[0] = vector[1] = vector[2] = 0.0;
}

//------------------------------------------------------------------------------
// ~ThrustPoint()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThrustPoint::~ThrustPoint()
{
}

//------------------------------------------------------------------------------
// ThrustPoint(const ThrustPoint& tp)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param tp The point copied to make this one
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThrustPoint::ThrustPoint(const ThrustPoint& tp) :
   time        (tp.time),
   magnitude   (tp.magnitude),
   mdot        (tp.mdot)
{
   vector[0] = tp.vector[0];
   vector[1] = tp.vector[1];
   vector[2] = tp.vector[2];
}

//------------------------------------------------------------------------------
// ThrustSegment::ThrustPoint& operator=(const ThrustPoint& tp)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param tp The point copied to this one
 *
 * @return This point, set to match tp
 */
//------------------------------------------------------------------------------
ThfDataSegment::ThrustPoint& ThfDataSegment::ThrustPoint::operator=(const ThrustPoint& tp)
{
   if (this != &tp)
   {
      time = tp.time;
      magnitude = tp.magnitude;
      vector[0] = tp.vector[0];
      vector[1] = tp.vector[1];
      vector[2] = tp.vector[2];
      mdot = tp.mdot;
   }
   return *this;
}

