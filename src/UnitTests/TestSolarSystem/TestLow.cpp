//------------------------------------------------------------------------------
//                                  TestLowFidelity
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
// Created: 2005/06/28
//
/**
 * Test driver for the Low Fidelity ephemeris modeling for Clestial bodies
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
#include "SolarSystemFactory.hpp"
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
#include "SlpFile.hpp"
#include "DeFile.hpp"
#include "TimeSystemConverter.hpp"
#include "LeapSecsFileReader.hpp"
#include "CalculatedPointFactory.hpp"
#include "CalculatedPoint.hpp"
#include "Barycenter.hpp"
#include "LibrationPoint.hpp"
#include "CoordUtil.hpp"
#include "TimeTypes.hpp"


using namespace std;

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
   
/*
   std::string eopFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/eop/eopc04.62-now";
   std::string nutFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf/NUTATION.DAT";
   std::string planFileName   = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf//NUT85.DAT";
   std::string SLPFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/slp/DBS_mn2000.dat";
   std::string DEFileName     = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/de/macp1941.405";
   std::string LeapFileName   = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/time/tai-utc.dat";
 */
   
   cout << "=-=-=-=-=-=-= TEST Low Fidelity Ephemeris Modeling ....." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout.setf(ios::fixed);
   cout.precision(30);

   std::ofstream foutLuna;
   foutLuna.open("./TestLowLuna.out");
   foutLuna.setf(ios::fixed);
   foutLuna.precision(30);

   std::ofstream foutMars;
   foutMars.open("./TestLowMars.out");
   foutMars.setf(ios::fixed);
   foutMars.precision(30);

   
   cout << "\n==> First, test the SolarSystemFactory <==" << endl;
   SolarSystemFactory *ssf = new SolarSystemFactory();
   cout << "SolarSystemFactory created .........." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   LeapSecsFileReader* ls;
   EopFile *eop;
   ItrfCoefficientsFile* itrf;
   try
   {
      ls = new LeapSecsFileReader(LeapFileName);
      ls->Initialize();
      eop = new EopFile(eopFileName);
      eop->Initialize();
      itrf = new ItrfCoefficientsFile(nutFileName, planFileName);
      itrf->Initialize();
      TimeConverterUtil::SetLeapSecsFileReader(ls);
      TimeConverterUtil::SetEopFile(eop);
   }
   catch (BaseException &bbee)
   {
      cout << "ERROR !!!!! " << bbee.GetMessage() << endl;
   }
   
   SolarSystem*   ss;
   Star*          sol;
   Planet*        earth;
   Planet*        mars;
   Planet*        jupiter;
   Moon*          luna;
   std::string    j2000BN = "";
   SpacePoint*    j2000B  = NULL;
   SlpFile* anSLP;
   DeFile*  aDE;
   try
   {
      cout << "\n==> Create the solar system <==" << endl;
      
      //ss              = new SolarSystem("TheSS");
      ss              = ssf->CreateSolarSystem("Default", "TheSS");
      cout << "solar system name = " << ss->GetName() << endl;
      cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;

      earth = (Planet*) ss->GetBody(SolarSystem::EARTH_NAME);
      cout << "earth name = " << earth->GetName() << endl;
      cout << "earth's type name is " << earth->GetTypeName() << endl;
      
      sol = (Star*) ss->GetBody(SolarSystem::SUN_NAME);
      cout << "sol name = " << sol->GetName() << endl;
      cout << "sol's type name is " << sol->GetTypeName() << endl;
      cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;

      luna = (Moon*) ss->GetBody(SolarSystem::MOON_NAME);
      cout << "luna name = " << luna->GetName() << endl;
      cout << "luna's type name is " << luna->GetTypeName() << endl;

      mars = (Planet*) ss->GetBody(SolarSystem::MARS_NAME);
      cout << "mars name = " << mars->GetName() << endl;
      cout << "mars's type name is " << mars->GetTypeName() << endl;

      jupiter = (Planet*) ss->GetBody(SolarSystem::JUPITER_NAME);
      cout << "jupiter name = " << jupiter->GetName() << endl;
      cout << "jupiter's type name is " << jupiter->GetTypeName() << endl;

      try
      {
         anSLP          = new SlpFile(SLPFileName);
         aDE            = new DeFile(Gmat::DE405,DEFileName);
         cout << "the SLP file is : " << anSLP->GetName() << endl;
         cout << "the DE file is : " << aDE->GetName() << endl;
      }
      catch (BaseException &be1)
      {
         cout << "ERROR with ephem file -> " << be1.GetMessage() << endl;
      }

      //ss->SetSource(Gmat::SLP);
      //ss->SetSourceFile(anSLP);
      //ss->SetSource(Gmat::DE_405);
      //ss->SetSourceFile(aDE);
      


      // set the j2000Body
      j2000BN = "Earth";
      j2000B  = earth;

      sol->SetJ2000BodyName(j2000BN);
      sol->SetJ2000Body(j2000B);
      earth->SetJ2000BodyName(j2000BN);
      earth->SetJ2000Body(j2000B);
      luna->SetJ2000BodyName(j2000BN);
      luna->SetJ2000Body(j2000B);
      mars->SetJ2000BodyName(j2000BN);
      mars->SetJ2000Body(j2000B);
      jupiter->SetJ2000BodyName(j2000BN);
      jupiter->SetJ2000Body(j2000B);
      
   }
   catch (GmatBaseException &ex)
   {
      cout << "Some kind of error ..........." << endl;
   }
   //bool OK = false;
   
   A1Mjd atTime1;
   
   // Epoch  Jan 01 2000 12:00:00.000
   Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(2000, 1, 1, 12, 0, 0.0));
   A1Mjd testTime(atTime);
   Rvector6 kepler(384400.0, 0.05490, 5.145396, 0.0, 0.0, 0.0); 
   Real atTimeMars = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(2000, 1, 1, 12, 0, 0.0)); 
   A1Mjd testTimeMars(atTimeMars);
   Real ma;
   Rvector6 cartMars(168187723.443443, 120521531.810573, -1608973.63010399,
                     -15.082688017249, 21.7337479741311, 0.82600091094756);
   Rvector6 keplerMars = CartesianToKeplerian(cartMars,
                        (mars->GetGravitationalConstant() +
                        sol->GetGravitationalConstant()), &ma);
   //Rvector6 keplerMars(227939186.0, 0.0934, 1.8497, 336.060, 49.558, 10.0); 
   Real atTimeEarth = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(2000, 1, 1, 12, 0, 0.0));
   A1Mjd testTimeEarth(atTimeEarth);
   Rvector6 keplerEarth(149598023.0, 0.016708717, 0.0, 102.937, 0.0, 0.0); 
   cout << "The epoch time (Luna) is " << testTime.Get() << endl;
   cout << "The epoch time (Mars) is " << testTimeMars.Get() << endl;
   cout << "The epoch time (Earth) is " << testTimeEarth.Get() << endl;
   cout.setf(ios::scientific);

   cout << "Test the central body names .........." << endl;
   cout << "Mars' central body is " << mars->GetCentralBody() << endl;
   cout << "Earth's central body is " << earth->GetCentralBody() << endl;
   cout << "Luna's central body is " << luna->GetCentralBody() << endl;
   
   cout << "NOW set the central body pointers for Earth, Mars and Luna ...." << endl;
   earth->SetRefObject(sol, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   mars->SetRefObject(sol, Gmat::CELESTIAL_BODY, SolarSystem::SUN_NAME);
   luna->SetRefObject(earth, Gmat::CELESTIAL_BODY, SolarSystem::EARTH_NAME);
   sol->SetRefObject(earth, Gmat::CELESTIAL_BODY, SolarSystem::EARTH_NAME);
   
   cout << "NOW set the epoch and initial keplerian elements for all" << endl;
   cout << "The epoch is: " << testTime.Get() << endl;
   cout << "The Luna keplerian elements are : \n" << kepler << endl;
   cout << "The Mars keplerian elements are : \n" << keplerMars << endl;
   cout << "The Earth keplerian elements are : \n" << keplerEarth << endl;
   
   ss->SetSource(Gmat::ANALYTIC);
   ss->SetAnalyticMethod(Gmat::LOW_FIDELITY);
   
   luna->SetLowFidelityEpoch(testTime);
   luna->SetLowFidelityElements(kepler);
   
   mars->SetLowFidelityEpoch(testTimeMars);
   mars->SetLowFidelityElements(keplerMars);
   earth->SetLowFidelityEpoch(testTimeEarth);
   earth->SetLowFidelityElements(keplerEarth);
   
   Real newAtTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(2005, 12, 28, 12, 0, 0.0));
   A1Mjd newTime(newAtTime);
   cout << "NOW try to propagate  ......... : " << endl;
   Rvector6 newState, newStateMars;
   foutLuna << " *********** Luna data ***************" << endl << endl;
   for (int i = 0; i <= 2350000; i += 10000)
   {
      newTime = testTime + i / GmatTimeUtil::SECS_PER_DAY;
      try
      {
         newState     = luna->GetState(newTime);
      }
      catch (BaseException &be)
      {
         cout << "ERROR getting new state: " << be.GetMessage() << endl;
      }
      foutLuna << i << "  " << newState[0] << " " << newState[1] << " " << newState[2]
         << "  " << newState[3] << " " << newState[4] << " " << newState[5] << std::endl;
   }
   foutMars << endl << endl << " *********** Mars data ***************" << endl << endl;
   for (int i = 0; i <= 686; i += 1)
   {
      newTime = testTime + i;
      try
      {
         newStateMars = mars->GetState(newTime);
      }
      catch (BaseException &be)
      {
         cout << "ERROR getting new state: " << be.GetMessage() << endl;
      }
      foutMars << (i * GmatTimeUtil::SECS_PER_DAY) << "  " << newStateMars[0] << " " << newStateMars[1] << " " << newStateMars[2]
         << "  " << newStateMars[3] << " " << newStateMars[4] << " " << newStateMars[5] << std::endl;
   }

   cout << "Now delete SolarSystem ............." << endl;
   delete ss;

   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "Now delete everything else ............." << endl;
   //if (ssf) delete ssf;
   //cout << "........ ssf deleted ............." << endl;
   if (ls) delete ls;
   cout << "........ ls deleted ............." << endl;
   if (eop) delete eop;
   cout << "........ eop deleted ............." << endl;
   if (itrf) delete itrf;
   cout << "........ itrf deleted ............." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "=-=-=-=-=-=-= END TEST Low Fidelity Ephemeris Modeling ....." << endl;

}
