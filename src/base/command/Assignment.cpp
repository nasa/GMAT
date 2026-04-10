//$Id$
//------------------------------------------------------------------------------
//                                Assignment
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
// Created: 2004/02/03
//
/**
 * Implementation for the Assignment command class.
 * 
 * The assignment class is the Command class that handles commands of the form
 * 
 *     GMAT object.parameter = value;
 *     GMAT sat.thruster1.ThrustDirection1 = value; (Setting to Parameter)
 *     GMAT variable = parameter;
 *     GMAT variable = equation;
 *
 */
//------------------------------------------------------------------------------


#include "Assignment.hpp"
#include "MathTree.hpp"
#include "StringUtil.hpp"
#include "TextParser.hpp"
#include "NumberWrapper.hpp"
#include "ArrayWrapper.hpp"
#include "ArrayElementWrapper.hpp"
#include "ObjectWrapper.hpp"
#include "StringWrapper.hpp"
#include "FunctionManager.hpp"
#include "MessageInterface.hpp"
#include "ObjectPropertyWrapper.hpp"
#include "GmatGlobal.hpp"               // for IsWritingGmatKeyword()

//#define DEBUG_RENAME
//#define DEBUG_EVAL_RHS
//#define DEBUG_ASSIGNMENT_IA
//#define DEBUG_ASSIGNMENT_SET
//#define DEBUG_ASSIGNMENT_INIT
//#define DEBUG_ASSIGNMENT_EXEC
//#define DEBUG_RUN_MATH_TREE 1
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_FUNCTION 1
//#define DEBUG_OBJECT_MAP
//#define DEBUG_ASSIGN_CALLING_FUNCTION
//#define DEBUG_CLONE_UPDATES
//#define DEBUG_VALIDATION
//#define DEBUG_SETTABILITY
//#define DEBUG_PROPERTY_CHANGE

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif
//#ifndef DEBUG_TRACE
//#define DEBUG_TRACE
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif
#ifdef DEBUG_TRACE
#include <ctime>                 // for clock()
#endif

class MathTree;


//------------------------------------------------------------------------------
//  Assignment()
//------------------------------------------------------------------------------
/**
 * Constructs the Assignment command (default constructor).
 */
//------------------------------------------------------------------------------
Assignment::Assignment  () :
   GmatCommand                   ("GMAT"),
   lhs                           ("Not_Set"),
   settabilityError              (""),
   omitLHSBecauseOfSettability   (false),
   rhs                           ("Not_Set"),
   lhsWrapper                    (NULL),
   rhsWrapper                    (NULL),
   rhsEquation                   (NULL),
   lhsOwner                      (NULL),
   lhsOwnerID                    (-1)
{
   objectTypeNames.push_back("GMAT");
   objectTypeNames.push_back("Assignment");
   
   // Changed to show summary data (LOJ: 2012.04.19)
   includeInSummary = true;
}


//------------------------------------------------------------------------------
//  ~Assignment()
//------------------------------------------------------------------------------
/**
 * Destroys the Assignment command (destructor).
 */
//------------------------------------------------------------------------------
Assignment::~Assignment()
{
   ClearMathTree();
}


//------------------------------------------------------------------------------
//  Assignment(const Assignment& a)
//------------------------------------------------------------------------------
/**
 * Constructs the Assignment command (copy constructor).
 *
 * @param <a> The instance that is copied.
 */
//------------------------------------------------------------------------------
Assignment::Assignment  (const Assignment& a) :
   GmatCommand                   (a),
   lhs                           (a.lhs),
   settabilityError              (""),
   omitLHSBecauseOfSettability   (false),
   rhs                           (a.rhs),
   lhsWrapper                    (NULL),
   rhsWrapper                    (NULL),
   rhsEquation                   (a.rhsEquation),  // Should be NULL
   lhsOwner                      (NULL),
   lhsOwnerID                    (a.lhsOwnerID)
{
}


//------------------------------------------------------------------------------
//  Assignment& operator=(const Assignment& a)
//------------------------------------------------------------------------------
/**
 * Assignment assignment operator.
 *
 * @param <a> The instance that is copied.
 *
 * @return This instance, set to match the input instance.
 */
//------------------------------------------------------------------------------
Assignment& Assignment::operator=(const Assignment& a)
{
   if (this == &a)
      return *this;
   
   lhs                         = a.lhs;
   settabilityError            = "";
   omitLHSBecauseOfSettability = false;
   rhs                         = a.rhs;
   lhsWrapper                  = NULL;
   rhsWrapper                  = NULL;
   rhsEquation                 = a.rhsEquation;  // Should be deleted and set NULL
   lhsOwner                    = NULL;
   lhsOwnerID                  = a.lhsOwnerID;
   
   return *this;
}


//------------------------------------------------------------------------------
// MathTree* GetMathTree()
//------------------------------------------------------------------------------
MathTree* Assignment::GetMathTree()
{
   MathTree *retval = NULL;
   if (rhsEquation != NULL)
      retval = rhsEquation->GetMathTree(false);
   return retval;
}

//------------------------------------------------------------------------------
// bool HasAFunction()
//------------------------------------------------------------------------------
bool Assignment::HasAFunction()
{
   const StringArray fNames = GetGmatFunctionNames();
   if (fNames.empty()) return false;
   return true;
}

//------------------------------------------------------------------------------
// const StringArray& GetGmatFunctionNames()
//------------------------------------------------------------------------------
const StringArray& Assignment::GetGmatFunctionNames()
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("Assignment::GetGmatFunctionNames() entered\n");
   #endif
   static StringArray functionNames;
   functionNames.clear();
   
   if (rhsEquation)
   {
      functionNames = rhsEquation->GetMathTree(false)->GetGmatFunctionNames();
      #ifdef DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("Assignment::GetGmatFunctionNames() returning %d GmatFunctions "
          " from the MathTree\n", functionNames.size());
      #endif
      return functionNames;
   }
   else
   {
      #ifdef DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("Assignment::GetGmatFunctionNames() returning 0 GmatFunctions, "
          "It's not a MathTree\n");
      #endif
      return functionNames;
   }
}


//------------------------------------------------------------------------------
// void SetMathWrappers()
//------------------------------------------------------------------------------
void Assignment::SetMathWrappers()
{
   // Set math Wrapper map
   if (rhsEquation)
      rhsEquation->GetMathTree(false)->SetMathWrappers(&mathWrapperMap);
}


//------------------------------------------------------------------------------
// virtual void SetFunction(Function *function)
//------------------------------------------------------------------------------
void Assignment::SetFunction(Function *function)
{
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("Assignment::SetFunction() function=%p, type = %s\n", function,
            (function == NULL ? "NULL function" :
             function->GetTypeName().c_str()));
   #endif
   
   if (rhsEquation && (function != NULL))
      rhsEquation->GetMathTree(false)->SetFunction(function);
   
   #ifdef DEBUG_FUNCTION
   MessageInterface::ShowMessage("Assignment::SetFunction() returning\n");
   #endif
}


//------------------------------------------------------------------------------
// std::vector<Function*> GetFunctions()
//------------------------------------------------------------------------------
std::vector<Function*> Assignment::GetFunctions() const
{
   if (rhsEquation)
      return rhsEquation->GetMathTree(false)->GetFunctions();
   
   std::vector<Function*> gf;
   return gf;
}


//------------------------------------------------------------------------------
// void SetPublisher(Publisher *pub)
//------------------------------------------------------------------------------
/**
 *  Passes the Publisher used by the Sandbox to FunctionManager through
 *  MathTree's FunctionRunner node.
 *
 *  @param <pub> The publisher
 */
//------------------------------------------------------------------------------
void Assignment::SetPublisher(Publisher *pub)
{
   GmatCommand::SetPublisher(pub);
   if (rhsEquation)
   {
      #ifdef DEBUG_PUBLISHER
      MessageInterface::ShowMessage
         ("Assignment::SetPublisher() setting publiser <%p> to MathTree\n");
      #endif
      rhsEquation->GetMathTree(false)->SetPublisher(pub);
   }
}


//------------------------------------------------------------------------------
//  void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local solar system for the GmatCommand
 * 
 * @param <ss> Pointer to the local solar system
 */
//------------------------------------------------------------------------------
void Assignment::SetSolarSystem(SolarSystem *ss)
{
   GmatCommand::SetSolarSystem(ss);
   
   if (rhsEquation)
      rhsEquation->GetMathTree(false)->SetSolarSystem(ss);
}
 
 
//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * @see GmatCommand
 */
//------------------------------------------------------------------------------
void Assignment::SetInternalCoordSystem(CoordinateSystem *cs)
{
   if (cs == NULL)
      return;
   
   GmatCommand::SetInternalCoordSystem(cs);
   
   if (rhsEquation)
   {
      #ifdef DEBUG_ASSIGNMENT_SET
      MessageInterface::ShowMessage
         ("Assignment::SetInternalCoordSystem() this=<%p>'%s'\n   internalCS=<%p>, "
          "rhsEquation=<%p>\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
          internalCoordSys, rhsEquation);
      #endif
      rhsEquation->GetMathTree(false)->SetInternalCoordSystem(internalCoordSys);
   }
}


//------------------------------------------------------------------------------
//  void SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
 * Passes the transient force vector into the commands that need them
 * 
 * @param tf The vector of transient forces
 * 
 * @note The default behavior in the GmatCommands is to ignore the vector.
 */
//------------------------------------------------------------------------------
void Assignment::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   GmatCommand::SetTransientForces(tf);
   
   if (rhsEquation)
      rhsEquation->GetMathTree(false)->SetTransientForces(forces);
}


//------------------------------------------------------------------------------
//  void SetObjectMap(ObjectMap *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local asset store used by the GmatCommand
 * 
 * @param <map> Pointer to the local object map
 */
//------------------------------------------------------------------------------
void Assignment::SetObjectMap(ObjectMap *map)
{
   GmatCommand::SetObjectMap(map);
   
   if (rhsEquation)
      rhsEquation->GetMathTree(false)->SetObjectMap(map);
}


//------------------------------------------------------------------------------
//  void SetGlobalObjectMap(ObjectMap *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the global asset store used by the GmatCommand
 * 
 * @param <map> Pointer to the global object map
 */
