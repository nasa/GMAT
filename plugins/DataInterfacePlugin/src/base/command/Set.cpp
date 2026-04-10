//$Id$
//------------------------------------------------------------------------------
//                                  Set
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
 * Class implementation for the Set command
 */
//------------------------------------------------------------------------------

#include "Set.hpp"
#include "FileManager.hpp"      // for GetPathname()
#include "MessageInterface.hpp"
#include "GmatGlobal.hpp"       // for GetDataPrecision()

// Conversion system code used to convert CS and epoch data
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "TimeSystemConverter.hpp"
#include "SpaceObject.hpp"

#include <sstream>

//#define DEBUG_INITIALIZATION
//#define DEBUG_SET_EXEC
//#define DEBUG_SET_OUTPUT
//#define DEBUG_PARSING

////---------------------------------
////  static data
////---------------------------------
//const std::string
//Set::PARAMETER_TEXT[SetParamCount - GmatCommandParamCount] =
//{
//   "ObjectNames",
//};
//
//const Gmat::ParameterType
//Set::PARAMETER_TYPE[SetParamCount - GmatCommandParamCount] =
//{
//   Gmat::STRINGARRAY_TYPE,   // "ObjectNames",
//};


//------------------------------------------------------------------------------
// Set()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
Set::Set() :
   GmatCommand       ("Set"),
   targetName        (""),
   target            (NULL),
   interfaceName     (""),
   theInterface      (NULL),
   loadAll           (true)
{
}


//------------------------------------------------------------------------------
// ~Set()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Set::~Set()
{
}


//------------------------------------------------------------------------------
// Set(const Set& sv)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param sv The instance that is copied.
 */
//------------------------------------------------------------------------------
Set::Set(const Set& sv) :
   GmatCommand       (sv),
   targetName        (sv.targetName),
   target            (NULL),
   interfaceName     (sv.interfaceName),
   theInterface      (NULL),
   loadAll           (sv.loadAll),
   selections        (sv.selections)
{
}


//------------------------------------------------------------------------------
// Set& operator=(const Set& sv)
//------------------------------------------------------------------------------
/**
 * Assignmant operator.
 * 
 * @param sv The instance that is copied.
 *
 * @return this instance, with internal data set to match the input command.
 */
//------------------------------------------------------------------------------
Set& Set::operator=(const Set& sv)
{
   if (this != &sv)
   {
      targetName    = sv.targetName;
      target        = NULL;
      interfaceName = sv.interfaceName;
      theInterface  = NULL;
      loadAll       = sv.loadAll;
      selections    = sv.selections;
   }
   
   return *this;
}


