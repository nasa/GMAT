//$Id$
//------------------------------------------------------------------------------
//                                 Achieve
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
// Created: 2004/01/26
//
/**
 * Definition for the Achieve command class
 */
//------------------------------------------------------------------------------


#include "Achieve.hpp"
#include "StringUtil.hpp"  // for ToReal()
#include <sstream>
#include "MessageInterface.hpp"

//#define DEBUG_ACHIEVE_PARSE
//#define DEBUG_ACHIEVE_PARAMS
//#define DEBUG_ACHIEVE_INIT
//#define DEBUG_ACHIEVE_EXEC
//#define DEBUG_WRAPPER_CODE


//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
const std::string
   Achieve::PARAMETER_TEXT[AchieveParamCount - SolverSequenceCommandParamCount] =
   {
      "TargeterName",
      "Goal",
      "GoalValue",
      "Tolerance",
      "AchievedValue"
   };
   
const Gmat::ParameterType
   Achieve::PARAMETER_TYPE[AchieveParamCount - SolverSequenceCommandParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::REAL_TYPE
   };


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Achieve()
//------------------------------------------------------------------------------
/**
 * Creates an Achieve command.  (default constructor)
 */
//------------------------------------------------------------------------------
Achieve::Achieve() :
   SolverSequenceCommand   ("Achieve"),
   targeterName            (""),
   goalName                (""),
   goal                    (NULL),
   achieveName             (""),
   achieve                 (NULL),
   toleranceName           ("0.1"),
   tolerance               (NULL),
   goalId                  (-1),
   targeter                (NULL),
   targeterDataFinalized   (false),
   achievedValue           (0.0)
{
   objectTypeNames.push_back("SolverCommand");
   objectTypeNames.push_back("Achieve");
   settables.push_back("Tolerance"); 
   parameterCount = AchieveParamCount;
}


//------------------------------------------------------------------------------
//  ~Achieve()
//------------------------------------------------------------------------------
/**
 * Destroys the Achieve command.  (destructor)
 */
//------------------------------------------------------------------------------
Achieve::~Achieve()
{
   ClearWrappers();
}

    
//------------------------------------------------------------------------------
//  Achieve(const Achieve& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Achieve command.  (Copy constructor)
 *
 * @param t Command that is replicated here.
 */
//------------------------------------------------------------------------------
Achieve::Achieve(const Achieve& t) :
   SolverSequenceCommand             (t),
   targeterName            (t.targeterName),
   goalName                (t.goalName),
   goal                    (NULL),
   achieveName             (t.achieveName),
   achieve                 (NULL),
   toleranceName           (t.toleranceName),
   tolerance               (NULL),
   goalId                  (t.goalId),
   targeter                (NULL),
   targeterDataFinalized   (false),
   achievedValue           (0.0)
{
   parameterCount = AchieveParamCount;
}


//------------------------------------------------------------------------------
//  Achieve& operator=(const Achieve& t)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Achieve command.
 *
 * @param t Command that is replicated here.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Achieve& Achieve::operator=(const Achieve& t)
{
   if (this == &t)
      return *this;
    
   SolverSequenceCommand::operator=(t);
   targeterName          = t.targeterName;
   goalName              = t.goalName;
   goal                  = NULL;
   achieveName           = t.achieveName;
   achieve               = NULL;
   toleranceName         = t.toleranceName;
   tolerance             = NULL;
   goalId                = t.goalId;
   targeter              = NULL;
   targeterDataFinalized = false;

   return *this;
}



//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Achieve.
 *
 * @return clone of the Achieve.
 */
//------------------------------------------------------------------------------
GmatBase* Achieve::Clone() const
{
   return (new Achieve(*this));
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Achieve::RenameRefObject(const UnsignedInt type,
                              const std::string &oldName,
                              const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (targeterName == oldName)
         targeterName = newName;
   }
   // make sure the wrappers know to rename any objects they may be using
   if (goal)
   {
      goal->RenameObject(oldName, newName);
      goalName = goal->GetDescription();
   }
   if (achieve)
   {
      achieve->RenameObject(oldName, newName);
      achieveName = achieve->GetDescription();
   }
   if (tolerance)
   {
      tolerance->RenameObject(oldName, newName);
      toleranceName = tolerance->GetDescription();
   }
   
   return SolverSequenceCommand::RenameRefObject(type, oldName, newName);
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
const ObjectTypeArray& Achieve::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SOLVER);
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
const StringArray& Achieve::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::SOLVER)
   {
      refObjectNames.push_back(targeterName);
   }
   else if (type == Gmat::PARAMETER)
   {
      // For array element, remove parenthesis before adding
      std::string objName;
      objName = GmatStringUtil::GetArrayName(goalName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(achieveName);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(toleranceName);
      refObjectNames.push_back(objName);
   }
   
   return refObjectNames;
}


