//$Id$
//------------------------------------------------------------------------------
//                           TcopsVHFData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: May 3, 2013
/**
 * Implementation of the TCOPS Vector Hold File data reader
 */
//------------------------------------------------------------------------------

#include "TcopsVHFData.hpp"
#include "FileUtil.hpp"
#include "InterfaceException.hpp"


//------------------------------------------------------------------------------
// TcopsVHFData(const std::string& theTypeName, const std::string& theName)
//------------------------------------------------------------------------------
/**
 * DEfault constructor
 *
 * @param theTypeName Name of the reader type
 * @param theName The name of the reader
 */
//------------------------------------------------------------------------------
TcopsVHFData::TcopsVHFData(const std::string& theTypeName,
      const std::string& theName) :
   FileReader        (theTypeName, theName),
   origin            ("Earth"),
   csSuffix          ("MJ2000Eq"),
   timeSystem        ("UTCModJulian")
{
   // Set up the engine accessor fields
   objectTypeNames.push_back("TcopsVHFData");

   // Fill in the list of supported data from the VHF
   supportedFields.clear();

   supportedFields.push_back("Epoch");
   fileStringMap["Epoch"]   = "EPOCH TIME FOR ELEMENTS";
   objectStringMap["Epoch"] = "Epoch";
   objectIDMap["Epoch"]     = -1;
   dataType["Epoch"]        = READER_TIMESTRING;
   dataLoaded["Epoch"]      = false;

   supportedFields.push_back("CartesianState");
   fileStringMap["CartesianState"]   = "CARTESIAN COORDINATES";
   objectStringMap["CartesianState"] = "CartesianX";
   objectIDMap["CartesianState"]     = -1;
   dataType["CartesianState"]        = READER_RVECTOR6;
   dataLoaded["CartesianState"]      = false;

   supportedFields.push_back("Cr");
   fileStringMap["Cr"]   = "CSUBR";
   objectStringMap["Cr"] = "Cr";
   objectIDMap["Cr"]     = -1;
   dataType["Cr"]        = READER_REAL;
   dataLoaded["Cr"]      = false;

   // Subtypes map the parameter names for individual elements of a 6-vector
   supportedFields.push_back("X ");
   fileStringMap["X "]   = "X ";
   objectStringMap["X "] = "X";
   objectIDMap["X "]     = -1;
   dataType["X "]        = READER_SUBTYPE;
   dataLoaded["X "]      = false;

   supportedFields.push_back("Y ");
   fileStringMap["Y "]   = "Y ";
   objectStringMap["Y "] = "Y";
   objectIDMap["Y "]     = -1;
   dataType["Y "]        = READER_SUBTYPE;
   dataLoaded["Y "]      = false;

   supportedFields.push_back("Z ");
   fileStringMap["Z "]   = "Z ";
   objectStringMap["Z "] = "Z";
   objectIDMap["Z "]     = -1;
   dataType["Z "]        = READER_SUBTYPE;
   dataLoaded["Z "]      = false;

   supportedFields.push_back("XDOT");
   fileStringMap["XDOT"]   = "XDOT";
   objectStringMap["XDOT"] = "VX";
   objectIDMap["XDOT"]     = -1;
   dataType["XDOT"]        = READER_SUBTYPE;
   dataLoaded["XDOT"]      = false;

   supportedFields.push_back("YDOT");
   fileStringMap["YDOT"]   = "YDOT";
   objectStringMap["YDOT"] = "VY";
   objectIDMap["YDOT"]     = -1;
   dataType["YDOT"]        = READER_SUBTYPE;
   dataLoaded["YDOT"]      = false;

   supportedFields.push_back("ZDOT");
   fileStringMap["ZDOT"]   = "ZDOT";
   objectStringMap["ZDOT"] = "VZ";
   objectIDMap["ZDOT"]     = -1;
   dataType["ZDOT"]        = READER_SUBTYPE;
   dataLoaded["ZDOT"]      = false;

   // Data needed to build the coordinate system name
   supportedFields.push_back("CoordinateSystem");
   fileStringMap["CoordinateSystem"]   = "REFERENCE COORDINATE SYSTEM:";
   // Empty string here implies no direct mapping
   objectStringMap["CoordinateSystem"] = "";
   objectIDMap["CoordinateSystem"]     = -1;
   dataType["CoordinateSystem"]        = READER_STRING;
   dataLoaded["CoordinateSystem"]      = false;

   supportedFields.push_back("CentralBody");
   fileStringMap["CentralBody"]   = "CENTRAL BODY:";
   objectStringMap["CentralBody"] = "";
   objectIDMap["CentralBody"]     = -1;
   dataType["CentralBody"]        = READER_STRING;
   dataLoaded["CentralBody"]      = false;
}

//------------------------------------------------------------------------------
// ~TcopsVHFData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TcopsVHFData::~TcopsVHFData()
{
}

//------------------------------------------------------------------------------
// TcopsVHFData(const TcopsVHFData& vhf) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param vhf The reader copied to make the new one
 */
//------------------------------------------------------------------------------
TcopsVHFData::TcopsVHFData(const TcopsVHFData& vhf) :
   FileReader        (vhf)
{
}

//------------------------------------------------------------------------------
// TcopsVHFData& operator=(const TcopsVHFData& vhf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param vhf The reader used to set properties for this one
 *
 * @return This reader, set to match vhf
 */
//------------------------------------------------------------------------------
TcopsVHFData& TcopsVHFData::operator=(const TcopsVHFData& vhf)
{
   if (this == &vhf)
   {
      FileReader::operator=(vhf);
   }

   return *this;
}