////------------------------------------------------------------------------------
//// std::string GetParameterText(const Integer id) const
////------------------------------------------------------------------------------
//std::string Set::GetParameterText(const Integer id) const
//{
//   if (id >= GmatCommandParamCount && id < SetParamCount)
//      return PARAMETER_TEXT[id - GmatCommandParamCount];
//   else
//      return GmatCommand::GetParameterText(id);
//}
//
//
////------------------------------------------------------------------------------
//// Integer GetParameterID(const std::string &str) const
////------------------------------------------------------------------------------
//Integer Set::GetParameterID(const std::string &str) const
//{
//   for (int i=GmatCommandParamCount; i<SetParamCount; i++)
//   {
//      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
//         return i;
//   }
//
//   return GmatCommand::GetParameterID(str);
//}
//
//
////------------------------------------------------------------------------------
//// Gmat::ParameterType GetParameterType(const Integer id) const
////------------------------------------------------------------------------------
//Gmat::ParameterType Set::GetParameterType(const Integer id) const
//{
//   if (id >= GmatCommandParamCount && id < SetParamCount)
//      return PARAMETER_TYPE[id - GmatCommandParamCount];
//   else
//      return GmatCommand::GetParameterType(id);
//}
//
//
////------------------------------------------------------------------------------
//// std::string GetParameterTypeString(const Integer id) const
////------------------------------------------------------------------------------
//std::string Set::GetParameterTypeString(const Integer id) const
//{
//   if (id >= GmatCommandParamCount && id < SetParamCount)
//      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
//   else
//      return GmatCommand::GetParameterTypeString(id);
//}
//
//
////------------------------------------------------------------------------------
////  bool  SetStringParameter(const Integer id, const std::string value)
////------------------------------------------------------------------------------
///**
// * This method sets the string parameter value, given the input
// * parameter ID.
// *
// * @param <id> ID for the requested parameter.
// * @param <value> string value for the requested parameter.
// *
// * @exception <CommandException> thrown if value is already in the list.
// *
// * @return  success flag.
// *
// */
////------------------------------------------------------------------------------
//bool Set::SetStringParameter(const Integer id, const std::string &value)
//{
//   if (id == OBJECT_NAMES)
//   {
//      Integer sz = objNameArray.size();
//      for (Integer ii = 0; ii < sz; ii++)
//      {
//         if (objNameArray[ii] == value)
//         {
//            std::string ex = "Attempting to add """;
//            ex += value + """ more than once to list of objects.\n";
//            throw CommandException(ex);
//         }
//      }
//
//      objNameArray.push_back(value);
//      return true;
//   }
//
//   return GmatCommand::SetStringParameter(id, value);
//}
//
//
////------------------------------------------------------------------------------
////  bool SetStringParameter(const std::string &label, const std::string &value)
////------------------------------------------------------------------------------
///**
// * Sets the value for a std::string parameter.
// *
// * @param <label> The (string) label for the parameter.
// * @param <value> New value for the parameter.
// *
// * @return The string stored for this parameter.
// */
////------------------------------------------------------------------------------
//bool Set::SetStringParameter(const std::string &label, const std::string &value)
//{
//   return SetStringParameter(GetParameterID(label), value);
//}
//
//
////------------------------------------------------------------------------------
////  std::string  GetStringParameter(const Integer id, const Integer index)
////------------------------------------------------------------------------------
///**
// * This method returns the string parameter value, given the input
// * parameter ID and the index into the array.
// *
// * @param <id> ID for the requested parameter.
// * @param <index> index into the StringArray parameter.
// *
// * @exception <CommandException> thrown if value is out of range
// *
// * @return  string value at index 'index'.
// *
// */
////------------------------------------------------------------------------------
//std::string Set::GetStringParameter(const Integer id,
//                                     const Integer index) const
//{
//   if (id == OBJECT_NAMES)
//   {
//      if ((index < 0) || (index >= ((Integer) objNameArray.size())))
//         throw CommandException
//            ("Index out of bounds when attempting to return object name\n");
//      return objNameArray.at(index);
//   }
//
//   return GmatCommand::GetStringParameter(id, index);
//}
//
//
////------------------------------------------------------------------------------
////  std::string GetStringParameter(const std::string &label,
////                                 const Integer index) const
////------------------------------------------------------------------------------
///**
// * Retrieve a string parameter.
// *
// * @param <label> The (string) label for the parameter.
// * @param <index> array index for the parameter.
// *
// * @return The string stored for this parameter.
// */
////------------------------------------------------------------------------------
//std::string Set::GetStringParameter(const std::string &label,
//                                     const Integer index) const
//{
//   return GetStringParameter(GetParameterID(label), index);
//}
//
//
////------------------------------------------------------------------------------
////  const StringArray&  GetStringArrayParameter(const Integer id)
////------------------------------------------------------------------------------
///**
// * This method returns the string array value, given the input
// * parameter ID .
// *
// * @param <id> ID for the requested parameter.
// *
//  * @return  string array.
// *
// */
////------------------------------------------------------------------------------
//const StringArray& Set::GetStringArrayParameter(const Integer id) const
//{
//   if (id == OBJECT_NAMES)
//      return objNameArray;
//
//   return GmatCommand::GetStringArrayParameter(id);
//}


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
std::string Set::GetRefObjectName(const UnsignedInt type) const
{
   if (type == Gmat::INTERFACE)
      return interfaceName;

   return targetName;
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
const StringArray& Set::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();
   refObjectNames.push_back(interfaceName);
   if (type != Gmat::INTERFACE)
      refObjectNames.push_back(targetName);

   // it can be any object, so ignore object type
   return refObjectNames;
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the reference objects that get Set and their sources.
 * 
 * @param type The type of the reference object.
 * @param name The name of the reference object.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Set::SetRefObjectName(const UnsignedInt type,
                            const std::string &name)
{
//   if (name == "")
//      return false;
//
//   // Set works for all types, so we don't check the type parameter
//   objNameArray.push_back(name);
   return true;
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the scripting for the Set command
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Set::InterpretAction()
{
   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("%s::InterpretAction for \"%s\" entered, "
         "type = %s\n", typeName.c_str(), generatingString.c_str(), 
         typeName.c_str());
   #endif

   std::string mainString = generatingString;

   // Remove trailing semicolons
   // Note: Cannot use UnsignedInt here because return is really a size_t
   size_t loc = mainString.find_last_of(';');
   while (loc != std::string::npos)
   {
      #ifdef DEBUG_PARSING
         MessageInterface::ShowMessage("Went from '%s' to ",
               mainString.c_str());
      #endif
      mainString = mainString.substr(0, loc);
      #ifdef DEBUG_PARSING
         MessageInterface::ShowMessage("'%s'\n", mainString.c_str());
      #endif
      loc = mainString.find_last_of(';');
   }

   StringArray blocks = parser.DecomposeBlock(mainString);

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Top level blocks from \"%s\":\n",
            generatingString.c_str());
      for (StringArray::iterator i = blocks.begin(); i != blocks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
   #endif

   // Check for paren matching
   loc = blocks[0].find('(');
   if (loc != std::string::npos)
   {
      size_t loc2 = blocks[0].find('(', loc+1);
      if (loc2 != std::string::npos)
      {
         throw CommandException("Too many opening parens in the Set command");
      }

      loc2 = blocks[0].find(')');
      if (loc2 == std::string::npos)
      {
         throw CommandException("Missing closing parens in the Set command");
      }

      if (loc > loc2)
      {
         throw CommandException("Closing paren found before opening paren in the Set command");
      }

      size_t loc3 = blocks[0].find(')', loc2+1);
      if (loc3 != std::string::npos)
      {
         throw CommandException("Too many closing parens in the Set command");
      }

   }

   StringArray chunks = parser.SeparateBy(blocks[0], "()");

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Chunks from block \"%s\":\n",
            blocks[0].c_str());
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
   #endif

   StringArray subchunks = parser.SeparateBy(chunks[0], " ");
   if (subchunks.size() < 3)
      throw CommandException(typeName + "::InterpretAction() cannot identify "
            "either the target or the data source -- is one missing? -- in "
            "line\n" + generatingString);
   if (subchunks.size() > 3)
      throw CommandException(typeName + "::InterpretAction() has too many "
               "component strings in the line\n" + generatingString);

   // Parse the main part of the command
   if (subchunks[0] != typeName)
      throw CommandException(typeName + "::InterpretAction() does not identify "
            "the correct command type in line\n" + generatingString);
   targetName = subchunks[1];
   interfaceName = subchunks[2];

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Identified the target \"%s\" and the "
         "data source \"%s\"\n", targetName.c_str(), interfaceName.c_str());
   #endif

   if (chunks.size() > 2)
      throw CommandException(typeName +
            "::InterpretAction() found too many components to parse in the "
            "line\n" + generatingString);

   // Parse the command options
   if (chunks.size() == 2)
      CheckForOptions(chunks[1]);

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("%s::InterpretAction succeeded\n", 
         typeName.c_str());
   #endif

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
bool Set::Initialize()
{
   bool retval = GmatCommand::Initialize();
   
   target = FindObject(targetName);
   if (target == NULL)
      throw CommandException("The Set command could not find the target "
            "object \"" + targetName + "\" needed to initialize");

   GmatBase *obj = FindObject(interfaceName);
   if (obj == NULL)
      throw CommandException("The Set command could not find the interface "
            "object \"" + interfaceName + "\" needed to initialize");

   if (obj->IsOfType("DataInterface"))
      theInterface = (DataInterface*)obj;
   else
      throw CommandException("The object \"" + interfaceName +
               "\" is not a FileInterface object."); 


   // If specific data elements are requested, warn if not in the reader
   if (!loadAll)
   {
      StringArray allKeywords = 
         theInterface->GetStringArrayParameter("SupportedFields");
      for (UnsignedInt i = 0; i < selections.size(); ++i)
         if (find(allKeywords.begin(), allKeywords.end(), selections[i]) ==
               allKeywords.end())
            MessageInterface::ShowMessage("*** Warning ***: The data keyword "
                  "\"%s\" is not a recognized keyword in the data reader "
                  "\"tvhf\" on the line:\n%s\n", selections[i].c_str(), 
                  generatingString.c_str());
   }
   
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
bool Set::Execute()
{
   #ifdef DEBUG_SET_EXEC
      MessageInterface::ShowMessage("Set::Execute() entered\n");
   #endif
   
   bool retval = false, valueSet = false;

   if (theInterface == NULL)
      throw CommandException("The FileInterface object \"" + interfaceName +
               "\" was not found."); 
   
   if (theInterface->Open() == 0)
   {
      retval = theInterface->LoadData();
      if (theInterface->Close() != 0)
      {
         MessageInterface::ShowMessage("The interface failed to close\n");
      }

      if (retval)
      {
         UnsignedInt parmCount = 0;
         StringArray allItems =
               theInterface->GetStringArrayParameter("SupportedFields");
         std::string theItem, theParmString;

         if (loadAll)
            parmCount = allItems.size();
         else
            parmCount = selections.size();

         std::stringstream parametersNotSet;

         for (UnsignedInt i = 0; i < parmCount; ++i)
         {
            theItem = (loadAll ? allItems[i] : selections[i]);
            DataReader::readerDataType fieldType =
                     theInterface->GetReaderParameterType(theItem);

            if (SetTargetParameterData(fieldType, theItem))
               valueSet = true;
            else
               parametersNotSet << "   The parameter " << theItem 
                                << " did not set data on the object "
                                << targetName 
                                << "; either the field is missing from the "
                                << "file or the field value is not valid.\n";
         }

         if (parametersNotSet.str().length() > 0)
            //if (loadAll)
            //   MessageInterface::ShowMessage("*** Warning *** Loading failed "
            //            "for the following parameters:\n%sin the line\n%s\n",
            //            parametersNotSet.str().c_str(),
            //            GetGeneratingString(Gmat::NO_COMMENTS).c_str());
            //else
               throw CommandException("Error loading data from a DataInterface:\n" +
                        parametersNotSet.str() + "in the line\n" + 
                        GetGeneratingString(Gmat::NO_COMMENTS));
      }
      #ifdef DEBUG_SET_EXEC
         else
            MessageInterface::ShowMessage("Set::Execute() LoadData returned "
                  "false\n");
      #endif
   }
   #ifdef DEBUG_SET_EXEC
      else
         MessageInterface::ShowMessage("Set::Execute(): The interface did not "
               "open\n");
   #endif

   BuildCommandSummary();

   return retval && valueSet;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void Set::RunComplete()
{
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Set.
 *
 * @return clone of the Set.
 */
//------------------------------------------------------------------------------
GmatBase* Set::Clone() const
{
   return (new Set(*this));
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
 * @return The script line that, when interpreted, defines this Set command.
 */
//------------------------------------------------------------------------------
const std::string& Set::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   generatingString = prefix + "Set " + targetName + " " + interfaceName;

   // Add selections
   if ((loadAll == false) && (selections.size() > 0))
   {
      generatingString += " (Data = {";
      for (UnsignedInt i = 0; i < selections.size(); ++i)
      {
         if (i > 0)
            generatingString += ", ";
         generatingString += "'" + selections[i] + "'";
      }
      generatingString += "})";
   }

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
bool Set::TakeAction(const std::string &action, const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("Set::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      selections.clear();
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
bool Set::RenameRefObject(const UnsignedInt type,
                           const std::string &oldName,
                           const std::string &newName)
{
   if (targetName == oldName)
      targetName = newName;
   if (interfaceName == oldName)
      interfaceName = newName;
   
   return true;
}

//------------------------------------------------------------------------------
// bool SetTargetParameterData(DataReader::readerDataType theType,
//                             const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Sets a parameter value on the target
 *
 * @param theType The data type as understtod in the Reader
 * @param forField The reader identifier for the parameter
 *
 * @return true is data was set, false if not
 */
//------------------------------------------------------------------------------
bool Set::SetTargetParameterData(DataReader::readerDataType theType,
      const std::string& forField)
{
   bool retval = false;
   std::string theParmString;

   if (theInterface->WasDataLoaded(forField))
   {
      theParmString =
            theInterface->GetObjectParameterName(forField);
      if (theParmString != "")
      {
         Integer id = target->GetParameterID(theParmString);
         Integer type = target->GetParameterType(id);
         switch (theType)
         {
         case DataReader::READER_REAL:
            {
               Real value = theInterface->GetRealValue(forField);
               #ifdef DEBUG_SET_EXEC
                  MessageInterface::ShowMessage("Setting %s on %s to "
                        "%.12lf\n", theParmString.c_str(),
                        targetName.c_str(), value);
               #endif
               if (type != Gmat::REAL_TYPE)
                  throw CommandException("The data interface parameter " +
                        forField + " has the wrong data type");
               target->SetRealParameter(id, value);
               retval = true;
            }
            break;

         case DataReader::READER_RVECTOR6:
            {
               Rvector6 values = theInterface->GetReal6Vector(forField);

               #ifdef DEBUG_SET_EXEC
                  MessageInterface::ShowMessage("Setting %s on %s\n",
                        theParmString.c_str(), targetName.c_str());
               #endif
               if (theInterface->UsesCoordinateSystem(forField))
               {
                  std::string csName =
                        theInterface->GetCoordinateSystemName(forField);

                  values = ConvertToTargetCoordinateSystem(csName, values);

                  #ifdef DEBUG_SET_EXEC
                     MessageInterface::ShowMessage("   6-vector C.S. is %s\n",
                           csName.c_str());
                  #endif
               }

               for (UnsignedInt i = 0; i < 6; ++i)
               {
                  if (target->GetParameterType(id + i) != Gmat::REAL_TYPE)
                     throw CommandException("The data interface parameter " +
                           forField + " has the wrong data type");
                  target->SetRealParameter(id + i, values[i]);
               }
               retval = true;
            }
            break;

         case DataReader::READER_STRING:
            retval = true;
            break;

         case DataReader::READER_TIMESTRING:
            {
               if (target->IsOfType("SpaceObject"))
               {
                  std::string epochSystem =
                        theInterface->GetTimeSystemName(forField);

                  if (epochSystem != "")
                  {
                     GmatEpoch fileEpoch = theInterface->GetRealValue(forField);
                     Real newEpoch = ConvertToSystemTime(epochSystem, fileEpoch);

                     #ifdef DEBUG_SET_EXEC
                        MessageInterface::ShowMessage("Setting time data %.12lf "
                              "to %.12lf\n", fileEpoch, newEpoch);
                     #endif

                     ((SpaceObject*)target)->SetEpoch(newEpoch);
                     retval = true;
                  }
               }
               else
               {
                  MessageInterface::ShowMessage("Epoch data can only be set on "
                        "Spacecraft objects and Formations ; ignoring the %s "
                        "setting for %s\n", forField.c_str(),
                        target->GetName().c_str());
               }
            }
            break;

         case DataReader::READER_SUBTYPE:
            retval = true;
            break;

         default:
            break;
         }
      }
      else
      {
         if (!loadAll)
            throw CommandException("The field " + forField +
               " was requested in the Set command, but the " + 
               theInterface->GetTypeName() + " " +
               interfaceName + " does not set that field on the object " + 
               targetName + ".");
         retval = true;
      }
   }
   else //if (!loadAll)
      throw CommandException("The field " + forField +
               " was requested in the Set command, but the FileInterface " +
               interfaceName + " does not contain data for that field.");

   return retval;
}

//-----------------------------------------------------------------------------
//Rvector6 ConvertToTargetCoordinateSystem(const std::string& from,
//      Rvector6& fromState)
//-----------------------------------------------------------------------------
/**
 * Performs coordinate system conversion on an Rvector6
 *
 * This method converts an input coordinate system based 6 element vector from
 * a coordinate system identified with the reader into the coordinate system 
 * applied to a SpaceObject, so that the data can be passed into the object.
 *
 * @param from A string identifying the reader coordinate system
 * @param fromState The 6-vector that needs to be converted
 *
 * @return The converted vector
 */
//-----------------------------------------------------------------------------
Rvector6 Set::ConvertToTargetCoordinateSystem(const std::string& from,
      Rvector6& fromState)
{
   Rvector6 newRep;
   SpacePoint *origin = NULL, *j2000body = solarSys->GetBody("Earth");

   std::string axisType = "", originName = "";
   if (from.find("MJ2000Eq") != std::string::npos)
   {
      axisType = "MJ2000Eq";
      originName = from.substr(0, from.find("MJ2000Eq"));
      origin = solarSys->GetBody(originName);
   }

   if (target->IsOfType("SpaceObject") && (origin != NULL))
   {
      #ifdef DEBUG_SET_EXEC
         MessageInterface::ShowMessage("Creating local CS with axes %s and "
               "origin at %s\n", axisType.c_str(), originName.c_str());
      #endif
      CoordinateSystem *fromCS = CoordinateSystem::CreateLocalCoordinateSystem(
            from, axisType, origin, NULL, NULL, j2000body, solarSys);
      //CoordinateSystem *toCS =
      //      (CoordinateSystem*)target->GetRefObject(Gmat::COORDINATE_SYSTEM,"");
      CoordinateSystem *toCS = CoordinateSystem::CreateLocalCoordinateSystem(
            "temp", "MJ2000Eq", solarSys->GetBody("Earth"), NULL, NULL, j2000body, solarSys);

      //GmatEpoch epoch = ConvertToSystemTime(
      //      theInterface->GetTimeSystemName("Epoch"),
      //      theInterface->GetRealValue("Epoch"));

      GmatEpoch epoch = ((SpaceObject*)target)->GetEpoch();

      if ((fromCS != NULL) && (toCS != NULL))
      {
         CoordinateConverter converter;
         converter.Convert(epoch, fromState, fromCS, newRep, toCS);
      }
      else
      {
         MessageInterface::ShowMessage("Unable to create the reference "
               "coordinate system %s\n", from.c_str());
         newRep = fromState;
      }

      if (fromCS)
         delete fromCS;
      if (toCS)
         delete toCS;
   }
   else
   {
      MessageInterface::ShowMessage("Unable to convert from %s into the "
            "target's coordinate system for the object %s; is it a "
            "Spacecraft?\n", from.c_str(), target->GetName().c_str());
      newRep = fromState;
   }

   return newRep;
}


//-----------------------------------------------------------------------------
// GmatEpoch ConvertToSystemTime(const std::string& from, GmatEpoch fromTime)
//-----------------------------------------------------------------------------
/**
 * Converts a time representation into A.1 Mod Julian time
 *
 * @param from String nemae of teh input time system
 * @param fromTime The input time
 *
 * @return The corresponding A.1 modJulian time
 */
//-----------------------------------------------------------------------------
GmatEpoch Set::ConvertToSystemTime(const std::string& from, GmatEpoch fromTime)
{
   std::string outStr;
   GmatEpoch newEpoch;
   TimeSystemConverter::Instance()->Convert(from, fromTime, "",
         "A1ModJulian", newEpoch, outStr);

   return newEpoch;
}

//-----------------------------------------------------------------------------
// void CheckForOptions(const std::string options)
//-----------------------------------------------------------------------------
/**
 * Fills in the options for the set command
 *
 * param options The string containing the options
 */
//-----------------------------------------------------------------------------
void Set::CheckForOptions(const std::string options)
{
   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Set::CheckForOptions(\"%s\") called\n", 
         options.c_str());
   #endif

   // Remove the parentheses if they are still in the string
   size_t start = options.find_first_of("(");
   if (start == std::string::npos)
      start = 0;
   else
      ++start;
   size_t end = options.find_last_of(")");
   std::string data = options.substr(start, end-start);

   // Check for bracket matching
   size_t loc = data.find('{');
   if (loc != std::string::npos)
   {
      size_t loc2 = data.find('{', loc+1);
      if (loc2 != std::string::npos)
      {
         throw CommandException("Too many opening brackets in the Set command");
      }

      loc2 = data.find('}');
      if (loc2 == std::string::npos)
      {
         throw CommandException("Missing closing bracket in the Set command");
      }

      if (loc > loc2)
      {
         throw CommandException("Closing bracket found before opening bracket in the Set command");
      }

      size_t loc3 = data.find('}', loc2+1);
      if (loc3 != std::string::npos)
      {
         throw CommandException("Too many closing brackets in the Set command");
      }
   }


   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Data string: \"%s\"\n", 
         data.c_str());
   #endif

   StringArray chunks = parser.SeparateBy(data, "={}");

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("String chunks as\n");
      for (UnsignedInt i = 0; i < chunks.size(); ++i)
         MessageInterface::ShowMessage("   %2d: \"%s\"\n", i, 
            chunks[i].c_str());
   #endif

   UnsignedInt index = 0;
   while (index < chunks.size())
   {
      if (chunks[index] == "Data")
      {
         selections.clear();
         ++index;
         if (index < chunks.size())
         {
            if (chunks[index] == "")
               ++index;

            #ifdef DEBUG_PARSING
               MessageInterface::ShowMessage("Parsing the Data option \"%s\"\n", 
                  chunks[index].c_str());
            #endif
            StringArray parts = parser.SeparateBy(chunks[index], "', ");
            for (UnsignedInt i = 0; i < parts.size(); ++i)
               if (parts[i] != "Epoch")
                  selections.push_back(parts[i]);
               else     // Always have time at the beginning
                  selections.insert(selections.begin(), parts[i]);

            #ifdef DEBUG_PARSING
               MessageInterface::ShowMessage("Data selections are\n");
               for (UnsignedInt i = 0; i < selections.size(); ++i)
                  MessageInterface::ShowMessage("   %2d: \"%s\"\n", i, 
                     selections[i].c_str());
            #endif
         }
         else
            MessageInterface::ShowMessage("*** Warning ***: The Set command "
                  "specifies a \"Data\" option, but no data is specified in "
                  "the line\n%s\nAll data will be loaded\n", 
                  generatingString.c_str());

         if (selections.size() > 0)
         {
            if (find(selections.begin(), selections.end(), "All") == selections.end())
               loadAll = false;
            else if (selections.size() > 1)
               MessageInterface::ShowMessage("*** Warning ***: The Set command "
                     "specifies several \"Data\" options including \"All\" in "
                     "the line\n%s\nAll data will be loaded\n", 
                     generatingString.c_str());
         }
      }
      else
         throw CommandException("The Set command option " + 
                                chunks[index] + " is not a known option");
      ++index;
   }
   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("CheckForOptions() finished\n");
   #endif
}
