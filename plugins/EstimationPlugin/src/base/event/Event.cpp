//$Id: Event.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                                Event
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
// Created: 2009/12/04
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Event base class.
 */
//------------------------------------------------------------------------------

#include "Event.hpp"
#include "EventException.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_FIXEDTIME
//#define DEBUG_INITIALIZATION
//#define DEBUG_DATA_STORE
//#define DEBUG_COORDINATE_SYSTEMS
//#define DEBUG_SET_RELATIVITY_CORRECTION
//#define DEBUG_SET_ETMINUSTAI_CORRECTION
//------------------------------------------------------------------------------
// Event::Event(const std::string &type, const std::string &name) :
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type Object type for the class (Gmat::EVENT)
 * @param name The Event's name
 */
//------------------------------------------------------------------------------
Event::Event(const std::string &type, const std::string &name) :
   GmatBase             (Gmat::EVENT, type, name),
   depth                (2),
   nyquist              (1.0e-99),
   tolerance            (1.0e-5),		// (1.0e-7),		// Notice that: when tolerance is small, the result of light time range may oscillate.
   maxAttempts          (50),
   estimatedEpoch       (0.0),
   status               (SEEKING),
   fixedEpoch           (-1),
   fixedTime            (0.0),
   varTime              (0.0),
   stepDirection        (-1.0),         // Step backwards by default
   bufferIndex          (0),
   bufferFillCount      (0),
   j2k                  (NULL),
   useRelativityCorrection (false),
//   useETMinusTAICorrection (false),
   solarSystem			(NULL)
{
}

//-----------------------------------------------------------------------------
// ~Event()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
Event::~Event()
{
}


//-----------------------------------------------------------------------------
// Event(const Event& ev) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ev The Event that is copied to this new one
 */
//-----------------------------------------------------------------------------
Event::Event(const Event& ev) :
   GmatBase             (ev),
   participantNames     (ev.participantNames),
   participants         (ev.participants),
   participantData      (ev.participantData),
   depth                (ev.depth),
   nyquist              (ev.nyquist),
   tolerance            (ev.tolerance),
   maxAttempts          (ev.maxAttempts),
   estimatedEpoch       (0.0),
   status               (SEEKING),
   fixedEpoch           (ev.fixedEpoch),
   fixedTime            (ev.fixedTime),
   varTime              (ev.varTime),
   stepDirection        (ev.stepDirection),
   bufferIndex          (0),
   bufferFillCount      (0),
   j2k                  (NULL),
   useRelativityCorrection (ev.useRelativityCorrection),
//   useETMinusTAICorrection (ev.useETMinusTAICorrection),
   solarSystem			(ev.solarSystem)
{
   for (UnsignedInt i = 0; i < ev.participantCS.size(); ++i)
      participantCS.push_back(ev.participantCS[i]);
}


//-----------------------------------------------------------------------------
// Event& operator=(const Event& ev)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ev The Event that is copied to this instance
 *
 * @return This instance, configured to match ev
 */
