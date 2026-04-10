//
//  TestCustomSensor.cpp
//  
//
//  Created by Stark, Michael E. (GSFC-5850) May 2017
//
//


#include "CustomSensorTestClass.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "RealUtilities.hpp"
#include <iostream>

//#define PRINT_STATE

using namespace std;
using namespace GmatMathConstants;


void TestUtilities()
{
   // all internal utilities are reentrant, no member data are used so no
   // need to initialize
   CustomSensorTestClass *cs=NULL;
   
   MessageInterface::ShowMessage("************ Starting Test #0 ********\n");
   MessageInterface::ShowMessage
      ("Tests of internal coordinate conversion & Rvector utilities\n");
   MessageInterface::ShowMessage
   ("results were verified using Matlab interactively with same data\n\n");
   
   //---------------------------------------------------------------------------
   // test angle conversions and projections
   //---------------------------------------------------------------------------
   
   Real cone = 30.0/180*PI;
   Real clock =  30.0/180*PI;
   Real RA, dec;
   
   // test ConeClockToRADEC
   MessageInterface::ShowMessage("cone=%16.9f rad, clock=%16.9f rad\n",
                                 cone, clock);
   cs->ConeClocktoRADEC(cone,clock,RA,dec);
   MessageInterface::ShowMessage("RA=%f, dec=%f\n", RA, dec);
   
   // test RADECtoUnitVec
   Rvector3 uVec = cs->RADECtoUnitVec(RA,dec);
   for (int i=0; i<3; i++)
      MessageInterface::ShowMessage("Element %d = %12.9f\n",i+1, uVec[i]);
   
   // test unit vector to stereographic
   Real xCoord, yCoord;
   cout << uVec[0] << "\n";
   cout << uVec[1] << "\n";
   cout << uVec[2] << "\n";
   
   cs->UnitVecToStereographic(uVec,xCoord,yCoord);
   MessageInterface::ShowMessage("Stereographic (x,y) = (%13.9f,%13.9f)\n",
                                 xCoord,yCoord);
   
   // test cone & clock to stereographic, should get same stereographic value
   // as above test does
   Real xCoord2, yCoord2;
   cs->ConeClockToStereographic(cone,clock,xCoord2,yCoord2);
   MessageInterface::ShowMessage("this result should match the last one\n");
   MessageInterface::ShowMessage("Stereographic (x,y) = (%13.9f,%13.9f)\n",
                                 xCoord2,yCoord2);
   
   // test cone & clock arrays to stereographic
   // Make sure iteration across arrays is correct
   Rvector coneAngleVec(3, cone, 2*cone, 3*cone);
   Rvector clockAngleVec(3, clock, 2*clock, 3*clock);
   Rvector xArray(3),yArray(3);
   
   
   MessageInterface::ShowMessage("\nStarting cone & clock arrays test\n\n");
   for (int i=0; i<3; i++)
   {
      MessageInterface::ShowMessage("cone & clock [%d] = (%12.9f %12.9f)\n",
                                    i, coneAngleVec[i], clockAngleVec[i]);
   }
   
   cs->ConeClockArraysToStereographic(coneAngleVec,clockAngleVec,
                                      xArray,yArray);
   
   MessageInterface::ShowMessage("first (x,y) = (%13.9f, %13.9f)\n",
                                 xArray[0],yArray[0]);
   MessageInterface::ShowMessage("second (x,y) = (%13.9f, %13.9f)\n",
                                 xArray[1],yArray[1]);
   MessageInterface::ShowMessage("third (x,y) = (%13.9f, %13.9f)\n",
                                 xArray[2],yArray[2]);
   
   
   //---------------------------------------------------------------------------
   // test rVector functions
   //---------------------------------------------------------------------------
   
   Rvector vec(6,3.0,1.0,4.0,1.0,5.0,2.0);
   MessageInterface::ShowMessage("\n\n rVector Tests of initial vector vec\n");
   for (int i=0; i < 6; i++)
   {
      MessageInterface::ShowMessage("%f ",vec[i]);
   }
   
   // test Max & Min
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("Maximum = %f, minimum= %f\n",
                                 cs->Max(vec), cs->Min(vec));
   
   // test sort in ascending order
   cs->Sort(vec);
   MessageInterface::ShowMessage("\nvector vec after ascending sort\n");
   for (int i=0; i < 6; i++)
   {
      MessageInterface::ShowMessage("%f ",vec[i]);
   }
   MessageInterface::ShowMessage("\n\n");
   
   // test sort in descending order
   Rvector vec2(6,3.0,1.0,4.0,1.0,5.0,2.0);
   MessageInterface::ShowMessage("vector vec2 before descending sort\n");
   for (int i=0; i < 6; i++)
   {
      MessageInterface::ShowMessage("%f ",vec2[i]);
   }
   
   cs->Sort(vec2,false);
   MessageInterface::ShowMessage("\n\nvector vec2 after descending sort\n");
   for (int i=0; i < 6; i++)
   {
      MessageInterface::ShowMessage("%f ",vec2[i]);
   }
   MessageInterface::ShowMessage("\n\n");
   
   
   // test sort with initial indices being moved along with array value
   MessageInterface::ShowMessage("Sort including index sort test\n");
   Rvector vec3(6,3.0,1.0,4.0,1.0,5.0,2.0);
   IntegerArray indices;
   
   MessageInterface::ShowMessage("vector vec3 before indexed sort\n");
   for (int i=0; i < 6; i++)
      MessageInterface::ShowMessage("%f ",vec3[i]);
   MessageInterface::ShowMessage("\n");
   if (indices.empty())
      MessageInterface::ShowMessage("Index array is empty\n");
   else
   {
      MessageInterface::ShowMessage("Index array = ");
      for (int i = 0; i < indices.size(); i++)
         MessageInterface::ShowMessage("%d ");
      MessageInterface::ShowMessage("\n");
   }
   
   cs->Sort(vec3,indices,false); // edit to get ascending (true)
   // or descending (false)
   
   MessageInterface::ShowMessage("vector vec3 & indices after indexed sort\n");
   for (int i=0; i < 6; i++)
      MessageInterface::ShowMessage("%f ",vec3[i]);
   MessageInterface::ShowMessage("\n");
   if (indices.empty())
      MessageInterface::ShowMessage("Index array is empty\n");
   else
   {
      MessageInterface::ShowMessage("Index array = ");
      for (int i = 0; i < indices.size(); i++)
         MessageInterface::ShowMessage("%d ",indices[i]);
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("\n************* End Test #0 ***********\n\n");
}


//---------------------------------------------------------------------------
// TestConstructor calls the constructor and then prints the member data
//---------------------------------------------------------------------------
void TestConstructor (int testID,
                      const Rvector &coneAngleVec,
                      const Rvector &clockAngleVec)
{
   MessageInterface::ShowMessage(
      "constructor test #%d\n",testID);
   // display inputs
   MessageInterface::ShowMessage("Cone & clock inputs to constructor\n");
   
   for (int i=0; i<coneAngleVec.GetSize(); i++)
   {
      MessageInterface::ShowMessage("cone & clock [%d] = (%12.9f %12.9f)\n",
                                    i+1, coneAngleVec[i], clockAngleVec[i]);
   }
   
   
   //declare and call constructor
   MessageInterface::ShowMessage("\nCalling Constructor\n");
   
   CustomSensorTestClass *cs = new CustomSensorTestClass(coneAngleVec,
                                                         clockAngleVec);
   
   // display the resulting state
   cs->PrintState();
   MessageInterface::ShowMessage(
      "End constructor Test #%d\n\n", testID);
   delete cs;
}
//------------------------------------------------------------------------------
// ******************* test driver for all tests ***********************
//------------------------------------------------------------------------------
int main()
{
   
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   std::string logFileName = "CustomSensorTest.txt";
   std::string outputPath = "./";
   
   MessageInterface::SetMessageReceiver(consoleMsg);
   
   /*---------------------------------------------------------------------------
   cout << "Enter log file name >";
   cin >> logFileName;
    */
   
   MessageInterface::SetLogFile(outputPath+logFileName);
   MessageInterface::SetLogEnable(true);
   MessageInterface::ShowMessage ("\n******Starting Test Runs******\n\n");
   
   //---------------------------------------------------------------------------
   // Test #0 tests coordinate conversions and added Rvector functions
   //---------------------------------------------------------------------------

   MessageInterface::ShowMessage("Tests Rvector and conversion utilities\n");
   TestUtilities();
   
   //---------------------------------------------------------------------------
   // Test #1, 3 simple data points, test constructor only
   //---------------------------------------------------------------------------
  
   MessageInterface::ShowMessage("************ Starting Test #1 ********\n");
   MessageInterface::ShowMessage("Tests constructor with 3 point FOV input\n");
   MessageInterface::ShowMessage("No test of point or region in FOV\n\n");
   
   // set up inputs
   Real cone = 30.0/180*PI;
   Real clock =  30.0/180*PI;
  
   Rvector coneAngleVec1(3, cone, 2*cone, 3*cone);
   Rvector clockAngleVec1(3, clock, 2*clock, 3*clock);
   
#ifdef PRINT_STATE
   TestConstructor(1,coneAngleVec1,clockAngleVec1);
#endif
   
   MessageInterface::ShowMessage("\n************* End Test #1 ***********\n\n");
   
   //---------------------------------------------------------------------------
   // Test #2, 8 points defining square field of view
   //---------------------------------------------------------------------------
   
   MessageInterface::ShowMessage("************* Starting Test #2 *******\n");
   MessageInterface::ShowMessage("square FOV, test target visibility\n\n");
   
   // setup inputs
   Real f = 20*PI/180;
   Rvector squareConeAngles(8,
                            f*5.0, f*7.0, f*5.0, f*7.0,
                            f*5.0, f*7.0, f*5.0, f*7.0);
   Rvector squareClockAngles(8,
                             0.0, PI/4, 2*PI/4, 3*PI/4,
                             PI,5*PI/4, 6*PI/4, 7*PI/4);
   
   //execute test
#ifdef PRINT_STATE
   TestConstructor(2, squareConeAngles,squareClockAngles);
#endif
   CustomSensor squareSensor (squareConeAngles,squareClockAngles);
   
   // point inside field of view
   Real viewConeAngle = 0.055;
   Real viewClockAngle = 0.0;
   MessageInterface::ShowMessage("Testing point inside FOV\n");
   MessageInterface::ShowMessage("view cone & clock angle=(%13.10f %13.10f)\n",
                                 viewConeAngle,viewClockAngle);

   bool isInView = squareSensor.CheckTargetVisibility(viewConeAngle,
                                                      viewClockAngle);
   // should be true
   if (isInView)
      MessageInterface::ShowMessage("PASS: isInView is true\n");
   else
      MessageInterface::ShowMessage("FAIL: isInView is false\n");
   
   // point outside field of view
   viewConeAngle = 2.0;
   viewClockAngle = 0.0;
   MessageInterface::ShowMessage("Testing point outside FOV\n");
   MessageInterface::ShowMessage("view cone & clock angle=(%13.10f %13.10f)\n",
                                 viewConeAngle,viewClockAngle);

   isInView = squareSensor.CheckTargetVisibility(viewConeAngle,
                                                 viewClockAngle);
   // should be false
   if (isInView)
      MessageInterface::ShowMessage("FAIL: isInView is true\n");
   else
      MessageInterface::ShowMessage("PASS: isInView is false\n");
 
   MessageInterface::ShowMessage("\n************* End Test #2 ***********\n\n");
 
   //---------------------------------------------------------------------------
   // Test #3, 8 points defining diamond field of view
   //---------------------------------------------------------------------------
   
   MessageInterface::ShowMessage("************* Starting Test #3 *******\n");
   MessageInterface::ShowMessage("diamond FOV, test target visibility\n\n");
   // setup inputs
   Real g = PI/60.0;
   Rvector diamondConeAngles(8,
                             g*5.0, g*2.0, g*5.0, g*2.0,
                             g*5.0, g*2.0, g*5.0, g*2.0);
   Rvector diamondClockAngles(8,
                              0.0, PI/4, 2*PI/4, 3*PI/4,
                              PI,5*PI/4, 6*PI/4, 7*PI/4);
   
   // execute test
#ifdef PRINT_STATE
   TestConstructor(3,diamondConeAngles,diamondClockAngles);
#endif
   CustomSensor diamondSensor(diamondConeAngles,diamondClockAngles);
   
   // point inside field of view
   viewConeAngle = 0.21;
   viewClockAngle = PI/2.0+0.055;
   MessageInterface::ShowMessage("Testing point inside FOV\n");
   MessageInterface::ShowMessage("view cone & clock angle=(%13.10f %13.10f)\n",
                                 viewConeAngle,viewClockAngle);
   isInView = diamondSensor.CheckTargetVisibility(viewConeAngle,
                                                 viewClockAngle);
   // should be true
   if (isInView)
      MessageInterface::ShowMessage("PASS: isInView is true\n");
   else
      MessageInterface::ShowMessage("FAIL: isInView is false\n");
   
   // point outside field of view
   viewConeAngle = 0.24;
   viewClockAngle = PI/2.0+0.055;
   MessageInterface::ShowMessage("Testing point outside FOV\n");
   MessageInterface::ShowMessage("view cone & clock angle=(%13.10f %13.10f)\n",
                                 viewConeAngle,viewClockAngle);
 
   isInView = diamondSensor.CheckTargetVisibility(viewConeAngle,
                                                  viewClockAngle);
   
   // should be false
   if (isInView)
      MessageInterface::ShowMessage("FAIL: isInView is true\n");
   else
      MessageInterface::ShowMessage("PASS: isInView is false\n");

   MessageInterface::ShowMessage("\n************* End Test #3 ***********\n\n");

   //---------------------------------------------------------------------------
   // Test #4, 8 points defining "comb" field of view
   //---------------------------------------------------------------------------
   
   MessageInterface::ShowMessage("************* Starting Test #4 *******\n");
   MessageInterface::ShowMessage("comb FOV, target & region visibility\n\n");
   // compute cone and clock angles according to Matlab unit test
   
   /******** This commented out code didn't produce the data Steve had
    ******** sent by e-mail. It was intended to replicate computations in 
    ********matlab unit test drivers
   // set up readable data
   //
   // modify values defined in matlab unit test to account for iterating through
   // yTemp and subtracting 1 from each value
      Real cl = 1.0;
   Real cu = 5.0;
   Rvector xTemp (24,
                   5.0,  5.0,  4.0,  4.0,  3.0,  3.0,
                   2.0,  2.0,  1.0,  1.0,  0.0,  0.0,
                  -1.0, -1.0, -2.0, -2.0, -3.0, -3.0,
                  -4.0, -4.0, -5.0, -5.0, -6.0,  0.0);
   
   Rvector yTemp (24,
                  -1.0, cu, cu, cl, cl, cu, cu, cl, cl, cu, cu, cl,
                    cl, cu, cu, cl, cl, cu, cu, cl, cl, cu, cu, -1.0);
   Rvector combConeAngle(24);
   Rvector combClockAngle(24);
   
   // now compute cone & clock angles
   for (int i= 0; i < 24; i++)
   {
      combConeAngle[i] = sqrt(pow(xTemp[i],2)+pow(yTemp[i],2))/20.0;
      combClockAngle[i] = GmatMathUtil::ATan2(yTemp[i],xTemp[i]);
   }
   ***************************************************/
   
   // *** hardcode cone & clock angles from e-mailed data
   
   Rvector combConeAngle(24,
                         0.254951, 0.353553, 0.320156, 0.206155, 0.158114,
                         0.291548, 0.269258, 0.111803, 0.070711, 0.254951,
                         0.250000, 0.050000, 0.070711, 0.254951, 0.269258,
                         0.111803, 0.158114, 0.291548, 0.320156, 0.206155,
                         0.254951, 0.353553, 0.390512, 0.304138);
   Rvector combClockAngle(24,
                          6.085790, 0.785398, 0.896055, 0.244979, 0.321751,
                          1.030377, 1.190290, 0.463648, 0.785398, 1.373401,
                          1.570796, 1.570796, 2.356194, 1.768192, 1.951303,
                          2.677945, 2.819842, 2.111216, 2.245537, 2.896614,
                          2.944197, 2.356194, 2.446854, 3.306741);
   
   //execute test
#ifdef PRINT_STATE
   TestConstructor(4,combConeAngle,combClockAngle);
#endif
   CustomSensor combSensor(combConeAngle,combClockAngle);
   
   // Target visibility test ****
   // point inside field of view
   viewConeAngle = 0.21;
   viewClockAngle = PI/2.0-0.015;
   MessageInterface::ShowMessage("Testing point inside FOV\n");
   MessageInterface::ShowMessage("view cone & clock angle=(%13.10f %13.10f)\n",
                                 viewConeAngle,viewClockAngle);
   isInView = combSensor.CheckTargetVisibility(viewConeAngle,
                                                  viewClockAngle);
   // should be true
   if (isInView)
      MessageInterface::ShowMessage("PASS: isInView is true\n");
   else
      MessageInterface::ShowMessage("FAIL: isInView is false\n");
   
   // point outside field of view
   viewConeAngle = 0.24;
   viewClockAngle = PI/2.0+0.055;
   MessageInterface::ShowMessage("Testing point outside FOV\n");
   MessageInterface::ShowMessage("view cone & clock angle=(%13.10f %13.10f)\n",
                                 viewConeAngle,viewClockAngle);
   
   isInView = combSensor.CheckTargetVisibility(viewConeAngle,
                                                  viewClockAngle);
   // should be false
   if (isInView)
      MessageInterface::ShowMessage("FAIL: isInView is true\n");
   else
      MessageInterface::ShowMessage("PASS: isInView is false\n");
   
   // Region visibility test ****
   // region inside FOV
   Rvector regionConeAngleVecIn(3, 0.1047, 0.0436, 0.1047);
   Rvector regionClockAngleVecIn(3,2.5409, 4.4608, 6.3807);
   MessageInterface::ShowMessage("Testing region inside FOV\n");
   for (int i = 0; i < regionConeAngleVecIn.GetSize(); i++)
      MessageInterface::ShowMessage(
                              "region cone & clock angle=(%13.10f %13.10f)\n",
                              regionConeAngleVecIn[i],
                                    regionClockAngleVecIn[i]);
   
   isInView = combSensor.CheckRegionVisibility(regionConeAngleVecIn,
                                               regionClockAngleVecIn);
   // should be true
   if (isInView)
      MessageInterface::ShowMessage("PASS: isInView is true\n");
   else
      MessageInterface::ShowMessage("FAIL: isInView is false\n");
   
   // region outside FOV
   Rvector regionConeAngleVecOut(3,  0.1047, 0.0611, 0.1047);
   Rvector regionClockAngleVecOut(3, 0.3491, 1.8708, 3.4907);
   MessageInterface::ShowMessage("Testing region outside FOV\n");
   for (int i = 0; i < regionConeAngleVecIn.GetSize(); i++)
      MessageInterface::ShowMessage(
                              "region cone & clock angle=(%13.10f %13.10f)\n",
                                    regionConeAngleVecOut[i],
                                    regionClockAngleVecOut[i]);
   
   isInView = combSensor.CheckRegionVisibility(regionConeAngleVecOut,
                                               regionClockAngleVecOut);
   // should be flase
   if (isInView)
      MessageInterface::ShowMessage("FAIL: isInView is true\n");
   else
      MessageInterface::ShowMessage("PASS: isInView is false\n");
   
   // ALL TEST RUNS HAVE COMPLETED
   
   MessageInterface::ShowMessage("\n************* End Test #4 ***********\n\n");
   MessageInterface::ShowMessage("*********** END OF TESTING **********\n");

} // end main