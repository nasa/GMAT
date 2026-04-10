//------------------------------------------------------------------------------
//                         SPICEPropagator.cpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Darrel Conway
// Created: 2019/09/26
//
// Copyright (c) 2019, Thinking Systems, Inc.
// All rights reserved

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Edited by Jairus Elarbee 7/20/2021 
//   -Added ICRF to FK5 conversion to Initialize() and UpdateState()
//    for state conversion from SPICE to GMAT
//
// Edited by Jairus Elarbee 10/05/2021 
//   -Removed ICRF to FK5 conversion and added TEME conversion function
//   -Added TEME Debug flag
//   -If User doesn't specify a Spacecraft epoch, then the TLE Prop will initialize at the TLE epoch
// Edited by Jairus Elarbee 10/25/2021 
//   -Added check for invalid TLE elements and warning for incorrect check-sum
// Edited by Jairus Elarbee 11/10/2021
//   -Updated to SPICE v66 TLE propagator (ev2lin -> spke10), now uses 2006 Vallado version
//   -TEME to J2000 conversion no longer needed, so removed

/**
 *  Declaration code for the SPICEPropFactory class.
 */


#include "SPICEPropagator.hpp"

#include <sstream>
#include "MessageInterface.hpp"
#include "PropagatorException.hpp"
#include "TLEReader.hpp"
#include "Spacecraft.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"
#include "TransformUtil.hpp"
#include "StringUtil.hpp"

//#define DEBUG_EXE

extern "C"  
{
   #include "SpiceUsr.h"    // for CSPICE routines
   #include "SpiceZfc.h"    // for spke10
}

const std::string
SPICEPropagator::PARAMETER_TEXT[SPICEPropParamCount - PropagatorParamCount] =
{
   "StepSize",          // Propagation step size
   // Optional config file containing the propagator parameters
   "ConfigurationFile", // CONFIG_FILENAME
   // All optional: these override the defaults and file settings
   "J2",                // J2   J2 value
   "J3",                // J3   J3 value
   "J4",                // J4   J4 value
   "KE",                // KE   sqrt(mu) in earth-radii**1.5/MIN
   "QO",                // QO   Upper bound of atmospheric model in KM
   "SO",                // SO   Lower bound of atmospheric model in KM
   "ER",                // ER   Earth equatorial radius in KM.
   "AE"                 // AE   Distance units/earth radius
};

/// EphemerisPropagator parameter types
const Gmat::ParameterType
SPICEPropagator::PARAMETER_TYPE[SPICEPropParamCount - PropagatorParamCount] =
{
   Gmat::REAL_TYPE,        // StepSize, in seconds
   Gmat::FILENAME_TYPE,    // CONFIG_FILENAME
   Gmat::REAL_TYPE,        // J2   J2 value
   Gmat::REAL_TYPE,        // J3   J3 value
   Gmat::REAL_TYPE,        // J4   J4 value
   Gmat::REAL_TYPE,        // KE   sqrt(mu) in earth-radii**1.5/MIN
   Gmat::REAL_TYPE,        // QO   Upper bound of atmospheric model in KM
   Gmat::REAL_TYPE,        // SO   Lower bound of atmospheric model in KM
   Gmat::REAL_TYPE,        // ER   Earth equatorial radius in KM.
   Gmat::REAL_TYPE         // AE   Distance units/earth radius
};


//----------------------------------------------------------------
// Internal data structure (implemented as a class)
//----------------------------------------------------------------

/**
 * Constructor
 */
SPICEPropagator::PropObject::PropObject(const std::string &name, CoordinateConverter *cvt) :
   satName     (name),
   theSat      (nullptr),
   a1epoch     (0.0),
   timeOffset  (0.0),
   tleName     (""),
   cconverter  (cvt)
{
}

/**
 * Destructor
 */
SPICEPropagator::PropObject::~PropObject()
{
}

/**
 * Copy constructor
 *
 * @param po The PropObject that is copied to this one
 */
SPICEPropagator::PropObject::PropObject(const PropObject& po) :
   satName        (po.satName),
   theSat         (po.theSat),
   a1epoch        (po.a1epoch),
   timeOffset     (po.timeOffset),
   tleName        (po.tleName),
   tleSettings    (po.tleSettings),
   cconverter     (po.cconverter)
{
}


bool SPICEPropagator::PropObject::SetBaseCS(CoordinateSystem *bcs)
{
   baseCS = bcs;
   return true;
}

/**
 * Assignment operator
 *
 * @param po The PropObject providing new settings here
 */
SPICEPropagator::PropObject& SPICEPropagator::PropObject::operator=(const PropObject& po)
{
   if (this != &po)
   {
      satName     = po.satName;
      theSat      = po.theSat;
      a1epoch     = po.a1epoch;
      timeOffset  = po.timeOffset;
      tleName     = po.tleName;
      tleSettings = po.tleSettings;
      cconverter  = po.cconverter;
   }
   return *this;
}

