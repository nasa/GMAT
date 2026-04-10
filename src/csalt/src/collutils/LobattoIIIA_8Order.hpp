//------------------------------------------------------------------------------
//                              LobattoIIIA_8Order
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

#ifndef LobattoIIIA_8Order_hpp
#define LobattoIIIA_8Order_hpp

#include "LobattoIIIASeparated.hpp"

class LobattoIIIA_8Order : public LobattoIIIASeparated
{
public:
   /// default constructor
   LobattoIIIA_8Order();
   /// copy constructor
   LobattoIIIA_8Order(const LobattoIIIA_8Order &copy);
   /// operator=
   LobattoIIIA_8Order& operator=(const LobattoIIIA_8Order &copy);
   /// destructor
   ~LobattoIIIA_8Order();

   /// abstract methods inherited from ImplicitRungeKutta
   void InitializeData();

   void LoadButcherTable();

   virtual ImplicitRungeKutta* Clone() const;
};

#endif //LobattoIIIA_8Order_hpp
