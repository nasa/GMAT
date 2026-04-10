//------------------------------------------------------------------------------
//                           SignalBase
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
// Created: Jan 9, 2014
/**
 * Base class for signals between two measurement participants
 */
//------------------------------------------------------------------------------

#include "SignalBase.hpp"
#include "SpaceObject.hpp"
#include "BodyFixedPoint.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "PropSetup.hpp"
#include "Propagator.hpp"
#include "ODEModel.hpp"
#include "SpaceObject.hpp"
#include "StringUtil.hpp"
#include <sstream>

//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION
//#define DEBUG_LIGHTTIME
//#define DEBUG_MOVETOEPOCH
//#define DEBUG_SOLVEFORS
//#define DEBUG_API

#ifdef DEBUG_API
   #include <fstream>
   std::ofstream apisbFile;
   bool apisbFileOpen = false;
#endif


//---------------------------------
// static data
//---------------------------------
const Rmatrix33  SignalBase::I33        = Rmatrix33(true);
const Rmatrix33  SignalBase::zero33     = Rmatrix33(false);

const Rvector3   SignalBase::zeroVector = Rvector3(0.0,0.0,0.0);
const Rvector3   SignalBase::xUnit      = Rvector3(1.0,0.0,0.0);
const Rvector3   SignalBase::yUnit      = Rvector3(0.0,1.0,0.0);
const Rvector3   SignalBase::zUnit      = Rvector3(0.0,0.0,1.0);



//------------------------------------------------------------------------------
// SignalBase(const std::string &typeStr, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param typeStr The object type
 * @param name Name of the new object
 */
//------------------------------------------------------------------------------
SignalBase::SignalBase(const std::string &typeStr, const std::string &name) :
   GmatBase             (Gmat::USER_OBJECT_ID_NEEDED, typeStr, name),
   next                 (NULL),
   previous             (NULL),
   tcs                  (NULL),
   rcs                  (NULL),
   ocs                  (NULL),
   j2k                  (NULL),
   satPrecEpoch         (21545.0),
   //satEpochID           (-1),
   signalIsFeasible     (false),       // Not feasible until calculated!
   includeLightTime     (true),

   useHorp              (false),
   forceHorp            (false),
   horpHeight           (500.0),
   horpAngle            (70.0),
   horpOblateness       (false),

   solarSystem          (NULL),
   navLog               (NULL),
   logLevel             (1),
   ionosphereCache      (NULL)
{
#ifdef DEBUG_API
   if (!apisbFileOpen)
   {
      apisbFile.open("SignalBase_API.txt");
      apisbFileOpen = true;
   }
   apisbFile << "Signal base class debug for the API\n" << std::endl;
#endif

   type = GmatType::GetTypeId("Signal");
}