//-----------------------------------------------------------------------------
Event& Event::operator=(const Event& ev)
{
   if (this != &ev)
   {
      participantNames     = ev.participantNames;;
      participants         = ev.participants;
      participantData      = ev.participantData;
      depth                = ev.depth;
      epoch.clear();
      value.clear();
      derivative.clear();
      bufferIndex          = 0;
      bufferFillCount      = 0;
      j2k                  = NULL;
	  useRelativityCorrection = ev.useRelativityCorrection;
//	  useETMinusTAICorrection = ev.useETMinusTAICorrection;
	  solarSystem          = ev.solarSystem;
      nyquist              = ev.nyquist;
      tolerance            = ev.tolerance;
      maxAttempts          = ev.maxAttempts;
      estimatedEpoch       = 0.0;
      status               = SEEKING;
      fixedEpoch           = ev.fixedEpoch;
      fixedTime            = ev.fixedTime;
      varTime              = ev.varTime;
      foundEpochs.clear();
      for (UnsignedInt i = 0; i < ev.participantCS.size(); ++i)
         participantCS.push_back(ev.participantCS[i]);
   }

   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the event for use
 *
 * @return true if initialization succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool Event::Initialize()
{
   bool retval = GmatBase::Initialize();

   // Set the Nyquist frequency
   EvaluateNyquist();

   RealArray temp;
   bufferIndex = 0;

   // Prep the data vectors
   value.clear();
   derivative.clear();
   if (depth > 0)
   {
      for (Integer i = 0; i < depth; ++i)
      {
         value.push_back(temp);
         derivative.push_back(temp);
      }
   }

   status = SEEKING;
   fixedEpoch = -1;
   varTime = 0.0;
   estimatedEpoch = 0.0;
   return retval;
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem* ss)
//------------------------------------------------------------------------------
/**
 * Set solar system to this Event object
 *
 * @param ss    a pointer to SolarSystem object
 */
//------------------------------------------------------------------------------
void Event::SetSolarSystem(SolarSystem* ss)
{
	solarSystem = ss;
}

//------------------------------------------------------------------------------
// void SetRelativityCorrection(bool useCorr)
//------------------------------------------------------------------------------
/**
 * Set relativity correct option to this Event object
 *
 * @param useCorr    the option to use relativity correction
 */
//------------------------------------------------------------------------------
void Event::SetRelativityCorrection(bool useCorr)
{
#ifdef DEBUG_SET_RELATIVITY_CORRECTION
	MessageInterface::ShowMessage("Start Event::SetRelativityCorrection(%s)\n", (useCorr?"true":"false"));
#endif

	useRelativityCorrection = useCorr;
#ifdef DEBUG_SET_RELATIVITY_CORRECTION
	MessageInterface::ShowMessage("Exit Event::SetRelativityCorrection()\n");
#endif
}


//------------------------------------------------------------------------------
// void FixState(GmatBase* obj, bool lockState)
//------------------------------------------------------------------------------
/**
 * Preserves state data for later use
 *
 * @param obj The object that needs to be preserved
 * @param lockState false if just identifying the fixed object, true if the
 *                  state should also be captured
 */
//------------------------------------------------------------------------------
void Event::FixState(GmatBase* obj, bool lockState)
{
}


//------------------------------------------------------------------------------
// void FixState()
//------------------------------------------------------------------------------
/**
 * Preserves state data for later use
 *
 * The state preserved here is set for an object that has already been
 * identified by a call to FixState(GmatBase* obj, bool lockState).
 */
//------------------------------------------------------------------------------
void Event::FixState()
{
}


//-----------------------------------------------------------------------------
// const GmatEpoch GetFixedEpoch()
//-----------------------------------------------------------------------------
/**
 * Retrieves the epoch of the fixed state data used in the Event
 *
 * @return The epoch
 */
//-----------------------------------------------------------------------------
GmatEpoch Event::GetFixedEpoch(){
   return fixedEpoch;
}


//-----------------------------------------------------------------------------
// Real GetFixedTimestep()
//-----------------------------------------------------------------------------
/**
 * Retrieves the time step to the fixed state epoch
 *
 * In general, this method will provide a more precise time step than can be
 * calculated by differencing epochs because the attribute tracked here is
 * measured in seconds while the fixedEpoch is a modified Julian date.
 *
 * @return The timestep
 */
//-----------------------------------------------------------------------------
Real Event::GetFixedTimestep()
{
   return fixedTime;
}

//-----------------------------------------------------------------------------
// Real GetVarTimestep()
//-----------------------------------------------------------------------------
/**
 * Retrieves the time step to the estimated location of the Event, as an offset
 * from the initial epoch when the location process started.
 *
 * @return The timestep
 */
//-----------------------------------------------------------------------------
Real Event::GetVarTimestep()
{
   return varTime;
}

//-----------------------------------------------------------------------------
// void SetFixedTimestep(Real step)
//-----------------------------------------------------------------------------
/**
 * Sets the timestep to the fixedEpoch
 *
 * @param step The timestep
 */
//-----------------------------------------------------------------------------
void Event::SetFixedTimestep(Real step)
{
   fixedTime = step;
   #ifdef DEBUG_FIXEDTIME
      MessageInterface::ShowMessage("Setting fixed timestep on %s; now %le "
            "sec\n", GetName().c_str(), fixedTime);
   #endif
}

//-----------------------------------------------------------------------------
// void SetVarTimestep(Real step)
//-----------------------------------------------------------------------------
/**
 * Sets the timestep to the estimated Event epoch
 *
 * @param The estimated step
 */
//-----------------------------------------------------------------------------
void Event::SetVarTimestep(Real step)
{
   varTime = step;
}


//------------------------------------------------------------------------------
// Real EstimateTimestep()
//------------------------------------------------------------------------------
/**
 * Provides an epoch estimate to the caller
 *
 * This method provides a public interface to retrieve the estimated epoch of a
 * root.  The default implementation calls the CalculateEpochEstimate() method
 * and returns the estimatedEpoch parameter, which is filled in that call.
 *
 * @return The estimated step to the desired A.1 ModJulian epoch
 */
//------------------------------------------------------------------------------
Real Event::EstimateTimestep()
{
   CalculateTimestepEstimate();
   return estimatedEpoch;
}


//------------------------------------------------------------------------------
// bool CheckZero()
//------------------------------------------------------------------------------
/**
 * Tests to see if a root (zero crossing) of the event function occurred
 *
 * @return true if an event or extremum was detected; otherwise false
 */
//------------------------------------------------------------------------------
bool Event::CheckZero()
{
   bool retval = false;

   // Implement the epoch guess; the default, here, just linear interpolates
   if (bufferFillCount == 1)
      for (UnsignedInt i = 0; i < value[0].size(); ++i)
         //if (GmatMathUtil::Abs(value[0][i] < tolerance))  // "at" a zero
		 if (GmatMathUtil::Abs(value[0][i]) < tolerance)  // "at" a zero
            retval = true;

   if (bufferFillCount > 1)
   {
      Integer ringIndex1, ringIndex2;
      // Index 2 is last point entered
      ringIndex2 = bufferIndex-1;  // last point entered
      ringIndex1 = (ringIndex2 > 0 ? ringIndex2 - 1 : depth - 1);

      for (UnsignedInt i = 0; i < value[ringIndex2].size(); ++i)
         if (value[ringIndex2][i] * value[ringIndex1][i] <= 0) // Bracketed here!
         {
            retval = true;
            status = ZERO_BRACKETED;
         }
      if (retval == false) // Check for sign change on derivatives
         for (UnsignedInt i = 0; i < derivative[ringIndex2].size(); ++i)
            if (derivative[ringIndex2][i] * derivative[ringIndex1][i] <= 0)
            {
               retval = true;
               status = EXTREMA_BRACKETED;
            }
   }

   return retval;
}


//-----------------------------------------------------------------------------
// EventStatus CheckStatus(const EventStatus newStatus)
//-----------------------------------------------------------------------------
/**
 * Retrieves the Event status, optionally after changing it to an input value
 *
 * @param newStatus The new Event status, if being set.  If newStatus is
 *                  UNKNOWN_STATUS (the default), the status is not changed.
 *
 * @return The Event status at the end of the call
 */
//-----------------------------------------------------------------------------
EventStatus Event::CheckStatus(const EventStatus newStatus)
{
   #ifdef DEBUG_STATUS
      MessageInterface::ShowMessage("CheckStatus(%d) called; new status is ",
            newStatus);
      switch (newStatus)
      {
         case SEEKING:
            MessageInterface::ShowMessage("SEEKING\n");
            break;

         case ZERO_BRACKETED:
            MessageInterface::ShowMessage("ZERO_BRACKETED\n");
            break;

         case EXTREMA_BRACKETED:
            MessageInterface::ShowMessage("EXTREMA_BRACKETED\n");
            break;

         case ITERATING:
            MessageInterface::ShowMessage("ITERATING\n");
            break;

         case LOCATED:
            MessageInterface::ShowMessage("LOCATED\n");
            break;

         case UNKNOWN_STATUS:
         default:
            MessageInterface::ShowMessage("UNKNOWN_STATUS\n");
            break;
      }
   #endif
   if (newStatus != UNKNOWN_STATUS)
      status = newStatus;

   return status;
}

//------------------------------------------------------------------------------
// void AddParticipant(SpacePoint *part)
//------------------------------------------------------------------------------
/**
 * Adds a participant pointer to the list of event participants
 *
 * @param part The participant
 */
//------------------------------------------------------------------------------
void Event::AddParticipant(SpacePoint *part)
{
   if (part != NULL)
   {
      if (find(participants.begin(), participants.end(), part) ==
            participants.end())
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Adding event participant %s\n",
                  part->GetName().c_str());
         #endif
         participants.push_back(part);
         participantNames.push_back(part->GetName());
         participantCS.push_back(NULL);

         EventData ed;
         ed.participantName  = part->GetName();
         ed.participantIndex = participants.size()-1;
         ed.fixedState       = false;
         participantData.push_back(ed);

         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage(
                  "Participant Array for %s now has %d members\n",
                  instanceName.c_str(), participants.size());
            MessageInterface::ShowMessage(
                  "ParticipantCS Array for %s now has %d members\n",
                  instanceName.c_str(), participantCS.size());
         #endif
      }
   }
}


