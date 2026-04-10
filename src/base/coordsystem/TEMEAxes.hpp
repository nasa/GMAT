//$Id$
//------------------------------------------------------------------------------
//                                  TEMEAxes
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
//
// Author: Jairus Elarbee
// Created: 2021/10/04
//
/**
 * Definition of the TEMEAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef TEMEAxes_hpp
#define TEMEAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "TrueOfDateAxes.hpp"

class GMAT_API TEMEAxes : public TrueOfDateAxes
{
public:

   // default constructor
   TEMEAxes(const std::string &itsName = "");
   // copy constructor
   TEMEAxes(const TEMEAxes &tod);
   // operator = for assignment
   const TEMEAxes& operator=(const TEMEAxes &tod);
   // destructor
   virtual ~TEMEAxes();
   
   // method to initialize the data
   virtual bool Initialize();

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;
   
protected:

   enum
   {
      TEMEAxesParamCount = TrueOfDateAxesParamCount,
   };

   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);
   virtual Real ComputeModEq(Real a1Epoch,
      Real tUT1,
      Real dPsi,
      Real longAscNodeLunar,
      Real cosEpsbar);

   virtual void InitializeReference(SpacePoint *refObj);

};
#endif // TEMEAxes_hpp
