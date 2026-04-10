//$Id: Ionosphere.cpp 1398 2011-04-21 20:39:37Z  $
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

#include "Ionosphere.hpp"

#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include "StringUtil.hpp"
#include "MagneticField.hpp"

//#define DEBUG_IONOSPHERE_ELECT_DENSITY
//#define DEBUG_IONOSPHERE_TEC
//#define DEBUG_IONOSPHERE_CORRECTION
//#define DEBUG_IONOSPHERE_CONSTRUCTION
//#define DEBUG_IONOSPHERE_INITIALIZE
//#define DEBUG_TRK223
//#define DEBUG_TRK223_FILELOADER
//#define DEBUG_TRK223_SOLVER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
IonosphereCorrectionModel* IonosphereCorrectionModel::instance = NULL;

const Real Ionosphere::NUM_OF_INTERVALS = 200;
const Real Ionosphere::IONOSPHERE_MAX_ALTITUDE = 2000.0;

IonosphereCorrectionModel* IonosphereCorrectionModel::Instance() 
{
   if (instance == NULL)
      instance = new IonosphereCorrectionModel;
   return instance;
}


Ionosphere* IonosphereCorrectionModel::GetIonosphereInstance()
{
   if (ionosphereObj == NULL)
      ionosphereObj = new Ionosphere("IRI2007");

   return ionosphereObj;
}


IonosphereCorrectionModel::IonosphereCorrectionModel()
{
   ionosphereObj = NULL;
}


IonosphereCorrectionModel::~IonosphereCorrectionModel() 
{
   if (ionosphereObj)
   {
      delete ionosphereObj;
      ionosphereObj = NULL;
   }

   if (instance)
   {
      delete instance;
      instance = NULL;
   }
}


//------------------------------------------------------------------------------
// Ionosphere(const std::string& nomme)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
//------------------------------------------------------------------------------
Ionosphere::Ionosphere(const std::string &nomme):
   MediaCorrection("Ionosphere", nomme),
   ap_data(nullptr),
   magnetic_history(nullptr),
   ionosphere_coefficients_full(nullptr)
{
#ifdef DEBUG_IONOSPHERE_CONSTRUCTION
   MessageInterface::ShowMessage("Ionosphere default construction\n");
#endif

   objectTypeNames.push_back("Ionosphere");
   model = 2;                 // 2 for ionosphere model
   
   waveLength = 0.0;          // wave length of the signal
   epoch = 0.0;               // time
}


//------------------------------------------------------------------------------
// ~Ionosphere()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Ionosphere::~Ionosphere()
{
    if (ap_data)
        delete ap_data;

    if (magnetic_history)
        delete magnetic_history;

    if (ionosphere_coefficients_full)
        delete ionosphere_coefficients_full;
}


//------------------------------------------------------------------------------
// Ionosphere(const Ionosphere& ions)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
Ionosphere::Ionosphere(const Ionosphere& ions) :
    MediaCorrection(ions),
    waveLength(ions.waveLength),
    epoch(ions.epoch),
    ap_data(ions.ap_data),
    magnetic_history(ions.magnetic_history),
    ionosphere_coefficients_full(ions.ionosphere_coefficients_full)
{
#ifdef DEBUG_IONOSPHERE_CONSTRUCTION
    MessageInterface::ShowMessage("Ionosphere copy construction\n");
#endif

    stationLoc = ions.stationLoc;
    spacecraftLoc = ions.spacecraftLoc;
}


//-----------------------------------------------------------------------------
// Ionosphere& operator=(const Ionosphere& ions)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param tps The Ionosphere that is provides parameters for this one
 *
 * @return This Ionosphere, configured to match tps
 */
