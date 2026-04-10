//$Id$
//------------------------------------------------------------------------------
//                                  GravityFileUtil
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/08/15
//
/**
 * Implementation of the GravityFileUtil class.  This is the code that reads the
 * different types of gravity files, such as .cof, .dat, .grv files.
 *
 */
//------------------------------------------------------------------------------
#include "GravityFileUtil.hpp"
#include "StringUtil.hpp"       // for Trim()
#include "FileTypes.hpp"        // for MAX_LINE_LEN
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdlib>                      // Required for GCC 4.3

//#define DEBUG_GRAVITY_FILE
//#define DEBUG_GRAVITY_COF_FILE
//#define DEBUG_GRAVITY_DAT_FILE
//#define DEBUG_GRAVITY_GRV_FILE

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string GravityFileUtil::GRAVITY_MODEL_NAMES[GmatGrav::NumGravityModels] =
{
      // Earth Model names
      "EGM-96",
      "JGM-2",
      "JGM-3",
      // Luna Model Names
      "LP-165",
      // Mars Model Names
//      "GMM-1",
//      "GMM-2B",
      "Mars-50C",
      // Venus Model Names
//      "MGN75HSAAP",
      "MGNP-180U",
      "Other",
      "None"
};

//------------------------------------------------------------------------------
// GmatGrav::GravityFileType GetFileType(const char *filename)
//------------------------------------------------------------------------------
/*
 * @see GravityFileType GetFileType(const std::string &filename)
 */
//------------------------------------------------------------------------------
GmatGrav::GravityFileType GravityFileUtil::GetFileType(const char *filename)
{
   return GetFileType(std::string(filename));
}

//------------------------------------------------------------------------------
// GmatGrav::GravityFileType GetFileType(const std::string &filename)
//------------------------------------------------------------------------------
/*
 * Returns recognized gravity file type by reading first non-comment line.
 *
 * Assumption:
 *    GFT_COF contains "POTFIELD"
 *    GFT_DAT contains valid Real number
 *    GFT_GRV contains "stk.v."
 *
 * @param  filename  Input file name
 * @return  Gravity file type
 *
 */
//------------------------------------------------------------------------------
GmatGrav::GravityFileType GravityFileUtil::GetFileType(const std::string &filename)
{
   #ifdef DEBUG_GRAVITY_FILE
   MessageInterface::ShowMessage
      ("GravityFileUtil::GetFileType() entered\n   filename = \"%s\"\n",
       filename.c_str());
   #endif

   std::ifstream inStream(filename.c_str());
   if (!inStream)
      throw GravityFileException("Cannot open gravity file \"" + filename + "\"");

   std::string line;
   GmatGrav::GravityFileType gft = GmatGrav::GFT_UNKNOWN;

   while (!inStream.eof())
   {
      getline(inStream, line);
       
      // if the line is blank, skip it
      if (GmatStringUtil::IsBlank(line, true)) continue;

      // Make upper case, so we can check for certain keyword
      line = GmatStringUtil::ToUpper(line);

      #ifdef DEBUG_GRAVITY_FILE
      MessageInterface::ShowMessage("   => line=<%s>\n", line.c_str());
      #endif

      // Get first non-comment line
      if (line[0] != 'C' && line[0] != '#')
      {
         if (line.find("POTFIELD") != line.npos)
         {
            gft = GmatGrav::GFT_COF;
            break;
         }
         else if ((line.find("STK.V.") != line.npos) ||
                  (line.find("stk.v.") != line.npos))
         {
            gft = GmatGrav::GFT_GRV;
            break;
         }
         else
         {
            Real rval;
            if (GmatStringUtil::ToReal(line, rval))
               gft = GmatGrav::GFT_DAT;

            break;
         }
      }
   }

   inStream.close();

   #ifdef DEBUG_GRAVITY_FILE
   MessageInterface::ShowMessage("GravityFileUtil::GetFileType() returning %d\n", gft);
   #endif

   if (gft == GmatGrav::GFT_UNKNOWN)
      throw GravityFileException
         ("Gravity file \"" + filename + " is of unknown format");

   return gft;
}

