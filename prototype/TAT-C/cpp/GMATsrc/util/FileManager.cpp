//$Id$
//------------------------------------------------------------------------------
//                            FileManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun, NASA/GSFC
// Created: 2004/04/02
/**
 * Implements FileManager class. This is singleton class which manages
 * list of file paths and names.
 */
//------------------------------------------------------------------------------
 
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include "FileTypes.hpp"          // for GmatFile::MAX_PATH_LEN
#include "FileUtil.hpp"           // for GmatFileUtil::
#include "StringTokenizer.hpp"    // for StringTokenizer()
#include "GmatGlobal.hpp"         // for SetTestingMode()
#include <fstream>
#include <sstream>
#include <iomanip>

#include <algorithm>                    // Required for GCC 4.3

#ifndef _MSC_VER  // if not Microsoft Visual C++
#include <dirent.h>
#endif

// For adding default input path and files
//#define __FM_ADD_DEFAULT_INPUT__

//#define DEBUG_FILE_MANAGER
//#define DEBUG_GMAT_PATH
//#define DEBUG_ADD_FILETYPE
//#define DEBUG_ABS_PATH
//#define DEBUG_FILE_PATH
//#define DEBUG_SET_PATH
//#define DEBUG_READ_STARTUP_FILE
//#define DEBUG_WRITE_STARTUP_FILE
//#define DEBUG_PLUGIN_DETECTION
//#define DEBUG_FILE_RENAME
//#define DEBUG_MAPPING
//#define DEBUG_STARTUP_WITH_ABSOLUTE_PATH
//#define DEBUG_BIN_DIR
//#define DEBUG_TEXTURE_FILE
//#define DEBUG_3DMODEL_FILE
//#define DEBUG_FIND_PATH
//#define DEBUG_FIND_INPUT_PATH
//#define DEBUG_FIND_OUTPUT_PATH
//#define DEBUG_REFRESH_FILES
//#define DEBUG_WORKING_DIR

//---------------------------------
// static data
//---------------------------------
const std::string
FileManager::FILE_TYPE_STRING[FileTypeCount] =
{
   // File path
   "BEGIN_OF_PATH",
   "ROOT_PATH",
   
   // Input path
   "TIME_PATH",
   "PLANETARY_COEFF_PATH",
   "PLANETARY_EPHEM_DE_PATH",
   "PLANETARY_EPHEM_SPK_PATH",
   "VEHICLE_EPHEM_PATH",
   "VEHICLE_EPHEM_SPK_PATH",
   "VEHICLE_EPHEM_CCSDS_PATH",
   "EARTH_POT_PATH",
   "LUNA_POT_PATH",
   "VENUS_POT_PATH",
   "MARS_POT_PATH",
   "OTHER_POT_PATH",
   "TEXTURE_PATH",
   "BODY_3D_MODEL_PATH",
   "MEASUREMENT_PATH",
   "GUI_CONFIG_PATH",
   "SPLASH_PATH",
   "ICON_PATH",
   "STAR_PATH",
   "VEHICLE_MODEL_PATH",
   "SPAD_PATH",
   "ATMOSPHERE_PATH",
   "FILE_UPDATE_PATH",
   
   // Output path
   "OUTPUT_PATH",
   "END_OF_PATH",
   
   // General file name
   "LOG_FILE",
   "REPORT_FILE",
   "EPHEM_OUTPUT_FILE",
   "SPLASH_FILE",
   "TIME_COEFF_FILE",
   
   // Specific file name
   "DE405_FILE",
   "DE421_FILE",
   "DE424_FILE",
   "DE430_FILE",
   "IAUSOFA_FILE",
   "ICRF_FILE",
   "PLANETARY_SPK_FILE",
   "JGM2_FILE",
   "JGM3_FILE",
   "EGM96_FILE",
   "LP165P_FILE",
   "MGNP180U_FILE",
   "MARS50C_FILE",
   "EOP_FILE",
   "PLANETARY_COEFF_FILE",
   "NUTATION_COEFF_FILE",
   "PLANETARY_PCK_FILE",
   "EARTH_LATEST_PCK_FILE",
   "EARTH_PCK_PREDICTED_FILE",
   "EARTH_PCK_CURRENT_FILE",
   "LUNA_PCK_CURRENT_FILE",
   "LUNA_FRAME_KERNEL_FILE",
   "LEAP_SECS_FILE",
   "LSK_FILE",
   "PERSONALIZATION_FILE",
   "MAIN_ICON_FILE",
   "STAR_FILE",
   "CONSTELLATION_FILE",
   "SPACECRAFT_MODEL_FILE",
   "SPAD_SRP_FILE",
   "CSSI_FLUX_FILE",
   "SCHATTEN_FILE",
   "MARINI_TROPO_FILE",
   "HELP_FILE",};

FileManager* FileManager::theInstance = NULL;


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// FileManager* Instance(const std::string &appName = "GMAT.exe")
//------------------------------------------------------------------------------
FileManager* FileManager::Instance(const std::string &appName)
{
   if (theInstance == NULL)
      theInstance = new FileManager(appName);
   return theInstance;
}


//------------------------------------------------------------------------------
// ~FileManager()
//------------------------------------------------------------------------------
FileManager::~FileManager()
{
   for (std::map<std::string, FileInfo*>::iterator pos = mFileMap.begin();
        pos != mFileMap.end(); ++pos)
   {
      if (pos->second)
      {
         #ifdef DEBUG_FILE_MANAGER
         MessageInterface::ShowMessage
            ("FileManager::~FileManager deleting %s\n", pos->first.c_str());
         #endif

         delete pos->second;
      }
   }
}


//------------------------------------------------------------------------------
// std::string GetBinDirectory(const std::string &appName = "GMAT.exe")
//------------------------------------------------------------------------------
std::string FileManager::GetBinDirectory(const std::string &appName)
{
   #ifdef DEBUG_BIN_DIR
   MessageInterface::ShowMessage
      ("FileManager::GetBinDirectory() entered, appName = %s, mAbsBinDir = '%s'\n",
       appName.c_str(), mAbsBinDir.c_str());
   #endif
   
   if (mAbsBinDir == "")
      SetBinDirectory(appName);
   
   #ifdef DEBUG_BIN_DIR
   MessageInterface::ShowMessage
      ("FileManager::GetBinDirectory() returning '%s'\n", mAbsBinDir.c_str());
   #endif
   
   return mAbsBinDir;
}


//------------------------------------------------------------------------------
// bool SetBinDirectory(const std::string &appName = "GMAT.exe",
//                      const std::string &binDir = "")
//------------------------------------------------------------------------------
/**
 * Sets bin directory where GMAT.exe reside. It sets only once when GMAT.exe
 * found in the directory. If input binDir is blank, it will try with
 * GmatFileUtil::GetApplicationPath().
 */
//------------------------------------------------------------------------------
bool FileManager::SetBinDirectory(const std::string &appName, const std::string &binDir)
{
   #ifdef DEBUG_BIN_DIR
   MessageInterface::ShowMessage
      ("FileManager::SetBinDirectory() entered, appName = '%s', binDir = '%s', mAbsBinDir = '%s'\n",
       appName.c_str(), binDir.c_str(), mAbsBinDir.c_str());
   #endif
   
   if (mAbsBinDir == "")
   {
      std::string appFullPath = binDir;
      if (binDir == "")
         appFullPath = GmatFileUtil::GetApplicationPath();
      
      #ifdef DEBUG_BIN_DIR
      MessageInterface::ShowMessage("   appFullPath = '%s'\n", appFullPath.c_str());
      #endif
      
      // Set absolute bin directory if it is not relative path and appName found
      if (appFullPath[0] != '.')
      {
         std::string appPath = GmatFileUtil::ParsePathName(appFullPath);
         std::string newPath = appPath + appName;
         if (GmatFileUtil::DoesFileExist(newPath))
         {
            mAbsBinDir = appPath;
            
            #ifdef DEBUG_BIN_DIR
            MessageInterface::ShowMessage
               ("FileManager::SetBinDirectory() returning true, bin directory set to '%s'\n",
                mAbsBinDir.c_str());
            #endif
            return true;
         }
         else
         {
            #ifdef DEBUG_BIN_DIR
            MessageInterface::ShowMessage
               ("   The file '%s' does not exist\n", newPath.c_str());
            #endif
         }
      }
   }
   else
   {
      #ifdef DEBUG_BIN_DIR
      MessageInterface::ShowMessage
         ("   The bin directory already set to '%s'\n", mAbsBinDir.c_str());
      #endif
   }
   
   #ifdef DEBUG_BIN_DIR
   MessageInterface::ShowMessage
      ("FileManager::SetBinDirectory() returning false, mAbsBinDir = '%s'\n", mAbsBinDir.c_str());
   #endif
   return false;
}


//------------------------------------------------------------------------------
// std::string GetGmatWorkingDirectory()
//------------------------------------------------------------------------------
/**
 * Returns GMAT working directory.  This is the directory where script is passed
 * to GMAT from the command line.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetGmatWorkingDirectory()
{
   return mGmatWorkingDir;
}


//------------------------------------------------------------------------------
// bool SetGmatWorkingDirectory(const std::string &newDir = "")
//------------------------------------------------------------------------------
/**
 * Sets GMAT working directory.  This is the directory where script resides.
 */
//------------------------------------------------------------------------------
bool FileManager::SetGmatWorkingDirectory(const std::string &newDir)
{
#ifdef DEBUG_WORKING_DIR
   MessageInterface::ShowMessage("Entering SetGmatWorkingDirectory with '%s'\n",
                                 newDir.c_str());
#endif
   // Allow resetting on purpose
   if (newDir == "")
   {
      mGmatWorkingDir = newDir;
   }
   else
   {
      if (DoesDirectoryExist(newDir))
      {
#ifdef DEBUG_WORKING_DIR
         MessageInterface::ShowMessage("In SetGmatWorkingDirectory - it DOES exist\n");
#endif
         mGmatWorkingDir = newDir;
         AddGmatIncludePath(newDir);
         // Add GMAT working directory to MATLAB search path so that this directory
         // will have higher priority in search path for the new file path implementation.
         // (LOJ: 2014.07.09)
         AddMatlabFunctionPath(newDir);
         // Also add it to GmatFunction path (LOJ: 2015.09.18)
         AddGmatFunctionPath(newDir);
//         //Python
//         AddPythonModulePath(newDir);
      }
      else
      {
#ifdef DEBUG_WORKING_DIR
         MessageInterface::ShowMessage("In SetGmatWorkingDirectory - it DOES NOT exist\n");
#endif
         return false;
      }
   }
   
   return true;
}


//------------------------------------------------------------------------------
// std::string GetCurrentWorkingDirectory()
//------------------------------------------------------------------------------
/**
 * @return System's current working directory of the process
 */
//------------------------------------------------------------------------------
std::string FileManager::GetCurrentWorkingDirectory()
{
   return GmatFileUtil::GetCurrentWorkingDirectory();
}


//------------------------------------------------------------------------------
// bool SetCurrentWorkingDirectory(const std::string &newDir = "")
//------------------------------------------------------------------------------
/**
 *  Sets system's current working directory of the process.
 */
//------------------------------------------------------------------------------
bool FileManager::SetCurrentWorkingDirectory(const std::string &newDir)
{
   return GmatFileUtil::SetCurrentWorkingDirectory(newDir);
}


//------------------------------------------------------------------------------
// std::string FindPath(const std::string &fileName, const FileType type,
//                      bool forInput, bool writeWarning = false, bool writeInfo = false,
//                      const std::string &objName)
//------------------------------------------------------------------------------
/**
 * Finds path for requested fileName using the file path search order.
 * This method calls FindPath() taking type name.
 *
 * @return path found using search order
 */
//------------------------------------------------------------------------------
std::string FileManager::FindPath(const std::string &fileName, const FileType type,
                                  bool forInput, bool writeWarning, bool writeInfo,
                                  const std::string &objName)
{
   #ifdef DEBUG_FIND_PATH
   MessageInterface::ShowMessage
      ("FileManager::FindPath() entered, fileName = '%s', type = %d, forInput = %d, "
       "writeWarning = %d, writeInfo = %d\n", fileName.c_str(), type, forInput,
       writeWarning, writeInfo);
   #endif
   
   std::string typeName;
   if (type >=0 && type < FileTypeCount)
   {
      typeName = FILE_TYPE_STRING[type];
   }
   else
   {
      std::stringstream ss("");
      ss << "*** INTERNAL ERROR *** FileManager::FindPath() enum type: " << type
         << " is out of bounds\n";
      
      throw UtilityException(ss.str());
   }
   
   return FindPath(fileName, typeName, forInput, writeWarning, writeInfo, objName);
}


//------------------------------------------------------------------------------
// std::string FindPath(const std::string &fileName, const std::string &fileType,
//                      bool forInput, bool writeWarning = false, bool writeInfo = false,
//                      const std::string &objName = "")
//------------------------------------------------------------------------------
/**
 * Finds path for requested fileName. If fileName has a absolute path, it will
 * return fileName or blank if path not found. If fileName has a relative path or
 * no path, it will find path using the following file path search order.
 * For Input:
 *    1) Current GMAT working directory
 *    2) Directory from the startup file in the application directory
 * For Output:
 *    1) Current GMAT working directory if it has relative path
 *    2) Directory from the startup file in the application directory
 *       if no path found
 *    3) Application directory
 *
 * It returns blank if filename is blank
 * It returns blank if path not found for input file.
 * If input fileName is blank, it uses default filename using the type
 *
 * @param  fileName     The requested filename to be searched
 *                      Enter blank name if default name to be used for the type
 * @param  fileType     The file type name of the input file
 * @param  forInput     Set to true if filename is for input
 * @param  writeWarning Set to true if warning should be written when no path found
 * @param  writeInfo    Set to true if information should be written for output path (currently not used)
 * @param  objName      The name of the calling object to be written to informational message
 *
 * @return full path name using search order
 */
