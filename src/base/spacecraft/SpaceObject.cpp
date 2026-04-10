//$Id$
//------------------------------------------------------------------------------
//                              SpaceObject
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
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/7/24
//
/**
 * Implements the base class used for spacecraft and formations. 
 */
//------------------------------------------------------------------------------


#include "SpaceObject.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_J2000_STATE 1
// #define DEBUG_STOPCONDITION_TRACKING

//---------------------------------
// static data
//---------------------------------
//
//const std::string SpaceObject::PARAMETER_TEXT[SpaceObjectParamCount -
//                                              SpacePointParamCount] =
//   {
//      "A1Epoch"
//   };
//
//
//const Gmat::ParameterType SpaceObject::PARAMETER_TYPE[SpaceObjectParamCount -
//                                                      SpacePointParamCount] =
//   {
//      Gmat::REAL_TYPE
//   };


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// SpaceObject(UnsignedInt typeId, const std::string &typeStr,
//             const std::string &instName)
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param <typeId>   UnsignedInt of the constructed object.
 * @param <typeStr>  String describing the type of object created.
 * @param <instName> Name of the constructed instance.
 */
//------------------------------------------------------------------------------
SpaceObject::SpaceObject(UnsignedInt typeId, const std::string &typeStr,
                         const std::string &instName) :
   SpacePoint        (typeId, typeStr, instName),
   state             (6),
   isManeuvering     (false),
   originName        ("Earth"),
   origin            (NULL),
   parmsChanged      (true),
   hasPublished      (false),
   hasEphemPropagated(false)
{
   objectTypes.push_back(Gmat::SPACEOBJECT);
   objectTypeNames.push_back("SpaceObject");
}


//------------------------------------------------------------------------------
// ~SpaceObject()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
SpaceObject::~SpaceObject()
{
}


//------------------------------------------------------------------------------
// SpaceObject(const SpaceObject& so)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <so> SpaceObject that is copied onto this one.
 */
//------------------------------------------------------------------------------
SpaceObject::SpaceObject(const SpaceObject& so) :
   SpacePoint        (so),
   state             (so.state),
   isManeuvering     (so.isManeuvering),
   originName        (so.originName),
   origin            (so.origin),
   parmsChanged      (true),
   lastStopTriggered (so.lastStopTriggered),
   hasPublished      (so.hasPublished),
   hasEphemPropagated(so.hasEphemPropagated)
{
   j2000Body = so.j2000Body;
}


//------------------------------------------------------------------------------
// SpaceObject& operator=(const SpaceObject& so)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <so> SpaceObject that is copied onto this one.
 *
 * @return this instance, configured like the input instance.
 */
//------------------------------------------------------------------------------
SpaceObject& SpaceObject::operator=(const SpaceObject& so)
{
   if (this == &so)
      return *this;
      
   SpacePoint::operator=(so);
   state         = so.state;
   isManeuvering = so.isManeuvering;
   originName    = so.originName;
   origin        = so.origin;
   parmsChanged  = true;       // Always update after using assignment
   lastStopTriggered = so.lastStopTriggered;
   hasPublished  = so.hasPublished;
   hasEphemPropagated = so.hasEphemPropagated;
   return *this;
}


//------------------------------------------------------------------------------
// GmatState& GetState()
//------------------------------------------------------------------------------
/**
 * Accessor for the GmatState of the object.
 *
 * @return The embedded GmatState.
 */
//------------------------------------------------------------------------------
GmatState& SpaceObject::GetState()
{
   #ifdef DEBUG_STATE_ACCESS
      MessageInterface::ShowMessage("Directly accessing state vector for %s\n",
            instanceName.c_str());
   #endif
   return state;
}

Rvector6 SpaceObject::GetLastState()
{
   Rvector6 result;
   Real *st = state.GetState();
   result.Set(st[0], st[1], st[2], st[3], st[4], st[5]);
   return result;
}

//------------------------------------------------------------------------------
// Real GetEpoch()
//------------------------------------------------------------------------------
/**
 * Accessor for the current epoch of the object, in A.1 Modified Julian format.
 *
 * @return The A.1 epoch.
 *
 * @todo The epoch probably should be TAI throughout GMAT.
 */
