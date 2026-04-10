//$Id$
//------------------------------------------------------------------------------
//                              ManeuverFrame
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
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the base class for maneuver reference frames. 
 */
//------------------------------------------------------------------------------

#include "ManeuverFrame.hpp"
#include <math.h>


//------------------------------------------------------------------------------
//  ManeuverFrame()
//------------------------------------------------------------------------------
/**
 * Constructs the maneuver frame object (default constructor).
 */
//------------------------------------------------------------------------------
ManeuverFrame::ManeuverFrame() :
   position        (NULL),
   velocity        (NULL)
{
   basisMatrix[0][0] = basisMatrix[1][1] = basisMatrix[2][2] = 1.0;
   basisMatrix[0][1] = basisMatrix[0][2] = basisMatrix[1][0] =
   basisMatrix[1][2] = basisMatrix[2][0] = basisMatrix[2][1] = 0.0;
}


//------------------------------------------------------------------------------
//  ~ManeuverFrame()
//------------------------------------------------------------------------------
/**
 * Destroys the maneuver frame object (destructor).
 */
//------------------------------------------------------------------------------
ManeuverFrame::~ManeuverFrame()
{
}


//------------------------------------------------------------------------------
//  ManeuverFrame(const ManeuverFrame& mf)
//------------------------------------------------------------------------------
/**
 * Constructs the maneuver frame object (copy constructor).
 * 
 * @param <mf> Object that is copied.
 */
//------------------------------------------------------------------------------
ManeuverFrame::ManeuverFrame(const ManeuverFrame& mf) :
   centralBody      (mf.centralBody),
   referenceBody    (mf.referenceBody),
   position         (NULL),
   velocity         (NULL)
{
	for (Integer i = 0; i < 3; i++)
	   for (Integer j = 0; j < 3; j++)
	      basisMatrix[i][j] = mf.basisMatrix[i][j];
}


//------------------------------------------------------------------------------
//  ManeuverFrame& operator=(const ManeuverFrame& mf)
//------------------------------------------------------------------------------
/**
 * Sets one maneuver frame object to match another (assignment operator).
 * 
 * @param <mf> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
ManeuverFrame& ManeuverFrame::operator=(const ManeuverFrame& mf)
{
	centralBody      = mf.centralBody;
   referenceBody    = mf.referenceBody;
   position         = NULL;
   velocity         = NULL;
   
   for (Integer i = 0; i < 3; i++)
	   for (Integer j = 0; j < 3; j++)
	      basisMatrix[i][j] = mf.basisMatrix[i][j];
	      
   if (this == &mf)
      return *this;
        
   return *this;
}


//------------------------------------------------------------------------------
//  void SetState(Real *pos, Real *vel)
//------------------------------------------------------------------------------
/**
 * Sets the pointers for the state data that need to incorporate the maneuver.
 * 
 * @param <pos> The starting address of the position data.
 * @param <vel> The starting address of the velocity data.  If this pointer is
 *              NULL, the code interpretes pos as a pointer to position data 
 *              followed by velocity data.
 */
//------------------------------------------------------------------------------
void ManeuverFrame::SetState(Real *pos, Real *vel)
{
   position = pos;
   if (vel)
      velocity = vel;
   else
      velocity = pos + 3;
}


//------------------------------------------------------------------------------
//  void CalculateBasis(Real basis[3][3])
//------------------------------------------------------------------------------
/**
 * Fills the input matrix with the orthonormal basis vectors used for the 
 * maneuver frame.
 * 
 * This method calls the protected CalculateBasis(void) method to perform the 
 * actual calculations.
 * 
 * @param <basis> The matrix that gets filled with the data.
 */
//------------------------------------------------------------------------------
void ManeuverFrame::CalculateBasis(Real basis[3][3])
{
   CalculateBasis();
    
   Integer i, j;
   for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
         basis[i][j] = basisMatrix[i][j];
}


//------------------------------------------------------------------------------
//  std::string GetFrameLabel(Integer id)
//------------------------------------------------------------------------------
/**
 * Gets the label for the maneuver frame principal axes.
 * 
 * @param <id> Integer ID of the axis -- 1, 2, or 3.
 * 
 * @return The label for the corresponding reference vector.
 */
//------------------------------------------------------------------------------
std::string ManeuverFrame::GetFrameLabel(Integer id)
{
   switch (id)
   {
      case 1:
         return "X";
      case 2:
         return "Y";
      case 3:
         return "Z";
      default:
         ;
   }
   return "Undefined";
}
