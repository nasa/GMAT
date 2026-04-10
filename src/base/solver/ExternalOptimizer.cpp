//$Id$
//------------------------------------------------------------------------------
//                         ExternalOptimizer
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan/GSFC
// Created: 2006.07.149
//
/**
 * Implementation for the external optimizer base class. 
 *
 */
//------------------------------------------------------------------------------


#include "ExternalOptimizer.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"



//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
ExternalOptimizer::PARAMETER_TEXT[ExternalOptimizerParamCount -OptimizerParamCount] =
{
   "FunctionPath",
   //"SourceType",
};

const Gmat::ParameterType
ExternalOptimizer::PARAMETER_TYPE[ExternalOptimizerParamCount - OptimizerParamCount] =
{
   Gmat::STRING_TYPE,
   //Gmat::STRING_TYPE,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ExternalOptimizer(std::string type, std::string name)
//------------------------------------------------------------------------------
ExternalOptimizer::ExternalOptimizer(std::string type, std::string name) :
   Optimizer               (type, name), 
   functionPath            (""),
   //sourceType              ("MATLAB"),
   sourceReady             (false),
   inSource                (NULL),
   inSourceServer          (NULL)
 {
   objectTypeNames.push_back("ExternalOptimizer");
   parameterCount = ExternalOptimizerParamCount;

   sourceType = "MATLAB";
   isInternal = false;
   requiresVariables = false;
   needsServerStartup = true;
}


//------------------------------------------------------------------------------
// ~ExternalOptimizer()
//------------------------------------------------------------------------------
ExternalOptimizer::~ExternalOptimizer()
{
   //FreeArrays();
}

//------------------------------------------------------------------------------
// ExternalOptimizer(const ExternalOptimizer &opt) :
//------------------------------------------------------------------------------
ExternalOptimizer::ExternalOptimizer(const ExternalOptimizer &opt) :
   Optimizer               (opt),
   functionPath            (opt.functionPath),
   //sourceType              (opt.sourceType),
   sourceReady             (false),
   inSource                (opt.inSource),
   inSourceServer          (opt.inSourceServer)
{
   parameterCount   = opt.parameterCount;
}


//------------------------------------------------------------------------------
// ExternalOptimizer& operator=(const ExternalOptimizer& opt)
//------------------------------------------------------------------------------
ExternalOptimizer& ExternalOptimizer::operator=(const ExternalOptimizer& opt)
{
   if (&opt == this)
      return *this;
   
   Optimizer::operator=(opt);
   
   functionPath        = opt.functionPath;
   //sourceType          = opt.sourceType;
   sourceReady         = opt.sourceReady;
   inSource            = opt.inSource;
   inSourceServer      = opt.inSourceServer;
   parameterCount      = opt.parameterCount;
   
   return *this;
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool ExternalOptimizer::Initialize()
{
   Optimizer::Initialize();

   // function path
   FileManager *fm = FileManager::Instance();
   std::string pathname;
   
   try
   {
      if (functionPath == "")
      {
         // matlab uses directory path
         if (sourceType == "MATLAB")
            pathname = fm->GetFullPathname("MATLAB_FUNCTION_PATH");
         
         functionPath = pathname;
      }
   }
   catch (GmatBaseException &)
   {
      try
      {
         // see if there is FUNCTION_PATH
         pathname = fm->GetFullPathname("FUNCTION_PATH");
         functionPath = pathname;
      }
      catch (GmatBaseException &)
      {
         throw;  // for now, at least
      }
   }
   
   #ifdef DEBUG_MATLAB_PATH
   MessageInterface::ShowMessage
      ("ExternalOptimizer::Initialize() functionPath='%s'\n", functionPath.c_str());
   #endif
   return true;
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string ExternalOptimizer::GetParameterText(const Integer id) const
{
   if ((id >= OptimizerParamCount) && (id < ExternalOptimizerParamCount))
      return PARAMETER_TEXT[id - OptimizerParamCount];
   return Optimizer::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer ExternalOptimizer::GetParameterID(const std::string &str) const
{
   for (Integer i = OptimizerParamCount; i < ExternalOptimizerParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - OptimizerParamCount])
         return i;
   }

   return Optimizer::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ExternalOptimizer::GetParameterType(
                                              const Integer id) const
{
   if ((id >= OptimizerParamCount) && (id < ExternalOptimizerParamCount))
      return PARAMETER_TYPE[id - OptimizerParamCount];

   return Optimizer::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string ExternalOptimizer::GetParameterTypeString(
                                      const Integer id) const
{
   return Optimizer::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string ExternalOptimizer::GetStringParameter(const Integer id) const
{
    if (id == FUNCTION_PATH)
        return functionPath;
    // if (id == SOURCE_TYPE)
    //     return sourceType;
    
    return Optimizer::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  Integer SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets a string or string array parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> string value for the parameter.
 *
 * @return  The value of the parameter at the completion of the call.
 */
//------------------------------------------------------------------------------
bool ExternalOptimizer::SetStringParameter(const Integer id,
                                           const std::string &value)
{
   if (id == FUNCTION_PATH) 
   {
      functionPath = value; 
      return true;
   }
   // if (id == SOURCE_TYPE) 
   // {
   //    // currently, only MATLAB allowed
   //    // Derived SNOPT from the ExternalOptimizer (LOJ: 2014.08.19)
   //    sourceType = value;  
   //    return true;
   // }
   
   
   return Optimizer::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param label The text label for the parameter
 *
 * @return The string assigned to the parameter
 */
 //------------------------------------------------------------------------------
std::string ExternalOptimizer::GetStringParameter(const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a parameter
 *
 * @param label The text label for the parameter
 * @param value The new parameter value
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
bool ExternalOptimizer::SetStringParameter(const std::string& label,
                                           const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

std::string ExternalOptimizer::GetStringParameter(const Integer id,
                                                  const Integer index) const
{
   return Optimizer::GetStringParameter(id, index);
}

bool ExternalOptimizer::SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index)
{
   return Optimizer::SetStringParameter(id, value, index);
}

std::string ExternalOptimizer::GetStringParameter(const std::string &label,
                                                  const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

bool ExternalOptimizer::SetStringParameter(const std::string &label, 
                                           const std::string &value,
                                           const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
//  std::string  GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 */
//------------------------------------------------------------------------------
/*
const StringArray& ExternalOptimizer::GetStringArrayParameter(
                                                        const Integer id) const
{
        
    if (id == PARAMETER_LIST)
        return parmList;
        
    return Optimizer::GetStringArrayParameter(id);
}
*/

