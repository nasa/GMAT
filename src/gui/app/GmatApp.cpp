//$Id$
//------------------------------------------------------------------------------
//                              GmatApp
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
// Author: Linda Jun
// Created: 2003/08/08
//
// 11/24/2003 - D. Conway, Thinking Systems, Inc.
// Changes:
//  - Added test for Unix envirnment, and set the size if the main frame in that
//    environment, so that the Linux build (and, presumably, Unix builds) would
//    appear reasonably sized.
//
/**
 * This class contains GMAT main application. Program starts here.
 */
//------------------------------------------------------------------------------

#include "gmatwxdefs.hpp"
#include "GmatApp.hpp"
#include "GmatMainFrame.hpp"
#include "GmatGlobal.hpp"
#include "ViewTextFrame.hpp"
#include "GmatAppData.hpp"
#include "Moderator.hpp"
#include "GuiPublisher.hpp"
#include <wx/datetime.h>
#include <wx/splash.h>
#include <wx/image.h>
#include <wx/config.h>
#include <wx/log.h>

#ifdef __linux
   #include <X11/Xlib.h>
#endif


#include "MessageInterface.hpp"
#include "PlotInterface.hpp"
#include "ListenerManagerInterface.hpp"
#include "GuiListenerManager.hpp"
#include "GuiMessageReceiver.hpp"
#include "GuiPlotReceiver.hpp"
#include "GuiInterpreter.hpp"
#include "FileUtil.hpp"
#include "StringUtil.hpp"          // for ToIntegerArray()
#include "DebuggerCommandFactory.hpp"


// Libpng-1.6 is more stringent about checking ICC profiles than previous versions.
// You can ignore the warning. To get rid of it, remove the iCCP chunk from the PNG image.
// For now just ignore warning (LOJ: 2014.09.23)
#define __IGNORE_PNG_WARNING__

//#define DEBUG_GMATAPP
//#define DEBUG_CMD_LINE


// In single window mode, don't have any child windows; use
// main window.

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. GmatApp and
// not wxApp)

IMPLEMENT_APP(GmatApp)
    
//------------------------------------------------------------------------------
// GmatApp()
//------------------------------------------------------------------------------
GmatApp::GmatApp()
: theMainFrame( NULL )
{
   GuiMessageReceiver *theMessageReceiver = GuiMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(theMessageReceiver);
   
   GuiPlotReceiver *thePlotReceiver = GuiPlotReceiver::Instance();
   PlotInterface::SetPlotReceiver(thePlotReceiver);

   GuiListenerManager *theListenerManager = GuiListenerManager::Instance();
   ListenerManagerInterface::SetListenerManager(theListenerManager);

   theModerator      = (Moderator *)NULL;
   showMainFrame     = true;
   scriptToRun       = "";
   buildScript       = false;
   runScript         = false;
   runBatch          = false;
   startMatlabServer = false;
   skipSplash        = false;

   #ifdef __linux
      if (XInitThreads() == 0)      // non-zero return on success
         MessageInterface::ShowMessage("Warning: XInitThreads() failed\n");
   #endif
}


//------------------------------------------------------------------------------
// OnInit()
//------------------------------------------------------------------------------
/**
 * The execution of Main program starts here.
 */
