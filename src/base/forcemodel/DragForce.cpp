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


#include "DragForce.hpp"
#include "StringUtil.hpp"     // for ToString()
#include "ODEModelException.hpp"
#include "MessageInterface.hpp"
#include "CoordinateSystem.hpp"
#include "TimeTypes.hpp"
#include "FileManager.hpp"    // for flux files
#include "PropagationStateManager.hpp"

#include <sstream>                 // for <<
#include <cmath>

// Uncomment to generate drag model data for debugging:
//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZE
//#define DEBUG_DRAGFORCE_DENSITY
//#define DEBUG_DRAGFORCE_PARAM
//#define DEBUG_DRAGFORCE_REFOBJ
//#define DEBUG_ANGVEL
//#define DEBUG_FIRST_CALL
//#define DEBUG_NAN_CONDITIONS
//#define DEBUG_FLUX_FILE
//#define DEBUG_FINITEDIFF

//#define DUMP_DERIVATIVE
//#define DUMP_DENSITY
//#define DUMP_INITIAL_STATE_DERIVATIVES_ONLY


//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif


#ifdef DEBUG_DRAGFORCE_DENSITY
   #include <fstream>
   
   std::ofstream dragdata;
#endif

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif



//---------------------------------
// static data
//---------------------------------
const std::string
DragForce::PARAMETER_TEXT[DragForceParamCount - PhysicalModelParamCount] =
{
   "AtmosphereModel",               // ATMOSPHERE_MODEL
   "AtmosphereBody",                // ATMOSPHERE_BODY
   "InputSource",                   // SOURCE_TYPE
   "HistoricWeatherSource",         // HISTORIC_WEATHER_SOURCE
   "PredictedWeatherSource",        // PREDICTED_WEATHER_SOURCE
   "CSSISpaceWeatherFile",          // CSSI_WEATHER_FILE
   "SchattenFile",                  // SCHATTEN_WEATHER_FILE
   "F107",                          // FLUX
   "F107A",                         // AVERAGE_FLUX
   "MagneticIndex",                 // MAGNETIC_INDEX
   "SchattenErrorModel",            // SCHATTEN_ERROR_MODEL
   "SchattenTimingModel",           // SCHATTEN_TIMING_MODEL
   "DragModel",                     // Drag Model - Spherical or SPADFile
   "FixedCoordinateSystem",         // FIXED_COORD_SYSTEM  (Read-only parameter)
   "AngularMomentumUpdateInterval", // W_UPDATE_INTERVAL (in days, Read-only)
   "KpToApMethod",                  // KP2AP_METHOD (Read-only)
   "DensityModel",                  // DENSITY_MODEL used for MarsGRAM2005
   "InputFile",                     // INPUTFILE     used for MarsGRAM2005 and exponential drag
};

const Gmat::ParameterType
DragForce::PARAMETER_TYPE[DragForceParamCount - PhysicalModelParamCount] =
{
   Gmat::OBJECT_TYPE,   // "AtmosphereModel"
   Gmat::STRING_TYPE,   // "AtmosphereBody",
   Gmat::STRING_TYPE,   // "InputSource", ("File" or "Constant" for now)
   Gmat::STRING_TYPE,   // "HistoricWeatherSource",
   Gmat::STRING_TYPE,   // "PredictedWeatherSource",
   Gmat::STRING_TYPE,   // "CSSISpaceWeatherFile",
   Gmat::STRING_TYPE,   // "SchattenFile",
   Gmat::REAL_TYPE,     // "F107",
   Gmat::REAL_TYPE,     // "F107A",
   Gmat::REAL_TYPE,     // "MagneticIndex",
   Gmat::STRING_TYPE,   // "SchattenError",
   Gmat::STRING_TYPE,   // "SchattenTiming",
   Gmat::STRING_TYPE,   // "DragModel",
   Gmat::STRING_TYPE,   // "FixedCoordinateSystem"
   Gmat::REAL_TYPE,     // "AngularMomentumUpdateInterval"
   Gmat::INTEGER_TYPE,  // "KpToApMethod"
   Gmat::STRING_TYPE,   // "DensityModel"
   Gmat::STRING_TYPE,   // "InputFile"
};

//------------------------------------------------------------------------------
// DragForce(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor for the DragForce.
 * 
 * @param name Optional name for this force component.
 */
//------------------------------------------------------------------------------
DragForce::DragForce(const std::string &name) :
   PhysicalModel           (Gmat::PHYSICAL_MODEL, "DragForce", name),
   sun                     (NULL),
   centralBody             (NULL),
   angVel                  (NULL),
   useExternalAtmosphere   (true),
   atmosphereType          (""),
   atmos                   (NULL),
   internalAtmos           (NULL),
   density                 (NULL),
   densityModel            (""),
   inputFile               (""),
   prefactor               (NULL),
   firedOnce               (false),
   hasWindModel            (false),
   satCount                (1),
   orbitDimension          (0),
   dragState               (NULL),
   wUpdateInterval         (0.02),      // 0.02 = 28.8 minutes
   wUpdateEpoch            (-1.0),              // Force update if not set to fixed w
   massID                  (-1),
   cdID                    (-1),
   areaID                  (-1),
   F107ID                  (-1),
   F107AID                 (-1),
   KPID                    (-1),
   cssiWFileID             (-1),
   schattenWFileID         (-1),
   cdEpsilonID             (-1),
   adsfID                  (-1),
   adsfEpsilonID           (-1),
   useCentralDifferences   (false),
   finiteDifferenceDv      (true),
   dataType                ("Constant"),
   historicWSource         ("ConstantFluxAndGeoMag"),
   predictedWSource        ("ConstantFluxAndGeoMag"),
   fluxF107                (150.0),
   fluxF107A               (150.0),
   kp                      (3.0),
   schattenErrorModel      ("Nominal"),
   schattenTimingModel     ("NominalCycle"),
   cartIndex               (0),
   fillCartesian           (false),
   cdEpsilonCount          (0),
   cdEpsilonIndex          (0),
   fillCdEpsilon           (false),
   atmosDensityEpsilonCount(0),
   atmosDensityEpsilonIndex(0),
   fillAtmosDensityEpsilon (false),
   cbFixed                 (NULL),
   internalCoordSystem     (NULL),
   kpApConversion          (0),
   dragShapeModel          ("Spherical"),
   dragShapeModelIndex     (ShapeModel::SPHERICAL_MODEL)
{
   #ifdef DEBUG_CONSTRUCTION
      MessageInterface::ShowMessage("DragForce default construction "
            "<'%s',%p>\n", GetName().c_str(), this);
   #endif

   dimension = 6;
   parameterCount = DragForceParamCount;
   objectTypeNames.push_back("DragForce");
   objectTypeNames.push_back("Drag");

   
   // Default Sun location, from the SLP file at MJD GmatTimeConstants::MJD_OF_J2000:
   sunLoc[0] =  2.65e+07;
   sunLoc[1] = -1.32757e+08;
   sunLoc[2] = -5.75566e+07;
   
   cbLoc[0]  = 0.0;
   cbLoc[1]  = 0.0;
   cbLoc[2]  = 0.0;
   
   FileManager *fm = FileManager::Instance();
   fluxPath = fm->GetAbsPathname("ATMOSPHERE_PATH");
   cssiWFile = fm->GetFilename("CSSI_FLUX_FILE");
   schattenWFile = fm->GetFilename("SCHATTEN_FILE");
   
   ap = CalculateAp(kp);
   
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   isConservative = false;
   hasMassJacobian = true;
   
   #ifdef DEBUG_DRAGFORCE_DENSITY
       dragdata.open("DragData.csv");
       dragdata << "Atmospheric drag parameters\n";
   #endif
}


//------------------------------------------------------------------------------
// ~DragForce()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
DragForce::~DragForce()
{
   // Do not delete atmos due to it is not created in DragForce class

   if (internalAtmos)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (internalAtmos, "internalAtmos", "DragForce::~DragForce()",
          "deleting internal atmosphere model", this);
      #endif
      delete internalAtmos;
   }

   if (density)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (density, "density", "DragForce::Initialize()",
          "deleting density[satCount]", this);
      #endif
      delete [] density;
   }

   if (dragState)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (dragState, "dragState", "DragForce::Initialize()",
          "deleting dragState[orbitDimension]", this);
      #endif
      delete [] dragState;
   }

   if (prefactor)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (prefactor, "prefactor", "DragForce::Initialize()",
          "deleting prefactor[satCount]", this);
      #endif
      delete [] prefactor;
   }
   
//   if (cbFixed)
//      delete cbFixed;

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata.close();
   #endif
}


//------------------------------------------------------------------------------
// DragForce(const DragForce& df)
//------------------------------------------------------------------------------
/**
 * Copy Constructor.
 * 
 * @param df The DragForce instance used to set parameters for this clone.
 */
//------------------------------------------------------------------------------
DragForce::DragForce(const DragForce& df) :
   PhysicalModel           (df),
   sun                     (NULL),
   centralBody             (NULL),
   angVel                  (NULL),
   useExternalAtmosphere   (df.useExternalAtmosphere),
   atmosphereType          (df.atmosphereType),
   atmos                   (NULL),
   density                 (NULL),
   densityModel            (df.densityModel),
   inputFile               (df.inputFile),
   prefactor               (NULL),
   firedOnce               (false),
   hasWindModel            (df.hasWindModel),
   satCount                (df.satCount),
   dragBody                (df.dragBody),
   dragState               (NULL),
   wUpdateInterval         (df.wUpdateInterval),
   wUpdateEpoch            (df.wUpdateEpoch),
   massID                  (df.massID),
   cdID                    (df.cdID),
   areaID                  (df.areaID),
   F107ID                  (df.F107ID),
   F107AID                 (df.F107AID),
   KPID                    (df.KPID),
   cssiWFileID             (df.cssiWFileID),
   schattenWFileID         (df.schattenWFileID),
   cdEpsilonID             (df.cdEpsilonID),
   cdEpsilon               (df.cdEpsilon),
   cdEpsilonInitial        (df.cdEpsilonInitial),
   cdInitial               (df.cdInitial),
   adsfID                  (df.adsfID),
   adsfEpsilonID           (df.adsfEpsilonID),
   adsfEpsilon             (df.adsfEpsilon),
   adsfEpsilonInitial      (df.adsfEpsilonInitial),
   adsfInitial             (df.adsfInitial),
   useCentralDifferences   (df.useCentralDifferences),
   finiteDifferenceDv      (df.finiteDifferenceDv),
   dataType                (df.dataType),
   historicWSource         (df.historicWSource),
   predictedWSource        (df.predictedWSource),
   fluxPath                (df.fluxPath),
   cssiWFile               (df.cssiWFile),
   schattenWFile           (df.schattenWFile),
   fluxF107                (df.fluxF107),
   fluxF107A               (df.fluxF107A),
   kp                      (df.kp),
   schattenErrorModel      (df.schattenErrorModel),
   schattenTimingModel     (df.schattenTimingModel),
   cartIndex               (df.cartIndex),
   fillCartesian           (df.fillCartesian),
   cdEpsilonCount          (df.cdEpsilonCount),
   cdEpsilonIndex          (df.cdEpsilonIndex),
   fillCdEpsilon           (df.fillCdEpsilon),
   atmosDensityEpsilonCount(df.atmosDensityEpsilonCount),
   atmosDensityEpsilonIndex(df.atmosDensityEpsilonIndex),
   fillAtmosDensityEpsilon (df.fillAtmosDensityEpsilon),
   cbFixed                 (NULL),
   internalCoordSystem     (NULL),
   kpApConversion          (df.kpApConversion),
   dragShapeModel          (df.dragShapeModel),
   dragShapeModelIndex     (df.dragShapeModelIndex)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("DragForce copy construction from <'%s',%p> to <'%s',%p>   enetered\n", df.GetName().c_str(), &df, GetName().c_str(), &(*this));
#endif

   internalAtmos = NULL;
   if (df.internalAtmos)
   {
      internalAtmos = (AtmosphereModel*)df.internalAtmos->Clone();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (internalAtmos, internalAtmos->GetName(), "DragForce::DragForce(copy)",
          "internalAtmos = (AtmosphereModel*)df.internalAtmos->Clone()", this);
      #endif
   }
   
   if (df.cbFixed)
//      cbFixed = (CoordinateSystem*)(df.cbFixed->Clone());      // Any other initialization needed?
      cbFixed = (CoordinateSystem*)(df.cbFixed);

   parameterCount = DragForceParamCount;

   dimension = df.dimension;
   orbitDimension = df.orbitDimension;

   sunLoc[0] = df.sunLoc[0];
   sunLoc[1] = df.sunLoc[1];
   sunLoc[2] = df.sunLoc[2];
   
   cbLoc[0]  = df.cbLoc[0];
   cbLoc[1]  = df.cbLoc[1];
   cbLoc[2]  = df.cbLoc[2];
   
   //density = new Real[1];
   if (df.satCount < 1)
      density = new Real[1];
   else
      density = new Real[df.satCount];
   ap = CalculateAp(kp);
   
   area.clear();
   mass.clear();
   dragCoeff.clear();
   atmosDensityScaleFactor.clear();

