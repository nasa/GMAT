//------------------------------------------------------------------------------
//                                  TestLibration
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
// Created: 2005/04/06
//
/**
 * Test driver for the Barycenter (and CalculatedPoint) class(es)
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
#include "SlpFile.hpp"
#include "DeFile.hpp"
#include "TimeSystemConverter.hpp"
#include "LeapSecsFileReader.hpp"
#include "CalculatedPointFactory.hpp"
#include "CalculatedPoint.hpp"
#include "Barycenter.hpp"
#include "LibrationPoint.hpp"


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
   std::string eopFileName    = 
      "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/eop/eopc04.62-now";
   std::string nutFileName    = 
      "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf/NUTATION.DAT";
   std::string planFileName   = 
      "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf//NUT85.DAT";
   std::string SLPFileName    = 
      "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem" + 
      "/slp/DBS_mn2000.dat";
   std::string DEFileName     = 
      "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem" + 
      "/de/macp1941.405";
   std::string LeapFileName   = 
      "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/time/tai-utc.dat";
    */
   
   cout << "=-=-=-=-=-=-= TEST CalculatedPoint/LibrationPoint ....." << endl;
   cout << " ------ number of objects instantiated = " << 
      GmatBase::GetInstanceCount() << endl;
   
   cout.setf(ios::fixed);
   cout.precision(30);
   
   static const std::string LIBNAMES[5] = 
   {
      "L1",
      "L2",
      "L3",
      "L4",
      "L5"
   };

   
   cout << "\n==> First, test the CalculatedPointFactory <==" << endl;
   CalculatedPointFactory *cpf = new CalculatedPointFactory();
   cout << "CalculatedPointFactory created .........." << endl;
   cout << " ------ number of objects instantiated = " << 
      GmatBase::GetInstanceCount() << endl;
   
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
   try
   {
      cout << "\n==> Create the solar system <==" << endl;
      
      ss              = new SolarSystem("TheSS");
      cout << "solar system name = " << ss->GetName() << endl;
      cout << " ------ number of objects instantiated = " << 
         GmatBase::GetInstanceCount() << endl;

      earth = (Planet*) ss->GetBody(SolarSystem::EARTH_NAME);
      cout << "earth name = " << earth->GetName() << endl;
      cout << "earth's type name is " << earth->GetTypeName() << endl;
      
      sol = (Star*) ss->GetBody(SolarSystem::SUN_NAME);
      cout << "sol name = " << sol->GetName() << endl;
      cout << "sol's type name is " << sol->GetTypeName() << endl;
      cout << " ------ number of objects instantiated = " << 
         GmatBase::GetInstanceCount() << endl;

      luna = (Moon*) ss->GetBody(SolarSystem::MOON_NAME);
      cout << "luna name = " << luna->GetName() << endl;
      cout << "luna's type name is " << luna->GetTypeName() << endl;

      mars = (Planet*) ss->GetBody(SolarSystem::MARS_NAME);
      cout << "mars name = " << mars->GetName() << endl;
      cout << "mars's type name is " << mars->GetTypeName() << endl;

      jupiter = (Planet*) ss->GetBody(SolarSystem::JUPITER_NAME);
      cout << "jupiter name = " << jupiter->GetName() << endl;
      cout << "jupiter's type name is " << jupiter->GetTypeName() << endl;

      SlpFile* anSLP;
      DeFile*  aDE;
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
      ss->SetSource(Gmat::DE_405);
      ss->SetSourceFile(aDE);

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
   bool OK = false;
   cout << "Now creating LibrationPoint L1" << endl;
   LibrationPoint *L1 = (LibrationPoint*) 
      cpf->CreateCalculatedPoint("LibrationPoint", "L1");
   
   cout << "Now setting bodies for L1 .........." << endl;
   const std::string bNames = "BodyNames";
   L1->SetStringParameter(bNames,SolarSystem::EARTH_NAME,0);
   L1->SetStringParameter(bNames,SolarSystem::SUN_NAME,1);
   L1->SetStringParameter(bNames,SolarSystem::MOON_NAME,2);
   cout << "Body names were set  ......" << endl;
   L1->SetStringParameter("Primary", "Earth");
   cout << "Primary name was set ......" << endl;
   L1->SetStringParameter("Secondary", "Luna");
   cout << "Secondary name was set ......" << endl;
   cout << "Now ready to set ref objects ......." << endl;
   L1->SetJ2000BodyName(j2000BN);
   L1->SetJ2000Body(j2000B);
   
   OK = L1->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");
   if (!OK) cout << "ERROR setting LibrationPoint with body Earth" << endl;
   OK = L1->SetRefObject(sol, Gmat::SPACE_POINT, "Sun");
   if (!OK) cout << "ERROR setting LibrationPoint with body Sun" << endl;
   OK = L1->SetRefObject(luna, Gmat::SPACE_POINT, "Luna");
   if (!OK) cout << "ERROR setting LibrationPoint with body Luna" << endl;
   
   Integer num = 0;
   StringArray names;
   cout << "Check out LibrationPoint L1 data ---------------------" << endl;
   num = L1->GetIntegerParameter("NumberOfBodies");
   cout << "L1 number of bodies = " << num << endl;
   cout << "L1 body names are: " << endl;
   names = L1->GetStringArrayParameter(L1->GetParameterID("BodyNames"));
   for (unsigned int i = 0; i < names.size(); i++)
      cout << " .... " << names.at(i) << endl;
   cout << "L1 bodies (their names) are: " << endl;
   for (Integer i = 0; i < num; i++)
   {
      cout << " .... " << 
      (L1->GetRefObject(Gmat::SPACE_POINT, names.at(i), i))->GetName() << endl;
   }
   cout << "L1's primary body is: " << L1->GetStringParameter("Primary") 
      << endl;
   cout << "L1's secondary body is: " << L1->GetStringParameter("Secondary") 
      << endl;
   
   A1Mjd atTime1;

   Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(2000,01,01,12,00,0.0));
   A1Mjd testTime(atTime);
   cout << "The test time is " << testTime.Get() << endl;
   cout << "*** NOW try to do L1 calculations ........." << endl;
   cout.setf(ios::scientific);
   cout << "Earth's position and velocity at test time are : " << endl << 
      earth->GetMJ2000State(atTime) << endl;
   cout << "Luna's position and velocity at test time are : " << endl << 
      luna->GetMJ2000State(atTime) << endl;
   
   cout << "R and V from Earth to Moon is: " << endl << (luna->GetMJ2000State(atTime)) -
      (earth->GetMJ2000State(atTime)) << endl;
   
   for (Integer i = 0; i < 5; i++)
   {
      L1->SetStringParameter("Point", LIBNAMES[i]);
      cout << "The libration point was set to : " << 
         L1->GetStringParameter("Point") << endl;
      try
      {
         cout << "Its position and velocity at test time = " << endl << 
            L1->GetMJ2000State(atTime) << endl;
      }
      catch (BaseException &be)
      {
         cout << "ERROR - message is : " << be.GetMessage() << endl;
      }
   }
   
   
   cout << "Now delete SolarSystem ............." << endl;
   delete ss;
   cout << " ------ number of objects instantiated = " << 
      GmatBase::GetInstanceCount() 
      << endl;
   cout << "Now delete LibrationPoint L1 ............." << endl;
   delete L1;
   cout << "........ LibrationPoint L1 deleted ............." << endl;
   
   cout << "Now delete everything else ............." << endl;
   delete cpf;
   cout << "........ cpf deleted ............." << endl;
   delete ls;
   cout << "........ ls deleted ............." << endl;
   delete eop;
   cout << "........ eop deleted ............." << endl;
   delete itrf;
   cout << "........ itrf deleted ............." << endl;
   cout << " ------ number of objects instantiated = " << 
      GmatBase::GetInstanceCount() 
      << endl;
   
   cout << "=-=-=-=-=-=-= END TEST CalculatedPoint/Barycenter ....." << endl;

}
