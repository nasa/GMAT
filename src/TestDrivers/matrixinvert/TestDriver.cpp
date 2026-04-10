//------------------------------------------------------------------------------
//                           GmatConsole driver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Darrel J. Conway
// Created: 2003/08/28
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Program entry point for GmatConsole.
 */
//------------------------------------------------------------------------------

#include "TestDriver.hpp"
#include "MatrixFactorization.hpp"
#include "CholeskyFactorization.hpp"
#include "SchurFactorization.hpp"
#include "LUFactorization.hpp"
#include "QRFactorization.hpp"
#include "QRFactorizationVector.hpp"


#include <fstream>

// Infrastructure
// #include "GmatRunner.hpp"
// #include "BuildCoreGmatObjects.hpp"
// #include "MessageInterface.hpp"

// // Specific classes used in the tests
// #include "ComponentTester.hpp"

#ifdef DEBUG_CONSOLE
#include <unistd.h>
#endif


/**
 * The program entry point.
 * 
 * @param <argc> The count of the input arguments.
 * @param <argv> The input arguments.
 * 
 * @return 0 on success.
 */
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   int retval = 0; 

   std::string msg = "General Mission Analysis Tool\nComponent Test Program\n";

   msg += "Build Date: ";
   msg += __DATE__;
   msg += "  ";
   msg += __TIME__;
   
   std::cout << "\n********************************************\n"
             << "***  GMAT Unit Test Application\n"
             << "********************************************\n\n"
             << msg << "\n\n"
             << std::endl;
   
   bool        testMode = true;

   if (!RunUnitTests())
      retval = -1;
      
   return retval;
}