//------------------------------------------------------------------------------
void Assignment::SetGlobalObjectMap(ObjectMap *map)
{
   #ifdef DEBUG_GLOBAL_OBJECT_MAP
   MessageInterface::ShowMessage
      ("Assignment::SetGlobalObjectMap() entered, map=<%p>\n", map);
   #endif
   
   GmatCommand::SetGlobalObjectMap(map);
   
   if (rhsEquation)
      rhsEquation->GetMathTree(false)->SetGlobalObjectMap(map);
   
   #ifdef DEBUG_GLOBAL_OBJECT_MAP
   MessageInterface::ShowMessage("Assignment::SetGlobalObjectMap() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * Assignment lines have the form
 *
 *    GMAT Sat.Element1 = 7654.321;
 *
 * or
 *
 *    GMAT object1 = object2;
 *
 * This method breaks this line into its constituent pieces.
 *
 * @return true on successful parsing of the command.
 *
 * @todo: Determine if Assignment::InterpretAction can be moved into the
 *        Interpreter
 */
//------------------------------------------------------------------------------
bool Assignment::InterpretAction()
{
   #ifdef DEBUG_ASSIGNMENT_IA
   MessageInterface::ShowMessage
      ("\nAssignment::InterpretAction() entered, currentFunction=<%p> '%s'\n",
       currentFunction,
       currentFunction ? currentFunction->GetFunctionPathAndName().c_str() : "NULL");
   #endif
   
   StringArray chunks = InterpretPreface();
   
   #ifdef DEBUG_ASSIGNMENT_IA
      MessageInterface::ShowMessage("Preface chunks as\n");
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
   #endif
      
   lhs = chunks[0];
   rhs = chunks[1]; 
   
   #ifdef DEBUG_ASSIGNMENT_IA
   MessageInterface::ShowMessage
      ("Checking if LHS has single quote or invalid brackets\n");
   #endif
   // check if there is single quote in LHS(loj: 2008.07.22)
   if (lhs.find("'") != lhs.npos)
      throw CommandException("An assignment command is not allowed to contain "
                             "single quote on the left-hand-side"); 
   
   if (!GmatStringUtil::HasNoBrackets(lhs,true))
      throw CommandException("An assignment command is not allowed to contain "
         "brackets, braces, or parentheses (except to indicate an array "
         "element) on the left-hand-side");
   
   #ifdef DEBUG_ASSIGNMENT_IA
   MessageInterface::ShowMessage("Checking if LHS has unexpected commas\n");
   #endif
   // check for unexpected commas on the left-hand-side
   Integer commaPos = -1;
   if (lhs.find(',') != lhs.npos)
   {
      GmatStringUtil::GetArrayCommaIndex(lhs, commaPos);
      if (commaPos == -1)
         throw CommandException("Command contains an unexpected comma on "
               "left-hand-side");
   }

   bool retval = ParseRHS(rhs);
   
   #ifdef DEBUG_ASSIGNMENT_IA
   MessageInterface::ShowMessage("Assignment::InterpretAction() returning true\n");
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// const StringArray& Assignment::GetObjectList()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of objects referenced in the Assignment command
 *
 * @return The object names
 */
//------------------------------------------------------------------------------
const StringArray& Assignment::GetObjectList()
{
   objects.clear();
   if (lhsWrapper != NULL)
      objects = lhsWrapper->GetRefObjectNames();
   else
      if (!omitLHSBecauseOfSettability)
         objects.push_back(lhs);
   if (rhsWrapper != NULL)
   {
      StringArray rhsNames = rhsWrapper->GetRefObjectNames();
      for (UnsignedInt i = 0; i < rhsNames.size(); ++i)
         objects.push_back(rhsNames[i]);
   }
   else if (rhsEquation == NULL)
   {
      objects.push_back(rhs);
   }
   else
   {
//      StringArray mathObjects =
//            rhsEquation->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
//      for (UnsignedInt i = 0; i < mathObjects.size(); ++i)
//         objects.push_back(mathObjects[i]);
   }

   #ifdef DEBUG_OBJECT
      MessageInterface::ShowMessage("Assignment::GetObjectList() Found %d "
            "object names:\n", objects.size());
      for (UnsignedInt i = 0; i < objects.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", objects[i].c_str());
   #endif

   return objects;
}


//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
/**
 * Checks the command for internal consistency during the final pass of script
 * parsing.
 *
 * @return true if no issues were detected, false otherwise
 */
//------------------------------------------------------------------------------
bool Assignment::Validate()
{
   bool retval = true;

   #ifdef DEBUG_VALIDATION
      MessageInterface::ShowMessage
         ("\nAssignment::Validate() '%s' entered\n", (lhs + " = " + rhs).c_str());
      MessageInterface::ShowMessage
         ("Assignment command has lhs = %s, rhs = %s\n", lhs.c_str(), rhs.c_str());
      MessageInterface::ShowMessage
         ("Validate has lhsWrapper = <%p>, rhsWrapper = <%p>, rhsEquation = <%p>\n",
          lhsWrapper, rhsWrapper, rhsEquation);
   #endif
   if (lhsWrapper != NULL)
   {
      Gmat::ParameterType lhsDataType = lhsWrapper->GetDataType();
      if (rhsWrapper != NULL)
      {
         Gmat::ParameterType rhsDataType = rhsWrapper->GetDataType();
         if (lhsWrapper->GetWrapperType() != rhsWrapper->GetWrapperType())
         {
            #ifdef DEBUG_VALIDATION
               MessageInterface::ShowMessage("Wrapper types don't match:\n");
               MessageInterface::ShowMessage
                  ("   lhsDataType: %d, rhsDataType: %d\n", lhsDataType, rhsDataType);
               ShowWrapper("   ", "lhsWrapper:", lhsWrapper);
               ShowWrapper("   ", "rhsWrapper:", rhsWrapper);               
               MessageInterface::ShowMessage("   Checking compatibility of %s "
                     "with %s\n", PARAM_TYPE_STRING[lhsDataType].c_str(),
                     PARAM_TYPE_STRING[rhsDataType].c_str());
            #endif
            if (lhsDataType != rhsDataType)
            {
               // Initially set last error message, if validated ok then blank out.
               lastErrorMessage = "Types of left and right of the equal sign are not compatible.";
               
               // Object = string is handled separately
               if ((lhsDataType == Gmat::OBJECT_TYPE) ||
                   (lhsDataType == Gmat::OBJECTARRAY_TYPE))
               {
                  if ((rhsDataType != Gmat::STRING_TYPE) &&
                      (rhsDataType != Gmat::STRINGARRAY_TYPE) &&
                      (rhsDataType != Gmat::OBJECT_TYPE) &&
                      (rhsDataType != Gmat::OBJECTARRAY_TYPE))
                     retval = false;
                  #ifdef DEBUG_VALIDATION
                  else
                     MessageInterface::ShowMessage("Assuming object array = string "
                           "okay for now\n");
                  #endif
                  // Check to see if we are allowed to set a string on this
                  // object property
                  bool canAssignString = true;
                  GmatBase *lhsObj = lhsWrapper->GetRefObject();
                  Integer  lhsID   = -1;
                  if (lhsWrapper->GetWrapperType() == Gmat::OBJECT_PROPERTY_WT)
                     lhsID = ((ObjectPropertyWrapper*)(lhsWrapper))->GetPropertyId();
                  if ((lhsObj != NULL) && (lhsID >= 0))
                     canAssignString = lhsObj->CanAssignStringToObjectProperty(lhsID);
                  if (!canAssignString && ((rhsDataType == Gmat::STRING_TYPE) ||
                        (rhsDataType == Gmat::STRINGARRAY_TYPE)))
                  {
                     retval = false;
                  }
               }
               else if (lhsDataType == Gmat::STRING_TYPE)
               {
                  // Allow setting String, Number, Variable, or ArrayElement to
                  // String.  This fixes bug 1340.
                  if (rhsDataType == Gmat::STRING_TYPE)
                     retval = true;
                  else if (rhsDataType == Gmat::OBJECT_TYPE) // can use name of object
                     retval = true;
                  else if (rhsDataType == Gmat::REAL_TYPE)
                  {
                     retval = true;
                     // Check if left is ParameterWrapper and it is a time parameter returning
                     // string, such as Gregorian time, then it is not allowed
                     // (GMT-2543 fix)
                     if (lhsWrapper->GetWrapperType() ==  Gmat::PARAMETER_WT)
                     {
                        Parameter *param = (Parameter*)(lhsWrapper->GetRefObject());
                        if (param->IsTimeParameter())
                           retval = false;
                     }
                  }
                  else
                     retval = false;
               }
               else if (lhsDataType == Gmat::FILENAME_TYPE)
               {
                  if ((rhsDataType != Gmat::STRING_TYPE) )
                     retval = false;
               }
               else if (lhsDataType == Gmat::ENUMERATION_TYPE ||
                        lhsDataType == Gmat::COLOR_TYPE) // For color setting (LOJ: 2013.11.22)
               {
                  if ((rhsDataType != Gmat::STRING_TYPE) )
                     retval = false;
               }
               else if (lhsDataType == Gmat::REAL_TYPE)
               {
                  retval = false;
                  if ((rhsDataType != Gmat::STRING_TYPE) &&
                      (rhsDataType != Gmat::REAL_TYPE))
                  {
                     // Setting one element array to a variable is ok, set it to
                     // true so that it can be checked at run time
                     if (rhsDataType == Gmat::RMATRIX_TYPE)
                     {
                        // Check if RHS is 1x1 array
                        if (rhsWrapper->GetWrapperType() == Gmat::ARRAY_WT)
                        {
                           retval = true;
                           Array *rhsArr = (Array*)(rhsWrapper->GetRefObject());
                           #ifdef DEBUG_VALIDATION
                           MessageInterface::ShowMessage
                              ("   Checking RHS for 1x1 array, rhsArr=<%p>\n", rhsArr);
                           #endif
                           if (rhsArr)
                           {
                              #ifdef DEBUG_VALIDATION
                              MessageInterface::ShowMessage
                                 ("   rowCount=%d, colCount=%d\n", rhsArr->GetRowCount(), rhsArr->GetColCount());
                              #endif
                              if (rhsArr->GetRowCount() != 1 || rhsArr->GetColCount() != 1)
                              {
                                 lastErrorMessage = "Right of the equal sign is not 1x1 Array.";
                                 retval = false;
                              }
                           }
                           else
                           {
                              lastErrorMessage = "Right of the equal sign is not valid.";
                              retval = false;
                           }
                        }
                     }
                  }
               }
               else if (lhsDataType == Gmat::RMATRIX_TYPE)
               {
                  retval = ValidateRmatrix(lhsWrapper, rhsWrapper);
               }
               else if (lhsDataType == Gmat::RVECTOR_TYPE) // LOJ: added 2014.02.11
               {
                  retval = ValidateRvector(lhsWrapper, rhsWrapper);
               }
               else if (lhsDataType == Gmat::INTEGER_TYPE)
               {
                  if (rhsDataType == Gmat::INTEGER_TYPE || // Redundant but safe
                      rhsDataType == Gmat::REAL_TYPE)
                     retval = true;
                  else
                     retval = false;
               }
               else if (lhsDataType == Gmat::RVECTOR_TYPE)
               {
                  if ((rhsDataType == Gmat::STRING_TYPE) ||
                      (rhsDataType == Gmat::RVECTOR_TYPE))
                     retval = true;
                  else
                     retval = false;
               }
               else if (lhsDataType == Gmat::BOOLEAN_TYPE ||
                        lhsDataType == Gmat::ON_OFF_TYPE)
               {
                  #ifdef DEBUG_VALIDATION
                  MessageInterface::ShowMessage
                     ("   We now allow case insensitive True/False/On/Off for BOOLEAN_TYPE "
                      "and OF_OFF_TYPE (2012.06.15)\n");
                  #endif
                  if (rhsDataType == Gmat::BOOLEAN_TYPE || rhsDataType == Gmat::ON_OFF_TYPE)
                     retval = true;
                  else
                     retval = false;
               }
               else
               {
                  #ifdef DEBUG_VALIDATION
                     MessageInterface::ShowMessage("Type mismatch in the line "
                           "\"%s\"; %d != %d; wrappers are %d and %d\n",
                           GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
                           lhsDataType, rhsDataType,
                           lhsWrapper->GetWrapperType(),
                           rhsWrapper->GetWrapperType());
                  #endif
                  retval = false;
               }
            }
         }
         else if (lhsWrapper->GetWrapperType() == Gmat::ARRAY_WT &&
                  rhsWrapper->GetWrapperType() == Gmat::ARRAY_WT)
         {
            #ifdef DEBUG_VALIDATION
            MessageInterface::ShowMessage("   Both sides are whole array\n");
            #endif
            Array *lhsArr = (Array*)(lhsWrapper->GetRefObject());
            Array *rhsArr = (Array*)(rhsWrapper->GetRefObject());
            
            if (lhsArr && rhsArr)
            {
               if ((lhsArr->GetRowCount() == rhsArr->GetRowCount()) &&
                   (lhsArr->GetColCount() == rhsArr->GetColCount()))
               {
                  retval = true;
               }
               else
               {
                  lastErrorMessage = "Array sizes of left and right of the "
                        "equal sign are not the same.";
                  retval = false;
               }
            }
            else
            {
               std::string which;
               if (!lhsArr && !rhsArr)
                  which = "Left and right";
               else if (!lhsArr)
                  which = "Left";
               else
                  which = "Right";
               
               lastErrorMessage = which + " of the equal sign is not valid.";
               retval = false;
            }
         }
         else if (lhsWrapper->GetWrapperType() == Gmat::ARRAY_ELEMENT_WT &&
                  rhsWrapper->GetWrapperType() == Gmat::ARRAY_ELEMENT_WT)
         {
            retval = ValidateArrayElement(lhsWrapper, rhsWrapper);
         }
      }
      else
      {
         if (lhsDataType == Gmat::STRING_TYPE && rhsEquation)
         {
            std::string msg;
            retval = rhsEquation->ValidateStringExpression(msg);
         }
         else if (rhsEquation == NULL)
         {
            // rhs Wrappers should have been set by now, so validation failed
            lastErrorMessage = "Right of the equal sign is not valid.";
            retval = false;
         }
         else
         {
            // Validate math tree
            std::string msg;
            retval = rhsEquation->Validate(msg);
            lastErrorMessage = msg;
         }
      }
   }
   else
   {
      // lhs Wrapper should have been set by now, so validation failed
      // Check for rhs Wrappers or rhs math tree first
      if (!rhsWrapper && !rhsEquation)
         lastErrorMessage = "Right of the equal sign is not valid.";
      else if (rhsWrapper || rhsEquation)
      {
         if (settabilityError.size() > 0)
            lastErrorMessage = settabilityError;
         else
            lastErrorMessage = "Left of the equal sign is not valid.";
      }
      retval = false;
   }
   
   if (retval)
      lastErrorMessage = "";
   
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage
      ("Assignment::Validate() '%s' returning %d\n", (lhs + " = " + rhs).c_str(), retval);
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Assignment command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Assignment::Initialize()
{
   #ifdef DEBUG_ASSIGNMENT_INIT
   MessageInterface::ShowMessage
      ("Assignment::Initialize() entered for <%s>, It's%s a math equation\n",
       generatingString.c_str(), (rhsEquation == NULL ? " not" : ""));
   ShowWrapper("   ", "lhsWrapper:", lhsWrapper);
   ShowWrapper("   ", "rhsWrapper:", rhsWrappmathTreeer);
   #endif
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps();
   #endif
   
   if (lhsWrapper == NULL || (rhsEquation == NULL && rhsWrapper == NULL))
      return false;
   
   if (GmatCommand::Initialize() == false)
      return false;
   
   // if rhs is not an equation, set ref obj on both lhs and rhs wrappers
   if (rhsEquation == NULL)
   {
      // Set references for the wrappers
      if (SetWrapperReferences(*lhsWrapper) == false)
         return false;
      
      if (SetWrapperReferences(*rhsWrapper) == false)
         return false;
   }
   else
   {
      // Set references for the lhs wrapper
      if (SetWrapperReferences(*lhsWrapper) == false)
         return false;
      
      std::map<std::string, ElementWrapper *>::iterator ewi;
      
      #ifdef DEBUG_ASSIGNMENT_INIT
      MessageInterface::ShowMessage("   Calling SetWrapperReferences() for mathWrapperMap\n");
      for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
      {
         ElementWrapper *wrapper = ewi->second;
         MessageInterface::ShowMessage
            ("   name=<%s>, wrapper=<%p>, type=%d\n", (ewi->first).c_str(), wrapper,
             wrapper ? wrapper->GetWrapperType() : -999);
      }
      #endif
      
      // Set references for the rhs math element wrappers
      for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
      {
         if (SetWrapperReferences(*(ewi->second)) == false)
            return false;
      }
      
      #ifdef DEBUG_ASSIGNMENT_INIT
      MessageInterface::ShowMessage
         ("Assignment::Initialize() Initializing topNode=%s, %s\n",
          topNode->GetTypeName().c_str(), topNode->GetName().c_str());
      #endif
      
      std::string fnMsg;
      if (currentFunction != NULL)
      {
          int lineNum = currentFunction->GetScriptLineNumber();
          fnMsg = currentFunction->GetFunctionPathAndName();
          fnMsg = "\n(On line " + std::to_string(lineNum) + " in Function \"" + fnMsg + "\")\n";
      }
      
      try
      {
         // Currently GmatFunction is not allowed in a math equation
         StringArray gmatFunctions = rhsEquation->GetMathTree(false)->GetGmatFunctionNames();
         #ifdef DEBUG_ASSIGNMENT_INIT
         MessageInterface::ShowMessage
            ("Assignment command has %d GmatFunctions\n", gmatFunctions.size());
         for (unsigned int i = 0; i < gmatFunctions.size(); i++)
            MessageInterface::ShowMessage
               ("gmatFunctions[%d] = '%s'\n", i, gmatFunctions[i].c_str());
         #endif
         
         
         // I think I fixed MathParser to parse GmatFunction with math operators,
         // but if any test fails, set this to 1 (LOJ: 2015.10.02)
         //===========================================================
         #if 0
         //===========================================================
         if (gmatFunctions.size() > 0)
         {
            // If any math operator found with GmatFunction, throw an exception
            // for GMT-5262 (LOJ: 2015.10.02)
            if (GmatStringUtil::IsThereMathSymbol(rhs))
            {
               #ifdef DEBUG_ASSIGNMENT_INIT
               MessageInterface::ShowMessage
                  ("Throwing exception: Using GmatFunction in math equation is not "
                   "allowed in this release");
               #endif
               throw CommandException
                  ("Using GmatFunction in math equation is not allowed in this release");
            }
         }
         //===========================================================
         #endif
         //===========================================================
         
         if (!rhsEquation->Initialize(objectMap, globalObjectMap))
            throw CommandException("Failed to initialize math equation or function");
      }
      catch (BaseException &e)
      {
         // Catch above exceptions here
         CommandException ce;
         ce.SetDetails("%s in\n    \"%s\"%s\n", e.GetDetails().c_str(),
                      generatingString.c_str(), fnMsg.c_str());
         throw ce;
      }
   }
   
   lhsOwner = lhsWrapper->GetRefObject();
   if (lhsWrapper->GetWrapperType() == Gmat::OBJECT_PROPERTY_WT)
      lhsOwnerID = ((ObjectPropertyWrapper*)(lhsWrapper))->GetPropertyId();

   #ifdef DEBUG_ASSIGNMENT_INIT
   MessageInterface::ShowMessage("Assignment::Initialize() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * The method that is fired to perform the command.
 *
 * Derived classes implement this method to perform their actions on 
 * GMAT objects.
 *
 * @return true if the Assignment command runs to completion, false if an error 
 *         occurs. 
 */
//------------------------------------------------------------------------------
bool Assignment::Execute()
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage
      (">>>>> CALL TRACE: Assignment::Execute() entered, '%s' Count = %d\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount);
   #endif
   
   #ifdef DEBUG_ASSIGNMENT_EXEC
   MessageInterface::ShowMessage
      ("\nAssignment::Execute() this=<%p> entered, for \"%s\"\n   "
       "callingFunction='%s', internalCS=<%p>\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL",
       internalCoordSys);
   MessageInterface::ShowMessage
      ("   lhsWrapper=<%p>, rhsWrapper=<%p>, rhsEquation=<%p>\n", lhsWrapper, rhsWrapper,
       rhsEquation);
   #endif
   
   if (lhsWrapper == NULL || (rhsWrapper == NULL && rhsEquation == NULL))
   {
      CommandException ce;
      ce.SetMessage("");
      std::string msg = "Assignment::Execute() failed, LHS or/and RHS wrappers "
            "are NULL";
      ce.SetDetails("%s in\n   \"%s\"\n", msg.c_str(), generatingString.c_str());
      throw ce;
   }
   
   ElementWrapper *outWrapper = NULL;
   
   #ifdef DEBUG_CLONE_UPDATES
      GmatBase *ptr = lhsWrapper->GetRefObject();
      MessageInterface::ShowMessage("Assignment LHS object is %s <%p>\n",
            (ptr != NULL ? ptr->GetName().c_str() : "Null PTR"), ptr);
   #endif

   try
   {
      bool retval = false;
      
      // In attempt to fix Func_LibrationSE.script for nested (loj: 2008.10.24)
      // function which doesn't give correct results.
      // We need to determine if ref object shoud be set to lhs to handle setting
      // spacecraft's CoordinateSystem which is not NULL and
      // CoordinateSystem is defined later in the GmatFunction script.
      // GMAT Sat1.CoordinateSystem = EarthSunL1_MJ2000Eq;
      // Since Spacecraft converts initial state to given CoordinateSystem
      // when CoordinateSyatem is not NULL for nested GmatFunction,
      // we don't want to convert until initial state is set.
      // So pass setRefObj to true when this command is not in GmatFunction and
      // if both lhs and rhs are whole objects.
      
      // @todo This code broke routine test script APT_Func_allPropagates on 2nd run,
      // so set setRefObj to true for now
      
      //bool setRefObj = false;
      //if (currentFunction == NULL)
      //   setRefObj = true;
      //else
      //   if (lhsWrapper->GetWrapperType() == Gmat::OBJECT &&
      //       lhsWrapper->GetWrapperType() == rhsWrapper->GetWrapperType())
      //      setRefObj = true;      
      //MessageInterface::ShowMessage("===> setRefObj=%d\n", setRefObj);
      
      bool setRefObj = true;
      
      // Use ElementWrapper static method SetValue() (loj: 2008.06.20)
      if (rhsEquation == NULL)
      {
         // If lhs is object property and is deprecated to remove in the future, ignore
         if (lhsWrapper->GetDataType() == Gmat::UNKNOWN_PARAMETER_TYPE)
         {
            #ifdef DEBUG_ASSIGNMENT_EXEC
            MessageInterface::ShowMessage
               ("Assignment::Execute() returning false, LHS data type is unknown\n");
            #endif
            return true;
         }
         
         #ifdef DEBUG_ASSIGNMENT_EXEC
         MessageInterface::ShowMessage
            ("   Calling ElementWrapper::SetValue() to set value to LHS\n");
         #endif
         retval = ElementWrapper::SetValue(lhsWrapper, rhsWrapper, solarSys, objectMap,
                                           globalObjectMap, setRefObj);
      }
      else
      {
         #ifdef DEBUG_ASSIGNMENT_EXEC_OBJECT_MAP
         ShowObjectMaps("object maps at the start");
         #endif
         
         #ifdef DEBUG_ASSIGNMENT_EXEC
         MessageInterface::ShowMessage("   Calling RunMathTree()\n");
         #endif
         
         outWrapper = rhsEquation->RunMathTree(lhsWrapper);
         if (outWrapper->IsOneDimArraySettingAllowed())
            lhsWrapper->AllowOneDimArraySetting(true);
         
         #ifdef DEBUG_ASSIGNMENT_EXEC
         MessageInterface::ShowMessage
            ("   Calling ElementWrapper::SetValue() to set value to LHS\n");
         #endif
         retval = ElementWrapper::SetValue(lhsWrapper, outWrapper, solarSys, objectMap,
                                           globalObjectMap, setRefObj);
      }
      
      // Check if setting object property
      // Also check if wrapper is a parameter wrapper which may update
      // a Parameter ref object (Fix for GMT-5247, LOJ: 2015.08.27)
      if (lhsWrapper->GetWrapperType() == Gmat::OBJECT_PROPERTY_WT ||
          lhsWrapper->GetWrapperType() == Gmat::OBJECT_WT ||
          lhsWrapper->GetWrapperType() == Gmat::PARAMETER_WT)
         HandleObjectPropertyChange(lhsWrapper);
      
      #ifdef DEBUG_ASSIGNMENT_EXEC
      MessageInterface::ShowMessage("   ElementWrapper::SetValue() returned %d\n", retval);
      #endif
      
      if (!retval)
      {
         if (outWrapper)
         {
            GmatBase *refObj = outWrapper->GetRefObject();
            if (refObj)
            {
               #ifdef DEBUG_MEMORY
               MemoryTracker::Instance()->Remove
                  (refObj, refObj->GetName(), "Assignment::Execute()",
                   //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting outWrapper's refObj");
                   " deleting outWrapper's refObj");
               #endif
               delete refObj;
            }
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (outWrapper, outWrapper->GetDescription(), "Assignment::Execute()",
                //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting outWrapper");
                " deleting outWrapper");
            #endif
            delete outWrapper;
         }
         
         throw CommandException("Assignment::Execute() failed");
      }
   }
   catch (BaseException &e)
   {
      #ifdef DEBUG_ASSIGNMENT_EXEC
      MessageInterface::ShowMessage
         ("   ==> Assignment::Execute() Caught Exception: %s\n", e.GetFullMessage().c_str());
      #endif
      
      // To make error message format consistent, just add "Command Exception:"
      std::string msg = e.GetFullMessage();
      if (msg.find("Exception") == msg.npos && msg.find("exception") == msg.npos)
         msg = "Command Exception: " + msg;
      
      if (callingFunction != NULL)
      {
         std::string funcPath = callingFunction->GetFunction()->GetFunctionPathAndName();
         int lineNum = GetScriptLineNumber();
         msg = msg + "\n(On line " + std::to_string(lineNum) + " in Function \"" + funcPath + "\")";
         MessageInterface::ShowMessage(msg + "\n");
         throw;
      }
      
      if (currentFunction != NULL)
      {
         MessageInterface::ShowMessage(msg + "\n");
      }
      else
      {
         if (outWrapper)
         {
            GmatBase *refObj = outWrapper->GetRefObject();
            if (refObj)
               delete refObj;
            delete outWrapper;
         }
         
         CommandException ce;
         ce.SetMessage("");
         ce.SetDetails("%s in\n   \"%s\"\n", msg.c_str(), generatingString.c_str());
         throw ce;
      }
   }
   
   #ifdef DEBUG_ASSIGNMENT_EXEC
   MessageInterface::ShowMessage("Assignment::Execute() returning true\n");
   #endif
   
   if (outWrapper)
   {
      GmatBase *refObj = outWrapper->GetRefObject();
      if (refObj)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (refObj, refObj->GetName(), "Assignment::Execute()",
             //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting outWrapper's refObj");
             " deleting outWrapper's refObj");
         #endif
         delete refObj;
         refObj = NULL;
      }
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (outWrapper, outWrapper->GetDescription(), "Assignment::Execute()",
          //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting outWrapper");
          " deleting outWrapper");
      #endif
      delete outWrapper;
      outWrapper = NULL;
   }
   
   // Update clones
   PassToClones();
   
   // Build command summary
   BuildCommandSummary(true);
   
   #ifdef DEBUG_TRACE
   clock_t t2 = clock();
   MessageInterface::ShowMessage
      (">>>>> CALL TRACE: Assignment::Execute() exiting, '%s' Count = %d, Run Time: %f seconds\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount,
       (Real)(t2-t1)/CLOCKS_PER_SEC);
   #endif
   return true;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void Assignment::RunComplete()
{
   if (rhsEquation)
   {
      #ifdef DEBUG_RUN_COMPLETE
      MessageInterface::ShowMessage
         ("Assignment::RunComplete() calling MathTree::Finalize()\n");
      #endif
      
      rhsEquation->Finalize();
   }
   
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
//  bool SkipInterrupt()
//------------------------------------------------------------------------------
/**
 * Allows select commands to avoid polling for user interrupts.
 * 
 * @return true if the command can skip polling; false if polling is needed.
 */
//------------------------------------------------------------------------------
bool Assignment::SkipInterrupt()
{
   return true;
}

//------------------------------------------------------------------------------
// void SetCallingFunction();
//------------------------------------------------------------------------------
void Assignment::SetCallingFunction(FunctionManager *fm)
{
   #ifdef DEBUG_ASSIGN_CALLING_FUNCTION
      MessageInterface::ShowMessage("Assignment::SetCallingFunction - fm is %s NULL\n",
            fm? "NOT" : "really");
      MessageInterface::ShowMessage("   and rhsEquation DOES %s exist\n",
            rhsEquation? "really" : "NOT");
   #endif
   GmatCommand::SetCallingFunction(fm);
   if (rhsEquation)
      rhsEquation->SetCallingFunction(fm);
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
/*
 * Returns wrapper object names used for creating wrappers in Validator and
 * used to determine if an object can be deleted.
 *
 * @param completeSet   return names that would not ordinarily be returned
 *
 * @return list of names of wrappers
 *
 */
//------------------------------------------------------------------------------
const StringArray& Assignment::GetWrapperObjectNameArray(bool completeSet)
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Assignment::GetWrapperObjectNameArray() lhs=<%s>, rhs=<%s>, rhsEquation=<%p>\n",
       lhs.c_str(), rhs.c_str(), rhsEquation);
   #endif
   
   wrapperObjectNames.clear();
   
   // If rhs is not an equation, just add rhs
   if (rhsEquation == NULL)
   {
      if (lhs != "" && rhs != "Not_Set")
      {
         // If LHS has more than 1 dot add to the list and Interpreter::ValidateCommand()
         // will figure out if it is settable Parameter or not.(LOJ: 2009.12.22)
         // Changed to use > 0 (LOJ: 2015.02.10)
         //if ((GmatStringUtil::NumberOfOccurrences(lhs, '.') > 1) || completeSet)
         if ((GmatStringUtil::NumberOfOccurrences(lhs, '.') > 0) || completeSet)
            wrapperObjectNames.push_back(lhs);
      }
      
      if (rhs != "" && rhs != "Not_Set")
      {
         wrapperObjectNames.push_back(rhs);
      }
   }
   else
   {
      if (lhs != "" && rhs != "Not_Set")
      {
         // If LHS has more than 1 dot add to the list and Interpreter::ValidateCommand()
         // will figure out if it is settable Parameter or not.(LOJ: 2009.12.22)
         // Changed to use > 0 (LOJ: 2015.02.10) Changed back to >1 for LHS when equation
         if ((GmatStringUtil::NumberOfOccurrences(lhs, '.') > 1) || completeSet)
            wrapperObjectNames.push_back(lhs);
      }
      StringArray wons = rhsEquation->GetWrapperObjectNames();

      #ifdef DEBUG_WRAPPER_CODE
         MessageInterface::ShowMessage("   Got the following from the MathTree:\n");
      #endif

      wrapperObjectNames.insert(wrapperObjectNames.end(), wons.begin(), wons.end());
      for (UnsignedInt i = 0; i < wons.size(); ++i)
      {
         mathWrapperMap[wons[i]] = NULL;
         
         #ifdef DEBUG_WRAPPER_CODE
         MessageInterface::ShowMessage
            ("   Math element %d: '%s'\n", i, wrapperObjectNames[i].c_str());
         #endif
      }
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Assignment::GetWrapperObjectNameArray() returning %d wrapper elements\n",
       wrapperObjectNames.size());
   for (UnsignedInt i=0; i<wrapperObjectNames.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", wrapperObjectNames[i].c_str());
   #endif
   
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool Assignment::SetElementWrapper(ElementWrapper *toWrapper, 
                                   const std::string &withName)
{
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Assignment::SetElementWrapper() entered, toWrapper=<%p>, name='%s'\n   "
       "lhs='%s'\n   rhs='%s', rhsEquation=<%p>\n", toWrapper,withName.c_str(),
       lhs.c_str(), rhs.c_str(), rhsEquation);
   #endif
   
   if (toWrapper == NULL)
   {
      #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage
         ("Assignment::SetElementWrapper() returning false, toWrapper is NULL\n");
      #endif
      return false;
   }
   
   bool retval = false;
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("   Setting wrapper \"%s\" of data type \"%d\" and of wrapper type \"%d\" "
       "on Assignment\n      \"%s\"\n", withName.c_str(), (Integer) (toWrapper->GetDataType()), 
       (Integer) (toWrapper->GetWrapperType()), GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   ElementWrapper *lhsOldWrapper = NULL;
   ElementWrapper *rhsOldWrapper = NULL;
   #ifdef DEBUG_WRAPPER_CODE
      ElementWrapper *rhsNewWrapper = NULL;
   #endif
   
   #ifdef DEBUG_WRAPPER_CODE
   ShowWrapper("   ", "lhsWrapper:", lhsWrapper);
   ShowWrapper("   ", "rhsWrapper:", rhsWrapper);
   #endif
   
   if (withName == lhs)
   {
      // Confirm settability for the LHS
      GmatBase *obj = toWrapper->GetRefObject();
      #ifdef DEBUG_SETTABILITY
         MessageInterface::ShowMessage("   *** Checking to see if %s "
               "is command mode settable...Wrapper has type %d",
               withName.c_str(), toWrapper->GetWrapperType());
      #endif
      if (toWrapper->GetWrapperType() == Gmat::OBJECT_PROPERTY_WT)
      {
         Integer id  =
               ((ObjectPropertyWrapper*)toWrapper)->GetPropertyId();

         #ifdef DEBUG_SETTABILITY
            MessageInterface::ShowMessage("...%s\n",
                  obj->IsParameterCommandModeSettable(id) ? "it is!" :
                        "it is not.");
         #endif

         if (!obj->IsParameterCommandModeSettable(id))
         {
            settabilityError = "The field " +
                  toWrapper->GetDescription() + " cannot be set after "
                  "the Mission Sequence has started";
            lastErrorMessage = settabilityError;
            omitLHSBecauseOfSettability = true;
            #ifdef DEBUG_WRAPPER_CODE
            MessageInterface::ShowMessage
               ("Assignment::SetElementWrapper() returning false, %s\n",
                settabilityError.c_str());
            #endif
            
            return false;
         }
      }
      else if (toWrapper->GetWrapperType() == Gmat::OBJECT_WT)
      {
         #ifdef DEBUG_SETTABILITY
            MessageInterface::ShowMessage("...%s\n",
                  obj->IsCommandModeAssignable() ? "it is!" :
                        "it is not.");
         #endif
         if (!obj->IsCommandModeAssignable())
         {
            settabilityError = "Object Assignment is not allowed in "
                  "the Mission Sequence for " + obj->GetTypeName() +
                  " objects";
            lastErrorMessage = settabilityError;
            omitLHSBecauseOfSettability = true;
            #ifdef DEBUG_WRAPPER_CODE
            MessageInterface::ShowMessage
               ("Assignment::SetElementWrapper() returning false, %s\n",
                settabilityError.c_str());
            #endif
            return false;
         }
      }
      else
      {
         #ifdef DEBUG_SETTABILITY
            MessageInterface::ShowMessage("...Who knows?!?\n");
         #endif
      }

      #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage("   Checking LHS...\n");
      #endif
      // All lhs object property wrapper are settable, so check first
      if (withName.find(".") == withName.npos ||
          (withName.find(".") != withName.npos &&
           toWrapper->GetWrapperType() == Gmat::OBJECT_PROPERTY_WT))
      {
         // To handle Count = Count + 1, old lhsWrapper cannot be deleted here
         // Note: for sat.X = sat.X + 1; LHS sat.X is ObjectPropertyWrapper but
         // sat.X is ParameterWrapper since only Parameters are allowed in the
         // equation - needs an improvment to use wrappers in the equation.
         if (lhsWrapper != toWrapper)
         {
            // Only replace if same wrapper type
            if (lhsWrapper == NULL ||
                (lhsWrapper != NULL && lhsWrapper->GetWrapperType() == toWrapper->GetWrapperType()))
            {
               lhsOldWrapper = lhsWrapper;
               lhsWrapper = toWrapper;
            }
         }
         retval = true;
      }
      else if (withName.find(".") != withName.npos)
      {
         // Some lhs Parameters are settable such as Sat.Thruster1.FuelMass, so check here
         Parameter *param = (Parameter*)toWrapper->EvaluateObject();
         if (param && param->IsSettable())
         {
            if (lhsWrapper != toWrapper)
            {
               // Only replace if same wrapper type
               if (lhsWrapper == NULL ||
                   (lhsWrapper != NULL && lhsWrapper->GetWrapperType() == toWrapper->GetWrapperType()))
               {
                  lhsOldWrapper = lhsWrapper;
                  lhsWrapper = toWrapper;
               }
            }
            retval = true;
         }
      }
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   ShowWrapper("   ", "lhsWrapper:", lhsWrapper);
   ShowWrapper("   ", "rhsWrapper:", rhsWrapper);
   #endif
   
   if (rhsEquation == NULL)
   {
      #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage("   Checking RHS and it is not a math tree...\n");
      #endif
      if (withName == rhs)
      {
         if (rhsWrapper != toWrapper)
         {
            rhsOldWrapper = rhsWrapper;
            #ifdef DEBUG_WRAPPER_CODE
               rhsNewWrapper = toWrapper;
            #endif
            rhsWrapper = toWrapper;
         }
         retval = true;
      }
   }
   else
   {
      #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage("   Checking RHS and it is a math tree...\n");
      std::map<std::string, ElementWrapper *>::iterator ewi;
      for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
         ShowWrapper("   ", ewi->first, ewi->second);
      #endif
      
      /* The code that needs to move to a more general purpose location. */
      rhsEquation->SetElementWrapper(toWrapper, withName);

      // If name found in the math wrapper map
      if (mathWrapperMap.find(withName) != mathWrapperMap.end())
      {
         #ifdef DEBUG_WRAPPER_CODE
         MessageInterface::ShowMessage("   name '%s' found in mathWrapperMap\n", withName.c_str());
         #endif
         // rhs should always be parameter wrapper, so check first
         // Now string wrapper is allowed such as strcat(a, ' new string') (LOJ: 2016.02.25)
         if ((withName.find(".") == withName.npos) ||
             (withName.find(".") != withName.npos &&
              (toWrapper->GetWrapperType() == Gmat::PARAMETER_WT ||
               toWrapper->GetWrapperType() == Gmat::NUMBER_WT)) ||
             (toWrapper->GetWrapperType() == Gmat::STRING_WT))
         {
            if (mathWrapperMap[withName] != toWrapper)
            {
               #ifdef DEBUG_WRAPPER_CODE
               MessageInterface::ShowMessage("   now setting rhsNewWrapper to <%p>\n", toWrapper);
               #endif
               rhsOldWrapper = mathWrapperMap[withName];
               #ifdef DEBUG_WRAPPER_CODE
                  rhsNewWrapper = toWrapper;
               #endif
               mathWrapperMap[withName] = toWrapper;
            }
            retval = true;
         }
      }
      /* To here */
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   ShowWrapper("   ", "lhsOldWrapper:", lhsOldWrapper);
   ShowWrapper("   ", "rhsOldWrapper:", rhsOldWrapper);
   ShowWrapper("   ", "   lhsWrapper:", lhsWrapper);
   ShowWrapper("   ", "   rhsWrapper:", rhsWrapper);
   ShowWrapper("   ", "rhsNewWrapper:", rhsNewWrapper);
   #endif
   
   // now delete old wrappers
   if (lhsOldWrapper == rhsOldWrapper && lhsOldWrapper != NULL)
   {
      // delete only lhs old wrapper
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (lhsOldWrapper, lhsOldWrapper->GetDescription(), "Assignment::SetElementWrapper()",
          //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting lhsOldWrapper");
          " deleting lhsOldWrapper");
      #endif
      delete lhsOldWrapper;
      lhsOldWrapper = NULL;
   }
   else
   {
      // check lhs wrapper
      if (lhsOldWrapper != NULL)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (lhsOldWrapper, lhsOldWrapper->GetDescription(), "Assignment::SetElementWrapper()",
             //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting lhsOldWrapper");
             " deleting lhsOldWrapper");
         #endif
         delete lhsOldWrapper;     
         lhsOldWrapper = NULL;
      }
      
      // check rhs wrapper
      if (rhsOldWrapper != NULL)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (rhsOldWrapper, rhsOldWrapper->GetDescription(), "Assignment::SetElementWrapper()",
             //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting rhsOldWrapper");
             " deleting rhsOldWrapper");
         #endif
         delete rhsOldWrapper;
         rhsOldWrapper = NULL;
      }
   }
   
   #ifdef DEBUG_WRAPPER_CODE
   MessageInterface::ShowMessage
      ("Assignment::SetElementWrapper() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
void Assignment::ClearWrappers()
{
   ElementWrapper* lhsEw = lhsWrapper;
   ElementWrapper* rhsEw = rhsWrapper;
   
   if (rhsEw)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (rhsEw, rhsEw->GetDescription(), "Assignment::ClearWrappers()",
          //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting rhs wrapper");
          " deleting rhs wrapper");
      #endif
      delete rhsEw;
      rhsEw = NULL;
   }
   
   // clear rhs math wrapper map
   if (rhsEquation)
   {
      // Check if thr RHS equation contains any LHS wrappers.
      // If so, release ownership here to avoid double deletion.
      WrapperMap rhsWrapperMap = rhsEquation->GetMathWrapperMap();
      if (lhsEw)
      {
         if (rhsWrapperMap.find(lhsEw->GetDescription()) !=
            rhsWrapperMap.end())
         {
            if (lhsEw == rhsWrapperMap.at(lhsEw->GetDescription()))
               lhsEw = NULL;
         }
      }
      rhsEquation->ClearWrappers();
   }
//   std::map<std::string, ElementWrapper *>::iterator ewi;
//   for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
//   {
//      if (ewi->second != NULL)
//      {
//         // if it is not the same as lhs wrapper, delete
//         if (ewi->second != lhsEw)
//         {
//            #ifdef DEBUG_MEMORY
//            MemoryTracker::Instance()->Remove
//               (ewi->second, (ewi->second)->GetDescription(),
//                "Assignment::ClearWrappers()",
//                //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting math node wrapper");
//                " deleting math node wrapper");
//            #endif
//            delete ewi->second;
//            ewi->second = NULL;
//         }
//      }
//   }
   
   if (lhsEw && lhsEw != rhsWrapper)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (lhsEw, lhsEw->GetDescription(), "Assignment::ClearWrappers()",
          //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting lhs wrapper");
          " deleting lhs wrapper");
      #endif
      delete lhsEw;
      lhsEw = NULL;
   }
   
   lhsWrapper = NULL;
   rhsWrapper = NULL;
   
   mathWrapperMap.clear();
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param <type> Type of the object that is renamed.
 * @param <oldName> The current name for the object.
 * @param <newName> The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Assignment::RenameRefObject(const UnsignedInt type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Assignment::RenameRefObject() entered <%s>\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   MessageInterface::ShowMessage
      ("   type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   // Go through lhs and rhs
   if (lhs.find(oldName) != lhs.npos)
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("   old name '%s' found in lhs: '%s', so replacing with '%s'\n",
          oldName.c_str(), lhs.c_str(), newName.c_str());
      #endif
      
      lhs = GmatStringUtil::ReplaceName(lhs, oldName, newName);
      
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("   after replacing, lhs = '%s'\n", lhs.c_str());
      #endif
   }
   
   if (rhs.find(oldName) != rhs.npos)
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("   old name '%s' found in rhs: '%s', so replacing with '%s'\n",
          oldName.c_str(), rhs.c_str(), newName.c_str());
      #endif
      
      rhs = GmatStringUtil::ReplaceName(rhs, oldName, newName);
      
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("   after replacing, rhs = '%s'\n", rhs.c_str());
      #endif
   }
   
   // Go through wrappers
   if (lhsWrapper)
      lhsWrapper->RenameObject(oldName, newName);
   
   if (rhsWrapper)
      rhsWrapper->RenameObject(oldName, newName);
   
   // Go through math tree
   if (rhsEquation)
      rhsEquation->RenameRefObject(type, oldName, newName);
   
   // Go through mathWrapperMap
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   Now checking %d wrappers in mathWrapperMap\n", mathWrapperMap.size());
   #endif
   std::map<std::string, ElementWrapper *>::iterator ewi;
   for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
   {
      std::string wrapperName = ewi->first;
      ElementWrapper *wrapper = ewi->second;
      if (wrapperName.find(oldName) != wrapperName.npos)
      {
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("   Found '%s' in the mathWrapperMap\n", wrapperName.c_str());
         #endif
         std::string oldWrapperName = wrapperName;
         wrapperName = GmatStringUtil::ReplaceName(wrapperName, oldName, newName);
         
         // If name changed then replace and remove old one
         if (oldWrapperName != wrapperName)
         {
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               ("   New name is '%s' and setting <%p> to wrapper\n", wrapperName.c_str(),
                wrapper);
            #endif
            
            mathWrapperMap[wrapperName] = wrapper;
            mathWrapperMap.erase(ewi);
         }
      }
   }
   
   // Update generatingString
   GetGeneratingString();
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Assignment::RenameRefObject() leaving <%s>\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Assignment.
 *
 * @return clone of the Assignment.
 */
//------------------------------------------------------------------------------
GmatBase* Assignment::Clone() const
{
   return (new Assignment(*this));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//                                            const Integer index) const
//------------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 * @param index The index when multiple StringArrays are supported.
 *
 * @return The requested StringArray.
 */
//------------------------------------------------------------------------------
const std::string& Assignment::GetGeneratingString(Gmat::WriteMode mode,
                                                   const std::string &prefix,
                                                   const std::string &useName)
{
   if (mode == Gmat::NO_COMMENTS)
   {
      generatingString = lhs + " = " + rhs + ";";
      return generatingString;
   }
   
   std::string gen;
   bool writeGmatKeyword = GmatGlobal::Instance()->IsWritingGmatKeyword();
   // We now write out GMAT prefix on option from the startup file (see GMT-3233)
   if (writeGmatKeyword)
      gen = prefix + "GMAT " + lhs + " = " + rhs + ";";
   else
      gen = prefix + lhs + " = " + rhs + ";";
   
   #ifdef DEBUG_ASSIGNMENT_SCRIPTING
   MessageInterface::ShowMessage("Assignment command generator is \"%s\"\n",
                                 gen.c_str());
   #endif
   
   generatingString = gen;
   
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// bool AffectsClones()
//------------------------------------------------------------------------------
/**
 * Identifies commands that update objects that could be cloned in other objects
 *
 * @return true, since Assignment may make updates that affect clones
 */
//------------------------------------------------------------------------------
bool Assignment::AffectsClones()
{
   if (lhsOwner != NULL)
   {
      if ( lhsOwner->IsOfType(Gmat::VARIABLE) ||
           lhsOwner->IsOfType(Gmat::STRING) ||

           // These 2 are current Windows issues
           lhsOwner->IsOfType(Gmat::SOLVER) ||
           lhsOwner->IsOfType(Gmat::ARRAY) )
         return false;
   }

   return true;
}


//------------------------------------------------------------------------------
// GmatBase* GetUpdatedObject()
//------------------------------------------------------------------------------
/**
 * Retrieves object that has updates so it can be passed to owned clones
 *
 * @return The object pointer to the LHS object is it might have an owned clone
 */
//------------------------------------------------------------------------------
GmatBase* Assignment::GetUpdatedObject()
{
   GmatBase *retval = NULL;

   if (lhsOwner != NULL)
   {
      // Add handlers for types known not to be affected by owned clones
      if ( !lhsOwner->IsOfType(Gmat::VARIABLE) &&
            !lhsOwner->IsOfType(Gmat::STRING) &&
            !lhsOwner->IsOfType(Gmat::ARRAY) )
         retval = lhsOwner;
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer     GetUpdatedObjectParameterIndex()
//------------------------------------------------------------------------------
/**
 * Returns updated parameter index for an updated object that may have clones
 *
 * Owned clone updates might change an entire object, or they might just change
 * a single object parameter.  If the change is to a single parameter, this
 * method returned the parameter index for the changed parameter so that that
 * field, and only that field, can be updated, minimizing disruptions to the
 * underlying class infrastructure.
 *
 * @return The parameter index, or -1 if the change is for the whole object.
 */
//------------------------------------------------------------------------------
Integer Assignment::GetUpdatedObjectParameterIndex()
{
   Integer parmIndex = (lhsOwnerID >= 0 ? lhsOwnerID : -1);

   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("Parameter index for assignment update: "
            "%d\n", parmIndex);
   #endif

   return parmIndex;
}


//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// bool ValidateArrayElement(ElementWrapper *lhsWrapper, ElementWrapper *rhsWrapper)
//------------------------------------------------------------------------------
bool Assignment::ValidateArrayElement(ElementWrapper *lhsWrapper,
                                      ElementWrapper *rhsWrapper)
{
   bool retval = true;
   
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage("   Both sides are array element\n");
   #endif
   ElementWrapper* lhsRowWrapper = ((ArrayElementWrapper*)(lhsWrapper))->GetRowWrapper();
   ElementWrapper* lhsColWrapper = ((ArrayElementWrapper*)(lhsWrapper))->GetColumnWrapper();
   ElementWrapper* rhsRowWrapper = ((ArrayElementWrapper*)(rhsWrapper))->GetRowWrapper();
   ElementWrapper* rhsColWrapper = ((ArrayElementWrapper*)(rhsWrapper))->GetColumnWrapper();
   #ifdef DEBUG_VALIDATION
   ShowWrapper("   ", "lhsRowWrapper:", lhsRowWrapper);
   ShowWrapper("   ", "lhsColWrapper:", lhsColWrapper);
   ShowWrapper("   ", "rhsRowWrapper:", rhsRowWrapper);
   ShowWrapper("   ", "rhscolWrapper:", rhsColWrapper);
   #endif
   
   // Check for valid array index if index is a number. We cannot validate other
   // wrapper types properly during parsing since those needs to be evaluated in
   // the mission sequence.   For example: "A(B(3,3), C)"
   if (lhsRowWrapper->GetWrapperType() == Gmat::NUMBER_WT &&
       lhsColWrapper->GetWrapperType() == Gmat::NUMBER_WT &&
       rhsRowWrapper->GetWrapperType() == Gmat::NUMBER_WT &&
       lhsColWrapper->GetWrapperType() == Gmat::NUMBER_WT)
   {
      Real lhsRow = lhsRowWrapper->EvaluateReal();
      Real lhsCol = lhsColWrapper->EvaluateReal();
      Real rhsRow = rhsRowWrapper->EvaluateReal();
      Real rhsCol = rhsColWrapper->EvaluateReal();
      Array *lhsArr = (Array*)(lhsWrapper->GetRefObject());
      Array *rhsArr = (Array*)(rhsWrapper->GetRefObject());
      
      if (lhsArr && rhsArr)
      {
         Integer lhsRowCount = lhsArr->GetRowCount();
         Integer lhsColCount = lhsArr->GetColCount();
         Integer rhsRowCount = rhsArr->GetRowCount();
         Integer rhsColCount = rhsArr->GetColCount();
         
         #ifdef DEBUG_VALIDATION
         MessageInterface::ShowMessage
            ("   lhsRowCount=%d, lhsColCount=%d, rhsRowCount=%d, rhsColCount=%d, "
             "lhsRow=%f, lhsCol=%f, rhsRow=%f, rhsCol=%f\n", lhsRowCount,
             lhsColCount, rhsRowCount, rhsColCount, lhsRow, lhsCol, rhsRow, rhsCol);
         #endif
         
         // Check lhs array index for out of bounds
         if (lhsRow > lhsRowCount || lhsCol > lhsColCount)
         {
            lastErrorMessage = "Array index of LHS is out-of-bounds. ";
            retval = false;
         }
         
         // Check rhs array index for out of bounds
         if (rhsRow > rhsRowCount || rhsCol > rhsColCount)
         {
            lastErrorMessage = lastErrorMessage + "Array index of RHS is out-of-bounds.";
            retval = false;
         }
      }
      else
      {
         lastErrorMessage = "Array of LHS or RHS is not set.";
         retval = false;
      }
   }
   return retval;
}


//------------------------------------------------------------------------------
// bool ValidateRmatrix(ElementWrapper *lhsWrapper, ElementWrapper *rhsWrapper)
//------------------------------------------------------------------------------
bool Assignment::ValidateRmatrix(ElementWrapper *lhsWrapper, ElementWrapper *rhsWrapper)
{
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage("   LHS is Rmatrix\n");
   #endif
   
   bool retval = false;
   Gmat::ParameterType lhsDataType = lhsWrapper->GetDataType();
   Gmat::ParameterType rhsDataType = rhsWrapper->GetDataType();
   
   if ((rhsDataType != Gmat::STRING_TYPE) &&
       (rhsDataType != Gmat::RMATRIX_TYPE))
   {
      if (rhsDataType == Gmat::REAL_TYPE)
      {
         retval = true;
         // Check if LHS is 1x1 array
         if (lhsWrapper->GetWrapperType() == Gmat::ARRAY_WT)
         {
            Array *lhsArr = (Array*)(lhsWrapper->GetRefObject());
            #ifdef DEBUG_VALIDATION
            MessageInterface::ShowMessage
               ("   Checking LHS for 1x1 array, lhsArr=<%p>\n", lhsArr);
            #endif
            if (lhsArr)
            {
               #ifdef DEBUG_VALIDATION
               MessageInterface::ShowMessage
                  ("   rowCount=%d, colCount=%d\n",
                   lhsArr->GetRowCount(), lhsArr->GetColCount());
               #endif
               //Should || be &&? (LOJ: 2014.02.11)
               //if (lhsArr->GetRowCount() != 1 || lhsArr->GetColCount() != 1)
               if (lhsArr->GetRowCount() != 1 && lhsArr->GetColCount() != 1)
               {
                  lastErrorMessage = "Left of the equal sign is not 1x1 Array.";
                  retval = false;
               }
            }
            else
            {
               lastErrorMessage = "Left of the equal sign is not valid.";
               retval = false;
            }
         }
      } // if (rhsDataType == Gmat::REAL_TYPE)
      else if (rhsDataType == Gmat::RVECTOR_TYPE) // LOJ: added 2014.02.11
      {
         retval = true;
         // Check if LHS is one column or row matrix
         if (lhsWrapper->GetWrapperType() == Gmat::ARRAY_WT)
         {
            Array *lhsArr = (Array*)(lhsWrapper->GetRefObject());
            #ifdef DEBUG_VALIDATION
            MessageInterface::ShowMessage
               ("   Checking LHS for Mx1 or 1xN array, lhsArr=<%p>\n", lhsArr);
            #endif
            if (lhsArr)
            {
               Integer rowCount = lhsArr->GetRowCount();
               Integer colCount = lhsArr->GetColCount();
               #ifdef DEBUG_VALIDATION
               MessageInterface::ShowMessage
                  ("   rowCount=%d, colCount=%d\n", rowCount, colCount);
               #endif
               if (rowCount == 1 || colCount == 1)
               {
                  // Check if RHS vector has the same size as LHS matrix
                  if (rhsWrapper->GetWrapperType() == Gmat::PARAMETER_WT)
                  {
                     Parameter *param = (Parameter*)(rhsWrapper->GetRefObject());
                     Integer vectorSize = param->GetIntegerParameter("VectorSize");
                     Integer arraySize = rowCount > colCount ? rowCount : colCount;
                     #ifdef DEBUG_VALIDATION
                     MessageInterface::ShowMessage
                        ("   vectorSize=%d, arraySize=%d\n", vectorSize, arraySize);
                     #endif
                     if (vectorSize != arraySize)
                     {
                        std::string sizeStr = GmatStringUtil::ToString(vectorSize, 1);
                        lastErrorMessage = "Left of the equal sign is not " + sizeStr + 
                           "x1 or 1x" + sizeStr + " Array.";
                        retval = false;
                     }
                  }
               }
               else
               {
                  lastErrorMessage = "Left of the equal sign is not Mx1 or 1xN Array.";
                  retval = false;
               }
            }
            else
            {
               lastErrorMessage = "Left of the equal sign is not valid.";
               retval = false;
            }
         }
      } // else if (rhsDataType == Gmat::RVECTOR_TYPE)
   }
   
   return retval;
} // ValidateRmatrix()


//------------------------------------------------------------------------------
// bool ValidateRvector(ElementWrapper *lhsWrapper, ElementWrapper *rhsWrapper)
//------------------------------------------------------------------------------
bool Assignment::ValidateRvector(ElementWrapper *lhsWrapper, ElementWrapper *rhsWrapper)
{
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage("   LHS is Rvector\n");
   #endif
   
   bool retval = false;
   Gmat::ParameterType lhsDataType = lhsWrapper->GetDataType();
   Gmat::ParameterType rhsDataType = rhsWrapper->GetDataType();
   
   if (rhsDataType == Gmat::STRING_TYPE)
   {
      // Check if RHS has vector string form such as [0 0 0 1]
      RealArray arr = GmatStringUtil::ToRealArray(rhs);
      Integer rhsSize = arr.size();
      #ifdef DEBUG_VALIDATION
      MessageInterface::ShowMessage("   RHS is a string: %s\n", rhs.c_str());
      MessageInterface::ShowMessage("   RHS has %d elements\n", rhsSize);
      #endif
      
      if (rhsSize > 0)
      {
         retval = true;
      }
      else
      {
         std::string sizeStr = GmatStringUtil::ToString(rhsSize, 1);
         lastErrorMessage = "Right of the equal sign is not " + sizeStr + 
            " element Vector.";
         retval = false;
      }
   }
   else
   {
      if (rhsDataType == Gmat::RMATRIX_TYPE) 
      {
         retval = true;
         // Check if RHS is one column or row matrix
         if (rhsWrapper->GetWrapperType() == Gmat::ARRAY_WT)
         {
            Array *rhsArr = (Array*)(rhsWrapper->GetRefObject());
            #ifdef DEBUG_VALIDATION
            MessageInterface::ShowMessage
               ("   Checking RHS for Mx1 or 1xN array, rhsArr=<%p>\n", rhsArr);
            #endif
            if (rhsArr)
            {
               Integer rowCount = rhsArr->GetRowCount();
               Integer colCount = rhsArr->GetColCount();
               #ifdef DEBUG_VALIDATION
               MessageInterface::ShowMessage
                  ("   rowCount=%d, colCount=%d\n", rowCount, colCount);
               #endif
               if (rowCount == 1 || colCount == 1)
               {
                  // Check if LHS vector has the same size as RHS matrix
                  if (lhsWrapper->GetWrapperType() == Gmat::PARAMETER_WT)
                  {
                     Parameter *param = (Parameter*)(lhsWrapper->GetRefObject());
                     Integer vectorSize = param->GetIntegerParameter("VectorSize");
                     Integer arraySize = rowCount > colCount ? rowCount : colCount;
                     #ifdef DEBUG_VALIDATION
                     MessageInterface::ShowMessage
                        ("   vectorSize=%d, arraySize=%d\n", vectorSize, arraySize);
                     #endif
                     if (vectorSize != arraySize)
                     {
                        std::string sizeStr = GmatStringUtil::ToString(vectorSize, 1);
                        lastErrorMessage = "Left of the equal sign is not " + sizeStr + 
                           "x1 or 1x" + sizeStr + " Array.";
                        retval = false;
                     }
                  }
               }
               else
               {
                  lastErrorMessage = "Left of the equal sign is not Mx1 or 1xN Array.";
                  retval = false;
               }
            }
            else
            {
               lastErrorMessage = "Left of the equal sign is not valid.";
               retval = false;
            }
         }
      } // else if (rhsDataType == Gmat::RVECTOR_TYPE)
   }
   
   return retval;
} // ValidateRvector()


//------------------------------------------------------------------------------
// void ClearMathTree()
//------------------------------------------------------------------------------
/**
 * Deletes math tree and clears wrappers.
 */
//------------------------------------------------------------------------------
void Assignment::ClearMathTree()
{
   if (rhsEquation)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (rhsEquation, rhsEquation->GetName(), "Assignment::~Assignment()", "deleting rhsEquation");
      #endif
      delete rhsEquation;
   }
   
   rhsEquation = NULL;
   ClearWrappers();
}


//------------------------------------------------------------------------------
// void HandleObjectPropertyChange(ElementWrapper *lhsWrapper)
//------------------------------------------------------------------------------
void Assignment::HandleObjectPropertyChange(ElementWrapper *lhsWrapper)
{
   #ifdef DEBUG_PROPERTY_CHANGE
   MessageInterface::ShowMessage
      ("\nAssignment::HandleObjectPropertyChange() entered, lhsWrapper='%s'\n",
       lhsWrapper->GetDescription().c_str());
   #endif
   
   std::string propName;
   GmatBase *obj = NULL;
   
   // Get property name
   if (lhsWrapper->GetWrapperType() == Gmat::OBJECT_PROPERTY_WT)
   {
      #ifdef DEBUG_PROPERTY_CHANGE
      MessageInterface::ShowMessage("   The wrapper type is OBJECT_PROPERTY_WT\n");
      #endif
      StringArray propNames = ((ObjectPropertyWrapper*)lhsWrapper)->GetPropertyNames();
      int propSize = propNames.size();
      
      #ifdef DEBUG_PROPERTY_CHANGE
      MessageInterface::ShowMessage("   There are %d properties\n", propSize);
      for (int i = 0; i < propSize; i++)
         MessageInterface::ShowMessage("      propNames[%d] = '%s'\n", i, propNames[i].c_str());
      #endif
      
      if (propSize == 0)
         throw  CommandException("Assignment command cannot handle object property change, "
                                 "property name is empty");
      
      // Get first property name (Why is there more than one property name?)
      propName = propNames[0];
      obj = lhsWrapper->GetRefObject();
   }
   else if (lhsWrapper->GetWrapperType() == Gmat::OBJECT_WT)
   {
      #ifdef DEBUG_PROPERTY_CHANGE
      MessageInterface::ShowMessage("   The wrapper type is OBJECT_WT\n");
      #endif
      obj = lhsWrapper->GetRefObject();
   }
   else if (lhsWrapper->GetWrapperType() == Gmat::PARAMETER_WT)
   {
      #ifdef DEBUG_PROPERTY_CHANGE
      MessageInterface::ShowMessage("   The wrapper type is PARAMETER_WT\n");
      #endif
      Parameter *param = (Parameter*)lhsWrapper->GetRefObject();
      obj = param->GetOwner();
   }
   
   if (obj != NULL)
   {
      // Check for the Spacecraft first
      if (obj->IsOfType(Gmat::SPACECRAFT))
      {
         if (propName == "OrbitColor")
         {
            #ifdef DEBUG_PROPERTY_CHANGE
            MessageInterface::ShowMessage("   Calling Publisher to handle orbit color change\n");
            #endif
            publisher->SetOrbitColorChanged(this, obj->GetStringParameter("OrbitColor"),
                                            obj->GetName(), lhs + " = " + rhs);
         }
         else if (propName == "TargetColor")
         {
            #ifdef DEBUG_PROPERTY_CHANGE
            MessageInterface::ShowMessage("   Calling Publisher to handle target color change\n");
            #endif
            publisher->SetTargetColorChanged(this, obj->GetStringParameter("TargetColor"),
                                             obj->GetName(), lhs + " = " + rhs);
         }
         else
         {
            #ifdef DEBUG_PROPERTY_CHANGE
            MessageInterface::ShowMessage("   Calling Publisher to handle spacecraft property change\n");
            #endif
            publisher->SetSpacecraftPropertyChanged(this, obj->GetRealParameter("A1Epoch"),
                                                    obj->GetName(), lhs + " = " + rhs);
         }
      }
      else if (propName == "OrbitColor")
      {
         #ifdef DEBUG_PROPERTY_CHANGE
         MessageInterface::ShowMessage("   Calling Publisher to handle orbit color change\n");
         #endif
         publisher->SetOrbitColorChanged(this, obj->GetStringParameter("OrbitColor"),
                                         obj->GetName(), lhs + " = " + rhs);
      }
      else if (propName == "TargetColor")
      {
         #ifdef DEBUG_PROPERTY_CHANGE
         MessageInterface::ShowMessage("   Calling Publisher to handle target color change\n");
         #endif
         publisher->SetTargetColorChanged(this, obj->GetStringParameter("TargetColor"),
                                          obj->GetName(), lhs + " = " + rhs);
      }
   }
   #ifdef DEBUG_PROPERTY_CHANGE
   MessageInterface::ShowMessage
      ("Assignment::HandleObjectPropertyChange() leaving, lhsWrapper='%s'\n",
       lhsWrapper->GetDescription().c_str());
   #endif
}


//------------------------------------------------------------------------------
// void PassToClones()
//------------------------------------------------------------------------------
/**
 * Passes assignment data to cloned objects
 *
 * This method walks the mission control sequence, locating commands that use
 * clones and setting the data to the clones as needed
 */
//------------------------------------------------------------------------------
void Assignment::PassToClones()
{
   if (lhsOwner == NULL)
      return;

   GmatCommand *current = GetNext();

   while ((current != NULL) && (current != this))
   {
      #ifdef DEBUG_CLONE_UPDATES
         MessageInterface::ShowMessage("%s: %d clones\n",
               current->GetTypeName().c_str(), current->GetCloneCount());
      #endif
      for (Integer i = 0; i < current->GetCloneCount(); ++i)
      {
         GmatBase *theClone = current->GetClone(i);
         #ifdef DEBUG_CLONE_UPDATES
            MessageInterface::ShowMessage("Clone %d: %s <%p>\n", i,
                  (theClone == NULL ? "is NULL" :
                   theClone->GetName().c_str()), theClone);
         #endif
         if (theClone == NULL)
            continue;

         if (theClone->GetName() == lhsOwner->GetName())
         {
            if (lhsOwnerID < 0)
            {
               #ifdef DEBUG_CLONE_UPDATES
                  MessageInterface::ShowMessage("Performing whole object "
                        "update for %s\n", theClone->GetName().c_str());
               #endif
               (*theClone) = (*lhsOwner);
            }
            else
            {
               #ifdef DEBUG_CLONE_UPDATES
                  MessageInterface::ShowMessage("Matching attribute %d <%s> "
                        "on the clone named %s\n", lhsOwnerID,
                        theClone->GetParameterText(lhsOwnerID).c_str(),
                        theClone->GetName().c_str());
               #endif
               MatchAttribute(lhsOwnerID, lhsOwner, theClone);
            }
         }
         #ifdef DEBUG_CLONE_UPDATES
            else
            {
               MessageInterface::ShowMessage("Clone \"%s\" has a different "
                     "name from owner \"%s\"\n", theClone->GetName().c_str(),
                     lhsOwner->GetName().c_str());
            }
         #endif
      }

      // Prevent infinite looping!
      if (current == current->GetNext())
         break;

      current = current->GetNext();
      #ifdef DEBUG_CLONE_UPDATES
         MessageInterface::ShowMessage("current: %p this: %p\n", current, this);
      #endif
   }
}


//------------------------------------------------------------------------------
// void MatchAttribute(Integer id, GmatBase *owner, GmatBase *receiver)
//------------------------------------------------------------------------------
/**
 * Passes assignment data to a specific (usually cloned) object parameter
 *
 * @param id The parameter ID for the parameter
 * @param owner The object that has the parameter setting already applied
 * @param receiver The object that is receiving the new parameter value
 */
//------------------------------------------------------------------------------
void Assignment::MatchAttribute(Integer id, GmatBase *owner, GmatBase *receiver)
{
   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("   MatchAttribute(%d, %s, %s) called\n",
            id, owner->GetName().c_str(), receiver->GetName().c_str());
   #endif

   Gmat::ParameterType theType = owner->GetParameterType(id);

   switch (theType)
   {
      case Gmat::REAL_TYPE:
         receiver->SetRealParameter(id, owner->GetRealParameter(id));
         break;

      case Gmat::INTEGER_TYPE:
         receiver->SetIntegerParameter(id, owner->GetIntegerParameter(id));
         break;

      case Gmat::BOOLEAN_TYPE:
         receiver->SetBooleanParameter(id, owner->GetBooleanParameter(id));
         break;

      case Gmat::ON_OFF_TYPE:
         receiver->SetOnOffParameter(id, owner->GetOnOffParameter(id));
         break;

      case Gmat::STRING_TYPE:
      case Gmat::ENUMERATION_TYPE:
         receiver->SetStringParameter(id, owner->GetStringParameter(id));
         break;

      case Gmat::UNSIGNED_INT_TYPE:
         receiver->SetUnsignedIntParameter(id,
               owner->GetUnsignedIntParameter(id));
         break;

      case Gmat::RVECTOR_TYPE:
         receiver->SetRvectorParameter(id, owner->GetRvectorParameter(id));
         break;

      case Gmat::RMATRIX_TYPE:
         receiver->SetRmatrixParameter(id, owner->GetRmatrixParameter(id));
         break;


      // Types not yet covered
      case Gmat::UNSIGNED_INTARRAY_TYPE:
      case Gmat::INTARRAY_TYPE:
      case Gmat::STRINGARRAY_TYPE:
      case Gmat::TIME_TYPE:
      case Gmat::OBJECTARRAY_TYPE:
      case Gmat::FILENAME_TYPE:
      default:
         break;
   }
}


//------------------------------------------------------------------------------
// bool ParseRHS(const std::string &rhs)
//------------------------------------------------------------------------------
/**
 * Parser for the right side of the expression
 *
 * @param rhs The right hand side string
 *
 * @return true if the RHS parsed
 */
//------------------------------------------------------------------------------
bool Assignment::ParseRHS(std::string &rhs)
{
   bool retval = true;

   bool isRhsString = false;

   // check for single quotes in rhs and remove before process further (LOJ: 2009.10.09)
   // so that mp.IsEquation() will not think as transpose
   if (rhs.find("{") != rhs.npos || rhs.find("'") != rhs.npos)
   {
      // Single quote is allowed if it is paired, such as {'FuelTank1'}
      if (GmatStringUtil::StartsWith(rhs, "{") && GmatStringUtil::EndsWith(rhs, "}"))
      {
         #ifdef DEBUG_ASSIGNMENT_IA
         MessageInterface::ShowMessage
            ("   Removing single quote from <%s>\n", rhs.c_str());
         #endif
         rhs = GmatStringUtil::RemoveAll(rhs, '\'');

         #ifdef DEBUG_ASSIGNMENT_IA
         MessageInterface::ShowMessage
            ("   After removing <%s>, Setting isRhsString to true\n", rhs.c_str());
         #endif
         isRhsString = true;
      }
     else if (GmatStringUtil::IsEnclosedWith(rhs, "'"))
        isRhsString = true;
   }
   else
   {
      // Check for common use of ./ (path) in GmatFunction to avoid creating
      // MathTree for divide
      if (rhs.find("./") != rhs.npos ||
          GmatStringUtil::StartsWith(rhs, "'") || GmatStringUtil::EndsWith(rhs, "'"))
      {
         isRhsString = true;
      }
   }

   // If there is still ; then report error since ; should have been removed
   if (!isRhsString && (rhs.find(";") != rhs.npos))
   {
      // Allow ; in the vector form [1; 2; 3]
      #ifdef DEBUG_ASSIGNMENT_IA
      MessageInterface::ShowMessage
         ("   Checking if semicolon is inside [] in <%s>\n", rhs.c_str());
      #endif
      // Check for all occurrences of semicolon
      std::string::size_type openPos = rhs.find("[");
      std::string::size_type closePos = rhs.find("]");
      bool isSemicolonOutside = false;
      if (openPos != rhs.npos && closePos != rhs.npos)
      {
         unsigned int len = rhs.length();
         for (unsigned int i = 0; i < len; i++)
         {
            if (rhs[i] == ';')
            {
               #ifdef DEBUG_ASSIGNMENT_IA
               MessageInterface::ShowMessage("   ; found at %u\n", i);
               #endif
               if (i < openPos && i > closePos)
               {
                  isSemicolonOutside = true;
                  break;
               }
            }
         }
      }

      if (isSemicolonOutside)
         throw CommandException("Is there a missing \"%\" for inline comment?");
   }

   #ifdef DEBUG_ASSIGNMENT_IA
   MessageInterface::ShowMessage("RHS %s a string\n", isRhsString ? "is" : "is not");
   #endif
   // Check if rhs is an equation
   if (!isRhsString)
   {
      #ifdef DEBUG_ASSIGNMENT_IA
      MessageInterface::ShowMessage
         ("Creating MathParser with configObjectMap = <%p>\nCalling MathParser::IsEquation()\n",
          configObjectMap);
      #endif

      RHSEquation *rhsParser = new RHSEquation;
      if (rhsParser->BuildExpression(rhs, configObjectMap))
         rhsEquation = rhsParser;
      else // if not an equation, check for unexpected commas on the right-hand-side
      {
         #ifdef DEBUG_ASSIGNMENT_IA
         MessageInterface::ShowMessage
            ("Assignment::InterpretAction() %s is not an equation\n", rhs.c_str());
         #endif

         // Delete old MathTree and clear mathWrapperMap
         ClearMathTree();
         delete rhsParser;

         // Check if all brackets are balanced if not enclosed with single quotes
         if (!GmatStringUtil::IsEnclosedWith(rhs, "'"))
         {
            if (!GmatStringUtil::AreAllBracketsBalanced(rhs, "[({])}"))
               throw CommandException
                  ("Parentheses or braces are unbalanced on the right-hand-side of "
                   "an assignment command");
         }
      }
   }

   return retval;
}