/**
 * Initializer
 *
 * This method is called when the propagator is initialized.
 *
 * @param toTime The base epoch for the propagator
 * @param params The TLE propagator parameters to be used
 *
 * @return true on success, false on failure
 */
bool SPICEPropagator::PropObject::Initialize(const GmatEpoch toTime, double *params)
{
   bool retval = false;

   if (theSat)
   {
      std::string tleSource = theSat->GetStringParameter("EphemerisName");
      tleName = theSat->GetStringParameter("Id");

      #ifdef DEBUG_EXE
         MessageInterface::ShowMessage("%s[0]:  Using TLE ID %s in file %s\n",
               theSat->GetName().c_str(), tleName.c_str(),
               tleSource.c_str());
      #endif

      std::stringstream errormsg;

      // Check to see if the TLE exists
      std::string oldTleSource = tleSource;
      tleSource = GmatFileUtil::FindFile(tleSource, true, true);
      if (tleSource == "")
         errormsg << "The 2-line element file \""
                  << oldTleSource << "\" could not be found.";
      else
      {
         // Check to see if the ID is set
         if (tleName == "")
         {
            if (errormsg.str() != "")
               errormsg << "\n";

            errormsg << "The spacecraft ID used for the  TLE "
                     << "identifier for spacecraft \"" << theSat->GetName()
                     << "\" is not set; please set the \"Id\" field on the "
                     << "spacecraft.";
         }
      }

      if (errormsg.str() != "")
         throw PropagatorException(errormsg.str());

      TLEReader reader(tleSource);
      tleSettings = reader.GetTLEData(tleName);

      if (tleSettings.tleLines[1] == "")
         throw PropagatorException("The spacecraft ID \"" + tleName +
               "\" was not found in the TLE file \"" + tleSource +
               "\" (Note: these identifiers are case sensitive).");

      // Added checks to ensure the values in the TLE are valid
      Real num;
      GmatStringUtil::ToReal(tleSettings.tleLines[2].substr(8, 8), num);
      if (tleSettings.tleLines[1].size() != 69 || tleSettings.tleLines[2].size() != 69)
      {
         throw PropagatorException("The number of characters in the data lines of the TLE " + tleSource + " does not match the TLE standard. The two data lines must each be exactly 69 characters in length");
      }
      if (num >= 180 || num < 0.0)
      {
         throw PropagatorException("The Inclination in a TLE must be between 0 and 180, but is " + GmatStringUtil::ToString(num));
      }
      GmatStringUtil::ToReal(tleSettings.tleLines[2].substr(17, 8), num);
      if (num >= 360 || num < 0.0)
      {
         throw PropagatorException("The RAAN in a TLE must be between 0 and 360, but is " + GmatStringUtil::ToString(num));
      }
      GmatStringUtil::ToReal(tleSettings.tleLines[2].substr(26, 8), num);
      if (num >= 9999999 || num < 0)
      {
         throw PropagatorException("The Eccentricity in a TLE must be between 0 and 1, but is " + GmatStringUtil::ToString(num/1000000));
      }
      GmatStringUtil::ToReal(tleSettings.tleLines[2].substr(34, 8), num);
      if (num >= 360 || num < 0.0)
      {
         throw PropagatorException("The AOP in a TLE must be between 0 and 360, but is " + GmatStringUtil::ToString(num));
      }
      GmatStringUtil::ToReal(tleSettings.tleLines[2].substr(43, 8), num);
      if (num >= 360 || num < 0.0)
      {
         throw PropagatorException("The MA in a TLE must be between 0 and 360, but is " + GmatStringUtil::ToString(num));
      }
      GmatStringUtil::ToReal(tleSettings.tleLines[2].substr(52, 11), num);
      if (num >= 99 || num < 0.0)
      {
         throw PropagatorException("The mean motion value of " + GmatStringUtil::ToString(num) + " is not valid");
      }

      // Check the Checksums
      for (Integer line = 1; line < 3; line++)
      {
         Integer sum = 0;
         for (int i = 0; i < tleSettings.tleLines[line].length() - 1; i++)
         {
            num = 0;
            GmatStringUtil::ToReal(tleSettings.tleLines[line].substr(i, 1), num);
            sum += num;
            if (tleSettings.tleLines[line].substr(i, 1) == "-")
            {
               sum += 1;
            }
         }
         GmatStringUtil::ToReal(tleSettings.tleLines[line].substr(68, 1), num);
         if (Real(sum % 10) != num)
         {
            MessageInterface::ShowMessage("TLE WARNING: Checksum in line %i of TLE is %2.0f, but should be %2.0f\n",line, num, Real(sum % 10));
         }
      }

      reader.ParseForSpice(tleSettings);
      
      // Convert TLE time to a1mjd
      char gregTime[30];
      timout_c(tleSettings.secFromJ2k, "DD Mon YYYY HR:MN:SC.###", 31, gregTime);
      TimeSystemConverter *tcv = TimeSystemConverter::Instance();
      a1epoch = tcv->Convert(tcv->ConvertGregorianToMjd(gregTime), TimeSystemConverter::UTCMJD, TimeSystemConverter::A1MJD);
      Real satTime = toTime;

      // If Spacecraft has no epoch defined, set to the epoch of the TLE
      if (satTime == 21545.000372499999)
      {
         satTime = a1epoch;
         theSat->SetEpoch("UTCGregorian",gregTime, a1epoch);
         
      }
      timeOffset = (satTime - a1epoch) * GmatTimeConstants::SECS_PER_DAY;

      #ifdef DEBUG_EXE
         MessageInterface::ShowMessage("Base GregTime: %s\n", gregTime);
         MessageInterface::ShowMessage("\nLine 1:  %s\n", tleSettings.tleLines[1].c_str());
         MessageInterface::ShowMessage("Line 2:  %s\n\n", tleSettings.tleLines[2].c_str());
         MessageInterface::ShowMessage("TLEData:\n   Epoch:  %.12lf   "
               "Offset:  %12lf\n", a1epoch, timeOffset);

         for (int i = 0; i < 10; ++i)
            MessageInterface::ShowMessage("   %2d: %.12lf\n", i, tleSettings.elements[i]);
      #endif

      // Prepare the elements and constants for SPICE spke10
      Real et = tleSettings.secFromJ2k + timeOffset;
      Real iState[6];
      for (Integer ill = 0; ill < 32; ill++) 
      {
         if (ill < 8)
         {
            inputs[ill] = params[ill];
         }
         else if (ill < 18)
         {
            inputs[ill] = tleSettings.elements[ill-8];
         }
         else if(ill < 28)
         {
            inputs[ill] = tleSettings.elements[ill - 18];
         }
         else
         {
            inputs[ill] = tleSettings.elements[9];
         }
      }

      // Evaluate the TLE
      try
      {
         spke10_(&et, inputs, iState);
      }
      catch (...)
      {
         throw PropagatorException("Error: SPICE encountered an problem propagating the TLE. Check that the TLE is valid and its orbit does not intersect Earth.");
      }
      Rvector6 tleState(iState);
      theSat->SetRealParameter("X", tleState[0]);
      theSat->SetRealParameter("Y", tleState[1]);
      theSat->SetRealParameter("Z", tleState[2]);
      theSat->SetRealParameter("VX", tleState[3]);
      theSat->SetRealParameter("VY", tleState[4]);
      theSat->SetRealParameter("VZ", tleState[5]);

      retval = true;
   }

   return retval;
}


