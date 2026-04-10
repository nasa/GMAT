//$Id$
//------------------------------------------------------------------------------
//                                 NonlinearConstraint
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
// Author: Wendy Shoan (GSFC/MAB)
// Created: 2006.08.14
//
/**
 * Definition for the NonlinearConstraint command class
 */
//------------------------------------------------------------------------------

#include "NonlinearConstraint.hpp"
#include "StringUtil.hpp"  // for ToReal()
#include "MessageInterface.hpp"
#include "Optimizer.hpp"

//#define DEBUG_NONLINEAR_CONSTRAINT_PARSE 1
//#define DEBUG_NONLINEAR_CONSTRAINT_INIT 1
//#define DEBUG_NONLINEAR_CONSTRAINT_EXEC 1
//#define DEBUG_NLC_VALUES
//#define DEBUG_NONLINEAR_CONSTRAINT_PARAM
//#define DEBUG_NLC_PARSING
//#define DEBUG_NLC_WRAPPER_CODE
//#define DEBUG_EXTENDEDSETTINGS

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
   NonlinearConstraint::PARAMETER_TEXT[NonlinearConstraintParamCount - SolverSequenceCommandParamCount] =
   {
      "OptimizerName",
      "ConstraintArg1",
      "LHSValue",
      "Operator",
      "ConstraintArg2",
      "RHSValue",
      "Tolerance",
   };
   
const Gmat::ParameterType
   NonlinearConstraint::PARAMETER_TYPE[NonlinearConstraintParamCount - SolverSequenceCommandParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::REAL_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::REAL_TYPE,
      Gmat::STRING_TYPE, // Tolerance: Wrapper for a REAL_TYPE,
   };

const std::string // order must match enum Operator 
   NonlinearConstraint::OP_STRINGS[3] =
   {
      "<=",
      ">=",
      "=",
   };


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  NonlinearConstraint()
//------------------------------------------------------------------------------
/**
 * Creates an NonlinearConstraint command.  (default constructor)
 */
//------------------------------------------------------------------------------
NonlinearConstraint::NonlinearConstraint() :
   SolverSequenceCommand             ("NonlinearConstraint"),
   optimizerName           (""),
   optimizer               (NULL),
   arg1Name                (""),
   arg1                    (NULL),
   constraintValue         (-999.99),
   arg2Name                (""),
   arg2                    (NULL),
   isInequality            (false),
   isIneqString            ("EqConstraint"),
   desiredValue            (0.0),
   op                      (EQUAL),
   toleranceName           (""),
   tolerance               (nullptr),
   toleranceValue          (0.0),
   toleranceWarningIssued  (false),
   deltaTolerance          (0.0),
   optimizerDataFinalized  (false),
   constraintId            (-1),
   interpreted             (false)
{
   objectTypeNames.push_back("SolverCommand");
   objectTypeNames.push_back("NonlinearConstraint");
   parameterCount = NonlinearConstraintParamCount;

   settables.push_back("Tolerance");
}


//------------------------------------------------------------------------------
//  NonlinearConstraint(const NonlinearConstraint& nlc)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a NonlinearConstraint command.  (Copy constructor)
 *
 * @param t Command that is replicated here.
 */
//------------------------------------------------------------------------------
NonlinearConstraint::NonlinearConstraint(const NonlinearConstraint& nlc) :
   SolverSequenceCommand             (nlc),
   optimizerName           (nlc.optimizerName),
   optimizer               (NULL),
   arg1Name                (nlc.arg1Name),
   arg1                    (NULL),
   constraintValue         (nlc.constraintValue),
   arg2Name                (nlc.arg2Name),
   arg2                    (NULL),
   isInequality            (nlc.isInequality),
   isIneqString            (nlc.isIneqString),
   desiredValue            (nlc.desiredValue),
   op                      (nlc.op),
   toleranceName           (nlc.toleranceName),
   tolerance               (nullptr),
   toleranceValue          (0.0),
   toleranceWarningIssued  (false),
   deltaTolerance          (0.0),
   optimizerDataFinalized  (false),
   constraintId            (-1),
   interpreted             (false)
{
}


