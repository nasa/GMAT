//$Id$
//------------------------------------------------------------------------------
//                                  CoordinateBase
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
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2004/12/20
//
/**
 * Implementation of the CoordinateBase class.  This is the base class for the 
 * CoordinateSystem and AxisSystem classes.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateBase.hpp"
#include "CoordinateSystemException.hpp"
#include "SolarSystem.hpp"
#include "SpacePoint.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SET_SS
//#define DEBUG_SET_REF
//#define DEBUG_CB_SET

//---------------------------------
// static data
//---------------------------------

const std::string
CoordinateBase::PARAMETER_TEXT[CoordinateBaseParamCount - GmatBaseParamCount] =
{
   "Origin",
   "J2000Body",
};

const Gmat::ParameterType
CoordinateBase::PARAMETER_TYPE[CoordinateBaseParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECT_TYPE,
   Gmat::OBJECT_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  CoordinateBase(UnsignedInt ofType, const std::string &itsType,
//             const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base CoordinateBase structures used in derived classes
 * (default constructor).
 *
 * @param ofType   UnsignedInts enumeration for the object.
 * @param itsType  GMAT script string associated with this type of object.
 * @param itsName  Optional name for the object.  Defaults to "".
 *
 * @note There is no parameter free constructor for CoordinateBase.  Derived 
 *       classes must pass in the typeId and typeStr parameters.
 */
//---------------------------------------------------------------------------
CoordinateBase::CoordinateBase(UnsignedInt ofType,
                               const std::string &itsType,
                               const std::string &itsName) :
   GmatBase(ofType,itsType,itsName),
   origin         (NULL),
   originName     ("Earth"),
   j2000Body      (NULL),
   j2000BodyName  ("Earth"),
   solar          (NULL),
   isBuiltIn      (false),
   allowModify    (true)
{
   objectTypeNames.push_back("CoordinateBase");
   parameterCount = CoordinateBaseParamCount;

   theTimeConverter = TimeSystemConverter::Instance();
}

//---------------------------------------------------------------------------
//  CoordinateBase(const CoordinateBase &coordBase);
//---------------------------------------------------------------------------
/**
 * Constructs base CoordinateBase structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param coordBase   CoordinateBase instance to copy to create "this" 
 *                    instance.
 */
//---------------------------------------------------------------------------
CoordinateBase::CoordinateBase(const CoordinateBase &coordBase) :
GmatBase(coordBase),
   origin        (NULL),
   originName    (coordBase.originName),
   j2000Body     (NULL),
   j2000BodyName (coordBase.j2000BodyName),
   solar         (NULL),
   isBuiltIn     (coordBase.isBuiltIn),
   allowModify   (coordBase.allowModify)
{
   theTimeConverter = TimeSystemConverter::Instance();
}

//---------------------------------------------------------------------------
//  CoordinateBase& operator=(const CoordinateBase &coordBase)
//---------------------------------------------------------------------------
/**
 * Assignment operator for CoordinateBase structures.
 *
 * @param coordBase  The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const CoordinateBase& CoordinateBase::operator=(const CoordinateBase &coordBase)
{
   if (&coordBase == this)
      return *this;
   GmatBase::operator=(coordBase);

   origin        = coordBase.origin;
   originName    = coordBase.originName;
   j2000Body     = coordBase.j2000Body;
   j2000BodyName = coordBase.j2000BodyName;
   solar         = coordBase.solar;
   
   isBuiltIn     = coordBase.isBuiltIn;
   allowModify   = coordBase.allowModify;

   return *this;
}

//---------------------------------------------------------------------------
//  ~CoordinateBase()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
CoordinateBase::~CoordinateBase()
{
}

//------------------------------------------------------------------------------
//  void  SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * This method sets the Solar System for the CoordinateBase class.
 *
 * @param <ss> solar system pointer
 *
 */
