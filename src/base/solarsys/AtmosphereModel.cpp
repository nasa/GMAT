//$Id$
//------------------------------------------------------------------------------
//                              AtmosphereModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/21
//
/**
 * Base class for the atmosphere models
 */
//------------------------------------------------------------------------------

#include "AtmosphereModel.hpp"
#include "MessageInterface.hpp"
#include "CelestialBody.hpp"        // To retrieve radius, flattening factor
#include "SolarSystemException.hpp"
#include "CoordinateSystem.hpp"
#include "GmatConstants.hpp"
#include "AngleUtil.hpp"            // For lat, long range setting
#include "CoordinateConverter.hpp"
#include "StringUtil.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"


#include <cmath>                    // for exp


//#define CHECK_KP2AP
//#define DEBUG_ANGVEL
//#define DEBUG_COORDINATE_TRANSFORMS
//#define DEBUG_CALCULATE_GEOCENTRICS
//#define DEBUG_CALCULATE_GEODETICS
//#define DEBUG_FLUX_FILE
//#define DEBUG_SCHATTEN_SETTINGS
//#define DUMP_FLUX_DATA

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string
AtmosphereModel::PARAMETER_TEXT[AtmosphereModelParamCount-GmatBaseParamCount] =
{
   "F107",
   "F107A",
   "MagneticIndex",                  // In GMAT, the "published" value is K_p.
   "CSSISpaceWeatherFile",
   "SchattenFile"
};

const Gmat::ParameterType
AtmosphereModel::PARAMETER_TYPE[AtmosphereModelParamCount-GmatBaseParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE
};

//------------------------------------------------------------------------------
//  AtmosphereModel(const std::string &typeStr, const std::string &name = "")
//------------------------------------------------------------------------------
/**
 *  Default Constructor.
 *
 *  @param <typeStr> The type of the derived atmosphere model.
 *  @param <name>    The name of the derived atmosphere model.
 */
//------------------------------------------------------------------------------
AtmosphereModel::AtmosphereModel(const std::string &typeStr, const std::string &name) :
   GmatBase             (Gmat::ATMOSPHERE, typeStr, name),
   fluxReader           (NULL),
   solarSystem          (NULL),
   mCentralBody         (NULL),
   CSSIFileName          (""),        // Set to a default when working
   schattenFileName      (""),
   sunVector            (NULL),
   centralBody          ("Earth"),
   centralBodyLocation  (NULL),
   cbRadius             (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]),
   cbFlattening         (0.0),       // Default is spherical
   fluxReaderLoaded     (false),
   constantF107         (150.0),
   constantF107a        (150.0),
   constantKp           (3.0),
   nominalF107          (150.0),
   nominalF107a         (150.0),
   nominalKp            (3.0),
   kpApConversion       (0),
   historicalDataSource (0),
   predictedDataSource  (0),
   mInternalCoordSystem (NULL),
   cbJ2000              (NULL),
   canDeleteCBJ2000CS   (false),
   cbFixed              (NULL),
   wUpdateInterval      (0.0),     // Default to always update
   wUpdateEpoch         (0.0),
   geoHeight            (0.0),
   geoLat               (0.0),
   geoLong              (0.0),
//   useGeodetic          (false),
   useGeodetic          (true),
   gha                  (0.0),
   ghaEpoch             (0.0),
   historicStart        (-1.0),
   historicEnd          (-1.0),
   predictStart         (-1.0),
   predictEnd           (-1.0),
   schattenTimingModel  (0),
   schattenErrorModel   (0),
   sod                  (0.0),
   yd                   (0),
   f107                 (0.0),
   f107a                (0.0)
{
   objectTypes.push_back(Gmat::ATMOSPHERE);
   objectTypeNames.push_back("AtmosphereModel");

   parameterCount = AtmosphereModelParamCount;
   nominalAp = ConvertKpToAp(nominalKp);
   constantAp = nominalAp;

   // Default to nominal Earth angular velocity
   angVel[0]      = 0.0;
   angVel[1]      = 0.0;
   angVel[2]      = 7.29211585530e-5;

   theTimeConverter = TimeSystemConverter::Instance();

   #ifdef CHECK_KP2AP
      MessageInterface::ShowMessage("K_p to A_p conversions:\n");
      for (Integer i = 0; i < 28; ++i)
         MessageInterface::ShowMessage("   %12.9lf -> %12.9lf\n", i/3.0,
            ConvertKpToAp(i/3.0));
   #endif
}

//------------------------------------------------------------------------------
//  ~AtmosphereModel()
//------------------------------------------------------------------------------
/**
 *  Destructor.
 */
//------------------------------------------------------------------------------
AtmosphereModel::~AtmosphereModel()
{
   if(fluxReader != NULL)
   {
      delete fluxReader;
      fluxReader = NULL;
   }

   if (cbJ2000)
      if (canDeleteCBJ2000CS)
         delete cbJ2000;
}

//------------------------------------------------------------------------------
//  AtmosphereModel(const AtmosphereModel& am)
//------------------------------------------------------------------------------
/**
 *  Copy constructor.
 *
 *  @param <am> The atmosphere model that is copied.
 */
//------------------------------------------------------------------------------
AtmosphereModel::AtmosphereModel(const AtmosphereModel& am) :
   GmatBase             (am),
   fluxReader           (NULL),
   solarSystem          (am.solarSystem),
   mCentralBody         (am.mCentralBody),
   CSSIFileName          (am.CSSIFileName),
   schattenFileName      (am.schattenFileName),
   sunVector            (NULL),
   centralBody          (am.centralBody),
   centralBodyLocation  (NULL),
   cbRadius             (am.cbRadius),
   cbFlattening         (am.cbFlattening),
   fluxReaderLoaded     (false),
   constantF107         (am.constantF107),
   constantF107a        (am.constantF107a),
   constantKp           (am.constantKp),
   nominalF107          (am.nominalF107),
   nominalF107a         (am.nominalF107a),
   nominalKp            (am.nominalKp),
   kpApConversion       (am.kpApConversion),
   historicalDataSource (am.historicalDataSource),
   predictedDataSource  (am.predictedDataSource),
   mInternalCoordSystem (am.mInternalCoordSystem),
   cbJ2000              (NULL),
   canDeleteCBJ2000CS   (false),
   cbFixed              (am.cbFixed),
   wUpdateInterval      (am.wUpdateInterval),
   wUpdateEpoch         (am.wUpdateEpoch),
   geoHeight            (0.0),
   geoLat               (0.0),
   geoLong              (0.0),
   useGeodetic          (am.useGeodetic),
   gha                  (0.0),
   ghaEpoch             (0.0),
   historicStart        (am.historicStart),
   historicEnd          (am.historicEnd),
   predictStart         (am.predictStart),
   predictEnd           (am.predictEnd),
   schattenTimingModel  (am.schattenTimingModel),
   schattenErrorModel   (am.schattenErrorModel),
   sod                  (am.sod),
   yd                   (am.yd),
   f107                 (am.f107),
   f107a                (am.f107a)
{
   parameterCount = AtmosphereModelParamCount;
   nominalAp = ConvertKpToAp(nominalKp);
   constantAp = nominalAp;
   theTimeConverter = TimeSystemConverter::Instance();

   isInitialized = false;
}