//------------------------------------------------------------------------------
bool GmatApp::OnInit()
{
   bool status = false;
   
   // Moved from TrajPlotCanvas.cpp (loj: 2009.01.08)
   wxInitAllImageHandlers();
   
   // set application name
   SetAppName("GMAT");
   
#if wxUSE_PRINTING_ARCHITECTURE
   // initialize print data and setup
   globalPrintData = new wxPrintData;
   globalPageSetupData = new wxPageSetupDialogData;
#endif // wxUSE_PRINTING_ARCHITECTURE
   
   try
   {
      // The only bin directory is set in the FileManager constructor
      GmatAppData *gmatAppData = GmatAppData::Instance();
      FileManager *fm = FileManager::Instance();
      std::string startupFile = fm->GetFullStartupFilePath();
      
      #ifdef DEBUG_GMATAPP
      MessageInterface::ShowMessage
         ("GmatApp::OnInit() Before Moderator initialization, startupFile = '%s'\n",
          startupFile.c_str());
      #endif
      
      // Continue work on this (loj: 2008.12.04)
      //@todo: Add all files contains gmat_startup_file in
      // startup up directory, and show user to select one
//      //---------------------------------------------------------
//      #ifdef __GET_STARTUP_FILE_FROM_USER__
//      //---------------------------------------------------------
//      bool readOtherStartupFile = false;
//      
//      wxArrayString choices;
//      choices.Add(startupFile);
//      choices.Add("Read other startup file");
//      wxString msg = "Please select GMAT startup file to read";
//      int result =
//         wxGetSingleChoiceIndex(msg, "GMAT Startup File", 
//                                choices, NULL, -1, -1, true, 150, 200);
//      if (result == 1)
//         readOtherStartupFile = true;
//      
//      // reading other startup file, save current directory and set it back
//      if (readOtherStartupFile)
//      {
//         wxString filename = 
//            ::wxFileSelector("Choose GMAT startup file", "",
//                             "gmat_startup_file.txt", "txt", "*.*");
//         if (filename != "")
//            startupFile = filename;
//      }
//      //---------------------------------------------------------      
//      #endif
//      //---------------------------------------------------------
      
      // Create the Moderator - GMAT executive
      theModerator = Moderator::Instance();
      
      // Use the GUI Publisher
      theModerator->OverridePublisher(GuiPublisher::Instance());

      // Check the argument list for the startup and/or log file names
      // here
      StringArray theFiles   = CheckForStartupAndLogFile();
      std::string theStartup = theFiles.at(0);
      std::string theLogfile = theFiles.at(1);
      
      if (theLogfile != "")
      {
         GmatGlobal::Instance()->SetLogfileSource(GmatGlobal::CMD_LINE, theLogfile);
      }
      
      std::string startupFileToRead = startupFile;
      if (theStartup != "")
         startupFileToRead = theStartup;
      
      // Initialize the Moderator.
      // The Moderator passes startup file to FileManger to read.
      // Save current working directory... Moderator can change it
      // but we need it when reading command line arguments
      std::string currDirBeforeInit = fm->GetCurrentWorkingDirectory();
      #ifdef DEBUG_GMATAPP
      MessageInterface::ShowMessage
         ("   curr dir before Moderator init = '%s'\n", currDirBeforeInit.c_str());
      #endif
      
//      if (theModerator->Initialize(startupFile, true))
      if (theModerator->Initialize(startupFileToRead, true))
      {
         // Register the debugger
         FactoryManager::Instance()->RegisterFactory(new DebuggerCommandFactory);

         GuiInterpreter *guiInterp = GuiInterpreter::Instance();
         theModerator->SetUiInterpreter(guiInterp);
         theModerator->SetInterpreterMapAndSS(guiInterp);
         guiInterp->BuildCreatableObjectMaps();
         
         // get GuiInterpreter
         gmatAppData->SetGuiInterpreter(
               (GuiInterpreter *)theModerator->GetUiInterpreter());
         
         // set default size
         wxSize size = wxSize(800, 600);
         wxPoint position = wxDefaultPosition;
         #ifdef __WXMAC__
         position = wxPoint(0,25); // put the main frame in the upper left corner
         #endif
         
         // for Windows
         #ifdef __WXMSW__
         // Get MainFrame position and size from the config file
         wxConfigBase *pConfig = gmatAppData->GetPersonalizationConfig();         
         wxString upperLeftStr, windowSizeStr;
         Integer windowX = -99, windowY = -99;
         Integer windowW = -99, windowH = -99;
         IntegerArray intValues;
         bool upperLeftOk = false;
         bool windowSizeOk = false;
         try
         {
            if (pConfig->Read("/MainFrame/UpperLeft", &upperLeftStr))
            {
               intValues = GmatStringUtil::ToIntegerArray(upperLeftStr.c_str());
               if (intValues.size() == 2)
               {
                  windowX = intValues[0];
                  windowY = intValues[1];
                  upperLeftOk = true;
               }
            }
            if (pConfig->Read("/MainFrame/Size", &windowSizeStr))
            {
               intValues = GmatStringUtil::ToIntegerArray(windowSizeStr.c_str());
               if (intValues.size() == 2)
               {
                  windowW = intValues[0];
                  windowH = intValues[1];
                  windowSizeOk = true;
               }
            }
            #ifdef DEBUG_CONFIG_FILE
            MessageInterface::ShowMessage
               ("   Initial screen pos,  Y = %4d, Y = %4d\n", windowX, windowY);
            MessageInterface::ShowMessage
               ("   Initial window size, W = %4d, H = %4d\n", windowW, windowH);
            #endif
         }
         catch (BaseException &be)
         {
            MessageInterface::ShowMessage("**** %s\n", be.GetFullMessage().c_str());
         }
         
         if (upperLeftOk && windowSizeOk)
         {
            // if upper left is not negative (not maximized),
            // then set new position and size
            if (windowX > 0 && windowY > 0)
            {
               position = wxPoint(windowX, windowY);
               size = wxSize(windowW, windowH);
            }
         }
         #endif
         // endif for Windows
         
         // The code above broke the Linux scaling.  This is a temporary hack to 
         // repair it.  PLEASE don't use UNIX macros to detect the Mac code!!!
         #ifdef __LINUX__
            size = wxSize(1024, 768);
         #endif
         // Mac doesn't look right, either
         #ifdef __WXMAC__
            size = wxSize(235,900);
         #endif
         
         // Set icon file from the startup file
         gmatAppData->SetIconFile();
         
         std::string currDirAfterInit = fm->GetCurrentWorkingDirectory();
         #ifdef DEBUG_GMATAPP
         MessageInterface::ShowMessage
            ("   curr dir after  Moderator init = '%s'\n", currDirAfterInit.c_str());
         #endif
         
         fm->SetCurrentWorkingDirectory(currDirBeforeInit);
         
         // Save current working directory... Moderator may have changed it in
         // ReadStartupFile, but we need old one when reading command line arguments
         
         // Process command line options
         bool pclo = ProcessCommandLineOptions();
         
         fm->SetCurrentWorkingDirectory(currDirAfterInit);
         
         // Build full path script if it has no path
         if (GmatFileUtil::HasNoPath(scriptToRun))
             scriptToRun = currDirBeforeInit + fm->GetPathSeparator() + scriptToRun;
         
         #ifdef DEBUG_GMATAPP
         MessageInterface::ShowMessage
            ("   After processing command line options...\n   Checking full path "
             "for script to load...\n   scriptToRun = <%s>\n", scriptToRun.c_str());
         #endif
         
         if (!pclo)
            return false;
         
         if (!showMainFrame)
            return false;
         
         #ifdef __IGNORE_PNG_WARNING__
         wxLogLevel logLevel = wxLog::GetLogLevel();
         wxLog::SetLogLevel(0);
         #endif
         
         if (GmatGlobal::Instance()->GetGuiMode() != GmatGlobal::MINIMIZED_GUI && skipSplash == false && GmatGlobal::Instance()->SkipSplashMode() != true)
         {            
            // Initializes all available image handlers.
            ::wxInitAllImageHandlers();
            
            // Changed to use FileManager::FindPath() (LOJ: 2014.06.26)
            //show the splash screen
            //wxString splashFile = theModerator->GetFileName("SPLASH_FILE").c_str();
            wxString splashFile = fm->FindPath("", "SPLASH_FILE", true, false, true).c_str();
            
            if (GmatFileUtil::DoesFileExist(splashFile.c_str()))
            {
               std::string ext = GmatFileUtil::ParseFileExtension(splashFile.c_str());
               std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
               wxBitmap *bitmap = NULL;
               if ((ext == "TIF") || (ext == "TIFF"))
               {
                  wxImage::AddHandler(new wxTIFFHandler);
                  bitmap = new wxBitmap(splashFile, wxBITMAP_TYPE_TIF);
               }
               else if ((ext == "JPG") || (ext == "JPEG"))
               {
                  wxImage::AddHandler(new wxJPEGHandler);
                  bitmap = new wxBitmap(splashFile, wxBITMAP_TYPE_JPEG);
               }
               else if (ext == "PNG")
               {
                  wxImage::AddHandler(new wxPNGHandler);
                  bitmap = new wxBitmap(splashFile, wxBITMAP_TYPE_PNG);
               }
               else
               {
                  MessageInterface::ShowMessage
                  ("*** WARNING *** Can't load SPLASH_FILE from '%s'\n", splashFile.WX_TO_C_STRING);
//                  ("*** WARNING *** Can't load SPLASH_FILE from '%s'\n", splashFile.c_str());
               }
               // Changed to time out in 4 sec (LOJ: 2009.10.07)
               if (bitmap != NULL)
               {
                  long splashStyle;
                  #ifdef __WXMAC__
                     splashStyle = wxSIMPLE_BORDER|wxSTAY_ON_TOP;
                  #else
                     splashStyle = wxSIMPLE_BORDER|wxSTAY_ON_TOP|wxFRAME_SHAPED;
                  #endif
                   wxRegion region(*bitmap, *wxGREEN);
                   wxSplashScreen *splash = new wxSplashScreen(*bitmap,
                                        wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
                                        4000, NULL, -1, wxDefaultPosition, wxSize(100, 100),
                                        splashStyle);
                   splash->SetShape(region);
               }
            }
            else
            {
               MessageInterface::ShowMessage
                  ("*** WARNING *** Can't load SPLASH_FILE from '%s'\n", splashFile.WX_TO_C_STRING);
            }
         }
         
         theModerator->LoadDefaultMission();

         #ifdef DEBUG_GMATAPP
         MessageInterface::ShowMessage("   Creating GmatMainFrame...\n");
         #endif
         
         theMainFrame =
            new GmatMainFrame((wxFrame *)NULL, -1,
                              _T("GMAT - General Mission Analysis Tool"),
                              position, size,
                              wxDEFAULT_FRAME_STYLE | wxHSCROLL | wxVSCROLL);
         
         #ifdef __IGNORE_PNG_WARNING__
         wxLog::SetLogLevel(logLevel);
         #endif
         
         WriteMessage("GMAT GUI successfully launched.\n", "");
         
         // Show any errors occured during initialization
         std::string savedMsg = MessageInterface::GetQueuedMessage();
         if (savedMsg != "")
            MessageInterface::ShowMessage(savedMsg);
         
         #ifdef DEBUG_GMATAPP
         MessageInterface::ShowMessage
            ("GmatApp::OnInit() size=%dx%d\n", size.GetWidth(), size.GetHeight());
         #endif
         
         // Mac user rather smaller frame and top left corner and show it.
         // (the frames, unlike simple controls, are not shown when created
         // initially)
         #ifndef __WXMAC__
         #ifdef __WXMSW__
         // if upper left is negative, then maximize
         if (windowX < 0)
         {
            theMainFrame->Maximize();
            theMainFrame->CenterOnScreen(wxBOTH);
         }
         #else
         // On Linux, open at default (1024x768) size in upper left of screen
         theMainFrame->SetPosition(wxPoint(32,32));

         // Here are the maximized settings:
         // theMainFrame->Maximize();
         // theMainFrame->CenterOnScreen(wxBOTH);
         #endif
         #endif
         
         if (startMatlabServer)
            theMainFrame->StartMatlabServer();
         
         if (GmatGlobal::Instance()->GetGuiMode() == GmatGlobal::MINIMIZED_GUI)
            theMainFrame->Show(false);
         else
         {
            theMainFrame->Show(true);
            theMainFrame->ManageMissionTree();
         }
         
         // Load or load and run script
         #ifdef DEBUG_CMD_LINE
         MessageInterface::ShowMessage
            ("  buildScript=%d, runScript=%d\n", buildScript, runScript);
         #endif
         
         if (buildScript && runScript)
         {
            BuildAndRunScript(true);
         }
         else if (buildScript)
         {
            BuildAndRunScript(false);
         }
         else if (runBatch)
         {
            RunBatch();
         }
         status = true;
      }
      else
      {
         // show error messages
         {
            wxBusyCursor bc;
            wxLogWarning(wxT("The Moderator failed to initialize."));

            // Check to see if the cursor changed as expected on all platforms; then remove this code:
            // and if ~wxBusyCursor doesn't do it, then call it manually
            //wxYield();
         }
         
         wxLogError(wxT("The error occurred during the initialization.  GMAT will exit"));
         wxLog::FlushActive();
         status = false;
      }
      
      // success: wxApp::OnRun() will be called which will enter the main message
      // loop and the application will run. If we returned FALSE here, the
      // application would exit immediately.

      if (startupMessageBuffer.length() > 0)
         MessageInterface::ShowMessage("%s\n", startupMessageBuffer.c_str());
      
      return status;
   }
   catch (BaseException &e)
   {
      WriteMessage("Following error encountered while launching GMAT GUI.\n",
                        e.GetFullMessage() + "\n\n");
      return false;
   }
   catch (...)
   {
      WriteMessage("Unknown error encountered while launching GMAT GUI.\n\n", "");
      //Re-throw the exception so OS can give a debug opportunity.
      throw;
      //return false;
   }
}


