//$Id$
//------------------------------------------------------------------------------
//                           GmatConsole driver
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
// Author: Darrel J. Conway
// Created: 2003/08/28
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Program entry point for GmatConsole.
 */
//------------------------------------------------------------------------------


#include "driver.hpp" 

#include <fstream>

#include "BaseException.hpp"
#include "ConsoleAppException.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "Moderator.hpp"
#include "StringUtil.hpp"
#include "GmatGlobal.hpp"

#include "CommandFactory.hpp"
#include "PointMassForce.hpp"
#include "PrintUtility.hpp"
#include "FileUtil.hpp"

//#define DEBUG_CONSOLE
//#define DEBUG_CONSOLE_STARTUP

#ifdef DEBUG_CONSOLE
#include <unistd.h>
#endif

static Moderator   *mod          = NULL;
static GmatGlobal  *gmatGlobal   = NULL;
static std::string lastRunScript = "";

//------------------------------------------------------------------------------
//  void ShowHelp()
//------------------------------------------------------------------------------
/**
 * Lists the commands available for the application.
 */
//------------------------------------------------------------------------------
void ShowHelp()
{
   std::cout << "Usage: One of the following\n"  
             << "   GmatConsole\n"
             << "   GmatConsole ScriptFileName\n"
             << "   GmatConsole <option> <string>\n\n"
             << "The first selection runs an interactive session.\n"
             << "The second runs the input script once and then exits.\n"
             << "The third selection executes specific testing scenarios.\n\n" 
             << "Valid options are:\n"
             << "   --help, -h                    Shows available options\n"
             << "   --version, -v                 Show version and build information\n"
             << "   --batch, -b <filename>        Runs multiple scripts listed in specified file\n"
             << "   --run, -r <filename>          Runs the input script once, then exits\n"
             << "   --logfile, -l <filename>      Specify the log file (ignored in Console interactive mode)\n"
             << "   --startup_file, -s <filename> Specify the startup file (ignored in Console interactive mode)\n"
             << "   --minimize, -m                Opens with GUI minimized (ignored for Console)\n"
             << "   --start-server                Starts GMAT Server on start-up (ignored for Console)\n"
             << "   --save                        Saves current script (interactive mode only)\n"
             << "   --summary                     Writes command summary (interactive mode only)\n"
             << "   --verbose <on/off>            Dump info messages to screen during run (default is on)\n"
             << "   --exit, -x                    Exit after run (default)\n"
             << std::endl << std::endl;
}


//------------------------------------------------------------------------------
// void RunScriptInterpreter(std::string script, int verbosity, bool batchmode)
//------------------------------------------------------------------------------
/**
 * Executes a script.
 * 
 * @param <script> The script file that is run.
 * @param <verbosity> Toggles the display of the command list (from the script)
 *                    on or off.  Likely to become more robust over time.
 * @param <batchmode> Flag indicating if the script is part of a batch or a
 *                    single script.
 */
//------------------------------------------------------------------------------
void RunScriptInterpreter(std::string script, int verbosity, bool batchmode)
{
//   static bool moderatorInitialized = false;
   
   std::ifstream fin(script.c_str());
   if (!(fin))
   {
      std::string errstr = "Script file ";
      errstr += script;
      errstr += " does not exist";
      if (!batchmode)
      {
         std::cout << errstr << std::endl;
         return;
      }
      else
         throw  ConsoleAppException(errstr);
   }
   else
      fin.close();

   // moved to main - wcs - Oct 2011
//   ConsoleMessageReceiver *theMessageReceiver =
//      ConsoleMessageReceiver::Instance();
//   MessageInterface::SetMessageReceiver(theMessageReceiver);
//
//   Moderator *mod = Moderator::Instance();
//
//   if (!moderatorInitialized)
//   {
//      if (!mod->Initialize())
//      {
//         throw ConsoleAppException("Moderator failed to initialize!");
//      }
//
//      moderatorInitialized = true;
//   }
    
   bool canRun;
   try
   {
      canRun = mod->InterpretScript(script);
      if (!canRun)
      {
         if (!batchmode)
         {
            std::cout << "\n***Could not read script.***\n\n";
            ShowHelp();
            throw ConsoleAppException("Errors were found in the script named \"" + script + "\"\n");
         }
         else
            throw ConsoleAppException("Script file did not parse");
         return;
      }
   }
   catch (BaseException &oops)
   {
      // std::cout << "ERROR!!!!!! ---- " << oops.GetFullMessage();
      MessageInterface::ShowMessage("ERROR!!!!!! ---- %s\n", oops.GetFullMessage().c_str());
      throw;
   }
   // print out the sequence
   GmatCommand *top = mod->GetFirstCommand();
   if (verbosity != 0)
   {
      PrintUtility* pu = PrintUtility::Instance();
      pu->PrintEntireSequence(top);
   }

   if (canRun)
   {   
      // And now run it
      if (mod->RunMission() != 1)
         throw ConsoleAppException("Moderator::RunMission failed");
   
      // Success!
      if (!batchmode)
         std::cout << "\n\n*** GMAT Integration test "
                   << "(Console version) successful! ***"
                   << "\n\n";
   }
}