//------------------------------------------------------------------------------
// AtmosphereModel& operator=(const AtmosphereModel& am)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param <am> AtmosphereModel instance used as a template for this copy.
 * 
 * @return A reference to this class, with members set to match the template.
 */
//------------------------------------------------------------------------------
AtmosphereModel& AtmosphereModel::operator=(const AtmosphereModel& am)
{
   if (this == &am)
      return *this;
        
   GmatBase::operator=(am);
   
   solarSystem          = am.solarSystem;
   mCentralBody         = am.mCentralBody;
   CSSIFileName          = am.CSSIFileName;
   schattenFileName      = am.schattenFileName;
   sunVector            = NULL;
   centralBody          = am.centralBody;
   centralBodyLocation  = NULL;
   fluxReader           = NULL;
   cbRadius             = am.cbRadius;
   cbFlattening         = am.cbFlattening;
   fluxReaderLoaded     = false;
   constantF107         = am.constantF107;
   constantF107a        = am.constantF107a;
   constantKp           = am.constantKp;
   nominalF107          = am.nominalF107;
   nominalF107a         = am.nominalF107a;
   nominalKp            = am.nominalKp;
   nominalAp            = ConvertKpToAp(nominalKp);
   constantAp           = nominalAp;
   kpApConversion       = am.kpApConversion;
   historicalDataSource = am.historicalDataSource;
   predictedDataSource  = am.predictedDataSource;
   mInternalCoordSystem = am.mInternalCoordSystem;
   if (cbJ2000 && canDeleteCBJ2000CS)
      delete cbJ2000;
   cbJ2000              = NULL;
   canDeleteCBJ2000CS   = false;
   cbFixed              = am.cbFixed;
   wUpdateInterval      = am.wUpdateInterval;
   wUpdateEpoch         = am.wUpdateEpoch;
   geoHeight            = 0.0;
   geoLat               = 0.0;
   geoLong              = 0.0;
   useGeodetic          = am.useGeodetic;
   gha                  = 0.0;
   ghaEpoch             = 0.0;
   historicStart        = am.historicStart;
   historicEnd          = am.historicEnd;
   predictStart         = am.predictStart;
   predictEnd           = am.predictEnd;
   schattenTimingModel  = am.schattenTimingModel;
   schattenErrorModel   = am.schattenErrorModel;

   sod                  = am.sod;
   yd                   = am.yd;
   f107                 = am.f107;
   f107a                = am.f107a;

   for (Integer i = 0; i < 7; i++)
      ap[i] = am.ap[i];

   isInitialized = false;

   return *this;
}


//------------------------------------------------------------------------------
// bool AtmosphereModel::Initialize()
//------------------------------------------------------------------------------
/**
 * Initialize()
 * 
 * @param None
 * 
 * @return true
 */
//------------------------------------------------------------------------------

bool AtmosphereModel::Initialize()
{
   if (mCentralBody == NULL)
   {
      if (centralBody != "")
      {
         if (solarSystem)
            SetCentralBody(centralBody);
         else
            throw AtmosphereException("Error: SolarSystem is not set to '" + GetName() + "' atmosphere model object before it runs initialization step\n");
      }
   }
   else
   {
      if (centralBody == "")
      {
         centralBody = mCentralBody->GetName();
      }
      else if (centralBody != mCentralBody->GetName())
      {
         if (solarSystem)
            SetCentralBody(centralBody);
         else
            throw AtmosphereException("Error: SolarSystem is not set to '" +
                  GetName() + "' atmosphere model object before it runs initialization step\n");
      }
   }


   if (fluxReader == NULL)
   {
      fluxReader = new SolarFluxReader();
      fluxReader->SetHistoricDataSource(historicalDataSource);
      fluxReader->SetPredictedDataSource(predictedDataSource);
      fluxReader->SetConstValues(constantF107, constantF107a, constantKp, constantAp);
   }

   return true;
}


//------------------------------------------------------------------------------
//  void SetSunVector(Real *sv)
//------------------------------------------------------------------------------
/**
 *  Sets the position vector for the Sun.
 * 
 * @param <sv> The Sun vector.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSunVector(Real *sv)
{
   sunVector = sv;
}

//------------------------------------------------------------------------------
//  void SetCentralBodyVector(Real *cv)
//------------------------------------------------------------------------------
/**
 *  Sets the position vector for the body with the atmosphere.
 * 
 *  @param <cv> The body's position vector.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetCentralBodyVector(Real *cv)
{
   centralBodyLocation = cv;
}

//------------------------------------------------------------------------------
// void SetUpdateParameters(Real interval, GmatEpoch epoch)
//------------------------------------------------------------------------------
/**
 * Sets the update interval and, if selected, the epoch to apply
 *
 * @param interval The update interval, in days
 * @param epoch    An update epoch
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetUpdateParameters(Real interval, GmatEpoch epoch)
{
   wUpdateInterval = interval;
   if ((epoch >= 0.0) && (cbFixed != NULL))
      UpdateAngularVelocity(epoch);
}


//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets the internal coordinate system used to construct the angular momentum
 * vector
 *
 * @param cs The coordinate system
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetInternalCoordSystem(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}

//------------------------------------------------------------------------------
// void SetCbJ2000CoordinateSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets the body centered J2000 coordinate system used to calculate geodetics 
 * when the CB is not the Earth.
 *
 * @param cs The coordinate system
 * @param canDelete Flag indicating ownership of the J2000 coordinate system
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetCbJ2000CoordinateSystem(CoordinateSystem *cs, bool canDelete)
{
   canDeleteCBJ2000CS = canDelete;
   cbJ2000 = cs;
}


// made changes  for bug GMT-5282
//------------------------------------------------------------------------------
// CoordinateSystem* GetCbJ2000CoordinateSystem()
//------------------------------------------------------------------------------
/**
 * Gets the body centered J2000 coordinate system.
 */
//------------------------------------------------------------------------------
CoordinateSystem* AtmosphereModel::GetCbJ2000CoordinateSystem()
{
   return cbJ2000;
}


