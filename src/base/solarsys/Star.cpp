//$Id$
//------------------------------------------------------------------------------
//                                  Star
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
// Created: 2004/01/29
//
/**
 * Implementation of the Star class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "A1Mjd.hpp"
#include "AngleUtil.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string
Star::PARAMETER_TEXT[StarParamCount - CelestialBodyParamCount] =
{
   "RadiantPower",
   "ReferenceDistance",
   "PhotosphereRadius"
};

const Gmat::ParameterType
Star::PARAMETER_TYPE[StarParamCount - CelestialBodyParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Star(std::string name)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Star class
 * (default constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body (default is the name of the Sun).
 */
//------------------------------------------------------------------------------
Star::Star(std::string name) :
   CelestialBody       ("Star",name)
{
   objectTypeNames.push_back("Star");
   parameterCount = StarParamCount;
   
   bodyType            = Gmat::STAR;
   bodyNumber          = 3;
   referenceBodyNumber = 3;
   rotationSrc         = Gmat::IAU_SIMPLIFIED;  // ??
   

   SaveAllAsDefault();
}

//------------------------------------------------------------------------------
//  Star(const Star &st)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Star class as a copy of the
 * specified Star class (copy constructor).
 *
 * @param <st> Star object to copy.
 */
//------------------------------------------------------------------------------
Star::Star(const Star &st) :
   CelestialBody       (st),
   radiantPower        (st.radiantPower),
   referenceDistance   (st.referenceDistance),
   photosphereRadius   (st.photosphereRadius)
{
}

//------------------------------------------------------------------------------
//  Star& operator= (const Star& st)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Star class.
 *
 * @param <st> the Star object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" Star with data of input Star st.
 */
//------------------------------------------------------------------------------
Star& Star::operator=(const Star &st)
{
   if (&st == this)
      return *this;

   CelestialBody::operator=(st);
   radiantPower        = st.radiantPower;
   referenceDistance   = st.referenceDistance;
   photosphereRadius   = st.photosphereRadius;

   return *this;
}

//------------------------------------------------------------------------------
//  ~Star()
//------------------------------------------------------------------------------
/**
 * Destructor for the Star class.
 */
//------------------------------------------------------------------------------
Star::~Star()
{
}

//------------------------------------------------------------------------------
//  Real GetRadiantPower() const
//------------------------------------------------------------------------------
/**
 * This method returns the radiant power of the star.
 *
 * @return radiant power of the star.
 *
 */
//------------------------------------------------------------------------------
Real Star::GetRadiantPower() const
{
   return radiantPower;
}

//------------------------------------------------------------------------------
//  Real GetReferenceDifference() const
//------------------------------------------------------------------------------
/**
 * This method returns the reference distance associated with the radiant power
 * of the star.
 *
 * @return reference distance of the star.
 *
 */
//------------------------------------------------------------------------------
Real Star::GetReferenceDistance() const
{
   return referenceDistance;
}

//------------------------------------------------------------------------------
//  bool SetRadiantPower(Real radPower, Real refDistance)
//------------------------------------------------------------------------------
/**
 * This method sets the radiant power and reference distance for the star.
 *
 * @param <radPower>    radiant power value.
 * @param <refDistance> reference distance value.
 *
 * @return flag indicating success of the operation.
 *
 */
//------------------------------------------------------------------------------
bool Star::SetRadiantPower(Real radPower, Real refDistance)
{
   radiantPower      = radPower;
   referenceDistance = refDistance;
   return true;
}

//------------------------------------------------------------------------------
//  bool SetPhotosphereRadius(Real rad)
//------------------------------------------------------------------------------
/**
 * This method sets the photosphere radius for the star.
 *
 * @param <rad>    photosphere radius value.
 *
 * @return flag indicating success of the operation.
 *
 */
//------------------------------------------------------------------------------
bool Star::SetPhotosphereRadius(Real rad)
{
   photosphereRadius = rad;
   return true;
}


//------------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns a boolean indicating whether or not the parameter
 * referenced by the input id is a read-only parameter for an object of this class.
 *
 * @param <id>    id of specified parameter
 *
 * @return true if the parameter is read-only; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool Star::IsParameterReadOnly(const Integer id) const
{
   // do not write out these items
   if ((id == RADIANT_POWER) || (id == REFERENCE_DISTANCE) ||
       (id == PHOTOSPHERE_RADIUS))
      return true;
   
   return CelestialBody::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Star.
 *
 * @return clone of the star.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Star::Clone() const
{
   return (new Star(*this));
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
void Star::Copy(const GmatBase* orig)
{
   operator=(*((Star *)(orig)));
}

//---------------------------------------------------------------------------
//  bool NeedsOnlyMainSPK()
//---------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the default SPK file contains
 * sufficient data for this Star.
 *
 * @return flag indicating whether or not an additional SPK file is needed
 *         for this Star; true, if only the default one is needed; false
 *         if an additional file is needed.
 */
//---------------------------------------------------------------------------
bool Star::NeedsOnlyMainSPK()
{
   if (instanceName == GmatSolarSystemDefaults::SUN_NAME)  return true;
   return false;
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Star::GetParameterText(const Integer id) const
{
   if ((id >= CelestialBodyParamCount) && (id < StarParamCount))
      return PARAMETER_TEXT[id - CelestialBodyParamCount];
   return CelestialBody::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer Star::GetParameterID(const std::string &str) const
{
   for (Integer i = CelestialBodyParamCount; i < StarParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - CelestialBodyParamCount])
         return i;
   }
   
   return CelestialBody::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Star::GetParameterType(const Integer id) const
{
   if ((id >= CelestialBodyParamCount) && (id < StarParamCount))
      return PARAMETER_TYPE[id - CelestialBodyParamCount];
      
   return CelestialBody::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Star::GetParameterTypeString(const Integer id) const
{
   return CelestialBody::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Star::GetRealParameter(const Integer id) const
{
   if (id == RADIANT_POWER)              return radiantPower;
   if (id == REFERENCE_DISTANCE)         return referenceDistance;
   if (id == PHOTOSPHERE_RADIUS)         return photosphereRadius;

   return CelestialBody::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <id>    ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return  Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Star::SetRealParameter(const Integer id, const Real value)
{
   if (id == RADIANT_POWER)              return (radiantPower        = value);
   if (id == REFERENCE_DISTANCE)         return (referenceDistance   = value);
   if (id == PHOTOSPHERE_RADIUS)         return (photosphereRadius   = value);
   
   return CelestialBody::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Rvector6 ComputeTwoBody(const A1Mjd &forTime)
//------------------------------------------------------------------------------
/**
 * This method computes the position and velocity at time forTime, with respect 
 * to the Earth.
 *
 * @return body state (position, velocity) at time forTime.
 *
 */
//------------------------------------------------------------------------------
Rvector6 Star::ComputeTwoBody(const A1Mjd &forTime)
{
   return KeplersProblem(forTime);   // already Earth-centered 
}


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

