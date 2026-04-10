//$Id$
//------------------------------------------------------------------------------
//                             ObjectManagedFunction
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.10.20
//
/**
 * Implements the ObjectManagedFunction base class for BuiltinGmatFunction and
 * UserDefinedFunction
 */
//------------------------------------------------------------------------------

#include "ObjectManagedFunction.hpp"
#include "FunctionException.hpp"    // for exception
#include "StringUtil.hpp"           // for GmatStringUtil::
#include "Parameter.hpp"            // for GetOwner()
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION_SET
//#define DEBUG_FUNCTION_IN_OUT
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_FUNCTION_OBJ
//#define DEBUG_OBJECT_MAP


//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif


//---------------------------------
// static data
//---------------------------------


//------------------------------------------------------------------------------
//  ObjectManagedFunction(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the ObjectManagedFunction object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
ObjectManagedFunction::ObjectManagedFunction(const std::string &typeStr, const std::string &name) :
   Function           (typeStr, name),
   objectStore        (NULL),
   globalObjectStore  (NULL),
   changingInputType  (false)
{
   if (typeStr != "")
      objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("ObjectManagedFunction");
}


//------------------------------------------------------------------------------
//  ~ObjectManagedFunction(void)
//------------------------------------------------------------------------------
/**
 * Destroys the ObjectManagedFunction object (destructor).
 */
//------------------------------------------------------------------------------
ObjectManagedFunction::~ObjectManagedFunction()
{
}


//------------------------------------------------------------------------------
//  ObjectManagedFunction(const ObjectManagedFunction &f)
//------------------------------------------------------------------------------
/**
 * Constructs the ObjectManagedFunction object (copy constructor).
 * 
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
ObjectManagedFunction::ObjectManagedFunction(const ObjectManagedFunction &f) :
   Function           (f),
   inputArgMap        (f.inputArgMap), // do I want to do this?
   outputArgMap       (f.outputArgMap), // do I want to do this?
   objectStore        (NULL),
   globalObjectStore  (NULL),
   changingInputType  (false)
{
}


//------------------------------------------------------------------------------
//  ObjectManagedFunction& operator=(const ObjectManagedFunction &f)
//------------------------------------------------------------------------------
/**
 * Sets one ObjectManagedFunction object to match another (assignment operator).
 * 
 * @param <f> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
ObjectManagedFunction& ObjectManagedFunction::operator=(const ObjectManagedFunction &f)
{
   if (this == &f)
      return *this;
   
   Function::operator=(f);
   
   objectStore        = NULL;
   globalObjectStore  = NULL;
   inputArgMap        = f.inputArgMap;   // do I want to do this?
   outputArgMap       = f.outputArgMap;  // do I want to do this?
   changingInputType  = f.changingInputType;
   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize = false)
//------------------------------------------------------------------------------
bool ObjectManagedFunction::Initialize(ObjectInitializer *objInit,
                                       bool reinitialize,
                                       bool resetLocalObjects)
{
   Function::Initialize(objInit, reinitialize);

   return true;
}


//------------------------------------------------------------------------------
// bool ObjectManagedFunction::Execute(ObjectInitializer *objInit)  [default implementation]
//------------------------------------------------------------------------------
bool ObjectManagedFunction::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   return true;
}


//------------------------------------------------------------------------------
// bool ObjectManagedFunction::Finalize(bool cleanUp = false)  [default implementation]
//------------------------------------------------------------------------------
void ObjectManagedFunction::Finalize(bool cleanUp)
{
   // Anything to do here?
}

//------------------------------------------------------------------------------
// void SetFunctionObjectMap(ObjectMap *objMap)
//------------------------------------------------------------------------------
void ObjectManagedFunction::SetFunctionObjectMap(ObjectMap *objMap)
{
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMap(objMap, "In ObjectManagedFunction::SetFunctionObjectMap",
                 "Input Object Map");
   #endif
   objectStore = objMap;
}

//------------------------------------------------------------------------------
// ObjectMap* SetFunctionObjectMap()
//------------------------------------------------------------------------------
ObjectMap* ObjectManagedFunction::GetFunctionObjectStore()
{
   return objectStore;
}

//------------------------------------------------------------------------------
// void SetGlobalObjectMap(ObjectMap *objMap)
//------------------------------------------------------------------------------
void ObjectManagedFunction::SetGlobalObjectMap(ObjectMap *objMap)
{
   globalObjectStore = objMap;
}


//------------------------------------------------------------------------------
// bool SetInputElementWrapper(const std::string &forName, ElementWrapper *wrapper)
//------------------------------------------------------------------------------
bool ObjectManagedFunction::SetInputElementWrapper(const std::string &forName, ElementWrapper *wrapper)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("ObjectManagedFunction::SetInputElementWrapper() entered, wrapper name \"%s\"\n", forName.c_str());
   MessageInterface::ShowMessage
      ("   wrapper=<%p>, wrapper type = %d, inputArgMap.size()=%d\n", wrapper,
       wrapper->GetWrapperType(), inputArgMap.size());
   #endif
   
   if (inputArgMap.find(forName) == inputArgMap.end())
   {
      #ifdef DEBUG_FUNCTION_IN_OUT
      MessageInterface::ShowMessage
         ("**** ERROR **** ObjectManagedFunction::SetInputElementWrapper() throwing exception: "
          "Input name '%s' not found in inputArgMap\n", forName.c_str());
      #endif
      std::string errMsg = "Unknown input argument \"" + forName;
      errMsg += "\" for function \"" + functionName + "\"";
      throw FunctionException(errMsg);
   }
   
   inputArgMap[forName] = wrapper;
   
   //@note old inputWrappers are deleted in the FunctionManager::CreateFunctionArgWrappers()
   // before creates new wrappers for input arguments
   
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("ObjectManagedFunction::SetInputElementWrapper() leaving, wrapper name \"%s\"\n", forName.c_str());
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// virtual ElementWrapper*  GetOutputArgument(Integer argNumber)
//------------------------------------------------------------------------------
/*
 * Implements GMAT FUNCTIONS design 27.2.2.3 GmatFunction Execution
 * step 4 of "Steps Performed on the Firstexecution"
 */
