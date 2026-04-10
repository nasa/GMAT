//$Id$
//------------------------------------------------------------------------------
//                            SolarFluxReader
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
// Author: Farideh Farahnak
// Created: 2014/11/14
// Updated By: Claire Conway
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II.
//
/**
 * SolarFluxReader class will be used by the AtomsphereModel class.
 */
//------------------------------------------------------------------------------

#include "SolarFluxReader.hpp"
#include "SolarSystemException.hpp"

#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"
#include <sstream>


//#define DEBUG_FILE_INDEXING
//#define DEBUG_GETFLUXINPUTS
//#define DEBUG_FLUXINTERPOLATION
//#define DUMP_FLUX_DATA
//#define DEBUG_FIRST_CALL
//#define OMIT_F107_OFFSET



//#define DEBUG_FIRSTFEW_READS

#ifdef DEBUG_FIRSTFEW_READS
   Integer howMany = 5;
   Integer numberReadIndex = 0;
   Integer refcount = 0;
#endif

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SolarFluxReader()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::SolarFluxReader() :
   histFileName       (""),
   predictFileName   (""),
   historicStart     (-1.0),
   historicDailyEnd  (-1.0),
   historicEnd       (-1.0),
   predictStart      (-1.0),
   predictEnd        (-1.0),
   schattenFluxIndex (0),
   schattenApIndex   (0),
   warnEpochBefore   (true),
   warnEpochAfter    (true),
   f107RefEpoch      (18408.0),  // 5/31/91, epoch when the station moved (Vallado)
   interpolateFlux   (true),
   interpolateGeo    (false),
   constantF107      (150.0),
   constantF107a     (150.0),
   constantKp        (3.0),
   constantAp        (15.0)
{
   if (!histFluxData.empty())
      histFluxData.clear();

   if (!predictFluxData.empty())
      predictFluxData.clear();

   begin_HistTag = "BEGIN OBSERVED";
   end_HistTag = "END OBSERVED";
   histOffset = -1;

   begin_DailyPred = "BEGIN DAILY_PREDICTED";
   end_DailyPred = "END DAILY_PREDICTED";

   begin_MonthlyPred = "BEGIN MONTHLY_PREDICTED";
   end_MonthlyPred = "END MONTHLY_PREDICTED";

   begin_MonthlyFit = "BEGIN MONTHLY_FIT";
   end_MonthlyFit = "END MONTHLY_FIT";

   predictedDataSource = -1;
   historicDataSource = -1;
}


//------------------------------------------------------------------------------
// ~SolarFluxReader()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::~SolarFluxReader()
{
   #ifdef DEBUG_FIRSTFEW_READS
      --refcount;
      MessageInterface::ShowMessage("Deleting Solar Flux Reader %p, leaving %d\n", this, refcount);
   #endif
}


//------------------------------------------------------------------------------
// SolarFluxReader(const SolarFluxReader &)
//------------------------------------------------------------------------------
/**
 * copy constructor
 */
//------------------------------------------------------------------------------
SolarFluxReader::SolarFluxReader(const SolarFluxReader &sfr) :
   historicStart     (-1.0),
   historicEnd       (-1.0),
   predictStart      (-1.0),
   historicDailyEnd  (-1.0),
   predictEnd        (-1.0),
   schattenFluxIndex (1),
   schattenApIndex   (0),
   warnEpochBefore   (true),
   warnEpochAfter    (true),
   f107RefEpoch      (sfr.f107RefEpoch),
   interpolateFlux   (sfr.interpolateFlux),
   interpolateGeo    (sfr.interpolateGeo),
   constantF107      (sfr.constantF107),
   constantF107a     (sfr.constantF107a),
   constantKp        (sfr.constantKp),
   constantAp        (sfr.constantAp)
{
   histFileName = sfr.histFileName;
   predictFileName = sfr.predictFileName;
   histFluxData = sfr.histFluxData;
   predictFluxData = sfr.predictFluxData;
   begin_HistTag = sfr.begin_HistTag;
   end_HistTag = sfr.end_HistTag;
   histOffset = sfr.histOffset;

   predictedDataSource = sfr.predictedDataSource;
   historicDataSource = sfr.historicDataSource;
}


//------------------------------------------------------------------------------
// SolarFluxReader& SolarFluxReader::operator=(const SolarFluxReader& sfr)
//------------------------------------------------------------------------------
/**
 * assignment operator
 */
//------------------------------------------------------------------------------
SolarFluxReader& SolarFluxReader::operator=(const SolarFluxReader &sfr)
{
   if (this == &sfr)
      return *this;

   histFileName = sfr.histFileName;
   predictFileName = sfr.predictFileName;
   histFluxData = sfr.histFluxData;
   predictFluxData = sfr.predictFluxData;
   begin_HistTag = sfr.begin_HistTag;
   end_HistTag = sfr.end_HistTag;
   histOffset = sfr.histOffset;
   line = sfr.line;
   historicStart = -1.0;
   historicDailyEnd = -1.0;
   historicEnd = -1.0;
   predictStart = -1.0;
   predictEnd = -1.0;
   schattenFluxIndex = 1;
   schattenApIndex = 0;
   
   warnEpochBefore = true;
   warnEpochAfter = true;
   f107RefEpoch = sfr.f107RefEpoch;
   interpolateFlux = sfr.interpolateFlux;
   interpolateGeo = sfr.interpolateGeo;
   predictedDataSource = sfr.predictedDataSource;
   historicDataSource = sfr.historicDataSource;

   constantF107  = sfr.constantF107;
   constantF107a = sfr.constantF107a;
   constantKp    = sfr.constantKp;
   constantAp    = sfr.constantAp;

   return *this;
}


