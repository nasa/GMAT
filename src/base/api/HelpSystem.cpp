//$Id$
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
 * 
 */
//------------------------------------------------------------------------------

#include "HelpSystem.hpp"
#include "FileUtil.hpp"
#include "FileManager.hpp"
#include "StringUtil.hpp"
#include "GmatBase.hpp"

//#define DEBUG_HELP_LOADING

#ifdef DEBUG_HELP_LOADING
   #include <iostream>
#endif


HelpSystem* HelpSystem::instance = NULL;

//------------------------------------------------------------------------------
// HelpSystem* Instance()
//------------------------------------------------------------------------------
/**
 * Singleton access method
 *
 * @return The help system singleton
 */
//------------------------------------------------------------------------------
HelpSystem* HelpSystem::Instance(const std::string &helpfile)
{
   if (instance == NULL)
   {
      std::string theHelp = helpfile;

      if (helpfile == "")
      {
         FileManager *fileman = FileManager::Instance();
         std::string datapath = fileman->GetAbsPathname("DATA_PATH");

         theHelp = datapath + "/api/GmatHelp.rst";
      }

      instance = new HelpSystem(theHelp);
   }

   return instance;
}


//------------------------------------------------------------------------------
// std::string Help(std::string forItem)
//------------------------------------------------------------------------------
/**
 * Accesses help for a topic
 *
 * @param forItem The help topic requested
 *
 * @return The system help for the topic, if available
 */
//------------------------------------------------------------------------------
std::string HelpSystem::Help(std::string forItem)
{
   std::string retval = "The item " + forItem +
                        " does not have help in the help system\n";

   if (forItem == "")
      retval = UpdateHelp(itemHelp["TopLevel"]);
   else
   {
      if (itemHelp.find(forItem) != itemHelp.end())
         retval = UpdateHelp(itemHelp[forItem]);
      else
      {
         GmatBase *forObject = theModerator->GetConfiguredObject(forItem);
         if (forObject)
            retval = forObject->Help();
      }
   }

   return retval;
}

// Hidden methods

//------------------------------------------------------------------------------
// HelpSystem(const std::string &helpfile)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param helpfile File name and path for the file containing help text
 */
//------------------------------------------------------------------------------
HelpSystem::HelpSystem(const std::string &helpfile)
{
   theModerator = Moderator::Instance();
   // Ensure GMAT is initialized
   if (!theModerator->IsInitialized())
      theModerator->Initialize("gmat_startup_file.txt");

   BuildHelp(helpfile);
}

//------------------------------------------------------------------------------
// ~HelpSystem()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
HelpSystem::~HelpSystem()
{
}


//------------------------------------------------------------------------------
// void HelpSystem::BuildHelp(const std::string &helpfile)
//------------------------------------------------------------------------------
/**
 * Reads the help from the help file and builds the underlying map
 *
 * @param helpfile The file containing help text.  The default is
 *        data/api/GmatHelp.rst
 */
//------------------------------------------------------------------------------
void HelpSystem::BuildHelp(const std::string &helpfile)
{
   StringArray helpData = GmatFileUtil::GetTextLines(helpfile);

   #ifdef DEBUG_HELP_LOADING
      std::cout << "helpFile " << helpfile << " contains " << helpData.size() << " lines\n";

      std::cout << "First 10:\n";
      for(Integer i = 0; i < 10; ++i)
         std::cout << i << ":  '" << helpData[i] << "'\n";
   #endif

   std::string keyword;
   StringArray text;

   bool buildingText = false;
   bool newKeyword = false;
   bool onlineComment = false;
   StringArray onlineCode;
   size_t kwstart, kwend;

   for (UnsignedInt i = 0; i < helpData.size(); ++i)
   {
      kwstart = helpData[i].find(".. _");
      if (kwstart != std::string::npos)
      {
         kwend = helpData[i].find(":");
         if (kwend != std::string::npos)
         {
            if (buildingText)
            {
               itemHelp[keyword] = text;
               text.clear();
            }
            newKeyword = true;
            keyword = helpData[i].substr(kwstart + 4, (kwend - kwstart - 4));

            #ifdef DEBUG_HELP_LOADING
               std::cout << "Found keyword " << keyword << "\n";
            #endif

            continue;
         }
      }

      if (newKeyword)
      {
         newKeyword = false;
         buildingText = true;
      }

      if (buildingText)
         text.push_back(helpData[i]);
   }

   if (buildingText)
      itemHelp[keyword] = text;
}


