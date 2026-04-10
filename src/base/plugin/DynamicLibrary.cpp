//$Id$
//------------------------------------------------------------------------------
//                              DynamicLibrary
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
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/04/18
// Updated: 2014/12/23 Ravi Mathur, Emergent Space Technologies, Inc.
//                     Updated to properly use rpaths across systems
//
/**
 * Implementation for library code loaded at run time.
 */
//------------------------------------------------------------------------------


#include "DynamicLibrary.hpp"

#include "GmatBaseException.hpp"
#include "MessageInterface.hpp"
#include "MessageReceiver.hpp"
#include "StringUtil.hpp"          // for StringToWideString()

// Platform specific library loader routines
#ifdef _WIN32
#define __WIN32__
#endif

#ifdef __WIN32__
#include <windows.h>
#else
#include <dlfcn.h>

/// @todo Put in the Mac/Linux switching logic for dynamic library extensions
#ifdef __linux
// Linux version:
#define UNIX_EXTENSION ".so"
#else
// Mac Version:
#define UNIX_EXTENSION ".dylib"

#endif

#endif

//------------------------------------------------------------------------------
// DynamicLibrary(const std::string &name, const std::string &path)
//------------------------------------------------------------------------------
/**
 * Default constructor.  Dynamic libraries MUST specify the library name.
 *
 * @param name File name for the dynamic library.
 * @param path File path for the library.  If unspecified, ./ is used.
 */
//------------------------------------------------------------------------------
DynamicLibrary::DynamicLibrary(const std::string &name,
      const std::string &path) :
   libName          (name),
   libPath          (path),
   libHandle        (NULL)
{
}


//------------------------------------------------------------------------------
// ~DynamicLibrary()
//------------------------------------------------------------------------------
/**
 * Destructor.  Closes the library if it was opened.
 */
//------------------------------------------------------------------------------
DynamicLibrary::~DynamicLibrary()
{
   if (libHandle != NULL)
      #ifdef __WIN32__
         FreeLibrary((HINSTANCE)libHandle);
      #else
         dlclose(libHandle);
      #endif
}


//------------------------------------------------------------------------------
// DynamicLibrary(const DynamicLibrary& dlib)
//------------------------------------------------------------------------------
/**
 * Copy constructor.  Sets the name and path, but does not open the library.
 *
 * @param dlib The instance that is copied.
 */
//------------------------------------------------------------------------------
DynamicLibrary::DynamicLibrary(const DynamicLibrary& dlib):
   libName          (dlib.libName),
   libPath          (dlib.libPath),
   libHandle        (NULL)
{
}


//------------------------------------------------------------------------------
// DynamicLibrary(const DynamicLibrary& dlib)
//------------------------------------------------------------------------------
/**
 * Assignmant operator.  Sets the name and path, but does not open the library.
 *
 * @param dlib The instance that is copied to this instance.
 */
