//$Id$
//------------------------------------------------------------------------------
//                            NewParameterFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/29
//
/**
 *  Declaration code for the NewParameterFactory class, responsible for creating
 *  the HSL VF13ad optimizer.
 */
//------------------------------------------------------------------------------


#ifndef NewParameterFactory_hpp
#define NewParameterFactory_hpp

#include "newparameter_defs.hpp"
#include "Factory.hpp"


class NEW_PARAMETER_API NewParameterFactory : public Factory
{
public:
   virtual GmatBase*  CreateObject(const std::string &ofType,
                                   const std::string &withName = "");
   virtual Parameter*  CreateParameter(const std::string &ofType,
                                       const std::string &withName = "");
   
   // default constructor
   NewParameterFactory();
   // constructor
   NewParameterFactory(StringArray createList);
   // copy constructor
   NewParameterFactory(const NewParameterFactory& fact);
   // assignment operator
   NewParameterFactory& operator=(const NewParameterFactory& fact);
   
   virtual ~NewParameterFactory();
   
   virtual bool DoesObjectTypeMatchSubtype(const std::string &theType,
                                           const std::string &theSubtype);

   virtual StringArray      GetListOfCreatableObjects(
                                  const std::string &qualifier = "");

private:
   /// Flag used to show that ParameterInfo has been registered
   bool registrationComplete;
};

#endif // NewParameterFactory_hpp