//------------------------------------------------------------------------------
ElementWrapper* ObjectManagedFunction::GetOutputArgument(Integer argNumber)
{
   if ((argNumber < 0) || (argNumber > (Integer) outputNames.size()) ||
       (argNumber> (Integer) outputArgMap.size()))
   {
      std::string errMsg = "Function error: argument number out-of-range\n";
      throw FunctionException(errMsg);
   }
   std::string argName = outputNames.at(argNumber);
   return GetOutputArgument(argName);
}


//------------------------------------------------------------------------------
// ElementWrapper* GetOutputArgument(const std::string &byName)
//------------------------------------------------------------------------------
ElementWrapper* ObjectManagedFunction::GetOutputArgument(const std::string &byName)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("ObjectManagedFunction::GetOutputArgument(%s) entered, outputArgMap.size()=%d\n",
       byName.c_str(), outputArgMap.size());
   #endif
   
   if (outputArgMap.find(byName) == outputArgMap.end())
   {
      std::string errMsg = "Function error: output \"" + byName;
      errMsg += "\" from function \"" + functionName;
      errMsg += "\" does not exist.\n";
      throw FunctionException(errMsg);
   }
   
   ElementWrapper *ew = outputArgMap[byName];
   
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("ObjectManagedFunction::GetOutputArgument(%s) returning <%p>, type=%d\n", byName.c_str(),
       ew, ew ? ew->GetDataType() : -999);
   #endif
   
   return ew;
}


//------------------------------------------------------------------------------
// WrapperArray& GetWrappersToDelete()
//------------------------------------------------------------------------------
WrapperArray& ObjectManagedFunction::GetWrappersToDelete()
{
   return wrappersToDelete;
}