#ifdef DEBUG_CONSTRUCTION
   if (df.atmos == NULL)
	   MessageInterface::ShowMessage("$$$ df.atmos = NULL\n");
   else
      MessageInterface::ShowMessage("$$$   df.atmos = <'%s',%p>\n", df.atmos->GetName().c_str(), df.atmos);

   if (df.internalAtmos == NULL)
	   MessageInterface::ShowMessage("$$$ df.internalAtmos = NULL\n");
   else
   {
	   MessageInterface::ShowMessage("$$$   df.internalAtmos = <'%s',%p>  df.internalAtmos.densitymodel = %s\n", df.internalAtmos->GetName().c_str(), df.internalAtmos, df.internalAtmos->GetStringParameter("DensityModel").c_str());
	   MessageInterface::ShowMessage("$$$   internalAtmos = <'%s',%p>  internalAtmos.densitymodel = %s\n", internalAtmos->GetName().c_str(), internalAtmos, internalAtmos->GetStringParameter("DensityModel").c_str());
   }
//   MessageInterface::ShowMessage("densityModel = '%s'\n",densityModel.c_str());
//   MessageInterface::ShowMessage("inputFile = '%s'\n",inputFile.c_str());
   MessageInterface::ShowMessage("DragForce copy construction from <'%s',%p> to <'%s',%p>   exit\n", df.GetName().c_str(), &df, GetName().c_str(), &(*this));
#endif
}


//------------------------------------------------------------------------------
// DragForce& operator=(const DragForce& df)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param df DragForce instance used as a template for this copy.
 * 
 * @return A reference to this class, with members set to match the template.
 */
//------------------------------------------------------------------------------
DragForce& DragForce::operator=(const DragForce& df)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("DragForce operator =\n");
#endif

   if (this == &df)
      return *this;
   
   PhysicalModel::operator=(df);
   
   sun                   = NULL;
   centralBody           = NULL;
   useExternalAtmosphere = df.useExternalAtmosphere;
   atmosphereType        = df.atmosphereType;
   
   massID                = df.massID;
   cdID                  = df.cdID;
   areaID                = df.areaID;
   F107ID                = df.F107ID;
   F107AID               = df.F107AID;
   KPID                  = df.KPID;
   cssiWFileID           = df.cssiWFileID;
   schattenWFileID       = df.schattenWFileID;
   
   cdEpsilonID           = df.cdEpsilonID;
   cdEpsilon             = df.cdEpsilon;
   cdEpsilonInitial      = df.cdEpsilonInitial;
   cdInitial             = df.cdInitial;
   adsfID                = df.adsfID;
   adsfEpsilonID         = df.adsfEpsilonID;
   adsfEpsilon           = df.adsfEpsilon;
   adsfEpsilonInitial    = df.adsfEpsilonInitial;
   adsfInitial           = df.adsfInitial;
   useCentralDifferences = df.useCentralDifferences;
   finiteDifferenceDv    = df.finiteDifferenceDv;

   if (internalAtmos != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (internalAtmos, "internalAtmos", "DragForce::operator=()",
          "deleting internal atmosphere model");
      #endif
      if (atmos == internalAtmos)
         atmos = NULL;
      delete internalAtmos;
   }
   internalAtmos = NULL;
   atmos = NULL;

   if (df.internalAtmos)
   {
      internalAtmos = (AtmosphereModel*)df.internalAtmos->Clone();
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (internalAtmos, internalAtmos->GetName(), "DragForce::operator=()",
          "internalAtmos = (AtmosphereModel*)df.internalAtmos->Clone()", this);
      #endif
   }
   
//   if (df.cbFixed)                                             // made changes for bug GMT-5282
//   {                                                           // made changes for bug GMT-5282
////      if (cbFixed)                                           // made changes for bug GMT-5282
////         delete cbFixed;                                     // made changes for bug GMT-5282
////      cbFixed = (CoordinateSystem*)(df.cbFixed->Clone());    // made changes for bug GMT-5282
//      cbFixed = (CoordinateSystem*)(df.cbFixed);               // made changes for bug GMT-5282
//   }                                                           // made changes for bug GMT-5282
   cbFixed               = df.cbFixed;                           // made changes for bug GMT-5282
   internalCoordSystem   = df.internalCoordSystem;               // made changes for bug GMT-5282

   angVel                = NULL;

   if (df.satCount < 1)
      density = new Real[1];
   else
      density = new Real[df.satCount];
   prefactor             = NULL;
   firedOnce             = false;
   hasWindModel          = df.hasWindModel;
   satCount              = df.satCount;
   dragBody              = df.dragBody;
   dragState             = NULL; //df.dragState;
   wUpdateInterval       = df.wUpdateInterval;
   wUpdateEpoch          = df.wUpdateEpoch;
   dataType              = df.dataType;
   historicWSource       = df.historicWSource;
   predictedWSource      = df.predictedWSource;
   cssiWFile             = df.cssiWFile;
   schattenWFile         = df.schattenWFile;
   fluxF107              = df.fluxF107;
   fluxF107A             = df.fluxF107A;
   schattenErrorModel    = df.schattenErrorModel;
   schattenTimingModel   = df.schattenTimingModel;
   kpApConversion        = df.kpApConversion;
   kp                    = df.kp;
   ap                    = CalculateAp(kp);
   
   dimension = df.dimension;
   orbitDimension = df. orbitDimension;

   sunLoc[0] = df.sunLoc[0];
   sunLoc[1] = df.sunLoc[1];
   sunLoc[2] = df.sunLoc[2];
   
   cbLoc[0]  = df.cbLoc[0];
   cbLoc[1]  = df.cbLoc[1];
   cbLoc[2]  = df.cbLoc[2];

   ap = CalculateAp(kp);
   
   area.clear();
   mass.clear();
   dragCoeff.clear();
   atmosDensityScaleFactor.clear();
   
   cartIndex = df.cartIndex;
   fillCartesian = df.fillCartesian;
   cdEpsilonCount = df.cdEpsilonCount;
   cdEpsilonIndex = df.cdEpsilonIndex;
   fillCdEpsilon = df.fillCdEpsilon;
   atmosDensityEpsilonCount = df.atmosDensityEpsilonCount;
   atmosDensityEpsilonIndex = df.atmosDensityEpsilonIndex;
   fillAtmosDensityEpsilon = df.fillAtmosDensityEpsilon;

   densityModel = df.densityModel;
   inputFile    = df.inputFile;
   
   dragShapeModel      = df.dragShapeModel;
   dragShapeModelIndex = df.dragShapeModelIndex;

   return *this;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the DragForce.
 *
 * @return clone of the DragForce.
 *
 */
//------------------------------------------------------------------------------
GmatBase* DragForce::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("DragForce::Clone()\n");
#endif

   return (new DragForce(*this));
}


//------------------------------------------------------------------------------
// bool GetComponentMap(Integer * map, Integer order) const
//------------------------------------------------------------------------------
/**
 * Mapping function for integrators of different orders.
 * 
 * Provides a map between the incoming state and the location of the 
 * corresponding derivative information.  The resulting data, passed back in the 
 * array map, is used to determine where the derivative data is stored for a
 * given component of the input state.
 * 
 * @param map     Array used for the mapping
 * @param order   Order of the requesting class (usually an integrator)
 */
//------------------------------------------------------------------------------
bool DragForce::GetComponentMap(Integer * map, Integer order) const
{
   Integer i6;

   if (order != 1)
      throw ODEModelException(
         "Drag supports 1st order equations of motion only");

   // Calculate how many spacecraft are in the model
   for (Integer i = 0; i < satCount; i++)
   {
      i6 = i * 6 + cartIndex;

      map[ i6 ] = i6 + 3;
      map[i6+1] = i6 + 4;
      map[i6+2] = i6 + 5;
      map[i6+3] = -1;
      map[i6+4] = -1;
      map[i6+5] = -1;
   }

   return true;
}


//------------------------------------------------------------------------------
// void SetSatelliteParameter(const Integer i, const std::string parmName, 
//                            const Real parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the force model.
 * 
 * For drag modeling, this method is used to set or update C_d, area, and mass.
 * 
 * @param i ID for the spacecraft
 * @param parmName name of the Spacecraft parameter 
 * @param parm Parameter value
 */
//------------------------------------------------------------------------------
void DragForce::SetSatelliteParameter(const Integer i, 
                                      const std::string parmName, 
                                      const Real parm,
                                      const Integer parmID)
{
   unsigned parmNumber = (unsigned)(i);

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Setting satellite parameter " << parmName
               << " for Spacecraft " << i << " to " << parm << "\n";
   #endif
    
   if (parmName == "Mass")
   {
      if (parmNumber < mass.size())
         mass[i] = parm;
      else
         mass.push_back(parm);
      if (parmID >= 0)
         massID = parmID;
   }
   
   if (parmName == "Cd")
   {
      if (dragShapeModelIndex == ShapeModel::SPHERICAL_MODEL)
      {
         if (parmNumber < dragCoeff.size())
            dragCoeff[i] = parm;
         else
         {
            dragCoeff.push_back(parm);
            cdInitial.push_back(parm);
         }

         if (parmID >= 0)
            cdID = parmID;
      }
   }

   if (parmName == "SPADDragScaleFactor")
   {
      if (dragShapeModelIndex == ShapeModel::SPAD_FILE_MODEL)
      {
         if (parmNumber < dragCoeff.size())
            dragCoeff[i] = parm;
         else
         {
            dragCoeff.push_back(parm);
            cdInitial.push_back(parm);
         }

         if (parmID >= 0)
            cdID = parmID;
      }
   }

   if (parmName == "DragArea")
   {
      if (parmNumber < area.size())
         area[i] = parm;
      else
         area.push_back(parm);
      if (parmID >= 0)
         areaID = parmID;
   }
   if (parmName == "Cd_Epsilon")
   {
       if (parmNumber < cdEpsilon.size())
          cdEpsilon[i] = parm;
       else
          cdEpsilon.push_back(parm);
       if (parmID >= 0)
          cdEpsilonID = parmID;
   }

   if (parmName == "AtmosDensityScaleFactor")
   {
      if (parmNumber < atmosDensityScaleFactor.size())
         atmosDensityScaleFactor[i] = parm;
      else
      {
         atmosDensityScaleFactor.push_back(parm);
         adsfInitial.push_back(parm);
      }

      if (parmID >= 0)
         adsfID = parmID;
   }
   if (parmName == "AtmosDensity_Epsilon")
   {
      if (parmNumber < adsfEpsilon.size())
         adsfEpsilon[i] = parm;
      else
         adsfEpsilon.push_back(parm);
      if (parmID >= 0)
         adsfEpsilonID = parmID;
   }
}


//------------------------------------------------------------------------------
// void SetSatelliteParameter(const Integer i, const Integer parmID,
//                            const Real parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the force model.
 *
 * For drag modeling, this method is used to set or update C_d, area, and mass.
 *
 * @param i ID for the spacecraft
 * @param parmID id of the Spacecraft parameter
 * @param parm Parameter value
 */
//------------------------------------------------------------------------------
void DragForce::SetSatelliteParameter(const Integer i,
                                      const Integer parmID,
                                      const Real parm)
{
   unsigned parmNumber = (unsigned)(i);
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Setting satellite parameter ID " << parmID
               << " for Spacecraft " << i << " to " << parm << "\n";
   #endif

   if (parmID == massID)
   {
      if (parmNumber < mass.size())
         mass[i] = parm;
      else
         mass.push_back(parm);
   }
   if (parmID == cdID)
   {
      if (parmNumber < dragCoeff.size())
         dragCoeff[i] = parm;
      else
      {
         dragCoeff.push_back(parm);
         cdInitial.push_back(parm);
      }
   }
   if (parmID == areaID)
   {
      if (parmNumber < area.size())
         area[i] = parm;
      else
         area.push_back(parm);
   }
   if (parmID == cdEpsilonID)
   {
      if (parmNumber < cdEpsilon.size())
         cdEpsilon[i] = parm;
      else
         cdEpsilon.push_back(parm);
   }
   if (parmID == adsfID)
   {
      if (parmNumber < atmosDensityScaleFactor.size())
         atmosDensityScaleFactor[i] = parm;
      else
      {
         atmosDensityScaleFactor.push_back(parm);
         adsfInitial.push_back(parm);
      }
   }
   if (parmID == adsfEpsilonID)
   {
      if (parmNumber < adsfEpsilon.size())
         adsfEpsilon[i] = parm;
      else
         adsfEpsilon.push_back(parm);
   }
}


//------------------------------------------------------------------------------
// void SetSatelliteParameter(const Integer i, const std::string parmName, 
//                            const std::string parm)
//------------------------------------------------------------------------------
/**
 * Passes spacecraft parameters to the force model.
 * 
 * For drag modeling, this method is used to set the body with the atmosphere.
 * 
 * @param i ID for the spacecraft
 * @param parmName name of the Spacecraft parameter 
 * @param parm Parameter value
 */
//------------------------------------------------------------------------------
void DragForce::SetSatelliteParameter(const Integer i, 
      const std::string parmName, const std::string parm)
{
   unsigned parmNumber = (unsigned)(i);
   if (parmName == "ReferenceBody")
   {
      if (parmNumber < dragBody.size())
         dragBody[i] = parm;
      else
         dragBody.push_back(parm);
   }
}


