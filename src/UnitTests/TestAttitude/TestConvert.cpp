//------------------------------------------------------------------------------
//                                  TestConvert
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
// Created: 2006.03.29
//
/**
 * Test driver for attitude conversion tests
 */
//------------------------------------------------------------------------------
#include <map>
#include <iostream>
#include <string>
#include <list>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SolarSystem.hpp"
#include "SpacePoint.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "Planet.hpp"
#include "Moon.hpp"
#include "A1Mjd.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "Linear.hpp"
#include "MessageWindow.hpp"
#include "Spacecraft.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateSystemException.hpp"
#include "AxisSystemFactory.hpp"
#include "SpacecraftFactory.hpp"
#include "MJ2000EqAxes.hpp"
#include "MJ2000EcAxes.hpp"
#include "BodyFixedAxes.hpp"
#include "EquatorAxes.hpp"
#include "SlpFile.hpp"
#include "DeFile.hpp"
#include "TimeSystemConverter.hpp"
#include "LeapSecsFileReader.hpp"
#include "ItrfCoefficientsFile.hpp"
#include "EopFile.hpp"
#include "Attitude.hpp"
#include "AttitudeFactory.hpp"
#include "CSFixed.hpp"
#include "Spinner.hpp"
#include "RealUtilities.hpp"


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
   
   cout << "=-=-=-=-=-=-= TEST attitude conversions and propagation ....." << endl;
   //cout << " ------ number of objects instantiated = " << GmatBase::GetInstanceCount() << endl;
   
   cout.setf(ios::fixed);
   cout.precision(18);
   
   
   cout << endl << ">>>>>>>******* First, test the conversions *******<<<<<<<" << endl;
   Rmatrix33 dcm1( 0.813797681349374,  0.469846310392954, -0.342020143325669,
                  -0.440969610529882,  0.882564119259386,  0.163175911166535,
                   0.378522306369792,  0.0180283112362973, 0.925416578398323);
   Rvector   quaternion1(4,0.0381345764748501, 0.189307857412, 
                           0.23929833774473,   0.951548524643789);
   Integer   seq[12][3] = {{1,2,3},{1,3,2},{2,3,1},{2,1,3},{3,1,2},{3,2,1},
                           {1,2,1},{1,3,1},{2,1,2},{2,3,2},{3,1,3},{3,2,3}};
   Real      theta1[12] = {-1.11605467700463, 10.4750381270859, 22.7958772588585,
                           22.245989694115, 26.5488216029811, 29.9999999999999,
                           53.9476112676121, 323.947611267612, 290.306434286384,
                           20.3064342863842, 92.7268304431963, 2.72683044319633};
   Real      theta2[12] = {22.2421809103095, 26.1657624772213, 28.0243206736047,
                           -1.03300210846732, 9.39128580204349, 20.00,
                           35.5313477628041, 35.5313477628041, 28.0467644314485,
                           28.0467644314485, 22.2687444952968, 22.2687444952968};
   Real      theta3[12] = {28.4517752565854, 24.944585788682, -1.17022943307879,
                           28.0292778865611, 20.2835594545297, 9.99999999999998,
                           310.642342047956, 40.642342047956, 92.1973986643425,
                           2.19739866434246, 295.505550260983, 25.5055502609825};
   Real      phi        = 35.8171011735842; 
   Rvector3  a(0.124015436814206, 0.615638058673445, 0.778209452618364);
   Rvector3  rates[12];
   rates[0].Set(2.1759016656915, 6.77818662316379, 4.17637268306857);
   rates[1].Set(7.40069642152502, 2.42487291788045, 8.26348221864978);
   rates[2].Set(5.77862592877558, 4.8968424672837, 2.28493392822373);
   rates[3].Set(6.76425066505365, 2.06392447996366, 4.87805198373008);
   rates[4].Set(2.9967754512649, 6.42327482092227, 4.51099843517834);
   rates[5].Set(6.16401525910817, 4.05579787672639, 7.10821738238178);
   rates[6].Set(-0.924506080756781, 7.05062659895456, 5.75236090491326);
   rates[7].Set(-0.924506080756778, 7.05062659895456, 5.75236090491326);
   rates[8].Set(11.0338644803049, 4.80461108809172, -4.7380928870878);
   rates[9].Set(11.0338644803049, 4.80461108809172, -4.7380928870878);
   rates[10].Set(-6.22698662109642, 6.66571052442454, 10.7625566526271);
   rates[11].Set(-6.22698662109643, 6.66571052442453, 10.7625566526272);
   
   Rvector3  angVel(5.0,5.0,5.0);
   
   cout << "Initial DCM = " << endl << dcm1 << endl << endl;
   cout << "Initial quaternion = " << endl << quaternion1 << endl << endl;
   cout << "Initial Euler sequences and angles are:" << endl;
   for (Integer i = 0; i < 12; i++)
      cout << "   " << seq[i][0] << "  " << seq[i][1] << "  " << seq[i][2] << "      "
         << theta1[i] << "   " << theta2[i] << "   " << theta3[i] << endl;
   cout << endl;
   cout << "Initial Euler angle and axis are:   " << endl << phi << endl << a << endl;   
   cout << "Initial Euler sequences and rates are:" << endl;
   for (Integer i = 0; i < 12; i++)
      cout << "   " << seq[i][0] << "  " << seq[i][1] << "  " << seq[i][2] << "      "
         << rates[i](0) << "   " << rates[i](1) << "   " << rates[i](2) << endl;
   cout << endl;
   cout << "Initial angular velocity is: " << endl;
   cout << angVel << endl;
   
   
   cout << endl << "******* Now convert the initial matrix to quaternion and what do we get?"
        << endl;
   cout << Attitude::ToQuaternion(dcm1) << endl;
   cout << endl 
      << "******* Now convert the initial quaternion to cosine matrix and what do we get?"
      << endl;
   cout << Attitude::ToCosineMatrix(quaternion1) << endl;
   
   //       ************ conversions between euler axis/angle and cosine matrix are
   //       ************ actually protected - was tested while those methods were
   //       ************ temporarily in the public part of the class - successful
   //cout << endl 
   //   << "******* Now convert the initial euler axis/angle to cosine matrix and what do we get?"
   //   << endl;
   //cout << sp->EulerAxisAndAngleToDCM(a,phi*GmatMathUtil::RAD_PER_DEG) << endl;
   //cout << endl 
   //   << "******* Now convert the initial cosine matrix to euler axis/angle and what do we get?"
   //   << endl;
   //Real phiOut;
   //Rvector3 aOut;
   //sp->DCMToEulerAxisAndAngle(dcm1,aOut,phiOut);
   //cout << phiOut * GmatMathUtil::DEG_PER_RAD << "      " << aOut[0] << "  " << aOut[1] << "  "
   //     << aOut[2]  << endl;
   cout << endl 
      << "******* Now convert the initial matrix to euler angles and what do we get?"
      << endl;
   Rvector3 eAng;
   for (Integer i=0;i<12;i++)
   {
      eAng = Attitude::ToEulerAngles(dcm1, seq[i][0], seq[i][1], seq[i][2])
             * GmatMathUtil::DEG_PER_RAD;
      cout << "For sequence " << seq[i][0] << " " << seq[i][1] << " " << seq[i][2] << "       " 
         <<  eAng[0] << "   " << eAng[1] << "   " << eAng[2] << endl;
   }
   cout << endl 
      << "******* Now convert the initial quaternion to euler angles and what do we get?"
      << endl;
   for (Integer i=0;i<12;i++)
   {
      eAng = Attitude::ToEulerAngles(quaternion1, seq[i][0], seq[i][1], seq[i][2])
      * GmatMathUtil::DEG_PER_RAD;
      cout << "For sequence " << seq[i][0] << " " << seq[i][1] << " " << seq[i][2] << "       " 
         <<  eAng[0] << "   " << eAng[1] << "   " << eAng[2] << endl;
   }

   Rmatrix33 cosMat;
   cout << endl 
      << "******* Now convert the initial euler angles to matrix and what do we get?"
      << endl;
   for (Integer i=0;i<12;i++)
   {
      eAng.Set(theta1[i] * GmatMathUtil::RAD_PER_DEG, 
               theta2[i] * GmatMathUtil::RAD_PER_DEG, 
               theta3[i] * GmatMathUtil::RAD_PER_DEG);
      cosMat = Attitude::ToCosineMatrix(eAng, seq[i][0], seq[i][1], seq[i][2]);
      cout << "For sequence " << seq[i][0] << " " << seq[i][1] << " " << seq[i][2] << endl;
      cout << cosMat << endl;
   }
   
   Rvector qOut(4,0.0,0.0,0.0,1.0);
   cout << endl 
      << "******* Now convert the initial euler angles (321) to quaternion and what do we get?"
      << endl;
   eAng.Set(theta1[5] * GmatMathUtil::RAD_PER_DEG,
            theta2[5] * GmatMathUtil::RAD_PER_DEG,
            theta3[5] * GmatMathUtil::RAD_PER_DEG);
   qOut = Attitude::ToQuaternion(eAng, seq[5][0], seq[5][1], seq[5][2]);
   cout << qOut << endl;
   //for (Integer i=0;i<12;i++)
   //{
   //   eAng.Set(theta1[i], theta2[i], theta3[i]);
   //   qOut = Attitude::ToQuaternion(eAng, seq[i][0], seq[i][1], seq[i][2]);
   //   cout << "For sequence " << seq[i][0] << " " << seq[i][1] << " " << seq[i][2] << endl;
   //   cout << qOut << endl;
   cout << endl 
      << "******* Now convert the initial euler angle rates to " << 
      "angular velocity and what do we get?"
      << endl;
   Rvector3 av;
   for (Integer i=0; i<12; i++)
   {      
      eAng.Set(theta1[i] * GmatMathUtil::RAD_PER_DEG, 
               theta2[i] * GmatMathUtil::RAD_PER_DEG, 
               theta3[i] * GmatMathUtil::RAD_PER_DEG);
      av = Attitude::ToAngularVelocity(rates[i] *GmatMathUtil::RAD_PER_DEG, 
                                       eAng, seq[i][0],
                                       seq[i][1], seq[i][2]);
      cout << "For sequence " << seq[i][0] << " " << seq[i][1] << " " 
      << seq[i][2] << "       " 
         <<  av[0] * GmatMathUtil::DEG_PER_RAD << "   " 
         <<  av[1] * GmatMathUtil::DEG_PER_RAD << "   " 
         <<  av[2] * GmatMathUtil::DEG_PER_RAD << endl;
   }

   cout << endl 
      << "******* Now convert the initial angular velocity to " << 
      "euler angle rates and what do we get?"
      << endl;
   Rvector3 ear;
   for (Integer i=0; i<12; i++)
   {      
      eAng.Set(theta1[i] * GmatMathUtil::RAD_PER_DEG, 
               theta2[i] * GmatMathUtil::RAD_PER_DEG, 
               theta3[i] * GmatMathUtil::RAD_PER_DEG);
      ear = Attitude::ToEulerAngleRates(angVel * GmatMathUtil::RAD_PER_DEG, 
                                        eAng, seq[i][0],
                                        seq[i][1], seq[i][2]);
      cout << "For sequence " << seq[i][0] << " " << seq[i][1] << " " 
         << seq[i][2] << "       " 
         <<  ear[0] * GmatMathUtil::DEG_PER_RAD << "   " 
         <<  ear[1] * GmatMathUtil::DEG_PER_RAD << "   " 
         <<  ear[2] * GmatMathUtil::DEG_PER_RAD << endl;
   }
   cout << endl << ">>>>>>>******* End test the conversions *******<<<<<<<" << endl;

   cout << endl << ">>>>>>>******* Second, test the propagations *******<<<<<<<" << endl;
   AxisSystemFactory *asf = new AxisSystemFactory();
   
   MJ2000EqAxes* mj;
   MJ2000EcAxes* mjec;
   //BodyFixedAxes* bf;
   mj   = (MJ2000EqAxes*)  asf->CreateAxisSystem("MJ2000Eq", "MJ1");
   mjec = (MJ2000EcAxes*)  asf->CreateAxisSystem("MJ2000Ec", "MJEC1");
   //bf   = (BodyFixedAxes*) asf->CreateAxisSystem("BodyFixed", "BF1");
   
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
   Moon*          luna;
   std::string    j2000BN = "";
   SpacePoint*    j2000B  = NULL;
   try
   {
      cout << "\n==> Create the solar system <==" << endl;
      
      ss              = new SolarSystem("TheSS");
      
      earth = (Planet*) ss->GetBody(SolarSystem::EARTH_NAME);
      
      sol = (Star*) ss->GetBody(SolarSystem::SUN_NAME);
      
      luna = (Moon*) ss->GetBody(SolarSystem::MOON_NAME);
      
      mars = (Planet*) ss->GetBody(SolarSystem::MARS_NAME);
      
      SlpFile* anSLP;
      DeFile*  aDE;
      try
      {
         anSLP          = new SlpFile(SLPFileName);
         aDE            = new DeFile(Gmat::DE405,DEFileName);
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
   }
   catch (GmatBaseException &ex)
   {
      cout << "Some kind of error ..........." << endl;
   }
   
   bool isOK = false;
   cout << "\n==> Now creating CoordinateSystems <==" << endl;
   CoordinateSystem* mj2000 = new CoordinateSystem("", "CoordSystemMJ2000");
   mj2000->SetSolarSystem(ss);
   mj2000->SetStringParameter("Origin","Earth"); 
   mj2000->SetStringParameter("J2000Body",j2000BN); 
   isOK = mj2000->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = mj2000->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = mj2000->SetRefObject(mj, Gmat::AXIS_SYSTEM, mj->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   mj2000->Initialize();
   
   
   CoordinateSystem* mj2000Ec = new CoordinateSystem("", "CoordSystemMJ2000Ecliptic");
   mj2000Ec->SetSolarSystem(ss);
   mj2000Ec->SetStringParameter("Origin","Earth"); 
   mj2000Ec->SetStringParameter("J2000Body",j2000BN); 
   isOK = mj2000Ec->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = mj2000Ec->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = mj2000Ec->SetRefObject(mjec, Gmat::AXIS_SYSTEM, mj->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   mj2000Ec->Initialize();
  /* 
   CoordinateSystem* bfCS = new CoordinateSystem("", "CoordSystemBodyFixed");
   bfCS->SetSolarSystem(ss);
   bfCS->SetStringParameter("Origin","Earth"); 
   bfCS->SetStringParameter("J2000Body",j2000BN);
   isOK = bfCS->SetRefObject(earth,Gmat::SPACE_POINT,"Earth");
   isOK = bfCS->SetRefObject(j2000B,Gmat::SPACE_POINT,j2000BN);
   isOK = bfCS->SetRefObject(bf, Gmat::AXIS_SYSTEM, bf->GetName());
   if (isOK) cout << "It seems to have been set OK!!!!!!!!!!!" << endl;
   else      cout << "ERROR setting axis system for coordinate system!!!!!" << endl;
   bfCS->Initialize();
   */
   
   cout << "NOW creating two spacecraft, one CSFixed and one a Spinner" << endl;
   SpacecraftFactory *scf = new SpacecraftFactory();
   Spacecraft *scCSF;
   Spacecraft *scSpin;
   try
   {
      scf = new SpacecraftFactory();
      cout << "Spacecraft Factory was created!!" << endl;
      scCSF  = (Spacecraft*) scf->CreateSpacecraft("Spacecraft", "CSFSC1");
      scSpin = (Spacecraft*) scf->CreateSpacecraft("Spacecraft", "SpinnerSC1");
      if (scCSF && scSpin)  
         cout << "SUCCESS creating two spacecraft, one CSFixed and one a Spinner" << endl;
   }
   catch (BaseException &be)
   {
      cout << "ERROR!!!!! -> " << be.GetMessage() << endl;
   }
   
   A1Mjd atTime1;   
   Real atTime = atTime1.UtcMjdToA1Mjd(ModifiedJulianDate(2004,6,1,12,0,0.0));
   
   // set epoch on the spacecraft ......
   ((SpaceObject*)scCSF)->SetEpoch(atTime);
   ((SpaceObject*)scSpin)->SetEpoch(atTime);
   
   // set the initial values for the attitudes
   cout << "\n==> Now, test the AttitudeFactory <==" << endl;
   AttitudeFactory *af = new AttitudeFactory();
   cout << "AttitudeFactory created .........." << endl;
   
   CSFixed       *csf = (CSFixed*) af->CreateAttitude("CSFixed", "CSF1");
   Spinner       *sp  = (Spinner*) af->CreateAttitude("Spinner", "SP1");
   
   if (csf && sp) 
      cout << "SUCCESS creating two attitude objects!!!" << endl;
   
   isOK = true;
   try
   {
      isOK = csf->SetStringParameter("ReferenceCoordinateSystemName", "CoordSystemMJ2000Ecliptic");
      if (!isOK) cout << "ERROR setting ref coord system on csf ......" << endl;
      isOK = sp->SetStringParameter("ReferenceCoordinateSystemName", "CoordSystemMJ2000Ecliptic");
      if (!isOK) cout << "ERROR setting ref coord system on sp ......" << endl;
   }
   catch (BaseException &be2)
   {
      cout << "ERROR setting coord system name ... " << be2.GetMessage() << endl;
   }
   cout << "About to set the objects ... " << endl;
   try
   {
      csf->SetRefObject(mj2000Ec, Gmat::COORDINATE_SYSTEM, "CoordSystemMJ2000Ecliptic");
      sp->SetRefObject(mj2000Ec, Gmat::COORDINATE_SYSTEM, "CoordSystemMJ2000Ecliptic");
   }
   catch (BaseException &be3)
   {
      cout << "ERROR setting object ... " << be3.GetMessage() << endl;
   }
   cout << "Objects successfully set ..........." << endl;
   
   Rmatrix33 I33(true);
   try
   {
      csf->SetRmatrixParameter("InitialDirectionCosineMatrix", I33);
      sp->SetRmatrixParameter("InitialDirectionCosineMatrix", I33);
   }
   catch (BaseException &be4)
   {
      cout << "ERROR setting matrix ... " << be4.GetMessage() << endl;
   }
   cout << "Matrix set successfuly ... " << endl;
   
   Rvector3 initAV(0.0, 0.0, 0.125663706143592);
   initAV *= GmatMathUtil::DEG_PER_RAD; // need initial things input in deg/sec
   try
   {
      csf->SetRvectorParameter("InitialAngularVelocity", initAV);
      sp->SetRvectorParameter("InitialAngularVelocity", initAV);
   }
   catch (BaseException &be5)
   {
      cout << "ERROR setting ang vel ... " << be5.GetMessage() << endl;
   }
   cout << "Successfully set angular velocity ... " << endl;
   
   
   cout << "Now giving the spacecraft their attitudes ........." << endl;
   try
   {
      scCSF->SetRefObject(csf, Gmat::ATTITUDE, "CSF1");
      scSpin->SetRefObject(sp, Gmat::ATTITUDE, "SP1");
   }
   catch (BaseException &be7)
   {
      cout << "ERROR setting attitudes on spacecraft ......." << endl;
      cout << " .... " << be7.GetMessage() << endl;
   }
   try
   {
      csf->Initialize();
      cout << "csf is INITIIALIZED ............ woo hoo" << endl;
      sp->Initialize();
      cout << "sp is INITIIALIZED ............ woo hoo" << endl;
   }
   catch (BaseException &be6)
   {
      cout << "ERROR initializing .... " << be6.GetMessage() << endl;
   }
   cout << "Both attitude objects are now INITIALIZED ............." << endl;
   try
   {
      scCSF->SetInternalCoordSystem(mj2000);
      scSpin->SetInternalCoordSystem(mj2000);
      scCSF->Initialize();
      scSpin->Initialize();
   }
   catch (BaseException &beee)
   {
      cout << "ERROR initializing a spacecraft .... " << beee.GetMessage() << endl;
   }
      
   Real dt       = 60.0; //  / GmatTimeUtil::SECS_PER_DAY;
   Rvector q(4,0.0,0.0,0.0,1.0);
   Rvector3 omega(0.0,0.0,0.0);
   Real theTime = atTime;
   cout << "First propagate the CSFixed one .............." << endl;
   for (Integer i=0; i<=60; i++)
   {
      theTime  = atTime + (i*dt)/ GmatTimeUtil::SECS_PER_DAY;
      q        = csf->GetQuaternion(theTime);
      omega    = csf->GetAngularVelocity(theTime);
      cout << "At dt = " << (i * 60) << " seconds, q =  " << q[0] << "  " 
         << q[1] << "  " << q[2] << "  " << q[3] << endl;
      cout << "                 omega =  " << omega[0] << "  " 
         << omega[1] << "  " << omega[2] << endl;
   }
   cout << "Now propagate the Spinner one .............." << endl;
   theTime = atTime; // reset
   for (Integer i=0; i<=60; i++)
   {
      theTime  = atTime + (i*dt)/ GmatTimeUtil::SECS_PER_DAY;
      q        = sp->GetQuaternion(theTime);
      omega    = sp->GetAngularVelocity(theTime); //  * GmatMathUtil::DEG_PER_RAD;
      cout << "At dt = " << (i * 60) << " seconds, q =  " << q[0] << "  " 
         << q[1] << "  " << q[2] << "  " << q[3] << endl;
      cout << "                 omega =  " << omega[0] << "  " 
         << omega[1] << "  " << omega[2] << endl;
      
   }
   cout << "**** NOW test the calls to the spacecraft for the cosine matrix ****" << endl;
   cout << "     (then convert that to a quaternion and match) - Spinner    ****" << endl;
   cout << "     Also, get the angular velocity                             ****" << endl;
   Rmatrix33 dcm;
   Rvector3  om;
   Rvector   q2(4,0.0,0.0,0.0,1.0);
   for (Integer i=0;i<=5;i++)
   {
      theTime  = atTime + (i*dt)/ GmatTimeUtil::SECS_PER_DAY;
      dcm = scSpin->GetAttitude(theTime);
      cout << "For dt = " << (i*60) << " seconds, the matrix is: " << endl;
      cout << dcm << endl;
      q2 = Attitude::ToQuaternion(dcm);
      cout << " ... and the corresponding quaternion is " << endl;
      cout << q2 << endl;
      om = scSpin->GetAngularVelocity(theTime);
      cout << "...... and the angular velocity is " << endl;
      cout << om << endl;
   }
   
   
   cout << "=-=-=-=-=-=-= END TEST attitude conversions and propagation ....." << endl;

}
