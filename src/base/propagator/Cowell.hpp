//$Id$
// **************************************************************************
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
// Author: Waka Waktola
// Created: 2005/05/16
//
/**
 * @note This code is a shell for the Cowell integrator.  The integrator is not
 *       currently implemented.
 */
// **************************************************************************

#ifndef Cowell_hpp
#define Cowell_hpp

#include "gmatdefs.hpp"
#include "Integrator.hpp"

/**
 * The Cowell predictor-corrector integrator
 * 
 * This code is a shell for the Cowell integrator.  The integrator is not 
 * yet implemented.
 * 
 */
class GMAT_API Cowell : public PredictorCorrector
{
public:
   Cowell(const std::string &typeStr, const std::string &nomme = "");
   virtual ~Cowell(void);
   Cowell(const Cowell& bs);
   Cowell& operator=(const Cowell& bs);
    
   virtual GmatBase* Clone(void) const;
   virtual bool Initialize();
   virtual bool Step(Real dt);
   virtual bool Step(void);
   virtual bool RawStep(void);
   virtual Real EstimateError(void);
   virtual bool AdaptStep(Real maxerror);

   // Parameter accessor methods -- overridden from GmatBase
   virtual std::string         GetParameterText(const Integer id) const;
   virtual Integer             GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string         GetParameterTypeString(const Integer id) const;
   virtual Real    GetRealParameter(const Integer id) const;
   virtual Real    GetRealParameter(const std::string &label) const;
   virtual Real    SetRealParameter(const Integer id, const Real value);
   virtual Real    SetRealParameter(const std::string &label, const Real value);
        
protected:

   enum
   {  /// Count of the parameters for this class
      CowellParamCount = IntegratorParamCount
   };
   
   // save for possible later use
   //static const std::string
   //PARAMETER_TEXT[CowellParamCount - IntegratorParamCount];
   //static const Gmat::ParameterType
   //PARAMETER_TYPE[CowellParamCount - IntegratorParamCount];

private:
   /// The estimated state
   Real *estimatedState;
};
#endif
