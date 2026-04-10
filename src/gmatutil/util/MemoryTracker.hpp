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
 * Declares MemoryTracker class which tracks memory usage. It is a singleton class -
 * only one instance of this class can be created.
 */
//------------------------------------------------------------------------------
#ifndef MemoryTracker_hpp
#define MemoryTracker_hpp

#include "utildefs.hpp"

class GMATUTIL_API MemoryTracker
{
public:
   
   static MemoryTracker* Instance();
   
   void           SetScript(const std::string &script);
   void           SetShowTrace(bool show);
   void           Add(void *addr, const std::string &objName,
                      const std::string &funName, const std::string &note = "",
                      void *from = NULL);
   void           Remove(void *addr, const std::string &objName,
                         const std::string &funName, const std::string &note = "",
                         void *from = NULL);
   UnsignedInt    GetNumberOfTracks();
   StringArray&   GetTracks(bool clearTracks = false, bool writeScriptName = false);
   
private:
   
   static MemoryTracker *instance;
   
   struct TrackType
   {
      std::string preface;
      void *address;
      std::string objectName;
      std::string functionName;
      std::string remark;
      std::string scriptName;
      TrackType(const std::string &pref, void* addr, const std::string &objName,
                const std::string &funName, const std::string &note,
                const std::string &script)
         {
            preface = pref;
            address = addr;
            objectName = objName;
            functionName = funName;
            remark = note;
            scriptName = script;
         };
   };
   
   std::string scriptFile;
   std::vector<TrackType> memoryTracks;
   StringArray allTracks;
   bool showTrace;
   
   MemoryTracker();
   ~MemoryTracker();
   
};

#endif // MemoryTracker_hpp

