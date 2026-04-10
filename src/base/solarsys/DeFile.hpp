//$Id$
//------------------------------------------------------------------------------
//                                  DeFile
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy C. Shoan
// Created: 2004/02/17
//
/**
 * This is the class that handles access to a DE file.
 *
 * @todo : complete Convert method for converting to binary
 *
 * @note Code reused from or based on JPL/JSC (D. Hoffman) code.
 *
 * @note If an ASCII file is input on creation, the file will be converted to
 *       a file in native binary format, for efficiency.  This conversion
 *       assumes that there is an appropriate header file in the same directory
 *       as the ASCII file.  The header file should be called header.FMT, where
 *       FMT is the format (e.g. 405, etc.) of the DE file.  NOTE: Conversion
 *       not yet done.
 */
//------------------------------------------------------------------------------
#ifndef DeFile_hpp
#define DeFile_hpp

#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "PlanetaryEphem.hpp"

#include <stdio.h> // for FILE, etc. (for JPL/JSC code (Hoffman))

class GMAT_API DeFile : public PlanetaryEphem
{
public:

   /// default constructor
   DeFile(Gmat::DeFileType ofType, std::string fileName = "",
          Gmat::DeFileFormat fmt = Gmat::DE_BINARY);  
   /// copy constructor
   DeFile(const DeFile& def);
   /// operator=
   DeFile& operator=(const DeFile& sdef);
   /// destructor
   ~DeFile();

   /// method to initialize the DeFile - must be done before De file can be read
   void             Initialize();
   /// method to return the body ID for the requested body
   Integer          GetBodyID(std::string bodyName);
   
   // Not implemented and not used anywhere, so commented out
   // /// method to return the ASCII file name (unknown if created with
   // /// BINARY file)
   // std::string      GetAsciiFileName() const;
   // /// method to return the BINARY file name
   // std::string      GetBinaryFileName() const;
   // /// method to return the type of De File
    Gmat::DeFileType GetDeFileType() const;
   
   /// method to return the position and velocity of the specified body
   /// at the specified time
   Real*            GetPosVel(Integer forBody, A1Mjd atTime,
                              bool overrideTimeSystem = false);

   Real*            GetPosVel(Integer forBody, GmatTime atTime, 
                              bool overrideTimeSystem = false);

   Real*            GetPosDelta(Integer forBody,
                                const GmatTime &atTime1,
                                const GmatTime &atTime2,
                                bool overrideTimeSystem = false);

   /// method to return angles and rates (for Luna ONLY!!)                
   void             GetAnglesAndRates(A1Mjd atTime, Real* angles, Real* rates,
                                      bool overrideTimeSystem = false);

   void             GetAnglesAndRates(const GmatTime &atTime, Real* angles, Real* rates,
                                      bool overrideTimeSystem = false);
   /// method to return the day-of-year and year of the start time of the
   /// DE file.
   Integer*         GetStartDayAndYear();

   /// method to convert an ASCII file to a binary file; this method assumes that
   /// there is  an appropriate header file in the same directory as the
   /// ASCII file.  @todo - Implementation is currently TBD
   std::string      Convert(std::string deFileNameAscii);


   /// static values for the IDs for each celestial body, as used on the DE files.
   static const Integer SUN_ID              = 10;
   static const Integer MERCURY_ID          = 0;
   static const Integer VENUS_ID            = 1;
   static const Integer EARTH_ID            = 2;
   static const Integer MOON_ID             = 9;
   static const Integer MARS_ID             = 3;
   static const Integer JUPITER_ID          = 4;
   static const Integer SATURN_ID           = 5;
   static const Integer URANUS_ID           = 6;
   static const Integer NEPTUNE_ID          = 7;
   static const Integer PLUTO_ID            = 8;
   static const Integer SS_BARY_ID          = 11;
   static const Integer EM_BARY_ID          = 12;
   static const Integer NUTATIONS_ID        = 13;
   static const Integer LIBRATIONS_ID       = 14;

protected:
   
   // We cannot initialize const Real here
   static const Real    JD_MJD_OFFSET; // = GmatTimeConstants::JD_JAN_5_1941;
   /// seconds offset to get from A1 to TDT (TT)
   static const Real    TT_OFFSET;     // = GmatTimeConstants::TT_TAI_OFFSET;

public:
   
   static const Integer ARRAY_SIZE_200 = 826;
   static const Integer ARRAY_SIZE_405 = 1018;
   static const Integer ARRAY_SIZE_421 = 1018;
   static const Integer ARRAY_SIZE_424 = 1018;
   static const Integer ARRAY_SIZE_430 = 1018;
   static const Integer MAX_ARRAY_SIZE = 1018;

   static const Integer FAILURE        = 1; // from JPL/JSC ephem_types.h
   static const Integer SUCCESS        = 0; // from JPL/JSC ephem_types.h


protected:

