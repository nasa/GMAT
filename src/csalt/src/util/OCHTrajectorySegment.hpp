//------------------------------------------------------------------------------
//                             OCH Trajectory Segment
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Jeremy Knittel / NASA
// Created: 2017.02.22
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------
    
#ifndef OCHTrajectorySegment_hpp
#define OCHTrajectorySegment_hpp

#include "TrajectorySegment.hpp"

class OCHTrajectorySegment : public TrajectorySegment
{
public:
   /// default constructor
   OCHTrajectorySegment();
   /// copy constructor
   OCHTrajectorySegment(const OCHTrajectorySegment &copy);
   /// operator=
   OCHTrajectorySegment& operator=(const OCHTrajectorySegment &copy);
   
   /// destructor
   virtual ~OCHTrajectorySegment(); 

   void        SetCentralBody(std::string s);
   std::string GetCentralBody();
   void        SetStartTime(Real t);
   Real        GetStartTime();
   void        SetStopTime(Real t);
   Real        GetStopTime();
   void        SetUsableStartTime(Real t);
   Real        GetUsableStartTime();
   void        SetUsableStopTime(Real t);
   Real        GetUsableStopTime();
   void        SetObjectId(std::string id);
   std::string GetObjectId();
   void        SetObjectName(std::string nomme);
   std::string GetObjectName();
   void        SetRefFrame(std::string s);
   std::string GetRefFrame();
   void        SetTimeSystem(std::string s);
   std::string GetTimeSystem();
   void        SetHasStartTime(bool b);
   bool        GetHasStartTime();
   void        SetHasStopTime(bool b);
   bool        GetHasStopTime();


protected:

   /// name of the central body
   std::string centralBody;
   /// the start time
   Real        startTime;
   /// the stop time
   Real        stopTime;
   /// the usable start time
   Real        usableStartTime;
   /// the usable stop time
   Real        usableStopTime;
   /// the object ID
   std::string objectId;
   /// the name of the object
   std::string objectName;
   /// the reference frame
   std::string refFrame;
   /// the time system
   std::string timeSystem;
   /// indicates if the segment has a start time
   bool        hasStartTime;
   /// indicates if the segment has a stop time
   bool        hasStopTime;
};

#endif
