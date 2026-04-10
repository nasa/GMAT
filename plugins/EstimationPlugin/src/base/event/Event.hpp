//$Id: Event.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         ClassName
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/04
//
/**
 * The event base class definition.
 */
//------------------------------------------------------------------------------


#ifndef Event_hpp
#define Event_hpp

#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "EventData.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"


/// Enumeration defining the different states an event can have
enum EventStatus
{
   SEEKING,
   ZERO_BRACKETED,
   EXTREMA_BRACKETED,
   ITERATING,
   LOCATED,
   UNKNOWN_STATUS
};


/**
 * The class used to represent time based events that may occur during
 * propagation.
 *
 * The Event class defines calculation objects that either get monitored during
 * propagation to find critical locations on a trajectory or that need to be
 * propagated to perform a precision calculation.  Typical examples of Event
 * objects as defined by GMAT are shadow entry times and light time correction
 * calculations.  The shadow entry time is an example of an Event that needs
 * critical location for mission analysis.  The light time correction event is
 * an example of an Event that gets processed to perform precision calculations.
 */
class ESTIMATION_API Event : public GmatBase
{
public:
   Event(const std::string &type, const std::string &name = "");
   virtual ~Event();
   Event(const Event& ev);
   Event& operator=(const Event& ev);

   virtual bool Initialize();

   /// Derived classes implement this method to perform their computations
   virtual Real Evaluate() = 0;
   virtual void FixState(GmatBase* obj, bool LockState = false);
   virtual void FixState();
   virtual GmatEpoch GetFixedEpoch();   virtual Real GetFixedTimestep();
   virtual Real GetVarTimestep();
   virtual void SetFixedTimestep(Real step);
   virtual void SetVarTimestep(Real step);
   virtual Real EstimateTimestep();
   virtual bool CheckZero();
   virtual EventStatus CheckStatus(const EventStatus newStatus=UNKNOWN_STATUS);

   virtual void AddParticipant(SpacePoint *part);
   virtual Integer GetParticipantIndex(GmatBase* p);
   virtual void AddCoordinateSystem (CoordinateSystem *cs,
         Integer forParticipant = -1);
   virtual const Rvector3& GetPosition(GmatBase* forParticipant);
   virtual const Rvector3& GetVelocity(GmatBase* forParticipant);
   virtual EventData& GetEventData(GmatBase* forParticipant);

   virtual void SetSolarSystem(SolarSystem* ss);
   virtual void SetRelativityCorrection(bool useCorr);

protected:
   /// The names of the participants active in the Event
   StringArray          participantNames;
   /// Pointers to the objects supplying data to the event
   ObjectArray          participants;
   /// A collection of state data for the participants
   std::vector<EventData>
                        participantData;
   /// Size of the ring buffer of stored data used while monitoring this event
   Integer              depth;
   /// Epoch of the event data stored in the ring buffer
   RealArray            epoch;
   /// The ring buffer of event values
   std::vector<RealArray> value;
   /// Ring buffer of the event derivatives used to locate extrema
   std::vector<RealArray> derivative;
   /// The critical frequency associated with this event
   Real                 nyquist;
   /// The tolerance needed when locating this event
   Real                 tolerance;
   /// Maximum number of attempts that will be tried when locating this event
   Real                 maxAttempts;
   /// The current estimate of the epoch for the event
   Real                 estimatedEpoch;					// unit: second
   /// An array of epoch that have been found for the event
   RealArray            foundEpochs;
   /// The current status of this event
   EventStatus          status;

   /// The epoch of the fixed elements in the event calculation
   GmatEpoch            fixedEpoch;						// unit: a1Mjd
   /// The time step to the epoch of the fixed elements
   Real                 fixedTime;						// unit: second
   /// The estimated time to the Event from the initial epoch
   Real                 varTime;						// unit: second
   /// The direction that the step needs to take
   Real                 stepDirection;

   /// The current index into the ring buffer
   Integer              bufferIndex;
   /// The total number of points that have been passed to the ring buffer since
   /// it was last reset
   Integer              bufferFillCount;

   /// J2000 coordinate system used when setting stored data
   CoordinateSystem     *j2k;
   /// Participant coordinate systems used when setting stored data
   std::vector<CoordinateSystem*> participantCS;
   /// Converter to convert different frames
   CoordinateConverter  converter;

   /// Solar system which is used for calculating state in solar system barycenter inertial coordinate system
   SolarSystem         *solarSystem;
   /// Flags to indicate using relativity correction
   bool                useRelativityCorrection;

   virtual void EvaluateNyquist();
   virtual void CalculateTimestepEstimate();
   virtual void BufferData(Real ep, RealArray &vals, RealArray &derivs);
   virtual void StoreParticipantData(Integer whichOne, SpacePoint *obj,
         GmatEpoch when);
};

#endif // Event_hpp