//------------------------------------------------------------------------------
Real SpaceObject::GetEpoch()
{
   return state.GetEpoch();
}

GmatTime SpaceObject::GetEpochGT()
{
   return state.GetEpochGT();
}


//------------------------------------------------------------------------------
// Real SetEpoch(const Real ep)
//------------------------------------------------------------------------------
/**
 * Accessor used to set epoch (in A.1 Modified Julian format) of the object.
 *
 * @param <ep> The new A.1 epoch.
 *
 * @return The updated A.1 epoch.
 *
 * @todo The epoch probably should be TAI throughout GMAT.
 */
//------------------------------------------------------------------------------
Real SpaceObject::SetEpoch(const Real ep)
{
   return state.SetEpoch(ep);
}


//Real SpaceObject::SetEpoch(const GmatTime ep)
//{
//   return state.SetEpoch(GmatTime(ep).GetMjd());
//}

GmatTime SpaceObject::SetEpochGT(const GmatTime ep)
{
   state.SetEpoch(GmatTime(ep).GetMjd());
   return state.SetEpochGT(ep);
}

//------------------------------------------------------------------------------
// bool IsManeuvering()
//------------------------------------------------------------------------------
/**
 * Function that checks to see if a finite burn needs to be applied to this 
 * object.
 *
 * @return true if a finite burn is active, false otherwise.
 */
//------------------------------------------------------------------------------
bool SpaceObject::IsManeuvering()
{
   return isManeuvering;
}


//------------------------------------------------------------------------------
// void IsManeuvering(bool mnvrFlag)
//------------------------------------------------------------------------------
/**
 * Function that sets or clears the maneuvering flag.
 * 
 * Derived classes may override this method so that the flag is updated based on
 * the state of the hardware attached to the SpaceObjects.
 * 
 * @param <mnvrFlag> The new value for the flag.
 */
//------------------------------------------------------------------------------
void SpaceObject::IsManeuvering(bool mnvrFlag)
{
   isManeuvering = mnvrFlag;
}


//------------------------------------------------------------------------------
// const StringArray& GetManeuveringMembers()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of maneuvering members
 *
 * For Spacecraft, this method returns the spacecraft name if it is maneuvering.
 * For Formations, the method is overridden to return names of each maneuvering
 * spacecraft.
 *
 * @return The list
 */
//------------------------------------------------------------------------------
const StringArray& SpaceObject::GetManeuveringMembers()
{
   maneuveringMembers.clear();

   if (isManeuvering)
      maneuveringMembers.push_back(instanceName);

   return maneuveringMembers;
}


//------------------------------------------------------------------------------
// bool ParametersHaveChanged()
//------------------------------------------------------------------------------
/**
 * Function used to test the parmsChanged flag.
 * 
 * @return The value of the flag.
 */
//------------------------------------------------------------------------------
bool SpaceObject::ParametersHaveChanged()
{
   return parmsChanged;
}

//------------------------------------------------------------------------------
// void ParametersHaveChanged(bool flag)
//------------------------------------------------------------------------------
/**
 * Uset to set or clear the parmsChanged flag from outside of the SpaceObject.
 * 
 * @param <flag>  The new value for the flag.
 */
//------------------------------------------------------------------------------
void SpaceObject::ParametersHaveChanged(bool flag)
{
   parmsChanged = flag;
}


/// @todo Waiting for CoordinateSystems in Spacecraft, then see if needed
void SpaceObject::SetOriginName(std::string cbName)
{
   originName = cbName;
}

SpacePoint* SpaceObject::GetOrigin()
{
   return origin;
}

const std::string SpaceObject::GetOriginName()
{
   return originName;
}


void SpaceObject::SetOrigin(SpacePoint *cb)
{
   origin = cb;
}



//------------------------------------------------------------------------------
// const Rvector6 GetMJ2000State(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Access the MJ2000 state for this SpaceObject.
 *
 * @param <atTime> Epoch for the state data.
 *
 * @return The Cartesian MJ2000 state.
 *
 * @todo Determine if this calculation should ber moved into the derived
 *       classes.
 */
