//$Id$
//------------------------------------------------------------------------------
//                              SpiceOrbitKernelReader
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under
// FDSS Task order 28.
//
// Author: Wendy C. Shoan
// Created: 2010.04.05
//
/**
 * Definition of the SpiceOrbitKernelReader, which reads SPICE SPK (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to read the specified
 * SPICE file(s) and return the requested data.
 */
//------------------------------------------------------------------------------
#include "SpiceOrbitKernelReader.hpp"
#include "gmatdefs.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "A1Mjd.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"
#include "UtilityException.hpp"
#include "ICRFFile.hpp"
#include <sstream>                 // for stringstream
#include <map>

//#define DEBUG_SPK_READING
//#define DEBUG_SPK_COVERAGE
//#define DEBUG_SPK_PLANETS

// -----------------------------------------------------------------------------
// static data
// -----------------------------------------------------------------------------
// none


// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  SpiceOrbitKernelReader()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpiceOrbitKernelReader class
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
SpiceOrbitKernelReader::SpiceOrbitKernelReader() :
   SpiceKernelReader       (),
   observingBodyNameSPICE  (NULL),
   aberrationSPICE         (NULL),
   spiceErrorMSG           ({})
{
   gapsFound = 0;
}

//------------------------------------------------------------------------------
//  SpiceOrbitKernelReader(const SpiceKernelReader &reader)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpiceOrbitKernelReader class, by copying
 * the input object.
 * (copy constructor).
 *
* @param <reader> SpiceOrbitKernelReader object to copy.
  */
//------------------------------------------------------------------------------
SpiceOrbitKernelReader::SpiceOrbitKernelReader(const SpiceOrbitKernelReader &reader) :
    SpiceKernelReader       (reader),
	observingBodyNameSPICE  (NULL),
	aberrationSPICE         (NULL),
	spiceErrorMSG           ({})
{
   gapsFound = reader.gapsFound;
}

//------------------------------------------------------------------------------
//  SpiceOrbitKernelReader& operator=(const SpiceOrbitKernelReader &reader)
//------------------------------------------------------------------------------
/**
 * This method copies the data from the input object to the object.
 *
 * @param <reader> the SpiceOrbitKernelReader object whose data to assign to "this"
 *                 SpiceOrbitKernelReader.
 *
 * @return "this" SpiceOrbitKernelReader with data of input SpiceOrbitKernelReader
 *         reader.
 */
//------------------------------------------------------------------------------
SpiceOrbitKernelReader& SpiceOrbitKernelReader::operator=(const SpiceOrbitKernelReader &reader)
{
   if (&reader == this)
      return *this;

   SpiceKernelReader::operator=(reader);

   observingBodyNameSPICE   = NULL;
   aberrationSPICE          = NULL;
   spiceErrorMSG            = {};
   gapsFound                = reader.gapsFound;
   return *this;
}

//------------------------------------------------------------------------------
// ~SpiceOrbitKernelReader()
//------------------------------------------------------------------------------
/**
 * This method is the destructor for the SpiceOrbitKernelReader.
 *
 */
//------------------------------------------------------------------------------
SpiceOrbitKernelReader::~SpiceOrbitKernelReader()
{

}

//------------------------------------------------------------------------------
//  SpiceOrbitKernelReader* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method clones the object.
 *
 * @return new object, cloned from "this" object.
 *
 */
//------------------------------------------------------------------------------
//SpiceOrbitKernelReader* SpiceOrbitKernelReader::Clone(void) const
SpiceInterface* SpiceOrbitKernelReader::Clone(void) const
{
   SpiceOrbitKernelReader * clonedSKR = new SpiceOrbitKernelReader(*this);

   return clonedSKR;
}

//------------------------------------------------------------------------------
// void GetCoverageStartAndEnd(const StringArray &kernels, Integer forNaifId.
//                             Real              &start,   Real    &end)
//------------------------------------------------------------------------------
/**
 * This (pure virtual) method determines the coverage for the specified object
 * over the specified kernels.
 *
 * @param kernels   the array of kernels over which to check the coverage
 * @param forNaifId the NAIF ID of the object for which coverage should
 *                  be determined
 * @param start     (output) the earliest time of coverage for the object
 *                  included in the specified kernels
 * @param end       (output) the latest time of coverage for the object
 *                  included in the specified kernels
 *
 * @note An exception is thrown if any of the kernels listed are not currently
 *       loaded into the kernel pool, and an attempt to load it fails.
 * @note The interval between the returned start and end times is not
 *       necessarily continuous.  The method checks all intervals over which
 *       there is coverage for the specified object and returns the earliest
 *       time and the latest time of coverage.  There could be gaps in
 *       coverage over that span.
 * @note The method will ignore kernels of types not containing the type of
 *       data requested, i.e. this method will ignore kernels in the list that
 *       are not of type 'spk'.
 * @note Celestial bodies must remember to include the main Solar System SPK
 *       kernel in the list of kernels passed in to this method.
 */
