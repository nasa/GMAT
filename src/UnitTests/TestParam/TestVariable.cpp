//------------------------------------------------------------------------------
//                                  TestVariable
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
// Created: 2005/07/15
//
// Modifications:
//
/**
 * Test driver for Variable, Array, String
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"

#include "Variable.hpp"
#include "Array.hpp"
#include "StringVar.hpp"

#include "ParameterException.hpp"
#include "StringTokenizer.hpp"
#include "MessageInterface.hpp"
#include "TestOutput.hpp"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   const std::string *descs;
   std::string *vals;
   Integer size;
   bool boolVal;
   Real realVal;
   Real expVal;      
   
   MessageInterface::SetLogFile("../../test/TestParam/GmatLog.txt");
   MessageInterface::ShowMessage("=========== TestVariable\n");
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test Variable\n");
   //---------------------------------------------------------------------------

   std::string exp;
   
   // test varG = 10.123
   // for varG
   Variable *varG = new Variable("varG");
   varG->SetStringParameter("Expression", "10.123");
   varG->SetRefObjectName(Gmat::PARAMETER, "varG");   // used during script parsing
   varG->SetRefObject(varG, Gmat::PARAMETER, "varG"); // used in Sandbox::Initialize()
   out.Put("----- varG exp = ", varG->GetStringParameter("Expression"));
   realVal = varG->EvaluateReal();
   out.Put("-> varG should return 10.123");
   out.Validate(realVal, 10.123);
   
   out.Put("----- now set parameter to 100.345");
   varG->SetReal(100.345);
   realVal = varG->EvaluateReal();
   out.Put("-> varG should return 100.345");
   out.Validate(realVal, 100.345);
   
   // for varA
   Variable *varA = new Variable("varA");
   varA->SetStringParameter("Expression", "123.123");
   varA->SetRefObjectName(Gmat::PARAMETER, "varA");   // used during script parsing
   varA->SetRefObject(varA, Gmat::PARAMETER, "varA"); // used in Sandbox::Initialize()
   out.Put("----- varA exp = ", varA->GetStringParameter("Expression"));
   realVal = varA->EvaluateReal();
   out.Put("-> varA should have 123.123");
   out.Validate(realVal, 123.123);
   
   // for varB
   Variable *varB = new Variable("varB");
   varB->SetStringParameter("Expression", "345.345");
   varB->SetRefObjectName(Gmat::PARAMETER, "varB");   // used during script parsing
   varB->SetRefObject(varB, Gmat::PARAMETER, "varB"); // used in Sandbox::Initialize()
   out.Put("----- varB exp = ", varB->GetStringParameter("Expression"));
   realVal = varB->EvaluateReal();
   out.Put("varB should have 345.345");
   out.Validate(realVal, 345.345);

   // for varA - use SetRealParameter() to set varB + 1000 to varA (1345.345)
   //varA->SetRealParameter("Value", varB->EvaluateReal() + 1000);
   varA->SetReal(varB->EvaluateReal() + 1000);
   realVal = varA->EvaluateReal();
   out.Put("Set varA to varB+1000, new varA = ", realVal); out.Put("");
   out.Validate(realVal, 1345.345);
   
   // for varA - use expression to set "varB + 1000" to varA
   exp = "varB + 1000";
   varA->SetStringParameter("Expression", exp);
   varA->SetRefObjectName(Gmat::PARAMETER, "varB");   // used during script parsing
   varA->SetRefObject(varB, Gmat::PARAMETER, "varB"); // used in Sandbox::Initialize()
   out.Put("----- varA exp = ", varA->GetStringParameter("Expression"));
   realVal = varA->EvaluateReal();
   out.Put("varA->EvaluateReal() = ", realVal); out.Put("");
   out.Validate(realVal, 1345.345);

   // for varC
   // use expression to set varC
   Variable *varC = new Variable("varC");
   exp = "varA * 10 + varB + 10"; // 1345.345 * 10 + 345.345 + 10 = 13808.795
   varC->SetStringParameter("Expression", exp);
   out.Put("----- varC exp = ", varC->GetStringParameter("Expression"));
   varC->SetRefObjectName(Gmat::PARAMETER, "varA");   // used during script parsing
   varC->SetRefObjectName(Gmat::PARAMETER, "varB");   // used during script parsing
   varC->SetRefObject(varA, Gmat::PARAMETER, "varA"); // used in Sandbox::Initialize()
   varC->SetRefObject(varB, Gmat::PARAMETER, "varB"); // used in Sandbox::Initialize()
   realVal = varC->EvaluateReal();   
   out.Put("varC->EvaluateReal() = ", realVal); out.Put("");
   out.Validate(realVal, 13808.795);

   // use StringTokenizer to parse ref. parameters
   Variable *varD = new Variable("varD");
   out.Put("----- use StringTokenizer to parse ref. parameters");
   exp = "varA * (varB + 10)";
   expVal = 1345.345 * (345.345 + 10);
   varD->SetStringParameter("Expression", exp);
   out.Put("----- varD exp = ", varD->GetStringParameter("Expression"));
   
   StringTokenizer st(exp, "()*/+-^ ");
   StringArray tokens = st.GetAllTokens();

   for (unsigned int i=0; i<tokens.size(); i++)
   {
      out.Put("token:<" + tokens[i] + ">");
      
      if (!isdigit(*tokens[i].c_str()))
         varD->SetRefObjectName(Gmat::PARAMETER, tokens[i]);
   }
   
   varD->SetRefObject(varA, Gmat::PARAMETER, "varA"); // used in Sandbox::Initialize()
   varD->SetRefObject(varB, Gmat::PARAMETER, "varB"); // used in Sandbox::Initialize()
   realVal = varD->EvaluateReal();   
   out.Put("varD->EvaluateReal() = ", realVal); out.Put("");
   out.Validate(realVal, expVal);
   
   // test varH = 10.123
   // for varH
   Variable *varH = new Variable("varH");
   varH->SetStringParameter("Expression", ".567");
   varH->SetRefObjectName(Gmat::PARAMETER, "varH");   // used during script parsing
   varH->SetRefObject(varH, Gmat::PARAMETER, "varH"); // used in Sandbox::Initialize()
   out.Put("----- varH exp = ", varH->GetStringParameter("Expression"));
   realVal = varH->EvaluateReal();
   out.Put("-> varH should return .567");
   out.Validate(realVal, .567);


   // for varI
   std::string str = ".5^2 + .6^2 * 2 + .5";
   expVal = 1.47;
   Variable *varI = new Variable("varI");
   varI->SetStringParameter("Expression", str);
   out.Put("----- varI exp = ", varI->GetStringParameter("Expression"));
   realVal = varI->EvaluateReal();
   out.Put("-> varH should return ", expVal);
   out.Validate(realVal, expVal);

   
   //str = "10^.5";
   //expVal = 3.16227766016838;
   str = "3^5";
   expVal = 243.0;
   varI->SetStringParameter("Expression", str);
   out.Put("----- varI exp = ", varI->GetStringParameter("Expression"));
   realVal = varI->EvaluateReal();
   out.Put("-> varH should return ", expVal);
   out.Validate(realVal, expVal);

   str = "(10+20)^.5";
   expVal = 5.47722557505166;
   varI->SetStringParameter("Expression", str);
   out.Put("----- varI exp = ", varI->GetStringParameter("Expression"));
   realVal = varI->EvaluateReal();
   out.Put("-> varH should return ", expVal);
   out.Validate(realVal, expVal);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test varH->Clone()\n");
   //---------------------------------------------------------------------------
   Variable *varH1 = (Variable*)varH->Clone();
   realVal = varH1->EvaluateReal();
   out.Put("-> varH1 should return .567");
   out.Validate(realVal, .567);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test Array\n");
   //---------------------------------------------------------------------------

   Array *arrA = new Array("arrA");
   arrA->SetIntegerParameter("NumRows", 5);
   arrA->SetIntegerParameter("NumCols", 3);
   out.Put("arrA->GetRmatrix() = \n", arrA->GetRmatrix());

   Rvector row1Vec(3, 2.1, 2.2, 2.3);
   out.Put("row1Vec ="); out.Put(row1Vec);
   out.Put("arrA->SetRvectorParameter('RowValue', row1Vec, 1");
   arrA->SetRvectorParameter("RowValue", row1Vec, 1);
   out.Put("arrA->GetRmatrix() = "); out.Put(arrA->GetRmatrix());
   
   Rvector col2Vec(5, 1.3, 2.3, 3.3, 4.3, 5.3);
   out.Put("col2Vec =\n", col2Vec);
   out.Put("arrA->SetRvectorParameter('ColValue', col2Vec, 2)");
   arrA->SetRvectorParameter("ColValue", col2Vec, 2);
   out.Put("arrA->GetRmatrix() ="); out.Put(arrA->GetRmatrix());
   
   Rvector row2Vec(3, 1.0, 2.0, 3.0); // real number must have ., otherwise it will not work
   out.Put("row2Vec ="); out.Put(row2Vec);
   out.Put("arrA->SetRvectorParameter('RowValue', row2Vec, 2)");
   arrA->SetRvectorParameter("RowValue", row2Vec, 2);
   out.Put("arrA->GetRmatrix() ="); out.Put(arrA->GetRmatrix());

   out.Put("==================== test exception");
   Array *arrB = new Array("arrB");
   arrB->SetIntegerParameter("NumRows", 3);
   try
   {
      out.Put("Set NumRows again");
      arrB->SetIntegerParameter("NumRows", 3);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   
   try
   {
      out.Put("arrB->GetRmatrix() =");
      arrB->GetRmatrix();
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   arrB->SetIntegerParameter("NumCols", 5);
   out.Put("arrB->GetRmatrix() ="); out.Put(arrB->GetRmatrix());
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test StringVar\n");
   //---------------------------------------------------------------------------
   StringVar *str1 = new StringVar("str1");
   str1->SetStringParameter("Expression", "My Test String");
   out.Put("str1->GetString() = ", str1->GetString());
   out.Put("str1->GetStringParameter('Expression') = ",
       str1->GetStringParameter("Expression"));
   StringVar *str2 = new StringVar("str1");
   out.Put("testing str2 = (StringVar*)str1->Clone()");
   str2 = (StringVar*)str1->Clone();
   out.Put("str2->GetStringParameter('Expression') = ",
       str2->GetStringParameter("Expression"));
   
   //---------------------------------------------
   // clean up
   //---------------------------------------------
   delete varA;
   delete varB;
   delete varC;
   delete varD;
   delete arrA;
   delete arrB;
   delete str1;
}

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("..\\..\\Test\\TestParam\\TestVariableOut.txt");
   out.SetPrecision(16);
   out.SetWidth(20);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of parameters!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   out.Close();
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
