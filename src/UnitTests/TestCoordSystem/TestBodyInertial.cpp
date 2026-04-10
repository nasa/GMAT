//------------------------------------------------------------------------------
//                                  TestBodyInertial
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
// Created: 2006.08.02
//
/**
 * Test driver for BodyInertialAxes coordinate system stuff (and the factory to
 * create object of BodyInertialAxes class).
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
#include "BodyInertialAxes.hpp"
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
   std::string eopFileName    = "/Users/wcshoan/dev/Ec_GMAT/bin/files/planetary_coeff/eopc04.62-now";
   std::string nutFileName    = "/Users/wcshoan/dev/Ec_GMAT/bin/files/planetary_coeff/NUTATION.DAT";
   std::string planFileName   = "/Users/wcshoan/dev/Ec_GMAT/bin/files/planetary_coeff/NUT85.DAT";
   std::string SLPFileName    = "/Users/wcshoan/dev/Ec_GMAT/bin/files/planetary_ephem/slp/DBS_mn2000.dat";
   std::string DEFileName     = "/Users/wcshoan/dev/Ec_GMAT/bin/files/planetary_ephem/de/macp1941.405";
   std::string LeapFileName   = "/Users/wcshoan/dev/Ec_GMAT/bin/files/time/tai-utc.dat";
   //std::string eopFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/eop/eopc04.62-now";
   //std::string nutFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf/NUTATION.DAT";
   //std::string planFileName   = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/itrf//NUT85.DAT";
   //std::string SLPFileName    = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/slp/DBS_mn2000.dat";
   //std::string DEFileName     = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/planetary_ephem/de/macp1941.405";
   //std::string LeapFileName   = "/Users/wendyshoan/WORKONLY/dev/build3/bin/files/time/tai-utc.dat";
   
   cout << "=-=-=-=-=-=-= TEST BodyInertialAxes ....." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout.setf(ios::fixed);
   cout.precision(16);

   
   cout << "\n==> First, test the AxisSystemFactory <==" << endl;
   AxisSystemFactory *asf = new AxisSystemFactory();
   cout << "AxisSystemFactory created .........." << endl;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;

   MJ2000EqAxes     *mj;
   BodyInertialAxes *biSol;
   BodyInertialAxes *biMercury;
   BodyInertialAxes *biVenus;
   BodyInertialAxes *biEarth;
   BodyInertialAxes *biLuna;
   BodyInertialAxes *biMars;
   BodyInertialAxes *biJupiter;
   BodyInertialAxes *biSaturn;
   BodyInertialAxes *biUranus;
   BodyInertialAxes *biNeptune;
   BodyInertialAxes *biPluto;
   
   mj        = (MJ2000EqAxes*)      asf->CreateAxisSystem("MJ2000Eq",     "mj2000");
   biSol     = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biSol");
   biMercury = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biMercury");
   biVenus   = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biVenus");
   biEarth   = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biEarth");
   biLuna    = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biLuna");
   biMars    = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biMars");
   biJupiter = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biJupiter");
   biSaturn  = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biSaturn");
   biUranus  = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biUranus");
   biNeptune = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biNeptune");
   biPluto   = (BodyInertialAxes*)  asf->CreateAxisSystem("BodyInertial", "biPluto");

   try
   {
      LeapSecsFileReader* ls     = new LeapSecsFileReader(LeapFileName);
      ls->Initialize();
      EopFile *eop               = new EopFile(eopFileName);
      eop->Initialize();
      TimeConverterUtil::SetLeapSecsFileReader(ls);
      TimeConverterUtil::SetEopFile(eop);
   }
   catch (BaseException &bbee)
   {
      cout << "ERROR !!!!! " << bbee.GetMessage() << endl;
   }

   
   SolarSystem*   ss;
   Star*          sol;
   Planet         *mercury, *venus, *earth, *mars;
   Planet         *jupiter, *saturn, *uranus, *neptune, *pluto;
   Moon*          luna;
   std::string    j2000BN = "";
   SpacePoint*    j2000B  = NULL;
   try
   {
      cout << "\n==> Create the solar system <==" << endl;
      
      ss              = new SolarSystem("TheSS");
      cout << "solar system name = " << ss->GetName() << endl;

      sol     = (Star*)   ss->GetBody(SolarSystem::SUN_NAME);
      mercury = (Planet*) ss->GetBody(SolarSystem::MERCURY_NAME);
      venus   = (Planet*) ss->GetBody(SolarSystem::VENUS_NAME);      
      earth   = (Planet*) ss->GetBody(SolarSystem::EARTH_NAME);
      luna    = (Moon*)   ss->GetBody(SolarSystem::MOON_NAME);
      mars    = (Planet*) ss->GetBody(SolarSystem::MARS_NAME);
      jupiter = (Planet*) ss->GetBody(SolarSystem::JUPITER_NAME);
      saturn  = (Planet*) ss->GetBody(SolarSystem::SATURN_NAME);
      uranus  = (Planet*) ss->GetBody(SolarSystem::URANUS_NAME);
      neptune = (Planet*) ss->GetBody(SolarSystem::NEPTUNE_NAME);
      pluto   = (Planet*) ss->GetBody(SolarSystem::PLUTO_NAME);

      cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;

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
      j2000BN = SolarSystem::EARTH_NAME;
      j2000B  = earth;

      mj->SetJ2000BodyName(j2000BN);
      mj->SetJ2000Body(j2000B);
      sol->SetJ2000BodyName(j2000BN);
      sol->SetJ2000Body(j2000B);
      mercury->SetJ2000BodyName(j2000BN);
      mercury->SetJ2000Body(j2000B);
      venus->SetJ2000BodyName(j2000BN);
      venus->SetJ2000Body(j2000B);
      earth->SetJ2000BodyName(j2000BN);
      earth->SetJ2000Body(j2000B);
      luna->SetJ2000BodyName(j2000BN);
      luna->SetJ2000Body(j2000B);
      mars->SetJ2000BodyName(j2000BN);
      mars->SetJ2000Body(j2000B);
      jupiter->SetJ2000BodyName(j2000BN);
      jupiter->SetJ2000Body(j2000B);
      saturn->SetJ2000BodyName(j2000BN);
      saturn->SetJ2000Body(j2000B);
      uranus->SetJ2000BodyName(j2000BN);
      uranus->SetJ2000Body(j2000B);
      neptune->SetJ2000BodyName(j2000BN);
      neptune->SetJ2000Body(j2000B);
      pluto->SetJ2000BodyName(j2000BN);
      pluto->SetJ2000Body(j2000B);
      
   }
   catch (GmatBaseException &ex)
   {
      cout << "Some kind of error ..........." << endl;
   }
   
   bool isOK = false;
   std::string itsName = "";
   CoordinateSystem *biSolCS, *biMercuryCS, *biVenusCS, *biEarthCS, *biLunaCS;
   CoordinateSystem *biMarsCS, *biJupiterCS, *biSaturnCS, *biUranusCS;
   CoordinateSystem *biNeptuneCS, *biPlutoCS, *mj2000CS;
   cout << "\n==> Now creating CoordinateSystems <==" << endl;
   try
   {
      itsName = "mj2000CS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      mj2000CS = new CoordinateSystem("", itsName);
      mj2000CS->SetSolarSystem(ss);
      mj2000CS->SetStringParameter("J2000Body",j2000BN); 
      mj2000CS->SetStringParameter("Origin",SolarSystem::EARTH_NAME); 
      isOK = mj2000CS->SetRefObject(earth,Gmat::SPACE_POINT,SolarSystem::EARTH_NAME);
      isOK = mj2000CS->SetRefObject(mj, Gmat::AXIS_SYSTEM, mj->GetName());
      if (isOK) cout << "   " <<itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      mj2000CS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biSolCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biSolCS = new CoordinateSystem("", itsName);
      biSolCS->SetSolarSystem(ss);
      biSolCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biSolCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biSolCS->SetStringParameter("Origin",SolarSystem::SUN_NAME); 
      isOK = biSolCS->SetRefObject(sol,Gmat::SPACE_POINT,SolarSystem::SUN_NAME);
      isOK = biSolCS->SetRefObject(biSol, Gmat::AXIS_SYSTEM, biSol->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biSolCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biMercuryCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biMercuryCS = new CoordinateSystem("", itsName);
      biMercuryCS->SetSolarSystem(ss);
      biMercuryCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biMercuryCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biMercuryCS->SetStringParameter("Origin",SolarSystem::MERCURY_NAME); 
      isOK = biMercuryCS->SetRefObject(mercury,Gmat::SPACE_POINT,SolarSystem::MERCURY_NAME);
      isOK = biMercuryCS->SetRefObject(biMercury, Gmat::AXIS_SYSTEM, biMercury->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biMercuryCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biVenusCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biVenusCS = new CoordinateSystem("", itsName);
      biVenusCS->SetSolarSystem(ss);
      biVenusCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biVenusCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biVenusCS->SetStringParameter("Origin",SolarSystem::VENUS_NAME); 
      isOK = biVenusCS->SetRefObject(venus,Gmat::SPACE_POINT,SolarSystem::VENUS_NAME);
      isOK = biVenusCS->SetRefObject(biVenus, Gmat::AXIS_SYSTEM, biVenus->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biVenusCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biEarthCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biEarthCS = new CoordinateSystem("", itsName);
      biEarthCS->SetSolarSystem(ss);
      biEarthCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biEarthCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biEarthCS->SetStringParameter("Origin",SolarSystem::EARTH_NAME); 
      isOK = biEarthCS->SetRefObject(earth,Gmat::SPACE_POINT,SolarSystem::EARTH_NAME);
      isOK = biEarthCS->SetRefObject(biEarth, Gmat::AXIS_SYSTEM, biEarth->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biEarthCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biLunaCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biLunaCS = new CoordinateSystem("", itsName);
      biLunaCS->SetSolarSystem(ss);
      biLunaCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biLunaCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biLunaCS->SetStringParameter("Origin",SolarSystem::MOON_NAME); 
      isOK = biLunaCS->SetRefObject(luna,Gmat::SPACE_POINT,SolarSystem::MOON_NAME);
      isOK = biLunaCS->SetRefObject(biLuna, Gmat::AXIS_SYSTEM, biLuna->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biLunaCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biMarsCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biMarsCS = new CoordinateSystem("", itsName);
      biMarsCS->SetSolarSystem(ss);
      biMarsCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biMarsCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biMarsCS->SetStringParameter("Origin",SolarSystem::MARS_NAME); 
      isOK = biMarsCS->SetRefObject(mars,Gmat::SPACE_POINT,SolarSystem::MARS_NAME);
      isOK = biMarsCS->SetRefObject(biMars, Gmat::AXIS_SYSTEM, biMars->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biMarsCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biJupiterCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biJupiterCS = new CoordinateSystem("", itsName);
      biJupiterCS->SetSolarSystem(ss);
      biJupiterCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biJupiterCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biJupiterCS->SetStringParameter("Origin",SolarSystem::JUPITER_NAME); 
      isOK = biJupiterCS->SetRefObject(jupiter,Gmat::SPACE_POINT,SolarSystem::JUPITER_NAME);
      isOK = biJupiterCS->SetRefObject(biJupiter, Gmat::AXIS_SYSTEM, biJupiter->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biJupiterCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biSaturnCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biSaturnCS = new CoordinateSystem("", itsName);
      biSaturnCS->SetSolarSystem(ss);
      biSaturnCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biSaturnCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biSaturnCS->SetStringParameter("Origin",SolarSystem::SATURN_NAME); 
      isOK = biSaturnCS->SetRefObject(saturn,Gmat::SPACE_POINT,SolarSystem::SATURN_NAME);
      isOK = biSaturnCS->SetRefObject(biSaturn, Gmat::AXIS_SYSTEM, biSaturn->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biSaturnCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biUranusCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biUranusCS = new CoordinateSystem("", itsName);
      biUranusCS->SetSolarSystem(ss);
      biUranusCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biUranusCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biUranusCS->SetStringParameter("Origin",SolarSystem::URANUS_NAME); 
      isOK = biUranusCS->SetRefObject(uranus,Gmat::SPACE_POINT,SolarSystem::URANUS_NAME);
      isOK = biUranusCS->SetRefObject(biUranus, Gmat::AXIS_SYSTEM, biUranus->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biUranusCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biNeptuneCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biNeptuneCS = new CoordinateSystem("", itsName);
      biNeptuneCS->SetSolarSystem(ss);
      biNeptuneCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biNeptuneCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biNeptuneCS->SetStringParameter("Origin",SolarSystem::NEPTUNE_NAME); 
      isOK = biNeptuneCS->SetRefObject(neptune,Gmat::SPACE_POINT,SolarSystem::NEPTUNE_NAME);
      isOK = biNeptuneCS->SetRefObject(biNeptune, Gmat::AXIS_SYSTEM, biNeptune->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biNeptuneCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   try
   {
      itsName = "biPlutoCS";
      cout << ">> Attempting to create " << itsName << " ..........." << endl;
      biPlutoCS = new CoordinateSystem("", itsName);
      biPlutoCS->SetSolarSystem(ss);
      biPlutoCS->SetStringParameter("J2000Body",j2000BN); 
      isOK = biPlutoCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
      biPlutoCS->SetStringParameter("Origin",SolarSystem::PLUTO_NAME); 
      isOK = biPlutoCS->SetRefObject(pluto,Gmat::SPACE_POINT,SolarSystem::PLUTO_NAME);
      isOK = biPlutoCS->SetRefObject(biPluto, Gmat::AXIS_SYSTEM, biPluto->GetName());
      if (isOK) cout << "   " << itsName << " seems to have been set OK!!!!!!!!!!!" << endl;
      else      cout << "ERROR setting up " <<  itsName << " references object(s)!!!!!" << endl;
      cout << "   Initializing " << itsName << " ..........." << endl;
      biPlutoCS->Initialize();
   }
   catch (BaseException &be)
   {
      cout << "*** ERROR *** creating and/or initializing " << itsName << "!!!" << endl;
      cout << be.GetMessage() << endl;
   }
   

   CoordinateConverter *cc = new CoordinateConverter();
   Rvector6 theState(-6821.47284360074,1261.20410114993,775.40938920649,
                    -0.43821296011, -2.43981897400, 0.09021855328);
   Rvector6 outState;
   
   A1Mjd atTime1;
   
   //Real ut1_utc = -0.5036517;  // 2005/01/01
   Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(1991,4,6,7,51,28.39));
   A1Mjd testTime(atTime);

   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biSolCS);
      cout << "------ The rotation matrix from biSolCS to MJ2000Eq is " << endl <<
              biSolCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation DOT matrix from biSolCS to MJ2000Eq is " << endl <<
              biSolCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biMercuryCS);
      cout << "------ The rotation matrix from biMercuryCS to MJ2000Eq is " << endl <<
              biMercuryCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation DOT matrix from biMercuryCS to MJ2000Eq is " << endl <<
              biMercuryCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biVenusCS);
      cout << "------ The rotation matrix from biVenusCS to MJ2000Eq is " << endl <<
              biVenusCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation matrix DOT from biVenusCS to MJ2000Eq is " << endl <<
              biVenusCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biEarthCS);
      cout << "------ The rotation matrix from biEarthCS to MJ2000Eq is " << endl <<
              biEarthCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation DOT matrix from biEarthCS to MJ2000Eq is " << endl <<
              biEarthCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biLunaCS);
      cout << "------ The rotation matrix from biLunaCS to MJ2000Eq is " << endl <<
              biLunaCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation DOT matrix from biLunaCS to MJ2000Eq is " << endl <<
              biLunaCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biMarsCS);
      cout << "------ The rotation matrix from biMarsCS to MJ2000Eq is " << endl <<
              biMarsCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation DOT matrix from biMarsCS to MJ2000Eq is " << endl <<
              biMarsCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biJupiterCS);
      cout << "------ The rotation matrix from biJupiterCS to MJ2000Eq is " << endl <<
              biJupiterCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation DOT matrix from biJupiterCS to MJ2000Eq is " << endl <<
              biJupiterCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biSaturnCS);
      cout << "------ The rotation matrix from biSaturnCS to MJ2000Eq is " << endl <<
              biSaturnCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation DOT matrix from biSaturnCS to MJ2000Eq is " << endl <<
              biSaturnCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biUranusCS);
      cout << "------ The rotation matrix from biUranusCS to MJ2000Eq is " << endl <<
              biUranusCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation DOT matrix from biUranusCS to MJ2000Eq is " << endl <<
              biUranusCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biNeptuneCS);
      cout << "------ The rotation matrix from biNeptuneCS to MJ2000Eq is " << endl <<
              biNeptuneCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation DOT matrix from biNeptuneCS to MJ2000Eq is " << endl <<
              biNeptuneCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }
   try
   {
      cc->Convert(testTime,theState, mj2000CS, outState, biPlutoCS);
      cout << "------ The rotation matrix from biPlutoCS to MJ2000Eq is " << endl <<
              biPlutoCS->GetLastRotationMatrix() << endl;
      cout << "------ The rotation DOT matrix from biPlutoCS to MJ2000Eq is " << endl <<
              biPlutoCS->GetLastRotationDotMatrix() << endl;
   }
   catch (BaseException &cse)
   {
      cout << "ERROR: " << cse.GetMessage() << endl;
   }

   
   cout << "Now delete SolarSystem ............." << endl;
   delete ss;
   cout << "Now delete CoordinateConverter ............." << endl;
   delete cc;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   cout << "Now delet each coordinate system ....." << endl;
   delete mj2000CS;
   delete biSolCS;
   delete biMercuryCS;
   delete biVenusCS;
   delete biEarthCS;
   delete biLunaCS;
   delete biMarsCS;
   delete biJupiterCS;
   delete biSaturnCS;
   delete biUranusCS;
   delete biNeptuneCS;
   delete biPlutoCS;
   
   cout << "Now delete everything else ............." << endl;
   delete asf;
   cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout << "=-=-=-=-=-=-= END TEST BodyInertialAxes ....." << endl;

}