//------------------------------------------------------------------------------
// ~SignalBase()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SignalBase::~SignalBase()
{
   //MessageInterface::ShowMessage("SignalBase::~SignalBase() for leg <%p> start\n", this);
   if (tcs)
   {
      if (tcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
         delete tcs;
   }
   
   if (rcs)
   {
      if (rcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
         delete rcs;
   }
   
   if (ocs)
      delete ocs;
   
   if (j2k)
      delete j2k;
   
   //MessageInterface::ShowMessage("SignalBase::~SignalBase() 1\n");
   #ifdef DEBUG_API
      if (apisbFileOpen)
      {
         apisbFile.close();
         apisbFileOpen = false;
      }
   #endif

   theData.CleanUp();
   
   // clean up std::vector<RealArray> theDataDerivatives;
   for (Integer i = 0; i < theDataDerivatives.size(); ++i)
      theDataDerivatives[i].clear();
   theDataDerivatives.clear();
   
   // clean up std::vector<RealArray> theDataRateDerivatives;
   for (Integer i = 0; i < theDataRateDerivatives.size(); ++i)
      theDataRateDerivatives[i].clear();
   theDataRateDerivatives.clear();

   //MessageInterface::ShowMessage("SignalBase::~SignalBase() 2\n");
   // This step is handled by the delete of createdObjects in GmatBase
   // Clear the entire list
   //if (next)
   //   delete next;
//MessageInterface::ShowMessage("SignalBase::~SignalBase() for leg <%p> end\n", this);
}


//------------------------------------------------------------------------------
// SignalBase(const SignalBase& sb) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param sb The signal base used to create this new one
 */
//------------------------------------------------------------------------------
SignalBase::SignalBase(const SignalBase& sb) :
   GmatBase             (sb),
   next                 (NULL),
   previous             (NULL),
   theData              (sb.theData),
   tcs                  (NULL),
   rcs                  (NULL),
   ocs                  (NULL),
   j2k                  (NULL),
   satPrecEpoch         (21545.0),
   //satEpochID           (-1),
   signalIsFeasible     (false),       // Never feasible until calculated!
   includeLightTime     (sb.includeLightTime),
   useHorp              (sb.useHorp),
   forceHorp            (sb.forceHorp),
   horpHeight           (sb.horpHeight),
   horpAngle            (sb.horpAngle),
   horpOblateness       (sb.horpOblateness),
   solarSystem          (sb.solarSystem),
   navLog               (sb.navLog),
   logLevel             (sb.logLevel),
   ionosphereCache      (NULL)
{
   // Clone the list
   if (sb.next)
   {
      GmatBase* clonedObj = sb.next->Clone();
      createdObjects.push_back(clonedObj);
      Add((SignalBase*)clonedObj);
   }
}


//------------------------------------------------------------------------------
// SignalBase& operator=(const SignalBase& sb)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param sb The signal base used to configure this one
 *
 * @return This SignalBase, set to look like sb
 */
//------------------------------------------------------------------------------
SignalBase& SignalBase::operator=(const SignalBase& sb)
{
   if (this != &sb)
   {
      GmatBase::operator=(sb);

      theData             = sb.theData;

      if (tcs)
      {
         if (tcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
            delete tcs;
      }
      tcs = NULL;
      if (rcs)
      {
         if (rcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
            delete rcs;
      }
      rcs = NULL;
      if (ocs)
         delete ocs;
      ocs = NULL;
      if (j2k)
         delete j2k;
      j2k = NULL;

      satPrecEpoch        = 21545.0;
      //satEpochID          = -1;
      signalIsFeasible    = false;     // Never feasible until calculated!
      includeLightTime    = sb.includeLightTime;
      solarSystem         = sb.solarSystem;
      navLog              = sb.navLog;
      logLevel            = sb.logLevel;

      useHorp             = sb.useHorp;
      forceHorp           = sb.forceHorp;
      horpHeight          = sb.horpHeight;
      horpAngle           = sb.horpAngle;
      horpOblateness      = sb.horpOblateness;

      next = NULL;
      previous = NULL;
      if (sb.next)
      {
         GmatBase* clonedObj = sb.next->Clone();
         createdObjects.push_back(clonedObj);
         Add((SignalBase*)clonedObj);
      }

      isInitialized = false;
   }

   return *this;
}


//------------------------------------------------------------------------------
// bool SetProgressReporter(ProgressReporter* reporter)
//------------------------------------------------------------------------------
/**
 * Sets the reporter that us used to collect run data
 *
 * @param reporter The progress reporter
 *
 * @return true is the reporter set, false if not
 */
//------------------------------------------------------------------------------
bool SignalBase::SetProgressReporter(ProgressReporter* reporter)
{
   bool retval = false;

   if (reporter)
   {
      navLog = reporter;
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetTransmitParticipantName(std::string name)
//------------------------------------------------------------------------------
/**
 * Sets the transmit node name
 *
 * @param name The name of the node that transmits the signal
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SignalBase::SetTransmitParticipantName(std::string name)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("SignalBase<%p>::SetTransmitParticipantName('%s')\n", this, name.c_str());
#endif

   theData.transmitParticipant = name;
   bool retval = false;
   if (name != "")
      retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// bool SetReceiveParticipantName(std::string name)
//------------------------------------------------------------------------------
/**
 * Sets the receive node name
 *
 * @param name The name of the node that receives the signal
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SignalBase::SetReceiveParticipantName(std::string name)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("SignalBase<%p>::SetReceivrParticipantName('%s')\n", this, name.c_str());
#endif

   theData.receiveParticipant = name;
   bool retval = false;
   if (name != "")
      retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns the list of signal nodes
 *
 * @param type The expected type for the return list (unused in this method)
 *
 * @return The list of nodes
 */
//------------------------------------------------------------------------------
const StringArray& SignalBase::GetRefObjectNameArray(
      const UnsignedInt type)
{
   refObjectNames.clear();
   if (theData.transmitParticipant != "")
      refObjectNames.push_back(theData.transmitParticipant);
   if (theData.receiveParticipant != "")
      refObjectNames.push_back(theData.receiveParticipant);

   return refObjectNames;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const UnsignedInt type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets the reference object pointers for the signal nodes
 *
 * @param obj The reference object
 * @param type The type of the object (unused here)
 * @param name The object's name
 *
 * @return true if the object was set, false if not
 */
//------------------------------------------------------------------------------
bool SignalBase::SetRefObject(GmatBase* obj, const UnsignedInt type,
      const std::string& name)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("SignalBase<%p>::SetRefObject(obj = <%p>, type = %d, name = '%s')\n", this, obj, type, name.c_str());
#endif

   bool retval = false;

   if (obj != NULL)
   {
      if (obj->IsOfType("SpacePoint"))
      {
         //satEpochID = obj->GetParameterID("A1Epoch");
         if (theData.transmitParticipant == name)
         {
            theData.tNode = (SpacePoint*)obj;
            retval = true;
         }
         if (theData.receiveParticipant == name)
         {
            theData.rNode = (SpacePoint*)obj;
            retval = true;
         }
      }
   }

   if (retval == false)
      retval = GmatBase::SetRefObject(obj, type, name);

   return retval;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string& oldName,
//       const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Renames the nodes when the core object name is changed
 *
 * @param type The node type (unused here)
 * @param oldName The name the object had before the rename
 * @param newName The name the object has after the rename
 *
 * @return true is a name was changed, false if not
 */
//------------------------------------------------------------------------------
bool SignalBase::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("SignalBase<%p>::RenameRefObject(type = %d, oldName = '%s', newName = '%s')\n", this, type, oldName.c_str(), newName.c_str());
#endif

   bool retval = false;

   if (oldName == theData.transmitParticipant)
   {
      theData.transmitParticipant = newName;
      retval = true;
   }

   else if (oldName == theData.receiveParticipant)
   {
      theData.receiveParticipant = newName;
      retval = true;
   }
   else retval = GmatBase::RenameRefObject(type, oldName, newName);

   return retval;
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem* ss)
//------------------------------------------------------------------------------
/**
 * Adds the solar system configuration to the signal
 *
 * @param ss The solar system in use in the run
 */
//------------------------------------------------------------------------------
void SignalBase::SetSolarSystem(SolarSystem* ss)
{
   solarSystem = ss;
}


//------------------------------------------------------------------------------
// SignalBase* GetStart(bool epochIsAtEnd)
//------------------------------------------------------------------------------
/**
 * Finds either the first or last node in the signal path
 *
 * @param epochIsAtEnd true to get the last node, false to get the first
 *
 * @return
 */
//------------------------------------------------------------------------------
SignalBase* SignalBase::GetStart(bool epochIsAtEnd)
{
   SignalBase *retval = this;
   if (epochIsAtEnd)
   {
      if (next)
         retval = next->GetStart(epochIsAtEnd);
   }
   else
   {
      if (previous)
         retval = previous->GetStart(epochIsAtEnd);
   }
   return retval;
}


//------------------------------------------------------------------------------
// SignalBase* GetNext()
//------------------------------------------------------------------------------
/**
 * Returns the next node in the list
 *
 * @return The node pointer
 */
//------------------------------------------------------------------------------
SignalBase* SignalBase::GetNext()
{
   return next;
}


//------------------------------------------------------------------------------
// bool Add(SignalBase* signalToAdd)
//------------------------------------------------------------------------------
/**
 * Adds a signal to a signal path
 *
 * @param signalToAdd The signal that is added to the path
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SignalBase::Add(SignalBase* signalToAdd)
{
   if (next)
      next->Add(signalToAdd);
   else
   {
      next = signalToAdd;
      //theData.next = &(signalToAdd->GetSignalData());              
      theData.next = signalToAdd->GetSignalDataObject();
      signalToAdd->SetPrevious(this);
   }

   return true;
}


//------------------------------------------------------------------------------
// void SetPropagator(PropSetup* propagator, GmatBase* forObj)
//------------------------------------------------------------------------------
/**
 * Clones propagators for use moving objects while finding light time solutions
 *
 * This method is used to assign propagators to signal participants so that
 * the iteration needed for light time computations can be performed.  The
 * derived signal classes initialize the propagator for the specific participant
 * it uses.
 *
 * This code is not likely to change when support for multiple propagators is
 * implemented.  Dense output propagators may impact this code, because the
 * current approach involves cloning the propagator, resulting in a reset of the
 * Internal data.
 *
 * @param propagator The propagator being passed to the signal
 * @param forObj The participant that uses this propagator; if NULL it is set
 *               for both the transmitter and receiver
 */
//------------------------------------------------------------------------------
void SignalBase::SetPropagator(PropSetup* propagator, GmatBase* forObj)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("SignalBase<%p>::SetPropagator(propagator = <%p>, forObject = <%p,'%s'>)\n", this, propagator, forObj, forObj->GetName().c_str());
#endif

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting a propagator <%p> in the Signal "
            "class for %s\n", propagator, (forObj == NULL ?
            "both participants" : forObj->GetName().c_str()));
   #endif
   
   if (propagator == NULL)
      throw MeasurementException("The propagator passed in for the object " +
            forObj->GetName() + " is NULL, so the object cannot be propagated "
                  "for light time evaluation");
   
   if (theData.solveLightTime)
   {
      if ((theData.tNode == forObj) || (forObj == NULL))
      {
         theData.tPropagator = propagator;
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("  Transmitter propagator set\n");
         #endif
      }
      
      if ((theData.rNode == forObj) || (forObj == NULL))
      {
         theData.rPropagator = propagator;
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("  Receiver propagator set\n");
         #endif
      }
      
      if (next)
         next->SetPropagator(propagator, forObj);
   }
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepared the signal for processing by validating internal members
 *
 * @return true if the signal can be processed, false if not
 */
//------------------------------------------------------------------------------
bool SignalBase::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("Start SignalBase<%p>::Initialize()\n", this);
#endif


   bool retval = false;

   if (GmatBase::Initialize())
   {
      if (navLog)
         logLevel = navLog->GetLogLevel("Signal");
      else
         logLevel = 32767;

      if (theData.tNode && theData.rNode)
      {
         // 1. Set value for theData.stationParticipant
         theData.stationParticipant =
               ((theData.tNode->IsOfType(Gmat::GROUND_STATION)) ||
                (theData.rNode->IsOfType(Gmat::GROUND_STATION)));

         // 2. Update theData.tPrecTime and theData.rPrecTime
         theData.tPrecTime = theData.tNode->GetEpoch();
         theData.rPrecTime = theData.rNode->GetEpoch();
         // Initially, ground station epoch is set the same at epoch for spacecraft
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
            theData.tPrecTime = theData.rPrecTime;
         else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
            theData.rPrecTime = theData.tPrecTime;
         
         retval = true;
      }
      else if (theData.rNode)
      {
         // For GPS position measurement
         // 1. Set value for theData.stationParticipant
         theData.stationParticipant = theData.rNode->IsOfType(Gmat::GROUND_STATION);

         // 2. Update theData.rPrecTime
         theData.rPrecTime = theData.rNode->GetEpoch();
         
         retval = true;
      }

      if (next)
         retval = next->Initialize() && retval;

      // Unset the init flag so InitializeSignal will get called
      isInitialized = false;
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("End SignalBase<%p>::Initialize()\n", this);
#endif
   return retval;
}


//------------------------------------------------------------------------------
// std::string GetPathDescription()
//------------------------------------------------------------------------------
/**
 * Returns the path description
 *
 * @return The description, enclosed in curly braces
 */
//------------------------------------------------------------------------------
std::string SignalBase::GetPathDescription(bool fullList)
{
   std::string retval = "{";
   if (theData.transmitParticipant != "")
      retval += theData.transmitParticipant;
   else
      retval += "transmit participant not set";
   retval += " -> ";
   if (theData.receiveParticipant != "")
      retval += theData.receiveParticipant;
   else
      retval += "receive participant not set";
   retval += "}";

   if (fullList)
   {
      if (next)
         retval += next->GetPathDescription();
   }

   return retval;
}


////------------------------------------------------------------------------------
//// SignalData& GetSignalData()
////------------------------------------------------------------------------------
///**
// * Retrieves the most recently calculated data set for the signal
// *
// * @return The data set
// */
////------------------------------------------------------------------------------
//SignalData& SignalBase::GetSignalData()
//{
//   return theData;
//}


//------------------------------------------------------------------------------
// SignalData* GetSignalDataObject()
//------------------------------------------------------------------------------
/**
 * Retrieves the reference to the SignalData object of the signal
 *
 * @return The data set
 */
//------------------------------------------------------------------------------
SignalData* SignalBase::GetSignalDataObject()
{
   return &theData;
}


//------------------------------------------------------------------------------
// void SetSignalData(const SignalData& newData)
//------------------------------------------------------------------------------
/**
 * Receives signal data from an outside source
 *
 * @param newData The new signal data
 */
//------------------------------------------------------------------------------
void SignalBase::SetSignalData(const SignalData& newData)
{
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("<%p>: Receiving data to update %s and %s...",
            this, theData.transmitParticipant.c_str(),
            theData.receiveParticipant.c_str());
      Integer count = 0;
   #endif

   // Pass in the current computed data
   if (theData.receiveParticipant == newData.receiveParticipant)
   {
      theData.rPrecTime = newData.rPrecTime;
      theData.rLoc  = newData.rLoc;
      theData.rVel  = newData.rVel;
      #ifdef DEBUG_LIGHTTIME
         ++count;
      #endif
   }
   if (theData.receiveParticipant == newData.transmitParticipant)
   {
      theData.rPrecTime = newData.tPrecTime;
      theData.rLoc  = newData.tLoc;
      theData.rVel  = newData.tVel;
      #ifdef DEBUG_LIGHTTIME
         ++count;
      #endif
   }
   if (theData.transmitParticipant == newData.receiveParticipant)
   {
      theData.tPrecTime = newData.rPrecTime;
      theData.tLoc  = newData.rLoc;
      theData.tVel  = newData.rVel;
      #ifdef DEBUG_LIGHTTIME
         ++count;
      #endif
   }
   if (theData.transmitParticipant == newData.transmitParticipant)
   {
      theData.tPrecTime = newData.tPrecTime;
      theData.tLoc  = newData.tLoc;
      theData.tVel  = newData.tVel;
      #ifdef DEBUG_LIGHTTIME
         ++count;
      #endif
   }

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("%d matching data sets updated; epochs "
            "%.12lf -> %.12lf\n", count, theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd());
   #endif
}

//------------------------------------------------------------------------------
// bool IsSignalFeasible()
//------------------------------------------------------------------------------
/**
 * Returns feasibility for the last signal calculated
 *
 * @return true if the signal is feasible, false if not
 */
//------------------------------------------------------------------------------
bool SignalBase::IsSignalFeasible()
{
   bool retval = signalIsFeasible;

   if (next)
      retval = retval && next->IsSignalFeasible();

   return retval;
}


//------------------------------------------------------------------------------
// void SignalBase::UsesLighttime(const bool tf)
//------------------------------------------------------------------------------
/**
 * Manages the light time computation flag
 *
 * @param tf true to include light time solutions, false to omit them
 */
//------------------------------------------------------------------------------
void SignalBase::UsesLighttime(const bool tf)
{
   includeLightTime = tf;
   if (next)
      next->UsesLighttime(tf);

}


//------------------------------------------------------------------------------
// void InitializeSignal()
//------------------------------------------------------------------------------
/**
 * Prepares the signal for use
 */
//------------------------------------------------------------------------------
void SignalBase::InitializeSignal(bool chainForwards)
{
   #ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SignalBase<%p>::InitializeSignal() for leg %s to %s\n", this, 
      (theData.tNode == NULL?"":theData.tNode->GetName().c_str()), 
      (theData.rNode == NULL?"":theData.rNode->GetName().c_str()));
   #endif

#ifdef DEBUG_API
   if (apisbFileOpen)
   {
      apisbFile << "Entered SignalBase::InitializeSignal for path "
                << GetPathDescription(false) << std::endl;
   }
#endif

   if (isInitialized)
   {
      return;
   }

   SpaceObject *spObj  = NULL;
   SpacePoint  *origin = NULL;
   CelestialBody *earth = solarSystem->GetBody("Earth");

   GmatTime gsPrecEpoch = theData.tPrecTime;

   // 1. processing for GSP measurement
   if ((theData.tNode == NULL) && (theData.rNode != NULL))
   {
      if (theData.rNode->IsOfType(Gmat::SPACEOBJECT))
      {
         theData.tPrecTime = theData.rPrecTime = theData.rNode->GetEpoch();
         gsPrecEpoch = theData.tPrecTime;
         satPrecEpoch = theData.rPrecTime;
         
#ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("case 1: Initializing with epoch %.12lf\n", satPrecEpoch.GetMjd());
#endif

         origin = ((SpaceObject*)theData.rNode)->GetOrigin();
         if (rcs)
         {
            if (rcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
               delete rcs;
         }
         rcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
            origin, NULL, NULL, earth, solarSystem);

         if (tcs)
         {
            if (tcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
            delete tcs;
         }
         tcs = CoordinateSystem::CreateLocalCoordinateSystem("TCS", "MJ2000Eq",
            origin, NULL, NULL, earth, solarSystem);

         if (ocs)
            delete ocs;
         ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS", "MJ2000Eq",
            origin, NULL, NULL, earth, solarSystem);

         if (j2k)
            delete j2k;
         j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
            origin, NULL, NULL, earth, solarSystem);
      }
      else
         throw MeasurementException("Error: GMAT cannot handle GPS tracking position of an object other than spacecraft.\n");
   }
   else
   {
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("case 2: Initializing with epoch %.12lf\n", satPrecEpoch.GetMjd());
   #endif


    #ifdef DEBUG_API
       if (apisbFileOpen)
       {
          apisbFile << "   Building coordinate systems; ss = <"
                    << solarSystem << ">\n   Data dump:" << std::endl;

         if (rcs)
         {
            if (rcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
               delete rcs;
         }
         rcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
            origin, NULL, NULL, earth, solarSystem);
         if (ocs)
            delete ocs;
         ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS", "Topocentric",
            bf, NULL, NULL, bf->GetJ2000Body(), solarSystem);
          apisbFile << "      epoch:  " << gsPrecEpoch.GetMjd()
                    << "\n      t node: " << theData.tNode
                    << " named "
                    << (theData.tNode != NULL ? theData.tNode->GetName() : "???")
                    << " of type "
                    << (theData.tNode != NULL ? theData.tNode->GetTypeName() : "NULL")
                    << "\n      r node: " << theData.rNode
                    << " named "
                    << (theData.rNode != NULL ? theData.rNode->GetName() : "???")
                    << " of type "
                    << (theData.rNode != NULL ? theData.rNode->GetTypeName() : "NULL")
                    << std::endl;
       }
    #endif

       // 2. Set tcs, rcs, ocs, and j2k coordinate systems
       if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
       {
    #ifdef DEBUG_API
       if (apisbFileOpen)
       {
          apisbFile << "         1.0";
       }
    #endif
          BodyFixedPoint *bf = (BodyFixedPoint*)theData.tNode;
          CoordinateSystem *bfcs = bf->GetBodyFixedCoordinateSystem();
          tcs = CoordinateSystem::CreateLocalCoordinateSystem("TCS", bfcs->GetAxisSystem()->GetTypeName(),
              bfcs->GetOrigin(), NULL, NULL, bfcs->GetJ2000Body(), bfcs->GetSolarSystem());

    #ifdef DEBUG_API
       if (apisbFileOpen)
       {
          apisbFile << " tcs = " << tcs << std::endl;
          apisbFile << "         1.1";
       }
    #endif
          spObj  = (SpaceObject*) theData.rNode;
          origin = spObj->GetOrigin();

    #ifdef DEBUG_API
       if (apisbFileOpen)
       {
          apisbFile << " origin = " << origin << std::endl;
          apisbFile << "         1.2";
       }
    #endif
          if (rcs)
             delete rcs;
          rcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
                   origin, NULL, NULL, earth, solarSystem);
    #ifdef DEBUG_API
       if (apisbFileOpen)
       {
          apisbFile << " rcs = " << rcs << std::endl;
          apisbFile << "         1.3";
       }
    #endif
          if (ocs)
             delete ocs;
          ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS","Topocentric",
                   bf, NULL, NULL, bf->GetJ2000Body(), solarSystem);
          
    #ifdef DEBUG_API
       if (apisbFileOpen)
       {
          apisbFile << " ocs = " << ocs << std::endl;
          apisbFile << "         1.4";
       }
    #endif
          if (j2k)
             delete j2k;
          j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
                   tcs->GetOrigin(), NULL, NULL, earth, solarSystem);
    #ifdef DEBUG_API
       if (apisbFileOpen)
       {
          apisbFile << " j2k = " << j2k << std::endl;
          apisbFile << "         1.5 Groundstation transmit code CS's set"
                    << std::endl;
       }
    #endif
       }
       else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
       {
          gsPrecEpoch = theData.rPrecTime;
          BodyFixedPoint *bf = (BodyFixedPoint*)theData.rNode;
          CoordinateSystem *bfcs = bf->GetBodyFixedCoordinateSystem();
          rcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", bfcs->GetAxisSystem()->GetTypeName(),
              bfcs->GetOrigin(), NULL, NULL, bfcs->GetJ2000Body(), bfcs->GetSolarSystem());
          if (tcs)
          {
             if (tcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
             delete tcs;
          }
          tcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
            origin, NULL, NULL, earth, solarSystem);

          spObj  = (SpaceObject*) theData.tNode;
          origin = spObj->GetOrigin();
          
          if (tcs)
             delete tcs;
          tcs = CoordinateSystem::CreateLocalCoordinateSystem("TCS", "MJ2000Eq",
                   origin, NULL, NULL, earth, solarSystem);
          
          if (ocs)
             delete ocs;
          ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS","Topocentric",
                   bf, NULL, NULL, earth, solarSystem);
          
          if (j2k)
             delete j2k;
          j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
                   rcs->GetOrigin(), NULL, NULL, earth, solarSystem);
       }
       else if (theData.rNode->IsOfType(Gmat::SPACECRAFT) && theData.tNode->IsOfType(Gmat::SPACECRAFT))
       {
          origin = ((SpaceObject *)((Spacecraft *)theData.rNode))->GetOrigin();

          if (rcs)
          {
             if (rcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
                delete rcs;
          }
          rcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
             origin, NULL, NULL, earth, solarSystem);


          origin = ((SpaceObject *)((Spacecraft *)theData.tNode))->GetOrigin();
          if (tcs)
          {
             if (tcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
                delete tcs;
          }
          tcs = CoordinateSystem::CreateLocalCoordinateSystem("TCS", "MJ2000Eq",
             origin, NULL, NULL, earth, solarSystem);

          if (ocs)
             delete ocs;
          ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS", "MJ2000Eq",
             origin, NULL, NULL, earth, solarSystem);

          if (j2k)
             delete j2k;
          j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
             origin, NULL, NULL, earth, solarSystem);
       }
      else
      {
         origin = ((SpaceObject*)theData.tNode)->GetOrigin();

         if (rcs)
         {
            if (rcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
               delete rcs;
         }
         rcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
            origin, NULL, NULL, earth, solarSystem);

         if (tcs)
         {
            if (tcs->GetAxisSystem()->GetTypeName() != "BodyFixed")
               delete tcs;
         }
         tcs = CoordinateSystem::CreateLocalCoordinateSystem("TCS", "MJ2000Eq",
            origin, NULL, NULL, earth, solarSystem);

         if (ocs)
            delete ocs;
         ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS", "MJ2000Eq",
            origin, NULL, NULL, earth, solarSystem);

         if (j2k)
            delete j2k;
         j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
            origin, NULL, NULL, earth, solarSystem);
      }
   }