//------------------------------------------------------------------------------
std::string FileManager::FindPath(const std::string &fileName, const std::string &fileType,
                                  bool forInput, bool writeWarning, bool writeInfo,
                                  const std::string &objName)
{
#ifdef DEBUG_FIND_PATH
   MessageInterface::ShowMessage
   ("FileManager::FindPath() entered\n   fileName = '%s'\n   fileType = '%s', forInput = %d, "
    "writeWarning = %d, writeInfo = %d, objName = '%s'\n", fileName.c_str(), fileType.c_str(),
    forInput, writeWarning, writeInfo, objName.c_str());
#endif
   
   mLastFilePathMessage = "";
   std::string fullname = fileName;
   bool writeFilePathInfo = GmatGlobal::Instance()->IsWritingFilePathInfo();
#ifdef DEBUG_FIND_PATH
   MessageInterface::ShowMessage("   writeFilePathInfo = %d\n", writeFilePathInfo);
#endif
   
   // If input filename is blank, get default name using type
   try
   {
      if (fileName == "")
         fullname = GetFilename(fileType);
   }
   catch (BaseException &be)
   {
#ifdef DEBUG_FIND_PATH
      MessageInterface::ShowMessage(be.GetFullMessage());
#endif
   }
   
   // Cannot handle blank, return blank
   if (fullname == "")
   {
#ifdef DEBUG_FIND_PATH
      MessageInterface::ShowMessage
      ("FileManager::FindPath() cannot find default filename for type '%s', "
       "so just returning blank\n", fileType.c_str());
#endif
      return "";
   }
   
   fullname = GmatFileUtil::ConvertToOsFileName(fullname);
   std::string pathOnly = GmatFileUtil::ParsePathName(fullname);
   std::string fileOnly = GmatFileUtil::ParseFileName(fullname);
   std::string gmatPath = GmatFileUtil::ConvertToOsFileName(mGmatWorkingDir);
   
   // Get default path for file type
   std::string defaultPath;
   try
   {
      defaultPath = GmatFileUtil::ConvertToOsFileName(GetPathname(fileType));
   }
   catch (BaseException &be)
   {
#ifdef DEBUG_FIND_PATH
      MessageInterface::ShowMessage("*** WARNING *** %s\n", be.GetFullMessage().c_str());
#endif
      // If *_POT_PATH, try OTHER_POT_PATH
      std::string::size_type potLoc = fileType.find("_POT_PATH");
      if (potLoc != fileType.npos)
      {
#ifdef DEBUG_FIND_PATH
         MessageInterface::ShowMessage("Trying OTHER_POT_PATH\n");
#endif
         std::string oldPot = fileType.substr(0, potLoc+1);
         std::string newPotPath = GmatStringUtil::Replace(fileType, oldPot, "OTHER_");
         defaultPath = GmatFileUtil::ConvertToOsFileName(GetPathname(newPotPath));
      }
   }
   
   std::string tempPath1, tempPath2;
   std::string pathToReturn;
   
#ifdef DEBUG_FIND_PATH
   MessageInterface::ShowMessage
   ("   fullname = '%s'\n   pathOnly = '%s'\n   fileOnly = '%s'\n   gmatpath = '%s'\n"
    "   defaultpath = '%s'\n", fullname.c_str(), pathOnly.c_str(), fileOnly.c_str(),
    gmatPath.c_str(), defaultPath.c_str());
#endif
   
   if (GmatFileUtil::IsPathAbsolute(fullname))
   {
#ifdef DEBUG_FIND_PATH
      MessageInterface::ShowMessage("   The filename has absolute path\n");
#endif
      
      if (GmatFileUtil::DoesFileExist(fullname))
      {
         pathToReturn = fullname;
      }
      else
      {
#ifdef DEBUG_FIND_PATH
         MessageInterface::ShowMessage("   The filename does not exist\n");
#endif
         if (forInput)
         {
            pathToReturn = "";
            if (writeWarning && gmatPath != "" && writeFilePathInfo)
            {
               MessageInterface::ShowMessage
               ("The input file '%s' does not exist\n", fullname.c_str());
            }
         }
         else // for output
         {
#ifdef DEBUG_FIND_PATH
            MessageInterface::ShowMessage
            ("   It is for output, so checking if directory '%s' exist\n",
             pathOnly.c_str());
#endif
            if (DoesDirectoryExist(pathOnly, false))
            {
#ifdef DEBUG_FIND_PATH
               MessageInterface::ShowMessage("   The directory exist\n");
#endif
               pathToReturn = fullname;
            }
            else
            {
               pathToReturn = "";
               if (writeWarning && gmatPath != "" && writeFilePathInfo)
               {
                  mLastFilePathMessage = "Cannot open output file '" + fullname +
                  "', the path '" + pathOnly + "' does not exist.";
                  MessageInterface::ShowMessage(mLastFilePathMessage + "\n");
               }
            }
         }
      }
   }
   else // filename without absolute path
   {
      #ifdef DEBUG_FIND_PATH
         MessageInterface::ShowMessage("   The filename does not have absolute path\n");
      #endif
      
      if (forInput)
      {
         // First search in GMAT working directory.
         // If GMAT directory is blank give some dummy name so that it can be failed to search
         if (gmatPath == "")
            tempPath1 = "__000_gmat_working_dir_is_blank_000__" + fullname;
         else
            tempPath1 = gmatPath + fullname;
         
         #ifdef DEBUG_FIND_INPUT_PATH
            MessageInterface::ShowMessage("   => first search Path = '%s'\n", tempPath1.c_str());
         #endif
         
         if (GmatFileUtil::DoesFileExist(tempPath1))
         {
            pathToReturn = tempPath1;
         }
         else
         {
            #ifdef DEBUG_FIND_INPUT_PATH
               MessageInterface::ShowMessage
               ("   '%s' does not exist, so search in default path\n", tempPath1.c_str());
               MessageInterface::ShowMessage
               ("   BinDirectory            = '%s'\n", mAbsBinDir.c_str());
               MessageInterface::ShowMessage
               ("   CurrentWorkingDirectory = '%s'\n", GetCurrentWorkingDirectory().c_str());
            #endif
            
            if (GmatFileUtil::IsPathRelative(fullname))
            {
               // Check relative to script directory first
               // Then check relative to bin directory
               tempPath2 = mAbsBinDir + fullname;
            }
            else
               tempPath2 = defaultPath + fullname;
            
               #ifdef DEBUG_FIND_INPUT_PATH
                  MessageInterface::ShowMessage
                        ("   => next search path = '%s' \n", tempPath2.c_str());
               #endif
            
            if (writeWarning && gmatPath != "" && writeFilePathInfo)
               MessageInterface::ShowMessage
               ("The input file '%s' does not exist in GMAT "
                "working directory\n   '%s', so trying default path from the "
                "startup file\n   '%s'\n", fullname.c_str(), tempPath1.c_str(),
                tempPath2.c_str());
            
            if (GmatFileUtil::DoesFileExist(tempPath2))
            {
               pathToReturn = tempPath2;
            }
            else
            {
               pathToReturn = "";
               if (writeWarning && gmatPath != "" && writeFilePathInfo)
                  MessageInterface::ShowMessage
                  ("*** WARNING *** The input file '%s' does not exist in default "
                   "path from the startup file '%s'\n", fullname.c_str(), tempPath2.c_str());
            }
         }
      }
      else // for output
      {
         if (GmatFileUtil::IsPathRelative(fullname))
         {
            #ifdef DEBUG_FIND_OUTPUT_PATH
               MessageInterface::ShowMessage("   The output filename has relative path\n");
            #endif
            
            // Check GMAT working (script) directory
            std::string tempPath = gmatPath + fullname;
            std::string outPath1 = GmatFileUtil::ParsePathName(tempPath);
            
            #ifdef DEBUG_FIND_OUTPUT_PATH
               MessageInterface::ShowMessage("   Checking if '%s' exist...\n",
                                             outPath1.c_str());
            #endif
            
            if (DoesDirectoryExist(outPath1, false))
            {
               pathToReturn = tempPath;
            }
            else
            {
               tempPath = defaultPath + fullname;
               std::string outPath2 = GmatFileUtil::ParsePathName(tempPath);
               
               #ifdef DEBUG_FIND_OUTPUT_PATH
                  MessageInterface::ShowMessage
                  ("   '%s' does not exist.\n   So checking if '%s' exist ...\n",
                  outPath1.c_str(), outPath2.c_str());
               #endif
               
               if (DoesDirectoryExist(outPath2, false))
               {
                  pathToReturn = tempPath;
               }
//               // Check the default path for the filetype
//               if (DoesDirectoryExist(defaultPath, false))
//                  pathToReturn = defaultPath + fullname;
               else
               {
                  std::string errmsg = "Error finding relative path for file \"";
                  errmsg += fullname + "\" - ";
                  errmsg += "specified path does not exist.\n";
                  throw UtilityException(errmsg);
               }
               //               tempPath = defaultPath + fullname;
               //               std::string outPath2 = GmatFileUtil::ParsePathName(tempPath);
               //
               //               #ifdef DEBUG_FIND_OUTPUT_PATH
               //               MessageInterface::ShowMessage
               //                  ("   '%s' does not exist.\n   So checking if '%s' exist ...\n",
               //                   outPath1.c_str(), outPath2.c_str());
               //               #endif
               //
               //               if (DoesDirectoryExist(outPath2, false))
               //               {
               //                  pathToReturn = tempPath;
               //               }
               //               else
               //               {
               //                  pathToReturn = mAbsBinDir + fileOnly;
               //                  #ifdef DEBUG_FIND_OUTPUT_PATH
               //                  MessageInterface::ShowMessage
               //                     ("   '%s' does not exist.\n   So set to use bin directory '%s'\n",
               //                      outPath2.c_str(), mAbsBinDir.c_str());
               //                  #endif
               //               }
            }
         }
         else // filename without any path
         {
            if (DoesDirectoryExist(defaultPath, false))
               pathToReturn = defaultPath + fullname;
            else
               pathToReturn = mAbsBinDir + fullname;
         }
      }
   }
   
   // Write info only if file path debug is on from the startup file (LOJ: 2014.09.22)
   // Write information about file location if file path debug mode is on
   
   std::string ioType = "output";
   std::string fType = "";
   std::string rwType = "written to";
   std::string oName = "";
   
   if (fileType.find("_FILE") != fileType.npos)
      fType = fileType + " ";
   
   if (forInput)
   {
      ioType = "input";
      rwType = "read from";
   }
   
   if (objName != "")
      oName = " for the object '" + objName + "'";
   
   // Write message where output goes or input from
   if (pathToReturn != "")
   {
      if (writeFilePathInfo)
         MessageInterface::ShowMessage
         ("*** The %s %sfile '%s'%s will be %s \n                    '%s'\n",
          ioType.c_str(), fType.c_str(), fullname.c_str(), oName.c_str(),
          rwType.c_str(), pathToReturn.c_str());
   }
   else
   {
      mLastFilePathMessage = "Cannot open " + ioType + " " + fType +
      "'" + fullname + "'";
      if (writeFilePathInfo)
         MessageInterface::ShowMessage(mLastFilePathMessage + "\n");
   }
   
   #ifdef DEBUG_FIND_PATH
      MessageInterface::ShowMessage
      ("FileManager::FindPath() returning '%s'\n", pathToReturn.c_str());
   #endif
   
   return pathToReturn;
} // FindPath()



//------------------------------------------------------------------------------
// std::string FindMainIconFile(bool writeInfo = true)
//------------------------------------------------------------------------------
std::string FileManager::FindMainIconFile(bool writeInfo)
{
   // Changed not to write warning per GMAT session (LOJ: 2014.10.29)
   #ifdef __WRITE_WARNING_PER_SESSION_
   static bool writeWarning = true;
   std::string fullpath = FindPath("", MAIN_ICON_FILE, true, writeWarning, writeInfo);
   if (mGmatWorkingDir != "")
      writeWarning = false;
   #else
   std::string fullpath = FindPath("", MAIN_ICON_FILE, true, false, writeInfo);
   #endif
   
   return fullpath;
}


//------------------------------------------------------------------------------
// std::string GetPathSeparator()
//------------------------------------------------------------------------------
/**
 * @return path separator; "/" or "\\" dependends on the platform
 */
//------------------------------------------------------------------------------
std::string FileManager::GetPathSeparator()
{
   // Changed back to return FileUtil::GetPathSeparator(); (LOJ: 2014.06.09)
   // Just return "/" for all operating system for consistency (LOJ: 2011.03.18)
   //return "/";
   
   return GmatFileUtil::GetPathSeparator();
}


//------------------------------------------------------------------------------
// bool DoesDirectoryExist(const std::string &dirPath, bool isBlankOk = true)
//------------------------------------------------------------------------------
/*
 * @return  true  If directory exist, false otherwise
 */
//------------------------------------------------------------------------------
bool FileManager::DoesDirectoryExist(const std::string &dirPath, bool isBlankOk)
{
   return GmatFileUtil::DoesDirectoryExist(dirPath, isBlankOk);
}


//------------------------------------------------------------------------------
// bool DoesFileExist(const std::string &filename)
//------------------------------------------------------------------------------
bool FileManager::DoesFileExist(const std::string &filename)
{
   return GmatFileUtil::DoesFileExist(filename);
}


//------------------------------------------------------------------------------
// bool RenameFile(const std::string &oldName, const std::string &newName,
//                 Integer &retCode, bool overwriteIfExists = false)
//------------------------------------------------------------------------------
bool FileManager::RenameFile(const std::string &oldName,
                             const std::string &newName,
                             Integer &retCode, bool overwriteIfExists)
{
   retCode = 0;
   bool oldExists = DoesFileExist(oldName);
   bool newExists = DoesFileExist(newName);
   #ifdef DEBUG_FILE_RENAME
      MessageInterface::ShowMessage("FM::Rename, old file (%s) exists = %s\n",
            oldName.c_str(), (oldExists? "true" : "false"));
      MessageInterface::ShowMessage("FM::Rename, new file (%s) exists = %s\n",
            newName.c_str(), (newExists? "true" : "false"));
   #endif
   // if a file with the old name does not exist, we cannot do anything
   if (!oldExists)
   {
      std::string errmsg = "Error renaming file \"";
      errmsg += oldName + "\" to \"";
      errmsg += newName + "\": file \"";
      errmsg += oldName + "\" does not exist.\n";
      throw UtilityException(errmsg);
   }

   // if a file with the new name does not exist, or exists but we are
   // supposed to overwrite it, try to do the rename
   if ((!newExists) || (newExists && overwriteIfExists))
   {
      #ifdef DEBUG_FILE_RENAME
         MessageInterface::ShowMessage("FM::Rename, attempting to rename %s to %s\n",
               oldName.c_str(), newName.c_str());
      #endif
      retCode = rename(oldName.c_str(), newName.c_str()); // overwriting is platform-dependent!!!!
      if (retCode == 0)
      {
         return true;
      }
      else
      {
         return false;
      }
   }
   else // it exists but we are not to overwrite it
      return false;
}


//------------------------------------------------------------------------------
// bool CopyFile(const std::string &oldName, const std::string &newName,
//                 Integer &retCode, bool overwriteIfExists = false)
//------------------------------------------------------------------------------
bool FileManager::CopyFile(const std::string &oldName,
                             const std::string &newName,
                             Integer &retCode, bool overwriteIfExists)
{
   retCode = 0;
   if (oldName == newName) return true;

   bool oldExists = DoesFileExist(oldName);
   bool newExists = DoesFileExist(newName);
   #ifdef DEBUG_FILE_RENAME
      MessageInterface::ShowMessage("FM::Copy, old file (%s) exists = %s\n",
            oldName.c_str(), (oldExists? "true" : "false"));
      MessageInterface::ShowMessage("FM::Copy, new file (%s) exists = %s\n",
            newName.c_str(), (newExists? "true" : "false"));
   #endif
   // if a file with the old name does not exist, we cannot do anything
   if (!oldExists)
   {
      std::string errmsg = "Error copying file \"";
      errmsg += oldName + "\" to \"";
      errmsg += newName + "\": file \"";
      errmsg += oldName + "\" does not exist.\n";
      throw UtilityException(errmsg);
   }

   // if a file with the new name does not exist, or exists but we are
   // supposed to overwrite it, try to do the rename
   if ((!newExists) || (newExists && overwriteIfExists))
   {
      #ifdef DEBUG_FILE_RENAME
         MessageInterface::ShowMessage("FM::Copy, attempting to copy %s to %s\n",
               oldName.c_str(), newName.c_str());
      #endif
      std::ifstream src(oldName.c_str(), std::ios::binary);
      std::ofstream dest(newName.c_str(), std::ios::binary);
      dest << src.rdbuf();
      retCode = src && dest;
      return retCode == 1;
   }
   else // it exists but we are not to overwrite it
      return false;
}


//------------------------------------------------------------------------------
// std::string GetStartupFileDir()
//------------------------------------------------------------------------------
/*
 * Returns startup file directory without name.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetStartupFileDir()
{
   return mStartupFileDir;
}


//------------------------------------------------------------------------------
// std::string GetStartupFileName()
//------------------------------------------------------------------------------
/*
 * Returns startup file name without directory.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetStartupFileName()
{
   return mStartupFileName;
}


//------------------------------------------------------------------------------
// std::string GetFullStartupFilePath()
//------------------------------------------------------------------------------
/*
 * Returns startup file directory and name
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFullStartupFilePath()
{
   #ifdef DEBUG_STARTUP_FILE
   MessageInterface::ShowMessage
      ("FileManager::GetFullStartupFilePath() mStartupFileDir='%s', "
       "mStartupFileName='%s'\n", mStartupFileDir.c_str(), mStartupFileName.c_str());
   #endif
   
   if (mStartupFileDir == "")
      return mStartupFileName;
   else
   {
      mStartupFileDir = GmatFileUtil::GetCurrentWorkingDirectory() + mPathSeparator;
      return mStartupFileDir + mStartupFileName;
   }
}


//void FileManager::ReadStartupFile(const char *fileName)
//{
//   ReadStartupFile(std::string(fileName));
//}


//------------------------------------------------------------------------------
// void ReadStartupFile(const std::string &fileName = "")
//------------------------------------------------------------------------------
/**
 * Reads GMAT startup file.
 *
 * @param <fileName> startup file name.
 *
 */