//------------------------------------------------------------------------------
const Rvector6 SpaceObject::GetMJ2000State(const A1Mjd &atTime)
{
   #ifdef DEBUG_J2000_STATE
      MessageInterface::ShowMessage(
         "SpaceObject::GetMJ2000State entered; epoch is %lf\n", atTime.Get());
   #endif
   if (j2000Body == NULL)
   {
      std::string errmsg = "SpaceObject::GetMJ2000State J2000 body ";
      errmsg            += j2000BodyName + " not yet set for ";
      errmsg            += instanceName;
      errmsg            += ".  Possible circular dependency in Spacecraft and ";
      errmsg            += "Coordinate System configuration.\n";
      throw SpaceObjectException(errmsg);
   }
         
   Real *st = state.GetState(); //ps.GetState();

   #ifdef DEBUG_J2000_STATE
      MessageInterface::ShowMessage("   %s Object state: [%lf %lf %lf %lf %lf "
            "%lf]\n", instanceName.c_str(), st[0], st[1], st[2], st[3], st[4],
            st[5]);
      MessageInterface::ShowMessage("   Accessing J2000 body state for %s\n",
         j2000Body->GetName().c_str());
   #endif
   Rvector6 bodyState = j2000Body->GetMJ2000State(atTime);

   #ifdef DEBUG_J2000_STATE
      MessageInterface::ShowMessage("   MJ2000: [%lf %lf %lf %lf %lf %lf]\n",
         bodyState[0], bodyState[1], bodyState[2], bodyState[3], bodyState[4], 
         bodyState[5]);
   #endif

   Rvector6 j2kState;
   
   j2kState[0] = st[0] - bodyState[0];
   j2kState[1] = st[1] - bodyState[1];
   j2kState[2] = st[2] - bodyState[2];

   j2kState[3] = st[3] - bodyState[3];
   j2kState[4] = st[4] - bodyState[4];
   j2kState[5] = st[5] - bodyState[5];

   #ifdef DEBUG_J2000_STATE
      MessageInterface::ShowMessage("   J2K state: [%lf %lf %lf %lf %lf %lf]\n",
            j2kState[0], j2kState[1], j2kState[2], j2kState[3], j2kState[4],
            j2kState[5]);
   #endif

   return j2kState;
}



//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000Acceleration(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Access the MJ2000 acceleration for this SpaceObject.
 *
 * @param <atTime> Epoch for the state data.
 *
 * @return The Cartesian MJ2000 acceleration.
 */
 //------------------------------------------------------------------------------
const Rvector3 SpaceObject::GetMJ2000Acceleration(const A1Mjd &atTime)
{
#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage(
      "SpaceObject::GetMJ2000Acceleration entered; epoch is %lf\n", atTime.Get());
#endif
   if (j2000Body == NULL)
   {
      std::string errmsg = "SpaceObject::GetMJ2000Acceleration J2000 body ";
      errmsg += j2000BodyName + " not yet set for ";
      errmsg += instanceName;
      errmsg += ".  Possible circular dependency in Spacecraft and ";
      errmsg += "Coordinate System configuration.\n";
      throw SpaceObjectException(errmsg);
   }

   // Acceleration presented in ForceModel contral body coordinate system 
   Rvector3 accel = acceleration;

#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage("   %s Object acceleration: [%lf %lf %lf]\n", 
      instanceName.c_str(), accel[0], accel[1], accel[2]);
   MessageInterface::ShowMessage("   Accessing J2000 body acceleration for %s\n",
      j2000Body->GetName().c_str());
#endif
   Rvector3 bodyAcceleration = j2000Body->GetMJ2000Acceleration(atTime);

#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage("   MJ2000 body acceleration: [%lf %lf %lf]\n",
      bodyAcceleration[0], bodyAcceleration[1], bodyAcceleration[2]);
