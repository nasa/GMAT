//$Id$
//------------------------------------------------------------------------------
//                               TestAbsoluteDate
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2016.05.11
//
/**
 * Unit-test driver for the AbsoluteDate
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
#include "TimeTypes.hpp"


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
      // Test the AbsoluteDate
      MessageInterface::ShowMessage("*** TEST*** AbsoluteDate\n");

      // Create the AbsoluteDate
      AbsoluteDate date;
      // Set the Gregorian date and test conversion to Julian Date
      date.SetGregorianDate(2017, 1, 15, 22, 30, 20.111);
      Real jd        = date.GetJulianDate();
      Real truthDate = 27769.4377327662 + 2430000;
      
      if (GmatMathUtil::IsEqual(truthDate, jd))
         MessageInterface::ShowMessage("OK - gregorian to julian date is correct!!\n");
      else
         MessageInterface::ShowMessage("*** ERROR - julian date is incorrect!!\n");
      
      // Set the julian date and test conversion to Gregorian date
      date.SetJulianDate(2457269.123456789);
      Rvector6 greg  = date.GetGregorianDate();
      Integer yr     = 2015;
      Integer mon    = 9;
      Integer day    = 3;
      Integer hr     = 14;
      Integer min    = 57;
      Real    sec    = 46.6665852069856;
      bool    gregOK = true;
      if ((Integer) greg[0] != yr)
      {
         gregOK = false;
         MessageInterface::ShowMessage("*** ERROR - gregorian (year) is incorrect!!\n");
      }
      if ((Integer) greg[1] != mon)
      {
         gregOK = false;
         MessageInterface::ShowMessage("*** ERROR - gregorian (month) is incorrect!!\n");
      }
      if ((Integer) greg[2] != day)
      {
         gregOK = false;
         MessageInterface::ShowMessage("*** ERROR - gregorian (day) is incorrect!!\n");
      }
      if ((Integer) greg[3] != hr)
      {
         gregOK = false;
         MessageInterface::ShowMessage("*** ERROR - gregorian (hour) (%d) is incorrect!!\n",
                                       greg[3]);
      }
      if ((Integer) greg[4] != min)
      {
         gregOK = false;
         MessageInterface::ShowMessage("*** ERROR - gregorian (minute) (%d) is incorrect!!\n",
                                       greg[4]);
      }
      if (!GmatMathUtil::IsEqual(sec, greg[5], 1.0e-13))
      {
         gregOK = false;
         MessageInterface::ShowMessage("*** ERROR - gregorian (second) (%16.14f) (%16.14f) is incorrect!!\n",
                                       greg[5], sec);
      }
      if (gregOK)
         MessageInterface::ShowMessage("OK - julian to gregorian date is correct!!\n");
     
      
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