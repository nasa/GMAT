//$Id$
//------------------------------------------------------------------------------
//                                 Global
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
// Author: Wendy C. Shoan
// Created: 2008.03.14
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
/**
 * Class implementation for the Global command
 */
//------------------------------------------------------------------------------


#include "Global.hpp"
#include "MessageInterface.hpp"
#include "CommandException.hpp"

//#define DEBUG_GLOBAL
//#define DEBUG_RENAME

//---------------------------------
// static data
//---------------------------------
//const std::string
//Global::PARAMETER_TEXT[GlobalParamCount - ManageObjectParamCount] =
//{
//};

//const Gmat::ParameterType
//Global::PARAMETER_TYPE[GlobalParamCount - ManageObjectParamCount] =
//{
//};

//------------------------------------------------------------------------------
// Global()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
Global::Global() :
   ManageObject("Global")
{
   objectTypeNames.push_back("Global");
}


//------------------------------------------------------------------------------
// ~Global()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Global::~Global()
{
}


//------------------------------------------------------------------------------
// Global(const Global &gl)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <gl> The instance that gets copied.
 */
//------------------------------------------------------------------------------
Global::Global(const Global &gl) :
   ManageObject(gl)
{
}


//------------------------------------------------------------------------------
// Global& operator=(const Global &gl)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <gl> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Global& Global::operator=(const Global &gl)
{
   if (&gl != this)
   {
      ManageObject::operator=(gl);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Override of the GmatBase clone method.
 * 
 * @return A new copy of this instance.
 */
//------------------------------------------------------------------------------
GmatBase* Global::Clone() const
{
   return new Global(*this);
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
bool Global::Initialize()
{
   #ifdef DEBUG_GLOBAL
      MessageInterface::ShowMessage("Global::Initialize() entered\n");
   #endif
      
   ManageObject::Initialize();
   
   // Set global flag in Initialize() so that GmatFunction can search global object before
   // Global command is executed (LOJ: 2015.08.26)
   Integer sz = objectNames.size();
   GmatBase *obj = NULL;
   for (Integer i = 0; i < sz; i++)
   {
      obj = FindObject(objectNames.at(i));
      #ifdef DEBUG_GLOBAL
      MessageInterface::ShowMessage(WriteObjectInfo("   ", obj));
      #endif
      if (obj)
      {
         obj->SetIsGlobal(true);
         #ifdef DEBUG_GLOBAL
         MessageInterface::ShowMessage(WriteObjectInfo("   ", obj));
         #endif
      }
   }
   
   return true;
}


//---------------------------------------------------------------------------
//  bool Execute()
//---------------------------------------------------------------------------
/**
 * The method that is fired to perform this Global command.
 *
 * @return true if the Global runs to completion, false if an error
 *         occurs.
 */
//---------------------------------------------------------------------------
bool Global::Execute()
{
   #ifdef DEBUG_GLOBAL
      MessageInterface::ShowMessage("Global::Execute() entered\n");
   #endif
   GmatBase    *mapObj;
   std::string ex;
   Integer     sz = objectNames.size();
   for (Integer ii=0; ii<sz; ii++)
   {
      // get it from the LOS, if it's there
      if (objectMap->find(objectNames.at(ii)) != objectMap->end())
      {
         mapObj = (*objectMap)[objectNames.at(ii)];
         #ifdef DEBUG_GLOBAL
			MessageInterface::ShowMessage
				("   Found <%p>'%s' in objectMap, so moving to GOS\n", mapObj,
				 objectNames.at(ii).c_str());
         #endif
         if (InsertIntoGOS(mapObj, objectNames.at(ii)))
            objectMap->erase(objectNames.at(ii));
      }
      else if (globalObjectMap->find(objectNames.at(ii)) != globalObjectMap->end())
      {
         mapObj = (*globalObjectMap)[objectNames.at(ii)];
         #ifdef DEBUG_GLOBAL
			MessageInterface::ShowMessage
				("   Found <%p>'%s' in globalObjctMap, make sure type's matching\n", mapObj,
				 objectNames.at(ii).c_str());
         #endif
         InsertIntoGOS(mapObj, objectNames.at(ii));
      }
      else
      {
         ex = "Global::Execute - object of name \"" + objectNames.at(ii);
         ex += "\" not found.\n";
         throw CommandException(ex);
      }
   }
   
   // Build command summary
   BuildCommandSummary(true);
   
   #ifdef DEBUG_GLOBAL
      MessageInterface::ShowMessage("Global::Execute() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * This method updates object names when the user changes them.
 *
 * @param type Type of object that is renamed.
 * @param oldName Old name for the object.
 * @param newName New name for the object.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Global::RenameRefObject(const UnsignedInt type,
                             const std::string &oldName,
                             const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Global::RenameRefObject() entered, type=%d, oldName='%s', newName='%s'\n",
       type, oldName.c_str(), newName.c_str());
   #endif
   
   for (Integer index = 0; index < (Integer)objectNames.size(); ++index)
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("objectName[%d] = '%s'\n", index, objectNames[index].c_str());
      #endif
      
      if (objectNames[index] == oldName)
      {
         objectNames[index] = newName;
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            (">>> changed objectName[%d] to '%s'\n", index, objectNames[index].c_str());
         #endif
         break;
      }
      else if (type == Gmat::PROP_SETUP)
      {
         // Handle default force model rename
         std::string oldFmName = oldName + "_ForceModel";
         if (objectNames[index] == oldFmName)
         {
            objectNames[index] = newName + "_ForceModel";
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               (">>> changed objectName[%d] to '%s'\n", index, objectNames[index].c_str());
            #endif
            break;
         }
      }
   }
   
   return true;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
const StringArray& Global::GetRefObjectNameArray(const UnsignedInt type)
{
   return objectNames;
}