//------------------------------------------------------------------------------
// Integer GetParticipantIndex(GmatBase* p)
//------------------------------------------------------------------------------
/**
 * Finds an object in the participant vector.
 *
 * @param p The candidate object that may be a participant
 *
 * @return The index to the participant, or -2 if the object is not a
 *         participant
 */
//------------------------------------------------------------------------------
Integer Event::GetParticipantIndex(GmatBase* p)
{
   Integer index = -2;
   for (UnsignedInt i = 0; i < participants.size(); ++i)
      if (participants[i] == p)
         index = i;

   return index;
}


//------------------------------------------------------------------------------
// void AddCoordinateSystem(CoordinateSystem *cs, Integer forParticipant)
//------------------------------------------------------------------------------
/**
 * Adds a coordinate system that a participant references.
 *
 * The coordinate system is not an owned object, so it is not managed in this
 * Event.
 *
 * @param cs The coordinate suystem
 * @param forParticipant The participant
 */
//------------------------------------------------------------------------------
void Event::AddCoordinateSystem(CoordinateSystem *cs, Integer forParticipant)
{
   #ifdef DEBUG_COORDINATE_SYSTEMS
      MessageInterface::ShowMessage("Event::AddCoordinateSystem(%p, %d)\n", cs,
            forParticipant);
   #endif
   if (forParticipant == -1)
   {
      // If this is an MJ2000Eq system, set it as THE MJ2000 system
      GmatBase *axes = cs->GetRefObject(Gmat::AXIS_SYSTEM, "");
      if (axes->IsOfType("MJ2000EqAxes"))
      {
         j2k = cs;
      }
      else
         MessageInterface::ShowMessage("Core j2k coordinate system for the "
               "event %s needs to be an MJ2000 equatorial system, but the "
               "input system is not of that type\n", instanceName.c_str());
   }
   else if ((forParticipant >= 0) &&
            (forParticipant < (Integer)participants.size()))
      participantCS[forParticipant] = cs;
   else
      MessageInterface::ShowMessage("Attempting to set a coordinate system for "
            "participant #%d on event %s, but the event only contains %d "
            "participants\n", forParticipant, instanceName.c_str(),
            participants.size());

   #ifdef DEBUG_COORDINATE_SYSTEMS
      MessageInterface::ShowMessage("CoordinateSystem added; current list:\n");
      for (UnsignedInt i = 0; i < participantCS.size(); ++i)
         MessageInterface::ShowMessage("   %d -> %p\n", i, participantCS[i]);
   #endif
}