//------------------------------------------------------------------------------
// OnExit()
//------------------------------------------------------------------------------
/**
* Actions to complete on exit from GMAT
*
* Possible exit codes are the following:
*          0 if successful run
*         -1 if sandbox number is invalid
*         -2 if exception thrown during sandbox initialization
*         -3 if unknown error occurred during sandbox initialization
*         -4 if execution interrupted by user
*         -5 if exception thrown during the sandbox execution
*         -6 if unknown error occurred during sandbox execution
*         -7 if no mission sequence was defined
*         -8 if there was a script error
*/
//------------------------------------------------------------------------------
int GmatApp::OnExit()
{
   wxSafeYield();
   // Moderator destructor is private, so just call Finalize()
   theModerator->Finalize();
   
#if wxUSE_PRINTING_ARCHITECTURE
   // delete global print data and setup
   if (globalPrintData)
      delete globalPrintData;
   if (globalPageSetupData)
      delete globalPageSetupData;
#endif // wxUSE_PRINTING_ARCHITECTURE
   
   WriteMessage("GMAT GUI exiting.\n", "");
   
   Integer exitCode = theModerator->GetExitCode();
   if (exitCode != 1)
      exit(exitCode);

   return 0;
}


//------------------------------------------------------------------------------
// int FilterEvent(wxEvent& event)
//------------------------------------------------------------------------------
/**
 * Keyboard events go to the component that currently has focus and do not propagate
 * to the parent.
 *
 * This function is called early in event-processing, so we can catch key events
 * globally and do things like F3 for find next.
 */
