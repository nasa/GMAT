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
// Created: 2009.02.24 / 2010.04.20
//
/**
 * Implementation of the SpiceInterface, which loads/unloads SPICE data (kernel)
 * files.  This class calls the JPL-supplied CSPICE routines.
 */
//------------------------------------------------------------------------------


#include "SpiceInterface.hpp"
#include "A1Mjd.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"
#include "UtilityException.hpp"
#include "FileManager.hpp"
#include "FileUtil.hpp"

//#define DEBUG_SPK_LOADING
//#define DEBUG_SPK_ISLOADED
//#define DEBUG_SPK_READING
//#define DEBUG_SPK_GETSTATE
//#define DEBUG_SPK_PLANETS
//#define DEBUG_VALID_KERNEL
//#define DEBUG_SPK_INIT
//#define DEBUG_FIND_KERNEL
//#define DEBUG_SPK_FULL_PATH

#ifdef DEBUG_VALID_KERNEL
#include <unistd.h>
#endif

//---------------------------------
// static data
//---------------------------------
const Integer SpiceInterface::DEFAULT_NAIF_ID           = -123456789;
const Integer SpiceInterface::DEFAULT_NAIF_ID_REF_FRAME = -123456789;

const std::string
SpiceInterface::VALID_ABERRATION_FLAGS[9] =
{
   "NONE",    // Apply no correction
   // the following 4 options apply to the 'reception' case, in which photons
   // depart from the target's location at time et-lt and arrive at the 
   // observer's location at et (input time)
   "LT",      // Correct for one-way light time
   "LT+S",    // Correct for one-way light time and stellar aberration
   "CN",      // Converged Newtonian light time correction
   "CN+S",    // Converged Newtonian light time and stellar aberration correction
   // the following 4 options apply to the 'transmission' case, in which photons
   // depart from the observer's location at time et and arrive at the 
   // target's location at et+lt (input time)
   "XLT",     // Correct for one-way light time
   "XLT+S",   // Correct for one-way light time and stellar aberration
   "XCN",     // Converged Newtonian light time correction
   "XCN+S",   // Converged Newtonian light time and stellar aberration correction
};

const Integer SpiceInterface::NUM_VALID_FRAMES = 1; // for now, only "J2000"

const std::string
SpiceInterface::VALID_FRAMES[12] =
{
   "J2000",   // default frame
   "NONE",   // TBD
   "NONE",   // TBD
   "NONE",   // TBD
   "NONE",   // TBD
   "NONE",   // TBD
   "NONE",   // TBD
   "NONE",   // TBD
   "NONE",   // TBD
   "NONE",   // TBD
   "NONE",   // TBD
   "NONE",   // TBD
};

const Integer SpiceInterface::MAX_SHORT_MESSAGE         = MAX_SHORT_MESSAGE_VALUE;
const Integer SpiceInterface::MAX_EXPLAIN_MESSAGE       = 320;
const Integer SpiceInterface::MAX_LONG_MESSAGE          = MAX_LONG_MESSAGE_VALUE;
const Integer SpiceInterface::MAX_CHAR_COMMENT          = 4000;
const std::string SpiceInterface::FILE_NOT_FOUND        = "";

/// map of requested (original) kernel names and the actual name
/// (including path, etc. as needed)
std::map<std::string, std::string> SpiceInterface::loadedKernels;

/// counter of number of instances created
Integer        SpiceInterface::numInstances = 0;
/// the name (full path) of the leap second kernel to use
std::string    SpiceInterface::lsKernel = "";

//------------------------------------------------------------------------------
// static public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  bool IsValidKernel(const std::string &fileName, const std::string &ofType)
//------------------------------------------------------------------------------
/**
 * This method checks to see if the input file is a valid SPICE kernel of the
 * specified type.
 *
 * @param <ofType>    type of kernel
 * @param <fileName>  full path of the file (kernel) to check
 *
 * @return true if the file (kernel) is a valid kernel of the requested type;
 *         false otherwise
 *
 */
