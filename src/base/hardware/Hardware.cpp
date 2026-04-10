//$Id$
//------------------------------------------------------------------------------
//                               Hardware
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
// Author: Darrel J. Conway
// Created: 2004/11/08
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class implementation for the Hardware base class.
 */
//------------------------------------------------------------------------------
#include "MessageInterface.hpp"

#include "Hardware.hpp"
#include "HardwareException.hpp"
#include "GmatConstants.hpp"
#include "StringUtil.hpp"
#include <string.h>

//#define DEBUG_HW_SET
//#define DEBUG_HW_SET_REAL_PARAM
//#define DEBUG_HW_INITIALIZE
//#define DEBUG_HW_SET_RMATRIX_PARAM


//---------------------------------
// static data
//---------------------------------

/// Labels used for the hardware element parameters.
const std::string
Hardware::PARAMETER_TEXT[HardwareParamCount - GmatBaseParamCount] =
{
//   "FieldOfView",
   "Coordinates",
   "RotationOption",
   "DirectionX",
   "DirectionY",
   "DirectionZ",
   "SecondDirectionX",
   "SecondDirectionY",
   "SecondDirectionZ",

   "HWOriginInBCSX",
   "HWOriginInBCSY",
   "HWOriginInBCSZ",

   "RotationMatrix",
   "R_SB11",
   "R_SB12",
   "R_SB13",
   "R_SB21",
   "R_SB22",
   "R_SB23",
   "R_SB31",
   "R_SB32",
   "R_SB33",
//   "MaskConeAngles",
//   "MaskClockAngles",
};


/// Types of the parameters used by all hardware elements.
const Gmat::ParameterType
Hardware::PARAMETER_TYPE[HardwareParamCount - GmatBaseParamCount] =
{
//   Gmat::OBJECT_TYPE,    // FieldOfView
   Gmat::STRING_TYPE,    // Coordinates
   Gmat::STRING_TYPE,    // RotationOption
   Gmat::REAL_TYPE,      // DirectionX
   Gmat::REAL_TYPE,      // DirectionY
   Gmat::REAL_TYPE,      // DirectionZ

   Gmat::REAL_TYPE,      // SecondDirectionX
   Gmat::REAL_TYPE,      // SecondDirectionY
   Gmat::REAL_TYPE,      // SecondDirectionZ

   Gmat::REAL_TYPE,      // HWOriginInBCSX
   Gmat::REAL_TYPE,      // HWOriginInBCSY
   Gmat::REAL_TYPE,      // HWOriginInBCSZ

   Gmat::RMATRIX_TYPE,   // RotationMatrix

   Gmat::REAL_TYPE,      // R_SB11
   Gmat::REAL_TYPE,      // R_SB12
   Gmat::REAL_TYPE,      // R_SB13
   Gmat::REAL_TYPE,      // R_SB21
   Gmat::REAL_TYPE,      // R_SB22
   Gmat::REAL_TYPE,      // R_SB23
   Gmat::REAL_TYPE,      // R_SB31
   Gmat::REAL_TYPE,      // R_SB32
   Gmat::REAL_TYPE,      // R_SB33
};



//------------------------------------------------------------------------------
//  Hardware(UnsignedInt typeId, const std::string &typeStr,
//           const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Hardware base class constructor.
 *
 * @param typeId Core object type for the component.
 * @param typeStr String label for the actual object type for the component.
 * @param nomme Name of the component.
 */
//------------------------------------------------------------------------------
Hardware::Hardware(UnsignedInt typeId, const std::string &typeStr,
                   const std::string &nomme) :
   GmatBase     (typeId, typeStr, nomme),
//   fovIsModeled (false),
//   fov          (NULL),
//   fovName      (""),
   coordinates   ("SpacecraftOrigin"),
   rotationInput ("Construct"),
   R_SB          (Rmatrix33(true)) // sets value to identity
   // ultimately Initialize() will construct from 2 vectors, e.g, like VNB frame
{
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage("Entering Hardware constructor\n");
      MessageInterface::ShowMessage("object name='%s', pointer=<%p>\n",
                                 GetName().c_str(),this);
   #endif
   objectTypes.push_back(Gmat::HARDWARE);
   objectTypeNames.push_back("Hardware");
   parameterCount = HardwareParamCount;

   // need to add a "needs initialization flag" set false at construction
   // set true if parameter changes for direction or secondDirection, and tested
   // when matrix is requested or used internally to Hardware class.
   direction[0] = 1.0;
   direction[1] = 0.0;
   direction[2] = 0.0;

   secondDirection[0] = 0.0;
   secondDirection[1] = 1.0;
   secondDirection[2] = 0.0;

   // default offset of zero means hardware frame shares origin with
   // s/c body frame. *** offset computation not yet implemented ***
   location[0] = 0.0;
   location[1] = 0.0;
   location[2] = 0.0;

   for (Integer i = GmatBaseParamCount; i < HardwareParamCount; ++i)
         parameterWriteOrder.push_back(i);

   coordinateTypes.clear();
   coordinateTypes.push_back("SpacecraftOrigin");
   coordinateTypes.push_back("FixedGrid");

   rotationInputTypes.clear();
   rotationInputTypes.push_back("DCM");
   //rotationInputTypes.push_back("Euler");
   rotationInputTypes.push_back("Construct");

   #ifdef DEBUG_HW_SET
   // print out initial conditions, first body to sensor DCM
   MessageInterface::ShowMessage("Body to sensor DCM in HW constructor =\n");
   for (int i=0; i < 3; i++)
   {
      MessageInterface::ShowMessage(R_SB.ToRowString(i,3,6) );
      MessageInterface::ShowMessage("\n");
   }

   // and then direction & offset vectors
   Rvector3 d1(direction[0], direction[1],direction[2]);
   Rvector3 d2(secondDirection[0],
               secondDirection[1],
               secondDirection[2]);
   Rvector3 offset(location[0], location[1], location[2]);
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("input vectors in HW constructor:\n");
   MessageInterface::ShowMessage("Primary Direction =\n");
   MessageInterface::ShowMessage(d1.ToString(3));
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("Secondary Direction =\n");
   MessageInterface::ShowMessage(d2.ToString(3));
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("HW frame origin offset =\n");
   MessageInterface::ShowMessage(offset.ToString(3));
   MessageInterface::ShowMessage("\n\n");
   MessageInterface::ShowMessage ("Exiting Hardware Constructor\n\n");
  #endif
}