//------------------------------------------------------------------------------
int GmatApp::FilterEvent(wxEvent& event)
{
   if (theMainFrame)
   {
      if (event.GetEventType() == wxEVT_KEY_DOWN)
      {
         // Find Next
         if (((wxKeyEvent&)event).GetKeyCode() == WXK_F3)
         {
            theMainFrame->OnFindNext( (wxCommandEvent&)event );
            return 1;
         }
         
         // Find and Replace
         if (((wxKeyEvent&)event).GetKeyCode() == 'H' &&     
             ((wxKeyEvent&)event).GetModifiers() == wxMOD_CONTROL)
         {
            theMainFrame->OnReplaceNext( (wxCommandEvent&)event );
            return 1;
         }
      }
   }
   
   return -1;
}


#ifndef INCLUDE_WX_DEBUG
//------------------------------------------------------------------------------
// void OnAssertFailure(const wxChar *file, int line, const wxChar *func, 
//         const wxChar *cond, const wxChar *msg)
//------------------------------------------------------------------------------
/**
 * Overrides the wxApp::OnAssertFailure method, hiding wxWidgets assertion 
 * failures.  
 *
 * To see the wx assertions, define the macro INCLUDE_WX_DEBUG in the 
 * preprocessor.
 *
 * @param  file The name of the source file where the assert occurred
 * @param  line The line number in this file where the assert occurred
 * @param  func The name of the function where the assert occurred, may be 
 *              empty if the compiler doesn't support C99 __FUNCTION__
 * @param  cond The condition of the failed assert in text form
 * @param  msg  The message specified as argument to wxASSERT_MSG or 
 *              wxFAIL_MSG, will be NULL if just wxASSERT or wxFAIL was used
 */
