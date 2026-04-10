//$Id$
//------------------------------------------------------------------------------
//                                  FileManager
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
// Author: Linda Jun
// Created: 2004/04/02
/**
 * Declares FileManager class. This is singleton class which manages list of
 * file paths and names.
 *
 * The textures files and non-Earth gravity potential files not appear in the
 * predefined enum FileType list can be retrieved by using file naming
 * convention. The texture files should have PLANETNAME_TEXTURE_FILE.
 * e.g. "EARTH_TEXTURE_FILE", "LUNA_TEXTURE_FILE", etc. The potential files
 * should have PLANETNAME_POT_FILE.
 */
//------------------------------------------------------------------------------
#ifndef FileManager_hpp
#define FileManager_hpp

#include "utildefs.hpp"
#include <map>
#include <list>
#include <fstream>

class GMATUTIL_API FileManager
{
public:
   
   // The following is predefined file paths/types.
   enum FileType
   {
      // File path
      BEGIN_OF_PATH = 0,
      ROOT_PATH,
      
      // Input path
      TIME_PATH,
      PLANETARY_COEFF_PATH,
      PLANETARY_EPHEM_DE_PATH,
      PLANETARY_EPHEM_SPK_PATH,
      VEHICLE_EPHEM_PATH,
      VEHICLE_EPHEM_SPK_PATH,
      VEHICLE_EPHEM_CCSDS_PATH,
      EARTH_POT_PATH,
      LUNA_POT_PATH,
      VENUS_POT_PATH,
      MARS_POT_PATH,
      OTHER_POT_PATH,
      TEXTURE_PATH, //Notes: TEXTURE_PATH is used in SetPathname()
      BODY_3D_MODEL_PATH, // For celestial body 3D model path
      MEASUREMENT_PATH,
      GUI_CONFIG_PATH,
      SPLASH_PATH,
      ICON_PATH,
      STAR_PATH,
      VEHICLE_MODEL_PATH,
      SPAD_PATH,
      ATMOSPHERE_PATH,
      FILE_UPDATE_PATH,
      
      // Output path
      OUTPUT_PATH,
      END_OF_PATH,
      
      // General file name
      LOG_FILE,
      REPORT_FILE,
      EPHEM_OUTPUT_FILE,
      SPLASH_FILE,
      TIME_COEFF_FILE,
      
      // Specific file name
      //    Notes: Don't add general planet potential files here. They are handled
      //    when gmat_startup_file are read by following naming convention.
      DE405_FILE,
      DE421_FILE,
      DE424_FILE,
      DE430_FILE,
      IAUSOFA_FILE,
      ICRF_FILE,
      PLANETARY_SPK_FILE,
      JGM2_FILE,
      JGM3_FILE,
      EGM96_FILE,
      LP165P_FILE,
      MGNP180U_FILE,
      MARS50C_FILE,
      EOP_FILE,
      PLANETARY_COEFF_FILE,
      NUTATION_COEFF_FILE,
      PLANETARY_PCK_FILE,
      EARTH_LATEST_PCK_FILE,
      EARTH_PCK_PREDICTED_FILE,
      EARTH_PCK_CURRENT_FILE,
      LUNA_PCK_CURRENT_FILE,
      LUNA_FRAME_KERNEL_FILE,
      LEAP_SECS_FILE,
      LSK_FILE,
      PERSONALIZATION_FILE,
      MAIN_ICON_FILE,
      STAR_FILE,
      CONSTELLATION_FILE,
      SPACECRAFT_MODEL_FILE,
      SPAD_SRP_FILE,
      CSSI_FLUX_FILE,
      SCHATTEN_FILE,
      MARINI_TROPO_FILE,
      HELP_FILE,
      FileTypeCount,
   };
   
   static FileManager* Instance(const std::string &appName = "GMAT.exe");
   ~FileManager();
   
   /// GMAT application directory
   std::string GetBinDirectory(const std::string &appName = "GMAT.exe");
   bool        SetBinDirectory(const std::string &appName = "GMAT.exe",
                               const std::string &newBin = "");
   
   /// GMAT working directory
   std::string GetGmatWorkingDirectory();
   bool        SetGmatWorkingDirectory(const std::string &newDir = "");
   
   /// System's current working directory of the process
   std::string GetCurrentWorkingDirectory();
   bool        SetCurrentWorkingDirectory(const std::string &newDir = "");
   
   /// Finds file path using search order
   std::string FindPath(const std::string &fileName, const FileType type,
                        bool forInput, bool writeWarning = false, bool writeInfo = false,
                        const std::string &objName = "");
   std::string FindPath(const std::string &fileName, const std::string &fileType,
                        bool forInput, bool writeWarning = false, bool writeInfo = false,
                        const std::string &objName = "");
   std::string FindMainIconFile(bool writeInfo = false);
   
   std::string GetPathSeparator();
   bool DoesDirectoryExist(const std::string &dirPath, bool isBlankOk = true);
   bool DoesFileExist(const std::string &filename);
   bool RenameFile(const std::string &oldName, const std::string &newName,
                   Integer &retCode, bool overwriteIfExists = false);
   bool CopyFile(const std::string &oldName, const std::string &newName,
                   Integer &retCode, bool overwriteIfExists = false);
   bool ValidatePaths();
   
   // Methods for startup file
   std::string GetStartupFileDir();
   std::string GetStartupFileName();
   std::string GetFullStartupFilePath();
//   void ReadStartupFile(const char *fileName = ""); // remove redundant method
   void ReadStartupFile(const std::string &fileName = "");
//   void WriteStartupFile(const char *fileName = "");// remove redundant method
   void WriteStartupFile(const std::string &fileName = "");
   