//------------------------------------------------------------------------------
void FileManager::ReadStartupFile(const std::string &fileName)
{
   #ifdef DEBUG_READ_STARTUP_FILE
   MessageInterface::ShowMessage
      ("FileManager::ReadStartupFile() entered, fileName='%s'\n", fileName.c_str());
   #endif
   
   RefreshFiles();
   
   // Set bin directory
   SetBinDirectory();
   
   // get current path and application path
   std::string currPath = GmatFileUtil::GetCurrentWorkingDirectory();
   std::string appFullPath = GmatFileUtil::GetApplicationPath();
   
   #ifdef DEBUG_READ_STARTUP_FILE
   MessageInterface::ShowMessage("   currPath = '%s'\n", currPath.c_str());
   MessageInterface::ShowMessage("   appFullPath = '%s'\n", appFullPath.c_str());
   #endif
   
   std::string line;
   mSavedComments.clear();

   std::string tmpStartupDir;
   std::string tmpStartupFile;
   std::string tmpStartupFilePath;

   if (GmatFileUtil::DoesFileExist(fileName))
   {
      #ifdef DEBUG_READ_STARTUP_FILE
      MessageInterface::ShowMessage("   startup file '%s' exist.\n", fileName.c_str());
      #endif
      tmpStartupFilePath = fileName;
   }
   else
   {
      #ifdef DEBUG_READ_STARTUP_FILE
      MessageInterface::ShowMessage
         ("   startup file '%s' does not exist, \n   so look in bin directory '%s'\n",
          fileName.c_str(), appFullPath.c_str());
      #endif
      
      // Search application directory for startup file
      std::string appPath = GmatFileUtil::ParsePathName(appFullPath);
      std::string newPath = appPath + "gmat_startup_file.txt";
      
      #ifdef DEBUG_READ_STARTUP_FILE
      MessageInterface::ShowMessage("   appPath               = '%s'\n", appPath.c_str());
      MessageInterface::ShowMessage("   new startup file path = '%s'\n", newPath.c_str());
      #endif
      
      if (GmatFileUtil::DoesFileExist(newPath))
      {
         tmpStartupFilePath = newPath;
         
         // set current directory to new path
         if (SetCurrentWorkingDirectory(appPath))
         {
            MessageInterface::ShowMessage
               ("GMAT working directory set to '%s'\n", appPath.c_str());
         }
         else
         {
            UtilityException ue;
            ue.SetDetails("FileManager::ReadStartupFile() cannot set working "
                          "directory to: \"%s\"", appPath.c_str());
            throw ue;
         }
      }
	  else
	  {
		  tmpStartupFilePath = newPath;
	  }
   }
   
   tmpStartupDir = GmatFileUtil::ParsePathName(tmpStartupFilePath);
   tmpStartupFile = GmatFileUtil::ParseFileName(tmpStartupFilePath);
   #ifdef DEBUG_READ_STARTUP_FILE
   MessageInterface::ShowMessage("   tmpStartupDir  = '%s'\n", tmpStartupDir.c_str());
   MessageInterface::ShowMessage("   tmpStartupFile = '%s'\n", tmpStartupFile.c_str());
   #endif
   
   if (tmpStartupDir == "")
      tmpStartupFilePath = tmpStartupFile;
   else
      tmpStartupFilePath = tmpStartupDir + mPathSeparator + tmpStartupFile;
   
//   // Reworked this part above so removed(LOJ: 2011.12.14)
//   #if 0
//   if (fileName == "")
//   {
//      tmpStartupDir = "";
//      tmpStartupFile = mStartupFileName;
//      tmpStartupFilePath = mStartupFileName;
//   }
//   else
//   {
//      tmpStartupDir = GmatFileUtil::ParsePathName(fileName);
//      tmpStartupFile = GmatFileUtil::ParseFileName(fileName);
//
//      if (tmpStartupDir == "")
//         tmpStartupFilePath = tmpStartupFile;
//      else
//         tmpStartupFilePath = tmpStartupDir + mPathSeparator + tmpStartupFile;
//   }
//   #endif

   
   #ifdef DEBUG_READ_STARTUP_FILE
   MessageInterface::ShowMessage
      ("FileManager::ReadStartupFile() reading '%s'\n", tmpStartupFilePath.c_str());
   #endif

   std::ifstream mInStream(tmpStartupFilePath.c_str());

   if (!mInStream)
   {
      UtilityException ue;
      ue.SetDetails("FileManager::ReadStartupFile() cannot open GMAT startup "
                    "file: \"%s\"", tmpStartupFilePath.c_str());
      throw ue;
   }

   
   // Read startup file
   while (!mInStream.eof())
   {
      // Use cross-platform GetLine
      GmatFileUtil::GetLine(&mInStream, line);
      
      #ifdef DEBUG_READ_STARTUP_FILE
      MessageInterface::ShowMessage("line=%s\n", line.c_str());
      #endif

      // Skip empty line or comment line
      if (line.length() > 0)     // Crashes in VS 2010 debugger without this
      {
         if (line[0] == '\0' || line[0] == '#')
         {
            // save line with ## in the first col
            if (line.size() > 1 && line[1] == '#')
               mSavedComments.push_back(line);
            continue;
         }
      }
      else
         continue;

      std::string type, equal, name;
      std::stringstream ss("");

      ss << line;
      ss >> type >> equal;

      if (equal != "=")
      {
         mInStream.close();
         throw UtilityException
            ("FileManager::ReadStartupFile() expecting '=' at line:\n" +
             std::string(line) + "\n");
      }

      // To fix bug 1916 (LOJ: 2010.10.08)
      // Since >> uses space as deliminter, we cannot use it.
      // So use GmatStringUtil::DecomposeBy() instead.
      //ss >> name;
      
      StringArray parts = GmatStringUtil::DecomposeBy(line, "=");
      name = parts[1];
      name = GmatStringUtil::Trim(name);
      
      #ifdef DEBUG_READ_STARTUP_FILE
      MessageInterface::ShowMessage("type=%s, name=%s\n", type.c_str(), name.c_str());
      #endif
      
      
      if (type == "RUN_MODE")
      {
         mRunMode = name;
         if (name == "TESTING")
            GmatGlobal::Instance()->SetRunMode(GmatGlobal::TESTING);
         else if (name == "TESTING_NO_PLOTS")
            GmatGlobal::Instance()->SetRunMode(GmatGlobal::TESTING_NO_PLOTS);
         else if (name == "EXIT_AFTER_RUN")
            GmatGlobal::Instance()->SetRunMode(GmatGlobal::EXIT_AFTER_RUN);
      }
      else if (type == "PLOT_MODE")
      {
         mPlotMode = name;
         if (name == "TILE")
            GmatGlobal::Instance()->SetPlotMode(GmatGlobal::TILED_PLOT);
      }
      else if (type == "MATLAB_MODE")
      {
         mMatlabMode = name;
         if (name == "SINGLE")
            GmatGlobal::Instance()->SetMatlabMode(GmatGlobal::SINGLE_USE);
         else if (name == "SHARED")
            GmatGlobal::Instance()->SetMatlabMode(GmatGlobal::SHARED);
         else if (name == "NO_MATLAB")
            GmatGlobal::Instance()->SetMatlabMode(GmatGlobal::NO_MATLAB);
      }
      else if (type == "DEBUG_MATLAB")
      {
         if (name == "ON")
         {
            mDebugMatlab = name;
            GmatGlobal::Instance()->SetMatlabDebug(true);
         }
      }
      else if (type == "DEBUG_MISSION_TREE")
      {
         if (name == "ON")
         {
            mDebugMissionTree = name;
            GmatGlobal::Instance()->SetMissionTreeDebug(true);
         }
      }
      else if (type == "DEBUG_PARAMETERS")
      {
         if (name == "ON")
         {
            mWriteParameterInfo = name;
            GmatGlobal::Instance()->SetWriteParameterInfo(true);
         }
      }
      else if (type == "DEBUG_FILE_PATH")
      {
         if (name == "ON")
         {
            mWriteFilePathInfo = name;
            GmatGlobal::Instance()->SetWriteFilePathInfo(true);
         }
      }
      else if (type == "WRITE_GMAT_KEYWORD")
      {
         if (name == "OFF")
         {
            mWriteGmatKeyword = name;
            GmatGlobal::Instance()->SetWriteGmatKeyword(false);
         }
      }
      else if (type == "WRITE_PERSONALIZATION_FILE")
      {
         if (name == "ON")
            GmatGlobal::Instance()->SetWritePersonalizationFile(true);
         else
            GmatGlobal::Instance()->SetWritePersonalizationFile(false);
      }
      else if (type == "HIDE_SAVEMISSION")
      {
         if (name == "TRUE")
            GmatGlobal::Instance()->AddHiddenCommand("SaveMission");
         else
            GmatGlobal::Instance()->RemoveHiddenCommand("SaveMission");
      }
      else if (type == "ECHO_COMMANDS")
      {
         if (name == "TRUE")
            GmatGlobal::Instance()->SetCommandEchoMode(true);
         else
            GmatGlobal::Instance()->SetCommandEchoMode(false);

      }
      else if (type == "NO_SPLASH")
      {
         if (name == "TRUE")
            GmatGlobal::Instance()->SetSkipSplashMode(true);
         else
            GmatGlobal::Instance()->SetSkipSplashMode(false);
      }
      else
      {
         // Ignore old VERSION specification (2011.03.18)
         if (type != "VERSION")
            AddFileType(type, name);
      }
   } // end While()
   
   // Since we set all output to ./ as default, we don't need this (LOJ: 2011.03.17)
   // Set VEHICLE_EPHEM_CCSDS_PATH to OUTPUT_PATH from the startup file if not set
   // so that ./output directory is not required when writing the ephemeris file.
//    if (mPathMap["VEHICLE_EPHEM_CCSDS_PATH"] == "./output/" &&
//        mPathMap["OUTPUT_PATH"] != "./files/output/")
//    {
//       mPathMap["VEHICLE_EPHEM_CCSDS_PATH"] = mPathMap["OUTPUT_PATH"];
//       #ifdef DEBUG_READ_STARTUP_FILE
//       MessageInterface::ShowMessage
//          ("==> VEHICLE_EPHEM_CCSDS_PATH set to '%s'\n", mPathMap["VEHICLE_EPHEM_CCSDS_PATH"].c_str());
//       #endif
//    }
   
   // add potential files by type names
   AddAvailablePotentialFiles();
   
   // save good startup file
   mStartupFileDir = tmpStartupDir;
   mStartupFileName = tmpStartupFile;
   
   // Check first to see if the user has set a log file on the command line -
   // that would take prcedence over a log file specified in the startup
   // file.  Also, check to see if the command-line-specified file exists and
   // if so, make sure it is a log file (i.e. starts with the correct text).
   // TBD
   
   // now use log file from the startup file if applicable
   std::string startupLog = GetAbsPathname("LOG_FILE");
   GmatGlobal *gmatGlobal = GmatGlobal::Instance();
   gmatGlobal->SetLogfileName(GmatGlobal::STARTUP, startupLog);
   
   Integer logSrc = gmatGlobal->GetLogfileSource();
   if (logSrc == GmatGlobal::CMD_LINE)
   {
      std::string cmdLineLog = gmatGlobal->GetLogfileName(GmatGlobal::CMD_LINE);
      MessageInterface::SetLogFile(cmdLineLog);
   }
   else // can't be SCRIPT yet since startup is read before scripts are parsed
   {
      MessageInterface::SetLogFile(startupLog);
   }
   MessageInterface::SetLogEnable(true);
   mInStream.close();

   /// @todo This code replaces relative paths with absolute.  It was implemented to 
   /// address an issue in R2014a, but the side effects were to severe for 
   /// the release.  It is commented out so that post release, we can asses how
   /// to proceed addressing path issues in GMAT.
   // SetPathsAbsolute();
   
   // Validate PATHs
   ValidatePaths();

   #ifdef DEBUG_MAPPING
   ShowMaps("In ReadStartupFile()");
   #endif
}

//void FileManager::WriteStartupFile(const char *fileName)
//{
//   WriteStartupFile(std::string(fileName));
//}

//------------------------------------------------------------------------------
// void WriteStartupFile(const std::string &fileName = "")
//------------------------------------------------------------------------------
/**
 * Reads GMAT startup file.
 *
 * @param <fileName> startup file name.
 *
 * @exception UtilityException thrown if file cannot be opened
 */
