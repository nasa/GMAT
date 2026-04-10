//$Id: EventData.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             EventData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Darrel J. Conway
// Created: Jun 3, 2010 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the EventData class
 */
//------------------------------------------------------------------------------


#include "EventData.hpp"


//------------------------------------------------------------------------------
// EventData()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
EventData::EventData() :
   participantName      (""),
   participantIndex     (-1),
   fixedState           (false),
   cs_origin            (NULL),
   epoch                (-1.0),
   position             (7000.0, 0.0, 0.0),
   velocity             (0.0, 0.0, 7.2)
{
   // Default STM is 6x6
   stm.SetSize(6, 6);

   // Initialize the matrices to identity matrices
   for (Integer i = 0; i < 3; ++i)
      rInertial2obj(i,i) = 1.0;
   for (Integer i = 0; i < 6; ++i)
      stm(i,i) = 1.0;
}


//------------------------------------------------------------------------------
// ~EventData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EventData::~EventData()
{
}


//------------------------------------------------------------------------------
// EventData(const EventData & ed)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ed The data copied into the new instance
 */
//------------------------------------------------------------------------------
EventData::EventData(const EventData & ed) :
   participantName      (ed.participantName),
   participantIndex     (ed.participantIndex),
   fixedState           (ed.fixedState),
   cs_origin            (ed.cs_origin),
   epoch                (ed.epoch),
   position             (ed.position),
   velocity             (ed.velocity),
   rInertial2obj        (ed.rInertial2obj),
   stm                  (ed.stm)
{
}


//------------------------------------------------------------------------------
// EventData& operator =(const EventData & ed)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ed The data copied into this instance
 *
 * @return This instance, with the data from ed
 */
//------------------------------------------------------------------------------
EventData & EventData::operator =(const EventData & ed)
{
   if (this != &ed)
   {
      participantName   = ed.participantName;
      participantIndex  = ed.participantIndex;
      fixedState        = ed.fixedState;
	   cs_origin         = ed.cs_origin;
      epoch             = ed.epoch;
      position          = ed.position;
      velocity          = ed.velocity;
      rInertial2obj     = ed.rInertial2obj;
      stm               = ed.stm;
   }

   return *this;
}