//------------------------------------------------------------------------------
DynamicLibrary& DynamicLibrary::operator=(const DynamicLibrary& dlib)
{
   if (&dlib != this)
   {
      libName = dlib.libName;
      libPath = dlib.libPath;
      libHandle = NULL;
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool LoadDynamicLibrary()
//------------------------------------------------------------------------------
/**
 * Loads the library into memory, and sets the MessageReceiver if necessary.
 *
 * @return true if the library was loaded.
 *
 * @note
 * The library is defined by its filename (libName) and pathname (libPath).
 * Search order is OS-dependent, as described below. In Mac/Linux, rpath is
 * defined in the GMAT top-level CMakeLists.txt file.
 *
 * Linux: (see dlopen manpage, modified here so pathname comes last)
 * - $LD_LIBRARY_PATH/<libName>.so
 * - runpath/<libName>.so (runpath = rpath, does NOT use libPath)
 * - <libPath><libName>.so (relative to cwd if libPath is relative path)
 *
 * Mac: (see dlopen manpage)
 * - $DYLD_LIBRARY_PATH/<libName>.dylib
 * - rpath/<libPath><libName>.dylib
 * - <libPath><libName>.dylib (relative to cwd if libPath is relative path)
 *
 * Windows: (see http://msdn.microsoft.com/en-us/library/ms682586.aspx)
 * - <libPath><libName>.dll (relative to cwd if libPath is relative path)
 * - %Path%\<libPath><libName>.dll (System %Path%)
 * - %Path%\<libPath><libName>.dll (User %Path%)
 */
//------------------------------------------------------------------------------
bool DynamicLibrary::LoadDynamicLibrary()
{
   std::string nameWithPath = libPath + libName;

   #ifdef __WIN32__
      #ifdef _UNICODE
         std::wstring nameWithPathW = GmatStringUtil::StringToWideString(nameWithPath);
         libHandle = LoadLibrary(nameWithPathW.c_str());
      #else
         libHandle = LoadLibrary(nameWithPath.c_str());
      #endif
      
      #ifdef DEBUG_LIBRARY_LOAD
         if (libHandle == NULL)
         {
            Integer retcode = GetLastError();
            MessageInterface::ShowMessage("Library load call returned %d\n", retcode);
            return false;
         }
      #endif
   #else
      // Linux dlopen() treats input paths as standalone, and ignores any
      // embedded rpaths or system LD_LIBRARY_PATH settings. So to increase
      // compatibility, first try opening with just the plugin filename so
      // that dlopen() will search rpaths and LD_LIBRARY_PATH.
      // See dlopen manpage for more info.
      #ifdef __linux__
         std::string libFullName = libName + UNIX_EXTENSION;
         libHandle = dlopen(libFullName.c_str(), RTLD_LAZY);
      #endif

      // On Linux this will only be true if the library couldn't be found
      // using its filename alone from above.
      // On Mac, dlopen() appends input paths to DYLD_LIBRARY_PATH or rpath,
      // so no special treatment is needed. Se dlopen manpage for more info.
      if(libHandle == NULL)
      {
         nameWithPath += UNIX_EXTENSION;
         libHandle = dlopen(nameWithPath.c_str(), RTLD_LAZY);
      }

      if (libHandle == NULL)
         MessageInterface::ShowMessage("\n%s\n", dlerror());
   #endif

   // Set the MessageReceiver if the Library needs it
   try
   {
      void (*SetMR)(MessageReceiver*) = NULL;

      SetMR = (void (*)(MessageReceiver*))GetFunction("SetMessageReceiver");
      MessageReceiver* mr = MessageInterface::GetMessageReceiver();
      SetMR(mr);
   }
   catch (GmatBaseException&)
   {
      // Just ignore these exceptions -- no message receiver interfaces
   }

   if (libHandle == NULL)
      return false;

   return true;
}


//------------------------------------------------------------------------------
// void (*GetFunction(const std::string &funName))()
//------------------------------------------------------------------------------
/**
 * Retrieves the function pointer for a specified function.
 *
 * @param funName The name of the function.
 *
 * @return The function pointer, or NULL if the function was not located.
 *         A NULL value should never be found, because the method throws if
 *         the function is not found.
 */
//------------------------------------------------------------------------------
void (*DynamicLibrary::GetFunction(const std::string &funName))()
{
   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName +
            " has not been opened successfully; cannot search for function \"" +
            funName + "\"\n");

   void (*func)() = NULL;

   #ifdef __WIN32__
      func = (void(*)())GetProcAddress((HINSTANCE)libHandle, funName.c_str());
   #else
      func = (void(*)())dlsym(libHandle, funName.c_str());
   #endif

   if (func == NULL)
      throw GmatBaseException("Library " + libName +
            " cannot locate the function \"" +
            funName + "\"\n");

   return func;
}


//------------------------------------------------------------------------------
// Integer GetFactoryCount()
//------------------------------------------------------------------------------
/**
 * Retrieves the number of factories in the plugin.
 *
 * @return The number of factories.
 */
//------------------------------------------------------------------------------
Integer DynamicLibrary::GetFactoryCount()
{
   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName + " has not been opened "
            "successfully; cannot search for factories \n");

   Integer (*FactoryCount)() = NULL;

   try
   {
      FactoryCount = (Integer(*)())GetFunction("GetFactoryCount");
   }
   catch (GmatBaseException&)
   {
      return 0;
   }

   return FactoryCount();
}


