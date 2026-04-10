//$Id$
//------------------------------------------------------------------------------
//                                 Maneuver
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/20
//
/**
 * Implementation for the Maneuver command class
 */
//------------------------------------------------------------------------------


#include "Maneuver.hpp"
#include "MessageInterface.hpp"
#include <sstream>                 // for <<
#include "StringUtil.hpp"

//#define DEBUG_MANEUVER 1
//#define DEBUG_MANEUVER_PARSE
//#define DEBUG_MANEUVER_INIT
//#define DEBUG_MANEUVER_EXEC
//#define DEBUG_MANEUVER_COMMAND_SUMMARY

//------------------------------------------------------------------------------
// Static Data
//------------------------------------------------------------------------------
const std::string
   Maneuver::PARAMETER_TEXT[ManeuverCommandParamCount - GmatCommandParamCount] =
   {
      "Burn",
      "Spacecraft",
      "BackProp",
   };


const Gmat::ParameterType
   Maneuver::PARAMETER_TYPE[ManeuverCommandParamCount - GmatCommandParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::BOOLEAN_TYPE,
   };



//------------------------------------------------------------------------------
// Maneuver()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
Maneuver::Maneuver() :
   GmatCommand          ("Maneuver"),
   burnName             (""),
   burn                 (NULL),
   satName              (""),
   sat                  (NULL),
   firedOnce            (false),
   localCS              (false),
   scNameM              (""),
   csNameM              (""),
   originNameM          (""),
   axesNameM            (""),
   decMassM             (false),
   maneuverBackwards    (false),
   elementIspMassData   (NULL)
{
   objectTypeNames.push_back("BurnCommand");
   parameterCount      = ManeuverCommandParamCount;
   physicsBasedCommand = true;

   tankNamesM.clear();
}


//------------------------------------------------------------------------------
// ~Maneuver()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Maneuver::~Maneuver()
{
   if (elementIspMassData)
   {
      delete [] elementIspMassData;
      elementIspMassData = NULL;
   }
}


//------------------------------------------------------------------------------
// Maneuver(const Maneuver& m)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param m The maneuver that is copied to this one.
 */
//------------------------------------------------------------------------------
Maneuver::Maneuver(const Maneuver& m) :
   GmatCommand          (m),
   burnName             (m.burnName),
   burn                 (NULL),
   satName              (m.satName),
   sat                  (NULL),
   firedOnce            (false),
   localCS              (m.localCS),
   scNameM              (m.scNameM),
   csNameM              (m.csNameM),
   originNameM          (m.originNameM),
   axesNameM            (m.axesNameM),
   decMassM             (m.decMassM),
   maneuverBackwards    (m.maneuverBackwards),
   elementIspMassData   (NULL)
{
   parameterCount = ManeuverCommandParamCount;
   tankNamesM.clear();
}


//------------------------------------------------------------------------------
// Maneuver& operator=(const Maneuver& m)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Maneuver command.
 *
 * @param m The maneuver that is copied to this one.
 *
 * @return This instance, with the internal data set to match m's.
 */
//------------------------------------------------------------------------------
Maneuver& Maneuver::operator=(const Maneuver& m)
{
   if (this == &m)
      return *this;

   GmatCommand::operator=(m);
   burnName  = m.burnName;
   burn      = NULL;
   satName   = m.satName;
   sat       = NULL;
   firedOnce = false;

   localCS            = m.localCS;
   scNameM            = m.scNameM;
   csNameM            = m.csNameM;
   originNameM        = m.originNameM;
   axesNameM          = m.axesNameM;
   decMassM           = m.decMassM;
   elementIspMassData = NULL;
   maneuverBackwards = m.maneuverBackwards;
   tankNamesM.clear();

   return *this;
}

////------------------------------------------------------------------------------
////  bool SetObject(const std::string &name, const UnsignedInt type,
////                 const std::string &associate,
////                 const UnsignedInt associateType)
////------------------------------------------------------------------------------
///**
// * Sets an object used by the Maneuver command.
// *
// * @param name The name of the object.
// * @param type Type of object.
// * @param associate Name of an associated object, if needed.
// * @param associateType Type of associated object.
// *
// * @return true if the object was set.
// */
////------------------------------------------------------------------------------
//bool Maneuver::SetObject(const std::string &name, const UnsignedInt type,
//                         const std::string &associate,
//                         const UnsignedInt associateType)
//{
//   return GmatCommand::SetObject(name, type, associate, associateType);
//}
//
//
////------------------------------------------------------------------------------
////  bool SetObject(GmatBase *obj, const UnsignedInt type)
////------------------------------------------------------------------------------
///**
// * Sets an object used by the Maneuver command.
// *
// * @param obj The object.
// * @param type Type of object.
// *
// * @return true if the object was set.
// */
////------------------------------------------------------------------------------
//bool Maneuver::SetObject(GmatBase *obj, const UnsignedInt type)
//{
//   return GmatCommand::SetObject(obj, type);
//}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Maneuver.
 *
 * @return clone of the Maneuver.
 */