#ifdef DEBUG_API
   if (apisbFileOpen)
   {
      apisbFile << "   Updating rotation matrices" << std::endl;
   }
#endif
   // 3. Update all rotation matrixes at gs time (or at transmit time when both nodes are spacecrafts
   std::string updateAll = "All";

   UpdateRotationMatrix(gsPrecEpoch.GetMjd(), updateAll);
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Late Binding Initialization complete:\n"
            "   SolarSystem pointer:  %p\n"
            "   Transmitter CS:       %p\n"
            "   Receiver CS:          %p\n"
            "   Observation CS:       %p\n"
            "   J2000 CS:             %p\n",
            solarSystem, tcs, rcs, ocs, j2k);
   #endif

   if (chainForwards)
   {
      if (next)
      {
         next->InitializeSignal(chainForwards);
      }
   }
   else
   {
      if (previous)
      {
         previous->InitializeSignal(chainForwards);
      }
   }

#ifdef DEBUG_API
   if (apisbFileOpen)
   {
      apisbFile << "   Signal " << GetPathDescription(false)
                << " is ready" << std::endl;
   }
#endif
   isInitialized = true;
}


//------------------------------------------------------------------------------
// void CalculateRangeVectorInertial()
//------------------------------------------------------------------------------
/**
 * 1. Computes the range vector in MJ2000 Equatorial coordinates.
 * 2. Update roation matrix at ground station's time. If both nodes are 
 *    spacecrafts, time is at transmit node.
 *
 * The default implementation here performs the instantaneous range vector
 * calculation.  Override this implementation for the light time corrected
 * vector.
 */