//------------------------------------------------------------------------------
// void ClearInOutArgMaps(bool deleteInputs, bool deleteOutputs)
//------------------------------------------------------------------------------
void ObjectManagedFunction::ClearInOutArgMaps(bool deleteInputs, bool deleteOutputs)
{
   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage
      ("ObjectManagedFunction::ClearInOutArgMaps() this=<%p> '%s' entered\n", this,
       GetName().c_str());
   MessageInterface::ShowMessage
      ("inputArgMap.size()=%d, outputArgMap.size()=%d\n", inputArgMap.size(),
       outputArgMap.size());
   #endif
   
   std::vector<ElementWrapper *> wrappersToDelete;
   std::map<std::string, ElementWrapper *>::iterator ewi;
   
   if (deleteInputs)
   {
      // input wrappers map
      for (ewi = inputArgMap.begin(); ewi != inputArgMap.end(); ++ewi)
      {
         if (ewi->second)
         {         
            if (find(wrappersToDelete.begin(), wrappersToDelete.end(), ewi->second) ==
                wrappersToDelete.end())
               wrappersToDelete.push_back(ewi->second);
         }
      }
   }
   
   if (deleteOutputs)
   {
      // output wrappers
      for (ewi = outputArgMap.begin(); ewi != outputArgMap.end(); ++ewi)
      {
         if (ewi->second)
         {
            if (find(wrappersToDelete.begin(), wrappersToDelete.end(), ewi->second) ==
                wrappersToDelete.end())
               wrappersToDelete.push_back(ewi->second);
         }
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
         ((*ewi), (*ewi)->GetDescription(), "ObjectManagedFunction::ClearInOutArgMaps()",
          "deleting wrapper");
      #endif
      delete (*ewi);
      (*ewi) = NULL;
   }
   
   inputArgMap.clear();
   outputArgMap.clear();
}

//------------------------------------------------------------------------------
// void SetChangingInputType(bool hasChangingInput)
//------------------------------------------------------------------------------
/**
 * Set whether this function will have changing types of inputs. This info is
 * utilized for knowing when reinitialization is necessary.
 *
 * @param <hasChangingInput> Whether the function input type will change
 *
 */
//------------------------------------------------------------------------------
void ObjectManagedFunction::SetChangingInputType(bool hasChangingInput)
{
   changingInputType = hasChangingInput;
}

//------------------------------------------------------------------------------
// bool HasChangingInputType()
//------------------------------------------------------------------------------
/**
 * Get whether this function will have changing types of inputs. This info is
 * utilized for knowing when reinitialization is necessary.
 *
 */
//------------------------------------------------------------------------------
bool ObjectManagedFunction::HasChangingInputType()
{
   return changingInputType;
}

//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,
//                         const std::string &actionData = "")
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action ("")
 * @return true if action successfully performed
 *
 */
//------------------------------------------------------------------------------
bool ObjectManagedFunction::TakeAction(const std::string &action,
                                       const std::string &actionData)
{
   #ifdef DEBUG_FUNCTION_ACTION
   MessageInterface::ShowMessage
      ("ObjectManagedFunction::TakeAction() action=%s, actionData=%s\n", action.c_str(),
       actionData.c_str());
   #endif
   
   return Function::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const char *value)
//------------------------------------------------------------------------------
/**
 * @see SetStringParameter(const Integer id, const std::string &value)
 */
//------------------------------------------------------------------------------
bool ObjectManagedFunction::SetStringParameter(const Integer id, const char *value)
{
   return SetStringParameter(id, std::string(value));
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return If value of the parameter was set.
 */
//------------------------------------------------------------------------------
bool ObjectManagedFunction::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("ObjectManagedFunction::SetStringParameter() entered, id=%d, value='%s'\n", id, value.c_str());
   #endif
   
   switch (id)
   {
   case FUNCTION_INPUT:
      {
         #ifdef DEBUG_FUNCTION_SET
         MessageInterface::ShowMessage("   Adding function input name '%s'\n", value.c_str());
         #endif
         
         // Ignore () as input, so that "function MyFunction()" will work as no input (LOJ: 2015.08.19)
         if (value == "()")
            return true;
         
         if (inputArgMap.find(value) == inputArgMap.end())
         {
            inputNames.push_back(value);
            inputArgMap.insert(std::make_pair(value,(ElementWrapper*) NULL));
         }
         else
            throw FunctionException
               ("In function file \"" + functionPath + "\": "
                "The input argument \"" + value + "\" already exists");
         
         return Function::SetStringParameter(id, value);
      }
   case FUNCTION_OUTPUT:
      {
         #ifdef DEBUG_FUNCTION_SET
         MessageInterface::ShowMessage("   Adding function output name '%s'\n", value.c_str());
         #endif
         if (outputArgMap.find(value) == outputArgMap.end())
         {
            outputNames.push_back(value);
            outputArgMap.insert(std::make_pair(value,(ElementWrapper*) NULL));
         }
         else
            throw FunctionException
               ("In function file \"" + functionPath + "\": "
                "The output argument \"" + value + "\" already exists");
         
         return Function::SetStringParameter(id, value);
      }
   default:
      return Function::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
bool ObjectManagedFunction::SetStringParameter(const std::string &label,
                                               const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool ObjectManagedFunction::SetStringParameter(const std::string &label,
                                               const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//---------------------------------
// protected
//---------------------------------

//------------------------------------------------------------------------------
// void ShowObjectMap(ObjectMap *objMap, const std::string &title,
//                    const std::string &mapName)
//------------------------------------------------------------------------------
void ObjectManagedFunction::ShowObjectMap(ObjectMap *objMap, const std::string &title,
                                          const std::string &mapName)
{
   MessageInterface::ShowMessage("%s\n", title.c_str());
   MessageInterface::ShowMessage("this=<%p>, functionName='%s'\n", this, functionName.c_str());
   if (objMap == NULL)
   {
      MessageInterface::ShowMessage("%s is NULL\n", mapName.c_str());
      return;
   }
   
   std::string objMapName = mapName;
   GmatBase *obj = NULL;
   std::string objName;
   std::string objTypeName = "NULL";
   std::string isGlobal;
   std::string isLocal;
   GmatBase *paramOwner = NULL;
   bool isParameter = false;
   std::string paramOwnerType;
   std::string paramOwnerName;
   
   if (objMapName == "")
      objMapName = "object map";
   
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage
      ("Here is %s <%p>, it has %d objects\n", objMapName.c_str(), objMap, objMap->size());
   for (ObjectMap::iterator i = objMap->begin(); i != objMap->end(); ++i)
   {
      obj = i->second;
      objName = i->first;
      objTypeName = "";
      isGlobal = "No";
      isLocal = "No";
      paramOwner = NULL;
      isParameter = false;
      paramOwnerType = "";
      paramOwnerName = "";
      
      if (obj)
      {
         objTypeName = obj->GetTypeName();
         if (obj->IsGlobal())
            isGlobal = "Yes";
         if (obj->IsLocal())
            isLocal = "Yes";
         if (obj->IsOfType(Gmat::PARAMETER))
         {
            isParameter = true;
            paramOwner = ((Parameter*)obj)->GetOwner();
            if (paramOwner)
            {
               paramOwnerType = paramOwner->GetTypeName();
               paramOwnerName = paramOwner->GetName();
            }
         }
      }
      MessageInterface::ShowMessage
         ("   %60s  <%p>  %-16s  IsGlobal:%-3s  IsLocal:%-3s", objName.c_str(), obj,
          objTypeName.c_str(), isGlobal.c_str(), isLocal.c_str());
      if (isParameter)
         MessageInterface::ShowMessage
            ("  ParameterOwner: <%p>[%s]'%s'\n", paramOwner, paramOwnerType.c_str(),
             paramOwnerName.c_str());
      else
         MessageInterface::ShowMessage("\n");
   }
}

