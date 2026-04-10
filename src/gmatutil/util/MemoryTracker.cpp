//$Id$
//------------------------------------------------------------------------------
//                                 MemoryTracker
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
// Created: 2008/11/21
//
/**
 * Implements MemoryTracker class which tracks memory usage. It is a singleton class -
 * only one instance of this class can be created.
 */
//------------------------------------------------------------------------------

#include "MemoryTracker.hpp"
#include "MessageInterface.hpp"
#include <stdio.h>                 // for sprintf()

//--------------------------------------
//  initialize static variables
//--------------------------------------
MemoryTracker* MemoryTracker::instance = NULL;

//------------------------------------------------------------------------------
// MemoryTracker* Instance()
//------------------------------------------------------------------------------
MemoryTracker* MemoryTracker::Instance()
{
   if (instance == NULL)
      instance = new MemoryTracker;
   
   return instance;
}


//------------------------------------------------------------------------------
// void SetScript(const std::string &script)
//------------------------------------------------------------------------------
void MemoryTracker::SetScript(const std::string &script)
{
   scriptFile = script;
}


//------------------------------------------------------------------------------
// void SetShowTrace(bool show)
//------------------------------------------------------------------------------
void MemoryTracker::SetShowTrace(bool show)
{
   showTrace = show;
}


//------------------------------------------------------------------------------
// void Add(void *addr, const std::string &objName, const std::string &funName,
//          const std::string &note, void *from)
//------------------------------------------------------------------------------
void MemoryTracker::Add(void *addr, const std::string &objName,
                        const std::string &funName, const std::string &note,
                        void *from)
{
   if (showTrace)
   {
      if (from == NULL)
      {
         MessageInterface::ShowMessage
            ("+++ Creating <%p> %-20s in %s  %s\n", addr, objName.c_str(),
             funName.c_str(), note.c_str());
      }
      else
      {
         MessageInterface::ShowMessage
            ("+++ Creating <%p> %-20s in %s  %s from <%p>\n", addr, objName.c_str(),
             funName.c_str(), note.c_str(), from);
      }
   }
   
   TrackType track("+++", addr, objName, funName, note, scriptFile);
   memoryTracks.push_back(track);
}


//------------------------------------------------------------------------------
// void Remove(void *addr, const std::string &objName, const std::string &funName,
//             const std::string &note, void *from)
//------------------------------------------------------------------------------
void MemoryTracker::Remove(void *addr, const std::string &objName,
                           const std::string &funName, const std::string &note,
                           void *from)
{
   if (showTrace)
   {
      if (from == NULL)
      {
         MessageInterface::ShowMessage
            ("--- Deleting <%p> %-20s in %s %s\n", addr, objName.c_str(),
             funName.c_str(), note.c_str());
      }
      else
      {
         MessageInterface::ShowMessage
            ("--- Deleting <%p> %-20s in %s %s from <%p>\n", addr, objName.c_str(),
             funName.c_str(), note.c_str(), from);
      }
   }
   
   bool trackFound = false;
   std::vector<TrackType>::iterator track = memoryTracks.begin();
   while (track != memoryTracks.end())
   {
      if ((*track).address == addr)
      {
         memoryTracks.erase(track);
         trackFound = true;
         break;
      }
      
      ++track;
   }
   
   if (!trackFound)
   {
      TrackType track("---", addr, objName, funName, note, scriptFile);
      memoryTracks.push_back(track);
   }
   
}


//------------------------------------------------------------------------------
// UnsignedInt GetNumberOfTracks()
//------------------------------------------------------------------------------
UnsignedInt MemoryTracker::GetNumberOfTracks()
{
   return memoryTracks.size();
}


//------------------------------------------------------------------------------
// StringArray& GetTracks(bool clearTracks, bool justGetSize, bool writeScriptName)
//------------------------------------------------------------------------------
/**
 * Returns memory tracks
 *
 * @param  clearTracks  Clears track if set to true [true]
 * @param  writeScriptName  Adds script name to track record
 */
//------------------------------------------------------------------------------
StringArray& MemoryTracker::GetTracks(bool clearTracks, bool writeScriptName)
{
   allTracks.clear();
   std::vector<TrackType>::iterator track = memoryTracks.begin();
   static char text[400];
   Integer trackCount = 0;

   #ifdef DEBUG_GET_TRACKS
   MessageInterface::ShowMessage
      ("===> MemoryTracker::GetTracks() There are %d memory tracks\n",
       memoryTracks.size());
   #endif
   
   while (track != memoryTracks.end() && trackCount < 1000)
   {
      trackCount++;
      std::string script = "";
      if (writeScriptName)
         script = (*track).scriptName;
      sprintf(text, "%s <%p> %-20s %-s  %s %s", ((*track).preface).c_str(), (*track).address,
              ((*track).objectName).c_str(), ((*track).functionName).c_str(),
              ((*track).remark).c_str(), script.c_str());
      allTracks.push_back(text);
      ++track;
   }
   
   if (clearTracks)
      memoryTracks.clear();
   
   #ifdef DEBUG_GET_TRACKS
   MessageInterface::ShowMessage
      ("===> MemoryTracker::GetTracks() returning %d memory tracks\n", allTracks.size());
   #endif
   
   return allTracks;
}


//------------------------------------------------------------------------------
//  MemoryTracker()
//------------------------------------------------------------------------------
MemoryTracker::MemoryTracker()
{
   showTrace = false;
}

//------------------------------------------------------------------------------
//  ~MemoryTracker()
//------------------------------------------------------------------------------
MemoryTracker::~MemoryTracker()
{
   memoryTracks.clear();
}
