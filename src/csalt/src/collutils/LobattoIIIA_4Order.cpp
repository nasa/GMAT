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
#include "LobattoIIIA_4Order.hpp"

#include "LowThrustException.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_LOBATTO_4ORDER

//------------------------------------------------------------------------------
// LobattoIIIA_4Order()
//------------------------------------------------------------------------------
/**
 * default constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_4Order::LobattoIIIA_4Order() :
   LobattoIIIASeparated()
{
   // Initialize of LobattoIIIA_4Order
   LoadButcherTable();
   InitializeData();
   ComputeDependencies();
}

//------------------------------------------------------------------------------
// LobattoIIIA_4Order(const LobattoIIIA_4Order &copy)
//------------------------------------------------------------------------------
/**
 * copy constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_4Order::LobattoIIIA_4Order(const LobattoIIIA_4Order &copy) :
   LobattoIIIASeparated(copy)
{
   /// copying matrices is done in Implicit RK level.
};

//------------------------------------------------------------------------------
// operator=(const LobattoIIIA_4Order &copy)
//------------------------------------------------------------------------------
/**
 * operator=
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_4Order& LobattoIIIA_4Order::operator=(
                                        const LobattoIIIA_4Order &copy)
{
   if (&copy == this)
      return *this;

   /// copying matrices is done in Implicit RK level.
   LobattoIIIASeparated::operator=(copy);
   
   return *this;
};

//------------------------------------------------------------------------------
// ~LobattoIIIA_4Order()
//------------------------------------------------------------------------------
/**
 * destructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_4Order::~LobattoIIIA_4Order()
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
void LobattoIIIA_4Order::InitializeData()
{
   numDefectCons = 2;
   numPointsPerStep = 3;
   // Set up non - dimensional stage times
   stageTimes.SetSize(3);
   stageTimes(0) = rhoVec(0);
   stageTimes(1) = rhoVec(1);
   stageTimes(2) = rhoVec(2);
   // Set up the optimization parameter dependency array
   paramDepArray.SetSize(2, 3);
   paramDepArray(0, 0) = -1;
   paramDepArray(1, 0) = -1;

   paramDepArray(0, 1) = 1;
   paramDepArray(1, 2) = 1;

   // Set up the function dependency array
   funcConstArray.SetSize(2, 3);
   funcConstArray(0, 0) = -sigmaMatrix(1, 0);
   funcConstArray(0, 1) = -sigmaMatrix(1, 1);
   funcConstArray(0, 2) = -sigmaMatrix(1, 2);

   funcConstArray(1, 0) = -betaVec(0);
   funcConstArray(1, 1) = -betaVec(1);
   funcConstArray(1, 2) = -betaVec(2);

   numStagePointsPerMesh = 1;
   numStateStagePointsPerMesh = 1;
   numControlStagePointsPerMesh = 1;
};

//------------------------------------------------------------------------------
// void LoadButcherTable()
//------------------------------------------------------------------------------
/**
 * This method loads data into the butcher table
 *
 */
//------------------------------------------------------------------------------
void LobattoIIIA_4Order::LoadButcherTable()
{
   // Populates the butcher array member data.Taken from table
   // labelled with(S = 3) in Betts.

   // The non - dimensionalized stage times
   rhoVec.SetSize(3);
   rhoVec(0) = 0.0;
   rhoVec(1) = 0.5;
   rhoVec(2) = 1.0;

   // The alpha vector
   betaVec.SetSize(3);
   betaVec(0) = 1.0 / 6.0;
   betaVec(1) = 4.0 / 6.0;
   betaVec(2) = 1.0 / 6.0;

   // The beta matrix
   sigmaMatrix.SetSize(3, 3);

   // Row 2
   sigmaMatrix(1, 0) = 5.0 / 24.0;
   sigmaMatrix(1, 1) = 1.0 / 3.0;
   sigmaMatrix(1, 2) = -1.0 / 24.0;
   // Row 3
   sigmaMatrix(2, 0) = 1.0 / 6.0;
   sigmaMatrix(2, 1) = 4.0 / 6.0;
   sigmaMatrix(2, 2) = 1.0 / 6.0;

   #ifdef DEBUG_LOBATTO_4ORDER
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
ImplicitRungeKutta* LobattoIIIA_4Order::Clone() const
{
   return (new LobattoIIIA_4Order(*this));
}

