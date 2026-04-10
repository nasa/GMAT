//$Id$
//------------------------------------------------------------------------------
//                                 GmatGlobal
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
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/01/12
//
/**
 * Declares Class for getting/setting global data used by GMAT.
 */
//------------------------------------------------------------------------------
#ifndef GmatGlobal_hpp
#define GmatGlobal_hpp

#include "utildefs.hpp"

// forward references
class EopFile;
class ItrfCoefficientsFile;

class GMATUTIL_API GmatGlobal
{
public:

   enum RunMode
   {
      NORMAL = 10,
      EXIT_AFTER_RUN,
      TESTING,
      TESTING_NO_PLOTS,
   };
   
   enum GuiMode
   {
      NORMAL_GUI = 20,
      MINIMIZED_GUI,
   };
   
   enum PlotMode
   {
      NORMAL_PLOT = 25,
      TILED_PLOT,
      CASCADED_PLOT,
   };
   
   ///@note MatlabInterface uses the same enum
   enum MatlabMode
   {
      SINGLE_USE = 30,
      SHARED,
      NO_MATLAB,  // MATLAB is not installed
   };
   
   enum LogfileSource
   {
      CMD_LINE = 35,
      SCRIPT,
      STARTUP,
   };
   
   static GmatGlobal* Instance();
   
   // GMAT version
   std::string GetGmatVersion();
   bool IsGmatCompiledIn64Bit();
   
   // Real to string conversion precison
   static const Integer DATA_PRECISION;
   static const Integer TIME_PRECISION;
   static const Integer DATA_WIDTH;
   static const Integer TIME_WIDTH;
   static const Integer INTEGER_WIDTH;
   
   Integer GetDataPrecision();
   Integer GetTimePrecision();
   Integer GetDataWidth();
   Integer GetTimeWidth();
   Integer GetIntegerWidth();
   std::string GetOutputPath();

   // Data Precision and Width
   void SetDataPrecision(Integer p);
   void SetTimePrecision(Integer p);
   void SetDataWidth(Integer w);
   void SetTimeWidth(Integer w);
   void SetIntegerWidth(Integer w);
   void SetOutputPath(const std::string &path);
   
   // MatlabFunction name extension
   void SetMatlabFuncNameExt(const std::string &ext);
   std::string GetMatlabFuncNameExt();
   
   // Run mode
   bool IsBatchMode();
   void SetBatchMode(bool flag);
   bool IsNitsClient();
   void SetNitsClient(bool flag);
   bool GetRunInterrupted();
   void SetRunInterrupted(bool flag);
   Gmat::RunState GetRunState();
   void SetRunState(Gmat::RunState rs);
   Gmat::RunState GetDetailedRunState();
   void SetDetailedRunState(Gmat::RunState drs);
   Integer GetRunMode();

   Integer GetRunModeStartUp(){return (isTesting? GmatGlobal::TESTING: runMode);};     // It is a temporary fix in order to run gression test with runmode = TESTING. 
                                                                                       // It needs to improve in the next GMAT release
   void SetRunMode(Integer mode);
   Integer GetGuiMode();
   void SetGuiMode(Integer mode);
   Integer GetPlotMode();
   void SetPlotMode(Integer mode);

   void SetCommandEchoMode(bool tf);
   bool EchoCommands();

   // Skip splash screen mode
   void SetSkipSplashMode(bool tfSplash);
   bool SkipSplashMode();

   // MATLAB
   Integer GetMatlabMode();
   void SetMatlabMode(Integer mode);
   bool IsMatlabAvailable();
   void SetMatlabAvailable(bool flag);
   bool IsMatlabDebugOn();
   void SetMatlabDebug(bool flag);
   
   // Debug MissionTree
   bool IsMissionTreeDebugOn();
   void SetMissionTreeDebug(bool flag);
   
   // Debug Parameters
   bool IsWritingParameterInfo();
   void SetWriteParameterInfo(bool flag);
   bool IsWritingFilePathInfo();
   void SetWriteFilePathInfo(bool flag);
   
   // Write GMAT keyword when saving to script or showing script
   bool IsWritingGmatKeyword();
   void SetWriteGmatKeyword(bool flag);

   // Event location
   void SetEventLocationAvailable(bool flag);
   bool IsEventLocationAvailable();
   
   // Script Include statment
   // ScriptInterpreter sets the flag and ResourceTree gets it
   void SetIncludeFoundInScriptResource(bool flag);
   bool GetIncludeFoundInScriptResource();
   
   // Writing of Personalization file (e.g. MyGmat.ini)
   void SetWritePersonalizationFile(bool flag);
   bool GetWritePersonalizationFile();
   
   bool IsGUISavable();
   
   // IO formatting
   bool IsScientific();
   bool ShowPoint();
   bool IsHorizontal();
   bool IsBinaryIn();
   bool IsBinaryOut();
   Integer GetSpacing();
   
   void SetScientific(bool flag);
   void SetShowPoint(bool flag);
   void SetHorizontal(bool flag);
   void SetBinaryIn(bool flag);
   void SetBinaryOut(bool flag);
   void SetSpacing(Integer sp);
   void SetPrefix(const std::string &prefix);
   void SetAppendEol(bool flag);
   
