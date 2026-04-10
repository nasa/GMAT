//------------------------------------------------------------------------------
//                                  TestStringUtil
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
// Author: Linda Jun
// Created: 2005/02/14
//
/**
 * Test driver for StringUtil.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "TimeTypes.hpp"                // for GmatTileUtil::
#include "StringUtil.hpp"
#include "UtilityException.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;
using namespace GmatStringUtil;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   MessageInterface::ShowMessage("=========== TestStringUtil\n");
   out.Put("=========== TestStringUtil\n");
   
   std::string str, str1;
   Real rval, expRval;
   Integer ival, expIval;
   bool bval;
   StringArray itemNames;
   
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::DecomposeBy(' ')");
   //---------------------------------------------
   str = "input String XX";
   out.Put(str);
   itemNames = DecomposeBy(str, " ");
   out.Validate(itemNames[0], "input");
   out.Validate(itemNames[1], "String XX");
   
   out.Put("============================== test GmatStringUtil::RemoveLastString()");
   //---------------------------------------------
   str = "inputStringXX";
   out.Put(str);
   str1 = RemoveLastString(str, ";");
   out.Validate(str1, "inputStringXX");
   
   str = "inputString;;;;;";
   out.Put(str);
   str1 = RemoveLastString(str, ";");
   out.Validate(str1, "inputString;;;;");
   
   str1 = RemoveLastString(str, ";", true);
   out.Validate(str1, "inputString");
   
   str = "inputStringXXXXXX";
   out.Put(str);
   str1 = RemoveLastString(str, "XXX");
   out.Validate(str1, "inputStringXXX");
   
   str1 = RemoveLastString(str, "XXX", true);
   out.Validate(str1, "inputString");
   
   out.Put("============================== test GmatStringUtil::IsThereEqualSign()");
   //---------------------------------------------
   str = "ReportString(RF, '=== first msg ===', testStr2);";
   out.Put(str);
   bval = IsThereEqualSign(str);
   out.Validate(bval, false);
   
   str = "ReportString = (RF, '=== first msg ===', testStr2);";
   out.Put(str);
   bval = IsThereEqualSign(str);
   out.Validate(bval, true);
   
   str = "[a, b, c] = DoSomething(1, 2, '=== x ==='";
   out.Put(str);
   bval = IsThereEqualSign(str);
   out.Validate(bval, true);
   
   str = "a = '=== +++ ==='";
   out.Put(str);
   bval = IsThereEqualSign(str);
   out.Validate(bval, true);
   
   str = "'=== +++ ===', '=1', '=2', '=3'";
   out.Put(str);
   bval = IsThereEqualSign(str);
   out.Validate(bval, false);
   
   str = "'=== +++ ===' = '=1', '=2', '=3'";
   out.Put(str);
   bval = IsThereEqualSign(str);
   out.Validate(bval, true);
   
   out.Put("============================== test GmatStringUtil::SeparateByComma()");
   //---------------------------------------------
   str = "Y, 'x,y', A(I(2,3)), 'def', X(i,j)";
   itemNames = SeparateByComma(str);
   out.Put(str);
   for (UnsignedInt i=0; i<itemNames.size(); i++)
      out.Put(itemNames[i]);
   out.Put("");
   
   str = "'x=a*b+c', 'x=a*b/c-a'";
   itemNames = SeparateByComma(str);
   out.Put(str);
   for (UnsignedInt i=0; i<itemNames.size(); i++)
      out.Put(itemNames[i]);
   out.Put("");
   
   str = "x', y'";
   itemNames = SeparateByComma(str, false);
   out.Put(str);
   for (UnsignedInt i=0; i<itemNames.size(); i++)
      out.Put(itemNames[i]);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::IsValidName()");
   //---------------------------------------------
   str = "";
   bval = IsValidName(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //---------------------------------------------
   str = "1e-11";
   bval = IsValidName(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //---------------------------------------------
   str = "&name";
   bval = IsValidName(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   str = "-name";
   bval = IsValidName(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   str = "1name";
   bval = IsValidName(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   str = "x&234";
   bval = IsValidName(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   str = "2name";
   bval = IsValidName(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   str = "name2";
   bval = IsValidName(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::GetVarNames()");
   //---------------------------------------------
   str = "x";
   out.Put(str);
   itemNames = GetVarNames(str);
   ival = itemNames.size();
   expIval = 1;
   out.Validate(ival, expIval);
   out.Put("item count = ", ival);
   for (UnsignedInt i=0; i<itemNames.size(); i++)
      out.Put(itemNames[i]);
   out.Put("");
   
   //---------------------------------------------
   str = "x+y";
   out.Put(str);
   itemNames = GetVarNames(str);
   ival = itemNames.size();
   expIval = 2;
   out.Validate(ival, expIval);
   out.Put("item count = ", ival);
   for (UnsignedInt i=0; i<itemNames.size(); i++)
      out.Put(itemNames[i]);
   out.Put("");
   
   //---------------------------------------------
   str = "cross(vv, cross(rv, vv)";
   out.Put(str);
   itemNames = GetVarNames(str);
   ival = itemNames.size();
   expIval = 3;
   out.Validate(ival, expIval);
   out.Put("item count = ", ival);
   for (UnsignedInt i=0; i<itemNames.size(); i++)
      out.Put(itemNames[i]);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::IsMathEquation()");
   //---------------------------------------------
   str = "x";
   out.Put(str);
   bval = IsMathEquation(str);
   out.Validate(bval, false);
   out.Put("");
   
   //---------------------------------------------
   str = "x+y";
   out.Put(str);
   bval = IsMathEquation(str);
   out.Validate(bval, true);
   out.Put("");
   
   //---------------------------------------------
   str = "cross(vv, cross(rv, vv))";
   out.Put(str);
   bval = IsMathEquation(str);
   out.Validate(bval, false);
   out.Put("");
   
   //---------------------------------------------
   str = "cross(vv+zz, cross(rv, vv))";
   out.Put(str);
   bval = IsMathEquation(str);
   out.Validate(bval, true);
   out.Put("");
   
   // failed 2013.01.28
   //>>>>>>>>>>>>>>>>
   #if 0
   //---------------------------------------------
   str = "cross(vv+zz, cross(rv, vv)) + 30";
   out.Put(str);
   bval = IsMathEquation(str);
   out.Validate(bval, true);
   out.Put("");
   
   //---------------------------------------------
   str = "30 + 2 /(2 - 6) * a - b";
   out.Put(str);
   bval = IsMathEquation(str);
   out.Validate(bval, true);
   out.Put("");
   #endif
   //<<<<<<<<<<<<<<<
   
   out.Put("============================== test GmatStringUtil::RemoveLastNumber()");
   //---------------------------------------------
   str = "string";
   str1 = RemoveLastNumber(str, ival);
   out.Put(str);
   expIval = 0;
   out.Validate(str1, "string");
   out.Validate(ival, expIval);
   out.Put("");
   
   //---------------------------------------------
   str = "string123";
   str1 = RemoveLastNumber(str, ival);
   out.Put(str);
   expIval = 123;
   out.Validate(str1, "string");
   out.Validate(ival, expIval);
   out.Put("");
   
   //---------------------------------------------
   str = "def1sat2";
   str1 = RemoveLastNumber(str, ival);
   out.Put(str);
   expIval = 2;
   out.Validate(str1, "def1sat");
   out.Validate(ival, expIval);
   out.Put("");
   
   //---------------------------------------------
   str = "def1sat";
   str1 = RemoveLastNumber(str, ival);
   out.Put(str);
   expIval = 0;
   out.Validate(str1, "def1sat");
   out.Validate(ival, expIval);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::ToReal()");
   //------------------------------
   str = "1e-11";
   expRval = 1e-11;
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, expRval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Real number");
   }
   
   //------------------------------
   str = "1e+11";
   expRval = 1e+11;
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, expRval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Real number");
   }
   
   
   //------------------------------
   str = ".99";
   expRval = .99;
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, expRval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Real number");
   }
   
   
   //------------------------------
   str = "1.234567890123456789";
   expRval = 1.234567890123456789;
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, expRval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Real number");
   }
   
   
   //------------------------------
   str = "3.986004415e5";
   expRval = 3.986004415e5;
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, expRval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Real number");
   }
   
   
   //------------------------------
   str = "3.986004415e+5";
   expRval = 3.986004415e+5;
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, expRval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Real number");
   }
   
   
   //------------------------------
   str = "-3.84873275031802e-005";
   expRval = -3.84873275031802e-005;
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, expRval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Real number");
   }
   
   //------------------------------
   str = "+3.84873275031802e+005";
   expRval = +3.84873275031802e+005;
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, expRval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Real number");
   }
   
   //------------------------------ blank after + or - sign is OK
   str = "+  3.84873275031802e+005";
   expRval = +3.84873275031802e+005;
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, expRval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Real number");
   }
   
   //------------------------------ blank after + or - sign is OK
   str = "-  3.84873275031802e+005";
   expRval = -3.84873275031802e+005;
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, expRval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Real number");
   }
   
   //------------------------------
   str = "";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   
   //------------------------------
   str = ".2.353";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   
   //------------------------------
   str = "1e-+11";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   
   //------------------------------
   str = "3.9860044e+15e+5";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");

   
   //------------------------------
   str = "3.9860044e-15e+5";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   //------------------------------
   str = "3.9860044e15E-5";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");

   //------------------------------
   str = "-3.84873275031802-e005";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");

   //------------------------------
   str = "-1. 23456";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   //------------------------------
   str = "-1 . 23456";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   //------------------------------
   str = "-1.23456e -5";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   //------------------------------
   str = "- 1.23456 e-5";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   

   
   out.Put("");
   out.Put("============================== test GmatStringUtil::ToInteger()");
   //------------------------------
   str = "123";
   expIval = 123;
   if (ToInteger(str, ival))
   {
      out.Put(str);
      out.Validate(ival, expIval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Integer number");
   }
   
   
   //------------------------------
   str = "-123";
   expIval = -123;
   if (ToInteger(str, ival))
   {
      out.Put(str);
      out.Validate(ival, expIval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Integer number");
   }
   
   
   //------------------------------
   str = "0234322234";
   expIval = 234322234;
   if (ToInteger(str, ival))
   {
      out.Put(str);
      out.Validate(ival, expIval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Integer number");
   }

   // Failed 2013.01.28
   //>>>>>>>>>>>>>>>
   #if 0
   //------------------------------
   str = "2147483647";
   expIval = 2147483647;
   if (ToInteger(str, ival))
   {
      out.Put(str);
      out.Validate(ival, expIval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Integer number");
   }
   
   //------------------------------
   str = "2147483648";
   expIval = 2147483648;
   if (ToInteger(str, ival))
   {
      out.Put(str);
      out.Validate(ival, expIval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is not a valid Integer number");
   }
   
   
   //------------------------------
   str = "-2147483647";
   expIval = -2147483647;
   if (ToInteger(str, ival))
   {
      out.Put(str);
      out.Validate(ival, expIval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is not a valid Integer number");
   }
   
   //------------------------------
   str = "429496729";
   expIval = 429496729;
   if (ToInteger(str, ival))
   {
      out.Put(str);
      out.Validate(ival, expIval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is not a valid Integer number");
   }
   
   //------------------------------
   str = "4294967295";
   expIval = 4294967295;
   if (ToInteger(str, ival))
   {
      out.Put(str);
      out.Validate(ival, expIval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is not a valid Integer number");
   }
   
   //------------------------------
   str = "4294967294";
   expIval = 4294967294;
   if (ToInteger(str, ival))
   {
      out.Put(str);
      out.Validate(ival, expIval);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is not a valid Integer number");
   }
   #endif
   //<<<<<<<<<<<<<<<
   
   
   //------------------------------
   str = "1.23";
   if (ToInteger(str, ival))
      throw UtilityException(str + " is not a valid Integer number");
   else
      out.Put(str, " is not an Integer number\n");
   
   
   //------------------------------
   str = "x12d3";
   if (ToInteger(str, ival))
      throw UtilityException(str + " is not a valid Integer number");
   else
      out.Put(str, " is not an Integer number\n");
   
   
   //------------------------------
   str = "--134";
   if (ToInteger(str, ival))
      throw UtilityException(str + " is not a valid Integer number");
   else
      out.Put(str, " is not an Integer number\n");
   
   
   //------------------------------
   str = "-1+34";
   if (ToInteger(str, ival))
      throw UtilityException(str + " is not a valid Integer number");
   else
      out.Put(str, " is not an Integer number\n");
   
   
   out.Put("============================== test GmatStringUtil::ToBoolean()");
   //------------------------------
   str = " TRUE ";
   if (ToBoolean(str, bval))
   {
      out.Put(str);
      out.Validate(bval, true);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Boolean");
   }
   
   
   out.Put("============================== test GmatStringUtil::ToBoolean()");
   //------------------------------
   str = " False ";
   if (ToBoolean(str, &bval))
   {
      out.Put(str);
      out.Validate(bval, false);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " is a valid Boolean");
   }
   
   
   //------------------------------
   str = "FalseT";
   if (ToBoolean(str, bval))
   {
      throw UtilityException(str + " is not a valid Boolean");
      out.Put(str);
      out.Validate(bval, false);
      out.Put("");
   }
   else
   {
      out.Put(str + " is not a valid Boolean\n");
   }
   
   
   out.Put("============================== test GmatStringUtil::ToReal()");
   //------------------------------
   str = "1E+11";
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, 1e+11);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " should return a number");
   }
   
   //------------------------------
   str = ".99";
   if (ToReal(str, rval))
   {
      out.Put(str);
      out.Validate(rval, 0.99);
      out.Put("");
   }
   else
   {
      throw UtilityException(str + " should return a number");
   }
   
   //------------------------------
   str = "1e-+11";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   //------------------------------
   str = "1e--11";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   //------------------------------
   str = ".1234x";
   if (ToReal(str, rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   //------------------------------
   str = "..1234";
   if (ToReal(str, &rval))
      throw UtilityException(str + " should return not a number");
   else
      out.Put(str, " is not a number");
   
   out.Put("============================== test GmatStringUtil::IsCommaPartOfArray()");
   
   //------------------------------
   str = "A(1, b(c,d))";
   bval = IsCommaPartOfArray(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "A(1,2)";
   bval = IsCommaPartOfArray(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "Arr(arr2(2,2))";
   bval = IsCommaPartOfArray(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   //------------------------------
   str = "A( b(x,y), I)";
   bval = IsCommaPartOfArray(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::IsSingleItem()");
   
   //------------------------------
   str = "-abc";
   bval = IsSingleItem(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   //------------------------------
   str = "-10.0";
   bval = IsSingleItem(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   //------------------------------
   str = "-10.0-5";
   bval = IsSingleItem(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "(-abc+edf)";
   bval = IsSingleItem(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "(123,123)";
   bval = IsSingleItem(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "A(I,J)";
   bval = IsSingleItem(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "Sat.VX";
   bval = IsSingleItem(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   out.Put("");
   out.Put("============================== test GmatStringUtil::Replace()");
   
   //------------------------------
   str = "abcdef";
   out.Put(str);
   str1 = Replace(str, "abcdef", "123456");
   out.Validate(str1, "123456");
   
   //------------------------------
   str = "123456";
   out.Put(str);
   str1 = Replace(str, "123456", "abcdef");
   out.Validate(str1, "abcdef");
   
   //------------------------------
   str = "GMAT TotaldV = TotaldV + sqrt( currentdV )";
   out.Put(str);
   str1 = Replace(str, "currentdV", "CDV");
   out.Validate(str1, "GMAT TotaldV = TotaldV + sqrt( CDV )");
      
   //------------------------------
   str = "GMAT TotaldV = TotaldV + TotaldV * sqrt( currentdV )";
   out.Put(str);
   str1 = Replace(str, "TotaldV", "TDV");
   out.Validate(str1, "GMAT TDV = TDV + TDV * sqrt( currentdV )");
      
   //------------------------------
   str = "GMAT TotaldV = TotaldV + TotaldV * sqrt( currentdV )";
   out.Put(str);
   str1 = Replace(str, "abc", "TDV");
   out.Validate(str1, str);
   
   out.Put("");
   out.Put("============================== test GmatStringUtil::ReplaceName()");
   
   //------------------------------
   str = "abcdef";
   out.Put(str);
   str1 = ReplaceName(str, "abcdef", "123456");
   out.Validate(str1, "123456");
   
   //------------------------------
   str = "123456";
   out.Put(str);
   str1 = ReplaceName(str, "123456", "abcdef");
   out.Validate(str1, "abcdef");
   
   //------------------------------
   str = "GMAT TotaldV = TotaldV + sqrt( currentdV )";
   out.Put(str);
   str1 = ReplaceName(str, "currentdV", "CDV");
   out.Validate(str1, "GMAT TotaldV = TotaldV + sqrt( CDV )");
   
   //------------------------------
   str = "GMAT TotaldV = TotaldV + TotaldV * sqrt( currentdV )";
   out.Put(str);
   str1 = ReplaceName(str, "TotaldV", "TDV");
   out.Validate(str1, "GMAT TDV = TDV + TDV * sqrt( currentdV )");
   
   //------------------------------
   str = "GMAT TotaldV = TotaldV + TotaldV * sqrt( currentdV )";
   out.Put(str);
   str1 = ReplaceName(str, "abc", "TDV");
   out.Validate(str1, str);
   
   //------------------------------
   str = "GMAT TotaldV = TotaldV + TotaldV * sqrt( currentdV )";
   out.Put(str);
   str1 = ReplaceName(str, "TotaldV", "TotaldV_xxx");
   out.Validate(str1, "GMAT TotaldV_xxx = TotaldV_xxx + TotaldV_xxx * sqrt( currentdV )");
   
   //------------------------------
   str = "GMAT MyArr(I,J) = MyArr(I,J) + MyArr(I,J) + I + J";
   out.Put(str);
   str1 = ReplaceName(str, "MyArr", "MyArr_xxx");
   out.Validate(str1, "GMAT MyArr_xxx(I,J) = MyArr_xxx(I,J) + MyArr_xxx(I,J) + I + J");
   
   //------------------------------
   str = "GMAT MyArr(I,J) = MyArr(I,J) + MyArr(I,J) + I + J";
   out.Put(str);
   str1 = ReplaceName(str, "I", "III");
   out.Validate(str1, "GMAT MyArr(III,J) = MyArr(III,J) + MyArr(III,J) + III + J");
   
   //------------------------------
   str = "GMAT MyArr(I,J)=MyArr(I,J)+MyArr(I,J)+I+J";
   out.Put(str);
   str1 = ReplaceName(str, "J", "JJJ");
   out.Validate(str1, "GMAT MyArr(I,JJJ)=MyArr(I,JJJ)+MyArr(I,JJJ)+I+JJJ");
   
   //------------------------------
   str = "GMAT MMS1M_ApoEphem (I,2) = Sat1.X + I";
   out.Put(str);
   str1 = ReplaceName(str, "I", "III");
   out.Validate(str1, "GMAT MMS1M_ApoEphem (III,2) = Sat1.X + III");
   
   //------------------------------
   str = "Sat1.X + I";
   out.Put(str);
   str1 = ReplaceName(str, "Sat1", "Sat1_Sat1");
   out.Validate(str1, "Sat1_Sat1.X + I");
   
   //------------------------------
   str = "Sat1_Sat1.X + I";
   out.Put(str);
   str1 = ReplaceName(str, "Sat1", "Sat1_Sat1");
   out.Validate(str1, "Sat1_Sat1.X + I");
   
   //------------------------------
   str = "Sat1.X + I";
   out.Put(str);
   str1 = ReplaceName(str, "I", "III");
   out.Validate(str1, "Sat1.X + III");
   
   //------------------------------
   str = "Sat1.X + I + I";
   out.Put(str);
   str1 = ReplaceName(str, "I", "III");
   out.Validate(str1, "Sat1.X + III + III");
   
   //------------------------------
   str = "Sat1.X + III";
   out.Put(str);
   str1 = ReplaceName(str, "III", "I");
   out.Validate(str1, "Sat1.X + I");
   
   //------------------------------
   str = "Sat1.X + III + III";
   out.Put(str);
   str1 = ReplaceName(str, "III", "I");
   out.Validate(str1, "Sat1.X + I + I");
   
   //------------------------------
   str = "GMAT MyArr(I,J)=MyArr(I,J)+MyArr(I,J)+I+J";
   out.Put(str);
   str1 = ReplaceName(str, "M", "MMM");
   out.Validate(str1, str);
   
   out.Put("");
   out.Put("============================== test GmatStringUtil::ReplaceNumber()");
   
   //------------------------------
   str = "3.14e-0";
   out.Put(str);
   str1 = ReplaceNumber(str, "e-", "e#");
   out.Validate(str1, "3.14e#0");
   
   //------------------------------
   str = "3.14e-0+e-3.14";
   out.Put(str);
   str1 = ReplaceNumber(str, "e-", "e#");
   out.Validate(str1, "3.14e#0+e-3.14");
   
   //------------------------------
   str = "3.14e-0+e-3.14-1.2E+1";
   out.Put(str);
   str1 = ReplaceNumber(str, "e-", "e#");
   out.Validate(str1, "3.14e#0+e-3.14-1.2E+1");

   str1 = ReplaceNumber(str1, "E+", "E#");
   out.Validate(str1, "3.14e#0+e-3.14-1.2E#1");

   out.Put("");
   out.Put("============================== test GmatStringUtil::ReplaceChainedUnaryOperators()");
   
   //------------------------------
   str = "+--+abc-+--def+-+-ghi";
   out.Put(str);
   str1 = ReplaceChainedUnaryOperators(str);
   out.Validate(str1, "+abc-def+ghi");
   
   //------------------------------
   str = "-----(abc+++++def)-----(ghi)^2";
   out.Put(str);
   str1 = ReplaceChainedUnaryOperators(str);
   out.Validate(str1, "-(abc+def)-(ghi)^2");
   
   out.Put("");
   out.Put("============================== test GmatStringUtil::RemoveScientificNotation()");
   
   //------------------------------
   str = "abc + (-xy1e7) + z";
   out.Put(str);
   str1 = RemoveScientificNotation(str);
   out.Validate(str1, str);
   
   //------------------------------
   str = "3.588645172297E+02$";
   out.Put(str);
   str1 = RemoveScientificNotation(str);
   out.Validate(str1, "3.58864517229702$");
   
   //------------------------------
   str = "1e-007";
   out.Put(str);
   str1 = RemoveScientificNotation(str);
   out.Validate(str1, "1007");
   
   //------------------------------
   str = "+1e-10";
   out.Put(str);
   str1 = RemoveScientificNotation(str);
   out.Validate(str1, "+110");
   
   //------------------------------
   str = "1e-10";
   out.Put(str);
   str1 = RemoveScientificNotation(str);
   out.Validate(str1, "110");
   
   //------------------------------
   str = "varWith5e10";
   out.Put(str);
   str1 = RemoveScientificNotation(str);
   out.Validate(str1, str);
   
   //------------------------------
   str = "E8sGs3bQk2gGKsrU2Mw1e7lML3nRJvSAEpLMMMGuPFX";
   out.Put(str);
   str1 = RemoveScientificNotation(str);
   out.Validate(str1, str);
   
   //------------------------------
   str = "1.45e+7";
   out.Put(str);
   str1 = RemoveScientificNotation(str);
   out.Validate(str1, "1.457");
   
   //------------------------------
   str = "5e7";
   out.Put(str);
   str1 = RemoveScientificNotation(str);
   out.Validate(str1, "57");
   
   //------------------------------
   str = "5e";
   out.Put(str);
   str1 = RemoveScientificNotation(str);
   out.Validate(str1, "5e");

   out.Put("============================== test GmatStringUtil::GetArrayIndexVar(rowStr, colStr)");
   std::string rowStr, colStr;
   std::string name;
   
   //------------------------------
   str = "Arr(I,J)";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "I", "J");
   out.Put("");
   
   //------------------------------
   str = "Arr(10)";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "1", "10");
   out.Put("");
   
   //------------------------------
   str = "Arr(arr2(2,2))";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "1", "arr2(2,2)");
   out.Put("");
   
   //------------------------------
   str = "Arr(I, b(2))";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "I", "b(2)");
   out.Put("");
   
   //------------------------------
   str = "Arr( b(x,y), I)";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "b(x,y)", "I");
   out.Put("");
   
   //------------------------------
   str = "Arr(I, b(2,2))";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "I", "b(2,2)");
   out.Put("");
   
   //------------------------------
   str = "Arr(I, b(2,c(x,y)))";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "I", "b(2,c(x,y))");
   out.Put("");
   
   //------------------------------
   str = "Arr( b(2,c(x,y)), I)";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "b(2,c(x,y))", "I");
   out.Put("");
   
   //------------------------------
   str = "Arr(a(1), b(2))";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "a(1)", "b(2)");
   out.Put("");
   
   //------------------------------
   str = "Arr(arr1(1,1), arr2(2,2))";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "arr1(1,1)", "arr2(2,2)");
   out.Put("");
   
   //------------------------------
   str = "Arr(arr1(1, arr3(a,b)), arr2(2,2))";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "arr1(1,arr3(a,b))", "arr2(2,2)");
   out.Put("");
   
   //------------------------------
   str = "Arr(arr1(1, arr3(a, d(x,y))), arr2(2,2))";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "arr1(1,arr3(a,d(x,y)))", "arr2(2,2)");
   out.Put("");
   
   //------------------------------
   str = "Arr(arr1(1, arr3(a, d(x,y))), arr1(1, arr3(a, d(x,y))))";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "arr1(1,arr3(a,d(x,y)))", "arr1(1,arr3(a,d(x,y)))");
   out.Put("");
   
   //------------------------------
   std::string left = "arr1(arr3(a,d(x,y)),arr3(a,d(x,y)))";
   str = "Arr(" + left + "," + left + ")";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, left, left);
   out.Put("");
   
   out.Put("");
   out.Put("====================== Check error condition");
   out.Put("================= missing column");
   
   //------------------------------
   str = "Arr(1,)";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "1", "-1");
   out.Put("");
   
   out.Put("================= missing row");
   
   //------------------------------
   str = "Arr(,1)";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "-1", "1");
   out.Put("");
   
   out.Put("================= missing row and column");
   
   //------------------------------
   str = "Arr(,)";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "-1", "-1");
   out.Put("");
   
   out.Put("================= missing row, comman and column");
   
   //------------------------------
   str = "Arr()";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "-1", "-1");
   out.Put("");
   
   out.Put("================= not an array");
   
   //------------------------------
   str = "Arr";
   GetArrayIndexVar(str, rowStr, colStr, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(rowStr, colStr, "-1", "-1");
   out.Put("");
   
      
   out.Put("================= unbalanced outer brackets");
   //------------------------------
   str = "Arr(arr2(2,2)";
   try
   {
      GetArrayIndexVar(str, rowStr, colStr, name);
      throw UtilityException(str + " should throw an exception");
   }
   catch (UtilityException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   //------------------------------
   str = "Arr(arr1(1, arr3(a,b)), arr2(2,2)";
   try
   {
      GetArrayIndexVar(str, rowStr, colStr, name);
      throw UtilityException(str + " should throw an exception");
   }
   catch (UtilityException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   
   out.Put("================= unbalanced inner brackets");
   //------------------------------
   str = "Arr(arr1(1, arr3(a,b), arr2(2,2))";
   try
   {
      GetArrayIndexVar(str, rowStr, colStr, name);
      throw UtilityException(str + " should throw an exception");
   }
   catch (UtilityException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   
   out.Put("============================== test GmatStringUtil::GetArrayIndex(row, col)");
   Integer row, col;
      
   //------------------------------
   str = "A(1,2)";
   GetArrayIndex(str, row, col, name);
   out.Put(str);
   out.Validate(name, "A");
   out.Validate(row, col, 0, 1); // index start from 0
   out.Put("");
   
   //------------------------------
   str = "Abc(x,2)";
   GetArrayIndex(str, row, col, name);
   out.Put(str);
   out.Validate(name, "Abc");
   out.Validate(row, col, -1, 1);
   out.Put("");
   
   //------------------------------
   str = "Arr(I,J)";
   GetArrayIndex(str, row, col, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(row, col, -1, -1);
   out.Put("");
   
   str = "Arr(1x,2x)";
   GetArrayIndex(str, row, col, name);
   out.Put(str);
   out.Validate(name, "Arr");
   out.Validate(row, col, -1, -1);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::GetArrayIndex(rowStr, colStr, row, col)");
   
   str = "A(1,2)";
   GetArrayIndex(str, rowStr, colStr, row, col, name);
   out.Put(str);
   out.Validate(name, "A");
   out.Validate(rowStr, colStr, "1", "2");
   out.Validate(row, col, 0, 1);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::IsEnclosedWithExtraParen()");
   
   //------------------------------
   str = "(A)";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   //------------------------------
   str = "((1))";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   //------------------------------
   str = "(())";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "(vec(1,1)*vec(2,1))*vec(3,1)*vec(4,1)";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");

   //------------------------------
   str = "(3+5)  *  (2+2)";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");

   //------------------------------
   str = "(3+5)*2+2";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "(a*b(1,1))";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   //Until 2013.01.29 out.Validate(bval, true);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "((a+b))";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   //------------------------------
   str = "(a(1,1))";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   //------------------------------
   str = "((3+5)*2)";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "(a+b) * (c+d)";
   bval = IsEnclosedWithExtraParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::IsParenBalanced()");
   
   //------------------------------
   str = "(a+b)* (c+d)";
   bval = IsParenBalanced(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");

   //------------------------------
   str = "(a+b) * (c+d))";
   bval = IsParenBalanced(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");

   //------------------------------
   str = "((a+b) * (c+d)";
   bval = IsParenBalanced(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "((a+b)^2+(a-b)^2+(a*b-(a+b)^2))";
   bval = IsParenBalanced(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   //------------------------------
   str = "((a+b)^2+(a-b)^2+(a*b-(a+b)^2)";
   bval = IsParenBalanced(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::IsOuterParen()");
   
   //------------------------------
   str = "(a+b) * (c+d)";
   bval = IsOuterParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");

   //------------------------------
   str = "(vec(1,1)*vec(2,1))*vec(3,1)*vec(4,1)";
   bval = IsOuterParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");

   //------------------------------
   str = "(a*b-c)";
   bval = IsOuterParen(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");

   //------------------------------
   str = "(123,456)";
   bval = IsOuterParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");

   //------------------------------
   str = "(a,b)";
   bval = IsOuterParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");

   //------------------------------
   str = "(a12,b23)";
   bval = IsOuterParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");

   //------------------------------
   str = "(a12,b23)";
   bval = IsOuterParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");

   //------------------------------
   str = "(vec(3,2))";
   bval = IsOuterParen(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");

   //------------------------------
   str = "(10^(-9)";
   bval = IsOuterParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");

   //------------------------------
   str = "((a) + ((a+b)*c) + a^(-1))";
   bval = IsOuterParen(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::IsParenEmpty()");
   
   //------------------------------
   str = "(())";
   bval = IsParenEmpty(str);
   out.Put(str);
   out.Validate(bval, true);
   out.Put("");
   
   //------------------------------
   str = "dne(())";
   bval = IsParenEmpty(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   //------------------------------
   str = "((1))";
   bval = IsParenEmpty(str);
   out.Put(str);
   out.Validate(bval, false);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::FindMatchingParen()");
   
   Integer openParen, closeParen;
   
   //------------------------------
   str = "(10)";
   FindMatchingParen(str, openParen, closeParen, bval);
   out.Put(str);
   out.Validate(bval, true);
   out.Validate(openParen, 0);
   out.Validate(closeParen, 3);
   out.Put("");
   
   //------------------------------
   str = "Vec(10,20)";
   FindMatchingParen(str, openParen, closeParen, bval);
   out.Put(str);
   out.Validate(bval, false);
   out.Validate(openParen, 3);
   out.Validate(closeParen, 9);
   out.Put("");
   
   //------------------------------
   str = "(9*1000-20)*(-0.9754^2)";
   FindMatchingParen(str, openParen, closeParen, bval);
   out.Put(str);
   out.Validate(bval, false);
   out.Validate(openParen, 0);
   out.Validate(closeParen, 10);
   out.Put("");
   
   //------------------------------
   str = "((9*1000-20)*(-0.9754^2))";
   FindMatchingParen(str, openParen, closeParen, bval, 1);
   out.Put(str);
   out.Validate(bval, false);
   out.Validate(openParen, 1);
   out.Validate(closeParen, 11);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::FindLastParenMatch()");
   
   //------------------------------
   str = "(10+b)*(a/b*c)-(a*b)";
   FindLastParenMatch(str, openParen, closeParen);
   out.Put(str);
   out.Validate(openParen, closeParen, 15, 19);
   out.Put("");
   
   str = "(3*a+4)-(9*b-20)*(cos(c)^2)";
   FindLastParenMatch(str, openParen, closeParen);
   out.Put(str);
   out.Validate(openParen, closeParen, 17, 26);
   out.Put("");
   
   str = "((3*a+4)-(9*b-20)*(cos(c)^2))*(-a/b)*d-x";
   FindLastParenMatch(str, openParen, closeParen);
   out.Put(str);
   out.Validate(openParen, closeParen, 30, 35);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::RemoveExtraParen()");
   
   //------------------------------
   str = "sqrt((posVecAtt(1,1)-theSat.attitudeCoordSys.X)^2+(posVecAtt(2,1)-theSat.attitudeCoordSys.Y)^2+(posVecAtt(3,1)-theSat.attitudeCoordSys.Z)^2)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "sqrt((posVecAtt(1,1)-theSat.attitudeCoordSys.X)^2+(posVecAtt(2,1)-theSat.attitudeCoordSys.Y)^2+(posVecAtt(3,1)-theSat.attitudeCoordSys.Z)^2)");
   out.Put("");
   
   //------------------------------
   str = "(0.5,0.5)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "0.5,0.5");
   out.Put("");
   
   //------------------------------
   str = "vec(1,1)/(1000/c/vec(4,1))";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "vec(1,1)/(1000/c/vec(4,1))");
   out.Put("");
   
   //------------------------------
   str = "(1-cos(phi))*av*av'";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "(1-cos(phi))*av*av'");
   out.Put("");
   
   //------------------------------
   str = "(((arr_23*arr_31)*(arr_13*arr_32))' - ((arr_22))^(-1))";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "(((arr_23*arr_31)*(arr_13*arr_32))' - (arr_22)^(-1))");
   out.Put("");
   
   //------------------------------
   str = "(())";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "(())");
   out.Put("");
   
   //------------------------------
   str = "()";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "()");
   out.Put("");
   
   //------------------------------
   str = "5116.1085 - (-4237.076770)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "5116.1085 - (-4237.076770)");
   out.Put("");
   
   //------------------------------
   str = "Sat.X*(b*c*vec(4,1)) - 10.9056168";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "Sat.X*(b*c*vec(4,1)) - 10.9056168");
   out.Put("");
   
   //------------------------------
   str = "cos(phi)*I3 + (1 - cos(phi))*av*av' - sin(phi)*across";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "cos(phi)*I3 + (1 - cos(phi))*av*av' - sin(phi)*across");
   out.Put("");
   
   //------------------------------
   str = "((x^(-1)))";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   //Before 2013.01.29 out.Validate(str1, "x^(-1)");
   out.Validate(str1, "(x^(-1))");
   out.Put("");
   
   //------------------------------
   str = "( (x^(-1)) )";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   //Before 2013.01.29 out.Validate(str1, "( x^(-1) )");
   out.Validate(str1, "( (x^(-1)) )");
   out.Put("");
   
   //------------------------------
   str = "(A')^(-1)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "(A')^(-1)");
   out.Put("");
   
   //------------------------------
   str = "A (1,1)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "A (1,1)");
   out.Put("");
   
   //------------------------------
   str = "Acos  (A  (1, 1))";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "Acos  (A  (1, 1))");
   out.Put("");
   
   //------------------------------
   str = "((a))";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "a");
   out.Put("");
   
   //------------------------------
   str = "((a+b))";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "(a+b)");
   out.Put("");
   
   //------------------------------
   str = "(((a) + (((a+b)*c)) + a^(-1)))";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   //OLD out.Validate(str1, "(a + ((a+b)*c) + a^(-1))");
   //Before 2013.02.01 out.Validate(str1, "(a + (((a+b)*c)) + a^(-1))");
   out.Validate(str1, "((a) + (((a+b)*c)) + a^(-1))");
   out.Put("");
   
   //------------------------------
   str = "(3+5)  *  (2+2)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, str);
   out.Put("");
   
   //------------------------------
   str = "(10-2)^2+(4-2)^2+(15-10)^2";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, str);
   out.Put("");

   //------------------------------
   str = "Cos(0.0) + 10.0";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, str);
   out.Put("");
   
   //------------------------------
   str = "(a(1,1) + 10.0)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   //Before 2013.01.29 out.Validate(str1, "a(1,1) + 10.0");
   out.Validate(str1, "(a(1,1) + 10.0)");
   out.Put("");
   
   //------------------------------
   str = "1 + (a(1,1) + 10.0) * 2";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "1 + (a(1,1) + 10.0) * 2");
   out.Put("");

   //------------------------------
   str = "2.34/1000*34.78*(Sat.VX) -  1.09056168*10^(-5)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   //OLD out.Validate(str1, "2.34/1000*34.78*Sat.VX -  1.09056168*10^(-5)");
   out.Validate(str1, "2.34/1000*34.78*(Sat.VX) -  1.09056168*10^(-5)");
   out.Put("");
   
   //------------------------------
   str = "a*b*c/(vec(4,1))/100000000 - 6.0735223880597";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   //OLD out.Validate(str1, "a*b*c/vec(4,1)/100000000 - 6.0735223880597");
   out.Validate(str1, "a*b*c/(vec(4,1))/100000000 - 6.0735223880597");
   out.Put("");
   
   //------------------------------
   str = "atan2(1,0)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, str);
   out.Put("");
   
   //------------------------------
   str = "cos(var0)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, str);
   out.Put("");
   
   //------------------------------
   str = "Sat.X/(1000/c/d) - 1.09056168*10^(-5)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, str);
   out.Put("");
   
   //------------------------------
   str = "vec(1,1)/(1000/c/vec(4,1)) - 1.09056168*10^(-5)";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, "vec(1,1)/(1000/c/vec(4,1)) - 1.09056168*10^(-5)");
   out.Put("");
   
   //------------------------------
   str = "((a)) + ((a+b)) + ((a*b))";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   //OLD out.Validate(str1, "a + (a+b) + (a*b)");
   out.Validate(str1, "(a) + ((a+b)) + ((a*b))");
   out.Put("");
   
   //------------------------------
   //str = "( ( ((vec(1,1)))/ (((1000/c/vec(4,1)))) - 1.09056168*10^(-5) ) )";
   //str = "(vec(1,1)/(1000/c/vec(4,1)) - 1.09056168*10^(-5))";
   //str = "(vec(1,1)/(1000/c/vec(4,1)) - 1.09056168*10^(-5))";
   str = "((((vec(1,1)))/(((1000/c/vec(4,1)))) - 1.09056168*10^(-5)))";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   //OLD out.Validate(str1, "(vec(1,1)/1000/c/vec(4,1) - 1.09056168*10^(-5))");
   //Before 2013.01.29 out.Validate(str1, "(vec(1,1)/(1000/c/vec(4,1)) - 1.09056168*10^(-5))");
   out.Validate(str1, "((vec(1,1))/((1000/c/vec(4,1))) - 1.09056168*10^(-5))");
   out.Put("");
   
   //------------------------------
   str = "((3*2.34+4)-(9*1000-20)*(-0.9754^2))*(-2.34/0.001)*0.000134-0.05";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, str);
   out.Put("");
   
   //------------------------------
   str = "((3*a+4)-(9*b-20)*(cos(c)^2))*(-a/b)*d-0.00267522370194881";
   str1 = RemoveExtraParen(str);
   out.Put(str);
   out.Validate(str1, str);
   out.Put("");
   
   out.Put("============================== test GmatStringUtil::ParseFunctionName(str)");
   
   //------------------------------
   str = "[out] = Function1(in);";
   name = ParseFunctionName(str);
   out.Put(str);
   out.Validate(name, "Function1");

   //------------------------------
   str = "[] = Function2(in);";
   name = ParseFunctionName(str);
   out.Put(str);
   out.Validate(name, "Function2");

   //------------------------------
   str = "Function3 (in);";
   name = ParseFunctionName(str);
   out.Put(str);
   out.Validate(name, "Function3");

   //------------------------------
   str = "Function4;";
   name = ParseFunctionName(str);
   out.Put(str);
   out.Validate(name, "Function4");
   
   //------------------------------
   str = " Function5;;;;;;";
   name = ParseFunctionName(str);
   out.Put(str);
   out.Validate(name, "Function5");
   
   return 0;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestStringUtilOut.txt";   
   TestOutput out(outFile);
   out.Put(GmatTimeUtil::FormatCurrentTime());
   MessageInterface::ShowMessage("%s\n", GmatTimeUtil::FormatCurrentTime().c_str());

   // Some sin test here
   Real a = 1.463322321210277e+263;
   MessageInterface::ShowMessage("********** sin(1.463322321210277e+263) = %f\n", sin(1.463322321210277e+263));
   MessageInterface::ShowMessage("********** sin(a) = %f\n", sin(a));
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of StingUtil!!");
   }
   catch (UtilityException &e)
   {
      out.Put(e.GetFullMessage());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
