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

#ifndef AtmosphereModel_hpp
#define AtmosphereModel_hpp

#include "GmatBase.hpp"
#include "SolarFluxReader.hpp"
#include "AtmosphereException.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton

class SolarSystem;          // Forward reference
class CelestialBody;

/**
 * Base class for atmosphere models.
 * 
 * Atmosphere models are used to model drag forces and other aerodynamics.
 */ 
class GMAT_API AtmosphereModel : public GmatBase
{
public:
   AtmosphereModel(const std::string &typeStr, const std::string &name = "");
   virtual ~AtmosphereModel();
   
   // copy constructor - needed by child classes (for Clone)
   AtmosphereModel(const AtmosphereModel& am);
   AtmosphereModel& operator=(const AtmosphereModel& am);
    
   //---------------------------------------------------------------------------
   // bool Density(Real *position, Real *density, Real epoch = GmatTimeConstants::MJD_OF_J2000,
   //              Integer count = 1)
   //---------------------------------------------------------------------------
   /**
    * Calculates the atmospheric density at a specified location.
    *
    * Density is the core calculation provided by classes derived from this one.
    * The output array, density, must contain the density at the requested
    * locations, expressed in kg / m^3.
    *
    * @param position  The input vector of spacecraft states, in MJ2000Eq
    *                      coordinates.
    * @param density   The array of output densities.
    * @param epoch     The current TAI modified Julian epoch.
    * @param count     The number of spacecraft contained in position.
    *
    * @return true on success, false if a problem is encountered.
    */
   //---------------------------------------------------------------------------
   virtual bool Density(Real *position, Real *density, Real epoch = GmatTimeConstants::MJD_OF_J2000,
                        Integer count = 1) = 0;

   virtual bool Initialize();
   void         SetSunVector(Real *sv);
   void         SetCentralBodyVector(Real *cv);
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetCentralBody(CelestialBody *cb);
   virtual void SetCentralBody(std::string cbName);
   std::string  GetCentralBodyName();

   void         SetUpdateParameters(Real interval, GmatEpoch epoch = -1.0);
   virtual void SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void SetCbJ2000CoordinateSystem(CoordinateSystem *cs, bool canDelete = false);
   virtual CoordinateSystem *
                GetCbJ2000CoordinateSystem();
   void         SetFixedCoordinateSystem(CoordinateSystem *cs);
   Real*        GetAngularVelocity(const Real GmatEpoch = -1.0);
   void         BuildAngularVelocity(const Real GmatEpoch);
   void         UpdateAngularVelocity(const Real GmatEpoch);
   void         SetKpApConversionMethod(Integer method);
   Real         ConvertKpToAp(const Real kp);

   void         SetInputSource(const std::string &historical = "ConstantFluxAndGeoMag",
                               const std::string &predicted = "ConstantFluxAndGeoMag");
   void         SetSchattenFlags(const std::string &timing,
                               const std::string &magnitude);

   // Extra methods some models may support
   virtual bool HasWindModel();
   virtual bool Wind(Real *position, Real* wind, Real ep,
						   Integer count = 1);
   virtual bool HasTemperatureModel();
   virtual bool Temperature(Real *position, Real *temperature, 
                            Real epoch = GmatTimeConstants::MJD_OF_J2000,
                            Integer count = 1);
   virtual bool HasPressureModel();
   virtual bool Pressure(Real *position, Real *pressure, 
                         Real epoch = GmatTimeConstants::MJD_OF_J2000,
                         Integer count = 1);


