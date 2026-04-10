//$Id$
//------------------------------------------------------------------------------
//                               TestAbsoluteDate
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2016.05.12
//
/**
 * Unit-test driver for the PointGroup
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "AbsoluteDate.hpp"
#include "Spacecraft.hpp"
#include "OrbitState.hpp"
#include "PointGroup.hpp"
#include "TimeTypes.hpp"


using namespace std;
using namespace GmatMathUtil;
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
      // Test the PointGroup
      MessageInterface::ShowMessage("*** TEST*** PointGroup\n");
      
//      AbsoluteDate date;
//      OrbitState   state;
//      Spacecraft   *sat = new Spacecraft(date, state);
      
      // Create a point group object with 50 points
      Integer testPointNum = 50;
      PointGroup pg;
      
      pg.AddHelicalPointsByNumPoints(testPointNum);
      
      // Test stored number of points
      Integer numPoints = pg.GetNumPoints();
      if (numPoints != testPointNum)
      {
         MessageInterface::ShowMessage(
               "*** ERROR - error in returned value for num points (%d)\n",
               pg.GetNumPoints());
         exit(0);
      }
      else
      {
         MessageInterface::ShowMessage("OK - got back the expected number of points!\n");
      }

      
      // Test the returned values for point locations.  Note (SPH)
      // this is a semi-rigorous test.  I plotted the points and they look correct,
      // and I then outputted the data and am using it as truth.
      Rmatrix  truthData(testPointNum, 3,
                         0.0,                        0.0,                       1.0,
                         0.0,                        0.0,                      -1.0,
                         0.433883739117558,                       0.0,         0.900968867902419,
                         0.134077448970272,          0.41264795740226,         0.900968867902419,
                         -0.351019318529051,         0.255030463062816,         0.900968867902419,
                         -0.351019318529051,        -0.255030463062815,         0.900968867902419,
                         0.134077448970272,         -0.41264795740226,         0.900968867902419,
                         0.433883739117558,                       0.0,        -0.900968867902419,
                         0.134077448970272,          0.41264795740226,        -0.900968867902419,
                         -0.351019318529051,         0.255030463062816,        -0.900968867902419,
                         -0.351019318529051,        -0.255030463062815,        -0.900968867902419,
                         0.134077448970272,         -0.41264795740226,        -0.900968867902419,
                         0.78183148246803,                       0.0,         0.623489801858733,
                         0.552838342998275,         0.552838342998275,         0.623489801858733,
                         4.78733711238551e-17,          0.78183148246803,         0.623489801858733,
                         -0.552838342998275,         0.552838342998275,         0.623489801858733,
                         -0.78183148246803,      9.57467422477103e-17,         0.623489801858733,
                         -0.552838342998275,        -0.552838342998275,         0.623489801858733,
                         -1.43620113371565e-16,         -0.78183148246803,         0.623489801858733,
                         0.552838342998275,        -0.552838342998275,         0.623489801858733,
                         0.78183148246803,                       0.0,        -0.623489801858733,
                         0.598917662600107,         0.502551589793308,        -0.623489801858733,
                         0.135763612173208,         0.769953705483544,        -0.623489801858733,
                         -0.390915741234015,         0.677085925295762,        -0.623489801858733,
                         -0.734681274773315,         0.267402115690236,        -0.623489801858733,
                         -0.734681274773315,        -0.267402115690236,        -0.623489801858733,
                         -0.390915741234015,        -0.677085925295762,        -0.623489801858733,
                         0.135763612173208,        -0.769953705483544,        -0.623489801858733,
                         0.598917662600107,        -0.502551589793309,        -0.623489801858733,
                         0.974927912181824,                       0.0,         0.222520933956314,
                         0.820161550378687,          0.52708582340226,         0.222520933956314,
                         0.404999691314914,         0.886825622084767,         0.222520933956314,
                         -0.138746708150268,          0.96500455176578,         0.222520933956314,
                         -0.638442008115133,         0.736801354657499,         0.222520933956314,
                         -0.935436481519112,         0.274668933435062,         0.222520933956314,
                         -0.935436481519112,        -0.274668933435062,         0.222520933956314,
                         -0.638442008115134,        -0.736801354657499,         0.222520933956314,
                         -0.138746708150268,         -0.96500455176578,         0.222520933956314,
                         0.404999691314914,        -0.886825622084767,         0.222520933956314,
                         0.820161550378687,         -0.52708582340226,         0.222520933956314,
                         0.974927912181824,                       0.0,        -0.222520933956314,
                         0.788733249245582,         0.573048248828767,        -0.222520933956314,
                         0.30126929315467,         0.927211543798553,        -0.222520933956314,
                         -0.30126929315467,         0.927211543798553,        -0.222520933956314,
                         -0.788733249245582,         0.573048248828767,        -0.222520933956314,
                         -0.974927912181824,      1.19394234705288e-16,        -0.222520933956314,
                         -0.788733249245582,        -0.573048248828767,        -0.222520933956314,
                         -0.30126929315467,        -0.927211543798553,        -0.222520933956314,
                         0.30126929315467,        -0.927211543798553,        -0.222520933956314,
                         0.788733249245582,        -0.573048248828767,        -0.222520933956314);
      truthData = truthData * 6378.1363;
      
      Real maxDiff = -INFINITY;
      for (Integer pointIdx = 0; pointIdx < testPointNum; pointIdx++)
      {
         Rvector3 truthPos3(truthData.GetElement(pointIdx,0),
                            truthData.GetElement(pointIdx,1),
                            truthData.GetElement(pointIdx,2));
         Rvector3 *ptPos = pg.GetPointPositionVector(pointIdx);
         Rvector3 diffVec = truthPos3 - (*ptPos);
//         MessageInterface::ShowMessage("  point position vector values are : %12.10f  %12.10f  %12.10f\n",
//                                       ptPos->GetElement(0),
//                                       ptPos->GetElement(1),
//                                       ptPos->GetElement(2));

         Real diff = diffVec.GetMagnitude();
         if (diff > maxDiff)
            maxDiff = diff;

      }
      if (maxDiff >= 1e-11)
      {
         MessageInterface::ShowMessage("*** ERROR - maxDiff for position is too great\n");
         exit(0);
      }
      else
      {
         MessageInterface::ShowMessage("OK - maxDiff is OK between truth data and data!\n");
      }
      
      // Test lat/lon contraint setting
      Real latUpper = PI/3;
      Real latLower = -PI/3;
      Real lonUpper = 2*PI - PI/6;
      Real lonLower = PI/6;
      PointGroup pg2;
      pg2.SetLatLonBounds(latUpper,latLower,lonUpper,lonLower);
      pg2.AddHelicalPointsByNumPoints(testPointNum);
      RealArray latVec;
      RealArray lonVec;
      pg2.GetLatLonVectors(latVec, lonVec);
      
      for (Integer pointIdx = 0; pointIdx < pg2.GetNumPoints(); pointIdx++)
      {
         if ((latVec.at(pointIdx) < latLower || latVec.at(pointIdx) > latUpper) ||
             (lonVec.at(pointIdx) < lonLower || lonVec.at(pointIdx) > lonUpper))
         {
            MessageInterface::ShowMessage(
                  "*** ERROR - latitude and or longitude violates constraint\n");
            exit(0);
         }
     }
      MessageInterface::ShowMessage("OK - lat and lon are not out-of-range!\n");
      
      //  Test setting your own lat lon using those from the previous test
      PointGroup pgCustom;
      pgCustom.AddUserDefinedPoints(latVec,lonVec);
      if (pg2.GetNumPoints() != pgCustom.GetNumPoints())
      {
         MessageInterface::ShowMessage(
                "*** ERROR - error setting user defined points\n");
         exit(0);
      }
      else
      {
         MessageInterface::ShowMessage("OK - setting user-defined points is OK!\n");
      }
      RealArray latVec2;
      RealArray lonVec2;
      pgCustom.GetLatLonVectors(latVec2, lonVec2);
      for (Integer pointIdx = 0; pointIdx < pgCustom.GetNumPoints(); pointIdx++)
      {
         if (latVec2.at(pointIdx) != latVec.at(pointIdx))
         {
            MessageInterface::ShowMessage(
                                          "*** ERROR - error setting user defined points (lat)\n");
            exit(0);
         }
         if (lonVec2.at(pointIdx) != lonVec.at(pointIdx))
         {
            MessageInterface::ShowMessage(
                                          "*** ERROR - error setting user defined points (lon)\n");
            exit(0);
         }
      }
      MessageInterface::ShowMessage("OK - setting/getting user-defined points is OK!\n");
      
      //  Test setting points based on angular separation
      PointGroup pgByAngle;
      Real angle = 1*PI/180;
      pgByAngle.AddHelicalPointsByAngle(angle);
      Rvector3 *v1 = pgByAngle.GetPointPositionVector(3);
      Rvector3 *v2 = pgByAngle.GetPointPositionVector(4);
      Real angleOut = GmatMathUtil::ACos(((*v1)*(*v2))/v1->GetMagnitude()/v2->GetMagnitude());
      if (GmatMathUtil::Abs(angle - angleOut) > 1e-6)
      {
         MessageInterface::ShowMessage(
                                       "*** ERROR - error in angle between points when setting based on angle\n");
         exit(0);
      }
      MessageInterface::ShowMessage("OK - setting by angular separation is OK!\n");
      
                     
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