//------------------------------------------------------------------------------
void  SpiceOrbitKernelReader::GetCoverageStartAndEnd(StringArray       &kernels,
                                                     Integer           forNaifId,
                                                     Real              &start,
                                                     Real              &end)
{
   #ifdef DEBUG_SPK_COVERAGE
      MessageInterface::ShowMessage("Entering GetCoverageStartAndEnd:\n");
      MessageInterface::ShowMessage("   forNaifId = %d\n", forNaifId);
      MessageInterface::ShowMessage("   kernels are:\n");
      if (kernels.empty())  MessageInterface::ShowMessage("   EMPTY!!!!\n");
      else
      {
         for (unsigned int ii = 0; ii < kernels.size(); ii++)
            MessageInterface::ShowMessage("   %d    %s\n", (Integer) ii, kernels.at(ii).c_str());
      }
   #endif
   // first check to see if a kernel specified is not loaded; if not,
   // try to load it
   for (unsigned int ii = 0; ii < kernels.size(); ii++)
      if (!IsLoaded(kernels.at(ii)))   LoadKernel(kernels.at(ii));

   SpiceInt         idSpice     = forNaifId;
   SpiceInt         arclen      = 4;
   SpiceInt         typlen      = 5;
   bool             firstInt    = true;
   bool             idOnKernel  = false;
   ConstSpiceChar   *kernelName = NULL;
   SpiceInt         objId       = 0;
   SpiceInt         numInt      = 0;
   SpiceChar        *kernelType;
   SpiceChar        *arch;
   SpiceDouble      b;
   SpiceDouble      e;
   Real             bA1;
   Real             eA1;
   SPICEINT_CELL(ids, 200);
   SPICEDOUBLE_CELL(cover, 200000);
   char             kStr[5] = "    ";
   char             aStr[4] = "   ";

   // look through each kernel
   for (unsigned int ii = 0; ii < kernels.size(); ii++)
   {
      #ifdef DEBUG_SPK_COVERAGE
         MessageInterface::ShowMessage("Checking coverage for ID %d on kernel %s\n",
               forNaifId, (kernels.at(ii)).c_str());
      #endif
      // SPICE expects forward slashes for directory separators
//      std::string kName = GmatStringUtil::Replace(kernels.at(ii), "\\", "/");
      std::string kName = GetFullKernelPath(kernels.at(ii));
      #ifdef DEBUG_SPK_COVERAGE
         MessageInterface::ShowMessage("--- Setting kernel name to %s\n", kName.c_str());
      #endif
      kernelName = kName.c_str();
      // check the type of kernel
      arch        = aStr;
      kernelType  = kStr;
      getfat_c(kernelName, arclen, typlen, arch, kernelType);
      if (failed_c())
      {
         ConstSpiceChar option[] = "LONG";
         SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
         //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
         SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
         getmsg_c(option, numChar, err);
         std::string errStr(err);
         std::string errmsg = "Error determining type of kernel \"";
         errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: [";
         errmsg += errStr + "]\n";
         reset_c();
         delete [] err;
         throw UtilityException(errmsg);
      }
      #ifdef DEBUG_SPK_COVERAGE
         MessageInterface::ShowMessage("Kernel is of type %s\n",
               kernelType);
      #endif
      // only deal with SPK kernels
      if (eqstr_c( kernelType, "spk" ))
      {
         spkobj_c(kernelName, &ids);
         // get the list of objects (IDs) for which data exists in the SPK kernel
         for (SpiceInt jj = 0;  jj < card_c(&ids);  jj++)
         {
            objId = SPICE_CELL_ELEM_I(&ids,jj);
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("Kernel contains data for object %d\n",
                     (Integer) objId);
            #endif
            // look to see if this kernel contains data for the object we're interested in
            if (objId == idSpice)
            {
               idOnKernel = true;
               break;
            }
         }
         // only deal with kernels containing data for the object we're interested in
         if (idOnKernel)
         {
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("Checking kernel %s for data for object %d\n",
                     (kernels.at(ii)).c_str(), (Integer) objId);
            #endif
            scard_c(0, &cover);   // reset the coverage cell
            spkcov_c (kernelName, idSpice, &cover);
            if (failed_c())
            {
               ConstSpiceChar option[] = "LONG";
               SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
               //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
               SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
               getmsg_c(option, numChar, err);
               std::string errStr(err);
               std::string errmsg = "Error determining coverage for SPK kernel \"";
               errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: [";
               errmsg += errStr + "]\n";
               reset_c();
               delete [] err;
               throw UtilityException(errmsg);
            }
            numInt = wncard_c(&cover);
            #ifdef DEBUG_SPK_COVERAGE
               MessageInterface::ShowMessage("Number of intervals found =  %d\n",
                     (Integer) numInt);
            #endif
            if ((firstInt) && (numInt > 0))
            {
               wnfetd_c(&cover, 0, &b, &e);
               if (failed_c())
               {
                  ConstSpiceChar option[] = "LONG";
                  SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
                  //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
                  SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
                  getmsg_c(option, numChar, err);
                  std::string errStr(err);
                  std::string errmsg = "Error getting interval times for SPK kernel \"";
                  errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: [";
                  errmsg += errStr + "]\n";
                  reset_c();
                  delete [] err;
                  throw UtilityException(errmsg);
               }
               start    = SpiceTimeToA1(b);
               end      = SpiceTimeToA1(e);
               firstInt = false;
            }
            for (SpiceInt jj = 0; jj < numInt; jj++)
            {
               wnfetd_c(&cover, jj, &b, &e);
               bA1 = SpiceTimeToA1(b);
               eA1 = SpiceTimeToA1(e);
               if (bA1 < start)  start = bA1;
               if (eA1 > end)    end   = eA1;
            }
         }

      }
   }
   if (firstInt)
   {
      std::stringstream errmsg("");
      errmsg << "Error - no data available for body with NAIF ID " << forNaifId << " on specified SPK kernels\n";
      throw UtilityException(errmsg.str());
   }
   #ifdef DEBUG_SPK_COVERAGE
      else
      {
         MessageInterface::ShowMessage("Number of intervals found for body with NAIF ID %d =  %d\n",
               forNaifId, (Integer) numInt);
      }
   #endif
}


//------------------------------------------------------------------------------
// void GetCoverageSpans(const StringArray &kernels, Integer forNaifId,
//                       RealArray &start, RealArray &end)
//------------------------------------------------------------------------------
/**
 * This method determines the coverage for the specified object over the
 * specified kernels.
 *
 * @param kernels   the array of kernels over which to check the coverage
 * @param forNaifId the NAIF ID of the object for which coverage should
 *                  be determined
 * @param start     (output) the starting time of coverage for the time
 *                  spans for the object in the specified kernels
 * @param end       (output) the end time time of coverage for the time
 *                  spans for the object in the specified kernels
 *
 * @note An exception is thrown if any of the kernels listed are not currently
 *       loaded into the kernel pool, and an attempt to load it fails.
 * @note The interval between the returned start and end times is not
 *       necessarily continuous.  The method checks all intervals over which
 *       there is coverage for the specified object and returns the start
 *       time and end time fro each interval of coverage.  There could be gaps
 *       in coverage over that span.
 * @note The method will ignore kernels of types not containing the type of
 *       data requested, i.e. this method will ignore kernels in the list that
 *       are not of type 'spk'.
 * @note Celestial bodies must remember to include the main Solar System SPK
 *       kernel in the list of kernels passed in to this method.
 */
//------------------------------------------------------------------------------
void  SpiceOrbitKernelReader::GetCoverageSpans(StringArray       &kernels,
                                               Integer           forNaifId,
                                               RealArray         &start,
                                               RealArray         &end)
{
   // first check to see if a kernel specified is not loaded; if not,
   // try to load it
   for (unsigned int ii = 0; ii < kernels.size(); ii++)
      if (!IsLoaded(kernels.at(ii)))
         LoadKernel(kernels.at(ii));

   SpiceInt         idSpice     = forNaifId;
   SpiceInt         arclen      = 4;
   SpiceInt         typlen      = 5;
   bool             idOnKernel  = false;
   ConstSpiceChar   *kernelName = NULL;
   SpiceInt         objId       = 0;
   SpiceInt         numInt      = 0;
   SpiceChar        *kernelType;
   SpiceChar        *arch;
   SpiceDouble      b;
   SpiceDouble      e;
   Real             bA1;
   Real             eA1;
   SPICEINT_CELL(ids, 200);
   SPICEDOUBLE_CELL(cover, 200000);
   char             kStr[5] = "    ";
   char             aStr[4] = "   ";

   start.clear();
   end.clear();

   // look through each kernel
   for (unsigned int ii = 0; ii < kernels.size(); ii++)
   {
      std::string kName = GetFullKernelPath(kernels.at(ii));
      kernelName = kName.c_str();
      // check the type of kernel
      arch        = aStr;
      kernelType  = kStr;
      getfat_c(kernelName, arclen, typlen, arch, kernelType);
      if (failed_c())
      {
         ConstSpiceChar option[] = "LONG";
         SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
         SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
         getmsg_c(option, numChar, err);
         std::string errStr(err);
         std::string errmsg = "Error determining type of kernel \"";
         errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: [";
         errmsg += errStr + "]\n";
         reset_c();
         delete [] err;
         throw UtilityException(errmsg);
      }

      // only deal with SPK kernels
      if (eqstr_c( kernelType, "spk" ))
      {
         spkobj_c(kernelName, &ids);
         // get the list of objects (IDs) for which data exists in the SPK kernel
         for (SpiceInt jj = 0;  jj < card_c(&ids);  jj++)
         {
            objId = SPICE_CELL_ELEM_I(&ids,jj);
            // look to see if this kernel contains data for the object we're interested in
            if (objId == idSpice)
            {
               idOnKernel = true;
               break;
            }
         }

         // only deal with kernels containing data for the object we're interested in
         if (idOnKernel)
         {
            scard_c(0, &cover);   // reset the coverage cell
            spkcov_c (kernelName, idSpice, &cover);

            if (failed_c())
            {
               ConstSpiceChar option[] = "LONG";
               SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
               //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
               SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
               getmsg_c(option, numChar, err);
               std::string errStr(err);
               std::string errmsg = "Error determining coverage for SPK kernel \"";
               errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: [";
               errmsg += errStr + "]\n";
               reset_c();
               delete [] err;
               throw UtilityException(errmsg);
            }

            numInt = wncard_c(&cover);

            if (numInt > 0)
            {
               for (SpiceInt i = 0; i < numInt; ++i)
               {
                  wnfetd_c(&cover, i, &b, &e);
                  if (failed_c())
                  {
                     ConstSpiceChar option[] = "LONG";
                     SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
                     SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
                     getmsg_c(option, numChar, err);
                     std::string errStr(err);
                     std::string errmsg = "Error getting interval times for SPK kernel \"";
                     errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: [";
                     errmsg += errStr + "]\n";
                     reset_c();
                     delete [] err;
                     throw UtilityException(errmsg);
                  }
                  start.push_back(SpiceTimeToA1(b));
                  end.push_back(SpiceTimeToA1(e));
               }
            }
         }

      }
   }
   if (start.size() == 0)
   {
      std::stringstream errmsg("");
      errmsg << "Error - no data available for body with NAIF ID " << forNaifId << " on specified SPK kernels\n";
      throw UtilityException(errmsg.str());
   }

   #ifdef DEBUG_SPK_SPANS
      MessageInterface::ShowMessage("Number of intervals found for body with NAIF ID %d =  %d\n",
            forNaifId, (Integer) numInt);
   #endif
}


//------------------------------------------------------------------------------
//  Rvector6 GetTargetState(const std::string &targetName,
//                          const A1Mjd       &atTime,
//                          const std::string &observingBodyName,
//                          const Integer     observingBodyNAIFId,
//                          const std::string &referenceFrame,
//                          const std::string &aberration)
//------------------------------------------------------------------------------
/**
 * This method returns the state of the target with respect to the observing body
 * at the input time.
 *
 * @param <targetName>        name of the target object.
 * @param <targetNAIFId>      NAIF ID of the target body
 * @param <atTime>            time at which the state is requested.
 * @param <observingBodyName> name of the observing body
 * @param <observingBodyNAIFId> NAIF ID of the observing body
 * @param <referenceFrame>    frame in which state should be returned
 * @param <aberration>        flag indicating aberration corrections, if any
 *
 * @return state at the input time
 *
 */
//------------------------------------------------------------------------------
Rvector6 SpiceOrbitKernelReader::GetTargetState(const std::string &targetName,
                                 const Integer     targetNAIFId,
                                 const A1Mjd       &atTime,
                                 const std::string &observingBodyName,
                                 const Integer     observingBodyNAIFId,
                                 const std::string &referenceFrame,
                                 const std::string &aberration)
{
   #ifdef DEBUG_SPK_READING
      MessageInterface::ShowMessage(
            "Entering SPKReader::GetTargetState with target = %s, naifId = %d, time = %12.10f, observer = %s, aberration = %s\n",
            targetName.c_str(), targetNAIFId, atTime.Get(), observingBodyName.c_str(), aberration.c_str());
//      Real start, end;
//      GetCoverageStartAndEnd(loadedKernels, targetNAIFId, start, end);
//      MessageInterface::ShowMessage("   coverage for object %s : %12.10f --> %12.10f\n",
//            targetName.c_str(), start, end);
   #endif
   std::string targetNameToUse = GmatStringUtil::ToUpper(targetName);
   if (targetNameToUse == "LUNA")  // We use Luna, instead of Moon, for GMAT
      targetNameToUse        = "MOON";
   if (targetNameToUse == "SOLARSYSTEMBARYCENTER")
      targetNameToUse = "SSB";
   objectNameSPICE           = targetNameToUse.c_str();
   observingBodyNameSPICE    = observingBodyName.c_str();
   referenceFrameSPICE       = referenceFrame.c_str();
   aberrationSPICE           = aberration.c_str();
   // convert time to Ephemeris Time (TDB)
   etSPICE                   = A1ToSpiceTime(atTime.Get());
   naifIDSPICE               = targetNAIFId;
   observerNaifIDSPICE       = observingBodyNAIFId;
//   boddef_c(objectNameSPICE, naifIDSPICE);        // CSPICE method to set NAIF ID for an object

   #ifdef DEBUG_SPK_READING
//      MessageInterface::ShowMessage("SET NAIF Id for object %s to %d\n",
//            targetNameToUse.c_str(), targetNAIFId);
//      MessageInterface::ShowMessage(
//            "In SPKReader::Converted (to TBD) time = %12.10f\n", etMjdAtTime);
//      MessageInterface::ShowMessage("  then the full JD = %12.10f\n",
//            (etMjdAtTime + GmatTimeConstants::JD_JAN_5_1941));
      MessageInterface::ShowMessage("So time passed to SPICE is %12.14f\n", (Real) etSPICE);
   #endif
   SpiceDouble state[6];
   SpiceDouble oneWayLightTime;
   if (aberration == "NONE")
   {
      spkgeo_c(naifIDSPICE, etSPICE, referenceFrameSPICE, observerNaifIDSPICE, state, &oneWayLightTime);
      #ifdef DEBUG_SPK_READING
         MessageInterface::ShowMessage(
               "In SPKReader::Called spkgeo_c with ID = %d and time = %12.10f, and got state out: %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
               (Integer) naifIDSPICE, (double) etSPICE, state[0], state[1], state[2], state[3], state[4], state[5]);
      #endif
   }
   else
   {
      spkezr_c(objectNameSPICE, etSPICE, referenceFrameSPICE, aberrationSPICE,
               observingBodyNameSPICE, state, &oneWayLightTime);
      #ifdef DEBUG_SPK_READING
         MessageInterface::ShowMessage(
               "In SPKReader::Called spkezr_c and got state out: %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
               state[0], state[1], state[2], state[3], state[4], state[5]);
      #endif
   }

#ifdef DEBUG_SPK_PLANETS
   Real        ttMjdAtTime   = TimeConverterUtil::Convert(atTime.Get(), TimeConverterUtil::A1MJD,
                               TimeConverterUtil::TTMJD, GmatTimeConstants::JD_JAN_5_1941);
//   Real etJd                 = etMjdAtTime + GmatTimeConstants::JD_JAN_5_1941;
   Real ttJd                 = ttMjdAtTime + GmatTimeConstants::JD_JAN_5_1941;
   MessageInterface::ShowMessage("Asking CSPICE for state of body %s, with observer %s, referenceFrame %s, and aberration correction %s\n",
         objectNameSPICE, observingBodyNameSPICE, referenceFrameSPICE, aberrationSPICE);
//   MessageInterface::ShowMessage(
//         "           Body: %s   TT Time:  %12.10f  TDB Time: %12.10f   state:  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
//         targetName.c_str(), ttJd, etJd, state[0], state[1], state[2], state[3], state[4], state[5]);
   MessageInterface::ShowMessage(
         "           Body: %s   TT Time:  %12.10f   state:  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
         targetName.c_str(), ttJd,  state[0], state[1], state[2], state[3], state[4], state[5]);
#endif
   bool failed = false;                                                        
   if (failed_c())
   {
      StoreErrorMessage(targetName);

      if(!CheckIfInCoverageRange(targetNAIFId, atTime))
         PrintErrorMessage(targetName);
      
      failed = true;                                                           
   }

   else if (spiceErrorMSG.size() != 0)
	   PrintWarningMessage(targetName);

   Rvector6 r6;
   if (referenceFrame == "J2000")
   {
      //Rotates the state from SPICEs J2000 (ICRF) to GMATs MJ2000Eq
      ICRFFile* icrfFile = ICRFFile::Instance();
      icrfFile->Initialize();

      Real outState[6];
      icrfFile->RotateFromICRFtoFK5(state, outState, atTime.Get());

      r6.Set(outState);
   }
   else
   {
      r6.Set(state);
   }

   if(failed)
      r6 = { -GmatRealConstants::REAL_MAX, -GmatRealConstants::REAL_MAX, -GmatRealConstants::REAL_MAX, -GmatRealConstants::REAL_MAX, -GmatRealConstants::REAL_MAX, -GmatRealConstants::REAL_MAX };

   #ifdef DEBUG_SPK_READING
      MessageInterface::ShowMessage(
            "In SPKReader:: returning state: %s\n", r6.ToString().c_str());
   #endif
   return r6;
}

//------------------------------------------------------------------------------
//  Rvector6 GetTargetState(const std::string &targetName,
//                          const GmatTime    &atTime,
//                          const std::string &observingBodyName,
//                          const Integer     observingBodyNAIFId,
//                          const std::string &referenceFrame,
//                          const std::string &aberration)
//------------------------------------------------------------------------------
/**
 * This method returns the state of the target with respect to the observing body
 * at the input time. This method differs from the one above as it uses GmatTime instead of A1Mjd
 *
 * @param <targetName>        name of the target object.
 * @param <targetNAIFId>      NAIF ID of the target body
 * @param <atTime>            time at which the state is requested.
 * @param <observingBodyName> name of the observing body
 * @param <observingBodyNAIFId> NAIF ID of the observing body
 * @param <referenceFrame>    frame in which state should be returned
 * @param <aberration>        flag indicating aberration corrections, if any
 *
 * @return state at the input time
 *
 */
//------------------------------------------------------------------------------
Rvector6 SpiceOrbitKernelReader::GetTargetState(const std::string &targetName,
   const Integer     targetNAIFId,
   const GmatTime    &atTime,
   const std::string &observingBodyName,
   const Integer     observingBodyNAIFId,
   const std::string &referenceFrame,
   const std::string &aberration)
{
#ifdef DEBUG_SPK_READING
   MessageInterface::ShowMessage(
      "Entering SPKReader::GetTargetState with target = %s, naifId = %d, time = %s, observer = %s, aberration = %s\n",
      targetName.c_str(), targetNAIFId, GmatTime(atTime).ToString().c_str(), observingBodyName.c_str(), aberration.c_str());
#endif
   std::string targetNameToUse = GmatStringUtil::ToUpper(targetName);
   if (targetNameToUse == "LUNA")  // We use Luna, instead of Moon, for GMAT
      targetNameToUse = "MOON";
   if (targetNameToUse == "SOLARSYSTEMBARYCENTER")
      targetNameToUse = "SSB";
   objectNameSPICE = targetNameToUse.c_str();
   observingBodyNameSPICE = observingBodyName.c_str();
   referenceFrameSPICE = referenceFrame.c_str();
   aberrationSPICE = aberration.c_str();
   // convert time to Ephemeris Time (TDB)
   //@todo: it needs to improve this code. Current SPICE lib cannot handle GmatTime
   etSPICE = A1ToSpiceTime(GmatTime(atTime).GetMjd());

   naifIDSPICE = targetNAIFId;
   observerNaifIDSPICE = observingBodyNAIFId;
   //   boddef_c(objectNameSPICE, naifIDSPICE);        // CSPICE method to set NAIF ID for an object

#ifdef DEBUG_SPK_READING
   //      MessageInterface::ShowMessage("SET NAIF Id for object %s to %d\n",
   //            targetNameToUse.c_str(), targetNAIFId);
   //      MessageInterface::ShowMessage(
   //            "In SPKReader::Converted (to TBD) time = %12.10f\n", etMjdAtTime);
   //      MessageInterface::ShowMessage("  then the full JD = %12.10f\n",
   //            (etMjdAtTime + GmatTimeConstants::JD_JAN_5_1941));
   MessageInterface::ShowMessage("So time passed to SPICE is %12.14f\n", (Real)etSPICE);
#endif
   SpiceDouble state[6];
   SpiceDouble oneWayLightTime;
   if (aberration == "NONE")
   {
      spkgeo_c(naifIDSPICE, etSPICE, referenceFrameSPICE, observerNaifIDSPICE, state, &oneWayLightTime);
#ifdef DEBUG_SPK_READING
      MessageInterface::ShowMessage(
         "In SPKReader::Called spkgeo_c with ID = %d and time = %12.10f, and got state out: %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
         (Integer)naifIDSPICE, (double)etSPICE, state[0], state[1], state[2], state[3], state[4], state[5]);
#endif
   }
   else
   {
      spkezr_c(objectNameSPICE, etSPICE, referenceFrameSPICE, aberrationSPICE,
         observingBodyNameSPICE, state, &oneWayLightTime);
#ifdef DEBUG_SPK_READING
      MessageInterface::ShowMessage(
         "In SPKReader::Called spkezr_c and got state out: %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
         state[0], state[1], state[2], state[3], state[4], state[5]);
#endif
   }

#ifdef DEBUG_SPK_PLANETS
   GmatTime ttMjdAtTime = TimeConverterUtil::Convert(atTime, TimeConverterUtil::A1MJD,
      TimeConverterUtil::TTMJD, GmatTimeConstants::JD_JAN_5_1941);
   GmatTime ttJd = ttMjdAtTime + GmatTimeConstants::JD_JAN_5_1941;
   MessageInterface::ShowMessage("Asking CSPICE for state of body %s, with observer %s, referenceFrame %s, and aberration correction %s\n",
      objectNameSPICE, observingBodyNameSPICE, referenceFrameSPICE, aberrationSPICE);
   MessageInterface::ShowMessage(
      "           Body: %s   TT Time:  %s   state:  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
      targetName.c_str(), ttJd.ToString().c_str(), state[0], state[1], state[2], state[3], state[4], state[5]);
#endif

   bool failed = false;                                   
   if (failed_c())
   {
      StoreErrorMessage(targetName);

      if (!CheckIfInCoverageRange(targetNAIFId, atTime.GetMjd()))
         PrintErrorMessage(targetName);
      
      failed = true;                                          
   }

   else if (spiceErrorMSG.size() != 0)
	   PrintWarningMessage(targetName);

   Rvector6 r6;
   if (referenceFrame == "J2000")
   {
      //Rotates the state from SPICEs J2000 (ICRF) to GMATs MJ2000Eq
      ICRFFile* icrfFile = ICRFFile::Instance();
      icrfFile->Initialize();

      Real outState[6];
      icrfFile->RotateFromICRFtoFK5(state, outState, atTime.GetMjd());

      r6.Set(outState);
   }
   else
   {
      r6.Set(state);
   }


   if (failed)
      r6 = { -GmatRealConstants::REAL_MAX, -GmatRealConstants::REAL_MAX, -GmatRealConstants::REAL_MAX, -GmatRealConstants::REAL_MAX, -GmatRealConstants::REAL_MAX, -GmatRealConstants::REAL_MAX };

#ifdef DEBUG_SPK_READING
   MessageInterface::ShowMessage(
      "In SPKReader:: returning state: %s\n", r6.ToString().c_str());
#endif
   return r6;
}


//------------------------------------------------------------------------------
//  Rmatrix66 GetRotationMatrix(std::string fromFrame,  )
//                              std::string toFrame,
//                              const GmatTime & atTime)
//------------------------------------------------------------------------------
/**
 * This method returns a 6x6 rotation matrix from the coordinate system fromFrame 
 * to the system toFrame at epoch atTime usng the spice call sxform_c
 *
 * @param fromFrame         frame to transform from
 * @param toFrame           frame to transform into
 * @param atTime            time at which the rotation matrix is requested.
 *
 * @return Rmatrix66 rotation matrix
 *
 */
 //------------------------------------------------------------------------------
 Rmatrix66 SpiceOrbitKernelReader::GetRotationMatrix(std::string fromFrame, std::string toFrame, const GmatTime & atTime)
 {
    Rmatrix66 retval = Rmatrix66();

    ConstSpiceChar *spiceFromFrame = fromFrame.c_str();
    ConstSpiceChar *spiceToFrame   = toFrame.c_str();
    
    //convert to spiceTime
    etSPICE = A1ToSpiceTime(GmatTime(atTime).GetMjd());


#ifdef DEBUG_SPK_READING

    MessageInterface::ShowMessage("Time passed to SPICE is %12.14f\n", (Real)etSPICE);
#endif
    SpiceDouble spiceRotMat[6][6];
  
    sxform_c(spiceFromFrame, spiceToFrame, etSPICE, spiceRotMat);

#ifdef DEBUG_SPK_READING
    MessageInterface::ShowMessage(
       "In SPKReader::Called sxform_c and got rotmat out:\n%12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n%12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n%12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n%12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n%12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n%12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
       spiceRotMat[0][0], spiceRotMat[0][1], spiceRotMat[0][2], spiceRotMat[0][3], spiceRotMat[0][4], spiceRotMat[0][5],
       spiceRotMat[1][0], spiceRotMat[1][1], spiceRotMat[1][2], spiceRotMat[1][3], spiceRotMat[1][4], spiceRotMat[1][5],
       spiceRotMat[2][0], spiceRotMat[2][1], spiceRotMat[2][2], spiceRotMat[2][3], spiceRotMat[2][4], spiceRotMat[2][5],
       spiceRotMat[3][0], spiceRotMat[3][1], spiceRotMat[3][2], spiceRotMat[3][3], spiceRotMat[3][4], spiceRotMat[3][5],
       spiceRotMat[4][0], spiceRotMat[4][1], spiceRotMat[4][2], spiceRotMat[4][3], spiceRotMat[4][4], spiceRotMat[4][5],
       spiceRotMat[5][0], spiceRotMat[5][1], spiceRotMat[5][2], spiceRotMat[5][3], spiceRotMat[5][4], spiceRotMat[5][5]);
#endif

    bool failed = false;
    if (failed_c())
    {
       ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
       SpiceInt       numChar = MAX_LONG_MESSAGE_VALUE;
       SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
       getmsg_c(option, numChar, err);
       std::string errstr(err);
       delete[] err;
       reset_c();

       std::string errmsg = "Error when generating rotation matrix from \"" + fromFrame + "\" to \"";
       errmsg += toFrame + "\". Message received from CSPICE is: [ ";
       errmsg += errstr + " ]\n";

       throw UtilityException(errmsg);
    }
    
    for (int r = 0; r < 6; ++r)
       for (int c = 0; c < 6; ++c)
          retval(r, c) = spiceRotMat[r][c];


    return retval;
 }


 //------------------------------------------------------------------------------
//  Rmatrix33 GetRotationMatrix33(std::string fromFrame,  )
//                              std::string toFrame,
//                              const GmatTime & atTime)
//------------------------------------------------------------------------------
/**
 * This method returns a 3x3 rotation matrix from the coordinate system fromFrame
 * to the system toFrame at epoch atTime usng the spice call pxform_c
 *
 * @param <fromFramee>        frame to transform from
 * @param <toFrame>           frame to transform into
 * @param <atTime>            time at which the rotation matrix is requested.
 *
 * @return Rmatrix33 rotation matrix
 *
 */
 //------------------------------------------------------------------------------
 Rmatrix33 SpiceOrbitKernelReader::GetRotationMatrix33(std::string fromFrame, std::string toFrame, const GmatTime & atTime)
 {
    Rmatrix33 retval = Rmatrix33();

    ConstSpiceChar *spiceFromFrame = fromFrame.c_str();
    ConstSpiceChar *spiceToFrame = toFrame.c_str();

    //convert to spiceTime
    etSPICE = A1ToSpiceTime(GmatTime(atTime).GetMjd());


#ifdef DEBUG_SPK_READING

    MessageInterface::ShowMessage("Time passed to SPICE is %12.14f\n", (Real)etSPICE);
#endif
    SpiceDouble spiceRotMat[3][3];

    pxform_c(spiceFromFrame, spiceToFrame, etSPICE, spiceRotMat);

#ifdef DEBUG_SPK_READING
    MessageInterface::ShowMessage(
       "In SPKReader::Called pxform_c and got rotmat out:\n%12.10f  %12.10f  %12.10f \n%12.10f  %12.10f  %12.10f \n%12.10f  %12.10f  %12.10f\n",
       spiceRotMat[0][0], spiceRotMat[0][1], spiceRotMat[0][2],
       spiceRotMat[1][0], spiceRotMat[1][1], spiceRotMat[1][2],
       spiceRotMat[2][0], spiceRotMat[2][1], spiceRotMat[2][2]);
#endif

    bool failed = false;
    if (failed_c())
    {
       ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
       SpiceInt       numChar = MAX_LONG_MESSAGE_VALUE;
       SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
       getmsg_c(option, numChar, err);
       std::string errstr(err);
       delete[] err;
       reset_c();

       std::string errmsg = "Error when generating rotation matrix from \"" + fromFrame + "\" to \"";
       errmsg += toFrame + "\". Message received from CSPICE is: [ ";
       errmsg += errstr + " ]\n";

       throw UtilityException(errmsg);
    }

    for (int r = 0; r < 3; ++r)
       for (int c = 0; c < 3; ++c)
          retval(r, c) = spiceRotMat[r][c];


    return retval;
 }



//------------------------------------------------------------------------------
//  void StoreErrorMessage(const std::string & targetName)
//------------------------------------------------------------------------------
/**
 *  This method pulls an error message from spice and, if it is an error message
 *  for Insufficient ephemeris data, it will store it for later. Otherwise, it will
 *  throw an error with the error message from spice.
 *
 *  @param <targetName>        name of the target object.
 *
 */
 //------------------------------------------------------------------------------
void SpiceOrbitKernelReader::StoreErrorMessage(const std::string & targetName)
{
	// Get the long message for later reporting, if appropriate
	ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
	SpiceInt       numChar = MAX_LONG_MESSAGE_VALUE;
	SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
	getmsg_c(option, numChar, err);
	std::string errstr(err);
	delete[] err;
	reset_c();
   bool stored = false;

   if (errstr.length() > 27)
   {
      if (errstr.substr(0, 27) == "Insufficient ephemeris data")
      {
         std::string msg = errstr.substr(0, errstr.length() - 26);
         std::string epoch = errstr.substr(errstr.length() - 25);

         spiceErrorMSG[msg] += epoch;
         stored = true;
      }
   }

	if(!stored)
	{
		std::string errmsg = "Error getting state for body \"";
		errmsg += targetName + "\". Message received from CSPICE is: [ ";
		errmsg += errstr + " ]\n";
		
		throw UtilityException(errmsg);
	}

}


//------------------------------------------------------------------------------
//  bool CheckIfInCoverageRange(Integer forNaifId, const A1Mjd & atTime)
//------------------------------------------------------------------------------
/**
 *  Checks if a given object, has data in a loaded kernel and if 
 *  the provided epoch is within the range of the loaded data.
 *  If there is no data loaded for the requested object, throws an error.
 *
 *  @param <forNaifId>        NAIF ID of the requested object.
 *  @param <atTime>           Epoch to be checked.
 *
 *  @return bool    true if the epoch is within the data range, false if not
 */
 //------------------------------------------------------------------------------
bool SpiceOrbitKernelReader::CheckIfInCoverageRange(Integer forNaifId, const A1Mjd & atTime)
{
   bool retVal = false;
   bool hasRange = true;
   if (objectCoverageRanges.find(forNaifId) == objectCoverageRanges.end())
   {
      StoreCoverageRange(forNaifId);
      if (objectCoverageRanges.find(forNaifId) == objectCoverageRanges.end())
      {
         hasRange = false;
      }
   }

   if (hasRange)
   {
      GmatTime start = std::get<0>(objectCoverageRanges[forNaifId]);
      GmatTime end = std::get<1>(objectCoverageRanges[forNaifId]);
      if (atTime >= start.GetMjd() && atTime <= end.GetMjd())
      {
         retVal = true;
      }
   }

   return retVal;
}


//------------------------------------------------------------------------------
//  void StoreCoverageRange(Integer forNaifId)
//------------------------------------------------------------------------------
/**
 *  Stores range of data in a given object into objectCoverageRanges. 
 *  Throws an error if no data is loaded for the given object.
 *  If you want to return the coverage range, use GetCoverageStartAndEnd().
 *
 *  @param <forNaifId>        NAIF ID of the requested object.
 *
 */
 //------------------------------------------------------------------------------
void SpiceOrbitKernelReader::StoreCoverageRange(Integer forNaifId)
{
   StringArray kers = {};
   std::map<std::string, std::string>::iterator ii;
   for (ii = loadedKernels.begin(); ii != loadedKernels.end(); ++ii)
   {
      kers.push_back((*ii).first);
   }

   Real start;
   Real end;

   GetCoverageStartAndEnd(kers, forNaifId, start, end);

   objectCoverageRanges[forNaifId] = {start, end};
}


//------------------------------------------------------------------------------
//  void PrintWarningMessage()
//------------------------------------------------------------------------------
/**
 * Prints a warning message using an error message previously stored from spice. 
 * If no error message was previously stored, nothing will be printed.
 * 
 * @param <targetName>        name of the target object.
 */
 //------------------------------------------------------------------------------
void SpiceOrbitKernelReader::PrintWarningMessage(const std::string & targetName)
{
	std::map<std::string, std::string>::iterator it;
	
	for (it = spiceErrorMSG.begin(); it != spiceErrorMSG.end(); it++)
	{
      gapsFound++;
		std::string errmsg = "WARNING data gaps in Spice file present for body \"";
		errmsg += targetName + "\". Message received from CSPICE is: ";
		errmsg += it->first + "s: ";
		errmsg += it->second.substr(0, 24) + " to " + it->second.substr(it->second.length() - 25) + "\n\n";

      if (gapsFound < 5)
         MessageInterface::ShowMessage(errmsg);
      else if (gapsFound == 5)
         MessageInterface::ShowMessage("WARNING 5 or more data gaps were found in the spice file present for body " + targetName + " no more warnings will be issued\n");
	}

	spiceErrorMSG = {};

}

//------------------------------------------------------------------------------
//  void PrintErrorMessage()
//------------------------------------------------------------------------------
/**
 * Prints an error message from spice. If failed_c(), then pulls message and throws
 * an error with that message. 
 * Else, if an error message was previously stored, throw an error with the message.
 *
 * @param <targetName>        name of the target object.
 */
 //------------------------------------------------------------------------------
void SpiceOrbitKernelReader::PrintErrorMessage(const std::string & targetName)
{

   if (failed_c())
   {
      ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
      SpiceInt       numChar = MAX_LONG_MESSAGE_VALUE;
      SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
      getmsg_c(option, numChar, err);
      std::string errstr(err);
      delete[] err;
      reset_c();
      
      std::string errmsg = "Error getting state for body \"";
      errmsg += targetName + "\". Message received from CSPICE is: [ ";
      errmsg += errstr + " ]\n";

      throw UtilityException(errmsg);
   }
   else
   {
      std::map<std::string, std::string>::iterator it;

      for (it = spiceErrorMSG.begin(); it != spiceErrorMSG.end(); it++)
      {
         gapsFound++;
         std::string errmsg = "Error getting state for body \"";
         errmsg += targetName + "\". Message received from CSPICE is: [ ";
         errmsg += it->first + " ";
         errmsg += it->second.substr(0, 24) + " ]\n";
         
         throw UtilityException(errmsg);

      }

   }
}



