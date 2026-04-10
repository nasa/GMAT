//$Id$
//------------------------------------------------------------------------------
//                                  NumberWrapper
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2007.01.18
//
/**
 * Definition of the NumberWrapper class.
 *
 *
 */
//------------------------------------------------------------------------------

#ifndef NumberWrapper_hpp
#define NumberWrapper_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElementWrapper.hpp"

class GMAT_API NumberWrapper : public ElementWrapper
{
public:

   // default constructor
   NumberWrapper();
   // copy constructor
   NumberWrapper(const NumberWrapper &nr);
   // operator = 
   const NumberWrapper& operator=(const NumberWrapper &nr);
   // destructor
   virtual ~NumberWrapper();
   
   virtual ElementWrapper*     Clone() const;
   virtual std::string         ToString();
   virtual Gmat::ParameterType GetDataType() const;
   
   virtual Real            EvaluateReal() const;  
   virtual bool            SetReal(const Real toValue);
   
   
protected:  

   // the Real value
   Real value;
   
   virtual void            SetupWrapper();
};
#endif // NumberWrapper_hpp