//------------------------------------------------------------------------------
// Integer RunBatch(std::string& batchfilename)
//------------------------------------------------------------------------------
/**
 * Executes a collection of scripts.
 * 
 * @param <batchfilename> The file containing the list of script files to run.
 * 
 * @return The number of lines parsed from the batch file.
 */
//------------------------------------------------------------------------------
Integer RunBatch(std::string& batchfilename)
{
   Integer count = 0, successful = 0, failed = 0, skipped = 0;
   std::string script;
   StringArray failedScripts;
   StringArray skippedScripts;
      
   std::cout << "Running batch file \"" << batchfilename << "\"" << std::endl;
   std::ifstream batchfile(batchfilename.c_str());
   
   if (!(batchfile))
   {
      std::string errstr = "Batch file ";
      errstr += batchfilename;
      errstr += " does not exist";
      std::cout << errstr << std::endl;
      return 0;
   }
   
   batchfile >> script;

   while (!batchfile.eof())
   {
      if (script == "--summary")
      {
         ShowCommandSummary();
      }
      else
      {
         ++count;
         if (script[0] != '%') {
            std::cout << "\n*************************************************\n*** " 
                      << count << ": \"" << script 
                      << "\"\n*************************************************\n" 
                      << std::endl;
            try {
               RunScriptInterpreter(script, 0, true);
               ++successful;
            }
            catch (BaseException &ex) {
               std::cout << "\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         << "!!!\n"
                         << "!!! Exception in script \"" << script << "\"\n"
                         << "!!!    \""
                         << ex.GetFullMessage() << "\"\n" 
                         << "!!!\n"
                         << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         << std::endl;
                         
               ++failed;
               failedScripts.push_back(script);
            }
            catch (...)
            {
               std::cout << "\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         << "!!!\n"
                         << "!!! Unhandled Exception in script \"" << script << "\"\n"
                         << "!!!\n"
                         << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                         << std::endl;
                         
               ++failed;
               failedScripts.push_back(script);
            }
         }
         else {
            std::cout << "\n*************************************************\n*** " 
                      << count << ": Skipping script \"" << script.substr(1)
                      << "\"\n*************************************************\n" 
                      << std::endl;
            skippedScripts.push_back(script.substr(1));
            ++skipped;
         }
      }
      batchfile >> script;
   }
   
   std::cout << "\n\n**************************************\n*** "
             << "Batch Run Statistics:"
             <<               "\n***   Successful scripts:  " 
             << successful << "\n***   Failed Scripts:      "
             << failed     << "\n***   Skipped Scripts:     "
             << skipped    << "\n**************************************\n";
             
   if (failed > 0) {
      std::cout << "\n**************************************\n" 
                << "***   Scripts that failed:\n";
      for (StringArray::iterator i = failedScripts.begin(); 
           i != failedScripts.end(); ++i)
         std::cout << "***      " << *i << "\n";
      std::cout << "**************************************\n";
   }
   if (skipped > 0) {
      std::cout << "\n**************************************\n"<< "***   Scripts that were skipped:\n";
      for (StringArray::iterator i = skippedScripts.begin(); 
           i != skippedScripts.end(); ++i)
         std::cout << "***      " << *i << "\n";
      std::cout << "**************************************\n\n";
   }

   return count;
}


//------------------------------------------------------------------------------
// void SaveScript(std::string filename)
//------------------------------------------------------------------------------
/**
 * Saves the current script to a file
 * 
 * @param <filename> The name of the script file.
 */
