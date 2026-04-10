//------------------------------------------------------------------------------
//                                  TestTime
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
// Author: Linda Jun
// Created: 2005/02/14
// Modification:
//   2005/02/14  L. Jun - Moved time related test cases from TestUtil.cpp
//   2005/02/14  A. Greene - Added time conversion testing
//
/**
 * Test driver for time related classes.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "RealTypes.hpp"
#include "TimeTypes.hpp"
#include "ArrayTemplate.hpp"
#include "ElapsedTime.hpp"
#include "A1Mjd.hpp"
#include "LeapSecsFileReader.hpp"
#include "TimeSystemConverter.hpp"
#include "EopFile.hpp"
#include "TestOutput.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;
using namespace TimeConverterUtil;


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{

   const std::string *descs;
   std::string *vals;
   Integer size;
   
   out.Put("\n============================== test A1Mjd()");
   A1Mjd a1mjd = A1Mjd(21545);
   size = a1mjd.GetNumData();
   descs = a1mjd.GetDataDescriptions();
   vals = a1mjd.ToValueStrings();
   for (int i=0; i<size; i++)
      out.Put(descs[i], " = ", vals[i]);

   out.Put("\n============================== test A1Date");
   A1Date a1date = A1Date(2003, 10, 2, 10, 30, 20);
   size = a1date.GetNumData();
   descs = a1date.GetDataDescriptions();
   vals = a1date.ToValueStrings();
   for (int i=0; i<size; i++)
      out.Put(descs[i], " = ", vals[i]);

   out.Put("\n============================== test UtcDate()");
   UtcDate utcdate = UtcDate(2003, 10, 2, 10, 30, 20);
   size = utcdate.GetNumData();
   descs = utcdate.GetDataDescriptions();
   vals = utcdate.ToValueStrings();
   for (int i=0; i<size; i++)
      out.Put(descs[i], " = ", vals[i]);
   
   out.Put("\n============================== test ElapsedTime()");
   ElapsedTime elapsedTime = ElapsedTime(100.0);
   size = elapsedTime.GetNumData();
   descs = elapsedTime.GetDataDescriptions();
   vals = elapsedTime.ToValueStrings();
   for (int i=0; i<size; i++)
      out.Put(descs[i], " = ", vals[i]);

   out.Put("\n============================== test A1Mjd.ToA1Date()");
   try
   {
      A1Date a1 = a1mjd.ToA1Date();
      size = a1.GetNumData();
      descs = a1.GetDataDescriptions();
      vals = a1.ToValueStrings();
      for (int i=0; i<size; i++)
         out.Put(descs[i], " = ", vals[i]);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }

   out.Put("\n============================== test A1Mjd.ToUtcDate()");
   try
   {
      UtcDate u1 = a1mjd.ToUtcDate();
      size = u1.GetNumData();
      descs = u1.GetDataDescriptions();
      vals = u1.ToValueStrings();
      for (int i=0; i<size; i++)
         out.Put(descs[i], " = ", vals[i]);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   FileManager *fileManager = FileManager::Instance();
   std::string lsFileName = fileManager->GetFullPathname("LEAP_SECS_FILE");
   MessageInterface::ShowMessage
      ("===> Setting leap seconds file to %s\n", lsFileName.c_str());
   
   std::string eopFileName = fileManager->GetFullPathname("EOP_FILE");
   MessageInterface::ShowMessage
      ("===> Setting eop file to %s\n", eopFileName.c_str());
   
   //---------------------------------------------------------------------------
   out.Put("\n============================== Test Leap Seconds File Reader");
   try
   {
      LeapSecsFileReader *tcfr = new LeapSecsFileReader(lsFileName);
      tcfr->Initialize();
      
      UtcMjd utcmMjd = a1mjd.ToUtcMjd() + 30000;
      // greater then last table position
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 29000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 28000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 27000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 26000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 25000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 24000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 23000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 22000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 21000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 20000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 19000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 18000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 17000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      utcmMjd = a1mjd.ToUtcMjd() + 16000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      // less than first position in table
      utcmMjd = a1mjd.ToUtcMjd() + 15000;
      out.Put(utcmMjd, " => number of leap seconds = ",
              tcfr->NumberOfLeapSecondsFrom(utcmMjd));
      
      delete tcfr;
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }

   out.Put("");
   //---------------------------------------------------------------------------
   out.Put("\n============================== test TimeSystemConverter()");
   try
   {
      LeapSecsFileReader *lsfr = new LeapSecsFileReader(lsFileName);
      lsfr->Initialize();
      SetLeapSecsFileReader(lsfr);
      
      //@todo GmatEop::FINALS causes TableTemplate error : index out-of-bounds.
      //EopFile *ef = new EopFile(eopFileName, GmatEop::FINALS);
      EopFile *ef = new EopFile(eopFileName, GmatEop::EOP_C04);
      ef->Initialize();
      SetEopFile(ef);
      
      Real taiMjd = 53180.5;
      cout.precision(15);
      Real a1Mjd = ConvertFromTaiMjd(A1MJD, taiMjd);
      out.Put(taiMjd, " => to A1Mjd = ", a1Mjd, " => to taimjd = ",
              ConvertToTaiMjd(A1MJD, a1Mjd));
      
      Real ttMjd = ConvertFromTaiMjd(TTMJD, taiMjd);
      out.Put(taiMjd, " => to TtMjd = ", ttMjd, " => to taimjd = ",
              ConvertToTaiMjd(TTMJD, ttMjd));
      
      Real utcMjd = ConvertFromTaiMjd(UTCMJD, taiMjd);
      out.Put(taiMjd, " => to UtcMjd = ", utcMjd, " => to taimjd = ",
              ConvertToTaiMjd(UTCMJD, utcMjd));
      
      Real ut1Mjd = ConvertFromTaiMjd(UT1MJD, taiMjd);
      out.Put(taiMjd, " => to Ut1Mjd = ", ut1Mjd, " => to taimjd = ",
              ConvertToTaiMjd(UT1MJD, ut1Mjd));
      
      Real tcbMjd = ConvertFromTaiMjd(TCBMJD, taiMjd);
      out.Put(taiMjd, " => to TcbMjd = ", tcbMjd );
      
      Real tdbMjd = ConvertFromTaiMjd(TDBMJD, taiMjd);
      out.Put(taiMjd, " => to TdbMjd = ", tdbMjd );
      
      out.Put("\n=============== Test Convert Method" );
      Real ttMjd2 = Convert(a1Mjd, A1MJD, TTMJD, GmatTimeUtil::JD_NOV_17_1858);
      out.Put("A1Mjd: ", a1Mjd, " ==> to TtMjd: ", ttMjd2 );
      out.Put("A1Mjd: ", a1Mjd, " ==> to UtcMjd: ",
              Convert(a1Mjd, A1MJD, UTCMJD, GmatTimeUtil::JD_NOV_17_1858));
      
      out.Put("\n=============== Test Convert Method with diff offset" );
      Real a1Mjd2 = a1Mjd + 500000;
      Real utcMjd2 = Convert(a1Mjd2, A1MJD, UTCMJD, GmatTimeUtil::JD_JAN_5_1941);
      out.Put("A1Mjd: ", a1Mjd2, " ==> to UtcMjd: ", utcMjd2 );
      
      out.Put("UtcMjd: ", utcMjd2, " ==> to TtMjd: ",
              Convert(utcMjd2, UTCMJD, TTMJD, GmatTimeUtil::JD_NOV_17_1858));
      
      delete lsfr;
      delete ef;
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   //---------------------------------------------------------------------------
   try
   {
      out.Put("\n=============== Test Convert(fromTypeStr, fromMjd, &fromStr, toTypeStr, toMjd, toStr)");
      LeapSecsFileReader *leapSecsFile = new LeapSecsFileReader(lsFileName);
      leapSecsFile->Initialize();
      TimeConverterUtil::SetLeapSecsFileReader(leapSecsFile);
      
      Real epoch = 25131.176088;
      Real toMjd;
      std::string epochFormat = "UTCGregorian";
      std::string epochStr;
      Convert("A1ModJulian", epoch, "", "UTCGregorian", toMjd, epochStr);
      out.Put(epochStr);
      
      epoch = 25131.171227249783;
      Convert("A1ModJulian", epoch, "", "UTCGregorian", toMjd, epochStr);
      out.Put(epochStr);
      
      delete leapSecsFile;
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   out.Put("");
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string startupFile = "gmat_startup_file.txt";
   FileManager *fm = FileManager::Instance();
   fm->ReadStartupFile(startupFile);
   
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "../../TestTime/";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestTimeOut.txt";
   TestOutput out(outFile);
   out.Put(GmatTimeUtil::GetCurrentTime());
   MessageInterface::ShowMessage("%s\n", GmatTimeUtil::GetCurrentTime().c_str());
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of time classes!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
