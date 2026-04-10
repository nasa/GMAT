//$Id$
//------------------------------------------------------------------------------
//                                   Vary
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/22
//
/**
 * Definition for the Vary command class
 */
//------------------------------------------------------------------------------


#include "Vary.hpp"
#include "ParameterException.hpp"
#include "DifferentialCorrector.hpp"
#include "Parameter.hpp"
#include "StringUtil.hpp"  // for Replace()
#include "MessageInterface.hpp"
#include "TextParser.hpp"
#include <sstream>         // for std::stringstream


//#define DEBUG_VARIABLE_RANGES
//#define DEBUG_VARY_EXECUTE
//#define DEBUG_VARY_PARAMS
//#define DEBUG_VARY_PARSING
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_RENAME
//#define DEBUG_VARY_GEN_STRING

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//------------------------------------------------------------------------------
//  static data
//------------------------------------------------------------------------------
const std::string Vary::PARAMETER_TEXT[VaryParamCount -
                                       SolverSequenceCommandParamCount] =
{
   "SolverName",
   "Variable",
   "InitialValue",
   "Perturbation",
   "Lower",
   "Upper",
   "MaxStep",
   "AdditiveScaleFactor",
   "MultiplicativeScaleFactor",
   "CurrentValue"
};

const Gmat::ParameterType Vary::PARAMETER_TYPE[VaryParamCount -
                                              SolverSequenceCommandParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
};
 

//------------------------------------------------------------------------------
//  Vary(void)
//------------------------------------------------------------------------------
/**
 * Creates a Vary command.  (default constructor)
 */
//------------------------------------------------------------------------------
Vary::Vary() :
   SolverSequenceCommand         ("Vary"),
   solverName                    (""),
   solver                        (NULL),
   variableName                  (""),
   variable                      (NULL),
   initialValueName              ("0.0"),
   initialValue                  (NULL),
   currentValue                  (0.0),
   perturbationName              ("0.001"),
   perturbation                  (NULL),
   variableLowerName             ("-9.999999e300"),
   variableLower                 (NULL),
   variableUpperName             ("9.999999e300"),
   variableUpper                 (NULL),
   variableMaximumStepName       ("9.999999e300"),
   variableMaximumStep           (NULL),
   additiveScaleFactorName       ("0.0"),
   additiveScaleFactor           (NULL),
   multiplicativeScaleFactorName ("1.0"),
   multiplicativeScaleFactor     (NULL),
   variableID                    (-1),
   solverDataFinalized           (false),
   isThisLastVaryCommand         (false)
{
   objectTypeNames.push_back("SolverCommand");
   objectTypeNames.push_back("Vary");
   settables.push_back("Perturbation"); 
   settables.push_back("MaxStep");
   settables.push_back("Lower");  
   settables.push_back("Upper");  
   settables.push_back("AdditiveScaleFactor");
   settables.push_back("MultiplicativeScaleFactor");
//    //parameterCount += 7;
//    additiveScaleFactor.push_back(0.0);
//    multiplicativeScaleFactor.push_back(1.0);
   parameterCount = VaryParamCount;
}


    
//------------------------------------------------------------------------------
//  Vary(const Vary& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Vary command.  (Copy constructor)
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Vary::Vary(const Vary& t) :
   SolverSequenceCommand         (t),
   solverName                    (t.solverName),
   solver                        (NULL),
   variableName                  (t.variableName),
   variable                      (NULL),
   initialValueName              (t.initialValueName),
   initialValue                  (NULL),
   currentValue                  (0.0),
   perturbationName              (t.perturbationName),
   perturbation                  (NULL),
   variableLowerName             (t.variableLowerName),
   variableLower                 (NULL),
   variableUpperName             (t.variableUpperName),
   variableUpper                 (NULL),
   variableMaximumStepName       (t.variableMaximumStepName),
   variableMaximumStep           (NULL),
   additiveScaleFactorName       (t.additiveScaleFactorName),
   additiveScaleFactor           (NULL),
   multiplicativeScaleFactorName (t.multiplicativeScaleFactorName),
   multiplicativeScaleFactor     (NULL),
   variableID                    (-1),
   solverDataFinalized           (t.solverDataFinalized),
   isThisLastVaryCommand         (t.isThisLastVaryCommand)
{
    parameterCount = t.parameterCount;
}


//------------------------------------------------------------------------------
//  Vary& operator=(const Vary& t)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Vary command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Vary& Vary::operator=(const Vary& t)
{
   if (this == &t)
      return *this;

   SolverSequenceCommand::operator=(t);
        
   solverName                    = t.solverName;
   variableName                  = t.variableName;
   initialValueName              = t.initialValueName;
   currentValue                  = 0.0;
   perturbationName              = t.perturbationName;
   variableLowerName             = t.variableLowerName;
   variableUpperName             = t.variableUpperName;
   variableMaximumStepName       = t.variableMaximumStepName;
   additiveScaleFactorName       = t.additiveScaleFactorName;
   multiplicativeScaleFactorName = t.multiplicativeScaleFactorName;

   // Initialize wrappers to NULL
   ClearWrappers();

   solver = NULL;
   variableID = -1;
   solverDataFinalized = t.solverDataFinalized;
    
   return *this;
}

//------------------------------------------------------------------------------
//  ~Vary(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Vary command.  (destructor)
 */
//------------------------------------------------------------------------------
Vary::~Vary()
{
   ClearWrappers();
}



//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Vary.
 *
 * @return clone of the Vary.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Vary::Clone() const
{
   return (new Vary(*this));
}


//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name.  (Used to
 *                indent commands)
 * @param useName Name that replaces the object's name.  (Not used in
 *                commands)
 *
 * @return The script line that, when interpreted, defines this Vary command.
 */