//------------------------------------------------------------------------------
void SaveScript(std::string filename)
{
//    Moderator *mod = Moderator::Instance();
   if ((mod == NULL) || (filename == ""))
   {
      std::cout << "\nUnable to save script - no script has been run.\n\n";
      return;
   }
   std::cout << "Now saving script " << filename << "." << std::endl;
   mod->SaveScript(filename);
   std::cout << "\n\n";
}

//------------------------------------------------------------------------------
// void ShowVersionInfo()
//------------------------------------------------------------------------------
/**
 * Displays the version information, either on screen or writing to a file
 */
void ShowVersionInfo()
{
   std::string msg = "\n\nBuild Date: ";

   // Fix bug GMT-7399
   // msg += __DATE__;
   msg += GmatGlobal::Instance()->GetGMATBuildDate();

   msg += "  ";
   
   // Fix bug GMT-7399
   // msg += __TIME__;
   msg += GmatGlobal::Instance()->GetGMATBuildTime();

   std::cout << msg << std::endl << std::endl;
}


//------------------------------------------------------------------------------
// void ShowCommandSummary(std::string filename)
//------------------------------------------------------------------------------
/**
 * Displays the command summary, on screen
 * 
 * @param <filename> The name of the summary file.
 */
//------------------------------------------------------------------------------
void ShowCommandSummary(std::string filename)
{
//   Moderator *mod = Moderator::Instance();
   if (mod == NULL)
   {
      std::cout << "\nUnable to show command summary - no script has been run.\n\n";
      return;
   }
   GmatCommand *cmd = mod->GetFirstCommand();
   if (cmd == NULL)
   {
      std::cout << "Unable to show command summary - command stream is empty.\n\n";
      return;
   }
   if (cmd->GetTypeName() == "NoOp")
   {
      cmd = cmd->GetNext();
      if (!cmd)
      {
         std::cout << "Unable to show command summary - no script has been run.\n\n";
         return;
      }
   }
   std::cout << "command type is: " << cmd->GetTypeName() << std::endl;
   
   if (filename == "")
   {
      std::cout << "\n\n";
      std::string summary = cmd->GetStringParameter("MissionSummary");
      std::cout << summary << "\n\n";
   }
   else
   {
      std::cout << "File output for command summaries is not yet available\n\n";
   }
}


//------------------------------------------------------------------------------
// void DumpDEData(double secsToStep)
//------------------------------------------------------------------------------
/**
 * Writes out the Earth and Moon position and velocity data for a set span to
 * the file EarthMoonDe.txt
 *
 * @param secsToStep The timestep to use
 * @param spanInSecs The time span in seconds
 */
//------------------------------------------------------------------------------
void DumpDEData(double secsToStep, double spanInSecs)
{
   double baseEpoch = 21545.0, currentEpoch = 21545.0;
   long step = 0;
   std::ofstream data("EarthMoonDe.txt");

//   Moderator *mod = Moderator::Instance();
//   if (!mod->Initialize()) {
//      throw ConsoleAppException("Moderator failed to initialize!");
//   }

   SolarSystem *sol = mod->GetSolarSystemInUse();
   if (sol == NULL)
      MessageInterface::ShowMessage("Oh no, the solar system is NULL!");

   CelestialBody *earth = sol->GetBody("Earth");
   CelestialBody *moon  = sol->GetBody("Luna");

   data << "Earth and Moon Position and Velocity from the DE file\n\n";

   data.precision(17);
   double targetEpoch = currentEpoch + spanInSecs / 86400.0;
   while (currentEpoch <= targetEpoch)
   {
      currentEpoch = baseEpoch + step * secsToStep / 86400.0;

      Rvector6 earthRV = earth->GetMJ2000State(currentEpoch);
      Rvector6 moonRV  = moon->GetMJ2000State(currentEpoch);
      Rvector3 moonAcc = moon->GetMJ2000Acceleration(currentEpoch);

      data << currentEpoch << " " << (step * secsToStep) << " "
//           << earthRV[0] << " " << earthRV[1] << " " << earthRV[2] << " "
//           << earthRV[3] << " " << earthRV[4] << " " << earthRV[5] << " | "
           << moonRV[0] << " " << moonRV[1] << " " << moonRV[2] << " "
           << moonRV[3] << " " << moonRV[4] << " " << moonRV[5]
           << moonAcc[0] << " " << moonAcc[1] << " " << moonAcc[2]
           << "\n";

      ++step;
   }
   data << std::endl;
}


