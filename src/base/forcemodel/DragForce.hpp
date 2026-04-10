//$Id$
//------------------------------------------------------------------------------
//                                DragForce
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
// Created: 2004/02/29
//
/**
 * Drag force modeling
 */
//------------------------------------------------------------------------------


#ifndef DragForce_hpp
#define DragForce_hpp

#include "PhysicalModel.hpp"
#include "AtmosphereModel.hpp"
#include "TimeTypes.hpp"
#include <vector>

// Forward reference
class CoordinateSystem;


/**
 * Class used to model accelerations due to drag.
 */
class GMAT_API DragForce : public PhysicalModel
{
public:
   DragForce(const std::string &name = "");
   virtual ~DragForce();
   
   DragForce(const DragForce& df); 
   DragForce&           operator=(const DragForce& df); 
   
   virtual bool         GetComponentMap(Integer * map, Integer order = 1) const;
   virtual void         SetSatelliteParameter(const Integer i,
                                              const std::string parmName, 
                                              const Real parm,
                                              const Integer parmID = -1);
   virtual void         SetSatelliteParameter(const Integer i,
                                              Integer parmID,
                                              const Real parm);
   virtual void         SetSatelliteParameter(const Integer i,
                                              const std::string parmName, 
                                              const std::string parm);
   virtual void         ClearSatelliteParameters(const std::string parmName = "");

//   virtual void         SetSpaceObject(const Integer i, GmatBase *obj); 

   virtual bool         AttitudeAffectsDynamics();

   virtual bool         IsUnique(const std::string &forBody = "");

