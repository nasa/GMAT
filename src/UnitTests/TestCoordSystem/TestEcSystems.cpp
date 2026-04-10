//------------------------------------------------------------------------------
//                                  TestEcSystems
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
// Created: 2005/06/07
//
/**
 * Test driver for Ecliptic coordinate system stuff
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
#include "TODEqAxes.hpp"
#include "MODEqAxes.hpp"
#include "TODEcAxes.hpp"
#include "MODEcAxes.hpp"
#include "TOEEcAxes.hpp"
#include "MOEEcAxes.hpp"
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
   std::string eopFileName    = "/GMAT/dev/datafiles/EOPFiles/eopc04.62-now";
   std::string nutFileName    = "/GMAT/dev/datafiles/ITRF/NUTATION.DAT";
   std::string planFileName   = "/GMAT/dev/datafiles/ITRF/NUT85.DAT";
   std::string SLPFileName    = "/GMAT/dev/datafiles/mac/DBS_mn2000.dat";
   std::string DEFileName     = "/GMAT/dev/datafiles/DEascii/macp1941.405";
   std::string LeapFileName   = "/GMAT/dev/datafiles/tai-utcFiles/tai-utc.dat";
   //std::string eopFileName    = 
   //"/Users/wendyshoan/WORKONLY/dev/build3/bin/files/eop/eopc04.62-now";
   //std::string nutFileName    = 
   //"/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf/NUTATION.DAT";
   //std::string planFileName   = 
   //"/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf//NUT85.DAT";
   //std::string SLPFileName    = 
   //"/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/slp/DBS_mn2000.dat";
   //std::string DEFileName     = 
   //"/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/de/macp1941.405";
   //std::string LeapFileName   = 
   //"/Users/wendyshoan/WORKONLY/dev/build3/bin/files/time/tai-utc.dat";
   
   cout << "=-=-=-=-=-=-= TEST Ecliptic Systems ....." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout.setf(ios::fixed);
   cout.precision(16);

   
   AxisSystemFactory *asf = new AxisSystemFactory();
   cout << "AxisSystemFactory created .........." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   MJ2000EqAxes* mj;
   TODEcAxes* todec;
   MODEcAxes* modec;
   TOEEcAxes* toeec;
   MOEEcAxes* moeec;
   mj    = (MJ2000EqAxes*)  asf->CreateAxisSystem("MJ2000Eq", "MJ1");
   todec = (TODEcAxes*) asf->CreateAxisSystem("TODEc", "TOD1");
   modec = (MODEcAxes*) asf->CreateAxisSystem("MODEc", "MOD1");
   toeec = (TOEEcAxes*) asf->CreateAxisSystem("TOEEc", "TOE1");
   moeec = (MOEEcAxes*) asf->CreateAxisSystem("MOEEc", "MOE1");

   LeapSecsFileReader    *ls;
   EopFile               *eop;
   ItrfCoefficientsFile *itrf;
   try
   {
      ls     = new LeapSecsFileReader(LeapFileName);
      ls->Initialize();
      eop               = new EopFile(eopFileName);
      eop->Initialize();
      itrf = new ItrfCoefficientsFile(nutFileName, planFileName);
      itrf->Initialize();
      todec->SetEopFile(eop);
      todec->SetCoefficientsFile(itrf);
      modec->SetEopFile(eop);
      modec->SetCoefficientsFile(itrf);
      toeec->SetEopFile(eop);
      toeec->SetCoefficientsFile(itrf);
      moeec->SetEopFile(eop);
      moeec->SetCoefficientsFile(itrf);
      TimeConverterUtil::SetLeapSecsFileReader(ls);
      TimeConverterUtil::SetEopFile(eop);
   }
   catch (BaseException &bbee)
   {
      cout << "ERROR !!!!! " << bbee.GetMessage() << endl;
   }

   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   if (mj)
   {
      cout << "MJ2000EqAxes object exists with name " << mj->GetName() << 
      " and type " << mj->GetTypeName() << endl;
   }
   if (todec)
   {
      cout << "TODEc object exists with name " << todec->GetName() << 
      " and type " << todec->GetTypeName() << endl;
   }
   if (modec)
   {
      cout << "MODEc object exists with name " << modec->GetName() << 
      " and type " << modec->GetTypeName() << endl;
   }
   if (toeec)
   {
      cout << "TOEEc object exists with name " << toeec->GetName() << 
      " and type " << toeec->GetTypeName() << endl;
   }
   if (moeec)
   {
      cout << "MOEEc object exists with name " << moeec->GetName() << 
      " and type " << moeec->GetTypeName() << endl;
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

   A1Mjd atTime1;
   A1Mjd theEpoch;
   
   // 6 Apr 1991 07:51:28.390 
   Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(1991,4,6,7,51,28.390));
   A1Mjd testTime(atTime);
   Real epochTime = theEpoch.UtcMjdToA1Mjd(ModifiedJulianDate(2000,1,1,0,0,0.0));
   A1Mjd epochA1(epochTime);
   cout << "The test time (as an A1MJD) is " << testTime.Get() << endl;
   cout << "The epoch time (as an A1MJD) is " << epochA1.Get() << endl;
   
   bool isOK = false;
   cout << "\n==> Now creating CoordinateSystems <==" << endl;
   CoordinateSystem* mj2000 = new CoordinateSystem("", "CoordSystemMJ2000");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   mj2000->SetSolarSystem(ss);
   mj2000->SetStringParameter("Origin","Earth"); 
   mj2000->SetStringParameter("J2000Body",j2000BN); 
   isOK = mj2000->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = mj2000->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = mj2000->SetRefObject(mj, Gmat::AXIS_SYSTEM, mj->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   mj2000->Initialize();
   
   
   CoordinateSystem* todCS = new CoordinateSystem("", "CoordSystemTODEc");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   todCS->SetSolarSystem(ss);
   todCS->SetStringParameter("Origin",SolarSystem::EARTH_NAME); 
   todCS->SetStringParameter("J2000Body",j2000BN);
   isOK = todCS->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = todCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = todCS->SetRefObject(todec, Gmat::AXIS_SYSTEM, todec->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   todec->SetEpoch(testTime);  // shouldn't matter
   todCS->Initialize();
   
   
   CoordinateSystem* modCS = new CoordinateSystem("", "CoordSystemMODEc");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   modCS->SetSolarSystem(ss);
   modCS->SetStringParameter("Origin",SolarSystem::EARTH_NAME); 
   modCS->SetStringParameter("J2000Body",j2000BN);
   isOK = modCS->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = modCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = modCS->SetRefObject(modec, Gmat::AXIS_SYSTEM, modec->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   modec->SetRealParameter("Epoch",testTime.Get());
   modCS->Initialize();

   CoordinateSystem* toeCS = new CoordinateSystem("", "CoordSystemTOEEc");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   toeCS->SetSolarSystem(ss);
   toeCS->SetStringParameter("Origin",SolarSystem::EARTH_NAME); 
   toeCS->SetStringParameter("J2000Body",j2000BN);
   isOK = toeCS->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = toeCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = toeCS->SetRefObject(toeec, Gmat::AXIS_SYSTEM, toeec->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   toeec->SetEpoch(testTime); 
   toeCS->Initialize();
   
   
   CoordinateSystem* moeCS = new CoordinateSystem("", "CoordSystemMOEEc");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   moeCS->SetSolarSystem(ss);
   moeCS->SetStringParameter("Origin",SolarSystem::EARTH_NAME); 
   moeCS->SetStringParameter("J2000Body",j2000BN);
   isOK = moeCS->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = moeCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = moeCS->SetRefObject(moeec, Gmat::AXIS_SYSTEM, moeec->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   moeec->SetRealParameter("Epoch",testTime.Get());
   moeCS->Initialize();
   
   
   CoordinateConverter *cc = new CoordinateConverter();

   Rvector6 theState(5102.5096000000003000, 6123.0115200000009000, 6378.1363000000001000,
                     -4.7432196000000006, 0.7905365999999999, 5.5337561900000010);

   Rvector6 outState;
   
   cout << "The test Rvector6 is " << endl << theState << endl;
   
   cout << "About to try to convert!!!!!" << endl;
   
   try
   {
      modec->SetRealParameter("Epoch",epochA1.Get());
      cc->Convert(testTime,theState, mj2000, outState, todCS);
      cout << "The output state (to TODEc) is : " << endl << outState << endl;
      cc->Convert(testTime,theState, mj2000, outState, modCS);
      cout << "The output state (to MODEc) is : " << endl << outState << endl;
      cc->Convert(testTime,theState, mj2000, outState, toeCS);
      cout << "The output state (to TOEEc) is : " << endl << outState << endl;
      cc->Convert(testTime,theState, mj2000, outState, moeCS);
      cout << "The output state (to MOEEc) is : " << endl << outState << endl;
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
   cout << "Now delete other CoordinateSystems (and its AxisSystem) ............." << endl;
   delete todCS;
   delete modCS;
   delete toeCS;
   delete moeCS;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "Now delete everything else ............." << endl;
   if (ls)    delete ls;
   cout << "ls deleted ............." << endl;
   if (eop)   delete eop;
   cout << "eop deleted ............." << endl;
   if (itrf)  delete itrf;
   cout << "itrf deleted ............." << endl;
   //if (anSLP) delete anSLP;  // ******** fix later ********
   //cout << "anSLP deleted ............." << endl;
   if (aDE)  delete aDE;
   cout << "aDE deleted ............." << endl;
   delete asf;
   cout << "asf deleted ............." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "=-=-=-=-=-=-= END TEST Ecliptic systems ....." << endl;

}
