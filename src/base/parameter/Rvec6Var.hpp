//$Id$
//------------------------------------------------------------------------------
//                                  Rvec6Var
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
 * Declares base class of parameters returning Rvector6.
 */
//------------------------------------------------------------------------------
#ifndef Rvec6Var_hpp
#define Rvec6Var_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include "Rvector6.hpp"

class GMAT_API Rvec6Var : public Parameter
{
public:

   Rvec6Var(const std::string &name = "",
            const std::string &typeStr = "Rvec6Var",
            GmatParam::ParameterKey key = GmatParam::USER_PARAM,
            GmatBase *obj = NULL, const std::string &desc = "",
            const std::string &unit = "",
            GmatParam::DepObject depObj = GmatParam::NO_DEP,
            UnsignedInt ownerType = Gmat::UNKNOWN_OBJECT);
   Rvec6Var(const Rvec6Var &copy);
   Rvec6Var& operator= (const Rvec6Var& right);
   virtual ~Rvec6Var();
   
   bool operator==(const Rvec6Var &right) const;
   bool operator!=(const Rvec6Var &right) const;
   
   virtual std::string  ToString();
   
   virtual const        Rvector6& GetRvector6() const;
   virtual void         SetRvector6(const Rvector6 &val);
   virtual const        Rvector6& EvaluateRvector6();
   
protected:
   
   Rvector6 mRvec6Value;
   
private:

};
#endif // Parameter_hpp
