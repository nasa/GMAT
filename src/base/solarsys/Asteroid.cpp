//$Id: Asteroid.cpp 5553 2008-06-03 16:46:30Z djcinsb $
//------------------------------------------------------------------------------
//                                  Asteroid
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
// Author: Wendy C. Shoan
// Created: 2009.01.12
//
/**
 * Implementation of the Asteroid class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "Asteroid.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"
#include "StringUtil.hpp"
#include "ColorTypes.hpp"               // for namespace GmatColor::

//#define DEBUG_ASTEROID 1


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none at this time


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Asteroid(std::string name)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Asteroid class
 * (default constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body (default is "").
 */
//------------------------------------------------------------------------------
Asteroid::Asteroid(std::string name) :
   CelestialBody     ("Asteroid",name)
{
   objectTypeNames.push_back("Asteroid"); 
   parameterCount      = AsteroidParamCount;
   
   // Set default colors
   SetDefaultColors(GmatColor::SALMON, GmatColor::DARK_GRAY);
   
   theCentralBodyName  = GmatSolarSystemDefaults::SUN_NAME; 
   bodyType            = Gmat::ASTEROID;
   bodyNumber          = -1;
   referenceBodyNumber = -1;
   

   DeterminePotentialFileNameFromStartup();
   SaveAllAsDefault();
}

//------------------------------------------------------------------------------
//  Asteroid(std::string name, const std::string &cBody)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Asteroid class
 * (constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body.
 * @param <cBody> pointer to a central body.
 */
//------------------------------------------------------------------------------
Asteroid::Asteroid(std::string name, const std::string &cBody) :
CelestialBody     ("Asteroid",name)
{
   objectTypeNames.push_back("Asteroid");
   parameterCount      = AsteroidParamCount;

   theCentralBodyName  = cBody; 
   bodyType            = Gmat::ASTEROID;
   bodyNumber          = -1;
   referenceBodyNumber = -1;

   DeterminePotentialFileNameFromStartup();
   SaveAllAsDefault();
}

//------------------------------------------------------------------------------
//  Asteroid(const Asteroid &copy)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Asteroid class as a copy of the
 * specified Asteroid class (copy constructor).
 *
 * @param <copy> Asteroid object to copy.
 */
//------------------------------------------------------------------------------
Asteroid::Asteroid(const Asteroid &copy) :
   CelestialBody (copy)
{
}

//------------------------------------------------------------------------------
//  Asteroid& operator= (const Asteroid& copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Asteroid class.
 *
 * @param <copy> the Asteroid object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" Asteroid with data of input Asteroid copy.
 */
//------------------------------------------------------------------------------
Asteroid& Asteroid::operator=(const Asteroid &copy)
{
   if (&copy == this)
      return *this;

   CelestialBody::operator=(copy);
   return *this;
}

//------------------------------------------------------------------------------
//  ~Asteroid()
//------------------------------------------------------------------------------
/**
 * Destructor for the Asteroid class.
 */
//------------------------------------------------------------------------------
Asteroid::~Asteroid()
{
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Asteroid.
 *
 * @return clone of the Asteroid.
 */
//------------------------------------------------------------------------------
GmatBase* Asteroid::Clone() const
{
   return (new Asteroid(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Set this instance to match the one passed in.
 *
 * @param <orig> The object that is being copied.
 */
//---------------------------------------------------------------------------
void Asteroid::Copy(const GmatBase* orig)
{
   operator=(*((Asteroid *)(orig)));
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none at this time

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

