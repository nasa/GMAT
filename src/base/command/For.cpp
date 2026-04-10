//$Id$
//------------------------------------------------------------------------------
//                                For
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
// Author:  Joey Gurganus
// Created: 2004/01/29
//
/**
 * Definition for the For command class
 */
//------------------------------------------------------------------------------
#include <ctype.h>               // for isalpha
#include <sstream>               // for std::stringstream, used to make generating string
#include "gmatdefs.hpp"
#include "For.hpp"
#include "BranchCommand.hpp"
#include "CommandException.hpp"
#include "FunctionManager.hpp"   // for GetFunctionPathAndName()
#include "StringUtil.hpp"        // for ToReal()
#include "MessageInterface.hpp"

//#define DEBUG_FOR_EXE 1
//#define DEBUG_FOR_INIT 1
//#define DEBUG_FOR_REAL
//#define DEBUG_FOR_PARAMS
//#define DEBUG_WRAPPER_CODE

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string
For::PARAMETER_TEXT[ForParamCount - BranchCommandParamCount] =
{
   "StartValue",
   "EndValue",
   "Step", 
   "IndexName",
   "StartName",
   "EndName",
   "IncrementName",
};

const Gmat::ParameterType
For::PARAMETER_TYPE[ForParamCount - BranchCommandParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
};

const Real For::UNINITIALIZED_VALUE = -999.99;
const Real For::DEFAULT_START       = 1;
const Real For::DEFAULT_END         = 10;
const Real For::DEFAULT_INCREMENT   = 1;

bool       For::writeInvalidLoopMessage = true;


//------------------------------------------------------------------------------
//  For()
//------------------------------------------------------------------------------
/**
 * Creates a For command.  (default constructor)
 */
//------------------------------------------------------------------------------
For::For() :
   BranchCommand   ("For"),
   startValue      (DEFAULT_START),
   endValue        (DEFAULT_END),
   stepSize        (DEFAULT_INCREMENT),
   currentValue    (UNINITIALIZED_VALUE),
   numPasses       ((int)UNINITIALIZED_VALUE),
   currentPass     ((int)UNINITIALIZED_VALUE),
   indexWrapper    (NULL),
   startWrapper    (NULL),
   incrWrapper     (NULL),
   endWrapper      (NULL),
   incrPositive    (true),
   indexName       ("I"),
   startName       ("1"),
   endName         ("10"),
   incrName        ("1")
{
   parameterCount = ForParamCount;
   objectTypeNames.push_back("For");
}

//------------------------------------------------------------------------------
//  For(const For& f)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a For command.  (Copy constructor)
 *
 * @param f For from which to copy data when creating a new For
 */
//------------------------------------------------------------------------------
For::For(const For& f) :
   BranchCommand   (f),
   startValue      (f.startValue),
   endValue        (f.endValue),
   stepSize        (f.stepSize),
   currentValue    (f.currentValue),
   numPasses       (f.numPasses),
   currentPass     (f.currentPass),
   indexWrapper    (NULL),
   startWrapper    (NULL),
   incrWrapper     (NULL),
   endWrapper      (NULL),
   incrPositive    (f.incrPositive),
   indexName       (f.indexName),
   startName       (f.startName),
   endName         (f.endName),
   incrName        (f.incrName)
{
}

//------------------------------------------------------------------------------
//  For& operator=(const For& f)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the For command.
 *
 * @param f  the For command whose data to use to set values for this one.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
