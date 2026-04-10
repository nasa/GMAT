//$Id:$
//------------------------------------------------------------------------------
//                               SpiceAttitude
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under
// FDSS Task order 28.
//
// Author: Wendy C. Shoan/GSFC
// Created: 2010.04.16
//
/**
 * Class implementation for the SpiceAttitude class.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Attitude.hpp"
#include "AttitudeException.hpp"
#include "SpiceAttitude.hpp"
#include "MessageInterface.hpp"
#include "AttitudeConversionUtility.hpp"

//#define DEBUG_SPICE_ATTITUDE
//#define DEBUG_SPICE_ATTITUDE_GET_SET

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string
SpiceAttitude::PARAMETER_TEXT[SpiceAttitudeParamCount - AttitudeParamCount] =
{
   "AttitudeKernelName",
   "SCClockKernelName",
   "FrameKernelName",
};

const Gmat::ParameterType
SpiceAttitude::PARAMETER_TYPE[SpiceAttitudeParamCount - AttitudeParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
};

const Integer SpiceAttitude::UNDEFINED_NAIF_ID           = -123456789;
const Integer SpiceAttitude::UNDEFINED_NAIF_ID_REF_FRAME = -123456789;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  SpiceAttitude(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpiceAttitude class
 * (default Constructor).
 */
//------------------------------------------------------------------------------
SpiceAttitude::SpiceAttitude(const std::string &attName) :
   Attitude("SpiceAttitude",attName),
   scName          (""),
   naifId          (UNDEFINED_NAIF_ID),
   refFrameNaifId  (UNDEFINED_NAIF_ID_REF_FRAME)
{
   parameterCount = SpiceAttitudeParamCount;
   objectTypeNames.push_back("SpiceAttitude");
   attitudeModelName         = "SpiceAttitude";
   modifyCoordSysAllowed     = false;
   setInitialAttitudeAllowed = false;
   #ifdef __USE_SPICE__
      reader = new SpiceAttitudeKernelReader();
   #endif
   ck.clear();
   sclk.clear();
   fk.clear();
   ckFullPath.clear();
   sclkFullPath.clear();
   fkFullPath.clear();
}

//------------------------------------------------------------------------------
//  SpiceAttitude(const SpiceAttitude &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpiceAttitude class as a copy of the
 * specified SpiceAttitude class.
 * (copy constructor)
 *
 * @param <att> SpiceAttitude object to copy.
 */
//------------------------------------------------------------------------------
SpiceAttitude::SpiceAttitude(const SpiceAttitude& att) :
   Attitude(att),
   scName           (att.scName),
   naifId           (att.naifId),
   refFrameNaifId   (att.refFrameNaifId)
{
   ck.clear();
   sclk.clear();
   fk.clear();
   ckFullPath.clear();
   sclkFullPath.clear();
   fkFullPath.clear();
   ck           = att.ck;
   sclk         = att.sclk;
   fk           = att.fk;
   ckFullPath   = att.ckFullPath;
   sclkFullPath = att.sclkFullPath;
   fkFullPath   = att.fkFullPath;
   #ifdef __USE_SPICE__
      reader = (att.reader)->Clone();
   #endif

}

//------------------------------------------------------------------------------
//  SpiceAttitude& operator= (const SpiceAttitude& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SpiceAttitude class.
 *
 * @param att the SpiceAttitude object whose data to assign to "this"
 *            SpiceAttitude.
 *
 * @return "this" SpiceAttitude with data of input SpiceAttitude att.
 */
//------------------------------------------------------------------------------
SpiceAttitude& SpiceAttitude::operator=(const SpiceAttitude& att)
{
   if (&att == this)
      return *this;
   Attitude::operator=(att);
   ck.clear();
   sclk.clear();
   fk.clear();
   ckFullPath.clear();
   sclkFullPath.clear();
   fkFullPath.clear();
   scName         = att.scName;
   naifId         = att.naifId;
   refFrameNaifId = att.refFrameNaifId;
   ck             = att.ck;
   sclk           = att.sclk;
   fk             = att.fk;
   ckFullPath     = att.ckFullPath;
   sclkFullPath   = att.sclkFullPath;
   fkFullPath     = att.fkFullPath;
   #ifdef __USE_SPICE__
      if (reader) delete reader;
      reader         = (att.reader)->Clone();
   #endif
   return *this;
}

