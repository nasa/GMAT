//$Id$
//------------------------------------------------------------------------------
//                                   MathParser
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS task
// 124.
//
// Author: Darrel J. Conway
// Created: 2006/03/16
// Modified: 2006/04/10 Linda Jun, NASA/GSFC
//   - Added actual code
//
/**
 * Class implementation for the MathParser.
 */
//------------------------------------------------------------------------------

#include "MathParser.hpp"
#include "MathFunction.hpp"
#include "FunctionRunner.hpp"
#include "StringUtil.hpp"       // for ParseParameter()
#include "FileManager.hpp"      // for GetGmatFunctionPath()
#include "MessageInterface.hpp"

#ifdef __UNIT_TEST__
#include "MathFactory.hpp"
#else
#include "Moderator.hpp"
#endif

//#define DEBUG_PARSE 1
//#define DEBUG_PARSE_NODE 1
//#define DEBUG_PARSE_EQUATION 1
//#define DEBUG_REMOVE_SPACES_IN_MATH_EQ 1
//#define DEBUG_MATH_PARSER 2
//#define DEBUG_DECOMPOSE 1
//#define DEBUG_PARENTHESIS 1
//#define DEBUG_FIND_LOWEST_OPERATOR 2
//#define DEBUG_FIND_OPERATOR 2
//#define DEBUG_ADD_SUBTRACT 1
//#define DEBUG_MULT_DIVIDE 1
//#define DEBUG_MATRIX_OPS 1
//#define DEBUG_FUNCTION 2
//#define DEBUG_ATAN2
//#define DEBUG_MATH_PARSER_PARAM 1
//#define DEBUG_INVERSE_OP 1
//#define DEBUG_POWER 1
//#define DEBUG_UNARY 1
//#define DEBUG_CREATE_NODE 1
//#define DEBUG_GMAT_FUNCTION 1
//#define DEBUG_MATH_ELEMENT

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

//------------------------------------------------------------------------------
//  MathParser(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the MathParser object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <nomme>   Name for the object
 */
//------------------------------------------------------------------------------
MathParser::MathParser(ObjectMap *objMap)
{
   theObjectMap = objMap;
   theGmatFuncCount = 0;
   powerOpStr = "^";
   inverseOpStr = "$";
   transposeOpStr = "'";
   powerOp = powerOpStr[0];
   inverseOp = inverseOpStr[0];
   transposeOp = transposeOpStr[0];
   
   BuildAllFunctionList();
}


//------------------------------------------------------------------------------
//  MathParser(const MathParser &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the MathParser object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
MathParser::MathParser(const MathParser &copy)
{
   theGmatFuncCount = 0;
   BuildAllFunctionList();
}


//------------------------------------------------------------------------------
//  MathParser& operator=(const MathParser &right)
//------------------------------------------------------------------------------
/**
 * Sets one MathParser object to match another (assignment operator).
 * 
 * @param <right> The object that is copied.
 * 
 * @return this object
 */
//------------------------------------------------------------------------------
MathParser& MathParser::operator=(const MathParser &right)
{
   if (this == &right)
      return *this;
   
   theGmatFuncCount = 0;
   BuildAllFunctionList();
   return *this;
}


//------------------------------------------------------------------------------
//  ~MathParser(void)
//------------------------------------------------------------------------------
/**
 * Destroys the MathParser object (destructor).
 */
//------------------------------------------------------------------------------
MathParser::~MathParser()
{
}


//------------------------------------------------------------------------------
// bool IsEquation(const std::string &str, bool checkMinusSign)
//------------------------------------------------------------------------------
/*
 * Examines if given string is a math equation.
 * Call this method with RHS of assignment.
 *
 * @param str The string to be examined for an equation
 * @param checkMinusSign Check for leading minus sign.
 *        Set this flag to true if minus sign is a part of string and is not considered
 *        math unary operator
 * @param Flag indicating that the equation can consist of a single entity
 *
 * @return  true if string is an equation, means has math operators and/or functions
 */
//------------------------------------------------------------------------------
bool MathParser::IsEquation(const std::string &str, bool checkMinusSign,
      bool allowSingleElement)
{
   // Convert tabs to spaces as we start
   std::string strNoTab = GmatStringUtil::Replace(str, "\t", " ");

   theEquation = strNoTab;
   
   #if DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("=================================================================\n");
   MessageInterface::ShowMessage
      ("MathParser::IsEquation() entered, str=<%s>, checkMinusSign=%d\n",
       str.c_str(), checkMinusSign);
   MessageInterface::ShowMessage
      ("=================================================================\n");
   #endif
   
   bool isEq = false;
   std::string left, right;
   Real rval;
   std::string::size_type opIndex;
   Integer errCode;
   
   // Check if string is enclosed with quotes
   if (GmatStringUtil::IsEnclosedWith(strNoTab, "'"))
   {
      #if DEBUG_PARSE_EQUATION
      MessageInterface::ShowMessage
         ("MathParser::IsEquation(%s) returning false, it is enclosed with quotes\n", str.c_str());
      #endif
      return false;
   }
   // Check if it is just a number
   else if (GmatStringUtil::IsValidReal(strNoTab, rval, errCode))
   {
      #if DEBUG_PARSE_EQUATION
      MessageInterface::ShowMessage("   real value = %g\n", rval);
      MessageInterface::ShowMessage
         ("MathParser::IsEquation(%s) returning false, it is a number\n", str.c_str());
      #endif
      return false;
   }
   else
   {
      #if DEBUG_PARSE_EQUATION
      MessageInterface::ShowMessage
         ("   GmatStringUtil::IsValidReal() errCode=%d\n", errCode);
      #endif
      
      // If it has only numbers and multiple dots
      if (errCode == -3)
      {
         #if DEBUG_PARSE_EQUATION
         MessageInterface::ShowMessage
            ("MathParser::IsEquation() 1 Throwing exception - Invalid number or invalid scientific notation\n");
         #endif
         throw MathException("Invalid number or math equation");
      }
      
      std::string str1 = strNoTab;
      // Check for any math symbols or blank and comma, etc
      if (str1.find_first_of(" (),*/+-^'") == str1.npos)
      {
         // Check for math function without input
         if (IsFunctionWithOptionalInput(str1))
         {
            #if DEBUG_PARSE_EQUATION
            MessageInterface::ShowMessage
               ("MathParser::IsEquation(%s) returning true, '%s' is a function with optional input\n",
                str.c_str(), str.c_str());
            #endif
            return true;
         }
         else if (!allowSingleElement)
         {
            #if DEBUG_PARSE_EQUATION
            MessageInterface::ShowMessage
               ("MathParser::IsEquation(%s) returning false, no () or math symbols found\n", str.c_str());
            #endif
            return false;
         }
      }
      
      // If [] found as in sat.Quaternion = [1.0 0.0 0.0 0.0], return false
      if (str1.find_first_of("[]") != str1.npos)
      {
         if (GmatStringUtil::IsEnclosedWithBrackets(str1))
         {
            #if DEBUG_PARSE_EQUATION
            MessageInterface::ShowMessage
               ("MathParser::IsEquation(%s) returning false, enclosed with []\n", str.c_str());
            #endif
            return false;
         }
      }
      
      bool isFunctionCall = false;
      // If invalid number found in the equation, check for more invalid math equation
      if (errCode <= -4)
      {
         // Check if any invalid math operators found
         #if DEBUG_PARSE_EQUATION
         MessageInterface::ShowMessage("   Checking for any invalid math operators\n");
         #endif
         if (!GmatStringUtil::IsMathEquation(str1, true))
         {
            #if DEBUG_PARSE_EQUATION
            MessageInterface::ShowMessage
               ("MathParser::IsEquation() 2 Throwing exception - Invalid number or math equation\n");
            #endif
            throw MathException("Invalid number or math equation");
         }
         else
         {
            // Check for invalid names without removing spaces first
            #if DEBUG_PARSE_EQUATION
            MessageInterface::ShowMessage
               ("   It is valid math equation. Checking for any invalid names in '%s'\n",
                str1.c_str());
            #endif
            std::string fnArg;
            std::string fnName = GmatStringUtil::ParseFunctionName(str1, fnArg);
            #if DEBUG_PARSE_EQUATION
            MessageInterface::ShowMessage
               ("   fnName = '%s', fnArgs = '%s'\n", fnName.c_str(), fnArg.c_str());
            #endif
            
            std::string str2 = str1;
            
            // Remove [] before checking if it is a function call with args
            if (fnName != "" && fnArg != "")
            {
               // If function is builtin function, validate more
               if (HasFunctionName(fnName, builtinFuncList))
               {
                  #if DEBUG_PARSE_EQUATION
                  MessageInterface::ShowMessage
                     ("   It is a builtin function call, so remove [] from fnArg before "
                      "checking for valid names\n");
                  #endif
                  if (fnArg.find("[") != fnArg.npos && fnArg.find("]") != fnArg.npos)
                  {
                     std::string noOuterParen = GmatStringUtil::RemoveOuterParen(fnArg);
                     // Trim both for validating args with spaces. ex) function( [a  b c ])
                     noOuterParen = GmatStringUtil::Trim(noOuterParen);
                     #if DEBUG_PARSE_EQUATION
                     MessageInterface::ShowMessage
                        ("   After removing outer () and trim, noOuterParen = '%s'\n",
                         noOuterParen.c_str());
                     #endif
                     if (GmatStringUtil::IsEnclosedWithBrackets(noOuterParen))
                     {
                        #if DEBUG_PARSE_EQUATION
                        MessageInterface::ShowMessage
                           ("   It is enclosed with [], so removing []\n");
                        #endif
                        str2 = GmatStringUtil::ReplaceFirst(str2, "[", "");
                        str2 = GmatStringUtil::ReplaceFirst(str2, "]", "");
                        // Replace semicolons with comma since it is allowed inside [] (LOJ: 2016.06.08)
                        str2 = GmatStringUtil::Replace(str2, ";", ",");
                        // Replace blank inside [] with command so that [1 2 3] can be validated
                        str2 = GmatStringUtil::Replace(str2, " ", ",");
                     }
                  }
               }
               else
               {
                  #if DEBUG_PARSE_EQUATION
                  MessageInterface::ShowMessage("   It is not a builtin function call\n");
                  #endif
               }
            }
            
            if (!GmatStringUtil::AreAllNamesValid(str2, true))
            {
               bool isValid = false;
               // Check if valid string literal is passed to a function
               if (str1.find("'") != str1.npos)
               {
                  #if DEBUG_PARSE_EQUATION
                  MessageInterface::ShowMessage
                     ("   String literal found, check if it is string function call\n");
                  #endif
                  isValid = GmatStringUtil::IsValidFunctionCall(str1);
               }
               if (isValid)
                  isFunctionCall = true;
               
               if (!isValid)
               {
                  #if DEBUG_PARSE_EQUATION
                  MessageInterface::ShowMessage
                     ("*** ERROR *** MathParser::IsEquation() 3 Throwing exception - "
                      "Invalid number or names\n");
                  #endif
                  throw MathException("Invalid number or names in math equation");
               }
            }
            else
            {
               #if DEBUG_PARSE_EQUATION
               MessageInterface::ShowMessage
                  ("   All names are valid, check for invalid function call for '%s'\n",
                   str1.c_str());
               #endif
               
               // Do prelimary check if it is string function call. String litereral
               // can be passed to string function such as '   my string ...' (LOJ: 2016.02.16)
               StringArray callItems = GmatStringUtil::ParseFunctionCall(str1);
               #if DEBUG_PARSE_EQUATION
               for (unsigned int i = 0; i < callItems.size(); i++)
                  MessageInterface::ShowMessage("   callItems[%d] = '%s'\n", i, callItems[i].c_str());
               #endif
               // Make function call to true if there is at least one argument
               if (callItems.size() > 1)
                  isFunctionCall = true;
            }
            
            #if DEBUG_PARSE_EQUATION
            MessageInterface::ShowMessage
               ("   <%s> %s a function call\n", str1.c_str(), isFunctionCall ? "is" : "is not");
            #endif
            
            // If it is not a function call, remove spaces in the equation
            if (!isFunctionCall)
            {
               str1 = RemoveSpaceInMathEquation(str1);
               #if DEBUG_PARSE_EQUATION
               MessageInterface::ShowMessage
                  ("   Checking for any errors in the equation\n   After removing "
                   "spaces in math eq, str1=<%s>\n", str1.c_str());
               #endif
               
               // Remove chained plus or minus signs
               str1 = GmatStringUtil::ReplaceChainedUnaryOperators(str1);
               #if DEBUG_PARSE_EQUATION
               MessageInterface::ShowMessage
                  ("   After replacing chained unary +- signs, str1=<%s>\n", str1.c_str());
               #endif
               
               if (!GmatStringUtil::IsMathEquation(str1, false, true))
               {
                  #if DEBUG_PARSE_EQUATION
                  MessageInterface::ShowMessage
                     ("MathParser::IsEquation() 4 Throwing exception - Invalid number or math equation\n");
                  #endif
                  throw MathException("Invalid number or math equation");
               }
            }
         }
      }
      
      // First check for string function which does not need math equation checking
      if (isFunctionCall)
      {
         #if DEBUG_PARSE_EQUATION
         MessageInterface::ShowMessage("   Check if it has any math function name\n");
         #endif
         
         // Check if function name is user defined Variable or Array name (GMT-3043)
         std::string anyFuncName = GetFunctionName(ANY_FUNCTION, str1, left);
         
         #if DEBUG_PARSE_EQUATION
         MessageInterface::ShowMessage("   anyFuncName = '%s'\n", anyFuncName.c_str());
         #endif
                  
         if (anyFuncName != "")
         {
            #if DEBUG_PARSE_EQUATION
            MessageInterface::ShowMessage("   theObjectMap = <%p>\n", theObjectMap);
            #endif
            if (theObjectMap != NULL && theObjectMap->find(anyFuncName) != theObjectMap->end())
            {
               GmatBase *obj = (*theObjectMap)[anyFuncName];
               if (obj)
               {
                  #if DEBUG_PARSE_EQUATION
                  MessageInterface::ShowMessage
                     ("   the object <%p><%s>'%s' found in the ObjectMap\n",
                      obj, obj->GetTypeName().c_str(),
                      obj->GetName().c_str());
                  #endif
                  
                  // If object is an Array check if it has valid index
                  if (obj->IsOfType(Gmat::ARRAY))
                  {
                     if (GmatStringUtil::IsSimpleArrayElement(str1))
                     {
                        #if DEBUG_PARSE_EQUATION
                        MessageInterface::ShowMessage
                           ("MathParser::IsEquation(%s) returning false, RHS is an Array\n",
                            str.c_str());
                        #endif
                        return false;
                     }
                  }
                  
                  std::string objTypeName = obj->GetTypeName();
                  #if DEBUG_PARSE_EQUATION
                  MessageInterface::ShowMessage
                     ("   Invalid indexing used with object type '%s' named '%s'\n",
                      objTypeName.c_str(), anyFuncName.c_str());
                  #endif
                  throw MathException
                     ("Invalid indexing used with object type \"" +
                      objTypeName + "\" named \"" + anyFuncName + "\"");
               }
            }
         }
         
         
         // Check if builtin function name found. Since builtin function cannot
         // used with math operators no further checking is needed.
         std::string builtinFuncName = GetFunctionName(BUILTIN_FUNCTION, str1, left);
         if (builtinFuncName != "")
         {
            #if DEBUG_PARSE_EQUATION
            MessageInterface::ShowMessage
               ("MathParser::IsEquation(%s) returning true, '%s' is built-in function\n",
                str1.c_str(), builtinFuncName.c_str());
            #endif
            return true;
         }
      }
      
      // Replace ^(-1) with inverse op
      str1 = GmatStringUtil::Replace(str1, "^(-1)", inverseOpStr);
      #if DEBUG_PARSE_EQUATION
      MessageInterface::ShowMessage
         ("   After replacing ^(-1) with %s, str1=<%s>\n", inverseOpStr.c_str(), str1.c_str());
      #endif
      // Remove blanks before checking for function name
      str1 = GmatStringUtil::RemoveAllBlanks(str1);
      #if DEBUG_PARSE_EQUATION
      MessageInterface::ShowMessage
         ("   After blank removal, str1=<%s>\n", str1.c_str());
      #endif
      
      // Remove extra layer of parenthesis before checking
      str1 = GmatStringUtil::RemoveExtraParen(str1);
      #if DEBUG_PARSE_EQUATION
      MessageInterface::ShowMessage
         ("   After extra paren removal, str1=<%s>\n", str1.c_str());
      #endif
      
      // Check for no operands
      if (str1.size() == 1 && GmatStringUtil::IsMathOperator(str1[0]))
      {
         #if DEBUG_PARSE_EQUATION
         MessageInterface::ShowMessage
            ("MathParser::IsEquation() 5 Throwing exception - Invalid number or math equation\n");
         #endif
         throw MathException("Incomplete math equation");
      }
      
      // build GmatFunction list first
      BuildGmatFunctionList(str1);
      
      if (GetFunctionName(MATH_FUNCTION, str1, left) != "" ||
          GetFunctionName(MATRIX_FUNCTION, str1, left) != "" ||
          GetFunctionName(UNIT_CONVERSION, str1, left) != "" ||
          FindOperatorFrom(str1, 0, left, right, opIndex) != "" ||
          GetFunctionName(GMAT_FUNCTION, str1, left) != "")
      {
         isEq = true;

         if (checkMinusSign)
         {
            // Check for - sign used as string
            if (GmatStringUtil::NumberOfOccurrences(str1, '-') == 1 &&
                GmatStringUtil::StartsWith(str1, "-") &&
                GmatStringUtil::IsSingleItem(str1))
            {
               isEq = false;
            }
         }
      }
      else
      {
         // Check for matrix transpose and inverse operator
         if (str1.find(transposeOpStr) != str1.npos || str1.find(inverseOpStr) != str1.npos)
            isEq = true;
         else if (allowSingleElement)
            isEq = true;
      }
   }
   
   #if DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("MathParser::IsEquation(%s) returning %u\n", str.c_str(), isEq);
   #endif
   
   return isEq;
}


