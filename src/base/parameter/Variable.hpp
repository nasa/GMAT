//$Id$
//------------------------------------------------------------------------------
//                                Variable
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
// Created: 2004/09/15
//
/**
 * Declares Variable class.
 */
//------------------------------------------------------------------------------
#ifndef Variable_hpp
#define Variable_hpp

#include "gmatdefs.hpp"
#include "RealVar.hpp"
#include "ParameterDatabase.hpp"
#include "ExpressionParser.hpp"

class GMAT_API Variable : public RealVar
{
public:

   Variable(const std::string &name, const std::string &valStr = "",
            const std::string &desc = "", const std::string &unit = "");
   Variable(const Variable &copy);
   Variable& operator=(const Variable &right);
   virtual ~Variable();

   // methods inherited from RealVar
   virtual Real GetReal() const;
   virtual Real EvaluateReal();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual void Copy(const GmatBase*);
   
   virtual bool IsParameterCommandModeSettable(const Integer id) const;

   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
   
   virtual bool RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName);
   
   virtual std::string GetRefObjectName(const UnsignedInt type) const;
   virtual bool SetRefObjectName(const UnsignedInt type,
                                 const std::string &name);
   virtual GmatBase* GetRefObject(const UnsignedInt type,
                                  const std::string &name);
   virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name = "");
   
   virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type);
   
   virtual const std::string& GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName);
protected:
   
   // Currently simple math expression (+-*/^) is no longer allowed in object mode.
   // Math equations are allowed in only command mode.
   // Save this for future? (LOJ: 2010.11.24)
   #ifdef __ALLOW_SIMPLE_VAR_EXP__
   ParameterDatabase *mParamDb;
   ExpressionParser *mExpParser;
   void CreateSimpleExpression();
   void CopySimpleExpression(const Variable &copy);
   void AssignSimpleExpression(const Variable &right);
   void DeleteSimpleExpression();
   #endif
};

#endif // Variable_hpp
