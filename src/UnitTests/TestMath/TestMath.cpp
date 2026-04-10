//------------------------------------------------------------------------------
//                                  TestMath
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
// Author: LaMont Ruley
// Created: 2006.04.14
//
/**
 * Test driver for math functions
 */
//------------------------------------------------------------------------------
#include <iostream>
#include <string>

#include "MathNode.hpp"
#include "MathElement.hpp"
#include "Add.hpp"
#include "Subtract.hpp"
#include "Multiply.hpp"
#include "Divide.hpp"
#include "Sqrt.hpp"
#include "Power.hpp"
#include "Exp.hpp"
#include "Log.hpp"
#include "Log10.hpp"
#include "Transpose.hpp"
#include "Determinant.hpp"
#include "Inverse.hpp"
#include "Norm.hpp"
#include "Sin.hpp"
#include "Cos.hpp"
#include "Tan.hpp"
#include "Asin.hpp"
#include "Acos.hpp"
#include "Atan.hpp"
#include "Atan2.hpp"
#include "DegToRad.hpp"
#include "RadToDeg.hpp"
#include "Array.hpp"
#include "Rmatrix.hpp"
#include "Sprintf.hpp"
#include "TestOutput.hpp"

using namespace std;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   // Create Real element
   MathElement *element1 = new MathElement("MathElement", "element1");
   element1->SetRealValue(0.3);

   // Create Real element
   MathElement *element2 = new MathElement("MathElement", "element2");
   element2->SetRealValue(-4.00);

   // Create 3x3 Rmatrix element
   MathElement *element3 = new MathElement("MathElement", "element3");
   Array *array1 = new Array("testMatrix1", "Matrix1", "test1");
   array1->SetIntegerParameter("NumRows", 3);
   array1->SetIntegerParameter("NumCols", 3);
   
   Rmatrix mat1 (3, 3,
                1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);
   array1->SetRmatrixParameter("RmatValue", mat1);
   element3->SetRefObject(array1, Gmat::PARAMETER, "testMatrix1");

   // Create 3x3 Rmatrix element
   MathElement *element4 = new MathElement("MathElement", "element4");
   Array *array2 = new Array("testMatrix2", "Matrix2", "test2");
   array2->SetIntegerParameter("NumRows", 3);
   array2->SetIntegerParameter("NumCols", 3);
   
   Rmatrix mat2 (3, 3,
                1.5, 2.5, 3.5,
                4.5, 5.5, 6.5,
                7.5, 8.5, 9.5);
   array2->SetRmatrixParameter("RmatValue", mat2);
   element4->SetRefObject(array2, Gmat::PARAMETER, "testMatrix2");
      
   // Create 3x1 Rmatrix element
   MathElement *element5 = new MathElement("MathElement", "element5");
   Array *array3 = new Array("testMatrix3", "Matrix3", "test3");
   array3->SetIntegerParameter("NumRows", 3);
   array3->SetIntegerParameter("NumCols", 1);
   
   Rmatrix mat3 (3, 1,
                10.0,
                40.0,
                70.0);
   array3->SetRmatrixParameter("RmatValue", mat3);
   element5->SetRefObject(array3, Gmat::PARAMETER, "testMatrix3");

   // Create NULL element
   MathElement *element6 = new MathElement("MathElement", "element6");
   Array *array4 = new Array("testMatrix4", "Matrix4", "test4");
   Rmatrix *mat4 = new Rmatrix();
   mat4 = NULL;