SPICEPropagator::SPICEPropagator(const std::string &withName) :
   Propagator     ("SPICESGP4", withName),
   configFileName (""),
   psm            (nullptr),
   state          (nullptr),
   j2kState       (nullptr),
   initialEpoch   (-987654321.0),
   currentEpoch   (-987654321.0),
   timeFromEpoch  (0.0),
   stepTaken      (60.0)
{
   params[0] = 1.082616e-3;     // geophs[ J2 ] =    1.082616e-3; 
   params[1] = -2.53881e-6;     // geophs[ J3 ] =   -2.53881e-6; 
   params[2] = -1.65597e-6;     // geophs[ J4 ] =   -1.65597e-6; 
   params[3] = 7.43669161e-2;   // geophs[ KE ] =    7.43669161e-2; 
   params[4] = 120.0;           // geophs[ QO ] =  120.0; 
   params[5] = 78.0;            // geophs[ SO ] =   78.0; 
   params[6] = 6378.135;        // geophs[ ER ] = 6378.135; 
   params[7] = 1.0;             // geophs[ AE ] =    1.0; 
}

SPICEPropagator::~SPICEPropagator()
{
   if (state != NULL)
      delete[] state;

   if (j2kState != NULL)
      delete[] j2kState;
}

SPICEPropagator::SPICEPropagator(const SPICEPropagator &sgp) :
   Propagator        (sgp),
   configFileName    (sgp.configFileName),
   psm               (nullptr),
   state             (nullptr),
   j2kState          (nullptr),
   initialEpoch      (-987654321.0),
   currentEpoch      (-987654321.0),
   timeFromEpoch     (0.0),
   stepTaken         (sgp.stepTaken)
{
   for (UnsignedInt i = 0; i < 8; ++i)
      params[i] = sgp.params[i];
}