//------------------------------------------------------------------------------
// GmatGrav::GravityModelType GetModelType(const char *filename, const char *forBody)
//------------------------------------------------------------------------------
GmatGrav::GravityModelType GravityFileUtil::GetModelType(const char *filename, const char *forBody)
{
   return GetModelType(std::string(filename), std::string(forBody));
}

//------------------------------------------------------------------------------
// GmatGrav::GravityModelType GetModelType(const std::string &filename, const std::string &forBody)
//------------------------------------------------------------------------------
/*
 * Returns recognized gravity model type for input body name, by reading
 * comment line(s).  NOTE - this will only work for COF format files.  Inputting
 * the name of a file with another format (e.g. GRV) will result in a return value
 * of GFM_OTHER.
 *
 * Assumptions:
 *    GFM_EGM96       contains "egm96"
 *    GFM_JGM2        contains "JGM-02"
 *    GFM_JGM3        contains "JGM-03"
 *    GFM_LP165P      contains "lp165p"
 *    GFM_GMM1        contains "GMM-1"        [future?]
 *    GFM_GMM2B       contains "GMM-2B"       [future?]
 *    GFM_MARS50C     contains "Mars-50c"
 *    GFM_MGN75HSAAP  contains "MGN75HSAAP"   [future?]
 *    GFM_MGNP180U    contains "MGNP180U"
 *
 *
 * @param   filename  Input file name
 * @param   forBody   Body Name
 * @return  Gravity model type
 *
 */
//------------------------------------------------------------------------------
GmatGrav::GravityModelType GravityFileUtil::GetModelType(const std::string &filename, const std::string &forBody)
{
   #ifdef DEBUG_GRAVITY_FILE
      MessageInterface::ShowMessage("Entering GetModelType, filename = %s, forBody = %s\n",
            filename.c_str(), forBody.c_str());
   #endif
   if (GmatStringUtil::IsBlank(filename, true))
     return GmatGrav::GFM_NONE;

   if (GetFileType(filename) != GmatGrav::GFT_COF) return GmatGrav::GFM_OTHER;

   std::ifstream inStream(filename.c_str());
   if (!inStream)
      throw GravityFileException("Cannot open gravity file \"" + filename + "\"");

   std::string line;
   GmatGrav::GravityModelType gfm = GmatGrav::GFM_OTHER;

   while (!inStream.eof())
   {
      getline(inStream, line);

      // Make upper case, so we can check for certain keyword
      line = GmatStringUtil::ToUpper(line);

      #ifdef DEBUG_GRAVITY_FILE
      MessageInterface::ShowMessage("   => line=<%s>\n", line.c_str());
      #endif

      // Read comment lines
      if (line[0] == 'C' || line[0] == '#') // do we need
      {
         if (forBody == GmatSolarSystemDefaults::EARTH_NAME)
         {
            if (line.find("EGM96") != line.npos)
            {
               gfm = GmatGrav::GFM_EGM96;
               break;
            }
            else if (line.find("JGM-02") != line.npos)
            {
               gfm = GmatGrav::GFM_JGM2;
               break;
            }
            else if (line.find("JGM-03") != line.npos)
            {
               gfm = GmatGrav::GFM_JGM3;
               break;
            }
         }
         else if (forBody == GmatSolarSystemDefaults::MOON_NAME)
         {
            if (line.find("LP165P") != line.npos)
            {
               gfm = GmatGrav::GFM_LP165P;
               break;
            }
         }
         else if (forBody == GmatSolarSystemDefaults::MARS_NAME)
         {
            if (line.find("MARS-50C") != line.npos)
            {
               gfm = GmatGrav::GFM_MARS50C;
               break;
            }
//            else if (line.find("GMM-1") != line.npos)
//            {
//               gfm = GmatGrav::GFM_GMM1;
//               break;
//            }
//            else if (line.find("GMM-2B") != line.npos)
//            {
//               gfm = GmatGrav::GFM_GMM2B;
//               break;
//            }
         }
         else if (forBody == GmatSolarSystemDefaults::VENUS_NAME)
         {
            if (line.find("MGNP180U") != line.npos)
            {
               gfm = GmatGrav::GFM_MGNP180U;
               break;
            }
//            else if (line.find("MGN75HSAAP") != line.npos)
//            {
//               gfm = GmatGrav::GFM_MGN75HSAAP;
//               break;
//            }
         }
      }
   }

   inStream.close();

   #ifdef DEBUG_GRAVITY_FILE
   MessageInterface::ShowMessage("GravityFileUtil::GetModelType() returning %d (%s)\n", gfm, GRAVITY_MODEL_NAMES[gfm].c_str());
   #endif


   return gfm;
}