//------------------------------------------------------------------------------
// Factory* GetGmatFactory(Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a Factory pointer from the plugin.
 *
 * @param index Zero based index into the list of factories
 *
 * @return The pointer to the factory at the specified index.
 */
//------------------------------------------------------------------------------
Factory* DynamicLibrary::GetGmatFactory(Integer index)
{
   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName + " has not been opened "
            "successfully; cannot search for factories\n");

   Factory* (*GetFactory)(Integer) = NULL;
   GetFactory = (Factory*(*)(Integer))GetFunction("GetFactoryPointer");

   Factory *theFactory = GetFactory(index);
   if (theFactory == NULL)
      MessageInterface::ShowMessage(
            "Cannot access factory #%d in the \"%s\" library\n", index,
            libName.c_str());

   return theFactory;
}


//------------------------------------------------------------------------------
// Integer GetTriggerManagerCount()
//------------------------------------------------------------------------------
/**
 * Retrieves the number of TriggerManagers in the plugin.
 *
 * @return The number of TriggerManagers.
 */
//------------------------------------------------------------------------------
Integer DynamicLibrary::GetTriggerManagerCount()
{
   Integer triggerCount = 0;

   // Test to see if there is a TriggerManager count function
   try
   {
      Integer (*tmCount)() = NULL;
      tmCount = (Integer (*)())GetFunction("GetTriggerManagerCount");
      if (tmCount != NULL)  // There may be TriggerManagers
         triggerCount = tmCount();
   }
   catch (GmatBaseException &)
   {
      // Ignored -- Just indicates that there are no TriggerManager interfaces
   }

   return triggerCount;
}


//------------------------------------------------------------------------------
// TriggerManager* GetTriggerManager(Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a TriggerManager pointer from the plugin.
 *
 * @param index Zero based index into the list of TriggerManagers
 *
 * @return The pointer to the TriggerManager at the specified index.
 */
//------------------------------------------------------------------------------
TriggerManager* DynamicLibrary::GetTriggerManager(Integer index)
{
   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName + " has not been opened "
            "successfully; cannot search for TriggerManagers\n");

   TriggerManager* (*GetTM)(Integer) = NULL;
   GetTM = (TriggerManager* (*)(Integer))GetFunction("GetTriggerManager");

   TriggerManager *theTM = GetTM(index);
   if (theTM == NULL)
      MessageInterface::ShowMessage(
            "Cannot access TriggerManager #%d in the \"%s\" library\n", index,
            libName.c_str());

   return theTM;
}


//------------------------------------------------------------------------------
// Integer GetMenuEntryCount()
//------------------------------------------------------------------------------
/**
 * Retrieves the number of new resource tree menu entries in the plugin.
 *
 * @return The number of new entries.
 */
//------------------------------------------------------------------------------
Integer DynamicLibrary::GetMenuEntryCount()
{
   Integer menuEntryCount = 0;

   // Test to see if there is a menu entry count function
   try
   {
      Integer (*meCount)() = NULL;
      meCount = (Integer (*)())GetFunction("GetMenuEntryCount");
      if (meCount != NULL)  // There may be TriggerManagers
         menuEntryCount = meCount();
   }
   catch (GmatBaseException &)
   {
      // Ignored -- Just indicates that there are no menu entry interfaces
   }

   return menuEntryCount;
}