SPICEPropagator& SPICEPropagator::operator=(const SPICEPropagator &sgp)
{
   if (&sgp != this)
   {
      configFileName = sgp.configFileName;
      psm            = nullptr;
      initialEpoch   = -987654321.0;
      currentEpoch   = -987654321.0;
      timeFromEpoch  = 0.0;
      stepTaken      = sgp.stepTaken;

      for (UnsignedInt i = 0; i < 8; ++i)
         params[i] = sgp.params[i];

      isInitialized = false;
   }

   return *this;
}

GmatBase* SPICEPropagator::Clone() const
{
   return new SPICEPropagator(*this);
}


// Parameter accessor methods -- overridden from GmatBase
std::string SPICEPropagator::GetParameterText(const Integer id) const
{
   if (id >= PropagatorParamCount && id < SPICEPropParamCount)
      return PARAMETER_TEXT[id - PropagatorParamCount];
   return Propagator::GetParameterText(id);
}

Integer SPICEPropagator::GetParameterID(const std::string &str) const
{
   Integer retval = -1;
   if (str == "CentralBody")
      retval = (str == "Earth" ? 255 : 256);
   if (str == "EpochFormat")
      retval = ((str == "FromSpacecraft" || str == "") ? 255 : 257);
   if (str == "StartEpoch")
      retval = (str == "FromSpacecraft" ? 255 : 258);

   if (retval >= 255)
   {
      if (retval == 256)
      {
         MessageInterface::ShowMessage("TLE propagators cannot override %s; "
               "skipping setting\n", str.c_str());
         retval = 255;
      }

      if (retval >= 257)
         MessageInterface::ShowMessage("TLE propagators set the %s "
               "setting from the spacecraft\n", str.c_str());

      return retval;
   }

   for (Integer i = PropagatorParamCount;
         i < SPICEPropParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - PropagatorParamCount])
         return i;
   }

   return Propagator::GetParameterID(str);
}


Gmat::ParameterType SPICEPropagator::GetParameterType(const Integer id) const
{
   if (id >= PropagatorParamCount && id < SPICEPropParamCount)
      return PARAMETER_TYPE[id - PropagatorParamCount];
   return Propagator::GetParameterType(id);
}


std::string SPICEPropagator::GetParameterTypeString(const Integer id) const
{
   if (id >= PropagatorParamCount && id < SPICEPropParamCount)
      return Propagator::PARAM_TYPE_STRING[GetParameterType(id)];
   return Propagator::GetParameterTypeString(id);
}


bool SPICEPropagator::IsParameterReadOnly(const Integer id) const
{
   // For now, all TLE params are read only
   if (id >= PropagatorParamCount)
      return true;

   return Propagator::IsParameterReadOnly(id);
}


bool SPICEPropagator::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}



Real SPICEPropagator::GetRealParameter(const Integer id) const
{
   if (id >= PropagatorParamCount)
   {
      Real retval = 0.0;

      switch (id)
      {
      case STEPSIZE:
         return stepSize;
      case J2:
         return params[0];
      case J3:
         return params[1];
      case J4:
         return params[2];
      case KE:
         return params[3];
      case QO:
         return params[4];
      case SO:
         return params[5];
      case ER:
         return params[6];
      case AE:
         return params[7];

      default:
         throw PropagatorException("The parameter " + GetParameterText(id) +
               "is not a real number parameter accessible by the user");
      }

      return retval;
   }

   return Propagator::GetRealParameter(id);
}


Real SPICEPropagator::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


Real SPICEPropagator::SetRealParameter(const Integer id, const Real value)
{
   if (id >= PropagatorParamCount)
   {
      Real retval = 0.0;

      switch (id)
      {
      case STEPSIZE:
         stepSize = value;
         stepSizeBuffer = value;
         return stepSize;
      case J2:
         params[0] = value;
         return params[0];
      case J3:
         params[1] = value;
         return params[1];
      case J4:
         params[2] = value;
         return params[2];
      case KE:
         params[3] = value;
         return params[3];
      case QO:
         params[4] = value;
         return params[4];
      case SO:
         params[5] = value;
         return params[5];
      case ER:
         params[6] = value;
         return params[6];
      case AE:
         params[7] = value;
         return params[7];

      default:
         throw PropagatorException("The parameter " + GetParameterText(id) +
               "is not a real number parameter accessible by the user");
       }


      return retval;
   }

   return Propagator::SetRealParameter(id, value);
}


Real SPICEPropagator::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


Real SPICEPropagator::GetRealParameter(const Integer id, const Integer index) const
{
   return Propagator::GetRealParameter(id, index);
}


Real SPICEPropagator::GetRealParameter(const Integer id, const Integer row,
      const Integer col) const
{
   return Propagator::GetRealParameter(id, row, col);
}


