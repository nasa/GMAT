//------------------------------------------------------------------------------
//                                  CommandEcho
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Joshua J. Raymond
// Created: June 23, 2017
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
* Class implementation for the CommandEcho command
*/
//------------------------------------------------------------------------------


#ifndef CommandEcho_hpp
#define CommandEcho_hpp

#include "ScriptToolsDefs.hpp"
#include "GmatCommand.hpp"
#include <fstream>

/**
* Command used to toggle commands being displayed in the message window
* window during a mission run.
*/
class ScriptTools_API CommandEcho : public GmatCommand
{
public:
   CommandEcho();
   virtual ~CommandEcho();
   CommandEcho(const CommandEcho& commandEcho);
   CommandEcho& operator=(const CommandEcho& commandEcho);
   bool SettingInput(std::string);

   // inherited from GmatCommand
   virtual bool         Execute();
   virtual bool         InterpretAction();
   virtual void         RunComplete();

   // inherited from GmatBase
   virtual bool         Initialize();
   virtual GmatBase*    Clone() const;

   // Parameter accessors
   virtual bool         RenameRefObject(const UnsignedInt type,
      const std::string &oldName,
      const std::string &newName);

   virtual const std::string&
      GetGeneratingString(Gmat::WriteMode mode,
      const std::string &prefix = "",
      const std::string &useName = "");

   DEFAULT_TO_NO_CLONES

protected:
   // Parameter IDs

   /// Paramert to keep track of whether CommandEcho is ON (true) or OFF (false)
   bool echoStatus;
   /// Parameter to convert ON and OFF to true and false through debugStatus
   std::string echoSetting;
   //std::string commandRename;
   /// Parameter to save initial setting on echo command before CommandEcho
   /// is used
   bool initval;
};

#endif // CommandEcho_hpp