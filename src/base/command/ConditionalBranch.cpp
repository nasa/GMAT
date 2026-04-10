//$Id$
//------------------------------------------------------------------------------
//                                ConditionalBranch
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
// Author:  Wendy Shoan/GSFC
// Created: 2004/09/27
//
/**
 * Definition for the ConditionalBranch command class
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <ctype.h>                // for isalpha
#include "gmatdefs.hpp"
#include "ConditionalBranch.hpp"
#include "Parameter.hpp"
#include "StringUtil.hpp"         // for GetArrayIndex()
#include "MessageInterface.hpp"

//#define DEBUG_CONDITIONS 1
//#define DEBUG_CONDITIONS_INIT 1
//#define DEBUG_CONDBR_GET_GEN_STRING
//#define DEBUG_CONDBR_GET_PARAM
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_OBJECT_MAP

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
ConditionalBranch::PARAMETER_TEXT[ConditionalBranchParamCount - BranchCommandParamCount] =
{
   "NumberOfConditions",
   "NumberOfLogicalOperators",
   "LeftHandStrings",
   "OperatorStrings",
   "RightHandStrings",
   "LogicalOperators",
};

const Gmat::ParameterType
ConditionalBranch::PARAMETER_TYPE[ConditionalBranchParamCount - BranchCommandParamCount] =
{
   Gmat::INTEGER_TYPE,     
   Gmat::INTEGER_TYPE, 
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
};

const std::string
ConditionalBranch::OPTYPE_TEXT[NumberOfOperators] =
{
   "==",
   "~=",
   ">",
   "<",
   ">=",
   "<="
};

const std::string
ConditionalBranch::LOGICAL_OPTYPE_TEXT[NumberOfLogicalOperators] =
{
   "&",
   "|"
};

//------------------------------------------------------------------------------
//  ConditionalBranch(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Creates a ConditionalBranch command.  (constructor)
 */
//------------------------------------------------------------------------------
ConditionalBranch::ConditionalBranch(const std::string &typeStr) :
   BranchCommand      (typeStr),
   numberOfConditions (0),
   numberOfLogicalOps (0)
{
   objectTypeNames.push_back("ConditionalBranch");
}


//------------------------------------------------------------------------------
//  ConditionalBranch(const ConditionalBranch &cb)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a ConditionalBranch command.  (Copy constructor)
 */
//------------------------------------------------------------------------------
ConditionalBranch::ConditionalBranch(const ConditionalBranch &cb) :
   BranchCommand      (cb),
   numberOfConditions (cb.numberOfConditions),
   numberOfLogicalOps (cb.numberOfLogicalOps)
{   
   Integer i = 0;
   for (i=0; i < numberOfConditions; i++)
   {
      lhsList.push_back((cb.lhsList).at(i));
      opStrings.push_back((cb.opStrings).at(i));
      opList.push_back((cb.opList).at(i));
      rhsList.push_back((cb.rhsList).at(i));
   }
   
   for (i=0; i < numberOfLogicalOps; i++)
   {
      logicalOpStrings.push_back((cb.logicalOpStrings).at(i));
      logicalOpList.push_back((cb.logicalOpList).at(i));
   }
   
   isInitialized = false;
}


//------------------------------------------------------------------------------
//  ConditionalBranch& operator=(const ConditionalBranch &cb)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ConditionalBranch command.
 *
 * @param  command from which to get values
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ConditionalBranch& ConditionalBranch::operator=(const ConditionalBranch &cb)
{
   if (this == &cb)
      return *this;
   
   BranchCommand::operator=(cb);
   numberOfConditions = cb.numberOfConditions;
   numberOfLogicalOps = cb.numberOfLogicalOps;
   lhsList.clear();
   opList.clear();
   rhsList.clear();
   ClearWrappers();
   Integer i = 0;
   
   for (i=0; i < numberOfConditions; i++)
   {
      lhsList.push_back((cb.lhsList).at(i));
      opStrings.push_back((cb.opStrings).at(i));
      opList.push_back((cb.opList).at(i));
      rhsList.push_back((cb.rhsList).at(i));
   }
   
   logicalOpList.clear();
   for (i=0; i < numberOfLogicalOps; i++)
   {
      logicalOpStrings.push_back((cb.logicalOpStrings).at(i));
      logicalOpList.push_back((cb.logicalOpList).at(i));
   }
   
   isInitialized = false;
   return *this;
}


//------------------------------------------------------------------------------
//  ~ConditionalBranch()
//------------------------------------------------------------------------------
/**
 * Destroys the ConditionalBranch command.  (destructor)
 */
//------------------------------------------------------------------------------
ConditionalBranch::~ConditionalBranch()
{
   ClearWrappers();
}


