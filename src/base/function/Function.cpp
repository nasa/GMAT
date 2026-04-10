//$Id$
//------------------------------------------------------------------------------
//                                   Function
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
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Defines the Function base class used for Matlab and Gmat functions.
 */
//------------------------------------------------------------------------------

#include "Function.hpp"
#include "FunctionException.hpp"    // for exception
#include "StringUtil.hpp"           // for GmatStringUtil::
#include "Parameter.hpp"            // for GetOwner()
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION_SET
//#define DEBUG_FUNCTION_IN_OUT

//---------------------------------
// static data
//---------------------------------
const std::string
Function::PARAMETER_TEXT[FunctionParamCount - GmatBaseParamCount] =
{
   "FunctionPath",
   "FunctionName",
   "Input",
   "Output"
};

const Gmat::ParameterType
Function::PARAMETER_TYPE[FunctionParamCount - GmatBaseParamCount] =
{
   Gmat::FILENAME_TYPE,       // "FunctionPath",
   Gmat::STRING_TYPE,         // "FunctionName",
   Gmat::STRINGARRAY_TYPE,    // "Input",
   Gmat::STRINGARRAY_TYPE,    // "Output"
};


//------------------------------------------------------------------------------
//  Function(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the Function object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
Function::Function(const std::string &typeStr, const std::string &name) :
   GmatBase           (Gmat::FUNCTION, typeStr, name),
   callDescription    (""),
   functionPath       (""),
   functionName       (""),
   solarSys           (NULL),
   internalCoordSys   (NULL),
   forces             (NULL),
   errorFound         (false)
{
   objectTypes.push_back(Gmat::FUNCTION);
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("Function");
   parameterCount = FunctionParamCount;
}


//------------------------------------------------------------------------------
//  ~Function(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Function object (destructor).
 */
//------------------------------------------------------------------------------
Function::~Function()
{
}

//------------------------------------------------------------------------------
//  Function(const Function &f)
//------------------------------------------------------------------------------
/**
 * Constructs the Function object (copy constructor).
 * 
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
Function::Function(const Function &f) :
   GmatBase           (f),
   callDescription    (f.callDescription),
   functionPath       (f.functionPath),
   functionName       (f.functionName),
   inputNames         (f.inputNames),
   outputNames        (f.outputNames),
   outputWrapperTypes (f.outputWrapperTypes),
   outputRowCounts    (f.outputRowCounts),
   outputColCounts    (f.outputColCounts),
   solarSys           (NULL),
   internalCoordSys   (NULL),
   forces             (NULL),
   errorFound         (false)
{
   parameterCount = FunctionParamCount;
}


//------------------------------------------------------------------------------
//  Function& operator=(const Function &f)
//------------------------------------------------------------------------------
/**
 * Sets one Function object to match another (assignment operator).
 * 
 * @param <f> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
Function& Function::operator=(const Function &f)
{
   if (this == &f)
      return *this;
   
   GmatBase::operator=(f);
   
   callDescription    = f.callDescription;
   functionPath       = f.functionPath;
   functionName       = f.functionName;
   solarSys           = f.solarSys;
   internalCoordSys   = f.internalCoordSys;
   forces             = f.forces;
   errorFound         = f.errorFound;
   inputNames         = f.inputNames;
   outputNames        = f.outputNames;
   outputWrapperTypes = f.outputWrapperTypes;
   outputRowCounts    = f.outputRowCounts;
   outputColCounts    = f.outputColCounts;
   return *this;
}

//------------------------------------------------------------------------------
// void SetCallDescription(const std::string &desc)
//------------------------------------------------------------------------------
void Function::SetCallDescription(const std::string &desc)
{
   callDescription = desc;
}


//------------------------------------------------------------------------------
// virtual WrapperTypeArray GetOutputTypes(IntegerArray &rowCounts,
//                                         IntegeArrayr &colCounts) const
//------------------------------------------------------------------------------
WrapperTypeArray Function::GetOutputTypes(IntegerArray &rowCounts,
                                          IntegerArray &colCounts) const
{
   rowCounts = outputRowCounts;
   colCounts = outputColCounts;
   return outputWrapperTypes;
}


//------------------------------------------------------------------------------
// virtual void SetOutputTypes(WrapperTypeArray &outputTypes,
//                             IntegerArray &rowCounts, IntegerArray &colCounts)
//------------------------------------------------------------------------------
/*
 * Sets function output types. This method is called when parsing the function
 * file from the Interpreter.
 */
