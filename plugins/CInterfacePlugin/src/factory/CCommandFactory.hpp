//------------------------------------------------------------------------------
//                         CCommandFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: 2011/06/14
//
/**
 *  Definition code for the CCommandFactory class, responsible for
 *  creating Command objects associated with the C Interface.
 */
//------------------------------------------------------------------------------
#ifndef CCommandFactory_hpp
#define CCommandFactory_hpp


#include "GmatCFunc_defs.hpp"
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "GmatCommand.hpp"

class CINTERFACE_API CCommandFactory : public Factory
{
public:
   GmatCommand*  CreateCommand(const std::string &ofType,
                               const std::string &withName = "");

   // default constructor
   CCommandFactory();
   // constructor
   CCommandFactory(StringArray createList);
   // copy constructor
   CCommandFactory(const CCommandFactory& fact);
   // assignment operator
   CCommandFactory& operator= (const CCommandFactory& fact);

   // destructor
   ~CCommandFactory();

      // method to return list of types of objects that this factory can create
   virtual StringArray      GetListOfCreatableObjects(
                                  const std::string &qualifier = "");

protected:
   // protected data

private:
   // private data

};

#endif // CCommandFactory_hpp