//------------------------------------------------------------------------------
//  ~Hardware()
//------------------------------------------------------------------------------
/**
 * Hardware base class destructor.
 */
//------------------------------------------------------------------------------
Hardware::~Hardware()
{}


//------------------------------------------------------------------------------
//  Hardware(const Hardware& hw)
//------------------------------------------------------------------------------
/**
 * Hardware copy constructor.
 *
 * @param hw The object being copied.
 */
//------------------------------------------------------------------------------
Hardware::Hardware(const Hardware& hw) :
   GmatBase(hw),
   coordinates   (hw.coordinates),
   rotationInput (hw.rotationInput)
//   fov (NULL)
{
#ifdef DEBUG_HW_SET
   MessageInterface::ShowMessage("Entering Hardware copy constructor\n");
   MessageInterface::ShowMessage("object name='%s', pointer=<%p>\n",
                                 GetName().c_str(),this);
#endif
   direction[0] = hw.direction[0];
   direction[1] = hw.direction[1];
   direction[2] = hw.direction[2];

   secondDirection[0] = hw.secondDirection[0];
   secondDirection[1] = hw.secondDirection[1];
   secondDirection[2] = hw.secondDirection[2];

   location[0] = hw.location[0];
   location[1] = hw.location[1];
   location[2] = hw.location[2];

   //fov          = (FieldOfView *) hw.fov->Clone();

   R_SB         = hw.R_SB;
//   fovName      = hw.fovName;
//   fovIsModeled = hw.fovIsModeled;

   for (Integer i = GmatBaseParamCount; i < HardwareParamCount; ++i)
         parameterWriteOrder.push_back(i);

   coordinateTypes.clear();
   for (Integer ii = 0; ii < hw.coordinateTypes.size(); ++ii)
      coordinateTypes.push_back(hw.coordinateTypes.at(ii));

   rotationInputTypes.clear();
   for (Integer ii = 0; ii < hw.rotationInputTypes.size(); ++ii)
      rotationInputTypes.push_back(hw.rotationInputTypes.at(ii));

#ifdef DEBUG_HW_SET
   MessageInterface::ShowMessage ("Exiting Hardware copy constructor\n\n");
#endif
}


//------------------------------------------------------------------------------
//  Hardware& operator=(const Hardware& hw)
//------------------------------------------------------------------------------
/**
 * Hardware assignment operator.
 *
 * @param hw The object being copied.
 *
 * @return this object, with parameters set to the input object's parameters.
 */
//------------------------------------------------------------------------------
Hardware& Hardware::operator=(const Hardware& hw)
{
   if (&hw == this)
      return *this;

   GmatBase::operator=(hw);

   coordinates   = hw.coordinates;
   rotationInput = hw.rotationInput;

   direction[0] = hw.direction[0];
   direction[1] = hw.direction[1];
   direction[2] = hw.direction[2];

   secondDirection[0] = hw.secondDirection[0];
   secondDirection[1] = hw.secondDirection[1];
   secondDirection[2] = hw.secondDirection[2];

   location[0] = hw.location[0];
   location[1] = hw.location[1];
   location[2] = hw.location[2];

//   fov          = hw.fov ? ((FieldOfView *) hw.fov->Clone()) : nullptr;
   R_SB         = hw.R_SB;
//   fovName      = hw.fovName;
//   fovIsModeled = hw.fovIsModeled;

   coordinateTypes.clear();
   for (Integer ii = 0; ii < hw.coordinateTypes.size(); ++ii)
      coordinateTypes.push_back(hw.coordinateTypes.at(ii));

   rotationInputTypes.clear();
   for (Integer ii = 0; ii < hw.rotationInputTypes.size(); ++ii)
      rotationInputTypes.push_back(hw.rotationInputTypes.at(ii));

   return *this;
}

// Field of view operations

//------------------------------------------------------------------------------
//  bool HasFOV()
//------------------------------------------------------------------------------
/**
 * This method determines if FOV is being modeled
 *
 * @return boolean is true if this hardware object includes a field of view
 *
 */
//------------------------------------------------------------------------------
bool Hardware::HasFOV()
{
   return false; //(!(fov == NULL));
}

//------------------------------------------------------------------------------
//  Rmatrix33& GetRotationMatrix() const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for the DCM representing s/c body to hardware frame
 * rotation.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Rmatrix33& Hardware::GetRotationMatrix()
{
   return R_SB;
}