#endif
   
   // Acceleration presented in j2kbody MJ2000Eq coordinate system
   Rvector3 j2kAcceleration;

   j2kAcceleration[0] = accel[0] - bodyAcceleration[0];
   j2kAcceleration[1] = accel[1] - bodyAcceleration[1];
   j2kAcceleration[2] = accel[2] - bodyAcceleration[2];

#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage("   J2K acceleration: [%lf %lf %lf]\n",
      j2kAcceleration[0], j2kAcceleration[1], j2kAcceleration[2]);
#endif

   return j2kAcceleration;
}



//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000Acceleration(const GmatTime &atTime)
//------------------------------------------------------------------------------
/**
 * Access the MJ2000 acceleration for this SpaceObject.
 *
 * @param <atTime> Epoch for the state data.
 *
 * @return The Cartesian MJ2000 acceleration.
 */
 //------------------------------------------------------------------------------
const Rvector3 SpaceObject::GetMJ2000Acceleration(const GmatTime &atTime)
{
#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage(
      "SpaceObject::GetMJ2000Acceleration entered; epoch is %s\n", atTime.ToString().c_str());
#endif
   if (j2000Body == NULL)
   {
      std::string errmsg = "SpaceObject::GetMJ2000Acceleration J2000 body ";
      errmsg += j2000BodyName + " not yet set for ";
      errmsg += instanceName;
      errmsg += ".  Possible circular dependency in Spacecraft and ";
      errmsg += "Coordinate System configuration.\n";
      throw SpaceObjectException(errmsg);
   }

   // Acceleration presented in ForceModel contral body coordinate system 
   Rvector3 accel = acceleration;

#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage("   %s Object acceleration: [%lf %lf %lf]\n",
      instanceName.c_str(), accel[0], accel[1], accel[2]);
   MessageInterface::ShowMessage("   Accessing J2000 body acceleration for %s\n",
      j2000Body->GetName().c_str());
#endif
   Rvector3 bodyAcceleration = j2000Body->GetMJ2000Acceleration(atTime);

#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage("   MJ2000 body acceleration: [%lf %lf %lf]\n",
      bodyAcceleration[0], bodyAcceleration[1], bodyAcceleration[2]);
#endif

   // Acceleration presented in j2kbody MJ2000Eq coordinate system
   Rvector3 j2kAcceleration;

   j2kAcceleration[0] = accel[0] - bodyAcceleration[0];
   j2kAcceleration[1] = accel[1] - bodyAcceleration[1];
   j2kAcceleration[2] = accel[2] - bodyAcceleration[2];

#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage("   J2K acceleration: [%lf %lf %lf]\n",
      j2kAcceleration[0], j2kAcceleration[1], j2kAcceleration[2]);
#endif

   return j2kAcceleration;
}


//------------------------------------------------------------------------------
// const Rvector6 GetMJ2000State(const GmatTime &atTime)
//------------------------------------------------------------------------------
/**
 * Access the MJ2000 state for this SpaceObject.
 *
 * @param <atTime>    High precision epoch for the state data.
 *
 * @return            The Cartesian MJ2000 state.
 *
 * @todo Determine if this calculation should ber moved into the derived
 *       classes.
 */
 //------------------------------------------------------------------------------
const Rvector6 SpaceObject::GetMJ2000State(const GmatTime &atTime)
{
#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage(
      "SpaceObject::GetMJ2000State entered; epoch is %lf\n", GmatTime(atTime).GetMjd());
#endif
   if (j2000Body == NULL)
   {
      std::string errmsg = "SpaceObject::GetMJ2000State J2000 body ";
      errmsg += j2000BodyName + " not yet set for ";
      errmsg += instanceName;
      errmsg += ".  Possible circular dependency in Spacecraft and ";
      errmsg += "Coordinate System configuration.\n";
      throw SpaceObjectException(errmsg);
   }

   Real *st = state.GetState(); //ps.GetState();

#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage("   %s Object state: [%lf %lf %lf %lf %lf "
      "%lf]\n", instanceName.c_str(), st[0], st[1], st[2], st[3], st[4],
      st[5]);
   MessageInterface::ShowMessage("   Accessing J2000 body state for %s\n",
      j2000Body->GetName().c_str());