//------------------------------------------------------------------------------
// void ClearSatelliteParameters(const std::string parmName)
//------------------------------------------------------------------------------
/**
 * Resets the DragForce to receive a new set of satellite parameters.
 *
 * @param parmName name of the Spacecraft parameter.  The empty string clear all
 *                 of the satellite parameters for the PhysicalModel.
 */
//------------------------------------------------------------------------------
void DragForce::ClearSatelliteParameters(const std::string parmName)
{
   if ((parmName == "Mass") || (parmName == ""))
      mass.clear();
   if ((parmName == "Cd") || (parmName == "SPADDragScaleFactor") || (parmName == ""))
   {
      dragCoeff.clear();
      cdInitial.clear();
   }

   if ((parmName == "Cd_Epsilon") || (parmName == ""))
      cdEpsilon.clear();
   if ((parmName == "DragArea") || (parmName == ""))
      area.clear();
   if ((parmName == "AtmosDensityScaleFactor") || (parmName == ""))
   {
      atmosDensityScaleFactor.clear();
      adsfInitial.clear();
   }
   if ((parmName == "AtmosDensity_Epsilon") || (parmName == ""))
      adsfEpsilon.clear();
   if ((parmName == "scObjs")  || (parmName == ""))
      scObjs.clear();
}

////////------------------------------------------------------------------------------
//////// void PhysicalModel::SetSpaceObject(const Integer i, GmatBase *obj)
////////------------------------------------------------------------------------------
///////**
////// * Passes spacecraft pointers to the force model.
////// *
////// * @param i   ID for the spacecraft
////// * @param obj pointer to the Spacecraft
////// */
////////------------------------------------------------------------------------------
//////void DragForce::SetSpaceObject(const Integer i, GmatBase *obj)
//////{
//////   unsigned parmNumber = (unsigned)(i);
//////   
//////   if (parmNumber < scObjs.size())
//////      scObjs[i] = obj;
//////   else
//////      scObjs.push_back(obj);
//////}

//------------------------------------------------------------------------------
// bool DragForce::AttitudeAffectsDynamics()
//------------------------------------------------------------------------------
/**
 * Detects if a the DragForce is dependant on its attitude
 *
 * @return true if the model's dynamics are affected by its attitude,
 *         false if it does not
 */
//------------------------------------------------------------------------------
bool DragForce::AttitudeAffectsDynamics()
{
   return (dragShapeModelIndex == ShapeModel::SPAD_FILE_MODEL);
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Allocates memory and performs other drag force initialization.
 */
//------------------------------------------------------------------------------
bool DragForce::Initialize()
{
#ifdef DEBUG_INITIALIZE
	MessageInterface::ShowMessage("Start DragForce::Initialize():  drag force <'%s',%p>\n", GetName().c_str(), this);
//	MessageInterface::ShowMessage("   densityModel = '%s'\n", densityModel.c_str());
//	MessageInterface::ShowMessage("   inputFile    = '%s'\n", inputFile.c_str());
#endif

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Entered DragForce::Initialize()\n";
   #endif

   bool retval = PhysicalModel::Initialize();
    
   if (retval)
   {
      orbitDimension = 6 * satCount;
      
      if (dragState)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (dragState, "dragState", "DragForce::Initialize()",
             "deleting dragState[orbitDimension]", this);
         #endif
         delete [] dragState;
      }
      
      dragState = new Real[orbitDimension];
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (dragState, "dragState", "DragForce::Initialize()",
          "dragState = new Real[orbitDimension]", this);
      #endif
      
      if (satCount <= 0)
         throw ODEModelException("Drag called with orbit dimension zero");

      if (density)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (density, "density", "DragForce::Initialize()",
             "deleting density[satCount]", this);
         #endif
         delete [] density;
         density = NULL;
      }
      
      if (prefactor)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (prefactor, "prefactor", "DragForce::Initialize()",
             "deleting prefactor[satCount]", this);
         #endif
         delete [] prefactor;
         prefactor = NULL;
      }
      
      // Set up density even if only used for a parameter
      if (satCount > 0)
         density   = new Real[satCount];
      else
         density   = new Real[1];
      
      if (satCount > 0)
         prefactor = new Real[satCount];
      else
         prefactor = new Real[1];

      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (density, "density", "DragForce::Initialize()",
          "density = new Real[satCount]", this);
      MemoryTracker::Instance()->Add
         (prefactor, "prefactor", "DragForce::Initialize()",
          "prefactor = new Real[satCount]", this);
      #endif
      
      // Set the atmosphere model
      if (solarSystem)
      {
         sun = solarSystem->GetBody(GmatSolarSystemDefaults::SUN_NAME);
         if (!sun)
            throw ODEModelException("The Sun is not in the solar system");
           
         centralBody = solarSystem->GetBody(bodyName);
         body = solarSystem->GetBody(bodyName);
   
         if (!centralBody)
            throw ODEModelException(
               "The central body (for Drag) is not in solar system");

         // If central body's atmosphere is not defined, then set central body's
         // atmosphere to be internal atmosphere
         std::string modelBodyIsUsing = centralBody->GetAtmosphereModelType();
         if (modelBodyIsUsing == "Undefined")
         {
            if (internalAtmos)
            {
               AtmosphereModel *amCloned = (AtmosphereModel*)internalAtmos->Clone();
               centralBody->SetAtmosphereModelType(atmosphereType);
               centralBody->SetAtmosphereModel(amCloned);
            }
         }

         // Set value for atmos
         if (useExternalAtmosphere)
         {
            atmos = centralBody->GetAtmosphereModel();
         }
         else
         {
            if ((atmosphereType == "BodyDefault") || (atmosphereType == "MarsGRAM2005"))
               atmos = centralBody->GetAtmosphereModel();
            else
               atmos = internalAtmos;
            if (!atmos)
               throw ODEModelException("Atmosphere model not defined");
            
         }
         
         if (atmos)
         {
            // Verify DragForce's central body is the same as atmosphere's central body 
            if (body == NULL)
               throw ODEModelException("No central body is defined for DragForce\n");

            atmos->SetSolarSystem(solarSystem);
            atmos->SetCentralBody(centralBody);

            // Only verify central body name after it was set
            if (body->GetName() != atmos->GetCentralBodyName())
               throw ODEModelException("The force model's central body ('" +
                  body->GetName() + "') and the Atmosphere model's central body ('" +
                  atmos->GetCentralBodyName() + "')are different\n");

            atmos->SetSunVector(sunLoc);
            atmos->SetCentralBodyVector(cbLoc);
            atmos->SetUpdateParameters(wUpdateInterval, wUpdateEpoch);
            atmos->SetKpApConversionMethod(kpApConversion);

            angVel = atmos->GetAngularVelocity();
            hasWindModel = atmos->HasWindModel();

            F107ID = atmos->GetParameterID("F107");
            F107AID = atmos->GetParameterID("F107A");
            KPID = atmos->GetParameterID("MagneticIndex");
            cssiWFileID = atmos->GetParameterID("CSSISpaceWeatherFile");
            schattenWFileID = atmos->GetParameterID("SchattenFile");

            if (F107ID < 0)
               throw ODEModelException("Atmosphere model initialization is incomplete");
            atmos->SetRealParameter(F107ID, fluxF107);
            atmos->SetRealParameter(F107AID, fluxF107A);
            atmos->SetRealParameter(KPID, kp);

            // Set the file names, possibly with path prefixes
            FileManager *fm = FileManager::Instance();

            std::string weatherfile = cssiWFile;
            if (fm->DoesFileExist(weatherfile) == false)
               weatherfile = fluxPath + cssiWFile;
            if (fm->DoesFileExist(weatherfile) == false)
               throw ODEModelException("Cannot open the CSSI space weather file " +
                     cssiWFile + ", nor the file at the location " + weatherfile);
            atmos->SetStringParameter(cssiWFileID, weatherfile);

            #ifdef DEBUG_FLUX_FILE
               MessageInterface::ShowMessage("CSSI File setting: %s\n",
                     atmos->GetStringParameter(cssiWFileID).c_str());
            #endif

            weatherfile = schattenWFile;
            if (fm->DoesFileExist(weatherfile) == false)
               weatherfile = fluxPath + schattenWFile;
            if (fm->DoesFileExist(weatherfile) == false)
               throw ODEModelException("Cannot open the predicted space weather file " +
                     schattenWFile + ", nor the file at the location " + weatherfile);
            atmos->SetStringParameter(schattenWFileID, weatherfile);

            #ifdef DEBUG_FLUX_FILE
               MessageInterface::ShowMessage("Schatten File setting: %s\n",
                     atmos->GetStringParameter(schattenWFileID).c_str());
            #endif

            if (cbFixed != NULL)
               atmos->SetFixedCoordinateSystem(cbFixed);
            if (internalCoordSystem != NULL)
               atmos->SetInternalCoordSystem(internalCoordSystem);
            if (atmos->GetCbJ2000CoordinateSystem() == NULL)
            {
               // Create an cbJ2000 coordinate system if it is not ready set in atmos
               if (centralBody != NULL)
               {
                  CoordinateSystem* cbJ2000 = CoordinateSystem::CreateLocalCoordinateSystem("cbJ2000", "MJ2000Eq",
                  centralBody, NULL, NULL, centralBody, solarSystem);
                  atmos->SetCbJ2000CoordinateSystem(cbJ2000, true);
               }
            }

			   try
			   {
                  // Try "InputFile" first because some models may use this field
                  // but not DensityModel
                  atmos->SetStringParameter("InputFile", inputFile);
                  atmos->SetStringParameter("DensityModel", densityModel);
            } catch (...){}

			   atmos->Initialize();
            // Set the source flags: constants, files, etc
            atmos->SetInputSource(historicWSource, predictedWSource);
            atmos->SetSchattenFlags(schattenTimingModel, schattenErrorModel);
         }
         else
         {
            if (atmosphereType != "BodyDefault")
            {
               std::string msg = "Could not create ";
               msg += atmosphereType;
               msg += " atmosphere model";
               throw ODEModelException(msg);
            }
         }
      }
   }
    
   firedOnce = false;

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Leaving DragForce::Initialize()\n";
   #endif

#ifdef DEBUG_INITIALIZE
	MessageInterface::ShowMessage("******* Script for <%s,%p>:\n%s\n", GetName().c_str(), this, GetGeneratingString().c_str());
	MessageInterface::ShowMessage("   densityModel = '%s'\n", densityModel.c_str());
	MessageInterface::ShowMessage("   inputFile    = '%s'\n", inputFile.c_str());
	MessageInterface::ShowMessage("End DragForce::Initialize():  drag force <%p,'%s'>\n\n", this, GetName().c_str());
#endif
   
   isInitialized = retval;
   return retval;
}


//------------------------------------------------------------------------------
// void BuildPrefactors(const std::string &forModel = "Spherical")
//------------------------------------------------------------------------------
/**
 * Builds drag prefactors prior to modeling the force.
 * 
 * The drag prefactor is given by
 * 
 *     \f[F_d = -\frac{1}{2} \frac{C_d A}{m} \f]
 * 
 * The atmospheric model classes provide densities in kg/m^3.  Since we need
 * accelerations in km/s^2, there is an extra factor of 1000 in the prefactor
 * to account for the units needed in the for the accelerations.  Areas are 
 * expressed in units of m^2, and the mass is in kg.
 */
//------------------------------------------------------------------------------
void DragForce::BuildPrefactors(const std::string &forModel)
{
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Building prefactors for " << satCount <<" Spacecraft\n";
   #endif
   
   if (!forceOrigin)
      throw ODEModelException(
         "Cannot use drag force: force model origin not set.");
    
   /// @todo: for maneuvering, need to be updating mass here
   for (Integer i = 0; i < satCount; ++i)
   {
      if (mass.size() < (unsigned)(i+1))
         throw ODEModelException("Spacecraft not set correctly");
      if (mass[i] <= 0.0)
      {
         std::string errorMsg = "Spacecraft ";
         errorMsg += i;
         errorMsg += " has non-physical mass; Drag modeling cannot be used.";
         throw ODEModelException(errorMsg);
      }
      if (forModel == "Spherical")
      {
         // Note: Prefactor is scaled to account for density in kg / m^3 (*1000/2)
         prefactor[i] = -500.0 * dragCoeff[i] * area[i] / mass[i];
      }
      else // SPAD
      {
         // Note: Prefactor is scaled to account for density in kg / m^3 (*1000/2)
         prefactor[i] = -500.0 / mass[i];
      }

      #ifdef DEBUG_DRAGFORCE_DENSITY
         dragdata << "Prefactor data\n   Spacecraft "
                  << i
                  << "\n      Cd = " << dragCoeff[i]
                  << "\n      Area = " << area[i]
                  << "\n      Mass = " << mass[i]
                  << "\n      Prefactor = " << prefactor[i]  << "\n";

         MessageInterface::ShowMessage(
            "Prefactor data\n   Spacecraft %d\n      Cd = %lf\n      "
            "Area = %lf\n      Mass = %lf\n      Prefactor = %lf\n", i,
            dragCoeff[i], area[i], mass[i], prefactor[i]);
      #endif
    }
}