//------------------------------------------------------------------------------
// bool GetFileInfo(const std::string &filename, Integer& degree, Integer& order,
//                  Real &mu, Real &radius)
//------------------------------------------------------------------------------
/*
 * Reads degree, order, gravity constant, and equatorial radius from the file.
 *
 * @param  filename  Input file name
 * @param  degree  Degree from the file
 * @param  order  Order from the file
 * @param  mu  Gravity constant from the file
 * @param  radius  Equatorial radius from the file
 *
 * @exception GravityFileException thrown if unrecognized file type found
 */
//------------------------------------------------------------------------------
bool GravityFileUtil::GetFileInfo(const std::string &filename, Integer& degree,
                              Integer& order, Real &mu, Real &radius)
{
   ReadFile(filename, degree, order, mu, radius, false);

   return true;
}


//------------------------------------------------------------------------------
// bool ReadFile(const std::string &filename, Integer& degree, Integer& order,
//               Real &mu, Real &radius, bool readCoeff, Real cbar[][361],
//               Real sbar[][DEG_DIM], Real dcbar[][DRF_DIM], Real dsbar[][DRF_DIM],
//               Integer maxDegree, Integer maxOrder, Integer maxDriftDegree = 0)
//------------------------------------------------------------------------------
bool GravityFileUtil::ReadFile(const std::string &filename, Integer& degree,
                           Integer& order, Real &mu, Real &radius,
                           bool readCoeff, Real cbar[][361], Real sbar[][DEG_DIM],
                           Real dcbar[][DRF_DIM], Real dsbar[][DRF_DIM],
                           Integer maxDegree, Integer maxOrder, Integer maxDriftDegree)
{
   GmatGrav::GravityFileType gft = GetFileType(filename);

   switch (gft)
   {
   case GmatGrav::GFT_COF:
      return ReadCofFile(filename, degree, order, mu, radius, readCoeff, cbar, sbar,
                         maxDegree, maxOrder, maxDriftDegree);
   case GmatGrav::GFT_DAT:
      return ReadDatFile(filename, degree, order, mu, radius, readCoeff, cbar, sbar,
                         dcbar, dsbar, maxDegree, maxOrder, maxDriftDegree);
   case GmatGrav::GFT_GRV:
      return ReadGrvFile(filename, degree, order, mu, radius, readCoeff, cbar, sbar,
                         maxDegree, maxOrder, maxDriftDegree);
   default:
      return false;
   }
}


