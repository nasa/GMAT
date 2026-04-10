//------------------------------------------------------------------------------
//                                  TestCoord
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
// Created: 2004/12/27
//
/**
 * Test driver for coordinate system stuff
 * Phase 1: Test AxisSystemFactory (however, no 'leaf' classes yet, so not 
 * doing much yet)
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
#include "SlpFile.hpp"
#include "DeFile.hpp"
#include "TimeSystemConverter.hpp"
#include "LeapSecsFileReader.hpp"


using namespace std;

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   cout << "=-=-=-=-=-=-= TEST coordinate system ....." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout.setf(ios::fixed);
   cout.precision(16);

   std::string eopFileName    = "/GMAT/dev/datafiles/EOPFiles/eopc04.62-now";
   std::string nutFileName    = "/GMAT/dev/datafiles/ITRF/NUTATION.DAT";
   std::string planFileName   = "/GMAT/dev/datafiles/ITRF/NUT85.DAT";
   std::string SLPFileName    = "/GMAT/dev/datafiles/mac/DBS_mn2000.dat";
   std::string DEFileName     = "/GMAT/dev/datafiles/DEascii/macp1941.405";
   std::string LeapFileName   = "/GMAT/dev/datafiles/tai-utcFiles/tai-utc.dat";
   
   cout << "\n==> First, test the AxisSystemFactory <==" << endl;
   AxisSystemFactory *asf = new AxisSystemFactory();
   cout << "AxisSystemFactory created .........." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   std::string
      AXES_TYPE[13] =
   {
      "MJ2000Eq",
      "MJ2000Ec",
      "TOEEq",
      "TOEEc",
      "MOEEq",
      "MOEEc",
      "TODEq",
      "TODEc",
      "MODEq",
      "MODEc",
      "ObjectReferenced",
      "Equator",
      "BodyFixed",
   };
   
   AxisSystem *as;
   MJ2000EqAxes* mj;
   MJ2000EqAxes* mj2;
   //MJ2000EcAxes* mj2;
   std::string tmpStr = "";
   //for (int i = 0; i < 13 ; i++)
   for (int i = 0; i < 2 ; i++) // only do the first one, for now
   {
      tmpStr = AXES_TYPE[i] + "1";
      as = asf->CreateAxisSystem(AXES_TYPE[i], tmpStr);
      if (as) 
      {
         cout << "AxisSystem of type " << AXES_TYPE[i] << 
                  " was created with name " << tmpStr << endl;
         if (i > 1)       delete as; 
         else if (i == 0) mj   = (MJ2000EqAxes*) as;
         //else             mj2  = (MJ2000EcAxes*) as;
      }
      else    cout << "NO " << AXES_TYPE[i] << " AxisSystem created." << endl;
   }
   
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   if (mj)
   {
      cout << "MJ2000EqAxes object exists with name " << mj->GetName() << 
      " and type " << mj->GetTypeName() << endl;
   }
   mj2 = (MJ2000EqAxes*)(asf->CreateAxisSystem("MJ2000Eq","MJ2000Eq2"));
   if (mj2)
   {
      cout << "MJ20002 object exists with name " << mj2->GetName() << 
      " and type " << mj2->GetTypeName() << endl;
   }

   try
   {
      LeapSecsFileReader* ls     = new LeapSecsFileReader(LeapFileName);
      ls->Initialize();
      EopFile *eop               = new EopFile(eopFileName);
      eop->Initialize();
      ItrfCoefficientsFile* itrf = new ItrfCoefficientsFile(nutFileName, planFileName);
      itrf->Initialize();
      //bf->SetEopFile(eop);
      //bf->SetCoefficientsFile(itrf);
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

      SlpFile* anSLP;
      DeFile*  aDE;
      try
      {
std::string SLPFileName = "/GMAT/dev/datafiles/mac/DBS_mn2000.dat";
std::string DEFileName  = "/GMAT/dev/datafiles/DEascii/macp1941.405";
         anSLP          = new SlpFile(SLPFileName);
         aDE            = new DeFile(Gmat::DE405,DEFileName);
         cout << "the SLP file is : " << anSLP->GetName() << endl;
         cout << "the DE file is : " << aDE->GetName() << endl;
      }
      catch (BaseException &be1)
      {
         cout << "ERROR with ephem file -> " << be1.GetMessage() << endl;
      }

      ss->SetSource(Gmat::SLP);
      ss->SetSourceFile(anSLP);
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

   
   bool isOK = false;
   cout << "\n==> Now creating CoordinateSystems <==" << endl;
   CoordinateSystem* mj2000 = new CoordinateSystem("", "CoordSystemMJ2000");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   mj2000->SetSolarSystem(ss);
   mj2000->SetStringParameter("OriginName","Earth"); 
   mj2000->SetStringParameter("J2000BodyName",j2000BN); 
   isOK = mj2000->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = mj2000->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = mj2000->SetRefObject(mj, Gmat::AXIS_SYSTEM, mj->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   mj2000->Initialize();
   
   
   CoordinateSystem* mj20002 = new CoordinateSystem("", "CoordSystemMJ2000eq");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   mj20002->SetSolarSystem(ss);
   mj20002->SetStringParameter("OriginName",SolarSystem::SUN_NAME); 
   mj20002->SetStringParameter("J2000BodyName",j2000BN);
   isOK = mj20002->SetRefObject(sol,Gmat::SPACE_POINT,SolarSystem::SUN_NAME);
   //isOK = mj20002->SetRefObject(mars,Gmat::SPACE_POINT,"Earth");
   isOK = mj20002->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = mj20002->SetRefObject(mj2, Gmat::AXIS_SYSTEM, mj2->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   mj20002->Initialize();
   
   try
   {
      cout << "Now checking CoordinateSystems' AxisSystems ..." << endl;
      GmatBase *mjObj = mj2000->GetRefObject(Gmat::AXIS_SYSTEM, mj->GetName());
      if (mjObj) 
      {
         cout << "mj2000 has a pointer to the axis system " << mjObj->GetName() << 
         " of type " << mjObj->GetTypeName() << endl;
      }
      else       cout << "ERROR -  no pointer to the axis system ????????" << endl;
      GmatBase *mjObj2 = mj20002->GetRefObject(Gmat::AXIS_SYSTEM, mj2->GetName());
      if (mjObj2) 
      {
         cout << "mj2000ec has a pointer to the axis system " << mjObj2->GetName() << 
         " of type " << mjObj2->GetTypeName() << endl;
      }
      else       cout << "ERROR -  no pointer to the axis system ????????" << endl;
   }
   catch (BaseException &be)
   {
      cout << "ERROR ------------- : " << be.GetMessage() << endl;
   }
   
   CoordinateConverter *cc = new CoordinateConverter();
   //cc->SetJ2000BodyName("Earth");
   //cc->SetJ2000Body(earth);

   //Rvector6 theState(15999.99999999998,0.0,0.0,
   //                  0.0, 3.8662018270519716, 3.8662018270519711);
   Rvector6 theState(18407337.2437560,146717552.364272,2436998.6080801622,
                     -29.85775713588113, 3.7988731566283533, -0.0883535323140749);
   Rvector6 outState;
   
   A1Mjd atTime1;
   Real ut1_utc = 0.456647;
   Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(2012,1,1,0,0,ut1_utc));
   A1Mjd testTime(atTime);
   cout << "The test time is " << testTime.Get() << endl;
   cout << "The test Rvector6 is " << theState << endl;
   
   cout << "About to try to convert!!!!!" << endl;
   
   try
   {
      cc->Convert(testTime,theState, mj2000, outState, mj20002);
      cout << "The output state is : " << outState << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   
   
   cout << "Now delete SolarSystem ............." << endl;
   delete ss;
   cout << "Now delete CoordinateConverter ............." << endl;
   delete cc;
   cout << "Now delete CoordinateSystem (and its AxisSystem) ............." << endl;
   delete mj2000;
   cout << "Now delete other CoordinateSystem (and its AxisSystem) ............." << endl;
   delete mj20002;
   //cout << "Now delete AxisSystem ............." << endl;
   //delete mj;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "Now delete everything else ............." << endl;
   delete asf;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "=-=-=-=-=-=-= END TEST coordinate system ....." << endl;

}
