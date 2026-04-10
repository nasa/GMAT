//$Id$
//------------------------------------------------------------------------------
//                                UpdateDynamicData
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Created: 2017/01/24, Joshua Raymond, Thinking Systems, Inc.
//
/**
* Defines UpdateDynamicData methods
*/
//------------------------------------------------------------------------------
#include "UpdateDynamicData.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
UpdateDynamicData::PARAMETER_TEXT[UpdateDynamicDataParamCount - GmatCommandParamCount] =
{
   "DynamicDataDisplay",
   "AddUpdateData",
   "AvailableParams"
};

const Gmat::ParameterType
UpdateDynamicData::PARAMETER_TYPE[UpdateDynamicDataParamCount - GmatCommandParamCount] =
{
   Gmat::STRING_TYPE,       // "DynamicDataDisplay"
   Gmat::OBJECTARRAY_TYPE,  // "AddUpdateData"
   Gmat::STRINGARRAY_TYPE   // "AvailableParams"
};

//------------------------------------------------------------------------------
// UpdateDynamicData
//------------------------------------------------------------------------------
/**
* The constructor
*/
//------------------------------------------------------------------------------
UpdateDynamicData::UpdateDynamicData() :
   GmatCommand("UpdateDynamicData"),
   dynamicData(NULL),
   dynamicTableName("")
{
   // GmatBase data
   objectTypeNames.push_back("SubscriberCommand");
   objectTypeNames.push_back("UpdateDynamicData");
}

//------------------------------------------------------------------------------
// UpdateDynamicData(const UpdateDynamicData &updateData)
//------------------------------------------------------------------------------
/**
* The copy constructor
*/
//------------------------------------------------------------------------------
UpdateDynamicData::UpdateDynamicData(const UpdateDynamicData &updateData) :
   GmatCommand(updateData),
   dynamicData(updateData.dynamicData),
   dynamicTableName(updateData.dynamicTableName)
{
   dataToUpdate = updateData.dataToUpdate;
}

