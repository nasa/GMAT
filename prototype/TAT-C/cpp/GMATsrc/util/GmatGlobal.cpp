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
 * Implements global data used by GMAT.
 */
//------------------------------------------------------------------------------

//#include "windows.h"                    // It needs for memory leak test
//#include "psapi.h"                      // It needs for memory leak test

#include "GmatGlobal.hpp"
#include "MessageInterface.hpp"
#include <algorithm>                    // Required for GCC 4.3

//---------------------------------
// static members
//---------------------------------

GmatGlobal* GmatGlobal::theGmatGlobal = NULL;

const Integer GmatGlobal::DATA_PRECISION = 16;
const Integer GmatGlobal::TIME_PRECISION = 16;
const Integer GmatGlobal::DATA_WIDTH = 16;
const Integer GmatGlobal::TIME_WIDTH = 16;
const Integer GmatGlobal::INTEGER_WIDTH = 4;

//---------------------------------
// public methods
//---------------------------------

// // It needs for memory leak test      
//unsigned long GmatGlobal::GetUsedMemorySize(bool isAtStart)
//{
//   static unsigned long startLen, endLen;
//
//   // Get memory size used for this process
//   PROCESS_MEMORY_COUNTERS_EX pmc;
//   GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc), sizeof(pmc));
//   SIZE_T memSize = pmc.PrivateUsage;
//   
//   if (isAtStart)
//   {
//      startLen = memSize;
//      fprintf(repPtr, "Hi there 1: memory size = %d\n", startLen);
//   }
//   else
//   {
//      endLen = memSize;
//      fprintf(repPtr, "Hi there 2: memory size = %d    difference = %d\n", endLen, endLen - startLen);
//   }
//
//   return memSize;
//}


//------------------------------------------------------------------------------
// GmatGlobal* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain the singleton.
 *
 * @return the singleton instance of the GmatGlobal class.
 */
//------------------------------------------------------------------------------
GmatGlobal* GmatGlobal::Instance()
{
   if (!theGmatGlobal)
      theGmatGlobal = new GmatGlobal;
        
   return theGmatGlobal;
}

//------------------------------------------------------------------------------
// std::string GetGmatVersion()
//------------------------------------------------------------------------------
std::string GmatGlobal::GetGmatVersion()
{
   return gmatVersion;
}

//------------------------------------------------------------------------------
// bool IsGmatCompiledIn64Bit()
//------------------------------------------------------------------------------
bool GmatGlobal::IsGmatCompiledIn64Bit()
{
   bool building64bit = true;
   #ifdef _MSC_VER  // if Microsoft Visual C++
   #ifndef USE_64_BIT_LONGS
   building64bit = false;
   #endif
   #endif
   
   return building64bit;
}

//------------------------------------------------------------------------------
// Integer GetDataPrecision()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetDataPrecision()
{
   return currentSetting.mDataPrecision;
}

//------------------------------------------------------------------------------
// Integer GetTimePrecision()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetTimePrecision()
{
   return currentSetting.mTimePrecision;
}

//------------------------------------------------------------------------------
// Integer GetDataWidth()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetDataWidth()
{
   return currentSetting.mDataWidth;
}


//------------------------------------------------------------------------------
// Integer GetTimeWidth()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetTimeWidth()
{
   return currentSetting.mTimeWidth;
}


//------------------------------------------------------------------------------
// Integer GetIntegerWidth()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetIntegerWidth()
{
   return currentSetting.mIntegerWidth;
}


//------------------------------------------------------------------------------
// std::string GetOutputPath()
//------------------------------------------------------------------------------
std::string GmatGlobal::GetOutputPath()
{
   return currentSetting.mOutputPath;
}

//------------------------------------------------------------------------------
// void SetDataPrecision(Integer p)
//------------------------------------------------------------------------------
void GmatGlobal::SetDataPrecision(Integer p)
{
   currentSetting.mDataPrecision = p;
}