//-----------------------------------------------------------------------------
// const Rvector3& GetPosition(GmatBase* forParticipant)
//-----------------------------------------------------------------------------
/**
 * Retrieves the Event's position data for the input participant.
 *
 * The return value may differ from the data on the input object if the Event is
 * holding the participant fixed and some propagation has occurred.
 *
 * @param forParticipant The participant whose position is requested
 *
 * @return The location of the participant
 */
//-----------------------------------------------------------------------------
const Rvector3& Event::GetPosition(GmatBase* forParticipant)
{
   for (UnsignedInt i = 0; i < participants.size(); ++i)
   {
      if (participants[i] == forParticipant)
      {
         return participantData[i].position;
      }
   }

   throw EventException("Unable to find participant " +
         forParticipant->GetName() + " in the " + typeName + " event");
}


//------------------------------------------------------------------------------
// const Rvector3& GetVelocity(GmatBase* forParticipant)
//------------------------------------------------------------------------------
/**
 * Retrieves the Event's velocity data for the input participant.
 *
 * The return value may differ from the data on the input object if the Event is
 * holding the participant fixed and some propagation has occurred.
 *
 * @param forParticipant The participant whose velocity is requested
 *
 * @return The velocity of the participant
 */
//------------------------------------------------------------------------------
const Rvector3& Event::GetVelocity(GmatBase* forParticipant)
{
   for (UnsignedInt i = 0; i < participants.size(); ++i)
   {
      if (participants[i] == forParticipant)
      {
         return participantData[i].velocity;
      }
   }

   throw EventException("Unable to find participant " +
         forParticipant->GetName() + " in the " + typeName + " event");
}


