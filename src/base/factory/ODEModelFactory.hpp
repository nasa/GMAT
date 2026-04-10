//$Id$
//------------------------------------------------------------------------------
//                         ODEModelFactory
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
// Author: Wendy Shoan
// Created: 2003/10/14
//
/**
 *  This class is the factory class for ODEModels.  
 */
//------------------------------------------------------------------------------
#ifndef ODEModelFactory_hpp
#define ODEModelFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "ODEModel.hpp"

class GMAT_API ODEModelFactory : public Factory
{
public:
   GmatBase*  CreateObject(const std::string &ofType,
                           const std::string &withName = "");
   ODEModel*  CreateODEModel(const std::string &ofType,
                             const std::string &withName = "");
   
   // default constructor
   ODEModelFactory();
   // constructor
   ODEModelFactory(StringArray createList);
   // copy constructor
   ODEModelFactory(const ODEModelFactory& fact);
   // assignment operator
   ODEModelFactory& operator= (const ODEModelFactory& fact);

   // destructor
   ~ODEModelFactory();

protected:
   // protected data

private:
   // private data


};

#endif // ODEModelFactory_hpp




