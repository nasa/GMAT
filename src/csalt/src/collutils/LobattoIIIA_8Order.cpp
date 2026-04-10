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
#include "LobattoIIIA_8Order.hpp"

#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_LOBATTO_8ORDER

//------------------------------------------------------------------------------
// LobattoIIIA_8Order()
//------------------------------------------------------------------------------
/**
 * default constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_8Order::LobattoIIIA_8Order() :
   LobattoIIIASeparated()
{
   // Initialize of LobattoIIIA_8Order
   LoadButcherTable();
   InitializeData();
   ComputeDependencies();
}

//------------------------------------------------------------------------------
// LobattoIIIA_8Order(const LobattoIIIA_8Order &copy)
//------------------------------------------------------------------------------
/**
 * copy constructor
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_8Order::LobattoIIIA_8Order(const LobattoIIIA_8Order &copy) :
   LobattoIIIASeparated(copy)
{
   /// copying matrices is done in Implicit RK level.
};

//------------------------------------------------------------------------------
// operator=(const LobattoIIIA_8Order &copy)
//------------------------------------------------------------------------------
/**
 * operator=
 *
 */
//------------------------------------------------------------------------------
LobattoIIIA_8Order& LobattoIIIA_8Order::operator=(
                                        const LobattoIIIA_8Order &copy)
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
LobattoIIIA_8Order::~LobattoIIIA_8Order()
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
void LobattoIIIA_8Order::InitializeData()
{
   numDefectCons = 4;
   numPointsPerStep = 5;
   // Set up non - dimensional stage times
   stageTimes.SetSize(5);
   stageTimes(0) = rhoVec(0);
   stageTimes(1) = rhoVec(1);
   stageTimes(2) = rhoVec(2);
   stageTimes(3) = rhoVec(3);
   stageTimes(4) = rhoVec(4);
   // Set up the optimization parameter dependency array
   paramDepArray.SetSize(4, 5);
   paramDepArray(0, 0) = -1;
   paramDepArray(1, 0) = -1;
   paramDepArray(2, 0) = -1;
   paramDepArray(3, 0) = -1;

   paramDepArray(0, 1) = 1;
   paramDepArray(1, 2) = 1;
   paramDepArray(2, 3) = 1;
   paramDepArray(3, 4) = 1;

   // Set up the function dependency array
   funcConstArray.SetSize(4, 5);
   funcConstArray(0, 0) = -sigmaMatrix(1, 0);
   funcConstArray(0, 1) = -sigmaMatrix(1, 1);
   funcConstArray(0, 2) = -sigmaMatrix(1, 2);
   funcConstArray(0, 3) = -sigmaMatrix(1, 3);
   funcConstArray(0, 4) = -sigmaMatrix(1, 4);

   funcConstArray(1, 0) = -sigmaMatrix(2, 0);
   funcConstArray(1, 1) = -sigmaMatrix(2, 1);
   funcConstArray(1, 2) = -sigmaMatrix(2, 2);
   funcConstArray(1, 3) = -sigmaMatrix(2, 3);
   funcConstArray(1, 4) = -sigmaMatrix(2, 4);

   funcConstArray(2, 0) = -sigmaMatrix(3, 0);
   funcConstArray(2, 1) = -sigmaMatrix(3, 1);
   funcConstArray(2, 2) = -sigmaMatrix(3, 2);
   funcConstArray(2, 3) = -sigmaMatrix(3, 3);
   funcConstArray(2, 4) = -sigmaMatrix(3, 4);

   funcConstArray(3, 0) = -betaVec(0);
   funcConstArray(3, 1) = -betaVec(1);
   funcConstArray(3, 2) = -betaVec(2);
   funcConstArray(3, 3) = -betaVec(3);
   funcConstArray(3, 4) = -betaVec(4);

   numStagePointsPerMesh = 3;
   numStateStagePointsPerMesh = 3;
   numControlStagePointsPerMesh = 3;
};

//------------------------------------------------------------------------------
// void LoadButcherTable()
//------------------------------------------------------------------------------
/**
 * This method loads data into the butcher table
 *
 */
//------------------------------------------------------------------------------
void LobattoIIIA_8Order::LoadButcherTable()
{
   // Populates the butcher array member data.Taken from table
   // labelled with(S = 5) in Betts.

   Real sqrt21 = sqrt(21);
   // The non - dimensionalized stage times
   rhoVec.SetSize(5);
   rhoVec(0) = 0.0;
   rhoVec(1) = 0.5 - sqrt21 / 14.0;
   rhoVec(2) = 0.5;
   rhoVec(3) = 0.5 + sqrt21 / 14.0;
   rhoVec(4) = 1.0;

   // The alpha vector
   betaVec.SetSize(5);
   betaVec(0) = 1.0 / 20.0;
   betaVec(1) = 49.0 / 180.0;
   betaVec(2) = 16.0 / 45.0;
   betaVec(3) = 49.0 / 180.0;
   betaVec(4) = 1.0 / 20.0;

   // The beta matrix
   sigmaMatrix.SetSize(5, 5);

   // Row 2
   sigmaMatrix(1, 0) = (119.0 + 3.0 * sqrt21) / 1960.0;
   sigmaMatrix(1, 1) = (343.0 - 9.0 * sqrt21) / 2520.0;
   sigmaMatrix(1, 2) = (392.0 - 96.0 * sqrt21) / 2205.0;
   sigmaMatrix(1, 3) = (343.0 - 69.0 * sqrt21) / 2520.0;
   sigmaMatrix(1, 4) = (-21.0 + 3.0 * sqrt21) / 1960.0;
   // Row 3
   sigmaMatrix(2, 0) = 13.0 / 320.0;
   sigmaMatrix(2, 1) = (392.0 + 105.0 * sqrt21) / 2880.0;
   sigmaMatrix(2, 2) = 8.0 / 45.0;
   sigmaMatrix(2, 3) = (392 - 105 * sqrt21) / 2880.0;
   sigmaMatrix(2, 4) = 3.0 / 320.0;
   // Row 4
   sigmaMatrix(3, 0) = (119.0 - 3.0 * sqrt21) / 1960.0;
   sigmaMatrix(3, 1) = (343.0 + 69.0 * sqrt21) / 2520.0;
   sigmaMatrix(3, 2) = (392.0 + 96.0 * sqrt21) / 2205.0;
   sigmaMatrix(3, 3) = (343.0 + 9.0 * sqrt21) / 2520.0;
   sigmaMatrix(3, 4) = (-21.0 - 3.0 * sqrt21) / 1960.0;
   // Row 5
   sigmaMatrix(4, 0) = 1.0 / 20.0;
   sigmaMatrix(4, 1) = 49.0 / 180.0;
   sigmaMatrix(4, 2) = 16.0 / 45.0;
   sigmaMatrix(4, 3) = 49.0 / 180.0;
   sigmaMatrix(4, 4) = 1.0 / 20.0;
   
   #ifdef DEBUG_LOBATTO_8ORDER
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
ImplicitRungeKutta* LobattoIIIA_8Order::Clone() const
{
   return (new LobattoIIIA_8Order(*this));
}


