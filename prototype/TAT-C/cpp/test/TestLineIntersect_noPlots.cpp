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

void MLSyntax(const std::string &matName, Rmatrix mat)
{
   Integer numRows, numCols;
   mat.GetSize(numRows, numCols);
   
   MessageInterface::ShowMessage("\n%s = new Rmatrix(%d, %d)\n", matName.c_str(),
                                 numRows, numCols);
   
   for (Integer r = 0; r < numRows; r++)
      for (Integer c = 0; c < numCols; c++)
      {
         MessageInterface::ShowMessage("%s[%d,%d] = %12.10f\n", matName.c_str(),
                                       r+1, c+1, mat(r,c));
      }
   
}
void MLSyntax(const std::string &matName, std::vector<IntegerArray> ia)
{
   Integer numRows = ia.size();
   Integer numCols = (ia.at(0)).size();
   
   MessageInterface::ShowMessage("\n%s = new Rmatrix(%d, %d)\n", matName.c_str(),
                                 numRows, numCols);
   
   for (Integer ii = 0; ii < numRows; ii++)
   {
      IntegerArray ithRow = ia.at(ii);
      for (Integer jj = 0; jj < numCols; jj++)
      {
         MessageInterface::ShowMessage("%s[%d,%d] = %d\n", matName.c_str(),
                                    ii+1,jj+1, ithRow.at(jj));
      }
   }
}


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
      Integer numLine = 16;
      
      Rmatrix XY1(numLine, 4,
                  // Random lines
                  0.87572400256463,    0.597302079801353,  0.280397005689543,  0.0451518760404848,
                  0.393098592724503,   0.65917606662187,   0.25942744797746,   0.240645885287937,
                  0.458066938000918,   0.580006430551879,  0.547095959676547,  0.00885503009860023,
                  0.208245204973289,   0.909951610123007,  0.541268467485969,  0.671593534312873,
                  0.757273162983819,   0.636006152111755,  0.788113184048992,  0.904813230017362,
                  0.546692028178502,   0.525561251235387,  0.869605555315031,  0.57241631161166,
                  0.357388192967744,   0.259623080110499,  0.787540530159212,  0.155461290328972,
                  0.700997789695237,   0.0511708673569459, 0.969427855895745,  0.502367433727028,
                  0.109222496688655,   0.73195012915998,   0.180468109688552,  0.567732878272974,
                  0.00661076596420229, 0.164291390918458,  0.930598962817579,  0.188271775739867,
                  // Parallel lines
                  0.25, 0.25, 0.75, 0.25,
                  0.75, 0.25, 0.75, 0.75,
                  0.75, 0.75, 0.25, 0.75,
                  0.25, 0.75, 0.25, 0.25,
                  // Coincident lines
                  0.5, 0.5, 0.75, 0.75,
                  0.4, 0.4, 0.75, 0.75
                  );
//      Rmatrix XY2(numLine-2, 4,   // comment out two lines to test this
      Rmatrix XY2(numLine, 4,
                  // Random lines
                  0.32419646346704,    0.106872780238886,   0.287584055249314,   0.905531348068602,
                  0.716037342838811,   0.732149758734352,   0.816712735890955,   0.417320883626846,
                  0.552928708571216,   0.970522786956106,   0.450528094868533,   0.154058327516862,
                  0.142281739817257,   0.608885538697176,   0.806632520363243,   0.539999263770361,
                  0.380363559412667,   0.719665612178919,   0.790172935024334,   0.937091367960309,
                  0.396569245211042,   0.302752325729782,   0.282957873186578,   0.660955064915145,
                  0.576741695694105,   0.459022394078519,   0.0683092386250433,  0.394657441563815,
                  0.0194015027111201,  0.0480286131823641,  0.0549307282223953,  0.258990542158967,
                  0.577580061269445,   0.385352105175205,   0.637522639339998,   0.847912159970545,
                  0.932196092808549,   0.361716380248139,   0.4242855812171,     0.945056074939906,
                  // Parallel lines
                  0.27, 0.27, 0.77, 0.27,
                  0.77, 0.27, 0.77, 0.77,
                  0.77, 0.77, 0.27, 0.77,
                  0.27, 0.77, 0.27, 0.27,
                  // Coincident lines
                  0.5, 0.5, 0.77, 0.77,
                  0.44, 0.44, 0.75, 0.75
                  );
      
      
      clock_t t0 = clock();
      
      Integer sz1row, sz1col, sz2row, sz2col;
      
      XY1.GetSize(sz1row, sz1col);
      XY2.GetSize(sz2row, sz2col);
      
      MLSyntax("XY1", XY1);
      MLSyntax("XY2", XY2);
