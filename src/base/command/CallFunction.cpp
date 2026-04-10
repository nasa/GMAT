//$Id$
//------------------------------------------------------------------------------
//                                 CallFunction
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
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the CallFunction command class
 */
//------------------------------------------------------------------------------
#include "CallFunction.hpp"
#include "BeginFunction.hpp"
#include "StringTokenizer.hpp"
#include "StringUtil.hpp"          // for Replace()
#include "FileUtil.hpp"            // for ParseFileName()
#include "FileManager.hpp"         // for GetAllMatlabFunctionPaths()
#include "MessageInterface.hpp"
#include "GmatGlobal.hpp"          // for IsWritingGmatKeyword()
#include <sstream>


//#define DEBUG_CALL_FUNCTION_PARAM
//#define DEBUG_CALL_FUNCTION_REF_OBJ
//#define DEBUG_CALL_FUNCTION_INIT
//#define DEBUG_CALL_FUNCTION_EXEC
//#define DEBUG_SEND_PARAM
//#define DEBUG_UPDATE_VAR
//#define DEBUG_UPDATE_OBJECT
//#define DEBUG_SHOW_ARRAY
//#define DEBUG_GET_OUTPUT
//#define DEBUG_OBJECT_MAP
//#define DEBUG_GLOBAL_OBJECT_MAP
//#define DEBUG_RUN_COMPLETE
//#define DEBUG_OBJECT_REF
//#define DEBUG_FUNCTION

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif
//#ifndef DEBUG_TRACE
//#define DEBUG_TRACE
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif
#ifdef DEBUG_TRACE
#include <ctime>                 // for clock()
#endif

//---------------------------------
// static data
//---------------------------------
const std::string
CallFunction::PARAMETER_TEXT[CallFunctionParamCount - GmatCommandParamCount] =
{
   "FunctionName",
   "AddInput",
   "AddOutput",
   "CommandStream",
};


const Gmat::ParameterType
CallFunction::PARAMETER_TYPE[CallFunctionParamCount - GmatCommandParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::OBJECT_TYPE,
};


//------------------------------------------------------------------------------
// CallFunction::CallFunction(const std::string &type)
//------------------------------------------------------------------------------
CallFunction::CallFunction(const std::string &type) :
   GmatCommand          (type),
   callcmds             (NULL),
   mFunction            (NULL),
   mFunctionName        (""),
   mFunctionPathAndName (""),
   isGmatFunction       (false),
   isMatlabFunction     (false),
   isBuiltinGmatFunction(false)
{
   mNumInputParams = 0;
   mNumOutputParams = 0;
   
   parameterCount = CallFunctionParamCount;
   objectTypeNames.push_back("CallFunction");
}


//------------------------------------------------------------------------------
// ~CallFunction()
//------------------------------------------------------------------------------
CallFunction::~CallFunction()
{
   if (callcmds)
      delete callcmds;
}


//------------------------------------------------------------------------------
// CallFunction(const CallFunction& cf) :
//------------------------------------------------------------------------------
CallFunction::CallFunction(const CallFunction& cf) :
   GmatCommand           (cf),
   callcmds              (NULL),
   mFunction             (cf.mFunction),
   mFunctionName         (cf.mFunctionName),
   mFunctionPathAndName  (cf.mFunctionPathAndName),
   fm                    (cf.fm)
{
   mNumInputParams = cf.mNumInputParams;
   mNumOutputParams = cf.mNumOutputParams;
   
   objectArray = cf.objectArray;
   mInputList = cf.mInputList;
   mOutputList = cf.mOutputList;
   callcmds = NULL;           // Commands must be reinitialized
   isGmatFunction = cf.isGmatFunction;
   isMatlabFunction = cf.isMatlabFunction;
   isBuiltinGmatFunction = cf.isBuiltinGmatFunction;
   
   mInputNames  = cf.mInputNames;
   mOutputNames = cf.mOutputNames;
   
   parameterCount = CallFunctionParamCount;
}