//------------------------------------------------------------------------------
//  ~SpiceAttitude()
//------------------------------------------------------------------------------
/**
 * Destroys the SpiceAttitude class (destructor).
 */
//------------------------------------------------------------------------------
SpiceAttitude::~SpiceAttitude()
{
   #ifdef __USE_SPICE__
      if (reader) delete reader;
   #endif
   ck.clear();
   sclk.clear();
   fk.clear();
   ckFullPath.clear();
   sclkFullPath.clear();
   fkFullPath.clear();
}


//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
 /**
 * Initializes the SpiceAttitude.
 *
 * @return Success flag.
 *
  */
//---------------------------------------------------------------------------
bool SpiceAttitude::Initialize()
{
   #ifdef DEBUG_SPICE_ATTITUDE
      MessageInterface::ShowMessage("Entering SpiceAttitude::Initialize\n");
   #endif
   bool isOK = Attitude::Initialize();
   if (!isOK) return false;

   if (scName == "")
   {
      std::string errmsg = "Error - object name not set on SpiceAttitude object.\n";
      throw AttitudeException(errmsg);
   }
   if (ck.empty())
   {
      std::string errmsg = "Error - no CK pointing kernel(s) set on SpiceAttitude for object ";
      errmsg += scName + "\n";
      throw AttitudeException(errmsg);
   }
   if (sclk.empty())
   {
      std::string errmsg = "Error - no SCLK clock kernel(s) set on SpiceAttitude for object ";
      errmsg += scName + "\n";
      throw AttitudeException(errmsg);
   }
   if (fk.empty())
   {
      std::string warnmsg = "Warning - no FK frame kernel(s) set on SpiceAttitude for object ";
      warnmsg += scName + ".  A Frame Kernel may be necessary.\n";
      MessageInterface::ShowMessage(warnmsg.c_str());
   }
   
   #ifdef __USE_SPICE__
      // Load the CK kernel(s)
      // Changed to use full path names (LOJ: 2014.06.27)
      //for (StringArray::iterator j = ck.begin(); j != ck.end(); ++j)
      for (StringArray::iterator j = ckFullPath.begin(); j != ckFullPath.end(); ++j)
      {
         #ifdef DEBUG_SPICE_ATTITUDE
         MessageInterface::ShowMessage("   ckFullPath = '%s'\n", (*j).c_str());
         #endif
         reader->LoadKernel(*j);
      }
      // Load the SCLK kernel(s)
      for (StringArray::iterator j = sclkFullPath.begin(); j != sclkFullPath.end(); ++j)
      {
         #ifdef DEBUG_SPICE_ATTITUDE
         MessageInterface::ShowMessage("   sclkFullPath = '%s'\n", (*j).c_str());
         #endif
         reader->LoadKernel(*j);
      }
      // Load the FK kernel(s), if any
      for (StringArray::iterator j = fkFullPath.begin(); j != fkFullPath.end(); ++j)
      {
         #ifdef DEBUG_SPICE_ATTITUDE
         MessageInterface::ShowMessage("   fkFullPath = '%s'\n", (*j).c_str());
         #endif
         reader->LoadKernel(*j);
      }
   #endif
   if (naifId == UNDEFINED_NAIF_ID)
   {
      #ifdef __USE_SPICE__
         naifId = reader->GetNaifID(scName);
         #ifdef DEBUG_SPICE_ATTITUDE
            MessageInterface::ShowMessage(
                  "Retrieved NAIF ID for %s -> %d\n", scName.c_str(), naifId);
         #endif
         if (naifId == 0)
         {
            std::string errmsg = "Error - NAIF ID not available for object \n";
            errmsg += scName + "\n";
            throw AttitudeException(errmsg);
         }
      #else
         std::string errmsg = "Error - NAIF ID not set on SpiceAttitude object.\n";
         throw AttitudeException(errmsg);
      #endif
   }

   if (refFrameNaifId == UNDEFINED_NAIF_ID_REF_FRAME)
   {
      std::string errmsg = "Error - NAIF ID for object reference frame not set on SpiceAttitude object.\n";
      throw AttitudeException(errmsg);
   }

   #ifdef DEBUG_SPICE_ATTITUDE
      MessageInterface::ShowMessage("Leaving SpiceAttitude::Initialize\n");
   #endif

   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the SpiceAttitude.
 *
 * @return clone of the SpiceAttitude.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SpiceAttitude::Clone() const
{
   return (new SpiceAttitude(*this));
}

//------------------------------------------------------------------------------
//  void SetObjectID(const std::string &objName, Integer objNaifId,
//                   Integer objRefFrameNaifId)
//------------------------------------------------------------------------------
/**
 * This method sets the object ID information (name, NAIF ID, reference frame
 * NAIF ID).
 *
 * @param objName           name of the object for which to set the object ids
 * @param objNaifId         NAIF ID of the object
 * @param objRefFrameNaifId reference frame NAIF ID
 *
 */
//------------------------------------------------------------------------------
void SpiceAttitude::SetObjectID(const std::string &objName, Integer objNaifId,
                                Integer objRefFrameNaifId)
{
   scName         = objName;
   naifId         = objNaifId;
   refFrameNaifId = objRefFrameNaifId;
}

//---------------------------------------------------------------------------
//  const Rvector&   GetQuaternion(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as a quaternion.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the quaternion representation of the attitude, computed at
 *         time atTime.
 */
//---------------------------------------------------------------------------
const Rvector&   SpiceAttitude::GetQuaternion(Real atTime)
{
   ComputeCosineMatrixAndAngularVelocity(atTime);
   attitudeTime     = atTime;
   quaternion       = AttitudeConversionUtility::ToQuaternion(dcm);
   return quaternion;
}

//---------------------------------------------------------------------------
//  const Rvector3&   GetEulerAngles(Real tTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as an array of Euler angles.  This
 * method assumes the previously set Euler sequence.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the euler angle representation of the attitude, computed at
 *         time atTime (radians).
 */
//---------------------------------------------------------------------------
const Rvector3&  SpiceAttitude::GetEulerAngles(Real atTime)
{
   ComputeCosineMatrixAndAngularVelocity(atTime);
   attitudeTime   = atTime;
   eulerAngles    = AttitudeConversionUtility::ToEulerAngles(dcm,
                              (Integer) eulerSequenceArray.at(0),
                              (Integer) eulerSequenceArray.at(1),
                              (Integer) eulerSequenceArray.at(2));
   return eulerAngles;
}

//---------------------------------------------------------------------------
//  const Rvector3&   GetEulerAngles(Real atTime,
//                                   Integer seq1, Integer seq2,
//                                   Integer seq3)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as an array of Euler angles.  This
 * method uses the Euler sequence passed in here.
 *
 * @param atTime  time at which to compute the attitude.
 * @param seq1    euler Sequence 1
 * @param seq2    euler sequence 2
 * @param seq3    euler sequence 3
 *
 * @return the euler angle representation of the attitude, computed at
 *         time atTime (radians).
 * @todo - figure out what to do if the time is the same, but the
 *         Euler sequence is different that the last time it was computed.
 */
//---------------------------------------------------------------------------
const Rvector3&  SpiceAttitude::GetEulerAngles(Real atTime,  Integer seq1,
                                               Integer seq2, Integer seq3)
{
   ComputeCosineMatrixAndAngularVelocity(atTime);
   attitudeTime   = atTime;
   eulerAngles    = AttitudeConversionUtility::ToEulerAngles(dcm, seq1, seq2, seq3);
   return eulerAngles;
}

//---------------------------------------------------------------------------
//  const Rmatrix33&   GetCosineMatrix(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as direction cosine matrix.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the direction cosine matrix representation of the attitude,
 *         computed at time atTime.
 */
//---------------------------------------------------------------------------
const Rmatrix33& SpiceAttitude::GetCosineMatrix(Real atTime)
{
   ComputeCosineMatrixAndAngularVelocity(atTime);
   attitudeTime = atTime;

   return dcm;
}

//---------------------------------------------------------------------------
//  const Rvector3&   GetAngularVelocity(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude rates at time atTime as an angular velocity.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the angular velocity representation of the attitude rates,
 *         computed at time atTime (radians/second).
 */
//---------------------------------------------------------------------------
const Rvector3& SpiceAttitude::GetAngularVelocity(Real atTime)
{
   ComputeCosineMatrixAndAngularVelocity(atTime);
   attitudeTime = atTime;
   return angVel;
}

//---------------------------------------------------------------------------
//  const Rvector3&   GetEulerAngleRates(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude rates at time atTime as an array of euler
 * angle rates.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the euler angle rates representation of the attitude rates,
 *         computed at time atTime (radians/second).
 */
//---------------------------------------------------------------------------
const Rvector3& SpiceAttitude::GetEulerAngleRates(Real atTime)
{
   ComputeCosineMatrixAndAngularVelocity(atTime);
   attitudeTime        = atTime;
   eulerAngles         = GetEulerAngles(atTime);
   eulerAngleRates     = AttitudeConversionUtility::ToEulerAngleRates(angVel,
                         eulerAngles,
                         (Integer) eulerSequenceArray.at(0),
                         (Integer) eulerSequenceArray.at(1),
                         (Integer) eulerSequenceArray.at(2));
   return eulerAngleRates;
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
std::string SpiceAttitude::GetParameterText(const Integer id) const
{
   if (id >= AttitudeParamCount && id < SpiceAttitudeParamCount)
      return PARAMETER_TEXT[id - AttitudeParamCount];
   return Attitude::GetParameterText(id);
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
Integer SpiceAttitude::GetParameterID(const std::string &str) const
{
   for (Integer i = AttitudeParamCount; i < SpiceAttitudeParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - AttitudeParamCount])
         return i;
   }

   return Attitude::GetParameterID(str);
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
Gmat::ParameterType SpiceAttitude::GetParameterType(const Integer id) const
{
   if (id >= AttitudeParamCount && id < SpiceAttitudeParamCount)
      return PARAMETER_TYPE[id - AttitudeParamCount];

   return Attitude::GetParameterType(id);
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
std::string SpiceAttitude::GetParameterTypeString(const Integer id) const
{
   return Attitude::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id    ID for the requested parameter
 * @param index index into the array of strings
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string  SpiceAttitude::GetStringParameter(const Integer id,
                                               const Integer index) const
{
   if (id == ATTITUDE_KERNEL_NAME)
   {
      if ((index < 0) || (index >= (Integer) ck.size()))
      {
         std::string errmsg = "Error attempting to retrieve CK kernel name for object ";
         errmsg += scName + " - index out-of-bounds.\n";
         throw AttitudeException(errmsg);
      }
      return ck.at(index);
   }
   if (id == SC_CLOCK_KERNEL_NAME)
   {
      if ((index < 0) || (index >= (Integer) sclk.size()))
      {
         std::string errmsg = "Error attempting to retrieve SCLK kernel name for object ";
         errmsg += scName + " - index out-of-bounds.\n";
         throw AttitudeException(errmsg);
      }
      return sclk.at(index);
   }
   if (id == FRAME_KERNEL_NAME)
   {
      if ((index < 0) || (index >= (Integer) fk.size()))
      {
         std::string errmsg = "Error attempting to retrieve FK kernel name for object ";
         errmsg += scName + " - index out-of-bounds.\n";
         throw AttitudeException(errmsg);
      }
      return fk.at(index);
   }
   return Attitude::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const std::string value,
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id    ID for the requested parameter.
 * @param value string value for the requested parameter.
 * @param index index into the string array
 *
 * @exception <AttitudeException> thrown if value is out of range
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool SpiceAttitude::SetStringParameter(const Integer id,
                                       const std::string &value,
                                       const Integer index)
{
   if (isInitialized)
      return true;
   
   #ifdef DEBUG_SPICE_ATTITUDE_GET_SET
      MessageInterface::ShowMessage(
            "\nEntering SetStringParameter with id = %d, value = \"%s\", index = %d\n",
            id, value.c_str(), index);
   #endif
   // Changed to save full path kernel names (LOJ: 2014.06.27)
   // We may need to show full path in the GUI as a hint in a future
   if (id == ATTITUDE_KERNEL_NAME)
   {
      if ((index < 0) || (index > (Integer) ck.size()))
      {
         std::string errmsg = "Error attempting to set CK kernel name for object ";
         errmsg += scName +  " - index out-of-bounds.\n";
         throw AttitudeException(errmsg);
      }
      std::string fnNoPath;
      std::string fnFullPath = GmatBase::GetFullPathFileName(fnNoPath, GetName(), value,
                     "VEHICLE_EPHEM_SPK_PATH", true, "", true, true);
      
      // Check for non-existent file
      if (fnFullPath == "")
         throw AttitudeException("The spice attitude kernel '" + value + "' does not exist");
      
      if (index == (Integer) ck.size())
      {
         ck.push_back(value);
         ckFullPath.push_back(fnFullPath);
      }
      else
      {
         ck.at(index) = value;
         ckFullPath.at(index) = fnFullPath;
      }
      return true;
   }
   if (id == SC_CLOCK_KERNEL_NAME)
   {
      if ((index < 0) || (index > (Integer) sclk.size()))
      {
         std::string errmsg = "Error attempting to set SCLK kernel name for object ";
         errmsg += scName + " - index out-of-bounds.\n";
         throw AttitudeException(errmsg);
      }
      
      std::string fnNoPath;
      std::string fnFullPath = GmatBase::GetFullPathFileName(fnNoPath, GetName(), value,
                     "VEHICLE_EPHEM_SPK_PATH", true, "", true, true);
      
      // Check for non-existent file
      if (fnFullPath == "")
         throw AttitudeException("The spice sc clock kernel '" + value + "' does not exist");
      
      if (index == (Integer) sclk.size())
      {
         sclk.push_back(value);
         sclkFullPath.push_back(fnFullPath);
      }
      else
      {
         sclk.at(index) = value;
         sclkFullPath.at(index) = fnFullPath;
      }
      return true;
   }
   if (id == FRAME_KERNEL_NAME)
   {
      if ((index < 0) || (index > (Integer) fk.size()))
      {
         std::string errmsg = "Error attempting to set FK kernel name for object ";
         errmsg += scName +  " - index out-of-bounds.\n";
         throw AttitudeException(errmsg);
      }
      
      std::string fnNoPath;
      std::string fnFullPath = GmatBase::GetFullPathFileName(fnNoPath, GetName(), value,
                     "VEHICLE_EPHEM_SPK_PATH", true, "", true, true);
      
      // Check for non-existent file
      if (fnFullPath == "")
         throw AttitudeException("The spice frame kernel '" + value + "' does not exist");
      
      if (index == (Integer) fk.size())
      {
         fk.push_back(value);
         fkFullPath.push_back(fnFullPath);
      }
      else
      {
         fk.at(index) = value;
         fkFullPath.at(index) = fnFullPath;
      }
      return true;
   }
   return Attitude::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const std::string label, const std::string value,
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter label.
 *
 * @param label string label for the requested parameter.
 * @param value string value for the requested parameter.
 * @param index index into the string array
 *
 * @exception <AttitudeException> thrown if value is out of range
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool SpiceAttitude::SetStringParameter(const std::string label,
                                       const std::string &value,
                                       const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the requested string array.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray
 */
//------------------------------------------------------------------------------
const StringArray& SpiceAttitude::GetStringArrayParameter(const Integer id) const
{
   if (id == ATTITUDE_KERNEL_NAME)
      return ck;
   else if (id == SC_CLOCK_KERNEL_NAME)
      return sclk;
   else if (id == FRAME_KERNEL_NAME)
      return fk;

   return Attitude::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
//  protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime)
//------------------------------------------------------------------------------
/**
 * This method computes the current CosineMatrix at the input time atTime.
 *
 * @param atTime the A1Mjd time at which to compute the attitude.
 *
 */
//------------------------------------------------------------------------------
void SpiceAttitude::ComputeCosineMatrixAndAngularVelocity(Real atTime)
{
   if (!isInitialized || needsReinit)  Initialize();

   #ifdef __USE_SPICE__
      reader->GetTargetOrientation(scName, naifId, refFrameNaifId, atTime, dcm, angVel);
   #else
      std::string errmsg = "Error - attempting to use SpiceAttitude when ";
      errmsg += "SPICE is not included in the GMAT build.\n";
      throw AttitudeException(errmsg);
   #endif

}


//------------------------------------------------------------------------------
//  virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime)
//------------------------------------------------------------------------------
/**
* This method computes the current CosineMatrix at the input time atTime.
*
* @param atTime the GmatTime time at which to compute the attitude.
*
*/
//------------------------------------------------------------------------------
void SpiceAttitude::ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime)
{
   ///@todo: it needs to add code for a high precision of time
   ComputeCosineMatrixAndAngularVelocity(atTime.GetMjd());
}

//------------------------------------------------------------------------------
//  private methods
//------------------------------------------------------------------------------
// none
