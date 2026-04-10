//$Id:
//------------------------------------------------------------------------------
//                           GmatDefaults
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS
// Task Order 28
//
// Author: W. Shoan/NASA/GSFC/583
// Created: 2011.03.04
//
/**
 * Provides commonly used default values.
 */
//------------------------------------------------------------------------------
#include <limits>
#include "utildefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector6.hpp"

#ifndef GmatDefaults_hpp
#define GmatDefaults_hpp

// Constants for physical and orbital parameters for Solar System bodies
namespace GmatSolarSystemDefaults
{
   /// Default planet data ---------------------------- planets ---------------
   enum DefaultPlanets
   {
      MERCURY = 0,
      VENUS,
      EARTH,
      MARS,
      JUPITER,
      SATURN,
      URANUS,
      NEPTUNE,
      PLUTO,
      NumberOfDefaultPlanets
   };

   /// default names for each of the possible celestial bodies in the solar system
   const std::string SOLAR_SYSTEM_BARYCENTER_NAME = "SolarSystemBarycenter";

   const std::string SUN_NAME        = "Sun";

   const std::string MERCURY_NAME    = "Mercury";

   const std::string VENUS_NAME      = "Venus";

   const std::string EARTH_NAME      = "Earth";
   const std::string MOON_NAME       = "Luna";

   const std::string MARS_NAME       = "Mars";
   const std::string PHOBOS_NAME     = "Phobos";
   const std::string DEIMOS_NAME     = "Deimos";

   const std::string JUPITER_NAME    = "Jupiter";
   const std::string METIS_NAME      = "Metis";
   const std::string ADRASTEA_NAME   = "Adrastea";
   const std::string AMALTHEA_NAME   = "Amalthea";
   const std::string THEBE_NAME      = "Thebe";
   const std::string IO_NAME         = "Io";
   const std::string EUROPA_NAME     = "Europa";
   const std::string GANYMEDE_NAME   = "Ganymede";
   const std::string CALLISTO_NAME   = "Callisto";

   const std::string SATURN_NAME     = "Saturn";
   const std::string PAN_NAME        = "Pan";
   const std::string ATLAS_NAME      = "Atlas";
   const std::string PROMETHEUS_NAME = "Promethus";
   const std::string PANDORA_NAME    = "Pandora";
   const std::string EPIMETHEUS_NAME = "Epimetheus";
   const std::string JANUS_NAME      = "Janus";
   const std::string MIMAS_NAME      = "Mimas";
   const std::string ENCELADUS_NAME  = "Enceladus";
   const std::string TETHYS_NAME     = "Tethys";
   const std::string TELESTO_NAME    = "Telesto";
   const std::string CALYPSO_NAME    = "Calypso";
   const std::string DIONE_NAME      = "Dione";
   const std::string HELENE_NAME     = "Helene";
   const std::string RHEA_NAME       = "Rhea";
   const std::string TITAN_NAME      = "Titan";
   const std::string IAPETUS_NAME    = "Iapetus";
   const std::string PHOEBE_NAME     = "Phoebe";

   const std::string URANUS_NAME     = "Uranus";
   const std::string CORDELIA_NAME   = "Cordelia";
   const std::string OPHELIA_NAME    = "Ophelia";
   const std::string BIANCA_NAME     = "Bianca";
   const std::string CRESSIDA_NAME   = "Cressida";
   const std::string DESDEMONA_NAME  = "Desdemona";
   const std::string JULIET_NAME     = "Juliet";
   const std::string PORTIA_NAME     = "Portia";
   const std::string ROSALIND_NAME   = "Rosalind";
   const std::string BELINDA_NAME    = "Belinda";
   const std::string PUCK_NAME       = "Puck";
   const std::string MIRANDA_NAME    = "Miranda";
   const std::string ARIEL_NAME      = "Ariel";
   const std::string UMBRIEL_NAME    = "Umbriel";
   const std::string TITANIA_NAME    = "Titania";
   const std::string OBERON_NAME     = "Oberon";

