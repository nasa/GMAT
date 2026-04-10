//------------------------------------------------------------------------------
//                             Code500Propagator
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
// Created: Apr 08, 2016 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task 26
//
/**
 * Implementation for the Code500Propagator class
 */
//------------------------------------------------------------------------------


#include "Code500Propagator.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include "NotAKnotInterpolator.hpp"    // Only one supported for now
#include "PropagatorException.hpp"
#include "CoordinateConverter.hpp"

#include <sstream>                     // for stringstream

//#define DEBUG_INITIALIZATION
//#define DEBUG_PROPAGATION
//#define DEBUG_INTERPOLATION
//#define DEBUG_EXECUTION


//---------------------------------
// static data
//---------------------------------

/// Code500Propagator parameter labels
const std::string Code500Propagator::PARAMETER_TEXT[
                 Code500PropagatorParamCount - EphemerisPropagatorParamCount] =
{
      "EphemFile"                  //EPHEMERISFILENAME
};

/// Code500Propagator parameter types
const Gmat::ParameterType Code500Propagator::PARAMETER_TYPE[
                 Code500PropagatorParamCount - EphemerisPropagatorParamCount] =
{
      Gmat::FILENAME_TYPE          //EPHEMERISFILENAME
};


const Real Code500Propagator::DUL_TO_KM         = 10000.0;
const Real Code500Propagator::DUL_DUT_TO_KM_SEC = 10000.0 / 864.0;
const Real Code500Propagator::DUT_TO_DAY        = 864.0 / 86400.0;
const Real Code500Propagator::DUT_TO_SEC        = 864.0;



//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Code500Propagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the object that gets constructed
 */
//------------------------------------------------------------------------------
Code500Propagator::Code500Propagator(const std::string &name) :
   EphemerisPropagator        ("Code500", name),
   ephemName                  (""),
   interp                     (NULL),
   satID                      (101.0),
   fileDataLoaded             (false),
   ephemRecords               (NULL),
   record                     (-1),
   stateIndex                 (-1),
   timeFromEphemStart         (-1.0),
   lastEpoch                  (-1.0),
   lastEpochGT                (-1.0),
   ephemCoord                 (NULL),
   j2k                        (NULL)
{
   // GmatBase data
  objectTypeNames.push_back("Code500Propagator");
  parameterCount       = Code500PropagatorParamCount;
}


//------------------------------------------------------------------------------
// ~Code500Propagator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Code500Propagator::~Code500Propagator()
{
   if (interp != NULL)
      delete interp;
   if (ephemCoord != NULL)
      delete ephemCoord;
   if (j2k != NULL)
      delete j2k;
}


//------------------------------------------------------------------------------
// Code500Propagator(const Code500Propagator & prop)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param prop The object that is copied into this new one
 */
//------------------------------------------------------------------------------
Code500Propagator::Code500Propagator(const Code500Propagator & prop) :
   EphemerisPropagator        (prop),
   ephemName                  (prop.ephemName),
   interp                     (NULL),
   satID                      (prop.satID),
   fileDataLoaded             (false),
   ephemRecords               (NULL),
   record                     (-1),
   stateIndex                 (-1),
   timeFromEphemStart         (-1.0),
   lastEpoch                  (-1.0),
   lastEpochGT                (-1.0),
   ephemCoord                 (NULL),
   j2k                        (NULL)
{
}


//------------------------------------------------------------------------------
// Code500Propagator & Code500Propagator::operator =(const Code500Propagator & prop)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param prop The object that is provides data for into this one
 *
 * @return This propagator, configured to match prop.
 */
