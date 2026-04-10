//$Id$
//------------------------------------------------------------------------------
//                                  Barycenter
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
// Created: 2005/04/05
//
/**
 * Implementation of the Barycenter class.
 *
 */
//------------------------------------------------------------------------------

#include <vector>
#include "gmatdefs.hpp"
#include "CalculatedPoint.hpp"
#include "Barycenter.hpp"
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "MessageInterface.hpp"
#include "GmatDefaults.hpp"
#include "SolarSystem.hpp"
#include "ColorTypes.hpp"               // for namespace GmatColor::

//#define DEBUG_BARYCENTER
//#define DEBUG_BARYCENTER_STATE
//#define DEBUG_BARYCENTER_BODIES

//---------------------------------
// static data
//---------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Barycenter(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Barycenter class
 * (default constructor).
 *
 * @param <itsName> parameter indicating the name of the barycenter.
 */
//------------------------------------------------------------------------------
Barycenter::Barycenter(const std::string &itsName) :
   CalculatedPoint("Barycenter", itsName),
   builtInSP      (NULL)
{
   objectTypes.push_back(Gmat::BARYCENTER);
   objectTypeNames.push_back("Barycenter");
   parameterCount = BarycenterParamCount;
   
   // Set default colors
   if (itsName == GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME)
      SetDefaultColors(GmatColor::TEAL, GmatColor::DARK_GRAY);
   else
      SetDefaultColors(GmatColor::GOLD, GmatColor::DARK_GRAY);
}

//------------------------------------------------------------------------------
//  Barycenter(const Barycenter &bary)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Barycenter class as a copy of the
 * specified Barycenter class (copy constructor).
 *
 * @param <bary> Barycenter object to copy.
 */
//------------------------------------------------------------------------------
Barycenter::Barycenter(const Barycenter &bary) :
   CalculatedPoint          (bary),
   builtInSP                (NULL)
{
   parameterCount  = bary.parameterCount;
}

//------------------------------------------------------------------------------
//  Barycenter& operator= (const Barycenter& bary)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Barycenter class.
 *
 * @param <bary> the Barycenter object whose data to assign to "this"
 *               calculated point.
 *
 * @return "this" Barycenter with data of input Barycenter bary.
 */
//------------------------------------------------------------------------------
Barycenter& Barycenter::operator=(const Barycenter &bary)
{
   if (&bary == this)
      return *this;

   CalculatedPoint::operator=(bary);
   parameterCount  = bary.parameterCount;
   builtInSP       = bary.builtInSP;

   return *this;
}

//------------------------------------------------------------------------------
//  ~Barycenter()
//------------------------------------------------------------------------------
/**
 * Destructor for the Barycenter class.
 */