//------------------------------------------------------------------------------
GmatBase* Maneuver::Clone() const
{
   return (new Maneuver(*this));
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
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
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param useName Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this Maneuver command.
 */
//------------------------------------------------------------------------------
const std::string& Maneuver::GetGeneratingString(Gmat::WriteMode mode,
                                                 const std::string &prefix,
                                                 const std::string &useName)
{
   generatingString = prefix + "Maneuver ";
   if (maneuverBackwards)
      generatingString += "BackProp ";
   generatingString += burnName + "(" + satName + ");";
   
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * This method renames an object used by the maneuver command.
 *
 * @param type The type of the object.
 * @param oldName The current name for the object.
 * @param newName The new name for the object.
 *
 * @return true if a name was changed, otherwise false
 */
//------------------------------------------------------------------------------
bool Maneuver::RenameRefObject(const UnsignedInt type,
                               const std::string &oldName,
                               const std::string &newName)
{
   #if DEBUG_MANEUVER
      MessageInterface::ShowMessage
         ("Maneuver::RenameConfiguredItem() type=%s, oldName=%s, newName=%s\n",
          GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif

   if (type == Gmat::SPACECRAFT)
   {
      if (satName == oldName)
      {
         satName = newName;
         return true;
      }
   }
   else if (type == Gmat::IMPULSIVE_BURN)
   {
      if (burnName == oldName)
      {
         burnName = newName;
         return true;
      }
   }
   // need to return true to avoid error messages in the GUI when the name doesn't match an object
   // the this command knows about
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Maneuver.
 *
 * @return the list of object types.
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Maneuver::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::IMPULSIVE_BURN);
   refObjectTypes.push_back(Gmat::SPACECRAFT);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref objects used by the Maneuver.
 *
 * @param type The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *             full list.
 *
 * @return The list of object names.
 *
 */
//------------------------------------------------------------------------------
const StringArray& Maneuver::GetRefObjectNameArray(const UnsignedInt type)
{
   #ifdef DEBUG_MANEUVER_REFOBJ
   MessageInterface::ShowMessage("Maneuver::GetRefObjectNameArray(%d)\n", type);
   #endif

   refObjectNames.clear();

   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::IMPULSIVE_BURN)
   {
      refObjectNames.push_back(burnName);
   }

   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::SPACECRAFT)
   {
      refObjectNames.push_back(satName);
   }

   #ifdef DEBUG_MANEUVER_REFOBJ
   MessageInterface::ShowMessage("===> returning\n");
   for (UnsignedInt i=0; i<refObjectNames.size(); i++)
      MessageInterface::ShowMessage("   %s\n", refObjectNames[i].c_str());
   #endif

   return refObjectNames;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Maneuver::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < ManeuverCommandParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer Maneuver::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < ManeuverCommandParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }

   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return Parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Maneuver::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < ManeuverCommandParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];

   return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Maneuver::GetParameterTypeString(const Integer id) const
{
   return GmatCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Maneuver::GetStringParameter(const Integer id) const
{
   if (id == burnNameID)
      return burnName;

   if (id == satNameID)
      return satName;

   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  std::string SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id    ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
bool Maneuver::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == burnNameID)
   {
      burnName = value;
      return true;
   }

   if (id == satNameID)
   {
      satName = value;
      return true;
   }

   return GmatCommand::SetStringParameter(id, value);
}

bool Maneuver::GetBooleanParameter(const Integer id) const
{
   if (id == backPropID)
      return maneuverBackwards;

   return GmatCommand::GetBooleanParameter(id);
}

bool Maneuver::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == backPropID)
   {
      maneuverBackwards = value;
      return maneuverBackwards;
   }

   return GmatCommand::SetBooleanParameter(id, value);
}

bool Maneuver::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