//------------------------------------------------------------------------------
// bool ReadCofFile(const std::string &filename, Integer& degree, Integer& order,
//                  Real &mu, Real &radius, bool readCoeff, Real cbar[][361],
//                  Real sbar[][DEG_DIM], Integer maxDegree, Integer maxOrder,
//                  Integer maxDriftDegree)
//------------------------------------------------------------------------------
bool GravityFileUtil::ReadCofFile(const std::string &filename, Integer& degree,
                              Integer& order, Real &mu, Real &radius, bool readCoeff,
                              Real cbar[][361], Real sbar[][DEG_DIM], Integer maxDegree,
                              Integer maxOrder, Integer maxDriftDegree)
{
   std::ifstream inStream(filename.c_str());
   if (!inStream)
      throw GravityFileException("Cannot open COF gravity file \"" + filename + "\"");

   Integer       n = -1, m = -1;
   Integer       fileOrder = -1, fileDegree = -1;
   Real          Cnm=0.0, Snm=0.0;
   Integer       noIdea;
   Real          noClue;
   Real          tmpMu;
   Real          tmpA;

   #ifdef DEBUG_GRAVITY_COF_FILE
   MessageInterface::ShowMessage("Entered GravityFileUtil::ReadCofFile\n");
   #endif

   std::string line;
   std::string firstStr;
   std::string degStr, ordStr;
   std::string nStr, mStr, cnmStr, snmStr;

   while (!inStream.eof())
   {
      getline(inStream, line);
      std::istringstream lineStream;

      // ignore comment lines
      if (line[0] != 'C')
      {
         firstStr = line.substr(0, 8);
         firstStr = GmatStringUtil::Trim(firstStr);

         if (firstStr == "END") break;
         if (firstStr == "POTFIELD")
         {
            degStr = line.substr(8, 3);
            ordStr = line.substr(11, 3);

            if ((GmatStringUtil::ToInteger(degStr, fileDegree)) &&
                (GmatStringUtil::ToInteger(ordStr, fileOrder)))
            {
               lineStream.str(line.substr(14));
               lineStream >> noIdea >> tmpMu >> tmpA >> noClue;
               if (tmpMu != 0.0)
                  mu = tmpMu / 1.0e09;     // -> km^3/sec^2
               if (tmpA  != 0.0)
                  radius = tmpA / GmatMathConstants::KM_TO_M;  // -> km

               // if not reading coefficients, stop after reading the mu and a
               if (!readCoeff)
                  break;
            }
            else
            {
               throw GravityFileException
                  ("File \"" + filename + "\" has error in \n   \"" + line + "\"");
            }
         }
         else if (firstStr == "RECOEF")
         {
            nStr = line.substr(8, 3);
            mStr = line.substr(11, 3);
            cnmStr = line.substr(17, 21);
            lineStream.str(line.substr(38, 21));
            lineStream >> snmStr;
            snmStr = GmatStringUtil::Trim(snmStr);
            //MessageInterface::ShowMessage
            //   ("===> nStr=%s, mStr=%s, snmStr=<%s>\n",
            //    nStr.c_str(), mStr.c_str(), snmStr.c_str());
            if ((GmatStringUtil::ToInteger(nStr, n)) &&
                (GmatStringUtil::ToInteger(mStr, m)) &&
                (GmatStringUtil::ToReal(cnmStr, Cnm)) &&
                ((snmStr == "") ||
                 (GmatStringUtil::ToReal(snmStr, Snm))))
            {
               if ( n <= maxDegree && m <= maxOrder )
               {
                  cbar[n][m] = Cnm;
                  sbar[n][m] = Snm;
                  //MessageInterface::ShowMessage
                  //   ("   cbar[%d][%d]=% .12e\n   sbar[%d][%d]=% .12e\n",
                  //    n, m, cbar[n][m], n, m, sbar[n][m]);
               }
            }
            else
            {
               throw GravityFileException
                  ("File \"" + filename + "\" has error in \n   \"" + line + "\"");
            }

            snmStr = "";
            Snm = 0.0;
         }
      }
   }

   degree = fileDegree;
   order = fileOrder;

   // make sure mu and a are in KM and Km^3/sec^2 (they are in meters on the files)

   #ifdef DEBUG_GRAVITY_COF_FILE
   MessageInterface::ShowMessage("   \"%s\" successfully read\n", filename.c_str());
   MessageInterface::ShowMessage
      ("   degree=%d, order=%d, mu=%f, radius=%f\n", degree, order,
       mu, radius);
   MessageInterface::ShowMessage
      ("   last n=%d, m=%d, Cnm=%le, Snm=%le\n", n, m, Cnm, Snm);
   MessageInterface::ShowMessage
      ("   last nStr=%s, mStr=%s, cnmStr=%s, snmStr=%s\n",
       nStr.c_str(), mStr.c_str(), cnmStr.c_str(), snmStr.c_str());
   #endif

   inStream.close();
   return true;
}


