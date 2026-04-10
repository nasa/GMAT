//$Id$
//------------------------------------------------------------------------------
//                              ProcessNoiseModel
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. under
// contract number NNG15CR67C
//
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2018/09/17
/**
 * The resource for process noise models
 */
//------------------------------------------------------------------------------

#ifndef ProcessNoiseModel_hpp
#define ProcessNoiseModel_hpp

#include "kalman_defs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix66.hpp"
#include "ProcessNoiseBase.hpp"

/**
 * Defines interfaces used in the ProcessNoiseModel, contained in libEKF.
 */
class KALMAN_API ProcessNoiseModel : public GmatBase
{
public:
   ProcessNoiseModel(const std::string &name);

   virtual ~ProcessNoiseModel();

   ProcessNoiseModel(const ProcessNoiseModel& pnm);
   ProcessNoiseModel& operator=(const ProcessNoiseModel& pnm);

   virtual void         SetSolarSystem(SolarSystem *ss);

   virtual Rmatrix66    GetProcessNoise(const GmatEpoch elapsedTime, const GmatTime &epoch);

   void                 SetNoiseModel(ProcessNoiseBase *noise);

   // Override from GmatBase
   virtual std::string  GetRefObjectName(const UnsignedInt type) const;

   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");

   virtual GmatBase*    Clone(void) const;

   virtual GmatBase*    GetOwnedObject(Integer whichOne);
   virtual bool         IsOwnedObject(Integer id) const;

   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;

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
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual const Rvector& GetRvectorParameter(const Integer id) const;
   virtual const Rvector& GetRvectorParameter(const std::string &label) const;
   virtual const Rvector& SetRvectorParameter(const Integer id,
                                              const Rvector &value);
   virtual const Rvector& SetRvectorParameter(const std::string &label,
                                              const Rvector &value);

   DEFAULT_TO_NO_CLONES

protected:
   /// Name of the process noise model
   std::string              noiseModelName;
   /// The process noise model owned object
   ProcessNoiseBase        *noiseModel;

   /// Solar system needed for noiseModel
   SolarSystem             *solarSystem;

   /// Time step for noise updates
   Real             updateTimeStep = 0;

   /// Parameter IDs for the ProcessNoiseModel
   enum
   {
      NOISE_TYPE = GmatBaseParamCount,
      UPDATE_TIME_STEP,

      // owned parameters
      COORD_SYS,
      RATE_VECTOR,
      ACCEL_SIGMA_VECTOR,
      ProcessNoiseModelParamCount
   };

   /// Strings describing the ProcessNoiseModel parameters
   static const std::string
                           PARAMETER_TEXT[ProcessNoiseModelParamCount -
                                              GmatBaseParamCount];
   /// Types of the ProcessNoiseModel parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[ProcessNoiseModelParamCount -
                                              GmatBaseParamCount];

private:

   Integer GetOwnedObjectId(Integer id, UnsignedInt objType) const;
};

#endif // ProcessNoiseModel_hpp