//------------------------------------------------------------------------------
bool SpiceInterface::IsValidKernel(const std::string &fileName, const std::string &ofType)
{
   #ifdef DEBUG_VALID_KERNEL
      MessageInterface::ShowMessage("Entering SpiceInterface::IsValidKernel with fileName = \"%s\" and ofType = \"%s\"\n",
            fileName.c_str(), ofType.c_str());
      char *curPath = NULL;
      size_t itsSize = 0;
      curPath = getcwd(curPath, itsSize);
      MessageInterface::ShowMessage("   and the current directory is \"%s\"\n", curPath);
   #endif

   // Initialize here, since this may be called before a KernelReader or
   // KernelWriter is created
   InitializeInterface();

   // SPICE expects forward slashes for directory separators
   std::string kName = FindKernel(fileName);
   if (kName == FILE_NOT_FOUND)
   {
      return false;
   }

   char             kStr[5] = "    ";
   char             aStr[4] = "   ";
   SpiceInt         arclen      = 4;
   SpiceInt         typlen      = 5;
   ConstSpiceChar   *kernelName = NULL;
   SpiceChar        *arch;
   SpiceChar        *kernelType;
   ConstSpiceChar   *typeToCheck = NULL;
   kernelName = kName.c_str();
   // check the type of kernel
   arch        = aStr;
   kernelType  = kStr;
   getfat_c(kernelName, arclen, typlen, arch, kernelType);
   if (failed_c())
   {
      #ifdef DEBUG_VALID_KERNEL
         MessageInterface::ShowMessage("In SpiceInterface::IsValidKernel, cannot find file \"%s\" ...\n",
               kName.c_str());
      #endif
      reset_c();
     return false;
   }
   #ifdef DEBUG_VALID_KERNEL
      MessageInterface::ShowMessage("In SpiceInterface::IsValidKernel, found file, now checking type ...\n");
   #endif

   typeToCheck = ofType.c_str();

   if (eqstr_c( kernelType, typeToCheck ))
   {
      #ifdef DEBUG_VALID_KERNEL
         MessageInterface::ShowMessage("In SpiceInterface::IsValidKernel, types match!...\n");
      #endif
      return true;
   }
   #ifdef DEBUG_VALID_KERNEL
      MessageInterface::ShowMessage("In SpiceInterface::IsValidKernel, types DO NOT match!...\n");
   #endif
   return false;
}


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  SpiceInterface()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpiceInterface class
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
SpiceInterface::SpiceInterface() :
   kernelNameSPICE         (NULL)
{
   theTimeConverter = TimeSystemConverter::Instance();
   InitializeInterface();
   numInstances++;
}

//------------------------------------------------------------------------------
//  SpiceInterface(const SpiceInterface &copy)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpiceInterface class, by copying
 * the input object.
 * (copy constructor).
 *
* @param <copy> SpiceInterface object to copy.
  */
//------------------------------------------------------------------------------
SpiceInterface::SpiceInterface(const SpiceInterface &copy) :
   kernelNameSPICE         (NULL)
{
   theTimeConverter = TimeSystemConverter::Instance();
   numInstances++;
//   // the kernels are all loaded into that one kernel pool,
//   // but we need to copy the names
//   loadedKernels = copy.loadedKernels;
//   lsKernel      = copy.lsKernel;
}

//------------------------------------------------------------------------------
//  SpiceInterface& operator=(const SpiceInterface &copy)
//------------------------------------------------------------------------------
/**
 * This method copies the data from the input object to the object.
 *
 * @param <copy> the SpiceInterface object whose data to assign to "this"
 *                 SpiceInterface.
 *
 * @return "this" SpiceInterface with data of input SpiceInterface reader.
 */
//------------------------------------------------------------------------------
SpiceInterface& SpiceInterface::operator=(const SpiceInterface &copy)
{
   if (&copy == this)
      return *this;

   kernelNameSPICE          = NULL;
//   // the kernels are all loaded into that one kernel pool,
//   // but we need to copy the names
//   loadedKernels.clear();
//   loadedKernels = copy.loadedKernels;
//   lsKernel      = copy.lsKernel;

   // don't modify numInstances - we are not creating a new instance here
   return *this;
}