//------------------------------------------------------------------------------
// bool Open()
//------------------------------------------------------------------------------
/**
 * Opens both Observed and Schatten input file streams.
 *
 * This method will open input file streams
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::Open()
{
   #ifdef DEBUG_FIRSTFEW_READS
      MessageInterface::ShowMessage("Opening flux files:\n");
      MessageInterface::ShowMessage("   H: %s\n   P: %s\n", histFileName.c_str(), predictFileName.c_str());
   #endif

   if (!histFileName.empty())
   {
      inHist.open(histFileName.c_str(), std::ios_base::in | std::ios_base::binary);
   }
   if (!predictFileName.empty())
   {
      inPredict.open(predictFileName.c_str(), std::ios_base::in | std::ios_base::binary);
   }

   return true;
}


//------------------------------------------------------------------------------
// bool LoadFluxData()
//------------------------------------------------------------------------------
/**
 * Loads both Observed and Schatten input file data.
 *
 * This method will load input file data.
 * 
 * @param Observed file name and Predicted(Schatten) file name.
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::LoadFluxData(const std::string &histFile, const std::string &predictFile)
{
   std::string theLine;

   if (!histFile.empty())
      histFileName = histFile;
   if (!predictFile.empty())
      predictFileName = predictFile;

   histFluxData.clear();
   predictFluxData.clear();

   FileManager *fm = FileManager::Instance();

   if (histFileName != "")
   {
      std::string weatherfile = histFileName;
      if (fm->DoesFileExist(weatherfile) == false)
         weatherfile = fm->GetAbsPathname("ATMOSPHERE_PATH") + weatherfile;
      if (fm->DoesFileExist(weatherfile) == false)
         throw SolarSystemException("Cannot open the historic space weather file " +
               histFileName + ", nor the file at the location " + weatherfile);

      histFileName = weatherfile;
   }

   if (predictFileName != "")
   {
      std::string weatherfile = predictFileName;
      if (fm->DoesFileExist(weatherfile) == false)
         weatherfile = fm->GetAbsPathname("ATMOSPHERE_PATH") + weatherfile;
      if (fm->DoesFileExist(weatherfile) == false)
         throw SolarSystemException("Cannot open the predicted space weather file " +
               predictFileName + ", nor the file at the location " + weatherfile);

      predictFileName = weatherfile;
   }

   // Open the files to load
   Open();
   
   // If Historic source is CSSI, load the data
   if (histFileName != "")
   {
      if (historicDataSource == 1)
      {
         inHist.seekg(0, std::ios_base::beg);

         while (!inHist.eof())
         {
            GmatFileUtil::GetLine(&inHist, theLine);

            if (theLine.find(begin_HistTag) != std::string::npos)
            {
               histOffset = inHist.tellg();
               break;
            }
         }
         LoadCSSIHistoric();
      }
   }

   // If Predicted source is CSSI, load the data
   if (predictFileName != "")
   {
      // Predicted source is CSSI
      if (predictedDataSource == 1)
      {
         if (inPredict.is_open())
         {
            inPredict.seekg(0, std::ios_base::beg);

            while (!inPredict.eof())
            {
               GmatFileUtil::GetLine(&inPredict, theLine);

               if (theLine.find(begin_DailyPred) != std::string::npos)
               {
                  predictOffset = inPredict.tellg();
                  break;
               }
            }
            LoadCSSIPredict();
         }
      }

      // If Predicted source is Schatten, load the data
      if (predictedDataSource == 2)
      {
         if (inPredict.is_open())
            LoadSchattenData();
         else
         {
            //throw an exception
            throw SolarSystemException("SolarFluxReader: The Schatten predict "
                  "file " + predictFileName + " could not be opened.\n");
         }
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Spans: [%lf %lf], [%lf %lf]\n",
            historicStart, historicEnd, predictStart, predictEnd);
   #endif

   return(Close());
}


//------------------------------------------------------------------------------
// bool Close()
//------------------------------------------------------------------------------
/**
 * Closes both Observed and Schatten input file streams.
 *
 * This method will close input file streams
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::Close()
{
   bool ret = false;
   if (inHist.is_open())
   {
      inHist.close();
      ret = true;
   }
   if (inPredict.is_open())
   {
      inPredict.close();
      ret = true;
   }

   return ret;
}

//------------------------------------------------------------------------------
// bool LoadCSSIHistoric()
//------------------------------------------------------------------------------
/**
 * Loads Historic (aka Observed) space weather data from a CSSI formatted file.
 *
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::LoadCSSIHistoric()
{
   Integer hour = 0, minute = 0;
   Real sec = 0.0;

   std::stringstream buffer;

   // Offset for historic data will start the file reader at "BEGIN OBSERVED". Start
   // processing from the next line.
   inHist.seekg(histOffset, std::ios_base::beg);
   std::string theLine;
   GmatFileUtil::GetLine(&inHist, theLine);

   Integer year, month, day, value;
   Real lastMjd = -1.0;

   // Read until end of file or "END OBSERVED"
   while (!inHist.eof() && (theLine.find(end_HistTag) == std::string::npos))
   {
      buffer.str("");
      buffer << theLine;

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%s\n", buffer.str());
      #endif

      if (theLine.length() > 8)
      {
         buffer.str("");
         buffer << theLine;
         buffer >> year >> month >> day;

         // Set ref epoch to midnight for the date on the current line
         Real mjd = ModifiedJulianDate(year, month, day, hour, minute, sec);

         if (mjd > lastMjd) // Only add a record if later than the last one
         {
            if ((mjd - lastMjd > 1.01) && (historicDailyEnd == -1.0))
               historicDailyEnd = lastMjd;

            FluxData fD;
            lastMjd = mjd;
            fD.epoch = mjd;

            // Just drop the next 2 entries;
            buffer >> value;
            buffer >> value;

            // The CSSI file contains Kp * 10, then rounded to an int. Undo that here.
            for (Integer m = 0; m < 8; ++m)
            {
               buffer >> value;
               fD.kp[m] = value / 10.0;
            }
            // Drop the sum column
            buffer >> value;
            for (Integer m = 0; m < 8; ++m)
               buffer >> fD.ap[m];

            buffer >> value;
            fD.apAvg = value;

            // Set for the second part of the string
            buffer.str("");
            buffer << theLine.substr(92);

            buffer >> fD.adjF107;
            buffer >> value;
            buffer >> fD.adjCtrF107a;
            buffer >> fD.obsF107;      // Intentional throw away (Adj Lst value)
            buffer >> fD.obsF107;
            buffer >> fD.obsCtrF107a;

            fD.index = -1;
            for (Integer l = 0; l<9; l++)
               fD.F107a[l] = -1;
            for (Integer l =0; l<3; l++)
               fD.apSchatten[l] = -1;

            fD.isHistData = true;
            histFluxData.push_back(fD);
         }
      }

      GmatFileUtil::GetLine(&inHist, theLine);
   }

   #ifdef DUMP_CSSI_DATA
      for (UnsignedInt i = 0; i < histFluxData.size(); ++i)
      {
         MessageInterface::ShowMessage("%4d:  %.2lf  [%.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f]  "
            "[%.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f] %f   [%.1f %.1f]  [%.1f %.1f]\n",
            i, histFluxData[i].epoch,
            histFluxData[i].kp[0], histFluxData[i].kp[1], histFluxData[i].kp[2], histFluxData[i].kp[3],
            histFluxData[i].kp[4], histFluxData[i].kp[5], histFluxData[i].kp[6], histFluxData[i].kp[7],
            histFluxData[i].ap[0], histFluxData[i].ap[1], histFluxData[i].ap[2], histFluxData[i].ap[3],
            histFluxData[i].ap[4], histFluxData[i].ap[5], histFluxData[i].ap[6], histFluxData[i].ap[7],
            histFluxData[i].apAvg, histFluxData[i].obsF107, histFluxData[i].obsCtrF107a,
            histFluxData[i].adjF107, histFluxData[i].adjCtrF107a);
      }
   #endif

   #ifdef DEBUG_GETFLUXINPUTS
      MessageInterface::ShowMessage("Loaded %d flux observation data records\n"
            "Loaded %d flux prediction data records\n",
            histFluxData.size(), predictFluxData.size());
   #endif

   if (histFluxData.size() > 0)
   {
      historicStart = histFluxData[0].epoch;
      // Note that epoch of last record is at day start; add 1 to reach end!
      historicEnd = histFluxData[histFluxData.size() - 1].epoch + 1.0;

      if (historicDailyEnd == -1.0)
         historicDailyEnd = historicEnd;

      for (Integer i = 0; i < histFluxData.size(); ++i)
         histFluxData[i].id = i;
   }

   return true;
}

//------------------------------------------------------------------------------
// bool LoadCSSIPredict()
//------------------------------------------------------------------------------
/**
 * Loads Predict input file data from a CSSI file.
 *
 * This method will load Predict input file data.
 *
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::LoadCSSIPredict()
{
   Integer hour = 0, minute = 0;
   Real sec = 0.0;

   std::stringstream buffer;

   // Offset for Predicted data will start the file reader at "BEGIN DAILY_PREDICTED".
   // Start processing from the next line.
   inPredict.seekg(predictOffset, std::ios_base::beg);
   std::string theLine;
   GmatFileUtil::GetLine(&inPredict, theLine);

   Integer year, month, day, value;
   Real lastMjd = -1.0;

   // Read until end of file or "END MONTHLY_FIT"
   while (!inPredict.eof() && (theLine.find(end_MonthlyFit) == std::string::npos))
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%s\n", theLine.c_str());
      #endif

      if (theLine.length() > 60)
      {
         buffer.str("");
         buffer << theLine;
         buffer >> year >> month >> day;

         // Set ref epoch to midnight for the date on the current line
         Real mjd = ModifiedJulianDate(year, month, day, hour, minute, sec);

         if (mjd > lastMjd) // Only add a record if later than the last one
         {
            if ((mjd - lastMjd > 1.01) && (historicDailyEnd == -1.0))
               historicDailyEnd = lastMjd;

            FluxData fD;
            lastMjd = mjd;
            fD.epoch = mjd;

            // Just drop the next 2 entries;
            buffer >> value;
            buffer >> value;

            // The CSSI file contains Kp * 10, then rounded to an int. Undo that here.
            for (Integer m = 0; m < 8; ++m)
            {
               buffer >> value;
               fD.kp[m] = value / 10.0;
            }
            // Drop the sum column
            buffer >> value;
            for (Integer m = 0; m < 8; ++m)
               buffer >> fD.ap[m];

            buffer >> value;
            fD.apAvg = value;

            // Set for the second part of the string
            buffer.str("");
            buffer << theLine.substr(92);

            buffer >> fD.adjF107;
            buffer >> value;
            buffer >> fD.adjCtrF107a;
            buffer >> fD.obsF107;      // Intentional throw away (Adj Lst value)
            buffer >> fD.obsF107;
            buffer >> fD.obsCtrF107a;

            for (Integer l = 0; l<9; l++)
               fD.F107a[l] = fD.obsF107;
            for (Integer l =0; l<3; l++)
               fD.apSchatten[l] = fD.kp[l];

            fD.isHistData = false;
            predictFluxData.push_back(fD);
         }
      }

      GmatFileUtil::GetLine(&inPredict, theLine);
   }

   if (predictFluxData.size() > 0)
   {
      predictStart = predictFluxData[0].epoch;
      predictEnd = predictFluxData[predictFluxData.size() - 1].epoch;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool LoadSchattenData()
//------------------------------------------------------------------------------
/**
 * Loads Predict input file data from a Schatten file.
 *
 * This method will load Predict input file data.
 * 
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::LoadSchattenData()
{ 
   Integer hour = 0, minute = 0, dom = 1;
   Real sec = 0.0;
   std::string theLine;
   std::stringstream messageQueue;

   Integer lineCounter = 0;
   std::stringstream lineList;
   while (!inPredict.eof())
   {

      GmatFileUtil::GetLine(&inPredict, theLine);

      // Skip header lines
      if ((theLine.find("NOMINAL TIMING") != std::string::npos) ||
          (theLine.find("BEGIN_DATA") != std::string::npos) ||
          ((theLine.find("mean")  != std::string::npos) &&
           (theLine.find("+2sig") != std::string::npos) &&
           (theLine.find("-2sig") != std::string::npos)))
         continue;

      line = theLine.c_str();
      ++lineCounter;

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%s\n", line);
      #endif

      // Last line in file may or may not have an "END DATA" tag,
      // If it is found, break
      if (theLine.find("END_DATA") != std::string::npos)
         break;

      std::istringstream buf(line);
      std::istream_iterator<std::string> beg(buf), end;
      std::vector<std::string> tokens(beg, end);

      // Len 0 is the Windows linefeed line ending on Mac/Linux, so continue
      if (theLine.length() == 0)
         continue;

      if (tokens.size() < 14)
      {
         MessageInterface::ShowMessage("Len %d, %d tokens: ", theLine.length(), tokens.size());
         for (Integer i = 0; i < tokens.size(); ++i)
            MessageInterface::ShowMessage(" %s", tokens[i].c_str());
         MessageInterface::ShowMessage("\n");

         if (lineList.str() != "")
            lineList << ", ";
         lineList << lineCounter;
      }
      else
      {
         FluxData fD;
         fD.isHistData = false;

         // Set ref epoch to midnight for the date on the current line
         Integer year = atoi(tokens[1].c_str());
         Integer month = atoi(tokens[0].c_str());

         if ((month < 1) || (month > 12))
         {
            #ifdef DEBUG_SCHATTEN_READ
               MessageInterface::ShowMessage("Bad month: %d Line: \"%s\"\n", month, theLine.c_str());
            #endif
            if (lineList.str() != "")
               lineList << ", ";
            lineList << lineCounter;
         }

         Real mjd = ModifiedJulianDate(year, month, dom, hour, minute, sec);
         ++month;
         if (month == 13)
         {
            month = 1;
            ++year;
         }
         Real nextmjd = ModifiedJulianDate(year, month, dom, hour, minute, sec);

         fD.epoch = (mjd + nextmjd) * 0.5 - 0.5;

         // Nominal Timing
         for (Integer l = 0; l < 3; l++)
            fD.F107a[l] = atof(tokens[l + 2].c_str());
         fD.apSchatten[0] = atof(tokens[5].c_str());

         // Early Timing
         for (Integer l = 0; l < 3; l++)
            fD.F107a[l + 3] = atof(tokens[l + 6].c_str());
         fD.apSchatten[1] = atof(tokens[9].c_str());

         // Late Timing
         for (Integer l = 0; l < 3; l++)
            fD.F107a[l + 6] = atof(tokens[l + 10].c_str());
         fD.apSchatten[2] = atof(tokens[13].c_str());
         fD.index = -1;
         fD.id = -1;

         for (Integer l = 0; l < 8; l++)
            fD.kp[l] = -1;
         for (Integer l = 0; l < 8; l++)
            fD.ap[l] = -1;
         fD.apAvg = -1;
         fD.adjF107 = -1;
         fD.adjCtrF107a = -1;
         fD.obsF107 = -1;
         fD.obsCtrF107a = -1;

         predictFluxData.push_back(fD);
      }
   }

   if (lineList.str().length() > 0)
      MessageInterface::ShowMessage("WARNING: Line(s) [%s] of the predict data "
            "file were not read correctly\n", lineList.str().c_str());

   #ifdef DEBUG_GETFLUXINPUTS
      MessageInterface::ShowMessage("Loaded %d flux predict data records\n",
            predictFluxData.size());
   #endif

   if (predictFluxData.size() > 0)
   {
      predictStart = predictFluxData[0].epoch;
      predictEnd = predictFluxData[predictFluxData.size() - 1].epoch;

      Integer runningDays = 0;
      for (Integer i = 0; i < predictFluxData.size(); ++i)
      {
         predictFluxData[i].index = (Integer)(predictFluxData[i].epoch - predictStart);
         predictFluxData[i].id = i;
      }
   }

   return true;
}

//------------------------------------------------------------------------------
// FluxData SolarFluxReader::GetInputs(GmatEpoch epoch)
//------------------------------------------------------------------------------
/**
 * Gets raw Observed or Predicted input file data for teh input epoch.
 * 
 * HistoricAndNearTerm source is always given precedence if the requested epoch
 * is in the historic data section.  Otherwise, predicted data is used from the
 * predicted source.  If the requested data is not on a file (regardless of the
 * file type), the closest but later data trelative to the requested epoch is
 * returned,  and issue a (single) warning message is posted indicating that
 * data was not available, so boundary data is being used from a specified
 * source.
 *
 * @param epoch The epoch for the requested data
 *
 * @return A populated FluxData struct
 */
