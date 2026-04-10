// ------------------------------------------------------------------------------
//                              LobattoIIIASeparated
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
// Author: Youngkwang Kim
// Created: 2017.01.24
//
/**
* From original MATLAB prototype:
*  Author: S. Hughes.  steven.p.hughes@nasa.gov
*
* Explanation goes here.
*
*/
//------------------------------------------------------------------------------

#ifndef LobattoIIIA_Separated_hpp
#define LobattoIIIA_Separated_hpp

#include "ImplicitRungeKutta.hpp"

class LobattoIIIASeparated : public ImplicitRungeKutta
{
public:
   /// default constructor
   LobattoIIIASeparated();
   /// copy constructor
   LobattoIIIASeparated(const LobattoIIIASeparated &copy);
   /// operator =
   LobattoIIIASeparated& operator=(const LobattoIIIASeparated &copy);

   /// destructor
   virtual ~LobattoIIIASeparated();
   
   bool         GetIsSeparated();


protected:
   /// flag indicating whether or not the ??? is separated
   bool isSeparated;
   
   Rvector GetFuncVecFromArray(Rmatrix funcArray, Integer numStates, Real deltaT);
};

#endif //LobattoIIIA_Separated_hpp