For& For::operator=(const For& f)
{
   if (this == &f)
     return *this;
   
   BranchCommand::operator=(f);
   startValue    = f.startValue;
   endValue      = f.endValue;
   stepSize      = f.stepSize;
   currentValue  = f.currentValue;
   numPasses     = f.numPasses;
   // Wrappers must be recreated, so set to NULL (partial fix for GMT-2525 LOJ: 2012.12.20)
   indexWrapper  = NULL; //f.indexWrapper;
   startWrapper  = NULL; //f.startWrapper;
   incrWrapper   = NULL; //f.incrWrapper;
   endWrapper    = NULL; //f.endWrapper;
   currentPass   = f.currentPass;
   incrPositive  = f.incrPositive;
   indexName     = f.indexName;
   startName     = f.startName;
   endName       = f.endName;
   incrName      = f.incrName;
   
   #ifdef DEBUG_ASSIGNMENT_OP
   MessageInterface::ShowMessage("For::operator=() this=<%p> leaving with following value:\n", this);
   MessageInterface::ShowMessage("   indexName  = '%s'\n", indexName.c_str());
   MessageInterface::ShowMessage("   startName  = '%s'\n", startName.c_str());
   MessageInterface::ShowMessage("   incrName   = '%s'\n", incrName.c_str());
   MessageInterface::ShowMessage("   endName    = '%s'\n", endName.c_str());
   MessageInterface::ShowMessage("   startValue = %.12f\n", startValue);
   MessageInterface::ShowMessage("   stepSize   = %.12f\n", stepSize);
   MessageInterface::ShowMessage("   endValue   = %.12f\n", endValue);
   #endif
   return *this;
}

//------------------------------------------------------------------------------
//  ~For()
//------------------------------------------------------------------------------
/**
 * Destroys the For command.  (destructor)
 */
//------------------------------------------------------------------------------
For::~For()
{
   ClearWrappers();
}

