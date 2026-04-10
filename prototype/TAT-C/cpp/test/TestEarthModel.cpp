//$Id$
//------------------------------------------------------------------------------
//                               TestEarthModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2016.05.11
//
/**
 * Unit-test driver for the Earth model
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "AbsoluteDate.hpp"
#include "OrbitState.hpp"
#include "Earth.hpp"
#include "TimeTypes.hpp"
#include "BodyFixedStateConverter.hpp" // for text of Earth::Convert


using namespace std;
using namespace GmatMathConstants;

//------------------------------------------------------------------------------
// Real vectorError (Rvector3 &v1 Rvector3 &v2)
//------------------------------------------------------------------------------
Real vectorError (Rvector3 &v1, Rvector3 &v2)
{
    Rvector3 diff = v1 - v2;
    return diff*diff;
}

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string outFormat = "%16.9f ";
   Real        tolerance = 1e-15;
   
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   MessageInterface::ShowMessage("%s\n",
                                 GmatTimeUtil::FormatCurrentTime().c_str());
   
   // Set global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);
   
   char *buffer = NULL;
   buffer = getenv("OS");
   if (buffer  != NULL)
   {
      MessageInterface::ShowMessage("Current OS is %s\n", buffer);
   }
   else
   {
      MessageInterface::ShowMessage("Buffer is NULL, should tell you OS environment\n");
   }
   
   MessageInterface::ShowMessage("*** START TESTS of Earth Model ***\n");
    
   
   try
   {
      // Test the AbsoluteDate
      // create the earth and test parameters to match values in default constructor
      Earth earth;
      Real radius = 6.3781363e+03;
      Real flattening = 0.0033527;
      
      // test GMT caluculation
      MessageInterface::ShowMessage("*** Starting ComputeGMT tests ***\n");
      Real GMT = earth.ComputeGMT(2457260.12345679);
      MessageInterface::ShowMessage("Calculated GMT = %12.10f\n", GMT);
      
      //  Need to verify the sign. (compares hi-fi output from GMAT/STK with
      //  low-fi model in TAT-C, hence the loose tolerance
      if (GmatMathUtil::Abs(GMT*180/PI - 198.002628503035)/198.002628503035 >= 1e-5)
         MessageInterface::ShowMessage("*** ERROR - error calculating GMT!!\n");
      else
         MessageInterface::ShowMessage("OK - GMT calculation is OK!\n");
      
      //cout << endl;
      //cout << "Hit enter to end" << endl;
      //cin.get();
      //---------------------------------------------
      
      // test conversion between ellipsoidal, spherical, and cartesian representations
      MessageInterface::ShowMessage("*** Starting Convert tests ***\n");
      
      Real error;
      bool convertPassed = true;
      // initial conditions based on location of SXM, 200 km height
      Real latitude = -63*PI/180;   // radians
      Real longitude = 18*PI/180;   // radians
      Real height=200;              // km
      
      // spherical to cartesian test
      Rvector3 origSpherical (latitude,longitude,height);
      Rvector3 testVector = earth.Convert(origSpherical, "Spherical", "Cartesian");
      Rvector3 truthVector = BodyFixedStateConverterUtil::Convert (origSpherical,"Spherical","Cartesian",
                                                                   flattening,radius);
      error = vectorError(testVector,truthVector);
      MessageInterface::ShowMessage("Spherical to Cartesian error = %12.10f\n",error);
      if (error>1.0e-10) convertPassed = false;
      
      //cartesian to spherical test
      Rvector3 newSpherical = earth.Convert(truthVector,"Cartesian","Spherical");
      error = vectorError(newSpherical,origSpherical);
      MessageInterface::ShowMessage("Cartesian to Spherical error = %12.10f\n",error);
      if (error>1.0e-10) convertPassed = false;
      
      //ellipsoid to spherical test
      Rvector3 origEllipsoid (latitude,longitude, height);
      testVector = earth.Convert(origEllipsoid,"Ellipsoid","Spherical");
      truthVector = BodyFixedStateConverterUtil::Convert(origEllipsoid,"Ellipsoid","Spherical",
                                                         flattening,radius);
      error = vectorError(testVector,truthVector);
      MessageInterface::ShowMessage("Ellipsoid to Spherical error = %12.10f\n",error);
      if (error>1.0e-10) convertPassed = false;
      
      //spherical to ellipsoid test
      Rvector3 newEllipsoid = earth.Convert(truthVector, "Spherical","Ellipsoid");
      error = vectorError(newEllipsoid,origEllipsoid);
      MessageInterface::ShowMessage("Spherical to Ellipsoid error = %12.10f\n",error);
      if (error>1.0e-10) convertPassed = false;
      
      //cartesian to ellipsoid test
      Rvector3 origCartesian(7000.0,100.0, 100.0);
      testVector = earth.Convert(origCartesian,"Cartesian","Ellipsoid");
      truthVector = BodyFixedStateConverterUtil::Convert(origCartesian,"Cartesian","Ellipsoid",
                                                         flattening,radius);
      error = vectorError(testVector,truthVector);
      MessageInterface::ShowMessage("Cartesian to Ellipsoid error = %12.10f\n",error);
      if (error>1.0e-10) convertPassed = false;
      
      // ellipsoid to cartesian test
      Rvector3 newCartesian = earth.Convert(truthVector,"Ellipsoid","Cartesian");
      error = vectorError(newCartesian,origCartesian);
      MessageInterface::ShowMessage("Ellipsoid to Cartesian error = %12.10f\n",error);
      if (error>1.0e-10) convertPassed = false;

	  // inertial to ellipsoid test
	  Real jd = 2457769.43773277;
	  Rvector3 inertialVector(
		  -2436.063522947054, 2436.063522947055, 5967.112612227063);
	  Rvector3 ellipsoidTruth(1.04987919204, 0.730506078412, 528.147942517);
	  Rvector3 newEllips = earth.InertialToBodyFixed(inertialVector, jd, "Ellipsoid");
	  error = vectorError(ellipsoidTruth, newEllips);
	  MessageInterface::ShowMessage("Inertial to Ellipsoid error  =  %12.10f\n", error);
	  if (error>1.0e-10) convertPassed = false;

	  // sun location test
	  Rvector3 sunEllipsoid = earth.GetSunPositionInBodyCoords(jd, "Ellipsoid");
	  Rvector3 sunTruth(-0.3656482498, 3.5748963692, 147151685.1403646800);
      error = vectorError(sunEllipsoid, sunTruth);
	  MessageInterface::ShowMessage("Sun w/r/t         Ellipsoid  =  %12.10f\n", error);
	  if (error>1.0e-10) convertPassed = false;
      
      //wrap up Convert test
      if (convertPassed)
         MessageInterface::ShowMessage("OK -- Geodetic conversion passes\n");
      else
         MessageInterface::ShowMessage("Geodetic conversion test fails\n ");
      
      cout << endl;
      cout << "Hit enter to end" << endl;
      cin.get();
      //----------------------------------------------
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n", be.GetFullMessage().c_str());
   }
   
    MessageInterface::ShowMessage("*** END TESTS of Earth Model ***\n");
}