//------------------------------------------------------------------------------
// EventData& GetEventData(GmatBase* forParticipant)
//------------------------------------------------------------------------------
/**
 * Retrieves the full EventData structure for a participant
 *
 * @param forParticipant The participant associated with the data
 *
 * @return The EventData structure
 */
//------------------------------------------------------------------------------
EventData& Event::GetEventData(GmatBase* forParticipant)
{
   for (UnsignedInt i = 0; i < participants.size(); ++i)
   {
      if (participants[i] == forParticipant)
      {
         return participantData[i];
      }
   }

   throw EventException("Unable to find participant " +
         forParticipant->GetName() + " in the " + typeName + " event");
}


//------------------------------------------------------------------------------
// void EvaluateNyquist()
//------------------------------------------------------------------------------
/**
 * Sets the Nyquist frequency for events that customize it
 *
 * The default method does nothing; the default Nyquist frequency (1e-99) is
 * used if the derived event does not reset it.
 */
//------------------------------------------------------------------------------
void Event::EvaluateNyquist()
{
}


//------------------------------------------------------------------------------
// void CalculateEpochEstimate()
//------------------------------------------------------------------------------
/**
 * Routine used to estimate the time offset for a root
 *
 * The default implementation assumes that the root is bracketed, and searches
 * between the enclosing epochs for the epoch of the root, using linear
 * interpolation to locate the root.
 */