   const std::string NEPTUNE_NAME    = "Neptune";
   const std::string NAIAD_NAME      = "Naiad";
   const std::string THALASSA_NAME   = "Thalassa";
   const std::string DESPINA_NAME    = "Despina";
   const std::string GALATEA_NAME    = "Galatea";
   const std::string LARISSA_NAME    = "Larissa";
   const std::string PROTEUS_NAME    = "Proteus";
   const std::string TRITON_NAME     = "Triton";

   const std::string PLUTO_NAME      = "Pluto";
   const std::string CHARON_NAME     = "Charon";

   /// Default barycenter data

   // default values for CelestialBody data

   const Integer               SSB_NAIF_ID          = 0;
   const Real                  SSB_MU               = 0.0; // Not Necessary

   /// Default planet data

   // default values for CelestialBody data
   const std::string           PLANET_NAMES[NumberOfDefaultPlanets] =
   {
      MERCURY_NAME,
      VENUS_NAME,
      EARTH_NAME,
      MARS_NAME,
      JUPITER_NAME,
      SATURN_NAME,
      URANUS_NAME,
      NEPTUNE_NAME,
      PLUTO_NAME,
   };

   // Units for Equatorial radius are km
   const Real                  PLANET_EQUATORIAL_RADIUS[NumberOfDefaultPlanets] =
   {
      2.43970000000000e+003, // to match STK 2006.01.31 - was 2439.7,
      6.05190000000000e+003, // match to STK 2006.01.31 - was 6051.8,
      6.3781363E3, // to match STK 2006.01.31 - was 6378.1363,
      3.39700000000000e+003, // to match STK 2006.01.31 - was 3396.200,
      7.14920000000000e+004, // to match STK 2006.01.31 - was 71492.00,
      6.02680000000000e+004, // to match STK 2006.01.31 - was 60368.0,
      2.55590000000000e+004, // to match STK 2006.01.31 - was 25559.0,
      2.52690000000000e+004, // to match STK 2006.01.31 - was 24764.0,
      1162.0 // changed to match with STK. old:1195.0
   };

   const Real                  PLANET_FLATTENING[NumberOfDefaultPlanets] =
   {
      0.0,
      0.0,
      0.00335270, // match to STK 2006.01.31 - was 0.0033528, // or 0.0033528106647474807198455?
      0.00647630, // match to STK 2006.01.31 - was 0.0064763,
      0.06487439, // match to STK 2006.01.31 - was 0.0648744,
      0.09796243, // match to STK 2006.01.31 - was 0.0979624,
      0.02292734, // match to STK 2006.01.31 - was 0.0229273,
      0.01856029, // match to STK 2006.01.31 - was 0.0171,
      0.0
   };

   // Units for Mu are km^3/s^2
   const Real                  PLANET_MU[NumberOfDefaultPlanets] =
   {
      22032.080486418,
      324858.59882646,
      398600.4415,
      42828.314258067,
      126712767.85780,
      37940626.061137,
      5794549.0070719,
      6836534.0638793,
      981.60088770700
   };



   const Real                  PLANET_TWO_BODY_EPOCH[NumberOfDefaultPlanets] =
   {
      21544.500370768266, 21544.500370768266, 21544.500370768266,
      21544.500370768266, 21544.500370768266, 21544.500370768266,
      21544.500370768266, 21544.500370768266, 21544.500370768266
   };