//------------------------------------------------------------------------------
// void TranslateOrigin(const Real *state, const Real now)
//------------------------------------------------------------------------------
/**
 * Used when the force model origin is not coincident with the drag body.
 * 
 * @param state The state vector, in MJ2000Eq coordinates centered at the force
 *              model origin.
 * @param now   The epoch of the calculations, in A.1 Modified Julian format.
 *
 * @note The current implementation throws if the force model origin is not the
 *       same as the body producing drag.
 */
//------------------------------------------------------------------------------
void DragForce::TranslateOrigin(const Real *state, const Real now)
{
   memcpy(dragState, &(state[cartIndex]),  orbitDimension * sizeof(Real));
   if (forceOrigin != centralBody)
   {
      throw ODEModelException(
         "DragForce::TranslateOrigin: Drag forces only work when the force "
         "model origin is the same as the body with the atmosphere producing "
         "drag in the current GMAT build.");
      Rvector6 cbrv = centralBody->GetMJ2000State(now);
      Rvector6 forv = forceOrigin->GetMJ2000State(now);
      Rvector6 delta = cbrv - forv;

      Integer i6;
      for (Integer i = 0; i < satCount; ++i)
      {
         i6 = i * 6;  //stateSize;
         dragState[i6]   -= delta[0];
         dragState[i6+1] -= delta[1];
         dragState[i6+2] -= delta[2];
         dragState[i6+3] -= delta[3];
         dragState[i6+4] -= delta[4];
         dragState[i6+5] -= delta[5];
      }
   }
}


