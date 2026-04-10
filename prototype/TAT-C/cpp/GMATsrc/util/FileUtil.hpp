//$Id$
//------------------------------------------------------------------------------
//                                 FileUtil
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
// Created: 2005/12/12
//
/**
 * This file provides methods to query file information and methods to compare
 * two output files. The compare summary is written to the log file.
 */
//------------------------------------------------------------------------------
#ifndef FileUtil_hpp
#define FileUtil_hpp

#include "utildefs.hpp"
#include <fstream>

namespace GmatFileUtil
{
   const Integer BUFFER_SIZE = 4096;
   static Real   COMPARE_TOLERANCE = 1.0e-9;
   
   std::string GMATUTIL_API GetPathSeparator();
   std::string GMATUTIL_API ConvertToOsFileName(const std::string &fileName);
   std::string GMATUTIL_API GetCurrentWorkingDirectory();
   bool        GMATUTIL_API SetCurrentWorkingDirectory(const std::string &newDir);
   std::string GMATUTIL_API GetApplicationPath();
   std::string GMATUTIL_API GetTemporaryDirectory();
   std::string GMATUTIL_API ParseFirstPathName(const std::string &fullPath, bool appendSep = true);
   std::string GMATUTIL_API ParsePathName(const char *fullPath, bool appendSep = true);
   std::string GMATUTIL_API ParsePathName(const std::string &fullPath, bool appendSep = true);
   std::string GMATUTIL_API ParseFileName(const char *fullPath, bool removeExt = false);
   std::string GMATUTIL_API ParseFileName(const std::string &fullPath, bool removeExt = false);
   std::string GMATUTIL_API ParseFileExtension(const char *fullPath, bool prependDot = false);
   std::string GMATUTIL_API ParseFileExtension(const std::string &fullPath, bool prependDot = false);
   std::string GMATUTIL_API GetInvalidFileNameMessage(Integer option = 1);
   
   bool GMATUTIL_API IsOsWindows();
   bool GMATUTIL_API IsPathRelative(const char *fullPath);
   bool GMATUTIL_API IsPathRelative(const std::string &fullPath);
   bool GMATUTIL_API IsPathAbsolute(const std::string &fullPath);
   bool GMATUTIL_API HasNoPath(const std::string &fullPath);
   bool GMATUTIL_API IsValidFileName(const std::string &fname, bool isBlankOk = true);
   bool GMATUTIL_API IsSameFileName(const char *fname1, const char *fname2);
   bool GMATUTIL_API IsSameFileName(const std::string &fname1, const std::string &fname2);
   bool GMATUTIL_API DoesDirectoryExist(const char *dirPath, bool isBlankOk = true);
   bool GMATUTIL_API DoesDirectoryExist(const std::string &dirPath, bool isBlankOk = true);
   bool GMATUTIL_API DoesFileExist(const char* filename);
   bool GMATUTIL_API DoesFileExist(const std::string &filename);
   bool GMATUTIL_API GetLine(std::istream *inStream, std::string &line);
   bool GMATUTIL_API IsAppInstalled(const std::string &appName, std::string &appLoc);

   std::string GetGmatPath();
   
   WrapperTypeArray GMATUTIL_API
      GetFunctionOutputTypes(std::istream *is, const StringArray &inputs,
                             const StringArray &outputs, std::string &errMsg,
                             IntegerArray &outputRows, IntegerArray &outputCols);
   
   StringArray GMATUTIL_API GetFileListFromDirectory(const std::string &dirName,
                                                 bool addPath = false);
   StringArray GMATUTIL_API GetTextLines(const std::string &fileName);

   bool                 PrepareCompare(Integer numDirsToCompare,
                           const std::string &basefilename,
                           const std::string &filename1,
                           const std::string &filename2,
                           const std::string &filename3,
                           std::ifstream &baseIn, std::ifstream &in1,
                           std::ifstream &in2, std::ifstream &in3);
   
   bool GMATUTIL_API        CompareLines(const std::string &line1,
                           const std::string &line2, Real &diff,
                           Real tol = COMPARE_TOLERANCE);
   
   StringArray GMATUTIL_API &CompareTextLines(Integer numDirsToCompare,
                           const char *basefilename,
                           const char *filename1,
                           const char *filename2,
                           const char *filename3,
                           int &file1DiffCount, int &file2DiffCount,
                           int &file3DiffCount, bool skipBlankLines = false);
   
   StringArray GMATUTIL_API &CompareTextLines(Integer numDirsToCompare,
                           const std::string &basefilename,
                           const std::string &filename1,
                           const std::string &filename2,
                           const std::string &filename3,
                           int &file1DiffCount, int &file2DiffCount,
                           int &file3DiffCount, bool skipBlankLines = false);
   
   StringArray GMATUTIL_API &CompareNumericLines(Integer numDirsToCompare,
                           const char *basefilename,
                           const char *filename1,
                           const char *filename2,
                           const char *filename3,
                           int &file1DiffCount, int &file2DiffCount,
                           int &file3DiffCount, Real tol = COMPARE_TOLERANCE);
   
   StringArray GMATUTIL_API &CompareNumericLines(Integer numDirsToCompare,
                           const std::string &basefilename,
                           const std::string &filename1,
                           const std::string &filename2,
                           const std::string &filename3,
                           int &file1DiffCount, int &file2DiffCount,
                           int &file3DiffCount, Real tol = COMPARE_TOLERANCE);
   
   StringArray GMATUTIL_API &CompareNumericColumns(Integer numDirsToCompare,
                           const char *basefilename,
                           const char *filename1,
                           const char *filename2,
                           const char *filename3,
                           Real tol = COMPARE_TOLERANCE);
   
   StringArray GMATUTIL_API &CompareNumericColumns(Integer numDirsToCompare,
                           const std::string &basefilename,
                           const std::string &filename1,
                           const std::string &filename2,
                           const std::string &filename3,
                           Real tol = COMPARE_TOLERANCE);
   
   bool GMATUTIL_API SkipHeaderLines(std::ifstream &in, RealArray &realArray,
                                 const std::string &filename);
   bool GMATUTIL_API IsAsciiFile(std::ifstream &file, const std::string &filename);
   bool GMATUTIL_API GetRealColumns(const std::string &line, RealArray &cols);
   
   static StringArray textBuffer;

}

#endif // FileUtil_hpp