//------------------------------------------------------------------------------
void SignalBase::CalculateRangeVectorInertial()
{

   // 1. Update all rotation matrixes at time of ground station. If both tnode and rNode are spacecrafts, update at transmit time
   std::string updateAll = "All";

   GmatTime gsPrecEpoch = theData.tPrecTime;
   if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
      gsPrecEpoch = theData.rPrecTime;
   UpdateRotationMatrix(gsPrecEpoch.GetMjd(), updateAll);

   // 2. Compute range vector in MJ2000 coordinate system 
   SpacePoint *origin1 = tcs->GetOrigin();
   SpacePoint *origin2 = rcs->GetOrigin();
   SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");

   theData.tOStateSSB = origin1->GetMJ2000PrecState(theData.tPrecTime) - ssb->GetMJ2000PrecState(theData.tPrecTime);
   theData.rOStateSSB = origin2->GetMJ2000PrecState(theData.rPrecTime) - ssb->GetMJ2000PrecState(theData.rPrecTime);
   theData.j2kOriginSep = (theData.rOStateSSB.GetR() - theData.tOStateSSB.GetR());
   theData.j2kOriginVel = (theData.rOStateSSB.GetV() - theData.tOStateSSB.GetV());
   
   // GMAT MathSpec Eq. 6.12
   theData.rangeVecInertial = theData.rLoc + theData.j2kOriginSep - theData.tLoc;
   
   // GMAT MathSpec Eq. 6.10
   SpacePoint* propOrigin = nullptr;
   Rvector6 tSSB2SunState;
   Rvector6 rSSB2SunState;
   if (theData.rNode->IsOfType(Gmat::SPACECRAFT) && theData.tNode->IsOfType(Gmat::SPACECRAFT))
   {

      SpacePoint* rPropOrigin = nullptr;
      if (theData.tPropagator->GetPropagator()->UsesODEModel())
         propOrigin = theData.tPropagator->GetODEModel()->GetForceOrigin();        // the origin of the coordinate system used in forcemodel       // fix bug GMT-5364
      else
         propOrigin = theData.tPropagator->GetPropagator()->GetPropOrigin();

      if (theData.rPropagator->GetPropagator()->UsesODEModel())
         rPropOrigin = theData.rPropagator->GetODEModel()->GetForceOrigin();        // the origin of the coordinate system used in forcemodel       // fix bug GMT-5364
      else
         rPropOrigin = theData.rPropagator->GetPropagator()->GetPropOrigin();

      tSSB2SunState = propOrigin->GetMJ2000PrecState(theData.tPrecTime)
         - ssb->GetMJ2000PrecState(theData.tPrecTime);
      rSSB2SunState = rPropOrigin->GetMJ2000PrecState(theData.rPrecTime)
         - ssb->GetMJ2000PrecState(theData.rPrecTime);
   }
   else
   {
      if (theData.tNode->IsOfType(Gmat::SPACECRAFT))
      {
         if (theData.tPropagator->GetPropagator()->UsesODEModel())
            propOrigin = theData.tPropagator->GetODEModel()->GetForceOrigin();        // the origin of the coordinate system used in forcemodel       // fix bug GMT-5364
         else
            propOrigin = theData.tPropagator->GetPropagator()->GetPropOrigin();
      }
      else
      {
         if (theData.rPropagator->GetPropagator()->UsesODEModel())
            propOrigin = theData.rPropagator->GetODEModel()->GetForceOrigin();        // the origin of the coordinate system used in forcemodel       // fix bug GMT-5364
         else
            propOrigin = theData.rPropagator->GetPropagator()->GetPropOrigin();
      }

      tSSB2SunState = propOrigin->GetMJ2000PrecState(theData.tPrecTime)
         - ssb->GetMJ2000PrecState(theData.tPrecTime);
      rSSB2SunState = propOrigin->GetMJ2000PrecState(theData.rPrecTime)
         - ssb->GetMJ2000PrecState(theData.rPrecTime);
   }
   Rvector3 disp = (rSSB2SunState - tSSB2SunState).GetR();
   theData.rangeVecI = theData.rangeVecInertial - disp;

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Origin Sep: %s\n tLoc: %s\n rLoc: %s\n",
            (theData.j2kOriginSep.ToString(8).c_str()),
            (theData.tLoc.ToString(8).c_str()),
            (theData.rLoc.ToString(8)).c_str());
   #endif
}

//------------------------------------------------------------------------------
// void CalculateRangeVectorObs()
//------------------------------------------------------------------------------
/**
 * Computes the range vector in the observation coordinate system
 *
 * This method calls CalculateRangeVectorInertial() to generate a range vector,
 * then transforms that vector into the coordinate system needed for the
 * observation data.
 */
//------------------------------------------------------------------------------
void SignalBase::CalculateRangeVectorObs()
{
   CalculateRangeVectorInertial();

   // 1. Update R_Transmitter_j2k, R_receiver_j2k, and R_Obs_j2k matrixes
   UpdateRotationMatrix(theData.tPrecTime.GetMjd(),"j2k_1");
   UpdateRotationMatrix(theData.rPrecTime.GetMjd(),"j2k_2");
   if (theData.stationParticipant)
   {
      if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         UpdateRotationMatrix(theData.tPrecTime.GetMjd(),"o_j2k");
      else
         UpdateRotationMatrix(theData.rPrecTime.GetMjd(),"o_j2k");
   }
   else
      R_Obs_j2k = I33;

   // 2. Compute the positions of the participants in their own frames
   theData.tLocTcs = theData.tJ2kRotation * theData.tLoc;
   theData.rLocRcs = theData.rJ2kRotation * theData.rLoc;

   // 3. Compute range vector observation
   // @todo: handle this for arbitrary participants; currently rotates if there
   // are any GS's in the participant list
   if (theData.stationParticipant)
      theData.rangeVecObs = R_Obs_j2k * theData.rangeVecInertial;      // R_Obs_j2k matrix was computed in CalculateRangeVectorInertial() function
   else
      theData.rangeVecObs = theData.rangeVecInertial;  // same, really, but skip
                                                       // multiplication by I33
}


//------------------------------------------------------------------------------
// void CalculateRangeRateVectorObs(Integer p1index, Integer p2index)
//------------------------------------------------------------------------------
/**
 * Computes the range rate vector in the observation coordinate system
 *
 * This method calls CalculateRangeVectorInertial() to generate a range vector,
 * then transforms that vector into the coordinate system needed for the
 * observation data.
 */
//------------------------------------------------------------------------------
void SignalBase::CalculateRangeRateVectorObs()
{
   SpacePoint *origin1 = tcs->GetOrigin();
   SpacePoint *origin2 = rcs->GetOrigin();
   SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");

   // 1. Compute tVel, rVel, j2kOriginVel, rangeRateVecInertial
   // Velocity of origin2 at time rPrecTime w.r.t. origin1 at time tPrecTime
   theData.j2kOriginVel = (origin1->GetMJ2000PrecVelocity(theData.tPrecTime) - ssb->GetMJ2000PrecVelocity(theData.tPrecTime)) -      // velocity of origin1 w.r.t SSB at transmit time tPrecTime in MJ2000Eq coordinate system
                          (origin2->GetMJ2000PrecVelocity(theData.rPrecTime) - ssb->GetMJ2000PrecVelocity(theData.rPrecTime));       // velocity of origin2 w.r.t SSB at receive time rPrecTime in MJ2000Eq coordinate system
//   theData.j2kOriginVel = origin2->GetMJ2000PrecVelocity(theData.rPrecTime) -
//                          origin1->GetMJ2000PrecVelocity(theData.tPrecTime);

//   theData.tVel = theData.tNode->GetMJ2000PrecVelocity(theData.tPrecTime);    // fix bug. It is incorrect because tVel is velocity of the transmited participant coordinate system MJ2000Eq. It has origin different from MJ2000 body
//   theData.rVel = theData.rNode->GetMJ2000PrecVelocity(theData.rPrecTime);    // fix bug. It is incorrect because tVel is velocity of the received participant coordinate system MJ2000Eq. It has origin different from MJ2000 body
   theData.rangeRateVecInertial = theData.rVel - theData.j2kOriginVel - theData.tVel;

   // 2. Compute the velocities of the participants in their own frames
   CalculateRangeVectorObs();                              // R_Transmitter_j2k, R_receiver_j2k, and R_Obs_j2k matrixes are updated in CalculateRangevectorObs() function
   //theData.tVelTcs = theData.tJ2kRotation * theData.tVel;
   //theData.rVelRcs = theData.rJ2kRotation * theData.rVel;

   // 3. Compute rangeRateVecObs
   if (theData.stationParticipant)
      theData.rangeRateVecObs = RDot_Obs_j2k*theData.rangeVecInertial + R_Obs_j2k*theData.rangeRateVecInertial;
   else
      theData.rangeRateVecObs = theData.rangeRateVecInertial;

}



Real SignalBase::GetParamDerivative(GmatBase *forObj, std::string paramName, GmatBase *associateObj)
{
   //MessageInterface::ShowMessage("SignalBase::GetParamDerivative(forObj = <%s>, paramName = <%s>, associateObj = <%s>) start\n", forObj->GetName().c_str(), paramName.c_str(), (associateObj == NULL?"":associateObj->GetName().c_str()));
   // Get index for paramName
   StringArray stmElemNames;
   std::string paramFullName;
   if (associateObj)
   {
      // associateObj is a Spacecraft object (example: estSat.Plate1.DiffuseFraction, associateObj is estSat, forObj is Plate1, and paramName is DiffuseFraction)
      stmElemNames = associateObj->GetStringArrayParameter("StmElementNames");
      paramFullName = forObj->GetName() + "." + paramName;
   }
   else
   {
	   if (forObj->IsOfType("SpaceCraft"))
	   {
		   // forObj is a Spacecraft object (example: estSat.Cr, forObj is estSat, paramName is Cr)
		   stmElemNames = forObj->GetStringArrayParameter("StmElementNames");
		   paramFullName = paramName;
	   }
   }

   UnsignedInt paramIndex = 0;
   bool found = false;
   //for (int i = 0; i < stmElemNames.size(); i++)
   //   MessageInterface::ShowMessage("for i = %d: stmElemNames[%d] = <%s>\n", i, i, stmElemNames[i].c_str());

   for (int i = 0; i < stmElemNames.size(); i++)
   {
      if (stmElemNames.at(i) == "CartesianState" || stmElemNames.at(i) == "KeplerianState")
         paramIndex += 0;  // vector that GetCDerivativeVector() returns does not include cartesian state
      else if (stmElemNames.at(i) == paramFullName)
      {
         found = true;
         break;
      }
      else
      {
         std::string name = stmElemNames.at(i);
         size_t pos = name.find_last_of('.');
         if (pos != name.npos)
            name = name.substr(pos + 1);
         paramIndex += forObj->GetEstimationParameterSize(forObj->GetParameterID(name));
      }
   }

   if (!found)
   {
      //MessageInterface::ShowMessage("Not found paramFullName <%s> in StmElementNames: deriv = 0.0\n", paramFullName.c_str());
      //MessageInterface::ShowMessage("SignalBase::GetParamDerivative(forObj = <%s>) end\n", forObj->GetName().c_str());
      return 0.0;
   }

   // Get C derivative vector
   Rvector dVector;
   if (associateObj)
      GetCDerivativeVector(associateObj, dVector, paramFullName);
   else
      GetCDerivativeVector(forObj, dVector, paramFullName);

   // Get paramName partial derivative
   Real deriv = dVector[paramIndex];

   //MessageInterface::ShowMessage("Found parameter with name <%s> in StmElementNames: deriv = %.12le    Index = %d\n", paramFullName.c_str(), deriv, paramIndex);
   //MessageInterface::ShowMessage("SignalBase::GetParamDerivative(forObj = <%s>) end\n", forObj->GetName().c_str());
   return deriv;
}