//------------------------------------------------------------------------------
void CoordinateBase::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_SET_SS
   MessageInterface::ShowMessage
      ("CoordinateBase::SetSolarSystem() entered, this='%s', solar=<%p>, ss=<%p>, "
       "originName='%s', j2000BodyName='%s', origin=<%p>'%s', j2000Body=<%p>'%s'\n",
       GetName().c_str(), solar, ss, originName.c_str(), j2000BodyName.c_str(),
       origin, origin ? origin->GetName().c_str() : "NULL", j2000Body,
       j2000Body ? j2000Body->GetName().c_str() : "NULL");
   #endif
   
   if (ss == NULL)
      return;
   
   if (solar != ss)
   {
      solar = ss;
      
      // set new origin 
      SpacePoint *sp = solar->GetBody(originName);
      if (sp != NULL)
      {
         #ifdef DEBUG_SET_SS
         MessageInterface::ShowMessage
            ("   Setting origin to <%p>'%s'\n", sp, sp->GetName().c_str());
         #endif
         origin = sp;
      }
      
      // set new j2000body
      sp = solar->GetBody(j2000BodyName);
      if (sp != NULL)
      {
         #ifdef DEBUG_SET_SS
         MessageInterface::ShowMessage
            ("   Setting j2000Body to <%p>'%s'\n", sp, sp->GetName().c_str());
         #endif
         j2000Body = sp;
      }
      
      // set J2000Body of origin
      if (origin != NULL)
          origin->SetJ2000Body(solar->GetBody(origin->GetJ2000BodyName()));
      
      // Set J2000Body for other reference objects
      SetJ2000BodyForOtherRefObjects();
      
      #ifdef DEBUG_SET_SS
      MessageInterface::ShowMessage
         ("CoordinateBase::SetSolarSystem() leaving, got new SolarSystem <%p>, "
          "origin=%p, j2000Body=%p\n", solar, origin, j2000Body);
      #endif
   }
}

//------------------------------------------------------------------------------
//  void  SetOriginName(const std::string &toName)
//------------------------------------------------------------------------------
/**
 * This method sets the origin name for the CoordinateBase class.
 *
 * @param <toName> the origin name
 *
 */
//------------------------------------------------------------------------------
void CoordinateBase::SetOriginName(const std::string &toName)
{
   #ifdef DEBUG_CB_SET
      MessageInterface::ShowMessage("Entering CB::SetOriginName for %, setting to %s and allowModify = %s\n",
            instanceName.c_str(), toName.c_str(), (allowModify? "true" : "false"));
   #endif
//   if (!allowModify)
//   {
//      std::string errmsg = "Modifications to built-in coordinate system ";
//      errmsg += instanceName + " are not allowed.\n";
//      throw CoordinateSystemException(errmsg);
//   }
   originName = toName;
}

//------------------------------------------------------------------------------
//  void  SetOrigin(SpacePoint *originPtr)
//------------------------------------------------------------------------------
/**
 * This method sets the origin for the CoordinateBase class.
 *
 * @param <originPtr> pointer to the origin
 *
 */
//------------------------------------------------------------------------------
void CoordinateBase::SetOrigin(SpacePoint *originPtr)
{
   #ifdef DEBUG_CB_SET
      MessageInterface::ShowMessage("Entering SetOrigin for %s\n", instanceName.c_str());
   #endif
//   if (!allowModify)
//   {
//      std::string errmsg = "Modifications to built-in coordinate system ";
//      errmsg += instanceName + " are not allowed.\n";
//      throw CoordinateSystemException(errmsg);
//   }
   origin = originPtr;
}

//------------------------------------------------------------------------------
//  bool RequiresJ2000Body()
//------------------------------------------------------------------------------
/**
 * Identifies objects that need to have the J2000 body set in the Sandbox.
 *
 * @return true for all CoordinateBase objects.
 */
//------------------------------------------------------------------------------
bool CoordinateBase::RequiresJ2000Body()
{
   return true;
}

//------------------------------------------------------------------------------
//  void  SetJ2000BodyName(const std::string &toName)
//------------------------------------------------------------------------------
/**
 * This method sets the j2000Body name for the CoordinateBase class.
 *
 * @param <toName> the J2000 Body name
 *
 * WARNING: The J200Body must be set identically for all objects in a GMAT run;
 * not doing so will give incorrect results.
 * In addition, the setting of a body other than Earth as the J2000Body has
 * not been tested.
 */
//------------------------------------------------------------------------------
void CoordinateBase::SetJ2000BodyName(const std::string &toName)
{
   j2000BodyName = toName;
}