//------------------------------------------------------------------------------
void Event::CalculateTimestepEstimate()
{
   // Implement the epoch guess; the default, here, just linear interpolates
   if (bufferFillCount < 2)
      throw EventException("Cannot estimate event epoch: not enough points");

   Integer ringIndex1, ringIndex2;
   // Index 2 is last point entered
   ringIndex2 = bufferIndex-1;  // last point entered
   ringIndex1 = (ringIndex2 > 0 ? ringIndex2 - 1 : depth - 1);

   estimatedEpoch = -1.0;

   /// todo: This does not look right for multivalued EF's; will need to
   /// override or address
   for (UnsignedInt i = 0; i < value[ringIndex2].size(); ++i)
      if (value[ringIndex2][i] * value[ringIndex1][i] <= 0) // Bracketed here!
      {
         Real slope = (value[ringIndex2][i] - value[ringIndex1][i]) /
               (epoch[ringIndex2] - epoch[ringIndex1]);
         estimatedEpoch = epoch[ringIndex1] - value[ringIndex1][i] / slope;
      }
//      else     // Not bracketed; look for extremum
//      {
//         // Not yet implemented
//         throw EventException(
//               "Extremum location not yet implemented in the Event base "
//               "class");
//      }
}


//------------------------------------------------------------------------------
// void BufferData(Real ep, RealArray &vals, RealArray &derivs)
//------------------------------------------------------------------------------
/**
 * Adds data to the ring buffer.
 *
 * @param ep The epoch of the incoming data
 * @param vals The event function value(s)
 * @param derivs The event function derivatives
 */
//------------------------------------------------------------------------------
void Event::BufferData(Real ep, RealArray &vals, RealArray &derivs)
{
   if (bufferIndex == depth)
      bufferIndex = 0;

   epoch[bufferIndex] = ep;
   value[bufferIndex] = vals;
   derivative[bufferIndex] = derivs;

   ++bufferIndex;
   ++bufferFillCount;
}


//------------------------------------------------------------------------------
// void StoreParticipantData(Integer whichOne, SpacePoint *obj, GmatEpoch when)
//------------------------------------------------------------------------------
/**
 * Stores the participant data at a set epoch
 *
 * @param whichOne The index of the participantData array for the incoming data
 * @param obj Pointer to the SpaceObject supplying the data
 * @param when The epoch of the data
 */