////------------------------------------------------------------------------------
////  bool CheckTargetVisibility(const Rvector3 &target)
////------------------------------------------------------------------------------
///**
// * This method determines if FOV is being modeled
// *
// * @param <target> is a unit vector in the spacecraft body frame
// *
// * @return boolean is true if the target vector points inside the FOV.
// *
// */
////------------------------------------------------------------------------------
//bool Hardware::CheckTargetVisibility(const Rvector3 &target)
//{
//   // may add use of location offset later; for now just rotation
//   return fov->CheckTargetVisibility(R_SB*target);
//
//}
//
////------------------------------------------------------------------------------
////  Rvector GetMaskConeAngles() const
////------------------------------------------------------------------------------
///**
// * This method returns cone angle(s)  for field of view
// * Note: see specifications for how each FOV class uses the vectors
// *
// * @return Rvector -- returns cone angle(s) for field of view
// *
// */
////------------------------------------------------------------------------------
//
//Rvector Hardware::GetMaskConeAngles() const
//{
//   static Rvector result;
//   if (fov)
//      return fov->GetMaskConeAngles();
//   else
//   {
//      // @todo write message
//      result.SetSize(0);
//      return result;
//   }
//}
//
////------------------------------------------------------------------------------
////  Rvector GetMaskClockAngles() const
////------------------------------------------------------------------------------
///**
// * This method returns clock angle(s)  for field of view. This vector is empty
// * if the FOV is object in class ConicalFOV.
// * Note: see specifications for how each FOV class uses the vectors
// *
// * @return Rvector -- returns clock angle(s for field of view
// *
// */
////------------------------------------------------------------------------------
//
//Rvector Hardware::GetMaskClockAngles() const
//{
//   static Rvector result;
//   if (fov)
//      return fov->GetMaskClockAngles();
//   else
//   {
//      // @todo write message
//      result.SetSize(0);
//      return result;
//   }
//}

//-------------------------------------------------------
// needs header
Rvector3 Hardware::GetLocation() const
{
   return Rvector3(location[0],
                   location[1],
                   location[2]);
}

// needs header
Rvector3 Hardware::GetDirection() const
{
   return Rvector3(direction[0],
                   direction[1],
                   direction[2]);
}

// needs header
Rvector3 Hardware::GetSecondDirection() const
{
   return Rvector3(secondDirection[0],
                   secondDirection[1],
                   secondDirection[2]);
}

//--------------------------------------------------------
// Parameter operations
//--------------------------------------------------------

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
std::string Hardware::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < HardwareParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
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
Integer Hardware::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < HardwareParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
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
Gmat::ParameterType Hardware::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < HardwareParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
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
std::string Hardware::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false if not,
 */
