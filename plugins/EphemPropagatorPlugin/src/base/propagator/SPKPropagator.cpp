//------------------------------------------------------------------------------
//                             SPKPropagator
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
// Created: Mar 26, 2010 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task 28
//
/**
 * Implementation for the SPKPropagator class
 */
//------------------------------------------------------------------------------


#include "SPKPropagator.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include <sstream>                 // for stringstream

//#define DEBUG_INITIALIZATION
//#define DEBUG_PROPAGATION
//#define TEST_TDB_ROUND_TRIP
//#define DEBUG_EPHEM_GAPS

//---------------------------------
// static data
//---------------------------------

/// SPKPropagator parameter labels
const std::string SPKPropagator::PARAMETER_TEXT[
                 SPKPropagatorParamCount - EphemerisPropagatorParamCount] =
{
      "SPKFiles"                    //SPKFILENAMES
};

/// SPKPropagator parameter types
const Gmat::ParameterType SPKPropagator::PARAMETER_TYPE[
                 SPKPropagatorParamCount - EphemerisPropagatorParamCount] =
{
      Gmat::STRINGARRAY_TYPE        //SPKFILENAMES
};


//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// SPKPropagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the object that gets constructed
 */
//------------------------------------------------------------------------------
SPKPropagator::SPKPropagator(const std::string &name) :
   EphemerisPropagator        ("SPK", name),
   warnOffEphem               (true),
   skr                        (NULL)
{
   // GmatBase data
  objectTypeNames.push_back("SPK");
  parameterCount       = SPKPropagatorParamCount;

  spkCentralBody       = centralBody;
  spkCentralBodyNaifId = SpiceInterface::DEFAULT_NAIF_ID;
}


//------------------------------------------------------------------------------
// ~SPKPropagator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SPKPropagator::~SPKPropagator()
{
   if (skr)
   {
      // unload the SPK kernels so they will not be retained in the kernel
      // pool
      skr->UnloadKernels(spkFileNames);
      delete skr;
   }
}


//------------------------------------------------------------------------------
// SPKPropagator(const SPKPropagator & spk)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param spk The object that is copied into this new one
 */
//------------------------------------------------------------------------------
SPKPropagator::SPKPropagator(const SPKPropagator & spk) :
   EphemerisPropagator        (spk),
   spkCentralBody             (spk.spkCentralBody),
   spkCentralBodyNaifId       (spk.spkCentralBodyNaifId),
   warnOffEphem               (spk.warnOffEphem),
   skr                        (NULL)
{
}


//------------------------------------------------------------------------------
// SPKPropagator & SPKPropagator::operator =(const SPKPropagator & spk)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param spk The object that is provides data for into this one
 *
 * @return This propagator, configured to match spk.
 */
