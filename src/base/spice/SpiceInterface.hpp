//$Id$
//------------------------------------------------------------------------------
//                              SpiceInterface
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
// Created: 2008.10.22
//
/**
 * Definition of the SpiceInterface, which manages SPICE data (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to load and unload kernels
 * into and out of the kernel pool.
 * Kernels that may be loaded (and then read by child classes) are:
 *  planetary ephemeris (SPK)
 *  spacecraft ephemeris (SPK)
 *  spacecraft pointing (CK)
 *  spacecraft clock (SCLK)
 *  leap second  (LSK)
 *  planetary constants (PcK)   < future >
 *  instrument (IK)             < future >
 *  
 *  Kernels that may be created by child writer classes are:
 *   spacecraft ephemeris (SPK)
 *   spacecraft attitude (CK)  < future >
 *   spacecraft clock (SCLK)   < future >
 *
 * This is the base class.  Classes inheriting from this one handle the reading
 * or writing of specific types of data (orbit, attitude, ...).
 */
//------------------------------------------------------------------------------

#ifndef SpiceInterface_hpp
#define SpiceInterface_hpp

#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton

// include the appropriate SPICE C header(s)
extern "C"  
{
#include "SpiceUsr.h"    // for CSPICE routines
}

#define MAX_LONG_MESSAGE_VALUE 1840
#define MAX_SHORT_MESSAGE_VALUE 320

class GMAT_API SpiceInterface
{
public:
   /// static method to check for valid kernels
   static bool         IsValidKernel(const std::string &fileName, const std::string &ofType);

   /// class methods
   SpiceInterface();
   SpiceInterface(const SpiceInterface &copy);
   SpiceInterface& operator=(const SpiceInterface &copy);

   virtual ~SpiceInterface();
   
   virtual SpiceInterface* Clone() const;     //  = 0; making non-abstract for use by EphemManager

   virtual bool        LoadKernel(const std::string &fileName);
   virtual bool        LoadKernels(const StringArray &fileNames);
   virtual bool        UnloadKernel(const std::string &fileName);
   virtual bool        UnloadKernels(const StringArray &fileNames);
   virtual bool        UnloadAllKernels();
   virtual bool        IsLoaded(const std::string &fileName);
   
   virtual StringArray GetValidAberrationCorrectionFlags();
   virtual StringArray GetValidFrames();
   virtual void        SetLeapSecondKernel(const std::string &lsk);
   
   virtual Integer     GetNaifID(const std::string &forObj, bool popupMsg = true);
   
   virtual Real        SpiceTimeToA1(SpiceDouble spiceTime);
   virtual SpiceDouble A1ToSpiceTime(Real a1Time);

   static const Integer     DEFAULT_NAIF_ID;
   static const Integer     DEFAULT_NAIF_ID_REF_FRAME;

protected:

   
   // data converted to SPICE types, to pass into SPICE methods
   /// the kernel name
   ConstSpiceChar  *kernelNameSPICE;
   
   static const std::string VALID_ABERRATION_FLAGS[9];
   static const Integer     NUM_VALID_FRAMES;
   static const std::string VALID_FRAMES[12];
   /// maximum number of characters for short, explanation of short, or
   /// long error message requested when calling CSPICE method getmsg_c
   static const Integer     MAX_SHORT_MESSAGE;
   static const Integer     MAX_EXPLAIN_MESSAGE;
   static const Integer     MAX_LONG_MESSAGE;
   static const Integer     MAX_CHAR_COMMENT;
   static const std::string FILE_NOT_FOUND;
   
   /// map of requested (original) kernel names and the actual name
   /// (including path, etc. as needed)
   static std::map<std::string, std::string> loadedKernels;

   /// counter of number of instances created
   static Integer        numInstances;
   /// the name (full path) of the leap second kernel to use
   static std::string lsKernel;

   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;

   //   static void InitializeReader();
   static void InitializeInterface();

   static std::string FindKernel(const std::string &withName);
   
   virtual std::string GetFullKernelPath(const std::string &forKernel);

};

#endif // SpiceInterface_hpp
