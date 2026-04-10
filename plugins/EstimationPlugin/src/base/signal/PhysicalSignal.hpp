//------------------------------------------------------------------------------
//                           PhysicalSignal
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
// Created: Jan 9, 2014
/**
 * Class used to model instantaneous signals
 */
//------------------------------------------------------------------------------

#ifndef PhysicalSignal_hpp
#define PhysicalSignal_hpp

#include "Ionosphere.hpp"
#include "SignalBase.hpp"
#include "Troposphere.hpp"


class PropSetup;

#define  SELECT_CENTRAL_BODY     1
#define  SELECT_PRIMARY_BODY     2
#define  SELECT_POINT_MASSES     4
#define  SELECT_ALL_BODIES       7

/**
 * Signal class used for instantaneous measurements
 *
 * This class might be going away based on the evolving measurement design
 */
class ESTIMATION_API PhysicalSignal: public SignalBase
{
public:
   PhysicalSignal(const std::string &typeStr, const std::string &name = "");
   virtual ~PhysicalSignal();
   PhysicalSignal(const PhysicalSignal& gs);
   PhysicalSignal& operator=(const PhysicalSignal& gs);

   virtual GmatBase* Clone() const;

   virtual void InitializeSignal(bool chainForwards = false);

   virtual bool ModelSignal(const GmatTime atEpoch, bool forSimulation, bool EpochAtReceive = true);
   
   virtual const std::vector<RealArray>&
                ModelSignalDerivative(GmatBase *obj, 
                                      Integer forId, 
                                      GmatTime *measTime, 
                                      bool forRangeRate = false);

   /// This function is used to add media correction to measurement model
   virtual void AddCorrection(const std::string& modelName,
                             const std::string& mediaCorrectionType);

   /// This function is used to get frequency at a given time from ramped frequency table
   Real           GetFrequencyFromRampTable(Real t, std::vector<RampTableData>* rampTB);
   /// This function is used to get frequency band at a given time from ramped frequency table
   Integer        GetFrequencyBandFromRampTable(Real t, std::vector<RampTableData>* rampTB);
   /// This function is used to specify frequency band based range of each band
   Integer        FrequencyBand(Real frequency);

   Real GetTDRSTransmitFrequency(std::vector<RampTableData>* rampTB);


protected:
   /// Flag indicating the initialization state of the new signal elements
   bool physicalSignalInitialized;

   /// Troposphere model object
   Troposphere* troposphere;

   /// Ionosphere model object
   Ionosphere* ionosphere;

   /// Flag indicating to use relativity correction
   bool useRelativity;
   /// Correction (unit: km)
   Real relCorrection;
   Real ettaiCorrection;
   Real mediaCorrection;

   /// Flag indicating to use Et-TAI correction
   bool useETTAI;

   bool GenerateLightTimeData(const GmatTime atEpoch, bool epochAtReceive);

   /// This function is used to compute relativity correction
   Real           RelativityCorrection(Rvector3 r1B, Rvector3 r2B, Real t1, Real t2);

   /// This function is used to compute Et-TAI correction
   Real           ETminusTAI(Real tA1MJD, SpacePoint* participant);

   /// This fucntion is used to compute signal frequency on each signal leg
   virtual bool   SignalFrequencyCalculation(std::vector<RampTableData>* rampTB, Real uplinkFrequency = 0.0, GmatTime measTime = 0.0);

   /// These functions are used to compute midia correction
   virtual bool   MediaCorrectionCalculation(std::vector<RampTableData>* rampTB = NULL);
//   virtual bool   MediaCorrectionCalculation1(std::vector<RampTableData>* rampTB = NULL);
   RealArray      TroposphereCorrection(Real freq, Real distance, Real elevationAngle, Real epoch);
   RealArray      IonosphereCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch1, Real epoch2);
   RealArray      MediaCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch1, Real epoch2, Real minElevationAngle);

   /// This function is used to calculate total hardware delay
   virtual bool   HardwareDelayCalculation();

private:
   /// ramp table and the beginning index and the ending index
   std::vector<RampTableData>* rampTable;
   UnsignedInt beginIndex;
   UnsignedInt endIndex;

   void           SpecifyBeginEndIndexesOfRampTable();
   bool           TestSignalBlockedBetweenTwoParticipants(Integer selection = SELECT_ALL_BODIES);
   bool           TestSignalBlockedByBody(CelestialBody* body, Rvector3 tRSSB, Rvector3 rRSSB, GmatTime tTime, GmatTime rTime);
   bool           TestHeightOfRayPath(CelestialBody* body, Rvector3 r1SSB,
                        Rvector3 r2SSB, GmatTime t1, GmatTime t2);

   //flags to correctly set data feasibility reasons.
   bool           blockedByOccultation;
   bool           blockedByHORP;
};

#endif /* PhysicalSignal_hpp */