//------------------------------------------------------------------------------
// bool GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Method invoked to calculate accelerations due to drag
 * 
 * This method calculates the acceleration due to drag using the formula
 * 
 * \f[\vec a = - {1\over 2} {{C_d A} \over {m}} \rho v_{rel}^2 \hat v_{rel}\f]
 *
 * @param dt            Additional time increment for the derivatitive 
 *                      calculation; defaults to 0.
 * @param state         Pointer to the current state data.  This can differ
 *                      from the PhysicalModel state if the subscribing
 *                      integrator samples other state values during 
 *                      propagation.  (For example, the Runge-Kutta integrators 
 *                      do this during the stage calculations.)
 * @param order         The order of the derivative to be taken (first 
 *                      derivative, second derivative, etc)
 *
 * @return              true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool DragForce::GetDerivatives(Real *state, Real dt, Integer order, 
      const Integer id)
{
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Entered DragForce::GetDerivatives()\n";
   #endif

   if ((Integer)scObjs.size() != satCount)
   {
      std::stringstream msg;
      msg << "Mismatch between satellite count (" << satCount
      << ") and object count (" << scObjs.size() << ")";
      throw ODEModelException(msg.str());
   }

   Integer i, i6, ix, j6;
   Real vRelative[3], vRelMag, factor;

   // Prepare EstimatedParameter variables
   Integer cdFogmIndex = 0;
   Integer adsfFogmIndex = cdFogmIndex + cdEpsilonCount;
   Integer fogmCount = cdEpsilonCount + atmosDensityEpsilonCount;
   Real *beta = new Real[fogmCount];
   bool *useFOGM = new bool[fogmCount];

   for (Integer i = 0; i < fogmCount; ++i)
   {
      beta[i] = 0.0;
      useFOGM[i] = false;
   }

   if (fillCdEpsilon)
   {
      for (Integer i = 0; i < satCount; ++i)
      {
         cdEpsilon[i] = state[cdEpsilonIndex + i];
         dragCoeff[i] = cdInitial[i] * (1 + cdEpsilon[i]);

         ObjectArray estParamObjs = scObjs[i]->GetRefObjectArray(GmatType::GetTypeId("EstimatedParameter"));

         for (Integer j = 0; j < estParamObjs.size(); ++j)
         {
            GmatBase *refObj = estParamObjs[j]->GetRefObject(GmatType::GetTypeId("EstimatedParameterModel"), "");
            if (refObj && refObj->IsOfType("FirstOrderGaussMarkov")
                && refObj->GetStringParameter("SolveFor") == "Cd")
            {
               useFOGM[cdFogmIndex + i] = true;
               beta[cdFogmIndex + i] = GmatMathUtil::Ln(2.0) / estParamObjs[j]->GetRealParameter("HalfLife");
               beta[cdFogmIndex + i] *= direction;
               break;
            }
         }

         if (useFOGM[cdFogmIndex + i])
         {
            deriv[cdEpsilonIndex + i] = -beta[cdFogmIndex + i] * cdEpsilon[i];
         }
      }
   }

   if (fillAtmosDensityEpsilon)
   {
      for (Integer i = 0; i < satCount; ++i)
      {
         adsfEpsilon[i] = state[atmosDensityEpsilonIndex + i];
         atmosDensityScaleFactor[i] = adsfInitial[i] * (1 + adsfEpsilon[i]);

         ObjectArray estParamObjs = scObjs[i]->GetRefObjectArray(GmatType::GetTypeId("EstimatedParameter"));

         for (Integer j = 0; j < estParamObjs.size(); ++j)
         {
            GmatBase *refObj = estParamObjs[j]->GetRefObject(GmatType::GetTypeId("EstimatedParameterModel"), "");
            if (refObj && refObj->IsOfType("FirstOrderGaussMarkov")
                && refObj->GetStringParameter("SolveFor") == "AtmosDensityScaleFactor")
            {
               useFOGM[adsfFogmIndex + i] = true;
               beta[adsfFogmIndex + i] = GmatMathUtil::Ln(2.0) / estParamObjs[j]->GetRealParameter("HalfLife");
               beta[adsfFogmIndex + i] *= direction;
               break;
            }
         }

         if (useFOGM[adsfFogmIndex + i])
         {
            deriv[atmosDensityEpsilonIndex + i] = -beta[adsfFogmIndex + i] * adsfEpsilon[i];
         }
      }
   }

   if (mass.size() > 0)
      BuildPrefactors(dragShapeModel);
   else
   {
      if (mass.size() == 0)
      {
         for (Integer i = 0; i < satCount; ++i)
         {
            if (!forceOrigin)
               throw ODEModelException(
                  "Cannot use drag force: force model origin not set.");

            #ifdef DEBUG_DRAGFORCE_DENSITY
                dragdata << "Using default prefactors for " << satCount
                         << " Spacecraft\n";
            #endif
            prefactor[i] = -0.5 * 2.2 * 15.0 / 875.0; // Dummy up the product
         }
      }
   }

   firedOnce = true;
    
   // First translate to the drag body from the force model origin
   Real now = epoch + (elapsedTime + dt) / GmatTimeConstants::SECS_PER_DAY;
   TranslateOrigin(state, now);

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "density[0] = " << density[0] << "\n";
   #endif

   #ifdef DEBUG_DRAGFORCE_EPOCH
      MessageInterface::ShowMessage("Drag epoch = %16.11lf\n", now);
   
   #endif

   #ifdef DEBUG_ANGVEL
      MessageInterface::ShowMessage("Ang Vel Vector: [%.12le %.12le "
            "%.12le]\n", angVel[0], angVel[1], angVel[2]);
   #endif

   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Looking up density\n";
   #endif

   GetDensity(dragState, now);
   Real wind[6];
   
   Rvector3 spadArea;

   #ifdef DUMP_DENSITY
      bool writeDensity = true;

      #ifdef DUMP_INITIAL_STATE_DERIVATIVES_ONLY
         if (dt != 0.0)
            writeDensity = false;
      #endif

      if (writeDensity)
      {
         MessageInterface::ShowMessage("Density:  %.12lf", now);

         for (Integer m = 0; m < satCount; ++m)
            MessageInterface::ShowMessage("  %.15le", density[m]);
         MessageInterface::ShowMessage("\n");
      }
   #endif

   if (fillCartesian)
   {
      for (i = 0; i < satCount; ++i)
      {
         i6 = i * 6;
         j6 = cartIndex + i * 6;
         #ifdef DEBUG_DRAGFORCE_DENSITY
            dragdata << "Spacecraft " << (i+1) << ": ";
         #endif
   
         
         if (hasWindModel)
         {
            // v_rel = v - w x R
            atmos->Wind(&(dragState[i6]), wind, now, 1);
            vRelative[0] = dragState[i6+3] - wind[3];
            vRelative[1] = dragState[i6+4] - wind[4];
            vRelative[2] = dragState[i6+5] - wind[5];
            vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                           vRelative[2]*vRelative[2]);

            #ifdef COMPAREAPPROACHES
               Real vRel[3];
               vRel[0] = dragState[i6+3] -
                              (angVel[1]*dragState[i6+2] - angVel[2]*dragState[i6+1]);
               vRel[1] = dragState[i6+4] -
                              (angVel[2]*dragState[ i6 ] - angVel[0]*dragState[i6+2]);
               vRel[2] = dragState[i6+5] -
                              (angVel[0]*dragState[i6+1] - angVel[1]*dragState[ i6 ]);
               MessageInterface::ShowMessage("v-Local wind: %lf %lf %lf\n",
                     dragState[i6+3] - wind[3], dragState[i6+4] - wind[4], dragState[i6+5] - wind[5]);
               MessageInterface::ShowMessage("VRel: %lf %lf %lf\n",
                     vRelative[0], vRelative[1], vRelative[2]);
            #endif
         }
         else
         {
            // v_rel = v - w x R
            vRelative[0] = dragState[i6+3] -
                           (angVel[1]*dragState[i6+2] - angVel[2]*dragState[i6+1]);
            vRelative[1] = dragState[i6+4] -
                           (angVel[2]*dragState[ i6 ] - angVel[0]*dragState[i6+2]);
            vRelative[2] = dragState[i6+5] -
                           (angVel[0]*dragState[i6+1] - angVel[1]*dragState[ i6 ]);
            vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                           vRelative[2]*vRelative[2]);
         }

         // Add density to the prefactor computation 
         factor = prefactor[i] * density[i];
   
         #ifdef DEBUG_DERIVATIVES_FOR_SPACECRAFT
            MessageInterface::ShowMessage("GD4P Drag: Prefactor = %.12lf, "
               "density = %.12le\n", prefactor[i], density[i]);
         #endif

         if (dragShapeModelIndex == ShapeModel::SPHERICAL_MODEL)
         {
            if (order == 1)
            {
               // Do dv/dt first, in case deriv = state
               deriv[3+j6] = factor * vRelMag * vRelative[0];// - a_indirect[0];
               deriv[4+j6] = factor * vRelMag * vRelative[1];// - a_indirect[1];
               deriv[5+j6] = factor * vRelMag * vRelative[2];// - a_indirect[2];

               // dr/dt = v term not built from drag force
               deriv[j6]   = 
               deriv[1+j6] = 
               deriv[2+j6] = 0.0;
      
               #ifdef DEBUG_DRAGFORCE_DENSITY
   //               for (Integer m = 0; m < satCount; ++m)
   //                  dragdata << "   Drag Accel: "
   //                           << deriv[3+i6] << "  "
   //                           << deriv[4+i6] << "  "
   //                           << deriv[5+i6] << "\n";
                  for (Integer m = 0; m < satCount; ++m)
                  {
                     MessageInterface::ShowMessage(
                        "   Position:   %16.9le  %16.9le  %16.9le\n",
                        state[i6], state[i6+1], state[i6+2]);
                     MessageInterface::ShowMessage(
                        "   Velocity:   %16.9le  %16.9le  %16.9le\n",
                        state[i6+3], state[i6+4], state[i6+5]);
                     MessageInterface::ShowMessage(
                        "   Drag Accel: %16.9le  %16.9le  %16.9le\n",
                        deriv[3+i6], deriv[4+i6], deriv[5+i6]);
                     MessageInterface::ShowMessage(
                        "   Density:    %16.9le\n", density[i]);
                  }
               #endif

            #ifdef DEBUG_NAN_CONDITIONS
               for (Integer j = 0; j < 6; ++j)
               if (GmatMathUtil::IsNaN(deriv[j6+j]))
               {
                  MessageInterface::ShowMessage("NAN found in drag force"
                        " element %d, Value is %lf at epoch %.12lf\n", j,
                        deriv[j], now);
                  MessageInterface::ShowMessage(
                     "   Position:   %16.9le  %16.9le  %16.9le\n",
                     state[i6], state[i6+1], state[i6+2]);
                  MessageInterface::ShowMessage(
                     "   Velocity:   %16.9le  %16.9le  %16.9le\n",
                     state[i6+3], state[i6+4], state[i6+5]);
                  MessageInterface::ShowMessage(
                     "   Drag Accel: %16.9le  %16.9le  %16.9le\n",
                     deriv[3+i6], deriv[4+i6], deriv[5+i6]);
                  MessageInterface::ShowMessage(
                     "   Density:    %16.9le\n", density[i]);
               }
            #endif

            }
            else
            {
               // Feed accelerations to corresponding components directly for RKN
               deriv[ j6 ] = factor * vRelMag * vRelative[0];// - a_indirect[0];
               deriv[1+j6] = factor * vRelMag * vRelative[1];// - a_indirect[1];
               deriv[2+j6] = factor * vRelMag * vRelative[2];// - a_indirect[2];
               deriv[3+j6] = 0.0;
               deriv[4+j6] = 0.0;
               deriv[5+j6] = 0.0;
      
               #ifdef DEBUG_DRAGFORCE_DENSITY
                  for (Integer m = 0; m < satCount; ++m)
                     dragdata << "   Accel: "
                              << deriv[i6] << "  "
                              << deriv[1+i6] << "  "
                              << deriv[2+i6] << "\n";
                  for (Integer m = 0; m < satCount; ++m)
                  {
                     MessageInterface::ShowMessage(
                        "   Position:   %16.9le  %16.9le  %16.9le\n",
                        state[i6], state[i6+1], state[i6+2]);
                     MessageInterface::ShowMessage(
                        "    Velocity:   %16.9le  %16.9le  %16.9le\n",
                        state[i6+3], state[i6+4], state[i6+5]);
                     MessageInterface::ShowMessage(
                        "   Drag Accel: %16.9le  %16.9le  %16.9le\n",
                        deriv[i6], deriv[1+i6], deriv[2+i6]);
                     MessageInterface::ShowMessage("   Density:    %16.9le\n",
                        density[i]);
                  }
               #endif
            }
         } // if Spherical
         else if (dragShapeModelIndex == ShapeModel::SPAD_FILE_MODEL)        // "SPAD"
         {
            if (!scObjs.at(i)->IsOfType("Spacecraft"))
            {
               std::stringstream msg;
               msg << "Satellite " << scObjs.at(i)->GetName();
               msg << " is not of type Spacecraft.  SPAD Drag area cannot ";
               msg << "be obtained.\n";
               throw ODEModelException(msg.str());
            }
            Rvector3 velVec(vRelative[0], vRelative[1], vRelative[2]);
            spadArea = ((Spacecraft*) scObjs.at(i))->GetSPADDragArea(now, velVec);
            if (order == 1)
            {  // @TODO - do I need to convert to KM here???
               // Do dv/dt first, in case deriv = state
               deriv[3+j6] = factor * spadArea[0] * vRelMag * vRelMag;
               deriv[4+j6] = factor * spadArea[1] * vRelMag * vRelMag;
               deriv[5+j6] = factor * spadArea[2] * vRelMag * vRelMag;
               
               // dr/dt = v term not built from drag force
               deriv[j6]   =
               deriv[1+j6] =
               deriv[2+j6] = 0.0;
               
            }
            else
            {  // @TODO - do I need to convert to KM here???
               // Feed accelerations to corresponding components directly for RKN
               deriv[ j6 ] = factor * spadArea[0] * vRelMag * vRelMag;
               deriv[1+j6] = factor * spadArea[1] * vRelMag * vRelMag;
               deriv[2+j6] = factor * spadArea[2] * vRelMag * vRelMag;
               deriv[3+j6] = 0.0;
               deriv[4+j6] = 0.0;
               deriv[5+j6] = 0.0;
               
            }
         }
      }

      #ifdef DUMP_DERIVATIVE
         MessageInterface::ShowMessage("State(%.12lf): [%le %le %le %le %le %le]\n",
               now, deriv[0], deriv[1], deriv[2], deriv[3], deriv[4], deriv[5]);
      #endif
   }

   if (fillSTM || fillAMatrix)
   {
      Integer iStart = (fillSTM ? stmStart : aMatrixStart);
      stmRowCount = 0;
      Integer scStmStartIndex = 0;
      for (i = 0; i < satCount; ++i)
      {
         scStmStartIndex += stmRowCount;
         Integer cdEpsilonRow = psm->GetSTMIndex(cdID, scObjs[i]) - scStmStartIndex;
         Integer adsfEpsilonRow = psm->GetSTMIndex(adsfID, scObjs[i]) - scStmStartIndex;

			StringArray sfs = scObjs[i]->GetStringArrayParameter("SolveFors");
			bool estimatingCd = false;
         bool estimatingAdsf = false;
			for (Integer j = 0; j < sfs.size(); ++j)
			{
            std::string solveFor = sfs[j];
            GmatBase *obj = scObjs[i]->GetRefObject(GmatType::GetTypeId("EstimatedParameter"), solveFor);

            if (obj)
               solveFor = obj->GetStringParameter("SolveFor");

				if ((solveFor == "Cd")|| (solveFor == "SPADDragScaleFactor"))
					estimatingCd = true;
            else if (solveFor == "AtmosDensityScaleFactor")
               estimatingAdsf = true;
			}

			// Get spacecraft object
			Spacecraft* sc = (Spacecraft*)scObjs[i];

			// Create aTilde matrix
			stmRowCount = sc->GetIntegerParameter("FullSTMRowCount");
			Integer stmSize = stmRowCount * stmRowCount;
			Real *aTilde = new Real[stmSize];
			for (Integer j = 0; j < stmRowCount; ++j)
         {
            ix = j * stmRowCount;
            for (Integer k = 0; k < stmRowCount; ++k)
               aTilde[ix+k] = 0.0;
         }

         // Build the base acceleration
         Rvector3 accel = Accelerate(i, &state[i*6], now, prefactor[0]);

         Rvector3 daccel, daccelm;
         Real pert = 1.0e-2;
         Real val;

         // Finite difference the position submatrix
         for (UnsignedInt j = 0; j < 3; ++j)
         {
            val = state[i*6 + j];
            state[i*6 + j] += pert;
            daccel = Accelerate(i, &state[i*6], now, prefactor[0]);
            ix = stmRowCount * 3 + j;

            if (useCentralDifferences)
            {
               state[i*6 + j] -= 2.0 * pert;
               daccelm = Accelerate(i, &state[i*6], now, prefactor[0]);

               #ifdef DEBUG_FINITEDIFF
                  MessageInterface::ShowMessage("R: [%le  %le  %le] - [%le  %le  %le] ==> ",
                        daccel[0], daccel[1], daccel[2], daccelm[0], daccelm[1], daccelm[2]);
               #endif

               for (UnsignedInt k = 0; k < 3; ++k)
                  aTilde[ix+k*stmRowCount] = (daccel[k] - daccelm[k]) / (2.0 * pert);

               #ifdef DEBUG_FINITEDIFF
                  MessageInterface::ShowMessage("[%le  %le  %le]\n",
                        aTilde[ix], aTilde[ix+1*stmRowCount], aTilde[ix+2*stmRowCount]);
               #endif
            }
            else
            {
               #ifdef DEBUG_FINITEDIFF
                  MessageInterface::ShowMessage("R: [%le  %le  %le] - [%le  %le  %le] ==> ",
                        daccel[0], daccel[1], daccel[2],  accel[0], accel[1], accel[2]);
               #endif

               for (UnsignedInt k = 0; k < 3; ++k)
                  aTilde[ix+k*stmRowCount] = (daccel[k] - accel[k]) / pert;

               #ifdef DEBUG_FINITEDIFF
                  MessageInterface::ShowMessage("[%le  %le  %le]\n",
                        aTilde[ix], aTilde[ix+1*stmRowCount], aTilde[ix+2*stmRowCount]);
               #endif
            }

            #ifdef DEBUG_A_MATRIX
               MessageInterface::ShowMessage("%d: [%.12le  %.12le  %.12le]\n", j,
                     aTilde[ix], aTilde[ix+1*stmRowCount], aTilde[ix+2*stmRowCount]);
            #endif

            state[i*6 + j] = val;
         }
         // Next handle the velocity submatrix
         if (finiteDifferenceDv)
         {
            pert = 1.0e-6;
            for (UnsignedInt j = 0; j < 3; ++j)
            {
               val = state[i*6 + j+3];
               state[i*6 + j+3] += pert;
               daccel = Accelerate(i, &state[i*6], now, prefactor[0]);
               ix = stmRowCount * 3 + j+3;

               if (useCentralDifferences)
               {
                  state[i*6 + j+3] -= 2.0 * pert;
                  daccelm = Accelerate(i, &state[i*6], now, prefactor[0]);

                  #ifdef DEBUG_FINITEDIFF
                     MessageInterface::ShowMessage("V: [%le  %le  %le] - [%le  %le  %le] ==> ",
                           daccel[0], daccel[1], daccel[2], daccelm[0], daccelm[1], daccelm[2]);
                  #endif

                  for (UnsignedInt k = 0; k < 3; ++k)
                     aTilde[ix+k*stmRowCount] = (daccel[k] - daccelm[k]) / (2.0 * pert);

                  #ifdef DEBUG_FINITEDIFF
                     MessageInterface::ShowMessage("[%le  %le  %le]\n",
                           aTilde[ix], aTilde[ix+1*stmRowCount], aTilde[ix+2*stmRowCount]);
                  #endif
               }
               else
               {
                  #ifdef DEBUG_FINITEDIFF
                     MessageInterface::ShowMessage("V: [%le  %le  %le] - [%le  %le  %le] ==> ",
                           daccel[0], daccel[1], daccel[2],  accel[0], accel[1], accel[2]);
                  #endif

                  for (UnsignedInt k = 0; k < 3; ++k)
                     aTilde[ix+k*stmRowCount] = (daccel[k] - accel[k]) / pert;

                  #ifdef DEBUG_FINITEDIFF
                     MessageInterface::ShowMessage("[%le  %le  %le]\n",
                           aTilde[ix], aTilde[ix+1*stmRowCount], aTilde[ix+2*stmRowCount]);
                  #endif
               }
               #ifdef DEBUG_A_MATRIX
                  MessageInterface::ShowMessage("%d: [%.12le  %.12le  %.12le]\n",
                        j+3, aTilde[ix], aTilde[ix+1*stmRowCount], aTilde[ix+2*stmRowCount]);
               #endif
               state[i*6 + j+3] = val;
            }
         }
         else
         {
            throw ODEModelException("Analytic differencing for drag model A-matrix "
                  "d(accel)/dv terms in not yet implemented");
         }

         if (estimatingCd)
         {
            for (UnsignedInt j = 0; j < 3; ++j)
            {
               ix = stmRowCount * (3 + j);
               aTilde[ix + cdEpsilonRow] = deriv[i * 6 + 3 + j] / (1 + cdEpsilon[i]);
               #ifdef DEBUG_A_MATRIX
                  MessageInterface::ShowMessage("Cd deriv %d: %.12le\n", j,
                        aTilde[ix+cdEpsilonRow]);
               #endif
            }

            if (fillCdEpsilon && useFOGM[cdFogmIndex + i])
            {
               ix = stmRowCount * (cdEpsilonRow);
               aTilde[ix + cdEpsilonRow] = -beta[cdFogmIndex + i];
            }
         }

         if (estimatingAdsf)
         {
            for (UnsignedInt j = 0; j < 3; ++j)
            {
               ix = stmRowCount * (3 + j);
               aTilde[ix + adsfEpsilonRow] = deriv[i * 6 + 3 + j] / (1 + adsfEpsilon[i]);
               #ifdef DEBUG_A_MATRIX
                  MessageInterface::ShowMessage("Atmosphere density scale factor deriv %d: %.12le\n", j,
                        aTilde[ix+ adsfEpsilonRow]);
               #endif
            }

            if (fillAtmosDensityEpsilon && useFOGM[adsfFogmIndex + i])
            {
               ix = stmRowCount * (adsfEpsilonRow);
               aTilde[ix + adsfEpsilonRow] = -beta[adsfFogmIndex + i];
            }
         }


         Integer element;
         for (Integer j = 0; j < stmRowCount; ++j)
         {
            for (Integer k = 0; k < stmRowCount; ++k)
            {
               element = j * stmRowCount + k;
               deriv[iStart+element] = aTilde[element];
            }
         }

         #ifdef DUMP_DERIVATIVE
            MessageInterface::ShowMessage("Drag Force A-matrix at epoch %.12lf for state "
                  "[%lf %lf %lf %.12lf %.12lf %.12lf]:\n",now, state[0],
                  state[1], state[2], state[3], state[4], state[5]);
            for (Integer m = 0; m < stmRowCount; ++m)
            {
               MessageInterface::ShowMessage("      ");
               for (Integer n = 0; n < stmRowCount; ++n)
               {
                  if (n > 0)
                     MessageInterface::ShowMessage(", ");
                  MessageInterface::ShowMessage("%le", aTilde[m*stmRowCount+n]);
               }
               MessageInterface::ShowMessage("\n");
            }
            MessageInterface::ShowMessage("--------------------------------------"
                  "-----------------------------------------------------------\n");
         #endif

         #ifdef DEBUG_FINITEDIFF
            MessageInterface::ShowMessage("Drag derivatives:\n   State: [");
            Integer p,q;
            for (p = 0; p < 6; ++p)
               MessageInterface::ShowMessage(" %le ", deriv[p]);
            MessageInterface::ShowMessage("]\n\n   A-Matrix: [");
            for (p = 0; p < stmRowCount; ++p)
            {
               if (p > 0)
                  MessageInterface::ShowMessage(";\n              ");
               for (q = 0; q < stmRowCount; ++q)
               {
                  MessageInterface::ShowMessage(" %le ", deriv[iStart + p*stmRowCount+q]);
               }
            }
            MessageInterface::ShowMessage("]\n\n");
         #endif
			
         delete[] aTilde;
			// Handle varied size of STMs
			iStart = iStart + stmSize;
      }
   }

   if (fillMassJacobian)
   {
      for (Integer i = 0; i < satCount; ++i)
      {
         i6 = i * 6;
         for (Integer k = 0; k < 6; ++k)
            massJacobian[i6+k] = -deriv[i6+k] / mass[i];
      }
   }

   #ifdef DEBUG_SHOW_Force
      static int iter = 0;
      if (iter == 0)
         MessageInterface::ShowMessage("Drag dv: [%le %le %le]\n", deriv[3],
               deriv[4], deriv[5]);
      if (++iter == 16)
         iter = 0;
   #endif

   delete[] beta;
   delete[] useFOGM;

   return true;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft* sc)