   // Methods overridden from GmatBase
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);

   virtual Real        GetRealParameter(const Integer id,
                                        const Integer index) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value,
                                        const Integer index);


   virtual bool SetStringParameter(const Integer id,
                                          const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                  const std::string &value);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;

   SolarFluxReader*     GetFluxReader();
   SolarFluxReader::FluxData GetFluxData(GmatEpoch epoch = -1.0);

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Solar flux file reader
   SolarFluxReader         *fluxReader;
   /// Buffer used for flux data while moving from raw data to massaged data
   SolarFluxReader::FluxData fDbuffer;
   /// The solarsystem
   SolarSystem             *solarSystem;
   /// The central body
   CelestialBody           *mCentralBody;
   /// CSSI Solar flux file name
   std::string             CSSIFileName;
   /// Schatten Solar flux file name
   std::string             schattenFileName;
   /// Vector from the central body to the sun
   Real                    *sunVector;
   /// Name of the central body
   std::string             centralBody;
   /// Location of the central body
   Real                    *centralBodyLocation;
   /// Central body radius
   Real                    cbRadius;
   /// Central body flattening factor
   Real                    cbFlattening;
   /// SolarFlux files are loaded ?
   bool                    fluxReaderLoaded;

   // Values used if a file is not set
   /// Constant value of F10.7 to use.  This is a fix for an issue uncovered in
   /// testing: the nominal values can be overridden in some of the atmosphere
   /// model code.
   Real                    constantF107;
   /// Constant 3 month average of the F10.7 data.
   Real                    constantF107a;
   /// Constant geomagnetic (planetary) index, \f$K_p\f$, the value users access.
   Real                    constantKp;
   /// Constant geomagnetic planetary amplitude, calculated via Vallado eq. 8-31.
   Real                    constantAp;

   /// Nominal value of F10.7 to use.
   Real                    nominalF107;
   /// Nominal 3 month average of the F10.7 data.
   Real                    nominalF107a;
   /// Nominal geomagnetic (planetary) index, \f$K_p\f$, the value users access.
   Real                    nominalKp;
   /// Nominal geomagnetic planetary amplitude, calculated via Vallado eq. 8-31.
   Real                    nominalAp;
   /// Index used to select Kp/Ap conversion method.  Default is a table lookup
   Integer                 kpApConversion;
   /// Indicator of the source for historical data: 0 for constants, 1 for CSSI
   Integer                 historicalDataSource;
   /// Indicator of the source for predicted data: 0 for constants, 1 for CSSI, 2 for Schatten
   Integer                 predictedDataSource;
   /// Internal coordinate system used for conversions
   CoordinateSystem        *mInternalCoordSystem;
   /// MJ2000 CS for the central body
   CoordinateSystem        *cbJ2000;
   /// Flag indicating is the CBJ2000CS should be deleted in the atmosphere destructor
   bool                    canDeleteCBJ2000CS;
   /// Body fixed CS for the central body
   CoordinateSystem        *cbFixed;
   /// Angular velocity of the central body
   Real                    angVel[3];
   /// Update interval for the angular momentum vector
   Real                    wUpdateInterval;
   /// Most recent update epoch for the angular momentum
   GmatEpoch               wUpdateEpoch;
   /// Most recent geodetic height calculated
   Real                    geoHeight;
   /// Most recent geodetic latitude calculated
   Real                    geoLat;
   /// Most recent geodetic longitude calculated
   Real                    geoLong;
   /// Specify geodetic coordinate system or gecentric to be used for calculation
   bool                    useGeodetic;
   /// Most recent GHA calculated
   Real                    gha;
   /// GHA epoch
   Real                    ghaEpoch;

   /// Start of the historic data when using file based history data
   GmatEpoch historicStart;
   /// End of the historic data when using file based history data
   GmatEpoch historicEnd;
      /// Start of the predict data when using file based predicted data
   GmatEpoch predictStart;
   /// End of the predict data when using file based predicted data
   GmatEpoch predictEnd;
   /// Schatten timing model to use
   Integer schattenTimingModel;
   /// Schatten error model to use
   Integer schattenErrorModel;

   // Fields used when retrieving data from a flux file

   /// Second of day
   Real                    sod;
   /// Year + Day of year, in the form YYYYDDD
   Integer                 yd;
   /// Value of F10.7 to use
   Real                    f107;
   /// 3 month average of the F10.7 data
   Real                    f107a;
   /// Geomagnetic index (Ap, not Kp)
   Real                    ap[7];

   /// Time converter singleton
   TimeSystemConverter     *theTimeConverter;

   // Input method shared by all MSISE models
   void                    GetInputs(GmatEpoch epoch);
   
   Real                    CalculateGeodetics(Real *position,
                                 GmatEpoch when = -1.0,
                                 bool includeLatLong = false);

   Real                    CalculateGeocentrics(Real *position,
                                 GmatEpoch when = -1.0,
                                 bool includeLatLong = false);

   enum
   {
      NOMINAL_FLUX = GmatBaseParamCount,
      NOMINAL_AVERAGE_FLUX,
      NOMINAL_MAGNETIC_INDEX,
      CSSI_WEATHER_FILE,
      SCHATTEN_WEATHER_FILE,
      AtmosphereModelParamCount
   };
   
   static const std::string
      PARAMETER_TEXT[AtmosphereModelParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[AtmosphereModelParamCount - GmatBaseParamCount];

private:  
    
};

#endif // AtmosphereModel_hpp
