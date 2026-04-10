//$Id$
//------------------------------------------------------------------------------
//                           ExponentialAtmosphere
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
 * Vallado's exponentially modeled atmosphere, with one correction.
 */
//------------------------------------------------------------------------------

#include "ExponentialAtmosphere.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"
#include "StringUtil.hpp"
#include "CelestialBody.hpp"
#include <cmath>
#include "MessageInterface.hpp"


const std::string
ExponentialAtmosphere::PARAMETER_TEXT[
      ExponentialAtmosphereParamCount - AtmosphereModelParamCount] =
{
   "InputFile"
};

const Gmat::ParameterType
ExponentialAtmosphere::PARAMETER_TYPE[
      ExponentialAtmosphereParamCount - AtmosphereModelParamCount] =
{
   Gmat::FILENAME_TYPE    // "InputFile"
};


//------------------------------------------------------------------------------
// ExponentialAtmosphere(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param  name  name of the model (default is blank)
 */
//------------------------------------------------------------------------------
ExponentialAtmosphere::ExponentialAtmosphere(const std::string &name) :
   AtmosphereModel      ("Exponential", name),
   scaleHeight          (NULL),
   refHeight            (NULL),
   refDensity           (NULL),
   altitudeBands        (0),  // if changed, SetConstants must be adjusted!!
   smoothDensity        (false),
   parameterFileName    ("")
{
}


//------------------------------------------------------------------------------
// ~ExponentialAtmosphere()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ExponentialAtmosphere::~ExponentialAtmosphere()
{
   if (scaleHeight)
      delete [] scaleHeight;
   if (refHeight)
      delete [] refHeight;
   if (refDensity)
      delete [] refDensity;
}


//------------------------------------------------------------------------------
// ExponentialAtmosphere(const ExponentialAtmosphere& atm)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param atm ExponentialAtmosphere object to copy in creating the new one.
 */
//------------------------------------------------------------------------------
ExponentialAtmosphere::ExponentialAtmosphere(const ExponentialAtmosphere& atm) :
   AtmosphereModel      (atm),
   refHeight            (NULL),
   refDensity           (NULL),
   scaleHeight          (NULL),
   altitudeBands        (atm.altitudeBands),
   smoothDensity        (false),
   parameterFileName    (atm.parameterFileName)
{
   if (altitudeBands != 0)
   {
      refHeight = new Real[altitudeBands];
      refDensity = new Real[altitudeBands];
      scaleHeight = new Real[altitudeBands];

      for (Integer i = 0; i < altitudeBands; ++i)
      {
         refHeight[i] = atm.refHeight[i];
         refDensity[i] = atm.refDensity[i];
         scaleHeight[i] = atm.scaleHeight[i];
      }
   }
}

//------------------------------------------------------------------------------
//  ExponentialAtmosphere& operator= (const ExponentialAtmosphere& atm)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ExponentialAtmosphere class.
 *
 * @param atm the ExponentialAtmosphere object whose data to assign to "this"
 *              ExponentialAtmosphere.
 *
 * @return This ExponentialAtmosphere with data of input ExponentialAtmosphere
 *          atm.
 */
