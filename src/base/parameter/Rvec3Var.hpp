//$Id$
//------------------------------------------------------------------------------
//                                  Rvec3Var
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
// Created: 2004/03/12
//
/**
 * Declares base class of parameters returning Rvector3.
 */
 //------------------------------------------------------------------------------
#ifndef Rvec3Var_hpp
#define Rvec3Var_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "Rvector3.hpp"

class GMAT_API Rvec3Var : public Parameter
{
public:

   Rvec3Var(const std::string &name = "",
      const std::string &typeStr = "Rvec3Var",
      GmatParam::ParameterKey key = GmatParam::USER_PARAM,
      GmatBase *obj = NULL, const std::string &desc = "",
      const std::string &unit = "",
      GmatParam::DepObject depObj = GmatParam::NO_DEP,
      UnsignedInt ownerType = Gmat::UNKNOWN_OBJECT);
   Rvec3Var(const Rvec3Var &copy);
   Rvec3Var& operator= (const Rvec3Var& right);
   virtual ~Rvec3Var();

   bool operator==(const Rvec3Var &right) const;
   bool operator!=(const Rvec3Var &right) const;

   virtual std::string  ToString();

   virtual const        Rvector3& GetRvector3() const;
   virtual void         SetRvector3(const Rvector3 &val);
   virtual const        Rvector3& EvaluateRvector3();

protected:

   Rvector3 mRvec3Value;

private:

};
#endif // Parameter_hpp