Real SignalBase::GetCrDerivative(GmatBase *forObj)
{
   // get index for Cr
   StringArray stmElemNames = forObj->GetStringArrayParameter("StmElementNames");
   UnsignedInt CrIndex = 0;
   bool found = false;
   for (int i = 0; i < stmElemNames.size(); i++) 
   {
      if (stmElemNames.at(i) == "CartesianState" || stmElemNames.at(i) == "KeplerianState")
         CrIndex += 0;  // vector that GetCDerivativeVector() returns does not include cartesian state
      else if ((stmElemNames.at(i) == "Cr") || (stmElemNames.at(i) == "SPADSRPScaleFactor"))
      {
         found = true;
         break;
      }
      else
      {
         CrIndex += forObj->GetEstimationParameterSize(forObj->GetParameterID(stmElemNames.at(i)));
      }
   }

   if (!found)
   {
      return 0.0;
   }

   // Get C derivative vector
   Rvector dVector;
   GetCDerivativeVector(forObj, dVector, "Cr");

   // Get Cr partial derivative
   Real deriv = dVector[CrIndex];

   return deriv;
}


Real SignalBase::GetCdDerivative(GmatBase *forObj)
{
   // get index for Cd
   StringArray stmElemNames = forObj->GetStringArrayParameter("StmElementNames");
   UnsignedInt CdIndex = 0;
   bool found = false;
   for (int i = 0; i < stmElemNames.size(); i++) {
      if (stmElemNames.at(i) == "CartesianState" || stmElemNames.at(i) == "KeplerianState")
         CdIndex += 0;  // vector that GetCDerivativeVector() returns does not include cartesian state
      else if ((stmElemNames.at(i) == "Cd") || (stmElemNames.at(i) == "SPADDragScaleFactor"))
      {
         found = true;
         break;
      }
      else 
      {
         CdIndex += forObj->GetEstimationParameterSize(forObj->GetParameterID(stmElemNames.at(i)));
      }
   }

   if (!found)
      return 0.0;

   // Get C derivative vector
   Rvector dVector;
   GetCDerivativeVector(forObj, dVector, "Cd");

   // Get Cr partial derivative
   Real deriv = dVector[CdIndex];
   
   return deriv;
}


Real SignalBase::GetADSFDerivative(GmatBase *forObj)
{
   // get index for Cd
   StringArray stmElemNames = forObj->GetStringArrayParameter("StmElementNames");
   UnsignedInt AdsfIndex = 0;
   bool found = false;
   for (int i = 0; i < stmElemNames.size(); i++) {
      if (stmElemNames.at(i) == "CartesianState" || stmElemNames.at(i) == "KeplerianState")
         AdsfIndex += 0;  // vector that GetCDerivativeVector() returns does not include cartesian state
      else if (stmElemNames.at(i) == "AtmosDensityScaleFactor")
      {
         found = true;
         break;
      }
      else
      {
         AdsfIndex += forObj->GetEstimationParameterSize(forObj->GetParameterID(stmElemNames.at(i)));
      }
   }

   if (!found)
      return 0.0;

   // Get C derivative vector
   Rvector dVector;
   GetCDerivativeVector(forObj, dVector, "AtmosDensityScaleFactor");

   // Get ADSF partial derivative
   Real deriv = dVector[AdsfIndex];

   return deriv;
}

// Added for Thrust Scale Factor Solve For
Real SignalBase::GetTSFDerivative(GmatBase *forObj, const std::string &paramName)
{
   // paramName looks like <segmentName>.TSF_Epsilon
   StringArray parts = GmatStringUtil::SeparateBy(paramName, ".");
   std::string tsfName = parts.at(0) + ".ThrustScaleFactor";

   // get index for TSF
   StringArray stmElemNames = forObj->GetStringArrayParameter("StmElementNames");
   UnsignedInt TSFIndex = 0;
   bool found = false;
   for (int i = 0; i < stmElemNames.size(); i++) {
      if (stmElemNames.at(i) == "CartesianState" || stmElemNames.at(i) == "KeplerianState")
         TSFIndex += 0;  // vector that GetCDerivativeVector() returns does not include cartesian state
      else if (stmElemNames.at(i) == tsfName) {
         found = true;
         break;
      }
      else {
         TSFIndex += forObj->GetEstimationParameterSize(forObj->GetParameterID(stmElemNames.at(i)));
      }
   }

   if (!found)
      return 0.0;

   // Get C derivative vector
   Rvector dVector;
   GetCDerivativeVector(forObj, dVector, "TSF");

	// Get TSF partial derivative
	Real deriv = dVector[TSFIndex];

	return deriv;
}


void SignalBase::GetCDerivativeVector(GmatBase *forObj, Rvector &deriv, const std::string &solveForType)
{
   // 1. Calculate phi matrix
   bool forTransmitter = true;
   if (theData.rNode == forObj)
      forTransmitter = false;
   else
   {
      if (theData.tNode != forObj)
         throw MeasurementException(solveForType + " derivative requested, but "
            "neither participant is the \"for\" object");
   }
   Rmatrix phi = (forTransmitter ? (theData.tSTM*theData.tSTMtm.Inverse()) : (theData.rSTM*theData.rSTMtm.Inverse()));

   // 2. Calculate E matrix
   Integer m = phi.GetNumColumns() - 6;
   Rmatrix E(3,m);
   for (Integer i = 0; i < 3; ++i)
   {
      for (Integer j = 0; j < m; ++j)
      {
         E(i, j) = phi(i, j + 6);
      }
   }
   
   // 3. Calculate: sign * R * phi
   Real sign = (forTransmitter ? -1.0 : 1.0);

   Rmatrix33 body2FK5_matrix = (forTransmitter ? theData.tJ2kRotation :
      theData.rJ2kRotation);
   Rmatrix tempMatrix(3,m);
   // tempMatrix = body2FK5_matrix * E;
   for (Integer i = 0; i < 3; ++i)
   {
      for (Integer j = 0; j < m; ++j)
      {
         tempMatrix(i, j) = 0.0;
         for (Integer k = 0; k < 3; ++k)
            tempMatrix(i, j) += sign*body2FK5_matrix(i, k) * E(k, j);
      }
   }


   // 4. Calculate range unit vector
   Rvector3 rangeVec = theData.rangeVecInertial;
   Rvector3 unitRange = rangeVec / rangeVec.GetMagnitude();


   // 5. Calculate C vector derivative
   deriv.SetSize(m);
   for (Integer j = 0; j < m; ++j)
   {
      deriv[j] = 0.0;
      for (Integer i = 0; i < 3; ++i)
         deriv[j] += unitRange[i]*tempMatrix(i, j);
   }
}


//------------------------------------------------------------------------------
// void GetRangeDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
//       Rvector &deriv)
//------------------------------------------------------------------------------
/**
 * Calculates the range derivative of a signal in a measurement
 *
 * @param forObj Pointer for the participants that hold the w.r.t field
 * @param wrtR Flag indicating if derivative with respect to position is desired
 * @param wrtV Flag indicating if derivative with respect to velocity is desired
 * @param deriv The structure that gets filled with derivative data
 */
//------------------------------------------------------------------------------
void SignalBase::GetRangeDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
      Rvector &deriv)
{
   Rmatrix derivMatrix;
   if (wrtR && wrtV)
   {
      //derivMatrix.SetSize(6, 6);
      derivMatrix.SetSize(3, 6);
   }
   else
      derivMatrix.SetSize(3,3);
   
   GetRangeVectorDerivative(forObj, wrtR, wrtV, derivMatrix);

   Rvector3 rangeVec = theData.rangeVecInertial;
   
   Rvector3 temp;
   Rmatrix33 mPart;
   Rvector3 unitRange = rangeVec / rangeVec.GetMagnitude();

   if (wrtR)
   {
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            mPart(i,j) = derivMatrix(i,j);

      temp = unitRange * mPart;
      for (Integer i = 0; i < 3; ++i)
         deriv[i] = temp(i);
   }
   if (wrtV)
   {
      Integer offset = (wrtR ? 3 : 0);
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
         {
            //mPart(i, j) = derivMatrix(i + offset, j + offset);
            mPart(i, j) = derivMatrix(i, j + offset);
         }

      temp = unitRange * mPart;
      for (Integer i = 0; i < 3; ++i)
         deriv[i+offset] = temp(i);
   }
}


//------------------------------------------------------------------------------
// void GetRangeVectorDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
//       Rmatrix& derivMatrix)
//------------------------------------------------------------------------------
/**
 * Calculates the range vector derivative of a signal in a measurement
 *
 * @param forObj Pointer for the participants that hold the w.r.t field
 * @param usingData SignalData for the signal being evaluated
 * @param wrtR Flag indicating if derivative with respect to position is desired
 * @param wrtV Flag indicating if derivative with respect to velocity is desired
 * @param derivMatrix The structure that gets filled with derivative data
 */
//------------------------------------------------------------------------------
void SignalBase::GetRangeVectorDerivative(GmatBase *forObj, bool wrtR,
      bool wrtV, Rmatrix& derivMatrix)
{
   bool forTransmitter = true;
   if (theData.rNode == forObj)
      forTransmitter = false;
   else
   {
      if (theData.tNode != forObj)
         throw MeasurementException("Range vector derivative requested, but "
               "neither participant is the \"for\" object");
   }

   // For now ignoring the barycenter piece -- we'll add it once core code works
   Rvector3 rangeVec = theData.rangeVecInertial;      // Check sign?

   // phi(t1,tm) = phi(t1, t0)* Inv(phi(tm, t0))   where: t0 is initial epoch; tm is measurement time; t1 is either transmit time or receive time
   //Rmatrix phi = (forTransmitter ? theData.tSTM : theData.rSTM);

   /// @todo Adjust the following code for multiple spacecraft

   Rmatrix phi = (forTransmitter ? (theData.tSTM*theData.tSTMtm.Inverse()) : (theData.rSTM*theData.rSTMtm.Inverse()));

   // matrix phi = [A B C
   //               D E F
   //               G H I]

   // Check: Not using old STM inverse approach -- STMs computed from initial
   // (measurement epoch based) state, so STM inverse is identity
   Rmatrix33 A, B;
   for (Integer i = 0; i < 3; ++i)
   {
      for (Integer j = 0; j < 3; ++j)
      {
         if (wrtR)
            A(i, j) = phi(i, j);              // sub-matrix A of state transition matrix Phi in Equation 6.31 in GMAT MathSpec
         if (wrtV)
         {
            //B(i,j) = phi(i+3,j+3);     // ekf mod 12/16
            B(i, j) = phi(i, j + 3);     // sub-matrix B of state transition matrix Phi in Equation 6.31 in GMAT MathSpec
         }
      }
   }
   Real sign = (forTransmitter ? -1.0 : 1.0);

   Rmatrix33 temp;
   Rmatrix33 body2FK5_matrix = (forTransmitter ? theData.tJ2kRotation :
         theData.rJ2kRotation);

   if (wrtR)
   {
      // temp = dataToUse.rInertial2obj * A;
      temp = body2FK5_matrix * A;

      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            derivMatrix(i,j) = sign * temp(i,j);
   }
   if (wrtV)
   {
      // temp = dataToUse.rInertial2obj * B;
     temp = body2FK5_matrix * B;

      Integer offset = (wrtR ? 3 : 0);
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
         {
            //derivMatrix(i+offset,j+offset) = sign * temp(i,j);
            derivMatrix(i, j + offset) = sign * temp(i, j);              // derivMatrix = [ [drhoVec/dr]  [drhoVec/dv] ] . It is a 3x6 matrix
         }
   }
}


