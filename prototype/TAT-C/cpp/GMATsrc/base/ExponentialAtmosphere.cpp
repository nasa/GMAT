//$Id$
//------------------------------------------------------------------------------
//                           ExponentialAtmosphere
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
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
#include <cmath>
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "UtilityException.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------


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
   scaleHeight          (NULL),
   refHeight            (NULL),
   refDensity           (NULL),
   altitudeBands        (28),
   smoothDensity        (false)
{
    SetConstants();
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
 * @param <atm> ExponentialAtmosphere object to copy in creating the new one.
 */
//------------------------------------------------------------------------------
ExponentialAtmosphere::ExponentialAtmosphere(const ExponentialAtmosphere& atm) :
   scaleHeight          (NULL),
   refHeight            (NULL),
   refDensity           (NULL),
   altitudeBands        (atm.altitudeBands),
   smoothDensity        (false)
{
   SetConstants();
}

//------------------------------------------------------------------------------
//  ExponentialAtmosphere& operator= (const ExponentialAtmosphere& atm)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ExponentialAtmosphere class.
 *
 * @param <atm> the ExponentialAtmosphere object whose data to assign to "this"
 *              ExponentialAtmosphere.
 *
 * @return "this" ExponentialAtmosphere with data of input ExponentialAtmosphere
 *          atm.
 */
//------------------------------------------------------------------------------
ExponentialAtmosphere& ExponentialAtmosphere::operator=(
      const ExponentialAtmosphere &atm)
{
   if (&atm == this)
      return *this;

   scaleHeight   = NULL;
   refHeight     = NULL;
   refDensity    = NULL;
   altitudeBands = atm.altitudeBands;
   smoothDensity = false;
   
   SetConstants();
   
   return *this;
}


//------------------------------------------------------------------------------
// bool Density(Real height, Real density)
//------------------------------------------------------------------------------
/**
 * Calculates the density given height above the ellipsoid
 * Vallado's method to interpolate the densities.
 * 
 * @param <height>   Height above ellipsoid
 * @param <density>  density
 *
 * @return true on success, throws on failure.
 */
//------------------------------------------------------------------------------
Real ExponentialAtmosphere::Density(Real height)
{
   if (!refDensity || !refHeight || !scaleHeight)
      throw UtilityException("Exponential atmosphere not initialized");
    

   Integer index= FindBand(height);
   Real density = 1.0e9 * refDensity[index] * exp(-(height - refHeight[index]) /
                  scaleHeight[index]);
   return density;
}

//------------------------------------------------------------------------------
// bool GetScaleHeight(Real height)
//------------------------------------------------------------------------------
/**
* Calculates scale height for exonential model given height
* Vallado's method to interpolate the densities.
*
* @param <height>   Height above ellipsoid
* @param <scale>  scale height, km
*
* @return true on success, throws on failure.
*/
//------------------------------------------------------------------------------
Real ExponentialAtmosphere::GetScaleHeight(Real height)
{
	Integer index = FindBand(height);
	//MessageInterface::ShowMessage("Band Index %12i \n", index);
	Real scale = scaleHeight[index];
	//MessageInterface::ShowMessage("Scale Height in Band %12.10f \n", scale);
	return scale;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void SetConstants()
//------------------------------------------------------------------------------
/**
 * Builds 3 arrays corresponding to the columns in Vallado's Table 8-4.
 * 
 * Users that want to build other atmosphere models that have the same form as
 * Vallado's (and Wertz's) can derive a class from this one and override this
 * method with their choice of constants.
 */
//------------------------------------------------------------------------------
void ExponentialAtmosphere::SetConstants()
{
   // Delete old array first
   if (scaleHeight)
      delete [] scaleHeight;
   
   scaleHeight = new Real[altitudeBands];
   if (!scaleHeight)
      throw UtilityException("Unable to allocate scaleHeight array for "
            "ExponentialAtmosphere model");
   
   // Delete old array first
   if (refHeight)
      delete [] refHeight;
   
   refHeight   = new Real[altitudeBands];
   if (!refHeight)
   {
      delete [] scaleHeight;
      scaleHeight = NULL;
      throw UtilityException("Unable to allocate refHeight array for "
            "ExponentialAtmosphere model");
   }
   
   // Delete old array first
   if (refDensity)
      delete [] refDensity;
   
   refDensity  = new Real[altitudeBands];
   if (!refDensity)
   {
      delete [] scaleHeight;
      scaleHeight = NULL;
      delete [] refHeight;
      refHeight = NULL;
      throw UtilityException("Unable to allocate refDensity array for "
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
   Integer index = altitudeBands - 1;
   //MessageInterface::ShowMessage("Height in FindBand() %12.10f \n", height);
   for (Integer i = 0; i < altitudeBands-1; ++i)
   {
	   //MessageInterface::ShowMessage("Loop Index %12i \n",i);
	   //MessageInterface::ShowMessage("Reference Height in Band %12.10f \n", refHeight[i + 1]);
      if (height < refHeight[i+1])
      {
         index = i;
         break;
      }
   }

   //MessageInterface::ShowMessage("Altitude Band Index %12i \n", index);
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
   throw UtilityException("Smoothing not yet coded for Exponential Drag");
}

//------------------------------------------------------------------------------
//ExponentialAtmosphere* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone the object (inherited from GmatBase).
 *
 * @return a clone of "this" object.
 */
//------------------------------------------------------------------------------
ExponentialAtmosphere* ExponentialAtmosphere::Clone() const
{
   return (new ExponentialAtmosphere(*this));
}