   /// structs representing the state date (position and velocity)
   /// (from JPL/JSC code - Hoffman)
   struct GMAT_API stateData
   {
      /// default constructor
      stateData()
      {
         Position[0] = 0.0;
         Position[1] = 0.0;
         Position[2] = 0.0;
         Velocity[0] = 0.0;
         Velocity[1] = 0.0;
         Velocity[2] = 0.0;

         Acceleration[0] = 0.0;
         Acceleration[1] = 0.0;
         Acceleration[2] = 0.0;
      }
      /// copy constructor
      stateData(const stateData& sd)
      {
         Position[0] = sd.Position[0];
         Position[1] = sd.Position[1];
         Position[2] = sd.Position[2];
         Velocity[0] = sd.Velocity[0];
         Velocity[1] = sd.Velocity[1];
         Velocity[2] = sd.Velocity[2];

         Acceleration[0] = sd.Acceleration[0];
         Acceleration[1] = sd.Acceleration[1];
         Acceleration[2] = sd.Acceleration[2];
      }
      /// operator=
      stateData& operator=(const stateData& sd)
      {
         Position[0] = sd.Position[0];
         Position[1] = sd.Position[1];
         Position[2] = sd.Position[2];
         Velocity[0] = sd.Velocity[0];
         Velocity[1] = sd.Velocity[1];
         Velocity[2] = sd.Velocity[2];

         Acceleration[0] = sd.Acceleration[0];
         Acceleration[1] = sd.Acceleration[1];
         Acceleration[2] = sd.Acceleration[2];
         return *this;
      }
      /// data
      double Position[3];
      double Velocity[3];
      double Acceleration[3];
   };

   typedef struct stateData stateType;
   
   // structs representing the format of the header records
   // (from JPL/JSC code - Hoffman)
   // wcs - added constructors, and operator=
#pragma pack(push, 1)
   struct GMAT_API recOneData
   {
      recOneData()  // default constructor
      {
         int i, j;
         for (i=0;i<3;i++)
         {
            strcpy(label[i],"\0");
            timeData[i]  = 0.0;
            libratPtr[i] = 0;
            for (j=0;j<12;j++) coeffPtr[j][i] = 0;
         }
         for (i=0;i<400;i++)    strcpy(constName[i],"\0");
         numConst        = 0;
         AU              = 0.0;
         EMRAT           = 0.0;
         #if (USE_64_BIT_LONGS == 1)
            DENUM           = (int) 0;
         #else
            DENUM           = (long int) 0;
         #endif
      }
      recOneData(const recOneData& r)  // copy constructor
      {
         int i, j;
         for (i=0;i<3;i++)
         {
            strcpy(label[i], r.label[i]);
            timeData[i]  = r.timeData[i];
            libratPtr[i] = r.libratPtr[i];
            for (j=0;j<12;j++) coeffPtr[j][i] = r.coeffPtr[j][i];
         }
         for (i=0;i<400;i++)    strcpy(constName[i],r.constName[i]);
         numConst        = r.numConst;
         AU              = r.AU;
         EMRAT           = r.EMRAT;
         DENUM           = r.DENUM;
      }
      recOneData& operator=(const recOneData& r) 
      {
         int i, j;
         for (i=0;i<3;i++)
         {
            strcpy(label[i], r.label[i]);
            timeData[i]  = r.timeData[i];
            libratPtr[i] = r.libratPtr[i];
            for (j=0;j<12;j++) coeffPtr[j][i] = r.coeffPtr[j][i];
         }
         for (i=0;i<400;i++)    strcpy(constName[i],r.constName[i]);
         numConst        = r.numConst;
         AU              = r.AU;
         EMRAT           = r.EMRAT;
         DENUM           = r.DENUM;
         return *this;
      }
      // data
      char label[3][84];
      char constName[400][6];
      double timeData[3];
      #if (USE_64_BIT_LONGS == 1)
         int numConst;
      #else
         long int numConst;
      #endif
      double AU;
      double EMRAT;
      #if (USE_64_BIT_LONGS == 1)
         int coeffPtr[12][3];
         int DENUM;
         int libratPtr[3];
         int RSize;
      #else
         long int coeffPtr[12][3];
         long int DENUM;
         long int libratPtr[3];
         long int RSize;
      #endif
   };

   struct GMAT_API recTwoData
   {
      // default constructor
      recTwoData()
      {
         int i;
         for (i=0;i<400;i++) constValue[i] = 0.0;
      }
      // copy constructor
      recTwoData(const recTwoData& r)
      {
         int i;
         for (i=0;i<400;i++) constValue[i] = r.constValue[i];
      }
      // operator=
      recTwoData& operator=(const recTwoData& r)
      {
         int i;
         for (i=0;i<400;i++) constValue[i] = r.constValue[i];
         return *this;
      }
      //data
      double constValue[400];
   };
#pragma pack(pop)
    
   typedef struct recOneData recOneType;
   typedef struct recTwoData recTwoType;

   // structs representing the formats of the header records - need ARRAY_SIZE?
   struct GMAT_API headerOne
   {
      recOneType data;
      char pad[MAX_ARRAY_SIZE*sizeof(double) - sizeof(recOneType)];  // MAX
   };

