//$Id$
//------------------------------------------------------------------------------
//                              FiniteThrust
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/12/20
//
/**
 * Implements the FiniteThrust class used to model the acceleration during a 
 * finite burn. 
 */
//------------------------------------------------------------------------------


#include "FiniteThrust.hpp"

//#define DEBUG_FINITETHRUST_INIT
//#define DEBUG_REGISTRATION
//#define DEBUG_FINITETHRUST_EXE
//#define DEBUG_MASS_FLOW
//#define DEBUG_STATE_ALLOCATION

#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include <sstream>               // for stringstream


//------------------------------------------------------------------------------
// FiniteThrust(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor for forces from finite burns.
 * 
 * @param name Name of the constructed instance.
 */
//------------------------------------------------------------------------------
FiniteThrust::FiniteThrust(const std::string &name) :
   PhysicalModel        (Gmat::PHYSICAL_MODEL, "FiniteThrust", name),
   satCount             (0),
   cartIndex            (-1),
   fillCartesian        (false),
   satThrustCount       (0),
   mDotIndex            (-1),
   depleteMass          (true)
{
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   objectTypeNames.push_back("FiniteThrust");
   isConservative = false;
}


//------------------------------------------------------------------------------
// ~FiniteThrust()
//------------------------------------------------------------------------------
/**
 * Destructor for forces from finite burns.
 */
//------------------------------------------------------------------------------
FiniteThrust::~FiniteThrust()
{
   #ifdef DEBUG_STATE_ALLOCATION
      MessageInterface::ShowMessage("Deleting FiniteThrust at %p\n", this);
   #endif
}


//------------------------------------------------------------------------------
// FiniteThrust(const FiniteThrust& ft)
//------------------------------------------------------------------------------
/**
 * Copy constructor for forces from finite burns.
 * 
 * @param ft The original that gets copied.
 */
//------------------------------------------------------------------------------
FiniteThrust::FiniteThrust(const FiniteThrust& ft) :
   PhysicalModel           (ft),
   burnNames               (ft.burnNames),
   mySpacecraft            (ft.mySpacecraft),
   spacecraft              (ft.spacecraft),
   satCount                (ft.satCount),
   cartIndex               (ft.cartIndex),
   fillCartesian           (ft.fillCartesian),
   satThrustCount          (ft.satThrustCount),
   mDotIndex               (ft.mDotIndex),
   depleteMass             (ft.depleteMass)
{
   burns.clear();
   scIndices.clear();
}


//------------------------------------------------------------------------------
// FiniteThrust& operator=(const FiniteThrust& ft)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param ft Instance that provides the parameter values.
 * 
 * @return This instance, set to match the input instance.
 */
//------------------------------------------------------------------------------
FiniteThrust& FiniteThrust::operator=(const FiniteThrust& ft)
{
   if (this != &ft)
   {
      PhysicalModel::operator=(ft);

      burnNames    = ft.burnNames;
      spacecraft   = ft.spacecraft;
      mySpacecraft = ft.mySpacecraft;
      burns.clear();
      scIndices.clear();

      satCount       = ft.satCount;
      cartIndex      = ft.cartIndex;
      fillCartesian  = ft.fillCartesian;
      satThrustCount = ft.satThrustCount;
      mDotIndex      = ft.mDotIndex;
      depleteMass    = ft.depleteMass;
   }
   return *this;
}