#endif
   Rvector6 bodyState = j2000Body->GetMJ2000State(atTime);

#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage("   MJ2000: [%lf %lf %lf %lf %lf %lf]\n",
      bodyState[0], bodyState[1], bodyState[2], bodyState[3], bodyState[4],
      bodyState[5]);
#endif

   Rvector6 j2kState;

   j2kState[0] = st[0] - bodyState[0];
   j2kState[1] = st[1] - bodyState[1];
   j2kState[2] = st[2] - bodyState[2];

   j2kState[3] = st[3] - bodyState[3];
   j2kState[4] = st[4] - bodyState[4];
   j2kState[5] = st[5] - bodyState[5];

#ifdef DEBUG_J2000_STATE
   MessageInterface::ShowMessage("   J2K state: [%lf %lf %lf %lf %lf %lf]\n",
      j2kState[0], j2kState[1], j2kState[2], j2kState[3], j2kState[4],
      j2kState[5]);
#endif

   return j2kState;
}


//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000Position(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Access the MJ2000 position for this SpaceObject.
 *
 * @param <atTime> Epoch for the state data.
 *
 * @return The Cartesian MJ2000 position.
 *
 * @todo Implement GetMJ2000Position in the derived classes, and remove this
 *       implementation.
 */
//------------------------------------------------------------------------------
const Rvector3 SpaceObject::GetMJ2000Position(const A1Mjd &atTime)
{
   const Rvector6 rv6 = GetMJ2000State(atTime);
   return (rv6.GetR()); 
}


const Rvector3 SpaceObject::GetMJ2000Position(const GmatTime &atTime)
{
   const Rvector6 rv6 = GetMJ2000State(atTime);
   return (rv6.GetR());
}


//------------------------------------------------------------------------------
// const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime)
//------------------------------------------------------------------------------
/**
 * Access the MJ2000 velocity for this SpaceObject.
 *
 * @param <atTime> Epoch for the state data.
 *
 * @return The Cartesian MJ2000 velocity.
 *
 * @todo Implement GetMJ2000Velocity in the derived classes, and remove this
 *       implementation.
 */
//------------------------------------------------------------------------------
const Rvector3 SpaceObject::GetMJ2000Velocity(const A1Mjd &atTime)
{
   const Rvector6 rv6 = GetMJ2000State(atTime);
   return (rv6.GetV());
}


const Rvector3 SpaceObject::GetMJ2000Velocity(const GmatTime &atTime)
{
   const Rvector6 rv6 = GetMJ2000State(atTime);
   return (rv6.GetV());
}


////------------------------------------------------------------------------------
////  Integer  GetParameterID(const std::string &str) const
////------------------------------------------------------------------------------
///**
// * This method returns the parameter ID, given the input parameter string.
// *
// * @param <str> string for the requested parameter.
// *
// * @return ID for the requested parameter.
// */
////------------------------------------------------------------------------------
//Integer SpaceObject::GetParameterID(const std::string &str) const
//{
//   for (Integer i = SpacePointParamCount; i < SpaceObjectParamCount; i++)
//   {
//      if (str == PARAMETER_TEXT[i - SpacePointParamCount])
//         return i;
//   }
//
//   return SpacePoint::GetParameterID(str);
//}


