//$Id$
//------------------------------------------------------------------------------
//                            Report
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Purchase
// order MOMS418823
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/07/06
//
/**
 *  Class implementation for the Report command.
 */
//------------------------------------------------------------------------------


#include "Report.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"       // for GetArrayIndex()
#include <sstream>

//#define DEBUG_REPORT_OBJ
//#define DEBUG_REPORT_SET
//#define DEBUG_REPORT_INIT
//#define DEBUG_REPORT_EXEC
//#define DEBUG_WRITE_HEADERS
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_OBJECT_MAP
//#define DEBUG_RENAME

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
const std::string
Report::PARAMETER_TEXT[ReportParamCount - GmatCommandParamCount] =
{
   "ReportFile",
   "Add",
};

const Gmat::ParameterType
Report::PARAMETER_TYPE[ReportParamCount - GmatCommandParamCount] =
{
   Gmat::STRING_TYPE,        // "ReportFile",
   Gmat::OBJECTARRAY_TYPE,   // "Add",
};


//---------------------------------
// public members
//---------------------------------

//------------------------------------------------------------------------------
//  Report()
//------------------------------------------------------------------------------
/**
 * Constructs the Report Command (default constructor).
 */
//------------------------------------------------------------------------------
Report::Report() :
   GmatCommand  ("Report"),
   rfName       (""),
   reporter     (NULL),
   reportID     (-1),
   numParams    (0),
   needsHeaders (true),
   hasExecuted  (false)
{
   // GmatBase data
   objectTypeNames.push_back("SubscriberCommand");
   objectTypeNames.push_back("Report");
}


//------------------------------------------------------------------------------
//  ~Report()
//------------------------------------------------------------------------------
/**
 * Destroys the Report Command (destructor).
 */
//------------------------------------------------------------------------------
Report::~Report()
{
   DeleteParameters();
}


//------------------------------------------------------------------------------
//  Report(const Report &rep)
//------------------------------------------------------------------------------
/**
 * Constructs the Report Command based on another instance (copy constructor).
 * 
 * @param rep The Report that is copied.
 */
//------------------------------------------------------------------------------
Report::Report(const Report &rep) :
   GmatCommand    (rep),
   rfName         (rep.rfName),
   reporter       (NULL),
   reportID       (-1),
   numParams      (0),
   needsHeaders   (rep.needsHeaders),
   hasExecuted    (rep.hasExecuted)
{
   parmNames = rep.parmNames;
   actualParmNames = rep.actualParmNames;
   parms.clear();
   parmWrappers.clear();
}


//------------------------------------------------------------------------------
//  Report& operator=(const Report &rep)
//------------------------------------------------------------------------------
/**
 * Sets this Report Command to match another instance (Assignment operator).
 * 
 * @param rep The Report that is copied.
 * 
 * @return This instance, configured to match the other and ready for 
 *         initialization.
 */