//------------------------------------------------------------------------------
//  bool SetCondition(const std::string &lhs, const std::string &operation,
//                    const std::string &rhs, Integer atIndex)
//------------------------------------------------------------------------------
/**
 * This method sets a condition for the ConditionalBranch Command.
 *
 * @param lhs        string representing the left-hand-side
 * @param operation  string representing the operator
 * @param rhs        string representing the right-hand-side
 * @param atIndex    index at which to set the condition
 *
 * @return true if successful; false otherwise.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetCondition(const std::string &lhs, 
                                     const std::string &operation,
                                     const std::string &rhs,
                                     Integer atIndex)
{
   #if DEBUG_CONDITIONS
   MessageInterface::ShowMessage
      ("ConditionalBranch::SetCondition() lhs=(%s), operation=(%s), rhs=(%s)\n",
       lhs.c_str(), operation.c_str(), rhs.c_str());
   #endif
   
   bool   retVal = false;
   OpType ot     = NumberOfOperators;
   
   // determine the operator
   for (Integer i = 0; i < NumberOfOperators; i++)
   {
      #if DEBUG_CONDITIONS
         MessageInterface::ShowMessage
         ("ConditionalBranch::In loop ...  operation is %s, OPTYPE_TEXT is %s\n",
          operation.c_str(), (OPTYPE_TEXT[i]).c_str());
      #endif
      if (operation == OPTYPE_TEXT[i])
      {
         ot = (OpType) i;
         #if DEBUG_CONDITIONS
            MessageInterface::ShowMessage
            ("ConditionalBranch::operation is %s, OPTYPE_TEXT is %s\n",
             operation.c_str(), (OPTYPE_TEXT[i]).c_str());
         #endif
         break;
      }
   }
   
   if (ot == NumberOfOperators)
   {
       std::string errMsg = "The value of \"" + operation + 
            "\" for the relational operator of conditional \"" 
            + typeName +
            "\" is not an allowed value.  The allowed values are: " +
            " [==, ~=, <, >, <=, >=]."; 
       throw CommandException(errMsg);
   }
   
   // put it at the end, if requested (and by default)
   if ((atIndex == -999) || (atIndex == numberOfConditions))
   {
      opStrings.push_back(operation);
      opList.push_back(ot);
      lhsList.push_back(lhs);
      rhsList.push_back(rhs);
      lhsWrappers.push_back(NULL);
      rhsWrappers.push_back(NULL);
      #if DEBUG_CONDITIONS
         MessageInterface::ShowMessage
         ("ConditionalBranch::added condition to end of list\n");
      #endif
      retVal = true;
      numberOfConditions++;
   }
   // assume that logical operators will be added in order
   else if ((atIndex < 0) || (atIndex > numberOfConditions))
   {
      throw CommandException(
            "ConditionalCommand error: condition index out of bounds");
   }
   // otherwise, replace an already-existing condition
   else 
   {
      opStrings.at(atIndex) = operation;
      opList.at(atIndex)    = ot;
      lhsList.at(atIndex)   = lhs;
      rhsList.at(atIndex)   = rhs;
      
      #if DEBUG_CONDITIONS
         MessageInterface::ShowMessage
         ("ConditionalBranch::inserted condition into list\n");
      #endif
      retVal = true;
   }
   
   return retVal;
}


//------------------------------------------------------------------------------
//  bool SetConditionOperator(const std::string &op, Integer atIndex)
//------------------------------------------------------------------------------
/**
 * This method sets a logical operator for the ConditionalBranch Command.
 *
 * @param op      operator
 * @param atIndex index at which to set the operator
 *
 * @return true if successful; false otherwise.
 *
 * @note This method assumes that condition operators are added in order
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetConditionOperator(const std::string &op, 
                                             Integer atIndex)
{
   #if DEBUG_CONDITIONS
   MessageInterface::ShowMessage
      ("ConditionalBranch::SetConditionOperator()op=%s, atIndex\n",
       op.c_str(), atIndex);
   #endif
   
   bool          retVal = false;
   LogicalOpType ot     = NumberOfLogicalOperators;
   
   // determine the logical operator
   for (Integer i = 0; i < NumberOfLogicalOperators; i++)
   {
      if (op == LOGICAL_OPTYPE_TEXT[i])
      {
         ot = (LogicalOpType) i;
         break;
      }
   }
   
   if (ot == NumberOfLogicalOperators)
   {
       std::string errMsg = "The value of \"" + op + 
            "\" for the logical operator of conditional \"" 
            + typeName +
            "\" is not an allowed value.  The allowed values are: " +
            " [&,|]."; 
       throw CommandException(errMsg);
   }
   
   if ((atIndex == -999) || (atIndex == numberOfLogicalOps))
   {
      logicalOpStrings.push_back(op);
      logicalOpList.push_back(ot);
      retVal = true;
      numberOfLogicalOps++;
   }
   // assume that logical operators will be added in order
   else if ((atIndex < 0) || (atIndex > numberOfLogicalOps))
   {
      throw CommandException(
            "ConditionalCommand error: logical operator index out of bounds");
   }
   // put it at the end, if requested (and by default)
   // otherwise, replace an already-existing logical operator
   else 
   {
      logicalOpStrings.at(atIndex) = op;
      logicalOpList.at(atIndex)    = ot;
      retVal                       = true;
   }
   return retVal;
}

//------------------------------------------------------------------------------
//  bool RemoveCondition(Integer atIndex)
//------------------------------------------------------------------------------
/**
 * Removes the condition for the command, at index atIndex.
 * 
 * @param <atIndex>   index in the list from which to remove the condition.
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::RemoveCondition(Integer atIndex)
{
   if ((atIndex < 0) || (atIndex >= numberOfConditions))
      throw CommandException(
            "RemoveCondition error - condition index out of bounds.");
   ElementWrapper *ew;
   lhsList.erase(lhsList.begin() + atIndex);
   ew = lhsWrappers.at(atIndex);
   lhsWrappers.erase(lhsWrappers.begin() + atIndex);
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Remove
      (ew, ew->GetDescription(), "ConditionalBranch::RemoveCondition()",
       GetGeneratingString(Gmat::NO_COMMENTS) + " deleting lhsWrapper");
   #endif
   delete ew;
   opStrings.erase(opStrings.begin() + atIndex);
   opList.erase(opList.begin() + atIndex);
   rhsList.erase(rhsList.begin() + atIndex);
   ew = rhsWrappers.at(atIndex);
   rhsWrappers.erase(rhsWrappers.begin() + atIndex);
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Remove
      (ew, ew->GetDescription(), "ConditionalBranch::RemoveCondition()",
       GetGeneratingString(Gmat::NO_COMMENTS) + " deleting rhsWrapper");
   #endif
   delete ew;
   numberOfConditions--;
   return true;
}

//------------------------------------------------------------------------------
//  bool RemoveConditionOperator(Integer atIndex)
//------------------------------------------------------------------------------
/**
 * Removes the logical operator for the command, at index atIndex.
 * 
 * @param <atIndex>   where in the list to remove the logical operator from.
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::RemoveConditionOperator(Integer atIndex)
{
   if ((atIndex < 0) || (atIndex >= numberOfLogicalOps))
      throw CommandException(
            "RemoveConditionOperator error - condition index out of bounds.");
   logicalOpStrings.erase(logicalOpStrings.begin() + atIndex);
   logicalOpList.erase(logicalOpList.begin() + atIndex);
   numberOfLogicalOps--;
   return true;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the conditional branch command.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::Initialize()
{
   bool retval = BranchCommand::Initialize();
   
   // Set references for the wrappers   
   #ifdef DEBUG_CONDITIONS_INIT
   MessageInterface::ShowMessage
      ("\nConditionalBranch::Initialize() this='%s' entered, Setting refs for "
       "%d lhs wrappers\n", GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       lhsWrappers.size());
   #endif
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps();
   #endif
   
   // Handle LHS references to clones
   for (std::vector<ElementWrapper*>::iterator i = lhsWrappers.begin();
        i < lhsWrappers.end(); i++)
   {
      std::string wrapperDesc = (*i)->GetDescription();
      #ifdef DEBUG_CONDITIONS_INIT
      MessageInterface::ShowMessage("   wrapper desc = '%s'\n", wrapperDesc.c_str());
      #endif
      Gmat::WrapperDataType wrapperType = (*i)->GetWrapperType();
      
      if (wrapperType == Gmat::PARAMETER_WT)
      {
         GmatBase *mapObj = FindObject(wrapperDesc);
         if (mapObj == NULL)
         {
            std::string msg = "Object named \"" + wrapperDesc +
               "\" cannot be found for the command '" +
               GetGeneratingString(Gmat::NO_COMMENTS) + "'";
            #ifdef DEBUG_CONDITIONS_INIT
            MessageInterface::ShowMessage("**** ERROR **** %s\n", msg.c_str());
            #endif
            throw CommandException(msg);
         }
         
         if (!mapObj->IsOfType("Parameter"))
            throw CommandException("Parameter type mismatch for " + mapObj->GetName());
         
         Parameter *param = (Parameter *)mapObj;
         
         if (param->NeedExternalClone())
            HandleReferencesToClones(param);
      }
   }
   
   // Now set LHS wrapper references
   for (std::vector<ElementWrapper*>::iterator i = lhsWrappers.begin();
        i < lhsWrappers.end(); i++)
   {
      #ifdef DEBUG_CONDITIONS_INIT
      MessageInterface::ShowMessage
         ("   wrapper desc = '%s'\n", (*i)->GetDescription().c_str());
      #endif
      
      if (SetWrapperReferences(*(*i)) == false)
         return false;      
      
      CheckDataType((*i), Gmat::REAL_TYPE, "Conditional Command");
   }
   
   #ifdef DEBUG_CONDITIONS_INIT
   MessageInterface::ShowMessage
      ("ConditionalBranch::Initialize() Setting refs for %d rhs wrappers\n",
       rhsWrappers.size());
   #endif
   
   // Handle RHS references to clones
   for (std::vector<ElementWrapper*>::iterator i = rhsWrappers.begin();
        i < rhsWrappers.end(); i++)
   {
      std::string wrapperDesc = (*i)->GetDescription();
      #ifdef DEBUG_CONDITIONS_INIT
      MessageInterface::ShowMessage("   wrapper desc = '%s'\n", wrapperDesc.c_str());
      #endif
      Gmat::WrapperDataType wrapperType = (*i)->GetWrapperType();
      
      if (wrapperType == Gmat::PARAMETER_WT)
      {
         GmatBase *mapObj = FindObject(wrapperDesc);
         if (mapObj == NULL)
         {
            std::string msg = "Object named \"" + wrapperDesc +
               "\" cannot be found for the command '" +
               GetGeneratingString(Gmat::NO_COMMENTS) + "'";
            #ifdef DEBUG_CONDITIONS_INIT
            MessageInterface::ShowMessage("**** ERROR **** %s\n", msg.c_str());
            #endif
            throw CommandException(msg);
         }
         
         if (!mapObj->IsOfType("Parameter"))
            throw CommandException("Parameter type mismatch for " + mapObj->GetName());
         
         Parameter *param = (Parameter *)mapObj;
         
         if (param->NeedExternalClone())
            HandleReferencesToClones(param);
      }
   }
   
   // Set RHS wrapper references
   for (std::vector<ElementWrapper*>::iterator j = rhsWrappers.begin();
        j < rhsWrappers.end(); j++)
   {
      #ifdef DEBUG_CONDITIONS_INIT
      MessageInterface::ShowMessage
         ("   wrapper desc = '%s'\n", (*j)->GetDescription().c_str());
      #endif
      
      if (SetWrapperReferences(*(*j)) == false)
         return false;
      
      CheckDataType((*j), Gmat::REAL_TYPE, "Conditional Command");
   }
   
   #ifdef DEBUG_CONDITIONS_INIT
   MessageInterface::ShowMessage
      ("ConditionalBranch::Initialize() returning %d\n", retval);
   #endif
   
   return retval;
}


//---------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//                      const std::string &oldName, const std::string &newName)
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
bool ConditionalBranch::RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ConditionalBranch::RenameRefObject() type=%d, oldName=%s, "
       "newName=%s\n", type, oldName.c_str(), newName.c_str());
   #endif
   for (UnsignedInt i=0; i<lhsList.size(); i++)
   {
      if (lhsList[i] == oldName) lhsList[i] = newName;
      if (lhsWrappers.at(i) != NULL)
      {
         (lhsWrappers.at(i))->RenameObject(oldName, newName);
         lhsList.at(i) = (lhsWrappers.at(i))->GetDescription();
      }
   }
   for (UnsignedInt j=0; j<rhsList.size(); j++)
   {
      if (rhsList[j] == oldName) rhsList[j] = newName;
      if (rhsWrappers.at(j) != NULL)
      {
         (rhsWrappers.at(j))->RenameObject(oldName, newName);
         rhsList.at(j) = (rhsWrappers.at(j))->GetDescription();
      }
   }

   BranchCommand::RenameRefObject(type, oldName, newName);
   
   return true;
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Achieve.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& ConditionalBranch::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref objects used by the Achieve.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 * 
 * @return the list of object names.
 * 
 */