//------------------------------------------------------------------------------
// bool FiniteThrust::operator==(const FiniteThrust& ft) const
//------------------------------------------------------------------------------
/**
 * Checks to see if two FiniteThrusts apply the same thruster force.
 *
 * The check performed here identifies if the burns overlap at all; in other
 * words, if the input burn applies finite thrust using any thruster-spacecraft
 * combination that is also covered by this instance, the return value will be
 * true indicating that the finite thrust from ft overlaps with this one.
 *
 * @param ft The second FiniteThrust
 *
 * @return true if the thrust is applied from the same thruster on the same
 * spacecraft as this instance applies, false if it is a different independent
 * force.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::operator==(const FiniteThrust& ft) const
{
   bool retval = false;

   #ifdef DEBUG_COMPARISON
      MessageInterface::ShowMessage("Comparing: Does %p == %p?\n", this, &ft);
      MessageInterface::ShowMessage("   %s\n", (isInitialized ? "Initialized" :
            "Not Initialized"));
   #endif
//   if (isInitialized)
   {
      StringArray ftBurnNames = ((FiniteThrust&)ft).GetRefObjectNameArray(
            Gmat::FINITE_BURN);
      StringArray ftSatNames = ((FiniteThrust&)ft).GetRefObjectNameArray(
            Gmat::SPACECRAFT);

      #ifdef DEBUG_COMPARISON
         MessageInterface::ShowMessage("   %d Sats, %d burns\n",
               ftSatNames.size(), ftBurnNames.size());
      #endif
      // Check to see if there is spacecraft overlap
      Integer satOverlap = 0, burnOverlap = 0;
      for (UnsignedInt i = 0; i < mySpacecraft.size(); ++i)
      {
         #ifdef DEBUG_COMPARISON
            MessageInterface::ShowMessage("   Sats: %s?\n",
                  mySpacecraft[i].c_str());
         #endif
         if (find(ftSatNames.begin(), ftSatNames.end(),
               mySpacecraft[i]) != ftSatNames.end())
         {
            ++satOverlap;
            #ifdef DEBUG_COMPARISON
               MessageInterface::ShowMessage("   MatchFound\n");
            #endif
         }
      }

      for (UnsignedInt i = 0; i < burnNames.size(); ++i)
      {
         #ifdef DEBUG_COMPARISON
            MessageInterface::ShowMessage("   Burns: %s?\n",
                  burnNames[i].c_str());
         #endif
         if (find(ftBurnNames.begin(), ftBurnNames.end(),
               burnNames[i]) != ftBurnNames.end())
         {
            ++burnOverlap;
            #ifdef DEBUG_COMPARISON
               MessageInterface::ShowMessage("   MatchFound\n");
            #endif
         }
      }

      // Note that the current test is not yet rigorous -- it will return a
      // false positive if sat1.thruster2 and sat2.thruster1 both fire if both
      // sats and thrusters are in the lists
      if ((satOverlap > 0) && (burnOverlap > 0))
         retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Overridden Clone method used to make copies from a GmatBase pointer.
 * 
 * @return New instance, set to look like this one.
 */
//------------------------------------------------------------------------------
GmatBase* FiniteThrust::Clone() const
{
   return (new FiniteThrust(*this));
}


//------------------------------------------------------------------------------
// void Clear(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Clears the arrays of elements that get set by the Propagate commands
 * 
 * @param type The type of element that gets cleared.  Set to 
 *             Gmat::UNKNOWN_OBJECT to clear all of the configrable arrays.
 */