bool Maneuver::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Maneuver command has the following syntax:
 *
 *     Maneuver burn1(sat1);
 *
 * where burn1 is an ImpulsiveBurn used to perform the maneuver, and sat1 is the
 * name of the spacecraft that is maneuvered.  This method breaks the script
 * line into the corresponding pieces, and stores the name of the ImpulsiveBurn
 * and the Spacecraft so they can be set to point to the correct objects during
 * initialization.
 *
 * @return true on successful parsing, throws an exception on failure
 */
//------------------------------------------------------------------------------
bool Maneuver::InterpretAction()
{
   StringArray chunks = InterpretPreface();

   #ifdef DEBUG_MANEUVER_PARSE
      MessageInterface::ShowMessage("Maneuver command chunks:\n");
      for (UnsignedInt i = 0; i < chunks.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", chunks[i].c_str());
   #endif

      StringArray subchunks = parser.SeparateSpaces(chunks[1]);

   #ifdef DEBUG_MANEUVER_PARSE
      MessageInterface::ShowMessage("Maneuver command subchunks:\n");
      for (UnsignedInt i = 0; i < subchunks.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", subchunks[i].c_str());
   #endif

   std::string burnChunk = chunks[1];
   if (subchunks.size() == 2)
   {
      if (subchunks[0] == "BackProp")
      {
         maneuverBackwards = true;
         burnChunk = subchunks[1];
      }
   }
   else if (subchunks.size() != 1)
      throw CommandException("Maneuver command is malformed; expecting "
                             "\"Maneuver ImpulsiveBurnName(SpacecraftName)\" or"
                             " \"Maneuver BackProp ImpulsiveBurnName"
                             "(SpacecraftName)\"\n");

   // Find and set the burn object name ...
   StringArray currentChunks = parser.Decompose(burnChunk, "()", false);

   if (currentChunks.size() < 2)
      throw CommandException("Missing Maneuver parameter. Expecting "
                             "\"ImpulsiveBurnName(SpacecraftName)\"\n");

   SetStringParameter(burnNameID, currentChunks[0]);

   #ifdef DEBUG_MANEUVER_PARSE
      MessageInterface::ShowMessage("In Maneuver, after Decompose, "
            "currentChunks = \n");
      for (unsigned int ii=0; ii<currentChunks.size(); ii++)
         MessageInterface::ShowMessage("    %s\n", currentChunks.at(ii).c_str());
   #endif

   // ... and the spacecraft that is maneuvered
   currentChunks = parser.SeparateBrackets(currentChunks[1], "()", ",");
   #ifdef DEBUG_MANEUVER_PARSE
      MessageInterface::ShowMessage("In Maneuver, after Separatebrackets, "
            "currentChunks = \n");
      for (unsigned int ii=0; ii<currentChunks.size(); ii++)
         MessageInterface::ShowMessage("    %s\n",currentChunks.at(ii).c_str());
   #endif
   if (currentChunks.size() > 1)
      throw CommandException("Unexpected text after spacecraft name in "
            "Maneuver command\n");

   if (currentChunks.size() == 0)
      throw CommandException("The Spacecraft name is missing in the Maneuver "
               "command\n");

   SetStringParameter(satNameID, currentChunks[0]);

   return true;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Maneuver.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Maneuver::Initialize()
{
   bool retval = false;

   #ifdef DEBUG_MANEUVER_INIT
   MessageInterface::ShowMessage
      ("Maneuver::Initialize() this=<%p>'%s' entered\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   if (GmatCommand::Initialize())
   {
      GmatBase *mapObj = NULL;
      if ((mapObj = FindObject(burnName)) == NULL)
         throw CommandException("Maneuver command cannot find the Burn " +
               burnName);
      if (mapObj->IsOfType(Gmat::BURN))
         burn = (Burn *)mapObj;
      else
         throw CommandException("The object " + burnName + " is not a burn, and "
               "cannot be used in as such in a Maneuver command");
   
      if ((mapObj = FindObject(satName)) == NULL)
         throw CommandException("Maneuver command cannot find the Spacecraft " +
               satName);
      if (mapObj->IsOfType(Gmat::SPACECRAFT))
         sat = (Spacecraft *)mapObj;
      else
         throw CommandException("The object " + satName + " is not a spacecraft, "
               "and cannot be used in as such in a Maneuver command");

      #ifdef DEBUG_MANEUVER_INIT
      MessageInterface::ShowMessage("   streamID=%d\n", streamID);
      MessageInterface::ShowMessage
         ("Maneuver::Initialize() this=<%p>'%s' returning true\n", this,
          GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      #endif

      if (elementIspMassData != NULL)
      {
         delete [] elementIspMassData;
         elementIspMassData = NULL;
      }
   
      firedOnce = false;
      retval = true;
   }
   else
      isInitialized = false;
   
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Fires an impulsive maneuver.
 *
 * This method calls the Fire method on an ImpulsiveBurn object.
 *
 * @return true if the Maneuver is executed, false if not.
 */
//------------------------------------------------------------------------------
bool Maneuver::Execute()
{
   #ifdef DEBUG_MANEUVER_EXEC
      MessageInterface::ShowMessage("Maneuver::Execute this=<%p> maneuvering "
            "%s\n", this, ((sat == NULL) ? "a NULL spaceecraft" :
               sat->GetName().c_str()));
   #endif
      
   Real epoch = sat->GetRealParameter("A1Epoch");
   
   #ifdef DEBUG_MANEUVER_EXEC
      Rvector6 state = sat->GetState(0); // Get cartesian state
      MessageInterface::ShowMessage
         ("   state before maneuver at epoch %f\n   %s\n", epoch,
               state.ToString().c_str());
   #endif
   
   burn->SetSpacecraftToManeuver(sat);
   
   // Set maneuvering to Publisher so that any subscriber can do its own action
   publisher->SetManeuvering(this, true, epoch, satName, "ImpulsiveBurn");
   
   bool retval = burn->Fire(NULL, epoch, maneuverBackwards);
   
   // Reset maneuvering to Publisher so that any subscriber can do its action
   publisher->SetManeuvering(this, false, epoch, satName, "ImpulsiveBurn");
   
   #ifdef DEBUG_MANEUVER_EXEC
      state = sat->GetState(0); // Get Cartesian state
      MessageInterface::ShowMessage
         ("   state after  maneuver at epoch %f \n   %s", epoch,
               state.ToString().c_str());
   #endif
   
   firedOnce = true;
   BuildCommandSummary(true);
   
   #ifdef DEBUG_MANEUVER_EXEC
      MessageInterface::ShowMessage("Maneuver::Execute this=<%p> complete\n",
            this);
   #endif

   return retval;
}


//-------------------------------------------
// Protected Methods
//-------------------------------------------

//------------------------------------------------------------------------------
// void BuildCommandSummary(bool commandCompleted)
//------------------------------------------------------------------------------
/**
 * This method saves the maneuver-specific data needed for the Command Summary
 *
 * @param commandCompleted Has the command completed?
 */
//------------------------------------------------------------------------------
void Maneuver::BuildCommandSummary(bool commandCompleted)
{
   GmatCommand::BuildCommandSummary(commandCompleted);

   if (elementIspMassData != NULL)
   {
      delete [] elementIspMassData;
   }
   elementIspMassData = new Real[5];

   #ifdef DEBUG_MANEUVER_COMMAND_SUMMARY
      MessageInterface::ShowMessage("Now about to save the Maneuver-specific "
            "data for the Command Summary\n");
   #endif

   // Save the Maneuver-specific data
   localCS     = burn->IsUsingLocalCoordSystem();
   scNameM     = burn->GetStringParameter(burn->GetParameterID(
                     "SpacecraftName"));
   csNameM     = burn->GetStringParameter(burn->GetParameterID(
                     "CoordinateSystem"));
   originNameM = burn->GetStringParameter(burn->GetParameterID("Origin"));
   axesNameM   = burn->GetStringParameter(burn->GetParameterID("Axes"));

   decMassM    = burn->GetBooleanParameter(burn->GetParameterID(
                     "DecrementMass"));

   #ifdef DEBUG_MANEUVER_COMMAND_SUMMARY
      MessageInterface::ShowMessage("... localCS     = %s\n",
            (localCS? "true" : "false"));
      MessageInterface::ShowMessage("... scNameM     = %s\n", scNameM.c_str());
      MessageInterface::ShowMessage("... csNameM     = %s\n", csNameM.c_str());
      MessageInterface::ShowMessage("... originNameM = %s\n",
            originNameM.c_str());
      MessageInterface::ShowMessage("... axesNameM   = %s\n",
            axesNameM.c_str());
      MessageInterface::ShowMessage("... decMassM    = %s\n",
            (decMassM ? "true" : "false"));
   #endif

   elementIspMassData[0] = burn->GetRealParameter(burn->GetParameterID(
         "Element1"));
   elementIspMassData[1] = burn->GetRealParameter(burn->GetParameterID(
         "Element2"));
   elementIspMassData[2] = burn->GetRealParameter(burn->GetParameterID(
         "Element3"));
   if (decMassM)
   {
      elementIspMassData[3] = burn->GetRealParameter(burn->GetParameterID(
            "Isp"));
      elementIspMassData[4] = burn->GetRealParameter(burn->GetParameterID(
            "DeltaTankMass"));
   }
   else
   {
      elementIspMassData[3] = 0.0;
      elementIspMassData[4] = 0.0;
   }
   #ifdef DEBUG_MANEUVER_COMMAND_SUMMARY
      MessageInterface::ShowMessage("... Element1          = %12.10f\n",
            elementIspMassData[0]);
      MessageInterface::ShowMessage("... Element2          = %12.10f\n",
            elementIspMassData[1]);
      MessageInterface::ShowMessage("... Element3          = %12.10f\n",
            elementIspMassData[2]);
      MessageInterface::ShowMessage("... Isp               = %12.10f\n",
            elementIspMassData[3]);
      MessageInterface::ShowMessage("... DeltaTankMass     = %12.10f\n",
            elementIspMassData[4]);
   #endif

   tankNamesM = burn->GetStringArrayParameter(burn->GetParameterID("Tank"));

   #ifdef DEBUG_MANEUVER_COMMAND_SUMMARY
      MessageInterface::ShowMessage("... tankNamesM        =\n");
      for (unsigned int ii = 0; ii < tankNamesM.size(); ii++)
         MessageInterface::ShowMessage("...    %s\n",
               tankNamesM.at(ii).c_str());
   #endif
}


//------------------------------------------------------------------------------
// void BuildCommandSummaryString(bool commandCompleted)
//------------------------------------------------------------------------------
/**
 * This method builds the Maneuver-specific part of the Command Summary string.
 *
 * @param commandCompleted Has the command completed?
 */
//------------------------------------------------------------------------------
void Maneuver::BuildCommandSummaryString(bool commandCompleted)
{
   bool canSummarize = commandCompleted && firedOnce;
   GmatCommand::BuildCommandSummaryString(canSummarize);

   if (canSummarize)
   {
      std::stringstream data;

      data << "\n"
           << "\n        Maneuver Summary"
           << "\n        -----------------"
           << "\n        Impulsive Burn:     " << burnName
           << "\n        Spacecraft:         " << scNameM;
      if (localCS)
      {
         data  << "\n        Origin:             " << originNameM
               << "\n        Axes:               " << axesNameM;

      }
      else
      {
         data << "\n        Coordinate System:  " << csNameM;
      }


      data << "\n        Delta V Vector:"
           << "\n           Element 1:  "
           << GmatStringUtil::BuildNumber(elementIspMassData[0]) << " km/s"
           << "\n           Element 2:  "
           << GmatStringUtil::BuildNumber(elementIspMassData[1]) << " km/s"
           << "\n           Element 3:  "
           << GmatStringUtil::BuildNumber(elementIspMassData[2]) << " km/s\n";

      if (decMassM)
      {
         Real thrust, tx, ty, tz;
         tx = elementIspMassData[0];
         ty = elementIspMassData[1];
         tz = elementIspMassData[2];
         thrust = GmatMathUtil::Sqrt(tx*tx + ty*ty + tz*tz);

         std::string tanklist;

         if (tankNamesM.size() == 1)
            tanklist = tankNamesM[0];
         else
         {
            for (UnsignedInt i = 0; i < tankNamesM.size(); ++i)
            {
               tanklist += tankNamesM[i];
               if (i != (tankNamesM.size() - 1))
                  tanklist += ", ";
               if (i == (tankNamesM.size() - 2))
                  tanklist += "and ";
            }
         }

         data << "\n        Mass depletion from " << tanklist <<":  "
              << "\n           Delta V:      "
              << GmatStringUtil::BuildNumber(thrust) << " km/s"
              << "\n           Isp:          "
              << GmatStringUtil::BuildNumber(elementIspMassData[3]) << " s"
              << "\n           Mass change:  "
              << GmatStringUtil::BuildNumber(elementIspMassData[4]) << " kg"
              << "\n";
		 data << "\n";

      }
      else
         data << "\n        No mass depletion\n";
	      data << "\n";
	     

      commandSummary = commandSummary + data.str();
   }
}
