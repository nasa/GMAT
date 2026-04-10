//$Id$
//------------------------------------------------------------------------------
//                                 Moderator
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
// number S-67573-G
//
// Developed further jointly by NASA/GSFC, Thinking Systems, Inc., and 
// Schafer Corp., under AFRL NOVA Contract #FA945104D03990003
//
// Author: Linda Jun
// Created: 2003/08/25
// Modified:  Dunn Idle (added MRPs)
// Date:      2010/08/24
//
/**
 * Implements operations of the GMAT executive.  It is a singleton class -
 * only one instance of this class can be created.
 */
//------------------------------------------------------------------------------

#include "Moderator.hpp"

// factories
#include "AtmosphereFactory.hpp"
#include "AttitudeFactory.hpp"
#include "AxisSystemFactory.hpp"
#include "BurnFactory.hpp"
#include "CelestialBodyFactory.hpp"
#include "CommandFactory.hpp"
#include "CoordinateSystemFactory.hpp"
#include "FunctionFactory.hpp"
#include "HardwareFactory.hpp"
#include "FieldOfViewFactory.hpp"
#include "ODEModelFactory.hpp"
#include "ParameterFactory.hpp"
#include "PhysicalModelFactory.hpp"
#include "PropagatorFactory.hpp"
#include "PropSetupFactory.hpp"
#include "SolverFactory.hpp"
#include "SpacecraftFactory.hpp"
#include "PlateFactory.hpp"
#include "StopConditionFactory.hpp"
#include "SubscriberFactory.hpp"
#include "CalculatedPointFactory.hpp"
#include "MathFactory.hpp"
#include "PlanetographicRegionFactory.hpp"
//#include "SpaceMeshFactory.hpp"
#include "Interface.hpp"
#include "XyPlot.hpp"
#include "OrbitPlot.hpp"
#include "GmatDefaults.hpp"

#include "NoOp.hpp"
#include "GravityField.hpp"
#include "RelativisticCorrection.hpp"
#include "CalculatedPoint.hpp"
#include "Barycenter.hpp"
#include "TimeSystemConverter.hpp"  // for SetLeapSecsFileReader(), SetEopFile()
#include "BodyFixedAxes.hpp"        // for SetEopFile(), SetCoefficientsFile()
#include "ICRFAxes.hpp"
#include "ObjectReferencedAxes.hpp"
#include "ParameterInfo.hpp"        // for ParameterInfo
#include "MessageInterface.hpp"
#include "CommandUtil.hpp"          // for GetCommandSeq()
#include "StringTokenizer.hpp"      // for StringTokenizer
#include "StringUtil.hpp"           // for GmatStringUtil::
#include "FileUtil.hpp"             // for GmatFileUtil::
#include <sstream>                  // for stringstream
#include <algorithm>                // for sort(), set_difference()
#include <ctime>                    // for clock()
#include <errno.h>                 

#include <chrono>

#ifdef __linux__
   #include <iostream>              // to detect GUI plugins
#endif

#ifdef CREATE_OUTPUT_FOLDER
#include <sys/stat.h>               // for mkdir
#endif

//#define DEBUG_ONLY_FOR_SCRIPT
//#define DEBUG_ONLY_FOR_FUNCTION

//#define DEBUG_INITIALIZE 1
//#define DEBUG_FINALIZE 1
//#define DEBUG_INTERPRET 1
//#define DEBUG_RUN 1
//#define DEBUG_CREATE_OBJECT 1
//#define DEBUG_CREATE_COMMAND 1
//#define DEBUG_CREATE_COORDSYS 1
//#define DEBUG_CREATE_RESOURCE 2
//#define DEBUG_CREATE_DEFAULT_RESOURCE 1
//#define DEBUG_CREATE_OTHER_RESOURCE 1
//#define DEBUG_CREATE_CALC_POINT
//#define DEBUG_CREATE_PARAMETER 1
//#define DEBUG_CREATE_EPHEMFILE 1
//#define DEBUG_CREATE_SUBSCRIBER 1
//#define DEBUG_CREATE_FUNCTION 1
//#define DEBUG_CREATE_VAR 1
//#define DEBUG_PARAMETER_REF_OBJ 1
//#define DEBUG_DEFAULT_COMMAND 1
//#define DEBUG_COMMAND_APPEND 1
//#define DEBUG_COMMAND_DELETE 1
//#define DEBUG_RENAME 1
//#define DEBUG_REMOVE 1
//#define DEBUG_DEFAULT_MISSION 2
//#define DEBUG_MULTI_STOP 2
//#define DEBUG_RUN_STATE 1
//#define DEBUG_LOOKUP_RESOURCE 1
//#define DEBUG_SEQUENCE_CLEARING 1
//#define DEBUG_CONFIG 1
//#define DEBUG_CONFIG_CHANGE 1
//#define DEBUG_GMAT_FUNCTION 1
//#define DEBUG_OBJECT_MAP 1
//#define DEBUG_FIND_OBJECT 2
//#define DEBUG_ADD_OBJECT 1
//#define DEBUG_SOLAR_SYSTEM 1
//#define DEBUG_SOLAR_SYSTEM_IN_USE 1
//#define DEBUG_CREATE_BODY
//#define DEBUG_PLUGIN_REGISTRATION
//#define DEBUG_MATLAB
//#define DEBUG_CCSDS_EPHEMERIS
//#define DEBUG_CREATE_PROP_SETUP 1
//#define DEBUG_CREATE_PHYSICAL_MODEL
//#define DEBUG_LIST_CALCULATED_POINT
//#define DEBUG_SHOW_SYSTEM_EXCEPTIONS
//#define DEBUG_MAC_ENVIRONMENT
//#define REPORT_UNHANDLED_EXCEPTIONS

#ifdef DEBUG_MAC_ENVIRONMENT
#include <stdlib.h>                 // for getenv (for Mac)
#endif

// Currently we can't use DataFile for 2011a release so commented out
// Actually we want to put this flag in BuildEnv.mk but it is very close to
// release so added it here and EphemerisFile.cpp
//#define __USE_DATAFILE__

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

// @note If this is enabled, Bug1430-Func_GFuncInsideCFlow.script leaves
// no memory tracks but AssigningWholeObjects.script crashes when exiting
//#define __ENABLE_CLEAR_UNMANAGED_FUNCTIONS__

//#define __CREATE_DEFAULT_BC__
//#define __SHOW_FINAL_STATE__
//#define __DISABLE_SOLAR_SYSTEM_CLONING__

//---------------------------------
// static data
//---------------------------------
Moderator*         Moderator::instance                    = NULL;
ScriptInterpreter* Moderator::theUiInterpreter            = NULL;
ScriptInterpreter* Moderator::theScriptInterpreter        = NULL;

bool               Moderator::thrusterDeprecateMsgWritten = false;
bool               Moderator::fuelTankDeprecateMsgWritten = false;



//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Moderator* Instance()
//------------------------------------------------------------------------------
Moderator* Moderator::Instance()
{
   if (instance == NULL)
      instance = new Moderator;
    
   return instance;
}

//------------------------------------------------------------------------------
// void OverridePublisher(Publisher *otherPublisher)
//------------------------------------------------------------------------------
/**
 * Replace the (GmatBase) Publisher with a derived instance
 *
 * @param otherPublisher The derived Publisher
 */
//------------------------------------------------------------------------------
void Moderator::OverridePublisher(Publisher *otherPublisher)
{
   thePublisher = otherPublisher;
}

//------------------------------------------------------------------------------
// bool Initialize(const std::string &startupFile, bool fromGui = false,
//      const std::string &suffix, const StringArray *forEntries)
//------------------------------------------------------------------------------
bool Moderator::Initialize(const std::string &startupFile, bool fromGui,
      const std::string &suffix, const StringArray *forEntries)
{
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage
      ("Moderator::Initialize() entered, startupFile='%s', fromGui=%d\n",
       startupFile.c_str(), fromGui);
   #endif
   
   isFromGui = fromGui;
   
   try
   {
      // We don't want to write before startup file is read so commented out
      //MessageInterface::ShowMessage("Moderator is reading startup file...\n");
      
      // Set theFileManager
      theFileManager = FileManager::Instance();
      theTimeConverter = TimeSystemConverter::Instance();

      // Read startup file, Set Log file
      theFileManager->ReadStartupFile(startupFile);
      if ((suffix != "") && (forEntries != NULL))
      {
         theFileManager->AdjustSettings(suffix, *forEntries);

         #ifdef CREATE_OUTPUT_FOLDER
            // Create the output path if it does not exist
            std::string outPath = theFileManager->GetPathname("OUTPUT_PATH");
            if (theFileManager->DoesDirectoryExist(outPath, true) == false)
            {
               mkdir(outPath.c_str(), S_IRWXU | S_IRWXG);
            }
         #endif
         MessageInterface::SetLogFile(theFileManager->GetAbsPathname("LOG_FILE"));
         MessageInterface::ShowMessage("Logging to %s\n", theFileManager->GetAbsPathname("LOG_FILE").c_str());
      }

      MessageInterface::ShowMessage("Moderator is updating data files...\n");
      // update data files from repository
      UpdateDataFiles();
      
      MessageInterface::ShowMessage("Moderator is creating core engine...\n");
      
      #ifdef DEBUG_MAC_ENVIRONMENT
         // On Mac, check the environment variable for DYLD_LIBRARY_PATH
         char *dyldPath = getenv("DYLD_LIBRARY_PATH");
         MessageInterface::ShowMessage("--- on Mac, DYLD_LIBRARY_PATH = %s\n", dyldPath);
      #endif

      // Set trace flag globally
      #ifdef DEBUG_MEMORY
         //MemoryTracker::Instance()->SetShowTrace(false);
      MemoryTracker::Instance()->SetShowTrace(true);
      #endif
      
      // Create Managers
      theFactoryManager = FactoryManager::Instance();
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)theFactoryManager\n", theFactoryManager);
      #endif
      
      theConfigManager = ConfigManager::Instance();

      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)theConfigManager\n", theConfigManager);
      #endif
      
      // Register factories
      theFactoryManager->RegisterFactory(new AtmosphereFactory());
      theFactoryManager->RegisterFactory(new AttitudeFactory());
      theFactoryManager->RegisterFactory(new AxisSystemFactory());
      theFactoryManager->RegisterFactory(new BurnFactory());
      theFactoryManager->RegisterFactory(new CalculatedPointFactory());
      theFactoryManager->RegisterFactory(new CommandFactory());
      theFactoryManager->RegisterFactory(new CoordinateSystemFactory());
      theFactoryManager->RegisterFactory(new FunctionFactory());
      theFactoryManager->RegisterFactory(new HardwareFactory());
      theFactoryManager->RegisterFactory(new FieldOfViewFactory());
      theFactoryManager->RegisterFactory(new MathFactory());
      theFactoryManager->RegisterFactory(new ODEModelFactory());
      theFactoryManager->RegisterFactory(new ParameterFactory());
      theFactoryManager->RegisterFactory(new PhysicalModelFactory());
      theFactoryManager->RegisterFactory(new PropagatorFactory());
      theFactoryManager->RegisterFactory(new PropSetupFactory());
      theFactoryManager->RegisterFactory(new SolverFactory());
      theFactoryManager->RegisterFactory(new SpacecraftFactory());
		theFactoryManager->RegisterFactory(new PlateFactory());
      theFactoryManager->RegisterFactory(new StopConditionFactory());
      theFactoryManager->RegisterFactory(new SubscriberFactory());
      theFactoryManager->RegisterFactory(new CelestialBodyFactory());
      theFactoryManager->RegisterFactory(new PlanetographicRegionFactory());
//      theFactoryManager->RegisterFactory(new SpaceMeshFactory());
      
      // Create publisher if not overridden
      if (thePublisher == NULL)
         thePublisher = Publisher::Instance();
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)thePublisher...\n", thePublisher);
      #endif
      
      // Create script interpreter
      theScriptInterpreter = ScriptInterpreter::Instance();
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)theScriptInterpreter\n", theScriptInterpreter);
      #endif
      
      // Load plugins listed in the startup file
      LoadPlugins();
      
      // Create default SolarSystem
      theDefaultSolarSystem = CreateSolarSystem("DefaultSolarSystem");
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)theDefaultSolarSystem\n", theDefaultSolarSystem);
      #endif
      
      theConfigManager->SetDefaultSolarSystem(theDefaultSolarSystem);
      
      // Create solar system in use
      /// @note: If the solar system can be configured by name, add it to the
      ///        ConfigManager by calling CreateSolarSystem().
      ///        Until then, just use solar system name as "SolarSystem"
      
      CreateSolarSystemInUse();
      
      // Create other files in use
      CreatePlanetaryCoeffFile();
      CreateTimeFile();
      
      // Create at least 1 Sandbox and NoOp Command
      Sandbox *sandbox = new Sandbox();
      GmatCommand *noOp = new NoOp();
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add(sandbox, "Sandbox", "Moderator::Initialize()", "");
      MemoryTracker::Instance()->Add(noOp, "NoOP", "Moderator::Initialize()", "");
      #endif
      
      sandboxes.push_back(sandbox);
      commands.push_back(noOp);
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  (%p)Sandbox 1\n", sandboxes[0]);
      MessageInterface::ShowMessage
         (".....created  (%p)%s\n", commands[0], commands[0]->GetTypeName().c_str());
      #endif
      
      // set objectMapInUse (loj: 2008.05.23)
      objectMapInUse = theConfigManager->GetObjectMap();
      
      #ifdef DEBUG_OBJECT_MAP
      MessageInterface::ShowMessage
         ("Moderator::Initialize() objectMapInUse was set to the "
          "configuration map <%p>\n", objectMapInUse);
      #endif
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Error occurred during initialization: " +
          e.GetFullMessage());
      return false;
   }
   catch (...)
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Unknown Error occurred during initialization");
      return false;
   }
   
   // Let's put current time out
   time_t rawtime;
   struct tm * timeinfo;
   
   time(&rawtime);
   timeinfo = localtime(&rawtime);
   char timestr[80];
   strftime(timestr, 80, "%Y-%m-%d %H:%M:%S",timeinfo);
   
   MessageInterface::ShowMessage
      ("%s GMAT Moderator successfully created core engine\n", timestr);
   
   // Check to see if there are any event locator factories
   StringArray elList = theFactoryManager->GetListOfItems(Gmat::EVENT_LOCATOR);
   if (elList.size() > 0)
      GmatGlobal::Instance()->SetEventLocationAvailable(true);

   // Check if MatlabInterface is required
   if (GmatGlobal::Instance()->GetMatlabMode() == GmatGlobal::NO_MATLAB)
   {
      MessageInterface::ShowMessage
         ("*** Use of MATLAB is disabled from the gmat_startup_file\n");
   }
   else
   {
      try
      {
         theMatlabInterface = theFactoryManager->CreateInterface("MatlabInterface", "MI");
         #ifdef DEBUG_MATLAB
         MessageInterface::ShowMessage
            ("Moderator::Initialize() theMatlabInterface=<%p>\n", theMatlabInterface);
         #endif
         // Check if MATLAB is installed
         // Do not override matlab setting in the startup file since
         // GmatFileUtil::IsAppInstalled() not implemented for all platforms
         std::string appLoc;
         bool hasMatlab = GmatFileUtil::IsAppInstalled("MATLAB", appLoc);
         // Since GmatFileUtil::IsAppInstalled() is not complete for all platforms,
         // assume there is MATLAB for now. (LOJ: 2010.04.07)
         hasMatlab = true;
         if (hasMatlab)
         {
            #ifdef DEBUG_MATLAB
            MessageInterface::ShowMessage
               ("*** MATLAB is installed in '%s'\n", appLoc.c_str());
            #endif
            GmatGlobal::Instance()->SetMatlabAvailable(true);
         }
         else
         {
            #ifdef DEBUG_MATLAB
            MessageInterface::ShowMessage("*** MATLAB is not installed\n");
            #endif
         }
      }
      catch (BaseException &be)
      {
         MessageInterface::ShowMessage(be.GetFullMessage());
      }
   }

   // Set MatlabInterface run mode, that is SINGLE_USE or SHARED MATLAB engine.
   if (theMatlabInterface == NULL)
      GmatGlobal::Instance()->SetMatlabMode(GmatGlobal::NO_MATLAB);
   else
      theMatlabInterface->
         SetIntegerParameter("MatlabMode",
                             GmatGlobal::Instance()->GetMatlabMode());
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Moderator::Initialize() returning true\n");
   #endif
   
   isInitialized = true;
   return true;;
} // Initialize()


//------------------------------------------------------------------------------
// bool IsInitialized()
//------------------------------------------------------------------------------
/**
 * Checks to see if Initialize() has succeeded (at least one time).#include <iostream>
 *
 *
 * @return true if Initialize() has been executed successfully, false if not
 */
//------------------------------------------------------------------------------
bool Moderator::IsInitialized()
{
   return isInitialized;
}


//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/*
 * Finalizes the system by closing all opened files by deleting objects.
 */
//------------------------------------------------------------------------------
void Moderator::Finalize()
{
   MessageInterface::ShowMessage("Moderator is deleting core engine...\n");
   
   #if DEBUG_FINALIZE > 0
   MessageInterface::ShowMessage("Moderator::Finalize() entered\n");
   #endif
   
   #if DEBUG_FINALIZE > 1
   //Notes: Do not use %s for command string, it may crash when it encounters
   // comment with % in the scripts
   GmatCommand *cmd = GetFirstCommand();
   MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
   MessageInterface::ShowMessage(GetScript());
   #endif
   
   #if DEBUG_FINALIZE > 0
   MessageInterface::ShowMessage
      (".....Moderator::Finalize() deleting (%p)theFileManager\n", theFileManager);
   MessageInterface::ShowMessage
      (".....Moderator::Finalize() deleting (%p)theEopFile\n", theEopFile);
   MessageInterface::ShowMessage
      (".....Moderator::Finalize() deleting (%p)theItrfFile\n", theItrfFile);
   MessageInterface::ShowMessage
      (".....Moderator::Finalize() deleting (%p)theLeapSecsFile\n", theLeapSecsFile);
   #endif
   
   delete theFileManager;
   delete theEopFile;
   delete theItrfFile;
   delete theLeapSecsFile;
   if (theMatlabInterface != NULL)
   {
      #if DEBUG_FINALIZE > 0
            MessageInterface::ShowMessage
            (".....Moderator::Finalize() deleting (%p)theMatlabInterface\n",
             theMatlabInterface);
      #endif
      delete theMatlabInterface;
      theMatlabInterface = NULL;
   }
   
   theFileManager = NULL;
   theEopFile = NULL;
   theItrfFile = NULL;
   theLeapSecsFile = NULL;
   
   //clear resource and command sequence
   try
   {
      #if DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage(".....clearing resource\n");
      MessageInterface::ShowMessage(".....clearing command sequence\n");
      #endif
      
      // Clear command sequence before resource (loj: 2008.07.10)
      // only 1 sandbox for now
      ClearCommandSeq(false, false);
      ClearResource();
      
      // Delete the plugin resource data
      for (UnsignedInt i = 0; i < userResources.size(); ++i)
         delete userResources[i];
      userResources.clear();

      // Close out the plug-in libraries
      std::map<std::string, DynamicLibrary*>::iterator i;
      for (i = userLibraries.begin(); i != userLibraries.end(); ++i)
      {
         delete i->second;
         i->second = NULL;
         #ifdef DEBUG_PLUGINS
            MessageInterface::ShowMessage("Closed %s.\n", i->first.c_str());
         #endif
      }

      
//       #if DEBUG_FINALIZE > 0
//       MessageInterface::ShowMessage
//          (".....deleting (%p)theGuiInterpreter\n", theGuiInterpreter);
//       #endif
//       delete theGuiInterpreter;
//       theGuiInterpreter = NULL;
      
      //delete theFactoryManager; (private destructor)
      //delete theConfigManager; (private destructor)
      //delete theScriptInterpreter; (private destructor)
      //delete theGuiInterpreter; (private destructor)
      #if DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage
         (".....Moderator::Finalize() deleting (%p)thePublisher\n",
          thePublisher);
      #endif      
      delete thePublisher;
      
      // delete solar systems
      #if DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage
         (".....Moderator::Finalize() deleting (%p)theDefaultSolarSystem\n",
          theDefaultSolarSystem);
      #endif      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (theDefaultSolarSystem, theDefaultSolarSystem->GetName(),
          "Moderator::Finalize()");
      #endif
      delete theDefaultSolarSystem;
      theDefaultSolarSystem = NULL;
      
      if (theSolarSystemInUse != NULL)
      {
         #if DEBUG_FINALIZE > 0
         MessageInterface::ShowMessage
            (".....Moderator::Finalize() deleting (%p)theSolarSystemInUse\n",
             theSolarSystemInUse);
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (theSolarSystemInUse, theSolarSystemInUse->GetName(),
             "Moderator::Finalize()");
         #endif
         delete theSolarSystemInUse;
         theSolarSystemInUse = NULL;
      }
      
      // delete internal coordinate system
      if (theInternalCoordSystem != NULL)
      {
         #if DEBUG_FINALIZE > 0
         MessageInterface::ShowMessage
            (".....Moderator::Finalize() deleting (%p)theInternalCoordSystem\n",
             theInternalCoordSystem);
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (theInternalCoordSystem, theInternalCoordSystem->GetName(),
             "Moderator::Finalize()");
         #endif
         delete theInternalCoordSystem;
         theInternalCoordSystem = NULL;
      }
      
      // Delete unmanaged functions (LOJ: 2009.03.24)
      // This causes crash on Func_MatlabObjectPassingCheck.script
      // so disable until it is fully tested (LOJ: 2009.04.08)
      #ifdef __ENABLE_CLEAR_UNMANAGED_FUNCTIONS__
      #if DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage
         (".....Moderator::Finalize() deleting %d unmanaged functions\n",
          unmanagedFunctions.size());
      #endif
      for (UnsignedInt i=0; i<unmanagedFunctions.size(); i++)
      {
         GmatBase *func = (GmatBase*)(unmanagedFunctions[i]);
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (func, func->GetName(), "Moderator::Finalize()",
             "deleting unmanaged function");
         #endif
         delete func;
         func = NULL;
      }
      unmanagedFunctions.clear();
      #endif
      
      // delete Sandbox (only 1 Sandbox for now)
      #if DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage
         (".....Moderator::Finalize() deleting (%p)sandbox 1\n", sandboxes[0]);
      #endif
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (sandboxes[0], "Sandbox", "Moderator::Finalize()");
      #endif
      delete sandboxes[0];
      commands[0] = NULL;
      sandboxes[0] = NULL;
      commands.clear();
      sandboxes.clear();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_MEMORY
   StringArray tracks = MemoryTracker::Instance()->GetTracks(true, false);
   MessageInterface::ShowMessage
      ("===> There are %d memory tracks after Finalize\n", tracks.size());
   for (UnsignedInt i=0; i<tracks.size(); i++)
      MessageInterface::ShowMessage("%s\n", tracks[i].c_str());
   #endif
   
   #if DEBUG_FINALIZE > 0
   MessageInterface::ShowMessage("Moderator::Finalize() exiting\n");
   #endif
} // Finalize()


//------------------------------------------------------------------------------
// void SetRunReady(bool flag = false)
//------------------------------------------------------------------------------
void Moderator::SetRunReady(bool flag)
{
   isRunReady = flag;
}

//------------------------------------------------------------------------------
// Integer GetExitCode()
//------------------------------------------------------------------------------
/**
* Returns the current exit code setting representing a successful or failed run
*/
//------------------------------------------------------------------------------
Integer Moderator::GetExitCode()
{
   return exitCode;
}

//------------------------------------------------------------------------------
// Integer IsFromGui()
//------------------------------------------------------------------------------
/**
* Returns boolean of whether GMAT is being run from the GUI or not
*/
//------------------------------------------------------------------------------
bool Moderator::IsFromGui()
{
   return isFromGui;
}

//---------------------------------------------------------------------------
//  std::string ToString() const
//---------------------------------------------------------------------------
/**
 * Retrieves a string representation of this object
 *
 * @return A string representation of this object
 */
 //------------------------------------------------------------------------------
std::string Moderator::ToString() const
{
   std::string outString = "Instance of GMAT Moderator ";
   outString += (isInitialized ? "is initialized." : "is not initialized.");
   if (isRunReady)
      outString += " Script ready to run.";
   else
      outString += " No script ready to run.";
   return outString;
}


//------------------------------------------------------------------------------
// Interface* GetMatlabInterface()
//------------------------------------------------------------------------------
/**
 * Returns MatlabInterface pointer.
 */ 
//------------------------------------------------------------------------------
Interface* Moderator::GetMatlabInterface()
{
   return theMatlabInterface;
}


//------------------------------------------------------------------------------
// bool OpenMatlabEngine()
//------------------------------------------------------------------------------
bool Moderator::OpenMatlabEngine()
{
   #ifdef DEBUG_MATLAB
   MessageInterface::ShowMessage
      ("Moderator::OpenMatlabEngine() theMatlabInterface=<%p>\n",
       theMatlabInterface);
   #endif
   
   if (theMatlabInterface != NULL)
   {
      #ifdef DEBUG_MATLAB
      MessageInterface::ShowMessage
         ("Moderator::OpenMatlabEngine() calling theMatlabInterface->Open()\n");
      #endif
      
      if (theMatlabInterface->Open() == 1)
         return true;
      else
         return false;
   }
   else
   {
      #ifdef DEBUG_MATLAB
      MessageInterface::ShowMessage
         ("Moderator::OpenMatlabEngine() theMatlabInterface is NULL, so returning false\n");
      #endif
      return false;
   }
}


//------------------------------------------------------------------------------
// bool CloseMatlabEngine()
//------------------------------------------------------------------------------
bool Moderator::CloseMatlabEngine()
{
   #ifdef DEBUG_MATLAB
   MessageInterface::ShowMessage
      ("Moderator::CloseMatlabEngine() theMatlabInterface=<%p>\n",
       theMatlabInterface);
   #endif
   
   if (theMatlabInterface != NULL)
   {
      #ifdef DEBUG_MATLAB
      MessageInterface::ShowMessage
         ("Moderator::CloseMatlabEngine() calling theMatlabInterface->Close()\n");
      #endif
      
      if (theMatlabInterface->Close() == 1)
         return true;
      else
         return false;
   }
   else
   {
      #ifdef DEBUG_MATLAB
      MessageInterface::ShowMessage
         ("Moderator::OpenMatlabEngine() theMatlabInterface is NULL, so returning false\n");
      #endif
      return false;
   }
}


//---------------------------- Plug-in modules ---------------------------------

//------------------------------------------------------------------------------
// void LoadPlugins()
//------------------------------------------------------------------------------
/**
 * Method that loads in the plug-in libraries listed in a user's startup file.
 * 
 * The GMAT startup file may list one or more plug-in libraries by name.  This 
 * method retrieves the list of libraries, and loads them into GMAT.
 * 
 * @note The current code looks for exactly one library -- the VF13ad library --
 *       and loads it into GMAT if found.  The generic updates for any user 
 *       library will be added in a later build.
 */ 
//------------------------------------------------------------------------------
void Moderator::LoadPlugins()
{
   StringArray pluginList = theFileManager->GetPluginList();

   // This is done for all plugins in the startup file
   for (StringArray::const_iterator i = pluginList.begin(); 
         i != pluginList.end(); ++i)
   {

      #ifndef __WIN32__
   
         #ifdef DEBUG_PLUGIN_REGISTRATION
            MessageInterface::ShowMessage(
                  "*** Loading dynamic library \"%s\": ", i->c_str());
         #endif
         LoadAPlugin(*i);

      #else
         
         #ifdef DEBUG_PLUGIN_REGISTRATION
            MessageInterface::ShowMessage(
                  "*** Loading dynamic library \"%s\": ", i->c_str());
         #endif
         LoadAPlugin(*i);
        
      #endif
   }
   
   if (theUiInterpreter != NULL)
      theUiInterpreter->BuildCreatableObjectMaps();
   if (theScriptInterpreter)
      theScriptInterpreter->BuildCreatableObjectMaps(true);
}

//------------------------------------------------------------------------------
// void LoadAPlugin(std::string pluginName)
//------------------------------------------------------------------------------
/**
 * Method that loads a plug-in library into memory.
 * 
 * This method loads a plug-in library into memory, and retrieves and registers 
 * any Factories contained in that plug-in.  If the library is not found, this 
 * method just returns.
 * 
 * @param pluginName The file name for the plug-in library.  The name should not 
 *                   include the file extension (e.g. ".ddl" or ".so").
 */ 
//------------------------------------------------------------------------------
void Moderator::LoadAPlugin(std::string pluginName)
{
   // Set platform specific slash style
   #ifdef DEBUG_PLUGIN_REGISTRATION
      MessageInterface::ShowMessage("Input plugin name: \"%s\"\n", pluginName.c_str());
   #endif

   char fSlash = '/';
   char bSlash = '\\';
   char osSlash = '\\';       // Default to Windows, but change if *nix

   #ifndef _WIN32
      osSlash = '/';          // Mac or Linux
   #endif

   #ifdef DEBUG_PLUGIN_REGISTRATION
      MessageInterface::ShowMessage("OS slash is \"%c\"\n", osSlash);
   #endif

   for (UnsignedInt i = 0; i < pluginName.length(); ++i)
   {
      if ((pluginName[i] == fSlash) || (pluginName[i] == bSlash))
         pluginName[i] = osSlash;
   }

   #ifdef DEBUG_PLUGIN_REGISTRATION
      MessageInterface::ShowMessage("Used plugin name:  \"%s\"\n", pluginName.c_str());
   #endif

   #ifdef __linux__
      if (!isFromGui)
      {
         // Linux exclusion code: if running without GUI, don't load GUI plugin
         std::ifstream ifs(pluginName + ".so", std::ios::binary);
         if (ifs.is_open())
         {
            std::string str((std::istreambuf_iterator<char>(ifs)),
                  std::istreambuf_iterator<char>());
            size_t pos = str.find("PluginWidget");
            if (pos != std::string::npos)
            {
               MessageInterface::ShowMessage("Skipping \"%s\": GUI plugins are "
                     "skipped in console mode\n", pluginName.c_str());
               return;
            }
         }
      }
   #endif

   DynamicLibrary *theLib = LoadLibrary(pluginName);

   if (theLib != NULL)
   {
      Integer fc = theLib->GetFactoryCount();

      if (fc > 0)
      {
         // Do the GMAT factory dance
         #ifdef DEBUG_PLUGIN_REGISTRATION
            MessageInterface::ShowMessage(
               "Library %s contains %d %s.\n", pluginName.c_str(), fc,
               ( fc==1 ? "factory" : "factories"));
         #endif
            
         // Now pass factories to the FactoryManager
         Factory *newFactory = NULL;
         for (Integer i = 0; i < fc; ++i)
         {
            newFactory = theLib->GetGmatFactory(i);
            if (newFactory != NULL)
            {
               if (theFactoryManager->RegisterFactory(newFactory) == false)
                  MessageInterface::ShowMessage(
                        "Factory %d in library %s failed to register with the "
                        "Factory Manager.\n", i, pluginName.c_str());
               else
               {
                  #ifdef DEBUG_PLUGIN_REGISTRATION
                     MessageInterface::ShowMessage(
                        "Factory %d in library %s is now registered with the "
                        "Factory Manager!\n", i, pluginName.c_str());

                     StringArray facts = newFactory->GetListOfCreatableObjects();

                     MessageInterface::ShowMessage(
                           "The new factory creates these objects types:\n");

                     for (UnsignedInt f = 0; f < facts.size(); ++f)
                        MessageInterface::ShowMessage("   %s\n",
                              facts[f].c_str());
                  #endif
               }
            } 
            else
               MessageInterface::ShowMessage(
                     "Factory %d in library %s was not constructed; a NULL "
                     "pointer was returned instead.\n", i, pluginName.c_str());
         }
      }
      else
         MessageInterface::PutMessage(
            "*** Library \"" + pluginName + "\" does not contain a factory\n");
      
      // Test to see if there might be TriggerManagers
      Integer triggerCount = theLib->GetTriggerManagerCount();
      #ifdef DEBUG_PLUGIN_REGISTRATION
         MessageInterface::ShowMessage(
            "Library %s contains %d %s.\n", pluginName.c_str(), triggerCount,
            ( triggerCount == 1 ? "TriggerManager" : "TriggerManagers"));
      #endif

      for (Integer i = 0; i < triggerCount; ++i)
      {
         TriggerManager *tm = theLib->GetTriggerManager(i);
         triggerManagers.push_back(tm);

         #ifdef DEBUG_PLUGIN_REGISTRATION
            MessageInterface::ShowMessage(
               "   TriggerManager %d of type %s is now registered.\n", i,
               tm->GetTriggerTypeString().c_str());
         #endif
      }
      // Check for new GUI elements
      Integer menuCount = theLib->GetMenuEntryCount();
      #ifdef DEBUG_PLUGIN_REGISTRATION
         MessageInterface::ShowMessage(
            "Library %s contains %d %s.\n", pluginName.c_str(), menuCount,
            ( menuCount == 1 ? "menu entry" : "menu entries"));
      #endif

      for (Integer i = 0; i < menuCount; ++i)
      {
         Gmat::PluginResource* res = theLib->GetMenuEntry(i);
         if (res != NULL)
         {
            #ifdef DEBUG_PLUGIN_REGISTRATION
               MessageInterface::ShowMessage("Adding user resource node:\n"
                     "   Name: %s\n   Parent: %s\n   type: %d\n"
                     "   subtype: %s\n", res->nodeName.c_str(),
                     res->parentNodeName.c_str(), res->type,
                     res->subtype.c_str());
            #endif
            userResources.push_back(res);
         }

         #ifdef DEBUG_PLUGIN_REGISTRATION
            MessageInterface::ShowMessage(
               "   Menu entry %d for node %s is now registered.\n", i,
               res->nodeName.c_str());
         #endif
      }

      // Get the GUI Factories
      Integer guiFactoryCount = theLib->GetGuiFactoryCount();
      for (Integer i = 0; i < guiFactoryCount; ++i)
      {
         GuiFactory *guiFact = theLib->GetGuiFactory(i);
         pluginGuiFactories.push_back(guiFact);
      }
   }
   else
   {
      if (pluginName.find("libFminconOptimizer") != pluginName.npos)
      {
         MessageInterface::PutMessage(
               "*** Error loading \"libFminconOptimizer\": "
               "Either libMatlabInterface could not load or is disabled, "
               "or the MATLAB Optimization Toolbox is not available.  "
               "Disable the appropriate PLUGIN line in "
               "bin/gmat_startup_file.txt to remove this error.\n");
      }
      else if (pluginName.find("libMatlabInterface") != pluginName.npos)
      {
         MessageInterface::PutMessage(
               "*** Error loading \"libMatlabInterface\": "
               "GMAT could not load the MATLAB interface. "
               "Please check that MATLAB is installed and configured "
               "correctly.  See the User Guide for instructions, or "
               "disable the appropriate PLUGIN line in "
               "bin/gmat_startup_file.txt to remove this error.\n");
      }
      else if (pluginName.find("libPythonInterface") != pluginName.npos)
      {
         MessageInterface::PutMessage(
            "*** Error loading \"" + pluginName + "\": "
            "GMAT could not load the Python interface. "
            "Please check that bin/gmat_startup_file.txt contains the PLUGIN line for "
            "\"libPythonInterface_py3X\" corresponding to an installed Python 3.X distribution. "
            "See the User Guide for instructions, or "
            "disable the appropriate PLUGIN line in "
            "bin/gmat_startup_file.txt to remove this error.\n");
      }
      else if(pluginName.find("libExternalForceModel") != pluginName.npos)
      {
         MessageInterface::PutMessage(
            "*** Error loading \"" + pluginName + "\": "
            "GMAT could not load the External Force Model interface. "
            "Please check that bin/gmat_startup_file.txt contains the PLUGIN lines for "
            "\"libExternalForceModel_py3X\" and \"libPythonInterface_py3X\" corresponding "
            "to an installed Python 3.X distribution. "
            "See the User Guide for instructions, or "
            "disable the appropriate PLUGIN line in "
            "bin/gmat_startup_file.txt to remove this error.\n");
      }
      else
      {
         MessageInterface::PutMessage(
            "*** Unable to load the dynamic library \"" + pluginName + "\"\n");
      }
   }
}

//------------------------------------------------------------------------------
// Dynamic library specific code
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DynamicLibrary *LoadLibrary(const std::string &libraryName)
//------------------------------------------------------------------------------
/**
 * Loads a dynamic library into memory.
 * 
 * Creates a DynamicLibrary object and uses that instance to provide the 
 * interfaces GMAT uses to load a dynamic library into memory.  If the library 
 * is not found or if it could not be loaded, a message is written out stating 
 * that the library did not open.
 * 
 * @param libraryName The file name for the plug-in library.  The name should 
 *                    not include the file extension.
 * 
 * @return The DynamicLibrary object that supplies the library interfaces, or a
 *         NULL pointer if the library did not load.
 */ 
//------------------------------------------------------------------------------
DynamicLibrary *Moderator::LoadLibrary(const std::string &libraryName)
{
   // libraryName may be a filename or a pathname. If the latter, split
   // it into its actual filename and path components. 
   std::string libName, libPath;
   size_t leafloc = libraryName.find_last_of("/\\"); // *nix or Windows
   if(leafloc != std::string::npos) // libraryName is a path
   {
      libPath = libraryName.substr(0, leafloc+1);
      libName = libraryName.substr(leafloc+1);
   }
   else // libraryName is a filename
   {
      libName = libraryName;
   }

   DynamicLibrary *theLib = new DynamicLibrary(libName, libPath);
   if (theLib->LoadDynamicLibrary())
   {
      userLibraries[libName] = theLib;
   }
   else
   {
      MessageInterface::ShowMessage("*** Library \"%s\" did not open.\n",
            libraryName.c_str());
      delete theLib;
      theLib = NULL;
   }
   
   return theLib;
}

//------------------------------------------------------------------------------
// bool IsLibraryLoaded(const std::string &libName)
//------------------------------------------------------------------------------
/**
 * Method that checks to see if a specified library has been loaded.
 * 
 * @param libName The name of the library.
 * 
 * @return true if the library has been loaded, false if not.
 */ 
//------------------------------------------------------------------------------
bool Moderator::IsLibraryLoaded(const std::string &libName)
{
   bool retval = false;
   if (userLibraries.find(libName) != userLibraries.end())
      retval = true;
   
   return retval;
}

//------------------------------------------------------------------------------
// void (*GetDynamicFunction(const std::string &funName, 
//                           const std::string &libraryName))()
//------------------------------------------------------------------------------
/**
 * Retrieves a specified function from a specified library.
 * 
 * @param funName The name of the requested function.
 * @param libraryName Teh name of the library that contains the function.
 * 
 * @return A function pointer for the specified function, or NULL if the 
 *         function is not found.  The returned function pointer has the 
 *         signature
 *             void (*)()
 *         and should be cast to the correct signature.
 */ 
//------------------------------------------------------------------------------
void (*Moderator::GetDynamicFunction(const std::string &funName, 
      const std::string &libraryName))()
{
   void (*theFunction)() = NULL;
   if (IsLibraryLoaded(libraryName))
   {
     try
     {
       theFunction = userLibraries[libraryName]->GetFunction(funName);
     }
     catch (...)
     { 
       MessageInterface::ShowMessage("Moderator::GetDynamicFunction() error");
       return NULL;
     }
   }

   return theFunction;
}


//------------------------------------------------------------------------------
// std::vector<GuiFactory*> RetrieveGuiFactories()
//------------------------------------------------------------------------------
/**
 * Retrieves the GUI plugin factories for use in the GUI
 *
 * @return The GUI plugin factory list
 */
//------------------------------------------------------------------------------
std::vector<GuiFactory*> Moderator::RetrieveGuiFactories()
{
   #ifdef DEBUG_GUI_PLUGINS
      MessageInterface::ShowMessage("The Moderator has %d GUI Factories\n",
            pluginGuiFactories.size());
      for (UnsignedInt i = 0; i < pluginGuiFactories.size(); ++i)
         MessageInterface::ShowMessage("   %d: %p\n", i, pluginGuiFactories[i]);
   #endif

   return pluginGuiFactories;
}

//------------------------ End of Plug-in Code ---------------------------------


//----- ObjectType
//------------------------------------------------------------------------------
// std::string GetObjectTypeString(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns object type name of given object type.
 *
 * @param <type> object type
 *
 * @return object type name
 */
//------------------------------------------------------------------------------
std::string Moderator::GetObjectTypeString(UnsignedInt type)
{
   return GmatBase::GetObjectTypeString(type);
}

//----- interpreter
//------------------------------------------------------------------------------
// GuiInterpreter* GetGuiInterpreter()
//------------------------------------------------------------------------------
/**
 * Returns GuiInterpreter pointer.
 *
 * @return GuiInterpreter pointer
 */
//------------------------------------------------------------------------------
ScriptInterpreter* Moderator::GetUiInterpreter()
{
   return theUiInterpreter;
}

//------------------------------------------------------------------------------
// ScriptInterpreter* GetScriptInterpreter()
//------------------------------------------------------------------------------
/**
 * Returns ScriptInterpreter pointer.
 *
 * @return ScriptInterpreter pointer
 */
//------------------------------------------------------------------------------
ScriptInterpreter* Moderator::GetScriptInterpreter()
{
   return theScriptInterpreter;
}


//------------------------------------------------------------------------------
// void SetGuiInterpreter(GuiInterpreter *guiInterp)
//------------------------------------------------------------------------------
/**
 * Sets GuiInterpreter pointer.
 */
//------------------------------------------------------------------------------
void Moderator::SetUiInterpreter(ScriptInterpreter *uiInterp)
{
   theUiInterpreter = uiInterp;
   theUiInterpreter->BuildCreatableObjectMaps();
   SetScriptInterpreter(uiInterp);
}


//------------------------------------------------------------------------------
// void SetScriptInterpreter(ScriptInterpreter *scriptInterp)
//------------------------------------------------------------------------------
/**
 * Sets ScriptInterpreter pointer.
 */
//------------------------------------------------------------------------------
void Moderator::SetScriptInterpreter(ScriptInterpreter *scriptInterp)
{
   if (theScriptInterpreter == NULL)
      theScriptInterpreter = scriptInterp;
}


//------------------------------------------------------------------------------
// void SetInterpreterMapAndSS(Interpreter *interp)
//------------------------------------------------------------------------------
/**
 * Sets Interpreter ObjectMap and SolarSystem to current pointers in use.
 * 
 * @param interp The Interpreter that is setup. 
 */
//------------------------------------------------------------------------------
void Moderator::SetInterpreterMapAndSS(Interpreter *interp)
{
   interp->SetObjectMap(objectMapInUse, true);
   interp->SetSolarSystemInUse(theSolarSystemInUse);
}

//----- object finding
//------------------------------------------------------------------------------
// void SetObjectMap(ObjectMap *objMap)
//------------------------------------------------------------------------------
void Moderator::SetObjectMap(ObjectMap *objMap)
{
   if (objMap != NULL)
   {
      if (objectMapInUse != objMap)
      {
         #ifdef DEBUG_OBJECT_MAP
         ShowObjectMap("Moderator::SetObjectMap() Here is the old object map in use", objectMapInUse);
         MessageInterface::ShowMessage
            ("Moderator::SetObjectMap() objectMapInUse will be set to new objMap <%p>, "
             "currentFunction='%s'\n", objMap,
             currentFunction ? currentFunction->GetName().c_str() : "NULL");
         ShowObjectMap("Moderator::SetObjectMap() Here is the new object map in use", objMap);
         #endif
         
         previousObjectMap = objectMapInUse;
         objectMapInUse = objMap;
      }
   }
}


//------------------------------------------------------------------------------
// void SetObjectManageOption(Integer option)
//------------------------------------------------------------------------------
/*
 * Sets object manage option. Usually objects created inside GmatFunction uses
 * object maps passed to Moderator. All objects created in the main sequence and
 * through the GUI are managed through Configuration Manager.
 *
 * @param <option>  0, if object is not managed
 *                  1, if configuration object map is used for finding objects
 *                     and adding new objects (default)
 *                  2, if function object map is used for finding objects and
 *                     adding new objects including automatic objects
 */
//------------------------------------------------------------------------------
void Moderator::SetObjectManageOption(Integer option)
{
   #ifdef DEBUG_OBJECT_MANAGE
   MessageInterface::ShowMessage
      ("=====> Moderator::SetObjectManageOption() option = %d\n", option);
   #endif
   objectManageOption = option;
}


//------------------------------------------------------------------------------
// Integer GetObjectManageOption()
//------------------------------------------------------------------------------
/*
 * returns object manage option.
 */
//------------------------------------------------------------------------------
Integer Moderator::GetObjectManageOption()
{
   return objectManageOption;
}


//------------------------------------------------------------------------------
// bool ResetObjectPointer(GmatBase *newObj, const std::string &name)
//------------------------------------------------------------------------------
/*
 * Sets configured object pointer with new pointer.
 *
 * @param  newObj  New object pointer
 * @param  name  Name of the configured object to be reset
 *
 * @return  true if pointer was reset, false otherwise
 */
//------------------------------------------------------------------------------
void Moderator::ResetObjectPointer(ObjectMap *objMap, GmatBase *newObj,
                                   const std::string &name)
{
   #if DEBUG_RESET_OBJECT
   MessageInterface::ShowMessage
      ("Moderator::ResetObjectPointer() entered, objMap=<%p>, newObj=<%p>, "
       "name='%s'\n", objMap, obj, newObj, name.c_str());
   #endif
   
   if (objMap->find(name) != objMap->end())
   {
      GmatBase *mapObj = (*objMap)[name];
      if (mapObj->GetName() == name)
      {
         // We want to replace if it has the same sub type
         if (newObj->IsOfType(mapObj->GetTypeName()))
         {
            #if DEBUG_RESET_OBJECT
            MessageInterface::ShowMessage
               ("   Replacing mapObj=<%p> with newObj=<%p>\n", mapObj, newObj);
            #endif
            (*objMap)[name] = newObj;
         }
      }
   }
   
   #if DEBUG_RESET_OBJECT
   MessageInterface::ShowMessage("Moderator::ResetObjectPointer() leaving\n");
   #endif
}


//----- factory
//------------------------------------------------------------------------------
// const StringArray& GetListOfFactoryItems(UnsignedInt type,
//       const std::string &qualifier)
//------------------------------------------------------------------------------
/**
 * Returns names of all configurable items of object type.
 *
 * @param <type> object type
 * @param qualifier Qualifier for filtered lists
 *
 * @return array of configurable item names; return empty array if none
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfFactoryItems(UnsignedInt type, const std::string &qualifier)
{
   return theFactoryManager->GetListOfItems(type, qualifier);
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfAllFactoryItems()
//------------------------------------------------------------------------------
/**
 * Return a list of all items that can be created.
 *
 * @return list of all creatable items.
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfAllFactoryItems()
{
   return theFactoryManager->GetListOfAllItems();
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfAllFactoryItemsExcept(const ObjectTypeArray &types)
//------------------------------------------------------------------------------
/**
 * Returns names of all configurable items of object type except given type
 *
 * @param <type> object types to be excluded
 *
 * @return array of configurable item names excluding types;
 *         return empty array if none
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfAllFactoryItemsExcept(const ObjectTypeArray &types)
{
   return theFactoryManager->GetListOfAllItemsExcept(types);
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfViewableItems(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Return a list of all viewable objets via the GUI
 *
 * @param  type  The enumerated ObjectType defined in gmatdef.hpp
 *
 * @return list of all viewable objects
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfViewableItems(UnsignedInt type)
{
   return theFactoryManager->GetListOfViewableItems(type);
}

//------------------------------------------------------------------------------
// const StringArray& GetListOfUnviewableItems(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Return a list of all unviewable objects via the GUI
 *
 * @return list of all viewable objects
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfUnviewableItems(UnsignedInt type)
{
   return theFactoryManager->GetListOfUnviewableItems(type);
}


//------------------------------------------------------------------------------
// const ObjectTypeArrayMap& GetAllObjectTypeArrayMap()
//------------------------------------------------------------------------------
/**
 * Return a map of all items that can be created.
 *
 * @return map of all creatable items with object type as a key
 */
//------------------------------------------------------------------------------
const ObjectTypeArrayMap& Moderator::GetAllObjectTypeArrayMap()
{
   return theFactoryManager->GetAllObjectTypeArrayMap();
}

//------------------------------------------------------------------------------
// bool DoesObjectTypeMatchSubtype(const std::string &theType,
//       const std::string &theSubtype)
//------------------------------------------------------------------------------
/**
 * Checks if a creatable object type matches a subtype.
 *
 * @param coreType The ObjectType of the candidate object
 * @param theType The script identifier for the object type
 * @param theSubtype The subtype being checked
 *
 * @return true if the scripted type and subtype match, false if the type does
 *         not match the subtype
 */
//------------------------------------------------------------------------------
bool Moderator::DoesObjectTypeMatchSubtype(const UnsignedInt coreType,
      const std::string &theType, const std::string &theSubtype)
{
   return theFactoryManager->DoesObjectTypeMatchSubtype(coreType, theType,
         theSubtype);
}


//----- configuration
//------------------------------------------------------------------------------
// ObjectMap* GetConfiguredObjectMap()
//------------------------------------------------------------------------------
/*
 * Returns configured object map
 */
//------------------------------------------------------------------------------
ObjectMap* Moderator::GetConfiguredObjectMap()
{
   return theConfigManager->GetObjectMap();
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfObjects(UnsignedInt type,
//                                     bool excludeDefaultObjects,
//                                     bool typemax)
//------------------------------------------------------------------------------
/**
 * Returns names of all configured items of object type.
 *
 * @param <type> object type
 * @param <excludeDefaultObjects> set this flag to true if default objects
 *           should be excluded, such as  default coordinate systems [false]
 * @param typemax Used to retrieve a range of types; this is the upper limit on
 *           the range (default, 0, means to only return type)
 *
 * @return array of configured item names of the type; return empty array if none
 *  return all configured item if type is UNKNOWN_OBJECT
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfObjects(UnsignedInt type,
      bool excludeDefaultObjects, UnsignedInt typemax)
{
   tempObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT)
      return theConfigManager->GetListOfAllItems();
   
   if (type == Gmat::CELESTIAL_BODY || type == Gmat::SPACE_POINT)
   {
      tempObjectNames.clear();
      
      if (theSolarSystemInUse == NULL)
         return tempObjectNames;
      
      if (type == Gmat::CELESTIAL_BODY)
      {
         // add bodies to the list
         tempObjectNames = theSolarSystemInUse->GetBodiesInUse();
      }
      else if (type == Gmat::SPACE_POINT)
      {
         // add Spacecraft to the list
         tempObjectNames = theConfigManager->GetListOfItems(Gmat::SPACECRAFT);
         
         // add bodies to the list
         StringArray bodyList = theSolarSystemInUse->GetBodiesInUse();
         for (UnsignedInt i=0; i<bodyList.size(); i++)
            tempObjectNames.push_back(bodyList[i]);
         
         // add CalculatedPoint to the list
         StringArray calptList =
            theConfigManager->GetListOfItems(Gmat::CALCULATED_POINT);
         // Do not add default (built-in) barycenter(s) on option
         #ifdef DEBUG_LIST_CALCULATED_POINT
            MessageInterface::ShowMessage("There are %d configured calculated points.\n", (Integer) calptList.size());
         #endif
         if (excludeDefaultObjects)
         {
            #ifdef DEBUG_LIST_CALCULATED_POINT
               MessageInterface::ShowMessage("--- Excluding default Calculated Point objects .....\n");
            #endif
            for (UnsignedInt i=0; i<calptList.size(); i++)
            {
               if (calptList[i] != GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME)
                  tempObjectNames.push_back(calptList[i]);
            }
         }
         else
         {
            #ifdef DEBUG_LIST_CALCULATED_POINT
               MessageInterface::ShowMessage("--- NOT Excluding default Calculated Point objects .....\n");
            #endif
            for (UnsignedInt i=0; i<calptList.size(); i++)
               tempObjectNames.push_back(calptList[i]);
         }
         
         StringArray osptList =
            theConfigManager->GetListOfItems(Gmat::SPACE_POINT);
         for (UnsignedInt i=0; i<osptList.size(); i++)
         {
            // do not add the same object name
            if (find(tempObjectNames.begin(), tempObjectNames.end(), osptList[i])
                == tempObjectNames.end())
               tempObjectNames.push_back(osptList[i]);
         }
      }
      
      return tempObjectNames;
   }
   
   // Do not add default coordinate systems on option
   if (type == Gmat::COORDINATE_SYSTEM && excludeDefaultObjects)
   {
      tempObjectNames.clear();
      StringArray csObjNames = theConfigManager->GetListOfItems(type);
      for (UnsignedInt i=0; i<csObjNames.size(); i++)
         if (csObjNames[i] != "EarthMJ2000Eq" && csObjNames[i] != "EarthMJ2000Ec" &&
             csObjNames[i] != "EarthFixed" && csObjNames[i] != "EarthICRF")
            tempObjectNames.push_back(csObjNames[i]);
      return tempObjectNames;
   }
   
   // Do not add default barycenter on option
   if (type == Gmat::CALCULATED_POINT && excludeDefaultObjects)
   {
      tempObjectNames.clear();
      StringArray cpNames = theConfigManager->GetListOfItems(type);
      for (UnsignedInt i=0; i<cpNames.size(); i++)
         if (cpNames[i] != GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME)
            tempObjectNames.push_back(cpNames[i]);
      return tempObjectNames;
   }

   return theConfigManager->GetListOfItems(type, typemax);
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfObjects(const std::string &typeName,
//                                     bool excludeDefaultObjects)
//------------------------------------------------------------------------------
/**
 * Returns names of all configured items of object type name
 *
 * @param <typeName> object type name
 * @param <excludeDefaultObjects> set this flag to true if default objects
 *           should be execluded, such as  default coordinate systems
 *
 * @return array of configured item names of the type; return empty array if none
 *  return all configured item if type is UNKNOWN_OBJECT
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetListOfObjects(const std::string &typeName,
                                               bool excludeDefaultObjects)
{
   if (typeName == "UnknownObject")
      return theConfigManager->GetListOfAllItems();
   
   if (typeName == "CelestialBody" || typeName == "SpacePoint")
   {
      tempObjectNames.clear();
      
      if (theSolarSystemInUse == NULL)
         return tempObjectNames;
      
      if (typeName == "CelestialBody")
      {
         // add bodies to the list
         tempObjectNames = theSolarSystemInUse->GetBodiesInUse();
      }
      else if (typeName == "SpacePoint")
      {
         // add Spacecraft to the list
         tempObjectNames = theConfigManager->GetListOfItems(Gmat::SPACECRAFT);
         
         // add bodies to the list
         StringArray bodyList = theSolarSystemInUse->GetBodiesInUse();
         for (UnsignedInt i=0; i<bodyList.size(); i++)
            tempObjectNames.push_back(bodyList[i]);
         
         // add CalculatedPoint to the list
         StringArray calptList =
            theConfigManager->GetListOfItems(Gmat::CALCULATED_POINT);
         if (excludeDefaultObjects)
         {
            #ifdef DEBUG_LIST_CALCULATED_POINT
               MessageInterface::ShowMessage("--- Excluding default Calculated Point objects .....\n");
            #endif
            for (UnsignedInt i=0; i<calptList.size(); i++)
            {
               if (calptList[i] != GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME)
                  tempObjectNames.push_back(calptList[i]);
            }
         }
         else
         {
            #ifdef DEBUG_LIST_CALCULATED_POINT
               MessageInterface::ShowMessage("--- NOT Excluding default Calculated Point objects .....\n");
            #endif
            for (UnsignedInt i=0; i<calptList.size(); i++)
               tempObjectNames.push_back(calptList[i]);
         }
         for (UnsignedInt i=0; i<calptList.size(); i++)
            tempObjectNames.push_back(calptList[i]);
         
         StringArray osptList =
            theConfigManager->GetListOfItems(Gmat::SPACE_POINT);
         for (UnsignedInt i=0; i<osptList.size(); i++)
            tempObjectNames.push_back(osptList[i]);
      }
      
      return tempObjectNames;
   }
   
   // Do not add default coordinate systems on option
   if (typeName == "CoordinateSystem" && excludeDefaultObjects)
   {
      tempObjectNames.clear();
      StringArray csObjNames = theConfigManager->GetListOfItems(typeName);
      for (UnsignedInt i=0; i<csObjNames.size(); i++)
         if (csObjNames[i] != "EarthMJ2000Eq" && csObjNames[i] != "EarthMJ2000Ec" &&
             csObjNames[i] != "EarthFixed" && csObjNames[i] != "EarthICRF")
            tempObjectNames.push_back(csObjNames[i]);
      return tempObjectNames;
   }
   // Do not add default barycenter on option
   if (typeName == "CalculatedPoint" && excludeDefaultObjects)
   {
      tempObjectNames.clear();
      StringArray cpNames = theConfigManager->GetListOfItems(typeName);
      for (UnsignedInt i=0; i<cpNames.size(); i++)
         if (cpNames[i] != GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME)
            tempObjectNames.push_back(cpNames[i]);
      return tempObjectNames;
   }
   
   return theConfigManager->GetListOfItems(typeName);
}


//------------------------------------------------------------------------------
// GmatBase* GetConfiguredObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* Moderator::GetConfiguredObject(const std::string &name)
{
   #ifdef DEBUG_CONFIG
   MessageInterface::ShowMessage
      ("Moderator::GetConfiguredObject() entered: name=%s\n", name.c_str());
   #endif
   
   std::string newName = name;
   
   // check for SolarSystem first until SolarSystem can be configured(LOJ: 2009.02.19)
   if (name == "SolarSystem" || name == "Solar System")
      return theSolarSystemInUse;
   
   // Ignore array indexing of Array
   std::string::size_type index = name.find_first_of("([");
   if (index != name.npos)
   {
      newName = name.substr(0, index);
      
      #ifdef DEBUG_CONFIG
      MessageInterface::ShowMessage
         ("Moderator::GetConfiguredObject() entered: newName=%s\n", newName.c_str());
      #endif
   }
   
   GmatBase *obj = theConfigManager->GetItem(newName);
   
   if (obj == NULL)
   {
      #if DEBUG_CONFIG
      MessageInterface::ShowMessage
         ("   Trying SolarSystem, theSolarSystemInUse=<%p>\n", theSolarSystemInUse);
      #endif
      
      // try SolarSystem
      if (theSolarSystemInUse)
         obj = (GmatBase*)(theSolarSystemInUse->GetBody(newName));
   }
   
   #ifdef DEBUG_CONFIG
   if (obj)
   {
      MessageInterface::ShowMessage
         ("Moderator::GetConfiguredObject() Found object: name=%s, type=%s, "
          "addr=%p\n", obj->GetName().c_str(), obj->GetTypeName().c_str(), obj);
   }
   else
   {
      MessageInterface::ShowMessage
         ("Moderator::GetConfiguredObject() Cannot find object: name=%s\n",
          newName.c_str());
   }
   #endif
   
   return obj;
}


//------------------------------------------------------------------------------
// bool ReconfigureItem(GmatBase *newobj, const std::string &name)
//------------------------------------------------------------------------------
/*
 * Sets configured object pointer with new pointer.
 *
 * @param  newobj  New object pointer
 * @param  name  Name of the configured object to be reset
 *
 * @return  true if pointer was reset, false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::ReconfigureItem(GmatBase *newobj, const std::string &name)
{
   // Reconfigure item only if name found in the configuration.
   // Changed due to GmatFunction implementation (loj: 2008.06.25)
   if (GetConfiguredObject(name))
      return theConfigManager->ReconfigureItem(newobj, name);
   else
      return true;
}


//------------------------------------------------------------------------------
// std::string GetNewName(const std::string &name, Integer startCount)
//------------------------------------------------------------------------------
/*
 * It gives new name by adding counter to the input name.
 *
 * @param <name> Base name to used to generate new name
 * @param <startCount> Starting counter
 * @return new name
 */
//------------------------------------------------------------------------------
std::string Moderator::GetNewName(const std::string &name, Integer startCount)
{
   if (name == "")
      return "";
   
   return theConfigManager->GetNewName(name, startCount);
}


//------------------------------------------------------------------------------
// GmatBase* AddClone(const std::string &name, std::string &cloneName)
//------------------------------------------------------------------------------
/*
 * Adds the clone of the named object to configuration.
 * It gives new name by adding counter to the name to be cloned.
 *
 * @parameter  name  The name of the object to be cloned
 * @parameter  cloneName  Name of the cloned object if object was cloned
 *                and added to configuration, blank otherwise
 *
 * @return  Cloned object pointer, NULL if it was not cloned
 */
//------------------------------------------------------------------------------
GmatBase* Moderator::AddClone(const std::string &name, std::string &cloneName)
{
   if (name == "")
      return NULL;
   
   return theConfigManager->AddClone(name, cloneName);
}


//------------------------------------------------------------------------------
// bool RenameObject(UnsignedInt type, const std::string &oldName
//                           const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames configured item
 *
 * @param <type> object type
 * @param <oldName>  old object name
 * @param <newName>  new object name
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::RenameObject(UnsignedInt type, const std::string &oldName,
                                     const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Moderator::RenameObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   // let's check to make sure it is a valid name
   if (!GmatStringUtil::IsValidName(newName, true))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "'%s' is not a valid object name.\nPlease enter a different name.\n",
         newName.c_str());
      return false;
   }
   
   // check to make sure it is not a command type
   StringArray commandNames = GetListOfFactoryItems(Gmat::COMMAND);
   for (Integer i=0; i<(Integer)commandNames.size(); i++)
   {
      if (commandNames[i] == newName)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "'%s' is not a valid object name.\nPlease enter a different name.\n",
            newName.c_str());
         return false;
      }
   }
   
   #if DEBUG_RENAME
   MessageInterface::ShowMessage("   Calling theConfigManager->RenameItem()\n");
   #endif
   
   bool renamed = false;
   try
   {
      renamed = theConfigManager->RenameItem(type, oldName, newName);
   }
   catch (BaseException &be)
   {
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("Moderator::RenameObject() caught exception:\n%s\n", be.GetFullMessage().c_str());
      #endif
      return false;
   }
   
   std::vector<UnsignedInt> relatedItemType;
   StringArray relatedOldName;
   StringArray relatedNewName;
   bool hasRelatedChange = theConfigManager->RelatedNameChange(relatedItemType,
         relatedOldName, relatedNewName);

   //--------------------------------------------------
   // rename object name used in mission sequence
   //--------------------------------------------------
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Moderator::RenameObject() ===> Change Command ref object names\n");
//   MessageInterface::ShowMessage
//      ("Moderator::RenameObject() ===> %s related name change from %s to %s\n",
//       (hasRelatedChange ? "Found" : "Has no"), relatedOldName.c_str(),
//       relatedNewName.c_str());
   #endif
   
   int sandboxIndex = 0; //handles one sandbox for now
   GmatCommand *cmd = commands[sandboxIndex]->GetNext();
   GmatCommand *child;
   std::string typeName;
   
   while (renamed && cmd != NULL)
   {
      typeName = cmd->GetTypeName();
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("...typeName=%12s, '%s'\n", typeName.c_str(),
          cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      #endif
      
      renamed = cmd->RenameRefObject(type, oldName, newName);
      if (hasRelatedChange)
         for (UnsignedInt i = 0; i < relatedItemType.size(); ++i)
            cmd->RenameRefObject(relatedItemType[i], relatedOldName[i],
                  relatedNewName[i]);
      if (!renamed)
         MessageInterface::ShowMessage
            ("Moderator failed to rename rename '%s' to '%s' in %s command\n",
             oldName.c_str(), newName.c_str(), typeName.c_str());
      
      child = cmd->GetChildCommand(0);
      
      while (renamed && (child != NULL) && (child != cmd))
      {
         typeName = child->GetTypeName();
         #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("......typeName=%12s, '%s'\n", typeName.c_str(),
             cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         #endif
         if (typeName.find("End") == typeName.npos)
         {
            renamed = child->RenameRefObject(type, oldName, newName);
            if (hasRelatedChange)
               for (UnsignedInt i = 0; i < relatedItemType.size(); ++i)
                  child->RenameRefObject(relatedItemType[i], relatedOldName[i],
                        relatedNewName[i]);

            if (!renamed)
               MessageInterface::ShowMessage
                  ("Moderator failed to rename rename '%s' to '%s' in %s command\n",
                   oldName.c_str(), newName.c_str(), typeName.c_str());
         }
         
         child = child->GetNext();
      }
      
      cmd = cmd->GetNext();
   }
   
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Moderator::RenameObject() rename status=%d\n", renamed);
   #endif

   return renamed;
}

//------------------------------------------------------------------------------
// bool RemoveObject(UnsignedInt type, const std::string &name,
//                           bool delIfNotUsed)
//------------------------------------------------------------------------------
/**
 * Removes item from the configured list.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <delIfNotUse> flag indicating delete if item is not used in the mission
 *                      sequence
 *
 * @return true if the item has been removed; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::RemoveObject(UnsignedInt type, const std::string &name,
                             bool delOnlyIfNotUsed)
{
   GmatCommand *cmd = GetFirstCommand();
   
   #if DEBUG_REMOVE
   MessageInterface::ShowMessage
      ("Moderator::RemoveObject() type=%d, name=%s, delOnlyIfNotUsed=%d\n",
       type, name.c_str(), delOnlyIfNotUsed);
   MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
   #endif
   
   if (!delOnlyIfNotUsed)
   {
      #if DEBUG_REMOVE
      MessageInterface::ShowMessage("   No need to check for items using, deleting %s\n", name.c_str());
      #endif
      return theConfigManager->RemoveItem(type, name);
   }
   else
   {
      // remove if object is not used in other resource
      #if DEBUG_REMOVE
      MessageInterface::ShowMessage("   Checking if '%s' is used in resource\n", name.c_str());
      #endif
      GmatBase *obj = theConfigManager->GetFirstItemUsing(type, name, false);
      if (obj != NULL)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** Cannot remove \"%s.\"  It is used in the %s "
             "object named \"%s\"\n", name.c_str(), obj->GetTypeName().c_str(),
             obj->GetName().c_str());
         return false;
      }
      else
      {
         #if DEBUG_REMOVE
         MessageInterface::ShowMessage
            ("   '%s' is not used in resource, checking command %s\n", name.c_str(), (cmd->GetName()).c_str());
         #endif
         // remove if object is not used in the command sequence
         std::string cmdName;
         GmatCommand *cmdUsing = NULL;
         if (GmatCommandUtil::FindObject(cmd, type, name, cmdName, &cmdUsing, true))
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** Cannot remove \"%s.\"  It is used in the %s "
                "command: '%s'\n",  name.c_str(), cmdName.c_str(),
                cmdUsing ? cmdUsing->GetGeneratingString(Gmat::NO_COMMENTS).c_str() : "");
            return false;
         }
         else
         {
            bool retval = theConfigManager->RemoveItem(type, name, true);
            #if DEBUG_REMOVE
            MessageInterface::ShowMessage
               ("Moderator::RemoveObject() It is not used in the sequence, "
                "returning %d from theConfigManager->RemoveItem()\n", retval);
            #endif
            return retval;
         }
      }
   }
}


//------------------------------------------------------------------------------
// bool HasConfigurationChanged(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
bool Moderator::HasConfigurationChanged(Integer sandboxNum)
{
   bool resourceChanged = theConfigManager->HasConfigurationChanged();
   bool commandsChanged = GmatCommandUtil::HasCommandSequenceChanged(commands[sandboxNum-1]);
   
   #if DEBUG_CONFIG_CHANGE
   MessageInterface::ShowMessage
      ("Moderator::HasConfigurationChanged() resourceChanged=%d, "
       "commandsChanged=%d\n", resourceChanged, commandsChanged);
   #endif
   
   return (resourceChanged || commandsChanged);
}


//------------------------------------------------------------------------------
// void ConfigurationChanged(GmatBase *obj, bool tf)
//------------------------------------------------------------------------------
void Moderator::ConfigurationChanged(GmatBase *obj, bool tf)
{
   #if DEBUG_CONFIG_CHANGE
   MessageInterface::ShowMessage
      ("Moderator::ConfigurationChanged() obj type=%s, name='%s', changed=%d\n",
       obj->GetTypeName().c_str(), obj->GetName().c_str(), tf);
   #endif
   
   if (obj != NULL)
   {
      if (obj->IsOfType(Gmat::COMMAND))
         ((GmatCommand*)obj)->ConfigurationChanged(true);
      else
         theConfigManager->ConfigurationChanged(true);
   }
}


//------------------------------------------------------------------------------
// void ResetConfigurationChanged(bool resetResource = true,
//                                bool resetCommands = true,
//                                Integer sandboxNum = 1)
//------------------------------------------------------------------------------
void Moderator::ResetConfigurationChanged(bool resetResource, bool resetCommands,
                                          Integer sandboxNum)
{
   #if DEBUG_CONFIG_CHANGE
   MessageInterface::ShowMessage
      ("Moderator::ResetConfigurationChanged() entered\n");
   #endif
   
   if (resetResource)
      theConfigManager->ConfigurationChanged(false);
   
   if (resetCommands)
      SetCommandsUnchanged(sandboxNum-1);
}

// SolarSystem
//------------------------------------------------------------------------------
// SolarSystem* GetDefaultSolarSystem()
//------------------------------------------------------------------------------
/**
 * Retrieves a default solar system object pointer.
 *
 * @return a default solar system object pointer
 */
//------------------------------------------------------------------------------
SolarSystem* Moderator::GetDefaultSolarSystem()
{
   return theConfigManager->GetDefaultSolarSystem();
}


//------------------------------------------------------------------------------
// SolarSystem* CreateSolarSystem(const std::string &name)
//------------------------------------------------------------------------------
SolarSystem* Moderator::CreateSolarSystem(const std::string &name)
{
   #if DEBUG_SOLAR_SYSTEM
   MessageInterface::ShowMessage
      ("Moderator::CreateSolarSystem() creating '%s'\n", name.c_str());
   #endif
   
   // There is no factory to create SolarSystem so just create by new
   // SolarSystem constructor Creates available planetary ephem sorce list.
   // Also set to DE405 as a default planetary ephem source
   SolarSystem *ss = new SolarSystem(name);
   
   #ifdef DEBUG_MEMORY
   std::string funcName;
   funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
   MemoryTracker::Instance()->Add
      (ss, name, "Moderator::CreateSolarSystem()", funcName);
   #endif
   
   #if DEBUG_SOLAR_SYSTEM
   MessageInterface::ShowMessage("Moderator::CreateSolarSystem() returning %p\n", ss);
   #endif
   
   return ss;
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse(Integer manage = 1)
//------------------------------------------------------------------------------
/*
 * Returns SolarSystem in use from configuration or object map in use
 *
 * @param  manage  If value is 1 it will return SolarSystem from the configuration
 *                 if value is 2 it will return SolarSystem from the object map in use
 */
//------------------------------------------------------------------------------
SolarSystem* Moderator::GetSolarSystemInUse(Integer manage)
{
   #if DEBUG_SOLAR_SYSTEM_IN_USE
   MessageInterface::ShowMessage
      ("Moderator::GetSolarSystemInUse() entered, manage=%d, objectMapInUse=<%p>, "
       "theInternalSolarSystem=<%p>\n", manage, objectMapInUse, theInternalSolarSystem);
   #endif
   
   SolarSystem *ss = NULL;
   if (manage == 1)
   {
      ss = theConfigManager->GetSolarSystemInUse();
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage("   Using SolarSystem from configuration <%p>\n", ss);
      #endif
   }
   else
   {
      ObjectMap::iterator pos = objectMapInUse->find("SolarSystem");
      if (pos != objectMapInUse->end())
      {
         ss = (SolarSystem*)pos->second;
         #if DEBUG_SOLAR_SYSTEM_IN_USE
         MessageInterface::ShowMessage("   Using SolarSystem from objectMapInUse <%p>\n", ss);
         #endif
      }
      
      if (ss == NULL)
      {
         ss = theInternalSolarSystem;
         #if DEBUG_SOLAR_SYSTEM_IN_USE
         MessageInterface::ShowMessage("   Using Internal SolarSystem <%p>\n", ss);
         #endif
      }      
   }
   
   // if SolarSystem is NULL, there is some problem
   if (ss == NULL)
      throw GmatBaseException
         ("Moderator::GetSolarSystemInUse() The SolarSystem in use is UNSET.\n");
   
   #if DEBUG_SOLAR_SYSTEM_IN_USE
   MessageInterface::ShowMessage
      ("Moderator::GetSolarSystemInUse() returning <%p>\n", ss);
   #endif
   
   return ss;
}


//------------------------------------------------------------------------------
// void SetSolarSystemInUse(SolarSystem *ss)
//------------------------------------------------------------------------------
void Moderator::SetSolarSystemInUse(SolarSystem *ss)
{
   if (ss != NULL)
      theConfigManager->SetSolarSystemInUse(ss);
   else
      throw GmatBaseException
         ("Moderator::SetSolarSystemInUse() cannot set NULL SolarSystem\n");
}


//------------------------------------------------------------------------------
// void SetInternalSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/*
 * Sets the internal solar system. The internal solar system is initially set to
 * theSolarSystemInUse for creating main objects and commands. When creating
 * objects and commands for GmatFunction, it will use the solar system cloned in
 * the Sandbox during the Sandbox initialization. Setting this internal solar
 * system happens during the GmatFunction initialization.
 */
//------------------------------------------------------------------------------
void Moderator::SetInternalSolarSystem(SolarSystem *ss)
{
   if (ss != NULL)
   {
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage
         ("Moderator::SetInternalSolarSystem() entered, ss=<%p>'%s'\n",
          ss, ss->GetName().c_str());
      #endif
      
      theInternalSolarSystem = ss;
   }
}


//------------------------------------------------------------------------------
// bool SetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
bool Moderator::SetSolarSystemInUse(const std::string &name)
{
   return theConfigManager->SetSolarSystemInUse(name);
}


// Create object
//------------------------------------------------------------------------------
// GmatBase* CreateObject(UnsignedInt objTypeId, const std::string &type,
//                        const std::string &name, bool createDefault)
//------------------------------------------------------------------------------
GmatBase* Moderator::CreateObject(UnsignedInt objTypeId, const std::string &type,
                                  const std::string &name, bool createDefault)
{
   #if DEBUG_CREATE_OBJECT
   MessageInterface::ShowMessage
      ("Moderator::CreateObject() objTypeId=%d<%s>, type='%s', name='%s', createDefault=%d, "
       "objectManageOption=%d\n", objTypeId, GetObjectTypeString(objTypeId).c_str(),
       type.c_str(), name.c_str(), createDefault, objectManageOption);
   #endif
   
   // Call specific medthods for setting ref objects, eop file, creating a container,
   // creating non-configurable object, or display deprecated messages
   if (objTypeId == Gmat::SPACECRAFT)
      return CreateSpacecraft(type, name, createDefault);
   else if (objTypeId == Gmat::AXIS_SYSTEM)
      return CreateAxisSystem(type, name, objectManageOption);
   else if (objTypeId == Gmat::CALCULATED_POINT)
      return CreateCalculatedPoint(type, name, true);
   else if (objTypeId == Gmat::CELESTIAL_BODY)
      return CreateCelestialBody(type, name);
   else if (objTypeId == Gmat::COORDINATE_SYSTEM)
      return CreateCoordinateSystem(name, createDefault, false, objectManageOption);
   // For displaying Hardware deprecated message.
   // When we clean up deprecated fields this special creation can be removed
   else if (objTypeId == Gmat::EVENT_LOCATOR)  
      return CreateEventLocator(type, name, createDefault);

	else if (objTypeId == Gmat::PLATE)
		return CreatePlate(type, name);

	else if (objTypeId == Gmat::HARDWARE)
      return CreateHardware(type, name);
   else if (objTypeId == Gmat::FIELD_OF_VIEW)
      return CreateFieldOfView(type, name);
   else if (objTypeId == Gmat::FUNCTION)
      return CreateFunction(type, name, objectManageOption);
   else if (objTypeId == Gmat::ODE_MODEL)
      return CreateODEModel(type, name);
   else if (objTypeId == Gmat::PHYSICAL_MODEL)
      return CreatePhysicalModel(type, name);
   else if (objTypeId == Gmat::PROPAGATOR) // Propagators are non-configurable
      return CreatePropagator(type, name);
   else if (objTypeId == Gmat::PROP_SETUP)
      return CreatePropSetup(name);
   
   
   GmatBase *obj = FindObject(name);
   if (obj == NULL)
   {      
      obj = theFactoryManager->CreateObject(objTypeId, type, name);
      
      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create an object of type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateObject()", funcName);
      }
      #endif
      
      // Add object to object map in use
      AddObjectToObjectMapInUse(name, obj, objTypeId);
      
      // @todo Set defaults if creating default object
      
      
      #if DEBUG_CREATE_OBJECT
      MessageInterface::ShowMessage
         ("Moderator::CreateObject() returning <%p>\n", obj);
      #endif
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_OBJECT
      MessageInterface::ShowMessage
         ("Moderator::CreateObject() Unable to create an object. "
          "<%p><%s>'%s' already exist\n", obj,
          obj->GetTypeName().c_str(), obj->GetName().c_str());
      #endif
      // Return obj if same type
      if (obj->GetTypeName() == type)
         return obj;
      else
         return NULL;
   }
}


//------------------------------------------------------------------------------
// GmatBase* CreateOtherObject(UnsignedInt objTypeId, const std::string &type,
//                             const std::string &name, bool createDefault)
//------------------------------------------------------------------------------
GmatBase* Moderator::CreateOtherObject(UnsignedInt objTypeId, const std::string &type,
                                       const std::string &name, bool createDefault)
{
   #if DEBUG_CREATE_OTHER_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateOtherObject() objTypeId=%d, type='%s', name='%s', createDefault=%d, "
       "objectManageOption=%d\n", objTypeId, type.c_str(), name.c_str(), createDefault,
       objectManageOption);
   #endif
   
   GmatBase *obj = NULL;
   if (FindObject(name) == NULL)
   {
      obj = theFactoryManager->CreateObject(objTypeId, type, name);
      
      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create an object of type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateOtherObject()", funcName);
      }
      #endif
      
      // Add object to object map in use
      AddObjectToObjectMapInUse(name, obj, objTypeId);
            
      #if DEBUG_CREATE_OTHER_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateOtherObject() returning <%p>\n", obj);
      #endif
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_OTHER_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateOtherObject() Unable to create an object "
          "name: %s already exist\n", name.c_str());
      #endif
      return FindObject(name);
   }
}


// CalculatedPoint
//------------------------------------------------------------------------------
// CalculatedPoint* CreateCalculatedPoint(const std::string &type,
//                                        const std::string &name,
//                                        bool addDefaultBodies = true)
//------------------------------------------------------------------------------
/**
 * Creates a calculated point object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <addDefaultBodies> true if add default bodies requested (true)
 *
 * @return a CalculatedPoint object pointer
 */
//------------------------------------------------------------------------------
CalculatedPoint* Moderator::CreateCalculatedPoint(const std::string &type,
                                                  const std::string &name,
                                                  bool addDefaultBodies)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateCalculatedPoint() type='%s', name='%s', "
       "addDefaultBodies=%d, objectManageOption=%d\n", type.c_str(), name.c_str(),
       addDefaultBodies, objectManageOption);
   #endif
   
   if (GetCalculatedPoint(name) == NULL)
   {
      #ifdef DEBUG_CREATE_CALC_POINT
         MessageInterface::ShowMessage("Moderator::Creating new %s named %s\n",
               type.c_str(), name.c_str());
      #endif
      CalculatedPoint *obj = theFactoryManager->CreateCalculatedPoint(type, name);
      
      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create a CalculatedPoint type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateCalculatedPoint()", funcName);
      }
      #endif
      
      // add default bodies
      if (type == "LibrationPoint")
      {
         if (addDefaultBodies)
         {
            obj->SetStringParameter("Primary", "Sun");
            obj->SetStringParameter("Point", "L1");
            obj->SetStringParameter("Secondary", "Earth");
            
            #ifdef __CREATE_DEFAULT_BC__
               // first create default Earth-Moon Barycenter
               CalculatedPoint *defBc = GetCalculatedPoint("DefaultBC");

               if (defBc == NULL)
               {
                  #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
                  MessageInterface::ShowMessage
                     ("Moderator::CreateCalculatedPoint() Calling CreateCalculatedPoint()\n");
                  #endif
                  defBc = CreateCalculatedPoint("Barycenter", "DefaultBC");
               }
               obj->SetStringParameter("Secondary", "DefaultBC");
               obj->SetRefObject(defBc, Gmat::SPACE_POINT, "DefaultBC");
            #endif
            
            // Set body and J2000Body pointer, so that GUI can create LibrationPoint
            // and use it in Coord.System conversion
            SpacePoint *sun = (SpacePoint*)FindObject("Sun");
            SpacePoint *earth = (SpacePoint*)FindObject("Earth");
            
            if (sun->GetJ2000Body() == NULL)
               sun->SetJ2000Body(earth);
            
            #ifdef DEBUG_CREATE_CALC_POINT
            MessageInterface::ShowMessage
               ("Moderator::Setting sun <%p> and earth <%p> to LibrationPoint %s\n", sun, earth, name.c_str());
            #endif
            
            obj->SetRefObject(sun, Gmat::SPACE_POINT, "Sun");
            obj->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");
         }
      }
      else if (type == "Barycenter")
      {
         if (addDefaultBodies)
         {
//            obj->SetStringParameter("BodyNames", "Earth");
//            obj->SetStringParameter("BodyNames", "Luna");

            ((CalculatedPoint*) obj)->SetDefaultBody("Earth");
            ((CalculatedPoint*) obj)->SetDefaultBody("Luna");

            // Set body and J2000Body pointer, so that GUI can create Barycenter
            // and use it in Coord.System conversion
            SpacePoint *earth = (SpacePoint*)FindObject("Earth");
            obj->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");

            SpacePoint *luna = (SpacePoint*)FindObject("Luna");
            if (luna->GetJ2000Body() == NULL)
                luna->SetJ2000Body(earth);
            obj->SetRefObject(luna, Gmat::SPACE_POINT, "Luna");
         }
      }
      
      // Manage it if it is a named CalculatedPoint
      try
      {
         if (name != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddCalculatedPoint(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateCalculatedPoint()\n" +
                                       e.GetFullMessage());
      }
         
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateCalculatedPoint() Unable to create CalculatedPoint "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetCalculatedPoint(name);
   }
}


//------------------------------------------------------------------------------
// CalculatedPoint* GetCalculatedPoint(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a calclulated point object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a CalculatedPoint object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
CalculatedPoint* Moderator::GetCalculatedPoint(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (CalculatedPoint*)FindObject(name);
}


// CelestialBody
//------------------------------------------------------------------------------
// CelestialBody* CreateCelestialBody(const std::string &type,
//                                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a celestial body object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a CelestialBody object pointer
 */
//------------------------------------------------------------------------------
CelestialBody* Moderator::CreateCelestialBody(const std::string &type,
                                              const std::string &name)
{
   #ifdef DEBUG_CREATE_BODY
   MessageInterface::ShowMessage
      ("Moderator::CreateCelestialBody() called with type = '%s' and name = '%s'\n",
       type.c_str(), name.c_str());
   if (GetCelestialBody(name) != NULL) 
      MessageInterface::ShowMessage("... that body alreday exists\n");
   #endif
   if (GetCelestialBody(name) == NULL)
   {
      CelestialBody *obj = theFactoryManager->CreateCelestialBody(type, name);
      
      if (obj == NULL)
         throw GmatBaseException
            ("The Moderator cannot create a CelestialBody type \"" + type + "\"\n");
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateCelestialBody()", funcName);
      }
      #endif
      
      // Add new celestial body to solar syatem in use
      Integer manage = 0; // Just set other than 1 here
      SolarSystem *ss = GetSolarSystemInUse(manage);
      obj->SetUserDefined(true);
      obj->SetSolarSystem(ss);
      obj->SetUpBody(); //Added so that it works inside a GmatFunction (LOJ: 2010.04.30)
      ss->AddBody(obj);
      #ifdef DEBUG_CREATE_BODY
      MessageInterface::ShowMessage
         ("Moderator::CreateCelestialBody() Created CelestialBody <%p> "
          "\"%s\" and added to Solar System <%p>\n", obj, name.c_str(), ss);
      #endif
      
      // Manually set configuration changed to true here since
      // SolarSystem is not configured yet
      if (objectManageOption == 1)
         theConfigManager->ConfigurationChanged(true);
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateCelestialBody() Unable to create CelestialBody "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetCelestialBody(name);
   }
}


//------------------------------------------------------------------------------
// CelestialBody* GetCelestialBody(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a celestial body object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a CelestialBody object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
CelestialBody* Moderator::GetCelestialBody(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (CelestialBody*)FindObject(name);
}


// Spacecraft
//------------------------------------------------------------------------------
// SpaceObject* CreateSpacecraft(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a spacecraft object by given name.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <createDefault> set this to true if default spacecraft to be created
 *
 * @return spacecraft object pointer
 */
//------------------------------------------------------------------------------
SpaceObject* Moderator::CreateSpacecraft(const std::string &type,
                                         const std::string &name,
                                         bool createDefault)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateSpacecraft() entered, type = '%s', name = '%s', "
       "createDefault = %d, objectManageOption = %d\n",
       type.c_str(), name.c_str(), createDefault, objectManageOption);
   #endif
   
   if (GetSpacecraft(name) == NULL)
   {
      Spacecraft *obj = (Spacecraft*)(theFactoryManager->CreateSpacecraft(type, name));
      
      if (obj == NULL)
         throw GmatBaseException
            ("The Moderator cannot create a Spacecraft type \"" + type + "\"\n");
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateSpacecraft()", funcName);
      }
      #endif
      
      // Create internal and default CoordinateSystems if they do not exist.
      // This code will make new mission to work after script errors occur.
      // This change was made while looking at Bug 1532 (LOJ: 2009.11.13)
      if (theInternalCoordSystem == NULL)
         CreateInternalCoordSystem();
      
      #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSpacecraft() Calling CreateDefaultCoordSystems() and "
          "CreateDefaultBarycenter()\n");
      #endif
      
      // Create the default coordinate systems and solar system barycenter
      CreateDefaultCoordSystems();
      CreateDefaultBarycenter();
      
      if (type == "Spacecraft")
      {
         // Set internal and default CoordinateSystem
         obj->SetInternalCoordSystem(theInternalCoordSystem);
         obj->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
         // Set coordinate system pointer if creating default spacecraft probably
         // from the GUI. We don't want to go through state conversion if creating
         // from the script. This will fix bug 2526 (LOJ: 2012.01.24)
         if (createDefault)
            obj->SetRefObject(GetCoordinateSystem("EarthMJ2000Eq"),
                              Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
      }
      
      // Manage it if it is a named Spacecraft
      try
      {
         if (name != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddSpacecraft(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateSpacecraft()\n" +
                                       e.GetFullMessage());
      }
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSpacecraft() returning <%p>'%s'\n", obj,
          obj->GetName().c_str());
      #endif
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSpacecraft() Unable to create Spacecraft "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetSpacecraft(name);
   }
}


//------------------------------------------------------------------------------
// SpaceObject* GetSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a spacecraft object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a SpaceObject object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
SpaceObject* Moderator::GetSpacecraft(const std::string &name)
{
   if (name == "")
      return NULL;
   else      
      return (SpaceObject*)FindObject(name);
}


//------------------------------------------------------------------------------
// std::string GetSpacecraftNotInFormation()
//------------------------------------------------------------------------------
/**
 * This method finds the first spacecraft name sorted by ascending order not
 * in any formation.
 *
 * @return  The first spacecraft name not in any formation.
 *          return "" if such spacecraft is not found.
 */
//------------------------------------------------------------------------------
std::string Moderator::GetSpacecraftNotInFormation()
{
   StringArray scList = GetListOfObjects(Gmat::SPACECRAFT);
   StringArray fmList = GetListOfObjects(Gmat::FORMATION);
   int numSc = scList.size(), numFm = fmList.size();
   
   if (numSc == 0 && numFm == 0)
      return "";
   
   if (numSc > 0 && numFm == 0)
      return GetDefaultSpacecraft()->GetName();
   
   // formation exists
   StringArray fmscListAll;
   
   //------------------------------------------
   // Merge spacecrafts in Formation
   //------------------------------------------
   for (int i=0; i<numFm; i++)
   {
      GmatBase *fm = GetConfiguredObject(fmList[i]);
      StringArray fmscList = fm->GetStringArrayParameter(fm->GetParameterID("Add"));
      fmscListAll.insert(fmscListAll.begin(), fmscList.begin(), fmscList.end());
   }
   
   // sort the lists in order to  set_difference()
   sort(scList.begin(), scList.end());
   sort(fmscListAll.begin(), fmscListAll.end());
   
   //------------------------------------------
   // Make list of spacecrafts not in Formation
   //------------------------------------------
   StringArray scsNotInForms;
   
   // The set_difference() algorithm produces a sequence that contains the
   // difference between the two ordered sets.
   set_difference(scList.begin(), scList.end(), fmscListAll.begin(),
                  fmscListAll.end(), back_inserter(scsNotInForms));
   
   if (scsNotInForms.size() > 0)
      return scsNotInForms[0];
   else
      return "";
}


// SpacePoints
//------------------------------------------------------------------------------
// SpacePoint* CreateSpacePoint(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a spacepoint object by given name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return SpacePoint object pointer
 */
//------------------------------------------------------------------------------
SpacePoint* Moderator::CreateSpacePoint(const std::string &type,
                                        const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateSpacePoint() type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   if (GetSpacePoint(name) == NULL)
   {
      SpacePoint *obj = (SpacePoint*)(theFactoryManager->CreateSpacePoint(type, name));
      
      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create a SpacePoint type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateSpacePoint()", funcName);
      }
      #endif
            
      // Manage it if it is a named SpacePoint
      try
      {
         if (name != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddSpacePoint(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateSpacePoint()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSpacePoint() Unable to create SpacePoint "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetSpacePoint(name);
   }
}

//------------------------------------------------------------------------------
// SpacePoint* GetSpacePoint(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a SpacePoint object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a SpacePoint object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
SpacePoint* Moderator::GetSpacePoint(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (SpacePoint*)FindObject(name);
}


// Plates
//------------------------------------------------------------------------------
// Plate* CreatePlate(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
* Creates a Plate object by given name.
*
* @param <type> object type
* @param <name> object name
*
* @return Plate object pointer
*/
//------------------------------------------------------------------------------
Plate* Moderator::CreatePlate(const std::string &type,
	const std::string &name)
{
#if DEBUG_CREATE_RESOURCE
	MessageInterface::ShowMessage
	("Moderator::CreatePlate() type = '%s', name = '%s'\n",
		type.c_str(), name.c_str());
#endif

	if (GetPlate(name) == NULL)
	{
		Plate *obj = (Plate*)(theFactoryManager->CreatePlate(type, name));

		if (obj == NULL)
		{
			throw GmatBaseException
			("The Moderator cannot create a Plate type \"" + type + "\"\n");
		}

#ifdef DEBUG_MEMORY
		if (obj)
		{
			std::string funcName;
			funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
			MemoryTracker::Instance()->Add
			(obj, name, "Moderator::CreatePlate()", funcName);
		}
#endif

		// Manage it if it is a named Plate
		try
		{
			if (name != "")
			{
				if (objectManageOption == 1)
					theConfigManager->AddPlate(obj);
				else if (objectManageOption == 2)
					AddObject(obj);
			}
		}
		catch (BaseException &e)
		{
			MessageInterface::ShowMessage("In Moderator::CreatePlate()\n" +
				e.GetFullMessage());
		}

		return obj;
	}
	else
	{
#if DEBUG_CREATE_RESOURCE
		MessageInterface::ShowMessage
		("Moderator::CreatePlate() Unable to create Plate "
			"name: %s already exist\n", name.c_str());
#endif
		return GetPlate(name);
	}
}

//------------------------------------------------------------------------------
// Plate* GetPlate(const std::string &name)
//------------------------------------------------------------------------------
/**
* Retrieves a Plate object pointer by given name.
*
* @param <name> object name
*
* @return a Plate object pointer, return null if name not found
*/
//------------------------------------------------------------------------------
Plate* Moderator::GetPlate(const std::string &name)
{
	if (name == "")
		return NULL;
	else
		return (Plate*)FindObject(name);
}


// Hardware
//------------------------------------------------------------------------------
// Hardware* CreateHardware(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a Hardware object by given name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return Hardware object pointer
 */
//------------------------------------------------------------------------------
Hardware* Moderator::CreateHardware(const std::string &type, const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateHardware() type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   if (GetHardware(name) == NULL)
   {
      Hardware *obj = theFactoryManager->CreateHardware(type, name);
      if ((type == "Thruster") && (!thrusterDeprecateMsgWritten))
      {
         std::string warnMsg = "*** WARNING *** Type \"Thruster\" is ";
         warnMsg += "deprecated.  ";
         warnMsg += "Please use \"ChemicalThruster\" or \"ElectricThruster\" ";
         warnMsg += "instead.  A ChemicalThruster will be created.\n";
         MessageInterface::ShowMessage(warnMsg);
         thrusterDeprecateMsgWritten = true;
      }
      if ((type == "FuelTank") && (!fuelTankDeprecateMsgWritten))
      {
         std::string warnMsg = "*** WARNING *** Type \"FuelTank\" is ";
         warnMsg += "deprecated.  ";
         warnMsg += "Please use \"ChemicalTank\" or \"ElectricTank\" ";
         warnMsg += "instead.  A ChemicalTank will be created.\n";
         MessageInterface::ShowMessage(warnMsg);
         fuelTankDeprecateMsgWritten = true;
      }
      
      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create a Hardware type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateHardware()", funcName);
      }
      #endif
      
      // Manage it if it is a named Hardware
      try
      {
         if (name != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddHardware(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateHardware()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateHardware() Unable to create Hardware "
          "name: \"%s\" already exists\n", name.c_str());
      #endif
      return GetHardware(name);
   }
}


//------------------------------------------------------------------------------
// Hardware* GetHardware(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Hardware object pointer by given name and add to configuration.
 *
 * @param <name> object name
 *
 * @return a Hardware object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Hardware* Moderator::GetHardware(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (Hardware*)FindObject(name);
}

// FieldOfView
//------------------------------------------------------------------------------
// FieldOfView* CreateFieldOfView(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a FieldOfView object by given name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return Hardware object pointer
 */
//------------------------------------------------------------------------------
FieldOfView* Moderator::CreateFieldOfView(const std::string &type, const std::string &name)
{
#if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
   ("Moderator::CreateFieldOfView() type = '%s', name = '%s'\n",
    type.c_str(), name.c_str());
#endif
   
   if (GetFieldOfView(name) == NULL)
   {
      FieldOfView *obj = theFactoryManager->CreateFieldOfView(type, name);

      if (obj == NULL)
      {
         throw GmatBaseException
         ("The Moderator cannot create a Hardware type \"" + type + "\"\n");
      }
      
#ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
         (obj, name, "Moderator::CreateHardware()", funcName);
      }
#endif
      
      // Manage it if it is a named FieldOfView
      try
      {
         if (name != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddFieldOfView(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateFieldOfView()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
   }
   else
   {
#if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
      ("Moderator::CreateFieldOfView() Unable to create Hardware "
       "name: \"%s\" already exists\n", name.c_str());
#endif
      return GetFieldOfView(name);
   }
}


//------------------------------------------------------------------------------
// FieldOfView* GetFieldOfView(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Hardware object pointer by given name and add to configuration.
 *
 * @param <name> object name
 *
 * @return a Hardware object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
FieldOfView* Moderator::GetFieldOfView(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (FieldOfView*)FindObject(name);
}


// Propagator
//------------------------------------------------------------------------------
// Propagator* CreatePropagator(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a propagator object by given type and name. Actually this creates
 * Integrator.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a propagator object pointer
 */
//------------------------------------------------------------------------------
Propagator* Moderator::CreatePropagator(const std::string &type,
                                        const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreatePropagator() type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   // GMAT doesn't name propagators, so we don't need to check the configuration
   // for them.  PropSetups are the only things that get named for propagation.
   Propagator *obj = theFactoryManager->CreatePropagator(type, name);
      
   if (obj ==  NULL)
      throw GmatBaseException
         ("The Moderator cannot create a Propagator type \"" + type + "\"\n");
      
   #ifdef DEBUG_MEMORY
   if (obj)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (obj, name, "Moderator::CreatePropagator()", funcName);
   }
   #endif
      
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreatePropagator() returning new Propagator <%p>'%s'\n",
       obj, obj->GetName().c_str());
   #endif
   
   return obj;
}

//------------------------------------------------------------------------------
// Propagator* GetPropagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a propagator object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a propagator object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Propagator* Moderator::GetPropagator(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (Propagator*)FindObject(name);
}

// PhysicalModel
//------------------------------------------------------------------------------
// PhysicalModel* CreateDefaultPhysicalModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a default physical model of full Earth gravity force with JGM2 file.
 *
 * @param <name> object name
 *
 * @return a physical model object pointer
 */
//------------------------------------------------------------------------------
PhysicalModel* Moderator::CreateDefaultPhysicalModel(const std::string &name)
{
   #ifdef DEBUG_CREATE_PHYSICAL_MODEL
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultPhysicalModel() entered, name='%s'\n", name.c_str());
   #endif
   std::string type = "GravityField";
   
   if (GetPhysicalModel(name) == NULL)
   {
      PhysicalModel *obj =
         theFactoryManager->CreatePhysicalModel(type, name);
      
      if (obj ==  NULL)
         throw GmatBaseException
               ("The Moderator cannot create a PhysicalModel type \"" + type +
                "\"\n");
      
      // set the EOP file, since it's a GravityField object
      HarmonicField *hf = (HarmonicField*) obj;
      #ifdef DEBUG_CREATE_PHYSICAL_MODEL
      MessageInterface::ShowMessage
         ("   Settng EopFile<%p> to HarmonicField<%p>\n", theEopFile, hf);
      #endif
      hf->SetEopFile(theEopFile);

      #ifdef DEBUG_MEMORY
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (obj, name, "Moderator::CreateDefaultPhysicalModel()", funcName);
      #endif
      
      SolarSystem *ss = GetSolarSystemInUse(objectManageOption);
      obj->SetName("Earth");
      obj->SetSolarSystem(ss);
      obj->SetBody("Earth");
      obj->SetBodyName("Earth");
      
      std::string potFile = GetFileName("JGM2_FILE");
      #ifdef DEBUG_DEFAULT_PHYSICAL_MODEL
      MessageInterface::ShowMessage
         ("Moderator::CreateDefaultPhysicalModel() "
          "calling %s->SetStringParameter(PotentialFile, %s)\n", obj->GetName().c_str(),
          potFile.c_str());
      #endif
      
      if (type == "GravityField")
         obj->SetStringParameter("PotentialFile", potFile);
      
      // Manage it if it is a named PhysicalModel
      try
      {
         if (name != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddPhysicalModel(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreatePhysicalModel()\n" +
                                       e.GetFullMessage());
      }
      
      #ifdef DEBUG_CREATE_PHYSICAL_MODEL
      MessageInterface::ShowMessage
         ("Moderator::CreateDefaultPhysicalModel() returning new PhysicalModel<%p>'%s'\n",
          obj, obj->GetName().c_str());
      #endif
      return obj;
   }
   else
   {
      #ifdef DEBUG_CREATE_PHYSICAL_MODEL
      MessageInterface::ShowMessage
         ("Moderator::CreateDefaultPhysicalModel() Unable to create PhysicalModel "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetPhysicalModel(name);
   }
}

//------------------------------------------------------------------------------
// PhysicalModel* CreatePhysicalModel(const std::string &type,
//                                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a physical model object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a physical model object pointer
 */
//------------------------------------------------------------------------------
PhysicalModel* Moderator::CreatePhysicalModel(const std::string &type,
                                              const std::string &name)
{
   #ifdef DEBUG_CREATE_PHYSICAL_MODEL
      MessageInterface::ShowMessage("Now attempting to create a PhysicalModel of type %s with name %s\n",
            type.c_str(), name.c_str());
   #endif
   PhysicalModel *obj = GetPhysicalModel(name);
   if (obj == NULL)
   {
      obj = theFactoryManager->CreatePhysicalModel(type, name);
      
      if (obj ==  NULL)
         throw GmatBaseException
            ("The Moderator cannot create a PhysicalModel type \"" + type + "\"\n");
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreatePhysicalModel()", funcName);
      }
      #endif
      
      // Manage it if it is a named PhysicalModel
      try
      {
         if (name != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddPhysicalModel(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreatePhysicalModel()\n" +
                                       e.GetFullMessage());
      }
   }
   else
   {
      #ifdef DEBUG_CREATE_PHYSICAL_MODEL
      MessageInterface::ShowMessage
         ("Moderator::CreatePhysicalModel() Unable to create PhysicalModel "
          "name: %s already exist\n", name.c_str());
      #endif
   }
   
   if ((obj != NULL) && obj->IsOfType("HarmonicField"))
   {
      HarmonicField *hf = (HarmonicField*) obj;
      #ifdef DEBUG_CREATE_PHYSICAL_MODEL
      MessageInterface::ShowMessage
         ("   Settng EopFile<%p> to HarmonicField<%p>\n", theEopFile, hf);
      #endif
      hf->SetEopFile(theEopFile);
   }
   if ((obj != NULL) && obj->IsOfType("RelativisticCorrection"))
   {
      RelativisticCorrection *rc = (RelativisticCorrection*) obj;
      rc->SetEopFile(theEopFile);
   }
   
   #ifdef DEBUG_CREATE_PHYSICAL_MODEL
   MessageInterface::ShowMessage
      ("Moderator::CreatePhysicalModel() returning new PhysicalModel<%p>'%s'\n",
       obj, obj->GetName().c_str());
   #endif
   return obj;
}

//------------------------------------------------------------------------------
// PhysicalModel* GetPhysicalModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a physical model object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a physical model object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
PhysicalModel* Moderator::GetPhysicalModel(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (PhysicalModel*)FindObject(name);
}

// AtmosphereModel
//------------------------------------------------------------------------------
// AtmosphereModel* CreateAtmosphereModel(const std::string &type,
//                                        const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates an atmosphere model object by given type and name and add to
 * configuration.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a atmosphereModel object pointer
 */
//------------------------------------------------------------------------------
AtmosphereModel* Moderator::CreateAtmosphereModel(const std::string &type,
                                                  const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateAtmosphereModel() type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   // if AtmosphereModel name doesn't exist, create AtmosphereModel
   AtmosphereModel *obj = GetAtmosphereModel(name);
   if (obj == NULL)
   {
      obj = theFactoryManager->CreateAtmosphereModel(type, name);
      
      if (obj ==  NULL)
         throw GmatBaseException
            ("The Moderator cannot create an AtmosphereModel type \"" + type + "\"\n");
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateAtmosphereModel()", funcName);
      }
      #endif
      
      // Manage it if it is a named AtmosphereModel
      try
      {
         if (name != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddAtmosphereModel(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateAtmosphereModel()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateAtmosphereModel() Unable to create AtmosphereModel "
          "name: %s already exist\n", name.c_str());
      #endif
      return obj;
   }
}

//------------------------------------------------------------------------------
// AtmosphereModel* GetAtmosphereModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves an atmosphere model object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a AtmosphereModel pointer, return null if name not found
 */
//------------------------------------------------------------------------------
AtmosphereModel* Moderator::GetAtmosphereModel(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (AtmosphereModel*)FindObject(name);
}

// Burn
//------------------------------------------------------------------------------
// Burn* CreateBurn(const std::string &type, const std::string &name,
//                  bool createDefault)
//------------------------------------------------------------------------------
/**
 * Creates a burn object by given type and name and add to configuration.
 * If createDefault is true, it will create "Local" coordinate system with
 * "VNB" axes. Usually this flag is set to true if ImpulsiveBurn object is
 * created from the GUI.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <createDefault> set to true if default burn to be created (false)
 *
 * @return a burn object pointer
 */
//------------------------------------------------------------------------------
Burn* Moderator::CreateBurn(const std::string &type,
                            const std::string &name, bool createDefault)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateBurn() type = '%s, name = '%s'\n", type.c_str(),
       name.c_str());
   #endif
   
   // if Burn name doesn't exist, create Burn
   if (GetBurn(name) == NULL)
   {
      Burn *obj = theFactoryManager->CreateBurn(type, name);
      
      if (obj ==  NULL)
         throw GmatBaseException
            ("The Moderator cannot create Burn type \"" + type + "\"\n");
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateBurn()", funcName);
      }
      #endif
      
      // Set default Axes to VNB
      if (createDefault)
      {
         obj->SetStringParameter(obj->GetParameterID("CoordinateSystem"), "Local");
         obj->SetStringParameter(obj->GetParameterID("Axes"), "VNB");
      }
      
      // Manage it if it is a named Burn
      try
      {
         if (name != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddBurn(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }            
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateBurn()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateBurn() Unable to create Burn "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetBurn(name);
   }
}

//------------------------------------------------------------------------------
// Burn* GetBurn(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a burn object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a burn pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Burn* Moderator::GetBurn(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (Burn*)FindObject(name);
}

// Parameter
//------------------------------------------------------------------------------
// bool Moderator::IsParameter(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Checks to see if a given type is a Parameter. If str has '.', it parses
 * string to get type before checking.
 *
 * @param <str> object type string
 *
 * @return true if the type is a registered parameter, false if not.
 */
//------------------------------------------------------------------------------
bool Moderator::IsParameter(const std::string &str)
{
   if (str == "") return false;
   StringArray sar = theFactoryManager->GetListOfItems(Gmat::PARAMETER);
   std::string type;
   
   if (str.find(".") == str.npos)
   {
      type = str;
   }
   else
   {
      std::string ownerName, depObj;
      GmatStringUtil::ParseParameter(str, type, ownerName, depObj);
   }
   
   if (find(sar.begin(), sar.end(), type) != sar.end()) {
      #ifdef DEBUG_LOOKUP_RESOURCE
      MessageInterface::ShowMessage("Found parameter \"%s\"\n", type.c_str());
      #endif
      return true;
   }

   #ifdef DEBUG_LOOKUP_RESOURCE
   MessageInterface::ShowMessage("Could not find parameter \"%s\"\n",
                                 type.c_str());
   #endif
   
   return false;   
}


//------------------------------------------------------------------------------
// Parameter* CreateAutoParameter(const std::string &type, const std::string &name
//                            const std::string &ownerName,
//                            const std::string &depName, bool manage)
//------------------------------------------------------------------------------
/**
 * Creates a parameter object by given type and name and add to configuration.
 *
 * @param <type> parameter type
 * @param <name> parameter name
 * @param <alreadyManaged> output flag indicating object already exist
 * @param <ownerName> parameter owner name ("")
 * @param <depName> dependent object name ("")
 * @param <manage>  0, if parameter is not managed
 *                  1, if parameter is added to configuration (default)
 *                  2, if Parameter is added to function object map
 *
 * @return a parameter object pointer
 */
//------------------------------------------------------------------------------
Parameter* Moderator::CreateAutoParameter(const std::string &type,
                                          const std::string &name,
                                          bool &alreadyManaged,
                                          const std::string &ownerName,
                                          const std::string &depName,
                                          Integer manage)
{
   #if DEBUG_CREATE_PARAMETER
   MessageInterface::ShowMessage
      ("Moderator::CreateAutoParameter() type='%s', name='%s', ownerName='%s', "
       "depName='%s', manage=%d\n", type.c_str(), name.c_str(), ownerName.c_str(),
       depName.c_str(), manage);
   #endif
   
   // if managing and Parameter already exist, give warning and return existing
   // Parameter
   alreadyManaged = false;
   Parameter *param = GetParameter(name);
   #if DEBUG_CREATE_PARAMETER
   MessageInterface::ShowMessage
      ("   managed param = <%p> '%s'\n", param, param ? param->GetName().c_str() : "NULL");
   #endif
   
   // if Parameter was created during GmatFunction parsing, just set reference object
   if (param != NULL && manage != 0)
   {
      #if DEBUG_CREATE_PARAMETER
      MessageInterface::ShowMessage
         ("*** WARNING *** Moderator::CreateAutoParameter() Unable to create "
          "Parameter name: %s already exist\n", name.c_str());
      MessageInterface::ShowMessage
         ("Moderator::CreateAutoParameter() returning <%s><%p>\n", param->GetName().c_str(),
          param);
      #endif
      
      // set Parameter reference object
      SetParameterRefObject(param, type, name, ownerName, depName, manage);
      
      // if Parameter is managed in the function object map, add it (loj: 2008.12.16)
      // so that we won't create multiple Parameters. FindObject() finds object from
      // objectMapInUse which can be object map from configuration or passed function
      // object map
      if (manage == 2)
         AddObject(param);
      
      alreadyManaged = true;
      
      #if DEBUG_CREATE_PARAMETER
      MessageInterface::ShowMessage
         ("Moderator::CreateAutoParameter() returning <%p>[%s]'%s', alreadyManaged=%d\n",
          param, param->GetTypeName().c_str(), param->GetName().c_str(), alreadyManaged);
      #endif
      
      return param;
   }
   
   param = CreateParameter(type, name, ownerName, depName, manage);
     
   #if DEBUG_CREATE_PARAMETER
   MessageInterface::ShowMessage
      ("Moderator::CreateAutoParameter() returning <%p>[%s]'%s', alreadyManaged=%d\n",
       param, param->GetTypeName().c_str(), param->GetName().c_str(), alreadyManaged);
   #endif
   
   return param;
}


//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string &type, const std::string &name
//                            const std::string &ownerName = "",
//                            const std::string &depName = "", bool manage = 1)
//------------------------------------------------------------------------------
/**
 * Creates a parameter object by given type and name and add to configuration.
 *
 * @param <type> parameter type
 * @param <name> parameter name
 * @param <ownerName> parameter owner name ("")
 * @param <depName> dependent object name ("")
 * @param <manage>  0, if parameter is not managed
 *                  1, if parameter is added to configuration (default)
 *                  2, if Parameter is added to function object map
 *
 * @return a parameter object pointer
 *
 * @note manage option will override member data objectManageOption so do not
 * use objectManageOption here.
 */
//------------------------------------------------------------------------------
Parameter* Moderator::CreateParameter(const std::string &type,
                                      const std::string &name,
                                      const std::string &ownerName,
                                      const std::string &depName,
                                      Integer manage)
{
   bool debugParameter = true;
   #ifdef DEBUG_ONLY_FOR_SCRIPT
   if (mainScriptFileName == "")
      debugParameter = false;
   #endif
   
   #ifdef DEBUG_ONLY_FOR_FUNCTION
   if (currentFunction == NULL)
      debugParameter = false;
   #endif
   
   #if DEBUG_CREATE_PARAMETER
   if (debugParameter)
   {
      MessageInterface::ShowMessage
         ("Moderator::CreateParameter() type='%s', name='%s', ownerName='%s', "
          "depName='%s', manage=%d\n", type.c_str(), name.c_str(), ownerName.c_str(),
          depName.c_str(), manage);
      MessageInterface::ShowMessage("   objectManageOption=%d\n", objectManageOption);
   }
   #endif
   
   // if managing and Parameter already exist, give warning and return existing
   // Parameter
   Parameter *param = GetParameter(name);
   #if DEBUG_CREATE_PARAMETER
   if (debugParameter)
   {
      MessageInterface::ShowMessage
         ("   managed param = <%p> '%s'\n", param, param ? param->GetName().c_str() : "NULL");
   }
   #endif
   
   // if Parameter was created during GmatFunction parsing, just set reference object
   if (param != NULL && manage != 0)
   {
      #if DEBUG_CREATE_PARAMETER
      if (debugParameter)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** Moderator::CreateParameter() Unable to create "
             "Parameter name: %s already exist\n", name.c_str());
         MessageInterface::ShowMessage
            ("Moderator::CreateParameter() returning <%s><%p>\n", param->GetName().c_str(),
             param);
      }
      #endif
      //If the found param of the same name is NOT the same type as the new param, throw an error
      if (param->GetTypeName() != type)
      {
         std::string errmsg = "The Moderator cannot create a Parameter named \"" + name +
            "\" of type \"" + type + "\" because a Parameter named \"" + name + "\"" +
            " of type \"" + param->GetTypeName() + "\" already exists.\n";

         throw GmatBaseException(errmsg);
      }


      // set Parameter reference object
      SetParameterRefObject(param, type, name, ownerName, depName, manage);
      
      // if Parameter is managed in the function object map, add it (loj: 2008.12.16)
      // so that we won't create multiple Parameters. FindObject() finds object from
      // objectMapInUse which can be object map from configuration or passed function
      // object map
      if (manage == 2)
         AddObject(param);
      
      return param;
   }
   
   // Check for deprecated Element* on Thruster, new Parameters are ThrustDirection*
   std::string newType = type;
   // Element1/2/3 no longer allowed for Thrusters as we have added
   // Burn parameters with dependencies - WCS 2013.07.11
//   if (type == "Element1" || type == "Element2" || type == "Element3")
//   {
//      Integer numDots = GmatStringUtil::NumberOfOccurrences(name, '.');
//      if (numDots > 1)
//      {
//         newType = GmatStringUtil::Replace(newType, "Element", "ThrustDirection");
//         #if DEBUG_CREATE_PARAMETER
//         MessageInterface::ShowMessage
//            ("   Parameter type '%s' in '%s' changed to '%s'\n", type.c_str(),
//             name.c_str(), newType.c_str());
//         #endif
//      }
//   }
   
   // Ceate new Parameter but do not add to ConfigManager yet
   #if DEBUG_CREATE_PARAMETER
   if (debugParameter)
   {
      MessageInterface::ShowMessage
         ("   about to call the factory manager to create %s\n", name.c_str());
   }
   #endif
   std::string ownerType = "";
   GmatBase * ownerObj = FindObject(ownerName);


   param = theFactoryManager->CreateParameter(newType, name, ownerObj);
   
   #ifdef DEBUG_MEMORY
   if (param)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (param, name, "Moderator::CreateParameter()", funcName);
   }
   #endif
   
   #if DEBUG_CREATE_PARAMETER
   if (debugParameter)
   {
      MessageInterface::ShowMessage
         ("   new param = <%p><%s>\n", param, param ? param->GetName().c_str() : "NULL");
   }
   #endif
   
   if (param == NULL)
   {
      #if DEBUG_CREATE_PARAMETER
      MessageInterface::ShowMessage
         ("   The type '%s' is not in the creatable list.\n", newType.c_str());
      #endif
      throw GmatBaseException
         ("The Moderator cannot create a Parameter type \"" + newType +
          "\" named \"" + name + "\"\n");
   }
   
   // We don't know the owner type the parameter before create,
   // so validate owner type after create.
   if (ownerName != "" && manage != 0)
      CheckParameterType(&param, newType, ownerName);
   
   // CheckParameterType() deletes param pointer if invalid Parameter
   // so check again
   if (param == NULL)
   {
      #if DEBUG_CREATE_PARAMETER
      if (debugParameter)
         MessageInterface::ShowMessage("Moderator::CreateParameter() returning NULL\n");
      #endif
      return NULL;
   }
   
   // set Parameter reference object
   SetParameterRefObject(param, newType, name, ownerName, depName, manage);
   
   // Add to configuration if manage flag is true and it is a named parameter
   try
   {
      // check if object is to be managed in configuration(loj: 2008.03.18)
      // @note Do not use objectManageOption here since manage flag overrides
      // this option for automatic objects such as Parameters
      if (manage == 1)
      {
         bool oldFlag = theConfigManager->HasConfigurationChanged();
         
         if (param->GetName() != "")
            theConfigManager->AddParameter(param);
         
         // if system paramter, set configuration changed to old flag.
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            #if DEBUG_CREATE_PARAMETER
            if (debugParameter)
            {
               MessageInterface::ShowMessage
                  ("   Resetting configuration changed flag for SystemParameter "
                   "'%s' to %d\n", param->GetName().c_str(), oldFlag);
            }
            #endif
            theConfigManager->ConfigurationChanged(oldFlag);
         }
      }
      else if (manage == 2) //LOJ: Add object if managed in FOS(2009.03.09)
      {
         AddObject(param);
      }
   }
   catch (BaseException &e)
   {
      // Use exception to remove Visual C++ warning
      e.GetMessageType();
      #if DEBUG_CREATE_PARAMETER
      MessageInterface::ShowMessage("In Moderator::CreateParameter()\n" +
                                    e.GetFullMessage() + "\n");
      #endif
   }
   
   #if DEBUG_CREATE_PARAMETER
   if (debugParameter)
   {
      MessageInterface::ShowMessage
         ("Moderator::CreateParameter() returning <%s><%p>\n", param->GetName().c_str(),
          param);
   }
   #endif
   
   return param;
}


//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a parameter object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a parameter object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Parameter* Moderator::GetParameter(const std::string &name)
{
   Parameter *obj = NULL;
   
   if (name != "")
   {
      // find Parameter from the current object map in use (loj: 2008.05.23)
      GmatBase* obj = FindObject(name);
      if (obj != NULL && obj->IsOfType(Gmat::PARAMETER))
         return (Parameter*)obj;
   }
   
   return obj;
}

// ODEModel
//------------------------------------------------------------------------------
// ODEModel* CreateODEModel(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Creates ODEModel with given name
 */
//------------------------------------------------------------------------------
ODEModel* Moderator::CreateODEModel(const std::string &type,
                                    const std::string &name)
{
   #ifdef DEBUG_CREATE_PHYSICAL_MODEL
   MessageInterface::ShowMessage
      ("Moderator::CreateODEModel() entered, name='%s', objectManageOption=%d\n",
       name.c_str(), objectManageOption);
   #endif
   
   ODEModel *obj = GetODEModel(name);
   
   if (obj == NULL)
   {
      obj = theFactoryManager->CreateODEModel(type, name);
      
      if (obj == NULL)
      {
         MessageInterface::ShowMessage("No fm\n");
         throw GmatBaseException
            ("The Moderator cannot create ODEModel named \"" + name + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateODEModel()", funcName);
      }
      #endif
      
      // Create default physical model
      PhysicalModel *pm = CreateDefaultPhysicalModel("");
      pm->SetName("_DefaultInternalForce_");
      obj->AddForce(pm);
      
      #ifdef DEBUG_CREATE_PHYSICAL_MODEL
      MessageInterface::ShowMessage
         ("Moderator::CreateODEModel() created _DefaultInternalForce_ and set to '%s'\n",
          obj->GetName().c_str());
      #endif
      
      // Manage it if it is a named ODEModel
      try
      {
         if (obj->GetName() != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddODEModel(obj);
            else if (objectManageOption == 2)
               // If an object is managed in the function object map, add it 
               // so that we won't create multiple objects. FindObject() finds object from
               // objectMapInUse which can be object map from configuration or passed function
               // object map
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateODEModel()\n" +
                                       e.GetFullMessage() + "\n");
      }
      
      #ifdef DEBUG_CREATE_PHYSICAL_MODEL
      MessageInterface::ShowMessage
         ("Moderator::CreateODEModel() returning new ODEModel, <%p> '%s'\n",
          obj, obj->GetName().c_str());
      #endif
      return obj;
   }
   else
   {
      #ifdef DEBUG_CREATE_PHYSICAL_MODEL
      MessageInterface::ShowMessage
         ("Moderator::CreateODEModel() Unable to create ODEModel "
          "name: %s already exist <%p>\n", name.c_str(), obj);
      #endif
      return obj;
   }
}


//------------------------------------------------------------------------------
// ODEModel* GetODEModel(const std::string &name)
//------------------------------------------------------------------------------
ODEModel* Moderator::GetODEModel(const std::string &name)
{
   ODEModel *fm = NULL;
   
   if (name != "")
   {
      // Find ODEModel from the current object map in use (loj: 2008.06.20)
      GmatBase* obj = FindObject(name);
      if (obj != NULL && obj->IsOfType(Gmat::ODE_MODEL))
      {
         fm = (ODEModel*)obj;
         
         #ifdef DEBUG_CREATE_PHYSICAL_MODEL
         MessageInterface::ShowMessage
            ("Moderator::GetODEModel() name='%s', returning <%p>\n", name.c_str(), fm);
         #endif
         
         return fm;
      }
   }
   
   #ifdef DEBUG_CREATE_PHYSICAL_MODEL
   MessageInterface::ShowMessage
      ("Moderator::GetODEModel() name='%s', returning <%p>\n", name.c_str(), fm);
   #endif
   
   return fm;
}


//------------------------------------------------------------------------------
// bool AddToODEModel(const std::string &ODEModelName,
//                      const std::string &forceName)
//------------------------------------------------------------------------------
bool Moderator::AddToODEModel(const std::string &odeModelName,
                                const std::string &forceName)
{
   bool status = true;
   ODEModel *fm = theConfigManager->GetODEModel(odeModelName);
   PhysicalModel *physicalModel = theConfigManager->GetPhysicalModel(forceName);
   fm->AddForce(physicalModel);
   return status;
}


// Solver
//------------------------------------------------------------------------------
// Solver* CreateSolver(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a solver object by given type and name and add to configuration.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a solver object pointer
 */
//------------------------------------------------------------------------------
Solver* Moderator::CreateSolver(const std::string &type, const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateSolver() type = '%s', name = '%s'\n", type.c_str(),
       name.c_str());
   #endif
   
   if (GetSolver(name) == NULL)
   {
      Solver *obj = theFactoryManager->CreateSolver(type, name);

      if (obj == NULL)
      {
         throw GmatBaseException
            ("The Moderator cannot create Solver type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateSolver()", funcName);
      }
      #endif
      
      // Manage it if it is a named Solver
      try
      {
         if (obj->GetName() != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddSolver(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateSolver()\n" +
                                       e.GetFullMessage());
      }
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateSolver() Unable to create Solver "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetSolver(name);
   }
}


//------------------------------------------------------------------------------
// Solver* GetSolver(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a solver object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a solver object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Solver* Moderator::GetSolver(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (Solver*)FindObject(name);
}

// PropSetup
//------------------------------------------------------------------------------
// PropSetup* CreateDefaultPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* Moderator::CreateDefaultPropSetup(const std::string &name)
{
   #if DEBUG_CREATE_PROP_SETUP
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultPropSetup() entered, name='%s'\n", name.c_str());
   #endif
   
   // Create PropSetup, PropSetup constructor creates default RungeKutta89 Integrator
   // and Earth PointMassForce
   PropSetup *propSetup = CreatePropSetup(name);   
   std::string theFmName = name + "_ForceModel";
   ODEModel *fm = CreateODEModel("ForceModel", theFmName);   
   propSetup->SetODEModel(fm);
   
   #if DEBUG_CREATE_PROP_SETUP
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultPropSetup() returning new DefaultPropSetup <%p>\n", propSetup);
   #endif
   
   return propSetup;
}

//------------------------------------------------------------------------------
// PropSetup* CreatePropSetup(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Creates PropSetup which contains Integrator and ODEModel.
 */
//------------------------------------------------------------------------------
PropSetup* Moderator::CreatePropSetup(const std::string &name)
{
   #if DEBUG_CREATE_PROP_SETUP
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage
      ("Moderator::CreatePropSetup() entered, name='%s'\n", name.c_str());
   #endif
   
   if (GetPropSetup(name) == NULL)
   {
      PropSetup *propSetup = theFactoryManager->CreatePropSetup(name);
      
      if (propSetup == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a PropSetup.\n"
             "Make sure PropSetup is correct type and registered to "
             "PropSetupFactory.\n");
         return NULL;
      }
      
      #ifdef DEBUG_MEMORY
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (propSetup, name, "Moderator::CreatePropSetup()", funcName);
      #endif
      
      // PropSetup creates default Integrator(RungeKutta89)
      // and default ODEModel (PointMassForce body=Earth)
      
      if (name != "")
      {
         if (objectManageOption == 1)
            theConfigManager->AddPropSetup(propSetup);
         else if (objectManageOption == 2)
         {
            // LOJ: 2015.03.11
            // If creating a default propagator in a function, create default ODEmodel
            // For the main script, Interpreter::FinalPass() creates default ODEModel.
            std::string theFmName = name + "_ForceModel";
            ODEModel *fm = CreateODEModel("ForceModel", theFmName);
            propSetup->SetODEModel(fm);
            AddObject(propSetup);
         }
      }
      
      #if DEBUG_CREATE_PROP_SETUP
      MessageInterface::ShowMessage
         ("Moderator::CreatePropSetup() returning new PropSetup <%p>\n", propSetup);
      #endif
      
      return propSetup;
   }
   else
   {
      #if DEBUG_CREATE_PROP_SETUP
      MessageInterface::ShowMessage
         ("Moderator::CreatePropSetup() Unable to create PropSetup "
          "name: %s already exist\n", name.c_str());
      #endif
      return GetPropSetup(name);
   }
}

//------------------------------------------------------------------------------
// PropSetup* GetPropSetup(const std::string &name)
//------------------------------------------------------------------------------
PropSetup* Moderator::GetPropSetup(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (PropSetup*)FindObject(name);
}


// MeasurementModel
//------------------------------------------------------------------------------
// MeasurementModelBase* CreateMeasurementModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a new named MeasurementModel and adds it to the configuration
 *
 * @param name The name of the new MeasurementModel
 *
 * @return The new MeasurementModel
 */
//------------------------------------------------------------------------------
MeasurementModelBase* Moderator::CreateMeasurementModel(const std::string &type,
      const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::GetMeasurementModel() name='%s'\n",
                                 name.c_str());
   #endif

   if (GetMeasurementModel(name) == NULL)
   {
      MeasurementModelBase *obj =
            theFactoryManager->CreateMeasurementModel(type, name);

      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a MeasurementModel.\n"
             "Make sure MeasurementModel is correct type and registered to "
             "MeasurementModelFactory.\n");
         return NULL;
      }

      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateMeasurementModel()", funcName);
      }
      #endif
      
      if (name != "")
      {
         if (objectManageOption == 1)
            theConfigManager->AddMeasurementModel(obj);
         else if (objectManageOption == 2)
            AddObject((GmatBase*)obj);
      }
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateMeasurementModel() returning new MeasurementModel "
               "<%p>\n", obj);
      #endif

      return obj;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateMeasurementModel() Unable to create "
          "MeasurementModel name: %s already exist\n", name.c_str());
      #endif
      return GetMeasurementModel(name);
   }
}

//------------------------------------------------------------------------------
// MeasurementModelBase* GetMeasurementModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a measurement model from the configuration
 *
 * @param name The name of the MeasurementModel object
 *
 * @return The named MeasurementModel
 */
//------------------------------------------------------------------------------
MeasurementModelBase* Moderator::GetMeasurementModel(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (MeasurementModelBase*)FindObject(name);
}

//// TrackingSystem
////------------------------------------------------------------------------------
//// TrackingSystem* CreateTrackingSystem(const std::string &type,
////       const std::string &name)
////------------------------------------------------------------------------------
///**
// * Creates a new named TrackingSystem and adds it to the configuration
// *
// * @param type The type of the new TrackingSystem
// * @param name The name of the new TrackingSystem
// *
// * @return The new TrackingSystem
// */
////------------------------------------------------------------------------------
//TrackingSystem* Moderator::CreateTrackingSystem(const std::string &type,
//         const std::string &name)
//{
//   #if DEBUG_CREATE_RESOURCE
//   MessageInterface::ShowMessage("====================\n");
//   MessageInterface::ShowMessage("Moderator::CreateTrackingSystem() type=%s, "
//            "name='%s'\n", type.c_str(), name.c_str());
//   #endif
//
//   if (GetTrackingSystem(name) == NULL)
//   {
//      TrackingSystem *obj = theFactoryManager->CreateTrackingSystem(type, name);
//
//      if (obj == NULL)
//      {
//         MessageInterface::PopupMessage
//            (Gmat::ERROR_, "The Moderator cannot create a TrackingSystem.\n"
//             "Make sure TrackingSystem is correct type and registered to "
//             "TrackingSystemFactory.\n");
//         return NULL;
//      }
//
//      #ifdef DEBUG_MEMORY
//      if (obj)
//      {
//         std::string funcName;
//         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
//         MemoryTracker::Instance()->Add
//            (obj, name, "Moderator::CreateTrackingSystem()", funcName);
//      }
//      #endif
//      
//      if (name != "")
//      {
//         if (objectManageOption == 1)
//            theConfigManager->AddTrackingSystem(obj);
//         else if (objectManageOption == 2)
//            AddObject((GmatBase*)obj);
//      }
//      
//      #if DEBUG_CREATE_RESOURCE
//      MessageInterface::ShowMessage
//         ("Moderator::CreateTrackingSystem() returning new TrackingSystem "
//               "<%p>\n", obj);
//      #endif
//
//      return obj;
//   }
//   else
//   {
//      #if DEBUG_CREATE_RESOURCE
//      MessageInterface::ShowMessage
//         ("Moderator::CreateTrackingSystem() Unable to create "
//          "TrackingSystem name: %s already exists\n", name.c_str());
//      #endif
//      return GetTrackingSystem(name);
//   }
//}
//
////------------------------------------------------------------------------------
//// TrackingSystem* GetTrackingSystem(const std::string &name)
////------------------------------------------------------------------------------
///**
// * Retrieves a TrackingSystem from the configuration
// *
// * @param name The name of the TrackingSystem object
// *
// * @return The named TrackingSystem
// */
////------------------------------------------------------------------------------
//TrackingSystem* Moderator::GetTrackingSystem(const std::string &name)
//{
//   if (name == "")
//      return NULL;
//   else
//      return (TrackingSystem*)FindObject(name);
//}

//// TrackingData
////------------------------------------------------------------------------------
//// TrackingData* CreateTrackingData(const std::string &name)
////------------------------------------------------------------------------------
///**
// * Creates a new named TrackingData object and adds it to the configuration
// *
// * @param name The name of the new TrackingData object
// *
// * @return The new TrackingData object
// */
////------------------------------------------------------------------------------
//TrackingData* Moderator::CreateTrackingData(const std::string &name)
//{
//   #if DEBUG_CREATE_RESOURCE
//   MessageInterface::ShowMessage("====================\n");
//   MessageInterface::ShowMessage("Moderator::CreateTrackingData() name='%s'\n",
//                                 name.c_str());
//   #endif
//
//   if (GetTrackingData(name) == NULL)
//   {
//      TrackingData *obj = theFactoryManager->CreateTrackingData(name);
//
//      if (obj == NULL)
//      {
//         MessageInterface::PopupMessage
//            (Gmat::ERROR_, "The Moderator cannot create a TrackingData object."
//             "\nMake sure TrackingData is correct type and registered to "
//             "TrackingDataFactory.\n");
//         return NULL;
//      }
//
//      #ifdef DEBUG_MEMORY
//      if (obj)
//      {
//         std::string funcName;
//         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
//         MemoryTracker::Instance()->Add
//            (obj, name, "Moderator::CreateTrackingData()", funcName);
//      }
//      #endif
//      
//      if (name != "")
//      {
//         if (objectManageOption == 1)
//            theConfigManager->AddTrackingData(obj);
//         else if (objectManageOption == 2)
//            AddObject((GmatBase*)obj);
//      }
//      
//      #if DEBUG_CREATE_RESOURCE
//      MessageInterface::ShowMessage
//         ("Moderator::CreateTrackingData() returning new TrackingData "
//               "<%p>\n", obj);
//      #endif
//      
//      return obj;
//   }
//   else
//   {
//      #if DEBUG_CREATE_RESOURCE
//      MessageInterface::ShowMessage
//         ("Moderator::CreateTrackingData() Unable to create "
//          "TrackingData name: %s already exists\n", name.c_str());
//      #endif
//      return GetTrackingData(name);
//   }
//}
//
////------------------------------------------------------------------------------
//// TrackingData* GetTrackingData(const std::string &name)
////------------------------------------------------------------------------------
///**
// * Retrieves a TrackingData object from the configuration
// *
// * @param name The name of the TrackingData object
// *
// * @return The named TrackingData object
// */
////------------------------------------------------------------------------------
//TrackingData* Moderator::GetTrackingData(const std::string &name)
//{
//   if (name == "")
//      return NULL;
//   else
//      return (TrackingData*)FindObject(name);
//}


//////------------------------------------------------------------------------------
////// CoreMeasurement* CreateMeasurement(const std::string &type,
//////       const std::string &name)
//////------------------------------------------------------------------------------
/////**
//// * This method calls the FactoryManager to create a new CoreMeasurement object
//// *
//// * @param type The type of measurement object needed
//// * @param name The new object's name, is it is to be configured
//// *
//// * @return The new object's pointer
//// */
//////------------------------------------------------------------------------------
////CoreMeasurement* Moderator::CreateMeasurement(const std::string &type,
////      const std::string &name)
////{
////   #if DEBUG_CREATE_RESOURCE
////   MessageInterface::ShowMessage("====================\n");
////   MessageInterface::ShowMessage("Moderator::CreateMeasurement() name='%s'\n",
////                                 name.c_str());
////   #endif
////
////   if (GetMeasurement(type, name) == NULL)
////   {
////      CoreMeasurement *obj = theFactoryManager->CreateMeasurement(type, name);
////
////      if (obj == NULL)
////      {
////         MessageInterface::PopupMessage
////            (Gmat::ERROR_, "The Moderator cannot create a Measurement.\n"
////             "Make sure Measurement %s is correct type and registered to "
////             "MeasurementFactory.\n", type.c_str());
////         return NULL;
////      }
////
////      #ifdef DEBUG_MEMORY
////      if (obj)
////      {
////         std::string funcName;
////         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
////         MemoryTracker::Instance()->Add
////            (obj, name, "Moderator::CreateMeasurementModel()", funcName);
////      }
////      #endif
////
////      if (name != "")
////      {
////         if (objectManageOption == 1)
////            theConfigManager->AddMeasurement(obj);
////         else if (objectManageOption == 2)
////            AddObject((GmatBase*)obj);
////      }
////      
////      #if DEBUG_CREATE_RESOURCE
////      MessageInterface::ShowMessage
////         ("Moderator::CreateMeasurement() returning new Measurement "
////               "<%p>\n", obj);
////      #endif
////
////      return obj;
////   }
////   else
////   {
////      #if DEBUG_CREATE_RESOURCE
////      MessageInterface::ShowMessage
////         ("Moderator::CreateMeasurement() Unable to create "
////          "Measurement name: %s already exist\n", name.c_str());
////      #endif
////      return GetMeasurement(type, name);
////   }
////}
////
//////------------------------------------------------------------------------------
////// CoreMeasurement* GetMeasurement(const std::string &type,
//////       const std::string &name)
//////------------------------------------------------------------------------------
/////**
//// * This method finds a configured CoreMeasurement
//// *
//// * @param type The type of measurement object needed
//// * @param name The new object's name, is it is to be configured
//// *
//// * @return The object's pointer; NULL if the object is not in the configuration
//// */
//////------------------------------------------------------------------------------
////CoreMeasurement* Moderator::GetMeasurement(const std::string &type,
////      const std::string &name)
////{
////   if (name == "")
////      return NULL;
////   else
////      return (CoreMeasurement*)FindObject(name);
////}


// DataFile
//------------------------------------------------------------------------------
// DataFile* CreateDataFile(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a new named DataFile and adds it to the configuration
 *
 * @param type The type of the DataFile object
 * @param name The name of the new DataFile
 *
 * @return The new DataFile
 */
//------------------------------------------------------------------------------
DataFile* Moderator::CreateDataFile(const std::string &type,
                                    const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage
      ("Moderator::CreateDataFile() type='%s', name='%s'\n", type.c_str(),
       name.c_str());
   #endif
   
   if (GetDataFile(name) == NULL)
   {
      DataFile *df = theFactoryManager->CreateDataFile(type, name);
      
      if (df == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a DataFile.\n"
             "Make sure DataFile is correct type and registered to "
             "DataFileFactory.\n");
         return NULL;
      }
      
      #ifdef DEBUG_MEMORY
      if (df)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (df, name, "Moderator::CreateDataFile()", funcName);
      }
      #endif
      
      if (name != "")
      {
         if (objectManageOption == 1)
            theConfigManager->AddDataFile(df);
         else if (objectManageOption == 2)
            AddObject((GmatBase*)df);
      }
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateDataFile() returning new DataFile <%p>\n", df);
      #endif
      
      return df;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateDataFile() Unable to create "
          "DataFile name: %s already exists\n", name.c_str());
      #endif
      return GetDataFile(name);
   }
}


//------------------------------------------------------------------------------
// DataFile* GetDataFile(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a measurement DataFile from the configuration
 *
 * @param name The name of the DataFile object
 *
 * @return The named DataFile
 */
//------------------------------------------------------------------------------
DataFile* Moderator::GetDataFile(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (DataFile*)FindObject(name);
}


// ObType
//------------------------------------------------------------------------------
// ObType* CreateObType(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
ObType* Moderator::CreateObType(const std::string &type, const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreateObType() name='%s'\n",
                                 name.c_str());
   #endif

   if (GetObType(name) == NULL)
   {
      ObType *ot = theFactoryManager->CreateObType(type, name);

      if (ot == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a ObType.\n"
             "Make sure ObType is correct type and registered to a "
             "ObTypeFactory.\n");
         return NULL;
      }

      #ifdef DEBUG_MEMORY
      if (ot)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (ot, name, "Moderator::CreateObType()", funcName);
      }
      #endif

      if (name != "")
      {
         if (objectManageOption == 1)
            theConfigManager->AddObType(ot);
         else if (objectManageOption == 2)
            AddObject((GmatBase*)ot);
      }
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateObType() returning new ObType <%p>\n", ot);
      #endif
      
      return ot;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateDataFile() Unable to create "
          "DataFile name: %s already exists\n", name.c_str());
      #endif
      return GetObType(name);
   }
}

//------------------------------------------------------------------------------
// ObType* GetObType(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a ObType from the configuration
 *
 * @param name The name of the ObType object
 *
 * @return The named ObType  (Should always return NULL)
 */
//------------------------------------------------------------------------------
ObType* Moderator::GetObType(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (ObType*)FindObject(name);
}


//------------------------------------------------------------------------------
// EventLocator* Moderator::CreateEventLocator(const std::string &type,
//                          const std::string &name, bool createDefault = false)
//------------------------------------------------------------------------------
/**
 * Calls the FactoryManager to create an EventLocator
 *
 * @param type The type of event locator to be created
 * @param name The name of the new EventLocator
 * @param createDefault if this is set, it will create/set the default SC on the
 *                      new locator
 *
 * @return The named EventLocator
 */
//------------------------------------------------------------------------------
EventLocator* Moderator::CreateEventLocator(const std::string &type,
                         const std::string &name, bool createDefault)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("====================\n");
   MessageInterface::ShowMessage("Moderator::CreateEventLocator() name='%s'\n",
                                 name.c_str());
   #endif

   if (GetEventLocator(name) == NULL)
   {
      EventLocator *el = theFactoryManager->CreateEventLocator(type, name);

      if (el == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create an EventLocator.\n"
             "Make sure EventLocator is correct type and registered to a "
             "EventLocatorFactory.\n");
         return NULL;
      }

      #ifdef DEBUG_MEMORY
      if (el)
      {
         std::string funcName;
         funcName = currentFunction ?
               "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (el, name, "Moderator::CreateEventLocator()", funcName);
      }
      #endif

      if (name != "")
      {
         if (objectManageOption == 1)
            theConfigManager->AddEventLocator(el);
         else if (objectManageOption == 2)
            AddObject((GmatBase*)el);
      }
      
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateEventLocator() returning new EventLocator <%p>\n", el);
      #endif
      
      if (createDefault)
      {
         // set spacecraft
         std::string scOrTarget = "Spacecraft";
         if (el->IsOfType("ContactLocator"))
               scOrTarget = "Target";
         Integer id = el->GetParameterID(scOrTarget);
         el->SetStringParameter(id, GetDefaultSpacecraft()->GetName());
      }

      return el;
   }
   else
   {
      #if DEBUG_CREATE_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateEventLocator() Unable to create "
          "EventLocator name: %s already exists\n", name.c_str());
      #endif
      return GetEventLocator(name);
   }
}


//------------------------------------------------------------------------------
// EventLocator* GetEventLocator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a previously created EvantLoactor
 *
 * @param name The name of the EventLocator
 *
 * @return A pointer to the EventLocator, or NULL if it is not in the
 *         configuration.
 */
//------------------------------------------------------------------------------
EventLocator* Moderator::GetEventLocator(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (EventLocator*)FindObject(name);
}



//------------------------------------------------------------------------------
// Interpolator* CreateInterpolator(const std::string &type,
//                                  const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a Interpolator object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a Interpolator object pointer
 */
//------------------------------------------------------------------------------
Interpolator* Moderator::CreateInterpolator(const std::string &type,
                                            const std::string &name)
{
   //loj: 3/22/04 theFactoryManager->CreateInterpolator() not implemented
   return NULL;
}

//------------------------------------------------------------------------------
// Interpolator* GetInterpolator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Interpolator object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a Interpolator object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Interpolator* Moderator::GetInterpolator(const std::string &name)
{
   return NULL;
}

// CoordinateSystem
//------------------------------------------------------------------------------
// CoordinateSystem* CreateCoordinateSystem(const std::string &name,
//                   bool createDefault = false, bool internal = false,
//                   Integer manage = 1)
//------------------------------------------------------------------------------
/**
 * Creates coordinate system
 *
 * @param  name  Name of the coordinate system to create
 * @param  createDefault  If this flag is set, it will create MJ2000Eq system
 * @param  internal  If this flag is set, it will not configure the CS
 * @param  manage  The value to use for managing the newly created CS
 *                 0 = do not add to configuration
 *                 1 = add to configuration
 *                 2 = function object map is used
 */
//------------------------------------------------------------------------------
CoordinateSystem* Moderator::CreateCoordinateSystem(const std::string &name,
                                                    bool createDefault,
                                                    bool internal, Integer manage)
{
   #if DEBUG_CREATE_COORDSYS
   MessageInterface::ShowMessage
      ("Moderator::CreateCoordinateSystem() name='%s', createDefault=%d, "
       "internal=%d, manage=%d, theSolarSystemInUse=%p\n", name.c_str(), createDefault,
       internal, manage, theSolarSystemInUse);
   #endif
   
   CoordinateSystem *obj = GetCoordinateSystem(name);
   
   if (obj == NULL)
   {
      #if DEBUG_CREATE_COORDSYS
      MessageInterface::ShowMessage
         ("   Cannot find CoordinateSystem named '%s', so create\n", name.c_str());
      #endif
      
      obj = theFactoryManager->CreateCoordinateSystem(name);
      
      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "The Moderator cannot create a CoordinateSystem.\n"
             "Make sure CoordinateSystem is correct type and registered to "
             "CoordinateSystemFactory.\n");
         
         return NULL;
         
         //throw GmatBaseException
         //   ("The Moderator cannot create a CoordinateSystem type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateCoordinateSystem()", funcName);
      }
      #endif
      
      // Manage it if it is a named CoordinateSystem
      try
      {
         if (name != "" && !internal && manage != 0)
         {
            if (manage == 1)
               theConfigManager->AddCoordinateSystem(obj);
            else
               // Do we really want to add a new CoordinateSystem to the
               // function object map? (loj: 2008.06.27)
               AddObject((GmatBase*)obj);
         }
         
         // Call GetSolarSystemInUse() to get SolarSystem from configuration
         // or object map in use (loj: 2008.06.27)
         SolarSystem *ss = GetSolarSystemInUse(manage);
         CelestialBody *earth = ss->GetBody("Earth");
         #if DEBUG_CREATE_COORDSYS
            MessageInterface::ShowMessage
               ("Mod::CreateCS = SolarSystem found at <%p>\n", ss);
            MessageInterface::ShowMessage
               ("Mod::CreateCS = Earth found at <%p>\n", earth);
         #endif
         
         // Set J2000Body and SolarSystem
         obj->SetJ2000BodyName("Earth");
         obj->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");
         obj->SetSolarSystem(ss);
         obj->Initialize();
         
         // Create axis if creating default coordinate system
         // or creating inside a function (LOJ: 2015.02.19 - to fix ACE script)
         if (createDefault || manage == 2)
         {
            AxisSystem *axis;
            if (name.find("Fixed") == std::string::npos)
               // create MJ2000Eq AxisSystem with Earth as origin
               axis = CreateAxisSystem("MJ2000Eq", "MJ2000Eq_Earth");
            else
               // create body fixed AxisSystem with Earth as origin
               axis = CreateAxisSystem("BodyFixed", "EarthFixed");
            
            obj->SetJ2000BodyName("Earth");
            obj->SetStringParameter("Origin", "Earth");
            obj->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");
            obj->SetRefObject(axis, Gmat::AXIS_SYSTEM, axis->GetName());
            obj->SetSolarSystem(ss);
            obj->Initialize();

            // Since CoordinateSystem clones AxisSystem, delete it from here
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (axis, "localAxes", "Moderator::CreateCoordinateSystem()",
                "deleting localAxes");
            #endif
            delete axis;
         }
      }
      catch (BaseException &e)
      {
         // Use exception to remove Visual C++ warning
         e.GetMessageType();
         #if DEBUG_CREATE_COORDSYS
         MessageInterface::ShowMessage("In Moderator::CreateCoordinateSystem() %s\n",
                                       e.GetFullMessage().c_str());
         #endif
      }
      
      #if DEBUG_CREATE_COORDSYS
      MessageInterface::ShowMessage
         ("Moderator::CreateCoordinateSystem() returning new %p\n", obj);
      #endif
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_COORDSYS
      MessageInterface::ShowMessage
         ("Moderator::CreateCoordinateSystem() Unable to create CoordinateSystem "
          "name: %s already exist\n", name.c_str());      
      MessageInterface::ShowMessage
         ("Moderator::CreateCoordinateSystem() returning existing %p\n", obj);
      #endif
      
      return obj;
   }
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetCoordinateSystem(const std::string &name, Integer manage = 1)
//------------------------------------------------------------------------------
CoordinateSystem* Moderator::GetCoordinateSystem(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (CoordinateSystem*)FindObject(name);
}


//------------------------------------------------------------------------------
// const StringArray& GetDefaultCoordinateSystemNames()
//------------------------------------------------------------------------------
const StringArray& Moderator::GetDefaultCoordinateSystemNames()
{
   return defaultCoordSystemNames;
}

//------------------------------------------------------------------------------
// bool IsDefaultCoordinateSystem(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Returns true if name found in the default coordinate system names, false otherwise
 **/
//------------------------------------------------------------------------------
bool Moderator::IsDefaultCoordinateSystem(const std::string &name)
{
   if (find(defaultCoordSystemNames.begin(), defaultCoordSystemNames.end(), name) ==
       defaultCoordSystemNames.end())
      return false;
   else
      return true;
}


// Subscriber
//------------------------------------------------------------------------------
// Subscriber* CreateSubscriber(const std::string &type, const std::string &name,
//                              bool createDefault = false)
//------------------------------------------------------------------------------
/**
 * Creates a subscriber object by given type and name if not already created.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <fileName> file name if used
 * @param <createDefalut> create default Subscriber by setting ref. object if true
 *
 * @return a subscriber object pointer
 */
//------------------------------------------------------------------------------
Subscriber* Moderator::CreateSubscriber(const std::string &type,
                                        const std::string &name,
                                        bool createDefault)
{
   #if DEBUG_CREATE_SUBSCRIBER
   MessageInterface::ShowMessage
      ("Moderator::CreateSubscriber() type='%s', name='%s'\n"
       "   createDefault=%d\n", type.c_str(), name.c_str(), createDefault);
   MessageInterface::ShowMessage
      ("   currentFunction = <%p>'%s', objectManageOption=%d\n", currentFunction,
       currentFunction ? currentFunction->GetName().c_str() : "NULL",
       objectManageOption);
   #endif
   
   if (GetSubscriber(name) == NULL)
   {      
      Subscriber *obj = theFactoryManager->CreateSubscriber(type, name);
      
      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Cannot create a Subscriber type: %s.\n"
             "Make sure %s is correct type and registered to SubscriberFactory.\n",
             type.c_str(), type.c_str());
         
         return NULL;
         
         //throw GmatBaseException
         //   ("The Moderator cannot create a Subscriber type\"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateSubscriber()", funcName);
      }
      #endif
      
      // Manage it if it is a named Subscriber
      try
      {
         if (obj->GetName() != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddSubscriber(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
         
         #if DEBUG_CREATE_SUBSCRIBER
         MessageInterface::ShowMessage
            ("Moderator::CreateSubscriber() Creating default Subscriber...\n");
         #endif
         
         if (createDefault)
         {
            if (type == "OrbitView")
            {
               // add default spacecraft and coordinate system
               obj->SetStringParameter("Add", GetDefaultSpacecraft()->GetName());
               obj->SetStringParameter("Add", "Earth");
               obj->SetStringParameter("CoordinateSystem", "EarthMJ2000Eq");
            }
            else if (type == "GroundTrackPlot")
            {
               // add default spacecraft and earth
               obj->SetStringParameter("Add", GetDefaultSpacecraft()->GetName());
               obj->SetStringParameter("Add", "Earth");
            }
            else if (type == "XYPlot")
            {
               // add default x,y parameter to XYPlot
               obj->SetStringParameter(XyPlot::XVARIABLE, GetDefaultX()->GetName());
               obj->SetStringParameter(XyPlot::YVARIABLES, GetDefaultY()->GetName(), 0);
               obj->Activate(true);
               
               // To validate and create element wrappers
               theScriptInterpreter->ValidateSubscriber(obj);
            }
            else if (type == "ReportFile")
            {
               // add default parameters to ReportFile
               obj->SetStringParameter(obj->GetParameterID("Filename"),
                                       name + ".txt");
               obj->SetStringParameter("Add", GetDefaultX()->GetName());
               obj->SetStringParameter("Add", GetDefaultY()->GetName());
               obj->Activate(true);
               
               // To validate and create element wrappers
               theScriptInterpreter->ValidateSubscriber(obj);
            }
            else if (type == "EphemerisFile")
            {
               // add default spacecraft and coordinate system
               obj->SetStringParameter("Spacecraft", GetDefaultSpacecraft()->GetName());
            }
            else if (type == "DynamicDataDisplay")
            {
               // add two empty rows and columns to the display
               obj->SetStringParameter("AddParameters", "1", 0);
               obj->SetStringParameter("AddParameters", "", 1);
               obj->SetStringParameter("AddParameters", "", 1);
               obj->SetStringParameter("AddParameters", "2", 0);
               obj->SetStringParameter("AddParameters", "", 1);
               obj->SetStringParameter("AddParameters", "", 1);
            }
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateSubscriber()\n" +
                                       e.GetFullMessage());
      }
      
      #if DEBUG_CREATE_SUBSCRIBER
      MessageInterface::ShowMessage
         ("Moderator::CreateSubscriber() returning new Subscriber <%p><%s>'%s'\n",
          obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
      #endif
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_SUBSCRIBER
      MessageInterface::ShowMessage
         ("Moderator::CreateSubscriber() Unable to create Subscriber "
          "name: %s already exist\n", name.c_str());
      #endif
      
      return GetSubscriber(name);
   }
}

//------------------------------------------------------------------------------
// Subscriber* GetSubscriber(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a subscriber object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a subscriber object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Subscriber* Moderator::GetSubscriber(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (Subscriber*)FindObject(name);
}


//------------------------------------------------------------------------------
// Integer GetNumberOfActivePlots()
//------------------------------------------------------------------------------
/**
 * Returns number of active plots which means plots with ShowPlot is on.
 */
//------------------------------------------------------------------------------
Integer Moderator::GetNumberOfActivePlots()
{
   Integer activePlotCount = 0;
   Subscriber *obj;
   StringArray names = theConfigManager->GetListOfItems(Gmat::SUBSCRIBER);
   
   #ifdef DEBUG_ACTIVE_PLOTS
   MessageInterface::ShowMessage
      ("Moderator::GetNumberOfActivePlots() subscriber count = %d\n", names.size());
   #endif
   
   //@todo
   // Should we create a new class GmatPlot and derive XYPlot and OrbitPlot from it?
   for (Integer i=0; i<(Integer)names.size(); i++)
   {
      obj = theConfigManager->GetSubscriber(names[i]);
      if (obj->IsOfType("XYPlot"))
      {
         if ( ((XyPlot*)obj)->GetBooleanParameter("ShowPlot") )
            activePlotCount++;
      }
      else if (obj->IsOfType("OrbitPlot"))
      {
         if ( ((OrbitPlot*)obj)->GetBooleanParameter("ShowPlot") )
            activePlotCount++;
      }
   }
   
   #ifdef DEBUG_ACTIVE_PLOTS
   MessageInterface::ShowMessage
      ("Moderator::GetNumberOfActivePlots() returning %d\n", activePlotCount);
   #endif
   
   return activePlotCount;
}


//------------------------------------------------------------------------------
// Subscriber* CreateEphemerisFile(const std::string &type,
//                                 const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a subscriber object by given type and name if not already created.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a subscriber object pointer
 */
//------------------------------------------------------------------------------
Subscriber* Moderator::CreateEphemerisFile(const std::string &type,
                                           const std::string &name)
{
   #if DEBUG_CREATE_EPHEMFILE
   MessageInterface::ShowMessage
      ("Moderator::CreateEphemerisFile() type='%s', name='%s'\n",
       type.c_str(), name.c_str());
   MessageInterface::ShowMessage
      ("   in function = <%p>'%s'\n", currentFunction,
       currentFunction ? currentFunction->GetName().c_str() : "NULL");
   #endif
   
   if (GetEphemerisFile(name) == NULL)
   {      
      Subscriber *obj = (Subscriber*)(theFactoryManager->CreateEphemerisFile(type, name));
      
      if (obj == NULL)
      {
         #ifdef __USE_DATAFILE__
            if (type == "CcsdsEphemerisFile")
            {
               MessageInterface::PopupMessage
                  (Gmat::ERROR_, "**** ERROR **** Cannot create an EphemerisFile type: %s.\n"
                   "Make sure to specify PLUGIN = libDataFile and PLUGIN = libCcsdsEphemerisFile\n"
                   "in the gmat_start_file and make sure such dlls exist.  "
                   "Make sure that libpcre-0 and libpcrecpp-0 also exist in the path\n",
                   type.c_str(), type.c_str());

               return NULL;
            }
         #else
            #ifdef DEBUG_CREATE_EPHEMFILE
            MessageInterface::ShowMessage
               ("CreateEphemerisFile() Creating a subscriber of type EphemerisFile\n");
            #endif
            // Try again with "EphemerisFile" type
            obj = (Subscriber*)(theFactoryManager->CreateSubscriber("EphemerisFile", name));
         #endif
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateEphemerisFile()", funcName);
      }
      #endif
      
      // Manage it if it is a named EphemerisFile
      try
      {
         if (name != "")
         {
            if (objectManageOption == 1)
               theConfigManager->AddSubscriber(obj);
            else if (objectManageOption == 2)
               AddObject(obj);
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateEphemerisFile()\n" +
                                       e.GetFullMessage());
      }
      
      #if DEBUG_CREATE_EPHEMFILE
      MessageInterface::ShowMessage
         ("Moderator::CreateEphemerisFile() returning <%p>\n", obj);
      #endif
      
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_EPHEMFILE
      MessageInterface::ShowMessage
         ("Moderator::CreateEphemerisFile() Unable to create EphemerisFile "
          "name: %s already exist\n", name.c_str());
      #endif
      
      return GetEphemerisFile(name);
   }
}

//------------------------------------------------------------------------------
// Subscriber* GetEphemerisFile(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a subscriber object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a subscriber object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Subscriber* Moderator::GetEphemerisFile(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return (Subscriber*)FindObject(name);
}

// Function
//------------------------------------------------------------------------------
// Function* CreateFunction(const std::string &type, const std::string &name,
//                          Integer manage = 1)
//------------------------------------------------------------------------------
/**
 * Creates a function object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <manage> if 0, new function is not managed
 *                    1, new function is added to configuration
 * @return a Function object pointer
 */
//------------------------------------------------------------------------------
Function* Moderator::CreateFunction(const std::string &type,
                                    const std::string &name,
                                    Integer manage)
{
   #if DEBUG_CREATE_FUNCTION
   MessageInterface::ShowMessage
      ("Moderator::CreateFunction() type = '%s', name = '%s', manage=%d\n",
       type.c_str(), name.c_str(), manage);
   MessageInterface::ShowMessage
      ("   in function = <%p>'%s'\n", currentFunction,
       currentFunction ? currentFunction->GetName().c_str() : "NULL");
   #endif
   
   if (GetFunction(name) == NULL)
   {
      Function *obj = theFactoryManager->CreateFunction(type, name);
      
      if (obj == NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Cannot create a Function type: %s.\n"
             "Make sure %s is correct type and registered to FunctionFactory.\n",
             type.c_str(), type.c_str());
         
         return NULL;
         
         //throw GmatBaseException
         //   ("The Moderator cannot create a Function type \"" + type + "\"\n");
      }
      
      #ifdef DEBUG_MEMORY
      if (obj)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (obj, name, "Moderator::CreateFunction()", funcName);
      }
      #endif
      
      // Manage it if it is a named Function
      try
      {
         if (name != "" && manage == 1)
         {
            #if DEBUG_CREATE_FUNCTION
            MessageInterface::ShowMessage
               ("==> Adding function '%s' to configuration map\n", name.c_str());
            #endif
            theConfigManager->AddFunction(obj);
         }
         else if (currentFunction != NULL)
         {
            if (manage == 0)
            {
               #if DEBUG_CREATE_FUNCTION
               MessageInterface::ShowMessage
                  ("==> Adding function '%s' to unmanagedFunctions\n", name.c_str());
               #endif
               unmanagedFunctions.push_back(obj);
            }
            else if (manage == 2) // Function is created inside a function
            {
               #if DEBUG_CREATE_FUNCTION
               MessageInterface::ShowMessage
                  ("==> Adding function '%s' to Sandbox global object map\n", name.c_str());
               #endif
               AddFunctionToGlobalObjectMap(obj);
            }
         }
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage("In Moderator::CreateFunction()\n" +
                                       e.GetFullMessage());
      }
      
      #if DEBUG_CREATE_FUNCTION
      MessageInterface::ShowMessage
         ("Moderator::CreateFunction() returning <%p> for '%s'\n", obj,
          obj->GetName().c_str());
      #endif
      return obj;
   }
   else
   {
      #if DEBUG_CREATE_FUNCTION
      MessageInterface::ShowMessage
         ("Moderator::CreateFunction() Unable to create Function "
          "name: %s already exist\n", name.c_str());
      #endif
      
      return GetFunction(name);
   }
}


//------------------------------------------------------------------------------
// Function* GetFunction(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a function object pointer by given name.
 *
 * @param <name> object name
 *
 * @return a Function object pointer, return null if name not found
 */
//------------------------------------------------------------------------------
Function* Moderator::GetFunction(const std::string &name)
{
   if (name == "")
      return NULL;
   else
      return theConfigManager->GetFunction(name);
}


//----- Non-configurable Items

// StopCondition
//------------------------------------------------------------------------------
// StopCondition* CreateStopCondition(const std::string &type,
//                                    const std::string &name)
//------------------------------------------------------------------------------
StopCondition* Moderator::CreateStopCondition(const std::string &type,
                                              const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateStopCondition() type = '%s', "
                                 "name = '%s'\n", type.c_str(), name.c_str());
   #endif
   
   StopCondition *stopCond = theFactoryManager->CreateStopCondition(type, name);
   
   if (stopCond ==  NULL)
   {
      throw GmatBaseException
         ("The Moderator cannot create StopCondition type \"" + type + "\"\n");
   }
   
   #ifdef DEBUG_MEMORY
   if (stopCond)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (stopCond, name, "Moderator::CreateStopCondition()", funcName);
   }
   #endif
   
   return stopCond;
}


//------------------------------------------------------------------------------
// AxisSystem* CreateAxisSystem(const std::string &type,
//                              const std::string &name, Integer manage)
//------------------------------------------------------------------------------
/**
 * Creates a AxisSystem object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 * @param <manage>  0, if parameter is not managed
 *                  1, if parameter is added to configuration (default)
 *                  2, if Parameter is added to function object map
 *
 * @return a AxisSystem object pointer
 */
//------------------------------------------------------------------------------
AxisSystem* Moderator::CreateAxisSystem(const std::string &type,
                                        const std::string &name, Integer manage)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateAxisSystem() type = '%s', name = '%s', manage=%d\n",
       type.c_str(), name.c_str(), manage);
   #endif
   
   AxisSystem *axisSystem = theFactoryManager->CreateAxisSystem(type, name);
   
   if (axisSystem == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot create a AxisSystem type: %s.\n"
          "Make sure %s is correct type and registered to AxisSystemFactory.\n",
          type.c_str(), type.c_str());
      
      return NULL;
      
      //throw GmatBaseException
      //   ("The Moderator cannot create AxisSystem type \"" + type + "\"\n");
   }
   
   #ifdef DEBUG_MEMORY
   if (axisSystem)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (axisSystem, name, "Moderator::CreateAxisSystem()", funcName);
   }
   #endif
   
   // set origin and j2000body
   axisSystem->SetOrigin((SpacePoint*)FindObject(axisSystem->GetOriginName()));
   axisSystem->SetJ2000Body((SpacePoint*)FindObject(axisSystem->GetJ2000BodyName()));
   
   // Notes: AxisSystem is not configured. It is local to CoordinateSystem
   // and gets deleted when CoordinateSystem is deleted.
   
   // DJC added 5/11/05.  The ScriptInterpreter does not have the parms needed
   // to set these references, so defaults are set here.  This might need to be
   // fixed later.
   
   /// @todo Evaluate how the AxixSystem file usage really should be set
   
   // Set required internal references if they are used
   if (axisSystem->UsesEopFile() == GmatCoordinate::REQUIRED)
      axisSystem->SetEopFile(theEopFile);
   if (axisSystem->UsesItrfFile() == GmatCoordinate::REQUIRED)
      axisSystem->SetCoefficientsFile(theItrfFile);
   
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateAxisSystem() returning <%p>\n", axisSystem);
   #endif
   
   return axisSystem;
}

// MathNode
//------------------------------------------------------------------------------
// MathNode* CreateMathNode(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates a MathNode object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return a MathNode object pointer
 */
//------------------------------------------------------------------------------
MathNode* Moderator::CreateMathNode(const std::string &type,
                                    const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage("Moderator::CreateMathNode() type = '%s', "
                                 "name = '%s'\n", type.c_str(), name.c_str());
   #endif
   
   MathNode *mathNode = theFactoryManager->CreateMathNode(type, name);
   
   if (mathNode ==  NULL)
   {
      throw GmatBaseException
         ("The Moderator cannot create MathNode type \"" + type + "\"\n");
   }
   
   #ifdef DEBUG_MEMORY
   if (mathNode)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (mathNode, name, "Moderator::CreateMathNode()", funcName);
   }
   #endif
   
   return mathNode;
}

//------------------------------------------------------------------------------
// Attitude* CreateAttitude(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Creates an Attitude object by given type and name.
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return an Attitude object pointer
 */
//------------------------------------------------------------------------------
Attitude* Moderator::CreateAttitude(const std::string &type,
                                    const std::string &name)
{
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateAttitude() type = '%s', name = '%s', objectManageOption=%d\n",
       type.c_str(), name.c_str(), objectManageOption);
   #endif
   
   Attitude *att = theFactoryManager->CreateAttitude(type, name);
   
   if (att == NULL)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot create an Attitude type: %s.\n"
          "Make sure %s is correct type and registered to AttitudeFactory.\n",
          type.c_str(), type.c_str());
      
      return NULL;
   }
   
   #ifdef DEBUG_MEMORY
   if (att)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (att, name, "Moderator::CreateAttitude()", funcName);
   }
   #endif
   
   #if DEBUG_CREATE_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::CreateAttitude() returning <%p> for type = '%s'\n", att,
       type.c_str());
   #endif
   return att;
}


//GmatCommand
//------------------------------------------------------------------------------
// GmatCommand* InterpretGmatFunction(const std::string fileName)
//------------------------------------------------------------------------------
/**
 * Retrieves a function object pointer by given name.
 *
 * @param <fileName>  Full path and name of the GmatFunction file.
 *
 * @return A command list that is executed to run the function.
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::InterpretGmatFunction(const std::string &fileName)
{
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("Moderator::InterpretGmatFunction() fileName='%s'\n", fileName.c_str());
   #endif
   
   GmatCommand *cmd = NULL;
   if (fileName != "")
      cmd =  theScriptInterpreter->InterpretGmatFunction(fileName);

   // Do not reset configuration changed flag here (LOJ: 2015.10.4 GMT5374)
   // #if DEBUG_GMAT_FUNCTION
   // MessageInterface::ShowMessage
   //    ("Moderator::InterpretGmatFunction() resetting configurationchanged\n");
   // #endif
   //ResetConfigurationChanged();
   
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("Moderator::InterpretGmatFunction() returning <%p><%s>\n", cmd,
       cmd ? cmd->GetTypeName().c_str() : "NULL");
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
// GmatCommand* InterpretGmatFunction(Function *funct, ObjectMap *objMap,
//                                    SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Retrieves a function object pointer by given name.
 *
 * @param <funct>  The GmatFunction pointer
 * @param <objMap> The object map pointer to be used for finding objects
 * @param <ss>     The solar system to be used
 *
 * @return A command list that is executed to run the function.
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::InterpretGmatFunction(Function *funct, ObjectMap *objMap,
                                              SolarSystem *ss)
{
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("Moderator::InterpretGmatFunction() function=<%p>, objMap=<%p>\n",
       funct, objMap);
   #endif
   
   currentFunction = funct;
   
   // If input objMap is NULL, use configured objects
   // use input object map otherwise   
   if (objMap == NULL)
   {
      objectMapInUse = theConfigManager->GetObjectMap();
      #ifdef DEBUG_OBJECT_MAP
      MessageInterface::ShowMessage
         ("Moderator::InterpretGmatFunction() objectMapInUse was set to the "
          "configuration map <%p>\n", objectMapInUse);
      #endif
   }
   else
   {
      objectMapInUse = objMap;
      #ifdef DEBUG_OBJECT_MAP
      MessageInterface::ShowMessage
         ("Moderator::InterpretGmatFunction() objectMapInUse was set to "
          "input objMap <%p>\n", objectMapInUse);
      #endif
   }
   
   // If input SolarSystem is NULL, use default SolarSystemInUse
   // use input SolarSystem otherwise      
   SolarSystem *solarSystemInUse = GetSolarSystemInUse();
   if (ss != NULL)
   {
      solarSystemInUse = ss;
      theInternalSolarSystem = ss;
   }
   
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("   Setting objectMapInUse<%p> to theScriptInterpreter\n"
       "   Setting theSolarSystemInUse<%p> to theScriptInterpreter\n",
       objectMapInUse, solarSystemInUse);
   #endif
   
   #if DEBUG_GMAT_FUNCTION > 1
   ShowObjectMap("Moderator::InterpretGmatFunction() Here is the object map in use");
   #endif
   
   // Set solar system in use and object map for GmatFunction
   SetSolarSystemAndObjectMap(solarSystemInUse, objectMapInUse, true,
                              "InterpretGmatFunction()");
   
   GmatCommand *cmd = NULL;
   cmd = theScriptInterpreter->InterpretGmatFunction(funct);
   
   // Do not reset configuration changed flag here (LOJ: 2015.10.4 GMT5374)
   // #if DEBUG_GMAT_FUNCTION
   // MessageInterface::ShowMessage
   //    ("Moderator::InterpretGmatFunction() resetting configurationchanged\n");
   // #endif
   //ResetConfigurationChanged();
   
   // reset current function to NULL
   currentFunction = NULL;
   
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("Moderator::InterpretGmatFunction() returning <%p><%s>\n", cmd,
       cmd ? cmd->GetTypeName().c_str() : "NULL");
   #endif
   
   return cmd;
} // InterpretGmatFunction()


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &type, const std::string &name
//                            bool &retFlag)
//------------------------------------------------------------------------------
/**
 * Creates command from the factory
 *
 * @param  type  The command type name such as Propagate, Maneuver
 * @param  name  The name of the command, usally blank
 * @param  retFlag  The return flag
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::CreateCommand(const std::string &type,
                                      const std::string &name, bool &retFlag)
{
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("Moderator::CreateCommand() entered: type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
   
   if (cmd == NULL)
   {
      //MessageInterface::PopupMessage
      //   (Gmat::ERROR_, "Cannot create a Command type: %s.\n"
      //    "Make sure %s is correct type and registered to Commandactory.\n",
      //    type.c_str(), type.c_str());
      
      throw GmatBaseException
         ("The Moderator cannot create a Command type \"" + type + "\"\n");
   }
   
   #ifdef DEBUG_MEMORY
   if (cmd)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (cmd, type, "Moderator::CreateCommand()", funcName);
   }
   #endif
   
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("Moderator::CreateCommand() returning <%p><%s>, retFlag=%d\n", cmd,
       cmd->GetTypeName().c_str(), retFlag);
   #endif
   
   retFlag = true;
   return cmd;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateDefaultCommand(const std::string &type,
//                                   const std::string &name,
//                                   GmatCommand *refCmd = NULL)
//------------------------------------------------------------------------------
/*
 * Creates a command with default settings. The input refCmd is only used for
 * EndFiniteBurn to match with BeginFiniteBurn. This method is usually called
 * from the GUI.
 *
 * @param  type  Command type
 * @param  name  Command name
 * @param  refCmd  Referenced command name
 *
 * @return  New command pointer
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::CreateDefaultCommand(const std::string &type,
                                             const std::string &name,
                                             GmatCommand *refCmd)
{
   #if DEBUG_DEFAULT_COMMAND
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultCommand() entered: type = '%s', name = '%s'\n",
       type.c_str(), name.c_str());
   ShowCommand("   refCmd = ", refCmd);
   #endif
   
   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
   
   if (cmd == NULL)
      throw GmatBaseException
         ("The Moderator cannot create a Command type \"" + type + "\"\n");
   
   #ifdef DEBUG_MEMORY
   if (cmd)
   {
      std::string funcName;
      funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
      MemoryTracker::Instance()->Add
         (cmd, type, "Moderator::CreateDefaultCommand()", funcName);
   }
   #endif
   
   Integer id;
   
   try
   {
      if (type == "If" || type == "While")
      {
         std::string str = GetDefaultSpacecraft()->GetName() + ".ElapsedDays";
         cmd->SetCondition(str, "<", "1.0");
      }
      else if (type == "For")
      {
         CreateParameter("Variable", "I");
         cmd->SetStringParameter("IndexName", "I");
         cmd->SetStringParameter("StartName", "1");
         cmd->SetStringParameter("EndName", "10");
      }
      else if (type == "Save" || type == "Global")
      {
         cmd->SetStringParameter("ObjectNames", GetDefaultSpacecraft()->GetName());
      }
      else if (type == "ClearPlot" || type == "MarkPoint")
      {
         // Set default XYPlot, create default XYPlot if it is not found
         Subscriber *defSub = GetDefaultSubscriber("XYPlot", false, true);
         if (defSub != NULL)
            cmd->SetStringParameter(cmd->GetParameterID("Subscriber"),
                                    defSub->GetName(), 0);
      }
      else if (type == "PenUp" || type == "PenDown")
      {
         Subscriber *defSub = GetDefaultSubscriber("PenUpDownSubscriber", false, false);
         cmd->SetStringParameter(cmd->GetParameterID("Subscriber"),
                                 defSub->GetName(), 0);
      }
      else if (type == "Toggle")
      {
         Subscriber *defSub = GetDefaultSubscriber("ToggleSubscriber", false, false);
         cmd->SetStringParameter(cmd->GetParameterID("Subscriber"),
                                 defSub->GetName());
      }
      else if (type == "Report")
      {
         Subscriber *sub = GetDefaultSubscriber("ReportFile", false);
         Parameter *param = GetDefaultX();
         cmd->SetStringParameter("ReportFile", sub->GetName());
         cmd->SetStringParameter("Add", param->GetName());
         cmd->SetRefObject(sub, Gmat::SUBSCRIBER, sub->GetName(), 0);
         cmd->SetRefObject(param, Gmat::PARAMETER, param->GetName(), 0);
      }
      else if (type == "Propagate")
      {
         cmd->SetObject(GetDefaultPropSetup()->GetName(), Gmat::PROP_SETUP);
         
         StringArray formList = GetListOfObjects(Gmat::FORMATION);
         
         if (formList.size() == 0)
         {
            cmd->SetObject(GetDefaultSpacecraft()->GetName(), Gmat::SPACECRAFT);
         }
         else
         {
            // Get first spacecraft name not in formation
            std::string scName = GetSpacecraftNotInFormation();
            if (scName != "")
               cmd->SetObject(scName, Gmat::SPACECRAFT);
            else
               cmd->SetObject(formList[0], Gmat::SPACECRAFT);
         }
         
         cmd->SetRefObject(CreateDefaultStopCondition(), Gmat::STOP_CONDITION, "", 0);
         cmd->SetSolarSystem(theSolarSystemInUse);
      }
      else if (type == "Maneuver")
      {
         // set burn
         id = cmd->GetParameterID("Burn");
         cmd->SetStringParameter(id, GetDefaultBurn("ImpulsiveBurn")->GetName());
         
         // set spacecraft
         id = cmd->GetParameterID("Spacecraft");
         cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName());
      }
      else if (type == "BeginFiniteBurn")
      {
         // set burn
         cmd->SetRefObjectName(Gmat::FINITE_BURN, GetDefaultBurn("FiniteBurn")->GetName());
         
         // set spacecraft
         cmd->SetRefObjectName(Gmat::SPACECRAFT, GetDefaultSpacecraft()->GetName());
      }
      else if (type == "EndFiniteBurn")
      {
         // get burn name of BeginFiniteBurn
         if (refCmd && refCmd->IsOfType("BeginFiniteBurn"))
         {
            // set burn
            cmd->SetRefObjectName(Gmat::FINITE_BURN,
                                  refCmd->GetRefObjectName(Gmat::FINITE_BURN));
            
            // set spacecraft
            StringArray scNames = refCmd->GetRefObjectNameArray(Gmat::SPACECRAFT);
            for (UnsignedInt i=0; i<scNames.size(); i++)
               cmd->SetRefObjectName(Gmat::SPACECRAFT, scNames[i]);
         }
         else
         {
            // set burn
            cmd->SetRefObjectName(Gmat::FINITE_BURN, GetDefaultBurn("FiniteBurn")->GetName());
            
            // set spacecraft
            cmd->SetRefObjectName(Gmat::SPACECRAFT, GetDefaultSpacecraft()->GetName());
         }
      }
      else if (type == "Target")
      {
         // set DifferentCorrector
         Solver *solver = GetDefaultBoundaryValueSolver();
         id = cmd->GetParameterID("Targeter");
         cmd->SetStringParameter(id, solver->GetName());
      }
      else if (type == "Optimize")
      {
         // set Optimizer
         Solver *solver = GetDefaultOptimizer();
         id = cmd->GetParameterID("OptimizerName");
         cmd->SetStringParameter(id, solver->GetName());
      }
      else if (type == "Vary")
      {
         Solver *solver = GetDefaultBoundaryValueSolver();
         // if refCmd is Optimize, set Optimizer, leave DifferentialCorrect otherwise
         if (refCmd && refCmd->IsOfType("Optimize"))
            solver = GetDefaultOptimizer();
         
         id = cmd->GetParameterID("SolverName");
         cmd->SetStringParameter(id, solver->GetName());
         
         // set sover pointer, so that GetGeneratingString() can write correctly
         cmd->SetRefObject(solver, Gmat::SOLVER);
         
         // set variable parameter
         id = cmd->GetParameterID("Variable");
         cmd->SetStringParameter(id, GetDefaultBurn("ImpulsiveBurn")->GetName() + ".Element1");
         
         id = cmd->GetParameterID("InitialValue");
         cmd->SetStringParameter(id, "0.5");
         
         id = cmd->GetParameterID("Perturbation");
         cmd->SetStringParameter(id, "0.0001");
         
         id = cmd->GetParameterID("Lower");
         cmd->SetStringParameter(id, "0.0");
         
         id = cmd->GetParameterID("Upper");
         std::stringstream ss("");
         ss << GmatMathConstants::PI;
         cmd->SetStringParameter(id, ss.str());
      
         id = cmd->GetParameterID("MaxStep");
         cmd->SetStringParameter(id, "0.2");
         
      }
      else if (type == "Achieve")
      {
         // Get BoundaryValueSolver as default solver
         Solver *solver = GetDefaultBoundaryValueSolver();
         
         #if DEBUG_DEFAULT_COMMAND
         MessageInterface::ShowMessage
            ("Moderator::CreateDefaultCommand() cmd=%s, solver=%s\n",
             cmd->GetTypeName().c_str(), solver->GetTypeName().c_str());
         #endif
         
         id = cmd->GetParameterID("TargeterName");
         cmd->SetStringParameter(id, solver->GetName());
         
         // set goal parameter
         id = cmd->GetParameterID("Goal");
         cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName() + ".Earth.RMAG");
         
         id = cmd->GetParameterID("GoalValue");
         cmd->SetStringParameter(id, "42165.0"); 
         
         id = cmd->GetParameterID("Tolerance");
         cmd->SetStringParameter(id, "0.1");
      }
      else if (type == "Minimize")
      {
         // Get Optimizer as default solver
         Solver *solver = GetDefaultOptimizer();
         
         #if DEBUG_DEFAULT_COMMAND
         MessageInterface::ShowMessage
            ("Moderator::CreateDefaultCommand() cmd=%s, solver=%s\n",
             cmd->GetTypeName().c_str(), solver->GetTypeName().c_str());
         #endif
         
         id = cmd->GetParameterID("OptimizerName");
         cmd->SetStringParameter(id, solver->GetName());
         
         // Set spacecraft RMAG as default ObjectiveName
         id = cmd->GetParameterID("ObjectiveName");
         cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName() + ".Earth.RMAG");
      }
      else if (type == "NonlinearConstraint")
      {
         // Set default value: DefaultSC.SMA = 7000 (for bug 2045 fix)
         
         // Get Optimizer as default solver
         Solver *solver = GetDefaultOptimizer();
         
         #if DEBUG_DEFAULT_COMMAND
         MessageInterface::ShowMessage
            ("Moderator::CreateDefaultCommand() cmd=%s, solver=%s\n",
             cmd->GetTypeName().c_str(), solver->GetTypeName().c_str());
         #endif
         
         id = cmd->GetParameterID("OptimizerName");
         cmd->SetStringParameter(id, solver->GetName());
         
         // set constraint parameter
         id = cmd->GetParameterID("ConstraintArg1");
         cmd->SetStringParameter(id, GetDefaultSpacecraft()->GetName() + ".SMA");
         id = cmd->GetParameterID("Operator");
         cmd->SetStringParameter(id, "=");          
         id = cmd->GetParameterID("ConstraintArg2");
         cmd->SetStringParameter(id, "7000"); 
      }
      else if (type == "FindEvents")
      {
         id = cmd->GetParameterID("EventLocator");
         EventLocator *defaultLocator = GetDefaultEventLocator();
         if (!defaultLocator)
         {
            // can't create new one because can't count on having
            // the plugins available
            cmd->SetStringParameter(id, "No Event Locators");
         }
         else
         {
            cmd->SetStringParameter(id, defaultLocator->GetName());
         }
      }
      else
      {
         // We need to set actual command string so that it can be saved to script
         std::string typeName = cmd->GetTypeName();
         std::string genStr = cmd->GetGeneratingString();
         // If there is comment only, prepend command string
         if (GmatStringUtil::StartsWith(genStr, "%"))
            cmd->SetGeneratingString(typeName + "; " + genStr);
      }
      
      // for creating ElementWrapper
      #if DEBUG_DEFAULT_COMMAND
      MessageInterface::ShowMessage
         ("   Moderator calling theScriptInterpreter->ValidateCommand()\n");
      #endif
      theScriptInterpreter->ValidateCommand(cmd);
      
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage("In Moderator::CreateDefaultCommand()\n" + e.GetFullMessage());
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #if DEBUG_DEFAULT_COMMAND
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultCommand() returning cmd=<%p><%s>\n", cmd,
       cmd->GetTypeName().c_str());
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
// bool AppendCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Appends command to last command
 *
 * @param  cmd  The command pointer to be appended
 * @param  sandboxNum  The sandbox number
 * @return  Return true if command successfully appended
 */
//------------------------------------------------------------------------------
bool Moderator::AppendCommand(GmatCommand *cmd, Integer sandboxNum)
{
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("==========> Moderator::AppendCommand(*cmd) entered, cmd=<%p><%s>'%s'\n",
       cmd, cmd->GetTypeName().c_str(), cmd->GetName().c_str());
   #endif
   
   // Get last command and append
   GmatCommand *lastCmd = GmatCommandUtil::GetLastCommand(commands[sandboxNum-1]);
   
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("     lastCmd=<%p><%s>'%s'\n", lastCmd, lastCmd->GetTypeName().c_str(),
       lastCmd->GetName().c_str());
   #endif
   
   bool retflag = false;
   
   if (lastCmd != NULL)
      retflag = lastCmd->Append(cmd);
   else
      retflag = commands[sandboxNum-1]->Append(cmd);
   
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("==========> Moderator::AppendCommand(*cmd) returning %s, cmd=<%p><%s>'%s'\n",
       retflag ? "true" : "false", cmd, cmd->GetTypeName().c_str(),
       cmd->GetName().c_str());
   #endif
   
   return retflag;
}


//------------------------------------------------------------------------------
// GmatCommand* AppendCommand(const std::string &type, const std::string &name,
//                           bool &retFlag, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Creates a command of type and appends to the last command in the sequence.
 *
 * @param  type  The type of the command to be created
 * @param  name  The name of the command to be created
 * @param  retFlag  The return flag
 * @param  sandboxNum  The sandbox number
 *
 * @ruturn  Returns the new command just created
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::AppendCommand(const std::string &type,
                                      const std::string &name, bool &retFlag,
                                      Integer sandboxNum)
{
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("==========> Moderator::AppendCommand(type) entered, type='%s', name='%s'\n",
       type.c_str(), name.c_str());
   #endif
   
   GmatCommand *cmd = theFactoryManager->CreateCommand(type, name);
   
   if (cmd != NULL)
   {
      #ifdef DEBUG_MEMORY
      if (cmd)
      {
         std::string funcName;
         funcName = currentFunction ? "function: " + currentFunction->GetName() : "";
         MemoryTracker::Instance()->Add
            (cmd, type, "Moderator::AppendCommand()", funcName);
      }
      #endif
      
      retFlag = AppendCommand(cmd, sandboxNum);
   }
   else
   {
      throw GmatBaseException
         ("The Moderator cannot create a Command type \"" + type + "\"\n");
   }
   
   #if DEBUG_CREATE_COMMAND
   MessageInterface::ShowMessage
      ("==========> Moderator::AppendCommand(type) returning <%p><%s>'%s', retFlag=%d\n",
       cmd, cmd->GetTypeName().c_str(), cmd->GetName().c_str(), retFlag);
   #endif
   
   return cmd;
}


//------------------------------------------------------------------------------
// bool InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Inserts command into the command sequence after previous command
 * 
 * @param cmd  Pointer to GmatCommand that is inserted
 * @param prev Pointer to GmatCommand preceding this GmatCommand
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Moderator::InsertCommand(GmatCommand *cmd, GmatCommand *prevCmd,
                              Integer sandboxNum)
{
   #if DEBUG_COMMAND_INSERT
   MessageInterface::ShowMessage("==========> Moderator::InsertCommand() entered\n");
   ShowCommand("     inserting cmd = ", cmd, " after prevCmd = ", prevCmd);
   #endif
   
   return commands[sandboxNum-1]->Insert(cmd, prevCmd);
}


//------------------------------------------------------------------------------
// GmatCommand* DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
//------------------------------------------------------------------------------
/*
 * Removes a command from the command sequence. The caller has to delete the command.
 *
 * If deleting branch command,
 * it will remove and delete all children from the branch. If deleting ScriptEvent,
 * it will remove and delete all commands including EndScrpt between BeginScrint
 * and EndScript.
 */
//------------------------------------------------------------------------------
GmatCommand* Moderator::DeleteCommand(GmatCommand *cmd, Integer sandboxNum)
{
   #if DEBUG_COMMAND_DELETE
   ShowCommand("==========> Moderator::DeleteCommand() cmd = ", cmd);
   #endif
   
   if (cmd == NULL)
      return NULL;

   GmatCommand *first = GetFirstCommand();
   GmatCommand *remvCmd = GmatCommandUtil::RemoveCommand(first, cmd);
   
   #if DEBUG_COMMAND_DELETE
   std::string cmdString2 = GmatCommandUtil::GetCommandSeqString(first);
   MessageInterface::ShowMessage("     ==> sequence after delete:");
   MessageInterface::ShowMessage(cmdString2);
   ShowCommand("==========> Moderator::DeleteCommand() Returning remvCmd = ", remvCmd);
   #endif
   return remvCmd;
}


//------------------------------------------------------------------------------
// GmatCommand* GetFirstCommand(Integer sandboxNum)
//------------------------------------------------------------------------------
GmatCommand* Moderator::GetFirstCommand(Integer sandboxNum)
{
   if (commands.empty())
   {
      return NULL;
   }
   return commands[sandboxNum-1];
}


//------------------------------------------------------------------------------
// void SetCommandsUnchanged(Integer whichList)
//------------------------------------------------------------------------------
/**
 * Resets the command list to the unchanged state.
 * 
 * @param <whichList>   index indicating which command list gets updated
 */
//------------------------------------------------------------------------------
void Moderator::SetCommandsUnchanged(Integer whichList)
{
   GmatCommandUtil::ResetCommandSequenceChanged(commands[whichList]);
}


//------------------------------------------------------------------------------
// bool UpdateDataFiles()
//------------------------------------------------------------------------------
/**
 * Validates the command.
 */
//------------------------------------------------------------------------------
bool Moderator::UpdateDataFiles()
{
   std::string file = theFileManager->GetStartupFileDir()+IFileUpdater::ShellFile;

   // no update file, continue with running GMAT
   if (!GmatFileUtil::DoesFileExist(file)) return false;

   bool retValue;
   try
   {
      errno = 0;
      int ret_value = system(file.c_str());
      retValue = (ret_value == 0 && errno == 0);
      if (retValue)
      {
         MessageInterface::PopupMessage
            (Gmat::INFO_, "Data Files have been successfully updated");
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "Data Files were not successfully updated");
      }
   }
   catch (...)
   {
   }
   Integer retCode;

   if (GmatFileUtil::DoesFileExist(file + ".bak"))
      remove((file + ".bak").c_str());
   theFileManager->RenameFile(file, file + ".bak", retCode, true);
   return retValue;
}


//------------------------------------------------------------------------------
// void ValidateCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Validates the command.
 */
//------------------------------------------------------------------------------
void Moderator::ValidateCommand(GmatCommand *cmd)
{
   theScriptInterpreter->ValidateCommand(cmd);
}


// CoordinateSystem
//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordinateSystem()
//------------------------------------------------------------------------------
/**
 * @return internal CoordinateSystem.
 */
//------------------------------------------------------------------------------
CoordinateSystem* Moderator::GetInternalCoordinateSystem()
{
   return theInternalCoordSystem;
}

//Planetary files
//------------------------------------------------------------------------------
// const StringArray& GetPlanetarySourceTypes()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetPlanetarySourceTypes()
{
   return theSolarSystemInUse->GetPlanetarySourceTypes();
}


//------------------------------------------------------------------------------
// const StringArray& GetPlanetarySourceNames()
//------------------------------------------------------------------------------
/**
 * @return a planetary source file names
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetPlanetarySourceNames()
{
   return theSolarSystemInUse->GetPlanetarySourceNames();
}


//------------------------------------------------------------------------------
// const StringArray& GetPlanetarySourceTypesInUse()
//------------------------------------------------------------------------------
/**
 * @return a planetary source types in use
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetPlanetarySourceTypesInUse()
{
   return theSolarSystemInUse->GetPlanetarySourceTypesInUse();
}


////------------------------------------------------------------------------------
//// StringArray& GetAnalyticModelNames()
////------------------------------------------------------------------------------
///**
// * @return available planetary analytic model names.
// */
////------------------------------------------------------------------------------
//const StringArray& Moderator::GetAnalyticModelNames()
//{
//   return theSolarSystemInUse->GetAnalyticModelNames();
//}
//
//
////------------------------------------------------------------------------------
//// bool SetAnalyticModelToUse(const std::string &modelName)
////------------------------------------------------------------------------------
//bool Moderator::SetAnalyticModelToUse(const std::string &modelName)
//{
//   return theSolarSystemInUse->SetAnalyticModelToUse(modelName);
//}


//------------------------------------------------------------------------------
// bool SetPlanetarySourceName(const std::string &sourceType,
//                           const std::string &fileName)
//------------------------------------------------------------------------------
bool Moderator::SetPlanetarySourceName(const std::string &sourceType,
                                       const std::string &fileName)
{
   return theSolarSystemInUse->SetPlanetarySourceName(sourceType, fileName);
}


//------------------------------------------------------------------------------
// std::string GetPlanetarySourceName(const std::string &sourceType)
//------------------------------------------------------------------------------
std::string Moderator::GetPlanetarySourceName(const std::string &sourceType)
{
   return theSolarSystemInUse->GetPlanetarySourceName(sourceType);
}


//------------------------------------------------------------------------------
// Integer SetPlanetarySourceTypesInUse(const StringArray &sourceTypes)
//------------------------------------------------------------------------------
/*
 * @param <sourceTypes> list of file type in the priority order of use
 *
 * @return 0, if error setting any of planetary file in the list.
 *         1, if error setting first planetary file in the list, but set to
 *            next available file.
 *         2, if successfuly set to first planetary file in the list
 */
//------------------------------------------------------------------------------
Integer Moderator::SetPlanetarySourceTypesInUse(const StringArray &sourceTypes)
{
   return theSolarSystemInUse->SetPlanetarySourceTypesInUse(sourceTypes);
}


//------------------------------------------------------------------------------
// Integer GetPlanetarySourceId(const std::string &sourceType)
//------------------------------------------------------------------------------
Integer Moderator::GetPlanetarySourceId(const std::string &sourceType)
{
   return theSolarSystemInUse->GetPlanetarySourceId(sourceType);
}


//------------------------------------------------------------------------------
// std::string GetPlanetarySourceNameInUse()
//------------------------------------------------------------------------------
std::string Moderator::GetCurrentPlanetarySource()
{
   return theSolarSystemInUse->GetCurrentPlanetarySource();
}


// Potential field files
//------------------------------------------------------------------------------
// std::string GetPotentialFileName(const std::string &fileType)
//------------------------------------------------------------------------------
std::string Moderator::GetPotentialFileName(const std::string &fileType)
{
   if (fileType == "JGM2")
      return theFileManager->GetFullPathname("JGM2_FILE");
   else if (fileType == "JGM3")
      return theFileManager->GetFullPathname("JGM3_FILE");
   else if (fileType == "EGM96")
      return theFileManager->GetFullPathname("EGM96_FILE");
   else if (fileType == "LP165P")
      return theFileManager->GetFullPathname("LP165P_FILE");
   else if (fileType == "MGNP180U")
      return theFileManager->GetFullPathname("MGNP180U_FILE");
   else if (fileType == "MARS50C")
      return theFileManager->GetFullPathname("MARS50C_FILE");
   else
      return "Unknown Potential File Type:" + fileType;
}


//------------------------------------------------------------------------------
// std::string GetFileName(const std::string &fileType, bool getFullPath = false,
//                         bool forInput = true)
//------------------------------------------------------------------------------
std::string Moderator::GetFileName(const std::string &fileType, bool getFullPath,
                                   bool forInput, bool writeWarning, bool writeInfo)
{
   // Now we can get full path or just file name (LOJ: 2014.06.30)
   if (getFullPath)
   {
      return theFileManager->FindPath("", fileType, forInput, writeWarning, writeInfo);
   }
   else
   {
      // Changed to use FileManager::GetFileName() (LOJ: 2014.06.30)
      //return theFileManager->GetFullPathname(fileType);
      return theFileManager->GetFilename(fileType);
   }
}


// Mission
//------------------------------------------------------------------------------
// bool LoadDefaultMission()
//------------------------------------------------------------------------------
bool Moderator::LoadDefaultMission()
{
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("Moderator::LoadDefaultMission() entered\n");
   #endif
   
   theScriptInterpreter->SetHeaderComment("");
   theScriptInterpreter->SetFooterComment("");
   
   ClearCommandSeq(true, true);
   ClearResource();
   
   // Set object manage option to configuration
   objectManageOption = 1;
   
   CreateDefaultMission();
   
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("Moderator::LoadDefaultMission() leaving\n");
   #endif
   
   return true;
}

// Resource
//------------------------------------------------------------------------------
// bool ClearResource()
//------------------------------------------------------------------------------
bool Moderator::ClearResource()
{
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage("Moderator::ClearResource() entered\n");
   MessageInterface::ShowMessage("   Removing configured objects...\n");
   #endif
   
   theConfigManager->RemoveAllItems();
   
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage("   Clearing Sandbox objects...\n");
   #endif
   
   ClearAllSandboxes();
   
   // Delete solar system in use. We want to begin with default solar system
   // before creating default mission or new script is read.
   #ifndef __DISABLE_SOLAR_SYSTEM_CLONING__
   // Do not delete SolarSystem in case user wants to create a new mission from
   // the GUI after getting errors in script build. This will fix Bug 1532 (LOJ: 2009.11.13)
   if (!isRunReady && endOfInterpreter)
   {
      #if DEBUG_SEQUENCE_CLEARING | DEBUG_FINALIZE > 0
      MessageInterface::ShowMessage
         (".....Mod::ClearResource - <%p>theSolarSystemInUse was not deleted since "
          "there was script errors\n", theSolarSystemInUse);
      #endif
   }
   else
   {
      if (theSolarSystemInUse != NULL)
      {
         #if DEBUG_SEQUENCE_CLEARING | DEBUG_FINALIZE > 0
         MessageInterface::ShowMessage
            (".....Mod::ClearResource - deleting (%p)theSolarSystemInUse\n", theSolarSystemInUse);
         #endif
         if (theInternalSolarSystem == theSolarSystemInUse) theInternalSolarSystem = NULL;
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (theSolarSystemInUse, theSolarSystemInUse->GetName(),
             "deleting theSolarSystemInUse in Moderator::ClearResource()");
         #endif
         
         delete theSolarSystemInUse;
         theSolarSystemInUse = NULL;
      }
   }
   #endif
   
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage("Moderator::ClearResource() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool LoadMinimumResource()
//------------------------------------------------------------------------------
bool Moderator::LoadMinimumResource()
{
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage("Moderator::LoadMinimumResource() entered\n");
   #endif
   
   theScriptInterpreter->SetHeaderComment("");
   theScriptInterpreter->SetFooterComment("");
   
   ClearCommandSeq(true, true);
   ClearResource();
   
   // Set object manage option to configuration
   objectManageOption = 1;
   
   CreateMinimumResource();
   
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("Moderator::LoadMinimumResource() leaving\n");
   #endif
   
   return true;
}


// Command Sequence
//------------------------------------------------------------------------------
// bool ClearCommandSeq(bool leaveFirstCmd, bool callRunComplete,
//                     Integer sandboxNum)
//------------------------------------------------------------------------------
/*
 * Deletes whole command sequence.
 *
 * @param  leaveFirstCmd  Set this flag to true if the first command should be
 *                        left undeleted (true)
 * @param  callRunComplete  Set this flag to true if RunComplete() should be
 *                        called for all commands (true)
 * @param  sandBoxNum  The sandobx number (1)
 */
//------------------------------------------------------------------------------
bool Moderator::ClearCommandSeq(bool leaveFirstCmd, bool callRunComplete,
                                Integer sandboxNum)
{
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage("Moderator::ClearCommandSeq() entered\n");
   #endif
   
   if (commands.empty())
   {
      #if DEBUG_SEQUENCE_CLEARING
      MessageInterface::ShowMessage
         ("Moderator::ClearCommandSeq() exiting, command array is empty\n");
      #endif
      return true;
   }
   
   GmatCommand *cmd = commands[sandboxNum-1];
   bool retval =
      GmatCommandUtil::ClearCommandSeq(cmd, leaveFirstCmd, callRunComplete);
   
   #if DEBUG_SEQUENCE_CLEARING
   MessageInterface::ShowMessage
      ("Moderator::ClearCommandSeq() returning %s\n", retval ? "true" : "false");
   #endif
   
   return retval;
}


// sandbox
//------------------------------------------------------------------------------
// void ClearAllSandboxes()
//------------------------------------------------------------------------------
void Moderator::ClearAllSandboxes()
{
   for (UnsignedInt i=0; i<sandboxes.size(); i++)
      if (sandboxes[i])
         sandboxes[i]->Clear();
   
   #ifdef DEBUG_MEMORY
   StringArray tracks = MemoryTracker::Instance()->GetTracks(false, false);
   MessageInterface::ShowMessage
      ("===> There are %d memory tracks after Sandbox clear\n", tracks.size());
   #endif
}


//------------------------------------------------------------------------------
// Sandbox* GetSandbox(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
Sandbox* Moderator::GetSandbox(Integer sandboxNum)
{
   return sandboxes[sandboxNum-1];
}


//------------------------------------------------------------------------------
// GmatBase* GetInternalObject(const std::string &name, Integer sandboxNum = 1)
//------------------------------------------------------------------------------
GmatBase* Moderator::GetInternalObject(const std::string &name, Integer sandboxNum)
{
   return sandboxes[sandboxNum-1]->GetInternalObject(name);
}


//------------------------------------------------------------------------------
// Integer RunMission(Integer sandboxNum)
//------------------------------------------------------------------------------
/*
 * Adds configured objects to sandbox and execute. The number of sandbox created
 * is declared in ther header as Gmat::MAX_SANDBOX. But currently only 1 sandbox
 * is used for running the mission.
 *
 * @param  sandboxNum  The sandbox number (1 to Gmat::MAX_SANDBOX)
 *
 * @return  1 if run was successful
 *         -1 if sandbox number is invalid
 *         -2 if exception thrown during sandbox initialization
 *         -3 if unknown error occurred during sandbox initialization
 *         -4 if execution interrupted by user
 *         -5 if exception thrown during the sandbox execution
 *         -6 if unknown error occurred during sandbox execution
 */
//------------------------------------------------------------------------------
Integer Moderator::RunMission(Integer sandboxNum)
{
   //MessageInterface::ShowMessage("\n========================================\n");
   //MessageInterface::ShowMessage("Moderator::RunMission() entered\n");
   MessageInterface::ShowMessage("Running mission...\n");
   Integer status = 1;
   // Set to 1 to always run the mission and get the sandbox error message
   // Changed this code while looking at Bug 1532 (LOJ: 2009.11.13)
   isRunReady = true;
   currentSandboxNumber = sandboxNum;
   
   #if DEBUG_CONFIG
   MessageInterface::ShowMessage
      ("Moderator::RunMission() HasConfigurationChanged()=%d\n", HasConfigurationChanged());
   #endif
   
//   clock_t t1 = clock(); // Should I clock after initilization?
   //Retrieve the system time. This result is UTCGregorian, but not in the accepted
   //GMAT format.
   std::chrono::system_clock::time_point time1 = std::chrono::system_clock::now();
   long int millis1 =
         std::chrono::duration_cast<std::chrono::milliseconds>
         (time1.time_since_epoch()).count();

   #ifdef DEBUG_TIME_SYSTEM
      // What is the epoch time?
      std::chrono::time_point<std::chrono::system_clock> p1;
      std::time_t epoch_time = std::chrono::system_clock::to_time_t(p1);
      std::string epstr = std::ctime(&epoch_time);
   #endif

   if (isRunReady)
   {
      // clear sandbox
      if (sandboxNum > 0 && sandboxNum <= Gmat::MAX_SANDBOX)
      {
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() before sandboxes[%d]->Clear()\n", sandboxNum-1);
         #endif
         
         sandboxes[sandboxNum-1]->Clear();
         
         if (pCreateWidget)
            sandboxes[sandboxNum-1]->SetWidgetCreator(pCreateWidget);
         
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after sandboxes[%d]->Clear()\n", sandboxNum-1);
         #endif
      }
      else
      {
         status = -1;
         MessageInterface::PopupMessage(Gmat::ERROR_,
                                        "Invalid Sandbox number " +
                                        std::to_string(sandboxNum));
         return status;
      }
		
		//--------------------------------------------------------------
		// Initialize sandbox
		//--------------------------------------------------------------
      try
      {
         // add objects to sandbox
         AddSolarSystemToSandbox(sandboxNum-1);
         AddTriggerManagersToSandbox(sandboxNum-1);
         AddInternalCoordSystemToSandbox(sandboxNum-1);
         AddPublisherToSandbox(sandboxNum-1);
         AddSubscriberToSandbox(sandboxNum-1);
         AddOtherObjectsToSandbox(sandboxNum-1);
         
         // add command sequence to sandbox
         AddCommandToSandbox(sandboxNum-1);
         
         #if DEBUG_RUN
         MessageInterface::ShowMessage
            ("Moderator::RunMission() after AddCommandToSandbox()\n");
         #endif
         
         // initialize Sandbox
         InitializeSandbox(sandboxNum-1);
		}
      catch (BaseException &e)
      {
			status = -2;
         std::string msg = e.GetFullMessage();
			MessageInterface::PopupMessage(Gmat::ERROR_, msg + "\n");
			isRunReady = false;
		}
		#ifdef DEBUG_SHOW_SYSTEM_EXCEPTIONS
      catch (const std::exception& ex) 
      {
         status = -3;
         MessageInterface::ShowMessage("**** ERROR **** Moderator caught a "
              "system level exception:\n    %s\nduring Sandbox initialization\n",
              ex.what());
         isRunReady = false;
      }
      #endif
		catch (...)
		{
			status = -3;
         MessageInterface::ShowMessage
            ("**** ERROR **** Moderator caught an unknown error during Sandbox "
				 "Initialization.\n");
			isRunReady = false;
		}
		
		
		//--------------------------------------------------------------
		// Execute sandbox
		//--------------------------------------------------------------
		if (isRunReady)
		{
		try
		{
         if (!loadSandboxAndPause)
         {
            #if DEBUG_RUN
            MessageInterface::ShowMessage
               ("Moderator::RunMission() after InitializeSandbox()\n");
            #endif

            // reset user interrupt flag
            GmatGlobal::Instance()->SetRunInterrupted(false);

            // execute sandbox
            runState = Gmat::RUNNING;
            ExecuteSandbox(sandboxNum-1);

            #if DEBUG_RUN
            MessageInterface::ShowMessage
               ("Moderator::RunMission() after ExecuteSandbox()\n");
            #endif
         }
         else
         {
            // Execute only the PrepareMissionSequence command
            GmatCommand *cmd = commands[sandboxNum-1]->GetNext();
            if (cmd->GetTypeName() == "PrepareMissionSequence")
               cmd->Execute();
         }
      }
      catch (BaseException &e)
      {
         std::string msg = e.GetFullMessage();
         
         // assign status
         // Look for "interrupted" (loj: 2008.02.05)
         //if (msg.find("Execution interrupted") != msg.npos)
         if (msg.find("interrupted") != msg.npos)
         {
            status = -4;
            MessageInterface::ShowMessage("GMAT execution stopped by user.\n");      
         }
         else
         {

            status = -5;
            // msg = "**** ERROR **** " + msg;
            // Dunn would like to note that this is the popup message we were
            // getting that only said "ERROR" and did not provide a message.
            // We might want to debug that some day.
            MessageInterface::PopupMessage(Gmat::ERROR_, msg + "\n");
         }
      }
      catch (...)
      {
         MessageInterface::ShowMessage
            ("Moderator::RunMission() Unknown error occurred during Mission Run.\n");
         status = -6;

         #ifdef REPORT_UNHANDLED_EXCEPTIONS
            std::exception_ptr p = std::current_exception();
            std::stringstream msg;
            msg << (p ? p.__cxa_exception_type()->name() : "null") << "\n";
            MessageInterface::ShowMessage(msg.str().c_str());
         #endif

         // Tell commands to reset their run states
         commands[sandboxNum-1]->RunComplete(true);

         //throw; // LOJ: We want to finish up the clearing process below
      }
		} // if (isRunReady)
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Cannot Run Mission. No mission sequence defined.\n");
      status = -7;
   }
   
   runState = Gmat::IDLE;
   thePublisher->SetRunState(runState);
   thePublisher->NotifyEndOfRun();
   if (theUiInterpreter != NULL)
      theUiInterpreter->NotifyRunCompleted();
   
   #if DEBUG_RUN > 1
   MessageInterface::ShowMessage("===> status=%d\n", status);
   #endif
   
   if (status == 1)
      MessageInterface::ShowMessage("\nMission run completed.\n");
   else if (status == -4)
      MessageInterface::ShowMessage("\n *** Mission run interrupted.\n");
   else
      MessageInterface::ShowMessage("\n *** Mission run failed.\n");
   
//   clock_t t2 = clock();

   std::chrono::system_clock::time_point time2 = std::chrono::system_clock::now();
   long int millis2 =
         std::chrono::duration_cast<std::chrono::milliseconds>
         (time2.time_since_epoch()).count();

   Real ms = millis2 - millis1;

   //MessageInterface::ShowMessage
   //   ("===> Total Run Time: %f seconds\n", (Real)(t2-t1)/CLOCKS_PER_SEC);
   
   MessageInterface::ShowMessage
      ("===> Total Run Time: %.3lf seconds\n", (ms/1000));

   #ifdef DEBUG_MEMORY
   StringArray tracks = MemoryTracker::Instance()->GetTracks(false, false);
   MessageInterface::ShowMessage
      ("===> There are %d memory tracks after MissionRun\n", tracks.size());
   #endif
   
   // show final state
   #ifdef __SHOW_FINAL_STATE__
   showFinalState = true;
   #endif
   
   if (showFinalState)
   {
      GmatCommand *cmd = GetFirstCommand();
      MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
      GmatCommand *lastCmd = GmatCommandUtil::GetLastCommand(cmd);
      
      MessageInterface::ShowMessage("\n========== Final State ==========\n");
      MessageInterface::ShowMessage(lastCmd->GetStringParameter("MissionSummary"));
      MessageInterface::ShowMessage("\n\n");      
   }
   else
   {
      MessageInterface::ShowMessage("\n========================================\n");
   }
   
   // Reset solar system in use and object map (LOJ: 2009.03.19)
   // So that users can create new objects from the GUI after GmatFunction run.
   objectMapInUse = theConfigManager->GetObjectMap();
   SetSolarSystemAndObjectMap(theSolarSystemInUse, objectMapInUse, false,
                              "RunMission()");
   // Reset the EOP file to its start-up file value (since it may have been 
   // changed on the Earth object)
   std::string origEopName = theFileManager->GetFullPathname("EOP_FILE");
   GmatGlobal::Instance()->GetEopFile()->ResetEopFile(origEopName);

   exitCode = status;

   return status;
} // RunMission()


//------------------------------------------------------------------------------
// Integer ChangeRunState(const std::string &state, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Changes run state.
 *
 * @param <state> run state string ("Stop", "Pause", "Resume")
 * @param <snadobxNum> sandbox number
 *
 * @return a status code
 *    1: successful,  < 0: error (tbd)
 */
//------------------------------------------------------------------------------
Integer Moderator::ChangeRunState(const std::string &state, Integer sandboxNum)
{
   #if DEBUG_RUN_STATE
   MessageInterface::ShowMessage
      ("Moderator::ChangeRunState(%s) entered\n", state.c_str());
   #endif
   
   if (state == "Stop" || state == "Pause" || state == "Resume")
   {
      GmatGlobal *gmatGlobal = GmatGlobal::Instance();
      if (state == "Stop")
      {
         runState = Gmat::IDLE;
         gmatGlobal->SetRunInterrupted(true);
      }
      else if (state == "Pause")
         runState = Gmat::PAUSED;
      else if (state == "Resume")
         runState = Gmat::RUNNING;
      
      gmatGlobal->SetRunState(runState);
   }
   else
      ; // do nothing
   
   return 1;
}


//------------------------------------------------------------------------------
// Gmat::RunState GetUserInterrupt()
//------------------------------------------------------------------------------
/**
 * Checks to see if the user has requested that the run stop or pause.
 * 
 * This method is called by the Sandbox periodically during a run to determine
 * if the user has requested that the run terminate before the mission sequence
 * has finished executing.
 * 
 * @return The expected state of the system (RUNNING, PAUSED, or IDLE).
 */
//------------------------------------------------------------------------------
Gmat::RunState Moderator::GetUserInterrupt()
{
   #if DEBUG_RUN_STATE
   MessageInterface::ShowMessage("Moderator::GetUserInterrupt() entered\n");
   #endif
   
   // give MainFrame input focus
   if (theUiInterpreter != NULL)
      theUiInterpreter->SetInputFocus();
   
   return runState;
}


//------------------------------------------------------------------------------
// Gmat::RunState GetRunState()
//------------------------------------------------------------------------------
/**
 * @return the state of the system (Gmat::RUNNING, Gmat::PAUSED, Gmat::IDLE)
 */
//------------------------------------------------------------------------------
Gmat::RunState Moderator::GetRunState()
{
   #if DEBUG_RUN_STATE
   MessageInterface::ShowMessage
      ("Moderator::GetRunsState() isRunReady=%d, endOfInterpreter=%d\n",
       isRunReady, endOfInterpreter);
   #endif
   
   // return RUNNING so that Matlab can wait for building objects
   if (!isRunReady && !endOfInterpreter)
      return Gmat::RUNNING;
   
   #if DEBUG_RUN_STATE
   MessageInterface::ShowMessage
      ("Moderator::GetRunsState() runState=%d\n", runState);
   #endif
   
   return runState;
}


//------------------------------------------------------------------------------
// Gmat::RunState GetDetailedRunState(Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * @param <snadobxNum> sandbox number (currently not used)
 *
 * @return the detailed state of the system including solver state
 *         (Gmat::IDLE, Gmat::RUNNING, Gmat::PAUSED, Gmat::TARGETING,
 *          Gmat::OPTIMIZING, etc.)
 */
//------------------------------------------------------------------------------
Gmat::RunState Moderator::GetDetailedRunState(Integer sandboxNum)
{
   #if DEBUG_RUN_STATE
   MessageInterface::ShowMessage
      ("Moderator::GetDetailedRunsState() isRunReady=%d, endOfInterpreter=%d\n",
       isRunReady, endOfInterpreter);
   #endif
   
   detailedRunState = thePublisher->GetRunState();
   GmatGlobal::Instance()->SetDetailedRunState(detailedRunState);
   
   #if DEBUG_RUN_STATE
   MessageInterface::ShowMessage
      ("Moderator::GetDetailedRunsState() detailedRunState=%d\n", detailedRunState);
   #endif
   
   return detailedRunState;
}


// Script

//------------------------------------------------------------------------------
// std::string GetMainScriptFileName()
//------------------------------------------------------------------------------
/**
 * Retrieves the main script file name if it is open.
 *
 * @return  The main script file name opened, "" if no script file opened
 */
//------------------------------------------------------------------------------
std::string Moderator::GetMainScriptFileName()
{
   return mainScriptFileName;
}


//------------------------------------------------------------------------------
// bool InterpretScript(const std::string &filename, bool readBack = false,
//                      const std::string &newPath = "")
//------------------------------------------------------------------------------
/**
 * Creates objects from script file. If readBack is true, it will save to
 * to new directory and interpret from it.  If newPath is blank "", then
 * it will create default directory "AutoSaved".
 *
 * @param <filename> input script file name
 * @param <readBack> true will read scripts, save, and read back in
 * @param <newPath> new path to be used for saving scripts
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::InterpretScript(const std::string &filename, bool readBack,
                                const std::string &newPath)
{
   #if DEBUG_INTERPRET
   MessageInterface::ShowMessage
      ("Moderator::InterpretScript() entered\n   filename='%s', readBack=%d, "
       "newPath='%s'\n", filename.c_str(), readBack, newPath.c_str());
   #endif
   bool isGoodScript = false;
   bool foundBeginMissionSeq = false;
   isRunReady = false;
   endOfInterpreter = false;
   runState = Gmat::IDLE;
   
   // If the log file is from a previously-read script in this session,
   // reset the log file to the one specified in the startup file
   GmatGlobal *gg    = GmatGlobal::Instance();
   Integer    logSrc = gg->GetLogfileSource();
   if (logSrc == GmatGlobal::SCRIPT)
   {
      gg->SetLogfileSource(GmatGlobal::STARTUP);
      std::string logName = gg->GetLogfileName(GmatGlobal::STARTUP);
      MessageInterface::SetLogFile(logName);
   }
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage
      ("\nInterpreting scripts from the file.\n***** file: " + filename + "\n");
   
   try
   {
      PrepareNextScriptReading();
      mainScriptFileName = filename;
      
      // Set GMAT working directory (for GMT-4408 LOJ: 2014.06.11)
      // GMAT working directory has script file
      std::string path = GmatFileUtil::ParsePathName(filename);
      
      if (GmatGlobal::Instance()->IsWritingFilePathInfo())
         MessageInterface::ShowMessage
            ("*** Setting '%s' as GMAT working directory\n", path.c_str());
      
      theFileManager->SetGmatWorkingDirectory(path);
      if (theUiInterpreter != NULL)
         theUiInterpreter->ResetIconFile();
      
      isGoodScript = theScriptInterpreter->Interpret(filename);
      foundBeginMissionSeq = theScriptInterpreter->FoundBeginMissionSequence();
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->SetScript(filename);
      #endif
      
      if (readBack)
      {
         #if DEBUG_INTERPRET
         MessageInterface::ShowMessage("===> newPath=%s\n", newPath.c_str());
         #endif
         
         std::string newpath = newPath;
         std::string sep = theFileManager->GetPathSeparator();
         UnsignedInt index = filename.find_last_of("/\\");
         std::string fname = filename.substr(index+1);
         
         if (newpath == "")
            newpath = filename.substr(0, index) + sep + "AutoSave" + sep;
         
         std::string newfile = newpath + fname;
         
         #if DEBUG_INTERPRET
         MessageInterface::ShowMessage
            ("===> newpath=%s\n===> newfile=%s\n", newpath.c_str(), newfile.c_str());
         #endif
         
         if (!theFileManager->DoesDirectoryExist(newpath))
         {
            std::string cmd = "mkdir " + newpath;
            
            int status = system(cmd.c_str());
            if (status != 0)
            {
               #if DEBUG_INTERPRET
               MessageInterface::ShowMessage
                  ("===> cmd=%s, status=%d\n", cmd.c_str(), status);
               #endif
            }
         }
         
         SaveScript(newfile);
         InterpretScript(newfile);
      }
      
      if (isGoodScript)
      {
         #if 0
         #if DEBUG_INTERPRET
         MessageInterface::ShowMessage
            ("Moderator::InterpretScript() successfully interpreted the script\n");
         #endif
         #endif
         MessageInterface::ShowMessage("Successfully interpreted the script\n");
         
         isRunReady = true;
      }
      else
      {
         #if DEBUG_INTERPRET
         MessageInterface::ShowMessage
            ("Moderator::InterpretScript() failed to interprete the script\n");
         #endif
         MessageInterface::ShowMessage("\n========================================\n");
         exitCode = -8;  //Exit code representing a script error
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      isRunReady = false;
   }
   
   ResetConfigurationChanged();
   endOfInterpreter = true;
   
   if (isGoodScript)
   {
      // Append BeginMissionSequence command if not there (New since 2010.07.09)
      GmatCommand *first = GetFirstCommand();
      GmatCommand *second = first->GetNext();

      
      #if DEBUG_INTERPRET
      ShowCommand("first cmd = ", first, " second cmd = ", second);
      #endif
      
      std::string firstCommandType = 
         (second != NULL ? second->GetTypeName() : "");
      
      if (!IsSequenceStarter(firstCommandType))
      {
         // Show warning message for now (LOJ: 2010.07.15)
         std::string firstCmdStr;
         if (second == NULL)
         {
            firstCmdStr = "There is no command detected.";
         }
         else
         {
            //firstCmdStr = "The first command detected is '";
            firstCmdStr = "Command mode entered at '";
            firstCmdStr = firstCmdStr +
               second->GetGeneratingString(Gmat::NO_COMMENTS) + "'";
         }
         
         //std::string knownStartCommands = "   [" + GetStarterStringList() + "]\n";
         //MessageInterface::PopupMessage
         //   (Gmat::WARNING_, "*** WARNING *** Mission Sequence start command "
         //    "is missing.  One will be required in future builds.  Recognized "
         //    "start commands are\n" + knownStartCommands + firstCmdStr);
         // Improved warning message (GMT-3546)
         std::string knownStartCommands = GetStarterStringList();
         if (foundBeginMissionSeq)
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "*** WARNING ***  Command mode entered before " +
                knownStartCommands + "; in future release, " + knownStartCommands +
                " is required before any command begins. " + firstCmdStr);
         }
         else
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "*** WARNING ***  " + knownStartCommands + " command is missing. "
                "One will be required in future release. " + firstCmdStr);
         }
         
         #if DEBUG_INTERPRET
         MessageInterface::ShowMessage
            ("==> Inserting 'BeginMissionSequence' after '%s'\n",
             first->GetTypeName().c_str());
         #endif
         bool retval;
         GmatCommand *bms = CreateCommand("BeginMissionSequence", "", retval);
         InsertCommand(bms, first);
      }
      
      if (second != NULL && second->GetTypeName() == "PrepareMissionSequence")
         loadSandboxAndPause = true;
      else
         loadSandboxAndPause = false;
      
      #if DEBUG_INTERPRET > 1
      MessageInterface::ShowMessage(GetScript());
      #endif
      
      #if DEBUG_INTERPRET > 1
      GmatCommand *cmd = GetFirstCommand();
      MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
      MessageInterface::ShowMessage("Moderator::InterpretScript() returning %d\n", isGoodScript);
      #endif

   }
   
   #if DEBUG_INTERPRET
   MessageInterface::ShowMessage
      ("Moderator::InterpretScript() returning isGoodScript=%d\n", isGoodScript);
   #endif
   
   return isGoodScript;
}

//------------------------------------------------------------------------------
// void ClearScript()
//------------------------------------------------------------------------------
/**
 * Clears scirpt related data
 */
//------------------------------------------------------------------------------
void Moderator::ClearScript()
{
   theScriptInterpreter->Clear();
}

//------------------------------------------------------------------------------
// bool InterpretScript(std::istringstream *ss, bool clearObjs)
//------------------------------------------------------------------------------
/**
 * Creates objects from stringstream
 *
 * @param <ss> input istringstream
 * @param <clearObjs> clears objects and mission sequence if true
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::InterpretScript(std::istringstream *ss, bool clearObjs)
{
   #if DEBUG_INTERPRET
   MessageInterface::ShowMessage
      ("Moderator::InterpretScript() entered, istringstream=<%p>, clearObjs=%d\n",
       ss, clearObjs);
   #endif
   bool isGoodScript = false;
   isRunReady = false;
   endOfInterpreter = false;
   runState = Gmat::IDLE;
   
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage
      ("\nInterpreting scripts from the input stream\n");
   
   try
   {
      PrepareNextScriptReading(clearObjs);
      
      // Set istream and Interpret
      theScriptInterpreter->SetInStream(ss);
      isGoodScript = theScriptInterpreter->Interpret();
      
      if (isGoodScript)
      {
         #if DEBUG_INTERPRET
         MessageInterface::ShowMessage
            ("Moderator::InterpretScript() successfully interpreted the script\n");
         #endif
         
         isRunReady = true;
      }
      else
      {
         MessageInterface::ShowMessage("\n========================================\n");
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      isRunReady = false;
   }
   
   ResetConfigurationChanged();
   endOfInterpreter = true;
   
   #if DEBUG_INTERPRET
   GmatCommand *cmd = GetFirstCommand();
   MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));
   MessageInterface::ShowMessage(GetScript());
   MessageInterface::ShowMessage
      ("Moderator::InterpretScript() returning isGoodScript=%d\n", isGoodScript);
   #endif
   
   return isGoodScript;
}


//------------------------------------------------------------------------------
// bool SaveScript(const std::string &filename,
//                 Gmat::WriteMode mode = Gmat::SCRIPTING)
//------------------------------------------------------------------------------
/**
 * Builds scripts from objects and write to a file.
 *
 * @param <filename> output script file name
 * @param <writeMode> write mode object(one of Gmat::SCRIPTING, Gmat::MATLAB_STRUCT)
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::SaveScript(const std::string &filename, Gmat::WriteMode mode)
{
   #ifdef DEBUG_SAVE_SCRIPT
   MessageInterface::ShowMessage
      ("Moderator::SaveScript() entered\n   file: %s, mode: %d\n",
       filename.c_str(), mode);
   MessageInterface::ShowMessage("The Script is saved to " + filename + "\n");
   #endif
   
   bool status = false;
   
   try
   {
      status = theScriptInterpreter->Build(filename, mode);
      if (status)
         ResetConfigurationChanged();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   return status;
}


//------------------------------------------------------------------------------
// std::string GetScript(Gmat::WriteMode mode = Gmat::SCRIPTING)
//------------------------------------------------------------------------------
/**
 * Returns built scripts from objects
 *
 * @param <writeMode> write mode object(one of Gmat::SCRIPTING, Gmat::MATLAB_STRUCT)
 *
 * @return built scripts from objects
 */
//------------------------------------------------------------------------------
std::string Moderator::GetScript(Gmat::WriteMode mode)
{
   //MessageInterface::ShowMessage("Moderator::GetScript() mode: %d\n", mode);
   
   try
   {
      std::stringstream os;
      theScriptInterpreter->SetOutStream(&os);
      
      if (theScriptInterpreter->Build(mode))
      {
         return os.str();
      }
      else
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "Unable to build script from objects\n");
         return "";
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage() + "\n");
      return "";
   }
}


//------------------------------------------------------------------------------
// Integer RunScript(Integer sandboxNum = 1)
//------------------------------------------------------------------------------
/**
 * Executes commands built from the script file.
 *
 * @param <sandboxNum> sandbox number
 *
 * @return a status code
 *    0 = successful, <0 = error (tbd)
 */
//------------------------------------------------------------------------------
Integer Moderator::RunScript(Integer sandboxNum)
{
   MessageInterface::ShowMessage("Moderator::RunScript() entered\n");
   return RunMission(sandboxNum);
}

//------------------------------------------------------------------------------
// bool StartMatlabServer()
//------------------------------------------------------------------------------
/**
 * Interface used to tell an Interpreter to start the MATLAB server.
 */
//------------------------------------------------------------------------------
bool Moderator::StartMatlabServer()
{
   if (theUiInterpreter != NULL)
   {
      theUiInterpreter->StartMatlabServer();
      return true;
   }
   return false;
}

//------------------------------------------------------------------------------
// std::vector<Gmat::PluginResources*> *Moderator::GetPluginResourceList()
//------------------------------------------------------------------------------
/**
 * Passes the list of plugin resources to the GUI
 *
 * @return The resource list parsed during initialization
 */
//------------------------------------------------------------------------------
std::vector<Gmat::PluginResource*> *Moderator::GetPluginResourceList()
{
   return &userResources;
}

//---------------------------------
//  private
//---------------------------------

// initialization
//------------------------------------------------------------------------------
// void CreatePlanetaryCoeffFile()
//------------------------------------------------------------------------------
void Moderator::CreatePlanetaryCoeffFile()
{
   #if DEBUG_INITIALIZE
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator initializing planetary coefficient file...\n");
   #endif
   
   std::string nutFileName =
      theFileManager->GetFullPathname("NUTATION_COEFF_FILE");
   MessageInterface::ShowMessage("Setting nutation file to %s\n",
                                 nutFileName.c_str());
   // wcs 2013.03.04 planFileName is CURRENTLY not used, since our default is
   // PLANETARY_1980 and PLANETARY_1996 is not allowed; however, we pass it in
   // here anyway since we are not removing the code to handle PLANETARY_1996
   std::string planFileName =
      theFileManager->GetFullPathname("PLANETARY_COEFF_FILE");
   // WCS 2016.07.13 - remove this message since we are not currently using a 
   // Planetary Coefficients file.
//   MessageInterface::ShowMessage("Setting planetary coeff. file to %s\n",
//                                 planFileName.c_str());
   
   theItrfFile = new ItrfCoefficientsFile(nutFileName, planFileName);
   theItrfFile->Initialize();
   GmatGlobal::Instance()->SetItrfCoefficientsFile(theItrfFile);
}


//------------------------------------------------------------------------------
// bool SetWidgetCreator(GuiWidgetCreatorCallback creatorFun)
//------------------------------------------------------------------------------
/**
 * Sets the callback function used to create plugin GUI widgets
 *
 * @param creatorFun The function pointer that is called to create the widgets
 */
//------------------------------------------------------------------------------
void Moderator::SetWidgetCreator(GuiWidgetCreatorCallback creatorFun)
{
   pCreateWidget = creatorFun;
}

//------------------------------------------------------------------------------
// void CreateTimeFile()
//------------------------------------------------------------------------------
void Moderator::CreateTimeFile()
{
   #if DEBUG_INITIALIZE
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator initializing time file...\n");
   #endif
   
   std::string filename = theFileManager->GetFullPathname("LEAP_SECS_FILE");
   MessageInterface::ShowMessage("Setting leap seconds file to %s\n",
                                 filename.c_str());
   theLeapSecsFile = new LeapSecsFileReader(filename);
   theLeapSecsFile->Initialize();
   
   filename = theFileManager->GetFullPathname("EOP_FILE");
   theEopFile = new EopFile(filename);
   
   theTimeConverter->SetLeapSecsFileReader(theLeapSecsFile);

   // The step to initialize EOP file has to be done after Leap Second file is set to TimeSystemConverter
   // due to theEopFile->Initialize() has step to convert time from UTC to A1
   theEopFile->Initialize();

   theTimeConverter->SetEopFile(theEopFile);
   GmatGlobal::Instance()->SetEopFile(theEopFile);
}


// prepare next script reading
//------------------------------------------------------------------------------
// void PrepareNextScriptReading(bool clearObjs = true)
//------------------------------------------------------------------------------
/*
 * This method prepares for next script reading by clearing commands and resource,
 * resetting object pointers
 *
 * @param <clearObjs> set to true if clearing commands and resource (true)
 */
//------------------------------------------------------------------------------
void Moderator::PrepareNextScriptReading(bool clearObjs)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::PrepareNextScriptReading() entered, %sclearing objects\n%s\n",
       clearObjs ? "" : "Not",
       "======================================================================");
   #endif
   
   // Set object manage option to configuration
   objectManageOption = 1;

   // Reset current script file
   mainScriptFileName = "";
   
   // Clear SpacePoint instance count so that Spacecraft color starts from the
   // same color for each run
   SpacePoint::ClearInstanceCount();
   
   // Clear command sequence before resource (loj: 2008.07.10)
   if (clearObjs)
   {
      //clear both resource and command sequence
      #if DEBUG_RUN
      MessageInterface::ShowMessage(".....Clearing both resource and command sequence...\n");
      #endif
      
      ClearCommandSeq(true, true);
      ClearResource();
   }
   
   // Set object map in use
   objectMapInUse = theConfigManager->GetObjectMap();
   
   #ifdef DEBUG_OBJECT_MAP
   MessageInterface::ShowMessage
      ("ObjectMapInUse was set to the configuration map <%p>\n", objectMapInUse);
   #endif
   
   #if DEBUG_RUN
   MessageInterface::ShowMessage(".....Creating SolarSystem in use...\n");
   #endif
   CreateSolarSystemInUse();
   
   // Need default CS's in case they are used in the script
   #if DEBUG_RUN
   MessageInterface::ShowMessage(".....Creating Default CoordinateSystem and Barycenter...\n");
   #endif
   CreateDefaultCoordSystems();
   // Create the default Solar System barycenter
   CreateDefaultBarycenter();
   
   #if DEBUG_OBJECT_MAP > 1
   ShowObjectMap("   Moderator::PrepareNextScriptReading() Here is the configured object map");
   #endif
   
   // Set solar system in use and object map (loj: 2008.03.31)
   #if DEBUG_RUN
   MessageInterface::ShowMessage(".....Setting SolarSystem and ObjectMap to Interpreter...\n");
   #endif
   
   // Reset initial solar system in use and object map 
   SetSolarSystemAndObjectMap(theSolarSystemInUse, objectMapInUse, false,
                              "PrepareNextScriptReading()");
   currentFunction = NULL;
   
   // Delete unmanaged functions (LOJ: 2009.03.24)
   // This causes crash on Func_AssignmentTest after running
   // Func_MatlabObjectPassingCheck.script
   // so disable until it is fully tested (LOJ: 2009.04.08)
   #ifdef __ENABLE_CLEAR_UNMANAGED_FUNCTIONS__
   #if DEBUG_RUN > 0
   MessageInterface::ShowMessage
      (".....Moderator::PrepareNextScriptReading() deleting %d unmanaged functions\n",
       unmanagedFunctions.size());
   #endif
   for (UnsignedInt i=0; i<unmanagedFunctions.size(); i++)
   {
      GmatBase *func = (GmatBase*)(unmanagedFunctions[i]);
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (func, func->GetName(), "Moderator::PrepareNextScriptReading()",
          "deleting unmanaged function");
      #endif
      delete func;
      func = NULL;
   }
   unmanagedFunctions.clear();
   #endif
   
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::PrepareNextScriptReading() exiting\n"
       "======================================================================\n");
   #endif
} // PrepareNextScriptReading


//------------------------------------------------------------------------------
// void CreateMinimumResource()
//------------------------------------------------------------------------------
/**
 * Creates minimum resource: solar system, default coordinate system.
 */
//------------------------------------------------------------------------------
void Moderator::CreateMinimumResource()
{
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator creating minimum resource...\n");
   #endif
   
   try
   {
      // Create solar system in use
      CreateSolarSystemInUse();
      
      // Create default coordinate systems
      CreateDefaultCoordSystems();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "*** Error occurred during minimum resource creation.\n    Message: " +
          e.GetFullMessage());
   }
   
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Moderator successfully created minimum resource\n");
   #endif
}


//------------------------------------------------------------------------------
// void CreateSolarSystemInUse()
//------------------------------------------------------------------------------
/**
 * Creates SolarSystem in use by cloning the default SolarSystem if cloning
 * SolarSystem is enabled, else it just copies value of the default SolarSystem
 * to SolarSystem in use.  It also creates internal CoordinateSystem in use.
 */
//------------------------------------------------------------------------------
void Moderator::CreateSolarSystemInUse()
{
   //-----------------------------------------------------------------
   #ifndef __DISABLE_SOLAR_SYSTEM_CLONING__
   //-----------------------------------------------------------------   
      // delete old SolarSystem in use and create new from default
      if (theSolarSystemInUse != NULL)
      {
         #if DEBUG_INITIALIZE
         MessageInterface::ShowMessage
            ("Moderator deleting old solar system in use <%p>\n", theSolarSystemInUse);
         #endif
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (theSolarSystemInUse, theSolarSystemInUse->GetName(),
             "Moderator::CreateSolarSystemInUse()");
         #endif
         delete theSolarSystemInUse;
      }
      
      theSolarSystemInUse = NULL;
      
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage
         (".....Setting theSolarSystemInUse to clone of theDefaultSolarSystem <%p>...\n",
          theDefaultSolarSystem);
      #endif
      
      theSolarSystemInUse = (SolarSystem*) theDefaultSolarSystem->Clone();
      theSolarSystemInUse->SetName("SolarSystem");
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (theSolarSystemInUse, theSolarSystemInUse->GetName(),
          "Moderator::CreateSolarSystemInUse()",
          "theSolarSystemInUse = theDefaultSolarSystem->Clone()");
      #endif
      
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage
         (".....Setting SolarSystemInUse to theInternalSolarSystem...\n");
      #endif
      theInternalSolarSystem = theSolarSystemInUse;
      
      // set solar system in use
      SetSolarSystemInUse(theSolarSystemInUse);
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         ("Moderator created new solar system in use: %p\n", theSolarSystemInUse);
      #endif
      
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
      if (theSolarSystemInUse == NULL)
      {
         theSolarSystemInUse = theDefaultSolarSystem->Clone();
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (theSolarSystemInUse, theSolarSystemInUse->GetName(),
             "Moderator::CreateSolarSystemInUse()",
             "theSolarSystemInUse = theDefaultSolarSystem->Clone()");
         #endif
         theSolarSystemInUse->SetName("SolarSystem");
         SetSolarSystemInUse(theSolarSystemInUse);
      }
      else
      {
         theSolarSystemInUse->Copy(theDefaultSolarSystem);
         theSolarSystemInUse->SetName("SolarSystem");
      }
      
      theInternalSolarSystem = theSolarSystemInUse;
      
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage
         ("Moderator::CreateSolarSystemInUse() theSolarSystemInUse=<%p>, "
          "theDefaultSolarSystem=<%p>\n", theSolarSystemInUse,  theDefaultSolarSystem);
      #endif
   //-----------------------------------------------------------------
   #endif
   //-----------------------------------------------------------------
   
   // delete old theInternalCoordSystem and create new one
   if (theInternalCoordSystem)
   {
      #if DEBUG_SOLAR_SYSTEM_IN_USE
      MessageInterface::ShowMessage
         (".....deleting (%p)theInternalCoordSystem\n", theInternalCoordSystem);
      #endif
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (theInternalCoordSystem, theInternalCoordSystem->GetName(),
          "Moderator::CreateSolarSystemInUse()");
      #endif
      delete theInternalCoordSystem;
      theInternalCoordSystem = NULL;
   }
   
   CreateInternalCoordSystem();   
}


//------------------------------------------------------------------------------
// void CreateInternalCoordSystem()
//------------------------------------------------------------------------------
/*
 * Creates the internal coordinate system. This coordinate system is used for
 * publishing data for OpenGL plot.
 * Currently it is EarthMJ2000Eq system.
 */
//------------------------------------------------------------------------------
void Moderator::CreateInternalCoordSystem()
{
   #if DEBUG_INITIALIZE
   //MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator creating internal coordinate system...\n");
   #endif
   
   if (theInternalCoordSystem != NULL)
   {
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         ("..... <%p>theInternalCoordSystem already exists\n",theInternalCoordSystem);
      #endif
   }
   else
   {
      // Create internal CoordinateSystem with no name, since we don't want
      // it to be configured.
      theInternalCoordSystem =
         CreateCoordinateSystem("InternalEarthMJ2000Eq", true, true);
      
      #if DEBUG_INITIALIZE
      MessageInterface::ShowMessage
         (".....created  <%p>theInternalCoordSystem\n", theInternalCoordSystem);
      #endif
   }
}


//------------------------------------------------------------------------------
// void CreateDefaultCoordSystems()
//------------------------------------------------------------------------------
void Moderator::CreateDefaultCoordSystems()
{
   #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultCoordSystems() checking if default coordinate systems should be created...\n");
   #endif

   // Do not create default barycenter inside a function (LOJ: 2014.12.17)
   if (currentFunction != NULL)
   {
      #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateDefaultCoordSystems() just returning, it is inside a function\n");
      #endif
      return;
   }
   
   
   defaultCoordSystemNames.clear();
   
   try
   {
      SpacePoint *earth = (SpacePoint*)GetConfiguredObject("Earth");
      SolarSystem *ss = GetSolarSystemInUse();
      
      // EarthMJ2000Eq
      CoordinateSystem *eqcs = GetCoordinateSystem("EarthMJ2000Eq");
      defaultCoordSystemNames.push_back("EarthMJ2000Eq");
      if (eqcs == NULL)
      {
         eqcs = CreateCoordinateSystem("EarthMJ2000Eq", true);
         
         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            (".....created <%p>'%s'\n", eqcs, eqcs->GetName().c_str());
         #endif
      }
      else
      {
         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            (".....found <%p>'%s'\n", eqcs, eqcs->GetName().c_str());
         #endif
         eqcs->SetSolarSystem(ss);
         eqcs->Initialize();
      }
      
      // EarthMJ2000Ec
      CoordinateSystem *eccs = GetCoordinateSystem("EarthMJ2000Ec");
      defaultCoordSystemNames.push_back("EarthMJ2000Ec");
      if (eccs == NULL)
      {
         eccs = CreateCoordinateSystem("EarthMJ2000Ec", false);
         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            (".....created <%p>'%s'\n", eccs, eccs->GetName().c_str());
         #endif
         AxisSystem *ecAxis = CreateAxisSystem("MJ2000Ec", "MJ2000Ec_Earth");
         eccs->SetStringParameter("Origin", "Earth");
         eccs->SetJ2000BodyName("Earth");
         eccs->SetRefObject(ecAxis, Gmat::AXIS_SYSTEM, ecAxis->GetName());
         eccs->SetOrigin(earth);
         eccs->SetJ2000Body(earth);
         eccs->SetSolarSystem(ss);
         eccs->Initialize();
         
         // Since CoordinateSystem clones AxisSystem, delete it from here
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (ecAxis, "localAxes", "Moderator::CreateDefaultCoordSystems()",
             "deleting localAxes");
         #endif
         delete ecAxis;
      }
      else
      {
         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            (".....found <%p>'%s'\n", eccs, eccs->GetName().c_str());
         #endif
         eccs->SetSolarSystem(ss);
         eccs->Initialize();
      }
      
      // EarthFixed
      CoordinateSystem *bfcs = GetCoordinateSystem("EarthFixed");
      defaultCoordSystemNames.push_back("EarthFixed");
      if (bfcs == NULL)
      {
         bfcs = CreateCoordinateSystem("EarthFixed", false);
         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            (".....created <%p>'%s'\n", bfcs, bfcs->GetName().c_str());
         #endif
         BodyFixedAxes *bfecAxis =
            (BodyFixedAxes*)CreateAxisSystem("BodyFixed", "BodyFixed_Earth");
         bfecAxis->SetEopFile(theEopFile);
         bfecAxis->SetCoefficientsFile(theItrfFile);
         bfcs->SetStringParameter("Origin", "Earth");
         bfcs->SetJ2000BodyName("Earth");
         bfcs->SetRefObject(bfecAxis, Gmat::AXIS_SYSTEM, bfecAxis->GetName());
         bfcs->SetOrigin(earth);
         bfcs->SetJ2000Body(earth);
         bfcs->SetSolarSystem(ss);
         bfcs->Initialize();
         
         // Since CoordinateSystem clones AxisSystem, delete it from here
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (bfecAxis, "localAxes", "Moderator::CreateDefaultCoordSystems()",
             "deleting localAxes");
         #endif
         delete bfecAxis;
      }
      else
      {
         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            (".....found <%p>'%s'\n", bfcs, bfcs->GetName().c_str());
         #endif
         bfcs->SetSolarSystem(ss);
         bfcs->Initialize();
      }

      // EarthICRF
      CoordinateSystem *earthICRFcs = GetCoordinateSystem("EarthICRF");
      defaultCoordSystemNames.push_back("EarthICRF");
      if (earthICRFcs == NULL)
      {
         earthICRFcs = CreateCoordinateSystem("EarthICRF", false);
         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            (".....created <%p>'%s'\n", earthICRFcs, earthICRFcs->GetName().c_str());
         #endif
         ICRFAxes *icrfAxis =
            (ICRFAxes*)CreateAxisSystem("ICRF", "ICRF_Axis");
         icrfAxis->SetEopFile(theEopFile);
         icrfAxis->SetCoefficientsFile(theItrfFile);
         earthICRFcs->SetStringParameter("Origin", "Earth");
         earthICRFcs->SetJ2000BodyName("Earth");
         earthICRFcs->SetRefObject(icrfAxis, Gmat::AXIS_SYSTEM, icrfAxis->GetName());
         earthICRFcs->SetOrigin(earth);
         earthICRFcs->SetJ2000Body(earth);
         earthICRFcs->SetSolarSystem(ss);
         earthICRFcs->Initialize();
         
         // Since CoordinateSystem clones AxisSystem, delete it from here
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (icrfAxis, "localAxes", "Moderator::CreateDefaultCoordSystems()",
             "deleting localAxes");
         #endif
         delete icrfAxis;
      }
      else
      {
         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            (".....found <%p>'%s'\n", earthICRFcs, earthICRFcs->GetName().c_str());
         #endif
         earthICRFcs->SetSolarSystem(ss);
         earthICRFcs->Initialize();
      }
      // Mark these as built-in coordinate systems
      eqcs->SetIsBuiltIn(true);
      eccs->SetIsBuiltIn(true);
      bfcs->SetIsBuiltIn(true);
      earthICRFcs->SetIsBuiltIn(true);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "Moderator::CreateDefaultCoordSystems() Error occurred during default "
          "coordinate system creation. " +  e.GetFullMessage());
   }
}

//------------------------------------------------------------------------------
// void CreateDefaultBarycenter()
//------------------------------------------------------------------------------
void Moderator::CreateDefaultBarycenter()
{
   #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultBarycenter() Checking if default barycenter should be created...\n");
   #endif
   
   // Do not create default barycenter inside a function (LOJ: 2014.12.17)
   if (currentFunction != NULL)
   {
      #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
      MessageInterface::ShowMessage
         ("Moderator::CreateDefaultBarycenter() just returning, it is inside a function\n");
      #endif
      return;
   }
   
   
   try
   {
      SolarSystem *ss = GetSolarSystemInUse();
      
      // Create BuiltIn Solar System Barycenter
      Barycenter *bary = (Barycenter*) GetCalculatedPoint(GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME);
      if (bary == NULL)
      {
         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            ("Moderator::CreateDefaultBarycenter() Calling CreateCalculatedPoint()\n");
         #endif
         
         bary = (Barycenter*) CreateCalculatedPoint("Barycenter", GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME, false);

         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            (".....created <%p>'%s'\n", bary, bary->GetName().c_str());
         #endif
      }
      else
      {
         #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
         MessageInterface::ShowMessage
            (".....found <%p>'%s'\n", bary, bary->GetName().c_str());
         #endif
      }
      bary->SetSolarSystem(ss);
      bary->SetIsBuiltIn(true, "SSB");
      bary->Initialize();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "Moderator::CreateDefaultBarycenter() Error occurred during default "
          "barycenter creation. " +  e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// void CreateDefaultParameters()
//------------------------------------------------------------------------------
/**
 * Creates and sets ref. object of built-in Parameters.
 *
 * @note Don't forget to set Parameter object and dependency object below
 */
//------------------------------------------------------------------------------
void Moderator::CreateDefaultParameters()
{
   // Create ImpulsiveBurn for ImpulsiveBurn Parameters
   // We don't need to create if we only need to add Parameters to database
   // so commented out (LOJ: 2016.05.19)
   // GetDefaultBurn("ImpulsiveBurn");
   // #if DEBUG_DEFAULT_MISSION > 0
   // MessageInterface::ShowMessage("-->default impulsive burn created\n");
   // #endif
   
   // ImpulsiveBurn parameters
   // Create ImpulsiveBurn Parameters to add to ParameterInfo database.
   CreateParameter("Element1", "DefaultIB.Element1");
   CreateParameter("Element2", "DefaultIB.Element2");
   CreateParameter("Element3", "DefaultIB.Element3");
   // CreateParameter("V", "DefaultIB.V");  // deprecated
   // CreateParameter("N", "DefaultIB.N");  // deprecated
   // CreateParameter("B", "DefaultIB.B");  // deprecated
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("-->default impulsive burn parameters created\n");
   #endif
   
   // Remove ImpulsiveBurn Parameters since these Parameters info has been
   // add to database and not used in the default mission
   RemoveObject(Gmat::PARAMETER, "DefaultIB.Element1", true);
   RemoveObject(Gmat::PARAMETER, "DefaultIB.Element2", true);
   RemoveObject(Gmat::PARAMETER, "DefaultIB.Element3", true);
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("-->default impulsive burn parameters deleted\n");
   #endif
   
   // Create FiniteBurn for FiniteBurn Parameters
   // We don't need to create if we only need to add Parameters to database
   // so commented out (LOJ: 2016.05.19)
   // GetDefaultBurn("FiniteBurn");
   // #if DEBUG_DEFAULT_MISSION > 0
   // MessageInterface::ShowMessage("-->default finite burn created\n");
   // #endif
   
   // FiniteBurn Parameters
   // Create FiniteBurn Parameters to add to ParameterInfo database.
   CreateParameter("TotalMassFlowRate", "DefaultFB.TotalMassFlowRate");
   CreateParameter("TotalAcceleration1", "DefaultFB.TotalAcceleration1");
   CreateParameter("TotalAcceleration2", "DefaultFB.TotalAcceleration2");
   CreateParameter("TotalAcceleration3", "DefaultFB.TotalAcceleration3");
   CreateParameter("TotalThrust1", "DefaultFB.TotalThrust1");
   CreateParameter("TotalThrust2", "DefaultFB.TotalThrust2");
   CreateParameter("TotalThrust3", "DefaultFB.TotalThrust3");
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("-->default finite burn parameters created\n");
   #endif
   
   // Remove FiniteBurn Parameters since these Parameters info already add to database
   // and not used in the default mission
   RemoveObject(Gmat::PARAMETER, "DefaultFB.TotalMassFlowRate", true);
   RemoveObject(Gmat::PARAMETER, "DefaultFB.TotalAcceleration1", true);
   RemoveObject(Gmat::PARAMETER, "DefaultFB.TotalAcceleration2", true);
   RemoveObject(Gmat::PARAMETER, "DefaultFB.TotalAcceleration3", true);
   RemoveObject(Gmat::PARAMETER, "DefaultFB.TotalThrust1", true);
   RemoveObject(Gmat::PARAMETER, "DefaultFB.TotalThrust2", true);
   RemoveObject(Gmat::PARAMETER, "DefaultFB.TotalThrust3", true);
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("-->default finite burn parameters deleted\n");
   #endif
   
   // Time parameters
   CreateParameter("ElapsedSecs", "DefaultSC.ElapsedSecs");
   CreateParameter("ElapsedDays", "DefaultSC.ElapsedDays");      
   // CreateParameter("CurrA1MJD", "DefaultSC.CurrA1MJD"); // Still used in some scripts so cannot remove  // deprecated
   CreateParameter("A1ModJulian", "DefaultSC.A1ModJulian");
   CreateParameter("A1Gregorian", "DefaultSC.A1Gregorian");
   CreateParameter("TAIModJulian", "DefaultSC.TAIModJulian");
   CreateParameter("TAIGregorian", "DefaultSC.TAIGregorian");
   CreateParameter("TTModJulian", "DefaultSC.TTModJulian");
   CreateParameter("TTGregorian", "DefaultSC.TTGregorian");
   CreateParameter("TDBModJulian", "DefaultSC.TDBModJulian");
   CreateParameter("TDBGregorian", "DefaultSC.TDBGregorian");
   CreateParameter("UTCModJulian", "DefaultSC.UTCModJulian");
   CreateParameter("UTCGregorian", "DefaultSC.UTCGregorian");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default time parameters  created\n");
   #endif
   
   // Cartesian parameters
   CreateParameter("X", "DefaultSC.EarthMJ2000Eq.X");
   CreateParameter("Y", "DefaultSC.EarthMJ2000Eq.Y");
   CreateParameter("Z", "DefaultSC.EarthMJ2000Eq.Z");
   CreateParameter("VX", "DefaultSC.EarthMJ2000Eq.VX");
   CreateParameter("VY", "DefaultSC.EarthMJ2000Eq.VY");
   CreateParameter("VZ", "DefaultSC.EarthMJ2000Eq.VZ");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default cartesian parameters created\n");
   #endif
   
   // Keplerian parameters
   CreateParameter("SMA", "DefaultSC.Earth.SMA");
   CreateParameter("ECC", "DefaultSC.Earth.ECC");
   CreateParameter("INC", "DefaultSC.Earth.INC");
   CreateParameter("RAAN", "DefaultSC.Earth.RAAN");
   CreateParameter("AOP", "DefaultSC.EarthMJ2000Eq.AOP");
   CreateParameter("TA", "DefaultSC.Earth.TA");
   CreateParameter("MA", "DefaultSC.Earth.MA");
   CreateParameter("EA", "DefaultSC.Earth.EA");
   CreateParameter("HA", "DefaultSC.Earth.HA");
   CreateParameter("MM", "DefaultSC.Earth.MM");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default keplerian parameters created\n");
   #endif
   
   // Equinoctial parameters
   CreateParameter("EquinoctialH", "DefaultSC.EarthMJ2000Eq.EquinoctialH");
   CreateParameter("EquinoctialK", "DefaultSC.EarthMJ2000Eq.EquinoctialK");
   CreateParameter("EquinoctialP", "DefaultSC.EarthMJ2000Eq.EquinoctialP");
   CreateParameter("EquinoctialQ", "DefaultSC.EarthMJ2000Eq.EquinoctialQ");
   CreateParameter("MLONG", "DefaultSC.EarthMJ2000Eq.MLONG");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default equinoctial parameters created\n");
   #endif

   // Equinoctial rate parameters
   CreateParameter("SMADot", "DefaultSC.DefaultProp_ForceModel.SMADot");
   CreateParameter("EquinoctialHDot", "DefaultSC.DefaultProp_ForceModel.EquinoctialHDot");
   CreateParameter("EquinoctialKDot", "DefaultSC.DefaultProp_ForceModel.EquinoctialKDot");
   CreateParameter("EquinoctialPDot", "DefaultSC.DefaultProp_ForceModel.EquinoctialPDot");
   CreateParameter("EquinoctialQDot", "DefaultSC.DefaultProp_ForceModel.EquinoctialQDot");
   CreateParameter("TLONGDot", "DefaultSC.DefaultProp_ForceModel.TLONGDot");
   
   // Modified by M.H.
   // ModEquinoctial parameters
   // Changed SemiLatusRectum to SemilatusRectum (lower case L)
   //CreateParameter("SemiLatusRectum", "DefaultSC.EarthMJ2000Eq.SemiLatusRectum");
   //CreateParameter("SemilatusRectum", "DefaultSC.EarthMJ2000Eq.SemilatusRectum");
   // Changed SemilatusRectum to origin dependent (LOJ: 2014.01.29)
   CreateParameter("SemilatusRectum", "DefaultSC.Earth.SemilatusRectum");
   CreateParameter("ModEquinoctialF", "DefaultSC.EarthMJ2000Eq.ModEquinoctialF");
   CreateParameter("ModEquinoctialG", "DefaultSC.EarthMJ2000Eq.ModEquinoctialG");
   CreateParameter("ModEquinoctialH", "DefaultSC.EarthMJ2000Eq.ModEquinoctialH");
   CreateParameter("ModEquinoctialK", "DefaultSC.EarthMJ2000Eq.ModEquinoctialK");
   CreateParameter("TLONG", "DefaultSC.EarthMJ2000Eq.TLONG");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default modequinoctial parameters created\n");
   #endif
   
   // Alternate Equinoctial parameters by HYKim
   CreateParameter("AltEquinoctialP", "DefaultSC.EarthMJ2000Eq.AltEquinoctialP");
   CreateParameter("AltEquinoctialQ", "DefaultSC.EarthMJ2000Eq.AltEquinoctialQ");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default alternate equinoctial parameters created\n");
   #endif
   
   // Delaunay parameters by M.H.
   CreateParameter("Delaunayl", "DefaultSC.EarthMJ2000Eq.Delaunayl");
   CreateParameter("Delaunayg", "DefaultSC.EarthMJ2000Eq.Delaunayg");
   CreateParameter("Delaunayh", "DefaultSC.EarthMJ2000Eq.Delaunayh");
   CreateParameter("DelaunayL", "DefaultSC.EarthMJ2000Eq.DelaunayL");
   CreateParameter("DelaunayG", "DefaultSC.EarthMJ2000Eq.DelaunayG");
   CreateParameter("DelaunayH", "DefaultSC.EarthMJ2000Eq.DelaunayH");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default delaunay parameters created\n");
   #endif
   
   // Planetodetic parameters by M.H.
   CreateParameter("PlanetodeticRMAG", "DefaultSC.EarthMJ2000Eq.PlanetodeticRMAG");
   CreateParameter("PlanetodeticLON", "DefaultSC.EarthMJ2000Eq.PlanetodeticLON");
   CreateParameter("PlanetodeticLAT", "DefaultSC.EarthMJ2000Eq.PlanetodeticLAT");
   CreateParameter("PlanetodeticVMAG", "DefaultSC.EarthMJ2000Eq.PlanetodeticVMAG");
   CreateParameter("PlanetodeticAZI", "DefaultSC.EarthMJ2000Eq.PlanetodeticAZI");
   CreateParameter("PlanetodeticHFPA", "DefaultSC.EarthMJ2000Eq.PlanetodeticHFPA");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default planetodetic parameters created\n");
   #endif
   
   // IncomingAsymptote parameters by YK
   // Added HyperbolicRadPer (LOJ: 2014.04.28)
   // Changed HyperbolicRadPer to IncomingRadPer and added IncomingC3Energy (LOJ: 2014.05.08)
   CreateParameter("IncomingRadPer", "DefaultSC.Earth.IncomingRadPer");
   CreateParameter("IncomingC3Energy", "DefaultSC.Earth.IncomingC3Energy");
   CreateParameter("IncomingRHA", "DefaultSC.EarthMJ2000Eq.IncomingRHA");
   CreateParameter("IncomingDHA", "DefaultSC.EarthMJ2000Eq.IncomingDHA");
   CreateParameter("IncomingBVAZI", "DefaultSC.EarthMJ2000Eq.IncomingBVAZI");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default IncomingAsymptote parameters created\n");
   #endif
   
   // OutgoingAsymptote parameters by YK
   // Added OutgoingRadPer and OutgoingC3Energy (LOJ: 2014.05.08)
   CreateParameter("OutgoingRadPer", "DefaultSC.Earth.OutgoingRadPer");
   CreateParameter("OutgoingC3Energy", "DefaultSC.Earth.OutgoingC3Energy");
   CreateParameter("OutgoingRHA", "DefaultSC.EarthMJ2000Eq.OutgoingRHA");
   CreateParameter("OutgoingDHA", "DefaultSC.EarthMJ2000Eq.OutgoingDHA");
   CreateParameter("OutgoingBVAZI", "DefaultSC.EarthMJ2000Eq.OutgoingBVAZI");

   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default OutgoingAsymptote parameters created\n");
   #endif
   
   // Modified by YK
   // BrouwerMeanShort parameters
   // In averaging process, osculating AOP relates to every elements.
   // Therefore, averaged elements have coordinate dependecy.
   // Changed Parameter names (see GMT-4228 LOJ: 2014.01.09)
   CreateParameter("BrouwerShortSMA", "DefaultSC.EarthMJ2000Eq.BrouwerShortSMA");
   CreateParameter("BrouwerShortECC", "DefaultSC.EarthMJ2000Eq.BrouwerShortECC");
   CreateParameter("BrouwerShortINC", "DefaultSC.EarthMJ2000Eq.BrouwerShortINC");
   CreateParameter("BrouwerShortRAAN", "DefaultSC.EarthMJ2000Eq.BrouwerShortRAAN");
   CreateParameter("BrouwerShortAOP", "DefaultSC.EarthMJ2000Eq.BrouwerShortAOP");
   CreateParameter("BrouwerShortMA", "DefaultSC.EarthMJ2000Eq.BrouwerShortMA");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default BrouwerMeanShort parameters created\n");
   #endif
   
   // Modified by YK
   // BrouwerMeanLong parameters
   // In averaging process, osculating AOP relates to every elements.
   // Therefore, averaged elements have coordinate dependecy.
   CreateParameter("BrouwerLongSMA", "DefaultSC.EarthMJ2000Eq.BrouwerLongSMA");
   CreateParameter("BrouwerLongECC", "DefaultSC.EarthMJ2000Eq.BrouwerLongECC");
   CreateParameter("BrouwerLongINC", "DefaultSC.EarthMJ2000Eq.BrouwerLongINC");
   CreateParameter("BrouwerLongRAAN", "DefaultSC.EarthMJ2000Eq.BrouwerLongRAAN");
   CreateParameter("BrouwerLongAOP", "DefaultSC.EarthMJ2000Eq.BrouwerLongAOP");
   CreateParameter("BrouwerLongMA", "DefaultSC.EarthMJ2000Eq.BrouwerLongMA");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default BrouwerMeanLong parameters created\n");
   #endif
   
   // Orbital parameters
   CreateParameter("VelApoapsis", "DefaultSC.Earth.VelApoapsis");
   CreateParameter("VelPeriapsis", "DefaultSC.Earth.VelPeriapsis");
   CreateParameter("Apoapsis", "DefaultSC.Earth.Apoapsis");
   CreateParameter("Periapsis", "DefaultSC.Earth.Periapsis");
   CreateParameter("OrbitPeriod", "DefaultSC.Earth.OrbitPeriod");
   CreateParameter("RadApo", "DefaultSC.Earth.RadApo");
   CreateParameter("RadPer", "DefaultSC.Earth.RadPer");
   CreateParameter("C3Energy", "DefaultSC.Earth.C3Energy");
   CreateParameter("Energy", "DefaultSC.Earth.Energy");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default orbital parameters created\n");
   #endif
   
   // Spherical parameters
   CreateParameter("RMAG", "DefaultSC.Earth.RMAG");
   CreateParameter("RA", "DefaultSC.Earth.RA");
   CreateParameter("DEC", "DefaultSC.EarthMJ2000Eq.DEC");
   CreateParameter("VMAG", "DefaultSC.EarthMJ2000Eq.VMAG");
   CreateParameter("RAV", "DefaultSC.EarthMJ2000Eq.RAV");
   CreateParameter("DECV", "DefaultSC.EarthMJ2000Eq.DECV");
   CreateParameter("AZI", "DefaultSC.EarthMJ2000Eq.AZI");
   CreateParameter("FPA", "DefaultSC.EarthMJ2000Eq.FPA");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default spherical parameters created\n");
   #endif
   
   // Angular parameters
   // Changed to create SemilatusRectum as part of ModEquinoctial Parameter above
   // (LOJ: 2014.01.22)
   //CreateParameter("SemilatusRectum", "DefaultSC.Earth.SemilatusRectum");
   CreateParameter("HMAG", "DefaultSC.HMAG");
   CreateParameter("HX", "DefaultSC.EarthMJ2000Eq.HX");
   CreateParameter("HY", "DefaultSC.EarthMJ2000Eq.HY");
   CreateParameter("HZ", "DefaultSC.EarthMJ2000Eq.HZ");
   CreateParameter("DLA", "DefaultSC.EarthMJ2000Eq.DLA");
   CreateParameter("RLA", "DefaultSC.EarthMJ2000Eq.RLA");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default angular parameters created\n");
   #endif
   
   #ifdef __ENABLE_ATMOS_DENSITY__
   // Environmental parameters
   CreateParameter("AtmosDensity", "DefaultSC.Earth.AtmosDensity");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default env parameters created\n");
   #endif
   #endif
   
   // Planet parameters
   CreateParameter("Altitude", "DefaultSC.Earth.Altitude");
   CreateParameter("MHA", "DefaultSC.Earth.MHA");
   CreateParameter("Longitude", "DefaultSC.Earth.Longitude");
   CreateParameter("Latitude", "DefaultSC.Earth.Latitude");
   CreateParameter("LST", "DefaultSC.Earth.LST");
   CreateParameter("BetaAngle", "DefaultSC.Earth.BetaAngle");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default planet parameters created\n");
   #endif
   
   // B-Plane parameters
   CreateParameter("BdotT", "DefaultSC.Earth.BdotT");
   CreateParameter("BdotR", "DefaultSC.Earth.BdotR");
   CreateParameter("BVectorMag", "DefaultSC.Earth.BVectorMag");
   CreateParameter("BVectorAngle", "DefaultSC.Earth.BVectorAngle");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default b-plane parameters created\n");
   #endif
   
   // Attitude parameters
   CreateParameter("DCM11", "DefaultSC.DCM11");
   CreateParameter("DCM12", "DefaultSC.DCM12");
   CreateParameter("DCM13", "DefaultSC.DCM13");
   CreateParameter("DCM21", "DefaultSC.DCM21");
   CreateParameter("DCM22", "DefaultSC.DCM22");
   CreateParameter("DCM23", "DefaultSC.DCM23");
   CreateParameter("DCM31", "DefaultSC.DCM31");
   CreateParameter("DCM32", "DefaultSC.DCM32");
   CreateParameter("DCM33", "DefaultSC.DCM33");
   CreateParameter("DirectionCosineMatrix", "DefaultSC.DirectionCosineMatrix");
   CreateParameter("EulerAngle1", "DefaultSC.EulerAngle1");
   CreateParameter("EulerAngle2", "DefaultSC.EulerAngle2");
   CreateParameter("EulerAngle3", "DefaultSC.EulerAngle3");
   CreateParameter("MRP1", "DefaultSC.MRP1");  // Dunn Added
   CreateParameter("MRP2", "DefaultSC.MRP2");  // Dunn Added
   CreateParameter("MRP3", "DefaultSC.MRP3");  // Dunn Added
   CreateParameter("Q1", "DefaultSC.Q1");
   CreateParameter("Q2", "DefaultSC.Q2");
   CreateParameter("Q3", "DefaultSC.Q3");
   CreateParameter("Q4", "DefaultSC.Q4");
   CreateParameter("Quaternion", "DefaultSC.Quaternion");
   CreateParameter("AngularVelocityX", "DefaultSC.AngularVelocityX");
   CreateParameter("AngularVelocityY", "DefaultSC.AngularVelocityY");
   CreateParameter("AngularVelocityZ", "DefaultSC.AngularVelocityZ");
   CreateParameter("EulerAngleRate1", "DefaultSC.EulerAngleRate1");
   CreateParameter("EulerAngleRate2", "DefaultSC.EulerAngleRate2");
   CreateParameter("EulerAngleRate3", "DefaultSC.EulerAngleRate3");

   // New Attitude parameters
   CreateParameter("BodyAlignmentVectorX", "DefaultSC.BodyAlignmentVectorX");     
   CreateParameter("BodyAlignmentVectorY", "DefaultSC.BodyAlignmentVectorY");     
   CreateParameter("BodyAlignmentVectorZ", "DefaultSC.BodyAlignmentVectorZ");     
   CreateParameter("BodyConstraintVectorX", "DefaultSC.BodyConstraintVectorX");   
   CreateParameter("BodyConstraintVectorY", "DefaultSC.BodyConstraintVectorY");   
   CreateParameter("BodyConstraintVectorZ", "DefaultSC.BodyConstraintVectorZ");   
   CreateParameter("Attitude", "DefaultSC.Attitude");                             
   CreateParameter("AttitudeReferenceBody", "DefaultSC.AttitudeReferenceBody");   
   CreateParameter("AttitudeConstraintType", "DefaultSC.AttitudeConstraintType"); 


   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default attitude parameters created\n");
   #endif
   
   // Ballistic/Mass parameters
   CreateParameter("DryMass", "DefaultSC.DryMass");
   CreateParameter("DryCenterOfMassX", "DefaultSC.DryCenterOfMassX");
   CreateParameter("DryCenterOfMassY", "DefaultSC.DryCenterOfMassY");
   CreateParameter("DryCenterOfMassZ", "DefaultSC.DryCenterOfMassZ");
   CreateParameter("DryMomentOfInertiaXX", "DefaultSC.DryMomentOfInertiaXX");
   CreateParameter("DryMomentOfInertiaXY", "DefaultSC.DryMomentOfInertiaXY");
   CreateParameter("DryMomentOfInertiaXZ", "DefaultSC.DryMomentOfInertiaXZ");
   CreateParameter("DryMomentOfInertiaYY", "DefaultSC.DryMomentOfInertiaYY");
   CreateParameter("DryMomentOfInertiaYZ", "DefaultSC.DryMomentOfInertiaYZ");
   CreateParameter("DryMomentOfInertiaZZ", "DefaultSC.DryMomentOfInertiaZZ");
   CreateParameter("Cd", "DefaultSC.Cd");
   CreateParameter("Cr", "DefaultSC.Cr");
   CreateParameter("DragArea", "DefaultSC.DragArea");
   CreateParameter("SRPArea", "DefaultSC.SRPArea");
   CreateParameter("TotalMass", "DefaultSC.TotalMass");
   CreateParameter("SystemCenterOfMassX", "DefaultSC.SystemCenterOfMassX");
   CreateParameter("SystemCenterOfMassY", "DefaultSC.SystemCenterOfMassY");
   CreateParameter("SystemCenterOfMassZ", "DefaultSC.SystemCenterOfMassZ");
   CreateParameter("SystemMomentOfInertiaXX", "DefaultSC.SystemMomentOfInertiaXX");
   CreateParameter("SystemMomentOfInertiaXY", "DefaultSC.SystemMomentOfInertiaXY");
   CreateParameter("SystemMomentOfInertiaXZ", "DefaultSC.SystemMomentOfInertiaXZ");
   CreateParameter("SystemMomentOfInertiaYY", "DefaultSC.SystemMomentOfInertiaYY");
   CreateParameter("SystemMomentOfInertiaYZ", "DefaultSC.SystemMomentOfInertiaYZ");
   CreateParameter("SystemMomentOfInertiaZZ", "DefaultSC.SystemMomentOfInertiaZZ");

   CreateParameter("SPADDragScaleFactor", "DefaultSC.SPADDragScaleFactor");
   CreateParameter("SPADSRPScaleFactor", "DefaultSC.SPADSRPScaleFactor");
   CreateParameter("AtmosDensityScaleFactor", "DefaultSC.AtmosDensityScaleFactor");

   CreateParameter("CdSigma", "DefaultSC.CdSigma");
   CreateParameter("CrSigma", "DefaultSC.CrSigma");
   CreateParameter("SPADDragScaleFactorSigma", "DefaultSC.SPADDragScaleFactorSigma");
   CreateParameter("SPADSRPScaleFactorSigma", "DefaultSC.SPADSRPScaleFactorSigma");
   CreateParameter("AtmosDensityScaleFactorSigma", "DefaultSC.AtmosDensityScaleFactorSigma");

   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default ballistic/mass parameters created\n");
   #endif
   
   // STM and A-Matrix parameters
   CreateParameter("OrbitSTM", "DefaultSC.OrbitSTM");
   CreateParameter("OrbitSTMA", "DefaultSC.OrbitSTMA");
   CreateParameter("OrbitSTMB", "DefaultSC.OrbitSTMB");
   CreateParameter("OrbitSTMC", "DefaultSC.OrbitSTMC");
   CreateParameter("OrbitSTMD", "DefaultSC.OrbitSTMD");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default STM parameters created\n");
   #endif
   
   // Covariance parameters
   CreateParameter("OrbitErrorCovariance", "DefaultSC.OrbitErrorCovariance");
   #if DEBUG_DEFAULT_MISSION > 1
   MessageInterface::ShowMessage("-->default covariance parameters created\n");
   #endif
   
   // If DEBUG_PARAMETERS was turned on from the startup file,
   // create hardware Parameters. (LOJ: 2012.03.09)
   // Now always create hardware Parameters since GuiItemManager and ParameterSelectDialog
   // can handle it (LOJ: 2012.06.11)
   //if (GmatGlobal::Instance()->IsWritingParameterInfo())
   //{
      // PowerSystem Parameters
      CreateParameter("TotalPowerAvailable", "DefaultSC.DefaultSolarPowerSystem.TotalPowerAvailable");
      CreateParameter("RequiredBusPower", "DefaultSC.DefaultSolarPowerSystem.RequiredBusPower");
      CreateParameter("ThrustPowerAvailable", "DefaultSC.DefaultSolarPowerSystem.ThrustPowerAvailable");

      // FuelTank Parameters
      CreateParameter("FuelMass", "DefaultSC.DefaultFuelTank.FuelMass");
      CreateParameter("FuelCenterOfMassX",
                      "DefaultSC.DefaultFuelTank.FuelCenterOfMassX");
      CreateParameter("FuelCenterOfMassY",
                      "DefaultSC.DefaultFuelTank.FuelCenterOfMassY");
      CreateParameter("FuelCenterOfMassZ",
                      "DefaultSC.DefaultFuelTank.FuelCenterOfMassZ");
      CreateParameter("FuelMomentOfInertiaXX",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaXX");
      CreateParameter("FuelMomentOfInertiaXY",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaXY");
      CreateParameter("FuelMomentOfInertiaXZ",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaXZ");
      CreateParameter("FuelMomentOfInertiaYY",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaYY");
      CreateParameter("FuelMomentOfInertiaYZ",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaYZ");
      CreateParameter("FuelMomentOfInertiaZZ",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaZZ");
      CreateParameter("FuelCenterOfMassX_BCS",
                      "DefaultSC.DefaultFuelTank.FuelCenterOfMassX_BCS");
      CreateParameter("FuelCenterOfMassY_BCS",
                      "DefaultSC.DefaultFuelTank.FuelCenterOfMassY_BCS");
      CreateParameter("FuelCenterOfMassZ_BCS",
                      "DefaultSC.DefaultFuelTank.FuelCenterOfMassZ_BCS");
      CreateParameter("FuelMomentOfInertiaXX_BCS",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaXX_BCS");
      CreateParameter("FuelMomentOfInertiaXY_BCS",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaXY_BCS");
      CreateParameter("FuelMomentOfInertiaXZ_BCS",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaXZ_BCS");
      CreateParameter("FuelMomentOfInertiaYY_BCS",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaYY_BCS");
      CreateParameter("FuelMomentOfInertiaYZ_BCS",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaYZ_BCS");
      CreateParameter("FuelMomentOfInertiaZZ_BCS",
                      "DefaultSC.DefaultFuelTank.FuelMomentOfInertiaZZ_BCS");
      CreateParameter("Volume", "DefaultSC.DefaultFuelTank.Volume");
      CreateParameter("FuelDensity", "DefaultSC.DefaultFuelTank.FuelDensity");
      CreateParameter("Pressure", "DefaultSC.DefaultFuelTank.Pressure");
      CreateParameter("Temperature", "DefaultSC.DefaultFuelTank.Temperature");
      CreateParameter("RefTemperature", "DefaultSC.DefaultFuelTank.RefTemperature");
      // Thruster Parameters
      CreateParameter("DutyCycle", "DefaultSC.DefaultThruster.DutyCycle");
      CreateParameter("Isp", "DefaultSC.DefaultThruster.Isp");
      CreateParameter("MassFlowRate", "DefaultSC.DefaultThruster.MassFlowRate");
      CreateParameter("ThrustMagnitude", "DefaultSC.DefaultThruster.ThrustMagnitude");
      CreateParameter("ThrustScaleFactor", "DefaultSC.DefaultThruster.ThrustScaleFactor");
      CreateParameter("GravitationalAccel", "DefaultSC.DefaultThruster.GravitationalAccel");
      CreateParameter("C1", "DefaultSC.DefaultThruster.C1");
      CreateParameter("C2", "DefaultSC.DefaultThruster.C2");
      CreateParameter("C3", "DefaultSC.DefaultThruster.C3");
      CreateParameter("C4", "DefaultSC.DefaultThruster.C4");
      CreateParameter("C5", "DefaultSC.DefaultThruster.C5");
      CreateParameter("C6", "DefaultSC.DefaultThruster.C6");
      CreateParameter("C7", "DefaultSC.DefaultThruster.C7");
      CreateParameter("C8", "DefaultSC.DefaultThruster.C8");
      CreateParameter("C9", "DefaultSC.DefaultThruster.C9");
      CreateParameter("C10", "DefaultSC.DefaultThruster.C10");
      CreateParameter("C11", "DefaultSC.DefaultThruster.C11");
      CreateParameter("C12", "DefaultSC.DefaultThruster.C12");
      CreateParameter("C13", "DefaultSC.DefaultThruster.C13");
      CreateParameter("C14", "DefaultSC.DefaultThruster.C14");
      CreateParameter("C15", "DefaultSC.DefaultThruster.C15");
      CreateParameter("C16", "DefaultSC.DefaultThruster.C16");
      CreateParameter("K1", "DefaultSC.DefaultThruster.K1");
      CreateParameter("K2", "DefaultSC.DefaultThruster.K2");
      CreateParameter("K3", "DefaultSC.DefaultThruster.K3");
      CreateParameter("K4", "DefaultSC.DefaultThruster.K4");
      CreateParameter("K5", "DefaultSC.DefaultThruster.K5");
      CreateParameter("K6", "DefaultSC.DefaultThruster.K6");
      CreateParameter("K7", "DefaultSC.DefaultThruster.K7");
      CreateParameter("K8", "DefaultSC.DefaultThruster.K8");
      CreateParameter("K9", "DefaultSC.DefaultThruster.K9");
      CreateParameter("K10", "DefaultSC.DefaultThruster.K10");
      CreateParameter("K11", "DefaultSC.DefaultThruster.K11");
      CreateParameter("K12", "DefaultSC.DefaultThruster.K12");
      CreateParameter("K13", "DefaultSC.DefaultThruster.K13");
      CreateParameter("K14", "DefaultSC.DefaultThruster.K14");
      CreateParameter("K15", "DefaultSC.DefaultThruster.K15");
      CreateParameter("K16", "DefaultSC.DefaultThruster.K16");
      CreateParameter("ThrustDirection1", "DefaultSC.DefaultThruster.ThrustDirection1");
      CreateParameter("ThrustDirection2", "DefaultSC.DefaultThruster.ThrustDirection2");
      CreateParameter("ThrustDirection3", "DefaultSC.DefaultThruster.ThrustDirection3");

      CreateParameter("AreaCoefficient", "DefaultSC.DefaultPlate.AreaCoefficient");
      CreateParameter("SpecularFraction", "DefaultSC.DefaultPlate.SpecularFraction");
      CreateParameter("DiffuseFraction", "DefaultSC.DefaultPlate.DiffuseFraction");
      CreateParameter("Area", "DefaultSC.DefaultPlate.Area");
      //CreateParameter("Type", "DefaultSC.DefaultPlate.Type");
      CreateParameter("LitFraction", "DefaultSC.DefaultPlate.LitFraction");
      CreateParameter("PlateNormal", "DefaultSC.DefaultPlate.PlateNormal");
      CreateParameter("PlateNormalHistoryFile", "DefaultSC.DefaultPlate.PlateNormalHistoryFile");
      CreateParameter("AreaCoefficientSigma", "DefaultSC.DefaultPlate.AreaCoefficientSigma");
      CreateParameter("SpecularFractionSigma", "DefaultSC.DefaultPlate.SpecularFractionSigma");
      CreateParameter("DiffuseFractionSigma", "DefaultSC.DefaultPlate.DiffuseFractionSigma");
      CreateParameter("PlateX", "DefaultSC.DefaultPlate.PlateX");
      CreateParameter("PlateY", "DefaultSC.DefaultPlate.PlateY");
      CreateParameter("PlateZ", "DefaultSC.DefaultPlate.PlateZ");
   //}

   CreateParameter("OrbitTime", "DefaultSC.EarthMJ2000Eq.OrbitTime");
   
   #ifdef DEBUG_CREATE_VAR
   // User variable
   Parameter *var = CreateParameter("Variable", "DefaultSC_EarthMJ2000Eq_Xx2");
   var->SetStringParameter("Expression", "DefaultSC.EarthMJ2000Eq.X * 2.0");
   var->SetRefObjectName(Gmat::PARAMETER, "DefaultSC.EarthMJ2000Eq.X");
   #endif
   
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("-->default parameters created\n");
   #endif

   //============================================================
   // Set Parameter object and dependency object
   //============================================================
   StringArray params = GetListOfObjects(Gmat::PARAMETER);
   Parameter *param;
   
   for (unsigned int i=0; i<params.size(); i++)
   {
      param = GetParameter(params[i]);
      
      // need spacecraft if system parameter
      if (param->GetKey() == GmatParam::SYSTEM_PARAM)
      {
//         if (param->GetOwnerType() == Gmat::SPACECRAFT)
         if ((param->GetOwnerType() == Gmat::SPACECRAFT) || (param->GetOwnerType() == Gmat::SPACE_POINT))
         {
            //MessageInterface::ShowMessage("name = '%s'\n", param->GetName().c_str());
            //param->SetStringParameter("Expression", param->GetName());
            param->SetRefObjectName(Gmat::SPACECRAFT, "DefaultSC");
            
            if (param->NeedCoordSystem())
            {
               param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
               if (param->IsOriginDependent())
                  param->SetStringParameter("DepObject", "Earth");
               else if (param->IsCoordSysDependent())
                  param->SetStringParameter("DepObject", "EarthMJ2000Eq");
            }
         }
         else if (param->GetOwnerType() == Gmat::IMPULSIVE_BURN)
         {
            //MessageInterface::ShowMessage("name = '%s'\n", param->GetName().c_str());
            param->SetRefObjectName(Gmat::IMPULSIVE_BURN, "DefaultIB");
         }
         else if (param->GetOwnerType() == Gmat::FINITE_BURN)
         {
            //MessageInterface::ShowMessage("name = '%s'\n", param->GetName().c_str());
            param->SetRefObjectName(Gmat::FINITE_BURN, "DefaultFB");
         }
      }
   }
   
   #if DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage("-->ref. object to parameters are set\n");
   #endif
}


//------------------------------------------------------------------------------
// void CreateDefaultMission()
//------------------------------------------------------------------------------
void Moderator::CreateDefaultMission()
{
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage("Moderator creating default mission...\n");
   #endif
   
   try
   {
      // Clear SpacePoint instance count so that Spacecraft color starts from the
      // same color for each run
      SpacePoint::ClearInstanceCount();
      
      //----------------------------------------------------
      // Create default resource
      //----------------------------------------------------
      
      // Create solar system in use
      CreateSolarSystemInUse();
      
      // Create default coordinate systems
      CreateDefaultCoordSystems();
      // Create the default Solar System barycenter
      CreateDefaultBarycenter();
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default CoordinateSystem and Barycenter created\n");
      #endif
      
      // Spacecraft
      Spacecraft *sc = (Spacecraft*)CreateSpacecraft("Spacecraft", "DefaultSC");
      sc->SetInternalCoordSystem(theInternalCoordSystem);
      sc->SetRefObject(GetCoordinateSystem("EarthMJ2000Eq"),
                       Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default Spacecraft created\n");
      #endif
      
      // PropSetup
      CreateDefaultPropSetup("DefaultProp");
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default PropSetup created\n");
      #endif
      
      // If DEBUG_PARAMETERS was turned on from the startup file,
      // create tank and thruster. (LOJ: 2012.03.09)
      // Hardware 
      if (GmatGlobal::Instance()->IsWritingParameterInfo())
      {
         CreateHardware("ChemicalTank", "DefaultFuelTank");
         CreateHardware("ChemicalThruster", "DefaultThruster");
         CreateHardware("Plate", "DefaultPlate");
         
         #if DEBUG_DEFAULT_MISSION > 0
         MessageInterface::ShowMessage("-->default hardware created\n");
         #endif
      }
            
      // Default Parameters
      CreateDefaultParameters();
      
      // StopCondition
      StopCondition *stopOnElapsedSecs =
         CreateStopCondition("StopCondition", "StopOnDefaultSC.ElapsedSecs");
      stopOnElapsedSecs->SetStringParameter("EpochVar", "DefaultSC.A1ModJulian");
      stopOnElapsedSecs->SetStringParameter("StopVar", "DefaultSC.ElapsedSecs");
      // Dunn changed ElapsedSecs for default mission to 12000.0 so the spacecraft
      // icon will stop on the near side of the earth where we can see it.  This
      // was required in two locations, so look for it again below.
      stopOnElapsedSecs->SetStringParameter("Goal", "12000.0");
      //stopOnElapsedSecs->SetStringParameter("Goal", "8640.0");
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default StopCondition created\n");
      #endif
      
      // Subscribers
      // OrbitView
      GetDefaultSubscriber("OrbitView");
      GetDefaultSubscriber("GroundTrackPlot");
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default Subscribers created\n");
      #endif
      
      //----------------------------------------------------
      // Create default mission sequence
      //----------------------------------------------------
      bool retval;
      
      // Append BeginMissionSequence command (New since 2010.07.09)
      //GmatCommand *cmd = AppendCommand("BeginMissionSequence", "", retval);
      AppendCommand("BeginMissionSequence", "", retval);
      
      // Propagate Command
      GmatCommand *propCommand = CreateCommand("Propagate", "", retval);
      propCommand->SetObject("DefaultProp", Gmat::PROP_SETUP);
      propCommand->SetObject("DefaultSC", Gmat::SPACECRAFT);
      propCommand->SetRefObject(stopOnElapsedSecs, Gmat::STOP_CONDITION, "", 0);
      propCommand->SetSolarSystem(theSolarSystemInUse);
      
      #if DEBUG_MULTI_STOP
      //----------------------------------------------------
      //just for testing multiple stopping condition
      //----- StopCondition 2
      StopCondition *stopOnX =
         CreateStopCondition("StopCondition", "StopOnDefaultSC.EarthMJ2000Eq.X");
      stopOnX->SetStringParameter("EpochVar", "DefaultSC.A1ModJulian");
      stopOnX->SetStringParameter("StopVar", "DefaultSC.EarthMJ2000Eq.X");
      stopOnX->SetStringParameter("Goal", "5000.0");
      propCommand->SetRefObject(stopOnX, Gmat::STOP_CONDITION, "", 1);
      #endif
      
      #if DEBUG_MULTI_STOP > 1
      StopCondition *stopOnPeriapsis =
         CreateStopCondition("StopCondition", "StopOnDefaultSC.Earth.Periapsis");
      stopOnPeriapsis->SetStringParameter("EpochVar", "DefaultSC.A1ModJulian");
      stopOnPeriapsis->SetStringParameter("StopVar", "DefaultSC.Earth.Periapsis");
      propCommand->SetRefObject(stopOnPeriapsis, Gmat::STOP_CONDITION, "", 2);
      //----------------------------------------------------
      #endif

      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage("-->default Propagate command created\n");
      #endif
      
      // Append Propagate command
      AppendCommand(propCommand);
      
      #if DEBUG_DEFAULT_MISSION
      MessageInterface::ShowMessage
         ("-->Setting SolarSystem <%p> and ObjectMap <%p> to theScriptInterpreter\n",
          theSolarSystemInUse, theConfigManager->GetObjectMap());
      #endif
      
      // Reset initial solar system in use and object map
      objectMapInUse = theConfigManager->GetObjectMap();
      SetSolarSystemAndObjectMap(theSolarSystemInUse, objectMapInUse, false,
                                 "CreateDefaultMission()");
      
      loadSandboxAndPause = false;
      isRunReady = true;
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_,
          "*** Error occurred during default mission creation.\n    The default "
          "mission will not run.\n    Message: " + e.GetFullMessage());
   }
   
   #if DEBUG_INITIALIZE
   MessageInterface::ShowMessage("Moderator successfully created default mission\n");
   #endif
} // CreateDefaultMission()


// Parameter reference object setting
//------------------------------------------------------------------------------
// void CheckParameterType(Parameter **param, const std::string &type,
//                         const std::string &ownerName)
//------------------------------------------------------------------------------
void Moderator::CheckParameterType(Parameter **param, const std::string &type,
                                   const std::string &ownerName)
{
   #ifdef DEBUG_PARAMETER_TYPE
   MessageInterface::ShowMessage
      ("Moderator::CheckParameterType() entered, param=<%p><%s>'%s', type='%s', "
       "ownerName='%s'\n", *param, *param ? (*param)->GetTypeName().c_str() : "NULL",
       *param ? (*param)->GetName().c_str() : "NULL", type.c_str(), ownerName.c_str());
   #endif
   
   GmatBase *obj = FindObject(ownerName);
   if (obj)
   {
      #if DEBUG_CREATE_PARAMETER
      MessageInterface::ShowMessage
         ("   Found owner object, obj=<%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(),
          obj->GetName().c_str());
      MessageInterface::ShowMessage
         ("   parameter owner type = %d, object type = %d\n", (*param)->GetOwnerType(),
          obj->GetType());
      #endif
      UnsignedInt paramType = (*param)->GetOwnerType();
//      if ((*param)->GetOwnerType() != obj->GetType())
      if (!obj->IsOfType(paramType))
      {
         std::string paramOwnerType = GetObjectTypeString((*param)->GetOwnerType());
         
         #ifdef DEBUG_PARAMETER_TYPE
         MessageInterface::ShowMessage
            ("   parameter owner type and object type do not match, paramOwnerType='%s'\n",
             paramOwnerType.c_str());
         #endif
         
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (*param, (*param)->GetName(), "Moderator::CheckParameterType()");
         #endif
         delete *param;
         *param = NULL;
         
         if (paramOwnerType == "")
         {
            #ifdef DEBUG_PARAMETER_TYPE
            MessageInterface::ShowMessage
               ("Moderator::CheckParameterType() throwing exception - "
                "Cannot find the object type which has '%s' as a Parameter type\n",
                type.c_str());
            #endif
            
            throw GmatBaseException
               ("Cannot find the object type which has \"" + type +
                "\" as a Parameter type");
         }
         else
         {
            #ifdef DEBUG_PARAMETER_TYPE
            MessageInterface::ShowMessage
               ("Moderator::CheckParameterType() throwing exception - "
                "Parameter type: '%s' should be property of '%s'\n", type.c_str(),
                paramOwnerType.c_str());
            #endif
            
            if ((type == "Element1") || (type == "Element2") ||
                (type == "Element3"))
            {
               std::string newType =
                     GmatStringUtil::Replace(type, "Element", "ThrustDirection");
               std::string errmsg  = "*** ERROR *** The Parameter type \"";
               errmsg             += type + "\" of Thruster is no longer accepted; ";
               errmsg             += "please use \"" + newType + "\" instead.\n";
               throw GmatBaseException(errmsg);
            }
            else
            {
               // Build detailed Parameter error message (LOJ: 2015.05.18)
               // throw GmatBaseException
               //    ("Parameter type: " + type + " should be property of " +
               //     paramOwnerType);
               ParameterInfo *paramInfo = ParameterInfo::Instance();
               std::string subMsg;
               if (paramInfo->IsForAttachedObject(type))
               {
                  UnsignedInt ownedObjType = paramInfo->GetOwnedObjectType(type);
                  subMsg = GetObjectTypeString(ownedObjType) + " attached to ";
               }
               
               throw GmatBaseException
                  ("Parameter type \"" + type + "\" should be property of " + subMsg +
                   paramOwnerType);
            }
         }
      }
   }
   
   #ifdef DEBUG_PARAMETER_TYPE
   MessageInterface::ShowMessage("Moderator::CheckParameterType() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetParameterRefObject(Parameter *param, const std::string &type, ...)
//------------------------------------------------------------------------------
/**
 * Sets parameter reference object
 *
 * @param <param> parameter pointer
 * @param <type> parameter type
 * @param <name> parameter name
 * @param <ownerName> parameter owner name
 * @param <depName> dependent object name
 */
//------------------------------------------------------------------------------
void Moderator::SetParameterRefObject(Parameter *param, const std::string &type,
                                      const std::string &name,
                                      const std::string &ownerName,
                                      const std::string &depName, Integer manage)
{
   bool debugParameter = true;
   #ifdef DEBUG_ONLY_FOR_SCRIPT
   if (mainScriptFileName == "")
      debugParameter = false;
   #endif
   
   #ifdef DEBUG_ONLY_FOR_FUNCTION
   if (currentFunction == NULL)
      debugParameter = false;
   #endif
   
   #if DEBUG_PARAMETER_REF_OBJ
   if (debugParameter)
   {
      MessageInterface::ShowMessage
         ("Moderator::SetParameterRefObject() entered, param=<%p>, type=<%s>, name=<%s>, "
          "ownerName=<%s>, dep=<%s>, manage=%d\n",  param, type.c_str(), name.c_str(),
          ownerName.c_str(), depName.c_str(), manage);
   }
   #endif
   
   // Why do we need to set expression for non-Variable? (loj: 2008.08.09)
   // Expression is set in the ScriptInterpreter or ParameterCreateDialog for
   // new Parameters
   // If type is Variable, don't set expression
   //if (type != "Variable")
   //   param->SetStringParameter("Expression", name);
   
   // Set parameter owner and dependent object
   if (ownerName != "")
   {
      #if DEBUG_PARAMETER_REF_OBJ
      if (debugParameter)
      {
         GmatBase *owner = FindObject(ownerName);
         MessageInterface::ShowMessage
            ("   Setting Parameter owner object <%p>[%s]'%s'\n", owner,
             owner ? owner->GetTypeName().c_str() : "NULL",
             owner ? owner->GetName().c_str() : "NULL");
      }
      #endif
      param->SetRefObjectName(param->GetOwnerType(), ownerName);
      param->AddRefObject((Parameter*)FindObject(ownerName));
   }
   
   std::string newDep = depName;
   
   // Set dependent object name if depName is not blank,
   // if depName is blank get default dep name
   if (depName != "")
      param->SetStringParameter("DepObject", depName);
   else
      newDep = param->GetStringParameter("DepObject");
   
   // Set SolarSystem
   param->SetSolarSystem(theSolarSystemInUse);
   param->SetInternalCoordSystem(theInternalCoordSystem);
   
   #if DEBUG_PARAMETER_REF_OBJ
   if (debugParameter)
      MessageInterface::ShowMessage
         ("   name='%s', newDep='%s'\n", name.c_str(), newDep.c_str());
   #endif
   
   if (newDep != "")
   {
      GmatBase *depObj = FindObject(newDep);
      if (depObj)
      {
         #if DEBUG_PARAMETER_REF_OBJ
         if (debugParameter)
         {
            MessageInterface::ShowMessage
               ("   Settng depObj=<%p><%s>'%s' to Parameter\n", depObj,
                depObj->GetTypeName().c_str(), depObj->GetName().c_str());
         }
         #endif
         param->AddRefObject(depObj);
      }
   }
   
   // I'm not sure if we always use EarthMJ2000Eq (loj: 2008.06.03)
   if (param->NeedCoordSystem())
      param->AddRefObject(FindObject("EarthMJ2000Eq"));
   
   // create parameter dependent coordinate system
   if (type == "Longitude" || type == "Latitude" || type == "Altitude" ||
       type == "MHA" || type == "LST")
   {
      // need body-fixed CS
      StringTokenizer st(name, ".");
      StringArray tokens = st.GetAllTokens();
      
      if (tokens.size() == 2 || (tokens.size() == 3 && tokens[1] == "Earth"))
      {
         #if DEBUG_PARAMETER_REF_OBJ
         if (debugParameter)
            MessageInterface::ShowMessage("   Creating 'EarthFixed' CoordinateSystem\n");
         #endif
         
         // default EarthFixed
         CoordinateSystem *cs = CreateCoordinateSystem("EarthFixed", false, false, manage);
         param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthFixed");
         // Set CoordinateSystem to param (2008.06.26)
         // It will work without setting CS pointer since EarthFixed is a default
         // CoordinateSyste, but for consistency just set here
         param->SetRefObject(cs, Gmat::COORDINATE_SYSTEM, "EarthFixed");
      }
      else if (tokens.size() == 3)
      {
         std::string origin = tokens[1];
         std::string axisName = origin + "Fixed";
         
         #if DEBUG_PARAMETER_REF_OBJ
         if (debugParameter)
            MessageInterface::ShowMessage
               ("   Creating '%s' CoordinateSystem\n", axisName.c_str());
         #endif
         
         CoordinateSystem *cs = CreateCoordinateSystem(axisName, false, false, manage);
         
         // create BodyFixedAxis with origin
         AxisSystem *axis = CreateAxisSystem("BodyFixed", "BodyFixed_Earth", manage);
         cs->SetStringParameter("Origin", origin);
         cs->SetRefObject(FindObject(origin), Gmat::SPACE_POINT, origin);
         cs->SetRefObject(axis, Gmat::AXIS_SYSTEM, axis->GetName());
         cs->SetJ2000BodyName("Earth");
         cs->SetRefObject(FindObject("Earth"), Gmat::SPACE_POINT, "Earth");
         cs->SetSolarSystem(theSolarSystemInUse);
         cs->Initialize();
         
         // Since CoordinateSystem clones AxisSystem, delete it from here
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (axis, "localAxes", "Moderator::SetParameterRefObject()",
             "deleting localAxes");
         #endif
         delete axis;
         
         param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, axisName);
         // Set CoordinateSystem to param (2008.06.26)
         // This will fix problem with NULL output CS pointer if Paremeter is
         // used in the GmatFunction
         param->SetRefObject(cs, Gmat::COORDINATE_SYSTEM, axisName);
      }
      else
      {
         MessageInterface::ShowMessage("===> Invalid parameter name: %s\n",
                                       name.c_str());
      }
   }
   #if DEBUG_PARAMETER_REF_OBJ
   if (debugParameter)
      MessageInterface::ShowMessage
         ("Moderator::SetParameterRefObject() leaving, param's depObject='%s'\n",
          param->GetStringParameter("DepObject").c_str());
   #endif
}


// object map
//------------------------------------------------------------------------------
// GmatBase* FindObject(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Finds object from the object map in use by name base on objectManageOption.
 * if objectManageOption is 
 *    0, if object is not managed
 *    1, if configuration object map is used for finding objects and adding new
 *          objects
 *    2, if function object map is used for finding objects and adding new
 *          objects including automatic objects
 */
//------------------------------------------------------------------------------
GmatBase* Moderator::FindObject(const std::string &name)
{
   bool debugFindObject = true;
   #ifdef DEBUG_ONLY_FOR_SCRIPT
   if (mainScriptFileName == "")
      debugFindObject = false;
   #endif
   
   #ifdef DEBUG_ONLY_FOR_FUNCTION
   if (currentFunction == NULL)
      debugFindObject = false;
   #endif
   
   #if DEBUG_FIND_OBJECT
   if (debugFindObject)
      MessageInterface::ShowMessage
         ("Moderator::FindObject() entered, name='%s', objectMapInUse=<%p>, "
          "objectManageOption=%d\n", name.c_str(), objectMapInUse, objectManageOption);
   #endif
   
   if (name == "")
   {
      #if DEBUG_FIND_OBJECT
      if (debugFindObject)
         MessageInterface::ShowMessage
            ("Moderator::FindObject() name is blank, so just returning NULL\n");
      #endif
      return NULL;
   }
   
   if (objectMapInUse == NULL)
   {
      #if DEBUG_FIND_OBJECT
      if (debugFindObject)
         MessageInterface::ShowMessage
            ("Moderator::FindObject() objectMapInUse is NULL, so just returning NULL\n");
      #endif
      return NULL;
   }
   
   // Ignore array indexing of Array
   std::string newName = name;
   std::string::size_type index = name.find_first_of("([");
   if (index != name.npos)
   {
      newName = name.substr(0, index);
      
      #if DEBUG_FIND_OBJECT
      if (debugFindObject)
         MessageInterface::ShowMessage("   newName=%s\n", newName.c_str());
      #endif
   }
   
   #if DEBUG_FIND_OBJECT > 1
   if (debugFindObject)
      ShowObjectMap("Moderator::FindObject() Here is the object map in use", objectMapInUse);
   #endif
   
   GmatBase *obj = NULL;
   
   if (objectMapInUse->find(newName) != objectMapInUse->end())
      obj = (*objectMapInUse)[newName];
   
   
   // This code will fix GMT-5162 but need more testing (LOJ: 2015.08.27)
   #if 1
   //======================================================================
   // LOJ: 2015.08.25
   // This code block handles finding an automatically created Parameter
   // such as globalSat.ElapsedDays where globalSat is a global object.
   // For example:
   //    function TestGlobalSat
   //    BeginMissionSequence;
   //    Global globalSat;
   //    Global globalProp;
   //    Propagate globalProp(globalSat) {globalSat.ElapsedDays = 1};
   // When above function is parsed, it is assumed that globalSat is
   // from the sandbox object map since globalSat is not created inside
   // a function.
   //
   // If Global is specified before the BeginMissionSequence, it could
   // be easier to handle global objects
   //======================================================================
   
   // If object is not found and creating inside a function, and not a Parameter,
   // try previous object map (LOJ: 2015.08.25)
   if ((obj == NULL && currentFunction != NULL) &&
       (newName.find_first_of('.') != newName.npos))
   {
      #if DEBUG_FIND_OBJECT
      if (debugFindObject)
         MessageInterface::ShowMessage
            ("   object not found and creating inside a function, so trying "
             "previous object map <%p>\n", previousObjectMap);
      #endif
      
      std::string type, ownerName, depObj;
      GmatStringUtil::ParseParameter(newName, type, ownerName, depObj);
      GmatBase *paramOwner = NULL;
      if (objectMapInUse->find(ownerName) != objectMapInUse->end())
         paramOwner = (*objectMapInUse)[ownerName];
      
      // Parameter owner is not found in the objectMapInUse (at this time it should
      // be function object map), use previous object map (sandbox object map)
      if (paramOwner == NULL)
      {
         if (previousObjectMap != NULL && 
             (previousObjectMap->find(newName) != previousObjectMap->end()))
         {
            GmatBase *objPrevMap = (*previousObjectMap)[newName];
            #if DEBUG_FIND_OBJECT
            if (debugFindObject)
               MessageInterface::ShowMessage
                  (WriteObjectInfo("   object found from the previous object map, ", objPrevMap).c_str());
            #endif
            
            if (previousObjectMap->find(ownerName) != previousObjectMap->end())
            {
               paramOwner = (*previousObjectMap)[ownerName];
               #if DEBUG_FIND_OBJECT
               if (debugFindObject)
                  MessageInterface::ShowMessage
                     (WriteObjectInfo("   paramOwner found from the previous object map, ", paramOwner).c_str());
               #endif
               if (paramOwner->IsGlobal())
                  obj = objPrevMap;
            }
         }
      }
   }
   #endif
   
   
   // Do we need this?
   // Commented out and use objectManageOption below (LOJ: 2015.08.24)
   #if 0
   // check objectManageOption
   Integer manage = 1;
   if (objectManageOption != 1)
      manage = 2;
   #endif
   
   // If object not found, try SolarSystem
   if (obj == NULL)
   {
      //SolarSystem *ss = GetSolarSystemInUse(manage);
      SolarSystem *ss = GetSolarSystemInUse(objectManageOption);
      
      if (ss)
      {
         obj = (GmatBase*)(ss->GetBody(newName));
         #if DEBUG_FIND_OBJECT
         if (debugFindObject && obj)
         {
            MessageInterface::ShowMessage
               ("   Found '%s' from the SolarSystem <%p> in the map\n",
                name.c_str(), ss);
         }
         #endif
      }
   }
   
   #if DEBUG_FIND_OBJECT
   if (debugFindObject)
      MessageInterface::ShowMessage
         ("Moderator::FindObject() returning <%p><%s>'%s'\n", obj, 
          obj ? obj->GetTypeName().c_str() : "NULL",
          obj ? obj->GetName().c_str() : "NULL");
   #endif
   
   return obj;
}

//------------------------------------------------------------------------------
// void AddObjectToObjectMapInUse(const std::string &name, GmatBase *obj,
//                                UnsignedInt objTypeId)
//------------------------------------------------------------------------------
void Moderator::AddObjectToObjectMapInUse(const std::string &name, GmatBase *obj,
                                          UnsignedInt objTypeId)
{
   try
   {
      if (name != "")
      {
         if (objectManageOption == 1)
         {
            theConfigManager->AddObject(objTypeId, obj);
            #if DEBUG_ADD_OBJECT
            MessageInterface::ShowMessage
               ("   ==> '%s' added to configuration\n", name.c_str());
            #endif
         }
         else if (objectManageOption == 2)
            AddObject(obj);
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("In Moderator::AddObjectToObjectMapInUse()\n" + e.GetFullMessage());
   }
}

//------------------------------------------------------------------------------
// bool AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/*
 * Adds object to the objectMapInUse
 *
 * @param  obj  object to add to object map
 *
 * @return true if successfully added, false otherwise
 */
//------------------------------------------------------------------------------
bool Moderator::AddObject(GmatBase *obj)
{
   #if DEBUG_ADD_OBJECT
   MessageInterface::ShowMessage
      ("Moderator::AddObject() entered, obj=<%p><%s><%s>, objectMapInUse=<%p>\n",
       obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", objectMapInUse);
   #endif
   
   if (obj == NULL || (obj && obj->GetName() == ""))
   {
      #if DEBUG_ADD_OBJECT
      MessageInterface::ShowMessage
         ("Moderator::AddObject() returning false, has NULL obj or name is blank\n");
      #endif
      
      return false;
   }
   
   if (objectMapInUse == NULL)
      throw GmatBaseException
         ("Moderator::AddObject() cannot add object named \"" + obj->GetName() +
          "\" to unset object map in use");
   
   #ifdef DEBUG_OBJECT_MAP
   MessageInterface::ShowMessage
      ("Moderator::Moderator::AddObject() Adding <%p><%s>'%s' to objectMapInUse <%p>\n",
       obj, obj->GetTypeName().c_str(), obj->GetName().c_str(), objectMapInUse);
   #endif
   
   // if name not found in the object map, then insert (loj: 2008.12.16)
   if (objectMapInUse->find(obj->GetName()) == objectMapInUse->end())
      objectMapInUse->insert(std::make_pair(obj->GetName(), obj));
   else
   {
      #if DEBUG_ADD_OBJECT
      MessageInterface::ShowMessage
         ("*** WARNING *** Moderator::AddObject() '%s' is already in the object map, so ignored\n",
          obj->GetName().c_str());
      #endif
   }
   
   #if DEBUG_ADD_OBJECT
   MessageInterface::ShowMessage("Moderator::AddObject() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void SetSolarSystemAndObjectMap(SolarSystem *ss, ObjectMap *objMap,
//         bool forFunction, const std::string &callFrom)
//------------------------------------------------------------------------------
/*
 * Sets the solar system in use and configured object map to interpreters.
 *
 * @param <ss> Pointer to the solar system
 * @param <objMap> Pointer to the object map
 * @param <forFunction> True if setting object map for function (false)
 */
//------------------------------------------------------------------------------
void Moderator::SetSolarSystemAndObjectMap(SolarSystem *ss, ObjectMap *objMap,
                                           bool forFunction,
                                           const std::string &callFrom)
{
   #if DEBUG_OBJECT_MAP
   MessageInterface::ShowMessage
      ("=====> Moderator::%s setting solarSystemInUse=<%p>, "
       "objectMapInUse=<%p> %s\n", callFrom.c_str(), ss, objMap,
       forFunction ? "for function" : "");
   ShowObjectMap("In Moderator::SetSolarSystemAndObjectMap()", objMap);
   #endif
   
   // Set solar system in use and object map 
   theScriptInterpreter->SetSolarSystemInUse(ss);
   theScriptInterpreter->SetObjectMap(objMap);
   if (!forFunction)
      theScriptInterpreter->SetFunction(NULL);
   if (theUiInterpreter != NULL)
   {
      theUiInterpreter->SetSolarSystemInUse(ss);
      theUiInterpreter->SetObjectMap(objMap);
      if (!forFunction)
         theUiInterpreter->SetFunction(NULL);
   }
}


//------------------------------------------------------------------------------
// bool IsSequenceStarter(const std::string &commandType)
//------------------------------------------------------------------------------
/*
 * Determines if a command identifies a mission control sequence start command
 *
 * @param commandType Type name for the command
 *
 * @return true if the command is a MCS start command
 */
//------------------------------------------------------------------------------
bool Moderator::IsSequenceStarter(const std::string &commandType)
{
   bool retval = false;

   if (sequenceStarters.empty())
      GetSequenceStarters();
   if (find(sequenceStarters.begin(), sequenceStarters.end(), commandType) != 
         sequenceStarters.end())
      retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// const StringArray& GetSequenceStarters()
//------------------------------------------------------------------------------
/*
 * Retrieves a StringArray listing the mission control sequence start commands
 *
 * @return The array naming the MCS start commands
 */
//------------------------------------------------------------------------------
const StringArray& Moderator::GetSequenceStarters()
{
   sequenceStarters.clear();
   sequenceStarters = theFactoryManager->GetListOfItems(Gmat::COMMAND, "SequenceStarters");

   return sequenceStarters;
}

//------------------------------------------------------------------------------
// const std::string& GetStarterStringList()
//------------------------------------------------------------------------------
/*
 * Retrieves a string listing the mission control sequence start commands
 *
 * This method is used to set teh list in exception related to the start commands
 *
 * @return The array naming the MCS start commands
 */
//------------------------------------------------------------------------------
const std::string& Moderator::GetStarterStringList()
{
   if (starterList == "")
   {
      if (sequenceStarters.empty())
         GetSequenceStarters();
      for (UnsignedInt i = 0; i < sequenceStarters.size(); ++i)
      {
         starterList += sequenceStarters[i];
         if (i+1 < sequenceStarters.size())
            starterList += ", ";
      }
   }

   return starterList;
}


// default objects
//------------------------------------------------------------------------------
// Spacecraft* GetDefaultSpacecraft()
//------------------------------------------------------------------------------
/*
 * Returns first spacecraft not in the Formation
 */
//------------------------------------------------------------------------------
Spacecraft* Moderator::GetDefaultSpacecraft()
{
   StringArray soConfigList = GetListOfObjects(Gmat::SPACECRAFT);

   #ifdef DEBUG_DEFAULT_COMMAND
   MessageInterface::ShowMessage
      ("Moderator::GetDefaultSpacecraft() entered, there are %d spacecrafts\n",
       soConfigList.size());
   #endif
   if (soConfigList.size() > 0)
   {
      // return 1st Spacecraft
      SpaceObject *so = GetSpacecraft(soConfigList[0]);
      #ifdef DEBUG_DEFAULT_COMMAND
      MessageInterface::ShowMessage
         ("Moderator::GetDefaultSpacecraft() returning existing spacecraft <%p>'%s'\n",
          so, so->GetName().c_str());
      #endif
      return (Spacecraft*)so;
   }
   else
   {
      // create Spacecraft
      SpaceObject *so = CreateSpacecraft("Spacecraft", "DefaultSC");
      #ifdef DEBUG_DEFAULT_COMMAND
      MessageInterface::ShowMessage
         ("Moderator::GetDefaultSpacecraft() returning new spacecraft <%p>'%s'\n",
          so, so->GetName().c_str());
      #endif
      return (Spacecraft*)so;
   }
}


//------------------------------------------------------------------------------
// PropSetup* GetDefaultPropSetup()
//------------------------------------------------------------------------------
PropSetup* Moderator::GetDefaultPropSetup()
{
   StringArray configList = GetListOfObjects(Gmat::PROP_SETUP);
   
   if (configList.size() > 0)
   {
      // return 1st PropSetup from the list
      return GetPropSetup(configList[0]);
   }
   else
   {
      // create PropSetup
      return CreateDefaultPropSetup("DefaultProp");
   }
}


//------------------------------------------------------------------------------
// Burn* GetDefaultBurn(const std::string &type)
//------------------------------------------------------------------------------
Burn* Moderator::GetDefaultBurn(const std::string &type)
{
   StringArray configList = GetListOfObjects(Gmat::BURN);
   #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::GetDefaultBurn() entered, type='%s', burnList.size()=%d\n",
       type.c_str(), configList.size());
   #endif
   
   if (configList.size() > 0)
   {
      for (UnsignedInt i=0; i<configList.size(); i++)
         if (GetBurn(configList[i])->IsOfType(type))
         {
            #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
            MessageInterface::ShowMessage
               ("Moderator::GetDefaultBurn() returning configured burn <%p>\n",
                configList[i]);
            #endif
            return GetBurn(configList[i]);
         }
   }
   
   Burn *burn = NULL;
   
   if (type == "ImpulsiveBurn")
      burn = CreateBurn("ImpulsiveBurn", "DefaultIB");
   else if (type == "FiniteBurn")
      burn = CreateBurn("FiniteBurn", "DefaultFB");
   
   #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::GetDefaultBurn() returning new burn <%p>\n", burn);
   #endif
   return burn;
}


//------------------------------------------------------------------------------
// Hardware* GetDefaultHardware(const std::string &type)
//------------------------------------------------------------------------------
Hardware* Moderator::GetDefaultHardware(const std::string &type)
{
   StringArray configList = GetListOfObjects(Gmat::HARDWARE);

   if (configList.size() > 0)
   {
      for (UnsignedInt i=0; i<configList.size(); i++)
         if (GetHardware(configList[i])->IsOfType(type))
            return GetHardware(configList[i]);
   }
   
   Hardware *hw = NULL;
   
   if (type == "FuelTank")
      hw = CreateHardware("ChemicalTank", "DefaultFuelTank");
   else if (type == "Thruster")
      hw = CreateHardware("ChemicalThruster", "DefaultThruster");
   else if (type == "Plate")
      hw = CreateHardware("Plate", "DefaultPlate");

   return hw;
}


//------------------------------------------------------------------------------
// Subscriber* GetDefaultSubscriber(const std::string &type, bool addObjects = true,
//                                  bool createIfNoneFound = true)
//------------------------------------------------------------------------------
/**
 * Returns default subcriber of given type, if createIfNoneFound is true, it will
 * create default subscriber.
 */
//------------------------------------------------------------------------------
Subscriber* Moderator::GetDefaultSubscriber(const std::string &type, bool addObjects,
                                            bool createIfNoneFound)
{
   #ifdef DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage
      ("Moderator::GetDefaultSubscriber() entered, type='%s'\n", type.c_str());
   #endif
   
   StringArray configList = GetListOfObjects(Gmat::SUBSCRIBER);
   int subSize = configList.size();
   Subscriber *sub = NULL;
   
   for (int i=0; i<subSize; i++)
   {
      sub = (Subscriber*)GetConfiguredObject(configList[i]);
      if (sub->GetTypeName() == type)
         return sub;
   }
   
   // If not ToggleSubscriber and not creating default subscriber, just return NULL
   if ((type != "ToggleSubscriber" && type != "PenUpDownSubscriber") && !createIfNoneFound)
      return NULL;
   
   if (type == "PenUpDownSubscriber")
   {
      // First look for Subscribers for PenUp/Down command in the order of
      // OrbitView, GroundTrackPlot, XYPlot
      Subscriber *orbitView = GetDefaultSubscriber("OrbitView", true, false);
      Subscriber *groundTrack = GetDefaultSubscriber("GroundTrackPlot", true, false);
      Subscriber *xyPlot = GetDefaultSubscriber("XYPlot", true, false);
      if (orbitView)         return orbitView;
      else if (groundTrack)  return groundTrack;
      else if (xyPlot)       return xyPlot;
      
      // If none found, create OrbitView as default toggle subscriber
      return GetDefaultSubscriber("OrbitView", true, true);
   }
   else if (type == "ToggleSubscriber")
   {
      // First look for Subscribers for Toggle command in the order of
      // OrbitView, GroundTrackPlot, XYPlot, ReportFile, and EphemerisFile.
      // (LOJ:2012.08.08 Fix for GMT-2374)
      Subscriber *orbitView = GetDefaultSubscriber("OrbitView", true, false);
      Subscriber *groundTrack = GetDefaultSubscriber("GroundTrackPlot", true, false);
      Subscriber *xyPlot = GetDefaultSubscriber("XYPlot", true, false);
      Subscriber *reportFile = GetDefaultSubscriber("ReportFile", true, false);
      Subscriber *ephemFile = GetDefaultSubscriber("EphemerisFile", true, false);
      if (orbitView)         return orbitView;
      else if (groundTrack)  return groundTrack;
      else if (xyPlot)       return xyPlot;
      else if (reportFile)   return reportFile;
      else if (ephemFile)    return ephemFile;
      
      // If none found, create OrbitView as default toggle subscriber
      return GetDefaultSubscriber("OrbitView", true, true);
   }
   else if (type == "OrbitView")
   {
      // create default OrbitView
      sub = CreateSubscriber("OrbitView", "DefaultOrbitView");
      sub->SetStringParameter("Add", "DefaultSC");
      sub->SetStringParameter("Add", "Earth");
      sub->SetStringParameter("CoordinateSystem", "EarthMJ2000Eq");
      sub->SetStringParameter("ViewPointVector", "[30000 0 0]");
      sub->Activate(true);
   }
   else if (type == "GroundTrackPlot")
   {
      // create default GroundTrackPlot
      sub = CreateSubscriber("GroundTrackPlot", "DefaultGroundTrackPlot");
      sub->SetStringParameter("Add", "DefaultSC");
      sub->SetStringParameter("Add", "Earth");
      sub->Activate(true);
   }
   else if (type == "XYPlot")
   {
      // create default XYPlot
      sub = CreateSubscriber("XYPlot", "DefaultXYPlot");
      sub->SetStringParameter("XVariable", "DefaultSC.A1ModJulian");
      sub->SetStringParameter("YVariables", "DefaultSC.EarthMJ2000Eq.X", 0);      
      sub->SetStringParameter("YVariables", "DefaultSC.EarthMJ2000Eq.Y", 1);
      sub->SetStringParameter("YVariables", "DefaultSC.EarthMJ2000Eq.Z", 2);
      sub->Activate(true);
      
      // To validate and create element wrappers
      theScriptInterpreter->ValidateSubscriber(sub);
   }
   else if (type == "ReportFile")
   {
      // create default ReportFile
      sub = CreateSubscriber("ReportFile", "DefaultReportFile");
      std::string scName = GetDefaultSpacecraft()->GetName();
      sub->SetStringParameter(sub->GetParameterID("Filename"),
                              "DefaultReportFile.txt");
      
      if (addObjects)
      {
         sub->SetStringParameter("Add", scName + ".A1ModJulian");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.X");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.Y");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.Z");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.VX");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.VY");
         sub->SetStringParameter("Add", scName + ".EarthMJ2000Eq.VZ");
      }
      sub->Activate(true);
      
      // To validate and create element wrappers
      theScriptInterpreter->ValidateSubscriber(sub);
   }
   else if (type == "EphemerisFile")
   {
      // create default EphemerisFile
      sub = CreateSubscriber("EphemerisFile", "DefaultEphemerisFile");
      // add default spacecraft and coordinate system
      sub->SetStringParameter("Spacecraft", GetDefaultSpacecraft()->GetName());
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** GetDefaultSubscriber() Undefined subscriber type: %s\n",
          type.c_str());
   }

   #ifdef DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage
      ("Moderator::GetDefaultSubscriber() returning, <%p>'%s'\n", sub,
       sub->GetName().c_str());
   #endif
   return sub;
}

//------------------------------------------------------------------------------
// Solver* GetDefaultBoundaryValueSolver()
//------------------------------------------------------------------------------
/**
 * Retrives configured boundary value solver object. If none exist, it creates 
 * a DifferentialCorrector as a default.
 */
//------------------------------------------------------------------------------
Solver* Moderator::GetDefaultBoundaryValueSolver()
{
   StringArray configList = GetListOfObjects(Gmat::SOLVER);
   Integer numSolver = configList.size();
   #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::GetDefaultBoundaryValueSolver() entered, numSolver=%d\n",
       numSolver);
   #endif
   
   GmatBase *obj = NULL;
   
   if (numSolver > 0)
   {
      for (Integer i = 0; i < numSolver; i++)
      {
         obj = GetConfiguredObject(configList[i]);
         if (obj->IsOfType("BoundaryValueSolver"))
         {
            #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
            MessageInterface::ShowMessage
               ("Moderator::GetDefaultBoundaryValueSolver() returning configured "
                "solver <%p>\n", obj);
            #endif
            return (Solver*)obj;
         }
      }
   }
   
   // create default boundary value Solver
   Solver *defSolver = CreateSolver("DifferentialCorrector", "DefaultDC");
   #ifdef DEBUG_CREATE_DEFAULT_RESOURCE
   MessageInterface::ShowMessage
      ("Moderator::GetDefaultBoundaryValueSolver() returning new solver <%p>\n", defSolver);
   #endif
   return defSolver;
}

//------------------------------------------------------------------------------
// Solver* GetDefaultOptimizer()
//------------------------------------------------------------------------------
/**
 * Retrives configured Optimizer object. If none exist, it creates FminconOptimizer
 * as a default.
 */
//------------------------------------------------------------------------------
Solver* Moderator::GetDefaultOptimizer()
{
   StringArray configList = GetListOfObjects(Gmat::SOLVER);
   Integer numSolver = configList.size();
   GmatBase *obj = NULL;
   
   if (numSolver > 0)
   {
      for (Integer i = 0; i < numSolver; i++)
      {
         obj = GetConfiguredObject(configList[i]);
         if (obj->IsOfType("Optimizer"))
            return (Solver*)obj;
      }
   }
   
   // Create default Optimizer
   return CreateSolver("FminconOptimizer", "DefaultSQP");
}

//------------------------------------------------------------------------------
// EventLocator* GetDefaultEventLocator()
//------------------------------------------------------------------------------
/**
 * Retrieves configured EventLocator object. If none exist, it creates an
 * EclipseLocator as a default (if it is available; else none)
 */
//------------------------------------------------------------------------------
EventLocator* Moderator::GetDefaultEventLocator()
{
   StringArray configList = GetListOfObjects(Gmat::EVENT_LOCATOR);
   Integer numLocator     = configList.size();
   GmatBase *obj = NULL;

   if (numLocator > 0)
   {
      // return 1st EventLocator
      EventLocator *el = GetEventLocator(configList[0]);
      #ifdef DEBUG_DEFAULT_COMMAND
      MessageInterface::ShowMessage
         ("Moderator::GetDefaultEventLocator() returning existing event locator <%p>'%s'\n",
          el, el->GetName().c_str());
      #endif
      return (EventLocator*)el;
   }

   StringArray locatorsAvailable = theFactoryManager->GetListOfItems(Gmat::EVENT_LOCATOR);
   for (unsigned int ii = 0; ii < locatorsAvailable.size(); ii++)
   {
      if (locatorsAvailable.at(ii) == "EclipseLocator")
      {
         // Create default EventLocator if EclipseLocator is available
         return CreateEventLocator("EclipseLocator", "DefaultEclipseLocator");
      }
   }
   // Cannot create a default EventLocator so returning NULL
   return NULL;

}


//------------------------------------------------------------------------------
// StopCondition* CreateDefaultStopCondition()
//------------------------------------------------------------------------------
StopCondition* Moderator::CreateDefaultStopCondition()
{
   StopCondition *stopCond = NULL;
   Parameter *param;
   
   Spacecraft *sc = GetDefaultSpacecraft();
   std::string scName = sc->GetName();
   
   std::string epochVar = scName + ".A1ModJulian";
   std::string stopVar = scName + ".ElapsedSecs";
   
   #ifdef DEBUG_DEFAULT_MISSION
   MessageInterface::ShowMessage
      ("Moderator::CreateDefaultStopCondition() scName=%s, epochVar=%s, "
       "stopVar=%s\n", scName.c_str(), epochVar.c_str(), stopVar.c_str());
   #endif
   
   if (GetParameter(epochVar) == NULL)
   {
      param = CreateParameter("A1ModJulian", epochVar);
      param->SetRefObjectName(Gmat::SPACECRAFT, scName);
   }
   
   if (GetParameter(stopVar) == NULL)
   {
      param = CreateParameter("ElapsedSecs", stopVar);
      param->SetRefObjectName(Gmat::SPACECRAFT, scName);
   }
   
   std::string stopCondName = "StopOn" + stopVar;
   
   stopCond = CreateStopCondition("StopCondition", "StopOn" + stopVar);
   
   stopCond->SetStringParameter("EpochVar", epochVar);
   stopCond->SetStringParameter("StopVar", stopVar);
   // Dunn changed ElapsedSecs for default mission to 12000.0 so the spacecraft
   // icon will stop on the near side of the earth where we can see it.
   stopCond->SetStringParameter("Goal", "12000.0");
   //stopCond->SetStringParameter("Goal", "8640.0");
   return stopCond;
}


//------------------------------------------------------------------------------
// Parameter* GetDefaultX()
//------------------------------------------------------------------------------
Parameter* Moderator::GetDefaultX()
{
   Spacecraft *sc = GetDefaultSpacecraft();
   Parameter* param = GetParameter(sc->GetName() + ".A1ModJulian");

   if (param == NULL)
   {
      param = CreateParameter("A1ModJulian", sc->GetName() + ".A1ModJulian");
      param->SetRefObjectName(Gmat::SPACECRAFT, sc->GetName());
   }
   
   return param;
}


//------------------------------------------------------------------------------
// Parameter* GetDefaultY()
//------------------------------------------------------------------------------
Parameter* Moderator::GetDefaultY()
{
   Spacecraft *sc = GetDefaultSpacecraft();
   Parameter* param = GetParameter(sc->GetName() + ".EarthMJ2000Eq.X");
   
   if (param == NULL)
   {
      param = CreateParameter("X", sc->GetName() + ".EarthMJ2000Eq.X");
      param->SetRefObjectName(Gmat::SPACECRAFT, sc->GetName());
   }
   
   return param;
}


// Sandbox
//------------------------------------------------------------------------------
// void AddFunctionToGlobalObjectMap(GmatBase *func)
//------------------------------------------------------------------------------
/**
 * This method is called when Sandbox triggers calling GmatFunction during
 * the Sandbox initialization. So GmatFunction created inside a function
 * should be added to Sanbox global object map
 */
//------------------------------------------------------------------------------
void Moderator::AddFunctionToGlobalObjectMap(Function *func)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddFunctionToGlobalObjectMap() adding, func=<%p>'%s' to "
       "Sandbox global object map\n", func, func ? func->GetName().c_str() : "NULL");
   #endif
   
   // Will using the last sandbox number work for multiple sanbox?
   if (currentSandboxNumber > 0 && currentSandboxNumber <= Gmat::MAX_SANDBOX)
   {
      sandboxes[currentSandboxNumber-1]->AddFunctionToGlobalObjectMap(func);
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::ERROR_, "Adding Function to empty or invalid Sandbox number" +
          std::to_string(currentSandboxNumber));
   }
}

//------------------------------------------------------------------------------
// void AddSolarSystemToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSolarSystemToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddSolarSystemToSandbox() entered\n");
   MessageInterface::ShowMessage
      ("   Adding theSolarSystemInUse<%p> to Sandbox\n", theSolarSystemInUse);
   #endif
   
   //If we are ready to configure SolarSystem by name
   // Script will have something like, "Create SolarSystem anotherSS;"
   #ifdef __USE_CONFIGURED_SOLAR_SYSTEM__
      SolarSystem *solarSys = theConfigManager->GetSolarSystemInUse(name);
      sandboxes[index]->AddSolarSystem(solarSys);
   #else
      sandboxes[index]->AddSolarSystem(theSolarSystemInUse);
   #endif
}


//------------------------------------------------------------------------------
// void Moderator::AddTriggerManagersToSandbox(Integer index)
//------------------------------------------------------------------------------
/**
 * Passes TriggerManager array to a Sandbox so the Sandbox can clone managers
 *
 * @param index The index of the sandbox getting the trigger managers
 */
//------------------------------------------------------------------------------
void Moderator::AddTriggerManagersToSandbox(Integer index)
{
   sandboxes[index]->AddTriggerManagers(&triggerManagers);
}


//------------------------------------------------------------------------------
// void AddInternalCoordSystemToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddInternalCoordSystemToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddInternalCoordSystemToSandbox() entered.\n");
   MessageInterface::ShowMessage
      ("   Adding theInternalCoordSystem<%p> to Sandbox\n", theInternalCoordSystem);
   #endif
   
   sandboxes[index]->SetInternalCoordSystem(theInternalCoordSystem);
   
}


//------------------------------------------------------------------------------
// void AddPublisherToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddPublisherToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddPublisherToSandbox() entered.\n");
   MessageInterface::ShowMessage
      ("   Adding thePublisher<%p> to Sandbox\n", thePublisher);
   #endif
   
   thePublisher->UnsubscribeAll();
   sandboxes[index]->SetPublisher(thePublisher);
   
}


//------------------------------------------------------------------------------
// void HandleCcsdsEphemerisFile(ObjectMap *objMap, bool deleteOld = false)
//------------------------------------------------------------------------------
/**
 * Creates plug-in CcsdsEphemerisFile object if EphemerisFile type is CCSDS
 * and replaces the old one.
 */
//------------------------------------------------------------------------------
void Moderator::HandleCcsdsEphemerisFile(ObjectMap *objMap, bool deleteOld)
{
   #ifdef DEBUG_CCSDS_EPHEMERIS
   MessageInterface::ShowMessage
      ("Moderator::HandleCcsdsEphemerisFile() entered, objMap=<%p>\n", objMap);
   #endif
   
   GmatBase *obj;
   
   for (ObjectMap::iterator i = objMap->begin(); i != objMap->end(); ++i)
   {
      obj = i->second;
      
      if (!obj)
         throw GmatBaseException
               ("Moderator::AddSubscriberToSandbox() Error in objMap\n");
      
      //==============================================================
      // Special handling for CcsdsEphemerisFile plug-in
      //==============================================================
      // This is needed since we create EphemerisFile object first
      // from the script "Create EphemerisFile" and then create
      // CcsdsEphemerisFile if file format contains CCSDS.
      // It will create CcsdsEphemerisFile object via
      // plug-in factory and replace the object pointer.
      //==============================================================
      // Create CcsdsEphemerisFile if file format is CCSDS
      if (obj->IsOfType(Gmat::EPHEMERIS_FILE))
      {
         std::string name = obj->GetName();
         std::string format = obj->GetStringParameter("FileFormat");
         
         #ifdef DEBUG_CCSDS_EPHEMERIS
         MessageInterface::ShowMessage
            ("   Format of the object<%p><%s>'%s' is '%s'\n",
             obj, obj->GetTypeName().c_str(), name.c_str(), format.c_str());
         #endif
         
         if (format.find("CCSDS") != format.npos)
         {
            // Check type name to avoid recreating a CcsdsEphemerisFile object for re-runs
            if (obj->GetTypeName() != "CcsdsEphemerisFile")
            {
               #ifdef DEBUG_CCSDS_EPHEMERIS
               MessageInterface::ShowMessage("   About to create new CcsdsEphemerisFile\n");
               #endif
               
               // Create unnamed CcsdsEphemerisFile
               GmatBase *newObj = CreateEphemerisFile("CcsdsEphemerisFile", "");
               if (newObj == NULL)
               {
                  throw GmatBaseException
                     ("Moderator::AddSubscriberToSandbox() Cannot continue due to missing "
                      "CcsdsEphemerisFile plugin dll\n");
               }
               
               newObj->SetName(name);
               ResetObjectPointer(objMap, newObj, name);
               ResetObjectPointer(objectMapInUse, newObj, name);
               newObj->Copy(obj);
               newObj->TakeAction("ChangeTypeName", "CcsdsEphemerisFile");
               
               #ifdef DEBUG_CCSDS_EPHEMERIS
               MessageInterface::ShowMessage
                  ("   New object <%p><%s>'%s' created\n", newObj, newObj->GetTypeName().c_str(),
                   name.c_str());
               #endif
               
               GmatBase *oldObj = obj;
               obj = newObj;
               
               // Delete old object on option
               if (deleteOld)
               {
                  #ifdef DEBUG_CCSDS_EPHEMERIS
                  MessageInterface::ShowMessage
                     ("   Deleting old object <%p><%s>'%s'\n", oldObj, oldObj->GetTypeName().c_str(),
                      name.c_str());
                  #endif
                  #ifdef DEBUG_MEMORY
                  MemoryTracker::Instance()->Remove
                     (oldObj, oldObj->GetName(), "Moderator::HandleCcsdsEphemerisFile()");
                  #endif
                  delete oldObj;
               }
            }
         }
      }
   }
   
   #ifdef DEBUG_CCSDS_EPHEMERIS
   ShowObjectMap("In Moderator::HandleCcsdsEphemerisFile()", objMap);
   MessageInterface::ShowMessage
      ("Moderator::HandleCcsdsEphemerisFile() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void AddSuscriberToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddSubscriberToSandbox(Integer index)
{
   #ifdef __USE_DATAFILE__
   // Handle CcsdsEphemerisFile which uses DataFile plugin
   HandleCcsdsEphemerisFile(objectMapInUse, false);
   #endif
   
   Subscriber *obj;
   StringArray names = theConfigManager->GetListOfItems(Gmat::SUBSCRIBER);
   
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddSubscriberToSandbox() count = %d\n", names.size());
   #endif
   
   for (Integer i=0; i<(Integer)names.size(); i++)
   {
      obj = theConfigManager->GetSubscriber(names[i]);
      sandboxes[index]->AddSubscriber(obj);
      
      #if DEBUG_RUN > 1
      MessageInterface::ShowMessage
         ("   Adding <%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(), 
          obj->GetName().c_str());
      #endif
   }
}


//------------------------------------------------------------------------------
// void AddOtherObjectsToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddOtherObjectsToSandbox(Integer index)
{
   GmatBase *obj;
   StringArray names = theConfigManager->GetListOfAllItems();
   
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddOtherObjectsToSandbox() entered, allItemCount = %d\n", names.size());
   for (Integer i=0; i<(Integer)names.size(); i++)
      MessageInterface::ShowMessage("   names[%2d] = '%s'\n", i, names[i].c_str());
   #endif
   
   for (Integer i=0; i<(Integer)names.size(); i++)
   {
      obj = theConfigManager->GetItem(names[i]);
      if (obj != NULL)
      {
         // Skip subscribers since those are handled separately
         if (obj->IsOfType(Gmat::SUBSCRIBER))
            continue;
         
         #ifdef DEBUG_RUN
         MessageInterface::ShowMessage
            ("   Adding <%p><%s>'%s'\n", obj, obj->GetTypeName().c_str(),
             obj->GetName().c_str());
         #endif
         sandboxes[index]->AddObject(obj);
      }
   }
}


//------------------------------------------------------------------------------
// void AddCommandToSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::AddCommandToSandbox(Integer index)
{
   #if DEBUG_RUN
   MessageInterface::ShowMessage
      ("Moderator::AddCommandToSandbox() entered\n");
   #endif
   
   GmatCommand *cmd = commands[index]->GetNext();
   
   if (cmd != NULL)
   {
      #if DEBUG_RUN > 1
      MessageInterface::ShowMessage
         ("   Adding <%p><%s>\n", cmd, cmd->GetTypeName().c_str());
      #endif
      
      sandboxes[index]->AddCommand(cmd);
   }
}


//------------------------------------------------------------------------------
// void InitializeSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::InitializeSandbox(Integer index)
{
   sandboxes[index]->Initialize();
}

//------------------------------------------------------------------------------
// void ExecuteSandbox(Integer index)
//------------------------------------------------------------------------------
void Moderator::ExecuteSandbox(Integer index)
{
   sandboxes[index]->Execute();
}

//---------------------------------
// private
//---------------------------------

//------------------------------------------------------------------------------
// void ShowCommand(const std::string &title1, GmatCommand *cmd1,
//                  const std::string &title2, GmatCommand *cmd2)
//------------------------------------------------------------------------------
void Moderator::ShowCommand(const std::string &title1, GmatCommand *cmd1,
                            const std::string &title2, GmatCommand *cmd2)
{
   if (title2 == "")
   {
      if (cmd1 == NULL)
         MessageInterface::ShowMessage("%s<%p><NULL>\n", title1.c_str(), cmd1);
      else
         MessageInterface::ShowMessage
            ("%s<%p><%s>\n", title1.c_str(), cmd1, cmd1->GetTypeName().c_str());
   }
   else
   {
      if (cmd2 == NULL)
         MessageInterface::ShowMessage
            ("%s<%p><NULL>%s<%p><NULL>\n", title1.c_str(), cmd1, title2.c_str(), cmd2);
      else
         MessageInterface::ShowMessage
            ("%s<%p><%s>%s<%p><%s>\n", title1.c_str(), cmd1, cmd1->GetTypeName().c_str(),
             title2.c_str(), cmd2, cmd2->GetTypeName().c_str());
   }
}

//------------------------------------------------------------------------------
// void ShowMissionSequence(const std::string &msg)
//------------------------------------------------------------------------------
void Moderator::ShowMissionSequence(const std::string &msg)
{
   MessageInterface::ShowMessage("%s", msg.c_str());
   GmatCommand *cmd = GetFirstCommand();
   MessageInterface::ShowMessage(GmatCommandUtil::GetCommandSeqString(cmd));   
}

//------------------------------------------------------------------------------
// void ShowObjectMap(const std::string &title, ObjectMap *objMap = NULL)
//------------------------------------------------------------------------------
void Moderator::ShowObjectMap(const std::string &title, ObjectMap *objMap)
{
   bool showObjectMap = true;
   #ifdef DEBUG_ONLY_FOR_SCRIPT
   if (mainScriptFileName == "")
      showObjectMap = false;
   #endif
   // #ifdef DEBUG_ONLY_FOR_FUNCTION
   // if (currentFunction == NULL)
   //    showObjectMap = false;
   // #endif
   
   if (!showObjectMap)
      return;
   
   std::string objMapName = "ObjectMap";
   MessageInterface::ShowMessage("%s\n", title.c_str());
   if (objMap == NULL)
   {
      MessageInterface::ShowMessage("%s is NULL\n", objMapName.c_str());
      return;
   }
   
   GmatBase *obj = NULL;
   std::string objName;
   std::string isGlobal;
   std::string isLocal;
   GmatBase *paramOwner = NULL;
   bool isParameter = false;
   std::string paramOwnerType;
   std::string paramOwnerName;
   
   MessageInterface::ShowMessage("========================================\n");
   MessageInterface::ShowMessage
      ("Here is %s <%p>, it has %d objects\n", objMapName.c_str(), objMap, objMap->size());
   for (ObjectMap::iterator i = objMap->begin(); i != objMap->end(); ++i)
   {
      obj = i->second;
      objName = i->first;
      isGlobal = "No";
      isLocal = "No";
      paramOwner = NULL;
      isParameter = false;
      paramOwnerType = "";
      paramOwnerName = "";
      
      if (obj)
      {
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
         ("   %50s  <%p>  %-16s  IsGlobal:%-3s  IsLocal:%-3s", objName.c_str(), obj,
          obj == NULL ? "NULL" : (obj)->GetTypeName().c_str(), isGlobal.c_str(),
          isLocal.c_str());
      if (isParameter)
         MessageInterface::ShowMessage
            ("  ParameterOwner: <%p>[%s]'%s'\n", paramOwner, paramOwnerType.c_str(),
             paramOwnerName.c_str());
      else
         MessageInterface::ShowMessage("\n");
   }



   #if 0
   MessageInterface::ShowMessage(title + "\n");
   if (objMap != NULL)
   {
      MessageInterface::ShowMessage
         (" passedObjectMap = <%p>, it has %d objects\n", objMap, objMap->size());
      for (ObjectMap::iterator i = objMap->begin(); i != objMap->end(); ++i)
      {
         MessageInterface::ShowMessage
            ("   %40s  <%p> [%s]\n", i->first.c_str(), i->second,
             i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
      }
   }
   
   if (objectMapInUse == NULL)
   {
      MessageInterface::ShowMessage("\nThe objectMapInUse is NULL\n");
      return;
   }
   
   MessageInterface::ShowMessage
      (" objectMapInUse = <%p>, it has %d objects\n", objectMapInUse, objectMapInUse->size());
   for (ObjectMap::iterator i = objectMapInUse->begin();
        i != objectMapInUse->end(); ++i)
   {
      MessageInterface::ShowMessage
         ("   %40s  <%p> [%s]\n", i->first.c_str(), i->second,
          i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
   }
   #endif
}

//------------------------------------------------------------------------------
// std::string WriteObjectInfo(const std::string &title, GmatBase *obj, bool addEol)
//------------------------------------------------------------------------------
std::string Moderator::WriteObjectInfo(const std::string &title, GmatBase *obj,
                                       bool addEol)
{
   std::stringstream objStream;
   char buff[10];
   sprintf(buff, "<%p>", obj);
   objStream << title << buff;
   
   if (obj)
   {
      objStream << "[" << obj->GetTypeName() << "]" << "'" << obj->GetName() << "'";
      objStream << ", IsGlobal:" << obj->IsGlobal() << ", IsLocal():" << obj->IsLocal();
   }
   else
   {
      objStream << "[NULL]'NULL'";
   }
   
   if (addEol)
      objStream << "\n";
   
   return objStream.str();
}

//------------------------------------------------------------------------------
// Moderator()
//------------------------------------------------------------------------------
/*
 * Constructor
 */
//------------------------------------------------------------------------------
Moderator::Moderator()
{
   isRunReady = false;
   showFinalState = false;
   loadSandboxAndPause = false;
   isInitialized = false;
   thePublisher = NULL;
   theDefaultSolarSystem = NULL;
   theSolarSystemInUse = NULL;
   theInternalCoordSystem = NULL;
   theInternalSolarSystem = NULL;
   runState = Gmat::IDLE;
   detailedRunState = Gmat::IDLE;
   objectManageOption = 1;
   currentSandboxNumber = 1;
   mainScriptFileName = "";
   theMatlabInterface = NULL;
   
   // The motivation of adding this data member was due to Parameter creation
   // in function mode. When Parameter is created, the Moderator automatically
   // sets it's reference object. For example, Sat.X, it sets Sat object pointer
   // found from the current object map. Since we don't always want to use
   // configuration map to find object, this objectMapInUse was added. (loj: 2008.05.23)
   objectMapInUse = NULL;
   previousObjectMap = NULL;
   currentFunction = NULL;
   
   sandboxes.reserve(Gmat::MAX_SANDBOX);
   commands.reserve(Gmat::MAX_SANDBOX);

   pCreateWidget = NULL;
   exitCode = 0;
}


//------------------------------------------------------------------------------
// ~Moderator()
//------------------------------------------------------------------------------
Moderator::~Moderator()
{
}
