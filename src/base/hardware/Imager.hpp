//------------------------------------------------------------------------------
//                           Imager
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS III
// contract, Task Order 112
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 17, 2021
/**
 * Imager intermediate class used to isolate FoV using hardware from other
 * hardware components
 */
//------------------------------------------------------------------------------

#ifndef Imager_hpp
#define Imager_hpp

#include "gmatdefs.hpp"
#include "Hardware.hpp"
#include "FieldOfView.hpp"

class GMAT_API Imager: public Hardware
{
public:
   Imager(UnsignedInt typeId, const std::string &ofType, const std::string &withName);
   virtual ~Imager();
   Imager(const Imager &other);
   Imager(std::string FOVFileName, std::string &withName); // Alternate Constructor for Default Groundstation MaskFile
   Imager& operator=(const Imager &other);

   virtual GmatBase* Clone() const;

   virtual bool Initialize();

   // Script Interfaces
   // Parameter access methods - overridden from GmatBase
   virtual std::string        GetParameterText(const Integer id) const;
   virtual Integer            GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                              GetParameterType(const Integer id) const;
   virtual std::string        GetParameterTypeString(const Integer id) const;

   virtual bool               IsParameterReadOnly(const Integer id) const;
   virtual bool               IsParameterReadOnly(const std::string &label) const;

   virtual bool               IsParameterVisible(const Integer id) const;

   // string parameter operations for FOV name
   virtual std::string        GetStringParameter (const Integer id) const;
   virtual std::string        GetStringParameter (const std::string &label) const;
   virtual bool               SetStringParameter (const Integer id,
                                           const std::string &value);
   virtual bool               SetStringParameter (const std::string &label,
                                           const std::string &value);

   virtual Real               GetRealParameter(const Integer id) const;
   virtual Real               SetRealParameter(const Integer id,
                                               const Real value);
   virtual Real               GetRealParameter(const std::string &label) const;
   virtual Real               SetRealParameter(const std::string &label,
                                         const Real value);

   virtual Integer               GetIntegerParameter(const Integer id) const;
   virtual Integer               SetIntegerParameter(const Integer id,
                                               const Integer value);
   virtual Integer               GetIntegerParameter(const std::string &label) const;
   virtual Integer               SetIntegerParameter(const std::string &label,
                                         const Integer value);


   // used by subclasses
   virtual Real               GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real               SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real               GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real               SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);

   virtual std::string        GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool               SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);

// pass throughs of subclass parameter calls
   virtual const Rvector&     GetRvectorParameter(const Integer id) const;
   virtual const Rvector&     SetRvectorParameter(const Integer id,
                                            const Rvector &value);
   virtual const Rvector&     GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&     SetRvectorParameter(const std::string &label,
                                            const Rvector &value);


   // Reference Objects
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


   virtual bool        CheckTargetVisibility(const Rvector3 &target);
   virtual Rvector     GetMaskConeAngles() const;
   virtual Rvector     GetMaskClockAngles() const;

   virtual FieldOfView* GetFieldOfView();
   virtual bool        HasFOV();

   DEFAULT_TO_NO_CLONES

protected:
   /// optional field of view flag, object & its name
   bool                    fovIsModeled;
   FieldOfView             *fov;
   std::string             fovName;
   Real                    naifID;

   /// Enumeration defining user accessible parameters for Hardware elements.
   enum
   {
      FOV_MODEL = HardwareParamCount,

      // Add read-only parameters for mask angles
      MASK_CONE_ANGLES,
      MASK_CLOCK_ANGLES,
      NAIF_ID,
      ImagerParamCount
   };

   /// Hardware Parameter labels
   static const std::string
                        PARAMETER_TEXT[ImagerParamCount - HardwareParamCount];
   /// Hardware Parameter types
   static const Gmat::ParameterType
                        PARAMETER_TYPE[ImagerParamCount - HardwareParamCount];

};

#endif /* Imager_hpp */
