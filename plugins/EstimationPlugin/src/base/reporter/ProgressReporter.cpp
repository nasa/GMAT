//------------------------------------------------------------------------------
//                           ProgressReporter
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 29, 2014
/**
 * Reporter for reports of progress data from the GMAT Navigation system
 */
//------------------------------------------------------------------------------

#include "ProgressReporter.hpp"
#include "MessageInterface.hpp"


ProgressReporter::ProgressReporter(const std::string firstLine,
      const std::string &file) :
   filename                   (file),
   bufferTrigger              (16383),    // Write when buffer.length() >= 16k
   report                     (NULL),
   writeToMessageInterface    (false),
   logLevel                   (1),         // Default to verbose
   logLevelDescriptor         ("Verbose")
{
   buffer = firstLine + "\n";

   levelDescriptors.push_back("Everything");
   levelDescriptors.push_back("Verbose");
   levels["Everything"] = 0;              // Everything is a debug mode
   levels["Verbose"] = 1;                 // Verbose writes a lot of data, but
                                          // not tons of core data
   levels["NotFound"] = 32767;            // 32k-1 for not found
}


ProgressReporter::~ProgressReporter()
{
   if (report)
   {
      if (report->is_open())
         report->close();
      delete report;
   }
}


ProgressReporter::ProgressReporter(const ProgressReporter& pr) :
   filename                   (pr.filename),
   buffer                     (pr.buffer),
   bufferTrigger              (pr.bufferTrigger),
   report                     (NULL),
   writeToMessageInterface    (pr.writeToMessageInterface),
   logLevel                   (pr.logLevel),
   levels                     (pr.levels),
   logLevelDescriptor         (pr.logLevelDescriptor),
   levelDescriptors           (pr.levelDescriptors)
{
}


ProgressReporter& ProgressReporter::operator=(const ProgressReporter& pr)
{
   if (this != &pr)
   {
      filename                = pr.filename;
      buffer                  = pr.buffer;
      bufferTrigger           = pr.bufferTrigger;
      report                  = NULL;
      writeToMessageInterface = pr.writeToMessageInterface;
      logLevel                = pr.logLevel;
      levels                  = pr.levels;
      logLevelDescriptor      = pr.logLevelDescriptor;
      levelDescriptors        = pr.levelDescriptors;
   }

   return *this;
}

void ProgressReporter::Initialize()
{
   if (filename == "")
      writeToMessageInterface = true;
   else
   {
      report = new std::ofstream();
      report->open(filename.c_str());
   }
}

void ProgressReporter::Finalize()
{
   Flush();

   // If writing to file, close the file handle and delete the stream
   if (report)
   {
      if (report->is_open())
         report->close();
      delete report;
      report = NULL;
   }

}

bool ProgressReporter::WriteData(std::string dataToWrite)
{
   bool retval = false;

   {
      buffer += dataToWrite + "\n";

      if (buffer.length() > bufferTrigger)
         retval = WriteBuffer();
      else
         retval = true;
   }

   return retval;
}

bool ProgressReporter::WriteData(const std::string& label, const Real value,
      const std::string& preface, const Integer depth)
{
   bool retval = false;

   return retval;
}

bool ProgressReporter::WriteData(const StringArray& labels,
      const RealArray& values, const std::string& preface, const Integer depth)
{
   bool retval = false;

   return retval;
}

bool ProgressReporter::WriteDataArray(const StringArray& labels,
      std::vector<RealArray*>& values, const std::string& preface,
      const Integer depth)
{
   bool retval = false;

   return retval;
}

bool ProgressReporter::WriteBuffer()
{
   bool retval = false;

   if (report != NULL)
   {
      (*report) << buffer;
      retval = true;
   }

   if (writeToMessageInterface)
   {
      MessageInterface::ShowMessage(buffer.c_str());
      if (report == NULL)
         retval = true;
   }

   if (retval)
      buffer = "";

   return retval;
}

UnsignedInt ProgressReporter::AddLogLevel(const std::string& newLevel)
{
   levelDescriptors.push_back(newLevel);
   levels[newLevel] = levelDescriptors.size();

   return levels[newLevel];
}

//UnsignedInt ProgressReporter::GetLogLevel()
//{
//   return logLevel;
//}

UnsignedInt ProgressReporter::GetLogLevel(const std::string& forDescriptor)
{
   UnsignedInt retval = logLevel;      // Default to global level

   if (forDescriptor != "")
   {
      if (subsystemLogLevel.count(forDescriptor) > 0)
         retval = subsystemLogLevel[forDescriptor];
   }

   return retval;
}

void ProgressReporter::SetLogLevel(UnsignedInt newLevel, const std::string & forType)
{
   if (levels.count(GetLogLevelDescriptor(newLevel)) > 0)
   {
      if (forType == "")
         // Set the global level
         logLevel = newLevel;
      else
      {

            MessageInterface::ShowMessage("Setting logging level to %d for %s\n",
                  newLevel, forType.c_str());

         subsystemLogLevel[forType] = newLevel;
      }
   }
}

void ProgressReporter::SetLogLevel(const std::string &levelName, const std::string & forType)
{

      MessageInterface::ShowMessage("SetLogLevel(%s, %s) called\n",
            levelName.c_str(), forType.c_str());

   if (levelName != "NotFound")
   {
      if (levels.count(levelName) > 0)
         SetLogLevel(levels[levelName], forType);
//      else
//         throw MeasuremetnException("Bad level setting")
   }
}

//std::string ProgressReporter::GetLogLevelDescriptor()
//{
//   return logLevelDescriptor;
//}

std::string ProgressReporter::GetLogLevelDescriptor(UnsignedInt forLevel)
{
   std::string retval = "NotFound";

   for (std::map <std::string, UnsignedInt>::iterator i = levels.begin();
         i != levels.end(); ++i)
   {
      if (i->second == forLevel)
         retval = i->first;
   }

   return retval;
}

bool ProgressReporter::Flush()
{
   bool retval = false;

   if (report != NULL)
   {
      (*report) << buffer;
      retval = true;
   }

   if (writeToMessageInterface)
   {
      MessageInterface::ShowMessage(buffer.c_str());
      if (report == NULL)
         retval = true;
   }

   if (retval)
      buffer = "";

   return retval;
}