//------------------------------------------------------------------------------
// void SetTimePrecision(Integer p)
//------------------------------------------------------------------------------
void GmatGlobal::SetTimePrecision(Integer p)
{
   currentSetting.mTimePrecision = p;
}

//------------------------------------------------------------------------------
// void SetDataWidth(Integer w)
//------------------------------------------------------------------------------
void GmatGlobal::SetDataWidth(Integer w)
{
   currentSetting.mDataWidth = w;
}

//------------------------------------------------------------------------------
// void SetTimeWidth(Integer w)
//------------------------------------------------------------------------------
void GmatGlobal::SetTimeWidth(Integer w)
{
   currentSetting.mTimeWidth = w;
}

//------------------------------------------------------------------------------
// void SetIntegerWidth(Integer w)
//------------------------------------------------------------------------------
void GmatGlobal::SetIntegerWidth(Integer w)
{
   currentSetting.mIntegerWidth = w;
}

//------------------------------------------------------------------------------
// void SetOutputPath(const std::string &path)
//------------------------------------------------------------------------------
void GmatGlobal::SetOutputPath(const std::string &path)
{
   currentSetting.mOutputPath = path;
}

//------------------------------------------------------------------------------
// void SetMatlabFuncNameExt(const std::string &ext)
//------------------------------------------------------------------------------
void GmatGlobal::SetMatlabFuncNameExt(const std::string &ext)
{
   matlabExt = ext;
}

//------------------------------------------------------------------------------
// std::string GetMatlabFuncNameExt()
//------------------------------------------------------------------------------
std::string GmatGlobal::GetMatlabFuncNameExt()
{
   return matlabExt;
}

//------------------------------------------------------------------------------
// bool IsBatchMode()
//------------------------------------------------------------------------------
bool GmatGlobal::IsBatchMode()
{
   return isBatchMode;
}

//------------------------------------------------------------------------------
// void SetBatchMode(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetBatchMode(bool flag)
{
   isBatchMode = flag;
}

//------------------------------------------------------------------------------
// bool IsNitsClient()
//------------------------------------------------------------------------------
bool GmatGlobal::IsNitsClient()
{
   return isNitsClient;
}

//------------------------------------------------------------------------------
// void SetNitsClient(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetNitsClient(bool flag)
{
   isNitsClient = flag;
}

//------------------------------------------------------------------------------
// bool GetRunInterrupted()
//------------------------------------------------------------------------------
bool GmatGlobal::GetRunInterrupted()
{
   return runInterrupted;
}

//------------------------------------------------------------------------------
// void SetRunInterrupted(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetRunInterrupted(bool flag)
{
   runInterrupted = flag;
}

//------------------------------------------------------------------------------
// Gmat::RunState GetRunState()
//------------------------------------------------------------------------------
Gmat::RunState GmatGlobal::GetRunState()
{
   return runState;
}

//------------------------------------------------------------------------------
// void GmatGlobal::SetRunState(Gmat::RunState rs)
//------------------------------------------------------------------------------
void GmatGlobal::SetRunState(Gmat::RunState rs)
{
   #ifdef DEBUG_RUN_STATE
   MessageInterface::ShowMessage
      ("GmatGlobal::SetRunState() setting runstate to %d\n", rs);
   #endif
   runState = rs;
}

//------------------------------------------------------------------------------
// Gmat::RunState GetDetailedRunState()
//------------------------------------------------------------------------------
Gmat::RunState GmatGlobal::GetDetailedRunState()
{
   return detailedRunState;
}

//------------------------------------------------------------------------------
// void GmatGlobal::SetDetailedRunState(Gmat::RunState drs)
//------------------------------------------------------------------------------
void GmatGlobal::SetDetailedRunState(Gmat::RunState drs)
{
   #ifdef DEBUG_RUN_STATE
   MessageInterface::ShowMessage
      ("GmatGlobal::SetDetailedRunState() setting detailedRunstate to %d\n", drs);
   #endif
   detailedRunState = drs;
}

