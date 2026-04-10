//$Id$
//------------------------------------------------------------------------------
//                               TestUserPathFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2015.06.30
//
/**
 * Test driver for FunctionOutputData classes.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Linear.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "Rmatrix.hpp"
#include "Rmatrix33.hpp"
//#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "FileManager.hpp"
#include "TimeTypes.hpp"
#include "Earth.hpp"
#include "OrbitState.hpp"
#include "ConicalSensor.hpp"
#include "KeyValueStatistics.hpp"
#include "Spacecraft.hpp"
#include "VisibilityReport.hpp"
#include "VisiblePOIReport.hpp"


using namespace std;

Real GetMaxError(const Rvector &vec)
{
   Real max = -999.99;
   for (Integer ii = 0; ii < vec.GetSize(); ii++)
      if (vec(ii) > max) max = vec(ii);
   return max;
}

Real GetMaxError(const Rmatrix &mat)
{
   Real max = -999.99;
   Integer r, c;
   mat.GetSize(r,c);
   for (Integer ii = 0; ii < r; ii++)
      for (Integer jj = 0; jj < c; jj++)
         if (mat(ii,jj) > max) max = mat(ii,jj);
   return max;
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
      MessageInterface::ShowMessage("Current oS is %s\n", buffer);
   }
   else
   {
      MessageInterface::ShowMessage("Buffer is NULL\n");
   }

   MessageInterface::ShowMessage("*** START TEST ***\n");

   try
   {
      // Test the PathFunctionContainer
      MessageInterface::ShowMessage("*** TEST*** doesn't do anything yet\n");

         
      cout << endl;
      cout << "Hit enter to end" << endl;
      cin.get();

      MessageInterface::ShowMessage("*** END TEST ***\n");
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n", be.GetFullMessage().c_str());
   }


   // TODO: why don't these deletes work? - cause segmentation violation
//   if (buffer) delete buffer;
//   if (pfc)    delete pfc;
//   if (fid)    delete fid;
//   if (bd)     delete bd;
//   if (dpf)    delete dpf;
//   if (pfRes)  delete pf2;

//   if (cost)   delete cost;
//   if (dyn)    delete dyn;
//   if (alg)    delete alg;
}

