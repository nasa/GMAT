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
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II.
//
/**
 * SolarFluxReader class will be used by the AtomsphereModel class.
 */
//------------------------------------------------------------------------------


#ifndef SOLARFLUXREADER_HPP
#define SOLARFLUXREADER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include "DateUtil.hpp"
#include "GmatConstants.hpp"
#include "GmatBase.hpp"

class GMAT_API SolarFluxReader
{

public:
   SolarFluxReader();
   SolarFluxReader(const SolarFluxReader &sfr);
   SolarFluxReader& operator=(const SolarFluxReader &sfr);
   ~SolarFluxReader(); 
   
   struct FluxDataCSSI
   {
      GmatEpoch epoch;
      Real kp[8];
      Real ap[8];
      Real apAvg;
      Real adjF107;
      Real adjCtrF107a;
      Real obsF107;
      Real obsCtrF107a;

      bool isHistData;
   };

   struct FluxData : FluxDataCSSI
   {
      // the first three items is NOMINAL, the second three items
      // is EARLY, and the last three items is LATE TIMING.
      Real F107a[9];
      // the first is NOMINAL, the second is EARLY, and the last is LATE TIMING.
      Real apSchatten[3];

      // FluxData();
      // FluxData(const FluxData &fD);
      // FluxData &operator=(const FluxData &fD);
      FluxData()
         {
            // Default all to -1
            epoch = -1.0;
            apAvg = -1.0;
            adjF107 = -1.0;
            adjCtrF107a = -1.0;
            obsF107 = -1.0;
            obsCtrF107a = -1.0;
            
            for (Integer j = 0; j < 9; j++)
               F107a[j] = -1.0;
            for (Integer j = 0; j < 3; j++)
               apSchatten[j] = -1.0;
            for (Integer j = 0; j < 8; j++)
            {
               kp[j] = -1.0;
               ap[j] = -1.0;
            }
            
            index = -1;
            id = -1;
            isHistData = true;
         }
      FluxData(const FluxData &fD)
         {
            epoch = fD.epoch;
            for (Integer j = 0; j < 8; j++)
            {
               kp[j] = fD.kp[j];
               ap[j] = fD.ap[j];
            }
            apAvg = fD.apAvg;
            adjF107 = fD.adjF107;
            adjCtrF107a = fD.adjCtrF107a;
            obsF107 = fD.obsF107;
            obsCtrF107a = fD.obsCtrF107a;
            for (Integer j = 0; j < 9; j++)
               F107a[j] = fD.F107a[j];
            for (Integer j = 0; j < 3; j++)
               apSchatten[j] = fD.apSchatten[j];
            index = fD.index;
            id = fD.id;
            isHistData = fD.isHistData;
         }
      FluxData &operator=(const FluxData &fD)
         {
            if (this != &fD)
            {
               epoch = fD.epoch;
               for (Integer j = 0; j < 8; j++)
               {
                  kp[j] = fD.kp[j];
                  ap[j] = fD.ap[j];
               }
               apAvg = fD.apAvg;
               adjF107 = fD.adjF107;
               adjCtrF107a = fD.adjCtrF107a;
               obsF107 = fD.obsF107;
               obsCtrF107a = fD.obsCtrF107a;
               for (Integer j = 0; j < 9; j++)
                  F107a[j] = fD.F107a[j];
               for (Integer j = 0; j < 3; j++)
                  apSchatten[j] = fD.apSchatten[j];
               index = fD.index;
               id = fD.id;
               isHistData = fD.isHistData;
            }
            
            return *this;
         }
      
      // Used in Schatten file indexing
      Integer index;
      Integer id;
   };
  
private:
   const char * begin_HistTag;
   const char * end_HistTag;

   const char * begin_DailyPred;
   const char * end_DailyPred;

   const char * begin_MonthlyPred;
   const char * end_MonthlyPred;

   const char * begin_MonthlyFit;
   const char * end_MonthlyFit;

   /// offset required to start reading observation (BEGIN OBSERVED) tag
   std::streamoff histOffset;
   /// offset required to start reading prediction (BEGIN DAILY_PREDICTED) tag
   std::streamoff predictOffset;
   /// each line in the file
   const char *line;

   std::string histFileName;
   std::string predictFileName;
   
   /// Obs File Stream
   std::ifstream inHist;
   /// Predict File Stream
   std::ifstream inPredict;
   /// CSSI data array 
   std::vector<FluxData> histFluxData;
   /// Schatten data array
   std::vector<FluxData> predictFluxData;

   GmatEpoch historicStart;
   GmatEpoch historicDailyEnd;
   GmatEpoch historicEnd;
   GmatEpoch predictStart;
   GmatEpoch predictEnd;

   /// Index for Schatten flux setting
   Integer schattenFluxIndex;
   /// Index for Schatten Ap value
   Integer schattenApIndex;
      
   /// Flag used to indicate that the "Too early" warning not yet issued
   bool warnEpochBefore;
   /// Flag used to indicate that the "Too late" warning not yet issued
   bool warnEpochAfter;

   /// Epoch reference point for when the F10.7 observations changed location
   GmatEpoch f107RefEpoch;
   /// Flag used to toggle interpolation of the f10.7 values
   bool interpolateFlux;
   /// Flag used to toggle interpolation for the geomagnetic index (predict only)
   bool interpolateGeo;

   /// Flag used to toggle between generating historic data from constants or a
   /// Weather file
   Integer historicDataSource;
   /// Flag used to toggle between generating predict data from constants, a
   /// Weather file, or a Schatten file
   Integer predictedDataSource;

   /// F107a to use when in constant data regime
   Real constantF107;
   /// Running average F107a to use when in constant data regime
   Real constantF107a;
   /// Constant Kp value to use when in constant data regime
   Real constantKp;
   /// Constant Ap value to use when in constant data regime. (This is computed)
   Real constantAp;

   bool LoadCSSIHistoric();
   bool LoadCSSIPredict();
   bool LoadSchattenData();
   Real ConvertApToKp(Real ap);
   void LoadAPArray(Real apArray[8], SolarFluxReader::FluxData fdData[4],
         Integer currentDayApIndex);

   bool FillRecord(FluxData &fD, GmatEpoch forEpoch);

public:
   /// Open CSSI and Schatten files if opened
   bool Open();
   /// Close CSSI and Schatten files if opened
   bool Close();
   /// Load data from both opened files
   bool LoadFluxData(const std::string &histFile = "", const std::string &predictFile = "");
   /// Get Flux data from either of two vectors filled in during LoadFluxData
   FluxData GetInputs(GmatEpoch epoch);

   void GetEpochs(GmatEpoch &hStart, GmatEpoch &hEnd, GmatEpoch &pStart,
                  GmatEpoch &pEnd);

   void PrepareApData(FluxData &fD, GmatEpoch epoch);
   void PrepareKpData(SolarFluxReader::FluxData &fD, GmatEpoch epoch);
   void SetSchattenFlags(Integer timingSet, Integer magnitudeSet);
   void SetConstValues(Real f107, Real f107a, Real kp, Real ap);
   void SetHistoricDataSource(Integer dataSource);
   void SetPredictedDataSource(Integer dataSource);
};


#endif