//------------------------------------------------------------------------------
// void SetFixedCoordinateSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets the body fixed coordinate system used to construct the angular momentum
 * vector
 *
 * @param cs The coordinate system
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetFixedCoordinateSystem(CoordinateSystem *cs)
{
   if (!cs->AreAxesOfType("BodyFixedAxes"))
      throw SolarSystemException(
            "AtmosphereModel: coordinate system is not of type BodyFixed.\n");
   cbFixed = cs;
}

//------------------------------------------------------------------------------
// Real* GetAngularVelocity(const GmatEpoch when)
//------------------------------------------------------------------------------
/**
 * Retrieves the angular momentum vector, optionally at a specified epoch
 *
 * @note This method return the pointer to a data member.  If that pointer is
 * stored, subsequent updates will change the data pointed to so that the user
 * need not refresh this data pointer.
 *
 * @param when The epoch for the desired vector.  If not specified or less than
 * 0.0, the last calculated vector is returned.
 *
 * @return A pointer to the angular momentum vector
 */
//------------------------------------------------------------------------------
Real* AtmosphereModel::GetAngularVelocity(const GmatEpoch when)
{
   if (when >= 0.0)
      UpdateAngularVelocity(when);
   return angVel;
}


//------------------------------------------------------------------------------
// void BuildAngularVelocity(const GmatEpoch when)
//------------------------------------------------------------------------------
/**
 * Constructs the angular velocity vector at the specified epoch.
 *
 * @param when The epoch
 */
//------------------------------------------------------------------------------
void AtmosphereModel::BuildAngularVelocity(const GmatEpoch when)
{
    Real in[3];
    Rmatrix33 rotMat = cbFixed->GetLastRotationMatrix();
    Rmatrix33 rotDotMat = cbFixed->GetLastRotationDotMatrix();

    // Build angVel from R' * Rdot
    in[0] = rotMat(0,2)*rotDotMat(0,1) + rotMat(1,2)*rotDotMat(1,1) +
          rotMat(2,2)*rotDotMat(2,1);
    in[1] = rotMat(0,0)*rotDotMat(0,2) + rotMat(1,0)*rotDotMat(1,2) +
          rotMat(2,0)*rotDotMat(2,2);
    in[2] = rotMat(0,1)*rotDotMat(0,0) + rotMat(1,1)*rotDotMat(1,0) +
          rotMat(2,1)*rotDotMat(2,0);

    // Rotate into the J2000 frame
    angVel[0] = rotMat(0,0)*in[0] + rotMat(0,1)*in[1] + rotMat(0,2)*in[2];
    angVel[1] = rotMat(1,0)*in[0] + rotMat(1,1)*in[1] + rotMat(1,2)*in[2];
    angVel[2] = rotMat(2,0)*in[0] + rotMat(2,1)*in[1] + rotMat(2,2)*in[2];

    wUpdateEpoch = when;

    #ifdef DEBUG_ANGVEL
       MessageInterface::ShowMessage("Rotation Matrix:\n");
       for (Integer m = 0; m < 3; ++m)
       {
          MessageInterface::ShowMessage("   [");
          for (Integer n = 0; n < 3; ++n)
             MessageInterface::ShowMessage(" %.14lf ",rotMat(m,n));
          if (m < 2)
             MessageInterface::ShowMessage(";\n");
          else
             MessageInterface::ShowMessage("]\n");
       }

       MessageInterface::ShowMessage("Rotation Dot Matrix:\n");
       for (Integer m = 0; m < 3; ++m)
       {
          MessageInterface::ShowMessage("   [");
          for (Integer n = 0; n < 3; ++n)
             MessageInterface::ShowMessage(" %.14lf ",rotDotMat(m,n));
          if (m < 2)
             MessageInterface::ShowMessage(";\n");
          else
             MessageInterface::ShowMessage("]\n");
       }

       MessageInterface::ShowMessage("AtmosphereModel::"
             "UpdateAngularVelocity(%.12lf) -> [%.12le %.12le %.12le]\n",
             when, angVel[0], angVel[1], angVel[2]);
    #endif
}