//------------------------------------------------------------------------------
// std::string UpdateHelp(const StringArray &helpText)
//------------------------------------------------------------------------------
/**
 * Process live updates for the online help system
 *
 * @param helpText The topical help that may need updated text
 *
 * @return A string with the processed topical help
 */
//------------------------------------------------------------------------------
std::string HelpSystem::UpdateHelp(const StringArray &helpText)
{
   std::stringstream theHelp;
   bool onlineComment = false;
   StringArray onlineCode;

   for (UnsignedInt i = 0; i < helpText.size(); ++i)
   {
      // Check for an "Online" comment, indicating text only seen online
      if (helpText[i].find(".. <Online>") != std::string::npos)
      {
         onlineComment = true;
         onlineCode.clear();
         continue;
      }
      if (onlineComment)
      {
         if ((helpText[i].find("   ", 0) == 0) || (helpText[i] == ""))
         {
            onlineCode.push_back(helpText[i]);
            continue;
         }
         else
         {
            ProcessOnlineCommentBlock(onlineCode, theHelp);
            onlineComment = false;
            theHelp << helpText[i] << "\n";
         }
      }
      else
         theHelp << helpText[i] << "\n";
   }

   if (onlineComment)
      ProcessOnlineCommentBlock(onlineCode, theHelp);

   return theHelp.str();
}


//------------------------------------------------------------------------------
// void ProcessOnlineCommentBlock(StringArray &onlineCode, std::stringstream &text)
//------------------------------------------------------------------------------
/**
 * Adds data for the running system to the help text
 *
 * @param onlineCode Text and directives to be added to the online help.
 */
//------------------------------------------------------------------------------
void HelpSystem::ProcessOnlineCommentBlock(StringArray &onlineCode,
      std::stringstream &text)
{
   #ifdef DEBUG_HELP_LOADING
      if (!theModerator)
         std::cout << "The Moderator is NULL" << "\n\n";
      else
         std::cout << "The Moderator is "
                   << (theModerator->IsInitialized() ? "" : "not ")
                   << "initialized\n\n";
   #endif

   for (UnsignedInt i = 0; i < onlineCode.size(); ++i)
   {
      std::string line;

      if (onlineCode[i].length() > 3)
         line = onlineCode[i].substr(3);
      else
         line = onlineCode[i];

      // Check for directives, identified by angle brackets
      size_t abstart = line.find("<");
      size_t abend = line.find(">") - abstart;
      if ((abstart != std::string::npos) && (abend != std::string::npos))
      {
         std::string command, type;
         command = line.substr(abstart + 1, abend - 1);
         StringArray chunks = GmatStringUtil::SeparateBy(command, " ");
         if (chunks.size() > 1)
         {
            command = chunks[0];
            type = chunks[1];
         }

         #ifdef DEBUG_HELP_LOADING
            std::cout << "-------------------------\n\nDirective: " << command
                      << " Argument " << type << "\n\n";
         #endif

         // @todo: Process help directives
         if (command == "CLASSLIST")    // List available subclasses
         {
            UnsignedInt typeID = GmatBase::GetObjectType(type);

            #ifdef DEBUG_HELP_LOADING
               std::cout << "\n"<< "ITEM TYPE IS " << typeID << "\n";
            #endif

            StringArray classes;
            classes = theModerator->GetListOfFactoryItems(typeID);

            #ifdef DEBUG_HELP_LOADING
               std::cout << "-------------------------\n\nFound "
                         << classes.size() << " classes\n\n";
            #endif

            line = "";
            for (UnsignedInt j = 0; j < classes.size(); ++j)
               line += "   " + classes[j] + "\n";
         }

         // Process help directives
         if (command == "OBJECTLIST")   // List objects of a set type
         {
            StringArray objects = theModerator->GetListOfObjects(type);
            line = "";

            if (objects.size() == 0)
               line += "   <None>\n";
            else
               for (UnsignedInt j = 0; j < objects.size(); ++j)
                  line += "   " + objects[j] + "\n";
         }
      }

      text << line << "\n";
   }
}