////------------------------------------------------------------------------------
//// std::string GetParameterText(const Integer id) const
////------------------------------------------------------------------------------
///**
// * This method returns the parameter text, given the input parameter ID.
// *
// * @param <id> Id for the requested parameter text.
// *
// * @return parameter text for the requested parameter.
// */
////------------------------------------------------------------------------------
//std::string SpaceObject::GetParameterText(const Integer id) const
//{
//   if (id >= SpacePointParamCount && id < SpaceObjectParamCount)
//      return PARAMETER_TEXT[id - SpacePointParamCount];
//   return SpacePoint::GetParameterText(id);
//}
//
//
////------------------------------------------------------------------------------
////  Gmat::ParameterType  GetParameterType(const Integer id) const
////------------------------------------------------------------------------------
///**
// * This method returns the parameter type, given the input parameter ID.
// *
// * @param <id> ID for the requested parameter.
// *
// * @return parameter type of the requested parameter.
// */
////------------------------------------------------------------------------------
//Gmat::ParameterType SpaceObject::GetParameterType(const Integer id) const
//{
//   if (id >= SpacePointParamCount && id < SpaceObjectParamCount)
//      return PARAMETER_TYPE[id - SpacePointParamCount];
//
//   return SpacePoint::GetParameterType(id);
//}
//
//
////------------------------------------------------------------------------------
////  std::string  GetParameterTypeString(const Integer id) const
////------------------------------------------------------------------------------
///**
// * This method returns the parameter type string, given the input parameter ID.
// *
// * @param <id> ID for the requested parameter.
// *
// * @return parameter type string of the requested parameter.
// */
////------------------------------------------------------------------------------
//std::string SpaceObject::GetParameterTypeString(const Integer id) const
//{
//   return SpacePoint::PARAM_TYPE_STRING[GetParameterType(id)];
//}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real SpaceObject::GetRealParameter(const Integer id) const
{
   if (id == EPOCH_PARAM)
      return state.GetEpoch();
   return SpacePoint::GetRealParameter(id);
}


