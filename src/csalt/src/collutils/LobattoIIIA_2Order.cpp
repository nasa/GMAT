//------------------------------------------------------------------------------
//                              ImplicitRK
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
#include "LobattoIIIA_2Order.hpp"

#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_LOBATTO_2ORDER


//------------------------------------------------------------------------------
// LobattoIIIA_2Order()
//------------------------------------------------------------------------------
/**
 * default constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_2Order::LobattoIIIA_2Order() :
   LobattoIIIASeparated()
{
   // Initialize of LobattoIIIA_2Order
   LoadButcherTable();
   InitializeData();
   ComputeDependencies();
}

//------------------------------------------------------------------------------
// LobattoIIIA_2Order(const LobattoIIIA_2Order &copy)
//------------------------------------------------------------------------------
/**
 * copy constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_2Order::LobattoIIIA_2Order(const LobattoIIIA_2Order &copy) :
   LobattoIIIASeparated(copy)
{
   /// copying matrices is done in Implicit RK level.
};

//------------------------------------------------------------------------------
// operator=(const LobattoIIIA_2Order &copy)
//------------------------------------------------------------------------------
/**
 * operator=
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_2Order& LobattoIIIA_2Order::operator=(const LobattoIIIA_2Order &copy)
{
   if (&copy == this)
      return *this;

   /// copying matrices is done in Implicit RK level.
   LobattoIIIASeparated::operator=(copy);
   
   return *this;
};

//------------------------------------------------------------------------------
// ~LobattoIIIA_8Order()
//------------------------------------------------------------------------------
/**
 * destructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_2Order::~LobattoIIIA_2Order()
{
   // do nothing
}

//------------------------------------------------------------------------------
// void InitializeData()
//------------------------------------------------------------------------------
/**
 * This method initializes the data
 *
 */
//------------------------------------------------------------------------------
void LobattoIIIA_2Order::InitializeData()
{
   numDefectCons = 1;
   numPointsPerStep = 2;
   // Set up non - dimensional stage times
   stageTimes.SetSize(2);
   stageTimes(0) = rhoVec(0);
   stageTimes(1) = rhoVec(1);
   // Set up the optimization parameter dependency array
   paramDepArray.SetSize(1, 2);
   paramDepArray(0, 0) = -1;
   paramDepArray(0, 1) = 1;

   // Set up the function dependency array
   funcConstArray.SetSize(1, 2);
   funcConstArray(0, 0) = -sigmaMatrix(1, 0);
   funcConstArray(0, 1) = -sigmaMatrix(1, 1);

   numStagePointsPerMesh = 0;
   numStateStagePointsPerMesh = 0;
   numControlStagePointsPerMesh = 0;
};

//------------------------------------------------------------------------------
// void LoadButcherTable()
//------------------------------------------------------------------------------
/**
 * This method loads data into the butcher table
 *
 */
//------------------------------------------------------------------------------
void LobattoIIIA_2Order::LoadButcherTable()
{
   // Populates the butcher array member data.Taken from table
   // labelled with(S = 3) in Betts.

   // The non - dimensionalized stage times
   rhoVec.SetSize(2);
   rhoVec(0) = 0.0;
   rhoVec(1) = 1.0;

   // The alpha vector
   betaVec.SetSize(2);
   betaVec(0) = 1.0 / 2.0;
   betaVec(1) = 1.0 / 2.0;

   // The beta matrix
   sigmaMatrix.SetSize(2, 2);

   // Row 2
   sigmaMatrix(1, 0) = 1.0 / 2.0;
   sigmaMatrix(1, 1) = 1.0 / 2.0;

   #ifdef DEBUG_LOBATTO_2ORDER
      MessageInterface::ShowMessage("sigmaMatrix is given as follows:\n%s\n",
                                    sigmaMatrix.ToString(12).c_str());
   #endif
};

//------------------------------------------------------------------------------
//  ImplicitRungeKutta* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Asteroid.
 *
 * @return clone of the ImplicitRungeKutta.
 */
//------------------------------------------------------------------------------
ImplicitRungeKutta* LobattoIIIA_2Order::Clone() const
{
   return (new LobattoIIIA_2Order(*this));
}