//------------------------------------------------------------------------------
void GmatApp::OnAssertFailure(const wxChar *file, int line, const wxChar *func, 
         const wxChar *cond, const wxChar *msg)
{
}
#endif

//------------------------------------------------------------------------------
// StringArray CheckForStartupAndLogFile()
//------------------------------------------------------------------------------
/**
 * Check the input arguments specifically and only for the startup file
 * and log file.
 *
 * NOTE; this method will access the argc and argv inputs
 *
 * @return array of startup and log file names
 */
//------------------------------------------------------------------------------
StringArray GmatApp::CheckForStartupAndLogFile()
{
   StringArray result;
   std::string theStartup = ""; // "gmat_startup_file.txt";
   std::string theLog     = ""; // "GmatLog.txt";
   
   // if there is only one arguments, that is only the application name
   // NOTE: on Mac, we expect --args before any other arguments.
   if (argc > 1)
   {
      for (int i = 1; i < argc; ++i)
      {
         #if wxCHECK_VERSION(3, 0, 0)
            std::string arg = argv[i].WX_TO_STD_STRING;
            std::string argNext = "";
            if (argc >= i + 2)
               argNext = argv[i+1].WX_TO_STD_STRING;
         #else
            std::string arg = argv[i];
            std::string argNext = "";
            if (argc >= i + 2)
               argNext = argv[i+1];
         #endif
         if ((arg == "--logfile") || (arg == "-l"))
         {
            if (argc < i + 2)
            {
//               MessageInterface::PutMessage("*** Missing log file name\n");
            }
            else
            {
               std::string lFile = argNext;
               if (lFile[0] != '-')
               {
                  // Replace single quotes
                  GmatStringUtil::Replace(lFile, "'", "");
                  theLog = lFile;
                  ++i;
               }
               else
               {
//                  MessageInterface::PutMessage("*** Missing log file name\n");
               }
            }
         }
         else if ((arg == "--startup_file") || (arg == "-s"))
         {
            if (argc < i + 2)
            {
//               MessageInterface::PutMessage("*** Missing startup file name\n");
            }
            else
            {
               std::string sFile = argNext;
               if (sFile[0] != '-')
               {
                  // Replace single quotes
                  GmatStringUtil::Replace(sFile, "'", "");
                  theStartup = sFile;
                  ++i;
                  if (!GmatFileUtil::DoesFileExist(theStartup))
                  {
                     std::string popMess = "ERROR: Startup file specified not found: \"" + theStartup + "\" does not exist.\n ";
                     MessageInterface::ShowMessage(popMess);
                  }
               }
               else
               {
//                  MessageInterface::PutMessage("*** Missing startup file name\n");
               }
            }
         }
         else
         {
            ; // ignore anything else for now
         }
      }
   }
   
   result.push_back(theStartup);
   result.push_back(theLog);
   
   return result;
}