//------------------------------------------------------------------------------
Code500Propagator & Code500Propagator::operator=(const Code500Propagator & prop)
{
   if (this != &prop)
   {
      EphemerisPropagator::operator=(prop);

      ephemName = prop.ephemName;
      if (interp != NULL)
      {
         delete interp;
         interp = NULL;
      }
      satID = prop.satID;
      fileDataLoaded = false;
      ephemRecords = NULL;
      record = -1;
      stateIndex = -1;
      lastEpoch = currentEpoch = prop.currentEpoch;
      lastEpochGT = currentEpochGT = prop.currentEpochGT;

      if (lastEpoch != -1.0)
      {
         if (hasPrecisionTime)
            timeFromEphemStart = (lastEpochGT - GmatTime(ephemStart)).GetTimeInSec();
         else
            timeFromEphemStart = (lastEpoch - ephemStart) *
               GmatTimeConstants::SECS_PER_DAY;
      }
      else
         timeFromEphemStart = -1.0;

      //ephemCoord = prop.ephemCoord;
      if (ephemCoord)
      {
         delete ephemCoord;
         ephemCoord = NULL;
      }
      if (prop.ephemCoord)
         ephemCoord = (CoordinateSystem*) (prop.ephemCoord->Clone());

      //j2k = prop.j2k;
      if (j2k)
      {
         delete j2k;
         j2k = NULL;
      }
      if (prop.j2k)
         j2k = (CoordinateSystem*)(prop.j2k->Clone());
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Generates a new object that matches this one
 *
 * @return The new object
 */
//------------------------------------------------------------------------------
GmatBase* Code500Propagator::Clone() const
{
   return new Code500Propagator(*this);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the script string for a parameter
 *
 * @param id The index of the parameter in the parameter tables
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetParameterText(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < Code500PropagatorParamCount)
      return PARAMETER_TEXT[id - EphemerisPropagatorParamCount];
   return EphemerisPropagator::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID of a parameter
 *
 * @param The script string for the parameter
 *
 * @return The parameter's ID
 */
//------------------------------------------------------------------------------
Integer Code500Propagator::GetParameterID(const std::string &str) const
{
   for (Integer i = EphemerisPropagatorParamCount;
         i < Code500PropagatorParamCount; ++i)
   {
       if (str == PARAMETER_TEXT[i - EphemerisPropagatorParamCount])
           return i;
   }

   return EphemerisPropagator::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Code500Propagator::GetParameterType(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < Code500PropagatorParamCount)
      return PARAMETER_TYPE[id - EphemerisPropagatorParamCount];
   return EphemerisPropagator::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string description of a parameter's type
 *
 * @param id The ID of the parameter
 *
 * @return The type of the parameter
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetParameterTypeString(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < Code500PropagatorParamCount)
      return EphemerisPropagator::PARAM_TYPE_STRING[GetParameterType(id)];
   return EphemerisPropagator::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * retrieves the dimensional units for a parameter
 *
 * @param id The ID of the parameter
 *
 * @return The unit label
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetParameterUnit(const Integer id) const
{
   return EphemerisPropagator::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Reports if a parameter should be hidden from the users
 *
 * @param id The ID of the parameter
 *
 * @return true if the parameter should be hidden, false if not
 */
//------------------------------------------------------------------------------
bool Code500Propagator::IsParameterReadOnly(const Integer id) const
{
   if (id == EPHEMERISFILENAME)
      return true;
   return EphemerisPropagator::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Reports if a parameter should be hidden from the users
 *
 * @param label The scripted string of the parameter
 *
 * @return true if the paameter should be hidden, false if not
 */
//------------------------------------------------------------------------------
bool Code500Propagator::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetStringParameter(const Integer id) const
{
   return EphemerisPropagator::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter
 *
 * @param id The ID of the parameter
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Code500Propagator::SetStringParameter(const Integer id,
      const std::string &value)
{

   if (id == EPHEMERISFILENAME)
   {
      return true;         // Idempotent, so return true
   }

   if (id == EPHEM_CENTRAL_BODY)
   {
      MessageInterface::ShowMessage("Warning:  Central bodies set on Code500 "
            "propagators have no effect.  Propagation uses the body specified "
            "on the ephemeris file for the propagator %s.\n",
            instanceName.c_str());
      return false;
   }

   return EphemerisPropagator::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from an array
 *
 * @param id The ID of the parameter
 * @param index The array index
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == EPHEMERISFILENAME)
   {
      return "";
   }

   return EphemerisPropagator::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array
 *
 * @param id The ID of the parameter
 * @param value The new value
 * @param index The index of the parameter in the array
 *
 * @return True on success, false on failure
 */
//------------------------------------------------------------------------------
bool Code500Propagator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   if (id == EPHEMERISFILENAME)
   {
      return false;
   }

   return EphemerisPropagator::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param id The ID of the parameter
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& Code500Propagator::GetStringArrayParameter(
      const Integer id) const
{
   return EphemerisPropagator::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter from an array of StringArrays
 *
 * @param id The ID of the parameter
 * @param index The index of the StringArray
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& Code500Propagator::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   return EphemerisPropagator::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param label The script label of the parameter
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter
 *
 * @param label The script label of the parameter
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Code500Propagator::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from an array
 *
 * @param label The script label of the parameter
 * @param index The array index
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
std::string Code500Propagator::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array
 *
 * @param label The script label of the parameter
 * @param value The new value
 * @param index The index of the parameter in the array
 *
 * @return True on success, false on failure
 */
//------------------------------------------------------------------------------
bool Code500Propagator::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param label The script label of the parameter
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& Code500Propagator::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter from an array of StringArrays
 *
 * @param label The script label of the parameter
 * @param index The index of the StringArray
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& Code500Propagator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the Code500Propagator for use in a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
//@todo: need GmatTime modification
bool Code500Propagator::Initialize()
{
   Integer logOption = 0;
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Code500Propagator::Initialize() entered\n");
      logOption = 1;
   #endif
   bool retval = false;

   // EphemerisPropagator::Initialize() function using value of ephemStart
   // but at this point ephemStart is not defined yet. The value of ephemStart
   // is only defined after reading Code500 ephemeris file. Therefore, the part
   // of reading Code500 file has to go before calling EphemerisPropagator::Initialize().
   if (EphemerisPropagator::Initialize())
   {
      stepTaken = 0.0;

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("   After base class initialization, "
               "initialEpoch = %.12lf\n", initialEpoch);
      #endif

      FileManager *fm = FileManager::Instance();
      std::string fullPath;

      if (propObjects.size() != 1)
         throw PropagatorException("Code 500 propagators require exactly one "
               "SpaceObject.");

      // The PSM isn't set until PrepareToPropagate fires.  The following is
      // also last minute setup, so only do it if the PSM has been set
      if ((psm != NULL) && !fileDataLoaded)
      {
         for (UnsignedInt i = 0; i < propObjects.size(); ++i)
         {
//            Real id = propObjects[i]->GetRealParameter("SatelliteID");
//            satID = id;

            if (propObjects[i]->IsOfType(Gmat::SPACECRAFT))
            {
               ephemName = propObjects[i]->GetStringParameter(
                     "EphemerisName");
               //currentEpoch = ((Spacecraft*)propObjects[i])->GetEpoch();
               //currentEpochGT = ((Spacecraft*)propObjects[i])->GetEpochGT();

               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("Spacecraft epoch is %.12lf\n",
                        currentEpoch);
                  MessageInterface::ShowMessage("Spacecraft epochGT is %s\n",
                     currentEpochGT.ToString().c_str());
               #endif
            }
            else
               throw PropagatorException("Code 500 ephemeris propagators only "
                     "work for Spacecraft.");

            if (ephemName == "")
               throw PropagatorException("The Code-500 propagator requires a "
                     "valid ephemeris file name");

            fullPath = fm->FindPath(ephemName, "VEHICLE_EPHEM_PATH", true, false, true);
            if (fullPath == "")
               throw PropagatorException("The Code 500 ephemeris file " +
                     ephemName + " does not exist");

            if (!ephem.OpenForRead(fullPath))
               throw PropagatorException("The Code 500 ephemeris file " +
                     ephemName + " failed to open");

            ephem.ReadHeader1(logOption);
            ephem.ReadHeader2(logOption);
            ephem.ReadDataRecords(-999, 0);

            Real timeSystem = ephem.GetTimeSystem();

			   SetEphemSpan();
            
            // At this point ephemStart is defined, EphemerisPropagator::Initialize()
            // should be called here.
            //EphemerisPropagator::Initialize();

            // Recalculate initialEpoch for case FROM_EPHEM
            if (startEpochSource == FROM_EPHEM)
            {
               if (((SpaceObject*)(propObjects[0]))->HasEphemPropagated())
               {
                  initialEpoch = ((SpaceObject*)(propObjects[0]))->GetEpoch();
                  if (hasPrecisionTime)
                     initialEpochGT = ((SpaceObject*)(propObjects[0]))->GetEpochGT();
                  initialEphemEpochSet = false;
                  timeFromEpoch = 0.0;
               }
               else
               {
                  if (ephemStart > 0)
                  {
                     initialEpoch = ephemStart;
                     if (hasPrecisionTime)
                        initialEpochGT = GmatTime(ephemStart);
                     initialEphemEpochSet = true;
                     timeFromEpoch = 0.0;
                  }
                  else
                  {
                     initialEphemEpochSet = false;
                  }
               }
            }

            currentEpoch = initialEpoch;
            if (hasPrecisionTime)
            {
               currentEpochGT = initialEpochGT;
               timeFromEphemStart = (initialEpochGT - GmatTime(ephemStart)).GetTimeInSec();
            }
            else
            {
               currentEpochGT = initialEpoch;
               timeFromEphemStart = (initialEpoch - ephemStart) *
                  GmatTimeConstants::SECS_PER_DAY;
            }
            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("EphemStart: %.12lf, InitialEpoch: "
                     "%.12lf, Time from start: %lf\n", ephemStart, initialEpoch,
                     timeFromEphemStart);

               if (ephemRecords != NULL)
                  MessageInterface::ShowMessage("EphemRecords contains %d "
                        "data blocks\n", ephemRecords->size());
               MessageInterface::ShowMessage("Record contents:\n");
            #endif

            startEpochs.clear();
            timeSteps.clear();
            timeSpans.clear();
            for (UnsignedInt i = 0; i < ephemRecords->size(); ++i)
            {
               // Save the data used by the Code500 propagator in GMAT compatible formats
               Real startDate = ephem.ReadDoubleField(&ephemRecords->at(i).dateOfFirstEphemPoint_YYYMMDD);
               Real startSecs = ephem.ReadDoubleField(&ephemRecords->at(i).secsOfDayForFirstEphemPoint);
               Integer year   = (Integer)(startDate / 10000);
               Integer month  = (Integer)((startDate - year * 10000) / 100);
               Integer day    = (Integer)(startDate - year * 10000 - month * 100);
               year += 1900;
               Integer hour   = (Integer)(startSecs / 3600);
               Integer minute = (Integer)((startSecs - hour * 3600) / 60);
               Real second    = startSecs - hour * 3600 - minute * 60;

               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("Start:  %04d/%02d/%02d %02d:"
                        "%02d:%lf %s\n", year, month, day, hour, minute, second,
                        (timeSystem == 1.0 ? "A.1" : "UTC"));
               #endif

               GmatEpoch epoch = ModifiedJulianDate(year, month, day, hour, minute, second);
               GmatEpoch theEpoch = 0.0;

               // If not A.1 time, convert it!
               if (timeSystem == 1.0)
                  theEpoch = epoch;
               else //if (timeSystem == 2.0) // Should check to be sure it was set
                  theEpoch = theTimeConverter->Convert(epoch, TimeSystemConverter::UTCMJD,
                        TimeSystemConverter::A1MJD);

               startEpochs.push_back(theEpoch);
               timeSteps.push_back(ephem.ReadDoubleField(&ephemRecords->at(i).timeIntervalBetweenPoints_SEC));

               Real span = timeSteps.back() * Code500EphemerisFile::NUM_STATES_PER_RECORD;
               if (timeSystem == 2.0) // If using UTC, adjust for leap seconds if necessary
                  span += theTimeConverter->NumberOfLeapSecondsFrom(epoch + span/GmatTimeConstants::SECS_PER_DAY) -
                          theTimeConverter->NumberOfLeapSecondsFrom(epoch);
               timeSpans.push_back(span);

               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("   %3d: Date %.0lf : %lf secs "
                        "=> %.12lf, stepping %lf s\n", i,
                        ephem.ReadDoubleField(&ephemRecords->at(i).dateOfFirstEphemPoint_YYYMMDD),
                        ephem.ReadDoubleField(&ephemRecords->at(i).secsOfDayForFirstEphemPoint), theEpoch,
                        ephem.ReadDoubleField(&ephemRecords->at(i).timeIntervalBetweenPoints_SEC));

                  MessageInterface::ShowMessage("      State: [%lf   %lf   "
                        "%lf]   [%lf   %lf   %lf]\n",
                        ephem.ReadDoubleField(&ephemRecords->at(i).firstStateVector_DULT[0]) * DUL_TO_KM,
                        ephem.ReadDoubleField(&ephemRecords->at(i).firstStateVector_DULT[1]) * DUL_TO_KM,
                        ephem.ReadDoubleField(&ephemRecords->at(i).firstStateVector_DULT[2]) * DUL_TO_KM,
                        ephem.ReadDoubleField(&ephemRecords->at(i).firstStateVector_DULT[3]) * DUL_DUT_TO_KM_SEC,
                        ephem.ReadDoubleField(&ephemRecords->at(i).firstStateVector_DULT[4]) * DUL_DUT_TO_KM_SEC,
                        ephem.ReadDoubleField(&ephemRecords->at(i).firstStateVector_DULT[5]) * DUL_DUT_TO_KM_SEC);

                  for (UnsignedInt j = 0; j < 4; ++j)
                     MessageInterface::ShowMessage("          %d: [%lf   %lf   "
                           "%lf]   [%lf   %lf   %lf]\n", j,
                           ephem.ReadDoubleField(&ephemRecords->at(i).stateVector2Thru50_DULT[j][0]) * DUL_TO_KM,
                           ephem.ReadDoubleField(&ephemRecords->at(i).stateVector2Thru50_DULT[j][1]) * DUL_TO_KM,
                           ephem.ReadDoubleField(&ephemRecords->at(i).stateVector2Thru50_DULT[j][2]) * DUL_TO_KM,
                           ephem.ReadDoubleField(&ephemRecords->at(i).stateVector2Thru50_DULT[j][3]) * DUL_DUT_TO_KM_SEC,
                           ephem.ReadDoubleField(&ephemRecords->at(i).stateVector2Thru50_DULT[j][4]) * DUL_DUT_TO_KM_SEC,
                           ephem.ReadDoubleField(&ephemRecords->at(i).stateVector2Thru50_DULT[j][5]) * DUL_DUT_TO_KM_SEC);
               #endif
            }

            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("The ephem spans from %.12lf to "
                     "%.12lf\nRecords pointer = %p\n", ephemStart, ephemEnd,
                     ephemRecords);
            #endif
            fileDataLoaded = true;

            // Setup central body
            centralBody = ephem.GetCentralBody();
            if (centralBody == "Moon")
               centralBody = "Luna";

            propOrigin = solarSystem->GetBody(centralBody);

            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("Setting central body to %s <%p>\n",
                     centralBody.c_str(), propOrigin);
            #endif
            
            // Setup coordinate system
            CelestialBody* earth = solarSystem->GetBody("Earth");
            if (!earth) throw PropagatorException("Earth undefined.");

            j2k = CoordinateSystem::CreateLocalCoordinateSystem("cbMJ2000Eq", "MJ2000Eq",
               propOrigin, NULL, NULL, earth, solarSystem);

            std::string axisSystemOnFile;
            if (ephem.GetCoordSystemIndicator() == 3)
               axisSystemOnFile = "TODEq";
            else if (ephem.GetCoordSystemIndicator() == 4)
               axisSystemOnFile = "MJ2000Eq";
            else if (ephem.GetCoordSystemIndicator() == 5)
               axisSystemOnFile = "BodyFixed";
            else
               throw PropagatorException("Ephemeris file does not use a supported coordinate system, "
                                         "Supported types are J2000, True of Date, and Body-Fixed");
            
            ephemCoord = CoordinateSystem::CreateLocalCoordinateSystem("csOnCode500Ephem", axisSystemOnFile,
               propOrigin, NULL, NULL, earth, solarSystem);
            
            // Build the interpolator.  For now, use not-a-knot splines
            if (interp != NULL)
               delete interp;
            interp = new NotAKnotInterpolator("Code500NotAKnot", 6);
            ephem.CloseForRead();

            Rvector6 outState;
            GetState(initialEpoch, outState);
            lastEpoch = initialEpoch;

            std::memcpy(state, outState.GetDataVector(), 6*sizeof(Real));

            //MoveToOrigin(currentEpoch);

            UpdateSpaceObject(currentEpoch);
         }
      }
   }

   // @todo: This is likely the source of GMT-5959
   if (startEpochSource == FROM_SCRIPT)
      for (UnsignedInt i = 0; i < propObjects.size(); ++i)
      {
         propObjects[i]->SetRealParameter("A1Epoch", currentEpoch);
         if (hasPrecisionTime)
            propObjects[i]->SetGmatTimeParameter("A1Epoch", currentEpochGT);
         else
            propObjects[i]->SetGmatTimeParameter("A1Epoch", GmatTime(currentEpoch));
      }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Code500Propagator::Initialize(), on exit, "
            "initialEpoch = %.12lf, current = %.12lf\n", initialEpoch,
            currentEpoch);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool Step()
//------------------------------------------------------------------------------
/**
 * Advances the state vector by the ephem step
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
//@todo: need GmatTime modification. Modification is completed
bool Code500Propagator::Step()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("Code500Propagator::Step() entered for %p: "
            "initialEpoch = %.12lf; stepsize = %.12lf; "
            "timeFromEpoch = %.12lf, Time from ephem start = %lf\n", this,
            initialEpoch, ephemStep, timeFromEpoch, timeFromEphemStart);
   #endif

   bool retval = false;

   for (UnsignedInt i = 0; i < propObjects.size(); ++i)
   {
      if (propObjects[i]->IsOfType(Gmat::SPACECRAFT))
      {
         std::string ephemNameSC = propObjects[i]->GetStringParameter(
            "EphemerisName");

         if (ephemName != ephemNameSC)
         {
            fileDataLoaded = false;
            Initialize();
         }
      }
   }

   if (hasPrecisionTime)
   {
      // Might need to do this:
      // currentEpoch = ((Spacecraft*)propObjects[i])->GetEpoch();
      if (record == -1)
      {
         // Initialize the pointers into the ephem data
         FindRecord(currentEpochGT);
      }

      if (record < 0)
         throw PropagatorException("Unable to propagate " + instanceName +
         ": is the epoch outside of the span of the ephemeris file?");

      Rvector6  outState;

      if (lastEpochGT != currentEpochGT)
      {
         lastEpochGT = currentEpochGT;
         timeFromEphemStart = (lastEpochGT - GmatTime(ephemStart)).GetTimeInSec();;
      }

      timeFromEphemStart += ephemStep;
      timeFromEpoch += ephemStep;
      stepTaken = ephemStep;

      currentEpochGT = ephemStart; currentEpochGT.AddSeconds(timeFromEphemStart);

      #ifdef DEBUG_PROPAGATION
         MessageInterface::ShowMessage("   ephemStart = %.12lf, timeFromStart = "
            "%lf sec => currentEpochGT after step = %s; lastEpochGT = %s\n",
            ephemStart, timeFromEphemStart, currentEpochGT.ToString().c_str(), lastEpochGT.ToString().c_str());
      #endif

      // Allow for slop in the last few bits
      bool flagOutOfDomain = false;
      if (currentEpochGT < ephemStart)
      {
         if ((GmatTime(ephemStart) - currentEpochGT).GetMjd() < 1.0e-10)
            currentEpochGT = ephemStart;
         else
            flagOutOfDomain = true;
      }
      else if (currentEpochGT > ephemEnd)
      {
         if ((currentEpochGT - GmatTime(ephemEnd)).GetMjd() < 1.0e-10)
            currentEpochGT = ephemEnd;
         else
            flagOutOfDomain = true;
      }
      if (flagOutOfDomain)
      {
         std::stringstream errmsg;
         errmsg.precision(16);
         errmsg << "The Code 500 Propagator "
            << instanceName
            << " is attempting to step outside of the span of the "
            "ephemeris data; halting.  ";
         errmsg << "The current Code 500 ephemeris covers the A.1 modified "
            "Julian span ";
         errmsg << ephemStart << " to " << ephemEnd << " and the "
            "requested epoch is " << currentEpochGT.ToString() << ".";
         throw PropagatorException(errmsg.str());
      }

      GetState(currentEpochGT, outState);
      lastEpochGT = currentEpochGT;

      std::memcpy(state, outState.GetDataVector(), 6*sizeof(Real));

      //MoveToOrigin(currentEpoch);

      UpdateSpaceObjectGT(currentEpochGT);

      #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("(Step for %p) State at epoch "
         "%s is [", this, currentEpochGT.ToString().c_str());
      for (Integer i = 0; i < dimension; ++i)
      {
         MessageInterface::ShowMessage("%.12lf", state[i]);
         if (i < 5)
            MessageInterface::ShowMessage("   ");
         else
            MessageInterface::ShowMessage("]\n");
      }
      #endif
   }
   else
   {
      // Might need to do this:
      // currentEpoch = ((Spacecraft*)propObjects[i])->GetEpoch();
      if (record == -1)
      {
         // Initialize the pointers into the ephem data
         FindRecord(currentEpoch);
      }

      if (record < 0)
         throw PropagatorException("Unable to propagate " + instanceName +
         ": is the epoch outside of the span of the ephemeris file?");

      Rvector6  outState;

      if (lastEpoch != currentEpoch)
      {
         lastEpoch = currentEpoch;
         timeFromEphemStart = (lastEpoch - ephemStart) *
            GmatTimeConstants::SECS_PER_DAY;
      }

      timeFromEphemStart += ephemStep;
      timeFromEpoch += ephemStep;
      stepTaken = ephemStep;

      currentEpoch = ephemStart + timeFromEphemStart /
         GmatTimeConstants::SECS_PER_DAY;

      #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("   ephemStart = %.12lf, timeFromStart = "
         "%lf sec => currentEpoch after step = %.12lf; lastEpoch = %.12lf\n",
         ephemStart, timeFromEphemStart, currentEpoch, lastEpoch);
      #endif

      // Allow for slop in the last few bits
      bool flagOutOfDomain = false;
      if (currentEpoch < ephemStart)
      {
         if (ephemStart - currentEpoch < 1.0e-10)
            currentEpoch = ephemStart;
         else
            flagOutOfDomain = true;
      }
      else if (currentEpoch > ephemEnd)
      {
         if (currentEpoch - ephemEnd < 1.0e-10)
            currentEpoch = ephemEnd;
         else
            flagOutOfDomain = true;
      }
      if (flagOutOfDomain)
      {
         std::stringstream errmsg;
         errmsg.precision(16);
         errmsg << "The Code 500 Propagator "
            << instanceName
            << " is attempting to step outside of the span of the "
            "ephemeris data; halting.  ";
         errmsg << "The current Code 500 ephemeris covers the A.1 modified "
            "Julian span ";
         errmsg << ephemStart << " to " << ephemEnd << " and the "
            "requested epoch is " << currentEpoch << ".";
         throw PropagatorException(errmsg.str());
      }

      GetState(currentEpoch, outState);
      lastEpoch = currentEpoch;

      std::memcpy(state, outState.GetDataVector(), 6*sizeof(Real));

      //MoveToOrigin(currentEpoch);

      UpdateSpaceObject(currentEpoch);

      #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("(Step for %p) State at epoch "
         "%.12lf is [", this, currentEpoch);
      for (Integer i = 0; i < dimension; ++i)
      {
         MessageInterface::ShowMessage("%.12lf", state[i]);
         if (i < 5)
            MessageInterface::ShowMessage("   ");
         else
            MessageInterface::ShowMessage("]\n");
      }
      #endif   
   }
   retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// bool RawStep()
//------------------------------------------------------------------------------
/**
 * Performs a propagation step without error control
 *
 * @note: RawStep is not used with the Code500Propagator
 *
 * @return false always
 */
//------------------------------------------------------------------------------
bool Code500Propagator::RawStep()
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// Real GetStepTaken()
//------------------------------------------------------------------------------
/**
 * Retrieves the size of the most recent Code500Propagator step
 *
 * @return The most recent step (0.0 if no step was taken with this instance).
 */
//------------------------------------------------------------------------------
Real Code500Propagator::GetStepTaken()
{
   return stepTaken;
}


//------------------------------------------------------------------------------
// void UpdateState()
//------------------------------------------------------------------------------
/**
 * Updates the propagation state vector with data from the
 * PropagationStateManager
 */
//------------------------------------------------------------------------------
void Code500Propagator::UpdateState()
{
   #ifdef DEBUG_EXECUTION
   if (hasPrecisionTime)
      MessageInterface::ShowMessage("Code500Propagator::UpdateState(): Updating state to epochGT %s\n",
         currentEpochGT.ToString().c_str());
   else
      MessageInterface::ShowMessage("Code500Propagator::UpdateState(): Updating state to epoch %.12lf\n",
            currentEpoch);
   #endif

   //Rvector6 theState;
   //if (hasPrecisionTime)
   //   GetState(currentEpochGT, theState);
   //else
   //   GetState(currentEpoch, theState);

   Real theState[6];
   if (hasPrecisionTime)
      interp->Interpolate(currentEpochGT.GetMjd(), theState);
   else
      interp->Interpolate(currentEpoch, theState);


   // Update state
   Real* ephemState = new Real[6];
   //std::memcpy(state, theState, 6 * sizeof(Real));
   std::memcpy(ephemState, theState, 6 * sizeof(Real));

   if (ephemCoord)
   {
      
      CoordinateConverter cc;
      if (hasPrecisionTime)
      {
         //cc.Convert(currentEpochGT, state, ephemCoord, j2kState, j2k);
         cc.Convert(currentEpochGT, ephemState, ephemCoord, state, j2k);
      }
      else
         //cc.Convert(currentEpoch, state, ephemCoord, j2kState, j2k);
         cc.Convert(currentEpoch, ephemState, ephemCoord, state, j2k);
   }
   else
   {
      //std::memcpy(j2kState, state, 6 * sizeof(Real));
      std::memcpy(state, ephemState, 6 * sizeof(Real));
   }

   delete[] ephemState;

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Code500Propagator::UpdateState(): exit\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetEphemSpan(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Determines the start and end epoch for the SPICE ephemerides associated with
 * the propagated spacecraft
 *
 * @param whichOne Not currrently used.
 */
//------------------------------------------------------------------------------
void Code500Propagator::SetEphemSpan(Integer whichOne)
{
   ephem.GetStartAndEndEpochs(ephemStart, ephemEnd, &ephemRecords);

   if (!initialEphemEpochSet)
   {
	   EphemerisPropagator::SetEphemSpan(whichOne);
   }
}

//------------------------------------------------------------------------------
// void Code500Propagator::FindRecord(GmatEpoch forEpoch)
//------------------------------------------------------------------------------
/**
 * Sets up the indices into the ephem data for a input epoch
 *
 * @param forEpoch The epoch that is being set up
 */
//------------------------------------------------------------------------------
void Code500Propagator::FindRecord(GmatEpoch forEpoch)
{
   record = -1;
   stateIndex = -1;

   if ((forEpoch >= ephemStart) && (forEpoch <= ephemEnd))
   {
      record = -2;
      for (UnsignedInt i = 0; i < ephemRecords->size(); ++i)
         if (forEpoch < startEpochs[i])
         {
            record = i - 1;
            break;
         }

      // Handle epoch in the final block
      if (record == -2)
      {
         record = ephemRecords->size() - 1;
      }

      // Now figure out the record number in the block
      Real secsPastStart = (forEpoch - startEpochs[record]) *
            GmatTimeConstants::SECS_PER_DAY;
      stateIndex = (Integer)(secsPastStart / timeSteps[record]);
   }

   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("Epoch %.12lf has index %d in block %d\n",
            forEpoch, stateIndex, record);
   #endif
}


void Code500Propagator::FindRecord(GmatTime forEpoch)
{
   record = -1;
   stateIndex = -1;

   if ((forEpoch >= ephemStart) && (forEpoch <= ephemEnd))
   {
      record = -2;
      for (UnsignedInt i = 0; i < ephemRecords->size(); ++i)
         if (forEpoch < startEpochs[i])
         {
            record = i - 1;
            break;
         }

      // Handle epoch in the final block
      if (record == -2)
      {
         record = ephemRecords->size() - 1;
      }

      // Now figure out the record number in the block
      Real secsPastStart = (forEpoch - GmatTime(startEpochs[record])).GetTimeInSec();
      stateIndex = (Integer)(secsPastStart / timeSteps[record]);
   }

#ifdef DEBUG_PROPAGATION
   MessageInterface::ShowMessage("Epoch %s has index %d in block %d\n",
      forEpoch.ToString().c_str(), stateIndex, record);
#endif
}

//------------------------------------------------------------------------------
// void GetState(GmatEpoch forEpoch, Rvector6 &outstate)
//------------------------------------------------------------------------------
/**
 * Returns the state data at the specified epoch
 *
 * @param forEpoch The epoch for the data
 * @param outstate The state vector that receives the data
 */
//------------------------------------------------------------------------------
void Code500Propagator::GetState(GmatEpoch forEpoch, Rvector6 &outstate)
{
   UpdateInterpolator(forEpoch);

   Real theState[6];
   //Real theStateMJ2000[6];
   if (interp->Interpolate(forEpoch, theState))
   {
      //cc.Convert(forEpoch, theState, ephemCoord, theStateMJ2000, j2k);
      //outstate.Set(theStateMJ2000);
      outstate.Set(theState);
   }
   else
   {
      throw PropagatorException("The propagator " + instanceName +
               " failed to interpolate a valid state for " +
               propObjects[0]->GetName());
   }

   #ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage("Interpolated state: %.12lf  [%.15lf  "
            "%.15lf  %.15lf  %.15lf  %.15lf  %.15lf]\n", forEpoch, theState[0],
            theState[1], theState[2], theState[3], theState[4], theState[5]);
   #endif
}


//------------------------------------------------------------------------------
// void GetState(GmatEpoch forEpoch, Rvector6 &outstate)
//------------------------------------------------------------------------------
/**
 * Returns the state data at the specified epoch
 *
 * @param forEpoch The epoch for the data
 * @param outstate The state vector that receives the data
 */
 //------------------------------------------------------------------------------
void Code500Propagator::GetState(GmatTime forEpoch, Rvector6 &outstate)
{
   UpdateInterpolator(forEpoch);

   Real theState[6];
   if (interp->Interpolate(forEpoch.GetMjd(), theState))
      outstate.Set(theState);
   else
   {
      throw PropagatorException("The propagator " + instanceName +
         " failed to interpolate a valid state for " +
         propObjects[0]->GetName());
   }

#ifdef DEBUG_INTERPOLATION
   MessageInterface::ShowMessage("Interpolated state: %s  [%.15lf  "
      "%.15lf  %.15lf  %.15lf  %.15lf  %.15lf]\n", forEpoch.ToString().c_str(), theState[0],
      theState[1], theState[2], theState[3], theState[4], theState[5]);
#endif
}

//------------------------------------------------------------------------------
// void UpdateInterpolator(forEpoch)
//------------------------------------------------------------------------------
/**
 * Method that updates buffer data in the interpolator as propagation proceeds
 *
 * This method passes ephem data to the interpolator and resets the time span
 * data to track the data loaded.  The "sweet" region used for interpolation
 * moves as propagation progresses, and is generally chosen, when possible, to
 * keep the points used for interpolation centered on the interpolation epoch.
 * Thus for the cubic spline interpolator used in this implementation, the
 * interpolator data is set so that the interpolation epoch (forEpoch) falls
 * between the second and third ephemeris points when possible, minimizing the
 * likelihood that the interpolation will ring.
 *
 * @param forEpoch The epoch that needs to be covered in the preferred region of
 *                 the interpolator data
 */
//------------------------------------------------------------------------------
void Code500Propagator::UpdateInterpolator(const GmatEpoch &forEpoch)
{
   GmatEpoch useEpoch = forEpoch;

   if ((forEpoch < ephemStart) || (forEpoch > ephemEnd))
   {
      if (ephemStart - forEpoch < 1.0e-10)
         useEpoch = ephemStart;
      if (forEpoch - ephemEnd < 1.0e-10)
         useEpoch = ephemEnd;
   }

   FindRecord(useEpoch);

   if (stateIndex == -1)
      throw PropagatorException("Requested epoch is outside of the span "
            "covered by the ephemeris file " + ephemName);

   Integer usedRecords[5][2];
   Integer block = record, line = stateIndex - 1;

   if (line < 0)
   {
      if (block == 0)
         // Handle the first data point
         line = 0;
      else
      {
         // Handle spanning the records
         --block;
         line = 49;
      }
   }

   // Handle the end of the ephem
   if (block + 2 >= ephemRecords->size())
   {
      Integer lastBlock = ephemRecords->size()-1;

      // Last block has indeterminate number of records, so figure it out
      GmatEpoch startEpochLastBlock = startEpochs[lastBlock];
      Integer lastBlockLastData = (ephemEnd - startEpochLastBlock + 1.0e-10) *
            GmatTimeConstants::SECS_PER_DAY / timeSteps[lastBlock];
      Integer lastBlockLastDataExtended = lastBlockLastData + 50;

      #ifdef DEBUG_INTERPOLATION
         MessageInterface::ShowMessage("End of data is at [%d, %d]\n",
               lastBlock, lastBlockLastData);
      #endif

      // Fix up the indexing!
      if (((block == lastBlock) && (line > lastBlockLastData - 4))
            || ((block + 1 == lastBlock) && (line > lastBlockLastDataExtended - 4))) // Needed if lastBlockLastData < 4
      {
         line = lastBlockLastData - 4;
         if (line < 0)
         {
            block = lastBlock - 1;
            line += 50;
         }
      }
   }

   for (UnsignedInt i = 0; i < 5; ++i)
   {
      usedRecords[i][0] = block;
      usedRecords[i][1] = line;
      ++line;
      if (line == 50)
      {
         // transition to the next record
         ++block;
         line = 0;
      }
   }

   Integer startIndex = 1;
   Integer endIndex   = 2;

   // Start of the ephem
   if (stateIndex == 0)
      startIndex = 0;
   if (stateIndex > 45)
      startIndex = stateIndex - 45;

   Real epoch;
   Real state[6];

   // Brute force for now: refill the interpolator
   interp->Clear();

   #ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage("Pairs used for epoch %.12lf:\n", forEpoch);
   #endif

   for (UnsignedInt i = 0; i < 5; ++i)
   {
      Real epochOffset = 0;
      for (UnsignedInt j = 0; j < usedRecords[i][0]; ++j)
         epochOffset += timeSpans.at(j);
      epochOffset += timeSteps[usedRecords[i][0]] * (usedRecords[i][1]);

      if (ephem.GetTimeSystem() == 2.0)  // Check Leap seconds for UTC
      {
         GmatEpoch startEpoch, currentEpoch;
         GmatEpoch startEpochUTC, currentEpochUTC;

         startEpoch = startEpochs[usedRecords[i][0]];
         currentEpoch = startEpoch + timeSteps[usedRecords[i][0]] * (usedRecords[i][1]) / GmatTimeConstants::SECS_PER_DAY;

         startEpochUTC = theTimeConverter->Convert(startEpoch, TimeSystemConverter::A1MJD,
                        TimeSystemConverter::UTCMJD);
         currentEpochUTC = theTimeConverter->Convert(currentEpoch, TimeSystemConverter::A1MJD,
                        TimeSystemConverter::UTCMJD);

         epochOffset+= theTimeConverter->NumberOfLeapSecondsFrom(currentEpochUTC) - theTimeConverter->NumberOfLeapSecondsFrom(startEpochUTC);
      }

      epochOffset /= GmatTimeConstants::SECS_PER_DAY;

      epoch = ephemStart + epochOffset;

      #ifdef DEBUG_INTERPOLATION
         MessageInterface::ShowMessage("  [%d, %d]", usedRecords[i][0],
               usedRecords[i][1]);
      #endif

      for (UnsignedInt j = 0; j < 3; ++j)
      {
         if (usedRecords[i][1] == 0)
         {
            state[j] = ephem.ReadDoubleField(&ephemRecords->at(usedRecords[i][0]).firstStateVector_DULT[j]) * DUL_TO_KM;
            state[j+3] = ephem.ReadDoubleField(&ephemRecords->at(usedRecords[i][0]).firstStateVector_DULT[j+3]) * DUL_DUT_TO_KM_SEC;
         }
         else
         {
            state[j] = ephem.ReadDoubleField(&ephemRecords->at(usedRecords[i][0]).stateVector2Thru50_DULT[usedRecords[i][1]-1][j]) * DUL_TO_KM;
            state[j+3] = ephem.ReadDoubleField(&ephemRecords->at(usedRecords[i][0]).stateVector2Thru50_DULT[usedRecords[i][1]-1][j+3]) * DUL_DUT_TO_KM_SEC;
         }
      }

      interp->AddPoint(epoch, state);
      #ifdef DEBUG_INTERPOLATION
         MessageInterface::ShowMessage(" --> [%.12lf %lf %lf...\n", epoch,
               state[0], state[1]);
      #endif
   }
   #ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage("\n");
   #endif
}


void Code500Propagator::UpdateInterpolator(const GmatTime &forEpoch)
{
   GmatTime useEpoch = forEpoch;

   if ((forEpoch < ephemStart) || (forEpoch > ephemEnd))
   {
      if ((GmatTime(ephemStart) - forEpoch).GetMjd() < 1.0e-10)
         useEpoch = ephemStart;
      if ((forEpoch - GmatTime(ephemEnd)).GetMjd() < 1.0e-10)
         useEpoch = ephemEnd;
   }

   FindRecord(useEpoch);

   if (stateIndex == -1)
      throw PropagatorException("Requested epoch is outside of the span "
      "covered by the ephemeris file " + ephemName);

   Integer usedRecords[5][2];
   Integer block = record, line = stateIndex - 1;

   if (line < 0)
   {
      if (block == 0)
         // Handle the first data point
         line = 0;
      else
      {
         // Handle spanning the records
         --block;
         line = 49;
      }
   }

   // Handle the end of the ephem
   if (block >= ephemRecords->size() - 2)
   {
      Integer lastBlock = ephemRecords->size() - 1;

      // Last block has indeterminate number of records, so figure it out
      GmatTime startEpochLastBlock = startEpochs[lastBlock];
      Integer lastBlockLastData = (GmatTime(ephemEnd) - startEpochLastBlock).GetTimeInSec()
                                   / timeSteps[lastBlock];

#ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage("End of data is at [%d, %d]\n",
         lastBlock, lastBlockLastData);
#endif

      // Fix up the indexing!
      if ((block == lastBlock) && (line > lastBlockLastData - 4))
      {
         line = lastBlockLastData - 4;
         if (line < 0)
         {
            --block;
            line += 50;
         }
      }
   }

   for (UnsignedInt i = 0; i < 5; ++i)
   {
      usedRecords[i][0] = block;
      usedRecords[i][1] = line;
      ++line;
      if (line == 50)
      {
         // transition to the next record
         ++block;
         line = 0;
      }
   }

   Integer startIndex = 1;
   Integer endIndex = 2;

   // Start of the ephem
   if (/*(record == 0) &&*/ (stateIndex == 0))
      startIndex = 0;
   if (/*(record == 0) &&*/ (stateIndex > 45))
      startIndex = stateIndex - 45;

   GmatTime epoch;
   Real state[6];

   // Brute force for now: refill the interpolator
   interp->Clear();

#ifdef DEBUG_INTERPOLATION
   MessageInterface::ShowMessage("Pairs used for epoch %s:\n", GmatTime(forEpoch).ToString().c_str());
#endif

   for (UnsignedInt i = 0; i < 5; ++i)
   {
      epoch = startEpochs[usedRecords[i][0]];
      epoch.AddSeconds(timeSteps[usedRecords[i][0]] * (usedRecords[i][1]));

#ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage("  [%d, %d]", usedRecords[i][0],
         usedRecords[i][1]);
#endif

      for (UnsignedInt j = 0; j < 3; ++j)
      {
         if (usedRecords[i][1] == 0)
         {
            state[j] = ephemRecords->at(usedRecords[i][0]).firstStateVector_DULT[j] * DUL_TO_KM;
            state[j + 3] = ephemRecords->at(usedRecords[i][0]).firstStateVector_DULT[j + 3] * DUL_DUT_TO_KM_SEC;
         }
         else
         {
            state[j] = ephemRecords->at(usedRecords[i][0]).stateVector2Thru50_DULT[usedRecords[i][1] - 1][j] * DUL_TO_KM;
            state[j + 3] = ephemRecords->at(usedRecords[i][0]).stateVector2Thru50_DULT[usedRecords[i][1] - 1][j + 3] * DUL_DUT_TO_KM_SEC;
         }
      }

      interp->AddPoint(epoch.GetMjd(), state);

#ifdef DEBUG_INTERPOLATION
      MessageInterface::ShowMessage(" --> [%s %lf %lf...\n", epoch.ToString().c_str(),
         state[0], state[1]);
#endif
   }
#ifdef DEBUG_INTERPOLATION
   MessageInterface::ShowMessage("\n");
#endif
}