Real SPICEPropagator::SetRealParameter(const Integer id, const Real value,
      const Integer index)
{
   return Propagator::SetRealParameter(id, value, index);
}


Real SPICEPropagator::SetRealParameter(const Integer id, const Real value,
      const Integer row, const Integer col)
{
   return Propagator::SetRealParameter(id, value, row, col);
}

std::string SPICEPropagator::GetStringParameter(const Integer id) const
{
   if (id == CONFIG_FILENAME)
      return configFileName;

   if (id == 257)
      return "A1ModJulian";

   if (id == 258)
      return "21545";

   return Propagator::GetStringParameter(id);
}


bool SPICEPropagator::SetStringParameter(const Integer id, const char *value)
{
   if (id >= 255)
      return true;

   if (id == CONFIG_FILENAME)
   {
      configFileName = value;
      return true;
   }

   return Propagator::SetStringParameter(id, value);
}


bool SPICEPropagator::SetStringParameter(const Integer id,
                                        const std::string &value)
{
   if (id >= 255)
      return true;

   if (id == CONFIG_FILENAME)
   {
      configFileName = value;

      MessageInterface::ShowMessage("WARNING:  TLE Configuration File is not yet coded\n");

      return true;
   }

   return Propagator::SetStringParameter(id, value);
}

std::string SPICEPropagator::GetStringParameter(const Integer id,
                                        const Integer index) const
{
   return Propagator::GetStringParameter(id, index);
}


bool SPICEPropagator::SetStringParameter(const Integer id,
                                        const char *value,
                                        const Integer index)
{
   return Propagator::SetStringParameter(id, value, index);
}


bool SPICEPropagator::SetStringParameter(const Integer id,
                                        const std::string &value,
                                        const Integer index)
{
   return Propagator::SetStringParameter(id, value, index);
}


std::string SPICEPropagator::GetStringParameter(const std::string &label) const
{
   if (label == "CentralBody")
      return "Earth";
   if (label == "EpochFormat")
      return "A1ModJulian";
   if (label == "StartEpoch")
      return "21545";

   return GetStringParameter(GetParameterID(label));
}


bool SPICEPropagator::SetStringParameter(const std::string &label,
                                        const char *value)
{
   if (label == "CentralBody")
      return true;
   if (label == "EpochFormat")
      return true;
   if (label == "StartEpoch")
      return true;

   return SetStringParameter(GetParameterID(label), value);
}


bool SPICEPropagator::SetStringParameter(const std::string &label,
                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


std::string SPICEPropagator::GetStringParameter(const std::string &label,
                                        const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


bool SPICEPropagator::SetStringParameter(const std::string &label,
                                        const std::string &value,
                                        const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}



const StringArray& SPICEPropagator::GetStringArrayParameter(const Integer id) const
{
   return Propagator::GetStringArrayParameter(id);
}


const StringArray& SPICEPropagator::GetStringArrayParameter(const Integer id,
                                             const Integer index) const
{
   return Propagator::GetStringArrayParameter(id, index);
}


const StringArray& SPICEPropagator::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


const StringArray& SPICEPropagator::GetStringArrayParameter(const std::string &label,
                                             const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}



std::string  SPICEPropagator::GetRefObjectName(const UnsignedInt type) const
{
   return Propagator::GetRefObjectName(type);
}

const StringArray& SPICEPropagator::GetRefObjectNameArray(const UnsignedInt type)
{
   if ((type == Gmat::SPACECRAFT) || (type == Gmat::SPACEOBJECT) ||
       (type == Gmat::FORMATION))
      return propObjectNames;

   return Propagator::GetRefObjectNameArray(type);
}


bool SPICEPropagator::SetRefObjectName(const UnsignedInt type,
                                      const std::string &name)
{
   bool retval = false;

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::SPACEOBJECT) ||
       (type == Gmat::FORMATION))
   {
      if (find(propObjectNames.begin(), propObjectNames.end(), name) ==
            propObjectNames.end())
         propObjectNames.push_back(name);

      retval = true;
   }

   return (Propagator::SetRefObjectName(type, name) || retval);
}


bool SPICEPropagator::RenameRefObject(const UnsignedInt type,
                                     const std::string &oldName,
                                     const std::string &newName)
{
   bool retval = false;

   if ((type == Gmat::SPACECRAFT) || (type == Gmat::SPACEOBJECT) ||
       (type == Gmat::FORMATION))
   {
      for (UnsignedInt i = 0; i < propObjectNames.size(); ++i)
         if (propObjectNames[i] == oldName)
            propObjectNames[i] = newName;
      retval = true;
   }

   return (Propagator::RenameRefObject(type, oldName, newName) || retval);
}


