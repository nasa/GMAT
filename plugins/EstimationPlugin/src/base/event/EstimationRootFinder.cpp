//$Id: EstimationRootFinder.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         EstimationRootFinder
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
// Created: 2009/12/09
//
/**
 * The root finder used to locate the epoch for events
 */
//------------------------------------------------------------------------------


#include "EstimationRootFinder.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"


//#define DEBUG_ROOT_SEARCH
//#define DEBUG_FIXED_STEP


//------------------------------------------------------------------------------
// EstimationRootFinder()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EstimationRootFinder::EstimationRootFinder() :
   propagator        (NULL)
{
}


//------------------------------------------------------------------------------
// ~EstimationRootFinder()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EstimationRootFinder::~EstimationRootFinder()
{
}


void EstimationRootFinder::CleanUp()
{
   // clean up PropSetup *propagator;
   if (propagator)
   {
      delete propagator;
      propagator = NULL;
   }

   // clean up std::vector<Spacecraft *> satBuffer;
   satBuffer.clear();

   // clean up std::vector<FormationInterface *> formBuffer;
   formBuffer.clear();

   //// clean up std::vector<Event*> *events
   //if (events)
   //{
   //   events->clear();
   //   delete events;
   //}
}

//------------------------------------------------------------------------------
// EstimationRootFinder(const EstimationRootFinder& rl)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EstimationRootFinder::EstimationRootFinder(const EstimationRootFinder& rl)
{
}


//------------------------------------------------------------------------------
// EstimationRootFinder& operator =(const EstimationRootFinder& rl)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rl The EstimationRootFinder that supplied the configuration data for this one
 *
 * @return This EstimationRootFinder
 */