//------------------------------------------------------------------------------
Barycenter::~Barycenter()
{
   // nothing to do here ..... hm .. hm .. hm
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Barycenter.
 *
 * @return clone of the Barycenter.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Barycenter::Clone() const
{
   return (new Barycenter(*this));
}

//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void Barycenter::Copy(const GmatBase* orig)
{
   // We don't want to copy instanceName
   std::string name = instanceName;
   operator=(*((Barycenter *)(orig)));
   instanceName     = name;
}

//---------------------------------------------------------------------------
//  const Rvector6 GetMJ2000State(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000Eq state of the Barycenter at the time atTime.
 *
 * @param <atTime> Time for which the state is requested.
 *
 * @return state of the Barycenter at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector6 Barycenter::GetMJ2000State(const A1Mjd &atTime)
{
   // if it's built-in, get the state from the SpacePoint
   if (isBuiltIn)
   {
      lastStateTime = atTime;
      lastState     = builtInSP->GetMJ2000State(atTime);
      #ifdef DEBUG_BARYCENTER_STATE
         MessageInterface::ShowMessage("Computing state for Barycenter %s, whose builtInSP is %s\n",
               instanceName.c_str(), (builtInSP->GetName()).c_str());
      #endif
      return lastState;
   }
   // otherwise, sum the masses and states
   CheckBodies();
   #ifdef DEBUG_BARYCENTER
      MessageInterface::ShowMessage("Entering BaryCenter::GetMJ2000EqState at time %12.10f\n",
            atTime.Get());
   #endif
   Real     bodyMass = 0.0;
   Rvector3 bodyPos(0.0,0.0,0.0);
   Rvector3 bodyVel(0.0,0.0,0.0);

   Real     weight   = 0.0;
   Rvector3 sumMassPos(0.0,0.0,0.0);
   Rvector3 sumMassVel(0.0,0.0,0.0);
   Rvector6 bodyState;
   Real     sumMass  = GetMass();

   for (unsigned int i = 0; i < bodyList.size() ; i++)
   {
      bodyMass    = ((CelestialBody*) (bodyList.at(i)))->GetMass();
      bodyState   = (bodyList.at(i))->GetMJ2000State(atTime);
      bodyPos     = bodyState.GetR();
      bodyVel     = bodyState.GetV();
      weight      = bodyMass/sumMass;
      #ifdef DEBUG_BARYCENTER
         MessageInterface::ShowMessage("Mass (and weight) of body %s = %12.10f (%12.10f)\n",
               ((bodyList.at(i))->GetName()).c_str(), bodyMass, weight);
         MessageInterface::ShowMessage("    pos = %s\n", (bodyPos.ToString()).c_str());
         MessageInterface::ShowMessage("    vel = %s\n", (bodyVel.ToString()).c_str());
      #endif
      sumMassPos += (weight * bodyPos);
      sumMassVel += (weight * bodyVel);
   }
   #ifdef DEBUG_BARYCENTER
      MessageInterface::ShowMessage("sumMassPos = %s\n",
            (sumMassPos.ToString()).c_str());
   #endif
   lastState.Set(sumMassPos(0), sumMassPos(1), sumMassPos(2),
         sumMassVel(0), sumMassVel(1), sumMassVel(2));
   lastStateTime = atTime;
   return lastState;
}


const Rvector6 Barycenter::GetMJ2000State(const GmatTime &atTime)
{
   // if it's built-in, get the state from the SpacePoint
   if (isBuiltIn)
   {
      lastStateTimeGT = atTime;
      lastStateTime   = GmatTime(atTime).GetMjd();

      lastState = builtInSP->GetMJ2000State(atTime);
#ifdef DEBUG_BARYCENTER_STATE
      MessageInterface::ShowMessage("Computing state for Barycenter %s, whose builtInSP is %s\n",
         instanceName.c_str(), (builtInSP->GetName()).c_str());
#endif
      return lastState;
   }
   // otherwise, sum the masses and states
   CheckBodies();
#ifdef DEBUG_BARYCENTER
   MessageInterface::ShowMessage("Entering BaryCenter::GetMJ2000EqState at time %12.10f\n",
      GmatTime(atTime).GetMjd());
#endif
   Real     bodyMass = 0.0;
   Rvector3 bodyPos(0.0, 0.0, 0.0);
   Rvector3 bodyVel(0.0, 0.0, 0.0);

   Real     weight = 0.0;
   Rvector3 sumMassPos(0.0, 0.0, 0.0);
   Rvector3 sumMassVel(0.0, 0.0, 0.0);
   Rvector6 bodyState;
   Real     sumMass = GetMass();

   for (unsigned int i = 0; i < bodyList.size(); i++)
   {
      bodyMass = ((CelestialBody*)(bodyList.at(i)))->GetMass();
      bodyState = (bodyList.at(i))->GetMJ2000State(atTime);
      bodyPos = bodyState.GetR();
      bodyVel = bodyState.GetV();
      weight = bodyMass / sumMass;
#ifdef DEBUG_BARYCENTER
      MessageInterface::ShowMessage("Mass (and weight) of body %s = %12.10f (%12.10f)\n",
         ((bodyList.at(i))->GetName()).c_str(), bodyMass, weight);
      MessageInterface::ShowMessage("    pos = %s\n", (bodyPos.ToString()).c_str());
      MessageInterface::ShowMessage("    vel = %s\n", (bodyVel.ToString()).c_str());
#endif
      sumMassPos += (weight * bodyPos);
      sumMassVel += (weight * bodyVel);
   }
#ifdef DEBUG_BARYCENTER
   MessageInterface::ShowMessage("sumMassPos = %s\n",
      (sumMassPos.ToString()).c_str());
#endif
   lastState.Set(sumMassPos(0), sumMassPos(1), sumMassPos(2),
      sumMassVel(0), sumMassVel(1), sumMassVel(2));
   
   lastStateTimeGT = atTime;
   lastStateTime   = GmatTime(atTime).GetMjd();

   return lastState;
}


//---------------------------------------------------------------------------
//  const Rvector3 GetMJ2000Position(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000Eq position of the Barycenter at the time atTime.
 *
 * @param <atTime> Time for which the position is requested.
 *
 * @return position of the Barycenter at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector3 Barycenter::GetMJ2000Position(const A1Mjd &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetR());
}


const Rvector3 Barycenter::GetMJ2000Position(const GmatTime &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetR());
}


//---------------------------------------------------------------------------
//  const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000Eq velocity of the Barycenter at the time atTime.
 *
 * @param <atTime> Time for which the velocity is requested.
 *
 * @return velocity of the Barycenter at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector3 Barycenter::GetMJ2000Velocity(const A1Mjd &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetV());
}


