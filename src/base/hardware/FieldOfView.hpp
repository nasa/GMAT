//------------------------------------------------------------------------------
//                           FieldOfView
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
// Author: Mike Stark, NASA/GSFC
// Created: 2019/04/02
//
/**
 * This class models the field of view. This is an 
 * abstract class; objects must be defined in terms of FieldOfView subclasses
 */
//------------------------------------------------------------------------------
#ifndef FOV_hpp
#define FOV_hpp

#include "GmatBase.hpp"
#include "gmatdefs.hpp"
#include "Rvector3.hpp"  // for checking target visibility
#include "Rvector.hpp"   // to represent sensor mask angles
#include "Rmatrix33.hpp"

class FieldOfView: public GmatBase
{
public:
   
   /// class construction/destruction
   FieldOfView(UnsignedInt typeId, const std::string &typeStr,
		     const std::string &itsName = "");
   FieldOfView(const FieldOfView &copy);
   FieldOfView& operator=(const FieldOfView &copy);
   
   virtual ~FieldOfView();
   
   /// Pure virtual functions for testing if point is in field of view. The
   /// implementation of this function depends on the type of hardware.
   /// the function is used to check if target is in FOV. Vector must be in
   /// the hardware frame to give correct result; the equivalent cone and clock
   /// angles denote a position on unit sphere where sensor boresight is
   /// the +Z axis. The default input value for clock angle allows the second
   /// function to be called with one parameter.
   virtual bool     CheckTargetVisibility(const Rvector3 &target) = 0;
   virtual bool     CheckTargetVisibility(Real targetConeAngle,
                                          Real targetClockAngle = 0.0) = 0;
   
   /// pure virtual functions to get  mask data
   /// implementations are found in subclasses
   virtual Rvector  GetMaskConeAngles()  = 0;
   virtual Rvector  GetMaskClockAngles() = 0;
   
   virtual UnsignedInt        GetColor() const;
   virtual UnsignedInt        GetAlpha() const;
   
   // Parameter access methods - overridden from GmatBase
   virtual std::string        GetParameterText(const Integer id) const;
   virtual Integer            GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                              GetParameterType(const Integer id) const;
   virtual std::string        GetParameterTypeString(const Integer id) const;
   
   virtual Real               GetRealParameter(const Integer id,const Integer index) const;
   virtual Real               SetRealParameter(const Integer id,const Real value, const Integer index);
   virtual Real               GetRealParameter(const Integer id) const;
   virtual Real               SetRealParameter(const Integer id,
                                               const Real value);

   virtual Real               GetRealParameter(const std::string &label) const;
   virtual Real               SetRealParameter(const std::string &label,
                                               const Real value);
 
   virtual const Rvector&  SetRvectorParameter(const Integer id, const Rvector &value);
   
   // added 2/11/20 to handle graphics color & opacity (alpha)
   // string parameter gets & puts, boolean tests used in setting color values
   virtual std::string     GetStringParameter(const Integer id) const;
   virtual bool            SetStringParameter(const Integer id,
                                              const std::string &value);
   virtual std::string     GetStringParameter(const std::string &label) const;
   virtual bool            SetStringParameter(const std::string &label,
                                              const std::string &value);
   
   virtual UnsignedInt     GetUnsignedIntParameter(const Integer id) const;
   virtual UnsignedInt     SetUnsignedIntParameter(const Integer id,
                                                   const UnsignedInt value);
   virtual UnsignedInt     GetUnsignedIntParameter(const std::string &label) const;
   virtual UnsignedInt     SetUnsignedIntParameter(const std::string &label,
                                                   const UnsignedInt value);
   
   virtual bool            IsSquareBracketAllowedInSetting(const Integer id) const;
   virtual bool            IsParameterCommandModeSettable(const Integer id) const;
   
   // Need functions to get and set parameters in the respective subclasses
   // QUESTION -- can we let them inherit directly from GMAT base without
   // intervening functions in FieldOfView?
   DEFAULT_TO_NO_CLONES
protected:
   
   /// The maximum excursion angle defines a circle on the unit sphere that
   /// contains the entire field of view. A simple inequality is all that is
   /// needed to reject such points, which in the case of more complicated
   ///  can save computation time.
   Real          maxExcursionAngle; // Radians
   UnsignedInt   color;
   UnsignedInt   alpha;
   
   virtual bool  CheckTargetMaxExcursionAngle(Real targetConeAngle);

   enum
   {
      COLOR = GmatBaseParamCount,
      ALPHA,
      FieldOfViewParamCount
   };
   
   /// Hardware Parameter labels
   static const std::string
                   PARAMETER_TEXT[FieldOfViewParamCount - GmatBaseParamCount];
   /// Hardware Parameter types
   static const Gmat::ParameterType
                   PARAMETER_TYPE[FieldOfViewParamCount - GmatBaseParamCount];
   
   
   // ***** MOVE THESE FUNCTIONS TO THE CUSTOM FOV CLASS *****
   // Coordinate conversion utilities
 void ConeClocktoRADEC(Real coneAngle, Real clockAngle,
                         Real &RA, Real &dec);
   Rvector3 RADECtoUnitVec(Real RA, Real dec);
   void UnitVecToStereographic(const Rvector3 &u,
                               Real &xCoord, Real &yCoord);
   void ConeClockToStereographic(Real coneAngle, Real clockAngle,
                                 Real &xCoord, Real &yCoord);
   void ConeClockArraysToStereographic(const Rvector &coneAngleVec,
                                       const Rvector &clockAngleVec,
                                       Rvector &xArray, Rvector &yArray);
   void RADECtoConeClock(Real RA, Real dec, Real &coneAngle, Real &clockAngle);
   void UnitVecToRADEC(const Rvector3 v, Real &RA, Real &dec);
 
};
#endif // FOV_hpp