//------------------------------------------------------------------------------
// bool ReadDatFile(const std::string &filename, Integer& degree, Integer& order,
//                  Real &mu, Real &radius, bool readCoeff, Real cbar[][361],
//                  Real sbar[][DEG_DIM], Integer maxDegree, Integer maxOrder,
//                  Integer maxDriftDegree)
//------------------------------------------------------------------------------
bool GravityFileUtil::ReadDatFile(const std::string &filename, Integer& degree,
                              Integer& order, Real &mu, Real &radius, bool readCoeff,
                              Real cbar[][361], Real sbar[][DEG_DIM],
                              Real dcbar[][DRF_DIM], Real dsbar[][DRF_DIM],
                              Integer maxDegree, Integer maxOrder, Integer maxDriftDegree)
{
   #ifdef DEBUG_GRAVITY_DAT_FILE
   MessageInterface::ShowMessage
      ("GravityFileUtil::ReadDatFile() filename=%s\n   maxDegree=%d, maxOrder=%d, "
       "maxDriftDegree=%d, readCoeff=%d\n", filename.c_str(), maxDegree, maxOrder,
       maxDriftDegree, readCoeff);
   #endif

   if (!readCoeff)
   {
      std::ifstream inStream(filename.c_str());
      if (!inStream)
         throw GravityFileException("Cannot open DAT gravity file \"" + filename + "\"");

      std::string line;
      while (!inStream.eof())
      {
         getline(inStream, line);

         // ignore comment lines
         if (line[0] != '#')
            break;
      }

      std::istringstream muStream(line);
      muStream >> mu;

      getline(inStream, line);
      std::istringstream raStream(line);
      raStream >> radius;

      mu = mu / 1.0e09;           // -> Km^3/sec^2
      radius  = radius / GmatMathConstants::KM_TO_M;  // -> Km

      while (!inStream.eof())
      {
         getline(inStream, line);
         if (line[0] != '#')
         {
            std::istringstream coefStream(line);
            coefStream >> degree >> order;
         }
      }

      #ifdef DEBUG_GRAVITY_DAT_FILE
      MessageInterface::ShowMessage
         ("GravityFileUtil::ReadDatFile() returning degree=%d, order=%d, mu=%le, "
          "radius=%le\n", degree, order, mu, radius);
      #endif

      inStream.close();
      return true;
   }


   //------------------------------------------------------------
   // read all information from file
   //------------------------------------------------------------
   FILE *fp = fopen( filename.c_str(), "r");
   if (!fp)
      throw GravityFileException("Cannot open DAT gravity file \"" + filename + "\"");

   char buf[GmatFile::MAX_LINE_LEN];
   Integer maxLen = GmatFile::MAX_LINE_LEN;
   Integer cc, dd, sz=0;
   Integer iscomment, rtn;
   Integer n=0, m=0;
   Integer fileDegree, fileOrder;
   Real    Cnm=0.0, Snm=0.0, dCnm=0.0, dSnm=0.0;

   //-------------------------------------------------------
   // read mu and radius
   //-------------------------------------------------------
   iscomment = 1;
   while ( iscomment )
   {
      rtn = fgetc( fp );
      if ( (char)rtn == '#' )
      {
         // Intentionally get the return and then ignore it to move warning from
         // system libraries to GMAT code base.  The "unused variable" warning
         // here can be safely ignored.
         char* ch = fgets( buf, maxLen, fp );
      }
      else
      {
         ungetc( rtn, fp );
         iscomment = 0;
      }
   }

   int len = fscanf(fp, "%lg\n", &mu ); mu = (Real)mu;
   len += fscanf(fp, "%lg\n", &radius ); radius = (Real)radius;
   radius  = radius / GmatMathConstants::KM_TO_M;  // -> Km
   mu = mu / 1.0e09;           // -> Km^3/sec^2

   fileDegree = 0;
   fileOrder  = 0;
   cc=0; n=0; m=0;

   // Check maxDegree, maxOrder, maxDriftDegree
   if (maxDegree <= 0 || maxDegree > 360)
   {
      GravityFileException ue;
      ue.SetDetails("Invalid MAX Degree passed %d", maxDegree);
      MessageInterface::ShowMessage
         ("**** ERROR **** Invalid MAX Degree passed %d\n", maxDegree);
      throw ue;
   }

   if (maxOrder <= 0 || maxOrder > 360)
   {
      fclose(fp);
      
      GravityFileException ue;
      ue.SetDetails("Invalid MAX Order passed %d", maxOrder);
      MessageInterface::ShowMessage
         ("**** ERROR **** Invalid MAX Order passed %d\n", maxOrder);
      throw ue;
   }

   if (maxDriftDegree <= 0 || maxDriftDegree > 2)
   {
      fclose(fp);
      
      GravityFileException ue;
      ue.SetDetails("Invalid MAX Drift Degree passed %d", maxDriftDegree);
      MessageInterface::ShowMessage
         ("**** ERROR **** Invalid MAX Drift Order passed %d\n", maxDriftDegree);
      throw ue;
   }


   for (cc = 2; cc <= maxDegree; ++cc)
      for (dd = 0; dd <= cc; ++dd)
         sz++;

   //-------------------------------------------------------
   // read coefficient drift rate
   //-------------------------------------------------------
   // The return here is intentionally ignored.  The compiler may throw a
   // warning for this line that can be safely ignored.
   char* ch = fgets( buf, maxLen, fp );
   while ( ( (char)(rtn=fgetc(fp)) != '#' ) && (rtn != EOF) )
   {
      ungetc( rtn, fp );
      // Intentionally get the return and then ignore it to move warning from
      // system libraries to GMAT code base.  The "unused variable" warning
      // here can be safely ignored.
      int len = fscanf( fp, "%i %i %le %le\n", &n, &m, &dCnm, &dSnm );
      if ( n <= maxDriftDegree  && m <= n )
      {
         dcbar[n][m] = (Real)dCnm;
         dsbar[n][m] = (Real)dSnm;
      }
   }

   //-------------------------------------------------------
   // read coefficients and store
   //-------------------------------------------------------
   ch = fgets( buf, maxLen, fp );

   fileDegree = 0;
   fileOrder  = 0;
   cc=0; n=0; m=0;
   do
   {
      if ( n <= maxDegree && m <= maxOrder )
      {
         cbar[n][m] = (Real)Cnm;
         sbar[n][m] = (Real)Snm;
      }
      if (n > fileDegree) fileDegree = n;
      if (m > fileOrder)  fileOrder  = m;

      cc++;
   } while ( ( cc<=sz ) && ( fscanf( fp, "%i %i %le %le\n", &n, &m, &Cnm, &Snm ) > 0 ));

   degree = fileDegree;
   order = fileOrder;

   #ifdef DEBUG_GRAVITY_DAT_FILE
   MessageInterface::ShowMessage("Leaving GravityFileUtil::ReadDatFile\n");
   MessageInterface::ShowMessage
      ("    degree = %d, order = %d, mu = %.4f, radius = %.4f\n", degree, order,
       mu, radius);
   MessageInterface::ShowMessage
      ("    cbar[2][0] = % .12e    sbar[2][0] = %.12e\n", cbar[2][0], sbar[2][0]);
   MessageInterface::ShowMessage
      ("    cbar[2][1] = % .12e    sbar[2][1] = %.12e\n", cbar[2][1], sbar[2][1]);
   MessageInterface::ShowMessage
      ("   dcbar[2][0] = % .12e   dsbar[2][0] = %.12e\n", dcbar[2][0], dsbar[2][0]);
   MessageInterface::ShowMessage
      ("   dcbar[2][1] = % .12e   dsbar[2][1] = %.12e\n", dcbar[2][1], dsbar[2][1]);
   #endif

   fclose(fp);
   return true;
}


