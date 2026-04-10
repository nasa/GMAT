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
//
/**
 * Definition for library code loaded at run time.
 */
//------------------------------------------------------------------------------


#ifndef DynamicLibrary_hpp
#define DynamicLibrary_hpp


#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "TriggerManager.hpp"


class GuiFactory;

/**
 * The DynamicLibrary class defines the interfaces that are needed to build a
 * GMAT plug-in library.  GMAT plugins usually include one or more classes
 * derived from GmatBase, one or more factories that create instances of these
 * classes, and three C-style functions that are accessed to import the Factory
 * into GMAT.  The functions required in the plugin library are defined as
 * follows:
 *
 *    Integer     GetFactoryCount();
 *    Factory*    GetFactoryPointer(Integer index);
 *    void        SetMessageReceiver(MessageReceiver* mr);
 *
 * The use of the first two functions matches the calls defined for this class.
 * The MessageReceiver is set using the SetMessageReceiver function when the
 * library is loaded into memory.
 *
 * Plugin libraries may also optionally create and use TriggerManagers.  A
 * TriggerManager is an engine-level component that exists at the scope of the
 * Sandbox.  Each unique TriggerManager is a singleton in the Sandbox, and is
 * used to trigger specific Mission Control Sequence actions during a run.  The
 * prototypical TriggerManager option is event location for shadow entry and
 * exit epoch calculations during a run.
 *
 * Plugin libraries that include TriggerManagers need to implement the following
 * two functions:
 *
 *    Integer           GetTriggerManagerCount();
 *    TriggerManager*   GetTriggerManager(Integer index);
 *
 * If these functions are not implemented, the library will load but no
 * TriggerManager will be loaded.  In other words, if your code does not need a
 * TriggerManager, there is no need to implement these functions.
 *
 * Plugin libraries that include resources that should appear on a ResourceTree
 * should implement the following function:
 *
 *    Integer               GetMenuEntryCount();
 *    Gmat::PluginResource* GetMenuEntry(Integer index);
 *
 * The parameter, index, for the 2nd function is the index of the new entry that
 * should be placed to the tree.  Factories that supply resources that already
 * have tree nodes should not implement this method; it is used to register
 * new types of objects with the resource tree.
 *
 * The PluginResource returned from this function provides four pieces of
 * information used to manage objects in the user interface:
 *
 *      std::string nodeName;         // Identifier for the resource
 *      std::string parentNodeName;   // Optional owning type identifier
 *      UnsignedInt type;             // Core type
 *      std::string subtype;          // Optional subtype off of the core
 *
 * The nodeName and type enumeration are required parameters.  If the new tree
 * node should appear at the resource level in the tree, the parentNodeName
 * should be set to the empty string.  If the new node should appear as a
 * subnode of an existing node, the name of the existing node should be set in
 * the parentNodeName string.  If the new object type is derived from an
 * extendible object type, the subtype name is passed in in the subtype field.
 * For example, GMAT has two subtypes of built in Solvers as of this writing:
 * Boundary Value Solvers and Optimizers.  If a new type of solver is added
 * through a plugin, the scripted subtype parameter is set in the subtype
 * field.  So, for example, a simulator subtype would set this field to the
 * string "Simulator".  The subtype field should be set to the empty string if
 * it is not needed.
 *
 * If these functions are not implemented, the library will load but no
 * ResourceTree updates will be loaded.  In other words, if your code does not
 * need any new ResourceTree items, there is no need to implement these
 * functions.
 *
 * Finally, plugins that implement GUI controls (panels, dialog boxes, etc)
 * should implement the following functions:
 *
 *    std::string GetGuiToolkitName();
 *    Integer GetGuiFactoryCount();
 *    GuiFactory* GetGuiFactory();
 *
 * The first method here, GetGuiToolkitName, returns a string identifying the
 * toolkit used for the GUI components.  Normal usage in GMAT would be to return
 * "wxWidgets" from this call.  Some third party groups may want to use this
 * structure to support other toolkits.  The design here is intended to allow
 * such extensions.
 */
class GMAT_API DynamicLibrary
{
public:
   DynamicLibrary(const std::string &name, const std::string &path = "./");
   virtual ~DynamicLibrary();
   DynamicLibrary(const DynamicLibrary& dlib);
   DynamicLibrary& operator=(const DynamicLibrary& dlib);

   bool                    LoadDynamicLibrary();
   void                    (*GetFunction(const std::string &funName))();

   Integer                 GetFactoryCount();
   Factory*                GetGmatFactory(Integer index = 0);
   Integer                 GetTriggerManagerCount();
   TriggerManager*         GetTriggerManager(Integer index = 0);

   // GUI elements
   Integer                 GetMenuEntryCount();
   Gmat::PluginResource*   GetMenuEntry(Integer index);
   std::string             GetGuiToolkitName();
   Integer                 GetGuiFactoryCount();
   GuiFactory*             GetGuiFactory(const Integer whichOne);

protected:
   std::string             libName;
   std::string             libPath;
   void *                  libHandle;
};

#endif /*DynamicLibrary_hpp*/
