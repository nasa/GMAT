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
#include "LobattoIIIA_6Order.hpp"

#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_LOBATTO_6ORDER

//------------------------------------------------------------------------------
// LobattoIIIA_6Order()
//------------------------------------------------------------------------------
/**
 * default constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_6Order::LobattoIIIA_6Order() :
   LobattoIIIASeparated()
{
   // Initialize of LobattoIIIA_6Order
   LoadButcherTable();
   InitializeData();
   ComputeDependencies();
}

//------------------------------------------------------------------------------
// LobattoIIIA_6Order()
//------------------------------------------------------------------------------
/**
 * copy constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_6Order::LobattoIIIA_6Order(const LobattoIIIA_6Order &copy) :
   LobattoIIIASeparated(copy)
{
   /// copying matrices is done in Implicit RK level.
};

//------------------------------------------------------------------------------
// operator=(const LobattoIIIA_6Order &copy)
//------------------------------------------------------------------------------
/**
 * operator=
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_6Order& LobattoIIIA_6Order::operator=(const LobattoIIIA_6Order &copy)
{
   if (&copy == this)
      return *this;

   /// copying matrices is done in Implicit RK level.
      /// copying matrices is done in Implicit RK level.
   LobattoIIIASeparated::operator=(copy);
   
   return *this;
};

//------------------------------------------------------------------------------
// LobattoIIIA_6Order()
//------------------------------------------------------------------------------
/**
 * destructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_6Order::~LobattoIIIA_6Order()
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
void LobattoIIIA_6Order::InitializeData()
{
   numDefectCons = 3;
   numPointsPerStep = 4;
   // Set up non - dimensional stage times
   stageTimes.SetSize(4);
   stageTimes(0) = rhoVec(0);
   stageTimes(1) = rhoVec(1);
   stageTimes(2) = rhoVec(2);
   stageTimes(3) = rhoVec(3);
   // Set up the optimization parameter dependency array
   paramDepArray.SetSize(3,4);
   paramDepArray(0, 0) = -1;
   paramDepArray(1, 0) = -1;
   paramDepArray(2, 0) = -1;

   paramDepArray(0, 1) = 1;
   paramDepArray(1, 2) = 1;
   paramDepArray(2, 3) = 1;

   // Set up the function dependency array
   funcConstArray.SetSize(3, 4);
   funcConstArray(0, 0) = -sigmaMatrix(1, 0);
   funcConstArray(0, 1) = -sigmaMatrix(1, 1);
   funcConstArray(0, 2) = -sigmaMatrix(1, 2);
   funcConstArray(0, 3) = -sigmaMatrix(1, 3);

   funcConstArray(1, 0) = -sigmaMatrix(2, 0);
   funcConstArray(1, 1) = -sigmaMatrix(2, 1);
   funcConstArray(1, 2) = -sigmaMatrix(2, 2);
   funcConstArray(1, 3) = -sigmaMatrix(2, 3);
   
   funcConstArray(2, 0) = -betaVec(0);
   funcConstArray(2, 1) = -betaVec(1);
   funcConstArray(2, 2) = -betaVec(2);
   funcConstArray(2, 3) = -betaVec(3);
   
   numStagePointsPerMesh = 2;
   numStateStagePointsPerMesh = 2;
   numControlStagePointsPerMesh = 2;
};

//------------------------------------------------------------------------------
// void LoadButcherTable()
//------------------------------------------------------------------------------
/**
 * This method loads data into the butcher table
 *
 */
//------------------------------------------------------------------------------
void LobattoIIIA_6Order::LoadButcherTable()
{
   // Populates the butcher array member data.Taken from table
   // labelled with(S = 5) in Betts.

   Real sqrt5 = sqrt(5.0);
   // The non - dimensionalized stage times
   rhoVec.SetSize(4);
   rhoVec(0) = 0.0;
   rhoVec(1) = 0.5 - sqrt5 / 10.0;
   rhoVec(2) = 0.5 + sqrt5 / 10.0;
   rhoVec(3) = 1.0;

   // The alpha vector
   betaVec.SetSize(4);
   betaVec(0) = 1.0 / 12.0;
   betaVec(1) = 5.0 / 12.0;
   betaVec(2) = 5.0 / 12.0;
   betaVec(3) = 1.0 / 12.0;

   // The beta matrix
   sigmaMatrix.SetSize(4, 4);

   // Row 2
   sigmaMatrix(1, 0) = (11.0 + 1.0 * sqrt5) / 120.0;
   sigmaMatrix(1, 1) = (25.0 - 1.0 * sqrt5) / 120.0;
   sigmaMatrix(1, 2) = (25.0 - 13.0 * sqrt5) / 120.0;
   sigmaMatrix(1, 3) = (-1.0 + 1.0 * sqrt5) / 120.0;
   // Row 3
   sigmaMatrix(2, 0) = (11.0 - 1.0 * sqrt5) / 120.0;
   sigmaMatrix(2, 1) = (25.0 + 13.0 * sqrt5) / 120.0;
   sigmaMatrix(2, 2) = (25.0 + 1.0 * sqrt5) / 120.0;
   sigmaMatrix(2, 3) = (-1.0 - 1.0 * sqrt5) / 120.0;
   // Row 4
   sigmaMatrix(3, 0) = 1.0 / 12.0;
   sigmaMatrix(3, 1) = 5.0 / 12.0;
   sigmaMatrix(3, 2) = 5.0 / 12.0;
   sigmaMatrix(3, 3) = 1.0 / 12.0;

   #ifdef DEBUG_LOBATTO_6ORDER
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
ImplicitRungeKutta* LobattoIIIA_6Order::Clone() const
{
   return (new LobattoIIIA_6Order(*this));
}