//------------------------------------------------------------------------------
void Function::SetOutputTypes(WrapperTypeArray &outputTypes,
                              IntegerArray &rowCounts, IntegerArray &colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("Function::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif
   
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;   
}


//------------------------------------------------------------------------------
// bool Function::IsNewFunction()
//------------------------------------------------------------------------------
/**
 * Defines the interface that identifies new functions. Normally this method
 * is called from GUI.
 *
 * @return true if the function is treated as new, false if not
 *
 * @note The default implementation just returns true.  Classes (like
 * GmatFunction) that need different behavior override this method to produce
 * the desired behavior.
 */
//------------------------------------------------------------------------------
bool Function::IsNewFunction()
{
   return true;
}

//------------------------------------------------------------------------------
// void SetNewFunction(bool flag)
//------------------------------------------------------------------------------
/**
 * Defines the interface that is used to identify new functions. Normally this
 * method is called from GUI for Gmatfunction.
 *
 * GmatFunctions use this interface to identify when a function is being loaded
 * for the first time.
 *
 * @param flag true for new functions, false if not new
 *
 * @note This default implementation doesn't do anything.
 */
//------------------------------------------------------------------------------
void Function::SetNewFunction(bool flag)
{
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize = false)
//------------------------------------------------------------------------------
bool Function::Initialize(ObjectInitializer *objInit, bool reinitialize)
{
   return true;
}


//------------------------------------------------------------------------------
// bool Function::Execute(ObjectInitializer *objInit)  [default implementation]
//------------------------------------------------------------------------------
bool Function::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   return true;
}


//------------------------------------------------------------------------------
// bool Function::Finalize(bool cleanUp = false)  [default implementation]
//------------------------------------------------------------------------------
void Function::Finalize(bool cleanUp)
{
   // nothing to do here
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void Function::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("Function::SetSolarSystem() entered, this='%s', ss=<%p>\n",
       GetName().c_str(), ss);
   #endif
   
   solarSys = ss;
}

//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
void Function::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("Function::SetInternalCoordSystem() entered, this='%s', cs=<%p>\n",
       GetName().c_str(), cs);
   #endif
   
   // if cs is not NULL, set (loj: 2008.10.07)
   if (cs != NULL)
      internalCoordSys = cs;
}

//------------------------------------------------------------------------------
// void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
void Function::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   forces = tf;
}


//------------------------------------------------------------------------------
// void SetErrorFound(bool errFlag)
//------------------------------------------------------------------------------
void Function::SetErrorFound(bool errFlag)
{
   errorFound = errFlag;
}

// called-from Sandbox
//------------------------------------------------------------------------------
// bool ErrorFound()
//------------------------------------------------------------------------------
bool Function::ErrorFound()
{
   return errorFound;
}

