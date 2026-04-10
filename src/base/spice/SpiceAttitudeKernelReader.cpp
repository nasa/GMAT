//$Id:$
//------------------------------------------------------------------------------
//                              SpiceAttitudeKernelReader
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
 * Definition of the SpiceAttitudeKernelReader, which reads SPICE CK (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to read the specified
 * SPICE file(s) and return the requested data.
 */
//------------------------------------------------------------------------------
#include "SpiceAttitudeKernelReader.hpp"
#include "gmatdefs.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "A1Mjd.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"
#include "UtilityException.hpp"


//#define DEBUG_CK_READING
//#define DEBUG_CK_COVERAGE

// -----------------------------------------------------------------------------
// static data
// -----------------------------------------------------------------------------
// none


// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  SpiceAttitudeKernelReader()
//---------------------------------------------------------------------------
/**
 * Creates a SpiceAttitudeKernelReader.
 *
 * @note This is the default constructor
 */
//---------------------------------------------------------------------------
SpiceAttitudeKernelReader::SpiceAttitudeKernelReader() :
   SpiceKernelReader()
{
}

//---------------------------------------------------------------------------
//  SpiceAttitudeKernelReader(const SpiceAttitudeKernelReader &reader)
//---------------------------------------------------------------------------
/**
 * Creates a SpiceAttitudeKernelReader, with data copied from the input
 * instance.
 *
 * @param reader the input reader whose data to copy.
 *
 * @note This is the copy constructor
 */
//---------------------------------------------------------------------------
SpiceAttitudeKernelReader::SpiceAttitudeKernelReader(const SpiceAttitudeKernelReader &reader) :
   SpiceKernelReader(reader)
{
}

//---------------------------------------------------------------------------
//  SpiceAttitudeKernelReader& operator=(const SpiceAttitudeKernelReader &reader)
//---------------------------------------------------------------------------
/**
 * Copies data from the input SpiceAttitudeKernelReader,into "this" instance.
 *
 * @param reader the input reader whose data to use.
 */
//---------------------------------------------------------------------------
SpiceAttitudeKernelReader& SpiceAttitudeKernelReader::operator=(const SpiceAttitudeKernelReader &reader)
{
   if (&reader == this)
      return *this;

   SpiceKernelReader::operator=(reader);

   return *this;
}

//---------------------------------------------------------------------------
//  ~SpiceAttitudeKernelReader()
//---------------------------------------------------------------------------
/**
 * Deletes "this" instance.
 *
 * @note This is the destructor.
 */
//---------------------------------------------------------------------------
SpiceAttitudeKernelReader::~SpiceAttitudeKernelReader()
{
}

//---------------------------------------------------------------------------
//  SpiceAttitudeKernelReader* Clone(void) const
//---------------------------------------------------------------------------
/**
 * Creates and returns a clone of "this" instance.
 *
 * @return Clone of "this" instance.
 */
//---------------------------------------------------------------------------
SpiceAttitudeKernelReader* SpiceAttitudeKernelReader::Clone(void) const
{
   SpiceAttitudeKernelReader * clonedSKR = new SpiceAttitudeKernelReader(*this);

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
 * @param forNaifId the NAIF ID of the reference frame for the object for which
 *                  coverage should be determined
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
 *       are not of type 'ck'.
 */
//------------------------------------------------------------------------------
void  SpiceAttitudeKernelReader::GetCoverageStartAndEnd(StringArray       &kernels,
                                                        Integer           forNaifId,
                                                        Real              &start,
                                                        Real              &end,
                                                        bool              needAngVel)
{
   // first check to see if a kernel specified is not loaded; if not,
   // try to load it
   for (unsigned int ii = 0; ii < kernels.size(); ii++)
      if (!IsLoaded(kernels.at(ii)))   LoadKernel(kernels.at(ii));

   SpiceInt         idSpice     = forNaifId;
   SpiceInt         arclen      = 4;
   SpiceInt         typlen      = 5;
   bool             firstInt    = true;
   bool             idOnKernel  = false;
   char             kStr[5]     = "    ";
   char             aStr[4]     = "   ";
   char             levelStr[8] = "SEGMENT";
   char             timeStr[4]  = "TDB";
   SpiceBoolean     needAv      = needAngVel;
   ConstSpiceChar   *kernelName = NULL;
   ConstSpiceChar   *level      = levelStr;
   ConstSpiceChar   *timeSys    = timeStr;
   SpiceDouble      tol         = 0.0;
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

   // look through each kernel
   for (unsigned int ii = 0; ii < kernels.size(); ii++)
   {
      #ifdef DEBUG_CK_COVERAGE
         MessageInterface::ShowMessage("Checking coverage for ID %d on kernel %s\n",
               forNaifId, (kernels.at(ii)).c_str());
      #endif
      // SPICE expects forward slashes for directory separators
      std::string kName = GmatStringUtil::Replace(kernels.at(ii), "\\", "/");
      kernelName = kName.c_str();
      // check the type of kernel
      arch        = aStr;
      kernelType  = kStr;
      getfat_c(kernelName, arclen, typlen, arch, kernelType);
      if (failed_c())
      {
         ConstSpiceChar option[] = "LONG";
         SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
         SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
         getmsg_c(option, numChar, err);
         std::string errStr(err);
         std::string errmsg = "Error determining type of kernel \"";
         errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: ";
         errmsg += errStr + "\n";
         reset_c();
         throw UtilityException(errmsg);
      }
      #ifdef DEBUG_CK_COVERAGE
         MessageInterface::ShowMessage("Kernel is of type %s\n",
               kernelType);
      #endif
      // only deal with CK kernels
      if (eqstr_c(kernelType, "ck") || eqstr_c(kernelType, "CK"))
      {
         ckobj_c(kernelName, &ids);
         // get the list of objects (IDs) for which data exists in the CK kernel
         for (SpiceInt jj = 0;  jj < card_c(&ids);  jj++)
         {
            objId = SPICE_CELL_ELEM_I(&ids,jj);
            #ifdef DEBUG_CK_COVERAGE
               MessageInterface::ShowMessage("Kernel contains data for object %d\n",
                     (Integer) objId);
            #endif
            // look to see if this kernel contains data for the object we're interested in
            if (objId == idSpice)
            {
               idOnKernel = true;
               break;
            }
//            if (objId == (idSpice * 1000))
//            {
//               idSpice     = idSpice * 1000;
//               naifIDSPICE = idSpice; // not the way to do this - should pass it back
//               idOnKernel  = true;
//               break;
//            }
         }
         // only deal with kernels containing data for the object we're interested in
         if (idOnKernel)
         {
            #ifdef DEBUG_CK_COVERAGE
               MessageInterface::ShowMessage("Checking kernel %s for data for object %d\n",
                     (kernels.at(ii)).c_str(), (Integer) objId);
            #endif
            scard_c(0, &cover);   // reset the coverage cell
            ckcov_c (kernelName, idSpice, needAv, level, tol, timeSys, &cover);
            if (failed_c())
            {
               ConstSpiceChar option[] = "LONG";
               SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
               SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
               getmsg_c(option, numChar, err);
               std::string errStr(err);
               std::string errmsg = "Error determining coverage for CK kernel \"";
               errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: ";
               errmsg += errStr + "\n";
               reset_c();
               throw UtilityException(errmsg);
            }
            numInt = wncard_c(&cover);
            #ifdef DEBUG_CK_COVERAGE
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
                  SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
                  getmsg_c(option, numChar, err);
                  std::string errStr(err);
                  std::string errmsg = "Error getting interval times for CK kernel \"";
                  errmsg += kernels.at(ii) + "\".  Message received from CSPICE is: ";
                  errmsg += errStr + "\n";
                  reset_c();
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
      char           itsName[256];
      SpiceChar      *itsNameSPICE = itsName;
      SpiceBoolean   found2;
      bodc2n_c(naifIDSPICE, 256, itsNameSPICE, &found2);
      if (found2 == SPICEFALSE)
      {
         std::string errmsg = "Error - unable to find name for body in SPICE kernel pool";
         throw UtilityException(errmsg);
      }
      else
      {
         std::string nameStr = itsNameSPICE;
         std::string errmsg = "Error - no data available for body ";
         errmsg += nameStr + " on specified CK kernels";
         throw UtilityException(errmsg);
      }
   }
}

//---------------------------------------------------------------------------
//  void GetTargetOrientation(const std::string &objectName,
//                            Integer           naifID,    const A1Mjd       &atTime,
//                            Rmatrix33         &r33,      Rvector3          &angVel,
//                            const std::string &referenceFrame = "J2000")
//---------------------------------------------------------------------------
/**
 * Gets the target orientation (cosine matrix and angular velocity) for the
 * requested time, given the object name and NAIF ID.
 *
 * @param   objectName   name of the object for which to obtain the data
 * @param   naifID       NAIF ID of the reference frame for the object
 * @param   atTime       time for which to obtain the data
 * @param   r33          (output) cosine matrix at time atTime
 * @param   angVel       (output) angular velocity at time atTime
 * @param   referenceFrame reference frame for the data (default "J2000")
 */
//---------------------------------------------------------------------------
void SpiceAttitudeKernelReader::GetTargetOrientation(const std::string &objectName,
                                                     Integer           naifID,
                                                     Integer           forFrameNaifId,
                                                     const A1Mjd       &atTime,
//                                                     Real              tolerance,
                                                     Rmatrix33         &r33,
                                                     Rvector3          &angVel,
                                                     const std::string &referenceFrame)
{
   #ifdef DEBUG_CK_READING
      MessageInterface::ShowMessage("Entering GetTargetOrientation for object %s, with NAIF ID %d, at time %12.10f, with frame = %s\n",
         objectName.c_str(), naifID, atTime.Get(), referenceFrame.c_str());
   #endif
   std::string objectNameToUse = objectName;

   objectNameToUse       = GmatStringUtil::ToUpper(objectNameToUse);
   objectNameSPICE       = objectNameToUse.c_str();
   naifIDSPICE           = naifID;
   frameNaifIDSPICE      = forFrameNaifId;
   referenceFrameSPICE   = referenceFrame.c_str();
   etSPICE               = A1ToSpiceTime(atTime.Get());

//   boddef_c(objectNameSPICE, naifIDSPICE);        // CSPICE method to set NAIF ID for an object - is this valid for spacecraft?
   // Convert the time (in TDB) to spacecaft ticks
   SpiceDouble scTime;
   
   #ifdef DEBUG_CK_READING
   // Why VS static build crashes here? (LOJ: 2014.06.27)
   // In igrf.c, function shellg_0_(),     *b0 = sqrt(bq2); bq2 is NAN
   MessageInterface::ShowMessage
      ("   Calling sce2c_c(), naifIDSPICE = %d, etSPICE = %d\n", naifIDSPICE, etSPICE);
   #endif
   
   sce2c_c(naifIDSPICE, etSPICE, &scTime);

   #ifdef DEBUG_CK_READING
   MessageInterface::ShowMessage("   After calling sce2c_c(), scTime = %f\n", scTime);
   #endif
   
   if (failed_c())
   {
      ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
      SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
      //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
      SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
      getmsg_c(option, numChar, err);
      std::string errStr(err);
      std::string errmsg = "Error getting spacecraft time (ticks) for object \"";
      errmsg += objectName + "\".  Message received from CSPICE is: ";
      errmsg += errStr + "\n";
      reset_c();
      delete [] err;
      throw UtilityException(errmsg);
   }
   // get the tolerance in spacecraft clock ticks
   std::string    tolerance = "01";  // this should probably be user input, or set as a constant
   ConstSpiceChar *tol = tolerance.c_str();
   SpiceDouble    tolTicks;
   sctiks_c(naifIDSPICE, tol, &tolTicks);
   if (failed_c())
   {
      ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
      SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
      //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
      SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
      getmsg_c(option, numChar, err);
      std::string errStr(err);
      std::string errmsg = "Error getting tolerance (ticks) for object \"";
      errmsg += objectName + "\".  Message received from CSPICE is: ";
      errmsg += errStr + "\n";
      reset_c();
      delete [] err;
      throw UtilityException(errmsg);
   }
   // the following lines are commented out for performance.  Running the check on coverage slows
   // down a run to about 30 times it's normal run time.
//   #ifdef DEBUG_CK_READING
//      MessageInterface::ShowMessage("First, check for coverage for object \"%s\", with NAIF ID %d\n",
//         objectName.c_str(), naifID);
//   #endif
//   Real beginCov = 0.0;
//   Real endCov   = 0.0;
//   GetCoverageStartAndEnd(loadedKernels, forFrameNaifId, beginCov, endCov, false);

   // Now get the C-matrix and angular velocity at the requested time
   SpiceDouble    cmat[3][3];
   SpiceDouble    av[3];
   SpiceBoolean   found;
   SpiceDouble    clkout;
   #ifdef DEBUG_CK_READING
      MessageInterface::ShowMessage("about to call ckgpav: \n");
      MessageInterface::ShowMessage("   NAIF ID  = %d\n"
                                    "   etSPICE  = %12.10f\n"
                                    "   scTime   = %12.10fn"
                                    "   tolTicks = %12.10f\n"
                                    "   refFrame = %s\n",
         (Integer) naifIDSPICE, (Real) etSPICE, (Real) scTime, (Real) tolTicks,
         referenceFrame.c_str());
   #endif
   ckgpav_c(frameNaifIDSPICE, scTime, tolTicks, referenceFrameSPICE, cmat, av, &clkout, &found);
//   ckgpav_c(naifIDSPICE, scTime, tolTicks, referenceFrameSPICE, cmat, av, &clkout, &found);
   if (failed_c())
   {
      ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
      SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
      //SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
      SpiceChar      *err = new SpiceChar[MAX_LONG_MESSAGE_VALUE];
      getmsg_c(option, numChar, err);
      std::string errStr(err);
      std::string errmsg = "Error getting C-matrix and/or angular velocity for object \"";
      errmsg += objectName + "\".  Message received from CSPICE is: ";
      errmsg += errStr + "\n";
      reset_c();
      delete [] err;
      throw UtilityException(errmsg);
   }
   if (found == SPICEFALSE)
   {
      std::string errmsg = "Pointing data for object ";
      errmsg += objectName + " not found on loaded CK/SCLK kernels.\n";
      throw UtilityException(errmsg);
   }
   #ifdef DEBUG_CK_READING
      MessageInterface::ShowMessage("results from ckgpav: \n");
      MessageInterface::ShowMessage("   cosMat = %12.10f  %12.10f  %12.10f\n"
                                    "            %12.10f  %12.10f  %12.10f\n"
                                    "            %12.10f  %12.10f  %12.10f\n",
                                    (Real)cmat[0][0], (Real)cmat[0][1], (Real)cmat[0][2],
                                    (Real)cmat[1][0], (Real)cmat[1][1], (Real)cmat[1][2],
                                    (Real)cmat[2][0], (Real)cmat[2][1], (Real)cmat[2][2]);
      MessageInterface::ShowMessage("   angvel = %12.10f  %12.10f  %12.10f\n",
                                   (Real)av[0], (Real)av[1], (Real)av[2]);
      MessageInterface::ShowMessage("   and clkout = %12.10f\n", (Real) clkout);
   #endif
   // Set output values
   r33.Set(cmat[0][0], cmat[0][1], cmat[0][2],
           cmat[1][0], cmat[1][1], cmat[1][2],
           cmat[2][0], cmat[2][1], cmat[2][2]);
   angVel.Set(av[0], av[1], av[2]);

}
