//$Id$
//------------------------------------------------------------------------------
//                         EKFCommandFactory
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
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2019/07/25
//
/**
 *  This class is the factory class for EKF and smoother commands.
 */
//------------------------------------------------------------------------------
#ifndef EKFCommandFactory_hpp
#define EKFCommandFactory_hpp


#include "kalman_defs.hpp"
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "GmatCommand.hpp"

class KALMAN_API EKFCommandFactory : public Factory
{
public:
   GmatCommand*  CreateCommand(const std::string &ofType,
                               const std::string &withName = "");

   // default constructor
   EKFCommandFactory();
   // constructor
   EKFCommandFactory(StringArray createList);
   // copy constructor
   EKFCommandFactory(const EKFCommandFactory& fact);
   // assignment operator
   EKFCommandFactory& operator= (const EKFCommandFactory& fact);

   // destructor
   ~EKFCommandFactory();

protected:
   // protected data

private:
   // private data

};

#endif // EKFCommandFactory_hpp
