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
 * An exponentially modeled atmosphere based on Vallado, pp 532-534 and Wertz,
 * p 820.
 */
//------------------------------------------------------------------------------

#ifndef ExponentialAtmosphere_hpp
#define ExponentialAtmosphere_hpp

//#include "AtmosphereModel.hpp"
#include "gmatdefs.hpp"
#include "TimeTypes.hpp"
#include "GmatConstants.hpp"

/**
 * The exponential atmosphere model in Vallado (2001) and Wertz (1978).
 * 
 * This code calculates the atmospheric density at a given position based on
 * the altitude of the input state above the spherical Earth.  This model does
 * not include an atmospheric bulge due to solar heating.  The code will need
 * to be refined to use the oblate Earth once oblateness is added to the code.
 * 
 * The density is given by
 * 
 * \f[\rho = \rho_0 e^{-{{h_{ellp} - h_0}\over{H}}}\f]
 * 
 * where \f$\rho_0\f$ is a reference density, specified at a reference altitude
 * \f$h_0\f$, \f$h_{ellp}\f$ is the height of the specified position above the 
 * body's ellipsoid, and \f$H\f$ is a scale height, used to scale the other 
 * variables in the formula.
 * 
 * Developers and other users can build exponential models for bodies other than 
 * the Earth by deriving a class off of this one and overriding the SetConstants
 * method.
 * 
 * @todo Replace the spherical Earth model with an oblate Earth model.
 * @test Check to see if the band discontinuities merit smoothing.
 */
class GMAT_API ExponentialAtmosphere 
{
public:
   ExponentialAtmosphere(const std::string &name = "");
   virtual ~ExponentialAtmosphere();
   ExponentialAtmosphere(const ExponentialAtmosphere& atm);
   ExponentialAtmosphere&  operator=(const ExponentialAtmosphere& atm);
    
   Real            Density(Real height);
   Real            GetScaleHeight(Real height);

protected: 
   /// Table of scale heights, \f$H\f$.
   Real                    *scaleHeight;
   /// Table of Reference heights, \f$h_0\f$.
   Real                    *refHeight;
   /// Table of reference densities, \f$\rho_0\f$.
   Real                    *refDensity;
   /// Number of altitude bands used in the model.
   Integer                 altitudeBands;
   /// Flag indicating if the altitude is "at" a boundary
   bool                    smoothDensity;
    
   virtual void            SetConstants();
   Integer                 FindBand(Real height);
   Real                    Smooth(Real height, Integer index);

   virtual ExponentialAtmosphere* Clone() const; // inherited from GmatBase

private:

};

#endif // ExponentialAtmosphere_hpp
