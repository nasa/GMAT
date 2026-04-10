//$Id$
//------------------------------------------------------------------------------
//                                 Minimize
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
// Author: Wendy Shoan (GSFC/MAB) (begun by Daniel Hunter during summer 2006)
// Created: 2006.08.11
//
/**
 * Definition for the Minimize command class
 */
//------------------------------------------------------------------------------


#include "Minimize.hpp"
#include "StringUtil.hpp"          // for ToDouble()
#include "MessageInterface.hpp"
#include <sstream>                 // for stringstream

//#define DEBUG_MINIMIZE_PARSE 1
//#define DEBUG_MINIMIZE_INIT 1
//#define DEBUG_MINIMIZE_EXEC 1
//#define DEBUG_MINIMIZE
//#define DEBUG_MINIMIZE_PARAM
//#define DEBUG_MINIMIZE_PARSING
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_RENAME


//---------------------------------
// static data
//---------------------------------
const std::string
   Minimize::PARAMETER_TEXT[MinimizeParamCount - SolverSequenceCommandParamCount] =
   {
      "OptimizerName",
      "ObjectiveName",
      "Cost"
   };
   
const Gmat::ParameterType
   Minimize::PARAMETER_TYPE[MinimizeParamCount - SolverSequenceCommandParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::REAL_TYPE,
   };


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Minimize()
//------------------------------------------------------------------------------
/**
 * Creates an Minimize command.  (default constructor)
 */
//------------------------------------------------------------------------------
Minimize::Minimize() :
   SolverSequenceCommand             ("Minimize"),
   optimizerName           (""),
   objectiveName           ("9.999999e300"),
   objective               (NULL),
   //objectiveValue          (-999.99),
   optimizer               (NULL),
   optimizerDataFinalized  (false),
   //isMinimizeParm          (false),
   objId                   (-1),
   interpreted             (false)
{
   objectTypeNames.push_back("SolverCommand");
   objectTypeNames.push_back("Minimize");
   
   // nothing to add to settables here ...

   #ifdef DEBUG_MINIMIZE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Creating Minimize command ...\n");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   parameterCount = MinimizeParamCount;
}


//------------------------------------------------------------------------------
//  Minimize(const Minimize& m)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Minimize command.  (Copy constructor)
 *
 * @param t Command that is replicated here.
 */
//------------------------------------------------------------------------------
Minimize::Minimize(const Minimize& m) :
   SolverSequenceCommand             (m),
   optimizerName           (m.optimizerName),
   objectiveName           (m.objectiveName),
   objective               (NULL),
   //objectiveValue          (m.objectiveValue),
   optimizer               (NULL),
   optimizerDataFinalized  (false),
   //isMinimizeParm          (m.isMinimizeParm),
   objId                   (m.objId),
   interpreted             (false)
{
   #ifdef DEBUG_MINIMIZE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Creating (copying) Minimize command ...\n");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   parameterCount = MinimizeParamCount;
}


//------------------------------------------------------------------------------
//  Minimize& operator=(const Minimize& m)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Minimize command.
 *
 * @param t Command that is replicated here.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Minimize& Minimize::operator=(const Minimize& m)
{
   if (this == &m)
      return *this;
    
   SolverSequenceCommand::operator=(m);
   optimizerName          = m.optimizerName;
   objectiveName          = m.objectiveName;
   objective              = NULL;
   //objectiveValue         = m.objectiveValue;
   optimizer              = NULL;
   optimizerDataFinalized = false;
   //isMinimizeParm         = m.isMinimizeParm;
   objId                  = m.objId;
   interpreted            = false;

   // Initialize wrappers to NULL
   ClearWrappers();

   return *this;
}

//------------------------------------------------------------------------------
//  ~Minimize()
//------------------------------------------------------------------------------
/**
 * Destroys the Minimize command.  (destructor)
 */
//------------------------------------------------------------------------------
Minimize::~Minimize()
{
   ClearWrappers();
   //delete objective;  // yes? no? not in ths case, it seems
}

    


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Minimize.
 *
 * @return clone of the Minimize.
 */
