//------------------------------------------------------------------------------
//                           RealRHS
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: May 31, 2018
/**
 * Equation parser for Real number data
 */
//------------------------------------------------------------------------------

#include "RHSEquation.hpp"

#include "MathParser.hpp"
#include "MathTree.hpp"
#include "StringUtil.hpp"
#include "CommandException.hpp"

// Wrappers and wrapped types
#include "ElementWrapper.hpp"
#include "NumberWrapper.hpp"
#include "ArrayWrapper.hpp"
#include "ObjectWrapper.hpp"
#include "StringWrapper.hpp"
#include "Array.hpp"
#include "FunctionManager.hpp"

//#define DEBUG_RUN_MATH_TREE
//#define DEBUG_WRAPPER_CODE

#ifdef DEBUG_RUN_MATH_TREE
#include "MessageInterface.hpp"
#endif

//------------------------------------------------------------------------------
// RealRHS()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
RHSEquation::RHSEquation() :
   rhsString      (""),
   resourceMap    (NULL),
   rhsWrapper     (NULL),
   mathTree       (NULL)
{
}

//------------------------------------------------------------------------------
// RealRHS()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RHSEquation::~RHSEquation()
{
   if (mathTree != NULL)
      delete mathTree;
}

//------------------------------------------------------------------------------
// RealRHS(const RealRHS &rhs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rhs The RHC copied here.  Note that the tree is not rebuilt.
 */
//------------------------------------------------------------------------------
RHSEquation::RHSEquation(const RHSEquation &rhs) :
         rhsString      (rhs.rhsString),
         resourceMap    (rhs.resourceMap),
         rhsWrapper     (NULL),
         mathTree       (NULL)
{
   // Initialize the wrapper map
   if (rhs.mathWrapperMap.size() == 0)
   {
      ElementWrapper *wrapper = NULL;
      for (WrapperMap::const_iterator it = rhs.mathWrapperMap.begin();
            it != rhs.mathWrapperMap.end(); ++it)
      {
         wrapper = it->second->Clone();
         mathWrapperMap[it->first] = wrapper;
      }
   }
}

//------------------------------------------------------------------------------
// RealRHS& operator=(const RealRHS &rhs)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param The RHC copied here.  Note that the tree is not rebuilt.
 *
 * @return this RealRHS, set with the rhs string.
 */