//------------------------------------------------------------------------------
// CallFunction& operator=(const CallFunction& cf)
//------------------------------------------------------------------------------
CallFunction& CallFunction::operator=(const CallFunction& cf)
{
   if (this == &cf)
      return *this;
   
   GmatCommand::operator=(cf);
   
   mFunction = cf.mFunction;
   mFunctionName = cf.mFunctionName;
   mFunctionPathAndName = cf.mFunctionPathAndName;
   mNumInputParams = cf.mNumInputParams;
   mNumOutputParams = cf.mNumOutputParams;
   
   objectArray = cf.objectArray;
   mInputList = cf.mInputList;
   mOutputList = cf.mOutputList;
   callcmds = NULL;           // Commands must be reinitialized
   isGmatFunction = cf.isGmatFunction;
   isMatlabFunction = cf.isMatlabFunction;
   isBuiltinGmatFunction = cf.isBuiltinGmatFunction;
   
   mInputNames  = cf.mInputNames;
   mOutputNames = cf.mOutputNames;
   fm           = cf.fm;
   
   return *this;
}


//------------------------------------------------------------------------------
// std::string FormEvalString()
//  String format
//    [Out1, Out2] = FunctionName(In1, In2, In3);
//------------------------------------------------------------------------------
std::string CallFunction::FormEvalString()
{
   #ifdef DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage
      ("CallFunction::FormEvalString() entered, mFunction=<%p>'%s'\n",
       mFunction, mFunction ? mFunction->GetName().c_str() : "NULL");
   #endif
   std::string evalString = "";
   
   // left hand side of evaluation string and equals (if necessary)
   if (mOutputList.size() > 1)
   {
      evalString = evalString + "[";
      Parameter *param = (Parameter *)mOutputList[0];
      evalString = evalString + param->GetName();
      
      for (unsigned int i=1; i<mOutputList.size(); i++)
      {
         param = (Parameter *)mOutputList[i];
         evalString = evalString +", " + param->GetName();
      }
      
      evalString = evalString + "] = ";
   }
   else if (mOutputList.size() == 1)
   {
      Parameter *param = (Parameter *)mOutputList[0];
      evalString = "[" + evalString + param->GetName() + "]";
      evalString = evalString +" = ";
   }
   else if (mOutputList.size() == 0)
   {
      // no left hand side
   }
   else
   {
      // need to throw an exception here
   }
   
   
   // right hand side of evaluation string
   // function name and left parenthesis
   evalString = evalString + mFunction->GetName().c_str() + "(";


   // input parameters
   if (mInputList.size() > 0)
   {
      Parameter *param = (Parameter *)mInputList[0];
      evalString = evalString + param->GetName();

      for (unsigned int i=1; i<mInputList.size(); i++)
      {
         param = (Parameter *)mInputList[i];
         evalString = evalString + ", " + param->GetName();
      }
   }
   
   // right parenthesis and semi-colon
   evalString = evalString + ");";
   
   return evalString;
}