//------------------------------------------------------------------------------
// std::string FindLowestOperator(const std::string &str, Integer &opIndex,
//                                Integer start = 0)
//------------------------------------------------------------------------------
/*
 * Finds lowest operator from the input string.
 * Single operators are +, -, *, /, ^, '.
 *
 * Precedence of operators is (highest to lowest)
 *    Parentheses ()
 *    power(^), matrix Transpose('), matrix inverse (^(-1)) - more to right has lower precedence
 *    Unary plus (+), unary minus (-)
 *    Multiplication (*), right division (/), matrix multiplication (*), matrix right division (/)
 *    Addition (+), subtraction (-)
 *
 * @param  str  Input string
 * @param  opIndex  Index of operator
 * @param  start  Index to start
 *
 * @return  Single operator,
 *          "", if operator not found
 */
//------------------------------------------------------------------------------
std::string MathParser::FindLowestOperator(const std::string &str,
                                           Integer &opIndex, Integer start)
{
   #if DEBUG_FIND_LOWEST_OPERATOR
   MessageInterface::ShowMessage
      ("============================== FindLowestOperator() entered, str=%s, start=%u, length=%u\n", str.c_str(),
       start, str.size());
   #endif
   
   Integer firstOpen = str.find_first_of("(", start);
   Integer start1 = start;
   Integer open1 = -1, close1 = -1;
   Integer length = str.size();
   Integer index = -1;
   bool isOuterParen = false;
   bool done = false;
   std::string opStr;
   IntegerMap opIndexMap;
   std::string substr;
   IntegerMap::iterator pos;
   
   #if DEBUG_FIND_LOWEST_OPERATOR
   MessageInterface::ShowMessage("   firstOpen=%u\n", firstOpen);
   #endif
   
   if (firstOpen == (Integer)str.npos)
      firstOpen = -1;   
   
   #if DEBUG_FIND_LOWEST_OPERATOR
   MessageInterface::ShowMessage
      ("FindLowestOperator() Find operator before first open parenthesis from <%s>\n"
       "   firstOpen=%d\n", str.c_str(), firstOpen);
   #endif
   
   //-----------------------------------------------------------------
   // find a lowest operator before first open paren
   //-----------------------------------------------------------------
   if (firstOpen > 0)
   {
      substr = str.substr(0, firstOpen);
      opStr = FindOperator(substr, index);
      if (opStr != "")
         opIndexMap[opStr] = index;
   }
   
   #if DEBUG_FIND_LOWEST_OPERATOR
   MessageInterface::ShowMessage
      ("FindLowestOperator() Find lowest operator before the last matching "
       "close parenthesis from <%s>\n", str.c_str());
   #endif
   //-----------------------------------------------------------------
   // find a lowest operator before last matching close paren
   //-----------------------------------------------------------------
   while (!done)
   {
      GmatStringUtil::FindMatchingParen(str, open1, close1, isOuterParen, start1);
      
      // find next open parenthesis '('
      std::string::size_type start2 = str.find('(', close1);
      if (start2 == str.npos)
      {
         #if DEBUG_FIND_LOWEST_OPERATOR > 1
         MessageInterface::ShowMessage
            ("   ===> There is no ( found after %d, so setting done to true\n", close1);
         #endif
         // Need to go through next steps when no more open parenthesis found,
         // so setting done = true instead of break
         // (LOJ: 2016.11.04 for GMT-5896 fix)
         //break;
         done = true;
      }
      
      start1 = (Integer)start2;
      #if DEBUG_FIND_LOWEST_OPERATOR > 1
      MessageInterface::ShowMessage("   last close paren is at %u\n", close1);
      MessageInterface::ShowMessage("   found next open parenthesis at %u\n", start1);
      #endif
      
      substr = str.substr(close1+1, start1-close1-1);
      
      #if DEBUG_FIND_LOWEST_OPERATOR > 1
      MessageInterface::ShowMessage
         ("   substr=%s from %u and length of %u\n", substr.c_str(), close1+1, start1-close1-1);
      #endif
      
      bool isAfterCloseParen = (close1 != -1 ? true : false);
      opStr = FindOperator(substr, index, isAfterCloseParen);
      
      if (opStr != "")
         opIndexMap[opStr] = close1 + index + 1;
   }
   
   
   //-----------------------------------------------------------------
   // find a lowest operator after last close paren
   //-----------------------------------------------------------------
   if (close1 != length-1)
   {
      substr = str.substr(close1+1);
      #if DEBUG_FIND_LOWEST_OPERATOR
      MessageInterface::ShowMessage
         ("FindLowestOperator() Find lowest operator after the last close parenthesis, "
          "substr=%s\n", substr.c_str());
      #endif
      
      opStr = FindOperator(substr, index);
      
      #if DEBUG_FIND_LOWEST_OPERATOR
      MessageInterface::ShowMessage
         ("FindOperator() returned opStr=<%s>, index=%d\n", opStr.c_str(), index);
      #endif
      
      if (opStr != "")
         opIndexMap[opStr] = close1 + index + 1;
   }
   
   #if DEBUG_FIND_LOWEST_OPERATOR
   MessageInterface::ShowMessage("   There are %d operators\n", opIndexMap.size());
   for (pos = opIndexMap.begin(); pos != opIndexMap.end(); ++pos)
      MessageInterface::ShowMessage
         ("      op=%s, index=%d\n", pos->first.c_str(), pos->second);
   #endif
   
   IntegerMap::iterator pos1 = opIndexMap.end();
   IntegerMap::iterator pos2 = opIndexMap.end();
   IntegerMap::iterator pos3 = opIndexMap.end();
   #if DEBUG_FIND_LOWEST_OPERATOR
      Integer index1 = -1;
   #endif
   Integer index2 = -1;
   std::string lastOp;
   bool opFound = false;
   bool unaryMinusFound = false;
   
   // find + or - first
   pos1 = opIndexMap.find("+");
   pos2 = opIndexMap.find("-");
   
   #if DEBUG_FIND_LOWEST_OPERATOR
   MessageInterface::ShowMessage
      ("   +op index=%d, -op index=%d\n",
       pos1 == opIndexMap.end() ? -1 : pos1->second,
       pos2 == opIndexMap.end() ? -1 : pos2->second);
   #endif
   
   if (pos1 != opIndexMap.end() || pos2 != opIndexMap.end())
   {
      #if DEBUG_FIND_LOWEST_OPERATOR
         if (pos1 != opIndexMap.end())
            index1 = pos1->second;
      #endif
      if (pos2 != opIndexMap.end())
         index2 = pos2->second;
      
      #if DEBUG_FIND_LOWEST_OPERATOR
      MessageInterface::ShowMessage("   index1=%d, index2=%d\n", index1, index2);
      #endif
      
      // Check for unary - operator
      if (index2 == 0)
      {
         // Check for function
         if (firstOpen > 0 && IsParenPartOfFunction(str.substr(1, firstOpen-1)))
            unaryMinusFound = false;
         else
            unaryMinusFound = true;
      }
      
      opStr = GetOperator(pos1, pos2, pos3, opIndexMap, index);
      if (opStr != "")
         opFound = true;
   }
   
   #if DEBUG_FIND_LOWEST_OPERATOR
   MessageInterface::ShowMessage
      ("   unaryMinusFound=%d, opFound=%d\n", unaryMinusFound, opFound);
   #endif
   
   if (!opFound)
   {
      // find * or /
      pos1 = opIndexMap.find("*");
      pos2 = opIndexMap.find("/");
      if (pos1 != opIndexMap.end() || pos2 != opIndexMap.end())
      {
         opStr = GetOperator(pos1, pos2, pos3, opIndexMap, index);
      }
      else
      {
         if (unaryMinusFound)
         {
            index = 0;
            opStr = "-";
         }
         else
         {
            // Find transpose or inverse of matrix or power
            pos1 = opIndexMap.find(inverseOpStr);
            pos2 = opIndexMap.find(transposeOpStr);
            pos3 = opIndexMap.find(powerOpStr);
            opStr = GetOperator(pos1, pos2, pos3, opIndexMap, index);
         }
      }
   }
   
   opIndex = index;
   
   #if DEBUG_FIND_LOWEST_OPERATOR
   MessageInterface::ShowMessage
      ("============================== FindLowestOperator() returning opStr=%s, opIndex=%d\n\n",
       opStr.c_str(), opIndex);
   #endif
   
   return opStr;
}


//------------------------------------------------------------------------------
// MathNode*  Parse(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Breaks apart the text representation of an equation and uses the component
 * pieces to construct the MathTree.
 *
 * @param str  Input equation to be parsed
 * @param allowSingleNode Allow a "tree" with only a top node
 *
 * @return constructed MathTree pointer
 */