//------------------------------------------------------------------------------
void FiniteThrust::Clear(const UnsignedInt type)
{
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACECRAFT)) 
   {
      mySpacecraft.clear();
      spacecraft.clear();
   }
   
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::THRUSTER)) 
   {
      burnNames.clear();
   }
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the names for referenced objects.
 * 
 * FiniteThrust instances use Spacecraft and FiniteThrust objects.  This method
 * sets the names for those objects.
 * 
 * @param type The type of object that has the name.
 * @param name The object's name
 * 
 * @return true on success, false otherwise.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::SetRefObjectName(const UnsignedInt type,
                                  const std::string &name)
{
   if (type == Gmat::SPACECRAFT) 
   {
      if (find(mySpacecraft.begin(), mySpacecraft.end(), name) == mySpacecraft.end())
         mySpacecraft.push_back(name);
      return true;
   }
   
   if (type == Gmat::FINITE_BURN) 
   {
      if (find(burnNames.begin(), burnNames.end(), name) == burnNames.end())
         burnNames.push_back(name);
      return true;
   }
   
   return PhysicalModel::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Accesses the names for referenced objects.
 * 
 * @param type The type of object requested.
 * 
 * @return The vector of names.
 */
//------------------------------------------------------------------------------
const StringArray& FiniteThrust::GetRefObjectNameArray(
                                                   const UnsignedInt type)
{
   if (type == Gmat::SPACECRAFT)
      return mySpacecraft;

   if (type == Gmat::FINITE_BURN)
      return burnNames;
      
   return PhysicalModel::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets referenced object pointers.
 * 
 * @param obj The object.
 * @param type The type of the object.
 * @param name The object's name.
 * 
 * @return true if the object is set, false if not.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                const std::string &name)
{
   if (type == Gmat::FINITE_BURN) 
   {
      if (obj->IsOfType("FiniteBurn") == false)
         throw ODEModelException(
            "FiniteThrust::SetRefObject cannot use objects of type " + 
            obj->GetTypeName());
      if (find(burns.begin(), burns.end(), obj) == burns.end())
      {
         burns.push_back((FiniteBurn*)obj);
         if (find(burnNames.begin(), burnNames.end(), name) == burnNames.end())
         {
            burnNames.push_back(name);
         }
      }
      return true;
   }
   
   return PhysicalModel::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets referenced object pointers in an object array.
 *
 * @param obj The object.
 * @param type The type of the object.
 * @param name The object's name.
 * @param index The index of the object in the array
 *
 * @return true if the object is set, false if not.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string &name, const Integer index)
{
   return PhysicalModel::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string &oldName,
//       const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Changes the name for reference objects
 *
 * @param type The type of the object.
 * @param oldName The object's name before the change.
 * @param newName The object's proposed name after the change.
 *
 * @return true if a change was made, false if not
 */
//------------------------------------------------------------------------------
bool FiniteThrust::RenameRefObject(const UnsignedInt type,
      const std::string &oldName, const std::string &newName)
{
   if (type == Gmat::FINITE_BURN)
   {
      for (UnsignedInt i = 0; i < burnNames.size(); ++i)
      {
         if (burnNames[i] == oldName)
            burnNames[i] = newName;
      }
      for (UnsignedInt i = 0; i < burns.size(); ++i)
      {
         if (burns[i] != NULL)
            if (burns[i]->GetName() == oldName)
               burns[i]->SetName(newName);
      }
      return true;
   }

   return PhysicalModel::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a reference object
 *
 * @param type The type of the object
 * @param name The object's name
 *
 * @return The object pointer
 */
//------------------------------------------------------------------------------
GmatBase* FiniteThrust::GetRefObject(const UnsignedInt type,
      const std::string &name)
{
   if (type == Gmat::FINITE_BURN)
   {
      if ((name == "") && (burns.size() > 0))
         return burns[0];

      for (UnsignedInt i = 0; i < burns.size(); ++i)
      {
         if (burns[i] != NULL)
            if (burns[i]->GetName() == name)
               return burns[i];
      }

      return NULL;
   }

   if (type == Gmat::SPACECRAFT)
   {
      if (name == "")
         if (spacecraft.size() > 0)
            return spacecraft[0];
      for (UnsignedInt i = 0; i < spacecraft.size(); ++i)
         if (spacecraft[i]->GetName() == name)
            return spacecraft[i];
      return NULL;
   }

   return PhysicalModel::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a pointer to a reference object from an array
 *
 * @param type The type of the object
 * @param name The object's name
 * @param index The index of the object in the array
 *
 * @return The object pointer
 */
//------------------------------------------------------------------------------
GmatBase* FiniteThrust::GetRefObject(const UnsignedInt type,
      const std::string &name, const Integer index)
{
   return PhysicalModel::GetRefObject(type, name, index);
}


//------------------------------------------------------------------------------
// bool IsTransient()
//------------------------------------------------------------------------------
/**
 * Specifies whether the force is transient.
 * 
 * @return true for all FiniteThrust forces.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::IsTransient()
{
   return true;
}

//------------------------------------------------------------------------------
// bool DepletesMass()
//------------------------------------------------------------------------------
/**
 * Detects mass depletion from a PhysicalModel
 *
 * @return true if the model depletes mass, false if it does not
 */
//------------------------------------------------------------------------------
bool FiniteThrust::DepletesMass()
{
   return depleteMass;
}


//------------------------------------------------------------------------------
// void SetPropList(ObjectArray *soList)
//------------------------------------------------------------------------------
/**
 * Sets the list of propagated space objects for transient forces.
 */
//------------------------------------------------------------------------------
void FiniteThrust::SetPropList(ObjectArray *soList)
{
   spacecraft.clear();
   for (ObjectArray::iterator i = soList->begin(); i != soList->end(); ++i)
      spacecraft.push_back(*i);
   
   #ifdef DEBUG_FINITETHRUST_INIT
      MessageInterface::ShowMessage("Spacecraft list contains %d objects\n",
            spacecraft.size());
   #endif
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Set up data structures to manage finite burns
 * 
 * @return              true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::Initialize()
{
   #ifdef DEBUG_FINITETHRUST_INIT
      MessageInterface::ShowMessage("FiniteThrust::Initialize entered\n");
   #endif

   isInitialized = PhysicalModel::Initialize();

   if (modelState != NULL)
   {
      delete [] modelState;
      modelState = NULL;
      rawState = NULL;
   }
      
   if (!isInitialized)
      throw ODEModelException("Unable to initialize FiniteThrust base");
      
   // set up the indices into the state vector that match spacecraft with active 
   // thrusters
   Integer satIndex, stateIndex;
   #ifdef DEBUG_FINITETHRUST_INIT
      MessageInterface::ShowMessage(
            "   Looping over satNames\n");
   #endif
   for (StringArray::iterator satName = mySpacecraft.begin(); 
        satName != mySpacecraft.end(); ++satName)
   {
      #ifdef DEBUG_FINITETHRUST_INIT
         MessageInterface::ShowMessage(
               "      satName = %s\n", satName->c_str());
      #endif
      satIndex = 0;
      stateIndex = 0;
      
      for (ObjectArray::iterator propSat = spacecraft.begin();
           propSat != spacecraft.end(); ++propSat) 
      {
         #ifdef DEBUG_FINITETHRUST_INIT
            MessageInterface::ShowMessage(
                  "      Looking at satIndex %d; stateIndex = %d\n", satIndex, 
                  stateIndex);
         #endif
         if ((*propSat)->GetName() == *satName) 
         {
            #ifdef DEBUG_FINITETHRUST_INIT
               MessageInterface::ShowMessage(
                  "      FiniteThrust::Initialize Matched up %s\n", 
                  satName->c_str());
            #endif
         }
         #ifdef DEBUG_FINITETHRUST_INIT
            MessageInterface::ShowMessage(
                  "      PropSat is \"%s\"\n", (*satName).c_str());
         #endif
         if ((*propSat)->IsOfType(Gmat::SPACEOBJECT))
         {
            stateIndex += ((SpaceObject*)(*propSat))->GetState().GetSize();
            ++satIndex;
         }
      }
   }

   for (UnsignedInt i = 0; i < burns.size(); ++i)
   {
      // Verify that the burn identifies minimal objects needed
      FiniteBurn *theBurn = burns[i];
      if (theBurn->GetStringArrayParameter(
            theBurn->GetParameterID("Thrusters")).size() == 0)
         throw ODEModelException("The FiniteBurn object \"" +
               theBurn->GetName() + "\" does not identify any Thrusters, and "
               "cannot be used for a finite burn.");
   }
   
   #ifdef DEBUG_FINITETHRUST_INIT
      MessageInterface::ShowMessage(
            "FiniteThrust::Initialize finished\n");
   #endif
   
   isInitialized = true;
   return true;
}


//------------------------------------------------------------------------------
// bool GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Method invoked to calculate derivatives
 * 
 * This method is invoked to fill the deriv array with derivative information 
 * for the system that is being integrated.  It uses the state and elapsedTime 
 * parameters, along with the time interval dt passed in as a parameter, to 
 * calculate the derivative information at time \f$t=t_0+t_{elapsed}+dt\f$.
 *
 * @param dt            Additional time increment for the derivatitive 
 *                      calculation; defaults to 0.
 * @param state         Pointer to the current state data.  This can differ
 *                      from the PhysicalModel state if the subscribing
 *                      integrator samples other state values during 
 *                      propagation.  (For example, the Runge-Kutta integrators 
 *                      do this during the stage calculations.)
 * @param order         The order of the derivative to be taken (first 
 *                      derivative, second derivative, etc)
 *
 * @return              true if the call succeeds, false on failure.
 */
//------------------------------------------------------------------------------
bool FiniteThrust::GetDerivatives(Real * state, Real dt, Integer order, 
      const Integer id)
{
   #ifdef DEBUG_FINITETHRUST_EXE
      MessageInterface::ShowMessage(
            "FiniteThrust::GetDerivatives entered; fillCartesian is %s, "
            "maneuvering %d spacecraft\n", (fillCartesian ? "true" : "false"),
            spacecraft.size());
   #endif

   Real accel[3], mDot, burnData[4];
   Integer i6, mloc = -1;
   Integer i = 0, j = 0;
   SpaceObject *sat;
   
   if (fillCartesian)
   {
      // Loop through the spacecraft list, building accels for active sats
      for (ObjectArray::iterator sc = spacecraft.begin();
           sc != spacecraft.end(); ++sc) 
      {
         i6 = cartIndex + i * 6;
         
         if (1)  // <-- NEED A TEST HERE!
         {
            if (j < satThrustCount)
            {
               mloc = mDotIndex + j;
               ++j;
            }
         }
         else
            mloc = -1;

         if ((*sc)->IsOfType(Gmat::SPACEOBJECT) == false)
            continue;
         
         // Just a convenience
         sat = (SpaceObject*)(*sc);

         #ifdef DEBUG_FINITETHRUST_EXE
            MessageInterface::ShowMessage("   Checking spacecraft %s\n", 
               sat->GetName().c_str());
         #endif

         if (find(mySpacecraft.begin(), mySpacecraft.end(), sat->GetName()) != 
             mySpacecraft.end()) 
         {

            #ifdef DEBUG_FINITETHRUST_EXE
               MessageInterface::ShowMessage("   Maneuvering\n");
            #endif
   
            if (sat->GetType() != Gmat::SPACECRAFT)
               throw ODEModelException("FiniteThrust::GetDerivatives Finite "
               "burns cannot maneuver " + sat->GetTypeName() + " objects");
            // Start with zero thrust
            mDot = accel[0] = accel[1] = accel[2] = 0.0;
   
            // Accumulate thrust and mass flow for each active thruster
            for (std::vector <FiniteBurn*>::iterator fb = burns.begin();
                 fb != burns.end(); ++fb)
            {
               // Setting the spacecraft here makes the setting too late for parameters evaluated at maneuver start
               (*fb)->SetSpacecraftToManeuver((Spacecraft*)sat);
               Real now = epoch + (elapsedTime + dt) / GmatTimeConstants::SECS_PER_DAY;
               if ((*fb)->Fire(burnData, now)) 
               {
                  #ifdef DEBUG_FINITETHRUST_EXE
                     MessageInterface::ShowMessage("%s ",(*fb)->GetName().c_str());
                  #endif
                  accel[0] += burnData[0];
                  accel[1] += burnData[1];
                  accel[2] += burnData[2];
                  #ifdef DEBUG_MASS_FLOW
                     MessageInterface::ShowMessage(
                           "   --> direction = [%.12le %.12le %.12le]",
                           accel[0], accel[1], accel[2]);
                  #endif
                  if ((*fb)->DepletesMass())
                  {
                     if (order != 1)
                        throw ODEModelException("Mass depletion cannot be "
                              "performed with the selected propagator.");
                     mDot += burnData[3];
                     #ifdef DEBUG_MASS_FLOW
                        MessageInterface::ShowMessage("  mDot =  %.12lf\n",
                              mDot);
                     #endif
                  }
                  #ifdef DEBUG_MASS_FLOW
                  else
                     MessageInterface::ShowMessage("  mDot =  0.0\n");
                  #endif

               }
               #ifdef DEBUG_FINITETHRUST_EXE
                  MessageInterface::ShowMessage(
                     "\n   Acceleration = [%18le  %18le  %18le]; "
                     "dm/dt = %.18le", accel[0], accel[1], accel[2], mDot);
               #endif
            }
      
            #ifdef DEBUG_FINITETHRUST_EXE
               MessageInterface::ShowMessage("\n");
            #endif
            
            // Apply the burns to the state vector
            if (order == 1) 
            {
               // dr/dt = v
               deriv[i6]     = 
               deriv[1 + i6] = 
               deriv[2 + i6] = 0.0;
               deriv[3 + i6] = accel[0];
               deriv[4 + i6] = accel[1];
               deriv[5 + i6] = accel[2];

               if (mloc >= 0)
               {
                  deriv[mloc+i]   = mDot;  // Is this right???
               }
            } 
            else  
            {
               deriv[ i6 ] = accel[0]; 
               deriv[i6+1] = accel[1]; 
               deriv[i6+2] = accel[2]; 
               deriv[i6+3] = 0.0; 
               deriv[i6+4] = 0.0; 
               deriv[i6+5] = 0.0; 
            }
         }
         else
         {
            // Zero any burn contribution that is not made for this spacecraft
            if (order == 1) 
            {
               // dr/dt = v
               deriv[i6]     =
               deriv[1 + i6] =
               deriv[2 + i6] =
               deriv[3 + i6] =
               deriv[4 + i6] =
               deriv[5 + i6] = 0.0;
            } 
            else  
            {
               deriv[ i6 ] = 0.0; 
               deriv[i6+1] = 0.0; 
               deriv[i6+2] = 0.0; 
               deriv[i6+3] = 0.0; 
               deriv[i6+4] = 0.0; 
               deriv[i6+5] = 0.0; 
            }
         }
         ++i;
      }
   }
   
   #ifdef DEBUG_FINITETHRUST_EXE
      //ShowDerivative("FiniteThrust::GetDerivatives() AFTER compute", state,
      //               satCount);
      MessageInterface::ShowMessage
         ("     deriv[1:3] = [%18le %18le %18le]\n     deriv[4:6] = [%18le %18le %18le]\n",
          deriv[0], deriv[1], deriv[2], deriv[3], deriv[4], deriv[5]);
      MessageInterface::ShowMessage("FiniteThrust::GetDerivatives finished\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft* sc)
//------------------------------------------------------------------------------
/**
 * Retrieves the Cartesian state vector of derivatives w.r.t. time
 *
 * @param sc The spacecraft that holds the state vector
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
Rvector6 FiniteThrust::GetDerivativesForSpacecraft(Spacecraft* sc)
{
   Rvector6 dv;

   Real accel[3], burnData[4];
   //Real mDot;     // in case we want a mass flow parameter

   if (find(mySpacecraft.begin(), mySpacecraft.end(), sc->GetName()) !=
       mySpacecraft.end())
   {
      // Start with zero thrust
      //mDot =
      accel[0] = accel[1] = accel[2] = 0.0;

      // Accumulate thrust and mass flow for each active thruster
      for (std::vector <FiniteBurn*>::iterator fb = burns.begin();
           fb != burns.end(); ++fb)
      {
         (*fb)->SetSpacecraftToManeuver(sc);
         Real now = sc->GetEpoch();
         if ((*fb)->Fire(burnData, now))
         {
            accel[0] += burnData[0];
            accel[1] += burnData[1];
            accel[2] += burnData[2];

            // No mass flow (for now, so left in place)
            //if ((*fb)->DepletesMass())
            //{
            //   if (order != 1)
            //      throw ODEModelException("Mass depletion cannot be "
            //              "performed with the selected propagator.");
            //   mDot += burnData[3];
            //   #ifdef DEBUG_MASS_FLOW
            //      MessageInterface::ShowMessage("  mDot =  %.12lf\n",
            //            mDot);
            //   #endif
            //}
            //#ifdef DEBUG_MASS_FLOW
            //   else
            //      MessageInterface::ShowMessage("  mDot =  0.0\n");
            //#endif
         }
      }

      // Apply the burns to the state vector
      dv[0]     =
      dv[1] =
      dv[2] = 0.0;
      dv[3] = accel[0];
      dv[4] = accel[1];
      dv[5] = accel[2];

   }
   else // Thrust does not apply to this spacecraft
   {
      dv[0] =
      dv[1] =
      dv[2] =
      dv[3] =
      dv[4] =
      dv[5] = 0.0;
   }

   return dv;
}


//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative 
 * information for a specified type.
 * 
 * @param id State Element ID for the derivative type
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool FiniteThrust::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "FiniteThrust checking for support for id %d\n", id);
   #endif
   
   if (id == Gmat::CARTESIAN_STATE)
      return true;
   
//   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
//      return true;
   
   if (id == Gmat::MASS_FLOW)
   {
      #ifdef DEBUG_REGISTRATION
         MessageInterface::ShowMessage("I support mass flow!\n");
      #endif
      return true;
   }
   
   return PhysicalModel::SupportsDerivative(id);
}


//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Function used to set the start point and size information for the state 
 * vector, so that the derivative information can be placed in the correct place 
 * in the derivative vector.
 * 
 * @param id State Element ID for the derivative type
 * @param index Starting index in the state vector for this type of derivative
 * @param quantity Number of objects that supply this type of data
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool FiniteThrust::SetStart(Gmat::StateElementId id, Integer index, 
                      Integer quantity, Integer sizeOfType)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage("FiniteThrust setting start data for id = "
            "%d to index %d; %d objects identified\n", id, index, quantity);
   #endif
   
   bool retval = false;
   
   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         satCount = quantity;
         cartIndex = index;
         fillCartesian = true;
         retval = true;
         break;
         
//      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
//         stmCount = quantity;
//         stmIndex = index;
//         fillSTM = true;
//         stmRowCount = sqrt(sizeOfType);
//         retval = true;
//         break;
         
//      case Gmat::ORBIT_A_MATRIX:
//         stmCount = quantity;
//         stmIndex = index;
//         fillSTM = true;
//         stmRowCount = sqrt(sizeOfType);
//         retval = true;
//         break;

      case Gmat::MASS_FLOW:
         // todo: add mass flow bits here
         satThrustCount = quantity;
         mDotIndex = index;
         depleteMass = true;
         retval = true;
         break;
         
      default:
         break;
   }
   
   return retval;
}
