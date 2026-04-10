//$Id$
//------------------------------------------------------------------------------
//                                  ArrayWrapper
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
// Created: 2007.04.04
//
/**
 * Definition of the ArrayWrapper class.
 *
 *
 */
//------------------------------------------------------------------------------

#ifndef ArrayWrapper_hpp
#define ArrayWrapper_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ElementWrapper.hpp"
#include "Array.hpp"

class GMAT_API ArrayWrapper : public ElementWrapper
{
public:

   // default constructor
   ArrayWrapper();
   // copy constructor
   ArrayWrapper(const ArrayWrapper &aw);
   // operator = 
   const ArrayWrapper& operator=(const ArrayWrapper &aw);
   // destructor
   virtual ~ArrayWrapper();
   
   virtual ElementWrapper*     Clone() const;

   virtual std::string         ToString();
   virtual Gmat::ParameterType GetDataType() const;
   
   virtual const StringArray&  GetRefObjectNames();
   virtual GmatBase*           GetRefObject(const std::string &name = "");
   virtual bool                SetRefObject(GmatBase *obj);
   virtual Real                EvaluateReal() const;
   virtual bool                SetReal(const Real toValue);
   // need to override this method, to handle the arrayName, rowName,
   // and columnName data members
   virtual bool                RenameObject(const std::string &oldName, 
                                           const std::string &newName);
   
   virtual const Rmatrix&      EvaluateArray() const;
   virtual bool                SetArray(const Rmatrix &toValue); 

protected:  
   
   /// pointer to the Array object
   Array          *array;   
   /// name of the array
   std::string     arrayName;
   
   virtual void         SetupWrapper(); 
};
#endif // ArrayWrapper_hpp