   const Rvector6              PLANET_TWO_BODY_ELEMENTS[NumberOfDefaultPlanets] =
   {
      Rvector6(57909212.938567216, 0.20562729774965544, 28.551674963293556,
               10.99100758149257, 67.548689584103984,  175.10396761800456),
      Rvector6(108208423.76486244, 0.0067572911404369688, 24.433051334216176,
               8.007373221205856, 124.55871063212626,     49.889845117140576),
      Rvector6(149653978.9783766,        0.01704556707314489, 23.439034090426388,
               0.00018646554487906264, 101.7416388084352,    358.12708491129),
      Rvector6(227939100.16983532,   0.093314935483163344, 24.677089965042784,
               3.3736838414054472, 333.01849018562076,     23.020633424007744),
      Rvector6(779362950.5867208, 0.049715759324379896, 23.235170252934984,
               3.253166212922,   12.959463238924978,    20.296667207322848),
      Rvector6(1433895241.1645338,  0.055944006117351672, 22.551333377462712,
               5.9451029086964872, 83.977808941927856,   316.23400767222348),
      Rvector6(2876804054.239868,   0.044369079419761096, 23.663364175915172,
               1.850441916938424, 168.86875273062818,    145.8502865552013),
      Rvector6(4503691751.2342816, 0.011211871260687014, 22.29780590076114,
               3.47555654789392,  33.957145210261132,   266.76236610390636),
      Rvector6(5909627293.567856,    0.24928777871911536, 23.4740184346088,
               43.998303104440304, 183.03164997859696,    25.513664216653164)
   };



   //         SpinAxisRA  Rate        SpinAxisDEC  Rate        Rotation    Rate
   const Rvector6              PLANET_ORIENTATION_PARAMETERS[NumberOfDefaultPlanets] =
   {
      Rvector6(281.01,    -0.033,      61.45,    -0.005,        329.548,    6.1385025),
      Rvector6(272.76,     0.0   ,     67.16,     0.0,          160.20,    -1.4813688),
      Rvector6(  0.0,     -0.641,      90.00,    -0.557,        190.147,  360.9856235),
      Rvector6(317.68143, -0.1061,     52.88650, -0.0609,       176.630,  350.89198226),
      Rvector6(268.05,    -0.009,      64.49,     0.003,        284.95,   870.5366420),
      Rvector6( 40.589,   -0.036,      83.537,   -0.004,         38.90,   810.7939024),

      Rvector6(257.311,    0.0,       -15.175,    0.0,          203.81,  -501.1600928),
      Rvector6(299.36,     0.70,       43.46,    -0.51,         253.18,   536.3128492), // Neptune needs more, though
      Rvector6(313.02,     0.0,         9.09,     0.0,          236.77,   -56.3623195)
   };

   const Integer               PLANET_NAIF_IDS[NumberOfDefaultPlanets] =
   {
         199,
         299,
         399,
         499,
         599,
         699,
         799,
         899,
         999,
   };

   const std::string           PLANET_SPICE_FRAME_ID[NumberOfDefaultPlanets] =
   {
         "IAU_MERCURY",
         "IAU_VENUS",
         "ITRF93",     // "IAU_EARTH" is low fidelity, should be "ITRF93"
         "IAU_MARS",
         "IAU_JUPITER",
         "IAU_SATURN",
         "IAU_URANUS",
         "IAU_NEPTUNE",
         "IAU_PLUTO",
   };


   /// Default planet data ----------------------------  moons  ---------------
   enum DefaultMoons
   {
      LUNA = 0,
   //      PHOBOS, // wcs 2009.02.17 - add these later?
   //      DEIMOS,
      // @todo - add other solar system moons here
      NumberOfDefaultMoons
   };

   // default values for CelestialBody data
   const std::string           MOON_NAMES[NumberOfDefaultMoons] =
   {
      MOON_NAME,
   //   PHOBOS_NAME,
   //   DEIMOS_NAME,
   };

   const std::string           MOON_CENTRAL_BODIES[NumberOfDefaultMoons] =
   {
      EARTH_NAME,
   //   MARS_NAME,
   //   MARS_NAME,
   };


   // Units for Equatorial radius are km
   const Real                  MOON_EQUATORIAL_RADIUS[NumberOfDefaultMoons] =
   {
      1738.2000,
   //   11.1,       // *** TEMPORARY ***
   //   11.1,       // *** TEMPORARY ***
   };

