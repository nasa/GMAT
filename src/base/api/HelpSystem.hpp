//------------------------------------------------------------------------------
//                           HelpSystem
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Oct 23, 2018
/**
 * Help system accessible for the GMAT API
 */
//------------------------------------------------------------------------------

#ifndef SRC_BASE_API_HELPSYSTEM_HPP_
#define SRC_BASE_API_HELPSYSTEM_HPP_

#include "gmatdefs.hpp"
#include "Moderator.hpp"

#include <string>
#include <sstream>
#include <map>

/**
 * \ingroup API
 * @brief The manager for the API help system
 */
class GMAT_API HelpSystem
{
public:
   static HelpSystem* Instance(
         const std::string &helpfile = "");
   std::string Help(std::string forItem = "");

private:
   /// The singleton
   static HelpSystem* instance;
   Moderator *theModerator;

//   std::map<std::string, std::string> itemHelp;
   // Lines of help for each topic
   std::map<std::string, StringArray> itemHelp;

   /// Registered objects
   StringArray knownClasses;

   HelpSystem(const std::string &helpfile);
   virtual ~HelpSystem();

   void BuildHelp(const std::string &helpfile);
   std::string UpdateHelp(const StringArray &helpText);
   void ProcessOnlineCommentBlock(StringArray &onlineCode, std::stringstream &text);
};

#endif /* SRC_BASE_API_HELPSYSTEM_HPP_ */