//------------------------------------------------------------------------------
GmatBase* Minimize::Clone() const
{
   return (new Minimize(*this));
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
bool Minimize::RenameRefObject(const UnsignedInt type,
                               const std::string &oldName,
                               const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Minimize::RenameRefObject() entered, type=%d, oldName='%s', newName='%s', "
       "objective=<%p>\n", type, oldName.c_str(), newName.c_str(), objective);
   #endif
   
   if (type == Gmat::SOLVER)
   {
      if (optimizerName == oldName)
         optimizerName = newName;
   }
   else if (type == Gmat::PARAMETER)
   {
      if (objectiveName.find(oldName) != objectiveName.npos)
	     objectiveName =
			GmatStringUtil::ReplaceName(objectiveName, oldName, newName);
   }
   // make sure the wrapper(s) know to rename any objects they may be using
   if (objective)
   {
      objective->RenameObject(oldName, newName);
      objectiveName = objective->GetDescription();
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Minimize::RenameRefObject() returning true, objectiveName='%s'\n",
       objectiveName.c_str());
   #endif

   return SolverSequenceCommand::RenameRefObject(type, oldName, newName);
}

// Parameter accessors
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
const ObjectTypeArray& Minimize::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SOLVER);
   //refObjectTypes.push_back(Gmat::PARAMETER);
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
const StringArray& Minimize::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::SOLVER)
   {
      refObjectNames.push_back(optimizerName);
   }
   else if (type == Gmat::PARAMETER)
   {
      // For array element, remove parenthesis before adding
      std::string objName;
      objName = GmatStringUtil::GetArrayName(objectiveName);
      refObjectNames.push_back(objName);
   }
   
   return refObjectNames;
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
std::string Minimize::GetParameterText(const Integer id) const
{
   if (id >= SolverSequenceCommandParamCount && id < MinimizeParamCount)
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
Integer Minimize::GetParameterID(const std::string &str) const
{
   for (Integer i = SolverSequenceCommandParamCount; i < MinimizeParamCount; i++)
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
Gmat::ParameterType Minimize::GetParameterType(const Integer id) const
{
   if (id >= SolverSequenceCommandParamCount && id < MinimizeParamCount)
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
std::string Minimize::GetParameterTypeString(const Integer id) const
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
bool Minimize::IsParameterReadOnly(const Integer id) const
{
   if (id == OBJECTIVE_VALUE)
      return true;

   return SolverSequenceCommand::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Determines if a parameter is displayed in scripting
 *
 * @param label Scripting for the parameter
 *
 * @return true if it should be shown.
 */
//------------------------------------------------------------------------------
bool Minimize::IsParameterReadOnly(const std::string &label) const
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
Real Minimize::GetRealParameter(const Integer id) const
{
   if ((id == OBJECTIVE_NAME) && (objective))
   {
      /// @note: Is there a path into this code?? OBJECTIVE_NAME type == string.
      return objective->EvaluateReal();
   }

   if (id == OBJECTIVE_VALUE)
   {
      if (objective)
         return objective->EvaluateReal();
   }
    
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
Real Minimize::SetRealParameter(const Integer id, const Real value)
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
std::string Minimize::GetStringParameter(const Integer id) const
{
   if (id == OPTIMIZER_NAME)
      return optimizerName;
        
   if (id == OBJECTIVE_NAME)
      return objectiveName;
        
   return SolverSequenceCommand::GetStringParameter(id);
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
bool Minimize::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_MINIMIZE_PARAM // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   MessageInterface::ShowMessage
      ("Minimize::SetStringParameter() id=%d, value=%s\n", id, value.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   
   if (id == OPTIMIZER_NAME) 
   {
      optimizerName = value;
      // Set the base class string
      solverName    = value;
      interpreted   = false;
      return true;
   }

   if (id == OBJECTIVE_NAME) 
   {
      objectiveName = value;
      interpreted   = false;

      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      return true;
   }


   return SolverSequenceCommand::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                                   const std::string &name = "")
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
bool Minimize::SetRefObject(GmatBase *obj, const UnsignedInt type,
                            const std::string &name)
{   
   #ifdef DEBUG_MINIMIZE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "setting ref obj %s of type %s on Minimize command\n",
      (obj->GetName()).c_str(), (obj->GetTypeName()).c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   if (type == Gmat::SOLVER) 
   {
      if (optimizerName == obj->GetName()) 
      {
         optimizer = (Solver*)obj;
         return true;
      }
      return false;
   }
   //if (type == Gmat::PARAMETER) 
   //{
   //   if (objectiveName == obj->GetName()) 
    //  {
    //     //objective = (ElementWrapper*)obj;
    //     return true;
    //  }
    //  return false;
   //}

   return SolverSequenceCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Minimize command has the following syntax:
 *
 *     Minimize myOPT(Var1);
 *
 * where myOPT is a Solver used to Minimize a set of variables to achieve the
 * corresponding goals.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
bool Minimize::InterpretAction()
{
   #ifdef DEBUG_MINIMIZE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Interpreting Minimize command ...\n");
      MessageInterface::ShowMessage("generatingString = %s",
         generatingString.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   /// @todo: Clean up this hack for the Minimize::InterpretAction method
   // Sample string:  "Minimize myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});"
   /// @todo clean that up - because that's not even right ^^^^^
   
      // Clean out any old data
   wrapperObjectNames.clear();
   ClearWrappers();

   StringArray chunks = InterpretPreface();

   #ifdef DEBUG_MINIMIZE_PARSING
      MessageInterface::ShowMessage("Preface chunks as\n");
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
      MessageInterface::ShowMessage("\n");
   #endif
   
   // Find and set the solver object name
   // This is the only setting in Vary that is not in a wrapper
   StringArray currentChunks = parser.Decompose(chunks[1], "()", false);
   if (!GmatStringUtil::HasNoBrackets(currentChunks.at(0), false))
      throw CommandException(
            "Solver name for Minimize command may not contain brackets, braces, or parentheses."); 
      
   SetStringParameter(OPTIMIZER_NAME, currentChunks[0]);
   if (currentChunks.size() < 2)
      throw CommandException("Missing field or value for Minimize command.");
 
    // The remaining text in the instruction is the variable definition and 
   // parameters, all contained in currentChunks[1].  Deal with those next.
   std::string cc = GmatStringUtil::Strip(currentChunks[1]);
   Integer ccEnd = cc.size() - 1;
   if ((cc.at(0) != '(') || (cc.at(ccEnd) != ')'))
      throw CommandException(
           "Missing parentheses, or unexpected characters found, around argument to Minimize command.");
   if (!GmatStringUtil::IsBracketBalanced(cc, "()"))
      throw CommandException("Parentheses unbalanced in Minimize command.");
   if ((cc.find('[') != cc.npos) || (cc.find(']') != cc.npos) ||
       (cc.find('{') != cc.npos) || (cc.find('}') != cc.npos) )
      throw CommandException("Minimize command may not contain brackets or braces.");

   std::string noSpaces     = GmatStringUtil::RemoveAll(cc,' ');
   currentChunks = parser.Decompose(noSpaces, "()", true, true);
   
   #ifdef DEBUG_MINIMIZE_PARSING
      MessageInterface::ShowMessage(
         "Minimize: after Decompose, current chunks = \n");
      for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
         MessageInterface::ShowMessage("   %s\n",
                                       currentChunks[jj].c_str());
   #endif

   if (currentChunks.size() == 0)
      throw CommandException("The objective function name is missing in the "
               "Minimize command\n");

   // First chunk is the objectiveName
   objectiveName = currentChunks[0];
      
   #ifdef DEBUG_MINIMIZE_PARSE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Minimize::InterpretAction() exiting\n");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      
   interpreted   = true;

   return true;
}

const StringArray& Minimize::GetWrapperObjectNameArray(bool completeSet)
{
   wrapperObjectNames.clear();
   wrapperObjectNames.push_back(objectiveName);
      
   return wrapperObjectNames;
}

bool Minimize::SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
{
   bool retval = false;

   if (toWrapper == NULL) return false;
   
   if (toWrapper->GetWrapperType() == Gmat::ARRAY_WT)
   {
      throw CommandException("A value of type \"Array\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Variable, Array Element, or Parameter ]. "); 
   }
   if (toWrapper->GetWrapperType() == Gmat::NUMBER_WT)
   {
      throw CommandException("A value of type \"Number\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Variable, Array Element, or Parameter ]. "); 
   }
   //if (toWrapper->GetWrapperType() == Gmat::STRING_OBJECT_WT)
   //{
   //   throw CommandException("A value of type \"String Object\" on command \"" + typeName + 
   //               "\" is not an allowed value.\nThe allowed values are:"
   //               " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   //}
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage("   Setting wrapper \"%s\" on Minimize command\n", 
      withName.c_str());
   #endif
   
   CheckDataType(toWrapper, Gmat::REAL_TYPE, "Minimize", true);

   if (objectiveName == withName)
   {
      objective = toWrapper;
      // Reset the generating string so initialization works correctly
      GetGeneratingString(Gmat::SCRIPTING, "", "");
      retval = true;
   }
   
   return retval;
}

void Minimize::ClearWrappers()
{
   std::vector<ElementWrapper*> temp;
   if (objective)
   {
      temp.push_back(objective);
      objective = NULL;
   }
   ElementWrapper *wrapper;
   for (UnsignedInt i = 0; i < temp.size(); ++i)
   {
      wrapper = temp[i];
      delete wrapper;
   }
}


//------------------------------------------------------------------------------
//  bool InterpretParameter(const std::string text, std::string &paramType,
//                          std::string &paramObj, std::string &parmSystem)
//------------------------------------------------------------------------------
/**
 * Breaks apart a parameter declaration into its component pieces
 *
 * @param text       The string that gets decomposed.
 * @param paramType  Type of parameter that is needed.
 * @param paramObj   The Object used for the parameter calculations.
 * @param parmSystem The coordinate system or body used for the parameter
 *                   calculations (or the empty string if this piece is
 *                   unspecified).
 *
 * @return success flag
 */
//------------------------------------------------------------------------------
/*
bool Minimize::InterpretParameter(const std::string text,
                                 std::string &paramType, 
                                 std::string &paramObj, 
                                 std::string &parmSystem)
{
   
   Real rval = 54321.12345;
   // check to see if it is a number first
   if (GmatStringUtil::ToDouble(text, &rval))
      return true;
   
   Integer start = 0, dotLoc = text.find(".", 0);
   if (dotLoc == (Integer)std::string::npos)
   {
//      throw CommandException("Minimize::InterpretParameter: Unable to "
//               "interpret parameter object in the string " +
//               text);
      parmObj = text; // the Variable is the parameter
      parmSystem = "";
      parmType   = "";
      return true;
   }
   
   paramObj = text.substr(start, dotLoc - start);
   start = dotLoc + 1;
   dotLoc = text.find(".", start);
   if (dotLoc != (Integer)std::string::npos) 
   {
      parmSystem = text.substr(start, dotLoc - start);
      start = dotLoc + 1;
   }
   else 
   {
      parmSystem = "";
   }
   
   paramType = text.substr(start);
   
   #ifdef DEBUG_MINIMIZE_PARSE // ~~~~ debug ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      MessageInterface::ShowMessage(
         "Minimize::InterpretParameter() Built parameter %s for object %s "
         "with CS %s\n", paramType.c_str(), paramObj.c_str(), parmSystem.c_str());
   #endif // ~~~~ end debug ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      
   return true;
}
*/

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Minimize command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Minimize::Initialize()
{
   #if DEBUG_MINIMIZE_INIT // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   MessageInterface::ShowMessage
      ("Minimize::Initialize() entered. optimizer=%p, objective=%p\n", 
      optimizer, objective);
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~

   if (!interpreted)
      if (!InterpretAction())
         throw CommandException(
            "Minimize: error interpreting input data\n");
   
   bool retval = SolverSequenceCommand::Initialize();

   if (optimizer == NULL)
      throw CommandException(
         "Optimizer not initialized for Minimize command\n  \""
         + generatingString + "\"\n");

   //Integer id = optimizer->GetParameterID("Goals");  // no clue
   //optimizer->SetStringParameter(id, goalName);

   // find objectiveName
   //GmatBase *obj = (*objectMap)[objectiveName];
   
   //if (obj == NULL) 
   //{
   //   std::string errorstr = "Could not find variable parameter ";
   //   errorstr += objectiveName;
   //   throw CommandException(errorstr);
   //}

   // The optimizer cannot be finalized until all of the loop is initialized
   optimizerDataFinalized = false;

   if (SetWrapperReferences(*objective) == false)
      return false;
   CheckDataType(objective, Gmat::REAL_TYPE, "Minimize");
   
   #if DEBUG_MINIMIZE_INIT // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   MessageInterface::ShowMessage
      ("Minimize::Initialize() exiting. optimizer=%p, objective=%p\n", 
      optimizer, objective);
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Minimize the variables defined for this targeting loop.
 *
 * This method (will eventually) feeds data to the optimizer state machine in 
 * order to determine the variable values needed to achieve the user specified
 * goals.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Minimize::Execute()
{
   #ifdef DEBUG_MINIMIZE_EXEC // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   MessageInterface::ShowMessage
      ("Minimize::Execute() optimizerDataFinalized=%d\n   optimizer=%p, objective=%p\n", 
       optimizerDataFinalized, optimizer, objective);
   MessageInterface::ShowMessage
      ("   objectiveName=%s\n", objectiveName.c_str());
   if (objective)
      MessageInterface::ShowMessage("   objective=%s\n", objective->GetDescription().c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   
   bool retval = true;
   if (!optimizerDataFinalized) 
   {
      // Tell the optimizer about the objective function
      Real minData[1];
      minData[0] = objective->EvaluateReal(); 
      //minData[1] = tolerance;
      objId = optimizer->SetSolverResults(minData, objectiveName, "Objective");

      optimizerDataFinalized = true;
      return retval;
   }
   
   Real val = -999.999;
   // Evaluate variable and pass it to the optimizer
   if (objective != NULL)
   {
      val = objective->EvaluateReal();
      #ifdef DEBUG_MINIMIZE_EXEC // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage
            ("   objective=%s, %p\n", objective->GetDescription().c_str(), objective);
         MessageInterface::ShowMessage("   Parameter target: %s val = %lf\n",
            objective->GetDescription().c_str(), val);
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      optimizer->SetResultValue(objId, val, "Objective");
   }
   else 
   {  // ERROR ERROR - should I throw an exception here? *********** TBD **************
      MessageInterface::ShowMessage(
         "Minimize: Warning - objective function is NULL\n"); 
      val = -999.999;
      //val = goalObject->GetRealParameter(parmId);  // again,no clue
      optimizer->SetResultValue(objId, val, "Objective");
      #ifdef DEBUG_MINIMIZE_EXEC // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage("   Object target: val = %lf\n", val);
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   }

   //targeter->SetResultValue(goalId, val);
   BuildCommandSummary(true);
   
   return retval;
}

void Minimize::RunComplete()
{
   optimizerDataFinalized = false;
   //ClearWrappers();
   SolverSequenceCommand::RunComplete();
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
 * @return The script line that, when interpreted, defines this Minimize command.
 */
//------------------------------------------------------------------------------
const std::string& Minimize::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   std::stringstream tol;
   //tol << tolerance;
   std::string gen = prefix + "Minimize " + optimizerName + "(" + objectiveName;

   generatingString = gen + ");";
   // Then call the base class method
   return SolverSequenceCommand::GetGeneratingString(mode, prefix, useName);
}


