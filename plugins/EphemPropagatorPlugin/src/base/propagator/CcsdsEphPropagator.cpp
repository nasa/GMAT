//------------------------------------------------------------------------------
//                             CcsdsEphPropagator
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
// Author: Tuan D. Nguyen / GSFC-NASA Code 583
// Created: 2021.07.19
//
/**
 * Implementation for the CcsdsEphPropagator class
 */
//------------------------------------------------------------------------------


#include "CcsdsEphPropagator.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include "PropagatorException.hpp"

#include <sstream>                     // for stringstream

//#define DEBUG_INITIALIZATION
//#define DEBUG_PROPAGATION
//#define DEBUG_INTERPOLATION
//#define DEBUG_FINALSTEP
//#define DEBUG_PROPAGATION


#define PAUSE_AT_BOUNDS


//---------------------------------
// static data
//---------------------------------

/// CcsdsEphPropagator parameter labels
const std::string CcsdsEphPropagator::PARAMETER_TEXT[
                 CcsdsEphPropagatorParamCount - EphemerisPropagatorParamCount] =
{
      "EphemFile"                  //EPHEMERISFILENAME
};

/// CcsdsEphPropagator parameter types
const Gmat::ParameterType CcsdsEphPropagator::PARAMETER_TYPE[
                 CcsdsEphPropagatorParamCount - EphemerisPropagatorParamCount] =
{
      Gmat::FILENAME_TYPE          //EPHEMERISFILENAME
};


//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// CcsdsEphPropagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the object that gets constructed
 */
//------------------------------------------------------------------------------
CcsdsEphPropagator::CcsdsEphPropagator(const std::string &name) :
   EphemerisPropagator        ("CCSDS-OEM", name),
   ephemName                  (""),
   fileDataLoaded             (false),
   ephemRecords               (NULL),
   stateIndex                 (-1),
   timeFromEphemStart         (-1.0),
   lastEpoch                  (-1.0),
   lastEpochGT                (-1.0),
   ephemCoord                 (NULL),
   j2k                        (NULL)
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("CcsdsEphPropagator::CcsdsEphPropagator() default constructor <%p>\n", this);
#endif 
   // GmatBase data
  objectTypeNames.push_back("CcsdsEphPropagator");
  parameterCount        = CcsdsEphPropagatorParamCount;
  theEphem              = &ephem;

#ifdef DEBUG_INITIALIZATION
  MessageInterface::ShowMessage("CcsdsEphPropagator::CcsdsEphPropagator() default constructor <%p> end\n", this);
#endif 
}


//------------------------------------------------------------------------------
// ~CcsdsEphPropagator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
CcsdsEphPropagator::~CcsdsEphPropagator()
{
   if (ephemCoord)
      delete ephemCoord;
   if (j2k)
      delete j2k;
}


//------------------------------------------------------------------------------
// CcsdsEphPropagator(const CcsdsEphPropagator & prop)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param prop The object that is copied into this new one
 */
//------------------------------------------------------------------------------
CcsdsEphPropagator::CcsdsEphPropagator(const CcsdsEphPropagator & prop) :
   EphemerisPropagator        (prop),
   ephemName                  (prop.ephemName),
   fileDataLoaded             (false),
   ephemRecords               (NULL),
   stateIndex                 (-1),
   timeFromEphemStart         (prop.timeFromEphemStart),
   lastEpoch                  (prop.lastEpoch),
   lastEpochGT                (prop.lastEpoch),
   ephemCoord                 (NULL),
   j2k                        (NULL)
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("CcsdsEphPropagator::CcsdsEphPropagator(copy from <%p> ) copy constructor <%p>\n", &prop, this);
#endif 

   theEphem              = &ephem;

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("CcsdsEphPropagator::CcsdsEphPropagator(copy from <%p> ) copy constructor <%p> end\n", &prop, this);
#endif 

}


//------------------------------------------------------------------------------
// CcsdsEphPropagator & CcsdsEphPropagator::operator =(const CcsdsEphPropagator & prop)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param prop The object that is provides data for into this one
 *
 * @return This propagator, configured to match prop.
 */