//------------------------------------------------------------------------------
//  void  SetJ2000Body(SpacePoint *j2000Ptr)
//------------------------------------------------------------------------------
/**
 * This method sets the j2000Body for the CoordinateBase class.
 *
 * @param <j2000Ptr> pointer to the j2000Body
 *
 * WARNING: The J200Body must be set identically for all objects in a GMAT run;
 * not doing so will give incorrect results.
 * In addition, the setting of a body other than Earth as the J2000Body has
 * not been tested.
 */
//------------------------------------------------------------------------------
void CoordinateBase::SetJ2000Body(SpacePoint *j2000Ptr)
{
   j2000Body = j2000Ptr;
   if (j2000Body) j2000BodyName = j2000Body->GetName();
}


//------------------------------------------------------------------------------
//  SolarSystem*  GetSolarSystem() const
//------------------------------------------------------------------------------
/**
 * This method returns the pointer to the solar system.
 *
 * @return pointer to the solar system
 *
 */
//------------------------------------------------------------------------------
SolarSystem* CoordinateBase::GetSolarSystem() const
{
   return solar;
}


//------------------------------------------------------------------------------
//  std::string  GetOriginName() const
//------------------------------------------------------------------------------
/**
 * This method returns the origin name for the CoordinateBase class.
 *
 * @return name of the origin for the CoordinateBase object
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetOriginName() const
{
   if (origin) return origin->GetName();
   else        return originName;
}

//------------------------------------------------------------------------------
//  SpacePoint*  GetOrigin() const
//------------------------------------------------------------------------------
/**
* This method returns the origin pointer for the CoordinateBase class.
 *
 * @return pointer to the origin for the CoordinateBase object
 */
//------------------------------------------------------------------------------
SpacePoint* CoordinateBase::GetOrigin() const
{
   return origin;
}

//------------------------------------------------------------------------------
//  std::string  GetJ2000BodyName() const
//------------------------------------------------------------------------------
/**
 * This method returns the j2000Body name for the CoordinateBase class.
 *
 * @return name of the j2000Body for the CoordinateBase object
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetJ2000BodyName() const
{
   if (j2000Body) return j2000Body->GetName();
   else           return j2000BodyName;
}

//------------------------------------------------------------------------------
//  SpacePoint*  GetJ2000Body() const
//------------------------------------------------------------------------------
/**
 * This method returns the j2000Body pointer for the CoordinateBase class.
 *
 * @return pointer to the j2000Body for the CoordinateBase object
 */
//------------------------------------------------------------------------------
SpacePoint* CoordinateBase::GetJ2000Body() const
{
   return j2000Body;
}


//------------------------------------------------------------------------------
//  bool  Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the CoordinateBase class.
 *
 * @return true, if successfully initialized; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool CoordinateBase::Initialize()
{
   if (!origin)
      throw CoordinateSystemException(
            "Origin has not been defined for CoordinateBase object " +
            instanceName);
   if (!j2000Body)
      throw CoordinateSystemException(
            "j2000Body has not been defined for CoordinateBase object " +
            instanceName);
   if (!solar)
      throw CoordinateSystemException(
            "Solar System has not been defined for CoordinateBase object " +
            instanceName);
   return true;
}

//---------------------------------------------------------------------------
// void SetModifyFlag(bool modFlag)
//---------------------------------------------------------------------------
/**
 * Sets the allowModify flag for the coordinate system (meant for the
 * built-in systems)
 *
 * @param modFlag  flag indicating whether or not this coordinate
 *                 system is a built-in system
 *
 */
//---------------------------------------------------------------------------
void CoordinateBase::SetModifyFlag(bool modFlag)
{
   allowModify = modFlag;
}

//---------------------------------------------------------------------------
// bool GetModifyFlag() const
//---------------------------------------------------------------------------
/**
 * Returns the allowModify flag for the coordinate system
 *
 * @return allowModify flag indicating whether or not this coordinate system
 *                     cannot currently be modified by the user
 *
 */
//---------------------------------------------------------------------------
bool CoordinateBase::GetModifyFlag() const
{
   return allowModify;
}

//---------------------------------------------------------------------------
// void SetIsBuiltIn(bool builtInFlag)
//---------------------------------------------------------------------------
/**
 * Sets the isBuiltIn flag for the coordinate system
 *
 * @param builtInFlag  flag indicating whether or not this coordinate
 *                     system is a built-in system
 *
 */
