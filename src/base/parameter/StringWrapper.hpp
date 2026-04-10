//$Id$
//------------------------------------------------------------------------------
//                                  StringWrapper
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun/GSFC
// Created: 2007/07/24
//
/**
 * Declares StringWrapper class.
 */
//------------------------------------------------------------------------------
#ifndef StringWrapper_hpp
#define StringWrapper_hpp

#include "gmatdefs.hpp"
#include "ElementWrapper.hpp"

class GMAT_API StringWrapper : public ElementWrapper
{
public:
   
   StringWrapper();
   StringWrapper(const StringWrapper &copy);
   const StringWrapper& operator=(const StringWrapper &right);
   virtual ~StringWrapper();
   
   virtual ElementWrapper*     Clone() const;
   virtual std::string         ToString();
   virtual Gmat::ParameterType GetDataType() const;
   
   virtual Real         EvaluateReal() const;
   virtual bool         SetReal(const Real val);
   
   virtual std::string  EvaluateString() const;
   virtual bool         SetString(const std::string &val);
   
protected:  
   
   // The string value
   std::string value;
   
   virtual void         SetupWrapper();
};
#endif // StringWrapper_hpp