//------------------------------------------------------------------------------
// Integer GetRunMode()
//------------------------------------------------------------------------------
/**
 * Returns current run mode:
 *    NORMAL - Normal run
 *    EXIT_AFTER_RUN - GMAT closes after run complete
 *    TESTING - GMAT shows extra menu options for testing with plots
 *    TESTING_NO_PLOTS - GMAT shows extra menu options for testing without plots
 */
//------------------------------------------------------------------------------
Integer GmatGlobal::GetRunMode()
{
   return runMode;
}

//------------------------------------------------------------------------------
// void SetRunMode(Integer mode)
//------------------------------------------------------------------------------
void GmatGlobal::SetRunMode(Integer mode)
{
   runMode = mode;

   // It is a temporary fix in order to run gression test with runmode = TESTING. 
   // It needs to improve in the next GMAT release
   if (runMode == GmatGlobal::TESTING)
      isTesting = true;

   if (runMode == EXIT_AFTER_RUN)
      isBatchMode = true;
}

//------------------------------------------------------------------------------
// Integer GetGuiMode()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetGuiMode()
{
   return guiMode;
}

//------------------------------------------------------------------------------
// void SetGuiMode(Integer mode)
//------------------------------------------------------------------------------
void GmatGlobal::SetGuiMode(Integer mode)
{
   guiMode = mode;
}

//------------------------------------------------------------------------------
// Integer GetGuiMode()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetPlotMode()
{
   return plotMode;
}

//------------------------------------------------------------------------------
// void SetCommandEchoMode(bool tf)
//------------------------------------------------------------------------------
/**
 * Sets the command echo mode
 *
 * @param tf true to turn on command echo mde, false to turn it off
 */
//------------------------------------------------------------------------------
void GmatGlobal::SetCommandEchoMode(bool tf)
{
   commandEchoMode = tf;
}

//------------------------------------------------------------------------------
// bool EchoCommands()
//------------------------------------------------------------------------------
/**
 * Returns the current command echo mode
 *
 * @return true if the mode is set to echo commands to the message window
 */
//------------------------------------------------------------------------------
bool GmatGlobal::EchoCommands()
{
   return commandEchoMode;
}

//------------------------------------------------------------------------------
// void SetPlotMode(Integer mode)
//------------------------------------------------------------------------------
void GmatGlobal::SetPlotMode(Integer mode)
{
   plotMode = mode;
}

//------------------------------------------------------------------------------
// Integer GetMatlabMode()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetMatlabMode()
{
   return matlabMode;
}

//------------------------------------------------------------------------------
// bool IsMatlabAvailable()
//------------------------------------------------------------------------------
bool GmatGlobal::IsMatlabAvailable()
{
   return isMatlabAvailable;
}

//------------------------------------------------------------------------------
// void SetMatlabAvailable(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetMatlabAvailable(bool flag)
{
   isMatlabAvailable = flag;
}

//------------------------------------------------------------------------------
// bool IsMatlabDebugOn()
//------------------------------------------------------------------------------
bool GmatGlobal::IsMatlabDebugOn()
{
   return isMatlabDebugOn;
}

//------------------------------------------------------------------------------
// void SetMatlabDebug(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetMatlabDebug(bool flag)
{
   isMatlabDebugOn = flag;
}

//------------------------------------------------------------------------------
// void SetEventLocationAvailable(bool flag)
//------------------------------------------------------------------------------
/**
 * Sets the event locator flag
 *
 * @param flag Flag that is true if there are event locators, false if not
 */
//------------------------------------------------------------------------------
void GmatGlobal::SetEventLocationAvailable(bool flag)
{
   isEventLocationAvailable = flag;
}

//------------------------------------------------------------------------------
// bool IsEventLocationAvailable()
//------------------------------------------------------------------------------
/**
 * Returns the event locator available flag
 *
 * @return The flag
 */