//------------------------------------------------------------------------------
//  NonlinearConstraint& operator=(const NonlinearConstraint& nlc)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the NonlinearConstraint command.
 *
 * @param nlc Command that is replicated here.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
NonlinearConstraint& NonlinearConstraint::operator=(const NonlinearConstraint& nlc)
{
   if (this != &nlc)
   {
      SolverSequenceCommand::operator=(nlc);
      optimizerName          = nlc.optimizerName;
      optimizer              = NULL;
      arg1Name               = nlc.arg1Name;
      arg1                   = NULL;
      constraintValue        = nlc.constraintValue;
      arg2Name               = nlc.arg2Name;
      arg2                   = NULL;
      isInequality           = nlc.isInequality;
      isIneqString           = nlc.isIneqString;
      desiredValue           = nlc.desiredValue;
      op                     = nlc.op;
      toleranceName          = nlc.toleranceName;
      toleranceValue         = 0.0;
      toleranceWarningIssued = false;
      deltaTolerance         = 0.0;

      optimizerDataFinalized = false;
      constraintId           = -1;
      interpreted            = false;

      if (tolerance)
      {
         delete tolerance;
         tolerance = nullptr;
      }
   }

   return *this;
}

//------------------------------------------------------------------------------
//  ~NonlinearConstraint()
//------------------------------------------------------------------------------
/**
 * Destroys the NonlinearConstraint command.  (destructor)
 */
