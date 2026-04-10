//------------------------------------------------------------------------------
//                              ImplicitRKPhase
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
// Author: Wendy Shoan / NASA
// Created: 2016.05.18
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef ImplicitRKPhase_hpp
#define ImplicitRKPhase_hpp

#include "Phase.hpp"
#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "NLPFuncUtil_ImplicitRK.hpp"

class CSALT_API ImplicitRKPhase : public Phase
{
public:
   
   ImplicitRKPhase();
   ImplicitRKPhase(const ImplicitRKPhase &copy);
   ImplicitRKPhase& operator=(const ImplicitRKPhase &copy);
   ~ImplicitRKPhase();

   /// Initializes the ImplicitRKPhase
   void InitializeTranscription();

   void SetTranscription(std::string type);
  
protected:
   /// the collocation method
   std::string collocationMethod;
};
#endif // ImplicitRKPhase_hpp