//------------------------------------------------------------------------------
// void UpdateAngularVelocity(const GmatEpoch when)
//------------------------------------------------------------------------------
/**
 * Updates the angular momentum vector based on the input epoch.
 *
 * Updates are only performed if the update epoch is outside of the update
 * interval period.
 *
 * @param when The epoch (in a.1 ModJulian format) of the desired update.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::UpdateAngularVelocity(const GmatEpoch when)
{
   if (wUpdateInterval >= 0.0)
   {
      if (fabs(when - wUpdateEpoch) > wUpdateInterval)
      {
         if (cbFixed == NULL)
            throw AtmosphereException("The body-fixed coordinate system is "
                  "not set");
         Real in[3], out[3];
         cbFixed->ToBaseSystem(A1Mjd(when), in, out, true, true);  // @todo - do we need ToMJ2000Eq here?
         BuildAngularVelocity(when);
      }
   }
}


//------------------------------------------------------------------------------
// void SetKpApConversionMethod(Integer method)
//------------------------------------------------------------------------------
/**
 * Selects the Kp -> Ap conversion method
 *
 * @param <method>  Method to use (see next method prolog)
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetKpApConversionMethod(Integer method)
{
   kpApConversion = method;
   nominalAp = ConvertKpToAp(nominalKp);
}


//------------------------------------------------------------------------------
// Real ConvertKpToAp(const Real kp)
//------------------------------------------------------------------------------
/**
 * Routine to convert Kp values into Ap values
 *
 * This method provides several different Kp->Ap, conversion methods.  The
 * method used is selected by the value of kpApConversion.  The implemented
 * approaches are as follows:
 *
 *   kpApConversion = 0: Table lookup using table 8-3 from Vallado,  3rd edition
 *
 *   kpApConversion = 1: Exponential approximation from Vallado 2nd Edition,
 *   (eq 8-31).  Ap is calculated using
 *
 *       \f[a_p = \exp\left(\frac{k_p + 1.6} {1.75}\right)\f]
 *
 *   kpApConversion = 2: A secant method that solves the transcendental equation
 *
 *       \f[28 K_p + 0.03 e^{K_p} = A_p + 100 (1 - e^{-0.08 A_p})\f]
 *
 *   using an iterative secant method.
 *
 * The default method is the table lookup
 *
 * @param <kp> The geomagnetic index, Kp.
 *
 * @return the corresponding geomagnetic amplitude, Ap.
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::ConvertKpToAp(const Real kp)
{
   Real ap = 15.0;

   switch (kpApConversion)
   {
      case 0:  // Table lookup
         {
            Integer index = (Integer)((kp + .01) * 3);
            switch (index)
            {
               case 0:
                  ap = 0.0;
                  break;
               case 1:
                  ap = 2.0;
                  break;
               case 2:
                  ap = 3.0;
                  break;
               case 3:
                  ap = 4.0;
                  break;
               case 4:
                  ap = 5.0;
                  break;
               case 5:
                  ap = 6.0;
                  break;
               case 6:
                  ap = 7.0;
                  break;
               case 7:
                  ap = 9.0;
                  break;
               case 8:
                  ap = 12.0;
                  break;
               /// case 9 is the default, below
               case 10:
                  ap = 18.0;
                  break;
               case 11:
                  ap = 22.0;
                  break;
               case 12:
                  ap = 27.0;
                  break;
               case 13:
                  ap = 32.0;
                  break;
               case 14:
                  ap = 39.0;
                  break;
               case 15:
                  ap = 48.0;
                  break;
               case 16:
                  ap = 56.0;
                  break;
               case 17:
                  ap = 67.0;
                  break;
               case 18:
                  ap = 80.0;
                  break;
               case 19:
                  ap = 94.0;
                  break;
               case 20:
                  ap = 111.0;
                  break;
               case 21:
                  ap = 132.0;
                  break;
               case 22:
                  ap = 154.0;
                  break;
               case 23:
                  ap = 179.0;
                  break;
               case 24:
                  ap = 207.0;
                  break;
               case 25:
                  ap = 236.0;
                  break;
               case 26:
                  ap = 300.0;
                  break;
               case 27:
                  ap = 400.0;
                  break;
               case 9:
               default:
                  ap = 15.0;
                  break;
            }
         }
         break;

      case 1:  // Vallado, 2nd edition
         ap = exp((kp + 1.6) / 1.75);
         break;

      case 2:
      default:
      {
         Real r = 28.0 * kp + 0.03 * exp(kp) - 100.0;

         Real x[3], y[2];
         x[0] = 0.0;
         x[1] = 500.0;

         Real epsilon = 1.0e-6;
         Integer maxIterations = 15, i = 0;

         do
         {
            y[0] = 100.0 * exp(-0.08 * x[0]) + r - x[0];
            y[1] = 100.0 * exp(-0.08 * x[1]) + r - x[1];

            x[2] = x[1] - y[1] * (x[1] - x[0]) / (y[1] - y[0]);
            x[0] = x[1];
            x[1] = x[2];

            if (i++ > maxIterations)
               throw AtmosphereException("ConvertKpToAp failed; too "
                     "many iterations");
         } while (fabs(y[1]) > epsilon);

         ap = x[2];
      }
   }
   return ap;
}


//------------------------------------------------------------------------------
// void AtmosphereModel::SetInputSource(const std::string &historical,
//                                      const std::string &predicted)
//------------------------------------------------------------------------------
/**
 * Sets teh sources for Flux and Geomagnetic indices
 *
 * @param historical Toggles between constants and CSSI data, using the strings
 *                   "ConstantFluxAndGeoMag" and "CSSISpaceWeatherFile"
 * @param predicted Toggles between constants, CSSI, and Schatten data, using
 *                  the strings "ConstantFluxAndGeoMag", "CSSISpaceWeatherFile",
 *                  and "SchattenFile"
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetInputSource(const std::string &historical,
                                     const std::string &predicted)
{
   if (historical == "ConstantFluxAndGeoMag")
      historicalDataSource = 0;
   else if (historical == "CSSISpaceWeatherFile")
      historicalDataSource = 1;
   else
      throw AtmosphereException("Invalid historical data source " + historical +
            " selected");

   if (predicted == "ConstantFluxAndGeoMag")
      predictedDataSource = 0;
   else if (predicted == "CSSISpaceWeatherFile")
      predictedDataSource = 1;
   else if (predicted == "SchattenFile")
      predictedDataSource = 2;
   else
      throw AtmosphereException("Invalid predicted data source " + predicted +
            " selected");

   fluxReader->SetHistoricDataSource(historicalDataSource);
   fluxReader->SetPredictedDataSource(predictedDataSource);

}

//------------------------------------------------------------------------------
// void SetSchattenFlags(const std::string &timing, const std::string &magnitude)
//------------------------------------------------------------------------------
/**
 * Pass-through function for the Schatten predict model selection
 *
 * @param timing The Schatten timing model
 * @param magnitude The Schatten error model
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSchattenFlags(const std::string &timing,
                            const std::string &magnitude)
{
   #ifdef DEBUG_SCHATTEN_SETTINGS
      MessageInterface::ShowMessage("AtmosphereModel::SetSchattenFlags"
            "(%s, %s)\n", timing.c_str(), magnitude.c_str());
   #endif

   if (timing == "EarlyCycle")
      schattenTimingModel = -1;
   if (timing == "NominalCycle")
      schattenTimingModel = 0;
   if (timing == "LateCycle")
      schattenTimingModel = 1;

   if (magnitude == "MinusTwoSigma")
      schattenErrorModel = -1;
   if (magnitude == "Nominal")
      schattenErrorModel = 0;
   if (magnitude == "PlusTwoSigma")
      schattenErrorModel = 1;

   if (fluxReader != NULL)
      fluxReader->SetSchattenFlags(schattenTimingModel, schattenErrorModel);
}


//-----------------------------------------------------------------------------
// bool AtmosphereModel::HasWindModel()
//-----------------------------------------------------------------------------
/**
 * Checks to see if a local wind model exists for the atmosphere.
 *
 * @return true if the model supports wind calculation, false if not
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::HasWindModel()
{
   return false;
}

//-----------------------------------------------------------------------------
// bool Wind(Real *position, Real* wind, Real ep, Integer count = 1)
//-----------------------------------------------------------------------------
/**
 * Retrieves the local wind in MJ2000Eq coordinates.
 *
 * @param position The location of the spacecraft
 * @param wind The local wind, as set by the method
 * @param ep The epoch of the calculations
 * @param count The number of spacecraft contained in pos
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::Wind(Real *position, Real* wind, Real ep,
						   Integer count)
{
   return false;
}

//-----------------------------------------------------------------------------
// bool HasTemperatureModel()
//-----------------------------------------------------------------------------
/**
 * Checks to see if a local temperature exists for the atmosphere.
 *
 * @return true if the model supports temperature calculation, false if not
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::HasTemperatureModel()
{
   return false;
}

//-----------------------------------------------------------------------------
// bool Temperature(Real *position, Real *temperature, Real epoch, 
//                  Integer count)
//-----------------------------------------------------------------------------
/**
 * Retrieves the temperature.
 *
 * @param position The location of the spacecraft
 * @param temperature The temperature, as set by the method
 * @param ep The epoch of the calculations
 * @param count The number of spacecraft contained in position
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::Temperature(Real *position, Real *temperature, 
                                  Real epoch, Integer count)
{
   return false;
}

//-----------------------------------------------------------------------------
// bool HasPressureModel()
//-----------------------------------------------------------------------------
/**
 * Checks to see if a local pressure exists for the atmosphere.
 *
 * @return true if the model supports pressure calculation, false if not
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::HasPressureModel()
{
   return false;
}

//-----------------------------------------------------------------------------
// bool Pressure(Real *position, Real *pressure, Real epoch, Integer count)
//-----------------------------------------------------------------------------
/**
 * Retrieves the pressure.
 *
 * @param position The location of the spacecraft
 * @param pressure The pressure, as set by the method
 * @param ep The epoch of the calculations
 * @param count The number of spacecraft contained in position
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::Pressure(Real *position, Real *pressure, 
                               Real epoch, Integer count)
{
   return false;
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 * 
 * @param <ss> Pointer to the solar system used in the modeling.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}

//------------------------------------------------------------------------------
// void SetCentralBody(CelestialBody *cb)
//------------------------------------------------------------------------------
/**
 * Sets the central body pointer
 *
 * @param <cb> Pointer to the central body used in the modeling.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetCentralBody(CelestialBody *cb)
{
   if (mCentralBody != cb)
   {
      mCentralBody = cb;
      cbRadius     = mCentralBody->GetEquatorialRadius();
      cbFlattening = mCentralBody->GetFlattening();

      centralBody = mCentralBody->GetName();

      #ifdef DEBUG_CB_PROPERTIES
         MessageInterface::ShowMessage("Body set to %s; radius = %.12lf, "
               "flattening = %.12lf\n", mCentralBody->GetName().c_str(),
               cbRadius, cbFlattening);
      #endif
   }
}


//------------------------------------------------------------------------------
// void SetCentralBody(std::string cbName)
//------------------------------------------------------------------------------
/**
* Sets the central body pointer
*
* @param <cb> Pointer to the central body used in the modeling.
*/
//------------------------------------------------------------------------------
void AtmosphereModel::SetCentralBody(std::string cbName)
{
   if (centralBody != cbName)
   {
      centralBody = cbName;
      mCentralBody = NULL;
      if (solarSystem)
      {
         mCentralBody = solarSystem->GetBody(cbName);
         cbRadius = mCentralBody->GetEquatorialRadius();
         cbFlattening = mCentralBody->GetFlattening();
      }

#ifdef DEBUG_CB_PROPERTIES
      MessageInterface::ShowMessage("Body set to %s; radius = %.12lf, "
         "flattening = %.12lf\n", mCentralBody->GetName().c_str(),
         cbRadius, cbFlattening);
#endif
   }
}



