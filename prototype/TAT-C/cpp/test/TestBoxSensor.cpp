//
//  Test Box Sensor (class RectangularSensor)
//  
//
//  Created by Stark, Michael E. (GSFC-5830) May 2018
//
//


#include "RectangularSensor.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "RealUtilities.hpp"
#include <iostream>

//#define PRINT_STATE

using namespace std;
using namespace GmatMathConstants;


//------------------------------------------------------------------------------
// ******************* test driver for all tests ***********************
//------------------------------------------------------------------------------
int main()
{
   
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   std::string logFileName = "BoxSensorTest.txt";
   std::string outputPath = "../output/";
   
   MessageInterface::SetMessageReceiver(consoleMsg);
   
   /*---------------------------------------------------------------------------
   cout << "Enter log file name >";
   cin >> logFileName;
    */
   
   MessageInterface::SetLogFile(outputPath+logFileName);
   MessageInterface::SetLogEnable(true);
   MessageInterface::ShowMessage ("\n******Starting Test Runs******\n\n");
   
   // define rectangular sensor & FOV for test, max cone angle = 60 deg (PI/3)
   RectangularSensor *box = new RectangularSensor(PI/4.0, PI/4.0);

//------------------------------------------------------------------------------
// *** Test 1 -- basic test of maxExcursionAngle
//------------------------------------------------------------------------------
   MessageInterface::ShowMessage("*** Test #1 ***\n");
   
   Real ptInsideCone = PI/6.0;
   Real ptInsideClock = PI/6.0;
   
   Real ptOutsideCone = PI/3;
   Real ptOutsideClock = PI/3;
   
   bool inside = box->CheckTargetVisibility(ptInsideCone,ptInsideClock);
   bool outside = !box->CheckTargetVisibility(ptOutsideCone, ptOutsideClock);
   
   if (inside)
      MessageInterface::ShowMessage("Point inside box passes\n");
   else
      MessageInterface::ShowMessage("Point inside box fails\n");
   
   if (outside)
      MessageInterface::ShowMessage("Point outside box passes\n");
   else
      MessageInterface::ShowMessage("Point outside box fails\n");
   MessageInterface::ShowMessage("\n");
   
//------------------------------------------------------------------------------
// *** Test 2 -- test of gets & puts
//------------------------------------------------------------------------------

   MessageInterface::ShowMessage("*** Test #2 ***\n");
   
   Real width  = DEG_PER_RAD*box->GetAngleWidth();
   Real height = DEG_PER_RAD*box->GetAngleHeight();
   
   MessageInterface::ShowMessage("Width = %f, Height = %f\n", width, height);
   MessageInterface::ShowMessage("changing to width = 60 deg, height = 30\n");
   
   box->SetAngleWidth(PI/3.0);
   box->SetAngleHeight(PI/6.0);
   
   MessageInterface::ShowMessage("Change has been made\n");
   width  = DEG_PER_RAD*box->GetAngleWidth();
   height = DEG_PER_RAD*box->GetAngleHeight();
   MessageInterface::ShowMessage("Width = %f, Height = %f\n", width, height);
   MessageInterface::ShowMessage("\n");

//------------------------------------------------------------------------------
// *** Test 3 -- test logic of being outside one side of rectangle
//            -- verifies all branches of if test in CheckTargetVisibility
//------------------------------------------------------------------------------
   
   MessageInterface::ShowMessage("*** Test #3 ***\n");
   Real north = 45.0;
   Real east = 75.0;
   Real south = -45.0;
   Real west = - 75;
   
   //top
   if (!box->CheckTargetVisibility(north,0.0))
      MessageInterface::ShowMessage("North passes\n");
   else
      MessageInterface::ShowMessage("North fails\n");
   
   //right
   if (!box->CheckTargetVisibility(0.0,east))
      MessageInterface::ShowMessage("East passes\n");
   else
      MessageInterface::ShowMessage("East fails\n");
   
   //bottom
   if (!box->CheckTargetVisibility(south,0.0))
      MessageInterface::ShowMessage("South passes\n");
   else
      MessageInterface::ShowMessage("South fails\n");
   
   //left
   if (!box->CheckTargetVisibility(0.0,west))
      MessageInterface::ShowMessage("West passes\n");
   else
      MessageInterface::ShowMessage("West fails\n");
   
} // end main
