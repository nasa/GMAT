//$Id$
//------------------------------------------------------------------------------
//                                 ManageObject
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
// Created: 2008.03.12
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
/**
 * Class implementation for the ManageObject command
 */
//------------------------------------------------------------------------------


#include "ManageObject.hpp"
#include "MessageInterface.hpp"
#include "CommandException.hpp"
#include "Array.hpp"
#include "StringUtil.hpp"           // for ParseParameter()

//#define DEBUG_MANAGE_OBJECT

//---------------------------------
// static data
//---------------------------------
const std::string
ManageObject::PARAMETER_TEXT[ManageObjectParamCount - GmatCommandParamCount] =
{
   "ObjectNames",
};

const Gmat::ParameterType
ManageObject::PARAMETER_TYPE[ManageObjectParamCount - GmatCommandParamCount] =
{
   Gmat::STRINGARRAY_TYPE,   //"ObjectNames",
};

//------------------------------------------------------------------------------
// ManageObject()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ManageObject::ManageObject(const std::string &typeStr) :
   GmatCommand(typeStr)
{
}


//------------------------------------------------------------------------------
// ~ManageObject()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ManageObject::~ManageObject()
{
}


//------------------------------------------------------------------------------
// ManageObject(const ManageObject &mo)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <mo> The instance that gets copied.
 */
//------------------------------------------------------------------------------
ManageObject::ManageObject(const ManageObject &mo) :
   GmatCommand(mo),
   objectNames   (mo.objectNames)      
{
}


//------------------------------------------------------------------------------
// ManageObject& operator=(const ManageObject &mo)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <mo> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ManageObject& ManageObject::operator=(const ManageObject &mo)
{
   if (&mo != this)
   {
      GmatCommand::operator=(mo);
      objectNames = mo.objectNames;
   }
   
   return *this;
}