//------------------------------------------------------------------------------
const std::string& Vary::GetGeneratingString(Gmat::WriteMode mode,
                                             const std::string &prefix,
                                             const std::string &useName)
{
   #ifdef DEBUG_VARY_GEN_STRING
   MessageInterface::ShowMessage
      ("Vary::GetGeneratingString() <%p>'%s' entered\n", this, GetTypeName().c_str());
   #endif
   
   // Build the local string
   std::stringstream details;
   details.precision(16);
   
   std::string gen = prefix + "Vary " + solverName + "(";
   
   // loj: added check for NULL pointer to avoid crash and we can still
   // get generating string (2008.05.22)
   // Iterate through the variables
   if (variable)
      details << variable->GetDescription() << " = ";
   else
      details << "Unknown-Variable" << " = ";
   
   if (initialValue)
      if (initialValue->GetWrapperType() != Gmat::NUMBER_WT)
         details << initialValue->GetDescription() <<  ", ";
      else
         details << initialValue->EvaluateReal() <<  ", ";
   else
      details << "Unknown-InitialValue"  <<  ", ";
   
   details << "{";
   Integer addCount = 0;
   
   if (solver)
   {
      #ifdef DEBUG_VARY_GEN_STRING
      MessageInterface::ShowMessage
         ("   solver = <%p><%s>'%s'\n", solver, solver->GetTypeName().c_str(),
          solver->GetName().c_str());
      #endif
      
      Integer id = solver->GetParameterID("AllowVariablePertSetting");
      if (solver->GetBooleanParameter(id))
      {
         details << "Perturbation = ";
         if (perturbation)
            details << perturbation->GetDescription();
         else
            details << "Unknown-Perturbation";
         
         addCount++;
      }
      
      id = solver->GetParameterID("AllowRangeSettings");
      if (solver->GetBooleanParameter(id))
      {
         if (addCount > 0)
            details << ", ";
         
         details << "Lower = ";
         if (variableLower)
            details << variableLower->GetDescription();
         else
            details << "Unknown-VariableLower";
         
         details << ", Upper = ";
         if (variableUpper)
            details << variableUpper->GetDescription();
         else
            details << "Unknown-VariableUpper";
         
         addCount++;
      }
      
      id = solver->GetParameterID("AllowStepsizeSetting");
      if (solver->GetBooleanParameter(id))
      {
         if (addCount > 0)
            details << ", ";
         
         details << "MaxStep = ";
         
         if (variableMaximumStep)
            details << variableMaximumStep->GetDescription();
         else
            details << "Unknown-VariableMaximumStep";
         
         addCount++;
      }
      
      id = solver->GetParameterID("AllowScaleSetting");
      if (solver->GetBooleanParameter(solver->GetParameterID("AllowScaleSetting")))
      {
         if (addCount > 0)
            details << ", ";
         
         details << "AdditiveScaleFactor = ";
         if (additiveScaleFactor)
            details << additiveScaleFactor->GetDescription();
         else
            details << "Unknown-AdditiveScaleFactor";
         
         details << ", MultiplicativeScaleFactor = ";
         if (multiplicativeScaleFactor)
            details << multiplicativeScaleFactor->GetDescription();
         else
            details << "Unknown-MultiplicativeScaleFactor";
      }
   }
   else
   {
      #ifdef DEBUG_VARY_GEN_STRING
      MessageInterface::ShowMessage("   solver is NULL\n");
      #endif

      details << "SOLVER IS NOT SET";
   }
   
   gen += details.str();
   generatingString = gen + "});";
   
   #ifdef DEBUG_VARY_GEN_STRING
   MessageInterface::ShowMessage
      ("Vary::GetGeneratingString() <%p>'%s' returning, '%s'\n",
       this, GetTypeName().c_str(), generatingString.c_str());
   #endif
   
   // Then call the base class method to handle comments
   return SolverSequenceCommand::GetGeneratingString(mode, prefix, useName);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Vary::RenameRefObject(const UnsignedInt type,
                           const std::string &oldName,
                           const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Vary::RenameRefObject() type=%d, oldName=%s, newName=%s\n",
       type, oldName.c_str(), newName.c_str());
   #endif

   if (type == Gmat::SOLVER)
   {
      if (solverName == oldName)
         solverName = newName;
   }
   
   // make sure the wrappers know to rename any objects they may be using
   if (variable)
   {
      variable->RenameObject(oldName, newName);
      variableName           = variable->GetDescription();
   }
   if (initialValue)        
   {
      initialValue->RenameObject(oldName, newName);
      initialValueName       = initialValue->GetDescription();
   }
   if (perturbation)        
   {
      perturbation->RenameObject(oldName, newName);
      perturbationName       = perturbation->GetDescription();
   }
   if (variableLower)     
   {
      variableLower->RenameObject(oldName, newName);
      variableLowerName    = variableLower->GetDescription();
   }
   if (variableUpper)     
   {
      variableUpper->RenameObject(oldName, newName);
      variableUpperName    = variableUpper->GetDescription();
   }
   if (variableMaximumStep) 
   {
      variableMaximumStep->RenameObject(oldName, newName);
      variableMaximumStepName= variableMaximumStep->GetDescription();
   }
   if (additiveScaleFactor) 
   {    
      additiveScaleFactor->RenameObject(oldName, newName);
      additiveScaleFactorName  = additiveScaleFactor->GetDescription();
   }
   if (multiplicativeScaleFactor) 
   {
      multiplicativeScaleFactor->RenameObject(oldName, newName);
       multiplicativeScaleFactorName    
                      = multiplicativeScaleFactor->GetDescription();
   }
   
   return SolverSequenceCommand::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Vary.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Vary::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SOLVER);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref objects used by the Vary.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 * 
 * @return the list of object names.
 * 
 */
