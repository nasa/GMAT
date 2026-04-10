//------------------------------------------------------------------------------
//                                  Save
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
// Author: Darrel J. Conway
// Created: 2004/02/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class implementation for the save command
 */
//------------------------------------------------------------------------------

#include "Save.hpp"
#include "FileManager.hpp"      // for GetPathname()
#include "MessageInterface.hpp"
#include "GmatGlobal.hpp"       // for GetDataPrecision()

// for writing to single file per Save command, if not defined
// it will create files per Save objects
#define __USE_SINGLE_FILE__

//#define DEBUG_SAVE_INIT
//#define DEBUG_SAVE_EXEC
//#define DEBUG_SAVE_OUTPUT

//---------------------------------
//  static data
//---------------------------------
const std::string
Save::PARAMETER_TEXT[SaveParamCount - GmatCommandParamCount] = 
{
   "ObjectNames",
};

const Gmat::ParameterType
Save::PARAMETER_TYPE[SaveParamCount - GmatCommandParamCount] =
{
   Gmat::STRINGARRAY_TYPE,   // "ObjectNames",
};


//------------------------------------------------------------------------------
// Save()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
Save::Save() :
   GmatCommand   ("Save"),
   appendData    (false),
   wasWritten    (false),
   writeVerbose  (false)
{
   fileArray = NULL;
}


//------------------------------------------------------------------------------
// ~Save()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Save::~Save()
{
   if (fileArray)
      delete [] fileArray;
}


//------------------------------------------------------------------------------
// Save(const Save& sv)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param sv The instance that is copied.
 */
//------------------------------------------------------------------------------
Save::Save(const Save& sv) :
   GmatCommand   (sv),
   fileNameArray (sv.fileNameArray),
   appendData    (sv.appendData),     // should be false...
   wasWritten    (sv.wasWritten),
   objNameArray  (sv.objNameArray),
   writeVerbose  (sv.writeVerbose)
{
   objArray.clear();
}


//------------------------------------------------------------------------------
// Save& operator=(const Save& sv)
//------------------------------------------------------------------------------
/**
 * Assignmant operator.
 * 
 * @param sv The instance that is copied.
 *
 * @return this instance, with internal data set to match the input command.
 */