//------------------------------------------------------------------------------
SPKPropagator & SPKPropagator::operator =(const SPKPropagator & spk)
{
   if (this != &spk)
   {
      EphemerisPropagator::operator=(spk);

      skr                  = NULL;
      warnOffEphem         = spk.warnOffEphem;
      spkCentralBody       = spk.spkCentralBody;
      spkCentralBodyNaifId = spk.spkCentralBodyNaifId;
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
GmatBase* SPKPropagator::Clone() const
{
   return new SPKPropagator(*this);
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
std::string SPKPropagator::GetParameterText(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < SPKPropagatorParamCount)
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
Integer SPKPropagator::GetParameterID(const std::string &str) const
{
   for (Integer i = EphemerisPropagatorParamCount;
         i < SPKPropagatorParamCount; ++i)
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
Gmat::ParameterType SPKPropagator::GetParameterType(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < SPKPropagatorParamCount)
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
std::string SPKPropagator::GetParameterTypeString(const Integer id) const
{
   if (id >= EphemerisPropagatorParamCount && id < SPKPropagatorParamCount)
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
std::string SPKPropagator::GetParameterUnit(const Integer id) const
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
bool SPKPropagator::IsParameterReadOnly(const Integer id) const
{
   if (id == SPKFILENAMES)
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
bool SPKPropagator::IsParameterReadOnly(const std::string &label) const
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
std::string SPKPropagator::GetStringParameter(const Integer id) const
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
bool SPKPropagator::SetStringParameter(const Integer id,
      const std::string &value)
{

   if (id == SPKFILENAMES)
   {
      if (value != "")
         if (find(spkFileNames.begin(), spkFileNames.end(), value) ==
               spkFileNames.end())
            spkFileNames.push_back(value);
      return true;         // Idempotent, so return true
   }

   bool retval = EphemerisPropagator::SetStringParameter(id, value);

   if ((retval = true) && (id == EPHEM_CENTRAL_BODY))
   {
      // Special case code that we may want to remove later
      if (value == "Moon")
         throw PropagatorException("\"Moon\" is not an allowed central body; "
               "try \"Luna\"", Gmat::ERROR_);
      if (centralBody == "Luna")
         spkCentralBody = "Moon";
      else
         spkCentralBody = centralBody;
   }

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
std::string SPKPropagator::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == SPKFILENAMES)
   {
      if ((index >= 0) && (index < (Integer)spkFileNames.size()))
         return spkFileNames[index];
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
bool SPKPropagator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   if (id == SPKFILENAMES)
   {
      if ((index >= 0) && (index < (Integer)spkFileNames.size()))
      {
         spkFileNames[index] = value;
         return true;
      }
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
const StringArray& SPKPropagator::GetStringArrayParameter(
      const Integer id) const
{
   if (id == SPKFILENAMES)
      return spkFileNames;
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
const StringArray& SPKPropagator::GetStringArrayParameter(const Integer id,
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
std::string SPKPropagator::GetStringParameter(const std::string &label) const
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
bool SPKPropagator::SetStringParameter(const std::string &label,
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
std::string SPKPropagator::GetStringParameter(const std::string &label,
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
bool SPKPropagator::SetStringParameter(const std::string &label,
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
const StringArray& SPKPropagator::GetStringArrayParameter(
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
const StringArray& SPKPropagator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the SPKPropagator for use in a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
//@todo: need GmatTime modification
bool SPKPropagator::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SPKPropagator::Initialize() entered\n");
      MessageInterface::ShowMessage("spkCentralBody is %s\n", spkCentralBody.c_str());
      MessageInterface::ShowMessage("solarSystem is %sNULL\n", (solarSystem? "NOT " : ""));
   #endif

   bool retval = false;

   if (EphemerisPropagator::Initialize())
   {
      // If skr already set, just keep it
      if (skr == NULL)
         skr = new SpiceOrbitKernelReader;

      spkCentralBodyNaifId = skr->GetNaifID(spkCentralBody);

      stepTaken = 0.0;
      j2ET = j2000_c();   // CSPICE method to return Julian date of J2000 (TDB)

      FileManager *fm = FileManager::Instance();
      std::string fullPath;

      if (propObjects.size() != 1)
         throw PropagatorException("SPICE propagators (i.e. \"SPK\" "
               "propagators) require exactly one SpaceObject.");
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Clearing %d naifIds\n", naifIds.size());
      #endif
      naifIds.clear();

      // The PSM isn't set until PrepareToPropagate fires.  The following is
      // also last minute setup, so only do it if the PSM has been set
      if (psm != NULL)
      {
         for (UnsignedInt i = 0; i < propObjects.size(); ++i)
         {
            Integer id = propObjects[i]->GetIntegerParameter("NAIFId");
            naifIds.push_back(id);

            // Load the SPICE files for each propObject
            StringArray spices;
            if (propObjects[i]->IsOfType(Gmat::SPACECRAFT))
               spices = propObjects[i]->GetStringArrayParameter(
                     "OrbitSpiceKernelName");
            else
               throw PropagatorException("Spice (SPK) propagators only work for "
                     "Spacecraft right now.");

            if (spices.size() == 0)
               throw PropagatorException("Spice (SPK) propagator requires at "
                     "least one orbit SPICE kernel for spacecraft " +
                     propObjects[i]->GetName());
            
            std::string spiceFile;
            for (UnsignedInt j = 0; j < spices.size(); ++j)
            {
               //fullPath = spices[j];
               spiceFile = spices[j];
               fullPath = fm->FindPath(spiceFile, "VEHICLE_EPHEM_SPK_PATH", true, false, true);
               
               #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage
                  ("Checking for kernel spiceFile = '%s', fullPath = '%s'\n",
                   spiceFile.c_str(), fullPath.c_str());
               #endif
               
               if (fullPath == "")
               {
                  throw PropagatorException("The Spice (SPK) file " + spiceFile + " does not exist");
               }
               
               if (skr->IsLoaded(fullPath) == false)
                  skr->LoadKernel(fullPath);

               if (find(spkFileNames.begin(), spkFileNames.end(), fullPath) ==
                     spkFileNames.end())
                  spkFileNames.push_back(fullPath);
            }

            // Build the list of start and end segments in the ephemeris
            LoadSpans();
         }

         // Set the start and end epochs before working with the data
         SetEphemSpan();
         // Enable a single "off ephem" warning message
         warnOffEphem = true;

         for (UnsignedInt i = 0; i < propObjects.size(); ++i)
         {
            #ifdef DEBUG_EPHEM_GAPS
               std::string scName = propObjects[0]->GetName();
               MessageInterface::ShowMessage("%s has %d ephemeris spans:\n",
                     scName.c_str(), spanStart.size());
               for (UnsignedInt i = 0; i < spanStart.size(); ++i)
                  MessageInterface::ShowMessage("   %d:  %.12lf to %.12lf\n", i+1,
                        spanStart[i], spanEnd[i]);
            #endif

            // Load the initial data point
            if (skr)
            {
               bool setState = true;

               try
               {
                  Rvector6  outState;
                  Integer   perSatDim = psm->GetState()->GetSize();

                  if (hasPrecisionTime)
                  {
                     for (UnsignedInt i = 0; i < propObjects.size(); ++i)
                     {
                        std::string scName = propObjectNames[i];
                        Integer id = naifIds[i];

                        if (initialEpochGT.GetMjd() < 0.0)
                           // match GT to the Real time if it hasn't been matched
                           initialEpochGT = initialEpoch;
                        currentEpochGT = initialEpochGT;
                        currentEpochGT.AddSeconds(timeFromEpoch);

                        // Allow for slop in the last few bits
                        if ((currentEpochGT < ephemStart - 1e-10) ||
                           (currentEpochGT > ephemEnd + 1e-10))
                        {
                           std::stringstream errmsg;
                           errmsg.precision(16);
                           errmsg << "The SPKPropagator "
                              << instanceName 
                              << " is attempting to initialize outside of the "
                              "timespan of the ephemeris data; halting.  ";
                           errmsg << "The current SPICE ephemeris covers the A.1 "
                              "modified Julian span ";
                           errmsg << ephemStart << " to " << ephemEnd << " and the " 
                              "requested epoch is " << currentEpochGT.ToString() << ".\n";
                           if (stopOutsideOfEphem)
                              throw PropagatorException(errmsg.str());

                           if (warnOffEphem)
                           {
                              MessageInterface::ShowMessage("WARNING:  %s\n",
                                    errmsg.str().c_str());
                              warnOffEphem = false;
                           }
                           setState = false;
                        }
                        #ifdef DEBUG_INITIALIZATION
                        MessageInterface::ShowMessage("Getting target state in %p " 
                           "for %s (ID = %ld) at epoch %s and CB %s\n", this, 
                           scName.c_str(), id, currentEpochGT.ToString().c_str(), 
                           spkCentralBody.c_str());
                        #endif

                        if (setState)
                           outState = skr->GetTargetState(scName, id, currentEpochGT,
                                            spkCentralBody, spkCentralBodyNaifId);

                        // Fill in the Cartesian state data
                        std::memcpy(state + i * perSatDim, outState.GetDataVector(),
                                    6*sizeof(Real));
                     }

                     UpdateSpaceObjectGT(currentEpochGT);

                     retval = true;
                  }
                  else
                  {
                     for (UnsignedInt i = 0; i < propObjects.size(); ++i)
                     {
                        std::string scName = propObjectNames[i];
                        Integer id = naifIds[i];

                        currentEpoch = initialEpoch + timeFromEpoch /
                           GmatTimeConstants::SECS_PER_DAY;

                        // Allow for slop in the last few bits
                        if ((currentEpoch < ephemStart - 1e-10) ||
                           (currentEpoch > ephemEnd + 1e-10))
                        {
                           std::stringstream errmsg;
                           errmsg.precision(16);
                           errmsg << "The SPKPropagator "
                              << instanceName
                              << " is attempting to initialize outside of the "
                              "timespan of the ephemeris data; halting.  ";
                           errmsg << "The current SPICE ephemeris covers the A.1 "
                              "modified Julian span ";
                           errmsg << ephemStart << " to " << ephemEnd << " and the "
                              "requested epoch is " << currentEpoch << ".\n";
                           if (stopOutsideOfEphem)
                              throw PropagatorException(errmsg.str());

                           if (warnOffEphem)
                           {
                              MessageInterface::ShowMessage("WARNING:  %s\n",
                                    errmsg.str().c_str());
                              warnOffEphem = false;
                           }
                           setState = false;
                        }
                        #ifdef DEBUG_INITIALIZATION
                        MessageInterface::ShowMessage("Getting target state in %p "
                           "for %s (ID = %ld) at epoch %lf and CB %s\n", this,
                           scName.c_str(), id, currentEpoch,
                           spkCentralBody.c_str());
                        #endif

                        if (setState)
                           outState = skr->GetTargetState(scName, id, A1Mjd(currentEpoch),
                                                       spkCentralBody, spkCentralBodyNaifId);

                        std::memcpy(state, outState.GetDataVector(), 6*sizeof(Real));
                     }

                     UpdateSpaceObject(currentEpoch);

                     retval = true;
                  }
               }
               catch (BaseException &e)
               {
                  MessageInterface::ShowMessage(e.GetFullMessage());
                  retval = false;
                  throw;
               }
            }
         }
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("SPKPropagator::Initialize(): Start state "
            "at epoch %.12lf is [", currentEpoch);
      for (Integer i = 0; i < dimension; ++i)
      {
         MessageInterface::ShowMessage("%.12lf", state[i]);
         if (i < dimension-1)
            MessageInterface::ShowMessage("   ");
         else
            MessageInterface::ShowMessage("]\n");
      }
      MessageInterface::ShowMessage("SPKPropagator::Initialize() finished\n");
   #endif

   return retval;
}



//------------------------------------------------------------------------------
// bool LoadSpans()
//------------------------------------------------------------------------------
/**
 * Calls the SPICE kernel reader to generate start and stop times for the ephem segments
 *
 * @return false if no times were set, true if at least one was set
 */
//------------------------------------------------------------------------------
bool SPKPropagator::LoadSpans()
{
   skr->GetCoverageSpans(spkFileNames, naifIds[0], spanStart, spanEnd);

   #ifdef DEBUG_GAPS
      MessageInterface::ShowMessage("%s has %d ephemeris spans for %s\n",
            instanceName.c_str(), spanStart.size(), propObjectNames[0].c_str());
   #endif
   
   return (spanStart.size() > 0);
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
bool SPKPropagator::Step()
{
   #ifdef DEBUG_PROPAGATION
   if (hasPrecisionTime)
   {
      MessageInterface::ShowMessage("SPKPropagator::Step() entered: "
            "initialEpoch = %s; stepsize = %.12lf; "
            "timeFromEpoch = %.12lf\n", initialEpochGT.ToString().c_str(),
            ephemStep, timeFromEpoch);
   }
   else
   {
      MessageInterface::ShowMessage("SPKPropagator::Step() entered: "
            "initialEpoch = %.12lf; stepsize = %.12lf; "
            "timeFromEpoch = %.12lf\n", initialEpoch, ephemStep, timeFromEpoch);
   }
   #endif

   bool retval = true;
   bool takeStep = true;

   if (skr)
   {
      try
      {
         Rvector6  outState;

         if (hasPrecisionTime)
         {
            for (UnsignedInt i = 0; i < propObjects.size(); ++i)
            {
               std::string scName = propObjectNames[i];
               Integer id = naifIds[i];

               if (lastEpochGT != currentEpochGT)
               {
                  lastEpochGT = currentEpochGT;
                  timeFromEphemStart = (lastEpochGT - ephemStart).GetTimeInSec();
               }

               timeFromEphemStart += ephemStep;
               timeFromEpoch += ephemStep;
               stepTaken = ephemStep;

               currentEpochGT = ephemStart;
               currentEpochGT.AddSeconds(timeFromEphemStart);

               // Allow for slop in the last few bits
               if ((currentEpochGT < ephemStart - 1e-10) || 
                  (currentEpochGT > ephemEnd + 1e-10))
               {
                  std::stringstream errmsg;
                  errmsg.precision(16);
                  errmsg << "The SPKPropagator " 
                     << instanceName 
                     << " is attempting to step outside of the span of the " 
                     "ephemeris data; halting.";
                  errmsg << "The current SPICE ephemeris covers the A.1 modified " 
                     "Julian span ";
                  errmsg << ephemStart << " to " << ephemEnd << " and the " 
                     "requested epoch is " << currentEpochGT.ToString() << ".\n";
                  if (stopOutsideOfEphem)
                     throw PropagatorException(errmsg.str());

                  //if (warnOffEphem)
                  //{
                  //   MessageInterface::ShowMessage("WARNING:  %s\n",
                  //         errmsg.str().c_str());
                  //   warnOffEphem = false;
                  //}
                  takeStep = false;
                  retval = false;
               }

			   bool skipEphemerisProp = true;

			   try {
				   if (takeStep)
					   outState = skr->GetTargetState(scName, id, currentEpochGT,
						   spkCentralBody, spkCentralBodyNaifId);
			   }
			   catch (BaseException &e)
			   {
				   if (!skipEphemerisProp)
					   throw;
				   else
				   {
					   if (takeStep)
						   if ((currentEpochGT < ephemStart - 1e-10) ||
							   (currentEpochGT > ephemEnd + 1e-10))
							   throw;
					#ifdef DEBUG_GAP
					   MessageInterface::ShowMessage("Skipping %s\n",
						   currentEpochGT.ToString().c_str());
					#endif
				   }
			   }

               /**
               *  @todo: When SPKProp can evolve more than one spacecraft, these
               *  memcpy lines need revision
               */
               //            std::memcpy(state, outState.GetDataVector(),
               //                  dimension*sizeof(Real));
               //            ReturnFromOrigin(currentEpoch);
               //            std::memcpy(j2kState, outState.GetDataVector(),
               std::memcpy(state, outState.GetDataVector(), 6*sizeof(Real));
               //MoveToOrigin(currentEpoch);

               lastEpoch = currentEpoch;
               lastEpochGT = currentEpochGT;

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

         }
         else
         {
            for (UnsignedInt i = 0; i < propObjects.size(); ++i)
            {
               std::string scName = propObjectNames[i];
               Integer id = naifIds[i];

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

               // Allow for slop in the last few bits
               if ((currentEpoch < ephemStart - 1e-10) ||
                  (currentEpoch > ephemEnd + 1e-10))
               {
                  std::stringstream errmsg;
                  errmsg.precision(16);
                  errmsg << "The SPKPropagator "
                     << instanceName
                     << " is attempting to step outside of the span of the "
                     "ephemeris data; halting.";
                  errmsg << "The current SPICE ephemeris covers the A.1 modified "
                     "Julian span ";
                  errmsg << ephemStart << " to " << ephemEnd << " and the "
                     "requested epoch is " << currentEpoch << ".\n";
                  if (stopOutsideOfEphem)
                     throw PropagatorException(errmsg.str());

                  //if (warnOffEphem)
                  //{
                  //   MessageInterface::ShowMessage("WARNING:  %s\n",
                  //         errmsg.str().c_str());
                  //   warnOffEphem = false;
                  //}
                  takeStep = false;
                  retval = false;
               }
			   //Always skip gaps for now
			   bool skipEphemerisProp = true;

			   try {
				   if (takeStep)
					   outState = skr->GetTargetState(scName, id, A1Mjd(currentEpoch),
						   spkCentralBody, spkCentralBodyNaifId);
			   } 
			   catch (BaseException &e)
			   {
				   if (!skipEphemerisProp)
					   throw;
				   else
				   {
					   if (takeStep)
						   if ((currentEpochGT < ephemStart - 1e-10) ||
							   (currentEpochGT > ephemEnd + 1e-10))
							   throw;
					#ifdef DEBUG_GAP
					   MessageInterface::ShowMessage("Skipping %s\n",
						   currentEpochGT.ToString().c_str());
					#endif
				   }
			   }
               /**
                *  @todo: When SPKProp can evolve more than one spacecraft, these
                *  memcpy lines need revision
                */
               //            std::memcpy(state, outState.GetDataVector(),
               //                  dimension*sizeof(Real));
               //            ReturnFromOrigin(currentEpoch);
               //            std::memcpy(j2kState, outState.GetDataVector(),
               std::memcpy(state, outState.GetDataVector(), 6*sizeof(Real));
               //MoveToOrigin(currentEpoch);
               lastEpoch = currentEpoch;
               //lastEpochGT = currentEpochGT;
               
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

         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage(e.GetFullMessage());
         retval = false;
         throw;
      }
   }

   #ifdef DEBUG_PROPAGATION
      else
         MessageInterface::ShowMessage("skr was not initialized]\n");
   #endif
         //Make sure all propObjects know that they propagated
   if (retval)
   {
      for (int i = 0; i < propObjects.size(); i++)
         ((SpaceObject*)(propObjects[i]))->HasEphemPropagated(true);
   }
   return retval;
}


//------------------------------------------------------------------------------
// bool RawStep()
//------------------------------------------------------------------------------
/**
 * Performs a propagation step without error control
 *
 * @note: RawStep is not used with the SPKPropagator
 *
 * @return false always
 */
//------------------------------------------------------------------------------
bool SPKPropagator::RawStep()
{
   bool retval = false;
   return retval;
}


//------------------------------------------------------------------------------
// Real GetStepTaken()
//------------------------------------------------------------------------------
/**
 * Retrieves the size of the most recent SPKPropagator step
 *
 * @return The most recent step (0.0 if no step was taken with this instance).
 */
//------------------------------------------------------------------------------
Real SPKPropagator::GetStepTaken()
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
void SPKPropagator::UpdateState()
{
   #ifdef DEBUG_EXECUTION
      if (hasPrecisionTime)
         MessageInterface::ShowMessage("Updating state to epochGT %s\n", 
            currentEpochGT.ToString().c_str());
      else
         MessageInterface::ShowMessage("Updating state to epoch %.12lf\n",
            currentEpoch);
   #endif

   if (skr)
   {
      try
      {
         Rvector6  outState;
         bool takeStep = true;

         if (hasPrecisionTime)
         {
            for (UnsignedInt i = 0; i < propObjects.size(); ++i)
            {
               std::string scName = propObjectNames[i];
               Integer id = naifIds[i];

               // Allow for slop in the last few bits
               if ((currentEpochGT < ephemStart - 1e-10) || 
                  (currentEpochGT > ephemEnd + 1e-10))
               {
                  std::stringstream errmsg;
                  errmsg.precision(16);
                  errmsg << "The SPKPropagator " 
                     << instanceName 
                     << " is attempting to access state data outside of the " 
                     "span of the ephemeris data; halting.  ";
                  errmsg << "The current SPICE ephemeris covers the A.1 modified " 
                     "Julian span " 
                     << ephemStart << " to " << ephemEnd << " and the " 
                     "requested epoch is " << currentEpochGT.ToString() << ".\n";
                  if (stopOutsideOfEphem)
                     throw PropagatorException(errmsg.str());

                  //if (warnOffEphem)
                  //{
                  //   MessageInterface::ShowMessage("WARNING:  %s\n",
                  //         errmsg.str().c_str());
                  //   warnOffEphem = false;
                  //}
                  takeStep = false;
               }

               // Always skip gaps for now
               bool skipEphemerisProp = true;

               try
               {
                  if (takeStep)
                     outState = skr->GetTargetState(scName, id, currentEpochGT,
                        spkCentralBody, spkCentralBodyNaifId);
               }
               catch (BaseException &)
               {
                  if (!skipEphemerisProp)
                     throw;
                  else
                  {
                     if (takeStep)
                        if ((currentEpochGT < ephemStart - 1e-10) ||
                            (currentEpochGT > ephemEnd + 1e-10))
                           throw;
                     #ifdef DEBUG_GAP
                        MessageInterface::ShowMessage("Skipping %s\n",
                              currentEpochGT.ToString().c_str());
                     #endif
                  }
               }

               /**
               *  @todo: When SPKProp can evolve more than one spacecraft, this
               *  memcpy line needs revision
               */
               std::memcpy(state, outState.GetDataVector(), 6*sizeof(Real));
               // Update the j2k state
               ReturnFromOriginGT(currentEpochGT);

               #ifdef DEBUG_PROPAGATION
               MessageInterface::ShowMessage("(UpdateState for %p) State at " 
                  "epoch %s is [", this, currentEpochGT.ToString().c_str());
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
         }
         else
         {
            for (UnsignedInt i = 0; i < propObjects.size(); ++i)
            {
               std::string scName = propObjectNames[i];
               Integer id = naifIds[i];

               // Allow for slop in the last few bits
               if ((currentEpoch < ephemStart - 1e-10) ||
                  (currentEpoch > ephemEnd + 1e-10))
               {
                  std::stringstream errmsg;
                  errmsg.precision(16);
                  errmsg << "The SPKPropagator "
                     << instanceName
                     << " is attempting to access state data outside of the "
                     "span of the ephemeris data; halting.  ";
                  errmsg << "The current SPICE ephemeris covers the A.1 modified "
                     "Julian span "
                     << ephemStart << " to " << ephemEnd << " and the "
                     "requested epoch is " << currentEpoch << ".\n";
                  if (stopOutsideOfEphem)
                     throw PropagatorException(errmsg.str());

                  //if (warnOffEphem)
                  //{
                  //   MessageInterface::ShowMessage("WARNING:  %s\n",
                  //         errmsg.str().c_str());
                  //   warnOffEphem = false;
                  //}
                  takeStep = false;
               }

               if (takeStep)
                  outState = skr->GetTargetState(scName, id, A1Mjd(currentEpoch),
                     spkCentralBody, spkCentralBodyNaifId);

               /**
                *  @todo: When SPKProp can evolve more than one spacecraft, this
                *  memcpy line needs revision
                */
               std::memcpy(state, outState.GetDataVector(), 6*sizeof(Real));
               // Update the j2k state
               ReturnFromOrigin(currentEpoch);

               #ifdef DEBUG_PROPAGATION
               MessageInterface::ShowMessage("(UpdateState for %p) State at "
                  "epoch %.12lf is [", this, currentEpoch);
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
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage(e.GetFullMessage());
         throw;
      }
   }
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
void SPKPropagator::SetEphemSpan(Integer whichOne)
{
   if (whichOne < 0)
      throw PropagatorException("SPKPropagator::SetEphemSpan(Integer whichOne):"
            " Invalid index");

   if (skr)
   {
      // @todo: When the SPKPropagator supports more than one spacecraft, the
      // ephem span needs to be modified to track spans for each spacecraft
      for (UnsignedInt i = 0; i < naifIds.size(); ++i)
         skr->GetCoverageStartAndEnd(spkFileNames, naifIds[i], ephemStart,
               ephemEnd);
	  
	  if (!initialEphemEpochSet)
	  {
		  EphemerisPropagator::SetEphemSpan(whichOne);
	  }

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("EphemSpan is [%.12lf %.12lf]\n",
               ephemStart, ephemEnd);
      #endif
   }
}