//------------------------------------------------------------------------------
// bool AddInputParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
bool CallFunction::AddInputParameter(const std::string &paramName, Integer index)
{
   if (paramName != "" && index == mNumInputParams)
   {
      mInputNames.push_back(paramName);
      mNumInputParams = mInputNames.size();
      mInputList.push_back(NULL);
      fm.AddInput(paramName, index);
      return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool AddOutputParameter(const std::string &paramName, Integer index)
//------------------------------------------------------------------------------
bool CallFunction::AddOutputParameter(const std::string &paramName, Integer index)
{
   if (paramName != "" && index == mNumOutputParams)
   {
      mOutputNames.push_back(paramName);
      mNumOutputParams = mOutputNames.size();
      mOutputList.push_back(NULL);
      fm.AddOutput(paramName, index);
      return true;
   }

   return false;
}

//---------------------------------------------------------------------------
// bool HasOtherReferenceToObject(const std::string &withName)
//---------------------------------------------------------------------------
/*
 * Determines whether or not this object has a hidden or indirect
 * reference to the input object.  This could be the case when a command has
 * wrappers and one of those wrappers has the named object as a reference.
 *
 * @param  withName   the name of the object we're looking for
 *
 * @return  true if it has a reference to the named object
 */
//---------------------------------------------------------------------------
bool CallFunction::HasOtherReferenceToObject(const std::string &withName)
{
   #ifdef DEBUG_OBJECT_REF
      MessageInterface::ShowMessage(
            "Entering HasOtherReferenceToObject for item \"%s\" of type \"%s\" and withName \"%s\"\n",
            instanceName.c_str(), typeName.c_str(), withName.c_str());
   #endif
   // Right now, for this class, we look at the parameters
   StringArray paramNames  = GetRefObjectNameArray(Gmat::PARAMETER);
   Integer     sz         = (Integer) paramNames.size();
   std::string objName    = "";
   StringArray byDots;
   for (Integer ii = 0; ii < sz; ii++)
   {
      byDots  = GmatStringUtil::SeparateDots(paramNames.at(ii));
      // Check the first part of the name for the object we're looking for
      objName = GmatStringUtil::Trim(GmatStringUtil::GetArrayName(byDots.at(0)));
      #ifdef DEBUG_OBJECT_REF
         MessageInterface::ShowMessage("for object %s, param name %d is: %s\n",
               instanceName.c_str(), ii, paramNames.at(ii).c_str());
         MessageInterface::ShowMessage("... and extracted name is: %s\n", objName.c_str());
      #endif
      if (objName == withName) return true;
      // If there are more than two parts when separated by dots, then there is
      // a dependency, so we need to check it for the object
      if (byDots.size() > 2)
      {
         objName = GmatStringUtil::Trim(GmatStringUtil::GetArrayName(byDots.at(1)));
         #ifdef DEBUG_OBJECT_REF
            MessageInterface::ShowMessage("for object %s, param name %d is: %s\n",
                  instanceName.c_str(), ii, paramNames.at(ii).c_str());
            MessageInterface::ShowMessage("... and 2nd extracted name is: %s\n", objName.c_str());
         #endif
         if (objName == withName) return true;
      }
   }

   return false;
}


//------------------------------------------------------------------------------
//  void SetObjectMap(std::map<std::string, GmatBase *> *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the local asset store used by the GmatCommand
 * 
 * @param <map> Pointer to the local object map
 */
//------------------------------------------------------------------------------
void CallFunction::SetObjectMap(std::map<std::string, GmatBase *> *map)
{
   GmatCommand::SetObjectMap(map);
   fm.SetObjectMap(map);
}


//------------------------------------------------------------------------------
//  void SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
//------------------------------------------------------------------------------
/**
 * Called by the Sandbox to set the global asset store used by the GmatCommand
 * 
 * @param <map> Pointer to the local object map
 */
//------------------------------------------------------------------------------
void CallFunction::SetGlobalObjectMap(std::map<std::string, GmatBase *> *map)
{
   #ifdef DEBUG_GLOBAL_OBJECT_MAP
   MessageInterface::ShowMessage
      ("CallFunction::SetGlobalObjectMap() entered, mFunctionName='%s', "
       "map=<%p>\n", mFunctionName.c_str(), map);
   #endif
   
   GmatCommand::SetGlobalObjectMap(map);
   
   // Now, find the function object
   GmatBase *mapObj = FindObject(mFunctionName);
   
   #ifdef DEBUG_GLOBAL_OBJECT_MAP
   MessageInterface::ShowMessage
      ("   mapObj=<%p><%s>'%s'\n", mapObj,
       mapObj ? mapObj->GetTypeName().c_str() : "NULL",
       mapObj ? mapObj->GetName().c_str() : "NULL");
   #endif
   
   if (mapObj == NULL)
   {
      //throw CommandException("CallFunction command cannot find Function " +
      //         mFunctionName + "\n");
      ; // leave NULL for now
   }
   else
   {
      mFunction = (Function *)mapObj;
      
      #ifdef DEBUG_GLOBAL_OBJECT_MAP
      MessageInterface::ShowMessage
         ("   mFunction=<%p><%s>\n", mFunction, mFunction->GetName().c_str());
      #endif
      
      // Set GmatFunction to FunctionManager (loj: 2008.09.03)
      // Set BuiltinGmatFunction to FunctionManager (loj: 2016.05.04)
      //if (mapObj->GetTypeName() == "GmatFunction")
      if (mapObj->IsOfType("GmatFunction") || mapObj->IsOfType("BuiltinGmatFunction"))
      {
         #ifdef DEBUG_FUNCTION
         MessageInterface::ShowMessage
            ("CallFunction::SetGlobalObjectMap() setting function<%p>'%s' to FunctionManager\n",
             mFunction, mFunction ? mFunction->GetName().c_str() : "NULL");
         #endif
         mFunction->SetCallDescription(GetGeneratingString(Gmat::NO_COMMENTS));
         fm.SetFunction(mFunction);
      }
   }
   fm.SetGlobalObjectMap(map);
   
   #ifdef DEBUG_GLOBAL_OBJECT_MAP
   MessageInterface::ShowMessage("CallFunction::SetGlobalObjectMap() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// bool HasAFunction()
//------------------------------------------------------------------------------
bool CallFunction::HasAFunction()
{
   return true;
}


//------------------------------------------------------------------------------
// virtual bool IsMatlabFunctionCall()
//------------------------------------------------------------------------------
bool CallFunction::IsMatlabFunctionCall()
{
   if (isMatlabFunction)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the CallFunction.
 *
 * @return clone of the CallFunction.
 *
 */
//------------------------------------------------------------------------------
GmatBase* CallFunction::Clone() const
{
   return (new CallFunction(*this));
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string CallFunction::GetParameterText(const Integer id) const
{
   #ifdef DEBUG_CALL_FUNCTION_PARAM
      MessageInterface::ShowMessage("CallFunction::GetParameterText\n");
   #endif

   if (id >= GmatCommandParamCount && id < CallFunctionParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterText(id);
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
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name.  (Used to
 *                indent commands)
 * @param useName Name that replaces the object's name.  (Not used in
 *                commands)
 *
 * @return The script line that, when interpreted, defines this CallFunction.
 */
//------------------------------------------------------------------------------
const std::string& CallFunction::GetGeneratingString(Gmat::WriteMode mode,
                                                     const std::string &prefix,
                                                     const std::string &useName)
{
   std::string gen;
   
   // Build the local string
   if (mode != Gmat::NO_COMMENTS)
   {
      bool writeGmatKeyword = GmatGlobal::Instance()->IsWritingGmatKeyword();
      // We now write out GMAT prefix on option from the startup file (see GMT-3233)
      if (writeGmatKeyword)
         gen = prefix + "GMAT ";
      else
         gen = prefix;
   }
   
   if (mOutputNames.size() > 0)
   {
      gen += "[";
      for (StringArray::iterator i = mOutputNames.begin();
           i != mOutputNames.end(); ++i)
      {
         if (i != mOutputNames.begin())
            gen += ", ";
         gen += *i;
      }
      gen += "] = ";
   }
   
   gen += mFunctionName;
   
   if (mInputNames.size() > 0)
   {
      gen += "(";
      for (StringArray::iterator i = mInputNames.begin();
           i != mInputNames.end(); ++i)
      {
         if (i != mInputNames.begin())
            gen += ", ";
         gen += *i;
      }
      gen += ")";
   }
   
   generatingString = gen + ";";
   
   if (mode == Gmat::NO_COMMENTS)
   {
	  InsertCommandName(generatingString);
      return generatingString;
   }
   
   // Then call the base class method
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer CallFunction::GetParameterID(const std::string &str) const
{
   #ifdef DEBUG_CALL_FUNCTION_PARAM
      MessageInterface::ShowMessage("CallFunction::GetParameterID \n");
   #endif

   for (int i=GmatCommandParamCount; i<CallFunctionParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType CallFunction::GetParameterType(const Integer id) const
{
   #ifdef DEBUG_CALL_FUNCTION_PARAM
      MessageInterface::ShowMessage("CallFunction::GetParameterType\n");
   #endif

   if (id >= GmatCommandParamCount && id < CallFunctionParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string CallFunction::GetParameterTypeString(const Integer id) const
{
   #ifdef DEBUG_CALL_FUNCTION_PARAM
      MessageInterface::ShowMessage("CallFunction::GetParameterTypeString\n");
   #endif

   if (id >= GmatCommandParamCount && id < CallFunctionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - GmatCommandParamCount)];
   else
      return GmatCommand::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string CallFunction::GetStringParameter(const Integer id) const
{
   #ifdef DEBUG_CALL_FUNCTION_PARAM
      MessageInterface::ShowMessage("CallFunction::GetStringParameter\n");
   #endif

   switch (id)
   {
   case FUNCTION_NAME:
      return fm.GetFunctionName();
      //return mFunctionName;
   default:
      return GmatCommand::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string CallFunction::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool CallFunction::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_CALL_FUNCTION_PARAM
      MessageInterface::ShowMessage
         ("CallFunction::SetStringParameter with id = %d and value = %s\n",
          id, value.c_str());
   #endif
      
   switch (id)
   {
   case FUNCTION_NAME:
      mFunctionName = value;
      mFunctionPathAndName = value;
      fm.SetFunctionName(value);
      return true;
   case ADD_INPUT:
      return AddInputParameter(value, mNumInputParams);
   case ADD_OUTPUT:
      return AddOutputParameter(value, mNumOutputParams);
   default:
      return GmatCommand::SetStringParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
bool CallFunction::SetStringParameter(const std::string &label,
                                      const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool CallFunction::SetStringParameter(const std::string &label,
                                const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const Integer id, const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool CallFunction::SetStringParameter(const Integer id, const std::string &value,
                                const Integer index)
{
   switch (id)
   {
   case ADD_INPUT:
      return AddInputParameter(value, index);
   case ADD_OUTPUT:
      return AddOutputParameter(value, index);
   default:
      return GmatCommand::SetStringParameter(id, value, index);
   }
}


//------------------------------------------------------------------------------
// virtual bool SetStringParameter(const std::string &label,
//                                 const std::string &value,
//                                 const Integer index)
//------------------------------------------------------------------------------
bool CallFunction::SetStringParameter(const std::string &label,
                                const std::string &value,
                                const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& CallFunction::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
   case ADD_INPUT:
      return mInputNames;
   case ADD_OUTPUT:
      return mOutputNames;
   default:
      return GmatCommand::GetStringArrayParameter(id);
   }
}


//------------------------------------------------------------------------------
// StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
const StringArray& CallFunction::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
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
bool CallFunction::TakeAction(const std::string &action,
                        const std::string &actionData)
{
   if (action == "ClearInput")
   {
      ClearInputParameters();
      return true;
   }
   else if (action == "ClearOutput")
   {
      ClearOutputParameters();
      return true;
   }
   else if (action == "Clear")
   {
      ClearInputParameters();
      ClearOutputParameters();
      objectArray.clear();
      return true;
   }

   return GmatCommand::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// StringArray GetRefObjectNameArray(const UnsignedInt type) const
//------------------------------------------------------------------------------
const StringArray& CallFunction::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();
   
   if (type == Gmat::PARAMETER)
   {
      for (unsigned int i=0; i<mInputNames.size(); i++)
         refObjectNames.push_back(mInputNames[i]);
      for (unsigned int i=0; i<mOutputNames.size(); i++)
         refObjectNames.push_back(mOutputNames[i]);
      return refObjectNames;
   }
   
   return refObjectNames;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool CallFunction::RenameRefObject(const UnsignedInt type,
                                   const std::string &oldName,
                                   const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("CallFunction::RenameRefObject() type=%d, oldName='%s', newName='%s'\n",
       type, oldName.c_str(), newName.c_str());
   #endif
   
   if (type == Gmat::FUNCTION)
   {
      if (mFunctionName == oldName)
         mFunctionName = newName;
   }
   else if (type == Gmat::PARAMETER)
   {
      // parameters - go through input and output
      for (unsigned int i=0; i<mInputNames.size(); i++)
      {
         if (mInputNames[i] == oldName)
         {
            mInputNames[i] = newName;
            break;
         }
         std::string arrName = GmatStringUtil::GetArrayName(mInputNames[i], "[]");
         if (arrName == oldName)
         {
            mInputNames[i] =
               GmatStringUtil::Replace(mInputNames[i], oldName, newName);
         }
      }

      for (unsigned int i=0; i<mOutputNames.size(); i++)
      {
         if (mOutputNames[i] == oldName)
         {
            mOutputNames[i] = newName;
            break;
         }
         std::string arrName = GmatStringUtil::GetArrayName(mOutputNames[i], "[]");
         if (arrName == oldName)
         {
            mOutputNames[i] =
               GmatStringUtil::Replace(mOutputNames[i], oldName, newName);
         }
      }
   }
   // Since parameter name is composed of spacecraftName.dep.paramType or
   // burnName.dep.paramType, check the type first
   else if (type == Gmat::SPACECRAFT || type == Gmat::BURN || type == Gmat::PLATE || 
            type == Gmat::HARDWARE   || type == Gmat::IMPULSIVE_BURN ||
            type == Gmat::COORDINATE_SYSTEM || type == Gmat::CALCULATED_POINT)
   {
      
      for (UnsignedInt i=0; i<mInputNames.size(); i++)
         if (mInputNames[i].find(oldName) != std::string::npos)
            mInputNames[i] =
               GmatStringUtil::Replace(mInputNames[i], oldName, newName);
      
      for (UnsignedInt i=0; i<mOutputNames.size(); i++)
         if (mOutputNames[i].find(oldName) != std::string::npos)
            mOutputNames[i] =
               GmatStringUtil::Replace(mOutputNames[i], oldName, newName);
   }

   return true;
}


// Reference object accessor methods
//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
GmatBase* CallFunction::GetRefObject(const UnsignedInt type,
                                     const std::string &name)
{
   switch (type)
   {
      case Gmat::PARAMETER:
         for (int i=0; i<mNumInputParams; i++)
         {
            if (mInputNames[i] == name)
               return mInputList[i];
         }
         
         for (int i=0; i<mNumOutputParams; i++)
         {
            if (mOutputNames[i] == name)
               return mOutputList[i];
         }
         
         throw GmatBaseException("ReportFile::GetRefObject() the object name: "
                           + name + "not found\n");
         
      case Gmat::FUNCTION:
         return mFunction;
         
      case Gmat::COMMAND:
         return callcmds;
         
      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return GmatCommand::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type, ...
//------------------------------------------------------------------------------
/**
 * Sets reference object pointer.
 *
 * @return true if object successfully set, false otherwise
 */
//------------------------------------------------------------------------------
bool CallFunction::SetRefObject(GmatBase *obj, const UnsignedInt type,
                                const std::string &name)
{
   #ifdef DEBUG_CALL_FUNCTION_REF_OBJ
   MessageInterface::ShowMessage
      ("CallFunction::SetRefObject() entered, obj=<%p><%s>'%s', type=%d, name='%s'\n",
       obj, obj ? obj->GetTypeName().c_str() : "NULL", obj ? obj->GetName().c_str() : "NULL",
       type, name.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   switch (type)
   {
   case Gmat::PARAMETER:
      for (int i=0; i<mNumInputParams; i++)
      {
         if (mInputNames[i] == name)
         {
            mInputList[i] = (Parameter*)obj;
            return true;
         }
      }

      for (int i=0; i<mNumOutputParams; i++)
      {
         if (mOutputNames[i] == name)
         {
            mOutputList[i] = (Parameter*)obj;
            return true;
         }
      }
      
   case Gmat::FUNCTION:
      if (name == mFunctionName)
      {
         mFunction = (Function *)obj;
         if (mFunction)
         {
            mFunctionPathAndName = mFunction->GetFunctionPathAndName();
            if (mFunction->IsOfType("BuiltinGmatFunction"))
            {
               #ifdef DEBUG_CALL_FUNCTION_REF_OBJ
               MessageInterface::ShowMessage
                  ("CallFunction::SetRefObject(), '%s' is builtin GMAT function\n",
                   mFunctionName.c_str());
               MessageInterface::ShowMessage
                  ("   Setting function<%p>'%s' to FunctionManager\n",
                   mFunction, mFunction ? mFunction->GetName().c_str() : "NULL");
               #endif
               fm.SetFunction(mFunction);
               isGmatFunction = false;
               isMatlabFunction = false;
               isBuiltinGmatFunction = true;
            }
            else if (mFunction->GetTypeName() == "GmatFunction")
            {
               #ifdef DEBUG_FUNCTION
               MessageInterface::ShowMessage
                  ("CallFunction::SetRefObject() Setting function<%p>'%s' to FunctionManager\n",
                   mFunction, mFunction ? mFunction->GetName().c_str() : "NULL");
               #endif
               fm.SetFunction(mFunction);
               isGmatFunction = true;
               isMatlabFunction = false;
               isBuiltinGmatFunction = false;
            }
         }
      }
      return true;
      
   case Gmat::COMMAND:
      if (callcmds)
         delete callcmds;
      callcmds = (GmatCommand*)obj;
      return true;
      
   default:
      break;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return GmatCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// virtual ObjectArray& GetRefObjectArray(const UnsignedInt type)
//------------------------------------------------------------------------------
ObjectArray& CallFunction::GetRefObjectArray(const UnsignedInt type)
{
   switch (type)
   {
   case Gmat::PARAMETER:
      objectArray.clear();

      for (unsigned int i=0; i<mInputList.size(); i++)
         objectArray.push_back(mInputList[i]);
      
      for (unsigned int i=0; i<mOutputList.size(); i++)
         objectArray.push_back(mOutputList[i]);
      
      return objectArray;
      
   default:
      break;
   }

   // Not handled here -- invoke the next higher SetReferenceObject call
   return GmatCommand::GetRefObjectArray(type);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool CallFunction::Initialize()
{
   #ifdef DEBUG_CALL_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("CallFunction::Initialize() this=<%p> entered\n   command = '%s'\n   "
          "function type is '%s', callingFunction is '%s', isBuiltinGmatFunction=%d\n",
          this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
          mFunction ? mFunction->GetTypeName().c_str() : "NULL",
          callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL",
          isBuiltinGmatFunction);
   #endif
   
   GmatCommand::Initialize();
   
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps("In CallFunction::Initialize()");
   #endif
   
   isGmatFunction = false;
   isMatlabFunction = false;
   isBuiltinGmatFunction = false;
   
   bool rv = true;  // Initialization return value
   if (!IsOfType("CallPythonFunction"))
   {
      if (mFunction == NULL)
         throw CommandException("CallFunction::Initialize() the function pointer is NULL");
      
      if (mFunction->IsOfType("BuiltinGmatFunction"))
         isBuiltinGmatFunction = true;
      else if (mFunction->GetTypeName() == "GmatFunction")
         isGmatFunction = true;
      else if (mFunction->GetTypeName() == "MatlabFunction")
         isMatlabFunction = true;
      
      if (!isGmatFunction && !isMatlabFunction && !isBuiltinGmatFunction)
         throw CommandException
            ("CallFunction::Initialize() the function is not a GmatFunction, "
             "MatlabFunction, or BuiltinGmatFunction");
      
      mFunctionPathAndName = mFunction->GetFunctionPathAndName();
      std::string fname = GmatFileUtil::ParseFileName(mFunctionPathAndName);
      if (fname == "")
         mFunctionPathAndName += mFunctionName;

      #ifdef DEBUG_CALL_FUNCTION_INIT
      MessageInterface::ShowMessage
         ("CallFunction::Initialize() returning %d, fname='%s', mFunctionName='%s', ...\n   "
          "mFunctionPathAndName='%s'\n", rv, fname.c_str(), mFunctionName.c_str(),
          mFunctionPathAndName.c_str());
      #endif
   }
   
   return rv;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
bool CallFunction::Execute()
{
   bool status = false;
   
   if (mFunction == NULL)
      throw CommandException("Function is not defined for CallFunction");
   
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;      
   clock_t t1 = clock();
   MessageInterface::ShowMessage
      (">>>>> CallFunction::Execute() entered, '%s' Count = %d\n",
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), callCount);
   #endif
   
   #ifdef DEBUG_CALL_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("CallFunction::Execute() this=<%p> entered, command = '%s'\n   "
          "function type is '%s', callingFunction is '%s'\n", this,
          GetGeneratingString(Gmat::NO_COMMENTS).c_str(), mFunction->GetTypeName().c_str(),
          callingFunction? (callingFunction->GetFunctionName()).c_str() : "NULL");
      #ifdef DEBUG_OBJECT_MAP
      ShowObjectMaps("object maps at the start");
      #endif
   #endif
      
   return status;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void CallFunction::RunComplete()
{
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("CallFunction::RunComplete() entered, this=<%p> '%s',\n   "
       "FCS %sfinalized\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       fm.IsFinalized() ? "already " : "NOT ");
   #endif
   
   if (!fm.IsFinalized())
   {
      #ifdef DEBUG_RUN_COMPLETE
      MessageInterface::ShowMessage("   Calling FunctionManager::Finalize()\n");
      #endif
      fm.Finalize();
   }
   
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage("   Calling GmatCommand::RunComplete()\n");
   #endif
   
   GmatCommand::RunComplete();
   
   #ifdef DEBUG_RUN_COMPLETE
   MessageInterface::ShowMessage
      ("CallFunction::RunComplete() leaving, this=<%p> '%s',\n   "
       "FCS %sfinalized\n", this, GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
       fm.IsFinalized() ? "already " : "NOT ");
   #endif
}


//------------------------------------------------------------------------------
// void ClearInputParameters()
//------------------------------------------------------------------------------
void CallFunction::ClearInputParameters()
{
   mInputList.clear();
   mInputNames.clear();
   mNumInputParams = 0;
}


//------------------------------------------------------------------------------
// void ClearOutputParameters()
//------------------------------------------------------------------------------
void CallFunction::ClearOutputParameters()
{
   mOutputList.clear();
   mOutputNames.clear();
   mNumOutputParams = 0;
}


//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 *  Sets the internal coordinate system used by the Sandbox.
 *
 *  @param <cs> The internal coordinate system.
 */
//------------------------------------------------------------------------------
void CallFunction::SetInternalCoordSystem(CoordinateSystem *cs)
{
   /// @todo Check initialization and cloning for the internal CoordinateSystem.
   //internalCoordSys = (CoordinateSystem*)(cs->Clone());
   internalCoordSys = cs;
   fm.SetInternalCoordinateSystem(internalCoordSys);
}


//------------------------------------------------------------------------------
// void SetPublisher(Publisher *pub)
//------------------------------------------------------------------------------
/**
 *  Passes the Publisher used by the Sandbox to FunctionManager
 *
 *  @param <pub> The publisher
 */
//------------------------------------------------------------------------------
void CallFunction::SetPublisher(Publisher *pub)
{
   #ifdef DEBUG_PUBLISHER
   MessageInterface::ShowMessage
      ("CallFunction::SetPublisher() setting publiser <%p> to FunctionManager\n", pub);
   #endif
   GmatCommand::SetPublisher(pub);
   fm.SetPublisher(pub);
}