//------------------------------------------------------------------------------
// Gmat::PluginResource *GetMenuEntry(Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a PluginResource structure pointer from the plugin containing the
 * data needed to update the GUI for a new resource type.
 *
 * @param index Zero based index into the list of PluginResources
 *
 * @return The pointer to the PluginResource structure that the Moderator needs.
 */
//------------------------------------------------------------------------------
Gmat::PluginResource *DynamicLibrary::GetMenuEntry(Integer index)
{
   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName + " has not been opened "
            "successfully; cannot search for GUI menu entries\n");

   // Test to see if there is a TriggerManager count function
   try
   {
      Gmat::PluginResource* (*GetMenuEntry)(Integer) = NULL;
      GetMenuEntry = (Gmat::PluginResource* (*)(Integer))
            GetFunction("GetMenuEntry");

      Gmat::PluginResource *res = GetMenuEntry(index);
      if (res == NULL)
         MessageInterface::ShowMessage(
               "Cannot access PluginResource #%d in the \"%s\" library\n",
               index, libName.c_str());

      return res;
   }
   catch (GmatBaseException &)
   {
      // Ignored -- Just indicates that there are no menu entries
   }

   return NULL;
}

//------------------------------------------------------------------------------
// std::string GetGuiToolkitName()
//------------------------------------------------------------------------------
/**
 * Method used to identify the toolkit needed for GUI plugin elements
 *
 * @return The name of the required toolkit (wxWidgets, for example).
 */
//------------------------------------------------------------------------------
std::string DynamicLibrary::GetGuiToolkitName()
{
   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName + " has not been opened "
            "successfully; cannot check the supported GUI toolkit\n");

   std::string retval = "";

   // Test to see if there is a TriggerManager count function
   try
   {
      std::string (*ToolkitName)() = NULL;

      ToolkitName = (std::string(*)())GetFunction("GetGuiToolkitName");
      retval = ToolkitName();
   }
   catch (GmatBaseException &)
   {
      // Ignored -- Just indicates that there are no menu entries
   }

   return retval;
}

//------------------------------------------------------------------------------
// Integer GetGuiFactoryCount()
//------------------------------------------------------------------------------
/**
 * Returns the number of GUI factory components supplied by a plugin
 *
 * @return The factory count
 */
//------------------------------------------------------------------------------
Integer DynamicLibrary::GetGuiFactoryCount()
{
   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName + " has not been opened "
            "successfully; cannot check the factory count\n");

   Integer retval = 0;

   // Test to see if there is a GUI factory count function
   try
   {
      Integer (*FactoryCount)() = NULL;

      FactoryCount = (Integer(*)())GetFunction("GetGuiFactoryCount");
      retval = FactoryCount();
   }
   catch (GmatBaseException &)
   {
      // Ignored -- Just indicates that there are no GUI factories
   }

   return retval;
}

//------------------------------------------------------------------------------
// GuiFactory* GetGuiFactory(const Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Returns a GUI factory that matches the toolkit used for the GUI.
 *
 * Implementors of GMAT GUIs need to implement the toolkit specific interfaces
 * needed to support plugin widgets for the toolkit.  The GMAT base code
 * interface only supplies the top level connection needed to start the process.
 *
 * @param whichOne The index for the GUI factory
 *
 * @return The corresponding factory
 */
//------------------------------------------------------------------------------
GuiFactory* DynamicLibrary::GetGuiFactory(const Integer whichOne)
{
   GuiFactory *theFactory = NULL;

   if (libHandle == NULL)
      throw GmatBaseException("Library " + libName + " has not been opened "
            "successfully; cannot search for factories\n");

   GuiFactory* (*GetFactory)(Integer) = NULL;
   GetFactory = (GuiFactory*(*)(Integer))GetFunction("GetGuiFactory");

   theFactory = GetFactory(whichOne);
   if (theFactory == NULL)
      MessageInterface::ShowMessage(
            "Cannot access GUI factory #%d in the \"%s\" library\n", whichOne,
            libName.c_str());

   return theFactory;
}