//------------------------------------------------------------------------------
const StringArray& ConditionalBranch::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();
   
   if (type == Gmat::PARAMETER)
   {
      // For array element, remove parenthesis before adding
      std::string objName;
      // LHS object names
      for (StringArray::iterator i = lhsList.begin(); i < lhsList.end(); i++)
      {
         objName = GmatStringUtil::GetArrayName((*i));
         refObjectNames.push_back(objName);
      }
      // RHS object names
      for (StringArray::iterator i = rhsList.begin(); i < rhsList.end(); i++)
      {
         objName = GmatStringUtil::GetArrayName((*i));
         refObjectNames.push_back(objName);
      }
   }
   
   return refObjectNames;
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the ConditionalBranch Command.
 *
 * @param <obj>   pointer to the reference object
 * @param <type>  type of the reference object 
 * @param <name>  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name)
{
   // Not handled here -- invoke the next higher SetRefObject call
   return BranchCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the ConditionalBranch Command.
 *
 * @param <type>  type of the reference object requested
 * @param <name>  name of the reference object requested
 * @param <index> index into the array of reference objects
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ConditionalBranch::GetRefObject(const UnsignedInt type,
                                          const std::string &name,
                                          const Integer index)
{
   // Not handled here -- invoke the next higher GetRefObject call
   return BranchCommand::GetRefObject(type, name, index);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the ConditionalBranch Command.
 *
 * @param <obj>   pointer to the reference object
 * @param <type>  type of the reference object 
 * @param <name>  name of the reference object
 * @param <index> index into the array of reference objects (where to put this
 *                 one)
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name, const Integer index)
{
   // Not handled here -- invoke the next higher SetRefObject call
   return BranchCommand::SetRefObject(obj, type, name, index);
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
 *
 */
//------------------------------------------------------------------------------
std::string ConditionalBranch::GetParameterText(const Integer id) const
{
   if (id >= BranchCommandParamCount && id < ConditionalBranchParamCount)
      return PARAMETER_TEXT[id - BranchCommandParamCount];
   return BranchCommand::GetParameterText(id);
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
 *
 */
//------------------------------------------------------------------------------
Integer ConditionalBranch::GetParameterID(const std::string &str) const
{
   for (Integer i = BranchCommandParamCount; i < ConditionalBranchParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - BranchCommandParamCount])
         return i;
   }
   
   return BranchCommand::GetParameterID(str);
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
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ConditionalBranch::GetParameterType(const Integer id) const
{
   if (id >= BranchCommandParamCount && id < ConditionalBranchParamCount)
      return PARAMETER_TYPE[id - BranchCommandParamCount];
   
   return BranchCommand::GetParameterType(id);
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
 *
 */
//------------------------------------------------------------------------------
std::string ConditionalBranch::GetParameterTypeString(const Integer id) const
{
   return ConditionalBranch::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer ConditionalBranch::GetIntegerParameter(const Integer id) const
{
   if (id == NUMBER_OF_CONDITIONS)          return numberOfConditions;
   if (id == NUMBER_OF_LOGICAL_OPS)         return numberOfLogicalOps;
   
   return BranchCommand::GetIntegerParameter(id); 
}


//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer ConditionalBranch::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method gets a string parameter value of a StringArray, for the input
 * parameter ID, at the input index into the array.
 *
 * @param <id>    ID for the requested parameter.
 * @param <index> index into the StringArray.
 *
 * @return  string value of the requested StringArray parameter, at the 
 *          requested index.
 *
 */
//------------------------------------------------------------------------------
std::string ConditionalBranch::GetStringParameter(const Integer id,
                                                  const Integer index) const
{
   std::string errorString = "ConditionalCommand error: Requested index ";
   errorString += index;
   errorString += " is out of bounds for ";
   if (id == LEFT_HAND_STRINGS)
   {
      if (index < 0 || index >= (Integer) lhsList.size())
      {
         errorString += "left hand side string list.";
         throw CommandException(errorString);
      }
      return (lhsList.at(index));
   }
   if (id == OPERATOR_STRINGS)   
   {
      if (index < 0 || index >= (Integer) opStrings.size())
      {
         errorString += "operator string list.";
         throw CommandException(errorString);
      }
      return (opStrings.at(index));
   }
   if (id == RIGHT_HAND_STRINGS) 
   {
      if (index < 0 || index >= (Integer) rhsList.size())
      {
         errorString += "right hand side string list.";
         throw CommandException(errorString);
      }
      return (rhsList.at(index));
   }
   if (id == LOGICAL_OPERATORS)  
   {
      if (index < 0 || index >= (Integer) logicalOpStrings.size())
      {
         errorString += "logical operator string list.";
         throw CommandException(errorString);
      }
      return (logicalOpStrings.at(index));
   }
   
   return BranchCommand::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value,
//                          const Integer index) const
//------------------------------------------------------------------------------
 /**
 * This method sets a string parameter value of a StringArray, for the input
 * parameter ID, at the input index into the array.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> value to set the parameter to
 * @param <index> index into the StringArray.
 *
 * @return  success or failure of the operation.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index)
{
   std::string errorString = "ConditionalCommand error: Requested index ";
   errorString += index;
   errorString += " is out of bounds for ";
   ElementWrapper *ew;
   if (id == LEFT_HAND_STRINGS)
   {
      if (index < 0 || index >= (Integer) lhsList.size())
      {
         errorString += "left hand side string list.";
         throw CommandException(errorString);
      }
      lhsList.at(index) = value;
      ew = lhsWrappers.at(index);
      lhsWrappers.at(index) = NULL;
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (ew, ew->GetDescription(), "ConditionalBranch::SetStringParameter()",
          GetGeneratingString(Gmat::NO_COMMENTS) + " deleting lhsWrapper");
      #endif
      delete ew;
      return true;
   }
   if (id == OPERATOR_STRINGS)   
   {
      if (index < 0 || index >= (Integer) opStrings.size())
      {
         errorString += "operator string list.";
         throw CommandException(errorString);
      }
      opStrings.at(index) = value;
      return true;
   }
   if (id == RIGHT_HAND_STRINGS) 
   {
      if (index < 0 || index >= (Integer) rhsList.size())
      {
         errorString += "right hand side string list.";
         throw CommandException(errorString);
      }
      rhsList.at(index) = value;
      ew = rhsWrappers.at(index);
      rhsWrappers.at(index) = NULL;
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (ew, ew->GetDescription(), "ConditionalBranch::SetStringParameter()",
          GetGeneratingString(Gmat::NO_COMMENTS) + " deleting rhsWrapper");
      #endif
      delete ew;
      return true;
   }
   if (id == LOGICAL_OPERATORS)  
   {
      if (index < 0 || index >= (Integer) logicalOpStrings.size())
      {
         errorString += "logical operator string list.";
         throw CommandException(errorString);
      }
      logicalOpStrings.at(index) = value;
      return true;
   }
   
   return BranchCommand::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label, 
//                                 const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method gets a string parameter value of a StringArray, for the input
 * parameter label, at the input index into the array.
 *
 * @param <label> label for the requested parameter.
 * @param <index> index into the StringArray.
 *
 * @return  string value of the requested StringArray parameter, at the 
 *          requested index.
 *
 */
//------------------------------------------------------------------------------
std::string ConditionalBranch::GetStringParameter(const std::string &label,
                                        const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value,
//                          const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method gets a string parameter value of a StringArray, for the input
 * parameter label, at the input index into the array.
 *
 * @param <label> label for the requested parameter.
 * @param <value> value to set the parameter to
 * @param <index> index into the StringArray.
 *
 * @return  success or failure of the operation.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetStringParameter(const std::string &label, 
                                        const std::string &value,
                                        const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& 
ConditionalBranch::GetStringArrayParameter(const Integer id) const
{
   if (id == LEFT_HAND_STRINGS)  return lhsList;
   if (id == OPERATOR_STRINGS)   return opStrings;
   if (id == RIGHT_HAND_STRINGS) return rhsList;
   if (id == LOGICAL_OPERATORS)  return logicalOpStrings;
   
   return BranchCommand::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& 
ConditionalBranch::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
/**
 * This method returns a list of names of the wrapper objects.
 *
 * @param completeSet   return names that would not ordinarily be returned
 *
 * @return  list of names of the wrapper objects
 *
 */
//------------------------------------------------------------------------------
const StringArray& ConditionalBranch::GetWrapperObjectNameArray(bool completeSet)
{
   wrapperObjectNames.clear();

   for (StringArray::iterator i = lhsList.begin(); i < lhsList.end(); i++)
   {
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
          (*i)) == wrapperObjectNames.end())
         wrapperObjectNames.push_back((*i));
   }
   
   for (StringArray::iterator j = rhsList.begin(); j < rhsList.end(); j++)
   {
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
          (*j)) == wrapperObjectNames.end())
         wrapperObjectNames.push_back((*j));
   }
   
   #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage
         ("ConditionalBranch::GetWrapperObjectNameArray() %s wrapper names are:\n",
          GetTypeName().c_str());
      for (Integer ii=0; ii < (Integer) wrapperObjectNames.size(); ii++)
         MessageInterface::ShowMessage("      %s\n", wrapperObjectNames[ii].c_str());
   #endif
      
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method sets the element wrapper with the specified name
 *
 * @param toWrapper   pointer to wrapper
 * @param withName    name of the object that the wrapper is for
 *
 * @return  true if successful; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetElementWrapper(ElementWrapper *toWrapper, 
                                          const std::string &withName)
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("ConditionalBranch::SetElementWrapper() this=<%p> '%s' entered\n",
       this, GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   bool retval = false;
   ElementWrapper *ew;

   if (toWrapper == NULL) return false;
   // this would be caught by next part, but this message is more meaningful
   if (toWrapper->GetWrapperType() == Gmat::ARRAY_WT)
   {
      throw CommandException("A value of type \"Array\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }
   CheckDataType(toWrapper, Gmat::REAL_TYPE, "ConditionalBranch", true);
   
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("ConditionalBranch::SetElementWrapper() Setting wrapper <%p> '%s'",
       toWrapper, withName.c_str());
   #endif
   Integer sz = lhsList.size();
   for (Integer i = 0; i < sz; i++)
   {
      if (lhsList.at(i) == withName)
      {
         #ifdef DEBUG_WRAPPER_CODE   
         MessageInterface::ShowMessage(
            "   Found wrapper name \"%s\" in lhsList\n", withName.c_str());
         #endif
         if (lhsWrappers.at(i) != NULL)
         {
            ew = lhsWrappers.at(i);
            lhsWrappers.at(i) = toWrapper;
            
            // Delete if wrapper name not found in the rhsList
            if (find(rhsList.begin(), rhsList.end(), withName) == rhsList.end())
            {
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Remove
                  (ew, ew->GetDescription(), "ConditionalBranch::SetElementWrapper()",
                   GetGeneratingString(Gmat::NO_COMMENTS) + " deleting lhsWrapper");
               #endif
               delete ew;
               ew = NULL;
            }
         }
         else lhsWrappers.at(i) = toWrapper;
         retval = true;
      }
   }
   sz = rhsList.size();
   for (Integer i = 0; i < sz; i++)
   {
      if (rhsList.at(i) == withName)
      {
         #ifdef DEBUG_WRAPPER_CODE   
         MessageInterface::ShowMessage(
            "   Found wrapper name \"%s\" in rhsList\n", withName.c_str());
         #endif
         if (rhsWrappers.at(i) != NULL)
         {
            ew = rhsWrappers.at(i);
            rhsWrappers.at(i) = toWrapper;
            
            // Delete if wrapper name not found in the lhsList
            if (find(lhsList.begin(), lhsList.end(), withName) == lhsList.end())
            {
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Remove
                  (ew, ew->GetDescription(), "ConditionalBranch::SetElementWrapper()",
                   GetGeneratingString(Gmat::NO_COMMENTS) + " deleting rhsWrapper");
               #endif
               delete ew;
               ew = NULL;
            }
         }
         else rhsWrappers.at(i) = toWrapper;
         retval = true;
      }
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
/**
 * This method clears all of the wrappers
 *
 */
//------------------------------------------------------------------------------
void ConditionalBranch::ClearWrappers()
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("ConditionalBranch::ClearWrappers() this=<%p> '%s' entered\n",
       this, GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   std::vector<ElementWrapper*> temp;
   Integer sz = (Integer) lhsWrappers.size();
   for (Integer i=0; i<sz; i++)
   {
      if (lhsWrappers.at(i) != NULL)
      {
         // Add lhs wrapper if it has not already added
         if (find(temp.begin(), temp.end(), lhsWrappers.at(i)) == temp.end())
            temp.push_back(lhsWrappers.at(i));
         
         lhsWrappers.at(i) = NULL;
      }
   }
   
   sz = (Integer) rhsWrappers.size();
   for (Integer i=0; i<sz; i++)
   {
      if (rhsWrappers.at(i) != NULL)
      {
         // Add rhs wrapper if it has not already added
         if (find(temp.begin(), temp.end(), rhsWrappers.at(i)) == temp.end())
            temp.push_back(rhsWrappers.at(i));
         
         rhsWrappers.at(i) = NULL;
      }
   }
   
   ElementWrapper *ew;
   for (UnsignedInt i = 0; i < temp.size(); ++i)
   {
      ew = temp[i];
      
      if (ew != NULL)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (ew, ew->GetDescription(), "ConditionalBranch::ClearWrappers()",
             GetTypeName() + " deleting lhsWrapper");
         #endif
         delete ew;
      }
      
      ew = NULL;
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage("ConditionalBranch::ClearWrappers() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  bool EvaluateCondition(Integer which)
//------------------------------------------------------------------------------
/**
 * This method evaluates the specified condition.
 *
 * @param which index into the array(s) of conditions
 *
 * @return false if which is out of bounds; result of evaluating the
 *         condition, otherwise.
 *
 * @note This method currently assumes that the rhs of the condition is a
 *       Real number.  In the future, we will need to allow for the possibility
 *       of the rhs of the condition being another parameter, which will need
 *       to be evaluated.
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::EvaluateCondition(Integer which)
{
   #ifdef DEBUG_CONDITIONS
   MessageInterface::ShowMessage
      ("ConditionalBranch::EvaluateCondition() entered; which = %d\n   "
       "Number of Conditions: %d\n", which, numberOfConditions);      
   MessageInterface::ShowMessage
      ("      lhs wrapper = %s        rhsWrapper = %s\n", 
       (lhsList.at(which)).c_str(), (rhsList.at(which)).c_str());
   #endif
   if ((which < 0) || (which >= numberOfConditions))
   {
      #ifdef DEBUG_CONDITIONS
         MessageInterface::ShowMessage(
         "ConditionalBranch::EvaluateCondition() - returning with FALSE!!!\n");
      #endif
      return false;
   }
   if ((lhsWrappers.at(which) == NULL) || (rhsWrappers.at(which) == NULL))
   {
      std::string errmsg = "Error evaluating condition \"" + lhsList.at(which);
      errmsg += " " + opStrings.at(which);
      errmsg += " " + rhsList.at(which);
      errmsg += "\" - wrapper is NULL\n";
      throw CommandException(errmsg);
   }
   Real lhsValue = (lhsWrappers.at(which))->EvaluateReal();   
   Real rhsValue = (rhsWrappers.at(which))->EvaluateReal();
   
   #ifdef DEBUG_CONDITIONS
      MessageInterface::ShowMessage(
         "   lhs = %.18f,  rhs = %.18f\n", lhsValue, rhsValue);
      MessageInterface::ShowMessage(
         "   lhs - rhs = %.18f\n", (lhsValue - rhsValue));
      MessageInterface::ShowMessage(
         "   operator = %d\n", (Integer) opList.at(which));
      MessageInterface::ShowMessage(
         "   operator (as string) = %s\n", 
         (OPTYPE_TEXT[(Integer) opList.at(which)]).c_str());
   #endif

   switch (opList.at(which))
   {
      case EQUAL_TO:
         return (lhsValue == rhsValue);
         break;
      case NOT_EQUAL:
         return (lhsValue != rhsValue);
         break;
      case GREATER_THAN:
         return (lhsValue > rhsValue);
         break;
      case LESS_THAN:
         #ifdef DEBUG_CONDITIONS
         if (lhsValue < rhsValue)
            MessageInterface::ShowMessage("   returning TRUE .......\n");
         else
            MessageInterface::ShowMessage("   returning FALSE .......\n");
         #endif
         return (lhsValue < rhsValue);
         break;
      case GREATER_OR_EQUAL:
         return (lhsValue >= rhsValue);
         break;
      case LESS_OR_EQUAL:
         return (lhsValue <= rhsValue);
         break;
      default:
         return false;
         break;
   }
   return false; 
   
}

//------------------------------------------------------------------------------
//  bool EvaluateAllConditions()
//------------------------------------------------------------------------------
/**
 * This method evaluates the entire list of conditions, using their logical
 * operators.  Evaluation is from left to right.
 *
 * @return result of evaluating the entire list of conditions, from left to
 *         right.
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::EvaluateAllConditions()
{
   #ifdef DEBUG_CONDITIONS
      MessageInterface::ShowMessage(
         "   \nEntering EvaluateAllConditions with number of conditions = %d\n",
         numberOfConditions);
   #endif
   if (numberOfConditions == 0)
      throw CommandException(
         "Error in conditional statement - no conditions specified.");
   
   if (numberOfConditions != (numberOfLogicalOps + 1))
      throw CommandException(
         "conditional statement incorrect - too few/many logical operators");
   
   // divide into sets of higher-precedence AND operators, then OR them
   // @todo Create a LogicTree for this type, allowing use of parentheses as well
   bool         done            = false;
   Integer      current         = 0;
   bool         soFar           = false;
   bool         setOfCmdsFound  = false;
   bool         evalAnds        = true;
   IntegerArray andConditions;
   while (!done)
   {
      andConditions.clear();
      setOfCmdsFound  = false;
      evalAnds        = true;
      while (!setOfCmdsFound)
      {
         andConditions.push_back(current);
         // are we at the end of the list of conditions?
         if (current == (numberOfConditions-1))
         {
            setOfCmdsFound = true;
            done           = true;
         }
         else
         {
            // or is the operator an OR?
            if (logicalOpList.at(current) == OR) setOfCmdsFound = true;
            current++;
         }
      }
      // found an OR, so evaluate the AND conditions
      for (Integer ii = 0; ii < (Integer) andConditions.size(); ii++)
      {
         evalAnds = evalAnds && EvaluateCondition(andConditions.at(ii));
      }
      // previous result OR current result from group of AND conditions
      soFar = soFar || evalAnds;
   } //  while !done
   andConditions.clear();
   
   #ifdef DEBUG_CONDITIONS
   if (soFar)
      MessageInterface::ShowMessage("   all are TRUE .......\n");
   else
      MessageInterface::ShowMessage("   some are FALSE .......\n");
   #endif
   
   return soFar;
}

//------------------------------------------------------------------------------
//  std::string GetConditionalString()
//------------------------------------------------------------------------------
/**
 * This method builds the string that generates the condition list.
 *
 * @return The string description of the conditions.
 */
//------------------------------------------------------------------------------
std::string ConditionalBranch::GetConditionalString()
{
   #ifdef DEBUG_CONDBR_GET_GEN_STRING
      MessageInterface::ShowMessage("Entering ConditionalBranch::GetConditionalString\n");
      MessageInterface::ShowMessage("... number of conditions = %d\n", 
         numberOfConditions);
   #endif
   std::string cond;
   
   if ((lhsList.size() == 0) || (rhsList.size() == 0))
      throw CommandException("Conditional command is missing its conditions!");
   
   // The first condition
   cond = lhsList[0] + " " + opStrings[0] + " " + rhsList[0];
   
   for (Integer i = 1; i < numberOfConditions; i++)
   {
      #ifdef DEBUG_CONDBR_GET_GEN_STRING
         MessageInterface::ShowMessage("Now adding condition %d to the string\n", i);
         MessageInterface::ShowMessage("The logical operator = \"%s\"\n", logicalOpStrings[i-1].c_str());
         MessageInterface::ShowMessage("The left-hand-side = \"%s\"\n", lhsList[i].c_str());
         MessageInterface::ShowMessage("The operator = \"%s\"\n", opStrings[i].c_str());
         MessageInterface::ShowMessage("The right-hand-side = \"%s\"\n", rhsList[i].c_str());
      #endif
      cond += " " + logicalOpStrings[i-1] + " ";
      cond += lhsList[i] + " " + opStrings[i] + " " + rhsList[i];
   }

   #ifdef DEBUG_CONDBR_GET_GEN_STRING
      MessageInterface::ShowMessage("Exiting ConditionalBranch::GetConditionalString\n");
      MessageInterface::ShowMessage("... returning cond = %s\n", cond.c_str());
   #endif
   return cond;
}