//------------------------------------------------------------------------------
bool GmatGlobal::IsEventLocationAvailable()
{
   return isEventLocationAvailable;
}

//------------------------------------------------------------------------------
// void SetIncludeFoundInScriptResource(bool flag)
//------------------------------------------------------------------------------
/**
 * Sets the #Include statement found in the script resouce flag. Normally this
 * flag is set from the ScriptInterpreter and the ResourceTree retrieves it.
 *
 * @param flag Flag that is true if there are #Include statements, false if not
 */
//------------------------------------------------------------------------------
void GmatGlobal::SetIncludeFoundInScriptResource(bool flag)
{
   includeFoundInScriptResource = flag;
}

//------------------------------------------------------------------------------
// bool GetIncludeFoundInScriptResource()
//------------------------------------------------------------------------------
/**
 * Returns the #Include statement found in the script resouce flag. Normally this
 * flag is set from the ScriptInterpreter and the ResourceTree retrieves it.
 *
 * @return The flag
 */
//------------------------------------------------------------------------------
bool GmatGlobal::GetIncludeFoundInScriptResource()
{
   return includeFoundInScriptResource;
}

void GmatGlobal::SetWritePersonalizationFile(bool flag)
{
   writePersonalizationFile = flag;
}

bool GmatGlobal::GetWritePersonalizationFile()
{
   return writePersonalizationFile;
}

//------------------------------------------------------------------------------
// bool IsGUISavable()
//------------------------------------------------------------------------------
/**
 * Returns flag indicating whether GUI can be saved or not.
 */
