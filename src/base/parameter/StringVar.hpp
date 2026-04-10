//$Id$
//------------------------------------------------------------------------------
//                                  StringVar
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
// Created: 2005/1/7
//
/**
 * Declares StringVar class which handles std::string value. The string value
 * is stored in Parameter::mExpr.
 */
//------------------------------------------------------------------------------
#ifndef StringVar_hpp
#define StringVar_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"

class GMAT_API StringVar : public Parameter
{
public:

   StringVar(const std::string &name = "",
             const std::string &typeStr = "String",
             GmatParam::ParameterKey key = GmatParam::USER_PARAM,
             GmatBase *obj = NULL, const std::string &desc = "",
             const std::string &unit = "",
             GmatParam::DepObject depObj = GmatParam::NO_DEP,
             UnsignedInt ownerType = Gmat::UNKNOWN_OBJECT,
             bool isTimeParam = false, bool isSettable = false,
             UnsignedInt ownedObjType = Gmat::UNKNOWN_OBJECT);
   StringVar(const StringVar &copy);
   StringVar& operator= (const StringVar& right);
   virtual ~StringVar();
   
   bool operator==(const StringVar &right) const;
   bool operator!=(const StringVar &right) const;

   // methods inherited from Parameter
   virtual std::string ToString();
   virtual const std::string& GetString() const;
   virtual void               SetString(const std::string &val);
   virtual const std::string& EvaluateString();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual void Copy(const GmatBase*);
   
   virtual Integer GetParameterID(const std::string &str) const;
   virtual bool IsParameterCommandModeSettable(const Integer id) const;
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
   
   virtual const std::string& GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName);
protected:
   
   enum
   {
      VALUE = ParameterParamCount,
      StringVarParamCount
   };
   
   static const std::string
      PARAMETER_TEXT[StringVarParamCount - ParameterParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[StringVarParamCount - ParameterParamCount];
   
   std::string mStringValue;
   
private:

};
#endif // StringVar_hpp