//------------------------------------------------------------------------------
// void GetRangeRateDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
//       Rvector &deriv)
//------------------------------------------------------------------------------
/**
* Calculates the range rate derivative of a signal in a measurement
*
* @param forObj Pointer for the participants that hold the w.r.t field
* @param wrtR Flag indicating if derivative with respect to position is desired
* @param wrtV Flag indicating if derivative with respect to velocity is desired
* @param deriv The structure that gets filled with derivative data
*/
//------------------------------------------------------------------------------
void SignalBase::GetRangeRateDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
   Rvector &deriv)
{
   Rmatrix drhoVec_dX(3,6), drhoVecDot_dX(3,6);
   GetRangeVectorDerivative(forObj, wrtR, wrtV, drhoVec_dX);
   GetRangeRateVectorDerivative(forObj, wrtR, wrtV, drhoVecDot_dX);

   Rvector3 rhoVec = theData.rangeVecInertial;
   Rvector3 rhoHat = rhoVec / rhoVec.GetMagnitude();
   Real rho = rhoVec.GetMagnitude();
   Rvector3 rhoVecDot = theData.rangeRateVecInertial;

   // Calculate matrix: drhoHat/drhoVec = [I-OuterProduct(rhoHat,rhoHat)]/|rhoVec|
   Rmatrix drhoHat_drhoVec(3,3);
   for (Integer row = 0; row < 3; ++row)
   {
      for (Integer col = 0; col < 3; ++col)
      {
         if (row == col)
            drhoHat_drhoVec(row, col) = 1.0 - rhoHat[row] * rhoHat[col];
         else
            drhoHat_drhoVec(row, col) = -(rhoHat[row] * rhoHat[col]);
         drhoHat_drhoVec(row, col) = drhoHat_drhoVec(row, col) / rho;
      }
   }

   Rmatrix drhoHat_dX = drhoHat_drhoVec * drhoVec_dX;       // [drhoHat/dX] = [drHat/drhoVec] * [drhoVec/dX] 

   Rvector6 drhoDot_dX;
   for (Integer col = 0; col < 6; ++col)
   {
      drhoDot_dX[col] = 0.0;
      for (Integer row = 0; row < 3; ++row)
      {
         drhoDot_dX[col] += (rhoVecDot[row] * drhoHat_dX(row, col));
      }
      
      for (Integer row = 0; row < 3; ++row)
      {
         drhoDot_dX[col] += (rhoHat[row] * drhoVecDot_dX(row, col));
      }
   }

   if (wrtR)
   {
      // Caluclate drhoDot/dr by extracting from drhoDot/dX vector
      for (Integer i = 0; i < 3; ++i)
         deriv[i] = drhoDot_dX(i);
   }
   if (wrtV)
   {
      // Calculate drhoDot/dv by extracting from drhoDot/dX vector
      Integer offset = (wrtR ? 3 : 0);
      for (Integer i = 0; i < 3; ++i)
         deriv[i + offset] = drhoDot_dX(i+3);
   }
}


//------------------------------------------------------------------------------
// void GetRangeRateVectorDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
//       Rmatrix& derivMatrix)
//------------------------------------------------------------------------------
/**
* Calculates the range rate vector derivative of a signal in a measurement
*
* @param forObj Pointer for the participants that hold the w.r.t field
* @param usingData SignalData for the signal being evaluated
* @param wrtR Flag indicating if derivative with respect to position is desired
* @param wrtV Flag indicating if derivative with respect to velocity is desired
* @param derivMatrix The structure that gets filled with derivative data
*/
//------------------------------------------------------------------------------
void SignalBase::GetRangeRateVectorDerivative(GmatBase *forObj, bool wrtR,
   bool wrtV, Rmatrix& derivMatrix)
{
   bool forTransmitter = true;
   if (theData.rNode == forObj)
      forTransmitter = false;
   else
   {
      if (theData.tNode != forObj)
         throw MeasurementException("Range rate vector derivative requested, but "
            "neither participant is the \"for\" object");
   }

   // For now ignoring the barycenter piece -- we'll add it once core code works
   Rvector3 rangeVec = theData.rangeVecInertial;      // Check sign?

   // phi(t1,tm) = phi(t1, t0)* Inv(phi(tm, t0))   where: t0 is initial epoch; tm is measurement time; t1 is either transmit time or receive time
   Rmatrix phi = (forTransmitter ? (theData.tSTM*theData.tSTMtm.Inverse()) : (theData.rSTM*theData.rSTMtm.Inverse()));

   // extract state transition matrix for X=(r,v)
   Rmatrix phiX(6,6);
   for (Integer row = 0; row < 6; ++row)
      for (Integer col = 0; col < 6; ++col)
         phiX(row, col) = phi(row, col);

   Rmatrix33 body2FK5_matrix = (forTransmitter ? theData.tJ2kRotation :
      theData.rJ2kRotation);

   Rmatrix33 body2FK5_matrixDot = (forTransmitter ? theData.tJ2kRotationDot :
      theData.rJ2kRotationDot);

   // Set: matrixQ = [[dR/dt], [R]]        . It is a 3x6 matrix
   Rmatrix matrixQ(3, 6);
   for (Integer row = 0; row < 3; ++row)
   {
      for (Integer col = 0; col < 6; ++col)
      {
         if (col < 3)
            matrixQ(row, col) = body2FK5_matrixDot(row, col);
         else
            matrixQ(row, col) = body2FK5_matrix(row, col - 3);
      }
   }

   Real sign = (forTransmitter ? -1.0 : 1.0);
   Rmatrix drhoVecDot_dX = (matrixQ * phiX) * sign;

   if (wrtR)
   {
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            derivMatrix(i, j) = drhoVecDot_dX(i, j);
   }
   
   if (wrtV)
   {
      Integer offset = (wrtR ? 3 : 0);
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            derivMatrix(i, j + offset) = drhoVecDot_dX(i, j+3);
   }
}


//------------------------------------------------------------------------------
// void UpdateRotationMatrix(Real atEpoch, const std::string& whichOne)
//------------------------------------------------------------------------------
/**
 * Updates the rotation matrices for the passed-in epoch
 *
 * @param atEpoch The epoch of the update
 * @param whichOne Identity of the matrix to update, or "All" for all
 */
//------------------------------------------------------------------------------
void SignalBase::UpdateRotationMatrix(Real atEpoch, const std::string& whichOne)
{
   if ((theData.tNode == NULL) && (theData.rNode != NULL))
   {
      // for GPS measurement
      R_j2k_Receiver = I33;
      theData.rJ2kRotation = I33;
      R_j2k_Transmitter = I33;
      theData.tJ2kRotation = I33;
      R_Obs_Receiver = I33;
      R_Obs_Transmitter = I33;
      R_Obs_j2k = I33;

      RDot_Obs_Receiver = zero33;
      RDot_Obs_Transmitter = zero33;
      RDot_Obs_j2k = zero33;

      theData.tJ2kRotationDot = zero33;
      theData.rJ2kRotationDot = zero33;

      return;
   }

   if ((theData.tNode->IsOfType(Gmat::GROUND_STATION)) ||
       (theData.rNode->IsOfType(Gmat::GROUND_STATION)))
   {
      Rvector6   dummyIn(1.0,2.0,3.0,  4.0,5.0,6.0);
      Rvector6   dummyOut;
      A1Mjd      itsEpoch(atEpoch);
      bool updated = false;

      if ((whichOne == "All") || (whichOne == "j2k_2"))
      {
         converter.Convert(itsEpoch,dummyIn,rcs,dummyOut,j2k);
         R_j2k_Receiver = converter.GetLastRotationMatrix();
         //               RDot_j2k_2 = converter.GetLastRotationDotMatrix();
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,rcs);
         theData.rJ2kRotation    = converter.GetLastRotationMatrix();
         theData.rJ2kRotationDot = converter.GetLastRotationDotMatrix();
         updated = true;
      }
      if ((whichOne == "All") || (whichOne == "j2k_1"))
      {
         converter.Convert(itsEpoch,dummyIn,tcs,dummyOut,j2k);
         R_j2k_Transmitter    = converter.GetLastRotationMatrix();
         //               RDot_j2k_1 = converter.GetLastRotationDotMatrix();
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,tcs);
         theData.tJ2kRotation    = converter.GetLastRotationMatrix();
         theData.tJ2kRotationDot = converter.GetLastRotationDotMatrix();
         updated = true;
      }
      if ((whichOne == "All") || (whichOne == "o_2"))
      {
         converter.Convert(itsEpoch,dummyIn,rcs,dummyOut,ocs);
         R_Obs_Receiver      = converter.GetLastRotationMatrix();
         RDot_Obs_Receiver   = converter.GetLastRotationDotMatrix();
         updated = true;
      }
      if ((whichOne == "All") || (whichOne == "o_1"))
      {
         converter.Convert(itsEpoch,dummyIn,tcs,dummyOut,ocs);
         R_Obs_Transmitter      = converter.GetLastRotationMatrix();
         RDot_Obs_Transmitter   = converter.GetLastRotationDotMatrix();
         updated = true;
      }
      if ((whichOne == "All") || (whichOne == "o_j2k"))
      {
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,ocs);
         R_Obs_j2k    = converter.GetLastRotationMatrix();
         RDot_Obs_j2k = converter.GetLastRotationDotMatrix();
         updated = true;
      }

     if (updated == false)
     {
        std::stringstream ss;
        ss << "Error: UpdateRotationMatrix() input parameter whichOne (" << whichOne << ") has an invalid value\n";
        throw MeasurementException(ss.str());
      }
   }
   else
   {
      // When both participants are spacecraft, all frames are inertial, so all
      // rotation matrices are I33 (which is the default on creation) and all
      // Rotation Dot matrices are zero33
      R_j2k_Receiver       = I33;
      theData.rJ2kRotation = I33;
      R_j2k_Transmitter    = I33;
      theData.tJ2kRotation = I33;
      R_Obs_Receiver       = I33;
      R_Obs_Transmitter    = I33;
      R_Obs_j2k            = I33;

      RDot_Obs_Receiver    = zero33;
      RDot_Obs_Transmitter = zero33;
      RDot_Obs_j2k         = zero33;

      theData.tJ2kRotationDot = zero33;
      theData.rJ2kRotationDot = zero33;
   }
}


