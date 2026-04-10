//$Id:  $
//------------------------------------------------------------------------------
//                              RelativisticCorrection
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
// number S-67573-G
//
// Author: W. Shoan
// Created: 2011.06.28
///
/**
 * This is the RelativisticCorrection class.
 *
 * @note original prolog information included at end of file prolog.
 */
#ifndef RelativisticCorrection_hpp
#define RelativisticCorrection_hpp

#include "PhysicalModel.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "gmatdefs.hpp"

/**
 * Relativistic correction model -- models the relativistic correction
 * for the ODEModel.
 */
class GMAT_API RelativisticCorrection : public PhysicalModel
{
public:
   RelativisticCorrection(const std::string &name = "", const std::string &forBodyName = "");
   virtual ~RelativisticCorrection();
   RelativisticCorrection(const RelativisticCorrection &rc);
   RelativisticCorrection& operator=(const RelativisticCorrection &rc);

   virtual bool Initialize();
   virtual bool GetDerivatives(Real *state, Real dt = 0.0, Integer order = 1,
                               const Integer id = -1);
   virtual Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc);

   virtual void SetEopFile(EopFile *eopF);


   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   // Parameter access methods - overridden from GmatBase
   virtual std::string         GetParameterText(const Integer id) const;
   virtual Integer             GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string         GetParameterTypeString(const Integer id) const;
   virtual bool                IsParameterReadOnly(const Integer id) const;

   virtual Real                GetRealParameter(const Integer id) const;
   virtual Real                SetRealParameter(const Integer id, const Real value);

   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index,
                         Integer quantity, Integer sizeOfType);

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:

   /// Pointer to the source of planetary ephemerides
   CelestialBody *theSun;

   /// Radius of the body
   Real bodyRadius;
   /// Gravitational parameter of the central body expressed in the local celestial
   /// body J2000 frame
   Real bodyMu;
   /// Gravitational parameter of the Sun expressed in the local celestial
   /// body J2000 frame
   Real sunMu;
   /// Spin rate of the body
   Real bodySpinRate;

   A1Mjd    now;

   /// Number of spacecraft in the state vector that use CartesianState
   Integer          satCount;

   /// Inertial coordinate system for the body
   CoordinateSystem *bodyInertial;
   /// Fixed coordinate system for the body
   CoordinateSystem *bodyFixed;

   EopFile          *eop;

   CoordinateConverter cc;


private:

   /// Parameter IDs
   enum
   {
      BODY_RADIUS = PhysicalModelParamCount,
      BODY_MU,
      RelativisticCorrectionParamCount  // Count of the parameters for this class
   };

   static const std::string PARAMETER_TEXT[RelativisticCorrectionParamCount - PhysicalModelParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[RelativisticCorrectionParamCount - PhysicalModelParamCount];

};

#endif

