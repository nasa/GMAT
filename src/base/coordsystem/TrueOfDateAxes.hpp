//$Id$
//------------------------------------------------------------------------------
//                                  TrueOfDateAxes
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/05/03
//
/**
 * Definition of the TrueOfDateAxes class.  This is the base class for those
 * DynamicAxes classes that implement True Of Date axis systems.
 *
 */
//------------------------------------------------------------------------------

#ifndef TrueOfDateAxes_hpp
#define TrueOfDateAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "DynamicAxes.hpp"
#include "A1Mjd.hpp"

class GMAT_API TrueOfDateAxes : public DynamicAxes
{
public:

   // default constructor
   TrueOfDateAxes(const std::string &itsType,
                  const std::string &itsName = "");
   // copy constructor
   TrueOfDateAxes(const TrueOfDateAxes &tod);
   // operator = for assignment
   const TrueOfDateAxes& operator=(const TrueOfDateAxes &tod);
   // destructor
   virtual ~TrueOfDateAxes();
   
   // initializes the TrueOfDateAxes
   virtual bool Initialize(); 

   virtual GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "FK5") const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;

protected:

   enum
   {
      TrueOfDateAxesParamCount = DynamicAxesParamCount
   };
    

};
#endif // TrueOfDateAxes_hpp
