//$Id: Troposphere.cpp 65 2010-07-22 00:10:28Z  $
//------------------------------------------------------------------------------
//                         Troposphere Model
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Tuan Dang Nguyen
// Created: 2010/07/22
//
/**
 * Troposphere media correction model.
 */
 //------------------------------------------------------------------------------
#include "Troposphere.hpp"
#include "CelestialBody.hpp"
#include "GmatConstants.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "Code500EphemerisFile.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"
#include "StringUtil.hpp"
#include "RealUtilities.hpp"

//#define DEBUG_TROPOSPHERE_CORRECTION
//#define DEBUG_TRK223
//#define DEBUG_TRK223_SOLVER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const Real Troposphere::term2 = 78.8828 / 77.624;



//------------------------------------------------------------------------------
// Troposphere(const std::string& nomme)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
 //------------------------------------------------------------------------------
Troposphere::Troposphere(const std::string& nomme) :
   MediaCorrection("Troposphere", nomme)
{
   objectTypeNames.push_back("Troposphere");
   model = 1;						// 1 for Troposphere model

   month = 0;
   epoch = 0;
   stationHeight = 0;
   mariniData.clear();
}

//------------------------------------------------------------------------------
// ~Troposphere()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
 //------------------------------------------------------------------------------
Troposphere::~Troposphere()
{
}

//------------------------------------------------------------------------------
// Troposphere(const Troposphere& tps)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
 //------------------------------------------------------------------------------
Troposphere::Troposphere(const Troposphere& tps) :
   MediaCorrection(tps)
{
   month = tps.month;
   epoch = tps.epoch;
   mariniData = tps.mariniData;
   stationHeight = tps.stationHeight;
}


//-----------------------------------------------------------------------------
// Troposphere& operator=(const Troposphere& mc)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param tps The Troposphere that is provides parameters for this one
 *
 * @return This Troposphere, configured to match tps
 */
 //-----------------------------------------------------------------------------
