//$Id: MediaCorrectionInterface.cpp 67 2010-06-17 21:56:16Z tdnguye2@NDC $
//------------------------------------------------------------------------------
//                         MediaCorrectionInterface
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
// Author: Tuan Dang Nguyen GSFC-NASA
// Created: 2010/06/17
//
/**
 * Media correction model.
 */
//------------------------------------------------------------------------------


#include "MediaCorrectionInterface.hpp"
#include "SolarSystemException.hpp"
#include "Moderator.hpp"
#include "FileUtil.hpp"

//#define DEBUG_TRK223_FILELOADER


//------------------------------------------------------------------------------
// MediaCorrectionInterface(const std::string &typeStr, const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
//------------------------------------------------------------------------------
MediaCorrectionInterface::MediaCorrectionInterface(const std::string &typeStr,
                                 const std::string &nomme) :
   GmatBase         (Gmat::MEDIA_CORRECTION, typeStr, nomme),
   dataPath         (""),
   solarSystem      (NULL),
   temperature      (295.1),      // 295.1K
   pressure         (1013.5),     // 1013.5 kPa
   humidityFraction (0.55),       // 55%
   waveLength       (0.0),
   range            (1.0),
   latitude         (0.0),
   longitude        (0.0)
{
   objectTypes.push_back(Gmat::MEDIA_CORRECTION);
//   objectTypeNames.push_back("MediaCorrection");

   modelName         = typeStr;
   model             = 0;
   modelTypeName     = "";
   DSNDatabase       = { };
   groundStationId = "";
   spacecraftId    = 0;
}

//------------------------------------------------------------------------------
// ~MediaCorrectionInterface()
//------------------------------------------------------------------------------
/**
 * Donstructor
 */
//------------------------------------------------------------------------------
MediaCorrectionInterface::~MediaCorrectionInterface()
{
}

//------------------------------------------------------------------------------
// MediaCorrectionInterface(const MediaCorrectionInterface& mdc)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
MediaCorrectionInterface::MediaCorrectionInterface(const MediaCorrectionInterface& mdc):
   GmatBase           (mdc),
   dataPath           (mdc.dataPath),
   pressure           (mdc.pressure),
   temperature        (mdc.temperature),
   humidityFraction   (mdc.humidityFraction),
   waveLength         (mdc.waveLength),
   range              (mdc.range),
   latitude           (mdc.latitude),
   longitude          (mdc.longitude)
{
   model             = mdc.model;
   modelName         = mdc.modelName;
   modelTypeName     = mdc.modelTypeName;
   solarSystem       = mdc.solarSystem;
   groundStationId   = mdc.groundStationId;
   spacecraftId      = mdc.spacecraftId;
   DSNDatabase       = mdc.DSNDatabase;
}


//-----------------------------------------------------------------------------
// MediaCorrectionInterface& MediaCorrectionInterface::operator=(const MediaCorrectionInterface& mc)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param mc The MediaCorrectionInterface that is provides parameters for this one
 *
 * @return This MediaCorrectionInterface, configured to match mc
 */