//------------------------------------------------------------------------------
void Event::StoreParticipantData(Integer whichOne, SpacePoint *obj,
      GmatEpoch when)
{
   #ifdef DEBUG_DATA_STORE
      MessageInterface::ShowMessage("Event(%s)::StoreParticipantData(%d, <%p>, "
            "%.12lf)\n", instanceName.c_str(), whichOne, obj, when);
   #endif
   participantData[whichOne].cs_origin = obj->GetJ2000Body();
   participantData[whichOne].epoch = when;
   participantData[whichOne].position = obj->GetMJ2000Position(when);
   participantData[whichOne].velocity = obj->GetMJ2000Velocity(when);

   #ifdef DEBUG_COORDINATE_SYSTEMS
      MessageInterface::ShowMessage("j2k %s NULL and pCS array size is %d\n",
            (j2k == NULL ? "is" : "is not"), participantCS.size());
   #endif

   if ((j2k != NULL) && (participantCS[whichOne] != NULL))
   {
      Rvector6   dummyIn(1.0,2.0,3.0,4.0,5.0,6.0);
      Rvector6   dummyOut;
      A1Mjd      itsEpoch(when);

      converter.Convert(when, dummyIn, j2k, dummyOut, participantCS[whichOne]);
      participantData[whichOne].rInertial2obj =
            converter.GetLastRotationMatrix();
   }

   #ifdef DEBUG_COORDINATE_SYSTEMS
      else
      {
         MessageInterface::ShowMessage("******** j2k -> %p and cs[%d] -> %p\n",
               j2k, whichOne, participantCS[whichOne]);
      }
   #endif

   Rmatrix *stateSTM = NULL;

   try
   {
      stateSTM = obj->GetParameterSTM(obj->GetParameterID("CartesianX"));
   }
   catch (BaseException &)
   {
      stateSTM = NULL;
   }

   if (stateSTM != NULL)
      participantData[whichOne].stm = *stateSTM;

   #ifdef DEBUG_DATA_STORE
      MessageInterface::ShowMessage("Participant data for %s updated\n",
            obj->GetName().c_str());
      MessageInterface::ShowMessage("   Current data:\n");
      // Will need to make more robust when the STM is variable in size
      for (UnsignedInt i = 0; i < participantData.size(); ++i)
      {
         MessageInterface::ShowMessage(
               "      Participant %d at epoch %.12lf:\n", i,
               participantData[i].epoch);
         MessageInterface::ShowMessage(
               "         Position: [%.12lf %.12lf %.12lf]\n",
               participantData[i].position[0], participantData[i].position[1],
               participantData[i].position[2]);
         MessageInterface::ShowMessage(
               "         Velocity: [%.12lf %.12lf %.12lf]\n",
               participantData[i].velocity[0], participantData[i].velocity[1],
               participantData[i].velocity[2]);
         MessageInterface::ShowMessage(
               "         R:        [%.12lf %.12lf %.12lf]\n",
               participantData[i].rInertial2obj(0,0),
               participantData[i].rInertial2obj(0,1),
               participantData[i].rInertial2obj(0,2));
         MessageInterface::ShowMessage(
               "                   [%.12lf %.12lf %.12lf]\n",
               participantData[i].rInertial2obj(1,0),
               participantData[i].rInertial2obj(1,1),
               participantData[i].rInertial2obj(1,2));
         MessageInterface::ShowMessage(
               "                   [%.12lf %.12lf %.12lf]\n",
               participantData[i].rInertial2obj(2,0),
               participantData[i].rInertial2obj(2,1),
               participantData[i].rInertial2obj(2,2));
         MessageInterface::ShowMessage("         STM:      [%.12lf %.12lf "
               "%.12lf %.12lf %.12lf %.12lf]\n",
               participantData[i].stm(0,0),
               participantData[i].stm(0,1),
               participantData[i].stm(0,2),
               participantData[i].stm(0,3),
               participantData[i].stm(0,4),
               participantData[i].stm(0,5));
         MessageInterface::ShowMessage("                   [%.12lf %.12lf "
               "%.12lf %.12lf %.12lf %.12lf]\n",
               participantData[i].stm(1,0),
               participantData[i].stm(1,1),
               participantData[i].stm(1,2),
               participantData[i].stm(1,3),
               participantData[i].stm(1,4),
               participantData[i].stm(1,5));
         MessageInterface::ShowMessage("                   [%.12lf %.12lf "
               "%.12lf %.12lf %.12lf %.12lf]\n",
               participantData[i].stm(2,0),
               participantData[i].stm(2,1),
               participantData[i].stm(2,2),
               participantData[i].stm(2,3),
               participantData[i].stm(2,4),
               participantData[i].stm(2,5));
         MessageInterface::ShowMessage("                   [%.12lf %.12lf "
               "%.12lf %.12lf %.12lf %.12lf]\n",
               participantData[i].stm(3,0),
               participantData[i].stm(3,1),
               participantData[i].stm(3,2),
               participantData[i].stm(3,3),
               participantData[i].stm(3,4),
               participantData[i].stm(3,5));
         MessageInterface::ShowMessage("                   [%.12lf %.12lf "
               "%.12lf %.12lf %.12lf %.12lf]\n",
               participantData[i].stm(4,0),
               participantData[i].stm(4,1),
               participantData[i].stm(4,2),
               participantData[i].stm(4,3),
               participantData[i].stm(4,4),
               participantData[i].stm(4,5));
         MessageInterface::ShowMessage("                   [%.12lf %.12lf "
               "%.12lf %.12lf %.12lf %.12lf]\n",
               participantData[i].stm(5,0),
               participantData[i].stm(5,1),
               participantData[i].stm(5,2),
               participantData[i].stm(5,3),
               participantData[i].stm(5,4),
               participantData[i].stm(5,5));

      }
   #endif
}