//------------------------------------------------------------------------------
Report& Report::operator=(const Report &rep)
{
   if (this != &rep)
   {
      rfName = rep.rfName;
      reporter = NULL;
      reportID = -1;
      needsHeaders = rep.needsHeaders;
      hasExecuted  = rep.hasExecuted;

      parmNames = rep.parmNames;
      actualParmNames = rep.actualParmNames;
      parms.clear();
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Report::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < ReportParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string Report::GetStringParameter(const Integer id) const
{
   if (id == REPORTFILE)
      return rfName;
   
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string Report::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Report::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case REPORTFILE:
      rfName = value;
      return true;
   case ADD:
      #ifdef DEBUG_REPORT_SET
      MessageInterface::ShowMessage
         ("Report::SetStringParameter() Adding parameter '%s'\n", value.c_str());
      #endif
      return AddParameter(value, numParams);
   default:
      return GmatCommand::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool Report::SetStringParameter(const std::string &label,
                                const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool Report::SetStringParameter(const Integer id, const std::string &value,
                                const Integer index)
{
   switch (id)
   {
   case ADD:
      return AddParameter(value, index);
   default:
      return GmatCommand::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool Report::SetStringParameter(const std::string &label,
                                const std::string &value,
                                const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& Report::GetStringArrayParameter(const Integer id) const
{
   #ifdef DEBUG_REPORTFILE_GET
   MessageInterface::ShowMessage
      ("Report::GetStringArrayParameter() id=%d, actualParmNames.size()=%d, "
       "numParams=%d\n", id, actualParmNames.size(), numParams);
   #endif
   
   switch (id)
   {
   case ADD:
      return actualParmNames;
   default:
      return GmatCommand::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& Report::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetWrapperObjectNameArray(bool completeSet = false)
//------------------------------------------------------------------------------
const StringArray& Report::GetWrapperObjectNameArray(bool completeSet)
{
   wrapperObjectNames.clear();
   wrapperObjectNames.insert(wrapperObjectNames.end(), actualParmNames.begin(),
                             actualParmNames.end());
   return wrapperObjectNames;
}


//------------------------------------------------------------------------------
// bool SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
//------------------------------------------------------------------------------
bool Report::SetElementWrapper(ElementWrapper *toWrapper,
                               const std::string &withName)
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Report::SetElementWrapper() this=<%p> '%s' entered, toWrapper=<%p>, "
       "withName='%s'\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       toWrapper, withName.c_str());
   #endif
   
   if (toWrapper == NULL)
      return false;
   
   // Do we need any type checking?
   // CheckDataType(toWrapper, Gmat::REAL_TYPE, "Report", true);
   
   bool retval = false;
   ElementWrapper *ew;
   std::vector<ElementWrapper*> wrappersToDelete;
   
   //-------------------------------------------------------
   // check parameter names
   //-------------------------------------------------------
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("   Checking %d Parameters\n", actualParmNames.size());
   for (UnsignedInt i=0; i<actualParmNames.size(); i++)
      MessageInterface::ShowMessage("      %s\n", actualParmNames[i].c_str());
   #endif
   
   Integer sz = actualParmNames.size();
   for (Integer i = 0; i < sz; i++)
   {
      if (actualParmNames.at(i) == withName)
      {
         #ifdef DEBUG_WRAPPER_CODE   
         MessageInterface::ShowMessage
            ("   Found wrapper name \"%s\" in actualParmNames\n", withName.c_str());
         #endif
         if (parmWrappers.at(i) != NULL)
         {
            ew = parmWrappers.at(i);
            parmWrappers.at(i) = toWrapper;
            // if wrapper not found, add to the list to delete
            if (find(wrappersToDelete.begin(), wrappersToDelete.end(), ew) ==
                wrappersToDelete.end())
               wrappersToDelete.push_back(ew);
         }
         else
         {
            parmWrappers.at(i) = toWrapper;
         }
         
         retval = true;
      }
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("   There are %d wrappers to delete\n", wrappersToDelete.size());
   #endif
   
   // Delete old ElementWrappers (loj: 2008.11.20)
   for (std::vector<ElementWrapper*>::iterator ewi = wrappersToDelete.begin();
        ewi < wrappersToDelete.end(); ewi++)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*ewi), (*ewi)->GetDescription(), "Report::SetElementWrapper()",
          GetGeneratingString(Gmat::NO_COMMENTS) + " deleting wrapper");
      #endif
      delete (*ewi);
      (*ewi) = NULL;
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Report::SetElementWrapper() exiting with %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void ClearWrappers()
//------------------------------------------------------------------------------
void Report::ClearWrappers()
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Report::ClearWrappers() this=<%p> '%s' entered\n   There are %d wrappers "
       "allocated, these will be deleted if not NULL\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), parmWrappers.size());
   #endif
   
   std::vector<ElementWrapper*> wrappersToDelete;
   
   // delete wrappers (loj: 2008.11.20)
   for (std::vector<ElementWrapper*>::iterator ewi = parmWrappers.begin();
        ewi < parmWrappers.end(); ewi++)
   {
      if ((*ewi) == NULL)
         continue;
      
      // if wrapper not found, add to the list to delete
      if (find(wrappersToDelete.begin(), wrappersToDelete.end(), (*ewi)) ==
          wrappersToDelete.end())
         wrappersToDelete.push_back((*ewi));
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("   There are %d wrappers to delete\n", wrappersToDelete.size());
   #endif
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
bool Report::TakeAction(const std::string &action, const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("Report::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif
   
   if (action == "Clear")
   {
      parmNames.clear();
      actualParmNames.clear();

      // I think we also need to clear wrappers here (loj: 2008.11.24)
      ClearWrappers();
      parmWrappers.clear();      
   }
   
   return false;
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
std::string Report::GetRefObjectName(const UnsignedInt type) const
{
   switch (type)
   {
   case Gmat::SUBSCRIBER:
      return rfName;
      
   case Gmat::PARAMETER:
      if (parmNames.size() == 0)
         return "";
      else
         return parmNames[0];
   default:
      return GmatCommand::GetRefObjectName(type);
   }
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
const StringArray& Report::GetRefObjectNameArray(const UnsignedInt type)
{
   static StringArray refObjectNames;
   refObjectNames.clear();
   
   switch (type)
   {
   case Gmat::SUBSCRIBER:
      refObjectNames.push_back(rfName);
      return refObjectNames;
   case Gmat::PARAMETER:
      return parmNames;
   default:
      return GmatCommand::GetRefObjectNameArray(type);
   }
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the ReportFile and Parameter objects used bt the Report command.
 * 
 * This method received the global instances of the objects used in the Report
 * command.  It checks their types and stores their names, so that the the 
 * objects can be retrieved from the local store in the Sandbox during 
 * initiialization.  It also tells the ReportFile instance that it will need
 * to be ready to receive data from a ReportCommand, so that the ReportFile does
 * not erroneously inform the user that no data will be written to the 
 * ReportFile.
 * 
 * @param <obj> Pointer to the reference object.
 * @param <type> type of the reference object.
 * @param <name> name of the reference object.
 * @param <index> Index into the object array.
 *
 * @return true if object successfully set, throws otherwise.
 */
//------------------------------------------------------------------------------
bool Report::SetRefObject(GmatBase *obj, const UnsignedInt type,
                          const std::string &name, const Integer index)
{
   if (obj == NULL)
   {
      #ifdef DEBUG_REPORT_OBJ
      MessageInterface::ShowMessage
         ("\nReport::SetRefObject() this=<%p> returning false since input object is NULL\n");
      #endif
      return false;
   }
   
   #ifdef DEBUG_REPORT_OBJ
   MessageInterface::ShowMessage
      ("\nReport::SetRefObject() this=<%p> entered, obj=<%p><%s>'%s'\n   type=%d<%s>, "
       "name='%s', index=%d\n", this, obj, obj->GetTypeName().c_str(), obj->GetName().c_str(),
       type, GetObjectTypeString(type).c_str(), name.c_str(), index);
   #endif
   
   if (type == Gmat::SUBSCRIBER)
   {
      if (obj->GetTypeName() != "ReportFile")
         throw CommandException("Report command must have a ReportFile name "
            "as the first parameter.\n");
      
      rfName = name;
      // Tell the ReportFile object that a command has requested its services
      obj->TakeAction("PassedToReport");
      reporter = (ReportFile*)obj;
   }
   else if (type == Gmat::PARAMETER)
   {
      #ifdef DEBUG_REPORT_OBJ
         MessageInterface::ShowMessage("   Received %s as a Parameter\n", name.c_str());
      #endif
         
      // All remaining refs should point to Parameter objects
      if (!obj->IsOfType("Parameter"))
         throw CommandException("Report command can only have Parameters "
            "in the list of reported values.\n");
      
      AddParameter(name, index, (Parameter*)obj);
   }
   
   #ifdef DEBUG_REPORT_OBJ
   MessageInterface::ShowMessage("Report::SetRefObject() returning true\n");
   #endif
   
   return true;
}


//---------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//                      const std::string &oldName, const std::string &newName)
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
bool Report::RenameRefObject(const UnsignedInt type,
                             const std::string &oldName,
                             const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Report::RenameRefObject() entered, type=%d<%s>, oldName='%s', newName='%s'\n",
       type, GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif

   // Check for allowed object types for name change.
   if (type != Gmat::SUBSCRIBER && type != Gmat::PARAMETER &&
       type != Gmat::SPACECRAFT && type != Gmat::COORDINATE_SYSTEM &&
       type != Gmat::PLATE && 
       type != Gmat::BURN && type != Gmat::IMPULSIVE_BURN &&
       type != Gmat::CALCULATED_POINT && type != Gmat::HARDWARE)
   {
      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("Report::RenameRefObject() returning true, no action is required\n");
      #endif
      
      return true;
   }
   
   if (type == Gmat::SUBSCRIBER)
   {
      if (rfName == oldName)
      {
         rfName = newName;
         #ifdef DEBUG_RENAME
         MessageInterface::ShowMessage
            ("   Subscriber name changed to '%s'\n", newName.c_str());
         #endif
      }
   }
   else
   {
      // Go through Parameter names
      for (UnsignedInt i=0; i<parmNames.size(); i++)
      {
         if (parmNames[i].find(oldName) != oldName.npos)
         {
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               ("   => Before rename, name: '%s'\n", parmNames[i].c_str());
            #endif
            
            parmNames[i] = GmatStringUtil::ReplaceName(parmNames[i], oldName, newName);
            
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               ("      After  rename, name: '%s'\n", parmNames[i].c_str());
            #endif
         }
      }
      
      // Go through actual Parameter names including array index
      for (UnsignedInt i=0; i<actualParmNames.size(); i++)
      {
         if (actualParmNames[i].find(oldName) != oldName.npos)
         {
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               ("   => Before rename, actual name: '%s'\n", actualParmNames[i].c_str());
            #endif
            
            actualParmNames[i] = GmatStringUtil::ReplaceName(actualParmNames[i], oldName, newName);
            
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               ("      After  rename, actual name: '%s'\n", actualParmNames[i].c_str());
            #endif
         }
      }
      
      // Go through wrappers
      for (WrapperArray::iterator i = parmWrappers.begin(); i < parmWrappers.end(); i++)
      {
         std::string desc = (*i)->GetDescription();
         if (desc.find(oldName) != oldName.npos)
         {
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               ("   => Before rename, wrapper desc = '%s'\n", desc.c_str());
            #endif
            
            (*i)->RenameObject(oldName, newName);
            desc = (*i)->GetDescription();
            
            #ifdef DEBUG_RENAME
            MessageInterface::ShowMessage
               ("      After  rename, wrapper desc = '%s'\n", desc.c_str());
            #endif
         }
      }
      
      // Go through generating string
      generatingString = GmatStringUtil::ReplaceName(generatingString, oldName, newName);
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("Report::RenameRefObject() leaving\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of this Report.
 *
 * @return clone of the Report.
 */
//------------------------------------------------------------------------------
GmatBase* Report::Clone() const
{
   return (new Report(*this));
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
//------------------------------------------------------------------------------
const std::string& Report::GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName)
{
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("Report::GetGeneratingString() rfName='%s', has %d parameters to write\n",
       rfName.c_str(), actualParmNames.size());
   #endif
   
   std::string gen = prefix + "Report " + rfName + " ";
   UnsignedInt numElem = actualParmNames.size();
   
   if (numElem > 1)
   {
      for (UnsignedInt i=0; i<numElem-1; i++)
         gen += actualParmNames[i] + " ";
      
      gen += actualParmNames[numElem-1];
   }
   else if (numElem == 1)
   {
      gen += actualParmNames[0];
   }
   
   generatingString = gen + ";";
   
   #ifdef DEBUG_GEN_STRING
   MessageInterface::ShowMessage
      ("   generatingString=<%s>, \n   now returning GmatCommand::GetGeneratingString()\n",
       generatingString.c_str());
   #endif
   
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Report command.
 *
 * @return true if the Report is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Report::Initialize()
{
   if (GmatCommand::Initialize() == false)
      return false;

   #ifdef DEBUG_REPORT_INIT
   MessageInterface::ShowMessage
      ("\nReport::Initialize() this=<%p> entered, has %d parameter names\n",
       this, parmNames.size());
   #endif
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps();
   #endif
   
   parms.clear();
   GmatBase *mapObj = NULL;
   
   if ((mapObj = FindObject(rfName)) == NULL)
      throw CommandException(
         "Report command cannot find ReportFile named \"" + (rfName) +
         "\"\n");
   
   reporter = (ReportFile *)mapObj;
   if (reporter->GetTypeName() != "ReportFile")
      throw CommandException(
         "Object named \"" + rfName +
         "\" is not a ReportFile; Report command cannot execute\n");
   
   // Tell the ReportFile object that a command has requested its services.
   // Added this here so that ReportFile::Initialize() doesn't throw exception
   // when there is no paramters to report (loj: 2008.06.11)
   reporter->TakeAction("PassedToReport");
   
   needsHeaders =
      reporter->GetBooleanParameter(reporter->GetParameterID("WriteHeaders"));
   hasExecuted = false;

   #ifdef DEBUG_REPORT_INIT
   MessageInterface::ShowMessage("   needsHeaders = %d\n", needsHeaders);
   #endif
   
   for (StringArray::iterator i = parmNames.begin(); i != parmNames.end(); ++i)
   {
      #ifdef DEBUG_REPORT_INIT
      MessageInterface::ShowMessage("   Now find object for '%s'\n", (*i).c_str());
      #endif
      
      mapObj = FindObject(*i);
      if (mapObj == NULL)
      {
         std::string msg = "Object named \"" + (*i) +
            "\" cannot be found for the Report command '" +
            GetGeneratingString(Gmat::NO_COMMENTS) + "'";
         #ifdef DEBUG_REPORT_INIT
         MessageInterface::ShowMessage("**** ERROR **** %s\n", msg.c_str());
         #endif
         //return false;
         throw CommandException(msg);
      }
      
      #ifdef DEBUG_REPORT_INIT
      MessageInterface::ShowMessage
         ("   Found object for '%s', <%p>[%s]\n", (*i).c_str(), mapObj,
          mapObj->GetTypeName().c_str());
      #endif
      
      if (!mapObj->IsOfType("Parameter"))
         throw CommandException("Parameter type mismatch for " + mapObj->GetName());
      
      // Check if it is reportable Parameter
      Parameter *param = (Parameter *)mapObj;
      if (!param->IsReportable())
         throw CommandException("Parameter \"" + mapObj->GetName() +
               "\" is not reportable");
      
      // Handle references to clones
      if (param->NeedExternalClone())
      {
         HandleReferencesToClones(param);
         
         // Moved this block to GmatCommand::HandleReferencesToClones() //LOJ: 2015.05.05
         // #ifdef DEBUG_REPORT_INIT
         // MessageInterface::ShowMessage("   Now handle external clone\n");
         // #endif
         // // For now, there is only one external clone
         // std::string cloneName = param->GetExternalCloneName(0);
         // GmatCommand *cmd = GetPrevious();
         // while (cmd != NULL)
         // {
         //    Integer count = cmd->GetCloneCount();
            
         //    for (Integer index = 0; index < count; ++index)
         //    {
         //       GmatBase *obj = cmd->GetClone(index);
         //       if (obj != NULL)
         //       {
         //          if (obj->GetName() == cloneName)
         //          {
         //             param->SetExternalClone(obj);
         //             cmd = NULL;
         //             break;
         //          }
         //       }
         //    }
         //    if (cmd != NULL)
         //       cmd = cmd->GetPrevious();
         // }
      }
      
      parms.push_back((Parameter *)mapObj);
   }
   
   // Set Wrapper references (LOJ: 2009.04.01)
   // We need this code to use ReportFile::WriteData() in Execute()
   for (WrapperArray::iterator i = parmWrappers.begin(); i < parmWrappers.end(); i++)
   {
      #ifdef DEBUG_REPORT_INIT
      MessageInterface::ShowMessage
         ("   wrapper desc = '%s'\n", (*i)->GetDescription().c_str());
      #endif
      
      if (SetWrapperReferences(*(*i)) == false)
         return false;      
   }
   
   #ifdef DEBUG_REPORT_INIT
   MessageInterface::ShowMessage("Report::Initialize() this=<%p> returning true\n", this);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Write the report data to a ReportFile.
 *
 * @return true if the Command runs to completion, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Report::Execute()
{
   if (parms.empty())
      throw CommandException("Report command has no parameters to write\n");
   if (reporter == NULL)
      throw CommandException("Reporter is not yet set\n");
   
   #ifdef DEBUG_REPORT_EXEC
   MessageInterface::ShowMessage
      ("\nReport::Execute() this=<%p> <%s> entered, reporter <%s> '%s' has %d Parameters\n",
       this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       reporter->GetName().c_str(), reporter->GetStringParameter("Filename").c_str(),
       parms.size());
   #endif
   
   // Build the data as a string
   std::stringstream datastream;
   
   // Set the stream to use the settings in the ReportFile.
   // Note that this is done here, rather than during initialization, in case
   // the user has changed the values during the run.
   Integer prec = reporter->GetIntegerParameter(reporter->GetParameterID("Precision"));
   datastream.precision(prec);
   
   bool leftJustify = false;
   if (reporter->GetOnOffParameter(reporter->GetParameterID("LeftJustify")) == "On")
      leftJustify = true;
   
   bool zeroFill = false;
   if (reporter->GetOnOffParameter(reporter->GetParameterID("ZeroFill")) == "On")
      zeroFill = true;
   
   int colWidth = reporter->GetIntegerParameter(reporter->GetParameterID("ColumnWidth"));
   
   #ifdef DEBUG_REPORT_EXEC
   MessageInterface::ShowMessage
      ("   precision=%d, leftJustify=%d, zeroFill=%d, colWidth=%d, needsHeaders=%d\n",
       prec, leftJustify, zeroFill, colWidth, needsHeaders);
   #endif
   
   if (leftJustify)
      datastream.setf(std::ios::left);
   
   // first time through, use cmd setting; after that, ask reporter
   if (hasExecuted)
      needsHeaders = reporter->TakeAction("CheckHeaderStatus");
   else
      needsHeaders = reporter->GetBooleanParameter(
            reporter->GetParameterID("WriteHeaders"));

   if (needsHeaders)
      WriteHeaders(datastream, colWidth);
   
   // if zero fill, show decimal point
   // showing decimal point automatically fills zero
   if (zeroFill)
      datastream.setf(std::ios::showpoint);
   
   // Write to report file using ReportFile::WriteData().
   // This method takes ElementWrapper array to write data to stream
   reporter->TakeAction("ActivateForReport", "On");
   bool retval = reporter->WriteData(parmWrappers);
   reporter->TakeAction("ActivateForReport", "Off");
   BuildCommandSummary(true);
   hasExecuted = true;
   
   #ifdef DEBUG_REPORT_EXEC
   MessageInterface::ShowMessage
      ("Report::Execute() this=<%p> returning %d\n", this, retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
void Report::RunComplete()
{
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("Report::RunComplete() this=<%p> '%s' entered\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
   
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
// void WriteHeaders(std::stringstream &datastream, Integer colWidth)
//------------------------------------------------------------------------------
/**
 * Writes column header by calling ReportFile::ReceiveData() with composed
 * headers
 */
//------------------------------------------------------------------------------
void Report::WriteHeaders(std::stringstream &datastream, Integer colWidth)
{
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage
      ("Report::WriteHeaders() entered, colWidth = %d, needsHeaders = %d\n",
       colWidth, needsHeaders);
   #endif
   
   reporter->TakeAction("ActivateForReport", "On");
   for (StringArray::iterator i = actualParmNames.begin();
        i != actualParmNames.end(); ++i)
   {
      datastream.width(colWidth);
      datastream << (*i);
      datastream << "   ";
   }
   
   std::string header = datastream.str();
   reporter->ReceiveData(header.c_str(), header.length());
   datastream.str("");
   
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage
      ("Report::WriteHeaders() leaving, needsHeaders set to false\n");
   #endif
   
   reporter->TakeAction("HeadersWritten");
   needsHeaders = false;
}


//------------------------------------------------------------------------------
// bool AddParameter(const std::string &paramName, Integer index, Parameter *param)
//------------------------------------------------------------------------------
/**
 * Adds Parameters to the report list.
 */
//------------------------------------------------------------------------------
bool Report::AddParameter(const std::string &paramName, Integer index,
                          Parameter *param)
{
   #ifdef DEBUG_REPORT_SET
   MessageInterface::ShowMessage
      ("Report::AddParameter() this=<%p>, Adding parameter '%s', index=%d, "
       "param=<%p>, numParams=%d\n", this, paramName.c_str(), index, param, numParams);
   #endif
   
   if (paramName == "")
   {
      #ifdef DEBUG_REPORT_SET
      MessageInterface::ShowMessage
         ("Report::AddParameter() returning false, input paramName is blank\n");
      #endif
      return false;
   }
   
   if (index < 0)
   {
      #ifdef DEBUG_REPORT_SET
      MessageInterface::ShowMessage
         ("Report::AddParameter() returning false, the index %d is less than 0\n");
      #endif
      return false;
   }
   
   // Since numParam is incremented after adding to arrays, index range varies
   // depends on whether parameter pointer is NULL or not
   if ((param == NULL && index > numParams) ||
       (param != NULL && index >= numParams))
   {
      #ifdef DEBUG_REPORT_SET
      MessageInterface::ShowMessage
         ("Report::AddParameter() returning false, the index %d is out of bounds, "
          "it must be between 0 and %d\n", index, param ? numParams + 1 : numParams);
      #endif
      return false;
   }
   
   if (param != NULL)
   {
      #ifdef DEBUG_REPORT_SET
      MessageInterface::ShowMessage
         ("   Set <%p>'%s' to index %d\n", param, paramName.c_str(), index);
      #endif
      parms[index] = param;
   }
   else
   {
      // Handle Array indexing first
      Integer row, col;
      std::string newName;      
      GmatStringUtil::GetArrayIndex(paramName, row, col, newName);
      
      parmNames.push_back(newName);
      actualParmNames.push_back(paramName);
      parms.push_back(param);
      parmWrappers.push_back(NULL);
      numParams = actualParmNames.size();
      
      #ifdef DEBUG_REPORT_SET
      MessageInterface::ShowMessage
         ("   Added '%s', size=%d\n", paramName.c_str(), numParams);
      #endif
      
      return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// void DeleteParameters()
//------------------------------------------------------------------------------
/**
 * Delete Parameters from the report list and wrappers.
 */
//------------------------------------------------------------------------------
void Report::DeleteParameters()
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("Report::DeleteParameters() this=<%p> '%s' entered\n   There are %d wrappers "
       "allocated, these will be deleted if not NULL\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), parmWrappers.size());
   #endif
   
   std::vector<ElementWrapper*> wrappersToDelete;
   
   // Add wrappers to the list to delete
   for (std::vector<ElementWrapper*>::iterator ewi = parmWrappers.begin();
        ewi < parmWrappers.end(); ewi++)
   {
      if ((*ewi) == NULL)
         continue;
      
      // If wrapper not found, add to the list to delete
      if (find(wrappersToDelete.begin(), wrappersToDelete.end(), (*ewi)) ==
          wrappersToDelete.end())
         wrappersToDelete.push_back((*ewi));
   }
   
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("   There are %d wrappers to delete\n", wrappersToDelete.size());
   #endif
   
   // Delete wrappers (loj: 2008.11.20)
   for (std::vector<ElementWrapper*>::iterator ewi = wrappersToDelete.begin();
        ewi < wrappersToDelete.end(); ewi++)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*ewi), (*ewi)->GetDescription(), "Report::DeleteParameters()",
          GetGeneratingString(Gmat::NO_COMMENTS) + " deleting wrapper");
      #endif
      delete (*ewi);
      (*ewi) = NULL;
   }
   
   parmWrappers.clear();   
   actualParmNames.clear();
   parms.clear();
}