   struct GMAT_API headerTwo
   {
      recTwoType data;
      char pad[MAX_ARRAY_SIZE*sizeof(double) - sizeof(recTwoType)];  // MAX
   };

   typedef struct headerOne headOneType;
   typedef struct headerTwo headTwoType;

private:
   std::string        theFileName;
   Gmat::DeFileFormat theFileFormat;
   
   /// ASCII file name - will be unknown if DeFile created with binary file
   std::string        asciiFileName;

   /// BINARY file name - may be input on creation, or may be result of
   /// conversion of input ASCII file; date will be read from this file.
   std::string        binaryFileName;

   /// type of DE file
   Gmat::DeFileType   defType;

   /// array size for the file format we're using
   Integer            arraySize;

   /// data from JPL/JSC code (Hoffman) ephem_read.c
   headOneType        H1;
   headTwoType        H2;
   recOneType         R1;
   FILE               *Ephemeris_File;
   double             Coeff_Array[MAX_ARRAY_SIZE];   // MAX
   double             T_beg , T_end , T_span;
   /// The base epoch for internal time calculations
   double             baseEpoch;
   double             mFileBeg;
   /// File beginning in ModJulian format
   double             mA1FileBeg;

//   #if (USE_64_BIT_LONGS == 1)
//      int             numConst;   // where is this used?
//   #else
//      long int        numConst;   // where is this used?
//   #endif
   
   Integer            EPHEMERIS; // caps because of reuse from JPL/JSC code (Hoffman)

   // method to initialize the DeFile - must be done before De file can be read
   void InitializeDeFile(std::string fName, Gmat::DeFileFormat fileFmt);


   // --------------- methods from JPL/JSC code (Hoffman) ______________________


   /*-------------------------------------------------------------------------*/
   /*  Read_Coefficients      - from JPL/JSC code (Hoffman)                   */
   /*-------------------------------------------------------------------------*/
   void Read_Coefficients( double Time );

   void Read_Coefficients(GmatTime Time);

   /*-------------------------------------------------------------------------*/
   /*  Initialize_Ephemeris      - from JPL/JSC code (Hoffman)                */
   /*-------------------------------------------------------------------------*/
   int Initialize_Ephemeris( char *fileName );

   /*-------------------------------------------------------------------------*/
   /*  Interpolate_Libration     - from JPL/JSC code (Hoffman)                */
   /*-------------------------------------------------------------------------*/
   void Interpolate_Libration( double Time, int Target, 
                               double Libration[3], double rates[3] );
   
   void Interpolate_Libration( const GmatTime &Time, int Target, 
                               double Libration[3], double rates[3] );

   /*-------------------------------------------------------------------------*/
   /*  Interpolate_Nutation     - from JPL/JSC code (Hoffman)                 */
   /*-------------------------------------------------------------------------*/
   void Interpolate_Nutation( double Time , int Target , double Nutation[2] );

   /*-------------------------------------------------------------------------*/
   /*  Interpolate_Position      - from JPL/JSC code (Hoffman)                */
   /*-------------------------------------------------------------------------*/
   void Interpolate_Position( double Time , int Target , double Position[3] );

   /*-------------------------------------------------------------------------*/
   /*  Interpolate_State     - from JPL/JSC code (Hoffman)                    */
   /*-------------------------------------------------------------------------*/
   void Interpolate_State( double Time , int Target , stateType *p );

   void Interpolate_State(GmatTime Time, int Target, stateType *p);

   void Interpolate_State_Delta( const GmatTime &Time, const GmatTime &Time2,
                                 int Target, stateType *p);

   /*-------------------------------------------------------------------------*/
   /*  Find_Value     - from JPL/JSC code (Hoffman)                           */
   /*-------------------------------------------------------------------------*/
   double Find_Value( char    name[]             ,
                      char    name_array[400][6] ,
                      double  value_array[400]   );

   /*-------------------------------------------------------------------------*/
   /**  Gregorian_to_Julian     - from JPL/JSC code (Hoffman)                 */
   /*-------------------------------------------------------------------------*/
   double Gregorian_to_Julian( int     year ,   int     month   ,
                               int     day  ,   int     hour    ,
                               int     min  ,   double  seconds );

   /*-------------------------------------------------------------------------*/
   /*  Integer modulo function.     - from JPL/JSC code (Hoffman)             */
   /*-------------------------------------------------------------------------*/
   int mod( int x, int y );

   /*-------------------------------------------------------------------------*/
   /*  Read_File_Line     - from JPL/JSC code (Hoffman)                       */
   /*-------------------------------------------------------------------------*/
   int Read_File_Line( FILE *inFile, int filter, char lineBuffer[82]);

   /*-------------------------------------------------------------------------*/
   /*  Read_Group_Header     - from JPL/JSC code (Hoffman)                    */
   /*-------------------------------------------------------------------------*/
   int Read_Group_Header( FILE *inFile );

   /*-------------------------------------------------------------------------*/
   /*  Warning     - from JPL/JSC code (Hoffman)                              */
   /*-------------------------------------------------------------------------*/

   //extern void Warning( int errorCode );
};
#endif // DeFile_hpp