//------------------------------------------------------------------------------
const StringArray& Vary::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::SOLVER)
   {
      refObjectNames.push_back(solverName);
      if ((solverName != SolverSequenceCommand::solverName) &&
          (SolverSequenceCommand::solverName != ""))
         refObjectNames.push_back(SolverSequenceCommand::solverName);
   }
   else if (type == Gmat::PARAMETER)
   {
      // For array element, remove parenthesis before adding
      std::string objName;
      objName = GmatStringUtil::GetArrayName(variableName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(initialValueName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(perturbationName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(variableLowerName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(variableUpperName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(variableMaximumStepName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(additiveScaleFactorName);
      refObjectNames.push_back(objName);
   }
   
   return refObjectNames;
}


//---------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//---------------------------------------------------------------------------
std::string Vary::GetParameterText(const Integer id) const
{
   if ((id >= SolverSequenceCommandParamCount) && (id < VaryParamCount))
      return PARAMETER_TEXT[id - SolverSequenceCommandParamCount];

   return SolverSequenceCommand::GetParameterText(id);
}


//---------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//---------------------------------------------------------------------------
Integer Vary::GetParameterID(const std::string &str) const
{
   for (Integer i = SolverSequenceCommandParamCount; i < VaryParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - SolverSequenceCommandParamCount])
         return i;
   }

   return SolverSequenceCommand::GetParameterID(str);
}


//---------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
Gmat::ParameterType Vary::GetParameterType(const Integer id) const
{
   if ((id >= SolverSequenceCommandParamCount) && (id < VaryParamCount))
      return PARAMETER_TYPE[id - SolverSequenceCommandParamCount];

    return SolverSequenceCommand::GetParameterType(id);
}


//---------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//---------------------------------------------------------------------------
std::string Vary::GetParameterTypeString(const Integer id) const
{
   return SolverSequenceCommand::PARAM_TYPE_STRING[GetParameterType(id)];
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
bool Vary::IsParameterReadOnly(const Integer id) const
{
   if (id == CURRENT_VALUE)
      return true;

   return SolverSequenceCommand::IsParameterReadOnly(id);
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
bool Vary::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}



//---------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
Real Vary::GetRealParameter(const Integer id) const
{
   if (id == INITIAL_VALUE)
      if (initialValue)
         return initialValue->EvaluateReal();
        
   if (id == PERTURBATION)
      if (perturbation)
         return perturbation->EvaluateReal();
        
   if (id == VARIABLE_LOWER)
      if (variableLower)
         return variableLower->EvaluateReal();
        
   if (id == VARIABLE_UPPER)
      if (variableUpper)
         return variableUpper->EvaluateReal();
        
   if (id == VARIABLE_MAXIMUM_STEP)
      if (variableMaximumStep)
         return variableMaximumStep->EvaluateReal();
            
   if (id == ADDITIVE_SCALE_FACTOR)
      if (additiveScaleFactor)
         return additiveScaleFactor->EvaluateReal();
         
   if (id == MULTIPLICATIVE_SCALE_FACTOR)
      if (multiplicativeScaleFactor)
         return multiplicativeScaleFactor->EvaluateReal();

   if (id == CURRENT_VALUE)
      return currentValue;

    return SolverSequenceCommand::GetRealParameter(id);
}


//---------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
Real Vary::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage(
      "Vary::Setting value of %f for parameter %s\n", value,
      (GetParameterText(id)).c_str());
   #endif

   return SolverSequenceCommand::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string Vary::GetStringParameter(const Integer id) const
{
   //if (id == solverNameID)
   if (id == SOLVER_NAME)
      return solverName;
        
   //if (id == variableNameID)
   if (id == VARIABLE_NAME)
      return variableName;
    
   if (id == INITIAL_VALUE)
      return initialValueName;
    
   if (id == PERTURBATION)
      return perturbationName;
    
   if (id == VARIABLE_LOWER)
      return variableLowerName;
    
   if (id == VARIABLE_UPPER)
      return variableUpperName;
    
   if (id == VARIABLE_MAXIMUM_STEP)
      return variableMaximumStepName;
    
   if (id == ADDITIVE_SCALE_FACTOR)
      return additiveScaleFactorName;
    
   if (id == MULTIPLICATIVE_SCALE_FACTOR)
      return multiplicativeScaleFactorName;
    
   return SolverSequenceCommand::GetStringParameter(id);
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Vary::GetStringParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetStringParameter(id);
}


//---------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool Vary::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == SOLVER_NAME)
   {
      solverName = value;
      // Set the superclass too; this will go away on refactoring
      SolverSequenceCommand::solverName = value;
      return true;
   }
   
   if (id == VARIABLE_NAME)
   {
      variableName = value;
      return true;
   }
   
   if ((id == VARIABLE_NAME) || (id == INITIAL_VALUE) || 
       (id == PERTURBATION) || (id == VARIABLE_LOWER) || 
       (id == VARIABLE_UPPER) || (id == VARIABLE_MAXIMUM_STEP) || 
       (id == ADDITIVE_SCALE_FACTOR) || (id == MULTIPLICATIVE_SCALE_FACTOR))
   {
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      
      // Also need to store the names for later mapping
      switch (id)
      {
         case INITIAL_VALUE:
            initialValueName = value;
            break;
         
         case PERTURBATION:
            perturbationName = value;
            break;
         
         case VARIABLE_LOWER:
            variableLowerName = value;
            break;
            
         case VARIABLE_UPPER:
            variableUpperName = value;
            break;
         
         case VARIABLE_MAXIMUM_STEP:
            variableMaximumStepName = value;
            break;
         
         case ADDITIVE_SCALE_FACTOR:
            additiveScaleFactorName = value;
            break;
         
         case MULTIPLICATIVE_SCALE_FACTOR:
            multiplicativeScaleFactorName = value;
            break;
         
         default:
            throw GmatBaseException("Unknown property for Vary command\n");
      }
      
      return true;
   }
    
   return SolverSequenceCommand::SetStringParameter(id, value);
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool Vary::SetStringParameter(const std::string &label, const std::string &value)
{
   Integer id = GetParameterID(label);
   return SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// void Vary::SetField(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Enabled direct updates to the Vary command settings from the API
 *
 * @param id Field ID for the setting
 * @param value New value for the setting
 */
//------------------------------------------------------------------------------
void Vary::SetField(const Integer id, const Real value)
{
   #ifdef DEBUG_VARY_API_ACCESS
      MessageInterface::ShowMessage("Setting field %s to Real value %le\n",
            GetParameterText(id).c_str(), value);
   #endif

   switch(id)
   {
   case INITIAL_VALUE:
      if (initialValue)
      {
         initialValue->SetReal(value);
         return;
      }
      break;

   case PERTURBATION:
      if (perturbation)
      {
         if (value != 0.0)
            perturbation->SetReal(value);
         return;
      }
      break;

   case VARIABLE_LOWER:
      if (variableLower)
      {
         variableLower->SetReal(value);
         return;
      }
      break;

   case VARIABLE_UPPER:
      if (variableUpper)
      {
         variableUpper->SetReal(value);
         return;
      }
      break;

   case VARIABLE_MAXIMUM_STEP:
      if (variableMaximumStep)
      {
         variableMaximumStep->SetReal(value);
         return;
      }
      break;

   case ADDITIVE_SCALE_FACTOR:
      if (additiveScaleFactor)
      {
         additiveScaleFactor->SetReal(value);
         return;
      }
      break;

   case MULTIPLICATIVE_SCALE_FACTOR:
      if (multiplicativeScaleFactor)
      {
         multiplicativeScaleFactor->SetReal(value);
         return;
      }
      break;

   default:
      break;
   }

   GmatBase::SetField(id, value);
}

//------------------------------------------------------------------------------
// void SetNumber(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Passes numeric data into fields
 *
 * @param id Field ID for the setting
 * @param value New value for the setting
 *
 * @return The real value for teh field
 */
//------------------------------------------------------------------------------
void Vary::SetNumber(const std::string &label, const Real value)
{
   SetField(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// void GetNumber(const Integer id)
//------------------------------------------------------------------------------
/**
 * Enabled direct updates to the Vary command settings from the API
 *
 * @param id Field ID for the setting
 *
 * @return The real value for the field
 */
//------------------------------------------------------------------------------
Real Vary::GetNumber(const Integer id)
{
   switch(id)
   {
   case INITIAL_VALUE:
      if (initialValue)
         return initialValue->EvaluateReal();

   case PERTURBATION:
      if (perturbation)
         return perturbation->EvaluateReal();

   case VARIABLE_LOWER:
      if (variableLower)
         return variableLower->EvaluateReal();

   case VARIABLE_UPPER:
      if (variableUpper)
         return variableUpper->EvaluateReal();

   case VARIABLE_MAXIMUM_STEP:
      if (variableMaximumStep)
         return variableMaximumStep->EvaluateReal();

   case ADDITIVE_SCALE_FACTOR:
      if (additiveScaleFactor)
         return additiveScaleFactor->EvaluateReal();

   case MULTIPLICATIVE_SCALE_FACTOR:
      if (multiplicativeScaleFactor)
         return multiplicativeScaleFactor->EvaluateReal();

   case CURRENT_VALUE:
         return currentValue;

   default:
      break;
   }

   return GmatBase::GetNumber(id);
}

Real Vary::GetNumber(const std::string &label)
{
   return GetNumber(GetParameterID(label));
}




//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets referenced objects.
 *
 * @param obj reference object pointer.
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool Vary::SetRefObject(GmatBase *obj, const UnsignedInt type,
                        const std::string &name)
{
   if (type == Gmat::SOLVER)
   {
      if (solverName == obj->GetName()) 
      {
         solver = (Solver*)obj;
         return true;
      }
      return false;
   }
   return SolverSequenceCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  void Vary::InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Vary command has the following syntax:
 *
 *     Vary myDC(Burn1.V = 0.5, {Pert = 0.0001, MaxStep = 0.05, 
 *               Lower = 0.0, Upper = 3.14159);
 *
 * where 
 *
 * 1. myDC is a Solver used to Vary a set of variables to achieve the
 * corresponding goals, 
 * 2. Burn1.V is the parameter that is varied, and
 * 3. The settings in the braces specify features about how the variable can
 * be changed.
 * 
 * This method breaks the script line into the corresponding pieces, and stores 
 * the name of the Solver so it can be set to point to the correct object 
 * during initialization.
 */
//------------------------------------------------------------------------------
bool Vary::InterpretAction()
{
   // Clean out any old data
   wrapperObjectNames.clear();
   ClearWrappers();

   StringArray chunks = InterpretPreface();

   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage("Preface chunks as\n");
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
      MessageInterface::ShowMessage("\n");
   #endif

   if (chunks.size() <= 1)
      throw CommandException("Missing information for Vary command.\n");
   
   if (chunks[1].at(0) == '(')
      throw CommandException("Missing solver name for Vary command.\n");
   
   if ((chunks[1].find("[") != chunks[1].npos) || (chunks[1].find("]") != chunks[1].npos))
      throw CommandException("Brackets not allowed in Vary command");

   if (!GmatStringUtil::AreAllBracketsBalanced(chunks[1], "({)}"))      
      throw CommandException
         ("Parentheses, braces, or brackets are unbalanced");

      // Find and set the solver object name
   // This is the only setting in Vary that is not in a wrapper
   StringArray currentChunks = parser.Decompose(chunks[1], "()", false);
   SetStringParameter(SOLVER_NAME, currentChunks[0]);
   
   // The remaining text in the instruction is the variable definition and 
   // parameters, all contained in currentChunks[1].  Deal with those next.
   //currentChunks = parser.SeparateBrackets(currentChunks[1], "()", ", ");
   //std::string noLeftBrace  = GmatStringUtil::RemoveAll(currentChunks[1],'{');
   //std::string noRightBrace = GmatStringUtil::RemoveAll(noLeftBrace,'}');
   //std::string noSpaces     = GmatStringUtil::RemoveAll(noRightBrace,' ');
   std::string noSpaces2     = GmatStringUtil::RemoveAll(currentChunks[1],' ');
   currentChunks = parser.Decompose(noSpaces2, "()", true, true);
   //currentChunks = parser.Decompose(currentChunks[1], "()", true, true);
   
   #ifdef DEBUG_VARY_PARSING
      //MessageInterface::ShowMessage(
      //   "Vary:  noSpaces = %s\n", noSpaces.c_str());
      MessageInterface::ShowMessage(
         "Vary: after Decompose, current chunks = \n");
      for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
         MessageInterface::ShowMessage("   %s\n",
                                       currentChunks[jj].c_str());
   #endif

   if (currentChunks.size() == 0)
      throw CommandException("The variable description is missing in the "
            "Vary command\n");
   
   // First chunk is the variable and initial value
   std::string lhs, rhs;
   if (!SeparateEquals(currentChunks[0], lhs, rhs, true))
      // Variable takes default initial value
      //rhs = "0.0";
   {
      throw CommandException("The variable \"" + lhs + 
         "\" is missing the \"=\" operator or an initial value required for a " + typeName + 
         " command.\n");
   }
      
   variableName = lhs;
   variableID = -1;
   
   initialValueName = rhs;
   
   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage(
         "Vary:  setting variableName to %s\n", variableName.c_str());
      MessageInterface::ShowMessage(
         "Vary:  setting initialValueName to %s\n", initialValueName.c_str());
   #endif
   
   if (currentChunks.size() > 1)
   {
      std::string noSpaces     = GmatStringUtil::RemoveAll(currentChunks[1],' ');
      // Now deal with the settable parameters
      //currentChunks = parser.SeparateBrackets(currentChunks[1], "{}", ",", false);
      currentChunks = parser.SeparateBrackets(noSpaces, "{}", ",", true);
      
      #ifdef DEBUG_VARY_PARSING
         MessageInterface::ShowMessage(
            "Vary: After SeparateBrackets, current chunks = \n");
         for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
            MessageInterface::ShowMessage("   %s\n",
                                          currentChunks[jj].c_str());
      #endif
      
      // currentChunks now holds all of the pieces - no need for more separation  
      
      for (StringArray::iterator i = currentChunks.begin(); 
           i != currentChunks.end(); ++i)
      {
         bool isOK = SeparateEquals(*i, lhs, rhs, true);
         #ifdef DEBUG_VARY_PARSING
            MessageInterface::ShowMessage("Setting Vary properties\n");
            MessageInterface::ShowMessage("   \"%s\" = \"%s\"\n", lhs.c_str(), rhs.c_str());
         #endif
         if (!isOK || lhs.empty() || rhs.empty())
            throw CommandException("The setting \"" + lhs + 
               "\" is missing the \"=\" operator or a value required for a " + typeName + 
               " command.\n");
         
         if (IsSettable(lhs))
            SetStringParameter(GetParameterID(lhs), rhs);
         else
            throw CommandException("The setting \"" + lhs + 
               "\" is not a valid setting for a " + typeName + 
               " command.\n");
      }
   }
   return true;
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& Vary::GetWrapperObjectNameArray(bool completeSet)
{
   wrapperObjectNames.clear();

   wrapperObjectNames.push_back(variableName);
   
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       initialValueName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(initialValueName);

   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       perturbationName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(perturbationName);
       
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       variableLowerName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(variableLowerName);
       
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       variableUpperName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(variableUpperName);
       
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       variableMaximumStepName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(variableMaximumStepName);
       
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       additiveScaleFactorName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(additiveScaleFactorName);
       
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       multiplicativeScaleFactorName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(multiplicativeScaleFactorName);
   
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool Vary::SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Vary::SetElementWrapper() <%p> entered, toWrapper=<%p>'%s', withName='%s'\n",
       this, toWrapper, toWrapper ? toWrapper->GetDescription().c_str() : "NULL",
       withName.c_str());
   #endif
   
   bool retval = false;
   
   if (toWrapper == NULL) return false;
   
   if (toWrapper->GetWrapperType() == Gmat::ARRAY_WT)
   {
      throw CommandException("A value of type \"Array\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }
   CheckDataType(toWrapper, Gmat::REAL_TYPE, "Vary", true);
   
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage("   Setting wrapper \"%s\" on Vary command\n", 
      withName.c_str());
   #endif
   
   if (variableName == withName)
   {
      if (variable != NULL &&
          !IsThereSameWrapperName(VARIABLE_NAME, withName))
         CollectOldWrappers(&variable);
      
      variable = toWrapper;
      retval = true;
   }
   
   if (initialValueName == withName)
   {
      if (initialValue != NULL &&
          !IsThereSameWrapperName(INITIAL_VALUE, withName))
         CollectOldWrappers(&initialValue);
      
      initialValue = toWrapper;
      retval = true;
   }
   
   if (perturbationName == withName)
   {
      if (perturbation != NULL&&
          !IsThereSameWrapperName(PERTURBATION, withName))
         CollectOldWrappers(&perturbation);
      
      perturbation = toWrapper;
      retval = true;
   }
   
   if (variableLowerName == withName)
   {
      if (variableLower != NULL &&
          !IsThereSameWrapperName(VARIABLE_LOWER, withName))
         CollectOldWrappers(&variableLower);
      
      variableLower = toWrapper;
      retval = true;
   }
   
   if (variableUpperName == withName)
   {
      if (variableUpper != NULL &&
          !IsThereSameWrapperName(VARIABLE_UPPER, withName))
         CollectOldWrappers(&variableUpper);
      
      variableUpper = toWrapper;
      retval = true;
   }
   
   if (variableMaximumStepName == withName) 
   {
      if (variableMaximumStep != NULL &&
          !IsThereSameWrapperName(VARIABLE_MAXIMUM_STEP, withName))
         CollectOldWrappers(&variableMaximumStep);
      
      variableMaximumStep = toWrapper;
      retval = true;
   }
   
   if (additiveScaleFactorName == withName)
   {
      if (additiveScaleFactor != NULL &&
          !IsThereSameWrapperName(ADDITIVE_SCALE_FACTOR, withName))
         CollectOldWrappers(&additiveScaleFactor);
      
      additiveScaleFactor = toWrapper;
      retval = true;
   }
   
   if (multiplicativeScaleFactorName == withName)
   {
      if (multiplicativeScaleFactor != NULL &&
          !IsThereSameWrapperName(MULTIPLICATIVE_SCALE_FACTOR, withName))
         CollectOldWrappers(&multiplicativeScaleFactor);
      
      multiplicativeScaleFactor = toWrapper;
      retval = true;
   }
   
   //@todo In nested function, there are still bad wrapper pointers so until it is resolved
   // delete if not in function. (tested with Func_BasicOptimizerTest.script,
   // Func_Pic_Achieve.script)
   //@todo Func_BasicOptimizerTest.script still shows 6 memory leak trace (LOJ: 2009.11.03)
   //+++ <026116C0> F Validator::CreateOtherWrapper()  ew = new VariableWrapper() 
   //+++ <026733F8> G Validator::CreateOtherWrapper()  ew = new VariableWrapper() 
   //+++ <02669DF8> 8 Validator::CreateElementWrapper()  ew = new NumberWrapper() 
   //+++ <026116C0> F Validator::CreateOtherWrapper()  ew = new VariableWrapper() 
   //+++ <026733F8> G Validator::CreateOtherWrapper()  ew = new VariableWrapper() 
   //+++ <02669DF8> 8 Validator::CreateElementWrapper()  ew = new NumberWrapper() 
   
   //Commented out since old wrappers are deleted in ClearWrappers() which is called
   // from the Validator::ValidateCommand() (LOJ: 2009.11.03)
   //if (currentFunction == NULL)
   //   DeleteOldWrappers();
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Vary::SetElementWrapper() <%p> returning %d\n", this, retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
void Vary::ClearWrappers()
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Vary::ClearWrappers() entered, has %d old wrappers\n", oldWrappers.size());
   #endif
   
   // For element wrapper cleanup
   ClearOldWrappers();
   CollectOldWrappers(&variable);
   CollectOldWrappers(&initialValue);
   CollectOldWrappers(&perturbation);
   CollectOldWrappers(&variableLower);
   CollectOldWrappers(&variableUpper);
   CollectOldWrappers(&variableMaximumStep);
   CollectOldWrappers(&additiveScaleFactor);
   CollectOldWrappers(&multiplicativeScaleFactor);
   DeleteOldWrappers();
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Varyer.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Vary::Initialize()
{
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("Vary::Initialize() entered\n");
   MessageInterface::ShowMessage("%s\n", GmatBase::WriteObjectInfo("", solver).c_str());
   #endif
   
   bool retval = SolverSequenceCommand::Initialize();
   
   if (solver == NULL)
      throw CommandException("solver not initialized for Vary command\n  \""
                             + generatingString + "\"\n");
   
   std::string solverType = solver->GetTypeName();
   std::string errMsg = "Upper and Lower bounds in the Vary command are required when using " +
      solverType + " in \n   " + GetGeneratingString(Gmat::NO_COMMENTS);
   
   Integer id = solver->GetParameterID("Variables");
   solver->SetStringParameter(id, variableName);
   
   // The solver cannot be finalized until all of the loop is initialized
   solverDataFinalized = false;

   // Set references for the wrappers   
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for variable\n");
   #endif
   if (SetWrapperReferences(*variable) == false)
      return false;
   CheckDataType(variable, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for initial value\n");
   #endif
   if (SetWrapperReferences(*initialValue) == false)
      return false;
   CheckDataType(initialValue, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for perturbation\n");
   #endif
   if (SetWrapperReferences(*perturbation) == false)
      return false;
   CheckDataType(perturbation, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for minimum\n");
   #endif
   if (SetWrapperReferences(*variableLower) == false)
      return false;
   if (solverType == "SNOPT")
      CheckDataType(variableLower, Gmat::REAL_TYPE, "Vary", false, true, -9.999999e300, errMsg);
   else
      CheckDataType(variableLower, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for maximum\n");
   #endif
   if (SetWrapperReferences(*variableUpper) == false)
      return false;
   if (solverType == "SNOPT")
      CheckDataType(variableUpper, Gmat::REAL_TYPE, "Vary", false, true, 9.999999e300, errMsg);
   else
      CheckDataType(variableUpper, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for max step\n");
   #endif
   if (SetWrapperReferences(*variableMaximumStep) == false)
      return false;
   CheckDataType(variableMaximumStep, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for additive scale factor\n");
   #endif
   if (SetWrapperReferences(*additiveScaleFactor) == false)
      return false;
   CheckDataType(additiveScaleFactor, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for mult. scale factor\n");
   #endif
   if (SetWrapperReferences(*multiplicativeScaleFactor) == false)
      return false;
   CheckDataType(multiplicativeScaleFactor, Gmat::REAL_TYPE, "Vary");

   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Vary command Initialization complete\n");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Vary the variables defined for this Varying loop.
 *
 * This method (will eventually) runs the Varyer state machine in order to
 * determine the variable values needed to achieve the user specified 
 * goals.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Vary::Execute()
{
   bool retval = true;

   #ifdef DEBUG_VARY_EXECUTE
      MessageInterface::ShowMessage
         ("Vary::Execute() solverDataFinalized=%d\n", solverDataFinalized);
   #endif
    
   if (!solverDataFinalized) 
   {
      #ifdef DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage
            ("Vary::Execute() - running code for the first time through <<<\n");
      #endif
      // First time through, tell the solver about the variables
      Real varData[6], asf, msf;
      asf = additiveScaleFactor->EvaluateReal();
      msf = 1.0 / multiplicativeScaleFactor->EvaluateReal();

      if (msf <= 0.0)
      {
         std::stringstream msg;
         msg << "Error in the Vary command; The multiplicative scale factor "
             << "has the value "
             << multiplicativeScaleFactor->EvaluateReal()
             << ", but multiplicative scale factors must be greater than 0 on "
             << "the line\n" << GetGeneratingString(Gmat::SCRIPTING);
         throw CommandException(msg.str());
      }

      Real initval = initialValue->EvaluateReal();
      Real minval  = variableLower->EvaluateReal();
      Real maxval  = variableUpper->EvaluateReal();

      // Make sure the initial variable is in range
      if (initval < minval)
      {
         MessageInterface::ShowMessage("*** Warning*** Initial variable value"
               " %lf is lower than lower bound (%lf). It will be adjusted to "
               "the lower bound.\n", initval, minval);
         initval = minval;
      }
      if (initval > maxval)
      {
         MessageInterface::ShowMessage("*** Warning*** Initial variable value"
               " %lf is greater than upper bound (%lf). It will be adjusted to "
               "the upper bound.\n", initval, maxval);
         initval = maxval;
      }

      varData[5] = initval;                // Initial value

      // scale by using Eq. 13.5 of Architecture document
      varData[0] = (varData[5] + asf) / msf;
      varData[1] = (perturbation->EvaluateReal()) / msf;         // pert
      varData[2] = (minval + asf) / msf;  // minimum
      varData[3] = (maxval + asf) / msf;  // maximum
      varData[4] = (variableMaximumStep->EvaluateReal()) / msf;  // largest step
      
      #ifdef DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage(
            "For variable \"%s\", data is [%15.9lf %15.9lf %15.9lf %15.9lf "
            "%15.9lf]\n", variableName.c_str(), varData[0], varData[1], varData[2], 
            varData[3], varData[4]);
      #endif      
   
      variableID = solver->SetSolverVariables(varData, variableName);

      #ifdef DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage("Solver variables were set\n");
      #endif

      solverDataFinalized = true;
      BuildCommandSummary(true);
      #ifdef DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage
            ("Vary::Execute - exiting with retval = %s\n",
            (retval? "true" : "False"));
      #endif
      return retval;
   }
   
   Real var = solver->GetSolverVariable(variableID);
   // scale using Eq. 13.6 of Architecture document
   var = var / multiplicativeScaleFactor->EvaluateReal() -
      additiveScaleFactor->EvaluateReal();
   
   currentValue = var;

   #ifdef DEBUG_VARIABLE_RANGES
      MessageInterface::ShowMessage(
         "Setting %s to %.12le; allowed range is [%.12le, %.12le]\n",
         variableName.c_str(), var, variableLower->EvaluateReal(), 
         variableUpper->EvaluateReal());
   #endif

   try
   {
      variable->SetReal(var);
      
      // Check if it should notify Publisher so that Subscribers can handle it
      // such as chunking the ephemeris data
      if (isThisLastVaryCommand)
      {
         #ifdef DEBUG_VARY_FOR_SUBSCRIBER
         MessageInterface::ShowMessage
            ("=====> <%p>'%s' is last Vary command = %d\n", this,
             GetGeneratingString().c_str());
         #endif
         
         GmatBase *refObj = variable->GetRefObject();
         
         #ifdef DEBUG_VARY_FOR_SUBSCRIBER
         MessageInterface::ShowMessage
            ("   variable refObj = <%p><%s>\n", refObj,
             refObj ? refObj->GetTypeName().c_str() : "NULL");
         #endif
         
         if (refObj)
         {
            bool notifyPublisher = false;
            bool isParameter = false;
            std::string satName;
            Real epoch = 0;
            
            if (refObj->IsOfType(Gmat::SPACECRAFT))
            {
               #ifdef DEBUG_VARY_FOR_SUBSCRIBER
               MessageInterface::ShowMessage("   refObj is a Spacecraft\n");
               #endif
               notifyPublisher = true;
               epoch = refObj->GetRealParameter("A1Epoch");
               satName = refObj->GetName();
            }
            else if (refObj->IsOfType("Parameter"))
            {
               #ifdef DEBUG_VARY_FOR_SUBSCRIBER
               MessageInterface::ShowMessage("   refObj is a Parameter\n");
               #endif
               
               GmatBase *paramOwner = ((Parameter*)refObj)->GetOwner();
               
               #ifdef DEBUG_VARY_FOR_SUBSCRIBER
               MessageInterface::ShowMessage
                  ("   paramOwner=<%p><%s>'%s'\n", paramOwner,
                   paramOwner ? paramOwner->GetTypeName().c_str() : "NULL",
                   paramOwner ? paramOwner->GetName().c_str() : "NULL");
               #endif
               
               // If Parameter owner is Spacecraft, set flag to notify Publisher that
               // Spacecraft property has changed
               if (paramOwner && paramOwner->IsOfType(Gmat::SPACECRAFT))
               {
                  notifyPublisher = true;
                  satName = paramOwner->GetName();
                  epoch = paramOwner->GetRealParameter("A1Epoch");
                  #ifdef DEBUG_VARY_FOR_SUBSCRIBER
                  MessageInterface::ShowMessage
                     ("   Current spacecraft epoch is %f\n", epoch);
                  #endif
               }
            }
            
            if (notifyPublisher)
            {
               std::string varMsg = variableName + " = " + initialValueName +
                  " in Vary command";
               #ifdef DEBUG_VARY_FOR_SUBSCRIBER
               MessageInterface::ShowMessage("   varMsg = %s\n", varMsg.c_str());
               #endif
               publisher->SetSpacecraftPropertyChanged(this, epoch, satName, varMsg);
            }
            
         } // end if (refObj)
      } // end if (isThisLastVaryCommand)
   }
   catch (ParameterException &pe)
   {
      #ifdef DEBUG_VARY_PARSING
         MessageInterface::ShowMessage("Parameter assignment error:\n   %s\n",
               pe.GetFullMessage().c_str());
      #endif
      throw CommandException("**** ERROR **** Error "
            "scripting the variable in the Vary command: Coordinate systems "
            "and Central Bodies are not supported on the left hand side at "
            "this time in line:\n" + GetGeneratingString(Gmat::NO_COMMENTS));
   }
   solver->SetUnscaledVariable(variableID, var);
   
   BuildCommandSummary(true);
   #ifdef DEBUG_VARY_EXECUTE
      MessageInterface::ShowMessage
         ("Vary::Execute - exiting with retval = %s\n",
          (retval? "true" : "False"));
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void Vary::RunComplete()
{
   #ifdef DEBUG_VARY_EXECUTE
      MessageInterface::ShowMessage(
      "In Vary::RunComplete, solverDataFinalized = %s, ... now setting it to false\n",
      (solverDataFinalized? "true" : "false"));
   #endif
   solverDataFinalized = false;
   SolverSequenceCommand::RunComplete();
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Performs an action specific to Vary commands
 *
 * @param action Text trigger for the requested action
 * @param actionData Additional data needed to perform the action (defaults to
 *                   the empty string)
 *
 * @return true if an action was performed, false if not
 */
//------------------------------------------------------------------------------
bool Vary::TakeAction(const std::string &action, const std::string &actionData)
{
   if (action == "SolverReset")
   {
      // Prep to refresh the solver data if called again
      RefreshData();
      return true;
   }

   return SolverSequenceCommand::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// void SetInitialValue(Solver *theSolver)
//------------------------------------------------------------------------------
/**
 * Updates initial variable values from a solver control sequence.
 * 
 * This method is used to change the initial value of the variable -- for 
 * example, once a targeter has found a solution, a call to this method sets the 
 * initial value of the variable to match the solved-for value.
 * 
 * @param theSolver The Solver class that requested the variable updates.  
 *                  Updates are only applied if this Vary command uses the same
 *                  Solver.
 */
//------------------------------------------------------------------------------
void Vary::SetInitialValue(Solver *theSolver)
{
   if (solver == theSolver)
   {
      if ((initialValue->GetWrapperType() != Gmat::VARIABLE_WT) &&
          (initialValue->GetWrapperType() != Gmat::ARRAY_ELEMENT_WT) &&
          (initialValue->GetWrapperType() != Gmat::OBJECT_PROPERTY_WT) &&
          (initialValue->GetWrapperType() != Gmat::PARAMETER_WT))
      {
         Real var = solver->GetSolverVariable(variableID);

         // Unscale the variables using Eq. 13.6 of Architecture document
         var = var / multiplicativeScaleFactor->EvaluateReal() -
            additiveScaleFactor->EvaluateReal();

         initialValue->SetReal(var);

         MessageInterface::ShowMessage("Apply Corrections has changed the "
               "initial guess for %s from %s to %.12lf\n", variableName.c_str(),
               initialValueName.c_str(), var);

         if (initialValue->GetWrapperType() == Gmat::NUMBER_WT)
         {
            std::stringstream numString;
            numString.precision(16);
            numString << var;
            initialValueName = numString.str();
         }
      }
      else
      {
         MessageInterface::ShowMessage("*** Warning*** Apply Corrections did "
               "not change the initial guess setting %s from %s because "
               "Variables, Array elements, and object fields cannot be reset "
               "using the Apply Corrections button\n",  variableName.c_str(),
               initialValueName.c_str());
      }
   }
   else
   {
      MessageInterface::ShowMessage("*** Warning*** Apply Corrections did not "
            "change the value for %s because the variable is new or modified\n",
            variableName.c_str());
   }
}

//------------------------------------------------------------------------------
// void SetIsThisLastVaryCommand(bool flag)
//------------------------------------------------------------------------------
/**
 * Sets whether this command is the last Vary command in the SolverBranchCommand.
 * This method is called from the Sandbox during the initialization.
 */
//------------------------------------------------------------------------------
void Vary::SetIsThisLastVaryCommand(bool flag)
{
   isThisLastVaryCommand = flag;
}

//------------------------------------------------------------------------------
// bool IsThisLastVaryCommand()
//------------------------------------------------------------------------------
/**
 * Sets whether this command is the last Vary command in the SolverBranchCommand.
 * This method is called from the Sandbox during the initialization.
 */
//------------------------------------------------------------------------------
bool Vary::IsThisLastVaryCommand()
{
   return isThisLastVaryCommand;
}

//------------------------------------------------------------------------------
// bool IsThereSameWrapperName(const std::string &wrapperName)
//------------------------------------------------------------------------------
/*
 * Checks if there is the same wrapper name. Wrapper name can be a number.
 *
 * @return  true if the same wrapper name found, false otherwise
 */
//------------------------------------------------------------------------------
bool Vary::IsThereSameWrapperName(int param, const std::string &wrapperName)
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Vary::IsThereSameWrapperName() <%p>'%s' entered, wrapperName='%s'\n",
       this, GetTypeName().c_str(), wrapperName.c_str());
   #endif
   
   bool retval = false;
   
   // Check for the same wrapper is used for other properties
   for (int i = VARIABLE_NAME; i < VaryParamCount; i++)
   {
      if (i == param)
         continue;

      // Wrappers only apply to the scripted parameters, treated as strings
      if (GetParameterType(i) == Gmat::STRING_TYPE)
         if (GetStringParameter(i) == wrapperName)
         {
            retval = true;
            break;
         }
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Vary::IsThereSameWrapperName() <%p>'%s' wrapperName='%s' returning %d\n",
       this, GetTypeName().c_str(), wrapperName.c_str(), retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void RefreshData()
//------------------------------------------------------------------------------
/**
 * Updates the variable data with the current values from the Mission Control
 * Sequence.
 */
//------------------------------------------------------------------------------
void Vary::RefreshData()
{
   Real varData[5], asf, msf;
   asf = additiveScaleFactor->EvaluateReal();
   msf = 1.0 / multiplicativeScaleFactor->EvaluateReal();

   varData[0] = initialValue->EvaluateReal();                // Initial value

   // scale by using Eq. 13.5 of Architecture document
   varData[0] = (varData[0] + asf) / msf;
   varData[1] = (perturbation->EvaluateReal()) / msf;         // pert
   varData[2] = (variableLower->EvaluateReal() + asf) / msf;  // minimum
   varData[3] = (variableUpper->EvaluateReal() + asf) / msf;  // maximum
   varData[4] = (variableMaximumStep->EvaluateReal()) / msf;  // largest step

   #ifdef DEBUG_VARY_EXECUTE
      MessageInterface::ShowMessage(
         "For variable \"%s\", data is [%15.9lf %15.9lf %15.9lf %15.9lf "
         "%15.9lf]\n", variableName.c_str(), varData[0], varData[1], varData[2],
         varData[3], varData[4]);
   #endif

   solver->RefreshSolverVariables(varData, variableName);
}
