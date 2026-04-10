//$Id$
//------------------------------------------------------------------------------
//                         CalculatedPointFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy Shoan
// Created: 2005/04/06
//
/**
*  This class is the factory class for CalculatedPoints.
 */
//------------------------------------------------------------------------------
#ifndef CalculatedPointFactory_hpp
#define CalculatedPointFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "CalculatedPoint.hpp"

class GMAT_API CalculatedPointFactory : public Factory
{
public:
   GmatBase*         CreateObject(const std::string &ofType,
                                  const std::string &withName = "");
   CalculatedPoint*  CreateCalculatedPoint(const std::string &ofType,
                                           const std::string &withName = "");

   // default constructor
   CalculatedPointFactory();
   // constructor
   CalculatedPointFactory(const StringArray &createList);
   // copy constructor
   CalculatedPointFactory(const CalculatedPointFactory &fact);
   // assignment operator
   CalculatedPointFactory& operator= (const CalculatedPointFactory &fact);

   // destructor
   ~CalculatedPointFactory();

protected:
      // protected data

private:
      // private data


};

#endif // CalculatedPointFactory_hpp




