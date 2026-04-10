//$Header: /cygdrive/p/dev/cvs/test/TestCoordSystem/TestTODEq.cpp,v 1.1 2005/07/01 14:07:36 wshoan Exp $
//------------------------------------------------------------------------------
//                                  TestTODEq
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
// Created: 2005/05/03
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
#include "BodyFixedAxes.hpp"
#include "TODEqAxes.hpp"
#include "MODEqAxes.hpp"
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
   
   cout << "=-=-=-=-=-=-= TEST MODEq and TODEq ....." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout.setf(ios::fixed);
   cout.precision(16);

   
   AxisSystemFactory *asf = new AxisSystemFactory();
   cout << "AxisSystemFactory created .........." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   AxisSystem*   as;
   MJ2000EqAxes* mj;
   TODEqAxes* todeq;
   MODEqAxes* modeq;
   BodyFixedAxes *bf;
   mj    = (MJ2000EqAxes*)  asf->CreateAxisSystem("MJ2000Eq", "MJ1");
   todeq = (TODEqAxes*) asf->CreateAxisSystem("TODEq", "TOE1");
   modeq = (MODEqAxes*) asf->CreateAxisSystem("MODEq", "MOE1");
   bf    = (BodyFixedAxes*) asf->CreateAxisSystem("BodyFixed", "BF1");

   try
   {
      LeapSecsFileReader* ls     = new LeapSecsFileReader(LeapFileName);
      ls->Initialize();
      EopFile *eop               = new EopFile(eopFileName);
      eop->Initialize();
      ItrfCoefficientsFile* itrf = new ItrfCoefficientsFile(nutFileName, planFileName);
      itrf->Initialize();
      bf->SetEopFile(eop);
      bf->SetCoefficientsFile(itrf);
      todeq->SetEopFile(eop);
      todeq->SetCoefficientsFile(itrf);
      modeq->SetEopFile(eop);
      modeq->SetCoefficientsFile(itrf);
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
   if (todeq)
   {
      cout << "TODEq object exists with name " << todeq->GetName() << 
      " and type " << todeq->GetTypeName() << endl;
   }
   if (modeq)
   {
      cout << "MODEq object exists with name " << modeq->GetName() << 
      " and type " << modeq->GetTypeName() << endl;
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
   
   //28 June 2000 15: 8:51.655
   Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(2000,6,28,15,8,51.655));
   A1Mjd testTime(atTime);
   cout << "The test time (as an A1MJD) is " << testTime.Get() << endl;
   
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
   
   
   CoordinateSystem* todCS = new CoordinateSystem("", "CoordSystemTODEq");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   todCS->SetSolarSystem(ss);
   todCS->SetStringParameter("OriginName",SolarSystem::EARTH_NAME); 
   todCS->SetStringParameter("J2000BodyName",j2000BN);
   isOK = todCS->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = todCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = todCS->SetRefObject(todeq, Gmat::AXIS_SYSTEM, todeq->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   todeq->SetEpoch(testTime);
   todCS->Initialize();
   
   
   CoordinateSystem* modCS = new CoordinateSystem("", "CoordSystemMODEq");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   modCS->SetSolarSystem(ss);
   modCS->SetStringParameter("OriginName",SolarSystem::EARTH_NAME); 
   modCS->SetStringParameter("J2000BodyName",j2000BN);
   isOK = modCS->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = modCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = modCS->SetRefObject(modeq, Gmat::AXIS_SYSTEM, modeq->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   modeq->SetRealParameter("Epoch",testTime.Get());
   modCS->Initialize();

   CoordinateSystem* bfCS = new CoordinateSystem("", "CoordSystemMODEq");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   bfCS->SetSolarSystem(ss);
   bfCS->SetStringParameter("OriginName",SolarSystem::EARTH_NAME); 
   bfCS->SetStringParameter("J2000BodyName",j2000BN);
   isOK = bfCS->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = bfCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = bfCS->SetRefObject(bf, Gmat::AXIS_SYSTEM, bf->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   bfCS->Initialize();
   
   /*
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
      GmatBase *toeObj2 = todCS->GetRefObject(Gmat::AXIS_SYSTEM, todeq->GetName());
      if (toeObj2) 
      {
         cout << "mj2000ec has a pointer to the axis system " << toeObj2->GetName() << 
         " of type " << toeObj2->GetTypeName() << endl;
      }
      else       cout << "ERROR -  no pointer to the axis system ????????" << endl;
   }
   catch (BaseException &be)
   {
      cout << "ERROR ------------- : " << be.GetMessage() << endl;
   }
   */
   CoordinateConverter *cc = new CoordinateConverter();

   Rvector6 theState(3961.7442603,6010.2156109,4619.3625758,
                     -5.314643386, 3.964357585, 1.752939153);

   Rvector6 outState;
   
   cout << "The test Rvector6 is " << endl << theState << endl;
   
   cout << "About to try to convert!!!!!" << endl;
   
   try
   {
      modeq->SetRealParameter("Epoch",testTime.Get());
      cc->Convert(testTime,theState, mj2000, outState, todCS);
      cout << "The output state (to TODEq) is : " << endl << outState << endl;
      cc->Convert(testTime,theState, mj2000, outState, modCS);
      cout << "The output state (to MODEq) is : " << endl << outState << endl;
      cc->Convert(testTime,theState, mj2000, outState, bfCS);
      cout << "The output state (to BodyFixed) is : " << endl << outState << endl;
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
   delete todCS;
   cout << "Now delete other CoordinateSystem (and its AxisSystem) ............." << endl;
   delete modCS;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "Now delete everything else ............." << endl;
   delete asf;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "=-=-=-=-=-=-= END TEST coordinate system ....." << endl;

}