Troposphere& Troposphere::operator=(const Troposphere& tps)
{
   if (this != &tps)
   {
      MediaCorrection::operator=(tps);

      month = tps.month;
      epoch = tps.epoch;
      mariniData = tps.mariniData;
      stationHeight = tps.stationHeight;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone()
//------------------------------------------------------------------------------
/**
 * Clone a Troposphere object
 */
 //------------------------------------------------------------------------------
GmatBase* Troposphere::Clone() const
{
   return new Troposphere(*this);
}
//------------------------------------------------------------------------------
// bool SetStationHeight(GmatEpoch ep)
//------------------------------------------------------------------------------
/**
 * Set month based on epoch
 */
 //------------------------------------------------------------------------------
bool Troposphere::SetStationHeight(Real height)
{
   stationHeight = height;

   return true;
}

//------------------------------------------------------------------------------
// bool SetTime(GmatEpoch ep)
//------------------------------------------------------------------------------
/**
 * Set month based on epoch
 */
 //------------------------------------------------------------------------------
bool Troposphere::SetTime(GmatEpoch ep)
{
   A1Mjd epochTime = ep;
   epoch = ep;
   month = epochTime.ToA1Date().GetMonth();

   return true;
}


//------------------------------------------------------------------------------
// RealArray Correction()
//------------------------------------------------------------------------------
/** Compute refraction corrections.
*  Parameters required are determined by the Troposphere model used
*  Supported Troposphere models are HopfieldSaastamoinen and Marini
*  return double[] containing tropospheric refraction corrections for range (m)
*                             elevation (rad) measurements
*                             media correction time delay (second)
*/
//------------------------------------------------------------------------------
RealArray Troposphere::Correction()
{
   RealArray out;

   if (modelTypeName == "HopfieldSaastamoinen")
   {
      out = CalculateHS();
   }
   else if (modelTypeName == "Marini")
   {
      out = CalculateMarini();
   }
   else if (modelTypeName == "TRK-2-23")
   {
      out = CalculateTRK223();
   }
   else
   {
      MessageInterface::ShowMessage("Troposphere::Correction: Unrecognized Troposphere model " + modelTypeName + " used\n"
         "Supported models are HopfieldSaastamoinen, Marini, and TRK-2-23\n");
      throw MeasurementException("Troposphere::Correction: Unrecognized Troposphere model " + modelTypeName + " used\n"
         "Supported models are HopfieldSaastamoinen, Marini, and TRK-2-23\n");
   }

#ifdef DEBUG_TROPOSPHERE_CORRECTION
   MessageInterface::ShowMessage(" Troposphere correction result:\n");
   MessageInterface::ShowMessage("   Range correction = %f m\n", out[0]);
   MessageInterface::ShowMessage("   Elevation angle correction = %f rad", out[1]);
   MessageInterface::ShowMessage("   Time correction = %f sec\n", out[2]);
#endif

   return out;
}

//------------------------------------------------------------------------------
// 
// Real Troposphere::FractionalExpander(Real timeComponent, Real aFactor, Real bFactor, Real cFactor)
//------------------------------------------------------------------------------
/** Compute the fractional expansion for the Neil Mapping function
*  @Real timeComponent
*  @Real aFactor
*  @Real bFactor
*  @Real cFactor 
*
*  return Real expanded number
*/
//------------------------------------------------------------------------------
Real Troposphere::FractionalExpander(  Real timeComponent, Real aFactor, Real bFactor, Real cFactor)
{

   Real expanded = (1 + aFactor / (1 + bFactor / (1 + cFactor))) / (GmatMathUtil::Sin(timeComponent) + aFactor / (GmatMathUtil::Sin(timeComponent) + bFactor / (GmatMathUtil::Sin(timeComponent) + cFactor)));

   return expanded;
}

//------------------------------------------------------------------------
//Real Troposphere::GetTRK223Time(std::string TRK223TimeLine)
//------------------------------------------------------------------------
/**
 * This function is used to parse the time in Mjd from a TRK-2-23 data entry
 *
 * @param TRK223TimeLine            The data to be parsed
 *
 * return time in Mjd format as real
 */
 //------------------------------------------------------------------------

Real Troposphere::GetTRK223Time(const std::string &TRK223TimeLine)
{

   Real timeReturn;

   // Check format of line, then convert to Modified Julian
   Integer year = 0;
   GmatStringUtil::ToInteger(TRK223TimeLine.substr(0, 2), year);
   if (year >= 69 && year < 1000)
   {
      year += 1900;
   }
   else if (year < 69)
   {
      year += 2000;
   }
   Integer monthTRK = 0;
   GmatStringUtil::ToInteger(TRK223TimeLine.substr(3, 2), monthTRK);
   Integer day = 0;
   GmatStringUtil::ToInteger(TRK223TimeLine.substr(6, 2), day);
   Integer hour = 0;
   GmatStringUtil::ToInteger(TRK223TimeLine.substr(9, 2), hour);
   Integer minute = 0;
   GmatStringUtil::ToInteger(TRK223TimeLine.substr(12, 2), minute);
   Real second = 0;
   if (TRK223TimeLine.length() > 14)
   {
      GmatStringUtil::ToReal(TRK223TimeLine.substr(15), second);
   }

   timeReturn = ModifiedJulianDate(year, monthTRK, day, hour, minute, second);

#ifdef DEBUG_TRK223_FILELOADER

   MessageInterface::ShowMessage("Troposphere::GetTRK223Time(): Time in TRK-2-23 format : " + timeStrip + "\n");
   MessageInterface::ShowMessage("Troposphere::GetTRK223Time(): Time converted to Modified Julian format : " + GmatStringUtil::ToString(timeReturn) + "\n");

#endif
   
   return timeReturn;
}


//------------------------------------------------------------------------------
// 
// Real Troposphere::LinearInterpolator(Real firstEntry, Real secondEntry, Real scaleFactor)
//------------------------------------------------------------------------------
/** Compute the linear interpolation between two points given a scale factor
*  @Real firstEntry is the smaller point
*  @Real secondEntry is the larger point
*  @Real scaleFactor is the scaling factor
*
*  return Real with Linearly Interpolated value
*/
//------------------------------------------------------------------------------
Real Troposphere::LinearInterpolator( Real firstEntry, Real secondEntry, Real scaleFactor)
{

   Real interpolation = (firstEntry + scaleFactor * (secondEntry - firstEntry));

   return interpolation;
}


//------------------------------------------------------------------------------
// RealArray CalculateHS()
//------------------------------------------------------------------------------
/** Compute refraction corrections using the HopfieldSaastamoinen model.
*  p double containing pressure in hPa
*  T double containing temperature in deg K
*  fh double containing relative humidity (0 <= fh <= 1)
*  E double containing elevation angle in radians
*  rho double containing range in m
*  return double[] containing tropospheric refraction corrections for range (m)
*                             elevation (rad) measurements
*                             media correction time delay (second)
*/
//------------------------------------------------------------------------------
RealArray Troposphere::CalculateHS()
{
   // Determine Re value
   if (!solarSystem)
   {
      MessageInterface::ShowMessage("Troposphere::Correction: Solar System is NULL; Cannot obtain Earth radius\n");
      throw MeasurementException("Troposphere::Correction: Solar System is NULL; Cannot obtain Earth radius\n");
   }
   CelestialBody *earth = solarSystem->GetBody(GmatSolarSystemDefaults::EARTH_NAME);
   if (!earth)
   {
      MessageInterface::ShowMessage("Troposphere::Correction: Cannot obtain Earth radius\n");
      throw MeasurementException("Troposphere::Correction: Cannot obtain Earth radius\n");
   }
   Real Re = earth->GetEquatorialRadius()*GmatMathConstants::KM_TO_M;			// get Earth radius in meters

#ifdef DEBUG_TROPOSPHERE_CORRECTION
   MessageInterface::ShowMessage("Troposphere::Correction(): Using HopfieldSaastamoinen model\n");
   MessageInterface::ShowMessage("   temperature = %f K ,  pressure = %f hPa,  humidity = %f\n", temperature, pressure, humidityFraction);
   MessageInterface::ShowMessage("   range = %lfm ,  elevationAngle = %lf radian,  waveLength = %lfm\n", range, elevationAngle, waveLength);
   MessageInterface::ShowMessage("   earth radius = %lf m\n", Re);
#endif

   // Specify Ce and Crho:
   double lambda = waveLength;
   double lp2_inv = 1.0 / ((lambda * 1.0E+06)*(lambda * 1.0E+06));
   double denom = (173.3 - lp2_inv);
   double term1 = 170.2649 / denom;
   double Ce = term1 * term2;
   double term3 = (173.3 + lp2_inv) / denom;
   double Crho = Ce * term3;

#ifdef DEBUG_TROPOSPHERE_CORRECTION
   MessageInterface::ShowMessage("   Ce = %lf ,  Crho = %lf\n", Ce, Crho);
#endif

   // Specify inputs:
   double p = pressure;
   double T = temperature;
   double fh = humidityFraction;
   double E = elevationAngle;
   double rho = range;

   // refractivities
   double N[2];
   // compute dry component refractivity
   N[0] = 77.624 * p / T;

   // compute wet component refractivity
   double Tc = T + GmatPhysicalConstants::ABSOLUTE_ZERO_C;
   double e = 6.10 * fh * exp(17.15 * Tc / (234.7 + Tc));
   N[1] = 371900.0 * e / (T*T) - 12.92 * e / T;


   // troposphere heights
   double h[2];
   // compute dry troposphere height
   h[0] = 5.0 * 0.002277 * p / (N[0] * 1.0E-06);

   // compute wet troposphere height
   h[1] = 5.0 * 0.002277 * e * (1255.0 / T + 0.05) / (N[1] * 1.0E-06);

   // distances to top of the troposphere
   double r[2];
   double alpha[9][2];
   double beta[7][2];
   double cosE = cos(E);
   double cosE2 = cosE * cosE;
   double sinE = sin(E);

   for (int j = 0; j < 2; j++)
   {
      r[j] = sqrt((Re + h[j])*(Re + h[j]) - (Re*Re*cosE2)) - Re * sinE;
      double aj = -1.0 * sinE / h[j];
      double bj = -1.0 * cosE2 / (2.0 * h[j] * Re);
      alpha[0][j] = 1.0;
      alpha[1][j] = 4.0*aj;
      alpha[2][j] = 6.0*aj*aj + 4.0*bj;
      alpha[3][j] = 4.0*aj*(aj*aj + 3.0*bj);
      alpha[4][j] = pow(aj, 4) + 12.0*aj*aj*bj + 6.0*bj*bj;
      alpha[5][j] = 4.0*aj*bj*(aj*aj + 3.0*bj);
      alpha[6][j] = bj * bj*(6.0*aj*aj + 4.0*bj);
      alpha[7][j] = 4.0 * aj * bj*bj*bj;
      alpha[8][j] = pow(bj, 4);
      beta[0][j] = 1.0;
      beta[1][j] = 3.0*aj;
      beta[2][j] = 3.0*(aj*aj + bj);
      beta[3][j] = aj * (aj*aj + 6.0*bj);
      beta[4][j] = 3.0*bj*(aj*aj + bj);
      beta[5][j] = 3.0 * aj * bj*bj;
      beta[6][j] = pow(bj, 3);
   }

   double drho = 0.0;
   double dE = 0.0;
   for (int j = 0; j < 2; j++)
   {
      double sum1 = 0.0;
      for (int i = 0; i < 9; i++)
      {
         double ii = (double)i;
         double temp1 = alpha[i][j] * pow(r[j], (i + 1)) / (ii + 1.0);
         sum1 = sum1 + temp1;
      }
      double sum2 = 0.0;
      for (int k = 0; k < 7; k++)
      {
         double kk = (double)k;
         double temp2 = beta[k][j] * pow(r[j], (k + 2)) / ((kk + 1.0)*(kk + 2.0)) + beta[k][j] * pow(r[j], (k + 1))*(rho - r[j]) / (kk + 1);
         sum2 = sum2 + temp2;
      }
      drho = drho + N[j] * 1.0E-06 * sum1;
      dE = dE + N[j] * 1.0E-06 * sum2 / h[j];
   }
   drho = Crho * drho;
   dE = Ce * 4.0 * cosE * dE / rho;                          // unit: radian

   RealArray out;
   out.push_back(drho);
   out.push_back(dE);
   out.push_back(drho / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);

   return out;
}


//------------------------------------------------------------------------------
// RealArray CalculateMarini()
//------------------------------------------------------------------------------
/** Compute refraction corrections using the Marini model.
*  Based on GTDS TROPOA.F
*
*  latitude double containing Ground Station latitude in rad
*  longitude double containing Ground Station longitude in rad
*  month integer containing month of the year
*  rho double containing range in m
*  E double containing elevation angle in radians
*  return double[] containing tropospheric refraction corrections for range (m)
*                             elevation (rad) measurements
*                             media correction time delay (second)
*/
//------------------------------------------------------------------------------
RealArray Troposphere::CalculateMarini()
{
#ifdef DEBUG_TROPOSPHERE_CORRECTION
   MessageInterface::ShowMessage("Troposphere::Correction(): Using Marini model\n");
   MessageInterface::ShowMessage("   latitude = %f deg ,  longitude = %f deg, month %d\n", latitude*GmatMathConstants::DEG_PER_RAD, longitude*GmatMathConstants::DEG_PER_RAD, month);
   MessageInterface::ShowMessage("   range = %lf m ,  elevationAngle = %lf radian\n", range, elevationAngle);
#endif

   if (mariniData.size() == 0)
   {
      LoadMariniDataFile();
   }

   // Specify inputs:
   double LATITUDE = latitude;
   double LONGITUDE = longitude;
   Integer MONTH = month - 1; // January = 0
   double ELEVATION = elevationAngle;
   double RGN = range;

   // Specify intermediate variables:
   Integer NS;
   Real HT;
   Real RHO, RS, P, Q, SINEA, COSEA, XIO, XI1, XII1, XII2;
   Real XKO, XMO, XM1, XMM1, XMM2, I, L, M;

   // Specify outputs:
   Real drho, dE;

   /*      SUBROUTINE TROPOA

   C  PURPOSE:  TO COMPUTE CORRECTIONS DUE TO THE TROPOSPHERE

   C  REFERENCE:  "CLOSED FORM SATELLITE TRACKING DATA CORRECTIONS FOR
   C        AN ARBITRARY TROPOSPHERIC PROFILE",JOHN W. MARINI,MARCH 1971,
   C         GSFC,X-551-71-122.                      */

   //C     GET THE MONTHLY MEAN VALUE OF REFRACTIVITY AND SCALE HEIGHT
   TROGET(LATITUDE, LONGITUDE, MONTH, NS, HT);

   //C     SLANT RANGE
   RHO = RGN;

   //C     SOME EQUATORIAL RADIUS
   RS = 6369.96;

   //C     Eq 7-197a p7-83;
   //P = DSQRT(2.0 * HT / RS);
   P = GmatMathUtil::Sqrt(2.0 * HT / RS);

   //C     Eq 7-197b p7-84
   //Q = 1.0D-6 * NS * RS / HT;
   Q = 1.0E-6 * NS * RS / HT;

   //C     SIN AND COS OF ELEVATION
   //SINEA = DSIN(ELEVATION);
   //COSEA = DCOS(ELEVATION);
   SINEA = GmatMathUtil::Sin(ELEVATION);
   COSEA = GmatMathUtil::Cos(ELEVATION);

   //C     Eq 7-203c p7-85
   //XIO = SQRT(PI) / (1.0 - 0.9206 * Q) ** 0.4468
   XIO = GmatMathUtil::Sqrt(GmatMathConstants::PI) / GmatMathUtil::Pow(1.0 - 0.9206 * Q, 0.4468);

   //C     Eq 7-203d p7-86
   XI1 = 2.0 / (1.0 - Q);

   //C     Eq 7-203a p7-85
   //XII = 0.5 - 0.25 * Q;
   XII1 = 0.5 - 0.25 * Q;

   //C     Eq 7-203b p7-85
   //XII2 = 0.75 - 0.5625 * Q + 0.125 * Q**2;
   XII2 = 0.75 - 0.5625 * Q + 0.125 * Q*Q;

   //C     Eq 7-205 p7-86
   //XKO = SQRT(2 * PI) / (1.0 - 0.9408 * Q) ** 0.4759;
   XKO = GmatMathUtil::Sqrt(2 * GmatMathConstants::PI) / GmatMathUtil::Pow(1.0 - 0.9408 * Q, 0.4759);

   //C     Eq 7-204c p7-86
   //XMO = XIO * (1.0 + Q + Q**2 * XIO**2 / 12.0) - 0.5 * Q * XKO;
   XMO = XIO * (1.0 + Q + Q * Q * XIO*XIO / 12.0) - 0.5 * Q * XKO;

   //C     Eq 7-204d p7-86
   XM1 = (2.0 + 0.5 * Q * XIO * XIO) / (1.0 - Q);

   //C     Eq 7-204a p7-86
   XMM1 = 0.5 - 0.375* Q;

   //C     Eq 7-204b p7-86
   //XMM2 = 0.75 * (1.0 - 25.0 / 24.0 * Q + 11.0 / 36.0 * Q**2);
   XMM2 = 0.75 * (1.0 - 25.0 / 24.0 * Q + 11.0 / 36.0 * Q*Q);

   //C     Eq 7-200a p7-84 WHERE F = Eq 7-201 p7-85
   I = BendingIntegral(SINEA, XII1, XII2, XIO, XI1, P);

   //C     Eq 7-199 p7-84
   //L = 1.D0 - I * SINEA + 0.5D-6 * NS * I**2;
   L = 1.0 - I * SINEA + 0.5E-6 * NS * I*I;

   //C     Eq 7-200b p7-84 WHERE F = Eq 7-201 p7-85
   M = BendingIntegral(SINEA, XMM1, XMM2, XMO, XM1, P);

   //C     Range correction in km  = Eq 7-198a p7-84
   //DRANGE = 1.D - 6 * NS * HT * (M - 0.5D - 6 * NS * (RS * COSEA * L) ** 2 / (RHO * HT))
   drho = 1.0E-6 * NS * HT * (M - 0.5E-6 * NS * GmatMathUtil::Pow(RS * COSEA * L, 2) / (RHO * HT));

   dE = 1.0E-6 * NS * COSEA * (I - RS * L / RHO);         // unit: radian

   drho *= GmatMathConstants::KM_TO_M;

   RealArray out;
   out.push_back(drho);
   out.push_back(dE);
   out.push_back(drho / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);

   return out;
}

//------------------------------------------------------------------------
//RealArray Troposphere::CalculateTRK223()
//------------------------------------------------------------------------
/**
 * This function is used to calculate troposphere correction.
 *
 *
 * return Ionosphere correction vector                 (units: (m, rad, s))
 */
 //------------------------------------------------------------------------

RealArray Troposphere::CalculateTRK223()
{
#ifdef DEBUG_TRK223
   MessageInterface::ShowMessage("Troposphere::CalculateTRK223(): Beginning TRK-2-23 Model Calculation \n FilePath names:\n");
   for (UnsignedInt i = 0; i < solarSystem->GetBody("Earth")->GetStringArrayParameter("DSNMediaFileDirectories").size(); ++i)
   {
      MessageInterface::ShowMessage("%i -   %s\n", i, solarSystem->GetBody("Earth")->GetStringArrayParameter("DSNMediaFileDirectories")[i].c_str());
   }
   
#endif
   
   // Determine which file is the seasonal correction and which is the delta correction
   std::string seasonal = "";
   std::string delta = "";

   std::string DSNComplexName = "";

   // Assigns Complex Name if individual station is specified
   // Included to allow common abbreviations in GMAT to convert to DSN format

   if (groundStationId == "GDS")
   {
      groundStationId = "DSN(C10)";
      DSNComplexName = "DSN(C10)";
   }
   else if (groundStationId == "CAN")
   {
      groundStationId = "DSN(C40)";
      DSNComplexName = "DSN(C40)";
   }
   else if (groundStationId == "MAD")
   {
      groundStationId = "DSN(C60)";
      DSNComplexName = "DSN(C60)";
   }

   Integer stationNumber;
   if (groundStationId.substr(0, 1) == "C")
   {
      GmatStringUtil::ToInteger(groundStationId.substr(1), stationNumber);
   }
   else
   {
      GmatStringUtil::ToInteger(groundStationId, stationNumber);
   }
   if (groundStationId.length() < 3)
   {
      groundStationId = "DSN(0" + groundStationId + ")";
   }
   else
   {
      groundStationId = "DSN(" + groundStationId + ")";
   }

   if (stationNumber < 30)
   {
      DSNComplexName = "DSN(C10)";
   }
   else if (stationNumber < 50 && stationNumber >= 30)
   {
      DSNComplexName = "DSN(C40)";
   }
   else if (stationNumber >= 50)
   {
      DSNComplexName = "DSN(C60)";
   }
   

   Integer seasonalLines[4] = { -1, -1, -1, -1 };
   Integer deltaLines[4] = { -1, -1, -1, -1 };

   // Find the Entries that correspond to the measurement time
   for (UnsignedInt i = 0; i < DSNDatabase.size(); ++i)
   {
      if (DSNDatabase[i][7] == "NONE")
      {
         if (DSNDatabase[i][0] == "ALL")
         {
            if (DSNDatabase[i][6] == DSNComplexName)
            {
               if (DSNDatabase[i][4] == "72/01/01,00:00")
               {
                  if (DSNDatabase[i][3] == "DRY NUPART")
                  {
                     seasonalLines[0] = i;
                  }
                  else if (DSNDatabase[i][3] == "WET NUPART")
                  {
                     seasonalLines[1] = i;
                  }
               }
               else if (GetTRK223Time(DSNDatabase[i][4]) <= epoch && GetTRK223Time(DSNDatabase[i][5]) >= epoch)
               {
                  if (DSNDatabase[i][3] == "DRY NUPART")
                  {
                     deltaLines[0] = i;
                  }
                  else if (DSNDatabase[i][3] == "WET NUPART")
                  {
                     deltaLines[1] = i;
                  }
               }
            }
            else if (DSNDatabase[i][6] == groundStationId)
            {
               if (DSNDatabase[i][4] == "72/01/01,00:00")
               {
                  if (DSNDatabase[i][3] == "DRY NUPART")
                  {
                     seasonalLines[2] = i;
                  }
                  else if (DSNDatabase[i][3] == "WET NUPART")
                  {
                     seasonalLines[3] = i;
                  }
               }
               else if (GetTRK223Time(DSNDatabase[i][4]) <= epoch && GetTRK223Time(DSNDatabase[i][5]) >= epoch)
               {
                  if (DSNDatabase[i][3] == "DRY NUPART")
                  {
                     deltaLines[2] = i;
                  }
                  else if (DSNDatabase[i][3] == "WET NUPART")
                  {
                     deltaLines[3] = i;
                  }
               }
            }
         }
      }
   }
     
   // Feed the lines into the calculator

   Real dryCorrection = 0;
   Real wetCorrection = 0;
   // Solve correction for the DSN Complex, adding delta if provided
   
   // Load the seasonal Data file and extract the relevent lines as strings
   if (seasonalLines[0] != -1)
   {
      dryCorrection = TRK223Solver(DSNDatabase[seasonalLines[0]]);
      wetCorrection = TRK223Solver(DSNDatabase[seasonalLines[1]]);
   }
      else
   {
	  throw MeasurementException("Error locating seasonal correction data for the groundstation \"" + groundStationId+"\". Ensure that the seasonal.csp file, with corrections for this groundstation, is located in one of the Earth.DSNAtmosphericDirectories. \n");
   }

   // Load the delta file and extract the relevent lines as strings

   if (delta != "")
   {
     // deltaLines = LoadTRK223DataFile(delta);
   }

   if (deltaLines[0] != -1)
   {
      dryCorrection += TRK223Solver(DSNDatabase[deltaLines[0]]);
      wetCorrection += TRK223Solver(DSNDatabase[deltaLines[1]]);
   }

   //Solve for the individual station, if specified
   if (seasonalLines[2] != -1)
   {
      dryCorrection += TRK223Solver(DSNDatabase[seasonalLines[2]]);
   }
   if (seasonalLines[3] != -1)
   {
      wetCorrection += TRK223Solver(DSNDatabase[seasonalLines[3]]);
   }
   if (deltaLines[2] != -1)
   {
      dryCorrection += TRK223Solver(DSNDatabase[deltaLines[2]]);
   }
   if (deltaLines[3] != -1)
   {
      wetCorrection += TRK223Solver(DSNDatabase[deltaLines[3]]);
   }

   // Apply Neils Mapping Function to the Corrections

   Real latitudeDegrees = GmatMathUtil::RadToDeg(latitude);
      
   // Define the Latitude Table
   Real latTable[5] = { 15.0, 30.0, 45.0, 60.0, 75.0 };

   // Set up Dry Table Coefficients

   Real aDryAvg[5] = { 1.2769934e-3, 1.2683230e-3, 1.2465397e-3, 1.2196049e-3, 1.2045996e-3 };
   Real bDryAvg[5] = { 2.9153695e-3, 2.9152299e-3, 2.9288445e-3, 2.9022565e-3, 2.9024912e-3 };
   Real cDryAvg[5] = { 62.610505e-3, 62.837393e-3, 63.721774e-3, 63.824265e-3, 64.258455e-3 };

   Real aDryAmp[5] = { 0.0, 1.2709626e-5, 2.6523662e-5, 3.4000452e-5, 4.1202191e-5 };
   Real bDryAmp[5] = { 0.0, 2.1414979e-5, 3.0160779e-5, 7.2562722e-5, 11.723375e-5 };
   Real cDryAmp[5] = { 0.0, 9.0128400e-5, 4.3497037e-5, 84.795348e-5, 170.37206e-5 };

   Real aHeightCorrection = 2.53e-5;
   Real bHeightCorrection = 5.49e-3;
   Real cHeightCorrection = 1.14e-3;

   // Set up Wet Table Coefficients

   Real aWetAvg[5] = { 5.8021897e-4, 5.6794847e-4, 5.8118019e-4, 5.9727542e-4, 6.1641693e-4 };
   Real bWetAvg[5] = { 1.4275268e-3, 1.5138625e-3, 1.4572752e-3, 1.5007428e-3, 1.7599082e-3 };
   Real cWetAvg[5] = { 4.3472961e-2, 4.6729510e-2, 4.3908931e-2, 4.4626982e-2, 5.4736038e-2 };

   Real interpolationScale = 0;
   Integer interpolationIndex = 0;

   Real aDryInterpolated = 1;
   Real bDryInterpolated = 1;
   Real cDryInterpolated = 1;

   Real aWetInterpolated = 1;
   Real bWetInterpolated = 1;
   Real cWetInterpolated = 1;
   Real neilTime = A1Mjd(epoch).ToA1Date().ToDayOfYear();
   neilTime = GmatMathConstants::TWO_PI*(neilTime - 28) / (365.25);

   latitudeDegrees = GmatMathUtil::Abs(latitudeDegrees);

   if (latitudeDegrees < 15)
   {
      interpolationScale = 0;

      aDryInterpolated = aDryAvg[0] + aDryAmp[0] * GmatMathUtil::Cos(neilTime);
      aDryInterpolated = bDryAvg[0] + bDryAmp[0] * GmatMathUtil::Cos(neilTime);
      aDryInterpolated = cDryAvg[0] + cDryAmp[0] * GmatMathUtil::Cos(neilTime);

      aDryInterpolated = aWetAvg[0];
      aDryInterpolated = bWetAvg[0];
      aDryInterpolated = cWetAvg[0];
   }
   else if (latitudeDegrees > 75)
   {
      interpolationScale = 0;

      aDryInterpolated = aDryAvg[4] + aDryAmp[4] * GmatMathUtil::Cos(neilTime);
      aDryInterpolated = bDryAvg[4] + bDryAmp[4] * GmatMathUtil::Cos(neilTime);
      aDryInterpolated = cDryAvg[4] + cDryAmp[4] * GmatMathUtil::Cos(neilTime);

      aDryInterpolated = aWetAvg[4];
      aDryInterpolated = bWetAvg[4];
      aDryInterpolated = cWetAvg[4];
   }
   else
   {
      for (Integer i = 1; i < 4; i++)
      {
         if (latitudeDegrees < latTable[i])
         {
            interpolationScale = (latitudeDegrees - latTable[i]) / (latTable[i - 1] - latTable[i]);
            interpolationIndex = i - 1;
         }
      }

      aDryInterpolated = LinearInterpolator(aDryAvg[interpolationIndex], aDryAvg[interpolationIndex + 1], interpolationScale) + LinearInterpolator(aDryAmp[interpolationIndex], aDryAmp[interpolationIndex + 1], interpolationScale)*GmatMathUtil::Cos(neilTime);
      bDryInterpolated = LinearInterpolator(bDryAvg[interpolationIndex], bDryAvg[interpolationIndex + 1], interpolationScale) + LinearInterpolator(bDryAmp[interpolationIndex], bDryAmp[interpolationIndex + 1], interpolationScale)*GmatMathUtil::Cos(neilTime);
      cDryInterpolated = LinearInterpolator(cDryAvg[interpolationIndex], cDryAvg[interpolationIndex + 1], interpolationScale) + LinearInterpolator(cDryAmp[interpolationIndex], cDryAmp[interpolationIndex + 1], interpolationScale)*GmatMathUtil::Cos(neilTime);

      aWetInterpolated = LinearInterpolator(aWetAvg[interpolationIndex], aWetAvg[interpolationIndex + 1], interpolationScale);
      bWetInterpolated = LinearInterpolator(bWetAvg[interpolationIndex], bWetAvg[interpolationIndex + 1], interpolationScale);
      cWetInterpolated = LinearInterpolator(cWetAvg[interpolationIndex], cWetAvg[interpolationIndex + 1], interpolationScale);

   }

   Real mDry = FractionalExpander(elevationAngle, aDryInterpolated, bDryInterpolated, cDryInterpolated);
   Real mDryDelta = ((1 / GmatMathUtil::Sin(elevationAngle)) - FractionalExpander(elevationAngle, aHeightCorrection, bHeightCorrection, cHeightCorrection))*stationHeight;
   Real dryScaleFactor = mDry + mDryDelta;
   Real wetScaleFactor = FractionalExpander(elevationAngle, aWetInterpolated, bWetInterpolated, cWetInterpolated);

   dryCorrection = dryCorrection * (mDry + mDryDelta);
   wetCorrection = wetCorrection * wetScaleFactor;

   // Specify outputs:
   Real drho, dE;

   // Lanyi Elevation Angle Correction Due to Atmospheric Effects (DSN Moyer 9.3.2.2, pg 9-21)

   // 1 Calculate the Mean height of Dry and Wet Air

   Real hDry = 0.86*8.567*(temperature / 292)*GmatMathUtil::Exp10(3);  // Units: m
   Real hWet = 2.4*GmatMathUtil::Exp10(3);                           // Units: m

   // 2 Calculate the Water vapor pressure and the dry surface pressure

   Real pWet = 6.11*humidityFraction*GmatMathUtil::Exp((17.27*(temperature - 273.16)) / (237.3 + (temperature - 273.16)));   // Units: mBar
   Real pDry = pressure - pWet;                                                                                           // Units: mBar

   // 3 Calculate the dry and water vapor surface refractivity, then total refractivity

   Real refractDry = 77.6*GmatMathUtil::Exp10(-6)*pDry / temperature;
   Real refractWet = ((377.6*GmatMathUtil::Exp10(3)) / temperature + 64.8)*GmatMathUtil::Exp10(-6)*pWet / temperature;
   Real refractTotal = refractDry + refractWet;

   // 4 Caculate the Dry and Wet zenith delay

   Real zenithDry = 0.22768*pDry*GmatMathUtil::Exp10(-2);   // Units: m
   Real zenithWet = refractWet * hWet;                        // Units: m

   // 5 Calculate the function a(y), where y is the unrefracted elevation angle of the spacecraft or a quasar
   Real radiusEarth = 6.378*GmatMathUtil::Exp10(6);         // Units: m
   Real aElvation = ((zenithDry / GmatMathUtil::Pow((1 - GmatMathUtil::Pow((GmatMathUtil::Cos(elevationAngle) / (1 + hDry / radiusEarth)), 2)), 3 / 2))
      + (zenithWet / GmatMathUtil::Pow((1 - GmatMathUtil::Pow((GmatMathUtil::Cos(elevationAngle) / (1 + hWet / radiusEarth)), 2)), 3 / 2)))
      *  (GmatMathUtil::Sin(elevationAngle) / radiusEarth);

   // 6 Calculate the intermediate F(x) function
   
   Real functionX = 1 / (1 + 0.5*(GmatMathUtil::Sqrt(1 + 2 * ((refractTotal - aElvation) / GmatMathUtil::Pow(GmatMathUtil::Tan(elevationAngle), 2))) - 1));

   // 7 Calculate the refraction correction

   Real angleCorrection = ((refractTotal - aElvation) / GmatMathUtil::Tan(elevationAngle))*functionX; // Units: rad

   drho = dryCorrection + wetCorrection;
   dE = angleCorrection;         // unit: radian

#ifdef DEBUG_TRK223
   MessageInterface::ShowMessage("Troposphere::CalculateTRK223(): The total correction at time %f is %f m.\n",epoch,drho);
   

#endif
   RealArray out;
   out.push_back(drho);
   out.push_back(dE);
   out.push_back(drho / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);

   return out;
}

//------------------------------------------------------------------------
//Real Troposphere::TRK223Solver(std::string TRK223Line)
//------------------------------------------------------------------------
/**
 * This function is used to solve the calculation for the correction
 *
 * @param TRK223Line                The data to be parsed
 *
 * return Tropospheric range correction                           (unit: m)
 */
 //------------------------------------------------------------------------

Real Troposphere::TRK223Solver(const StringArray &TRK223Line)
{
#ifdef DEBUG_TRK223_SOLVER

   MessageInterface::ShowMessage("Troposphere::TRK223Solver(): Beginning TRK-2-23 Solver With the elements: \n");
   for (UnsignedInt i = 0; i < TRK223Line.size(); ++i)
   {
      std::string messageTRK = TRK223Line[i];
      MessageInterface::ShowMessage("%i -   %s\n", i, messageTRK);
   }


#endif
   // Extract the model type from the line

   std::string modelType = TRK223Line[1];

   // Strip the line of the Coefficients
   RealArray coefs = GmatStringUtil::ToRealArray(TRK223Line[2]);

   Real epochStart = GetTRK223Time(TRK223Line[4]);
   Real epochEnd = GetTRK223Time(TRK223Line[5]);

#ifdef DEBUG_TRK223_SOLVER
   MessageInterface::ShowMessage("Troposphere::TRK223Solver():  Correction Type is: " + modelType + "\n");
   MessageInterface::ShowMessage("Troposphere::TRK223Solver():  Epoch Start is " + GmatStringUtil::RealToString(epochStart) + "\n");
   MessageInterface::ShowMessage("Troposphere::TRK223Solver():  Epoch  End  is " + GmatStringUtil::RealToString(epochEnd) + "\n");
   MessageInterface::ShowMessage("Troposphere::TRK223Solver():  Cofficients are: (\n");

   for (UnsignedInt i = 0; i < coefs.size(); ++i)
   {
      MessageInterface::ShowMessage(GmatStringUtil::RealToString(coefs[i]) + ", ");
   }
   MessageInterface::ShowMessage(")\n");

#endif

   // Determine if the correction information is for a constant, trig, or power series calculation
   Real drho = 0;

   if (modelType == "CONST")
   {
      drho = coefs[0];
   }
   else if (modelType == "TRIG")
   {
      Real Time = epoch * 86400;
      Real spanStart = epochStart * 86400;
      Time = GmatMathConstants::TWO_PI*(Time - spanStart) / coefs[0];                //Test variable
      drho = coefs[1];
      Integer count = 1;
      for (Integer i = 2; i < coefs.size(); i++)
      {
         drho += coefs[i] * GmatMathUtil::Cos(Time*count) + coefs[i + 1] * GmatMathUtil::Sin(Time*count);
         count++;
         i++;
      }
   }
   else if (modelType == "NRMPOW")
   {
      Real Time = epoch * 86400;
      Real spanStart = epochStart * 86400;
      Real spanEnd = epochEnd * 86400;
      Real tS = Time - spanStart;
      Real eS = spanEnd - spanStart;

      Time = 2 * ((Time - spanStart) / (spanEnd - spanStart)) - 1;                //Test variable
      for (Integer i = 0; i < coefs.size(); i++)
      {
         drho += coefs[i] * GmatMathUtil::Pow(Time, i);
      }
   }
   else
   {
      throw MeasurementException("Error: Do not recognize time model " + modelType + " at time " + TimeSystemConverter::Instance()->ConvertMjdToGregorian(epoch));
   }

#ifdef DEBUG_TRK223_SOLVER
   MessageInterface::ShowMessage("Troposphere::TRK223Solver():  Correction is: " + GmatStringUtil::ToString(drho) + " m\n");
#endif

   return drho;
}
//------------------------------------------------------------------------------
// void Troposphere::TROGET(Real FLATD, Real FLOND, Integer MON, Integer &NS, Real &HT)
//------------------------------------------------------------------------------
/** Load the troposphere data from a data file for the Marini model.
*  Based on GTDS TROGET.F
*
*  FLATD double containing Ground Station latitude in rad
*  FLOND double containing Ground Station longitude in rad
*  MON integer containing month of the year January = 0
*  NS integer output containing the refractivity
*  HT double output containing the scale height
*/
//------------------------------------------------------------------------------
void Troposphere::TROGET(Real FLATD, Real FLOND, Integer MON, Integer &NS, Real &HT)
{
   Integer NZHMON = 37068;

   for (UnsignedInt ii = 0U; ii < mariniData.size(); ii++)
   {
      if (abs(FLATD*GmatMathConstants::DEG_PER_RAD - mariniData[ii].latitude) < 1.0)
      {
         Real DLON = abs(FLOND*GmatMathConstants::DEG_PER_RAD - mariniData[ii].longitude);

         if (DLON < 1.0 || (360.0 - DLON) < 1.0)
         {
            NZHMON = mariniData[ii].refractivity[MON];
            break;
         }
      }
   }

   NS = NZHMON / 100;
   HT = Real(GmatMathUtil::Mod(NZHMON, 100)) * 1.E-1;
}


//------------------------------------------------------------------------------
// void Troposphere::BendingIntegral(Real ALPHA, Real FF1_Tropo, Real FF2_Tropo,
//                                   Real FO, Real F1, Real P)
//------------------------------------------------------------------------------
/** A FUNCTION WHICH APPROXIMATES THE BENDING INTEGRAL, I(ALPHA)
*  Based on GTDS F.F
*  NOTE: 'FF1_Tropo' and 'FF2_Tropo' are necessary because FF1 is an OS X
*        Macro and the code will not compile with original argument name 'FF1'
*/
//------------------------------------------------------------------------------
Real Troposphere::BendingIntegral(Real ALPHA, Real FF1_Tropo, Real FF2_Tropo,
   Real FO, Real F1, Real P)
{
   Real PSQ, X1, X2, X3, X4, F;
   Real Q1 = 1.0;

   PSQ = P * P;
   X1 = FF1_Tropo * PSQ;
   X2 = FF2_Tropo * PSQ / FF1_Tropo - X1;
   X3 = FO * FO * FF1_Tropo * (Q1 + X1 / X2) - (Q1 + F1 * FF1_Tropo);
   X4 = FO * X1 / X3 / P * 1.21313;
   X3 = X2 / X3 * 1.320903;
   X2 = X2 * 1.08885;
   F = X2 / (ALPHA + X3 / (ALPHA + X4));
   F = Q1 / (ALPHA + X1 / (ALPHA + F));

   return F;
}


//------------------------------------------------------------------------------
// void LoadMariniDataFile()
//------------------------------------------------------------------------------
/** Load the data file containing refractivity data used by the Marini model
*/
//------------------------------------------------------------------------------
void Troposphere::LoadMariniDataFile()
{
   FileManager *fm = FileManager::Instance();
   std::string filename = fm->GetFullPathname(FileManager::MARINI_TROPO_FILE);

   std::ifstream tropoFile(filename.c_str());
   if (!tropoFile)
      throw MeasurementException("Error opening troposphere data file \"" + filename + "\"\n");

   std::string theLine;

   MariniDataStruct mariniLine;
   mariniData.clear();

   while (!tropoFile.eof())
   {
      std::getline(tropoFile, theLine);

      std::stringstream ss;
      ss.str(theLine);
      ss >> mariniLine.latitude >> mariniLine.longitude;

      for (UnsignedInt jj = 0U; jj < 12; jj++)
      {
         ss >> mariniLine.refractivity[jj];
      }

      mariniData.push_back(mariniLine);
   }

   if (mariniData.size() == 0)
      throw MeasurementException("Unable to load refractivity data from troposphere data file \"" + filename + "\"\n");
}