//-----------------------------------------------------------------------------
Ionosphere& Ionosphere::operator=(const Ionosphere& ions)
{
    if (this != &ions)
    {
        MediaCorrection::operator=(ions);

        waveLength = ions.waveLength;
        epoch = ions.epoch;
        stationLoc = ions.stationLoc;
        spacecraftLoc = ions.spacecraftLoc;

        ap_data = new APData(*ions.ap_data);
        magnetic_history = new MagneticHistory(*ions.magnetic_history);
        ionosphere_coefficients_full = new IonosphereCoefficientsFull(*ions.ionosphere_coefficients_full);
    }
    return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone()
//------------------------------------------------------------------------------
/**
 * Clone a Ionosphere object
 */
//------------------------------------------------------------------------------
GmatBase* Ionosphere::Clone() const
{
    return new Ionosphere(*this);
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
bool Ionosphere::Initialize()
{
   if (IsInitialized())
      return true;

#ifdef DEBUG_IONOSPHERE_INITIALIZE
   MessageInterface::ShowMessage("Ionosphere::Initialize()\n");
#endif

   if (MediaCorrectionInterface::Initialize())
   {
      ap_data = new APData(dataPath);
      magnetic_history = new MagneticHistory(dataPath);
      ionosphere_coefficients_full = new IonosphereCoefficientsFull(dataPath);

      isInitialized = true;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool SetWaveLength(Real lambda)
//------------------------------------------------------------------------------
/**
 * Set wave length
 * @param lambda  The wave length
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetWaveLength(Real lambda)
{
    waveLength = lambda;
    return true;
}


//------------------------------------------------------------------------------
// bool SetTime(GmatEpoch ep)
//------------------------------------------------------------------------------
/**
 * Set time
 * @param ep  The time
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetTime(GmatEpoch ep)
{
    epoch = ep;
   
    return true;
}


//------------------------------------------------------------------------------
// bool SetStationPosition(Rvector3 p)
//------------------------------------------------------------------------------
/**
 * Set station position
 * @param p  Position of station. (unit: km)
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetStationPosition(Rvector3 p)
{
    stationLoc = p;
    return true;
}


//------------------------------------------------------------------------------
// bool SetSpacecraftPosition(Rvector3 p)
//------------------------------------------------------------------------------
/**
 * Set spacecraft position
 * @param p  Position of spacecraft. (unit: km)
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetSpacecraftPosition(Rvector3 p)
{
    spacecraftLoc = p;
    return true;
}


//------------------------------------------------------------------------------
// bool SetEarthRadius(Real r)
//------------------------------------------------------------------------------
/**
 * Set earth radius
 * @param r  radius of earth. (unit: km)
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetEarthRadius(Real r)
{
    earthRadius = r;
    return true;
}

const Real Ionosphere::RADIUS = 6378.1363; // Equatorial radius
const Real Ionosphere::EC_SQ = 6.69415940271e-3; // Earth's eccentricity

const Real Ionosphere::ANNUAL_FREQ = 0.017203534; // ~2 * pi / 365.25
const std::vector<IntegerArray> Ionosphere::STORM_CODES = { {2, 2, 7, 12, 17, 17},
                                                            {3, 1, 6, 11, 16, 16},
                                                            {4, 0, 5, 10, 15, 15},
                                                            {3, 1, 6, 11, 16, 16},
                                                            {2, 2, 7, 12, 17, 17},
                                                            {1, 3, 8, 13, 18, 18},
                                                            {0, 4, 9, 14, 19, 19},
                                                            {1, 3, 8, 13, 18, 18} };

const std::vector<RealArray> Ionosphere::B0F_DAY_LO = { {201.0, 108.0,  78.0},
                                                        {210.0, 142.0,  94.0},
                                                        {192.0, 110.0,  81.0},
                                                        {199.0,  77.0,  65.0} };

const std::vector<RealArray> Ionosphere::B0F_DAY_HI = { {240.0, 124.0, 102.0},
                                                        {245.0, 164.0, 127.0},
                                                        {233.0, 120.0, 109.0},
                                                        {230.0,  96.0,  81.0} };

const std::vector<RealArray> Ionosphere::B0F_NIGHT_LO = { { 68.0,  65.0,  81.0},
                                                          { 61.0,  81.0,  84.0},
                                                          { 68.0,  68.0,  81.0},
                                                          { 67.0,  75.0,  70.0} };

const std::vector<RealArray> Ionosphere::B0F_NIGHT_HI = { { 80.0,  98.0,  87.0},
                                                          { 83.0, 100.0,  91.0},
                                                          { 71.0,  94.0,  88.0},
                                                          { 65.0, 112.0,  78.0} };

                                                          
//------------------------------------------------------------------------------
// Real storm(Integer doy, Real rgma, const Real rap)
//------------------------------------------------------------------------------
/**
 * Calculate storm value. This code is inherited from IRI2007
 * @param day day of year
 * @param rgma geomagnetic latitude (degrees)
 * @param rap
 * @return interpolated storm value
 */
//------------------------------------------------------------------------------
Real Ionosphere::storm(Integer doy, Real rgma, const Real rap)
{
    if (rgma < 0.0)
        doy = (doy + 172) % 365;

    Real rs = (Real)((doy >= 82) ? (doy - 82) : (doy + 283)) / 45.6;
    Real s1, facs = modf(rs, &s1);
    Integer s2 = ((Integer)s1 + 1) % 8;

    rgma = abs(rgma);
    Real rl = (rgma + 10.0) / 20.0;
    if (rl == 5.0)
        rl = 4.9;

    Real l1, facl = modf(rl, &l1);
    Integer l2 = (Integer)l1 + 1;

    if (rap < 300.0)
    {
        Real rapf = 300.0;

        Real cf1 = storm_poly(STORM_CODES[(Integer)s1][(Integer)l1], rapf);
        Real cf2 = storm_poly(STORM_CODES[(Integer)s1][l2], rapf);
        Real cf3 = storm_poly(STORM_CODES[s2][(Integer)l1], rapf);
        Real cf4 = storm_poly(STORM_CODES[s2][l2], rapf);

        Real cf300 = cf1 * (1 - facs) * (1 - facl) + cf2 * (1 - facs) * facl + cf3 * facs * (1 - facl) + cf4 * facs * facl;
        return (cf300 - 1.0) * rap / 100.0 - cf300 * 2.0 + 3.0;
    }
    else
    {
        Real cf1 = storm_poly(STORM_CODES[(Integer)s1][(Integer)l1], rap);
        Real cf2 = storm_poly(STORM_CODES[(Integer)s1][l2], rap);
        Real cf3 = storm_poly(STORM_CODES[s2][(Integer)l1], rap);
        Real cf4 = storm_poly(STORM_CODES[s2][l2], rap);

        return cf1 * (1 - facs) * (1 - facl) + cf2 * (1 - facs) * facl + cf3 * facs * (1 - facl) + cf4 * facs * facl;
    }
}

//------------------------------------------------------------------------------
// Real storm_poly(const Integer code, const Real rap)
//------------------------------------------------------------------------------
/**
 * Calculate storm polynomial. This code is inherited from IRI2007
 * @param code determinate which poly to use
 * @param rap argument
 * @return calculated value
 */
//------------------------------------------------------------------------------
Real Ionosphere::storm_poly(const Integer code, const Real rap)
{
    switch (code)
    {
    case 0:
        return 1.0136e0 + (-9.1700e-5 + 1.1600e-8 * rap) * rap;
    case 1:
        return 1.0478e0 - 1.3700e-5 * rap;
    case 2:
        return 1.0;
    case 3:
        return 1.0258e0 + (7.1400e-5 - 1.4600e-8 * rap) * rap;
    case 4:
        return 1.0;
    case 5:
        return 1.0770e0 + (-3.2100e-4 + (9.8600e-8 - 9.4400e-12 * rap) * rap) * rap;
    case 6:
        return 1.0543e0 + (-1.6600e-4 + 2.2500e-8 * rap) * rap;
    case 7:
        return 1.0103e0 + (-4.1000e-5 + (-1.6700e-8 + 3.0400e-12 * rap) * rap) * rap;
    case 8:
        return 9.9927e-1 + (1.3600e-4 - 1.6200e-8 * rap) * rap;
    case 9:
        return 9.6876e-1 + (2.2900e-4 + (-9.4200e-8 + 9.3200e-12 * rap) * rap) * rap;
    case 10:
        return 1.0971e0 + (-3.8900e-4 + (1.1700e-7 - 1.0700e-11 * rap) * rap) * rap;
    case 11:
        return 1.0971e0 + (-3.0800e-4 + 4.3200e-8 * rap) * rap;
    case 12:
        return 1.0777e0 + (-2.8100e-4 + 3.9700e-8 * rap) * rap;
    case 13:
        return 1.1134e0 + (-1.9000e-4 + 3.1300e-8 * rap) * rap;
    case 14:
        return 1.0237e0 + (4.7600e-5 + (-8.0400e-8 + 1.0900e-11 * rap) * rap) * rap;
    case 15:
        return 1.0703e0 + (-2.8000e-4 + 3.9100e-8 * rap) * rap;
    case 16:
        return 1.0248e0 + (-2.0700e-4 + 2.5800e-8 * rap) * rap;
    case 17:
        return 1.0945e0 + (-2.9100e-4 + 3.4500e-8 * rap) * rap;
    case 18:
        return 1.1622e0 + (-3.3000e-4 + 4.7600e-8 * rap) * rap;
    case 19:
        return 1.1393e0 + (-4.0400e-4 + (1.1300e-7 - 1.0100e-11 * rap) * rap) * rap;
    default:
        return 0.0; // Will never happen. Just here to keep the compiler happy.
    }
}

//------------------------------------------------------------------------------
// Real hmf2ed(const Real magbr, const Real rssn, const Real fof2, const Real foe, const Real xm3000)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param magbr magnetic field angle
 * @param rssn
 * @param fof2
 * @param foe
 * @param xm3000
 * @return
 */
//------------------------------------------------------------------------------
Real Ionosphere::hmf2ed(const Real magbr, const Real rssn, const Real fof2, const Real foe, const Real xm3000)
{
    Real f1 = rssn * 0.00232 + 0.222;
    Real f2 = 1.2 - exp(rssn * 0.0239) * 0.0116;
    Real f3 = (rssn - 25.0) * 0.096 / 150.0;
    Real delm = f1 * (1.0 - rssn / 150.0 * exp(-magbr * magbr / 1600.0)) / (fof2 / foe - f2) + f3;
    return 1490.0 / (xm3000 + delm) - 176.0;
}

//------------------------------------------------------------------------------
// foeedi(const Real cov, const Real xhi, const Real cos_xhinon, const Real lat, const Real cos_lat)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param cov
 * @param xhi zenith angle (degrees)
 * @param cos_xhinon cos(noon zenith)
 * @param lat latitude (degrees)
 * @param cos_lat cos(latitude)
 * @return
 */
//------------------------------------------------------------------------------
Real Ionosphere::foeedi(const Real cov, const Real xhi, const Real cos_xhinon, const Real lat, const Real cos_lat)
{
    const Real xlati = abs(lat);

    const Real a = (cov - 66.0) * 0.0094 + 1.0;
    Real sm, c;
    if (xlati < 32.0)
    {
        sm = cos_lat * 1.92 - 1.93;
        c = cos_lat * 116.0 + 23.0;
    }
    else
    {
        sm = 0.11 - cos_lat * 0.49;
        c = cos_lat * 35.0 + 92.0;
    }

    const Real b = pow(cos_xhinon, sm);

    const Real sp = (xlati > 12.0) ? 1.2 : 1.31;

    const Real xhic = xhi - log(exp((xhi - 89.98) / 3.0) + 1.0) * 3.0;
    const Real d = pow(cos(GmatMathConstants::RAD_PER_DEG * xhic), sp);

    Real r4foe = a * b * c * d;

    Real smin = (cov - 60.0) * 0.0015 + 0.121;
    smin *= smin;
    if (r4foe < smin)
        r4foe = smin;

    return pow(r4foe, 0.25);
}

//------------------------------------------------------------------------------
// fof1ed(const Real magbr, const Real rssn, const Real xhi, const Real cos_xhi)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param magbr
 * @param rssn
 * @param xhi zenith (degrees)
 * @param cos_xhi cos(xhi)
 * @return
 */
//------------------------------------------------------------------------------
Real Ionosphere::fof1ed(const Real magbr, const Real rssn, const Real xhi, const Real cos_xhi)
{
    const Real ylati = abs(magbr);

    if (xhi > 90.0)
        return 0.0;

    const Real dla = ylati;
    const Real f0 = dla * (0.0058 - dla * 1.2e-4) + 4.35;
    const Real f100 = dla * (0.011 - dla * 2.3e-4) + 5.348;
    const Real fs = f0 + (f100 - f0) * rssn / 100.0;
    const Real xmue = dla * (0.0046 - dla * 5.4e-5) + 0.093 + rssn * 3e-4;

    Real fof1 = fs * pow(cos_xhi, xmue);
    const Real chi0 = dla * 0.349504 + 49.84733;
    const Real chi100 = dla * 0.509932 + 38.96113;
    const Real chim = chi0 + (chi100 - chi0) * rssn / 100.0;

    if (xhi > chim)
        fof1 = -fof1;

    return fof1;
}

//------------------------------------------------------------------------------
// f1_c1(const Real modip, const Real l_hour, const Real sux200, const Real sax200)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param modip modified dipole
 * @param l_hour local time
 * @param sux200 sunrise time at 200 km
 * @param sax200 sunset time at 200km
 * @return
 */
//------------------------------------------------------------------------------
Real Ionosphere::f1_c1(const Real modip, const Real l_hour, const Real sux200, const Real sax200)
{
    const Real absmdp = abs(modip);
    const Real dela = (absmdp >= 18.0) ? (exp(-(absmdp - 30.0) / 10.0) + 1.0) : 4.32;
    const Real c1old = 0.11 / dela + 0.09;
    const Real c1 = (sux200 == sax200) ? (c1old * 2.5) : (c1old * 2.5 * cos((l_hour - 12.0) / (sux200 - sax200) * GmatMathConstants::PI));

    if (c1 < 0.0)
        return 0.0;

    return c1;
}

//------------------------------------------------------------------------------
// xmded(const Real cos_xhi, const Real rssn)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param cos_xhi cos(zenith)
 * @param rssn
 * @return
 */
//------------------------------------------------------------------------------
Real Ionosphere::xmded(const Real cos_xhi, const Real rssn)
{
    if (cos_xhi < 0.0)
        return 4e8;

    const Real y = rssn * 8.8e6 + 6.05e8;
    const Real yyy = -0.1 / pow(cos_xhi, 2.7);
    const Real ymd = (yyy < -40.0) ? 0.0 : y * exp(yyy);

    if (ymd < 4e8)
        return 4e8;

    return ymd;
}

//------------------------------------------------------------------------------
// f1_prob(const Real cos_xhi, const Real m_lat, const Real rssn)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param cos_xhi cos(zenith)
 * @param m_lat magnetic latitude (degrees)
 * @param rssn
 * @return
 */
//------------------------------------------------------------------------------
Real Ionosphere::f1_prob(const Real cos_xhi, const Real m_lat, const Real rssn)
{
    const Real xarg = cos_xhi * 0.5 + 0.5;
    const Real a = rssn * 0.0854 + 2.98;
    const Real b = 0.0107 - rssn * 0.0022;
    const Real gamma = a + (b + (rssn * 1.47e-5 - 2.56e-4) * m_lat) * m_lat;

    const Real f1pr = pow(xarg, gamma);
    if (f1pr < 0.001)
        return 0.0;

    return f1pr;
}

//------------------------------------------------------------------------------
// tal(const Real hdeep, Real& depth, const Real width, const Real dlndh, Real& spt_0, Real& spt_1, Real& spt_2, Real& spt_3)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param hdeep
 * @param depth
 * @param width
 * @param dlndh
 * @param spt_0
 * @param spt_1
 * @param spt_2
 * @param spt_3
 * @return
 */
//------------------------------------------------------------------------------
bool Ionosphere::tal(const Real hdeep, Real& depth, const Real width, const Real dlndh, Real& spt_0, Real& spt_1, Real& spt_2, Real& spt_3)
{
    Real z1;
    if (depth < 0.0)
    {
        depth = -depth;
        z1 = log(1.0 - depth / 100.0) / (hdeep * hdeep);
    }
    else
        z1 = -depth / (hdeep * 100.0 * hdeep);

    Real z3 = dlndh / (width * 2.0);
    const Real z4 = hdeep - width;
    spt_3 = ((z1 * (width - hdeep * 2.0) * width + z3 * z4 * hdeep) * 2.0 / (hdeep * width * z4 * z4 * z4));
    spt_2 = (z1 * (width * 2.0 - hdeep * 3.0) / (hdeep * z4 * z4) - (hdeep * 2.0 + width) * spt_3);
    spt_1 = z1 * -2.0 / hdeep - hdeep * 2.0 * spt_2 - hdeep * 3.0 * hdeep * spt_3;
    spt_0 = z1 - hdeep * (spt_1 + hdeep * (spt_2 + hdeep * spt_3));

    bool aus6 = false;
    const Real b = spt_2 * 4.0 / (spt_3 * 5.0) + hdeep;
    Real z2 = b * b / 4.0 - (spt_0 * -2.0 / (spt_3 * 5 * hdeep));
    if (z2 < 0.0)
        return aus6;

    z3 = sqrt(z2);
    z1 = b / 2.0;
    z2 = -z1 + z3;
    if (z2 > 0.0 && z2 < width)
        aus6 = true;

    if (abs(z3) > 1e-15)
    {
        z2 = -z1 - z3;
        if (z2 > 0.0 && z2 < width)
            aus6 = true;

        return aus6;
    }

    z2 = (spt_0 * -2.0 / (spt_3 * 5 * hdeep)) / z2;
    if (z2 > 0.0 && z2 < width)
        aus6 = true;

    return aus6;
}

//------------------------------------------------------------------------------
// b0_98(const Real hpol_factor_200, const RealArray& bfr_n_day, const RealArray& bfr_s_day, const RealArray& bfr_n_night, const RealArray& bfr_s_night, const Real lon, const Real modip)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param hpol_factor_200 amount of dayight at 200km
 * @param bfr_n_day
 * @param bfr_s_day
 * @param bfr_n_night
 * @param bfr_s_night
 * @param lon longitude (degrees)
 * @param modip modified dipole
 * @return
 */
//------------------------------------------------------------------------------
Real Ionosphere::b0_98(const Real hpol_factor_200, const RealArray& bfr_n_day, const RealArray& bfr_s_day, const RealArray& bfr_n_night, const RealArray& bfr_s_night, const Real lon, const Real modip)
{
    RealArray bfd_n(3), bfd_s(3);

    for (Integer lat_idx = 0; lat_idx < 3; lat_idx++)
    {
        bfd_n[lat_idx] = bfr_n_night[lat_idx] + hpol_factor_200 * (bfr_n_day[lat_idx] - bfr_n_night[lat_idx]);
        bfd_s[lat_idx] = bfr_s_night[lat_idx] + hpol_factor_200 * (bfr_s_day[lat_idx] - bfr_s_night[lat_idx]);
    }

    const Real zx1 = bfd_s[2];
    const Real zx2 = bfd_s[1];
    const Real zx3 = (200 < lon && lon < 320) ? bfd_s[0] : bfd_n[0];
    const Real zx4 = bfd_n[1];
    const Real zx5 = bfd_n[2];

    Real ret_val = zx1;
    ret_val += 3.0 * ((zx2 - zx1) / 27.0) * (log(exp(modip / 3.0 + 15.0) + 1.0) - log(exp(-15.0) + 1.0));
    ret_val += 3.0 * ((zx3 - zx2) / 18.0 - (zx2 - zx1) / 27.0) * (log(exp(modip / 3.0 + 6.0) + 1.0) - log(exp(-24.0) + 1.0));
    ret_val += 3.0 * ((zx4 - zx3) / 18.0 - (zx3 - zx2) / 18.0) * (log(exp(modip / 3.0) + 1.0) - log(exp(-30.0) + 1.0));
    ret_val += 3.0 * ((zx5 - zx4) / 27.0 - (zx4 - zx3) / 18.0) * (log(exp(modip / 3.0 - 6.0) + 1.0) - log(exp(-36.0) + 1.0));
    ret_val += 3.0 * ((zx4 - zx5) / 27.0) * (log(exp(modip / 3.0 - 15.0) + 1.0) - log(exp(-45.0) + 1.0));
    return ret_val;
}

//------------------------------------------------------------------------------
// regfa1_xe2(const Real hef, const Real hmf2, const Real xe2h, const Real nmf2, const Real nmf1, const Real b0, const Real b1, Real& hmf1)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * Find root of function xe2 in hmf1
 * @param hef
 * @param hmf2
 * @param xe2h
 * @param nmf2
 * @param nmf1
 * @param b0
 * @param b1
 * @param hmf1
 * @return if failure
 */
//------------------------------------------------------------------------------
bool Ionosphere::regfa1_xe2(const Real hef, const Real hmf2, const Real xe2h, const Real nmf2, const Real nmf1, const Real b0, const Real b1, Real& hmf1)
{
    const Real x11 = hef;
    const Real x22 = hmf2;
    const Real fx11= xe2h;
    const Real fx22= nmf2;
    const Real fw = nmf1;

    Real f1 = fx11 - fw;
    Real f2 = fx22 - fw;
    if (f1 * f2 > 0)
    {
        hmf1 = 0.0;
        return true;
    }

    Real ep = 0.001;
    Real x1 = x11;
    Real x2 = x22;
    Real dx, x;

    bool k = false;
    Integer ng = 2;
    Integer lfd = 0;
    bool links = false, l1;

    while (true)
    {
        if (k)
        {
            l1 = links;
            dx = (x2 - x1) / ng;
            if (!links)
                dx *= ng - 1;

            x = x1 + dx;
        }
        else
            x = (x1 * f2 - x2 * f1) / (f2 - f1);

        Real fx = xe2(x, hmf2, nmf2, b0, b1) - fw;
        lfd += 1;
        if (lfd > 20)
        {
            ep *= 10.0;
            lfd = 0;
        }

        links = f1 * fx > 0.0;
        k = !k;
        if (links)
        {
            x1 = x;
            f1 = fx;
        }
        else
        {
            x2 = x;
            f2 = fx;
        }

        if (abs(x2 - x1) <= ep)
        {
            hmf1 = x;
            return false;
        }

        if (!k && (links && !l1 || !links && l1))
            ng <<= 1;
    }
}

//------------------------------------------------------------------------------
// regfa1_xe3(const Real hf1, const Real hef, const Real xf1, const Real xf2, const Real nme, const Real hmf2, const Real nmf2, const Real b0, const Real b1, const Real f1reg, const Real hmf1, const Real c1, Real& hst)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * Find root of function xe2 in hst
 * @param hf1
 * @param hef
 * @param xf1
 * @param xf2
 * @param nme
 * @param hmf2
 * @param nmf2
 * @param b0
 * @param b1
 * @param f1reg
 * @param hmf1
 * @param c1
 * @param hst
 * @return if failure
 */
//------------------------------------------------------------------------------
bool Ionosphere::regfa1_xe3(const Real hf1, const Real hef, const Real xf1, const Real xf2, const Real nme, const Real hmf2, const Real nmf2, const Real b0, const Real b1, const Real f1reg, const Real hmf1, const Real c1, Real& hst)
{
    const Real x11 = hf1;
    const Real x22 = hef;
    const Real fx11 = xf1;
    const Real fx22 = xf2;
    const Real fw = nme;

    Real f1 = fx11 - fw;
    Real f2 = fx22 - fw;
    if (f1 * f2 > 0)
    {
        hst = 0.0;
        return true;
    }

    Real ep = 0.001;
    Real x1 = x11;
    Real x2 = x22;
    Real dx, x;

    bool k = false;
    Integer ng = 2;
    Integer lfd = 0;
    bool links = false, l1;

    while (true)
    {
        if (k)
        {
            l1 = links;
            dx = (x2 - x1) / ng;
            if (!links)
                dx *= ng - 1;

            x = x1 + dx;
        }
        else
            x = (x1 * f2 - x2 * f1) / (f2 - f1);

        Real fx = xe3(x, hmf2, nmf2, b0, b1, f1reg, hmf1, c1) - fw;
        lfd += 1;
        if (lfd > 20)
        {
            ep *= 10.0;
            lfd = 0;
        }

        links = f1 * fx > 0.0;
        k = !k;
        if (links)
        {
            x1 = x;
            f1 = fx;
        }
        else
        {
            x2 = x;
            f2 = fx;
        }

        if (abs(x2 - x1) <= ep)
        {
            hst = x;
            return false;
        }

        if (!k && (links && !l1 || !links && l1))
            ng <<= 1;
    }
}

//------------------------------------------------------------------------------
// xe1(const Real height, const Real nmf2, const Real hmf2, const Real b2top)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param height height above elipsoid in km
 * @param 
 * @param 
 * @param 
 * @param 
 * @return electron density (e / m^3)
 */
//------------------------------------------------------------------------------
Real Ionosphere::xe1(const Real height, const Real nmf2, const Real hmf2, const Real b2top)
{
    const Real dh = height - hmf2;
    const Real g1 = dh * 0.125;
    const Real z = dh / (b2top * (g1 * 100.0 / (b2top * 100.0 + g1) + 1.0));

    if (z > 40.0)
        return 0.0;

    const Real ee = exp(z);
    Real ep;
    if (ee > 1e7)
        ep = 4.0 / ee;
    else
    {
        const Real r1 = ee + 1.0;
        ep = ee * 4.0 / (r1 * r1);
    }

    return nmf2 * ep;
}

//------------------------------------------------------------------------------
// xe2(const Real height, const Real hmf2, const Real nmf2, const Real b0, const Real b1)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param height height above elipsoid in km
 * @param hmf2
 * @param nmf2
 * @param b0
 * @param b1
 * @return electron density (e / m^3)
 */
//------------------------------------------------------------------------------
Real Ionosphere::xe2(const Real height, const Real hmf2, const Real nmf2, const Real b0, const Real b1)
{
    Real x = (hmf2 - height) / b0;
    if (x <= 0.0)
        x = 0.0;

    Real z = pow(x, b1);
    if (z > 88.0)
        z = 88.0;

    return nmf2 * exp(-z) / cosh(x);
}

//------------------------------------------------------------------------------
// xe3(const Real height, const Real hmf2, const Real nmf2, const Real b0, const Real b1, const bool f1reg, const Real hmf1, const Real c1)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param height height above elipsoid in km
 * @param hmf2
 * @param nmf2
 * @param b0
 * @param b1
 * @param f1reg
 * @param hmf1
 * @param c1
 * @return electron density (e / m^3)
 */
//------------------------------------------------------------------------------
Real Ionosphere::xe3(const Real height, const Real hmf2, const Real nmf2, const Real b0, const Real b1, const bool f1reg, const Real hmf1, const Real c1)
{
    const Real h1bar = f1reg ? (hmf1 * (1.0 - pow((hmf1 - height) / hmf1, c1 + 1.0))) : height;
    return xe2(h1bar, hmf2, nmf2, b0, b1);
}

//------------------------------------------------------------------------------
// xe4(const Real height, const Real hmf2, const Real nmf2, const Real b0, const Real b1, const bool f1reg, const Real hmf1, const Real c1, const Real nme, const Real t, const Real hst, const Real hef, const Real hz)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param height height above elipsoid in km
 * @param hmf2
 * @param nmf2
 * @param b0
 * @param b1
 * @param f1reg
 * @param hmf1
 * @param c1
 * @param nme
 * @param t
 * @param hst
 * @param hef
 * @param hz
 * @return electron density (e / m^3)
 */
//------------------------------------------------------------------------------
Real Ionosphere::xe4(const Real height, const Real hmf2, const Real nmf2, const Real b0, const Real b1, const bool f1reg, const Real hmf1, const Real c1, const Real nme, const Real t, const Real hst, const Real hef, const Real hz)
{
    if (hst < 0.0)
        return nme + t * (height - hef);

    Real h1bar;
    if (hst == hef)
        h1bar = height;
    else if (t < 0)
        h1bar = hz + t * 0.5 + sqrt(t * (t * 0.25 + hz - height));
    else
        h1bar = hz + t * 0.5 - sqrt(t * (t * 0.25 + hz - height));

    return xe3(h1bar, hmf2, nmf2, b0, b1, f1reg, hmf1, c1);
}

//------------------------------------------------------------------------------
// xe5(const Real height, const bool is_night_110, const Real nme, const Real e_0, const Real e_1, const Real e_2, const Real e_3)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param height height above elipsoid in km
 * @param is_night_110 is it night at 110km above the ellipoid?
 * @param nme
 * @param e_0
 * @param e_1
 * @param e_2
 * @param e_3
 * @return electron density (e / m^3)
 */
//------------------------------------------------------------------------------
Real Ionosphere::xe5(const Real height, const bool is_night_110, const Real nme, const Real e_0, const Real e_1, const Real e_2, const Real e_3)
{
    const Real t3 = height - 110.0;
    const Real t1 = t3 * t3 * (e_0 + t3 * (e_1 + t3 * (e_2 + t3 * e_3)));

    if (is_night_110)
        return nme * exp(t1);
    else
        return nme * (t1 + 1);
}

//------------------------------------------------------------------------------
// xe6(const Real height, const Real hdx, const Real nme, const Real d1, const Real xkk, const Real hmd, const Real fp3u, const Real fp30, const Real nmd, const Real fp1, const Real fp2)
//------------------------------------------------------------------------------
/**
 * This code is inherited from IRI2007
 * @param height height above elipsoid in km
 * @param hdx
 * @param nme
 * @param d1
 * @param xkk
 * @param hmd
 * @param fp3u
 * @param fp30
 * @param nmd
 * @param fp1
 * @param fp2
 * @return electron density (e / m^3)
 */
//------------------------------------------------------------------------------
Real Ionosphere::xe6(const Real height, const Real hdx, const Real nme, const Real d1, const Real xkk, const Real hmd, const Real fp3u, const Real fp30, const Real nmd, const Real fp1, const Real fp2)
{
    if (height > hdx)
        return nme * exp(-d1 * pow(110.0 - height, xkk));

    const Real z = height - hmd;
    const Real fp3 = (z > 0.0) ? fp30 : fp3u;

    return nmd * exp(z * (fp1 + z * (fp2 + z * fp3)));
}

//---------------------------------------------------------------------------
// Real Ionosphere::TEC()
// This function is used to calculate number of electron inside a 1 meter 
// square cross sectioncylinder with its bases on spacecraft and on ground 
// station.
//
//  return value: tec  (unit: number of electrons per 1 meter square)
//---------------------------------------------------------------------------
Real Ionosphere::TEC(Rvector3 end, Rvector3 start)
{
#ifdef DEBUG_IONOSPHERE_TEC
   MessageInterface::ShowMessage("         It performs calculation electron density along the path\n");
   MessageInterface::ShowMessage("            from ground station location: (%lf,  %lf,  %lf)km\n", stationLoc[0], stationLoc[1], stationLoc[2]);
   MessageInterface::ShowMessage("            to spacecraft location:       (%lf,  %lf,  %lf)km\n", spacecraftLoc[0], spacecraftLoc[1], spacecraftLoc[2]);
   MessageInterface::ShowMessage("         Earth radius : %lf\n", earthRadius);
#endif
//   Rvector3 sR;
//   if (spacecraftLoc.GetMagnitude() - earthRadius > IONOSPHERE_MAX_ATTITUDE)
//      sR = spacecraftLoc.GetUnitVector() * (IONOSPHERE_MAX_ATTITUDE + earthRadius); 
//   else
//      sR = spacecraftLoc;

//   //Rvector3 dR = (spacecraftLoc - stationLoc) / NUM_OF_INTERVALS;
//   Rvector3 dR = (sR - stationLoc) / NUM_OF_INTERVALS;
//   Rvector3 p1 = stationLoc;

   // Fix bug to calculate end point
   // Solution to where a line intersects a sphere is a quadratic equation

      // Evenly spaced integration points
   Rvector3 dR = (end - start) / NUM_OF_INTERVALS;
   Rvector3 p1 = start;
   Rvector3 p2;
   Real electdensity, ds;
   Real tec = 0.0;

   p2 = p1 + dR;
   electdensity = electronDensities[0];                // unit: electron / m^3
   ds = (p2 - p1).GetMagnitude()*GmatMathConstants::KM_TO_M;   // unit: m
   tec += electdensity * ds; 
   p1 = p2;

   for(int i = 1; i < NUM_OF_INTERVALS; ++i)
   {
      p2 = p1 + dR;
      electdensity = (electronDensities[i] + electronDensities[i-1]) / 2.0;                // unit: electron / m^3
      ds = (p2-p1).GetMagnitude()*GmatMathConstants::KM_TO_M;   // unit: m
      tec += electdensity*ds;                                   // unit: electron / m^2
      p1 = p2;
   }

   //// Gaussian Quadrature:
   //Rvector3 dR = (end - start);
   //Rvector3 p1 = start;
   //Rvector3 p2;
   //Real electdensity, ds;
   //Real tec = 0.0;
   //for(int i = 0; i < NUM_OF_INTERVALS; ++i)
   //{
   //   p2 = start + dR*Ionosphere::QUAD_POINTS[i];
   //   electdensity = ElectronDensity(p2)*Ionosphere::QUAD_WEIGHTS[i];                   // unit: electron / m^3
   //   tec += electdensity*dR.GetMagnitude()*GmatMathConstants::KM_TO_M;                                   // unit: electron / m^2
   //}
   
   return tec;
}


//---------------------------------------------------------------------------
// Real Ionosphere::BendingAngle()
//---------------------------------------------------------------------------
Real Ionosphere::BendingAngle(Rvector3 end, Rvector3 start)
{

   // 2. Calculate angle correction
   Rvector3 rangeVec = end - start;
   Rvector3 dR = rangeVec / NUM_OF_INTERVALS;
   Rvector3 r_i1 = end;
   Rvector3 r_i;
   Real n_i, n_i1, density_i, density_i1;
   
   // Frequency of signal
   Real freq = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / waveLength;
   const Real index_of_refrac_coeff = 40.3 / (freq * freq);

   // Angle of incidence at position r_i1
   Real theta_i1 = GmatMathUtil::ACos(rangeVec.GetUnitVector()*r_i1.GetUnitVector());             // unit: radian
   
   // Elevetion angle at position r_i1
   Real beta_i1 = GmatMathConstants::PI_OVER_TWO - theta_i1;                                      // unit: radian
   //MessageInterface::ShowMessage("Elevation angle = %.12lf degree\n", beta_i1*GmatMathConstants::DEG_PER_RAD);

   // Electron density at position r_i1
   density_i1 = electronDensities[NUM_OF_INTERVALS-1];

   // Index of refaction at position ri1
   n_i1 = 1 - index_of_refrac_coeff * density_i1;

   // Refaction correction 
   Real dtheta_i1 = 0.0;
   for (int i = NUM_OF_INTERVALS-1; i >= 0; --i)
   {
      // the previous position of r_i
      r_i = r_i1 - dR;
      
      // density at position r_i 
      density_i = electronDensities[i];
      
      // index of refaction at position r_i 
      n_i = 1 - index_of_refrac_coeff * density_i;

      Real dtheta = ((n_i1 - n_i)/ n_i) * GmatMathUtil::Tan(theta_i1);
      //MessageInterface::ShowMessage("dtheta = %.12lf rad\n", dtheta);
      dtheta_i1 += dtheta;

      // Reset position
      r_i1 = r_i;
      // Recalculate angle of incidence
      theta_i1 = GmatMathUtil::ACos(rangeVec.GetUnitVector()*r_i1.GetUnitVector()) - dtheta_i1;             // unit: radian
      // Reset desity, index of refaction for the new position
      density_i1 = density_i;
      n_i1 = n_i;
   }
   
   Real dbeta = -dtheta_i1;             // elevation angle's correction equals negative of incidence angle's correction 
   //MessageInterface::ShowMessage("Elevation angle correction = %.12lf x e-3 degree\n", dbeta*GmatMathConstants::DEG_PER_RAD*1000.0);
   return dbeta;
}


//---------------------------------------------------------------------------
// RealArray Ionosphere::Correction()
// This function is used to calculate Ionosphere correction
// Return values:
//    . Range correction (unit: m)
//    . Angle correction (unit: radian)
//    . Time correction  (unit: s)
//---------------------------------------------------------------------------
RealArray Ionosphere::Correction()
{
#ifdef DEBUG_IONOSPHERE_CORRECTION
   MessageInterface::ShowMessage("Ionosphere::Correction() start\n");
#endif
   
   RealArray ra;

   if (modelTypeName == "TRK-2-23")
   {
      ra = CalculateTRK223();
   }
   else if (modelTypeName == "IRI2007")
   {
      ra = CalculateIRI2007();
   }
   else
   {
      MessageInterface::ShowMessage("Ionosphere::Correction: Unrecognized Ionosphere model " + modelTypeName + " used\n"
         "Supported models are IRI2007 and TRK-2-23\n");
      throw MeasurementException("Ionosphere::Correction: Unrecognized Ionosphere model " + modelTypeName + " used\n"
         "Supported models are IRI2007 and TRK-2-23\n");
   }

#ifdef DEBUG_IONOSPHERE_CORRECTION
   MessageInterface::ShowMessage(" Ionosphere correction result:\n");
   MessageInterface::ShowMessage("   Range correction = %f m\n", ra[0]);
   MessageInterface::ShowMessage("   Elevation angle correction = %f rad", ra[1]);
   MessageInterface::ShowMessage("   Time correction = %f sec\n", ra[2]);
#endif

   
   return ra;
}

RealArray Ionosphere::CalculateIRI2007()
{
#ifdef DEBUG_IONOSPHERE_CORRECTION
    MessageInterface::ShowMessage("Ionosphere::CalculateIRI_Fast() start\n");
#endif

    static Integer igrz_WarningCount = 0;

   // Initialize before doing calculation
    if (!IsInitialized())
        Initialize();


   //------------------------------------------
   // Configure for the epoch

    GmatEpoch epoch_new;
    std::string time;
    TimeSystemConverter::Instance()->Convert("A1ModJulian", epoch, "", "UTCGregorian", epoch_new, time, 2);
    const Integer year = atoi(time.substr(0, 4).c_str());
    const Integer month = atoi(time.substr(5, 2).c_str());
    const Integer day = atoi(time.substr(8, 2).c_str());
    const Integer hour = atoi(time.substr(11, 2).c_str());
    const Real hours = atof(time.substr(11, 2).c_str()) + atof(time.substr(14, 2).c_str()) / 60 +
        atof(time.substr(17, 2).c_str()) / 3600 + atof(time.substr(20, 3).c_str()) / 3600000.0;

    Integer doy = day; // Day of year
    switch (month)
    {
    case 2:
        doy += 31;
        break;
    case 3:
        doy += 59;
        break;
    case 4:
        doy += 90;
        break;
    case 5:
        doy += 120;
        break;
    case 6:
        doy += 151;
        break;
    case 7:
        doy += 181;
        break;
    case 8:
        doy += 212;
        break;
    case 9:
        doy += 243;
        break;
    case 10:
        doy += 273;
        break;
    case 11:
        doy += 304;
        break;
    case 12:
        doy += 334;
        break;
    }

    if (month > 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
        doy += 1;

    MagneticField* magnetic_field = magnetic_history->make_magnetic_field(year, doy);
    if (magnetic_field == nullptr)
        throw MeasurementException("Error: Epoch is out of range.");

    IonosphereCoefficients* ionosphere_coefficients = ionosphere_coefficients_full->make_ionosphere_coefficients(year, month, day, hours);
    if (ionosphere_coefficients == nullptr && igrz_WarningCount == 0)
    {
        MessageInterface::ShowMessage(
            "Warning: The epoch (%.12lf A1MJD) is out of the time range of the ionosphere ig_rz.dat file "
            "(%s). Ionospheric corrections are set to zero.\n",
            epoch, ionosphere_coefficients_full->month_range().c_str());

        igrz_WarningCount = 1;
    }

    const Real rap = ap_data->get_rap(year, month, day, hour);
    if (std::isnan(rap))
    {
        delete magnetic_field;

        throw MeasurementException("Error: Epoch is out of range. Time range for Ionosphere calculation is from " + ap_data->start_date() + " to " + ap_data->end_date() + ".\n");
    }

    if (ionosphere_coefficients == nullptr)
    {
        delete magnetic_field;

        RealArray out = { 0.0, 0.0, 0.0 };
        return out;
    }

    const Real td_today = doy + hours / 24.0;

    Integer n_season = (doy + 45) / 92;
    if (n_season == 0)
        n_season = 4;

    Integer s_season = n_season + 2;
    if (s_season > 4)
        s_season -= 4;

    const Real declination_today = 0.4058938 * sin(ANNUAL_FREQ * (td_today - 81.3051))
        + 6.649704e-3 * sin(2 * ANNUAL_FREQ * (td_today - 43.9181))
        + 2.914700e-3 * sin(3 * ANNUAL_FREQ * (td_today - 22.4181))
        - 2.268928e-4 * sin(4 * ANNUAL_FREQ * (td_today + 12.9069))
        + 1.919862e-4 * sin(6 * ANNUAL_FREQ * (td_today - 9.4731)) + 5.919057e-3;

    const Real sin_dcl_today = sin(declination_today);
    const Real cos_dcl_today = cos(declination_today);

    const Real eqn_of_time_today = -3.220132e-2 * sin(ANNUAL_FREQ * (td_today - 3.5631))
        - 4.306600e-2 * sin(2 * ANNUAL_FREQ * (td_today + 9.4369))
        + 1.178097e-3 * sin(3 * ANNUAL_FREQ * (td_today - 52.5631))
        - 8.726646e-4 * cos(4 * ANNUAL_FREQ * (td_today - 16.5631));

    Real td_tomorrow = td_today + 1.0;
    const Real declination_tomorrow = 0.4058938 * sin(ANNUAL_FREQ * (td_tomorrow - 81.3051))
        + 6.649704e-3 * sin(2 * ANNUAL_FREQ * (td_tomorrow - 43.9181))
        + 2.914700e-3 * sin(3 * ANNUAL_FREQ * (td_tomorrow - 22.4181))
        - 2.268928e-4 * sin(4 * ANNUAL_FREQ * (td_tomorrow + 12.9069))
        + 1.919862e-4 * sin(6 * ANNUAL_FREQ * (td_tomorrow - 9.4731)) + 5.919057e-3;

    const Real sin_dcl_tomorrow = sin(declination_tomorrow);
    const Real cos_dcl_tomorrow = cos(declination_tomorrow);

    const Real eqn_of_time_tomorrow = -3.220132e-2 * sin(ANNUAL_FREQ * (td_tomorrow - 3.5631))
        - 4.306600e-2 * sin(2 * ANNUAL_FREQ * (td_tomorrow + 9.4369))
        + 1.178097e-3 * sin(3 * ANNUAL_FREQ * (td_tomorrow - 52.5631))
        - 8.726646e-4 * cos(4 * ANNUAL_FREQ * (td_tomorrow - 16.5631));

    // cos(radians(sqrt(height * 1e3) * 0.0347 + 90.83))
    const Real cos_horizon_depression_80 = -0.18471715;
    const Real cos_horizon_depression_110 = -0.21369007;
    const Real cos_horizon_depression_200 = -0.28147592;

    const Real rssn = ionosphere_coefficients->rssn;
    const Real cov = rssn * (rssn * 8.9e-4 + 0.728) + 63.75;

    RealArray bfr_n_day(3), bfr_s_day(3), bfr_n_night(3), bfr_s_night(3);
    const Real coef = (rssn - 10.0) / 90.0;
    for (Integer lat_idx = 0; lat_idx < 3; lat_idx++)
    {
        bfr_n_day[lat_idx] = B0F_DAY_LO[n_season - 1][lat_idx] + coef * (B0F_DAY_HI[n_season - 1][lat_idx] - B0F_DAY_LO[n_season - 1][lat_idx]);
        bfr_s_day[lat_idx] = B0F_DAY_LO[s_season - 1][lat_idx] + coef * (B0F_DAY_HI[s_season - 1][lat_idx] - B0F_DAY_LO[s_season - 1][lat_idx]);

        bfr_n_night[lat_idx] = B0F_NIGHT_LO[n_season - 1][lat_idx] + coef * (B0F_NIGHT_HI[n_season - 1][lat_idx] - B0F_NIGHT_LO[n_season - 1][lat_idx]);
        bfr_s_night[lat_idx] = B0F_NIGHT_LO[s_season - 1][lat_idx] + coef * (B0F_NIGHT_HI[s_season - 1][lat_idx] - B0F_NIGHT_LO[s_season - 1][lat_idx]);
    }

    // 1. Calculate end points which speccify path inside ionosphere
    // Solution to where a line intersects a sphere is a quadratic equation
    Real a, b, c, discriminant;
    Rvector3 s = spacecraftLoc - stationLoc;
    // Solve for intersection of signal with sphere of IONOSPHERE_MAX_ALTITUDE
    a = s * s;
    b = 2.0 * stationLoc * s;
    c = stationLoc * stationLoc - (earthRadius + IONOSPHERE_MAX_ALTITUDE) * (earthRadius + IONOSPHERE_MAX_ALTITUDE);

    discriminant = b * b - 4.0 * a * c;
    if (discriminant <= 0)
    {
        // Path does not travel through ionosphere
        RealArray out = { 0.0, 0.0, 0.0 };
        return out;
    }

    Real d1, d2; // Roots of quadratic equation
    Real val = GmatMathUtil::Sqrt(discriminant);
    d1 = (-b - val) / (2.0 * a);
    d2 = (-b + val) / (2.0 * a);

    if ((d1 > 1 && d2 > 1) || (d1 < 0 && d2 < 0))
    {
        // Segment between start and end does not travel through ionosphere
        RealArray out = { 0.0, 0.0, 0.0 };
        return out;
    }

    d1 = GmatMathUtil::Max(d1, 0); // Truncate segment before start point of signal
    d2 = GmatMathUtil::Min(d2, 1); // Truncate segment after end point of signal

    Rvector3 start, end;
    start = stationLoc + d1 * s;
    end = stationLoc + d2 * s;

    electronDensities = {};

    Rvector3 dR = (end - start) / NUM_OF_INTERVALS;
    Rvector3 p1 = start;
    Rvector3 p2;

    for (int i = 0; i < NUM_OF_INTERVALS; ++i)
    {
        p2 = p1 + dR;

        const Real rxy = hypot(p2[0], p2[1]);
        Real r_lat = atan2(p2[2], rxy);
        Real old_lat, cFactor, sin_lat;
        do
        {
            old_lat = r_lat;
            sin_lat = sin(old_lat);
            cFactor = RADIUS / sqrt(1.0 - EC_SQ * sin_lat * sin_lat);
            r_lat = atan2(p2[2] + cFactor * EC_SQ * sin_lat, rxy);
        } while (fabs(r_lat - old_lat) > 1.0e-7);

        sin_lat = sin(r_lat);
        cFactor = RADIUS / sqrt(1.0 - EC_SQ * sin_lat * sin_lat);

        const Real cos_lat = cos(r_lat);
        const Real height = rxy / cos_lat - cFactor;

        if (height < 65.0 || height > 2000.0) // Outside ionosphere
        {
            electronDensities.push_back(0.0);
            p1 = p2;
            continue;
        }

        const Real lat = r_lat * GmatMathConstants::DEG_PER_RAD;

        Real r_lon = atan2(p2[1], p2[0]);
        const Real lon = (r_lon >= 0) ? r_lon * GmatMathConstants::DEG_PER_RAD : r_lon * GmatMathConstants::DEG_PER_RAD + 360.0;

        const Real sin_lon = p2[1] / rxy;
        const Real cos_lon = p2[0] / rxy;

        const Real hour_local = fmod(hours + lon / 15.0, 24.0);
        const Real td_noon = doy + 1.5 - lon / 360;

        const Real declination_noon = 0.4058938 * sin(ANNUAL_FREQ * (td_noon - 81.3051))
            + 6.649704e-3 * sin(2 * ANNUAL_FREQ * (td_noon - 43.9181))
            + 2.914700e-3 * sin(3 * ANNUAL_FREQ * (td_noon - 22.4181))
            - 2.268928e-4 * sin(4 * ANNUAL_FREQ * (td_noon + 12.9069))
            + 1.919862e-4 * sin(6 * ANNUAL_FREQ * (td_noon - 9.4731)) + 5.919057e-3;

        const Real eqn_of_time_noon = -3.220132e-2 * sin(ANNUAL_FREQ * (td_noon - 3.5631))
            - 4.306600e-2 * sin(2 * ANNUAL_FREQ * (td_noon + 9.4369))
            + 1.178097e-3 * sin(3 * ANNUAL_FREQ * (td_noon - 52.5631))
            - 8.726646e-4 * cos(4 * ANNUAL_FREQ * (td_noon - 16.5631));

        // Noon zenith
        Real cos_xhinon = sin_lat * sin(declination_noon) + cos_lat * cos(declination_noon) * cos(eqn_of_time_noon);

        if (cos_xhinon <= 0)
            cos_xhinon = 1.7453292519072936e-05; // cos(89.999 deg)

        bool use_tomorrow;
        if (hours + 12 * r_lon / GmatMathConstants::PI < 0)
            use_tomorrow = true;
        else if (hours + 12 * r_lon / GmatMathConstants::PI >= 24)
            use_tomorrow = false;
        else
            use_tomorrow = r_lon >= 0;

        Real a, b, eqn_of_time;
        if (use_tomorrow)
        {
            a = sin_lat * sin_dcl_tomorrow;
            b = cos_lat * cos_dcl_tomorrow;
            eqn_of_time = eqn_of_time_tomorrow;
        }
        else
        {
            a = sin_lat * sin_dcl_today;
            b = cos_lat * cos_dcl_today;
            eqn_of_time = eqn_of_time_today;
        }

        const Real cos_xhi = a + b * cos((GmatMathConstants::PI / 12.0) * (hour_local - 12.0) + eqn_of_time);
        const Real xhi = GmatMathConstants::DEG_PER_RAD * acos(cos_xhi); // zenith

        const Real cos_phi_80 = (cos_horizon_depression_80 - a) / b;
        Real sax80, sux80, hpol_factor_80;
        bool is_night_80;
        if (cos_phi_80 <= -1.0)
        {
            // High latitude summer
            sax80 = sux80 = 99;
            is_night_80 = false;
            hpol_factor_80 = 1.0;
        }
        else if (cos_phi_80 >= 1.0)
        {
            // High latitude winter
            sax80 = sux80 = -99;
            is_night_80 = true;
            hpol_factor_80 = 0.0;
        }
        else
        {
            const Real phi = 12 * acos(cos_phi_80) / GmatMathConstants::PI;
            const Real local_noon = 12 - 12 * eqn_of_time / GmatMathConstants::PI;

            sax80 = fmod(local_noon - phi, 24.0);
            if (sax80 < 0.0)
                sax80 += 24.0;

            sux80 = fmod(local_noon + phi, 24.0);
            if (sux80 < 0.0)
                sux80 += 24.0;

            is_night_80 = !(sax80 < hour_local && hour_local < sux80);
            hpol_factor_80 = 1.0 / (exp(sax80 - hour_local) + 1.0) - 1.0 / (exp(sux80 - hour_local) + 1.0);
        }

        if (height < 80.0 && is_night_80)
        {
            electronDensities.push_back(0.0);
            p1 = p2;
            continue;
        }

        const Real cos_phi_110 = (cos_horizon_depression_110 - a) / b;
        Real sax110, sux110, hpol_factor_110;
        bool is_night_110;
        if (cos_phi_110 <= -1.0)
        {
            // High latitude summer
            sax110 = sux110 = 99;
            is_night_110 = false;
            hpol_factor_110 = 1.0;
        }
        else if (cos_phi_110 >= 1.0)
        {
            // High latitude winter
            sax110 = sux110 = -99;
            is_night_110 = true;
            hpol_factor_110 = 0.0;
        }
        else
        {
            Real phi = 12 * acos(cos_phi_110) / GmatMathConstants::PI;
            Real local_noon = 12 - 12 * eqn_of_time / GmatMathConstants::PI;

            sax110 = fmod(local_noon - phi, 24.0);
            if (sax110 < 0.0)
                sax110 += 24.0;

            sux110 = fmod(local_noon + phi, 24.0);
            if (sux110 < 0.0)
                sux110 += 24.0;

            is_night_110 = !(sax110 < hour_local && hour_local < sux110);
            hpol_factor_110 = 1.0 / (exp(sax110 - hour_local) + 1.0) - 1.0 / (exp(sux110 - hour_local) + 1.0);
        }

        const Real cos_phi_200 = (cos_horizon_depression_200 - a) / b;
        Real sax200, sux200, hpol_factor_200;
        if (cos_phi_200 <= -1.0)
        {
            // High latitude summer
            sax200 = sux200 = 99;
            hpol_factor_200 = 1.0;
        }
        else if (cos_phi_200 >= 1.0)
        {
            // High latitude winter
            sax200 = sux200 = -99;
            hpol_factor_200 = 0.0;
        }
        else
        {
            Real phi = 12 * acos(cos_phi_200) / GmatMathConstants::PI;
            Real local_noon = 12 - 12 * eqn_of_time / GmatMathConstants::PI;

            sax200 = fmod(local_noon - phi, 24.0);
            if (sax200 < 0.0)
                sax200 += 24.0;

            sux200 = fmod(local_noon + phi, 24.0);
            if (sux200 < 0.0)
                sux200 += 24.0;

            hpol_factor_200 = 1.0 / (exp(sax200 - hour_local) + 1.0) - 1.0 / (exp(sux200 - hour_local) + 1.0);
        }

        Real magbr, modip, sin_modip;
        magnetic_field->get_field(sin_lat, cos_lat, sin_lon, cos_lon, magbr, modip, sin_modip);

        const Real foe = foeedi(cov, xhi, cos_xhinon, lat, cos_lat);
        const Real nme = foe * 1.24e10 * foe;

        Real xm3000, fof2;
        ionosphere_coefficients->get_vals(cos_lat, sin_lon, cos_lon, sin_modip, xm3000, fof2);

        const Real hmf2 = hmf2ed(magbr, rssn, fof2, foe, xm3000);

        if (rap >= 200.0)
            fof2 *= storm(doy, magnetic_field->conver(lat, lon), rap);

        const Real nmf2 = fof2 * 1.24e10 * fof2;

        if (height >= hmf2)
        {
            Real dndhmx = log(fof2) * 1.714 - 3.467 + log(xm3000) * 2.02;
            dndhmx = exp(dndhmx) * 0.01;
            const Real b2bot = fof2 * 0.04774 * fof2 / dndhmx;

            Real b2k = 3.22 - fof2 * 0.0538 - hmf2 * 0.00664 + hmf2 * 0.113 / b2bot + rssn * 0.00257;
            const Real ee = exp((b2k - 1.0) * 2.0);
            b2k = (b2k * ee + 1.0) / (ee + 1.0);
            const Real b2top = b2k * b2bot;

            electronDensities.push_back(xe1(height, nmf2, hmf2, b2top));
            p1 = p2;
            continue;
        }

        Integer season;
        if (lat > 0)
            season = n_season;
        else
        {
            season = n_season + 2;
            if (season > 4)
                season -= 4;
        }

        const Real b0 = b0_98(hpol_factor_200, bfr_n_day, bfr_s_day, bfr_n_night, bfr_s_night, lon, modip);
        const Real b1 = 2.6 + (1.9 - 2.6) * hpol_factor_200;

        const Real fof1 = fof1ed(magbr, rssn, xhi, cos_xhi);
        const Real nmf1 = fof1 * 1.24e10 * fof1;

        const Real c1 = f1_c1(modip, hour_local, sux200, sax200);

        const Real m_lat = magnetic_field->geomag_lat(sin_lat, cos_lat, sin_lon, cos_lon);

        bool f1reg = f1_prob(cos_xhi, m_lat, rssn) >= 0.5;

        const Real dela = (abs(modip) >= 18.0) ? (exp(-(abs(modip) - 30.0) / 10.0) + 1.0) : 4.32;

        const Real xdel = (season == 4) ? (10.0 / dela) : (5.0 / dela);
        const Real dndhbr = (season == 2) ? (0.01 / dela) : (0.016 / dela);

        const Real hdeep = 28.0 + (10.5 / dela - 28.0) * hpol_factor_110;
        Real width = (22.0 / dela + 45.0) + ((17.8 / dela) - (22.0 / dela + 45.0)) * hpol_factor_110;
        Real depth = 81.0 + (xdel - 81.0) * hpol_factor_110;
        const Real dlndh = 0.06 + (dndhbr - 0.06) * hpol_factor_110;

        Real e_0, e_1, e_2, e_3;
        if (depth < 1.0)
            width = 0.0;
        else
        {
            if (is_night_110)
                depth = -depth;

            if (tal(hdeep, depth, width, dlndh, e_0, e_1, e_2, e_3))
                width = 0.0;
        }

        Real hef = 110.0 + width;
        Real hefold = hef;

        const Real nmd = xmded(cos_xhi, rssn);
        const Real hmd = 88.0 + (81.0 - 88.0) * hpol_factor_80;
        const Real f_0 = 0.05 + ((0.03 / dela + 0.02) - 0.05) * hpol_factor_80;
        const Real f_1 = 4.5 + (4.6 - 4.5) * hpol_factor_80;
        const Real f_2 = -4.0 + (-11.5 + 4.0) * hpol_factor_80;
        const Real fp1 = f_0;
        const Real fp2 = -fp1 * fp1 / 2.0;
        const Real fp30 = (-f_1 * fp2 - fp1 + 1.0 / f_1) / (f_1 * f_1);
        const Real fp3u = (-f_2 * fp2 - fp1 - 1.0 / f_2) / (f_2 * f_2);
        const Real hdx = hmd + f_1;

        Real x = hdx - hmd;
        const Real xdx = nmd * exp(x * (fp1 + x * (fp2 + x * fp30)));
        const Real dxdx = xdx * (fp1 + x * (fp2 * 2.0 + x * 3.0 * fp30));
        x = 110.0 - hdx;
        Real xkk = -dxdx * x / (xdx * log(xdx / nme));

        const Real xkkmax = 5.0;
        Real xe2h;
        Real d1;
        if (xkk > xkkmax)
        {
            xkk = xkkmax;
            d1 = -log(xdx / nme) / pow(x, xkk);
        }
        else
            d1 = dxdx / (xdx * xkk * pow(x, xkk - 1.0));

        Real hmf1;
        if (f1reg) // This flow copied from IRI2007
        {
            const Real bnmf1 = nmf1 * 0.9;
            if (nme >= bnmf1)
                goto bravo;

        alpha:
            xe2h = xe2(hef, hmf2, nmf2, b0, b1);
            if (xe2h > bnmf1)
            {
                hef -= 1.0;
                if (hef > 110.0)
                    goto alpha;
            }
            else
            {
                if (!regfa1_xe2(hef, hmf2, xe2h, nmf2, nmf1, b0, b1, hmf1))
                    goto charlie;
            }
        bravo:
            hmf1 = 0.0;
            f1reg = false;
        charlie:
            if (hef == hefold)
                goto cont;

            width = hef - 110.0;
            if (is_night_110)
                depth = -depth;

            if (!tal(hdeep, depth, width, dlndh, e_0, e_1, e_2, e_3))
                goto cont;

            width = 0.0;
            hefold = hef = 110.0;
            goto alpha;
        }
        else
            hmf1 = 0.0;

    cont:
        Real hf1, xf1;
        if (f1reg)
        {
            hf1 = hmf1;
            xf1 = nmf1;
        }
        else
        {
            hf1 = (hmf2 + hef) / 2.0;
            xf1 = xe2(hf1, hmf2, nmf2, b0, b1);
        }

        Real hz, t, hst;

        Real xf2 = xe3(hef, hmf2, nmf2, b0, b1, f1reg, hmf1, c1);
        if (xf2 > nme || regfa1_xe3(hf1, hef, xf1, xf2, nme, hmf2, nmf2, b0, b1, f1reg, hmf1, c1, hst))
        {
            hz = (hef + hf1) / 2.0;
            const Real xnehz = xe3(hz, hmf2, nmf2, b0, b1, f1reg, hmf1, c1);
            t = (xnehz - nme) / (hz - hef);
            hst = -333.0;
        }
        else
        {
            hz = (hst + hf1) / 2.0;
            const Real d = hz - hst;
            t = d * d / (hst - hef);
        }

        const Real xhmf1 = hmf1;
        if (hmf1 <= 0.0)
            hmf1 = hz;

        hmf1 = f1reg ? xhmf1 : hmf2;

        if (height > hmf1)
            electronDensities.push_back(xe2(height, hmf2, nmf2, b0, b1));
        else if (height > hz)
            electronDensities.push_back(xe3(height, hmf2, nmf2, b0, b1, f1reg, hmf1, c1));
        else if (height > hef)
            electronDensities.push_back(xe4(height, hmf2, nmf2, b0, b1, f1reg, hmf1, c1, nme, t, hst, hef, hz));
        else if (height > 110.0)
            electronDensities.push_back(xe5(height, is_night_110, nme, e_0, e_1, e_2, e_3));
        else
            electronDensities.push_back(xe6(height, hdx, nme, d1, xkk, hmd, fp3u, fp30, nmd, fp1, fp2));

        p1 = p2;
    }

    delete ionosphere_coefficients;
    delete magnetic_field;

    const Real freq = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / waveLength;

    const Real tec = TEC(end, start);                  // Equation 6.70 of MONTENBRUCK and GILL      // unit: number of electrons/ m^2
    const Real drho = 40.3 * tec / (freq * freq);  // Equation 6.69 of MONTENBRUCK and GILL      // unit: meter

   // Unit of dphi has to be radian because in all caller functions use correction in radian unit.

    const Real dphi = BendingAngle(end, start);                                                      // unit: radian
    const Real dtime = drho / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;                  // unit: s

#ifdef DEBUG_IONOSPHERE_CORRECTION
    MessageInterface::ShowMessage
    ("Ionosphere::Correction: freq = %.12lf MHz,  tec = %.12lfe16,  "
        "drho = %.12lf m, dphi = %.12lf degree, dtime = %.12lf s\n", freq / 1.0e6,
        tec / 1.0e16, drho, dphi* GmatMathConstants::DEG_PER_RAD, dtime);
#endif

    RealArray out = { drho, dphi, dtime };

    return out;
}

//------------------------------------------------------------------------
//RealArray Ionosphere::CalculateTRK223()
//------------------------------------------------------------------------
/**
 * This function is used to calculate Ionosphere correction.
 *
 *
 * return Ionosphere correction vector                 (units: (m, rad, s))
 */
 //------------------------------------------------------------------------

RealArray Ionosphere::CalculateTRK223()
{
#ifdef DEBUG_TRK223
   MessageInterface::ShowMessage("Ionosphere::CalculateTRK223(): Beginning TRK-2-23 Model Calculation \n FilePath names:\n");
   for (UnsignedInt i = 0; i < DSNFilePaths.size(); ++i)
   {
      MessageInterface::ShowMessage("%i -   %s\n", i, DSNFilePaths[i].c_str());
   }
#endif
   
   // Assigns Complex Name if individual station is specified
   std::string DSNComplexName = "";

   // Included to allow common abbreviations in GMAT to convert to DSN format
   if (groundStationId == "GDS")
   {
      groundStationId = "DSN(C10)";
      DSNComplexName = "DSN(C10)";
   }
   else if (groundStationId == "CAN")
   {
      groundStationId = "DSN(C40)";
      DSNComplexName = "DSN(C40)";
   }
   else if (groundStationId == "MAD")
   {
      groundStationId = "DSN(C60)";
      DSNComplexName = "DSN(C60)";
   }
   
   Integer stationNumber;
   if (groundStationId.substr(0, 1) == "C")
   {
      GmatStringUtil::ToInteger(groundStationId.substr(1), stationNumber);
   }
   else
   {
      GmatStringUtil::ToInteger(groundStationId, stationNumber);
   }
   
   if (groundStationId.length() < 3)
   {
      groundStationId = "DSN(0" + groundStationId + ")";
   }
   else
   {
      groundStationId = "DSN(" + groundStationId + ")";
   }

   if (stationNumber < 30)
   {
      DSNComplexName = "DSN(C10)";
   }
   else if (stationNumber < 50 && stationNumber >= 30)
   {
      DSNComplexName = "DSN(C40)";
   }
   else if (stationNumber >= 50)
   {
      DSNComplexName = "DSN(C60)";
   }

   std::string spacecraftName = "SCID(" + GmatStringUtil::RemoveAllBlanks(GmatStringUtil::ToString(spacecraftId)) + ")";

   // Array to hold the indices for the correction data
   Integer ionoLine[2] = {-1,-1};

   // Find the Entries that correspond to the measurement time
   for (UnsignedInt i = 0; i < DSNDatabase.size(); ++i)
   {
      if (DSNDatabase[i][7] == spacecraftName)
      {
         if (DSNDatabase[i][0] == "DOPRNG" || DSNDatabase[i][0] == "RANGE")
         {
            if (DSNDatabase[i][6] == DSNComplexName)
            {
               if (GetTRK223Time(DSNDatabase[i][4]) <= epoch && GetTRK223Time(DSNDatabase[i][5]) >= epoch)
               {
                  if (DSNDatabase[i][3] == "CHPART")
                  {
                     ionoLine[0] = i;
                  }
               }
            }
            else if (DSNDatabase[i][6] == groundStationId)
            {
               if (GetTRK223Time(DSNDatabase[i][4]) <= epoch && GetTRK223Time(DSNDatabase[i][5]) >= epoch)
               {
                  if (DSNDatabase[i][3] == "CHPART")
                  {
                     ionoLine[1] = i;
                  }
               }
            }
         }
      }
   }

   Real correction = 0;

   //Solve correction for the DSN Complex
   if (ionoLine[0] != -1)
   {
      correction = TRK223Solver(DSNDatabase[ionoLine[0]], epoch);
   }
   else
   {
      throw MeasurementException("Ionosphere::CalculateTRK223(): Unable to find ionospheric correction for " + groundStationId + " in DSN Complex " + DSNComplexName + " and " + spacecraftName + " at " + TimeSystemConverter::Instance()->ConvertMjdToGregorian(epoch));
   }
   
   //Solve for the individual station, if specified
   if (ionoLine[1] != -1)
   {
      correction += TRK223Solver(DSNDatabase[ionoLine[1]], epoch);
   }

   // Specify outputs:
   Real drho, dE;

   drho = correction;        // Sign is negative per TRK-2-23 Specifications 3.1.8 Sign of Calibration for doppler and narrow-band, positive for wide-band/range
   dE = 0;                    // unit: radian

   RealArray out;
   out.push_back(drho);
   out.push_back(dE);
   out.push_back(drho / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);

   return out;
}




//------------------------------------------------------------------------
//Real Ionosphere::GetTRK223Time()
//------------------------------------------------------------------------
/**
 * This function is used to parse the time in Mjd from a TRK-2-23 data entry
 *
 * @param TRK223TimeLine            The data to be parsed
 * @param epochStart = true         Bool to state if drawing the time from the start or end of period. Default to start.
 *
 * return time in Mjd format as real
 */
 //------------------------------------------------------------------------

Real Ionosphere::GetTRK223Time(const std::string &TRK223TimeLine)
{
   Real timeReturn;

   // Check format of line, then convert to Modified Julian
   Integer year = 0;
   GmatStringUtil::ToInteger(TRK223TimeLine.substr(0, 2), year);
   if (year >= 69 && year < 1000)
   {
      year += 1900;
   }
   else if (year < 69)
   {
      year += 2000;
   }
   Integer monthTRK = 0;
   GmatStringUtil::ToInteger(TRK223TimeLine.substr(3, 2), monthTRK);
   Integer day = 0;
   GmatStringUtil::ToInteger(TRK223TimeLine.substr(6, 2), day);
   Integer hour = 0;
   GmatStringUtil::ToInteger(TRK223TimeLine.substr(9, 2), hour);
   Integer minute = 0;
   GmatStringUtil::ToInteger(TRK223TimeLine.substr(12, 2), minute);
   Real second = 0;
   if (TRK223TimeLine.length() > 14)
   {
      GmatStringUtil::ToReal(TRK223TimeLine.substr(15), second);
   }

   timeReturn = ModifiedJulianDate(year, monthTRK, day, hour, minute, second);

#ifdef DEBUG_TRK223_FILELOADER

   MessageInterface::ShowMessage("Ionosphere::GetTRK223Time(): Time in TRK-2-23 format : " + timeStrip + "\n");
   MessageInterface::ShowMessage("Ionosphere::GetTRK223Time(): Time converted to Modified Julian format : " + GmatStringUtil::ToString(timeReturn.GetReal()) + "\n");

#endif

   return timeReturn;
}

//------------------------------------------------------------------------
//Real Ionosphere::TRK223Solver(const StringArray &TRK223Line, Real epochTime)
//------------------------------------------------------------------------
/**
 * This function is used to solve the calculation for the correction
 *
 * @param TRK223Line                The data to be parsed
 * @param epochTime                 The time at which the measurement is occuring (Format: Mjd)
 *
 * return Ionosphere range correction                             (unit: m)
 */
 //------------------------------------------------------------------------

Real Ionosphere::TRK223Solver(const StringArray &TRK223Line,Real epochTime)
{
#ifdef DEBUG_TRK223_SOLVER

   MessageInterface::ShowMessage("Ionosphere::TRK223Solver(): Beginning TRK-2-23 Solver With the line: \"" + TRK223Line + "\"\n");

#endif
   // Extract the model type from the line

   std::string modelType = TRK223Line[1];

   // Strip the line of the Coefficients
   RealArray coefs = GmatStringUtil::ToRealArray(TRK223Line[2]);

   Real epochStart = GetTRK223Time(TRK223Line[4]);
   Real epochEnd = GetTRK223Time(TRK223Line[5]);

#ifdef DEBUG_TRK223_SOLVER
   MessageInterface::ShowMessage("Ionosphere::TRK223Solver():  Correction Type is: " + modelType + "\n");
   MessageInterface::ShowMessage("Ionosphere::TRK223Solver():  Epoch Start is " + GmatStringUtil::RealToString(epochStart) + "\n");
   MessageInterface::ShowMessage("Ionosphere::TRK223Solver():  Epoch  End  is " + GmatStringUtil::RealToString(epochEnd) + "\n");
   MessageInterface::ShowMessage("Ionosphere::TRK223Solver():  Cofficients are: (\n");

   for (UnsignedInt i = 0; i < coefs.size(); ++i)
   {
      MessageInterface::ShowMessage(GmatStringUtil::RealToString(coefs[i]) + ", ");
   }
   MessageInterface::ShowMessage(")\n");

#endif

   // Determine if the correction information is for a constant, trig, or power series calculation
   Real drho = 0;

   if (modelType == "CONST")
   {
      drho = coefs[0];
   }
   else if (modelType == "TRIG")
   {
      Real Time = epochTime * 86400;
      Real spanStart = epochStart * 86400;
      Time = GmatMathConstants::TWO_PI*(Time - spanStart) / coefs[0];                //Test variable
      drho = coefs[1];
      Integer count = 1;
      for (Integer i = 2; i < coefs.size(); i++)
      {
         drho += coefs[i] * GmatMathUtil::Cos(Time*count) + coefs[i + 1] * GmatMathUtil::Sin(Time*count);
         count++;
         i++;
      }

   }
   else if (modelType == "NRMPOW")
   {
      Real Time = epochTime * 86400;
      Real spanStart = epochStart * 86400;
      Real spanEnd = epochEnd * 86400;
      Real tS = Time - spanStart;
      Real eS = spanEnd - spanStart;

      Time = 2 * ((Time - spanStart) / (spanEnd - spanStart)) - 1;                //Test variable
      for (Integer i = 0; i < coefs.size(); i++)
      {
         drho += coefs[i] * GmatMathUtil::Pow(Time, i);
      }

   }
   else
   {
      throw MeasurementException("Error: Math Format, " + modelType + ", found in .csp file does not match the allowed types NRMPOW, TRIG, or CONST.");
   }

   Real freq = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / waveLength;  // Hz
   Real freqSBand = 2295.0 * 1000000.0;                                        // S-Band Freq in Hz

   drho = drho * (freqSBand / freq)*(freqSBand / freq);

#ifdef DEBUG_TRK223_SOLVER
   MessageInterface::ShowMessage("Ionosphere::TRK223Solver():  Correction is: " + GmatStringUtil::ToString(drho) + " m\n");
#endif
   return drho;
}