//------------------------------------------------------------------------------
// StringArray CheckForStartupAndLogFile(int argc, char *argv[])
//------------------------------------------------------------------------------
/**
 * Check the input arguments specifically and only for the startup file
 * and log file.
 *
 * @param <argc> The count of the input arguments.
 * @param <argv> The input arguments.
 *
 * @return array of startup and log file names
 */
//------------------------------------------------------------------------------
StringArray CheckForStartupAndLogFile(int argc, char *argv[])
{
   StringArray result;
   std::string theStartup = "gmat_startup_file.txt";
   std::string theLog     = ""; 
   
   // if we're in interactive mode, these options are ignored
   // if there are only two arguments, the second should be a script
   if (argc > 2)
   {
      std::string arg("");
      for (int i = 1; i < argc; ++i)
      {
         arg = argv[i];
         if ((arg == "--logfile") || (arg == "-l"))
         {
            if (argc < i + 2)
            {
               MessageInterface::ShowMessage("*** Missing log file name\n");
            }
            else
            {
               std::string lFile = argv[i+1];
               if (lFile[0] != '-')
               {
                  // Replace single quotes
                  GmatStringUtil::Replace(lFile, "'", "");
                  theLog = lFile;
                  ++i;
               }
               else
               {
                  MessageInterface::ShowMessage("*** Missing log file name\n");
               }
            }
         }
         else if ((arg == "--startup_file") || (arg == "-s"))
         {
            if (argc < i + 2)
            {
               MessageInterface::ShowMessage("*** Missing startup file name\n");
            }
            else
            {
               std::string sFile = argv[i+1];
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
                     throw ConsoleAppException(popMess);
                  }
               }
               else
               {
                  MessageInterface::ShowMessage("*** Missing startup file name\n");
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
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
/**
 * The program entry point.
 *
 * @param <argc> The count of the input arguments.
 * @param <argv> The input arguments.
 *
 * @return EXIT_SUCCESS (platform-specific value) on success.
 */
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      std::string msg = "General Mission Analysis Tool\nConsole Based Version\n";

      msg += "Build Date: ";

      // Fix bug GMT-7399
      // msg += __DATE__;
      msg += GmatGlobal::Instance()->GetGMATBuildDate();
      
      msg += "  ";

      // Fix bug GMT-7399
      // msg += __TIME__;
      msg += GmatGlobal::Instance()->GetGMATBuildTime();
      
      std::cout << "\n********************************************\n"
                << "***  GMAT Console Application\n"
                << "********************************************\n\n"
                << msg << "\n\n"
                << std::endl;
      
      char scriptfile[1024];
      bool runcomplete = false;
      int verbosity = 1;
      std::string optionParm = "";
      StringArray parms;
      std::string currentArg("");
      bool        batchRun = false;
      bool        settingVerbose = false;
      
      
      // Set the message receiver and moderator pointers here
      ConsoleMessageReceiver *theMessageReceiver = ConsoleMessageReceiver::Instance();
      MessageInterface::SetMessageReceiver(theMessageReceiver);

      #ifdef DEBUG_CONSOLE
         char   *path = NULL;
         size_t itsSize = 256;
         path = getcwd(path, itsSize);
         MessageInterface::ShowMessage(" --------> Running from directory %s\n", path);
         MessageInterface::ShowMessage(" --------> and the application path is %s\n", argv[0]);
      #endif

      StringArray filesSpecified = CheckForStartupAndLogFile(argc, argv);
      std::string startUpFile    = filesSpecified.at(0);
      std::string logFile        = filesSpecified.at(1);
      
      if (logFile != "")
      {
         gmatGlobal = GmatGlobal::Instance();
         gmatGlobal->SetLogfileSource(GmatGlobal::CMD_LINE, logFile);
      }
      
      // Can't set the log file yet because we don't have the OUTPUT_PATH until
      // we've read the startup file
      
      #ifdef DEBUG_CONSOLE_STARTUP
         MessageInterface::ShowMessage(" --------> startup set in args to %s\n", startUpFile.c_str());
      #endif
      mod = Moderator::Instance();

      if (mod == NULL || !(mod->Initialize(startUpFile)))
      {
         std::cout << "Moderator failed to initialize!  Unable to run GmatConsole." << std::endl;
         exit(EXIT_FAILURE);
//         return 1;
      }

      // Make sure the parameter database is populated
      mod->CreateDefaultParameters();

      if (argc < 2)    // interactive mode
      {
         bool ignoreNext = false;
         do
         {
            std::cout << "Enter a script file, "
                      << "q to quit, or an option:  ";

            std::cin >> scriptfile;
            // Drop the return character -- may be platform dependent
            std::cin.ignore(1);

            parms.clear();
            std::string chunk;

            if ((!strcmp(scriptfile, "q"))      || (!strcmp(scriptfile, "Q")) ||
                (!strcmp(scriptfile, "--exit")) || (!strcmp(scriptfile, "-x")))
            {
               runcomplete = true;
            }
            else if (scriptfile[0] == '-')
            {
               if ((!strcmp(scriptfile, "--help")) || (!strcmp(scriptfile, "-h")))
               {
                  ShowHelp();
               }
               else if (!strcmp(scriptfile, "--args"))
               {
                  ; // ignore this - it's used by the open statement when opening the GUI version
               }
               else if ((!strcmp(scriptfile, "--run")) || (!strcmp(scriptfile, "-r")))
               {
                  batchRun = false;  // will grab script file name as next arg
               }
               else if ((!strcmp(scriptfile, "--batch")) || (!strcmp(scriptfile, "-b")))
               {
                  batchRun = true; // will grab batch file name as next arg
               }
               else if ((!strcmp(scriptfile, "--logfile")) || (!strcmp(scriptfile, "-l")))
               {
                  std::cout << "\n--logfile option ignored in interactive mode\n ";
                  ignoreNext = true; // skip log file name
               }
               else if ((!strcmp(scriptfile, "--startup_file")) || (!strcmp(scriptfile, "-s")))
               {
                  std::cout << "\n--startup_file option ignored in interactive mode\n ";
                  ignoreNext = true; // skip startup file name
               }
               else if ((!strcmp(scriptfile, "--minimize")) || (!strcmp(scriptfile, "-m")))
               {
                  std::cout << "\n--minimize option ignored\n ";
               }
               else if (!strcmp(scriptfile, "--save"))
               {
                  SaveScript(lastRunScript); // need a filename here!
               }
               else if ((!strcmp(scriptfile, "--version")) || (!strcmp(scriptfile, "-v")))
               {
                  ShowVersionInfo();
               }
               else if (!strcmp(scriptfile, "--summary"))
               {
                  ShowCommandSummary();
               }
               else if (!strcmp(scriptfile, "--verbose"))
               {
                  settingVerbose = true;
               }
               else if (!strcmp(scriptfile, "--start-server"))
               {
                  std::cout << "\nGMAT server currently unavailable to GmatConsole\n ";
               }
               // Options used for some detailed tests but hidden from casual users
               // (i.e. missing from the help messages)
               else if (!strcmp(scriptfile, "--DumpDEData"))
               {
                  DumpDEData(0.001, 0.2);
               }
               else
               {
                  std::cout << "Unrecognized option.\n\n";
                  ShowHelp();
               }
            }
            else if (!runcomplete)
            {
               if (ignoreNext)
               {
                  ignoreNext = false;
                  continue;
               }
               if (settingVerbose)
               {
                  if (!strcmp(scriptfile, "off"))
                     verbosity = 0;
                  else if (!strcmp(scriptfile, "on"))
                     verbosity = 1;
                  else
                     MessageInterface::ShowMessage("Invalid option for --verbose: %s\n", scriptfile);
                  std::cout << "Verbose mode is "
                            << (verbosity == 0 ? "off" : "on")
                            << "\n";
                  settingVerbose = false;
               }
               else
               {
                  if (batchRun) // --batch or -b
                  {
                     std::string theFile(scriptfile);
                     RunBatch(theFile);
                  }
                  else  // --run or -r
                  {
                     RunScriptInterpreter(scriptfile, verbosity);
                     lastRunScript = scriptfile;
                  }
                  batchRun = false;
               }
            }

         } while (!runcomplete);
      }
      else   // non-interactive mode
      {
         std::string arg("");
         // check for a single filename argument first
         if (argc == 2)
         {
            arg = argv[1];
            if (!GmatStringUtil::StartsWith(arg, "-"))
            {
               // Replace single quotes
               GmatStringUtil::Replace(arg, "'", "");
               #ifdef DEBUG_CONSOLE
                  MessageInterface::ShowMessage(" --------> GmatConsole attempting to run script: %s\n", arg.c_str());
               #endif
               RunScriptInterpreter(arg.c_str(), verbosity);
               runcomplete = true;
            }
         }
         if (!runcomplete)
         {
            bool skipNext = false;
            for (int i = 1; i < argc; ++i)
            {
               arg = argv[i];
               if (skipNext)
               {
                  skipNext = false;
                  continue;
               }
               if (arg == "--start-server")
               {
                  std::cout << "\nGMAT server currently unavailable to GmatConsole\n ";
               }
               else if (arg == "--args")
               {
                  ; // ignore this - it's used by the open statement when opening the GUI version
               }
               else if ((arg == "--logfile") || (arg == "-l"))
               {
                  if (argc >= i + 2) // there could be a log file name next
                  {
                     std::string nextArg = argv[i+1];
                     if (nextArg[0] != '-')
                        skipNext = true; // skip log file name - handled previously
                  }
               }
               else if ((arg == "--startup_file") || (arg == "-s"))
               {
                  if (argc >= i + 2) // there could be a startup file name next
                  {
                     std::string nextArg = argv[i+1];
                     if (nextArg[0] != '-')
                        skipNext = true; // skip startup file name - handled previously
                  }
               }
               else if ((arg == "--minimize") || (arg == "-m"))
               {
                  std::cout << "\n--minimize option ignored by GmatConsole\n ";
               }
               else if ((arg == "--version") || (arg == "-v"))
               {
                  ShowVersionInfo();
               }
               else if (arg == "--save")
               {
                  ; // SaveScript(); // should this be ignored here? yes
               }
               else if (arg == "--summary")
               {
                  ShowCommandSummary();
               }
               else if ((arg == "--help") || (arg == "-h"))
               {
                  ShowHelp();
               }
               else if (arg == "--verbose")
               {
                  if (argc < i + 2)
                  {
                     MessageInterface::ShowMessage("*** Missing verbose value\n");
                  }
                  else
                  {
                     std::string verboseValue = argv[i+1];
                     ++i;
                     if (verboseValue == "off")
                        verbosity = 0;
                     else if (verboseValue == "on")
                        verbosity = 1;
                     else
                        MessageInterface::ShowMessage("Invalid option for --verbose: %s\n", scriptfile);
                     std::cout << "Verbose mode is "
                               << (verbosity == 0 ? "off" : "on")
                               << "\n";
                  }
               }
               else if ((arg == "--run") || (arg == "-r"))
               {
                  if (argc < i + 2)
                  {
                     MessageInterface::ShowMessage("*** Missing script file name\n");
                  }
                  else
                  {
                     std::string scriptToRun = argv[i+1];
                     // Replace single quotes
                     GmatStringUtil::Replace(scriptToRun, "'", "");
                     ++i;
                     #ifdef DEBUG_CONSOLE
                        MessageInterface::ShowMessage(" --------> GmatConsole attempting to run script: %s\n", scriptToRun.c_str());
                     #endif
                     RunScriptInterpreter(scriptToRun.c_str(), verbosity);
                  }
               }
               else if ((arg == "--batch") || (arg == "-b"))
               {
                  if (argc < i + 2)
                  {
                     MessageInterface::ShowMessage("*** Missing batch file name\n");
                  }
                  else
                  {
                     std::string batchToRun = argv[i+1];
                     // Replace single quotes
                     GmatStringUtil::Replace(batchToRun, "'", "");
                     ++i;
                     RunBatch(batchToRun);
                  }
               }
               else if ((arg == "--exit") || (arg == "-x"))
               {
                  ; // ignored - console always exits at end of non-interactive run
               }
               else
               {
                  MessageInterface::ShowMessage("The option \"%s\" is not valid.\n", arg.c_str());
                  ShowHelp();
                  break;
               }
            }
         } // !runcomplete
      }
   }
   catch (BaseException &ex)
   {
//      MessageInterface::ShowMessage("EXITing GmatConsole with exit code %d", EXIT_FAILURE);
      std::cout << "EXITing GmatConsole with exit code " << EXIT_FAILURE << std::endl;
      std::cout << ex.GetFullMessage() << std::endl;
      exit(EXIT_FAILURE);
   }
   
   Moderator::Instance()->Finalize();
   
   exit(EXIT_SUCCESS);
//   return 0; // EXIT_SUCCESS
}

