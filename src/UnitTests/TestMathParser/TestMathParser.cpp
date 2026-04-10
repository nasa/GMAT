//------------------------------------------------------------------------------
//                                  TestMathParser
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
// Created: 2006/04/24
//
/**
 * Purpose:
 * Unit test driver of MathParser class.
 *
 * Output file:
 * TestMathParserOut.txt
 *
 * Description MathParser:
 * The MathParser class takes a line of script that evaluates to inline math,
 * and breaks that line apart into its component elements using a recursive 
 * descent algorithm.  The resulting representation is stored in a binary tree 
 * structure, which is calculated, depth first, when the expression needs to be
 * evaluated during execution of a script.
 * 
 * Test Procedure:
 * 1. Create MathParser.
 * 2. Create string containg math expression.
 * 3. Create a MathNode pointer.
 * 4. Pass the expression to Parse() "node = mp.Parse(expstr)".
 * 5. Call EvaluateNode() to evaluate and validate the node by passing expected result.
 * 6. Repeat 2 through 5 to test differerent math expression.
 *
 * Validation method:
 * The test driver code knows expected results and throws an exception if the
 * result is not within the tolerance.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "MathParser.hpp"
#include "GmatGlobal.hpp"
#include "MathNode.hpp"
#include "MathFunction.hpp"
#include "MathElement.hpp"
#include "Variable.hpp"
#include "Array.hpp"
#include "StringVar.hpp"
#include "BaseException.hpp"
#include "Rmatrix33.hpp"
#include "ArrayWrapper.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "TestOutput.hpp"

#include <iostream>

namespace GmatTest
{
   WrapperMap testWrapper;
   ElementWrapper *ewI;
}

//#define DEBUG_TEST_MATH_PARSER 2

using namespace std;


//------------------------------------------------------------------------------
// void GetNodes(MathNode *node, MathNode **left, MathNode **right)
//------------------------------------------------------------------------------
void GetNodes(MathNode *node, MathNode **left, MathNode **right)
{
   *left = node->GetLeft();
   *right = node->GetRight();

   #if DEBUG_TEST_MATH_PARSER
   MessageInterface::ShowMessage
      ("==> GetNodes() node=%s, %s\n", node->GetTypeName().c_str(),
       node->GetName().c_str());
   
   if (*left)
      MessageInterface::ShowMessage
         ("   left=%s, %s\n", (*left)->GetTypeName().c_str(),
          (*left)->GetName().c_str());

   if (*right)
      MessageInterface::ShowMessage
         ("   right=%s, %s\n", (*right)->GetTypeName().c_str(),
          (*right)->GetName().c_str());

   #endif
}


//------------------------------------------------------------------------------
// void SetParameters(MathNode *node, const std::string &leftName, Parameter *leftParam,
//                    const std::string &rightName, Parameter *rightParam)
//------------------------------------------------------------------------------
void SetParameters(MathNode *node, const std::string &leftName, Parameter *leftParam,
                   const std::string &rightName, Parameter *rightParam)
{
   #if DEBUG_TEST_MATH_PARSER
   MessageInterface::ShowMessage
      ("==========> SetParameters() entered\n   node=<%p>, leftName='%s', leftParam=<%p>, "
       "rightName='%s', rightParam=<%p>\n", node, leftName.c_str(), leftParam,
       rightName.c_str(), rightParam);
   #endif
   
   MathNode *left = NULL;
   MathNode *right = NULL;
   
   if (node->IsFunction())
      GetNodes(node, &left, &right);
   
   #if DEBUG_TEST_MATH_PARSER
   MessageInterface::ShowMessage("   left=<%p>, right=<%p>\n", left, right);
   #endif
   
   if (left)
   {
      if (!left->IsFunction())
      {
         if (left->GetName() == "arrI")
         {
            #if DEBUG_TEST_MATH_PARSER > 1
            MessageInterface::ShowMessage("   Calling left->SetRefObject()\n");
            #endif
            
            GmatTest::ewI->SetDescription("arrI");
            GmatTest::ewI->SetRefObject(leftParam);
            GmatTest::testWrapper.clear();
            GmatTest::testWrapper.insert(make_pair("arrI", GmatTest::ewI));
            left->SetMathWrappers(&GmatTest::testWrapper);
            left->SetRefObject(rightParam, Gmat::PARAMETER, rightName);
         }
         else
            left->SetRefObject(leftParam, Gmat::PARAMETER, leftName);
         
         #if DEBUG_TEST_MATH_PARSER > 1
         Rmatrix mat = left->GetMatrixValue();
         MessageInterface::ShowMessage
            ("==> SetParameters() left mat=\n%s\n", mat.ToString(12).c_str());
         #endif
      }
      else
      {
         SetParameters(left, leftName, leftParam, rightName, rightParam);
      }
   }
   
   
   if (right)
   {
      if (!right->IsFunction())
      {         
         if (!right->IsNumber())
         {
            right->SetRefObject(rightParam, Gmat::PARAMETER, rightName);
            
            #if DEBUG_TEST_MATH_PARSER > 1
            Rmatrix mat = right->GetMatrixValue();
            MessageInterface::ShowMessage
               ("==> SetParameters() right mat=\n%s\n", mat.ToString(12).c_str());
            #endif
         }
      }
      else
      {
         SetParameters(right, leftName, leftParam, rightName, rightParam);
      }
   }
   
   #if DEBUG_TEST_MATH_PARSER
   MessageInterface::ShowMessage("==========> SetParameters() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void EvaluateNode(MathNode *node, TestOutput &out, Real expVal, Rmatrix &expMat)
//------------------------------------------------------------------------------
void EvaluateNode(MathNode *node, TestOutput &out, Real expVal, Rmatrix &expMat)
{
   Integer returnType;
   Integer numRow;
   Integer numCol;
   Real realVal;
   Rmatrix rmat;
   std::string nodeType = node->GetTypeName();
   
   #if DEBUG_TEST_MATH_PARSER
   MessageInterface::ShowMessage
      ("==========> EvaluateNode() node=%s, %s\n==> Now validate inputs\n", nodeType.c_str(),
       node->GetName().c_str());
   #endif
   
   if (node->ValidateInputs())
   {
      node->GetOutputInfo(returnType, numRow, numCol);
      
      #if DEBUG_TEST_MATH_PARSER
      MessageInterface::ShowMessage
         ("==> returnType=%d, numRow=%d, numCol=%d\n", returnType,numRow, numCol);
      #endif
      
      if (returnType == Gmat::REAL_TYPE)
      {
         realVal = node->Evaluate();
         out.Validate(realVal, expVal, 1.e-10);
      }
      else
      {
         rmat.SetSize(numRow, numCol);
         rmat = node->MatrixEvaluate();
         out.Validate(rmat, expMat);
      }
   }
   else
   {
      throw MathException
         ("*** TestMathParser::EvaluateNode() " + nodeType +
          "->ValidateInputs() returned false\n");
   }
   
   #if DEBUG_TEST_MATH_PARSER
   MessageInterface::ShowMessage("==========> EvaluateNode() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void TestIsEquation(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestIsEquation(TestOutput &out, MathParser &mp)
{
   std::string expstr;
   bool boolVal;
   bool expBoolVal;
   
   out.Put("============================== Test IsEquation()");

   //------------------------------
   expstr = "123.456";
   expBoolVal = false;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   
   //------------------------------
   expstr = "-123.456";
   expBoolVal = false;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   
   //------------------------------
   expstr = "Cos(0)";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   
   //------------------------------
   expstr = "ars(1,1)";
   expBoolVal = false;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   
   //------------------------------
   expstr = "a+b";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   
   //------------------------------
   expstr = "-abc";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   
   //------------------------------
   expstr = "M'";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   
   //------------------------------
   expstr = "M^(-1)";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   
   //------------------------------
   expstr = "TA1 = abs( TA1 - 360 )";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);

   //------------------------------
   expstr = "cross(vv, cross(rv, vv));";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   
}


//------------------------------------------------------------------------------
// void TestFindLowestOperator(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestFindLowestOperator(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test FindLowestOperator()");
   std::string expstr;
   Integer opIndex;
   std::string str1;
   
   #if 1
   //------------------------------
   expstr = "y^2^(-1)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "^");
   out.Validate(opIndex, 3);
   out.Put("");
   #endif
   
   #if 1
   //------------------------------
   expstr = "y^(-1)^2";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "^");
   out.Validate(opIndex, 6);
   out.Put("");
   
   //------------------------------
   expstr = "A'^(-1)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "^(-1)");
   out.Validate(opIndex, 2);
   out.Put("");
   
   //------------------------------
   expstr = "-tan(11.907)+1.47756418563724";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "+");
   out.Validate(opIndex, 12);
   out.Put("");
   
   //------------------------------
   expstr = "2.0e-1+3.0e-1+4.0e+0";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "+");
   out.Validate(opIndex, 13);
   out.Put("");
   
   //------------------------------
   expstr = "(rv'*vv)*vv";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "*");
   out.Validate(opIndex, 8);
   out.Put("");
   
   //------------------------------
   expstr = "2^3^4";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "^");
   out.Validate(opIndex, 3);
   out.Put("");
   
   //------------------------------
   expstr = "-218.6/-248.715095169/(-209.5774/-132.61614521353)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "/");
   out.Validate(opIndex, 21);
   out.Put("");
   
   //------------------------------
   expstr = "-(-0.001008965327910524)^869.28";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 0);
   out.Put("");
   
   //------------------------------
   expstr = "-((var4/var3))";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 0);
   out.Put("");
   
   //------------------------------
   expstr = "count+1";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "+");
   out.Validate(opIndex, 5);
   out.Put("");
   
   //------------------------------
   expstr = "((3*a+4)-(9*b-20)*(cos(c)^2))*(-a/b)*d-x";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 38);
   out.Put("");
   
   //------------------------------
   expstr = "(3*a+4)-(9*b-20)*(cos(c)^2)*(-a/b)*d-x";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 36);
   out.Put("");
   
   //------------------------------
   expstr = "(3*a+4)*(9*b-20)-(cos(c)^2)*(-a/b)*(d-x)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 16);
   out.Put("");
   
   //------------------------------
   expstr = "(3*a+4)*(9*b-20)-(cos(c)^2)*(-a/b)*(d-x)+5";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "+");
   out.Validate(opIndex, 40);
   out.Put("");
   
   //------------------------------
   expstr = "(3*a+4)*(9*b-20)/(cos(c)^2)*(-a/b)*(d-x)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "*");
   out.Validate(opIndex, 34);
   out.Put("");
   
   //------------------------------
   expstr = "(3*a+4)^(9*b-20)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "^");
   out.Validate(opIndex, 7);
   out.Put("");
   
   //------------------------------
   expstr = "(3*a+4)-(9*b-20)*5-2+2";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "+");
   out.Validate(opIndex, 20);
   out.Put("");
   
   //------------------------------
   expstr = "(3+5)*(2+2)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "*");
   out.Validate(opIndex, 5);
   out.Put("");
   
   //------------------------------
   expstr = "5^(-1/2)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "^");
   out.Validate(opIndex, 1);
   out.Put("");
   
   //------------------------------
   expstr = "(3+5)*2+2";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "+");
   out.Validate(opIndex, 7);
   out.Put("");
   
   //------------------------------
   expstr = "1*1-1*(10*-50)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 3);
   out.Put("");
   
   //------------------------------
   expstr = "(1*1)-1*(10*-50)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 5);
   out.Put("");
   
   //------------------------------
   expstr = "((3*2+4)-(9*1000-20)*(-0.97^2))*(-2.34/0.001)*0.134";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "*");
   out.Validate(opIndex, 45);
   out.Put("");
   
   //------------------------------
   expstr = "Sat.X*(b*c*vec(4,1))-10.9056168";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 20);
   out.Put("");
   
   //------------------------------
   expstr = "a*b*c/vec";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "/");
   out.Validate(opIndex, 5);
   out.Put("");
   
   //------------------------------
   expstr = "(a*b*c/vec)*(s+y)/2*a*b*(a/b)*2-5";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 31);
   out.Put("");
   
   //------------------------------
   expstr = "(a*b*c/vec)*(s+y)/2*a*b*(a/b)*2*5";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "*");
   out.Validate(opIndex, 31);
   out.Put("");
   
   //------------------------------
   expstr = "cos(phi)*I3+(1-cos(phi))*av*av'-sin(phi)*across";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 31);
   out.Put("");
   
   //------------------------------
   expstr = "cos(phi)*I3+(1-cos(phi))*av*av'";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "+");
   out.Validate(opIndex, 11);
   out.Put("");
   
   //------------------------------
   expstr = "a++4";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "+");
   out.Validate(opIndex, 1);
   out.Put("");
   
   //------------------------------
   expstr = "a+-4";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "+");
   out.Validate(opIndex, 1);
   out.Put("");
   
   //------------------------------
   expstr = "a--4";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 1);
   out.Put("");
   
   //------------------------------
   expstr = "a-+4";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 1);
   out.Put("");
   
   //------------------------------
   expstr = "-a4";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "-");
   out.Validate(opIndex, 0);
   out.Put("");
   
   //------------------------------
   expstr = "sqrt(1.0^2+2.0^2+3.0^2)+sqrt(1.0^2+2.0^2+3.0^2);";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "+");
   out.Validate(opIndex, 23);
   out.Put("");
   
   //------------------------------
   expstr = "acos(sv1'*SpinVector/S1)*180;";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "*");
   out.Validate(opIndex, 24);
   out.Put("");
   
   //------------------------------
   expstr = "acos(sv1'*SpinVector/S1)*180/pi;";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "/");
   out.Validate(opIndex, 28);
   out.Put("");
   
   //------------------------------
   expstr = "5*-2";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "*");
   out.Validate(opIndex, 1);
   out.Put("");
   
   //------------------------------
   expstr = "M^(-1)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "");
   out.Validate(opIndex, std::string::npos);
   out.Put("");
   
   //------------------------------
   expstr = "sin(94*0.0174532925199433)^2;";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "^");
   out.Validate(opIndex, 26);
   out.Put("");
   
   #endif
} //TestFindLowestOperator()


//------------------------------------------------------------------------------
// void TestOpsWithNumber(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestOpsWithNumber(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test Math Operations with Number");
   
   std::string expstr;
   Real expRealVal;
   Rmatrix unsetMat;
   MathNode *node = NULL;
   
   #if 1
   //------------------------------
   expstr = "2+3+4";
   expRealVal = 2 + 3 + 4;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "2.0e-1 + 3.0e-1 + 4.0e+0";
   expRealVal = 2.0e-1 + 3.0e-1 + 4.0e+0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(2.0e-1 + 3.0e-1) - (4.0e+0 - 5.0e-1)";
   expRealVal = (2.0e-1 + 3.0e-1) - (4.0e+0 - 5.0e-1);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "((2.0e-1 + 3.0e-1) - (4.0e+0 - 5.0e-1)) + (1000e-0003 - 500e-00004)";
   expRealVal = ((2.0e-1 + 3.0e-1) - (4.0e+0 - 5.0e-1)) + (1000e-0003 - 500e-00004);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   #endif
   
   #if 1
   //------------------------------
   expstr = "((((0.584628238e+005-((0.87652836e+0005-0.242169149e-01))-0.2799199e-0000004)-0.8234313e-000001)-0.247998748e-0006)-0.665e-000004)-0.619624588838e-000001;";
   expRealVal = ((((0.584628238e+005-((0.87652836e+0005-0.242169149e-01))-0.2799199e-0000004)-0.8234313e-000001)-0.247998748e-0006)-0.665e-000004)-0.619624588838e-000001;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   #endif
   
   #if 1
   //------------------------------
   expstr = "2^3^4";
   expRealVal = pow(pow(2.0, 3.0), 4.0);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "0.137466421432647^    0.06533509 ^ -0.02467255477529   ";
   expRealVal = pow(pow(0.137466421432647, 0.06533509), -0.02467255477529);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "-218.6  /  -248.715095169  / (   -209.5774  /  -132.61614521353   )   ";
   expRealVal = -218.6  /  -248.715095169  / (   -209.5774  /  -132.61614521353   )   ;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;

   //------------------------------
   expstr = "0.001008965327910524^869.28";
   expRealVal = pow(0.001008965327910524, 869.28);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "-0.001008965327910524^869.28";
   expRealVal = -pow(0.001008965327910524, 869.28);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   #endif
   
   #if 1
   //------------------------------
   expstr = "-(-0.001008965327910524)^869.28";
   expRealVal = -pow((-0.001008965327910524), 869.28);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "-(1.9846348 / -1967)^869.28";
   expRealVal = -pow((1.9846348 / -1967), 869.28);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   #endif
   
   #if 1
   //------------------------------
   expstr = "123.456";
   expRealVal = 123.456;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "3+5*2";
   expRealVal = 13;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "3+5+2*2";
   expRealVal = 12;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "3+5*2*2";
   expRealVal = 23;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5116.1085^0";
   expRealVal = 1.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5^-2";
   expRealVal = 0.04;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5  *   -2";
   expRealVal = -10;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5/-2";
   expRealVal = -2.5;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5+-2";
   expRealVal = 3.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5-+2";
   expRealVal = 3.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5++2";
   expRealVal = 7.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5--2";
   expRealVal = 7.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "-3*2 + 6*8";
   expRealVal = 42.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "-3*2 - 6*8";
   expRealVal = -54.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "0.2*0.3*2.0/3.0/5.0 - 10.0";
   expRealVal = -9.992;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "10 - 50 + 1 + 30 - 25";
   expRealVal = 10 - 50 + 1 + 30 - 25;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   #endif
}

//------------------------------------------------------------------------------
// void TestOpsWithNumberWithParen(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestOpsWithNumberWithParen(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test Math Operations with Number with Parenthesis");
   
   std::string expstr;
   Real expRealVal;
   Rmatrix unsetMat;
   MathNode *node = NULL;
   
   //------------------------------
   expstr = "5^(-1/2)";
   expRealVal = 0.447213595499958;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(3+5)*2+2";
   expRealVal = 18;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(3+5)  *  (2+2)";
   expRealVal = 32;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "((3+5)*2)*2";
   expRealVal = 32;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "((3+5)*2)*2";
   expRealVal = 32;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(3+5)*(2+2)*(4+5)";
   expRealVal = 288;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5116.1085 + (-4237.076770)";
   expRealVal = 879.03173;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5116.1085 - (-4237.076770)";
   expRealVal = 9353.18527;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "30.0 * (-2.0)";
   expRealVal = -60.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "30.0 / (-2.0)";
   expRealVal = -15.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "( ( 5^2 - 4/2 )*2 - 3*5  ) / 4";
   expRealVal = 7.75;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "-(50/2*2)";
   expRealVal = -50.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "-(50*2/2)";
   expRealVal = -50.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "-(50/2/2)";
   expRealVal = -12.5;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "( 10 - 2 )^2 + ( 4 - 2 )^2 + ( 15 - 10 )^2";
   expRealVal = 93.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "1*1 - 1*(10*-50)";
   expRealVal = 501.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(1*1) - 1*(10*-50)";
   expRealVal = 501.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(7+ 10)*10";
   expRealVal = 170.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "10*(7+ 10)";
   expRealVal = 170.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(10)*5*6/2";
   expRealVal = 150;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "200*1000^(-1)";
   expRealVal = 0.2;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(10)*5*6/2 - 200*1000^(-1)";
   expRealVal = 149.8;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "0.2*(5*6*0.2) - 50";
   expRealVal = -48.8;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(0.2)*(5*6*0.2) - 50";
   expRealVal = -48.8;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(0.2)*5*6*0.2 - 50";
   expRealVal = -48.8;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "2.34*0.000134*34.78/(1000) - 1.09056168*10^(-5)";
   expRealVal = 0.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "2.34*0.000134*34.78/1000 - 1.09056168*10^(-5)";
   expRealVal = 0.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "0.5/(1000/0.5/2.0) - 20.2343*10^(-2)";
   expRealVal = -0.201843;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "5/(3/2/6)";
   expRealVal = 20.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "( (3.4*2.34+4.2) )";
   expRealVal = 1.215600000000000e+001;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(((-0.9754)^2) )";
   expRealVal = 9.514051600000001e-001;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "( (3.4*2.34+4.2)-(9.1*1000.23-20.21) )";
   expRealVal = -9.069726999999999e+003;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "( (-0.9754^2) )";
   expRealVal = -9.514051600000001e-001;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "( (3.4*2.34+4.2)-(9.1*1000.23-20.21)*(-0.9754^2) )";
   expRealVal = 8.652706348716281e+003;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "( (3.4*2.34+4.2)-(9.1*1000.23-20.21)*(-0.9754^2) )*(-2.34/0.001)";
   expRealVal = -2.024733285599610e+007;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "( (3.4*2.34+4.2)-(9.1*1000.23-20.21)*(-0.9754^2) )*(-2.34/0.001)*0.000134 - 0.05";
   expRealVal = -2.713192602703477e+003;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
}


//------------------------------------------------------------------------------
// void TestFunctionWithNumber(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestFunctionWithNumber(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test Function with Number");
   
   std::string expstr;
   Real expRealVal;
   Rmatrix unsetMat;
   MathNode *node = NULL;

   #if 1
   
   //------------------------------
   expstr = "(cos(0.000134)^2)";
   expRealVal = (pow(cos(0.000134), 2.0));
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "( (3.4*2.34+4.2)-(9.1*1000.23-20.21)*(cos(0.000134)^2) )*(-2.34/0.001)*0.000134 - 0.05";
   expRealVal = ( (3.4*2.34+4.2)-(9.1*1000.23-20.21)*(pow(cos(0.000134),2)) )*(-2.34/0.001)*0.000134 - 0.05;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "( (3*2.34+4)-(9*1000-20)*(cos(34.78)^2) )*(-2.34/0.001)*0.000134 - 0.00267522370194881";
   expRealVal = ( (3*2.34+4)-(9*1000-20)*(pow(cos(34.78),2)) )*(-2.34/0.001)*0.000134 - 0.00267522370194881;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Sqrt(( 10 - 2 )^2 + ( 4 - 2 )^2 + ( 15 - 10 )^2)";
   expRealVal = sqrt(pow(( 10 - 2 ), 2.0) + pow(( 4 - 2 ), 2.0) + pow(( 15 - 10 ), 2.0));
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Cos(0.0) + 10.0";
   expRealVal = cos(0.0) + 10.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Cos(0.0) + 10.0^2";
   expRealVal = cos(0.0) + pow(10.0, 2.0);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Sqrt(39)";
   expRealVal = sqrt(39);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Sqrt(44+10*10)";
   expRealVal = sqrt(44+10*10);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Sqrt(10*10+(54-10))";
   expRealVal = sqrt(10*10+(54-10));
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Sqrt(2^2 + 3^2 + 4^2)";
   expRealVal = sqrt(pow(2.0, 2.0) + pow(3.0, 2.0) + pow(4.0, 2.0));
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "acos(0)";
   expRealVal = acos(0);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "asin(1)";
   expRealVal = asin(1);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "atan(1)";
   expRealVal = atan(1);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "atan2(1,0)";
   expRealVal = GmatMathUtil::ATan(1,0);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "atan2(5-2+1,((2-1)+(2+5)))";
   expRealVal = GmatMathUtil::ATan(5-2+1,((2-1)+(2+5)));
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "exp(1)";
   expRealVal = exp(1);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "log(5+4*2-3)";
   expRealVal = log(5+4*2-3);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "log10(10)";
   expRealVal = log10(10);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "tan(0.5)";
   expRealVal = tan(0.5);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "DegToRad(180)";
   expRealVal = GmatMathUtil::DegToRad(180);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "RadToDeg(3.14159265358979)";
   expRealVal = GmatMathUtil::RadToDeg(3.14159265358979);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   
   //------------------------------
   expstr = "Rad2Deg(3.14159265358979)";
   expRealVal = GmatMathUtil::RadToDeg(3.14159265358979);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Deg2Rad(180)";
   expRealVal = GmatMathUtil::DegToRad(180);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "sqrt( 1.0^2 + 2.0^2 + 3.0^2 ) + sqrt( 4.0^2 + 5.0^2 + 6.0^2 );;";
   expRealVal = sqrt( pow(1.0, 2.0) + pow(2.0, 2.0) + pow(3.0, 2.0) ) +
      sqrt( pow(4.0, 2.0) + pow(5.0, 2.0) + pow(6.0, 2.0) );
   //expRealVal = 12.51662177416606;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(sin(0.5)^2);";
   expRealVal = (pow(sin(0.5), 2.0));
   //expRealVal = 0.229848847;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "sin(0.5 * 1.0)^2;";
   expRealVal = pow(sin(0.5 * 1.0), 2.0);
   //expRealVal = 0.229848847;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(sin(0.5 * 1.0)^2);";
   expRealVal = (pow(sin(0.5 * 1.0), 2.0));
   //expRealVal = 0.229848847;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(sin(94*0.0174532925199433))^2;";
   expRealVal = pow((sin(94*0.0174532925199433)), 2.0);
   //expRealVal = 0.9951340343707851;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   #endif
}


//------------------------------------------------------------------------------
// void TestOpsWithMatrix(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestOpsWithMatrix(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test Math Operation with Matrix");
   
   std::string expstr;
   Real expRealVal;
   Rmatrix unsetMat;
   MathNode *node = NULL;
   MathNode *left = NULL;
   MathNode *right = NULL;
   
   //------------------------------
   Rmatrix matA(3, 3, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0);
   Rmatrix matB(3, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
   expstr = "matA+matB";
   Rmatrix33 expMat1(11, 22, 33, 44, 55, 66, 77, 88, 99);
   out.Put(expstr + " should return\n", expMat1);
   node = mp.Parse(expstr);
   GetNodes(node, &left, &right);
   left->SetMatrixValue(matA);
   right->SetMatrixValue(matB);
   EvaluateNode(node, out, expRealVal, expMat1);
   delete node;
   
   //------------------------------
   expstr = "matA-matB";
   Rmatrix33 expMat2(9, 18, 27, 36, 45, 54, 63, 72, 81);
   out.Put(expstr + " should return\n", expMat2);
   node = mp.Parse(expstr);
   GetNodes(node, &left, &right);
   left->SetMatrixValue(matA);
   right->SetMatrixValue(matB);
   EvaluateNode(node, out, expRealVal, expMat2);
   delete node;
}


//------------------------------------------------------------------------------
// void TestVariable(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestVariable(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test Math Operation and Function with Variable");
   
   std::string expstr;
   Real expRealVal;
   Rmatrix unsetMat;
   MathNode *node = NULL;
   
   //-------------------------------------------------------------------
   // Until Wrapper stuff is implemented, the followng no longer works
   //-------------------------------------------------------------------
   
   //------------------------------
   expstr = "varA+varB";
   Variable *varA = new Variable("varA", "10.123");
   Variable *varB = new Variable("varB", "21.345");
   expRealVal = 31.468;
   out.Put("varA = ", varA->EvaluateReal());
   out.Put("varB = ", varB->EvaluateReal());
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   SetParameters(node, "varA", varA, "varB", varB);      
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "-varA";
   expRealVal = -10.123;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   SetParameters(node, "varA", varA, "varB", varB);      
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "varA^0";
   expRealVal = 1.0;
   out.Put("varA = ", varA->EvaluateReal());
   out.Put(expstr + " should return ", expRealVal);      
   node = mp.Parse(expstr);
   SetParameters(node, "varA", varA, "", NULL);      
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Cos(var0)";
   Variable *var0 = new Variable("var0", "0.0");
   expRealVal = 1.0;
   out.Put("var0 = ", var0->EvaluateReal());
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   SetParameters(node, "var0", var0, "", NULL);      
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Sin(var0)";
   expRealVal = 0.0;
   out.Put("var0=", var0->EvaluateReal());
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   SetParameters(node, "var0", var0, "", NULL);      
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(-varA/varB)";
   expRealVal = -4.742562661044741e-001;
   out.Put("varA=", varA->EvaluateReal());
   out.Put("varB=", varB->EvaluateReal());
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   SetParameters(node, "varA", varA, "varB", varB);      
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "abs( varA - 360 )";
   expRealVal = GmatMathUtil::Abs(10.123 - 360.0);
   out.Put("varA=", varA->EvaluateReal());
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   SetParameters(node, "varA", varA, "", NULL);      
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
}


//------------------------------------------------------------------------------
// void TestMatrixOpsAndFunctions(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestMatrixOpsAndFunctions(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test Math Operation and Function with Array");
   
   std::string expstr;
   Real expRealVal;
   Rmatrix unsetMat;
   MathNode *node = NULL;
   
   Rmatrix matA(3, 3, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0);
   Rmatrix matB(3, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
   Rmatrix33 expMat1(11, 22, 33, 44, 55, 66, 77, 88, 99);
   
   expstr = "arrA+arrB";
   Array *arrA = new Array("arrA");
   Array *arrB = new Array("arrB");
   arrA->SetSize(3,3);
   arrB->SetSize(3,3);
   arrA->SetRmatrixParameter("RmatValue", matA);
   arrB->SetRmatrixParameter("RmatValue", matB);
   out.Put("arrA =\n", arrA->GetRmatrixParameter("RmatValue"));
   out.Put("arrB =\n", arrB->GetRmatrixParameter("RmatValue"));
   out.Put(expstr + " should return\n", expMat1);
   node = mp.Parse(expstr);
   SetParameters(node, "arrA", arrA, "arrB", arrB);      
   EvaluateNode(node, out, expRealVal, expMat1);
   delete node;
   
   //------------------------------
   expstr = "-arrA";
   Rmatrix expMatNegate = -matA;
   out.Put(expstr + " should return\n", expMatNegate);
   node = mp.Parse(expstr);
   SetParameters(node, "arrA", arrA, "arrB", arrB);      
   EvaluateNode(node, out, expRealVal, expMatNegate);
   delete node;
   
   //------------------------------
   expstr = "transpose(-5)";
   expRealVal = -5.0;
   out.Put(expstr + " should return\n", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "transpose(arrA)";
   Rmatrix expMat3 = matA.Transpose();
   out.Put(expstr + " should return\n", expMat3);
   node = mp.Parse(expstr);
   SetParameters(node, "arrA", arrA, "", NULL);      
   EvaluateNode(node, out, expRealVal, expMat3);
   delete node;
   
   //------------------------------
   expstr = "arrA'";
   out.Put(expstr + " should return\n", expMat3);
   node = mp.Parse(expstr);
   SetParameters(node, "arrA", arrA, "", NULL);      
   EvaluateNode(node, out, expRealVal, expMat3);
   delete node;
   
   //------------------------------
   expstr = "arrA' + arrA'";
   Rmatrix expMat32 = expMat3*2;
   out.Put(expstr + " should return\n", expMat32);
   node = mp.Parse(expstr);
   SetParameters(node, "arrA", arrA, "arrA", arrA);      
   EvaluateNode(node, out, expRealVal, expMat32);
   delete node;
   
   //------------------------------
   //@note SetParameters() assumes input matrix is arrI
   expstr = "det(arrI)";
   Array *arrI = new Array("arrI");
   arrI->SetSize(3,3);
   Rmatrix matI(3, 3, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
   arrI->SetRmatrixParameter("RmatValue", matI);
   out.Put("arrI =\n", arrI->GetRmatrixParameter("RmatValue"));
   expRealVal = 1.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   SetParameters(node, "arrI", arrI, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "inv(arrI)";
   out.Put(expstr + " should return\n", arrI);
   node = mp.Parse(expstr);
   SetParameters(node, "arrI", arrI, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, matI);
   delete node;
   
   //------------------------------
   expstr = "arrI^(-1)";
   out.Put(expstr + " should return\n", arrI);
   node = mp.Parse(expstr);
   SetParameters(node, "arrI", arrI, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, matI);
   delete node;
   
   //------------------------------
   expstr = "arrI^(-1) + arrI^(-1)";
   Rmatrix expMatI2 = matI*2;
   out.Put(expstr + " should return\n", expMatI2);
   node = mp.Parse(expstr);
   SetParameters(node, "arrI", arrI, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, expMatI2);
   delete node;
   
   //------------------------------
   expstr = "arrI^(-1) * arrI^(-1)";
   out.Put(expstr + " should return\n", matI);
   node = mp.Parse(expstr);
   SetParameters(node, "arrI", arrI, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, matI);
   delete node;
   
   //------------------------------
   expstr = "arrI^(-1) - arrI^(-1)";
   Rmatrix mat0 = matI - matI;
   out.Put(expstr + " should return\n", mat0);
   node = mp.Parse(expstr);
   SetParameters(node, "arrI", arrI, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, mat0);
   delete node;
   
   //------------------------------
   expstr = "arrI^(-1) + arrI^(-1) - arrI^(-1)";
   out.Put(expstr + " should return\n", matI);
   node = mp.Parse(expstr);
   SetParameters(node, "arrI", arrI, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, matI);
   delete node;
   
   //------------------------------
   expstr = "norm(arrC)";
   Array *arrC = new Array("arrC");
   arrC->SetSize(1,4);
   Rmatrix matC(1, 4, 0.0, 1.0, 2.0, 3.0);
   arrC->SetRmatrixParameter("RmatValue", matC);
   out.Put("arrC =\n", arrC->GetRmatrixParameter("RmatValue"));
   expRealVal = 3.74165738677394;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   SetParameters(node, "arrC", arrC, "", NULL);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete arrC;
   delete node;
   
   //------------------------------
   expstr = "arrI*arrI";
   out.Put("arrI =\n", arrA->GetRmatrixParameter("RmatValue"));
   Rmatrix expMat4 = matI;
   out.Put(expstr + " should return\n", matI);
   node = mp.Parse(expstr);
   SetParameters(node, "arrI", arrI, "arrI", arrI);      
   EvaluateNode(node, out, expRealVal, expMat4);
   delete node;
   
   //------------------------------
   expstr = "(norm(arrC) + det(arrI)) * arrI";
   expMat3 = 4.7416573867739409 * matI;
   out.Put(expstr + " should return\n", expMat3);
   node = mp.Parse(expstr);
   SetParameters(node, "arrC", arrC, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, expMat3);
   delete node;
   
   //------------------------------
   expstr = "norm(arrC) + det(arrI * arrI)";
   expRealVal = 4.7416573867739409;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   SetParameters(node, "arrC", arrC, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "(transpose(arrA + arrB)) * 1.0";
   Rmatrix expMat5 = expMat1.Transpose();
   out.Put(expstr + " should return\n", expMat5);
   node = mp.Parse(expstr);
   SetParameters(node, "arrA", arrA, "arrB", arrB);      
   EvaluateNode(node, out, expRealVal, expMat5);
}


//------------------------------------------------------------------------------
// void TestLongEquations(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestLongEquations(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test long equations");
   
   std::string expstr;
   bool boolVal;
   bool expBoolVal;
   MathNode *node = NULL;
   
   //------------------------------
   expstr = "sin(  abs(-.5) + acos(.5) - asin(.5)*atan(.5)*atan2(.5, .5) - "
      "cos(.02) / DegToRad(45) - det(ArrayOut22)^exp(.5 ) + log(.5) - "
      "norm(Array31) - RadToDeg(pi/4) + sqrt(2) + tan(2) );";
   //expstr = "sin(  abs(-.5) + acos(.5) - asin(.5)*atan(.5)*atan2(.5, .5) );";
   //expstr = "sin(  abs(-.5) + acos(.5) );"; // OK
   //expstr = "sin(  abs(-.5) + acos(.5) - acos(.5) );"; // OK
   //expstr = "sin(  abs(-.5) + acos(.5) - asin(.5) );";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   node = mp.Parse(expstr);
}


//------------------------------------------------------------------------------
// void TestJustParsing(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestJustParsing(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test just parsing");
   
   std::string expstr;
   bool boolVal;
   bool expBoolVal;
   MathNode *node = NULL;
   
   //------------------------------
   expstr = "cos(phi)*I3 + (1 - cos(phi))*av*av' - sin(phi)*across";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "sv2(2,1) * sv3(3,1) - sv2(3,1) * sv3(2,1)";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "ACE.VX - CurrentV(1,1)";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "EarthSat.ECC*sin( EarthSat.AOP )"; 
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "( ( v^2 - mu/r )*rv - rdotv*vv  ) / mu";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "( VX - MarsL1Sat.MarsFK5.VX )^2 + ( VY - MarsL1Sat.MarsFK5.VY )^2 + "
      "( VZ - MarsL1Sat.MarsFK5.VZ )^2";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "nv(1,1)*ev(1,1) + nv(2,1)*ev(2,1) + nv(3,1)*ev(3,1) ";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "(cnu + e)*sqrtmup";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "sqrtmup*(cnu + e)";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "vec(1,1)*vec(2,1)*(vec(3,1)*vec(4,1)) - 10.9056168";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "(vec(1,1)*vec(2,1))*vec(3,1)*vec(4,1) - 10.9056168";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "vec(1,1)*vec(4,1)/(vec(3,1)*vec(2,1)) - 9.01552616446233*10^(-9)";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "(vec(1,1)*vec(4,1)*vec(3,1))/vec(2,1) - 1.09056168*10^(-5)";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "a*vec(2,1)/(Sat.Z*.000134)/1000000 -  0.502089895548136";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "acos( nv(1,1)/n )";
   node = mp.Parse(expstr);
         
   //------------------------------
   expstr = "(a)*b*c*d - 10.9056168";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "( (3*a+4)-(9*b-20)*(cos(c)^2) )*(-a/b)*d - 0.00267522370194881";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "dummyVar = DefaultSC.TA + 1";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "acos(sv1'*SpinVector/S1)*180;";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "acos(sv1'*SpinVector/S1)*180/pi;";
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "acos( xxx )";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   node = mp.Parse(expstr);
   
   //------------------------------
   expstr = "sin(INC*d2r)^2;";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put(expstr + " should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   node = mp.Parse(expstr);
   
   //------------------------------
   try
   {
      expstr = "cross(vv, cross(rv, vv));";
      expBoolVal = true;
      boolVal = mp.IsEquation(expstr, false);
      out.Put(expstr + " should return ", expBoolVal);
      out.Validate(boolVal, expBoolVal);
      node = mp.Parse(expstr);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
      out.Put(be.GetFullMessage() + "\n");
   }
}


//------------------------------------------------------------------------------
// void TestFunctionRunner(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestFunctionRunner(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test FunctionRunner");
   
   std::string expstr;
   Real expRealVal;
   Rmatrix unsetMat;
   MathNode *node = NULL;
   
   /*
   //------------------------------
   expstr = "Sqrt(1+2+3)";
   expRealVal = sqrt(1+2+3);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   
   //------------------------------
   expstr = "Factorial(1+2+3)";
   expRealVal = 720;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   //EvaluateNode(node, out, expRealVal, unsetMat);
   //delete node;
   
   //------------------------------
   expstr = "Sqrt(Times2(1))";
   expRealVal = sqrt(2);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   //EvaluateNode(node, out, expRealVal, unsetMat);
   //delete node;
   */
   
   //------------------------------
   try
   {
      expstr = "Times(1, 2)";
      expRealVal = 2;
      out.Put(expstr + " should return ", expRealVal);
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
      delete node;
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage() + "\n");
   }
   
   //------------------------------
   try
   {
      expstr = "Times(Sqrt(1), 10)";
      expRealVal = 20;
      out.Put(expstr + " should return ", expRealVal);
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
      delete node;
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage() + "\n");
   }
   
   //------------------------------
   try
   {
      expstr = "FindMax3(a, b, c)";
      expRealVal = 10;
      out.Put(expstr + " should return ", expRealVal);
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
      delete node;
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage() + "\n");
   }
   
   //------------------------------
   try
   {
      expstr = "FindMax3(Sqrt(1), 10, 5)";
      expRealVal = 10;
      out.Put(expstr + " should return ", expRealVal);
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
      delete node;
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage() + "\n");
   }
}


