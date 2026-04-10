//$Id$
//------------------------------------------------------------------------------
//                               Hardware
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
// Author: Darrel J. Conway
// Created: 2004/11/08
// Modified: by Mike Stark to add option of representing field of view
// Date modified: 2019/05/xx
//
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class definition for the Hardware base class.
 */
//------------------------------------------------------------------------------


#ifndef HARDWARE_HPP
#define HARDWARE_HPP

#include "GmatBase.hpp"
#include "FieldOfView.hpp"
#include "Rmatrix33.hpp" // for rotation from spacecraft body to hardware frame
#include "Rvector3.hpp"  // to define target vector in FOV visibility check

/**
 * Base class used for spacecraft hardware.
 *
 * This class is the base class for spacecraft fuel tanks, thrusters, and other
 * hardware elements that can be added to a spacecraft in GMAT.  It contains
 * data structures that locate the center of the element in the spacecraft's
 * body coordinate system (BCS) and that orient the elements in the same system.
 *
 * This class has been modified (May 2019) to allow modeling fields of view.
 * FOV modeling includes determining whether a unit vector is in the FOV, and
 * returning the field of view mask for graphics display.
 *
 * @note The current builds of GMAT do not model torques or moments of inertia,
 * so the parameter access for those pieces is commented out.
 */
class GMAT_API Hardware : public GmatBase
{
public:
   Hardware(UnsignedInt typeId, const std::string &typeStr,
            const std::string &nomme = "");
   virtual ~Hardware();
   Hardware(const Hardware& hw);
   Hardware&               operator=(const Hardware& hw);

   /// Field of View Operations
   virtual bool        HasFOV();
   virtual Rmatrix33&  GetRotationMatrix();
//   virtual bool        CheckTargetVisibility(const Rvector3 &target);
//   virtual Rvector     GetMaskConeAngles() const;
//   virtual Rvector     GetMaskClockAngles() const;

   virtual Rvector3    GetLocation() const;
   virtual Rvector3    GetDirection() const;
   virtual Rvector3    GetSecondDirection() const;

   // Parameter access methods - overridden from GmatBase
   virtual std::string        GetParameterText(const Integer id) const;
   virtual Integer            GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                              GetParameterType(const Integer id) const;
   virtual std::string        GetParameterTypeString(const Integer id) const;

   virtual bool        IsParameterReadOnly(const Integer id) const;
   virtual bool        IsParameterReadOnly(const std::string &label) const;

   virtual bool        IsParameterVisible(const Integer id) const;

   // string parameter operations for FOV name
   virtual std::string GetStringParameter (const Integer id) const;
   virtual std::string GetStringParameter (const std::string &label) const;
   virtual bool        SetStringParameter (const Integer id,
                                           const std::string &value);
   virtual bool        SetStringParameter (const std::string &label,
                                           const std::string &value);

   virtual Real               GetRealParameter(const Integer id) const;
   virtual Real               SetRealParameter(const Integer id,
                                               const Real value);
   virtual Real               GetRealParameter(const std::string &label) const;
   virtual Real               SetRealParameter(const std::string &label,
                                         const Real value);

   // used by subclasses
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);

   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);

// pass throughs of subclass parameter calls
   virtual const Rvector&
                        GetRvectorParameter(const Integer id) const;
   virtual const Rvector&
                        SetRvectorParameter(const Integer id,
                                            const Rvector &value);
   virtual const Rvector&
                        GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&
                        SetRvectorParameter(const std::string &label,
                                            const Rvector &value);

   virtual const Rmatrix&  GetRmatrixParameter(const Integer id) const;
   virtual const Rmatrix&  SetRmatrixParameter(const Integer id,
                                               const Rmatrix &value);

   virtual const Rmatrix&  GetRmatrixParameter(const std::string &label) const;
   virtual const Rmatrix&  SetRmatrixParameter(const std::string &label,
                                               const Rmatrix &value);


   //--------------------------------------------
   // Reference object functions
   //--------------------------------------------
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual bool         HasRefObjectTypeArray();
   virtual const        ObjectTypeArray& GetRefObjectTypeArray();
   virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type);

   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);

   virtual bool         SetRefObjectName(const UnsignedInt type,
                                         const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");

   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
// pass throughs of subclass reference object calls
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);

   /// initialize body to sensor rotation
   virtual bool         Initialize();

   DEFAULT_TO_NO_CLONES

protected:
   /// Location of center of the hardware element on the spacecraft, in meters.
   Real                    location[3];
   /// Principle direction for hardware element on the spacecraft.
   Real                    direction[3];
   /// Secondary direction, to complete the orientation.
   Real                    secondDirection[3];
   /// The coordinates used for direction and location settings
   std::string             coordinates;
   /// The allowable coordinate types to use for setting rotations and origin
   StringArray             coordinateTypes;
   /// The rotation input used for DCM, Euler, and Construct settings
   ///                   - DCM      : Direction Cosine Matrix
   ///                   - Euler    : Compute rotation matrix from Euler angles
   ///                   - Construct: Compute rotation matrix from two
   ///                                non-parallel vectors
   std::string             rotationInput;
   /// The allowable rotation input types to use for setting DCM, Euler, and
   /// Construct.
   StringArray             rotationInputTypes;
   /// Field of view flag, used to identify view enabled Hardware objects
//   bool                    fovIsModeled;
//   FieldOfView             *fov;
//   std::string             fovName;

   /// rotation from body to hardware frame
   Rmatrix33               R_SB;  //******* compute from direction & second direction? ******

   /// Enumeration defining user accessible parameters for Hardware elements.
   enum
   {
      // Use direction vectors orient the hardware
      COORDINATES = GmatBaseParamCount,
      ROTATION_INPUT_TYPE,
      DIRECTION_X,
      DIRECTION_Y,
      DIRECTION_Z,
      SECOND_DIRECTION_X,
      SECOND_DIRECTION_Y,
      SECOND_DIRECTION_Z,

      // Location of hardware frame origin in s/c body frame.  This point is
      // usually at the center of mass or geometric center of the object
      HW_ORIGIN_BCS_X,
      HW_ORIGIN_BCS_Y,
      HW_ORIGIN_BCS_Z,
      ROTATION_MATRIX,
      R_SB11,
      R_SB12,
      R_SB13,
      R_SB21,
      R_SB22,
      R_SB23,
      R_SB31,
      R_SB32,
      R_SB33,
      HardwareParamCount
   };

   /// Hardware Parameter labels
   static const std::string
                        PARAMETER_TEXT[HardwareParamCount - GmatBaseParamCount];
   /// Hardware Parameter types
   static const Gmat::ParameterType
                        PARAMETER_TYPE[HardwareParamCount - GmatBaseParamCount];
};

#endif // HARDWARE_HPP