//------------------------------------------------------------------------------
// ~SpiceInterface()
//------------------------------------------------------------------------------
/**
 * This method is the destructor for the SpiceInterface.
 *
 */
//------------------------------------------------------------------------------
SpiceInterface::~SpiceInterface()
{
   numInstances--;
   if (numInstances <= 0)
   {
      #ifdef DEBUG_SPK_LOADING
         MessageInterface::ShowMessage("UNLOADING ALL Kernels!\n");
      #endif
      UnloadAllKernels();
   }
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the SpiceInterface.
 *
 * @return clone of the SpiceInterface.
 *
 */
//------------------------------------------------------------------------------
SpiceInterface* SpiceInterface::Clone() const
{
   return (new SpiceInterface(*this));
}



//------------------------------------------------------------------------------
//  bool LoadKernel(const std::string &fileName)
//------------------------------------------------------------------------------
/**
 * This method loads the input file into the SPICE kernel pool.
 *
 * @param <fileName>  full path of the file (kernel) to load.
 *
 * @return success flag.
 *
 */
//------------------------------------------------------------------------------
bool SpiceInterface::LoadKernel(const std::string &fileName)
{
   #ifdef DEBUG_SPK_LOADING
//      char *path=NULL;
//      size_t size = 0;
//      path=getcwd(path,size);
      MessageInterface::ShowMessage("SpiceInterface: Attempting to load kernel %s <---------\n",
                  fileName.c_str());
//      MessageInterface::ShowMessage( "   and CURRENT PATH = %s\n", path);
   #endif

   std::map<std::string, std::string>::iterator ii;
   for (ii = loadedKernels.begin(); ii != loadedKernels.end(); ++ii)
   {
      if ((*ii).first == fileName)
      {
         #ifdef DEBUG_SPK_LOADING
            MessageInterface::ShowMessage("SpiceInterface: kernel %s is ALREADY LOADED!!!\n",
                        fileName.c_str());
         #endif
         return false;  // already loaded
      }
   }
   // SPICE expects forward slashes for directory separators
   std::string fName = FindKernel(fileName);
   if (fName == FILE_NOT_FOUND)
   {
      std::string errmsg = "Error loading kernel \"";
      errmsg += fileName + "\".  File not found.\n";
      throw UtilityException(errmsg);
   }

   kernelNameSPICE = fName.c_str();
   furnsh_c(kernelNameSPICE);
   if (failed_c()) //  == SPICETRUE)
   {
//      ConstSpiceChar option[] = "SHORT"; // retrieve short error message, for now
//      SpiceInt       numChar  = MAX_SHORT_MESSAGE;
//      SpiceChar      err[MAX_SHORT_MESSAGE];
      #ifdef DEBUG_SPK_LOADING
            MessageInterface::ShowMessage("SpiceInterface: Error loading kernel %s (%s) <---------\n",
                  fileName.c_str(), fName.c_str());
      #endif
      ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
      SpiceInt       numChar  = MAX_LONG_MESSAGE;
      SpiceChar      err[MAX_LONG_MESSAGE];
      getmsg_c(option, numChar, err);
      std::string errStr(err);
      std::string errmsg = "Error loading kernel \"";
      errmsg += fileName + "\".  Message received from CSPICE is: ";
      errmsg += errStr + "\n";
      reset_c();
      throw UtilityException(errmsg);
   }
   #ifdef DEBUG_SPK_LOADING
      else
      {
         MessageInterface::ShowMessage("SpiceInterface Successfully loaded kernel %s (%s) <---------\n",
               fileName.c_str(), fName.c_str());
      }
   #endif

   // Write the kernel name to the log
   MessageInterface::LogMessage
      ("Kernel %s has been loaded.\n", fileName.c_str());

   // Add the pair to the map of kernels
   loadedKernels.insert(std::make_pair(fileName, fName));
   
   return true;
}

//------------------------------------------------------------------------------
//  bool LoadKernels(const StringArray &fileNames)
//------------------------------------------------------------------------------
/**
 * This method loads the input files into the SPICE kernel pool.
 *
 * @param <fileNames>  array of kernels to load.
 *
 * @todo Find files if full path is not given - reference to appropriate
 *       path from startup file
 *
 * @return success flag.
 *
 */
//------------------------------------------------------------------------------
bool SpiceInterface::LoadKernels(const StringArray &fileNames)
{
   for (unsigned int ii = 0; ii < fileNames.size(); ii++)
      LoadKernel(fileNames.at(ii));
   return true;
}

//------------------------------------------------------------------------------
//  bool UnloadKernel(const std::string &fileName)
//------------------------------------------------------------------------------
/**
 * This method unloads the input file from the SPICE kernel pool.
 *
 * @param <fileName>  full path of the file (kernel) to unload.
 *
 * @return success flag.
 *
 */
//------------------------------------------------------------------------------
bool SpiceInterface::UnloadKernel(const std::string &fileName)
{
   bool        found          = false;
   std::string kernelToUnload = "";

   std::map<std::string, std::string>::iterator ii;
   for (ii = loadedKernels.begin(); ii != loadedKernels.end(); ++ii)
   {
      if ((*ii).first == fileName)
      {
         kernelToUnload = (*ii).second;
         found          = true;
      }
   }
   if (!found)
   {
      #ifdef DEBUG_SPK_LOADING
         MessageInterface::ShowMessage(
               "SpiceInterface::UnloadKernel() - kernel %s is not currently loaded.\n",
               fileName.c_str());
      #endif
      return false;
   }

   #ifdef DEBUG_SPK_LOADING
      MessageInterface::ShowMessage("Now attempting to unload kernel %s (%s)\n",
            fileName.c_str(), kernelToUnload.c_str());
   #endif

   std::string fName = kernelToUnload;
   kernelNameSPICE = fName.c_str();
   unload_c(kernelNameSPICE);
   if (failed_c())
   {
//      ConstSpiceChar option[] = "SHORT"; // retrieve short error message, for now
//      SpiceInt       numChar  = MAX_SHORT_MESSAGE;
//      SpiceChar      err[MAX_SHORT_MESSAGE];
      ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
      SpiceInt       numChar  = MAX_LONG_MESSAGE;
      SpiceChar      err[MAX_LONG_MESSAGE];
      getmsg_c(option, numChar, err);
      std::string errStr(err);
      std::string errmsg = "*** WARNING *** Error unloading kernel \"";
      errmsg += fileName + "\".  Message received from CSPICE is: ";
      errmsg += errStr + "\n";
      reset_c();
//      throw UtilityException(errmsg);
      MessageInterface::ShowMessage("%s", errmsg.c_str());
   }

   #ifdef DEBUG_SPK_LOADING
      MessageInterface::ShowMessage("Successfully UNloaded kernel %s (%s)\n",
            fileName.c_str(), kernelToUnload.c_str());
   #endif
   // erase the unloaded file from the map (by key)
   loadedKernels.erase(fileName);
   return true; 
}

//------------------------------------------------------------------------------
//  bool UnloadKernels(const StringArray &fileNames)
//------------------------------------------------------------------------------
/**
 * This method unloads the input files into the SPICE kernel pool.
 *
 * @param <fileNames>  array of kernels to load.
 *
 * @todo Find files if full path is not given - reference to appropriate
 *       path from startup file
 *
 * @return success flag.
 *
 */
//------------------------------------------------------------------------------
bool SpiceInterface::UnloadKernels(const StringArray &fileNames)
{
   for (unsigned int ii = 0; ii < fileNames.size(); ii++)
      UnloadKernel(fileNames.at(ii));
   return true;
}

//------------------------------------------------------------------------------
//  bool UnloadAllKernels()
//------------------------------------------------------------------------------
/**
 * This method unloads all loaded kernels from the SPICE kernel pool.
 *
 * @return success flag.
 *
 */
//------------------------------------------------------------------------------
bool SpiceInterface::UnloadAllKernels()
{
   std::string kName;
   std::map<std::string, std::string>::iterator ii;
   for (ii = loadedKernels.begin(); ii != loadedKernels.end(); ++ii)
   {
      kName = (*ii).second;
      #ifdef DEBUG_SPK_LOADING
         MessageInterface::ShowMessage("Now attempting to unload kernel %s\n",
               kName.c_str());
      #endif
      kernelNameSPICE = kName.c_str();
      unload_c(kernelNameSPICE);
      if (failed_c())
      {
//         ConstSpiceChar option[] = "SHORT"; // retrieve short error message, for now
//         SpiceInt       numChar  = MAX_SHORT_MESSAGE;
//         SpiceChar      err[MAX_SHORT_MESSAGE];
         ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
         SpiceInt       numChar  = MAX_LONG_MESSAGE;
         SpiceChar      err[MAX_LONG_MESSAGE];
         getmsg_c(option, numChar, err);
         std::string errStr(err);
         std::string errmsg = "Error unloading kernel \"";
         errmsg += kName + "\".  Message received from CSPICE is: ";
         errmsg += errStr + "\n";
         reset_c();
         throw UtilityException(errmsg);
      }
      // @todo - handle exceptional conditions (SPICE signals) here ...
      #ifdef DEBUG_SPK_LOADING
         MessageInterface::ShowMessage("Successfully unloaded kernel %s\n",
               kName.c_str());
      #endif
   }
   loadedKernels.clear();
   return true;
}

//------------------------------------------------------------------------------
//  bool IsLoaded(const std::string &fileName)
//------------------------------------------------------------------------------
/**
 * This method checks to see if the input file is loaded into the kernel pool.
 *
 * @param <fileName>  full path of the file (kernel) to check for.
 *
 * @return true if the file (kernel) has been loaded;
 *         false otherwise
 *
 */
//------------------------------------------------------------------------------
bool SpiceInterface::IsLoaded(const std::string &fileName)
{
   #ifdef DEBUG_SPK_ISLOADED
      MessageInterface::ShowMessage("IsLoaded::Now attempting to find kernel name %s\n", fileName.c_str());
   #endif

   std::map<std::string, std::string>::iterator ii;
   for (ii = loadedKernels.begin(); ii != loadedKernels.end(); ++ii)
   {
      if ((*ii).first == fileName)
      {
         #ifdef DEBUG_SPK_ISLOADED
             MessageInterface::ShowMessage("IsLoaded::kernel name %s WAS INDEED ALREADY LOADED\n", fileName.c_str());
         #endif
         return true;
      }
   }
   #ifdef DEBUG_SPK_ISLOADED
      MessageInterface::ShowMessage("IsLoaded::kernel name %s NOT ALREADY LOADED\n", fileName.c_str());
   #endif
   return false;
}


//------------------------------------------------------------------------------
//  StringArray GetValidAberrationCorrectionFlags()
//------------------------------------------------------------------------------
/**
 * This method returns a string array containing all valid aberration flags.
 *
  * @return array of  strings representing valid SPICE aberration flags.
 *
 */
//------------------------------------------------------------------------------
StringArray SpiceInterface::GetValidAberrationCorrectionFlags()
{
   StringArray aberr;
   for (unsigned int ii = 0; ii < 9; ii++)
      aberr.push_back(VALID_ABERRATION_FLAGS[ii]);
   return aberr;
}

//------------------------------------------------------------------------------
//  StringArray GetValidFrames()
//------------------------------------------------------------------------------
/**
 * This method returns a string array containing all valid frames.
 *
  * @return array of  strings representing valid (built-in) SPICE frames.
 *
 */
//------------------------------------------------------------------------------
StringArray SpiceInterface::GetValidFrames()
{
   StringArray frames;
   for (Integer ii = 0; ii < NUM_VALID_FRAMES; ii++)
      frames.push_back(VALID_FRAMES[ii]);
   return frames;
}

//------------------------------------------------------------------------------
//  void SetLeapSecondKernel(const std::string &lsk)
//------------------------------------------------------------------------------
/**
 * This method sets the leap second kernel, loading it into the kernel pool.
 *
 * @param <lsk>  full path of the leap second  kernel to load.
 *
 *
 */
//------------------------------------------------------------------------------
void SpiceInterface::SetLeapSecondKernel(const std::string &lsk)
{
   lsKernel = lsk;
   if (!IsLoaded(lsKernel))
   {
      #ifdef DEBUG_SPK_LOADING
         MessageInterface::ShowMessage("SpiceInterface::SetLeapSecondKernel NOW loading LSK kernel %s\n", lsk.c_str());
      #endif
      LoadKernel(lsKernel);
   }
   else
   {
      #ifdef DEBUG_SPK_LOADING
         MessageInterface::ShowMessage("SpiceInterface::SetLeapSecondKernel LSK kernel %s was already loaded ...\n", lsk.c_str());
      #endif
   }
}

//------------------------------------------------------------------------------
//  Integer GetNaifID(const std::string &forObj, bool popupMsg)
//------------------------------------------------------------------------------
/**
 * This method returns the NAIF Id of an object, given its name.
 *
 * @param <forObj>   name of the object.
 * @param <popupMsg> indicates whether or not to put up a Popup message if there
 *                   is an error
 *
 * @return corresponding NAIF id; or 0 if not found
 *
 */
//------------------------------------------------------------------------------
Integer SpiceInterface::GetNaifID(const std::string &forObj, bool popupMsg)
{
   SpiceBoolean   found;
   SpiceInt       id;
   std::string    nameToUse = forObj;
   if (GmatStringUtil::ToUpper(forObj) == "LUNA")
      nameToUse = "MOON";
   else if (GmatStringUtil::ToUpper(forObj) == "SOLARSYSTEMBARYCENTER")
      nameToUse = "SSB";

   ConstSpiceChar *bodyName = nameToUse.c_str();
   bodn2c_c(bodyName, &id, &found);
   if (found == SPICEFALSE)
   {
      if (popupMsg)
      {
         std::string warnmsg = "Cannot find NAIF ID for object ";
         warnmsg += forObj + ".  Insufficient data available.  Another SPICE Kernel may be necessary.";
         MessageInterface::PopupMessage(Gmat::WARNING_, warnmsg);
      }
      return 0;
   }
   #ifdef DEBUG_SPK_READING
      MessageInterface::ShowMessage("NAIF ID for body %s has been found: it is %d\n",
                                    forObj.c_str(), (Integer) id);
   #endif
   return (Integer) id;
}


//------------------------------------------------------------------------------
//  Real SpiceTimeToA1(SpiceDouble spiceTime)
//------------------------------------------------------------------------------
/**
 * This method converts from a SPICE time (seconds from J2000 ET) to A.1.
 *
 * @param <spiceTime>  SPICE time (seconds since J2000 ET).
 *
 * @return resulting A.1 time
 *
 */
//------------------------------------------------------------------------------
Real SpiceInterface::SpiceTimeToA1(SpiceDouble spiceTime)
{
   SpiceDouble j2ET    = j2000_c();
   SpiceDouble julianOffset = GmatTimeConstants::JD_JAN_5_1941 - j2ET;
   Real        tdbTime = (spiceTime / GmatTimeConstants::SECS_PER_DAY) - julianOffset;

   Real        a1Time  = theTimeConverter->Convert(tdbTime, TimeSystemConverter::TDBMJD,
                         TimeSystemConverter::A1MJD, GmatTimeConstants::JD_JAN_5_1941);

   return a1Time;
}


//------------------------------------------------------------------------------
//  SpiceDouble A1ToSpiceTime(Real a1Time)
//------------------------------------------------------------------------------
/**
 * This method converts from a n A.1 time to SPICE time (seconds from J2000 ET).
 *
 * @param <spiceTime>  A.1 time .
 *
 * @return resulting SPICE time
 *
 */
//------------------------------------------------------------------------------
SpiceDouble SpiceInterface::A1ToSpiceTime(Real a1Time)
{
   SpiceDouble j2ET      = j2000_c();
   Real        tdbTime   = theTimeConverter->Convert(a1Time, TimeSystemConverter::A1MJD,
                           TimeSystemConverter::TDBMJD, GmatTimeConstants::JD_JAN_5_1941);
   SpiceDouble julianOffset = GmatTimeConstants::JD_JAN_5_1941 - j2ET;
   SpiceDouble spiceTime = (tdbTime + julianOffset) *
                           GmatTimeConstants::SECS_PER_DAY;

   return spiceTime;
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// static protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  void InitializeInterface()
//------------------------------------------------------------------------------
/**
 * This method initializes the static data for the reader, and sets up
 * SPICE error handling.
 *
 * @return corresponding NAIF id; or 0 if not found
 *
 */
//------------------------------------------------------------------------------
//void SpiceInterface::InitializeReader()
void SpiceInterface::InitializeInterface()
{
   if (numInstances == 0)
   {
      loadedKernels.clear();
      kclear_c();  // clear all kernels from the pool
      // Get path for output
      FileManager *fm = FileManager::Instance();
      std::string outPath = fm->GetAbsPathname(FileManager::OUTPUT_PATH) + "GMATSpiceKernelError.txt";
      // need to get rid of const-ness to convert to SpiceChar*
      char *pathChar;
      pathChar = new char[outPath.length() + 1];
      strcpy(pathChar, outPath.c_str());

      // set output file for cspice methods
      SpiceChar      *spiceErrorFileFullPath = pathChar;
      errdev_c("SET", 1840, spiceErrorFileFullPath);
      if (failed_c())
      {
         #ifdef DEBUG_SPK_INIT
            MessageInterface::ShowMessage("In SpiceInterface::InitializeInterface, CSPICE reports an error using file \"%s\" ...\n",
                  outPath.c_str());
         #endif
         ConstSpiceChar option[] = "LONG"; // retrieve long error message, for now
         SpiceInt       numChar  = MAX_LONG_MESSAGE;
         SpiceChar      err[MAX_LONG_MESSAGE];
         getmsg_c(option, numChar, err);
         std::string errStr(err);
         std::string errmsg = "Error using error file \"";
         errmsg += outPath + "\".  Message received from CSPICE is: ";
         errmsg += errStr + "\n";
         reset_c();
         throw UtilityException(errmsg);
      }


      // set actions for cspice error writing
      char  report[5] = "LONG"; // "ALL";
      char  action[7] = "RETURN";
      errprt_c("SET", 1840, report);
      erract_c("SET", 1840, action);

      delete [] pathChar;
   }
}

//------------------------------------------------------------------------------
//  std::string FindKernel(const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method finds the kernel with the given name, if it exists.  The code
 * will check locations in this order:
 * - the path as specified
 * - the directory specified for vehicle spice kernels (VEHICLE_EPHEM_SPK_PATH)
 * - the path relative to the current script location
 * - the path relative to the executable directory
 *
 * @return full file name, if found; blank, otherwise
 *
 */
//------------------------------------------------------------------------------
std::string SpiceInterface::FindKernel(const std::string &withName)
{
   #ifdef DEBUG_FIND_KERNEL
      MessageInterface::ShowMessage("Entering FindKernel with withName = %s\n",
                                    withName.c_str());
   #endif
   bool fileFound = true;
   // SPICE expects forward slashes for directory separators
   std::string fullName = GmatStringUtil::Replace(withName, "\\", "/");
   #ifdef DEBUG_FIND_KERNEL
      MessageInterface::ShowMessage("In FindKernel, fullName = %s\n",
                                    fullName.c_str());
   #endif
   if (!(GmatFileUtil::DoesFileExist(fullName)))
   {
      FileManager *fm = FileManager::Instance();
      // If there is no path for the file, look for it in the
      // directory specified for planetary SPK
      if (fullName.find("/") == fullName.npos)
      {
         std::string fullName1;
         // Changed to use VEHICLE_EPHEM_SPK_PATH (LOJ: 2014.06.18)
         //std::string spkPath = fm->GetPathname(FileManager::SPK_PATH);
         std::string vehicleSpkPath =
                     fm->GetPathname(FileManager::VEHICLE_EPHEM_SPK_PATH);
         fullName1 = vehicleSpkPath + fullName;
         #ifdef DEBUG_FIND_KERNEL
            MessageInterface::ShowMessage(
                              "In FindKernel, NOW fullName1 (SPK1) = %s\n",
                              fullName1.c_str());
         #endif
         if (!(GmatFileUtil::DoesFileExist(fullName1)))
         {
            std::string planetarySpkPath =
                        fm->GetPathname(FileManager::PLANETARY_EPHEM_SPK_PATH);
            fullName1 = planetarySpkPath + fullName;
            #ifdef DEBUG_FIND_KERNEL
               MessageInterface::ShowMessage(
                                 "In FindKernel, NOW fullName1 (SPK2) = %s\n",
                                 fullName1.c_str());
            #endif
            if (!(GmatFileUtil::DoesFileExist(fullName1)))
            {
               fileFound = false;
            }
         }
         fullName = fullName1;  // last place looked
      }
      else // Otherwise, see if it is a relative path name
      {
         // Try to find it with the absolute path, by adding default
         // directory(ies)
         if (fullName.size() > 1 && fullName[0] == '.')
         {
            std::string fullName1;
            // Check for path relative to script (gmat working directory) first
            fullName1 = fm->GetGmatWorkingDirectory() +
                        fm->GetPathSeparator() + fullName;
            #ifdef DEBUG_FIND_KERNEL
               MessageInterface::ShowMessage("In FindKernel, fullName1 = %s\n",
                                             fullName1.c_str());
            #endif
            if (!(GmatFileUtil::DoesFileExist(fullName1)))
            {
               // Check for path relative to the current working directory
               fullName1 = fm->GetCurrentWorkingDirectory() +
                           fm->GetPathSeparator() + fullName;
               #ifdef DEBUG_FIND_KERNEL
                   MessageInterface::ShowMessage(
                                     "In FindKernel, NOW fullName1 = %s\n",
                                    fullName1.c_str());
               #endif
               if (!(GmatFileUtil::DoesFileExist(fullName1)))
                  fileFound = false;
               else
                  fullName = fullName1;
            }
            else
               fullName = fullName1;
         }
         else
         {
            fileFound = false;
         }
      }
   }
   if (fileFound)
      return fullName;
   else
      return FILE_NOT_FOUND;
}

//------------------------------------------------------------------------------
//  std::string GetFullKernelPath(const std::string &forKernel)

//------------------------------------------------------------------------------
/**
 * This method finds the full kernel path for the kernel with the given name, 
 * if it has been loaded.
 *
 * @return full kenel path name, if found; blank, otherwise
 *
 */
//------------------------------------------------------------------------------
std::string SpiceInterface::GetFullKernelPath(const std::string &forKernel)
{
   #ifdef DEBUG_SPK_FULL_PATH
      MessageInterface::ShowMessage(
                     "IsLoaded::Now attempting to find kernel name %s\n",
                     forKernel.c_str());
   #endif
   
   std::map<std::string, std::string>::iterator ii;
   for (ii = loadedKernels.begin(); ii != loadedKernels.end(); ++ii)
   {
      if ((*ii).first == forKernel)
      {
      #ifdef DEBUG_SPK_FULL_PATH
         MessageInterface::ShowMessage(
                  "IsLoaded::kernel name %s WAS INDEED ALREADY LOADED as %s\n",
                  forKernel.c_str(), ((*ii).second).c_str());
      #endif
         return (*ii).second;
      }
   }
   #ifdef DEBUG_SPK_FULL_PATH
      MessageInterface::ShowMessage(
                     "IsLoaded::kernel name %s NOT FOUND \n",
                     forKernel.c_str());
   #endif
   return "";
}