//------------------------------------------------------------------------------
EstimationRootFinder& EstimationRootFinder::operator =(const EstimationRootFinder& rl)
{
   if (this != &rl)
   {
      propagator = NULL;
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetPropSetup(PropSetup* ps)
//------------------------------------------------------------------------------
/**
 * Sets the propagator used to generate data in the root finding process
 *
 * @param ps The propagator
 */
//------------------------------------------------------------------------------
void EstimationRootFinder::SetPropSetup(PropSetup* ps)
{
   propagator = (PropSetup*)(ps->Clone());

   #ifdef DEBUG_INITIALIZATION
      if (propagator != NULL)
         MessageInterface::ShowMessage("EstimationRootFinder set to use the "
               "PropSetup named %s\n", propagator->GetName().c_str());
      else
         MessageInterface::ShowMessage("EstimationRootFinder PropSetup cleared (set to "
               "NULL)\n");
   #endif
}


//-----------------------------------------------------------------------------
// void EstimationRootFinder::FixState(Event *thisOne)
//-----------------------------------------------------------------------------
/**
 * Fixes state data for an event
 *
 * This method tells the input Event to set the state data for its fixed states
 * for use during the event location process.  The method propagates to the
 * fixed state epoch if propagation is needed.
 *
 * @param thisOne The Event that needs to fix some state data
 */
//-----------------------------------------------------------------------------
void EstimationRootFinder::FixState(Event *thisOne)
{
   #ifdef DEBUG_FIXED_STEP
      MessageInterface::ShowMessage("EstimationRootFinder::FixState(%s) called\n",
            thisOne->GetName().c_str());
   #endif

   Real dt = thisOne->GetFixedTimestep();

   #ifdef DEBUG_FIXED_STEP
         MessageInterface::ShowMessage("   Stepping %.15lf \n", dt);
   #endif

   if (dt != 0.0)
   {
      ODEModel *ode = propagator->GetODEModel();
      GmatEpoch newEpoch =
            propagator->GetPropStateManager()->GetState()->GetEpoch();

      #ifdef DEBUG_FIXED_STEP
	     MessageInterface::ShowMessage("   start Epoch = %.15lf\n",newEpoch); 
         MessageInterface::ShowMessage("   Stepping %.15lf \n", dt);
      #endif
      // Propagate by dt
      propagator->GetPropagator()->Step(dt);
      // Need fixed time offset here as well
      newEpoch += dt/GmatTimeConstants::SECS_PER_DAY;
      #ifdef DEBUG_FIXED_STEP
         MessageInterface::ShowMessage("to epoch %.15lf\n", newEpoch);
      #endif
      ode->UpdateSpaceObject(newEpoch);
   }

   thisOne->FixState();

   #ifdef DEBUG_FIXED_STEP
      MessageInterface::ShowMessage("EstimationRootFinder::FixState(%s) exit\n",
            thisOne->GetName().c_str());
   #endif

}


//------------------------------------------------------------------------------
// Real Locate(ObjectArray &whichOnes)
//------------------------------------------------------------------------------
/**
 * Interface to initiate a root search for one or more events
 *
 * @param whichOnes The events that need to have roots found
 *
 * @return The epoch of the earliest event in the list
 */
//------------------------------------------------------------------------------
Real EstimationRootFinder::Locate(ObjectArray &whichOnes)
{
   #ifdef DEBUG_ROOT_SEARCH
      MessageInterface::ShowMessage("EstimationRootFinder::Locate called with %d "
            "events\n", whichOnes.size());
   #endif
   Real rootEpoch = -1.0;

   events = (std::vector<Event*>*)(&whichOnes);

   for (UnsignedInt i = 0; i < whichOnes.size(); ++i)
   {
      Real foundEpoch = FindRoot(i);
      if (foundEpoch > 0.0)
      {
         rootEpoch = (rootEpoch == -1.0 ? foundEpoch :
                      (rootEpoch > foundEpoch ? foundEpoch : rootEpoch));
      }
   }

   return rootEpoch;
}


//------------------------------------------------------------------------------
// Real EstimationRootFinder::FindRoot(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Drives the root finding search
 *
 * The current implementation performs all propagation from the point at which
 * the search started.  In other words, if the Event being located has a fixed
 * time offset from the initial state at the start of the search, the variable
 * time step in the search does not add to this fixed time step when propagation
 * is performed.  Instead, the propagation to the variable time is applied to
 * the initial state.  The implementation does it this way to minimize round-off
 * errors from multiple propagation step accumulations.
 *
 * @param whichOne The index of the root that is being located
 *
 * @return The epoch for the root
 */
//------------------------------------------------------------------------------
Real EstimationRootFinder::FindRoot(Integer whichOne)
{
   Real rootEpoch = -1;

   #ifdef DEBUG_ROOT_SEARCH
      MessageInterface::ShowMessage("EstimationRootFinder::FindRoot(%d) searching\n",
            whichOne);
   #endif

   Real dtFixed = (*events)[whichOne]->GetFixedTimestep();
   Real dt = (*events)[whichOne]->GetVarTimestep();

   #ifdef DEBUG_ROOT_SEARCH
      MessageInterface::ShowMessage("   fixedtimestep dtFixed = %.15lf\n", dtFixed);
   #endif

   // Propagate by dt
   propagator->GetPropagator()->Step(dtFixed + dt);

   // Need fixed time offset here as well
   Real oldEpoch = (*events)[whichOne]->GetFixedEpoch();
   Real newEpoch = oldEpoch + dt/GmatTimeConstants::SECS_PER_DAY;
   #ifdef DEBUG_ROOT_SEARCH
      MessageInterface::ShowMessage("                           timestep   dt = %.15lf sec\n", dt);
	  MessageInterface::ShowMessage("                           timestep   dt = %.15lf\n", dt/GmatTimeConstants::SECS_PER_DAY);
      MessageInterface::ShowMessage("EstimationRootFinder::FindRoot: oldEpoch = %.12lf\n", oldEpoch);
      MessageInterface::ShowMessage("EstimationRootFinder::FindRoot: newEpoch = %.12lf\n", newEpoch);
   #endif
   propagator->GetODEModel()->UpdateSpaceObject(newEpoch);

//   #ifdef DEBUG_ROOT_SEARCH
//      Real ef = (*events)[whichOne]->Evaluate();
//      MessageInterface::ShowMessage("   Event function = %.12lf at epoch "
//            "%.15lf\n", ef, newEpoch);
//   #endif

   rootEpoch = newEpoch;
   return rootEpoch;
}


//------------------------------------------------------------------------------
// void BufferSatelliteStates(bool fillingBuffer)
//------------------------------------------------------------------------------
/**
 * Preserves satellite state data so it can be restored after locating an event
 *
 * @param <fillingBuffer> Flag used to indicate the fill direction.
 */
//------------------------------------------------------------------------------
void EstimationRootFinder::BufferSatelliteStates(bool fillingBuffer)
{
   Spacecraft *fromSat = NULL, *toSat = NULL;
   FormationInterface *fromForm = NULL, *toForm = NULL;
   std::string soName;

   for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
        i != satBuffer.end(); ++i)
   {
      soName = (*i)->GetName();
      if (fillingBuffer)
      {
//         fromSat = (Spacecraft *)FindObject(soName);
         toSat = *i;
      }
      else
      {
         fromSat = *i;
//         toSat = (Spacecraft *)FindObject(soName);
      }

      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage(
            "   Sat is %s, fill direction is %s; fromSat epoch = %.12lf   "
            "toSat epoch = %.12lf\n",
            fromSat->GetName().c_str(),
            (fillingBuffer ? "from propagator" : "from buffer"),
            fromSat->GetRealParameter("A1Epoch"),
            toSat->GetRealParameter("A1Epoch"));

         MessageInterface::ShowMessage(
            "   '%s' Satellite state:\n", fromSat->GetName().c_str());
         Real *satrv = fromSat->GetState().GetState();
         MessageInterface::ShowMessage(
            "      %.12lf  %.12lf  %.12lf\n      %.12lf  %.12lf  %.12lf\n",
            satrv[0], satrv[1], satrv[2], satrv[3], satrv[4], satrv[5]);
      #endif

      (*toSat) = (*fromSat);

      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage(
            "After copy, From epoch %.12lf to epoch %.12lf\n",
            fromSat->GetRealParameter("A1Epoch"),
            toSat->GetRealParameter("A1Epoch"));
      #endif
   }

   for (std::vector<FormationInterface *>::iterator i = formBuffer.begin();
        i != formBuffer.end(); ++i)
   {
      soName = (*i)->GetName();
      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage("Buffering formation %s, filling = %s\n",
            soName.c_str(), (fillingBuffer?"true":"false"));
      #endif
      if (fillingBuffer)
      {
//         fromForm = (Formation *)FindObject(soName);
         toForm = *i;
      }
      else
      {
         fromForm = *i;
//         toForm = (Formation *)FindObject(soName);
      }

      #ifdef DEBUG_STOPPING_CONDITIONS
         MessageInterface::ShowMessage(
            "   Formation is %s, fill direction is %s; fromForm epoch = %.12lf"
            "   toForm epoch = %.12lf\n",
            fromForm->GetName().c_str(),
            (fillingBuffer ? "from propagator" : "from buffer"),
            fromForm->GetRealParameter("A1Epoch"),
            toForm->GetRealParameter("A1Epoch"));
      #endif

      (*toForm) = (*fromForm);

      toForm->UpdateState();

      #ifdef DEBUG_STOPPING_CONDITIONS
         Integer count = fromForm->GetStringArrayParameter("Add").size();

         MessageInterface::ShowMessage(
            "After copy, From epoch %.12lf to epoch %.12lf\n",
            fromForm->GetRealParameter("A1Epoch"),
            toForm->GetRealParameter("A1Epoch"));

         MessageInterface::ShowMessage(
            "   %s for '%s' Formation state:\n",
            (fillingBuffer ? "Filling buffer" : "Restoring states"),
            fromForm->GetName().c_str());

         Real *satrv = fromForm->GetState().GetState();

         for (Integer i = 0; i < count; ++i)
            MessageInterface::ShowMessage(
               "      %d:  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf  %.12lf\n",
               i, satrv[i*6], satrv[i*6+1], satrv[i*6+2], satrv[i*6+3],
               satrv[i*6+4], satrv[i*6+5]);
      #endif
   }

   #ifdef DEBUG_STOPPING_CONDITIONS
      for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
           i != satBuffer.end(); ++i)
         MessageInterface::ShowMessage(
            "   Epoch of '%s' is %.12lf\n", (*i)->GetName().c_str(),
            (*i)->GetRealParameter("A1Epoch"));
   #endif
}

