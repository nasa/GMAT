//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under purchase
// order NNG16LD52P
//
//
/**
 * Definition of the Conical FOV class.  This class models a conical FOV.
 */
//------------------------------------------------------------------------------
#ifndef ConicalFOV_hpp
#define ConicalFOV_hpp

#include "gmatdefs.hpp"
#include "FieldOfView.hpp"

class ConicalFOV : public FieldOfView
{
public:

   /// class construction/destruction
	ConicalFOV(const std :: string &itsName = "");
	ConicalFOV( const ConicalFOV &copy);
	ConicalFOV& operator=(const ConicalFOV &copy);

   virtual ~ConicalFOV();

   /// Check the target visibility given the input unit vector:
   /// determines whether or not the point is in the sensor FOV.
   virtual bool     CheckTargetVisibility(const Rvector3 &target);

   /// Check the target visibility given the input cone and clock angles:
   /// determines whether or not the point is in the sensor FOV.
   virtual bool  CheckTargetVisibility(Real viewConeAngle,
                                       Real viewClockAngle = 0.0);
   virtual Rvector  GetMaskConeAngles();
   virtual Rvector  GetMaskClockAngles();

   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
  // virtual void         Copy(const GmatBase* inst);


    // Parameter access methods - overridden from GmatBase
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;

    virtual Real GetRealParameter(const Integer id) const;
    virtual Real GetRealParameter(const std::string &label) const;
    virtual Real SetRealParameter(const Integer id, const Real value);
    virtual Real SetRealParameter(const std::string &label,const Real value);

    DEFAULT_TO_NO_REFOBJECTS

protected:

   /// Field-of-View (radians)
   Real                  fieldOfViewAngle;
   enum
   {
      FIELD_OF_VIEW_ANGLE = FieldOfViewParamCount,
      ConicalFOVParamCount
   };

   /// Hardware Parameter labels
   static const std::string
                   PARAMETER_TEXT[ConicalFOVParamCount - FieldOfViewParamCount];
   /// Hardware Parameter types
   static const Gmat::ParameterType
                   PARAMETER_TYPE[ConicalFOVParamCount - FieldOfViewParamCount];
};
#endif // ConicalFOV_hpp
