//$Id$
//------------------------------------------------------------------------------
//                                  FileUpdaterSVN
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Thomas G. Grubb/GSFC
// Created: 2016.01.12
//
/**
 * Definition of the FileUpdaterSVN class.  This is the SVN implementation
 * for a class that updates data files from a repository
 *
 */
//------------------------------------------------------------------------------

#ifndef FileUpdaterSVN_hpp
#define FileUpdaterSVN_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "IFileUpdater.hpp"
#include <fstream>          // for std::ofstream
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "FileUtil.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"

// forward references

class GMAT_API FileUpdaterSVN : IFileUpdater
{
public:
   FileUpdaterSVN(std::string location, std::string server);
   virtual ~FileUpdaterSVN();
   virtual bool CheckForUpdates();
   virtual void CopyVersionedFiles();
   virtual bool SaveUpdateScript( const std::string &filename );

protected:
   bool Finalize();
   bool Initialize();
   bool ExecuteCheck(const std::string &fileName);
   bool ParseUpdateCheck(const std::string &fileName);
};

#endif // FileUpdaterSVN_hpp