//------------------------------------------------------------------------------
// bool ReadGrvFile(const std::string &filename, Integer& degree, Integer& order,
//                  Real &mu, Real &radius, bool readCoeff, Real cbar[][361],
//                  Real sbar[][DEG_DIM], Integer maxDegree, Integer maxOrder,
//                  Integer maxDriftDegree = 0)
//------------------------------------------------------------------------------
bool GravityFileUtil::ReadGrvFile(const std::string &filename, Integer& degree,
                              Integer& order, Real &mu, Real &radius, bool readCoeff,
                              Real cbar[][361], Real sbar[][DEG_DIM], Integer maxDegree,
                              Integer maxOrder, Integer maxDriftDegree)
{
   std::ifstream inStream(filename.c_str());
   if (!inStream)
      throw GravityFileException("Cannot open GRV gravity file \"" + filename + "\"");

   Integer       n, m;
   Integer       fileOrder = -1, fileDegree = -1;
   Real          Cnm = 0.0, Snm = 0.0;
   Real          tmpMu = 0.0;
   Real          tmpA  = 0.0;
   std::string   isNormalized = "";

   #ifdef DEBUG_GRAVITY_GRV_FILE
   MessageInterface::ShowMessage("Entered GravityFileUtil::ReadGrvFile\n");
   #endif

   std::string line;
   std::string firstStr;

   // Read header line
   getline(inStream, line);

   while (!inStream.eof())
   {
      getline(inStream, line);

      //MessageInterface::ShowMessage("=> line=<%s>\n", line.c_str());
      if (line == "")
         continue;

      std::istringstream lineStream;
      lineStream.str(line);

      // ignore comment lines
      if (line[0] != '#')
      {
         lineStream >> firstStr;
         if (firstStr == "END") break;

         std::string upperString = GmatStringUtil::ToUpper(firstStr);

         // ignore the stk version and blank lines
         if ((upperString == "MODEL") ||
             (upperString =="BEGIN"))
         {
            // do nothing - we don't need to know this
         }
         else if (upperString == "DEGREE")
         {
            lineStream >> fileDegree;
         }
         else if (upperString == "ORDER")
         {
            lineStream >> fileOrder;
         }
         else if (upperString == "GM")
         {
            lineStream >> tmpMu;
            if (tmpMu != 0.0)
               mu = tmpMu / 1.0e09;     // -> Km^3/sec^2
         }
         else if (upperString == "REFDISTANCE")
         {
            lineStream >> tmpA;
            if (tmpA != 0.0)
               radius = tmpA / GmatMathConstants::KM_TO_M;  // -> Km
         }
         else if (upperString == "NORMALIZED")
         {
            lineStream >> isNormalized;
            if (isNormalized == "No")
               throw GravityFileException("File " + filename + " is not normalized.");
         }
         else
         {
            // We can stop here if we don't read coefficients
            if (!readCoeff)
               break;

            // Ensure that m and n fall in the allowed ranges
            n = (Integer) atoi(firstStr.c_str());
            if ((n > 0) && (n < maxDegree))
            {
               lineStream >> m;
               if ((m >= 0) && (m <= n))
               {
                  lineStream >> Cnm >> Snm;
                  cbar[n][m] = (Real)Cnm;
                  sbar[n][m] = (Real)Snm;
               }
            }
         }
      }
   }

   degree = fileDegree;
   order = fileOrder;

   inStream.close();

   #ifdef DEBUG_GRAVITY_GRV_FILE
   if (readCoeff)
   {
      MessageInterface::ShowMessage("Leaving GravityFileUtil::ReadGrvFile\n");
      MessageInterface::ShowMessage("   cbar[ 2][ 0] = %le   sbar[ 2][ 0] = %le   \n",
                                    cbar[2][0], sbar[2][0]);
      MessageInterface::ShowMessage("   cbar[20][20] = %le   sbar[20][20] = %le   \n",
                                    cbar[20][20], sbar[20][20]);
   }
   #endif

   return true;
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GravityFileUtil()
//------------------------------------------------------------------------------
GravityFileUtil::GravityFileUtil()
{
}


//------------------------------------------------------------------------------
// GravityFileUtil(const GravityFileUtil &copy)
//------------------------------------------------------------------------------
GravityFileUtil::GravityFileUtil(const GravityFileUtil &copy)
{
}


//------------------------------------------------------------------------------
// const GravityFileUtil& operator=(const GravityFileUtil &right)
//------------------------------------------------------------------------------
const GravityFileUtil& GravityFileUtil::operator=(const GravityFileUtil &right)
{
   return *this;
}


//------------------------------------------------------------------------------
// ~GravityFileUtil()
//------------------------------------------------------------------------------
GravityFileUtil::~GravityFileUtil()
{
}

