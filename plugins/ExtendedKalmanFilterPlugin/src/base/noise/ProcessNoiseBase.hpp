//$Id$
//------------------------------------------------------------------------------
//                              ProcessNoiseBase
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
 * The base class for process noise models
 */
//------------------------------------------------------------------------------

#ifndef ProcessNoiseBase_hpp
#define ProcessNoiseBase_hpp

#include "kalman_defs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix66.hpp"
#include "CoordinateSystem.hpp"

/**
 * Defines interfaces used in the ProcessNoiseBase, contained in libEKF.
 */
class KALMAN_API ProcessNoiseBase : public GmatBase
{
public:
   ProcessNoiseBase(const std::string &itsTypeName, 
      const std::string &itsName, const std::string &itsShortName);

   virtual ~ProcessNoiseBase();

   ProcessNoiseBase(const ProcessNoiseBase& pnb);
   ProcessNoiseBase& operator=(const ProcessNoiseBase& pnb);

   virtual std::string     GetShortName();
   virtual void            SetSolarSystem(SolarSystem *ss);
   virtual bool            Initialize();

   virtual Rmatrix66       GetProcessNoise(const GmatEpoch elapsedTime, const GmatTime &epoch) = 0;

   // Override from GmatBase
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   //virtual bool         IsParameterReadOnly(const Integer id) const;
   //virtual bool         IsParameterReadOnly(const std::string &label) const;
   //virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);

   virtual std::string  GetRefObjectName(const UnsignedInt type) const;

   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   DEFAULT_TO_NO_CLONES

protected:

   std::string          shortName;

   /// Solar system needed for CoordinateSystem
   SolarSystem          *solarSystem;

   bool                 needsReinit;

   std::string          coordSysName;
   CoordinateSystem*    coordinateSystem;
   CoordinateSystem*    j2k;

   virtual void         ConvertMatrix(Rmatrix &mat, const GmatTime &epoch);
   virtual void         ConvertMatrix(Rmatrix &mat, const Real &epoch);

   /// Parameter IDs for the ProcessNoiseBase
   enum
   {
      COORD_SYS = GmatBaseParamCount,
      ProcessNoiseBaseParamCount
   };

   /// Strings describing the ProcessNoiseModel parameters
   static const std::string
                           PARAMETER_TEXT[ProcessNoiseBaseParamCount -
                                              GmatBaseParamCount];
   /// Types of the ProcessNoiseModel parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[ProcessNoiseBaseParamCount -
                                              GmatBaseParamCount];
};

#endif // ProcessNoiseBase_hpp