//------------------------------------------------------------------------------
SolarFluxReader::FluxData SolarFluxReader::GetInputs(GmatEpoch epoch)
{
   // Local instance for the return.  A more efficient approach would be to pass
   // in the struct to be filled.
   FluxData fD;
   Integer index;


   #ifdef DEBUG_GETFLUXINPUTS
      MessageInterface::ShowMessage("GetInputs(%.12lf): index = %d, using ",
            epoch, index);
   #endif

   // if the requested epoch falls beyond the
   // last item in the histFluxData, then search in predictFluxData
   if (epoch >= historicEnd)
   {
      #ifdef DEBUG_GETFLUXINPUTS
         MessageInterface::ShowMessage("predict data\n");
      #endif

      if ((predictFluxData.size() > 0))
      {
         // Init to the first predict record
         fD = predictFluxData[0];

         if (epoch < predictStart)
         {
            // Warn the user that the epoch is too early
            if (warnEpochBefore)
            {
               MessageInterface::ShowMessage("Warning: Requested epoch for "
                     "solar flux data is earlier than the starting epoch on "
                     "the predict flux file.  GMAT is using the first file "
                     "entry.\n");
               warnEpochBefore = false;
            }
         }
         else if (epoch > predictEnd)
         {
            // Warn the user that the epoch is too late
            if (warnEpochAfter)
            {
               MessageInterface::ShowMessage("Warning: Requested epoch for solar flux "
                  "data is later than the ending epoch on the predict flux file; "
                  "using the last file entry.\n");
               warnEpochAfter = false;
            }
            fD = predictFluxData[predictFluxData.size() - 1];
         }
         else
         {
            // Look up the data for epoch
            for (UnsignedInt i = 1; i < predictFluxData.size(); ++i)
            {
               if (predictFluxData[i].epoch > epoch)
               {
                  fD = predictFluxData[i-1];
                  if (predictedDataSource == 1) // using CSSI predict data
                     fD.index = i-1;
                  else // Schatten predict data
                     fD.index = -1;
                  break;
               }
            }
         }
      }
      else // Off the CSSI historic data and there is no predict data read
      {
         // Warn the user that the epoch is too late
         if (warnEpochAfter)
         {
            MessageInterface::ShowMessage("Warning: Requested epoch for solar flux "
               "data is later than the ending epoch on the historical flux file.  GMAT "
               "is using the last file entry.\n");
            warnEpochAfter = false;
         }                                          // fix bug GMT-5304
         if (histFluxData.size() > 0)
         {
            index = histFluxData.size() - 1;            // fix bug GMT-5304
            fD = histFluxData[index];                   // fix bug GMT-5304
            fD.index = index;                           // fix bug GMT-5304
         }
      }
   }
   else // Using historic data
   {
      // Find the index for the requested data
      index = (Integer) (epoch - historicStart);

      if (index < 0)
      {
         // Warn the user that the epoch is too early
         if (warnEpochBefore)
         {
            MessageInterface::ShowMessage("Warning: Requested epoch for solar "
                  "flux data is earlier than the starting epoch on the "
                  "historical flux file.  GMAT is using the first file entry.\n");
            warnEpochBefore = false;
         }
         index = 0;
      }

      if (epoch > historicDailyEnd) // Need to lookup the calendar bin
      {
         // Look up the data for epoch
         for (UnsignedInt i = ((Integer)historicDailyEnd - historicStart); i < histFluxData.size(); ++i)
         {
            if (histFluxData[i].epoch > epoch)
            {
               index = i-1;
               break;
            }
         }
      }

      #ifdef DEBUG_GETFLUXINPUTS
         MessageInterface::ShowMessage("observed data\n");
      #endif

      fD = histFluxData[index];
      fD.index = index;
   }

#ifdef DEBUG_FIRSTFEW_READS
   if (numberReadIndex < howMany)
   {
      MessageInterface::ShowMessage("Raw Data from read %d for epoch %.12lf:\n",
            numberReadIndex, epoch);
      MessageInterface::ShowMessage("   Record epoch:     %lf\n", fD.epoch);
      MessageInterface::ShowMessage("   F107 Obs:         %lf\n", fD.obsF107);
      MessageInterface::ShowMessage("   F107 Adj:         %lf\n", fD.adjF107);
      MessageInterface::ShowMessage("   F107a Obs Ctr:    %lf\n", fD.obsCtrF107a);
      MessageInterface::ShowMessage("   F107a Adj Ctr:    %lf\n", fD.adjCtrF107a);
      MessageInterface::ShowMessage("   Ap:               %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.ap[0], fD.ap[1], fD.ap[2], fD.ap[3], fD.ap[4], fD.ap[5], fD.ap[6], fD.ap[7]);
      MessageInterface::ShowMessage("   Ap Ave:           %lf\n", fD.apAvg);
      MessageInterface::ShowMessage("   Kp:               %lf %lf %lf %lf %lf %lf %lf %lf\n\n", fD.kp[0], fD.kp[1], fD.kp[2], fD.kp[3], fD.kp[4], fD.kp[5], fD.kp[6], fD.kp[7]);
      MessageInterface::ShowMessage("   F107a(Schatten):  %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.F107a[0], fD.F107a[1], fD.F107a[2], fD.F107a[3], fD.F107a[4], fD.F107a[5], fD.F107a[6], fD.F107a[7], fD.F107a[8]);
      MessageInterface::ShowMessage("   Ap(Schatten):     %lf %lf %lf\n", fD.apSchatten[0], fD.apSchatten[1], fD.apSchatten[2]);
      MessageInterface::ShowMessage("   Record %s obs data\n\n", (fD.isHistData ? "is" : "is not"));
   }
#endif

   #ifdef DEBUG_JR_INPUTS
      MessageInterface::ShowMessage(JR Inputs: "%lf %lf %lf\n", fD.kp[0],
            fD.obsF107, fD.obsCtrF107a);
   #endif

   return fD;
}

//------------------------------------------------------------------------------
// PrepareApData(GmatEpoch epoch, Integer index, FluxData &fD)
//------------------------------------------------------------------------------
/**
 * Prepares the data used by the MSISE models
 *
 * Function that replaces the ap data with data MSISE models need, and the F10.7
 * obs value with the value from the previous da
 *
 * On return, the input FluxData object contains the following Ap data:
 *
 *     Ap[0]  DAILY AP
 *     Ap[1]  3 HR AP INDEX FOR CURRENT TIME
 *     Ap[2]  3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME
 *     Ap[3]  3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME
 *     Ap[4]  3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME
 *     Ap[5]  AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR
 *              TO CURRENT TIME
 *     Ap[6]  AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 57 HRS PRIOR
 *             TO CURRENT TIME
 *
 * per the MSIS specifications.
 *
 * @param fD The data record that contains raw data for the requested epoch
 *
 * @return void
*/
//------------------------------------------------------------------------------
void SolarFluxReader::PrepareApData(SolarFluxReader::FluxData &fD, GmatEpoch epoch )
{
   #ifdef DEBUG_FILE_INDEXING
      MessageInterface::ShowMessage("Reading flux data; epoch = %12lf, "
         "fD.Epoch = %.12lf, index = %d\n", epoch, fD.epoch, fD.index);
   #endif
   
   Integer f107index = fD.index;

   if (fD.isHistData)
   {
      Real fracEpoch = epoch - fD.epoch;
      Integer subIndex = (Integer)floor(fracEpoch * 8);

      // F10.7 is measured at 8 pm (5pm before 5/31/91), so we use the current
      // row for data from 8 am on the current day to 8 am the next day.
      // f107index is used to track that piece.
      Real f107Offset = (epoch < f107RefEpoch ? 5.0/24.0 : 8.0/24.0);

      #ifdef OMIT_F107_OFFSET
         f107Offset = 0.0;
      #endif

      if (fracEpoch < f107Offset)
         f107index = (f107index > 0 ? f107index - 1 : 0);

      if (subIndex >= 8) // Off the end of the array
         subIndex = 7;

      Real apValues[32];
      Integer i = 0,j = 0;

      #ifdef DEBUG_FILE_INDEXING
         MessageInterface::ShowMessage("   subindex = %d\n", subIndex);
      #endif
      for (i = subIndex, j = 0; i >= 0; --i, ++j)
         apValues[j] = fD.ap[i];

      if (fD.index > 0)
      {
         FluxData fD_OneBefore = histFluxData[fD.index - 1];
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = fD_OneBefore.ap[i];
      }
      else
      {
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = histFluxData[0].ap[0];
      }
      if (fD.index > 1)
      {
         FluxData fD_TwoBefore = histFluxData[fD.index - 2];
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = fD_TwoBefore.ap[i];
      }
      else
      {
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = histFluxData[0].ap[0];
      }

      if ( fD.index > 2)
      {
         FluxData fD_ThreeBefore = histFluxData[fD.index - 3];
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = fD_ThreeBefore.ap[i];
      }
      else
      {
         for (i = 7; i >= 0; j++,i--)
            apValues[j] = histFluxData[0].ap[0];
      }

      for (i = 0; i < 7; i++)
      {
         if (i == 0)
            fD.ap[i] = fD.apAvg;
         if (i >= 1 && i <= 4)
            fD.ap[i] = apValues[i-1];
         if (i == 5)
         {
            fD.ap[i] = 0;
            for (Integer l = 0; l < 8; l++)
               fD.ap[i] += apValues[i+l-1];
            fD.ap[i] /= 8.0;
         }
         if (i == 6)
         {
            fD.ap[i] = 0;
            for (Integer l = 8; l < 16; l++)
               if ( apValues[i+l-2] >= 0)
                  fD.ap[i] += apValues[i+l-2];
            fD.ap[i] /= 8.0;
         }
      }

      // Update the F10.7 data and (if selected) interpolate
      if (interpolateFlux && (epoch >= historicStart))
      {
         Real vals[2];
         Real eps[2];
         Integer index = fD.id;

         // Pick the correct timespan
         eps[0] = histFluxData[index].epoch + f107Offset + 0.5;
         if (eps[0] > epoch)
         {
            eps[1] = eps[0];
            --index;
            if (index >= 0)
               eps[0] = histFluxData[index].epoch + f107Offset + 0.5;
            else
               index = 0;
         }
         else
         {
            eps[1] = (index < histFluxData.size() - 1 ?
                  histFluxData[index+1].epoch + f107Offset + 0.5 : eps[0] + 1.0);
         }
         vals[0] = histFluxData[index-1].obsF107;

         if (index < histFluxData.size()-1)
            vals[1] = histFluxData[index].obsF107;
         else
            vals[1] = vals[0];

         Real dt = eps[1] - eps[0];
         Real delta = epoch - eps[0];
         Real portion = delta / dt;
         fD.obsF107 = vals[0] + portion * (vals[1] - vals[0]);

         #ifdef DEBUG_FLUXINTERPOLATION
            MessageInterface::ShowMessage("F10.7 Interpolated from [%lf %lf] "
                  "to [%lf %lf] to get [%lf  %lf]\n", eps[0], vals[0], eps[1],
                  vals[1], epoch, fD.obsF107);
         #endif
      }
      else
      {
         // Daily value from previous day
         fD.obsF107 = (f107index > 0 ? histFluxData[f107index-1].obsF107 :
                                       histFluxData[f107index].obsF107);
      }

      // Average value from detected day
      fD.obsCtrF107a = histFluxData[f107index].obsCtrF107a;
   }
   else // predict data
   {
      bool useConstHistory = (histFluxData.size() > 0 ? false : true);

      if (predictedDataSource == 1)
      {
         // CSSI data handler for a predict source
         f107index = (fD.id >= 0 ? fD.id : fD.index);

         // Data sets for the various elements needed for Kp processing
         FluxData fD_forFlux, fD_forAverageFlux;

         // Kp reading at epoch - 6.7 Hrs, per Vallado and Finkleman, but Ap
         // apparently uses current epoch data, so no Ap offset is needed
         Real fracEpoch = epoch - fD.epoch;

         // Determine which Ap value is needed form the Ap record
         Integer subIndex = (Integer)floor(fracEpoch * 8);

         // Don't overflow
         if (subIndex >= 8)
            subIndex = 7;

         // F10.7 is measured @ 8 pm (5 pm before refEpoch, 5/31/91), so we use the
         // current row for data from 8 am on the current day to 8 am the next day.
         Real f107Offset = (epoch < f107RefEpoch ? 5.0/24.0 : 8.0/24.0);

         #ifdef OMIT_F107_OFFSET
            f107Offset = 0.0;
         #endif

         // Ap data prep
         FluxData fdData[4];

         // Ap data needs records going back as much as 3 days, so load those
         // records up

         // Current epoch data
         if (!FillRecord(fdData[0], epoch - 1.0))
            fdData[0].ap[0] = fdData[0].ap[1] = fdData[0].ap[2] =
            fdData[0].ap[3] = fdData[0].ap[4] = fdData[0].ap[5] =
            fdData[0].ap[6] = fdData[0].ap[7] = fdData[0].apAvg = constantAp;
         if (!FillRecord(fdData[1], epoch - 1.0))
            fdData[1].ap[0] = fdData[1].ap[1] = fdData[1].ap[2] =
            fdData[1].ap[3] = fdData[1].ap[4] = fdData[1].ap[5] =
            fdData[1].ap[6] = fdData[1].ap[7] = fdData[1].apAvg = constantAp;
         if (!FillRecord(fdData[2], epoch - 1.0))
            fdData[2].ap[0] = fdData[2].ap[1] = fdData[2].ap[2] =
            fdData[2].ap[3] = fdData[2].ap[4] = fdData[2].ap[5] =
            fdData[2].ap[6] = fdData[2].ap[7] = fdData[2].apAvg = constantAp;
         if (!FillRecord(fdData[3], epoch - 1.0))
            fdData[3].ap[0] = fdData[3].ap[1] = fdData[3].ap[2] =
            fdData[3].ap[3] = fdData[3].ap[4] = fdData[3].ap[5] =
            fdData[3].ap[6] = fdData[3].ap[7] = fdData[3].apAvg = constantAp;

         // If fracEpoch earlier than the measurement tod, use previous day record
         GmatEpoch f107Epoch;
         if (fracEpoch < f107Offset)
            f107Epoch = epoch - 1.0;
         else
            f107Epoch = epoch;

         // Use F10.7 record from previous day
         if (!FillRecord(fD_forFlux, f107Epoch - 1.0))
            fD_forFlux.obsF107 = constantF107;

         // Use current day F10.7a record
         if (!FillRecord(fD_forAverageFlux, f107Epoch))
            fD_forAverageFlux.obsCtrF107a = constantF107a;

         // Fill in the data
         // @note: Not averaging predict flux values. May need to add option

         fD.obsF107     = fD_forFlux.obsF107;
         fD.obsCtrF107a = fD_forAverageFlux.obsCtrF107a;

         #ifdef DEBUG_AP_DATA
            MessageInterface::ShowMessage("Ap [%lf %lf %lf %lf %lf %lf %lf %lf] -> ",
                  fD.ap[0], fD.ap[1], fD.ap[2], fD.ap[3], fD.ap[4], fD.ap[5],
                  fD.ap[6], fD.ap[7]);
         #endif

         LoadAPArray(fD.ap, fdData, subIndex);

         #ifdef DEBUG_AP_DATA
            MessageInterface::ShowMessage("[%lf %lf %lf %lf %lf %lf %lf %lf]\n",
                  fD.ap[0], fD.ap[1], fD.ap[2], fD.ap[3], fD.ap[4], fD.ap[5],
                  fD.ap[6], fD.ap[7]);
         #endif
      }
      else if (predictedDataSource == 2)
      {
         // Schatten handler
         if (interpolateFlux)
         {
            Real vals[2];
            Real eps[2];

            vals[0] = fD.F107a[schattenFluxIndex];
            eps[0]  = fD.epoch;
            if (fD.id < predictFluxData.size()-1)
            {
               vals[1] = predictFluxData[fD.id+1].F107a[schattenFluxIndex];
               eps[1]  = predictFluxData[fD.id+1].epoch;
            }
            else
            {
               // Make it flat
               vals[1] = vals[0];
               eps[1]  = eps[0] + 1.0;
            }

            Real dt = eps[1] - eps[0];
            Real delta = epoch - eps[0];
            Real portion = delta / dt;

            fD.obsF107     = vals[0] + portion * (vals[1] - vals[0]);
            fD.obsCtrF107a = fD.obsF107;

            #ifdef DEBUG_FLUXINTERPOLATION
            MessageInterface::ShowMessage("F10.7 Interpolated from [%lf %lf] "
                  "to [%lf %lf] to get [%lf  %lf]\n", eps[0], vals[0], eps[1],
                  vals[1], epoch, fD.obsF107);
            #endif
         }
         else
         {
            fD.obsF107     = fD.F107a[schattenFluxIndex];
            fD.obsCtrF107a = fD.F107a[schattenFluxIndex];
         }

         if (interpolateGeo)
         {
            Real apInterp = fD.apSchatten[schattenApIndex];

            Real vals[2];
            Real eps[2];

            vals[0] = fD.apSchatten[schattenApIndex];
            eps[0]  = fD.epoch;
            if (fD.id < predictFluxData.size()-1)
            {
               vals[1] = predictFluxData[fD.id+1].apSchatten[schattenApIndex];
               eps[1]  = predictFluxData[fD.id+1].epoch;
            }
            else
            {
               // Make it flat
               vals[1] = vals[0];
               eps[1]  = eps[0] + 1.0;
            }

            Real dt = eps[1] - eps[0];
            Real delta = epoch - eps[0];
            Real portion = delta / dt;

            apInterp += portion * (vals[1] - vals[0]);

            #ifdef DEBUG_FLUXINTERPOLATION
               MessageInterface::ShowMessage("Ap Interpolated from [%lf %lf] to "
                     "[%lf %lf] to get [%lf  %lf]\n", eps[0], vals[0], eps[1],
                     vals[1], epoch, apInterp);
            #endif

            for (Integer i = 0; i < 8; i++)
               fD.ap[i] = apInterp;
         }
         else
         {
            for (Integer i = 0; i < 8; i++)
               fD.ap[i] = fD.apSchatten[schattenApIndex];
         }
      }
      else
         throw SolarSystemException("Attempting to read solar flux data from "
               "an unknown source");
   }

#ifdef DUMP_FLUX_DATA
   MessageInterface::ShowMessage("%.12lf ==> %lf  %lf  [%lf %lf %lf %lf %lf %lf %lf %lf]\n",
         epoch, fD.obsF107, fD.obsCtrF107a, fD.ap[0], fD.ap[1], fD.ap[2],
         fD.ap[3], fD.ap[4], fD.ap[5], fD.ap[6], fD.ap[7]);
#endif

#ifdef DEBUG_FIRSTFEW_READS
   if (numberReadIndex < howMany)
   {
      MessageInterface::ShowMessage("Data after Ap Prep from read %d:\n", numberReadIndex);
      MessageInterface::ShowMessage("   Record epoch:     %lf\n", fD.epoch);
      MessageInterface::ShowMessage("   F107 Obs:         %lf\n", fD.obsF107);
      MessageInterface::ShowMessage("   F107 Adj:         %lf\n", fD.adjF107);
      MessageInterface::ShowMessage("   F107a Obs Ctr:    %lf\n", fD.obsCtrF107a);
      MessageInterface::ShowMessage("   F107a Adj Ctr:    %lf\n", fD.adjCtrF107a);
      MessageInterface::ShowMessage("   Ap:               %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.ap[0], fD.ap[1], fD.ap[2], fD.ap[3], fD.ap[4], fD.ap[5], fD.ap[6], fD.ap[7]);
      MessageInterface::ShowMessage("   Ap Ave:           %lf\n", fD.apAvg);
      MessageInterface::ShowMessage("   Kp:               %lf %lf %lf %lf %lf %lf %lf %lf\n\n", fD.kp[0], fD.kp[1], fD.kp[2], fD.kp[3], fD.kp[4], fD.kp[5], fD.kp[6], fD.kp[7]);
      MessageInterface::ShowMessage("   F107a(Schatten):  %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.F107a[0], fD.F107a[1], fD.F107a[2], fD.F107a[3], fD.F107a[4], fD.F107a[5], fD.F107a[6], fD.F107a[7], fD.F107a[8]);
      MessageInterface::ShowMessage("   Ap(Schatten):     %lf %lf %lf\n\n", fD.apSchatten[0], fD.apSchatten[1], fD.apSchatten[2]);

      ++numberReadIndex;
   }
#endif
}


/**
 * Packages the Ap data array per the MSISE specifications
 *
 * The Ap data used in the MSISE90 (and presumably other MSISE data sets) is
 * packaged differently from the file data:
 *
 *        AP - MAGNETIC INDEX(DAILY) OR WHEN SW(9)=-1. :
 *           - ARRAY CONTAINING:
 *             (1) DAILY AP
 *             (2) 3 HR AP INDEX FOR CURRENT TIME
 *             (3) 3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME
 *             (4) 3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME
 *             (5) 3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME
 *             (6) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR
 *                    TO CURRENT TIME
 *             (7) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 59 HRS PRIOR
 *                    TO CURRENT TIME
 *
 * This method collects the Ap data and packs it into the expected form.
 *
 * @param apArray Ap array from the fD data set that is being filled
 * @param fdData  Array of fD data set from the current day, yesterday,
 *                2 days ago, and 3 days ago
 * @param currentDayApIndex Index into requested epoch's time of day in the array
 */
void SolarFluxReader::LoadAPArray(Real apArray[8],
      SolarFluxReader::FluxData fdData[4],
      Integer currentDayApIndex)
{
   SolarFluxReader::FluxData current = fdData[0];
   Integer index = currentDayApIndex, fdIndex = 0;

   // (1) DAILY AP
   apArray[0] = current.apAvg;

   // (2) 3 HR AP INDEX FOR CURRENT TIME
   apArray[1] = current.ap[index];

   // 3 HR AP INDEX FOR 3 HRS BEFORE CURRENT TIME
   if (index == 0)
   {
      index = 7;
      ++fdIndex;
      current = fdData[fdIndex];
   }
   else
      index = index - 1;
   apArray[2] = current.ap[index];

   // (4) 3 HR AP INDEX FOR 6 HRS BEFORE CURRENT TIME
   if (index == 0)
   {
      index = 7;
      ++fdIndex;
      current = fdData[fdIndex];
   }
   else
      index = index - 1;
   apArray[3] = current.ap[index];

   // (5) 3 HR AP INDEX FOR 9 HRS BEFORE CURRENT TIME
   if (index == 0)
   {
      index = 7;
      ++fdIndex;
      current = fdData[fdIndex];
   }
   else
      index = index - 1;
   apArray[4] = current.ap[index];

   // (6) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 12 TO 33 HRS PRIOR TO CURRENT TIME
   Real ave = 0.0;
   for (Integer i = 0; i < 8; ++i)
   {
      if (index == 0)
      {
         index = 7;
         ++fdIndex;
         current = fdData[fdIndex];
      }
      else
         index = index - 1;
      ave += current.ap[index];
   }
   apArray[5] = ave / 8.0;

   // (7) AVERAGE OF EIGHT 3 HR AP INDICIES FROM 36 TO 59 HRS PRIOR TO CURRENT TIME
   ave = 0.0;
   for (Integer i = 0; i < 8; ++i)
   {
      if (index == 0)
      {
         index = 7;
         ++fdIndex;
         current = fdData[fdIndex];
      }
      else
         index = index - 1;
      ave += current.ap[index];
   }
   apArray[6] = ave / 8.0;
}

//------------------------------------------------------------------------------
// PrepareKpData(GmatEpoch epoch, Integer index, FluxData &fD)
//------------------------------------------------------------------------------
/**
 * Function that replaces the Kp data with data the Jacchia Roberts model needs
 *
 * @param fD The data record that contains raw data for the requested epoch
 *
 * @return void
*/
//------------------------------------------------------------------------------
void SolarFluxReader::PrepareKpData(SolarFluxReader::FluxData &fD, GmatEpoch epoch )
{
   Integer f107index = fD.index;

   if (fD.isHistData)
   {
      // Historic data management
      f107index = fD.id;

      // Fill in fD.kp[0] so it contains the reading at epoch - 6.7 Hrs, per
      // Vallado and Finkleman
      Real fracEpoch = epoch - fD.epoch;
      Real fracEpochKp = fracEpoch - 6.7/24.0;
      Integer subIndex = (Integer)floor(fracEpochKp * 8);

      // F10.7 is measured at 8 pm (5pm before 5/31/91), so we use the current
      // row for data from 8 am on the current day to 8 am the next day.
      // f107index is used to track that piece.
      Real f107Offset = (epoch < f107RefEpoch ? 5.0/24.0 : 8.0/24.0);

      #ifdef OMIT_F107_OFFSET
         f107Offset = 0.0;
      #endif

      if (fracEpoch < f107Offset)
         f107index = (f107index > 0 ? f107index - 1 : 0);

      if (subIndex >= 8)
         subIndex = 7;

      if (subIndex > 0)
         fD.kp[0] = fD.kp[subIndex];

      if (subIndex < 0 && fD.id > 0)
      {
         FluxData fD_OneBefore = histFluxData[fD.id - 1];
         fD.kp[0] = fD_OneBefore.kp[8+subIndex];
      }

      // Daily value from previous day
      fD.obsF107 = (f107index > 0 ? histFluxData[f107index-1].obsF107 :
                                    histFluxData[f107index].obsF107);

      // Average value from detected day
      fD.obsCtrF107a = (f107index > 0 ? histFluxData[f107index-1].obsCtrF107a :
                                        histFluxData[f107index].obsCtrF107a);

      #ifdef DEBUG_FIRST_CALL
         std::stringstream msg;
         msg   << "Epoch:                  " << epoch
               << "\nfracEpoch:            " << fracEpoch
               << "\nfracEpochKp:          " << fracEpochKp
               << "\nSubindex:             " << subIndex
               << "\nSource record index:  " << f107index
               << "\n     Epoch: " << fD.epoch
               << "\n     f107:  " << fD.obsF107
               << "\n     f107a: " << fD.obsCtrF107a
               << "\n     Kp:    " << fD.kp[0]
               << "\n";
         throw SolarSystemException(msg.str());
      #endif
   }
   // Predicted data, from Schatten predict files
   else if (fD.index == -1) // Schatten predict data
   {
      fD.obsF107     = fD.F107a[schattenFluxIndex];
      fD.obsCtrF107a = fD.F107a[schattenFluxIndex];
      Real kp = ConvertApToKp(fD.apSchatten[schattenApIndex]);
      for (Integer i = 0; i < 8; i++)
         fD.kp[i] = kp;
   }
   else
   {
      // Predicted data; it's a bit tricky at changeover from historic data.
      // Set a flag for the historic data source
      bool useConstHistory = (histFluxData.size() > 0 ? false : true);

      f107index = (fD.id >= 0 ? fD.id : fD.index);

      // Data sets for the various elements needed for Kp processing
      FluxData fD_forKp, fD_forFlux, fD_forAverageFlux;

      // determine indexing for Kp reading at epoch - 6.7 Hrs, per
      // Vallado and Finkleman
      Real fracEpoch = epoch - fD.epoch;
      Real fracEpochKp = fracEpoch - 6.7/24.0;

      // Determine which Kp value is needed form the Kp record
      Integer subIndex = (Integer)floor(fracEpochKp * 8);
      // Don't overflow (negative values are handled later)
      if (subIndex >= 8)
         subIndex = 7;

      // F10.7 is measured @ 8 pm (5 pm before refEpoch, 5/31/91), so we use the
      // current row for data from 8 am on the current day to 8 am the next day.
      Real f107Offset = (epoch < f107RefEpoch ? 5.0/24.0 : 8.0/24.0);

      #ifdef OMIT_F107_OFFSET
         f107Offset = 0.0;
      #endif

      // If fracEpoch earlier than the measurement tod, index one record earlier
      if (fracEpoch < f107Offset)
         f107index = f107index - 1;

      // Now look up the records to use

//      /*
         // New approach

      if (subIndex >= 0)
      {
         if (!FillRecord(fD_forKp, epoch))
            fD_forKp.kp[0] = fD_forKp.kp[1] = fD_forKp.kp[2] =
            fD_forKp.kp[3] = fD_forKp.kp[4] = fD_forKp.kp[5] =
            fD_forKp.kp[6] = fD_forKp.kp[7] = constantKp;
      }
      else // Use the previous day's record
      {
         if (!FillRecord(fD_forKp, epoch - 1.0))
            fD_forKp.kp[0] = fD_forKp.kp[1] = fD_forKp.kp[2] =
            fD_forKp.kp[3] = fD_forKp.kp[4] = fD_forKp.kp[5] =
            fD_forKp.kp[6] = fD_forKp.kp[7] = constantKp;

         subIndex += 8;
      }

      // Now look up the records to use
      if (fracEpoch >= f107Offset)
      {
         // Previous day for flux
         if (!FillRecord(fD_forFlux, epoch - 1.0))
            fD_forFlux.obsF107 = constantF107;

         // current day for average flux
         fD_forAverageFlux = fD;
      }
      else // Use yesterday's set record
      {
         if (!FillRecord(fD_forFlux, epoch - 2.0))
            fD_forFlux.obsF107 = constantF107;
         if (!FillRecord(fD_forAverageFlux, epoch - 1.0))
            fD_forAverageFlux.obsCtrF107a = constantF107a;
      }
//      */

//      // Code that has been sanity checked, but that has issues in the monthly ranges
//      // Edited for new calls in future data, but not at Hist-> Pred boundary
//      // <<<<< From here >>>>>
//      if (subIndex >= 0)
//         fD_forKp = fD;
//      else
//      {
//         // Need to dig out historic data
//         if (f107index > 0)
//         {
////            fD_forKp = predictFluxData[f107index - 1];
//            FillRecord(fD_forKp, epoch - 1.0);
//         }
//         else
//         {
//            // Files don't know the const values; for now, use the first
//            // predicted data record
//            if (useConstHistory)
//               fD_forKp = predictFluxData[0];
//            else
//               fD_forKp = histFluxData[histFluxData.size() - 1];
//         }
//         // Set the offset into the record
//         subIndex += 8;
//      }
//
//      // Set the record for the daily and average flux values
//      if (fracEpoch < f107Offset)
//      {
//         // Need the daily record from 2 days ago
//         if (f107index > 1)
//         {
////            fD_forFlux = predictFluxData[f107index - 2];
//            FillRecord(fD_forFlux, epoch - 2.0);
//         }
//         else
//         {
//            // Files don't know the const values; for now, use the first
//            // predicted data record
//            if (useConstHistory)
//               fD_forFlux = predictFluxData[0];
//            else
//            {
//               // Paranoia: handle a single historic record
//               Integer index = (histFluxData.size() > 1 ?
//                  histFluxData.size() - 2 : histFluxData.size() - 1);
//               fD_forFlux = histFluxData[index];
//            }
//         }
//
//         // Need yesterday's average record
//         if (f107index > 0)
//         {
////            fD_forAverageFlux = predictFluxData[f107index - 1];
//            FillRecord(fD_forAverageFlux, epoch - 1.0);
//         }
//         else
//         {
//            // Files don't know the const values; for now, use the first
//            // predicted data record
//            if (useConstHistory)
//               fD_forAverageFlux = predictFluxData[0];
//            else
//               fD_forAverageFlux = histFluxData[histFluxData.size() - 1];
//         }
//      }
//      else
//      {
//         // Need the record from 1 day ago
//         if (f107index > 0)
//         {
////            fD_forFlux = predictFluxData[f107index - 1];
//            FillRecord(fD_forFlux, epoch - 1.0);
//         }
//         else
//         {
//            // Files don't know the const values; for now, use the first
//            // predicted data record
//            if (useConstHistory)
//               fD_forFlux = predictFluxData[0];
//            else
////               fD_forFlux = histFluxData[histFluxData.size() - 1];
//               FillRecord(fD_forFlux, epoch - 1.0);
//         }
//         // Use the input record for the average
//         fD_forAverageFlux = fD;
//      }
//      // <<<<< To here >>>>>

      fD.kp[0]       = fD_forKp.kp[subIndex];
      fD.obsF107     = fD_forFlux.obsF107;
      fD.obsCtrF107a = fD_forAverageFlux.obsCtrF107a;
   }

#ifdef DUMP_FLUX_DATA
   MessageInterface::ShowMessage("%.12lf ==> %lf  %lf  [%lf %lf %lf %lf %lf %lf %lf %lf]\n",
         epoch, fD.obsF107, fD.obsCtrF107a, fD.kp[0], fD.kp[1], fD.kp[2],
         fD.kp[3], fD.kp[4], fD.kp[5], fD.kp[6], fD.kp[7]);
#endif

#ifdef DEBUG_FIRSTFEW_READS
   if (numberReadIndex < howMany)
   {
      MessageInterface::ShowMessage("Data after Kp Prep from read %d:\n", numberReadIndex);
      MessageInterface::ShowMessage("   Record epoch:     %lf\n", fD.epoch);
      MessageInterface::ShowMessage("   F107 Obs:         %lf\n", fD.obsF107);
      MessageInterface::ShowMessage("   F107 Adj:         %lf\n", fD.adjF107);
      MessageInterface::ShowMessage("   F107a Obs Ctr:    %lf\n", fD.obsCtrF107a);
      MessageInterface::ShowMessage("   F107a Adj Ctr:    %lf\n", fD.adjCtrF107a);
      MessageInterface::ShowMessage("   Ap:               %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.ap[0], fD.ap[1], fD.ap[2], fD.ap[3], fD.ap[4], fD.ap[5], fD.ap[6], fD.ap[7]);
      MessageInterface::ShowMessage("   Ap Ave:           %lf\n", fD.apAvg);
      MessageInterface::ShowMessage("   Kp:               %lf %lf %lf %lf %lf %lf %lf %lf\n\n", fD.kp[0], fD.kp[1], fD.kp[2], fD.kp[3], fD.kp[4], fD.kp[5], fD.kp[6], fD.kp[7]);
      MessageInterface::ShowMessage("   F107a(Schatten):  %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", fD.F107a[0], fD.F107a[1], fD.F107a[2], fD.F107a[3], fD.F107a[4], fD.F107a[5], fD.F107a[6], fD.F107a[7], fD.F107a[8]);
      MessageInterface::ShowMessage("   Ap(Schatten):     %lf %lf %lf\n\n", fD.apSchatten[0], fD.apSchatten[1], fD.apSchatten[2]);

      ++numberReadIndex;
   }
#endif
}

//------------------------------------------------------------------------------
// void SolarFluxReader::GetEpochs(GmatEpoch &hStart, GmatEpoch &hEnd, GmatEpoch &pStart,
//                GmatEpoch &pEnd)
//------------------------------------------------------------------------------
/**
 * Accessor to retrieve the epoch data from the files
 *
 * @param hStart Epoch of the first historical data point
 * @param hEnd Epoch of the last historical data point
 * @param PStart Epoch of the first predict data point
 * @param pEnd Epoch of the last predict data point
 */
//------------------------------------------------------------------------------
void SolarFluxReader::GetEpochs(GmatEpoch &hStart, GmatEpoch &hEnd, GmatEpoch &pStart,
               GmatEpoch &pEnd)
{
   hStart = historicStart;
   hEnd = historicEnd;
   pStart = predictStart;
   pEnd = predictEnd;
}


//------------------------------------------------------------------------------
// Real ConvertApToKp(Real ap)
//------------------------------------------------------------------------------
/**
 * Conversion routine to go Ap -> Kp
 *
 * The current implementation performs linear interpolation.  Vallado recommends
 * cubic splines instead.
 *
 * @param ap The input Ap value
 *
 * @return The corresponding Kp
 */
//------------------------------------------------------------------------------
Real SolarFluxReader::ConvertApToKp(Real ap)
{
   Real kp = 0.0;
   Real apl, apr, kpl, kpr;

   // For now, linear interpolate.  Vallado recommeds splines.
   if (ap <= 2.0)
   {
      apl = 0.0;      apr = 2.0;
      kpl = 0.0;      kpr = 0.33;
   }
   else if (ap <= 3.0)
   {
      apl = 2.0;      apr = 3.0;
      kpl = 0.33;     kpr = 0.67;
   }
   else if (ap <= 4.0)
   {
      apl = 3.0;      apr = 4.0;
      kpl = 0.67;     kpr = 1.0;
   }
   else if (ap <= 5.0)
   {
      apl = 4.0;      apr = 5.0;
      kpl = 1.0;      kpr = 1.33;
   }
   else if (ap <= 6.0)
   {
      apl = 5.0;      apr = 6.0;
      kpl = 1.33;     kpr = 1.67;
   }
   else if (ap <= 7.0)
   {
      apl = 6.0;      apr = 7.0;
      kpl = 1.67;     kpr = 2.0;
   }
   else if (ap <= 9.0)
   {
      apl = 7.0;      apr = 9.0;
      kpl = 2.0;      kpr = 2.33;
   }
   else if (ap <= 12.0)
   {
      apl = 9.0;      apr = 12.0;
      kpl = 2.33;     kpr = 2.67;
   }
   else if (ap <= 15.0)
   {
      apl = 12.0;     apr = 15.0;
      kpl = 2.67;     kpr = 3.0;
   }
   else if (ap <= 18.0)
   {
      apl = 15.0;     apr = 18.0;
      kpl = 3.0;      kpr = 3.33;
   }
   else if (ap <= 22.0)
   {
      apl = 18.0;     apr = 22.0;
      kpl = 3.33;     kpr = 3.67;
   }
   else if (ap <= 27.0)
   {
      apl = 22.0;     apr = 27.0;
      kpl = 3.67;     kpr = 4.0;
   }
   else if (ap <= 32.0)
   {
      apl = 27.0;     apr = 32.0;
      kpl = 4.0;      kpr = 4.33;
   }
   else if (ap <= 39.0)
   {
      apl = 32.0;     apr = 39.0;
      kpl = 4.33;     kpr = 4.67;
   }
   else if (ap <= 48.0)
   {
      apl = 39.0;     apr = 48.0;
      kpl = 4.67;     kpr = 5.0;
   }
   else if (ap <= 56.0)
   {
      apl = 48.0;     apr = 56.0;
      kpl = 5.0;      kpr = 5.33;
   }
   else if (ap <= 67.0)
   {
      apl = 56.0;     apr = 67.0;
      kpl = 5.33;     kpr = 5.67;
   }
   else if (ap <= 80.0)
   {
      apl = 67.0;     apr = 80.0;
      kpl = 5.67;     kpr = 6.0;
   }
   else if (ap <= 94.0)
   {
      apl = 80.0;     apr = 94.0;
      kpl = 6.0;      kpr = 6.33;
   }
   else if (ap <= 111.0)
   {
      apl = 94.0;     apr = 111.0;
      kpl = 6.33;     kpr = 6.67;
   }
   else if (ap <= 132.0)
   {
      apl = 111.0;    apr = 132.0;
      kpl = 6.67;     kpr = 7.0;
   }
   else if (ap <= 154.0)
   {
      apl = 132.0;    apr = 154.0;
      kpl = 7.0;      kpr = 7.33;
   }
   else if (ap <= 179.0)
   {
      apl = 154.0;    apr = 179.0;
      kpl = 7.33;     kpr = 7.67;
   }
   else if (ap <= 207.0)
   {
      apl = 179.0;    apr = 207.0;
      kpl = 7.67;     kpr = 8.0;
   }
   else if (ap <= 236.0)
   {
      apl = 207.0;    apr = 236.0;
      kpl = 8.0;      kpr = 8.33;
   }
   else if (ap <= 300.0)
   {
      apl = 236.0;    apr = 300.0;
      kpl = 8.33;     kpr = 8.67;
   }
   else
   {
      apl = 300.0;    apr = 400.0;
      kpl = 8.67;     kpr = 9.0;
   }

   kp = kpl + (ap - apl) * (kpr - kpl) / (apr - apl);

   return kp;
}


//------------------------------------------------------------------------------
// bool FillRecord(FluxData &fD, GmatEpoch forEpoch)
//------------------------------------------------------------------------------
/**
 * Locates a flux data record for an input epoch
 *
 * @param fD A FluxData record that needs to be filled in
 * @param forEpoch The epoch of the data requested
 *
 * @return true if a record was filled in, false if not
 */
//------------------------------------------------------------------------------
bool SolarFluxReader::FillRecord(FluxData &fD, GmatEpoch forEpoch)
{
   bool retval = false;
   bool useConstHistoric = (histFluxData.size() == 0);
   bool useConstPredicts = (predictFluxData.size() == 0);

   if ((forEpoch < predictStart) && !useConstHistoric)
   {
      // Find the index for the requested data
      Integer index = (Integer) (forEpoch - historicStart);

      if (index >= 0)
      {
         if (index < histFluxData.size() - 1)
         {
            if ((forEpoch < histFluxData[index+1].epoch) &&
                (forEpoch >= histFluxData[index].epoch))
            {
               fD = histFluxData[index];
               retval = true;
            }
            else // Epoch not bracketed
            {
               if (forEpoch < histFluxData[index].epoch)
               {
                  while ((forEpoch < histFluxData[index].epoch) && (index >= 0))
                     --index;
                  if (index >= 0)
                  {
                     fD = histFluxData[index];
                     retval = true;
                  }
               }
               else
               {
                  // Walk the array, stopping with the last record
                  while ((forEpoch > histFluxData[index].epoch) &&
                        (index < histFluxData.size()))
                     ++index;
                  fD = histFluxData[index];
                  retval = true;
               }
            }
         }
         else // Use the last record. Note:"too late" warning is generated elsewhere
         {
            fD = histFluxData[histFluxData.size() - 1];
            retval = true;
         }
      }
      else // Use the first record. Note:"too early" warning is generated elsewhere
      {
         fD = histFluxData[0];
         retval = true;
      }
   }
   else if (!useConstPredicts)
   {
      if (forEpoch >= predictStart)
      {
         // Find the record in the predict data set
         UnsignedInt index;
         for (index = 0; index < predictFluxData.size(); ++index)
            if (index+1 < predictFluxData.size())
               if (predictFluxData[index+1].epoch > forEpoch)
                  break;

         fD = predictFluxData[index];
         retval = true;
      }
      else if (!useConstHistoric)
      {
         // Might never hit this case
         fD = predictFluxData[0];
         retval = true;
      }
   }

   // Return is false if fD was not set
   return retval;
}

//------------------------------------------------------------------------------
// void SetConstValues(Real f107, Real f107a, Real kp, Real ap)
//------------------------------------------------------------------------------
/**
 * Sets constant values
 *
 * @param f107  Constant F10.7 setting
 * @param f107a Constant F10.7a setting
 * @param kp    Constant Kp setting
 * @param ap    Constant Ap setting
 */
//------------------------------------------------------------------------------
void SolarFluxReader::SetConstValues(Real f107, Real f107a, Real kp, Real ap)
{
   constantF107  = f107;
   constantF107a = f107a;
   constantKp    = kp;
   constantAp    = ap;
}

//------------------------------------------------------------------------------
// void SetSchattenFlags(Integer timingSet, Integer magnitudeSet)
//------------------------------------------------------------------------------
/**
 * Sets the indices into the data for Schatten predicts
 *
 * @param timingSet  Sets timing selection: -1 => early, 0 => nominal, +1 => late
 * @param magnitudeSet Sets magnitude selection: -1 => -2 sigma, 0 => nominal,
 *                                               +1 =>  +2 sigma
 */
//------------------------------------------------------------------------------
void SolarFluxReader::SetSchattenFlags(Integer timingSet, Integer magnitudeSet)
{
   #ifdef DEBUG_SCHATTEN_SETTINGS
      MessageInterface::ShowMessage("Setting flux reader timing to %d and "
            "error model to %d\n", timingSet, magnitudeSet);
   #endif

   if (timingSet == -1)
   {
      if (magnitudeSet == -1)
         schattenFluxIndex = 5;
      if (magnitudeSet == 0)
         schattenFluxIndex = 3;
      if (magnitudeSet == 1)
         schattenFluxIndex = 4;
      schattenApIndex = 1;
   }

   if (timingSet == 0)
   {
      if (magnitudeSet == -1)
         schattenFluxIndex = 2;
      if (magnitudeSet == 0)
         schattenFluxIndex = 0;
      if (magnitudeSet == 1)
         schattenFluxIndex = 1;
      schattenApIndex = 0;
   }

   if (timingSet == 1)
   {
      if (magnitudeSet == -1)
         schattenFluxIndex = 8;
      if (magnitudeSet == 0)
         schattenFluxIndex = 6;
      if (magnitudeSet == 1)
         schattenFluxIndex = 7;
      schattenApIndex = 2;
   }

   #ifdef DEBUG_FIRSTFEW_READS
      numberReadIndex = 0;
   #endif
}

//------------------------------------------------------------------------------
// void SetHistoricDataSource(Integer dataSource)
//------------------------------------------------------------------------------
/**
 * Sets the data source for the historic data
 *
 * @param dataSource  Sets historic data source.
 * 0 = ConstantFluxAndGeoMag  1 = CSSISpaceWeatherFile
 */
//------------------------------------------------------------------------------
void SolarFluxReader::SetHistoricDataSource(Integer dataSource)
{
   historicDataSource = dataSource;

   if (historicDataSource < 0 || historicDataSource > 1)
   {
      throw SolarSystemException("SolarFluxReader: The historic data source is"
                  "invalid. Valid options are constant or a CSSI Space Weather file.\n");
   }
}

//------------------------------------------------------------------------------
// void SetPredictedDataSource(Integer dataSource)
//------------------------------------------------------------------------------
/**
 * Sets the data source for the prediction data
 *
 * @param dataSource  Sets predicted data source.
 * 0 = ConstantFluxAndGeoMag  1 = CSSISpaceWeatherFile  2 = SchattenFile
 */
//------------------------------------------------------------------------------
void SolarFluxReader::SetPredictedDataSource(Integer dataSource)
{
   predictedDataSource = dataSource;

   if (predictedDataSource < 0 || predictedDataSource > 2)
   {
      throw SolarSystemException("SolarFluxReader: The predicted data source is"
                  "invalid. Valid options are constant, a CSSI Space Weather"
                  "file, or a Schatten file.\n");
   }
}