//---------------------------------------------------------------------------
bool Hardware::IsParameterReadOnly(const Integer id) const
{
//   if (id == MASK_CONE_ANGLES)
//   {
//      return true;
//   }
//   if (id == MASK_CLOCK_ANGLES)
//   {
//      return true;
//   }
   if (/*(id == FOV_MODEL)        ||*/
       (id == ROTATION_INPUT_TYPE)|| (id == ROTATION_MATRIX)    ||
       (id == COORDINATES)        || (id == DIRECTION_X)        ||
       (id == DIRECTION_Y)        || (id == DIRECTION_Z)        ||
       (id == SECOND_DIRECTION_X) || (id == SECOND_DIRECTION_Y) ||
       (id == SECOND_DIRECTION_Z) || (id == HW_ORIGIN_BCS_X)    ||
       (id == HW_ORIGIN_BCS_Y)    || (id == HW_ORIGIN_BCS_Z)    ||
       (id >= R_SB11 && id <= R_SB33))
      return true;

   return GmatBase::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool Hardware::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//---------------------------------------------------------------------------
// bool IsParameterVisible(const Integer id) const
//---------------------------------------------------------------------------
bool Hardware::IsParameterVisible(const Integer id) const
{
   if (/*(id == FOV_MODEL)          ||*/
       (id == COORDINATES)        || (id == DIRECTION_X)        ||
       (id == DIRECTION_Y)        || (id == DIRECTION_Z)        ||
       (id == SECOND_DIRECTION_X) || (id == SECOND_DIRECTION_Y) ||
       (id == SECOND_DIRECTION_Z) || (id == HW_ORIGIN_BCS_X)    ||
       (id == HW_ORIGIN_BCS_Y)    || (id == HW_ORIGIN_BCS_Z))
      return false;

   return GmatBase::IsParameterVisible(id);
}

// string parameter operations

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Hardware::GetStringParameter(const Integer id) const
{
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage
      ("Hardware::GetStringParameter() entered for parameter '%s', id=%d\n",
      GetParameterText(id).c_str(), id);
   #endif
   std::string name;
   switch (id)
   {
      case COORDINATES:
         return coordinates;

//      case FOV_MODEL:
//         if (fov) // != NULL
//            name = fov->GetName();
//         else
//            name = fovName;
//            name = "UndefinedFieldOfView";
//         #ifdef DEBUG_HW_SET
//            MessageInterface::ShowMessage
//               ("Hardware::GetStringParameter() '%s' object name is '%s'\n",
//                GetParameterText(id).c_str(), name.c_str());
//            MessageInterface::ShowMessage(
//               "Exiting Hardware::GetStringParameter()\n\n");
//         #endif
//         return name;
         // break;

      case ROTATION_INPUT_TYPE:
         return rotationInput;

      default:
         #ifdef DEBUG_HW_SET
            MessageInterface::ShowMessage(
               "Parameter is not for HW class, call GmatBase");
            MessageInterface::ShowMessage(
               "Exiting Hardware::GetStringParameter()\n\n");
         #endif
         return GmatBase::GetStringParameter(id);
   }
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage
         ("Hardware::GetStringParameter() '%s' object name is '%s'\n",
          GetParameterText(id).c_str(), name.c_str());
   MessageInterface::ShowMessage("Code should not get here\n");
   MessageInterface::ShowMessage("Exiting Hardware::GetStringParameter()\n\n");
   #endif
   return name;
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Hardware::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool Hardware::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage
         ("Hardware::SetStringParameter() '%s', id=%d, value='%s'\n",
           GetName().c_str(), id, value.c_str());
   #endif

   switch (id)
   {
      case COORDINATES:
         if (find(coordinateTypes.begin(), coordinateTypes.end(), value) != coordinateTypes.end())
            coordinates = value;
         else
         {
            std::string errmsg = "*** Error *** The value" +
               value + "for field Coordinates on "
               "object \"" + instanceName + "\" is not an allowed value. "
               "Allowed values are [";
            for (Integer ii = 0; ii < coordinateTypes.size(); ++ii)
               errmsg += coordinateTypes.at(ii) + " ";
            errmsg.resize(errmsg.size() - 1);
            errmsg += "]";
            throw HardwareException(errmsg);
         }

//      case FOV_MODEL:
//      {
//         fovIsModeled = true;
//         fovName = value;
//         #ifdef DEBUG_HW_SET
//            MessageInterface::ShowMessage
//               ("Hardware::SetStringParameter() '%s' object name is '%s'\n",
//                GetParameterText(id).c_str(), fovName.c_str());
//            MessageInterface::ShowMessage(
//               "Exiting Hardware::SetStringParameter()\n\n");
//         #endif
//         return true;
//      }

      case ROTATION_INPUT_TYPE:
         #ifdef DEBUG_HW_SET
            MessageInterface::ShowMessage
                   ("Beginning of ROTATION_INPUT_TYPE 'value='%s'\n",
                    value.c_str());
         #endif
         if (find(rotationInputTypes.begin(),
                  rotationInputTypes.end(), value) != rotationInputTypes.end())
            rotationInput = value;
         else
         {
            std::string errmsg = "\n\nThe value \"" +
               value + "\" for field \"RotationOption\" on "
               "object \"" + instanceName + "\" is not an allowed value. "
               "\n\nAllowed values are [";
            for (Integer ii = 0; ii < rotationInputTypes.size(); ++ii)
               errmsg += rotationInputTypes.at(ii) + " ";
            errmsg.resize(errmsg.size() - 1);
            errmsg += "]\n";
            throw HardwareException(errmsg);
         }
         #ifdef DEBUG_HW_SET
            MessageInterface::ShowMessage
                   ("End of ROTATION_INPUT_TYPE 'value='%s'\n",
                    value.c_str());
         #endif
         break;

      default:
         #ifdef DEBUG_HW_SET
            MessageInterface::ShowMessage(
               "Calling GmatBase fromHardware::SetStringParameter() '%s' ",
               GetParameterText(id).c_str());
            // @TODO: fovName is not declared at this time
            // MessageInterface::ShowMessage("object name is '%s'\n",
            //                              fovName.c_str());
            MessageInterface::ShowMessage(
               "Exiting Hardware::SetStringParameter()\n\n");
         #endif
         return GmatBase::SetStringParameter(id, value);
   }
   return true;
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool Hardware::SetStringParameter(const std::string &label,
                                   const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


// real parameter with id as argument
//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real Hardware::GetRealParameter(const Integer id) const
{
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage
         ("Hardware::GetRealParameter() id name is '%s'\n",
          GetParameterText(id).c_str());
   #endif
   switch (id)
   {
      case DIRECTION_X:
         return direction[0];

      case DIRECTION_Y:
         return direction[1];

      case DIRECTION_Z:
         return direction[2];

      case SECOND_DIRECTION_X:
         return secondDirection[0];

      case SECOND_DIRECTION_Y:
         return secondDirection[1];

      case SECOND_DIRECTION_Z:
         return secondDirection[2];

      case HW_ORIGIN_BCS_X:
         return location[0];

      case HW_ORIGIN_BCS_Y:
         return location[1];

      case HW_ORIGIN_BCS_Z:
         return location[2];

      // @TODO:  Should we use this R_SB below for getting?
      case R_SB11:
         return R_SB(0,0);

      case R_SB12:
         return R_SB(0,1);

      case R_SB13:
         return R_SB(0,2);

      case R_SB21:
         return R_SB(1,0);

      case R_SB22:
         return R_SB(1,1);

      case R_SB23:
         return R_SB(1,2);

      case R_SB31:
         return R_SB(2,0);

      case R_SB32:
         return R_SB(2,1);

      case R_SB33:
         return R_SB(2,2);

      default:
         break;   // Default just drops through
   }
   return GmatBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real Hardware::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_HW_SET_REAL_PARAM
      MessageInterface::ShowMessage(
         "DEBUG: Hardware::SetRealParameter(id = %d (%s), value = %12.10f)\n",
          id, GetParameterText(id).c_str(), value);
   #endif

   if ((id >= R_SB11 && id <= R_SB33) && (value < -1.0 || value > 1.0))
   {
      HardwareException hwe("");
      hwe.SetDetails(errorMessageFormat.c_str(),
                     GmatStringUtil::ToString(value, 16).c_str(),
                     GetParameterText(id).c_str(),
                     "-1.0 <= Real Number <= 1.0");
      throw hwe;
   }

   switch (id)
   {
      case DIRECTION_X:
         return direction[0] = value;

      case DIRECTION_Y:
         return direction[1] = value;

      case DIRECTION_Z:
         //       if (value < 0)
         //       {
         //          throw HardwareException("Z_Direction must be >= 0");
         //       }
         return direction[2] = value;

      case SECOND_DIRECTION_X:
         return secondDirection[0] = value;

      case SECOND_DIRECTION_Y:
         return secondDirection[1] = value;

      case SECOND_DIRECTION_Z:
         //         if (value < 0) {
         //            throw HardwareException("Z_Direction must be >= 0");
         //         }
         return secondDirection[2] = value;

      case HW_ORIGIN_BCS_X:
         return location[0] = value;

      case HW_ORIGIN_BCS_Y:
         return location[1] = value;

      case HW_ORIGIN_BCS_Z:
         //         if (value < 0) {
         //            throw HardwareException("Z_Direction must be >= 0");
         //         }
         return location[2] = value;

      case R_SB11:
         return R_SB(0,0) = value;

      case R_SB12:
         return R_SB(0,1) = value;

      case R_SB13:
         return R_SB(0,2) = value;

      case R_SB21:
         return R_SB(1,0) = value;

      case R_SB22:
         return R_SB(1,1) = value;

      case R_SB23:
         return R_SB(1,2) = value;

      case R_SB31:
         return R_SB(2,0) = value;

      case R_SB32:
         return R_SB(2,1) = value;

      case R_SB33:
         return R_SB(2,2) = value;

      default:
         break;   // Default just drops through
   }


   return GmatBase::SetRealParameter(id, value);
}

// real parameter with parameter string label as argument
//------------------------------------------------------------------------------
//  Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real Hardware::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real Hardware::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

///////////////////////////////////////////////////////////////////
// Real parameters with indices, used by subclasses as passthroughs
Real Hardware::GetRealParameter(const Integer id,
                                const Integer index) const
{
//   if (id == MASK_CONE_ANGLES)
//   {
//      Rvector result = GetMaskConeAngles();
//      if ((index < 0) || (index >= result.GetSize()))
//         throw HardwareException("Index out-of-range for MaskConeAngles\n");
//      return result[index];
//   }
//   if (id == MASK_CLOCK_ANGLES)
//   {
//      Rvector result = GetMaskClockAngles();
//      if ((index < 0) || (index >= result.GetSize()))
//         throw HardwareException("Index out-of-range for MaskClockAngles\n");
//      return result[index];
//   }
   return GmatBase::GetRealParameter(id, index);
}

Real Hardware::SetRealParameter(const Integer id, const Real value,
                                const Integer index)
{
   return GmatBase::SetRealParameter(id, value, index);
}

Real Hardware::GetRealParameter(const std::string &label,
                                const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}

Real Hardware::SetRealParameter(const std::string &label,
                                const Real value,
                                const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}

// passthroughs of Rvector parameter calls for subclasses to use
 const Rvector& Hardware::GetRvectorParameter(const Integer id) const
{
//   static Rvector result;
//   #ifdef DEBUG_HW_SET
//      MessageInterface::ShowMessage
//         ("Hardware::GetRvectorParameter() id name is '%s'\n",
//          GetParameterText(id).c_str());
//   #endif
//   if (id == MASK_CONE_ANGLES)  // retrieved from fov; cannot be set
//   {
//      Rvector angles = GetMaskConeAngles();
//      result.SetSize(angles.GetSize());
//      result = angles;
//      return result;
//   }
//   if (id == MASK_CLOCK_ANGLES) // retrieved from fov; cannot be set
//   {
//      Rvector angles = GetMaskClockAngles();
//      result.SetSize(angles.GetSize());
//      result = angles;
//      return result;
//   }
   return GmatBase::GetRvectorParameter(id);
}

const Rvector& Hardware::SetRvectorParameter(const Integer id,
                                             const Rvector &value)
{
   return GmatBase::SetRvectorParameter(id, value);
}

const Rvector& Hardware::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}

const Rvector& Hardware::SetRvectorParameter(const std::string &label,
                                             const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  const Rmatrix&  GetRmatrixParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * This method gets the Rmatrix parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  the Rmatrix parameter.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& Hardware::GetRmatrixParameter(const Integer id) const
{
   if (id == ROTATION_MATRIX)
      return R_SB;

   return GmatBase::GetRmatrixParameter(id);
}

//------------------------------------------------------------------------------
//  const Rmatrix&  GetRmatrixParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * This method gets the Rmatrix parameter value, given the input
 * parameter ID.
 *
 * @param <label> string ID for the requested parameter.
 *
 * @return  the Rmatrix parameter.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& Hardware::GetRmatrixParameter(const std::string &label) const
{
   return GetRmatrixParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  const Rmatrix&  SetRmatrixParameter(const Integer id, const Rmatrix& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rmatrix parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Rmatrix value for the requested parameter.
 *
 * @return  the Rmatrix parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& Hardware::SetRmatrixParameter(const Integer id,
                                             const Rmatrix &value)
{
#ifdef DEBUG_HW_SET_RMATRIX_PARAM
         Integer r, c;
         value.GetSize(r, c);
         MessageInterface::ShowMessage(
            "\nDEBUG: Hardware::SetRmatrixParameter(%s), value(%d, %d)",
            GetParameterText(id).c_str(), r, c);
#endif

   // TODO:  may need to add more later
   if (id == ROTATION_MATRIX)
   {
      if (rotationInput == "DCM")
      {

#ifdef DEBUG_HW_SET_RMATRIX_PARAM
         MessageInterface::ShowMessage(
            "\nDEBUG: Hardware::SetRmatrixParameter, values for DCM:"
            "\n       [ %f  %f %f"
            "\n         %f  %f %f"
            "\n         %f  %f %f ]\n\n",
            value(0,0), value(0,1), value(0,2),
            value(1,0), value(1,1), value(1,2),
            value(2,0), value(2,1), value(2,2));
#endif

         if (value.IsOrthonormal(
                     GmatAttitudeConstants::DCM_ORTHONORMALITY_TOLERANCE))
         {
            R_SB = value;

            return R_SB;
         }
         else
         {
            // TODO: need to throw exception
            std::string errmsg;
            errmsg = "*** WARNING*** Object Hardware's RotationMatrix "
                     "has a non-orthogonal rotation matrix.\n";
            MessageInterface::ShowMessage(errmsg);
            // throw HardwareException(errmsg);

         }
      }
   }

   return GmatBase::SetRmatrixParameter(id,value);

}

//------------------------------------------------------------------------------
//  const Rmatrix&  SetRmatrixParameter(const std::string &label,
//                                      const Rmatrix& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rmatrix parameter value, given the input
 * parameter label.
 *
 * @param <label> string ID for the requested parameter.
 * @param <value> Rmatrix value for the requested parameter.
 *
 * @return  the Rmatrix parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& Hardware::SetRmatrixParameter(const std::string &label,
                                                 const Rmatrix &value)
{
   return SetRmatrixParameter(GetParameterID(label), value);
}


// pass throughs of string parameter calls for subclasses to use

//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Hardware::GetStringParameter(const Integer id,
                                         const Integer index) const
{
   return GmatBase::GetStringParameter(id, index);
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value,
//                          const Integer index)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool Hardware::SetStringParameter(const Integer id, const std::string &value,
                                  const Integer index)
{
   return GmatBase::SetStringParameter(id, value, index);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

//--------------------------------------------------------
// Reference object functions
//--------------------------------------------------------

//---------------------------------------------------------------------------
//  std::string GetRefObjectName(const UnsignedInt type) const
//---------------------------------------------------------------------------
/**
 * Returns the name of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.
 *
 * @return The name of the reference object.
 */
//------------------------------------------------------------------------------
std::string  Hardware::GetRefObjectName(const UnsignedInt type) const
{
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage("Entering Hardware::GetRefObjectName() ");
      MessageInterface::ShowMessage("for object '%s'\n", GetName().c_str());
      if (type == Gmat::FIELD_OF_VIEW)
         MessageInterface::ShowMessage("type == FIELD_OF_FIEW\n");
      if (type == Gmat::CONICAL_FOV)
         MessageInterface::ShowMessage("type == CONICAL_FOV\n");

   #endif
//   if (type == Gmat::FIELD_OF_VIEW)
//   {
//      #ifdef DEBUG_HW_SET
//         MessageInterface::ShowMessage("Exiting Hardware::GetRefObjectName()");
//         MessageInterface::ShowMessage("Ref Object Name = '%s'\n\n",
//                                       fovName.c_str());
//      #endif
//      return fovName;
//   }

   // kick it up the chain for all other types
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage("Exiting Hardware::GetRefObjectName(), ");
      MessageInterface::ShowMessage("Calling GmatBase\n\n");
   #endif
   return GmatBase::GetRefObjectName(type);
}

//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool Hardware::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of reference object types used by this class.
 *
 * @return the list of object types.
 *
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Hardware::GetRefObjectTypeArray()
{
   #ifdef DEBUG_HW_SET
   MessageInterface::ShowMessage("Entering Hardware::GetRefObjectTypeArray");
      MessageInterface::ShowMessage("() for object '%s'\n", GetName().c_str());
   #endif

   // refObjectTypes is defined in GmatBase
   refObjectTypes.clear();
//   if (fovIsModeled)
//      refObjectTypes.push_back(Gmat::FIELD_OF_VIEW);
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage(
      "Exiting Hardware::GetRefObjectTypeArray()\n\n");
   #endif
   return refObjectTypes;
}

//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * This method returns an array with the names of the referenced objects.
 *
 * @return a vector with the names of objects of the requested type.
 */
//------------------------------------------------------------------------------
const StringArray&
Hardware::GetRefObjectNameArray(const UnsignedInt type)
{
#ifdef DEBUG_HW_SET
   MessageInterface::ShowMessage
   ("Hardware::GetRefObjectNameArray():\n");
   MessageInterface::ShowMessage("for object=<%p> '%s' entered, input type='%s'\n",
    this, GetName().c_str(), GmatBase::GetObjectTypeString(type).c_str());
#endif

   static StringArray fullList;  // Maintain scope if the full list is requested
   fullList.clear();

   // If type is UNKNOWN_OBJECT, add fovName anyway.
   if ( (type == Gmat::UNKNOWN_OBJECT) || (type ==Gmat::FIELD_OF_VIEW) )
   {
//      // Put in the FieldOfView object name
//      if (fovIsModeled)
//      {
//         fullList.push_back(fovName);
//         #ifdef DEBUG_HW_SET
//           MessageInterface::ShowMessage("pushed name '%s' onto list\n",
//                                       fovName.c_str());
//         #endif
//      }
      #ifdef DEBUG_HW_SET
         MessageInterface::ShowMessage(
            "Exiting Hardware::GetRefObjectNameArray()\n\n");
      #endif
      return fullList;
   }
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage(
         "Returning GmatBase::GetRefObjectNameArray()\n\n");
   #endif

   return GmatBase::GetRefObjectNameArray(type);
}

//---------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Returns the reference object pointer.  (Derived classes should implement
 * this method as needed.)
 *
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return reference object pointer.
 */
//------------------------------------------------------------------------------
GmatBase*    Hardware::GetRefObject(const UnsignedInt type,
                                    const std::string &name)
{
//   if ((type == Gmat::FIELD_OF_VIEW) && (name == fovName))
//      return (GmatBase*) fov;

   // kick it up the chain for all other types
   return GmatBase::GetRefObject(type, name);
}

//---------------------------------------------------------------------------
//  bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//---------------------------------------------------------------------------
/**
 * Sets the name of the reference object.  (Derived classes should implement
 * this as needed.)
 *
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
//--------------------------------------------------------
bool Hardware::SetRefObjectName(const UnsignedInt type,
                                const std::string &name)
{
//   if (type == Gmat::FIELD_OF_VIEW)
//   {
//      fovName = name;
//      #ifdef DEBUG_HW_SET
//         MessageInterface::ShowMessage("Exiting Hardware::SetRefObjectName(),");
//         MessageInterface::ShowMessage(" fovname = '%s'\n\n");
//      #endif
//      return true;
//   }

   // kick it up the chain for all other types
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage("Exiting Hardware::SetRefObjectName(), ");
      MessageInterface::ShowMessage("type != Field_Of_View, call GmatBase\n\n");
   #endif

   return GmatBase::SetRefObjectName(type,name);
}

//---------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//---------------------------------------------------------------------------
/**
 * Sets the reference object.  (Derived classes should implement
 * this method as needed.)
 *
 * @param <obj> reference object pointer.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 *
 * @return success of the operation.
 */
//--------------------------------------------------------
bool Hardware::SetRefObject(GmatBase *obj, const UnsignedInt type,
                            const std::string &name)
{
#ifdef DEBUG_HW_SET
   MessageInterface::ShowMessage("Entering Hardware::SetRefObject()\n");
   MessageInterface::ShowMessage("(this, input type & name =<%p> '%s' '%d'\n",
                                 this, GetName().c_str(), type);
   MessageInterface::ShowMessage("Ref: ptr,type & name = <%p> '%s' '%s'\n",
                                 obj,
                                 obj->GetTypeName().c_str(),
                                 obj->GetName().c_str());
#endif

   if (obj == NULL)
   {
      #ifdef DEBUG_HW_SET
         MessageInterface::ShowMessage("Reference to NULL, return false");
         MessageInterface::ShowMessage("Hardware::SetRefObject <%p> '%s'\n",
                                    this, GetName().c_str());
         MessageInterface::ShowMessage("Exiting Hardware::SetRefObject()\n\n");
      #endif
      return false;

   }
#ifdef DEBUG_HW_SET
   // @TODO:  fovName is not declared at this time
   // MessageInterface::ShowMessage("fovname = '%s'\n", fovName.c_str());
   MessageInterface::ShowMessage("type    = %d\n",type);
   MessageInterface::ShowMessage("gmat::FIELD_OF_VIEW = '%d'\n",
                                 Gmat::FIELD_OF_VIEW);
#endif

//   if ( (obj->IsOfType(Gmat::FIELD_OF_VIEW)) &&
//        (fovName == name)  )
//   {
//      fov = (FieldOfView*) obj;
//      #ifdef DEBUG_HW_SET
//         MessageInterface::ShowMessage ("fov reference set");
//         MessageInterface::ShowMessage ("fov =<%p>, fov name = '%s'\n", fov,
//                                        fov ? fov->GetName().c_str() : "NULL");
//         MessageInterface::ShowMessage("Exiting Hardware::SetRefObject()\n\n");
//      #endif
//      return true;
//   }

   // kick it up the chain for all other types
   #ifdef DEBUG_HW_SET
      MessageInterface::ShowMessage("Exiting Hardware::SetRefObject <%p> '%s', ",
                                    this, GetName().c_str());
      MessageInterface::ShowMessage("calling GmatBase\n\n");
   #endif
   return GmatBase::SetRefObject(obj,type,name);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 *
 * @return true if object name changed, false if not.
 */
//--------------------------------------------------------
bool Hardware::RenameRefObject(const UnsignedInt type,
                               const std::string &oldName,
                               const std::string &newName)
{
//   if ((type == Gmat::FIELD_OF_VIEW) && (fovName == oldName))
//   {
//      fovName = newName;
//      return true;
//   }

   // only reaches here if name is not found
   return false;

}
////////////////////////////////////////////////////////////////////////
// pass throughs of calls by subclasses (ref parameters with indices)

GmatBase* Hardware::GetRefObject(const UnsignedInt type,
                                 const std::string &name,
                                 const Integer index)
{
   return GmatBase::GetRefObject(type,name,index);
}


bool      Hardware::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                 const std::string &name,
                                 const Integer index)
{
   return GmatBase::SetRefObject(obj,type,name,index);
}


//------------------------------------------------------------------------
// This function is used to verify a referenced object is properly set up
// or not.
//------------------------------------------------------------------------
/*
bool Hardware::VerifyRefObject(std::string subTypeName, GmatBase* obj)
{
	MessageInterface::ShowMessage("Hardware::VerifyRefObject \n");
	return true;
}
*/

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the body to sensor rotation matrix; only parameter computed
 * from user inputs
 *
 * @return true unless initialization fails.
 */
//------------------------------------------------------------------------------
bool Hardware::Initialize()
{
   // compute R_SB from direction and second direction
   // default settings for R_SB are primary direction along +Z of spacecraft
   // body coordinate system (BCS), the secondary direction along -Y. The +Z of
   // the HW coordinate system in this default aligns with the +Z of BCS. The
   // other two axes of the default HW system are +X assigned to the normal and
   // +Y computed as Z x X.
   //
   // The location variable will be used in an upgrade to allow for translation
   // of the origin within the BCS frame.
   //
   #ifdef DEBUG_HW_INITIALIZE
      MessageInterface::ShowMessage("Entering Hardware::Initialize for ");
      MessageInterface::ShowMessage("object='%s', pointer=<%p>\n",
                                    GetName().c_str(),this);
      // @TODO: fovName and fov are not declared at this time
      // MessageInterface::ShowMessage("References FieldOfView object '%s'\n ",
      //                              fovName.c_str());
      // if (fov == NULL)
      //   MessageInterface::ShowMessage("No FOV, no FOV initialization needed\n");
      // else
      //   MessageInterface::ShowMessage("FOV pointer is set to <%p>/n",fov);


   #endif

//   @TODO:  may add fov code back later
//   if (fov)
//      fov->Initialize();

   Rvector3 zhat(direction[0], direction[1], direction[2]);
   Rvector3 vhat(secondDirection[0], secondDirection[1], secondDirection[2]);

#ifdef DEBUG_HW_INITIALIZE
   MessageInterface::ShowMessage("Parameter vectors in Initialize():\n");
   MessageInterface::ShowMessage("Primary Direction =\n");
   MessageInterface::ShowMessage(zhat.ToString(3));
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("Secondary Direction =\n");
   MessageInterface::ShowMessage(vhat.ToString(3));
   MessageInterface::ShowMessage("\n");
   MessageInterface::ShowMessage("HW frame origin offset =\n");
   Rvector3 offset(location[0], location[1], location[2]);
   MessageInterface::ShowMessage(offset.ToString(3));
   MessageInterface::ShowMessage("\nHW's rotationInput = %s\n\n",
                                 rotationInput.c_str());
#endif
   if (zhat.IsZeroVector() || vhat.IsZeroVector())
   {
      #ifdef DEBUG_HW_INITIALIZE
         MessageInterface::ShowMessage(
                                       "At least one direction is set to zero vector, using default I33\n");//
         MessageInterface::ShowMessage("   for body to sensor rotation\n\n");
         MessageInterface::ShowMessage("Exiting Initialize()\n\n");
      #endif
      return true;
   }


   // Check for construct
   if (rotationInput == "Construct")
   {
      zhat = zhat.Normalize();
      vhat = vhat.Normalize();
      Rvector3 normal = Cross (zhat,vhat);

      Real mag = normal.GetMagnitude();
      if (mag < 1.0e-05)
      {
         MessageInterface::ShowMessage(
             "Two direction vectors are collinear in hardware initialization, "
         );
         MessageInterface::ShowMessage("  magnitude of normal is %12.7f\n",mag);
         MessageInterface::ShowMessage("  Exiting Initialize()\n\n");
         return false; // is exception raised here?
      }
      Rvector3 xhat = normal/mag;
      Rvector3 yhat = Cross (zhat,xhat);

      // form body to sensor rotation matrix R_SB
      R_SB(0,0) = xhat(0);
      R_SB(0,1) = xhat(1);
      R_SB(0,2) = xhat(2);
      R_SB(1,0) = yhat(0);
      R_SB(1,1) = yhat(1);
      R_SB(1,2) = yhat(2);
      R_SB(2,0) = zhat(0);
      R_SB(2,1) = zhat(1);
      R_SB(2,2) = zhat(2);

      #ifdef DEBUG_HW_INITIALIZE
         MessageInterface::ShowMessage(
              "Body to sensor rotation in Initialize()\n");
         for (int i=0; i < 3; i++)
         {
            MessageInterface::ShowMessage(R_SB.ToRowString(i,3,6) );
            MessageInterface::ShowMessage("\n");
         }
      #endif
   }
   else if (rotationInput == "DCM")
   {
      // Pass...
   }
   else if (rotationInput == "Euler")
   {
      throw HardwareException("\nRotationOption(Euler) is not "
                              "implemented yet.");
   }
   else
   {
      // @TODO: Is it necessary?
      throw HardwareException("The input value for RotationOption('" +
                              rotationInput + "') is not allowed value.");
   }

   #ifdef DEBUG_HW_INITIALIZE
      MessageInterface::ShowMessage("Exiting Initialize()\n\n");
   #endif

   return true;
}
