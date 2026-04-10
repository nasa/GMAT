//------------------------------------------------------------------------------
//                               SystemTest_Analysis
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2016.05.31
//
/**
 * System tester for analysis
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
#include "Earth.hpp"
#include "KeyValueStatistics.hpp"
#include "VisiblePOIReport.hpp"
#include "OrbitState.hpp"
#include "PointGroup.hpp"
#include "Propagator.hpp"
#include "ConicalSensor.hpp"
#include "CustomSensor.hpp"
#include "RectangularSensor.hpp"
#include "CoverageChecker.hpp"
#include "TimeTypes.hpp"


using namespace std;
using namespace GmatMathUtil;
using namespace GmatMathConstants;

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   //  User config.  
   //   caseFlag 1 uses conical sensor with 10 second step size
   //   caseFlag 2 uses custom sensor configured to mimic conical with 
   //              10 second step size
   //   caseFlag 3 uses TROPICS sensor with 2.0 second step size 

   Integer caseFlag = 1;
   Integer flagIn;
   cout << "Enter caseFlag >";
   cin >> flagIn;
   if ( (flagIn > 4) || (flagIn < 1) )
      cout << "Invalid input, using caseFlag=1\n";
   else
      caseFlag = flagIn;

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
      MessageInterface::ShowMessage("*** TEST*** Analysis!!!!\n");
      
      // This is a usage example that drives O-C code and computes standard
      //  statistical products typical of O-C analysis.  This script is an example
      //  of how R-M might use the O-C data.
      
      AbsoluteDate  *date;
      OrbitState    *state;
      ConicalSensor *conicalSensor;
      Spacecraft    *sat1;
      Propagator    *prop;
      PointGroup    *pGroup;
	   Earth         *earth;
      CustomSensor  *customSensor;
      CustomSensor  *tropicsSensor;
      RectangularSensor *tropicsBoxSensor;

      std::vector<IntervalEventReport> coverageEvents;

      
      clock_t t0 = clock();
      Integer numIter = 1;

      for (Integer ii = 0; ii < numIter; ii++) // **********
      {
      
      bool showPlots = false;

	   // Create an Earth model
	   earth = new Earth();

      // Create the epoch object and set the initial epoch
      date = new AbsoluteDate();
      date->SetGregorianDate(2017, 1, 15, 22, 30, 20.111);
      
//      MessageInterface::ShowMessage(" --- date created\n");
      
      // Create the spacecraft state object and set Keplerian elements
      state = new OrbitState();
      state->SetKeplerianState(6900.0, 0.000, 90.0*RAD_PER_DEG,
          0, 0, 90.0*RAD_PER_DEG);
      
//      MessageInterface::ShowMessage(" --- state created\n");
      
      // Create a conical sensor with FOV of coneAngle  -- case 1
      Real coneAngle = 30*PI/180;
      conicalSensor = new ConicalSensor(coneAngle);

      // Create a custom sensor with FOV of coneAngle -- case 2
      Rvector coneAngleVec(20, coneAngle, coneAngle, coneAngle, coneAngle, 
          coneAngle, coneAngle, coneAngle, coneAngle, coneAngle, coneAngle, 
          coneAngle, coneAngle, coneAngle, coneAngle, coneAngle, coneAngle, 
          coneAngle, coneAngle, coneAngle, coneAngle);
      Rvector clockAngleVec(20, 0, 0.330693963535768, 0.661387927071536, 0.992081890607304, 1.32277585414307,
          1.65346981767884, 1.98416378121461, 2.31485774475038, 2.64555170828614, 2.97624567182191,
          3.30693963535768, 3.63763359889345, 3.96832756242922, 4.29902152596499, 4.62971548950075,
          4.96040945303652, 5.29110341657229, 5.62179738010806, 5.95249134364382, 2*PI);
      customSensor = new CustomSensor(coneAngleVec,clockAngleVec);

      //  TROPICS sensor - case 3
      Rvector tropicsConeAngle(5, 0.9949922073045261, 0.9949922073045261, 0.9949922073045261,
          0.9949922073045261, 0.9949922073045261);
      Rvector tropicsClockAngle(5, 0.02181442636896036, 3.119778227220833, 3.163407079958754,
          -0.02181442636896036, 0.02181442636896036);
      tropicsSensor = new CustomSensor(tropicsConeAngle, tropicsClockAngle);

      // TROPICS box -- case 4
         tropicsBoxSensor = new RectangularSensor(0.9949922073045261, 0.02181442636896036);
      
      // Create a spacecraft giving it a state and epoch
      sat1 = new Spacecraft(date,state);

      //  Add the sensor to the spacecraft
      Real stepSize = 0;
      if (caseFlag == 1){
          sat1->AddSensor(conicalSensor);
          stepSize = 10.0;
      }
      else if (caseFlag == 2) {
          sat1->AddSensor(customSensor);
          stepSize = 10.0;
      }
      else if (caseFlag == 3){
          sat1->AddSensor(tropicsSensor);
          stepSize = 2.0;
      }
      else if (caseFlag == 4) {
         sat1->AddSensor(tropicsBoxSensor);
         stepSize = 2.0;
      }
      
      // Create the propagator
      prop = new Propagator(sat1);
      
//      MessageInterface::ShowMessage(" --- propagator created\n");
      
      // Create the point group and initialize the coverage checker
      pGroup = new PointGroup();
      pGroup->AddHelicalPointsByNumPoints(200);
      CoverageChecker *covChecker = new CoverageChecker(pGroup,sat1);
      covChecker->SetComputePOIGeomtryData(true);
      
 
      // Propagate for a duration and collect data
      Real    startDate = date->GetJulianDate();
      Integer count     = 0;
      // Over 1 day
      while (date->GetJulianDate() < ((Real)startDate + 5.0))
      {

          // Compute points in view
          IntegerArray accessPoints = covChecker->AccumulateCoverageData();

//         MessageInterface::ShowMessage(" --- about to advance the time and propagate\n");
         // Propagate
         date->Advance(stepSize);
         prop->Propagate(*date);
//         MessageInterface::ShowMessage(" --- done advancing the time and propagating\n");
      
         // Compute lat., lon., and height of s/c w/r/t the ellipsoid
         Real     jDate = sat1->GetJulianDate();
         Rvector6 cartState = sat1->GetCartesianState();
         Rvector3 inertialPosVec(cartState(0), cartState(1), cartState(2));
         Rvector3 latLonHeight = earth->InertialToBodyFixed(inertialPosVec, jDate, "Ellipsoid");

      }
      
//      MessageInterface::ShowMessage(" --- propagation completed\n");
         
         
      
      // Compute coverage data
      t0 = clock();
      coverageEvents = covChecker->ProcessCoverageData();

      
      
//      MessageInterface::ShowMessage(" --- ProcessCoverageData completed (%d reports)\n",
//                                    (Integer) coverageEvents.size());

      } // *****
      Real timeSpent = ((Real) (clock() - t0)) / CLOCKS_PER_SEC;
      //MessageInterface::ShowMessage("TIME SPENT in %d iterations is %12.10f seconds\n",
      //                            numIter,timeSpent);
      MessageInterface::ShowMessage("Time spent in coverage code is %12.8f sec",
                                    timeSpent);
                                    
      
      RealArray lonVec;
      RealArray latVec;
      // Compute coverate stats.  Shows how R-M might use data for coverage analysis
      // start the timer   ******
      
      // Create Lat\Lon Grid
      for (Integer pointIdx = 0; pointIdx < pGroup->GetNumPoints(); pointIdx++)
      {
         Rvector3 *vec = pGroup->GetPointPositionVector(pointIdx);
         lonVec.push_back(ATan(vec->GetElement(1),vec->GetElement(0))*DEG_PER_RAD);
         latVec.push_back(ASin(vec->GetElement(2)/vec->GetMagnitude())*DEG_PER_RAD);
      }
      
      MessageInterface::ShowMessage(" --- lat/long set-up completed\n");
      
      // Compute total coverage statistics from all coverage events
      Rvector totalCoverageDuration(pGroup->GetNumPoints());
      IntegerArray numPassVec;  // (pGroup->GetNumPoints());
      for (Integer ii = 0; ii < pGroup->GetNumPoints(); ii++)
         numPassVec.push_back(0);
      Rvector minPassVec(pGroup->GetNumPoints());
      Rvector maxPassVec(pGroup->GetNumPoints());
      for (UnsignedInt eventIdx = 0; eventIdx < coverageEvents.size(); eventIdx++)
      {
         IntervalEventReport currEvent = coverageEvents.at(eventIdx);
         std::vector<VisiblePOIReport> discreteEvents = currEvent.GetPOIEvents();
         Integer          poiIndex       = currEvent.GetPOIIndex();
         Real             eventDuration  = (currEvent.GetEndDate().GetJulianDate() -
                                            currEvent.GetStartDate().GetJulianDate()) * 24.0;

         totalCoverageDuration(poiIndex) = totalCoverageDuration(poiIndex) + eventDuration;
//         Rvector3 vec = pGroup->GetPointPositionVector(pointIdx); // ?
         
//      %     lat = atan2(vec(2),vec(1))*180/pi;
//      %     lon = asin(vec(3)/norm(vec));
//      %     Z(lat,lon) = totalCoverageDuration;
      
      // Save the maximum duration if necessary
         if (eventDuration > maxPassVec(poiIndex))
            maxPassVec(poiIndex) = eventDuration;

      
         if (minPassVec(poiIndex) == 0 ||  (eventDuration< maxPassVec(poiIndex)))
            minPassVec(poiIndex) = eventDuration;

         numPassVec.at(poiIndex) = numPassVec.at(poiIndex) + 1;
      }
      
      // *********** display stuff ***********
      // Write the simple coverage report to the MATLAB command window
      MessageInterface::ShowMessage("       =======================================================================\n");
      MessageInterface::ShowMessage("       ==================== Brief Coverage Analysis Report ===================\n");
      MessageInterface::ShowMessage("       lat (deg): Latitude of point in degrees                  \n");
      MessageInterface::ShowMessage("       lon (deg): Longitude of point in degrees                  \n");
      MessageInterface::ShowMessage("       numPasses: Number of total passes seen by a point                           \n");
      MessageInterface::ShowMessage("       totalDur : Total duration point was observed in hours                         \n");
      MessageInterface::ShowMessage("       minDur   : Duration of the shortest pass in minutes                         \n");
      MessageInterface::ShowMessage("       maxDur   : Duration of the longest pass in hours                            \n");
      MessageInterface::ShowMessage("       =======================================================================\n");
      MessageInterface::ShowMessage("       =======================================================================\n");
      MessageInterface::ShowMessage("  ");
      
//      data = [latVec,lonVec, numPassVec, totalCoverageDuration, minPassVec, maxPassVec];  <<<<<<<<<
      Integer headerCount = 1;
      Integer dataEnd     = 0;
      for (Integer passIdx = 0; passIdx < pGroup->GetNumPoints(); passIdx+= 10)
      {
         MessageInterface::ShowMessage("       lat (deg)     lon (deg)       numPasses  totalDur    minDur      maxDur\n");
         dataEnd = passIdx + 10;  // 9;
         for (Integer ii = 0; ii < 10; ii++)  // 9
            MessageInterface::ShowMessage("       %le    %le    %d    %le    %le    %le \n",
//            MessageInterface::ShowMessage("       %12.10f    %12.10f    %d    %12.10f    %12.10f    %12.10f \n",
                                          latVec.at(passIdx+ii),
                                          lonVec.at(passIdx+ii),
                                          numPassVec.at(passIdx+ii),
                                          totalCoverageDuration(passIdx+ii),
                                          minPassVec(passIdx+ii),
                                          maxPassVec(passIdx+ii));

      }

      if (dataEnd + 1 < pGroup->GetNumPoints())
      {
         MessageInterface::ShowMessage("       lat (deg)    lon (deg)     numPasses   totalDur    minDur      maxDur\n");
         for (Integer ii = dataEnd; ii < pGroup->GetNumPoints(); ii++)
            MessageInterface::ShowMessage("       %le    %le    %d    %le    %le    %le \n",
//            MessageInterface::ShowMessage("       %12.10f    %12.10f    %d    %12.10f    %12.10f    %12.10f \n",
                                          latVec.at(ii),
                                          lonVec.at(ii),
                                          numPassVec.at(ii),
                                          totalCoverageDuration(ii),
                                          minPassVec(ii),
                                          maxPassVec(ii));
      }

      
      cout << endl;
      cout << "Hit enter to end" << endl;
      cin.get();
      
      // delete pointers here
      
      MessageInterface::ShowMessage("*** END TEST ***\n");
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n", be.GetFullMessage().c_str());
   }
   
}
