//------------------------------------------------------------------------------
//                             EphemerisPropagator
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
 * Implementation for the EphemerisPropagator class
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <cstdio>                   // for sprintf
#include "EphemerisPropagator.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"


//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION
//#define DEBUG_EXTENDED_STATES
//#define DEBUG_REORIGIN
//#define DEBUG_INITIAL_EPHEM_STATE
//#define DEBUG_REF_OBJ

//------------------------------------------------------------------------------
// Static Data
//------------------------------------------------------------------------------

/// EphemerisPropagator parameter labels
const std::string
EphemerisPropagator::PARAMETER_TEXT[EphemerisPropagatorParamCount - PropagatorParamCount] =
{
      "StepSize",       // EPHEM_STEP_SIZE
      "CentralBody",    // EPHEM_CENTRAL_BODY
      "EpochFormat",    // EPHEM_EPOCH_FORMAT
      "StartEpoch",     // EPHEM_START_EPOCH
      "EphemStart",     // EPHEM_START_A1MJD
      "EphemEnd",     // EPHEM_END_A1MJD
      "StartOptions"    // EPHEM_START_OPTIONS
};

/// EphemerisPropagator parameter types
const Gmat::ParameterType
EphemerisPropagator::PARAMETER_TYPE[EphemerisPropagatorParamCount - PropagatorParamCount] =
{
      Gmat::REAL_TYPE,        // EPHEM_STEP_SIZE
      Gmat::OBJECT_TYPE,      // EPHEM_CENTRAL_BODY
      Gmat::STRING_TYPE,      // EPHEM_EPOCH_FORMAT
      Gmat::STRING_TYPE,      // EPHEM_START_EPOCH
      Gmat::REAL_TYPE,      // EPHEM_START_A1MJD
      Gmat::REAL_TYPE,      // EPHEM_END_A1MJD
      Gmat::STRINGARRAY_TYPE  // EPHEM_START_OPTIONS
};


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EphemerisPropagator(const std::string & typeStr, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param typeStr Script description for the propagator type
 * @param name Name of the constructed object
 */
//------------------------------------------------------------------------------
EphemerisPropagator::EphemerisPropagator(const std::string & typeStr,
      const std::string & name) :
   Propagator           (typeStr, name),
   ephemStep            (300.0),
   epochFormat          ("A1ModJulian"),
   initialEpoch         (-987654321.0),
   currentEpoch         (-987654321.0),
   initialEpochGT       (-987654321.0),
   currentEpochGT       (-987654321.0),
   timeFromEpoch        (0.0),
   ephemStart           (-987654321.0),
   ephemEnd             (987654321.0),
   stopOutsideOfEphem   (true),
   psm                  (NULL),
   state                (NULL),
   j2kState             (NULL),
   stepTaken            (0.0),
   startEpochSource     (FROM_EPHEM),
   initialEphemEpochSet (true),
   stepDirection        (1.0),
   solarSystem          (NULL)
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("EphemerisPropagator::EphemerisPropagator() default constructor <%p>\n", this);
#endif 

   objectTypeNames.push_back("EphemerisPropagator");
   parameterCount = EphemerisPropagatorParamCount;

   std::stringstream epochString("");
   epochString << "EphemStart";
   startEpoch = "EphemStart";
   startOptions.push_back("FromSpacecraft");
   startOptions.push_back("EphemStart");

   theTimeConverter = TimeSystemConverter::Instance();

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("EphemerisPropagator::EphemerisPropagator() default constructor <%p> end\n", this);
#endif 
}


//------------------------------------------------------------------------------
// ~EphemerisPropagator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EphemerisPropagator::~EphemerisPropagator()
{
   if (state != NULL)
      delete [] state;

   if (j2kState != NULL)
      delete [] j2kState;
}


//------------------------------------------------------------------------------
// EphemerisPropagator(const EphemerisPropagator & ep)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ep The Ephemeris propagator copied to the new one.
 */
//------------------------------------------------------------------------------
EphemerisPropagator::EphemerisPropagator(const EphemerisPropagator & ep) :
   Propagator              (ep),
   ephemStep               (ep.ephemStep),
   epochFormat             (ep.epochFormat),
   startEpoch              (ep.startEpoch),
   initialEpoch            (ep.initialEpoch),
   currentEpoch            (ep.currentEpoch),
   initialEpochGT          (ep.initialEpochGT),
   currentEpochGT          (ep.currentEpochGT),
   timeFromEpoch           (ep.timeFromEpoch),
   ephemStart              (ep.ephemStart),
   ephemEnd                (ep.ephemEnd),
   stopOutsideOfEphem      (ep.stopOutsideOfEphem),
   psm                     (NULL),
   state                   (NULL),
   j2kState                (NULL),
   stepTaken               (0.0),
   startOptions            (ep.startOptions),
   startEpochSource        (ep.startEpochSource),
   initialEphemEpochSet	   (ep.initialEphemEpochSet),
   stepDirection           (ep.stepDirection),
   solarSystem             (NULL)
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("EphemerisPropagator::EphemerisPropagator(copy from <%p>) copy constructor <%p>\n", &ep, this);
#endif 
   theTimeConverter = TimeSystemConverter::Instance();

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("EphemerisPropagator::EphemerisPropagator(copy from <%p>) copy constructor <%p> end\n", &ep, this);
#endif 
}


//------------------------------------------------------------------------------
// EphemerisPropagator& operator=(const EphemerisPropagator& ep)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ep The Ephemeris propagator supplying parameters for this one.
 *
 * @return This EphemerisPropagator configured to match ep.
 */
//------------------------------------------------------------------------------
EphemerisPropagator& EphemerisPropagator::operator=(
      const EphemerisPropagator& ep)
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("EphemerisPropagator::operator=(assign from <%p>) assign <%p>\n", &ep, this);
#endif 
   if (this != &ep)
   {
      Propagator::operator=(ep);

      ephemStep     = ep.ephemStep;
      epochFormat   = ep.epochFormat;
      startEpoch    = ep.startEpoch;
      initialEpoch  = ep.initialEpoch;
      currentEpoch  = ep.currentEpoch;
      initialEpochGT = ep.initialEpochGT;
      currentEpochGT = ep.currentEpochGT;

      timeFromEpoch = ep.timeFromEpoch;
      ephemStart    = ep.ephemStart;
      ephemEnd      = ep.ephemEnd;
      stopOutsideOfEphem = ep.stopOutsideOfEphem;
      psm           = NULL;
      if (state != NULL)
      {
         delete [] state;
         state = NULL;
      }
      if (j2kState != NULL)
      {
         delete [] j2kState;
         j2kState = NULL;
      }
      stepTaken        = 0.0;
      startOptions     = ep.startOptions;
      startEpochSource = ep.startEpochSource;
	  initialEphemEpochSet = ep.initialEphemEpochSet;
      stepDirection    = ep.stepDirection;
      solarSystem      = NULL;
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("EphemerisPropagator::operator=(assign from <%p>) assign <%p> end\n", &ep, this);
#endif 
   return *this;
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer EphemerisPropagator::GetParameterID(const std::string &str) const
{
   for (Integer i = PropagatorParamCount;
         i < EphemerisPropagatorParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - PropagatorParamCount])
         return i;
   }

   return Propagator::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string EphemerisPropagator::GetParameterText(const Integer id) const
{
   if (id >= PropagatorParamCount && id < EphemerisPropagatorParamCount)
      return PARAMETER_TEXT[id - PropagatorParamCount];
   return Propagator::GetParameterText(id);
}