   void SetDefaultFormat();
   void SetCurrentFormat(bool scientific = false, bool showPoint = false,
                         Integer width = GmatGlobal::DATA_WIDTH,
                         Integer precision = GmatGlobal::DATA_PRECISION,
                         bool horizontal = true, Integer spacing = 1,
                         const std::string &prefix = "", bool appendEol = true,
                         bool binaryIn = false, bool binaryOut = false);
   
   void GetActualFormat(bool &scientific, bool &showPoint, Integer &precision,
                        Integer &width, bool &horizontal, Integer &spacing,
                        std::string &prefix, bool &appendEol); 
   
   void SetActualFormat(bool scientific, bool showPoint, Integer precision,
                        Integer width, bool horizontal = true, Integer spacing = 1,
                        const std::string &prefix = "", bool appendEol = true);
   
   void SetToDefaultFormat();
   void SetToCurrentFormat();

   // hidden commands (not in menu)
   void ClearHiddenCommands();
   void AddHiddenCommand(const std::string &cmd);
   bool IsHiddenCommand(const char *cmd);
   bool IsHiddenCommand(const std::string &cmd);
   void RemoveHiddenCommand(const std::string &cmd);
   const StringArray& GetHiddenCommands();

   // Files
   EopFile* GetEopFile();
   ItrfCoefficientsFile* GetItrfCoefficientsFile();
   void SetEopFile(EopFile *eop);
   void SetItrfCoefficientsFile(ItrfCoefficientsFile *itrf);
   
   // Log file set-up
   void        SetLogfileSource(Integer src, const std::string logfileName = "");
   void        SetLogfileName(Integer forSrc, const std::string logfileName);
   Integer     GetLogfileSource();
   std::string GetLogfileName(Integer forSrc = 0);

//   unsigned long GetUsedMemorySize(bool isAtStart);


private:

   // Global setting
   struct GMATUTIL_API Setting
   {
      Setting(Integer dp = DATA_PRECISION, Integer tp = TIME_PRECISION,
              Integer dw = DATA_WIDTH, Integer tw = TIME_WIDTH,
              Integer iw = INTEGER_WIDTH, const std::string &op = "")
      {
         mDataPrecision = dp;
         mTimePrecision = tp;
         mDataWidth = dw;
         mTimeWidth = tw;
         mIntegerWidth = iw;
         mOutputPath = op;
      };
      
      Integer mDataPrecision;
      Integer mTimePrecision;
      Integer mDataWidth;
      Integer mTimeWidth;
      Integer mIntegerWidth;
      std::string mOutputPath;
   };
   
   // I/O formatting   
   struct GMATUTIL_API IoFormat
   {
      IoFormat(bool scientific = false, bool showPoint = false,
               Integer precision = GmatGlobal::DATA_PRECISION,
               Integer width = GmatGlobal::DATA_WIDTH,
               bool horizontal = true, Integer spacing = 1,
               const std::string &prefix = "", bool appendEol = true,
               bool binaryIn = false, bool binaryOut = false)
      {
         mScientific = scientific;
         mShowPoint = showPoint;
         mPrecision = precision;
         mWidth = width;
         mHorizontal = horizontal;
         mSpacing = spacing;
         mPrefix = prefix;
         mAppendEol = appendEol;
         mBinaryIn = binaryIn;
         mBinaryOut = binaryOut;
      };
      
      bool mScientific;    /// format using scientific notation
      bool mShowPoint;     /// format using ios::showpoint
      Integer mPrecision;  /// the number of digits of precision
      Integer mWidth;      /// the field width
      bool mHorizontal;    /// format horizontally if true. Default is false
      Integer mSpacing;    /// determines number of spaces in between each element
      bool mBinaryIn;      /// read in binary if true. Default is false
      bool mBinaryOut;     /// print in binary if true. Default is false
      std::string mPrefix; /// prefix to be used for vertical formatting
      bool mAppendEol ;    /// appends eol if true. Default is true
   };
   
   std::string gmatVersion;
   
   Setting defaultSetting;
   Setting currentSetting;
   
   bool isBatchMode;
   bool isNitsClient;
   bool runInterrupted;
   Gmat::RunState runState;
   Gmat::RunState detailedRunState;
   bool isMatlabAvailable;
   bool isMatlabDebugOn;
   bool isMissionTreeDebugOn;
   bool isWritingParameterInfo;
   bool isWritingFilePathInfo;
   bool isWritingGmatKeyword;
   bool commandEchoMode;
   bool skipSplash;
   
   bool isEventLocationAvailable;
   bool includeFoundInScriptResource;
   
   bool writePersonalizationFile;

   Integer isTesting;            // It is a temporary fix in order to run gression test with runmode = TESTING. 
                                 // It needs to improve in the next GMAT release
   
   Integer runMode;
   Integer guiMode;
   Integer plotMode;
   Integer matlabMode;
   IoFormat defaultFormat;
   IoFormat currentFormat;
   IoFormat actualFormat;
   
   std::string matlabExt;

   std::list<std::string> mHiddenCommands;
   StringArray mHiddenCommandsArray;

   EopFile *theEopFile;
   ItrfCoefficientsFile *theItrfFile;
   
   // Logfile
   Integer logfileSrc;
   std::string cmdLineLog;
   std::string scriptLog;
   std::string startupLog;
   
   /// The singleton instance
   static GmatGlobal *theGmatGlobal;
   
   GmatGlobal();
   ~GmatGlobal();

//   FILE* repPtr;         // It needs for memory leak test
};

#endif // GmatGlobal_hpp