//------------------------------------------------------------------------------
bool GmatGlobal::IsGUISavable()
{
   // Currently GUI cannot be saved when a main script contains #Include
   // before the BeginMissinSequence
   // Are there any other situations GUI cannot be saved?
   if (includeFoundInScriptResource)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// bool IsMissionTreeDebugOn()
//------------------------------------------------------------------------------
bool GmatGlobal::IsMissionTreeDebugOn()
{
   return isMissionTreeDebugOn;
}

//------------------------------------------------------------------------------
// void SetMissionTreeDebug(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetMissionTreeDebug(bool flag)
{
   isMissionTreeDebugOn = flag;
}

//------------------------------------------------------------------------------
// bool IsWritingParameterInfo()
//------------------------------------------------------------------------------
bool GmatGlobal::IsWritingParameterInfo()
{
   return isWritingParameterInfo;
}

//------------------------------------------------------------------------------
// void SetWriteParameterInfo(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetWriteParameterInfo(bool flag)
{
   isWritingParameterInfo = flag;
}

//------------------------------------------------------------------------------
// bool IsWritingFilePathInfo()
//------------------------------------------------------------------------------
bool GmatGlobal::IsWritingFilePathInfo()
{
   return isWritingFilePathInfo;
}

//------------------------------------------------------------------------------
// void SetWriteFilePathInfo(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetWriteFilePathInfo(bool flag)
{
   isWritingFilePathInfo = flag;
}

//------------------------------------------------------------------------------
// bool IsWritingGmatKeyword()
//------------------------------------------------------------------------------
bool GmatGlobal::IsWritingGmatKeyword()
{
   return isWritingGmatKeyword;
}

//------------------------------------------------------------------------------
// void SetWriteGmatKeyword(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetWriteGmatKeyword(bool flag)
{
   isWritingGmatKeyword = flag;
}

//------------------------------------------------------------------------------
// bool IsScientific()
//------------------------------------------------------------------------------
bool GmatGlobal::IsScientific()
{
   return actualFormat.mScientific;
}

//------------------------------------------------------------------------------
// bool ShowPoint()
//------------------------------------------------------------------------------
bool GmatGlobal::ShowPoint()
{
   return actualFormat.mShowPoint;
}

//------------------------------------------------------------------------------
// bool IsHorizontal()
//------------------------------------------------------------------------------
bool GmatGlobal::IsHorizontal()
{
   return actualFormat.mHorizontal;
}

//------------------------------------------------------------------------------
// bool IsBinaryIn()
//------------------------------------------------------------------------------
bool GmatGlobal::IsBinaryIn()
{
   return actualFormat.mBinaryIn;
}

//------------------------------------------------------------------------------
// bool IsBinaryOut()
//------------------------------------------------------------------------------
bool GmatGlobal::IsBinaryOut()
{
   return actualFormat.mBinaryOut;
}

//------------------------------------------------------------------------------
// Integer GetSpacing()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetSpacing()
{
   return currentFormat.mSpacing;
}

//------------------------------------------------------------------------------
// void SetScientific(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetScientific(bool flag)
{
   actualFormat.mScientific = flag;
}

//------------------------------------------------------------------------------
// void SetShowPoint(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetShowPoint(bool flag)
{
   actualFormat.mShowPoint = flag;
}

//------------------------------------------------------------------------------
// void SetHorizontal(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetHorizontal(bool flag)
{
   actualFormat.mHorizontal = flag;
}

//------------------------------------------------------------------------------
// void SetBinaryIn(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetBinaryIn(bool flag)
{
   actualFormat.mBinaryIn = flag;
}

//------------------------------------------------------------------------------
// void SetBinaryOut(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetBinaryOut(bool flag)
{
   actualFormat.mBinaryOut = flag;
}

//------------------------------------------------------------------------------
// void SetSpacing(Integer sp)
//------------------------------------------------------------------------------
void GmatGlobal::SetSpacing(Integer sp)
{
   actualFormat.mSpacing = sp;
}

//------------------------------------------------------------------------------
// void SetPrefix(const std::string &prefix)
//------------------------------------------------------------------------------
void GmatGlobal::SetPrefix(const std::string &prefix)
{
   actualFormat.mPrefix = prefix;
}

//------------------------------------------------------------------------------
// void SetAppendEol(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetAppendEol(bool flag)
{
   actualFormat.mAppendEol = flag;
}

//------------------------------------------------------------------------------
// void SetMatlabMode(Integer mode)
//------------------------------------------------------------------------------
void GmatGlobal::SetMatlabMode(Integer mode)
{
   matlabMode = mode;
   if (mode == NO_MATLAB)
      isMatlabAvailable = false;
}


//------------------------------------------------------------------------------
// void SetDefaultFormat()
//------------------------------------------------------------------------------
void GmatGlobal::SetDefaultFormat()
{
   defaultFormat.mScientific = false;
   defaultFormat.mShowPoint = false;
   defaultFormat.mWidth = DATA_WIDTH;
   defaultFormat.mPrecision = DATA_PRECISION;
   defaultFormat.mHorizontal = true;
   defaultFormat.mSpacing = 1;
   defaultFormat.mPrefix = "";
   defaultFormat.mAppendEol = true;
   defaultFormat.mBinaryIn = false;
   defaultFormat.mBinaryOut = false;
}


//------------------------------------------------------------------------------
// void SetCurrentFormat(bool scientific, bool showPoint, Integer width,
//                       Integer precision, bool horizontal, Integer spacing,
//                       const std::string &prefix, bool appendEol,
//                       bool binaryIn, bool binaryOut)
//------------------------------------------------------------------------------
/*
 * Sets current Real data input/output formatting.
 *
 * @param  scientific  Formats using scientific notation if true (false)
 * @param  showPoint  Formats using ios::showpoint if true (false)
 * @param  width  Width to be used in formatting (GmatGlobal::DATA_WIDTH)
 * @param  precision  Precision to be used in formatting (GmatGlobal::DATA_PRECISION)
 * @param  horizontal  Format horizontally if true (false)
 * @param  spacing  Spacing to be used in formatting (1)
 * @param  appendEol  Appends eol if true (true)
 * @param  binaryIn  Reads binary data if true (false)
 * @param  binaryOut  Writes binary data if true (false) 
 */
//------------------------------------------------------------------------------
void GmatGlobal::SetCurrentFormat(bool scientific, bool showPoint, Integer width,
                                  Integer precision, bool horizontal, Integer spacing,
                                  const std::string &prefix, bool appendEol,
                                  bool binaryIn, bool binaryOut)
{
   currentFormat.mScientific = scientific;
   currentFormat.mShowPoint = scientific;
   currentFormat.mWidth = width;
   currentFormat.mPrecision = precision;
   currentFormat.mHorizontal = horizontal;
   currentFormat.mSpacing = spacing;
   currentFormat.mPrefix = prefix;   
   currentFormat.mAppendEol = appendEol;   
   currentFormat.mBinaryIn = binaryIn;
   currentFormat.mBinaryOut = binaryOut;
}


//------------------------------------------------------------------------------
// void GetActualFormat(bool &scientific, bool &showPoint, Integer &precision,
//                      Integer &width, bool &horizontal, Integer &spacing,
//                      std::string &prefix, bool &appendEol)
//------------------------------------------------------------------------------
void GmatGlobal::GetActualFormat(bool &scientific, bool &showPoint,
                                 Integer &precision, Integer &width,
                                 bool &horizontal, Integer &spacing,
                                 std::string &prefix, bool &appendEol)
{
   scientific = actualFormat.mScientific;
   showPoint = actualFormat.mShowPoint;
   precision = actualFormat.mPrecision;
   width = actualFormat.mWidth;
   horizontal = actualFormat.mHorizontal;
   spacing = actualFormat.mSpacing;
   prefix = actualFormat.mPrefix;
   appendEol = actualFormat.mAppendEol;
}


//------------------------------------------------------------------------------
// void SetActualFormat(bool scientific, bool showPoint, Integer precision,
//                      Integer width, bool horizontal = false, Integer spacing = 1,
//                      const std::string &prefix = "", bool appendEol = true)
//------------------------------------------------------------------------------
void GmatGlobal::SetActualFormat(bool scientific, bool showPoint, Integer precision,
                                 Integer width, bool horizontal, Integer spacing,
                                 const std::string &prefix, bool appendEol)
{
   actualFormat.mScientific = scientific;
   actualFormat.mShowPoint = showPoint;
   actualFormat.mWidth = width;
   actualFormat.mPrecision = precision;
   actualFormat.mHorizontal = horizontal;
   actualFormat.mSpacing = spacing;
   actualFormat.mPrefix = prefix;
   actualFormat.mAppendEol = appendEol;
}

//------------------------------------------------------------------------------
// void SetToDefaultFormat()
//------------------------------------------------------------------------------
void GmatGlobal::SetToDefaultFormat()
{
   actualFormat = defaultFormat;
}

//------------------------------------------------------------------------------
// void SetToCurrentFormat()
//------------------------------------------------------------------------------
void GmatGlobal::SetToCurrentFormat()
{
   actualFormat = currentFormat;
}

//------------------------------------------------------------------------------
// EopFile* GetEopFile()
//------------------------------------------------------------------------------
EopFile* GmatGlobal::GetEopFile()
{
   return theEopFile;
}


//------------------------------------------------------------------------------
// ItrfCoefficientsFile* GetItrfCoefficientsFile()
//------------------------------------------------------------------------------
ItrfCoefficientsFile* GmatGlobal::GetItrfCoefficientsFile()
{
   return theItrfFile;
}


//------------------------------------------------------------------------------
// void SetEopFile(EopFile *eop)
//------------------------------------------------------------------------------
void GmatGlobal::SetEopFile(EopFile *eop)
{
   theEopFile = eop;
}


//------------------------------------------------------------------------------
// void SetItrfCoefficientsFile(ItrfCoefficientsFile *itrf)
//------------------------------------------------------------------------------
void GmatGlobal::SetItrfCoefficientsFile(ItrfCoefficientsFile *itrf)
{
   theItrfFile = itrf;
}

//------------------------------------------------------------------------------
// void SetLogfileSource(Integer src, const std::string logfileName)
//------------------------------------------------------------------------------
void GmatGlobal::SetLogfileSource(Integer src, const std::string logfileName)
{
   logfileSrc = src;
   if (logfileName != "")
   {
      if (src == GmatGlobal::CMD_LINE)
         cmdLineLog = logfileName;
      else if (src == GmatGlobal::SCRIPT)
         scriptLog  = logfileName;
      else if (src == GmatGlobal::STARTUP)
         startupLog = logfileName;
   }
}

//------------------------------------------------------------------------------
// void SetLogfileName(Integer forSrc, const std::string logfileName)
//------------------------------------------------------------------------------
void GmatGlobal::SetLogfileName(Integer forSrc, const std::string logfileName)
{
   if (forSrc == GmatGlobal::CMD_LINE)
      cmdLineLog = logfileName;
   else if (forSrc == GmatGlobal::SCRIPT)
      scriptLog  = logfileName;
   else if (forSrc == GmatGlobal::STARTUP)
      startupLog = logfileName;
}

//------------------------------------------------------------------------------
// Integer GetLogfileSource()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetLogfileSource()
{
   return logfileSrc;
}

//------------------------------------------------------------------------------
// std::string GetLogfileName(Integer forSrc)
//------------------------------------------------------------------------------
std::string GmatGlobal::GetLogfileName(Integer forSrc)
{
   Integer srcId = forSrc;
   if (forSrc == 0) // get current log file name
      srcId = GetLogfileSource();
   
   if (srcId == GmatGlobal::CMD_LINE)
      return cmdLineLog;
   else if (srcId == GmatGlobal::SCRIPT)
      return scriptLog;
   else if (srcId == GmatGlobal::STARTUP)
      return startupLog;
   else
      return "";
}


//------------------------------------------------------------------------------
// void  AddHiddenCommand(const std::string &cmd)
//------------------------------------------------------------------------------
/*
 * Add command to hide in menu
 *
 * @param  cmd  command name to be added
 */
//------------------------------------------------------------------------------
void GmatGlobal::AddHiddenCommand(const std::string &cmd)
{
   #ifdef DEBUG_HIDDEN_COMMAND
   MessageInterface::ShowMessage
      ("GmatGlobal::AddHiddenCommand() Adding %s to HiddenCommands\n",
       cmd.c_str());
   #endif

   mHiddenCommands.push_back(cmd);

   #ifdef DEBUG_HIDDEN_COMMAND
   std::list<std::string>::iterator pos;
   pos = mHiddenCommands.begin();
   while (pos != mHiddenCommands.end())
   {
      MessageInterface::ShowMessage
         ("   mHiddenCommands=%s\n",(*pos).c_str());
      ++pos;
   }
   #endif
}


//------------------------------------------------------------------------------
// const StringArray& GetHiddenCommands()
//------------------------------------------------------------------------------
const StringArray& GmatGlobal::GetHiddenCommands()
{
   mHiddenCommandsArray.clear();

   std::list<std::string>::iterator listpos = mHiddenCommands.begin();
   while (listpos != mHiddenCommands.end())
   {
      mHiddenCommandsArray.push_back(*listpos);
      ++listpos;
   }

   return mHiddenCommandsArray;
}


//------------------------------------------------------------------------------
// void ClearHiddenCommands()
//------------------------------------------------------------------------------
void GmatGlobal::ClearHiddenCommands()
{
   mHiddenCommands.clear();
}

//------------------------------------------------------------------------------
// void  IsHiddenCommand(const char *cmd)
//------------------------------------------------------------------------------
/*
 * returns true if this command should not be shown in menu
 *
 * @return  true to hide
 */
//------------------------------------------------------------------------------
bool GmatGlobal::IsHiddenCommand(const char *cmd)
{
   return IsHiddenCommand(std::string(cmd));
}

//------------------------------------------------------------------------------
// void  IsHiddenCommand(const std::string &cmd)
//------------------------------------------------------------------------------
/*
 * returns true if this command should not be shown in menu
 *
 * @return  true to hide
 */
//------------------------------------------------------------------------------
bool GmatGlobal::IsHiddenCommand(const std::string &cmd)
{
   std::list<std::string>::iterator pos =
      find(mHiddenCommands.begin(), mHiddenCommands.end(), cmd);
   return pos != mHiddenCommands.end();
}



//------------------------------------------------------------------------------
// void  RemoveHiddenCommand(const std::string &cmd)
//------------------------------------------------------------------------------
/*
 * Remove command so that it is shown in menu
 *
 * @param  cmd  command name to be removed
 */
//------------------------------------------------------------------------------
void GmatGlobal::RemoveHiddenCommand(const std::string &cmd)
{
   #ifdef DEBUG_HIDDEN_COMMAND
   MessageInterface::ShowMessage
      ("GmatGlobal::RemoveHiddenCommand() Removing %s to HiddenCommands\n",
       cmd.c_str());
   #endif

   std::list<std::string>::iterator pos =
      find(mHiddenCommands.begin(), mHiddenCommands.end(), cmd);
   if (pos != mHiddenCommands.end())
	mHiddenCommands.erase(pos);

   #ifdef DEBUG_HIDDEN_COMMAND
   pos = mHiddenCommands.begin();
   while (pos != mHiddenCommands.end())
   {
      MessageInterface::ShowMessage
         ("   mHiddenCommands=%s\n",(*pos).c_str());
      ++pos;
   }
   #endif
}

//------------------------------------------------------------------------------
// void SetSkipSplashMode(bool tfSplash)
//------------------------------------------------------------------------------
/**
* Sets the skip splash screen mode on startup
*
* @param tf true to turn on skip splash screen, false to turn it off
*/
//------------------------------------------------------------------------------
void GmatGlobal::SetSkipSplashMode(bool tfSplash)
{
   skipSplash = tfSplash;
}

//------------------------------------------------------------------------------
// bool SkipSplashMode()
//------------------------------------------------------------------------------
/**
* Returns the current skip splash screen mode
*
* @return true if the mode is set to not display the splash screen on startup
*/
//------------------------------------------------------------------------------
bool GmatGlobal::SkipSplashMode()
{
   return skipSplash;
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// GmatGlobal()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
GmatGlobal::GmatGlobal()
{
   // Current GMAT version.
   // @note: Make sure to switch it to the official release number for RC1
   gmatVersion = "R2018a";
   
   isTesting = false;                      // It is a temporary fix in order to run gression test with runmode = TESTING. 
                                           // It needs to improve in the next GMAT release

   isBatchMode                  = false;
   isNitsClient                 = false;
   runInterrupted               = false;
   isMatlabAvailable            = false;
   isMatlabDebugOn              = false;
   isEventLocationAvailable     = false;
   includeFoundInScriptResource = false;
   writePersonalizationFile     = true;
   isMissionTreeDebugOn         = false;
   isWritingParameterInfo       = false;
   isWritingFilePathInfo        = false;
   isWritingGmatKeyword         = true;
   commandEchoMode              = false;
   runMode = NORMAL;
   runState = Gmat::IDLE;
   guiMode = NORMAL_GUI;
   plotMode = NORMAL_PLOT;
   matlabMode = SHARED;
   matlabExt = "__m__";
   
   SetDefaultFormat();
   SetToDefaultFormat();
   
   // Logfile
   logfileSrc = GmatGlobal::STARTUP;
   cmdLineLog = "";
   scriptLog  = "";
   startupLog = "";


//   repPtr = fopen("rep.txt", "w");          // It needs for memory leak test
}


//------------------------------------------------------------------------------
// ~GmatGlobal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GmatGlobal::~GmatGlobal()
{
}