//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool AtmosphereModel::IsParameterReadOnly(const Integer id) const
{
   // Since these parameters are handled in the DragForce, make them
   // read only 
   if ((id == NOMINAL_FLUX) || (id == NOMINAL_AVERAGE_FLUX) ||
       (id == NOMINAL_MAGNETIC_INDEX) || (id == SCHATTEN_WEATHER_FILE) ||
       (id == CSSI_WEATHER_FILE) )
//      return true;
      return false;
   
   return GmatBase::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool AtmosphereModel::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string AtmosphereModel::GetParameterText(const Integer id) const
{
   if ((id >= GmatBaseParamCount) && (id < AtmosphereModelParamCount))
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer AtmosphereModel::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < AtmosphereModelParamCount; ++i)
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType AtmosphereModel::GetParameterType(const Integer id) const
{
   if ((id >= GmatBaseParamCount) && (id < AtmosphereModelParamCount))
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string AtmosphereModel::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::GetRealParameter(const Integer id) const
{
   if (id == NOMINAL_FLUX)
      return constantF107;
   if (id == NOMINAL_AVERAGE_FLUX)
      return constantF107a;
   if (id == NOMINAL_MAGNETIC_INDEX)
      return constantKp;

   return GmatBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * This method traps entries for F107, F107A, and the Magnetic index and ensures
 * that these parameters have positive values.
 *
 * @param <id>    ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::SetRealParameter(const Integer id, const Real value)
{
   if (id == NOMINAL_FLUX)
   {
      if (value > 0.0)
      {
         constantF107 = value;
         nominalF107  = value;
      }
      return constantF107;
   }

   if (id == NOMINAL_AVERAGE_FLUX)
   {
      if (value > 0.0)
      {
         constantF107a = value;
         nominalF107a  = value;
      }
      return constantF107a;
   }

   if (id == NOMINAL_MAGNETIC_INDEX)
   {
      if (value > 0.0)
      {
         constantKp = value;
         nominalKp  = value;
         nominalAp  = ConvertKpToAp(nominalKp);
         constantAp = nominalAp;
      }
      return constantKp;
   }
   
   /// @todo Throw exceptions when the values are unphysical here.

   return GmatBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Call through method since derived classes need it (e.g. MarsGRAM)
 *
 * @param id The parameter ID
 * @param index Index of the requested real number
 *
 * @return The array based real parameter requested.
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::GetRealParameter(const Integer id,
      const Integer index) const
{
   return GmatBase::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Call through method since derived classes need it (e.g. MarsGRAM)
 *
 * @param id The parameter ID
 * @param value The new parameter value
 * @param index Index of the requested real number
 *
 * @return The new array based real parameter requested.
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::SetRealParameter(const Integer id,
                                     const Real value,
                                     const Integer index)
{
   return GmatBase::SetRealParameter(id, value, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets string based values on the model
 *
 * @param id The ID of the parameter being set
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool AtmosphereModel::SetStringParameter(const Integer id,
      const std::string &value)
{
   if (id == CSSI_WEATHER_FILE)
   {
      if (value != "")
      {
         bool headerFound = false;
         bool startFound = false;
         bool fileIsValid = false;

         // If the file is a SCHATTEN file, we require 3 tags:
         // "DATATYPE CSSISPACEWEATHER", "BEGIN OBSERVED", and "END OBSERVED"
         std::string searchFor = "DATATYPE CSSISPACEWEATHER";
         std::string line;

         // Does it exist?
         // Set the file names, possibly with path prefixes
         FileManager *fm = FileManager::Instance();

         std::string weatherfile = value;
         if (fm->DoesFileExist(weatherfile) == false)
            weatherfile = fm->GetAbsPathname("ATMOSPHERE_PATH") + weatherfile;
         if (fm->DoesFileExist(weatherfile) == false)
            throw SolarSystemException("Cannot open the observed space weather file " +
                  value + ", nor the file at the location " + weatherfile);

         std::ifstream inStream(weatherfile.c_str());

         // Is is a known format?
         while (!inStream.eof() && !fileIsValid)
         {
            GmatFileUtil::GetLine(&inStream, line);
            
            // if the line is blank, skip it
            if (GmatStringUtil::IsBlank(line, true)) continue;

            // Make upper case, so we can check for certain keyword
            line = GmatStringUtil::ToUpper(line);
            if (std::string(line).find(searchFor) != std::string::npos)
            {
               if (!headerFound)
               {
                  headerFound = true;
                  searchFor = "BEGIN OBSERVED";
               }
               if (headerFound && !startFound)
               {
                  startFound = true;
                  searchFor = "END OBSERVED";
               }
               if (headerFound && startFound)
               {
                  fileIsValid = true;
               }
            }
         }
         inStream.close();

         if (fileIsValid)
            CSSIFileName = weatherfile;
         else
            throw SolarSystemException("Observed space weather measurement "
                  "file \"" + value + "\" is in an unknown format");
      }
      return true;
   }

   if (id == SCHATTEN_WEATHER_FILE)
   {
      if (value != "")
      {
         bool startFound = false;
         bool fileIsValid = false;

         // If the file is a SCHATTEN file, we require 3 tags:
         // "PREDICTED SOLAR DATA", "BEGIN_DATA", and "END_DATA"
         std::string searchFor = "BEGIN_DATA";
         std::string line;

         // Does it exist?
         FileManager *fm = FileManager::Instance();

         std::string weatherfile = value;
         if (fm->DoesFileExist(weatherfile) == false)
            weatherfile = fm->GetAbsPathname("ATMOSPHERE_PATH") + weatherfile;
         if (fm->DoesFileExist(weatherfile) == false)
            throw SolarSystemException("Cannot open the predicted space weather file " +
                  value + ", nor the file at the location " + weatherfile);

         std::ifstream inStream(weatherfile.c_str());

         // Is is a known format?
         while (!inStream.eof() && !fileIsValid)
         {
            GmatFileUtil::GetLine(&inStream, line);
            
            // if the line is blank, skip it
            if (GmatStringUtil::IsBlank(line, true)) continue;

            // Make upper case, so we can check for certain keyword
            line = GmatStringUtil::ToUpper(line);
            if (std::string(line).find(searchFor) != std::string::npos)
            {
                if (!startFound)
                {
                    startFound = true;
                    searchFor = "END_DATA";
                    continue;
                }
                if (startFound)
                {
                    fileIsValid = true;
                }
            }
         }
         inStream.close();

         if (fileIsValid)
            schattenFileName = weatherfile;
         else
            throw SolarSystemException("Predicted space weather measurement "
                  "file \"" + value + "\" is in an unknown format");
      }
      return true;
   }

   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string based value
 *
 * @param id The ID for the requested parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string AtmosphereModel::GetStringParameter(const Integer id) const
{
   if (id == CSSI_WEATHER_FILE)
      return CSSIFileName;

   if (id == SCHATTEN_WEATHER_FILE)
      return schattenFileName;

   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string based parameter using the script label
 *
 * @param label The text label for the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string AtmosphereModel::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets string based values on the model
 *
 * @param label The text label for the parameter
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool AtmosphereModel::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// SolarFluxReader* GetFluxReader()
//------------------------------------------------------------------------------
/**
 * Accessor for the flux reader for atmosphere data Parameters
 *
 * @return The flux reader pointer
 */
//------------------------------------------------------------------------------
SolarFluxReader* AtmosphereModel::GetFluxReader()
{
   return fluxReader;
}


//------------------------------------------------------------------------------
// SolarFluxReader::FluxData GetFluxData(GmatEpoch epoch = -1.0);
//------------------------------------------------------------------------------
/**
 * Retrieves the flux data record for the input epoch
 *
 * @param epoch The epoch for teh data,  The default, -1.0, retrieves the
 *              current record
 *
 * @return The internal fDbuffer record
 */
//------------------------------------------------------------------------------
SolarFluxReader::FluxData AtmosphereModel::GetFluxData(GmatEpoch epoch)
{
   if (epoch > 0.0)
      GetInputs(epoch);
   return fDbuffer;
}


//------------------------------------------------------------------------------
// Real CalculateGeodetics(Real *position, GmatEpoch when, bool includeLatLong)
//------------------------------------------------------------------------------
/**
 * Calculates the geodetic height, latitude and longitude for the input state
 *
 * The method used here is the same as used in the parameter code.  We may want
 * to refactor so that both call a common, low level source.
 *
 * @param position       The cb-centered MJ2000 Cartesian state for the
 *                       calculations
 * @param when           Epoch for the lat/long calculations
 * @param includeLatLong true to calculate latitude and longitude, false to skip
 *
 * @return The geodetic height.  The latitude and longitude are filled in to
 *         class variables
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::CalculateGeodetics(Real *position, GmatEpoch when,
                                         bool includeLatLong)
{
   Rvector6 instate(position), state;

   CoordinateSystem *j2000ToUse = (cbJ2000 == NULL ? mInternalCoordSystem : cbJ2000);

#ifdef DEBUG_CALCULATE_GEODETICS
   MessageInterface::ShowMessage("AtmosphereModel::CalculateGeodetics():   cbJ2000 = <%p>    mInternalCoordSystem = <%p>\n", cbJ2000, mInternalCoordSystem);
   if (cbJ2000 == NULL)
	   MessageInterface::ShowMessage("cbJ2000 == NULL, mInternalCoordSystem <%p,%s>\n", mInternalCoordSystem, mInternalCoordSystem->GetName().c_str());
   else
	   MessageInterface::ShowMessage("cbJ2000 <%p,%s>\n", cbJ2000, cbJ2000->GetName().c_str());
   MessageInterface::ShowMessage("j2000ToUse cs<%p,%s>: origin = %s    axes = %s\n", j2000ToUse, j2000ToUse->GetName().c_str(), j2000ToUse->GetOrigin()->GetName().c_str(), j2000ToUse->GetStringParameter("Axes").c_str());
#endif

   if (when == -1.0)
      when = wUpdateEpoch;

   #ifdef DEBUG_COORDINATE_TRANSFORMS
      MessageInterface::ShowMessage("Geodetic calculations at epoch %.12lf\n",
            when);
      if ((mInternalCoordSystem != NULL)&&(cbFixed != NULL))
         MessageInterface::ShowMessage("Internal CS:\n%s\nFixed:\n%s\n",
            mInternalCoordSystem->GetGeneratingString(
                  Gmat::NO_COMMENTS).c_str(),
            cbFixed->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      if (cbJ2000 != NULL)
         MessageInterface::ShowMessage("cbJ2000 CS:\n%s\n",
            cbJ2000->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      MessageInterface::ShowMessage("Position: %lf, %lf, %lf\n", position[0], position[1], position[2]);
   #endif

   CoordinateConverter mCoordConverter;
   mCoordConverter.Convert(A1Mjd(when), instate, j2000ToUse,
                           state, cbFixed);

   // Build angular momentum
   if (wUpdateEpoch != when)
      BuildAngularVelocity(when);

   // Get the body fixed geodetic height
   // Vallado algorithm 12 (Vallado, 2nd ed, p. 177)
   Real rxy = sqrt(state[0]*state[0] + state[1]*state[1]);
   geoLat = atan2(state[2], rxy);

   Real delta = 1.0;
   Real geodeticTolerance = 1.0e-7;    // Better than 0.0001 degrees
   Real ecc2 = cbFlattening * (2.0 - cbFlattening);

   Real cFactor, oldlat, sinlat;
   while (delta > geodeticTolerance)
   {
      oldlat = geoLat;
      sinlat = sin(oldlat);
      cFactor = cbRadius / sqrt(1.0 - ecc2 * sinlat * sinlat);
      geoLat = atan2(state[2] + cFactor*ecc2*sinlat, rxy);
      delta = fabs(geoLat - oldlat);
   }

   sinlat = sin(geoLat);
   cFactor = cbRadius / sqrt(1.0 - ecc2 * sinlat * sinlat);
   geoHeight = rxy / cos(geoLat) - cFactor;

   // Only do lat/long (in degrees) if needed
   if (includeLatLong)
   {
      geoLat = geoLat * GmatMathConstants::DEG_PER_RAD;
      geoLat = AngleUtil::PutAngleInDegRange(geoLat, -90.0, 90.0);
      geoLong = atan2(state[1], state[0]) * GmatMathConstants::DEG_PER_RAD;
      geoLong = AngleUtil::PutAngleInDegRange(geoLong, -180.0, 180.0);
   }

   #ifdef DEBUG_COORDINATE_TRANSFORMS
      MessageInterface::ShowMessage("Geodetics:\n   Height = %.6lf\n   "
            "Latitude = %.6lf\n   Longitude = %.6lf\n", geoHeight, geoLat,
            (includeLatLong ? geoLong : -999999.999999));
   #endif

   return geoHeight;

}


//------------------------------------------------------------------------------
// Real CalculateGeocentrics(Real *position, GmatEpoch when, bool includeLatLong)
//------------------------------------------------------------------------------
/**
 * Calculates the geocentric height, latitude and longitude for the input state
 *
 * The method used here is the same as used in the parameter code.  We may want
 * to refactor so that both call a common, low level source.
 *
 * @param position       The cb-centered MJ2000 Cartesian state for the
 *                       calculations
 * @param when           Epoch for the lat/long calculations
 * @param includeLatLong true to calculate latitude and longitude, false to skip
 *
 * @return The geocentrics height.  The latitude and longitude are filled in to
 *         class variables
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::CalculateGeocentrics(Real *position, GmatEpoch when,
                                           bool includeLatLong)
{
   Rvector6 instate(position), state;

   CoordinateSystem *j2000ToUse = (cbJ2000 == NULL ? mInternalCoordSystem : cbJ2000);

#ifdef DEBUG_CALCULATE_GEOCENTRICS
   if (cbJ2000 == NULL)
	   MessageInterface::ShowMessage("cbJ2000 == NULL, mInternalCoordSystem <%p,%s>\n", mInternalCoordSystem, mInternalCoordSystem->GetName().c_str());
   else
	   MessageInterface::ShowMessage("cbJ2000 <%p,%s>\n", cbJ2000, cbJ2000->GetName().c_str());
   MessageInterface::ShowMessage("j2000ToUse cs<%p,%s>: origin = %s    axes = %s\n", j2000ToUse, j2000ToUse->GetName().c_str(), j2000ToUse->GetOrigin()->GetName().c_str(), j2000ToUse->GetStringParameter("Axes").c_str());
#endif

   if (when == -1.0)
      when = wUpdateEpoch;

   #ifdef DEBUG_COORDINATE_TRANSFORMS
      MessageInterface::ShowMessage("Geocentric calculations at epoch %.12lf\n",
            when);
      MessageInterface::ShowMessage("Internal CS:\n%s\nFixed:\n%s\n",
            mInternalCoordSystem->GetGeneratingString(
                  Gmat::NO_COMMENTS).c_str(),
            cbFixed->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      if (cbJ2000 != NULL)
         MessageInterface::ShowMessage("cbJ2000 CS:\n%s\n",
            cbJ2000->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      MessageInterface::ShowMessage("Position: %lf, %lf, %lf\n", position[0], position[1], position[2]);
   #endif

   CoordinateConverter mCoordConverter;
   mCoordConverter.Convert(A1Mjd(when), instate, j2000ToUse,
                           state, cbFixed);

   // Build angular momentum
   if (wUpdateEpoch != when)
      BuildAngularVelocity(when);

   // Get the body fixed geocentric height
   //
   Real rxy = sqrt(state[0]*state[0] + state[1]*state[1]);
   geoLat = atan2(state[2], rxy);
//   geoHeight = rxy / cos(geoLat) - cbRadius;		// This equation is not correct for ellipsoid
//
//   Real cs = cos(geoLat);
//   Real sn = sin(geoLat);
//   Real f = cbFlattening;
//   geoHeight = rxy / cs - cbRadius*sqrt(cs*cs + (1-f)*(1-f)*sn*sn);		// Fixed bug GMT-4184

   Real delta = 1.0;
   Real tolerance = 1.0e-7;    // Better than 0.0001 degrees
   Real ecc2 = cbFlattening * (2.0 - cbFlattening);

   Real cFactor, oldlat, newlat, sinlat;
   newlat = geoLat;
   while (delta > tolerance)
   {
      oldlat = newlat;
      sinlat = sin(oldlat);
      cFactor = cbRadius / sqrt(1.0 - ecc2 * sinlat * sinlat);
      newlat = atan2(state[2] + cFactor*ecc2*sinlat, rxy);
      delta = fabs(newlat - oldlat);
   }

   sinlat = sin(newlat);
   cFactor = cbRadius / sqrt(1.0 - ecc2 * sinlat * sinlat);
   geoHeight = rxy / cos(newlat) - cFactor;


   // Only do lat/long (in degrees) if needed
   if (includeLatLong)
   {
      geoLat = geoLat * GmatMathConstants::DEG_PER_RAD;
      geoLat = AngleUtil::PutAngleInDegRange(geoLat, -90.0, 90.0);
      geoLong = atan2(state[1], state[0]) * GmatMathConstants::DEG_PER_RAD;
      geoLong = AngleUtil::PutAngleInDegRange(geoLong, -180.0, 180.0);
   }

   #ifdef DEBUG_COORDINATE_TRANSFORMS
      MessageInterface::ShowMessage("Geocentrics:\n   Height = %.6lf\n   "
            "Latitude = %.6lf\n   Longitude = %.6lf\n", geoHeight, geoLat,
            (includeLatLong ? geoLong : -999999.999999));
   #endif

   return geoHeight;
}


//------------------------------------------------------------------------------
//  void GetInputs(Real epoch)
//------------------------------------------------------------------------------
/**
 *  Sets the input global data for the model, either from a file or from user
 *  input constants.
 *
 *  @param epoch The current TAIJulian epoch
 */
//------------------------------------------------------------------------------
void AtmosphereModel::GetInputs(GmatEpoch epoch)
{
   #ifdef DEBUG_FLUX_FILE
      MessageInterface::ShowMessage("GetInputs, %s\n",
            (fluxReaderLoaded ? "true" : "false"));
   #endif

   // Process the epoch information
   Integer iEpoch = (Integer)(epoch);  // Truncate the epoch
   Integer yearOffset = (Integer)((epoch + 5.5) / GmatTimeConstants::DAYS_PER_YEAR);
   Integer year   = 1941 + yearOffset;
   Integer doy = iEpoch - (Integer)(yearOffset * GmatTimeConstants::DAYS_PER_YEAR) + 5;

   sod  = GmatTimeConstants::SECS_PER_DAY * (epoch - iEpoch + 0.5);  // Includes noon/midnight adjustment
   if (sod < 0.0)
   {
      sod += GmatTimeConstants::SECS_PER_DAY;
      doy -= 1;
   }

   if (sod > GmatTimeConstants::SECS_PER_DAY)
   {
      sod -= GmatTimeConstants::SECS_PER_DAY;
      doy += 1;
   }
   yd = year*1000 + doy;

   if (!fluxReaderLoaded)
   {
      std::string theHistFile = "", thePredictFile = "";

      // Only load the files that are needed
      if (historicalDataSource != 0)
         theHistFile = CSSIFileName;

      if (predictedDataSource == 1)
         thePredictFile = CSSIFileName;
      else if (predictedDataSource == 2)
         thePredictFile = schattenFileName;

      if ((historicalDataSource != 0) || (predictedDataSource != 0))
      {
         fluxReader->SetHistoricDataSource(historicalDataSource);
         fluxReader->SetPredictedDataSource(predictedDataSource);
         fluxReaderLoaded = fluxReader->LoadFluxData(theHistFile, thePredictFile);

         if (fluxReaderLoaded)
         {
            fluxReader->GetEpochs(historicStart, historicEnd, predictStart,
                  predictEnd);

            // Handle constants for historic data
            if (historicalDataSource == 0)
               historicEnd = predictStart;
         }
         if (predictedDataSource != 0)
         {
            #ifdef DEBUG_SCHATTEN_SETTINGS
               MessageInterface::ShowMessage("Setting Schatten flags: timing "
                     "%d, error %d\n", schattenTimingModel, schattenErrorModel);
            #endif

            fluxReader->SetSchattenFlags(schattenTimingModel, schattenErrorModel);
         }
      }
      else
      {
         // All constants; set the ranges for minimal impact
         fluxReaderLoaded = true;
         historicStart = 0.0;
         // GMAT's epoch is capped here:
         historicEnd = 58127.5;
         predictStart = predictEnd = 58127.5;
      }

      #ifdef DEBUG_FLUX_FILE
         MessageInterface::ShowMessage("Epoch settings: [%lf -> %lf], "
               "[%lf -> %lf]\n", historicStart, historicEnd, predictStart,
               predictEnd);
      #endif
   }

   #ifdef DEBUG_FLUX_FILE
      MessageInterface::ShowMessage("Data loaded; looking for epoch %lf\n", epoch);
   #endif

   if (fluxReaderLoaded && epoch > 0.0)
   {
      if (epoch < historicEnd)
      {
         switch(historicalDataSource)
         {
         case 1:
            fDbuffer = fluxReader->GetInputs(epoch);
            fluxReader->PrepareApData(fDbuffer, epoch);
            f107 = fDbuffer.obsF107;
            f107a = fDbuffer.obsCtrF107a;
            for (Integer i = 0; i < 7; i++)
                ap[i] = fDbuffer.ap[i];
            #ifdef DEBUG_FLUX_FILE
               MessageInterface::ShowMessage("%lf Historic flux: CSSI\n", epoch);
            #endif
            break;

         case 0:
         default:
            f107 = constantF107;
            f107a = constantF107a;
            for (Integer i = 0; i < 7; i++)
               ap[i] = constantAp;
            #ifdef DEBUG_FLUX_FILE
               MessageInterface::ShowMessage("%lf Historic flux: Constants\n", epoch);
            #endif
            break;
         }
      }
      else // Later than historic data end
      {
         switch(predictedDataSource)
         {
         case 1:
         case 2:
            fDbuffer = fluxReader->GetInputs(epoch);
            fluxReader->PrepareApData(fDbuffer, epoch);
            f107 = fDbuffer.obsF107;
            f107a = fDbuffer.obsCtrF107a;
            for (Integer i = 0; i < 7; i++)
                ap[i] = fDbuffer.ap[i];
            #ifdef DEBUG_FLUX_FILE
               MessageInterface::ShowMessage("%lf Predict flux: Schatten\n", epoch);
            #endif
            break;

         case 0:
         default:
            f107 = constantF107;
            f107a = constantF107a;
            for (Integer i = 0; i < 7; i++)
               ap[i] = constantAp;
            #ifdef DEBUG_FLUX_FILE
               MessageInterface::ShowMessage("%lf Predict flux: Constants\n", epoch);
            #endif
            break;
         }
      }
   }
   else  // All constants all the time
   {
      f107 = constantF107;
      f107a = constantF107a;
      for (Integer i = 0; i < 7; i++)
         ap[i] = constantAp;
   }

   #ifdef DUMP_FLUX_DATA
      MessageInterface::ShowMessage("%.12lf   %lf  %lf    [%lf %lf %lf %lf %lf "
            "%lf %lf]\n", epoch, f107, f107a, ap[0], ap[1], ap[2], ap[3], ap[4],
            ap[5], ap[6]);
   #endif

   #ifdef DEBUG_FLUX_FILE
      MessageInterface::ShowMessage("Flux data for %s model\n", instanceName.c_str());
      MessageInterface::ShowMessage("   Epoch:  %.12lf\n", epoch);
      MessageInterface::ShowMessage("   F10.7:  %lf\n", f107);
      MessageInterface::ShowMessage("   F10.7a: %lf\n", f107a);
      MessageInterface::ShowMessage("   Ap:     [%lf %lf %lf %lf %lf %lf %lf]\n", ap[0], ap[1], ap[2], ap[3], ap[4], ap[5], ap[6]);
   #endif
}


std::string AtmosphereModel::GetCentralBodyName()
{
   return centralBody;
}