//-----------------------------------------------------------------------------
MediaCorrectionInterface& MediaCorrectionInterface::operator=(const MediaCorrectionInterface& mc)
{
   if (this != &mc)
   {
      GmatBase::operator=(mc);

      model             = mc.model;
      modelName         = mc.modelName;
      modelTypeName     = mc.modelTypeName;
      solarSystem       = mc.solarSystem;
      groundStationId   = mc.groundStationId;
      spacecraftId      = mc.spacecraftId;

      dataPath          = mc.dataPath;
      DSNDatabase       = mc.DSNDatabase;

      temperature       = mc.temperature;
      pressure          = mc.pressure;
      humidityFraction  = mc.humidityFraction;
      waveLength        = mc.waveLength;
      range             = mc.range;

   }

   return *this;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs any pre-run initialization that the object needs.
 *
 * @return true unless initialization fails.
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::Initialize()
{
   if (IsInitialized())
      return true;

   if (IsOfType("Ionosphere"))
   {
      // Get path of data folder
      FileManager* fm = FileManager::Instance();
      dataPath = fm->GetPathname("DATA_PATH");

      isInitialized = true;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool SetModel(Integer mod)
//------------------------------------------------------------------------------
/**
 * Set a correction model
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetModel(Integer mod)
{
   model = mod;
   return true;
}


//------------------------------------------------------------------------------
// bool SetModelName(std::string modName)
//------------------------------------------------------------------------------
/**
 * Set a correction model name
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetModelName(std::string modName)
{
   modelName = modName;
   return true;
}


//------------------------------------------------------------------------------
// bool SetModelTypeName(Real T)
//------------------------------------------------------------------------------
/**
 * Set the name of the type of media correction model used
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetModelTypeName(std::string type)
{
	modelTypeName = type;
	return true;
}

//------------------------------------------------------------------------------
//  void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer in oreder to access needed physical parameter
 * value(s).
 */
//------------------------------------------------------------------------------
void MediaCorrectionInterface::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}

//------------------------------------------------------------------------------
// bool SetSpacecraftId(std::string spacecraftID)
//------------------------------------------------------------------------------
/**
 * Passes the ground station name to the model. Currently used for TRK-2-23 correction method.
 */
 //------------------------------------------------------------------------------
bool  MediaCorrectionInterface::SetSpacecraftId(Integer spacecraftIdNum)
{
   spacecraftId = GmatMathUtil::Abs(spacecraftIdNum);
   return true;
}


//------------------------------------------------------------------------------
// bool SetStationId(std::string groundStationId)
//------------------------------------------------------------------------------
/**
 * Passes the ground station name to the model. Currently used for TRK-2-23 correction method.
 */
 //------------------------------------------------------------------------------
bool  MediaCorrectionInterface::SetStationId(const std::string &groundStationIden)
{
   groundStationId = groundStationIden;
   return true;
}

//------------------------------------------------------------------------------
// bool SetTemperature(Real T)
//------------------------------------------------------------------------------
/**
 * Set temperature
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetTemperature(Real T)
{
   temperature = T;
   return true;
}

//------------------------------------------------------------------------------
// bool MediaCorrectionInterface::SetDSNDatabase( SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * This function will pass the DSNCorrectionDatabase from the solarSystem to the MediaCorrection object,
 * building and and updating the DSNCorrectionDataBase on the solarSystem object if necessary
 *
 * @param *ss                     the SolarSystem object
 *
 * return true if completed correctly
 */
 //------------------------------------------------------------------------
bool MediaCorrectionInterface::SetDSNDatabase( SolarSystem *ss)
{
   if (ss->GetBody("Earth")->GetDSNCorrectionDatabase().size() == 0)
   { 
      
      StringArray DSNFilePaths = ss->GetBody("Earth")->GetStringArrayParameter("DSNMediaFileDirectories");
      StringArray CSPFilelist;
      StringArray DSNFileList;

      if (DSNFilePaths.size() == 0) 
      {
         throw SolarSystemException("Error: No directories are set for Earth.DSNMediaFileDirectories. Directories containing TRK-2-23 files must be specified to use the TRK-2-23 model.\n");
      }
      MessageInterface::ShowMessage("Accessing Directories For TRK-2-23 Model\n");
      for (UnsignedInt i = 0; i < DSNFilePaths.size(); i++)
      {
         MessageInterface::ShowMessage("Checking directory " + DSNFilePaths[i] + " for .csp and .csp.ql Files \n");
         if (GmatFileUtil::HasNoPath(DSNFilePaths[i]))
         {
            DSNFilePaths[i] = GmatFileUtil::GetCurrentWorkingDirectory() + DSNFilePaths[i];
         }

         if (GmatFileUtil::IsOsWindows())
         {
            StringArray filePaths = GmatFileUtil::GetFileListFromDirectory(DSNFilePaths[i] + "\\*.csp.ql", true);
            DSNFileList.insert(DSNFileList.end(), filePaths.begin(), filePaths.end());
            filePaths = GmatFileUtil::GetFileListFromDirectory(DSNFilePaths[i] + "\\*.csp", true);
            CSPFilelist.insert(CSPFilelist.end(), filePaths.begin(), filePaths.end());
         }
         else
         {
            StringArray filePaths = GmatFileUtil::GetFileListFromDirectory(DSNFilePaths[i] + "/*.csp.ql", true);
            DSNFileList.insert(DSNFileList.end(), filePaths.begin(), filePaths.end());
            filePaths = GmatFileUtil::GetFileListFromDirectory(DSNFilePaths[i] + "/*.csp", true);
            CSPFilelist.insert(CSPFilelist.end(), filePaths.begin(), filePaths.end());
         }

      }

      DSNFileList.insert(DSNFileList.end(), CSPFilelist.begin(), CSPFilelist.end());
      if (DSNFileList.size() == 0)
      {
         throw SolarSystemException("Error: No .csp or .csp.ql files found among the Earth.DSNMediaFileDirectories.\n");
      }
      std::vector<StringArray> DSNCorrectionDatabase;

      for (UnsignedInt i = 0; i < DSNFileList.size(); ++i)
      {
         std::vector<StringArray> tempFile = LoadTRK223DataFile(DSNFileList[i]);
         DSNCorrectionDatabase.insert(DSNCorrectionDatabase.end(), tempFile.begin(), tempFile.end());

      }

      MessageInterface::ShowMessage("\n");
      ss->GetBody("Earth")->SetDSNCorrectionDatabase(DSNCorrectionDatabase);

   }
   
   DSNDatabase = ss->GetBody("Earth")->GetDSNCorrectionDatabase();

   return true;
}

//------------------------------------------------------------------------
// std::vector<StringArray> MediaCorrectionInterface::LoadTRK223DataFile(std::string filePath)
//------------------------------------------------------------------------
/**
 * This function is used to parse the data from a .csp file and add it to the DSNDatabase
 *
 * @param filePath                     The path to the .csp file to be read
 *
 * return std:vector<StringArray> with the parsed data from the file
 */
 //------------------------------------------------------------------------

std::vector<StringArray> MediaCorrectionInterface::LoadTRK223DataFile(std::string &filePath)
{

#ifdef DEBUG_TRK223_FILELOADER

   MessageInterface::ShowMessage("Troposphere::LoadTRK223DataFile(): Using TRK-2-23 model, loading file: \n" + filePath + "\n");

   MessageInterface::ShowMessage("Troposphere::LoadTRK223DataFile(): Reading Lines \n" + filePath + "\n");

#endif

   MessageInterface::ShowMessage("Accessing file " + filePath + " \n");
   FileManager *fm = FileManager::Instance();
   if (GmatFileUtil::HasNoPath(filePath))
   {
      filePath = GmatFileUtil::GetCurrentWorkingDirectory() + filePath;
   }
   std::ifstream tropoFile(filePath);
   
   std::string theLine;
   std::string nextLine;

   std::vector<StringArray> theFile;

   // Will search entire file until both dry and wet corrections data is found, or until file ends
   while (!tropoFile.eof())
   {
      GmatFileUtil::GetLine(&tropoFile, theLine);

      if (theLine.find("#") != 0)                         // Check to see if it is not a comment line
      {
         if (theLine.find("ADJUST") != std::string::npos)       // Combines lines into a single line if the file has separated them
         {
            int howLongIsLine = 1;
            GmatFileUtil::GetLine(&tropoFile, nextLine);
            theLine = theLine + nextLine;
            while (theLine.find("DSN") == std::string::npos)
            {
               howLongIsLine++;
               GmatFileUtil::GetLine(&tropoFile, nextLine);
               theLine = theLine + nextLine;
               if (tropoFile.eof())
               {
                  std::string errmsg = "Error: The final entry of the file " + filePath + " is corrupted.\n";
                  throw SolarSystemException(errmsg);
               }
               if(howLongIsLine > 3)
               {
                  std::string errmsg = "Error: The line " + theLine + " in the file " + filePath + " is of irregular length.\n";
                  throw SolarSystemException(errmsg);
               }

            }

#ifdef DEBUG_TRK223_FILELOADER

            MessageInterface::ShowMessage("Troposphere::LoadTRK223DataFile(): " +theLine+ "\n");

#endif
            
            std::string dataType, mathType, coefficients, modelType, timeStart, timeEnd, DSNId, sourceId ="";
            StringArray processedFile;

            bool isInLine = (theLine.rfind("ADJUST(") != std::string::npos && theLine.rfind("BY") != std::string::npos &&theLine.rfind("MODEL(") != std::string::npos &&theLine.rfind("FROM(") != std::string::npos &&theLine.rfind("TO(") != std::string::npos);
            if (!isInLine)
            {
               std::string errmsg = "Error: there is a corrupted entry in line \"" + theLine + "\" in the file " + filePath + " \n";
               throw SolarSystemException(errmsg);

            }

            dataType = theLine.substr(theLine.rfind("ADJUST(") + 7);
            dataType = dataType.substr(0, dataType.find(")"));
            processedFile.push_back(dataType);

            // Extract the model type from the line

            mathType = theLine.substr(theLine.rfind("BY") + 3);
            mathType = mathType.substr(0, mathType.find("("));
            processedFile.push_back(mathType);

            // Strip the line of the Coefficients
            coefficients = theLine.substr(theLine.rfind("BY") + 3);
            coefficients = coefficients.substr(coefficients.find("(") + 1);
            coefficients = coefficients.substr(0, coefficients.find(")"));
            processedFile.push_back(coefficients);

            modelType = theLine.substr(theLine.rfind("MODEL(") + 6);
            modelType = modelType.substr(0, modelType.find(")"));
            processedFile.push_back(modelType);
            
            timeStart = theLine.substr(theLine.rfind("FROM(") + 5);
            timeStart = timeStart.substr(0, timeStart.find(")"));
            processedFile.push_back(timeStart);

            timeEnd = theLine.substr(theLine.rfind("TO(") + 3);
            timeEnd = timeEnd.substr(0, timeEnd.find(")"));
            processedFile.push_back(timeEnd);

            DSNId = theLine.substr(theLine.rfind("DSN"));
            DSNId = DSNId.substr(0, DSNId.find(")") + 1);
            processedFile.push_back(DSNId);

            if (bool isStation = theLine.find("SCID") != std::string::npos)
            {
               sourceId = theLine.substr(theLine.rfind("SCID"));
               sourceId = sourceId.substr(0, sourceId.find(")") + 1);
            }
            else if (bool isStation = theLine.find("QUASAR") != std::string::npos)
            {
               sourceId = theLine.substr(theLine.rfind("QUASAR"));
               sourceId = sourceId.substr(0, sourceId.find(")") + 1);
            }
            else
            {
               sourceId = "NONE";
            }
            
            processedFile.push_back(sourceId);
            theFile.push_back(processedFile);
         }
      }
      std::stringstream ss;

   }
   return  theFile;
}

//------------------------------------------------------------------------------
// bool SetPressure(Real P)
//------------------------------------------------------------------------------
/**
 * Set pressure
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetPressure(Real P)
{
   pressure = P;
   return true;
}


//------------------------------------------------------------------------------
// bool SetHumidityFraction(Real humFr)
//------------------------------------------------------------------------------
/**
 * Set humidity fraction
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetHumidityFraction(Real humFr)
{
   humidityFraction = humFr;
   return true;
}

//------------------------------------------------------------------------------
// bool SetElevationAngle(Real elevation)
//------------------------------------------------------------------------------
/**
 * Set elevation angle
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetElevationAngle(Real elevation)
{
   elevationAngle = elevation;
   return true;
}

//------------------------------------------------------------------------------
// bool SetRange(Real r)
//------------------------------------------------------------------------------
/**
 * Set range
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetRange(Real r)
{
   range = r;
   return true;
}

//------------------------------------------------------------------------------
// bool SetWaveLength(Real lambda)
//------------------------------------------------------------------------------
/**
 * Set wave length
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetWaveLength(Real lambda)
{
   waveLength = lambda;
   return true;
}

//------------------------------------------------------------------------------
// bool SetLatitude(Real lat)
//------------------------------------------------------------------------------
/**
 * Set latitude
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetLatitude(Real lat)
{
	latitude = lat;
	return true;
}

//------------------------------------------------------------------------------
// bool SetLongitude(Real lat)
//------------------------------------------------------------------------------
/**
 * Set longitude
 */
//------------------------------------------------------------------------------
bool MediaCorrectionInterface::SetLongitude(Real lon)
{
	longitude = lon;
	return true;
}