//------------------------------------------------------------------------------
// std::string GetFunctionPathAndName()
//------------------------------------------------------------------------------
std::string Function::GetFunctionPathAndName()
{
   return functionPath;
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Function::IsParameterReadOnly(const Integer id) const
{
   // We want both path and name when we write out, so skip this parameter
   if ((id == FUNCTION_NAME) || (id == FUNCTION_INPUT) || (id == FUNCTION_OUTPUT))
      return true;
   
   return GmatBase::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the name of the parameter with the input id.
 * 
 * @param <id> Integer id for the parameter.
 * 
 * @return The string name of the parameter.
 */
//------------------------------------------------------------------------------
std::string Function::GetParameterText(const Integer id) const
{
   if (id >= FUNCTION_PATH && id < FunctionParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Gets the id corresponding to a named parameter.
 * 
 * @param <str> Name of the parameter.
 * 
 * @return The ID.
 */
//------------------------------------------------------------------------------
Integer Function::GetParameterID(const std::string &str) const
{
   for (Integer i = FUNCTION_PATH; i < FunctionParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Function::GetParameterType(const Integer id) const
{
   if (id >= FUNCTION_PATH&& id < FunctionParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the text description for the type of a parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The text description of the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string Function::GetParameterTypeString(const Integer id) const
{
   if (id >= FUNCTION_PATH&& id < FunctionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * 
 * @return The value of the parameter.
 */
//------------------------------------------------------------------------------
std::string Function::GetStringParameter(const Integer id) const
{
   if (id == FUNCTION_PATH)
   {
      #ifdef DEBUG_GET
      MessageInterface::ShowMessage
         ("Function::GetStringParameter() <%p> returning functionPath '%s'\n",
          this, functionPath.c_str());
      #endif
      return functionPath;
   }
   else if (id == FUNCTION_NAME)
      return functionName;
   
   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Function::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param index Index for parameters in arrays.  Use -1 or the index free 
 *              version to add the value to the end of the array.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//---------------------------------------------------------------------------
std::string Function::GetStringParameter(const Integer id, 
                                         const Integer index) const
{
   switch (id)
   {
   case FUNCTION_INPUT:
      if (index >= (Integer)inputNames.size())
      {
         FunctionException fe;
         fe.SetDetails("The index of %d for field \"%s\" is out of bounds for the "
                       "object named \"%s\"", index, GetParameterText(id).c_str(),
                       GetName().c_str());
         throw fe;
      }
      else
         return inputNames[index];
      
   case FUNCTION_OUTPUT:
      if (index >= (Integer)outputNames.size())
      {
         FunctionException fe;
         fe.SetDetails("The index of %d for field \"%s\" is out of bounds for the "
                       "object named \"%s\"", index, GetParameterText(id).c_str(),
                       GetName().c_str());
         throw fe;
      }
      else
         return outputNames[index];
      
   default:
      return GmatBase::GetStringParameter(id);
   }
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label,
//                                 const Integer index) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param index Index for parameters in arrays.
 *
 * @return The string stored for this parameter, or the empty string if there
 *         is no string association.
 */
//---------------------------------------------------------------------------
std::string Function::GetStringParameter(const std::string &label,
                                         const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& Function::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case FUNCTION_INPUT:
      return inputNames;
   case FUNCTION_OUTPUT:
      return outputNames;
   default:
      return GmatBase::GetStringArrayParameter(id);
   }
}

//---------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//---------------------------------------------------------------------------
const StringArray& Function::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const char *value)
//------------------------------------------------------------------------------
/**
 * @see SetStringParameter(const Integer id, const std::string &value)
 */
//------------------------------------------------------------------------------
bool Function::SetStringParameter(const Integer id, const char *value)
{
   return SetStringParameter(id, std::string(value));
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return If value of the parameter was set.
 */
//------------------------------------------------------------------------------
bool Function::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("Function::SetStringParameter() entered, id=%d, value='%s'\n", id, value.c_str());
   #endif
   
   switch (id)
   {
   case FUNCTION_NAME:
      functionName = value;
      return true;
   case FUNCTION_INPUT:
      // Nothing to set here
      return true;
   case FUNCTION_OUTPUT:
      // Nothing to set here
      return true;
   default:
      return GmatBase::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
bool Function::SetStringParameter(const std::string &label,
                                  const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool Function::SetStringParameter(const std::string &label,
                                  const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// void         SetOutputName(const std::string &mOutputNames)
//------------------------------------------------------------------------------
void Function::SetOutputName(const std::string &outputName)
{
}

//---------------------------------
// protected
//---------------------------------

