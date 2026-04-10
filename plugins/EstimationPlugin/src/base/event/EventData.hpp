//$Id: EventData.hpp 1398 2011-04-21 20:39:37Z  $
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


#ifndef EventData_hpp
#define EventData_hpp

#include "estimation_defs.hpp"
#include "gmatdefs.hpp"
#include "Rmatrix33.hpp"
#include "Rvector3.hpp"

// Forward reference
class SpacePoint;

/**
 * A structure used to store data calculated in events that is used when
 * computing measurements and their derivatives.
 */
class ESTIMATION_API EventData
{
public:
   EventData();
   virtual ~EventData();
   EventData(const EventData& ed);
   EventData& operator=(const EventData& ed);

   // The data in the structure
   /// Name of the participant associated with these data
   std::string participantName;
   /// Index for the participant
   Integer participantIndex;
   /// Flag indicating is the participant is fixed during event calculation
   bool fixedState;

   /// Inertial coordinate system used for this event data:
   // the origin of the inertial coordinate system
   SpacePoint* cs_origin;

   /// Epoch associated with the data
   GmatEpoch epoch;
   /// The participant's j2000eq position
   Rvector3  position;
   /// The participant's j2000eq velocity
   Rvector3  velocity;
   /// The rotation matrix from j2000eq to the participant's coordinate system
   Rmatrix33 rInertial2obj;      // Measurement needs to set this one
   /// The state transition matrix for the data
   Rmatrix   stm;
};

#endif /* EventData_hpp */