   bool                 Initialize();
   virtual bool         GetDerivatives(Real * state, Real dt = 0.0, 
                                       Integer order = 1, 
                                       const Integer id = -1);
   virtual Rvector6     GetDerivativesForSpacecraft(Spacecraft *sc);

   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   
   // Parameter accessor methods -- overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   virtual bool         WriteEmptyStringParameter(const Integer id) const;
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const Integer id, const Real value);
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const char *value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;

   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const Integer id,
                                            const Integer index) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value,
                                            const Integer index);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label,
                                            const Integer index) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value,
                                            const Integer index);

   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   
   virtual Integer      GetOwnedObjectCount();
   virtual GmatBase*    GetOwnedObject(Integer whichOne);
   
   // Special access methods used by drag forces
   bool                 SetInternalAtmosphereModel(AtmosphereModel* atm);
   AtmosphereModel*     GetInternalAtmosphereModel();
   AtmosphereModel*     GetAtmosphereModel();
   static std::string   CheckFluxFile(const std::string &filename, bool isHistoric);
   
   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index, 
                         Integer quantity, Integer totalSize);

   // Made public so it can be called for the AtmosDensity parameter
   Real                 GetDensity(Real *state,
                              Real when = GmatTimeConstants::MJD_OF_J2000,
                              Integer count = -1);


   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Sun pointer for bulge calculations
   CelestialBody        *sun;
   /// Position of the Sun
   Real                 sunLoc[3];
   /// Central body pointer for bulge calculations
   CelestialBody        *centralBody;
   /// Position of the body with the atmosphere
   Real                 cbLoc[3];
   /// Angular velocity of the central body
   Real                 *angVel;
   /// Flag to indicate if the atmosphere model is externally owned or internal
   bool                 useExternalAtmosphere;
   /// Name of the atmosphere model we want to use
   std::string          atmosphereType;
   /// Pointer to the atmosphere model used
   AtmosphereModel      *atmos;
   /// Pointer to Internal atmosphere model
   AtmosphereModel      *internalAtmos;
   /// Array of densities
   Real                 *density;
   /// Density model: "High", "Low", or "Mean"
   std::string          densityModel;
   /// Inputfile containing all setting parameters for MarsGRAM
   std::string          inputFile;
   /// Array of products of spacecraft properties
   Real                 *prefactor;
   /// Flag used to determine if data has changed for the prefactors
   bool                 firedOnce;
   /// Flag used to indicate that local wind calcs are used
   bool                 hasWindModel;
   /// Number of spacecraft in the state vector that use CartesianState
   Integer              satCount;
   /// Central bodies used for atmosphere source
   StringArray          dragBody;
   /////// Pointers to the spacecraft 
   ////std::vector<GmatBase*> scObjs; 

   /// Spacecraft drag areas
   std::vector <Real>   area;
   /// Spacecraft masses
   std::vector <Real>   mass;
   /// Spacecraft coefficients of drag
   std::vector <Real>   dragCoeff;
   /// Spacecraft atmosphere density scale factor
   std::vector <Real>   atmosDensityScaleFactor;
   /// Size of the CartesianState data -- in other words, 6 * satCount
   Integer              orbitDimension;
   /// State vector translated from force model origin to body with atmosphere
   Real                 *dragState;
   /// Interval of angular momentum vector updates, in days
   Real                 wUpdateInterval;
   /// Epoch of last angular momentum update
   Real                 wUpdateEpoch;
   
   /// ID used to retrieve mass data
   Integer massID;
   /// ID used to retrieve coefficient of drag
   Integer cdID;
   /// ID used to retrieve drag area
   Integer areaID;
   /// ID used to set F10.7
   Integer F107ID;
   /// ID used to set F10.7A
   Integer F107AID;
   /// ID used to set Geomagnetic index
   Integer KPID;
   /// ID used to set Historic Weather File name
   Integer cssiWFileID;
   /// ID used to set Schatten Weather File name
   Integer schattenWFileID;

   ///// Flag indicating if Cd is being estimated 
   //bool estimatingCd; 
   /// ID for the CdEpsilon parameter
   Integer cdEpsilonID;
   ///// Row/Column for the Cd entries in the A-matrix and STM 
   //Integer cdEpsilonRow; 
   /// Current value(s) of Cd
   std::vector<Real> cdEpsilon;
   /// Initial value(s) of Cd epsilon at dt = 0.0
   std::vector<Real> cdEpsilonInitial;
   /// Initial value(s) of Cd
   std::vector<Real> cdInitial;

   /// ID used to retrieve atmosphere density scale factor
   Integer adsfID;
   /// ID for the AtmosDensity_Epsilon parameter
   Integer adsfEpsilonID;
   /// Current value(s) of atmosphere density scale factor
   std::vector<Real> adsfEpsilon;
   /// Initial value(s) of AtmosDensity_Epsilon at dt = 0.0
   std::vector<Real> adsfEpsilonInitial;
   /// Initial value(s) of atmosphere density scale factor
   std::vector<Real> adsfInitial;

   /// Flag used to indicate that central differences are used for the A-matrix
   bool useCentralDifferences;
   /// Flag used to finite difference the velocity derivatives
   bool finiteDifferenceDv;

   // Optional input parameters used by atmospheric models
   /// Type of input data -- "File" or "Constant"
   std::string          dataType;
   /// Historic Weather data type
   std::string          historicWSource;
   /// Predicted Weather data type
   std::string          predictedWSource;
   /// Default path to the flux files
   std::string          fluxPath;
   /// Historic Weather File name
   std::string          cssiWFile;
   /// Schatten Weather File name
   std::string          schattenWFile;
   /// "Current" value of F10.7
   Real                 fluxF107;
   /// Running average of the F10.7
   Real                 fluxF107A;
   /// Magnetic field index, Ap (a calculated value)
   Real                 ap;
   /// Magnetic field index, Kp (user specified)
   Real                 kp;
   /// Schatten Error Model
   std::string          schattenErrorModel;
   /// Schatten Timing Model
   std::string          schattenTimingModel;

   /// Start index for the Cartesian state
   Integer              cartIndex;
   /// Flag indicating if the Cartesian state should be populated
   bool                 fillCartesian;
   /// Start index for the Cd epsilon state
   Integer              cdEpsilonCount;
   /// Start index for the Cd epsilon state
   Integer              cdEpsilonIndex;
   /// Flag indicating if the Cd epsilon state should be populated
   bool                 fillCdEpsilon;
   /// Start index for the atmosphere density scale factor state
   Integer              atmosDensityEpsilonCount;
   /// Start index for the atmosphere density scale factor state
   Integer              atmosDensityEpsilonIndex;
   /// Flag indicating if the atmosphere density scale factor state should be populated
   bool                 fillAtmosDensityEpsilon;

   /// CS used to get the w cross r piece
   CoordinateSystem     *cbFixed;
   /// CS used to for conversions
   CoordinateSystem     *internalCoordSystem;
   /// Index used to select Kp/Ap conversion method.  Default is a table lookup
   Integer              kpApConversion;

   // the model to use - Spherical or SPADFile
   std::string          dragShapeModel;
   ShapeModel           dragShapeModelIndex;
   
   void                 BuildPrefactors(const std::string &forModel = "Spherical");
   void                 TranslateOrigin(const Real *state, const Real now);
//   void                 GetDensity(Real *state, Real when = GmatTimeConstants::MJD_OF_J2000);
      
   Real                 CalculateAp(Real kp);
   Rvector3             Accelerate(Integer scID, Real *theState,
                                   GmatEpoch &theEpoch, Real prefactor);

   
   /// Parameter IDs
   enum
   {
      ATMOSPHERE_MODEL = PhysicalModelParamCount, 
      ATMOSPHERE_BODY,
      SOURCE_TYPE,
      HISTORIC_WEATHER_SOURCE,
      PREDICTED_WEATHER_SOURCE,
      CSSI_WEATHER_FILE,
      SCHATTEN_WEATHER_FILE,
      FLUX,
      AVERAGE_FLUX,
      MAGNETIC_INDEX,
      SCHATTEN_ERROR_MODEL,
      SCHATTEN_TIMING_MODEL,
      DRAG_MODEL,
      FIXED_COORD_SYSTEM,
      W_UPDATE_INTERVAL,
      KP2AP_METHOD,
      DENSITY_MODEL,
      INPUT_FILE,
      DragForceParamCount
   };
   
   static const std::string 
      PARAMETER_TEXT[DragForceParamCount - PhysicalModelParamCount];
   static const Gmat::ParameterType 
      PARAMETER_TYPE[DragForceParamCount - PhysicalModelParamCount];
};

#endif // DragForce_hpp
