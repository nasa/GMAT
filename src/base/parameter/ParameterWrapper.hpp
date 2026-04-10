//$Id$
//------------------------------------------------------------------------------
//                                  ParameterWrapper
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
 * Definition of the ParameterWrapper class.
 *
 *
 */
//------------------------------------------------------------------------------

#ifndef ParameterWrapper_hpp
#define ParameterWrapper_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElementWrapper.hpp"
#include "Parameter.hpp"

class GMAT_API ParameterWrapper : public ElementWrapper
{
public:

   // default constructor
   ParameterWrapper();
   // copy constructor
   ParameterWrapper(const ParameterWrapper &pw);
   // operator = 
   const ParameterWrapper& operator=(const ParameterWrapper &pw);
   // destructor
   virtual ~ParameterWrapper();
   
   virtual ElementWrapper*     Clone() const;
   virtual std::string         ToString();
   virtual Gmat::ParameterType GetDataType() const;
   
   virtual GmatBase*           GetRefObject(const std::string &name = "");
   virtual bool                SetRefObject(GmatBase *obj);
   virtual bool                RenameObject(const std::string &oldName, 
                                            const std::string &newName);
   
   virtual Real                EvaluateReal() const;
   virtual bool                SetReal(const Real toValue);
   
   virtual std::string         EvaluateString() const;
   virtual bool                SetString(const std::string &toValue);
   
   virtual const Rmatrix&      EvaluateArray() const;
   virtual bool                SetArray(const Rmatrix &toValue);
   
   virtual const Rvector&      EvaluateRvector() const;
   virtual bool                SetRvector(const Rvector &toValue);
   
   virtual GmatBase*           EvaluateObject() const;
   virtual bool                SetObject(const GmatBase* obj);
   
protected:  
   
   // pointer to the Parameter object
   Parameter *param;
   
   virtual void                SetupWrapper(); 
};
#endif // ParameterWrapper_hpp
