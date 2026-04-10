//------------------------------------------------------------------------------
//                         TestHyperSensitive
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
// Author: Youngkwang Kim
// Created: 2017.02.20
//
/**
 * Test driver for LobattoIIIaMathUtil.
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
#include "ImplicitRKPhase.hpp"
#include "RadauPhase.hpp"
#include "Trajectory.hpp"
//#include "Phase.hpp"
#include "HyperSensitivePointObject.hpp"
#include "HyperSensitivePathObject.hpp"
#include "boost/config.hpp"  // BOOST
#include "LobattoIIIaMathUtil.hpp"

static const Real INF = std::numeric_limits<Real>::infinity();

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

Rvector dummyFunction(Real time)
{
   Rvector output(3);

   output(0) = sin(time);
   output(1) = cos(time);
   output(2) = tan(time / 4.0);
   return output;
}

Rvector dummyFunction2(Real time)
{
   Rvector output(1);

   output(0) = sin(time);
   return output;
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
   MessageInterface::ShowMessage("*** TESTing LobattoIIIaMathUtil ***\n");
   
   try
   {
      // test romberg integration
      Rvector (*pointer)(Real) = &dummyFunction;
      //Rvector output = LobattoIIIaMathUtil::GetRombergIntegration(pointer, 3, 0, PI, 8);

      //MessageInterface::ShowMessage("*** TEST *** Integration Results are:\n%s\n",
      //   output.ToString(16).c_str());

      // test convolution
      {
         Rvector x(4, 0.0, 1.0, 2.0, 3.0), h(3, 4.0, 5.0, 6.0);
         Rvector z = LobattoIIIaMathUtil::convolution(x, h);      
         MessageInterface::ShowMessage("*** TEST *** Convolution Results are:\n%s\n",
            z.ToString(16).c_str());
      }
      
      // test hermite interpolation
      {
         Rvector x(5, 0.0, 1.0, 2.5, 3.0, 6.0), y(5, 5.0, 6.0, 7.0, 8.0, 0.0), yp(5, -2.0, 3.0, 1.0, -3.0, 0.5);
         Rvector hp = LobattoIIIaMathUtil::GetHermiteCoeff(&x, &y, &yp);
         MessageInterface::ShowMessage("*** TEST *** Hermite interpolation coefficients are:\n%s\n", hp.ToString(16).c_str());
         Real fValue = LobattoIIIaMathUtil::ComputeFunctionValue(1.234, hp);
         Real fDotValue = LobattoIIIaMathUtil::ComputeDerivativeValue(1.234, hp);
         MessageInterface::ShowMessage("*** TEST *** Hermite interpolation coefficients are:\n%f\n", fValue);
         MessageInterface::ShowMessage("*** TEST *** Hermite interpolation coefficients are:\n%f\n", fDotValue);
      }
      cout << "Hit enter to end" << endl;
      cin.get();
      
      MessageInterface::ShowMessage("*** END TEST ***\n");
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n", be.GetFullMessage().c_str());
   }
   
}
