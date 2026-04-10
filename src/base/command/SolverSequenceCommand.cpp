//$Id$
//------------------------------------------------------------------------------
//                           SolverSequenceCommand
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28.
//
// Author: Darrel J. Conway
// Created: 2012/08/22
//
/**
 * Implementation for the SolverSequenceCommand base class
 */
//------------------------------------------------------------------------------

#include "SolverSequenceCommand.hpp"


//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
SolverSequenceCommand::PARAMETER_TEXT[SolverSequenceCommandParamCount -
                                              GmatCommandParamCount] =
      {
         "SolverName"
      };

const Gmat::ParameterType
SolverSequenceCommand::PARAMETER_TYPE[SolverSequenceCommandParamCount -
                                              GmatCommandParamCount]=
      {
          Gmat::STRING_TYPE
      };



//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// SolverSequenceCommand(const std::string &cmdType)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param cmdType The script string used to invoke one of these commands
 */
//------------------------------------------------------------------------------
SolverSequenceCommand::SolverSequenceCommand(const std::string &cmdType) :
   GmatCommand          (cmdType),
   solverName           ("")
{
   objectTypeNames.push_back("SolverSequenceCommand");
   parameterCount = SolverSequenceCommandParamCount;
}


//------------------------------------------------------------------------------
// ~SolverSequenceCommand()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SolverSequenceCommand::~SolverSequenceCommand()
{
}


//------------------------------------------------------------------------------
// SolverSequenceCommand(const SolverSequenceCommand& ssc)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ssc The command providing data for this new one
 */
//------------------------------------------------------------------------------
SolverSequenceCommand::SolverSequenceCommand(const SolverSequenceCommand& ssc) :
   GmatCommand          (ssc),
   solverName           (ssc.solverName)
{
}


//------------------------------------------------------------------------------
// SolverSequenceCommand& operator=(const SolverSequenceCommand& ssc)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ssc The command used to update this one's data
 *
 * @return this command, configured to match ssc
 */
//------------------------------------------------------------------------------
SolverSequenceCommand& SolverSequenceCommand::operator=(
      const SolverSequenceCommand& ssc)
{
   if (this != &ssc)
   {
      GmatCommand::operator =(ssc);
      solverName = ssc.solverName;
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//       const std::string& oldName, const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Resets the name for referenced objects
 *
 * @param type The type of the object that is renamed
 * @param oldName The old name of the object
 * @param newName The new name
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool SolverSequenceCommand::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   if (oldName == solverName)
   {
      solverName = newName;
      return true;
   }
   return true;
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the script string for a parameter
 *
 * @param id The parameter's ID
 *
 * @return The script string
 */
//------------------------------------------------------------------------------
std::string SolverSequenceCommand::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < SolverSequenceCommandParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   return GmatCommand::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter ID for a scripted string
 *
 * @param str The script string for the parameter
 *
 * @return The parameter's ID
 */
//------------------------------------------------------------------------------
Integer SolverSequenceCommand::GetParameterID(const std::string& str) const
{
   for (Integer i = GmatCommandParamCount;
         i < SolverSequenceCommandParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }

   return GmatCommand::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrived teh type of a parameter
 *
 * @param id The parameter's ID
 *
 * @return The type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType SolverSequenceCommand::GetParameterType(
      const Integer id) const
{
   if (id >= GmatCommandParamCount && id < SolverSequenceCommandParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];

   return GmatCommand::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing a parameter's type
 *
 * @param id The paraemter's ID
 *
 * @return The type, as a string
 */
//------------------------------------------------------------------------------
std::string SolverSequenceCommand::GetParameterTypeString(
      const Integer id) const
{
   return GmatCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Determines if a parameter is not intended for access by typical users
 *
 * @param id The ID of the parameter
 *
 * @return true for parameters that are hidden, false for ones that are visible
 */
//------------------------------------------------------------------------------
bool SolverSequenceCommand::IsParameterReadOnly(const Integer id) const
{
   // For now, hide the SolverName
   if (id == SOLVER_NAME)
      return true;

   return GmatCommand::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Determines if a parameter is not intended for access by typical users
 *
 * @param label The script string of the parameter
 *
 * @return true for parameters that are hidden, false for ones that are visible
 */
//------------------------------------------------------------------------------
bool SolverSequenceCommand::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param id The parameter's ID
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string SolverSequenceCommand::GetStringParameter(const Integer id) const
{
   if (id == SOLVER_NAME)
      return solverName;
   return GmatCommand::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a parameter
 *
 * @param id The ID of the parameter
 * @param value The new parameter value
 *
 * @return true if the reset succeeded, false if not
 */
//------------------------------------------------------------------------------
bool SolverSequenceCommand::SetStringParameter(const Integer id,
      const std::string& value)
{
   if (id == SOLVER_NAME)
   {
      solverName = value;
      return true;
   }
   return GmatCommand::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a StringArray
 *
 * @param id The parameter's ID
 * @param index The index into the array
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string SolverSequenceCommand::GetStringParameter(const Integer id,
      const Integer index) const
{
   return GmatCommand::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in a StringArray
 *
 * @param id The parameter's ID
 * @param value The new parameter value
 * @param index The index into the array
 *
 * @return true if the reset succeeded, false if not
 */
//------------------------------------------------------------------------------
bool SolverSequenceCommand::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
   return GmatCommand::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a parameter
 *
 * @param label The string used to script the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string SolverSequenceCommand::GetStringParameter(const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a parameter
 *
 * @param label The string used to script the parameter
 * @param value The new parameter value
 *
 * @return true if the reset succeeded, false if not
 */
//------------------------------------------------------------------------------
bool SolverSequenceCommand::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a StringArray
 *
 * @param label The string used to script the parameter
 * @param index The index into the array
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string SolverSequenceCommand::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in a StringArray
 *
 * @param label The string used to script the parameter
 * @param value The new parameter value
 * @param index The index into the array
 *
 * @return true if the reset succeeded, false if not
 */
//------------------------------------------------------------------------------
bool SolverSequenceCommand::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}