//------------------------------------------------------------------------------
MathNode* MathParser::Parse(const std::string &str, bool allowSingleNode)
{
   std::string strNoTab = GmatStringUtil::Replace(str, "\t", " ");

   originalEquation = strNoTab;
   theEquation = strNoTab;
   
   #if DEBUG_PARSE
   MessageInterface::ShowMessage
      ("\n=================================================================\n");
   MessageInterface::ShowMessage
      ("MathParser::Parse() entered, theEquation=%s\n", theEquation.c_str());
   MessageInterface::ShowMessage
      ("=================================================================\n");
   #endif

   // first remove all blank spaces, extra layer of parenthesis, and semicolon
   std::string newEq = RemoveSpaceInMathEquation(theEquation);
   #if DEBUG_PARSE
   MessageInterface::ShowMessage
      ("   After removing spaces, newEq=%s\n", newEq.c_str());
   #endif
   
   newEq = GmatStringUtil::RemoveExtraParen(newEq);
   #if DEBUG_PARSE
   MessageInterface::ShowMessage
      ("   After removing extra parenthesis, newEq=%s\n", newEq.c_str());
   #endif
   
   std::string::size_type index = newEq.find_first_of(';');
   if (index != newEq.npos && index == newEq.length() - 1)
      newEq.erase(index);
   
   // check if parenthesis are balanced
   if (!GmatStringUtil::IsParenBalanced(newEq))
      throw MathException("Found unbalanced parenthesis");
   
   #if DEBUG_PARSE
   MessageInterface::ShowMessage
      ("MathParser::Parse() newEq=%s\n", newEq.c_str());
   #endif
   
   // First check for string function which does not need math equation checking
   std::string left = "";
   std::string builtinFuncName = GetFunctionName(BUILTIN_FUNCTION, newEq, left);
   #if DEBUG_PARSE
   MessageInterface::ShowMessage("   builtinFuncName='%s'\n", builtinFuncName.c_str());
   #endif
   if (builtinFuncName != "")
   {
      #if DEBUG_PARSE
      MessageInterface::ShowMessage
         ("   '%s' is built-in function, use original string\n", builtinFuncName.c_str());
      #endif
      MathNode *topNode = CreateNode(builtinFuncName, str);
      #if DEBUG_PARSE
      MessageInterface::ShowMessage
         ("MathParser::Parse() newEq<%s> is a built-in function, so returning "
          "node of %s<%p>\n", newEq.c_str(), builtinFuncName.c_str(), topNode);
      #endif
      return topNode;
   }
   
   // Check for invalid math symbols
   if (!GmatStringUtil::IsMathEquation(newEq, true))
      throw MathException("Invalid math equation");
   
   // Check for invalid math equation for transpose
   #if DEBUG_PARSE
   MessageInterface::ShowMessage("   Checking for invalid math equation\n");
   #endif
   std::string::size_type length = newEq.size();
   for (UnsignedInt i = 0; i < length; i++)
   {
      char ch = newEq[i];
      if (ch == transposeOp)
      {
         if (i <= length-2)
         {
            char nextCh = newEq[i+1];
            if (GmatStringUtil::IsMathOperator(nextCh) || nextCh == ')' || nextCh == ',')
               continue;
            else
            {
               #if DEBUG_PARSE
               MessageInterface::ShowMessage
                  ("   Invalid next character after transpose found, nextCh = '%c'\n", nextCh);
               #endif
               throw MathException("Invalid math equation after transpose");
            }
         }
      }
   }
   
   // Check for real number first
   #if DEBUG_PARSE
   MessageInterface::ShowMessage("   Checking if it is a real number\n");
   #endif
   Real rval;
   Integer errCode;
   if (GmatStringUtil::IsValidReal(newEq, rval, errCode))
   {
       MathNode *topNode = CreateNode("MathElement", newEq);
       #if DEBUG_PARSE
       MessageInterface::ShowMessage
          ("MathParser::Parse() newEq<%s> is a real number <%f>, so returning "
           "MathElement node <%p>\n", newEq.c_str(), rval, topNode);
       #endif
       return topNode;
   }
   
   
   // Now start parsing
   // Build GmatFunction list first
   BuildGmatFunctionList(newEq);
   
   // Replace ^(-1) with inverse op (GMT-3046 LOJ: 2012.09.07)
   newEq = GmatStringUtil::Replace(newEq, "^(-1)", inverseOpStr);
   #if DEBUG_PARSE
   MessageInterface::ShowMessage
      ("MathParser::Parse() After replacing ^(-1) with %s\n   newEq=%s\n",
       inverseOpStr.c_str(), newEq.c_str());
   #endif
   
   // Remove spaces first
   newEq = RemoveSpaceInMathEquation(newEq);
   
   // Replace chained unary operators to correct operator
   newEq = GmatStringUtil::ReplaceChainedUnaryOperators(newEq);
   #if DEBUG_PARSE
   MessageInterface::ShowMessage
      ("MathParser::Parse() After replacing chained +- signs\n   newEq=%s\n", newEq.c_str());
   #endif
   
   // Remove first + sign which is unary +
   if (newEq[0] == '+')
      newEq = newEq.substr(1);
   #if DEBUG_PARSE
   MessageInterface::ShowMessage
      ("MathParser::Parse() After removing unary + at 0\n   newEq=%s\n", newEq.c_str());
   #endif
   
   // Add () if newEq is function with no input so that ParseNode will recognize
   // as a function
   if (IsFunctionWithOptionalInput(newEq))
       newEq = newEq + "()";
   
   // Parse top node
   MathNode *topNode = ParseNode(newEq);
   
   #if DEBUG_PARSE
   WriteNode(topNode, 0);
   MessageInterface::ShowMessage
      ("MathParser::Parse() returning topNode=<%p><%s>\n", topNode,
       topNode->GetTypeName().c_str());
   MessageInterface::ShowMessage
      ("=================================================================\n");
   #endif
   
   return topNode;
}


//------------------------------------------------------------------------------
// StringArray GetGmatFunctionNames()
//------------------------------------------------------------------------------
StringArray MathParser::GetGmatFunctionNames()
{
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::GetGmatFunctionNames() returning %d names\n", theGmatFuncCount);
   for (Integer i=0; i<theGmatFuncCount; i++)
      MessageInterface::ShowMessage("   <%s>\n", gmatFuncList[i].c_str());
   #endif
   
   return gmatFuncList;
}