// Parameter accessors

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
std::string Achieve::GetParameterText(const Integer id) const
{
   if (id >= SolverSequenceCommandParamCount && id < AchieveParamCount)
      return PARAMETER_TEXT[id - SolverSequenceCommandParamCount];
   return SolverSequenceCommand::GetParameterText(id);
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
Integer Achieve::GetParameterID(const std::string &str) const
{
   for (Integer i = SolverSequenceCommandParamCount; i < AchieveParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SolverSequenceCommandParamCount])
         return i;
   }

   return SolverSequenceCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Achieve::GetParameterType(const Integer id) const
{
   if (id >= SolverSequenceCommandParamCount && id < AchieveParamCount)
      return PARAMETER_TYPE[id - SolverSequenceCommandParamCount];

   return SolverSequenceCommand::GetParameterType(id);
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
std::string Achieve::GetParameterTypeString(const Integer id) const
{
   return SolverSequenceCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Determines if a parameter is displayed in scripting
 *
 * @param id ID of the parameter
 *
 * @return true if it should be shown.
 */
//------------------------------------------------------------------------------
bool Achieve::IsParameterReadOnly(const Integer id) const
{
   if (id == achievedValueID)
      return true;

   return SolverSequenceCommand::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Determines if a parameter is displayed in scripting
 *
 * @param label Scripted field name of the parameter
 *
 * @return true if it should be shown.
 */
//------------------------------------------------------------------------------
bool Achieve::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param id ID for the requested parameter value.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Achieve::GetRealParameter(const Integer id) const
{
   if (id == goalNameID)
      if (goal) return goal->EvaluateReal();
   if (id == goalValueID)
      if (achieve) return achieve->EvaluateReal();
   if (id == toleranceID)
      if (tolerance) return tolerance->EvaluateReal();
    
   return SolverSequenceCommand::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param id    ID for the parameter whose value to change.
 * @param value value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Achieve::SetRealParameter(const Integer id, const Real value)
{
   return SolverSequenceCommand::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Achieve::GetStringParameter(const Integer id) const
{
   if (id == targeterNameID)
      return targeterName;
        
   if (id == goalNameID)
      return goalName;
        
   if (id == goalValueID) 
      return achieveName;

   if (id == toleranceID) 
      return toleranceName;

   return SolverSequenceCommand::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const char *value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id    ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
bool Achieve::SetStringParameter(const Integer id, const char *value)
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
 * @param id    ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
bool Achieve::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_ACHIEVE_PARAMS
   MessageInterface::ShowMessage
      ("Achieve::SetStringParameter() id=%d, value=%s\n", id, value.c_str());
   #endif
   
   if (id == targeterNameID) 
   {
      targeterName = value;
      // Set the base class string
      solverName    = value;
      return true;
   }

   if (id == goalNameID) 
   {
      goalName = value;
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      return true;
   }
   
   if (id == goalValueID) 
   {
      achieveName = value;
      
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      return true;
   }
   
   if (id == toleranceID) 
   {
      toleranceName = value;
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      
      // Do range check here if value is a real number
      Real tol;
      if (GmatStringUtil::ToReal(value, tol))
         SetTolerance(tol);
      
      return true;
   }

   return SolverSequenceCommand::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// void SetField(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Enabled direct updates to the Vary command settings from the API
 *
 * @param id Field ID for the setting
 * @param value New value for the setting
 */
//------------------------------------------------------------------------------
void Achieve::SetField(const Integer id, const Real value)
{
   #ifdef DEBUG_VARY_API_ACCESS
      MessageInterface::ShowMessage("Setting field %s to Real value %le\n",
            GetParameterText(id).c_str(), value);
   #endif

   switch(id)
   {
   case goalValueID:
      if (achieve)
      {
         achieve->SetReal(value);
         return;
      }
      break;

   case toleranceID:
      if (tolerance)
      {
         tolerance->SetReal(value);
         return;
      }
      break;

   default:
      break;
   }

   GmatBase::SetField(id, value);
}

//------------------------------------------------------------------------------
// void GetField(const Integer id)
//------------------------------------------------------------------------------
/**
 * Enabled direct updates to the Vary command settings from the API
 *
 * @param id Field ID for the setting
 *
 * @return The field value
 */
//------------------------------------------------------------------------------
std::string Achieve::GetField(const Integer id)
{
   return GmatBase::GetField(id);
}

//------------------------------------------------------------------------------
// void GetNumber(const Integer id)
//------------------------------------------------------------------------------
/**
 * Enabled direct updates to the Vary command settings from the API
 *
 * @param value New value for the setting
 *
 * @return The real value for teh field
 */
//------------------------------------------------------------------------------
Real Achieve::GetNumber(const Integer id)
{
   switch(id)
   {
   case goalValueID:
      if (achieve)
         return achieve->EvaluateReal();
      break;

   case toleranceID:
      if (tolerance)
         return tolerance->EvaluateReal();
      break;

   case achievedValueID:
      return achievedValue;

   default:
      break;
   }

   return GmatBase::GetNumber(id);
}

// Multiple variables on the same line are not allowed in the current build.
// const StringArray& Achieve::GetStringArrayParameter(const Integer id) const; 


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                                     const std::string &name = "")
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
bool Achieve::SetRefObject(GmatBase *obj, const UnsignedInt type,
                           const std::string &name)
{   
   if (type == Gmat::SOLVER) 
   {
      if (targeterName == obj->GetName()) 
      {
         targeter = (Solver*)obj;
         return true;
      }
      return false;
   }

   return SolverSequenceCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Achieve command has the following syntax:
 *
 *     Achieve myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});
 *     Achieve myDC(Sat1.SMA = Var1, {Tolerance = 0.1});
 *     Achieve myDC(Sat1.SMA = Arr1(1,1), {Tolerance = 0.1});
 *     Achieve myDC(Sat1.SMA = Arr1(I,J), {Tolerance = 0.1});
 *
 * where myDC is a Solver used to Achieve a set of variables to achieve the
 * corresponding goals.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
bool Achieve::InterpretAction()
{
   // Clean out any old data
   wrapperObjectNames.clear();
   ClearWrappers();

   StringArray chunks = InterpretPreface();

   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage("Preface chunks as\n");
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
      MessageInterface::ShowMessage("\n");
   #endif
      
   if (chunks.size() <= 1)
      throw CommandException("Missing information for Achieve command.\n");
   
   if (chunks[1].at(0) == '(')
      throw CommandException("Missing solver name for Achieve command.\n");
      
   if ((chunks[1].find("[") != chunks[1].npos) || (chunks[1].find("]") != chunks[1].npos))
      throw CommandException("Brackets not allowed in Achieve command");

   if (!GmatStringUtil::AreAllBracketsBalanced(chunks[1], "({)}"))
   {
      throw CommandException
         ("Parentheses, braces, or brackets are unbalanced or incorrectly placed\n");
   }
   
   // Find and set the solver object name
   // This is the only setting in Achieve that is not in a wrapper
   StringArray currentChunks = parser.Decompose(chunks[1], "()", false);
   SetStringParameter(targeterNameID, currentChunks[0]);
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage("current chunks as\n");
      for (StringArray::iterator i = currentChunks.begin(); i != currentChunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
      MessageInterface::ShowMessage("\n");
   #endif
   
   std::string noSpaces2     = GmatStringUtil::RemoveAll(currentChunks[1],' ');
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage(
         "Achieve: noSpaces2 = %s\n", noSpaces2.c_str());
   #endif   
   currentChunks = parser.Decompose(noSpaces2, "()", true, true);
   
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage(
         "Achieve: after Decompose, current chunks = \n");
      for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
         MessageInterface::ShowMessage("   %s\n",
                                       currentChunks[jj].c_str());
   #endif

   if (currentChunks.size() == 0)
      throw CommandException("The goal description is missing in the "
            "Achieve command\n");

   // First chunk is the goal and achieve (target) value
   std::string lhs, rhs;
   if (!SeparateEquals(currentChunks[0], lhs, rhs, true))
   {
      throw CommandException("The goal \"" + lhs + 
         "\" is missing the \"=\" operator or a goal value required for an " + typeName + 
         " command.\n");
   }
      
   goalName = lhs;
   achieveName = rhs;
   
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage(
         "Achieve:  setting goalName to %s\n", goalName.c_str());
      MessageInterface::ShowMessage(
         "Achieve:  setting achieveName to %s\n", achieveName.c_str());
   #endif
   
   // if there are no more chunks, just return
   if (currentChunks.size() == (Integer) 1) return true;
   
   std::string noSpaces     = GmatStringUtil::RemoveAll(currentChunks[1],' ');
   // Now deal with the settable parameters
   currentChunks = parser.SeparateBrackets(noSpaces, "{}", ",", true);
   
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage(
         "Achieve: After SeparateBrackets, current chunks = \n");
      for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
         MessageInterface::ShowMessage("   %s\n",
                                       currentChunks[jj].c_str());
   #endif
   
   // currentChunks now holds all of the pieces - no need for more separation  
   
   for (StringArray::iterator i = currentChunks.begin(); 
        i != currentChunks.end(); ++i)
   {
      bool isOK = SeparateEquals(*i, lhs, rhs, true);
      #ifdef DEBUG_ACHIEVE_PARSE
         MessageInterface::ShowMessage("Setting Achieve properties\n");
         MessageInterface::ShowMessage("   \"%s\" = \"%s\"\n", lhs.c_str(), rhs.c_str());
      #endif
      
      if (!isOK || lhs.empty() || rhs.empty())
         throw CommandException("The setting \"" + lhs + 
            "\" is missing the \"=\" operator or a value required for an " + typeName + 
            " command.\n");
      
      if (IsSettable(lhs))
         SetStringParameter(GetParameterID(lhs), rhs);
      else
         throw CommandException("The setting \"" + lhs + 
            "\" is not a valid setting for an " + typeName + 
            " command.\n");
   }
   
   return true;
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& Achieve::GetWrapperObjectNameArray(bool completeSet)
{
   wrapperObjectNames.clear();

   wrapperObjectNames.push_back(goalName);
   
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       achieveName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(achieveName);

   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       toleranceName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(toleranceName);
   
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool Achieve::SetElementWrapper(ElementWrapper *toWrapper, 
              const std::string &withName)
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Achieve::SetElementWrapper() this=<%p> entered, wrapperName='%s'\n",
       this, withName.c_str());
   ShowWrapper("   ", "", toWrapper);
   MessageInterface::ShowMessage
      ("   goal=<%p>, achieve=<%p>, tolerance=<%p>\n", goal, achieve, tolerance);
   #endif
   bool retval = false;

   if (toWrapper == NULL) return false;
   
   if (toWrapper->GetWrapperType() == Gmat::ARRAY_WT)
   {
      throw CommandException("A value of type \"Array\" on command \"" + 
                  typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }
   
   CheckDataType(toWrapper, Gmat::REAL_TYPE, "Achieve", true);

   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage(
               "   Setting wrapper \"%s\" on Achieve command\n", 
      withName.c_str());
   #endif

   if (goalName == withName)
   {
      if (toWrapper->GetWrapperType() == Gmat::NUMBER_WT)
      {
         std::string errmsg = "The value of \"" + goalName;
         errmsg            += "\" for field \"Goal\" on object \"";
         errmsg            += instanceName + "\" is not an allowed value.\n";
         errmsg            += "The allowed values are: ";
         errmsg            += "[ Object Property, Array Element, Variable, ";
         errmsg            += "or Parameter, excluding numbers].";
         throw CommandException(errmsg);
      }
      goal = toWrapper;
      #ifdef DEBUG_WRAPPER_CODE   
      MessageInterface::ShowMessage
         ("   goal set to wrapper <%p>'%s'\n", toWrapper, withName.c_str());
      #endif
      retval = true;
   }
   
   std::vector<ElementWrapper*> temp;
   if (achieveName == withName)
   {
      if (achieve != NULL)
         temp.push_back(achieve);
      
      achieve = toWrapper;
      #ifdef DEBUG_WRAPPER_CODE   
      MessageInterface::ShowMessage
         ("   achieve set to wrapper <%p>'%s'\n", toWrapper, withName.c_str());
      #endif
      retval = true;
   }
   
   if (toleranceName == withName)
   {
      if (tolerance != NULL)
      {
         if (find(temp.begin(), temp.end(), tolerance) == temp.end())
            temp.push_back(tolerance);
      }
      tolerance = toWrapper;
      #ifdef DEBUG_WRAPPER_CODE   
      MessageInterface::ShowMessage
         ("   tolerance set to wrapper <%p>'%s'\n", toWrapper, withName.c_str());
      #endif
      retval = true;
   }
   
   // Delete old wrappers
   ElementWrapper *wrapper;
   for (UnsignedInt i = 0; i < temp.size(); ++i)
   {
      wrapper = temp[i];
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (wrapper, wrapper->GetDescription(), "Achieve::ClearWrappers()",
          GetGeneratingString(Gmat::NO_COMMENTS) + " deleting wrapper");
      #endif
      #ifdef DEBUG_WRAPPER_CODE   
      MessageInterface::ShowMessage("   Deleting wrapper<%p>\n", wrapper);
      #endif
      delete wrapper;
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Achieve::SetElementWrapper() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
void Achieve::ClearWrappers()
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage("Achieve::ClearWrappers() this=<%p> entered\n", this);
   #endif
   std::vector<ElementWrapper*> temp;
   if (goal)
   {
      temp.push_back(goal);
      goal = NULL;
   }
   if (achieve)
   {
      if (find(temp.begin(), temp.end(), achieve) == temp.end())
         temp.push_back(achieve);
      achieve = NULL;
   }
   if (tolerance)
   {
      if (find(temp.begin(), temp.end(), tolerance) == temp.end())
         temp.push_back(tolerance);
      tolerance = NULL;
   }
   
   ElementWrapper *wrapper;
   for (UnsignedInt i = 0; i < temp.size(); ++i)
   {
      wrapper = temp[i];
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (wrapper, wrapper->GetDescription(), "Achieve::ClearWrappers()",
          GetGeneratingString(Gmat::NO_COMMENTS) + " deleting wrapper");
      #endif
      #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage("   Deleting wrapper<%p>\n", wrapper);
      #endif
      delete wrapper;
   }
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage("Achieve::ClearWrappers() this=<%p> leaving\n", this);
   #endif
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Achieve command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Achieve::Initialize()
{
   #ifdef DEBUG_ACHIEVE_INIT
   MessageInterface::ShowMessage
      ("Achieve::Initialize() entered, targeter=<%p>\n", targeter);
   #endif
   
   bool retval = SolverSequenceCommand::Initialize();

   if (targeter == NULL)
      throw CommandException(
         "Targeter not initialized for Achieve command\n  \""
         + generatingString + "\"\n");
   
   Integer id = targeter->GetParameterID("Goals");
   targeter->SetStringParameter(id, goalName);
   
   // Set references for the wrappers   
   #ifdef DEBUG_ACHIEVE_INIT
   MessageInterface::ShowMessage
      ("Setting refs for goal <%p>'%s'\n", goal, goal ? goal->GetDescription().c_str() : "NULL");
   if (goal)
      MessageInterface::ShowMessage
         ("   refObj of goal = <%p>'%s'\n", goal->GetRefObject(),
          goal->GetRefObject() ? goal->GetRefObject()->GetName().c_str() : "NULL");
   #endif
   if (SetWrapperReferences(*goal) == false)
      return false;
   CheckDataType(goal, Gmat::REAL_TYPE, "Achieve");
   #ifdef DEBUG_ACHIEVE_INIT
      MessageInterface::ShowMessage("Setting refs for achieve\n");
   #endif
   if (SetWrapperReferences(*achieve) == false)
      return false;
   CheckDataType(achieve, Gmat::REAL_TYPE, "Achieve");
   #ifdef DEBUG_ACHIEVE_INIT
      MessageInterface::ShowMessage("Setting refs for tolerance\n");
   #endif
   if (SetWrapperReferences(*tolerance) == false)
      return false;
   CheckDataType(tolerance, Gmat::REAL_TYPE, "Achieve");
   
   // The targeter cannot be finalized until all of the loop is initialized
   targeterDataFinalized = false;
   
   #ifdef DEBUG_ACHIEVE_INIT
   MessageInterface::ShowMessage
      ("Achieve::Initialize() exiting. targeter=<%p>\n", targeter);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Achieve the variables defined for this targeting loop.
 *
 * This method (will eventually) feeds data to the targeter state machine in 
 * order to determine the variable values needed to achieve the user specified
 * goals.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Achieve::Execute()
{
   #ifdef DEBUG_ACHIEVE_EXEC
   MessageInterface::ShowMessage
      ("Achieve::Execute() targeterDataFinalized=%d\n   targeter=<%p>'%s'\n",
       targeterDataFinalized, targeter, targeter->GetName().c_str());
   MessageInterface::ShowMessage
      ("   goalName=%s, achieveName=%s\n", goalName.c_str(), achieveName.c_str());
   MessageInterface::ShowMessage
      ("   goal=<%p>'%s'\n", goal, goal ? goal->GetDescription().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("   achieve=<%p>'%s'\n", achieve, achieve ? achieve->GetDescription().c_str() : "NULL");
   #endif

   if (goal == NULL || achieve == NULL || tolerance == NULL)
      throw CommandException("NULL element wrappers found in Achieve command\n");
   
   bool retval = true;
   if (!targeterDataFinalized) 
   {
      // Tell the targeter about the goals and tolerances
      Real goalData[2];
      goalData[0] = goal->EvaluateReal();
      goalData[1] = tolerance->EvaluateReal();

      if (goalData[1] <= 0.0)
      {
         CommandException ce;
         ce.SetDetails(errorMessageFormat.c_str(),
                    tolerance->GetDescription().c_str(), "Tolerance",
                    "Real Number, Array element, Variable, or Parameter > 0.0");
         throw ce;
      }

      goalId = targeter->SetSolverResults(goalData, goalName);
      targeterDataFinalized = true;
      
      #ifdef DEBUG_ACHIEVE_EXEC
      MessageInterface::ShowMessage
         ("   Set goal data '%s' [%f, %f] to targeter<%p>'%s'\n", goalName.c_str(),
          goalData[0], goalData[1], targeter, targeter->GetName().c_str());
      #endif
      
      return retval;
   }
   
   Real val = -999.999;
   
   // Evaluate the floating target (if there is one) and set it on the targeter
   val = achieve->EvaluateReal();

   #ifdef DEBUG_ACHIEVE_EXEC
   MessageInterface::ShowMessage
      ("   Setting achieve = %f to targeter<%p>\n", val, targeter);
   #endif
   targeter->UpdateSolverGoal(goalId, val);
   
   // Evaluate goal and pass it to the targeter
   val = goal->EvaluateReal();
   #ifdef DEBUG_ACHIEVE_EXEC
   MessageInterface::ShowMessage
      ("   Setting goal = %f to targeter<%p>\n", val, targeter);
   #endif
   targeter->SetResultValue(goalId, val);
   achievedValue = val;
   
   // Evaluate tolerance pass it to the targeter
   val = tolerance->EvaluateReal();
   #ifdef DEBUG_ACHIEVE_EXEC
   MessageInterface::ShowMessage
      ("   Setting tolerance = %f to targeter<%p>\n", val, targeter);
   #endif
   targeter->UpdateSolverTolerance(goalId, val);
   
   BuildCommandSummary(true);
   
   return retval;
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
 * @param mode    Specifies the type of serialization requested.  (Not yet used
 *                in commands)
 * @param prefix  Optional prefix appended to the object's name.  (Not yet used
 *                in commands)
 * @param useName Name that replaces the object's name.  (Not yet used in
 *                commands)
 *
 * @return The script line that, when interpreted, defines this Achieve command.
 */
//------------------------------------------------------------------------------
const std::string& Achieve::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   std::string gen = prefix + "Achieve " + targeterName + "(" + goalName +
                     " = " + achieveName + ", {Tolerance = " + toleranceName;
   
   generatingString = gen + "});";
   
   // Then call the base class method for preface and inline comments
   return SolverSequenceCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void Achieve::RunComplete()
{
   #ifdef DEBUG_ACHIEVE_EXEC
      MessageInterface::ShowMessage(
      "In Achieve::RunComplete, targeterDataFinalized = %s, ... now setting it to false\n",
      (targeterDataFinalized? "true" : "false"));
   #endif
   targeterDataFinalized = false;
   SolverSequenceCommand::RunComplete();
}


//------------------------------------------------------------------------------
// void SetTolerance(Real value)
//------------------------------------------------------------------------------
void Achieve::SetTolerance(Real value)
{
   #ifdef DEBUG_ACHIEVE_PARAM
   MessageInterface::ShowMessage
      ("Achieve::SetTolerance() entered, value=%f, tolerance=<%p>\n", value,
       tolerance);
   #endif
   
   if (value > 0.0)
   {
      if (tolerance)
      {
         tolerance->SetReal(value);
         #ifdef DEBUG_ACHIEVE_PARAM
         MessageInterface::ShowMessage
            ("   value=%f set to tolerance<%p>\n", value, tolerance);
         #endif
      }
   }
   else
   {
      CommandException ce;
      ce.SetDetails(errorMessageFormat.c_str(),
                    GmatStringUtil::ToString(value, GetDataPrecision()).c_str(),
                    "Tolerance",
                    "Real Number, Array element, Variable, or Parameter > 0.0");
      throw ce;
   }
   
   #ifdef DEBUG_ACHIEVE_PARAM
   MessageInterface::ShowMessage("Achieve::SetTolerance() leaving\n");
   #endif
}
