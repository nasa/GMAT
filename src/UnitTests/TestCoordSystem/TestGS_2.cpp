//------------------------------------------------------------------------------
//                                  TestGeocentricSolar
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
// Created: 2005/06/13
//
/**
 * Test driver for Geocentric Solar (Ecliptic and Magnetic) coordinate systems
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
#include "ObjectReferencedAxes.hpp"
#include "TODEqAxes.hpp"
#include "MODEqAxes.hpp"
#include "TODEcAxes.hpp"
#include "MODEcAxes.hpp"
#include "TOEEcAxes.hpp"
#include "MOEEcAxes.hpp"
#include "GeocentricSolarEclipticAxes.hpp"
#include "GeocentricSolarMagneticAxes.hpp"
#include "SlpFile.hpp"
#include "DeFile.hpp"
#include "TimeSystemConverter.hpp"
#include "LeapSecsFileReader.hpp"
#include "EopFile.hpp"
#include "ItrfCoefficientsFile.hpp"


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
   "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/slp/DBS_mn2000.dat";
   std::string DEFileName     = 
   "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/de/macp1941.405";
   std::string LeapFileName   = 
   "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/time/tai-utc.dat";
    */
   
   cout << "=-=-=-=-=-=-= TEST Geocentric Solar Systems ....." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout.setf(ios::fixed);
   cout.precision(16);

   
   AxisSystemFactory *asf = new AxisSystemFactory();
   cout << "AxisSystemFactory created .........." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   MJ2000EqAxes*                mj;
   GeocentricSolarEclipticAxes* gse;
   GeocentricSolarMagneticAxes* gsm;
   ObjectReferencedAxes*        ora;
   mj    = (MJ2000EqAxes*)                 asf->CreateAxisSystem("MJ2000Eq", "MJ1");
   gse   = (GeocentricSolarEclipticAxes*)  asf->CreateAxisSystem("GSE", "Gse1");
   gsm   = (GeocentricSolarMagneticAxes*)  asf->CreateAxisSystem("GSM", "Gsm1");
   ora   = (ObjectReferencedAxes*)         asf->CreateAxisSystem("ObjectReferenced", "Ora1");
   cout << "Created all of the axis systems ......" << endl;

   LeapSecsFileReader    *ls;
   EopFile               *eop;
   ItrfCoefficientsFile *itrf;
   cout << "About to create the data files ....." << endl;
   try
   {
      ls     = new LeapSecsFileReader(LeapFileName);
      cout << "LeapSec file created ...." << endl;
      eop    = new EopFile(eopFileName);
      cout << "EopFile file created ...." << endl;
      itrf   = new ItrfCoefficientsFile(nutFileName, planFileName);
      cout << "ITRF file created ...." << endl;
      ls->Initialize();
      eop->Initialize();
      itrf->Initialize();
      cout << "All are initialized ...." << endl;
      /*
      if (ora->UsesEopFile() != GmatCoordinate::NOT_USED)
      {
         cout << "Setting eop file for ORA .... " << endl;
         ora->SetEopFile(eop);
      }
      else cout << "ORA does ot use EOP file. " << endl;
      if (ora->UsesItrfFile() != GmatCoordinate::NOT_USED)
      {
         cout << "Setting itrf file for ORA .... " << endl;
         ora->SetCoefficientsFile(itrf);
      }
      else cout << "ORA does ot use ITRF file. " << endl;
       */
      gsm->SetEopFile(eop);
      gsm->SetCoefficientsFile(itrf);
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
   if (gse)
   {
      cout << "GSE object exists with name " << gse->GetName() <<
      " and type " << gse->GetTypeName() << endl;
   }
   if (gsm)
   {
      cout << "GSM object exists with name " << gsm->GetName() <<
      " and type " << gsm->GetTypeName() << endl;
   }
   if (ora)
   {
      cout << "ORA object exists with name " << ora->GetName() <<
      " and type " << ora->GetTypeName() << endl;
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
      //cout << "solar system name = " << ss->GetName() << endl;
      //cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;

      earth = (Planet*) ss->GetBody(SolarSystem::EARTH_NAME);
      //cout << "earth name = " << earth->GetName() << endl;
      //cout << "earth's type name is " << earth->GetTypeName() << endl;
      
      sol = (Star*) ss->GetBody(SolarSystem::SUN_NAME);
      //cout << "sol name = " << sol->GetName() << endl;
      //cout << "sol's type name is " << sol->GetTypeName() << endl;

      luna = (Moon*) ss->GetBody(SolarSystem::MOON_NAME);
      //cout << "luna name = " << luna->GetName() << endl;
      //cout << "luna's type name is " << luna->GetTypeName() << endl;

      mars = (Planet*) ss->GetBody(SolarSystem::MARS_NAME);

      jupiter = (Planet*) ss->GetBody(SolarSystem::JUPITER_NAME);
      cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;

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
   Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(2004,6,1,11,59,(60.0 - 0.4706294)));
   A1Mjd testTime(atTime);
   Real epochTime = theEpoch.UtcMjdToA1Mjd(ModifiedJulianDate(2004,6,1,11,59,(60.0 - 0.4706294)));
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
   
   
   CoordinateSystem* gseCS = new CoordinateSystem("", "CoordSystemGSE");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   gseCS->SetSolarSystem(ss);
   gseCS->SetStringParameter("Origin",SolarSystem::EARTH_NAME); 
   gseCS->SetStringParameter("J2000Body",j2000BN);
   isOK = gseCS->SetRefObject(earth,Gmat::SPACE_POINT,SolarSystem::EARTH_NAME); 
   //isOK = gse->SetRefObject(earth,Gmat::SPACE_POINT,SolarSystem::EARTH_NAME); // * NOTE
   //isOK = gse->SetRefObject(sol,Gmat::SPACE_POINT,SolarSystem::SUN_NAME);     // * NOTE
   gse->SetPrimaryObject(earth);
   gse->SetSecondaryObject(sol);
   //if (!isOK) cout << "problem setting secondary ......... " << endl;
   isOK = gseCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = gseCS->SetRefObject(gse, Gmat::AXIS_SYSTEM, gse->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   gseCS->Initialize();

   CoordinateSystem* gsmCS = new CoordinateSystem("", "CoordSystemGSM");
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   gsmCS->SetSolarSystem(ss);
   gsmCS->SetStringParameter("Origin",SolarSystem::EARTH_NAME);
   gsmCS->SetStringParameter("J2000Body",j2000BN);
   isOK = gsmCS->SetRefObject(earth,Gmat::SPACE_POINT,SolarSystem::EARTH_NAME);
   //isOK = gse->SetRefObject(earth,Gmat::SPACE_POINT,SolarSystem::EARTH_NAME); // * NOTE
   //isOK = gse->SetRefObject(sol,Gmat::SPACE_POINT,SolarSystem::SUN_NAME);     // * NOTE
   gsm->SetPrimaryObject(earth);
   gsm->SetSecondaryObject(sol);
   //if (!isOK) cout << "problem setting secondary ......... " << endl;
   isOK = gsmCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = gsmCS->SetRefObject(gsm, Gmat::AXIS_SYSTEM, gsm->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   gsmCS->Initialize();
   cout << "GSM initialized ......." << endl;

   try
   {
      ora->SetStringParameter("Primary",SolarSystem::SUN_NAME);
      ora->SetStringParameter("Secondary",SolarSystem::EARTH_NAME);
      cout << "Strings set for ora ......." << endl;
      isOK = ora->SetRefObject(earth,Gmat::SPACE_POINT,SolarSystem::EARTH_NAME);
      isOK = ora->SetRefObject(sol,Gmat::SPACE_POINT,SolarSystem::SUN_NAME);
      cout << "Bodies set for ora ......." << endl;
   }
   catch (BaseException &beORA)
   {
      cout << "ERROR ------" <<  beORA.GetMessage() << endl;
      cout << "Trying to continue .........." << endl;
   }
   CoordinateSystem* orCS = new CoordinateSystem("", "CoordSystemObjRef");
   try
   {
      cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
      orCS->SetSolarSystem(ss);
      orCS->SetStringParameter("Origin",SolarSystem::EARTH_NAME);
      orCS->SetStringParameter("J2000Body",j2000BN);
      //orCS->SetPrimaryObject(earth);
      //orCS->SetSecondaryObject(luna);
      isOK = orCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      isOK = orCS->SetRefObject(earth,Gmat::SPACE_POINT,SolarSystem::EARTH_NAME);
      isOK = orCS->SetRefObject(ora, Gmat::AXIS_SYSTEM, ora->GetName());
      if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
      orCS->SetXAxis("-R");
      orCS->SetZAxis("N");
      orCS->Initialize();
   }
   catch (BaseException& beor)
   {
      cout << "ERROR in ObjRef - " << beor.GetMessage() << endl;
   }
   
   
   CoordinateConverter *cc = new CoordinateConverter();

   Rvector6 theState(-4453.783586, -5038.203756, -426.384456,
                     3.831888000, -2.887221000, -6.018232000);

   Rvector6 outState;
   
   cout << "The test Rvector6 is " << endl << theState << endl;
   
   cout << "About to try to convert!!!!!" << endl;
   
   try
   {
      cc->Convert(testTime,theState, mj2000, outState, orCS);
      cout << "The output state (to ORA) is : " << endl << outState << endl;
      cc->Convert(testTime,theState, mj2000, outState, gseCS);
      cout << "The output state (to GSE) is : " << endl << outState << endl;
      cc->Convert(testTime,theState, mj2000, outState, gsmCS);
      cout << "The output state (to GSM) is : " << endl << outState << endl;
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
   delete gseCS;
   delete gsmCS;
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
   //if (aDE)  delete aDE;
   //cout << "aDE deleted ............." << endl;
   //delete asf;
   //cout << "asf deleted ............." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "=-=-=-=-=-=-= END TEST Ecliptic systems ....." << endl;

}