bool SPICEPropagator::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                  const std::string &name)
{
   bool retval = false;

   if (obj->IsOfType(Gmat::SPACEOBJECT))
   {
      for (UnsignedInt i = 0; i < propObjectNames.size(); ++i)
         if (propObjectNames[i] == name)
         {
            propObjects[i] = obj;
            retval = true;
         }
      if (retval == false)
      {
         propObjectNames.push_back(name);
         propObjects.push_back(obj);
      }
      return true;
   }

   return (Propagator::SetRefObject(obj, type, name) || retval);
}


bool SPICEPropagator::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                  const std::string &name,
                                  const Integer index)
{
   bool retval = false;

   if (obj->IsOfType(Gmat::SPACEOBJECT))
   {
      if (propObjectNames[index] == name)
      {
         propObjects[index] = obj;
         retval = true;
      }
   }

   return (Propagator::SetRefObject(obj, type, name, index) || retval);
}



bool SPICEPropagator::UsesODEModel()
{
   return false;
}

Real* SPICEPropagator::AccessOutState()
{
   return outState;
}

void SPICEPropagator::SetPropStateManager(PropagationStateManager *sm)
{
   psm = sm;
}

bool SPICEPropagator::Initialize()
{
   bool retval = false;
   cconverter.Initialize();

   Integer perSatDim = 6;

   // Load Leap Sec SPICE kernel if not already loaded
   std::string leapsecfile;
   FileManager *fileman = FileManager::Instance();
   leapsecfile = fileman->GetFullPathname("LSK_FILE");
   SpiceChar filtyp[256], source[256];
   SpiceInt handle;
   SpiceBoolean found;
   kinfo_c (leapsecfile.c_str(), 256, 256, filtyp, source, &handle, &found);
   if (!found)
   {
      furnsh_c(leapsecfile.c_str());
   }

   //   // Set Earth attitude data
   //   std::string earthLatest  = fileman->GetFullPathname("EARTH_LATEST_PCK_FILE");
   //   std::string earthPredict = fileman->GetFullPathname("EARTH_PCK_PREDICTED_FILE");
   //   std::string earthCurrent = fileman->GetFullPathname("EARTH_PCK_CURRENT_FILE");
   //   furnsh_c(earthPredict.c_str());
   //   furnsh_c(earthCurrent.c_str());
   //   furnsh_c(earthLatest.c_str());


   Integer oldDim = dimension;
   dimension = perSatDim * propObjects.size();
   if (dimension > 0)
   {
      if (state != NULL)
         delete [] state;
      state = new Real[dimension];

      if ((j2kState != NULL) && (oldDim != dimension))
      {
         delete [] j2kState;
         j2kState = NULL;
      }
      if (j2kState == NULL)
         j2kState = new Real[dimension];

      inState = outState = state;
   }

   if (Propagator::Initialize())
   {
      SPICEPropObjects.clear();
      Real initialTTEpoch;

      /// @todo  Manage formations
      for (UnsignedInt i = 0; i < propObjects.size(); ++i)
      {
         if (i == 0)
         {
            initialEpoch = ((Spacecraft*)(propObjects[0]))->GetEpoch();
            timeFromEpoch = 0.0;

            /// Convert A.1 -> TT
            TimeSystemConverter *tcv = TimeSystemConverter::Instance();
            initialTTEpoch = tcv->Convert(initialEpoch,
                  TimeSystemConverter::A1, TimeSystemConverter::TT);

            Real j2kRelativeEpoch = initialTTEpoch - 21545.0;

            initialTleEpoch = j2kRelativeEpoch *
                               GmatTimeConstants::SECS_PER_DAY;
         }
         else
            if (((Spacecraft*)(propObjects[0]))->GetEpoch() != initialEpoch)
               throw PropagatorException("Spacecraft epochs need to match for "
                     "the TLE Propagators");

         if (((Spacecraft*)(propObjects[i]))->GetJ2000BodyName() == "Earth")
         {
            if (i == 0)
            {
               propOrigin = ((Spacecraft*)(propObjects[0]))->GetJ2000Body();
               baseCS = ((Spacecraft*)(propObjects[0]))->GetInternalCoordSystem();
            }
         }
         else
         {
            throw PropagatorException("Spacecraft must have a central body of Earth for use "
               "with the TLE Propagators");
         }

         PropObject tleobj(propObjects[i]->GetName(), &cconverter);
         tleobj.theSat  = (Spacecraft*)propObjects[i];

         if (tleobj.Initialize(initialTTEpoch, params))
         {
            SPICEPropObjects.push_back(tleobj);
         }
         else
            throw PropagatorException("The TLE data failed to initialize for " +
                  propObjects[i]->GetName());

         // Load the state from the TLE into the spacecraft
         #ifdef DEBUG_EXE
            MessageInterface::ShowMessage("\n\nSpacecraft epoch (sec):  %.12lf\n",
                  initialTleEpoch);
         #endif
            
         if (currentEpoch == -987654321.0)
         {
            currentEpoch = tleobj.theSat->GetEpoch();
            initialEpoch = currentEpoch;
         }
      }

      // Set direction for the default step.  Step(Real) overrides the direction
//      ephemStep = fabs(ephemStep) * stepDirection;
      
      UpdateSpaceObject();

      retval = isInitialized = true;
   }

   return retval;
}


