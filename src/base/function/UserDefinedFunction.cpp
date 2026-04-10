//$Id$
//------------------------------------------------------------------------------
//                             UserDefinedFunction
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
 * Implements UserDefinedFunction class. All user defined function classes
 * such as GmatFunction are derived from this class.
 */
//------------------------------------------------------------------------------

#include "UserDefinedFunction.hpp"
#include "FunctionException.hpp"    // for exception
#include "StringUtil.hpp"           // for GmatStringUtil::
#include "Parameter.hpp"            // for GetOwner()
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION_SET
//#define DEBUG_FUNCTION_FINALIZE

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
//  UserDefinedFunction(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the UserDefinedFunction object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
UserDefinedFunction::UserDefinedFunction(const std::string &typeStr, const std::string &name) :
   ObjectManagedFunction(typeStr, name),
   fcs                (NULL),
   fcsInitialized     (false),
   fcsFinalized       (false),
   validator          (NULL),
   objectsInitialized (false)
{
   if (typeStr != "")
      objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("UserDefinedFunction");
}


//------------------------------------------------------------------------------
//  ~UserDefinedFunction(void)
//------------------------------------------------------------------------------
/**
 * Destroys the UserDefinedFunction object (destructor).
 */
//------------------------------------------------------------------------------
UserDefinedFunction::~UserDefinedFunction()
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("UserDefinedFunction::~UserDefinedFunction() <%p>[%s]'%s' entered\n", this, GetTypeName().c_str(),
       GetName().c_str());
   #endif
   
   // Clear original objects
   ClearAutomaticObjects();
   ClearFunctionObjects();
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("UserDefinedFunction::~UserDefinedFunction() <%p>[%s]'%s' leaving\n", this, GetTypeName().c_str(),
       GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  UserDefinedFunction(const UserDefinedFunction &f)
//------------------------------------------------------------------------------
/**
 * Constructs the UserDefinedFunction object (copy constructor).
 * 
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
UserDefinedFunction::UserDefinedFunction(const UserDefinedFunction &f) :
   ObjectManagedFunction (f),
   fcs                (NULL),
   fcsInitialized     (f.fcsInitialized),
   fcsFinalized       (f.fcsFinalized),
   functionObjectMap  (f.functionObjectMap), // Do I want to do this?
   validator          (f.validator),
   objectsInitialized (false)
{
   //parameterCount = UserDefinedFunctionParamCount;
}


//------------------------------------------------------------------------------
//  UserDefinedFunction& operator=(const UserDefinedFunction &f)
//------------------------------------------------------------------------------
/**
 * Sets one UserDefinedFunction object to match another (assignment operator).
 * 
 * @param <f> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
UserDefinedFunction& UserDefinedFunction::operator=(const UserDefinedFunction &f)
{
   if (this == &f)
      return *this;
   
   ObjectManagedFunction::operator=(f);
   
   fcs                = NULL;
   fcsInitialized     = f.fcsInitialized;
   fcsFinalized       = f.fcsFinalized;
   validator          = f.validator;
   objectsInitialized = f.objectsInitialized;
   return *this;
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize = false)
//------------------------------------------------------------------------------
bool UserDefinedFunction::Initialize(ObjectInitializer *objInit,
                                     bool reinitialize,
                                     bool resetLocalObjects)
{
   validator = Validator::Instance();
   ObjectManagedFunction::Initialize(objInit, reinitialize, resetLocalObjects);
   return true;
}


//------------------------------------------------------------------------------
// bool UserDefinedFunction::Execute(ObjectInitializer *objInit)  [default implementation]
//------------------------------------------------------------------------------
bool UserDefinedFunction::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   return true;
}


//------------------------------------------------------------------------------
// bool UserDefinedFunction::Finalize(bool cleanUp = false)  [default implementation]
//------------------------------------------------------------------------------
void UserDefinedFunction::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("\nUserDefinedFunction::Finalize() entered, fcsFinalized=%d\n", fcsFinalized);
   #endif
   
   if (sandboxObjects.size() == 0)
   {
      #ifdef DEBUG_FUNCTION_FINALIZE
      MessageInterface::ShowMessage
         ("UserDefinedFunction::Finalize() leaving, there is nothing to do since snadboxObjects are empty\n\n");
      #endif
      return;
   }
   
   if (globalObjectStore == NULL)
   {
      #ifdef DEBUG_FUNCTION_FINALIZE
      MessageInterface::ShowMessage
         ("UserDefinedFunction::Finalize() leaving, there is nothing to do since globalObjectStore is NULL\n\n");
      #endif
      return;
   }
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   ShowObjects("In UserDefinedFunction::Finalize()");
   #endif
   
   // Set ref Parameter back to original ref Parameter for global ReportFile and XyPlot
   // since these subscribers uses Parameters for reporting and plotting
   GmatBase *sandboxObj = NULL;
   GmatBase *globalObj = NULL;
   GmatBase *autoObj = NULL;
   std::string sandboxObjName;
   std::string globalObjName;
   std::string autoObjName;
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("Going through sandboxObjects to see if global Parameters in global "
       "Subscribers need to point back to sandbox pointer\n");
   #endif
   // Go through already managed sandbox objects
   for (UnsignedInt i = 0; i < sandboxObjects.size(); i++)
   {
      sandboxObj = sandboxObjects[i];
      #ifdef DEBUG_FUNCTION_FINALIZE
      MessageInterface::ShowMessage
         ("   sandboxObj=<%p>[%s]'%s'\n", sandboxObj,
          sandboxObj ? sandboxObj->GetTypeName().c_str() : "NULL",
          sandboxObj ? sandboxObj->GetName().c_str() : "NULL");
      #endif
      
      if (sandboxObj == NULL)
         continue;
      
      sandboxObjName = sandboxObj->GetName();
      if (sandboxObj->IsOfType(Gmat::PARAMETER))
      {
         GmatBase *paramOwner = ((Parameter*)sandboxObj)->GetOwner();
         #ifdef DEBUG_FUNCTION_FINALIZE
         MessageInterface::ShowMessage
            ("   sandboxObj is Parameter, paramOwner = <%p>[%s]'%s'\n", paramOwner,
             paramOwner ? paramOwner->GetTypeName().c_str() : "NULL",
             paramOwner ? paramOwner->GetName().c_str() : "NULL");
         #endif
         
         // Go through global ReportFile or XYPlot and reset ref Parameter
         for (ObjectMap::iterator i = globalObjectStore->begin(); i != globalObjectStore->end(); ++i)
         {
            try
            {
               globalObj = i->second;
               globalObjName = i->first;
               #ifdef DEBUG_FUNCTION_FINALIZE
               MessageInterface::ShowMessage(GmatBase::WriteObjectInfo("   ", globalObj));
               MessageInterface::ShowMessage("Checking if it is ReportFile or XYPlot\n");
               #endif
               if (globalObj && 
                   (globalObj->IsOfType("ReportFile") || globalObj->IsOfType("XYPlot")))
               {
                  #ifdef DEBUG_FUNCTION_FINALIZE
                  MessageInterface::ShowMessage
                     ("   Found global <%p>[%s]'%s', so calling globalObj->SetRefObject(%s)\n",
                      globalObj, globalObj->GetTypeName().c_str(), globalObjName.c_str(),
                      sandboxObjName.c_str());
                  #endif
                  
                  // We can use globalObj->GetRefObjectNameArray() and go through this list
                  // or ignore exception. Ignoring exception for now.
                  try
                  {
                     globalObj->SetRefObject(sandboxObj, Gmat::PARAMETER, sandboxObjName);
                  }
                  catch (BaseException &be)
                  {
                     // Ignore exception
                     #ifdef DEBUG_FUNCTION_FINALIZE
                     MessageInterface::ShowMessage("   Ignoring exception: %s\n", be.GetFullMessage().c_str());
                     #endif
                  }
               }
            }
            catch (BaseException &be)
            {
               #ifdef DEBUG_FUNCTION_FINALIZE
               MessageInterface::ShowMessage
                  ("Caught exception: %s\n", be.GetFullMessage().c_str());
               #endif
               // If cleanup mode, ignore exception
               if (!cleanUp)
                  throw;
            }
         }
      }
   }
   
   ObjectManagedFunction::Finalize();
   
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage("UserDefinedFunction::Finalize() leaving\n\n");
   #endif
}

//------------------------------------------------------------------------------
// bool IsFcsFinalized()
//------------------------------------------------------------------------------
bool UserDefinedFunction::IsFcsFinalized()
{
   return fcsFinalized;
}

//------------------------------------------------------------------------------
// bool IsFunctionControlSequenceSet()
//------------------------------------------------------------------------------
bool UserDefinedFunction::IsFunctionControlSequenceSet()
{
   if (fcs != NULL) return true;
   return false;
}

//------------------------------------------------------------------------------
// bool SetFunctionControlSequence(GmatCommand *cmd)
//------------------------------------------------------------------------------
bool UserDefinedFunction::SetFunctionControlSequence(GmatCommand *cmd)
{
   #ifdef DEBUG_FUNCTION_SET
      if (!cmd) MessageInterface::ShowMessage("Trying to set FCS on %s, but it is NULL!!!\n",
            functionName.c_str());
      else
      {
         MessageInterface::ShowMessage("Setting FCS for function %s with FCS pointer = %p\n",
               functionName.c_str(), cmd);  
         MessageInterface::ShowMessage("First command is a %s\n", (cmd->GetTypeName()).c_str());
      }
   #endif
   fcs = cmd;
   return true;
}

//------------------------------------------------------------------------------
// GmatBase* GetFunctionControlSequence()
//------------------------------------------------------------------------------
GmatCommand* UserDefinedFunction::GetFunctionControlSequence()
{
   return fcs;
}


//------------------------------------------------------------------------------
// void ClearFunctionObjects()
//------------------------------------------------------------------------------
void UserDefinedFunction::ClearFunctionObjects()
{
   #ifdef DEBUG_FUNCTION_OBJ
   MessageInterface::ShowMessage
      ("UserDefinedFunction::ClearFunctionObjects() this=<%p> '%s' entered\n   "
       "functionObjectMap.size()=%d, sandboxObjects.size()=%d, "
       "objectsToDelete.size()=%d\n", this, GetName().c_str(),
       functionObjectMap.size(), sandboxObjects.size(), objectsToDelete.size());
   ShowObjectMap(&functionObjectMap, "In ClearFunctionObjects()", "functionObjectMap");
   #endif
   
   StringArray toDelete;
   ObjectMap::iterator omi = functionObjectMap.begin();
   while (omi != functionObjectMap.end())
   {
      GmatBase *obj = omi->second;
      #ifdef DEBUG_FUNCTION_OBJ
      MessageInterface::ShowMessage
         ("   ==> Checking if <%p>[%s]'%s' can be deleted\n", obj,
          obj ? obj->GetTypeName().c_str() : "NULL", (omi->first).c_str());
      #endif
      
      // If object is not NULL and not this function, delete
      if (obj != NULL && (!obj->IsOfType("GmatFunction")))
      {
         #ifdef DEBUG_FUNCTION_OBJ
         MessageInterface::ShowMessage
            ("   isLocal = %d, isGlobal = %d, isAutomaticGlobal = %d, \n",
             obj->IsLocal(), obj->IsGlobal(), obj->IsAutomaticGlobal());
         #endif
         // @note CelestialBody is added to SolarSystem and it will be deleted when
         // when SolarSystem in use is deleted (LOJ: 2014.12.23)
         if (obj->IsLocal() && !(obj->IsOfType(Gmat::CELESTIAL_BODY)))
         {
            #ifdef DEBUG_MEMORY
            GmatBase *obj = omi->second;
            MemoryTracker::Instance()->Remove
               (obj, obj->GetName(), "Function::ClearFunctionObjects()",
                "deleting functionObj");
            #endif
            #ifdef DEBUG_FUNCTION_OBJ
            MessageInterface::ShowMessage("   Deleting since it is a local object\n");
            #endif
            delete omi->second;
            omi->second = NULL;
            //functionObjectMap.erase(omi);
            //++omi;
         }
      }
      ++omi;
   }
   
   #ifdef DEBUG_FUNCTION_OBJ
   MessageInterface::ShowMessage
      ("UserDefinedFunction::ClearFunctionObjects() this=<%p> '%s' leaving\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// void AddFunctionObject(GmatBase *obj)
//------------------------------------------------------------------------------
void UserDefinedFunction::AddFunctionObject(GmatBase *obj)
{
   #ifdef DEBUG_FUNCTION_OBJ
   MessageInterface::ShowMessage
      ("UserDefinedFunction::AddFunctionObject() entered, obj=<%p>[%s]'%s'\n", obj,
       obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL");
   //ShowObjects("Entered UserDefinedFunction::AddFunctionObject()");
   #endif
   
   if (obj && obj->GetName() != "")
   {
      std::string objName = obj->GetName();
      if (functionObjectMap.find(objName) == functionObjectMap.end())
         functionObjectMap.insert(std::make_pair(objName, obj));
   }
   #ifdef DEBUG_FUNCTION_OBJ
   MessageInterface::ShowMessage("UserDefinedFunction::AddFunctionObject() leaving\n");
   ShowObjectMap(&functionObjectMap, "Leaving AddFunctionObject()", "functionObjectMap");
   #endif
}


//------------------------------------------------------------------------------
// GmatBase* FindFunctionObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* UserDefinedFunction::FindFunctionObject(const std::string &name)
{
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("UserDefinedFunction::FindFunctionObject() entered, name='%s'\n", name.c_str());
   //ShowObjects("In Function::FindFunctionObject()");
   #endif
   
   // Ignore array index
   std::string newName = name;
   std::string::size_type index = name.find_first_of("([");
   if (index != name.npos)
      newName = name.substr(0, index);
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage("   newName='%s'\n", newName.c_str());
   #endif
   
   GmatBase *obj = NULL;
   
   if (functionObjectMap.find(newName) != functionObjectMap.end())
      obj = functionObjectMap[newName];
   
   // try SolarSystem if obj is still NULL
   if (obj == NULL && solarSys != NULL)
      obj = (GmatBase*)(solarSys->GetBody(newName));
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("UserDefinedFunction::FindFunctionObject() returning <%p>\n", obj);
   #endif
   
   return obj;
}

//------------------------------------------------------------------------------
// ObjectMap* GetFunctionObjectMap()
//------------------------------------------------------------------------------
ObjectMap* UserDefinedFunction::GetFunctionObjectMap()
{
   return &functionObjectMap;
}

//------------------------------------------------------------------------------
// void ClearAutomaticObjects()
//------------------------------------------------------------------------------
void UserDefinedFunction::ClearAutomaticObjects()
{
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("UserDefinedFunction::ClearAutomaticObjects() this=<%p> '%s' entered\n   "
       "automaticObjectMap.size()=%d, sandboxObjects.size()=%d, "
       "objectsToDelete.size()=%d\n", this, GetName().c_str(),
       automaticObjectMap.size(), sandboxObjects.size(), objectsToDelete.size());
   #endif
   
   StringArray toDelete;
   ObjectMap::iterator omi;
   for (omi = automaticObjectMap.begin(); omi != automaticObjectMap.end(); ++omi)
   {
      #ifdef DEBUG_AUTO_OBJ
      MessageInterface::ShowMessage
         ("Checking if <%p> '%s' can be deleted\n", omi->second,
          (omi->first).c_str());
      #endif
      if (omi->second != NULL)
      {
         // If same object found in sandboxObjects then skip
         if (!sandboxObjects.empty() &&
             find(sandboxObjects.begin(), sandboxObjects.end(), omi->second) !=
             sandboxObjects.end())
            continue;

         std::string objName = (omi->second)->GetName();
         
         // If same object found in functionObjectMap then skip
         if (functionObjectMap.find(objName) != functionObjectMap.end())
         {
            ObjectMap::iterator iobjfound = functionObjectMap.find(objName);
            if (omi->second == iobjfound->second)
               continue;
         }
         
         //------------------------------------------------------
         #ifdef __COLLECT_AUTO_OBJECTS__
         //------------------------------------------------------
         
         // if object is not in objectsToDelete then add
         if (find(objectsToDelete.begin(), objectsToDelete.end(), omi->second) ==
             objectsToDelete.end())
         {
            #ifdef DEBUG_AUTO_OBJ
            MessageInterface::ShowMessage("   Added to objectsToDelete\n");
            #endif
            objectsToDelete.push_back(omi->second);
         }
         else
         {
            #ifdef DEBUG_AUTO_OBJ
            MessageInterface::ShowMessage("   Already in the objectsToDelete\n");
            #endif
         }
         
         //------------------------------------------------------
         #else
         //------------------------------------------------------
         
         // if object not found in sandboxObjects then delete
         if (find(sandboxObjects.begin(), sandboxObjects.end(), omi->second) ==
             sandboxObjects.end())
         {
            #ifdef DEBUG_MEMORY
            GmatBase *obj = omi->second;
            MemoryTracker::Instance()->Remove
               (obj, obj->GetName(), "UserDefinedFunction::ClearAutomaticObjects()",
                "deleting autoObj");
            #endif
            delete omi->second;
            omi->second = NULL;
         }
         //------------------------------------------------------
         #endif //__COLLECT_AUTO_OBJECTS__
         //------------------------------------------------------
      }
      else
      {
         #ifdef DEBUG_AUTO_OBJ
         MessageInterface::ShowMessage("   Skipped since sandbox object\n");
         #endif
      }
      toDelete.push_back(omi->first); 
   }
   
   for (unsigned int kk = 0; kk < toDelete.size(); kk++)
      automaticObjectMap.erase(toDelete.at(kk));
   
   #ifdef __COLLECT_AUTO_OBJECTS__
   // delete old automatic objects collected if not already deleted
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("   There are %d automatic objects to delete\n", objectsToDelete.size());
   #endif
   
   ObjectArray::iterator oai;
   for (oai = objectsToDelete.begin(); oai != objectsToDelete.end(); ++oai)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         ((*oai), (*oai)->GetName(), "UserDefinedFunction::ClearAutomaticObjects()", "deleting old object");
      #endif
      delete (*oai);
      (*oai) = NULL;
   }
   objectsToDelete.clear();
   #endif //__COLLECT_AUTO_OBJECTS__
   
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("UserDefinedFunction::ClearAutomaticObjects() this=<%p> '%s' leaving\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// void AddAutomaticObject(const std::string &withName, GmatBase *obj,
//                         bool alreadyManaged)
//------------------------------------------------------------------------------
/*
 * Adds automatic objects such as Parameter, e.g. sat.X, to automatic object map.
 * The GmatFunction::Initialize() calls Validator to validate and to create
 * ElementWrappers for commands in the FCS. The Validator creates automatic
 * Parameters if necessary and calls this method for adding. Since function can be
 * initialized more than one time multiple automatic Parameters can be created
 * which is the case for nested and recursive call.
 *
 * @param  withName  Automatic object name
 * @param  obj  Automatic object pointer
 * @param  alreadyManaged  true if object is Sandbox object
 *
 * @note Make sure that old automatic objects are deleted properly
 */
//------------------------------------------------------------------------------
void UserDefinedFunction::AddAutomaticObject(const std::string &withName, GmatBase *obj,
                                  bool alreadyManaged)
{
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("\nUserDefinedFunction::AddAutomaticObject() <%p>'%s' entered\n   name='%s', obj=<%p>[%s]'%s', "
       "alreadyManaged=%d, objectStore=<%p>\n", this, GetName().c_str(),
       withName.c_str(), obj, obj->GetTypeName().c_str(), obj->GetName().c_str(),
       alreadyManaged, objectStore);
   #endif
   
   // Make sure that the owner of automatic Parameter exist in the objectStore
   // (LOJ: 2009.03.25)
   if (objectStore != NULL)
   {
      std::string type, ownerName, dep;
      GmatStringUtil::ParseParameter(withName, type, ownerName, dep);
      GmatBase *owner = FindObject(ownerName);
      #ifdef DEBUG_AUTO_OBJ
      MessageInterface::ShowMessage
         ("   ownerName='%s', owner=<%p>[%s]'%s'\n", ownerName.c_str(), owner,
          owner ? owner->GetTypeName().c_str() : "NULL",
          owner ? owner->GetName().c_str() : "NULL");
      #ifdef DEBUG_OBJECT_MAP
      ShowObjectMap(objectStore, "In UserDefinedFunction::AddAutomaticObject", "objectStore");
      #endif
      #endif
   }
   
   // If same name exist in the sandbox, add it to sandboxObjects so that
   // it can be handled appropriately during finalization
   if (alreadyManaged)
   {
      if (find(sandboxObjects.begin(), sandboxObjects.end(), obj) ==
          sandboxObjects.end() &&
          automaticObjectMap.find(withName) == automaticObjectMap.end())         
      {
         #ifdef DEBUG_AUTO_OBJ
         MessageInterface::ShowMessage
            ("   ==> Adding <%p>'%s' to sandboxObjects\n", obj, obj->GetName().c_str());
         #endif
         sandboxObjects.push_back(obj);
      }
   }
   
   #ifdef __COLLECT_AUTO_OBJECTS__
   if (automaticObjectMap.find(withName) != automaticObjectMap.end())
   {
      GmatBase *oldObj = automaticObjectMap[withName];
      #ifdef DEBUG_AUTO_OBJ
      MessageInterface::ShowMessage
         ("   Found oldObj=<%p>[%s] '%s'\n", oldObj, oldObj ?
          oldObj->GetTypeName().c_str() : "NULL",
          oldObj ? oldObj->GetName().c_str() : "NULL");
      #endif
      
      // if old object is not in the sandboxObjects add it to delete list,
      // since objects in the Sandbox objectMap will be deleted from the Sandbox first.
      // (LOJ: 2009.03.16)
      if (oldObj != NULL &&
          (find(sandboxObjects.begin(), sandboxObjects.end(), oldObj) ==
           sandboxObjects.end()))
      {
         if (find(objectsToDelete.begin(), objectsToDelete.end(), oldObj) ==
             objectsToDelete.end())
         {
            #ifdef DEBUG_AUTO_OBJ
            MessageInterface::ShowMessage("   Adding to objectsToDelete\n");
            #endif
            objectsToDelete.push_back(oldObj);
         }
      }
   }
   #endif
   
   // Insert object to automaticObjectMap if not already inserted
   if (automaticObjectMap.find(withName) != automaticObjectMap.end())
   {
      #ifdef DEBUG_AUTO_OBJ
      MessageInterface::ShowMessage
         ("   The object <%p>'%s' is already in automaticObjectMap\n", obj, withName.c_str());
      #endif
   }
   else
   {
      #ifdef DEBUG_AUTO_OBJ
      MessageInterface::ShowMessage
         ("   ==> Inserting <%p>'%s' to automaticObjectMap\n", obj, withName.c_str());
      #endif
      
      // Objects in automaticObjectMap are cloned and add to function objectStore
      // in GmatFunction::Initialize()
      automaticObjectMap.insert(std::make_pair(withName,obj));
   }
   
   #ifdef DEBUG_AUTO_OBJ
   ShowObjectMap(&automaticObjectMap, "In UserDefinedFunction::AddAutomaticObject()", "automaticObjectMap");
   MessageInterface::ShowMessage
      ("UserDefinedFunction::AddAutomaticObject() <%p>'%s' leaving\n\n", this, GetName().c_str());
   #endif
}

//------------------------------------------------------------------------------
// GmatBase* FindAutomaticObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* UserDefinedFunction::FindAutomaticObject(const std::string &name)
{
   if (automaticObjectMap.find(name) != automaticObjectMap.end())
      return automaticObjectMap[name];
   else
      return NULL;
}


//------------------------------------------------------------------------------
// ObjectMap* GetAutomaticObjectMap()
//------------------------------------------------------------------------------
ObjectMap* UserDefinedFunction::GetAutomaticObjectMap()
{
   return &automaticObjectMap;
}


//---------------------------------
// protected
//---------------------------------
//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* UserDefinedFunction::FindObject(const std::string &name)
{
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("UserDefinedFunction::FindObject() <%p>'%s' entered, name = '%s'\n", this,
       functionName.c_str(), name.c_str());
   ShowObjectMap(objectStore, "In FindObject()", "objectStore");
   ShowObjectMap(globalObjectStore, "In FindObject()", "globalObjectStore");
   ShowObjectMap(&functionObjectMap, "In FindObject()", "functionObjectMap");
   #endif
   
   std::string newName = name;
   GmatBase *obj = NULL;
   
   // Ignore array indexing of Array
   std::string::size_type index = name.find('(');
   if (index != name.npos)
      newName = name.substr(0, index);

   // Check for the object in the Local Object Store (LOS) first
   if (objectStore && objectStore->find(newName) != objectStore->end())
      obj = (*objectStore)[newName];
   
   // If not found in the LOS, check the Global Object Store (GOS)
   else if (globalObjectStore && globalObjectStore->find(newName) != globalObjectStore->end())
      obj = (*globalObjectStore)[newName];
   
   // Let's try SolarSystem (loj: 2008.06.12)
   else if (solarSys && solarSys->GetBody(newName))
      obj = (GmatBase*)(solarSys->GetBody(newName));
   
   // If still not found, try functionObjectMap
   // Should this moved to top?
   else if (functionObjectMap.find(newName) != functionObjectMap.end())
      obj = functionObjectMap[newName];
   
   #ifdef DEBUG_FIND_OBJECT
   MessageInterface::ShowMessage
      ("UserDefinedFunction::FindObject() <%p>'%s' returning <%p> for '%s'\n", this,
       functionName.c_str(), obj, name.c_str());
   #endif
   return obj;
}


//------------------------------------------------------------------------------
//bool IsAutomaticObjectGlobal(const std::string &autoObjName, GmatBase *owner)
//------------------------------------------------------------------------------
/**
 * Checks if the owner of automatic object such as Parameter is global object.
 *
 * @param autoObjName  Input object name
 * @param *owner        Owner of the automatic object if owner is found
 */
//------------------------------------------------------------------------------
bool UserDefinedFunction::IsAutomaticObjectGlobal(const std::string &autoObjName,
                                       GmatBase **owner)
{
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("UserDefinedFunction::IsAutomaticObjectGlobal() entered, autoObjName='%s'\n",
       autoObjName.c_str());
   #endif
   
   *owner = NULL;
   std::string type, ownerName, dep;
   GmatStringUtil::ParseParameter(autoObjName, type, ownerName, dep);
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage("   ownerName='%s'\n", ownerName.c_str());
   #endif
   
   bool isParamOwnerGlobal = false;

   // First check if we are using a local version, then check global
   if (objectStore->find(ownerName) != objectStore->end())
      *owner = (objectStore->find(ownerName))->second;
   else if (globalObjectStore->find(ownerName) != globalObjectStore->end())
   {
      *owner = (globalObjectStore->find(ownerName))->second;
      isParamOwnerGlobal = true;
   }
   
   #ifdef DEBUG_AUTO_OBJ
   MessageInterface::ShowMessage
      ("UserDefinedFunction::IsAutomaticObjectGlobal() leaving, autoObjName='%s' is %s, ",
       autoObjName.c_str(), isParamOwnerGlobal ? "global" : "not global");
   MessageInterface::ShowMessage(GmatBase::WriteObjectInfo("owner=", (*owner)));
   #endif
   
   return isParamOwnerGlobal;
}

//------------------------------------------------------------------------------
// void ShowObjects(const std::string &title)
//------------------------------------------------------------------------------
void UserDefinedFunction::ShowObjects(const std::string &title)
{
   MessageInterface::ShowMessage("%s\n", title.c_str());
   MessageInterface::ShowMessage("this=<%p>, functionName='%s'\n", this, functionName.c_str());
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("solarSys          = <%p>\n", solarSys);
   MessageInterface::ShowMessage("internalCoordSys  = <%p>\n", internalCoordSys);
   MessageInterface::ShowMessage("forces            = <%p>\n", forces);
   MessageInterface::ShowMessage("sandboxObjects.size() = %d\n", sandboxObjects.size());
   for (UnsignedInt i = 0; i < sandboxObjects.size(); i++)
   {
      GmatBase *obj = sandboxObjects[i];
      MessageInterface::ShowMessage
         ("   <%p>[%s]'%s'", obj, obj ? obj->GetTypeName().c_str() : "NULL",
          obj ? obj->GetName().c_str() : "NULL");
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         GmatBase *paramOwner = ((Parameter*)obj)->GetOwner();
         MessageInterface::ShowMessage
            ("   paramOwner = <%p>[%s]'%s'\n", paramOwner,
             paramOwner ? paramOwner->GetTypeName().c_str() : "NULL",
             paramOwner ? paramOwner->GetName().c_str() : "NULL");
      }
      else
      {
         MessageInterface::ShowMessage("\n");
      }
   }
   ShowObjectMap(&functionObjectMap, "ShowObjects()", "functionObjectMap");
   ShowObjectMap(&automaticObjectMap, "ShowObjects()", "automaticObjectMap");
   ShowObjectMap(objectStore, "ShowObjects()", "objectStore");
   ShowObjectMap(globalObjectStore, "ShowObjects()", "globalObjectStore");
   MessageInterface::ShowMessage("========================================\n");
}