GmatTime SpaceObject::GetGmatTimeParameter(const Integer id) const
{
   if (id == EPOCH_PARAM)
      return state.GetEpochGT();
   return SpacePoint::GetGmatTimeParameter(id);
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <label> String description for the requested parameter value.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real SpaceObject::GetRealParameter(const std::string &label) const
{
   if (label == "A1Epoch")
      return state.GetEpoch();
   return GetRealParameter(GetParameterID(label));
}

GmatTime SpaceObject::GetGmatTimeParameter(const std::string &label) const
{
   if (label == "A1Epoch")
      return state.GetEpochGT();
   return GetGmatTimeParameter(GetParameterID(label));
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
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real SpaceObject::SetRealParameter(const Integer id, const Real value)
{
   if (id == EPOCH_PARAM)
      return state.SetEpoch(value);
   return SpacePoint::SetRealParameter(id, value);
}


GmatTime SpaceObject::SetGmatTimeParameter(const Integer id, const GmatTime value)
{
   if (id == EPOCH_PARAM)
   {
      state.SetEpoch(GmatTime(value).GetMjd());
      return state.SetEpochGT(value);
   }

   return SpacePoint::SetGmatTimeParameter(id, value);
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <label> String description for the parameter value.
 * @param <value> value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real SpaceObject::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


GmatTime SpaceObject::SetGmatTimeParameter(const std::string &label, const GmatTime value)
{
   return SetGmatTimeParameter(GetParameterID(label), value);
}


Real SpaceObject::GetRealParameter(const Integer id, const Integer row,
                                   const Integer col) const
{
   return SpacePoint::GetRealParameter(id, row, col);
}

Real SpaceObject::GetRealParameter(const std::string &label, 
                                      const Integer row, 
                                      const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}

Real SpaceObject::SetRealParameter(const Integer id, const Real value,
                                      const Integer row, const Integer col)
{
   return SpacePoint::SetRealParameter(id, value, row, col);
}

Real SpaceObject::SetRealParameter(const std::string &label,
                                      const Real value, const Integer row,
                                      const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}

//---------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value, Integer index)
//---------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 * @param index Index for parameters in arrays.  Use -1 or the index free
 *              version to add the value to the end of the array.
 *
 * @return the parameter value at the end of this call, or
 *         REAL_PARAMETER_UNDEFINED if the parameter id is invalid or the
 *         parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real SpaceObject::SetRealParameter(const Integer id, const Real value,
                                   const Integer index)
{
    return SpacePoint::SetRealParameter(id, value, index);
}

//------------------------------------------------------------------------------
// void ClearLastStopTriggered()
//------------------------------------------------------------------------------
/*
 * Clears the names of the last stopping conditions that triggered a stop.
 */
//------------------------------------------------------------------------------
void SpaceObject::ClearLastStopTriggered()
{
   lastStopTriggered.clear();

   #ifdef DEBUG_STOPCONDITION_TRACKING
      MessageInterface::ShowMessage("Cleared stop identifier from \"%s\"\n", 
         instanceName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void SetLastStopTriggered(const std::string &stopCondName)
//------------------------------------------------------------------------------
/*
 * Adds name of a triggered stopping condition to the list of stops triggered.
 *
 * @param  stopCondName  The name of the triggering stopping condition.
 */
//------------------------------------------------------------------------------
void SpaceObject::SetLastStopTriggered(const std::string &stopCondName)
{
   lastStopTriggered.push_back(stopCondName);
   
   #ifdef DEBUG_STOPCONDITION_TRACKING
      MessageInterface::ShowMessage("Set stop identifier on \"%s\" to \"%s\"\n", 
         instanceName.c_str(), stopCondName.c_str());
   #endif
}

//------------------------------------------------------------------------------
// const std::string SpaceObject::GetLastStopTriggered()
//------------------------------------------------------------------------------
/**
 * This method returns the first entry in the list of triggered stopping
 * conditions, for use in debugging
 *
 * @return The name of the triggered stop
 */
//------------------------------------------------------------------------------
const std::string SpaceObject::GetLastStopTriggered()
{
   if (lastStopTriggered.size() > 0)
      return lastStopTriggered[0];
   return "";
}

//------------------------------------------------------------------------------
// bool WasLastStopTriggered(const std::string &stopCondName)
//------------------------------------------------------------------------------
/*
 * Compares the name of the last stopping condition that triggered with the 
 * input stopping condition name.
 *
 * @param  stopCondName  The name of the stopping condition being compared.
 * 
 * @return true if the names match, false otherwise.
 */
//------------------------------------------------------------------------------
bool SpaceObject::WasLastStopTriggered(const std::string &stopCondName)
{
   #ifdef DEBUG_STOPCONDITION_TRACKING
      MessageInterface::ShowMessage(
         "Checking to see if triggered stop \"%s\" on \"%s\" is in the last "
         "stop triggered list\n", stopCondName.c_str(),
         instanceName.c_str());
   #endif

   if (find(lastStopTriggered.begin(), lastStopTriggered.end(), stopCondName) != 
         lastStopTriggered.end())
      return true;
   return false;
}


//------------------------------------------------------------------------------
// bool HasEphemPropagated()
//------------------------------------------------------------------------------
/**
 * Queries to see if the object has been propagated using an EphemerisPropagator
 *
 * This method is accessed from EphemerisPropagators so that epoch/state data
 * can be managed by that class of propagators.
 *
 * @return true if an ephem propagator has been used, false if not
 */
//------------------------------------------------------------------------------
bool SpaceObject::HasEphemPropagated()
{
   return hasEphemPropagated;
}


//------------------------------------------------------------------------------
// void HasEphemPropagated(bool tf)
//------------------------------------------------------------------------------
/**
 * Tells the object it has been propagated using an EphemerisPropagator.
 *
 * This method is accessed from EphemerisPropagators so that epoch/state data
 * can be managed by that class of propagators.
 *
 * @param tf true if an ephem propagator has been used, false if not
 */
//------------------------------------------------------------------------------
void SpaceObject::HasEphemPropagated(bool tf)
{
   hasEphemPropagated = tf;
}

// Start on a fix for bug 648; these methods are not currently used, but are in 
// place for use when the single step publishing issues are ready to be worked.
void SpaceObject::HasPublished(bool tf)
{
   hasPublished = tf;
}

bool SpaceObject::HasPublished()
{
   return hasPublished;
}

bool SpaceObject::SetPrecisionTimeFlag(bool onOff)
{
   state.SetPrecisionTimeFlag(onOff);
   return SpacePoint::SetPrecisionTimeFlag(onOff);
}


//------------------------------------------------------------------------------
// bool isFromEphemeris()
//------------------------------------------------------------------------------
/**
 * This method checks to see if the spacecraft has an attached ephemeris file.
 */
 //------------------------------------------------------------------------------
bool SpaceObject::HasEphemerisFile()
{
	return SpacePoint::HasEphemerisFile();
}