//------------------------------------------------------------------------------
ExponentialAtmosphere& ExponentialAtmosphere::operator=(
      const ExponentialAtmosphere &atm)
{
   if (&atm == this)
      return *this;

   AtmosphereModel::operator=(atm);
   parameterFileName = atm.parameterFileName;
   
   altitudeBands = atm.altitudeBands;
   if (altitudeBands == 0)
   {
      refHeight = NULL;
      refDensity = NULL;
      scaleHeight = NULL;
   }
   else
   {
      refHeight = new Real[altitudeBands];
      refDensity = new Real[altitudeBands];
      scaleHeight = new Real[altitudeBands];

      for (Integer i = 0; i < altitudeBands; ++i)
      {
         refHeight[i] = atm.refHeight[i];
         refDensity[i] = atm.refDensity[i];
         scaleHeight[i] = atm.scaleHeight[i];
      }
   }

   smoothDensity = false;   
   return *this;
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string ExponentialAtmosphere::GetParameterText(const Integer id) const
{
   if ((id >= AtmosphereModelParamCount) && (id < ExponentialAtmosphereParamCount))
      return PARAMETER_TEXT[id - AtmosphereModelParamCount];

   return AtmosphereModel::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer ExponentialAtmosphere::GetParameterID(const std::string &str) const
{
   for(Integer i = AtmosphereModelParamCount; i < ExponentialAtmosphereParamCount; ++i)
   {
      if (PARAMETER_TEXT[i - AtmosphereModelParamCount] == str)
         return i;
   }

   return AtmosphereModel::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ExponentialAtmosphere::GetParameterType(const Integer id) const
{
   if ((id >= AtmosphereModelParamCount) && (id < ExponentialAtmosphereParamCount))
      return PARAMETER_TYPE[id - AtmosphereModelParamCount];

   return AtmosphereModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string ExponentialAtmosphere::GetParameterTypeString(const Integer id) const
{
   return AtmosphereModel::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the setting for string based fields
 *
 * @param id The ID for the field
 *
 * @return The setting
 */
//------------------------------------------------------------------------------
std::string ExponentialAtmosphere::GetStringParameter(const Integer id) const
{
   if (id == INPUTFILENAME)
      return parameterFileName;

   return AtmosphereModel::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// std::string SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets values for string based fields
 *
 * @param id The ID for the field
 * @param value The new value for the field
 *
 * @return The true if the setting was updated
 */
//------------------------------------------------------------------------------
bool ExponentialAtmosphere::SetStringParameter(const Integer id,
                                        const std::string &value)
{
   if (id == INPUTFILENAME)
   {
      parameterFileName = value;
      return true;
   }

   return AtmosphereModel::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the setting for string based fields
 *
 * @param label The scripted field name for the field
 *
 * @return The setting
 */
//------------------------------------------------------------------------------
std::string ExponentialAtmosphere::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// std::string SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets values for string based fields
 *
 * @param label The scripted field name for the field
 * @param value The new value for the field
 *
 * @return The setting
 */
//------------------------------------------------------------------------------
bool ExponentialAtmosphere::SetStringParameter(const std::string &label,
                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// void SetCentralBody(CelestialBody *cb)
//------------------------------------------------------------------------------
/**
 * Sets the central body providing the atmosphere
 *
 * @param cb The body
 */
//------------------------------------------------------------------------------
void ExponentialAtmosphere::SetCentralBody(CelestialBody *cb)
{
   AtmosphereModel::SetCentralBody(cb);
   ReadParameters(cb->GetName());
}

//------------------------------------------------------------------------------
// bool ExponentialAtmosphere::ReadParameters(std::string cbName)
//------------------------------------------------------------------------------
/**
 * Loads the data for an exponential atmosphere
 *
 * @param cbName The name of the central body
 *               Note: used for bodies that are set up in the startup file
 *
 * @return true if the atmosphere data was set
 */
//------------------------------------------------------------------------------
bool ExponentialAtmosphere::ReadParameters(std::string cbName)
{
   if (refDensity)
      delete[] refDensity;
   if (refHeight)
      delete[] refHeight;
   if (scaleHeight)
      delete[] scaleHeight;

   Integer dataSize = 100;
   refDensity = new Real[dataSize];
   refHeight = new Real[dataSize];
   scaleHeight = new Real[dataSize];
   
   std::string fullFileName;

   // The parameterFileName field overrides startup file settings
   if (parameterFileName == "")
   {
      FileManager *fm = FileManager::Instance();
      if (cbName == "Earth")
         fullFileName = fm->GetAbsPathname("EARTH_EXPONENTIAL_FILE");
      else if (cbName == "Mars")
         fullFileName = fm->GetAbsPathname("MARS_EXPONENTIAL_FILE");
      else
         throw AtmosphereException("Error: No data file is available for " + cbName + "'s Exponential Atmosphere model.\n");
   }
   else
   {
      // May need to make this more path robust; for now just use the scripted value
      fullFileName = parameterFileName;
   }

   if (GmatFileUtil::DoesFileExist(fullFileName) == false)
      throw AtmosphereException("Error: Cannot open " + cbName + "'s exponential atmosphere data file " +
         fullFileName);

   std::ifstream inStream(fullFileName);
   std::string line;
   altitudeBands = 0;
   Integer lineNum = 0;
   while (!inStream.eof())
   {
      GmatFileUtil::GetLine(&inStream, line);
      ++lineNum;
      // if the line is blank, skip it
      if (GmatStringUtil::IsBlank(line, true)) 
         continue;
      
      // Read a record
      StringArray data = GmatStringUtil::SeparateByComma(line);
      if (data.size() != 3)
      {
         std::stringstream ss;
         ss << "Error: " << cbName << "' Exponential Atmosphere data file '" << fullFileName << "' has incorrect format in line " << lineNum <<"\n";
         ss << "<" << line << ">";
         throw AtmosphereException(ss.str());
      }
      
      // resize when need
      if (altitudeBands >= dataSize)
      {
         // memory overfow, increase data size by 100
         Real* refDensityNew = new Real[dataSize+100];
         Real* refHeightNew = new Real[dataSize+100];
         Real* scaleHeightNew = new Real[dataSize+100];
         for (Integer i = 0; i < dataSize; ++i)
         {
            refDensityNew[i] = refDensity[i];
            refHeightNew[i] = refHeight[i];
            scaleHeightNew[i] = scaleHeight[i];
         }
         delete[] refDensity;
         delete[] refHeight;
         delete[] scaleHeight;

         refHeight = refHeightNew;
         refDensity = refDensityNew;
         scaleHeight = scaleHeightNew;

         dataSize = dataSize + 100;
      }

      GmatStringUtil::ToReal(data[0], refHeight[altitudeBands]);
      GmatStringUtil::ToReal(data[1], refDensity[altitudeBands]);
      GmatStringUtil::ToReal(data[2], scaleHeight[altitudeBands]);

      ++altitudeBands;
   }
   
   inStream.close();
   
   return true;
}


//------------------------------------------------------------------------------
// bool Density(Real *position, Real *density, Real epoch, Integer count)
//------------------------------------------------------------------------------
/**
 * Calculates the density at each of the states in the input vector using
 * Vallado's method to interpolate the densities.
 * 
 * @param <pos>      The input vector of spacecraft states
 * @param <density>  The array of output densities
 * @param <epoch>    The current TAIJulian epoch (unused here)
 * @param <count>    The number of spacecraft contained in pos
 *
 * @return true on success, throws on failure.
 */
//------------------------------------------------------------------------------
bool ExponentialAtmosphere::Density(Real *position, Real *density, Real epoch,
                                    Integer count)
{
   if (!refDensity || !refHeight || !scaleHeight)
      throw AtmosphereException("Exponential atmosphere not initialized");
    
   if (centralBodyLocation == NULL)
      throw AtmosphereException("Exponential atmosphere: Central body vector "
             "was not initialized");
        
   Real loc[3], height;
   Integer i, index;
    
   for (i = 0; i < count; ++i)
   {
      loc[0] = position[ i*6 ];
      loc[1] = position[i*6+1];
      loc[2] = position[i*6+2];
        
      height = CalculateGeodetics(loc, epoch);
      if (height < 0.0)
         throw AtmosphereException("Exponential atmosphere: Position vector "
                                   "is inside central body");
            
      index = FindBand(height);
      if (smoothDensity)
         density[i] = Smooth(height, index);
      else
         density[i] = refDensity[index] * exp(-(height - refHeight[index]) /
                                                scaleHeight[index]);
   }

   #ifdef DEBUG_DENSITY
      MessageInterface::ShowMessage("Height: %lf, Density: %le\n", height, density[0]);
   #endif

   return true;
}


//------------------------------------------------------------------------------
// void SetConstants()
//------------------------------------------------------------------------------
/**
 * Builds 3 arrays corresponding to the columns in Vallado's Table 8-4.
 * 
 * Users that want to build other atmosphere models that have the same form as
 * Vallado's (and Wertz's) can derive a class from this one and override this
 * method with their choice of constants.
 * NOTE: this code assumes that altitudeBands = 28
 */
//------------------------------------------------------------------------------
void ExponentialAtmosphere::SetConstants()
{
   // Delete old array first
   if (scaleHeight)
      delete [] scaleHeight;
   
   scaleHeight = new Real[altitudeBands];
   if (!scaleHeight)
      throw AtmosphereException("Unable to allocate scaleHeight array for "
            "ExponentialAtmosphere model");
   
   // Delete old array first
   if (refHeight)
      delete [] refHeight;
   
   refHeight   = new Real[altitudeBands];
   if (!refHeight) {
      delete [] scaleHeight;
      scaleHeight = NULL;
      throw AtmosphereException("Unable to allocate refHeight array for "
            "ExponentialAtmosphere model");
   }
   
   // Delete old array first
   if (refDensity)
      delete [] refDensity;
   
   refDensity  = new Real[altitudeBands];
   if (!refDensity) {
      delete [] scaleHeight;
      scaleHeight = NULL;
      delete [] refHeight;
      refHeight = NULL;
      throw AtmosphereException("Unable to allocate refDensity array for "
            "ExponentialAtmosphere model");
   }
   
   // The following assignments contain the data in the table in Vallado,
   // p. 534.  These values are identical to the nominal values in Wertz p. 820.
   refHeight[0]    = 0.0;
   refDensity[0]   = 1.225;
   scaleHeight[0]  = 7.249;
   refHeight[1]    = 25.0;
   refDensity[1]   = 3.899e-2;
   scaleHeight[1]  = 6.349;
   refHeight[2]    = 30.0;
   refDensity[2]   = 1.774e-2;
   scaleHeight[2]  = 6.682;
   refHeight[3]    = 40.0;
   refDensity[3]   = 3.972e-3;
   scaleHeight[3]  = 7.554;
   refHeight[4]    = 50.0;
   refDensity[4]   = 1.057e-3;
   scaleHeight[4]  = 8.382;
   refHeight[5]    = 60.0;
   refDensity[5]   = 3.206e-4;
   scaleHeight[5]  = 7.714;
   refHeight[6]    = 70.0;
   refDensity[6]   = 8.770e-5;
   scaleHeight[6]  = 6.549;
   refHeight[7]    = 80.0;
   refDensity[7]   = 1.905e-5;
   scaleHeight[7]  = 5.799;
   refHeight[8]    = 90.0;
   refDensity[8]   = 3.396e-6;
   scaleHeight[8]  = 5.382;
   refHeight[9]    = 100.0;
   refDensity[9]   = 5.297e-7;
   scaleHeight[9]  = 5.877;
   refHeight[10]   = 110.0;
   refDensity[10]  = 9.661e-8;
   scaleHeight[10] = 7.263;
   refHeight[11]   = 120.0;
   refDensity[11]  = 2.438e-8;
   scaleHeight[11] = 9.473;
   refHeight[12]   = 130.0;
   refDensity[12]  = 8.484e-9;
   scaleHeight[12] = 12.636;
   refHeight[13]   = 140.0;
   refDensity[13]  = 3.845e-9;
   scaleHeight[13] = 16.149;
   refHeight[14]   = 150.0;
   refDensity[14]  = 2.070e-9;
   scaleHeight[14] = 22.523;
   refHeight[15]   = 180.0;
   refDensity[15]  = 5.464e-10;
   scaleHeight[15] = 29.740;
   refHeight[16]   = 200.0;
   refDensity[16]  = 2.789e-10;
   scaleHeight[16] = 37.105;
   refHeight[17]   = 250.0;
   refDensity[17]  = 7.248e-11;
   scaleHeight[17] = 45.546;
   refHeight[18]   = 300.0;
   refDensity[18]  = 2.418e-11;
   scaleHeight[18] = 53.628;
   refHeight[19]   = 350.0;
   refDensity[19]  = 9.518e-12;     /// @note This coefficient was corrected
                                    /// from Vallado's value of 9.158e-12
   scaleHeight[19] = 53.298;
   refHeight[20]   = 400.0;
   refDensity[20]  = 3.725e-12;
   scaleHeight[20] = 58.515;
   refHeight[21]   = 450.0;
   refDensity[21]  = 1.585e-12;
   scaleHeight[21] = 60.828;
   refHeight[22]   = 500.0;
   refDensity[22]  = 6.967e-13;
   scaleHeight[22] = 63.822;
   refHeight[23]   = 600.0;
   refDensity[23]  = 1.454e-13;
   scaleHeight[23] = 71.835;
   refHeight[24]   = 700.0;
   refDensity[24]  = 3.614e-14;
   scaleHeight[24] = 88.667;
   refHeight[25]   = 800.0;
   refDensity[25]  = 1.170e-14;
   scaleHeight[25] = 124.64;
   refHeight[26]   = 900.0;
   refDensity[26]  = 5.245e-15;
   scaleHeight[26] = 181.05;
   refHeight[27]   = 1000.0;
   refDensity[27]  = 3.019e-15;
   scaleHeight[27] = 268.00;
}


//------------------------------------------------------------------------------
// Integer FindBand(Real height)
//------------------------------------------------------------------------------
/**
 * Determines which altitude band the point of interest occupies.
 * 
 * @param <height>  The height above the body's reference ellipsoid.
 * 
 * @return The index of the corresponding band.
 */
//------------------------------------------------------------------------------
Integer ExponentialAtmosphere::FindBand(Real height)
{
   Integer index = altitudeBands-1;
   
   for (Integer i = 0; i < altitudeBands-1; ++i) {
      if (height < refHeight[i+1]) {
         index = i;
         break;
      }
   }
         
   return index;
}


//------------------------------------------------------------------------------
// Real Smooth(Real height, Integer index)
//------------------------------------------------------------------------------
/**
 * Smooths discontinuities between the altitude bands.
 * 
 * @param <height>  The height above the body's reference ellipsoid.
 * @param <index>   The index corresponding to this height.
 * 
 * @return The smoothed density.
 * 
 * @note Smoothing has not been implemented in this build because integration 
 *       seems stable across the small discontinuities in Vallado's model.
 */
//------------------------------------------------------------------------------
Real ExponentialAtmosphere::Smooth(Real height, Integer index)
{
   throw AtmosphereException("Smoothing not yet coded for Exponential Drag");
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone the object (inherited from GmatBase).
 *
 * @return a clone of "this" object.
 */
//------------------------------------------------------------------------------
GmatBase* ExponentialAtmosphere::Clone() const
{
   return (new ExponentialAtmosphere(*this));
}

