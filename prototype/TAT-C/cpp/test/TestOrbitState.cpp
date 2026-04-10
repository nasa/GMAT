//$Id$
//------------------------------------------------------------------------------
//                               TestOrbitState
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2016.05.11
//
/**
 * Unit-test driver for the OrbitState
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Rvector6.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "AbsoluteDate.hpp"
#include "OrbitState.hpp"
#include "TimeTypes.hpp"


using namespace std;
using namespace GmatMathConstants;

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
      // Test the AbsoluteDate
      MessageInterface::ShowMessage("*** TEST*** OrbitState\n");

      // Set up equivalent truth data
      Rvector6 truthCart(
                         -2436.063522947054,  2436.063522947055, 5967.112612227063,
                         -5.385803634090905, -5.378203080755706, 0.009308738717021944);
      Rvector6 truthKep(6900,0.002,PI/3,PI/4,PI/4,PI/4);

      // Create the spacecraft state object and set Keplerian elements
      OrbitState state;
      
      //  Set Keplerian and test conversion to Cartesian
      state.SetKeplerianVectorState(truthKep);
      Rvector6 cartOut = state.GetCartesianState();
      MessageInterface::ShowMessage(
             "EXPECTED cartesian state: \n%12.10f  %12.10f %12.10f\n%12.10f  %12.10f %12.10f\n",
              truthCart[0], truthCart[1], truthCart[2], truthCart[3], truthCart[4], truthCart[5]);
      MessageInterface::ShowMessage(
              "COMPUTED cartesian state: \n%12.10f  %12.10f %12.10f\n%12.10f  %12.10f %12.10f\n",
              cartOut[0], cartOut[1], cartOut[2], cartOut[3], cartOut[4], cartOut[5]);
      Rvector6 diff    = cartOut - truthCart;
      Real     diffMag = diff.GetMagnitude();
      
      if (diffMag > 1e-11)
         MessageInterface::ShowMessage("*** ERROR - error in conversion from Keplerian to Cartesian (%12.10f) \n", diffMag);
      
      // Set Cartesian state and test conversion to Keplerian
      state.SetCartesianState(truthCart);
      Rvector6 kepOut   = state.GetKeplerianState();
      MessageInterface::ShowMessage(
                                    "EXPECTED keplerian state: \n%12.10f  %12.10f %12.10f\n%12.10f  %12.10f %12.10f\n",
                                    truthKep[0], truthKep[1], truthKep[2], truthKep[3], truthKep[4], truthKep[5]);
      MessageInterface::ShowMessage(
                                    "COMPUTED keplerian state: \n%12.10f  %12.10f %12.10f\n%12.10f  %12.10f %12.10f\n",
                                    kepOut[0], kepOut[1], kepOut[2],
                                    kepOut[3], kepOut[4],
                                    kepOut[5]);
//      kepOut[0], kepOut[1], kepOut[2]*RAD_PER_DEG,
//      kepOut[3]*RAD_PER_DEG, kepOut[4]*RAD_PER_DEG,
//      kepOut[5]*RAD_PER_DEG);
      Rvector6 diff2    = kepOut - truthKep;
      Real     diffMag2 = diff2.GetMagnitude();
      
      if (diffMag2 > 1e-11)
         MessageInterface::ShowMessage("*** ERROR - error in conversion from Cartesian to Keplerian (%12.10f) \n", diffMag2);
      
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