//      MessageInterface::ShowMessage("XY1 = new Rmatrix(16,4)\n");
//      for (Integer ii = 0; ii < sz1row; ii++)
//      {
//         for (Integer jj = 0; jj < 4; jj++)
//            MessageInterface::ShowMessage("XY1[%d, %d] = %12.10f\n",
//                                          ii, jj, XY1(ii,jj));
//      }
//      MessageInterface::ShowMessage("XY2 = new Rmatrix(16,4)\n");
//      for (Integer ii = 0; ii < sz2row; ii++)
//      {
//         for (Integer jj = 0; jj < 4; jj++)
//            MessageInterface::ShowMessage("XY2[%d, %d] = %12.10f\n",
//                                          ii, jj, XY2(ii,jj));
//      }
      
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
//      MessageInterface::ShowMessage("intAdjacencyMatrixTruth = new Rmatrix(16,16)\n");
//      for (Integer ii = 0; ii < adjacency.size(); ii++)
//      {
//         IntegerArray ia = adjacency.at(ii);
//         for (Integer jj = 0; jj < ia.size(); jj++)
//            MessageInterface::ShowMessage(" adjacency[%d,%d] =     %d\n",
//                                          ii, jj, ia.at(jj));
//      }
      MLSyntax("intAdjacencyMatrixTruth", adjacency);
      
      MLSyntax("intMatrixXTruth", matrixX);
      MLSyntax("intMatrixYTruth", matrixY);
      MLSyntax("intNormalizedDistance1To2Truth", distance1To2);
      MLSyntax("intNormalizedDistance2To1Truth", distance2To1);

      MLSyntax("parAdjacencyMatrixTruth", parallelAdjacency);
      
      MLSyntax("coincAdjacencyMatrixTruth", coincidentAdjacency);
      
//      MessageInterface::ShowMessage("\nintMatrixXTruth = %s\n",
//                                    matrixX.ToString(12).c_str());
//      MessageInterface::ShowMessage("\nintMatrixYTruth = %s\n",
//                                    matrixY.ToString(12).c_str());
//      MessageInterface::ShowMessage("\nintNormalizedDistance1To2Truth = %s\n",
//                                    distance1To2.ToString(12).c_str());
//      MessageInterface::ShowMessage("\nintNormalizedDistance2To1Truth = %s\n",
//                                    distance2To1.ToString(12).c_str());
//      MessageInterface::ShowMessage("\nparAdjacencyMatrixTruth:\n");
//      for (Integer ii = 0; ii < parallelAdjacency.size(); ii++)
//      {
//         IntegerArray ia = parallelAdjacency.at(ii);
//         for (Integer jj = 0; jj < ia.size(); jj++)
//            MessageInterface::ShowMessage(" parallelAdjacency(%d,%d) =     %d\n",
//                                          ii, jj, ia.at(jj));
//      }
//      MessageInterface::ShowMessage("\ncoincAdjacencyMatrixTruth:\n");
//      for (Integer ii = 0; ii < coincidentAdjacency.size(); ii++)
//      {
//         IntegerArray ia = coincidentAdjacency.at(ii);
//         for (Integer jj = 0; jj < ia.size(); jj++)
//            MessageInterface::ShowMessage(" coincidentAdjacency(%d,%d) =     %d\n",
//                                          ii, jj, ia.at(jj));
//      }
      
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