bool SPICEPropagator::Step(Real dt)
{
   bool retval = false;

   if (isInitialized)
   {
      Real tempStep = stepSize;
      stepSize = dt;

      #ifdef DEBUG_EXE
         MessageInterface::ShowMessage("Stepping by %.6le: from %.12lf to ",
               dt, currentEpoch);
      #endif

      retval = Step();
      if (retval)
         ((SpaceObject*)(propObjects[0]))->HasEphemPropagated(true);
      stepSize = tempStep;

      #ifdef DEBUG_EXE
               MessageInterface::ShowMessage("%.12lf: [%.12lf, %.12lf, %.12lf, "
                     "%.12lf, %.12lf, %.12lf]\n", currentEpoch, state[0],
                     state[1], state[2], state[3], state[4], state[5]);
      #endif
   }

   return retval;
}

/*
 * Performs the actual state advance
 *
 * @retval true if the step was taken
 */
bool SPICEPropagator::Step()
{
   bool retval = false;
//   static Real timeFromEpoch = 0.0;  // With this, the state does advance
   timeFromEpoch += stepSize;
   Real propEpoch = initialTleEpoch + timeFromEpoch;
   for (UnsignedInt i = 0; i < SPICEPropObjects.size(); ++i)
   {
      if (SPICEPropObjects[i].inputs[0] != params[0])
         for (Integer ill = 0; ill < 32; ill++)
         {
            if (ill < 8)
            {
               SPICEPropObjects[i].inputs[ill] = params[ill];
            }
            else if (ill < 18)
            {
               SPICEPropObjects[i].inputs[ill] = SPICEPropObjects[i].tleSettings.elements[ill - 8];
            }
            else if (ill < 28)
            {
               SPICEPropObjects[i].inputs[ill] = SPICEPropObjects[i].tleSettings.elements[ill - 18];
            }
            else
            {
               SPICEPropObjects[i].inputs[ill] = SPICEPropObjects[i].tleSettings.elements[9];
            }
         }
      try 
      {
         spke10_(&propEpoch, SPICEPropObjects[i].inputs, j2kState + i * 6);
      }
      catch(...)
      {
         throw PropagatorException("Error: SPICE encountered an problem propagating the TLE. Check that the TLE is valid and its orbit does not intersect Earth.");
      }

      memcpy(state + i * 6, j2kState + i * 6, 6 * sizeof(Real));
      
   }

   stepTaken = stepSize;

   currentEpoch = initialEpoch + timeFromEpoch /
            GmatTimeConstants::SECS_PER_DAY;
   UpdateSpaceObject();

   #ifdef DEBUG_EXE
      MessageInterface::ShowMessage("Step(%lf) %.12lf: [%.12lf, %.12lf, %.12lf, "
            "%.12lf, %.12lf, %.12lf]\n", stepTaken, currentEpoch, state[0],
            state[1], state[2], state[3], state[4], state[5]);
   #endif

   return true;
}


bool SPICEPropagator::RawStep()
{
   bool retval = false;
   return retval;
}


Real SPICEPropagator::GetStepTaken()
{
   return stepTaken;
}

Real SPICEPropagator::GetTime()
{
   return timeFromEpoch;
}

void SPICEPropagator::SetInternalCoordSystem(CoordinateSystem *cs)
{
   baseCS = cs;
   for (UnsignedInt i = 0; i < SPICEPropObjects.size(); ++i)
      SPICEPropObjects[i].SetBaseCS(cs);
}


void SPICEPropagator::UpdateSpaceObject(Real newEpoch)
{
   #ifdef DEBUG_EXE
      MessageInterface::ShowMessage(
            "EphemerisPropagator::UpdateSpaceObject(%.12lf) called\n", newEpoch);
   #endif

   if (psm)
   {
      Integer stateSize;
      Integer vectorSize;
      GmatState *newState;
      ReturnFromOrigin(newEpoch);

      newState = psm->GetState();
      stateSize = newState->GetSize();
      vectorSize = stateSize * sizeof(Real);

      currentEpoch = initialEpoch + timeFromEpoch / GmatTimeConstants::SECS_PER_DAY;

      // Update the epoch if it was passed in
      if (newEpoch != -1.0)
      {
         currentEpoch = newEpoch;
         timeFromEpoch = (currentEpoch-initialEpoch) * GmatTimeConstants::SECS_PER_DAY;
      }
      UpdateState();

//      memcpy(newState->GetState(), j2kState, vectorSize);
      memcpy(newState->GetState(), state, vectorSize);
      newState->SetEpoch(currentEpoch);
      newState->SetEpochGT(currentEpoch);
      psm->MapVectorToObjects();
   }
}