//------------------------------------------------------------------------------
CcsdsEphPropagator & CcsdsEphPropagator::operator=(const CcsdsEphPropagator & prop)
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("CcsdsEphPropagator::CcsdsEphPropagator::operator=(assign from <%p> ) <%p>\n", &prop, this);
#endif 

   if (this != &prop)
   {
      EphemerisPropagator::operator=(prop);

      ephemName = prop.ephemName;
      fileDataLoaded = false;
      ephemRecords = NULL;
      stateIndex = -1;

      lastEpoch = currentEpoch = prop.currentEpoch;
      lastEpochGT = currentEpochGT = prop.currentEpochGT;

      if (hasPrecisionTime)
         timeFromEphemStart = (lastEpochGT - GmatTime(ephemStart)).GetTimeInSec();
      else
         timeFromEphemStart = (lastEpoch - ephemStart) * GmatTimeConstants::SECS_PER_DAY;
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("CcsdsEphPropagator::CcsdsEphPropagator::operator=(assign from <%p> ) <%p> end\n", &prop, this);
#endif 
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
GmatBase* CcsdsEphPropagator::Clone() const
{
   return new CcsdsEphPropagator(*this);
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
std::string CcsdsEphPropagator::GetParameterText(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < CcsdsEphPropagatorParamCount)
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
Integer CcsdsEphPropagator::GetParameterID(const std::string &str) const
{
   for (Integer i = EphemerisPropagatorParamCount;
         i < CcsdsEphPropagatorParamCount; ++i)
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
Gmat::ParameterType CcsdsEphPropagator::GetParameterType(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < CcsdsEphPropagatorParamCount)
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
std::string CcsdsEphPropagator::GetParameterTypeString(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < CcsdsEphPropagatorParamCount)
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
std::string CcsdsEphPropagator::GetParameterUnit(const Integer id) const
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
bool CcsdsEphPropagator::IsParameterReadOnly(const Integer id) const
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
bool CcsdsEphPropagator::IsParameterReadOnly(const std::string &label) const
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
std::string CcsdsEphPropagator::GetStringParameter(const Integer id) const
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
bool CcsdsEphPropagator::SetStringParameter(const Integer id,
      const std::string &value)
{

   if (id == EPHEMERISFILENAME)
   {
      return true;         // Idempotent, so return true
   }

   bool retval = EphemerisPropagator::SetStringParameter(id, value);

   return retval;
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
std::string CcsdsEphPropagator::GetStringParameter(const Integer id,
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
bool CcsdsEphPropagator::SetStringParameter(const Integer id,
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
const StringArray& CcsdsEphPropagator::GetStringArrayParameter(
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
const StringArray& CcsdsEphPropagator::GetStringArrayParameter(const Integer id,
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
std::string CcsdsEphPropagator::GetStringParameter(const std::string &label) const
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
bool CcsdsEphPropagator::SetStringParameter(const std::string &label,
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
std::string CcsdsEphPropagator::GetStringParameter(const std::string &label,
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
bool CcsdsEphPropagator::SetStringParameter(const std::string &label,
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
const StringArray& CcsdsEphPropagator::GetStringArrayParameter(
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
const StringArray& CcsdsEphPropagator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the CcsdsEphPropagator for use in a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool CcsdsEphPropagator::Initialize()
{
   Integer logOption = 0;
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("CcsdsEphPropagator::Initialize() <%p> entered\n", this);
      logOption = 1;
   #endif

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
         throw PropagatorException("CCSDS-OEM propagators require exactly one "
            "SpaceObject.");

      // The PSM isn't set until PrepareToPropagate fires.  The following is
      // also last minute setup, so only do it if the PSM has been set
      if ((psm != NULL) && !fileDataLoaded)
      {
         for (UnsignedInt i = 0; i < propObjects.size(); ++i)
         {
            if (propObjects[i]->IsOfType(Gmat::SPACECRAFT))
            {
               ephemName = propObjects[i]->GetStringParameter(
                  "EphemerisName");

#ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("Spacecraft <%s> has ephemeris file = <%s>\n",
                  propObjects[i]->GetName().c_str(), ephemName.c_str());
#endif


#ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("Spacecraft epoch is %.12lf\n",
                  currentEpoch);
#endif
            }
            else
               throw PropagatorException("CCSDS-OEM propagators only "
                  "work for Spacecraft.");

            if (ephemName == "")
               throw PropagatorException("The CCSDS-OEM propagator requires a "
                  "valid ephemeris file name");

            fullPath = fm->FindPath(ephemName, "VEHICLE_EPHEM_PATH", true, false, true);

#ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("     ephem file fullPath = <%s>\n", fullPath.c_str());
#endif
            if (fullPath == "")
               throw PropagatorException("The CCSDS-OEM file " +
                  ephemName + " does not exist");

            if (!ephem.OpenForRead(fullPath, "TimePosVel"))
               throw PropagatorException("The CCSDS-OEM file " +
                  ephemName + " failed to open");

#ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("     successfully open ephem file with fullPath = <%s>\n", fullPath.c_str());
#endif

            Integer dumpdata = 0;
#ifdef DEBUG_EPHEM_READ
            dumpdata = 1;
#endif
            ephem.ReadDataRecords(dumpdata);
            SetEphemSpan();
            fileDataLoaded = true;

            // Setup central body
            //centralBody = ephem.GetCentralBody();
            if (centralBody == "Moon")
               centralBody = "Luna";
            if (centralBody == "")     // CCSDS .e spec: Use "vehicle CB, and Earth by default"
               centralBody = "Earth";

            propOrigin = solarSystem->GetBody(centralBody);

            ephem.CloseForRead();
			   currentEpoch = initialEpoch;
            Rvector6 interpVal = ephem.InterpolatePoint(currentEpoch);
            std::memcpy(state, interpVal.GetDataVector(),
               6 * sizeof(Real));

            // Setup ephem coordinate system and j2kBodyMJ2000Eq coordinate system
            std::string axisSystemOnFile = ephem.GetReferenceFrame();
            BuildCoordinates(ephem.GetReferenceFrame());

            if (hasPrecisionTime)
            {
			      currentEpochGT = initialEpochGT;
			      lastEpoch = currentEpoch;
			      lastEpochGT = currentEpochGT;
               timeFromEphemStart = (lastEpochGT - GmatTime(ephemStart)).GetTimeInSec();
               UpdateState();
               UpdateSpaceObjectGT(currentEpochGT);
            }
            else
            {
			      currentEpochGT = initialEpoch;
			      lastEpoch = currentEpoch;
			      lastEpochGT = currentEpochGT;
               timeFromEphemStart = (lastEpoch - ephemStart) * GmatTimeConstants::SECS_PER_DAY;
               UpdateState();
               UpdateSpaceObject(currentEpoch);
            }
            
         }
      }
   }
   else
   {
      isInitialized = false;
      return false;
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
      MessageInterface::ShowMessage("CcsdsEphPropagator::Initialize(), <%p> on exit, "
            "initialEpoch = %.12lf, current = %.12lf\n", this, initialEpoch,
            currentEpoch);
   #endif
   
   isInitialized = true;
   return true;
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
//@todo: need GmatTime modification
bool CcsdsEphPropagator::Step()
{
   #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("CcsdsEphPropagator::Step() entered for <%p>: "
            "initialEpoch = %.12lf; stepsize = %.12lf; "
            "timeFromEpoch = %.12lf\n", this, initialEpoch, ephemStep,
            timeFromEpoch);
   #endif

   bool retval = false;

   Rvector6  outState;

   #ifdef DEBUG_FINALSTEP
      if (currentEpoch == ephemEnd)
         MessageInterface::ShowMessage("Stepping off the end of the ephem\n");
   #endif
   if (hasPrecisionTime)
   {
      if (lastEpochGT != currentEpochGT)
      {
         lastEpochGT = currentEpochGT;
         timeFromEphemStart = (lastEpochGT - GmatTime(ephemStart)).GetTimeInSec();
      }
   }
   else
   {
      if (lastEpoch != currentEpoch)
      {
         lastEpoch = currentEpoch;
         timeFromEphemStart = (lastEpoch - ephemStart) *
            GmatTimeConstants::SECS_PER_DAY;
      }
   }

   timeFromEphemStart += ephemStep;
   timeFromEpoch += ephemStep;
   stepTaken = ephemStep;

   if (hasPrecisionTime)
      currentEpochGT = GmatTime(ephemStart).AddSeconds(timeFromEphemStart);
   else
      currentEpoch = ephemStart + timeFromEphemStart /
         GmatTimeConstants::SECS_PER_DAY;

   #ifdef DEBUG_PROPAGATION
   if (hasPrecisionTime)
      MessageInterface::ShowMessage("   ephemStart = %.12lf, timeFromStart = "
         "%lf sec => currentEpochGT after step = %s; lastEpoch = %s; "
         "ephemEnd = %.12lf\n", ephemStart, timeFromEphemStart, 
         currentEpochGT.ToString().c_str(),
         lastEpochGT.ToString().c_str(), ephemEnd);
   else
      MessageInterface::ShowMessage("   ephemStart = %.12lf, timeFromStart = "
         "%lf sec => currentEpoch after step = %.12lf; lastEpoch = %.12lf; "
         "ephemEnd = %.12lf\n", ephemStart, timeFromEphemStart, currentEpoch,
         lastEpoch, ephemEnd);
   #endif

   #ifdef PAUSE_AT_BOUNDS
      // Code to step to ephem bound before stepping out of bounds
   if (hasPrecisionTime)
   {
      if ((lastEpochGT < GmatTime(ephemEnd)) && (currentEpochGT > GmatTime(ephemEnd)))
         currentEpochGT = GmatTime(ephemEnd);
      if ((lastEpochGT > GmatTime(ephemStart)) && (currentEpochGT < GmatTime(ephemStart)))
         currentEpochGT = GmatTime(ephemStart);
      #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("   --> Updated currentEpoch after step "
         "= %s\n", currentEpochGT.ToString().c_str());
      #endif
   }
   else
   {
      if ((lastEpoch < ephemEnd) && (currentEpoch > ephemEnd))
         currentEpoch = ephemEnd;
      if ((lastEpoch > ephemStart) && (currentEpoch < ephemStart))
         currentEpoch = ephemStart;
      #ifdef DEBUG_PROPAGATION
      MessageInterface::ShowMessage("   --> Updated currentEpoch after step "
         "= %.12lf\n", currentEpoch);
      #endif
   }
   #endif

   // Allow for slop in the last few bits
   bool flagOutOfDomain = false;
   if (hasPrecisionTime)
   {
      if (currentEpochGT < GmatTime(ephemStart))
      {
         if ((GmatTime(ephemStart) - currentEpochGT).GetMjd() < 1.0e-10)
            currentEpochGT = GmatTime(ephemStart);
         else
            flagOutOfDomain = true;
      }
      else if (currentEpochGT > GmatTime(ephemEnd))
      {
         if ((currentEpochGT - GmatTime(ephemEnd)).GetMjd() < 1.0e-10)
            currentEpochGT = GmatTime(ephemEnd);
         else
            flagOutOfDomain = true;
      }
   }
   else
   {
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
   }

   #ifdef DEBUG_FINALSTEP
   MessageInterface::ShowMessage("******** ==> Triggered step past end code\n");
   #endif

   if (flagOutOfDomain)
   {
      #ifdef DEBUG_PROPAGATION
      if (hasPrecisionTime)
         MessageInterface::ShowMessage("   ephemStart = %.12lf, timeFromStart = "
            "%lf sec => currentEpochGT after step = %s; lastEpochGT = %s; "
            "ephemEnd = %.12lf\n", ephemStart, timeFromEphemStart, 
            currentEpochGT.ToString().c_str(),
            lastEpochGT.ToString().c_str(), ephemEnd);
      else
         MessageInterface::ShowMessage("   ephemStart = %.12lf, timeFromStart = "
         "%lf sec => currentEpoch after step = %.12lf; lastEpoch = %.12lf; "
         "ephemEnd = %.12lf\n", ephemStart, timeFromEphemStart, currentEpoch,
         lastEpoch, ephemEnd);
      #endif

      std::stringstream errmsg;
      errmsg.precision(16);
      errmsg << "The CCSDS-OEM Propagator "
             << instanceName
             << " is attempting to step outside of the span of the "
                "ephemeris data; halting.  ";
      errmsg << "The current CCSDS-OEM file covers the A.1 modified "
                "Julian span ";
      errmsg << ephemStart << " to " << ephemEnd << " and the "
         "requested epoch is ";
      if (hasPrecisionTime)
         errmsg << currentEpochGT.ToString() << ".";
      else
         errmsg << currentEpoch << ".";
      throw PropagatorException(errmsg.str());
   }

   #ifdef DEBUG_FINALSTEP
   if (hasPrecisionTime)
      MessageInterface::ShowMessage("Last epoch: %s Stepping to %s; step "
         "size %.12lf\n", lastEpochGT.ToString().c_str(), 
         currentEpochGT.ToString().c_str(),
         (currentEpochGT - lastEpochGT).GetTimeInSec());
   else
      MessageInterface::ShowMessage("Last epoch: %.12lf Stepping to %.12lf; step "
         "size %.12lf\n", lastEpoch, currentEpoch,
         (currentEpoch - lastEpoch) * 86400.0);
   #endif
   
   Rvector6 interpVal;
   if (hasPrecisionTime)
   {
      interpVal = ephem.InterpolatePoint(currentEpochGT.GetMjd());
      lastEpochGT = currentEpochGT;
   }
   else
   {
      interpVal = ephem.InterpolatePoint(currentEpoch);
      lastEpoch = currentEpoch;
   }
   
   std::memcpy(state, interpVal.GetDataVector(), 6*sizeof(Real));

   #ifdef DEBUG_FINALSTEP
      if (currentEpoch == ephemEnd)
         MessageInterface::ShowMessage("Stepped to the end of the ephem\n");
   #endif
   
   if (hasPrecisionTime)
      UpdateSpaceObjectGT(currentEpochGT);
   else
      UpdateSpaceObject(currentEpoch);

   #ifdef DEBUG_PROPAGATION
      if (hasPrecisionTime)
         MessageInterface::ShowMessage("(Step for %p) State at epoch "
            "%s is [", this, currentEpochGT.ToString().c_str());
      else
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
   //Make sure all propObjects know that they propagated
   for (int i = 0; i < propObjects.size(); i++)
      ((SpaceObject*)(propObjects[i]))->HasEphemPropagated(true);

   retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// bool RawStep()
//------------------------------------------------------------------------------
/**
 * Performs a propagation step without error control
 *
 * @note: RawStep is not used with the CcsdsEphPropagator
 *
 * @return false always
 */
//------------------------------------------------------------------------------
bool CcsdsEphPropagator::RawStep()
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// Real GetStepTaken()
//------------------------------------------------------------------------------
/**
 * Retrieves the size of the most recent CcsdsEphPropagator step
 *
 * @return The most recent step (0.0 if no step was taken with this instance).
 */
//------------------------------------------------------------------------------
Real CcsdsEphPropagator::GetStepTaken()
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
void CcsdsEphPropagator::UpdateState()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Updating state to epoch %.12lf\n",
            currentEpoch);
   #endif

   // Update state
   Rvector6 theState;
   if (hasPrecisionTime)
      theState = ephem.InterpolatePoint(currentEpochGT.GetMjd());
   else
      theState = ephem.InterpolatePoint(currentEpoch);
   
   if (ephemCoord)
   {
      // std::memcpy(state, theState.GetDataVector(), 6 * sizeof(Real));
      Real *ephemState = new Real[6];
      std::memcpy(ephemState, theState.GetDataVector(), 6 * sizeof(Real));

      // Update state and j2kState
      if (hasPrecisionTime)
      {
         cc.Convert(currentEpochGT, ephemState, ephemCoord, state, j2k);
         //cc.Convert(currentEpochGT, state, ephemCoord, j2kState, j2k);
      }
      else
      {
         cc.Convert(currentEpoch, ephemState, ephemCoord, state, j2k);
         //cc.Convert(currentEpoch, state, ephemCoord, j2kState, j2k);
      } 
      delete[] ephemState;
   }
   else 
      std::memcpy(state, theState.GetDataVector(), 6 * sizeof(Real));
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
void CcsdsEphPropagator::SetEphemSpan(Integer whichOne)
{
   ephem.GetStartAndEndEpochs(ephemStart, ephemEnd, &ephemRecords);
   spanStart.push_back(ephemStart);
   spanEnd.push_back(ephemEnd);
   if (!initialEphemEpochSet)
   {
	   EphemerisPropagator::SetEphemSpan(whichOne);
   }
}


//------------------------------------------------------------------------------
// void BuildCoordinates(const std::string & coordinates)
//------------------------------------------------------------------------------
/**
 * Configure coordinate systems for conversion
 *
 * @param coordinates - the coordinate system to convert from
 */
void CcsdsEphPropagator::BuildCoordinates(const std::string & coordinates)
{

   if (coordinates != "")
   {
      std::string cb = ephem.GetCentralBody();
      if ((coordinates != "EME2000") || (cb != "Earth"))
      {

         if (j2k)
            delete j2k;
         if (ephemCoord)
            delete ephemCoord;

         //Here we are transforming from the central body in the ephem to Earth and back. This will
         //cause some noise depending on the distance to the body. 

         std::string cb = ephem.GetCentralBody();
         if (cb == "Moon")
            cb = "Luna";

         // Setup ephem coordinate system and j2kBodyMJ2000Eq coordinate system                      
         std::string axisSystemOnFile = ephem.GetReferenceFrame();                                   
         if (axisSystemOnFile == "EME2000")                                                          
            axisSystemOnFile = "MJ2000Eq"; 
         else if (axisSystemOnFile == "ICRF")
            axisSystemOnFile = "ICRF";
         else if (axisSystemOnFile == "TOD")
            axisSystemOnFile = "TODEq";
         else if (axisSystemOnFile == "TDR")
            axisSystemOnFile = "BodyFixed";
         else if (axisSystemOnFile == "GRC")
            axisSystemOnFile = "BodyFixed";
         else
            throw PropagatorException("The Coordinate System \"" + coordinates + "\" in use in \"" + ephemName + "\" is not supported by GMAT.\n");

         CelestialBody * ori = solarSystem->GetBody(cb);
         CelestialBody *earth = solarSystem->GetBody("Earth");
         ephemCoord = CoordinateSystem::CreateLocalCoordinateSystem("csCCSDS_OEM", axisSystemOnFile, 
            ori, NULL, NULL, earth, solarSystem);                                           
         j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2kBodyMJ2000Eq", "MJ2000Eq",          
            propOrigin, NULL, NULL, earth, solarSystem, true);                                        

      }
   }

}

////------------------------------------------------------------------------------
//// void CcsdsEphPropagator::FindRecord(GmatEpoch forEpoch)
////------------------------------------------------------------------------------
///**
// * Sets up the indices into the ephem data for a input epoch
// *
// * On return, stateIndex points to the record of the data point matching or
// * earlier than forEpoch.
// *
// * @param forEpoch The epoch that is being set up
// */
////------------------------------------------------------------------------------
//void CcsdsEphPropagator::FindRecord(GmatEpoch forEpoch)
//{
//   GmatEpoch currentEpoch;
//
//   stateIndex = -1;
//   for (UnsignedInt i = 0; i < ephemRecords->size(); ++i)
//   {
//      currentEpoch = ephemStart + ephemRecords->at(i).timeFromEpoch / GmatTimeConstants::SECS_PER_DAY;
//
//      if (currentEpoch > forEpoch)
//         break;
//
//      stateIndex = i;
//   }
//}
//
//void CcsdsEphPropagator::FindRecord(GmatTime forEpoch)
//{
//   GmatEpoch currentEpoch;
//
//   stateIndex = -1;
//   for (UnsignedInt i = 0; i < ephemRecords->size(); ++i)
//   {
//      currentEpochGT = ephemStart + ephemRecords->at(i).timeFromEpoch / GmatTimeConstants::SECS_PER_DAY;
//
//      if (currentEpochGT > forEpoch)
//         break;
//
//      stateIndex = i;
//   }
//}
//
//
////------------------------------------------------------------------------------
//// void GetState(GmatEpoch forEpoch, Rvector6 &outstate)
////------------------------------------------------------------------------------
///**
// * Returns the state data at the specified epoch
// *
// * @param forEpoch The epoch for the data
// * @param outstate The state vector that receives the data
// */
////------------------------------------------------------------------------------
//void CcsdsEphPropagator::GetState(GmatEpoch forEpoch, Rvector6 &outstate)
//{
//   UpdateInterpolator(forEpoch);
//
//   Real theState[6];
//   if (interp->Interpolate(forEpoch, theState))
//      outstate.Set(theState);
//   else
//   {
//      std::stringstream date;
//      date.precision(16);
//      date << forEpoch;
//
//      throw PropagatorException("The propagator " + instanceName +
//               " failed to interpolate a valid state for " +
//               propObjects[0]->GetName() + " for epoch " + date.str());
//   }
//
//   #ifdef DEBUG_INTERPOLATION
//      MessageInterface::ShowMessage("Interpolated state: %.12lf  [%.15lf  "
//            "%.15lf  %.15lf  %.15lf  %.15lf  %.15lf]\n", forEpoch, theState[0],
//            theState[1], theState[2], theState[3], theState[4], theState[5]);
//   #endif
//}
