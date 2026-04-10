//$Id: Ionosphere.hpp 65 2010-06-21 00:10:28Z  $
//------------------------------------------------------------------------------
//                         Ionosphere Model
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
// Author: Tuan Dang Nguyen
// Created: 2010/06/21
//
/**
 * IRI 2007 ionosphere media correction model.
 */
//------------------------------------------------------------------------------
#ifndef Ionosphere_hpp
#define Ionosphere_hpp

#include "MediaCorrection.hpp"
#include "gmatdefs.hpp"
#include "Rvector3.hpp"

#include "APData.hpp"
#include "MagneticHistory.hpp"
#include "IonosphereCoefficientsFull.hpp"

class Ionosphere: public MediaCorrection
{
public:
   Ionosphere(const std::string &nomme);
   virtual ~Ionosphere();
   Ionosphere(const Ionosphere& ions);
   Ionosphere& operator=(const Ionosphere& ions);
   virtual GmatBase*    Clone() const;

   virtual bool Initialize();

   bool SetWaveLength(Real lambda);
   bool SetTime(GmatEpoch ep);
   bool SetStationPosition(Rvector3 p);
   bool SetSpacecraftPosition(Rvector3 p);
   bool SetEarthRadius(Real r);

   Real TEC(Rvector3 end, Rvector3 start);
   Real BendingAngle(Rvector3 end, Rvector3 start);            // specify the change of elevation angle
   virtual RealArray Correction();
   RealArray CalculateIRI2007();
   RealArray CalculateTRK223();
   Real TRK223Solver(const StringArray &TRK223Line, Real epochTime);
   Real GetTRK223Time(const std::string &TRK223TimeLine);

private:
   // These methods are inherited from IRI20027
   static Real storm(Integer doy, Real rgma, const Real rap);
   static Real storm_poly(const Integer code, const Real rap);
   static Real hmf2ed(const Real xmagbr, const Real rssn, const Real fof2, const Real foe, const Real xm3000);
   static Real foeedi(const Real cov, const Real xhi, const Real cos_xhinon, const Real lat, const Real cos_lat);
   static Real fof1ed(const Real magbr, const Real rssn, const Real xhi, const Real cos_xhi);
   static Real f1_c1(const Real modip, const Real l_hour, const Real sux200, const Real sax200);
   static Real xmded(const Real cos_xhi, const Real rssn);
   static Real f1_prob(const Real cos_xhi, const Real m_lat, const Real rssn);
   static bool tal(const Real hdeep, Real& depth, const Real width, const Real dlndh, Real& spt_0, Real& spt_1, Real& spt_2, Real& spt_3);
   static Real b0_98(const Real hpol_factor_200, const RealArray& bfr_n_day, const RealArray& bfr_s_day, const RealArray& bfr_n_night, const RealArray& bfr_s_night, const Real lon, const Real modip);
   static bool regfa1_xe2(const Real hef, const Real hmf2, const Real xe2h, const Real nmf2, const Real nmf1, const Real b0, const Real b1, Real& hmf1);
   static bool regfa1_xe3(const Real hf1, const Real  hef, const Real xf1, const Real xf2, const Real nme, const Real hmf2, const Real nmf2, const Real b0, const Real b1, const Real f1reg, const Real hmf1, const Real c1, Real& hst);

   // These methods are inherited from IRI20027
   static Real xe1(const Real height, const Real nmf2, const Real hmf2, const Real b2top);
   static Real xe2(const Real height, const Real hmf2, const Real nmf2, const Real b0, const Real b1);
   static Real xe3(const Real height, const Real hmf2, const Real nmf2, const Real b0, const Real b1, const bool f1reg, const Real hmf1, const Real c1);
   static Real xe4(const Real height, const Real hmf2, const Real nmf2, const Real b0, const Real b1, bool f1reg, const Real hmf1, const Real c1, const Real nme, const Real t, const Real hst, const Real hef, const Real hz);
   static Real xe5(const Real height, const bool is_night_110, const Real nme, const Real e_0, const Real e_1, const Real e_2, const Real e_3);
   static Real xe6(const Real height, const Real hdx, const Real nme, const Real d1, const Real xkk, const Real hmd, const Real fp3u, const Real fp30, const Real nmd, const Real fp1, const Real fp2);

   Real waveLength;          // wave length of the signal
   GmatEpoch epoch;          // time
   Rvector3 stationLoc;      // station location
   Rvector3 spacecraftLoc;   // spacecraft location

   Real earthRadius;
   
   RealArray electronDensities;

   APData* ap_data;
   MagneticHistory* magnetic_history;
   IonosphereCoefficientsFull* ionosphere_coefficients_full;

   static const Real NUM_OF_INTERVALS;
   static const Real IONOSPHERE_MAX_ALTITUDE;
   static const Real ANNUAL_FREQ;
   static const Real RADIUS, EC_SQ;

   static const std::vector<IntegerArray> STORM_CODES;
   static const std::vector<RealArray> B0F_DAY_LO, B0F_DAY_HI, B0F_NIGHT_LO, B0F_NIGHT_HI;
};


class IonosphereCorrectionModel
{
public:
   static IonosphereCorrectionModel* Instance(); 
   Ionosphere* GetIonosphereInstance();

private:
   IonosphereCorrectionModel();
   virtual ~IonosphereCorrectionModel();

   static IonosphereCorrectionModel* instance;
   Ionosphere* ionosphereObj;
};


#endif //Ionosphere_hpp_