//void SPICEPropagator::UpdateSpaceObjectGT(GmatTime newEpoch)
//{
//}


void SPICEPropagator::UpdateFromSpaceObject()
{
   psm->MapObjectsToVector();
   GmatState *newState = psm->GetState();

   Integer stateSize = newState->GetSize();
   Integer vectorSize = stateSize * sizeof(Real);

   // Retrieve epoch and calculate the time from epoch for the state update
   currentEpoch = newState->GetEpoch();
   timeFromEpoch = (currentEpoch-initialEpoch) * GmatTimeConstants::SECS_PER_DAY;
   UpdateState();

   memcpy(newState->GetState(), state, vectorSize);
   newState->SetEpoch(currentEpoch);
   psm->MapVectorToObjects();

   memcpy(state, newState->GetState(), newState->GetSize() * sizeof(Real));

    // Transform to the force model origin
   if (HasPrecisionTime())
      MoveToOriginGT();
   else
      MoveToOrigin();
}


void SPICEPropagator::RevertSpaceObject()
{
//   if (HasPrecisionTime())
//   {
//      timeFromEpoch = (previousState.GetEpochGT() - initialEpochGT).GetTimeInSec();
//      currentEpochGT = initialEpochGT; currentEpochGT.AddSeconds(timeFromEpoch);
//      UpdateState();
//
//      //MoveToOriginGT();
//      ReturnFromOriginGT(); // J.McGreevy's version upload by Darrel on 10/12/2017
//   }
//   else
   {
      #ifdef DEBUG_EXE
         MessageInterface::ShowMessage("Reverting state from epoch %.12lf to ", currentEpoch);
      #endif

      timeFromEpoch = (previousState.GetEpoch() - initialEpoch) * GmatTimeConstants::SECS_PER_DAY;
      currentEpoch = previousState.GetEpoch();

      #ifdef DEBUG_EXE
         MessageInterface::ShowMessage("%.12lf\n", currentEpoch);
      #endif

      UpdateState();

      // MoveToOrigin();
      ReturnFromOrigin();
   }
}

void SPICEPropagator::BufferState()
{
   GmatState *stateToBuffer = psm->GetState();
   previousState = (*stateToBuffer);
}


/**
 * Updates the propagation state vector with data from the
 * PropagationStateManager
 */
//------------------------------------------------------------------------------
void SPICEPropagator::UpdateState()
{
   #ifdef DEBUG_EXE
      MessageInterface::ShowMessage("Updating state to epoch %.12lf\n",
            currentEpoch);
   #endif
      for (UnsignedInt i = 0; i < SPICEPropObjects.size(); ++i)
      {
         Real tleEpoch = initialTleEpoch + timeFromEpoch;

         // This if() is not robust enough to determine data refresh requirements
         // so always refreshing for now
         // if (SPICEPropObjects[i].inputs[0] != params[0])
            for (Integer ill = 0; ill < 32; ill++)
            {
               if (ill < 8)
               {
                  SPICEPropObjects[i].inputs[ill] = params[ill];
               }
               else if (ill < 18)
               {
                  SPICEPropObjects[i].inputs[ill] = SPICEPropObjects[i].tleSettings.elements[ill - 8];
               }
               else if (ill < 28)
               {
                  SPICEPropObjects[i].inputs[ill] = SPICEPropObjects[i].tleSettings.elements[ill - 18];
               }
               else
               {
                  SPICEPropObjects[i].inputs[ill] = SPICEPropObjects[i].tleSettings.elements[9];
               }
            }
         try
         {
            spke10_(&tleEpoch, SPICEPropObjects[i].inputs, j2kState + i * 6);
         }
         catch (...)
         {
            throw PropagatorException("Error: SPICE encountered an problem propagating the TLE. Check that the TLE is valid and its orbit does not intersect Earth.");
         }

         memcpy(state + i * 6, j2kState + i * 6, 6 * sizeof(Real));
         outState = state;

      }
}

/**
 *  Makes the epoch data consistent between the TLE setting and GMAT
 *
 *  @param satSource True if the Spacecraft epoch is the sync epoch, false if
 *  the Spacecraft needs to reset to match the TLE epoch
 */
bool SPICEPropagator::SyncEpochs(bool satSource)
{
   bool retval = false;

   Real theEpoch;          // A.1 Mod Julian
   Real theTleEpoch;       // TT sec from MJ2000

   if (satSource)
   {

   }
   else
   {
      theTleEpoch = initialTleEpoch + timeFromEpoch;
   }

   return retval;
}
