//$Id$
//------------------------------------------------------------------------------
//                              JacchiaRobertsAtmosphere
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
// Author: Waka A. Waktola
// Created: 2004/05/11
//
/**
 * The Jacchia-Roberts atmosphere.
 *
 * @note This is the JR model, ported from the Swingby/Windows source
 */
//------------------------------------------------------------------------------

#ifndef JacchiaRobertsAtmosphere_hpp
#define JacchiaRobertsAtmosphere_hpp

#include "AtmosphereModel.hpp"
#include "CelestialBody.hpp"
#include "A1Mjd.hpp"
#include "TimeTypes.hpp"

class GMAT_API JacchiaRobertsAtmosphere : public AtmosphereModel
{
public:
   JacchiaRobertsAtmosphere(const std::string &name = "");
   virtual ~JacchiaRobertsAtmosphere();
   JacchiaRobertsAtmosphere(const JacchiaRobertsAtmosphere& jr);
   JacchiaRobertsAtmosphere& operator=(const JacchiaRobertsAtmosphere& jr);
    
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
    
   bool Density(Real *position, Real *density, Real epoch = GmatTimeConstants::MJD_OF_J2000,
                Integer count = 1);
   virtual void SetCentralBody(CelestialBody *cb);

private:
   ///  Auxiliary temperature related quantities
   Real root1;
   Real root2;
   Real x_root;
   Real y_root;

   ///  Some intermediate temperature results used by density computations
   Real t_infinity;
   Real tx;
   Real sum;
   Real cbPolarRadius;
   Real cbPolarSquared;

   /// low altitude density in g/cm**2
   static const Real RHO_ZERO;
   /// Temperature in degrees kelvin at height of 90km
   static const Real TZERO;
   /// earth gravitational constant m/sec**2
   static const Real G_ZERO;
   /// gas constant (joules/(degK-mole))
   static const Real GAS_CON;
   /// Avogadro's number
   static const Real AVOGADRO;

   // Tables of constants from the model (formerly defined as globals)
   /// Constants for series expansion
   static const Real CON_C[5];
   /// Constants for series expansion
   static const Real CON_L[5];
   /// Constants required between 90 km  and 100 km
   static const Real MZERO;
   /// Constants for series expansion
   static const Real M_CON[7];
   /// Constants for series expansion
   static const Real S_CON[6];
   /// Constants for series expansion
   static const Real S_BETA[6];
   /// Constants required for attitudes between 100 km and 125 km
   static const Real OMEGA;
   /// Constants for series expansion
   static const Real ZETA_CON[7];
   /// Molecular masses of atmospheric  constituents in grams/mole
   static const Real MOL_MASS[6];
   /// Number density divided by Avogadro's number of atmospheric constituents
   static const Real NUM_DENS[5];
   // Constants required for altitude greater than 125 km
   /// Polynomial coefficients for constituent densities of each atmospheric gas
   static const Real CON_DEN[5][7];
   /// Struct used to hold F107, Kp, temp
   GEOPARMS geo;



   Real JacchiaRoberts(Real height, Real space_craft[3], Real sun[3],
                      Real a1_time);
   Real exotherm(Real space_craft[3], Real sun[3], GEOPARMS *geo,
                       Real height, Real sun_dec, Real geo_lat);
   Real rho_100(Real height, Real temperature);
   Real rho_125(Real height, Real temperature);
   Real rho_cor(Real height, Real a1_time, Real geo_lat,
                      GEOPARMS *geo);
   Real rho_high(Real height, Real temperature, Real t_500,
                       Real sun_dec, Real geo_lat);
   void roots(Real a[], Integer na, Real croots[][2], Integer irl);
   void deflate_polynomial(Real c[], Integer n, Real root, Real c_new[]);
   Real length_of(Real v[3]);
   Real dot_product(Real a[3] , Real b[3]);
};

#endif // JacchiaRobertsAtmosphere_hpp
