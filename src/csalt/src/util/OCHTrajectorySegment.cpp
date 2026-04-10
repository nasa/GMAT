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
// Created: 2017.02.23
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "StringUtil.hpp"
#include "OCHTrajectorySegment.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
OCHTrajectorySegment::OCHTrajectorySegment() :
    TrajectorySegment(),
    centralBody      (""),
    startTime        (0.0),
    stopTime         (0.0),
    usableStartTime  (0.0),
    usableStopTime   (0.0),
    objectId         (""),
    objectName       (""),
    refFrame         (""),
    timeSystem       (""),
    hasStartTime     (false),
    hasStopTime      (false)
{
}


//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
OCHTrajectorySegment::OCHTrajectorySegment(const OCHTrajectorySegment &copy) :
    TrajectorySegment(copy),
    centralBody      (copy.centralBody),
    startTime        (copy.startTime),
    stopTime         (copy.stopTime),
    usableStartTime  (copy.usableStartTime),
    usableStopTime   (copy.usableStopTime),
    objectId         (copy.objectId),
    objectName       (copy.objectName),
    refFrame         (copy.refFrame),
    timeSystem       (copy.timeSystem),
    hasStartTime     (copy.hasStartTime),
    hasStopTime      (copy.hasStopTime)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
OCHTrajectorySegment& OCHTrajectorySegment::operator=(
                                            const OCHTrajectorySegment &copy)
{
   if (&copy == this)
      return *this;   

    TrajectorySegment::operator=(copy);
   
    centralBody     = copy.centralBody;
    startTime       = copy.startTime;
    stopTime        = copy.stopTime;
    usableStartTime = copy.usableStartTime;
    usableStopTime  = copy.usableStopTime;
    objectId        = copy.objectId;
    objectName      = copy.objectName;
    refFrame        = copy.refFrame;
    timeSystem      = copy.timeSystem;
    hasStartTime    = copy.hasStartTime;
    hasStopTime     = copy.hasStopTime;

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OCHTrajectorySegment::~OCHTrajectorySegment()
{
    // Nothing to do here
}


void OCHTrajectorySegment::SetCentralBody(std::string s)
{
   centralBody = s;
};

std::string OCHTrajectorySegment::GetCentralBody()
{
   return centralBody;
};

void OCHTrajectorySegment::SetStartTime(Real t)
{
   startTime = t;
};

Real OCHTrajectorySegment::GetStartTime()
{
   return startTime;
};

void OCHTrajectorySegment::SetStopTime(Real t)
{
   stopTime = t;
};

Real OCHTrajectorySegment::GetStopTime()
{
   return stopTime;
};

void OCHTrajectorySegment::SetUsableStartTime(Real t)
{
   usableStartTime = t;
};

Real OCHTrajectorySegment::GetUsableStartTime()
{
   return usableStartTime;
};

void OCHTrajectorySegment::SetUsableStopTime(Real t)
{
   usableStopTime = t;
};

Real OCHTrajectorySegment::GetUsableStopTime()
{
   return usableStopTime;
};

void OCHTrajectorySegment::SetObjectId(std::string id)
{
   objectId = id;
};

std::string OCHTrajectorySegment::GetObjectId()
{
   return objectId;
};

void OCHTrajectorySegment::SetObjectName(std::string nomme)
{
   objectName = nomme;
};

std::string OCHTrajectorySegment::GetObjectName()
{
   return objectName;
};

void OCHTrajectorySegment::SetRefFrame(std::string s)
{
   refFrame = s;
};

std::string OCHTrajectorySegment::GetRefFrame()
{
   return refFrame;
};

void OCHTrajectorySegment::SetTimeSystem(std::string s)
{
   timeSystem = s;
};

std::string OCHTrajectorySegment::GetTimeSystem()
{
   return timeSystem;
};

void OCHTrajectorySegment::SetHasStartTime(bool b)
{
   hasStartTime = b;
};

bool OCHTrajectorySegment::GetHasStartTime()
{
   return hasStartTime;
};

void OCHTrajectorySegment::SetHasStopTime(bool b)
{
   hasStopTime = b;
};

bool OCHTrajectorySegment::GetHasStopTime()
{
   return hasStopTime;
};

