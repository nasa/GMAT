//$Id$
//------------------------------------------------------------------------------
//                              ExternalModel
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. 
//
// Author: Alex Campbell
// Created: 07/25/2022

#ifndef ExternalModel_hpp
#define ExternalModel_hpp

#include "externalmodel_defs.hpp"
#include "PhysicalModel.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Rvector6.hpp"
#include "gmatdefs.hpp"
#include "PythonInterface.hpp"


class EXTERNALMODEL_API ExternalModel : public PhysicalModel
{
public:
   ExternalModel(const std::string &name = "");
   virtual ~ExternalModel();
   ExternalModel(const ExternalModel &external);
   ExternalModel& operator=(const ExternalModel &external);

   virtual bool IsUnique(const std::string &forBody = "");

   virtual bool Initialize();
   virtual bool GetDerivatives(Real *state, Real dt = 0.0, Integer order = 1, 
                               const Integer id = -1);
   virtual Real* PythonDerivatives(Real *state, Real now, StringArray stateDescription, Integer order = 1,
	   const Integer id = -1);
   virtual Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc);

   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   // Parameter access methods - overridden from GmatBase
   virtual std::string         GetParameterText(const Integer id) const;
   virtual Integer             GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string         GetParameterTypeString(const Integer id) const;
   virtual bool                IsParameterReadOnly(const Integer id) const;
      
   virtual bool                GetBooleanParameter(const Integer id) const;
   virtual bool                SetBooleanParameter(const Integer id, const bool value);
 
   
   virtual std::string         GetStringParameter(const Integer id) const;
   virtual std::string         GetStringParameter(const std::string &label) const;
   virtual bool                SetStringParameter(const Integer id,
                                                  const std::string &value);
   virtual bool                SetStringParameter(const std::string &label,
                                                  const char *value);
   virtual bool                SetStringParameter(const std::string &label,
                                                  const std::string &value);


   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index, 
                         Integer quantity, Integer totalSize);

   virtual Rvector3 GetTorquesForSpacecraft(Spacecraft *sc);

   StringArray GetStateDateTypes();

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS


protected:   

   /// The name of the script file
   std::string		scriptFilename;
   /// The name of the script file
   std::string		fullFilePath;
   /// Flag if other forces should be ignored and just the forces from the script be calculated
   bool				excludeForces;
   /// String for the entry point into the script
   std::string      entryPoint;
   /// String for the torque entry point into the script
   std::string      torqueEntryPoint;
   /// Python Interface singleton
   PythonInterface *pythonIf;
   /// Number of spacecraft in the state vector that use CartesianState
   Integer          satCount;

   bool initializedMessagePrinted;


private:

   /// Parameter IDs
   enum
   {
      SCRIPT_FILENAME = PhysicalModelParamCount,
      EXCLUDE_OTHER_FORCES,
      ENTRY_POINT,  
      ExternalParamCount  // Count of the parameters for this class
   };
   
   static const std::string PARAMETER_TEXT[ExternalParamCount - PhysicalModelParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[ExternalParamCount - PhysicalModelParamCount];

};

#endif

