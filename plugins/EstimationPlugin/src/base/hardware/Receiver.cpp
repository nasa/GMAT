//$Id: Receiver.cpp 47 2010-03-31$
//------------------------------------------------------------------------------
//                             Receiver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Tuan Dang Nguyen
// Created: 2010/03/31 by Tuan Dang Nguyen (GSFC-NASA)
//
/**
 * Implementation for the Receiver class
 */
//------------------------------------------------------------------------------

#include "Receiver.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Text strings used to script Receiver properties
const std::string
Receiver::PARAMETER_TEXT[ReceiverParamCount - RFHardwareParamCount] =
{
   "FrequencyModel",
   "CenterFrequency",
   "Bandwidth",
   "Id",
   "ErrorModels",
};

/// Integer IDs associated with the Receiver properties
const Gmat::ParameterType
Receiver::PARAMETER_TYPE[ReceiverParamCount - RFHardwareParamCount] =
{
   Gmat::STRING_TYPE,         // FREQUENCY_MODEL
   Gmat::REAL_TYPE,           // CENTER_FREQUENCY
   Gmat::REAL_TYPE,           // BANDWIDTH
   //Gmat::INTEGER_TYPE,        // RECEIVER_ID
   Gmat::STRING_TYPE,         // RECEIVER_ID
   Gmat::OBJECTARRAY_TYPE,    // ERROR_MODELS
};

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Receiver(const std::string &ofType, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name for the receiver
 */
//------------------------------------------------------------------------------

Receiver::Receiver(const std::string &ofType, const std::string &name) :
   RFHardware      (ofType, name),
   frequencyModel  ("constant"),
   centerFrequency (0.0),
   bandwidth       (1.0e18),
   //receiverId      (-1)
   receiverId      ("0")
{
   //MessageInterface::ShowMessage("Receiver default constructor <%p,%s>\n", this, GetName().c_str());

   objectTypeNames.push_back("Receiver");
   parameterCount = ReceiverParamCount;

   isTransmitted1 = false;
   signal1 = new Signal();

   for (Integer i = RFHardwareParamCount; i < ReceiverParamCount; ++i)
      parameterWriteOrder.push_back(i);
}

//------------------------------------------------------------------------------
// ~Receiver()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Receiver::~Receiver()
{
   //MessageInterface::ShowMessage("Receiver default destructor <%p,%s>\n", this, GetName().c_str());
   if (signal1 != NULL)
      delete signal1;
   
   for (UnsignedInt i = 0; i < errorModels.size(); ++i)
   {
      if (errorModels[i] != NULL)
         delete errorModels[i];
   }
   errorModels.clear();
   errorModelNames.clear();
}

//------------------------------------------------------------------------------
// Receiver(const Receiver & recei)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param recei The receiver that provides configuration data for this new
 *              one.
 */
//------------------------------------------------------------------------------
Receiver::Receiver(const Receiver& recei):
   RFHardware      (recei),
   frequencyModel  (recei.frequencyModel),
   centerFrequency (recei.centerFrequency),
   bandwidth       (recei.bandwidth),
   receiverId      (recei.receiverId),
   errorModelNames (recei.errorModelNames)
{
   //MessageInterface::ShowMessage("Receiver copy constructor <%p,%s>: copy from <%p,%s>\n", this, GetName().c_str(), &recei, recei.GetName().c_str());

   for (UnsignedInt i = 0; i < recei.errorModels.size(); ++i)
   {
      if (recei.errorModels[i])
         errorModels.push_back(recei.errorModels[i]->Clone());
      else
         errorModels.push_back(NULL);
   }

   for (Integer i = RFHardwareParamCount; i < ReceiverParamCount; ++i)
      parameterWriteOrder.push_back(i);
}

//------------------------------------------------------------------------------
// Receiver& operator =(const Receiver & recei)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param recei The Receiver that provides configuration data for this one.
 *
 * @return This Receiver, set to match recei.
 */
//------------------------------------------------------------------------------
Receiver& Receiver::operator=(const Receiver& recei)
{
   //MessageInterface::ShowMessage("Receiver::opertor= <%p,%s>: copy from <%p,%s>\n", this, GetName().c_str(), &recei, recei.GetName().c_str());

   if (this != &recei)
   {
      RFHardware::operator=(recei);

      frequencyModel  = recei.frequencyModel;
      centerFrequency = recei.centerFrequency;
      bandwidth       = recei.bandwidth;
      receiverId      = recei.receiverId;
      errorModelNames = recei.errorModelNames;

      for (UnsignedInt i = 0; i < errorModels.size(); ++i)
      {
         if (errorModels[i])
            delete errorModels[i];
      }
      errorModels.clear();

      for (UnsignedInt i = 0; i < recei.errorModels.size(); ++i)
      {
         if (recei.errorModels[i])
            errorModels.push_back(recei.errorModels[i]->Clone());
         else
            errorModels.push_back(NULL);
      }

   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this receiver
 *
 * @return A new Receiver configured like this one.
 */
//------------------------------------------------------------------------------
GmatBase* Receiver::Clone() const
{
   return new Receiver(*this);
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void Receiver::Copy(const GmatBase* orig)
{
   operator=(*((Receiver *)(orig)));
}


//-----------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//-----------------------------------------------------------------------------
/**
 * Method used to find the integer ID associated with the script string for an
 * Receiver object property.
 *
 * @param str The script string
 *
 * @return The integer ID associated with the string
 */
//-----------------------------------------------------------------------------
Integer Receiver::GetParameterID(const std::string & str) const
{
   for (Integer i = RFHardwareParamCount; i < ReceiverParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - RFHardwareParamCount])
      {
         if (IsParameterReadOnly(i))
            throw HardwareException("Error: '" + str + "' parameter was not defined in GMAT Receiver's syntax.\n");
         return i;
      }
   }

   return RFHardware::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Finds the script string associated with a Receiver object's property,
 * given the integer ID for that property.
 *
 * @param id The property's ID.
 *
 * @return The string associated with the property.
 */
//------------------------------------------------------------------------------
std::string Receiver::GetParameterText(const Integer id) const
{
   if (id >= RFHardwareParamCount && id < ReceiverParamCount)
      return PARAMETER_TEXT[id - RFHardwareParamCount];
   return RFHardware::GetParameterText(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the Receiver property associated
 * with the input ID.
 *
 * @param id The property's ID
 *
 * @return A text description of the property's type.
 */
//------------------------------------------------------------------------------
std::string Receiver::GetParameterTypeString(const Integer id) const
{
   return RFHardware::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the enumerated parameter type for a specified property
 *
 * @param id The property's ID
 *
 * @return The ParameterType of the object.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Receiver::GetParameterType(const Integer id) const
{
   if (id >= RFHardwareParamCount && id < ReceiverParamCount)
      return PARAMETER_TYPE[id - RFHardwareParamCount];

   return RFHardware::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units for a property
 *
 * @param id The property's ID
 *
 * @return The units for the property
 *
 * @note: This method is not yet used in GMAT.  Once the code uses it, the unit
 * data for the Receiver properties will need to be specified.
 */
//------------------------------------------------------------------------------
std::string Receiver::GetParameterUnit(const Integer id) const
{
   switch(id)
   {
      case RECEIVER_ID:
      case FREQUENCY_MODEL:
         return "";                              // It has no unit

      case CENTER_FREQUENCY:
      case BANDWIDTH:
         return "MHz";                           // The units of center frequency and bandwidth are MHz

      default:
         break;
   }
   
   return RFHardware::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Specifies whether a property is an expected user scripted parameter.
 *
 * This method is used when an object is serialized to determine if the
 * corresponding property should be included in the serialization.  Typical use
 * is when an object is written to the screen or to a file; objects identified
 * as "read only" by a return value of true from this method are but places in
 * the text shown on the screen or in the file.
 *
 * Users can script the "read only" parameters, but do so at their own risk.
 *
 * @param label The scriptable label of the property
 *
 * @return true if the property is read only, false if not.
 */
//------------------------------------------------------------------------------
bool Receiver::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Specifies whether a property is an expected user scripted parameter.
 *
 * This method is used when an object is serialized to determine if the
 * corresponding property should be included in the serialization.  Typical use
 * is when an object is written to the screen or to a file; objects identified
 * as "read only" by a return value of true from this method are not placed in
 * the text shown on the screen or in the file.
 *
 * Users can script the "read only" parameters, but do so at their own risk.
 *
 * @param id The integer ID of the property
 *
 * @return true if the property is read only, false if not.
 */
//------------------------------------------------------------------------------
bool Receiver::IsParameterReadOnly(const Integer id) const
{
   if ((id == FREQUENCY_MODEL)||(id == CENTER_FREQUENCY)||(id == BANDWIDTH))
      return true;

   return RFHardware::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param id The integer ID of the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Real Receiver::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case CENTER_FREQUENCY:
         return centerFrequency;
      case BANDWIDTH:
        return bandwidth;
      default:
         break;
   }

   return RFHardware::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param id The integer ID of the property
 * @param value The new value for the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Real Receiver::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
//      case CENTER_FREQUENCY:
//         if (value >= 0.0)
//            centerFrequency = value;
//         return centerFrequency;
//      case BANDWIDTH:
//         if (value >= 0.0)
//            bandwidth = value;
//         return bandwidth;

//      case HARDWARE_DELAY:
      case CENTER_FREQUENCY:
      case BANDWIDTH:
         MessageInterface::ShowMessage("Warning: the script to assign %lf to '%s.%s' parameter was skipped. In the current GMAT version, this parameter is not used.\n", value, GetName().c_str(), GetParameterText(id).c_str());
         return 0.0;

      default:
         break;
   }

   return RFHardware::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param label The scriptable label for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Real Receiver::GetRealParameter(const std::string & label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param label The scriptable label for the property
 * @param value The new value for the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Real Receiver::SetRealParameter(const std::string & label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value of a string property.
 *
 * @param id The id for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
std::string Receiver::GetStringParameter(const Integer id) const
{
   switch (id)
   {
      case FREQUENCY_MODEL:
         return frequencyModel;
      case RECEIVER_ID:
         return receiverId;
      default:
         break;
   }
   
   return RFHardware::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string property.
 *
 * @param id The id for the property
 * @param value The new value for the property
 *
 * @return true if the setting success
 */
//------------------------------------------------------------------------------
#include "StringUtil.hpp"
bool Receiver::SetStringParameter(const Integer id,
                                  const std::string &value)
{
   switch (id)
   {
      case FREQUENCY_MODEL:
//       frequencyModel = value;
         MessageInterface::ShowMessage("Warning: the script to assign '%s' to '%s.%s' parameter was skipped. In the current GMAT version, this parameter is not used.\n", value.c_str(), GetName().c_str(), GetParameterText(id).c_str());
         return true;
      case RECEIVER_ID:
      {
         // Verify input value for ID:
         Integer iVal;
         bool pass = GmatStringUtil::ToInteger(value, iVal);
         if (pass && (iVal >= 0))
         {
            receiverId = value;
            return true;
         }
         else
            return false;
      }
      case ERROR_MODELS:
      {
         if (!GmatStringUtil::IsValidIdentity(value))
            throw HardwareException("Error: '" + value + "' set to " + GetName() + ".ErrorModels parameter is an invalid name.\n");

         // Only add error model if it is not in the list already
         if (find(errorModelNames.begin(), errorModelNames.end(), value) ==
            errorModelNames.end())
         {
            errorModelNames.push_back(value);
         }
         return true;
      }
      default:
         break;
   }
   
   return RFHardware::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string property.
 *
 * @param label The scriptable label for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
std::string Receiver::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string property.
 *
 * @param label The scriptable label for the property
 * @param value The new value for the property
 *
 * @return true if the setting success.
 */
//------------------------------------------------------------------------------
bool Receiver::SetStringParameter(const std::string &label, const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


std::string Receiver::GetStringParameter(const Integer id, const Integer index) const
{
   if (id == ERROR_MODELS)
   {
      if ((0 <= index) && (index < errorModelNames.size()))
         return errorModelNames[index];
      else
         throw HardwareException("Error: index is out of bound when getting " + GetName() + ".ErrorModels parameter.\n");
   }

   return RFHardware::GetStringParameter(id, index);
}


bool Receiver::SetStringParameter(const Integer id, const std::string &value, const Integer index)
{
   if (id == ERROR_MODELS)
   {
      //if ((0 <= index) && (index < errorModelNames.size()))
      //{
      //   errorModelNames[index] = value;
      //   return true;
      //}
      //else if (index == errorModelNames.size())
      //{
      //   errorModelNames.push_back(value);
      //   return true;
      //}
      //else
      //{
      //   throw HardwareException("Error: index is out of bound when getting " + GetName() + ".ErrorModels parameter.\n");
      //   return false;
      //}

      if (!GmatStringUtil::IsValidIdentity(value))
         throw HardwareException("Error: '" + value + "' set to " + GetName() + ".ErrorModels parameter is an invalid name.\n");

      // Only add the error model if it is not in the list already
      if (find(errorModelNames.begin(), errorModelNames.end(), value) ==
         errorModelNames.end())
      {
         errorModelNames.push_back(value);
      }
      return true;
   }

   return RFHardware::SetStringParameter(id, value, index);
}


std::string Receiver::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


bool Receiver::SetStringParameter(const std::string &label, const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//Integer Receiver::GetIntegerParameter(const Integer id) const
//{
//   if (id == RECEIVER_ID)
//      return receiverId;
//
//   return RFHardware::GetIntegerParameter(id);
//}
//
//Integer Receiver::SetIntegerParameter(const Integer id, const Integer value)
//{
//   if (id == RECEIVER_ID)
//   {
//      receiverId = value;
//      return receiverId;
//   }
//
//   return RFHardware::SetIntegerParameter(id, value);
//}
//
//
//Integer Receiver::GetIntegerParameter(const std::string &label) const
//{
//   return GetIntegerParameter(GetParameterID(label));
//}
//
//
//Integer Receiver::SetIntegerParameter(const std::string &label, const Integer value)
//{
//   return SetIntegerParameter(GetParameterID(label), value);
//}


GmatBase* Receiver::GetRefObject(const UnsignedInt type, const std::string &name)
{
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::ERROR_MODEL))
   {
      for (UnsignedInt i = 0; i < errorModels.size(); ++i)
      {
         if (errorModels[i]->GetName() == name || errorModels[i]->GetFullName() == name)
            return errorModels[i];
      }
   }

   return  RFHardware::GetRefObject(type, name);
}


bool Receiver::SetRefObject(GmatBase *obj, const UnsignedInt type, const std::string &name)
{
   if (obj == NULL)
      return false;

   if (type == Gmat::ERROR_MODEL)
   {
      if (obj->GetType() == Gmat::ERROR_MODEL)
      {
         for (UnsignedInt i = 0; i < errorModels.size(); ++i)
         {
            // If already there, ignore
            if (errorModels[i]->GetFullName() == obj->GetFullName())
            {
               if (obj->GetName() == obj->GetFullName()) // Check only during initialization
               {
                  return true;
               }
            }

            // Don't add if it has type (trip and strand) the same as the one in the list
            if (errorModels[i]->GetStringParameter("Type") == obj->GetStringParameter("Type"))
            {
               if (errorModels[i]->GetName() != obj->GetName()) // Check only during initialization
               {
                  MessageInterface::ShowMessage("Warning: Both '%s' and '%s' are of type '%s' Each measurement type can accept only one error model\n", errorModels[i]->GetName().c_str(), obj->GetName().c_str(), obj->GetStringParameter("Type").c_str());
                  return true;
               }
               // This renames the error model when called by Spacecraft::CreateErrorModelForSignalPath(). Should not trigger during initialization
               if (errorModels[i]->GetName() == obj->GetName() && errorModels[i]->GetFullName().find(errorModels[i]->GetName()) != name.npos && errorModels[i]->GetFullName() != obj->GetFullName())
               {
                  GmatBase* refObj = obj->Clone();       // a error model needs to be cloned
                  errorModels[i] = refObj;
                  return true;
               }
            }
         }

         GmatBase* refObj = obj->Clone();       // a error model needs to be cloned
         if (refObj->GetName() == refObj->GetFullName())
         {
            refObj->SetFullName(GetName() + "." + refObj->GetName()); // It needs to have full name to differentiate it from initialization. ex: "SAT.ErrorModel1"  
         }
         errorModels.push_back(refObj);

         return true;
      }
      else
         return false;      // <-- throw here; It was supposed to be error model, but isn't.
   }

   return RFHardware::SetRefObject(obj, type, name);
}


ObjectArray& Receiver::GetRefObjectArray(const UnsignedInt type)
{
   if (type == Gmat::ERROR_MODEL)
   {
      return errorModels;
   }

   return RFHardware::GetRefObjectArray(type);
}


ObjectArray& Receiver::GetRefObjectArray(const std::string& typeString)
{
   if (typeString == "ErrorModel")
      return errorModels;

   return RFHardware::GetRefObjectArray(typeString);
}


const StringArray& Receiver::GetRefObjectNameArray(const UnsignedInt type)
{
   RFHardware::GetRefObjectNameArray(type);

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::ERROR_MODEL))
   {
      for (UnsignedInt i = 0; i < errorModelNames.size(); ++i)
         refObjectNames.push_back(errorModelNames[i]);
   }

   return refObjectNames;
}


//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the StringArray parameter value, given the input
* parameter ID.
*
* @param <id> ID for the requested parameter.
*
* @return  StringArray value of the requested parameter.
*
*/
//------------------------------------------------------------------------------
const StringArray& Receiver::GetStringArrayParameter(const Integer id) const
{
   if (id == ERROR_MODELS)
      return errorModelNames;

   return RFHardware::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter((const std::string &label) 
//                       const
//------------------------------------------------------------------------------
/**
* This method returns the StringArray parameter value, given the input
* parameter label.
*
* @param <label> label for the requested parameter.
*
* @return  StringArray value of the requested parameter.
*
*/
//------------------------------------------------------------------------------
const StringArray& Receiver::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


bool Receiver::RenameRefObject(const UnsignedInt type, const std::string &oldName,
                               const std::string &newName)
{
   if ((type == Gmat::UNKNOWN_OBJECT)||(type == Gmat::ERROR_MODEL))
   {
      for (UnsignedInt i = 0; i < errorModelNames.size(); ++i)
      {
         if (errorModelNames[i] == oldName)
            errorModelNames[i] = newName;
      }
      return true;
   }

   return RFHardware::RenameRefObject(type, oldName, newName);
}


const ObjectTypeArray& Receiver::GetRefObjectTypeArray()
{
   refObjectTypes.clear();

   RFHardware::GetRefObjectTypeArray();
   refObjectTypes.push_back(Gmat::ERROR_MODEL);

   return refObjectTypes;
}


bool Receiver::Initialize()
{
   if (!RFHardware::Initialize())
      return false;

   for (UnsignedInt i = 0; i < errorModels.size(); ++i)
   {
      if (!errorModels[i]->Initialize())
         return false;
   }

   return true;
}

//------------------------------------------------------------------------------
// Real GetDelay(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This function is used to get hardware delay.
 *
 * @return hardware delay
 */
//------------------------------------------------------------------------------
Real Receiver::GetDelay(Integer whichOne)
{
   if (whichOne == 0)
      return RFHardware::GetDelay();
   else
      throw HardwareException("Delay index is out of bound\n");
}


//------------------------------------------------------------------------------
// bool SetDelay(Real delay, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This function is used to set hardware delay.
 *
 * @return true if it is successfully set, otherwise it throws an exception.
 */
//------------------------------------------------------------------------------
bool Receiver::SetDelay(Real delay, Integer whichOne)
{
   switch(whichOne)
   {
   case 0:
      hardwareDelay1 = delay;
      return true;
   default:
      throw HardwareException("Delay index is out of bound\n");
   }
}


//------------------------------------------------------------------------------
// bool IsFeasible(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Verify a given signal is feasible or not.
 *
 * @param whichOne      The index specifying a given signal.
 *                                              The signal is used by receiver is signal1
 *
 * @return      true if it is feasible, false otherwise.
 */
//------------------------------------------------------------------------------
bool Receiver::IsFeasible(Integer whichOne)
{
   // It needs to check for frequencyModel:
   
   Real high_freq = centerFrequency + bandwidth/2;
   Real low_freq = centerFrequency - bandwidth/2;
   Signal* s = RFHardware::GetSignal();
   
   if ((low_freq <= s->GetValue())&&(s->GetValue() <= high_freq))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Integer GetSignalCount()
//------------------------------------------------------------------------------
/**
 * Get the number of signals.
 *
 * @return the number of signals. For a receiver, number of signal is 1
 */
//------------------------------------------------------------------------------
Integer Receiver::GetSignalCount()
{
   return 1;
}


//------------------------------------------------------------------------------
// bool IsTransmitted(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Verify a given signal having ability to transmit or not.
 *
 * @param whichOne      The index specifying a given signal.
 *
 * @return true for ability to transmit, false otherwise.
 */
//------------------------------------------------------------------------------
bool Receiver::IsTransmitted(Integer whichOne)
{
   return this->isTransmitted1;
}


//------------------------------------------------------------------------------
// Signal* GetSignal(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Get a specified signal.
 *
 * @param whichOne  The index specifying a given signal.
 *
 * @return          a signal for a given index.
 */
//------------------------------------------------------------------------------
Signal* Receiver::GetSignal(Integer whichOne)
{
   return RFHardware::GetSignal();
}


//------------------------------------------------------------------------------
// bool SetSignal(Signal* s, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Set a signal for a given index.
 *
 * @param s         The signal needed to set to
 * @param whichOne  The index specifying a given signal.
 *
 * @return          true if signal is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Receiver::SetSignal(Signal* s,Integer whichOne)
{
   return RFHardware::SetSignal(s);
}

