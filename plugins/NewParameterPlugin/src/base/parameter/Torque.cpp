//------------------------------------------------------------------------------
//                              Torque
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
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: Nov 16, 2021
//
/**
 * Defines the Torque classes.
 */
//------------------------------------------------------------------------------
#include "Torque.hpp"

//==============================================================================
//                              BurnTorque
//==============================================================================
/**
* Implements BurnTorque parameter class.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// BurnTorque(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
* Constructor.
*
* @param name name of the parameter
* @param obj reference object pointer
*/
//------------------------------------------------------------------------------
BurnTorque::BurnTorque(const std::string &name, GmatBase *obj)
   : OdeRvec3(name, "BurnTorque", obj, "BurnTorque", "N*m",
      Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// BurnTorque(const BurnTorque &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
 //------------------------------------------------------------------------------
BurnTorque::BurnTorque(const BurnTorque &copy)
   : OdeRvec3(copy)
{

}

//------------------------------------------------------------------------------
// const BurnTorque& operator=(const BurnTorque &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
 //------------------------------------------------------------------------------
const BurnTorque& BurnTorque::operator=(const BurnTorque &right)
{
   if (this != &right)
   {
      OdeRvec3::operator=(right);
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~FMAcceleration()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
 //------------------------------------------------------------------------------
BurnTorque::~BurnTorque()
{
   
}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
 //------------------------------------------------------------------------------
bool BurnTorque::Evaluate()
{
   mRvec3Value = OdeData::GetOdeRvec3("BurnTorque");

   if (mRvec3Value == Rvector3::RVECTOR3_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
 //------------------------------------------------------------------------------
GmatBase* BurnTorque::Clone() const
{
   return new BurnTorque(*this);
}



// methods inherited from GmatBase
// Added to allow script access to the torque arrays for computational purposes


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Returns an ID for the scripted string
 *
 * @param str The scripted field name
 *
 * @return The field ID
 */
//------------------------------------------------------------------------------
Integer BurnTorque::GetParameterID(const std::string &str) const
{
   if (str == "VectorSize")
      return VECTOR_SIZE;
   return OdeRvec3::GetParameterID(str);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the field name for an ID
 *
 * @param id The field ID
 *
 * @return The field name used in scripting
 */
//------------------------------------------------------------------------------
std::string BurnTorque::GetParameterText(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return "VectorSize";
   return OdeRvec3::GetParameterText(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies fields that should not be written to scripts on save
 *
 * @param id The field ID
 *
 * @return true if the field should not be written
 */
//------------------------------------------------------------------------------
bool BurnTorque::IsParameterReadOnly(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return true;
   return OdeRvec3::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies fields that can be set in command mode
 *
 * @param id The field ID
 *
 * @return true if the field can be set during a run
 */
//------------------------------------------------------------------------------
bool BurnTorque::IsParameterCommandModeSettable(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return false;
   return OdeRvec3::IsParameterCommandModeSettable(id);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the setting for a field
 *
 * @param id The field ID
 *
 * @return The integer setting
 */
//------------------------------------------------------------------------------
Integer BurnTorque::GetIntegerParameter(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return 3;
   return OdeRvec3::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the setting for a field
 *
 * @param label The script label for the field
 *
 * @return The integer setting
 */
//------------------------------------------------------------------------------
Integer BurnTorque::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//==============================================================================
//                            GravityTorque
//==============================================================================
/**
* Implements GravityTorque parameter class.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GravityTorque(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
* Constructor.
*
* @param name name of the parameter
* @param obj reference object pointer
*/
//------------------------------------------------------------------------------
GravityTorque::GravityTorque(const std::string &name, GmatBase *obj)
   : OdeRvec3(name, "GravityTorque", obj, "GravityTorque", "N*m",
      Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// GravityTorque(const BurnTorque &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
 //------------------------------------------------------------------------------
GravityTorque::GravityTorque(const GravityTorque &copy)
   : OdeRvec3(copy)
{

}

//------------------------------------------------------------------------------
// const GravityTorque& operator=(const BurnTorque &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
 //------------------------------------------------------------------------------
const GravityTorque& GravityTorque::operator=(const GravityTorque &right)
{
   if (this != &right)
   {
      OdeRvec3::operator=(right);
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~GravityTorque()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
 //------------------------------------------------------------------------------
GravityTorque::~GravityTorque()
{

}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
 //------------------------------------------------------------------------------
bool GravityTorque::Evaluate()
{
   mRvec3Value = OdeData::GetOdeRvec3("GravityTorque");

   if (mRvec3Value == Rvector3::RVECTOR3_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
 //------------------------------------------------------------------------------
GmatBase* GravityTorque::Clone() const
{
   return new GravityTorque(*this);
}



// methods inherited from GmatBase
// Added to allow script access to the torque arrays for computational purposes


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Returns an ID for the scripted string
 *
 * @param str The scripted field name
 *
 * @return The field ID
 */
//------------------------------------------------------------------------------
Integer GravityTorque::GetParameterID(const std::string &str) const
{
   if (str == "VectorSize")
      return VECTOR_SIZE;
   return OdeRvec3::GetParameterID(str);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the field name for an ID
 *
 * @param id The field ID
 *
 * @return The field name used in scripting
 */
//------------------------------------------------------------------------------
std::string GravityTorque::GetParameterText(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return "VectorSize";
   return OdeRvec3::GetParameterText(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies fields that should not be written to scripts on save
 *
 * @param id The field ID
 *
 * @return true if the field should not be written
 */
//------------------------------------------------------------------------------
bool GravityTorque::IsParameterReadOnly(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return true;
   return OdeRvec3::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies fields that can be set in command mode
 *
 * @param id The field ID
 *
 * @return true if the field can be set during a run
 */
//------------------------------------------------------------------------------
bool GravityTorque::IsParameterCommandModeSettable(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return false;
   return OdeRvec3::IsParameterCommandModeSettable(id);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the setting for a field
 *
 * @param id The field ID
 *
 * @return The integer setting
 */
//------------------------------------------------------------------------------
Integer GravityTorque::GetIntegerParameter(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return 3;
   return OdeRvec3::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the setting for a field
 *
 * @param label The script label for the field
 *
 * @return The integer setting
 */
//------------------------------------------------------------------------------
Integer GravityTorque::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}



//==============================================================================
//                                SRPTorque
//==============================================================================
/**
* Implements SRPTorque parameter class.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SRPTorque(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
* Constructor.
*
* @param name name of the parameter
* @param obj reference object pointer
*/
//------------------------------------------------------------------------------
SRPTorque::SRPTorque(const std::string &name, GmatBase *obj)
   : OdeRvec3(name, "SRPTorque", obj, "SRPTorque", "N*m",
      Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// SRPTorque(const SRPTorque &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
 //------------------------------------------------------------------------------
SRPTorque::SRPTorque(const SRPTorque &copy)
   : OdeRvec3(copy)
{

}

//------------------------------------------------------------------------------
// const SRPTorque& operator=(const SRPTorque &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
 //------------------------------------------------------------------------------
const SRPTorque& SRPTorque::operator=(const SRPTorque &right)
{
   if (this != &right)
   {
      OdeRvec3::operator=(right);
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~SRPTorque()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
 //------------------------------------------------------------------------------
SRPTorque::~SRPTorque()
{

}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
 //------------------------------------------------------------------------------
bool SRPTorque::Evaluate()
{
   mRvec3Value = OdeData::GetOdeRvec3("SRPTorque");

   if (mRvec3Value == Rvector3::RVECTOR3_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
 //------------------------------------------------------------------------------
GmatBase* SRPTorque::Clone() const
{
   return new SRPTorque(*this);
}


// methods inherited from GmatBase
// Added to allow script access to the torque arrays for computational purposes


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Returns an ID for the scripted string
 *
 * @param str The scripted field name
 *
 * @return The field ID
 */
//------------------------------------------------------------------------------
Integer SRPTorque::GetParameterID(const std::string &str) const
{
   if (str == "VectorSize")
      return VECTOR_SIZE;
   return OdeRvec3::GetParameterID(str);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the field name for an ID
 *
 * @param id The field ID
 *
 * @return The field name used in scripting
 */
//------------------------------------------------------------------------------
std::string  SRPTorque::GetParameterText(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return "VectorSize";
   return OdeRvec3::GetParameterText(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies fields that should not be written to scripts on save
 *
 * @param id The field ID
 *
 * @return true if the field should not be written
 */
//------------------------------------------------------------------------------
bool  SRPTorque::IsParameterReadOnly(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return true;
   return OdeRvec3::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies fields that can be set in command mode
 *
 * @param id The field ID
 *
 * @return true if the field can be set during a run
 */
//------------------------------------------------------------------------------
bool SRPTorque::IsParameterCommandModeSettable(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return false;
   return OdeRvec3::IsParameterCommandModeSettable(id);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the setting for a field
 *
 * @param id The field ID
 *
 * @return The integer setting
 */
//------------------------------------------------------------------------------
Integer SRPTorque::GetIntegerParameter(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return 3;
   return OdeRvec3::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the setting for a field
 *
 * @param label The script label for the field
 *
 * @return The integer setting
 */
//------------------------------------------------------------------------------
Integer SRPTorque::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//==============================================================================
//                               TotalTorque
//==============================================================================
/**
* Implements TotalTorque parameter class.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TotalTorque(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
/**
* Constructor.
*
* @param name name of the parameter
* @param obj reference object pointer
*/
//------------------------------------------------------------------------------
TotalTorque::TotalTorque(const std::string &name, GmatBase *obj)
   : OdeRvec3(name, "TotalTorque", obj, "TotalTorque", "N*m",
      Gmat::SPACECRAFT, GmatParam::ODE_MODEL)
{
   mDepObjectName = "";
}

//------------------------------------------------------------------------------
// TotalTorque(const TotalTorque &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
 //------------------------------------------------------------------------------
TotalTorque::TotalTorque(const TotalTorque &copy)
   : OdeRvec3(copy)
{

}

//------------------------------------------------------------------------------
// const TotalTorque& operator=(const TotalTorque &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
 //------------------------------------------------------------------------------
const TotalTorque& TotalTorque::operator=(const TotalTorque &right)
{
   if (this != &right)
   {
      OdeRvec3::operator=(right);
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~TotalTorque()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
 //------------------------------------------------------------------------------
TotalTorque::~TotalTorque()
{

}

//------------------------------------------------------------------------------
// virtual bool Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates value of the parameter.
 *
 * @return true if parameter value successfully evaluated; false otherwise
 */
 //------------------------------------------------------------------------------
bool TotalTorque::Evaluate()
{
   mRvec3Value = OdeData::GetOdeRvec3("TotalTorque");

   if (mRvec3Value == Rvector3::RVECTOR3_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// virtual GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
 //------------------------------------------------------------------------------
GmatBase* TotalTorque::Clone() const
{
   return new TotalTorque(*this);
}



// methods inherited from GmatBase
// Added to allow script access to the torque arrays for computational purposes


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Returns an ID for the scripted string
 *
 * @param str The scripted field name
 *
 * @return The field ID
 */
//------------------------------------------------------------------------------
Integer      TotalTorque::GetParameterID(const std::string &str) const
{
   if (str == "VectorSize")
      return VECTOR_SIZE;
   return OdeRvec3::GetParameterID(str);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the field name for an ID
 *
 * @param id The field ID
 *
 * @return The field name used in scripting
 */
//------------------------------------------------------------------------------
std::string  TotalTorque::GetParameterText(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return "VectorSize";
   return OdeRvec3::GetParameterText(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies fields that should not be written to scripts on save
 *
 * @param id The field ID
 *
 * @return true if the field should not be written
 */
//------------------------------------------------------------------------------
bool         TotalTorque::IsParameterReadOnly(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return true;
   return OdeRvec3::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Identifies fields that can be set in command mode
 *
 * @param id The field ID
 *
 * @return true if the field can be set during a run
 */
//------------------------------------------------------------------------------
bool         TotalTorque::IsParameterCommandModeSettable(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return false;
   return OdeRvec3::IsParameterCommandModeSettable(id);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the setting for a field
 *
 * @param id The field ID
 *
 * @return The integer setting
 */
//------------------------------------------------------------------------------
Integer      TotalTorque::GetIntegerParameter(const Integer id) const
{
   if (id == VECTOR_SIZE)
      return 3;
   return OdeRvec3::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the setting for a field
 *
 * @param label The script label for the field
 *
 * @return The integer setting
 */
//------------------------------------------------------------------------------
Integer      TotalTorque::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}