//------------------------------------------------------------------------------
//  bool Append(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the For loop.
 *
 * This method calls the BranchCommand base class method that adds a command
 * to the command sequence that branches off of the main mission sequence.  This
 * extension was needed so that the EndFor command can be set to point back 
 * to the head of the For loop.
 *
 * @param cmd   command to append to the For loop
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool For::Append(GmatCommand *cmd)
{
   if (!BranchCommand::Append(cmd))
     return false;

   // If at the end of a For branch, point that end back to this command.
   if (cmd->GetTypeName() == "EndFor") 
   {
      if ((nestLevel== 0) && (branchToFill != -1))
      {
        cmd->Append(this);
        // for loop is complete; -1 points us back to the main sequence.
        branchToFill = -1;
      }
      else
      {
       --nestLevel;
      }
   }
   if (cmd->GetTypeName() == "For")
   {
      ++nestLevel;
   }

   return true;
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the For loop.
 *
 * @return true if the command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool For::Initialize()
{
   #if DEBUG_FOR_INIT
   MessageInterface::ShowMessage
      ("For::Initialize() this=<%p> '%s' entered\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif   
   // Set references for the wrappers
   #ifdef DEBUG_FOR_PARAMS
      MessageInterface::ShowMessage("   Setting refs for indexWrapper\n");
   #endif
   if (SetWrapperReferences(*indexWrapper) == false)
      return false;
   CheckDataType(indexWrapper, Gmat::REAL_TYPE, "For");
   #ifdef DEBUG_FOR_PARAMS
      MessageInterface::ShowMessage("   Setting refs for startWrapper\n");
   #endif
   if (SetWrapperReferences(*startWrapper) == false)
      return false;
   CheckDataType(startWrapper, Gmat::REAL_TYPE, "For");
   #ifdef DEBUG_FOR_PARAMS
      MessageInterface::ShowMessage("   Setting refs for endWrapper\n");
   #endif
   if (SetWrapperReferences(*endWrapper) == false)
      return false;
   CheckDataType(endWrapper, Gmat::REAL_TYPE, "For");
   #ifdef DEBUG_FOR_PARAMS
      MessageInterface::ShowMessage("   Setting refs for incrWrapper\n");
   #endif
   if (SetWrapperReferences(*incrWrapper) == false)
      return false;   
   CheckDataType(incrWrapper, Gmat::REAL_TYPE, "For");
   
   bool retval = BranchCommand::Initialize();
   
   #if DEBUG_FOR_INIT
   MessageInterface::ShowMessage
      ("For::Initialize() this=<%p> '%s' exiting\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   return retval;
}

//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Execute this For loop.
 *
 * @return true if the command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool For::Execute()
{
   #ifdef DEBUG_FOR_EXE
      MessageInterface::ShowMessage(
         "For::Execute() status: commandComplete = %s, "
         "commandExecuting = %s, branchExecuting = %s\n",
         ((commandComplete) ? "true" : "false"),
         ((commandExecuting) ? "true" : "false"),
         ((branchExecuting) ? "true" : "false") );
   #endif
   bool retval = true;

   if (branchExecuting)
   {
      #ifdef DEBUG_FOR_EXE
         MessageInterface::ShowMessage(
         "... branch still executing -> about to call ExecuteBranch\n");
      #endif
      retval = ExecuteBranch();
      if (!branchExecuting)
      {
         // Branch finished executing; update loop indexWrapper
         currentValue += stepSize;
         currentPass  += 1;
         // We do need currentValue to be incremented; however, we only
         // want to set currentValue on the index variable if it will
         // not pass the endValue (ie. the value of the index variable
         // should never end up going past the endValue)
         bool pastEnd = false;
         if (incrPositive)
            pastEnd = (currentValue > endValue);
         else
            pastEnd = (currentValue < endValue);
         if (!pastEnd)
         {
            indexWrapper->SetReal(currentValue);
            #ifdef DEBUG_FOR_EXE
               MessageInterface::ShowMessage(
               "...... Branch done executing -> currentValue updated to %.4f\n",
               currentValue);
            #endif
         }
         else
         {
            #ifdef DEBUG_FOR_EXE
               MessageInterface::ShowMessage(
               "...... Branch done executing -> currentValue NOT updated, as value would be > endValue\n");
            #endif
         }
      }
   }
   else 
   {
      if (!commandExecuting)
         BranchCommand::Execute();
   
      if (StillLooping())
      {
         branchExecuting = true;
         #ifdef DEBUG_FOR_EXE
            MessageInterface::ShowMessage(
            "... still looping -> about to call ExecuteBranch\n");
         #endif
         retval = ExecuteBranch();
      }
      else
      {
         publisher->FlushBuffers();
         commandComplete  = true;
         commandExecuting = false;
         currentValue     = UNINITIALIZED_VALUE;  // start the loop over
      }
   }
   
   BuildCommandSummary(true);
   return retval;
}

//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Resets the For command to an uninitialized state.
 */
//------------------------------------------------------------------------------
void For::RunComplete()
{
   currentValue = UNINITIALIZED_VALUE;
   
   BranchCommand::RunComplete();
}


//------------------------------------------------------------------------------
// Inherited from GmatBase
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the For.
 *
 * @return clone of the For.
 *
 */
//------------------------------------------------------------------------------
GmatBase* For::Clone() const
{
   return (new For(*this));
}

//------------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//------------------------------------------------------------------------------
void For::Copy(const GmatBase* orig)
{
   operator=(*((For *)(orig)));
}

//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                         const std::string &prefix,
//                                         const std::string &useName)
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
 * @param prefix  Optional prefix appended to the object's name.  (Used to set
 *                indentation for GmatCommands.)
 * @param useName Name that replaces the object's name.  (Not used in
 *                commands.)
 *
 * @return The script line that, when interpreted, defines this For command.
 */
//------------------------------------------------------------------------------
const std::string& For::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
      MessageInterface::ShowMessage("Entering For::GetGeneratingString() this=<%p>\n", this);
      MessageInterface::ShowMessage("... indexName  = '%s'\n", indexName.c_str());
      MessageInterface::ShowMessage("... startName  = '%s'\n", startName.c_str());
      MessageInterface::ShowMessage("... incrName   = '%s'\n", incrName.c_str());
      MessageInterface::ShowMessage("... endName    = '%s'\n", endName.c_str());
      MessageInterface::ShowMessage("... startValue = %.12f\n", startValue);
      MessageInterface::ShowMessage("... stepSize   = %.12f\n", stepSize);
      MessageInterface::ShowMessage("... endValue   = %.12f\n", endValue);
   #endif
   std::stringstream gen;
   gen.precision(4);
   
   gen << indexName << " = ";
   gen << startName << ":";
   gen << incrName  << ":";
   gen << endName;
   
   if (mode == Gmat::NO_COMMENTS)
   {
      generatingString = "For " + gen.str() + ";";
	  InsertCommandName(generatingString);
      return generatingString;
   }
   
   generatingString = prefix + "For " + gen.str() + ";";
   
   // Then call the base class method
   return BranchCommand::GetGeneratingString(mode, prefix, useName);
}