//---------------------------------------------------------------------------
//  std::string GetParameterUnit(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the unit for the parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return unit for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string EphemerisPropagator::GetParameterUnit(const Integer id) const
{
   return Propagator::GetParameterUnit(id);
}


//---------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter, or
 *         UNKNOWN_PARAMETER_TYPE.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType EphemerisPropagator::GetParameterType(
      const Integer id) const
{
   if (id >= PropagatorParamCount && id < EphemerisPropagatorParamCount)
      return PARAMETER_TYPE[id - PropagatorParamCount];
   return Propagator::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string EphemerisPropagator::GetParameterTypeString(const Integer id) const
{
   if (id >= PropagatorParamCount && id < EphemerisPropagatorParamCount)
      return Propagator::PARAM_TYPE_STRING[GetParameterType(id)];
   return Propagator::GetParameterTypeString(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param id Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool EphemerisPropagator::IsParameterReadOnly(const Integer id) const
{
   if ((id == INITIAL_STEP_SIZE) || (id == EPHEM_START_OPTIONS) || (id == EPHEM_START_A1MJD) || (id == EPHEM_END_A1MJD))
      return true;
   return Propagator::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param label Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool EphemerisPropagator::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::IsParameterCommandModeSettable(const Integer id) const
{
   // Override this one from the parent class
   if ((id == EPHEM_STEP_SIZE) || (id ==EPHEM_EPOCH_FORMAT) ||
       (id == EPHEM_START_EPOCH))
      return true;
   
   // And let the parent class handle its own
   return Propagator::IsParameterCommandModeSettable(id);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the Real parameters specific to the EphemerisPropagator
 *
 * @param id The ID for the parameter that is retrieved
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const Integer id) const
{
   if (id == EPHEM_STEP_SIZE)
   {
      return ephemStep;
   }

   if (id == EPHEM_START_A1MJD)
   {
      return ephemStart;
   }

   if (id == EPHEM_END_A1MJD)
   {
      return ephemEnd;
   }
   return Propagator::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the Real parameters specific to the EphemerisPropagator
 *
 * @param id The ID for the parameter that is retrieved
 * @param value The new value for the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_SET_PROPAGATOR_PARAMETER
   	MessageInterface::ShowMessage("EphemProp %p setting ID %d to %lf\n", this,
      	   id, value);
   #endif
   if (id == EPHEM_STEP_SIZE)
   {
      if (value != 0.0)
         ephemStep = value;
      return ephemStep;
   }

   if (id == INITIAL_STEP_SIZE)
   {
//      if (value != 0.0)
//         ephemStep = value;

      if (value < 0.0)
         stepDirection = -1.0;
      else
         stepDirection = 1.0;
      // Let this fall through to Propagator, so no return here
   }

   #ifdef DEBUG_SET_PROPAGATOR_PARAMETER
      MessageInterface::ShowMessage("   ---> <%p> has ephem step %lf\n", this, ephemStep);
   #endif
   
   return Propagator::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves parameters specific to the EphemerisPropagator from a RealArray
 *
 * @param id The ID for the parameter that is retrieved
 * @param index The index of the parameter in the RealArray
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const Integer id,
      const Integer index) const
{
   return Propagator::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * Retrieves parameters specific to the EphemerisPropagator from a Real matrix
 *
 * @param id The ID for the parameter that is retrieved
 * @param row The row index of the parameter in the matrix
 * @param col The column index of the parameter in the matrix
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const Integer id, const Integer row,
      const Integer col) const
{
   return Propagator::GetRealParameter(id, row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves parameters specific to the EphemerisPropagator from a RealArray
 *
 * @param id The ID for the parameter that is retrieved
 * @param value The new value for the parameter
 * @param index The index of the parameter in the RealArray
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const Integer id, const Real value,
      const Integer index)
{
   return Propagator::SetRealParameter(id, value, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer row,
//       const Integer col)
//------------------------------------------------------------------------------
/**
 * Retrieves parameters specific to the EphemerisPropagator from a RealArray
 *
 * @param id The ID for the parameter that is retrieved
 * @param value The new value for the parameter
 * @param row The row index of the parameter in the matrix
 * @param col The column index of the parameter in the matrix
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const Integer id, const Real value,
      const Integer row, const Integer col)
{
   return Propagator::SetRealParameter(id, value, row, col);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real parameter from then object
 *
 * @param label Script identifier for the parameter
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Real parameter
 *
 * @param label Script identifier for the parameter
 * @param value The new value
 *
 * @return The parameter's value at the end of the call
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const std::string &label,
      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real parameter value from a RealArray
 *
 * @param label Script identifier for the parameter
 * @param index The index of the parameter in the array
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const std::string &label,
      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real EphemerisPropagator::SetRealParameter(const std::string &label,
//       const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a Real parameter value in a RealArray
 *
 * @param label Script identifier for the parameter
 * @param value The new value
 * @param index The index of the parameter in the array
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const std::string &label,
      const Real value, const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real parameter value from a matrix of Reals
 *
 * @param label Script identifier for the parameter
 * @param row The row index of the parameter in the matrix
 * @param col The column index of the parameter in the matrix
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetRealParameter(const std::string &label,
      const Integer row, const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value,
//       const Integer row, const Integer col))
//------------------------------------------------------------------------------
/**
 * Sets a Real parameter value in a matrix of Reals
 *
 * @param label Script identifier for the parameter
 * @param value The new value
 * @param row The row index of the parameter in the matrix
 * @param col The column index of the parameter in the matrix
 *
 * @return The parameter's value
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::SetRealParameter(const std::string &label,
      const Real value, const Integer row, const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves string parameters
 *
 * @param id The id for the parameter
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string  EphemerisPropagator::GetStringParameter(const Integer id) const
{
   switch (id)
   {
      case EPHEM_CENTRAL_BODY:
         return centralBody;

      case EPHEM_EPOCH_FORMAT:
         return epochFormat;

      case EPHEM_START_EPOCH:
         return startEpoch;

      default:
         break;
   }

   return Propagator::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a parameter
 *
 * @param id The ID of the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetStringParameter(const Integer id,
      const std::string &value)
{
   switch (id)
   {
      case EPHEM_CENTRAL_BODY:
         //Turned off functionality to set central bodies in Ephems. 
         //Still returns true to ensure back functionality.
         //centralBody = value;
         return true;

      case EPHEM_EPOCH_FORMAT:
         if (theTimeConverter->IsValidTimeSystem(value))
         {
            epochFormat = value;   
         }
         else
         {
            char msg[512];
            std::string timeRepList;
            StringArray validReps = theTimeConverter->GetValidTimeRepresentations();
            for (UnsignedInt i = 0; i < validReps.size(); ++i)
            {
               if (i != 0)
                  timeRepList += ", ";
               timeRepList += validReps[i];
            }

            // The valid format list here should be retrieved from TimeSystemConverter,
            // once that code is refactored
            std::sprintf(msg, errorMessageFormat.c_str(), value.c_str(),
                  PARAMETER_TEXT[EPHEM_EPOCH_FORMAT - PropagatorParamCount].c_str(),
                  timeRepList.c_str());

            throw PropagatorException(msg);
         }
         return true;

      case EPHEM_START_EPOCH:
         startEpoch = value; 
         if (startEpoch == "FromSpacecraft")
            startEpochSource = FROM_SPACECRAFT;
         else if (startEpoch == "EphemStart")
            startEpochSource = FROM_EPHEM;
         else
            startEpochSource = FROM_SCRIPT;
         return true;

      default:
         break;
   }

   return Propagator::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves string parameters from a StringArray
 *
 * @param id The id for the parameter
 * @param index Index into the array
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string EphemerisPropagator::GetStringParameter(const Integer id,
      const Integer index) const
{
   return Propagator::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets string parameters in a StringArray
 *
 * @param id The id for the parameter
 * @param value The new string value
 * @param index Index into the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   return Propagator::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves string parameters
 *
 * @param label The script name for the parameter
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string EphemerisPropagator::GetStringParameter(
      const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a parameter
 *
 * @param label The script name for the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves string parameters from a StringArray
 *
 * @param label The script name for the parameter
 * @param index Index into the array
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string EphemerisPropagator::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets string parameters in a StringArray
 *
 * @param label The script name for the parameter
 * @param value The new string value
 * @param index Index into the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accesses StringArray parameters
 *
 * @param id The ID of the parameter
 *
 * @return The corresponding StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EphemerisPropagator::GetStringArrayParameter(
      const Integer id) const
{
   if (id == EPHEM_START_OPTIONS)
   {
      return startOptions;
   }
   return Propagator::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Accesses StringArray parameters from a vector of StringArrays
 *
 * @param id The ID of the parameter
 * @param index The index of the array in the vector of arrays
 *
 * @return The corresponding StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EphemerisPropagator::GetStringArrayParameter(
      const Integer id, const Integer index) const
{
   return Propagator::GetStringArrayParameter(id, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accesses StringArray parameters from a vector of StringArrays
 *
 * @param label The text label of the parameter
 *
 * @return The corresponding StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EphemerisPropagator::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Accesses StringArray parameters from a vector of StringArrays
 *
 * @param label The text label of the parameter
 * @param index The index of the array in the vector of arrays
 *
 * @return The corresponding StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EphemerisPropagator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}

// Reference object code

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 *
 * @param ss Pointer to the solar system used in the modeling.
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::SetSolarSystem(SolarSystem *ss)
{
   if (ss == NULL)
      MessageInterface::ShowMessage("Setting NULL solar system on %s\n",
         instanceName.c_str());
   else
   {
      solarSystem = ss;

      // "Moon" no longer allowed
//      std::string bName = centralBody;
//      if (bName == "Moon")
//         bName = "Luna";
      propOrigin = solarSystem->GetBody(centralBody);

      if (propOrigin == NULL)
         throw PropagatorException(
            "Ephemeris propagator origin (" + centralBody +
            ") was not found in the solar system");

      // "Moon" no longer allowed
//      bName = j2kBodyName;
//      if (bName == "Moon")
//         bName = "Luna";
      j2kBody = solarSystem->GetBody(j2kBodyName);
   }
}


//------------------------------------------------------------------------------
// std::string  GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a reference object of the specified type
 *
 * @param type The type of the object
 *
 * @return The object's name
 */
//------------------------------------------------------------------------------
std::string  EphemerisPropagator::GetRefObjectName(
      const UnsignedInt type) const
{
      return Propagator::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of reference objects of the specified type
 *
 * @param type The type of the objects
 *
 * @return The list of names
 */
//------------------------------------------------------------------------------
const StringArray& EphemerisPropagator::GetRefObjectNameArray(
      const UnsignedInt type)
{
   #ifdef DEBUG_REF_OBJ
      MessageInterface::ShowMessage("EphemerisPropagator::"
            "GetRefObjectNameArray(%d) called\n", type);
   #endif

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::SPACEOBJECT) ||
       (type == Gmat::FORMATION))
      return propObjectNames;

   return Propagator::GetRefObjectNameArray(type);
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of reference objects of the specified type
 *
 * @param type The type of the objects
 *
 * @return The object array
 */
 //------------------------------------------------------------------------------
ObjectArray& EphemerisPropagator::GetRefObjectArray(const UnsignedInt type)
{
#ifdef DEBUG_REF_OBJ
   MessageInterface::ShowMessage("EphemerisPropagator::"
      "GetRefObjectArray(%d) called\n", type);
#endif

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::SPACEOBJECT) ||
      (type == Gmat::FORMATION))
      return propObjects;

   return Propagator::GetRefObjectArray(type);
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectArray(const std::string& typeString)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of reference objects of the specified type
 *
 * @param typeString The name of the object to retreive
 *
 * @return The object array
 */
 //------------------------------------------------------------------------------
ObjectArray& EphemerisPropagator::GetRefObjectArray(const std::string& typeString)
{
#ifdef DEBUG_REF_OBJ
   MessageInterface::ShowMessage("EphemerisPropagator::"
      "GetRefObjectArray(%d) called\n", type);
#endif
   if (typeString == "PropObjects")
      return propObjects;

   return Propagator::GetRefObjectArray(typeString);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Passes in the name of a reference object for use during initialization
 *
 * @param type The type of the object
 * @param name The object's name
 *
 * @return true if the name was set, false if not
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetRefObjectName(const UnsignedInt type,
      const std::string &name)
{
   bool retval = false;

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::SPACEOBJECT) ||
       (type == Gmat::FORMATION))
   {
      if (find(propObjectNames.begin(), propObjectNames.end(), name) ==
            propObjectNames.end())
         propObjectNames.push_back(name);

      retval = true;
   }

   return (Propagator::SetRefObjectName(type, name) || retval);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//       const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Changes the name of a previously set reference object.
 *
 * This method changes the name or list of names of reference objects.  It does
 * not change the actual objects themselves; that side of the renaming is
 * handled separately.
 *
 * @param type The type of object that is being renamed
 * @param oldName The object's name before the change
 * @param newName The name that now identifies the object
 *
 * @return true if a object name was changed, false if not
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::RenameRefObject(const UnsignedInt type,
      const std::string &oldName, const std::string &newName)
{
   bool retval = false;

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::SPACEOBJECT) ||
       (type == Gmat::FORMATION))
   {
      for (UnsignedInt i = 0; i < propObjectNames.size(); ++i)
         if (propObjectNames[i] == oldName)
            propObjectNames[i] = newName;
      retval = true;
   }

   return (Propagator::RenameRefObject(type, oldName, newName) || retval);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string &name)
//------------------------------------------------------------------------------
/**
 * Passes reference objects in to the propagator
 *
 * @param obj The object
 * @param type The object's type
 * @param name The object's name
 *
 * @return true if the object was stored, otherwise false
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetRefObject(GmatBase *obj,
      const UnsignedInt type, const std::string &name)
{
   #ifdef DEBUG_REF_OBJ
         MessageInterface::ShowMessage
               ("EphemerisPropagator::SetRefObject() <%s> entered, obj=<%p><%s>"
                "<%s>, type=%d, name='%s'\n", GetName().c_str(), obj,
                (obj ? obj->GetTypeName().c_str() : "NULL"),
                (obj ? obj->GetName().c_str() : "NULL"), type, name.c_str());
   #endif

   bool retval = false;

   if (obj->IsOfType(Gmat::SPACEOBJECT))
   {
      for (UnsignedInt i = 0; i < propObjectNames.size(); ++i)
         if (propObjectNames[i] == name)
         {
            propObjects[i] = obj;
            retval = true;
         }
      if (retval == false)
      {
         propObjectNames.push_back(name);
         propObjects.push_back(obj);
      }
      return true;
   }

   return (Propagator::SetRefObject(obj, type, name) || retval);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Passes reference objects in to the propagator for use in an object array
 *
 * @param obj The object
 * @param type The object's type
 * @param name The object's name
 * @param index Index of the object in the array
 *
 * @return true if the object was stored, otherwise false
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::SetRefObject(GmatBase *obj,
      const UnsignedInt type, const std::string &name, const Integer index)
{
   bool retval = false;

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting object named %s at index %d\n",
            name.c_str(), index);
   #endif

   if (obj->IsOfType(Gmat::SPACEOBJECT))
   {
      if (propObjectNames[index] == name)
      {
         propObjects[index] = obj;
         retval = true;
      }
   }

   return (Propagator::SetRefObject(obj, type, name, index) || retval);
}


//---------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//---------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * The Ephem propagators use this method to toggle into a mode that lets a
 * script continue to run even when the requested epoch is outside of the span
 * of an ephem.  This lets Space Network (TDRS participant) measurement models
 * process data that may indicate an epoch off the span, basically by omitting
 * measurements at those epochs.
 *
 * @param action The string descriptor for the requested action.
 * @param actionData Optional data used for the action.
 *
 * @return true if the action was performed, false if not.
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::TakeAction(const std::string &action,
      const std::string &actionData)
{
   bool retval = false;

   if (action == "RunPastStartEnd")
   {
      stopOutsideOfEphem = false;
   }
   else
      retval =  Propagator::TakeAction(action, actionData);

   return retval;
}

//------------------------------------------------------------------------------
// bool UsesODEModel()
//------------------------------------------------------------------------------
/**
 * Used to tell the PropSetup if an ODE model is needed for the propagator
 *
 * @return true if an ODEModel is required, false if not
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::UsesODEModel()
{
   return false;
}


//------------------------------------------------------------------------------
// void SetPropStateManager(PropagationStateManager *sm)
//------------------------------------------------------------------------------
/**
 * Sets the PSM for ephemeris based propagators
 *
 * @param sm The propagation state manager
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::SetPropStateManager(PropagationStateManager *sm)
{
   psm = sm;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the EphemeridPropagator for use during a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::Initialize()
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("EphemerisPropagator::Initialize() entered\n");
#endif

   bool retval = false;

   Integer perSatDim = 6;

   if (psm)
      perSatDim = psm->GetState()->GetSize();

   #ifdef DEBUG_EXTENDED_STATES
      MessageInterface::ShowMessage("perSatDim = %d\n", perSatDim);
   #endif

   Integer oldDim = dimension;
   dimension = perSatDim * propObjects.size();
   if (dimension > 0)
   {
      if (state != NULL)
         delete [] state;
      state = new Real[dimension];

      if ((j2kState != NULL) && (oldDim != dimension))
      {
         delete [] j2kState;
         j2kState = NULL;
      }

      if (j2kState == NULL)
         j2kState = new Real[dimension];
   }

   //for (UnsignedInt i = 0; i < dimension; ++i)
   //   state[i] = j2kState[i] = 0.0;

   if (Propagator::Initialize())
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Ephemeris has %spropagated\n",
               (((SpaceObject*)(propObjects[0]))->HasEphemPropagated()) ? "" : "not ");
         MessageInterface::ShowMessage("State epoch %s from spacecraft\n",
               ((startEpochSource == FROM_SPACECRAFT) ? "is" : "is not"));
         MessageInterface::ShowMessage("State epoch %s from ephemeris\n",
               ((startEpochSource == FROM_EPHEM) ? "is" : "is not"));
         MessageInterface::ShowMessage("State epoch %s from script\n",
               ((startEpochSource == FROM_SCRIPT) ? "is" : "is not"));
      #endif
      switch (startEpochSource)
      {
         case FROM_SPACECRAFT:
            if (propObjects.size() > 0)
            {
               // Spacecraft update epochs during the run, so we have to update
               // timeFromEpoch for this case
               
               
               
               initialEpoch = ((SpaceObject*)(propObjects[0]))->GetEpoch();
               if (hasPrecisionTime)
                  initialEpochGT = ((SpaceObject*)(propObjects[0]))->GetEpochGT();

               if (ephemStart > 0)
               {
                  if (ephemStart > initialEpoch)
                  {
                     initialEpoch = ephemStart;
                     if (hasPrecisionTime)
                        initialEpochGT = GmatTime(ephemStart);
                  }
               }
               else
               {
                  initialEphemEpochSet = false;
               }
               timeFromEpoch = 0.0;
               #ifdef DEBUG_INITIALIZATION
                  if (hasPrecisionTime)
                     MessageInterface::ShowMessage("From spacecraft initialEpochGT = %s, "
                        "elapsed time = %lf\n", initialEpochGT.ToString().c_str(), timeFromEpoch);
                  else
                     MessageInterface::ShowMessage("From spacecraft epoch = %lf, "
                        "elapsed time = %lf\n", initialEpoch, timeFromEpoch);
               #endif

               #ifdef DEBUG_INITIAL_EPHEM_STATE
                  MessageInterface::ShowMessage("From spacecraft\n");
               #endif

               Rvector6 state;
               if (hasPrecisionTime)
                  state = ((SpaceObject*)(propObjects[0]))->GetMJ2000State(initialEpochGT);
               else
                  state = ((SpaceObject*)(propObjects[0]))->GetMJ2000State(initialEpoch);

               for (UnsignedInt q = 0; q < 6; ++q)
                  j2kState[q] = state[q];
            }
            break;

         case FROM_EPHEM:
         default:
            if (((SpaceObject*)(propObjects[0]))->HasEphemPropagated())
            {
               initialEpoch = ((SpaceObject*)(propObjects[0]))->GetEpoch();
               if (hasPrecisionTime)
                  initialEpochGT = ((SpaceObject*)(propObjects[0]))->GetEpochGT();
               timeFromEpoch = 0.0;
            }
            else
            {
               if (ephemStart > 0)
               {
                  initialEpoch = ephemStart;
                  if (hasPrecisionTime)
                     initialEpochGT = GmatTime(ephemStart);

#ifdef DEBUG_INITIAL_EPHEM_STATE
                  MessageInterface::ShowMessage("From Ephem\n");
#endif
               }
               else
               {
                  initialEphemEpochSet = false;
               }
            }
            break;


         case FROM_SCRIPT:
            if (((SpaceObject*)(propObjects[0]))->HasEphemPropagated())
            {
               initialEpoch = ((SpaceObject*)(propObjects[0]))->GetEpoch();
               if (hasPrecisionTime)
                  initialEpochGT = ((SpaceObject*)(propObjects[0]))->GetEpoch();
               timeFromEpoch = 0.0;
            }
            else
            {
               initialEpoch = ConvertToRealEpoch(startEpoch, epochFormat);
               if (hasPrecisionTime)
                  initialEpochGT = initialEpoch;
               #ifdef DEBUG_INITIAL_EPHEM_STATE
               MessageInterface::ShowMessage("From Script\n");
               #endif
            }
            break;
      }

      if (currentEpoch == -987654321.0)
      {
         currentEpoch = initialEpoch;
      }

      if (hasPrecisionTime)
      {
         if (currentEpochGT == GmatTime(-987654321.0))
            currentEpochGT = initialEpochGT;
      }

#ifdef DEBUG_INITIALIZATION
      if (hasPrecisionTime)
      {
         MessageInterface::ShowMessage("Initial epoch set to %s\n",
            initialEpochGT.ToString().c_str());
         MessageInterface::ShowMessage("Current epoch set to %s\n",
            currentEpochGT.ToString().c_str());
      }
      else
      {
         MessageInterface::ShowMessage("Initial epoch set to %.12lf\n",
            initialEpoch);
         MessageInterface::ShowMessage("Current epoch set to %.12lf\n",
            currentEpoch);
      }
      #endif

      // Set direction for the default step.  Step(Real) overrides the direction
      ephemStep = fabs(ephemStep) * stepDirection;

      isInitialized = true;

      retval = true;
   }
   else
   {
      isInitialized = false;
      retval = false;
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("EphemerisPropagator::Initialize() end\n");
#endif
   return retval;
}


//------------------------------------------------------------------------------
// bool Step(Real dt)
//------------------------------------------------------------------------------
/**
 * Advances the state vector by timestep dt
 *
 * @param dt The time step, in seconds
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EphemerisPropagator::Step(Real dt)
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Stepping by %.12lf\n", dt);
   #endif

   bool retval = false;

   if (isInitialized)
   {
      Real tempStep = ephemStep;
      ephemStep = dt;
      retval = Step();
      if (retval)
         ((SpaceObject*)(propObjects[0]))->HasEphemPropagated(true);
      ephemStep = tempStep;
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer GetDimension()
//------------------------------------------------------------------------------
/**
 * Retrieves the size of the state vector that gets propagated
 *
 * @return The vector size
 */
//------------------------------------------------------------------------------
Integer EphemerisPropagator::GetDimension()
{
   if (dimension == 0)
   {
      Integer perSatDim = (psm ? psm->GetState()->GetSize() : 6);
      dimension = perSatDim * propObjects.size();
   }
   return dimension;
}


//------------------------------------------------------------------------------
// Real* GetState()
//------------------------------------------------------------------------------
/**
 * Retrieves the Real state that gets propagated
 *
 * @return The state
 */
//------------------------------------------------------------------------------
Real* EphemerisPropagator::GetState()
{
   return state;
}


//------------------------------------------------------------------------------
// Real* GetJ2KState()
//------------------------------------------------------------------------------
/**
 * Retrieves the Real state that gets propagated in the J2000 j2kBody reference
 * frame
 *
 * @return The state
 */
//------------------------------------------------------------------------------
Real* EphemerisPropagator::GetJ2KState()
{
   return j2kState;
}


//------------------------------------------------------------------------------
// void UpdateSpaceObject(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Passes state data from propagator to the objects that are being propagated
 *
 * @param newEpoch The epoch of the state; use -1.0 to leave the epoch unchanged
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::UpdateSpaceObject(Real newEpoch)
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage(
            "EphemerisPropagator::UpdateSpaceObject(%.12lf) called\n", newEpoch);
   #endif

   if (psm)
   {
      Integer stateSize;
      Integer vectorSize;
      GmatState *newState;
      ReturnFromOrigin(newEpoch);

      newState = psm->GetState();
      stateSize = newState->GetSize();
      vectorSize = stateSize * sizeof(Real);

      currentEpoch = initialEpoch + timeFromEpoch / GmatTimeConstants::SECS_PER_DAY;

      // Update the epoch if it was passed in
      if (newEpoch != -1.0)
      {
         currentEpoch = newEpoch;
         timeFromEpoch = (currentEpoch-initialEpoch) * GmatTimeConstants::SECS_PER_DAY;
      }
      UpdateState();

      memcpy(newState->GetState(), j2kState, vectorSize);
      newState->SetEpoch(currentEpoch);
      //newState->SetEpochGT(currentEpochGT);

      psm->MapVectorToObjects();

//      // Formation code: Not part of initial release
//      // Update elements for each Formation
//      for (UnsignedInt i = 0; i < stateObjects.size(); ++i)
//         if (stateObjects[i]->IsOfType(Gmat::FORMATION))
//            ((Formation*)stateObjects[i])->UpdateElements();

      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage
               ("EphemerisPropagator::UpdateSpaceObject() on \"%s\" "
                "currentEpoch = %f, passed in epoch = %f\n", GetName().c_str(),
                currentEpoch, newEpoch);
      #endif
   }
}


void EphemerisPropagator::UpdateSpaceObjectGT(GmatTime newEpoch)
{
#ifdef DEBUG_EXECUTION
   MessageInterface::ShowMessage(
      "EphemerisPropagator::UpdateSpaceObjectGT(%s) called\n", newEpoch.ToString().c_str());
#endif

   if (psm)
   {
      Integer stateSize;
      Integer vectorSize;
      GmatState *newState;
      //ReturnFromOriginGT(newEpoch);

      newState = psm->GetState();
      stateSize = newState->GetSize();
      vectorSize = stateSize * sizeof(Real);

      currentEpochGT = initialEpochGT;
      currentEpochGT.AddSeconds(timeFromEpoch);

      // Update the epoch if it was passed in
      if (newEpoch != -1.0)
      {
         currentEpochGT = newEpoch;
         timeFromEpoch = (currentEpochGT - initialEpochGT).GetTimeInSec();
      }
      UpdateState();
      ReturnFromOriginGT(newEpoch);

      memcpy(newState->GetState(), j2kState, vectorSize);
      newState->SetEpochGT(currentEpochGT);
      psm->MapVectorToObjects();

      //      // Formation code: Not part of initial release
      //      // Update elements for each Formation
      //      for (UnsignedInt i = 0; i < stateObjects.size(); ++i)
      //         if (stateObjects[i]->IsOfType(Gmat::FORMATION))
      //            ((Formation*)stateObjects[i])->UpdateElements();

#ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage
         ("EphemerisPropagator::UpdateSpaceObjectGT() on \"%s\" "
         "currentEpochGT = %s, passed in epoch = %s\n", GetName().c_str(),
         currentEpochGT.ToString().c_str(), newEpoch.ToString().c_str());
#endif
   }
}


//------------------------------------------------------------------------------
// void UpdateFromSpaceObject()
//------------------------------------------------------------------------------
/**
 * Fills the state vector with data from the objects that are being propagated
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::UpdateFromSpaceObject()
{
//   // Formation code: Not part of initial release
//   // Update elements for each Formation
//   for (UnsignedInt i = 0; i < stateObjects.size(); ++i)
//      if (stateObjects[i]->IsOfType(Gmat::FORMATION))
//         ((Formation*)stateObjects[i])->UpdateState();

   psm->MapObjectsToVector();
   GmatState *newState = psm->GetState();
   memcpy(j2kState, newState->GetState(), newState->GetSize() * sizeof(Real));
   

    // Transform to the force model origin
   if (HasPrecisionTime())
   {
      currentEpochGT = newState->GetEpochGT();
      currentEpoch = newState->GetEpoch();
      MoveToOriginGT();
   }
   else
   {
      currentEpoch = newState->GetEpoch();
      currentEpochGT = currentEpoch;
      MoveToOrigin();
   }
}


//------------------------------------------------------------------------------
// void RevertSpaceObject()
//------------------------------------------------------------------------------
/**
 * Restores SpaceObjects to the states stored in a buffer
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::RevertSpaceObject()
{
   #ifdef DEBUG_ODEMODEL_EXE
      MessageInterface::ShowMessage
         ("ODEModel::RevertSpacecraft() prevElapsedTime=%f elapsedTime=%f\n",
          prevElapsedTime, elapsedTime);
   #endif
   
   if (HasPrecisionTime())
   {
      timeFromEpoch = (previousState.GetEpochGT() - initialEpochGT).GetTimeInSec();
      currentEpochGT = initialEpochGT; currentEpochGT.AddSeconds(timeFromEpoch);
      UpdateState();

      //MoveToOriginGT();
      ReturnFromOriginGT(); // J.McGreevy's version upload by Darrel on 10/12/2017
   }
   else
   {                 
      timeFromEpoch = (previousState.GetEpoch() - initialEpoch) * GmatTimeConstants::SECS_PER_DAY;
      currentEpoch = initialEpoch + timeFromEpoch / GmatTimeConstants::SECS_PER_DAY;
      UpdateState();

      // MoveToOrigin();
      ReturnFromOrigin();   // J.McGreevy's version upload by Darrel on 10/12/2017
   }
}


//------------------------------------------------------------------------------
// void BufferState()
//------------------------------------------------------------------------------
/**
 * Buffers SpaceObjects for later restoration
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::BufferState()
{
   GmatState *stateToBuffer = psm->GetState();
   previousState = (*stateToBuffer);
}


//------------------------------------------------------------------------------
// Real GetTime()
//------------------------------------------------------------------------------
/**
 * Retrieves the time elapsed in the state from the propagator's base epoch
 *
 * @return The elapsed time
 */
//------------------------------------------------------------------------------
Real EphemerisPropagator::GetTime()
{
   return timeFromEpoch;
}


//------------------------------------------------------------------------------
// void SetTime(Real t)
//------------------------------------------------------------------------------
/**
 * Sets the time elapsed in the state from the propagator's base epoch
 *
 * @param t The elapsed time
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::SetTime(Real t)
{
   timeFromEpoch = t;
}


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Real ConvertToRealEpoch(const std::string &theEpoch,
//                         const std::string &theFormat)
//------------------------------------------------------------------------------
/**
 * Converts an epoch string is a specified format into
 *
 * @param theEpoch The input epoch
 * @param theFormat The format of the input epoch
 *
 * @return
 */
//------------------------------------------------------------------------------
GmatEpoch EphemerisPropagator::ConvertToRealEpoch(const std::string &theEpoch,
                                   const std::string &theFormat)
{
   Real fromMjd = -999.999;
   Real retval = -999.999;
   std::string outStr;

   theTimeConverter->Convert(theFormat, fromMjd, theEpoch, "A1ModJulian",
         retval, outStr);

   if (retval == -999.999)
      throw PropagatorException("Error converting the time string \"" +
            theEpoch + "\"; please check the format for the input string.");
   return retval;
}


//------------------------------------------------------------------------------
// Real* GetAnalyticState()
//------------------------------------------------------------------------------
/**
 * State access method for ephemeric propagators
 *
 * @return The state pointer
 */
//------------------------------------------------------------------------------
Real* EphemerisPropagator::GetAnalyticState()
{
   return j2kState;
}


//------------------------------------------------------------------------------
// void SetEphemSpan(const GmatEpoch start, const GmatEpoch end)
//------------------------------------------------------------------------------
/**
 * Sets the ephem start and end epochs
 *
 * @param start The start epoch, in the A.1 modified Julian system
 * @param end The end epoch, in the A.1 modified Julian system
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::SetEphemSpan(const GmatEpoch start, const GmatEpoch end)
{
   if (end <= start)
      throw PropagatorException("The ephemeris propagator " + instanceName +
            " was passed an invalid span in the call to "
            "EphemerisPropagator::SetEphemSpan(const GmatEpoch start, "
            "const GmatEpoch end): start >= end");
   ephemStart = start;
   ephemEnd   = end;
}


//------------------------------------------------------------------------------
// void SetEphemSpan(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Sets the initial epochs for a given ephem if they have not been set
 *
 * @param whichOne The index of the ephem being processed
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::SetEphemSpan(Integer whichOne)
{
	if (!initialEphemEpochSet)
	{
      if (startEpochSource == FROM_SPACECRAFT)
      {
         if (initialEpoch > ephemStart)
         {
            initialEphemEpochSet = true;
            return;
         }
      }
		initialEpoch = ephemStart;
      if (hasPrecisionTime)
         initialEpochGT = GmatTime(ephemStart);

		initialEphemEpochSet = true;
		if (currentEpoch == -987654321.0)
		{
			currentEpoch = initialEpoch;
		}

      if (hasPrecisionTime)
      {
		   if (currentEpochGT == GmatTime(-987654321.0))
		   {
			  currentEpochGT = initialEpochGT;
		   }
      }
		
	}
}


//------------------------------------------------------------------------------
// void MoveToOrigin(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Provides the interface that Propagators that do not use ODEModels use to
 * translate from the J2000 body to the propagator's central body
 *
 * @param newEpoch The epoch of the state that is translated
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::MoveToOrigin(Real newEpoch)
{
   #ifdef DEBUG_REORIGIN
      MessageInterface::ShowMessage("ODEModel::MoveToOrigin entered\n");
   #endif

   #ifdef DEBUG_REORIGIN
      MessageInterface::ShowMessage(
            "SatCount = %d, dimension = %d, stateSize = %d\n",cartObjCount,
            dimension, stateSize);
      MessageInterface::ShowMessage(
            "StatePointers: rawState = %p, modelState = %p\n", rawState,
            modelState);
      MessageInterface::ShowMessage(
          "ODEModel::MoveToOrigin()\n   Input state: [ ");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%lf ", rawState[i]);
      MessageInterface::ShowMessage("]\n   model state: [ ");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%lf ", modelState[i]);
      MessageInterface::ShowMessage("]\n\n");
   #endif

   if (state != j2kState)
      memcpy(state, j2kState, dimension*sizeof(Real));

   if (centralBody != j2kBodyName)
   {
      Rvector6 cbState, mj2kState, delta;
      Real now = ((newEpoch < 0.0) ? currentEpoch : newEpoch);
      cbState = propOrigin->GetMJ2000State(now);
      mj2kState = j2kBody->GetState(now);

      delta = cbState - mj2kState;

      /**
       *  @todo When multiple spacecraft are supported, these items will need
       *  to be set to match the PSM's state vector
       */
      Integer cartObjCount = 1;
      Integer cartStateStart = 0;

      for (Integer i = 0; i < cartObjCount; ++i)
      {
         Integer i6 = cartStateStart + i * 6;
         for (int j = 0; j < 6; ++j)
            state[i6+j] -= delta[j];

         #ifdef DEBUG_REORIGIN
            MessageInterface::ShowMessage(
                "ODEModel::MoveToOrigin()\n"
                "   Input state: [%lf %lf %lf %lf %lf %lf]\n"
                "   j2k state:   [%lf %lf %lf %lf %lf %lf]\n"
                "   cb state:    [%lf %lf %lf %lf %lf %lf]\n"
                "   delta:       [%lf %lf %lf %lf %lf %lf]\n"
                "   model state: [%lf %lf %lf %lf %lf %lf]\n\n",
                rawState[i6], rawState[i6+1], rawState[i6+2], rawState[i6+3],
                rawState[i6+4], rawState[i6+5],
                mj2kState[0], mj2kState[1], mj2kState[2], mj2kState[3],
                mj2kState[4], mj2kState[5],
                cbState[0], cbState[1], cbState[2], cbState[3], cbState[4],
                cbState[5],
                delta[0], delta[1], delta[2], delta[3], delta[4], delta[5],
                modelState[i6], modelState[i6+1], modelState[i6+2],
                modelState[i6+3], modelState[i6+4], modelState[i6+5]);
         #endif
      }
   }

   #ifdef DEBUG_REORIGIN
      MessageInterface::ShowMessage(
          "   Move Complete\n   Input state: [ ");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%lf ", rawState[i]);
      MessageInterface::ShowMessage("]\n   model state: [ ");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%lf ", modelState[i]);
      MessageInterface::ShowMessage("]\n\n");

      MessageInterface::ShowMessage("ODEModel::MoveToOrigin Finished\n");
   #endif
}


void EphemerisPropagator::MoveToOriginGT(GmatTime newEpoch)
{
#ifdef DEBUG_REORIGIN
   MessageInterface::ShowMessage("ODEModel::MoveToOriginGT entered\n");
#endif

#ifdef DEBUG_REORIGIN
   MessageInterface::ShowMessage(
      "SatCount = %d, dimension = %d, stateSize = %d\n", cartObjCount,
      dimension, stateSize);
   MessageInterface::ShowMessage(
      "StatePointers: rawState = %p, modelState = %p\n", rawState,
      modelState);
   MessageInterface::ShowMessage(
      "ODEModel::MoveToOriginGT()\n   Input state: [ ");
   for (Integer i = 0; i < dimension; ++i)
      MessageInterface::ShowMessage("%lf ", rawState[i]);
   MessageInterface::ShowMessage("]\n   model state: [ ");
   for (Integer i = 0; i < dimension; ++i)
      MessageInterface::ShowMessage("%lf ", modelState[i]);
   MessageInterface::ShowMessage("]\n\n");
#endif

   if (state != j2kState)
      memcpy(state, j2kState, dimension*sizeof(Real));

   if (centralBody != j2kBodyName)
   {
      Rvector6 cbState, mj2kState, delta;
      GmatTime now = ((newEpoch < 0.0) ? currentEpoch : newEpoch);
      cbState = propOrigin->GetMJ2000State(now);
      mj2kState = j2kBody->GetState(now);

      delta = cbState - mj2kState;

      /**
      *  @todo When multiple spacecraft are supported, these items will need
      *  to be set to match the PSM's state vector
      */
      Integer cartObjCount = 1;
      Integer cartStateStart = 0;

      for (Integer i = 0; i < cartObjCount; ++i)
      {
         Integer i6 = cartStateStart + i * 6;
         for (int j = 0; j < 6; ++j)
            state[i6 + j] -= delta[j];

#ifdef DEBUG_REORIGIN
         MessageInterface::ShowMessage(
            "ODEModel::MoveToOriginGT()\n"
            "   Input state: [%lf %lf %lf %lf %lf %lf]\n"
            "   j2k state:   [%lf %lf %lf %lf %lf %lf]\n"
            "   cb state:    [%lf %lf %lf %lf %lf %lf]\n"
            "   delta:       [%lf %lf %lf %lf %lf %lf]\n"
            "   model state: [%lf %lf %lf %lf %lf %lf]\n\n",
            rawState[i6], rawState[i6 + 1], rawState[i6 + 2], rawState[i6 + 3],
            rawState[i6 + 4], rawState[i6 + 5],
            mj2kState[0], mj2kState[1], mj2kState[2], mj2kState[3],
            mj2kState[4], mj2kState[5],
            cbState[0], cbState[1], cbState[2], cbState[3], cbState[4],
            cbState[5],
            delta[0], delta[1], delta[2], delta[3], delta[4], delta[5],
            modelState[i6], modelState[i6 + 1], modelState[i6 + 2],
            modelState[i6 + 3], modelState[i6 + 4], modelState[i6 + 5]);
#endif
      }
   }

#ifdef DEBUG_REORIGIN
   MessageInterface::ShowMessage(
      "   Move Complete\n   Input state: [ ");
   for (Integer i = 0; i < dimension; ++i)
      MessageInterface::ShowMessage("%lf ", rawState[i]);
   MessageInterface::ShowMessage("]\n   model state: [ ");
   for (Integer i = 0; i < dimension; ++i)
      MessageInterface::ShowMessage("%lf ", modelState[i]);
   MessageInterface::ShowMessage("]\n\n");

   MessageInterface::ShowMessage("ODEModel::MoveToOriginGT Finished\n");
#endif
}


//------------------------------------------------------------------------------
// void ReturnFromOrigin(Real newEpoch)
//------------------------------------------------------------------------------
/**
 * Provides the interface that Propagators that do not use ODEModels use to
 * translate from the propagator's central body to the J2000 body
 *
 * @param newEpoch The epoch of the state that is translated
 */
//------------------------------------------------------------------------------
void EphemerisPropagator::ReturnFromOrigin(Real newEpoch)
{
   if ((j2kBody == NULL) || (propOrigin == NULL))
   {
      MessageInterface::ShowMessage("Cannot reorigin; j2kBody = %p, "
            "propOrigin = %p\n", j2kBody, propOrigin);
      return;
   }
   #ifdef DEBUG_REORIGIN
      MessageInterface::ShowMessage("ODEModel::ReturnFromOrigin entered\n");
   #endif

   if (state != j2kState)
      memcpy(j2kState, state, dimension*sizeof(Real));
   if (centralBody != j2kBodyName)
   {
      Rvector6 cbState, jkState, delta;
      Real now = ((newEpoch < 0.0) ? currentEpoch : newEpoch);
      cbState = propOrigin->GetMJ2000State(now);
      jkState = j2kBody->GetState(now);

      delta = jkState - cbState;

      /**
       *  @todo When multiple spacecraft are supported, these items will need
       *  to be set to match the PSM's state vector
       */
      Integer cartObjCount = 1;
      Integer cartStateStart = 0;

      for (Integer i = 0; i < cartObjCount; ++i)
      {
         Integer i6 = cartStateStart + i * 6;
         for (int j = 0; j < 6; ++j)
            j2kState[i6+j] -= delta[j];
            #ifdef DEBUG_REORIGIN
               MessageInterface::ShowMessage(
                   "ODEModel::ReturnFromOrigin()\n   Input (model) state: [%lf %lf %lf %lf %lf"
                   " %lf]\n   j2k state:   [%lf %lf %lf %lf %lf %lf]\n"
                   "   cb state:    [%lf %lf %lf %lf %lf %lf]\n"
                   "   delta:       [%lf %lf %lf %lf %lf %lf]\n"
                   "   raw state: [%lf %lf %lf %lf %lf %lf]\n\n",
                   modelState[0], modelState[1], modelState[2], modelState[3], modelState[4],
                   modelState[5],
                   j2kState[0], j2kState[1], j2kState[2], j2kState[3], j2kState[4],
                   j2kState[5],
                   cbState[0], cbState[1], cbState[2], cbState[3], cbState[4],
                   cbState[5],
                   delta[0], delta[1], delta[2], delta[3], delta[4], delta[5],
                   rawState[0], rawState[1], rawState[2], rawState[3],
                   rawState[4], rawState[5]);
         #endif
      }
   }
}


void EphemerisPropagator::ReturnFromOriginGT(GmatTime newEpoch)
{
   if ((j2kBody == NULL) || (propOrigin == NULL))
   {
      MessageInterface::ShowMessage("Cannot reorigin; j2kBody = %p, "
         "propOrigin = %p\n", j2kBody, propOrigin);
      return;
   }
#ifdef DEBUG_REORIGIN
   MessageInterface::ShowMessage("ODEModel::ReturnFromOriginGT entered\n");
#endif

   if (state != j2kState)
      memcpy(j2kState, state, dimension*sizeof(Real));

#ifdef DEBUG_REORIGIN
   MessageInterface::ShowMessage(
      "ODEModel::ReturnFromOriginGT()\n state: [%.12lf   %.12lf   %.12lf   %.12lf   %.12lf   %.12lf]\n", state[0], state[1], state[2], state[3], state[4], state[5]);
#endif

   if (centralBody != j2kBodyName)
   {
      Rvector6 cbState, jkState, delta;
      GmatTime now = ((newEpoch < 0.0) ? currentEpoch : newEpoch);
      cbState = propOrigin->GetMJ2000State(now);
      jkState = j2kBody->GetState(now);

      delta = jkState - cbState;

      /**
      *  @todo When multiple spacecraft are supported, these items will need
      *  to be set to match the PSM's state vector
      */
      Integer cartObjCount = 1;
      Integer cartStateStart = 0;

      for (Integer i = 0; i < cartObjCount; ++i)
      {
         Integer i6 = cartStateStart + i * 6;
         for (int j = 0; j < 6; ++j)
            j2kState[i6 + j] -= delta[j];
#ifdef DEBUG_REORIGIN
         MessageInterface::ShowMessage(
            "ODEModel::ReturnFromOriginGT()\n"
//            "Input (model) state: [%lf %lf %lf %lf %lf %lf]\n"
            "   j2k state:   [%lf %lf %lf %lf %lf %lf]\n"
            "   cb state:    [%lf %lf %lf %lf %lf %lf]\n"
            "   delta:       [%lf %lf %lf %lf %lf %lf]\n",
//            "   raw state: [%lf %lf %lf %lf %lf %lf]\n\n",
//            modelState[0], modelState[1], modelState[2], modelState[3], modelState[4],
//            modelState[5],
            j2kState[0], j2kState[1], j2kState[2], j2kState[3], j2kState[4],
            j2kState[5],
            cbState[0], cbState[1], cbState[2], cbState[3], cbState[4],
            cbState[5],
            delta[0], delta[1], delta[2], delta[3], delta[4], delta[5]   //,
//            rawState[0], rawState[1], rawState[2], rawState[3],
//            rawState[4], rawState[5]
           );
#endif
      }
   }
}
