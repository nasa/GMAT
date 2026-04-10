//$Id$
//------------------------------------------------------------------------------
//                                  IFileUpdater
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
// Created: 2016.01.05
//
/**
 * Definition of the IFileUpdater class.  This is the virtual class/interface
 * for a class that updates data files from a repository
 *
 */
//------------------------------------------------------------------------------

#ifndef IFileUpdater_hpp
#define IFileUpdater_hpp

#include "utildefs.hpp"
#include "Date.hpp"
#include "FileManager.hpp"

// forward references

class GMATUTIL_API IFileUpdater
{
public:
   static const std::string ShellFile;

   // note that there are other status like unversioned (local but not on server), but the fileupdate
   // should not report these files
   enum filestatus
   {     
      NOMINAL,             // no changes between local and server
      ADDED,               // added on server
      DELETED,             // deleted on server
      LOCALMODIFIED,       // modified locally
      LOCALMISSING,        // not found locally (not an error in our case)
      MODIFIED             // modified on server (newer)

   };
   enum fileop
   {     
      NONE,
      UPDATE,
      REVERT
   };

   struct repoFile {
      std::string name;
      filestatus status;
      fileop operation;
      std::string version;
      std::string comment;
      FileManager::FileType fileType;
      //Date filedate;
   };

   IFileUpdater(std::string location, std::string server) { mLocation = location; mServer = server; }
   virtual ~IFileUpdater() {}
   // check for updates and fill the files vector
   virtual bool CheckForUpdates() = 0;
   // return the list of files that have changed
   const std::vector<repoFile>& GetFiles() { return files; }
   // set the list of files to be updated
   virtual void SetFiles( const std::vector<repoFile> aList ) { files = aList; }
   // save the update files script to file to be executed on startup
   virtual bool SaveUpdateScript( const std::string &filename ) = 0;
   std::string GetLocation() { return mLocation; }
   std::string GetServer() { return mServer; }

protected:
   std::vector<repoFile> files;
   std::string mLocation;
   std::string mServer;
};

#endif // IFileUpdater_hpp
