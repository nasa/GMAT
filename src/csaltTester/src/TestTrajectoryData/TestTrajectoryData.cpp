//------------------------------------------------------------------------------
//                         TestTrajectoryData
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
// Author: Jeremy Knittel
// Created: 2017.02.01
//
/**
 * Test driver for TrajectoryData classes.
 */
//------------------------------------------------------------------------------
// global igrid iGfun jGvar traj
#include <iostream>
#include <string>
#include <ctime>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Linear.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "Rmatrix33.hpp"
//#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "FileManager.hpp"
#include "TimeTypes.hpp"
#include "FunctionOutputData.hpp"
#include "PathFunctionContainer.hpp"
#include "PointFunctionContainer.hpp"
#include "UserFunctionManager.hpp"
#include "UserPointFunctionManager.hpp"
#include "UserFunctionProperties.hpp"
#include "RadauPhase.hpp"
#include "Trajectory.hpp"
#include "TrajectoryData.hpp"
#include "ArrayTrajectoryData.hpp"
#include "OCHTrajectoryData.hpp"
// #include "TrajectorySegment.hpp"
//#include "Phase.hpp"
#include "../HelperClasses/BrachistichronePointObject.hpp"
#include "../HelperClasses/BrachistichronePathObject.hpp"
#include "boost/config.hpp"  // BOOST


static const Real INF = std::numeric_limits<Real>::infinity();

static const std::string rootDir = "/Users/wshoan/git/lowthrust/CPlusPlus/test/TestTrajectoryData/";

