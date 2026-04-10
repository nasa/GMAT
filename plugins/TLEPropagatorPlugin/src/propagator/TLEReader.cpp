// Author: Darrel Conway
// Created: 2019/09/26
//
// Copyright (c) 2019, Thinking Systems, Inc.
// All rights reserved

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Edited by Jairus Elarbee 10/05/2021 
//   -Allow user to use TLE NORAD ID in addition to spacecraft name in Spacecraft.Id

#include "TLEReader.hpp"
#include <fstream>
#include <cstring>
#include <sstream>

#include <iostream>

extern "C"  
{
   #include "SpiceUsr.h"    // for CSPICE routines
}


TLEReader::TLEReader(const std::string &tleFile) :
   filename(tleFile)
{
}


TLEReader::~TLEReader()
{
}


TLEData TLEReader::GetTLEData(const std::string &forSatellite)
{
   TLEData theData;
   std::string line;

   if (filename != "")
   {
      std::string prev = "";
      std::ifstream infile(filename);
      while (std::getline(infile, line))
      {
         if (line.find(forSatellite) == 0)
         {
            theData.tleLines[0] = line;
            std::getline(infile, line);
            if (line.find('\r'))
            {
               int end = line.find('\r');
               line = line.substr(0, end);
            }
            theData.tleLines[1] = line;
            std::getline(infile, line);
            if (line.find('\r'))
            {
               int end = line.find('\r');
               line = line.substr(0, end);
            }
            theData.tleLines[2] = line;

            break;
         }
         else if (line.find(forSatellite) == 2)
         {
            if (prev.length() > 65)
            {
               prev = "GMAT TLE Sat";
            }
            else if (prev.find('\r'))
            {
               int end = prev.find('\r');
               prev = prev.substr(0, end);
            }
            theData.tleLines[0] = prev;
            if (line.find('\r'))
            {
               int end = line.find('\r');
               line = line.substr(0, end);
            }
            theData.tleLines[1] = line;
            std::getline(infile, line);
            if (line.find('\r'))
            {
               int end = line.find('\r');
               line = line.substr(0, end);
            }
            theData.tleLines[2] = line;
            break;
         }
         else if (forSatellite =="SatId" && line.find("1 ") == 0 && line.size() > 68 && line.size() < 72)
         {
            if (prev.length() > 65)
            {
               prev = "GMAT TLE Sat";
            }
            else if (prev.find('\r'))
            {
               int end = prev.find('\r');
               prev = prev.substr(0, end);
            }
            theData.tleLines[0] = prev;
            if (line.find('\r'))
            {
               int end = line.find('\r');
               line = line.substr(0, end);
            }
            prev = line;
            std::getline(infile, line);
            if (line.find("2 ") == 0 && line.size() > 68 && line.size() < 72)
            {
               if (line.find('\r'))
               {
                  int end = line.find('\r');
                  line = line.substr(0, end);
               }
               theData.tleLines[1] = prev;
               theData.tleLines[2] = line;
            }
            break;
         }
         prev = line;
      }
   }

   return theData;
}


void TLEReader::ParseForSpice(TLEData &theData)
{
   int linelen = theData.tleLines[1].length() + 1;
   char lines[180];
   strcpy(lines, theData.tleLines[1].c_str());
   strcpy(lines + linelen, theData.tleLines[2].c_str());

   getelm_c(2000, linelen, lines, &(theData.secFromJ2k), theData.elements);
}