//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name) const
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to the Parameter ref object (used as
 * the index for the For command).
 *
 * @param type  type of reference object to retrieve
 * @param name  name of the reference object to retrieve
 *
 * @return pointer to the requested reference object
 */
//------------------------------------------------------------------------------
GmatBase* For::GetRefObject(const UnsignedInt type,
                            const std::string &name)
{
   // Not handled here -- invoke the next higher GetRefObject call
   return BranchCommand::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
//  bool SetRefObject(const UnsignedInt type,
//                    const std::string &name) const
//------------------------------------------------------------------------------
/**
 * This method sets a pointer to the Parameter reference object
 *
 * @param type  type of reference object to set
 * @param name  name of the reference object to set
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool For::SetRefObject(GmatBase *obj, const UnsignedInt type,
                       const std::string &name)
{
   // Not handled here -- invoke the next higher SetRefObject call
   return BranchCommand::SetRefObject(obj, type, name);
}

//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/*
 * Renames referenced objects
 *
 * @param <type> type of the reference object.
 * @param <oldName> old name of the reference object.
 * @param <newName> new name of the reference object.
 *
 * @return always true to indicate RenameRefObject() was implemented.
 */
//---------------------------------------------------------------------------
bool For::RenameRefObject(const UnsignedInt type,
                          const std::string &oldName,
                          const std::string &newName)
{
   if (indexWrapper) 
   {
      indexWrapper->RenameObject(oldName, newName);
      indexName  = indexWrapper->GetDescription();
   }
   if (startWrapper) 
   {
      startWrapper->RenameObject(oldName, newName);
      startName  = startWrapper->GetDescription();
   }
   if (endWrapper) 
   {
      endWrapper->RenameObject(oldName, newName);
      endName  = endWrapper->GetDescription();
   }
   if (incrWrapper) 
   {
      incrWrapper->RenameObject(oldName, newName);
      incrName  = incrWrapper->GetDescription();
   }
   BranchCommand::RenameRefObject(type, oldName, newName);
   
   return true;
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of reference object types used by the For.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& For::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   return refObjectTypes;
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of reference objects used by the For.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 * 
 * @return the list of object names.
 * 
 */
//------------------------------------------------------------------------------
const StringArray& For::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();
   
   if (type == Gmat::PARAMETER)
   {
      // For array element, remove parenthesis before adding
      std::string objName;
      objName = GmatStringUtil::GetArrayName(indexName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(startName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(endName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(incrName);
      refObjectNames.push_back(objName);
   }
   
   return refObjectNames;
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string For::GetParameterText(const Integer id) const
{
   if ((id >= BranchCommandParamCount) && (id < ForParamCount))
      return PARAMETER_TEXT[id - BranchCommandParamCount];
   return BranchCommand::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer For::GetParameterID(const std::string &str) const
{
   for (Integer i = BranchCommandParamCount; i < ForParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - BranchCommandParamCount])
         return i;
   }
   
   return BranchCommand::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType For::GetParameterType(const Integer id) const
{
   if ((id >= BranchCommandParamCount) && (id < ForParamCount))
      return PARAMETER_TYPE[id - BranchCommandParamCount];
   
   return BranchCommand::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string For::GetParameterTypeString(const Integer id) const
{
   return BranchCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a Real parameter value
 *
 * @param id    Integer ID for the requested parameter
 *
 * @return      the Real value of the requested parameter
 */
//------------------------------------------------------------------------------
Real For::GetRealParameter(const Integer id) const
{
   if (id == START_VALUE)
   {
      #ifdef DEBUG_FOR_REAL
         MessageInterface::ShowMessage(
         "Entering For::GetReal with id = %d, ", id);
         MessageInterface::ShowMessage("And the value is : %.12f\n", startValue);
      #endif
      return startValue;
   }
   if (id == END_VALUE)
   {
      #ifdef DEBUG_FOR_REAL
         MessageInterface::ShowMessage(
         "Entering For::GetReal with id = %d, ", id);
         MessageInterface::ShowMessage("And the value is : %.12f\n", endValue);
      #endif
      return endValue;
   }
   if (id == STEP)
   {
      #ifdef DEBUG_FOR_REAL
         MessageInterface::ShowMessage(
         "Entering For::GetReal with id = %d, ", id);
         MessageInterface::ShowMessage("And the value is : %.12f\n", stepSize);
      #endif
      return stepSize;
   }
   
   return BranchCommand::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 *
 * @return   the Real parameter value after being set
 */
//------------------------------------------------------------------------------
Real For::SetRealParameter(const Integer id, const Real value)
{
   return BranchCommand::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 *
 * @return  the Real value of the requested parameter
 */
//------------------------------------------------------------------------------
Real For::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 *
 * @return   the Real parameter value after being set
 */
//------------------------------------------------------------------------------
Real For::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id  ID for the requested parameter.
 *
 * @return the string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string For::GetStringParameter(const Integer id) const
{
   if      (id == INDEX_NAME)     return indexName;
   else if (id == START_NAME)     return startName;
   else if (id == END_NAME)       return endName;
   else if (id == INCREMENT_NAME) return incrName;
   
   return BranchCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const char *value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id     ID for the requested parameter.
 * @param value  string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool For::SetStringParameter(const Integer id, const char *value)
{
   return SetStringParameter(id, std::string(value));
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id     ID for the requested parameter.
 * @param value  string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool For::SetStringParameter(const Integer id, const std::string &value)
{
   bool retVal = false;
   
   if (id == INDEX_NAME) 
   {
      indexName = value;
      retVal    = true;
   }
   else if (id == START_NAME)
   {
      #ifdef DEBUG_FOR_REAL
         MessageInterface::ShowMessage(
         "In For::SetStrP, setting startName to %s\n", value.c_str());
      #endif
      startName = value;
      retVal = true;
   }
   else if (id == END_NAME)
   {
      #ifdef DEBUG_FOR_REAL
         MessageInterface::ShowMessage(
         "In For::SetStrP, setting endName to %s\n", value.c_str());
      #endif
      endName = value;
      retVal = true;
   }
   else if (id == INCREMENT_NAME)
   {
      incrName = value;
      retVal = true;
   }
   if (retVal) return true;  // is this right?
   return BranchCommand::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    label  label ID for the parameter
 *
 * @return  the string value of the parameter
 */
//------------------------------------------------------------------------------
std::string For::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    label  Integer ID for the parameter
 * @param    value  The new value for the parameter
 */
//------------------------------------------------------------------------------
bool For::SetStringParameter(const std::string &label,
                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// const StringArray& For::GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
/**
 * This method returns an array of object names for the wrappers.
 *
 * @param completeSet   return names that would not ordinarily be returned
 *
 * @return    array of object names for the wrappers
 */
//------------------------------------------------------------------------------
const StringArray& For::GetWrapperObjectNameArray(bool completeSet)
{
   wrapperObjectNames.clear();
   wrapperObjectNames.push_back(indexName);

   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       startName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(startName);
    
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       endName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(endName);
    
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       incrName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(incrName);
    
   return wrapperObjectNames;
}

//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method sets the element wrapper for the object with the input name.
 *
 * @param toWrapper   pointer to the wrapper
 * @param withName    name of the object
 *
 * @return    success flag
 */
//------------------------------------------------------------------------------
bool For::SetElementWrapper(ElementWrapper *toWrapper,
                            const std::string &withName)
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("For::SetElementWrapper() this=<%p> '%s' entered, toWrapper=<%p>, name='%s'\n"
       "   currentFunction=<%p> '%s'\n   callingFunction=<%p> '%s'\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), toWrapper, withName.c_str(),
       currentFunction, currentFunction ? currentFunction->GetFunctionPathAndName().c_str() : "NULL",
       callingFunction, callingFunction ? callingFunction->GetFunctionName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   indexWrapper=<%p>, startWrapper=<%p>, endWrapper=<%p>, incrWrapper=<%p>\n",
       indexWrapper, startWrapper, endWrapper, incrWrapper);
   #endif
   
   bool retval = false;
   
   if (toWrapper == NULL) return false;
   
   // this would be caught by next part, but this message is more meaningful
   if (toWrapper->GetWrapperType() == Gmat::ARRAY_WT)
   {
      throw CommandException("A value of type \"Array\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }
   
   // Check for Wrapper data type, it should be REAL_TYPE
   CheckDataType(toWrapper, Gmat::REAL_TYPE, "For", true);
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("   Setting wrapper \"%s\" on For command\n", withName.c_str());
   #endif
   
   std::vector<ElementWrapper*> wrappersToDelete;
   
   if (indexName == withName)
   {
      if (toWrapper->GetWrapperType() != Gmat::VARIABLE_WT)
      {
         std::string errmsg = "The value of \"" + indexName;
         errmsg            += "\" for field \"Index Name\" on command \"";
         errmsg            += GetTypeName() + "\" is not an allowed value.\n";
         errmsg            += "The allowed values are: [ Variable].";
         throw CommandException(errmsg);
      }
      if (indexWrapper != NULL)
         wrappersToDelete.push_back(indexWrapper);
      indexWrapper = toWrapper;
      retval = true;
   }
   if (startName == withName)
   {
      if (startWrapper != NULL && (startWrapper != endWrapper && startWrapper != incrWrapper))
      {
         if (find(wrappersToDelete.begin(), wrappersToDelete.end(), startWrapper) ==
             wrappersToDelete.end())
         {
            wrappersToDelete.push_back(startWrapper);
            #ifdef DEBUG_WRAPPER_CODE   
            MessageInterface::ShowMessage
               ("   Added startWrapper<%p> to delete list\n", startWrapper);
            #endif
         }
      }
      startWrapper = toWrapper;
      writeInvalidLoopMessage = true;
      retval = true;
   }
   if (endName == withName)
   {
      if (endWrapper != NULL && endWrapper != incrWrapper &&
          (find(wrappersToDelete.begin(), wrappersToDelete.end(), endWrapper) ==
           wrappersToDelete.end()))
      {
         wrappersToDelete.push_back(endWrapper);
         #ifdef DEBUG_WRAPPER_CODE   
         MessageInterface::ShowMessage
            ("   Added endWrapper<%p> to delete list\n", endWrapper);
         #endif
      }
      endWrapper = toWrapper;
      writeInvalidLoopMessage = true;
      retval = true;
   }
   if (incrName == withName)
   {
      if ((incrWrapper != NULL) &&
          (find(wrappersToDelete.begin(), wrappersToDelete.end(), incrWrapper) ==
           wrappersToDelete.end()))
      {
         wrappersToDelete.push_back(incrWrapper);
         #ifdef DEBUG_WRAPPER_CODE   
         MessageInterface::ShowMessage
            ("   Added incrWrapper<%p> to delete list\n", incrWrapper);
         #endif
      }
      incrWrapper = toWrapper;
      writeInvalidLoopMessage = true;
      retval = true;
   }
   
   // Delete old ElementWrappers
   if (callingFunction == NULL)
   {
      for (std::vector<ElementWrapper*>::iterator ewi = wrappersToDelete.begin();
           ewi < wrappersToDelete.end(); ewi++)
      {
         #ifdef DEBUG_WRAPPER_CODE   
         MessageInterface::ShowMessage("   About to delete wrapper<%p>\n", *ewi);
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            ((*ewi), (*ewi)->GetDescription(), "For::SetElementWrapper()", "deleting wrapper");
         #endif
         delete (*ewi);
      }
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("   indexWrapper=<%p>, startWrapper=<%p>, endWrapper=<%p>, incrWrapper=<%p>\n",
       indexWrapper, startWrapper, endWrapper, incrWrapper);
   MessageInterface::ShowMessage
      ("For::SetElementWrapper() this=<%p> '%s' returning %d\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), retval);
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
/**
 * This method clears the element wrappers.
 *
 */
//------------------------------------------------------------------------------
void For::ClearWrappers()
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("For::ClearWrappers() this=<%p> '%s' entered\n   currentFunction=<%p> '%s'\n"
       "   callingFunction=<%p> '%s'\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       currentFunction, currentFunction ? currentFunction->GetFunctionPathAndName().c_str() : "NULL",
       callingFunction, callingFunction ? callingFunction->GetFunctionName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   indexWrapper=<%p>, startWrapper=<%p>, endWrapper=<%p>, incrWrapper=<%p>\n",
       indexWrapper, startWrapper, endWrapper, incrWrapper);
   #endif
   
   std::vector<ElementWrapper*> temp;
   if (indexWrapper)
   {
      temp.push_back(indexWrapper);
      indexWrapper = NULL;
   }
   if (startWrapper)
   {
      if (find(temp.begin(), temp.end(), startWrapper) == temp.end())
         temp.push_back(startWrapper);
      startWrapper = NULL;
   }
   if (endWrapper)
   {
      if (find(temp.begin(), temp.end(), endWrapper) == temp.end())
         temp.push_back(endWrapper);
      endWrapper = NULL;
   }
   if (incrWrapper)
   {
      if (find(temp.begin(), temp.end(), incrWrapper) == temp.end())
         temp.push_back(incrWrapper);
      incrWrapper = NULL;
   }
   
   ElementWrapper *wrapper;
   for (UnsignedInt i = 0; i < temp.size(); ++i)
   {
      wrapper = temp[i];
      if (wrapper)
      {
         #ifdef DEBUG_WRAPPER_CODE   
         MessageInterface::ShowMessage("   About to delete wrapper<%p>\n", wrapper);
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (wrapper, wrapper->GetDescription(), "For::ClearWrappers()",
             GetGeneratingString(Gmat::NO_COMMENTS) + " deleting wrapper");
         #endif
         delete wrapper;
         wrapper = NULL;
      }
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("   indexWrapper=<%p>, startWrapper=<%p>, endWrapper=<%p>, incrWrapper=<%p>\n",
       indexWrapper, startWrapper, endWrapper, incrWrapper);
   MessageInterface::ShowMessage
      ("For::ClearWrappers() this=<%p> '%s' leaving\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool StillLooping()
//------------------------------------------------------------------------------
/**
 * Method to determine whether or not we should still execute the loop
 *
 * @return    flag indicating whether or not we should still execute the loop
 *            (false if counter has passed the specified end value)
 */
//------------------------------------------------------------------------------
bool For::StillLooping()
{
   // initialize the loop, if it's the first time through
   if (currentValue == UNINITIALIZED_VALUE)
   {
      startValue  = startWrapper->EvaluateReal();
      endValue    = endWrapper->EvaluateReal();
      stepSize    = incrWrapper->EvaluateReal();
      if (stepSize >= 0.0)
         incrPositive = true;
      else
         incrPositive = false;
      numPasses   = (int)( floor((endValue - startValue)/stepSize)) + 1;
      currentPass = 1;
      
      #if DEBUG_FOR_EXE
      MessageInterface::ShowMessage
         ("For::StillLooping() startValue=%f, stepSize=%f, endValue=%f\n",
          startValue, stepSize, endValue);
      #endif
      
      if ((stepSize == 0.0) ||
          ((stepSize > 0.0) && (startValue > endValue)) ||
                  ((stepSize < 0.0) && (startValue < endValue)) )
      {
         if (writeInvalidLoopMessage)
         {
            std::string namePartOfMsg = "";
            if (summaryName != "")
            {
               namePartOfMsg = "for For command \"" + summaryName;
               namePartOfMsg += "\" ";
            }
            MessageInterface::ShowMessage
               ("*** WARNING *** Loop start(%d):increment(%d):end(%d) values %s"
                "would result in an infinite loop.  Loop not executed.\n",
                (Integer) startValue, (Integer) stepSize, (Integer) endValue,
                namePartOfMsg.c_str());
            writeInvalidLoopMessage = false;
         }
         commandComplete = true;
         return false;
      }
      else
      {
         currentValue = startValue;
         indexWrapper->SetReal(currentValue);
         commandComplete  = false;
      }
   }
   if (((stepSize > 0.0) && (currentPass <= numPasses)) ||
       ((stepSize < 0.0) && (currentPass <= numPasses)) )
      return true;
   
   // not looping anymore
   return false;
}

