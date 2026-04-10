//$Id$
//------------------------------------------------------------------------------
//                                  RealVar
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/08
//
/**
 * Declares base class of parameters returning Real.
 */
//------------------------------------------------------------------------------
#ifndef RealVar_hpp
#define RealVar_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"

class GMAT_API RealVar : public Parameter
{
public:

   RealVar(const std::string &name = "", const std::string &valStr = "",
           const std::string &typeStr = "RealVar",
           GmatParam::ParameterKey key = GmatParam::USER_PARAM,
           GmatBase *obj = NULL, const std::string &desc = "",
           const std::string &unit = "",
           GmatParam::DepObject depObj = GmatParam::NO_DEP,
           UnsignedInt ownerType = Gmat::UNKNOWN_OBJECT,
           bool isTimeParam = false, bool isSettable = false,
           bool isPlottable = true, bool isReportable = true,
           UnsignedInt ownedObjType = Gmat::UNKNOWN_OBJECT);
   RealVar(const RealVar &copy);
   RealVar& operator= (const RealVar& right);
   virtual ~RealVar();
   
   bool operator==(const RealVar &right) const;
   bool operator!=(const RealVar &right) const;
   
   // methods inherited from Parameter
   virtual bool Initialize();
   virtual std::string ToString();
   
   virtual Real GetReal() const;
   virtual void SetReal(Real val);
   
   virtual Integer GetParameterID(const std::string &str) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual bool IsParameterReadOnly(const Integer id) const;

   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
   
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
protected:

   enum
   {
      VALUE = ParameterParamCount,
      RealVarParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[RealVarParamCount - ParameterParamCount];
   static const std::string
      PARAMETER_TEXT[RealVarParamCount - ParameterParamCount];
   
   bool mValueSet;
   bool mIsNumber;
   Real mRealValue;
   
private:

};
#endif // RealVar_hpp
