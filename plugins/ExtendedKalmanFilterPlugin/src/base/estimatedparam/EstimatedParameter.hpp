//$Id$
//------------------------------------------------------------------------------
//                             EstimatedParameter
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
// Created: 2020/02/04
/**
 * The resource for estimated parameters
 */
//------------------------------------------------------------------------------

#ifndef EstimatedParameter_hpp
#define EstimatedParameter_hpp

#include "kalman_defs.hpp"
#include "GmatBase.hpp"
#include "EstimatedParameterModel.hpp"

/**
 * Defines interfaces used in the EstimatedParameter, contained in libEKF.
 */
class KALMAN_API EstimatedParameter : public GmatBase
{
public:
   EstimatedParameter(const std::string &name);

   virtual ~EstimatedParameter();

   EstimatedParameter(const EstimatedParameter& ep);
   EstimatedParameter& operator=(const EstimatedParameter& ep);

   virtual Rmatrix      GetProcessNoise(GmatEpoch elapsedTime, const Real accelVector[3]);
   virtual void         SetConversionFactor(Real factor);

   void                 SetEstimatedParameterModel(EstimatedParameterModel *param);

   // Override from GmatBase
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

   DEFAULT_TO_NO_CLONES

protected:
   /// Name of the owned EstimatedParameterModel
   std::string              estimatedParameterModelName;
   /// The EstimatedParameterModel owned object
   EstimatedParameterModel   *estimatedParameterModel;

   /// Parameter IDs for the EstimatedParameter
   enum
   {
      EST_PARAM_TYPE = GmatBaseParamCount,

      // owned parameters
      SOLVE_FOR,
      VALUE,
      SIGMA,
      HALF_LIFE,
      EstimatedParameterParamCount
   };

   /// Strings describing the EstimatedParameter parameters
   static const std::string
                           PARAMETER_TEXT[EstimatedParameterParamCount -
                                              GmatBaseParamCount];
   /// Types of the EstimatedParameter parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[EstimatedParameterParamCount -
                                              GmatBaseParamCount];

private:

   Integer GetOwnedObjectId(Integer id, UnsignedInt objType) const;
};

#endif // EstimatedParameter_hpp