//------------------------------------------------------------------------------
NonlinearConstraint::~NonlinearConstraint()
{
   ClearWrappers();
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the NonlinearConstraint.
 *
 * @return clone of the NonlinearConstraint.
 */
//------------------------------------------------------------------------------
GmatBase* NonlinearConstraint::Clone() const
{
   return (new NonlinearConstraint(*this));
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
bool NonlinearConstraint::RenameRefObject(const UnsignedInt type,
                               const std::string &oldName,
                               const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (optimizerName == oldName)
         optimizerName = newName;
   }
   if (arg1) 
   {
      arg1->RenameObject(oldName, newName);
      arg1Name  = arg1->GetDescription();
   }
   if (arg2) 
   {
      arg2->RenameObject(oldName, newName);
      arg2Name  = arg2->GetDescription();
   }
   if (tolerance)
   {
      tolerance->RenameObject(oldName, newName);
      toleranceName  = tolerance->GetDescription();
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
const ObjectTypeArray& NonlinearConstraint::GetRefObjectTypeArray()
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
const StringArray& NonlinearConstraint::GetRefObjectNameArray(const UnsignedInt type)
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
      objName = GmatStringUtil::GetArrayName(arg1Name);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(arg2Name);
      refObjectNames.push_back(objName);
      objName = GmatStringUtil::GetArrayName(toleranceName);
      if (objName != "")
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
std::string NonlinearConstraint::GetParameterText(const Integer id) const
{
   if (id >= SolverSequenceCommandParamCount && id < NonlinearConstraintParamCount)
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
Integer NonlinearConstraint::GetParameterID(const std::string &str) const
{
   for (Integer i = SolverSequenceCommandParamCount; i < NonlinearConstraintParamCount; i++)
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
Gmat::ParameterType NonlinearConstraint::GetParameterType(const Integer id) const
{
   if (id >= SolverSequenceCommandParamCount && id < NonlinearConstraintParamCount)
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
std::string NonlinearConstraint::GetParameterTypeString(const Integer id) const
{
   return SolverSequenceCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}

bool NonlinearConstraint::IsParameterReadOnly(const Integer id) const
{
   switch (id)
   {
   case LHS_CONSTRAINT_VALUE:
   case RHS_CONSTRAINT_VALUE:
      return true;

   default:
      break;
   }
   return SolverSequenceCommand::IsParameterReadOnly(id);
}

bool NonlinearConstraint::IsParameterReadOnly(const std::string &label) const
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
Real NonlinearConstraint::GetRealParameter(const Integer id) const
{
   if (id == TOLERANCE)
      if (tolerance)
         return tolerance->EvaluateReal();

   if (id == LHS_CONSTRAINT_VALUE)
      if (arg1)
         return arg1->EvaluateReal();
    
   if (id == RHS_CONSTRAINT_VALUE)
      if (arg2)
         return arg2->EvaluateReal();
    
   return SolverSequenceCommand::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *h
 * @param id    ID for the parameter whose value to change.
 * @param value value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real NonlinearConstraint::SetRealParameter(const Integer id, const Real value)
{
   if (id == RHS_CONSTRAINT_VALUE)
   {
      if (arg2)
      {
         arg2->SetReal(value);
         return arg2->EvaluateReal();
      }
   }

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
std::string NonlinearConstraint::GetStringParameter(const Integer id) const
{
   if (id == OPTIMIZER_NAME)
      return optimizerName;
        
   if (id == CONSTRAINT_ARG1)
      return arg1Name;
      
   if (id == CONSTRAINT_ARG2)
      return arg2Name;
      
   if (id == TOLERANCE)
      return toleranceName;
      
   if (id == OPERATOR)
   {
      return OP_STRINGS[(Integer)op];
   }
        
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
bool NonlinearConstraint::SetStringParameter(const Integer id, const char *value)
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
bool NonlinearConstraint::SetStringParameter(const Integer id, 
                                             const std::string &value)
{
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARAM
   MessageInterface::ShowMessage
      ("NonlinearConstraint::SetStringParameter() id=%d, value=%s\n", 
         id, value.c_str());
   #endif
   
   if (id == OPTIMIZER_NAME) 
   {
      optimizerName = value;
      // Set the base class string
      solverName    = value;
      optimizerDataFinalized = false;
      return true;
   }

   if (id == CONSTRAINT_ARG1) 
   {
      arg1Name       = value;
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      //interpreted    = false;
      return true;
   }
   
   if (id == CONSTRAINT_ARG2) 
   {
      arg2Name       = value;
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      //interpreted    = false;
      return true;
   }
   
   if (id == OPERATOR)
   {
      Operator oldOp = op;

      if (value == "<=")
         op = LESS_THAN_OR_EQUAL;
      else if (value == ">=")
         op = GREATER_THAN_OR_EQUAL;
      else if (value == "=")
         op = EQUAL;
      else
      {
         std::string errmsg = "The conditional operator \"" + value;
         errmsg            += "\" is not allowed in a NonlinearConstraint command.\n";
         errmsg            += "The allowed values are [=, <=, >=]\n";
         throw CommandException(errmsg);
      }

      if (op != oldOp)
      {
         optimizerDataFinalized = false;
         if (op == EQUAL)
         {
            isInequality = false;
            isIneqString = "EqConstraint";
         }
         else
         {
            isInequality = true;
            isIneqString = "IneqConstraint";
         }
      }

      //interpreted   = false;
      return true;
   }

   if (id == TOLERANCE)
   {
      toleranceName = value;
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) ==
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      return true;
   }

   return SolverSequenceCommand::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// void SetField(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * API access to update the value of the RHS value
 *
 * @param id The field ID
 * @param value The new value
 */
//------------------------------------------------------------------------------
void NonlinearConstraint::SetField(const Integer id, const Real value)
{
//   // Uncomment if LHS becomes settable
//   if (id == LHS_CONSTRAINT_VALUE)
//      if (arg1)
//         arg1->SetReal(value);

   if (id == RHS_CONSTRAINT_VALUE)
      if (arg2)
         arg2->SetReal(value);

   if (id == TOLERANCE)
      if (tolerance)
         tolerance->SetReal(value);

   SolverSequenceCommand::SetField(id, value);
}

//------------------------------------------------------------------------------
// void SetNumber(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets a numeric field value
 *
 * @param label The field name
 * @param value The new value
 */
//------------------------------------------------------------------------------
void NonlinearConstraint::SetNumber(const std::string &label, const Real value)
{
   Integer id = GetParameterID(label);

   switch (id)
   {
   case LHS_CONSTRAINT_VALUE:
//   // Uncomment if LHS becomes settable
//      if (arg1)
//         return arg1->EvaluateReal();
      break;

   case RHS_CONSTRAINT_VALUE:
      if (arg2)
         arg2->SetReal(value);
      return;

   case TOLERANCE:
      if (value >= 0.0)
         if (tolerance)
            tolerance->SetReal(value);
      return;

   default:
      break;
   }

   SolverSequenceCommand::SetNumber(label, value);
}

//------------------------------------------------------------------------------
// Real GetNumber(const Integer id)
//------------------------------------------------------------------------------
/**
 * Retrieves a numeric field value
 *
 * @param label The field ID
 *
 * @return The current value */
//------------------------------------------------------------------------------
Real NonlinearConstraint::GetNumber(const Integer id)
{
   switch (id)
   {
   case LHS_CONSTRAINT_VALUE:
      if (arg1)
         return arg1->EvaluateReal();
      break;

   case RHS_CONSTRAINT_VALUE:
      if (arg2)
         return arg2->EvaluateReal();
      break;

   case TOLERANCE:
      if (tolerance)
         return tolerance->EvaluateReal();

   default:
      break;
   }

   return SolverSequenceCommand::GetNumber(id);
}

//------------------------------------------------------------------------------
// Real GetNumber(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Retrieves a numeric field value
 *
 * @param label The field name
 *
 * @return The current value
 */
//------------------------------------------------------------------------------
Real NonlinearConstraint::GetNumber(const std::string &label)
{
   return GetNumber(GetParameterID(label));
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
bool NonlinearConstraint::SetRefObject(GmatBase *obj, const UnsignedInt type,
                            const std::string &name)
{   
   if (type == Gmat::SOLVER) 
   {
      if (optimizerName == obj->GetName()) 
      {
         optimizer = (Solver*)obj;
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
 * The NonlinearConstraint command has the following syntax:
 *
 *     NonlinearConstraint myOpt(Sat1.SMA = 21545.0, {Tolerance = 0.1});
 *     NonlinearConstraint myOpt(Sat1.SMA = Var1, {Tolerance = 0.1});
 *     NonlinearConstraint myOpt(Sat1.SMA = Arr1(1,1), {Tolerance = 0.1});
 *     NonlinearConstraint myOpt(Sat1.SMA = Arr1(I,J), {Tolerance = 0.1});
 *
 * where myOpt is a Solver used to optimize a set of variables to the
 * corresponding constraints.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
bool NonlinearConstraint::InterpretAction()
{
   bool tooMuch = false;
   // Clean out any old data
   wrapperObjectNames.clear();
   ClearWrappers();

   StringArray chunks = InterpretPreface();

   #ifdef DEBUG_NLC_PARSING
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
            "Solver name for NonlinearConstraint command may not contain brackets, braces, or parentheses."); 
   SetStringParameter(OPTIMIZER_NAME, currentChunks[0]);

   if (currentChunks.size() < 2)
      throw CommandException("Missing field or value for NonlinearConstraint command.");
   if (currentChunks.size() > 2) 
      tooMuch = true;

   std::string cc = GmatStringUtil::Strip(currentChunks[1]);
   Integer ccEnd = cc.size() - 1;
   if ((tooMuch) || (cc.at(0) != '(') || (cc.at(ccEnd) != ')'))
      throw CommandException(
           "Missing parentheses, or unexpected characters found, around logical expression argument to NonlinearConstraint command.");

   if (!GmatStringUtil::IsBracketBalanced(cc, "()"))
      throw CommandException("Parentheses unbalanced in NonlinearConstraint command.");

   if ((cc.find('[') != cc.npos) || (cc.find(']') != cc.npos))
      throw CommandException("The NonlinearConstraint command may not contain brackets.");
   
   std::string noSpaces2     = GmatStringUtil::RemoveAll(cc,' ');
   //std::string noSpaces2     = GmatStringUtil::RemoveAll(currentChunks[1],' ');
   currentChunks = parser.SeparateBrackets(noSpaces2, "()", ",", false);
   
   #ifdef DEBUG_NLC_PARSING
      MessageInterface::ShowMessage(
         "NLC:  noSpaces2 = %s\n", noSpaces2.c_str());
      MessageInterface::ShowMessage(
         "NLC: after SeparateBrackets, current chunks = \n");
      for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
         MessageInterface::ShowMessage("   %s\n",
                                       currentChunks[jj].c_str());
   #endif

   bool testForMore = false;
   if ((Integer) currentChunks.size() > 1) testForMore = true;
   Integer end;
   
   if (currentChunks.size() == 0)
      throw CommandException("The actual constraint is missing in the "
            "NonlinearConstraint command\n");
   
   std::string constraintStr = currentChunks[0];
   Integer opSize = 1;
   if ((end = constraintStr.find(">=", 0)) != (Integer) constraintStr.npos)
   {
      op = GREATER_THAN_OR_EQUAL;
      isInequality = true;
      isIneqString = "IneqConstraint";
      opSize       = 2;
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_INIT
         MessageInterface::ShowMessage("NLC:InterpretAction: greater_than_or_equal\n");
      #endif
   }
   else if ((end = constraintStr.find("<=", 0)) != (Integer) constraintStr.npos)
   {
      op = LESS_THAN_OR_EQUAL;
      isInequality = true;
      isIneqString = "IneqConstraint";
      opSize       = 2;
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_INIT
         MessageInterface::ShowMessage("NLC:InterpretAction: less_than_or_equal\n");
      #endif
   }
   else if ((end = constraintStr.find("=>", 0)) != (Integer) constraintStr.npos)
   {
      std::string errmsg = "The string \"=>\" is an invalid conditional operator";
      errmsg            += " in a NonlinearConstraint command.\n";
      errmsg            += "The allowed values are [=, <=, >=]\n";
      throw CommandException(errmsg);
   }
   else if ((end = constraintStr.find("=<", 0)) != (Integer) constraintStr.npos)
   {
      std::string errmsg = "The string \"=<\" is an invalid conditional operator";
      errmsg            += " in a NonlinearConstraint command.\n";
      errmsg            += "The allowed values are [=, <=, >=]\n";
      throw CommandException(errmsg);
   }
   else if ((end = constraintStr.find("==", 0)) != (Integer) constraintStr.npos)
   {
      std::string errmsg = "The string \"==\" is an invalid conditional operator";
      errmsg            += " in a NonlinearConstraint command.\n";
      errmsg            += "The allowed values are [=, <=, >=]\n";
      throw CommandException(errmsg);
   }
   else if ((end = constraintStr.find("=", 0)) != (Integer) constraintStr.npos)
   {
      //end = constraintStr.find("=", 0);
      op = EQUAL;
      isInequality = false;
      isIneqString = "EqConstraint";
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_INIT
         MessageInterface::ShowMessage("NLC:InterpretAction: equal\n");
      #endif
   }
   else
   {
      std::string errmsg = "The conditional operator is missing or invalid ";
      errmsg            += " in a NonlinearConstraint command.\n";
      errmsg            += "The allowed values are [=, <=, >=]\n";
      throw CommandException(errmsg);
   }
   arg1Name = constraintStr.substr(0,end);
   arg2Name = constraintStr.substr(end+opSize, (constraintStr.npos - end + opSize - 1));
   #ifdef DEBUG_NLC_PARSING
      MessageInterface::ShowMessage("... arg1Name = %s\n", arg1Name.c_str());
      MessageInterface::ShowMessage("... operator = %s\n", OP_STRINGS[(Integer)op].c_str());
      MessageInterface::ShowMessage("... arg2Name = %s\n", arg2Name.c_str());
   #endif

   panelDescriptor = "(" + OP_STRINGS[(Integer)op] + ") " + arg1Name;

   // Handle additional settings
   if (testForMore)
   {
      std::string noSpaces     = GmatStringUtil::RemoveAll(currentChunks[1],' ');
      currentChunks = parser.SeparateBrackets(noSpaces, "{}", ",", false);
      
      #ifdef DEBUG_NLC_PARSING
         MessageInterface::ShowMessage(
            "NLC: After SeparateBrackets, current chunks = \n");
         for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
            MessageInterface::ShowMessage("   %s\n",
                                          currentChunks[jj].c_str());
      #endif
      
      // currentChunks now holds all of the pieces - no need for more separation  
      
      std::string lhs, rhs;
      for (StringArray::iterator i = currentChunks.begin(); 
           i != currentChunks.end(); ++i)
      {
         SeparateEquals(*i, lhs, rhs);
         
         if (IsSettable(lhs))
         {
            #ifdef DEBUG_EXTENDEDSETTINGS
               MessageInterface::ShowMessage("Setting %s to %s\n", lhs.c_str(),
                     rhs.c_str());
            #endif

            SetStringParameter(GetParameterID(lhs), rhs);
         }
         else
         {
            throw CommandException("The setting \"" + lhs + 
               "\" is not a valid setting for a " + typeName + 
               " command.\n");
         }
      }
   }
   
   interpreted = true;
  
   return true;
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& NonlinearConstraint::GetWrapperObjectNameArray(bool completeSet)
{
   wrapperObjectNames.clear();

   wrapperObjectNames.push_back(arg1Name);
   
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       arg2Name) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(arg2Name);

   if (toleranceName != "")
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(),
          toleranceName) == wrapperObjectNames.end())
         wrapperObjectNames.push_back(toleranceName);

   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool NonlinearConstraint::SetElementWrapper(ElementWrapper *toWrapper,
                                            const std::string &withName)
{
   bool retval = false;

   if (toWrapper == NULL) return false;
   
   if (toWrapper->GetWrapperType() == Gmat::ARRAY_WT)
   {
      throw CommandException("A value of type \"Array\" on command \"" + typeName + 
                  "\" is not currently an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }
   //if (toWrapper->GetWrapperType() == Gmat::STRING_OBJECT_WT)
   //{
   //   throw CommandException("A value of type \"String Object\" on command \"" + typeName + 
   //               "\" is not an allowed value.\nThe allowed values are:"
   //               " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   //}
   CheckDataType(toWrapper,Gmat::REAL_TYPE, "NonlinearConstraint", true);
   
   #ifdef DEBUG_NLC_WRAPPER_CODE   
   MessageInterface::ShowMessage("   Setting wrapper \"%s\" on NLC command\n", 
      withName.c_str());
   #endif

   if (arg1Name == withName)
   {
      arg1   = toWrapper;
      retval = true;
   }
   
   if (arg2Name == withName)
   {
      arg2   = toWrapper;
      retval = true;
   }
       
   if (toleranceName == withName)
   {
      tolerance = toWrapper;
      retval    = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
void NonlinearConstraint::ClearWrappers()
{
   std::vector<ElementWrapper*> temp;
   if (arg1)
   {
      temp.push_back(arg1);
      arg1 = NULL;
   }
   if (arg2)
   {
      if (find(temp.begin(), temp.end(), arg2) == temp.end())
      {
         temp.push_back(arg2);
         arg2 = NULL;
      }
   }
   if (tolerance)
   {
      if (find(temp.begin(), temp.end(), tolerance) == temp.end())
      {
         temp.push_back(tolerance);
         tolerance = nullptr;
      }
   }
   
   ElementWrapper *wrapper;
   for (UnsignedInt i = 0; i < temp.size(); ++i)
   {
      wrapper = temp[i];
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (wrapper, wrapper->GetDescription(), "NonlinearConstraint::ClearWrappers()",
          GetTypeName() + " deleting wrapper");
      #endif
      delete wrapper;
   }
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the NonlinearConstraint command.
 *
 * @return true if the SolverSequenceCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool NonlinearConstraint::Initialize()
{
   #if DEBUG_NONLINEAR_CONSTRAINT_INIT
   MessageInterface::ShowMessage
      ("NonlinearConstraint::Initialize() entered. interpreted=%d, optimizer=%p, "
       "arg1=%p, arg2=%p, tolerance=%p\n", interpreted, optimizer, arg1, arg2,
       tolerance);
   #endif

   // Commented out since this caused crash when this command created
   // from the GUI and run (loj: 2007.05.15)
   //if (!interpreted)
   //   if (!InterpretAction())
   //      throw CommandException(
   //         "NonlinearConstraint: error interpreting input data\n");
   
   bool retval = SolverSequenceCommand::Initialize();

   if (optimizer == NULL)
      throw CommandException(
         "Optimizer not initialized for NonlinearConstraint command\n  \""
         + generatingString + "\"\n");
   
   // Set references for the wrappers   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARAM
      MessageInterface::ShowMessage("Setting refs for arg1\n");
   #endif
   if (SetWrapperReferences(*arg1) == false)
      return false;
   CheckDataType(arg1, Gmat::REAL_TYPE, "NonlinearConstraint");
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARAM
      MessageInterface::ShowMessage("Setting refs for arg2\n");
   #endif
   if (SetWrapperReferences(*arg2) == false)
      return false;
   CheckDataType(arg2, Gmat::REAL_TYPE, "NonlinearConstraint");

   if (tolerance)
   {
      if (SetWrapperReferences(*tolerance) == false)
         return false;
      CheckDataType(tolerance, Gmat::REAL_TYPE, "NonlinearConstraint");

      toleranceWarningIssued = false;
   }

   // The optimizer cannot be finalized until all of the loop is initialized
   optimizerDataFinalized = false;

   #if DEBUG_NONLINEAR_CONSTRAINT_INIT
   MessageInterface::ShowMessage
      ("NonlinearConstraint::Initialize() exiting. optimizer=%p, arg1=%p, "
            "arg2 = %p, tolerance = %p\n", optimizer, arg1, arg2, tolerance);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * NonlinearConstraint the variables defined for this targeting loop.
 *
 * This method (will eventually) feeds data to the targeter state machine in 
 * order to determine the variable values needed to achieve the user specified
 * goals.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool NonlinearConstraint::Execute()
{
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
   MessageInterface::ShowMessage
      ("NonlinearConstraint::Execute() optimizerDataFinalized=%d\n, optimizer="
            "%s, arg1=%p, arg2=%p, tolerance = %p\n", optimizerDataFinalized,
            optimizer, arg1, arg2, tolerance);
   MessageInterface::ShowMessage
      ("   arg1Name=%s\n", arg1Name.c_str());
   MessageInterface::ShowMessage
      ("   arg2Name=%s\n", arg2Name.c_str());
   MessageInterface::ShowMessage
      ("   toleranceName=%s\n", toleranceName.c_str());
   #endif
   
   bool retval = true;
   if (!optimizerDataFinalized) 
   {
      // Tell the optimizer about the constraint
      Real conData;
      conData = 54321.0;
      constraintId = optimizer->SetSolverResults(&conData, arg1Name,
                     isIneqString);

      optimizerDataFinalized = true;
      return retval;
   }
   
   //#ifdef DEBUG_NLC_VALUES
   //   MessageInterface::ShowMessage("NLC:Execute - desiredValue = %.12f\n",
   //      desiredValue);
   //#endif
   
   Integer ineqSign = 0;
   Real constraintValue = -999.99;
   // Evaluate variable and pass it to the optimizer
   if ( (arg1 != NULL) && (arg2 != NULL) )
   {
      desiredValue    = arg2->EvaluateReal();
      constraintValue = arg1->EvaluateReal();

      // Get and save current tolerance value
      if (tolerance)
         toleranceValue = tolerance->EvaluateReal();

      panelDesired = desiredValue;
      panelAchieved = constraintValue;

      #ifdef DEBUG_NLC_VALUES
         MessageInterface::ShowMessage("NLC:Execute - desiredValue = %.12f\n",
            desiredValue);
         MessageInterface::ShowMessage("NLC:Execute - (1) constraintValue = %.12f\n",
            constraintValue);
      #endif
      switch (op)
      {
         case EQUAL:
            constraintValue = constraintValue - desiredValue;
            if (tolerance)
               deltaTolerance  = fabs(constraintValue) - toleranceValue;
            ineqSign = 0;
            break;
         case LESS_THAN_OR_EQUAL:
            constraintValue = constraintValue - desiredValue;
            if (tolerance)
               deltaTolerance  = constraintValue - toleranceValue;
            ineqSign = -1;
            break;
         case GREATER_THAN_OR_EQUAL:
            constraintValue = desiredValue - constraintValue;
            if (tolerance)
               deltaTolerance  = toleranceValue - constraintValue;
            ineqSign = 1;
            break;
         default:
            break;
      }
      #ifdef DEBUG_NLC_VALUES
         MessageInterface::ShowMessage("NLC:Execute - (2) constraintValue now = %.12f\n",
            constraintValue);
         MessageInterface::ShowMessage(
            "NonlinearConstraint::Execute: ID = %d, IneqString = %s Sign=%d desired = %.16f, achieved = %.16f, delta = %.16f, expected =  %.16f\n", 
            constraintId, isIneqString.c_str(), ineqSign, desiredValue, panelAchieved, 
            desiredValue-panelAchieved, constraintValue);

         if (tolerance)
            MessageInterface::ShowMessage("Tolerance: %lf  DeltaTolerance: %le\n",
                  toleranceValue, deltaTolerance);
      #endif

      optimizer->SetResultValue(constraintId, constraintValue, isIneqString);

      Real within = -1.0;

      if (tolerance)
      {
         within = tolerance->EvaluateReal();
         if ((within <= 0.0) && !toleranceWarningIssued)
         {
            std::string warning = "**** WARNING **** The tolerance on the "
                  "nonlinear constraint ";
            warning += arg1Name + " is not positive.  The feasibility "
                  "tolerance will be used instead.\n";
            MessageInterface::ShowMessage(warning.c_str());

            toleranceWarningIssued = true;
         }
      }

      ((Optimizer *) optimizer)->SetConstraintValues(constraintId, desiredValue, 
         panelAchieved, ineqSign, within);

      #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
         MessageInterface::ShowMessage
            ("   constraint=%s, %p\n", constraint->GetTypeName().c_str(), constraint);
         MessageInterface::ShowMessage("   Parameter target: %s constraintValue = %lf\n",
            constraint->GetTypeName().c_str(), constraintValue);
      #endif
   }
   else 
   {
      // isn't this an error???
      //constraintValue = -999.99;  // what to do here?
      //constraintValue = constraintObject->GetRealParameter(parmId);  // again,no clue
      optimizer->SetResultValue(constraintId, constraintValue, isIneqString);
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
         MessageInterface::ShowMessage("   Object target: constraintValue = %lf\n", 
         constraintValue);
      #endif
   }

   BuildCommandSummary(true);
   
   return retval;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void NonlinearConstraint::RunComplete()
{
   #ifdef DEBUG_NLC_EXECUTE
      MessageInterface::ShowMessage(
      "In NLC::RunComplete, optimizerDataFinalized = %s, ... now setting it to false\n",
      (solverDataFinalized? "true" : "false"));
   #endif
   optimizerDataFinalized = false;
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
 * @return The script line that, when interpreted, defines this NonlinearConstraint command.
 */
//------------------------------------------------------------------------------
const std::string& NonlinearConstraint::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   //std::stringstream tol;  // may need tolerance in the future
   //tol << tolerance;
   std::string opString = OP_STRINGS[(Integer)op];
   //if (op == EQUAL)                   opString = "=";
   //else if (op == LESS_THAN_OR_EQUAL) opString = "<=";
   //else                               opString = ">=";
   //std::string gen = prefix + "NonlinearConstraint " + optimizerName + "(" + 
   //                  constraintName +  opString + nlcParmName + 
   //                  ", {Tolerance = " + tol.str() + "}";
   std::string gen = prefix + "NonlinearConstraint " + optimizerName + "(" + 
                     arg1Name +  opString + arg2Name; 

   if (toleranceName != "")
   {
      gen += ", {Tolerance = " + toleranceName + "}";
   }
 
   generatingString = gen + ");";
   // Then call the base class method
   return SolverSequenceCommand::GetGeneratingString(mode, prefix, useName);
}