//-----------------------------------------------------------------------------
// Integer GetParmIdFromEstID(Integer forId, GmatBase *obj)
//-----------------------------------------------------------------------------
/**
 * Uses a measurement ID and object to convert that id into a parameter ID
 *
 * @param forId The measurement ID
 * @param obj The object that owns the id
 *
 * @return The parameter ID on the object
 */
//-----------------------------------------------------------------------------
Integer SignalBase::GetParmIdFromEstID(Integer forId, GmatBase *obj)
{
   return forId - obj->GetType() * 250;
}


void SignalBase::MoveToEpoch(const GmatTime theEpoch, bool epochAtReceive,
      bool moveAll)
{
   GmatTime t = theEpoch;
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Called MoveToEpoch(%.12lf, %s%s)\n",
            t.GetMjd(),
            (epochAtReceive ? "Epoch at Receive" :  "Epoch at Transmit"),
            (moveAll ? ", Moving all participants" : ""));
   #endif

   // 1. Propagate receive node at time theEpoch and update its SignalData
   if (epochAtReceive || moveAll)
   {

      /// @todo Adjust the following code for multiple spacecraft


      Real dt = (theEpoch - theData.rPrecTime).GetTimeInSec();
      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   dt = (%.12lf - %.12lf)*86400 => %.12le\n",
               t.GetMjd(), theData.rPrecTime.GetMjd(), dt);
      #endif
      
      // Update model epoch
      ODEModel *ode = NULL;
      if (theData.tNode->IsOfType(Gmat::SPACEOBJECT))
      {
         if (theData.tPropagator != NULL)
         {
            if (theData.tPropagator->GetPropagator()->UsesODEModel())
            {
               ode = theData.tPropagator->GetODEModel();
               ode->UpdateInitialData();
            }
            else
            {
               theData.tPropagator->GetPropagator()->UpdateSpaceObjectGT(theData.rPrecTime);
            }
         }
      }
      if (theData.rNode->IsOfType(Gmat::SPACEOBJECT))
      {
         if (theData.rPropagator != NULL)
         {
            if (theData.rPropagator->GetPropagator()->UsesODEModel())
            {
               ode = theData.rPropagator->GetODEModel();
               ode->UpdateInitialData();
            }
            else
            {
               theData.rPropagator->GetPropagator()->UpdateSpaceObjectGT(theData.rPrecTime);
            }
         }
      }
      
      if (dt != 0.0)
      {
         StepParticipant(dt, false);
      }
      else
      {
         Rvector6 state;
         Rmatrix stm;           // state transition matrix
         if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         {
            state = theData.rNode->GetMJ2000PrecState(theEpoch);

            // For a ground station, its STM is I matrix
            stm.SetSize(6,6,true);
            for (UnsignedInt i = 0; i < 6; ++i)
               stm(i,i) = 1.0;
         }
         else
         {
            // Retrieve spacecraft data from the propagator
            const Real *pstate =
                  theData.rPropagator->GetPropagator()->AccessOutState();

            // set value for state and STM
            // This step is used to convert spacecraft's state to Spacecraft.CoordinateSystem
            state.Set(pstate);
            SpacePoint* spacecraftOrigin = ((Spacecraft*)(theData.rNode))->GetOrigin();                 // the origin of the transmit spacecraft's cooridinate system   // fix bug GMT-5364

            SpacePoint* propOrigin = nullptr;
            if (theData.rPropagator->GetPropagator()->UsesODEModel())
               propOrigin = theData.rPropagator->GetODEModel()->GetForceOrigin();        // the origin of the coordinate system used in forcemodel       // fix bug GMT-5364
            else
               propOrigin = theData.rPropagator->GetPropagator()->GetPropOrigin();

            state = state + (propOrigin->GetMJ2000PrecState(theData.rPrecTime) - spacecraftOrigin->GetMJ2000PrecState(theData.rPrecTime));                        // fix bug GMT-5364

			   // Set size for stm
            Integer stmRowCount = theData.rNode->GetIntegerParameter("FullSTMRowCount");
            stm.SetSize(stmRowCount, stmRowCount, true);

			   // Get start index of STM
			   const std::vector<ListItem*>* map = theData.rPropagator->GetPropStateManager()->GetStateMap();
            UnsignedInt stmStartIndex = -1;
            for (UnsignedInt index = 0; index < map->size(); ++index)
	   		{
               if (((*map)[index]->object == theData.rNode) && ((*map)[index]->elementName == "STM"))
               {
                  stmStartIndex = index;
                  break;
               }
			   }

			   // Set value for stm
			   for (UnsignedInt i = 0; i < stmRowCount; ++i)
               for (UnsignedInt j = 0; j < stmRowCount; ++j)
                  stm(i,j) = pstate[stmStartIndex + i*stmRowCount + j];
         }

         theData.rLoc = state.GetR();
         theData.rVel = state.GetV();
         // Whenever updating participant's state in signal data, it needs to update participant's STM
         if (theData.rSTM.GetNumRows() != stm.GetNumRows())
            theData.rSTM.SetSize(stm.GetNumRows(), stm.GetNumRows(), true);
         theData.rSTM = stm;

         SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
         theData.rOStateSSB = rcs->GetOrigin()->GetMJ2000PrecState(theData.rPrecTime) - 
                            ssb->GetMJ2000PrecState(theData.rPrecTime);
      }
      #ifdef DEBUG_MOVETOEPOCH
         if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
            MessageInterface::ShowMessage("Move to epoch: %.12lf  participant %s  position in EarthMJ2000Eq [%.12lf   %.12lf   %.12lf]   velocity [%.12lf   %.12lf   %.12lf]\n", t.GetMjd(), theData.rNode->GetName().c_str(), theData.rLoc[0], theData.rLoc[1], theData.rLoc[2], theData.rVel[0], theData.rVel[1], theData.rVel[2]);
         else
            MessageInterface::ShowMessage("Move to epoch: %.12lf  participant %s  position in %s [%.12lf   %.12lf   %.12lf]   velocity [%.12lf   %.12lf   %.12lf]\n", t.GetMjd(), theData.rNode->GetName().c_str(), theData.rNode->GetStringParameter("CoordinateSystem").c_str(),theData.rLoc[0], theData.rLoc[1], theData.rLoc[2], theData.rVel[0], theData.rVel[1], theData.rVel[2]);
      #endif
   }

   // 2. Propagate transmit node at time theEpoch and update it's SignalData
   if (!epochAtReceive || moveAll)
   {
     Real dt = (theEpoch - theData.tPrecTime).GetTimeInSec();
     #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   dt = (%.12lf - %.12lf)*86400 => %.12le\n",
               t.GetMjd(), theData.tPrecTime.GetMjd(), dt);
      #endif
      if (dt != 0.0)
      {
         StepParticipant(dt, true);
      }
      else
      {
         Rvector6 state;
         Rmatrix stm;         // state transition matrix
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         {
            state = theData.tNode->GetMJ2000PrecState(theEpoch);

            // For a ground station, its STM is a 6x6 I matrix
            stm.SetSize(6,6,true);
            for (UnsignedInt i = 0; i < 6; ++i)
               stm(i,i) = 1.0;
         }
         else
         {
            // Retrieve spacecraft data from the propagator
            const Real *pstate =
                  theData.tPropagator->GetPropagator()->AccessOutState();

            // set value for state and STM
            // This step is used to convert spacecraft's state to Spacecraft.CoordinateSystem
            state.Set(pstate);
            SpacePoint* spacecraftOrigin = ((Spacecraft*)(theData.tNode))->GetOrigin();                 // the origin of the transmit spacecraft's cooridinate system   // fix bug GMT-5364

            SpacePoint* propOrigin = nullptr;
            if (theData.tPropagator->GetPropagator()->UsesODEModel())
               propOrigin = theData.tPropagator->GetODEModel()->GetForceOrigin();        // the origin of the coordinate system used in forcemodel       // fix bug GMT-5364
            else
               propOrigin = theData.tPropagator->GetPropagator()->GetPropOrigin();

            state = state + (propOrigin->GetMJ2000PrecState(theData.tPrecTime) - spacecraftOrigin->GetMJ2000PrecState(theData.tPrecTime));                        // fix bug GMT-5364

			   // Set size for stm
            Integer stmRowCount = theData.tNode->GetIntegerParameter("FullSTMRowCount");
            stm.SetSize(stmRowCount, stmRowCount, true);
            
			   // Get start index of STM
			   const std::vector<ListItem*>* map = theData.tPropagator->GetPropStateManager()->GetStateMap();
            UnsignedInt stmStartIndex = -1;
            for (UnsignedInt index = 0; index < map->size(); ++index)
            {
               if (((*map)[index]->object == theData.tNode) && ((*map)[index]->elementName == "STM"))
               {
                  stmStartIndex = index;
                  break;
               }
            }
            
			   // Set value for stm
            for (UnsignedInt i = 0; i < stmRowCount; ++i)
               for (UnsignedInt j = 0; j < stmRowCount; ++j)
                  stm(i,j) = pstate[stmStartIndex + i*stmRowCount + j];
         }

         theData.tLoc = state.GetR();
         theData.tVel = state.GetV();
         // Whenever updating participant's state in signal data, it needs to update participant's STM
         if (theData.tSTM.GetNumRows() != stm.GetNumRows())
            theData.tSTM.SetSize(stm.GetNumRows(), stm.GetNumRows(), true);
         theData.tSTM = stm;

         SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
         theData.tOStateSSB = tcs->GetOrigin()->GetMJ2000PrecState(theData.tPrecTime) - 
                            ssb->GetMJ2000PrecState(theData.tPrecTime);
      }
      #ifdef DEBUG_MOVETOEPOCH
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
            MessageInterface::ShowMessage("Move to epoch: %.12lf  participant %s  position in EarthMJ2000Eq [%.12lf   %.12lf   %.12lf]   velocity [%.12lf   %.12lf   %.12lf]\n", t.GetMjd(), theData.tNode->GetName().c_str(), theData.tLoc[0], theData.tLoc[1], theData.tLoc[2], theData.tVel[0], theData.tVel[1], theData.tVel[2]);
         else
            MessageInterface::ShowMessage("Move to epoch: %.12lf  participant %s  position in %s [%.12lf   %.12lf   %.12lf]   velocity [%.12lf   %.12lf   %.12lf]\n", t.GetMjd(), theData.tNode->GetName().c_str(), theData.tNode->GetStringParameter("CoordinateSystem").c_str(), theData.tLoc[0], theData.tLoc[1], theData.tLoc[2], theData.tVel[0], theData.tVel[1], theData.tVel[2]);
      #endif
   }
}

