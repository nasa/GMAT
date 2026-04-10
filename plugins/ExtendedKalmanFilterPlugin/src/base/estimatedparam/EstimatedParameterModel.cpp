//$Id$
//------------------------------------------------------------------------------
//                           EstimatedParameterModel
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. under
// contract number NNG15CR67C
//
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2020/02/04
/**
 * The base class for estimated parameters
 */
//------------------------------------------------------------------------------

#include "EstimatedParameterModel.hpp"
#include "EstimatedParameterException.hpp"
#include "CoordinateConverter.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------

const std::string
EstimatedParameterModel::PARAMETER_TEXT[EstimatedParameterModelParamCount - GmatBaseParamCount] =
{
   "SolveFor",
};

const Gmat::ParameterType
EstimatedParameterModel::PARAMETER_TYPE[EstimatedParameterModelParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,      // SolveFor
};


//------------------------------------------------------------------------------
// EstimatedParameterModel(const std::string &itsTypeName,
//       const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param itsTypeName The type name of the new object
 * @param itsName The name of the new object
 * @param itsShortName The short name of the new object
 */
//------------------------------------------------------------------------------
EstimatedParameterModel::EstimatedParameterModel(const std::string &itsTypeName,
       const std::string &itsName, const std::string &itsShortName) :
   GmatBase    (GmatType::GetTypeId("EstimatedParameterModel"),itsTypeName,itsName),
   shortName        (itsShortName),
   solveFor         (""),
   convFactor       (1.0)
{
   objectTypes.push_back(GmatType::GetTypeId("EstimatedParameterModel"));
   objectTypeNames.push_back("EstimatedParameterModel");

   parameterCount = EstimatedParameterModelParamCount;
}


//------------------------------------------------------------------------------
// ~EstimatedParameterModel()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EstimatedParameterModel::~EstimatedParameterModel()
{
}


//------------------------------------------------------------------------------
// EstimatedParameterModel(const EstimatedParameterModel& epm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
EstimatedParameterModel::EstimatedParameterModel(
      const EstimatedParameterModel& epm) :
   GmatBase (epm),
   shortName        (epm.shortName),
   solveFor         (epm.solveFor),
   allowedSolveFors (epm.allowedSolveFors),
   convFactor       (epm.convFactor)
{
}



//------------------------------------------------------------------------------
// EstimatedParameterModel& operator=(const EstimatedParameterModel& epm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
EstimatedParameterModel& EstimatedParameterModel::operator=(
      const EstimatedParameterModel& epm)
{
   if (this != &epm)
   {
      GmatBase::operator=(epm);

      shortName        = epm.shortName;
      solveFor         = epm.solveFor;
      allowedSolveFors = epm.allowedSolveFors;
      convFactor       = epm.convFactor;
   }
   return *this;
}


//------------------------------------------------------------------------------
// std::string GetShortName()
//------------------------------------------------------------------------------
/**
 * Get the short name for the script field
 */
//------------------------------------------------------------------------------
std::string EstimatedParameterModel::GetShortName()
{
   return shortName;
}


//------------------------------------------------------------------------------
// void SetConversionFactor(Real factor)
//------------------------------------------------------------------------------
/**
 * Set the value to convert from the epsilon value to the nominal value
 *
 * @param factor The conversion factor for the EstimatedParameter
 */
 //------------------------------------------------------------------------------
void EstimatedParameterModel::SetConversionFactor(Real factor)
{
   convFactor = factor;
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType EstimatedParameterModel::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EstimatedParameterModelParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string EstimatedParameterModel::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EstimatedParameterModelParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string EstimatedParameterModel::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EstimatedParameterModelParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer EstimatedParameterModel::GetParameterID(const std::string &str) const
{
   for (int i=GmatBaseParamCount; i<EstimatedParameterModelParamCount; i++)
   {
      if (str == EstimatedParameterModel::PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string EstimatedParameterModel::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case SOLVE_FOR:
      return solveFor;
   default:
      return GmatBase::GetStringParameter(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string EstimatedParameterModel::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool EstimatedParameterModel::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case SOLVE_FOR:
      if (find(allowedSolveFors.begin(), allowedSolveFors.end(), value) != allowedSolveFors.end())
      {
         solveFor = value;
         return true;
      }
      else
      {
         std::string allowedList;

         if (allowedSolveFors.size() > 0)
         {
            allowedList = allowedSolveFors[0];
            for (UnsignedInt n = 1; n < allowedSolveFors.size(); n++)
               allowedList += ", " + allowedSolveFors[n];
         }

         throw EstimatedParameterException("Error: The value \"" + value + "\" is not allowed "
            "for field \"SolveFors\". Allowed values are {" + allowedList + "}");
         return false;
      }

   default:
      return GmatBase::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool EstimatedParameterModel::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const StringArray& GetAllowedSolveFors() const
//------------------------------------------------------------------------------
/**
 * Gets the list of allowed solve for parameters
 *
 * @return The array of allowed solve for parameters
 */
 //------------------------------------------------------------------------------
const StringArray& EstimatedParameterModel::GetAllowedSolveFors() const
{
   return allowedSolveFors;
}
