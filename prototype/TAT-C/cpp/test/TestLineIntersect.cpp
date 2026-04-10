//$Id$
//------------------------------------------------------------------------------
//                               TestLineIntersect
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2017.03.27
//
/**
 * Unit-test driver for the LinearAlgebra class (in particular, the 
 * LineSegmentIntersect method)
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Rmatrix.hpp"
#include "TimeTypes.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "LinearAlgebra.hpp"

using namespace std;

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string outFormat = "%16.9f ";
   Real        tolerance = 1e-15;
   
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   MessageInterface::ShowMessage("%s\n",
                                 GmatTimeUtil::FormatCurrentTime().c_str());
   
   // Set global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);
   
   char *buffer = NULL;
   buffer = getenv("OS");
   if (buffer  != NULL)
   {
      MessageInterface::ShowMessage("Current OS is %s\n", buffer);
   }
   else
   {
      MessageInterface::ShowMessage("Buffer is NULL\n");
   }

   MessageInterface::ShowMessage("*** START TEST ***\n");

   try
   {
      // Test the LineSegmentIntersect method
      MessageInterface::ShowMessage("*** TEST*** LineIntersect\n");

      // randomly generate line segments (we want 0.0 <= val <= 1.0)
      Integer numLine = 20;
      
      Rmatrix XY1(numLine, 4);
      Rmatrix XY2(numLine+10, 4);
      
      for (Integer ii = 0; ii < numLine; ii++)
      {
         XY1(ii,0) = (rand()%100) /100.00;
//         MessageInterface::ShowMessage(" random number for XY1(%d,%d) = %12.10f\n",
//                                       ii,0, XY1(ii,0));
         XY1(ii,1) = (rand()%100) /100.00;
         XY1(ii,2) = (rand()%100) /100.00;
         XY1(ii,3) = (rand()%100) /100.00;
         
         XY2(ii,0) = (rand()%100) /100.00;
         XY2(ii,1) = (rand()%100) /100.00;
         XY2(ii,2) = (rand()%100) /100.00;
         XY2(ii,3) = (rand()%100) /100.00;
      }
      
      // Add parallel line segments
      for (Integer ii = 0; ii < 5; ii++)
      {
         XY2((numLine+ii), 0) = XY1(ii, 0) + 0.1;
         XY2((numLine+ii), 1) = XY1(ii, 1) + 0.1;
         XY2((numLine+ii), 2) = XY1(ii, 2) + 0.1;
         XY2((numLine+ii), 3) = XY1(ii, 3) + 0.1;
      }
      
      // Add coincident line segments
      for (Integer ii = 0; ii < 5; ii++)
      {
         XY2(numLine+5+ii, 0) = XY1(5+ii, 0);
         XY2(numLine+5+ii, 1) = XY1(5+ii, 1);
         XY2(numLine+5+ii, 2) = XY1(5+ii, 2);
         XY2(numLine+5+ii, 3) = XY1(5+ii, 3);
      }
      
      clock_t t0 = clock();
      
      Integer sz1row, sz1col, sz2row, sz2col;
      
      XY1.GetSize(sz1row, sz1col);
      XY2.GetSize(sz2row, sz2col);
      
      MessageInterface::ShowMessage("**** XY1:::\n");
      for (Integer ii = 0; ii < sz1row; ii++)
      {
         MessageInterface::ShowMessage("XY1(%d, 0) = %12.10f\n",
                                       ii, XY1(ii,0));
         MessageInterface::ShowMessage("XY1(%d, 1) = %12.10f\n",
                                       ii, XY1(ii,1));
         MessageInterface::ShowMessage("XY1(%d, 2) = %12.10f\n",
                                       ii, XY1(ii,2));
         MessageInterface::ShowMessage("XY1(%d, 3) = %12.10f\n",
                                       ii, XY1(ii,3));
      }
      MessageInterface::ShowMessage("\n**** XY2:::\n");
      for (Integer ii = 0; ii < sz2row; ii++)
      {
         MessageInterface::ShowMessage("XY2(%d,0) = %12.10f\n",
                                       ii, XY2(ii,0));
         MessageInterface::ShowMessage("XY2(%d,1) = %12.10f\n",
                                       ii, XY2(ii,1));
         MessageInterface::ShowMessage("XY2(%d,2) = %12.10f\n",
                                       ii, XY2(ii,2));
         MessageInterface::ShowMessage("XY2(%d,3) = %12.10f\n",
                                       ii, XY2(ii,3));
      }
      
      std::vector<IntegerArray>  adjacency;
      Rmatrix                    matrixX(sz1row, sz2row);
      Rmatrix                    matrixY(sz1row, sz2row);
      Rmatrix                    distance1To2(sz1row, sz2row);
      Rmatrix                    distance2To1(sz1row, sz2row);
      std::vector<IntegerArray>  parallelAdjacency;
      std::vector<IntegerArray>  coincidentAdjacency;

      LinearAlgebra::LineSegmentIntersect(XY1, XY2, adjacency,
                                          matrixX, matrixY,
                                          distance1To2, distance2To1,
                                          parallelAdjacency, coincidentAdjacency);
      
      Real timeSpent = ((Real) (clock() - t0)) / CLOCKS_PER_SEC;
      MessageInterface::ShowMessage(
                  "TIME SPENT in LineSegmentIntersection is %12.10f seconds\n",
                  timeSpent);
      MessageInterface::ShowMessage("**** adjacency:\n");
      for (Integer ii = 0; ii < adjacency.size(); ii++)
      {
         IntegerArray ia = adjacency.at(ii);
         for (Integer jj = 0; jj < ia.size(); jj++)
            MessageInterface::ShowMessage(" adjacency(%d,%d) =     %d\n",
                                          ii, jj, ia.at(jj));
      }
      MessageInterface::ShowMessage("\n**** matrixX = %s\n",
                                    matrixX.ToString(12).c_str());
      MessageInterface::ShowMessage("\n**** matrixY = %s\n",
                                    matrixY.ToString(12).c_str());
      MessageInterface::ShowMessage("\n**** distance1To2 = %s\n",
                                    distance1To2.ToString(12).c_str());
      MessageInterface::ShowMessage("\n**** distance2To1 = %s\n",
                                    distance2To1.ToString(12).c_str());
      MessageInterface::ShowMessage("\n**** parallel adjacency:\n");
      for (Integer ii = 0; ii < parallelAdjacency.size(); ii++)
      {
         IntegerArray ia = parallelAdjacency.at(ii);
         for (Integer jj = 0; jj < ia.size(); jj++)
            MessageInterface::ShowMessage(" parallelAdjacency(%d,%d) =     %d\n",
                                          ii, jj, ia.at(jj));
      }
      MessageInterface::ShowMessage("\n**** coincident adjacency:\n");
      for (Integer ii = 0; ii < coincidentAdjacency.size(); ii++)
      {
         IntegerArray ia = coincidentAdjacency.at(ii);
         for (Integer jj = 0; jj < ia.size(); jj++)
            MessageInterface::ShowMessage(" coincidentAdjacency(%d,%d) =     %d\n",
                                          ii, jj, ia.at(jj));
      }
      
      cout << endl;
      cout << "Hit enter to end" << endl;
      cin.get();

      MessageInterface::ShowMessage("*** END TEST ***\n");
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n", be.GetFullMessage().c_str());
   }

}