// Parameter access methods - overridden from GmatBase
//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string ManageObject::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < ManageObjectParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   return GmatCommand::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer ManageObject::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < ManageObjectParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   
   return GmatCommand::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType ManageObject::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < ManageObjectParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];
      
   return GmatCommand::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string ManageObject::GetParameterTypeString(const Integer id) const
{
   return GmatCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string ManageObject::GetStringParameter(const Integer id) const
{
   return GmatCommand::GetStringParameter(id);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID and the index into the array.
 *
 * @param <id> ID for the requested parameter.
 * @param <index> index into the StringArray parameter.
 *
 * @exception <CommandException> thrown if value is out of range
 *
 * @return  string value at index 'index'.
 *
 */
//------------------------------------------------------------------------------
std::string ManageObject::GetStringParameter(const Integer id,
                                             const Integer index) const
{
   if (id == OBJECT_NAMES)
   {
      if ((index < 0) || (index >= ((Integer) objectNames.size())))
         throw CommandException(
               "Index out of bounds when attempting to return object name\n");
      return objectNames.at(index);
    }
   return GmatCommand::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const std::string &label, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * string label and the index into the array.
 *
 * @param <label> label for the requested parameter.
 * @param <index> index into the StringArray parameter.
 *
 * @return  string value at index 'index'.
 *
 */
//------------------------------------------------------------------------------
std::string ManageObject::GetStringParameter(const std::string &label,
                                             const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @exception <CommandException> thrown if value is already in the list.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool ManageObject::SetStringParameter(const Integer id, 
                                      const std::string &value)
{
   if (id == OBJECT_NAMES)
   {
      Integer sz = objectNames.size();
      for (Integer ii = 0; ii < sz; ii++)
         if (objectNames[ii] == value)
         {
            std::string ex = "Attempting to add """;
            ex += value + """ more than once to list of objects.\n";
            throw CommandException(ex);
         }
      objectNames.push_back(value);
      return true;
    }
   return GmatCommand::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const std::string &label, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @exception <CommandException> thrown if value is already in the list.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool ManageObject::SetStringParameter(const std::string &label, 
                                      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
bool ManageObject::SetStringParameter(const Integer id, 
                                      const std::string &value,
                                      const Integer index)
{
   if (id == OBJECT_NAMES)
   {
      Integer sz = objectNames.size();
      if (index < 0)
         throw CommandException(
               "Index of object name array out of bounds for ManageObject command.\n");
      if (index > sz)
         throw CommandException(
               "Missing elements in Object Name list for ManageObject command.\n");
      // push it onto the end of the list
      if (index == sz) objectNames.push_back(value);
      // or replace one of the already-existing names
      else
         objectNames.at(index) = value;
      return true;
   }
   return GmatCommand::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
bool ManageObject::SetStringParameter(const std::string &label, 
                                      const std::string &value,
                                      const Integer index)
{
   return SetStringParameter(GetParameterID(label),value,index);
}

//------------------------------------------------------------------------------
//  const StringArray&  GetStringArrayParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * This method returns the string array value, given the input
 * parameter ID .
 *
 * @param <id> ID for the requested parameter.
 *
  * @return  string array.
 *
 */
//------------------------------------------------------------------------------
const StringArray& 
ManageObject::GetStringArrayParameter(const Integer id) const
{
   if (id == OBJECT_NAMES)
   {
      return objectNames;
   }
   return GmatCommand::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * @see GmatCommand
 */
//------------------------------------------------------------------------------
const std::string& ManageObject::GetGeneratingString(Gmat::WriteMode mode,
																	  const std::string &prefix,
																	  const std::string &useName)
{
   // Build the local string
   generatingString = prefix + "Global";
   for (StringArray::iterator i = objectNames.begin(); i != objectNames.end(); ++i)
      generatingString += " " + *i;
   generatingString += ";";
	
   // Then call the base class method
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool ManageObject::TakeAction(const std::string &action, const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("ManageObject::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      objectNames.clear();
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Method that initializes the internal data structures.
 * 
 * @return true if initialization succeeds.
 */
//------------------------------------------------------------------------------
bool ManageObject::Initialize()
{
   #ifdef DEBUG_MANAGE_OBJECT
      MessageInterface::ShowMessage("ManageObject::Initialize() entered\n");
   #endif
      
   GmatCommand::Initialize();
   Integer numNames = (Integer) objectNames.size();
   if (numNames <= 0)
   {
      std::string ex = "No objects listed for " + GetTypeName() + " command.\n";
      throw CommandException(ex);
   }
   return true;
}


//------------------------------------------------------------------------------
// bool InsertIntoGOS(GmatBase *obj, const std::string &withName)
//------------------------------------------------------------------------------
bool ManageObject::InsertIntoGOS(GmatBase *obj, const std::string &withName)
{
   #ifdef DEBUG_MANAGE_OBJECT
   MessageInterface::ShowMessage
      ("ManageObject::InsertIntoGOS() entered, obj=<%p>, name='%s', IsGlobal=%d\n",
       obj, withName.c_str(), obj->IsGlobal());
   #endif
   if (!obj)
   {
      std::string errMsg = "ManageObject::InsertIntoGOS() passed a NULL object\n";
      throw CommandException(errMsg);
   }
   GmatBase *mapObj;
   std::string ex;
   ////std::string objType = obj->GetTypeName();
   UnsignedInt objType = obj->GetType();
   // if it is already in the GOS, make sure the types match
   if (globalObjectMap->find(withName) != globalObjectMap->end())
   {
      mapObj = (*globalObjectMap)[withName];
      if (!mapObj->IsOfType(objType))
      {
         ex = "Object of name """ + withName;
         ex += """, but of a different type, already exists in Global Object Store\n";
         throw CommandException(ex);
      }
      ////if (objType == "Array")
      if (objType == Gmat::PARAMETER && obj->GetTypeName() == "Array")
      {
         Integer r1, r2, c1, c2;
         ((Array*) mapObj)->GetSize(r1, c1);
         ((Array*) obj)->GetSize(r2, c2);
         if ((r1 != r2) || (c1 != c2))
         {
            ex = "Array of name """ + withName;
            ex += """, but with different dimensions already exists in Global Object Store\n";
            throw CommandException(ex);
         }
      }
      
      // This Warning is very annoying when GmatFunction is running in a loop,
      // so defined macro here (loj: 2008.10.08)
      #ifdef __SHOW_GOS_WARNING__
      ex = "ManageObject::InsertIntoGOS() Cannot add more than "
         "one object with name \"";
      ex += withName + "\" to the Global Object Store";
      MessageInterface::ShowMessage("*** WARNING *** " + ex + ", So ignored.\n");
      // Let's just ignore for now to run TargetHohmannTransfer.gmf (loj: 2008.06.05) 
      //throw CommandException(ex);
      #endif
      
      // it is already in there, so we do not need to put this one in; clean it up
      #ifdef DEBUG_MANAGE_OBJECT
      MessageInterface::ShowMessage
         ("   object %s was already in object store ...\n", withName.c_str());
      MessageInterface::ShowMessage
         ("   pointer for obj = <%p> and pointer for mapObj = <%p>\n", obj, mapObj);
      #endif
      if (mapObj != obj) 
      {
         #ifdef DEBUG_MANAGE_OBJECT
         MessageInterface::ShowMessage
            ("ManageObject::InsertIntoGOS() returning false, object<%p> is not "
             "the same as mapObj<%p>\n", obj, mapObj);
         #endif
         return false;
      }
   }
   else
   {
      #ifdef DEBUG_MANAGE_OBJECT
      MessageInterface::ShowMessage
         ("   Inserting <%p>'%s' into globalObjectMap and setting global flag to true\n",
          obj, obj->GetName().c_str());
      #endif
      // put it into the globalObjectMap
      globalObjectMap->insert(std::make_pair(withName,obj));
      // Set global flag to true (LOJ: 2015.07.16)
      obj->SetIsGlobal(true);
   }
   
   #ifdef DEBUG_MANAGE_OBJECT
   MessageInterface::ShowMessage("   Is object global: %s\n", obj->IsGlobal() ? "yes" : "no");
   MessageInterface::ShowMessage("   Checking if Parameter should be moved to globalObjectMap\n");
   #endif
   
   // Check for automatic Parameters such as globalSat.x 
   // If Parameter owner is global, make Parameter global and move it to globalObjectMap
   // (LOJ: 2015.07.20)
   StringArray movedObjects;
   for (ObjectMap::iterator iter = objectMap->begin(); iter != objectMap->end(); ++iter)
   {
      std::string objName = iter->first;
      GmatBase *obj = iter->second;
      
      // Check if name indicates a Parameter (i.e it has dot)
      if (objName.find_first_of('.') != objName.npos)
      {
         #ifdef DEBUG_MANAGE_OBJECT
         MessageInterface::ShowMessage("  ==> '%s' is a Parameter\n", objName.c_str());
         #endif
         
         // Check if Parameter owner is global after verifying it doesn't have a
         // local copy (i.e. in a function)
         std::string type, ownerName, dep;
         GmatStringUtil::ParseParameter(objName, type, ownerName, dep);
         if (objectMap->find(ownerName) != objectMap->end())
            continue;
         else if (globalObjectMap->find(ownerName) != globalObjectMap->end())
         {
            // If it is not already in the GOS, add it
            if (globalObjectMap->find(objName) == globalObjectMap->end())
            {
               #ifdef DEBUG_MANAGE_OBJECT
               MessageInterface::ShowMessage
                  ("   Inserting <%p>'%s' into globalObjectMap and setting global flag to true\n",
                   obj, obj->GetName().c_str());
               #endif
               // Put it into the GOS
               globalObjectMap->insert(std::make_pair(withName,obj));
               // Set global flag to true
               obj->SetIsGlobal(true);
               // Add it to movedObjects
               movedObjects.push_back(objName);
            }
         }
      }
   }
   
   // Remove from the objectMap
   for (unsigned int ii = 0; ii < movedObjects.size(); ii++)
      objectMap->erase(movedObjects.at(ii));
   movedObjects.clear();  
   
   #ifdef DEBUG_MANAGE_OBJECT
   MessageInterface::ShowMessage("ManageObject::InsertIntoGOS() returning true\n");
   #endif
   return true;
}