bool RunUnitTests()
{
   bool retval = true;


  
   Rmatrix myMatrix(8, 8); // Square, positive definite matrix
                           // Matrix is not symmetric and causes issues in 
                           // Schur factoring

   myMatrix(0, 0) = 21;
   myMatrix(0, 1) = 3;
   myMatrix(0, 2) = 5;
   myMatrix(0, 3) = 1;
   myMatrix(0, 4) = 9;
   myMatrix(0, 5) = 3;
   myMatrix(0, 6) = 4;
   myMatrix(0, 7) = 2;

   myMatrix(1, 0) = 4;
   myMatrix(1, 1) = 18;
   myMatrix(1, 2) = 5;
   myMatrix(1, 3) = 9;
   myMatrix(1, 4) = 2;
   myMatrix(1, 5) = 3;
   myMatrix(1, 6) = 4;
   myMatrix(1, 7) = 1;

   myMatrix(2, 0) = 6;
   myMatrix(2, 1) = 9;
   myMatrix(2, 2) = 20;
   myMatrix(2, 3) = 4;
   myMatrix(2, 4) = 7;
   myMatrix(2, 5) = 11;
   myMatrix(2, 6) = 3;
   myMatrix(2, 7) = 4;

   myMatrix(3, 0) = 3;
   myMatrix(3, 1) = 8;
   myMatrix(3, 2) = 2;
   myMatrix(3, 3) = 32;
   myMatrix(3, 4) = 9;
   myMatrix(3, 5) = 7;
   myMatrix(3, 6) = 7;
   myMatrix(3, 7) = 2;

   myMatrix(4, 0) = 3;
   myMatrix(4, 1) = 8;
   myMatrix(4, 2) = 2;
   myMatrix(4, 3) = 7;
   myMatrix(4, 4) = 16;
   myMatrix(4, 5) = 2;
   myMatrix(4, 6) = 1;
   myMatrix(4, 7) = 1;

   myMatrix(5, 0) = 8;
   myMatrix(5, 1) = 10;
   myMatrix(5, 2) = 2;
   myMatrix(5, 3) = 3;
   myMatrix(5, 4) = 2;
   myMatrix(5, 5) = 30;
   myMatrix(5, 6) = 7;
   myMatrix(5, 7) = 6;

   myMatrix(6, 0) = 1;
   myMatrix(6, 1) = 2;
   myMatrix(6, 2) = 1;
   myMatrix(6, 3) = 4;
   myMatrix(6, 4) = 7;
   myMatrix(6, 5) = 1;
   myMatrix(6, 6) = 44;
   myMatrix(6, 7) = 1;

   myMatrix(7, 0) = 9;
   myMatrix(7, 1) = 10;
   myMatrix(7, 2) = 2;
   myMatrix(7, 3) = 3;
   myMatrix(7, 4) = 6;
   myMatrix(7, 5) = 1;
   myMatrix(7, 6) = 1;
   myMatrix(7, 7) = 25;

   Rmatrix schurMatrix(8, 8); // Schur Test

   schurMatrix(0, 0) = 30;
   schurMatrix(0, 1) = 2;
   schurMatrix(0, 2) = 3;
   schurMatrix(0, 3) = 4;
   schurMatrix(0, 4) = 5;
   schurMatrix(0, 5) = 6;
   schurMatrix(0, 6) = 7;
   schurMatrix(0, 7) = 8;

   schurMatrix(1, 0) = 2;
   schurMatrix(1, 1) = 30;
   schurMatrix(1, 2) = 4;
   schurMatrix(1, 3) = 5;
   schurMatrix(1, 4) = 6;
   schurMatrix(1, 5) = 7;
   schurMatrix(1, 6) = 8;
   schurMatrix(1, 7) = 9;

   schurMatrix(2, 0) = 3;
   schurMatrix(2, 1) = 4;
   schurMatrix(2, 2) = 50;
   schurMatrix(2, 3) = 6;
   schurMatrix(2, 4) = 7;
   schurMatrix(2, 5) = 8;
   schurMatrix(2, 6) = 9;
   schurMatrix(2, 7) = 10;

   schurMatrix(3, 0) = 4;
   schurMatrix(3, 1) = 5;
   schurMatrix(3, 2) = 6;
   schurMatrix(3, 3) = 70;
   schurMatrix(3, 4) = 8;
   schurMatrix(3, 5) = 9;
   schurMatrix(3, 6) = 10;
   schurMatrix(3, 7) = 11;

   schurMatrix(4, 0) = 5;
   schurMatrix(4, 1) = 6;
   schurMatrix(4, 2) = 7;
   schurMatrix(4, 3) = 8;
   schurMatrix(4, 4) = 90;
   schurMatrix(4, 5) = 10;
   schurMatrix(4, 6) = 11;
   schurMatrix(4, 7) = 12;

   schurMatrix(5, 0) = 6;
   schurMatrix(5, 1) = 7;
   schurMatrix(5, 2) = 8;
   schurMatrix(5, 3) = 9;
   schurMatrix(5, 4) = 10;
   schurMatrix(5, 5) = 110;
   schurMatrix(5, 6) = 12;
   schurMatrix(5, 7) = 13;

   schurMatrix(6, 0) = 7;
   schurMatrix(6, 1) = 8;
   schurMatrix(6, 2) = 9;
   schurMatrix(6, 3) = 10;
   schurMatrix(6, 4) = 11;
   schurMatrix(6, 5) = 12;
   schurMatrix(6, 6) = 130;
   schurMatrix(6, 7) = 14;

   schurMatrix(7, 0) = 8;
   schurMatrix(7, 1) = 9;
   schurMatrix(7, 2) = 10;
   schurMatrix(7, 3) = 11;
   schurMatrix(7, 4) = 12;
   schurMatrix(7, 5) = 13;
   schurMatrix(7, 6) = 14;
   schurMatrix(7, 7) = 150;

   Rmatrix rectMatrix(6, 8); // Rectangular matrix
   rectMatrix(0, 0) = 21;
   rectMatrix(0, 1) = 3;
   rectMatrix(0, 2) = 5;
   rectMatrix(0, 3) = 1;
   rectMatrix(0, 4) = 9;
   rectMatrix(0, 5) = 3;
   rectMatrix(0, 6) = 4;
   rectMatrix(0, 7) = 2;

   rectMatrix(1, 0) = 4;
   rectMatrix(1, 1) = 18;
   rectMatrix(1, 2) = 5;
   rectMatrix(1, 3) = 9;
   rectMatrix(1, 4) = 2;
   rectMatrix(1, 5) = 3;
   rectMatrix(1, 6) = 4;
   rectMatrix(1, 7) = 1;

   rectMatrix(2, 0) = 6;
   rectMatrix(2, 1) = 9;
   rectMatrix(2, 2) = 20;
   rectMatrix(2, 3) = 4;
   rectMatrix(2, 4) = 7;
   rectMatrix(2, 5) = 11;
   rectMatrix(2, 6) = 3;
   rectMatrix(2, 7) = 4;

   rectMatrix(3, 0) = 3;
   rectMatrix(3, 1) = 8;
   rectMatrix(3, 2) = 2;
   rectMatrix(3, 3) = 32;
   rectMatrix(3, 4) = 9;
   rectMatrix(3, 5) = 7;
   rectMatrix(3, 6) = 7;
   rectMatrix(3, 7) = 2;

   rectMatrix(4, 0) = 3;
   rectMatrix(4, 1) = 8;
   rectMatrix(4, 2) = 2;
   rectMatrix(4, 3) = 7;
   rectMatrix(4, 4) = 16;
   rectMatrix(4, 5) = 2;
   rectMatrix(4, 6) = 1;
   rectMatrix(4, 7) = 1;

   rectMatrix(5, 0) = 8;
   rectMatrix(5, 1) = 10;
   rectMatrix(5, 2) = 2;
   rectMatrix(5, 3) = 3;
   rectMatrix(5, 4) = 2;
   rectMatrix(5, 5) = 30;
   rectMatrix(5, 6) = 7;
   rectMatrix(5, 7) = 6;

   Rmatrix transRectMatrix(8, 6); // Transpose of rectangular matrix
   for (int i = 0; i < transRectMatrix.GetNumRows(); i++)
   {
      for (int j = 0; j < transRectMatrix.GetNumColumns(); j++)
         transRectMatrix(i, j) = rectMatrix(j, i);
   }

   Rmatrix smallMatrix(5, 5); // Symmetric, positive definite matrix for
                              // Cholesky inversion
   smallMatrix(0, 0) = 1;
   smallMatrix(0, 1) = 2;
   smallMatrix(0, 2) = 2;
   smallMatrix(0, 3) = 2;
   smallMatrix(0, 4) = 2;
   smallMatrix(1, 0) = 2;
   smallMatrix(1, 1) = 8;
   smallMatrix(1, 2) = 0;
   smallMatrix(1, 3) = 0;
   smallMatrix(1, 4) = 0;
   smallMatrix(2, 0) = 2;
   smallMatrix(2, 1) = 0;
   smallMatrix(2, 2) = 24;
   smallMatrix(2, 3) = 4;
   smallMatrix(2, 4) = 4;
   smallMatrix(3, 0) = 2;
   smallMatrix(3, 1) = 0;
   smallMatrix(3, 2) = 4;
   smallMatrix(3, 3) = 35;
   smallMatrix(3, 4) = 6;
   smallMatrix(4, 0) = 2;
   smallMatrix(4, 1) = 0;
   smallMatrix(4, 2) = 4;
   smallMatrix(4, 3) = 6;
   smallMatrix(4, 4) = 40;

   Rmatrix qrTest(4, 3);
   qrTest(0, 0) = 1;
   qrTest(0, 1) = 2;
   qrTest(0, 2) = 3;
   qrTest(1, 0) = 1;
   qrTest(1, 1) = 5;
   qrTest(1, 2) = 6;
   qrTest(2, 0) = 1;
   qrTest(2, 1) = 8;
   qrTest(2, 2) = 9;
   qrTest(3, 0) = 1;
   qrTest(3, 1) = 11;
   qrTest(3, 2) = 12;
   
   Rmatrix newRectMatrix(5, 3);
   newRectMatrix(0, 0) = 1;
   newRectMatrix(0, 1) = 3;
   newRectMatrix(0, 2) = 8;
   newRectMatrix(1, 0) = 2;
   newRectMatrix(1, 1) = 3;
   newRectMatrix(1, 2) = 9;
   newRectMatrix(2, 0) = 1;
   newRectMatrix(2, 1) = 0;
   newRectMatrix(2, 2) = 2;
   newRectMatrix(3, 0) = 4;
   newRectMatrix(3, 1) = 7;
   newRectMatrix(3, 2) = 2;
   newRectMatrix(4, 0) = 2;
   newRectMatrix(4, 1) = 2;
   newRectMatrix(4, 2) = 2;


   // -------------------------------------------------------------------------
   // Cholesky factorization tests
   
   CholeskyFactorization *ci = new CholeskyFactorization();
   Rmatrix ciUpper(8, 8);
   Rmatrix cholMatrix = myMatrix;
   Rmatrix blankMatrix;
   ci->Factor(myMatrix, ciUpper, blankMatrix);

   std::cout << "Cholesky Upper Triangular Matrix:" << std::endl;
   for (int i = 0; i < myMatrix.GetNumRows(); i++)
   {
      for (int j = 0; j < myMatrix.GetNumColumns(); j++)
      {
         std::cout << ciUpper(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";

   ci->Invert(smallMatrix);
   std::cout << "Cholesky Inverse Matrix:" << std::endl;
   for (int i = 0; i < smallMatrix.GetNumRows(); i++)
   {
      for (int j = 0; j < smallMatrix.GetNumColumns(); j++)
      {
         std::cout << smallMatrix(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";
   
   // -------------------------------------------------------------------------
   // Schur factorization tests

   SchurFactorization *si = new SchurFactorization();
   Rmatrix siMatrix = schurMatrix;

   si->Invert(siMatrix);
   std::cout << "Schur Inverse Matrix:" << std::endl;
   for (int i = 0; i < siMatrix.GetNumRows(); i++)
   {
      for (int j = 0; j < siMatrix.GetNumColumns(); j++)
      {
         std::cout << siMatrix(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";

   Rmatrix sdOrthog(8, 8);
   Rmatrix sdUpper(8, 8);


   si->Factor(schurMatrix, sdOrthog, sdUpper);
   std::cout << "Schur Unitary Matrix:" << std::endl;
   for (int i = 0; i < sdOrthog.GetNumRows(); i++)
   {
      for (int j = 0; j < sdOrthog.GetNumColumns(); j++)
      {
         std::cout << sdOrthog(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";

   std::cout << "Schur Upper Triangular Matrix:" << std::endl;
   for (int i = 0; i < sdUpper.GetNumRows(); i++)
   {
      for (int j = 0; j < sdUpper.GetNumColumns(); j++)
      {
         std::cout << sdUpper(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";

   // -------------------------------------------------------------------------
   // LU Factorization tests

   LUFactorization *lu = new LUFactorization();
   Rmatrix luMatrix = myMatrix;

   lu->Invert(luMatrix);
   std::cout << "LU Inverse Matrix:" << std::endl;
   for (int i = 0; i < myMatrix.GetNumRows(); i++)
   {
      for (int j = 0; j < myMatrix.GetNumColumns(); j++)
      {
         std::cout << luMatrix(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";
  
   Rmatrix luUpper(6, 8);
   Rmatrix luLower(6, 6);
   lu->Factor(rectMatrix, luLower, luUpper);

   std::cout << "L Matrix (rectangular input):" << std::endl;
   for (int i = 0; i < luLower.GetNumRows(); i++)
   {
      for (int j = 0; j < luLower.GetNumColumns(); j++)
      {
         std::cout << luLower(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";

   std::cout << "U Matrix (rectangular input):" << std::endl;
   for (int i = 0; i < luUpper.GetNumRows(); i++)
   {
      for (int j = 0; j < luUpper.GetNumColumns(); j++)
      {
         std::cout << luUpper(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";
   
   // -------------------------------------------------------------------------
   // QR Factorization tests
   
   QRFactorization *qr = new QRFactorization();
   Rmatrix qrMatrix = myMatrix;

   qr->Invert(qrMatrix);
   std::cout << "QR Inverse Matrix:" << std::endl;
   for (int i = 0; i < myMatrix.GetNumRows(); i++)
   {
      for (int j = 0; j < myMatrix.GetNumColumns(); j++)
      {
         std::cout << qrMatrix(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";

   // ----- Test QR with rectangular input matrix
   Rmatrix qrUpper(6, 8);
   Rmatrix qrOrthog(6, 6);
   qr->Factor(rectMatrix, qrUpper, qrOrthog);
   std::cout << "Q Matrix (rectangular input):" << std::endl;
   for (int i = 0; i < qrOrthog.GetNumRows(); i++)
   {
      for (int j = 0; j < qrOrthog.GetNumColumns(); j++)
      {
         std::cout << qrOrthog(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";

   std::cout << "R Matrix (rectangular input):" << std::endl;
   for (int i = 0; i < qrUpper.GetNumRows(); i++)
   {
      for (int j = 0; j < qrUpper.GetNumColumns(); j++)
      {
         std::cout << qrUpper(i, j) << "   ";
      }
      std::cout << "\n";
   }
   std::cout << "\n";
   
   
   // ----- Test deleting row or column from A using QR
   
   QRFactorization *qrDelete = new QRFactorization(false);
   Rmatrix QDelete(6, 6);
   Rmatrix RDelete(6, 8);
   qrDelete->Factor(rectMatrix, RDelete, QDelete);
   Rmatrix R1;
   Rmatrix Q1;
   qrDelete->RemoveFromQR(RDelete, QDelete, "row", 4, R1, Q1);
   Rmatrix result = Q1 * R1;

   std::cout << "\n\nMatrix after row/col removal: \n";
   printMatrix(result);
   
   // ----- Test adding row or column to A using QR

   
   QRFactorization *qrAdd = new QRFactorization(false);
   Rmatrix QAdd(8, 8);
   Rmatrix RAdd(8, 6);
   qrAdd->Factor(transRectMatrix, RAdd, QAdd);
   Rmatrix R2;
   Rmatrix Q2;
   Rvector newCol(8);
   newCol[0] = 3;
   newCol[1] = -4;
   newCol[2] = 5;
   newCol[3] = 13;
   newCol[4] = -1;
   newCol[5] = 7;
   newCol[6] = 9;
   newCol[7] = 4;
   qrAdd->AddToQR(RAdd, QAdd, "col", 0, newCol, R2, Q2);
   Rmatrix result2 = Q2 * R2;

   std::cout << "\nMatrix after row/col insertion: \n";
   printMatrix(result2);

   
   // -------------------------------------------------------------------------
   // LU linear system solver

   Rmatrix systemMatrix(3, 3);
   systemMatrix(0, 0) = 1;
   systemMatrix(0, 1) = 4;
   systemMatrix(0, 2) = 7;
   systemMatrix(1, 0) = 2;
   systemMatrix(1, 1) = 5;
   systemMatrix(1, 2) = 8;
   systemMatrix(2, 0) = 3;
   systemMatrix(2, 1) = 6;
   systemMatrix(2, 2) = 10;

   Rvector b(3);
   Rvector x(3);

   b[0] = 1;
   b[1] = 1;
   b[2] = 1;

   LUFactorization *lu2 = new LUFactorization();
   lu2->SolveSystem(systemMatrix, b, x);

   std::cout << "x Vector" << std::endl;
   std::cout << x[0] << std::endl;
   std::cout << x[1] << std::endl;
   std::cout << x[2] << std::endl;
   std::cout << std::endl;

   // ----- Using system solver with rectangular matrix

   Rmatrix a(4, 3);
   a(0, 0) = 4.000000;
   a(0, 1) = 6.000000;
   a(0, 2) = 3.300000;
   a(1, 0) = 2.000000;
   a(1, 1) = 9.000000;
   a(1, 2) = 7.100000;
   a(2, 0) = 3.000000;
   a(2, 1) = 1.000000;
   a(2, 2) = 2.200000;
   a(3, 0) = 4.000000;
   a(3, 1) = 6.000000;
   a(3, 2) = 1.900000;

   b.SetSize(4);
   b[0] = 3;
   b[1] = 5.5;
   b[2] = 2;
   b[3] = 8;

   x.SetSize(3);

   lu->SolveSystem(a, b, x);

   std::cout << "x Vector" << std::endl;
   std::cout << x[0] << std::endl;
   std::cout << x[1] << std::endl;
   std::cout << x[2] << std::endl;

   std::cout << lu->Determinant(myMatrix) << std::endl;
   
   return retval;
}

//------------------------------------------------------------------------------
// void printMatrix(Rmatrix inputMatrix)
//------------------------------------------------------------------------------
/**
* Method to print a matrix onto the screen
*
*/
//------------------------------------------------------------------------------
void printMatrix(Rmatrix inputMatrix)
{
   for (Integer i = 0; i < inputMatrix.GetNumRows(); i++)
   {
      for (Integer j = 0; j < inputMatrix.GetNumColumns(); j++)
         std::cout << inputMatrix(i, j) << "   ";
      std::cout << std::endl;
   }
   std::cout << std::endl;
}