//------------------------------------------------------------------------------
// void SetIonosphereCache(SignalDataCache::SimpleSignalDataCache* cache)
//------------------------------------------------------------------------------
/**
 * Sets the ionosphere cache for the signal
 *
 * @param cache The ionosphere cache
 */
 //------------------------------------------------------------------------------
void SignalBase::SetIonosphereCache(SignalDataCache::SimpleSignalDataCache * cache)
{
   // Set it to all signals in the path
   ionosphereCache = cache;

   if (next)
      next->SetIonosphereCache(cache);
}


//------------------------------------------------------------------------------
// void SetStrandId(unsigned long id)
//------------------------------------------------------------------------------
/**
 * Sets the strand ID
 *
 * @param id The strand id
 */
 //------------------------------------------------------------------------------
void SignalBase::SetStrandId(unsigned long id)
{
   strandId = id;
}

//------------------------------------------------------------------------------
// void SetupHorp(Real height, Real angle, bool useOblateness)
//------------------------------------------------------------------------------
/**
 * Sets data used for signals that need height of ray path settings
 *
 * @param height The minimum height allowing a signal to be valid
 * @param angle The central angle used when determining if a signal in behind a body
 * @param useOblateness Toggle to use oblate bodies rather than spherical
 */
//------------------------------------------------------------------------------
void SignalBase::SetupHorp(Real height, Real angle, bool useOblateness, bool force)
{
   useHorp = true;
   forceHorp = force;
   horpHeight = height;
   horpAngle = angle;
   horpOblateness = useOblateness;
   if (next)
      next->SetupHorp(height, angle, useOblateness, force);
}

//------------------------------------------------------------------------------
// void SetPrevious(SignalBase* prev)
//------------------------------------------------------------------------------
/**
 * Build the backwards link for the doubly linked list
 *
 * @param prev Link to the owner of this signal in the list
 */
//------------------------------------------------------------------------------
void SignalBase::SetPrevious(SignalBase* prev)
{
   previous = prev;
}


//------------------------------------------------------------------------------
// bool StepParticipant(Real stepToTake, bool forTransmitter)
//------------------------------------------------------------------------------
/**
 * Steps a participant in time, and updates the J2k state buffer accordingly
 *
 * @param stepToTake The amount of time needed for the propagation
 * @param forTransmitter True to step the transmitter participant, false to
 *                       step the receiver
 *
 * @return true is a step succeeded, false if it failed
 */
//------------------------------------------------------------------------------
bool SignalBase::StepParticipant(Real stepToTake, bool forTransmitter)
{
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Called StepParticipant(%.12le, %s)\n",
            stepToTake, (forTransmitter ? "stepping transmitter" :
            "stepping receiver"));
   #endif
   bool retval = false;
   
   // 1. Get associated propagator for transmitter node (or receiver node)
   Propagator *prop = NULL;

   if (forTransmitter)
   {
      if (theData.tNode->IsOfType(Gmat::SPACEOBJECT))
      {
         if (theData.tPropagator != NULL)
            prop = theData.tPropagator->GetPropagator();
         else
            throw MeasurementException("The propagator for the transmitting "
                  "participant \"" + theData.transmitParticipant +
                  "\" has not been set in the signal that needs it");
      }
   }
   else
   {
      if (theData.rNode->IsOfType(Gmat::SPACEOBJECT))
      {
         if (theData.rPropagator != NULL)
            prop = theData.rPropagator->GetPropagator();
         else
            throw MeasurementException("The propagator for the receiving "
                  "participant \"" + theData.transmitParticipant +
                  "\" has not been set in the signal that needs it");
      }
   }

   
   // 2. Propagate transmitter node (or receiver node) for stepToTake and specify its state
   Rvector6 state;
   Rmatrix stm;         // state transition matrix

   GmatTime tPrecTimeNew = theData.tPrecTime + stepToTake / GmatTimeConstants::SECS_PER_DAY;
   GmatTime rPrecTimeNew = theData.rPrecTime + stepToTake / GmatTimeConstants::SECS_PER_DAY;

   if (prop)      // Handle spacecraft
   {
      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("Propagating numerically\n");
      #endif

      const Real *outState = prop->AccessOutState();

      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   ---> Before: %.12lf\n", state[0]);
      #endif
      
      retval = prop->Step(stepToTake);
      if (retval == false)
      {  
         StringArray propObjectTypes = prop->GetTypeNames();
         if (std::find(propObjectTypes.begin(), propObjectTypes.end(), "Integrator") != propObjectTypes.end() && prop->GetRealParameter("MinStep") != 0.0)
         {
            throw MeasurementException("The value of " + prop->GetName().substr(0, prop->GetName().length() - 6) + ".MinStep is too large "
               "for light time calculation. Lower the value or set it to 0 to resolve this issue.");
         }

      }

      /// @todo Adjust the following code for multiple spacecraft

      // This step is used to convert spacecraft's state to Spacecraft.CoordinateSystem
      state.Set(outState);
      if (forTransmitter)
      {
         SpacePoint* spacecraftOrigin = ((Spacecraft*)(theData.tNode))->GetOrigin();                 // the origin of the transmit spacecraft's coordinate system

         SpacePoint* propOrigin = nullptr;
         if (theData.tPropagator->GetPropagator()->UsesODEModel())
            propOrigin = theData.tPropagator->GetODEModel()->GetForceOrigin();        // the origin of the coordinate system used in force model       // fix bug GMT-5364
         else
            propOrigin = theData.tPropagator->GetPropagator()->GetPropOrigin();

         state = state + (propOrigin->GetMJ2000PrecState(tPrecTimeNew) - spacecraftOrigin->GetMJ2000PrecState(tPrecTimeNew));
      }
      else
      {
         SpacePoint* spacecraftOrigin = ((Spacecraft*)(theData.rNode))->GetOrigin();                 // the origin of the transmit spacecraft's coordinate system

         SpacePoint* propOrigin = nullptr;
         if (theData.rPropagator->GetPropagator()->UsesODEModel())
            propOrigin = theData.rPropagator->GetODEModel()->GetForceOrigin();        // the origin of the coordinate system used in force model       // fix bug GMT-5364
         else
            propOrigin = theData.rPropagator->GetPropagator()->GetPropOrigin();

         state = state + (propOrigin->GetMJ2000PrecState(rPrecTimeNew) - spacecraftOrigin->GetMJ2000PrecState(rPrecTimeNew));
      }
      
      // Set size for stm
      Integer stmRowCount;
      if (forTransmitter)
         stmRowCount = theData.tNode->GetIntegerParameter("FullSTMRowCount");
      else
         stmRowCount = theData.rNode->GetIntegerParameter("FullSTMRowCount");
      stm.SetSize(stmRowCount, stmRowCount, true);
      
      // Get start index of STM
	   UnsignedInt stmStartIndex = -1;
	   if (forTransmitter)
      {
	      const std::vector<ListItem*>* map = theData.tPropagator->GetPropStateManager()->GetStateMap();
         for (UnsignedInt index = 0; index < map->size(); ++index)
		   {
            if (((*map)[index]->object == theData.tNode) && ((*map)[index]->elementName == "STM"))
            {
               stmStartIndex = index;
               break;
            }
		   }
	   }
	   else
      {
         const std::vector<ListItem*>* map = theData.rPropagator->GetPropStateManager()->GetStateMap();
         for (UnsignedInt index = 0; index < map->size(); ++index)
         {
            if (((*map)[index]->object == theData.rNode) && ((*map)[index]->elementName == "STM"))
            {
               stmStartIndex = index;
               break;
            }
         }
      }

	  // Set value for stm
      for (UnsignedInt i = 0; i < stmRowCount; ++i)
         for (UnsignedInt j = 0; j < stmRowCount; ++j)
            stm(i,j) = outState[stmStartIndex + i*stmRowCount + j];
      
      // Buffer the STM
      if (forTransmitter)
      {
         if (theData.tSTM.GetNumRows() != stmRowCount)
            theData.tSTM.SetSize(stmRowCount, stmRowCount, true);
         theData.tSTM = stm;
      }
      else
      {
         if (theData.rSTM.GetNumRows() != stmRowCount)
            theData.rSTM.SetSize(stmRowCount, stmRowCount, true);
         theData.rSTM = stm;
      }
      
      
      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   ---> After %.12lf : %.12lf\n",
               prop->GetStepTaken(), state(0));
      #endif

      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   STM = %s\n", stm.ToString().c_str());
      #endif
   }
   else           // Handle ground stations and other analytic props
   {
      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("Propagating analytically\n");
      #endif
      if (forTransmitter)
      {
         state = theData.tNode->GetMJ2000PrecState(tPrecTimeNew);
      }
      else
      {
         state = theData.rNode->GetMJ2000PrecState(rPrecTimeNew);
      }
      
      // For ground station, its STM is a 6x6 I matrix
      stm.SetSize(6,6,true);
      for(UnsignedInt i = 0; i < 6; ++i)
         stm(i,i) = 1.0;
   }

   // 3. Set value for SignalData object associated to transmitter node (or receiver node)
   SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
   if (forTransmitter)
   {
      theData.tLoc = state.GetR();
      theData.tVel = state.GetV();
      // whenever updating participant's state, it needs to update its STM
      if (theData.tSTM.GetNumRows() != stm.GetNumRows())
         theData.tSTM.SetSize(stm.GetNumRows(), stm.GetNumRows(), true);
      theData.tSTM = stm;

      theData.tPrecTime += stepToTake / GmatTimeConstants::SECS_PER_DAY;
      theData.tOStateSSB = tcs->GetOrigin()->GetMJ2000PrecState(theData.tPrecTime) - 
                         ssb->GetMJ2000PrecState(theData.tPrecTime);
   }
   else
   {
      theData.rLoc = state.GetR();
      theData.rVel = state.GetV();
      // whenever updating participant's state, it needs to update its STM
      if (theData.rSTM.GetNumRows() != stm.GetNumRows())
         theData.rSTM.SetSize(stm.GetNumRows(), stm.GetNumRows(), true);
      theData.rSTM = stm;
      theData.rPrecTime += stepToTake / GmatTimeConstants::SECS_PER_DAY;
      theData.rOStateSSB = rcs->GetOrigin()->GetMJ2000PrecState(theData.rPrecTime) - 
                         ssb->GetMJ2000PrecState(theData.rPrecTime);
   }
   
   retval = true;
   return retval;
}