//------------------------------------------------------------------------------
/**
 * Retrieves the Cartesian state vector of derivatives w.r.t. time
 *
 * @param sc The spacecraft that holds the state vector
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
Rvector6 DragForce::GetDerivativesForSpacecraft(Spacecraft* sc)
{
   Rvector6 dv;

   if (atmos == NULL)
      Initialize();

   Real vRelative[3], vRelMag, factor, mass, cd, area, adsf;
   mass = sc->GetRealParameter("TotalMass");
   cd   = sc->GetRealParameter("Cd");
   area = sc->GetRealParameter("DragArea");
   adsf = sc->GetRealParameter("AtmosDensityScaleFactor");

   Real prefactor = 1;
   if (dragShapeModelIndex == ShapeModel::SPHERICAL_MODEL)
   {
      // Note: Prefactor is scaled to account for density in kg / m^3
      prefactor = -500.0 * cd * area / mass;
   }
   else if (dragShapeModelIndex == ShapeModel::SPAD_FILE_MODEL)    // SPAD
   {
      prefactor = -500.0 / mass;  // area and Cd are from file
   }

   // First translate to the drag body from the force model origin
   Real *j2kState = sc->GetState().GetState();
   Real state[6];
   Real now = sc->GetEpoch();
   GmatTime nowgt = sc->GetEpochGT();

   if (hasPrecisionTime)
      BuildModelStateGT(nowgt, state, j2kState);
   else
      BuildModelState(now, state, j2kState);

   Real dens = 0.0;
   if (atmos != NULL)
   {
      if (hasPrecisionTime) 
         atmos->Density(state, &dens, nowgt.GetMjd(), 1);
      else
         atmos->Density(state, &dens, now, 1);

      dens *= adsf;

      if (angVel == NULL)
         angVel = atmos->GetAngularVelocity();
   }
   else
      throw ODEModelException("Atmospheric model is NULL in the DragForce");

   if (hasWindModel)
   {
      Real wind[6];

      // v_rel = v - w x R
      if (hasPrecisionTime)
         atmos->Wind(&(state[0]), wind, nowgt.GetMjd(), 1);
      else
         atmos->Wind(&(state[0]), wind, now, 1);

      vRelative[0] = state[3] - wind[3];
      vRelative[1] = state[4] - wind[4];
      vRelative[2] = state[5] - wind[5];
      vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                     vRelative[2]*vRelative[2]);
   }
   else
   {
      // v_rel = v - w x R
      vRelative[0] = state[3] -
                     (angVel[1]*state[2] - angVel[2]*state[1]);
      vRelative[1] = state[4] -
                     (angVel[2]*state[0] - angVel[0]*state[2]);
      vRelative[2] = state[5] -
                     (angVel[0]*state[1] - angVel[1]*state[0]);
      vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                     vRelative[2]*vRelative[2]);
   }

   factor = prefactor * dens;

   #ifdef DEBUG_DERIVATIVES_FOR_SPACECRAFT
      MessageInterface::ShowMessage("GD4S Drag: Prefactor = %.12lf, "
         "density = %.12le\n", prefactor, dens);
   #endif

   if (dragShapeModelIndex == ShapeModel::SPHERICAL_MODEL)
   {
      // Do dv/dt first, in case deriv = state
      dv[3] = factor * vRelMag * vRelative[0];// - a_indirect[0];
      dv[4] = factor * vRelMag * vRelative[1];// - a_indirect[1];
      dv[5] = factor * vRelMag * vRelative[2];// - a_indirect[2];

      // dr/dt = v term not built from drag force
      dv[0] =
      dv[1] =
      dv[2] = 0.0;
   }
   else if (dragShapeModelIndex == ShapeModel::SPAD_FILE_MODEL)   // SPAD
   {
      Rvector3 velVec(vRelative[0], vRelative[1], vRelative[2]);
      Rvector3 spadArea = sc->GetSPADDragArea(now, velVec);
      
      dv[3] = factor * spadArea[0] * vRelMag * vRelative[0];
      dv[4] = factor * spadArea[1] * vRelMag * vRelative[1];
      dv[5] = factor * spadArea[2] * vRelMag * vRelative[2];

      // dr/dt = v term not built from drag force
      dv[0] =
      dv[1] =
      dv[2] = 0.0;
   }

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage("Drag Accel: [%le %le %le]\n", dv[3],
               dv[4], dv[5]);
   #endif

   return dv;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor for the string names for the parameters
 *
 * @param      id    ID for the parameter of interest.
 * 
 * @return     text identifier for the parameter.
 */
//------------------------------------------------------------------------------
std::string DragForce::GetParameterText(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < DragForceParamCount)
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   else
      return PhysicalModel::GetParameterText(id);   
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Accessor for the string IDs for the parameters
 *
 * @param      str    text identifier for the parameter.
 * 
 * @return     ID for the parameter of interest.
 */
//------------------------------------------------------------------------------
Integer DragForce::GetParameterID(const std::string &str) const
{
   for (int i = PhysicalModelParamCount; i < DragForceParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - PhysicalModelParamCount])
         return i;
   }
   
   return PhysicalModel::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor for the data types for the parameters
 *
 * @param      id    ID for the parameter of interest.
 * 
 * @return     data type for the parameter of interest.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType DragForce::GetParameterType(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < DragForceParamCount)
      return PARAMETER_TYPE[id - PhysicalModelParamCount];
   else
      return PhysicalModel::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor for the string description of the data types for the parameters
 *
 * @param      id    ID for the parameter of interest.
 * 
 * @return     string describing the data type for the parameter of interest.
 */
