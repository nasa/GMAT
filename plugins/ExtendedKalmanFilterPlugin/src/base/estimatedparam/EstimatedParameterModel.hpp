//$Id$
//------------------------------------------------------------------------------
//                           EstimatedParameterModel
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
 * The base class for estimated parameters
 */
//------------------------------------------------------------------------------

#ifndef EstimatedParameterModel_hpp
#define EstimatedParameterModel_hpp

#include "kalman_defs.hpp"
#include "GmatBase.hpp"
#include "Rmatrix.hpp"
#include "CoordinateSystem.hpp"

/**
 * Defines interfaces used in the EstimatedParameterModel, contained in libEKF.
 */
class KALMAN_API EstimatedParameterModel : public GmatBase
{
public:
   EstimatedParameterModel(const std::string &itsTypeName, 
      const std::string &itsName, const std::string &itsShortName);

   virtual ~EstimatedParameterModel();

   EstimatedParameterModel(const EstimatedParameterModel& epm);
   EstimatedParameterModel& operator=(const EstimatedParameterModel& epm);

   virtual std::string     GetShortName();

   virtual Rmatrix         GetProcessNoise(GmatEpoch elapsedTime,
                                           const Real accelVector[3]) = 0;

   virtual void            SetConversionFactor(Real factor);

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

   const StringArray&   GetAllowedSolveFors() const;

   DEFAULT_TO_NO_CLONES

protected:

   std::string          shortName;

   /// Solve-for variable for this parameter
   std::string          solveFor;
   /// Array of allowed solve-fors
   StringArray          allowedSolveFors;

   /// The conversion factor to go between the nominal and epsilon values
   Real                 convFactor;

   /// Parameter IDs for the EstimatedParameterModel
   enum
   {
      SOLVE_FOR = GmatBaseParamCount,
      EstimatedParameterModelParamCount
   };

   /// Strings describing the EstimatedParameter parameters
   static const std::string
                           PARAMETER_TEXT[EstimatedParameterModelParamCount -
                                              GmatBaseParamCount];
   /// Types of the EstimatedParameter parameters
   static const Gmat::ParameterType
                           PARAMETER_TYPE[EstimatedParameterModelParamCount -
                                              GmatBaseParamCount];
};

#endif // EstimatedParameterModel_hpp