//------------------------------------------------------------------------------
Save& Save::operator=(const Save& sv)
{
   if (this == &sv)
      return *this;
        
   fileNameArray = sv.fileNameArray;
   appendData    = sv.appendData;
   wasWritten    = sv.wasWritten;
   objNameArray  = sv.objNameArray;
   writeVerbose  = sv.writeVerbose;
   objArray.clear();
   
   return *this;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string Save::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < SaveParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Save::GetParameterID(const std::string &str) const
{
   for (int i=GmatCommandParamCount; i<SaveParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Save::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < SaveParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Save::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < SaveParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatCommand::GetParameterTypeString(id);
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
bool Save::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == OBJECT_NAMES)
   {
      Integer sz = objNameArray.size();
      for (Integer ii = 0; ii < sz; ii++)
      {
         if (objNameArray[ii] == value)
         {
            std::string ex = "Attempting to add """;
            ex += value + """ more than once to list of objects.\n";
            throw CommandException(ex);
         }
      }
      
      objNameArray.push_back(value);
      return true;
   }
   
   return GmatCommand::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <value> New value for the parameter.
 *
 * @return The string stored for this parameter.
 */
//------------------------------------------------------------------------------
bool Save::SetStringParameter(const std::string &label, const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
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
std::string Save::GetStringParameter(const Integer id,
                                     const Integer index) const
{
   if (id == OBJECT_NAMES)
   {
      if ((index < 0) || (index >= ((Integer) objNameArray.size())))
         throw CommandException
            ("Index out of bounds when attempting to return object name\n");
      return objNameArray.at(index);
   }
   
   return GmatCommand::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label,
//                                 const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <label> The (string) label for the parameter.
 * @param <index> array index for the parameter.
 *
 * @return The string stored for this parameter.
 */
//------------------------------------------------------------------------------
std::string Save::GetStringParameter(const std::string &label,
                                     const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
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
const StringArray& Save::GetStringArrayParameter(const Integer id) const
{
   if (id == OBJECT_NAMES)
      return objNameArray;
   
   return GmatCommand::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// bool GetRefObjectName(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves the reference object names.
 * 
 * @param type The type of the reference object.
 * 
 * @return the name of the object.
 */
//------------------------------------------------------------------------------
std::string Save::GetRefObjectName(const UnsignedInt type) const
{
   if (objNameArray.size() == 0)
      return "";
   else
      return objNameArray[0];
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
const StringArray& Save::GetRefObjectNameArray(const UnsignedInt type)
{
   // it can be any object, so ignore object type
   return objNameArray;
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the reference objects that get saved.
 * 
 * @param type The type of the reference object.
 * @param name The name of the reference object.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Save::SetRefObjectName(const UnsignedInt type,
                            const std::string &name)
{
   if (name == "")
      return false;
      
   // Save works for all types, so we don't check the type parameter
   objNameArray.push_back(name);
   return true;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Connects up the object associations prior to saving them.
 * 
 * @return true on success
 */
//------------------------------------------------------------------------------
bool Save::Initialize()
{
   bool retval = GmatCommand::Initialize();
   
   wasWritten = false;
   appendData = false;
   
   FileManager *fm = FileManager::Instance();
   std::string outPath = fm->GetAbsPathname(FileManager::OUTPUT_PATH);
   
   Integer objCount = objNameArray.size();
   
   #ifdef DEBUG_SAVE_INIT
   MessageInterface::ShowMessage
      ("Save::Initialize() objCount=%d, outPath=%s\n", objCount, outPath.c_str());
   #endif
   
   // throw an exception if no objects to save
   if (objCount == 0)   
      throw CommandException("Save command has no objects to save");
   
   #ifndef __USE_SINGLE_FILE__
      for (Integer i=0; i<objCount; i++)
         fileNameArray.push_back(outPath + objNameArray[i]);
      fileArray = new std::ofstream[objCount];
   #else
      fileNameArray.push_back(outPath + objNameArray[0]);
      fileArray = new std::ofstream[1];
   #endif
      
   Integer index = 0;
   
   for (StringArray::iterator i = objNameArray.begin(); i != objNameArray.end(); ++i)
   {
      #ifdef DEBUG_SAVE_INIT
      MessageInterface::ShowMessage("   objName=%s\n", (*i).c_str());
      #endif
      
      if (FindObject(*i) == NULL)
      {
         if (solarSys)
         {
            // Maybe it's a solar system object     
            if ((*i) == "SolarSystem")
            {
               objArray.push_back(solarSys);
               UpdateOutputFileNames(index, *i);
               index++;
               continue;
            }
            else if (solarSys)
            {
               // Maybe it's a solar system body object
               GmatBase *body = solarSys->GetBody(*i);
               if (body != NULL)
               {
                  objArray.push_back(body);
                  UpdateOutputFileNames(index, *i);
                  index++;
                  continue;
               }
            }
         }
         
         std::string errorString = "Save command cannot find object \"";
         errorString += *i;
         errorString += "\"";
         throw CommandException(errorString);
      }
      else
      {
         objArray.push_back(FindObject(*i));
      }
      
      UpdateOutputFileNames(index, *i);
      index++;
   }
   
   #ifdef __USE_SINGLE_FILE__
      if (objCount > 1)
         fileNameArray[0] += ".data";
      else if (index == objCount)
         fileNameArray[0] += ("." + objArray[0]->GetTypeName() + ".data");
      
      #ifdef DEBUG_SAVE_INIT
      MessageInterface::ShowMessage
         ("Save::Initialize() fileNameArray[%d]=%s\n", 0,
          fileNameArray[0].c_str());
      #endif
   #endif
   
   #ifdef DEBUG_SAVE_INIT
   MessageInterface::ShowMessage
      ("Save::Initialize() Saving %d object(s)\n", objArray.size());
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Write objects to a text file.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Save::Execute()
{
   #ifdef DEBUG_SAVE_EXEC
   MessageInterface::ShowMessage("Save::Execute() entered\n");
   #endif
   
   Integer prec = GmatGlobal::Instance()->GetDataPrecision();
   
   if (objArray.empty())
      throw CommandException("Save command has no objects to save");
   
   if (!objArray[0])
      throw CommandException("Object not set for Save command");
   
   #ifndef __USE_SINGLE_FILE__
   
      for (UnsignedInt i=0; i<objArray.size(); i++)
      {
         if (appendData && wasWritten)
            fileArray[i].open(fileNameArray[i].c_str(), std::ios::app);
         else
            fileArray[i].open(fileNameArray[i].c_str());
         
         fileArray[i].precision(prec);
      }
      
   #else

      // Changed to append the data once data was written so that saving data
      // within a loop won't overwrite old data (loj: 4/24/07)
      //if (appendData && wasWritten)
      if (appendData || wasWritten)
      {
         #ifdef DEBUG_SAVE_EXEC
         MessageInterface::ShowMessage("   open %s as append\n", fileNameArray[0].c_str());
         #endif
         
         fileArray[0].open(fileNameArray[0].c_str(), std::ios::app);
      }
      else
      {
         #ifdef DEBUG_SAVE_EXEC
         MessageInterface::ShowMessage("   open %s as new\n", fileNameArray[0].c_str());
         #endif
         
         fileArray[0].open(fileNameArray[0].c_str());
      }
      
      fileArray[0].precision(prec);
      
   #endif
      
      
   for (UnsignedInt i=0; i<objArray.size(); i++)
      WriteObject(i, objArray[i]);
   
   wasWritten = true;
   
   #ifndef __USE_SINGLE_FILE__   
      for (UnsignedInt i=0; i<objArray.size(); i++)
         fileArray[i].close();
   #else
      fileArray[0].close();
   #endif
   
   BuildCommandSummary(true);
   
   return true;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void Save::RunComplete()
{
   fileNameArray.clear();
   objArray.clear();
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Save.
 *
 * @return clone of the Save.
 */
//------------------------------------------------------------------------------
GmatBase* Save::Clone() const
{
   return (new Save(*this));
}


//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param mode    Specifies the type of serialization requested.  (Not yet used
 *                in commands)
 * @param prefix  Optional prefix appended to the object's name.  (Not yet used
 *                in commands)
 * @param useName Name that replaces the object's name.  (Not yet used in
 *                commands)
 *
 * @return The script line that, when interpreted, defines this Save command.
 */
//------------------------------------------------------------------------------
const std::string& Save::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   generatingString = prefix + "Save";
   for (StringArray::iterator i = objNameArray.begin(); i != objNameArray.end(); ++i)
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
bool Save::TakeAction(const std::string &action, const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("Save::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      objNameArray.clear();
      return true;
   }

   return false;
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
bool Save::RenameRefObject(const UnsignedInt type,
                           const std::string &oldName,
                           const std::string &newName)
{
   for (Integer index = 0; index < (Integer)objNameArray.size(); ++index)
   {
      if (objNameArray[index] == oldName)
         objNameArray[index] = newName;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// void UpdateOutputFileNames(Integer index, const std::string objName)
//------------------------------------------------------------------------------
void Save::UpdateOutputFileNames(Integer index, const std::string objName)
{
   // If saving multiple objects for single file,
   // we want to append object names.
   #ifdef __USE_SINGLE_FILE__
      if (index > 0)
         fileNameArray[0] = fileNameArray[0] + "_" + objName;
      
   #else
      fileNameArray[index] += ("." + objArray[index]->GetTypeName() + ".data");
      
      #ifdef DEBUG_SAVE_OUTPUT
      MessageInterface::ShowMessage
         ("Save::UpdateOutputFileNames() fileNameArray[%d]=%s\n", index,
          fileNameArray[index].c_str());
      #endif
   #endif
}


//------------------------------------------------------------------------------
// void Save::WriteObject()
//------------------------------------------------------------------------------
/**
 * Writes out the script snippet that is needed to recreate an object.
 */
//------------------------------------------------------------------------------
void Save::WriteObject(UnsignedInt i, GmatBase *o)
{
   #ifdef DEBUG_SAVE_OUTPUT
      MessageInterface::ShowMessage("Save: %s <a %s> has %d parameters\n",
         o->GetName().c_str(), o->GetTypeName().c_str(),
         o->GetParameterCount());
   #endif
   
   std::string objectname = o->GetName();
   std::string prefix = "";

   /**
    * @note:
    * Since we are using GetGeneratingString() we don't need to write "Create"
    * except Variables and Strings.  Variables and Strings are special case that
    * they are written in group in the ScriptInterpreter.  For example,
    * "Create Variable var1 var2 var3;" are written when save script.
    * For Arrays, it needs to know the dimension of array so it is handled in the
    * Array::GetGeneratingString().
    */
   
   // "Create Propagator" creates a PropSetup.  This code handles
   // that special case.
   //std::string tname = o->GetTypeName();
   //if (tname == "PropSetup")
   //   tname = "Propagator";   
   //file << "Create " << tname << " " << o->GetName() << "\n";
   
   if (o->GetTypeName() == "Variable" || o->GetTypeName() == "String")
      prefix = "Create " + o->GetTypeName() + " " + o->GetName() + "\n";

   // Do not write comments
   o->SetShowPrefaceComment(false);
   o->SetShowInlineComment(false);
   
   #ifdef __USE_SINGLE_FILE__
   fileArray[0] << prefix << o->GetGeneratingString(Gmat::SCRIPTING, "", "");
      fileArray[0] << std::endl;
   #else
      fileArray[i] << prefix << o->GetGeneratingString(Gmat::SCRIPTING, "", "");
      fileArray[i] << std::endl;
   #endif
   
   // Set back to write comments
   o->SetShowPrefaceComment(true);
   o->SetShowInlineComment(true);
      
   #ifdef DEBUG_SAVE_OUTPUT
      MessageInterface::ShowMessage("Save:WriteObject() leaving\n");
   #endif
      
}


