//$Id$
//------------------------------------------------------------------------------
//                           EventLocator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 6, 2011
//
/**
 * Definition of the the event locator base class
 */
//------------------------------------------------------------------------------


#ifndef EventLocator_hpp
#define EventLocator_hpp

#include <fstream>

#include "GmatBase.hpp"
#include "Spacecraft.hpp"
#include "CelestialBody.hpp"
#include "LocatedEvent.hpp"
#include "PlanetographicRegion.hpp"
//#include "EphemManager.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton

//#include "LocatedEventTable.hpp"  // may need this

class EphemManager;

///**
// * Base class for the event locators.
// *
// * EventLocators are container classes that collect together a set of event
// * functions defining a specific type of location process.  For example, the
// * EclipseLocator class collects together Penumbra, Umbra, and Antumbra event
// * functions.
// */
/**
 * Base class for the event locators.
 *
 * EventLocators are classes determine event times and durations of a
 * specified type (e.g. eclipses)
 */
class GMAT_API EventLocator: public GmatBase
{
public:
   EventLocator(const std::string &typeStr, const std::string &nomme = "");
   virtual ~EventLocator();
   EventLocator(const EventLocator& el);
   EventLocator& operator=(const EventLocator& el);

   // Inherited (GmatBase) methods for parameters
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual bool         IsParameterCommandModeSettable(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);

   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");
   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   virtual const ObjectTypeArray& GetTypesForList(const Integer id);
   virtual const ObjectTypeArray& GetTypesForList(const std::string &label);

   virtual void         SetEpoch(const std::string &ep, Integer id);
   virtual std::string  GetEpochString(const std::string &whichOne = "INITIAL",
                        const std::string &outFormat = "UTCGregorian") const;
   virtual void         SetAppend(bool appendIt);

   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                              const std::string &name);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

//   // Accessors for the owned subscribers
//   virtual Integer      GetOwnedObjectCount();
//   virtual GmatBase*    GetOwnedObject(Integer whichOne);

   virtual bool         Initialize();
   virtual bool         ReportEventData(const std::string &reportNotice = "");
   virtual void         LocateEvents(const std::string &reportNotice = "");
   virtual bool         FileWasWritten();
   virtual bool         IsInAutomaticMode();

protected:
   /// We need to store vector of Events
   typedef std::vector<LocatedEvent*>    EventList;

   /// Name of the event data file
   std::string                 filename;
   /// Flag set when a run writes data to the event report
   bool                        fileWasWritten;

   /// Use the light time delay (true = use it; false = instantaneous geometry)
   bool                        useLightTimeDelay;
   /// Use the stellar aberration?
   bool                        useStellarAberration;
   /// Write the report or not?
   bool                        writeReport;
   /// Should we do location at the end, when commanded to do so, or not at all?
   std::string                 runMode;
   /// String to write when the locator is running
   std::string                 locatingString;
   /// Use the entire time interval (true  - use the entire interval; false,
   /// use the input start and stop epochs)
   bool                        useEntireInterval;
   /// Append to the report or not (appends if true; creates new report,
   /// renaming existing report, if false)
   bool                        appendReport;
   /// The format of the input epoch
   std::string                 epochFormat;
   /// First epoch to search for events (as input)
   std::string                 initialEpoch;
   /// Last epoch to search for events (as input)
   std::string                 finalEpoch;
   /// Step size - roughly equal to minimum-duration event found
   Real                        stepSize;
   /// First epoch to search for events (as Real)
   Real                        initialEp;
   /// Last epoch to search for events (as Real)
   Real                        finalEp;
   /// The start epoch of the interval (depends on useEntireInterval flag)
   Real                        fromEpoch;
   /// The end epoch of the interval (depends on useEntireInterval flag)
   Real                        toEpoch;
   /// the start time of the current FindEvents
   Real        findStart;
   /// the stop time of the current FindEvents
   Real        findStop;
   /// The start (spacecraft) time
   Real        scStart;
   /// The current (spacecraft) time
   Real        scNow;

//   /// The number of events found in the current specified time range
//   Integer                     numEventsFound;

//   /// Names of the "target" spacecraft in the location  <future>
//   StringArray satNames;
   /// Name of the target spacecraft for the locator
   std::string                 targetName;
//   /// Pointers to the sats -- using SpaceObject so Formations can be supported  <future>
//   std::vector<SpaceObject*> targets;
   /// Pointer to the target spacecraft
   Spacecraft                  *sat;
   /// Pointer to the target region
   PlanetographicRegion        *region;
   /// If the target is a region not a sat
   bool                       targetIsRegion;
   /// The space environment
   SolarSystem                 *solarSys;
   /// The occulting body names
   StringArray                 occultingBodyNames;
   /// The occulting bodies
   std::vector<CelestialBody*> occultingBodies;
   /// The file stream
   std::fstream                theReport;
   /// the default occulting bodies (if none are set)
   // names of the default bodies to use
   StringArray                 defaultOccultingBodies;
   /// pointer ot the EphemManager for the spacecraft/target
   EphemManager                *em;
   /// Has the initial epoch been set?
   bool                        initialEpochSet;
   /// Has the final epoch been set?
   bool                        finalEpochSet;

   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;

   /// Published parameters for event locators
    enum
    {
//       SATNAMES = GmatBaseParamCount,  // future?
       SATNAME = GmatBaseParamCount,
       EVENT_FILENAME,
       OCCULTING_BODIES,
       INPUT_EPOCH_FORMAT,
       INITIAL_EPOCH,
       STEP_SIZE,
       FINAL_EPOCH,
       USE_LIGHT_TIME_DELAY,
       USE_STELLAR_ABERRATION,
       WRITE_REPORT,
       RUN_MODE,
       USE_ENTIRE_INTERVAL,
       EventLocatorParamCount
    };

    /// burn parameter labels
    static const std::string
       PARAMETER_TEXT[EventLocatorParamCount - GmatBaseParamCount];
    /// burn parameter types
    static const Gmat::ParameterType
       PARAMETER_TYPE[EventLocatorParamCount - GmatBaseParamCount];
    static const std::string RUN_MODES[3];
    static const Integer numModes;
    static const std::string defaultFormat;
    static const Real        defaultInitialEpoch;
    static const Real        defaultFinalEpoch;

    static const Real STEP_MULTIPLE;

    Real                   EpochToReal(const std::string &ep);
    bool                   OpenReportFile(bool renameOld = true);
    virtual std::string    GetAbcorrString();
    virtual CelestialBody* GetCelestialBody(const std::string &withName);
    virtual std::string    GetNoEventsString(const std::string &forType);
    virtual void           SetLocatingString(const std::string &forType);
    virtual void           FindEvents() = 0;
};

#endif /* EventLocator_hpp */