//------------------------------------------------------------------------------
// void TestSpecialCase(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestSpecialCase(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test special case");
   
   std::string expstr;
   Integer opIndex;
   std::string str1;
   Real expRealVal;
   Rmatrix unsetMat;
   MathNode *node = NULL;
   
   //------------------------------
   expstr = "200*1000^(-1)";
   str1 = mp.FindLowestOperator(expstr, opIndex);
   out.Put(expstr);
   out.Validate(str1, "*");
   out.Validate(opIndex, 3);
   expRealVal = 0.2;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
}


//------------------------------------------------------------------------------
// void TestSpecialParsing(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestSpecialParsing(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test special parsing");
   
   std::string expstr;
   Real expRealVal;
   bool boolVal;
   bool expBoolVal;
   MathNode *node = NULL;
   Rmatrix unsetMat;
   Array *arrI = NULL;
   
   
   #if 1
   expstr = "atan2(arrLHSArg(3,3),arrRHSArg(3,3))";
   node = mp.Parse(expstr);
   delete node;
   #endif
   
   
   #if 1
   try
   {
      expstr = "atan2(a,b,c)";
      node = mp.Parse(expstr);
      delete node;
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage() + "\n");
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
   }
   #endif
   
   
   #if 1
   try
   {
      expstr = "atan2(arrLHSArg(3,3),)";
      node = mp.Parse(expstr);
      delete node;
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage() + "\n");
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
   }
   #endif
   
   
   #if 1
   try
   {
      expstr = "atan2(arrLHSArg(3,3)+1234.123)";
      node = mp.Parse(expstr);
      delete node;
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage() + "\n");
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
   }
   
   try
   {
      expstr = "atan2(arrLHSArg(3,3)arrRHSArg(3,3))";
      node = mp.Parse(expstr);
      delete node;
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage() + "\n");
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
   }
   #endif

   
   #if 1
   //------------------------------
   expstr = "Rad2Deg(-6.283185307179586)";
   node = mp.Parse(expstr);
   delete node;
   
   //------------------------------
   expstr = "2+3+4";
   node = mp.Parse(expstr);
   delete node;
   
   //------------------------------
   expstr = "2.0e-1 + 3.0e-1 + 4.0e+0";
   node = mp.Parse(expstr);
   delete node;
   
   //------------------------------
   expstr = "(rv'*vv)*vv";
   node = mp.Parse(expstr);
   delete node;
   
   //------------------------------
   expstr = "((v^2 - mu/r)*rv - (rv'*vv)*vv)/mu";
   node = mp.Parse(expstr);
   delete node;
   
   //------------------------------
   expstr = "2^3^4";
   node = mp.Parse(expstr);
   delete node;
   
   //------------------------------
   expstr = "-218.6  /  -248.715095169  / (   -209.5774  /  -132.61614521353   )   ";
   node = mp.Parse(expstr);
   delete node;
   
   //------------------------------
   expstr = "count + 1";
   node = mp.Parse(expstr);
   delete node;
   
   //------------------------------
   expstr = "-(-0.001008965327910524)^869.28";
   node = mp.Parse(expstr);
   delete node;
   #endif
   
   //------------------------------
   //@note SetParameters() assumes input matrix is arrI
   #if 1
   expstr = "norm(arrI)";
   arrI = new Array("arrI");   
   arrI->SetSize(1,4);
   Rmatrix matI(1, 4, 0.0, 1.0, 2.0, 3.0);
   arrI->SetRmatrixParameter("RmatValue", matI);
   out.Put("arrI =\n", arrI->GetRmatrixParameter("RmatValue"));
   expRealVal = 3.74165738677394;
   out.Put("IsEquation(" + expstr + ") should return ", expRealVal);
   node = mp.Parse(expstr);
   SetParameters(node, "arrI", arrI, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete arrI;
   delete node;
   #endif
   
   #if 1
   //------------------------------
   expstr = "norm(RadToDeg(DegToRad(exp(log10(-log(atan(acos(asin(tan(cos(sin(3)))/( 1.5239+ 10 )))/( 1.4378+ 10 ))))))))";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put("IsEquation(" + expstr + "} should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   node = mp.Parse(expstr);
   expRealVal = 1.374192020424485;
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   #endif
   
   #if 1
   //------------------------------
   expstr = "norm(2.3)";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put("IsEquation(" + expstr + ") should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   node = mp.Parse(expstr);
   expRealVal = 2.3;
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   #endif

   #if 1
   //------------------------------
   expstr = "det(3.5)";
   expBoolVal = true;
   boolVal = mp.IsEquation(expstr, false);
   out.Put("IsEquation(" + expstr + ") should return ", expBoolVal);
   out.Validate(boolVal, expBoolVal);
   node = mp.Parse(expstr);
   expRealVal = 3.5;
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "inv(((arrI)))";
   arrI = new Array("arrI");
   arrI->SetSize(3,3);
   Rmatrix matII(3, 3, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
   arrI->SetRmatrixParameter("RmatValue", matII);
   out.Put("arrI =\n", arrI->GetRmatrixParameter("RmatValue"));
   out.Put(expstr + " should return\n", matII);
   node = mp.Parse(expstr);
   SetParameters(node, "arrI", arrI, "arrI", arrI);
   EvaluateNode(node, out, expRealVal, matII);
   delete arrI;
   delete node;
   #endif
   
   #if 1
   //------------------------------
   expstr = "(inv(arrI))^2"; // works!
   node = mp.Parse(expstr);
   delete node;
   expstr = "(inv(arrI))'";  // works!
   node = mp.Parse(expstr);
   delete node;
   expstr = "inv(  (inv(arrI))' )"; // works!
   node = mp.Parse(expstr);
   delete node;
   expstr = "inv(((arr_22*arr_22)+(inv((arr_22*arr_23*arr_32))' ))' -inv(((arr_23*arr_32)' )))"; // works!
   node = mp.Parse(expstr);
   delete node;
   #endif
   
   #if 1
   //------------------------------
   expstr = "inv(arr_55 - arr_51*arr_15)'";
   node = mp.Parse(expstr);
   delete node;
   #endif
   
   #if 1
   //------------------------------
   expstr = "degToRad(exp(log10(-log(atan(acos(asin(tan(cos(sin(3)))/( 1.5239+ 10 )))/( 1.4378+ 10 ))))));";
   node = mp.Parse(expstr);
   delete node;
   #endif

   // Not working
//    #if 1
//    expstr = "inv(((arr_22*arr_22)+(inv((arr_22*arr_23*arr_32))' ))' -inv(((arr_23*arr_32)' )))";
//    arrI = new Array("arrI");
//    arrI->SetSize(3,3);
//    Rmatrix matIa(3, 3, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
//    arrI->SetRmatrixParameter("RmatValue", matIa);
//    out.Put("arrI =\n", arrI->GetRmatrixParameter("RmatValue"));
//    out.Put(expstr + " should return\n", matIa);
//    node = mp.Parse(expstr);
//    SetParameters(node, "arrI", arrI, "arrI", arrI);
//    EvaluateNode(node, out, expRealVal, matIa);
//    delete arrI;
//    delete node;
//    #endif
}


//------------------------------------------------------------------------------
// void TestParsingLongEquation(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestParsingLongEquation(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test parsing long equation");
   
   std::string expstr;
   MathNode *node = NULL;
   
   #if 1
   try
   {
      expstr = "((ABC11)^((-(det(arr99(1,1))))/(((sat1.X)/((cos((atan(log((-(sin((((exp(var2)))/(log10(-(-arrA11)/((+sat2.X))))))))))''))))))))";
      node = mp.Parse(expstr);
      delete node;
   }
   catch (BaseException &be)
   {
      out.Put(be.GetFullMessage() + "\n");
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
   }
   #endif
   
}


//------------------------------------------------------------------------------
// void TestMathFunctions(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestMathFunctions(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test BuiltInMathFunctions");
   
   std::string expstr;
   Real expRealVal;
   Rmatrix unsetMat;
   MathNode *node = NULL;
   
   //------------------------------
   expstr = "-tan(11.907)  + 1.47756418563724";
   expRealVal = -tan(11.907)  + 1.47756418563724;
   out.Put(expstr + " should return\n", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "transpose(-5)";
   expRealVal = -5.0;
   out.Put(expstr + " should return\n", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   // Not working since Math uses ElementWrappers.
//    //------------------------------
//    Rmatrix matA(1, 1, 10.0);
//    Array *arrA = new Array("arrA");
//    arrA->SetSize(1,1);
//    expstr = "transpose(arrA)";
//    Rmatrix expMatA = matA.Transpose();
//    out.Put(expstr + " should return\n", expMatA);
//    node = mp.Parse(expstr);
//    SetParameters(node, "arrA", arrA, "", NULL);      
//    EvaluateNode(node, out, expRealVal, expMatA);
//    delete node;
   
   //------------------------------
   expstr = "norm(-45)";
   expRealVal = 45.0;
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
   //------------------------------
   expstr = "Rad2Deg(-6.283185307179586)";
   expRealVal = GmatMathUtil::RadToDeg(-6.283185307179586);
   out.Put(expstr + " should return ", expRealVal);
   node = mp.Parse(expstr);
   EvaluateNode(node, out, expRealVal, unsetMat);
   delete node;
   
}


//------------------------------------------------------------------------------
// void TestValidation(TestOutput &out, MathParser &mp)
//------------------------------------------------------------------------------
void TestValidation(TestOutput &out, MathParser &mp)
{
   out.Put("============================== Test Validation");
   
   std::string expstr;
   Real expRealVal;
   Rmatrix unsetMat;
   MathNode *node = NULL;

   #if 0
   try
   {
      expstr = "-norm(2,3)";
      out.Put(expstr + " should throw an exception");
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
      out.Put(be.GetFullMessage() + "\n");
   }
   #endif
   
   #if 0
   try
   {
      expstr = "-norm(1+2, 3+4)";
      out.Put(expstr + " should throw an exception");
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
      out.Put(be.GetFullMessage() + "\n");
   }
   
   try
   {
      expstr = "-norm()";
      out.Put(expstr + " should throw an exception");
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
      out.Put(be.GetFullMessage() + "\n");
   }
   
   try
   {
      expstr = "-Atan2()";
      out.Put(expstr + " should throw an exception");
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
      out.Put(be.GetFullMessage() + "\n");
   }
   
   try
   {
      expstr = "Atan2(1+2, 2+3, 3+4)";
      out.Put(expstr + " should throw an exception");
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
      out.Put(be.GetFullMessage() + "\n");
   }
   #endif
   
   
   #if 0
   try
   {
      expstr = "123 + ";
      out.Put(expstr + " should throw an exception");
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
      out.Put(be.GetFullMessage() + "\n");
   }
   
   try
   {
      expstr = "123 / ";
      out.Put(expstr + " should throw an exception");
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
      out.Put(be.GetFullMessage() + "\n");
   }
   
   try
   {
      expstr = "* 123";
      out.Put(expstr + " should throw an exception");
      node = mp.Parse(expstr);
      EvaluateNode(node, out, expRealVal, unsetMat);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage(be.GetFullMessage() + "\n");
      out.Put(be.GetFullMessage() + "\n");
   }

   #endif
}


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{   
   //---------------------------------------------------------------------------
   out.Put("======================================== Test TestMathParser\n");
   //---------------------------------------------------------------------------
   
   MathParser mp = MathParser();
   
   try
   {
      #if 0
      TestFindLowestOperator(out, mp);
      #endif
      
      #if 0
      TestSpecialParsing(out, mp);
      #endif
      
      #if 1
      TestParsingLongEquation(out, mp);
      #endif
      
      #if 0
      TestOpsWithNumber(out, mp);
      #endif
      
      #if 0
      TestMathFunctions(out, mp);
      #endif
      
      #if 0
      TestValidation(out, mp);
      #endif
      
      #if 0
      TestIsEquation(out, mp);
      #endif
      
      #if 0
      TestFunctionWithNumber(out, mp);
      #endif
      
      #if 0
      TestOpsWithNumberWithParen(out, mp);
      TestOpsWithMatrix(out, mp);
      TestLongEquations(out, mp);
      TestJustParsing(out, mp);
      TestSpecialCase(out, mp);
      #endif
      
      #if 0
      ////TestVariable(out, mp); // currently not working due to NULL ElementWrapper
      ////TestMatrixOpsAndFunctions(out, mp);    // currently not working due to NULL ElementWrapper
      ////TestFunctionRunner(out, mp); // currently not working due to NULL Function
      #endif
      
      // Just testing StringArray
      #if 0
      StringArray names1;
      StringArray names12;
      names1.push_back("aaa");
      names1.push_back("bbb");
      names1.push_back("ccc");
      names1.push_back("ddd");
      
      StringArray::iterator pos;
      MessageInterface::ShowMessage("========== original names\n");
      for (pos = names1.begin(); pos != names1.end(); ++pos)
         MessageInterface::ShowMessage("  pos='%s'\n", (*pos).c_str());
      
      names12 = names1;
      pos = find(names1.begin(), names1.end(), "ccc");
      if (pos != names1.end())
      {
         names1.insert(pos, names12[0]);
         MessageInterface::ShowMessage("========== names after inserting aaa before ccc\n");
         for (pos = names1.begin(); pos != names1.end(); ++pos)
            MessageInterface::ShowMessage("  pos='%s'\n", (*pos).c_str());
      }
      #endif
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage() + "\n");
      out.Put(e.GetFullMessage() + "\n");
      throw MathException("\n>>>>> Unit testing of MathParser was Unsuccessful!!");
   }
   
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
   std::string outFile = outPath + "TestMathParserOut.txt";
   TestOutput out(outFile);
   out.Put(GmatTimeUtil::FormatCurrentTime());
   MessageInterface::ShowMessage("%s\n", GmatTimeUtil::FormatCurrentTime().c_str());
   
   // Set global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);
   
   // Initialize global variable
   GmatTest::ewI = new ArrayWrapper();
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of MathParser!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage() + "\n");
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }

   // clean up
   delete GmatTest::ewI;
   out.Close();
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
