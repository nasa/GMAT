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
// Author: Wendy Shoan
// Created: 2005/02/24
//
/**
 * Test driver for testing conversions of times
 */
//------------------------------------------------------------------------------
#include <map>
#include <iostream>
#include <string>
#include <list>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystemFactory.hpp"
#include "AxisSystem.hpp"
#include "SolarSystem.hpp"
#include "SpacePoint.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "Moon.hpp"
#include "Planet.hpp"
#include "A1Mjd.hpp"
#include "SolarSystemException.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "Linear.hpp"
#include "MessageWindow.hpp"
#include "GmatBaseException.hpp"
#include "Spacecraft.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateSystemException.hpp"
#include "CoordinateConverter.hpp"
#include "MJ2000EqAxes.hpp"
#include "MJ2000EcAxes.hpp"
#include "BodyFixedAxes.hpp"
#include "EquatorAxes.hpp"
#include "SlpFile.hpp"
#include "DeFile.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"
#include "LeapSecsFileReader.hpp"


using namespace std;

using namespace GmatTimeUtil;

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string eopFileName    = "/GMAT/dev/datafiles/EOPFiles/eopc04.62-now";
   std::string nutFileName    = "/GMAT/dev/datafiles/ITRF/NUTATION.DAT";
   std::string planFileName   = "/GMAT/dev/datafiles/ITRF/NUT85.DAT";
   std::string SLPFileName    = "/GMAT/dev/datafiles/mac/DBS_mn2000.dat";
   std::string DEFileName     = "/GMAT/dev/datafiles/DEascii/macp1941.405";
   std::string LeapFileName   = "/GMAT/dev/datafiles/tai-utcFiles/tai-utc.dat";
   //std::string eopFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/eop/eopc04.62-now";
   //std::string nutFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf/NUTATION.DAT";
   //std::string planFileName   = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf//NUT85.DAT";
   //std::string SLPFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/slp/DBS_mn2000.dat";
   //std::string DEFileName     = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/de/macp1941.405";
   //std::string LeapFileName   = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/time/tai-utc.dat";
   
   cout << "=-=-=-=-=-=-= TEST time conversions ....." << endl;
   
   cout.setf(ios::fixed);
   cout.precision(20);

   try
   {
      LeapSecsFileReader* ls     = new LeapSecsFileReader(LeapFileName);
      ls->Initialize();
      EopFile *eop               = new EopFile(eopFileName);
      eop->Initialize();
      ItrfCoefficientsFile* itrf = new ItrfCoefficientsFile(nutFileName, planFileName);
      itrf->Initialize();
       TimeConverterUtil::SetLeapSecsFileReader(ls);
      TimeConverterUtil::SetEopFile(eop);
   }
   catch (BaseException &bbee)
   {
      cout << "ERROR !!!!! " << bbee.GetMessage() << endl;
   }

   A1Mjd atTime1;
   

   cout << "**** testing time conversions, starting with a UTC calendar time of:" << endl;
   cout << "     6 Apr 1991 07:51:28.39  UTC" << endl;
   Real origMJD = ModifiedJulianDate(1991,4,6,7,51,28.39);
   Real origJD  = origMJD + JD_JAN_5_1941;
   //cout << "Original MJD (UTC) = " << origMJD << endl;
   cout << "Original JD (UTC) = " << origJD << endl;
   Real atTime = atTime1.UtcMjdToA1Mjd(origMJD);
   A1Mjd testTime(atTime);
   //cout << "The test time is " << testTime.Get() << endl;
   //cout << "A1 (MJD) is " << testTime.Get() << endl;
   Real origA1JD = testTime.Get()  + JD_JAN_5_1941;
   cout << "Original A1 (JD) is " << origA1JD << endl;
   
   Real mjdUTC = TimeConverterUtil::Convert(testTime.Get(),
                 "A1Mjd", "UtcMjd", JD_JAN_5_1941);
   // convert to MJD referenced from time used in EOP file
   mjdUTC = mjdUTC + JD_JAN_5_1941 - JD_NOV_17_1858;
   Real jdUTC = mjdUTC + JD_NOV_17_1858;
   
   cout << "UTC (JD) is " << jdUTC << endl;
   
   Real mjdUT1 = TimeConverterUtil::Convert(testTime.Get(),
                 "A1Mjd", "Ut1Mjd", JD_JAN_5_1941);
   Real jdUT1    = mjdUT1 + JD_JAN_5_1941; // right?

   cout << "UT1 (JD) is " << jdUT1<< endl;
   
   
   Real mjdTT = TimeConverterUtil::Convert(testTime.Get(),
                "A1Mjd", "TtMjd", JD_JAN_5_1941);      
   Real jdTT    = mjdTT + JD_JAN_5_1941; 

   cout << "TT (JD) is " << jdTT << endl;

   Real mjdTDB = TimeConverterUtil::Convert(testTime.Get(),
                 "A1Mjd", "TdbMjd", JD_JAN_5_1941);      
   Real jdTDB    = mjdTDB + JD_JAN_5_1941; 
   
   cout << "TDB (JD) is " << jdTDB << endl;
   
   Real mjdTCB = TimeConverterUtil::Convert(testTime.Get(),
                 "A1Mjd", "TcbMjd", JD_JAN_5_1941);      
   Real jdTCB    = mjdTCB + JD_JAN_5_1941; 
   
   cout << "TCB (JD) is " << jdTCB << endl;
   
   
   cout << "=-=-=-=-=-=-= END TEST time conversions ....." << endl;

}