   const Real                  MOON_FLATTENING[NumberOfDefaultMoons] =
   {
      0.0,
   //   0.0,
   //   0.0,
   };

   // Units for Mu are km^3/s^2
   const Real                  MOON_MU[NumberOfDefaultMoons] =
   {
      4902.8005821478,
   //   7.22e-4,        // *** TEMPORARY ***
   //   7.22e-4,        // *** TEMPORARY ***
   };




   const Real                  MOON_TWO_BODY_EPOCH[NumberOfDefaultMoons] =
   {
         21544.500370768266,
   //      21544.500370768266,   // *** TEMPORARY ***
   //      21544.500370768266,   // *** TEMPORARY ***
   };

   const Rvector6              MOON_TWO_BODY_ELEMENTS[NumberOfDefaultMoons] =
   {
         Rvector6(      385494.90434829952,  0.055908943292024992,   20.940245433093748,
               12.233244412716252, 68.004298803147648,     137.94325682926458),
   //      Rvector6(      385494.90434829952,  0.055908943292024992,   20.940245433093748,
   //            12.233244412716252, 68.004298803147648,     137.94325682926458),           // *** TEMPORARY ***
   //      Rvector6(      385494.90434829952,  0.055908943292024992,   20.940245433093748,
   //            12.233244412716252, 68.004298803147648,     137.94325682926458),           // *** TEMPORARY ***
   };



   //         SpinAxisRA  Rate        SpinAxisDEC  Rate        Rotation    Rate
   const Rvector6              MOON_ORIENTATION_PARAMETERS[NumberOfDefaultMoons] =
   {
      Rvector6(0.0, 0.0, 0.0, 0.0, 0.0, 0.0),  // for now, for default moons, calculations are done in the Moon class
   //   Rvector6(0.0, 0.0, 0.0, 0.0, 0.0, 0.0),  // for now, for default moons, calculations are done in the Moon class
   //   Rvector6(0.0, 0.0, 0.0, 0.0, 0.0, 0.0),  // for now, for default moons, calculations are done in the Moon class
   };

   const Integer               MOON_NAIF_IDS[NumberOfDefaultMoons] =
   {
         301,    // Luna
   //      401,    // Phobos
   //      402,    // Deimos
   };

   const std::string           MOON_SPICE_FRAME_ID[NumberOfDefaultMoons] =
   {
         "MOON_PA",
//         "IAU_PHOBOS",
//         "IAU_DEIMOS",
   };


   /// Default star data ----------------------------  the Sun  ---------------

   // default values for CelestialBody data

   const Real                  STAR_EQUATORIAL_RADIUS  = 695990.0000;  // km
   const Real                  STAR_FLATTENING         = 0.0;
   // Units for MU are km^3/s^2
   const Real                  STAR_MU                 = 132712440017.99;

   // NOTE - these must change when Earth's default values change!!!!!!!!!!!!
   const Real                  STAR_TWO_BODY_EPOCH           = 21544.500370768266;
   const Rvector6              STAR_TWO_BODY_ELEMENTS        = Rvector6(
         149653978.9783766,        0.01704556707314489,  23.439034090426388,
         0.00018646554487906264, 281.7416388084352,     358.12708491129);

   const Rvector6              STAR_ORIENTATION_PARAMETERS   = Rvector6(
         286.13, 0.0, 63.87, 0.0, 84.10, 14.1844000);
   const Integer               STAR_NAIF_IDS                 = 10;
   const std::string           STAR_SPICE_FRAME_ID           = "IAU_SUN";

   const Real                  STAR_RADIANT_POWER            = 1358.0;       // W / m^2
   // Units for reference distance are km (1 AU)
   const Real                  STAR_REFERENCE_DISTANCE       = GmatPhysicalConstants::ASTRONOMICAL_UNIT;;  // km
   // Units for radius are meters
   const Real                  STAR_PHOTOSPHERE_RADIUS       = 695990000.0;;  // m



} // GmatSolarSystemDefaults

#endif // GmatDefaults_hpp