//------------------------------------------------------------------------------
// const StringArray& GetSupportedFieldNames()
//------------------------------------------------------------------------------
/**
 * Returns the list of supported field
 *
 * @note: This method is overriding the base class method (see
 * DataReader::GetSupportedFieldNames()) and does not add functionality, so it
 * should be removed.
 *
 * @return The list
 */
//------------------------------------------------------------------------------
const StringArray& TcopsVHFData::GetSupportedFieldNames()
{
   return supportedFields;
}

//------------------------------------------------------------------------------
// bool TcopsVHFData::UsesCoordinateSystem(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Checks to see if a field uses a coordinate system
 *
 * @param forField The field identifier for check
 *
 * @return true if a coordinate system is associated with the field, false
 *              if not
 */
//------------------------------------------------------------------------------
bool TcopsVHFData::UsesCoordinateSystem(const std::string& forField)
{
   if (forField == "CartesianState")
      return true;
   return false;  // Should call the base class method here
}

//------------------------------------------------------------------------------
// std::string GetCoordinateSystemName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a used coordinate system
 *
 * @param forField The field identifier for the data that used the coordinate
 *                 system
 *
 * @return The name of the coordinate system
 */
//------------------------------------------------------------------------------
std::string TcopsVHFData::GetCoordinateSystemName(const std::string& forField)
{
   std::string retval = "";
   if (forField == "CartesianState")
      retval = origin + csSuffix;
   return retval;  // Should call the base class method here
}

//------------------------------------------------------------------------------
// bool UsesOrigin(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Checks to see if a field uses an origin
 *
 * @param forField The field identifier for check
 *
 * @return true if an origin is associated with the field, false if not
 */
//------------------------------------------------------------------------------
bool TcopsVHFData::UsesOrigin(const std::string& forField)
{
   // Uncomment if needed; CartesianState needs full CS, I think
//   if (forField == "CartesianState")
//      return true;
   return false;  // Should call the base class method here
}

//------------------------------------------------------------------------------
// std::string GetOriginName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a used origin
 *
 * @param forField The field identifier for the data that used the origin
 *
 * @return The name of the origin
 */
//------------------------------------------------------------------------------
std::string TcopsVHFData::GetOriginName(const std::string& forField)
{
   std::string retval = "";
   if (forField == "CartesianState")
      retval = origin;
   return retval;  // Should call the base class method here
}

//------------------------------------------------------------------------------
// bool UsesTimeSystem(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Checks to see if a field uses a time system
 *
 * @param forField The field identifier for check
 *
 * @return true if a time system is associated with the field, false if not
 */
//------------------------------------------------------------------------------
bool TcopsVHFData::UsesTimeSystem(const std::string& forField)
{
   if (forField == "Epoch")
      return true;
   return false;  // Should call the base class method here
}

//------------------------------------------------------------------------------
// std::string GetTimeSystemName(const std::string& forField)
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a used time system
 *
 * @param forField The field identifier for the data that used the time system
 *
 * @return The name of the time system
 */
//------------------------------------------------------------------------------
std::string TcopsVHFData::GetTimeSystemName(const std::string& forField)
{
   std::string retval = "";
   if (forField == "Epoch")
      retval = "UTCModJulian";
   return retval;  // Should call the base class method here
}

//------------------------------------------------------------------------------
// void BuildOriginName()
//------------------------------------------------------------------------------
/**
 * Translation table between origins in the TVHF and the names GMAT recognizes
 */
//------------------------------------------------------------------------------
void TcopsVHFData::BuildOriginName()
{
   if (stringData.find("CentralBody") != stringData.end())
   {
      std::string body = stringData["CentralBody"];

      origin = "";

      if (body == "SUN")
         origin = "Sun";
      if (body == "MERCURY")
         origin = "Mercury";
      if (body == "VENUS")
         origin = "Venus";
      if (body == "EARTH")
         origin = "Earth";
      if (body == "MOON")
         origin = "Luna";
      if (body == "MARS")
         origin = "Mars";
      if (body == "JUPITER")
         origin = "Jupiter";
      if (body == "SATURN")
         origin = "Saturn";
      if (body == "URANUS")
         origin = "Uranus";
      if (body == "NEPTUNE")
         origin = "Neptune";
      if (body == "PLUTO")
         origin = "Pluto";

      if (origin == "")
         throw InterfaceException("The TVHF state data is set with the "
               "origin \"" + body + "\", which is not recognized in GMAT's "
               "TVHF reader");
   }
}

//------------------------------------------------------------------------------
// void BuildCSName()
//------------------------------------------------------------------------------
/**
 * Translation table between CSs in the TVHF and the names GMAT recognizes
 */
//------------------------------------------------------------------------------
void TcopsVHFData::BuildCSName()
{
   if (stringData.find("CoordinateSystem") != stringData.end())
   {
      std::string cs = stringData["CoordinateSystem"];

      if (cs == "J2000")
         csSuffix = "MJ2000Eq";
      else if (cs == "TOD")
         throw InterfaceException("The TVHF state data is set in the TOD "
               "coordinate system, which is not supported by GMAT's TVHF "
               "reader");
//         csSuffix = "TODEq";
      else if (cs == "1950")
         throw InterfaceException("The TVHF state data is set in the Mean "
               "Equator and Equinox of 1950, which is not supported in GMAT");
      else
         throw InterfaceException("The TVHF state data is set in the " + cs +
               "coordinate system, which is not recognized by GMAT's "
               "TVHF reader");
   }
}