using namespace std;
using namespace GmatMathConstants;

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
   // Real        tolerance = 1e-15;
   
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
   
   MessageInterface::ShowMessage("*** TESTing Trajectory Data class ***\n");
   
   ArrayTrajectoryData myTrajData = ArrayTrajectoryData();

   MessageInterface::ShowMessage("*** ArrayTrajectoryData created!!\n");

   TrajectoryDataStructure localData;

   MessageInterface::ShowMessage("*** TrajectoryDataStructure created!!\n");

   myTrajData.SetNumSegments(3);
   MessageInterface::ShowMessage("*** ArrayTrajectoryData NumSegments!!\n");
   myTrajData.SetNumStateParams(0,3);
   myTrajData.SetNumControlParams(0,2);
   myTrajData.SetNumIntegralParams(0,2);
   MessageInterface::ShowMessage("*** ArrayTrajectoryData num params!!\n");
   myTrajData.SetNumStateParams(1,1);
   myTrajData.SetNumControlParams(1,0);
   myTrajData.SetNumIntegralParams(1,0);
   myTrajData.SetNumStateParams(2,3);
   myTrajData.SetNumControlParams(2,2);
   myTrajData.SetNumIntegralParams(2,2);

   MessageInterface::ShowMessage("*** ArrayTrajectoryData set up!!\n");

   localData.states.SetSize(3);
   localData.controls.SetSize(2);
   localData.integrals.SetSize(2);

   for (Integer i = 0; i < 11; i++)
   {
       float f = float(i);
       localData.time = f;
       localData.states(0)    = -.3*f*f*f +  4.0*f*f - 15.0*f + 50.0;
       localData.states(1)    =  .1*f*f*f -  2.0*f*f + 17.0*f - 3.3;
       localData.states(2)    =   0*f*f*f - 20.0*f*f -  6.0*f - 30.3;
       localData.controls(0)  = -.1*f*f*f +  2.0*f*f - 17.0*f + 3.3;
       localData.controls(1)  = -.5*f*f*f +  1.0*f*f + 13.0*f - 18.0;
       localData.integrals(0) = -.1*f*f*f -  3.0*f*f +  9.0*f + 13.3;
       localData.integrals(1) =   8*f*f*f - 20.0*f*f + 13.0*f - 18.0;
       myTrajData.AddDataPoint(0,localData);
   }


   localData.states.SetSize(1);
   localData.controls.SetSize(0);
   localData.integrals.SetSize(0);

   for (Integer i = 13; i < 21; i++)
   {
       float f = float(i);
       localData.time = f;
       localData.states(0)    = -.3*f*f*f +  4.0*f*f - 15.0*f + 50.0;
    //    localData.states(1)    =  .1*f*f*f -  2.0*f*f + 17.0*f - 3.3;
    //    localData.states(2)    =   0*f*f*f - 20.0*f*f -  6.0*f - 30.3;
    //    localData.controls(0)  = -.1*f*f*f +  2.0*f*f - 17.0*f + 3.3;
    //    localData.controls(1)  = -.5*f*f*f +  1.0*f*f + 13.0*f - 18.0;
    //    localData.integrals(0) = -.1*f*f*f -  3.0*f*f +  9.0*f + 13.3;
    //    localData.integrals(1) =   8*f*f*f - 20.0*f*f + 13.0*f - 18.0;
       myTrajData.AddDataPoint(1,localData);
   }


   localData.states.SetSize(3);
   localData.controls.SetSize(2);
   localData.integrals.SetSize(2);

   for (Integer i = 20; i < 30; i++)
   {
       float f = float(i);
       localData.time = f;
       localData.states(0)    = -.3*f*f*f +  4.0*f*f - 15.0*f + 50.0;
       localData.states(1)    =  .1*f*f*f -  2.0*f*f + 17.0*f - 3.3;
       localData.states(2)    =   0*f*f*f - 20.0*f*f -  6.0*f - 30.3;
       localData.controls(0)  = -.1*f*f*f +  2.0*f*f - 17.0*f + 3.3;
       localData.controls(1)  = -.5*f*f*f +  1.0*f*f + 13.0*f - 18.0;
       localData.integrals(0) = -.1*f*f*f -  3.0*f*f +  9.0*f + 13.3;
       localData.integrals(1) =   8*f*f*f - 20.0*f*f + 13.0*f - 18.0;
       myTrajData.AddDataPoint(2,localData);
   }
   
   MessageInterface::ShowMessage("*** TESTing Interpolation ***\n");

   Rvector interpTimes;
   interpTimes.SetSize(321);
   for (Integer i = 0; i < 321; i++)
   {
       interpTimes[i] = (float(i)-10)/10;
   }

   myTrajData.SetInterpType(TrajectoryData::SPLINE);
   myTrajData.SetAllowInterSegmentExtrapolation(true);
   myTrajData.SetAllowExtrapolation(true);

   MessageInterface::ShowMessage("*** myTrajData set up with interp times, etc. ***\n");

   std::vector<TrajectoryDataStructure> interpData = myTrajData.Interpolate(interpTimes);

   MessageInterface::ShowMessage("*** Interpolation complete ***\n");

   MessageInterface::ShowMessage("Time\t\tTruth1\t\tTruth2\t\tTruth3\t\tTruth4\t\tTruth5\t\tTruth6\t\tTruth7\t\tState1\t\tState2\t\tState3\t\tControl1\tControl2\tIntegral1\tIntegral2\n");
   for (Integer i = 0; i < 321; i++)
   {
       float f = float(interpTimes[i]);
    //    if (f > 13 && f < 20) f *= -1;
       Real truthValue1    = -.3*f*f*f +  4.0*f*f - 15.0*f + 50.0;
       if (f > 11.5 && f <= 20)
       {
            MessageInterface::ShowMessage("%f\t%f\t%f\n",interpTimes[i],truthValue1,interpData[i].states(0));
       }
       else {
            Real truthValue2    =  .1*f*f*f -  2.0*f*f + 17.0*f - 3.3;
            Real truthValue3    =   0*f*f*f - 20.0*f*f -  6.0*f - 30.3;
            Real truthValue4    = -.1*f*f*f +  2.0*f*f - 17.0*f + 3.3;
            Real truthValue5    = -.5*f*f*f +  1.0*f*f + 13.0*f - 18.0;
            Real truthValue6    = -.1*f*f*f -  3.0*f*f +  9.0*f + 13.3;
            Real truthValue7    =   8*f*f*f - 20.0*f*f + 13.0*f - 18.0;
            MessageInterface::ShowMessage("%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t\n",interpTimes[i],truthValue1,truthValue2,truthValue3,truthValue4,truthValue5,truthValue6,truthValue7,interpData[i].states(0),interpData[i].states(1),interpData[i].states(2),interpData[i].controls(0),interpData[i].controls(1),interpData[i].integrals(0),interpData[i].integrals(1));
       }
   }

   cout << endl;
   cout << endl;

   MessageInterface::ShowMessage("*** TESTing OCH Trajectory Data ***\n");

   Rvector ochTimes;
   ochTimes.SetSize(120);
   for (Integer i = 0; i < 100; i++)
   {
       ochTimes(i) = (21545.11518363824 - 21545.00069444444) * float(i) / 100.0 + 21545.00069444444;
   }
   for (Integer i = 100; i < 120; i++)
   {
       ochTimes(i) = (21546.013586 - 21546.00069444444) * float(i) / 100.0 + 21546.00069444444;
   }

   try
   {
      OCHTrajectoryData myOCHdata = OCHTrajectoryData(rootDir + "OCHistoryFileExample.och");

      myOCHdata.WriteToFile(rootDir + "OCHistoryOutputExample.och");

      myOCHdata.SetInterpType(TrajectoryData::NOTAKNOT);
      myOCHdata.SetAllowInterSegmentExtrapolation(true);
      myOCHdata.SetAllowExtrapolation(true);

      std::vector<TrajectoryDataStructure> interpOCHData = myOCHdata.Interpolate(ochTimes);

      for (Integer i = 0; i < 120; i++)
      {
          MessageInterface::ShowMessage("%f\t",ochTimes[i]);

          for (Integer idx = 0; idx < interpOCHData[i].states.GetSize(); idx++)
          {
               MessageInterface::ShowMessage("%f\t",interpOCHData[i].states(idx));    
          }
          for (Integer idx = 0; idx < interpOCHData[i].controls.GetSize(); idx++)
          {
               MessageInterface::ShowMessage("%f\t",interpOCHData[i].controls(idx));    
          }
          for (Integer idx = 0; idx < interpOCHData[i].integrals.GetSize(); idx++)
          {
               MessageInterface::ShowMessage("%f\t",interpOCHData[i].integrals(idx));    
          }

          MessageInterface::ShowMessage("\n");
      }

      cout << endl;
      cout << endl;
      MessageInterface::ShowMessage("*** TESTing OCH Trajectory Data round 2***\n");
      cout << endl;
      cout << endl;

      myOCHdata.ReadFromFile(rootDir + "OCHistoryOutputExample.och");

      myOCHdata.SetInterpType(TrajectoryData::NOTAKNOT);
      myOCHdata.SetAllowInterSegmentExtrapolation(true);
      myOCHdata.SetAllowExtrapolation(true);

   //    interpOCHData = myOCHdata.Interpolate(ochTimes);
      Rmatrix stateData = myOCHdata.GetState(ochTimes);
      Rmatrix controlData = myOCHdata.GetControl(ochTimes);
      Rmatrix integralData = myOCHdata.GetIntegral(ochTimes);
      Integer numStateParams,numTimes,numControlParams,numIntegralParams;
      stateData.GetSize(numTimes,numStateParams);
      controlData.GetSize(numTimes,numControlParams);
      integralData.GetSize(numTimes,numIntegralParams);

      for (Integer idx = 0; idx < 120; idx++)
      {
          MessageInterface::ShowMessage("%f\t",ochTimes[idx]);

          for (Integer i = 0; i < numStateParams; i++)
          {
               MessageInterface::ShowMessage("%f\t",stateData(idx,i));    
          }
          for (Integer i = 0; i < numControlParams; i++)
          {
               MessageInterface::ShowMessage("%f\t",controlData(idx,i));    
          }
          for (Integer i = 0; i < numIntegralParams; i++)
          {
               MessageInterface::ShowMessage("%f\t",integralData(idx,i));    
          }

          MessageInterface::ShowMessage("\n");
      }

   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("ERROR! exception: %s\n", be.GetFullMessage().c_str());
   }

   cout << endl;
   cout << endl;
   MessageInterface::ShowMessage("*** TESTing OCH Trajectory Data round 3***\n");
   cout << endl;
   cout << endl;

// @todo this is not how we want to copy data from one child class to another (data loss, incompatibility, etc.) <- FIX THIS
//   OCHTrajectoryData myOCHData2 = OCHTrajectoryData(myTrajData); // ?????????
//   myOCHData2.WriteToFile("/Users/jknittel/Collocation/LowThrust/lowthrust/CPlusPlus/test/TestTrajectoryData/OCHExample2.och");

   myTrajData.WriteToFile(rootDir + "OCHExample2.och");

   cout << endl;
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
   
   MessageInterface::ShowMessage("*** END TEST ***\n");   
}