//---------------------------------------------------------------------------
void CoordinateBase::SetIsBuiltIn(bool builtInFlag)
{
   isBuiltIn = builtInFlag;
}

//---------------------------------------------------------------------------
// bool IsBuiltIn() const
//---------------------------------------------------------------------------
/**
 * Returns the isBuiltIn flag for the coordinate system
 *
 * @return isBuiltIn flag indicating whether or not this coordinate system
 *                   is a built-in system
 *
 */
//---------------------------------------------------------------------------
bool CoordinateBase::IsBuiltIn() const
{
   return isBuiltIn;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id  Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < CoordinateBaseParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str  string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer CoordinateBase::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < CoordinateBaseParamCount; i++)
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
Gmat::ParameterType CoordinateBase::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < CoordinateBaseParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   
   return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id  ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetParameterTypeString(const Integer id) const
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
 * @return true if the parameter is read only, false (the default) if not
 */
//---------------------------------------------------------------------------
bool CoordinateBase::IsParameterReadOnly(const Integer id) const
{
   // We don't write out the name of the J2000 Body
   if (id == J2000_BODY_NAME)
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
bool CoordinateBase::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id  ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetStringParameter(const Integer id) const
{
   if (id == ORIGIN_NAME)   
      return originName;
   else if (id == J2000_BODY_NAME)   
      return j2000BodyName;
   
   return GmatBase::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const char *value)
//------------------------------------------------------------------------------
/**
 * @see SetStringParameter(const Integer id, const std::string value)
 */
//------------------------------------------------------------------------------
bool CoordinateBase::SetStringParameter(const Integer id, 
                                        const char *value)
{
   return SetStringParameter(id, std::string(value));
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id     ID for the requested parameter.
 * @param value  string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateBase::SetStringParameter(const Integer id, 
                                        const std::string &value)
{
   #ifdef DEBUG_CB_SET
      MessageInterface::ShowMessage("Entering CB::SetStringParameter for %s with id = %d, values = %s, allowModify = %s\n",
            instanceName.c_str(), id, value.c_str(), (allowModify? "true" : "false"));
   #endif
    if (!allowModify)
    {
       std::string errmsg = "Modifications to built-in coordinate system ";
       errmsg += instanceName + " are not allowed.\n";
       throw CoordinateSystemException(errmsg);
    }
    if (id == ORIGIN_NAME) 
    {
       originName    = value; 
       return true;
    }
    else if (id == J2000_BODY_NAME) 
    {
       // Disabled for now -- just post a warning message if the user tries to 
       // set the J2000 body.
//       MessageInterface::ShowMessage(
//          "Warning: Attempting to set the read-only parameter 'J2000Body' "
//          "on the coordinate system '%s'\n", instanceName.c_str());
//       // j2000BodyName = value; 
       static bool writeIgnoredMessage = true;
       if (writeIgnoredMessage)
       {
          MessageInterface::ShowMessage
             ("*** WARNING *** \"J2000Body\" on Coordinate Systems is ignored and will be "
              "removed from a future build\n");
          writeIgnoredMessage = false;
       }
       return true;
    }
    
   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    label  label ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string CoordinateBase::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    label  String label for the parameter
 * @param    value  The new value for the parameter
 */
//------------------------------------------------------------------------------
bool CoordinateBase::SetStringParameter(const std::string &label,
                                        const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    label  String label for the parameter
 * @param    value  The new value for the parameter
 */
//------------------------------------------------------------------------------
 bool CoordinateBase::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const UnsignedInt type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the CoordinateBase class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CoordinateBase::GetRefObject(const UnsignedInt type,
                                       const std::string &name)
{
   switch (type)
   {
      case Gmat::SPACE_POINT:
         if (name == originName)    return origin;
         if (name == j2000BodyName) return j2000Body;
      default:
         break;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObject(type, name);
}

// DJC added 5/9/05 to facilitate Sandbox initialization
//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns the name(s) of the reference object(s). (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The name(s) of the reference object(s).
 */
//------------------------------------------------------------------------------
const StringArray& CoordinateBase::GetRefObjectNameArray(const UnsignedInt type)
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
   {
      static StringArray refs;
      refs.clear();
      
      refs.push_back(originName);
      if (j2000BodyName != originName)
         refs.push_back(j2000BodyName);
      
      return refs;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the CoordinateBase class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object 
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool CoordinateBase::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                  const std::string &name)
{
   #ifdef DEBUG_SET_REF
   MessageInterface::ShowMessage
      ("CoordinateBase::SetRefObject() <%s> of type %s, obj=<%p>'%s', name=%s\n",
       GetName().c_str(), GetTypeName().c_str(), obj, obj ? obj->GetName().c_str() : "NULL",
       name.c_str());
   MessageInterface::ShowMessage("   needs celestial body origin?   %s\n",
         (RequiresCelestialBodyOrigin()? "true" : "false"));
   #endif
   
   if (obj == NULL)
      return false;
   
   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      SpacePoint *sp = (SpacePoint*) obj;
      
      if (name == originName)
      {
         // Origin may have to be a celestial body
         if (RequiresCelestialBodyOrigin() && !(sp->IsOfType("CelestialBody")))
         {
            CoordinateSystemException cse;
            cse.SetDetails(errorMessageFormat.c_str(),
                           name.c_str(), "Origin", "Celestial Body");
            throw cse;
         }
         #ifdef DEBUG_SET_REF
         MessageInterface::ShowMessage
            ("   Setting origin <%p>'%s' to <%p>'%s'\n", origin,
             origin ? origin->GetName().c_str() : "NULL", sp, sp->GetName().c_str());
         #endif
         origin = sp;
      }

      #ifdef DEBUG_SET_REF
      MessageInterface::ShowMessage
         ("   origin = <%p>'%s'\n", origin, origin ? origin->GetName().c_str() : "NULL");
      #endif
      
      // J2000Body must be a celestial body
      if (name == j2000BodyName)
      {
         if (!(sp->IsOfType("CelestialBody")))
         {
            CoordinateSystemException cse;
            cse.SetDetails(errorMessageFormat.c_str(),
                           name.c_str(), "J2000Body", "Celestial Body");
            throw cse;
         }
         #ifdef DEBUG_SET_REF
         MessageInterface::ShowMessage
            ("   Setting j2000Body <%p>'%s' to <%p>'%s'\n", j2000Body,
             j2000Body ? j2000Body->GetName().c_str() : "NULL", sp, sp->GetName().c_str());
         #endif
         j2000Body = sp;
      }
      
      #ifdef DEBUG_SET_REF
      MessageInterface::ShowMessage
         ("   j2000Body = <%p>'%s'\n", j2000Body, j2000Body ? j2000Body->GetName().c_str() : "NULL");
      #endif
      
      if (j2000Body == NULL)
         return false;
      
      if (origin != NULL)
      {
         if (name == origin->GetJ2000BodyName())
         {
            #ifdef DEBUG_SET_REF
            MessageInterface::ShowMessage
               ("   Setting J2000Body of origin <%p>'%s' to <%p>'%s'\n", origin, origin->GetName().c_str(),
                sp, sp->GetName().c_str());
            #endif
            origin->SetJ2000Body(sp);
         }
      }
      
      #ifdef DEBUG_SET_REF
      MessageInterface::ShowMessage("   solar = <%p>\n", solar);
      #endif
      
      if (solar == NULL)
         return false;
      
      // Set J2000Body for other reference objects
      SetJ2000BodyForOtherRefObjects();
      
      #ifdef DEBUG_SET_REF
      MessageInterface::ShowMessage("CoordinateBase::SetRefObject() returning true\n");
      #endif
      return true;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   #ifdef DEBUG_SET_REF
   MessageInterface::ShowMessage
      ("CoordinateBase::SetRefObject() returning GmatBase::SetRefObject()\n");
   #endif
   return GmatBase::SetRefObject(obj, type, name);
}

//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames the specified reference object to the specified new name.
 *
 * @param type type of the reference object to rename
 * @param oldName old name of the object
 * @param newName new name of the object
 *
 * @return true if rename is successful or not necessary because the
 *         axis system does not contain a reference to the object;
 *         false otherwise
 */
//---------------------------------------------------------------------------
bool CoordinateBase::RenameRefObject(const UnsignedInt type,
                                     const std::string &oldName,
                                     const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("CoordinateBase::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif

   if (originName == oldName)
      originName = newName;

   return true;
}


//------------------------------------------------------------------------------
// void SetJ2000BodyForOtherRefObjects()
//------------------------------------------------------------------------------
void CoordinateBase::SetJ2000BodyForOtherRefObjects()
{
   if (UsesPrimary())
   {
      //SpacePoint* primaryObject = GetPrimaryObject();
      std::string primaryName = GetPrimaryObjectName();
      SpacePoint* primaryObject = solar->GetBody(primaryName);
      #ifdef DEBUG_SET_REF
      MessageInterface::ShowMessage
         ("   primaryName = '%s', primaryObject = <%p>'%s'\n", primaryName.c_str(),
          primaryObject, primaryObject ? primaryObject->GetName().c_str() : "NULL");
      #endif
      if (primaryObject != NULL)
      {
         std::string j2000BodyName = primaryObject->GetJ2000BodyName();
         SpacePoint *j2000Body = solar->GetBody(j2000BodyName);
         if (j2000Body != NULL)
         {
            #ifdef SET_REF_OBJ
            MessageInterface::ShowMessage
               ("   Setting J2000Body of primaryObject <%p>'%s' to <%p>'%s'\n", primaryObject,
                primaryObject->GetName().c_str(), j2000Body, j2000Body->GetName().c_str());
            #endif
            primaryObject->SetJ2000Body(j2000Body);
         }
      }
   }
   else
   {
      #ifdef SET_REF_OBJ
      MessageInterface::ShowMessage("   Not using primary object\n");
      #endif
   }
   
   if (UsesSecondary())
   {
      //SpacePoint* secondaryObject = GetSecondaryObject();
      std::string secondaryName = GetSecondaryObjectName();
      SpacePoint* secondaryObject = solar->GetBody(secondaryName);
      #ifdef DEBUG_SET_REF
      MessageInterface::ShowMessage
         ("   secondaryName = '%s', secondaryObject = <%p>'%s'\n", secondaryName.c_str(),
          secondaryObject, secondaryObject ? secondaryObject->GetName().c_str() : "NULL");
      #endif
      if (secondaryObject != NULL)
      {
         std::string j2000BodyName = secondaryObject->GetJ2000BodyName();
         SpacePoint *j2000Body = solar->GetBody(j2000BodyName);
         if (j2000Body != NULL)
         {
            #ifdef DEBUG_SET_REF
            MessageInterface::ShowMessage
               ("   Setting J2000Body of secondaryObject <%p>'%s' to <%p>'%s'\n", secondaryObject,
                secondaryObject->GetName().c_str(), j2000Body, j2000Body->GetName().c_str());
            #endif
            secondaryObject->SetJ2000Body(j2000Body);
         }
      }
   }
   else
   {
      #ifdef DEBUG_SET_REF
      MessageInterface::ShowMessage("   Not using secondary object\n");
      #endif
   }
   
   if (UsesReferenceObject())
   {
      //SpacePoint* refObject = GetReferenceObject();
      std::string refObjName = GetReferenceObjectName();
      SpacePoint* refObject = solar->GetBody(refObjName);
      #ifdef DEBUG_SET_REF
      MessageInterface::ShowMessage
         ("   refObjName = '%s', refObject = <%p>'%s'\n", refObjName.c_str(), refObject,
          refObject ? refObject->GetName().c_str() : "NULL");
      #endif
      if (refObject != NULL)
      {
         std::string j2000BodyName = refObject->GetJ2000BodyName();
         SpacePoint *j2000Body = solar->GetBody(j2000BodyName);
         if (j2000Body != NULL)
         {
            #ifdef DEBUG_SET_REF
            MessageInterface::ShowMessage
               ("   Setting J2000Body of refObject <%p>'%s' to <%p>'%s'\n", refObject,
                refObject->GetName().c_str(), j2000Body, j2000Body->GetName().c_str());
            #endif
            refObject->SetJ2000Body(j2000Body);
         }
      }
   }
   else
   {
      #ifdef DEBUG_SET_REF
      MessageInterface::ShowMessage("   Not using reference object\n");
      #endif
   }
   
   #ifdef DEBUG_SET_REF
   MessageInterface::ShowMessage("CoordinateBase::SetRefObject() leaving\n");
   #endif
}