//------------------------------------------------------------------------------
// UpdateDynamicData& operator=(const UpdateDynamicData &updateData)
//------------------------------------------------------------------------------
/**
* The assignment operator
*/
//------------------------------------------------------------------------------
UpdateDynamicData& UpdateDynamicData::operator=(const UpdateDynamicData &updateData)
{
   if (this != &updateData)
   {
      dynamicData = NULL;
      dynamicTableName = updateData.dynamicTableName;
      dataToUpdate = updateData.dataToUpdate;
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~UpdateDynamicData()
//------------------------------------------------------------------------------
/**
* The destructor
*/
//------------------------------------------------------------------------------
UpdateDynamicData::~UpdateDynamicData()
{

}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
* This method returns a clone of this Report.
*
* @return clone of the Report.
*/
//------------------------------------------------------------------------------
GmatBase* UpdateDynamicData::Clone() const
{
   return (new UpdateDynamicData(*this));
}

//------------------------------------------------------------------------------
//bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                  const std::string &name,
//                  const Integer index)
//------------------------------------------------------------------------------
bool UpdateDynamicData::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index)
{
   if (obj == NULL)
   {
#ifdef DEBUG_UPDATE_OBJ
      MessageInterface::ShowMessage
         ("\nUpdateDynamicData::SetRefObject() this=<%p> returning false since input object is NULL\n");
#endif
      return false;
   }

#ifdef DEBUG_UPDATE_OBJ
   MessageInterface::ShowMessage
      ("\nUpdateDynamicData::SetRefObject() this=<%p> entered, obj=<%p><%s>'%s'\n   type=%d<%s>, "
      "name='%s', index=%d\n", this, obj, obj->GetTypeName().c_str(), obj->GetName().c_str(),
      type, GetObjectTypeString(type).c_str(), name.c_str(), index);
#endif

   if (type == Gmat::SUBSCRIBER)
   {
      if (obj->GetTypeName() != "DynamicDataDisplay")
         throw CommandException("UpdateDynamicDisplay command must have a "
         "DynamicDataDisplay name as the first parameter.\n");

      dynamicTableName = name;
      dynamicData = (DynamicDataDisplay*)obj;
      dynamicDataStruct = dynamicData->GetDynamicDataStruct();
      for (Integer i = 0; i < dynamicDataStruct.size(); ++i)
      {
         for (Integer j = 0; j < dynamicDataStruct[i].size(); ++j)
            availableParams.push_back(dynamicDataStruct[i][j].paramName);
      }
      return true;
   }

   return GmatCommand::SetRefObject(obj, type, name, index);
}

//---------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//                      const std::string &oldName,
//                      const std::string &newName)
//---------------------------------------------------------------------------
/*
* Renames referenced objects
*
* @param <type> type of the reference object.
* @param <oldName> old name of the reference object.
* @param <newName> new name of the reference object.
*
* @return always true to indicate RenameRefObject() was implemented.
*/
//---------------------------------------------------------------------------
bool UpdateDynamicData::RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName)
{
   // Check for allowed object types for name change.
   if (type != Gmat::SUBSCRIBER && type != Gmat::PARAMETER &&
      type != Gmat::SPACECRAFT && type != Gmat::COORDINATE_SYSTEM &&
      type != Gmat::PLATE && 
      type != Gmat::BURN && type != Gmat::IMPULSIVE_BURN &&
      type != Gmat::CALCULATED_POINT && type != Gmat::HARDWARE)
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("UpdateDynamicData::RenameRefObject() returning true, no action is required\n");
      #endif

      return true;
   }

   if (type == Gmat::SUBSCRIBER)
   {
      if (dynamicTableName == oldName)
         dynamicTableName = newName;
   }
   else
   {
      for (Integer i = 0; i < dataToUpdate.size(); ++i)
      {
         if (dataToUpdate[i].find(oldName) != oldName.npos)
         {
            dataToUpdate[i] =
               GmatStringUtil::ReplaceName(dataToUpdate[i], oldName, newName);
         }
      }
   }

   return true;
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
* Accesses arrays of names for referenced objects.
*
* @param type Type of object requested.
*
* @return the StringArray containing the referenced object names.
*/
//------------------------------------------------------------------------------
const StringArray& UpdateDynamicData::GetRefObjectNameArray(const UnsignedInt type)
{
   static StringArray refObjectNames;
   refObjectNames.clear();

   switch (type)
   {
   case Gmat::SUBSCRIBER:
      refObjectNames.push_back(dynamicTableName);
      return refObjectNames;
   default:
      return GmatCommand::GetRefObjectNameArray(type);
   }
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer UpdateDynamicData::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < UpdateDynamicDataParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }

   return GmatCommand::GetParameterID(str);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string UpdateDynamicData::GetStringParameter(const Integer id) const
{
   if (id == DYNAMIC_DATA_DISPLAY)
      return dynamicTableName;

   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string UpdateDynamicData::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool UpdateDynamicData::SetStringParameter(const Integer id,
                                           const std::string &value)
{
   switch (id)
   {
   case DYNAMIC_DATA_DISPLAY:
      dynamicTableName = value;
      return true;
   case ADD_UPDATE_DATA:
      dataToUpdate.push_back(value);
      return true;
   default:
      return GmatCommand::SetStringParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool UpdateDynamicData::SetStringParameter(const std::string &label,
                                           const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// const std::string& UpdateDynamicData::GetGeneratingString(Gmat::WriteMode mode,
//    const std::string &prefix,
//    const std::string &useName)
//------------------------------------------------------------------------------
const std::string& UpdateDynamicData::GetGeneratingString(Gmat::WriteMode mode,
   const std::string &prefix,
   const std::string &useName)
{
   if (dynamicTableName != "")
      generatingString = prefix + "UpdateDynamicData " + dynamicTableName;
   else
      generatingString = prefix + "UpdateDynamicData";

   for (Integer i = 0; i < dataToUpdate.size(); ++i)
      generatingString += " " + dataToUpdate[i];

   generatingString += ";";

   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}

//------------------------------------------------------------------------------
//const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& UpdateDynamicData::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case AVAILABLE_PARAMS:
      return availableParams;
   default:
      return GmatCommand::GetStringArrayParameter(id);
   }
}

//------------------------------------------------------------------------------
//const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& UpdateDynamicData::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
* Parses the command string and builds the corresponding command structures.
*
* The UpdateDynamicDisplay command has the following syntax:
*
*     UpdateDynamicDisplay DynamicDataDisplay Param1 Param2...;
*
* where DynamicDataDisplay is the name of the DynamicDataDisplay object that
* will be updated.  Parameter names added after that (i.e. Param1, Param2, ...)
* can be listed to only update specified parameters in the selected
* DynamicDataDisplay.  These parameters must have already been added to the
* DynamicDataDisplay object in its creation.
*
* @return true on successful parsing of the command.
*/
//------------------------------------------------------------------------------
bool UpdateDynamicData::InterpretAction()
{
   TextParser tp;
   StringArray chunks = tp.SeparateSpaces(generatingString);

   // Clear the old data
   objects.clear();
   dataToUpdate.clear();
   dynamicTableName = "";

   // Set the new data
   if (chunks.size() > 1)
   {
      SetStringParameter(DYNAMIC_DATA_DISPLAY, chunks[1]);
      for (Integer i = 2; i < chunks.size(); ++i)
      {
         if (i == chunks.size() - 1)
         {
            if (chunks[i].back() == ';')
               chunks[i].erase(chunks[i].end() - 1, chunks[i].end());
         }
         SetStringParameter(ADD_UPDATE_DATA, chunks[i]);
      }

      // Add DynamicDataDisplay to reference objects
      objects.push_back(chunks[1]);
   }
   
   return true;
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
* Method to initialize this command and set a pointer to the corresponding
* DynamicDataDisplay this command is referring to
*/
//------------------------------------------------------------------------------
bool UpdateDynamicData::Initialize()
{
   if (GmatCommand::Initialize() == false)
      return false;

   GmatBase *mapObj;

   if ((mapObj = FindObject(dynamicTableName)) == NULL)
      throw CommandException(
      "UpdateDynamicData command cannot find a DynamicDataDisplay named \"" +
      (dynamicTableName)+"\"\n");

   dynamicData = (DynamicDataDisplay *)mapObj;

   if (!dynamicData->IsOfType("DynamicDataDisplay"))
      throw CommandException(
      "The object named \"" + dynamicTableName + "\" is not a "
      "DynamicDataDisplay, UpdateDynamicData cannot execute\n");

   dynamicDataStruct = dynamicData->GetDynamicDataStruct();

   if (dataToUpdate.size() > 0)
   {
      StringArray incorrectNames;
      for (Integer updateIdx = 0; updateIdx < dataToUpdate.size(); ++updateIdx)
      {
         bool matchFound = false;
         bool testAltName = false;
         StringArray separatedName =
            GmatStringUtil::SeparateDots(dataToUpdate[updateIdx]);
         if (separatedName.size() > 2)
            testAltName = true;
         for (Integer i = 0; i < dynamicDataStruct.size(); ++i)
         {
            for (Integer j = 0; j < dynamicDataStruct[i].size(); ++j)
            {
               // If there is only one dot, just look for the same paramater
               // name
               if (!testAltName)
               {
                  StringArray separatedDataName =
                     GmatStringUtil::SeparateDots(dynamicDataStruct[i][j].paramName);
                  if (dataToUpdate[updateIdx] ==
                     dynamicDataStruct[i][j].paramName)
                  {
                     matchFound = true;
                     break;
                  }
                  else if (separatedDataName.size() > 2)
                  {
                     if (separatedDataName[1] == "EarthMJ2000Eq" &&
                        dataToUpdate[updateIdx] == (separatedDataName[0]
                        + "." + separatedDataName[2]))
                     {
                        dataToUpdate[updateIdx] = separatedName[0] +
                           ".EarthMJ2000Eq." + separatedName[1];
                        matchFound = true;
                        break;
                     }
                     else if (separatedDataName[1] == "Earth" &&
                        dataToUpdate[updateIdx] == (separatedDataName[0]
                        + "." + separatedDataName[2]))
                     {
                        dataToUpdate[updateIdx] = separatedName[0] +
                           ".Earth." + separatedName[1];
                        matchFound = true;
                        break;
                     }
                  }
               }
               // If multiple dots were used, check the name and the dependent
               // object if needed in case the correct parameter was used, just
               // with a different name
               else
               {
                  if (dataToUpdate[updateIdx] ==
                     dynamicDataStruct[i][j].paramName)
                  {
                     matchFound = true;
                     break;
                  }
                  else if (separatedName[0] + "." + separatedName[2] ==
                     dynamicDataStruct[i][j].paramName && separatedName[1] ==
                     dynamicDataStruct[i][j].paramRef->GetStringParameter("DepObject"))
                  {
                     dataToUpdate[updateIdx] =
                        separatedName[0] + "." + separatedName[2];
                     matchFound = true;
                     break;
                  }
               }
               
            }
            if (matchFound)
               break;
         }
         if (!matchFound)
            incorrectNames.push_back(dataToUpdate[updateIdx]);
      }

      if (incorrectNames.size() > 0)
      {
         std::string errMsg = "Cannot find parameter(s) [";
         for (Integer i = 0; i < incorrectNames.size(); ++i)
            errMsg += " " + incorrectNames[i];
         errMsg += " ] in object \"" + dynamicTableName + "\"\n";

         throw CommandException(errMsg);
      }
   }

   return true;
}

//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
* Update the data of the selected DynamicDataDisplay
*/
//------------------------------------------------------------------------------
bool UpdateDynamicData::Execute()
{
   return dynamicData->UpdateData(dataToUpdate);
}