//------------------------------------------------------------------------------
// bool ProcessCommandLineOptions()
//------------------------------------------------------------------------------
/// @note Should this be moved to the Moderator so the options are available to
/// all user interfaces?
bool GmatApp::ProcessCommandLineOptions()
{
   #ifdef DEBUG_CMD_LINE
   MessageInterface::PutMessage("\nGmatApp::ProcessCommandLineOptions() entered\n");
   #endif
   
   bool retval = true;

   std::string gmatUsage;
   std::string gmatText =
         "If no arguments are specified, GMAT opens with the full GUI and with the default\n"
         "mission loaded.  If a GMAT script name is provided for 'scriptname', GMAT will\n"
         "open with the specified script loaded.  GMAT is started whether or not any of the\n"
         "listed options are used.\n";
   #ifdef __WXMAC__
      gmatUsage =
            "Usage: open GMAT.app [--args [options] [scriptname]]\n\n";
      gmatText +=
            "The \"--args\" must appear before any specified options or a scriptname.\n";
   #else
      gmatUsage =
            "Usage: gmat [options] [scriptname]\n\n";
   #endif
   std::string gmatHelp = gmatUsage + gmatText + "\n";
   gmatHelp +=
         "-h, \t--help                   \t\t\tDisplay command line usage information in the Message Window\n"
         "-l, \t--logfile <filename>     \t\tSpecify log file name\n"
         "-s, \t--startup_file <filename>\tSpecify the startup file to read\n"
         "-m, \t--minimize               \t\tOpen GMAT with a minimized interface\n"
         "-r, \t--run <filename>         \t\tAutomatically run the specified script after loading\n"
         "    \t                         \t\t\t[has no effect if no script is specified]\n"
         "-v, \t--version                \t\tDisplay version information in the Message Window\n"
         "-x, \t--exit                   \t\t\tExit GMAT after running the specified script\n"
         "    \t                         \t\t\t[if specified with only a script name (i.e. NO --run option), GMAT simply opens and closes]\n"
         "-ns, \t--no_splash             \t\t\tSkip showing the GMAT splash screen upon startup\n";
   
   
   //wxString commandLineOptions = wxT(gmatHelp.c_str());
   wxString commandLineOptions = wxString(gmatHelp.c_str());
   
   #ifdef DEBUG_CMD_LINE
   MessageInterface::PutMessage("argc = %d\n", argc);
   #endif
   
   // Handle any command line arguments
   if (argc > 1)
   {
      bool skipNext = false;
      for (int i = 1; i < argc; ++i)
      {
         #if wxCHECK_VERSION(3, 0, 0)
         std::string arg = argv[i].WX_TO_STD_STRING;
         std::string argNext = "";
         if (argc >= i + 2)
            argNext = argv[i+1].WX_TO_STD_STRING;
         #else
         std::string arg = argv[i];
         std::string argNext = "";
         if (argc >= i + 2)
            argNext = argv[i+1];
         #endif
         // If we are skipping the next one (for startup_file or logfile, e.g.),
         // continue to the next argument
         if (skipNext)
         {
            skipNext = false;
            continue;
         }
         
         #ifdef DEBUG_CMD_LINE
         MessageInterface::PutMessage("arg = %s\n", arg.c_str());
         #endif
         if ((arg == "--version") || (arg == "-v"))
         {
            wxString buildDate;

            // Fix bug GMT-7399
            // buildDate.Printf("Build Date: %s %s\n\n", __DATE__, __TIME__);
            buildDate.Printf("Build Date: %s %s\n\n",
               GmatGlobal::Instance()->GetGMATBuildDate(),
               GmatGlobal::Instance()->GetGMATBuildTime());

            startupMessageBuffer += buildDate;
            #ifdef DEBUG_CMD_LINE
            MessageInterface::PutMessage(buildDate.c_str());
            #endif
         }
         #ifdef __ADD_GMAT_SERVER__
         else if (arg == "--start-server")
         {
            startMatlabServer = true;
         }
         #endif
         else if ((arg == "--run") || (arg == "-r"))
         {
            if (argc < 3)
            {
               MessageInterface::ShowMessage
                  ("Please enter script file name to run\n");
            }
            else
            {
               buildScript = true;
               runScript = true;
            }
         }
         else if ((arg == "--help") || (arg == "-h"))
         {
            startupMessageBuffer += commandLineOptions;
            //MessageInterface::ShowMessage(commandLineOptions.c_str());
         }
         else if ((arg == "--exit") || (arg == "-x"))
         {
            GmatGlobal::Instance()->SetRunMode(GmatGlobal::EXIT_AFTER_RUN);
         }
         else if ((arg == "--logfile") || (arg == "-l"))
         {
            bool missingLogfile = true;
            if (argc >= i + 2) // there could be a log file name next
            {
               std::string nextArg = argNext;
               if (nextArg[0] != '-')
               {
                  skipNext = true; // skip log file name - handled previously
                  missingLogfile = false;
               }
            }
            if (missingLogfile)
            {
               MessageInterface::ShowMessage(
                    "*** Missing log file name: ignoring --logfile argument\n");
            }
         }
         else if ((arg == "--startup_file") || (arg == "-s"))
         {
            bool missingStartup = true;
            if (argc >= i + 2) // there could be a startup file name next
            {
               std::string nextArg = argNext;
               if (nextArg[0] != '-')
               {
                  skipNext = true; // skip startup file name - handled previously
                  missingStartup = false;
               }
            }
            if (missingStartup)
            {
               MessageInterface::ShowMessage(
                  "*** Missing startup file name: ignoring --startup_file argument\n");
            }
            if (!GmatFileUtil::DoesFileExist(argNext))
            {
               std::string popMess = "ERROR: Startup file specified not found: \"" + argNext + "\" does not exist.\n ";
               MessageInterface::PopupMessage(Gmat::ERROR_, popMess);
               throw GmatBaseException(popMess);
            }
         }
         else if ((arg == "--minimize") || (arg == "-m"))
         {
            GmatGlobal::Instance()->SetGuiMode(GmatGlobal::MINIMIZED_GUI);
         }
         else if ((arg == "--nits") || (arg == "-n"))
         {
            // NITS Support
            MessageInterface::ShowMessage("GMAT is running as a NITS client\n");
            // Check for NITS plugin by checking for a NITS connection command
            StringArray commands = 
               theModerator->GetListOfFactoryItems(Gmat::COMMAND);
            if (find(commands.begin(), commands.end(), "SendMessage") == commands.end())
            {
               wxBusyCursor bc;
               wxLogError("GMAT was started as a NITS client, but the "
                  "NITS plugin was not loaded.\n"
                  "The error occurred during the initialization.  GMAT will exit.");
               wxLog::FlushActive();
               retval = false;
            }
            else
            {
               scriptToRun = "NITS_Config.script";
               // Replace single quotes
               scriptToRun = GmatStringUtil::Replace(scriptToRun, "'", "");
               runScript = true;
               GmatGlobal::Instance()->SetNitsClient(true);
               GmatGlobal::Instance()->SetRunMode(GmatGlobal::EXIT_AFTER_RUN);
               
               // When this changes from throw-away prototype mode to NITS compatible
               // mode, we'll want to do a NITS handshake here to register as a NITS
               // client.  That mode is not yet ready for use, though.  Here's a start on 
               // how to do that, via a static method in the command that does not 
               // (and cannot) yet exist:
               //SendMessage::ShakeHands();
               
               ++i;
               #ifdef DEBUG_CMD_LINE
                  MessageInterface::PutMessage("%s\n", scriptToRun.c_str());
               #endif
            }
         }
         else if (arg == "--save")
         {
            ; // ignore this argument for the GUI GMAT
         }
         else if (arg == "--no_splash" || arg == "-ns")
            skipSplash = true; //Skips showing the splash screen on startup
         else
         {
            #ifdef DEBUG_CMD_LINE
            MessageInterface::PutMessage("Checking if arg is script name, arg=<%s>\n", arg.c_str());
            #endif
            
            bool isArgValid = false;
            // Remove single quotes before checking
            std::string tempfile = GmatStringUtil::Replace(arg, "'", "");
            std::string currpath = GmatFileUtil::GetCurrentWorkingDirectory();
            std::string pathsep  = GmatFileUtil::GetPathSeparator();
            
            #ifdef DEBUG_CMD_LINE
            MessageInterface::PutMessage("tempfile = <%s>\n", tempfile.c_str());
            MessageInterface::PutMessage("currpath = <%s>\n", currpath.c_str());
            MessageInterface::PutMessage("pathsep  = <%s>\n", pathsep.c_str());
            #endif
            
            // Check for file type association with GMAT on Windows
            // since argv[0] contains the file name to run
            // 2013.03.11: Changed to check for the script file so that script
            // name can appear anywhere in the argument list, for example --run can
            // appear after script name such as, 'BplaneTarget.script' -- run
            // So removed #ifdef __WIN32__
            //#ifdef __WIN32__
            if (GmatFileUtil::DoesFileExist(tempfile))
            {
               #ifdef DEBUG_CMD_LINE
               MessageInterface::PutMessage
                  ("The script file <%s> found, so build full path if needed\n", tempfile.c_str());
               #endif
               
               // Set this as script to run
               // Since it looks in the current directory, build full path script name if needed
               if (GmatFileUtil::IsPathAbsolute(tempfile))
               {
                  #ifdef DEBUG_CMD_LINE
                  MessageInterface::PutMessage("   It has absolute path\n");
                  #endif
                  scriptToRun = tempfile;
               }
               else
               {
                  #ifdef DEBUG_CMD_LINE
                  MessageInterface::PutMessage("   It has no absolute path\n");
                  #endif
                  scriptToRun = currpath + pathsep + tempfile;
               }
               
               buildScript = true;
               isArgValid = true;
               #ifdef DEBUG_CMD_LINE
               MessageInterface::PutMessage("scriptToRun = <%s>\n", scriptToRun.c_str());
               #endif
            }
            else
            {
               #ifdef DEBUG_CMD_LINE
               MessageInterface::PutMessage
                  ("The script file '%s' not found, so checking current directory '%s'\n",
                   tempfile.c_str(), currpath.c_str());
               #endif
               
               // Set this as script to run
               // Since it looks in the current directory, build full path script name if needed
               if (!GmatFileUtil::IsPathAbsolute(tempfile))
               {
                  #ifdef DEBUG_CMD_LINE
                  MessageInterface::PutMessage("   It has no absolute path, so build full path...\n");
                  #endif
                  // Set to script name so that it can be displayed in the error message
                  scriptToRun = currpath + pathsep + tempfile;
               }
               
               #ifdef DEBUG_CMD_LINE
               MessageInterface::PutMessage("new file name = <%s>\n", scriptToRun.c_str());
               #endif
               if (GmatFileUtil::DoesFileExist(scriptToRun))
               {
                  // set current directory to new path
                  GmatFileUtil::SetCurrentWorkingDirectory(scriptToRun);
                  buildScript = true;
                  isArgValid = true;
               }
               else
               {
                  MessageInterface::ShowMessage
                     ("*** Cannot find the script file '%s'\n", scriptToRun.c_str());
               }
            }
            //#endif
            //@todo implement this for mac and linux?
            
            if (!isArgValid)
            {
               MessageInterface::PutMessage("The option \"%s\" is not valid.\n", arg.c_str());
               MessageInterface::PutMessage(commandLineOptions.c_str());
               break;
            }
         }
      }
   }
   
   #ifdef DEBUG_CMD_LINE
   MessageInterface::PutMessage
      ("GmatApp::ProcessCommandLineOptions() returning %d, buildScript=%d runScript=%d\n"
       "   scriptToRun = '%s'\n\n", retval, buildScript, runScript, scriptToRun.c_str());
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// void BuildAndRunScript(bool runScript)
//------------------------------------------------------------------------------
void GmatApp::BuildAndRunScript(bool runScript)
{
   #ifdef DEBUG_CMD_LINE
   MessageInterface::ShowMessage
      ("GmatApp::BuildAndRunScript() entered, runScript=%d, scriptToRun='%s'\n",
       runScript, scriptToRun.c_str());
   #endif
   
   if (GmatGlobal::Instance()->GetGuiMode() == GmatGlobal::MINIMIZED_GUI)
      theMainFrame->Iconize(true);
   
   // Build script
   bool builtOk = false;
   try
   {
      #ifdef DEBUG_CMD_LINE
      MessageInterface::ShowMessage("   Building script ...\n");
      #endif
      #ifndef __WXMAC__
      wxSafeYield();
      #endif
      builtOk = theMainFrame->BuildScript(scriptToRun.c_str(), true);
   }
   catch (BaseException &e)
   {
      WriteMessage("Following error encountered while building the script\n   ",
                   scriptToRun + "\n   ", e.GetFullMessage());
   }
   catch (...)
   {
      WriteMessage("Unknown error encountered while building the script\n   ",
                   scriptToRun + "\n");
   }
   
   // Run script
   Integer runStatus = 0;
   if (builtOk && runScript)
   {
      try
      {
         #ifdef DEBUG_CMD_LINE
         MessageInterface::ShowMessage("   Running script ...\n");
         #endif
         #ifndef __WXMAC__
         wxSafeYield();
         #endif
         runStatus = theMainFrame->RunCurrentScript();
         if (runStatus != 1)
         {
            WriteMessage("Failed to run the script\n   ", scriptToRun + "\n");
            MessageInterface::LogMessage("   Run status is %d\n", runStatus);
         }
      }
      catch (BaseException &e)
      {
         WriteMessage("Following error encountered while running the script\n   ",
                      scriptToRun + "\n   ", e.GetFullMessage());
      }
      catch (...)
      {
         WriteMessage("Unknown error encountered while running the script\n   ",
                      scriptToRun + "\n");
      }
   }
   else
   {
      if (!builtOk)
         WriteMessage("Failed to build the script\n   ", scriptToRun + "\n");
   }
   
   #ifndef __WXMAC__
   wxSafeYield();
   #endif
   // Close GMAT on option
   if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::EXIT_AFTER_RUN)
   {
      #ifdef __LINUX__
         // Linux needs this to run in the test system successfully
         exit(0);
      #endif

      //Set auto exit mode to GmatMainFrame
      theMainFrame->SetAutoExitAfterRun(true);
      theMainFrame->Close();
      #ifdef __LINUX__
      // Linux needs this to complete shutdown
      MessageInterface::ShowMessage("\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// void RunBatch()
//------------------------------------------------------------------------------
void GmatApp::RunBatch()
{
}


//------------------------------------------------------------------------------
// void WriteMessage(const std::string &msg1,const std::string &msg2, ... )
//------------------------------------------------------------------------------
void GmatApp::WriteMessage(const std::string &msg1, const std::string &msg2,
                           const std::string &msg3)
{
   wxDateTime now = wxDateTime::Now();
   wxString wxNowStr = now.FormatISODate() + " " + now.FormatISOTime() + " ";
//   std::string nowStr = wxNowStr.c_str();
   std::string nowStr = wxNowStr.WX_TO_STD_STRING;
   
   MessageInterface::LogMessage(nowStr + msg1 + msg2 + msg3);
}