//------------------------------------------------------------------------------
void FileManager::WriteStartupFile(const std::string &fileName)
{
   std::string outFileName = "gmat_startup_file.new.txt";
   mPathWrittenOuts.clear();
   mFileWrittenOuts.clear();
   
   if (fileName != "")
      outFileName = fileName;
   
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage
      ("FileManager::WriteStartupFile() entered, outFileName = %s\n",
       outFileName.c_str());
   #endif

   std::ofstream outStream(outFileName.c_str());

   if (!outStream)
      throw UtilityException
         ("FileManager::WriteStartupFile() cannot open:" + fileName);

   //---------------------------------------------
   // write header
   //---------------------------------------------
   WriteHeader(outStream);
   
   // set left justified
   outStream.setf(std::ios::left);
   
   // don't write CURRENT_PATH
   mPathWrittenOuts.push_back("CURRENT_PATH");
   
   //---------------------------------------------
   // write RUN_MODE if not blank
   //---------------------------------------------
   if (mRunMode != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing RUN_MODE\n");
      #endif
      outStream << std::setw(22) << "RUN_MODE" << " = " << mRunMode << "\n";
   }
   
   // Write other option as comments
   outStream << std::setw(22) << "#RUN_MODE" << " = TESTING\n";
   outStream << std::setw(22) << "#RUN_MODE" << " = TESTING_NO_PLOTS\n";
   outStream << std::setw(22) << "#RUN_MODE" << " = EXIT_AFTER_RUN\n";
   
   //---------------------------------------------
   // write PLOT_MODE if not blank
   //---------------------------------------------
   if (mPlotMode != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing PLOT_MODE\n");
      #endif
      outStream << std::setw(22) << "PLOT_MODE" << " = " << mRunMode << "\n";
   }
   
   // Write other option as comments
   // There are no other options implemented for now
   //outStream << std::setw(22) << "#PLOT_MODE" << " = TILE\n";
   
   //---------------------------------------------
   // write MATLAB_MODE if not blank
   //---------------------------------------------
   if (mMatlabMode != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing MATLAB_MODE\n");
      #endif
      outStream << std::setw(22) << "MATLAB_MODE" << " = " << mMatlabMode << "\n";
   }
   
   // Write other option as comments
   outStream << std::setw(22) << "#MATLAB_MODE" << " = SINGLE\n";
   outStream << std::setw(22) << "#MATLAB_MODE" << " = SHARED\n";
   outStream << std::setw(22) << "#MATLAB_MODE" << " = NO_MATLAB\n";
   
   //---------------------------------------------
   // write DEBUG_MATLAB if not blank
   //---------------------------------------------
   if (mDebugMatlab != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing DEBUG_MATLAB\n");
      #endif
      outStream << std::setw(22) << "DEBUG_MATLAB" << " = " << mDebugMatlab << "\n";
   }
      
   //---------------------------------------------
   // write DEBUG_MISSION_TREE if not blank
   //---------------------------------------------
   if (mDebugMissionTree != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing DEBUG_MISSION_TREE\n");
      #endif
      outStream << std::setw(22) << "DEBUG_MISSION_TREE" << " = " << mDebugMissionTree << "\n";
   }
   
   if (mRunMode != "" || mPlotMode != "" || mMatlabMode != "" ||
       mDebugMatlab != "" || mDebugMissionTree != "")
      outStream << "#-----------------------------------------------------------\n";
   
   //---------------------------------------------
   // write DEBUG_PARAMETERS if not blank
   //---------------------------------------------
   if (mWriteParameterInfo != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing PARAMETER_INFO\n");
      #endif
      outStream << std::setw(22) << "DEBUG_PARAMETERS" << " = " << mWriteParameterInfo << "\n";
   }
   
   if (mRunMode != "" || mPlotMode != "" || mMatlabMode != "" ||
       mDebugMatlab != "" || mDebugMissionTree != "" || mWriteParameterInfo != "")
      outStream << "#-----------------------------------------------------------\n";
   
   //---------------------------------------------
   // write DEBUG_FILE_PATH if not blank
   //---------------------------------------------
   if (mWriteFilePathInfo != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing FILE_PATH_INFO\n");
      #endif
      outStream << std::setw(22) << "DEBUG_FILE_PATH" << " = " << mWriteFilePathInfo << "\n";
   }
   
   if (mRunMode != "" || mPlotMode != "" || mMatlabMode != "" ||
       mDebugMatlab != "" || mDebugMissionTree != "" || mWriteParameterInfo != "" ||
       mWriteFilePathInfo != "")
      outStream << "#-----------------------------------------------------------\n";
   
   //---------------------------------------------
   // write WRITE_GMAT_KEYWORD if not blank
   //---------------------------------------------
   if (mWriteGmatKeyword != "")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing GMAT_KEYWORD_INFO\n");
      #endif
      outStream << std::setw(22) << "WRITE_GMAT_KEYWORD" << " = " << mWriteGmatKeyword << "\n";
   }
   
   if (mRunMode != "" || mPlotMode != "" || mMatlabMode != "" ||
       mDebugMatlab != "" || mDebugMissionTree != "" || mWriteParameterInfo != "" ||
       mWriteFilePathInfo != "" || mWriteGmatKeyword != "" )
      outStream << "#-----------------------------------------------------------\n";
   
   //---------------------------------------------
   // write ROOT_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing ROOT_PATH path\n");
   #endif
   outStream << std::setw(22) << "ROOT_PATH" << " = " << mPathMap["ROOT_PATH"]
             << "\n";
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("ROOT_PATH");
   
   //---------------------------------------------
   // write PLUGIN next 
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing PLUGIN\n");
   #endif
   if (mPluginList.size() > 0)
   {
      for (UnsignedInt i = 0; i < mPluginList.size(); ++i)
      {
         outStream << std::setw(22) << "PLUGIN" << " = " << mPluginList[i]
                   << "\n";
      }
      outStream << "#-----------------------------------------------------------\n";
   }
   
   //---------------------------------------------
   // write OUTPUT_PATH and output files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing OUTPUT_PATH paths\n");
   #endif
   outStream << std::setw(22) << "OUTPUT_PATH" << " = "
             << mPathMap["OUTPUT_PATH"] << "\n";
   WriteFiles(outStream, "LOG_");
   WriteFiles(outStream, "REPORT_");
   WriteFiles(outStream, "SCREENSHOT_");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("OUTPUT_PATH");
   
   //---------------------------------------------
   // write MEASUREMENT_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing MEASUREMENT_PATH paths\n");
   #endif
   outStream << std::setw(22) << "MEASUREMENT_PATH" << " = "
             << mPathMap["MEASUREMENT_PATH"] << "\n";
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("MEASUREMENT_PATH");
   
   //---------------------------------------------
   // write the VEHICLE_EPHEM_CCSDS_PATH next if set
   //---------------------------------------------
   if (mPathMap["VEHICLE_EPHEM_CCSDS_PATH"] != "./output/")
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing VEHICLE_EPHEM_CCSDS_PATH path\n");
      #endif
      outStream << std::setw(22) << "VEHICLE_EPHEM_CCSDS_PATH" << " = "
                << mPathMap["VEHICLE_EPHEM_CCSDS_PATH"];
      outStream << "\n#---------------------------------------------"
            "--------------\n";
      mPathWrittenOuts.push_back("VEHICLE_EPHEM_CCSDS_PATH");
   }
   
   //---------------------------------------------
   // write GMAT_INCLUDE_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing GMAT_INCLUDE_PATH paths\n");
   #endif
   bool isEmptyPath = true;
   for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
        pos != mPathMap.end(); ++pos)
   {
      if (pos->first == "GMAT_INCLUDE_PATH")
      {
         // Write all GmatInclude paths
         std::list<std::string>::iterator listpos = mGmatIncludePaths.begin();
         while (listpos != mGmatIncludePaths.end())
         {
            outStream << std::setw(22) << pos->first << " = "
                      << *listpos << "\n";
            ++listpos;
         }
         isEmptyPath = false;
         break;
      }
   }
   if (isEmptyPath)
      outStream << std::setw(22) << "#GMAT_INCLUDE_PATH " << " = " << "\n";
   
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("GMAT_INCLUDE_PATH");
   
   //---------------------------------------------
   // write GMAT_FUNCTION_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing GMAT_FUNCTION_PATH paths\n");
   #endif
   isEmptyPath = true;
   for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
        pos != mPathMap.end(); ++pos)
   {
      if (pos->first == "GMAT_FUNCTION_PATH")
      {
         // Write all GmatFunction paths
         std::list<std::string>::iterator listpos = mGmatFunctionPaths.begin();
         while (listpos != mGmatFunctionPaths.end())
         {
            outStream << std::setw(22) << pos->first << " = "
                      << *listpos << "\n";
            ++listpos;
         }
         isEmptyPath = false;
         break;
      }
   }
   if (isEmptyPath)
      outStream << std::setw(22) << "#GMAT_FUNCTION_PATH " << " = " << "\n";
   
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("GMAT_FUNCTION_PATH");
   
   //---------------------------------------------
   // write MATLAB_FUNCTION_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing MATLAB_FUNCTION_PATH paths\n");
   #endif
   isEmptyPath = true;
   for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
        pos != mPathMap.end(); ++pos)
   {
      if (pos->first == "MATLAB_FUNCTION_PATH")
      {
         // Write all GmatFunction paths
         std::list<std::string>::iterator listpos = mMatlabFunctionPaths.begin();
         while (listpos != mMatlabFunctionPaths.end())
         {
            outStream << std::setw(22) << pos->first << " = " << *listpos << "\n";
            ++listpos;
         }
         break;
      }
   }
   if (isEmptyPath)
      outStream << std::setw(22) << "#MATLAB_FUNCTION_PATH " << " = " << "\n";
   
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("MATLAB_FUNCTION_PATH");
   
   //---------------------------------------------
   // write DATA_PATH next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing DATA_PATH path\n");
   #endif
   outStream << std::setw(22) << "DATA_PATH" << " = " << mPathMap["DATA_PATH"]
             << "\n";
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("DATA_PATH");
   
   //---------------------------------------------
   // write any relative path used in PLANETARY_EPHEM_SPK_PATH
   //---------------------------------------------
   std::string spkPath = mPathMap["PLANETARY_EPHEM_SPK_PATH"];
   if (spkPath.find("_PATH") != spkPath.npos)
   {
      std::string relPath = GmatFileUtil::ParseFirstPathName(spkPath, false);
      if (find(mPathWrittenOuts.begin(), mPathWrittenOuts.end(), relPath) ==
          mPathWrittenOuts.end())
      {
         #ifdef DEBUG_WRITE_STARTUP_FILE
         MessageInterface::ShowMessage("   .....Writing %s\n", relPath.c_str());
         #endif
         outStream << std::setw(22) << relPath << " = " << mPathMap[relPath] << "\n";
         outStream << "#-----------------------------------------------------------\n";
         mPathWrittenOuts.push_back(relPath);
      }
   }
   
   //---------------------------------------------
   // write the PLANETARY_EPHEM_SPK_PATH and SPK file next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing SPK path\n");
   #endif
   outStream << std::setw(22) << "PLANETARY_EPHEM_SPK_PATH" << " = "
             << mPathMap["PLANETARY_EPHEM_SPK_PATH"] << "\n";
   WriteFiles(outStream, "PLANETARY SPK");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("PLANETARY_EPHEM_SPK_PATH");
   
   //---------------------------------------------
   // write the PLANETARY_EPHEM_DE_PATH and DE file next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing DE path\n");
   #endif
   outStream << std::setw(22) << "PLANETARY_EPHEM_DE_PATH" << " = "
             << mPathMap["PLANETARY_EPHEM_DE_PATH"] << "\n";
   WriteFiles(outStream, "DE405");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("PLANETARY_EPHEM_DE_PATH");
   
   //---------------------------------------------
   // write the PLANETARY_COEFF_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing PLANETARY_COEFF_PATH path\n");
   #endif
   outStream << std::setw(22) << "PLANETARY_COEFF_PATH" << " = "
             << mPathMap["PLANETARY_COEFF_PATH"] << "\n";
   WriteFiles(outStream, "EOP_FILE");
   WriteFiles(outStream, "PLANETARY_COEFF_FILE");
   WriteFiles(outStream, "NUTATION_COEFF_FILE");
   WriteFiles(outStream, "PLANETARY_PCK_FILE");
   WriteFiles(outStream, "EARTH_LATEST_PCK_FILE");
   WriteFiles(outStream, "EARTH_PCK_PREDICTED_FILE");
   WriteFiles(outStream, "EARTH_PCK_CURRENT_FILE");
   WriteFiles(outStream, "LUNA_PCK_CURRENT_FILE");
   WriteFiles(outStream, "LUNA_FRAME_KERNEL_FILE");
   
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("PLANETARY_COEFF_PATH");

   //---------------------------------------------
   // write the TIME_PATH and TIME file next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing TIME path\n");
   #endif
   outStream << std::setw(22) << "TIME_PATH" << " = " << mPathMap["TIME_PATH"] << "\n";
   WriteFiles(outStream, "LEAP_");
   WriteFiles(outStream, "LSK_");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("TIME_PATH");

   //---------------------------------------------
   // write the ATMOSPHERE_PATH and CSSI FLUX file next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing ATMOSPHERE path\n");
   #endif
   outStream << std::setw(22) << "ATMOSPHERE_PATH" << " = " << mPathMap["ATMOSPHERE_PATH"] << "\n";
   WriteFiles(outStream, "CSSI_FLUX_");
   WriteFiles(outStream, "SCHATTEN_");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("ATMOSPHERE_PATH");

   //---------------------------------------------
   // write *_POT_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing *_POT_PATH paths\n");
   #endif
   for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
        pos != mPathMap.end(); ++pos)
   {
      if (pos->first.find("_POT_") != std::string::npos)
      {
         outStream << std::setw(22) << pos->first << " = "
                   << pos->second << "\n";
         mPathWrittenOuts.push_back(pos->first);
      }
   }
   outStream << "#-----------------------------------------------------------\n";
   WriteFiles(outStream, "POT_FILE");
   WriteFiles(outStream, "EGM96");
   WriteFiles(outStream, "JGM");
   WriteFiles(outStream, "MARS50C");
   WriteFiles(outStream, "MGNP180U");
   WriteFiles(outStream, "LP165P");
   
   outStream << "#-----------------------------------------------------------\n";

   //---------------------------------------------
   // write the GUI_CONFIG_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing GUI_CONFIG_PATH path\n");
   #endif
   outStream << std::setw(22) << "GUI_CONFIG_PATH" << " = "
             << mPathMap["GUI_CONFIG_PATH"] << "\n";
   WriteFiles(outStream, "PERSONALIZATION_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("GUI_CONFIG_PATH");
   
   //---------------------------------------------
   // write the ICON_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing ICON_PATH path\n");
   #endif
   outStream << std::setw(22) << "ICON_PATH" << " = "
             << mPathMap["ICON_PATH"] << "\n";
   WriteFiles(outStream, "ICON_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("ICON_PATH");

   //---------------------------------------------
   // write the SPLASH_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing SPLASH_PATH path\n");
   #endif
   outStream << std::setw(22) << "SPLASH_PATH" << " = "
             << mPathMap["SPLASH_PATH"] << "\n";
   WriteFiles(outStream, "SPLASH_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("SPLASH_PATH");
   
   //---------------------------------------------
   // write the TEXTURE_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing TEXTURE_PATH path\n");
   #endif
   outStream << std::setw(22) << "TEXTURE_PATH" << " = "
             << mPathMap["TEXTURE_PATH"] << "\n";
   WriteFiles(outStream, "TEXTURE_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("TEXTURE_PATH");

   //---------------------------------------------
   // write the STAR_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing STAR_PATH path\n");
   #endif
   outStream << std::setw(22) << "STAR_PATH" << " = "
             << mPathMap["STAR_PATH"] << "\n";
   WriteFiles(outStream, "STAR_FILE");
   WriteFiles(outStream, "CONSTELLATION_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("STAR_PATH");

   //---------------------------------------------
   // write the VEHICLE_EPHEM_SPK_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing VEHICLE_EPHEM_SPK_PATH path\n");
   #endif
   outStream << std::setw(22) << "VEHICLE_EPHEM_SPK_PATH" << " = "
             << mPathMap["VEHICLE_EPHEM_SPK_PATH"] << "\n";
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("VEHICLE_EPHEM_SPK_PATH");
   
   //---------------------------------------------
   // write the VEHICLE_MODEL_PATH and files next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing VEHICLE_MODEL_PATH path\n");
   #endif
   outStream << std::setw(22) << "VEHICLE_MODEL_PATH" << " = "
             << mPathMap["VEHICLE_MODEL_PATH"] << "\n";
   WriteFiles(outStream, "SPACECRAFT_MODEL_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mPathWrittenOuts.push_back("VEHICLE_MODEL_PATH");
   
   //---------------------------------------------
   // write the HELP_FILE next
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing HELP_FILE\n");
   #endif
   
   if (GetFilename("HELP_FILE") == "")
      outStream << std::setw(22) << "#HELP_FILE " << " = " << "\n";
   else
      WriteFiles(outStream, "HELP_FILE");
   outStream << "#-----------------------------------------------------------\n";
   mFileWrittenOuts.push_back("HELP_FILE");
   
   //---------------------------------------------
   // write rest of paths and files
   //---------------------------------------------
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing rest of paths and files\n");
   #endif
   WriteFiles(outStream, "-OTHER-PATH-");
   WriteFiles(outStream, "-OTHER-");
   outStream << "#-----------------------------------------------------------\n";
   
   //---------------------------------------------
   // write saved comments
   //---------------------------------------------
   if (!mSavedComments.empty())
   {
      #ifdef DEBUG_WRITE_STARTUP_FILE
      MessageInterface::ShowMessage("   .....Writing saved comments\n");
      #endif
      outStream << "# Saved Comments\n";
      outStream << "#-----------------------------------------------------------\n";
      for (UnsignedInt i=0; i<mSavedComments.size(); i++)
         outStream << mSavedComments[i] << "\n";
      outStream << "#-----------------------------------------------------------\n";
   }

   outStream << "\n";
   outStream.close();

   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("FileManager::WriteStartupFile() exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// std::string GetRootPath()
//------------------------------------------------------------------------------
/**
 * Retrives root pathname.
 *
 * @return file pathname if path type found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetRootPath()
{
   return mPathMap["ROOT_PATH"];
}


//------------------------------------------------------------------------------
// bool GetTextureMapFile(const std::string &inFileName, const std::string &bodyName,
//                        const std::string &objName, std::string &outFileName,
//                        std::string &outFullPathName, bool writeWarning)
//------------------------------------------------------------------------------
bool FileManager::GetTextureMapFile(const std::string &inFileName, const std::string &bodyName,
                                    const std::string &objName, std::string &outFileName,
                                    std::string &outFullPathName, bool writeWarning)
{
   #ifdef DEBUG_TEXTURE_FILE
   MessageInterface::ShowMessage
      ("\nFileManager::GetTextureMapFile() entered\n   inFileName = '%s'\n   "
       "bodyName = '%s', objName = '%s', writeWarning = %d\n", inFileName.c_str(),
       bodyName.c_str(), objName.c_str(), writeWarning);
   #endif
   
   bool retval = true;
   std::string actualFile = inFileName;
   std::string fullPath;
   std::string mapFileType = GmatStringUtil::ToUpper(bodyName) + "_TEXTURE_FILE";
   mLastFilePathMessage = "";
   bool writeInfo = false;
   
   outFileName = inFileName;
   
   #ifdef DEBUG_TEXTURE_FILE
   MessageInterface::ShowMessage("   actualFile  = '%s'\n", actualFile.c_str());
   MessageInterface::ShowMessage("   mapFileType = '%s'\n", mapFileType.c_str());
   #endif
   
   try
   {
      if (inFileName == "")
         actualFile = GetFilename(mapFileType);
      
      fullPath =
         FindPath(actualFile, mapFileType, true, writeWarning, writeInfo, objName);
      
      #ifdef DEBUG_TEXTURE_FILE
      MessageInterface::ShowMessage("   actualFile = '%s'\n", actualFile.c_str());
      MessageInterface::ShowMessage("   fullPath   = '%s'\n", fullPath.c_str());
      #endif
      
      // If fullPath is blank, try with TEXTURE_PATH since non-standard bodies'
      // texture map file may not be available such as SOMECOMET1_TEXTURE_FILE
      if (fullPath == "")
      {
         fullPath = 
            FindPath(actualFile, "TEXTURE_PATH", true, false, writeInfo, objName);
      }
      
      if (fullPath == "")
      {
         if (inFileName == "")
         {
            mLastFilePathMessage = GetLastFilePathMessage() + ", so using " + actualFile + ".";
         }
         else
         {
            mLastFilePathMessage = GetLastFilePathMessage();
            retval = false;
         }
      }
      else if (inFileName == "")
      {
         outFileName = actualFile;
         std::string msg = "*** WARNING *** There is no texture map file "
            "specified for " + objName + ", so using " + actualFile;
         mLastFilePathMessage = msg;
         if (writeWarning)
            MessageInterface::ShowMessage(msg + "\n");
      }
      
      outFullPathName = fullPath;
   }
   catch (BaseException &be)
   {
      #ifdef DEBUG_TEXTURE_FILE
      MessageInterface::ShowMessage("%s\n", be.GetFullMessage().c_str());
      #endif
      if (inFileName == "")
      {
         actualFile = "GenericCelestialBody.jpg";
         std::string msg = "*** WARNING *** There is no texture map file "
            "specified for " + objName + ", so using " + actualFile;
         mLastFilePathMessage = msg;
         if (writeWarning)
            MessageInterface::ShowMessage(msg + "\n");
         fullPath = 
            FileManager::Instance()->FindPath(actualFile, "TEXTURE_PATH", true, false,
                                              writeInfo, objName);
         outFileName = actualFile;
         outFullPathName = fullPath;
      }
      else
      {
         outFullPathName = "";
         retval = false;
      }
   }
   
   #ifdef DEBUG_TEXTURE_FILE
   MessageInterface::ShowMessage
      ("   outFileName = '%s'\n   outFullPathName = '%s'\n   mLastFilePathMessage = '%s'\n",
       outFileName.c_str(), outFullPathName.c_str(), mLastFilePathMessage.c_str());
   MessageInterface::ShowMessage("FileManager::GetTextureMapFile() returnng %d\n\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// bool GetBody3dModelFile(const std::string &inFileName, const std::string &bodyName,
//                         const std::string &objName, std::string &outFileName,
//                         std::string &outFullPathName, bool writeWarning)
//------------------------------------------------------------------------------
bool FileManager::GetBody3dModelFile(const std::string &inFileName, const std::string &bodyName,
                                     const std::string &objName, std::string &outFileName,
                                     std::string &outFullPathName, bool writeWarning)
{
   #ifdef DEBUG_3DMODEL_FILE
   MessageInterface::ShowMessage
      ("\nFileManager::GetBody3dModelFile() entered\n   inFileName = '%s'\n   "
       "bodyName = '%s', objName = '%s', writeWarning = %d\n", inFileName.c_str(),
       bodyName.c_str(), objName.c_str(), writeWarning);
   #endif
   
   bool retval = true;
   std::string actualFile = inFileName;
   std::string fullPath;
   std::string modelFileType = GmatStringUtil::ToUpper(bodyName) + "_3D_MODEL_FILE";
   mLastFilePathMessage = "";
   bool writeInfo = false;
   
   outFileName = inFileName;
   
   #ifdef DEBUG_3DMODEL_FILE
   MessageInterface::ShowMessage("   actualFile  = '%s'\n", actualFile.c_str());
   MessageInterface::ShowMessage("   modelFileType = '%s'\n", modelFileType.c_str());
   #endif
   
   try
   {
      if (inFileName == "")
         actualFile = GetFilename(modelFileType);
      
      fullPath =
         FindPath(actualFile, modelFileType, true, writeWarning, writeInfo, objName);
      
      #ifdef DEBUG_3DMODEL_FILE
      MessageInterface::ShowMessage("   actualFile = '%s'\n", actualFile.c_str());
      MessageInterface::ShowMessage("   fullPath   = '%s'\n", fullPath.c_str());
      #endif
      
      // If fullPath is blank, try with BODY_3D_MODEL_PATH since non-standard bodies'
      // 3d file may not be available such as SOMECOMET1_3D_MODEL_FILE
      if (fullPath == "")
      {
         fullPath = 
            FindPath(actualFile, "BODY_3D_MODEL_PATH", true, false, writeInfo, objName);
      }
      
      if (fullPath == "")
      {
         if (inFileName == "")
         {
            mLastFilePathMessage = GetLastFilePathMessage() + ", so using " + actualFile + ".";
         }
         else
         {
            mLastFilePathMessage = GetLastFilePathMessage();
            retval = false;
         }
      }
      else if (inFileName == "")
      {
         outFileName = actualFile;
         std::string msg = "*** WARNING *** There is no 3D model file "
            "specified for " + objName + ", so using " + actualFile;
         mLastFilePathMessage = msg;
         if (writeWarning)
            MessageInterface::ShowMessage(msg + "\n");
      }
      
      outFullPathName = fullPath;
   }
   catch (BaseException &be)
   {
      #ifdef DEBUG_3DMODEL_FILE
      MessageInterface::ShowMessage("%s\n", be.GetFullMessage().c_str());
      #endif
      if (inFileName == "")
      {
         actualFile = "";
         std::string msg = "*** WARNING *** There is no 3d model file "
            "specified for " + objName + ", so using " + actualFile;
         mLastFilePathMessage = msg;
         if (writeWarning)
            MessageInterface::ShowMessage(msg + "\n");
         fullPath = 
            FileManager::Instance()->FindPath(actualFile, "BODY_3D_MODEL_PATH", true, false,
                                              writeInfo, objName);
         outFileName = actualFile;
         outFullPathName = fullPath;
      }
      else
      {
         outFullPathName = "";
         retval = false;
      }
   }
   
   #ifdef DEBUG_3DMODEL_FILE
   MessageInterface::ShowMessage
      ("   outFileName = '%s'\n   outFullPathName = '%s'\n   mLastFilePathMessage = '%s'\n",
       outFileName.c_str(), outFullPathName.c_str(), mLastFilePathMessage.c_str());
   MessageInterface::ShowMessage("FileManager::GetBody3dModelFile() returnng %d\n\n", retval);
   #endif
   return retval;
}


//------------------------------------------------------------------------------
// std::string GetPathname(const FileType type)
//------------------------------------------------------------------------------
/**
 * Retrives absolute path for the type without filename.
 *
 * @param <type> enum file type of which path to be returned.
 *
 * @return file pathname if path type found.
 * @exception thrown if enum type is out of bounds.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetPathname(const FileType type)
{
   if (type >=0 && type < FileTypeCount)
      return GetPathname(FILE_TYPE_STRING[type]);
   
   std::stringstream ss("");
   ss << "FileManager::GetPathname() enum type: " << type
      << " is out of bounds\n";
   
   throw UtilityException(ss.str());
}


//------------------------------------------------------------------------------
// std::string GetPathname(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Retrives absolute pathname for the type name without filename.
 *
 * @param <typeName> file type name of which pathname to be returned.
 *
 * @return pathname if type found.
 * @exception thrown if type cannot be found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetPathname(const std::string &typeName)
{
   std::string fileType = GmatStringUtil::ToUpper(typeName);
   
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage
      ("FileManager::GetPathname() entered, flleType='%s'\n", fileType.c_str());
   #endif

   std::string pathname;
   bool nameFound = false;
   
   // if typeName contains _PATH
   if (fileType.find("_PATH") != fileType.npos)
   {
      if (mPathMap.find(fileType) != mPathMap.end())
      {
         pathname = mPathMap[fileType];
         nameFound = true;
      }
   }
   else
   {
      // typeName contains _FILE      
      if (mFileMap.find(fileType) != mFileMap.end())
      {
         pathname = mFileMap[fileType]->mPath;
         nameFound = true;
      }
   }
   
   if (nameFound)
   {
      #ifdef DEBUG_FILE_PATH
      MessageInterface::ShowMessage("   pathname = '%s'\n", pathname.c_str());
      #endif
      
      // Replace relative path with absolute path
      std::string abspath = ConvertToAbsPath(pathname);
      
      #ifdef DEBUG_FILE_PATH
      MessageInterface::ShowMessage
         ("FileManager::GetPathname() returning '%s'\n", abspath.c_str());
      #endif
      
      return abspath;
   }
   else
   {
      throw UtilityException("FileManager::GetPathname() file type: " + typeName +
                             " is unknown\n");
   }
}


//------------------------------------------------------------------------------
// std::string GetFilename(const FileType type)
//------------------------------------------------------------------------------
/**
 * Retrives filename for the type without path.
 *
 * @param <type> enum file type of which filename to be returned.
 *
 * @return file filename if file type found
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFilename(const FileType type)
{
   bool nameFound = false;
   std::string name;
   if (type >=0 && type < FileTypeCount)
   {
      name = GetFilename(FILE_TYPE_STRING[type]);
      nameFound = true;
   }
   
   if (nameFound)
   {
      name = GmatFileUtil::ParseFileName(name);
      return name;
   }
   
   std::stringstream ss("");
   ss << "FileManager::GetFilename() enum type: " << type
      << " is out of bounds\n";

   throw UtilityException(ss.str());
}


//------------------------------------------------------------------------------
// std::string GetFilename(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Retrives filename for the type name without path.
 *
 * @param <type> file type name of which filename to be returned.
 *
 * @return file filename if file type found
 * @exception thrown if type cannot be found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFilename(const std::string &typeName)
{
   #ifdef DEBUG_GET_FILENAME
   MessageInterface::ShowMessage
      ("FileManager::GetFilename() entered, typeName = '%s'\n", typeName.c_str());
   #endif
   
   bool nameFound = false;
   std::string name;
   if (mFileMap.find(typeName) != mFileMap.end())
   {
      name = mFileMap[typeName]->mFile;
      nameFound = true;
   }
   
   if (nameFound)
   {
      name = GmatFileUtil::ParseFileName(name);
      #ifdef DEBUG_GET_FILENAME
      MessageInterface::ShowMessage
         ("FileManager::GetFilename() returning '%s'\n", name.c_str());
      #endif
      return name;
   }
   
   throw UtilityException("FileManager::GetFilename() file type: " + typeName +
                           " is unknown\n");
}


//------------------------------------------------------------------------------
// std::string GetFullPathname(const FileType type)
//------------------------------------------------------------------------------
/**
 * Retrieves full pathname for the type.
 *
 * @param <type> file type of which filename to be returned.
 *
 * @return file pathname if file type found
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFullPathname(const FileType type)
{
   return GetAbsPathname(type);
}


//------------------------------------------------------------------------------
// std::string GetFullPathname(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Retrives full pathname for the type name.
 *
 * @param <type> file type name of which filename to be returned.
 *
 * @return file pathname if file type name found
 * @exception thrown if type cannot be found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetFullPathname(const std::string &typeName)
{
   return GetAbsPathname(typeName);
}


//------------------------------------------------------------------------------
// std::string GetAbsPathname(const FileType type)
//------------------------------------------------------------------------------
/**
 * Retrieves full pathname for the type.
 *
 * @param <type> file type of which filename to be returned.
 *
 * @return file pathname if file type found
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
std::string FileManager::GetAbsPathname(const FileType type)
{

   if (type >=0 && type < FileTypeCount)
      return GetAbsPathname(FILE_TYPE_STRING[type]);

   std::stringstream ss("");
   ss << "FileManager::GetAbsPathname() enum type: " << type <<
      " is out of bounds\n";

   throw UtilityException(ss.str());
}


//------------------------------------------------------------------------------
// std::string GetAbsPathname(const std::string &typeName)
//------------------------------------------------------------------------------
/**
 * Retrieves full pathname for the type name.
 *
 * @param <type> file type name of which filename to be returned.
 *
 * @return file pathname if file type name found
 * @exception thrown if type cannot be found.
 */
//------------------------------------------------------------------------------
std::string FileManager::GetAbsPathname(const std::string &typeName)
{
   std::string fileType = GmatStringUtil::ToUpper(typeName);
   std::string absPath;

   #ifdef DEBUG_ABS_PATH
   MessageInterface::ShowMessage
      ("FileManager::GetAbsPathname() typeName='%s', fileType='%s'\n",
       typeName.c_str(), fileType.c_str());
   #endif
   
   // typeName contains _PATH
   if (fileType.find("_PATH") != fileType.npos)
   {
      if (mPathMap.find(fileType) != mPathMap.end())
      {
         absPath = ConvertToAbsPath(fileType);
                  
         #ifdef DEBUG_ABS_PATH
         MessageInterface::ShowMessage
            ("FileManager::GetAbsPathname() with _PATH returning '%s'\n", absPath.c_str());
         #endif
         return absPath;
      }
   }
   else
   {
      if (mFileMap.find(fileType) != mFileMap.end())
      {
         std::string path = GetPathname(fileType);
         absPath = path + mFileMap[fileType]->mFile;
      }
      else if (mFileMap.find(fileType + "_ABS") != mFileMap.end())
      {
         absPath = mFileMap[typeName]->mFile;
      }

      #ifdef DEBUG_ABS_PATH
      MessageInterface::ShowMessage
         ("FileManager::GetAbsPathname() without _PATH returning '%s'\n", absPath.c_str());
      #endif
      return absPath;
   }

   throw UtilityException
      (GmatStringUtil::ToUpper(typeName) + " not in the gmat_startup_file\n");

}


//------------------------------------------------------------------------------
// std::string ConvertToAbsPath(const std::string &relPath, bool appendPathSep = true)
//------------------------------------------------------------------------------
/**
 * Converts relative path to absolute path
 */
//------------------------------------------------------------------------------
std::string FileManager::ConvertToAbsPath(const std::string &relPath, bool appendPathSep)
{
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage
      ("FileManager::ConvertToAbsPath() relPath='%s'\n", relPath.c_str());
   #endif
   
   //std::string absPath = relPath;
   std::string absPath;
   bool        startsWithSeparator = false;
   if ((relPath[0] == '\\') || (relPath[0] == '/'))
      startsWithSeparator = true;
   StringTokenizer st(relPath, "/\\");
   StringArray allNames = st.GetAllTokens();
   StringArray pathNames;
   
   #ifdef DEBUG_FILE_PATH
   Integer numNames = allNames.size();
   MessageInterface::ShowMessage("There are %d names in relPath\n", numNames);
   for (int i = 0; i < numNames; i++)
      MessageInterface::ShowMessage("   allNames[%d] = '%s'\n", i, allNames[i].c_str());
   #endif
   
   for (UnsignedInt i = 0; i < allNames.size(); i++)
   {
      std::string name = allNames[i];
      
      #ifdef DEBUG_FILE_PATH
      MessageInterface::ShowMessage("   allNames[%d] = '%s'\n", i, name.c_str());
      #endif
      
      absPath = name;
      if (GmatStringUtil::EndsWith(name, "_PATH"))
      {
         #ifdef DEBUG_FILE_PATH
         MessageInterface::ShowMessage("   _PATH found\n");
         #endif
         
         if (mPathMap.find(name) != mPathMap.end())
            absPath = mPathMap[name];
         
         #ifdef DEBUG_FILE_PATH
         MessageInterface::ShowMessage("   1st absPath = '%s'\n", absPath.c_str());
         #endif
         
         // If _PATH found and it is not the same as original name,
         // Call ConvertToAbsPath() again
         if (absPath.find("_PATH") != absPath.npos && absPath != name)
            absPath = ConvertToAbsPath(absPath);
         
         #ifdef DEBUG_FILE_PATH
         MessageInterface::ShowMessage("   2nd absPath = '%s'\n", absPath.c_str());
         #endif
         
         pathNames.push_back(absPath);
      }
      else
      {
         #ifdef DEBUG_FILE_PATH
         MessageInterface::ShowMessage("   _PATH not found\n");
         MessageInterface::ShowMessage("   absPath = '%s'\n", absPath.c_str());
         #endif
         
         pathNames.push_back(absPath);
      }
   }
   
   absPath = "";
   // For paths that already started with the separator (were already absolute paths)
   if (startsWithSeparator)
      absPath += mPathSeparator;
   for (UnsignedInt i = 0; i < pathNames.size(); i++)
   {
      if (i < pathNames.size() - 1)
      {
         if (GmatStringUtil::EndsWithPathSeparator(pathNames[i]))
            absPath = absPath + pathNames[i];
         else
            absPath = absPath + pathNames[i] + "/";
      }
      else
      {
         if (GmatStringUtil::EndsWithPathSeparator(pathNames[i]))
            absPath = absPath + pathNames[i];
         else
         {
            if (appendPathSep)
               absPath = absPath + pathNames[i] + "/";
            else
               absPath = absPath + pathNames[i];
         }
      }
   }
     
   // Convert path to absolute by prepending bin dir (LOJ: 2014.06.18)
   if (absPath != "" && absPath[0] == '.')
      absPath = mAbsBinDir + absPath;
   
   // Conver to OS path name
   absPath = GmatFileUtil::ConvertToOsFileName(absPath);
   
   #ifdef DEBUG_FILE_PATH
   MessageInterface::ShowMessage
      ("FileManager::ConvertToAbsPath() returning '%s'\n", absPath.c_str());
   #endif
   
   return absPath;
}

//------------------------------------------------------------------------------
// void SetAbsPathname(const FileType type, const char *newpath)
//------------------------------------------------------------------------------
/**
 * Sets absoulute pathname for the type.
 *
 * @param <type> file type of which path to be set.
 * @param <newpath> new pathname.
 *
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
void FileManager::SetAbsPathname(const FileType type, const char *newpath)
{
   SetAbsPathname(type, std::string(newpath));
}


//------------------------------------------------------------------------------
// void SetAbsPathname(const FileType type, const std::string &newpath)
//------------------------------------------------------------------------------
/**
 * Sets absoulute pathname for the type.
 *
 * @param <type> file type of which path to be set.
 * @param <newpath> new pathname.
 *
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
void FileManager::SetAbsPathname(const FileType type, const std::string &newpath)
{
   if (type >= BEGIN_OF_PATH && type <= END_OF_PATH)
   {
      SetAbsPathname(FILE_TYPE_STRING[type], newpath);
   }
   else
   {
      std::stringstream ss("");
      ss << "FileManager::SetAbsPathname() enum type: " << type <<
         " is out of bounds of file path\n";

      throw UtilityException(ss.str());
   }
}

//------------------------------------------------------------------------------
// void SetAbsPathname(const std::string &type, const char *newpath)
//------------------------------------------------------------------------------
/**
 * Sets absolute pathname for the type.
 *
 * @param <type> type name of which path to be set.
 * @param <newpath> new pathname.
 *
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
void FileManager::SetAbsPathname(const std::string &type, const char *newpath)
{
   SetAbsPathname(type, std::string(newpath));
}

//------------------------------------------------------------------------------
// void SetAbsPathname(const std::string &type, const std::string &newpath)
//------------------------------------------------------------------------------
/**
 * Sets absolute pathname for the type.
 *
 * @param <type> type name of which path to be set.
 * @param <newpath> new pathname.
 *
 * @exception thrown if enum type is out of bounds
 */
//------------------------------------------------------------------------------
void FileManager::SetAbsPathname(const std::string &type, const std::string &newpath)
{
   if (mPathMap.find(type) != mPathMap.end())
   {
      if (type.find("_PATH") != type.npos)
      {
         std::string str2 = newpath;

         // append '/' if not there
         std::string::size_type index = str2.find_last_of("/\\");
         if (index != str2.length() - 1)
         {
            str2 = str2 + mPathSeparator;
         }
         else
         {
            index = str2.find_last_not_of("/\\");
            str2 = str2.substr(0, index+1) + mPathSeparator;
         }

         mPathMap[type] = str2;

         #ifdef DEBUG_SET_PATH
         MessageInterface::ShowMessage
            ("FileManager::SetAbsPathname() %s set to %s\n", type.c_str(),
             str2.c_str());
         #endif
      }
      else
      {
         throw UtilityException
            ("FileManager::SetAbsPathname() type doesn't contain _PATH");
      }
   }
}


//------------------------------------------------------------------------------
// void ClearGmatIncludePath()
//------------------------------------------------------------------------------
void FileManager::ClearGmatIncludePath()
{
   mGmatIncludePaths.clear();
}


//------------------------------------------------------------------------------
// void AddGmatIncludePath(const char *path, bool addFront)
//------------------------------------------------------------------------------
void FileManager::AddGmatIncludePath(const char *path, bool addFront)
{
   return AddGmatIncludePath(std::string(path), addFront);
}


//------------------------------------------------------------------------------
// void  AddGmatIncludePath(const std::string &path, bool addFront, bool addFront)
//------------------------------------------------------------------------------
/*
 * If new path it adds to the GmatInclude path list.
 * If path already exist, it moves to the front or back of the list, depends on
 * addFront flag.
 *
 * @param  path  path name to be added
 * @param  addFront  if set to true, it adds to the front, else adds to the back (true)
 */
//------------------------------------------------------------------------------
void FileManager::AddGmatIncludePath(const std::string &path, bool addFront)
{
   #ifdef DEBUG_GMAT_PATH
   MessageInterface::ShowMessage
      ("FileManager::AddGmatIncludePath() Adding %s to GmatIncludePath\n   "
       "addFront=%d\n", path.c_str(), addFront);
   #endif

   std::string pathname = path;

   // if path has full pathname (directory and filename), remove filename first
   if (path.find(".") != path.npos)
      pathname = GmatFileUtil::ParsePathName(path);

   std::list<std::string>::iterator pos =
      find(mGmatIncludePaths.begin(), mGmatIncludePaths.end(), pathname);

   if (pos == mGmatIncludePaths.end())
   {
      #ifdef DEBUG_GMAT_PATH
      MessageInterface::ShowMessage
         ("   the pathname <%s> is new, so adding to %s\n", pathname.c_str(),
          addFront ? "front" : "back");
      #endif

      // if new pathname, add to front or back of the list
      if (addFront)
         mGmatIncludePaths.push_front(pathname);
      else
         mGmatIncludePaths.push_back(pathname);
   }
   else
   {
      // if existing pathname remove and add front or back of the list
      #ifdef DEBUG_GMAT_PATH
      MessageInterface::ShowMessage
         ("   the pathname <%s> already exists, so moving to %s\n", pathname.c_str(),
          addFront ? "front" : "back");
      #endif

      std::string oldPath = *pos;
      mGmatIncludePaths.erase(pos);
      if (addFront)
         mGmatIncludePaths.push_front(oldPath);
      else
         mGmatIncludePaths.push_back(oldPath);
   }

   #ifdef DEBUG_GMAT_PATH
   pos = mGmatIncludePaths.begin();
   while (pos != mGmatIncludePaths.end())
   {
      MessageInterface::ShowMessage
         ("------   mGmatIncludePaths = %s\n", (*pos).c_str());
      ++pos;
   }
   #endif
}


//------------------------------------------------------------------------------
// std::string GetGmatIncludePath(const char *incName)
//------------------------------------------------------------------------------
std::string FileManager::GetGmatIncludePath(const char *incName)
{
   return GetGmatPath(GMAT_INCLUDE, mGmatIncludePaths, std::string(incName));
}


//------------------------------------------------------------------------------
// std::string GetGmatIncludePath(const std::string &incName)
//------------------------------------------------------------------------------
/*
 * Returns the absolute path that has Include filename.
 * It searches in the most recently added path first which is at the top of
 * the list.
 *
 * @param   incName  Include filename to be located
 * @return  Path  Path that has Include filename
 */
//------------------------------------------------------------------------------
std::string FileManager::GetGmatIncludePath(const std::string &incName)
{
   return GetGmatPath(GMAT_INCLUDE, mGmatIncludePaths, incName);
}


//------------------------------------------------------------------------------
// const StringArray& GetAllGmatIncludePaths()
//------------------------------------------------------------------------------
const StringArray& FileManager::GetAllGmatIncludePaths()
{
   mGmatIncludeFullPaths.clear();

   std::list<std::string>::iterator listpos = mGmatIncludePaths.begin();
   while (listpos != mGmatIncludePaths.end())
   {
      mGmatIncludeFullPaths.push_back(ConvertToAbsPath(*listpos));
      ++listpos;
   }

   return mGmatIncludeFullPaths;
}


//------------------------------------------------------------------------------
// void ClearGmatFunctionPath()
//------------------------------------------------------------------------------
void FileManager::ClearGmatFunctionPath()
{
   mGmatFunctionPaths.clear();
}


//------------------------------------------------------------------------------
// void AddGmatFunctionPath(const char *path, bool addFront)
//------------------------------------------------------------------------------
void FileManager::AddGmatFunctionPath(const char *path, bool addFront)
{
   return AddGmatFunctionPath(std::string(path), addFront);
}


//------------------------------------------------------------------------------
// void  AddGmatFunctionPath(const std::string &path, bool addFront, bool addFront)
//------------------------------------------------------------------------------
/*
 * If new path it adds to the GmatFunction path list.
 * If path already exist, it moves to the front or back of the list, depends on
 * addFront flag.
 *
 * @param  path  path name to be added
 * @param  addFront  if set to true, it adds to the front, else adds to the back (true)
 */
//------------------------------------------------------------------------------
void FileManager::AddGmatFunctionPath(const std::string &path, bool addFront)
{
   #ifdef DEBUG_GMAT_PATH
   MessageInterface::ShowMessage
      ("FileManager::AddGmatFunctionPath() Adding %s to GmatFunctionPath\n   "
       "addFront=%d\n", path.c_str(), addFront);
   #endif

   std::string pathname = path;

   // if path has full pathname (directory and filename), remove filename first
   if (path.find(".") != path.npos)
      pathname = GmatFileUtil::ParsePathName(path);

   std::list<std::string>::iterator pos =
      find(mGmatFunctionPaths.begin(), mGmatFunctionPaths.end(), pathname);

   if (pos == mGmatFunctionPaths.end())
   {
      #ifdef DEBUG_GMAT_PATH
      MessageInterface::ShowMessage
         ("   the pathname <%s> is new, so adding to %s\n", pathname.c_str(),
          addFront ? "front" : "back");
      #endif

      // if new pathname, add to front or back of the list
      if (addFront)
         mGmatFunctionPaths.push_front(pathname);
      else
         mGmatFunctionPaths.push_back(pathname);
   }
   else
   {
      // if existing pathname remove and add front or back of the list
      #ifdef DEBUG_GMAT_PATH
      MessageInterface::ShowMessage
         ("   the pathname <%s> already exists, so moving to %s\n", pathname.c_str(),
          addFront ? "front" : "back");
      #endif

      std::string oldPath = *pos;
      mGmatFunctionPaths.erase(pos);
      if (addFront)
         mGmatFunctionPaths.push_front(oldPath);
      else
         mGmatFunctionPaths.push_back(oldPath);
   }

   #ifdef DEBUG_GMAT_PATH
   pos = mGmatFunctionPaths.begin();
   while (pos != mGmatFunctionPaths.end())
   {
      MessageInterface::ShowMessage
         ("------   mGmatFunctionPaths = %s\n", (*pos).c_str());
      ++pos;
   }
   #endif
}


//------------------------------------------------------------------------------
// std::string GetGmatFunctionPath(const char *funcName)
//------------------------------------------------------------------------------
std::string FileManager::GetGmatFunctionPath(const char *funcName)
{
   return GetGmatPath(GMAT_FUNCTION, mGmatFunctionPaths, std::string(funcName));
}


//------------------------------------------------------------------------------
// std::string GetGmatFunctionPath(const std::string &funcName)
//------------------------------------------------------------------------------
/*
 * Returns the absolute path that has GmatFunction name.
 * It searches in the most recently added path first which is at the top of
 * the list.
 *
 * @param   funcName  Name of the GmatFunction to be located
 * @return  Path that has GmatFunction name
 */
//------------------------------------------------------------------------------
std::string FileManager::GetGmatFunctionPath(const std::string &funcName)
{
   return GetGmatPath(GMAT_FUNCTION, mGmatFunctionPaths, funcName);
}


//------------------------------------------------------------------------------
// const StringArray& GetAllGmatFunctionPaths()
//------------------------------------------------------------------------------
const StringArray& FileManager::GetAllGmatFunctionPaths()
{
   mGmatFunctionFullPaths.clear();

   std::list<std::string>::iterator listpos = mGmatFunctionPaths.begin();
   while (listpos != mGmatFunctionPaths.end())
   {
      mGmatFunctionFullPaths.push_back(ConvertToAbsPath(*listpos));
      ++listpos;
   }

   return mGmatFunctionFullPaths;
}


//------------------------------------------------------------------------------
// void ClearMatlabFunctionPath()
//------------------------------------------------------------------------------
void FileManager::ClearMatlabFunctionPath()
{
   mMatlabFunctionPaths.clear();
}


void FileManager::AddMatlabFunctionPath(const char *path, bool addFront)
{
   return AddMatlabFunctionPath(std::string(path), addFront);
}


//------------------------------------------------------------------------------
// void  AddMatlabFunctionPath(const std::string &path, bool addFront = true)
//------------------------------------------------------------------------------
/*
 * If new path it adds to the MatlabFunction path list.
 * If path already exist, it moves to the front or back of the list, depends on
 * addFront flag.
 *
 * @param  path  path name to be added
 * @param  addFront  if set to true, it adds to the front, else adds to the back [true]
 */
//------------------------------------------------------------------------------
void FileManager::AddMatlabFunctionPath(const std::string &path, bool addFront)
{
   #ifdef DEBUG_GMAT_PATH
   MessageInterface::ShowMessage
      ("FileManager::AddMatlabFunctionPath() Adding %s to MatlabFunctionPath\n",
       path.c_str());
   #endif

   std::list<std::string>::iterator pos =
      find(mMatlabFunctionPaths.begin(), mMatlabFunctionPaths.end(), path);

   if (pos == mMatlabFunctionPaths.end())
   {
      // if new path, add to front or back of the list
      if (addFront)
         mMatlabFunctionPaths.push_front(path);
      else
         mMatlabFunctionPaths.push_back(path);
   }
   else
   {
      // if existing path remove and add front or back of the list
      std::string oldPath = *pos;
      mMatlabFunctionPaths.erase(pos);
      if (addFront)
         mMatlabFunctionPaths.push_front(oldPath);
      else
         mMatlabFunctionPaths.push_back(oldPath);
   }

   #ifdef DEBUG_GMAT_PATH
   pos = mMatlabFunctionPaths.begin();
   while (pos != mMatlabFunctionPaths.end())
   {
      MessageInterface::ShowMessage
         ("   mMatlabFunctionPaths=%s\n",(*pos).c_str());
      ++pos;
   }
   #endif
}


std::string FileManager::GetMatlabFunctionPath(const char *funcName)
{
   return GetMatlabFunctionPath(std::string(funcName));
}


//------------------------------------------------------------------------------
// std::string GetMatlabFunctionPath(const std::string &funcName)
//------------------------------------------------------------------------------
/*
 * Returns the absolute path that has MatlabFunction name.
 * It searches in the most recently added path first which is at the top of
 * the list.
 *
 * @param   funcName  Name of the MatlabFunction to be located
 * @return  Path that has MatlabFunction name
 */
//------------------------------------------------------------------------------
std::string FileManager::GetMatlabFunctionPath(const std::string &funcName)
{
   std::string path = GetGmatPath(MATLAB_FUNCTION, mMatlabFunctionPaths, funcName);
   
   // Write informational message if debug is turned on from the startup file
   if (mWriteFilePathInfo == "ON")
   {
      if (path == "")
         MessageInterface::ShowMessage
            ("*** Using MATLAB built-in function '%s'\n", funcName.c_str());
      else
         MessageInterface::ShowMessage
            ("*** Using MATLAB function '%s' from '%s'\n", funcName.c_str(), path.c_str());
   }
   
   return path;
}


//------------------------------------------------------------------------------
// const StringArray& GetAllMatlabFunctionPaths()
//------------------------------------------------------------------------------
const StringArray& FileManager::GetAllMatlabFunctionPaths()
{
   mMatlabFunctionFullPaths.clear();

   std::list<std::string>::iterator listpos = mMatlabFunctionPaths.begin();
   while (listpos != mMatlabFunctionPaths.end())
   {
      mMatlabFunctionFullPaths.push_back(ConvertToAbsPath(*listpos));
      ++listpos;
   }

   return mMatlabFunctionFullPaths;
}


//------------------------------------------------------------------------------
// void AddPythonModulePath(const std::string& path)
//------------------------------------------------------------------------------
/**
 * Adds a folder to the buffer for the Python search path
 *
 * @param path The new folder that may contain Python modules
 */
//------------------------------------------------------------------------------
void FileManager::AddPythonModulePath(const std::string& path)
{
#ifdef DEBUG_GMAT_PATH
	MessageInterface::ShowMessage
		("FileManager::AddPythonModulePath() Adding %s to PythonModulePath\n",
		path.c_str());
#endif

	std::list<std::string>::iterator pos =
		find(mPythonModulePaths.begin(), mPythonModulePaths.end(), path);

	if (pos == mPythonModulePaths.end())
	{
		mPythonModulePaths.push_back(path);
	}
	

#ifdef DEBUG_GMAT_PATH
	pos = mPythonModulePaths.begin();
	while (pos != mPythonModulePaths.end())
	{
		MessageInterface::ShowMessage
			("   mPythonModulePaths=%s\n", (*pos).c_str());
		++pos;
	}
#endif
}


// ------------------------------------------------------------------------------
// const StringArray& GetAllPythonModulePaths()
//------------------------------------------------------------------------------
const StringArray& FileManager::GetAllPythonModulePaths()
{
	mPythonModuleFullPaths.clear();

	std::list<std::string>::iterator listpos = mPythonModulePaths.begin();
	while (listpos != mPythonModulePaths.end())
	{
		mPythonModuleFullPaths.push_back(ConvertToAbsPath(*listpos));
		++listpos;
	}

	return mPythonModuleFullPaths;
}


//------------------------------------------------------------------------------
// std::string GetLastFilePathMessage()
//------------------------------------------------------------------------------
/**
 * Returns the last file path message set from FindPath().
 */
//------------------------------------------------------------------------------
std::string FileManager::GetLastFilePathMessage()
{
   return mLastFilePathMessage;
}

//------------------------------------------------------------------------------
// const StringArray& GetPluginList()
//------------------------------------------------------------------------------
/**
 * Accesses the list of plug-in libraries parsed from the startup file.
 *
 * @return The list of plug-in libraries
 */
//------------------------------------------------------------------------------
const StringArray& FileManager::GetPluginList()
{
   return mPluginList;
}


//------------------------------------------------------------------------------
// void AdjustSettings(const std::string &suffix, const StringArray &forEntries)
//------------------------------------------------------------------------------
/**
 * Appends a suffix to a list of settings stored in the file manager
 *
 * @param suffix The suffix to be appended to the setting
 * @param forEntries A list of entries that receive the suffix
 */
//------------------------------------------------------------------------------
void FileManager::AdjustSettings(const std::string &suffix,
                                 const StringArray &forEntries)
{
   std::string temp;
   for (UnsignedInt i = 0; i < forEntries.size(); ++i)
   {
      // Adjust if path
      temp = mPathMap[forEntries[i]];
      if (temp != "")
      {
         std::string trailingSlash = "";
         if ((temp[temp.length()-1] == '/') || (temp[temp.length()-1] == '\\'))
         {
            trailingSlash = temp.substr(temp.length()-1);
            temp = temp.substr(0, temp.length()-1);
         }
         temp += suffix;
         temp += trailingSlash;
         mPathMap[forEntries[i]] = temp;
      }
      else // Adjust if file
      {
         FileInfo *tempFI = mFileMap[forEntries[i]];
         temp = tempFI->mFile;
         std::string extension = "";

         if (temp.find(".") != std::string::npos)
         {
            extension = temp.substr(temp.find("."));
            temp = temp.substr(0, temp.find("."));
         }

         if (temp != "")
         {
            temp += suffix + extension;
            tempFI->mFile = temp;
         }
      }
   }
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// std::string GetGmatPath(GmatPathType type, const std::list<std::string> &pathList
//                         const std::string &name)
//------------------------------------------------------------------------------
/*
 * Searches proper GMAT path list from the top and return first path found.
 *
 * @param  type  type of function (MATLAB_FUNCTION, GMAT_FUNCTION)
 * @param  pathList  function path list to use in search
 * @param  name  name of the function to search
 */
//------------------------------------------------------------------------------
std::string FileManager::GetGmatPath(GmatPathType type,
                                     std::list<std::string> &pathList,
                                     const std::string &name)
{
   #ifdef DEBUG_GMAT_PATH
   MessageInterface::ShowMessage
      ("FileManager::GetGmatPath(%s) with type %d entered\n",
       name.c_str(), type);
   #endif
   
   std::string name1 = name;
   if (type == GMAT_FUNCTION)
   {
      if (name.find(".gmf") == name.npos)
         name1 = name1 + ".gmf";
   }
   else if (type == MATLAB_FUNCTION)
   {
      if (name.find(".m") == name.npos)
         name1 = name1 + ".m";
   }
   else if (type == GMAT_INCLUDE)
   {
      // GMAT include file can be any extension
      name1 = name;
      
      // If include name contains absolute path, just return the blank path
      if (GmatFileUtil::IsPathAbsolute(name1))
      {
         std::string blankPath;
         #ifdef DEBUG_GMAT_PATH
         MessageInterface::ShowMessage
            ("FileManager::GetGmatPath(%s) returning '%s'\n", name.c_str(),
             blankPath.c_str());
         #endif
         return blankPath;
      }
   }
   else
   {
      UtilityException ue;
      ue.SetDetails("*** INTERNAL ERROR *** FileManager::GetGmatPath() The path "
                    "type %d is undefined\n", type);
      throw ue;
   }
   
   // Search through pathList
   // The most recent path added to the last, so search backwards
   std::string pathName, fullPath;
   bool fileFound = false;

   // Search from the top of the list, which is the most recently added path
   // The search order goes from top to bottom. (loj: 2008.10.02)
   std::list<std::string>::iterator pos = pathList.begin();
   while (pos != pathList.end())
   {
      pathName = *pos;
      fullPath = ConvertToAbsPath(pathName) + name1;

      #ifdef DEBUG_GMAT_PATH
      MessageInterface::ShowMessage("   pathName='%s'\n", pathName.c_str());
      MessageInterface::ShowMessage("   fullPath='%s'\n", fullPath.c_str());
      #endif

      if (GmatFileUtil::DoesFileExist(fullPath))
      {
         fileFound = true;
         break;
      }

      pos++;
   }

   if (fileFound)
      fullPath = GmatFileUtil::ParsePathName(fullPath);
   else
      fullPath = "";

   #ifdef DEBUG_GMAT_PATH
   MessageInterface::ShowMessage
      ("FileManager::GetGmatPath(%s) returning '%s'\n", name.c_str(),
       fullPath.c_str());
   #endif

   return fullPath;
}


//------------------------------------------------------------------------------
// void AddFileType(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Adds file type, path, name to the list. If typeName ends with _PATH, it is
 * added to path map. If typeName ends with _FILE, it is added to file map, an
 * exception is throw otherwise.
 *
 * @param <type> file type
 * @param <name> file path or name
 *
 * @excepton thrown if typeName does not end with _PATH or _FILE
 */
//------------------------------------------------------------------------------
void FileManager::AddFileType(const std::string &type, const std::string &name)
{
   #ifdef DEBUG_ADD_FILETYPE
   MessageInterface::ShowMessage
      ("FileManager::AddFileType() entered, type=%s, name=%s\n", type.c_str(), name.c_str());
   #endif

   if (type.find("_PATH") != type.npos)
   {
      std::string str2 = name;
      
      // append '/' if '\\' or '/' not there
      if (!GmatStringUtil::EndsWithPathSeparator(str2))
         str2 = str2 + mPathSeparator;
      
      mPathMap[type] = str2;

      #ifdef DEBUG_ADD_FILETYPE
      MessageInterface::ShowMessage
         ("   Adding %s = %s to mPathMap\n", type.c_str(), str2.c_str());
      #endif
      
      // Handle Gmat and Matlab Function path
      if (type == "GMAT_FUNCTION_PATH")
         AddGmatFunctionPath(str2, false);
      else if (type == "MATLAB_FUNCTION_PATH")
         AddMatlabFunctionPath(str2, false);
	  else if (type == "PYTHON_MODULE_PATH")
		  AddPythonModulePath(str2);

   }
   else if (type.find("_FILE_ABS") != type.npos)
   {
      mFileMap[type] = new FileInfo("", name);
   }
   else if (type.find("_FILE") != type.npos)
   {
      std::string pathName;
      std::string fileName;
      
      // file name
      std::string::size_type pos = name.find_last_of("/");
      if (pos == name.npos)
         pos = name.find_last_of("\\");
      
      if (pos != name.npos)
      {
         std::string pathName = name.substr(0, pos);
         std::string fileName = name.substr(pos+1, name.npos);
         mFileMap[type] = new FileInfo(pathName, fileName);
         
         #ifdef DEBUG_ADD_FILETYPE
         MessageInterface::ShowMessage
            ("   Adding %s and %s to mFileMap\n", pathName.c_str(), fileName.c_str());
         #endif
      }
      else
      {
         //loj: Should we add current path?
         std::string pathName = "CURRENT_PATH";
         mPathMap[pathName] = "./";
         std::string fileName = name;
         mFileMap[type] = new FileInfo(pathName, fileName);
         
         #ifdef DEBUG_ADD_FILETYPE
         MessageInterface::ShowMessage
            ("   Adding %s and %s to mFileMap\n", pathName.c_str(), fileName.c_str());
         MessageInterface::ShowMessage
            ("   'PATH/' not found in line:\n   %s = %s\n   So adding CURRENT_PATH = ./\n",
             type.c_str(), name.c_str());
         #endif
         
         //loj: Should we just throw an exception?
         //mInStream.close();
         //throw UtilityException
         //   ("FileManager::AddFileType() expecting 'PATH/' in line:\n" +
         //    type + " = " + name);
      }
   }
   else if (type == "PLUGIN")
   {
      #ifdef DEBUG_PLUGIN_DETECTION
         MessageInterface::ShowMessage("Adding plug-in %s to plugin list\n",
               name.c_str());
      #endif
      mPluginList.push_back(name);
   }
   else
   {
      throw UtilityException
         ("FileManager::AddFileType() file type should have '_PATH' or '_FILE'"
          " in:\n" + type);
   }
   
   #ifdef DEBUG_ADD_FILETYPE
   MessageInterface::ShowMessage("FileManager::AddFileType() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void AddAvailablePotentialFiles()
//------------------------------------------------------------------------------
void FileManager::AddAvailablePotentialFiles()
{
   // add available potential files

   // earth gravity files
   if (mFileMap.find("JGM2_FILE") == mFileMap.end())
      AddFileType("JGM2_FILE", "EARTH_POT_PATH/JGM2.cof");

   if (mFileMap.find("JGM3_FILE") == mFileMap.end())
      AddFileType("JGM3_FILE", "EARTH_POT_PATH/JGM3.cof");

   if (mFileMap.find("EGM96_FILE") == mFileMap.end())
      AddFileType("EGM96_FILE", "EARTH_POT_PATH/EGM96low.cof");

   // luna gravity files
   if (mFileMap.find("LP165P_FILE") == mFileMap.end())
      AddFileType("LP165P_FILE", "LUNA_POT_PATH/LP165P.cof");

   // venus gravity files
   if (mFileMap.find("MGNP180U_FILE") == mFileMap.end())
      AddFileType("MGNP180U_FILE", "VENUS_POT_PATH/MGNP180U.cof");

   // mars gravity files
   if (mFileMap.find("MARS50C_FILE") == mFileMap.end())
      AddFileType("MARS50C_FILE", "MARS_POT_PATH/Mars50c.cof");

}


//------------------------------------------------------------------------------
// void WriteHeader(std::ofstream &outStream)
//------------------------------------------------------------------------------
void FileManager::WriteHeader(std::ofstream &outStream)
{
   outStream << "#-------------------------------------------------------------------------------\n";
   outStream << "# General Mission Analysis Tool (GMAT) startup file\n";
   outStream << "#-------------------------------------------------------------------------------\n";
   outStream << "# Comment line starts with #\n";
   outStream << "# Comment line starting with ## will be saved when saving startup file.\n";
   outStream << "#\n";
   outStream << "# Path/File naming convention:\n";
   outStream << "#   - Path name should end with _PATH\n";
   outStream << "#   - File name should end with _FILE\n";
   outStream << "#   - Path/File names are case sensative\n";
   outStream << "#\n";
   outStream << "# You can add potential and texture files by following the naming convention.\n";
   outStream << "#   - Potential file should begin with planet name and end with _POT_FILE\n";
   outStream << "#   - Texture file should begin with planet name and end with _TEXTURE_FILE\n";
   outStream << "#\n";
   outStream << "# If same _FILE is specified multiple times, it will use the last one.\n";
   outStream << "#\n";
   outStream << "# You can have more than one line containing GMAT_FUNCTION_PATH. GMAT will store \n";
   outStream << "# the multiple paths you specify and scan for GMAT Functions using the paths \n";
   outStream << "# in top to bottom order and use the first function found from the search paths.\n";
   outStream << "#\n";
   outStream << "# In order for an object plugin to work inside GMAT, the plugin dynamic link libraries; \n";
   outStream << "# Windows(.dll), Linux(.so) and Mac(.dylib), must be placed in the folder containing\n";
   outStream << "# the GMAT executable or application. Once placed in the correct folder \n";
   outStream << "# the PLUGIN line below must be set equal to the plugin name without the dynamic link \n";
   outStream << "# library extension with the comment (#) removed from the front of the line.\n";
   outStream << "#\n";
   outStream << "# Some available PLUGINs are:\n";
   outStream << "# PLUGIN = libMatlabInterface\n";
   outStream << "# PLUGIN = libFminconOptimizer\n";
   outStream << "# PLUGIN = libVF13Optimizer\n";
   outStream << "# PLUGIN = libDataFile\n";
   outStream << "# PLUGIN = libCcsdsEphemerisFile\n";
   outStream << "# PLUGIN = libGmatEstimation\n";
   outStream << "#\n";
   outStream << "#===============================================================================\n";
}

//------------------------------------------------------------------------------
// void WriteFiles(std::ofstream &outStream, const std::string &type)
//------------------------------------------------------------------------------
void FileManager::WriteFiles(std::ofstream &outStream, const std::string &type)
{
   #ifdef DEBUG_WRITE_STARTUP_FILE
   MessageInterface::ShowMessage("   .....Writing %s file\n", type.c_str());
   #endif
   
   std::string realPath;
   
   // Write remainder of path
   if (type == "-OTHER-PATH-")
   {
      for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
           pos != mPathMap.end(); ++pos)
      {
         // if name not found in already written out list, then write
         if (find(mPathWrittenOuts.begin(), mPathWrittenOuts.end(), pos->first) ==
             mPathWrittenOuts.end())
         {
            if (pos->second != "")
            {
               realPath = pos->second;
               mPathWrittenOuts.push_back(pos->first);
               outStream << std::setw(22) << pos->first << " = "
                         << realPath << "\n";
            }
         }
      }
      return;
   }
   
   // Write remainder of files
   if (type == "-OTHER-")
   {
      for (std::map<std::string, FileInfo*>::iterator pos = mFileMap.begin();
           pos != mFileMap.end(); ++pos)
      {
         // if name not found in already written out list, then write
         if (find(mFileWrittenOuts.begin(), mFileWrittenOuts.end(), pos->first) ==
             mFileWrittenOuts.end())
         {
            if (pos->second)
            {
               realPath = pos->second->mPath;
               if (realPath == "CURRENT_PATH")
                  realPath = "";
               else
                  realPath = realPath + mPathSeparator;
               
               mFileWrittenOuts.push_back(pos->first);
               outStream << std::setw(22) << pos->first << " = "
                         << realPath << pos->second->mFile << "\n";
            }
         }
      }
      return;
   }
   
   for (std::map<std::string, FileInfo*>::iterator pos = mFileMap.begin();
        pos != mFileMap.end(); ++pos)
   {
      if (pos->first.find(type) != std::string::npos)
      {
         if (pos->second)
         {
            realPath = pos->second->mPath;
            if (realPath == "CURRENT_PATH")
               realPath = "";
            else
               realPath = realPath + mPathSeparator;
            
            mFileWrittenOuts.push_back(pos->first);
            outStream << std::setw(22) << pos->first << " = "
                      << realPath << pos->second->mFile << "\n";
         }
      }
   }
}


//------------------------------------------------------------------------------
// void RefreshFiles()
//------------------------------------------------------------------------------
void FileManager::RefreshFiles()
{
   mRunMode = "";
   mPlotMode = "";
   mMatlabMode = "";
   mDebugMatlab = "";
   mDebugMissionTree = "";
   mWriteParameterInfo = "";
   mWriteFilePathInfo = "";
   mWriteGmatKeyword = "";
   mLastFilePathMessage = "";
   mPathMap.clear();
   mGmatFunctionPaths.clear();
   mMatlabFunctionPaths.clear();
   mGmatFunctionFullPaths.clear();
   mSavedComments.clear();
   mPluginList.clear();

   for (std::map<std::string, FileInfo*>::iterator iter = mFileMap.begin();
        iter != mFileMap.begin(); ++iter)
      delete iter->second;

   mFileMap.clear();
   
   //-------------------------------------------------------
   // add root and data path
   //-------------------------------------------------------
   AddFileType("ROOT_PATH", "../");
   AddFileType("DATA_PATH", "ROOT_PATH/data");
   AddFileType("FILE_UPDATE_PATH", "ROOT_PATH/data");
   
   //-------------------------------------------------------
   // add default output paths and files
   //-------------------------------------------------------
   std::string defOutPath = "../output";
   if (!DoesDirectoryExist(defOutPath))
      defOutPath = "./";
   
   AddFileType("OUTPUT_PATH", defOutPath);
   AddFileType("LOG_FILE", "OUTPUT_PATH/GmatLog.txt");
   AddFileType("REPORT_FILE", "OUTPUT_PATH/ReportFile.txt");
   AddFileType("EPHEM_OUTPUT_FILE", "OUTPUT_PATH/EphemerisFile.eph");
   AddFileType("MEASUREMENT_PATH", "OUTPUT_PATH");
   AddFileType("VEHICLE_EPHEM_CCSDS_PATH", "OUTPUT_PATH");
   AddFileType("SCREENSHOT_FILE", "OUTPUT_PATH");
   
   
   // Should we add default input paths and files?
   // Yes, for now in case of startup file doesn't specify all the required
   // input path and files (LOJ: 2011.03.21)
   // Currently #define __FM_ADD_DEFAULT_INPUT__ was commented out
#ifdef __FM_ADD_DEFAULT_INPUT__

   #ifdef DEBUG_REFRESH_FILES
   MessageInterface::ShowMessage
      ("FileManager::RefreshFiles() creatng default input paths and files\n");
   #endif
   
   //-------------------------------------------------------
   // create default input paths and files
   //-------------------------------------------------------
   
   // planetary de files
   AddFileType("PLANETARY_EPHEM_DE_PATH", "DATA_PATH/planetary_ephem/de/");
   AddFileType("DE405_FILE", "PLANETARY_EPHEM_DE_PATH/leDE1941.405");

   // planetary spk files
   AddFileType("PLANETARY_EPHEM_SPK_PATH", "DATA_PATH/planetary_ephem/spk/");
   AddFileType("PLANETARY_SPK_FILE", "PLANETARY_EPHEM_SPK_PATH/de421.bsp");

   // vehicle spk files
   AddFileType("VEHICLE_EPHEM_SPK_PATH", "DATA_PATH/vehicle/ephem/spk/");

   // earth gravity files
   AddFileType("EARTH_POT_PATH", "DATA_PATH/gravity/earth/");
   AddFileType("JGM2_FILE", "EARTH_POT_PATH/JGM2.cof");
   AddFileType("JGM3_FILE", "EARTH_POT_PATH/JGM3.cof");
   AddFileType("EGM96_FILE", "EARTH_POT_PATH/EGM96.cof");

   // luna gravity files
   AddFileType("LUNA_POT_PATH", "DATA_PATH/gravity/luna/");
   AddFileType("LP165P_FILE", "LUNA_POT_PATH/lp165p.cof");

   // venus gravity files
   AddFileType("VENUS_POT_PATH", "DATA_PATH/gravity/venus/");
   AddFileType("MGNP180U_FILE", "VENUS_POT_PATH/MGNP180U.cof");

   // mars gravity files
   AddFileType("MARS_POT_PATH", "DATA_PATH/gravity/mars/");
   AddFileType("MARS50C_FILE", "MARS_POT_PATH/Mars50c.cof");

   // planetary coeff. files
   AddFileType("PLANETARY_COEFF_PATH", "DATA_PATH/planetary_coeff/");
   AddFileType("EOP_FILE", "PLANETARY_COEFF_PATH/eopc04.62-now");
   // wcs 2013.03.04 PLANETARY_COEFF_FILE is CURRENTLY not used, since our
   // default is PLANETARY_1980 and PLANETARY_1996 is not allowed; however,
   // we will leave this here as a placeholder anyway
   AddFileType("PLANETARY_COEFF_FILE", "PLANETARY_COEFF_PATH/NUT85.DAT");
   AddFileType("NUTATION_COEFF_FILE", "PLANETARY_COEFF_PATH/NUTATION.DAT");

   AddFileType("PLANETARY_PCK_FILE", "PLANETARY_COEFF_PATH/SPICEPlanetaryConstantsKernel.tpc");
   AddFileType("EARTH_LATEST_PCK_FILE", "PLANETARY_COEFF_PATH/earth_latest_high_prec.bpc");
   AddFileType("EARTH_PCK_PREDICTED_FILE", "PLANETARY_COEFF_PATH/SPICEEarthPredictedKernel.bpc");
   AddFileType("EARTH_PCK_CURRENT_FILE", "PLANETARY_COEFF_PATH/SPICEEarthCurrentKernel.bpc");
   AddFileType("LUNA_PCK_CURRENT_FILE", "PLANETARY_COEFF_PATH/SPICELunaCurrentKernel.bpc");
   AddFileType("LUNA_FRAME_KERNEL_FILE", "PLANETARY_COEFF_PATH/SPICELunaFrameKernel.tf");

   // time path and files
   AddFileType("TIME_PATH", "DATA_PATH/time/");
   AddFileType("LEAP_SECS_FILE", "TIME_PATH/tai-utc.dat");
   AddFileType("LSK_FILE", "TIME_PATH/naif0010.tls");

   // atmosphere path and files
   AddFileType("ATMOSPHERE_PATH", "DATA_PATH/atmosphere/earth/");
   AddFileType("CSSI_FLUX_FILE", "ATMOSPHERE_PATH/CSSI_2004To2026.txt");
   AddFileType("SCHATTEN_FILE", "ATMOSPHERE_PATH/SchattenPredict.txt");
   AddFileType("MARINI_TROPO_FILE", "ATMOSPHERE_PATH/marini.dat");
   
   // gui config file path
   AddFileType("GUI_CONFIG_PATH", "DATA_PATH/gui_config/");
   
   // personalization file
   AddFileType("PERSONALIZATION_FILE", "OUTPUT_PATH/MyGmat.ini");
   
   // icon path and main icon file
   AddFileType("ICON_PATH", "DATA_PATH/graphics/icons/");
   
   #if defined __WXMSW__
   AddFileType("MAIN_ICON_FILE", "ICON_PATH/GMATWin32.ico");
   #elif defined __WXGTK__
   AddFileType("MAIN_ICON_FILE", "ICON_PATH/GMATLinux48.xpm");
   #elif defined __WXMAC__
   AddFileType("MAIN_ICON_FILE", "ICON_PATH/GMATIcon.icns");
   #endif

   // splash file path
   AddFileType("SPLASH_PATH", "DATA_PATH/graphics/splash/");
   AddFileType("SPLASH_FILE", "SPLASH_PATH/GMATSplashScreen.tif");
   
   // texture file path
   AddFileType("TEXTURE_PATH", "DATA_PATH/graphics/texture/");
   AddFileType("SUN_TEXTURE_FILE", "TEXTURE_PATH/Sun.jpg");
   AddFileType("MERCURY_TEXTURE_FILE", "TEXTURE_PATH/Mercury_JPLCaltech.jpg");
   AddFileType("EARTH_TEXTURE_FILE", "TEXTURE_PATH/ModifiedBlueMarble.jpg");
   AddFileType("MARS_TEXTURE_FILE", "TEXTURE_PATH/Mars_JPLCaltechUSGS.jpg");
   AddFileType("JUPITER_TEXTURE_FILE", "TEXTURE_PATH/Jupiter_HermesCelestiaMotherlode.jpg");
   AddFileType("SATRUN_TEXTURE_FILE", "TEXTURE_PATH/Saturn_gradiusCelestiaMotherlode.jpg");
   AddFileType("URANUS_TEXTURE_FILE", "TEXTURE_PATH/Uranus_JPLCaltech.jpg");
   AddFileType("NEPTUNE_TEXTURE_FILE", "TEXTURE_PATH/Neptune_BjornJonsson.jpg");
   AddFileType("PLUTO_TEXTURE_FILE", "TEXTURE_PATH/Pluto_JPLCaltech.jpg");
   AddFileType("LUNA_TEXTURE_FILE", "TEXTURE_PATH/Moon_HermesCelestiaMotherlode.jpg");
   
   // star path and files
   AddFileType("STAR_PATH", "DATA_PATH/graphics/stars/");
   AddFileType("STAR_FILE", "STAR_PATH/inp_StarCatalog.txt");
   AddFileType("CONSTELLATION_FILE", "STAR_PATH/inp_Constellation.txt");
   
   // models
   AddFileType("VEHICLE_MODEL_PATH", "DATA_PATH/vehicle/models/");
   AddFileType("SPACECRAFT_MODEL_FILE", "VEHICLE_MODEL_PATH/aura.3ds");
   
   // help file
   AddFileType("HELP_FILE", "");
   
#endif

}


//------------------------------------------------------------------------------
// void ShowMaps(const std::string &msg)
//------------------------------------------------------------------------------
void FileManager::ShowMaps(const std::string &msg)
{
   MessageInterface::ShowMessage("%s\n", msg.c_str());
   MessageInterface::ShowMessage("Here is path map, there are %d items\n", mPathMap.size());
   for (std::map<std::string, std::string>::iterator pos = mPathMap.begin();
        pos != mPathMap.end(); ++pos)
   {
      MessageInterface::ShowMessage("%20s: %s\n", (pos->first).c_str(), (pos->second).c_str());
   }
   
   MessageInterface::ShowMessage("Here is file map, there are %d items\n", mFileMap.size());
   for (std::map<std::string, FileInfo*>::iterator pos = mFileMap.begin();
        pos != mFileMap.end(); ++pos)
   {
      MessageInterface::ShowMessage
         ("%20s: %20s  %s\n", (pos->first).c_str(), (pos->second)->mPath.c_str(), (pos->second)->mFile.c_str());
   }
}


//------------------------------------------------------------------------------
// FileManager(const std::string &appName = "GMAT.exe")
//------------------------------------------------------------------------------
/*
 * Constructor
 */
//------------------------------------------------------------------------------
FileManager::FileManager(const std::string &appName)
{
   MessageInterface::SetLogEnable(false); // so that debug can be written from here
   
   #ifdef DEBUG_FILE_MANAGER
   MessageInterface::ShowMessage
      ("FileManager::FileManager() entered, MAX_PATH = %d, MAX_PATH_LEN = %d\n",
       MAX_PATH, GmatFile::MAX_PATH_LEN);
   #endif
   
   // Set only bin directory
   SetBinDirectory(appName);
   
   // Set platform dependent data
   mIsOsWindows = GmatFileUtil::IsOsWindows();
   mPathSeparator = GetPathSeparator();
   mStartupFileDir = GmatFileUtil::GetCurrentWorkingDirectory() + mPathSeparator;
   
   // Set GMAT startup file
   mStartupFileName = "gmat_startup_file.txt";
   
   GmatGlobal::Instance()->AddHiddenCommand("SaveMission");
   
   #ifdef DEBUG_STARTUP_FILE
   MessageInterface::ShowMessage
      ("FileManager::FileManager() entered, mPathSeparator='%s', "
       "mStartupFileDir='%s', mStartupFileName='%s'\n", mPathSeparator.c_str(),
       mStartupFileDir.c_str(), mStartupFileName.c_str());
   #endif
   
   RefreshFiles();
}


//------------------------------------------------------------------------------
// void SetPathsAbsolute()
//------------------------------------------------------------------------------
/*
 * Sets the paths read from the startup file to absolute paths
 *
 * This method is separate from ReadStartupFile so that if it is broken, it can
 * just be commented out.
 */
//------------------------------------------------------------------------------
void FileManager::SetPathsAbsolute()
{
   std::string mGmatBinDir = GmatFileUtil::GetGmatPath();

   #ifdef DEBUG_STARTUP_WITH_ABSOLUTE_PATH
      MessageInterface::ShowMessage("Real bin folder = %s\nPaths:\n", 
         mGmatBinDir.c_str());
      for (std::map<std::string, std::string>::iterator path = mPathMap.begin(); 
         path != mPathMap.end(); ++path)
         MessageInterface::ShowMessage("   %s\n", path->second.c_str());
      MessageInterface::ShowMessage("Files:\n");
      for (std::map<std::string, FileInfo*>::iterator file = mFileMap.begin(); 
         file != mFileMap.end(); ++file)
         MessageInterface::ShowMessage("   %s  %s\n", file->second->mPath.c_str(),
               file->second->mFile.c_str());
   #endif

   for (std::map<std::string, std::string>::iterator path = mPathMap.begin(); 
      path != mPathMap.end(); ++path)
   {
      #ifdef DEBUG_STARTUP_WITH_ABSOLUTE_PATH
         MessageInterface::ShowMessage("Checking %s:  ", path->second.c_str());
      #endif
      const char* setting = path->second.c_str();
      if (setting[0] == '.')
      {
         path->second = mGmatBinDir + path->second;
         #ifdef DEBUG_STARTUP_WITH_ABSOLUTE_PATH
               MessageInterface::ShowMessage("reset to %s", path->second.c_str());
         #endif
      }
      #ifdef DEBUG_STARTUP_WITH_ABSOLUTE_PATH
         MessageInterface::ShowMessage("\n");
      #endif
   }
   
   #ifdef DEBUG_STARTUP_WITH_ABSOLUTE_PATH
      MessageInterface::ShowMessage("Paths:\n");
      for (std::map<std::string, std::string>::iterator path = mPathMap.begin(); 
         path != mPathMap.end(); ++path)
         MessageInterface::ShowMessage("   %s\n", path->second.c_str());
      MessageInterface::ShowMessage("Files:\n");
      for (std::map<std::string, FileInfo*>::iterator file = mFileMap.begin(); 
         file != mFileMap.end(); ++file)
         MessageInterface::ShowMessage("   %s  %s\n", file->second->mPath.c_str(),
               file->second->mFile.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void ValidatePaths()
//------------------------------------------------------------------------------
/*
 * Validate all paths in file manager
 */
//------------------------------------------------------------------------------
bool FileManager::ValidatePaths()
{
   std::string s = "";
   int i = 0;
   FileType type;

   for ( int fooInt = BEGIN_OF_PATH; fooInt != END_OF_PATH; fooInt++ )
   {
      type = FileType(fooInt);
      switch (type)
      {
         case BEGIN_OF_PATH:
         case END_OF_PATH:
            break;
         // non fatal paths?
         case TEXTURE_PATH:
         case BODY_3D_MODEL_PATH:
         case MEASUREMENT_PATH:
         case GUI_CONFIG_PATH:
         case SPLASH_PATH:
         case ICON_PATH:
         case VEHICLE_MODEL_PATH:
            try
            {
               if (!DoesDirectoryExist(GetFullPathname(type)))
               {
                  MessageInterface::ShowMessage("%s directory does not exist: %s",
                     FileManager::FILE_TYPE_STRING[type].c_str(),
                     GetFullPathname(type).c_str());
               }
            }
            catch (UtilityException &e)
            {
                  MessageInterface::ShowMessage("%s directory not specified in gmat_startup_file",
                     FileManager::FILE_TYPE_STRING[type].c_str());
            }
            break;
         default:
            try
            {
               if (!DoesDirectoryExist(GetFullPathname(type)))
               {
                  i++;
                  if (i > 9) goto loopexit;
                  if (i > 1)
                     s = s + "\n";
                  s = s + FileManager::FILE_TYPE_STRING[type] + " = " + 
                     GetFullPathname(type);
               }
            }
            catch (UtilityException &e)
            {
               i++;
               if (i > 9) goto loopexit;
               if (i > 1)
                  s = s + "\n";
               s = s + FileManager::FILE_TYPE_STRING[type] + " = MISSING in gmat_startup_file";
            }
      }
   }

loopexit:
   UtilityException ue;
   if (i == 1)
   {
      ue.SetDetails("The following directory does not exist:\n%s", s.c_str());
      throw ue;
   }
   else if (i > 1)
   {
      ue.SetDetails("At least %d directories do not exist, including:\n%s", i, s.c_str());
      throw ue;
   }
   return i == 0;
}
