//$Id$
//------------------------------------------------------------------------------
//                            AssetFactory
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
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: 2008/08/12
//
/**
 *  This class is the factory class for assets.  
 */
//------------------------------------------------------------------------------

#ifndef AssetFactory_hpp
#define AssetFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"


class GMAT_API AssetFactory : public Factory
{
public:
   AssetFactory();
   virtual ~AssetFactory();
   AssetFactory(StringArray createList);
   AssetFactory(const AssetFactory& af);
   AssetFactory& operator=(const AssetFactory& af);
   
   virtual SpacePoint*      CreateObject(const std::string &ofType,
                                         const std::string &withName = "");        
   virtual SpacePoint*      CreateSpacePoint(const std::string &ofType,
                                             const std::string &withName = "");        

};

#endif /*AssetFactory_hpp*/
