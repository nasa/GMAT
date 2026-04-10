//$Id$
//------------------------------------------------------------------------------
//                         FieldOfViewFactory
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
// Author: Syeda Kazmi (Based on FieldOfViewFactory)
// Created: 5/20/2019
//
/**
 *  This class is the factory class for fov.
 */
//------------------------------------------------------------------------------
#ifndef FieldOfViewFactory_hpp
#define FieldOfViewFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "FieldOfView.hpp"

class GMAT_API FieldOfViewFactory : public Factory
{
public:
   // method to create Hardware object
   virtual GmatBase*    CreateObject(const std::string &ofType,
                                     const std::string &withName = "");
   virtual FieldOfView* CreateFieldOfView(const std::string &ofType,
                                       const std::string &withName = "");

   // default constructor
   FieldOfViewFactory();
   // constructor
   FieldOfViewFactory(StringArray createList);
   // copy constructor
   FieldOfViewFactory(const FieldOfViewFactory& fact);
   // assignment operator
   FieldOfViewFactory& operator= (const FieldOfViewFactory& fact);

   // destructor
   ~FieldOfViewFactory();

protected:
   // protected data

private:
   // private data
};

#endif // FieldOfViewFactory_hpp
