//$Id$
//------------------------------------------------------------------------------
//                                  CalculatedPoint
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
// Created: 2005/04/04
//
/**
 * Implementation of the CalculatedPoint class.
 *
 * @note This is an abstract class.
 */
//------------------------------------------------------------------------------

#include <vector>
#include <algorithm>              // for find()
#include "gmatdefs.hpp"
#include "SpacePoint.hpp"
#include "CalculatedPoint.hpp"
#include "SolarSystem.hpp"
#include "SolarSystemException.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"

//#define DEBUG_CP_OBJECT
//#define DEBUG_CP_BODIES
//#define DEBUG_CP_SET_STRING
//#define DEBUG_CP_ACTION
//#define DEBUG_CP_RENAME
//#define DEBUG_CP_CLOAKING

//---------------------------------
// static data
//---------------------------------
const std::string
CalculatedPoint::PARAMETER_TEXT[CalculatedPointParamCount - SpacePointParamCount] =
{
   "NumberOfBodies",
   "BodyNames",
};

const Gmat::ParameterType
CalculatedPoint::PARAMETER_TYPE[CalculatedPointParamCount - SpacePointParamCount] =
{
   Gmat::INTEGER_TYPE,
   Gmat::OBJECTARRAY_TYPE,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  CalculatedPoint(const std::string &ptType, 
//                  const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the CalculatedPoint class
 * (default constructor).
 *
 * @param <ptType>  string representation of its point type
 * @param <itsName> parameter indicating the name of the CalculatedPoint.
 */
//------------------------------------------------------------------------------
CalculatedPoint::CalculatedPoint(const std::string &ptType, 
                                 const std::string &itsName) :
   SpacePoint(Gmat::CALCULATED_POINT, ptType, itsName),
   numberOfBodies (0),
   isBuiltIn      (false),
   builtInType    (""),
   lastStateTime  (GmatTimeConstants::MJD_OF_J2000),
   lastStateTimeGT(GmatTime(GmatTimeConstants::MJD_OF_J2000))
{
   objectTypes.push_back(Gmat::CALCULATED_POINT);
   objectTypeNames.push_back("CalculatedPoint");
   parameterCount = CalculatedPointParamCount;

   // we want to cloak the DEFAULT CalculatedPoint data; i.e. we want to write only those
   // parameters that have been modified by the user to a script; and we don't
   // want to include the Create line either.  This will not be true for user-defined
   // CalculatedPoints (LOJ: 2013.12.17)
//   cloaking = true; // by default, calculated points are user-defined; the
                      // built-in SSB will have its isBuiltin flag set by a
                      // call to SetIsBuiltIn, where the cloaking flag will
                      // be set properly.   (WCS: 2014.01.10)
}

//------------------------------------------------------------------------------
//  CalculatedPoint(const CalculatedPoint &cp)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the CalculatedPoint class as a copy of the
 * specified CalculatedPoint object (copy constructor).
 *
 * @param <cp> CalculatedPoint object to copy.
 */
//------------------------------------------------------------------------------
CalculatedPoint::CalculatedPoint(const CalculatedPoint &cp) :
   SpacePoint    (cp),
   isBuiltIn     (cp.isBuiltIn),
   builtInType   (cp.builtInType),
   lastStateTime (cp.lastStateTime),
   lastStateTimeGT (cp.lastStateTimeGT),
   lastState     (cp.lastState)
{
   bodyNames.clear();
   bodyList.clear();
   defaultBodies.clear();

   // copy the list of body pointers - we don't want to do this here wcs 2012.08.18
   // Why?
   bodyList = cp.bodyList;

   // copy the list of body names
   for (unsigned int i = 0; i < (cp.bodyNames).size(); i++)
   {
      bodyNames.push_back((cp.bodyNames).at(i));
   }
   numberOfBodies = (Integer) bodyList.size();

   // copy the list of default body names
   for (unsigned int i = 0; i < (cp.defaultBodies).size(); i++)
   {
      defaultBodies.push_back((cp.defaultBodies).at(i));
   }
}

//------------------------------------------------------------------------------
//  CalculatedPoint& operator= (const CalculatedPoint& cp)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the CalculatedPoint class.
 *
 * @param <cp> the CalculatedPoint object whose data to assign to "this"
 *             calculated point.
 *
 * @return "this" CalculatedPoint with data of input CalculatedPoint cp.
 */
//------------------------------------------------------------------------------
CalculatedPoint& CalculatedPoint::operator=(const CalculatedPoint &cp)
{
   if (&cp == this)
      return *this;

   SpacePoint::operator=(cp);
   bodyNames.clear();
   bodyList.clear();
   defaultBodies.clear();
   // copy the list of body pointers
   for (unsigned int i = 0; i < (cp.bodyList).size(); i++)
   {
      bodyList.push_back((cp.bodyList).at(i));
   }
   // copy the list of body names
   for (unsigned int i = 0; i < (cp.bodyNames).size(); i++)
   {
      bodyNames.push_back((cp.bodyNames).at(i));
   }
   numberOfBodies = (Integer) bodyList.size();

   // copy the list of default body names
   for (unsigned int i = 0; i < (cp.defaultBodies).size(); i++)
   {
      defaultBodies.push_back((cp.defaultBodies).at(i));
   }
   isBuiltIn       = cp.isBuiltIn;
   builtInType     = cp.builtInType;
   lastStateTime   = cp.lastStateTime;
   lastStateTimeGT = cp.lastStateTimeGT;
   lastState       = cp.lastState;

   return *this;
}

//------------------------------------------------------------------------------
//  ~CalculatedPoint()
//------------------------------------------------------------------------------
/**
 * Destructor for the CalculatedPoint class.
 */
//------------------------------------------------------------------------------
CalculatedPoint::~CalculatedPoint()
{
   bodyNames.clear();
   bodyList.clear();
   defaultBodies.clear();
}

//---------------------------------------------------------------------------
//  bool IsBuiltIn()
//---------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not this is a built-in calculated point.
 *
 * @return flag indicating whether or not this is a built-in calculated point
 */
//---------------------------------------------------------------------------
bool CalculatedPoint::IsBuiltIn()
{
   return isBuiltIn;
}

//---------------------------------------------------------------------------
//  void SetIsBuiltIn(bool builtIn, const std::string &ofType)
//---------------------------------------------------------------------------
/**
 * Sets the isBuiltIn flag and built-in type.
 *
 * @param <builtIn> built-in flag
 * @param <ofType>  built-in type
 */
//---------------------------------------------------------------------------
void CalculatedPoint::SetIsBuiltIn(bool builtIn, const std::string &ofType)
{
   isBuiltIn   = builtIn;
   builtInType = ofType;
   // Don't cloak the user-defined calculated points!
   if (isBuiltIn) cloaking  =  true;
   else           cloaking  =  false;
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
Real CalculatedPoint::GetEpoch()
{
   return lastStateTime.Get();
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
Real CalculatedPoint::SetEpoch(const Real ep)
{
   A1Mjd a1(ep);
   GetMJ2000State(a1);
   return lastStateTime.Get();
}

Real CalculatedPoint::SetEpoch(const GmatTime ep)
{
   GmatTime a1(ep);
   GetMJ2000State(a1);
   lastStateTime = lastStateTimeGT.GetMjd();

   return lastStateTime.Get();
}


Rvector6 CalculatedPoint::GetLastState()
{
   return lastState;
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
std::string CalculatedPoint::GetParameterText(const Integer id) const
{
   if (id >= SpacePointParamCount && id < CalculatedPointParamCount)
      return PARAMETER_TEXT[id - SpacePointParamCount];
   return SpacePoint::GetParameterText(id);
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
Integer CalculatedPoint::GetParameterID(const std::string &str) const
{
   for (Integer i = SpacePointParamCount; i < CalculatedPointParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SpacePointParamCount])
         return i;
   }
   
   // Special handler for "Add" - per Steve 2005.05.18
   if (str == "Add") return BODY_NAMES;
   
   return SpacePoint::GetParameterID(str);
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
Gmat::ParameterType CalculatedPoint::GetParameterType(const Integer id) const
{
   if (id >= SpacePointParamCount && id < CalculatedPointParamCount)
      return PARAMETER_TYPE[id - SpacePointParamCount];
      
   return SpacePoint::GetParameterType(id);
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
std::string CalculatedPoint::GetParameterTypeString(const Integer id) const
{
   return SpacePoint::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool CalculatedPoint::IsParameterReadOnly(const Integer id) const
{
   if (id == NUMBER_OF_BODIES)
      return true;

   return SpacePoint::IsParameterReadOnly(id);
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
bool CalculatedPoint::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool IsParameterCloaked(const Integer id) const
//------------------------------------------------------------------------------
bool CalculatedPoint::IsParameterCloaked(const Integer id) const
{
   #ifdef DEBUG_CP_CLOAKING
   MessageInterface::ShowMessage
      ("CalculatedPoint::IsParameterCloaked() <%p><%s>'%s' entered, id = %2d %s, cloaking = %s, "
       "isBuiltIn = %s\n",  this, GetTypeName().c_str(), GetName().c_str(), id,
       (GetParameterText(id)).c_str(), cloaking ? "true" : "false", isBuiltIn ? "true" : "false");
   #endif
   
   if (!cloaking)
   {
      #ifdef DEBUG_CP_CLOAKING
      MessageInterface::ShowMessage
         ("CalculatedPoint::IsParameterCloaked() <%p>'%s' returning false - object is not cloaked\n",
          this, GetName().c_str());
      #endif
      return false;
   }
   
   // if it's read-only, we'll cloak it
   if (IsParameterReadOnly(id))
   {
      #ifdef DEBUG_CP_CLOAKING
      MessageInterface::ShowMessage
         ("CalculatedPoint::IsParameterCloaked() <%p>'%s' returning true - parameter %d (%s) is read-only\n",
          this, GetName().c_str(), id, (GetParameterText(id)).c_str());
      #endif
      return true;
   }
   
   if (id >= SpacePointParamCount && id < CalculatedPointParamCount)
   {
      if (id == BODY_NAMES)
      {
         // If built-in calculated point, BodyNames are cloaked
         if (isBuiltIn)
         {
            #ifdef DEBUG_CP_CLOAKING
            MessageInterface::ShowMessage
               ("CalculatedPoint::IsParameterCloaked() <%p>'%s' returning true - "
                "it is built-in point\n", this, GetName().c_str());
            #endif
            return true;
         }
      }
   }
   
   bool isCloaked = SpacePoint::IsParameterCloaked(id);
   #ifdef DEBUG_CP_CLOAKING
   MessageInterface::ShowMessage
      ("CalculatedPoint::IsParameterCloaked() <%p>'%s' returning %s\n", this, GetName().c_str(),
       isCloaked ? "true" : "false");
   #endif
   return isCloaked;
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer     CalculatedPoint::GetIntegerParameter(const Integer id) const
{
   if (id == NUMBER_OF_BODIES)      return bodyList.size();
   return SpacePoint::GetIntegerParameter(id); 
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer     CalculatedPoint::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label)); 
}

Real CalculatedPoint::GetRealParameter(const Integer id) const
{
   if (id == EPOCH_PARAM)  // from SpacePoint
   {
      return lastStateTime.Get();
   }

   return SpacePoint::GetRealParameter(id);
}


Real CalculatedPoint::SetRealParameter(const Integer id,
                                       const Real value)
{
   if (id == EPOCH_PARAM)  // from SpacePoint
   {
      A1Mjd a1(value);
      GetMJ2000State(a1);
      return lastStateTime.Get();
   }

   return SpacePoint::SetRealParameter(id, value);
}


Real CalculatedPoint::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

Real CalculatedPoint::SetRealParameter(const std::string &label,
                                      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID and index.
 *
 * @param <id>    ID for the requested parameter.
 * @param <index> index into the array of strings, for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string CalculatedPoint::GetStringParameter(const Integer id,
                                                const Integer index) const
{
   if (id == BODY_NAMES)             
   {
      try
      {
         return bodyNames.at(index);
      }
      catch (const std::exception &)
      {
         throw SolarSystemException("CalculatedPoint error: index out-of-range.");
      }
   }

   return SpacePoint::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 * @note - This should not be necessary here, but GCC is getting confused 
 *         about this method.
 *
 */
//------------------------------------------------------------------------------
std::string CalculatedPoint::GetStringParameter(const Integer id) const
{
   return SpacePoint::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const std::string &label, 
//                                  const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter label and index.
 *
 * @param <label> label for the requested parameter.
 * @param <index> index into the array of strings, for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string CalculatedPoint::GetStringParameter(const std::string &label,
                                                const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the specified parameter.
 * @param <value> string value for the specified parameter.
 *
 * @return  success flag.
 *
 *
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::SetStringParameter(const Integer id, 
                                         const std::string &value)
{
   #ifdef DEBUG_CP_SET_STRING
      MessageInterface::ShowMessage("Entering CalculatedPoint::SetString with id = %d (%s), value = %s\n",
            id, GetParameterText(id).c_str(), value.c_str());
   #endif
   if (id == BODY_NAMES)
   {
      if (isBuiltIn)
      {
         std::string errmsg = "The value of \"";
         errmsg += value + "\" for field \"BodyNames\" on built-in CalculatedPoint \"";;
         errmsg += instanceName + "\" is not an allowed value.\n";
         errmsg += "The allowed values are: [None].\n";
         throw SolarSystemException(errmsg);
      }
      std::string value1 = GmatStringUtil::Trim(value);
      // If there are names inside a brace-enclosed list, reset the
      // entire array of names to that list
      if (GmatStringUtil::IsEnclosedWithBraces(value1))
      {
//         bodyNames.clear();
         TakeAction("ClearBodies");
         StringArray nameList = GmatStringUtil::ToStringArray(value1);
         for (unsigned int ii = 0; ii < nameList.size(); ii++)
         {
            ValidateBodyName(nameList.at(ii), true, true);
         }
      }
      else
      {
         ValidateBodyName(value, true, true);
      }
      
      #ifdef DEBUG_CP_SET_STRING
         MessageInterface::ShowMessage("Exiting CalculatedPoint::SetString: BodyNames are: \n");
         for (unsigned int ii = 0; ii < bodyNames.size(); ii++)
            MessageInterface::ShowMessage("   %d     %s\n", (Integer) ii, (bodyNames.at(ii)).c_str());
      #endif
      return true;
   }

   return SpacePoint::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const std::string &label, 
//                                  const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the specified parameter.
 * @param <value> string value for the specified parameter.
 *
 * @return  success flag.
 *
 * @note - This should not be necessary here, but GCC is getting confused 
 *         cbout this method.
 *
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::SetStringParameter(const std::string &label, 
                                         const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const std::string value.
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID, and the index.
 *
 * @param <id>    ID for the specified parameter.
 * @param <value> string value for the specified parameter.
 * @param <index> index into the array of strings.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool  CalculatedPoint::SetStringParameter(const Integer id,
                                          const std::string &value,
                                          const Integer index) 
{
   #ifdef DEBUG_CP_SET_STRING
      MessageInterface::ShowMessage(
            "Entering CalculatedPoint::SetString with id = %d (%s), index = %d, and value = %s\n",
            id, GetParameterText(id).c_str(), index, value.c_str());
   #endif
   if (id == BODY_NAMES)
   {
      if (isBuiltIn)
      {
         std::string errmsg = "The value of \"";
         errmsg += value + "\" for field \"BodyNames\" on built-in CalculatedPoint \"";;
         errmsg += instanceName + "\" is not an allowed value.\n";
         errmsg += "The allowed values are: [None].\n";
         throw SolarSystemException(errmsg);
      }
      if (index == (Integer) bodyNames.size())
      {
         return ValidateBodyName(value, true, true);
      }
      // replace current name
      else
      {
         return ValidateBodyName(value, true, false, index);
      }
   }
   
   return SpacePoint::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const std::string &label, const std::string value.
//                           const Integer index)
//------------------------------------------------------------------------------
/**
* This method sets the string parameter value, given the input
 * parameter label, and the index.
 *
 * @param <label> label for the specified parameter.
 * @param <value> string value for the specified parameter.
 * @param <index> index into the array of strings.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool  CalculatedPoint::SetStringParameter(const std::string &label,
                                          const std::string &value,
                                          const Integer index) 
{
   return SetStringParameter(GetParameterID(label),value,index);
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
const StringArray& CalculatedPoint::GetStringArrayParameter(const Integer id) const
{
   if (id == BODY_NAMES)
   {
      //If there have been body names set, return those names
      if (!bodyNames.empty()) return bodyNames;
      // Otherwise, return the default set of body names
      else                    return defaultBodies;
   }
   
   return SpacePoint::GetStringArrayParameter(id);
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
const StringArray& CalculatedPoint::GetStringArrayParameter(
                                    const std::string &label) const
{   
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * Returns the reference object pointer.
 *
 * @param <type>  type of the reference object.
 * @param <name>  name of the reference object.
 * @param <inde>x Index into the object array.
 * 
 * @return reference object pointer.
 */
//------------------------------------------------------------------------------
GmatBase* CalculatedPoint::GetRefObject(const UnsignedInt type,
                                        const std::string &name,
                                        const Integer index)
{
   if (type == Gmat::SPACE_POINT)
   {
      try
      {
         return bodyList.at(index);
      }
      catch (const std::exception &)
      {
         throw SolarSystemException(
               "CalculatedPoint error: index out-of-range.");
      }
   }
   return SpacePoint::GetRefObject(type, name, index);
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
bool CalculatedPoint::SetRefObject(GmatBase *obj, 
                                   const UnsignedInt type,
                                   const std::string &name)
{
   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      if (!obj->IsOfType("CelestialBody") && !obj->IsOfType("Barycenter"))
      {
         std::string errmsg = "The value of \"";
         errmsg += name + "\" for field \"BodyNames\" on CalculatedPoint \"";;
         errmsg += instanceName + "\" is not an allowed value.\n";
         errmsg += "The allowed values are: [CelestialBody or Barycenter (except SSB)].\n";
         throw SolarSystemException(errmsg);
      }
      // check to see if it's already in the list
      std::vector<SpacePoint*>::iterator pos =
         find(bodyList.begin(), bodyList.end(), obj);
      if (pos != bodyList.end())
      {
         #ifdef DEBUG_CP_OBJECT
         MessageInterface::ShowMessage
            ("CalculatedPoint::SetRefObject() the body <%p> '%s' already exist, so "
             "returning true\n", (*pos), name.c_str());
         #endif
         return true;
      }
      
      // If ref object has the same name, reset it (loj: 2008.10.24)      
      pos = bodyList.begin();
      std::string bodyName;
      bool bodyFound = false;
      while (pos != bodyList.end())
      {
         bodyName = (*pos)->GetName();         
         if (bodyName == name)
         {
            #ifdef DEBUG_CP_OBJECT
            MessageInterface::ShowMessage
               ("CalculatedPoint::SetRefObject() resetting the pointer of body '%s' <%p> to "
                "<%p>\n", bodyName.c_str(), (*pos), (SpacePoint*)obj);
            #endif
            
            (*pos) = (SpacePoint*)obj;
            bodyFound = true;
         }
         ++pos;
      }
      
      // If ref object not found, add it (loj: 2008.10.24)
      if (!bodyFound)
      {
         #ifdef DEBUG_CP_OBJECT
         MessageInterface::ShowMessage
            ("CalculatedPoint::SetRefObject() this=<%p> '%s', adding <%p> '%s' "
             "to bodyList for object %s\n", this, GetName().c_str(), obj, name.c_str(),
             instanceName.c_str());
         #endif
         
         bodyList.push_back((SpacePoint*) obj);         
         numberOfBodies++;
      }
      
      return true;
   }
   
   return SpacePoint::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <type>    reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 * 
 * @return true if object name changed, false if not.
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::RenameRefObject(const UnsignedInt type,
                                      const std::string &oldName,
                                      const std::string &newName)
{
   #ifdef DEBUG_CP_RENAME
      MessageInterface::ShowMessage("Entering LP::Rename with type = %d, oldName = %s, newName = %s\n",
            (Integer) type, oldName.c_str(), newName.c_str());
   #endif
   if ((type == Gmat::SPACE_POINT) || (type == Gmat::CALCULATED_POINT))
   {
      for (unsigned int i=0; i< bodyNames.size(); i++)
      {
         if (bodyNames[i] == oldName)
             bodyNames[i] = newName;
      }
      for (unsigned int i=0; i< defaultBodies.size(); i++)
      {
         if (defaultBodies[i] == oldName)
            defaultBodies[i] = newName;
      }
   }
   return true;
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& CalculatedPoint::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object. 
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& CalculatedPoint::GetRefObjectNameArray(const UnsignedInt type)
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
   {
      if (!bodyNames.empty())
         return bodyNames;
      else
         return defaultBodies;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return SpacePoint::GetRefObjectNameArray(type);
}

//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <action>     The string descriptor for the requested action.
 * @param <actionData> Optional data used for the action.
 *
 * @return true if the action was performed, false if not.
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::TakeAction(const std::string &action,
                                 const std::string &actionData)
{
   #ifdef DEBUG_CP_ACTION
      MessageInterface::ShowMessage(
            "Entering CP::TakeAction with action = \"%s\", actionData = \"%s\"\n",
            action.c_str(), actionData.c_str());
   #endif
   if (action == "ClearBodies")
   {
      bodyNames.clear();
      bodyList.clear();
//      defaultBodies.clear();
      numberOfBodies = 0;
      return true; 
   }
   return SpacePoint::TakeAction(action, actionData);
}

//---------------------------------------------------------------------------
//  bool TakeRequiredAction(const Integer id))
//---------------------------------------------------------------------------
/**
 * Tells object to take whatever action it needs to take before the value
 * of the specified parameter is set (e.g. clearing out arrays)
 *
 * @param <id> parameter for which to take prerequisite action.
 *
 * @return true if the action was performed (or none needed), false if not.
 */
//------------------------------------------------------------------------------
bool CalculatedPoint::TakeRequiredAction(const Integer id)
{
   #ifdef DEBUG_CP_ACTION
      MessageInterface::ShowMessage(
            "Entering CP::TakeRequiredAction with id = %d (%s)\n",
            id, (GetParameterText(id)).c_str());
   #endif
   if (id == BODY_NAMES) return TakeAction("ClearBodies");
   return SpacePoint::TakeRequiredAction(id);
}


//---------------------------------------------------------------------------
//  void SetDefaultBody(const std::string &defBody)
//---------------------------------------------------------------------------
/**
 * Method used to set a default body for this CalculatedPoint.
 *
 * @param <defBody> name of a default body for this CalculatedPoint.
 */
//---------------------------------------------------------------------------
void CalculatedPoint::SetDefaultBody(const std::string &defBody)
{
   if (find(defaultBodies.begin(), defaultBodies.end(), defBody) == defaultBodies.end())
   {
      #ifdef DEBUG_CP_BODIES
         MessageInterface::ShowMessage("Adding %s to DEFAULT body name list for object %s\n",
               defBody.c_str(), instanceName.c_str());
      #endif
      defaultBodies.push_back(defBody);
   }
}

//---------------------------------------------------------------------------
//  const StringArray& GetDefaultBodies() const
//---------------------------------------------------------------------------
/**
 * Method returning a list of Default body names.
 *
 * @return the list of default body names for this CalculatedPoint.
 */
//---------------------------------------------------------------------------
const StringArray& CalculatedPoint::GetDefaultBodies() const
{
   return defaultBodies;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  bool ValidateBodyName(const std::string &itsName, bool addToList = true,
//                        bool addToEnd = true, Integer index = 0)
//---------------------------------------------------------------------------
/**
 * Adds the body name to the list if:
 * - it is not a duplicate
 * - it is not the built-in SolarSystemBarycenter
 *
 * @param <itsName>   name of the body to add to the bodyNames list
 * @param <addToList> if true, will insert the name into the list
 * @param <addToEnd>  if true, will append the name to the end of
 *                    the list; otherwise, will add at index specified
 * @param <index>     index at which to add the name, if addToEnd = false
 *
 * @return true if successful; false otherwise
 */
//---------------------------------------------------------------------------
bool CalculatedPoint::ValidateBodyName(const std::string &itsName, bool addToList, bool addToEnd, Integer index)
{
   if (itsName == GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME)
   {
      std::string errmsg = "The value of \"";
      errmsg += itsName + "\" for field \"BodyNames\" on CalculatedPoint \"";;
      errmsg += instanceName + "\" is not an allowed value.\n";
      errmsg += "The allowed values are: [CelestialBody or Barycenter (except SSB)].\n";
      throw SolarSystemException(errmsg);
   }
   if (addToList)
   {
      if (find(bodyNames.begin(), bodyNames.end(), itsName) == bodyNames.end())
      {
         if (addToEnd)
         {
            #ifdef DEBUG_CP_BODIES
               MessageInterface::ShowMessage("Adding %s to body name list for object %s\n",
                     itsName.c_str(), instanceName.c_str());
            #endif
            bodyNames.push_back(itsName);
         }
         else
         {
            if ((index < 0) || (index > (Integer) bodyNames.size()))
               return false;  // throw an exception here?
            #ifdef DEBUG_CP_BODIES
               MessageInterface::ShowMessage("Adding %s to body name list at index %d for object %s\n",
                     itsName.c_str(), index, instanceName.c_str());
            #endif
            bodyNames.at(index) = itsName;
         }
      }
      else
      {
         std::string errmsg = "Body ";
         errmsg += itsName + " already in list for CalculatedPoint ";
         errmsg += instanceName + ".  Each celestial body must be listed only once.\n";
         throw SolarSystemException(errmsg);
      }
   }
   return true;
}



//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time