//------------------------------------------------------------------------------
// bool IsInputRequiredForFunction(const std::string &fn)
//------------------------------------------------------------------------------
bool MathParser::IsInputRequiredForFunction(const std::string &fn)
{
   if (fn == "rand" || fn == "randn")
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// bool IsFunctionWithOptionalInput(const std::string &fn)
//------------------------------------------------------------------------------
/**
 * Checks if input is not an object name and it is a function name with
 * no input required.
 */
//------------------------------------------------------------------------------
bool MathParser::IsFunctionWithOptionalInput(const std::string &fn)
{
   #ifdef DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("MathParser::IsFunctionWithOptionalInput() entered, fn='%s'\n", fn.c_str());
   #endif
   
   bool isFunction = false;
   if (theObjectMap != NULL && theObjectMap->find(fn) != theObjectMap->end())
   {
      #ifdef DEBUG_PARSE_EQUATION
      MessageInterface::ShowMessage("   '%s' is a created object name\n", fn.c_str());
      #endif
   }
   else
   {
      if (!IsInputRequiredForFunction(fn) && IsMathFunction(fn))
      {
         #ifdef DEBUG_PARSE_EQUATION
         MessageInterface::ShowMessage("   '%s' is a math function\n", fn.c_str());
         #endif
         isFunction = true;
      }
   }
   
   #ifdef DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("MathParser::IsFunctionWithOptionalInput() returning %d\n", isFunction);
   #endif
   return isFunction;
}

//------------------------------------------------------------------------------
// MathNode* ParseNode(const std::string &str)
//------------------------------------------------------------------------------
MathNode* MathParser::ParseNode(const std::string &str)
{
   #if DEBUG_PARSE_NODE
   MessageInterface::ShowMessage("\nMathParser::ParseNode() entered, str='%s'\n", str.c_str());
   #endif
   
   MathNode *mathNode = NULL;
   
   // Check for math element first
   if (IsMathElement(str))
   {
      std::string str1 = GmatStringUtil::RemoveExtraParen(str);
      #if DEBUG_PARSE_NODE
      MessageInterface::ShowMessage
         ("   After removing extra parenthesis, str=<%s>\n", str1.c_str());
      #endif

      // Add () for math function whose input is optional, so that it can create
      // proper function node
      if (!IsInputRequiredForFunction(str1))
         str1 = str1 + "()";
      
      // Check for GmatFunction with no parenthesis
      if (IsGmatFunction(str1))
      {
         mathNode = CreateNode(str1, "");
         #if DEBUG_PARSE_NODE
         MessageInterface::ShowMessage
            ("MathParser::Parse() <%s> is a GmatFunction, so returning "
             "MathElement node <%p>\n", str.c_str(), mathNode);
         #endif
      }
      else
      {
         mathNode = CreateNode("MathElement", str1);
         #if DEBUG_PARSE_NODE
         MessageInterface::ShowMessage
            ("MathParser::Parse() <%s> is a math element, so returning "
             "MathElement node <%p>\n", str1.c_str(), mathNode);
         #endif
      }
      return mathNode;
   }
   
   #if DEBUG_PARSE_NODE
   MessageInterface::ShowMessage("Calling Decompose(<%s>)\n", str.c_str());
   #endif
   
   StringArray items = Decompose(str);
   std::string op = items[0];
   std::string left = items[1];
   std::string right = items[2];
   
   #if DEBUG_PARSE_NODE
   WriteItems("MathParser::ParseNode() After Decompose()", items);
   #endif
   
   // If operator is empty, create MathElement, create MathFunction otherwise
   if (op == "")
   {
      #if DEBUG_PARSE_NODE
      MessageInterface::ShowMessage
         ("=====> Should create MathElement: '%s'\n", left.c_str());
      #endif
      
      std::string str1 = left;
      if (str1.find_first_of("()") != str1.npos)
      {
         // Remove extra parenthesis before creating a node (LOJ: 2010.07.29)
         str1 = GmatStringUtil::RemoveExtraParen(str1);
         
         // Check for empty parenthesis such as () or (()) since RemoveExtraParen()
         // does not remove it.
         if (GmatStringUtil::IsParenEmpty(str1))
         {
            #if DEBUG_PARSE_NODE
            MessageInterface::ShowMessage
               ("   <%s> has empty parenthesis, so setting str1 to blank\n", str1.c_str());
            #endif
            str1 = "";
         }
      }
      
      #if DEBUG_PARSE_NODE
      MessageInterface::ShowMessage
         ("       Creating MathElement with '%s'\n", str1.c_str());
      #endif
      
      if (str1 == "")
      {
         // Check if it contains math
         if (GmatStringUtil::IsMathEquation(str))
         {
            #if DEBUG_PARSE_NODE
            MessageInterface::ShowMessage
               ("MathParser::ParseNode(<%s>) throwing math is not allowed in array element referencing\n",
                str.c_str());
            #endif
            throw MathException("Math is not allowed in array element referencing");
         }
         else
         {
            #if DEBUG_PARSE_NODE
            MessageInterface::ShowMessage
               ("MathParser::ParseNode(<%s>) *** Throwing Exception: 1 Missing input arguments\n", str.c_str());
            #endif
            throw MathException("Missing input arguments");
         }
      }
      
      mathNode = CreateNode("MathElement", str1);
   }
   else
   {
      #if DEBUG_PARSE_NODE
      MessageInterface::ShowMessage
         ("=====> Should create MathNode: %s\n", op.c_str());
      #endif
      
      std::string operands = "( " + left + ", " + right + " )";
      if (right == "")
         operands = "( " + left + " )";
      
      mathNode = CreateNode(op, operands);
      
      MathNode *leftNode = NULL;
      MathNode *rightNode = NULL;
      
      // if node is FunctionRunner, just create left node as MathElement(loj: 2008.08.25)
      // input nodes are created when FunctionRunner is created
      if (mathNode->IsOfType("FunctionRunner"))
      {
         leftNode = CreateNode("MathElement", left);
         mathNode->SetChildren(leftNode, rightNode);
         
         leftNode->SetFunctionInputFlag(true);
      }
      else
      {
         #if DEBUG_PARSE_NODE
         MessageInterface::ShowMessage
            ("   left='%s', right='%s'\n", left.c_str(), right.c_str());
         #endif
         
         // check for empty argument for function
         if (left == "")
         {
            if (IsMathFunction(op))
            {
               // Check if function's input is optional. There is no input so 1 is assumed
               if (!IsInputRequiredForFunction(op))
               {
                  #if DEBUG_PARSE_NODE
                  MessageInterface::ShowMessage
                     ("'%s' function's input is optional so 1 is assumed\n", op.c_str());
                  #endif
               }
               else
               {
                  #if DEBUG_PARSE_NODE
                  MessageInterface::ShowMessage
                     ("MathParser::ParseNode(<%s>) *** Throwing Exception: "
                      "2 Missing input arguments\n", str.c_str());
                  #endif
                  throw MathException(op + "() - Missing input arguments");
               }
            }
         }
         else
         {
            #if DEBUG_PARSE_NODE
            MessageInterface::ShowMessage
               ("===============> Parse left node: '%s'\n", left.c_str());
            #endif
            leftNode = ParseNode(left);
         }
         
         
         // check if two operands are needed
         if (right == "")
         {
            if (op == "Add" || op == "Subtract" || op == "Multiply" || op == "Divide" || op == "Power")
            {
               MessageInterface::ShowMessage
                  (">>>>> Throwing Exception: %s() - Not enought input argumets\n", op.c_str());
               throw MathException(op + "() - Not enough input arguments");
            }
         }
         else
         {
            #if DEBUG_PARSE_NODE
            MessageInterface::ShowMessage
               ("===============> Parse right node: '%s'\n", right.c_str());
            #endif
            rightNode = ParseNode(right);
         }
         
         mathNode->SetChildren(leftNode, rightNode);
      }
   }
   
   #if DEBUG_PARSE_NODE
   MessageInterface::ShowMessage
      ("MathParser::ParseNode() returning <%p><%s>\n",
       mathNode, mathNode->GetTypeName().c_str());
   #endif
   
   return mathNode;
}


//------------------------------------------------------------------------------
// MathNode* CreateNode(const std::string &type, const std::string &exp)
//------------------------------------------------------------------------------
/**
 * Creates MathNode through the Moderator.
 *
 * @param type  Type of math node to create
 * @param exp   Math expression (equation) to be used as name or passed to FunctionRunner
 *
 * @return StringArray of elements
 */
//------------------------------------------------------------------------------
MathNode* MathParser::CreateNode(const std::string &type, const std::string &exp)
{
   #if DEBUG_CREATE_NODE
   MessageInterface::ShowMessage
      ("\nMathParser::CreateNode() type=%s, expression=%s\n", type.c_str(),
       exp.c_str());
   #endif
   
   // check if type is GmatFunction
   std::string actualType = type;
   std::string nodeName = exp;
   
   // If node is FunctionRunner, add function name to node name (loj: 2008.08.21)
   if (IsGmatFunction(type))
   {
      actualType = "FunctionRunner";
      nodeName = type + exp;
   }
   
   #if DEBUG_CREATE_NODE
   MessageInterface::ShowMessage("   actualType=%s\n", actualType.c_str());
   #endif
   
   MathNode *node = NULL;
   
   #ifdef __UNIT_TEST__
   static MathFactory mf;
   node = mf.CreateMathNode(actualType, nodeName);
   #else
   Moderator* theModerator = Moderator::Instance();
   node = theModerator->CreateMathNode(actualType, nodeName);
   #endif
   
   if (node == NULL)
      throw MathException("Cannot create MathNode of \"" + actualType + "\"");
   
   #ifdef DEBUG_MORE_MEMORY
   MessageInterface::ShowMessage
      ("MathParser::CreateNode() node <%p><%s> '%s' created\n", node, actualType.c_str());
   #endif
   
   if (actualType == "FunctionRunner")
   {
      FunctionRunner *fRunner = (FunctionRunner*)node;
      fRunner->SetFunctionName(type);
      
      // add function input arguments
      std::string exp1 = exp;
      exp1 = GmatStringUtil::RemoveOuterParen(exp1);
      
      #if DEBUG_CREATE_NODE > 1
      MessageInterface::ShowMessage("   exp1='%s'\n", exp1.c_str());
      #endif
      
      StringArray inputs = GmatStringUtil::SeparateBy(exp1, ",", true);
      for (UnsignedInt i=0; i<inputs.size(); i++)
      {
         //================================================================
         #ifdef __ALLOW_MATH_EXP_NODE__
         //================================================================
         
         // Create all input nodes (loj: 2008.08.25)
         MathNode *node = ParseNode(inputs[i]);
         
         #if DEBUG_CREATE_NODE > 1
         MessageInterface::ShowMessage("   inputs[%d] = '%s'\n", i, inputs[i].c_str());
         MessageInterface::ShowMessage
            ("------------------------------------------ Create input nodes\n");
         #endif
         #if DEBUG_CREATE_NODE > 1
         WriteNode(node, 0);
         MessageInterface::ShowMessage
            ("-------------------------------------------------------------\n");
         #endif
         
         fRunner->AddInputNode(node);
         
         // If input is GmatFunction or math equation, set "" (loj: 2008.08.22)
         // Should we do this here? Just hold off for now
         if (IsGmatFunction(inputs[i]) || IsEquation(inputs[i], false))
         {
            Integer type1, row1, col1;
            node->GetOutputInfo(type1, row1, col1);
            if (type1 == Gmat::REAL_TYPE)
               fRunner->AddFunctionInput(inputs[i]);
            else
               fRunner->AddFunctionInput(inputs[i]);
            
            #if DEBUG_CREATE_NODE
            MessageInterface::ShowMessage
               ("   Setting \"\" to FunctionRunner, input has Function or Equation\n");
            #endif
         }
         else
         {
            fRunner->AddFunctionInput(inputs[i]);
         }
         
         //================================================================
         #else
         //================================================================
         
         // check if input is math expression
         if (IsGmatFunction(inputs[i]) || IsEquation(inputs[i], false))
            throw MathException
               ("*** WARNING *** Currently passing math expression to a "
                "function is not allowed in \"" + originalEquation + "\"");
         
         fRunner->AddFunctionInput(inputs[i]);
         
         //================================================================
         #endif
         //================================================================
         
      }
      
      // add function output arguments
      fRunner->AddFunctionOutput("");
      
      // set function inputs and outputs to FunctionManager through FunctionRunner
      fRunner->SetFunctionInputs();
      fRunner->SetFunctionOutputs();
   }
   
   #if DEBUG_CREATE_NODE
   MessageInterface::ShowMessage
      ("MathParser::CreateNode() '%s' returning node <%p><%s>\n", exp.c_str(), node,
       node->GetTypeName().c_str());
   #endif
   
   return node;
}


//------------------------------------------------------------------------------
// StringArray Decompose(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Breaks string into operator, left and right elements.
 *
 * The order of parsing is as follows:
 *    ParseParenthesis()
 *    ParseAddSubtract()
 *    ParseMultDivide()
 *    ParsePower()
 *    ParseUnary()
 *    ParseMatrixOps()
 *    ParseMathFunctions()
 *    ParseUnitConversion()
 *
 * @return StringArray of elements
 *         elements[0] = operator, elements[1] = left, elements[2] = right
 */
//------------------------------------------------------------------------------
StringArray MathParser::Decompose(const std::string &str)
{
   #if DEBUG_DECOMPOSE
   MessageInterface::ShowMessage
      ("MathParser::Decompose() entered, str=%s\n", str.c_str());
   #endif
   
   // Remove outer pair of parenthesis (GMT-3046 LOJ:2012.09.06)
   std::string str1 = GmatStringUtil::RemoveExtraParen(str);
   #if DEBUG_DECOMPOSE
   MessageInterface::ShowMessage
      ("MathParser::Decompose() 1 after removing extra paren, str1=%s\n", str1.c_str());
   #endif
   
   // Remove first + sign which is unary +
   if (str1[0] == '+')
      str1 = str1.substr(1);
   #if DEBUG_DECOMPOSE
   MessageInterface::ShowMessage
      ("MathParser::Decompose() after removing unary + at 0, str1=%s\n", str1.c_str());
   #endif
   
   StringArray items;
   // If no parenthesis nor math symbols found, it is math element
   if (IsMathElement(str1))
   {
      FillItems(items, "", str1, "");
      #if DEBUG_DECOMPOSE
      WriteItems("MathParser::Decompose() 1 It is a math element, returning ", items);
      #endif
      return items;
   }
   
   #if DEBUG_DECOMPOSE
   MessageInterface::ShowMessage
      ("MathParser::Decompose() It is not a math element, so calling ParseParenthesis()\n");
   #endif
   
   items = ParseParenthesis(str1);
   
   #if DEBUG_DECOMPOSE > 1
   WriteItems("MathParser::Decompose() after ParseParenthesis() ", items);
   #endif
   
   // if no operator found and left is not empty, decompose again
   if (items[0] == "" && items[1] != "")
   {
      std::string str2 = items[1];
      // Remove first + sign which is unary +
      if (str2[0] == '+')
      {
         str2 = str2.substr(1);
         items[1] = str2;
      }
      
      if (IsMathElement(items[1]))
      {
         #if DEBUG_DECOMPOSE
         WriteItems("MathParser::Decompose() 2 It is a math element, returning ", items);
         #endif
         return items;
      }
      
      #if DEBUG_DECOMPOSE > 1
      MessageInterface::ShowMessage
         ("MathParser::Decompose() items[0] is blank and items[1] is <%s>, so calling Decompose(<%s>)\n",
          items[1].c_str(), items[1].c_str());
      #endif
      
      items = Decompose(items[1]);
      
      #if DEBUG_DECOMPOSE > 1
      WriteItems("MathParser::Decompose() after Decompose() ", items);
      #endif
      
      if (IsMathElement(items[1]))
      {
         #if DEBUG_DECOMPOSE
         WriteItems("MathParser::Decompose() 2 It is a math element, returning ", items);
         #endif
         return items;
      }
   }
   
   if (GmatStringUtil::IsOuterParen(str1))
       str1 = GmatStringUtil::RemoveExtraParen(str1);
   
   #if DEBUG_DECOMPOSE > 1
   MessageInterface::ShowMessage
      ("MathParser::Decompose() 2 after removing extra paren, str1=%s\n", str1.c_str());
   #endif
   
   if (items[0] == "function")
      items[0] = "";
   
   if (items[0] == "")
   {
      items = ParseAddSubtract(str1);
      
      if (items[0] == "number")
      {
         #if DEBUG_DECOMPOSE
         WriteItems("MathParser::Decompose() It is a number, returning ", items);
         #endif
         items[0] = "";
         return items;
      }
      
      if (items[0] == "")
      {
         items = ParseMultDivide(str1);
         
         if (items[0] == "")
         {
            items = ParsePower(str1);
            
            if (items[0] == "")
            {
               items = ParseUnary(str1);
               
               if (items[0] == "")
               {
                  items = ParseMatrixOps(str1);
                  
                  if (items[0] == "")
                  {
                     items = ParseMathFunctions(str1);
                     if (items[0] == "")
                     {
                        items = ParseUnitConversion(str1);
                        
                     }
                  }
               }
            }
         }
      }
   }
   
   // Catch missing operands here (Fix for GMT-2961 LOJ: 2012.02.23)
   if (items[0] != "" && items[1] == "" && items[2] == "")
   {
      if (IsParenPartOfFunction(items[0]))
      {
         #if DEBUG_DECOMPOSE
         MessageInterface::ShowMessage("   ===> '%s' is a function\n", items[0].c_str());
         #endif
      }
      else
      {
         #if DEBUG_DECOMPOSE
         MessageInterface::ShowMessage
            ("MathParser::Decompose() *** Throwing Exception: %s - Missing arguments\n",
             items[0].c_str());
         #endif
         throw MathException(items[0] + " - 3 Missing input arguments");
      }
   }
   
   #if DEBUG_DECOMPOSE
   WriteItems("MathParser::Decompose() returning ", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseParenthesis(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Returns string array of operator, left and right
 */
//------------------------------------------------------------------------------
StringArray MathParser::ParseParenthesis(const std::string &str)
{
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("\n========================================"
       " MathParser::ParseParenthesis() entered, str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";
   std::string left = "";
   std::string right = "";
   std::string::size_type opIndex = str.npos;
   std::string::size_type openParen = str.find('(');
   
   //-----------------------------------------------------------------
   // if no opening parenthesis '(' found, just return
   //-----------------------------------------------------------------   
   if (openParen == str.npos)
   {
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("======================================== MathParser::ParseParenthesis() returning, "
                 "open parenthesis not found: ", items, true);
      #endif
      
      return items;
   }
   
   //-----------------------------------------------------------------
   // if lowest operator is + or - and not negate, just return
   //-----------------------------------------------------------------
   Integer index1;
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage("   Calling FindLowestOperator() str=<%s>\n", str.c_str());
   #endif
   std::string opStr1 = FindLowestOperator(str, index1);
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage("   lowest operator found <%s>\n", opStr1.c_str());
   #endif
   if ((opStr1 == "+" || opStr1 == "-") && index1 != 0)
   {
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("======================================== MathParser::ParseParenthesis() returning, "
                 "lowest + or - found: ", items, true);
      #endif
      
      return items;      
   }
   
   //-----------------------------------------------------------------
   // if lowest operator is * or /, just return with operator
   //-----------------------------------------------------------------
   if (opStr1 == "*" || opStr1 == "/")
   {      
      bool opFound1;      
      op = GetOperatorName(opStr1, opFound1);
      
      left = str.substr(0, index1);
      right = str.substr(index1+1, str.npos);
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("======================================== MathParser::ParseParenthesis() returning, "
                 " lowest * or / found: ", items, true);
      #endif
      
      return items;
   }
   
   //-----------------------------------------------------------------
   // if lowest operator is ^, just return with operator
   //-----------------------------------------------------------------
   if (opStr1 == powerOpStr)
   {      
      bool opFound1;      
      op = GetOperatorName(opStr1, opFound1);
      
      left = str.substr(0, index1);
      right = str.substr(index1+1, str.npos);
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("======================================== MathParser::ParseParenthesis() returning, "
                 "lowest ^ found: ", items, true);
      #endif
      
      return items;
   }
   
   //-----------------------------------------------------------------
   // if lowest operator is ', just return with operator
   //-----------------------------------------------------------------
   if (opStr1 == transposeOpStr || opStr1 == inverseOpStr)
   {      
      bool opFound1;      
      op = GetOperatorName(opStr1, opFound1);
      
      left = str.substr(0, index1);
      right = str.substr(index1+1, str.npos);
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("======================================== MathParser::ParseParenthesis() returning, "
                 "lowest ' or inverse found: ", items, true);
      #endif
      
      return items;
   }
   
   //-----------------------------------------------------------------
   // if ( is part of fuction, just return first parenthesis
   //-----------------------------------------------------------------
   std::string strBeforeParen = str.substr(0, openParen);
   
   #if DEBUG_PARENTHESIS > 1
   MessageInterface::ShowMessage
      ("   ParseParenthesis() strBeforeParen=%s\n", strBeforeParen.c_str());
   #endif
   
   if (IsParenPartOfFunction(strBeforeParen))
   {
      // find matching closing parenthesis
      UnsignedInt index2 = FindMatchingParen(str, openParen);
      
      #if DEBUG_PARENTHESIS > 1
      MessageInterface::ShowMessage
         ("   ParseParenthesis() Parenthesis is part of function. str=%s, size=%u, "
          "openParen=%u, index2=%u\n", str.c_str(), str.size(), openParen, index2);
      #endif
      
      // if last char is ')'
      if (index2 == str.size()-1)
      {
         #if DEBUG_PARENTHESIS > 1
         MessageInterface::ShowMessage
            ("   ParseParenthesis() last char is ) so find function name\n");
         #endif
         // find math function
         op = GetFunctionName(GMAT_FUNCTION, str, left);
         if (op == "")
            op = GetFunctionName(MATH_FUNCTION, str, left);
         if (op == "")
            op = GetFunctionName(MATRIX_FUNCTION, str, left);
         if (op == "")
            op = GetFunctionName(UNIT_CONVERSION, str, left);
      }
      
      #if DEBUG_PARENTHESIS > 1
      MessageInterface::ShowMessage("   ParseParenthesis() op='%s'\n", op.c_str());
      #endif
      
      // See if there is an operator before this function
      std::string op1, left1, right1;
      op1 = FindOperatorFrom(str, 0, left1, right1, opIndex);
      if (op1 != "" && opIndex != str.npos)
      {
         if (opIndex < openParen)
         {
            // return blank for next parse
            FillItems(items, "", "", "");
            
            #if DEBUG_PARENTHESIS
            WriteItems("======================================== MathParser::ParseParenthesis() returning, "
                       " found operator before function: ", items, true);
            #endif
            
            return items;
         }
      }
      
      #ifdef DEBUG_ATAN2
      MessageInterface::ShowMessage
         ("   Check for atan2 function which requires 2 args, str=<%s>\n", str.c_str());
      #endif
      
      // Handle special atan2(y,x) function
      if (op == "atan2")
      {
         std::string::size_type openParen = str.find_first_of('(');
         if (openParen == str.npos)
         {
            throw MathException("Atan2() - Missing or invalid input arguments");
         }
         else
         {
            std::string str1 = str;
            std::string atan2Str = str1.substr(openParen);
            bool parsingFailed = false;
            #ifdef DEBUG_ATAN2
            MessageInterface::ShowMessage("   After removing atan2, atan2Str=<%s>\n", atan2Str.c_str());
            #endif
            
            atan2Str = GmatStringUtil::RemoveExtraParen(atan2Str);
            #ifdef DEBUG_ATAN2
            MessageInterface::ShowMessage("   After removing extra parenthesis, atan2Str=<%s>\n", atan2Str.c_str());
            #endif
            
            StringArray parts = GmatStringUtil::SeparateByComma(atan2Str, false);
            
            #ifdef DEBUG_ATAN2
            for (UnsignedInt i = 0; i < parts.size(); i++)
               MessageInterface::ShowMessage("..... (1) %d '%s'\n", i, parts[i].c_str());
            #endif
            
            // In case all extra parenthesis didn't get removed by GmatStringUtil::RemoveExtraParen()
            // remove it here
            if (parts.size() == 1)
            {
               parsingFailed = true;
               std::string str2 = str1.substr(openParen+1, index2-openParen-1);               
               parts = GmatStringUtil::SeparateByComma(str2, false);
               
               #ifdef DEBUG_ATAN2
               for (UnsignedInt i = 0; i < parts.size(); i++)
                  MessageInterface::ShowMessage("..... (2) %d '%s'\n", i, parts[i].c_str());
               #endif
            }
            
            if (parts.size() == 2)
            {
               left = parts[0];
               right = parts[1];
               if (left != "" && right != "")
                  parsingFailed = false;
            }
            
            if (parsingFailed)
               throw MathException("Atan2() - Missing or invalid input arguments");
         }
      }
      else
      {
         left = str.substr(openParen+1, index2-openParen-1);
      }
      
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("======================================== MathParser::ParseParenthesis() returning,"
                 " parenthesis is part of function: ", items, true);
      #endif
      
      return items;
   }
   
   //-----------------------------------------------------------------
   // if enclosed with parenthesis
   //-----------------------------------------------------------------
   if (GmatStringUtil::IsEnclosedWithExtraParen(str, false))
   {   
      left = str.substr(1, str.size()-2);
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("======================================== MathParser::ParseParenthesis() returning, "
                 "enclosed by parenthesis, so removed: ", items, true);
      #endif
      
      return items;
   }
   
   // Get operator name
   if (opStr1 != "")
   {
      bool opFound;      
      op = GetOperatorName(opStr1, opFound);
      
      if (opFound)
      {
         left = str.substr(0, index1);
         right = str.substr(index1+1, str.npos);
         
         if (op == "Subtract" && left == "")
         {
            op = "Negate";
            left = right;
            right = "";
         }
         
         FillItems(items, op, left, right);
         
         #if DEBUG_PARENTHESIS
         WriteItems("======================================== MathParser::ParseParenthesis() returning, "
                    "found lowest operator: ", items, true);
         #endif
         return items;
      }
   }
   else
   {
      #if DEBUG_PARENTHESIS
      MessageInterface::ShowMessage
         ("   ParseParenthesis() No operator found\n");
      #endif
   }
   
   FillItems(items, op, left, right);
   
   #if DEBUG_PARENTHESIS
   WriteItems("======================================== MathParser::ParseParenthesis() returning, ",
              items, true);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// std::string size_type FindOperatorIndex(std::string::size_type index1,
//                       std::string:size_type index2, std::string::size_type index)
//------------------------------------------------------------------------------
std::string::size_type MathParser::FindOperatorIndex(std::string::size_type index1,
                                                     std::string::size_type index2,
                                                     std::string::size_type index3)
{
   #if DEBUG_FIND_OPERATOR > 1
   MessageInterface::ShowMessage
      ("FindOperatorIndex() entered, index1=%u, index2=%u, index3=%u\n", index1, index2, index3);
   #endif
   
   Integer i1 = (Integer)index1;
   Integer i2 = (Integer)index2;
   Integer i3 = (Integer)index3;
   
   #if DEBUG_FIND_OPERATOR > 1
   MessageInterface::ShowMessage("   i1=%3d, i2=%3d, i3=%3d\n", i1, i2, i3);
   #endif
   
   std::string::size_type index = std::string::npos;
   
   if (i1 > i2 && i1 > i3)
      index = index1;
   else if (i2 > i1 && i2 > i3)
      index = index2;
   else
      index = index3;
   
   #if DEBUG_FIND_OPERATOR > 1
   MessageInterface::ShowMessage("FindOperatorIndex() returning %u\n", index);
   #endif
   
   return index;
}


//------------------------------------------------------------------------------
// std::string FindOperatorFrom(const std::string &str, std::string::size_type start,
//                              std::string &left, std::string &right,
//                              std::string::size_type &opIndex)
//------------------------------------------------------------------------------
std::string MathParser::FindOperatorFrom(const std::string &str, std::string::size_type start,
                                         std::string &left, std::string &right,
                                         std::string::size_type &opIndex)
{
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("MathParser::FindOperatorFrom() entered, str=%s, start=%u\n", str.c_str(), start);
   #endif
   
   StringArray items;
   std::string op;
   std::string str1 = str.substr(start);
   std::string::size_type index = str1.npos;
   str1 = GmatStringUtil::RemoveScientificNotation(str1);
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("   After removing sci notation, str1=<%s>\n", str1.c_str());
   #if DEBUG_FIND_OPERATOR > 1
   MessageInterface::ShowMessage("   Find + -\n");
   #endif
   #endif
   
   std::string::size_type index1 = str1.find_last_of("+");
   std::string::size_type index2 = str1.find_last_of("-");
   std::string::size_type index3 = str1.npos;
   index = FindOperatorIndex(index1, index2);
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage("   +, - index from FindOperatorIndex() = %u\n", index);
   #endif
   
   if (index == str1.npos)
   {
      #if DEBUG_FIND_OPERATOR > 1
      MessageInterface::ShowMessage("   Find * /\n");
      #endif
      index1 = str1.find_last_of("*");
      index2 = str1.find_last_of("/");
      index = FindOperatorIndex(index1, index2);
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage("   *, / index from FindOperatorIndex() = %u\n", index);
      #endif
      
      if (index == str1.npos)
      {
         #if DEBUG_FIND_OPERATOR > 1
         MessageInterface::ShowMessage("   Find transposeOp, inverseOp, or ^\n");
         #endif
         
         index1 = str1.find_last_of(transposeOpStr);
         index2 = str1.find_last_of(inverseOpStr);
         index3 = str1.find_last_of(powerOpStr);
         index = FindOperatorIndex(index1, index2, index3);
         #if DEBUG_FIND_OPERATOR > 1
         MessageInterface::ShowMessage("   '$^ index from FindOperatorIndex() = %u\n", index);
         #endif
      }
   }
   
   if (index != str1.npos)
   {
      bool opFound = false;
      op = str1.substr(index, 1);
      op = GetOperatorName(op, opFound);
      left = str1.substr(0, index);
      right = str1.substr(index+1, str1.size()-index);
      opIndex = index + start;
   }
   else
   {
      opIndex = index;
   }
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("MathParser::FindOperatorFrom() returning op=%s, left=%s, right=%s, opIndex=%u\n",
       op.c_str(), left.c_str(), right.c_str(), opIndex);
   #endif
   
   return op;
   
}


//------------------------------------------------------------------------------
// std::string GetOperatorName(const std::string &op, bool &opFound)
//------------------------------------------------------------------------------
std::string MathParser::GetOperatorName(const std::string &op, bool &opFound)
{
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::GetOperatorName() op=%s\n", op.c_str());
   #endif
   
   std::string opName = "<" + op + "> :Unknown Operator";
   opFound = true;
   
   if (op == "+")
      opName = "Add";
   else if (op == "-")
      opName = "Subtract";
   else if (op == "*")
      opName = "Multiply";
   else if (op == "/")
      opName = "Divide";
   else if (op == powerOpStr)
      opName = "Power";
   else if (op == transposeOpStr)
      opName = "Transpose";
   else if (op == inverseOpStr)
      opName = "Inv";
   else
      opFound = false;
   
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::GetOperatorName() opFound=%u, opName=%s\n", opFound,
       opName.c_str());
   #endif
   
   return opName;
}


//------------------------------------------------------------------------------
// std::string FindOperator(const std::string &str, Integer &opIndex)
//------------------------------------------------------------------------------
/*
 * Finds the right most lowest operator from the input string.
 *
 * Precedence of operators is (lowest to highest)
 *    +, -
 *    *, /
 *    unary -
 *    ', ^, ^(-1)
 *    ()
 *
 * Single operators are +, -, *, /, ^, '
 * Double operators are ++, --, +-, -+, *-, /-, ^+, ^-
 *
 * 
 * @param  str  Input string
 * @param  opIndex  Index of operator
 *
 * @return  Single operator,
 *          First operator, if double operator found
 *          "", if operator not found
 */
//------------------------------------------------------------------------------
std::string MathParser::FindOperator(const std::string &str, Integer &opIndex,
                                     bool isAfterCloseParen)
{
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("========== MathParser::FindOperator() entered, str=<%s>, isAfterCloseParen=%d\n",
       str.c_str(), isAfterCloseParen);
   #endif
   
   std::string str1 = str;
   str1 = GmatStringUtil::RemoveScientificNotation(str1);
   
   StringArray items;
   std::string op;
   std::string::size_type index;
   std::string::size_type index1 = str1.find_last_of("+");
   std::string::size_type index2 = str1.find_last_of("-");
   std::string::size_type firstUnaryMinusIndex = str1.find_first_of("-");
   std::string::size_type index3 = firstUnaryMinusIndex;
   bool unaryMinusFound = false;
   bool checkNext = true;
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindOperator() for +,- index1=%u, index2=%u, firstUnaryMinusIndex=%u\n",
       index1, index2, firstUnaryMinusIndex);
   #endif
   
   if (index1 != str1.npos || index2 != str1.npos)
      checkNext = false;
   
   // Check for unary - operator
   if (firstUnaryMinusIndex != str1.npos)
   {
      // LOJ:2012.01.19 (for bug 2493 fix)
      // if unary - is at the first position and length is greator than 1,
      // check for another + or - after 0
      if (firstUnaryMinusIndex == 0)
      {
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage
            ("   Found unary minus operator at %u\n", firstUnaryMinusIndex);
         #endif
         unaryMinusFound = true;
         
         if (str1.size() > 1)
         {
            // If minus sign found after close parenthesis, then it is not an
            // unary minus, so send whole string to FindOperator() (LOJ: 2012.03.23)
            std::string str2 = (isAfterCloseParen ? str1 : str1.substr(1, str1.npos));
            #if DEBUG_FIND_OPERATOR
            MessageInterface::ShowMessage
               ("   Check if there are more operators in <%s>\n", str2.c_str());
            #endif
            std::string opStr = FindOperator(str2, opIndex);
            if (opStr != "")
            {
               index1 = str2.find_last_of("+");
               index2 = str2.find_last_of("-");
               
               #if DEBUG_FIND_OPERATOR
               MessageInterface::ShowMessage
                  ("   last plus = %u, last minus = %u\n", index1, index2);
               #endif
               
               if (index1 != str1.npos || index2 != str1.npos)
               {
                  if (index1 != str1.npos)
                     index1 = index1 + 1;
                  if (index2 != str.npos)
                     index2 = index2 + 1;
               }
               else
               {
                  // Need to check for * or /
                  checkNext = true;
               }
            }
            #if DEBUG_FIND_OPERATOR
            MessageInterface::ShowMessage
               ("FindOperator() 2 for +,- index1=%u, index2=%u, checkNext=%d\n",
                index1, index2, checkNext);
            #endif
         }
      }
      else
      {
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage("   Check for double operator such as *- or /-\n");
         #endif
         // Check for double operator such as *- or /- except ^-1 for inverse or ' for transpose
         std::string doubleOp = str1.substr(index2-1, 1);
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage("   doubleOp=<%s>\n", doubleOp.c_str());
         #endif
         
         if (doubleOp == powerOpStr)
         {
            // Make - operator not found by setting npos
            index2 = str1.npos;
            checkNext = true;
         }
         else if (doubleOp == transposeOpStr || doubleOp == inverseOpStr)
         {
            checkNext = false;
            unaryMinusFound = false;
         }
         else
         {
            if ((index2 != str1.npos) && IsValidOperator(doubleOp))
            {
               #if DEBUG_FIND_OPERATOR
               MessageInterface::ShowMessage
                  ("   Found unary minus operator at %u\n", firstUnaryMinusIndex);
               #endif
               unaryMinusFound = true;
               // Make - operator not found by setting npos
               index2 = str1.npos;
               firstUnaryMinusIndex = str1.npos;
               checkNext = true;
            }
         }
      }
      
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage("   checkNext=%d\n", checkNext);
      #endif
      
      // Figure out which operator + or - to assign or check for next operator
      if (index1 != str1.npos || index2 != str1.npos)
      {
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage
            ("   Figure out which + or - to use or check for next operator\n");
         #endif
         
         std::string::size_type index4 = FindOperatorIndex(index1, index2);
         
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage("   index4=%u\n", index4);
         #endif
         
         // Changed index4 > 0 to index4 != str1.npos since index1 and index2
         // are recomputed above when unary - is found (LOJ: 2012.01.19)
         //if ((index4 > 0) &&
         if ((index4 != str1.npos) && (index4 > 0) &&
             (str[index4-1] == '*' || str[index4-1] == '/' || str[index4-1] == '\''))
         {
            checkNext = true;
            if (str[index4-1] == '\'')
               checkNext = false;
         }
         else if (index4 == index2 && index4 > 0 && str[index4-1] == '^')
         {
            checkNext = true;
         }
         else if (index4 == index1)
         {
            checkNext = false;
         }
      }
   }
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage("FindOperator() %s *,/\n", checkNext ? "check" : "skip");
   #endif
   
   if (checkNext)
   {
      index1 = str1.find_last_of("*");
      index2 = str1.find_last_of("/");
      
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage
         ("FindOperator() for *,/ index1=%u, index2=%u\n", index1, index2);
      #endif
      
      if (index1 == str1.npos && index2 == str1.npos)
      {
         // Check for the unary minus at the beginning
         //LOJ: 2013.01.30 for GMT-3497 fix
         //if (unaryMinusFound)
         if (unaryMinusFound && firstUnaryMinusIndex != 0)
         {
            op = "-";
            opIndex = 0;
            
            #if DEBUG_FIND_OPERATOR
            MessageInterface::ShowMessage
               ("========== FindOperator() returning op=%s, opIndex=%u, unary minus found\n",
                op.c_str(), opIndex);
            #endif
            
            return op;
         }
         
         // try for transpose or inverse or ^
         index1 = str1.find_last_of(transposeOpStr);
         index2 = str1.find_last_of(inverseOpStr);
         index3 = str1.find_last_of(powerOpStr);
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage
            ("FindOperator() for %s index1=%u, for %s index2=%u, for ^ index3=%u, "
             "str1=<%s>\n", transposeOpStr.c_str(), index1, inverseOpStr.c_str(),
             index2, index3, str1.c_str());
         #endif
         
         if (index1 == str1.npos && index2 == str1.npos)
         {
            // try for ^
            index1 = str1.find_last_of(powerOpStr);
            index2 = str1.npos;
            #if DEBUG_FIND_OPERATOR
            MessageInterface::ShowMessage
               ("FindOperator() for ^ index1=%u, str1=<%s>\n", index1, str1.c_str());
            #endif
         }
      }
   }
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindOperator() index1=%u, index2=%u, index3=%u\n", index1, index2, index3);
   #endif
   
   index = FindOperatorIndex(index1, index2, index3);
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindOperator() Checking for double operator, str1=<%s>, index=%u, index1=%u, "
       "index2=%u\n", str1.c_str(), index, index1, index2);
   #endif
   
   // check for double operator such as *-, /-
   if (index == 0)
   {
      op = str1.substr(index, 1);
      opIndex = index;
   }
   else if ((index > 0 && index != str.npos) &&
            // (str[index-1] == '+' || str[index-1] == '-' ||
            //  str[index-1] == '*' || str[index-1] == '/' ||
            //  str[index-1] == '^'))
            (str1[index-1] == '+' || str1[index-1] == '-' ||
             str1[index-1] == '*' || str1[index-1] == '/' ||
             str1[index-1] == '^'))
   {
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage
         ("   Found double operator <%c> at %d\n", str1[index-1], index-1);
      #endif
      op = str1.substr(index-1, 1);
      opIndex = index - 1;
   }
   else if (index != str1.npos)
   {
      op = str1.substr(index, 1);
      opIndex = index;
   }
   else
   {
      op = "";
      opIndex = -1;
   }
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("========== FindOperator() returning op=%s, opIndex=%u\n", op.c_str(), opIndex);
   #endif
   
   return op;
   
}


//------------------------------------------------------------------------------
// std::string GetOperator(const IntegerMap::iterator &pos1,
//                         const IntegerMap::iterator &pos2,
//                         const IntegerMap::iterator &pos3,
//                         const IntegerMap &opIndexMap,
//                         Integer &opIndex)
//------------------------------------------------------------------------------
/**
 * Finds operator by comparing the position
 *
 * @param pos1  Position of + or * operator
 * @param pos2  Position of - or / opearator
 * @param opIndexMap  Map containing operator positions
 * @param opIndex  Output operator index
 */
//------------------------------------------------------------------------------
std::string MathParser::GetOperator(const IntegerMap::iterator &pos1,
                                    const IntegerMap::iterator &pos2,
                                    const IntegerMap::iterator &pos3,
                                    const IntegerMap &opIndexMap,
                                    Integer &opIndex)
{
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage("GetOperator() entered\n");   
   MessageInterface::ShowMessage
      ("   pos1=<%s><%d>\n", pos1 != opIndexMap.end() ? pos1->first.c_str() : "NULL",
       pos1 != opIndexMap.end() ? pos1->second : -1);
   MessageInterface::ShowMessage
      ("   pos2=<%s><%d>\n", pos2 != opIndexMap.end() ? pos2->first.c_str() : "NULL",
       pos2 != opIndexMap.end() ? pos2->second : -1);
   MessageInterface::ShowMessage
      ("   pos3=<%s><%d>\n", pos3 != opIndexMap.end() ? pos3->first.c_str() : "NULL",
       pos3 != opIndexMap.end() ? pos3->second : -1);
   #endif
   
   std::string opStr;
   Integer index = -1;
   std::string op1 = pos1 != opIndexMap.end() ? pos1->first : "";
   std::string op2 = pos2 != opIndexMap.end() ? pos2->first : "";
   std::string op3 = pos3 != opIndexMap.end() ? pos3->first : "";
   std::string ops = op1 + op2 + op3;
   std::string opsToFind = inverseOpStr + transposeOpStr + powerOpStr;

   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage("   ops=<%s>, opsToFind=<%s>\n", ops.c_str(), opsToFind.c_str());
   #endif
   
   // Find operator for inverse, transpose, and power
   if (ops.find_first_of(opsToFind) != ops.npos)
   {
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage("   inverse, transpose, or power found\n");
      #endif
      std::string::size_type i1 = std::string::npos;
      std::string::size_type i2 = std::string::npos;
      std::string::size_type i3 = std::string::npos;
      
      if (pos1 != opIndexMap.end())
         i1 = pos1->second == -1 ? std::string::npos : pos1->second;
      if (pos2 != opIndexMap.end())
         i2 = pos2->second == -1 ? std::string::npos : pos2->second;
      if (pos3 != opIndexMap.end())
         i3 = pos3->second == -1 ? std::string::npos : pos3->second;
      
      std::string::size_type opIndex1 = FindOperatorIndex(i1, i2, i3);
      
      if (opIndex1 == i1)
      {
         opStr = pos1->first;
         index = pos1->second;
      }
      else if (opIndex1 == i2)
      {
         opStr = pos2->first;
         index = pos2->second;
      }
      else if (opIndex1 == i3)
      {
         opStr = pos3->first;
         index = pos3->second;
      }
      
      opIndex = index;
      
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage
         ("GetOperator() return opStr='%s', opIndex=%d\n", opStr.c_str(), opIndex);
      #endif
      
      return opStr;
   }
   
   
   // Find operator for +, -, *, /
   if (pos1 != opIndexMap.end() || pos2 != opIndexMap.end())
   {
      if (pos2 == opIndexMap.end())
      {
         opStr = pos1->first;
         index = pos1->second;
      }
      else if (pos1 == opIndexMap.end())
      {
         opStr = pos2->first;
         index = pos2->second;
      }
      else
      {
         // if unary - found at 0 position and + found, then set operator to +
         if ((pos2->first == "-" && pos2->second == 0) && pos1 != opIndexMap.end())
         {
             opStr = pos1->first;
             index = pos1->second;
         }
         else
         {
            // if operators on the same level of precedence are evaluated
            // from left to right
            if (pos1->second > pos2->second)
            {
               opStr = pos1->first;
               index = pos1->second;
            }
            else
            {
               opStr = pos2->first;
               index = pos2->second;
            }
         }
      }
   }
   
   opIndex = index;
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("GetOperator() return opStr='%s', opIndex=%d\n", opStr.c_str(), opIndex);
   #endif
   
   return opStr;
}


//------------------------------------------------------------------------------
// std::string RemoveSpaceInMathEquation(const std::string &str)
//------------------------------------------------------------------------------
std::string MathParser::RemoveSpaceInMathEquation(const std::string &str)
{
   #if DEBUG_REMOVE_SPACES_IN_MATH_EQ
   MessageInterface::ShowMessage
      ("\nMathParser::RemoveSpaceInMathEquation() entered, str=<%s>\n", str.c_str());
   #endif
   
   std::string str1 = GmatStringUtil::Trim(str);  
   std::string::iterator begin = str1.begin();
   std::string::iterator pos = str1.begin();
   Integer dist = 0, lastNonBlank = 0;
   while (pos != str1.end())
   {
      dist = distance(begin, pos);
      if (dist > 0)
      {
         char currCh = *pos;
         char prevCh = str1[dist-1];
         #if DEBUG_REMOVE_SPACES_IN_MATH_EQ > 1
         MessageInterface::ShowMessage("   currCh=<%c>, prev char=<%c>\n", currCh, prevCh);
         #endif
         
         if (currCh == ' ')
         {
            #if DEBUG_REMOVE_SPACES_IN_MATH_EQ > 1
            MessageInterface::ShowMessage("   ===> current ch is blank, dist=%d\n", currCh, dist);
            #endif
            if (isalnum(prevCh))
            {
               #if DEBUG_REMOVE_SPACES_IN_MATH_EQ > 1
               MessageInterface::ShowMessage
                  ("   previous ch <%c> is alphanumeric, lastNonBlank=%d\n", prevCh, lastNonBlank);
               #endif
               // Check if digit is part of name
               std::string substr = str1.substr(lastNonBlank, dist-lastNonBlank);
               if (GmatStringUtil::IsLastNumberPartOfName(substr))
               {
                  #if DEBUG_REMOVE_SPACES_IN_MATH_EQ > 1
                  MessageInterface::ShowMessage
                     ("   => <%c> is part of name <%s>, so deleting <%c>\n", prevCh, substr.c_str(), *pos);
                  #endif
                  str1.erase(pos);
                  lastNonBlank = distance(begin, pos);
               }
               else
                  ++pos;
            }
            else if (prevCh == '.')
               ++pos;
            else
            {
               #if DEBUG_REMOVE_SPACES_IN_MATH_EQ > 1
               MessageInterface::ShowMessage
                  ("   => previous ch <%c> is not alphanumeric or dot, so deleting <%c>\n",
                   prevCh, *pos);
               #endif
               str1.erase(pos);
               lastNonBlank = distance(begin, pos);
            }
         }
         else if (currCh == '(' || currCh == ')' || currCh == ',' ||
                  GmatStringUtil::IsMathOperator(currCh))
         {
            #if DEBUG_REMOVE_SPACES_IN_MATH_EQ > 1
            MessageInterface::ShowMessage("   ===> current ch is (), or operators\n", currCh);
            #endif
            if (prevCh == ' ')
            {
               #if DEBUG_REMOVE_SPACES_IN_MATH_EQ > 1
               MessageInterface::ShowMessage
                  ("   => previous ch is blank, so deleting <%c>\n", prevCh, *(pos-1));
               #endif
               str1.erase(pos-1);
               lastNonBlank = distance(begin, pos);
            }
            else
               ++pos;
         }
         else
            ++pos;
      }
      else // else of if (dist > 0)
         ++pos;
   }
   
   #if DEBUG_REMOVE_SPACES_IN_MATH_EQ
   MessageInterface::ShowMessage
      ("MathParser::RemoveSpaceInMathEquation() returning, str=<%s>\n\n", str1.c_str());
   #endif
   
   return str1;
}


//------------------------------------------------------------------------------
// UnsignedInt FindSubtract(const std::string &str, std::string::size_type start)
//------------------------------------------------------------------------------
std::string::size_type MathParser::FindSubtract(const std::string &str,
                                                std::string::size_type start)
{
   #if DEBUG_INVERSE_OP
   MessageInterface::ShowMessage
      ("MathParser::FindSubtract() str=%s, start=%u\n", str.c_str(), start);
   #endif
   
   std::string::size_type index2 = str.find('-', start);
   #if DEBUG_INVERSE_OP
   MessageInterface::ShowMessage("MathParser::FindSubtract() index2=%u\n", index2);
   #endif
   
   // If minus found, return it
   if (index2 != str.npos)
   {
      #if DEBUG_INVERSE_OP
      MessageInterface::ShowMessage
         ("MathParser::FindSubtract() found minus (-) returning index2=%u\n",
          index2);
      #endif
      return index2;
   }
   
   #if DEBUG_INVERSE_OP
   MessageInterface::ShowMessage
      ("MathParser::FindSubtract() returning str.size()=%u\n", str.size());
   #endif
   
   return str.size();
}


//------------------------------------------------------------------------------
// StringArray ParseAddSubtract(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseAddSubtract(const std::string &str)
{
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("MathParser::ParseAddSubtract() entered, str=%s, str.size()=%d\n",
       str.c_str(), str.size());
   #endif
   
   //-----------------------------------------------------------------
   // Operators of equal precedence evaluate from left to right.
   // 10 - 50 + 1 + 30 - 25 should produce
   // (((10 - 50) + 1) + 30) - 25
   //-----------------------------------------------------------------
   
   StringArray items;
   std::string op = "";
   std::string left;
   std::string right;

   // find last - or +
   std::string::size_type index1 = str.find_last_of('+');
   std::string::size_type index2 = str.find_last_of('-');
   
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("ParseAddSubtract() index1=%u, index2=%u\n", index1, index2);
   #endif
   
   //-------------------------------------------------------
   // no + or - found
   //-------------------------------------------------------
   if (index1 == str.npos && index2 == str.npos)
   {
      FillItems(items, "", "", "");
      #if DEBUG_ADD_SUBTRACT
      WriteItems("ParseAddSubtract(): '+' or '-' not found", items);
      #endif
      return items;
   }
   
   //-------------------------------------------------------
   // find lowest operator, expecting + or -
   //-------------------------------------------------------
   Integer index;
   std::string opStr = FindLowestOperator(str, index);
   
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("After FindLowestOperator() opStr=%s, index=%d\n",
       opStr.c_str(), index);
   #endif
   
   //-------------------------------------------------------
   // lowest operator is not + or -
   //-------------------------------------------------------
   if (opStr != "+" && opStr != "-")
   {
      // Check for scientific number
      if (GmatStringUtil::IsNumber(str))
         FillItems(items, "number", str, "");
      else
         FillItems(items, "", "", "");
      
      #if DEBUG_ADD_SUBTRACT
      WriteItems("ParseAddSubtract(): lowest op is not '+' or '-'", items);
      #endif
      return items;
   }
   
   //-------------------------------------------------------
   // If unary operator, handle it later in ParseUnary()
   //-------------------------------------------------------
   if (index2 == 0 && index1 == str.npos)
   {
      #if DEBUG_ADD_SUBTRACT
      MessageInterface::ShowMessage
         ("MathParser::ParseAddSubtract() found unary str=%s\n", str.c_str());
      #endif
      
      FillItems(items, op, left, right);
      return items;
   }
   
   
   UnsignedInt indexLeft = index;
   UnsignedInt indexRight = index + 1;
   
   bool opFound;      
   op = GetOperatorName(opStr, opFound);
   
   // if double operator +- or -+ found
   if (str[index+1] == '+' || str[index+1] == '-')
   {
      #if DEBUG_ADD_SUBTRACT
      MessageInterface::ShowMessage
         ("ParseAddSubtract() double operator found, %s\n",
          str.substr(index, 2).c_str());
      #endif
      
      if (opStr == "+" && str[index+1] == '+')
         op = "Add";
      else if (opStr == "+" && str[index+1] == '-')
         op = "Subtract";
      else if (opStr == "-" && str[index+1] == '-')
         op = "Add";
      else if (opStr == "-" && str[index+1] == '+')
         op = "Subtract";
      
      indexRight = indexRight + 1;
   }
   
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("ParseAddSubtract() indexLeft=%u, indexRight=%u, op=%s\n",
       indexLeft, indexRight, op.c_str());
   #endif
   
   left = str.substr(0, indexLeft);
   right = str.substr(indexRight, str.npos);
   
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("ParseAddSubtract() op=%s, left=%s, right=%s\n", op.c_str(),
       left.c_str(), right.c_str());
   #endif
   
   if (right == "")
      throw MathException("Need right side of \"" + op + "\"");
   
   FillItems(items, op, left, right);
   
   #if DEBUG_ADD_SUBTRACT
   WriteItems("After ParseAddSubtract()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseMultDivide(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseMultDivide(const std::string &str)
{
   #if DEBUG_MULT_DIVIDE
   MessageInterface::ShowMessage
      ("MathParser::ParseMultDivide() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";

   //-----------------------------------------------------------------
   // find last * or /
   // because we want to evaluate * or / in the order it appears
   // a * b / c * d
   //-----------------------------------------------------------------
   
   std::string::size_type index1 = str.find_last_of('*');
   std::string::size_type index2 = str.find_last_of('/');
   
   if (index1 == str.npos && index2 == str.npos)
   {
      FillItems(items, "", "", "");
      #if DEBUG_MULT_DIVIDE
      WriteItems("ParseMultDivide(): No * or / found", items);
      #endif
      return items;
   }
   
   //-------------------------------------------------------
   // find lowest operator, expecting * or /
   //-------------------------------------------------------
   Integer index;
   std::string opStr = FindLowestOperator(str, index);
   
   #if DEBUG_MULT_DIVIDE
   MessageInterface::ShowMessage
      ("after FindLowestOperator() opStr=%s, index=%d\n",
       opStr.c_str(), index);
   #endif
   
   //-------------------------------------------------------
   // lowest operator is not / or *
   //-------------------------------------------------------
   if (opStr != "/" && opStr != "*")
   {
      FillItems(items, "", "", "");
      #if DEBUG_MULT_DIVIDE
      WriteItems("ParseMultDivide(): lowest op is not '/' or '*'", items);
      #endif
      return items;
   }
   
   bool opFound;
   op = GetOperatorName(opStr, opFound);
   std::string left = str.substr(0, index);
   std::string right = str.substr(index+1, str.npos);
   
   //-------------------------------------------------------
   // find double operator *+, *-, /+, /-
   //-------------------------------------------------------
   if (str[index+1] == '+' || str[index+1] == '-')
   {
      std::string right = str.substr(index+2, str.npos);
      
      #if DEBUG_MULT_DIVIDE
      MessageInterface::ShowMessage
         ("combined operator found, %s\n", str.substr(index, 2).c_str());
      #endif
   }
   
   if (left == "")
      throw MathException("Need left side of the operator \"" + op + "\"");
   
   if (right == "")
      throw MathException("Need right side of the operator \"" + op + "\"");
   
   FillItems(items, op, left, right);

   #if DEBUG_MULT_DIVIDE
   WriteItems("After ParseMultDivide()", items);
   #endif
   
   return items;
   
}


//------------------------------------------------------------------------------
// StringArray ParsePower(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParsePower(const std::string &str)
{
   #if DEBUG_POWER
   MessageInterface::ShowMessage
      ("MathParser::ParsePower() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";
   
   // We should find last ^ insted of first ^ to fix bug 2176 (LOJ: 2010.10.29)
   std::string::size_type index1 = str.find_last_of('^');
   
   if (index1 == str.npos)
   {
      FillItems(items, "", "", "");
      return items;
   }
   
   // If there are inverseOpStr or transposeOpStr, handle it later in DecomposeMatrixOps()
   // since power, transpose, and inverse have the same operator precedence
   if (str.find(inverseOpStr) != str.npos || str.find(transposeOpStr) != str.npos)
   {
      #if DEBUG_INVERSE_OP
      MessageInterface::ShowMessage
         ("MathParser::ParsePower() found inverseOp(%s) for ^(-1) or transposeOp(%s), str=%s\n",
          inverseOpStr.c_str(), transposeOpStr.c_str(), str.c_str());
      #endif
      
      FillItems(items, "", "", "");
      return items;
   }
   
   
   // If first unary operator found, handle it later in ParseUnary()
   if (str.find("-") != str.npos)
   {
      UnsignedInt index3 = str.find("-");
      if (index3 == 0)
      {
         #if DEBUG_UNARY
         MessageInterface::ShowMessage
            ("MathParser::ParsePower() found - unary str=%s\n", str.c_str());
         #endif
      
         FillItems(items, "", "", "");
         return items;
      }
   }
   
   
   std::string::size_type index = str.npos;
   if (index1 != str.npos)
   {
      op = "Power";
      index = index1;
   }
   
   std::string left = str.substr(0, index);
   std::string right = str.substr(index+1, str.npos);

   if (left == "")
      throw MathException("Need left side of the operator \"" + op + "\"");
   
   if (right == "")
      throw MathException("Need right side of the operator \"" + op + "\"");
   
   FillItems(items, op, left, right);
   
   #if DEBUG_MATH_PARSER > 1
   WriteItems("After ParsePower()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseUnary(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseUnary(const std::string &str)
{
   #if DEBUG_UNARY
   MessageInterface::ShowMessage
      ("MathParser::ParseUnary() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";
   
   // find  - or +
   std::string::size_type index1 = str.find('-');
   std::string::size_type index2 = str.find('+');
   
   if (index1 == str.npos && index2 == str.npos)
   {
      FillItems(items, "", "", "");
      return items;
   }
   
   if (index1 != str.npos)
      op = "Negate";
   else
      op = "";
   
   std::string left;
   left = str.substr(index1+1, str.npos);
   
   FillItems(items, op, left, "");
   
   #if DEBUG_UNARY
   WriteItems("After ParseUnary()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseMathFunctions(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseMathFunctions(const std::string &str)
{
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("\nMathParser::ParseMathFunctions() entered, str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string left;
   
   // find first math function
   std::string fnName = GetFunctionName(MATH_FUNCTION, str, left);
   
   if (fnName == "")
   {
      // let's try GmatFunction name
      fnName = GetFunctionName(GMAT_FUNCTION, str, left);
      
      if (fnName == "")
      {
         FillItems(items, "", "", "");
         
         #if DEBUG_FUNCTION
         WriteItems("ParseMathFunctions() math function not found, returning", items);
         #endif
         
         return items;
      }
   }
   
   if (left == "")
      throw MathException("Need an argument of the function \"" + fnName + "\"");
   
   FillItems(items, fnName, left, "");
   
   #if DEBUG_FUNCTION > 1
   WriteItems("ParseMathFunctions() returing: ", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseMatrixOps(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseMatrixOps(const std::string &str)
{
   #if DEBUG_MATRIX_OPS
   MessageInterface::ShowMessage
      ("\nMathParser::ParseMatrixOps() entered, str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string left;
   std::string right;
   
   // find matrix function
   std::string fnName = GetFunctionName(MATRIX_FUNCTION, str, left);
   
   // Check ^ for power ^,  ' for transpose and ^(-1) for inverse
   if (fnName == "")
   {
      std::string::size_type index1 = str.find_last_of(powerOpStr);
      std::string::size_type index2 = str.find_last_of(inverseOpStr);
      std::string::size_type index3 = str.find_last_of(transposeOpStr);
      std::string::size_type index = FindOperatorIndex(index1, index2, index3);
      if (index != str.npos)
      {
         #if DEBUG_MATRIX_OPS
         MessageInterface::ShowMessage
            ("   Found %s, %s, or %s found at %u\n", powerOpStr.c_str(),
             inverseOpStr.c_str(), transposeOpStr.c_str(), index);
         #endif
         
         bool opFound = false;
         std::string opName = str.substr(index, 1);
         opName = GetOperatorName(opName, opFound);
         left = str.substr(0, index);
         right = str.substr(index+1);
         fnName = opName;
         
         #if DEBUG_MATRIX_OPS
         MessageInterface::ShowMessage
            ("   Found matrix function <%s>\n", opName.c_str());
         #endif
      }
      
      //FillItems(items, fnName, left, "");
      FillItems(items, fnName, left, right);
   }
   else // matrix function name found
   {
      FillItems(items, fnName, left, "");
   }
   
   #if DEBUG_MATRIX_OPS
   WriteItems("ParseMatrixOps() returning: ", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseUnitConversion(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseUnitConversion(const std::string &str)
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("\nMathParser::ParseUnitConversion() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string left;
   
   // find first math function
   std::string fnName = GetFunctionName(UNIT_CONVERSION, str, left);

   if (fnName == "")
      FillItems(items, "", "", "");
   else
      FillItems(items, fnName, left, "");
   
   #if DEBUG_MATH_PARSER > 1
   WriteItems("After ParseUnitConversion()", items);
   #endif
   
   return items;
}

//------------------------------------------------------------------------------
// bool IsMathElement(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Checks if string is a math element such as number, sat.X, arr(1,1).
 */
//------------------------------------------------------------------------------
bool MathParser::IsMathElement(const std::string &str)
{
   #ifdef DEBUG_MATH_ELEMENT
   MessageInterface::ShowMessage
      ("MathParser::IsMathElement() entered, str=<%s>\n", str.c_str());
   #endif
   if (str.find('\'') != str.npos) // ' for transpose
   {
      // If input is enclosed with single quote the it is a String so return true
      if (GmatStringUtil::IsEnclosedWith(str, "'"))
      {
         #ifdef DEBUG_MATH_ELEMENT
         MessageInterface::ShowMessage
            ("MathParser::IsMathElement(<%s>) returning false, string literal found\n", str.c_str());
         #endif
         //return true; // Is string allowed?
         return false; // LOJ: Changed to return false (2015.01.21)
      }
      else
      {
         #ifdef DEBUG_MATH_ELEMENT
         MessageInterface::ShowMessage
            ("MathParser::IsMathElement(<%s>) returning false, transpose ' found\n", str.c_str());
         #endif
         return false;
      }
   }
   else if (str.find(inverseOpStr) != str.npos)
   {
      #ifdef DEBUG_MATH_ELEMENT
      MessageInterface::ShowMessage
         ("MathParser::IsMathElement(<%s>) returning false, invere operator <%s> found\n",
          str.c_str(), inverseOpStr.c_str());
      #endif
      return false;
   }
   else if (GmatStringUtil::IsThereMathSymbol(str))
   {
      // Check for unary + or - signs
      if (str.find_first_of("+-") != str.npos)
      {
         Real rval;
         Integer errCode;
         if (GmatStringUtil::IsValidReal(str, rval, errCode))
         {
            #ifdef DEBUG_MATH_ELEMENT
            MessageInterface::ShowMessage
               ("MathParser::IsMathElement(<%s>) returning true, it is a number\n", str.c_str());
            #endif
            return true;
         }
      }
      
      #ifdef DEBUG_MATH_ELEMENT
      MessageInterface::ShowMessage
         ("MathParser::IsMathElement(<%s>) returning false, math operator found\n", str.c_str());
      #endif
      return false;
   }
   else if (str.find_first_of("()") != str.npos)
   {
      if (GmatStringUtil::IsParenPartOfArray(str))
      {
         if (IsParenPartOfFunction(str))
         {
            #ifdef DEBUG_MATH_ELEMENT
            MessageInterface::ShowMessage
               ("MathParser::IsMathElement(<%s>) returning false, it is a function call\n", str.c_str());
            #endif
            return false;
         }
         else
         {
            // Check for unary operator
            if (str.find_first_of("+-") != str.npos)
            {
               #ifdef DEBUG_MATH_ELEMENT
               MessageInterface::ShowMessage
                  ("MathParser::IsMathElement(<%s>) returning false, it has unary + or -\n", str.c_str());
               #endif
               return false;
            }
            else
            {
               #ifdef DEBUG_MATH_ELEMENT
               MessageInterface::ShowMessage
                  ("MathParser::IsMathElement(<%s>) returning true, it is an array "
                   "element or Parameter\n", str.c_str());
               #endif
               return true;
            }
         }
      }
      else
      {
         #ifdef DEBUG_MATH_ELEMENT
         MessageInterface::ShowMessage
            ("MathParser::IsMathElement(<%s>) returning false, paren is not part of an array\n", str.c_str());
         #endif
         return false;
      }
   }
   else if (IsFunctionWithOptionalInput(str))
   {
      return false;
   }
   else
   {
      #ifdef DEBUG_MATH_ELEMENT
      MessageInterface::ShowMessage
         ("MathParser::IsMathElement(<%s>) returning true\n", str.c_str());
      #endif
      return true;
   }
}

//------------------------------------------------------------------------------
// bool IsMathFunction(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Tests if input string is any of built-in math functions
 */
//------------------------------------------------------------------------------
bool MathParser::IsMathFunction(const std::string &str)
{
   if (HasFunctionName(str, realFuncList) ||
       HasFunctionName(str, matrixFuncList) ||
       HasFunctionName(str, unitConvList))
      return true;
   
   return false;
}


//------------------------------------------------------------------------------
// bool HasFunctionName(const std::string &str, const StringArray &fnList)
//------------------------------------------------------------------------------
bool MathParser::HasFunctionName(const std::string &str, const StringArray &fnList)
{
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::HasFunctionName() str=%s\n", str.c_str());
   #endif
   
   // Find name from the input function list as is
   if (find(fnList.begin(), fnList.end(), str) != fnList.end())
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::HasFunctionName() returning true\n");
      #endif
      return true;
   }
   
   // Try lower the first letter and find
   std::string str1 = GmatStringUtil::ToLower(str, true);
   if (find(fnList.begin(), fnList.end(), str1) != fnList.end())
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::HasFunctionName() returning true\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::HasFunctionName() returning false\n");
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// bool IsParenPartOfFunction(const std::string &str)
//------------------------------------------------------------------------------
bool MathParser::IsParenPartOfFunction(const std::string &str)
{
   // Check function name in the GmatFunction list first (loj: 2008.08.20)
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::IsParenPartOfFunction() checking for function name '%s'\n",
       str.c_str());
   #endif
   
   std::string str1 = str;
   if (str.find_first_of('(') != str.npos)
   {
      std::string::size_type openParen = str.find_first_of('(');
      str1 = str.substr(0, openParen);
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage("   Checking GmatFunction list...\n");
   #endif
   if (HasFunctionName(str1, gmatFuncList))
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::IsParenPartOfFunction() returning true, GmatFunction found\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage("   Checking internal MathFunction list...\n");
   #endif
   if (HasFunctionName(str1, realFuncList))
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::IsParenPartOfFunction() returning true, MathFunction found\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage("   Checking internal MatrixFunction list...\n");
   #endif
   if (HasFunctionName(str1, matrixFuncList))
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::IsParenPartOfFunction() returning true, MatrixFunction found\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage("   Checking internal UnitConversionFunction list...\n");
   #endif
   if (HasFunctionName(str1, unitConvList))
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::IsParenPartOfFunction() returning true, UnitConversionFunction found\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage("   Checking internal StringFunction list...\n");
   #endif
   if (HasFunctionName(str1, builtinFuncList))
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::IsParenPartOfFunction() returning true, StringFunction found\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::IsParenPartOfFunction() returning false, no function found\n");
   #endif
   return false;
}


//------------------------------------------------------------------------------
// bool IsGmatFunction(const std::string &name)
//------------------------------------------------------------------------------
bool MathParser::IsGmatFunction(const std::string &name)
{
   // if name has open parenthesis, get it up to
   std::string name1 = name;
   std::string::size_type index = name1.find("(");
   if (index != name1.npos)
      name1 = name1.substr(0, index);
   
   for (int i=0; i<theGmatFuncCount; i++)
      if (name1 == gmatFuncList[i])
         return true;
   
   return false;
}


//------------------------------------------------------------------------------
// bool IsValidOperator(const std::string &str)
//------------------------------------------------------------------------------
bool MathParser::IsValidOperator(const std::string &str)
{
   char op = str[0];
   
   if (op == '+' || op == '-' || op == '*' || op == '/' || op == '^' ||
       op == transposeOp || op == inverseOp)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// std::string GetFunctionName(UnsignedInt functionType, const std::string &str,
//                             std::string &left)
//------------------------------------------------------------------------------
std::string MathParser::GetFunctionName(UnsignedInt functionType,
                                        const std::string &str,
                                        std::string &left)
{
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::GetFunctionName() entered, functionType=%u, str='%s'\n",
       functionType, str.c_str());
   #endif

   left = "";
   std::string fnName = "";
   
   // if string does not start with letter, just return
   if (!isalpha(str[0]))
   {
      // Remove extra parenthesis and try again
      std::string str1 = GmatStringUtil::RemoveExtraParen(str, true);
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::GetFunctionName() After removing extra parenthesis, str1=<%s>\n",
          str1.c_str());
      #endif
      
      if (!isalpha(str1[0]))
      {
         #if DEBUG_FUNCTION
         MessageInterface::ShowMessage
            ("MathParser::GetFunctionName() leaving, fnName='%s', left='%s'\n",
             fnName.c_str(), left.c_str());
         #endif
         return fnName;
      }
   }
   
   switch (functionType)
   {
   case ANY_FUNCTION:
      {
         BuildFunction(str, realFuncList, fnName, left);
         if (fnName != "")
            break;
         BuildFunction(str, matrixFuncList, fnName, left);
         if (fnName != "")
            break;
         BuildFunction(str, unitConvList, fnName, left);
         if (fnName != "")
            break;
         BuildFunction(str, builtinFuncList, fnName, left);
         if (fnName != "")
            break;
         BuildFunction(str, gmatFuncList, fnName, left);
         break;
      }
   case MATH_FUNCTION:
      {
         BuildFunction(str, realFuncList, fnName, left);
         break;
      }
   case MATRIX_FUNCTION:
      {
         BuildFunction(str, matrixFuncList, fnName, left);
         break;
      }
   case UNIT_CONVERSION:
      {
         BuildFunction(str, unitConvList, fnName, left);
         break;
      }
   case BUILTIN_FUNCTION:
      {
         BuildFunction(str, builtinFuncList, fnName, left);
         break;
      }
   case GMAT_FUNCTION:
      {
         BuildFunction(str, gmatFuncList, fnName, left);
         break;
      }
   default:
      break;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::GetFunctionName() leaving, fnName='%s', left='%s'\n",
       fnName.c_str(), left.c_str());
   #endif
   
   return fnName;
}


//------------------------------------------------------------------------------
// void BuildGmatFunctionList(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Builds GmatFunction list found in the GmatFunction path.
 */
//------------------------------------------------------------------------------
void MathParser::BuildGmatFunctionList(const std::string &str)
{
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildGmatFunctionList() entered, str='%s'\n", str.c_str());
   #endif
   
   StringArray names = GmatStringUtil::GetVarNames(str);
   FileManager *fm = FileManager::Instance();
   
   for (UnsignedInt i=0; i<names.size(); i++)
   {
      #if DEBUG_GMAT_FUNCTION > 1
      MessageInterface::ShowMessage
         ("   BuildGmatFunctionList() checking '%s'\n", names[i].c_str());
      #endif
      
      if (fm->GetGmatFunctionPath(names[i]) != "")
      {         
         // if GmatFunction not registered, add to the list
         if (find(gmatFuncList.begin(), gmatFuncList.end(), names[i]) == gmatFuncList.end())
            gmatFuncList.push_back(names[i]);
      }
   }
   
   theGmatFuncCount = gmatFuncList.size();
   
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildGmatFunctionList() found total of %d GmatFunction(s)\n",
       theGmatFuncCount);
   for (Integer i=0; i<theGmatFuncCount; i++)
      MessageInterface::ShowMessage("   '%s'\n", gmatFuncList[i].c_str());
   #endif
}


//------------------------------------------------------------------------------
// void BuildFunction(const std::string &str, const StringArray &fnList,
//                    std::string &fnName, std::string &left)
//------------------------------------------------------------------------------
void MathParser::BuildFunction(const std::string &str, const StringArray &fnList,
                               std::string &fnName, std::string &left)
{
   UnsignedInt count = fnList.size();
   fnName = "";
   left = "";
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildFunction() entered, str='%s', number of functions=%d\n",
       str.c_str(), count);
   #endif
   
   if (count == 0)
      return;
   
   std::string::size_type functionIndex = str.npos;
   std::string str1 = GmatStringUtil::Trim(str);
   // Check if function name is in the function list
   //std::string fname = GmatStringUtil::ParseFunctionName(str);
   std::string fnArg;
   std::string fname = GmatStringUtil::ParseFunctionName(str1, fnArg);
   
   #if DEBUG_FUNCTION > 1
   MessageInterface::ShowMessage("   fname = '%s'\n", fname.c_str());
   #endif
   
   if (find(fnList.begin(), fnList.end(), fname) != fnList.end())
   {
      //fnName = fname;
      std::string nameToCheck = fname;
      std::string::size_type openParen = str1.find_first_of('(');
      if (openParen != fname.npos)
      {
         // Check if there are only spaces between function name and open parenthesis
         std::string str2 = str.substr(fname.size(), openParen-fname.size());
         str2 = GmatStringUtil::RemoveAllBlanks(str2);
         #if DEBUG_FUNCTION > 1
         MessageInterface::ShowMessage("   OpenParen found at %u\n", openParen);
         MessageInterface::ShowMessage("   After removing blanks, str2 = '%s'\n", str2.c_str());
         #endif
         if (str2 == "")
         {
            fnName = fname;
            //functionIndex = str.find(fname + "(");
            functionIndex = str.find(nameToCheck);
         }
      }
   }
   else
   {
      // Let's try lower case of the first letter
      if (isalpha(fname[0]) && isupper(fname[0]))
      {
         // MSVC++ failes to do: fname[0] = tolower(fname[0])
         std::string fname1 = fname;
         fname1[0] = tolower(fname[0]);
         
         #if DEBUG_FUNCTION > 1
         MessageInterface::ShowMessage
            ("   function name '%s' not found so trying lower case '%s'\n",
             fname.c_str(), fname1.c_str());
         #endif
         
         if (find(fnList.begin(), fnList.end(), fname1) != fnList.end())
         {
            fnName = fname1;
            functionIndex = str.find(fname + "(");
         }
      }
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildFunction() fnName='%s', functionIndex=%u\n",
       fnName.c_str(), functionIndex);
   #endif
   
   if (fnName != "")
   {
      std::string::size_type index1 = str.find("(", functionIndex);
      
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::BuildFunction() calling FindMatchingParen() with start "
          "index=%u\n", index1);
      #endif
      
      UnsignedInt index2 = FindMatchingParen(str, index1);
      
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::BuildFunction() matching ) found at %u\n", index2);
      #endif
      
      // Check for invalid operators after function (LOJ: 2012.01.19)
      if (index2 != str.npos)
      {
         #if DEBUG_FUNCTION
         MessageInterface::ShowMessage
         ("   now checking for invalid operators after function: '%s'\n", fnName.c_str());
         #endif
         // Remove blanks after matching parenthesis before checking.
         // I thought blanks are already removed by the time it gets here.
         // Anyway look into it later (LOJ: 2012.01.19) -
         // Blanks are not removed if it is called from IsEquation()
         std::string str1 = GmatStringUtil::RemoveAll(str, ' ', index2);
         std::string nextOp = str1.substr(index2+1, 1);
         #if DEBUG_FUNCTION
         MessageInterface::ShowMessage
            ("   str1='%s', nextOp='%s'\n", str1.c_str(), nextOp.c_str());
         #endif
         if (nextOp != "" && !IsValidOperator(nextOp))
            throw MathException("Invalid math operator \"" + nextOp + "\" found");
      }
      
      left = str.substr(index1+1, index2-index1-1);
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildFunction() leaving, fnName=%s, left=%s\n",
       fnName.c_str(), left.c_str());
   #endif
}


//------------------------------------------------------------------------------
// std::string::size_type FindMatchingParen(const std::string &str, size_type start)
//------------------------------------------------------------------------------
std::string::size_type MathParser::FindMatchingParen(const std::string &str,
                                                     std::string::size_type start)
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::FindMatchingParen() entered, str='%s', str.size()=%u, start=%u\n",
       str.c_str(), str.size(), start);
   #endif
   
   int leftCounter = 0;
   int rightCounter = 0;
   
   for (UnsignedInt i = start; i < str.size(); i++)
   {
      if (str[i] == '(')
         leftCounter++;

      if (str[i] == ')')
         rightCounter++;
      
      if (leftCounter == rightCounter)
         return i;
   }
   
   #if DEBUG_MATH_PARSER
   MessageInterface::ShowMessage
      ("**** ERROR ****  MathParser::FindMatchingParen() Unmatching parenthesis found\n");
   #endif
   
   throw MathException("Unmatching parenthesis found");
}


//------------------------------------------------------------------------------
// void FillItems(StringArray &items, const std::string &op,
//                const std::string &left, const std::string &right)
//------------------------------------------------------------------------------
void MathParser::FillItems(StringArray &items, const std::string &op,
                           const std::string &left, const std::string &right)
{
   items.push_back(op);
   items.push_back(left);
   items.push_back(right);
}


//------------------------------------------------------------------------------
// void WriteItems(const std::string &msg, StringArray &items, bool addEol = false)
//------------------------------------------------------------------------------
void MathParser::WriteItems(const std::string &msg, StringArray &items, bool addEol)
{
   MessageInterface::ShowMessage
      ("%s items = <%s> <%s> <%s>\n", msg.c_str(),
       items[0].c_str(), items[1].c_str(), items[2].c_str());
   if (addEol)
      MessageInterface::ShowMessage("\n");
}


//------------------------------------------------------------------------------
// void WriteNode(MathNode *node, UnsignedInt level)
//------------------------------------------------------------------------------
void MathParser::WriteNode(MathNode *node, UnsignedInt level)
{
#if DEBUG_PARSE
   
   if (node == NULL)
      return;

   level++;

   if (!node->IsFunction())
   {
      for (UnsignedInt i=0; i<level; i++)
         MessageInterface::ShowMessage("....");
      
      MessageInterface::ShowMessage
         ("node=%s: %s\n", node->GetTypeName().c_str(), node->GetName().c_str());
   }
   else
   {
      for (UnsignedInt i=0; i<level; i++)
         MessageInterface::ShowMessage("....");

      if (node->IsOfType("FunctionRunner"))
         MessageInterface::ShowMessage
            ("node=%s: %s\n", node->GetTypeName().c_str(), node->GetName().c_str());
      else
         MessageInterface::ShowMessage
            ("node=%s: %s\n", node->GetTypeName().c_str(), node->GetName().c_str());
      
      if (node->GetLeft())
      {
         for (UnsignedInt i=0; i<level; i++)
            MessageInterface::ShowMessage("....");
         
         MessageInterface::ShowMessage
            ("left=%s: %s\n", node->GetLeft()->GetTypeName().c_str(),
             node->GetLeft()->GetName().c_str());
         
         WriteNode(node->GetLeft(), level);
      }
      
      
      if (node->GetRight())
      {
         for (UnsignedInt i=0; i<level; i++)
            MessageInterface::ShowMessage("....");
         
         MessageInterface::ShowMessage
            ("right=%s: %s\n", node->GetRight()->GetTypeName().c_str(),
             node->GetRight()->GetName().c_str());
         
         WriteNode(node->GetRight(), level);
      }
   }
#endif
}


//------------------------------------------------------------------------------
// void BuildAllFunctionList()
//------------------------------------------------------------------------------
/*
 * Builds function list.
 *
 * @note
 * We cannot use the list from the Moderator::GetListOfFactoryItems(Gmat::MATH_TREE)
 * since it needs to be grouped into the order of parsing.
 *    ParseParenthesis()
 *    ParseAddSubtract()
 *    ParseMultDivide()
 *    ParsePower()
 *    ParseUnary()
 *    ParseMatrixOps()
 *    ParseMathFunctions()
 *    ParseUnitConversion()
 *
 */
//------------------------------------------------------------------------------
void MathParser::BuildAllFunctionList()
{
   // Why power (^) is not here? 
   // It is not here since it is one of basic math operator such as +, -, *, / (LOJ: 2012.01.17)
   
   // Functions for real number
   realFuncList.push_back("abs");
   realFuncList.push_back("acos");
   realFuncList.push_back("asin");
   realFuncList.push_back("atan");
   realFuncList.push_back("atan2");
   realFuncList.push_back("ceil");
   realFuncList.push_back("cos");
   realFuncList.push_back("exp");
   realFuncList.push_back("fix");
   realFuncList.push_back("floor");
   realFuncList.push_back("log");
   realFuncList.push_back("log10");
   realFuncList.push_back("sin");
   realFuncList.push_back("tan");
   realFuncList.push_back("sqrt");
   
   // Functions for matrix and vector
   matrixFuncList.push_back("det");
   matrixFuncList.push_back("inv");
   matrixFuncList.push_back("norm");
   matrixFuncList.push_back("rand");
   matrixFuncList.push_back("randn");
   matrixFuncList.push_back("transpose");
   
   // Unit Conversion functions
   unitConvList.push_back("deg2Rad");
   unitConvList.push_back("degToRad");
   unitConvList.push_back("rad2Deg");
   unitConvList.push_back("radToDeg");
   
   // Builtin functions taking more than 1 input argument with Real or String
   // cannot be used with operators
   builtinFuncList.push_back("cross");
   builtinFuncList.push_back("diag");
   builtinFuncList.push_back("min");
   builtinFuncList.push_back("mod");
   builtinFuncList.push_back("setSeed");
   builtinFuncList.push_back("sprintf");
   builtinFuncList.push_back("strcat");
   builtinFuncList.push_back("strcmp");
   builtinFuncList.push_back("strfind");
   builtinFuncList.push_back("strrep");
   builtinFuncList.push_back("strlen");
   builtinFuncList.push_back("substr");
   
   // Matrix Operators
   matrixOpList.push_back("'");      // transpose()
   matrixOpList.push_back("^(-1)");  // inv()
   
}