const Rvector3 Barycenter::GetMJ2000Velocity(const GmatTime &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetV());
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the reference object.
 *
 * @param <obj>   reference object pointer.
 * @param <type>  type of the reference object.
 * @param <name>  name of the reference object.
 * @param <index> Index into the object array.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool Barycenter::SetRefObject(GmatBase *obj,
                              const UnsignedInt type,
                              const std::string &name)
{
   if ((obj->IsOfType(Gmat::SPACE_POINT)) && (!obj->IsOfType("CelestialBody")))
   {
      std::string errmsg = "The value of \"";
      errmsg += name + "\" for field \"BodyNames\" on CalculatedPoint \"";;
      errmsg += instanceName + "\" is not an allowed value.\n";
      errmsg += "The allowed values are: [CelestialBody or Barycenter (except SSB)].\n";
      throw SolarSystemException(errmsg);
   }

   return CalculatedPoint::SetRefObject(obj, type, name);
}


//---------------------------------------------------------------------------
//  Real GetMass()
//---------------------------------------------------------------------------
/**
 * Method returning the total mass of the celestial bodies included in
 * this Barycenter.
 *
 * @return total mass of the celestial bodies included in this Barycenter.
 */
//---------------------------------------------------------------------------
Real Barycenter::GetMass() 
{
   // if it's built-in, get the mass from the SpacePoint
   if (isBuiltIn)
      return ((CelestialBody*)builtInSP)->GetMass();
   // otherwise, sum the masses of the bodies
   Real     sumMass  = 0.0;
   for (unsigned int i = 0; i < bodyList.size() ; i++)
   {
      sumMass    += ((CelestialBody*) (bodyList.at(i)))->GetMass();
   }
   return sumMass;
}


//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Method that initializes this Barycenter.
 *
 * @return success flag for initializing
 */
//---------------------------------------------------------------------------
bool Barycenter::Initialize()
{
   #ifdef DEBUG_BARYCENTER_BODIES
      MessageInterface::ShowMessage("Entering Barycenter::Initialize\n   bodyNames:\n");
      for (unsigned int ii = 0; ii < bodyNames.size(); ii++)
         MessageInterface::ShowMessage("   %d    %s\n", ii, (bodyNames.at(ii)).c_str());
      MessageInterface::ShowMessage("and defaultBodies:\n");
      for (unsigned int ii = 0; ii < defaultBodies.size(); ii++)
         MessageInterface::ShowMessage("   %d    %s\n", ii, (defaultBodies.at(ii)).c_str());
   #endif
   if (isBuiltIn)
   {
      // create the builtInSP here
      if ((builtInType == "SSB") || (builtInType == GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME))
      {
         builtInSP = theSolarSystem->GetSpecialPoint(GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME);
         if (!builtInSP)
         {
            std::string errmsg = "No special point \"";
            errmsg += GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME + "\" found in Solar System.\n";
            throw SolarSystemException(errmsg);
         }
      }
      else // no others currently available
      {
         throw SolarSystemException("Unknown built-in barycenter\n");
      }
   }
   else
   {
      if ((bodyNames.empty()) && !(defaultBodies.empty()))
      {
         for (unsigned int ii = 0; ii < defaultBodies.size(); ii++)
            bodyNames.push_back(defaultBodies.at(ii));
      }
      if (bodyNames.empty())
      {
         std::string errmsg = "No celestial body specified for Barycenter ";
         errmsg += instanceName + "\n";
         throw SolarSystemException(errmsg);
      }
      #ifdef DEBUG_BARYCENTER_BODIES
         MessageInterface::ShowMessage("at end of Barycenter::Initialize\n   bodyNames:\n");
         for (unsigned int ii = 0; ii < bodyNames.size(); ii++)
            MessageInterface::ShowMessage("   %d    %s\n", ii, (bodyNames.at(ii)).c_str());
      #endif
   }
   return CalculatedPoint::Initialize();
}

//---------------------------------------------------------------------------
//  StringArray GetBuiltInNames()
//---------------------------------------------------------------------------
/**
 * Returns the name(s) of the built-in space point.
 *
 * @return  name of the space point for the built-in barycenter
 */
//---------------------------------------------------------------------------
StringArray Barycenter::GetBuiltInNames()
{
   StringArray spNames;
   if (builtInType ==  "SSB")
      spNames.push_back("SolarSystemBarycenter");
   return spNames;
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  void CheckBodies()
//------------------------------------------------------------------------------
/**
 * Method for the Barycenter class that checks to make sure the bodyList has
 * been defined appropriately (i.e. all CelestialBody objects).
 *
 */
//------------------------------------------------------------------------------
void Barycenter::CheckBodies()
{
   if ((bodyNames.empty()) && !(defaultBodies.empty()))
   {
      for (unsigned int ii = 0; ii < defaultBodies.size(); ii++)
         bodyNames.push_back(defaultBodies.at(ii));
   }
   if (bodyNames.empty())
      throw SolarSystemException("Attempting to use Barycenter with no bodies set ...\n");

   for (unsigned int i = 0; i < bodyList.size() ; i++)
      if ((bodyList.at(i))->GetType() != Gmat::CELESTIAL_BODY)
         throw SolarSystemException(
         "Barycenter defined incorrectly with non-Celestial Body components");
}