//   array4->SetRmatrixParameter("RmatValue", mat4);
//   element6->SetRefObject(array4, Gmat::PARAMETER, "testMatrix4");
   
   out.Put("=============== Test Math Functions ===============");
   out.Put("real1 =  0.3");
   out.Put("real2 = -4.00");
   out.Put("mat1 = "); out.Put(array1->GetMatrix());
   out.Put("mat2 = "); out.Put(array2->GetMatrix());
   out.Put("mat3 = "); out.Put(array3->GetMatrix());

   out.Put("=============== Simple Math ===============");
   out.Put("========================= Add");
   Add *node1 = new Add("testAdd");

   try
   {
      node1->SetChildren(element1, element2);
      out.Put("real1 + real2 = ", node1->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node1->SetChildren(element1, element3);
      out.Put("real1 + mat1 = ");
      out.Put(node1->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node1->SetChildren(element3, element1);
      out.Put("mat1 + real1 = ");
      out.Put(node1->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node1->SetChildren(element3, element4);
      out.Put("mat1 + mat2 = ");
      out.Put(node1->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node1->SetChildren(element4, element5);
      out.Put("mat2 + mat3 = ");
      out.Put(node1->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
  
   out.Put("========================= Subtract");
   Subtract *node2 = new Subtract("testSubtract");
   try
   {
      node2->SetChildren(element1, element2);
      out.Put("real1 - real2 = ", node2->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node2->SetChildren(element1, element3);
      out.Put("real1 - mat1 = ");
      out.Put(node2->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   
   try
   {
      node2->SetChildren(element3, element1);
      out.Put("mat1 - real1 = ");
      out.Put(node2->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   
   try
   {
      node2->SetChildren(element3, element4);
      out.Put("mat1 - mat2 = ");
      out.Put(node2->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node2->SetChildren(element4, element5);
      out.Put("mat2 - mat3 = ");
      out.Put(node2->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Multiply");
   Multiply *node3 = new Multiply("testMultiply");
   try
   {
      node3->SetChildren(element1, element2);
      out.Put("real1 * real2 = ", node3->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   
   try
   {
      node3->SetChildren(element1, element3);
      out.Put("real1 * mat1 = ");
      out.Put(node3->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   
   try
   {
      node3->SetChildren(element1, element3);
      out.Put("mat1 * real1 = ");
      out.Put(node3->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   
   try
   {
      node3->SetChildren(element3, element4);
      out.Put("mat1 * mat2 = ");
      out.Put(node3->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node3->SetChildren(element4, element5);
      out.Put("mat2 * mat3 = ");
      out.Put(node3->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node3->SetChildren(element5, element4);
      out.Put("mat3 * mat2 = ");
      out.Put(node3->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Divide");
   Divide *node4 = new Divide("testDivide");
   try
   {
      node4->SetChildren(element1, element2);
      out.Put("real1 / real2 = ", node4->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node4->SetChildren(element1, element3);
      out.Put("real1 / mat1 = ");
      out.Put(node4->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   
   try
   {
      node4->SetChildren(element3, element1);
      out.Put("mat1 / real1 = ");
      out.Put(node4->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   
   try
   {
      node4->SetChildren(element3, element4);
      out.Put("mat1 / mat2 = ");
      out.Put(node4->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node4->SetChildren(element4, element5);
      out.Put("mat2 / mat3 = ");
      out.Put(node4->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Sqaure Root");
   Sqrt *node5 = new Sqrt("testSqrt1");
   try
   {
      node5->SetChildren(element1, element2);
      out.Put("Sqrt(real1) = ", node5->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node5->SetChildren(element2, element1);
      out.Put("Sqrt(real2) = ");
      out.Put(node5->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node5->SetChildren(element4, element5);
      out.Put("Sqrt(mat2) = ");
      out.Put(node5->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("");

   out.Put("=============== Power, Log functions ===============");
   out.Put("========================= Power");
   Power *node6 = new Power("testPower");
   try
   {
      node6->SetChildren(element1, element2);
      out.Put("Power(real1, real2) = ", node6->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node6->SetChildren(element3, element1);
      out.Put("Power(mat1, real1) = ", node6->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Exp");
   Exp *node7 = new Exp("testExp");
   try
   {
      node7->SetChildren(element1, element2);
      out.Put("Exp(real1) = ", node7->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node7->SetChildren(element3, element2);
      out.Put("Exp(mat1) = ");
      out.Put(node7->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Log");
   Log *node8 = new Log("testLog");
   try
   {
      node8->SetChildren(element1, element2);
      out.Put("Log(real1) = ", node8->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node8->SetChildren(element3, element2);
      out.Put("Log(mat1) = ");
      out.Put(node8->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Log10");
   Log10 *node9 = new Log10("testLog10");
   try
   {
      node9->SetChildren(element1, element2);
      out.Put("Log10(real1) = ", node9->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node9->SetChildren(element3, element2);
      out.Put("Log10(mat1) = ");
      out.Put(node9->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("");

   out.Put("=============== Matrix functions ===============");
   out.Put("========================= Transpose");
   Transpose *node10 = new Transpose("testTranspose");
   node10->SetChildren(element4, element5);
   try
   {
      node10->SetChildren(element1, element2);
      out.Put("Transpose(real1) = ", node10->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node10->SetChildren(element3, element2);
      out.Put("Transpose(mat1) = ");
      out.Put(node10->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Determinant");
   Determinant *node11 = new Determinant("testDeterminant");
   try
   {
      node11->SetChildren(element1, element2);
      out.Put("Determinant(real1) = ", node11->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node11->SetChildren(element3, element2);
      out.Put("Determinant(mat1) = ");
      out.Put(node11->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node11->SetChildren(element3, element4);
      out.Put("Determinant(mat1) = ");
      out.Put(node11->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Inv");
   Inverse *node12 = new Inverse("testInverse");
   try
   {
      node12->SetChildren(element1, element2);
      out.Put("Inverse(real1) = ", node12->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node12->SetChildren(element3, element2);
      out.Put("Inverse(mat1) = ");
      out.Put(node12->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Norm");
   Norm *node13 = new Norm("testNorm");
   node13->SetChildren(element5, element4);
   try
   {
      node13->SetChildren(element1, element2);
      out.Put("Norm(real1) = ", node13->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node13->SetChildren(element3, element2);
      out.Put("Norm(mat1) = ");
      out.Put(node13->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node13->SetChildren(element3, element2);
      out.Put("Norm(mat1) = ");
      out.Put(node13->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node13->SetChildren(element5, element4);
      out.Put("Norm(mat3) = ");
      out.Put(node13->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("");
   out.Put("=============== Trigonometric functions ===============");
   out.Put("========================= Sin");
   Sin *node14 = new Sin("testSin");
   try
   {
      node14->SetChildren(element1, element2);
      out.Put("Sin(real1) = ", node14->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node14->SetChildren(element3, element2);
      out.Put("Sin(mat1) = ");
      out.Put(node14->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Cos");
   Cos *node15 = new Cos("testCos");
   try
   {
      node15->SetChildren(element1, element2);
      out.Put("Cosine(real1) = ", node15->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node15->SetChildren(element3, element2);
      out.Put("Cosine(mat1) = ");
      out.Put(node15->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Tan");
   Tan *node16 = new Tan("testTan");
   try
   {
      node16->SetChildren(element1, element2);
      out.Put("Tangent(real1) = ", node16->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node16->SetChildren(element3, element2);
      out.Put("Tangent(mat1) = ");
      out.Put(node16->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Asin");
   Asin *node17 = new Asin("testAsin");
   try
   {
      node17->SetChildren(element1, element2);
      out.Put("Arc Sin(real1) = ", node17->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node17->SetChildren(element3, element2);
      out.Put("Arc Sin(mat1) = ");
      out.Put(node17->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Acos");
   Acos *node18 = new Acos("testAcos");
   try
   {
      node18->SetChildren(element1, element2);
      out.Put("Arc Cos(real1) = ", node18->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node18->SetChildren(element3, element2);
      out.Put("Arc Cos(mat1) = ");
      out.Put(node18->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Atan");
   Atan *node19 = new Atan("testAtan");
   try
   {
      node19->SetChildren(element1, element2);
      out.Put("Arc Tan(real1) = ", node19->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node19->SetChildren(element3, element2);
      out.Put("Arc Tan(mat1) = ");
      out.Put(node19->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= Atan2");
   Atan2 *node20 = new Atan2("testAtan2");
   try
   {
      node20->SetChildren(element1, element2);
      out.Put("Arc Tan2(real1, real2) = ", node20->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node20->SetChildren(element3, element4);
      out.Put("Arc Tan2(mat1, mat2) = ");
      out.Put(node20->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("");
   out.Put("=============== Unit conversion functions ===============");
   out.Put("========================= DegToRad");
   DegToRad *node21 = new DegToRad("testDegToRad");
   try
   {
      node21->SetChildren(element1, element2);
      out.Put("DegToRad(real1) = ", node21->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node21->SetChildren(element3, element4);
      out.Put("DegToRad(mat1) = ");
      out.Put(node21->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   out.Put("========================= RadToDeg");
   RadToDeg *node22 = new RadToDeg("testRadToDeg");
   try
   {
      node22->SetChildren(element1, element2);
      out.Put("RadToDeg(real1) = ", node22->Evaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   try
   {
      node22->SetChildren(element3, element4);
      out.Put("RadToDeg(mat1) = ");
      out.Put(node22->MatrixEvaluate());
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   return 0;
}

//------------------------------------------------------------------------------
//int RunSprintfTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunSprintfTest(TestOutput &out)
{
   int n;
   double vars[3];
   vars[0] = -1713.27528417579;
   vars[1] = -1.29183883411;
   vars[2] = -1151.0;
   
   out.Put("==> test: vars[0] = -1713.27528417579, vars[1] = -1.29183883411, vars[2] = -1151.0\n");
   n = sprintf(outBuffer, "%s", " d specifier ");
   out.Put("==> test: %s\n", outBuffer);
   n = sprintf(outBuffer, "%d %d %d", vars[0], vars[1], vars[2]);
   out.Put("==> test: %s\n", outBuffer);
   n = sprintf(outBuffer, "%s", " x specifier ");
   out.Put("==> test: %s\n", outBuffer);
   n = sprintf(outBuffer, "%x %x %x", vars[0], vars[1], vars[2]);
   out.Put("==> test: %s\n", outBuffer);
   n = sprintf(outBuffer, "%s", " A specifier ");
   out.Put("==> test: %s\n", outBuffer);
   n = sprintf(outBuffer, "%A %A %A", vars[0], vars[1], vars[2]);
   out.Put("==> test: %s\n", outBuffer);
   
   out.Put("==========\n");
   for (int i = 0; i < 3; i++)
   {
      n = sprintf(outBuffer, "%d", vars[i]);
      out.Put("%s\n", outBuffer);
   }
   
   out.Put("==========\n");
   for (int i = 0; i < 3; i++)
   {
      n = sprintf(outBuffer, "%x", vars[i]);
      out.Put("%s\n", outBuffer);
   }

   out.Put("==========\n");
   for (int i = 0; i < 3; i++)
   {
      n = sprintf(outBuffer, "%A", vars[i]);
      out.Put("%s\n", outBuffer);
   }
   out.Put("==========\n");
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("..\\..\\Test\\TestMath\\TestMathFunctionOut.txt");
   
   try
   {
      RunTest(out);
      // Need to complete this test
      //RunSprintfTest(out);
      out.Put("\nSuccessfully ran unit testing of Math Functions!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}