//------------------------------------------------------------------------------
RHSEquation& RHSEquation::operator=(const RHSEquation &rhs)
{
   if (this != &rhs)
   {
      rhsString = rhs.rhsString;
      resourceMap = rhs.resourceMap;

      if (rhsWrapper != NULL)
         delete rhsWrapper;
      rhsWrapper = NULL;

      if (mathTree != NULL)
         delete mathTree;
      mathTree = NULL;

      // Leak to fix here
      mathWrapperMap.clear();

      // Initialize the wrapper map
      if (rhs.mathWrapperMap.size() == 0)
      {
         ElementWrapper *wrapper = NULL;
         for (WrapperMap::const_iterator it = rhs.mathWrapperMap.begin();
               it != rhs.mathWrapperMap.end(); ++it)
         {
            wrapper = it->second->Clone();
            mathWrapperMap[it->first] = wrapper;
         }
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// std::string GetEquationString()
//------------------------------------------------------------------------------
/**
 * Returns the string that built the MathTree
 *
 * @return The equation string
 */
//------------------------------------------------------------------------------
std::string RHSEquation::GetEquationString()
{
   return rhsString;
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Sets up a (Real) RHS if the rhsString has been set
 *
 * @return true if there is a configured MathTree, false if not
 */
//------------------------------------------------------------------------------
bool RHSEquation::Initialize(ObjectMap *objectMap, ObjectMap *globalObjectMap)
{
   bool retval = true;
   if ((rhsString != "") && (resourceMap != NULL))
      retval = BuildExpression(rhsString, resourceMap);

   #ifdef DEBUG_RUN_MATH_TREE
      MessageInterface::ShowMessage("Initializing MathTree at address %p\n",
            mathTree);
   #endif

   // Set references for the math element wrappers
   std::map<std::string, ElementWrapper*>::iterator ewi;
   for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
      SetWrapperReferences(*(ewi->second), objectMap, globalObjectMap);

   if (mathTree && retval)
   {
      retval = mathTree->Initialize(objectMap, globalObjectMap);
      mathTree->SetMathWrappers(&mathWrapperMap);

      if (retval)
      {
         try
         {
            #ifdef DEBUG_RUN_MATH_TREE
               MessageInterface::ShowMessage("   Validating\n");
            #endif

            MathNode *topNode = mathTree->GetTopNode();
            if (!topNode->ValidateInputs())
               throw CommandException("Failed to validate math equation or "
                     "function inputs");
         }
         catch (BaseException &be)
         {
            throw CommandException("Failed to validate math equation or "
                  "function inputs: " + be.GetFullMessage());
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetWrapperReferences(ElementWrapper *ew, ObjectMap *objectMap,
//       ObjectMap *globalObjectMap)
//------------------------------------------------------------------------------
/**
 * Updates wrappers with resource pointers to objects used in current run
 *
 * @param ew The wrapper that is updated
 * @param objectMap The local object map of resources
 * @param globalObjectMap The global object map of resources
 *
 * @return true
 */
//------------------------------------------------------------------------------
void RHSEquation::SetWrapperReferences(ElementWrapper &ew, ObjectMap *objectMap,
      ObjectMap *globalObjectMap)
{
   StringArray refs = ew.GetRefObjectNames();
   for (UnsignedInt i = 0; i < refs.size(); ++i)
   {
      GmatBase *obj = (*objectMap)[refs[i]];
      if (obj == nullptr)
      {
         obj = (*globalObjectMap)[refs[i]];
         // Set the object pointer in the map for later use
         (*objectMap)[refs[i]] = obj;
      }
      if (obj)
         ew.SetRefObject(obj);
   }
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper,
//                        const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Sets the wrapper objects used in the equation
 *
 * @param toWrapper The element wrapper that the equation needs
 * @param withName The name of the wrapper, so it can be matched to the needed
 *                 member of the wrapper map.
 *
 * @return true if the wrapper was set
 */
//------------------------------------------------------------------------------
bool RHSEquation::SetElementWrapper(ElementWrapper *toWrapper,
                                   const std::string &withName)
{
   #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage("Setting RHS equation element wrapper %s\n",
            withName.c_str());
   #endif

   bool retval = false;

   // If name found in the math wrapper map
   if (mathWrapperMap.find(withName) != mathWrapperMap.end())
   {
      #ifdef DEBUG_WRAPPER_CODE
         MessageInterface::ShowMessage("   name '%s' found in mathWrapperMap\n",
               withName.c_str());
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
               MessageInterface::ShowMessage("   now setting equation wrapper "
                     "%s to <%p> on RHS %p\n", withName.c_str(), toWrapper,
                     this);
            #endif
            mathWrapperMap[withName] = toWrapper;
         }
         retval = true;
      }
   }
   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
/**
 * Clears the wrappers used in the equation
 */
//------------------------------------------------------------------------------
void RHSEquation::ClearWrappers()
{
   // clear rhs math wrapper map
   std::map<std::string, ElementWrapper *>::iterator ewi;
   for (ewi = mathWrapperMap.begin(); ewi != mathWrapperMap.end(); ++ewi)
   {
      if (ewi->second != NULL)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (ewi->second, (ewi->second)->GetDescription(),
             "Assignment::ClearWrappers()",
             //GetGeneratingString(Gmat::NO_COMMENTS) + " deleting math node wrapper");
             " deleting math node wrapper");
         #endif
         delete ewi->second;
         ewi->second = NULL;
      }
   }

   mathWrapperMap.clear();
}


//------------------------------------------------------------------------------
// void SetCallingFunction(FunctionManager *fm)
//------------------------------------------------------------------------------
/**
 * Passes Function objects to the MathTree
 *
 * @param fm The manager for the Function
 */
//------------------------------------------------------------------------------
void RHSEquation::SetCallingFunction(FunctionManager *fm)
{
   if (mathTree)
      mathTree->SetCallingFunction(fm);
}


//------------------------------------------------------------------------------
// void Assignment::SetMathWrappers()
//------------------------------------------------------------------------------
/**
 * Passes the wrappers to the math tree for use
 */
//------------------------------------------------------------------------------
void RHSEquation::SetMathWrappers()
{
   if (mathTree)
      mathTree->SetMathWrappers(&mathWrapperMap);
}


//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/**
 * Tells the MathTree that we are done using it
 */
//------------------------------------------------------------------------------
void RHSEquation::Finalize()
{
   if (mathTree)
      mathTree->Finalize();
}


//------------------------------------------------------------------------------
// bool BuildExpression(const std::string &rhside, ObjectMap *theMap)
//------------------------------------------------------------------------------
/**
 * Code that breaks the string into a MathTree.
 *
 * This method builds a MathTree from an input string.  The string is first
 * tested to see if it defines an equation.  If so, the MathTree construction
 * is attempted.
 *
 * @param rhside  The string describing the equation
 * @param theMap  An ObjectArray containing the objects that can be used in
 *                evaluating the tree.
 * @param allowSingleElement Flag to identify a single element as an equation if
 *                the parsing is clean
 *
 * @return true if a MathTree was built, false if not (and no exception was
 *         thrown).
 */
//------------------------------------------------------------------------------
bool RHSEquation::BuildExpression(const std::string &rhside, ObjectMap *theMap,
      bool allowSingleElement)
{
   bool retval = false;

   if (/*(rhside == "") && */ (mathTree != NULL))
      retval = true;
   else
   {
      MathParser mp(theMap);
      if (mp.IsEquation(rhside, false, allowSingleElement))
      {
         rhsString = rhside;

         // Parse RHS if equation
         #ifdef DEBUG_ASSIGNMENT_IA
         MessageInterface::ShowMessage
            ("Assignment::InterpretAction() %s is an equation\n", rhs.c_str());
         #endif

         MathNode *topNode = mp.Parse(rhsString, true);

         #ifdef DEBUG_ASSIGNMENT_IA
         if (topNode)
            MessageInterface::ShowMessage
               ("   topNode=%s\n", topNode->GetTypeName().c_str());
         #endif

         // Check if string has missing start quote.
         // It will be an error if rhs with blank space removed matches with
         // any GmatFunction name without letter case
         std::string str1 = rhsString;
         if (GmatStringUtil::EndsWith(str1, "'"))
         {
            #ifdef DEBUG_ASSIGNMENT_IA
            MessageInterface::ShowMessage("   <%s> ends with '\n", str1.c_str());
            #endif

            str1 = GmatStringUtil::RemoveLastString(str1, "'");
            str1 = GmatStringUtil::RemoveAll(str1, ' ');
            StringArray gmatFnNames = mp.GetGmatFunctionNames();
            bool isError = false;
            for (UnsignedInt i=0; i<gmatFnNames.size(); i++)
            {
               if (GmatStringUtil::ToUpper(str1) == GmatStringUtil::ToUpper(gmatFnNames[i]))
               {
                  isError = true;
                  break;
               }
            }
            if (isError)
               throw CommandException("Found missing start quote on the right-"
                     "hand side of an Assignment command or equation");
         }

         mathTree = new MathTree("MathTree", rhsString);
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (mathTree, rhs, "Assignment::InterpretAction()", "mathTree = new MathTree()");
         #endif

         mathTree->SetTopNode(topNode);
         mathTree->SetGmatFunctionNames(mp.GetGmatFunctionNames());
         retval = true;
      }
//      else
//         throw CommandException("The expression \"" + rhside + "\" is not a "
//               "valid equation.");
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool Validate(std::string &msg)
//------------------------------------------------------------------------------
/**
 * Validator for the RHS expression
 *
 * @param msg String used for error messages
 *
 * @return true if validated, false if not
 */
//------------------------------------------------------------------------------
bool RHSEquation::Validate(std::string &msg)
{
   bool retval = false;

   if (mathTree)
      retval = mathTree->Validate(msg);
   else
      msg = "The MathTree for the right hand side has not been set";

   return retval;
}


//------------------------------------------------------------------------------
// bool ValidateStringExpression(std::string &msg)
//------------------------------------------------------------------------------
/**
 * Validator for RHS string expressions
 *
 * @param msg String used for error messages
 *
 * @return true if validated, false if not
 */
//------------------------------------------------------------------------------
bool RHSEquation::ValidateStringExpression(std::string &msg)
{
   bool retval = false;

   // Now allow string function such as strcat() (LOJ: 2016.02.18)
   MathNode *topNode = mathTree->GetTopNode();
   if (topNode && topNode->IsOfType("StringFunctionNode"))
   {
      #ifdef DEBUG_VALIDATION
      MessageInterface::ShowMessage
         ("   We now allow string function, so it is valid\n");
      #endif
      retval = true;
   }
   else
   {
      msg = "Right of the equal sign is not a valid string.";
   }

   return retval;
}

//------------------------------------------------------------------------------
// WrapperMap GetMathWrapperMap()
//------------------------------------------------------------------------------
/**
 * Returns the math wrapper map used for RHS math elements
 *
 * @return The math wrapper map
 */
 //------------------------------------------------------------------------------
WrapperMap RHSEquation::GetMathWrapperMap()
{
   return mathWrapperMap;
}

//------------------------------------------------------------------------------
// MathTree *RealRHS::GetMathTree(bool releaseOwnership)
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the MathTree (so the Assignment command can access it).
 *
 * @param releaseOwnership Flag indicating if the calling function acquires
 *                         ownership of the tree.  If so, this object no longer
 *                         holds the MathTree.  (Default is true, relinquishing
 *                         the pointer.)
 *
 * @return The MathTree
 */
//------------------------------------------------------------------------------
MathTree* RHSEquation::GetMathTree(bool releaseOwnership)
{
   MathTree *retval = mathTree;

   if (releaseOwnership)
      mathTree = NULL;

   return retval;
}


//------------------------------------------------------------------------------
// ElementWrapper* RunMathTree()
//------------------------------------------------------------------------------
/**
 * Evaluates a RHS math tree, returning a wrapper containing the resultant.
 *
 * @return The MathTree evaluation, in an ElementWrapper.
 */
//------------------------------------------------------------------------------
ElementWrapper* RHSEquation::RunMathTree(ElementWrapper *lhsWrapper)
{
   #ifdef DEBUG_RUN_MATH_TREE
   MessageInterface::ShowMessage
      ("Assignment::RunMathTree() entered, mathTree=<%p>\n", mathTree);
   #endif

   if (mathTree == NULL)
      return NULL;

   Gmat::ParameterType lhsDataType;
   if (lhsWrapper != NULL)
      lhsDataType = lhsWrapper->GetDataType();
   else
      lhsDataType = Gmat::UNKNOWN_PARAMETER_TYPE;

   ElementWrapper *outWrapper = NULL;

   // Evaluate math tree
   Integer returnType;
   Integer numRow;
   Integer numCol;

   MathNode *topNode = mathTree->GetTopNode();

   if (topNode)
   {
      #ifdef DEBUG_RUN_MATH_TREE
      MessageInterface::ShowMessage
         ("Assignment::RunMathTree() topNode=%s, %s\n",
          topNode->GetTypeName().c_str(), topNode->GetName().c_str());
      #endif

      topNode->GetOutputInfo(returnType, numRow, numCol);

      // Equations embedded in other scripting may not have LHS wrappers
      if (lhsWrapper == NULL)
      {
         lhsDataType = (Gmat::ParameterType)returnType;
      }

      #ifdef DEBUG_RUN_MATH_TREE
      MessageInterface::ShowMessage
         ("   lhsDataType=%d, returnType=%d, numRow=%d, numCol=%d\n",
          lhsDataType, returnType, numRow, numCol);
      #endif

      if (lhsDataType != returnType)
      {
         bool isOk = false;
         if (lhsDataType == Gmat::REAL_TYPE &&
             returnType == Gmat::RMATRIX_TYPE && numRow == 1 && numCol == 1)
         {
            // It's ok to assign 1x1 matrix to scalar
            isOk = true;
         }
         else if (lhsDataType == Gmat::RMATRIX_TYPE)
         {
            Array *array = (Array*)lhsWrapper->GetRefObject();

            // It's ok to assign scalar to 1x1 matrix
            if (array && array->GetRowCount() == 1 && array->GetColCount() == 1)
               isOk = true;
         }

         if (!isOk)
         {
            std::string lhsTypeStr = GmatBase::PARAM_TYPE_STRING[lhsDataType];
            CommandException ce;
            ce.SetDetails("Cannot set type \"%s\" to type \"%s\"",
                          GmatBase::PARAM_TYPE_STRING[returnType].c_str(),
                          lhsTypeStr.c_str());
            throw ce;
         }
      }

      // @note Need to set description before setting value to output wrapper

      switch (returnType)
      {
         case Gmat::REAL_TYPE:
         {
            #ifdef DEBUG_RUN_MATH_TREE
            MessageInterface::ShowMessage("   Calling topNode->Evaluate()\n");
            #endif

            Real rval = -9999.9999;
            rval = topNode->Evaluate();

            #ifdef DEBUG_RUN_MATH_TREE
            MessageInterface::ShowMessage("   Returned %f (%s)\n",
                              rval, GmatStringUtil::ToString(rval).c_str());
            MessageInterface::ShowMessage("   Returned %f\n", rval);
            MessageInterface::ShowMessage("   Creating NumberWrapper for output\n");
            #endif

            outWrapper = new NumberWrapper();
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (outWrapper, GmatStringUtil::ToString(rval), "Assignment::RunMathTree()",
                "outWrapper = new NumberWrapper()");
            #endif

            outWrapper->SetDescription(GmatStringUtil::ToString(rval));
            outWrapper->SetReal(rval);
            break;
         }
         case Gmat::RMATRIX_TYPE:
         {
            #ifdef DEBUG_RUN_MATH_TREE
            MessageInterface::ShowMessage
               ("   MathTree return type is Rmatrix so Calling topNode->MatrixEvaluate()\n");
            #endif

            Rmatrix rmat;
            rmat.SetSize(numRow, numCol);
            rmat = topNode->MatrixEvaluate();
            #ifdef DEBUG_RUN_MATH_TREE
            MessageInterface::ShowMessage
               ("   rmat.rows=%d, rmat.cols=%d\n", rmat.GetNumRows(), rmat.GetNumColumns());
            #endif
            // create Array, this array will be deleted when ArrayWrapper is deleted
            Array *outArray = new Array("ArrayOutput");
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (outArray, "outArray", "Assignment::RunMathTree()",
                "outArray = new Array()");
            #endif
            outArray->SetSize(numRow, numCol);
            outArray->SetRmatrix(rmat);

            #ifdef DEBUG_RUN_MATH_TREE
            MessageInterface::ShowMessage
               ("   Creating ArrayWrapper for output, topNodeType='%s', topNodeName='%s'\n",
                topNode->GetTypeName().c_str(), topNode->GetName().c_str());
            #endif

            outWrapper = new ArrayWrapper();

            // Allow setting one dimential row or column vector to each other
            // for Cross() function (LOJ: 2016.03.14)
            if (topNode->GetTypeName() == "Cross3")
               outWrapper->AllowOneDimArraySetting(true);

            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (outWrapper, "outWrapper", "Assignment::RunMathTree()",
                "outWrapper = new ArrayWrapper()");
            #endif
            outWrapper->SetDescription("ArrayOutput");
            outWrapper->SetRefObject(outArray);
            break;
         }
         case Gmat::OBJECT_TYPE:
         {
            #ifdef DEBUG_RUN_MATH_TREE
            MessageInterface::ShowMessage("   Return type is OBJECT_TYPE\n");
            MessageInterface::ShowMessage("   Calling topNode->MatrixEvaluate()\n");
            #endif

            GmatBase *outObj = topNode->EvaluateObject();

            if (outObj)
            {
               #ifdef DEBUG_RUN_MATH_TREE
               MessageInterface::ShowMessage("   Creating ObjectWrapper for output\n");
               #endif
               outWrapper = new ObjectWrapper();
               outWrapper->SetDescription(outObj->GetName());
               outWrapper->SetRefObject(outObj->Clone());
            }
            else
            {
               CommandException ce;
               ce.SetDetails("Cannot set \"%s\" to the return object. The "
                     "return type of the equation is an OBJECT and it is NULL",
                             rhsString.c_str());
               throw ce;
            }
            break;
         }
         case Gmat::STRING_TYPE:
         {
            #ifdef DEBUG_RUN_MATH_TREE
            MessageInterface::ShowMessage("   Calling topNode->EvaluateString()\n");
            #endif

            std::string strval = topNode->EvaluateString();

            #ifdef DEBUG_RUN_MATH_TREE
            MessageInterface::ShowMessage("   Returned '%s'\n", strval.c_str());
            MessageInterface::ShowMessage("   Creating StringWrapper for output\n");
            #endif

            outWrapper = new StringWrapper();
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Add
               (outWrapper, GmatStringUtil::ToString(rval), "Assignment::RunMathTree()",
                "outWrapper = new NumberWrapper()");
            #endif

            outWrapper->SetDescription(strval);
            outWrapper->SetString(strval);
            break;
         }
         default:
         {
            CommandException ce;
            ce.SetDetails("Cannot set \"%s\" to to the return object. The "
                  "return type of the equation or function is unknown",
                  rhsString.c_str());
            throw ce;
         }
      }
   }
   else
   {
      throw CommandException("RHS is an equation, but top node is NULL\n");
   }

   #ifdef DEBUG_RUN_MATH_TREE
   MessageInterface::ShowMessage
      ("Assignment::RunMathTree() returning, outWrapper=<%p>\n", outWrapper);
   #endif

   return outWrapper;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//          const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Object rename method (matching the GmatBase version)
 *
 * @param type The type of object getting renamed
 * @param oldName Name of the object before it get changed
 * @param newName The new object name
 *
 * @return true if an object name was changed
 */
//------------------------------------------------------------------------------
bool RHSEquation::RenameRefObject(const UnsignedInt type, const std::string &oldName,
         const std::string &newName)
{
   bool retval = false;

   if (mathTree)
      retval = mathTree->RenameRefObject(type, oldName, newName);

   // Handle the wrapper map
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

            // Do not remove the iterator - it is used to iterate the for loop!
            // mathWrapperMap.erase(ewi);
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// StringArray GetWrapperObjectNames()
//------------------------------------------------------------------------------
/**
 * Accessor for the names of objects used in the RHS expression
 *
 * @return A list of object names
 */
//------------------------------------------------------------------------------
StringArray RHSEquation::GetWrapperObjectNames()
{
   StringArray wons;

   if (mathTree == NULL)
      throw CommandException("The equation \"" + rhsString +
            "\" has not yet been constructed");

   MathNode *topNode = mathTree->GetTopNode();
   if (topNode && topNode->IsOfType("BuiltinFunctionNode"))
   {
      #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage("   It is a builtin function call\n");
      #endif
      // Add math node elements to wrapper object names for string function
      StringArray tmpArray = mathTree->GetWrapperObjectNameArray();
      if (tmpArray.size() > 0)
         wons.insert(wons.end(), tmpArray.begin(), tmpArray.end());
   }
   else
   {
      // Add math node elements to ref object names
      StringArray tmpArray = mathTree->GetRefObjectNameArray(Gmat::PARAMETER);
      if (tmpArray.size() > 0)
         wons.insert(wons.end(), tmpArray.begin(), tmpArray.end());
   }

   #ifdef DEBUG_WRAPPER_CODE
      MessageInterface::ShowMessage("   Got the following from the MathTree:\n");
   #endif

   // Initialize the wrapper map
   if (mathWrapperMap.size() == 0)
   {
      for (UnsignedInt i = 0; i < wons.size(); ++i)
         mathWrapperMap[wons[i]] = NULL;
   }

   return wons;
}


//------------------------------------------------------------------------------
// StringArray GetFunctionNames()
//------------------------------------------------------------------------------
/**
 * Returns the list of functions used in the equation
 *
 * @return A string array containing the function names
 */
//------------------------------------------------------------------------------
StringArray RHSEquation::GetFunctionNames()
{
   StringArray funs;
   if (mathTree)
      funs = mathTree->GetGmatFunctionNames();
   return funs;
}