//------------------------------------------------------------------------------
std::string DragForce::GetParameterTypeString(const Integer id) const
{
   if (id >= PhysicalModelParamCount && id < DragForceParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
bool DragForce::IsParameterReadOnly(const Integer id) const
{
   if (id == FLUX || id == AVERAGE_FLUX || id == MAGNETIC_INDEX ||
       id == HISTORIC_WEATHER_SOURCE || id == PREDICTED_WEATHER_SOURCE ||
       id == CSSI_WEATHER_FILE || id == SCHATTEN_WEATHER_FILE ||
       id == SCHATTEN_ERROR_MODEL || id == SCHATTEN_TIMING_MODEL)
   {
      if (atmosphereType == "Exponential")
         return true;
      else
      {
         if (bodyName == "Earth")
            return false;

         // Only support the CSSI file and Schatten file at the Earth
         if (id == FLUX || id == AVERAGE_FLUX || id == MAGNETIC_INDEX)
            return false;
         return true;
      }
   }
   
   if (id == ATMOSPHERE_BODY || id == SOURCE_TYPE ||
       id == FIXED_COORD_SYSTEM || id == W_UPDATE_INTERVAL ||
       id == KP2AP_METHOD)
      return true;
   
   return PhysicalModel::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
bool DragForce::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

bool DragForce::WriteEmptyStringParameter(const Integer id) const
{
   if (id == HISTORIC_WEATHER_SOURCE || id == PREDICTED_WEATHER_SOURCE)
   {
      if (atmosphereType == "Exponential")
         return false;
      else
         return true;
   }

   return PhysicalModel::WriteEmptyStringParameter(id);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for the floating point parameters
 *
 * @param      id    ID for the parameter of interest.
 * 
 * @return     current value for the parameter.
 */
//------------------------------------------------------------------------------
Real DragForce::GetRealParameter(const Integer id) const
{
   if (id == FLUX)
      return fluxF107;
    
   if (id == AVERAGE_FLUX)
      return fluxF107A;
    
   if (id == MAGNETIC_INDEX)
      return kp;

   if (id == W_UPDATE_INTERVAL)
      return wUpdateInterval;

   return PhysicalModel::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real DragForce::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Write accessor for the floating point parameters
 *
 * @param      id    ID for the parameter of interest.
 * @param      value The new value for the parameter.
 * 
 * @return     current (new) value for the parameter.
 */
//------------------------------------------------------------------------------
Real DragForce::SetRealParameter(const Integer id, const Real value)
{
//   ODEModelException fme;
   if (id == FLUX)
   {
      if (value >= 0.0)
      {
         fluxF107 = value;
         if (atmos)
         {
            if (F107ID < 0)
            {
               F107ID = atmos->GetParameterID("F107");
               F107AID = atmos->GetParameterID("F107A");
               KPID = atmos->GetParameterID("MagneticIndex");
            }
            atmos->SetRealParameter(F107ID, fluxF107);
         }
      }
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ODEModelException(
            "The value of \"" + buffer.str() + "\" for field \"F107(Solar Flux)\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Real Number >= 0.0]. ");
      }
      return fluxF107;
   }
    
   if (id == AVERAGE_FLUX)
   {
      if (value >= 0.0)
      {
         fluxF107A = value;
         if (atmos)
         {
            if (F107AID < 0)
            {
               F107ID = atmos->GetParameterID("F107");
               F107AID = atmos->GetParameterID("F107A");
               KPID = atmos->GetParameterID("MagneticIndex");
            }
            atmos->SetRealParameter(F107AID, fluxF107A);
         }
      }
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ODEModelException(
            "The value of \"" + buffer.str() + "\" for field \"F107A(Average Solar Flux)\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Real Number >= 0.0]. ");
      }
      return fluxF107A;
   }
    
   if (id == MAGNETIC_INDEX)
   {
      if ((value >= 0.0) && (value <= 9.0))
      {
         kp = value;
         ap = CalculateAp(kp);
         if (atmos)
         {
            if (KPID < 0)
            {
               F107ID = atmos->GetParameterID("F107");
               F107AID = atmos->GetParameterID("F107A");
               KPID = atmos->GetParameterID("MagneticIndex");
            }
            atmos->SetRealParameter(KPID, kp);
         }
      }
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ODEModelException(
            "The value of \"" + buffer.str() + "\" for field \"Magnetic Index\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [0.0 <= Real Number <= 9.0]. ");
      }
      return kp;
   }

   if (id == W_UPDATE_INTERVAL)
   {
      wUpdateInterval = value;
      return wUpdateInterval;
   }

   return PhysicalModel::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, Real value)
//------------------------------------------------------------------------------
Real DragForce::SetRealParameter(const std::string &label, Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for the text parameters
 *
 * @param      id    ID for the parameter of interest.
 * 
 * @return     current value for the parameter.
 */
//------------------------------------------------------------------------------
std::string DragForce::GetStringParameter(const Integer id) const
{
   if (id == ATMOSPHERE_MODEL) 
      return atmosphereType;

   if (id == ATMOSPHERE_BODY)
      return bodyName;
   
   if (id == SOURCE_TYPE)
      return dataType;

   if (id == HISTORIC_WEATHER_SOURCE)
      return historicWSource;

   if (id == PREDICTED_WEATHER_SOURCE)
      return predictedWSource;
    
   if (id == CSSI_WEATHER_FILE)
      return cssiWFile;

   if (id == SCHATTEN_WEATHER_FILE)
      return schattenWFile;

   if(id == SCHATTEN_ERROR_MODEL)
      return schattenErrorModel;

   if (id == SCHATTEN_TIMING_MODEL)
      return schattenTimingModel;
   
   if (id == DRAG_MODEL)
      return dragShapeModel;
   
   if (id == INPUT_FILE)
      return inputFile;

   if (id == DENSITY_MODEL)
      return densityModel;

   if (id == FIXED_COORD_SYSTEM)
      return bodyName + "Fixed";

   return PhysicalModel::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string DragForce::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const char *value)
//------------------------------------------------------------------------------
/**
 * @see SetStringParameter(const Integer id, const std::string &value)
 */
//------------------------------------------------------------------------------
bool DragForce::SetStringParameter(const Integer id, const char *value)
{
   return SetStringParameter(id, std::string(value));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Write accessor for the text parameters.
 *
 * @param      id    ID for the parameter of interest.
 * @param      value The new value for the parameter.
 * 
 * @return     true if the parameter was set, false if the call failed.
 */
//------------------------------------------------------------------------------
bool DragForce::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_DRAGFORCE_PARAM
   MessageInterface::ShowMessage
      ("DragForce::SetStringParameter() '%s' entered, id=%d, value=%s\n",
       GetName().c_str(), id, value.c_str());
   #endif
   
   if (id == ATMOSPHERE_MODEL)
   {
      atmosphereType = value;
      
      if ((value == "") || (value == "BodyDefault"))
         useExternalAtmosphere = true;
      else
      {
         atmos = NULL;
         useExternalAtmosphere = false;
      }
      
      return true;
   }
   
   if (id == ATMOSPHERE_BODY)
   {
      if (value == "")
         return false;

      bodyName = value;
      return true;
   }
   
   
   if (id == SOURCE_TYPE)      // "File" or "Constant" for now
   {
      if ((value != "File") && (value != "Constant"))
         return false;
      dataType = value;
      return true;
   }
    
   if (id == HISTORIC_WEATHER_SOURCE)
   {
      if (atmosphereType == "Exponential")
         return false;

      if ((value == "CSSISpaceWeatherFile") ||
          (value == "ConstantFluxAndGeoMag"))
      {
         historicWSource = value;
         return true;
      }
      
      ODEModelException badVal("");
      badVal.SetDetails(errorMessageFormat.c_str(), value.c_str(),
            GetParameterText(id).c_str(),
            "'CSSISpaceWeatherFile', 'ConstantFluxAndGeoMag'");
      throw badVal;
   }

   if (id == PREDICTED_WEATHER_SOURCE)
   {
      if (atmosphereType == "Exponential")
         return false;

      if ((value == "CSSISpaceWeatherFile") ||
          (value == "ConstantFluxAndGeoMag") ||
          (value == "SchattenFile"))
      {
         predictedWSource = value;
         return true;
      }

      ODEModelException badVal("");
      badVal.SetDetails(errorMessageFormat.c_str(), value.c_str(),
            GetParameterText(id).c_str(),
            "'CSSISpaceWeatherFile', 'ConstantFluxAndGeoMag', 'SchattenFile'");
//            "'ConstantFluxAndGeoMag', 'SchattenFile'");
      throw badVal;
   }

   if (id == CSSI_WEATHER_FILE)
   {
      if (atmosphereType == "Exponential")
         return false;

      if (value == "")
         return false;

      cssiWFile = value;
      return true;
   }

   if (id == SCHATTEN_WEATHER_FILE)
   {
      if (atmosphereType == "Exponential")
         return false;

      if (value == "")
         return false;

      schattenWFile = value;
      return true;
   }

   if (id == SCHATTEN_ERROR_MODEL)
   {
      if (atmosphereType == "Exponential")
         return false;

      if ((value == "Nominal") || (value == "PlusTwoSigma") ||
          (value == "MinusTwoSigma"))
      {
         schattenErrorModel = value;
         return true;
      }

      ODEModelException badVal("");
      badVal.SetDetails(errorMessageFormat.c_str(), value.c_str(),
            GetParameterText(id).c_str(),
            "'Nominal', 'PlusTwoSigma', 'MinusTwoSigma'");
      throw badVal;
   }

   if (id == SCHATTEN_TIMING_MODEL)
   {
      if (atmosphereType == "Exponential")
         return false;
      
      if ((value == "NominalCycle") || (value == "EarlyCycle") ||
          (value == "LateCycle"))
      {
         schattenTimingModel = value;
         return true;
      }
      
      ODEModelException badVal("");
      badVal.SetDetails(errorMessageFormat.c_str(), value.c_str(),
                        GetParameterText(id).c_str(),
                        "'NominalCycle', 'EarlyCycle, 'LateCycle'");
      throw badVal;
   }
   
   if (id == DRAG_MODEL)
   {
      if (value == "Spherical")
      {
         dragShapeModel = value;
         dragShapeModelIndex = ShapeModel::SPHERICAL_MODEL;
         return true;
      }
      else if (value == "SPADFile")
      {
         dragShapeModel = value;
         dragShapeModelIndex = ShapeModel::SPAD_FILE_MODEL;
         return true;
      }

      ODEModelException odee("");
      odee.SetDetails(errorMessageFormat.c_str(),
                      value.c_str(),
                      "DragModel", "\"Spherical\" or \"SPADFile\"");
      throw odee;

      return false;
   }
   
   if (id == DENSITY_MODEL)
   {
      densityModel = value;
      try
      {
         if (atmos != NULL)
            return atmos->SetStringParameter("DensityModel", densityModel);
      }
      catch (...)
      {

      }
      return true;
   }

   if (id == INPUT_FILE)
   {
      inputFile = value;
      try
      {
         if (atmos != NULL)
            return atmos->SetStringParameter("InputFile", inputFile);
      }
      catch (...)
      {

      }
      return true;
   }

   return PhysicalModel::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
bool DragForce::SetStringParameter(const std::string &label,
                                   const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool DragForce::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
#ifdef DEBUG_DRAGFORCE_PARAM
   MessageInterface::ShowMessage(
      "DragForce::SetStringParameter() label=%s, value=%s\n", label.c_str(),
      value.c_str());
#endif
   
   return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves eumeration symbols of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return list of enumeration symbols
 */
//---------------------------------------------------------------------------
const StringArray& DragForce::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   switch (id)
   {
      case DRAG_MODEL:
         enumStrings.clear();
         enumStrings.push_back("Spherical");
         enumStrings.push_back("SPADFile");
         return enumStrings;
      default:
         return PhysicalModel::GetPropertyEnumStrings(id);
   }
}


Integer DragForce::GetIntegerParameter(const Integer id) const
{
   if (id == KP2AP_METHOD)
      return kpApConversion;

   return PhysicalModel::GetIntegerParameter(id);
}

Integer DragForce::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == KP2AP_METHOD)
   {
      kpApConversion = value;
      if (atmos)
         atmos->SetKpApConversionMethod(kpApConversion);
      return kpApConversion;
   }

   return PhysicalModel::SetIntegerParameter(id, value);
}

Integer DragForce::GetIntegerParameter(const Integer id,
      const Integer index) const
{
   return PhysicalModel::GetIntegerParameter(id, index);
}

Integer DragForce::SetIntegerParameter(const Integer id, const Integer value,
      const Integer index)
{
   return PhysicalModel::SetIntegerParameter(id, value, index);
}

Integer DragForce::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

Integer DragForce::SetIntegerParameter(const std::string &label,
      const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

Integer DragForce::GetIntegerParameter(const std::string &label,
      const Integer index) const
{
   return GetIntegerParameter(GetParameterID(label), index);
}

Integer DragForce::SetIntegerParameter(const std::string &label,
      const Integer value, const Integer index)
{
   return SetIntegerParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the HarmonicField class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* DragForce::GetRefObject(const UnsignedInt type,
                                      const std::string &name)
{
   switch (type)
   {
      case Gmat::COORDINATE_SYSTEM:
         { // Set scope for local variable
            std::string fixedCSName = bodyName + "Fixed";
            if ((cbFixed) && (name == fixedCSName))
               return cbFixed;
         }
         break;

      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return PhysicalModel::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// void SetRefObject(GmatBase *obj, const UnsignedInt type,
//                              const std::string &name)
//------------------------------------------------------------------------------
/**
 * Accessor used to set the AtmosphereModel.
 * 
 * @param obj The model.
 * @param type Type of object passed in.
 * @param name Name of the object.
 */
//------------------------------------------------------------------------------
bool DragForce::SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name)
{
   #ifdef DEBUG_DRAGFORCE_REFOBJ
   MessageInterface::ShowMessage
      ("DragForce::SetRefObject() <%p>'%s' entered, obj=<%p>'%s', name='%s'\n",
       this, GetName().c_str(), obj, obj ? obj->GetName().c_str() : "NULL",
       name.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   if (type == Gmat::ATMOSPHERE)
   {
      if (obj->GetType() != Gmat::ATMOSPHERE)
         throw ODEModelException("DragForce::SetRefObject: AtmosphereModel "
                                   "type set incorrectly.");
      SetInternalAtmosphereModel((AtmosphereModel*)obj);

      if (obj != NULL)
      {
         if (obj->IsOfType("MarsGRAM2005"))
         {
            densityModel = internalAtmos->GetStringParameter("DensityModel");
         }

         try
         {
            inputFile = internalAtmos->GetStringParameter("InputFile");
         }
         catch (...)
         {
            // Dropping through - the expoential drag meodel also uses InputFile,
            // so I'm leaving this option open for user provided models as well
         }
      }

      return true;
   }

   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (obj->GetType() != Gmat::COORDINATE_SYSTEM)
         throw ODEModelException("DragForce::SetRefObject: Coordinate System "
                                 "type set incorrectly.");

      if (((CoordinateSystem*)(obj))->AreAxesOfType("BodyFixedAxes"))
      {
         cbFixed = (CoordinateSystem*)(obj);
         if (internalAtmos != NULL)
            internalAtmos->SetFixedCoordinateSystem(cbFixed);
      }
      else
      {
         internalCoordSystem = (CoordinateSystem*)(obj);
         if (internalAtmos != NULL)
            internalAtmos->SetInternalCoordSystem(internalCoordSystem);
      }

      return true;
   }

   return PhysicalModel::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// virtual Integer GetOwnedObjectCount()
//------------------------------------------------------------------------------
Integer DragForce::GetOwnedObjectCount()
{
   // for now only 1 internal object
   if (internalAtmos != NULL)
      return 1;
   else
      return 0;
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetOwnedObject(Integer whichOne)
//------------------------------------------------------------------------------
GmatBase* DragForce::GetOwnedObject(Integer whichOne)
{
   // for now only 1 internal object
   return internalAtmos;
}


//------------------------------------------------------------------------------
// bool SetInternalAtmosphereModel(AtmosphereModel* atm)
//------------------------------------------------------------------------------
/**
 * Sets the internal atmosphere model for the DragForce.
 *
 * @param atm AtmosphereModel to use when useExternalAtmosphere is false.
 *
 * @return flag indicating success of the operation.
 */
//------------------------------------------------------------------------------
bool DragForce::SetInternalAtmosphereModel(AtmosphereModel* atm)
{
   #ifdef DEBUG_DRAGFORCE_REFOBJ
   MessageInterface::ShowMessage
      ("DragForce::SetInternalAtmosphereModel() entered, atm=<%p>, "
       "internalAtmos=<%p>\n", atm, internalAtmos);
   #endif
   
   if (atm == NULL)
   {
      #ifdef DEBUG_DRAGFORCE_REFOBJ
      MessageInterface::ShowMessage
         ("DragForce::SetInternalAtmosphereModel() returning false, atm is NULL\n");
      #endif
      return false;
   }
   
   if (internalAtmos)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (internalAtmos, "internalAtmos", "DragForce::SetInternalAtmosphereModel()",
          "deleting internal atmosphere model");
      #endif
      if (atmos == internalAtmos)
         atmos = NULL;
      delete internalAtmos;
   }
   
   internalAtmos = atm;
   if (atmos == NULL)
      atmos = internalAtmos;

   if (cbFixed != NULL)
      internalAtmos->SetFixedCoordinateSystem(cbFixed);
   if (internalCoordSystem != NULL)
      internalAtmos->SetInternalCoordSystem(internalCoordSystem);
   
   F107ID = internalAtmos->GetParameterID("F107");
   F107AID = internalAtmos->GetParameterID("F107A");
   KPID = internalAtmos->GetParameterID("MagneticIndex");
   cssiWFileID = internalAtmos->GetParameterID("CSSISpaceWeatherFile");
   schattenWFileID = internalAtmos->GetParameterID("SchattenFile");

   if (F107ID < 0)
      throw ODEModelException("Atmosphere model initialization is incomplete");
   internalAtmos->SetRealParameter(F107ID, fluxF107);
   internalAtmos->SetRealParameter(F107AID, fluxF107A);
   internalAtmos->SetRealParameter(KPID, kp);

   // Set the file names
   internalAtmos->SetStringParameter(cssiWFileID, cssiWFile);
   internalAtmos->SetStringParameter(schattenWFileID, schattenWFile);


   #ifdef DEBUG_DRAGFORCE_REFOBJ
   MessageInterface::ShowMessage
      ("DragForce::SetInternalAtmosphereModel() returning true, internalAtmos=<%p>\n",
       internalAtmos);
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// AtmosphereModel* GetInternalAtmosphereModel()
//------------------------------------------------------------------------------
/**
 * Gets the internal atmosphere model for the DragForce.
 *
 * @return pointer to the internal atmosphere model used by the DragForce
 *         object when useExternalAtmosphere is set to false.
 */
//------------------------------------------------------------------------------
AtmosphereModel* DragForce::GetInternalAtmosphereModel()
{
   return internalAtmos;
}


//------------------------------------------------------------------------------
// AtmosphereModel* GetInternalAtmosphereModel()
//------------------------------------------------------------------------------
/**
 * Gets the internal atmosphere model for the DragForce.
 *
 * @return pointer to the internal atmosphere model used by the DragForce
 *         object when useExternalAtmosphere is set to false.
 */
//------------------------------------------------------------------------------
AtmosphereModel* DragForce::GetAtmosphereModel()
{
   return (internalAtmos ? internalAtmos : atmos);
}


//------------------------------------------------------------------------------
// bool CheckFluxFile(const std::string &filename, bool isHistoric)
//------------------------------------------------------------------------------
/**
 * Checks (1) for file existence and (2) for file keywords
 *
 * @param filename The file to check
 * @param isHistoric Flag indicating if the file is used for measured data
 *
 * @return true if the file is valid and in contains the expected keywords
 */
//------------------------------------------------------------------------------
std::string DragForce::CheckFluxFile(const std::string &filename, bool isHistoric)
{
   std::string retval = "";

   Integer loc = 0;

   // This is the keyword list, consisting of a header followed by begin/end tags
   std::vector<StringArray> keywords;
   StringArray tags;
   // CSSI used for historic data
   tags.push_back("DATATYPE CSSISPACEWEATHER");
   tags.push_back("BEGIN OBSERVED");
   tags.push_back("END OBSERVED");
   keywords.push_back(tags);
   tags.clear();

   // CSSI used for predict data
   tags.push_back("DATATYPE CSSISPACEWEATHER");
   tags.push_back("BEGIN DAILY_PREDICTED");
   tags.push_back("END DAILY_PREDICTED");
   tags.push_back("BEGIN MONTHLY_PREDICTED");
   tags.push_back("END MONTHLY_PREDICTED");
   keywords.push_back(tags);
   tags.clear();

   // Schatten
   tags.push_back("BEGIN_DATA");
   tags.push_back("END_DATA");
   keywords.push_back(tags);

   // Check for file existence, possibly with path prefixes
   FileManager *fm = FileManager::Instance();
   std::string fluxPath = fm->GetAbsPathname("ATMOSPHERE_PATH");

   std::string weatherfile = filename;
   #ifdef DEBUG_FLUX_FILE
   MessageInterface::ShowMessage("%d %s\n", ++loc, weatherfile.c_str());
   #endif

   if (fm->DoesFileExist(weatherfile) == false)
   {
      #ifdef DEBUG_FLUX_FILE
      MessageInterface::ShowMessage("No file; adding path %s\n", fluxPath.c_str());
      #endif
      weatherfile = fluxPath + filename;
   }

   #ifdef DEBUG_FLUX_FILE
   MessageInterface::ShowMessage("%d %s\n", ++loc, weatherfile.c_str());
   #endif

   if (fm->DoesFileExist(weatherfile) == false)
   {
      MessageInterface::ShowMessage("Cannot locate\n");

      std::string fileUsage = isHistoric ? "observed" : "predicted";
      throw ODEModelException("Cannot open the " +
            fileUsage + " space weather file " +
            filename + ", nor the file at the location " + weatherfile);
   }

   #ifdef DEBUG_FLUX_FILE
   MessageInterface::ShowMessage("The file %s exists\n", weatherfile.c_str());
   #endif

   // File exists; check for keywords
   UnsignedInt count = (isHistoric ? 1 : keywords.size());
   UnsignedInt start = (isHistoric ? 0 : 1);
   UnsignedInt index = 9999;

   bool fileIsValid = false;

   std::ifstream inStream(weatherfile.c_str());
   std::string line;

   #ifdef DEBUG_FLUX_FILE
   MessageInterface::ShowMessage("Checking for the header:\n");
   #endif

   // Check headers to determine the index of the keywork list
   while (!inStream.eof() && (index == 9999))
   {
      getline(inStream, line);

      // if the line is blank, skip it
      if (GmatStringUtil::IsBlank(line, true)) continue;

      // Make upper case, so we can check for certain keyword
      line = GmatStringUtil::ToUpper(line);
      for (UnsignedInt i = start; i < count; ++i)
      {
         if (std::string(line).find(keywords[i][0]) != std::string::npos)
         {
            index = i;
            break;
         }
      }
   }

   if (index < 9999)
   {
      // Check start/end tags
      UnsignedInt kwCount = keywords[index].size();
      for (UnsignedInt i = 1; i < kwCount; ++i)
      {
         bool kwFound = false;
         std::string currentString = keywords[index][i];

         while (!inStream.eof() && !kwFound)
         {
            getline(inStream, line);

            // if the line is blank, skip it
            if (GmatStringUtil::IsBlank(line, true)) continue;

            // Make upper case, so we can check for certain keyword
            line = GmatStringUtil::ToUpper(line);
            if (std::string(line).find(currentString) != std::string::npos)
            {
               kwFound = true;
               if (i == kwCount - 1)
                  // Valid iff header and all keywords, in order, were found
                  fileIsValid = true;
            }
         }
      }
   }
   inStream.close();

   if (fileIsValid)
      retval = weatherfile;
      #ifdef DEBUG_FLUX_FILE
      MessageInterface::ShowMessage("The file %s; returning \"%s\"\n",
         (fileIsValid ? "is valid" : "is NOT valid"), retval.c_str());
      #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative 
 * information for a specified type.
 * 
 * @param id State Element ID for the derivative type
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool DragForce::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "DragForce checking for support for id %d\n", id);
   #endif
      
   if (id == Gmat::CARTESIAN_STATE)
      return true;
   
   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
      return true;
   
   if (id == Gmat::ORBIT_A_MATRIX)
      return true;

   if (id == Gmat::CD_EPSILON)
      return true;

   if (id == Gmat::ATMOS_DENSITY_EPSILON)
      return true;

   return PhysicalModel::SupportsDerivative(id);
}


//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Function used to set the start point and size information for the state 
 * vector, so that the derivative information can be placed in the correct place 
 * in the derivative vector.
 * 
 * @param id          State Element ID for the derivative type
 * @param index       Starting index in the state vector for this type of derivative
 * @param quantity    Number of objects that supply this type of data
 * @param totalSize   Total of all STMs' size
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool DragForce::SetStart(Gmat::StateElementId id, Integer index, 
                      Integer quantity, Integer totalSize)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage("DragForce setting start data for id = %d"
            " to index %d; %d objects identified\n", id, index, quantity);
   #endif
   
   bool retval = false;
   
   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         satCount = quantity;
         cartesianCount = quantity;
         cartIndex = index;
         fillCartesian = true;
         retval = true;
         break;
         
      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
         stmCount = quantity;
         stmStart = index;
         fillSTM = true;
			totalSTMSize = totalSize;
         retval = true;
         break;

      case Gmat::ORBIT_A_MATRIX:
         aMatrixCount = quantity;
         aMatrixStart = index;
         fillAMatrix = true;
			totalSTMSize = totalSize;
         retval = true;
         break;

      case Gmat::CD_EPSILON:
         cdEpsilonCount = quantity;
         cdEpsilonIndex = index;
         fillCdEpsilon = true;
         retval = true;
         break;

      case Gmat::ATMOS_DENSITY_EPSILON:
         atmosDensityEpsilonCount = quantity;
         atmosDensityEpsilonIndex = index;
         fillAtmosDensityEpsilon = true;
         retval = true;
         break;
         
      default:
         break;
   }
   
   #ifdef DEBUG_INITIALIZE
      MessageInterface::ShowMessage("SetStart: total STM size = %d\n", totalSTMSize);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// void DragForce::GetDensity(Real *state)
//------------------------------------------------------------------------------
/**
 * Accessor for the density calculation in the embedded AtmosphereModel.
 * 
 * @param state Cartesian position/velocity state specifying where the density
 *              is needed.
 * @param when  TAI Modified Julian epoch for the calculation
 *
 * @return The first computed density value
 */
//------------------------------------------------------------------------------
Real DragForce::GetDensity(Real *state, Real when, Integer count)
{
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Entered DragForce::GetDensity()\n";
   #endif

   if (count == -1)
      count = satCount;

   // Give it a default value if the atmosphere model is not set
   if (!atmos)
   {
      for (Integer i = 0; i < count; ++i)
         density[i] = 4.0e-13;
   }
   else
   {
      //if (atmos)
      //{
         if (sun && centralBody)
         {
            // Update the Sun vector
            Rvector sunV = sun->GetState(when);
            Rvector cbV  = centralBody->GetState(when);
                
            sunLoc[0] = sunV[0];
            sunLoc[1] = sunV[1];
            sunLoc[2] = sunV[2];
            cbLoc[0]  = cbV[0];
            cbLoc[1]  = cbV[1];
            cbLoc[2]  = cbV[2];
         }
      //}

      #ifdef DEBUG_DRAGFORCE_DENSITY
         dragdata << "Calling atmos->Density() on " << atmos->GetTypeName()
                  << "\n";
      #endif
      atmos->Density(state, density, when, count);
      #ifdef DEBUG_DRAGFORCE_DENSITY
         dragdata << "Returned from atmos->Density()\n";
      #endif
        
      #ifdef DEBUG_DRAGFORCE_DENSITY
         for (Integer m = 0; m < count; ++m)
            dragdata << "   Epoch: " << when
                     << "   State: "
                     << state[m*6] << "  "
                     << state[m*6+1] << "  "
                     << state[m*6+2] << "    Density: "
                     << density[m] << "\n";
      #endif
   }
   #ifdef DEBUG_DRAGFORCE_DENSITY
      dragdata << "Leaving DragForce::GetDensity()\n";
   #endif

   // Sanity check the results
   if (GmatMathUtil::IsNaN(density[0]))
   {
      std::stringstream epdata;
      epdata << when;
      throw ODEModelException("The drag force generated an atmospheric density "
            "that is not a number for the model " + atmosphereType +
            " at MJD " + epdata.str() + ".  Check the input data files for "
            "values that are unphysical.");
   }

   if (GmatMathUtil::IsInf(density[0]))
   {
      std::stringstream epdata;
      epdata << when;
      throw ODEModelException("The drag force generated an atmospheric density "
            "that is infinite for the model " + atmosphereType +
            " at MJD " + epdata.str() + ".  Check the input data files for "
            "values that are unphysical.");
   }

   // Apply atmosphere density scale factor
   for (Integer i = 0; i < count && i < atmosDensityScaleFactor.size(); ++i)
      density[i] *= atmosDensityScaleFactor[i];

   return density[0];
}


//------------------------------------------------------------------------------
// bool IsUnique(const std::string &forBody)
//------------------------------------------------------------------------------
/**
 * Specifies if a force is the one and only force, optionally at a given body.
 *
 * @param forBody The body at which the force must be unique, if body specific
 *
 * @return true if the force should be unique in the ODEModel.
 */
//------------------------------------------------------------------------------
bool DragForce::IsUnique(const std::string& forBody)
{
   if (bodyName == forBody)
      return true;
   return false;
}


//------------------------------------------------------------------------------
// Rvector3 Accelerate(Integer scID, Real *theState, GmatEpoch &theEpoch)
//------------------------------------------------------------------------------
/**
 * Computed the drag acceleration at the input state and epoch
 *
 * This method is used to finite difference the data for the A-matrix and STM
 *
 * @param theState The state used for the calculations
 * @param theEpoch The epoch associated with the state
 * @param prefactor The prefactor term: -1/2 Cd A/m
 *
 * @return The resulting acceleration, as a 3-vector
 */
//------------------------------------------------------------------------------
Rvector3 DragForce::Accelerate(Integer scID, Real *theState, GmatEpoch &theEpoch, Real prefactor)
{
   Real vRelative[3], vRelMag, factor;
   Rvector3 accel;

   Real theDensity = GetDensity(theState, theEpoch, 1);

   if (hasWindModel)
   {
      Real wind[6];

      // v_rel = v - w x R
      atmos->Wind(theState, wind, theEpoch, 1);
      vRelative[0] = theState[3] - wind[3];
      vRelative[1] = theState[4] - wind[4];
      vRelative[2] = theState[5] - wind[5];
      vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                     vRelative[2]*vRelative[2]);
   }
   else
   {
      // v_rel = v - w x R
      vRelative[0] = theState[3] -
                     (angVel[1]*theState[2] - angVel[2]*theState[1]);
      vRelative[1] = theState[4] -
                     (angVel[2]*theState[0] - angVel[0]*theState[2]);
      vRelative[2] = theState[5] -
                     (angVel[0]*theState[1] - angVel[1]*theState[0]);
      vRelMag = sqrt(vRelative[0]*vRelative[0] + vRelative[1]*vRelative[1] +
                     vRelative[2]*vRelative[2]);
   }

   // prefactor will include appropriate data for Spherical or SPAD
   factor = prefactor * theDensity;
   
   // Just check for SPAD or Spherical here???
   if (dragShapeModelIndex == ShapeModel::SPHERICAL_MODEL)
   {
      accel[0] = factor * vRelMag * vRelative[0];
      accel[1] = factor * vRelMag * vRelative[1];
      accel[2] = factor * vRelMag * vRelative[2];
   }
   else if (dragShapeModelIndex == ShapeModel::SPAD_FILE_MODEL)        // SPAD
   {
      Rvector3 velVec(vRelative[0], vRelative[1], vRelative[2]);
      Rvector3 spadArea = ((Spacecraft*) scObjs.at(scID))->GetSPADDragArea(
                                                           theEpoch, velVec);

      accel[0] = factor * spadArea[0] * vRelMag * vRelMag;
      accel[1] = factor * spadArea[1] * vRelMag * vRelMag;
      accel[2] = factor * spadArea[2] * vRelMag * vRelMag;
   }

   return accel;
}


//------------------------------------------------------------------------------
// Real CalculateAp(Real kp)
//------------------------------------------------------------------------------
/**
 * Converts Kp values to Ap values.  If the atmosphere model is set, the
 * calculation is performed using the AtmosphereModel::ConvertKpToAp method.
 * If not, the value is calculated using Vallado (2nd Edition), eq 8-31:
 *
 *    \f[a_p = \exp\left(\frac{k_p + 1.6} {1.75}\right)\f]
 *
 * @param <kp> The planetary index that is converted.
 *
 * @return The corresponding (approximate) planetary amplitude, Ap.
 */
//------------------------------------------------------------------------------
Real DragForce::CalculateAp(Real kp)
{
   Real newAp = 0.0;

   if (atmos)
      newAp = atmos->ConvertKpToAp(kp);
   else
      newAp = exp((kp + 1.6) / 1.75);

   return newAp;
}