   std::string GetRootPath();
   
   // Methods for texture map file
   bool GetTextureMapFile(const std::string &inFileName, const std::string &bodyName,
                          const std::string &objName, std::string &outFileName,
                          std::string &outFullPathName, bool writeWarning);
   // Methods for celestial body model file
   bool GetBody3dModelFile(const std::string &inFileName, const std::string &bodyName,
                           const std::string &objName, std::string &outFileName,
                           std::string &outFullPathName, bool writeWarning);
   
   // Methods returning path
   std::string GetPathname(const FileType type);
   std::string GetPathname(const std::string &typeName);
   
   // Methods returning filename
   std::string GetFilename(const FileType type);
   std::string GetFilename(const std::string &typeName);
   
   // Methods returning full path and filename
   //loj: Why the name "GetFullPathName()" doesn't work? Reserved word?
   // I'm getting unresolved ref on GetFullPathNameA()
   std::string GetFullPathname(const FileType type);
   std::string GetFullPathname(const std::string &typeName);
   std::string GetAbsPathname(const FileType type);
   std::string GetAbsPathname(const std::string &typeName);
   
   std::string ConvertToAbsPath(const std::string &relPath, bool appendPathSep = true);
   
   void SetAbsPathname(const FileType type, const char *newpath);
   void SetAbsPathname(const FileType type, const std::string &newpath);
   void SetAbsPathname(const std::string &type, const char *newpath);
   void SetAbsPathname(const std::string &type, const std::string &newpath);
   
   // GMAT include script methods
   void ClearGmatIncludePath();
   void AddGmatIncludePath(const char *path, bool addFront = true);
   void AddGmatIncludePath(const std::string &path, bool addFront = true);
   std::string GetGmatIncludePath(const char *incName);
   std::string GetGmatIncludePath(const std::string &incName);
   const StringArray& GetAllGmatIncludePaths();
   
   // GMAT function methods
   void ClearGmatFunctionPath();
   void AddGmatFunctionPath(const char *path, bool addFront = true);
   void AddGmatFunctionPath(const std::string &path, bool addFront = true);
   std::string GetGmatFunctionPath(const char *funcName);
   std::string GetGmatFunctionPath(const std::string &funcName);
   const StringArray& GetAllGmatFunctionPaths();
   
   // MATLAB function methods
   void ClearMatlabFunctionPath();
   void AddMatlabFunctionPath(const char* path, bool addFront = true);
   void AddMatlabFunctionPath(const std::string &path, bool addFront = true);
   std::string GetMatlabFunctionPath(const char* funcName);
   std::string GetMatlabFunctionPath(const std::string &funcName);
   const StringArray& GetAllMatlabFunctionPaths();
   
   // Python methods
   void AddPythonModulePath(const std::string &path);
   const StringArray& GetAllPythonModulePaths();

   // Warning/Error message
   std::string GetLastFilePathMessage();
   
   // Plug-in code
   const StringArray& GetPluginList();
   
   void AdjustSettings(const std::string &suffix, const StringArray &forEntries);

private:
   
   enum GmatPathType
   {
      GMAT_FUNCTION = 101,
      MATLAB_FUNCTION,
      GMAT_INCLUDE,
   };
   
   struct FileInfo
   {
      std::string mPath;
      std::string mFile;
      
      FileInfo(const std::string &path, const std::string &file)
         { mPath = path; mFile = file; }
   };
   
   bool        mIsOsWindows;
   std::string mAbsBinDir;
   std::string mGmatWorkingDir;
   std::string mPathSeparator;
   std::string mStartupFileDir;
   std::string mStartupFileName;
   std::string mRunMode;
   std::string mPlotMode;
   std::string mMatlabMode;
   std::string mDebugMatlab;
   std::string mDebugMissionTree;
   std::string mWriteParameterInfo;
   std::string mWriteFilePathInfo;
   std::string mWriteGmatKeyword;
   std::string mLastFilePathMessage;
   
   std::ifstream mInStream;
   std::map<std::string, std::string> mPathMap;
   std::map<std::string, FileInfo*> mFileMap;
   
   std::list<std::string> mGmatIncludePaths;
   std::list<std::string> mGmatFunctionPaths;
   std::list<std::string> mMatlabFunctionPaths;
   
   StringArray mGmatIncludeFullPaths;
   StringArray mGmatFunctionFullPaths;
   StringArray mMatlabFunctionFullPaths;
   
   StringArray mSavedComments;
   StringArray mPathWrittenOuts;
   StringArray mFileWrittenOuts;

   //Python
   StringArray mPythonModuleFullPaths;
   std::list<std::string> mPythonModulePaths;

   StringArray mPluginList;
   
   std::string GetGmatPath(GmatPathType type, std::list<std::string> &pathList,
                           const std::string &name);
   void AddFileType(const std::string &type, const std::string &name);
   void AddAvailablePotentialFiles();
   void WriteHeader(std::ofstream &outStream);
   void WriteFiles(std::ofstream &outStream, const std::string &type);
   void RefreshFiles();
   
   // For debugging
   void ShowMaps(const std::string &msg);
   
   static FileManager *theInstance;
   static const std::string FILE_TYPE_STRING[FileTypeCount];
   
   FileManager(const std::string &appName = "GMAT.exe");
   
   void SetPathsAbsolute();
};
#endif // FileManager_hpp
